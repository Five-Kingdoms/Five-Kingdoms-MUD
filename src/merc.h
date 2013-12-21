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

/* Accommodate old non-Ansi compilers. */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#define DECLARE_SONG_FUN( fun )		void fun( )
#define DECLARE_MOB_FUN( fun )		void fun( )
#define DECLARE_OBJ_FUN( fun )		void fun( )
#define DECLARE_ROOM_FUN( fun )		void fun( )
#define DECLARE_EFF_UPDATE( fun )       int  fun( )

#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#define DECLARE_SONG_FUN( fun )		SONG_FUN  fun
#define DECLARE_EFF_UPDATE( fun )       EFF_UPDATE_FUN fun
#define DECLARE_MOB_FUN( fun )		OBJ_FUN	  fun
#define DECLARE_OBJ_FUN( fun )		OBJ_FUN	  fun
#define DECLARE_ROOM_FUN( fun )		ROOM_FUN  fun

#endif


#include <crypt.h>


/* system calls */
int unlink();
int system();

/* Short scalar types.                                     *
 * Diavolo reports AIX compiler has bugs with short types. */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int				sh_int;
typedef int				bool;
#define unix
#else
typedef short    int			sh_int;
typedef unsigned char			bool;
#endif
typedef unsigned char                   byte;
typedef unsigned int                    word;

#include "s_skill.h"

/* Structure types. */
typedef struct	trap_data		TRAP_DATA;
typedef struct	trap_index_data		TRAP_INDEX_DATA;
typedef struct	cabal_index_data	CABAL_INDEX_DATA;
typedef struct	cabal_data		CABAL_DATA;
typedef struct	cabal_room_data		CROOM_DATA;
typedef struct	cabal_vroom_data	CVROOM_DATA;
typedef struct	cabal_skill_data	CSKILL_DATA;
typedef struct	cabal_member_data	CMEMBER_DATA;
typedef struct	cabal_vote_data		CVOTE_DATA;
typedef struct	bounty_data		BOUNTY_DATA;
typedef struct  _quest_node		QUEST_DATA;
typedef struct	bbid_data		BBID_DATA;
typedef struct	vote_data		VOTE_DATA;
typedef struct	affect_data		AFFECT_DATA;       //REGULAR AFFECT
typedef struct  effect_type             EFFECT_DATA;       //GEN EFFECT.
typedef struct	area_data		AREA_DATA;
typedef struct	ban_data		BAN_DATA;
typedef struct 	buf_type	 	BUFFER;
typedef struct	alias_type		ALIAS_DATA;
typedef struct	char_data		CHAR_DATA;
typedef struct	descriptor_data		DESCRIPTOR_DATA;
typedef struct	exit_data		EXIT_DATA;
typedef struct	extra_descr_data	EXTRA_DESCR_DATA;
typedef struct	help_data		HELP_DATA;
typedef struct	kill_data		KILL_DATA;
typedef struct	mob_index_data		MOB_INDEX_DATA;
typedef struct	note_data		NOTE_DATA;
typedef struct	obj_data		OBJ_DATA;
typedef struct	obj_index_data		OBJ_INDEX_DATA;
typedef struct	pc_data			PC_DATA;
typedef struct	reset_data		RESET_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	shop_data		SHOP_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	weather_data		WEATHER_DATA;
typedef struct	affect_disp_data	AFF_DISPLAY;



//OLD
typedef struct  mob_prog_data           MPROG_DATA;
//NEW
typedef struct  prog_list               PROG_LIST;
typedef struct  prog_code               PROG_CODE;

typedef struct  obj_spell_data          OBJ_SPELL_DATA;
typedef struct  challenge_data          CHALLENGE_DATA;
typedef struct  obj_message_data	OBJ_MESSAGE_DATA;
typedef struct  trigfile_data		TRIGFILE_DATA;
typedef struct	skill_data		SKILL_DATA;

// Struct double_list
// added by Athaekeetha
// A generic doubly linked list using void pointers so that it can hold
// pretty much a list of anything.  Have fun :)
struct Double_List;

typedef struct _Double_List {
  void * cur_entry;                 // current list element, cast to whatever you put in
  struct _Double_List * next_node;  // initialize when created, gcc wont!
  struct _Double_List * prev_node;  // same goes here!
} Double_List;

/* Function types. */
typedef	void DO_FUN	args( ( CHAR_DATA *ch, char *argument ) );
typedef bool SPEC_FUN	args( ( CHAR_DATA *ch ) );
typedef void SPELL_FUN  args( ( int sn, int level, CHAR_DATA *ch, void *vo, int target ) );
typedef void SONG_FUN   args( ( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument ) );
typedef void OBJ_FUN	args( ( OBJ_DATA *obj, char *argument ) );
typedef void ROOM_FUN	args( ( ROOM_INDEX_DATA *room, char *argument ) );

//Function for gen effects
typedef int EFF_UPDATE_FUN args ( (CHAR_DATA* ch, void* arg1, void* arg2, int gn, int* int1, int* int2, int flag) );

/* String and memory management parameters. */
#define	MAX_PERM_BLOCK		120000
#define MAX_KEY_HASH             1024
#define MAX_STRING_LENGTH	 4608
#define MAX_ROOM_VNUM		 32769
#define MSL			 4608
#define MAX_INPUT_LENGTH	  256
#define MIL			  256
#define PAGELEN			   22
#define LOG_MALLOC		TRUE
#define OBJCOUNT_PFILE_NUMBER	1500	//number of pfiles for 1:1 ratio of rares
#define STUPID_NUMBER_OF_SPELLS  1000


//Attack On Sight
#define AOS_LEVEL		 200		//Number of race members required to kill before they attack on sight
#define AOS_KILLS_PER_LEVEL	 50		//kills for each increase in hostilities
#define AOS_IMPROVE_TICKS	 8		//ticks before 1 kill is removed from race_kills

/* Game parameters.                                  *
 * Increase the max'es if you add more of something. *
 * Adjust the pulse numbers to suit yourself.        */
#define MAX_CABAL		  32
#define MAX_SOCIALS		  256
#define MAX_SKILL                 896
#define MAX_SECT		  13
#define MAX_PSALM                 32	//maximum amount of psalms in the game
#define START_PSALM               8	//starting max of psalms for character
#define MAX_TRAPVAL		  5	//number of integer misc vals in traps
#define MAX_TRAPS		  2	//number of traps/char
#define MAX_FREE_VNUM		256	//max number of free vnums the mud will keep track of
#define MAX_HOURS		24	//number of hours in a day
#define MAX_DAYS		10	//number of the days in the week
#define MAX_WEEKS		6	//number of weeks in the month
#define MAX_MONTHS		6	//number of months in a year
#define MAX_AFF_DISPLAY		32	//number of custom affects to allow

#define MAIN_PORT		6666
#define TEST_PORT		6665

/* VERSION FOR SAVE FILES */
#define SAVE_VERSION		3001

/* TOTAL SUPPORT IN THE WORLD */
#define WORLD_SUPPORT_VALUE	10000	//used to calculate support value for each area
#define CABAL_FULL_SUPPORT	25	//percent of world support
#define CABAL_FULL_SUPPORT_VAL	2500	//percent of world support
#define CABAL_UNDERDOG_SUP	1500	//support under which cabal is given underdog attack bonus

/* Safety counter for loops */
#define OUTPUT_SAFETY		 1000	//comm.c game_loop_unix

//The end of gsn and begginning of gen is decided by MAX_SKILL!

#define MAX_EFF_UPDATE             32    //Ceiling on different flags.
#define MAX_EFFECTS                128    //Ceiling on amount of gen's
//the end of gen is decided by MAX_EFFECTS!

#define MAX_VALUE1                  16   //Max length of the int. arrays.
#define MAX_VALUE2                  1    //Max length of the int. arrays.
#define MAX_VALUE3                  1    //Max length of the int. arrays.

#define MAX_MALFORM		    9    //Max level of malforemd weapon.
#define MAX_SONG                   64
#define MAX_GROUP		   30
#define MAX_GROUPS		   32	//Maximum various groups of s_skills in the mud
#define MAX_IN_GROUP		   15
#define MAX_CLASS                  32
#define MAX_CLASS_NOW              18
#define MAX_PC_RACE                34
#define MAX_RACE		   67
#define MAX_ANATOMY		   10
#define MAX_ADVENTURER_TIME	   288000


/* number of crooms in a cabal index */
#define CROOM_LEVELS	12

/* AVATAR effects maxes */
#define MAX_AVATAR		7       //maximum levels avatar can achive.
#define MAX_AVATAR_CMD		4       //number of commands the "call" executes
#define DIVFAVOR_GAIN		1000    //divine favor worth of each kill
#define DIVFAVOR_LOSS		-1000   //divine favor worth of each death
#define DIVFAVOR_COST		-1      //BASE amount of favor list per tick at level 0

/* AWAKEN LIFE STATE CONSTANTS */
#define AWAKENLIFE_NONE	0
#define AWAKENLIFE_RAMP	1
#define AWAKENLIFE_DEST	2
#define AWAKENLIFE_MOVE	3
#define AWAKENLIFE_RETR	4


/* CRUSADER CRUSADE CONST */
#define CRUSADE_RACE		0
#define CRUSADE_CLASS		1
#define CRUSADE_CABAL		2

/* WEAPON ENHANCEMENT */
#define ENHANCE_NONE		0
#define ENHANCE_BASIC		1
#define ENHANCE_JUGGER		2

/* CONTINGENCY IDENTIFIERS */
#define CONT_NONE		0
#define CONT_DISPEL		(A)
#define CONT_SUMMON		(B)
#define CONT_ATTACK		(C)
#define CONT_ATTACKED		(D)
#define CONT_HP30		(E)
#define CONT_MN30		(F)

/* SEASONS */
#define	SEASON_SPRING		0
#define	SEASON_SUMMER		1
#define	SEASON_FALL		2
#define	SEASON_WINTER		3

/* bits for dispel/cancel/interrupt checks on skills */
#define GN_STATIC		0	//cannot be removed, must expire
#define GN_DISPEL		(A)	//can be dispelled
#define GN_CANCEL		(B)	//can be cancelled
#define GN_INTERRUPT		(C)	//can be physicly dispelled (haymaker)
#define GN_NODEATH		(D)	//is not removed on death
#define GN_EXTEND		(E)	//can be extended by various spells
#define GN_NO_DND		(F)	//cannot be memorized with DND system
#define GN_PSI			(G)	//removed if pos < resting
#define GN_PSI_WEE		(H)	//psi amp success if saves > -15
#define GN_PSI_MED		(I)	//psi amp success if saves > -30
#define GN_PSI_STR		(J)	//psi amp success if saves > -45
#define GN_NOCAST		(K)	//cannot be normaly cast
#define GN_BENEFICIAL		(L)	//beneficial spell
#define GN_HARMFUL		(M)	//bad spell



//combinations of GN bits
#define GN_ANYDISP		(GN_DISPEL | GN_CANCEL | GN_INTERRUPT )
#define GN_NOCANCEL		(GN_DISPEL | GN_INTERRUPT )

//shorthands
#define GN_INT			GN_INTERRUPT
#define GN_STA			GN_STATIC
#define GN_BEN			(GN_BENEFICIAL | GN_ANYDISP | GN_EXTEND)
#define GN_HAR			(GN_HARMFUL | GN_NOCANCEL)
#define GN_ANY			GN_ANYDISP



/* these are used for check_crusade in fight.c */
#define CRUSADE_NONE		0
#define CRUSADE_MATCH		1
#define CRUSADE_NOTMATCH	2

/* used for weapon positions */
#define WEPPOS_NORMAL		0
#define WEPPOS_HIGH		1
#define WEPPOS_LOW		2

/* used for prefered battle positions */
#define BATPOS_MIDDLE		0
#define BATPOS_LEFT		1
#define BATPOS_RIGHT		2

/* CRIMES constants */
#define MAX_CRIME               6
#define CRIME_SHEATH		0
#define CRIME_LOOT		1
#define CRIME_THEFT		2
#define CRIME_ASSAULT		3
#define CRIME_MURDER		4
#define CRIME_OBSTRUCT		5


#define CRIME_FLAG_L		(A)
#define CRIME_FLAG_T		(B)
#define CRIME_FLAG_A		(C)
#define CRIME_FLAG_M		(D)
#define CRIME_FLAG_S		(E)
#define CRIME_FLAG_O		(F)

#define CRIME_ALLOW		0
#define CRIME_LIGHT		1
#define CRIME_NORMAL		2
#define CRIME_HEAVY		3
#define CRIME_DEATH		4


/* spell cancellation constants */


/* CABAL POINTS CONSTANTS */

#define MAX_TATTOO                 9
#define MAX_DIETY		   20//maximum number of dieties avaliable.
#define MAX_HOMETOWN               14
#define MAX_CELL		   3//max number of cells in a jail.
#define MAX_DAMAGE_MESSAGE        110
#define MAX_LEVEL		   60
#define MAX_QUEST		   32
#define LEVEL_HERO		   (MAX_LEVEL - 10)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 9)

#define PULSE_PER_SECOND	    4
#define PULSE_AGGR		    2
#define PULSE_VIOLENCE		  ( 3 * PULSE_PER_SECOND)
#define PULSE_MOBILE		  (10 * PULSE_PER_SECOND)
#define PULSE_TICK                (30 * PULSE_PER_SECOND)
#define PULSE_HALFTICK            (PULSE_TICK / 2)
#define PULSE_AREA                (60 * PULSE_PER_SECOND)
#define PULSE_ARMY                (15 * PULSE_PER_SECOND)
#define PULSE_DATA                (300 * PULSE_PER_SECOND)
#define PULSE_MSAVE               (900 * PULSE_PER_SECOND)
#define PULSE_ARMYSAVE            (900 * PULSE_PER_SECOND)
#define PULSE_LOGIN		    2
#define PULSE_REBOOT		   300 * PULSE_AREA
#define PULSE_SEED		   120 * PULSE_AREA
#define PULSE_VOTE		   PULSE_REBOOT / 2
#define PULSE_CABAL		   PULSE_TICK

/* REBOOT FLAGS */
#define	REBOOT_COLOR		(A)
#define	REBOOT_PURGE		(B)
#define	REBOOT_WIPE		(C)
#define	REBOOT_COUNT		(D)
#define	REBOOT_HELPS		(E)
#define	REBOOT_CABAL		(F)


#define IMPLEMENTOR             (MAX_LEVEL    )
#define	CREATOR			(MAX_LEVEL - 1)
#define SUPREME			(MAX_LEVEL - 2)
#define DEITY			(MAX_LEVEL - 3)
#define GOD			(MAX_LEVEL - 4)
#define IMMORTAL		(MAX_LEVEL - 5)
#define DEMI			(MAX_LEVEL - 6)
#define ANGEL			(MAX_LEVEL - 7)
#define AVATAR			(MAX_LEVEL - 8)
#define MASTER			(MAX_LEVEL - 9)
#define HERO			LEVEL_HERO

#define LEVEL_NEWBIE		5

/* Cabal Ranks */
#define RANK_MAX		6
#define MAX_CLANR		RANK_MAX
#define RANK_LEADER		5
#define RANK_ELDER		4
#define RANK_TRUSTED		3
#define RANK_NEWBIE		0

/* HELP FILE CONSTANTS */
/* Defines */
#define			HELP_ALL	-1
#define			HELP_CLASS	-2
#define			HELP_RACE	-3
#define			HELP_CABAL	-4
#define			HELP_PSALM	-5

/* path finding */
#define BFS_ERROR	   -1
#define BFS_ALREADY_THERE  -2
#define BFS_NO_PATH	   -3

/* Site ban structure. */
#define BAN_SUFFIX		A
#define BAN_PREFIX		B
#define BAN_NEWBIES		C
#define BAN_ALL			D
#define BAN_PERMIT		E
#define BAN_PERMANENT		F

/*continuum message defs*/
#define CONTINUUM_NONE      0
#define CONTINUUM_DEATH     1
#define CONTINUUM_ATTACK    2
#define CONTINUUM_HURT      3

/* Different types of shielding against WAIT_STATE */
#define LAG_SHIELD_NONE		0
#define LAG_SHIELD_PROT		1//protective shield
#define LAG_SHIELD_FFIELD	2//Forcefield
#define LAG_SHIELD_RFIELD	3//Reflectivefield

#define LAG_SHIELD_BALANCE	10//iron Balance
#define LAG_SHIELD_IWILL	11//iron will
#define LAG_SHIELD_ARMOR	12//Crusader armor enhancement

#define LAG_SHIELD_NONCORP	20//Mist form etc.




/* The system flags of gen effects, stored in flag1 */
#define        EFF_F1_NONE         0     //No flags.
#define        EFF_F1_NOSAVE       (A)     //Effect is not saved.
#define        EFF_F1_NOEXTRACT    (B)     //UPDATE_KILL isn't run on extraction.
#define        EFF_F1_ALLOW_EXTEND (C)     //Spell is a normal spell effect and may be extended for benefit.
#define        EFF_F1_CMDBLOCK     (D)     //Blocks Commands from execution (high priority).
#define        EFF_F1_NODEATH      (E)     //Not removed on death


/*effect update constants, passed to the effect_update function*/

#define EFF_UPDATE_NONE          0 //Just in case we need this.
#define EFF_UPDATE_INIT          1 //Called to set the effect.
#define EFF_UPDATE_KILL          2 //Called when effect is stripped by handler
#define EFF_UPDATE_ENTER         3 //On entering a room flagged SPEC_UPDATE
#define EFF_UPDATE_LEAVE         4 //On leaving the room with SPEC_UPDATE
#define EFF_UPDATE_PRECOMBAT     5 //At the begginging of a combat round
#define EFF_UPDATE_POSTCOMBAT    6 //At the end of combat round.
#define EFF_UPDATE_PREATTACK     7 //Before attack
#define EFF_UPDATE_POSTATTACK    8 //After attack
#define EFF_UPDATE_PRESPELL      9 //Before spell is cast.
#define EFF_UPDATE_POSTSPELL    10 //After spell (after spell function is run)
#define EFF_UPDATE_PRECOMMAND   11 //Before any command
#define EFF_UPDATE_POSTCOMMAND  12 //After any command
#define EFF_UPDATE_PREDAMAGE    13 //Before any damage is applied
#define EFF_UPDATE_POSTDAMAGE   14 //After any damage is applied
#define EFF_UPDATE_TICK         15 //On tick
#define EFF_UPDATE_MOBTICK      16 //On mob update tick
#define EFF_UPDATE_PREDEATH     17 //Right before death
#define EFF_UPDATE_POSTDEATH    18 //Right after death
#define EFF_UPDATE_PREKILL      19 //Before Kill
#define EFF_UPDATE_POSTKILL     20 //Right after kill.
#define EFF_UPDATE_PREEXP       21 //Before granting Exp/Gold
#define EFF_UPDATE_POSTEXP      22 //After granting Exp/Gold
#define EFF_UPDATE_PREVIOLENCE  23 //Before any combat
#define EFF_UPDATE_POSTVIOLENCE 24 //After any combat
#define EFF_UPDATE_OBJTICK	25 //Object tick update
#define EFF_UPDATE_OBJTOCHAR	26 //After obj is taken by character.
#define EFF_UPDATE_DODGE	27 //On successful dodge.
#define EFF_UPDATE_PARRY        28 //On successfull parry/dual parry/monk parry
#define EFF_UPDATE_SHBLOCK	29 //On successfull Shield block.
#define EFF_UPDATE_VIOTICK	30 //On violence tick (3sec)
#define EFF_UPDATE_MOVE		31 //On attempt to move out of room


/* Defs for obj spells */
#define SPELL_TAR_SELF		1
#define SPELL_TAR_VICT		2

/* Bit wise flags for align/ethos of dieties */
#define DIETY_ALIGN_EVIL	(A)
#define DIETY_ALIGN_GOOD	(B)
#define DIETY_ALIGN_NEUTRAL	(C)

#define DIETY_ETHOS_LAWFUL	(A)
#define DIETY_ETHOS_NEUTRAL	(B)
#define DIETY_ETHOS_CHAOTIC	(C)

#define PATH_LIFE		0
#define PATH_DEATH		1
#define PATH_CHANCE		2
#define PATH_EQUIL		3
#define PATH_KNOW		4
#define PATH_FAITH		5

#define ALIGN_EVIL		DIETY_ALIGN_EVIL
#define ALIGN_NEUTRAL		DIETY_ALIGN_NEUTRAL
#define ALIGN_GOOD		DIETY_ALIGN_GOOD

#define ETHOS_CHAOTIC		DIETY_ETHOS_CHAOTIC
#define ETHOS_NEUTRAL		DIETY_ETHOS_NEUTRAL
#define ETHOS_LAWFUL		DIETY_ETHOS_LAWFUL

/* DEFINES AS PASSED BACK BY DEITY LOOKUP */
/* based on archtype in the table*/
#define		DEITY_0		0	//One god
#define		DEITY_1		1	//Justice
#define		DEITY_2		2	//Warrior
#define		DEITY_3		3	//MAge
#define		DEITY_4		4	//Goods
#define		DEITY_5		5	//CHaos
#define		DEITY_6		6	//Undead

/* cabal flags */
#define		CABAL_NONE	0	//empty
#define		CABAL_CHANGED	(A)	//if changed in OLC
#define		CABAL_SUBCABAL	(B)	//if cabal is a sub cabal
#define		CABAL_CHAT	(C)	//if cabal has chat channel
#define		CABAL_CANHIDE	(D)	//if member can hide his cabal flag
#define		CABAL_CANSHOW	(E)	//if member can show his cabal flag
#define		CABAL_JUSTICE	(F)	//members are counted as police when crimes are checked
#define		CABAL_ROYAL	(G)	//cabal and its memebers are counted as royals (cabal name must be a homecity name)
#define		CABAL_SERVICE	(H)	//cabal always gives negative trade bonus to its partner
#define		CABAL_AREA	(I)	//cabal can own areas
#define		CABAL_COFFER	(J)	//cabal can store large amounts of cps
#define		CABAL_HIGHGAIN	(K)	//cabal members gain extra cp / tick
#define		CABAL_VRCHANGED	(L)	//if the virtual rooms have changed
#define		CABAL_HEARGUARD	(M)	//cabal can hear guards report criminals
#define		CABAL_GATE	(N)	//cabal is closed up when no members are about
#define		CABAL_GUARDMOB	(O)	//cabal has a standard guard


#define		CABAL_TRADE	(P)	//cabal has at least 1 trade pact
#define		CABAL_NAPACT	(Q)	//cabal has at least 1 non agression pact
#define		CABAL_ALLIANCE	(R)	//cabal has at least 1 alliance
#define		CABAL_VENDETTA	(S)	//cabal has at least 1 vendetta

#define		CABAL_SPONSOR	(T)	//does cabal require sponsorship

#define		CABAL_ALLOW_2	(U)	//allows cabal powers of given rank
#define		CABAL_ALLOW_3	(V)	//allows cabal powers of given rank
#define		CABAL_RAID	(W)	//allows cabal  to use raids
#define		CABAL_ALLOW_4	(X)	//allows cabal powers of given rank
#define		CABAL_ALLOW_5	(Y)	//allows cabal powers of given rank

#define		CABAL_ROUGE	(Z)	//cabal collects bounties and offers spy info
#define		CABAL_SPIES	(aa)	//cabal has inside info on other cabals and bounties
#define		CABAL_ELITE	(bb)	//cabal is allowed +2 max army units
#define		CABAL_SWARM	(cc)	//cabal has faster recruitment
#define		CABAL_NOMAGIC	(dd)	//cabal restricted on use of magic
#define		CABAL_HISTORY	(ee)	//cabal allows its allies histories


/* cabal flags2 */
#define		CABAL2_NEWBIE	(A)	//cabal allows its members newbie chat
#define		CABAL2_NOTEALL	(B)	//cabal allows its members notes to all
#define		CABAL2_AREAGAIN	(C)	//cabal allows its members notes to all


/* cabal room data flags */
#define		CROOM_NODUP	(A)	//cabal room cannot be built more then once
#define		CROOM_FIXEDNAME	(B)	//cabal room cannot have its name changed
#define		CROOM_PITROOM	(C)	//cabal room serves as storage for decayed corpses. First notake container is the pit

#include "tome.h"
#include "dndspell.h"
#include "armies.h"
#include "raid.h"
#include "autoquest.h"


struct critical_type{
  int min_anat;
  char* name;
};

struct  mob_prog_data
{
    MPROG_DATA *next;
    int         type;
    char *      arglist;
    char *      comlist;
    sh_int	dowhen;
};

struct prog_list
{
    int                 trig_type;
    char *              trig_phrase;
    int			vnum;
    char *              code;
    PROG_LIST *         next;
    bool                valid;
};

struct prog_code
{
    int			vnum;
    char *              code;
    PROG_CODE *         next;
};

struct struckdrunk
{
    sh_int  min_drunk_level;
    sh_int  number_of_rep;
    char    *replacement[11];
};

struct obj_spell_data
{
    OBJ_SPELL_DATA   *next;
    sh_int           spell;
    sh_int           target;
    sh_int	     percent;
    char	     *message;
    char	     *message2;
};

struct challenge_data
{
    CHALLENGE_DATA   *next;
    char	     *name;
    char	     *record;
    sh_int           win;
    sh_int           loss;
    sh_int	     tie;
    sh_int	     refused;
};


struct _quest_node{

  QUEST_DATA*		next;
  bool			valid;

  int			save;
  char*			quest;
};

struct trigfile_data
{
    TRIGFILE_DATA	*next;
    char		*name;
    sh_int		dowhen;
};

struct obj_message_data
{
    char	*onself;
    char	*onother;
    char	*offself;
    char	*offother;
};

struct	ban_data
{
    BAN_DATA *	next;
    bool	valid;
    sh_int	ban_flags;
    sh_int	level;
    char *	name;
    char *	text;
};



struct alias_type{
  ALIAS_DATA* next;	//next alias
  ALIAS_DATA* prev;	//prev alias
  char* name;		//name phrase for alias
  char* content;	//actual contents of the alias
};

struct buf_type
{
    BUFFER *    next;
    bool        valid;
    sh_int      state;  /* error state of the buffer */
    sh_int      size;   /* size in k */
    char *      string; /* buffer's string */
};

/* Time and weather stuff. */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

struct	time_info_data
{
    int		hour;
    int		day;
    int		month;
    int		year;
};

struct	weather_data
{
    sh_int		mmhg;
    sh_int		change;
    sh_int		sky;
    sh_int		sunlight;
};

/* mud data global */
struct mud_type{
  time_t        crash_time;		/* last crash	*/
  time_t        current_time;		/* time of this pulse */
  time_t        save_time;		/* time of last save */

  char		str_boot_time[MIL];
  char		str_save_time[MIL];

  struct time_info_data time_info;	/* mud day/hour etc. */
  struct weather_data weather_info;/* mud weather */

  int           mudport;		/* mudport */
  int           pfiles;			/* total pfiles above level 15 */
  int		bounties;		/* total of bounties collected */

  int           max_on;			/* most people on */
  int           deaths;			/* deaths so far */
  int           siege_iden;		/* special identifier for sieges */
  int		tot_bastion;		/* total of possible bastions in all loaded areas */


  bool		wizlock;		/* Is wizlocked	*/
  bool		newlock;		/* Is newlocked	*/

  char*		questor;		/* current questor */
  sh_int	max_quest;		/* max of quests ever */
  time_t	last_questor;		/* last login by the questor */
};


