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

/* typedefs */


/* globals/typedefs */



/* constants */
#define VOTE_VALS		5
#define VOTE_DEFMAX		128
#define VOTE_MAXSKIP		3

/* error returns from create_app */
#define	VOTE_NONE		0
#define	VOTE_ERROR		1
#define	VOTE_ACCEPTED		2

/* vote weights */
#define VOTE_WEIGHT_NORM	1
#define VOTE_WEIGHT_ELDER	2
#define VOTE_WEIGHT_LEADER	3


/* vote flags */
#define VOTE_NONE		0
#define VOTE_URGE		(A)	//vote duration cab be lowered at cost
#define VOTE_SUBJ		(B)	//creator of vote can set the subject
#define VOTE_NOOWNER		(C)	//creator of vote is the owner automaticly
#define VOTE_DUP		(D)	//vote can always be duplicated
#define VOTE_NOCAB_COST		(E)	//vote does not cost cabal anything

/* types of votes (the values MUST correspond to entries in the vote_table) */
#define VOTE_UNKNOWN	0
#define VOTE_GENERAL	1	//creates a note with results on completion
#define VOTE_CAB_APPLY	2	//inducts a member on completion
#define VOTE_CAB_PROM	3	//promotes a member on completion
#define VOTE_CAB_DEMO	4	//demotes a member on completion
#define VOTE_CAB_ELDER	5	//elders a member on completion
#define VOTE_CAB_LEADER	6	//leaders a member on completion
#define VOTE_CAB_POLL	7	//a general yes/no poll
#define VOTE_LAW	8	//changes crime penalties for a given royal cabal/city
#define VOTE_PACT	9	//sets up the whole pact agreenment cycle between two cabals
#define VOTE_CAB_TAX	10	//changes the rate of taxation on any cp gains fro cabal members
#define VOTE_BUILD	11	//created by "do_build" allows a virtual room to be created
#define VOTE_TITHE	12	//transfers cps to another cabal
#define VOTE_CAB_EXPEL	13	//kicks out a cabal member, can only be done on ranks lower then ELDER
#define VOTE_CAB_SPONS	14	//toggles the sponsor requirement for cabal
#define VOTE_TOME	15	//creates a tome



/* duration of votes */
#define VOTE_MINUTE	60
#define VOTE_HOUR	3600
#define VOTE_DAY	86400

#define VOTE_DUR_VSHORT	4  * VOTE_HOUR
#define VOTE_DUR_SHORT	     VOTE_DAY
#define VOTE_DUR_MED	3  * VOTE_DAY
#define VOTE_DUR_LONG	5  * VOTE_DAY
#define VOTE_DUR_VLONG	7  * VOTE_DAY

#define VOTE_TIME_TO_REAPPLY	3 * VOTE_DAY

/* VOTES */
#define VOTE_NONE		0
#define	VOTE_CAB_APP		1


/* main vote structure */
struct vote_data{
  VOTE_DATA*	next;
  bool		valid;
  int		type;		//type of the vote
  char*		owner;		//string for owner/target of the vote (char/cabal etc.)

  int		yes;
  int	        no;
  int		pass;
  bool		veto;		//was the vote vetoed. (used only right after voting )
  int		max_vote;	//totaly votes after which the vote is tallied

  time_t	time_stamp;	//time at which vote was created
  time_t	life_time;	//length after which vote is autotallied

  int		value[VOTE_VALS];//misc values

  char*		from;		//creator of the vote
  char*		subject;	//subject of the vote
  char*		string;		//misc. string if needed
};


/* table for various votes with lengths etc. */
struct vote_type{
  char*		name;
  int		life_time;
  int		flag;

};

/* externs */
extern	VOTE_DATA*			vote_list;
extern const struct vote_type		vote_table[];

/* macros */
#define IS_VALID(data)		((data) != NULL && (data)->valid)


/* functions */
int		vote_lookup	(const char *name );
CVOTE_DATA*	get_cvote	( CABAL_INDEX_DATA* pCabal, int type );
void		show_votes	( CHAR_DATA* ch );
void		free_vote	(VOTE_DATA *vote);
VOTE_DATA*	new_vote	();
void		add_vote	( VOTE_DATA* pv );
bool		rem_vote	( VOTE_DATA* pv );
void		read_vote	(FILE* fp, VOTE_DATA* vote);
void		write_vote	(FILE* fp, VOTE_DATA* vote);
VOTE_DATA*	create_vote	(CHAR_DATA* ch, char* owner, char* subject, char* string, int type, int v0, int v1, int v2, int v3, int v4);
bool		create_app_vote ( CHAR_DATA* ch, char* to, char* subject, char* text );
bool		can_create_cvote( CHAR_DATA* ch, CVOTE_DATA* cVote);
bool		is_vote_to	(CHAR_DATA* ch, VOTE_DATA* pv);
int		has_votes	( CHAR_DATA* ch );
void		interpret_vote	(CHAR_DATA* ch, char* argument );
void		check_vote	( VOTE_DATA* pv );
void		vote_update	( );
bool		is_duplicated	( VOTE_DATA* pVote );
