#ifndef APPLICATION_H
#define APPLICATION_H

#include "mud_session.h"
#include "mud_ui.h"

void application_add_input_char(mud_session *session, mud_ui *ui, mud_char_t ch);
void application_delete_input_char(mud_session *session, mud_ui *ui);
void application_submit_input(mud_session *session, mud_ui *ui);
void application_write_output(mud_session *session, mud_ui *ui, mud_char_t *output, int len);
void application_history_back(mud_session *session, mud_ui *ui);
void application_history_forward(mud_session *session, mud_ui *ui);
void application_history_forward_end(mud_session *session, mud_ui *ui);
void application_history_add(mud_session *session, mud_ui *ui, mud_string *entry);
void application_page_up(mud_session *session, mud_ui *ui);
void application_page_down(mud_session *session, mud_ui *ui);
void application_resize(mud_session *session, mud_ui *ui, int lines, int cols);
void application_search_backwards(mud_session *session, mud_ui *ui, const char *search_str);

#endif
