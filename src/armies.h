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

#include "misc.h"

#ifndef __ARMY_H__
#define __ARMY_H__

#define	SAVEARMY_DIR		"../savemud/"
#define	SAVEARMY_FILE		"armies.sav"
#define	SAVEARMY_TEMPFILE	"armies.tmp"

#define DEFENSE_AIWAIT		600	//seconds before AI defends
#define ARMY_DEFENSE_MULT	100
#define ARMY_DAMAGE_MULT	100
#define ARMY_NODEFENDER_MULT	50
#define	ARMY_HASH_SIZE		256
#define ARMY_HASH_KEY		251

/* MAX VALS */
#define MAX_ARMY_ID		512
#define ARMY_VARS		3
#define ARMY_MAXREPORT		16
#define ARMY_PATH_MAX	       1024

/* DEFINES for pcdata->armies COUNTS */
/*
  NOR   = normal armies in barracks
  UPG   = elite armies in barracks
  NORQ  = normal units queued up
  UPGQ  = elites units queued up
  TIMN  = ticks before normal units trained or -1 for pause
  TIMU  = ticks for elite units
  OTIN  = saved ticks while normal queue paused
  OTIU  = saved ticks while elites paused
*/
#define NOR	0
#define UPG	1
#define NORQ	2
#define UPGQ	3
#define TIMN	4
#define TIMU	5
#define OTIN	6
#define OTIU	7
#define MAX_TRAIN_QUEUE	8

/* army report types */
#define	REPORT_NEUT	0
#define	REPORT_GOOD	1
#define	REPORT_BAD	3

/* ARMY STATES */
#define	AS_NONE		0
#define	AS_GARR		1
#define	AS_FORT		2
#define	AS_PATR		3
#define	AS_ATTA		4
#define	AS_MOVE		5
#define	AS_RETU		6
#define	AS_LEAV		7
#define	AS_SHAD		8
#define	AS_FIGH		9
#define	AS_DISB		10
#define	AS_DEFE		11
#define	AS_CIRC		12
#define	AS_BESE		13
#define	AS_MAX		14

/* ARMY TYPES (index->type) */
#define ARMY_TYPE_UNIT		0
#define ARMY_TYPE_BASTION	(A)

/* ARMY FLAGS (index->army_flags) */
#define ARMY_FLAG_NONE		0
#define ARMY_FLAG_ELITE		(A)
#define ARMY_FLAG_FLYING	(B)
#define ARMY_FLAG_GARRISON	(C)
#define ARMY_FLAG_FOUGHT	(D)

/* ARMY_ROOM_DATA->armies DESIGNATIONS */
/*  0 1 2 3 4 5 
 *  ___________
 * |B|D|D|A|A|A|
 *  -----------
 */
#define	INROOM_BASTION		0
#define	INROOM_DEFENDERS	1
#define	INROOM_ATTACKERS	3
#define	INROOM_MAXARMIES	6


/* DURATIONS */
//NEW
#define	ARMY_DUR_REC		4
#define	ARMY_DUR_SREC		3
#define	ARMY_DUR_UPG		2
#define	ARMY_DUR_SUPG		1
#define	ARMY_DUR_FORTIFY	24

//OLD (remove)
#define	BUILD_ARMY_DUR		8
#define	UPGRADE_ARMY_DUR	12
#define	BUILD_TOWER_DUR		120 //1 hour
#define	UPGRADE_TOWER_DUR	360 //3 hours
#define	COMBAT_SIEGE_DUR	120 //1 hour
#define	COMBAT_ARMY_DUR		60  //0.5 hour
#define	CORRUPT_DUR		720 //6 hours

/* TYPE DEFS */
typedef struct report_queue REPORT_QUEUE;
typedef struct army_report ARMY_REPORT;
typedef struct army_data  ARMY_DATA;
typedef struct tower_data TOWER_DATA;       
typedef struct army_index_data ARMY_INDEX_DATA;
typedef struct army_room_data ARMY_ROOM_DATA;