/* chargen states (HAVE to correspond to places in chargen_table) */
#define CGEN_GREET			1
#define CGEN_MAIN			2
#define CGEN_HELP			3
#define CGEN_ENTER			4
#define CGEN_CREATE			5
#define CGEN_PASS			6
#define CGEN_IMMPASS			7
#define CGEN_MOTD			8
#define CGEN_READY			9
#define CGEN_CON_NAME			10
#define CGEN_NEW_PASS			11
#define CGEN_CON_PASS			12
#define CGEN_NEW_LAST			13
#define CGEN_CON_LAST			14
#define CGEN_NEW_SEX			15
#define CGEN_NEW_RACE			16
#define CGEN_CON_RACE			17
#define CGEN_NEW_CLAS			18
#define CGEN_CON_CLAS			19
#define CGEN_NEW_ALIG			20
#define CGEN_CON_ALIG			21
#define CGEN_NEW_ETHO			22
#define CGEN_CON_ETHO			23
#define CGEN_ROLL			24
#define CGEN_NEW_RELI			25
#define CGEN_CON_RELI			26
#define CGEN_NEW_HOME			27
#define CGEN_CON_HOME			28
#define CGEN_ASK_DETA			29
#define CGEN_NEW_DONE			30
#define CGEN_CON_BREAK			31
#define CGEN_DET_BODY			32
#define CGEN_DET_FACE			33
#define CGEN_DET_SKIN			34
#define CGEN_DET_EYES			35
#define CGEN_DET_HAIR			36
#define CGEN_DET_HAIRL			37
#define CGEN_DET_HAIRT			38
#define CGEN_DET_HAIRF			39
#define CGEN_DET_PERK			40
#define CGEN_CON_PERK			41
#define CGEN_DET_HAND			42
#define CGEN_DET_AGE			43
#define CGEN_CON_AGE			44
#define CGEN_DET_WEAP			45
#define CGEN_NEW_TELE			46
#define CGEN_NEW_ENEM			47
#define CGEN_NEW_ADVE			48
#define CGEN_NAGSCREEN			49
#define CGEN_NAGLAG			50
#define CGEN_MAX			51

//used for the tables in detailed character generation options
#define CGEN_MAX_BODY			14
#define CGEN_MAX_FACE			14
#define CGEN_MAX_SKIN			 6
#define CGEN_MAX_HAIR			14
#define CGEN_MAX_HAIRL			10
#define CGEN_MAX_HAIRT			11
#define CGEN_MAX_HAIRF			10
#define CGEN_MAX_EYES			11


/* Connected state for a channel. */
#define CON_PLAYING                      0

#define CON_GET_NAME                     101
#define CON_GET_OLD_PASSWORD             102
#define CON_CONFIRM_NEW_NAME             103
#define CON_GET_NEW_PASSWORD             104
#define CON_CONFIRM_NEW_PASSWORD         105
#define CON_GET_FAMILY			 106
#define CON_CONFIRM_FAMILY		 107
#define CON_GET_NEW_RACE                 108
#define CON_GET_NEW_SEX                  109
#define CON_GET_NEW_CLASS                110
#define CON_GET_ALIGNMENT                112
#define CON_GET_ETHOS                    113
#define CON_GET_HOMETOWN                 114
#define CON_READ_IMOTD                   115
#define CON_READ_MOTD                    116
#define CON_GET_PURGE_PASSWORD		 118
#define CON_CONFIRM_IMMORTAL		 121
#define CON_QUICK_STATS                  124

#define CON_BREAK_CONNECT                117
#define CON_HOTREBOOT_RECOVER		 120
#define CON_SELECT                       122
#define CON_VOTE_BOOTH                   125
#define CON_VOTE			 126

/* Descriptor (channel) structure. */
struct	descriptor_data
{
    DESCRIPTOR_DATA *	next;
    Double_List *	snoop_by;
    CHAR_DATA *		character;
    CHAR_DATA *		original;
    char **             pString;
    int			outsize;
    char *		outbuf;
    char *		showstr_head;
    char *		showstr_point;
    char *		host;
    char *              ident;
    char		inbuf		[4 * MIL];
    char		incomm		[MIL];
    char		inlast		[MIL];
    sh_int		connected;
    void *              pEdit;
    sh_int              editor;
    int                 ifd;
    pid_t               ipid;
    int			timer;
    bool		valid;
    bool		fcommand;
    sh_int		descriptor;
    sh_int		repeat;
    int			outtop;
    sh_int              port;
    sh_int              ip;
};

/* Attribute bonus structures. */
struct	str_app_type
{
    sh_int	tohit;
    sh_int	todam;
    sh_int	carry;
    sh_int	wield;
};

struct	int_app_type
{
    sh_int	practice;
    sh_int	learn;
    sh_int	manap;
};

struct	wis_app_type
{
    sh_int	practice;
    sh_int	manap;
};

struct	dex_app_type
{
    sh_int	defensive;
};

struct	con_app_type
{
    sh_int	hitp;
    sh_int	shock;
};

/* TO types for act. */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3
#define TO_ALL		    4
#define TO_CHARVICT	    5
#define TO_IMMROOM	    6
#define TO_FORMAT	    7
#define TO_WATCHROOM	    8

/* weapon ratings */
#define	WEAPON_ATT		0
#define	WEAPON_DEF		1
#define	WEAPON_WITHDIS		2
#define	WEAPON_TOBEDIS		3
#define	WEAPON_WITHSDIS		4

/* weapon class */
#define WEAPON_EXOTIC		0
#define WEAPON_SWORD		1
#define WEAPON_DAGGER		2
#define WEAPON_SPEAR		3
#define WEAPON_MACE		4
#define WEAPON_AXE		5
#define WEAPON_FLAIL		6
#define WEAPON_WHIP		7
#define WEAPON_POLEARM		8
#define WEAPON_STAFF		9
#define	WEAPON_MAX		10


/* structures */


struct flag_type
{
    char *name;
    int bit;
    bool settable;
};

struct cabal_type
{
    char 	*name;
    char 	*who_name;
    int		cabal_vnum;
    char 	*currency;
    int		pit_room;
    int		pit_vnum;
    int		enemy_vnum;
    int		respawn[5];
    int         bastion_vnum;
};

struct deity_type
{
  char*		way;
  char*		god;
  sh_int	path;
  sh_int	align;
  sh_int	ethos;
  int		shrine;
  int		tattoo;
};

struct mal_deity_type
{
  char*		god;
};

struct mal_name_type
{
  char*	lower;
  char* higher;
};

struct tattoo_type
{
    char        *god;
    sh_int      vnum;
};

struct monk_type
{
    char	*wear;
    sh_int	value;
    sh_int	weight;
};

struct hometown_type
{
    char        *name;
  //temples
  int g_vnum;
  int n_vnum;
  int e_vnum;

  //pit room
  int pitr_g_vnum;
  int pitr_n_vnum;
  int pitr_e_vnum;

  //pit
  int      pit_g_vnum;
  int      pit_n_vnum;
  int      pit_e_vnum;

  //race
  char*		race;

};

struct jail_type
{
  char*	name;
  int	guardhouse;	//Room with JailGuard
			// (off_flags with guild_guard && grup = VNUM_JUSTICE
  int	entrance;	//Room right past the guardhouse for checking entry.
  int	cell[MAX_CELL];	//Jail cells (room2 JAIL_CELL)
  int	exe_mob;	//Executioner mob
  int	exe_room;	//Room for executions
};

struct position_type
{
    char *name;
    char *short_name;
};

struct mcharge_type
{
char* desc;
};

struct maxmcharge_type
{
char* desc;
};

struct room_color_type
{
  char ch;
};

struct study_msg_type
{
char* desc;
};

struct sex_type
{
    char *name;
};

struct ethos_type
{
    char *name;
};

struct anatomy_type
{
    char *name;
};

struct size_type
{
    char *name;
};

struct warlord_type
{
    char *name;
};

struct tarot_type
{
    char *name;
};

/* table with the nouns/verbs for lag attacks. */
struct lagattack_type
{
  sh_int*	gsn;
  char*		noun;//noun for the attack
  char*		to_char;//verb send to char. "your bash collides into"
  char*		to_vict;//verb sent to vict. "collides with your"
  char*		to_room;//verb to room "collides into"
  char*		ba_char;//message when balance is successfull.
  char*		ba_vict;
  char*		ba_room;
};


/* Help table types. */
struct	help_data
{
    HELP_DATA *	next;
    int		vnum;
    sh_int	level;
    sh_int	type;
    char *	keyword;
    char *	text;
    char *      ansi_text;
};

/* Shop types. */
#define MAX_TRADE	 5

struct	shop_data
{
    SHOP_DATA *	next;			/* Next shop in list		*/
    int		keeper;			/* Vnum of shop keeper mob	*/
    sh_int	buy_type [MAX_TRADE];	/* Item types shop will buy	*/
    sh_int	profit_buy;		/* Cost multiplier for buying	*/
    sh_int	profit_sell;		/* Cost multiplier for selling	*/
    sh_int	open_hour;		/* First opening hour		*/
    sh_int	close_hour;		/* First closing hour		*/
};

/* warning table composition */
struct warn_type
{
  char*	subj;
  char* text;
};

/* objseed table cell, used for seeding random objects into the game */
struct objseed_type
{
  int	vnum;		//vnum obj object to seed
  int	num;		//number of objects to seed
  int	chance;		//chance for object to be created and reset

  int	amin;		//minimum area vnum to seed in
  int	amax;		//max area vnum to seed in

  int rmin;		//minimum room vnum to seed in
  int rmax;		//max area room to seed in

  char* i_area[10];
  int ia_num;		//areas to select from using above

   char* e_area[10];
  int ea_num;		//areas to exclude from selection

  int i_sect[10];
  int is_num;		//sectors required to select

  int e_sect[10];
  int es_num;		//sectors that exclude a choice

  int i_rom1[32];
  int ir1_num;		//room1 bits required to select

  int e_rom1[32];
  int er1_num;		//room1 bits that exclude a choice

  int i_rom2[32];
  int ir2_num;		//room1 bits required to select

  int e_rom2[32];
  int er2_num;		//room1 bits that exclude a choice

  int pool;		//size of area pool to choose from
  bool fExit;		//TRUE = Exit required for selection
};

/* used for modify armor */
struct amodify_type {
  char*	name;		//name of modification
  int	level;		//level of sophistication
  int	bit;		//bit that signifies it
  int	ident;		//integer identifier to be used in amodify function
  int	dur;		//how long this takes to complete

  int	weight;		//percent weight increase (+/-) min 1 lb

  bool	fPart;		//does it needs part (same obj type)
  bool	fSame;		//does the part need to be from same object
  bool  fRare;		//does this need rare or unique parts?
  int	stones;		//how many start stones needed

  char* help;		//help text (one line)
};

/* used for modify weapon */
struct wmodify_type {
  char*	name;		//name of modification
  int	level;		//level of sophistication
  int	hlevel;		//level of holy weapon required
  int	bit;		//bit that signifies it
  int	ident;		//integer identifier to be used in amodify function
  int	dur;		//how long this takes to complete

  int	weight;		//percent weight increase (+/-) min 1 lb

  bool	fPart;		//does it needs part (same obj type)
  bool  fRare;		//does this need rare or unique parts?
  int	stones;		//how many start stones needed

  char* help;		//help text (one line)
};




/* Per-class stuff. */
#define MAX_GUILD      16
#define MAX_STATS 	6

/* STAT indexing constants */
#define STAT_STR 	0
#define STAT_INT	1
#define STAT_WIS	2
#define STAT_DEX	3
#define STAT_CON	4
#define STAT_LUCK	5

#define MAX_MODIFIER 	16	//Storest APPLY_XXXX modifiers with MOD_XXXX as index.
/* aff_mod indexing constants */
#define MOD_SPELL_LEVEL       0	/* Bonus to spell level on casting */
#define MOD_AFFL_LEVEL	      1	/* Bonus to spell level on casting */
#define MOD_MALED_LEVEL       2	/* Bonus to spell level on casting */
#define MOD_MENTAL_LEVEL      3	/* Bonus to spell level on casting */
#define MOD_SPELL_COST        4	/* % increase to spell cost 100 = 2x cost, -50 = 0.5 cost */
#define MOD_WAIT_STATE        5	/* % increase to spell lag, 100 = 2x lag, -50 = 0.5 lag */
#define MOD_SPELL_LEARN       6	/* % increase of chance to learn from non-spells (100% = double normal) */
#define MOD_SKILL_LEARN       7	/* % increase of chance to learn from spells (100% = double normal) */
#define MOD_HIT_GAIN	      8	/* hit recovery (100 = double normal) */
#define MOD_MANA_GAIN	      9	/* mana recovery (100 = double normal) */
#define MOD_MOVE_GAIN	      10/* move recovery (100 = double normal) */
#define MOD_AGE		      11/* age in years */



struct	class_type
{
    char *	name;			/* the full name of the class */
    char 	who_name	[4];	/* Three-letter name for 'who'	*/
    sh_int	attr_prime;		/* Prime attribute		*/
    sh_int	weapon;			/* First weapon			*/
    int		align;			/* Aligns allowed bits		*/
    int		guild[MAX_GUILD];	/* Vnum of guild entrance	*/
    sh_int	thac0_00;		/* Thac0 for level  0		*/
    sh_int	thac0_32;		/* Thac0 for level 32		*/
    sh_int	hp_min;			/* Min hp gained on leveling	*/
    sh_int	hp_max;			/* Max hp gained on leveling	*/
    bool        fMana;                  /* Class is magical             */
    bool        sMana;                  /* Class is semi-magical        */
    sh_int      extra_exp;              /* Extra experience per level   */
    int*	gcn;			/* pointer to game class number */
    char *	base_group;		/* base skills gained		*/
    char *	default_group;		/* default skills gained	*/
    sh_int	stats[MAX_STATS];	/* stat modifiers by class	*/
    char*	ss;			/* name of skill set if any	*/
};

struct item_type
{
    sh_int	type;
    char *	name;
};

struct thief_trap_type{
  char*		name;
  int		vnum;
  int		duration;
  char*		skill;

  bool		small;	//can trap be put on "take" object
  bool		exit;	//can trap be put on exit
  bool		obj;	//can trap be put on obj
};

struct weapon_type
{
    char *	name;
    sh_int	vnum;
    sh_int	type;
    sh_int	*gsn;
    sh_int	*gsn_mastery;
    sh_int	*gsn_expert;
};

struct sheath_type
{
  char *	class;	//class that this applies to
  bool		show;	//can others see the weapon sheathed
  bool		both;	//can class rearm itsself from Right sheath

  char *	l_str;	//left sheath string
  char *	r_str;	//right sheath string
};


struct weapon_rate_type
{
  sh_int	attack;		//offensive rating of the weapon
  sh_int	defense;	//defensive rating of the weapon
  sh_int	with_dis;	//effectivness at disarming weapons
  sh_int	tobe_dis;	//difficulaty of being disarmed
  sh_int	with_sdis;	//effectivness at disarming shields
};



struct wiznet_type
{
    char *	name;
    long 	flag;
    sh_int	level;
};

struct attack_type
{
    char *	name;			/* name */
    char *	noun;			/* message */
    sh_int   	damage;			/* damage class */
};

struct race_type
{
    char *	name;			/* call name of the race */
    bool	pc_race;		/* can be chosen by pcs */
    long	act2;			/* act bits for the race */
    long	act;			/* act bits for the race */
    long	aff;			/* aff bits for the race */
    long	off;			/* off bits for the race */
    long	imm;			/* imm bits for the race */
    long        res;			/* res bits for the race */
    long	vuln;			/* vuln bits for the race */
    long	form;			/* default form flag for the race */
    long	parts;			/* default parts for the race */
    int*	grn;			/* pointer to game race number */
};

/* additional data for pc races */
struct pc_race_type
{
    char *	name;			/* MUST be in race_type */
    char        who_name[5];            /* 5 letter name for who */
    char *	anatomy;		/* anatomy for monks */
    sh_int      class_mult;             /* exp per level */
    /* Kaslai: I DID SOMETHING DANGEROUS - Skills was 5 elements, I changed it to 10. */
    char *	skills[10];		/* bonus skills for the race */
    bool 	can_pick;
    bool 	show;			/* is the race shown to player in reputation screen etc. */
    sh_int 	stats[MAX_STATS];	/* stat modifiers by race */
    sh_int	max_stats[MAX_STATS];	/* maximum stats */
    sh_int	size;			/* aff bits for the race */
    sh_int      align;			/* possible aligns bits */
    sh_int      ethos;			/* possible ethos bits */
    sh_int	hit;			/* starting hp */
    sh_int	mana;			/* starting mana */
    sh_int	practice;		/* starting practice */
    int		lifespan;		/* maximum nautral age */
    char *      class_choice[MAX_CLASS];/* available classes */
};

struct spec_type
{
    char * 	name;			/* special function name */
    SPEC_FUN *	function;		/* the function */
};


struct avatar_type
{
  int	min_favor;	//minimum of divine favor needed for level.
  int	flags;		//any flags to be applied to ch->act for each level.
  char* cmd[MAX_AVATAR_CMD];		//commands avaliable each level to "call" command.
  char* desc;		//level description shown in "score"
};



bool    MOBtrigger;

#define ERROR_PROG        -1
#define IN_FILE_PROG       0
#define ACT_PROG           (A)
#define SPEECH_PROG        (B)
#define RAND_PROG          (C)
#define FIGHT_PROG         (D)
#define DEATH_PROG         (E)
#define HITPRCNT_PROG      (F)
#define ENTRY_PROG         (G)
#define GREET_PROG         (H)
#define ALL_GREET_PROG     (I)
#define GIVE_PROG          (J)
#define BRIBE_PROG         (K)
#define TIME_PROG	   (L)
#define GET_PROG	   (M)
#define DROP_PROG	   (N)
#define GUILD_PROG	   (O)
#define BLOODY_PROG	   (P)
#define JUSTICE_PROG	   (Q)
#define TELL_PROG	   (R)

/* Overlapping triggers for new progs */
#define TRIG_ACT           (A)
#define TRIG_SPEECH        (B)
#define TRIG_RANDOM        (C)
#define TRIG_FIGHT         (D)
#define TRIG_HPCNT	   (E)
#define TRIG_DEATH         (F)
#define TRIG_ENTRY         (G)
#define TRIG_GREET         (H)
#define TRIG_GRALL	   (I)
#define TRIG_BRIBE         (J)
#define TRIG_KILL          (K)
#define TRIG_DELAY	   (L)
#define TRIG_SURR          (M)
#define TRIG_EXIT          (N)
#define TRIG_EXALL         (O)
#define TRIG_GET	   (P)
#define TRIG_DROP	   (Q)
#define TRIG_SIT	   (R)
#define TRIG_GIVE          (S)
#define TRIG_TELL	   (U)
#define TRIG_USE	   (V)
#define TRIG_SPECIAL	   (W)
#define TRIG_LOOK	   (X)
#define TRIG_PUT	   (Y)
#define TRIG_SHOOT	   (Z)


/* Prog types */
#define PRG_MPROG	0
#define PRG_OPROG	1
#define PRG_RPROG	2


/* Data structure for notes. */
#define NOTE_NOTE	0
#define NOTE_IDEA       1
#define NOTE_PENALTY    2
#define NOTE_NEWS       3
#define NOTE_CHANGES    4
#define NOTE_APPLICATION 5

/* POSITIONS in the WARNING TABLE */
#define WARN_PK		0
#define WARN_PK_IMM	1 //same as belo but to imms
#define WARN_PK_END	2 //player pked exessivly
#define WARN_PK_VIO	3 //player pked on pk warning
#define WARN_LAWFUL	4 //city has become lawful
#define WARN_BOUNTY	5 //bounty fulfilled
#define WARN_GOODPK	6 //good aligned pking good aligned
#define WARN_TOME	7 //new tome is up for grabs


struct	note_data
{
    NOTE_DATA *	next;
    char *	sender;
    char *	date;
    char *	to_list;
    char *	subject;
    char *	text;
    bool 	valid;
    sh_int	type;
    sh_int	prefix;
    time_t  	date_stamp;
};

/* An affect. */
struct	affect_data
{
    AFFECT_DATA *	next;
    bool		valid;
    int			where;
    int			type;
    int			level;
    int			duration;
    int			location;
    int			modifier;
    int			bitvector;
    char*               string;
    bool                has_string;
    int			flags;
};

/* a trap */
struct	trap_data{
  TRAP_DATA *	next;
  TRAP_DATA *	next_trap;
  bool		valid;

  AREA_DATA*	area;
  TRAP_INDEX_DATA* pIndexData;

  CHAR_DATA*	owner;		//owner of the trap if any
  OBJ_DATA*	on_obj;		//obj on which trap is set
  EXIT_DATA*	on_exit;	//exit on which trap is set

  int		vnum;
  char*		name;
  char*		echo;		//custom echo to be shown to victim
  char*		oEcho;		//custom echo to be shown to room

  bool		armed;		//is it armed
  sh_int	age;		//age in ticks, use to remove traps
  sh_int	type;		//type of trap
  sh_int	level;		//level of trap
  sh_int	duration;	//duration (removed at 0 or ignored at -1)
  int		flags;		//trap flags if any
  int		value[MAX_TRAPVAL];	//misc values
};

/* a trap index*/
struct	trap_index_data{
  TRAP_INDEX_DATA *next;
  AREA_DATA*	area;
  int		vnum;
  char*		name;
  char*		echo;		//custom echo to be shown
  char*		oEcho;		//custom echo to be shown to room

  bool		valid;

  sh_int	type;		//type of trap
  sh_int	level;		//level of trap
  sh_int	duration;	//duration
  int		flags;		//trap flags if any
  int		value[MAX_TRAPVAL];	//misc values
};

/* cabal skill data */
struct cabal_skill_data{
  CSKILL_DATA*	next;
  SKILL_DATA*	pSkill;	//data on the actual skill

  int	min_rank;	//min rank to use
  int	max_rank;	//max rank to use
  int	min_cost;	//minimum amount of cp before it can be used
  int	cost;		//actual cost in cp's
  int	flag;		//flags if needed (not used right now)
};

/* cabal member data */
struct cabal_member_data{
  CMEMBER_DATA* next;
  CABAL_DATA* pCabal;

  char*	name;
  char* sponsor;
  char* sponsored;

  bool	connected;
  bool	garrison_changed;
  int	vnum;	//used to link cabal pointer

  int	class;
  int	race;
  int	level;
  int	rank;
  int	kills;		//used for bounty records in sydicate
  long	cp;
  long	cp_pen;		//added to player's cp's on update, used for penalties or bonuses when player is offline.
  int	armies[8];	//army queues related contents
  int	hours;
  int   mhours;
  int	month;


  time_t last_update;	//last time the data was updated used for mhours etc.
  time_t time_stamp;	//last time somone voted to promote/demote this person
  time_t last_login;	//last time the person logged in.

};

/* structure for cabal room data which represents choices of rooms a cabal can build */
struct cabal_room_data{
  ROOM_INDEX_DATA* pRoomIndex;
  int	vnum;
  int	level;  //croom flags that this room posseses
};

/* structure for cabal virtual room created based on croom data */
struct cabal_vroom_data{
  CVROOM_DATA* next;

  int vnum;			//vnum of the skeleton room this was based on
  ROOM_INDEX_DATA* pRoom;	//room that this data represents

  bool fComplete;		//is this room complete, voted and ready to be loaded

  int level;			//cabal room level
  int pos[3];			//3d coordinates
};

/* cabal vote data */
struct cabal_vote_data{
  CVOTE_DATA* next;

  int	vote;	//reference to the vote table

  int	cost;	//cost in cp's to create the vote
  int	min;	//min rank to use the vote
  int	max;	//max rank touse the vote

};


/* cabal index data */
struct cabal_index_data{
/* cabal list */
  CABAL_INDEX_DATA *next;

  AREA_DATA*	pArea;//area this is saved in.

  int		vnum;
  int		parent_vnum;
  int		version;

/* skills */
  CSKILL_DATA*	skills;

/* votes */
  CVOTE_DATA*	votes;

/* room choices */
  CROOM_DATA*	crooms[CROOM_LEVELS];

/* file name */
  char*		file_name;

/* clan name */
  char*		clan;

  int	anchor_vnum;
  int	guard_vnum;
  int	pit_vnum;
  char*	city_name;
  char*	enemy_name;


/* title prefixes for members */
  char*		mprefix;
  char*		fprefix;
  char*		sprefix;	//sponsor prefix

  char*		gate_on;	 //string echoed with act(..) when gate is closed
  char*		gate_off;	 //string echoed with act(..) when gate is opened
  char*		gate_msg;	 //string shown when you try to enter locked cabal

/* army and its upgrade mob */
  int		army;
  int		army_upgrade;
  int		tower;
  int		tower_upgrade;

  sh_int	max_room;	//maximum number of rooms allowed

  char*		name;
  char*		currency;
  char*		who_name;
  char*		immortal;

  char* mranks[RANK_MAX]; //male/neutral
  char* franks[RANK_MAX]; //female

  int		flags;
  int		flags2;

  int		progress; //progress flags

  int		min_rank;		//for children, this is minimum cabal rank to join
  int		min_lvl;
  int		max_lvl;

  int		max_member;

  int		class[MAX_CLASS]; //classes allowed (-1 == class[0] means all)
  int		race[MAX_PC_RACE]; //races allowed (-1 == race[0] means all)
  bool		fAvatar;	//are avatars allowed
  int		align; //aligns (good/neut/evil)
  int		ethos; //ethois (lawful, neut, chaotic)

};


/* cabal index data */
struct	cabal_data{
  CABAL_DATA*		next;
  CABAL_INDEX_DATA*	pIndexData;

  CABAL_DATA*		parent;

  int			version;
  int			vnum;

  /* enemy cabal */
  CABAL_DATA*	enemy;

  /* members */
  CMEMBER_DATA* members;
  CMEMBER_DATA* last_recruit;	//NULL or points to person who last got army

  /* vitual rooms present */
  CVROOM_DATA*	cvrooms;

  /* cabal object */
  OBJ_DATA* citem;

  /* cabal altar */
  CHAR_DATA* altar;

/* cabal guard */
  CHAR_DATA* cguard;

/* armies */
  ARMY_DATA*	armies;

/* army reports */
  REPORT_QUEUE report_q;

/* last army report focus */
  ROOM_INDEX_DATA* last_focus;

  ROOM_INDEX_DATA*	anchor;
  ROOM_INDEX_DATA*	pitroom;
  AREA_DATA*		city;	//set in fix_cabals
  MOB_INDEX_DATA*	guard;	//set in fix cabals

/* gate state */
  bool		fGate;

/* clan handle */
  int		clan;

/* armies ready to be conscripted by members */
  int		armies_ready;
  int		ready_gain;	//contains fraction of a ready army in 1/10ths

  sh_int	cur_room;	//current number of rooms built

/* monetary info */
  long		cp;		//cps in coffers
  long		raid_cps;	//net gain/loss from raids
  long		raid_sup;
  long		bonus_sup;	//current bonus support due to recent raids (tends towards 0)
  long		cp_gain[9];	//last cp gain, income, trade, costs, last army, last room, last trade
  int		support[9];	//last support gain, income, trade, cost, last army, last room, last trade
  int		cp_tax;		//amount in percent to divert from all member's gain to coffers.

/* time stamps */
  time_t	time_stamp;	//last time the cp's were transfered over
  time_t	room_stamp;	//last time the cabal was checked for pos. cp to sustain rooms
  time_t	member_stamp;	//last time the cabal members were checked for time logged in
  time_t	raid_stamp;	//last time the cabal made a raid
  time_t	present_stamp;  //last time a person was present in cabal

  char*		name;
  char*		currency;
  char*		who_name;
  char*		immortal;

  int		flags;
  int		flags2;

  int		max_member;
  int		cur_member;
  int		max_leader;
  int		cur_leader;
  int		max_elder;
  int		cur_elder;

