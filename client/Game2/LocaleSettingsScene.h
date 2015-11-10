
// LocaleSettingsScene.h
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

#include "../../engine/Util/Config.h"

#include "SettingsScene.h"

namespace HoverRace {
	namespace Display {
		class Label;
	}
	namespace Util {
		class Locale;
	}
}

namespace HoverRace {
namespace Client {

/**
* Locale settings dialog.
* @author Michael Imamura
*/
class LocaleSettingsScene : public SettingsScene
{
	using SUPER = SettingsScene;

public:
	LocaleSettingsScene(Display::Display &display, GameDirector &director,
		const std::string &parentTitle);
	virtual ~LocaleSettingsScene() { }

protected:
	void LoadFromConfig() override;
	void ResetToDefaults() override;

private:
	Util::Locale &localeCfg;

	std::shared_ptr<Display::Label> localeLbl;
};

}  // namespace Client
}  // namespace HoverRace