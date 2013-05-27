
// ConsoleScene.cpp
//
// Copyright (c) 2013 Michael Imamura.
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

#include "StdAfx.h"

#include "../../../engine/Display/Display.h"
#include "../../../engine/Display/FillBox.h"
#include "../../../engine/Display/Label.h"
#include "../GameDirector.h"

#include "ConsoleScene.h"

using namespace HoverRace::Util;

namespace HoverRace {
namespace Client {
namespace HoverScript {

namespace {
	static const std::string COMMAND_PROMPT(">> ");
	static const std::string CONTINUE_PROMPT(":> ");
}

class ConsoleScene::LogLines
{
	public:
		LogLines(Display::Display &display, const Display::Vec2 &charSize);
		~LogLines();

	public:
		void ScrollTop();
		void Scroll(int i);
		void ScrollBottom();

	public:
		void Add(const std::string &s, const Display::UiFont &font, Display::Color color);
		void Clear();

	public:
		void PrepareRender();
		void Render();

	private:
		Display::Display &display;

		const Display::Vec2 &charSize;
		std::deque<Display::Label*> lines;
		unsigned int pos;
		unsigned int num;

		static const int MAX_LINES = 20;
};

ConsoleScene::ConsoleScene(Display::Display &display, GameDirector &director,
                           SysConsole &console) :
	SUPER("Console"),
	display(display), director(director), console(console),
	layoutChanged(true), charSize(0, 0)
{
	typedef Display::UiViewModel::Alignment Alignment;

	Config *cfg = Config::GetInstance();

	displayConfigChangedConn = display.GetDisplayConfigChangedSignal().
		connect(std::bind(&ConsoleScene::OnDisplayConfigChanged, this));

	submitBuffer.reserve(1024);
	historyBuffer.reserve(1024);
	commandLine.reserve(1024);

	winShadeBox = new Display::FillBox(1280, 720, 0xbf000000);
	winShadeBox->AttachView(display);

	Display::UiFont font(cfg->GetDefaultMonospaceFontName(), 30);

	inputLbl = new Display::Label(COMMAND_PROMPT, font, 0xffffffff);
	inputLbl->SetPos(0, 719);
	inputLbl->SetAlignment(Alignment::SW);
	inputLbl->AttachView(display);

	measureLbl = new Display::Label(" ", font, 0xffffffff);
	measureLbl->AttachView(display);

	logClearedConn = console.GetLogClearedSignal().connect(
		std::bind(&ConsoleScene::OnLogCleared, this));
	logAddedConn = console.GetLogAddedSignal().connect(
		std::bind(&ConsoleScene::OnLogAdded, this, std::placeholders::_1));

	logLines = new LogLines(display, charSize);

	console.ReadLogs(std::bind(&ConsoleScene::AppendLogLine, this, std::placeholders::_1));
}

ConsoleScene::~ConsoleScene()
{
	delete logLines;

	delete measureLbl;
	delete inputLbl;
	delete winShadeBox;
}

void ConsoleScene::OnDisplayConfigChanged()
{
	layoutChanged = true;
}

void ConsoleScene::OnConsoleToggle()
{
	director.RequestPopScene();
}

void ConsoleScene::OnTextInput(const std::string &s)
{
	commandLine += s;
	UpdateCommandLine();
}

void ConsoleScene::OnTextControl(Control::TextControl::key_t key)
{
	switch (key) {
		case Control::TextControl::BACKSPACE:
			if (!commandLine.empty()) {
				commandLine.resize(commandLine.length() - 1);
				UpdateCommandLine();
			}
			break;

		case Control::TextControl::ENTER:
			console.LogHistory(inputLbl->GetText());
			console.SubmitChunk(commandLine);
			commandLine.clear();
			UpdateCommandLine();
			break;

		default:
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
				"ConsoleScene: Unhandled text control key: %d\n", key);
	}
}

void ConsoleScene::OnLogCleared()
{
	logLines->Clear();
}

void ConsoleScene::OnLogAdded(int idx)
{
	if (idx > lastLogIdx) {
		console.ReadLogs(lastLogIdx + 1, idx,
			std::bind(&ConsoleScene::AppendLogLine, this, std::placeholders::_1));
	}
}

void ConsoleScene::AppendLogLine(const SysConsole::LogLine &line)
{
	Display::Color color(0xff7f7f7f);
	switch (line.level) {
		case SysConsole::LogLevel::HISTORY: color = 0xffffffff; break;
		case SysConsole::LogLevel::INFO: color = 0xffbfbfbf; break;
		case SysConsole::LogLevel::ERROR: color = 0xffff0000; break;
		default:
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
				"ConsoleScene: Unhandled log level: %d\n", line.level);
	}

