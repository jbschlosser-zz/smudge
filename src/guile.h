#ifndef GUILE_H
#define GUILE_H

#include <libguile.h>
#include "session.h"

void init_guile(void);

// Functions to export to Scheme.
SCM write_to_stderr(SCM output);
SCM send_command(SCM command);
SCM load_config_file(void);

// Global session on which to operate. It must be global.
session *guile_current_session;
void set_guile_current_session(session *sess);

#endif
