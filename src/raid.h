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


#ifndef __RAID_H__
#define __RAID_H__


#define RAID_WAIT	7200	//wait between raids in area in seconds
#define CAB_RAID_WAIT	3600	//wait between raids from same cabal


//types of raids
#define RAID_NORMAL	-1
#define RAID_UNDEAD_1	0
#define RAID_UNDEAD_2	1
#define RAID_UNDEAD_3	2
#define RAID_UNDEAD_4	3
#define RAID_DEMON_1	4
#define RAID_DEMON_2	5
#define RAID_DEMON_3	6
#define RAID_DEMON_4	7
#define RAID_NATURE_1	8
#define RAID_NATURE_2	9
#define RAID_NATURE_3	10
#define RAID_NATURE_4	11
#define RAID_MAX	12

typedef struct raid_data RAID_DATA;

struct raid_data{
  CABAL_DATA* pCab;
  int mob_vnum;		//mob skeleton to spawn
  int mob_total;	//total mobs to spawn
  int lifetime;		//how long to spawn for

  int mob_spawned;	//mobs spawned so far
  int duration;		//duration so far

  int act;		//act flags to add
  int race;
  int attack;
  char* name;		//custom strings
  char* short_desc;
  char* long_desc;
};

struct raid_strings{
  int	act;
  char*	race;
  char* attack;
  char* name;		//custom strings
  char* short_desc;
  char* long_desc;  
};

void raid_update( AREA_DATA* pArea, RAID_DATA* pr );
void raidmob_check( CHAR_DATA* vch, CHAR_DATA* victim);

#endif
