
// SessionPeer.h
//
// Copyright (c) 2010, 2013 Michael Imamura.
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

#include "../../../engine/Util/WorldCoordinates.h"
#include "../../../engine/Script/Peer.h"

namespace HoverRace {
	namespace Client {
		namespace HoverScript {
			class PlayerPeer;
		}
		class ClientSession;
	}
	namespace Script {
		class Core;
	}
}

namespace HoverRace {
namespace Client {
namespace HoverScript {

/**
 * Scripting peer for a game session.
 * @author Michael Imamura
 */
class SessionPeer : public Script::Peer {
	typedef Script::Peer SUPER;
	public:
		SessionPeer(Script::Core *scripting, ClientSession *session);
		virtual ~SessionPeer();

	public:
		static void Register(Script::Core *scripting);

	public:
		template<class Fn>
		void ForEachPlayer(Fn fn)
		{
			if (!playerRefs.empty()) {
				std::for_each(playerRefs.begin(), playerRefs.end(), fn);
			}
		}
		std::shared_ptr<PlayerPeer> &GetPlayer(unsigned idx)
		{
			return playerRefs[idx];
		}

		void OnSessionEnd();

	protected:
		void VerifySession() const;

	public:
		int LGetNumPlayers() const;
		MR_SimulationTime LGetTime() const;

	private:
		ClientSession *session;
		luabind::object rules;
		luabind::object players;
		std::vector<std::shared_ptr<PlayerPeer>> playerRefs;
};
typedef std::shared_ptr<SessionPeer> SessionPeerPtr;

}  // namespace HoverScript
}  // namespace Client
}  // namespace HoverRace