  int		class[MAX_CLASS]; //classes allowed (-1 == class[0] means all)
  int		race[MAX_PC_RACE]; //races allowed (-1 == race[0] means all)
  bool		fAvatar;	//are avatars allowed
  int		align; //aligns (good/neut/evil)
  int		ethos; //ethois (lawful, neut, chaotic)
  int		progress; //progress flags

  int		present;		//current number of members present
  int		count_present;		//temporarly used to count ->present

};

/* a temp or learned skill */
struct	skill_data
{
    SKILL_DATA *	next;
    bool		valid;
    sh_int		sn;
    sh_int		level;
    sh_int		rating;
    sh_int		learned;
    sh_int		keep;
};

/* where definitions */
#define TO_AFFECTS	0
#define TO_OBJECT	1
#define TO_IMMUNE	2
#define TO_RESIST	3
#define TO_VULN		4
#define TO_WEAPON	5
#define TO_AFFECTS2     6
#define TO_NONE         7
#define TO_SKILL        8

/* for objects (to_obj and to_weapon are above)*/
#define TO_WEAR        30	//wear flags


/* A kill structure (indexed by level). */
struct	kill_data
{
    sh_int		number;
    sh_int		killed;
};

/************** information for area builders begin here *****************/

/* Cabal Vnums */
#define VNUM_LEGION	15500
#define VNUM_JUSTICE	15700

#define VNUM_KNIGHT	15910
#define VNUM_CHAOS      16100

#define VNUM_MYSTIC     16300
#define VNUM_WARLORD	16501
#define VNUM_ASSASSIN   27500
#define VNUM_HERALD     28100

/* Not Used */
#define VNUM_MASTER	4494
#define VNUM_SEEKER	4495
#define VNUM_BATTLE	4496
#define VNUM_ENTROPY	4497
#define VNUM_SHADOW	4498



/* Well known mob virtual numbers. */
#define MOB_VNUM_FLESH_GOLEM       1
#define MOB_VNUM_STONE_GOLEM       2
#define MOB_VNUM_ZOMBIE            3
#define MOB_VNUM_MAMMOTH	   4
#define MOB_VNUM_SHADOWDEMON	   5
#define MOB_VNUM_LEOPARD	   6
#define MOB_VNUM_WYVERN 	   7
#define MOB_VNUM_DISPLACER	   8
#define MOB_VNUM_KNIGHT_1	   9
#define MOB_VNUM_TRAP_PROXY	   10
#define MOB_VNUM_RAVEN		   11
#define MOB_VNUM_DEMON		   12
#define MOB_VNUM_SPECIAL_GUARD     13
#define MOB_VNUM_SHADOW		   14
#define MOB_VNUM_NIGHTWALKER       15
#define MOB_VNUM_STALLION          16
#define MOB_VNUM_FAMILIAR          17
#define MOB_VNUM_HOUND		   18
#define MOB_VNUM_KNIGHT_2	   19
#define MOB_VNUM_KNIGHT_3	   20
#define MOB_VNUM_NEMESIS	   21
#define MOB_VNUM_MAGIC_EYE	   22
#define MOB_VNUM_DUMMY		   23
#define MOB_VNUM_VAPOR             24
#define MOB_VNUM_SOLID             25
#define MOB_VNUM_CLONE             26
#define MOB_VNUM_SKIN              27
#define MOB_VNUM_DECOY             28
#define MOB_VNUM_SPIDER            30
#define MOB_VNUM_PHOENIX           31
#define MOB_VNUM_WISH_TRAINER      32
#define MOB_VNUM_PLAYER_SPIRIT     33
#define MOB_VNUM_VANGUARD          34
#define MOB_VNUM_STALKER	   35
#define MOB_VNUM_SERVANT_FIRE	   36
#define MOB_VNUM_SERVANT_WATER     37
#define MOB_VNUM_SERVANT_AIR       38
#define MOB_VNUM_SERVANT_EARTH     39
#define MOB_VNUM_JUSTICE_SENTINEL  40
#define MOB_VNUM_ROYAL_GUARD       41
#define MOB_VNUM_JUSTICE_PATROL    42
#define MOB_VNUM_RAID_WEAK	   52
#define MOB_VNUM_RAID_MEDI	   53
#define MOB_VNUM_RAID_NORM	   54
#define MOB_VNUM_RAID_STRO	   55
#define MOB_VNUM_MONSTER	   56
#define MOB_VNUM_ALTAR		   57
#define MOB_VNUM_CABAL_GUARD	   58

/* nexus mounts */
#define MOB_VNUM_SERPENT_1	43
#define MOB_VNUM_SERPENT_2	44
#define MOB_VNUM_SERPENT_3	45
#define MOB_VNUM_SERPENT_D1	46
#define MOB_VNUM_SERPENT_D2	47
#define MOB_VNUM_SERPENT_D3	48
#define MOB_VNUM_SERPENT_U1	49
#define MOB_VNUM_SERPENT_U2	50
#define MOB_VNUM_SERPENT_U3	51

/* Beast type Beast vnums */
#define BEAST_RACE_TIGER	0
#define BEAST_RACE_WOLF		1
#define BEAST_RACE_BEAR		2
#define BEAST_RACE_FALCON	3
#define BEAST_RACE_MAX		4

/* RT ASCII conversions -- used so we can have letters in this file */
#define A		  	1
#define B			2
#define C			4
#define D			8
#define E			16
#define F			32
#define G			64
#define H			128
#define I			256
#define J			512
#define K		        1024
#define L		 	2048
#define M			4096
#define N		 	8192
#define O			16384
#define P			32768
#define Q			65536
#define R			131072
#define S			262144
#define T			524288
#define U			1048576
#define V			2097152
#define W			4194304
#define X			8388608
#define Y			16777216
#define Z			33554432
#define aa			67108864 	/* doubled due to conflicts */
#define bb			134217728
#define cc			268435456
#define dd			536870912
#define ee			1073741824


/* align ranges */
#define EVIL_THRESH           -350      /* Person is evil below this align */
#define GOOD_THRESH            350      /* Person is good above this align */

/* quest types */
#define QUEST_TEMP		0
#define QUEST_PERM		1
#define QUEST_INVIS		2

/* Familiar_act bits for special mobs.  ie familiars. */
#define SPECIAL_SENTINEL	(A)		/* Will not move */
#define SPECIAL_RETURN		(B)		/* Returns to owner */
#define SPECIAL_WATCH		(C)		/* Echos stuff to owner */
#define SPECIAL_COWARD		(D)		/* Returns to owner when attacked */


/* ACT bits for mobs. */
#define ACT_IS_NPC		(A)		/* Auto set for mobs	*/
#define ACT_SENTINEL	    	(B)		/* Stays in one room	*/
#define ACT_SCAVENGER	      	(C)		/* Picks up objects	*/
#define ACT_CONTRACTER		(D)		/* Can make contracts   */
#define ACT_MOUNT		(E)		/* Can make contracts   */
#define ACT_AGGRESSIVE		(F)    		/* Attacks PC's		*/
#define ACT_STAY_AREA		(G)		/* Won't leave area	*/
#define ACT_WIMPY               (H)             /* Flees in combat      */
#define ACT_PET			(I)		/* Auto set for pets	*/
#define ACT_TRAIN		(J)		/* Can train PC's	*/
#define ACT_PRACTICE		(K)		/* Can practice PC's	*/
#define ACT_NONCOMBAT		(L)		/* Can practice PC's	*/
#define ACT_RAIDER		(M)		/* MOB created by a raid*/
#define ACT_UNDEAD              (O)             /* Acts like undead     */
#define ACT_CLERIC              (Q)             /* Acts like cleric     */
#define ACT_MAGE                (R)             /* Acts like mage       */
#define ACT_THIEF               (S)             /* Acts like thief      */
#define ACT_WARRIOR             (T)             /* Acts like warrior    */
#define ACT_NOALIGN             (U)             /* No alignment         */
#define ACT_NOPURGE             (V)             /* Can not be purged    */
#define ACT_OUTDOORS            (W)             /* Stays outdoors       */
#define ACT_INDOORS             (Y)             /* Stays indoors        */
#define ACT_TOO_BIG             (Z)       /* Cant bj/bs/str/ass/cleave */
#define ACT_IS_HEALER           (aa)            /* Acts like healer     */
#define ACT_GAIN                (bb)            /* Can gain converts    */
#define ACT_UPDATE_ALWAYS       (cc)            /* Always reset         */
#define ACT_WANDER              (dd)            /* Wanders a lot        */
#define ACT_NO_WANDER           (ee)            /* Wanders a lot        */

/* ACT2 bits for mobs. */
#define ACT_DUPLICATE           (B)		/* a duplicated mob       */
#define ACT_NOEXP               (C)		/* no exp given           */
#define ACT_LAWFUL		(D)		/* will jail criminals it kills */
#define ACT_HISTORY		(E)
#define ACT_NEED_MASTER		(F)		/* extracted if no master */
#define ACT_MORPH		(G)		/* morphs to match opponent */
#define ACT_NOLOSS		(H)		/* no exp loss when pc killed */
#define ACT_NODEATH		(I)		/* MOB causes mort-wound not death to PCs (still runs spec_kill though)*/


/* damage classes */
#define DAM_NONE                0
#define DAM_BASH                1
#define DAM_PIERCE              2
#define DAM_SLASH               3
#define DAM_FIRE                4
#define DAM_COLD                5
#define DAM_LIGHTNING           6
#define DAM_ACID                7
#define DAM_POISON              8
#define DAM_NEGATIVE            9
#define DAM_HOLY                10
#define DAM_ENERGY              11
#define DAM_MENTAL              12
#define DAM_DISEASE             13
#define DAM_DROWNING            14
#define DAM_LIGHT		15
#define DAM_OTHER               16
#define DAM_HARM		17
#define DAM_CHARM		18
#define DAM_SOUND		19
#define DAM_SILVER		20
#define DAM_WOOD		21
#define DAM_IRON		22
#define DAM_MITHRIL		23
#define DAM_MALED		24
#define DAM_INTERNAL            25     //Not affected by any defense.
#define DAM_AIR			26

/* spell type for const.c */
#define SPELL_OTHER		0
#define SPELL_AFFLICTIVE	1
#define SPELL_MALEDICTIVE	2
#define SPELL_MALEDICTION	2
#define SPELL_MENTAL		3
#define SPELL_PROTECTIVE	4
#define SPELL_PROTECTION	4
#define SPELL_CURATIVE		5
#define SPELL_RESTORATIVE	6

/* OFF bits for mobiles */
#define OFF_AREA_ATTACK         (A)
#define OFF_BACKSTAB            (B)
#define OFF_BASH                (C)
#define OFF_BERSERK             (D)
#define OFF_DISARM              (E)
#define OFF_DODGE               (F)
#define OFF_FADE                (G)
#define OFF_FAST                (H)
#define OFF_KICK                (I)
#define OFF_KICK_DIRT           (J)
#define OFF_PARRY               (K)
#define OFF_RESCUE              (L)
#define OFF_TAIL                (M)
#define OFF_TRIP                (N)
#define OFF_CRUSH		(O)
#define ASSIST_ALL       	(P)
#define ASSIST_ALIGN	        (Q)
#define ASSIST_RACE    	     	(R)
#define ASSIST_PLAYERS      	(S)
#define ASSIST_GUARD        	(T)
#define ASSIST_VNUM		(U)
#define GUILD_GUARD		(V)
#define CABAL_GUARD		(W)
#define OFF_EXTRA_ATTACK	(X)
#define OFF_DPARRY		(Y)

/* perk bits */
#define PERK_OVERWEIGHT		(A)
#define PERK_UNDERWEIGHT	(B)
#define PERK_BOOKWORM		(C)
#define PERK_ATHLETIC		(D)
#define PERK_HEALTHY		(E)
#define PERK_SICKLY		(F)
#define PERK_STOUT		(G)
#define PERK_NIMBLE		(H)
#define PERK_CHARISMATIC	(I)
#define PERK_FEARSOME		(J)
#define PERK_GREEDY		(K)
#define PERK_TALL		(L)
#define PERK_SHORT		(M)
#define PERK_ALCOHOLIC		(N)
#define PERK_ADDICT		(O)
#define PERK_LUCKY		(P)
#define PERK_UNLUCKY		(Q)
#define PERK_PIOUS		(R)
#define PERK_MAGICAL		(S)
#define PERK_ARTIFICER		(T)
#define PERK_TINKERER		(U)
#define PERK_EAGLEEYE		(V)
#define PERK_ADVENTURER		(X)
#define PERK_HEROIC		(Y)
#define PERK_MERCY		(Z)
#define CGEN_MAX_PERK		27

/* return values for check_imm */
#define IS_IMMUNE		0
#define IS_RESISTANT		1
#define IS_NORMAL		2
#define IS_VULNERABLE		3

/* IMM bits for mobs */
#define IMM_SUMMON              (A)
#define IMM_CHARM               (B)
#define IMM_MAGIC               (C)
#define IMM_WEAPON              (D)
#define IMM_BASH                (E)
#define IMM_PIERCE              (F)
#define IMM_SLASH               (G)
#define IMM_FIRE                (H)
#define IMM_COLD                (I)
#define IMM_LIGHTNING           (J)
#define IMM_ACID                (K)
#define IMM_POISON              (L)
#define IMM_NEGATIVE            (M)
#define IMM_HOLY                (N)
#define IMM_ENERGY              (O)
#define IMM_MENTAL              (P)
#define IMM_DISEASE             (Q)
#define IMM_DROWNING            (R)
#define IMM_LIGHT		(S)
#define IMM_SOUND		(T)
#define IMM_WOOD                (X)
#define IMM_SILVER              (Y)
#define IMM_IRON                (Z)
#define IMM_MITHRIL             (aa)
#define IMM_AIR			(bb)


/* RES bits for mobs */
#define RES_SUMMON		(A)
#define RES_CHARM		(B)
#define RES_MAGIC               (C)
#define RES_WEAPON              (D)
#define RES_BASH                (E)
#define RES_PIERCE              (F)
#define RES_SLASH               (G)
#define RES_FIRE                (H)
#define RES_COLD                (I)
#define RES_LIGHTNING           (J)
#define RES_ACID                (K)
#define RES_POISON              (L)
#define RES_NEGATIVE            (M)
#define RES_HOLY                (N)
#define RES_ENERGY              (O)
#define RES_MENTAL              (P)
#define RES_DISEASE             (Q)
#define RES_DROWNING            (R)
#define RES_LIGHT		(S)
#define RES_SOUND		(T)
#define RES_WOOD                (X)
#define RES_SILVER              (Y)
#define RES_IRON                (Z)
#define RES_MITHRIL             (aa)
#define RES_AIR			(bb)

/* VULN bits for mobs */
#define VULN_SUMMON		(A)
#define VULN_CHARM		(B)
#define VULN_MAGIC              (C)
#define VULN_WEAPON             (D)
#define VULN_BASH               (E)
#define VULN_PIERCE             (F)
#define VULN_SLASH              (G)
#define VULN_FIRE               (H)
#define VULN_COLD               (I)
#define VULN_LIGHTNING          (J)
#define VULN_ACID               (K)
#define VULN_POISON             (L)
#define VULN_NEGATIVE           (M)
#define VULN_HOLY               (N)
#define VULN_ENERGY             (O)
#define VULN_MENTAL             (P)
#define VULN_DISEASE            (Q)
#define VULN_DROWNING           (R)
#define VULN_LIGHT		(S)
#define VULN_SOUND		(T)
#define VULN_WOOD               (X)
#define VULN_SILVER             (Y)
#define VULN_IRON		(Z)
#define VULN_MITHRIL		(aa)
#define VULN_AIR		(bb)

/* body form */
#define FORM_EDIBLE             (A)
#define FORM_POISON             (B)
#define FORM_MAGICAL            (C)
#define FORM_INSTANT_DECAY      (D)
#define FORM_OTHER              (E)  /* defined by material bit */

/* actual form */
#define FORM_ANIMAL             (G)
#define FORM_SENTIENT           (H)
#define FORM_UNDEAD             (I)
#define FORM_CONSTRUCT          (J)
#define FORM_MIST               (K)
#define FORM_INTANGIBLE         (L)
#define FORM_BIPED              (M)
#define FORM_CENTAUR            (N)
#define FORM_INSECT             (O)
#define FORM_SPIDER             (P)
#define FORM_CRUSTACEAN         (Q)
#define FORM_WORM               (R)
#define FORM_BLOB		(S)
#define FORM_MAMMAL             (V)
#define FORM_BIRD               (W)
#define FORM_REPTILE            (X)
#define FORM_SNAKE              (Y)
#define FORM_DRAGON             (Z)
#define FORM_AMPHIBIAN          (aa)
#define FORM_FISH               (bb)
#define FORM_COLD_BLOOD		(cc)

#define FORM_HUMANOID		FORM_EDIBLE|FORM_SENTIENT|FORM_MAMMAL|FORM_BIPED

/* body parts */
#define PART_HEAD               (A)
#define PART_ARMS               (B)
#define PART_LEGS               (C)
#define PART_HEART              (D)
#define PART_BRAINS             (E)
#define PART_GUTS               (F)
#define PART_HANDS              (G)
#define PART_FEET               (H)
#define PART_FINGERS            (I)
#define PART_EAR                (J)
#define PART_EYE		(K)
#define PART_LONG_TONGUE        (L)
#define PART_EYESTALKS          (M)
#define PART_TENTACLES          (N)
#define PART_FINS               (O)
#define PART_WINGS              (P)
#define PART_TAIL               (Q)
/* for combat */
#define PART_CLAWS              (U)
#define PART_FANGS              (V)
#define PART_HORNS              (W)
#define PART_SCALES             (X)
#define PART_TUSKS		(Y)

#define PART_HUMANOID		PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE
#define PART_ANIMAL		PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_EAR|PART_EYE|PART_CLAWS



/* Bits for 'affected_by'. */
#define AFF_BLIND		(A)
#define AFF_INVISIBLE		(B)
#define AFF_DETECT_EVIL		(C)
#define AFF_DETECT_INVIS	(D)
#define AFF_DETECT_MAGIC	(E)
#define AFF_DETECT_HIDDEN	(F)
#define AFF_DETECT_GOOD		(G)
#define AFF_SANCTUARY		(H)
#define AFF_FAERIE_FIRE		(I)
#define AFF_INFRARED		(J)
#define AFF_CURSE		(K)
#define AFF_FAERIE_FOG          (L)
#define AFF_POISON		(M)
#define AFF_PROTECT_EVIL	(N)
#define AFF_PROTECT_GOOD	(O)
#define AFF_SNEAK		(P)
#define AFF_HIDE		(Q)
#define AFF_SLEEP		(R)
#define AFF_CHARM		(S)
#define AFF_FLYING		(T)
#define AFF_PASS_DOOR		(U)
#define AFF_HASTE		(V)
#define AFF_CALM		(W)
#define AFF_PLAGUE		(X)
#define AFF_WEAKEN		(Y)
#define AFF_TREEFORM		(Z)
#define AFF_BERSERK		(aa)
#define AFF_SIEGE		(bb)
#define AFF_FLAG                (cc)//Used for settings flags with other effects.
#define AFF_SLOW		(dd)
#define AFF_WARD_LIGHT		(ee)

/* Bits for 'affected2_by'. */
#define AFF_PROTECT_SHIELD      (A)
#define AFF_ICE_SHIELD          (B)
#define AFF_FIRE_SHIELD         (C)
#define AFF_HOLD                (D)
#define AFF_BLUR                (E)
#define AFF_TERRAIN             (F)
#define AFF_MISDIRECTION        (G)
#define AFF_SHADOWFORM          (H)
#define AFF_RAGE	        (I)
#define AFF_CAMOUFLAGE	        (J)
#define AFF_KEEN_SIGHT	        (K)
#define AFF_SILENCE             (L)
#define AFF_SENSE_EVIL          (M)
#define AFF_MANA_SHIELD         (N)
#define AFF_BARRIER             (O)
#define AFF_CATALEPSY           (P)
#define AFF_SEVERED		(Q)
#define AFF_PETRIFY		(R)
#define AFF_NOREMOVE            (bb)//affect is not removed when item is uneqiped
#define AFF_FLAG                (cc)//Used for settings flags with other effects.


/* peforms automatic responses to combat actions */
#define CCOMBAT_TRIPPED	1
#define CCOMBAT_TRIP	2
#define CCOMBAT_DISARM  3
#define CCOMBAT_BASHED	4
#define CCOMBAT_EVADE	5
#define CCOMBAT_DPARRY	6


/* quests */
#define QUEST_CRUS		1
#define QUEST_AVTA		2
#define QUEST_DEMO		3
#define QUEST_VAMP		4
#define QUEST_UNDE		5
#define QUEST_PSI		6

/* death blow */
#define DB_NORMAL 0
#define DB_STRIP  1
#define DB_SPLIT  2
#define DB_FULL   3

/* Sex. */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2

/* Ethos */
#define CH_ETHOS_LAWFUL			0
#define CH_ETHOS_NEUTRAL		1
#define CH_ETHOS_CHAOTIC		2

/* AC types */
#define AC_PIERCE			0
#define AC_BASH				1
#define AC_SLASH			2
#define AC_EXOTIC			3

/* dice */
#define DICE_NUMBER			0
#define DICE_TYPE			1
#define DICE_BONUS			2

/* size */
#define SIZE_TINY			0
#define SIZE_SMALL			1
#define SIZE_MEDIUM			2
#define SIZE_LARGE			3
#define SIZE_HUGE			4
#define SIZE_GIANT			5

/* Well known object virtual numbers. */
#define OBJ_VNUM_PIT		      1
#define OBJ_VNUM_SOUL                 1
#define OBJ_VNUM_GOLD_ONE	      2
#define OBJ_VNUM_GOLD_SOME	      3
#define OBJ_VNUM_RANGER_STAFF	      4
#define OBJ_VNUM_RANGER_ARROW	      5
#define OBJ_VNUM_VANGUARD_WEP	      6
#define OBJ_VNUM_TRAP_PROXY	      8
#define OBJ_VNUM_TOWER_CORPSE	      9
#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_GUTS		     16
#define OBJ_VNUM_BRAINS		     17
#define OBJ_VNUM_RANGER_BOW	     18
#define OBJ_VNUM_TOME		     19
#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22
#define OBJ_VNUM_DISC		     23
#define OBJ_VNUM_HOLY_ARMOR	     25
#define OBJ_VNUM_HOLY_WEAPON	     26
#define OBJ_VNUM_STEAK		     27
#define OBJ_VNUM_BATTLE_PONCHO	     28
#define OBJ_VNUM_JUSTICE_SHIELD	     29
#define OBJ_VNUM_JUSTICE_BADGE	     30
#define OBJ_VNUM_POTION 	     31
#define OBJ_VNUM_CHAOS_BLADE 	     32
#define OBJ_VNUM_JUSTICE_SHIELD2     33 //elder shield (heals)
#define OBJ_VNUM_ENTROPIC_SLUDGE     34
#define OBJ_VNUM_PLANAR_BLADE 	     35
#define OBJ_VNUM_RAID_OBJECT 	     36
#define OBJ_VNUM_CABAL_ITEM 	     37
#define OBJ_VNUM_BOULDER_HOLE 	     38
#define OBJ_VNUM_CROWN		     39
#define OBJ_VNUM_LOCKER		     40
#define OBJ_VNUM_LEGS		     46
#define OBJ_VNUM_WARLORD_BELT 	     50
#define OBJ_VNUM_WARLORD_EARRING     51
#define OBJ_VNUM_WARLORD_PONCHO      52
#define OBJ_VNUM_WARLORD_SKULL       53
#define OBJ_VNUM_WARLORD_LIGHT       54
#define OBJ_VNUM_THROW		     55
#define OBJ_VNUM_CLONE		     56
#define OBJ_VNUM_FAMILIAR_TAG	     57
#define OBJ_VNUM_FECES		     59
#define OBJ_VNUM_GLASS		     60
#define OBJ_VNUM_ROYAL_FLAG          61
#define OBJ_VNUM_ARMY_CORPSE	     62
#define OBJ_VNUM_VOMIT		     63
#define OBJ_VNUM_TORCH		     65
#define OBJ_VNUM_DRUID_STAFF	     66
#define OBJ_VNUM_PAPER		     67
#define OBJ_VNUM_RESEARCH	     69
#define OBJ_VNUM_HAND		     70
#define OBJ_VNUM_FOOT		     71
#define OBJ_VNUM_FINGERS	     72
#define OBJ_VNUM_EAR		     73
#define OBJ_VNUM_EYES		     74
#define OBJ_VNUM_TONGUE		     75
#define OBJ_VNUM_FINS		     76
#define OBJ_VNUM_WINGS		     77
#define OBJ_VNUM_TAIL		     78
#define OBJ_VNUM_CLAWS		     79
#define OBJ_VNUM_FANGS		     80
#define OBJ_VNUM_HORN		     81
#define OBJ_VNUM_TUSK		     82
#define OBJ_VNUM_GRUM		     83
#define OBJ_VNUM_HEALER_STAFF	     84
#define OBJ_VNUM_PIXIE_DUST	     90
#define OBJ_VNUM_PRAYER_BEADS	     91
#define OBJ_VNUM_FAKE_CORPSE	     92
#define OBJ_VNUM_BLOOD_POOL	     100
#define OBJ_VNUM_MAL_WEAPON	     101
#define OBJ_VNUM_VOODOO_DOLL	     102
#define OBJ_VNUM_DUMMY_CABAL	     103
#define OBJ_VNUM_EXECUTION	     104
#define OBJ_VNUM_JAILBAG	     105
#define OBJ_VNUM_BLOOD_SPOT	     111
#define OBJ_VNUM_BLOOD_POOL2	     113
#define OBJ_VNUM_STARSTONE	     141
#define OBJ_VNUM_ARMORCRAFT	     142
#define OBJ_VNUM_JUGGERNAUT	     143
#define OBJ_VNUM_WEAPONCRAFT	     144


#define OBJ_VNUM_ROSE		   1001
#define OBJ_VNUM_SCHOOL_MACE	   7780
#define OBJ_VNUM_SCHOOL_DAGGER	   7781
#define OBJ_VNUM_SCHOOL_SWORD	   7782
#define OBJ_VNUM_SCHOOL_SPEAR	   7783
#define OBJ_VNUM_SCHOOL_STAFF	   7784
#define OBJ_VNUM_SCHOOL_AXE	   7785
#define OBJ_VNUM_SCHOOL_FLAIL	   7786
#define OBJ_VNUM_SCHOOL_WHIP	   7787
#define OBJ_VNUM_SCHOOL_POLEARM    7788
#define OBJ_VNUM_SCHOOL_VEST	   7789
#define OBJ_VNUM_SCHOOL_SHIELD	   7790
#define OBJ_VNUM_SCHOOL_BANNER     7791
#define OBJ_VNUM_SCHOOL_RECORDER   7792
#define OBJ_VNUM_VALMIRAN_MAP	   4426
#define OBJ_VNUM_RHEYDIN_MAP	   11569
#define OBJ_VNUM_S_SERINGALE_MAP   4427
#define OBJ_VNUM_E_THERA_MAP   	   4428
#define OBJ_VNUM_W_THERA_MAP       4429
#define OBJ_VNUM_WHISTLE	   2116

#define OBJ_VNUM_SCHOOL_CROSSBOW     30390

/* Item types. */
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		      8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_CLOTHING		     11
#define ITEM_FURNITURE		     12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		     15
#define ITEM_HERB                    16
#define ITEM_DRINK_CON		     17
#define ITEM_KEY		     18
#define ITEM_FOOD		     19
#define ITEM_MONEY		     20
#define ITEM_BOAT		     22
#define ITEM_CORPSE_NPC		     23
#define ITEM_CORPSE_PC		     24
#define ITEM_FOUNTAIN		     25
#define ITEM_PILL		     26
#define ITEM_MAP		     28
#define ITEM_GEM		     32
#define ITEM_JEWELRY		     33
#define ITEM_CABAL                   34
#define ITEM_ARTIFACT                35
#define ITEM_RELIC                   36
#define ITEM_THROW                   37
#define ITEM_INSTRUMENT		     38
#define ITEM_SOCKET		     39
#define ITEM_RESEARCH		     40
#define ITEM_RANGED		     42
#define ITEM_PROJECTILE		     43

