
#include <fstream>

#include "GlobalNamespace/BoolSettingsController.hpp"
#include "GlobalNamespace/ColorPickerButtonController.hpp"
#include "GlobalNamespace/FormattedFloatListSettingsValueController.hpp"
#include "GlobalNamespace/HSVPanelController.hpp"
#include "GlobalNamespace/ReleaseInfoViewController.hpp"
#include "GlobalNamespace/UIKeyboardManager.hpp"
#include "HMUI/ButtonSpriteSwap.hpp"
#include "HMUI/ColorGradientSlider.hpp"
#include "HMUI/CurvedCanvasSettings.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/DropdownWithTableView.hpp"
#include "HMUI/EventSystemListener.hpp"
#include "HMUI/HoverHintController.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/InputFieldView_InputFieldChanged.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/TableView_CellsGroup.hpp"
#include "HMUI/TableView_ScrollPositionType.hpp"
#include "HMUI/TextPageScrollView.hpp"
#include "HMUI/TextSegmentedControl.hpp"
#include "HMUI/TimeSlider.hpp"
#include "HMUI/Touchable.hpp"
#include "HMUI/UIKeyboard.hpp"
#include "Polyglot/LocalizedTextMeshProUGUI.hpp"
#include "System/Action.hpp"
#include "System/Action_2.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
#include "System/Convert.hpp"
#include "UnityEngine/AdditionalCanvasShaderChannels.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/CanvasGroup.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "UnityEngine/Events/UnityAction_1.hpp"
#include "UnityEngine/ImageConversion.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/RenderMode.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/TextureFormat.hpp"
#include "UnityEngine/TextureWrapMode.hpp"
#include "UnityEngine/UI/CanvasScaler.hpp"
#include "UnityEngine/UI/RectMask2D.hpp"
#include "UnityEngine/UI/ScrollRect.hpp"
#include "VRUIControls/VRGraphicRaycaster.hpp"
#include "Zenject/DiContainer.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreen.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreenManager.hpp"
#include "questui/shared/CustomTypes/Components/List/QuestUITableView.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"


#define DEFAULT_BUTTONTEMPLATE "PracticeButton"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace UnityEngine::Events;
using namespace TMPro;
using namespace HMUI;
using namespace Polyglot;
using namespace VRUIControls;
using namespace Zenject;