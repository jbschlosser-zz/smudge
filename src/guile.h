#ifndef GUILE_H
#define GUILE_H

#include <libguile.h>
#include "session.h"

// Set up the Guile Scheme interpreter.
void init_guile(void);

// Global session on which to operate. It must be global so that
// it can be accessed by the functions exported to scheme.
session *guile_current_session;
void set_guile_current_session(session *sess);

// Functions to export to Scheme.
SCM scheme_write_to_stderr(SCM output);
SCM scheme_send_command(SCM command);
SCM scheme_load_config_file(void);
SCM scheme_search_backwards(SCM search_str);

#endif
