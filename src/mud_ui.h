#ifndef MUD_UI_H
#define MUD_UI_H

#include "history_buffer.h"
#include "mud_char.h"
#include "mud_window.h"
#include "scrollback_buffer.h"

typedef struct {
    mud_window *_output_window;
    scrollback_buffer *_output_buffer;
    int _output_scrollback;
    mud_window *_input_line;
    char *_input_buffer;
    int _input_index;
    int _max_input_size;
    history_buffer *_history;
    int _history_index;
} mud_ui;

// Constructor/destructor.
mud_ui *mud_ui_create(int output_buffer_size, int input_buffer_size, int history_size);
void mud_ui_destroy(mud_ui *ui);

// Member functions.
int mud_ui_get_char(mud_ui *ui);
void mud_ui_resize(mud_ui *ui, int newlines, int newcols);
void mud_ui_write_formatted_output(mud_ui *ui, mud_char_t *output, int len);
void mud_ui_write_unformatted_output(mud_ui *ui, const char *output, int len);
void mud_ui_history_back(mud_ui *ui);
void mud_ui_history_forward(mud_ui *ui);
void mud_ui_history_forward_end(mud_ui *ui);
void mud_ui_page_up(mud_ui *ui);
void mud_ui_page_down(mud_ui *ui);
void mud_ui_input_add_char(mud_ui *ui, char ch);
void mud_ui_input_delete_char(mud_ui *ui);
char *mud_ui_submit_input(mud_ui *ui);

#endif
