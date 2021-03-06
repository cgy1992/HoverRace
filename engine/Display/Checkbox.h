
// Checkbox.h
//
// Copyright (c) 2013, 2014 Michael Imamura.
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

#pragma once

#include "StateButton.h"

#if defined(_WIN32) && defined(HR_ENGINE_SHARED)
#	ifdef MR_ENGINE
#		define MR_DllDeclare   __declspec( dllexport )
#	else
#		define MR_DllDeclare   __declspec( dllimport )
#	endif
#else
#	define MR_DllDeclare
#endif

namespace HoverRace {
	namespace Display {
		class Display;
	}
}

namespace HoverRace {
namespace Display {

/**
 * A checkbox widget.
 * @author Michael Imamura
 */
class MR_DllDeclare Checkbox : public StateButton
{
	using SUPER = StateButton;

public:
	Checkbox(Display &display, const std::string &text,
		uiLayoutFlags_t layoutFlags = 0);
	Checkbox(Display &display, const Vec2 &size, const std::string &text,
		uiLayoutFlags_t layoutFlags = 0);
	virtual ~Checkbox() { }

private:
	void Init();
	void InitIcon(bool enabled, bool checked);

protected:
	void FireClickedSignal() override;
};

}  // namespace Display
}  // namespace HoverRace

#undef MR_DllDeclare
