/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  * 
 *                                                                         *
 *      ROM 2.4 is copyright 1993-1998 Russ Taylor                         *
 *      ROM has been brought to you by the ROM consortium                  *
 *          Russ Taylor (rtaylor@hypercube.org)                            *
 *          Gabrielle Taylor (gtaylor@hypercube.org)                       *
 *          Brian Moore (zump@rom.org)                                     *
 *      By using this code, you have agreed to follow the terms of the     *
 *      ROM license, in the file Rom24/doc/rom.license                     *
 *                                                                         *
 * Code Adapted and Improved by Abandoned Realms Mud                       *
 * and Aabahran: The Forsaken Lands Mud by Virigoth                        *
 *                                                                         *
 * Continued Production of this code is available at www.flcodebase.com    *
 ***************************************************************************/

#ifndef _ANSI_
#define _ANSI_

#define ANSI_ESCAPE "[0"
#define ANSI_NORMAL ";0"
#define ANSI_UNDERL ";4"
#define ANSI_BLINK  ";5"
#define ANSI_INVERS ";7"

#define ANSI_BLACK  ";30"
#define ANSI_RED    ";31"
#define ANSI_GREEN  ";32"
#define ANSI_YELLOW ";33"
#define ANSI_BLUE   ";34"
#define ANSI_PURPLE ";35"
#define ANSI_CYAN   ";36"
#define ANSI_WHITE  ";37"
#define ANSI_BOLD_BLACK  ";1;30"
#define ANSI_BOLD_RED    ";1;31"
#define ANSI_BOLD_GREEN  ";1;32"
#define ANSI_BOLD_YELLOW ";1;33"
#define ANSI_BOLD_BLUE   ";1;34"
#define ANSI_BOLD_PURPLE ";1;35"
#define ANSI_BOLD_CYAN   ";1;36"
#define ANSI_BOLD_WHITE  ";1;37"
                            
#define ANSI_BLINK_BLACK  ";5;30"
#define ANSI_BLINK_RED    ";5;31"
#define ANSI_BLINK_GREEN  ";5;32"
#define ANSI_BLINK_YELLOW ";5;33"
#define ANSI_BLINK_BLUE   ";5;34"
#define ANSI_BLINK_PURPLE ";5;35"
#define ANSI_BLINK_CYAN   ";5;36"
#define ANSI_BLINK_WHITE  ";5;37"
#define ANSI_BLINK_BOLD_BLACK  ";1;5;30"
#define ANSI_BLINK_BOLD_RED    ";1;5;31"
#define ANSI_BLINK_BOLD_GREEN  ";1;5;32"
#define ANSI_BLINK_BOLD_YELLOW ";1;5;33"
#define ANSI_BLINK_BOLD_BLUE   ";1;5;34"
#define ANSI_BLINK_BOLD_PURPLE ";1;5;35"
#define ANSI_BLINK_BOLD_CYAN   ";1;5;36"
#define ANSI_BLINK_BOLD_WHITE  ";1;5;37"
                            
#define ANSI_UNDER_BLACK  ";4;30"
#define ANSI_UNDER_RED    ";4;31"
#define ANSI_UNDER_GREEN  ";4;32"
#define ANSI_UNDER_YELLOW ";4;33"
#define ANSI_UNDER_BLUE   ";4;34"
#define ANSI_UNDER_PURPLE ";4;35"
#define ANSI_UNDER_CYAN   ";4;36"
#define ANSI_UNDER_WHITE  ";4;37"
#define ANSI_UNDER_BOLD_BLACK  ";1;4;30"
#define ANSI_UNDER_BOLD_RED    ";1;4;31"
#define ANSI_UNDER_BOLD_GREEN  ";1;4;32"
#define ANSI_UNDER_BOLD_YELLOW ";1;4;33"
#define ANSI_UNDER_BOLD_BLUE   ";1;4;34"
#define ANSI_UNDER_BOLD_PURPLE ";1;4;35"
#define ANSI_UNDER_BOLD_CYAN   ";1;4;36"
#define ANSI_UNDER_BOLD_WHITE  ";1;4;37"

