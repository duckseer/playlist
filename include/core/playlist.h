//
// Created by 冯宇飞 on 25-6-14.
//

#ifndef PLAYLIST_H
#define PLAYLIST_H
#include <string>
#include <unordered_map>
#include <vector>

#include "audio_file.h"

extern std::vector<AudioFile *> all_audio_files;// 用于存储所有音频文件指针
extern std::vector<AudioFile *> show_audio_files;// 用于存储需要显示的文件指针
extern std::unordered_map<std::string, AudioFile*> path_to_audio_file;// 用于去重


class playlist {

};

void insert_to_audio_files(const std::string & path,bool is_selected);


#endif //PLAYLIST_H
