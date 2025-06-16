//
// Created by 冯宇飞 on 25-6-14.
//

#include "ui/Callbacks.h"

#include <cstring>
#include <fstream>
#include <string>

#include "core/globals.h"
#include "ui/Window.h"
#include "FL/Fl_Native_File_Chooser.H"
#include "utils/file_utils.h"

void show_choose_base_dir(Fl_Widget *, void *) {
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
    // 2. 获取用户选择的文件路径
    const char *filename = file_chooser.filename();
    // 3. 读取文件内容
    if (std::ifstream infile(filename); infile.is_open()) {
        std::string line;
        while (std::getline(infile, line)) {
            if (line.empty()) continue;
            // 统一转换为UNIX风格路径（使用正斜杠）
            std::string path = fs::path(line).lexically_normal().generic_string();
            fs::path base_path = fs::path(base_dir->value()).lexically_normal().generic_string();
            fs::path replace_path = fs::path(replace_base_dir->value()).lexically_normal().generic_string();
            // 如果需要替换路径前缀
            if (!replace_path.empty() && !base_path.empty()) {
                if (size_t pos = path.find(replace_path.string()); pos != std::string::npos) {
                    path.replace(pos, replace_path.string().length(), base_path.string());
                }
            }
            // 检查文件是否存在
            if (fs::exists(path)) {
                insert_to_audio_files(path, true);
            }
        }
        search_callback(nullptr, nullptr);
        to_file_list_callback(nullptr, nullptr);
        infile.close();
        fl_alert("导入成功: %s", filename);
    } else {
        fl_alert("打开失败: %s (错误: %s)", filename, std::strerror(errno));
    }
}


void export_callback(Fl_Widget *widget, void *data) {
    // 1. 弹出文件保存对话框
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("选择导出路径");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    file_chooser.preset_file("default_playlist.m3u");
    file_chooser.filter("M3U Playlist\t*.m3u");
    if (file_chooser.show() != 0) return;
    // 2. 获取用户选择的文件路径
    const char *filename = file_chooser.filename();
    // 3. 写入文件
    if (std::ofstream outfile(filename); outfile.is_open()) {
        save_browser_selected_items();
        // 统一转换为UNIX风格路径（使用正斜杠）
        fs::path base_path = fs::path(base_dir->value()).lexically_normal().generic_string();
        fs::path replace_path = fs::path(replace_base_dir->value()).lexically_normal().generic_string();
        bool need_replace = !replace_path.empty();
        for (const auto &file: all_audio_files) {
            if (file->is_selected) {
                // 统一当前路径的分隔符
                std::string path = fs::path(file->path).lexically_normal().generic_string();
                // 替换路径前缀（现在两边都是正斜杠）
                if (need_replace) {
                    if (size_t pos = path.find(base_path.string()); pos != std::string::npos) {
                        path.replace(pos, base_path.string().length(), replace_path.string());
                    }
                }
                outfile << path << "\n";
            }
        }
        outfile.close();
        fl_alert("导出成功: %s", filename);
    } else {
        fl_alert("导出失败！无法创建文件");
    }
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