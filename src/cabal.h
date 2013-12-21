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
 * and -Mirlan-: The Age of Mirlan Mud by Virigoth                        *
 *                                                                         *
 * Continued Production of this code is available at www.flcodebase.com    *
 ***************************************************************************/
/* save version for mud file */
#define SAVECAB_VER		1000
#define CVROOMS_MARKER	-2	//maker in cabal file to read virtual rooms

/* directory and files */
#define SAVECAB_DIR        "../savemud/"        /* mud/cabal etc. files */
#define PACT_FILE	  "pacts.cab"		/* mud info */
#define SAVECAB_FILE	  "cabal.sav"		/* mud info */
#define TEMPCAB_FILE	  "cabal.tmp"		/* temporary write file */
#define TEMPCVR_FILE	  "cvrooms.tmp"		/* temporary write file */


/* duration for how long you have to return cabal item */
#define	CABAL_STANDARD_TIMER	12

/* bits for progression requirements for cabals used in cabal_progress_table */
#define PROGRESS_NONE	0
#define PROGRESS_CP	(A)	//300 cp per rank at vote creation
#define PROGRESS_SKILL	(B)	//4 spells mastered per rank (2 if B+C)
#define PROGRESS_SPELL	(C)	//4 skills mastered per rank (2 if B+C)
#define PROGRESS_QUEST	(D)	//20% of quests per rank
#define PROGRESS_COUP	(E)	//allows coup progress (kill of superior)
#define PROGRESS_CHALL	(F)	//allows for deathmark/challenge of superior

/* integer values for friend checks on cabals */
#define CABAL_FRIEND	0
#define CABAL_NEUTRAL	1
#define CABAL_ENEMY	2

/* Bit wise flags for align/ethos of cabals */
#define CALIGN_EVIL	(A)
#define CALIGN_GOOD	(B)
#define CALIGN_NEUTRAL	(C)

/* WAIT TIME for how long you can capture an item after a person quit */
#define		CABAL_PRESENT_CAPUTREWAIT	600

/* Minimum costs for the respawn items */
#define		CP_RESPAWN_5	0
#define		CP_RESPAWN_1	50
#define		CP_RESPAWN_2	200
#define		CP_RESPAWN_3	400
#define		CP_RESPAWN_4	600

/* update duration for members */
#define		CMEMBER_PULSE	0

/* used to report cabal skill cp requirements. */
#define CABAL_COST_FAILED	1001

/* GLOBAL MODIFIER TO ALL CABAL PRICES */
#define		CABAL_PROFIT	100

/* CABAL POINTS */

/* CP CONTROL VARIABLES */
#define		CP		1		//Worth of single CP
#define		CPTS		(200 * CP)	//worth of single cabal currency (CPoints)
#define		CP_BASEGAIN	50		//amount of cpts gain per tick wi

/* ECONOMY CONTROLS */
#define		ECONOMY_UPDATE	7200		//the time in seconds for unit of economic gain (cps/ECONOMY_UPDATE)
#define		ECONOMY_MIN		-250 * CPTS	//the lowest amount of cps before mud starts selling things off
#define		ECONOMY_COFF_MIN	-1000 * CPTS	//the lowest amount of cps cabal may have
#define		ECONOMY_COFF_MAX	10000 * CPTS	//the max of cp that can be stored WITH coffers
#define		ECONOMY_NOCOFF_MAX	500 * CPTS	//the max of cp that can be stored WITHOUT coffers

/* DEFINES FOR ECONOMY UPDATES */
#define		CABAL_ROOM_PULSE	ECONOMY_UPDATE		//time between room checks. (2h)
#define		CABAL_BASE_SUPPORT	500


#define		ECO_GAIN	0
#define		ECO_INCOME	1
#define		ECO_TRADE	2
#define		ECO_COST	3
#define		ECO_LAST_RENT	4
#define		ECO_LAST_ARMY	5
#define		ECO_LAST_TRADE	6
#define		ECO_LAST_CORR	7
#define		ECO_LAST_BONUS	8
#define		ECO_MAX		9


/* PACT DEFINES */
#define PACT_MARKER	-1	//maker in cabal file to read a pact instead of cabal
#define PACT_NONE	0
#define PACT_INCOMPLETE	1
#define PACT_COMPLETE	2

#define PACT_TRADE	(A)
#define PACT_NAPACT	(B)
#define PACT_ALLIANCE	(C)
#define PACT_VENDETTA	(D)
#define PACT_PEACE	(E) //this pact cancells vendetta and is removed when both sides approve
#define PACT_BREAK	(F) //this pact cancells all trade pacts between the two cabals as soon as it is voted yes
#define PACT_IMPROVE	(G) //this pact allows trade to improve to new level

