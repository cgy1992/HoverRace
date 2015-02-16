
// SdlFillBoxView.cpp
//
// Copyright (c) 2013, 2015 Michael Imamura.
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

#include <SDL2/SDL.h>

#include "../FillBox.h"

#include "SdlFillBoxView.h"

namespace HoverRace {
namespace Display {
namespace SDL {

void SdlFillBoxView::Render()
{
	const Color color = model.GetColor();
	double border = model.GetBorder();
	const Color borderColor = model.GetBorderColor();

	const bool hasFill = color.bits.a > 0;
	const bool hasBorder = border > 0.0 && borderColor.bits.a > 0;

	if (!hasFill && !hasBorder) return;  // Nothing to draw.

	SDL_Renderer *renderer = display.GetRenderer();
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// Calculate the screen-space size and position.

	Vec2 pos = display.LayoutUiPosition(
		model.GetAlignedPos(model.GetSize().x, model.GetSize().y));

	const Vec2 &size = model.GetSize();
	double w = size.x;
	double h = size.y;
	if (!model.IsLayoutUnscaled()) {
		double uiScale = display.GetUiScale();
		w *= uiScale;
		h *= uiScale;
	}

	// Stash the calculated coordinates for UI hit-testing.
	screenPos = pos;
	screenSize.x = w;
	screenSize.y = h;

	SDL_Rect rect = {
		static_cast<int>(pos.x), static_cast<int>(pos.y),
		static_cast<int>(w), static_cast<int>(h) };

	if (hasFill) {
		SDL_SetRenderDrawColor(renderer,
			color.bits.r, color.bits.g, color.bits.b, color.bits.a);
		SDL_RenderFillRect(renderer, &rect);
	}

	if (hasBorder) {
		SDL_SetRenderDrawColor(renderer,
			borderColor.bits.r, borderColor.bits.g,
			borderColor.bits.b, borderColor.bits.a);

		int thick;
		if (model.IsLayoutUnscaled()) {
			thick = static_cast<int>(border);
		}
		else {
			double uiScale = display.GetUiScale();
			thick = static_cast<int>(border * uiScale);
		}

		if (thick <= 1) {
			// Draw a single wireframe rect.
			SDL_RenderDrawRect(renderer, &rect);
		}
		else {
			// Draw four quads for the border.
			const int sideH = rect.h - (thick * 2) - 1;
			SDL_Rect rects[4] = {
				{ rect.x, rect.y, rect.w, thick },
				{ rect.x, rect.y + thick, thick, sideH },
				{ rect.x, rect.y + rect.h - thick - 1, rect.w, thick },
				{ rect.x + rect.w - thick, rect.y + thick, thick, sideH },
			};
			SDL_RenderFillRects(renderer, rects, 4);
		}
	}
}

Vec3 SdlFillBoxView::Measure()
{
	return model.GetSize().Promote();
}

}  // namespace SDL
}  // namespace Display
}  // namespace HoverRace
