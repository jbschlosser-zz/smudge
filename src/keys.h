#ifndef KEYS_H
#define KEYS_H

#include "mud_session.h"
#include "mud_ui.h"

typedef void (*key_binding)(mud_session *session, mud_ui *ui, int key);

//struct _table_entry;
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
void key_binding_unset(mud_session *session, mud_ui *ui, int key);
void key_binding_do_nothing(mud_session *session, mud_ui *ui, int key);
void key_binding_page_up(mud_session *session, mud_ui *ui, int key);
void key_binding_page_down(mud_session *session, mud_ui *ui, int key);
void key_binding_history_back(mud_session *session, mud_ui *ui, int key);
void key_binding_history_forward(mud_session *session, mud_ui *ui, int key);
void key_binding_history_forward_end(mud_session *session, mud_ui *ui, int key);
void key_binding_add_input_char(mud_session *session, mud_ui *ui, int key);
void key_binding_delete_input_char(mud_session *session, mud_ui *ui, int key);
void key_binding_backspace_input_char(mud_session *session, mud_ui *ui, int key);
void key_binding_input_cursor_left(mud_session *session, mud_ui *ui, int key);
void key_binding_input_cursor_right(mud_session *session, mud_ui *ui, int key);
void key_binding_search(mud_session *session, mud_ui *ui, int key);
void key_binding_submit_input(mud_session *session, mud_ui *ui, int key);

#endif
