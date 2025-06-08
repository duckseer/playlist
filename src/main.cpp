//
// Created by 冯宇飞 on 25-6-1.
//


#include <algorithm>
#include <clocale>

#include "FL/Fl_check_Browser.H"
#include "FL/Fl_Double_Window.H"
#include "FL/Fl_Terminal.H"
#include <FL/Fl_Native_File_Chooser.H> // 文件选择对话框
#include <sys/stat.h>  // 用于获取文件大小
#include <vector>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;


#define TERMINAL_HEIGHT 120

struct AudioFile {
    fs::path path;
    std::string path_str;
    mutable bool is_selected = false;

    explicit AudioFile(fs::path p, bool selected = false)
        : path(std::move(p)), path_str(path.string()), is_selected(selected) {
    }

    [[nodiscard]] bool contains(const std::string &substr) const {
        return path_str.find(substr) != std::string::npos;
    }
};

Fl_File_Browser *files;
Fl_Input *base_dir;
Fl_Input *replace_base_dir;
Fl_Input *search_file;
Fl_Shared_Image *image = nullptr;
Fl_Check_Browser *audio_file_check_browser; // 多选框控件指针
std::vector<AudioFile *> all_audio_files;
std::vector<AudioFile *> show_audio_files;

void show_choose_base_dir(Fl_Widget *, void *);

void search_callback(Fl_Widget *, void *);

void save_browser_selected_items();

void to_file_list_callback(Fl_Widget *, void *);

void export_callback(Fl_Widget *, void *);

void show_choose_replace_base_dir(Fl_Widget *, void *);

int main(const int argc, char *argv[]) {
    Fl_File_Icon *icon;
    auto *window = new Fl_Double_Window(900, 800 + TERMINAL_HEIGHT, "选择一个文件夹,扫描其下的所有音频文件并根据选定文件生成播放列表");
    setlocale(LC_ALL, "");
    Fl::scheme(nullptr);
    Fl_File_Icon::load_system_icons();
    auto *left_group = new Fl_Group(0, 10, 500, 840); // 增加高度以容纳多选框
    left_group->begin(); {
        int argn = 1;
        while (argn < argc) {
            if (Fl::arg(argc, argv, argn) == 0) break;
        }
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
            // Icon found; assign it..
            button->labelcolor(FL_YELLOW);
            icon->label(button);
        } else {
            // Fallback if no icon found
            button->label("..");
        }

        auto *to_file_list_button = new Fl_Button(400, 400, 25, 25, "->");
        to_file_list_button->callback(to_file_list_callback);
        auto *export_button = new Fl_Button(10, 800, 40, 25, "导出");
        export_button->callback(export_callback);
    }
    left_group->end();
    auto *right_group = new Fl_Group(400, 10, 500, 800);
    right_group->begin(); {
        int argn = 1;
        while (argn < argc) {
            if (Fl::arg(argc, argv, argn) == 0) break;
        }
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
    window->resizable(files);
    window->resizable(left_group);
    window->resizable(right_group);
    window->end();
    window->show(1, argv);

    Fl::run();
    return 0;
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

void save_browser_selected_items() {
    // 保存多选框选中的文件
    // FLTK多选框默认触发条件是FL_WHEN_NEVER,故默认不会自动触发回调,所以在需要的地方手动触发
    // 由于默认FLTK多选框索引从1开始,所以需要减1
    for (int i = 1; i <= audio_file_check_browser->nitems(); i++) {
        show_audio_files[i - 1]->is_selected = audio_file_check_browser->checked(i);
    }
}

void scan_dir(const fs::path &dir, const int depth = 0) {
    if (constexpr int MAX_DEPTH = 100; depth >= MAX_DEPTH) {
        return;
    }
    try {
        for (const auto &entry: fs::directory_iterator(dir)) {
            if (entry.is_directory()) {
                scan_dir(entry.path(), depth + 1);
            } else if (entry.is_regular_file()) {
                const auto &path = entry.path();
                // 检查文件大小和扩展名
                if (entry.file_size() >= 1024 * 1024) {
                    // 获取文件扩展名并转换为小写
                    std::string ext = path.extension().string();
                    std::ranges::transform(ext, ext.begin(), ::tolower);
                    // 检查是否是音频文件扩展名
                    if (ext == ".mp3" || ext == ".wav" || ext == ".flac" ||
                        ext == ".aac" || ext == ".ogg" || ext == ".m4a") {
                        auto *file = new AudioFile{fs::path(path), false};
                        all_audio_files.emplace_back(file);
                        show_audio_files.emplace_back(file);
                    }
                }
            }
        }
    } catch (const fs::filesystem_error &e) {
        printf("[ERROR] %s\n", e.what());
    }
}

void show_choose_base_dir(Fl_Widget *, void *) {
    // 1. 弹出文件保存对话框
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("选择需要扫描的文件夹");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
    if (file_chooser.show() != 0) return; // 用户取消选择
    // 2. 获取用户选择的文件路径
    const char *filename = file_chooser.filename();
    all_audio_files.clear();
    base_dir->value(filename);
    scan_dir(filename);
    for (const auto &file: all_audio_files) {
        audio_file_check_browser->add(file->path.filename().string().c_str());
    }
    audio_file_check_browser->redraw();
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
