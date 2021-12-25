#include "GlobalNamespace/StandardLevelDetailView.hpp"

#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/ReleaseInfoViewController.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/TextPageScrollView.hpp"
#include "System/Action.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/Collider2D.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/Button_ButtonClickedEvent.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "Utils/DateUtils.hpp"
#include "Utils/UIUtils.hpp"
#include "codegen.hpp"
#include "main.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "questui/shared/QuestUI.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "songloader/shared/API.hpp"

#define PathToSprite(path) (QuestUI::BeatSaberUI::FileToSprite(path))
#define MakeDelegate(DelegateType, varName) \
  (il2cpp_utils::MakeDelegate<DelegateType>(classof(DelegateType), varName))

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace GlobalNamespace;
using namespace QuestUI;
using namespace BeatSaver;
using namespace HMUI;

GlobalNamespace::StandardLevelDetailViewController* viewController = nullptr;
bool firstActivate = false;
UnityEngine::UI::Button* button = nullptr;
std::string levelHash = "";
GlobalNamespace::IPreviewBeatmapLevel* level = nullptr;
HMUI::ImageView* imageView = nullptr;
HMUI::ModalView* modal = nullptr;

TMPro::TextMeshProUGUI* Text(Transform* transform, std::string string,
                             Vector2 pos, Vector2 sizeDelta) {
  return BeatSaberUI::CreateText(transform, string, false, pos, sizeDelta);
}

std::string removeDecimals(std::string s) {
  for (int i = 0; i < 7; i++) {
    s.pop_back();
  }
  return s;
}

std::string ToString(Il2CppString* s) { return to_utf8(csstrtostr(s)); }

HMUI::ModalView* createModalView(Transform* parent, Vector2 size,
                                 std::string text, Vector2 textloc,
                                 bool center) {
  HMUI::ModalView* modal = BeatSaberUI::CreateModal(
      parent, size, [](HMUI::ModalView* modal) {}, true);
  if (center) {
    TMPro::TextMeshProUGUI* textMesh =
        BeatSaberUI::CreateText(modal->get_transform(), text, false, textloc);
    textMesh->set_alignment(TMPro::TextAlignmentOptions::Center);
  } else {
    TMPro::TextMeshProUGUI* textMesh =
        BeatSaberUI::CreateText(modal->get_transform(), text, false, textloc);
  }
  modal->Show(true, true, nullptr);
  return modal;
}

