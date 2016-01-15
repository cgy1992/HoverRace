
// Engine.cpp
//
// Copyright (c) 2016 Michael Imamura.
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

#include <curl/curl.h>

#include "Util/OS.h"
#include "Util/WorldCoordinates.h"
#include "Exception.h"

#include "Engine.h"

using namespace HoverRace::Util;

namespace HoverRace {

namespace {

std::string globalModuleName;

}  // namespace

/**
 * Initialize the engine.
 * @param moduleName The name of the application.
 * @throw Exception An engine component failed to initialize.
 */
Engine::Engine(const std::string &moduleName)
{
	globalModuleName = moduleName;

	srand(static_cast<unsigned int>(time(nullptr)));

	auto curlErrCode = curl_global_init(CURL_GLOBAL_ALL);
	if (curlErrCode != 0) {
		std::ostringstream oss;
		oss << "libcurl initialization failed: " << curlErrCode;
		throw Exception(oss.str());
	}

	OS::TimeInit();

	MR_InitTrigoTables();
}

Engine::~Engine()
{
	OS::TimeShutdown();

	curl_global_cleanup();
}

const std::string &Engine::GetModuleName()
{
	return globalModuleName;
}

}  // namespace HoverRace
