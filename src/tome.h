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
#ifndef __TOME_H__
#define __TOME_H__

/* INTERFACE FOR tome.c */

//DEFINES
#define		MAX_TOPIC	128

//TYPEDEFS
typedef struct tome_entry_s TOME_DATA;
typedef struct tome_topic_s TOME_TOPIC;

struct tome_entry_s {
  bool		canFree;		//set if allocated using malloc

  char		title[MIL];		//title of the tome
  char		author[MIL];		//name of author
  char*		text;			//pointer to text
  
  TOME_DATA*	next;
  TOME_TOPIC*	topic;		//pointer to the topic this is in
};

//Prototypes
void InitTomes();
void SaveTomes();
void EchoTome( TOME_DATA* newtome );
TOME_DATA* AddTome(char* subject, char* title, char* author, char* text);

#endif //__TOME_H__
