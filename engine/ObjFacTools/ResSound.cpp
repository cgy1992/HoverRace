// ResSound.cpp
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

#include "../Parcel/ObjStream.h"

#include "ResSound.h"

using namespace HoverRace::VideoServices;

namespace HoverRace {
namespace ObjFacTools {

ResShortSound::ResShortSound(int pResourceId) :
	mResourceId(pResourceId),
	mSound(nullptr), mNbCopy(0), mDataLen(0), mData(nullptr)
{
}

ResShortSound::~ResShortSound()
{
	SoundServer::DeleteShortSound(mSound);
	delete[] mData;
}

void ResShortSound::Serialize(Parcel::ObjStream &pArchive)
{
	if(pArchive.IsWriting()) {
		pArchive << mNbCopy;
		pArchive << mDataLen;
		pArchive.Write(mData, mDataLen);
	}
	else {
		SoundServer::DeleteShortSound(mSound);
		mSound = nullptr;

		delete[] mData;
		mData = nullptr;

		pArchive >> mNbCopy;
		pArchive >> mDataLen;

		mData = new char[mDataLen];

		pArchive.Read(mData, mDataLen);

		mSound = SoundServer::CreateShortSound(mData, mNbCopy);

	}
}

ResContinuousSound::ResContinuousSound(int pResourceId) :
	mResourceId(pResourceId),
	mSound(nullptr), mNbCopy(0), mDataLen(0), mData(nullptr)
{
}

ResContinuousSound::~ResContinuousSound()
{
	SoundServer::DeleteContinuousSound(mSound);
	delete[] mData;
}

void ResContinuousSound::Serialize(Parcel::ObjStream &pArchive)
{
	if(pArchive.IsWriting()) {
		pArchive << mNbCopy;
		pArchive << mDataLen;
		pArchive.Write(mData, mDataLen);

	}
	else {
		SoundServer::DeleteContinuousSound(mSound);
		mSound = nullptr;

		delete[] mData;
		mData = nullptr;

		pArchive >> mNbCopy;
		pArchive >> mDataLen;

		mData = new char[mDataLen];

		pArchive.Read(mData, mDataLen);

		mSound = SoundServer::CreateContinuousSound(mData, mNbCopy);
	}
}

}  // namespace ObjFacTools
}  // namespace HoverRace
