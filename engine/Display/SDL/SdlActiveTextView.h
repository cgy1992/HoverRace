
// SdlActiveTextView.h
//
// Copyright (c) 2015 Michael Imamura.
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

#include "../TypeCase.h"

#include "SdlDisplay.h"
#include "SdlView.h"

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
		class ActiveText;
	}
}

namespace HoverRace {
namespace Display {
namespace SDL {

class MR_DllDeclare SdlActiveTextView : public SdlView<ActiveText>
{
	using SUPER = SdlView<ActiveText>;

public:
	SdlActiveTextView(SdlDisplay &disp, ActiveText &model);
	virtual ~SdlActiveTextView();

public:
	void OnModelUpdate(int prop) override;

public:
	Vec3 Measure() override;
	void PrepareRender() override;
	void Render() override;

private:
	void OnUiScaleChanged();
	void UpdateTexture();

private:
	std::shared_ptr<TypeCase> typeCase;
	TypeLine typeLine;
	bool needsUpdate;
	int width;
	int height;
	double unscaledWidth;
	double unscaledHeight;
	boost::signals2::scoped_connection uiScaleChangedConnection;
};

}  // namespace SDL
}  // namespace Display
}  // namespace HoverRace

#undef MR_DllDeclare
