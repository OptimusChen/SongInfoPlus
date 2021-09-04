#include "main.hpp"

#include <stdio.h>

#include <iostream>
#include <string>

#include "GlobalNameSpace/PreviewBeatmapLevelSO.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSegmentedControlController.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapDifficultySegmentedControlController.hpp"
#include "GlobalNamespace/BeatmapLevelDataExtensions.hpp"
#include "GlobalNamespace/BeatmapLevelDataLoaderSO.hpp"
#include "GlobalNamespace/BeatmapLevelLoader.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/HapticFeedbackController.hpp"
#include "GlobalNamespace/IBeatmapDataAssetFileModel.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevelData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/LevelCollectionTableView.hpp"
#include "GlobalNamespace/LoadingControl.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "HMUI/CurvedCanvasSettings.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/ModalView.hpp"
#include "HMUI/ScrollView.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/Touchable.hpp"
#include "HMUI/ViewController.hpp"
#include "System/Collections/IEnumerator.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/register.hpp"
#include "custom-types/shared/types.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreen.hpp"
#include "questui/shared/QuestUI.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "songdownloader/shared/Exceptions.hpp"

#define UTILS_FUNCTIONS_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <ctime>
#include <string>
#include <string_view>
#include <vector>

#include "custom-types/shared/register.hpp"

using namespace custom_types;

using namespace std;
using namespace HMUI;
using namespace GlobalNamespace;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace UnityEngine;
using namespace BeatSaver;
using namespace BeatSaver::API;
using namespace BeastSaber;
using namespace ScoreSaber;
using namespace il2cpp;
using namespace il2cpp::utils;

static ModInfo modInfo;  // Stores the ID and version of our mod, and is sent to
                         // the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration &getConfig() {
  static Configuration config(modInfo);
  config.Load();
  return config;
}

// Returns a logger, useful for printing debug messages
Logger &getLogger() {
  static Logger *logger = new Logger(modInfo);
  return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo &info) {
  info.id = ID;
  info.version = VERSION;
  modInfo = info;

  getConfig().Load();  // Load the config file
  getLogger().info("Completed setup!");
}

custom_types::Helpers::Coroutine coroutine() {
  for (int i = 0; i < 30; i++) {
    // Timer
    co_yield reinterpret_cast<System::Collections::IEnumerator *>(
        CRASH_UNLESS(WaitForSeconds::New_ctor(1)));
  }

  co_return;
}

auto getSongIDfromDifficulty(GlobalNamespace::IDifficultyBeatmap *beatmap) {
  auto levelID = il2cpp_utils::RunMethod<Il2CppString *>(beatmap->get_level(),
                                                         "get_levelID")
                     .value();
  return levelID;
}

std::string getMonthFromString(std::string s) {
  if (s.starts_with("01")) {
    return "January";
  } else if (s.starts_with("02")) {
    return "Febuary";
  } else if (s.starts_with("03")) {
    return "March";
  } else if (s.starts_with("04")) {
    return "April";
  } else if (s.starts_with("05")) {
    return "May";
  } else if (s.starts_with("06")) {
    return "June";
  } else if (s.starts_with("07")) {
    return "July";
  } else if (s.starts_with("08")) {
    return "August";
  } else if (s.starts_with("09")) {
    return "September";
  } else if (s.starts_with("10")) {
    return "October";
  } else if (s.starts_with("11")) {
    return "November";
  } else if (s.starts_with("12")) {
    return "December";
  }
  return "Month";
}

std::string getNumberEnd(std::string number) {
  if (number.ends_with("0") || number.ends_with("4") || number.ends_with("5") ||
      number.ends_with("6") || number.ends_with("7") || number.ends_with("8") ||
      number.ends_with("9"))
    return "th";
  if (number.ends_with("1")) return "st";
  if (number.ends_with("2")) return "nd";
  if (number.ends_with("3")) return "rd";
  return "th";
}

std::string getReadableDate(std::string s) {
  Il2CppString *s2 = il2cpp_utils::newcsstr(s);
  ::Array<::Il2CppString *> *array = s2->Split('-');

  Il2CppString *day = array->get(2)->Split('T')->get(0);
  Il2CppString *year = array->get(0);
  Il2CppString *month = array->get(1);

  std::string day2 = to_utf8(csstrtostr(day));

  if (day2.starts_with("0")) {
    day2.substr(1);
  }

  return getMonthFromString(to_utf8(csstrtostr(month))) + " " + day2 +
         getNumberEnd(to_utf8(csstrtostr(day))) + ", " +
         to_utf8(csstrtostr(year));
}

std::string id = "";
bool loadedButton = false;

