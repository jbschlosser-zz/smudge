#include <libguile.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include "mud_connection.h"
#include "mud_ui.h"

static SCM write_to_stderr(SCM output)
{
    char *str = scm_to_locale_string(output);
    fprintf(stderr, "%s", str);
    free(str);

    return SCM_UNSPECIFIED;
}

static mud_connection *connection = NULL;
static SCM send_command(SCM command)
{
    char *str = scm_to_locale_string(command);
    int result = mud_connection_send_command(connection, str, strlen(str));
    free(str);

    return scm_from_int(result);
}

static SCM reload_config(void)
{
    scm_c_eval_string("(catch #t (lambda () (primitive-load config-filename)) (lambda (key . args) #t) (lambda (key . args) (display-backtrace (make-stack #t) (current-output-port))))");

    return SCM_UNSPECIFIED;
}

void init_guile(void)
{
    // Set up debugging.
    //SCM_DEVAL_P = 1;
    //SCM_BACKTRACE_P = 1;
    //SCM_RECORD_POSITIONS_P = 1;
    //SCM_RESET_DEBUG_MODE;

    // Set up the hooks.
    scm_c_define_gsubr("write-to-stderr", 1, 0, 0, &write_to_stderr);
    scm_c_define("send-command-hook", scm_make_hook(scm_from_int(1)));
    scm_c_define_gsubr("send-command", 1, 0, 0, &send_command);
    scm_c_define("config-filename", scm_from_locale_string("mud.scm"));
    scm_c_define_gsubr("reload-config", 0, 0, 0, &reload_config);

    // Load up regex support.
    scm_c_use_module("ice-9 regex");

    // Load the config file.
    reload_config();
    /*scm_c_eval_string("(define (lazy-catch-handler key . args)\
            (let ((stack (make-stack #t lazy-catch-handler)))\
             (display-backtrace stack (current-output-port))\
             (scm-error key \"?\" \"?\" args)\
             )\
                )\
              (catch #t (lambda () (lazy-catch #t\
                             (lambda () (primitive-load config-filename))\
                                           lazy-catch-handler))\
               (lambda (key . args) #t))");*/
    //scm_c_eval_string("(catch #t (lambda () (primitive-load config-filename)) (lambda (key . args) #t) (lambda (key . args) (display-backtrace (make-stack #t) (current-output-port))))");
    //scm_c_primitive_load("mud.scm");
}

void init_ncurses(void)
{
    initscr(); // Start ncurses mode.
    cbreak(); // Pass characters directly to the ui without waiting for the return key to be pressed.
    noecho(); // Don't echo characters as they are typed.
    start_color(); // Start colors.
    use_default_colors(); // Allows default colors (i.e. the usual color of the terminal foreground or background).
    init_color_pairs(); // Set up the color pairs to be used throughout.
}

void end_ncurses(void)
{
    endwin(); // End ncurses mode.
}

int RESIZE_OCCURRED = 0;
void signal_handler(int signum)
{
    if(signum == SIGWINCH)
        RESIZE_OCCURRED = 1;
}