/* Extra flags. Make sure to add in tables.c*/
#define ITEM_GLOW		(A)
#define ITEM_HUM		(B)
#define ITEM_DARK		(C)
#define ITEM_LOCK		(D)
#define ITEM_EVIL		(E)
#define ITEM_INVIS		(F)
#define ITEM_MAGIC		(G)
#define ITEM_NODROP		(H)
#define ITEM_BLESS		(I)
#define ITEM_ANTI_GOOD		(J)
#define ITEM_ANTI_EVIL		(K)
#define ITEM_ANTI_NEUTRAL	(L)
#define ITEM_NOREMOVE		(M)
#define ITEM_INVENTORY		(N)
#define ITEM_NOPURGE		(O)
#define ITEM_ROT_DEATH		(P)
#define ITEM_VIS_DEATH		(Q)
#define ITEM_PSI_ENHANCE	(R)
#define ITEM_NONMETAL		(S)
#define ITEM_NOLOCATE		(T)
#define ITEM_MELT_DROP		(U)
#define ITEM_HAD_TIMER		(V)
#define ITEM_SELL_EXTRACT	(W)
#define ITEM_USE		(X)
#define ITEM_BURN_PROOF		(Y)
#define ITEM_NOUNCURSE		(Z)
#define ITEM_CABALTAKE		(aa)  //used to check if cabal item has
                                      //already been taken (cannot recive
                                      //bonus after it has been removed)
#define ITEM_HOLDS_RARE		(bb)  //used to check if container can hold
                                      //rare items
#define ITEM_UPDATE_ALWAYS	(cc)  //oprog is always updaed even if no one
					//in area
#define ITEM_STAIN		(dd)  //Stain flag used for anticheating.
#define ITEM_SOCKETABLE         (ee)  //can put gems in weapon
#define ITEM_NO_REQUEST	(ff) //Unrequestable item

/* Wear flags. */
/* Note..  make sure to adjust in do_compare */
#define ITEM_TAKE		(A)
#define ITEM_WEAR_FINGER	(B)
#define ITEM_WEAR_NECK		(C)
#define ITEM_WEAR_BODY		(D)
#define ITEM_WEAR_HEAD		(E)
#define ITEM_WEAR_LEGS		(F)
#define ITEM_WEAR_FEET		(G)
#define ITEM_WEAR_HANDS		(H)
#define ITEM_WEAR_ARMS		(I)
#define ITEM_WEAR_SHIELD	(J)
#define ITEM_WEAR_ABOUT		(K)
#define ITEM_WEAR_WAIST		(L)
#define ITEM_WEAR_WRIST		(M)
#define ITEM_WIELD		(N)
#define ITEM_HOLD		(O)
#define ITEM_NO_SAC		(P)
#define ITEM_WEAR_FLOAT		(Q)
#define ITEM_WEAR_SECONDARY     (R)
#define ITEM_UNIQUE             (S)
#define ITEM_WEAR_TATTOO        (T)
#define ITEM_WEAR_SHROUD        (U)
#define ITEM_REPLACEME		(W)
#define ITEM_REPLACEME2		(V)
#define ITEM_RARE		(X)
#define ITEM_PARRY		(Y)
#define ITEM_WEAR_EARRING	(Z)
#define ITEM_WEAR_QUIVER	(aa)
//#define ITEM_REPLACEME	(bb)
#define ITEM_HAS_OWNER		(cc)//requires gen_has_owner
#define ITEM_WEAR_FACE		(dd)
#define ITEM_WEAR_RANGED        (ee)//Holds ranged items



/* weapon types */
#define WEAPON_FLAMING		(A)
#define WEAPON_FROST		(B)
#define WEAPON_VAMPIRIC		(C)
#define WEAPON_SHARP		(D)
#define WEAPON_VORPAL		(E)
#define WEAPON_TWO_HANDS	(F)
#define WEAPON_SHOCKING		(G)
#define WEAPON_POISON		(H)
#define WEAPON_JEWELED		(I)
#define WEAPON_PARALYZE		(J)
#define WEAPON_WOUNDS		(K)
#define WEAPON_DEADLY		(L)


/* ranged weapon flags */
#define RANGED_NONE		0
#define RANGED_FAST		(A)	//takes half the time to fire out of combat, and auto fires back when shot at
#define RANGED_LONGRANGE	(B)	//has half the range penalties to skill
#define RANGED_PASSDOOR		(C)	//shots ignore doors
#define RANGED_ARMORPIERCE	(D)	//shots ignore armor
#define RANGED_SLOW		(E)	//takes longer to reload in combat
#define RANGED_UNLIMITED	(F)	//creates its own ammo

/* projectile types */
#define PROJECTILE_ARROW	(A)
#define PROJECTILE_BOLT		(B)
#define PROJECTILE_STONE	(C)
#define PROJECTILE_QUARREL	(D)
#define PROJECTILE_DART		(E)
#define PROJECTILE_SPEAR	(F)
#define PROJECTILE_BLADE	(G)
#define PROJECTILE_BULLET	(H)

/* projectile special types */
#define PROJ_SPEC_POISON	(A)
#define PROJ_SPEC_PLAGUE	(B)
#define PROJ_SPEC_SHARP		(C)
#define PROJ_SPEC_BLUNT		(D)
#define PROJ_SPEC_PARALYZE	(E)
#define PROJ_SPEC_FLAMING	(F)
#define PROJ_SPEC_FROST		(G)
#define PROJ_SPEC_SLEEP		(H)
#define PROJ_SPEC_BARBED	(I)
#define PROJ_SPEC_SHOCK		(J)
#define PROJ_SPEC_EXPLODE	(K)
#define PROJ_SPEC_VORPAL	(L)
#define PROJ_SPEC_PASSDOOR	(M)

/* trap flags */
#define TRAP_NONE		0
#define TRAP_NODISARM		(A)
#define TRAP_ALLDISARM		(B)
#define TRAP_EASY		(C)
#define TRAP_MEDIUM		(D)
#define TRAP_HARD		(E)
#define TRAP_NODODGE		(F)
#define TRAP_SAVES		(G)
#define TRAP_INVIS		(H)
#define TRAP_VIS		(I)
#define TRAP_DELAY		(J)
#define TRAP_NOTIFY		(K)
#define TRAP_REARM		(L)
#define TRAP_SILENT		(M)
#define TRAP_PERMAMENT		(N)
#define TRAP_AREA		(O)	//area effect on group of victim

/* trap types */
#define TTYPE_NONE		0
#define TTYPE_DAMAGE		1
#define TTYPE_XDAMAGE		2
#define TTYPE_SPELL		3
#define TTYPE_DUMMY		4
#define TTYPE_MOB		5

/* trap reset types */
#define TRAP_ON_OBJ		0
#define TRAP_ON_EXIT		1

/* socket flags */
#define SOCKET_ARMOR		(A)	//can only be put on armor
#define SOCKET_WEAPON		(B)	//can only be put on a weapon


/* Apply types (for affects). */
#define APPLY_NONE               0
#define APPLY_STR                1
#define APPLY_DEX                2
#define APPLY_INT                3
#define APPLY_WIS                4
#define APPLY_CON                5
#define APPLY_SEX                6
#define APPLY_CLASS              7
#define APPLY_LEVEL              8
#define APPLY_AGE                9
#define APPLY_SIZE              10
#define APPLY_WEIGHT            11
#define APPLY_MANA              12
#define APPLY_HIT               13
#define APPLY_MOVE              14
#define APPLY_GOLD              15
#define APPLY_EXP               16
#define APPLY_AC                17
#define APPLY_HITROLL           18
#define APPLY_DAMROLL           19
#define APPLY_SAVING_AFFL       20
#define APPLY_SAVING_MALED      21
#define APPLY_SAVING_MENTAL     22
#define APPLY_SAVING_BREATH     23
#define APPLY_SAVING_SPELL      24
#define APPLY_SPELL_AFFECT      25
#define APPLY_LUCK              26
#define APPLY_SPELL_LEVEL       27	/* Bonus to spell level on casting */
#define APPLY_AFFL_LEVEL	28	/* Bonus to spell level on casting */
#define APPLY_MALED_LEVEL       29	/* Bonus to spell level on casting */
#define APPLY_MENTAL_LEVEL      30	/* Bonus to spell level on casting */
#define APPLY_WAIT_STATE        31	/* % increase to spell lag, 100 = 2x lag, -50 = 0.5 lag */
#define APPLY_SPELL_LEARN       32	/* % increase of chance to learn from non-spells (100% = double normal) */
#define APPLY_SKILL_LEARN       33	/* % increase of chance to learn from spells (100% = double normal) */
#define APPLY_HIT_GAIN		34	/* hit recovery (100 = double normal) */
#define APPLY_MANA_GAIN		35	/* mana recovery (100 = double normal) */
#define APPLY_MOVE_GAIN		36	/* move recovery (100 = double normal) */
#define APPLY_SPELL_COST        37	/* % increase to spell cost 100 = 2x cost, -50 = 0.5 cost */


/* object applies */
#define APPLY_O_NONE	  APPLY_NONE	//nothing
#define APPLY_O_COND		100	//conditon of obj
#define APPLY_O_LEVEL		101	//level of object
#define APPLY_O_CABAL		102	//cabal of object
#define APPLY_O_WEIGHT		103	//weight of object (1/10 lb)
#define APPLY_O_COST		104	//cost of object
#define APPLY_O_VAL0		105	//val0 of object
#define APPLY_O_VAL1		106	//val1 of object
#define APPLY_O_VAL2		107	//val2 of object
#define APPLY_O_VAL3		108	//val3 of object
#define APPLY_O_VAL4		109	//val4 of object
#define APPLY_O_DTYPE		110	//damage type of weapon

/* Values for containers (value[1]). */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8
#define CONT_PUT_ON		     16

/* Well known room virtual numbers. */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_LIMIT               3
#define ROOM_VNUM_CABAL_LIMBO	      4
#define ROOM_VNUM_CROOM_DUMMY	      5
#define ROOM_VNUM_CROOM_GARDEN	      8
#define ROOM_VNUM_BOUNTY              6
#define ROOM_VNUM_MAZE                7
#define ROOM_VNUM_BURROW	   101
#define ROOM_VNUM_TEMPLE	   11665
#define ROOM_VNUM_ALTAR		   4490
#define ROOM_VNUM_SCHOOL	   7700
#define ROOM_VNUM_PORTAL	   7700
#define ROOM_VNUM_CRUS_PORTAL	   6400
#define ROOM_VNUM_PSI_PORTAL	   12601
#define ROOM_VNUM_DARK_RIFT	   1051



/* Room flags. */
#define ROOM_DARK		(A)
#define ROOM_MARK		(B)
#define ROOM_NO_MOB		(C)
#define ROOM_INDOORS		(D)
#define ROOM_DAMAGE		(E)	//Causes small damage each combat tick
#define ROOM_NO_QUIT		(I)
#define ROOM_PRIVATE		(J)
#define ROOM_SAFE		(K)
#define ROOM_SOLITARY		(L)
#define ROOM_PET_SHOP		(M)
#define ROOM_NO_INOUT		(N)
#define ROOM_IMP_ONLY		(O)
#define ROOM_GODS_ONLY		(P)
#define ROOM_PATH_MARK		(Q)
#define ROOM_NEWBIES_ONLY	(R)
#define ROOM_NO_GHOST		(S)
#define ROOM_NOWHERE		(T)
#define ROOM_REGISTRY		(U)
#define ROOM_NO_GATEIN		(V)
#define ROOM_NO_GATEOUT		(W)
#define ROOM_NO_SUMMONIN	(X)
#define ROOM_NO_SUMMONOUT	(Y)
#define ROOM_NO_RECALL		(Z)
#define ROOM_NO_VORTEX		(aa)
#define ROOM_NO_SCAN		(bb)
#define ROOM_NO_TELEPORTIN	(cc)
#define ROOM_NO_TELEPORTOUT	(dd)
#define ROOM_DARK_RIFT		(ee)

/* room2 flags */
#define ROOM_NO_MAGIC		(A)
#define ROOM_NO_RESET		(B)
#define ROOM_DAY		(C)
#define ROOM_NIGHT		(D)
#define ROOM_NO_NEWBIES		(E)
#define ROOM_ALARM		(F)
#define ROOM_SPEC_UPDATE        (G)
#define ROOM_JAILCELL		(H)	//Special flags for jail rooms, must also be included in jail_table
#define ROOM_PSI_FEAR		(I)
#define ROOM_MERCY		(J)
#define ROOM_NOHUNGER		(K)	//no hunger/thirst
#define ROOM_LIBRARY		(L)	//library (tomes avaliable)
#define ROOM_GUILD_ENTRANCE	(M)	//guild entrance

/* Directions. */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5
#define MAX_DOOR		      6

/* Exit flags. */
#define EX_ISDOOR		      (A)
#define EX_CLOSED		      (B)
#define EX_LOCKED		      (C)
#define EX_PICKPROOF		      (F)
#define EX_NOPASS		      (G)
#define EX_EASY			      (H)
#define EX_HARD			      (I)
#define EX_INFURIATING		      (J)
#define EX_NOCLOSE		      (K)
#define EX_NOLOCK		      (L)
#define EX_SECRET		      (M)

/* Sector types. */
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_SWAMP		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_LAVA		     11
#define SECT_SNOW		     12


/* Equipment wear locations. */
#define WEAR_NONE		     -1
#define WEAR_LIGHT                    0
#define WEAR_FINGER_L                 1
#define WEAR_FINGER_R                 2
#define WEAR_NECK_1                   3
#define WEAR_NECK_2                   4
#define WEAR_BODY                     5
#define WEAR_HEAD                     6
#define WEAR_FACE		      7
#define WEAR_LEGS                     8
#define WEAR_FEET                     9
#define WEAR_HANDS                   10
#define WEAR_ARMS                    11
#define WEAR_SHIELD                  12
#define WEAR_ABOUT                   13
#define WEAR_WAIST                   14
#define WEAR_WRIST_L                 15
#define WEAR_WRIST_R                 16
#define WEAR_WIELD                   17
#define WEAR_HOLD                    18
#define WEAR_FLOAT                   19
#define WEAR_SECONDARY               20
#define WEAR_SHROUD                  21
#define WEAR_TATTOO                  22
#define WEAR_EARRING		     23
#define WEAR_RANGED		     24
#define WEAR_SHEATH_L		     25
#define WEAR_SHEATH_R		     26
#define WEAR_QUIVER		     27
#define MAX_WEAR                     28

/**************** information for area builders end here *****************/

/* Conditions. */
#define COND_DRUNK		      0
#define COND_FULL		      1
#define COND_THIRST		      2
#define COND_HUNGER		      3

/* Positions. */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_MEDITATE		      4
#define POS_SLEEPING		      5
#define POS_RESTING		      6
#define POS_SITTING		      7
#define POS_FIGHTING		      8
#define POS_STANDING		      9

/* ACT bits for players. */
#define PLR_IS_NPC		(A)		/* Don't EVER set.	*/
/* RT auto flags */
#define PLR_AUTOASSIST		(C)
#define PLR_AUTOEXIT		(D)
#define PLR_AUTOLOOT		(E)
#define PLR_AUTOSAC             (F)
#define PLR_AUTOGOLD		(G)
#define PLR_AUTOSPLIT		(H)

/* RT personal flags */
#define PLR_HOLYLIGHT		(N)
#define PLR_UNDEAD		(O)
#define PLR_NOSUMMON		(Q)
#define PLR_NOFOLLOW		(R)

/* penalty flags */
#define PLR_PERMIT		(U)
#define PLR_LOG			(W)
#define PLR_DENY		(X)
#define PLR_FREEZE		(Y)
#define PLR_WANTED              (Z)
#define PLR_DOOF		(bb)
#define PLR_HEARALL		(cc)
#define PLR_OUTCAST		(dd)



/* ACT2 bits for players. */
#define PLR_NOQUIT		(A)
#define PLR_NOWHO		(B)
#define PLR_ANNOY		(C)
#define PLR_GIMP		(D)

#define PLR_GARBLE		(H)
#define PLR_EQLIMIT		(I)
#define PLR_MASQUERADE          (J)   //Masquerade flag for vamps.
#define PLR_CODING		(K)   // IMM is coding flag
#define PLR_ROYAL		(L)   //Player is a royal
#define PLR_NOBLE		(M)   //Player is a noble
#define PLR_DESC		(N)   //Used for desc approval
#define PLR_AVATAR		(P)   //Used for AVATAR type
#define PLR_NOPURGE		(Q)   //Rares are not purged due to lack of presence


/* MSG flags used for log in messages actual messages stored in table */
#define MSG_NONE		0
#define MSG_QUEST		(A)
#define MSG_PURGE		(B)
#define MSG_CAB_ACCEPT		(C)
#define MSG_CAB_REJECT		(D)
#define MSG_CAB_PROM		(E)
#define MSG_CAB_DEMO		(F)
#define MSG_CAB_EXPEL		(G)
#define MSG_CAB_TEXPEL		(H)
#define MSG_BOUNTY		(I)
#define MSG_BOUNTY_REWARD	(J)
#define MSG_BOUNTY_RETURN	(K)
#define MSG_CABAL_ALLOW		(L)

/* COLOR flags, used for color stuff */
#define COLOR_ON		(A)	//color on
#define COLOR_DAMAGE		(B)	//damage color code on

/* AFFECT flags, used for affects */
#define AFLAG_NONE		0

/* GAME flags, used for gameplay */

#define	GAME_AMOB		(A)	//Autoaim on mobs
#define	GAME_APC		(B)	//Autoaim on PC's
#define	GAME_AOBJ		(C)	//Autoaim on Obj's
#define	GAME_SHOW_CABAL		(D)	//Shows char's cabal to players
#define	GAME_NOREQ_CABAL	(E)	//Doest auto-requip
#define	GAME_NOEXP		(F)	//Used to prevent getting exp
#define	GAME_STAIN		(G)	//Cheating Stain flag.
#define GAME_SEX		(H)     //Autosex on whois list
#define GAME_NEWBIE		(I)     //Allows player to use newbie chat
#define GAME_PKWARN		(J)     //Player recived AUTO warning about pk
#define GAME_NOSPELLS		(K)     //Player cannot cast spells
#define GAME_ASHEATH		(L)     //Players auto sheath in cities
#define GAME_MERCY		(M)     //Stun death instead of regular death
#define GAME_NOQUESTOR		(N)     //Cannot become master questor
#define GAME_NOALIAS		(O)     //aliases disabled
#define GAME_NEWSCORE		(P)     //new score sheet
#define GAME_DNDS		(Q)	// DND SPELL SYSTEM USED
#define GAME_NOAUTOFIRE		(R)	//no ranged weapons in combat

/* RT comm flags -- may be used on both mobs and chars */
#define COMM_QUIET              (A)
#define COMM_DEAF            	(B)
#define COMM_NOWIZ              (C)
#define COMM_NOAUCTION          (D)
#define COMM_NOPRAY             (E)
#define COMM_NOQUESTION         (F)
#define COMM_YELLSOFF           (G)
#define COMM_NOCABAL            (H)
#define COMM_NONEWBIE           (I)
#define COMM_NOCLAN             (J)
#define COMM_TELEPATH           (X)
/* display flags */
#define COMM_COMPACT            (L)
#define COMM_BRIEF		(M)
#define COMM_PROMPT		(N)
#define COMM_TELNET_GA		(P)
#define COMM_SHOW_AFFECTS	(Q)

/* penalties */
#define COMM_NOEMOTE		(T)
#define COMM_NOYELL             (U)
#define COMM_NOTELL		(V)
#define COMM_NOCHANNELS		(W)
#define COMM_SNOOP_PROOF	(Y)
#define COMM_AFK		(Z)

/* WIZnet flags */
#define WIZ_ON			(A)
#define WIZ_TICKS		(B)
#define WIZ_LOGINS		(C)
#define WIZ_SITES		(D)
#define WIZ_LINKS		(E)
#define WIZ_DEATHS		(F)
#define WIZ_RESETS		(G)
#define WIZ_MOBDEATHS		(H)
#define WIZ_FLAGS		(I)
#define WIZ_PENALTIES		(J)
#define WIZ_FIGHTING		(K)
#define WIZ_LEVELS		(L)
#define WIZ_SECURE		(M)
#define WIZ_SWITCHES		(N)
#define WIZ_SNOOPS		(O)
#define WIZ_RESTORE		(P)
#define WIZ_LOAD		(Q)
#define WIZ_NEWBIE		(R)
#define WIZ_NOTES		(S)
#define WIZ_SPAM		(T)
#define WIZ_MEMORY		(U)
#define WIZ_SAVE		(V)

/* Prototype for a mob. */
struct	mob_index_data
{
    MOB_INDEX_DATA *	next;
    SPEC_FUN *		spec_fun;
    SHOP_DATA *		pShop;
    AREA_DATA *         area;
    CABAL_INDEX_DATA*   pCabal;
  /* OLD MPROG */
    MPROG_DATA *        mobprogs;
    TRIGFILE_DATA *	trigfile;

  /* NEW MPROG */
    PROG_LIST *		mprogs;
    long		mprog_flags;

    char *		player_name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    long		act;
    long		act2;
    sh_int		race;
    sh_int		size;
    sh_int		start_pos;
    sh_int		default_pos;
    bool		new_format;
    long		affected_by;
    long		affected2_by;
    long		off_flags;
    long		imm_flags;
    long		res_flags;
    long		vuln_flags;
    long                gold;
    long		form;
    long		parts;
    int			vnum;
    sh_int		group;
    sh_int		count;
    sh_int		killed;
    sh_int		alignment;
    sh_int		level;
    sh_int		hitroll;
    sh_int		hit[3];
    sh_int		mana[3];
    sh_int		damage[3];
    sh_int		ac[4];
    sh_int 		dam_type;
    sh_int		sex;
    sh_int              progtypes;
};

/* Dennis */
/* One character (PC or NPC). */
struct	char_data
{
    CHAR_DATA *		next;
    CHAR_DATA *		next_player;
    CHAR_DATA *		next_in_room;
    CHAR_DATA *		master;
    CHAR_DATA *		leader;
    CHAR_DATA *		fighting;
    CHAR_DATA *		reply;
    CHAR_DATA *		pet;
    CHAR_DATA *         hunting;
    CHAR_DATA *		summoner;
    CHAR_DATA *		doppel;
    CHAR_DATA *		mprog_target;
    sh_int		mprog_delay;
    SPEC_FUN *		spec_fun;
    void*		spec_data;	//misc pointer for persistent data
    PATH_QUEUE*		spec_path;	//path data for spec progs
    MOB_INDEX_DATA *	pIndexData;
    DESCRIPTOR_DATA *	desc;
    AFFECT_DATA *	affected;
    AFFECT_DATA *	affected2;
    OBJ_DATA *		carrying;
    OBJ_DATA *		on;
    TRAP_DATA*		traps[MAX_TRAPS];
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	was_in_room;
    PC_DATA *		pcdata;
    CABAL_DATA*		pCabal;
    char *		name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    char *		prompt;
    char *		prefix;
    time_t		logon;
    time_t		logoff;
    int			save_ver;
    sh_int		lines;  /* for the pager */
    sh_int		position;
    bool		extracted;
    sh_int		reload;		//used by ranged weapons to count down to 0 to reload
    sh_int		armor[4];
    sh_int		hit;
    sh_int		mana;
    sh_int		move;
    sh_int		max_hit;
    sh_int		max_mana;
    sh_int		max_move;
    sh_int			perm_stat[MAX_STATS];
    sh_int			mod_stat[MAX_STATS];
    sh_int			aff_mod[MAX_MODIFIER];
    bool		valid;
    bool		caballeader;
    sh_int		hunttime;
    sh_int		group;
    sh_int              hometown;
    sh_int              tattoo;
    sh_int		sex;
    sh_int		class;
    sh_int		race;
    sh_int		level;
    sh_int		trust;
    sh_int		timer;
    sh_int		idle;
    sh_int		wait;
    int			played;
    int			in_bank;
    long		exp;
    long                max_exp;
    sh_int              mpactnum;
    long		special;
    long		id;
    long		gold;
    long		act;
    long		act2;
    long		comm;   /* RT added to pad the vector */
    long		wiznet; /* wiz stuff */
    long		game;	/* gameplay flags (autoaim, autohealth, etc)*/
    int			color;  /* color flags */
    long		imm_flags;
    long		res_flags;
    long		vuln_flags;
    long                affected_by;
    long                affected2_by;
    sh_int		invis_level;
    sh_int		incog_level;
    sh_int		practice;
    sh_int		train;
    sh_int		carry_weight;
    sh_int		carry_number;
    sh_int		savingaffl;
    sh_int		savingmaled;
    sh_int		savingmental;
    sh_int		savingbreath;
    sh_int		savingspell;
    sh_int		alignment;
    sh_int		hitroll;
    sh_int		damroll;
    sh_int		wimpy;
    int			homevnum;
    sh_int		status;
    long		form;
    long		parts;
    sh_int		size;
    long		off_flags;
    sh_int		damage[3];
    sh_int		dam_type;
    sh_int		start_pos;
    sh_int		default_pos;
};

/* data for custom display of effects */
struct affect_disp_data{
  int	gsn;
  char name[3];
};

