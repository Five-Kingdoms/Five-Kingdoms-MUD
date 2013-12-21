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
#define		CLAN_DIR	"../clans/"
#define		CLAN_TMP_FILE	"temp_clan"
#define		MAX_CLANS	32
#define		MAX_INACTIVE	604800
#define		CLAN_APPLY_WIN  604800

/* data types */
typedef struct clan_data	CLAN_DATA;
typedef struct clan_member	CLAN_MEMBER;

/* macros */
#if defined(KEY)
#undef KEY
#endif
#define	KEY( literal, member, value )		\
	if (!str_cmp(word, literal)){		\
	  member = value;			\
	  fMatch = TRUE;			\
	  break;				\
	}					\

#if defined(KEYS)
#undef KEYS
#endif
#define KEYS(literal, member, value)		\
	if (!str_cmp(word, literal)){		\
        member  = value;                         \
        fMatch = TRUE;				\
        break;                                  \
    }	

/* structs */
struct clan_member{
  CLAN_MEMBER* next;

  char*		name;
  int		race;
  int		class;
  int		level;
  int		rank;
  int		pts;
  
  long		joined;
  long		last_logon;
  long		allowed;
};

struct clan_data{
  char*		name;
  CLAN_MEMBER	members;
  int		max;
};


/* INTERFACE */
void		ClanSave( int handle );
void		SaveClans();
void		LoadClans();
int		GetClanHandle( char* name );
char*		GetClanName( int handle );
void		SynchClanData( CHAR_DATA* ch );
CLAN_MEMBER*	GetClanMember( int handle, char* name );
void		CharFromClan( CHAR_DATA* ch );
void		CharToClan( CHAR_DATA* ch, int handle );
bool		ClanApplicationCheck( CHAR_DATA* ch, CABAL_DATA* pCab);
void		ClanEcho( int handle, char* string );
int		ClanSecondsToApply( CHAR_DATA* ch );
bool		ClanRename(int handle, char* current, char* new );
