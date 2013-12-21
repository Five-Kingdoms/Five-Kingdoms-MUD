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
#ifndef __AQUEST_H__
#define __AQUEST_H__

//defines
enum quest_type{
  QUEST_NONE = 0,
  QUEST_SLAY,
  QUEST_GET,
  QUEST_GIVE,
  QUEST_FIND,

  QUEST_MAX,
};

enum quest_level{
  LEVEL_EASY,
  LEVEL_MEDIUM,
  LEVEL_HARD
};

#define QUEST_ARG_MAX		10


typedef struct autoquest_type  AQUEST_DATA;

struct autoquest_type{
  int quest;    //type of the quest, QUEST_NONE for no quest
  int origin;	//vnum of the mob who gives quest/reward
  int target;	//vnum of the mob which is the target of the quest
  int arg;	//argument if any to the quest
  long life;	//time by which it has to be completed (non zero for time limited)

  bool fClan;	//does the quest reward with clan points
};

//MACROS
#define HAS_AQUEST(ch)		(!IS_NPC((ch)) && (ch)->pcdata->aquest.quest > QUEST_NONE)

//INTERFACE
void		aquest( CHAR_DATA* ch, char* argument);
void		cquest( CHAR_DATA* ch, char* argument);
char*		GetQuestString( AQUEST_DATA* pQ );
bool		QuestRoomCheck( CHAR_DATA* ch, ROOM_INDEX_DATA* room );
bool		QuestKillCheck(CHAR_DATA* ch, CHAR_DATA* victim);
bool		QuestTimeCheck(CHAR_DATA* ch );

#endif