#define PACT_FAIL_RATING	50		//if a pact falls below this value it is broken or downgraded
#define PACT_START_RATING	250		//start rating for pacts, goes up with time/actions, goes down with actions
#define PACT_MAX_RATING		2000		//a 100% rating for pacts, also used to calculate rate of rating increase

#define PACT_NORMAL_DUR		120		//time before normal/trade pacts achive a full rating (hours)
#define PACT_NAPACT_DUR		180		//time before non aggression pacts achive a full rating (hours)
#define PACT_ALLIANCE_DUR	360		//time before alliance achives a full rating (hours)
#define PACT_VENDETTA_DUR	240		//time before vendetta breaks off if nothing is done to support it

/* trade gains */
#define TRADE_CP_SERVE		0
#define TRADE_CP_WAR		 10
#define TRADE_CP_START		-10
#define TRADE_CP_TRADE		 25
#define TRADE_CP_NA		 40
#define TRADE_CP_MAX		 60

#define TRADE_SUP_SERVE		-10
#define TRADE_SUP_WAR		 25
#define TRADE_SUP_START		-15
#define TRADE_SUP_TRADE		 5
#define TRADE_SUP_NA		10
#define TRADE_SUP_MAX		15


/* EVENTS ORIENTATED DEFINES */

/* EVENT REWARDS */
#define		CP_EVENT_WANTED_COST	(25 * CPTS)	//Gained when a criminal is properly wanted
#define		CP_EVENT_WAN_PEN_COST	(-20 * CPTS)	//Lost when wanted someone who did not commit a crime
#define		CP_EVENT_CAPTURE_COST	(30 * CPTS)	//Gained when wanted criminal is apprehended
#define		CP_EVENT_BOUNTY_COST	(25 * CPTS)	//Gained when bounty is successfuly completed
#define		CP_EVENT_PC_COST	(60 * CPTS)	//Gained when cabal member is killed
#define		CP_EVENT_ELDER_COST	(120 * CPTS)	//Gained when elder is killed
#define		CP_EVENT_LEADER_COST	(180 * CPTS)	//Gained when leader is killed.
#define		CP_EVENT_STANDARD_COST	(25 * CPTS)	//Gained when standard is taken
#define		CP_EVENT_RETURN_COST	(30 * CPTS)	//Gained when standard is returned

#define		CP_EVENT_VICTORY_COST	(0 * CPTS)	//Gained when army  is destroyed (auto 50% bouns = 3CPS)
#define		CP_EVENT_SIEGE_COST	(10 * CPTS)	//Gained when tower is captured (auto 50% bouns = 30CPS)
#define		CP_EVENT_AREA_COST	(50 * CPTS)	//Gained when neutral area is taken
#define		CP_EVENT_CONQUER_COST	(75 * CPTS)	//Gained when areas is conquered

#define		CABAL_STALKER_REWARD	(50)		//Stalker gain for killing unnatural
/* EVENTS (split from 1-20 = object, 21-20 = character ) */

/* require nothing */
#define		CP_EVENT_NONE		0

/*OBJECT EVENTS */
#define		CP_EVENT_OBJ		1	//deummy used only for safety checks

/* CHAR EVENTS */
#define		CP_EVENT_CHAR		20	//dummy only used for safety checks
#define		CP_EVENT_PC		20
#define		CP_EVENT_WANTED		21	//criminal was properly wanted
#define		CP_EVENT_WANTED_FALSE	22	//penalty for wanting the wrong person
#define		CP_EVENT_CAPTURE	23	//criminal was captured and jailed
#define	        CP_EVENT_BOUNTY 	24
#define	        CP_EVENT_STANDARD 	25
#define	        CP_EVENT_RETURN 	26

/* army events */
#define		CP_EVENT_MILITARY	40	//marker for start of military events
#define	        CP_EVENT_VICTORY 	40	//an enemy army is destroyed
#define	        CP_EVENT_SIEGE		41	//an enemy tower/bastions is destroyed
#define	        CP_EVENT_AREA		42	//a neutral area is taken
#define	        CP_EVENT_CONQUER	43	//an area is conquered


/* CABAL ROOM BUILDER DEFINES */
#define		P_X			0	//x coordinate in the pos array (+ east, - west)
#define		P_Y			1	//x coordinate in the pos array (+ north, - south)
#define		P_Z			2	//x coordinate in the pos array (+ up, - down )

/* main structure for cabal data, delcared in merc.h */
struct	cabal_index_data;
char*	const	rank_letters [RANK_MAX];

struct pact_data;
typedef struct pact_data PACT_DATA;

/* structure for pacts */
struct pact_data{
  PACT_DATA*	next;

