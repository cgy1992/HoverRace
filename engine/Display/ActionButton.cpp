
// ActionButton.cpp
//
// Copyright (c) 2013-2016 Michael Imamura.
//
// Licensed under GrokkSoft HoverRace SourceCode License v1.0(the "License");
// you may not use this file except in compliance with the License.
//
// A copy of the license should have been attached to the package from which
// you have taken this file. If you can not find the license you can not use
// this file.
//
//
// The author makes no representations about the suitability of
// this software for any purpose.  It is provided "as is" "AS IS",
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.
//
// See the License for the specific language governing permissions
// and limitations under the License.

#include "KeycapIcon.h"

#include "ActionButton.h"

namespace HoverRace {
namespace Display {

/**
 * Constructor for automatically-sized button with automatic label.
 * @note The button will have no text until an action is attached.
 * @param display The display child elements will be attached to.
 * @param layoutFlags Optional layout flags.
 */
ActionButton::ActionButton(Display &display, uiLayoutFlags_t layoutFlags) :
	SUPER(display, "", layoutFlags),
	fixedText(), armed(true)
{
	InitKeycap();
}

/**
 * Constructor for automatically-sized button with fixed label.
 * @note The button will have no text until an action is attached.
 * @param display The display child elements will be attached to.
 * @param text The fixed label (will not be set from controller).
 * @param layoutFlags Optional layout flags.
 */
ActionButton::ActionButton(Display &display, const std::string &text,
	uiLayoutFlags_t layoutFlags) :
	SUPER(display, "", layoutFlags),
	fixedText(text), armed(true)
{
	InitKeycap();
}

/**
 * Constructor for fixed-sized button with automatic label.
 * @note The button will have no text until an action is attached.
 * @param display The display child elements will be attached to.
 * @param size The fixed button size.
 * @param layoutFlags Optional layout flags.
 */
ActionButton::ActionButton(Display &display, const Vec2 &size,
	uiLayoutFlags_t layoutFlags) :
	SUPER(display, size, "", layoutFlags),
	fixedText(), armed(true)
{
	InitKeycap();
}

/**
 * Constructor for fixed-sized button with fixed label.
 * @note The button will have no text until an action is attached.
 * @param display The display child elements will be attached to.
 * @param size The fixed button size.
 * @param text The fixed label (will not be set from controller).
 * @param layoutFlags Optional layout flags.
 */
ActionButton::ActionButton(Display &display, const Vec2 &size,
	const std::string &text, uiLayoutFlags_t layoutFlags) :
	SUPER(display, size, "", layoutFlags),
	fixedText(text), armed(true)
{
	InitKeycap();
}

ActionButton::~ActionButton()
{
}

void ActionButton::InitKeycap()
{
	using Controller = Control::InputEventController;
	keycap = std::make_shared<KeycapIcon>(1,
		Controller::HashKeyboardEvent(SDLK_UNKNOWN));
	keycap->AttachView(display);
	SetIcon(keycap);
}

/**
 * Attach an action to the button.
 * This updates the button's text with the name and hotkey of the action.
 * Clicking on the button will fire the attached action.
 * This is typically called from Client::Scene::AttachController.
 * @param controller The controller that owns the action.
 * @param action The action to attach.
 */
void ActionButton::AttachAction(Control::InputEventController&,
	Control::InputEventController::VoidActionPtr action)
{
	this->action = action;

	SetText(fixedText.empty() ? action->GetName() : fixedText);
	keycap->SetKeyHash(action->GetPrimaryTrigger());
	RequestLayout();
}

/**
 * Detach the currently-attached action (if any).
 * @note It is usually unnecessary to call this from
 *       Client::Scene::DetachController since this class maintains a weak
 *       reference to the attached action.
 */
void ActionButton::DetachAction()
{
	action.reset();
}

void ActionButton::FireClickedSignal()
{
	// "Disarm" this button to prevent infinite recursion when the "OK" action
	// is in play -- the action would fire and be received by the scene, which
	// would pass it to the focused button, which is probably this button.
	//
	// We don't simply disable the button temporarily -- that would pass the
	// focus to another button, triggering it instead.
	if (!armed) return;
	armed = false;

	if (auto actionLock = action.lock()) {
		(*actionLock)(1);
	}

	armed = true;
}

}  // namespace Display
}  // namespace HoverRace
