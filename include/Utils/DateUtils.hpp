#pragma once

#include "TMPro/TextMeshProUGUI.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

namespace SongInfoPlus {
namespace DateUtils {
std::string getMonthFromString(std::string s);
std::string getNumberEnd(std::string number);
std::string getReadableDate(std::string s);
}  // namespace DateUtils
}  // namespace SongInfoPlus