  int		type;			//type of pact
  int		complete;		//is it in effect? (0 no, 1 one side, 2 both sides agreed)
  int		rating;			//starts at 500, when reaches 0 is broken off
  int		Adv;			//maximum level this pact can advance too, uses same values as type

  int		c_cp;			//%cp boost to creator
  int		c_sup;			//%support boost to benefactor
  int		b_cp;			//%cp boost to benefactor
  int		b_sup;			//%support boost to benefactor

  CABAL_DATA* creator;		//pointer to creator, filled in when created
  CABAL_DATA* benefactor;		//pointer to benefactor filled in when created

  time_t	time_stamp;
};

/* structure for cabal rooms */
struct croom_type{
  char* name;
  char* desc;
  int	flags;
  int	flags2;
  int	cost;	//cost in percent of cabal's full cost
  int   percent;	//level of full support required for activation
};
extern const struct croom_type croom_table[CROOM_LEVELS];


/* macros */
#define CHANGE_CABAL( pCab )	(SET_BIT((pCab)->flags, CABAL_CHANGED))
#define VRCHANGE_CABAL( pCab )	(SET_BIT((pCab)->flags, CABAL_VRCHANGED))

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                                    \
    if ( !str_cmp( word, literal ) )                                    \
    {                                                                   \
        field  = value;                                                 \
        fMatch = TRUE;                                                  \
        break;                                                          \
    }

/* provided to free strings */
#if defined(KEYS)
#undef KEYS
#endif

#define KEYS( literal, field, value )					\
    if ( !str_cmp( word, literal ) )                                    \
    {                                                                   \
        free_string(field);                                             \
        field  = value;                                                 \
        fMatch = TRUE;                                                  \
        break;                                                          \
    }


