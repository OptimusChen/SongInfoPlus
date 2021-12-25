#pragma once

#include "HMUI/ImageView.hpp"
#include "HMUI/ModalView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/UI/Button.hpp"

namespace SongInfoPlus {
class UIUtils {
 public:
  static std::unordered_map<std::string, TMPro::TextMeshProUGUI*>* MAP;
  static bool contains(std::string string);
  static void update(std::string string, std::string s);
  static void registerModal(std::string string, TMPro::TextMeshProUGUI* text);
  static void skewButton(UnityEngine::UI::Button* button, float skew);
  static void recolorButton(UnityEngine::UI::Button* button,
                            UnityEngine::Color color, HMUI::ImageView* ignore);
};
}  // namespace SongInfoPlus