#ifndef clipboard_h
#define clipboard_h
void fake_ctrl_c();
void get_clipboard(std::string &content);
void set_clipboard(std::string &content);
#endif