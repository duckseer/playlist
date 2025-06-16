//
// Created by 冯宇飞 on 25-6-14.
//

#include "ui/Window.h"

#include "core/globals.h"
#include "core/playlist.h"
#include "FL/Fl_Button.H"
#include "FL/Fl_Double_Window.H"
#include "ui/Callbacks.h"

Window::Window() {
    Fl_File_Icon *icon;
    main_window = new Fl_Double_Window(900, 800 + TERMINAL_HEIGHT, "选择一个文件夹,扫描其下的所有音频文件并根据选定文件生成播放列表");
    setlocale(LC_ALL, "");
    Fl::scheme(nullptr);
    Fl_File_Icon::load_system_icons();
    auto *left_group = new Fl_Group(0, 10, 500, 840); // 增加高度以容纳多选框
    left_group->begin(); {
        base_dir = new Fl_Input(60, 10, 300, 25, "根目录:");
        search_file = new Fl_Input(60, 40, 300, 25, "搜索:");
        search_file->callback(search_callback);
        // 创建多选框
        audio_file_check_browser = new Fl_Check_Browser(10, 90, 380, 700, "选择文件:");
        audio_file_check_browser->type(FL_MULTI_BROWSER);
        audio_file_check_browser->align(FL_ALIGN_TOP_LEFT);
        auto *button = new Fl_Button(360, 10, 25, 25);
        button->tooltip("选择要扫描的根目录..");
        button->callback(show_choose_base_dir);
        if ((icon = Fl_File_Icon::find(".", Fl_File_Icon::DIRECTORY))) {
            button->labelcolor(FL_YELLOW);
            icon->label(button);
        } else {
            button->label("..");
        }
        auto *to_file_list_button = new Fl_Button(400, 400, 25, 25, "->");
        to_file_list_button->callback(to_file_list_callback);
        auto *import_button = new Fl_Button(10, 800, 40, 25, "导入");
        import_button->callback(import_callback);
        auto *export_button = new Fl_Button(60, 800, 40, 25, "导出");
        export_button->callback(export_callback);
    }
    left_group->end();
    auto *right_group = new Fl_Group(400, 10, 500, 800);
    right_group->begin(); {
        replace_base_dir = new Fl_Input(500, 10, 300, 25, "替换根目录:");
        files = new Fl_File_Browser(450, 90, 380, 700);
        files->align(FL_ALIGN_LEFT);
        auto *button = new Fl_Button(800, 10, 25, 25);
        button->tooltip("选择要替换的根目录..");
        button->callback(show_choose_replace_base_dir);
        if ((icon = Fl_File_Icon::find(".", Fl_File_Icon::DIRECTORY))) {
            // Icon found; assign it..
            button->labelcolor(FL_YELLOW);
            icon->label(button);
        } else {
            // Fallback if no icon found
            button->label("..");
        }
    }
    right_group->end();
    main_window->resizable(files);
    main_window->resizable(left_group);
    main_window->resizable(right_group);
    main_window->end();
}

void Window::run(const int argc, char *argv[]) {
    window = new Window();
    int argn = 1;
    while (argn < argc) {
        if (Fl::arg(argc, argv, argn) == 0) break;
    }
    main_window->show(argc - argn, argv);
    Fl::run();
}

void reflash_audio_file_check_browser() {
    audio_file_check_browser->clear();
    for (const auto &file: show_audio_files) {
        audio_file_check_browser->add(file->path.filename().string().c_str(),file->is_selected);
    }
    audio_file_check_browser->redraw();
}
