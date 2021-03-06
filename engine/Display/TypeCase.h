
// TypeCase.h
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

#include <SDL2/SDL.h>

#include "Color.h"

#include "UiFont.h"

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
namespace Display {

class TypeCase;
class TypeLine;

/**
 * A single glyph in the backing texture.
 * @author Michael Imamura
 */
class GlyphEntry
{
public:
	GlyphEntry() : cp(0), advance(0), page(0) { }

public:
	bool IsInitialized() const { return cp > 0; }

public:
	MR_UInt32 cp;  ///< The Unicode character point represented by this glyph.
	int advance;  ///< The width of the glyph when placed next to other glyphs.
	unsigned int page;  ///< Index of the texture that contains the character.
	SDL_Rect srcRect;  ///< The bounds of the glyph in the texture.
};

/**
 * Character-by-character text renderer.
 * @author Michael Imamura
 */
class MR_DllDeclare TypeCase
{
public:
	/**
	 * Constructor.
	 * @param font The font.
	 * @param width The width of the backing texture.
	 * @param height The height of the backing texture.
	 */
	TypeCase(const UiFont &font, int width, int height) :
		font(font), width(width), height(height) { }
	TypeCase(const TypeCase&) = delete;

	virtual ~TypeCase() { }

	TypeCase &operator=(const TypeCase&) = delete;

public:
	/**
	 * Retrieve the width of each of the backing textures.
	 * @return The width, in pixels.
	 */
	int GetWidth() const { return width; }

	/**
	 * Retrieve the height of each of the backing textures.
	 * @return The height, in pixels.
	 */
	int GetHeight() const { return height; }

	/**
	 * Count the number of active backing textures.
	 * @return The total number of textures (may be zero).
	 */
	virtual MR_UInt32 CountTextures() const = 0;

public:
	/**
	 * Prepare a string to be rendered.
	 *
	 * If @p rects is omitted, then this can be used to preemptively add
	 * characters to the type case before they are used later.
	 *
	 * @param s The text string that will be rendered.
	 * @param[out] rects Optional destination buffer for the prepared text.
	 */
	virtual void Prepare(const std::string &s, TypeLine *rects = nullptr) = 0;

	/**
	 * Render a string.
	 *
	 * Only characters which have been previously added via AddChars() will
	 * be rendered.
	 *
	 * @param s The string to render.
	 * @param color The color of the string.
	 * @param x The screen X coordinate of the upper-left corner.
	 * @param y The screen Y coordinate of the upper-left corner.
	 * @param caret The optional caret position.
	 */
	virtual void Render(const TypeLine &s, const Color color, int x, int y,
		boost::optional<size_t> caret = {}) = 0;

	/**
	 * Renders the backing texture (for debugging purposes).
	 * @param idx The texture index.
	 * @param x The x coordinate.
	 * @param y The y coordinate.
	 * @param scale The scaling factor.
	 */
	virtual void RenderTexture(MR_UInt32 idx, int x, int y,
		double scale = 1.0) = 0;

protected:
	const UiFont font;
	const int width;
	const int height;
};

/**
 * A line of text, prepared by a TypeCase.
 *
 * This is intended to be a reusable buffer; i.e., it can be cleared and a
 * new prepared line of text loaded in place.
 *
 * @author Michael Imamura
 */
class MR_DllDeclare TypeLine
{
public:
	TypeLine() : width(0), height(0) { }

public:
	/**
	 * Check if this instance has been prepared.
	 * @return @c true if prepared, @c false if not.
	 */
	bool IsPrepared() const {
		return static_cast<bool>(typeCase);
	}

	/**
	 * Render this line using the owning TypeCase.
	 * @param color The color of the string.
	 * @param x The screen X coordinate of the upper-left corner.
	 * @param y The screen Y coordinate of the upper-left corner.
	 * @param caret The optional caret position.
	 */
	void Render(const Color color, int x, int y,
		boost::optional<size_t> caret = {})
	{
		if (typeCase) {
			typeCase->Render(*this, color, x, y, caret);
		}
	}

public:
	std::shared_ptr<TypeCase> typeCase;  // Owning TypeCase, for sanity checking.
	std::vector<std::pair<GlyphEntry*, SDL_Rect>> glyphs;
	int width;
	int height;
};

}  // namespace Display
}  // namespace HoverRace

#undef MR_DllDeclare
