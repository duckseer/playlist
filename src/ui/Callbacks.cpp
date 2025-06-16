//
// Created by 冯宇飞 on 25-6-14.
//

#include "ui/Callbacks.h"

#include <string>

#include "core/globals.h"
#include "ui/Window.h"
#include "FL/Fl_Native_File_Chooser.H"
#include "utils/file_utils.h"
#include <filesystem>
#include <fstream>

#include "core/playlist.h"
namespace fs = std::filesystem;

void show_choose_base_dir(Fl_Widget *, void *) {
    save_browser_selected_items();
    // 1. 弹出文件保存对话框
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("选择需要扫描的文件夹");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
    if (file_chooser.show() != 0) return; // 用户取消选择
    // 2. 获取用户选择的文件路径
    const char *filename = file_chooser.filename();
    base_dir->value(filename);
    scan_dir(filename,0);
    reflash_audio_file_check_browser();
}


void search_callback(Fl_Widget *, void *) {
    save_browser_selected_items();
    audio_file_check_browser->clear();
    show_audio_files.clear();
    const auto *input = search_file;
    const std::string search_term = input->value();
    int browser_index = 1; // FLTK索引从1开始
    for (auto file: all_audio_files) {
        if (file->contains(search_term)) {
            show_audio_files.push_back(file); // 或存储索引i
            std::string display_name = file->path.filename().string();
            audio_file_check_browser->add(display_name.c_str());
            if (file->is_selected) {
                audio_file_check_browser->set_checked(browser_index);
            }
            browser_index++;
        }
    }
    audio_file_check_browser->redraw();
}

void to_file_list_callback(Fl_Widget *, void *) {
    save_browser_selected_items();
    char relative[FL_PATH_MAX];
    files->clear();
    for (const auto &file: all_audio_files) {
        if (file->is_selected) {
            fl_filename_relative(relative, sizeof(relative), file->path.string().c_str());
            files->add(relative, Fl_File_Icon::find(file->path.string().c_str(), Fl_File_Icon::PLAIN));
        }
    }
    files->redraw();
}

void import_callback(Fl_Widget *, void *) {
    // 1. 弹出文件选择对话框
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("选择导入文件");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
    file_chooser.filter("M3U Playlist\t*.m3u");
    if (file_chooser.show() != 0) return;
    save_browser_selected_items();
    import_audio_files(file_chooser.filename());
}


void export_callback(Fl_Widget *widget, void *data) {
    // 1. 弹出文件保存对话框
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("选择导出路径");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    file_chooser.preset_file("default_playlist.m3u");
    file_chooser.filter("M3U Playlist\t*.m3u");
    if (file_chooser.show() != 0) return;
    export_audio_files(file_chooser.filename());

}

void save_browser_selected_items() {
    // 保存多选框选中的文件
    // FLTK多选框默认触发条件是FL_WHEN_NEVER,故默认不会自动触发回调,所以在需要的地方手动触发
    // 由于默认FLTK多选框索引从1开始,所以需要减1
    for (int i = 1; i <= audio_file_check_browser->nitems(); i++) {
        show_audio_files[i - 1]->is_selected = audio_file_check_browser->checked(i);
    }
}

void show_choose_replace_base_dir(Fl_Widget *, void *) {
    // 1. 弹出文件夹选择对话框
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("选择需要替换的文件夹");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
    if (file_chooser.show() != 0) return; // 用户取消选择
    // 2. 获取用户选择的文件路径
    const char *filename = file_chooser.filename();
    replace_base_dir->value(filename);
}