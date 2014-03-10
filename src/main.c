#include <libguile.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include "keys.h"
#include "session.h"
#include "user_interface.h"

#define RECV_BUFFER_MAX_SIZE 24576

static SCM write_to_stderr(SCM output)
{
    char *str = scm_to_locale_string(output);
    fprintf(stderr, "%s", str);
    free(str);

    return SCM_UNSPECIFIED;
}

static session *main_session = NULL;
static SCM send_command(SCM command)
{
    char *str = scm_to_locale_string(command);
    int result = mud_connection_send_command(main_session->connection, str, strlen(str));
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

    // SET UP THE SESSION.
    main_session = session_create(
        mud_connection_create(REAL_SOCKET_OPS),
        scrollback_create(line_buffer_create(10000)),
        history_create(line_buffer_create(100)),
        input_line_create());

    // CONNECT TO THE MUD SERVER.
    const char *host = argv[1];
    const char *port = argv[2];
    bool connected = mud_connection_connect(main_session->connection, host, port);
    if(!connected) {
        printf("Error: Could not make a connection to '%s' on port '%s'.\n", host, port);
        exit(1);
    }

    // SET UP THE USER INTERFACE.
    // Start ncurses.
    init_ncurses();

    // Create the UI.
    user_interface *ui = user_interface_create(REAL_WINDOW_OPS, REAL_WINDOW_OPS, 0, 0, LINES, COLS);

    // CREATE THE KEYBINDINGS.
    key_binding_table *bindings = key_binding_table_create();

    // MAIN LOOP.
    int input_keycode = 0x0;
    color_char received_data[RECV_BUFFER_MAX_SIZE];
    while(1) {
        // Handle a resize if necessary.
        if(RESIZE_OCCURRED) {
            // Restart ncurses.
            endwin();
            refresh();
            clear();

            // Resize things.
            user_interface_resize(ui, LINES, COLS);

            // Refresh the UI.
            // TODO: Change this to use the public interface.
            main_session->output_data->_dirty = true;
            main_session->input_data->_dirty = true;

            // We're done here.
            RESIZE_OCCURRED = 0;
        }

        // GET DATA FROM THE SERVER.
        int received = mud_connection_receive(main_session->connection, received_data, RECV_BUFFER_MAX_SIZE);
        bool connected = mud_connection_connected(main_session->connection);
        if(received < 0 || !connected) {
            // TODO: Handle this case!
            break;
        }
        if(received > 0) {
            // Add the data to scrollback.
            scrollback_write(main_session->output_data, received_data, received);
        }

        // REFRESH THE UI.
        if(scrollback_is_dirty(main_session->output_data)) {
            user_interface_refresh_output_window(ui, main_session->output_data);
            scrollback_clear_dirty(main_session->output_data);
            syslog(LOG_INFO, "Scrollback has been refreshed");
        }
        if(input_line_is_dirty(main_session->input_data)) {
            user_interface_refresh_input_line_window(ui, main_session->input_data);
            input_line_clear_dirty(main_session->input_data);
            syslog(LOG_INFO, "Input line has been refreshed");
        }

        // GET INPUT FROM THE USER.
        // Perform the function bound to the key.
        input_keycode = user_interface_get_char(ui);
        if(input_keycode == ERR) {
            usleep(10000);
        } else {
            key_binding binding = key_binding_table_get_binding(bindings, input_keycode);
            if(binding)
                binding(main_session, ui, input_keycode);
            else
                key_binding_unset(main_session, ui, input_keycode);
        }
    }

    end_ncurses();
    key_binding_table_destroy(bindings);
    session_destroy(main_session);
    user_interface_destroy(ui);
}

int main(int argc, char **argv)
{
    scm_boot_guile(argc, argv, &main_with_guile, NULL);

    // Never reached.
    return 0;
}