/* Data which only PC's have. */
struct	pc_data
{
  PC_DATA *		next;
  BUFFER * 		buffer;
  MOB_INDEX_DATA *	pStallion;	//points to index to be used
  CHAR_DATA *		familiar;
  CHAR_DATA *		guarding;
  CHAR_DATA *		guarded_by;
  CHAR_DATA *		eavesdropped;
  CHAR_DATA *		eavesdropping;
  CHAR_DATA *		duet;
  NOTE_DATA *		pnote;
  VOTE_DATA *		pvote;
  BBID_DATA *		pbbid;
  BOUNTY_DATA*		pbounty;
  CVROOM_DATA*		pcvroom;
  TOME_DATA*		ptome;
  SKILL_DATA *		newskills;
  QUEST_DATA*		quests;
  CABAL_DATA*	        last_cabal;
  CMEMBER_DATA*		member;
  ALIAS_DATA*		aliases;
  DNDS_MEMORY		dndmemory[MAX_SKILL];
  DNDS_HEADER*		dndspells;	//pointer to one of the templates
  DNDS_HEADER		dndtemplates[DNDS_TEMPLATES][DNDS_MAXLEVEL];
  ROOM_INDEX_DATA*	army_focus;
  ROOM_INDEX_DATA*	last_focus;
  AQUEST_DATA		aquest;
  AFF_DISPLAY		custom_aff[MAX_AFF_DISPLAY];
  char *		dndtemplate_name[DNDS_TEMPLATES];
  char *		pwd;
  char *		pwddel;
  char *		pwdimm;
  char *		bamfin;
  char *		bamfout;
  char *		title;
  char *		ignore;
  char *		family;
  char *		alias;
  char *		race;
  char *		deity;
  char *		clan;
  char *		royal_guards;
  bool			royal_confirm;
  bool			valid;
  bool			group_known	[MAX_GROUP];
  bool              	confirm_delete;
  bool			confirm_quit;
  sh_int		month;
  int			mplayed;
  int			divfavor;
  int			messages;	//flags for general message on login
  long			cpoints;	//cabal points
  int			clan_points;
  sh_int		ss_picked[MAX_GROUPS];	//how many skills picked group
  sh_int		psalms[MAX_PSALM];	//psalms gained
  sh_int		psalm_pen;		//maximum psalm penalty
  int			beacon;
  sh_int		way;
  sh_int		ethos;
  sh_int		last_level;
  sh_int		security;
  sh_int		kpc;
  sh_int		knpc;
  sh_int		dpc;
  sh_int		dnpc;
  sh_int		dall;
  sh_int 		flagged;
  sh_int 		rank;
  sh_int 		clan_rank;
  sh_int 		clan_handle;
  sh_int 		beast_type;
  sh_int		enemy_race;
  byte			race_kills[MAX_PC_RACE];
  time_t		fight_delay;
  time_t		pk_delay;
  time_t		ghost;
  time_t		last_note;
  time_t		last_idea;
  time_t		last_penalty;
  time_t		last_news;
  time_t		last_changes;
  time_t		last_application;
  time_t		last_vote;
  time_t		last_bounty;
  time_t		bank_stamp;
  long			birth_date;
  sh_int		vote_skip;
  time_t		last_app;
  sh_int		perm_hit;
  sh_int		perm_mana;
  sh_int		perm_move;
  sh_int		true_sex;
  sh_int		condition	[4];
  sh_int		learned		[MAX_SKILL];
  sh_int		to_learned	[MAX_SKILL];
  int			progress	[MAX_SKILL];
  sh_int		songlearned	[MAX_SONG];
  sh_int		roomnum;
  sh_int		transnum;
  sh_int		quest[MAX_QUEST];
  sh_int		questorder[MAX_QUEST];
  sh_int		anatomy[MAX_ANATOMY];
  sh_int		wep_pos;	//weapon position (low, high, normal)
  sh_int		bat_pos;	//battle position (left, right, middle)
  sh_int		max_quests;	//total of perm quests
  sh_int		body_type;
  sh_int		face_type;
  sh_int		skin_color;
  sh_int		eye_color;
  sh_int		hair_color;
  sh_int		hair_length;
  sh_int		hair_texture;
  sh_int		facial_hair;
  sh_int		start_wep;
  sh_int		nagtimer;
  int			perk_bits;
  bool			lefthanded;
  bool			fDetail;
};


/* Liquids. */
#define LIQ_WATER        0

struct	liq_type
{
    char *	liq_name;
    char *	liq_color;
    sh_int	liq_affect[5];
};

/* Extra description data for a room or object. */
struct	extra_descr_data
{
    EXTRA_DESCR_DATA *next;	/* Next in list                     */
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
    bool valid;
};

/* Prototype for an object. */
struct	obj_index_data
{
    OBJ_INDEX_DATA *	next;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    AREA_DATA *         area;
    OBJ_SPELL_DATA *    spell;
    OBJ_MESSAGE_DATA *  message;
    PROG_LIST *		oprogs;
    CABAL_INDEX_DATA*	pCabal;
    long		oprog_flags;
    char *		name;
    char *		short_descr;
    char *		description;
    char *		material;
    bool		new_format;
    int			vnum;
    sh_int		reset_num;
    sh_int		item_type;
    sh_int		level;
    sh_int		class;
    sh_int		race;
    sh_int 		condition;
    sh_int		count;
    sh_int              weight;
    int			extra_flags;
    int			wear_flags;
    int			cost;
    int			value[5];
};

/* One object. */
struct	obj_data
{
    OBJ_DATA *		next;
    OBJ_DATA *		next_content;
    OBJ_DATA *		next_bidobj;
    OBJ_DATA *		contains;
    OBJ_DATA *		in_obj;
    OBJ_DATA *		on;
    CHAR_DATA *		carried_by;
    BBID_DATA *		bbid;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    TRAP_DATA*		traps;
    OBJ_SPELL_DATA*	spell;
    OBJ_INDEX_DATA *	pIndexData;
    ROOM_INDEX_DATA *	in_room;
    CABAL_DATA*		pCabal;
    CHAR_DATA *		oprog_target;
    sh_int		oprog_delay;
    char *		name;
    char *		short_descr;
    char *		description;
    char *		material;
    long		owner;
    int			recall;
    int			extra_flags;
    long		wear_flags;
    int			cost;
    int			value	[5];
    bool		valid;
    bool		enchanted;
    bool		eldritched;
    bool		extracted;
    sh_int		item_type;
    sh_int		wear_loc;
    sh_int		weight;
    sh_int		level;
    sh_int 		condition;
    sh_int 		race;
    sh_int 		class;
    sh_int		timer;
    int			homevnum;
    int			vnum;
    sh_int		idle;
    int			save_ver;
};

/* Exit data. */
struct	exit_data
{
  ROOM_INDEX_DATA*	to_room;
  int			vnum;
  EXIT_DATA *		next;
  EXIT_DATA *		next_in_area;
  TRAP_DATA*		traps;
  char *		keyword;
  char *		description;
  sh_int		exit_info;
  sh_int		key;
  int			rs_flags;
  sh_int		orig_door;
};


/* Area-reset definition.        *
 *   '*': comment                *
 *   'M': read a mobile          *
 *   'O': read an object         *
 *   'P': put object in object   *
 *   'G': give object to mobile  *
 *   'E': equip object to mobile *
 *   'R': randomize room exits   *
 *   'T': trap on object or exit *
 *   'S': stop (end of list)     */
struct	reset_data
{
    RESET_DATA *	next;
    char		command;
    int	arg1;
    int	arg2;
    int	arg3;
    int	arg4;

};

/* Area definition. */
struct	area_data
{
    AREA_DATA *		next;
    RESET_DATA *	reset_first;
    RESET_DATA *	reset_last;
    CABAL_DATA *	pCabal;	//cabal which owns area (royal, or area cabal)
    TOWER_DATA*		towers; //data for each cabal's towers in area
    EXIT_DATA*		exits;
    RAID_DATA*          raid;
    char *		name;
    char *		file_name;
    char *		credits;
    char *              builders;
    char *		prefix;
    time_t		last_raid;
    bool		empty;
    int                 vnum;
    int			startroom;
    int                 area_flags;
    int			support;	//value of support this area grants when taken over
    sh_int              security;
    sh_int		age;
    sh_int		nplayer;
    sh_int		low_range;
    sh_int		high_range;
    int			min_vnum;
    int			max_vnum;
    sh_int		idle;
    sh_int              bastion_max;
    sh_int              bastion_current;
    sh_int              crimes[MAX_CRIME];
    sh_int		cabal_distance[MAX_CABAL];
    sh_int		cabal_influence[MAX_CABAL];
};

/* Room type. */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    CHAR_DATA *		people;
    ARMY_ROOM_DATA	room_armies;
    OBJ_DATA *		contents;
    EXTRA_DESCR_DATA *	extra_descr;
    AREA_DATA *		area;
    EXIT_DATA *		exit	[6];
    EXIT_DATA * 	old_exit[6];
    RESET_DATA *        reset_first;
    RESET_DATA *        reset_last;
    PROG_LIST *		rprogs;
    CHAR_DATA *		rprog_target;
    CABAL_DATA*		pCabal;
    long		rprog_flags;
    sh_int		rprog_delay;
    char *		name;
    char *		description;
    char *		description2;
    int			room_flags;
    int			room_flags2;
    int			vnum;
    int			cabal_vnum;
    Double_List *	watch_vnums;
    sh_int		light;
    sh_int		sector_type;
    sh_int		heal_rate;
    sh_int 		mana_rate;
    sh_int		psitimer;
    sh_int		temp;
    long		tracks[MAX_ANATOMY];//contains mud time when someone last stepped into this room
};

/* Types of attacks.                               *
 * Must be non-overlapping with spell/skill types, *
 * but may be arbitrary beyond that.               */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000
#define TYPE_NOBLOCKHIT              10000

/* Target types. */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4
#define TAR_OBJ_CHAR_DEF	    5
#define TAR_OBJ_CHAR_OFF	    6
#define TAR_OBJ_ROOM                7
#define TAR_DOOR                    8
#define TAR_ROOM_CHAR_DEF           9
#define TAR_OBJ_WORN		    10


#define TARGET_CHAR		    0
#define TARGET_OBJ		    1
#define TARGET_ROOM		    2
#define TARGET_NONE		    3
#define TARGET_DOOR                 4

/* Skills include spells as a particular case. */
struct	skill_type
{
    char *	name;			/* Name of skill		*/
    sh_int	skill_level[MAX_CLASS];	/* Level needed by class	*/
    sh_int	rating[MAX_CLASS];	/* How hard it is to learn	*/
    SPELL_FUN *	spell_fun;		/* Spell pointer (for spells)	*/
    sh_int	target;			/* Legal targets		*/
    sh_int	minimum_position;	/* Position for caster / user	*/
    sh_int *	pgsn;			/* Pointer to associated gsn	*/
    sh_int	spell_type;		/* Spell type			*/
    sh_int	min_mana;		/* Minimum mana used		*/
    sh_int	beats;			/* Waiting time after use	*/
    char *	noun_damage;		/* Damage message		*/
    char *	msg_off;		/* Wear off message		*/
    char *      msg_off2;               /* Wear off message to others   */
    int		flags;			/* Special behavior flags	*/
};

struct psalm_type{
  char*	name;		//showin in format: Psalm of <name>
  sh_int* gsn;		//gsn of the spell in skill_table
  sh_int* check;	//gsn/sn to check for when checking for presence

  int chant;		//wait before psalm is cast (meditation length)
  int wait;		//wait before a new chant can be attempted
  bool fTwo;		//can another prayer be put after this one
  bool fRand;		//can this prayer be the random start one
  bool fGroup;		//can this prayer be stared with a group
};

struct song_type
{
    char *      name;                   /* Name of song */
    sh_int	skill_level;		/* Level needed by bards*/
    sh_int	rating;			/* How hard it is to learn */
    SONG_FUN *  song_fun;               /* function called for song */
    sh_int      minimum_position;       /* min position to play song */
    bool	refrain;		/* can song be refrained */
    bool	instrument;		/* does song require instrument */
    bool	duet;			/* is song duetable		*/
    sh_int *	pgsn;			/* Pointer to associated gsn	*/
    sh_int	spell_type;		/* Spell type			*/
    sh_int      min_mana;               /* min mana song takes */
    sh_int      beats;                  /* Waiting time after use */
    char *	noun_damage;		/* Damage message		*/
    char *	msg_off;		/* Wear off message		*/
    char *      msg_off2;               /* Wear off message to others   */
};



struct effect_type
{
  char*   name;              /* Name of skill*/

  sh_int *pgen;              /* Pointer to associated gen*/

  char*   noun_damage;       /* Damage message*/

  char*   msg_off;           /* StandardExpiry Message to_char*/
  char*   msg_off2;          /* StandardExpiry Message to_room*/

  long    flags;             /*Bitwise flags (RESERVED)*/
  long    flags2;            /*Bitwise flags (RESERVED)*/

  EFF_UPDATE_FUN *effect_update_fun [MAX_EFF_UPDATE];
  /* Function pointers for updates*/


  sh_int  PreBeats;          /* RESERVED*/
  sh_int  beats;             /* Waiting time after  use*/

  int     value1[MAX_VALUE1]; /*RESERVED*/
  int     value2[MAX_VALUE2]; /*RESERVED*/
  int     value3[MAX_VALUE3]; /*RESERVED*/

  void* paf;          /*RESERVED*/

};



struct  group_type
{
    char *	name;
    sh_int	rating[MAX_CLASS];
    char *	spells[MAX_IN_GROUP];
};



/* common return values for races */
extern int     grn_illithid;
extern int     grn_half_elf;
extern int     grn_faerie;
extern int     grn_feral;
extern int     grn_undead;
extern int     grn_demon;
extern int     grn_stone;
extern int     grn_fire;
extern int     grn_werebeast;
extern int     grn_vampire;
extern int     grn_ogre;
extern int     grn_minotaur;

/* common return values for classes */
extern int     gcn_monk;
extern int     gcn_psi;
extern int     gcn_vampire;
extern int     gcn_ranger;
extern int     gcn_dk;
extern int     gcn_crusader;
extern int     gcn_ninja;
extern int     gcn_necro;
extern int     gcn_bard;
extern int     gcn_adventurer;
extern int     gcn_blademaster;
extern int     gcn_warrior;
extern int     gcn_cleric;
extern int     gcn_shaman;
extern int     gcn_healer;
extern int     gcn_paladin;
extern int     gcn_druid;

/* These are skill_lookup return values for common skills and spells. */

extern sh_int  gsn_avatar_of_steel;
extern sh_int  gsn_age;
extern sh_int  gsn_backstab;
extern sh_int  gsn_battlesphere;
extern sh_int  gsn_battlefocus;
extern sh_int  gsn_battletrance;
extern sh_int  gsn_blanket;
extern sh_int  gsn_bladework;
extern sh_int  gsn_bladestorm;
extern sh_int  gsn_battlestance;
extern sh_int  gsn_behead;
extern sh_int  gsn_arms_main;
extern sh_int  gsn_archery;
extern sh_int  gsn_baptize;
extern sh_int  gsn_bolo;
extern sh_int  gsn_bribe;
extern sh_int  gsn_breach;
extern sh_int  gsn_2hands;
extern sh_int  gsn_2h_handling;
extern sh_int  gsn_2h_tactics;
extern sh_int  gsn_2h_mastery;
extern sh_int  gsn_dodge;
extern sh_int  gsn_dysentery;
extern sh_int  gsn_doomsinger;
extern sh_int  gsn_dheal;
extern sh_int  gsn_sretrib;
extern sh_int  gsn_soul_tap;
extern sh_int  gsn_subvocal;
extern sh_int  gsn_vitality;
extern sh_int  gsn_vredem;
extern sh_int  gsn_virtues;
extern sh_int  gsn_dwrath;
extern sh_int  gsn_sbane;
extern sh_int  gsn_stand_ground;
extern sh_int  gsn_steel_wall;
extern sh_int  gsn_pwater;
extern sh_int  gsn_probe;
extern sh_int  gsn_icefire;
extern sh_int  gsn_icalm;
extern sh_int  gsn_identify;
extern sh_int  gsn_smight;
extern sh_int  gsn_dmight;
extern sh_int  gsn_dvoid;
extern sh_int  gsn_envenom;
extern sh_int  gsn_enfeeblement;
extern sh_int  gsn_energy_drain;
extern sh_int  gsn_hide;
extern sh_int  gsn_high_sight;
extern sh_int  gsn_peek;
extern sh_int  gsn_pick_lock;
extern sh_int  gsn_sneak;
extern sh_int  gsn_steal;
extern sh_int  gsn_pry;
extern sh_int  gsn_protective_shield;
extern sh_int  gsn_plant;
extern sh_int  gsn_planar_seal;
extern sh_int  gsn_preserve;
extern sh_int  gsn_predict;
extern sh_int  gsn_disarm;
extern sh_int  gsn_weapon_cleave;
extern sh_int  gsn_webbing;
extern sh_int  gsn_wlock;
extern sh_int  gsn_weaponcraft;
extern sh_int  gsn_weaken;
extern sh_int  gsn_shield_cleave;
extern sh_int  gsn_shadow_strike;
extern sh_int  gsn_shadowdancer;
extern sh_int  gsn_shadowmaster;
extern sh_int  gsn_sheath;
extern sh_int  gsn_enhanced_damage;
extern sh_int  gsn_kick;
extern sh_int  gsn_parry;
extern sh_int  gsn_path_anger;
extern sh_int  gsn_path_fury;
extern sh_int  gsn_path_dev;
extern sh_int  gsn_fpart;
extern sh_int  gsn_footwork;
extern sh_int  gsn_fury;
extern sh_int  gsn_first_parry;
extern sh_int  gsn_fear;
extern sh_int  gsn_feign;
extern sh_int  gsn_drug_use;
extern sh_int  gsn_dreamprobe;
extern sh_int  gsn_field_fear;
extern sh_int  gsn_second_parry;
extern sh_int  gsn_serpent;
extern sh_int  gsn_secret_arts;
extern sh_int  gsn_rescue;
extern sh_int  gsn_rapid_fire;
extern sh_int  gsn_raise_morale;
extern sh_int  gsn_rev_time;
extern sh_int  gsn_resurrection;
extern sh_int  gsn_second_attack;
extern sh_int  gsn_third_attack;
extern sh_int  gsn_tracker;
extern sh_int  gsn_transform;
extern sh_int  gsn_torment_bind;
extern sh_int  gsn_turn_undead;
extern sh_int  gsn_blindness;
extern sh_int  gsn_bloodhaze;
extern sh_int  gsn_bloodvow;
extern sh_int  gsn_chant;
extern sh_int  gsn_cyclone;
extern sh_int  gsn_charm_person;
extern sh_int  gsn_curse;
extern sh_int  gsn_cure_light;
extern sh_int  gsn_curse_weapon;
extern sh_int  gsn_invis;
extern sh_int  gsn_invigorate;
extern sh_int  gsn_insight;
extern sh_int  gsn_mark_prey;
extern sh_int  gsn_marksman;
extern sh_int  gsn_maelstrom;
extern sh_int  gsn_mark_fear;
extern sh_int  gsn_mass_invis;
extern sh_int  gsn_masochism;
extern sh_int  gsn_plague;
extern sh_int  gsn_poison;
extern sh_int  gsn_powerstrike;
extern sh_int  gsn_sleep;
extern sh_int  gsn_fly;
extern sh_int  gsn_sanctuary;
extern sh_int  gsn_sacred_runes;
extern sh_int  gsn_adv_handling;
extern sh_int  gsn_adrenaline_rush;
extern sh_int  gsn_axe;
extern sh_int  gsn_avenger;
extern sh_int  gsn_apierce;
extern sh_int  gsn_armorcraft;
extern sh_int  gsn_armored_rush;
extern sh_int  gsn_armor_enhance;
extern sh_int  gsn_abduct;
extern sh_int  gsn_dagger;
extern sh_int  gsn_fired;
extern sh_int  gsn_flail;
extern sh_int  gsn_mace;
extern sh_int  gsn_polearm;
extern sh_int  gsn_shield_block;
extern sh_int  gsn_spear;
extern sh_int  gsn_spirit_bind;
extern sh_int  gsn_staff;
extern sh_int  gsn_sword;
extern sh_int  gsn_whip;
extern sh_int  gsn_sword_mastery;
extern sh_int  gsn_exotic_mastery;
extern sh_int  gsn_exile;
extern sh_int  gsn_dagger_mastery;
extern sh_int  gsn_spear_mastery;
extern sh_int  gsn_axe_mastery;
extern sh_int  gsn_mace_mastery;
extern sh_int  gsn_flail_mastery;
extern sh_int  gsn_staff_mastery;
extern sh_int  gsn_whip_mastery;
extern sh_int  gsn_polearm_mastery;
extern sh_int  gsn_shield_expert;
extern sh_int  gsn_exotic_expert;
extern sh_int  gsn_sword_expert;
extern sh_int  gsn_dagger_expert;
extern sh_int  gsn_spear_expert;
extern sh_int  gsn_mace_expert;
extern sh_int  gsn_axe_expert;
extern sh_int  gsn_flail_expert;
extern sh_int  gsn_staff_expert;
extern sh_int  gsn_whip_expert;
extern sh_int  gsn_polearm_expert;
extern sh_int  gsn_bash;
extern sh_int  gsn_basic_armor;
extern sh_int  gsn_adv_armor;
extern sh_int  gsn_hunt;
extern sh_int  gsn_berserk;
extern sh_int  gsn_dirt;
extern sh_int  gsn_hand_to_hand;
extern sh_int  gsn_haymaker;
extern sh_int  gsn_harm;
extern sh_int  gsn_trip;
extern sh_int  gsn_trample;
extern sh_int  gsn_lash;
extern sh_int  gsn_leash;
extern sh_int  gsn_leadership;
extern sh_int  gsn_fast_healing;
extern sh_int  gsn_false_weapon;
extern sh_int  gsn_fade;
extern sh_int  gsn_haggle;
extern sh_int  gsn_lore;
extern sh_int  gsn_lotus_sc;
extern sh_int  gsn_meditation;
extern sh_int  gsn_melee;
extern sh_int  gsn_scrolls;
extern sh_int  gsn_staves;
extern sh_int  gsn_wands;
extern sh_int  gsn_wanted;
extern sh_int  gsn_recall;
extern sh_int  gsn_hunt;
extern sh_int  gsn_dual_wield;
extern sh_int  gsn_death_grasp;
extern sh_int  gsn_deathweaver;
extern sh_int  gsn_deathweaver;
extern sh_int  gsn_death_kiss;
extern sh_int  gsn_death_shroud;
extern sh_int  gsn_phantasmal_griffon;
extern sh_int  gsn_illusionary_spectre;
extern sh_int  gsn_phantom_dragon;
extern sh_int  gsn_trance;
extern sh_int  gsn_blind_fighting;
extern sh_int  gsn_shield_disarm;
extern sh_int  gsn_rake;
extern sh_int  gsn_misdirection;
extern sh_int  gsn_missile_shield;
extern sh_int  gsn_shadowform;
extern sh_int  gsn_shadow_door;
extern sh_int  gsn_fourth_attack;
extern sh_int  gsn_deathblow;
extern sh_int  gsn_com_lan;
extern sh_int  gsn_counter;
extern sh_int  gsn_lay_on_hands;
extern sh_int  gsn_detect_hidden;
extern sh_int  gsn_detect_traps;
extern sh_int  gsn_deteriorate;
extern sh_int  gsn_cleave;
extern sh_int  gsn_assassinate;
extern sh_int  gsn_blackjack;
extern sh_int  gsn_gag;
extern sh_int  gsn_strangle;
extern sh_int  gsn_acupuncture;
extern sh_int  gsn_herb;
extern sh_int  gsn_hex;
extern sh_int  gsn_tame;
extern sh_int  gsn_beast_call;
extern sh_int  gsn_rage;
extern sh_int  gsn_regeneration;
extern sh_int  gsn_bodyslam;
extern sh_int  gsn_dual_backstab;
extern sh_int  gsn_grapple;
extern sh_int  gsn_greenheart;
extern sh_int  gsn_charge;
extern sh_int  gsn_shoot;
extern sh_int  gsn_ambush;
extern sh_int  gsn_endure;
extern sh_int  gsn_barkskin;
extern sh_int  gsn_warcry;
extern sh_int  gsn_circle;
extern sh_int  gsn_swing;
extern sh_int  gsn_headbutt;
extern sh_int  gsn_camouflage;
extern sh_int  gsn_chameleon;
extern sh_int  gsn_quiet_movement;
extern sh_int  gsn_keen_sight;
extern sh_int  gsn_vanish;
extern sh_int  gsn_voodoo_spell;
extern sh_int  gsn_chii;
extern sh_int  gsn_blindness_dust;
extern sh_int  gsn_blink;
extern sh_int  gsn_poison_smoke;
extern sh_int  gsn_butcher;
extern sh_int  gsn_dual_parry;
extern sh_int  gsn_caltraps;
extern sh_int  gsn_nerve;
extern sh_int  gsn_nerve_amp;
extern sh_int  gsn_numbness;
extern sh_int  gsn_riposte;
extern sh_int  gsn_ripple;
extern sh_int  gsn_righteous;
extern sh_int  gsn_truesight;
extern sh_int  gsn_trophy;
extern sh_int  gsn_spellbane;
extern sh_int  gsn_spellkiller;
extern sh_int  gsn_spell_blast;
extern sh_int  gsn_doppelganger;
extern sh_int  gsn_mirror_image;
extern sh_int  gsn_mirage;
extern sh_int  gsn_mirror_cloak;
extern sh_int  gsn_cognizance;
extern sh_int  gsn_stance;
extern sh_int  gsn_forms;
extern sh_int  gsn_forestmeld;
extern sh_int  gsn_wanted;
extern sh_int  gsn_judge;
extern sh_int  gsn_spy;
extern sh_int  gsn_knight;
extern sh_int  gsn_guard;
extern sh_int  gsn_stallion;
extern sh_int  gsn_relax;
extern sh_int  gsn_garble;
extern sh_int  gsn_brew;
extern sh_int  gsn_challenge;
extern sh_int  gsn_fatality;
extern sh_int  gsn_bounty_collect;
extern sh_int  gsn_climb;
extern sh_int  gsn_damnation;
extern sh_int  gsn_dam_masteryB;
extern sh_int  gsn_dam_masteryS;
extern sh_int  gsn_dam_masteryP;
extern sh_int  gsn_flashfire;
extern sh_int  gsn_drained;
extern sh_int  gsn_mortally_wounded;
extern sh_int  gsn_mercy_wait;
extern sh_int  gsn_mounted;
extern sh_int  gsn_ancient_lore;
extern sh_int  gsn_invoke;
extern sh_int  gsn_mimic;
extern sh_int  gsn_camp;
extern sh_int  gsn_scout;
extern sh_int  gsn_scourge;
extern sh_int  gsn_throw;
extern sh_int  gsn_roar;
extern sh_int  gsn_edge_craft;
extern sh_int  gsn_counterfeit;
extern sh_int  gsn_ranger_staff;
extern sh_int  gsn_healer_staff;
extern sh_int  gsn_heal;
extern sh_int  gsn_druid_staff;
extern sh_int  gsn_familiar_link;
extern sh_int  gsn_extort;
extern sh_int  gsn_squire;
extern sh_int  gsn_silence;
extern sh_int  gsn_blasphemy;
extern sh_int  gsn_vomit;
extern sh_int  gsn_defecate;
extern sh_int  gsn_insomnia;
extern sh_int  gsn_mystic_healing;
extern sh_int  gsn_shield_of_words;
extern sh_int  gsn_children_sky;
extern sh_int  gsn_pilfer;
extern sh_int  gsn_mind_link;
extern sh_int  gsn_mindsurge;
extern sh_int  gsn_mind_disrupt;
extern sh_int  gsn_life_insurance;
extern sh_int  gsn_lifedrain;
extern sh_int  gsn_linked_mind;
extern sh_int  gsn_ensnare;
extern sh_int  gsn_timer;
extern sh_int  gsn_time_comp;
extern sh_int  gsn_pixie_dust;
extern sh_int  gsn_bat_form;
extern sh_int  gsn_batter;
extern sh_int  gsn_wolf_form;
extern sh_int  gsn_mist_form;
extern sh_int  gsn_double_kick;
extern sh_int  gsn_coffin;
extern sh_int  gsn_stake;
extern sh_int  gsn_eavesdrop;
extern sh_int  gsn_kinetics;
extern sh_int  gsn_thrust;
extern sh_int  gsn_ecstacy;
extern sh_int  gsn_anatomy;
extern sh_int  gsn_beads;
extern sh_int  gsn_martial_arts_b;
extern sh_int  gsn_martial_arts_a;
extern sh_int  gsn_kickboxing_b;
extern sh_int  gsn_kickboxing_a;
extern sh_int  gsn_double_grip;
extern sh_int  gsn_monkey;
extern sh_int  gsn_tiger;
extern sh_int  gsn_crane;
extern sh_int  gsn_mantis;
extern sh_int  gsn_drunken;
extern sh_int  gsn_horse;
extern sh_int  gsn_holy_hands;
extern sh_int  gsn_dragon;
extern sh_int  gsn_buddha;
extern sh_int  gsn_sense_motion;
extern sh_int  gsn_balance;
extern sh_int  gsn_aura;
extern sh_int  gsn_catalepsy;
extern sh_int  gsn_healing;
extern sh_int  gsn_kinesis;
extern sh_int  gsn_mind_thrust;
extern sh_int  gsn_mind_disruption;
extern sh_int  gsn_mind_blast;
extern sh_int  gsn_psalm_master;
extern sh_int  gsn_psychic_crush;
extern sh_int  gsn_psionic_blast;
extern sh_int  gsn_psi_amp;
extern sh_int  gsn_psychic_purge;
extern sh_int  gsn_pugil;
extern sh_int  gsn_puppet_master;
extern sh_int  gsn_flesh_armor;
extern sh_int  gsn_detect_aura;
extern sh_int  gsn_displacement;
extern sh_int  gsn_dispel_magic;
extern sh_int  gsn_iron_will;
extern sh_int  gsn_ironarm;
extern sh_int  gsn_iron_curtain;
extern sh_int  gsn_bio_manipulation;
extern sh_int  gsn_ego_whip;
extern sh_int  gsn_tele_lock;
extern sh_int  gsn_telepathy;
extern sh_int  gsn_temp_storm;
extern sh_int  gsn_there_not_there;
extern sh_int  gsn_levitate;
extern sh_int  gsn_gravitate;
extern sh_int  gsn_bio_feedback;
extern sh_int  gsn_pyramid_of_force;
extern sh_int  gsn_body_weaponry;
extern sh_int  gsn_ectoplasm;
extern sh_int  gsn_hypnosis;
extern sh_int  gsn_forget;
extern sh_int  gsn_graft_weapon;
extern sh_int  gsn_mental_barrier;
extern sh_int  gsn_mental_knife;
extern sh_int  gsn_empower;
extern sh_int  gsn_offensive;
extern sh_int  gsn_onslaught;
extern sh_int  gsn_omnipotence;
extern sh_int  gsn_ogtrium;
extern sh_int  gsn_ocularis;
extern sh_int  gsn_strium;
extern sh_int  gsn_ptrium;
extern sh_int  gsn_uorder;
extern sh_int  gsn_istrength;
extern sh_int  gsn_orealts_1st;
extern sh_int  gsn_orealts_2nd;
extern sh_int  gsn_defensive;
extern sh_int  gsn_chii_bolt;
extern sh_int  gsn_thrash;
extern sh_int  gsn_deadfall;
extern sh_int  gsn_windmill;
extern sh_int  gsn_cusinart;
extern sh_int  gsn_wire_delay;
extern sh_int  gsn_siege_delay;
extern sh_int  gsn_ghoul_touch;
extern sh_int  gsn_paralyze;
extern sh_int  gsn_feedback;
extern sh_int  gsn_entomb;
extern sh_int  gsn_insect_swarm;
extern sh_int  gsn_con_damage;
extern sh_int  gsn_cone;
extern sh_int  gsn_dark_ritual;
extern sh_int  gsn_covenant;
extern sh_int  gsn_burrow;
extern sh_int  gsn_cutpurse;
extern sh_int  gsn_cutter;
extern sh_int  gsn_offhand_disarm;
extern sh_int  gsn_werepower;
extern sh_int  gsn_weretiger;
extern sh_int  gsn_werewolf;
extern sh_int  gsn_werebear;
extern sh_int  gsn_werefalcon;
extern sh_int  gsn_fasting;
extern sh_int  gsn_shed_skin;
extern sh_int  gsn_crusade;
extern sh_int  gsn_heroism;
extern sh_int  gsn_decoy;
extern sh_int  gsn_prayer;
extern sh_int  gsn_refrain;
extern sh_int  gsn_lullaby;
extern sh_int  gsn_tarot;
extern sh_int  gsn_tko;
extern sh_int  gsn_leech;
extern sh_int  gsn_noquit;
extern sh_int  gsn_nolearn;
extern sh_int  gsn_duplicate;
extern sh_int  gsn_dance_song;
extern sh_int  gsn_attract;
extern sh_int  gsn_taunt;
extern sh_int  gsn_uppercut;
extern sh_int  gsn_condor;
extern sh_int  gsn_disorient;
extern sh_int  gsn_concen;
extern sh_int  gsn_seasons;
extern sh_int  gsn_hysteria;
extern sh_int  gsn_lust;
extern sh_int  gsn_reputation;
extern sh_int  gsn_unforgiven;
extern sh_int  gsn_phoenix;
extern sh_int  gsn_brawl;
extern sh_int  gsn_brain_death;
extern sh_int  gsn_unholy_strength;
extern sh_int  gsn_ram;
extern sh_int  gsn_embrace;
extern sh_int  gsn_embrace_poison;
extern sh_int  gsn_secarts;
extern sh_int  gsn_continuum;
extern sh_int  gsn_cont;
extern sh_int  gsn_tear;
extern sh_int  gsn_unholy_gift;
extern sh_int  gsn_unminding;
extern sh_int  gsn_mana_charge;
extern sh_int  gsn_disguise;
extern sh_int  gsn_push;
extern sh_int  gsn_purity;
extern sh_int  gsn_const;
extern sh_int  gsn_epal;
extern sh_int  gsn_study;
extern sh_int  gsn_spell_trap;
extern sh_int  gsn_mana_trap;
extern sh_int  gsn_warmaster_code;
extern sh_int  gsn_call_roc;
extern sh_int  gsn_corrupt;
extern sh_int  gsn_lifeforce;
extern sh_int  gsn_entangle;
extern sh_int  gsn_forest_mist;
extern sh_int  gsn_forest_walk;
extern sh_int  gsn_treeform;
extern sh_int  gsn_trap_silvanus;
extern sh_int  gsn_circle_of_protection;
extern sh_int  gsn_nausea;
extern sh_int  gsn_research;
extern sh_int  gsn_decipher;

