/*
 * Copyright (C) 2014 Joel Schlosser All Rights Reserved.
 *
 * This file is part of smudge.
 *
 * smudge is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * smudge is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with smudge.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
