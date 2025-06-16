//
// Created by 冯宇飞 on 25-6-14.
//

#include "core/playlist.h"

#include <unordered_map>

#include "FL/fl_ask.H"

std::vector<AudioFile *> all_audio_files;// 用于存储所有音频文件指针
std::vector<AudioFile *> show_audio_files;// 用于存储需要显示的文件指针
std::unordered_map<std::string, AudioFile*> path_to_audio_file;  // 路径 → AudioFile* 映射

void insert_to_audio_files(const std::string& path, const bool is_selected = false) {
    try {
        // 获取规范化路径
        const std::string abs_path = fs::canonical(path).string();
        // 尝试插入或查找
        if (auto [it, inserted] = path_to_audio_file.try_emplace(abs_path, nullptr); inserted) {  // 新文件
            // 创建对象并更新映射
            auto* file = new AudioFile{fs::path(path), is_selected};
            all_audio_files.push_back(file);
            show_audio_files.push_back(file);
            it->second = file;  // 存储指针
        } else if (is_selected){  // 已存在且需要选中
            // 直接通过 map 获取指针
            it->second->is_selected = true;
        }
    }
    catch (const fs::filesystem_error& e) {
        fl_alert("路径错误: %s", e.what());
    }
}

