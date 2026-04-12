#ifndef FCPUT_GRAPHICS_ANSISEQ_H
#define FCPUT_GRAPHICS_ANSISEQ_H

#ifndef FCPUT_GRAPHICS_ANSISEQ_NONAMESPACE

/////////////// CSI Commands ///////////////

/// @Brief Control Sequence Introducer (CSI); ESC[
#define FCPG_CSI "\x1b["

//// Select Graphics Rendition (SGR) commands
//// Control sequence: CSI n m
//// Sets display attributes

#define FCPG_RESET 				FCPG_CSI "0m"
#define FCPG_BOLD  				FCPG_CSI "1m"
#define FCPG_FAINT 				FCPG_CSI "2m"
#define FCPG_ITALIC 			FCPG_CSI "3m"
#define FCPG_UNDERLINE 		FCPG_CSI "4m"
#define FCPG_SLOWBLINK 		FCPG_CSI "5m"
#define FCPG_RAPIDBLINK 	FCPG_CSI "6m"
#define FCPG_REVERSEVIDEO FCPG_CSI "7m"
#define FCPG_CONCEAL 			FCPG_CSI "8m"
#define FCPG_CROSSEDOUT 	FCPG_CSI "9m"
#define FCPG_PRIMARY 			FCPG_CSI "10"
#define FCPG_FOREGROUND 	FCPG_CSI "38"
#define FCPG_BACKGROUND 	FCPG_CSI "48"

#define FCPG_FG_BLACK 	FCPG_CSI "30m"
#define FCPG_FG_RED 		FCPG_CSI "31m"
#define FCPG_FG_GREEN 	FCPG_CSI "32m"
#define FCPG_FG_YELLOW 	FCPG_CSI "33m"
#define FCPG_FG_BLUE 		FCPG_CSI "34m"
#define FCPG_FG_MAGENTA FCPG_CSI "35m"
#define FCPG_FG_CYAN 		FCPG_CSI "36m"
#define FCPG_FG_WHITE 	FCPG_CSI "37m"

// Utility macros
#define FCPG_WRITE_AS(str, ascii) ascii str FCPG_RESET

#else

/////////////// CSI Commands ///////////////

/// @Brief Control Sequence Introducer (CSI); ESC[
#define CSI "\x1b["

//// Select Graphics Rendition (SGR) commands

#define RESET 				CSI "0m"
#define BOLD  				CSI "1m"
#define FAINT 				CSI "2m"
#define ITALIC 			CSI "3m"
#define UNDERLINE 		CSI "4m"
#define SLOWBLINK 		CSI "5m"
#define RAPIDBLINK 	CSI "6m"
#define REVERSEVIDEO CSI "7m"
#define CONCEAL 			CSI "8m"
#define CROSSEDOUT 	CSI "9m"
#define PRIMARY 			CSI "10"
#define FOREGROUND 	CSI "38"
#define BACKGROUND 	CSI "48"

#define FG_BLACK 	CSI "30m"
#define FG_RED 		CSI "31m"
#define FG_GREEN 	CSI "32m"
#define FG_YELLOW 	CSI "33m"
#define FG_BLUE 		CSI "34m"
#define FG_MAGENTA CSI "35m"
#define FG_CYAN 		CSI "36m"
#define FG_WHITE 	CSI "37m"

// Utility macros
#define WRITE_AS(str, ascii) ascii str FCPG_RESET

#endif	// FCPUT_GRAPHICS_ANSISEQ_NONAMESPACE

#endif	// FCPUT_GRAPHICS_ANSISEQ_H