/* MACROS */
#define	IS_ARMY( army, flag )		( (army)->army_flags & (flag) )
#define	ARMY_HASHVAL( army )	( (army)->ID % ARMY_HASH_KEY )
#define	ID_HASHVAL( ID )	( (ID) % ARMY_HASH_KEY )

#define GARR_RED( ch )		( (ch)->pCabal ? get_parent((ch)->pCabal)->armies_ready : 0 )
#define GARR_NOR( ch )		( IS_NPC((ch)) || !(ch)->pcdata->member ? 0 : (ch)->pcdata->member->armies[NOR])
#define GARR_NORQ( ch )		( IS_NPC((ch)) || !(ch)->pcdata->member ? 0 : (ch)->pcdata->member->armies[NORQ])
#define GARR_UPG( ch )		( IS_NPC((ch)) || !(ch)->pcdata->member ? 0 : (ch)->pcdata->member->armies[UPG])
#define GARR_UPGQ( ch )		( IS_NPC((ch)) || !(ch)->pcdata->member ? 0 : (ch)->pcdata->member->armies[UPGQ])
#define GARR_TOT( ch )		( GARR_NOR((ch)) + GARR_UPG((ch)) )
#define GET_LETTER( pa )	( IS_ARMY( (pa), ARMY_FLAG_ELITE) ? UPPER((pa)->pCabal->who_name[2]) : LOWER((pa)->pCabal->who_name[2]))

#define SET_FOCUS( ch, focus )		( (ch)->pcdata->army_focus = (focus))
#define SET_LASTFOCUS( ch, focus )	( (ch)->pcdata->last_focus = (focus))
#define SET_QFOCUS(pc, focus)		( get_parent((pc))->last_focus = focus)
#define GET_FOCUS( ch )			( (ch)->pcdata->army_focus )
#define GET_LASTFOCUS( ch )		( (ch)->pcdata->last_focus )
#define GET_QFOCUS( pc )		( get_parent((pc))->last_focus )


#define AKEY( key, field, val)						\
if (!str_cmp( word, key)){						\
  (field)	=	(val);						\
  fMatch = TRUE;							\
  break;								\
}

#define AKEYS( key, field, val)						\
if (!str_cmp( word, key)){						\
  free_string( (field) );						\
  (field)	=	(val);						\
  fMatch = TRUE;							\
  break;								\
}


/* STRUCTS */
struct army_report{
  ARMY_REPORT*	next;
  ARMY_REPORT*	prev;
  time_t	time;
  char*		string;
};

struct report_queue{
  ARMY_REPORT	top;
  ARMY_REPORT	bot;
  int		size;
};



struct army_index_data{
  ARMY_INDEX_DATA*	next;
  AREA_DATA*		area;		//area this index is in
  int			vnum;		//army vnum
  char*			noun;		//Demons (used in short cabal updates)
  char*			short_desc;	//A Demon Horde
  char*			long_desc;	//A pack of demons is here.
  char*			desc;		//Detailed description
  sh_int		type;		//One of ARMY_TYPE (bastion, army etc.)
  int			cost;		//cost to raise
  int			support;	//% of each bastion's support share
  int			off_dice[3];	//offensive dice
  int			hit_dice[3];	//hp dice
  int			arm_dice[3];	//armor dice
  int			army_flags;	//special flags
};

struct army_data{
  ARMY_DATA*		next;
  ARMY_INDEX_DATA*	pIndexData;	//Original index data
  ARMY_DATA*		attacking;	//army we are attacking
  PATH_QUEUE*		path;		//path this army will move along
  CABAL_DATA*	pCabal;		//cabal this belongs to
  ROOM_INDEX_DATA*	in_room;	//room this army is in if any
  int			in_slot;	//in room_armies slot if any
  
