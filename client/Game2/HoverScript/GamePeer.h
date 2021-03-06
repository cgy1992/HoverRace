
// GamePeer.h
//
// Copyright (c) 2010, 2014-2016 Michael Imamura.
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

#include <luabind/luabind.hpp>
#include <luabind/object.hpp>

#include "../../../engine/Script/Handlers.h"
#include "../../../engine/Script/Peer.h"

namespace HoverRace {
	namespace Client {
		namespace HoverScript {
			class ConfigPeer;
			class SessionPeer;
		}
		class GameDirector;
		class Rulebook;
		class RulebookLibrary;
	}
	namespace Display {
		class Display;
	}
	namespace Script {
		class Core;
	}
}

namespace HoverRace {
namespace Client {
namespace HoverScript {

/**
 * Scripting peer for system-level control of the game.
 * @author Michael Imamura
 */
class GamePeer : public Script::Peer
{
	using SUPER = Script::Peer;

public:
	GamePeer(Script::Core &scripting, GameDirector &director,
		RulebookLibrary &rulebookLibrary);
	virtual ~GamePeer() { }

public:
	static void Register(Script::Core &scripting);

public:
	void SetDisplay(Display::Display *display) { this->display = display; }

public:
	void OnInit();
	void OnShutdown();
	void OnSessionStart(std::shared_ptr<SessionPeer> sessionPeer);
	void OnSessionEnd(std::shared_ptr<SessionPeer> sessionPeer);

protected:
	void VerifyInitialized() const;

public:
	ConfigPeer *LGetConfig();

	void LAddLocalPlayer();
	void LAddLocalPlayer_N(const std::string &name);

	void LAnnounce(const std::string &label);
	void LAnnounce_T(const std::string &label, const std::string &text);

	bool LIsInitialized();

	void LOnInit(const luabind::object &fn);
	void LOnInit_N(const std::string &name, const luabind::object &fn);

	void LOnShutdown(const luabind::object &fn);
	void LOnShutdown_N(const std::string &name, const luabind::object &fn);

	void LOnSessionBegin(const luabind::object &fn);
	void LOnSessionBegin_N(const std::string &name, const luabind::object &fn);

	void LOnSessionEnd(const luabind::object &fn);
	void LOnSessionEnd_N(const std::string &name, const luabind::object &fn);

	void LScreenshot();

	void LStartMenuMenu();

	void LStartPractice(const std::string &track);
	void LStartPractice_O(const std::string &track,
		const luabind::object &opts);
	void LStartPractice_RO(const std::string &track,
		const std::string &rulebookName, const luabind::object &opts);

	void LShutdown();

	bool LToggleHud();

private:
	GameDirector &director;
	RulebookLibrary &rulebookLibrary;
	Display::Display *display;
	bool initialized;
	Script::Handlers onInit;
	Script::Handlers onShutdown;
	Script::Handlers onSessionStart;
	Script::Handlers onSessionEnd;
};

}  // namespace HoverScript
}  // namespace Client
}  // namespace HoverRace
