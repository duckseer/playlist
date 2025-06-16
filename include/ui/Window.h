//
// Created by 冯宇飞 on 25-6-14.
//

#ifndef WINDOW_H
#define WINDOW_H

#define TERMINAL_HEIGHT 120
#include "FL/Fl_Check_Browser.H"
#include "FL/Fl_Double_Window.H"
#include "FL/Fl_File_Browser.H"
#include "FL/Fl_Input.H"

class Window;
inline Fl_Double_Window *main_window;
inline Fl_File_Browser *files;
inline Fl_Input *base_dir;
inline Fl_Input *replace_base_dir;
inline Fl_Input *search_file;
inline Fl_Shared_Image *image;
inline Fl_Check_Browser *audio_file_check_browser; // 多选框控件指针
inline Window* window;

class Window {
    explicit Window();
public:
    static void run(int argc, char *argv[]) ;
};

void reflash_audio_file_check_browser();

#endif //WINDOW_H