/* BATTLE MAGE */
extern sh_int			gsn_forcefield;
extern sh_int			gsn_terra_shield;
extern sh_int			gsn_rust;
extern sh_int			gsn_air_shield;
extern sh_int			gsn_blades;
extern sh_int			gsn_field_dress;
extern sh_int			gsn_plumbum;
extern sh_int			gsn_dan_blade;
extern sh_int			gsn_mana_lock;
extern sh_int			gsn_ref_shield;

/* Miners */
extern sh_int			gsn_sense_vein;
extern sh_int			gsn_dig;
extern sh_int			gsn_gemlore;
extern sh_int			gsn_extrude;

//anatomies
extern sh_int			gsn_mob_expert;
extern sh_int			gsn_human_expert;
extern sh_int			gsn_elf_expert;
extern sh_int			gsn_giant_expert;
extern sh_int			gsn_dwarf_expert;
extern sh_int			gsn_demihuman_expert;
extern sh_int			gsn_flying_expert;
extern sh_int			gsn_beast_expert;
extern sh_int			gsn_unique_expert;

extern sh_int			gsn_mob_master;
extern sh_int			gsn_human_master;
extern sh_int			gsn_elf_master;
extern sh_int			gsn_dwarf_master;
extern sh_int			gsn_giant_master;
extern sh_int			gsn_beast_master;
extern sh_int			gsn_demihuman_master;
extern sh_int			gsn_flying_master;
extern sh_int			gsn_unique_master;

/*begin externals for gen numbers*/
extern sh_int gen_firestorm;
extern sh_int gen_mana_charge;
extern sh_int gen_gold_loss;
extern sh_int gen_guise;
extern sh_int gen_study;
extern sh_int gen_insect_swarm;
extern sh_int gen_panic;
extern sh_int gen_malform;
extern sh_int gen_has_owner;
extern sh_int gen_dark_meta;
extern sh_int gen_soul_reaver;
extern sh_int gen_handof_chaos;
extern sh_int gen_request;
extern sh_int gen_unlife;
extern sh_int gen_seremon;
extern sh_int gen_holy_hands;
extern sh_int gen_voodoo_doll;
extern sh_int gen_pc_charm;
extern sh_int gen_blades;
extern sh_int gen_batter;
extern sh_int gen_dan_blade;
extern sh_int gen_anger;
extern sh_int gen_terra_shield;
extern sh_int gen_telepathy;
extern sh_int gen_ref_shield;
extern sh_int gen_plumbum;
extern sh_int gen_watchtower;
extern sh_int gen_discord;
extern sh_int gen_orb;
extern sh_int gen_conceal;
extern sh_int gen_d_shroud;
extern sh_int gen_cspawn;
extern sh_int gen_visit;
extern sh_int gen_sav_ward;
extern sh_int gen_sav_cata;
extern sh_int gen_challenge;
extern sh_int gen_circle_of_protection;
extern sh_int gen_yearning;
extern sh_int gen_ensnare;
extern sh_int gen_mob_gen;
extern sh_int gen_jail;
extern sh_int gen_addiction;
extern sh_int gen_levitation;
extern sh_int gen_scribe;
extern sh_int gen_siege;
extern sh_int gen_photon_wall;
extern sh_int gen_crusade;
extern sh_int gen_avenger;
extern sh_int gen_bleed;
extern sh_int gen_apierce;
extern sh_int gen_chant;
extern sh_int gen_dvoid;
extern sh_int gen_strium;
extern sh_int gen_ward;
extern sh_int gen_acraft;
extern sh_int gen_wcraft;
extern sh_int gen_hwep;
extern sh_int gen_approach;
extern sh_int gen_suffocate;
extern sh_int gen_contract;
extern sh_int gen_spirit;
extern sh_int gen_coup;
extern sh_int gen_diplomacy;
extern sh_int gen_recruit;
extern sh_int gen_vengeance;
extern sh_int gen_conjure;
extern sh_int gen_maze;
extern sh_int gen_chron;
extern sh_int gen_paradox;
extern sh_int gen_temp_avenger;
extern sh_int gen_move_dam;
extern sh_int gen_kinesis;
extern sh_int gen_create_object;
extern sh_int gen_ainfection;
extern sh_int gen_dreamwalk;
extern sh_int gen_svise;
extern sh_int gen_upgrade;
extern sh_int gen_metabolic_boost;
extern sh_int gen_phantasm;
extern sh_int gen_dragon_blood;
extern sh_int gen_regenerate;
extern sh_int gen_temp_storm;

/* game tables */
extern	const		char*		predict_table[];
extern	const	struct weapon_rate_type	weapon_rating[WEAPON_MAX];
extern	const		int		progress_table[];
extern	const		char*		day_name[MAX_DAYS];
extern	const		char*		month_name[MAX_MONTHS];
extern  const   struct  deity_type      deity_table[MAX_DIETY];
extern  const   struct  flag_type       path_table[];
extern  const   struct  mal_deity_type  mal_deity_table[];
extern  const   struct  mal_name_type	mal_name_table[];
extern  const   struct  monk_type       monk_table[8];
extern  const   struct  hometown_type   hometown_table[MAX_HOMETOWN];
extern	const	struct	position_type	position_table[];
extern	const	struct	mcharge_type	mcharge_table[];
extern	const	struct	maxmcharge_type	maxmcharge_table[];
extern	const	struct	room_color_type	room_color_table[MAX_SECT];
extern	const	struct	study_msg_type	study_msg_table[];
extern	const	struct	sex_type	sex_table[];
extern	const	struct	ethos_type	ethos_table[];
extern	const	struct	anatomy_type	anatomy_table[];
extern	const	struct	size_type	size_table[];
extern	const	struct	warlord_type	warlord_table[];
extern  const	struct	song_type	song_table[MAX_SONG];
extern	const	struct	tarot_type	tarot_table[];
extern	const	struct	lagattack_type	lagattack_table[];
extern	const	struct	warn_type	warn_table[];
extern	const	struct	psalm_type	psalm_table[];
extern	const	struct	amodify_type	amodify_table[];
extern	const	struct	wmodify_type	wmodify_table[];
extern		struct	objseed_type	objseed_table[];
extern	const	struct	jail_type	jail_table[MAX_HOMETOWN];

extern	const	struct	flag_type	mprog_flags[];
extern	const	struct	flag_type	oprog_flags[];
extern	const	struct	flag_type	rprog_flags[];
extern	const	struct	flag_type	message_flags[];

/* flag tables */
extern	const	struct	flag_type	act_flags[];
extern	const	struct	flag_type	act2_flags[];
extern	const	struct	flag_type	plr_flags[];
extern	const	struct	flag_type	game_flags[];
extern	const	struct	flag_type	color_flags[];
extern	const	struct	flag_type	affect_flags[];
extern	const	struct	flag_type	affect2_flags[];
extern	const	struct	flag_type	off_flags[];
extern	const	struct	flag_type	imm_flags[];
extern	const	struct	flag_type	form_flags[];
extern	const	struct	flag_type	special_flags[];
extern	const	struct	flag_type	part_flags[];
extern	const	struct	flag_type	comm_flags[];
extern	const	struct	flag_type	extra_flags[];
extern	const	struct	flag_type	wear_flags[];
extern	const	struct	flag_type	army_flags[];
extern	const	struct	flag_type	army_types[];
extern	const	struct	flag_type	weapon_flags[];
extern	const	struct	flag_type	container_flags[];
extern	const	struct	flag_type	room_flags[];
extern	const	struct	flag_type	room_flags2[];
extern	const	struct	flag_type	exit_flags[];
extern	const	struct	flag_type	crime_table[];
extern	const	struct	flag_type	punish_table[];
extern	const	struct	flag_type	cabal_flags[];
extern	const	struct	flag_type	cabal_flags2[];
extern	const	struct	flag_type	cabal_progress_flags[];
extern	const	struct	flag_type	pact_flags[];
extern	const	struct	flag_type	croom_flags[];
extern	const	struct	flag_type	cont_table[];


/* Utility macros. */
#define IS_VALID(data)		((data) != NULL && (data)->valid)
#define VALIDATE(data)		((data)->valid = TRUE)
#define INVALIDATE(data)	((data)->valid = FALSE)
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define GN_POS(gn)              ((gn - MAX_SKILL < 0) ? gn : gn - MAX_SKILL)
#define IS_GEN(gn)              (( (gn >= MAX_SKILL) && (gn < (MAX_SKILL + MAX_EFFECTS))) ? TRUE : FALSE)
#define	HAS_SS(class)		((class_table[(class)].ss != NULL && class_table[(class)].ss[0] != '\0'))
#define IS_NULLSTR(str)		((str)==NULL || (str)[0]=='\0')
#define IS_VIRROOM(room)	((room)->vnum >= MAX_ROOM_VNUM)
#define IS_VIRVNUM(vnum)	((vnum) >= MAX_ROOM_VNUM)
#define IS_ORIGIN(pos)		((pos)[0] == 0 && (pos)[1] == 0 && (pos)[2] == 0)
#define IS_AREA(area, flag)	(IS_SET((area)->area_flags, (flag)))


/* Character macros. */
#define IS_ACT(ch, bit)		(IS_SET((ch)->act_flags, (bit)))
#define HAS_CLAN(ch)		(!IS_NPC((ch)) && (ch)->pcdata->clan_handle > 0)
#define GET_CLAN(ch)		(!IS_NPC((ch)) ? (ch)->pcdata->clan_handle : 0)
#define CLAN_ELDER(ch)		(!IS_NPC((ch)) && (ch)->pcdata->clan_rank >= RANK_ELDER )
#define CLAN_LEADER(ch)		(!IS_NPC((ch)) && (ch)->pcdata->clan_rank  > RANK_ELDER )
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_QRACE(ch)		(!IS_NPC(ch) && !IS_NULLSTR(ch->pcdata->race))
#define IS_ROYAL(ch)		(IS_SET((ch)->act2, PLR_ROYAL))
#define IS_NOBLE(ch)		(IS_SET((ch)->act2, PLR_NOBLE))
#define IS_HIGHBORN(ch)		(IS_ROYAL((ch)) || IS_NOBLE((ch)))
#define IS_UNDEAD(ch)		(IS_SET((ch)->act, ACT_UNDEAD))
#define IS_AVATAR(ch)		(IS_SET((ch)->act2, PLR_AVATAR))
#define IS_DEMON(ch)		((ch)->race == grn_demon)
#define IS_TELEPATH(ch)		(!IS_NPC((ch)) && IS_SET((ch)->comm, COMM_TELEPATH))
#define IS_MASQUE(ch)		(!IS_NPC((ch)) && IS_SET((ch)->act2, PLR_MASQUERADE))
#define IS_WANTED(ch)		(!IS_NPC((ch)) && IS_SET((ch)->act,  PLR_WANTED))
#define IS_OUTLAW(ch)           (!IS_NPC((ch)) && (ch)->pcdata->flagged > 14 )
#define IS_LEADER(ch)		(!IS_NPC(ch) && (ch)->pcdata->rank == RANK_LEADER)
#define IS_ELDER(ch)		(!IS_NPC(ch) && (ch)->pcdata->rank >= RANK_ELDER)
#define IS_DNDS(ch)		(!IS_NPC(ch) && IS_GAME((ch), GAME_DNDS))
#define IS_IMMORTAL(ch)		(!IS_NPC(ch) && get_trust(ch) >= AVATAR)
#define IS_HERO(ch)		(!IS_NPC(ch) && get_trust(ch) >= LEVEL_HERO)
#define IS_MASTER(ch)		(!IS_NPC((ch)) && get_trust((ch)) == MASTER)
#define IS_TRUSTED(ch,level)	(!IS_NPC(ch) && get_trust((ch)) >= (level))
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))
#define IS_AFFECTED2(ch, sn)    (IS_SET((ch)->affected2_by, (sn)))
#define IS_GAME(ch, flag)	(IS_SET((ch)->game, (flag)))
#define IS_PERK(ch, perk)	(IS_NPC(ch) ? FALSE : IS_SET((ch)->pcdata->perk_bits, (perk)))
#define IS_AREA(area, flag)	(IS_SET((area)->area_flags, (flag)))
#define IS_ROOM(room, sn)	(IS_SET((room)->room_flags, (sn)))
#define IS_ROOM2(room, sn)	(IS_SET((room)->room_flags2, (sn)))
#define IS_CABAL(pCab, flag)	(IS_SET((pCab)->flags, (flag)))
#define IS_CABAL2(pCab, flag)	(IS_SET((pCab)->flags2, (flag)))
#define IS_COLOR(ch, sn)	(IS_SET((ch)->color, (sn)))
#define IS_GOOD(ch)		((ch)->alignment >= GOOD_THRESH)
#define IS_EVIL(ch)		((ch)->alignment <= EVIL_THRESH)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))
#define IS_AWAKE(ch)		((ch)->position > POS_SLEEPING && !IS_STONED(ch))
#define IS_STONED(ch)		(IS_AFFECTED2((ch), AFF_PETRIFY))
#define GET_HITROLL(ch)         (get_hitroll((ch)))
#define GET_DAMROLL(ch)         (get_damroll((ch)))
#define IS_OUTSIDE(ch)          (!IS_SET((ch)->in_room->room_flags,ROOM_INDOORS))
#define WAIT_STATE(ch, npulse)	(get_trust(ch) < AVATAR ? ((ch)->wait = UMAX((ch)->wait, (npulse))) : ((ch)->wait = 0))
#define get_carry_weight(ch)    ((ch)->carry_weight)
#define IS_LIMITED(obj)		(CAN_WEAR(obj,ITEM_UNIQUE) || CAN_WEAR(obj,ITEM_RARE))
#define act(format,ch,arg1,arg2,type)	act_new((format),(ch),(arg1),(arg2),(type),POS_RESTING)
#define IS_WEREBEAST(race)	(race > 18 && race < 26)

/* Object macros. */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))
#define IS_TRAP(trap, stat)	(IS_SET((trap)->flags, (stat)))
#define IS_SOC_STAT(obj, stat)	((obj)->item_type == ITEM_SOCKET && IS_SET((obj)->value[0], (stat)))
#define IS_PROJ_TYPE(obj, stat) ( ((obj)->item_type == ITEM_RANGED || (obj)->item_type == ITEM_PROJECTILE) && IS_SET((obj)->value[0],(stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)        ((obj)->item_type == ITEM_CONTAINER ? (obj)->value[4] : 100)
#define IS_PROJ_SPEC(obj,stat) ((obj)->item_type == ITEM_PROJECTILE && IS_SET((obj)->value[4],(stat)))

/* Description macros. */
#define PERS(ch, looker)	(pers (ch,looker))
#define PERS2(ch)	        (pers2 (ch))

/* M/O/R PROGS MACROS */
#define HAS_TRIGGER_MOB(ch,trig) (IS_SET((ch)->pIndexData->mprog_flags,(trig)))
#define HAS_TRIGGER_OBJ(obj,trig) (IS_SET((obj)->pIndexData->oprog_flags,(trig)))
#define HAS_TRIGGER_ROOM(room,trig) (IS_SET((room)->rprog_flags,(trig)))

/* Structure for a social in the socials table. */
struct  social_type
{
    char      name[20];
    char *    char_no_arg;
    char *    others_no_arg;
    char *    char_found;
    char *    others_found;
    char *    vict_found;
    char *    char_not_found;
    char *    char_auto;
    char *    others_auto;
};

/* Global constants. */
extern	const	struct	str_app_type	str_app		[26];
extern	const	struct	int_app_type	int_app		[26];
extern	const	struct	wis_app_type	wis_app		[26];
extern	const	struct	dex_app_type	dex_app		[26];
extern	const	struct	con_app_type	con_app		[26];
extern	const	struct	class_type	class_table	[MAX_CLASS];
extern	const	struct	ss_type		ss_table	[MAX_CLASS];
extern	const	struct	ss_group_type	ss_group_table	[MAX_GROUPS];
extern	const	struct	weapon_type	weapon_table	[];
extern	const	struct	sheath_type	sheath_table	[];
extern  const   struct  item_type	item_table	[];
extern	const	struct	wiznet_type	wiznet_table	[];
extern	const	struct	attack_type	attack_table	[];
extern  const	struct  race_type	race_table	[];
extern	const	struct	pc_race_type	pc_race_table	[];
extern  const	struct	spec_type	spec_table	[];
extern	const	struct	liq_type	liq_table	[];
extern	const	struct	skill_type	skill_table	[MAX_SKILL];
extern	const	struct	critical_type	critical_table	[];
extern	const	EFFECT_DATA      	effect_table	[MAX_EFFECTS];
extern  const   struct  group_type      group_table	[MAX_GROUP];
extern	const	struct	avatar_type	avatar_info	[MAX_AVATAR];  //Avatar subrace info.
extern          struct  social_type     social_table    [MAX_SOCIALS];
extern  char *  const                   title_table     [MAX_CLASS] [MAX_LEVEL+1] [2];
extern  char *  const                   clanr_table	[MAX_CLANR] [2];
extern  char            last_command                    [MSL];
extern  char            last_mprog                      [MSL];
extern	char		last_malloc			[MSL];
extern	char		last_loop			[MSL];

/* Global variables. */
extern		struct	mud_type	mud_data;	//holds all the saved mud data
extern		HELP_DATA	  *	help_first;
extern		HELP_DATA	  *	help_last;
extern		SHOP_DATA	  *	shop_first;
extern		TRAP_DATA	  *	trap_list;
extern		CHAR_DATA	  *	char_list;
extern		CHAR_DATA	  *	player_list;
extern		DESCRIPTOR_DATA   *	descriptor_list;
extern		OBJ_DATA	  *	object_list;
extern          PROG_CODE         *     mprog_list;
extern          PROG_CODE         *     rprog_list;
extern          PROG_CODE         *     oprog_list;
extern		TRAP_INDEX_DATA	  *	trap_index_list;
extern		CABAL_INDEX_DATA  *	cabal_index_list;
extern		CABAL_DATA	  *	cabal_list;

extern          NOTE_DATA *note_list;
extern          NOTE_DATA *idea_list;
extern          NOTE_DATA *application_list;
extern          NOTE_DATA *penalty_list;
extern          NOTE_DATA *news_list;
extern          NOTE_DATA *changes_list;

extern		char			bug_buf		[];
extern		bool			fLogAll;//interp.c
extern		bool			fLogLoop;//interp.c
extern		FILE *			fpReserve;
extern		KILL_DATA		kill_table	[];
extern		char			log_buf		[];

extern          int                     reboot_tick_counter;
extern          char			reboot_buffer [MIL];
extern          int			reboot_act;
extern          int                     restart;
		int			control;



/* OS-dependent declarations.                                  *
 * These are all very standard library functions,              *
 * but some systems have incomplete or non-ansi header files.  */
#if	defined(_AIX)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(apollo)
int	atoi		args( ( const char *string ) );
void *	calloc		args( ( unsigned nelem, size_t size ) );
char *	crypt		args( ( const char *key, const char *salt) );
#endif

#if	defined(hpux)
char *	crypt		args( ( const char *key, const char *salt));
#endif

#if	defined(linux)
int	fclose		args( ( FILE *stream ) );
char *	crypt		args( ( const char *key, const char *salt));
#endif

#if	defined(MIPS_OS)
char *	crypt		args( ( const char *key, const char *salt) );
#endif

