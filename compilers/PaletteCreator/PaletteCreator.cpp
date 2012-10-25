// PaletteCreator.cpp
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
#include "stdafx.h"

#define VC_EXTRALEAN							  // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>								  // MFC core and standard components
#include <afxext.h>								  // MFC extensions
#include <afxtempl.h>

#include <typeinfo.h>
#include "math.h"

#include "../VideoServices/ColorPalette.h"
#include "../ColorTools/ColorTools.h"

// This program create a NeoPaint compatible palette int the file Colortab.pal
//
// To use this palette, you must create a pcx image with neopaint. This image should be loaded
// in PPSP. In PSP save the palette in a file. This palette should then be used
// for all the FireBall bitmaps
//
// The resulting palette is interesting but the color quantization by PSP is poor
// (but it is the only program that we have
//

// This files also create the file ColorTab.cpp
// This file should be included in the VideoServices project

// Local prototypes
static void CreatePaletteFile();
static void CreateColorTabFile();
static void InitColorTable(FILE * pOutput);
static void InitColorAdditionTable(FILE * pOutput);
static void InitBasicPaletteTable(FILE * pOutput);

// static MR_UInt8 GetNearestColor( double mRed, double mGreen, double mBlue );

void main()
{

	printf("FIREBALL project palette creator,         (C)Copyright 1996 GrokkSoft inc\n\n");
   printf(_("The palette will be put in the ColorTab.pal file.\n"));
   printf(_("The predefined tables will be placed in ColorTab.cpp\n"));

	MR_ColorTools::Init();

	CreatePaletteFile();

	CreateColorTabFile();

}

void CreatePaletteFile()
{
	int lColorIndex = 0;
	FILE *lOutput = fopen("ColorTab.pal", "w");

	// Print the header
	/* Old format.. components form 0 to 63
	   fprintf( lOutput, "NeoPaint Palette File\n"
	   "(C)1992 OSCS Software Development, Inc.\n"
	   "256\n" );
	 */
	// New format .. components 0 to 255
	fprintf(lOutput, "JASC-PAL\n" "0100\n" "256\n");

	// Create the color table
	// PALETTEENTRY* lTable = new PALETTEENTRY[MR_NB_COLORS-MR_RESERVED_COLORS];

	// fprintf( lOutput, "0 0 0\n" ); // A nice magenta, that should not be present if tha palette
	// lColorIndex++;

	// Create windows reserved colors
	for(; lColorIndex < MR_RESERVED_COLORS; lColorIndex++) {
		fprintf(lOutput, "255 0 255\n");		  // A nice magenta, that should not be present if tha palette
	}

	for(; lColorIndex < MR_ColorTools::GetNbColors() + MR_RESERVED_COLORS; lColorIndex++) {
		int lRed;
		int lGreen;
		int lBlue;

		MR_ColorTools::GetIntComponents((MR_UInt8) lColorIndex - MR_RESERVED_COLORS, lRed, lGreen, lBlue);
		fprintf(lOutput, "%d %d %d\n", lRed, lGreen, lBlue);

	}

	for(; lColorIndex < MR_NB_COLORS; lColorIndex++) {
		fprintf(lOutput, "255 0 255\n");		  // A nice magenta, that should not be present if tha palette
	}

	// Clean-up
	fclose(lOutput);

}

void CreateColorTabFile()
{
	FILE *lOutput = fopen("ColorTab.cpp", "w");

	fprintf(lOutput, "// ColorTab.cpp\n" "// \n" "// This file have been auto generated by the program PaletteCreator\n" "// This file should be add to the VideoServicesProject\n" "// \n" " \n" "#include \"stdafx.h\" \n" "#include \"ColorPalette.h\" \n" " \n");

	// Create the ColorTableVariable
	fprintf(lOutput, "MR_UInt8 MR_ColorTable[ MR_NB_COLOR_INTENSITY ][ MR_NB_COLORS ] = \n" "{\n ");

	InitColorTable(lOutput);

	fprintf(lOutput, "};\n\n");

	// Create the ColorAdditionTable variable

	fprintf(lOutput, "MR_UInt8 MR_ColorAdditionTable[ MR_NB_COLORS ][ MR_NB_COLORS ] = \n" "{\n ");

	InitColorAdditionTable(lOutput);

	fprintf(lOutput, "};\n\n");

	// Create MR_BasicPalette
	extern double MR_BasicPalette[][3];
	extern int MR_BasicPaletteSize;

	fprintf(lOutput, "int MR_BasicPaletteSize = %d;\n\n" "double   MR_BasicPalette[%d][ 3 ] = \n" "{\n ", MR_ColorTools::GetNbColors(), MR_ColorTools::GetNbColors()

		);

	InitBasicPaletteTable(lOutput);

	fprintf(lOutput, "};\n\n");

	// Ending banner
	fprintf(lOutput, " \n" "// End of the auto-generated file" " \n");

	fclose(lOutput);
}