void main_with_guile(void *data, int argc, char **argv)
{
    if(argc != 3 && data == NULL) {
        printf("Usage: mud <host> <port>\n");
        exit(1);
    }

    // SET UP SIGNAL HANDLERS.
    // Handle the window resize signal.
    struct sigaction action;
    memset(&action, 0x00, sizeof(action));
    action.sa_handler = signal_handler;
    sigaction(SIGWINCH, &action, NULL);

    // Purposely ignore SIGPIPE. When socket errors occur, they should
    // be checked for and handled at the location they occur.
    signal(SIGPIPE, SIG_IGN);

    // OPEN THE SYSLOG.
    openlog("mud", 0x00, LOG_USER);

    // SET UP GUILE.
    init_guile();

    // CONNECT TO THE MUD SERVER.
    const char *host = argv[1];
    const char *port = argv[2];
    connection = mud_connection_create(REAL_SOCKET_OPS);
    bool connected = mud_connection_connect(connection, host, port);
    if(!connected) {
        printf("Error: Could not make a connection to '%s' on port '%s'.\n", host, port);
        exit(1);
    }

    // SET UP THE USER INTERFACE.
    // Start ncurses.
    init_ncurses();

    // Create the UI.
    mud_ui *ui = mud_ui_create(
        1000, // Output buffer size.
        1000, // Input buffer size.
        100); // History size.

    // MAIN LOOP.
    int input_keycode = 0x0;
    mud_char_t received_data[8192];
    while(input_keycode != 65) {
        // Handle a resize if necessary.
        if(RESIZE_OCCURRED) {
            // Restart ncurses.
            endwin();
            refresh();
            clear();

            // Resize the application.
            mud_ui_resize(ui, LINES, COLS);

            // We're done here.
            RESIZE_OCCURRED = 0;
        }

        // Check for incoming data from the server.
        int received = mud_connection_receive(connection, received_data, 8192);
        bool connected = mud_connection_connected(connection);
        if(received < 0 || !connected) {
            // TODO: Handle this case!
            break;
        }
        if(received > 0) {
            // Write the received data, keeping the scrollback locked, if necessary.
            // TODO: Fix this so that it works when the scrollback buffer is full.
            // Currently, in this case, no adjustment will be made to lock scrollback
            // in the right place. Maybe write formatted output could return the number
            // of lines written?
            int lines_before = mud_ui_scrollback_avail(ui);
            mud_ui_write_formatted_output(ui, received_data, received);
            int lines_after = mud_ui_scrollback_avail(ui);
            if(mud_ui_get_scrollback(ui) > 0)
                mud_ui_adjust_scrollback(ui, lines_after - lines_before);
        }

        // Attempt to get a char from the user.
        input_keycode = mud_ui_get_char(ui);
        if(input_keycode == ERR) {
            usleep(10000);
            continue;
        }

        // Handle the keypress.
        if(input_keycode == 0xA) {
            int lines_before = mud_ui_scrollback_avail(ui);
            char *input = mud_ui_submit_input(ui);
            int lines_after = mud_ui_scrollback_avail(ui);

            // Keep scrollback locked for this line, if necessary.
            if(mud_ui_get_scrollback(ui) > 0)
                mud_ui_adjust_scrollback(ui, lines_after - lines_before);

            // Run the hook.
            SCM symbol = scm_c_lookup("send-command-hook");
            SCM send_command_hook = scm_variable_ref(symbol);
            if(scm_is_true(scm_hook_p(send_command_hook)) && scm_is_false(scm_hook_empty_p(send_command_hook))) {
                scm_c_run_hook(send_command_hook, scm_list_1(scm_from_locale_string(input)));
            } else {
                // Send the command to the server.
                int send_result = mud_connection_send_command(connection, input, strlen(input));
                if(send_result < 0) {
                    // TODO: Handle this case!
                    break;
                }
            }

            free(input);
            continue;
        }
        if(input_keycode == 0xD)
            continue;
        if(input_keycode == KEY_RESIZE)
            continue;
        if(input_keycode == 27) {
            mud_ui_history_forward_end(ui);
            continue;
        }
        if(input_keycode == KEY_UP) {
            mud_ui_history_back(ui);
            continue;
        }
        if(input_keycode == KEY_DOWN) {
            mud_ui_history_forward(ui);
            continue;
        }
        if(input_keycode == KEY_PPAGE) {
            mud_ui_page_up(ui);
            continue;
        }
        if(input_keycode == KEY_NPAGE) {
            mud_ui_page_down(ui);
            continue;
        }
        if(input_keycode == KEY_BACKSPACE || input_keycode == 127) {
            mud_ui_input_delete_char(ui);
            continue;
        }

        char letter = input_keycode & 0xFF;
        mud_ui_input_add_char(ui, letter);
    }

    end_ncurses();
    mud_ui_destroy(ui);
}

int main(int argc, char **argv)
{
    scm_boot_guile(argc, argv, &main_with_guile, NULL);

    // Never reached.
    return 0;
}
