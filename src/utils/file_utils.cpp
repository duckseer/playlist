//
// Created by 冯宇飞 on 25-6-14.
//

#include "utils/file_utils.h"

#include <algorithm>

#include "ui/Callbacks.h"

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
