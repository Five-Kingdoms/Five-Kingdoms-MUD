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

/* Written by Virigoth sometime circa april 2000 for FORSAKEN LANDS mud.*/
/* This is the implementation of the selectable skills code		*/
/* NOT TO BE USED OR REPLICATED WITHOUT EXPLICIT PERMISSION OF AUTHOR	*/
/* umplawny@cc.umanitoba.ca						*/

/* Following defines some data types used by the whole selectable skills*
 * system								*/

#define MAX_SELECT		16	//How many choices allowed per class
#define MAX_GROUP_SKILL		16	//Max skills per group
#define MAX_PRE			4	//Max of skills used for requirement
#define MAX_EX			4	//Max of skills used for exclusion
/* 
The following three compose the main table used for reference when
doing the selection for skills.  It is referenced by the s_select_table
for data on each group 
*/

struct ss_info_type{
  char *name;		//name of skill
  sh_int rating;	//how easy it is to learn
  sh_int skill;		//start skill when selected
  char *pre[MAX_PRE];	//prerequisites for the skill
  char *ex[MAX_EX];	//skills wich exclude this choice
};

struct ss_group_type{
  char* name;					//name of group
  sh_int skills;					//How many skills to select from
  struct ss_info_type ss_info[MAX_GROUP_SKILL];	//data on each skill choice
};



/*
Following are used for the main s_select_table which contains data on each group
and amount of skills avaliable to particular set of skills
*/
struct ss_type{
  char*		name;
  sh_int	level[MAX_SELECT];	//level at which selections are made
  sh_int	picks[MAX_SELECT];	//how many total selections allowed
  char*		group[MAX_SELECT];	//name of group out of ss_group_table selections are made from.
};




