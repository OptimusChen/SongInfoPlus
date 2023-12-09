#include "main.hpp"
#include "SongInfoModal.hpp"
#include "Sprites.hpp"
#include "include/Utils/UIUtils.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "HMUI/CurvedCanvasSettingsHelper.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/Application.hpp"
#include "GlobalNamespace/LevelBar.hpp"
#include "GlobalNamespace/SoloFreePlayFlowCoordinator.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/UI/Selectable.hpp"
#include "UnityEngine/GameObject.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

using namespace QuestUI;
using namespace HMUI;
using namespace GlobalNamespace;
using namespace UnityEngine::UI;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->get_gameObject()->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();  \
    layout##identifier->set_preferredWidth(width);                                          \
    layout##identifier->set_preferredHeight(height)

namespace SongInfoPlus {

    SongInfoModal::SongInfoModal(GlobalNamespace::StandardLevelDetailViewController *view) {
        viewController = view;

        artModal = nullptr;
        descModal = nullptr;

        initialized = false;
    }

    void SongInfoModal::Initialize(StandardLevelDetailView* view) {
        if (initialized) return;

        image = view->levelBar->songArtworkImageView;

        button = BeatSaberUI::CreateUIButton(image->get_transform(), "", "PlayButton", {1.5, 0}, {13.9, 16}, [=]() {
            this->Show();
        });

        button->set_name("DetailsButton");

        SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(ButtonCoroutine()));

        UIUtils::SkewButton(button, image->skew);
        UIUtils::RecolorButton(button, Color(0, 0, 0, 0), {image});

        SoloFreePlayFlowCoordinator* soloFlow = ArrayUtil::First(GameObject::FindObjectsOfType<SoloFreePlayFlowCoordinator*>());

        modal = BeatSaberUI::CreateModal(soloFlow->get_topViewController()->get_transform(), {100.0f, 45.0f}, [](HMUI::ModalView* modal) {}, true);

        loading = UIUtils::CreateLoadingIndicator(modal->get_transform());
        loading->set_active(false);

        SetupMainModal();
        SetupDescriptionModal();
        SetupArtworkModal();
        SetupHashModal();

