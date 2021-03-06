// ActionPerformers.h
// A collection of classes implementing the ActionPerformer template class
// (just has to implement operator() and a constructor).  These are all
// actions to be performed when the user presses a control key.
//
// Copyright (c) 2010, Ryan Curtin
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
//

#pragma once

#include "Controller.h"
#include "ControlAction.h"

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
	namespace MainCharacter {
		class MainCharacter;
	}
}

namespace HoverRace {
namespace Control {

/***
 * \class PlayerEffectAction
 *
 * Base class for actions that affect a player.  This gives us a unified
 * constructor so we can hang on to a shared pointer to the player.  For speed
 * concerns, most of these do not check if the pointer is NULL.  Given that that
 * condition should never happen, this should not be a problem.
 */
class MR_DllDeclare PlayerEffectAction : public ControlAction<int> {
	public:
		// C++0x should give us constructor inheritance... whenever that happens
		PlayerEffectAction(std::string name, int listOrder, MainCharacter::MainCharacter* mc);

		void SetMainCharacter(MainCharacter::MainCharacter* mc);
		virtual void operator()(int value) = 0;

	protected:
		// TODO: shared_ptr
		MainCharacter::MainCharacter* mc;
};

/***
 * \class EngineAction
 * 
 * Turns the motor on or off.
 */
class MR_DllDeclare EngineAction : public PlayerEffectAction {
	public:
		EngineAction(std::string name, int listOrder, MainCharacter::MainCharacter* mc) : PlayerEffectAction(name, listOrder, mc) { }

		/***
		 * eventValue > 0: turn engine on.
		 * eventValue == 0: turn engine off.
		 */
		virtual void operator()(int eventValue);
};

/***
 * \class TurnLeftAction
 *
 * Turns the craft left.
 */
class MR_DllDeclare TurnLeftAction : public PlayerEffectAction {
	public:
		TurnLeftAction(std::string name, int listOrder, MainCharacter::MainCharacter* mc) : PlayerEffectAction(name, listOrder, mc) { }

		/***
		 * eventValue > 0: set craft to turn left
		 * eventValue == 0: stop left turn
		 */
		virtual void operator()(int eventValue);
};

/***
 * \class TurnRightAction
 *
 * Turns the craft right.
 */
class MR_DllDeclare TurnRightAction : public PlayerEffectAction {
	public:
		TurnRightAction(std::string name, int listOrder, MainCharacter::MainCharacter* mc) : PlayerEffectAction(name, listOrder, mc) { }
		
		/***
		 * eventValue > 0: set craft to turn right
		 * eventValue == 0: stop right turn
		 */
		virtual void operator()(int eventValue);
};

/***
 * \class JumpAction
 *
 * Makes the craft jump.
 */
class MR_DllDeclare JumpAction : public PlayerEffectAction {
	public:
		JumpAction(std::string name, int listOrder, MainCharacter::MainCharacter* mc) : PlayerEffectAction(name, listOrder, mc) { }
		
		/***
		 * The craft will be told to jump, regardless of eventValue.
		 */
		virtual void operator()(int eventValue);
};

/***
 * \class PowerupAction
 *
 * Fires a missile, lays a mine, or uses a speed can, depending on the user's
 * selected item.
 */
class MR_DllDeclare PowerupAction : public PlayerEffectAction {
	public:
		PowerupAction(std::string name, int listOrder, MainCharacter::MainCharacter* mc) : PlayerEffectAction(name, listOrder, mc) { }
		
		/***
		 * Uses the item, regardless of eventValue.
		 */
		virtual void operator()(int eventValue);
};

/***
 * \class ChangeItemAction
 *
 * Changes the current item.
 */
class MR_DllDeclare ChangeItemAction : public PlayerEffectAction {
	public:	
		ChangeItemAction(std::string name, int listOrder, MainCharacter::MainCharacter* mc) : PlayerEffectAction(name, listOrder, mc) { }
		
		/***
		 * Changes the current item, regardless of eventValue.
		 */
		virtual void operator()(int eventValue);
};

/***
 * \class BrakeAction
 *
 * Brakes the craft (or turns it around so that if used in conjunction with the engine,
 * the craft slows down).
 */
class MR_DllDeclare BrakeAction : public PlayerEffectAction {
	public:
		BrakeAction(std::string name, int listOrder, MainCharacter::MainCharacter* mc) : PlayerEffectAction(name, listOrder, mc) { }
		
		/***
		 * eventValue > 0: set brake state on
		 * eventValue == 0: set brake state off
		 */
		virtual void operator()(int eventValue);
};

/***
 * \class LookBackAction
 *
 * Sets whether or not the player is looking back.  This is arguably not a
 * PlayerEffectAction but we'll leave it here for now.
 */
class MR_DllDeclare LookBackAction : public PlayerEffectAction {
	public:
		LookBackAction(std::string name, int listOrder, MainCharacter::MainCharacter* mc) : PlayerEffectAction(name, listOrder, mc) { }
		
		/***
		 * eventValue > 0: set lookback state on
		 * eventValue == 0: set lookback state off
		 */
		virtual void operator()(int eventValue);
};

} // namespace Control
} // namespace HoverRace

#undef MR_DllDeclare
