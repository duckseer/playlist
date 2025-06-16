//
// Created by 冯宇飞 on 25-6-14.
//

#ifndef FILE_UTILS_H
#define FILE_UTILS_H


#include <filesystem>

namespace fs = std::filesystem;

class file_utils {

};


void scan_dir(const fs::path &dir, int depth);

void import_audio_files(const char *filename);

void export_audio_files(const char *filename);

#endif //FILE_UTILS_H