        initialized = true;
    }

    void SongInfoModal::SetupMainModal() {
        horizontal = BeatSaberUI::CreateHorizontalLayoutGroup(modal->get_transform());

        horizontal->set_padding(RectOffset::New_ctor(1, 1, 1, 1));
        horizontal->set_spacing(6);

        SetPreferredSize(horizontal, 100, 45);

        VerticalLayoutGroup* buttons = BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());

        buttons->set_padding(RectOffset::New_ctor(1, 1, 1, 1));
        buttons->set_childAlignment(TextAnchor::MiddleCenter);

        SetPreferredSize(buttons, 30, 45);

        UIUtils::SkewButton(BeatSaberUI::CreateUIButton(buttons->get_transform(), "BEATSAVER", Vector2::get_zero(), Vector2(20.0f, 9.0f), [=]() { BeatSaverClicked(); }), 0);
        UIUtils::SkewButton(BeatSaberUI::CreateUIButton(buttons->get_transform(), "LEVEL HASH", Vector2::get_zero(), Vector2(20.0f, 9.0f), [=]() { HashClicked(); }), 0);
        UIUtils::SkewButton(BeatSaberUI::CreateUIButton(buttons->get_transform(), "DESCRIPTION", Vector2::get_zero(), Vector2(20.0f, 9.0f), [=]() { DescriptionClicked(); }), 0);
        UIUtils::SkewButton(BeatSaberUI::CreateUIButton(buttons->get_transform(), "VIEW ARTWORK", Vector2::get_zero(), Vector2(20.0f, 9.0f), [=]() { ArtworkClicked(); }), 0);

        VerticalLayoutGroup* infoAndButton = BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());

        infoAndButton->set_spacing(2.5);
        infoAndButton->set_padding(RectOffset::New_ctor(1, 1, 1, 1));

        SetPreferredSize(infoAndButton, 30, 9);

        VerticalLayoutGroup* info = BeatSaberUI::CreateVerticalLayoutGroup(infoAndButton->get_transform());

        info->set_padding(RectOffset::New_ctor(1, 1, 1, 1));
        info->set_spacing(7);

        auto logo = BeatSaberUI::CreateText(info->get_transform(), "SongInfo+", false);
        logo->set_fontSize(9);

        SetPreferredSize(info, 30, 40);

        VerticalLayoutGroup* text = BeatSaberUI::CreateVerticalLayoutGroup(info->get_transform());

        text->set_spacing(4.5);
        text->set_padding(RectOffset::New_ctor(1, 1, 1, 1));

        SetPreferredSize(text, 30, 20);

        key = BeatSaberUI::CreateText(text->get_transform(), "Key: xxxx", false);
        key->set_fontSize(3);

        author = BeatSaberUI::CreateText(text->get_transform(), "Author: xxxx", false);
        author->set_fontSize(3);

        mapper = BeatSaberUI::CreateText(text->get_transform(), "Mapper: xxxx", false);
        mapper->set_fontSize(3);

        uploaded = BeatSaberUI::CreateText(text->get_transform(), "Uploaded: xxxx", false);
        uploaded->set_fontSize(3);

        HorizontalLayoutGroup* buttonLayout = BeatSaberUI::CreateHorizontalLayoutGroup(infoAndButton->get_transform());

        buttonLayout->set_childControlWidth(false);

        SetPreferredSize(buttonLayout, 30, 9);

        UIUtils::SkewButton(BeatSaberUI::CreateUIButton(buttonLayout->get_transform(), "OK", "PlayButton", Vector2::get_zero(),Vector2(25.0f, 9.0f),[=]() {
            modal->Hide(true, nullptr);
          }),
        0);

        VerticalLayoutGroup* voting = BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());

        voting->get_rectTransform()->set_anchoredPosition({0, -10});

        SetPreferredSize(voting, 20, 40);

        BeatSaberUI::AddHoverHint(voting, "Can't check for unique game installs on modded game, I blame meta.");

        auto vote = BeatSaberUI::CreateClickableText(voting->get_transform(), "Vote Disabled", false);
        vote->set_fontSize(4);
        vote->set_alignment(TMPro::TextAlignmentOptions::Center);

        auto upvote = BeatSaberUI::CreateHorizontalLayoutGroup(voting->get_transform());
        SetPreferredSize(upvote, 8, 8);

        auto up = BeatSaberUI::CreateImage(upvote->get_transform(), BeatSaberUI::Base64ToSprite(ARROW));
        up->set_preserveAspect(true);
        up->set_color(Color(0.388f, 1, 0.388f, 1));

        rating = BeatSaberUI::CreateText(voting->get_transform(), "0", false);
        rating->set_alignment(TMPro::TextAlignmentOptions::Center);

        auto downvote = BeatSaberUI::CreateHorizontalLayoutGroup(voting->get_transform());
        SetPreferredSize(downvote, 8, 8);

        auto down = BeatSaberUI::CreateImage(downvote->get_transform(), BeatSaberUI::Base64ToSprite(DOWN_ARROW));
        down->set_preserveAspect(true);
        down->set_color(Color(1, 0.188f, 0.188f, 1));

        BeatSaberUI::CreateText(voting->get_transform(), "Rating", false)->set_alignment(TMPro::TextAlignmentOptions::Center);

        percentage = BeatSaberUI::CreateText(voting->get_transform(), "0%", false);
        percentage->set_fontSize(8);
        percentage->set_alignment(TMPro::TextAlignmentOptions::Center);
    }

    void SongInfoModal::SetupDescriptionModal() {
        descModal = BeatSaberUI::CreateModal(horizontal->get_transform()->get_parent(), {80, 65}, Vector2::get_zero(), [=](HMUI::ModalView* desc) {
            desc->Hide(true, nullptr);
            modal->Show(true, true, nullptr);
        }, true);

        GameObject* scrollable = BeatSaberUI::CreateScrollableModalContainer(descModal);
        scrollable->GetComponent<ExternalComponents*>()->Get<RectTransform*>()->set_sizeDelta({8, 1});

        HorizontalLayoutGroup* horizon = BeatSaberUI::CreateHorizontalLayoutGroup(scrollable->get_transform());
        VerticalLayoutGroup* layout = BeatSaberUI::CreateVerticalLayoutGroup(horizon->get_transform());

        description = BeatSaberUI::CreateText(layout->get_transform(), "xxx", false);
        description->set_alignment(TMPro::TextAlignmentOptions::MidlineLeft);
        description->set_enableWordWrapping(true);

        layout->set_childControlHeight(true);
        layout->set_childForceExpandHeight(true);
        layout->set_childControlWidth(true);
        layout->set_childForceExpandWidth(true);

        horizon->set_childControlWidth(true);
        horizon->set_childForceExpandWidth(true);

        description->get_gameObject()->AddComponent<LayoutElement*>()->set_preferredWidth(65.0f);

        auto exit = BeatSaberUI::CreateUIButton(scrollable->get_transform(), "OK", "PlayButton", Vector2::get_zero(), {30, 8}, [=]() {
            descModal->HandleBlockerButtonClicked();
        });

        UIUtils::SkewButton(exit, 0);
    }

    void SongInfoModal::SetupArtworkModal() {
        artModal = BeatSaberUI::CreateModal(horizontal->get_transform()->get_parent(), {63, 72}, Vector2::get_zero(), [=](HMUI::ModalView* art) {
            art->Hide(true, nullptr);
            modal->Show(true, true, nullptr);
        }, true);

        VerticalLayoutGroup* vertical = BeatSaberUI::CreateVerticalLayoutGroup(artModal->get_transform());

        SetPreferredSize(vertical, 63, 72);

        vertical->set_padding(RectOffset::New_ctor(5, 5, 5, 5));
        vertical->set_spacing(3);

        artwork = BeatSaberUI::CreateImage(vertical->get_transform(), image->get_sprite(), Vector2::get_zero(), {65, 65});
        artwork->set_preserveAspect(true);

        SetPreferredSize(artwork, 65, 65);

        auto exit = BeatSaberUI::CreateUIButton(vertical->get_transform(), "OK", "PlayButton", Vector2::get_zero(), {10, 2}, [=]() {
            artModal->HandleBlockerButtonClicked();
        });

        SetPreferredSize(exit, 20, 8);

        UIUtils::SkewButton(exit, 0);
    }

    void SongInfoModal::SetupHashModal() {
        hashModal = BeatSaberUI::CreateModal(horizontal->get_transform()->get_parent(), {100, 20}, Vector2::get_zero(), [=](HMUI::ModalView* hash) {
            hash->Hide(true, nullptr);
            modal->Show(true, true, nullptr);
        }, true);

        VerticalLayoutGroup* vertical = BeatSaberUI::CreateVerticalLayoutGroup(hashModal->get_transform());

        SetPreferredSize(vertical, 100, 20);

        vertical->set_padding(RectOffset::New_ctor(2, 2, 2, 2));
        vertical->set_spacing(3);

        levelHash = BeatSaberUI::CreateText(vertical->get_transform(), "Hash: xxxx", false, {0, 0});
        levelHash->set_alignment(TMPro::TextAlignmentOptions::Center);

        auto exit = BeatSaberUI::CreateUIButton(vertical->get_transform(), "OK", "PlayButton", Vector2::get_zero(), Vector2::get_zero(), [=]() {
            hashModal->HandleBlockerButtonClicked();
        });

        SetPreferredSize(exit, 20, 8);

        UIUtils::SkewButton(exit, 0);
    }

    void SongInfoModal::RefreshModal(bool show) {
        if (!initialized) return;

        if (show) Show();
    }

    void SongInfoModal::Show() {
        modal->Show(true, false, nullptr);

        horizontal->get_gameObject()->set_active(false);
        loading->set_active(true);

        StringW hash = map->get_levelID();

        if (!hash->StartsWith("custom_level_")) return;

        hash = hash->Substring(13);

        levelHash->set_text(string_format("Hash: %s", static_cast<std::string>(hash).c_str()));

        getLogger().info("SongInfoMap: %s", static_cast<std::string>(hash).c_str());

        BeatSaver::API::GetBeatmapByHashAsync(hash, [=] (std::optional<BeatSaver::Beatmap> optionalMap) {
            if (!optionalMap.has_value()) return;

            optionalBeatmap.emplace(optionalMap.value());

            auto beatmap = optionalBeatmap.value();

            MainThreadScheduler::Schedule([=]() {
                key->set_text("Key: " + beatmap.GetId());
                author->set_text("Author: " + map->get_songAuthorName());
                mapper->set_text("Mapper: " + map->get_levelAuthorName());
                uploaded->set_text("Uploaded: " + UIUtils::getReadableDate(beatmap.GetUploaded()));

                int upvotes = beatmap.GetStats().GetUpVotes();
                int downvotes = beatmap.GetStats().GetDownVotes();

                rating->set_text(std::to_string(upvotes - downvotes));

                float percent = beatmap.GetStats().GetScore() * 100;

                percentage->set_text(string_format("%.0f", percent) + "%");
                percentage->set_color(UIUtils::GetColor(percent / 100));

                description->set_text(beatmap.GetDescription());

                loading->set_active(false);
                horizontal->get_gameObject()->set_active(true);
            });
        });
    }

    void SongInfoModal::BeatSaverClicked() {
        auto beatmap = optionalBeatmap.value();

        Application::OpenURL(string_format("https://beatsaver.com/maps/%s", beatmap.GetId().c_str()));
    }

    void SongInfoModal::HashClicked() {
        modal->Hide(true, nullptr);

        hashModal->Show(true, true, nullptr);
    }

    void SongInfoModal::DescriptionClicked() {
        modal->Hide(true, nullptr);

        descModal->Show(true, true, nullptr);
    }

    void SongInfoModal::ArtworkClicked() {
        modal->Hide(true, nullptr);

        artwork->set_sprite(image->get_sprite());

        artModal->Show(true, true, nullptr);
    }

    void SongInfoModal::SetBeatmap(GlobalNamespace::IPreviewBeatmapLevel* map) {
        this->map = map;
    }

    // jank
    custom_types::Helpers::Coroutine SongInfoModal::ButtonCoroutine() {
        while (button && image) {
            if (button->isPointerInside) {
                image->set_color(Color(0.60f, 0.80f, 1.0f, 1.0f));
            } else {
                image->set_color(Color(1, 1, 1, 1));
            }

            co_yield nullptr;
        }

        co_return;
    }
}