
// GameScene.cpp
//
// Copyright (c) 2010, 2013, 2014 Michael Imamura.
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

#include "../StdAfx.h"

#include "../../engine/Display/Display.h"
#include "../../engine/Display/Hud.h"
#include "../../engine/Model/Track.h"
#include "../../engine/Parcel/TrackBundle.h"
#include "../../engine/Util/Duration.h"
#include "../../engine/Util/Loader.h"
#include "../../engine/VideoServices/SoundServer.h"
#include "../../engine/VideoServices/VideoBuffer.h"

#include "HoverScript/GamePeer.h"
#include "HoverScript/HudPeer.h"
#include "HoverScript/MetaPlayer.h"
#include "HoverScript/MetaSession.h"
#include "HoverScript/PlayerPeer.h"
#include "HoverScript/SessionPeer.h"

#include "ClientSession.h"
#include "Rulebook.h"
#include "Rules.h"

#include "GameScene.h"

using namespace HoverRace::Client::HoverScript;
using namespace HoverRace::Util;

namespace HoverRace {
namespace Client {

GameScene::Viewport::Viewport(Display::Display &display, Observer *observer,
                              Display::Hud *hud) :
	observer(observer), hud(hud)
{
	hud->AttachView(display);
}

GameScene::GameScene(const std::string &name,
                     Display::Display &display, GameDirector &director,
                     Script::Core *scripting, std::shared_ptr<Rules> rules,
                     std::shared_ptr<Loader> loader) :
	SUPER(name),
	display(display), director(director), scripting(scripting), rules(rules),
	finishedLoading(false), muted(false),
	session(nullptr),
	firedOnStart(false), firedOnRaceFinish(false)
{
	finishedLoadingConn =
		loader->GetFinishedLoadingSignal().connect(
			std::bind(&GameScene::OnFinishedLoading, this));

	session = new ClientSession(rules);

	// Schedule the remaining load items.
	ScheduleLoad(loader);
}

GameScene::~GameScene()
{
	Cleanup();
}

void GameScene::Cleanup()
{
	director.GetSessionChangedSignal()(nullptr);
	if (metaSession) {
		metaSession->GetSession()->OnSessionEnd();
	}
	delete session;

	// Apply sound state to halt currently-playing continuous sounds.
	VideoServices::SoundServer::ApplyContinuousPlay();
}

void GameScene::ScheduleLoad(std::shared_ptr<Loader> loader)
{
	auto rulebook = rules->GetRulebook();

	loader->AddLoader("Track and players", [=]{
		// Load the selected track
		std::shared_ptr<Model::Track> track;

		auto entry = this->rules->GetTrackEntry();
		if (!entry) throw Parcel::ObjStreamExn("Track does not exist.");
		track = Config::GetInstance()->GetTrackBundle()->OpenTrack(entry);
		if (!track) throw Parcel::ObjStreamExn("Track does not exist.");
		if (!session->LoadNew(entry->name.c_str(), scripting,
			track, &display.GetLegacyDisplay()))
		{
			throw Parcel::ObjStreamExn("Track load failed.");
		}

		// This must be done after the track has loaded.
		if (!session->CreateMainCharacter(0)) {
			throw Exception("Main character creation failed");
		}

		//TODO: Support split-screen with multiple viewports.
		viewports.emplace_back(
			display,
			new Observer(),
			new Display::Hud(display, session->GetPlayer(0), track,
				Vec2(1280, 720)));
	});

	loader->AddLoader("Session", [=]{
		metaSession = rulebook->GetMetas().session(
			std::make_shared<SessionPeer>(scripting, session));
		metaSession->OnInit();
		session->SetMeta(metaSession);

		director.GetSessionChangedSignal()(metaSession);

		session->AdvancePhase(ClientSession::Phase::PREGAME);

		auto sessionPeer = metaSession->GetSession();
		sessionPeer->ForEachPlayer([&](std::shared_ptr<MetaPlayer> &player) {
			auto playerPeer = player->GetPlayer();
			//TODO: Look up the correct HUD for this player.
			playerPeer->SetHud(std::make_shared<HudPeer>(scripting, display,
				viewports.back().hud));
			player->OnJoined(metaSession);
		});
	});
}

void GameScene::OnFinishedLoading()
{
	finishedLoading = true;
}

void GameScene::SetHudVisible(bool visible)
{
	for (auto &viewport : viewports) {
		viewport.observer->SetHudVisible(visible);
		viewport.hud->SetVisible(visible);
	}
}

void GameScene::SetMuted(bool muted)
{
	this->muted = muted;
}

void GameScene::Advance(Util::OS::timestamp_t tick)
{
	SUPER::Advance(tick);

	if (!finishedLoading) return;

	session->Process();

	if (!firedOnRaceFinish && session->GetPlayer(0)->HasFinish()) {
		metaSession->GetSession()->GetPlayer(0)->OnFinish();
		OnRaceFinish();
		firedOnRaceFinish = true;
	}
	else if (!firedOnStart && session->GetPlayer(0)->HasStarted()) {
		metaSession->GetSession()->GetPlayer(0)->OnStart();
		firedOnStart = true;
	}

	// Update HUD state last, after game state is settled for this frame.
	for (auto &viewport : viewports) {
		viewport.hud->Advance(tick);
	}
}

void GameScene::PrepareRender()
{
	SUPER::PrepareRender();

	if (!finishedLoading) return;

	for (auto &viewport : viewports) {
		viewport.hud->PrepareRender();
	}
}

void GameScene::Render()
{
	if (!finishedLoading) return;

	auto cfg = Config::GetInstance();
	MR_SimulationTime simTime = session->GetSimulationTime();

	{
		VideoServices::VideoBuffer *videoBuf = &display.GetLegacyDisplay();
		VideoServices::VideoBuffer::Lock lock(*videoBuf);

		int i = 0;
		for (auto &viewport : viewports) {
			viewport.observer->RenderNormalDisplay(videoBuf, session,
				session->GetPlayer(i++),
				simTime, session->GetBackImage());
		}
	}

	if (cfg->runtime.enableHud) {
		for (auto &viewport : viewports) {
			if (viewport.hud->IsVisible()) {
				viewport.hud->Render();
			}
		}
	}

	// Trigger sounds.
	if (!muted) {
		int i = 0;
		for (auto &viewport : viewports) {
			viewport.observer->PlaySounds(session->GetCurrentLevel(),
				session->GetPlayer(i++));
		}
		VideoServices::SoundServer::ApplyContinuousPlay();
	}
}

void GameScene::OnRaceFinish()
{
	//TODO: Currently assuming only one player, so we go directly from PLAYING
	//      to DONE (AdvancePhase will ensure that the POSTGAME event is fired).
	session->AdvancePhase(ClientSession::Phase::DONE);
	director.GetSessionChangedSignal()(nullptr);
}

}  // namespace HoverScript
}  // namespace Client
