//
// Created by 冯宇飞 on 25-6-14.
//

#include "core/audio_file.h"

#include <filesystem>
namespace fs = std::filesystem;

#include <string>

AudioFile::AudioFile(fs::path p, const bool selected)
    : path(std::move(p)),
      path_str(path.string()),
      is_selected(selected) {}

[[nodiscard]] bool AudioFile::contains(const std::string& substr) const {
    return path_str.find(substr) != std::string::npos;
}