  char*			commander;	//Name of commander, otherwise cabal's
  sh_int		command_rank;	//commander's cabal rank
  char*			noun;		//Demons (used in short cabal updates)
  char*			short_desc;	//A Demon Horde
  char*			long_desc;	//A pack of demons is here.
  char*			desc;		//Detailed description

  word			ID;		//ID of unit
  sh_int		order;		//The original order to the unit
  int			ovars[ARMY_VARS];//Original order's variables
  int			vars[ARMY_VARS];//state variables if any
  sh_int		state;		//Current army state
  int			lifetime;	//army ticks before next state decision
  int			cabal_ai_life;  //army ticks before next cabal_ai deci.

  int			src_room;	//start room vnum for path when its set
  int			att_slot;	//for fix_armies to repoint attackers

  int			off_dice[3];	//offensive dice
  int			hit;		//current hitpoints
  int			max_hit;	//max hit
  int			armor;		//current armor
  int			max_armor;	//max armor
  int			army_flags;	//flags
};

/* holds info about armies in room */
struct army_room_data{
  ARMY_DATA*	armies[INROOM_MAXARMIES];
  int		count;	//armies in slots.
  int		bastions;//how many bastions in slots
  int		defenders;//how many defenders in slots
  int		attackers;//how many attackers in slots
};

/* holds description/constant/lifetime of state */
struct state_table_s {
  char* name;
  int	bit;
  int	lifetime;
};

/* used for keeping track of towers in area */
struct tower_data{
  TOWER_DATA*		next;

  CABAL_DATA*	pCabal;
  AREA_DATA*	        pArea;

  int			towers;
  bool			fRein;	//does area have reinforcements
};

  
ARMY_INDEX_DATA*	new_army_index	( void );
ARMY_INDEX_DATA*	get_army_index	( int vnum );

void	refresh_area_support	( );
void	reinforce_refresh	( CABAL_DATA* pCabal );
bool	can_reinforce		( CABAL_DATA* pCabal, AREA_DATA* area );
void	area_to_cabal		( AREA_DATA* area, CABAL_DATA* pc, bool fSilent );
void	area_from_cabal		( AREA_DATA* area,  bool fSilent );
bool	break_alliance_check	( CABAL_DATA* pCab, CABAL_DATA* pExAlly );

void	save_armies		( void );
void	load_armies		( void );

void	add_army_index		( ARMY_INDEX_DATA* pai );
void	show_army_index		( CHAR_DATA* ch, ARMY_INDEX_DATA* pai );
void	save_army_indexes	( FILE *fp, AREA_DATA *pArea );
void	fread_army_indexes	( FILE* fp, AREA_DATA* pArea );
void	recruit_army		( CHAR_DATA* ch, bool fElite );
void	update_garrison		( CHAR_DATA*  ch, int nor, int upg );
void	update_garrison_cm	( CMEMBER_DATA*  cm, int nor, int upg );
bool	check_army_max		( CMEMBER_DATA* cm );

int	get_army_count		( char* name );
int	get_max_army		( int level );
TOWER_DATA* get_tower_data	( CABAL_DATA* pCabal, AREA_DATA* area );
void	army_update		( void );
void	show_room_armies	( CHAR_DATA* ch, ARMY_ROOM_DATA* pArd );
void	examine_room_armies	( CHAR_DATA* ch, ARMY_ROOM_DATA* pArd );
void	army_battle_echo	( ROOM_INDEX_DATA* room );

void	army_report		( ARMY_DATA* pa, char* string, int type, bool fSave );
void	cabal_report		( CABAL_DATA* pc, char* string, int type, bool fSave );
void	init_repq		( REPORT_QUEUE* q);


bool	army_interpret		( CHAR_DATA* ch, char* argument );
ARMY_DATA* get_army_room	( CHAR_DATA* ch, ROOM_INDEX_DATA* room,char* name);



#endif


