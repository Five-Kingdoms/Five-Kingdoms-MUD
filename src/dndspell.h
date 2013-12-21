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

#ifndef __DNDSPELL_H__
#define __DNDSPELL_H__

/* INTERFACE FOR DNDSPELL.H */


//DEFINES
#define	DNDS_MAXLEVEL	8	//Maximum of spell groups to remember into
#define DNDS_MAXREMEM	12	//Maximum of spells that can be remembered
#define	DNDS_TEMPLATES	8	//Maxi set of templates

#define DNDS_MEMTICK	3	//value gained per vtick
#define DNDS_STARTTICKS	15	//Number of ticks before any memorization start
#define DNDS_FULLTICKS	(120 + DNDS_STARTTICKS) //number of violence ticks for full memory

#define ADD_MEMTICK_FALSE  0
#define ADD_MEMTICK_ADD    1
#define ADD_MEMTICK_FULL   2

typedef struct dndspell_s DNDS_DATA;
typedef struct dnds_header_s DNDS_HEADER;
typedef sh_int DNDS_MEMORY;

//MACROS
#define SET_DNDMEM( pc, sn, mem) ((pc)->dndmemory[sn] = (mem))
#define SET_DNDMEM_H(dndsh, spell, mem)	((dndsh)->memory[(dndsh)->spells[spell].sn] = (mem))
#define GET_DNDMEM( pc, sn) ((pc)->dndmemory[sn])
#define GET_DNDMEM_H(dndsh, spell) ((dndsh)->memory[(dndsh)->spells[spell].sn])


struct dndspell_s{
  int sn;	//spell memorized
  int level;	//current group this is in
  int set;	//count how many to memorize
  int mem;	//temp hold for spells memorized during dirty_template
};

struct dnds_header_s{
  DNDS_DATA spells[DNDS_MAXREMEM];	//spells in this group
  sh_int*   memory;		//memory spell pool by sn
  int cur_total;		//total memorized spells
  int set_total;		//total set spells
  int mem_ticks;		//used to accumulate amount of memorization
};



//Prototypes
void reset_dndspells(PC_DATA* pc );
void reset_memticks( PC_DATA* pc );
int add_memticks( PC_DATA* pc, int val, int class, int ch_level );
void dnds_memorize_update( CHAR_DATA* ch );
void fwrite_dnds_template(FILE* fp, DNDS_HEADER* dndsh, int template);
int fread_dnds( FILE* fp, DNDS_DATA* dndsd, int char_ver );
DNDS_DATA* add_dndspell(PC_DATA* pc, int sn, int level, int mem, int set);
bool dnds_psicheck( CHAR_DATA* ch );
bool useup_dnds( PC_DATA* pc, int sn );
DNDS_DATA* dnds_lookup( PC_DATA* pc, int sn, int level );
void forget_dndtemplates( PC_DATA* pc );
bool memorize_spells( DNDS_HEADER* dndsh, int gain );
void fwrite_dndmemory( FILE* fp, DNDS_MEMORY* memory);
#endif
