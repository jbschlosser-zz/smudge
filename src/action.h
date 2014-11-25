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

#ifndef ACTION_H
#define ACTION_H

#include "user_interface.h"

struct _session;

// An action that can be performed on the state of a MUD session.
// This struct acts as a base class, with each type of specific
// action "deriving" from it by embedding it at their beginning.
// TODO: Potentially remove the user interface, since it shouldn't
// be updated from within actions anyway.
typedef struct _action {
    void (*perform)(struct _action *act, struct _session *sess, user_interface *ui); // Function for performing the specific action.
                                                                             // This function is set by the derived action.
    void (*destroy)(struct _action *act); // Function for destroying the action. This function is set by the derived action.
} action;

// The default destructor for an action just frees it. Note that the
// default cannot be used for actions that own dynamically-allocated
// memory.
void action_default_destroy(action *act);

// Convenience macros for declaring/defining actions with no additional state.
#define DECLARE_ACTION(name) \
    typedef struct {\
        action base;\
    } name;\
    name *name##_create(void);\
    void name##_destroy(action *act);\
    void name##_perform(action *act, struct _session *sess, user_interface *ui);

#define DEFINE_ACTION(name, sess, ui) \
    name *name##_create(void)\
    {\
        name *act = malloc(sizeof(name));\
        act->base.perform = name##_perform;\
        act->base.destroy = action_default_destroy;\
        return act;\
    }\
    void name##_perform(action* _action_, struct _session *sess, user_interface *ui)

// Convenience macros for declaring/defining actions with additional state. This
// is a bit more complex since each action can own dynamically-allocated memory.
// Because of this, definition for such actions has been broken into three parts:
// constructing its state, destroying its state, and performing the action itself.
#define DECLARE_ACTION_1(name, state_var) \
    typedef struct {\
        action base;\
        state_var;\
    } name;\
    name *name##_create(state_var);\
    void name##_destroy(action *act);\
    void name##_perform(action *act, struct _session *sess, user_interface *ui);

#define DEFINE_ACTION_1_STATE_CREATE(name, derived, state, ...)\
    name *name##_create(state)\
    {\
        name *derived = malloc(sizeof(name));\
        derived->base.perform = name##_perform;\
        derived->base.destroy = name##_destroy;\
        __VA_ARGS__\
        return derived;\
    }

#define DEFINE_ACTION_1_STATE_DESTROY(name, derived, ...)\
    void name##_destroy(action *act)\
    {\
        if(!act) return;\
        name *derived = (name*)act;\
        (void)derived;\
        __VA_ARGS__\
        free(act);\
    }

#define DEFINE_ACTION_1_PERFORM(name, derived, sess, ui, ...)\
    void name##_perform(action *act, struct _session *sess, user_interface *ui)\
    {\
        name *derived = (name*)act;\
        __VA_ARGS__\
    }

// Actions.
DECLARE_ACTION(do_nothing_action);
DECLARE_ACTION(page_up_action);
DECLARE_ACTION(page_down_action);
DECLARE_ACTION(history_back_action);
DECLARE_ACTION(history_forward_action);
DECLARE_ACTION(history_forward_end_action);
DECLARE_ACTION(delete_input_char_action);
DECLARE_ACTION(backspace_input_char_action);
DECLARE_ACTION(clear_input_line_action);
DECLARE_ACTION(input_cursor_left_action);
DECLARE_ACTION(input_cursor_right_action);
DECLARE_ACTION(search_backwards_from_input_line_action);
DECLARE_ACTION(submit_from_input_line_action);

DECLARE_ACTION_1(search_backwards_action, color_string* search_str);
DECLARE_ACTION_1(send_command_action, char* command_str);
DECLARE_ACTION_1(add_input_char_action, color_char char_to_add);
DECLARE_ACTION_1(add_to_history_action, color_string* entry);
DECLARE_ACTION_1(write_output_line_action, color_string *line);
DECLARE_ACTION_1(unset_key_binding_action, int keycode)
DECLARE_ACTION_1(submit_input_action, color_string* input_str);

#endif