#if	defined(NeXT)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(sequent)
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined(sun)
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
size_t  fread           args( ( void *ptr, size_t size, size_t n, FILE *stream) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined(ultrix)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

/* The crypt(3) function is not available on some operating systems. *
 * In particular, the U.S. Government prohibits its export from the  *
 * United States to foreign countries.                               *
 * Turn on NOCRYPT to keep passwords in plain text.                  */
#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif

/* Data files used by the server.                                         *
 * AREA_LIST contains a list of areas to boot.                            *
 * All files are read in completely at bootup.                            *
 * Most output files (bug, typo, shutdown) are append-only.               *
 * The NULL_FILE is held open so that we have a stream handle in reserve, *
 * so players can go ahead and telnet to all the other descriptors.       *
 * Then we close it whenever we need to open a file (e.g. a save file).   */
#if defined(unix)
#define PLAYER_DIR        "../player/"            /* Player files */
#define PURGE_DIR         "../purge/"             /* Purged files for password checking */
#define GOD_DIR           "../gods/"              /* list of gods */
#define TEMP_FILE         "../player/romtmp"      /* Temporary file */
#define NULL_FILE         "/dev/null"             /* To reserve one stream */
#define MOB_DIR           "../mobprogs/"	  /* MOBProg files */
#define CHALLENGE_DIR	  "../challenge/"   /* Challenge files */
#define HISTORY_DIR       "../history/"     /* History files */
#define HTML_DIR	  "../public_html/"
#define VOTE_DIR	  "../voter/votes/"
#define AREA_DIR      "../area/"
#endif

#define AREA_LIST         AREA_DIR "area.lst"     /* List of areas */
#define HELP_FILE         AREA_DIR "help.txt"              /* For 'helps' */
#define BUG_FILE          "bugs.txt"              /* For 'bug' and bug() */
#define TYPO_FILE         "typos.txt"             /* For 'typo' */
#define NOTE_FILE         "notes.not"             /* For 'notes' */
#define IDEA_FILE         "idea.not"              /* For 'ideas' */
#define APPLICATION_FILE  "apps.not"              /* For 'applications' */
#define PENALTY_FILE      "penal.not"             /* For 'penalties' */
#define NEWS_FILE         "news.not"              /* For 'news' */
#define CHANGES_FILE      "chang.not"             /* For 'changes' */
#define SHUTDOWN_FILE     "shutdown.txt"          /* For 'shutdown' */
#define BAN_FILE          "ban.txt"               /* For 'bans' */
#define LIMIT_FILE        "limit.txt"             /* For 'limits' */
#define LIMITTEMP_FILE    "limit.tmp"             /* For 'limits' */
#define CRASH_FILE	  "../log/crash.txt"	  /* Crash log */
#define TRACKIP_FILE	  "../log/trackip.txt"	  /* Crash log */
#define EVENT_FILE	  "../log/events.txt"     /* Log with player evens*/
#define HOTREBOOT_FILE	  "reboot.txt"		  /* For hot reboot */
#define OBJSAVE_FILE	  "objsave.txt"		  /* For saving objects */
#define ILLEGAL_NAME_FILE "../area/Illegal.txt"   /* For not using same name */
#define APPROVED_NAME_FILE "../area/approved.txt"   /* For allowed names on newbie ban */
#define BOUNTY_FILE_CP    "top-10.cp"		  /* top-10 list copy */
#define WHO_HTML_FILE	  "who.html"		  /* For HTML who list */
#define AREA_HTML_FILE	  "area_html_data.txt"	  /* For HTML area map */
#define CHALLENGE_FILE	  "challenge.sav"	  /* For challenges */
#define TOME_TEMP_FILE	  "/area/tomes.txt"	  /* For tomes */
#define TOME_FILE	  "/area/tomes.txt"	  /* For tomes */


/* Our function prototypes.  */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define TID	TRAP_INDEX_DATA
#define CID	CABAL_INDEX_DATA
#define HID	HELP_DATA
#define TD	TRAP_DATA
#define RID	ROOM_INDEX_DATA
#define AID	AREA_DATA
#define SF	SPEC_FUN
#define AD	AFFECT_DATA

/* act_comm.c */
void    makedrunk	args( (char *string, CHAR_DATA * ch) );
bool	can_follow	args( (CHAR_DATA* ch, CHAR_DATA* victim) );
void  	check_sex	args( ( CHAR_DATA *ch) );
void	add_follower	args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	stop_follower	args( ( CHAR_DATA *ch ) );
void 	nuke_pets	args( ( CHAR_DATA *ch ) );
void	die_follower	args( ( CHAR_DATA *ch, bool remove ) );
void	die_follower_pc	args( ( CHAR_DATA *ch) );
void	j_yell	 	args( ( CHAR_DATA *ch, char *argument) );
void	do_jcabaltalk	args( ( CHAR_DATA *ch, char *argument) );
bool	is_same_group	args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void	init_malloc	args( ( char *argument) );
void	end_malloc	args( ( char *argument) );
void	init_loop	args( ( char *argument) );
void	end_loop	args( ( char *argument) );
bool	check_approved_name args( (char *name) );
bool	lookup_history  args( ( CHAR_DATA *ch, char *argument ) );
void	list_history    args( ( CHAR_DATA *ch, char *argument ) );
void	set_cleric_skillset args( (CHAR_DATA* ch, int value) );
void	set_race_skills args( (CHAR_DATA* ch, int value) );
void	update_skills	args( (CHAR_DATA* ch) );
bool	purge_limited	args( (CHAR_DATA* ch) );
void	set_start_wep args( (CHAR_DATA* ch) );
char*	get_crus_title	(CHAR_DATA* ch, int level);

/* act_info.c */
void	set_title	args( ( CHAR_DATA *ch, char *title ) );
void	do_promptexit	args( (CHAR_DATA* ch, char* str) );
char *  pers            args( ( CHAR_DATA *ch, CHAR_DATA *looker) );
char *  pers2           args( ( CHAR_DATA *ch ) );
void	where		( CHAR_DATA* ch, AREA_DATA* area, char* argument );

/* act_move.c */
void	move_char	args( ( CHAR_DATA *ch, int door, bool follow ) );

/* act_obj.c */
bool	check_sheath	args( (CHAR_DATA* ch, int iWear) );
void    get_obj         args( ( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container ) );
void	defecate	args( ( CHAR_DATA *ch ) );
void    vomit		args( ( CHAR_DATA *ch ) );
void	adorn_obj	args( (OBJ_DATA* socket, OBJ_DATA* obj, CHAR_DATA *ch) );
void	unadorn_obj	args( (OBJ_DATA* socket, OBJ_DATA* obj, CHAR_DATA *ch) );

/* act_wiz.c */
int	    name_ok	args( (char *name) );
void wiznet             args( (char *string, CHAR_DATA *ch, OBJ_DATA *obj, long flag, long flag_skip, int min_level ) );
inline bool set_owner	args( (CHAR_DATA* ch, OBJ_DATA* obj, char* deity) );
inline bool clear_owner	args( (OBJ_DATA* obj) );

/* cabal */
bool    spec_special_guard	args( ( CHAR_DATA *ch ) );

/* comm.c */
void    bugf (char * fmt, ...) __attribute__ ((format(printf,1,2)));
void    nlogf (char * fmt, ...) __attribute__ ((format(printf,1,2)));
void    sendf args( ( CHAR_DATA *ch, char *fmt, ...) ) __attribute__ ((format(printf, 2,3)));
void	show_string	args( ( struct descriptor_data *d, char *input) );
void	close_socket	args( ( DESCRIPTOR_DATA *dclose ) );
void    write_to_buffer args( ( DESCRIPTOR_DATA *d, const char *txt, int length ) );
void	send_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void	page_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void*    act            args( ( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type ) );
void    act_new         args( ( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type, int min_pos) );
char *  first_arg       args( ( char *argument, char *arg_first, bool fCase ) );

/* db.c */
void	seed_object	    ( int i );
char *  print_flags     args( ( int flag ) );
void	boot_db		args( ( bool fHotReboot ) );
void	area_update	args( ( void ) );
CD *	create_mobile	args( ( MOB_INDEX_DATA *pMobIndex ) );
void    clone_mobile    args( ( CHAR_DATA *parent, CHAR_DATA *clone ) );
void	clone_room	args( ( ROOM_INDEX_DATA* src, ROOM_INDEX_DATA* dest, bool fFull ) );
OD *    create_object   args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void	clone_object	args( ( OBJ_DATA *parent, OBJ_DATA *clone ) );
void	clear_char	args( ( CHAR_DATA *ch ) );
char *	get_extra_descr	args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
char *get_auto_extra_descr( const char *name, EXTRA_DESCR_DATA *ed,bool fAuto);
MID *	get_mob_index	args( ( int vnum ) );
OID *	get_obj_index	args( ( int vnum ) );
TID *	get_trap_index	args( ( int vnum ) );
CID *	get_cabal_index	args( ( int vnum ) );
CABAL_INDEX_DATA *get_cabal_index_str( char* name );
HID*	get_help_index	args( ( int vnum ) );
RID *	get_room_index	args( ( int vnum ) );
char	fread_letter	args( ( FILE *fp ) );
int	fread_number	args( ( FILE *fp ) );
unsigned long fread_unsigned_long args( ( FILE *fp ) );
long	fread_long	args( ( FILE *fp ) );
long 	fread_flag	args( ( FILE *fp ) );
char *	fread_string	args( ( FILE *fp ) );
char *  fread_string_eol args(( FILE *fp ) );
void	fread_to_eol	args( ( FILE *fp ) );
char *	fread_word	args( ( FILE *fp ) );
char *	fread_word_noquote args( ( FILE *fp ) );
long	flag_convert	args( ( char letter) );
void *	alloc_mem	args( ( int sMem ) );
void *	alloc_perm	args( ( int sMem ) );
void	free_mem	args( ( void *pMem, int sMem ) );
char *	str_dup		args( ( const char *str ) );
void	free_string	args( ( char *pstr ) );
int	temp_adjust	args( ( int number ) );
int	number_fuzzy	args( ( int number ) );
int	number_range	args( ( int from, int to ) );
int	number_percent	args( ( void ) );
int	number_door	args( ( void ) );
int	random_door	( CHAR_DATA* ch, ROOM_INDEX_DATA* room );
int	number_bits	args( ( int width ) );
long    number_mm       args( ( void ) );
int	dice		args( ( int number, int size ) );
int	interpolate	args( ( int level, int value_00, int value_32 ) );
void	smash_tilde	args( ( char *str ) );
void	smash_tilde2	args( ( char *str ) );
void	str_replace	args( ( char *buf, const char *s, const char *repl) );
bool	str_cmp		args( ( const char *astr, const char *bstr ) );
bool	str_cmp_2	args( ( const char *astr, const char *bstr ) );
bool	str_prefix	args( ( const char *astr, const char *bstr ) );
bool	str_infix	args( ( const char *astr, const char *bstr ) );
bool	str_suffix	args( ( const char *astr, const char *bstr ) );
char *	capitalize	args( ( const char *str ) );
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	append_string	args( ( char *file, char *str ) );
void	bug		args( ( const char *str, int param ) );
void	log_string	args( ( const char *str ) );
void	log_event	args( ( CHAR_DATA* ch, char *str ) );

void	tail_chain	args( ( void ) );

/* fight.c */
bool	do_combatdraw	args( (CHAR_DATA* ch, OBJ_DATA* prim, OBJ_DATA* sec) );
bool 	is_safe		args( (CHAR_DATA *ch, CHAR_DATA *victim ) );
bool 	is_safe_quiet	args( (CHAR_DATA *ch, CHAR_DATA *victim ) );
bool 	is_safe_quiet_all args( (CHAR_DATA *ch, CHAR_DATA *victim ) );
void 	raw_kill	args( (CHAR_DATA *ch, CHAR_DATA *victim ) );
void 	_raw_kill	args( (CHAR_DATA *ch, CHAR_DATA *victim, bool fBountyCheck ) );
void	cabal_requip	args( (CHAR_DATA* ch) );
void	violence_update	args( ( void ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	multi_hit_extra	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int change ) );
int     damage          args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int class, bool show ) );
int	virtual_damage	args( (CHAR_DATA *ch,CHAR_DATA *victim, OBJ_DATA* obj, int dam, int dt, int dam_type, int h_roll, int lvl,  bool show, bool fVirt, int sn) );

void	update_pos	args( ( CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
void	dam_message	args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* obj, int dam, int dt, bool immune, bool fVirt, int sn, bool fShow ) );
bool 	is_area_safe	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool 	is_area_safe_quiet	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
CHAR_DATA *  check_guard   args( (CHAR_DATA *ch, CHAR_DATA *mob) );
void	dam_dead	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	update_violence	args( ( CHAR_DATA *ch ) );
int	check_crusade	args( (CHAR_DATA* ch, CHAR_DATA* victim) );
int	check_avenger	args( (CHAR_DATA* ch, CHAR_DATA* victim) );

/* handler.c */
int			count_obj_inlist( OBJ_DATA* list, int count );
int			get_max_anat( CHAR_DATA* ch, int anat );
bool			counter_check( CHAR_DATA* ch, CHAR_DATA* victim, int skill, int dam, int dam_type, int sn);
int			getBmWearLoc( CHAR_DATA* ch, bool fPrim);
OBJ_DATA*		getBmWep( CHAR_DATA* ch, bool fPrim );
int			get_maxcount( OBJ_INDEX_DATA* pIndex);
bool			can_spawn( OBJ_INDEX_DATA* pIndex );
bool			can_exist( OBJ_INDEX_DATA* pIndex );
void			aecho( AREA_DATA* area, char* echo );
int			get_path_vnum( CHAR_DATA* mob);
ROOM_INDEX_DATA*	get_path_dest( PATH_QUEUE* path );
int	set_path( CHAR_DATA* mob, ROOM_INDEX_DATA* src, ROOM_INDEX_DATA* dest, int max_dist, CABAL_DATA* pCab);
int	get_weapon_rating( int weapon_type, int rating_category );
int	get_weapon_obj_rating( OBJ_DATA* obj, int category );
int	get_skill_reps	( int skill_level );
void	rltom_date	( long real_time, int* h, int* d, int* m, int* y );
char*	mud_date_string	( int hour, int Day, int month, int year );
int	get_hitroll	( CHAR_DATA* ch );
int	get_damroll	( CHAR_DATA* ch );
int	get_season	( int month, int day );
char*	mud_date	();
int	get_dice_avg	( int number, int type, int bonus );
bool	IS_GNBIT	( int sn, int bit );
AFFECT_DATA* get_random_affect( AFFECT_DATA* affected, int gn_bit );
char*  show_crusade	(AFFECT_DATA* paf);
inline CHAR_DATA* in_char(OBJ_DATA* obj);
inline OBJ_DATA* in_obj	(OBJ_DATA* obj);
char*	get_proj_number	( int number );
char*	get_condition	(int condition, bool fParen);
int	GET_AC		(CHAR_DATA* ch, int type);
int	GET_AC2		(CHAR_DATA* ch, int type);
QUEST_DATA* has_quest	(CHAR_DATA* ch, char* quest);
QUEST_DATA* add_quest	(CHAR_DATA* ch, char* quest, int fSave);
void	add_psalm	( CHAR_DATA* ch, int sn );
void	remove_psalm	( CHAR_DATA* ch, int sn );
bool	del_quest	(CHAR_DATA* ch, char* quest);
bool	can_loot	args( (CHAR_DATA* ch, OBJ_DATA* container) );
int	dir_lookup	args( (char* arg) );
int     get_temple      args( (CHAR_DATA *ch) );
int     get_temple_pitroom  args( (CHAR_DATA *ch) );
int     get_temple_pit  args( (int pitroom) );
OBJ_DATA* get_cabal_pit ( CABAL_DATA* pc );
int     get_charmed_by  args( (CHAR_DATA *ch) );
int	get_summoned	args( (CHAR_DATA *ch, int vnum) );
int	get_leadered	args( (CHAR_DATA *ch, int vnum) );
int	get_control	args( (CHAR_DATA *ch, int vnum) );
AD      *affect_find    args( (AFFECT_DATA *paf, int sn) );
AD	*affect_loc_find args( (AFFECT_DATA *paf, int location) );
int	affect_loc_add	args( (AFFECT_DATA *paf, int location) );
OBJ_SPELL_DATA     *spell_obj_find args( (OBJ_SPELL_DATA *spell, int sn) );
void	affect_check	args( (CHAR_DATA *ch, int where, int vector) );
int	count_users	args( (OBJ_DATA *obj) );
void	affect_enchant	args( (OBJ_DATA *obj) );
int 	check_immune	args( (CHAR_DATA *ch, int dam_type, bool fSaves) );
int     cabal_lookup    args( (const char *name) );
int	cabal_vnumlookup args( (int cabal_vnum) );
int     cabal_bastionlookup  args ( (int cabal_vnum) );
int	position_lookup	args( (const char *name) );
int 	sex_lookup	args( (const char *name) );
int 	ethos_lookup	args( (const char *name) );
int 	anatomy_lookup	args( (const char *name) );
int 	size_lookup	args( (const char *name) );
int	liq_lookup	args( ( const char *name) );
int 	material_lookup args( ( const char *name) );
int     flag_lookup     args( ( const char *name, const struct flag_type *flag_table) );
int     flag_lookup2    args( ( const char *name, const struct flag_type *flag_table) );
int     objseed_lookup  args( ( int vnum ) );
int	weapon_lookup	args( ( const char *name) );
int	sheath_lookup	args( ( const char *class) );
int	trap_lookup	args( ( const char *name) );
int	damtype_lookup	args( ( const char *name) );
int	weapon_sn_lookup ( int sn );
int	weapon_type	args( ( const char *name) );
char 	*weapon_name	args( ( int weapon_Type) );
int	item_lookup	args( ( const char *name) );
char	*item_name	args( ( int item_type) );
int	attack_lookup	args( ( const char *name) );
int	lagattack_lookup args( (int sn) );
int	race_lookup	args( ( const char *name) );
long	wiznet_lookup	args( ( const char *name) );
int	class_lookup	args( ( const char *name) );
int     hometown_lookup args( ( const char *name) );
int     jail_lookup	args( ( const char *name) );
bool    has_cabal       args( ( CHAR_DATA *ch, int vnum) );
int	cp_calculate_timegain args( (CHAR_DATA* ch) );
OBJ_DATA* get_citem_char args( (CHAR_DATA* ch, int vnum) );
bool    check_cabal     args( ( CHAR_DATA *ch, int vnum) );
bool    check_cabal_quiet args( ( CHAR_DATA *ch, int vnum) );
bool    check_cabal_gone args( ( CHAR_DATA *ch, int vnum) );
bool	is_cabal	args( (CHAR_DATA* ch, char* const cabal ) );

bool    is_pk           args( ( CHAR_DATA *ch, CHAR_DATA *victim) );//takes ghost status into account
bool    is_pk_abs       args( ( CHAR_DATA *ch, CHAR_DATA *victim) );//ignores ghost status
bool    is_fight_delay  args( ( CHAR_DATA *ch, int time) );
bool    is_ghost        args( ( CHAR_DATA *ch, int time) );
bool    is_old_mob      args( ( CHAR_DATA *ch) );
int     affect_by_size  args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
int	get_skill	args( ( CHAR_DATA *ch, int sn ) );
int	get_psalm	      (CHAR_DATA* ch, const char* name);
int	max_psalm	      (CHAR_DATA* ch );
bool	has_psalm	      (CHAR_DATA* ch, int sn);
int	sklevel		args( ( CHAR_DATA *ch, int sn ) );
int	weapon_sn	args( (OBJ_DATA* wield) );
int	get_weapon_sn	args( ( CHAR_DATA *ch, bool secondary ) );
int	get_weapon_skill args( (CHAR_DATA *ch, int sn, bool fTwo) );
int	get_weapon_skill_obj args( (CHAR_DATA *ch, OBJ_DATA* obj) );
int     get_age         args( ( CHAR_DATA *ch ) );
int     get_abs_age     args( ( CHAR_DATA *ch ) );
void    reset_char      args( ( CHAR_DATA *ch ) );
int	get_trust	args( ( CHAR_DATA *ch ) );
int	get_abs_stat	      ( CHAR_DATA *ch, int stat );
int	get_max_stat	      ( CHAR_DATA *ch, int stat );
int	get_curr_stat	args( ( CHAR_DATA *ch, int stat ) );
int	get_curr_cond	args( ( OBJ_DATA *obj ) );
int 	get_max_train	args( ( CHAR_DATA *ch, int stat ) );
int     roll_stat       args( ( CHAR_DATA *ch ) );
int	_roll_stat	args( ( CHAR_DATA *ch, int stat ) );
int	can_carry_n	args( ( CHAR_DATA *ch ) );
int	can_carry_w	args( ( CHAR_DATA *ch ) );
bool	is_name		args( ( char *str, char *namelist ) );
bool	is_auto_name	args( ( char *str, char *namelist ) );
bool	is_exact_name	args( ( char *str, char *namelist ) );
AFFECT_DATA*	affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	song_affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
AFFECT_DATA*	affect_to_obj	args( ( OBJ_DATA *obj, AFFECT_DATA *paf ) );
OBJ_SPELL_DATA* spell_to_obj args( (OBJ_DATA *obj, OBJ_SPELL_DATA *sp, bool bDup) );
void	affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	song_affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_remove_obj args((OBJ_DATA *obj, AFFECT_DATA *paf ) );
void    spell_obj_remove  args( (OBJ_DATA* obj, OBJ_SPELL_DATA* spell) );
void    spell_obj_strip   args( (OBJ_DATA* obj, int spell) );
void	affect_strip	args( ( CHAR_DATA *ch, int sn ) );
void	song_affect_strip	args( ( CHAR_DATA *ch, int sn ) );
void	affect_strip_obj args( ( OBJ_DATA *obj, int sn ) );
bool	is_affected	args( ( CHAR_DATA *ch, int sn ) );
bool	is_song_affected args( ( CHAR_DATA *ch, int sn ) );
bool	is_affected_obj	args( ( OBJ_DATA *obj, int sn ) );
AD*	affect_join	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
AD*	affect_join_obj	args( ( OBJ_DATA  *obj, AFFECT_DATA *paf ) );
void	ch_from_room	args( ( CHAR_DATA *ch ) );
void	char_from_room	args( ( CHAR_DATA *ch ) );
void	ch_to_room	( CHAR_DATA* ch, ROOM_INDEX_DATA* pRoomIndex);
void	char_to_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void	do_autos	args( ( CHAR_DATA *ch ) );
void    do_obj_spell    args( ( OBJ_DATA *obj, CHAR_DATA *ch, CHAR_DATA *victim ) );
void	trap_to_char	args( ( TRAP_DATA *trap, CHAR_DATA *ch ) );
void	trap_to_obj	args( ( TRAP_DATA *trap, OBJ_DATA *obj ) );
void	trap_to_exit	args( ( TRAP_DATA *trap, EXIT_DATA *exit ) );
void	trap_from_char	args( ( TRAP_DATA *trap) );
void	trap_from_obj	args( ( TRAP_DATA *trap) );
void	trap_from_exit	args( ( TRAP_DATA *trap) );
void	extract_trap	args( ( TRAP_DATA *trap) );
TD*	create_trap	args( (TID* pTrapIndex, CHAR_DATA* owner) );

void	obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_to_ch	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_from_char	args( ( OBJ_DATA *obj ) );
int	apply_ac	args( ( OBJ_DATA *obj, int iWear, int type, int mod ));
OD *	get_eq_char	args( ( CHAR_DATA *ch, int iWear ) );
void	equip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	unequip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int	count_obj_list	args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void	obj_from_room	args( ( OBJ_DATA *obj ) );
void	obj_to_room	args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_obj	args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_from_obj	args( ( OBJ_DATA *obj ) );
bool	empty_obj	args( (OBJ_DATA* obj) );
bool	empty_char	args( (CHAR_DATA* obj) );
void    extract_obj     args( ( OBJ_DATA *obj ) );
void	extract_char	args( ( CHAR_DATA *ch, bool fPull ) );
// void	WAIT_STATE2	args( ( CHAR_DATA *ch, int npulse ) );
CD *	get_char_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument ) );
CD *	get_char_world	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char	args( ( char *argument ) );
OD *	get_obj_type	args( ( OBJ_INDEX_DATA *pObjIndexData, ROOM_INDEX_DATA* pRoom  ) );
OD *    get_obj_list    args( ( CHAR_DATA *ch, char *argument, OBJ_DATA *list ) );
OD *    get_obj_carry   args( ( CHAR_DATA *ch, char *argument, CHAR_DATA *viewer ) );
OD *	get_obj_wear	args( ( CHAR_DATA *ch, char *argument, CHAR_DATA *viewer ) );
OD *	get_obj_here	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument ) );
OD *	get_obj_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *    create_money    args( ( int gold ) );
int	get_obj_number	args( ( OBJ_DATA *obj ) );
int	get_obj_weight	args( ( OBJ_DATA *obj ) );
int	get_obj_weight_char ( CHAR_DATA* ch, OBJ_DATA *obj );
int	get_true_weight	args( ( OBJ_DATA *obj ) );
bool	room_is_dark	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	room_has_sun	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	have_access	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
bool	room_is_private	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_see_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) );
TRAP_DATA* can_see_trap	args( (CHAR_DATA *ch, TRAP_DATA* pt ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *	affect_loc_name	args( ( int location ) );
char *	affect_bit_name	args( ( int vector ) );
char *  affect2_bit_name args(( int vector ) );
char *	extra_bit_name	args( ( int extra_flags ) );
char * 	wear_bit_name	args( ( int wear_flags ) );
char *  room_bit_name   args( ( int room_flags ) );
char *  room2_bit_name   args( ( int room2_flags ) );
char *  sector_bit_name   args( ( int room_flags ) );
char *	act_bit_name	args( ( int flags, bool first, bool npc ) );
char *	off_bit_name	args( ( int flags ) );
char *	game_bit_name	args( ( int flags ) );
char*	perk_bit_name	(int flags );
char *	color_bit_name	args( ( int flags ) );
char *  imm_bit_name	args( ( int imm_flags ) );
char * 	form_bit_name	args( ( int form_flags ) );
char * 	special_bit_name args( ( int flags ) );
char *	part_bit_name	args( ( int part_flags ) );
char *	weapon_bit_name	args( ( int weapon_flags ) );
char *  comm_bit_name	args( ( int flags ) );
char *	cont_bit_name	args( ( int cont_flags) );
int	get_skillmax	args( (CHAR_DATA* ch, int sn) );
CHAR_DATA *char_file_active args ((char *argument ) );
AREA_DATA* get_rand_aexit	(AREA_DATA* pArea);
RID  *get_random_room   args ( (CHAR_DATA *ch, bool fArea, bool fGimp) );
AID  *get_rand_area   args ( (int min_av, int max_av,
				int min_rv, int max_rv,
				char** includ, int i_num,
				char** exclud, int e_num) );
RID* get_rand_room
(int min_av, int max_av,	//min and max area to select from
 int min_rv, int max_rv,	//min and max room to select from

 char** i_area, int ia_num,	//areas to select from using above
 char** e_area, int ea_num,	//areas to exclude from selection

 int* i_sect, int is_num,	//sectors required to select
 int* e_sect, int es_num,	//sectors that exclude a choice

 int* i_rom1, int ir1_num,	//room1 bits required to select
 int* e_rom1, int er1_num,	//room1 bits that exclude a choice

 int* i_rom2, int ir2_num,	//room1 bits required to select
 int* e_rom2, int er2_num,	//room1 bits that exclude a choice

 int pool,			//size of area pool to choose from
 bool fExit,			//TRUE = Exit required for selection
 bool fSafe,			//TRUE = Room must be empty of mobs/pcs
 CHAR_DATA* ch			//character if check is needed
 );

int  tarot_find    args( (AFFECT_DATA *paf) );
void	nskill_to_char	args( ( CHAR_DATA *ch, SKILL_DATA *nsk ) );
SKILL_DATA  *nskill_find    args( (SKILL_DATA *paf, int sn) );
void	nskill_remove	args( ( CHAR_DATA *ch, SKILL_DATA *nsk ) );
bool	has_nskill	args( ( CHAR_DATA *ch, int sn ) );
void	clear_nskill	args( ( CHAR_DATA *ch ) );
CHAR_DATA* get_group_room	args ( (CHAR_DATA* ch, bool fCharm) );
CHAR_DATA* get_group_world	args ( (CHAR_DATA* ch, bool fCharm) );
OBJ_DATA* has_twohanded	args ( (CHAR_DATA* ch) );
bool	is_armor_enhanced      (CHAR_DATA* ch);

/* interp.c */
void	interpret	args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number	args( ( char *arg ) );
int	number_argument	args( ( char *argument, char *arg ) );
int	mult_argument	args( ( char *argument, char *arg) );
char *	one_argument	args( ( char *argument, char *arg_first ) );
char *	one_argument_2	args( ( char *argument, char *arg_first ) );
void	WAIT_STATE2	args( ( CHAR_DATA *ch, int npulse ) );
bool    check_social    args( ( CHAR_DATA *ch, char *command, char *argument ) );
int	adjust_condition      (OBJ_DATA* obj, int val);

/* misc.c */
void			bankUpdate( CHAR_DATA* ch );
bool	hasVoted	( DESCRIPTOR_DATA* d );
void	create_spirit_mob( ROOM_INDEX_DATA* from, int to_room, char* name );
char*	short_bar( int cur, int max );
void	health_prompt	args( (char* buf, int cur, int max, bool fDig) );
int	find_first_step args( (ROOM_INDEX_DATA *src, ROOM_INDEX_DATA *target, int maxdist, bool fPassDoor, int* dist ) );
void	do_hal		( char* to, char* subject, char* text, int type );
bool	trip_traps	(CHAR_DATA* ch, TRAP_DATA* list );
DESCRIPTOR_DATA*	bring( ROOM_INDEX_DATA* room, char* argument );
void    purge		( CHAR_DATA* victim );
void	save_challenges	( );
void	load_challenges ( );
void	update_challenge( CHAR_DATA* ch, char* name, char* record, int win, int loss, int tie, int refused );

/* magic.c */
int	find_spell	args( ( CHAR_DATA *ch, const char *name) );
//int     mana_cost       args( (CHAR_DATA *ch, int min_mana, int level) );
int     _mana_cost      args( ( CHAR_DATA *ch, int spell_number) );
int	skill_lookup	args( ( const char *name ) );
int	psalm_lookup	      (const char* sn);
int	psalm_lookup_sn	      (int sn);
int	psalm_check_lookup_sn	      (int sn);
int	slot_lookup	args( ( int slot ) );
bool	saves_spell	args( ( int level, CHAR_DATA *victim, int dam_type, int spell_type ) );
void    obj_cast_spell  args( ( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj ) );

/* mob_prog.c */
#ifdef DUNNO_STRSTR
char *  strstr                  args ( (const char *s1, const char *s2 ) );
#endif
void    mprog_wordlist_check    args ( ( char * arg, CHAR_DATA *mob, CHAR_DATA* actor, OBJ_DATA* object, void* vo, int type ) );
void    mprog_percent_check     args ( ( CHAR_DATA *mob, CHAR_DATA* actor, OBJ_DATA* object, void* vo, int type ) );
void    mprog_act_trigger       args ( ( char* buf, CHAR_DATA* mob, CHAR_DATA* ch, OBJ_DATA* obj, void* vo ) );
void    mprog_bribe_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch, int amount ) );
void    mprog_entry_trigger     args ( ( CHAR_DATA* mob ) );
void    mprog_give_trigger      args ( ( CHAR_DATA* mob, CHAR_DATA* ch, OBJ_DATA* obj ) );
void    mprog_greet_trigger     args ( ( CHAR_DATA* mob ) );
void    mprog_fight_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_hitprcnt_trigger  args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_death_trigger     args ( ( CHAR_DATA *ch, CHAR_DATA* mob )
);
void    mprog_random_trigger    args ( ( CHAR_DATA* mob ) );
void    mprog_speech_trigger    args ( ( char* txt, CHAR_DATA* ch, CHAR_DATA* mob ) );
void    mprog_tell_trigger    args ( ( char* txt, CHAR_DATA* mob ) );
void	mprog_time_trigger	args ( ( CHAR_DATA* mob ) );
void    mprog_get_trigger       args ( ( CHAR_DATA* mob, CHAR_DATA* ch, OBJ_DATA* obj ) );
void    mprog_drop_trigger      args ( ( CHAR_DATA* mob, CHAR_DATA* ch, OBJ_DATA* obj ) );
void    mprog_guild_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_bloody_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_justice_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );


