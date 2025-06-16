//
// Created by 冯宇飞 on 25-6-14.
//

#ifndef PLAYLIST_H
#define PLAYLIST_H
#include <string>
#include <unordered_set>
#include <vector>

#include "audio_file.h"

inline std::vector<AudioFile *> all_audio_files;// 用于存储所有音频文件指针
inline std::vector<AudioFile *> show_audio_files;// 用于存储需要显示的文件指针
inline std::unordered_set<std::string> unique_paths;// 用于去重


class playlist {

};

void insert_to_audio_files(const std::string & path,bool is_selected);


#endif //PLAYLIST_H