void InitColorTable(FILE * pOutput)
{
	for(int lIntensity = 0; lIntensity < MR_NB_COLOR_INTENSITY; lIntensity++) {
		fprintf(pOutput, "{ ");

		for(int lColor = 0; lColor < MR_NB_COLORS; lColor++) {
			int lEntry = 0;

			if((lColor >= MR_RESERVED_COLORS) && (lColor < MR_ColorTools::GetNbColors() + MR_RESERVED_COLORS)) {
				double lRed;
				double lGreen;
				double lBlue;

				MR_ColorTools::GetComponents((MR_UInt8) lColor - MR_RESERVED_COLORS, lRed, lGreen, lBlue);

				lRed *= double (lIntensity) / double (MR_NORMAL_INTENSITY);
				lGreen *= double (lIntensity) / double (MR_NORMAL_INTENSITY);
				lBlue *= double (lIntensity) / double (MR_NORMAL_INTENSITY);

				lEntry = MR_ColorTools::GetNearest(lRed, lGreen, lBlue) + MR_RESERVED_COLORS;
			}

			fprintf(pOutput, "%d", lEntry);

			if(lColor + 1 < MR_NB_COLORS) {
				fprintf(pOutput, ", ");
			}
			else {
				fprintf(pOutput, " ");
			}

		}
		if(lIntensity + 1 < MR_NB_COLORS) {
			fprintf(pOutput, "},\n ");
		}
		else {
			fprintf(pOutput, "}\n ");
		}
	}
}

void InitColorAdditionTable(FILE * pOutput)
{
	for(int lColor1 = 0; lColor1 < MR_NB_COLORS; lColor1++) {
		fprintf(pOutput, "{ ");
		for(int lColor2 = 0; lColor2 < MR_NB_COLORS; lColor2++) {
			int lEntry = 0;

			if((lColor1 >= MR_RESERVED_COLORS) && (lColor1 < MR_ColorTools::GetNbColors() + MR_RESERVED_COLORS)
			&& (lColor2 >= MR_RESERVED_COLORS) && (lColor2 < MR_ColorTools::GetNbColors() + MR_RESERVED_COLORS)) {
				double lRed1;
				double lGreen1;
				double lBlue1;
				double lRed2;
				double lGreen2;
				double lBlue2;

				MR_ColorTools::GetComponents((MR_UInt8) lColor1 - MR_RESERVED_COLORS, lRed1, lGreen1, lBlue1);
				MR_ColorTools::GetComponents((MR_UInt8) lColor2 - MR_RESERVED_COLORS, lRed2, lGreen2, lBlue2);

				lRed1 = (lRed1 + lRed2) / 2.0;
				lGreen1 = (lGreen1 + lGreen2) / 2.0;
				lBlue1 = (lBlue1 + lBlue2) / 2.0;

				lEntry = MR_ColorTools::GetNearest(lRed1, lGreen1, lBlue1) + MR_RESERVED_COLORS;
			}

			fprintf(pOutput, "%d", lEntry);

			if(lColor2 + 1 < MR_NB_COLORS) {
				fprintf(pOutput, ", ");
			}
			else {
				fprintf(pOutput, " ");
			}

		}
		if(lColor1 + 1 < MR_NB_COLORS) {
			fprintf(pOutput, "},\n ");
		}
		else {
			fprintf(pOutput, "}\n ");
		}
	}
}

static void InitBasicPaletteTable(FILE * pOutput)
{
	for(int lCounter = 0; lCounter < MR_ColorTools::GetNbColors(); lCounter++) {
		double lRed;
		double lGreen;
		double lBlue;

		MR_ColorTools::GetComponents(lCounter, lRed, lGreen, lBlue);

		fprintf(pOutput, " { %f, %f, %f }", lRed, lGreen, lBlue);

		if(lCounter + 1 < MR_ColorTools::GetNbColors()) {
			fprintf(pOutput, ",\n ");
		}
		else {
			fprintf(pOutput, "\n ");
		}

	}

}
