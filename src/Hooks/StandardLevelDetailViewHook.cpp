#include "main.hpp"

#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"

using namespace GlobalNamespace;
using namespace SongInfoPlus;

std::shared_ptr<SongInfoModal> modal;

MAKE_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &StandardLevelDetailView::RefreshContent, void, StandardLevelDetailView* self) {
    StandardLevelDetailView_RefreshContent(self);

    getLogger().info("refreshing");
    modal->Initialize(self);

    auto level = reinterpret_cast<IPreviewBeatmapLevel*>(self->get_selectedDifficultyBeatmap()->get_level());

    modal->SetBeatmap(level);
    modal->RefreshModal(false);
}

MAKE_HOOK_MATCH(StandardLevelDetailViewController_DidActivate, &StandardLevelDetailViewController::DidActivate, void, StandardLevelDetailViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    StandardLevelDetailViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    getLogger().info("%p", modal.get());
    if (modal) return;

    getLogger().info("creating new modal");
    modal = std::make_shared<SongInfoModal>(self);
}

MAKE_HOOK_MATCH(MenuTransitionsHelper_RestartGame, &MenuTransitionsHelper::RestartGame, void, MenuTransitionsHelper* self, System::Action_1<Zenject::DiContainer *> *finishCallback) {
    getLogger().info("%p", modal.get());

    if (modal) {
        modal->initialized = false;
    }

    MenuTransitionsHelper_RestartGame(self, finishCallback);
}

void SongInfoPlus::Hooks::StandardLevelDetailView() {
    INSTALL_HOOK(getLogger(), StandardLevelDetailView_RefreshContent)
    INSTALL_HOOK(getLogger(), StandardLevelDetailViewController_DidActivate)
}

void SongInfoPlus::Hooks::MenuTransitionsHelper() {
    INSTALL_HOOK(getLogger(), MenuTransitionsHelper_RestartGame)
}