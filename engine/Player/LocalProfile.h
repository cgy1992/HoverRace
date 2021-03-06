
// LocalProfile.h
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

#pragma once

#include "Profile.h"

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
	namespace Util {
		namespace yaml {
			class MapNode;
		}
	}
}

namespace HoverRace {
namespace Player {

/**
 * A profile that is stored on the local filesystem.
 * @author Michael Imamura
 */
class MR_DllDeclare LocalProfile : public Profile
{
	using SUPER = Profile;

public:
	LocalProfile() = delete;
	LocalProfile(std::shared_ptr<AvatarGallery> avatarGallery);
	LocalProfile(std::shared_ptr<AvatarGallery> avatarGallery,
		const boost::uuids::uuid &uid);
	virtual ~LocalProfile() { }

public:
	bool isLoaded() const { return loaded; }

	std::shared_ptr<Display::Res<Display::Texture>> GetAvatar() const override;

private:
	void Load(Util::yaml::MapNode *root, const std::string &filename);

public:
	EditableProfile *Edit() override { return this; }
	RenamableProfile *EditName() override { return this; }

protected:
	void Save() override;

private:
	bool loaded;
};

}  // namespace Player
}  // namespace HoverRace

#undef MR_DllDeclare