MAKE_HOOK_MATCH(StandardLevelDetailView_RefreshContent,
                &GlobalNamespace::StandardLevelDetailView::RefreshContent, void,
                GlobalNamespace::StandardLevelDetailView* self) {
  StandardLevelDetailView_RefreshContent(self);
  level = reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(
      self->get_selectedDifficultyBeatmap()->get_level());
  Il2CppString* csHash = level->get_levelID();
  levelHash = to_utf8(csstrtostr(csHash));
  if (!levelHash.starts_with("custom_level_")) return;
  levelHash = levelHash.substr(13);

  if (viewController) {
    UnityEngine::Transform* transform = viewController->get_transform();
    auto levelBarTransform =
        transform->Find(il2cpp_utils::newcsstr("LevelDetail"))
            ->Find(il2cpp_utils::newcsstr("LevelBarBig"));
    if (!levelBarTransform) return;
    auto* imageTransform =
        levelBarTransform->Find(il2cpp_utils::newcsstr("SongArtwork"))
            ->GetComponent<RectTransform*>();
    imageView = imageTransform->GetComponent<HMUI::ImageView*>();

    auto playButtonTransform =
        viewController->standardLevelDetailView->actionButton->get_transform();
    auto rectTransform =
        viewController->standardLevelDetailView->actionButton->get_transform()
            ->GetParent()
            ->GetParent()
            ->GetParent()
            ->get_gameObject()
            ->GetComponent<RectTransform*>();
    rectTransform->set_localScale(UnityEngine::Vector3{1, 2, 1});
    playButtonTransform->GetParent()->GetParent()->set_localScale(
        Vector3{1, 0.5f, 1});
    playButtonTransform->GetParent()->GetParent()->set_localPosition(
        Vector3{0, 13.7f, 0});

    std::function<void()> click = [&]() {
      modal = BeatSaberUI::CreateModal(
          viewController->get_transform(), UnityEngine::Vector2(120.0f, 60.0f),
          [](HMUI::ModalView* modal) {}, true);
      modal->Show(true, true, nullptr);
      Transform* mtransform = modal->get_transform();
      SongInfoPlus::UIUtils::registerModal(
          "key",
          Text(mtransform, "Key: Loading...", {-7.0f, 9.5f}, {5.0f, 5.0f}));
      SongInfoPlus::UIUtils::registerModal(
          "author",
          Text(mtransform, "Author: Loading...", {-7.0f, 4.5f}, {5.0f, 5.0f}));
      SongInfoPlus::UIUtils::registerModal(
          "mapper",
          Text(mtransform, "Mapper: Loading...", {-7.0f, -1.5f}, {5.0f, 5.0f}));
      SongInfoPlus::UIUtils::registerModal(
          "uploaded", Text(mtransform, "Uploaded: Loading...", {-7.0f, -6.5f},
                           {5.0f, 5.0f}));
      SongInfoPlus::UIUtils::registerModal(
          "downloads", Text(mtransform, "Downloads: Loading...",
                            {-7.0f, -12.5f}, {5.0f, 5.0f}));
      SongInfoPlus::UIUtils::registerModal(
          "rating",
          Text(mtransform, "Loading...", {46.0f, 3.0f}, {5.0f, 5.0f}));
      SongInfoPlus::UIUtils::registerModal(
          "percent",
          Text(mtransform, "Loading...", {46.0f, -21.0f}, {5.0f, 5.0f}));

      UI::Button* exit = BeatSaberUI::CreateUIButton(
          mtransform, "OK", "PlayButton", UnityEngine::Vector2(2.5f, -22.5f),
          UnityEngine::Vector2(30.0f, 12.0f),
          [=]() { modal->Hide(true, nullptr); });
      SongInfoPlus::UIUtils::skewButton(exit, 0.0f);

      UI::Button* beatsaver = BeatSaberUI::CreateUIButton(
          mtransform, "BEATSAVER", Vector2(-40.0f, 22.5f),
          Vector2(30.0f, 12.0f), nullptr);
      SongInfoPlus::UIUtils::skewButton(beatsaver, 0.0f);

      UI::Button* hash = BeatSaberUI::CreateUIButton(
          mtransform, "LEVEL HASH", Vector2(-40.0f, 7.5f),
          Vector2(30.0f, 12.0f), nullptr);
      SongInfoPlus::UIUtils::skewButton(hash, 0.0f);

      UI::Button* desc = BeatSaberUI::CreateUIButton(
          mtransform, "DESCRIPTION", Vector2(-40.0f, -7.5f),
          Vector2(30.0f, 12.0f), nullptr);
      SongInfoPlus::UIUtils::skewButton(desc, 0.0f);

      UI::Button* art = BeatSaberUI::CreateUIButton(
          mtransform, "VIEW ARTWORK", Vector2(-40.0f, -22.5f),
          Vector2(30.0f, 12.0f), nullptr);
      SongInfoPlus::UIUtils::skewButton(art, 0.0f);

      std::string icons =
          "/sdcard/ModData/com.beatgames.beatsaber/Mods/SongInfoPlus/"
          "Icons/";
      std::string arrow_down = icons + "arrow_down.png";
      std::string arrow_up = icons + "arrow.png";

      TMPro::TextMeshProUGUI* logo =
          BeatSaberUI::CreateText(mtransform, "Song Info Plus", false,
                                  UnityEngine::Vector2(15.0f, 22.5f));
      logo->set_fontSize(10.0f);

      BeatSaberUI::CreateText(mtransform, "Vote", false,
                              UnityEngine::Vector2(47.0f, 18.0f),
                              UnityEngine::Vector2(10.0f, 10.0f));

      BeatSaberUI::CreateText(mtransform, "Rating", false,
                              UnityEngine::Vector2(45.0f, -18.0f),
                              UnityEngine::Vector2(10.0f, 10.0f));

      HMUI::ImageView* arrow_up_view =
          BeatSaberUI::CreateImage(mtransform, PathToSprite(arrow_up),
                                   UnityEngine::Vector2(45.0f, 12.5f),
                                   UnityEngine::Vector2(10.0f, 10.0f));
      arrow_up_view->set_color(Color(0.0f, 1.0f, 0.0f, 1.0f));

      HMUI::ImageView* arrow_down_view =
          BeatSaberUI::CreateImage(mtransform, PathToSprite(arrow_down),
                                   UnityEngine::Vector2(45.0f, -6.5f),
                                   UnityEngine::Vector2(10.0f, 10.0f));
      arrow_down_view->set_color(Color(1.0f, 0.0f, 0.0f, 1.0f));

      BeatSaberUI::AddHoverHint(arrow_up_view->get_gameObject(),
                                "Voting is Disabled on the Quest!");

      BeatSaberUI::AddHoverHint(arrow_down_view->get_gameObject(),
                                "Voting is Disabled on the Quest!");

      BeatSaver::API::GetBeatmapByHashAsync(
          levelHash, [=](std::__ndk1::optional<BeatSaver::Beatmap> optional) {
            if (optional.has_value()) {
              MainThreadScheduler::Schedule([=] {
                BeatSaver::Beatmap beatmap = optional.value();
                int upvotes = beatmap.GetStats().GetUpVotes();
                int downvotes = beatmap.GetStats().GetDownVotes();
                int rating = upvotes - downvotes;
                std::string percent = removeDecimals(
                    std::to_string(round(beatmap.GetStats().GetScore() * 100)));

                SongInfoPlus::UIUtils::update("key", "Key: " + beatmap.GetId());
                SongInfoPlus::UIUtils::update(
                    "author",
                    "Author: " + ToString(level->get_songAuthorName()));
                SongInfoPlus::UIUtils::update(
                    "mapper",
                    "Mapper: " + ToString(level->get_levelAuthorName()));
                SongInfoPlus::UIUtils::update(
                    "uploaded",
                    "Uploaded: " + SongInfoPlus::DateUtils::getReadableDate(
                                       beatmap.GetUploaded()));
                SongInfoPlus::UIUtils::update(
                    "downloads",
                    "Downloads: " +
                        std::to_string(beatmap.GetStats().GetDownloads()));
                SongInfoPlus::UIUtils::update("rating", std::to_string(rating));
                SongInfoPlus::UIUtils::update("percent", percent + "%");

                beatsaver->get_onClick()->AddListener(MakeDelegate(
                    Events::UnityAction*, std::function([=]() {
                      HMUI::ModalView* linkModal = createModalView(
                          viewController->get_transform(),
                          Vector2(100.0f, 20.0f),
                          "Link: https://beatsaver.com/maps/" + beatmap.GetId(),
                          Vector2(0.0f, 3.0f), true);
                      modal->Hide(true, nullptr);
                      linkModal->add_blockerClickedEvent(
                          MakeDelegate(System::Action*, std::function([=]() {
                                         linkModal->Hide(true, nullptr);
                                         modal->Show(true, true, nullptr);
                                       })));
                      Button* exit = BeatSaberUI::CreateUIButton(
                          linkModal->get_transform(), "OK", "PlayButton",
                          {0.0f, -5.0f}, {30.0f, 8.0f}, [=] {
                            linkModal->Hide(true, nullptr);
                            modal->Show(true, true, nullptr);
                          });
                    })));

                hash->get_onClick()->AddListener(MakeDelegate(
                    Events::UnityAction*, std::function([=]() {
                      HMUI::ModalView* hashModal =
                          createModalView(viewController->get_transform(),
                                          Vector2(100.0f, 20.0f), levelHash,
                                          Vector2(0.0f, 3.0f), true);
                      modal->Hide(true, nullptr);
                      hashModal->add_blockerClickedEvent(
                          MakeDelegate(System::Action*, std::function([=]() {
                                         hashModal->Hide(true, nullptr);
                                         modal->Show(true, true, nullptr);
                                       })));
                      Button* exit = BeatSaberUI::CreateUIButton(
                          hashModal->get_transform(), "OK", "PlayButton",
                          {0.0f, -5.0f}, {30.0f, 8.0f}, [=] {
                            hashModal->Hide(true, nullptr);
                            modal->Show(true, true, nullptr);
                          });
                    })));

                desc->get_onClick()->AddListener(MakeDelegate(
                    Events::UnityAction*, std::function([=]() {
                      HMUI::ModalView* descModal =
                          createModalView(viewController->get_transform(),
                                          Vector2(80.0f, 65.0f), "",
                                          Vector2(0.0f, 0.0f), false);
                      modal->Hide(true, nullptr);
                      descModal->add_blockerClickedEvent(
                          MakeDelegate(System::Action*, std::function([=]() {
                                         descModal->Hide(true, nullptr);
                                         modal->Show(true, true, nullptr);
                                       })));

                      GameObject* scrollable =
                          BeatSaberUI::CreateScrollableModalContainer(
                              descModal);
                      scrollable->GetComponent<ExternalComponents*>()
                          ->Get<RectTransform*>()
                          ->set_sizeDelta({8.0f, 1.0f});
                      HorizontalLayoutGroup* horizon =
                          BeatSaberUI::CreateHorizontalLayoutGroup(
                              scrollable->get_transform());
                      VerticalLayoutGroup* layout =
                          BeatSaberUI::CreateVerticalLayoutGroup(
                              horizon->get_transform());

                      TextMeshProUGUI* tmproText = BeatSaberUI::CreateText(
                          layout->get_transform(), beatmap.GetDescription(),
                          false);
                      tmproText->set_alignment(
                          TextAlignmentOptions::MidlineLeft);
                      tmproText->set_enableWordWrapping(true);

                      LayoutElement* noticelayout =
                          tmproText->get_gameObject()
                              ->AddComponent<LayoutElement*>();
                      layout->set_childControlHeight(true);
                      layout->set_childForceExpandHeight(true);
                      layout->set_childControlWidth(true);
                      layout->set_childForceExpandWidth(true);
                      horizon->set_childControlWidth(true);
                      horizon->set_childForceExpandWidth(true);
                      noticelayout->set_preferredWidth(65.0f);

                      UI::Button* exitDesc = BeatSaberUI::CreateUIButton(
                          scrollable->get_transform(), "OK", "PlayButton",
                          {0.0f, 0.0f}, {30.0f, 8.0f}, [=]() {
                            descModal->Hide(true, nullptr);
                            modal->Show(true, true, nullptr);
                          });
                      SongInfoPlus::UIUtils::skewButton(exitDesc, 0.0f);
                    })));

                art->get_onClick()->AddListener(MakeDelegate(
                    Events::UnityAction*, std::function([=]() {
                      modal->Hide(true, nullptr);
                      HMUI::ModalView* artModal = createModalView(
                          viewController->get_transform(),
                          Vector2(63.0f, 72.0f), "", Vector2(0.0f, 0.0f), true);
                      artModal->add_blockerClickedEvent(
                          MakeDelegate(System::Action*, std::function([=]() {
                                         artModal->Hide(true, nullptr);
                                         modal->Show(true, true, nullptr);
                                       })));
                      BeatSaberUI::CreateImage(
                          artModal->get_transform(), imageView->get_sprite(),
                          Vector2(0.0f, 5.0f), Vector2(56.0f, 56.0f))
                          ->set_preserveAspect(true);
                      UI::Button* exitArt = BeatSaberUI::CreateUIButton(
                          artModal->get_transform(), "OK", "PlayButton",
                          UnityEngine::Vector2(0.0f, -28.0f),
                          UnityEngine::Vector2(30.0f, 8.0f), [=]() {
                            artModal->Hide(true, nullptr);
                            modal->Show(true, true, nullptr);
                          });
                      SongInfoPlus::UIUtils::skewButton(exitArt, 0.0f);
                    })));
              });
            }
          });
    };

    if (imageView->skew != 0.0f) {
      if (!button) {
        button = QuestUI::BeatSaberUI::CreateUIButton(
            viewController->get_transform(), "",
            UnityEngine::Vector2(-26.65f, 10.2f),
            UnityEngine::Vector2(15.1f, 9.4f), click);
        button->set_name(il2cpp_utils::newcsstr("DetailsButton"));
        SongInfoPlus::UIUtils::skewButton(button, 0.3f);
        SongInfoPlus::UIUtils::recolorButton(
            button, Color(0.0f, 0.0f, 0.0f, 0.0f), imageView);
      }
    } else {
      if (!button) {
        button = QuestUI::BeatSaberUI::CreateUIButton(
            viewController->get_transform(), "",
            UnityEngine::Vector2(-25.5f, 10.2f),
            UnityEngine::Vector2(15.1f, 9.4f), click);
        button->set_name(il2cpp_utils::newcsstr("DetailsButton"));

        BeatSaberUI::CreateText(button->get_transform(), "Info", true)
            ->set_alignment(TMPro::TextAlignmentOptions::Center);
        SongInfoPlus::UIUtils::skewButton(button, 0.0f);
      }
    }
    button->get_gameObject()->SetActive(true);
  }
}

MAKE_HOOK_MATCH(
    StandardLevelDetailViewController_DidActivate,
    &GlobalNamespace::StandardLevelDetailViewController::DidActivate, void,
    GlobalNamespace::StandardLevelDetailViewController* self,
    bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling) {
  StandardLevelDetailViewController_DidActivate(
      self, firstActivation, addedToHeirarchy, screenSystemEnabling);
  firstActivate = firstActivation;
  viewController = self;

  if (modal) {
    modal->get_gameObject()->SetActive(false);
  }

  if (button) {
    button->get_gameObject()->SetActive(false);
  }
}

void SongInfoPlus::Hooks::StandardLevelDetailView() {
  INSTALL_HOOK(getLogger(), StandardLevelDetailView_RefreshContent);
  INSTALL_HOOK(getLogger(), StandardLevelDetailViewController_DidActivate);
}