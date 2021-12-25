#include "Utils/UIUtils.hpp"

#include "HMUI/ImageView.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "questui/shared/QuestUI.hpp"
#define UTILS_FUNCTIONS_H

using namespace UnityEngine;

std::unordered_map<std::string, TMPro::TextMeshProUGUI*>*
    SongInfoPlus::UIUtils::MAP =
        new std::unordered_map<std::string, TMPro::TextMeshProUGUI*>();

bool SongInfoPlus::UIUtils::contains(std::string string) {
  return MAP->contains(string);
}

void SongInfoPlus::UIUtils::registerModal(std::string string,
                                          TMPro::TextMeshProUGUI* text) {
  if (MAP->contains(string)) {
    MAP->erase(string);
  }
  MAP->insert({string, text});
}

void SongInfoPlus::UIUtils::update(std::string string, std::string text) {
  MAP->at(string)->SetText(il2cpp_utils::newcsstr(text));
}

void SongInfoPlus::UIUtils::recolorButton(UnityEngine::UI::Button* button,
                                          UnityEngine::Color color,
                                          HMUI::ImageView* ignore) {
  ::Array<HMUI::ImageView*>* buttonComponents =
      button->GetComponentsInChildren<HMUI::ImageView*>();

  for (int i = 0; i < buttonComponents->get_Length(); i++) {
    HMUI::ImageView* view = buttonComponents->get(i);
    if (view != ignore) view->set_color(color);
  }
}

void SongInfoPlus::UIUtils::skewButton(UnityEngine::UI::Button* button,
                                       float skew) {
  ::Array<HMUI::ImageView*>* buttonComponents =
      button->GetComponentsInChildren<HMUI::ImageView*>();

  for (int i = 0; i < buttonComponents->get_Length(); i++) {
    HMUI::ImageView* view = buttonComponents->get(i);
    view->skew = skew;
  }
}