#define ANSI_BLINK_UNDER_BLACK  ";5;4;30"
#define ANSI_BLINK_UNDER_RED    ";5;4;31"
#define ANSI_BLINK_UNDER_GREEN  ";5;4;32"
#define ANSI_BLINK_UNDER_YELLOW ";5;4;33"
#define ANSI_BLINK_UNDER_BLUE   ";5;4;34"
#define ANSI_BLINK_UNDER_PURPLE ";5;4;35"
#define ANSI_BLINK_UNDER_CYAN   ";5;4;36"
#define ANSI_BLINK_UNDER_WHITE  ";5;4;37"
#define ANSI_BLINK_UNDER_BOLD_BLACK  ";1;5;4;30"
#define ANSI_BLINK_UNDER_BOLD_RED    ";1;5;4;31"
#define ANSI_BLINK_UNDER_BOLD_GREEN  ";1;5;4;32"
#define ANSI_BLINK_UNDER_BOLD_YELLOW ";1;5;4;33"
#define ANSI_BLINK_UNDER_BOLD_BLUE   ";1;5;4;34"
#define ANSI_BLINK_UNDER_BOLD_PURPLE ";1;5;4;35"
#define ANSI_BLINK_UNDER_BOLD_CYAN   ";1;5;4;36"
#define ANSI_BLINK_UNDER_BOLD_WHITE  ";1;5;4;37"
                            
#define ANSI_CLS    "[2J"
#define ANSI_HOME   "[1;1H"
#endif

char *color_table[]=
{
  ANSI_NORMAL,
  ANSI_RED,
  ANSI_GREEN,
  ANSI_YELLOW,
  ANSI_BLUE,
  ANSI_PURPLE,
  ANSI_CYAN,
  ANSI_WHITE,
  ANSI_BLACK,
  ANSI_BOLD_RED,
  ANSI_BOLD_GREEN,
  ANSI_BOLD_YELLOW,
  ANSI_BOLD_BLUE,
  ANSI_BOLD_PURPLE,
  ANSI_BOLD_CYAN,
  ANSI_BOLD_WHITE,
  ANSI_BOLD_BLACK,
  ANSI_BLINK_RED,
  ANSI_BLINK_GREEN,
  ANSI_BLINK_YELLOW,
  ANSI_BLINK_BLUE,
  ANSI_BLINK_PURPLE,
  ANSI_BLINK_CYAN,
  ANSI_BLINK_WHITE,
  ANSI_BLINK_BLACK,
  ANSI_BLINK_BOLD_RED,
  ANSI_BLINK_BOLD_GREEN,
  ANSI_BLINK_BOLD_YELLOW,
  ANSI_BLINK_BOLD_BLUE,
  ANSI_BLINK_BOLD_PURPLE,
  ANSI_BLINK_BOLD_CYAN,
  ANSI_BLINK_BOLD_WHITE,
  ANSI_BLINK_BOLD_BLACK,
  ANSI_UNDER_RED,
  ANSI_UNDER_GREEN,
  ANSI_UNDER_YELLOW,
  ANSI_UNDER_BLUE,
  ANSI_UNDER_PURPLE,
  ANSI_UNDER_CYAN,
  ANSI_UNDER_WHITE,
  ANSI_UNDER_BLACK,
  ANSI_UNDER_BOLD_RED,
  ANSI_UNDER_BOLD_GREEN,
  ANSI_UNDER_BOLD_YELLOW,
  ANSI_UNDER_BOLD_BLUE,
  ANSI_UNDER_BOLD_PURPLE,
  ANSI_UNDER_BOLD_CYAN,
  ANSI_UNDER_BOLD_WHITE,
  ANSI_UNDER_BOLD_BLACK,
  ANSI_BLINK_UNDER_RED,
  ANSI_BLINK_UNDER_GREEN,
  ANSI_BLINK_UNDER_YELLOW,
  ANSI_BLINK_UNDER_BLUE,
  ANSI_BLINK_UNDER_PURPLE,
  ANSI_BLINK_UNDER_CYAN,
  ANSI_BLINK_UNDER_WHITE,
  ANSI_BLINK_UNDER_BLACK,
  ANSI_BLINK_UNDER_BOLD_RED,
  ANSI_BLINK_UNDER_BOLD_GREEN,
  ANSI_BLINK_UNDER_BOLD_YELLOW,
  ANSI_BLINK_UNDER_BOLD_BLUE,
  ANSI_BLINK_UNDER_BOLD_PURPLE,
  ANSI_BLINK_UNDER_BOLD_CYAN,
  ANSI_BLINK_UNDER_BOLD_WHITE,
  ANSI_BLINK_UNDER_BOLD_BLACK,
  0
};
  
