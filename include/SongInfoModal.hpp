#pragma once

#include "HMUI/ModalView.hpp"
#include "HMUI/ImageView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "questui/shared/CustomTypes/Components/ClickableImage.hpp"
#include "custom-types/shared/coroutine.hpp"

namespace SongInfoPlus {
    class SongInfoModal {
        public:
            explicit SongInfoModal(GlobalNamespace::StandardLevelDetailViewController* view);

            UnityEngine::UI::Button* button;
            bool initialized;

            void Initialize(GlobalNamespace::StandardLevelDetailView* view);
            void RefreshModal(bool show);
            void SetBeatmap(GlobalNamespace::IPreviewBeatmapLevel* map);
            void Show();

        private:
            HMUI::ModalView* modal;
            HMUI::ModalView* artModal;
            HMUI::ModalView* descModal;

            HMUI::ImageView* image;
            HMUI::ImageView* artwork;

            TMPro::TextMeshProUGUI* key;
            TMPro::TextMeshProUGUI* author;
            TMPro::TextMeshProUGUI* mapper;
            TMPro::TextMeshProUGUI* uploaded;
            TMPro::TextMeshProUGUI* rating;
            TMPro::TextMeshProUGUI* percentage;
            TMPro::TextMeshProUGUI* levelHash;
            TMPro::TextMeshProUGUI* description;

            UnityEngine::GameObject* loading;

            std::optional<BeatSaver::Beatmap> optionalBeatmap;

            UnityEngine::UI::HorizontalLayoutGroup* horizontal;

            GlobalNamespace::StandardLevelDetailViewController* viewController;

            GlobalNamespace::IPreviewBeatmapLevel* map;

            void SetupMainModal();
            void BeatSaverClicked();
            void HashClicked();
            void DescriptionClicked();
            void ArtworkClicked();
            custom_types::Helpers::Coroutine ButtonCoroutine();
    };
}