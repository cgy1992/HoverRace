
// ClickRegion.cpp
//
// Copyright (c) 2013-2015 Michael Imamura.
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

#include "../Control/Action.h"
#include "../Util/Log.h"

#include "ClickRegion.h"

namespace Log = HoverRace::Util::Log;

namespace HoverRace {
namespace Display {

/**
 * Constructor for automatically-sized button.
 * @param display The display child elements will be attached to.
 * @param layoutFlags Optional layout flags.
 */
ClickRegion::ClickRegion(Display &display, uiLayoutFlags_t layoutFlags) :
	SUPER(layoutFlags),
	display(display),
	size(0, 0), autoSize(true), needsSizing(true),
	enabled(true), pressed(false)
{
}

/**
 * Constructor for fixed-sized button.
 * @param display The display child elements will be attached to.
 * @param size The fixed button size.
 * @param layoutFlags Optional layout flags.
 */
ClickRegion::ClickRegion(Display &display, const Vec2 &size,
	uiLayoutFlags_t layoutFlags) :
	SUPER(layoutFlags),
	display(display),
	size(size), autoSize(false), needsSizing(false),
	enabled(true), pressed(false)
{
}

ClickRegion::~ClickRegion()
{
}

bool ClickRegion::OnMouseMoved(const Vec2 &pos)
{
	if (IsEnabled()) {
		if (IsPressed()) {
			OnMouseDrag(ScreenPosToRel(pos));
		}
		bool hit = TestHit(pos);
		if (hit && !IsFocused()) {
			RequestFocus();
		}
		return hit;
	}
	else {
		//TODO: Unset focus.
		return false;
	}
}

bool ClickRegion::OnMousePressed(const Control::Mouse::Click &click)
{
	using namespace Control;

	if (IsEnabled() && click.btn == Mouse::LEFT && TestHit(click.pos)) {
		SetPressed(true);
		OnMouseDrag(ScreenPosToRel(click.pos));
		return true;
	}
	else {
		return false;
	}
}

bool ClickRegion::OnMouseReleased(const Control::Mouse::Click &click)
{
	using namespace Control;

	bool retv = false;

	if (click.btn == Mouse::LEFT) {
		if (IsPressed() && TestHit(click.pos)) {
			FireClickedSignal();
			retv = true;
		}
		SetPressed(false);
	}

	return retv;
}

bool ClickRegion::OnAction()
{
	FireClickedSignal();
	return true;
}

void ClickRegion::FireClickedSignal()
{
	clickedSignal(*this);
}

bool ClickRegion::TryFocus(const Control::Nav&)
{
	if (IsFocused()) return true;
	if (!IsEnabled()) return false;

	SetFocused(true);
	return true;
}

/**
 * Retrieve the size of the button.
 * If automatic sizing is enabled, then this will calculate the size
 * immediately (the same caveats as calling Measure()).
 * @return The size, where @c x is the width and @c y is the height.
 */
const Vec2 &ClickRegion::GetSize()
{
	if (needsSizing) {
		Vec3 calcSize = Measure();
		size.x = calcSize.x;
		size.y = calcSize.y;
		needsSizing = false;
	}
	return size;
}

/**
 * Set the button to a fixed size.
 * @param size The size of the container, where @c x is the width
 *             and @c y is the height.
 * @see SetAutoSize()
 */
void ClickRegion::SetSize(const Vec2 &size)
{
	if (this->size != size) {
		this->size = size;
		autoSize = false;
		needsSizing = false;
		FireModelUpdate(Props::SIZE);
	}
}

/**
 * Enable automatic sizing.
 * The size of the button will be determined by the contents.
 * To set a fixed size, call SetSize(const Vec2&).
 */
void ClickRegion::SetAutoSize()
{
	if (!autoSize) {
		autoSize = true;
		needsSizing = true;
		size.x = 0;
		size.y = 0;
		FireModelUpdate(Props::SIZE);
	}
}

void ClickRegion::SetPressed(bool pressed)
{
	if (this->pressed != pressed) {
		this->pressed = pressed;
		FireModelUpdate(Props::PRESSED);
	}
}

void ClickRegion::SetEnabled(bool enabled)
{
	if (this->enabled != enabled) {
		this->enabled = enabled;
		FireModelUpdate(Props::ENABLED);

		// Change back to unpressed and unfocused if disabled.
		if (!enabled) {
			SetPressed(false);
			RelinquishFocus(Control::Nav::NEUTRAL);
		}
	}
}

/**
 * Indicate that the size of the button may have changed, when auto-sizing
 * is enabled.
 *
 * If auto-sizing is not enabled, then this does nothing.
 */
void ClickRegion::RequestSizing()
{
	//FIXME: Always requesting sizing now to fix icon sizing when fixed size.
	//       We need a better way of dealing with this, really...
	needsSizing = true;
}

/**
 * Check if the coordinates of an event are within the bounds of this component.
 * @param pos The screen-space position.
 * @return true if the test passes, false if the test fails.
 */
bool ClickRegion::TestHit(const Vec2 &pos)
{
	// We assume that the view has made the screen-space bounds available.
	Vec2 boundsPos = GetView()->GetScreenPos();
	Vec2 boundsSize = GetView()->GetScreenSize();
	return pos.x >= boundsPos.x &&
		pos.y >= boundsPos.y &&
		(pos.x < boundsPos.x + boundsSize.x) &&
		(pos.y < boundsPos.y + boundsSize.y);
}

/**
 * Convert screen coordinates to relative (widget) coordinates.
 * @param pos The screen coordinates.
 * @return The relative coordinates.
 */
Vec2 ClickRegion::ScreenPosToRel(const Vec2 &pos)
{
	auto boundsPos = GetView()->GetScreenPos();
	auto boundsSize = GetView()->GetScreenSize();
	auto &widgetSize = GetSize();

	auto relScrPos = pos - boundsPos;

	return {
		(relScrPos.x * widgetSize.x) / boundsSize.x,
		(relScrPos.y * widgetSize.y) / boundsSize.y
	};
}

}  // namespace Display
}  // namespace HoverRace
