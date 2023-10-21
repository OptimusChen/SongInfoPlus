#include "include/Utils/UIUtils.hpp"

#include "HMUI/ImageView.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/GradientColorKey.hpp"
#include "UnityEngine/Gradient.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "beatsaber-hook/shared/utils/typedefs-array.hpp"
#include "System/Activator.hpp"

#include <iomanip>
#include <sstream>
#include <ctime>

using namespace QuestUI;
using namespace UnityEngine;

namespace UIUtils {

    void SkewButton(UI::Button* button, float skew) {
        ArrayW<HMUI::ImageView*> images = button->GetComponentsInChildren<HMUI::ImageView*>();

        for (int i = 0; i < images.size(); i++) {
            HMUI::ImageView* view = images->get(i);
            view->skew = skew;
        }
    }

    void RecolorButton(UI::Button* button, Color color, std::unordered_set<HMUI::ImageView*> ignored) {
        ArrayW<HMUI::ImageView*> images = button->GetComponentsInChildren<HMUI::ImageView*>();

        for (int i = 0; i < images.size(); i++) {
            HMUI::ImageView* view = images->get(i);

            if (ignored.find(view) != ignored.end()) continue;

            view->set_color(color);
            view->set_color0(Color(0, 0, 1, 0.4));
            view->set_color1(Color(0, 0, 1, 0.4));
        }
    }

    GameObject* CreateLoadingIndicator(Transform* parent) {
        auto original = ArrayUtil::First(Resources::FindObjectsOfTypeAll<GameObject*>(), [](auto el) { return to_utf8(csstrtostr(el->get_name())) == "LoadingIndicator"; });
        auto loadingIndicator = Object::Instantiate(original, parent, false);
        loadingIndicator->set_name("SongInfoPlusLoadingIndicator");

        loadingIndicator->AddComponent<UI::LayoutElement*>();
        return loadingIndicator;
    }

    std::string getReadableDate(StringW unreadableDate) {
        StringW date = unreadableDate->Split('T').get(0);

        const std::string months[] = {"January", "February", "March", "April", "May", "June",
                                      "July", "August", "September", "October", "November", "December"};
        const std::string daySuffixes[] = {"th", "st", "nd", "rd", "th", "th", "th", "th", "th", "th"};

        // Parse the date
        int year, month, day;
        sscanf(static_cast<std::string>(date).c_str(), "%d-%d-%d", &year, &month, &day);

        // Handle day suffixes
        std::string daySuffix;
        if (day >= 11 && day <= 13) {
            daySuffix = "th";
        } else {
            daySuffix = daySuffixes[day % 10];
        }

        // Construct the formatted date
        std::string formattedDate = months[month - 1] + " " + std::to_string(day) + daySuffix + ", " + std::to_string(year);

        return formattedDate;
    }

    Color GetColor(float percent) {
        Gradient* gradient = Gradient::New_ctor();

        gradient->set_colorKeys(ArrayW<GradientColorKey>({
            GradientColorKey(Color::get_black(), 0),
            GradientColorKey(Color(1, 0.188f, 0.188f, 1), 0.33f),
            GradientColorKey(Color::get_yellow(), .66f),
            GradientColorKey(Color(0.388f, 1, 0.388f, 1), 0.95f),
            GradientColorKey(Color::get_green(), 1)
        }));

        return gradient->Evaluate(percent);
    }
}