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


extern char str_empty[1];
extern int mobile_count;
extern int top_trap_index; 

long 	get_pc_id args( (void) );
long	get_mob_id args( (void) );

/* stuff for providing a crash-proof buffer */
#define MAX_BUF		16384
#define MAX_BUF_LIST 	11
#define BASE_BUF 	1024

/* valid states */
#define BUFFER_SAFE	0
#define BUFFER_OVERFLOW	1
#define BUFFER_FREED 	2

#define ND NOTE_DATA
ND	*new_note args( (void) );
void	free_note args( (NOTE_DATA *note) );
#undef ND

#define CHA CHALLENGE_DATA
CHA	*new_challenge args( (void) );
void	free_challenge args( (CHA *note) );
#undef CHA

#define BD BAN_DATA
BD	*new_ban args( (void) );
void	free_ban args( (BAN_DATA *ban) );
#undef BD

#define DD DESCRIPTOR_DATA
DD	*new_descriptor args( (void) );
void	free_descriptor args( (DESCRIPTOR_DATA *d) );
#undef DD

#define ED EXTRA_DESCR_DATA
ED	*new_extra_descr args( (void) );
void	free_extra_descr args( (EXTRA_DESCR_DATA *ed) );
#undef ED

#define TD TRAP_DATA
TD	*new_trap args( (void) );
void	free_trap args( (TRAP_DATA *tr) );
#undef TD

#define TID TRAP_INDEX_DATA
TID	*new_trap_index args( (void) );
void	free_trap_index args( (TRAP_INDEX_DATA *tr) );
#undef TID

#define CID CABAL_INDEX_DATA
CID	*new_cabal_index args( (void) );
void	free_cabal_index args( (CID *ca) );
#undef CID

#define HD HELP_DATA
HD	*new_help_index args( (void) );
void	free_help_index args( (HD *hp) );
#undef HD

#define AD AFFECT_DATA
AD	*new_affect args( (void) );
void	free_affect args( (AFFECT_DATA *af) );
#undef AD

#define OSD OBJ_SPELL_DATA
OSD	*new_obj_spell args( (void) );
void	free_obj_spell args( (OBJ_SPELL_DATA *sp) );
#undef OSD

#define SD SKILL_DATA
SD	*new_skill args( (void) );
void	free_skill args( (SKILL_DATA *sk) );
#undef SD

#define OD OBJ_DATA
OD	*new_obj args( (void) );
void	free_obj args( (OBJ_DATA *obj) );
#undef OD

#define CD CHAR_DATA
#define PD PC_DATA
CD	*new_char args( (void) );
void	free_char args( (CHAR_DATA *ch) );
PD	*new_pcdata args( (void) );
void	free_pcdata args( (PC_DATA *pcdata) );
#undef PD
#undef CD

/* Not sure what is a good value for MAX_FREE                                *
 * If a dup fails str_dup will not defrag unless the number                  *
 * of numFree >= MAX_FREE. numFree is NOT the current number of free blocks, */
#define   MAX_FREE     512


QUEST_DATA *new_quest();
void free_quest(QUEST_DATA *quest);

/* buffer procedures */
BUFFER	*new_buf args( (void) );
BUFFER  *new_buf_size args( (int size) );
void	free_buf args( (BUFFER *buffer) );
bool	add_buf args( (BUFFER *buffer, char *string) );
void	clear_buf args( (BUFFER *buffer) );
char	*buf_string args( (BUFFER *buffer) );
CMEMBER_DATA* new_cmember( void );
void	free_cmember( CMEMBER_DATA* cm );


CABAL_DATA	*new_cabal args( (void) );
void		free_cabal args( (CABAL_DATA *ca) );
