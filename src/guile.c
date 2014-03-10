#include "guile.h"

void init_guile(void)
{
    guile_current_session = NULL;

    // Set up the hooks.
    scm_c_define_gsubr("write-to-stderr", 1, 0, 0, &write_to_stderr);
    scm_c_define("send-command-hook", scm_make_hook(scm_from_int(1)));
    scm_c_define_gsubr("send-command", 1, 0, 0, &send_command);
    scm_c_define("config-filename", scm_from_locale_string("mud.scm"));
    scm_c_define_gsubr("reload-config", 0, 0, 0, &load_config_file);

    // Load up regex support.
    scm_c_use_module("ice-9 regex");

    // Load the config file.
    load_config_file();
}

SCM write_to_stderr(SCM output)
{
    char *str = scm_to_locale_string(output);
    fprintf(stderr, "%s", str);
    free(str);

    return SCM_UNSPECIFIED;
}

SCM send_command(SCM command)
{
    if(!guile_current_session) return SCM_BOOL_F;

    char *str = scm_to_locale_string(command);
    int result = mud_connection_send_command(guile_current_session->connection, str, strlen(str));
    free(str);

    return scm_from_int(result);
}

SCM load_config_file(void)
{
    scm_c_eval_string("(catch #t (lambda () (primitive-load config-filename)) (lambda (key . args) #t) (lambda (key . args) (display-backtrace (make-stack #t) (current-output-port))))");

    return SCM_UNSPECIFIED;
}

void set_guile_current_session(session *sess)
{
    guile_current_session = sess;
}
