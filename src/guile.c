#include "guile.h"

void init_guile(void)
{
    guile_current_session = NULL;

    // Set up the hooks.
    scm_c_define_gsubr("write-to-stderr", 1, 0, 0, &scheme_write_to_stderr);
    scm_c_define("send-command-hook", scm_make_hook(scm_from_int(1)));
    scm_c_define_gsubr("send-command", 1, 0, 0, &scheme_send_command);
    scm_c_define("config-filename", scm_from_locale_string("mud.scm")); // TODO: Make this configurable.
    scm_c_define_gsubr("reload-config", 0, 0, 0, &scheme_load_config_file);
    scm_c_define_gsubr("search-backwards", 1, 0, 0, &scheme_search_backwards);

    // Load up regex support.
    scm_c_use_module("ice-9 regex");

    // Load the config file.
    scheme_load_config_file();
}

void set_guile_current_session(session *sess)
{
    guile_current_session = sess;
}

SCM scheme_write_to_stderr(SCM output)
{
    char *str = scm_to_locale_string(output);
    fprintf(stderr, "%s", str);
    free(str);

    return SCM_UNSPECIFIED;
}

SCM scheme_send_command(SCM command)
{
    if(!guile_current_session) return SCM_BOOL_F;

    char *str = scm_to_locale_string(command);
    int result = mud_connection_send_command(guile_current_session->connection, str, strlen(str));
    free(str);

    return scm_from_int(result);
}

SCM scheme_load_config_file(void)
{
    scm_c_eval_string("(catch #t (lambda () (primitive-load config-filename)) (lambda (key . args) #t) (lambda (key . args) (display-backtrace (make-stack #t) (current-output-port))))");

    return SCM_UNSPECIFIED;
}

SCM scheme_search_backwards(SCM search_str)
{
    if(!guile_current_session) return SCM_BOOL_F;

    char *str = scm_to_locale_string(search_str);
    scrollback_search_backwards(
        guile_current_session->output_data,
        str,
        &guile_current_session->last_search_result);
    free(str);

    return SCM_UNSPECIFIED;
}