	// Colorize based on message type.
	logLines->Add(line.line, measureLbl->GetFont(), color);

	lastLogIdx = line.idx;
}

void ConsoleScene::UpdateCommandLine()
{
	inputLbl->SetText(
		(console.GetInputState() == Console::ISTATE_COMMAND ?
			COMMAND_PROMPT : CONTINUE_PROMPT) + commandLine);
}

void ConsoleScene::Layout()
{
	// Measure the size of a single character.
	// We assume that the monospace font we're using really has the same height
	// and width for all glyphs.
	const Display::Vec3 charSize3 = measureLbl->Measure();
	charSize.x = charSize3.x;
	charSize.y = charSize3.y;

	layoutChanged = false;
}

void ConsoleScene::AttachController(Control::InputEventController &controller)
{
	controller.AddConsoleToggleMaps();
	auto &consoleToggleAction = controller.actions.sys.consoleToggle;
	consoleToggleConn = consoleToggleAction->Connect(std::bind(&ConsoleScene::OnConsoleToggle, this));

	auto &textAction = controller.actions.ui.text;
	textInputConn = textAction->Connect(std::bind(&ConsoleScene::OnTextInput, this, std::placeholders::_1));

	auto &controlAction = controller.actions.ui.control;
	textControlConn = controlAction->Connect(std::bind(&ConsoleScene::OnTextControl, this, std::placeholders::_1));

	SDL_StartTextInput();
}

void ConsoleScene::DetachController(Control::InputEventController &controller)
{
	SDL_StopTextInput();

	textControlConn.disconnect();
	textInputConn.disconnect();
	consoleToggleConn.disconnect();
}

void ConsoleScene::Advance(Util::OS::timestamp_t tick)
{
	// Act like the starting and stopping phases don't even exist.
	switch (GetPhase()) {
		case Phase::STARTING:
			SetPhase(Phase::RUNNING);
			break;
		case Phase::STOPPING:
			SetPhase(Phase::STOPPED);
			break;
	}
}

void ConsoleScene::PrepareRender()
{
	if (layoutChanged) {
		Layout();
	}

	winShadeBox->PrepareRender();
	inputLbl->PrepareRender();
	logLines->PrepareRender();
}

void ConsoleScene::Render()
{
	winShadeBox->Render();
	inputLbl->Render();
	logLines->Render();
}

// LogLines ////////////////////////////////////////////////////////////////////

ConsoleScene::LogLines::LogLines(Display::Display &display,
                                 const Display::Vec2 &charSize) :
	display(display), charSize(charSize),
	lines(), pos(0), num(10)
{
}

ConsoleScene::LogLines::~LogLines()
{
	Clear();
}

void ConsoleScene::LogLines::ScrollTop()
{
	if (pos != 0) {
		pos = 0;
	}
}

void ConsoleScene::LogLines::Scroll(int i)
{
	if (i < 0) {
		if ((unsigned int)-i >= pos) {
			ScrollTop();
		}
		else {
			pos += i;
		}
	}
	else if (lines.size() > num) {
		if (pos + i > lines.size() - num) {
			ScrollBottom();
		}
		else {
			pos += i;
		}
	}
}

void ConsoleScene::LogLines::ScrollBottom()
{
	pos = (lines.size() <= num) ? 0 : (lines.size() - num);
}

void ConsoleScene::LogLines::Add(const std::string &s,
                                 const Display::UiFont &font,
                                 Display::Color color)
{
	// Remove the top line if we're full.
	if (lines.size() == MAX_LINES) {
		delete lines.front();
		lines.pop_front();
	}

	Display::Label *lbl = new Display::Label(s.empty() ? " " : s, font, color);
	lbl->SetAlignment(Display::UiViewModel::Alignment::SW);
	lbl->AttachView(display);

	lines.push_back(lbl);
	ScrollBottom();
}

void ConsoleScene::LogLines::Clear()
{
	for (auto iter = lines.begin(); iter != lines.end(); ++iter)
	{
		delete *iter;
	}
	lines.clear();
	pos = 0;
}

void ConsoleScene::LogLines::PrepareRender()
{
	double y = 720.0 - charSize.y;

	for (auto iter = lines.rbegin(); iter != lines.rend(); ++iter) {
		Display::Label *lbl = *iter;
		lbl->SetPos(0, y);
		lbl->PrepareRender();
		y -= charSize.y;
	}
}

void ConsoleScene::LogLines::Render()
{
	for (auto iter = lines.rbegin(); iter != lines.rend(); ++iter) {
		(*iter)->Render();
	}
}

}  // namespace HoverScript
}  // namespace Client
}  // namespace HoverRace
