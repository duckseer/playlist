//
// Created by 冯宇飞 on 25-6-14.
//

#include "core/playlist.h"

#include "core/globals.h"
#include "ui/Window.h"

void insert_to_audio_files(const std::string& path, const bool is_selected = false) {
    // 尝试插入路径（检查是否重复）
    if (const std::string abs_path = fs::canonical(path).string(); unique_paths.insert(abs_path).second) {
        // 新文件 - 创建并添加
        auto* file = new AudioFile{fs::path(path), is_selected};
        all_audio_files.emplace_back(file);
        show_audio_files.emplace_back(file);
    } else {
        // 文件已存在 - 更新选中状态
        const auto it = std::ranges::find_if(all_audio_files,
                                       [&abs_path](const AudioFile* file) {
                                           return fs::canonical(file->path).string() == abs_path;
                                       });

        if (it != all_audio_files.end()) {
            (*it)->is_selected = is_selected;
        }
        reflash_audio_file_check_browser();
    }
}
