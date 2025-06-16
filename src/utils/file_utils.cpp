//
// Created by 冯宇飞 on 25-6-14.
//

#include "utils/file_utils.h"

#include <algorithm>
#include <cstring>
#include <fstream>

#include "core/playlist.h"
#include "FL/fl_ask.H"
#include "ui/Callbacks.h"
#include "ui/Window.h"

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
                        insert_to_audio_files(path.generic_string(),false);
                    }
                }
            }
        }
    } catch (const fs::filesystem_error &e) {
        printf("[ERROR] %s\n", e.what());
    }
}

void import_audio_files(const char *filename) {
    auto filepath = fs::path(reinterpret_cast<const char8_t*>(filename));
    // 3. 读取文件内容
    if (std::ifstream infile(filepath); infile.is_open()) {
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
        reflash_audio_file_check_browser();
        search_callback(nullptr, nullptr);
        to_file_list_callback(nullptr, nullptr);
        infile.close();
        fl_alert("导入成功: %s", filename);
    } else {
        fl_alert("打开失败: %s (错误: %s)", filename, std::strerror(errno));
    }
}

void export_audio_files(const char *filename) {
    auto filepath = fs::path(reinterpret_cast<const char8_t*>(filename));
    // 3. 写入文件
    if (std::ofstream outfile(filepath); outfile.is_open()) {
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