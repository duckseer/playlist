// globals.h
#pragma once
#include <unordered_set>

#include "core/audio_file.h"
#include <vector>


extern std::vector<AudioFile *> all_audio_files;// 用于存储所有音频文件指针
extern std::vector<AudioFile *> show_audio_files;// 用于存储需要显示的文件指针
extern std::unordered_set<std::string> unique_paths;// 用于去重