MAKE_HOOK_MATCH(
    StandardLevelDetailViewController_DidActivate,
    &GlobalNamespace::StandardLevelDetailViewController::DidActivate, void,
    GlobalNamespace::StandardLevelDetailViewController *self,
    bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling) {
  StandardLevelDetailViewController_DidActivate(
      self, firstActivation, addedToHeirarchy, screenSystemEnabling);

  /* Find beat map */
  auto difficulty = self->get_selectedDifficultyBeatmap();

  if (!difficulty) return;

  /* Get custom level songs */
  std::string hash = to_utf8(csstrtostr(getSongIDfromDifficulty(difficulty)));

  if (!hash.starts_with("custom_level_")) return;

  hash = hash.substr(strlen("custom_level_"));

  /* Turn the names of the songs to upper case */
  for (auto &c : hash) c = toupper(c);

  id = hash;

  auto beatmap = BeatSaver::API::GetBeatmapByHash(hash);

  if (!loadedButton) {
    if (!beatmap) {
      return;
    }

    HMUI::ModalView *mapInfo = BeatSaberUI::CreateModal(
        self->get_transform(), UnityEngine::Vector2(120.0f, 80.0f),
        [](HMUI::ModalView *modal) {}, true);
    UnityEngine::GameObject *mapInfom =
        BeatSaberUI::CreateScrollableModalContainer(mapInfo);

    HMUI::ModalView *beatsaverInfo = BeatSaberUI::CreateModal(
        self->get_transform(), UnityEngine::Vector2(120.0f, 80.0f),
        [](HMUI::ModalView *modal) {}, true);
    UnityEngine::GameObject *beatsaverInfom =
        BeatSaberUI::CreateScrollableModalContainer(beatsaverInfo);

    HMUI::ModalView *hashInfo = BeatSaberUI::CreateModal(
        self->get_transform(), UnityEngine::Vector2(120.0f, 80.0f),
        [](HMUI::ModalView *modal) {}, true);
    UnityEngine::GameObject *hashInfom =
        BeatSaberUI::CreateScrollableModalContainer(hashInfo);

    HMUI::ModalView *descInfo = BeatSaberUI::CreateModal(
        self->get_transform(), UnityEngine::Vector2(120.0f, 80.0f),
        [](HMUI::ModalView *modal) {}, true);
    UnityEngine::GameObject *descInfom =
        BeatSaberUI::CreateScrollableModalContainer(descInfo);

    auto descriptionText = BeatSaberUI::CreateText(
        descInfom->get_transform(), beatmap.value().GetDescription(), false,
        UnityEngine::Vector2(0.0f, 0.0f), UnityEngine::Vector2(5.0f, 5.0f));

    HMUI::ModalView *artInfo = BeatSaberUI::CreateModal(
        self->get_transform(), UnityEngine::Vector2(120.0f, 80.0f),
        [](HMUI::ModalView *modal) {}, true);
    UnityEngine::GameObject *artInfom =
        BeatSaberUI::CreateScrollableModalContainer(artInfo);

    auto levelBarTransform = self->get_transform()
                                 ->Find(il2cpp_utils::newcsstr("LevelDetail"))
                                 ->Find(il2cpp_utils::newcsstr("LevelBarBig"));
    if (!levelBarTransform) return;

    auto imageTransform =
        levelBarTransform->Find(il2cpp_utils::newcsstr("SongArtwork"))
            ->GetComponent<UnityEngine::RectTransform *>();

    auto imageView = imageTransform->GetComponent<HMUI::ImageView *>();

    auto artwork = BeatSaberUI::CreateImage(
        artInfom->get_transform(), imageView->get_sprite(),
        UnityEngine::Vector2(0.0f, 0.0f), UnityEngine::Vector2(120.0f, 80.0f));

    auto key =
        BeatSaberUI::CreateText(mapInfom->get_transform(), "Key : 0", false,
                                Vector2(200.0f, 80.0f), Vector2(5.0f, 5.0f));
    auto author = BeatSaberUI::CreateText(
        mapInfom->get_transform(), "Author : unknown", false,
        Vector2(200.0f, 80.0f), Vector2(5.0f, 5.0f));
    auto mapper = BeatSaberUI::CreateText(
        mapInfom->get_transform(), "Mapper : unknown", false,
        Vector2(180.0f, 80.0f), Vector2(5.0f, 5.0f));
    auto uploaded = BeatSaberUI::CreateText(
        mapInfom->get_transform(), "Uploaded : never", false,
        Vector2(180.0f, 80.0f), Vector2(5.0f, 5.0f));
    auto downloads = BeatSaberUI::CreateText(
        mapInfom->get_transform(), "Downloads : 0", false,
        Vector2(180.0f, 80.0f), Vector2(5.0f, 5.0f));

    auto okButton = BeatSaberUI::CreateUIButton(
        mapInfom->get_transform(), "OK", "PlayButton",
        [mapInfo]() { mapInfo->Hide(true, nullptr); });

    auto beatsaver =
        BeatSaberUI::CreateUIButton(mapInfom->get_transform(), "BEATSAVER",
                                    [mapInfo, self, beatsaverInfo]() {
                                      mapInfo->Hide(true, nullptr);
                                      beatsaverInfo->Show(true, true, nullptr);
                                    });
    auto hash = BeatSaberUI::CreateUIButton(
        mapInfom->get_transform(), "LEVEL HASH", [mapInfo, self, hashInfo]() {
          mapInfo->Hide(true, nullptr);
          hashInfo->Show(true, true, nullptr);
        });
    auto desc = BeatSaberUI::CreateUIButton(
        mapInfom->get_transform(), "DESCRIPTION",
        [mapInfo, self, descInfo, descriptionText]() {
          mapInfo->Hide(true, nullptr);
          descInfo->Show(true, true, nullptr);
          std::string hash = to_utf8(csstrtostr(
              getSongIDfromDifficulty(self->get_selectedDifficultyBeatmap())));

          if (!hash.starts_with("custom_level_")) {
            return;
          }

          hash = hash.substr(13);

          int i = 0;
          char c;
          while (hash[i]) {
            c = hash[i];
            putchar(toupper(c));
            i++;
          }

          auto m = BeatSaver::API::GetBeatmapByHash(hash);

          descriptionText->SetText(
              il2cpp_utils::newcsstr(m.value().GetDescription()));
        });

    auto art = BeatSaberUI::CreateUIButton(
        mapInfom->get_transform(), "VIEW ARTWORK",
        [mapInfo, self, artInfom, artInfo, artwork]() {
          mapInfo->Hide(true, nullptr);
          artInfo->Show(true, true, nullptr);
          auto levelBarTransform =
              self->get_transform()
                  ->Find(il2cpp_utils::newcsstr("LevelDetail"))
                  ->Find(il2cpp_utils::newcsstr("LevelBarBig"));
          if (!levelBarTransform) return;

          auto imageTransform =
              levelBarTransform->Find(il2cpp_utils::newcsstr("SongArtwork"))
                  ->GetComponent<UnityEngine::RectTransform *>();

          auto imageView = imageTransform->GetComponent<HMUI::ImageView *>();

          artwork->set_sprite(imageView->get_sprite());
        });

    auto button = QuestUI::BeatSaberUI::CreateUIButton(
        self->get_transform(), "", UnityEngine::Vector2(-27.65f, 20.15f),
        UnityEngine::Vector2(15.1f, 17.0f),
        [mapInfo, mapInfom, self, key, author, mapper, uploaded, downloads,
         difficulty]() {
          mapInfo->Show(true, true, nullptr);

          std::string hash = to_utf8(csstrtostr(
              getSongIDfromDifficulty(self->get_selectedDifficultyBeatmap())));

          if (!hash.starts_with("custom_level_")) {
            return;
          }

          hash = hash.substr(13);

          int i = 0;
          char c;
          while (hash[i]) {
            c = hash[i];
            putchar(toupper(c));
            i++;
          }

          auto m = BeatSaver::API::GetBeatmapByHash(hash);

          key->SetText(il2cpp_utils::newcsstr("Key : " + m.value().GetId()));
          author->SetText(il2cpp_utils::newcsstr(
              "Author : " + m.value().GetMetadata().GetSongAuthorName()));
          mapper->SetText(il2cpp_utils::newcsstr(
              "Mapper : " + m.value().GetMetadata().GetLevelAuthorName()));
          uploaded->SetText(il2cpp_utils::newcsstr(
              "Uploaded : " + getReadableDate(m.value().GetUploaded())));
          downloads->SetText(il2cpp_utils::newcsstr(
              "Downloads : " +
              std::to_string(m.value().GetStats().GetDownloads())));
        });

    auto components = button->GetComponentsInChildren<HMUI::ImageView *>();

    for (int i = 0; i < components->get_Length(); i++) {
      components->get(i)->skew = 0.0f;
      components->get(i)->set_preserveAspect(false);
      components->get(i)->set_color(Color(0.0f, 0.0f, 0.0f, 0));
    }

    loadedButton = true;
  }

  auto levelBarTransform = self->get_transform()
                               ->Find(il2cpp_utils::newcsstr("LevelDetail"))
                               ->Find(il2cpp_utils::newcsstr("LevelBarBig"));
  if (!levelBarTransform) return;

  auto imageTransform =
      levelBarTransform->Find(il2cpp_utils::newcsstr("SongArtwork"))
          ->GetComponent<UnityEngine::RectTransform *>();
  imageTransform->set_sizeDelta(UnityEngine::Vector2(15.0f, 15.0f));
  imageTransform->set_localPosition(UnityEngine::Vector3(-35.0f, -15.0f, 3.0f));
  imageTransform->SetAsFirstSibling();

  auto imageView = imageTransform->GetComponent<HMUI::ImageView *>();
  imageView->set_preserveAspect(false);
  imageView->skew = 0.0f;
}

void DidActivate(HMUI::ViewController *self, bool firstActivation,
                 bool addedToHierarchy, bool screenSystemEnabling) {}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
  il2cpp_functions::Init();
  QuestUI::Init();
  QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);

  getLogger().info("Installing hooks...");
  INSTALL_HOOK(getLogger(), StandardLevelDetailViewController_DidActivate);
  getLogger().info("Installed all hooks!");

  custom_types::Register::AutoRegister();
}