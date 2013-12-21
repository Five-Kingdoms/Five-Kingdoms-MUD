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

/* DELARATIONS FOR jail.h */
/* Prevents sitting in JAILCELL past ABUSE_CHECK / (ABUSE_RATIO * 10) ticks */
#define	ABUSE_CHECK	40
#define ABUSE_RATIO	2
#define ABUSE_KICK	140	//kicks justice out of cell after ABUSE_KIKC / (ABUSE_RATIO * 10) ticks


/* TYPES OF INCARCERATION (stored in gen_jail.modifier*/
#define	JAIL_NONE	-1	//Used for killing gen if not in jail cell
#define	JAIL_RELEASE	0
#define	JAIL_TRANSFER	1
#define	JAIL_NORMAL	2
#define	JAIL_BAIL	3
#define	JAIL_EXTENDED	4
#define	JAIL_EXECUTE	5


/* DURATIONS OF JAIL TYPES */
#define	DUR_NORMAL	20
#define	DUR_EXTENDED	30
#define	DUR_BAIL	5
#define	DUR_EXECUTE	12
#define	DUR_RELEASE	0

/* SEVERITY MODIFIERS */
#define	SEV_EXTENDED	3	/* ticks */
#define	SEV_BAIL	10000	/* gold */



struct execution_type{
  char*	name;		//Name of execution
  char* announce;	//announcement to be fittied into a yell:" For his crimes against the Laws of Aabahran,\n\r 
			//<name> has been sentenced by <justice> <announce>."
  char* pre_act;	//string to be passed to "act" with , mob, NULL, ch, TO_ROOM,  1 tick before execution
			//Also passed to criminal right before execution
  char* on_act_r;	//string to be passed to "act" with , mob, NULL, ch, TO_NOTVICT,  AT execution
  char* on_act_c;	//string to be passed to "act" with , mob, NULL, ch, TO_VICT,  AT execution
  char* room_desc;	//String to be set as Room desc. of EXECUTION_OBJ created after execution.  
			//Single %s used for name of criminal.
};


extern const struct execution_type execution_table[];


/* functions */
int		show_crimes	( CHAR_DATA* ch, CHAR_DATA* victim, bool fPunish, bool fPenOnly );
AFFECT_DATA*	set_crime	( CHAR_DATA* ch, CHAR_DATA* victim, AREA_DATA* area, int crime );
bool		is_crime	( ROOM_INDEX_DATA* room, int crime, CHAR_DATA* victim );
int		crime_lookup	(const char *name );
int		get_random_rares( CHAR_DATA* ch, CHAR_DATA* victim, int number, bool fRandom );
