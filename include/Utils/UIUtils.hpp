#pragma once

#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/Color.hpp"
#include "HMUI/ImageView.hpp"
#include "UnityEngine/Transform.hpp"

using namespace UnityEngine;

namespace UIUtils {
    void SkewButton(UI::Button* button, float skew);
    void RecolorButton(UI::Button* button, Color color, std::unordered_set<HMUI::ImageView*> ignored);
    GameObject* CreateLoadingIndicator(Transform* parent);
    std::string getReadableDate(StringW unreadableDate);
    Color GetColor(float percent);
};