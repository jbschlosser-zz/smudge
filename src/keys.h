#ifndef KEYS_H
#define KEYS_H

#include "session.h"
#include "user_interface.h"

typedef void (*key_binding)(session *session, user_interface *ui, int key);

typedef struct _table_entry {
    int keycode;
    key_binding binding;
    struct _table_entry *next;
} _key_binding_table_entry;

typedef struct {
    int _buckets;
    _key_binding_table_entry **_bindings;
} key_binding_table;

// Construction/destruction.
key_binding_table *key_binding_table_create(void);
void key_binding_table_set_binding(key_binding_table *kb, int keycode, key_binding binding);
key_binding key_binding_table_get_binding(key_binding_table *kb, int keycode);
void key_binding_table_destroy(key_binding_table *kb);

// Key binding functions.
void key_binding_unset(session *session, user_interface *ui, int key);
void key_binding_do_nothing(session *session, user_interface *ui, int key);
void key_binding_page_up(session *session, user_interface *ui, int key);
void key_binding_page_down(session *session, user_interface *ui, int key);
void key_binding_history_back(session *session, user_interface *ui, int key);
void key_binding_history_forward(session *session, user_interface *ui, int key);
void key_binding_history_forward_end(session *session, user_interface *ui, int key);
void key_binding_add_input_char(session *session, user_interface *ui, int key);
void key_binding_delete_input_char(session *session, user_interface *ui, int key);
void key_binding_backspace_input_char(session *session, user_interface *ui, int key);
void key_binding_input_cursor_left(session *session, user_interface *ui, int key);
void key_binding_input_cursor_right(session *session, user_interface *ui, int key);
void key_binding_search(session *session, user_interface *ui, int key);
void key_binding_submit_input(session *session, user_interface *ui, int key);

#endif