/* functions */
void			save_cabals	( bool fChanged, CHAR_DATA* ch);
void			save_cabal	( CABAL_DATA* pCab );
void			save_pacts	( );
void			load_cabals	( );
void			load_pacts	( );
void			load_cvrooms	( );
void			fix_cabals	( );
bool			char_from_cabal	( CHAR_DATA* ch );
bool			char_to_cabal	( CHAR_DATA* ch, CABAL_DATA* pCab );
CABAL_DATA*		get_cabal	( const char *name);
CABAL_DATA*		get_cabal_vnum	( int vnum );
void			update_cabal_skills( CHAR_DATA* ch, CABAL_DATA* pCabal, bool fRemove, bool fParent );
CSKILL_DATA*		get_cskill	( CABAL_DATA* pCab, int sn );
bool			is_cabal_vnum	( CABAL_DATA* pCab, int vnum );
inline bool		_is_cabal	( CABAL_DATA* pCab, CABAL_DATA* pCabal );
bool			is_cabal	( CHAR_DATA* ch, char* const cabal );
bool			is_same_cabal	( CABAL_DATA* pCab, CABAL_DATA* pCabal );
inline CABAL_DATA* get_parent	( CABAL_DATA* pCab );
CMEMBER_DATA*		get_cmember	( char* name, CABAL_DATA* cp );
void			cmember_to_cabal( CMEMBER_DATA* cm, CABAL_DATA* cp );
CMEMBER_DATA*		add_cmember	( CHAR_DATA* ch, CABAL_DATA* pCab );
int			promote		( CHAR_DATA* ch, CABAL_DATA* pc, int gain );
void			update_cmember	( CHAR_DATA* ch );
bool			has_child_choice( CABAL_DATA* pCab, int rank );
void			cabal_echo	( CABAL_DATA* pCabal, char* string );
void			cabal_echo_flag ( int flag, char* string);
char*			get_crank	( CHAR_DATA* victim );
inline char*		get_crank_str	( CABAL_DATA* pCabal, int sex, int rank );
bool			check_cabal_req ( CHAR_DATA* ch, CABAL_DATA* pc );
void			handle_skill_cost( CHAR_DATA* ch, CABAL_DATA* pCabal, int sn );
void			handle_skill_cost_percent( CHAR_DATA* ch, CABAL_DATA* pCabal, int sn, int percent );
inline int		get_cskill_cost( CHAR_DATA* ch, ROOM_INDEX_DATA* room, CSKILL_DATA* cSkill );
void			distribute_cps  ( CHAR_DATA* ch, CABAL_DATA* pCab, int gain, bool fScale);
PACT_DATA*		create_pact	( CABAL_DATA* pCre, CABAL_DATA* pBen, int type );
PACT_DATA*		create_pact_vnum( int cre, int ben, int type );
void			free_pact	( PACT_DATA* pp );
PACT_DATA*		get_pact	( CABAL_DATA* pCabal1, CABAL_DATA* pCabal2, int type, bool fTrade );
PACT_DATA*		get_pact_abs	( CABAL_DATA* pCabal1, CABAL_DATA* pCabal2, int type, bool fTrade );
void			add_pact	( PACT_DATA* pp );
bool			rem_pact	( PACT_DATA* pp );
int			pact_gain	( CABAL_DATA* pCabal1, CABAL_DATA* pCabal2, int type, int gain );
void			load_pact	( FILE* fp );
void			show_pacts	( CHAR_DATA* ch, CABAL_DATA* pCab );
void			refresh_pact_flags();
void			cabal_update	( );
void			pact_update	( );
int			CPS_CAB_GAIN	( CABAL_DATA* pc, int gain );
int			CP_CAB_GAIN	( CABAL_DATA* pc, int gain );
inline int		GET_CAB_CP	( CABAL_DATA* pc );
int			cp_event	( CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj, int event );
int			cp_event_army	( ARMY_DATA* pa, CABAL_DATA* pEn, void* vo, int event );
void			affect_cabal_relations( CABAL_DATA* Pc1, CABAL_DATA* Pc2, int gain, bool fSelf );
void			affect_justice_relations( CABAL_DATA* pRoyal, int gain );
void			free_croom	( CROOM_DATA* cr );
CROOM_DATA*		new_croom	( );
CVROOM_DATA*		new_cvroom	( );
void			free_cvroom	( CVROOM_DATA* cvr );
void			fix_vir_exits	( CVROOM_DATA* pcv );
CVROOM_DATA*		get_cvroom_xyz	( int x, int y, int z, CABAL_DATA* pCabal, bool fComplete, bool fNotComplete );
void			cvroom_from_cabal( CVROOM_DATA* pcv, CABAL_DATA* pc );
void			cvroom_to_cabal ( CVROOM_DATA* pcv, CABAL_DATA* pc );
bool			sell_room	( CVROOM_DATA* pcv, CABAL_DATA* pCab, int cost );
void			get_new_coordinates( int* pos, int dir );
int			is_friendly	( CABAL_DATA* pCabal, CABAL_DATA* pEnemy );
void			lock_cabal	( CABAL_DATA* pCabal, bool fON );
CMEMBER_DATA*		sponsor_check	( CABAL_DATA* pCab, char* name );
void			cmember_from_cabal(CMEMBER_DATA* cm, CABAL_DATA* cp );
bool			check_promo_req ( CHAR_DATA* ch, CMEMBER_DATA* cm, int rank );
int			get_support_cp_bonus (CABAL_DATA* pCab, int support );
int			SUPPORT_GAIN	( CABAL_DATA* pCabal, int eco_sector, int gain );
int			get_corruption_support  ( CABAL_DATA* pCabal, int area_support );
void			show_cabal_map( CHAR_DATA* ch, CABAL_DATA* pCabal, int ForceZ );
int			get_wepdam	( OBJ_DATA* wield, CHAR_DATA* ch, CHAR_DATA* vch, int dt, int* dam_t );
void			refresh_cabal_support();
CABAL_DATA*		create_cabal	( CABAL_INDEX_DATA* index );
void			save_cabal_indexes( FILE *fp, AREA_DATA *pArea );
void			fread_cabal_indexes( FILE* fp, AREA_DATA* pArea );
void			justice_raid_gain( long sup, long cps );
void			stalker_reward( CHAR_DATA* vch, CHAR_DATA* victim);
bool 			check_promo_hours( CMEMBER_DATA* cMem, int rank );
void			show_altar( CHAR_DATA* ch, CHAR_DATA* altar );
CHAR_DATA*		reset_altar( ROOM_INDEX_DATA* room, CABAL_DATA* pCab );
void			cabal_altar_death( CHAR_DATA* ch, CHAR_DATA* victim );
void			handle_cabal_item_entrance( CHAR_DATA* ch, ROOM_INDEX_DATA* anchor );
OBJ_DATA*		reset_cabal_item( CHAR_DATA* altar, CABAL_DATA* pCab );
bool			is_captured( CABAL_DATA* pCab );
void		        handle_ctf_kill( CHAR_DATA* tch, CHAR_DATA* victim );
void			handle_cabal_item_floor( CHAR_DATA* altar );
bool			is_in_altar( CABAL_DATA* pCab );
void     		reset_cabal_guard( ROOM_INDEX_DATA* pRoom, CABAL_DATA* pCab );
void			remove_cabalguard( CABAL_DATA* pCab );
bool			is_captured_by( CABAL_DATA* pCabal, CABAL_DATA* pCapturee );
void			refresh_cvroom_flags( CABAL_DATA* pc );
void			CalcCabalDistances();
void			AreaInfluenceUpdate( AREA_DATA* area );
void			set_capture_item_timer( CHAR_DATA* victim );
