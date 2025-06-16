//
// Created by 冯宇飞 on 25-6-14.
//

#ifndef AUDIO_FILE_H
#define AUDIO_FILE_H
#include <string>
#include <filesystem>

class AudioFile;
namespace fs = std::filesystem;

class AudioFile {
public:
    fs::path path;
    std::string path_str;
    mutable bool is_selected = false;

    explicit AudioFile(fs::path p, bool selected = false);

    /// @brief 检查路径是否包含指定字符串
    /// @param substr 要搜索的子串
    [[nodiscard]] bool contains(const std::string& substr) const;

};



#endif //AUDIO_FILE_H
