// ResSound.h
//
//
// Copyright (c) 1995-1998 - Richard Langlois and Grokksoft Inc.
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

#include "../VideoServices/SoundServer.h"
#include "../Util/MR_Types.h"

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
	namespace Parcel {
		class ObjStream;
	}
}

namespace HoverRace {
namespace ObjFacTools {

class MR_DllDeclare ResShortSound
{
protected:
	int mResourceId;
	VideoServices::ShortSound *mSound;

	int mNbCopy;
	MR_UInt32 mDataLen;
	char *mData;

public:
	// Only available for resourceLib and construction
	ResShortSound(int pResourceId);
	~ResShortSound();

	int GetResourceId() const { return mResourceId; }
	void Serialize(Parcel::ObjStream &pArchive);

	VideoServices::ShortSound *GetSound() const { return mSound; }
};

class MR_DllDeclare ResContinuousSound
{
protected:
	int mResourceId;
	HoverRace::VideoServices::ContinuousSound *mSound;

	int mNbCopy;
	MR_UInt32 mDataLen;
	char *mData;

public:
	// Only available for resourceLib and construction
	ResContinuousSound(int pResourceId);
	~ResContinuousSound();

	int GetResourceId() const { return mResourceId; }
	void Serialize(Parcel::ObjStream &pArchive);

	VideoServices::ContinuousSound *GetSound() const { return mSound; }
};

}  // namespace ObjFacTools
}  // namespace HoverRace

#undef MR_DllDeclare