/* new prog functions */
PROG_CODE*    get_prog_index args( ( int vnum, int type ) );
void	program_flow	args( ( sh_int vnum, char *source, CHAR_DATA *mob,
				OBJ_DATA *obj, ROOM_INDEX_DATA *room,
				CHAR_DATA *ch, const void *arg1,
				const void *arg2 ) );
void	p_act_trigger	args( ( char *argument, CHAR_DATA *mob,
				OBJ_DATA *obj, ROOM_INDEX_DATA *room,
				CHAR_DATA *ch, const void *arg1,
				const void *arg2, int type ) );
bool	p_percent_trigger args( ( CHAR_DATA *mob, OBJ_DATA *obj,
				ROOM_INDEX_DATA *room, CHAR_DATA *ch,
				const void *arg1, const void *arg2, int type ) );
void	p_bribe_trigger  args( ( CHAR_DATA *mob, CHAR_DATA *ch, int amount ) );
bool	p_exit_trigger   args( ( CHAR_DATA *ch, int dir, int type ) );
void	p_give_trigger   args( ( CHAR_DATA *mob, OBJ_DATA *obj,
				ROOM_INDEX_DATA *room, CHAR_DATA *ch,
				OBJ_DATA *dropped, int type ) );
void 	p_greet_trigger  args( ( CHAR_DATA *ch, int type ) );
void	p_hprct_trigger  args( ( CHAR_DATA *mob, CHAR_DATA *ch ) );

void mob_interpret	args( ( CHAR_DATA *ch, char *argument ) );
void obj_interpret	args( ( OBJ_DATA *obj, char *argument ) );
void room_interpret	args( ( ROOM_INDEX_DATA *room, char *argument ) );

/* recycle.c */
bool	check_ban	args( ( char *site, int type) );
bool	check_spam	args( ( char *site ) );
void	do_traceban	args( ( char *argument ) );
bool    add_buf		args( ( BUFFER *buffer, char *string ) );
void	append_note	args( ( NOTE_DATA *pnote ) );
void temp_hash_add      ( char *, int len );
char *temp_hash_find    ( const char *, int len );
void free_quest		( QUEST_DATA* q);

/* save.c */
void	save_char_obj	args( ( CHAR_DATA *ch ) );
int	load_char	args( ( DESCRIPTOR_DATA *d, char *name ) );
char *	load_purge	args( ( char *name ) );
void	load_obj	args( ( CHAR_DATA *ch ) );

/* s_skill.c */
void	interpret_select	args ( ( CHAR_DATA *ch, char *argument ) );
int	ss_lookup		args ( ( const char *name ) );
int	ss_group_lookup		args ( ( const char *name ) );
int	can_group_select	args ( ( CHAR_DATA* ch, int gr, int level) );
int	can_ss_select		args ( ( CHAR_DATA* ch, int* lvl) );
int	show_group_choice	args ( ( CHAR_DATA* ch, int group, bool fShow) );

/* skills.c */
bool	spellkillerCheck( CHAR_DATA* ch, CHAR_DATA* victim, int spell );
bool	predictCheck( CHAR_DATA* ch, CHAR_DATA* victim, char* command, char* skill);
bool    parse_gen_groups args(( CHAR_DATA *ch, char *argument ) );
void    list_group_costs args(( CHAR_DATA *ch ) );
void    list_group_known args(( CHAR_DATA *ch ) );
int     exp_per_level   args( ( CHAR_DATA *ch, int level ) );
long     total_exp       args( ( CHAR_DATA *ch ) );
void    check_improve   args( ( CHAR_DATA *ch, int sn, bool success, int bonus_gain ) );
int     group_lookup    args( ( const char *name) );
void	gn_add		args( ( CHAR_DATA *ch, int gn) );
void 	gn_remove	args( ( CHAR_DATA *ch, int gn) );
void 	group_add	args( ( CHAR_DATA *ch, const char *name, bool deduct) );
void	group_remove	args( ( CHAR_DATA *ch, const char *name) );
void    hunt_victim     args( ( CHAR_DATA *ch ) );
bool    is_empowered    args( ( CHAR_DATA *ch, int type) );
bool    is_empowered_quiet    args( ( CHAR_DATA *ch, int type) );
void    beast_transform args(( CHAR_DATA *ch ) );

/* song.c */
void    s_yell          args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool force ) );
int	song_lookup	args( ( const char *name ) );
int	sglevel		args( ( CHAR_DATA *ch, int sn ) );
int	find_song	args( ( CHAR_DATA *ch, const char *name) );
bool	refrain_sing	args( ( CHAR_DATA *ch, int sn ) );
void	song_strip	args( ( CHAR_DATA *ch, int sn ) );
void	song_to_room	args( ( CHAR_DATA *ch, char *argument, bool brief, bool sleep, bool duet) );


/* special.c */
bool	spec_path_walk (CHAR_DATA *ch);
SF *	spec_lookup	args( ( const char *name ) );
char *	spec_name	args( ( SPEC_FUN *function ) );
void	acid_effect	args( (void *vo, int level, int dam, int target) );
void	cold_effect	args( (void *vo, int level, int dam, int target) );
void	fire_effect	args( (void *vo, int level, int dam, int target) );
void	poison_effect	args( (void *vo, int level, int dam, int target) );
void	shock_effect	args( (void *vo, int level, int dam, int target) );
void	paralyze_effect	args( (void *vo, int level, int dam, int target) );
void    barb_effect     args( (void *vo, int level, int dam, int target) );
void    wound_effect    args( (CHAR_DATA* ch, CHAR_DATA* victim,  int level, int dam) );

/* cabal.c / cabal2.c */
bool same_clan		args( (CHAR_DATA* ch, CHAR_DATA* victim) );
bool guard_fade_check	args( (CHAR_DATA* ch) );
bool sneak_fade_check	args( (CHAR_DATA* ch) );
int  member_present	args( (int cabal) );
inline int GET_CP	args( (CHAR_DATA* ch) );
int CPS_GAIN		args( (CHAR_DATA* ch, int gain, bool fshow) );
int CP_GAIN		args( (CHAR_DATA* ch, int gain, bool fshow) );
AFFECT_DATA* is_exiled	    ( CHAR_DATA* ch, int vnum );
bool check_contingency	args( ( CHAR_DATA* ch, AFFECT_DATA* pCon, int flag ) );
bool check_ccombat_actions  ( CHAR_DATA* ch, CHAR_DATA* victim, int type );

/* jail.c */
void	jail_abuse_check args( (CHAR_DATA* ch, bool fGain) );
bool	jail_check	args( (ROOM_INDEX_DATA* room) );
void	jail_char	args( (CHAR_DATA* cha, CHAR_DATA* victim) );
int	jail_cell_lookup args( (int vnum) );
void	unjail_char	args( (CHAR_DATA* ch) );
AFFECT_DATA* set_sentence args( (CHAR_DATA* ch, int type, int level) );
void	jail_announce	args( (int jail, char* string) );
void	jail_execute(CHAR_DATA* ch, CHAR_DATA* mob, int exe_type, char* name);


/* avatar.c */
void	update_avatar	args( (CHAR_DATA* ch, bool fRem) );
void	divfavor_update	args( (CHAR_DATA* ch, CHAR_DATA* victim) );
void	divfavor_gain	args( (CHAR_DATA* ch, int gain) );
int	get_avatar_level args( (CHAR_DATA* ch) );
char*	get_avatar_desc args( (CHAR_DATA* ch) );
void	avatar_tick	args( (CHAR_DATA* ch) );


/* update.c */
void	advance_level	args( ( CHAR_DATA *ch, bool hide ) );
void	gain_exp	args( ( CHAR_DATA *ch, int gain ) );
void	gain_condition	args( ( CHAR_DATA *ch, int iCond, int value ) );
void	update_handler	args( ( void ) );

/* Viri: Make those more usefull
#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	TD
#undef	TID
#undef	CID
#undef	HID
#undef	RID
#undef	AID
#undef	SF
#undef  AD
*/

/****** SAM *********/

struct size_count_data
{
    int count;
    int size;
};

struct system_data
{
    struct
    {
        long heapsize;
        int allocs;
        int frees;
        struct
	{
            int size;
            int count;
            int highwater;
        } used;
        struct size_count_data free;
        struct size_count_data other;
        int defrags;
        int newchunks;
        int merges;
    } sam;
};

extern struct system_data sysdata;


/*********************************** OLC ********************************/

/* Object defined in limbo.are                      *
 * Used in save.c to load objects that don't exist. */
#define OBJ_VNUM_DUMMY 30

/* Area flags. */
#define         AREA_NONE       0
#define         AREA_CHANGED    A      /* Area has been modified. */
#define         AREA_ADDED      B      /* Area has been added to. */
#define         AREA_LOADING    C      /* Used for counting in db.c */
#define         AREA_CITY       D      /* Used for counting in db.c */
#define         AREA_MUDSCHOOL  E      /* Used for various */
#define         AREA_NOMORTAL   F      /* Prevents teleport by mortal */
#define         AREA_NOREPOP	G      /* Repop ONLY if empty */
#define         AREA_RESTRICTED H      /* Restricted access or system area */
#define         AREA_LAWFUL     I      /* causes AREA_CITY to be set in this area regardless of pacts with justice */
#define         AREA_MARKED    (J)     /* used for searches across area data */
#define         AREA_NOPATH    (K)     /* not used for pathfinding  */
#define         AREA_RAID      (L)     /* raiders have damaged the city*/
#define         AREA_CABAL     (M)     /* area is cabal home */


#define MAX_DIR        6
#define NO_FLAG -99    /* Must not be used in flags or stats. */

/* Global Constants */
extern char *  const   dir_name        [];
extern char *  const   dir_name2       [];
extern char *  const   shrt_dir_name   [];
extern const   sh_int  rev_dir         [];          /* sh_int - ROM OLC */
extern const   sh_int  movement_loss   [];
extern const   struct  spec_type       spec_table      [];

/* Global variables */
extern          AREA_DATA *             area_first;
extern          AREA_DATA *             area_last;
extern          SHOP_DATA *             shop_last;
extern          int                     top_affect;
extern          int                     top_area;
extern          int                     top_ed;
extern          int                     top_exit;
extern          int                     top_help;
extern          int                     top_mob_index;
extern          int                     top_obj_index;
extern          int                     top_cabal_index;
extern          int                     top_reset;
extern          int                     top_room;
extern          int                     top_shop;
extern          int                     top_vnum_mob;
extern          int                     top_vnum_obj;
extern          int                     top_vnum_room;
extern          char                    str_empty       [1];

extern  MOB_INDEX_DATA *        mob_index_hash  [MAX_KEY_HASH];
extern  OBJ_INDEX_DATA *        obj_index_hash  [MAX_KEY_HASH];
extern  ROOM_INDEX_DATA *       room_index_hash [MAX_KEY_HASH];

/* db.c */
void   reset_area      args( ( AREA_DATA * pArea ) );
void   reset_room      args( ( ROOM_INDEX_DATA *pRoom ) );

/* string.c */
void   string_edit     args( ( CHAR_DATA *ch, char **pString ) );
void   string_append   args( ( CHAR_DATA *ch, char **pString ) );
char * string_replace  args( ( char * orig, char * old, char * new ) );
void   string_add      args( ( CHAR_DATA *ch, char *argument ) );
char * format_string   args( ( char *oldstring /*, bool fSpace */ ) );
char * first_arg       args( ( char *argument, char *arg_first, bool fCase ) );
char * string_unpad    args( ( char * argument ) );
char * string_proper   args( ( char * argument ) );

/* olc.c */
bool   run_olc_editor  args( ( DESCRIPTOR_DATA *d ) );
char   *olc_ed_name    args( ( CHAR_DATA *ch ) );
char   *olc_ed_vnum    args( ( CHAR_DATA *ch ) );
char   *flag_string    args( ( const struct flag_type *flag_table, int bits ));
char   *fwrite_flag    args( ( long flags, char buf[] ) );
void	show_flag_cmds( CHAR_DATA *ch, const struct flag_type *flag_table );
int	wear_bit       args ( (int loc) );
int	wear_loc       args ( (int bits, int count) );

/* bit.c */
extern const struct flag_type  area_flags[];
extern const struct flag_type  sex_flags[];
extern const struct flag_type  exit_flags[];
extern const struct flag_type  door_resets[];
extern const struct flag_type  room_flags[];
extern const struct flag_type  room_flags2[];
extern const struct flag_type  sector_flags[];
extern const struct flag_type  type_flags[];
extern const struct flag_type  extra_flags[];
extern const struct flag_type  wear_flags[];
extern const struct flag_type  act_flags[];
extern const struct flag_type  act2_flags[];
extern const struct flag_type  affect_flags[];
extern const struct flag_type  affect2_flags[];
extern const struct flag_type  apply_flags[];
extern const struct flag_type  wear_loc_strings[];
extern const struct flag_type  wear_loc_flags[];
extern const struct flag_type  container_flags[];

/* ROM OLC: */
extern const struct flag_type   form_flags[];
extern const struct flag_type   special_flags[];
extern const struct flag_type   part_flags[];
extern const struct flag_type   ac_type[];
extern const struct flag_type   size_flags[];
extern const struct flag_type   off_flags[];
extern const struct flag_type   imm_flags[];
extern const struct flag_type   res_flags[];
extern const struct flag_type   vuln_flags[];
extern const struct flag_type   position_flags[];
extern const struct flag_type   weapon_class[];
extern const struct flag_type   weapon_type2[];
extern const struct flag_type   socket_flags[];
extern const struct flag_type   trap_flags[];
extern const struct flag_type   trap_table[];
extern const struct flag_type   damtype_table[];
extern const struct thief_trap_type   thief_traps[];
extern const struct flag_type   apply_types[];
extern const struct flag_type   projectile_type[];
extern const struct flag_type   proj_spec_type[];
extern const struct flag_type   ranged_type[];
struct bit_type
{
       const   struct  flag_type *     table;
       char *                          help;
};
extern const   struct  bit_type        bitvector_type[];

/* Hot reboot macro */
#define CH(descriptor)  ((descriptor)->original ? \
(descriptor)->original : (descriptor)->character)

/********************************************************************************/
/* More definitions */

void    dump_corpse     	args( ( CHAR_DATA *ch ) );
void    eq_purge        	args( ( CHAR_DATA *ch ) );
void    deny_record     	args( ( CHAR_DATA *ch ) );
bool    is_ignore       	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
char *  format_obj_to_char      args( ( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort ) );
void    show_list_to_char       args( ( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing ) );
void    list_to_char       args( ( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing, bool fShowAll ) );
void    show_char_to_char_0     args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    char_to_char		args( ( CHAR_DATA *victim, CHAR_DATA *ch, bool fAll ) );
void    show_char_to_char_1     args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char       args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool    check_blind             args( ( CHAR_DATA *ch ) );
bool    is_ignore               args( ( CHAR_DATA *ch, CHAR_DATA *victim));
int     find_door       	args( ( CHAR_DATA *ch, char *arg ) );
OBJ_DATA*    has_key         	args( ( CHAR_DATA *ch, int key ) );
void    show_char_to_char_0     args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
bool    remove_obj      	args( (CHAR_DATA *ch, int iWear, bool fReplace ) );
int     get_cost        	args( (CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ) );
void    obj_to_keeper   	args( (OBJ_DATA *obj, CHAR_DATA *ch ) );
ROOM_INDEX_DATA * room_index_hash [MAX_KEY_HASH];
ROOM_INDEX_DATA * find_location args( ( CHAR_DATA *ch, char *arg ) );
void    weather_update  	args( ( void ) );
void    char_update     	args( ( void ) );
void    obj_update      	args( ( void ) );
int     flag_value      	args( ( const struct flag_type *flag_table, char *argument) );
void	do_lfind		args( (CHAR_DATA *ch, char *argument) );
void	do_rfind		args( (CHAR_DATA *ch, char *argument) );
void	do_wfind		args( (CHAR_DATA *ch, char *argument) );
void	do_wefind		args( (CHAR_DATA *ch, char *argument) );
void	do_gofind		args( (CHAR_DATA *ch, char *argument) );
void	do_imfind		args( (CHAR_DATA *ch, char *argument) );
void	do_efind		args( (CHAR_DATA *ch, char *argument) );
void	do_sfind		args( (CHAR_DATA *ch, char *argument) );
void	do_ssfind		args( (CHAR_DATA *ch, char *argument) );
void	do_skfind		args( (CHAR_DATA *ch, char *argument) );
void	init_signals   		args( (void) );
void	do_auto_shutdown 	args( (void) );
bool    check_parse_name        args( ( char *name ) );
bool    check_exist_name        args( ( char *name ) );
bool    check_reconnect         args( ( DESCRIPTOR_DATA *d, char *name, bool fConn ) );
bool    check_playing           args( ( DESCRIPTOR_DATA *d, char *name ) );
int     main                    args( ( int argc, char **argv ) );
void    nanny                   args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool    process_output          args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void    read_from_buffer        args( ( DESCRIPTOR_DATA *d ) );
void    stop_idling             args( ( CHAR_DATA *ch ) );
void    bust_a_prompt           args( ( CHAR_DATA *ch ) );
void    install_other_handlers  args( ( ) );
pid_t   getpid                  args( ( void ) );
void    do_pipe         args( ( CHAR_DATA *ch, char *argument ) );
void    game_loop_unix          args( ( int control ) );
int     init_socket             args( ( int port ) );
void    init_descriptor         args( ( int control ) );
bool    read_from_descriptor    args( ( DESCRIPTOR_DATA *d ) );
bool    write_to_descriptor     args( ( int desc, char *txt, int length ) );
void    hotreboot_recover       args( ( void ) );
bool    write_to_descriptor     args( ( int desc, char *txt, int length ) );
int     execl           args( ( const char *path, const char *arg, ... ) );
int     close           args( ( int fd ) );


void    check_assist		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

bool	check_parry_monk	args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool Second, bool fVirt, int sn) );
bool	check_parry		args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn) );
bool	check_dual_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level,  bool fVirt, int sn) );
bool	check_shield_block	args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn) );
bool	check_block		args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn) );
bool	check_dodge		args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn) );
bool	check_riposte		args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn) );
bool	check_blink		args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn) );


void    death_cry       args( ( CHAR_DATA *ch, CHAR_DATA* killer ) );
void    group_gain      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int     xp_compute      args( ( CHAR_DATA *gch,CHAR_DATA *victim, int total_levels, int members ) );
bool    is_safe         args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
OBJ_DATA* make_corpse     args( ( CHAR_DATA *ch ) );
void    dump_corpse     args( ( CHAR_DATA *ch ) );
void    one_hit         args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary ) );
void    mob_hit         args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    set_fighting    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    disarm          args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	kick		( CHAR_DATA* ch, CHAR_DATA* victim, int chance );
void	disarm_offhand	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_area_safe    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    a_yell          args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    save_challenge  args( ( CHAR_DATA *ch, CHAR_DATA *victim, int result ) );
void    do_fulfill      args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void    do_fulfill_bounty      args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void	mercy_effect	      ( CHAR_DATA *och, CHAR_DATA *victim, int dur );
void    mortal_wound    args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
bool    is_challenger   args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void    remove_challenge args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
bool	challenge_safe	args( ( CHAR_DATA* ch, CHAR_DATA* victim) );
void    affect_modify   args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
FILE            *       popen           args( ( const char *command, const char *type ) );
int                     pclose          args( ( FILE *stream ) );
char            *       fgetf           args( ( char *s, int n, register FILE *iop ) );
void    save_limits     args( ( void ) );
void    remove_limit    args( ( int vnum ) );
int	rename		args( (const char *oldfname, const char *newfname) );
void    fwrite_char     args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fwrite_obj      args( ( CHAR_DATA *ch,  OBJ_DATA  *obj, FILE *fp, int iNest, int hold_limit, bool CABAL ) );
void    fwrite_pet      args( ( CHAR_DATA *pet, FILE *fp, bool fArmy) );
void    fread_char      args( ( CHAR_DATA *ch,  FILE *fp ) );
CHAR_DATA*    fread_pet args( ( CHAR_DATA *ch,  FILE *fp, ROOM_INDEX_DATA* room ) );
OBJ_DATA* fread_obj	args( (CHAR_DATA *ch, FILE *fp, ROOM_INDEX_DATA* pRoom ) );
void    fread_null      args( ( CHAR_DATA *ch,  FILE *fp ) );
int     hit_gain        args( ( CHAR_DATA *ch ) );
int     mana_gain       args( ( CHAR_DATA *ch ) );
int     move_gain       args( ( CHAR_DATA *ch ) );
void    mobile_update   args( ( void ) );
void    weather_update  args( ( void ) );
void    char_update     args( ( void ) );
void    obj_update      args( ( void ) );
void    aggr_update     args( ( void ) );
void    obj_to_keeper   args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
bool    write_to_descriptor args( ( int desc, char *txt, int length ) );
bool    is_challenger   args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    return_to_leader args(( CHAR_DATA *ch ) );
void    say_spell       args( ( CHAR_DATA *ch, int sn ) );
void    wear_obj        args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, bool twohands ) );
int     find_door       args( ( CHAR_DATA *ch, char *arg ) );
void    m_yell          args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool force ) );
bool    saves_dispel    args( ( int dis_level, int spell_level, int duration) );
bool    check_dispel    args( ( int dis_level, CHAR_DATA *victim, int sn) );
void    shield_disarm   args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	questnum	args( ( CHAR_DATA *ch, char *argument ) );
void	do_throw_monk	args( ( CHAR_DATA *ch, char *argument ) );
void	show_quest	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	dump_core	args( ( const char *why ) );
pid_t	waitpid		args( ( pid_t pid, int *status, int options) );
bool	vamp_check	args( ( CHAR_DATA *ch ) );
bool	vamp_day_check	args( ( CHAR_DATA *ch ) );
bool	in_monastery	args( ( CHAR_DATA *ch ) );
void	do_in		args( ( CHAR_DATA *ch ) );
void	do_out		args( ( CHAR_DATA *ch, int door ) );
int	get_monk	args( ( int value ) );
bool	monk_good	args( ( CHAR_DATA *ch, int value ) );
void	set_delay	args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void	ecto_vapor	args( ( CHAR_DATA *ch, CHAR_DATA *ecto) );
void	ecto_solid	args( ( CHAR_DATA *ch, CHAR_DATA *ecto) );
int     spell_lvl_mod   args( ( CHAR_DATA *ch, int spell_number) );
bool    vamp_can_sneak  args( ( CHAR_DATA *ch, ROOM_INDEX_DATA* to_room) );
bool    effect_dark_metamorphosis  args( (CHAR_DATA* ch, CHAR_DATA* victim, int dam, int dam_type) );
int     dam_spell       args(  (CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dam_type) );
void     make_item_char args(  (CHAR_DATA*, int vnum, int life) );
inline bool  bat_blind  args(  (CHAR_DATA* ch) );
int get_spell_damage    args(  (CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dam_type) );
void do_broadcast       args(  (CHAR_DATA *ch, char* argument ) );
void broadcast          args(  (CHAR_DATA *ch, const char *string) );

//update.c
void do_embrace_damage  args( (CHAR_DATA* ch) );

//act_move.c
bool room_spec_update   args( (ROOM_INDEX_DATA* in_room) );

//handler.c
void un_unlife          args( (CHAR_DATA* ch) );
bool has_custom_title   args( (CHAR_DATA* ch) );
inline void string_to_affect   args( (AFFECT_DATA* paf, char* str) );
inline void string_from_affect   args( (AFFECT_DATA* paf) );
int deity_lookup        args( (const char *name ) );
int way_lookup         args( (const char *name ) );
inline bool is_owner	args( (OBJ_DATA* obj, CHAR_DATA* ch) );
inline bool can_take	args( ( CHAR_DATA *ch, OBJ_DATA *obj) );
inline char*		args( get_vir_attack(int dt) );
inline char*		args( get_skill_name(int sn) );

//fight.c
void continuum          args( (CHAR_DATA* ch, int type)  );
int get_AC		args( (CHAR_DATA *ch, CHAR_DATA *victim,int dt, int dam_type, bool fVirt) );
int get_THAC0		args( (CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* obj, int dt, bool secondary, bool fNpc, long act, bool fVirt, int gsn, int level) );

//magic3.c
void embrace_poison     args( (CHAR_DATA* ch, CHAR_DATA* victim) );
inline bool effect_field_of_fear  args( (CHAR_DATA* ch) );

//magic2.c
CHAR_DATA* create_zombie args( (CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj) );
//magic.c
inline bool embrace_heal_check args(  (int sn, int level, CHAR_DATA* ch, CHAR_DATA* vic, int healed) );

//skills.c
int get_lagprot 	args( (CHAR_DATA* ch) );
void lagprot_yell	args( (CHAR_DATA* ch, CHAR_DATA* victim, int gsn, int shield) );
void lagprot_msg	args( (CHAR_DATA* ch, CHAR_DATA* victim, int sn, int shield, bool fYell) );
bool do_lagprot		args( (CHAR_DATA* ch, CHAR_DATA* victim, int chance, int* do_dam, int* lag, int sn, bool fSuccess, bool fYell) );
void trip		args( (CHAR_DATA* ch, CHAR_DATA* victim, int chance) );
//skills2.c

//skills3.c
inline int is_mcharged       args ( (OBJ_DATA* obj) );
inline int get_maxmcharge    args ( (OBJ_DATA* obj) );
inline int mana_flare        args ( (CHAR_DATA* ch, int mana) );

//effect.c
int gn_pos             args( (sh_int gn) );
int effect_lookup      args( ( const char *name ) );
int run_effect_update  args( (CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag, int UPDATE_TYPE) );
bool mcharge_info      args( (CHAR_DATA* ch,  char* buf, OBJ_DATA* obj, AFFECT_DATA* paf, bool say) );
char* study_info       args( (CHAR_DATA* ch, bool fImm, CHAR_DATA* vic) );
char* abduct_info       args( (CHAR_DATA* ch) );
void hwep_gain	       (CHAR_DATA* ch, CHAR_DATA* victim, int mult );

//db.c
bool    format_room_name args( (ROOM_INDEX_DATA* room, char* name) );

