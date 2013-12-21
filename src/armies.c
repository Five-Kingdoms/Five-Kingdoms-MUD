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

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <malloc.h>
#include "merc.h"
#include "misc.h"
#include "olc.h"
#include "recycle.h"
#include "armies.h"
#include "interp.h"
#include "cabal.h"

/***************************************************************/
/*Following are army orientanted routines for Age of Mirlan  */
/*mud created by Virigoth circa Oct 2003.  Copyrighted for Forsaken*/
/*Lands mud Oct 01, 2003.  Do not use or copy without explicit */
/*permission of author. (Voytek Plawny aka Virigoth)	       */
/***************************************************************/
const struct state_table_s state_table [] = {
  /*	name		bit			       life	*/
    {  "idle",		AS_NONE,			0	},
    {  "garrison",	AS_GARR,			0	},
    {  "fortify",       AS_FORT,			0	},
    {  "patrol",        AS_PATR,			11	},
    //attack causes combat from attack spot to defender
    {  "attack",	AS_ATTA,			0	},
    {  "move",		AS_MOVE,			5	},
    {  "return",	AS_RETU,			5	},
    {  "leaving",	AS_LEAV,			0	},
    //Shadow waits untill no attackers in room
    {  "circling",	AS_SHAD,			11	},
    {  "fighting",	AS_FIGH,			0	},
    {  "disbanding",	AS_DISB,			0	},
    //Defend causes combat from defender spot to attacker
    {  "defending",	AS_DEFE,			0	},
    //Circle waits untill defender spot open in room
    {  "circling",	AS_CIRC,			11	},
    {  "besieged",	AS_BESE,			0	},
    {  NULL,            0,				0	}
};

extern char *fix_string( const char *str );
extern int top_area;

static ARMY_INDEX_DATA army_index_list;
static int top_army_index;

static ARMY_DATA* army_list[ARMY_HASH_SIZE];
static int top_army;

static int	free_army_ids[MAX_ARMY_ID];
static int	last_army_id = 0;


/* PROTOTYPES */
void show_armies( CHAR_DATA* ch, CABAL_DATA* pc, char* name );
void show_area_armies(CHAR_DATA* ch,AREA_DATA* area, CABAL_DATA* pCabal);
int* gen_vars( int v0, int v1, int v2 );
int set_army_path( ARMY_DATA* pa, int src_vnum, int dest_vnum );
int count_attackers( ROOM_INDEX_DATA* room );
int count_defenders( ROOM_INDEX_DATA* room, bool fCountBastion );
int get_army_support( ARMY_DATA* pa, AREA_DATA* area );
void army_ai( ARMY_DATA* pa );

TOWER_DATA* get_tower_data( CABAL_DATA* pCabal, AREA_DATA* area );
void tower_from_area( CABAL_DATA* pCabal, AREA_DATA* area );
TOWER_DATA* tower_to_area( CABAL_DATA* pCabal, AREA_DATA* area, bool fSilent );
void reinforce_area( CABAL_DATA* pCabal, AREA_DATA* area );
bool unreinforce_area( CABAL_DATA* pCabal, AREA_DATA* area, bool fShort);

void show_report_q(REPORT_QUEUE* q, CHAR_DATA* ch );
void show_world_armies( CHAR_DATA* ch, CABAL_DATA* pc );

ARMY_DATA* get_bastion(ROOM_INDEX_DATA* room );
ARMY_DATA* get_attack_target( CABAL_DATA* pc, ROOM_INDEX_DATA* pRoom );
ARMY_DATA* get_defend_target( CABAL_DATA* pc, ROOM_INDEX_DATA* pRoom );

void stop_attacking( ARMY_DATA* pa );

//army actions
bool army_garrison( ARMY_DATA* pa);
bool army_return( ARMY_DATA* pa );
bool army_patrol( ARMY_DATA* pa );
bool army_fortify( ARMY_DATA* pa );
bool army_attack( ARMY_DATA* pa );
bool army_defend( ARMY_DATA* pa );
bool army_move( ARMY_DATA* pa );
bool army_leave( ARMY_DATA* pa );
bool army_intercept( ARMY_DATA* pa );

//checks for army actions
bool can_bastion(CHAR_DATA* ch,  ARMY_DATA* pa, ROOM_INDEX_DATA* in_room, CABAL_DATA* pc, bool fQuiet );
bool can_attack(CHAR_DATA* ch,  ARMY_DATA* pa, ROOM_INDEX_DATA* in_room, CABAL_DATA* pc, bool fQuiet );
bool can_defend(CHAR_DATA* ch,  ARMY_DATA* pa, ROOM_INDEX_DATA* in_room, CABAL_DATA* pc, bool fQuiet );
bool can_fortify(CHAR_DATA* ch,  ROOM_INDEX_DATA* in_room, CABAL_DATA* pc, bool fQuiet );
bool can_patrol(CHAR_DATA* ch,  ARMY_DATA* pa, ROOM_INDEX_DATA* in_room, CABAL_DATA* pc, bool fQuiet );
bool can_return(CHAR_DATA* ch,  ROOM_INDEX_DATA* in_room, ARMY_DATA* pa, bool fQuiet );



void order_army( ARMY_DATA* pa, sh_int state, int* vars );
/* NEW */

//PRIVATE FUNCTIONS
/* gets the next free army id */
int get_army_id(){
  static int top_army_id;
  int id = 0;

  /* check if there are any unused ones waiting */
  if (last_army_id > 0){
    id = free_army_ids[--last_army_id];
  }
  else{
    id = ++top_army_id;
  }
  return id;
}

/* records an army id to be reused */
void free_army_id( int id ){
  if (last_army_id < MAX_ARMY_ID)
    free_army_ids[last_army_id++] = id;
}

static ARMY_DATA* army_free;
ARMY_DATA* new_army(){
  ARMY_DATA *pa;

  if (army_free == NULL){
    pa = alloc_mem( sizeof( ARMY_DATA ));
  }
  else{
    pa = army_free;
    army_free = army_free->next;
  }
  memset(pa, 0, sizeof(*pa));

  //set id
  pa->ID	= get_army_id();
  //set commander to default
  pa->commander = str_dup("none");
  pa->next = NULL;

  return pa;
}

void free_army( ARMY_DATA* pa ){

  //free path if any
  if (pa->path)
    clean_path_queue( pa->path );
  //free id
  free_army_id( pa->ID );

  //Free commander name
  if (!IS_NULLSTR(pa->commander))
    free_string( pa->commander );
  pa->attacking = NULL;

  pa->next = army_free;
  army_free = pa;
}

void add_army( ARMY_DATA* pa ){
  word hash = ARMY_HASHVAL(pa);

  /* add army onto the hash index for quick lookup */
  pa->next = army_list[hash];
  army_list[hash] = pa;

  top_army++;

}

void rem_army( ARMY_DATA* pa ){
  word hash = ARMY_HASHVAL(pa);
  ARMY_DATA* prev = army_list[hash];

  if (prev == NULL){
    bug("armies.c>rem_army : Cannot remove army as non exist.", 0);
    return;
  }
  else if (army_list[hash] == pa ){
    army_list[hash] = pa->next;
    free_army( pa );
    top_army--;
    return;
  }

  for (; prev->next; prev = prev->next){
    if (prev->next == pa){
      prev->next = pa->next;
      free_army( pa );
      top_army--;
      return;
    }
  }
  bug("armies.c>rem_army : Cannot remove army as non found.", 0);
  return;
}

ARMY_INDEX_DATA* new_army_index(){
  ARMY_INDEX_DATA *pai = malloc( sizeof( *pai ));
  memset( pai, 0, sizeof( ARMY_INDEX_DATA ));
  return pai;
}

void free_army_index( ARMY_INDEX_DATA* pai ){

  if (!IS_NULLSTR(pai->noun)){
    free_string( pai->noun );
  }
  if (!IS_NULLSTR(pai->short_desc)){
    free_string( pai->short_desc );
  }
  if (!IS_NULLSTR(pai->long_desc)){
    free_string( pai->long_desc );
  }
  if (!IS_NULLSTR(pai->desc)){
    free_string( pai->desc );
  }
  free( pai );
}

void add_army_index( ARMY_INDEX_DATA* pai ){

  pai->next = army_index_list.next;
  army_index_list.next = pai;
  top_army_index++;
}

void rem_army_index( ARMY_INDEX_DATA* pai ){
  ARMY_INDEX_DATA* prev = &army_index_list;

  if (prev->next == NULL){
    bug("armies.c>rem_army_index : Cannot remove index as non exist.", 0);
    return;
  }
  for (; prev->next; prev = prev->next){
    if (prev->next == pai){
      prev->next = pai->next;
      free_army_index( pai );
      top_army_index--;
      return;
    }
  }
  bug("armies.c>rem_army_index : Cannot remove index as non found.", 0);
  return;
}

void fwrite_army_index( FILE* fp, ARMY_INDEX_DATA* pai ){
  char buf[MIL];

  fprintf( fp, "#%d\n", pai->vnum );
  fprintf( fp, "%s\n", fwrite_flag(pai->type, buf) );
  fprintf( fp, "%d\n", pai->cost );
  fprintf( fp, "%d\n", pai->support );
  fprintf( fp, "%s\n", fwrite_flag(pai->army_flags, buf) );
  fprintf( fp, "%d %d %d\n",
	   pai->off_dice[DICE_NUMBER],
	   pai->off_dice[DICE_TYPE],
	   pai->off_dice[DICE_BONUS]);
  fprintf( fp, "%d %d %d\n",
	   pai->hit_dice[DICE_NUMBER],
	   pai->hit_dice[DICE_TYPE],
	   pai->hit_dice[DICE_BONUS]);
  fprintf( fp, "%d %d %d\n",
	   pai->arm_dice[DICE_NUMBER],
	   pai->arm_dice[DICE_TYPE],
	   pai->arm_dice[DICE_BONUS]);
  fprintf( fp, "%s~\n", fix_string(pai->noun) );
  fprintf( fp, "%s~\n", fix_string(pai->short_desc) );
  fprintf( fp, "%s~\n", fix_string(pai->long_desc) );
  fprintf( fp, "%s~\n", fix_string(pai->desc) );
}



bool fread_army_index( FILE* fp, ARMY_INDEX_DATA* pai, AREA_DATA* pArea, int vnum ){

  pai->vnum			= vnum;
  pai->area			= pArea;
  pai->type			= fread_flag( fp );
  pai->cost			= fread_number( fp );
  pai->support			= fread_number( fp );
  pai->army_flags		= fread_flag( fp );

  pai->off_dice[DICE_NUMBER]	= fread_number( fp );
  pai->off_dice[DICE_TYPE]	= fread_number( fp );
  pai->off_dice[DICE_BONUS]	= fread_number( fp );

  pai->hit_dice[DICE_NUMBER]	= fread_number( fp );
  pai->hit_dice[DICE_TYPE]	= fread_number( fp );
  pai->hit_dice[DICE_BONUS]	= fread_number( fp );

  pai->arm_dice[DICE_NUMBER]	= fread_number( fp );
  pai->arm_dice[DICE_TYPE]	= fread_number( fp );
  pai->arm_dice[DICE_BONUS]	= fread_number( fp );

  pai->noun			= fread_string( fp );
  pai->short_desc		= fread_string( fp );
  pai->long_desc		= fread_string( fp );
  pai->desc			= fread_string( fp );

  if (get_army_index( vnum ) != NULL){
    bug("armies.c>fread_army_index: vnum %d duplicated.", vnum );
    return FALSE;
  }
  else
    return TRUE;
}

//sets armie's commander to given string
void set_army_commander( ARMY_DATA* pa, char* name, sh_int rank ){
  if (!IS_NULLSTR(pa->commander))
    free_string( pa->commander);
  pa->commander = str_dup( name );
  if (rank >= 0)
    pa->command_rank = rank;
}

//gets finds an army in the world with given id or NULL
ARMY_DATA* get_army_world( word ID ){
  ARMY_DATA* pa;
  word hash = ID_HASHVAL( ID );

  for (pa = army_list[hash]; pa; pa = pa->next){
    if (pa->ID == ID)
      return pa;
  }

  return NULL;
}

//takes army out of slot
void army_from_slot( ARMY_DATA* pa ){
  int slot = pa->in_slot;
  if (pa->in_room == NULL){
    bug("armies.c>army_from_slot: Army not in room. (%d)", pa->ID );
    return;
  }
  if (pa->in_slot < 0){
    bug("armies.c>army_from_slot: Army not in slot. (%d)", pa->ID );
    return;
  }
  /* decrease support */
  SUPPORT_GAIN(pa->pCabal, ECO_INCOME, -get_army_support( pa, pa->in_room->area) );

  if (pa->pIndexData->type == ARMY_TYPE_BASTION)
    tower_from_area(pa->pCabal, pa->in_room->area);

  pa->in_room->room_armies.armies[pa->in_slot] = NULL;

  if (slot == INROOM_BASTION)
    pa->in_room->room_armies.bastions--;
  else if (slot < INROOM_ATTACKERS)
    pa->in_room->room_armies.defenders--;
  else
    pa->in_room->room_armies.attackers--;
  pa->in_room->room_armies.count--;

  pa->in_slot = -1;
}

//puts an army into given slot in given room
void army_to_slot( ARMY_DATA* pa, int slot, bool fSilent ){
  if (pa->in_room == NULL){
    bug("armies.c>army_to_slot: Army not in room. (%d)", pa->ID );
    return;
  }

  if (pa->in_slot >= 0)
    army_from_slot( pa );

  if (pa->in_room->room_armies.armies[slot]){
    bug("armies.c>army_to_slot: Tried to place army in non empty slot.", 0);
    return;
  }

  /* increase support for this army */
  SUPPORT_GAIN(pa->pCabal, ECO_INCOME, get_army_support( pa, pa->in_room->area) );

  if (pa->pIndexData->type == ARMY_TYPE_BASTION)
    tower_to_area(pa->pCabal, pa->in_room->area, fSilent);

  pa->in_room->room_armies.armies[slot] = pa;
  if (slot == INROOM_BASTION)
    pa->in_room->room_armies.bastions++;
  else if (slot < INROOM_ATTACKERS)
    pa->in_room->room_armies.defenders++;
  else
    pa->in_room->room_armies.attackers++;
  pa->in_room->room_armies.count++;

  pa->in_slot = slot;
}


//moves an army out of a room
void army_from_room( ARMY_DATA* pa ){
  int i;
  ARMY_DATA* pAr;

  if (pa->in_room == NULL){
    bug("armies.c>army_from_room: army (%u) not in room.",  pa->ID);
    return;
  }

  stop_attacking( pa );

  if ( (i = pa->in_slot) >= 0)
    army_from_slot( pa );

  /* if this was bastion, reset armor on all defenders */
  /* end remove tower from area */
  if (i == INROOM_BASTION){
    for (i = INROOM_DEFENDERS; i < INROOM_ATTACKERS; i++){
      pAr = pa->in_room->room_armies.armies[i];
      if (pAr != NULL)
	pAr->armor = pAr->max_armor;
    }
  }
  pa->in_room = NULL;
}

//moves the army to a room
void army_to_room( ARMY_DATA* pa, ROOM_INDEX_DATA* pRoom ){
  if (pa->in_room)
    army_from_room( pa );
  pa->in_room = pRoom;
}




//creates a single ready to use army based on index
ARMY_DATA* create_army( ARMY_INDEX_DATA* pIndex, CABAL_DATA* pCabal ){
  ARMY_DATA* pa;

  if (pIndex == NULL)
    return NULL;

  //new army automaticly sets id on the army
  pa = new_army();

  pa->pIndexData		= pIndex;
  pa->pCabal			= pCabal;
  pa->off_dice[DICE_NUMBER]	= pIndex->off_dice[DICE_NUMBER];
  pa->off_dice[DICE_TYPE]	= pIndex->off_dice[DICE_TYPE];
  pa->off_dice[DICE_BONUS]	= pIndex->off_dice[DICE_BONUS];
  pa->max_hit			= dice( pIndex->hit_dice[DICE_NUMBER], pIndex->hit_dice[DICE_TYPE]) + pIndex->hit_dice[DICE_BONUS];
  pa->max_armor			= dice( pIndex->arm_dice[DICE_NUMBER], pIndex->arm_dice[DICE_TYPE]) + pIndex->arm_dice[DICE_BONUS];

  //allows for easy balancing
  pa->max_hit = ARMY_DEFENSE_MULT * pa->max_hit / 100;

  pa->hit			= pa->max_hit;
  pa->armor			= pa->max_armor;
  pa->army_flags		= pIndex->army_flags;
  pa->noun			= str_dup( pIndex->noun );
  pa->short_desc		= str_dup( pIndex->short_desc );
  pa->long_desc			= str_dup( pIndex->long_desc );
  pa->desc			= str_dup( pIndex->desc );

  /* some safeties */
  pa->hit = UMAX(1, pa->hit );
  pa->max_hit = UMAX(1, pa->max_hit );
  pa->in_slot = -1; //no slot

  //set commander to cabal name initial
  if (pCabal)
    set_army_commander( pa, pCabal->name, 0 );
  else
    set_army_commander( pa, "Unknown", 0 );

  //if this army vnum is cabal's upgrade vnum set elite flag
  if (pCabal){
    if (pIndex->type == ARMY_TYPE_UNIT && pIndex->vnum == pCabal->pIndexData->army_upgrade)
      SET_BIT(pa->army_flags, ARMY_FLAG_ELITE);
    else if (pIndex->type == ARMY_TYPE_BASTION && pIndex->vnum == pCabal->pIndexData->tower_upgrade)
      SET_BIT(pa->army_flags, ARMY_FLAG_ELITE);
  }
  //add army to the list of armies in the game
  add_army( pa );
  return pa;
}

//extracts a single army out of the game
void extract_army( ARMY_DATA* pa ){

  //army_from_room does stop attackin as well, no sense doing it twice
  if (pa->in_room)
    army_from_room( pa );
  else
    stop_attacking( pa );
  //remove army from list of armies in the game, this will also free it
  //as well as its id
  rem_army( pa );
}





//begins a recruitment of an army fElite TRUE to recruit elite unit.
void start_recruitment( CHAR_DATA* ch, bool fUpgrade, int num, int cost ){
  CABAL_DATA* pCab = get_parent( ch->pCabal );
  int* queue, *timer, start_dur;

//SAFETIES
  if (pCab == NULL || ch->pcdata == NULL || ch->pcdata->member == NULL)
    return;
  else
    num = URANGE(-1, num, 10);

  if (fUpgrade){
    start_dur = IS_CABAL(pCab, CABAL_SWARM) ? ARMY_DUR_SUPG : ARMY_DUR_UPG;
    queue = &ch->pcdata->member->armies[UPGQ];
    timer = &ch->pcdata->member->armies[TIMU];
  }
  else{
    start_dur = IS_CABAL(pCab, CABAL_SWARM) ? ARMY_DUR_SREC : ARMY_DUR_REC;
    queue = &ch->pcdata->member->armies[NORQ];
    timer = &ch->pcdata->member->armies[TIMN];
  }

  //if value == -1 we increase the queue by one
  if (num == -1)	*queue = UMIN(10, *queue + 1);
  else			*queue = num;

  if (num == 0){
    send_to_char("Training halted.\n\r", ch);
    *timer = -1;
  }
  else{
    sendf(ch, "%d units remaining to %s.\n\r",
	  *queue,
	  fUpgrade ? "train" : "conscript");
    if (*timer < 1)
      *timer = start_dur;
  }
}

/* sets state of a single army and when it will be re-examined */
/* INTERNAL, do not use */
void set_state( ARMY_DATA* pa, sh_int state, int lifetime, int* vars ){
  int i = 0;

  pa->state	= state;
  pa->lifetime	= lifetime;

  for (i = 0; i < ARMY_VARS; i++)
    pa->vars[i] = vars ? vars[i] : 0;
}

/* short AI function to allow cabals with no members to respond to attacks */
void army_cabal_ai( ARMY_DATA* pBas){
  CABAL_DATA* pCab;
  ARMY_DATA* pa;
  ROOM_INDEX_DATA* room = pBas->in_room;
  char buf[MIL];
  int vnum, i;
  int armies;

  if (pBas == NULL || room == NULL)
    return;
  else if (pBas->cabal_ai_life > 0)
    return;
  else if ( (pCab = get_parent(pBas->pCabal)) == NULL)
    return;
  else if (pCab->armies_ready < 1 || pCab->present > 0)
    return;
  else if ( (mud_data.current_time - pCab->present_stamp) < DEFENSE_AIWAIT)
    return;

  //check if the room is being attacked and we have space to defend
  if (room->room_armies.bastions < 1
      || room->room_armies.defenders  > 1
      || room->room_armies.attackers < 1)
    return;
  else if (!can_defend( NULL, NULL, room, pCab, TRUE ))
    return;

  /* Conditions fulfilled:
     1) We have a bastion in room
     2) There is a defender spot open
     3) There are attackers in room
     4) Can defend the room
     5) We have armies to defend with
  */

  if (room->room_armies.defenders < 1)
    armies = 2;
  else
    armies = 1;

  //spawn an army and send it to hold the room
  //if we have more then 5 recruits send an elite
  if (pCab->armies_ready - armies > 5){
    vnum = pCab->pIndexData->army_upgrade;
    pCab->armies_ready -= 1 * armies;
  }
  else{
    vnum = pCab->pIndexData->army;
    pCab->armies_ready -= armies;
  }

  for (i = 0; i < armies; i++){
    if ( (pa = create_army(get_army_index( vnum ), pCab)) == NULL){
      return;
    }

    /* set commander and order */
    set_army_commander( pa, pCab->name, RANK_ELDER - 1 );
    order_army( pa, AS_DEFE, gen_vars( room->vnum, 0, 0));
    army_ai( pa );

    //Bastion lifetime marks when we should send armies again
    //pa->lifetime is movement length + 1 turn to enter, + 1 to make new decision
    pBas->cabal_ai_life = 2 + pa->lifetime;

    sprintf( buf, "%s %s confirmed.",
	     capitalize(state_table[pa->order].name),
	     room->name );
    army_report( pa, buf, REPORT_NEUT, FALSE );
  }

}

/* Main army AI function, switches states based on current state */
void army_ai( ARMY_DATA* pa ){
  int lifetime = 0, to_vnum, from_vnum;
  char buf[MIL];

  switch (pa->state){
  default:
  case AS_RETU:
    return;
// DISBANDING: does nothing, hp regen is negative army will disband when dies
  case AS_DISB:
    break;
// IDLE: reverts to original order if any
  case AS_NONE:
    set_state(pa, pa->order, lifetime, pa->ovars );
    break;
// FIGHTIN/BESIEGEDG: if not in combat, reverts to old orders
  case AS_BESE:
  case AS_FIGH:
    if (pa->attacking == NULL){
      //check if we defeated everything
      if (pa->in_slot < INROOM_ATTACKERS){
	if ( (pa->attacking = get_defend_target( pa->pCabal, pa->in_room )) == NULL){
	  sprintf(buf, "Standing down in %s.", pa->in_room->name);
	  army_report( pa, buf, REPORT_GOOD, FALSE );
	  //this is not needed as we cascade down to it anyway, but just in ase
	  set_state(pa, pa->order, 1, pa->ovars );
	}
	else
	  break;
      }
      /* attacker check (attackers pause if defenders dead) */
      else{
	ARMY_DATA* tar = get_attack_target(pa->pCabal, pa->in_room);
	if (tar == NULL){
	  sprintf(buf, "Victorious in %s.", pa->in_room->name);
	  army_report( pa, buf, REPORT_GOOD, FALSE );
	  //this is not needed as we cascade down to it anyway, but just in ase
	  set_state(pa, pa->order, 1, pa->ovars );
	  break;
	}
	else if (tar->pIndexData->type != ARMY_TYPE_BASTION){
	  pa->attacking = tar;
	  break;
	}
      }
      set_state(pa, pa->order, 1, pa->ovars );
    }
    else if (is_friendly(pa->pCabal, pa->attacking->pCabal) != CABAL_ENEMY){
      stop_attacking( pa );
    }
    break;
// ATTACK <vnum>: attack defenders in room v0, move there if not there
  case AS_ATTA:
    /* check if we are in wrong room, if so move */
    if (pa->in_room == NULL || pa->in_room->vnum != pa->vars[0]){
      /* not in right room, move */
      lifetime = state_table[AS_LEAV].lifetime;
      set_state(pa, AS_LEAV, lifetime, gen_vars( pa->vars[0], 0, 0));
    }
    // check if we can attack something here
    else if (can_attack(NULL, pa, pa->in_room, pa->pCabal, TRUE)){
      break;
    }
    /* if nothing in room to attack, check if can patrol, or return */
    else{
      if (can_patrol(NULL, pa, pa->in_room, pa->pCabal, TRUE)){
	/* set current state to patrol, set over all order to return
	   so we return after patrol is over
	*/
	pa->order = AS_RETU;
	lifetime = state_table[AS_PATR].lifetime;
	set_state(pa, AS_PATR, 0, gen_vars( pa->vars[0], 0, 0));

	sprintf(buf, "Nothing to attack in %s.", pa->in_room->name);
	army_report( pa, buf, REPORT_NEUT, FALSE );
      }
      else{
	lifetime = state_table[AS_RETU].lifetime;
	set_state(pa, AS_RETU, 0, gen_vars( 0, 0, 0));
	sprintf(buf, "No enemies in %s.", pa->in_room->name);
	army_report( pa, buf, REPORT_NEUT, FALSE );
      }
    }
    break;
// DEFEND <vnum>: attack attackers in room v0, move there if not there
  case AS_DEFE:
    /* check if we are in wrong room, if so move */
    if (pa->in_room == NULL || pa->in_room->vnum != pa->vars[0]){
      /* not in right room, move */
      lifetime = state_table[AS_LEAV].lifetime;
      set_state(pa, AS_LEAV, lifetime, gen_vars( pa->vars[0], 0, 0));
    }
    // check if we can attack something here
    else if (can_defend(NULL, pa, pa->in_room, pa->pCabal, TRUE)){
      //reset post circle TRUE in v2
      pa->vars[2] = 0;
      break;
    }
    /* if nothing in room to defend, check if can patrol, or shadow */
    else{
      //if there is no space for defense, but there are attackers circle
      //If we've already circled then vars2 is TRUE
      if (!pa->vars[2]
	  && (INROOM_ATTACKERS - 1 - count_defenders( pa->in_room, FALSE)) < 1
	  && get_defend_target( pa->pCabal, pa->in_room )){
	//we leave order as is, as we want to try to defend after shadowing
	lifetime = state_table[AS_CIRC].lifetime;
	//set v0 to updates we want to shadow for
	set_state(pa, AS_CIRC, -1, gen_vars( 0, 0, lifetime));
	sprintf(buf, "Circling. Lacking space to defend.");
	army_report( pa, buf, REPORT_NEUT, TRUE );
      }
      else if (can_patrol(NULL, pa, pa->in_room, pa->pCabal, TRUE)){
	/* set current state to patrol, set over all order to return
	   so we return after patrol is over
	*/
	pa->order = AS_RETU;
	lifetime = state_table[AS_PATR].lifetime;
	set_state(pa, AS_PATR, 0, gen_vars( pa->vars[0], 0, 0));
	sprintf(buf, "Defend. No enemies in area.");
	army_report( pa, buf, REPORT_NEUT, FALSE );
      }
      //return
      else{
	lifetime = state_table[AS_RETU].lifetime;
	set_state(pa, AS_RETU, 0, gen_vars( 0, 0, 0));
	sprintf(buf, "Returning. Cannot defend area.");
	army_report( pa, buf, REPORT_NEUT, TRUE );
      }
    }
    break;
// GARRISON <vnum>: garrison room v0, move there if not there
  case AS_GARR:
    /* check if we are in wrong room, if so move */
    if (pa->in_room == NULL || pa->in_room->vnum != pa->vars[0]){
      /* not in right room, move */
      lifetime = state_table[AS_LEAV].lifetime;
      set_state(pa, AS_LEAV, lifetime, gen_vars( pa->vars[0], 0, 0));
    }
    /* if we can bastion, check if there are attackers in room, if so
       start shadowing in order to reinforce the garrison once attackers
       leave.  If we've already shadowed then var2 is TRUE
    */
    else if (pa->vars[2] == 0 && count_attackers(pa->in_room) > 0){
      lifetime = state_table[AS_SHAD].lifetime;
      set_state(pa, AS_SHAD, 0, gen_vars( 0, 0, lifetime));
      sprintf(buf, "Circling. Can't garrison due to battle.");
      army_report( pa, buf, REPORT_BAD, TRUE );
    }
    /* if not in wrong room, check if we can bastion, if not return */
    else if (count_attackers(pa->in_room) > 0
	     || !can_bastion( NULL, pa, pa->in_room, pa->pCabal, TRUE )){
      lifetime = state_table[AS_RETU].lifetime;
      set_state(pa, AS_RETU, 0, gen_vars( 0, 0, 0));
      sprintf(buf, "Can't garrison %s.", pa->in_room->area->name);
      army_report( pa, buf, REPORT_NEUT, TRUE );
    }
    break;
// FORTIFY <vnum>: upgrade bastion in room v0, move there if not there
  case AS_FORT:
    /* check if we are in wrong room, if so move */
    if (pa->in_room == NULL || pa->in_room->vnum != pa->vars[0]){
      /* not in right room, move */
      lifetime = state_table[AS_LEAV].lifetime;
      set_state(pa, AS_LEAV, lifetime, gen_vars( pa->vars[0], 0, 0));
    }
    /* we are in correct room, check if fortifications not started (v2 == 0) */
    else if (pa->vars[2] == 0){
      //can we start fortifications?
      if (!can_fortify( NULL, pa->in_room, pa->pCabal, TRUE )){
	lifetime = state_table[AS_RETU].lifetime;
	set_state(pa, AS_RETU, 0, gen_vars( 0, 0, 0));
	sprintf(buf, "Unable to fortify %s.", pa->in_room->name);
	army_report( pa, buf, REPORT_NEUT, TRUE );
      }
      else{
	sprintf(buf, "Fortifying in %s.", pa->in_room->name);
	army_report( pa, buf, REPORT_GOOD, TRUE );
	pa->vars[2] = ARMY_DUR_FORTIFY;
      }
    }
    /* we are in process of foritfying, check for interruptions */
    else{
      /* check for not being able to build a garrison anymore */
      if (pa->vars[2] && !can_fortify( NULL, pa->in_room, pa->pCabal, TRUE )){
	lifetime = state_table[AS_RETU].lifetime;
	set_state(pa, AS_RETU, 0, gen_vars( 0, 0, 0));
	sprintf(buf, "Unable to fortify %s.", pa->in_room->name);
	army_report( pa, buf, REPORT_BAD, TRUE );
	pa->vars[2] = 0;
      }
      /* if we can fortify, check if there are attackers in room, if so
	 start shadowing in order to fortify once attackers are gone
	 leave.  If 'we already circled then vars[2] is TRUE
      */
      else if (count_attackers(pa->in_room) > 0){
	lifetime = state_table[AS_SHAD].lifetime;
	set_state(pa, AS_SHAD, 0, gen_vars( 0, 0, lifetime));
	sprintf(buf, "Circling.  Fortification interrupted by battle.");
	army_report( pa, buf, REPORT_BAD, TRUE );
      }
    }
    break;
// LEAVE <for-room>: leave room before moving to another room
  case AS_LEAV:
    //set the path before leaving
    if (pa->vars[2] == 0)
      from_vnum = pa->pCabal->anchor->vnum;
    else
      from_vnum = pa->vars[2];
    to_vnum = pa->vars[0];

    /* try to get the path to the destination */
    if ( (lifetime = set_army_path( pa, from_vnum, to_vnum)) < 0){
      lifetime = state_table[AS_RETU].lifetime;
      set_state(pa, AS_RETU, 0, gen_vars( 0, 0, 0));
      sprintf(buf, "Returning. Could not reach destination.");
      army_report( pa, buf, REPORT_NEUT, TRUE );
      break;
    }
    //lifetime set by set_army_path above
    set_state(pa, AS_MOVE, lifetime, gen_vars( pa->vars[0], 0, 0));
    break;
// MOVE <roomvnum>: move to v0
  case AS_MOVE:
    /* are we in the destination room? */
    if (pa->in_room && pa->in_room->vnum == pa->vars[0]){
      //reverts to original order
      set_state(pa, pa->order, 0, pa->ovars );
      //run ai once
      if (pa->state != AS_MOVE)
	army_ai( pa );
    }
    break;
//PATROL: check if in room, move if not.  Otherwise try to patrol
  case AS_PATR:
    /* check if we are in wrong room, if so move */
    if (pa->in_room == NULL || pa->in_room->vnum != pa->vars[0]){
      /* not in right room, move */
      lifetime = state_table[AS_LEAV].lifetime;
      set_state(pa, AS_LEAV, lifetime, gen_vars( pa->vars[0], 0, 0));
    }
    /* if we are in a room and in a slot (already patroling) check if
       we should return
    */
    else if (pa->in_slot >= 0){
      /* if our order was something else then patrol execute it now */
      if (pa->order != AS_PATR){
	set_state(pa, pa->order, 0, pa->ovars);
	sprintf(buf, "Commencing %s.  Finished patroling.", state_table[pa->order].name);
	army_report( pa, buf, REPORT_NEUT, TRUE );
      }
      else{
	//no reason to return, reset lifetime untill next check
	pa->lifetime = state_table[pa->state].lifetime;
      }
    }
    /* if right room, but not in slot check if we can patrol, if not return */
    else if (!can_patrol( NULL, pa, pa->in_room, pa->pCabal, TRUE )){
      lifetime = state_table[AS_RETU].lifetime;
      set_state(pa, AS_RETU, 0, gen_vars( 0, 0, 0));
      sprintf(buf, "Could not patrol %s.", pa->in_room->name);
      army_report( pa, buf, REPORT_NEUT, TRUE );
    }
    break;
//SHADOW: Stay around the room untill v2 runs out
//	  or there are no more enemies.  Then revert to order.
  case AS_SHAD:
    if (pa->in_room == NULL
	|| --pa->vars[2] < 1
	|| count_attackers(pa->in_room) < 1){
      //revert to original order but v2 is now TRUE
      set_state(pa, pa->order, 1, gen_vars(pa->ovars[0], pa->ovars[1], 1) );
    }
    break;
//CIRCLE: Stay around the room untill v2 runs out
//	  or there are empty defender slots.  Then revert to order.
  case AS_CIRC:
    if (pa->in_room == NULL
	|| --pa->vars[2] < 1
	|| (INROOM_ATTACKERS - 1 - count_defenders(pa->in_room, FALSE) > 0)){
      //revert to original order but v2 is now TRUE
      set_state(pa, pa->order, 1, gen_vars(pa->ovars[0], pa->ovars[1], 1) );
    }
    break;
  }
}

//performs actions when state's lifetime reaches 0
//This function should NEVER switch state
void army_action( ARMY_DATA* pa ){
  int lifetime = 0;

  switch (pa->state){
  default:
    return;
//FIGHT/BESIEGED:
  case AS_FIGH:
  case AS_BESE:
    break;
//LEAVE: If in room, exit it before moving to v0
  case AS_LEAV:
    army_leave( pa );
    pa->lifetime = lifetime;
    break;
//MOVE: Move along the armie's path to destination
  case AS_MOVE:
    if (pa->in_room){
      bug("armies.c>army_action: AS_MOVE before army left.", 0);
      army_from_room( pa );
    }
    else if (!army_move( pa )){
      bug("armies.c>army_action: AS_MOVE with invalid path.", 0);
      extract_army( pa );
      return;
    }
    //if by some chance an army got lost and keeps trying to move to NULL room
    else if (pa->in_room == NULL){
      bug("armies.c>army_action: AS_MOVE with NULL destination. Army killed", 0);
      extract_army( pa );
    }
    else
      pa->lifetime = lifetime;
    break;
//SHADOW:  leave slot if in one
  case AS_SHAD:
    if (pa->in_slot >= 0)
      army_from_slot( pa );
    break;
//GARRISON: Garrison/Build a bastion in room the army is in
  case AS_GARR:
    if (pa->in_room == NULL){
      bug("armies.c>army_action: AS_GARR while not in room.", 0);
      extract_army( pa );
      return;
    }
    //army is extracted inside army_garrison
    army_garrison( pa);
    break;
//FORTIFY: upgrade a bastion in room the army is in
  case AS_FORT:
    if (pa->in_room == NULL){
      bug("armies.c>army_action: AS_FORT while not in room.", 0);
      extract_army( pa );
      return;
    }
    /* build fortifications when v2 reaches 0 */
    else if (pa->vars[2] > 0 && --pa->vars[2] == 0){
      //army is extracted inside army_fortify
      army_fortify( pa);
    }
    break;
//PATROL: Patrol in room the army is in
  case AS_PATR:
    if (pa->in_room == NULL){
      bug("armies.c>army_action: AS_PATR while not in room.", 0);
      extract_army( pa );
      return;
    }
    army_patrol( pa);
    /* set lifetime before next decision if should continue patrol */
    pa->lifetime = state_table[pa->state].lifetime;
    break;
//ATTACK
  case AS_ATTA:
    if (pa->in_room == NULL){
      bug("armies.c>army_action: AS_ATTA while not in room.", 0);
      extract_army( pa );
      return;
    }
    if (army_attack( pa))
      /* set infinite lifetime, army will switch back after attack is done */
      pa->lifetime = -1;
    else
      pa->lifetime = 0;
    break;
//DEFEND
  case AS_DEFE:
    if (pa->in_room == NULL){
      bug("armies.c>army_action: AS_DEFE while not in room.", 0);
      extract_army( pa );
      return;
    }
    if (army_defend( pa))
      /* set infinite lifetime, army will switch back after attack is done */
      pa->lifetime = -1;
    else
      pa->lifetime = 0;
    break;
//RETURN: put army back in barracks
  case AS_RETU:
    //If we are in room leave it, then start returning
    if (pa->in_room){
      army_leave( pa );
      pa->lifetime = state_table[AS_RETU].lifetime;
      break;
    }
    //army extracted inside
    army_return( pa );
    break;
  }
}

/* gives an army an order.  Use this to give an army its general order */
void order_army( ARMY_DATA* pa, sh_int state, int* vars ){
  int i = 0;

  pa->order	= state;
  pa->state	= state;
  pa->lifetime	= state_table[state].lifetime;

  for (i = 0; i < ARMY_VARS; i++){
    pa->vars[i] = vars ? vars[i] : 0;
    pa->ovars[i] = pa->vars[i];
  }

  army_ai( pa );
  army_action( pa );
}

bool can_bastion(CHAR_DATA* ch,  ARMY_DATA* pa, ROOM_INDEX_DATA* in_room, CABAL_DATA* pc, bool fQuiet ){
  int door, def = 0, i;
  EXIT_DATA* pExit;
  ARMY_INDEX_DATA* pai;
  bool fDoor = TRUE;

  /* safety */
  if (ch == NULL)
    fQuiet = TRUE;

  /* check for targetting virtual rooms */
  if (IS_VIRVNUM(in_room->vnum)){
    if (!fQuiet)
      send_to_char("You cannot garrison virtual rooms.", ch );
    return FALSE;
  }
  else if (!can_reinforce( pc, in_room->area)){
    if (!fQuiet)
      send_to_char("You cannot reinforce this area. (\"help reinforce\")\n\r", ch );
    return FALSE;
  }
  else if ( (pai = get_army_index(pc->pIndexData->tower)) == NULL){
    if (!fQuiet)
      send_to_char("Your cabal has no fortificantion's to build.\n\r", ch );
    return FALSE;
  }
  /* check if we can make a garrison here */
  else if (in_room->room_armies.armies[INROOM_BASTION] == NULL
	   && in_room->area->bastion_current >= in_room->area->bastion_max){
    if (!fQuiet)
      send_to_char("This area cannot support any more bastions.\n\r", ch );
    return FALSE;
    /* check money */
    if (GET_CAB_CP(pc) < pai->cost){
      if (!fQuiet){
	sendf(ch, "[%s] coffers must hold at least %d %ss in order to construct a bastion.\n\r",
	      pc->who_name,
	      pai->cost,
	      pc->currency);
      }
      return FALSE;
    }
  }
/* cannot take over cabal areas */
  else if (IS_AREA(in_room->area, AREA_CABAL)){
    if (!fQuiet)
      send_to_char("You cannot take over a Cabal!\n\r", ch );
    return  FALSE;
  }
/* check for pacts */
  else if ( in_room->area->pCabal
	    && is_friendly( pc, in_room->area->pCabal) == CABAL_NEUTRAL){
    if (!fQuiet)
      sendf( ch, "You will have to declare a Vendetta against %s before taking armed action against them!\n\r",
	     in_room->area->pCabal->who_name);
    return FALSE;
  }
  if (IS_SET(in_room->area->area_flags, AREA_RESTRICTED)
      || IS_SET(in_room->room_flags, ROOM_SAFE)
      || IS_SET(in_room->room_flags, ROOM_PRIVATE)
      || IS_SET(in_room->room_flags, ROOM_SOLITARY)
      || IS_SET(in_room->room_flags, ROOM_NOWHERE)
      || IS_SET(in_room->room_flags, ROOM_DAMAGE)
      || IS_SET(in_room->room_flags2, ROOM_NO_MAGIC)
      || IS_SET(in_room->room_flags2, ROOM_JAILCELL)
      || IS_VIRVNUM(in_room->vnum)){
    if (!fQuiet)
      send_to_char("You cannot build a tower due to nature of this room.\n\r", ch );
    return FALSE;
  }
  /* check doors */
  for (door = 0; door < MAX_DOOR; door++){
    if ( (pExit = in_room->exit[door]) == NULL)
      continue;
    //exit must be two way in same dir.
    else if (pExit->to_room->exit[rev_dir[door]] == NULL){
      fDoor = FALSE;
      break;
    }
    //exit must be two way in same dir.
    else if (pExit->to_room->exit[rev_dir[door]]->to_room != in_room){
      fDoor = FALSE;
      break;
    }
  }
  if (!fDoor){
    if (!fQuiet)
      send_to_char("The room cannot have one way exits.\n\r", ch);
    return FALSE;
  }
  /* check how many defender spots are open */
  for (i = INROOM_DEFENDERS; i < INROOM_ATTACKERS; i++){
    if (in_room->room_armies.armies[i] == NULL)
      def++;
  }
  /* two cases
     1) Bastion is not there, we can garrison
     2) Bastion is there we can garrison if:
     a) bastion is friendly
     b) spots are open
  */
  if (in_room->room_armies.armies[INROOM_BASTION] != NULL){
    if (is_friendly( pc, in_room->room_armies.armies[INROOM_BASTION]->pCabal) != CABAL_FRIEND){
      if (!fQuiet)
	send_to_char("Unable to defend non-allied bastion.\n\r", ch);
      return FALSE;
    }
    else if (def < 1
	     && (pa == NULL
		 || pa->in_room != in_room
		 || pa->in_slot <= INROOM_BASTION
		 || pa->in_slot >= INROOM_ATTACKERS)
	     ){
      if (!fQuiet)
	send_to_char("Unable to garrison due to lack of space.\n\r", ch);
      return FALSE;
    }
  }

  /* all seems good */
  return TRUE;
}

//checks if we can fortify bastion in this room
bool can_fortify(CHAR_DATA* ch,  ROOM_INDEX_DATA* in_room, CABAL_DATA* pc, bool fQuiet ){

  /* safety */
  if (ch == NULL)
    fQuiet = TRUE;

  if (!can_reinforce( pc, in_room->area)){
    if (!fQuiet)
      send_to_char("You cannot reinforce this area. (\"help reinforce\")\n\r", ch );
    return FALSE;
  }
  else if (get_army_index(pc->pIndexData->tower_upgrade) == NULL){
    if (!fQuiet)
      send_to_char("Your cabal has no fortificantion's to build.\n\r", ch );
    return FALSE;
  }
  /* check if our bastion exists in this room */
  else if (in_room->room_armies.armies[INROOM_BASTION] == NULL){
    if (!fQuiet)
      send_to_char("There is no bastion there to fortify.\n\r", ch);
    return FALSE;
  }
  else if (!is_same_cabal(in_room->room_armies.armies[INROOM_BASTION]->pCabal, pc)){
    if (!fQuiet)
      send_to_char("You cannot fortify bastions that do not belong to you.\n\r", ch);
    return FALSE;
  }

  /* all seems good */
  return TRUE;
}

//checks if a patrol can arrive in the room
bool can_patrol(CHAR_DATA* ch,  ARMY_DATA* pa, ROOM_INDEX_DATA* in_room, CABAL_DATA* pc, bool fQuiet ){
  int def = 0, i;

  /* safety */
  if (ch == NULL)
    fQuiet = TRUE;

  /* check for targetting virtual rooms */
  if (IS_VIRVNUM(in_room->vnum)){
    if (!fQuiet)
      send_to_char("You cannot patrol virtual rooms.", ch );
    return FALSE;
  }
  else if (!can_reinforce( pc, in_room->area)){
    if (!fQuiet)
      send_to_char("You cannot reinforce this area. (\"help reinforce\")\n\r", ch );
    return FALSE;
  }
/* cannot take over cabal areas */
  else if (in_room->area->pCabal && in_room->area->pCabal->anchor
	   && in_room->area->pCabal->anchor->area == in_room->area){
    if (!fQuiet)
      send_to_char("You cannot patrol inside a Cabal!\n\r", ch );
    return  FALSE;
  }
/* check for pacts */
  else if ( in_room->area->pCabal
	    && is_friendly( pc, in_room->area->pCabal) == CABAL_NEUTRAL){
    if (!fQuiet)
      sendf( ch, "You will have to declare a Vendetta against %s before taking armed action against them!\n\r",
	     in_room->area->pCabal->who_name);
    return FALSE;
  }
  /* check if bastion is friendly */
  if (in_room->room_armies.armies[INROOM_BASTION] != NULL
      && is_friendly(pc, in_room->room_armies.armies[INROOM_BASTION]->pCabal) != CABAL_FRIEND){
    if (!fQuiet)
      send_to_char("Cannot patrol around unfriendly garrison.\n\r", ch);
    return FALSE;
  }
  /* check how many defender spots are open */
  for (i = INROOM_DEFENDERS; i < INROOM_ATTACKERS; i++){
    if (in_room->room_armies.armies[i] == NULL
	|| in_room->room_armies.armies[i] == pa)
      def++;
  }
  if (def < 1){
    if (!fQuiet)
      send_to_char("Unable to patrol due to lack of space.\n\r", ch);
    return FALSE;
  }
  /* all seems good */
  return TRUE;
}

//checks if an army can be order to another location
bool can_relocate(CHAR_DATA* ch, ARMY_DATA* pa, bool fQuiet ){
  /* safety */
  if (ch == NULL)
    fQuiet = TRUE;

  //is army in room
  if (pa->in_room == NULL){
    if (!fQuiet)
      send_to_char("This army is not stationed anywhere.\n\r", ch);
    return FALSE;
  }
  //is the army incombat?
  else if (pa->attacking){
    if (!fQuiet)
      send_to_char("Army cannot leave while engaged in battle.\n\r", ch);
    return FALSE;
  }
  //is army a bastion or garrison
  else if (pa->in_slot == INROOM_BASTION){
    if (!fQuiet){
      send_to_char("You cannot relocate bastions.\n\r", ch);
    }
    return FALSE;
  }
  //is this a garrison
  else if (IS_ARMY(pa, ARMY_FLAG_GARRISON)){
    if (!fQuiet){
      send_to_char("You cannot relocate garrisons.\n\r", ch);
    }
    return FALSE;
  }
  return TRUE;
}

//checks if an army can return to garrison
bool can_return(CHAR_DATA* ch,  ROOM_INDEX_DATA* in_room, ARMY_DATA* pa, bool fQuiet ){

  /* safety */
  if (ch == NULL)
    fQuiet = TRUE;

  //is army in room
  if (pa->in_room == NULL){
    if (!fQuiet)
      send_to_char("This army is not stationed anywhere.\n\r", ch);
    return FALSE;
  }
  //is the army incombat?
  else if (pa->attacking){
    if (!fQuiet)
      send_to_char("Army cannot leave while engaged in battle.\n\r", ch);
    return FALSE;
  }
  //is army a bastion with garrison?
  else if (pa->in_slot == INROOM_BASTION){
    ARMY_DATA* pa;
    int i;
    for (i = INROOM_DEFENDERS; i < INROOM_ATTACKERS; i++){
      if ( (pa = in_room->room_armies.armies[i]) == NULL)
	continue;
      else if (IS_ARMY(pa, ARMY_FLAG_GARRISON)){
	if (!fQuiet)
	  send_to_char("Cannot disband a bastion with garrisons.\n\r", ch);
	return FALSE;
      }
    }
  }
  return TRUE;
}

//checks if army can/has anything to attack in room
bool can_attack(CHAR_DATA* ch,  ARMY_DATA* pa, ROOM_INDEX_DATA* in_room, CABAL_DATA* pc, bool fQuiet ){
  int att = 0;

  /* safety */
  if (ch == NULL)
    fQuiet = TRUE;

  /* check for targetting virtual rooms */
  if (IS_VIRVNUM(in_room->vnum)){
    if (!fQuiet)
      send_to_char("You cannot attack virtual rooms.", ch );
    return FALSE;
  }
  else if (!can_reinforce( pc, in_room->area)){
    if (!fQuiet)
      send_to_char("You cannot reinforce this area. (\"help reinforce\")\n\r", ch );
    return FALSE;
  }
/* cannot take over cabal areas */
  else if (in_room->area->pCabal && in_room->area->pCabal->anchor
	   && in_room->area->pCabal->anchor->area == in_room->area){
    if (!fQuiet)
      send_to_char("You cannot attack inside a Cabal!\n\r", ch );
    return  FALSE;
  }
/* check for pacts */
  else if ( in_room->area->pCabal
	    && is_friendly( pc, in_room->area->pCabal) == CABAL_NEUTRAL){
    if (!fQuiet)
      sendf( ch, "You will have to declare a Vendetta against %s before taking armed action against them!\n\r",
	     in_room->area->pCabal->who_name);
    return FALSE;
  }
  /* check if any attacker slots open */
  if (pa == NULL ||
      pa->in_room == NULL || pa->in_room != in_room
      || pa->in_slot < 0 || pa->in_slot < INROOM_ATTACKERS){
    att = INROOM_MAXARMIES - INROOM_ATTACKERS - count_attackers( in_room);
    if (att < 1){
      if (!fQuiet)
	send_to_char("Unable to attack due to lack of space.\n\r", ch);
      return FALSE;
    }
  }
  /* check if we have possible targets */
  if (get_attack_target( pc, in_room) == NULL){
    if (!fQuiet)
      send_to_char("No valid attack targets detected.\n\r", ch);
    return FALSE;
  }
  /* all seems good */
  return TRUE;
}

//checks if army can/has anything to defend from in room
bool can_defend(CHAR_DATA* ch,  ARMY_DATA* pa, ROOM_INDEX_DATA* in_room, CABAL_DATA* pc, bool fQuiet ){
  int def = 0;

  /* safety */
  if (ch == NULL)
    fQuiet = TRUE;

  /* check for targetting virtual rooms */
  if (IS_VIRVNUM(in_room->vnum)){
    if (!fQuiet)
      send_to_char("You cannot defend virtual rooms.", ch );
    return FALSE;
  }
  else if (!can_reinforce( pc, in_room->area)){
    if (!fQuiet)
      send_to_char("You cannot reinforce this area. (\"help reinforce\")\n\r", ch );
    return FALSE;
  }
/* cannot take over cabal areas */
  else if (in_room->area->pCabal && in_room->area->pCabal->anchor
	   && in_room->area->pCabal->anchor->area == in_room->area){
    if (!fQuiet)
      send_to_char("You cannot attack inside a Cabal!\n\r", ch );
    return  FALSE;
  }
/* check for pacts */
  else if ( in_room->area->pCabal
	    && is_friendly( pc, in_room->area->pCabal) == CABAL_NEUTRAL){
    if (!fQuiet)
      sendf( ch, "You will have to declare a Vendetta against %s before taking armed action against them!\n\r",
	     in_room->area->pCabal->who_name);
    return FALSE;
  }
  /* check if any attacker slots open */
  if (pa == NULL
      || pa->in_room == NULL || pa->in_room != in_room
      || pa->in_slot < 0 || pa->in_slot >= INROOM_ATTACKERS){
    def = INROOM_ATTACKERS - 1 - count_defenders( in_room, FALSE);

    if (def < 1){
      if (!fQuiet)
	send_to_char("Unable to defend due to lack of space.\n\r", ch);
      return FALSE;
    }
  }
  /* check if we have possible targets */
  if (get_defend_target( pc, in_room) == NULL){
    if (!fQuiet)
      send_to_char("No valid attack targets detected.\n\r", ch);
    return FALSE;
  }
  /* all seems good */
  return TRUE;
}

ARMY_DATA* bastion( ARMY_INDEX_DATA* pai, ROOM_INDEX_DATA* room, CABAL_DATA* pc, int hp){
  ARMY_DATA* pBas;
  CABAL_DATA* pOld;

  if (room == NULL)
    return NULL;

  /* create new army */
  pBas = create_army( pai, pc );

  /* we have a living tower now, knock it down to 1% hp */
  pBas->hit = UMAX(1, hp * pBas->max_hit / 100 );

  /* save current cabal */
  pOld = room->area->pCabal;

  /* stick it into the bastion slot */
  army_to_room( pBas, room );
  army_to_slot( pBas, INROOM_BASTION, FALSE);

  /* check if changed hands */
  if (room->area->pCabal && !is_same_cabal(pOld, room->area->pCabal)){
    if (pOld == NULL)
      cp_event_army( pBas, pOld, room->area, CP_EVENT_AREA );
    else if (pOld != NULL){
      cp_event_army( pBas, pOld, room->area, CP_EVENT_CONQUER );
    }
  }
  return pBas;
}

/* creates a cabal bastion in the room the army is in */
ARMY_DATA* bastion_room( ROOM_INDEX_DATA* room, CABAL_DATA* pc, int hp){
  ARMY_INDEX_DATA* pai;
  char buf[MIL];

  if (room == NULL)
    return NULL;
  /* make sure the cabal has a prototype */
  else if ( (pai = get_army_index( pc->pIndexData->tower )) == NULL)
    return NULL;
  else if ( GET_CAB_CP( pc ) < pai->cost){
    cabal_echo(pc, "Coffers could not cover creation of bastion." );
    return NULL;
  }
  CP_CAB_GAIN( pc, -pai->cost);

  sprintf( buf, "%d %ss have been drawn from coffers to cover construction.",
	   pai->cost,
	   pc->currency);
  cabal_echo(pc, buf );

  //put bastion in
  return (bastion( pai, room, pc, hp ));
}

int* gen_vars( int v0, int v1, int v2){
  static int vars[ARMY_VARS];

  memset(&vars, 0, sizeof(int) * ARMY_VARS);

  vars[0]	= v0;
  vars[1]	= v1;
  vars[2]	= v2;

  return vars;
}

//leaves a room, reports leaving if in room returns duration of travel
bool army_leave(ARMY_DATA* pa ){
  char buf[MIL];
  if (pa->in_room == NULL)
    return FALSE;
  else
    pa->vars[2] = pa->in_room->vnum;

  sprintf( buf, "Departing from %s.", pa->in_room->area->name );
  army_report( pa, buf, REPORT_NEUT, FALSE );

  if (pa->in_room && pa->in_room->people)
    act("$t leaves the area.",  pa->in_room->people, pa->short_desc, NULL, TO_ALL);
  army_from_room( pa );
  return TRUE;
}

/* moves an army to designated room vnum if possible
 * will switch armies state if something happends along the way
 * returns TRUE on normal uneventful move
 *
 * Follows the path room by room and tests for patrols
 * If hostile ungarrisoned, defender detected:
   1) If attack slots open, attack
   2) If attacks slots not open enter state untill can attack or leave
*/

bool army_move( ARMY_DATA* pa ){
  PATH_QUEUE* curr = pa->path;
  ROOM_INDEX_DATA* last_room = NULL;

  for (;curr; curr = curr->next){
    last_room = curr->room;
    //make sure there are defenders
    if (curr->room->room_armies.defenders < 1)
      continue;
    //if defenders are busy with max attacking armies, move on
    else if (curr->room->room_armies.attackers > INROOM_MAXARMIES - INROOM_ATTACKERS)
      continue;
    //if this is an army with attack order to this room ignore
    else if (pa->order == AS_ATTA && pa->vars[0] == curr->room->vnum)
      continue;
    // possibility of intercept, enter rooom
    army_to_room( pa, curr->room );
    //check for intercept
    if (!army_intercept( pa ))
      army_from_room( pa );
    else{
      return TRUE;
    }
  }
  //reached destination safely
  army_to_room( pa, last_room );
  clean_path_queue( pa->path );
  pa->path = NULL;

  if (pa->in_room && pa->in_room->people)
    act("$t enters the area.",  pa->in_room->people, pa->short_desc, NULL, TO_ALL);
  return TRUE;
}

/* returns an army to garrison */
bool army_return( ARMY_DATA* pa ){
  CMEMBER_DATA* cm = get_cmember( pa->commander, pa->pCabal );
  char buf[MIL];

  //army that has a commander
  if (cm != NULL){
    //report in
    sprintf( buf, "Returned to barracks (%s).", pa->commander);
    army_report( pa, buf, REPORT_NEUT, FALSE );

    if (IS_SET(pa->army_flags, ARMY_FLAG_ELITE))
      update_garrison_cm( cm, 0, 1);
    else
      update_garrison_cm( cm, 1, 0);
  }
  else{
    if (pa->pCabal->armies_ready < 10)
      pa->pCabal->armies_ready ++;
    sprintf( buf, "Returned to recruitment pool.");
    army_report( pa, buf, REPORT_NEUT, FALSE );
  }
  extract_army( pa );
  return TRUE;
}

/* builds a bastion or garrisons one of the spots */
bool army_garrison( ARMY_DATA* pa ){
  ARMY_DATA* pBas;
  int i;
  char buf[MIL];

  if (pa->in_room == NULL)
    return FALSE;
  else if (!can_bastion( NULL, pa, pa->in_room, pa->pCabal, TRUE ))
    return FALSE;

  /* bastion does not exist */
  if ( (pBas = get_bastion( pa->in_room )) == NULL){
    ROOM_INDEX_DATA* room = pa->in_room;
    CABAL_DATA* pc = pa->pCabal;

    //create a new bastion, elite army at 50%hp, otherwise 1&
    if (IS_SET(pa->army_flags, ARMY_FLAG_ELITE))
      pBas = bastion_room( room, pc,  50);
    else
      pBas = bastion_room( room, pc,  1);

    if (pBas == NULL){
      bug("armies.c>army_garrison: Expected a new bastion but found NULL.", 0);
    }
    else{
      set_army_commander( pBas, pBas->pCabal->name, pa->command_rank );
      //report in
      sprintf( buf, "Constructed bastion[#%-4d] at %-20.20s``.", pBas->ID, pBas->in_room->name);
      army_report( pa, buf, REPORT_GOOD, TRUE );
      if (pa->in_room->people){
	act("$t construct $T in the area.",
	    pa->in_room->people,
	    pa->short_desc,
	    pBas->short_desc,
	    TO_ALL);
      }
    }

    //get rid of old army
    extract_army( pa );
    return TRUE;
  }

  /* bastion exists */
  /* get the slot we can garrison in */
  for (i = INROOM_DEFENDERS; i < INROOM_ATTACKERS; i++){
    if (pa->in_room->room_armies.armies[i] == NULL
	|| pa == pa->in_room->room_armies.armies[i]){

      /* set their order to idle */
      order_army( pa, AS_NONE, gen_vars(0, 0, 0));

      if (pa->in_room->people){
	act("$t garrison $T.",
	    pa->in_room->people,
	    pa->short_desc,
	    pBas->short_desc,
	    TO_ALL);
      }
      //report in
      sprintf( buf, "Garrisoned bastion at %-20.20s``.", pa->in_room->name);
      army_report( pa, buf, REPORT_GOOD, TRUE );

      //if cabal is different from bastion, switch sides
      if (!is_same_cabal(pa->pCabal, pBas->pCabal )){
	sprintf( buf, "Now under %s's command.", pBas->pCabal->name);
	army_report( pa, buf, REPORT_GOOD, TRUE );
	pa->pCabal = pBas->pCabal;
	set_army_commander( pa, pa->pCabal->name, RANK_ELDER -1 );
	sprintf( buf, "Joined your forces in %s.", pa->in_room->name);
	army_report( pa, buf, REPORT_GOOD, TRUE );
      }
      set_army_commander( pa, pa->pCabal->name, -1 );

      //set garrison bit so the support is increased when inserted into slot
      SET_BIT(pa->army_flags, ARMY_FLAG_GARRISON );

      /* insert into slot and gain armor */
      army_to_slot( pa, i, FALSE );
      pa->armor = pa->max_armor + pBas->armor;

      return TRUE;
    }
  }
  return FALSE;
}

//upgrades a bastion in the room
//extracts old bastion and checks for increase in friendly defender armor
bool army_fortify( ARMY_DATA* pa ){
  ARMY_DATA* pAr, *pBas;
  ROOM_INDEX_DATA* room = pa->in_room;
  CABAL_DATA* pc = pa->pCabal;
  int hit, i;
  bool fElite = IS_SET(pa->army_flags, ARMY_FLAG_ELITE) ? TRUE : FALSE;
  char buf[MIL];

  if (room == NULL)
    return FALSE;
  else if (!can_fortify( NULL, room, pa->pCabal, TRUE ))
    return FALSE;

  /* bastion must exists if can_fortify worked */
  if (pa->in_room->people){
    act("$t completes the fortifications.",
	pa->in_room->people,
	pa->short_desc,
	NULL,
	TO_ALL);
  }
  //report in
  sprintf( buf, "Fortified bastion[#%-4d] at %s.",
	   room->room_armies.armies[INROOM_BASTION]->ID,
	   room->name);
  army_report( pa, buf, REPORT_GOOD, TRUE );

  /* extract fortifying army */
  extract_army( pa );

  /* store current hp */
  hit = room->room_armies.armies[INROOM_BASTION]->hit;
  /* extract old bastion */
  extract_army(room->room_armies.armies[INROOM_BASTION]);

  //create an upgraded version
  if ( (pBas = create_army( get_army_index( pc->pIndexData->tower_upgrade ), pc)) == NULL){
    bug("armies.c->army_fortify: could not create bastion. (%d)", pc->pIndexData->tower_upgrade);
    return FALSE;
  }

  pBas->hit = hit;
  //set upgrade start hp to 1/2 if not elite old bastion's hp
  if (!fElite)
    pBas->hit = 1 + pBas->hit / 2;

  /* put it in room and into bastion slot */
  army_to_room( pBas, room );
  army_to_slot( pBas, INROOM_BASTION, FALSE );

/* run through garrisons and reset armor to new number */
  for (i = INROOM_DEFENDERS; i < INROOM_ATTACKERS; i++){
    pAr = room->room_armies.armies[i];
    if (pAr == NULL || !IS_SET(pAr->army_flags, ARMY_FLAG_GARRISON ))
      continue;
    else
      pAr->armor = pAr->max_armor + pBas->armor;
  }
  return TRUE;
}

/* sets an army as patrol */
bool army_patrol( ARMY_DATA* pa ){
  int i;
  char buf[MIL];

  if (pa->in_room == NULL)
    return FALSE;
  else if (!can_patrol( NULL, pa, pa->in_room, pa->pCabal, TRUE ))
    return FALSE;

  /* get the slot we can patrol in */
  for (i = INROOM_DEFENDERS; i < INROOM_ATTACKERS; i++){
    if (pa->in_room->room_armies.armies[i] == NULL){

      /* insert into slot */
      army_to_slot( pa, i, FALSE );

      if (pa->in_room->people){
	act("$t begins to patrol the area.",
	    pa->in_room->people,
	    pa->short_desc,
	    NULL,
	    TO_ALL);
      }
      //report in
      sprintf( buf, "Patroling %s.", pa->in_room->name);
      army_report( pa, buf, REPORT_GOOD, TRUE );
      return TRUE;
    }
  }
  return FALSE;
}

//sets armies attacking each other
void set_attacking( ARMY_DATA* att, ARMY_DATA* def){
  att->attacking = def;
  if (def->attacking == NULL)
    def->attacking = att;
}

void stop_attacking( ARMY_DATA* pa ){
  int i;

  if (pa->in_room == NULL){
    ARMY_DATA* tpa, *pa_next;
    int hash;
    nlogf("BUG: stop_attacking: army not in room. (%s with ID %d)",
	  pa->noun, pa->ID);
    for (hash = 0; hash < ARMY_HASH_SIZE; hash++){
      for (tpa = army_list[hash]; tpa; tpa = pa_next){
	pa_next = tpa->next;
	if (tpa->attacking == pa)
	  tpa->attacking = NULL;
      }
    }
    pa->attacking = NULL;
    return;
  }
  for (i = INROOM_BASTION; i < INROOM_MAXARMIES; i++){
    if (pa->in_room->room_armies.armies[i]
	&& pa->in_room->room_armies.armies[i]->attacking == pa)
      pa->in_room->room_armies.armies[i]->attacking = NULL;
  }
  pa->attacking = NULL;
}

//forces any allies of defender at war with attacker to attack attacker
//bastions checked last
void ally_attack_check( ARMY_DATA* att, ARMY_DATA* def, bool fQuiet ){
  ARMY_DATA* pa;
  int i;
  bool fGarr = FALSE;

  for (i = INROOM_ATTACKERS - 1; i >= INROOM_BASTION; i--){
    if ( (pa = att->in_room->room_armies.armies[i]) == NULL)
      continue;
    else if (pa == att)
      continue;
    else if (is_friendly(pa->pCabal, def->pCabal) != CABAL_FRIEND)
      continue;
    else if (is_friendly(pa->pCabal, att->pCabal) != CABAL_ENEMY)
      continue;
    else{
      if (i == INROOM_BASTION){
	if (fGarr)
	  set_state(pa, AS_BESE, 0, gen_vars(0,0,0));
	else
	  set_state(pa, AS_FIGH, 0, gen_vars(0,0,0));
	continue;
      }
      else if (i != INROOM_BASTION)
	fGarr = TRUE;

      if (pa->attacking)
	continue;

      if (!fQuiet && pa != def){
	char buf[MIL];

	if (is_same_cabal(def->pCabal, pa->pCabal)){
	  sprintf( buf, "Providing battle support for [#%-4d].", def->ID);
	  army_report( pa, buf, REPORT_GOOD, TRUE );
	}
	else{
	  sprintf( buf, "Providing support to %s's %s[#%-4d].",
		   def->pCabal->name,
		   def->noun,
		   def->ID);
	  army_report( pa, buf, REPORT_GOOD, TRUE );
	  sprintf( buf, "%s's %s[#%-4d] providing support.",
		   pa->pCabal->name,
		   pa->noun,
		   pa->ID);
	  army_report( def, buf, REPORT_GOOD, TRUE );
	}
      }
      set_attacking( pa, att );
    }
  }
}

/* sets an army as attacker, combat starts on next army_combat update */
bool army_attack( ARMY_DATA* pa ){
  ARMY_DATA* tar;
  int i;
  bool fQuiet = TRUE;
  char buf[MIL];

  if (pa->in_room == NULL)
    return FALSE;
  else if (!can_attack( NULL, pa, pa->in_room, pa->pCabal, TRUE ))
    return FALSE;

  /* get the slot we can attack from */
  if (pa->in_slot < 0){
    fQuiet = FALSE;
    for (i = INROOM_ATTACKERS; i < INROOM_MAXARMIES; i++){
      if (pa->in_room->room_armies.armies[i] == NULL){

	/* insert into slot */
	army_to_slot( pa, i, FALSE );
	//report ion
	sprintf( buf, "Engaging enemies.");
	army_report( pa, buf, REPORT_GOOD, TRUE );
	break;
      }
    }
  }
  if ( (tar = get_attack_target( pa->pCabal, pa->in_room)) == NULL){
    bug("armies.c>army_attack: Attacking without valid target. (%d)", pa->ID);
    return TRUE;
  }
  if (pa->in_room->people){
    act("$t begins battle with $T.",
	pa->in_room->people,
	pa->short_desc,
	tar->short_desc,
	TO_ALL);
  }
  //report to enemies
  sprintf( buf, "Attacked by %s[#%-4d] in %s.",
	   pa->noun,
	   pa->ID,
	   tar->in_room->area->name);
  army_report( tar, buf, REPORT_BAD, TRUE );
  set_attacking( pa, tar );
  ally_attack_check( tar, pa, fQuiet );
  return FALSE;
}

/* checks if an army can be intercepted in this room */
bool army_intercept( ARMY_DATA* pa ){
  ARMY_DATA* pTar = NULL, *pAr;
  char buf[MIL];
  int i;

  if (pa->in_room == NULL)
    return FALSE;

  /* check for any defender threats, then for slots open */
  for (i = INROOM_DEFENDERS; i < INROOM_MAXARMIES; i++){
    /* defender check */
    if (i < INROOM_ATTACKERS){
      if ( (pAr = pa->in_room->room_armies.armies[i]) == NULL)
	continue;
      else if (is_friendly( pa->pCabal, pAr->pCabal) != CABAL_ENEMY)
	continue;
      //found possible enemy
      if (pTar == NULL || pAr->hit < pTar->hit)
	pTar = pAr;
    }
    /* attacker check */
    else{
      //if there were no threats, break off
      if (pTar == NULL)
	return FALSE;
      //look for valid spot
      else if (pa->in_room->room_armies.armies[i] == NULL){
	/* insert into slot */
	army_to_slot( pa, i, FALSE );

	/* start the attack */
	set_attacking( pa, pTar );
	ally_attack_check(pa, pTar, TRUE );

	if (pa->in_room->people){
	  act("$T intercepts $t as it moves past.",
	      pa->in_room->people,
	      pa->short_desc,
	      pTar->short_desc,
	      TO_ALL);
	}
	//report in
	sprintf( buf, "Intercepted by enemy %s[#%-4d].", pTar->noun, pTar->ID);
	army_report( pa, buf, REPORT_BAD, TRUE );
	sprintf( buf, "Intercepted enemy %s[#%-4d]. Engaging.", pa->noun, pa->ID);
	army_report( pTar, buf, REPORT_BAD, TRUE );
	return TRUE;
      }//end if slot open
    }//end for attacker slots
  }//end for each slot

  /* if were are here, means there were targets, but no valid slots to enter */
  return FALSE;
}

/* sets an army as defender, combat starts on next army_combat update */
bool army_defend( ARMY_DATA* pa ){
  ARMY_DATA* tar, *pAr;
  int i;
  bool fQuiet = TRUE;
  char buf[MIL];

  if (pa->in_room == NULL)
    return FALSE;
  else if (!can_defend( NULL, pa, pa->in_room, pa->pCabal, TRUE ))
    return FALSE;

  /* get the slot we can defend from */
  if (pa->in_slot < 0){
    fQuiet = FALSE;
    for (i = INROOM_DEFENDERS; i < INROOM_ATTACKERS; i++){
      if (pa->in_room->room_armies.armies[i] == NULL){

	/* insert into slot */
	army_to_slot( pa, i, FALSE );

	//report in
	sprintf( buf, "Defending %s from enemies.", pa->in_room->area->name);
	army_report( pa, buf, REPORT_GOOD, TRUE );
	break;
      }
    }
  }
  if ( (tar = get_defend_target( pa->pCabal, pa->in_room)) == NULL){
    bug("armies.c>army_defend: Defending without valid target. (%d)", pa->ID);
    return TRUE;
  }
  if (pa->in_room->people){
    act("$t manouvers to defend the area.",
	pa->in_room->people,
	pa->short_desc,
	NULL,
	TO_ALL);
  }
  //report to enemies
  sprintf( buf, "Attacked by %s[#%-4d] in %s.",
	   pa->noun,
	   pa->ID,
	   tar->in_room->area->name);
  army_report( tar, buf, REPORT_BAD, TRUE );
  set_attacking( pa, tar );
  ally_attack_check( tar, pa, fQuiet );

  /* retarget enemies */
  for (i = INROOM_ATTACKERS; i < INROOM_MAXARMIES; i++){
    if ( (pAr = pa->in_room->room_armies.armies[i]) != NULL){
      if ( (tar = get_attack_target( pAr->pCabal, pa->in_room)) == NULL){
	bug("armies.c>army_defend: Attacker without valid target. (%d)", pa->ID);
	return TRUE;
      }
      set_attacking( pAr, tar );
    }
  }

  return FALSE;
}

//initilizes an empty report queue for use
void init_repq( REPORT_QUEUE* q){
  q->bot.prev = NULL;
  q->bot.next = &q->top;
  q->top.prev = &q->bot;
  q->top.next = NULL;

  q->size = 0;
}

//create a new report
ARMY_REPORT* new_report( char* string ){
  ARMY_REPORT* pRep = malloc( sizeof( *pRep ));

  memset( pRep, 0, sizeof( ARMY_REPORT ));

  pRep->string = str_dup( string );
  pRep->time   = mud_data.current_time;

  return pRep;
}

//frees a report
void free_report( ARMY_REPORT* pRep ){

  if (!IS_NULLSTR( pRep->string ))
    free_string( pRep->string );

  free( pRep );
}

//pops a report off the top of queue
void pop_report(  REPORT_QUEUE* q ){
  ARMY_REPORT* pRep;

  if (q->size < 1)
    return;
  else
    pRep = q->top.prev;

  pRep->next->prev	= pRep->prev;
  pRep->prev->next	= pRep->next;

  q->size --;

  free_report( pRep );
}

//inserts a report at bottom of queue
void add_report( REPORT_QUEUE* q, ARMY_REPORT* pRep ){
  if (q == NULL || pRep == NULL)
    return;

  pRep->next		= q->bot.next;
  pRep->next->prev	= pRep;
  pRep->prev		= &q->bot;
  pRep->prev->next	= pRep;

  if (++q->size > ARMY_MAXREPORT)
    pop_report( q );
}


//adds a report into a queue
void add_army_report( REPORT_QUEUE* q, char* string ){
  ARMY_REPORT* pRep;

  if (IS_NULLSTR(string))
    return;
  else
    pRep = new_report( string );
  add_report( q, pRep );
}

bool is_commander( CHAR_DATA* ch, ARMY_DATA* pa ){
  if (IS_NPC(ch))
    return FALSE;
  else if (!is_same_cabal( ch->pCabal, pa->pCabal))
    return FALSE;
  else if (IS_ELDER(ch))
    return TRUE;
  else if (ch->pcdata->rank > pa->command_rank)
    return TRUE;
  else
    return (!str_cmp(ch->name, pa->commander));
}

//counts how many attackers in area
int count_attackers( ROOM_INDEX_DATA* room ){
  int i;
  int count = 0;

  for (i = INROOM_ATTACKERS; i < INROOM_MAXARMIES; i++){
    if (room->room_armies.armies[i] != NULL)
      count++;
  }
  return count;
}

//counts how many attackers in area
int count_defenders( ROOM_INDEX_DATA* room, bool fCountBastion ){
  int i;
  int count = 0;
  if (fCountBastion)
    i = INROOM_BASTION;
  else
    i = INROOM_DEFENDERS;

  for (; i < INROOM_ATTACKERS; i++){
    if (room->room_armies.armies[i] != NULL)
      count++;
  }
  return count;
}

//returns poitner to bastion if there is one in area
ARMY_DATA* get_bastion(ROOM_INDEX_DATA* room ){
  return (room->room_armies.armies[INROOM_BASTION]);
}

/* get a target defender for an attacker */
ARMY_DATA* get_attack_target( CABAL_DATA* pc, ROOM_INDEX_DATA* pRoom ){
  ARMY_DATA* pa, *pBas;
  int i = 0;
  int low_hp = 9999;
  int low_spot = -1;

  if (pc == NULL ||pRoom == NULL)
    return NULL;

  /* check each defender spot to see if they are at war with us */
  for (i = INROOM_DEFENDERS; i < INROOM_ATTACKERS; i++){
    if ( (pa = pRoom->room_armies.armies[i]) == NULL)
      continue;
    else if (is_friendly(pc, pa->pCabal) != CABAL_ENEMY)
      continue;
    //got a target
    if (pa->hit < low_hp){
      low_hp = pa->hit;
      low_spot = i;
    }
  }
  /* get bastion from here */
  if (low_spot == -1
      && (pBas = get_bastion( pRoom )) != NULL
      && is_friendly( pc, pBas->pCabal) == CABAL_ENEMY)
    low_spot = pBas->in_slot;

  if (low_spot == -1)
    return NULL;
  else
    return (pRoom->room_armies.armies[low_spot]);
}

/* get a target attacker for a defender */
ARMY_DATA* get_defend_target( CABAL_DATA* pc, ROOM_INDEX_DATA* pRoom ){
  ARMY_DATA* pa;
  int i = 0;
  int low_hp = 9999;
  int low_spot = -1;

  if (pc == NULL ||pRoom == NULL)
    return NULL;

  /* check each attacker spot to see if they are at war with us */
  for (i = INROOM_ATTACKERS; i < INROOM_MAXARMIES; i++){
    if ( (pa = pRoom->room_armies.armies[i]) == NULL)
      continue;
    else if (is_friendly(pc, pa->pCabal) != CABAL_ENEMY)
      continue;
    //got a target
    if (pa->hit < low_hp){
      low_hp = pa->hit;
      low_spot = i;
    }
  }

  if (low_spot == -1)
    return NULL;
  else
    return (pRoom->room_armies.armies[low_spot]);
}


//check if an army is dead
bool is_army_dead( ARMY_DATA* pa ){
  if (pa->hit <= 0)
    return TRUE;
  else
    return FALSE;
}

//creates an army corpse object in room
void army_make_corpse( ARMY_DATA* pa, ROOM_INDEX_DATA* room ){
  OBJ_DATA*  obj;
  char buf[MIL];

  if (room == NULL)
    return;
  if (pa->pIndexData->type == ARMY_TYPE_BASTION)
    obj = create_object( get_obj_index( OBJ_VNUM_TOWER_CORPSE), 60 );
  else
    obj = create_object( get_obj_index( OBJ_VNUM_ARMY_CORPSE), 60 );
  if (obj == NULL)
    return;


  //set the strings
  sprintf( buf, obj->name, pa->noun );
  free_string( obj->name );
  obj->name = str_dup( buf );

  sprintf( buf, obj->short_descr, pa->short_desc );
  free_string( obj->short_descr );
  obj->short_descr = str_dup( buf );

  sprintf( buf, obj->description, pa->short_desc);
  free_string( obj->description );
  obj->description = str_dup( buf );

  obj->timer = 24;
  obj_to_room( obj, room );

}

//forces all the defenders to return
void return_defenders( ROOM_INDEX_DATA* room ){
  ARMY_DATA* pa;
  int i;

  if (room->room_armies.defenders < 1)
    return;
  for (i =INROOM_DEFENDERS; i < INROOM_ATTACKERS; i++){
    if ( (pa = room->room_armies.armies[i]) == NULL)
      return;
    else{
      stop_attacking( pa );
      order_army( pa, AS_RETU, gen_vars( 0, 0, 0));
    }
  }
}

//performs necessary actions for army to die
void army_death( ARMY_DATA* att, ARMY_DATA* def ){
  char buf[MIL];
  char* word_room, *word;

  if (def->in_room == NULL){
    bug("armies.c>army_death: Defender is not in room. (ID%d)", def->ID);
  }
  /* if this is bastion being killed, return defenders */
  if (def->in_room && def->pIndexData->type == ARMY_TYPE_BASTION){
    return_defenders( def->in_room );
  }
/* SELF KILL */
  if (att == def){
  //show echo
    if (def->pIndexData->type == ARMY_TYPE_BASTION){
      word = "Dismantled";
      word_room = "dismantled";
    }
    else{
      word = "disbanded";
      word_room = "disbanded";
    }
    if (def->in_room && def->in_room->people){
      if (def->short_desc[strlen(def->short_desc)] == 's')
	act("$t have been $T!", def->in_room->people, def->short_desc, word_room, TO_ALL);
      else
	act("$t has been $T!", def->in_room->people, def->short_desc, word_room, TO_ALL);
    }
    //echo to cabal of defeated army
    sprintf(buf, "%s %s[#%-4d].", word, def->noun, att->ID );
    army_report( def, buf, REPORT_BAD, TRUE );
  }
/* NORMAL KILL */
  else{
    //show echo
    if (def->pIndexData->type == ARMY_TYPE_BASTION){
      word = "Captured";
      word_room = "CAPTURED";
    }
    else{
      word = "Defeated";
      word_room = "DEFEATED";
    }
    //echo to room
    if (def->in_room && def->in_room->people){
      act("$t has been $T!", def->in_room->people, def->short_desc, word_room, TO_ALL);
    }
    //echo to cabal of defeated army
    sprintf(buf, "%s by %s[#%-4d].", word, att->noun, att->ID );
    army_report( def, buf, REPORT_BAD, TRUE );

    //echo to cabal of attacking army
    if (def->pIndexData->type == ARMY_TYPE_BASTION){
      ROOM_INDEX_DATA* room = def->in_room;
      ARMY_INDEX_DATA* pai;

      //reward
      cp_event_army( att, def->pCabal, NULL, CP_EVENT_SIEGE );

      //try to get index for new tower, we always downgrade when capturing
      pai = get_army_index( att->pCabal->pIndexData->tower);
      if (pai == NULL){
	bug("armies.c>army_death: Could not get tower index for cabal %d", att->pCabal->vnum);
	extract_army( def );
	return;
      }

      //extract the defeated bastion, and put in a fresh one of ours
      extract_army( def );
      def = bastion( pai, room, att->pCabal,  1);

      sprintf(buf, "Captured enemy bastion.[#%-4d].", def->ID );
      army_report( att, buf, REPORT_GOOD, TRUE );
      return;
    }
    else{
      cp_event_army( att, def->pCabal, NULL, CP_EVENT_VICTORY );
      sprintf(buf, "Triumphed over %s.", def->noun );
      army_report( att, buf, REPORT_GOOD, TRUE );
    }
  }

  /* COMMON THINGS TO DO AFTER DEATH */
  army_make_corpse( def, def->in_room );
  extract_army( def );
}

/* Meat and potatoes of army combat
   Phases:
   1) Precombat
     1.1) Are we fighting anyone (return if not)
     2.2) Zero dam, splash_dam
     2.3) Set state to AS_FIGH
   2) Combat
     2.1) Calculate damage
     2.2) Splash_dam = 1/4 dam if flying
     2.3) Apply damage minus armor absorption armor * dam / 100;
     2.4) Apply splash damage but never to less then 1 hit
   3) Post Combat
     3.1) Is enemy dead? (army_death)
     3.2) Return damage done
*/

int army_combat( ARMY_DATA* pa ){
  int dam, dam_splash = 0;
  //  int a_sup, d_sup;

  if (pa->attacking == NULL)
    return 0;
  if (pa->pIndexData->type == ARMY_TYPE_BASTION
      && pa->state != AS_FIGH)
    return 0;
  else if (pa->state != AS_FIGH)
    set_state( pa, AS_FIGH, 0, gen_vars(0, 0, 0));


  dam = dice( pa->off_dice[DICE_NUMBER], pa->off_dice[DICE_TYPE]);
  dam += pa->off_dice[DICE_BONUS];

  //if no members on the defending cabal, lower damage
  if (pa->attacking->pCabal && pa->attacking->pCabal->present < 1)
    dam = ARMY_NODEFENDER_MULT * dam / 100;
  else
    dam = ARMY_DAMAGE_MULT * dam / 100;

  /* disabled as not needed
  //underdog modifier for cabals with low support.
  if (pa->pCabal->present > 0
      && (a_sup = pa->pCabal->support[ECO_INCOME]) < CABAL_UNDERDOG_SUP
      && (d_sup = pa->attacking->pCabal->support[ECO_INCOME]) > CABAL_UNDERDOG_SUP){
    dam = URANGE(dam, dam * (1 + (4 * (CABAL_UNDERDOG_SUP - a_sup)) / CABAL_UNDERDOG_SUP), 5 * dam);
  }
  */
  //boost from leadership
  if (pa->pCabal->present > 0 && pa->in_room){
    CHAR_DATA* ch = pa->in_room->people;
    int temp = 0, mod = 0;

    for (; ch; ch = ch->next_in_room){
      if (ch->pCabal == NULL || IS_NPC(ch) || !IS_AWAKE(ch) || !is_same_cabal(ch->pCabal, pa->pCabal))
	continue;
      else if ( (temp = get_skill( ch, gsn_leadership)) > mod){
	mod = temp;
	check_improve( ch, gsn_leadership, TRUE, 1);
      }
    }
    if ( pa->attacking->pCabal->present > 0 )
      dam += mod * dam / 100;
  }

  if (IS_ARMY(pa, ARMY_FLAG_FLYING))
    dam_splash = dam / 4;

  /* adjust damage for armor */
  dam = (100 - pa->attacking->armor) * dam / 100;

  /* damage target */
  pa->attacking->hit -= dam;

  /* DEBUG
  if (pa->pIndexData->type == ARMY_TYPE_BASTION && pa->in_room->people){
    char buf[MIL];
    sprintf( buf, "%s did %d damage to %s(%d/%d)",
	     pa->short_desc,
	     dam,
	     pa->attacking->short_desc,
	     pa->attacking->hit,
	     pa->attacking->max_hit);
    act(buf, pa->in_room->people, NULL, NULL, TO_ALL);
  }
  */

  /* splash damage */
  if (dam_splash){
    ARMY_DATA* pAr;
    int i;
    for (i = INROOM_MAXARMIES - 1; i >= INROOM_BASTION; i--){
      if ( (pAr = pa->in_room->room_armies.armies[i]) == NULL)
	continue;
      else if (pAr->attacking != pa)
	continue;
      else if (pa->attacking == pAr)
	continue;
      else if (pAr->hit > 1){
	pAr->hit = UMAX( 1, pAr->hit - (100 - pAr->armor) * dam_splash / 100);
	dam_splash /= 2;
      }
    }
  }
  if (is_army_dead(pa->attacking)){
    ARMY_DATA* victim = pa->attacking;
    stop_attacking( pa );
    army_death( pa, victim);
  }
  return dam;
}

/* MAIN COMBAT UPDATE, called from army_update */
void army_combat_update(){
  ARMY_DATA* pa, *pa_next;
  int hash;

  /* to avoid problems with extraction, there are following pahses *
     - Start of loop
     1) Is this army dead? (army_death)
     2) Is army in room and in slot and attacking ? (skip army if not )
     3) Perform combat in room that this army is in.
       3.0) do ally_assist check
       3.1) calculate damage to person we are attacking
       3.2) apply the damage to person we are attacking
       3.3) is the person we are attacking dead? (army_death)
  */

  for (hash = 0; hash < ARMY_HASH_SIZE; hash++){
    for (pa = army_list[hash]; pa; pa = pa_next){
      pa_next = pa->next;

      /* if army is dead, kill it and continue */
      if (is_army_dead( pa )){
	stop_attacking( pa );
	army_death( pa, pa );
	continue;
      }
      else if (pa->in_room == NULL || pa->in_slot < 0 || pa->attacking == NULL)
	continue;

      /* ally assist check */
      ally_attack_check( pa, pa->attacking, FALSE );

      /* rudimentary ai for cabal with no members */
      /* called only if this is a bastion */
      /* disabled as not needed
      if (pa->in_slot == INROOM_BASTION
	  && (pa->state == AS_FIGH || pa->state == AS_BESE))
	army_cabal_ai( pa );
      */
      /* perform combat for everyone in room */
      army_combat( pa );

      /* if army is dead, kill it and continue */
      if (is_army_dead( pa )){
	stop_attacking( pa );
	army_death( pa, pa );
	continue;
      }
      /* END OF COMBAT UPDATE FOR THIS ARMY */
    }//end for army in hash cell
  }//end for hash cell
}

/* ARMY PATH FUNCTIONS */
//creates an army path, returns duration of movement or FALSE
int set_army_path( ARMY_DATA* pa, int src_vnum, int dest_vnum ){
  ROOM_INDEX_DATA* src, *dest;
  PATH_QUEUE* path;
  int dist = 0, dur;

  if ( (src = get_room_index( src_vnum )) == NULL)
    return -1;
  if ( (dest = get_room_index( dest_vnum )) == NULL)
    return -1;

  if ( (path = generate_path(src, dest, ARMY_PATH_MAX, TRUE, &dist, pa->pCabal)) == NULL)
    return -1;

  clean_path();
  if (pa->path)
    clean_path_queue( pa->path );
  pa->path = path;
  pa->src_room = src_vnum;

  /* get duration of move */
  if (src->area == dest->area)
    dur = 0;
  else
    dur = UMIN(10, dist / 15 );

  if (pa->pCabal && IS_CABAL(pa->pCabal, CABAL_ROYAL))
    dur = dur / 2;
  return (UMAX(0, dur));
}

/* calculates support this army uses in given area */
int get_army_support( ARMY_DATA* pa, AREA_DATA* area ){

  if (pa == NULL )
    return 0;
  else if (!IS_ARMY(pa, ARMY_FLAG_GARRISON))
    return 0;
  else
    return (pa->pIndexData->support * area->support/(100 * area->bastion_max));
}

/* writes a single army to file */
void fwrite_army( FILE* fp, ARMY_DATA* pa ){
  char buf[MIL];
  int i = 0;

  if (pa == NULL || pa->pCabal == NULL)
    return;

  //Easy stuff
  fprintf( fp, "#%d\n",			pa->pIndexData->vnum );
  fprintf( fp, "Cabal %s~\n",		pa->pCabal->name );
  fprintf( fp, "Comm %s~\n",		pa->commander );
  fprintf( fp, "CommRank %d\n",		pa->command_rank );
  fprintf( fp, "HitMax %d\n",		pa->max_hit );
  fprintf( fp, "Hit %d\n",		pa->hit );
  fprintf( fp, "Arm %d\n",		pa->armor );
  fprintf( fp, "ArmMax  %d\n",		pa->max_armor );

  //State/Order vars
  fprintf( fp, "Vars %d ", ARMY_VARS);
  for (i = 0; i < ARMY_VARS; i++){
    fprintf( fp, "%d %d   ",		pa->ovars[i], pa->vars[i] );
  }
  fprintf( fp, "\n");

  //Offense dice
  fprintf( fp, "OffDice %d ", DICE_BONUS );
  for (i = 0; i < DICE_BONUS; i++){
    fprintf( fp, "%d ", pa->off_dice[i]);
  }
  fprintf( fp, "\n" );

  //Things that can be taken from index
  if (pa->order)
    fprintf( fp, "Order %d\n",		pa->order );
  if (pa->state)
    fprintf( fp, "State %d\n",		pa->state );
  if (pa->lifetime)
    fprintf( fp, "Life  %d\n",		pa->lifetime);
  if (pa->army_flags != pa->pIndexData->army_flags)
    fprintf( fp, "Flags %s\n",		fwrite_flag( pa->army_flags, buf ) );
  if (pa->cabal_ai_life)
    fprintf( fp, "AiLife  %d\n",		pa->cabal_ai_life);

/* loaded from indexes each time
  if (str_cmp( pa->noun, pa->pIndexData->noun))
    fprintf( fp, "Noun %s~\n",		pa->noun );
  if (str_cmp( pa->short_desc, pa->pIndexData->short_desc))
    fprintf( fp, "Short %s~\n",		pa->short_desc );
  if (str_cmp( pa->long_desc, pa->pIndexData->long_desc))
    fprintf( fp, "Long %s~\n",		pa->long_desc );
  if (str_cmp( pa->desc, pa->pIndexData->desc))
    fprintf( fp, "Desc %s~\n",		pa->desc );
*/

  //things that might or might not be there
  if (pa->path){
    PATH_QUEUE* curr = pa->path;
    while (curr->next){curr = curr->next;}
    fprintf( fp, "Path %d %d\n",	pa->src_room, curr->room->vnum );
  }
  if (pa->in_room)
    fprintf( fp, "Room %d %d\n",	pa->in_room->vnum, pa->in_slot );
  if (pa->attacking)
    fprintf( fp, "Attack %d\n", pa->attacking->in_slot );

  //put some space before next one
  fprintf( fp, "END\n\n");
}

/* writes ALL armies to file */
bool fwrite_armies( FILE* fp ){
  AREA_DATA* pe;
  ARMY_DATA* pa;
  int hash;

  for (hash = 0; hash < ARMY_HASH_SIZE; hash++){
    for (pa = army_list[hash]; pa; pa = pa->next){
      //write army
      fwrite_army( fp, pa );
    }
  }
  fprintf( fp, "#0\n" );

  //write the status of area influences
  for (pe = area_first; pe; pe = pe->next){
    /* never write the 0th vnum */
    if (pe->vnum == 0)
      continue;
    fprintf( fp, "#%d %d %d ", pe->vnum, MAX_CABAL, pe->pCabal ? pe->pCabal->vnum : 0);
    for (hash = 0; hash < MAX_CABAL; hash++){
      fprintf( fp, "%d ", IS_AREA(pe, AREA_CABAL) ? 0 : pe->cabal_influence[hash]);
    }
    fprintf( fp, "\n");
  }
  fprintf( fp, "#0\n");

  return TRUE;
}

/* reads influence of a single area from file */
void fread_influence( FILE* fp, int vnum ){
  AREA_DATA* pe = get_area_data( vnum );
  CABAL_DATA* pCab = NULL;
  int max = fread_number( fp );
  int cab = fread_number( fp );
  int i;

  if (pe == NULL){
    bug("armise.c>fread_influence: could not load area %d.", vnum );
  }
  if ( cab ){
    if ((pCab = get_cabal_vnum( cab )) == NULL){
      bug("armise.c>fread_influence: could not assign cabal %d.", cab );
    }
    else if (!IS_AREA(pe, AREA_CABAL))
      pe->pCabal = pCab;
  }

  for (i = 0; i < max; i++){
    if (pe && max <= MAX_CABAL)
      pe->cabal_influence[i] = fread_number( fp );
    else
      fread_number( fp );
  }
}

/* read a single army from file */
ARMY_DATA* fread_army( FILE* fp, int vnum ){
  ARMY_DATA* pa;
  ARMY_INDEX_DATA* pAi;
  char *word = NULL;
  bool fMatch = FALSE, fExtract = FALSE;
  int i = 0;

  if ( vnum < 1 )
    return NULL;
  else if ( (pAi = get_army_index( vnum )) == NULL){
    bug("fread_army: Could not load index %d", vnum);
    return NULL;
  }
  //get a new army data
  pa = create_army( pAi, NULL );

  //reset any data here that is NOT 0 normaly
  pa->att_slot = -1;

  /* start reading the data in */
  for (; ;){
    fExtract = FALSE;
    word = feof(fp) ? "END" : fread_word( fp );

    //select the commands
    switch( UPPER(word[0]) ){
    case 'A':
      AKEY(  "AiLife",		pa->cabal_ai_life,	fread_number( fp ) );
      AKEY(  "Arm",		pa->armor,		fread_number( fp ) );
      AKEY(  "ArmMax",		pa->max_armor,		fread_number( fp ) );
      AKEY(  "Attack",		pa->att_slot,		fread_number( fp ) );
      break;
    case 'C':
      if (!str_cmp( "Cabal", word )){
	CABAL_DATA* pCabal;
	char* name = fread_string( fp );

	if ( (pCabal = get_cabal( name )) == NULL){
	  char buf[MIL];
	  sprintf( buf, "fread_army:Cabal %s not found.", name );
	  bug( buf, 0 );
	  fExtract = TRUE;
	}
	free_string( name );
	pa->pCabal = pCabal;

	fMatch = TRUE;
	break;
      }
      AKEYS( "Comm",		pa->commander,		fread_string( fp ) );
      AKEY(  "CommRank",	pa->command_rank,	fread_number( fp ) );
      break;
    case 'D':
      AKEYS( "Desc",		pa->desc,		fread_string( fp ) );
      break;
    case 'E':
      if (!str_cmp("END", word)){
	if (fExtract){
	  extract_army( pa );
	  return NULL;
	}
	return pa;
      }
      break;
    case 'F':
      AKEY(  "Flags",		pa->army_flags,		fread_flag( fp )   );
      break;
    case 'H':
      AKEY(  "Hit",		pa->hit,		fread_number( fp ) );
      AKEY(  "HitMax",		pa->max_hit,		fread_number( fp ) );
      break;
    case 'L':
      AKEY(  "Life",		pa->lifetime,		fread_number( fp ) );
      AKEYS( "Long",		pa->long_desc,		fread_string( fp ) );
      break;
    case 'N':
      AKEYS( "Noun",		pa->noun,		fread_string( fp ) );
      break;
    case 'O':
      if (!str_cmp( "OffDice", word )){
	int max = fread_number( fp );

	for (i = 0; i < max && i < DICE_BONUS; i++){
	  pa->off_dice[i]	=	fread_number( fp );
	}

	fMatch = TRUE;
	break;
      }
      AKEY(  "Order",		pa->order,		fread_number( fp ) );
      break;
    case 'P':
      if (!str_cmp( "Path", word)){
	int src_room	=	fread_number( fp );
	int des_room	=	fread_number( fp );
	if (set_army_path( pa, src_room, des_room ) < 0){
	  char buf[MIL];
	  sprintf( buf, "fread_army: could not load path from %d to %d.", src_room, des_room );
	  bug( buf, 0);
	}
	fMatch = TRUE;
	break;
      }
      break;
    case 'R':
      if (!str_cmp("Room", word)){
	int vnum	= fread_number( fp );
	int slot	= fread_number( fp );
	ROOM_INDEX_DATA* room = get_room_index( vnum );

	if (room == NULL){
	  bug( "fread_army: could not place army in room %d.", vnum );
	  fExtract = TRUE;
	}
	else{
	  army_to_room( pa, room );
	  if (slot >= 0 )
	    army_to_slot( pa, slot, FALSE );
	}
	fMatch = TRUE;
	break;
      }
    case 'S':
      AKEYS( "Short",		pa->short_desc,		fread_string( fp ) );
      AKEY(  "State",		pa->state,		fread_number( fp ) );
      break;
    case 'V':
      if (!str_cmp( "Vars", word )){
	int max = fread_number( fp );

	for (i = 0; i < max && i < ARMY_VARS; i++){
	  pa->ovars[i]	=	fread_number( fp );
	  pa->vars[i]	=	fread_number( fp );
	}

	fMatch = TRUE;
	break;
      }
      break;
    }//end select
    if ( !fMatch ){
      bugf("fread_army: [%s] no match.", word);
      fread_to_eol( fp );
    }
  }//end for

  if (fExtract)
    extract_army( pa );
  return NULL;
}

void fread_influences( FILE* fp ){
  char c;
  int vnum;

  for (;;){
    c = fread_letter( fp );

    if (c != '#'){
      bug("fread_influences: # not found.", 0);
      exit( 1 );
    }
    else
      vnum = fread_number( fp );
    if (vnum == 0)
      break;
    fread_influence( fp, vnum);
  }
}

void fread_armies( FILE* fp ){
  char c;
  int vnum;

  for (;;){
    c = fread_letter( fp );

    if (c != '#'){
      bug("fread_armies: # not found.", 0);
      exit( 1 );
    }
    else
      vnum = fread_number( fp );
    if (vnum == 0)
      break;
    else if (fread_army( fp, vnum) == NULL){
      bug("fread_armies: could not read army vnum %d", vnum );
    }
  }
}

//repoints any data that needs repointing after loading armies from save files
void fix_armies(){
  ARMY_DATA* pa;
  ARMY_DATA* pTar;
  int hash;

  for (hash = 0; hash < ARMY_HASH_SIZE; hash++){
    for (pa = army_list[hash]; pa; pa = pa->next){
      if (pa->att_slot < 0)
	continue;
      else if ( pa->in_room == NULL){
	bug("fix_armies: army attacking in NULL room. ID:%d", pa->ID);
	continue;
      }
      else if ( (pTar = pa->in_room->room_armies.armies[pa->att_slot]) == NULL){
	bugf("fix_armies: Army attacking a null slot. ID%d Slot %d", pa->ID, pa->att_slot);
	continue;
      }
      else
	pa->attacking = pTar;
    }
  }
}

//sets charactery army focus onto a given army id, returns TRUE on success
//ID of -1 sets to the room person is in
bool set_focus( CHAR_DATA* ch, ROOM_INDEX_DATA* focus ){
  ROOM_INDEX_DATA* last = GET_FOCUS(ch);
  if (focus == NULL){
    SET_LASTFOCUS( ch, GET_FOCUS(ch) );
    SET_FOCUS( ch, focus );
    return TRUE;
  }
  SET_FOCUS( ch, focus );
  if (last == NULL)
    SET_LASTFOCUS( ch, focus );
  else
    SET_LASTFOCUS( ch, last );
  return TRUE;
}


//returns focus of an army
ROOM_INDEX_DATA* get_army_focus( CHAR_DATA* ch, int ID ){
  ARMY_DATA* pa;

  if (IS_NPC(ch))
    return NULL;
  else if (ch->in_room == NULL)
    return NULL;
  else if (ID < 0){
    return ch->in_room;
  }

  if ( (pa = get_army_world( ID )) == NULL || pa->in_room == NULL)
    return NULL;
  return pa->in_room;
}

//SHOWS ARMY COMMAND PROMPT
/*
A = Attack,	G = Garrison
P = Patrol,	F = Fortify
H = Hold,	B = Barracks

C = Conscript,	T = Train
R = Release,

Z = Zoom,	M = Map
O = Orders,	Q = Queue

? = Help

*/
void show_army_prompt( CHAR_DATA* ch ){
  bool fBlind = FALSE;

  if (IS_AFFECTED(ch, AFF_BLIND) || IS_AFFECTED2(ch, AFF_TERRAIN) || is_affected(ch, gsn_forest_mist)){
    fBlind = TRUE;
  }

  /* DEBUG PROMPT
  sendf( ch, "\n\r<%d: %d/%d.%d/%d >[APHGFB]-[CTR]-[ZMOQ]-[?] Map#%-3d: %-20.20s``",
	 GARR_RED( ch ),
	 GARR_NOR( ch ),
	 ch->pcdata->member->armies[TIMN],
	 GARR_UPG( ch ),
	 ch->pcdata->member->armies[TIMU],
	 GET_FOCUS(ch) ? GET_FOCUS(ch)->area->vnum : 0,
	 GET_FOCUS(ch) ? GET_FOCUS(ch)->area->name: "???");
  */
  sendf( ch, "\n\r<%d: %d.%d >[APHGFB]-[CTR]-[ZMOQ]-[?] Map#%-3d: %-20.20s``",
	 GARR_RED( ch ),
	 GARR_NOR( ch ),
	 GARR_UPG( ch ),
	 GET_FOCUS(ch) ? GET_FOCUS(ch)->area->vnum : 0,
	 GET_FOCUS(ch) ? fBlind ? "???" : GET_FOCUS(ch)->area->name : "???");
}

//shows standart army command info
void show_army_screen( CHAR_DATA* ch, ROOM_INDEX_DATA* room ){
  TOWER_DATA* tow = get_tower_data( get_parent(ch->pCabal), room->area);
  bool fBlind = FALSE;

  if (IS_AFFECTED(ch, AFF_BLIND) || IS_AFFECTED2(ch, AFF_TERRAIN) || is_affected(ch, gsn_forest_mist)){
    fBlind = TRUE;
  }

  sendf( ch, "%-20.20s`` %d/%d%s\n\r%-25s``\n\r",
	 fBlind ? "???" : room->area->name,
	 tow ? tow->towers : 0,
	 room->area->bastion_max,
	 tow && tow->fRein ? "R" : "",
	 fBlind ? "???" : room->name);
  send_to_char("\n\r",ch);
  if (room->room_armies.count){
    examine_room_armies(ch, &room->room_armies );
  }
  else
    send_to_char("No units spotted.\n\r", ch);
}

//shows report queue
void army_order_queue( CHAR_DATA* ch ){

  show_report_q(&get_parent(ch->pCabal)->report_q, ch );
}

//shows world or specific map
void army_order_map( CHAR_DATA* ch, char* argument ){
  int vnum = atoi( argument );
  AREA_DATA* area;

  if (IS_AFFECTED(ch, AFF_BLIND) || IS_AFFECTED2(ch, AFF_TERRAIN) || is_affected(ch, gsn_forest_mist)){
    send_to_char("You can't see the map!\n\r", ch);
    return;
  }
  if (IS_NULLSTR(argument)){
    show_area_armies( ch, GET_FOCUS(ch)->area, ch->pCabal );
    return;
  }
  else if (vnum){
    if ( (area = get_area_data( vnum )) == NULL){
      send_to_char("No such map.\n\r", ch);
      return;
    }
    show_area_armies( ch, area, ch->pCabal );
    return;
  }
  else if (!str_prefix( argument, "world")){
    show_world_armies( ch, ch->pCabal );
    return;
  }
  else{
    CABAL_DATA* pc = get_cabal( argument );
    if ( (pc = get_parent(pc)) == NULL){
      send_to_char("No such cabal.\n\r", ch);
      return;
    }
    show_world_armies( ch, pc);
    return;
  }
}

//train order
void army_order_muster( CHAR_DATA* ch, char* argument ){
  CABAL_DATA* pc = get_parent( ch->pCabal );
  ARMY_INDEX_DATA* pai;
  int cost, val;

  if ( (pai = get_army_index( pc->pIndexData->army)) == NULL){
    send_to_char("Your cabal has no armies to be recruited.\n\r", ch );
    return;
  }
  else
    cost = pai->cost;

  /* get number to recruit */
  if(IS_NULLSTR( argument))
    val = -1;//+1 to current queue
  else
    val = atoi( argument );
  start_recruitment( ch, FALSE, val, cost );
}

//sets forcus on army or current room
bool army_order_focus( CHAR_DATA* ch, char* argument ){
  ROOM_INDEX_DATA* focus;

  if (IS_NULLSTR(argument)){
    set_focus( ch, ch->in_room);
    send_to_char("Focus set to current room.\n\r", ch);
  }
  else if ( (focus = get_army_focus( ch, atoi(argument))) == NULL)
    send_to_char("No army with such ID.\n\r", ch);
  else if (!set_focus( ch, focus))
    send_to_char("Error! Unable to set focus.\n\r", ch);
  else{
    return TRUE;
  }
  return FALSE;
}

void army_order_scrap( CHAR_DATA* ch, char* argument ){
  int tot;
  int nor;
  int upg;

  if (IS_NULLSTR( argument ))
    tot = 1;
  else
    tot = atoi( argument );

  /* decide how many armies of each type to disband */
  if (tot > GARR_NOR(ch)){
    nor = GARR_NOR(ch);
    upg = UMIN(GARR_UPG(ch), tot - nor);
  }
  else{
    upg = 0;
    nor = tot;
  }
  update_garrison( ch, -nor, -upg );
  sendf( ch, "Barracks at: %d.%d\n\r", GARR_NOR(ch), GARR_UPG(ch));
}

void army_order_list( CHAR_DATA* ch ){
  show_armies( ch, ch->pCabal, ch->name );
}

void army_order_upgrade( CHAR_DATA* ch, char* argument ){
  CABAL_DATA* pc = get_parent( ch->pCabal );
  ARMY_INDEX_DATA* pai;
  int cost, val;

  if ( (pai = get_army_index( pc->pIndexData->army_upgrade)) == NULL){
    send_to_char("Your cabal has no elite armies.\n\r", ch );
    return;
  }
  else
    cost = pai->cost;

  /* get number to recruit */
  if(IS_NULLSTR( argument))
    val = -1;
  else
    val = atoi( argument );
  start_recruitment( ch, TRUE, val, cost );
}

void army_order_attack( CHAR_DATA* ch, char* argument, bool fElite){
  CABAL_DATA* pc = get_parent(ch->pCabal );
  ROOM_INDEX_DATA* in_room = GET_FOCUS(ch);
  ARMY_DATA* pa = NULL;
  char buf[MIL];
  int ID, vnum = 0;

  /* find the army to be ordered to garrison */
  if (!IS_NULLSTR(argument)){
    if ( (ID = atoi( argument )) < 1){
      send_to_char("Attack focus with which army?\n\r", ch);
      return;
    }
    else if ( (pa = get_army_world( ID )) == NULL){
      send_to_char("No such army detected.\n\r", ch);
      return;
    }
    else if (!is_commander( ch, pa)){
      send_to_char("You cannot command that army.\n\r", ch);
      return;
    }
    else if (!can_relocate( ch, pa, FALSE ))
      return;
  }
  else{
    if (!fElite && GARR_NOR(ch) > 0){
      vnum = pc->pIndexData->army;
    }
    else if (fElite && GARR_UPG(ch) > 0){
      vnum = pc->pIndexData->army_upgrade;
    }
    else{
      send_to_char("You lack required armies.\n\r", ch);
      return;
    }
  }

  if (!can_attack( ch, NULL, in_room, pc, FALSE))
    return;

  /* check for army prototype if army not pre-selected */
  if ( !pa){
    if ( (pa = create_army(get_army_index( vnum ), pc)) == NULL){
      send_to_char("Your cabal does not have an army to send.\n\r", ch );
      return;
    }
    else{
      /* remove an army from barracks */
      if (fElite)
	update_garrison( ch, 0, -1 );
      else
	update_garrison( ch, -1, 0 );
    }
  }

  /* set commander and order */
  set_army_commander( pa, ch->name, ch->pcdata->rank );
  order_army( pa, AS_ATTA, gen_vars( in_room->vnum, 0, 0));
  sprintf( buf, "%s %s confirmed.",
	   capitalize(state_table[pa->order].name),
	   in_room->name );
  army_report( pa, buf, REPORT_NEUT, FALSE );

  sprintf( buf, "Spy Report: [%s`0] %s about to attack %s`0 in %s.",
	   get_parent(ch->pCabal)->who_name,
	   pa->noun,
	   in_room->name,
	   in_room->area->name);
//  cabal_echo_flag(CABAL_SPIES, buf );
}

void army_order_patrol( CHAR_DATA* ch, char* argument, bool fElite){
  CABAL_DATA* pc = get_parent(ch->pCabal );
  ROOM_INDEX_DATA* in_room = GET_FOCUS(ch);
  ARMY_DATA* pa = NULL;
  char buf[MIL];
  int ID, vnum = 0;

  /* find the army to be ordered to garrison */
  if (!IS_NULLSTR(argument)){
    if ( (ID = atoi( argument )) < 1){
      send_to_char("Patrol focus with which army?\n\r", ch);
      return;
    }
    else if ( (pa = get_army_world( ID )) == NULL){
      send_to_char("No such army detected.\n\r", ch);
      return;
    }
    else if (!is_commander( ch, pa)){
      send_to_char("You cannot command that army.\n\r", ch);
      return;
    }
    else if (!can_relocate( ch, pa, FALSE ))
      return;
  }
  else{
    if (!fElite && GARR_NOR(ch) > 0){
      vnum = pc->pIndexData->army;
    }
    else if (fElite && GARR_UPG(ch) > 0){
      vnum = pc->pIndexData->army_upgrade;
    }
    else{
      send_to_char("You lack required armies.\n\r", ch);
      return;
    }
  }

  if (!can_patrol( ch, pa, in_room, pc, FALSE))
    return;

  /* check for army prototype if army not pre-selected */
  if ( !pa){
    if ( (pa = create_army(get_army_index( vnum ), pc)) == NULL){
      send_to_char("Your cabal does not have an army to send.\n\r", ch );
      return;
    }
    else{
      /* remove an army from barracks */
      if (fElite)
	update_garrison( ch, 0, -1 );
      else
	update_garrison( ch, -1, 0 );
    }
  }

  /* set commander and order */
  set_army_commander( pa, ch->name, ch->pcdata->rank );
  order_army( pa, AS_PATR, gen_vars( in_room->vnum, 0, 0));
  sprintf( buf, "%s %s confirmed.",
	   capitalize(state_table[pa->order].name),
	   in_room->name );
  army_report( pa, buf, REPORT_NEUT, FALSE );

  sprintf( buf, "Spy Report: [%s`0] %s about to patrol %s`0 in %s.",
	   get_parent(ch->pCabal)->who_name,
	   pa->noun,
	   in_room->name,
	   in_room->area->name);
//  cabal_echo_flag(CABAL_SPIES, buf );
}

void army_order_defend( CHAR_DATA* ch, char* argument, bool fElite){
  CABAL_DATA* pc = get_parent(ch->pCabal );
  ROOM_INDEX_DATA* in_room = GET_FOCUS(ch);
  ARMY_DATA* pa = NULL;
  char buf[MIL];
  int ID, vnum = 0;

  /* find the army to be ordered to garrison */
  if (!IS_NULLSTR(argument)){
    if ( (ID = atoi( argument )) < 1){
      send_to_char("Defend focus with which army?\n\r", ch);
      return;
    }
    else if ( (pa = get_army_world( ID )) == NULL){
      send_to_char("No such army detected.\n\r", ch);
      return;
    }
    else if (!is_commander( ch, pa)){
      send_to_char("You cannot command that army.\n\r", ch);
      return;
    }
    else if (!can_relocate( ch, pa, FALSE ))
      return;
  }
  else{
    if (!fElite && GARR_NOR(ch) > 0){
      vnum = pc->pIndexData->army;
    }
    else if (fElite && GARR_UPG(ch) > 0){
      vnum = pc->pIndexData->army_upgrade;
    }
    else{
      send_to_char("You lack required armies.\n\r", ch);
      return;
    }
  }

  if (!can_defend( ch, pa, in_room, pc, FALSE))
    return;

  /* check for army prototype if army not pre-selected */
  if ( !pa){
    if ( (pa = create_army(get_army_index( vnum ), pc)) == NULL){
      send_to_char("Your cabal does not have an army to send.\n\r", ch );
      return;
    }
    else{
      /* remove an army from barracks */
      if (fElite)
	update_garrison( ch, 0, -1 );
      else
	update_garrison( ch, -1, 0 );
    }
  }

  /* set commander and order */
  set_army_commander( pa, ch->name, ch->pcdata->rank );
  order_army( pa, AS_DEFE, gen_vars( in_room->vnum, 0, 0));
  sprintf( buf, "%s %s confirmed.",
	   capitalize(state_table[pa->order].name),
	   in_room->name );
  army_report( pa, buf, REPORT_NEUT, FALSE );

  sprintf( buf, "Spy Report: [%s`0] %s about to defend %s`0 in %s.",
	   get_parent(ch->pCabal)->who_name,
	   pa->noun,
	   in_room->name,
	   in_room->area->name);
//  cabal_echo_flag(CABAL_SPIES, buf );
}

void army_order_garrison( CHAR_DATA* ch, char* argument, bool fElite ){
  char buf[MIL];
  CABAL_DATA* pc = get_parent(ch->pCabal );
  ROOM_INDEX_DATA* in_room = GET_FOCUS(ch);
  ARMY_DATA* pa = NULL;
  ARMY_INDEX_DATA* pai;
  int ID, vnum = 0;

  /* find the army to be ordered to garrison */
  if (!IS_NULLSTR(argument)){
    if ( (ID = atoi( argument )) < 1){
      send_to_char("Garrison focus with which army?\n\r", ch);
      return;
    }
    else if ( (pa = get_army_world( ID )) == NULL){
      send_to_char("No such army detected.\n\r", ch);
      return;
    }
    else if (!is_commander( ch, pa)){
      send_to_char("You cannot command that army.\n\r", ch);
      return;
    }
    else if (!can_relocate( ch, pa, FALSE ))
      return;
  }
  else{
    if (!fElite && GARR_NOR(ch) > 0){
      vnum = pc->pIndexData->army;
    }
    else if (fElite && GARR_UPG(ch) > 0){
      vnum = pc->pIndexData->army_upgrade;
    }
    else{
      send_to_char("You lack required armies.\n\r", ch);
      return;
    }
  }

  if (!can_bastion(ch, pa, in_room, pc, FALSE ))
    return;

  /* check for army prototype if army not pre-selected */
  if ( !pa){
    if ( (pa = create_army(get_army_index( vnum ), pc)) == NULL){
      send_to_char("Your cabal does not have an army to send.\n\r", ch );
      return;
    }
    else{
      /* remove an army from barracks */
      if (fElite)
	update_garrison( ch, 0, -1 );
      else
	update_garrison( ch, -1, 0 );
    }
  }
  else if ( (pai = get_army_index( pc->pIndexData->tower )) == NULL){
    send_to_char("Your cabal does not have a tower to build.\n\r", ch );
    return;
  }

  /* set commander and order */
  set_army_commander( pa, ch->name, ch->pcdata->rank );
  order_army( pa, AS_GARR, gen_vars( in_room->vnum, 0, 0));
  sprintf( buf, "%s %s confirmed.",
	   capitalize(state_table[pa->order].name),
	   in_room->name );
  army_report( pa, buf, REPORT_NEUT, FALSE );

  sprintf( buf, "Spy Report: [%s`0] %s about to garrison %s`0 in %s.",
	   get_parent(ch->pCabal)->who_name,
	   pa->noun,
	   in_room->name,
	   in_room->area->name);
//  cabal_echo_flag(CABAL_SPIES, buf );
}

void army_order_fortify( CHAR_DATA* ch, char* argument, bool fElite ){
  char buf[MIL];
  CABAL_DATA* pc = get_parent(ch->pCabal );
  ROOM_INDEX_DATA* in_room = GET_FOCUS(ch);
  ARMY_DATA* pa = NULL;
  ARMY_INDEX_DATA* pai;
  int ID, vnum = 0;

  /* find the army to be ordered to garrison */
  if (!IS_NULLSTR(argument)){
    if ( (ID = atoi( argument )) < 1){
      send_to_char("Fortify focus with which army?\n\r", ch);
      return;
    }
    else if ( (pa = get_army_world( ID )) == NULL){
      send_to_char("No such army detected.\n\r", ch);
      return;
    }
    else if (!is_commander( ch, pa)){
      send_to_char("You cannot command that army.\n\r", ch);
      return;
    }
    else if (!can_relocate( ch, pa, FALSE ))
      return;
  }
  else{
    if (!fElite && GARR_NOR(ch) > 0){
      vnum = pc->pIndexData->army;
    }
    else if (fElite && GARR_UPG(ch) > 0){
      vnum = pc->pIndexData->army_upgrade;
    }
    else{
      send_to_char("You lack required armies.\n\r", ch);
      return;
    }
  }

  if (!can_fortify(ch, in_room, pc, FALSE ))
    return;

  /* check for army prototype if army not pre-selected */
  if ( !pa){
    if ( (pa = create_army(get_army_index( vnum ), pc)) == NULL){
      send_to_char("Your cabal does not have an army to send.\n\r", ch );
      return;
    }
    else{
      /* remove an army from barracks */
      if (fElite)
	update_garrison( ch, 0, -1 );
      else
	update_garrison( ch, -1, 0 );
    }
  }
  else if ( (pai = get_army_index( pc->pIndexData->tower_upgrade )) == NULL){
    send_to_char("Your cabal does not have a fortified tower to build.\n\r", ch );
    return;
  }

  /* set commander and order */
  set_army_commander( pa, ch->name, ch->pcdata->rank );
  order_army( pa, AS_FORT, gen_vars( in_room->vnum, 0, 0));
  sprintf( buf, "%s %s confirmed.",
	   capitalize(state_table[pa->order].name),
	   in_room->name );
  army_report( pa, buf, REPORT_NEUT, FALSE );

  sprintf( buf, "Spy Report: [%s`0] %s about to fortify %s`0 in %s.",
	   get_parent(ch->pCabal)->who_name,
	   pa->noun,
	   in_room->name,
	   in_room->area->name);
//  cabal_echo_flag(CABAL_SPIES, buf );
}

void army_order_return( CHAR_DATA* ch, char* argument ){
  char buf[MIL];
  ARMY_DATA* pa = NULL;
  int tar_ID;

  /* get target army */
  if (IS_NULLSTR(argument)){
    send_to_char("Order which army to return?\n\r", ch);
    return;
  }

  if ( (tar_ID = atoi( argument )) < 1){
    send_to_char("That is not a valid army ID.\n\r", ch);
    return;
  }
  else if ( (pa = get_army_world( tar_ID)) == NULL){
    sendf( ch, "No unit with ID: %d found.\n\r", tar_ID);
    return;
  }
  else if (!is_commander(ch, pa)){
    send_to_char("You cannot command that army.\n\r", ch );
    return;
  }
  else if (!can_return( ch, pa->in_room, pa, FALSE))
    return;

  if (pa->pIndexData->type != ARMY_TYPE_BASTION){
    sprintf( buf, "Spy Report: [%s`0] %s departing %s`0 in %s.",
	     get_parent(ch->pCabal)->who_name,
	     pa->noun,
	     pa->in_room->name,
	     pa->in_room->area->name);
    /* set order */
    order_army( pa, AS_RETU, gen_vars( 0, 0, 0));
  }
  else{
    sprintf( buf, "Spy Report: [%s`0] %s disbanded in %s`0 in %s.",
	     get_parent(ch->pCabal)->who_name,
	     pa->noun,
	     pa->in_room->name,
	     pa->in_room->area->name);

    sprintf( buf, "%s %s confirmed.",
	     capitalize(state_table[AS_DISB].name),
	     pa->in_room->name );
    /* set order */
    order_army( pa, AS_DISB, gen_vars( 0, 0, 0));
    army_report( pa, buf, REPORT_NEUT, FALSE );
  }
//  cabal_echo_flag(CABAL_SPIES, buf );
}

//army interpret function
bool army_interpret( CHAR_DATA* ch, char* argument ){
  char cmd[MIL];
  bool fMatch = FALSE;

  /* interpret armies only if person has a focus */
  if (IS_NPC(ch) || GET_FOCUS( ch ) == NULL)
    return FALSE;
  else if (ch->pCabal == NULL){
    SET_FOCUS(ch, NULL );
    return FALSE;
  }

  while ( isspace(*argument) ) {
    argument++;
  }
  argument = one_argument( argument, cmd );

  if (IS_NULLSTR( cmd )){
    show_army_screen( ch, GET_FOCUS(ch) );
    show_army_prompt( ch );
    return TRUE;
  }
/* HELP */
  else if (!str_prefix( cmd, "?")){
    send_to_char("[A]ttack,    [P]atrol,  [H]old,   [G]arrison,  [F]ortify, [B]arracks\n\r"\
		 "[aA]ttack,   [pP]atrol, [hH]old,  [gG]arrison, [fF]ortify,[bB]arracks\n\r\n\r"\
		 "[C]onscript, [T]rain,   [R]elease\n\r"\
		 "[Z]oom,      [zZ]oom    [M]ap,    [O]rders,    [Q]ueue\n\r"\
		 "[?] - This screen.\n\r", ch );
    return TRUE;
  }
/* GARRISON  */
  else if (!str_prefix( cmd, "garrison")){
    fMatch = TRUE;
    army_order_garrison( ch, argument, FALSE);
  }
  else if (!str_prefix( cmd, "ggarrison")){
    fMatch = TRUE;
    army_order_garrison( ch, argument, TRUE);
  }
/* FORTIFY  */
  else if (!str_prefix( cmd, "fortify")){
    fMatch = TRUE;
    army_order_fortify( ch, argument, FALSE);
  }
  else if (!str_prefix( cmd, "ffortify")){
    fMatch = TRUE;
    army_order_fortify( ch, argument, TRUE);
  }
/* ATTACK  */
  else if (!str_prefix( cmd, "attack")){
    fMatch = TRUE;
    army_order_attack( ch, argument, FALSE );
  }
  else if (!str_prefix( cmd, "aattack")){
    fMatch = TRUE;
    army_order_attack( ch, argument, TRUE );
  }
/* DEFEND/HOLD  */
  else if (!str_prefix( cmd, "hold")){
    fMatch = TRUE;
    army_order_defend( ch, argument, FALSE );
  }
  else if (!str_prefix( cmd, "hhold")){
    fMatch = TRUE;
    army_order_defend( ch, argument, TRUE );
  }
/* PATROL  */
  else if (!str_prefix( cmd, "patrol")){
    fMatch = TRUE;
    army_order_patrol( ch, argument, FALSE );
  }
  else if (!str_prefix( cmd, "ppatrol")){
    fMatch = TRUE;
    army_order_patrol( ch, argument, TRUE);
  }
/* RETURNBARRACKS   */
  else if (!str_prefix( cmd, "barracks")){
    fMatch = TRUE;
    army_order_return( ch, argument );
  }
/* CONSCRIPT (recruit) */
  else if (!str_prefix( cmd, "conscript")){
    fMatch = TRUE;
    army_order_muster( ch, argument );
  }
/* TRAIN (make elites) */
  else if (!str_prefix( cmd, "train")){
    fMatch = TRUE;
    army_order_upgrade( ch, argument );
  }
/* RELEASE (get rid of armies in barracks) */
  else if (!str_prefix( cmd, "release")){
    fMatch = TRUE;
    army_order_scrap( ch, argument );
  }
/* ORDERS (shows list of armies) */
  else if (!str_prefix( cmd, "orders")){
    fMatch = TRUE;
    army_order_list( ch );
  }
/* QUEUE (show past reports) */
  else if (!str_prefix( cmd, "queue")){
    fMatch = TRUE;
    army_order_queue( ch );
  }
/* MAP (show maps) */
  else if (!str_prefix( cmd, "map")){
    fMatch = TRUE;
    army_order_map( ch, argument );
  }
/* ZOOM (sets focus) */
  else if (!str_prefix( cmd, "zoom")){
    fMatch = TRUE;
    if (army_order_focus( ch, argument ))
      show_army_screen( ch, GET_FOCUS( ch ) );
  }
/* ZZOOM (sets focus to last focus) */
  else if (!str_prefix( cmd, "zzoom")){
    fMatch = TRUE;
    set_focus( ch, GET_LASTFOCUS( ch ));
    show_army_screen( ch, GET_FOCUS( ch ) );
  }
  if (fMatch)
    show_army_prompt( ch );
  return fMatch;
}

struct area_info_s {
  AREA_DATA*	area;
  int		bas;
  int		ubas;
  int		sup_loss;
  bool		fAtt;


  int		ebas;
  int		eubas;
};

//shows status of all areas owned by a cabal
void show_world_armies( CHAR_DATA* ch, CABAL_DATA* pc ){
  struct area_info_s ainfo[top_area];
  ARMY_DATA* pa;
  BUFFER* output;
  AREA_DATA* area;
  char buf[MIL];
  int hash, i;
  float support;
  bool fUpg, fBas;

  memset( &ainfo, 0, sizeof( struct area_info_s ) * top_area );

  for (hash = 0; hash < ARMY_HASH_SIZE; hash++){
    for (pa = army_list[hash]; pa; pa = pa->next){
      /* skip armies not in room/slot */
      if (pa->in_room == NULL || pa->in_slot < 0)
	continue;

      /* updates counts in the given area */
      fUpg = IS_ARMY(pa, ARMY_FLAG_ELITE) ? TRUE : FALSE;
      fBas = pa->in_slot == INROOM_BASTION;

      if (is_same_cabal(pa->pCabal, pc)){
	ainfo[pa->in_room->area->vnum].area = pa->in_room->area;
	ainfo[pa->in_room->area->vnum].bas += fBas && !fUpg;
	ainfo[pa->in_room->area->vnum].ubas += fBas && fUpg;
	if (pa->attacking)
	  ainfo[pa->in_room->area->vnum].fAtt = TRUE;
	if (is_same_cabal(pa->in_room->area->pCabal, pa->pCabal))
	  ainfo[pa->in_room->area->vnum].sup_loss += get_army_support( pa, pa->in_room->area);
      }
      else{
	ainfo[pa->in_room->area->vnum].ebas += fBas && !fUpg;
	ainfo[pa->in_room->area->vnum].eubas += fBas && fUpg;
      }
    }//end for cell
  }//end for hash

  /* we have complete world info now, spit it out*/
  output = new_buf();
  sprintf( buf, "%-3.3s %-26.26s`` %-4.4s %-5.5s  %-7.7s  %-3.3s\n\r",
	   "MAP",
	   "          Name",
	   "Our",
	   "Tot",
	   "Enemy",
	   "Support");
  add_buf( output, buf );

  for (i = 0; i < top_area; i++){
    if ( (area = ainfo[i].area) == NULL)
      ainfo[i].area = get_area_data( i );

    if ( (area = ainfo[i].area) == NULL || area->pCabal == NULL || !is_same_cabal(area->pCabal, pc))
      continue;

    if (area->pCabal == pc)
      support = 100.0 * (float)ainfo[i].sup_loss / CABAL_FULL_SUPPORT_VAL;
    else
      support = 0;
    sprintf( buf, "%3d   %-24.24s``  %2d   %2d    %2d    %3.3f%% %s\n\r",
	     i,
	     area->name,
	     ainfo[i].bas + ainfo[i].ubas,
	     area->bastion_max,
	     ainfo[i].ebas + ainfo[i].eubas,
	     support,
	     ainfo[i].fAtt ? "`![B]``" : "");
    add_buf( output, buf );
  }
  page_to_char(buf_string(output),ch);
  free_buf(output);
}

//shows all units by room in the area
// Focus   Bas  Gar    Pat  Attackers    Room
//[#1234]  [K] [K V]  [K K]  [n n S] Central Square of Val Miran
//[#1234]             [k k]  [S S S] Southern Square of Val Miran

void show_area_armies(CHAR_DATA* ch, AREA_DATA* area, CABAL_DATA* pCabal){
  const int MAX_ROOMS = 36;
  struct room_info_s{
    int		ID;
    ARMY_DATA*	pBas;
    int		gar;
    ARMY_DATA*	pGar[2];
    int		pat;
    ARMY_DATA*	pPat[2];
    int		att;
    ARMY_DATA*	pAtt[3];
    ROOM_INDEX_DATA* pRoom;
  }rinfo[MAX_ROOMS];

  ROOM_INDEX_DATA* room;
  BUFFER* output;
  ARMY_DATA* pa;
  int rooms = 0, slot, vnum, i;
  char buf[MIL], bas_buf[MIL], gar_buf[MIL], pat_buf[MIL], att_buf[MIL];
  char tem[8];
  int tot_bas = 0, tot_gar = 0, tot_pat = 0, sup_los = 0, support = 0;
  bool fOur = TRUE;

  memset( &rinfo, 0, sizeof( struct room_info_s ) * MAX_ROOMS );
  for (vnum = area->min_vnum; vnum <= area->max_vnum; vnum++){
    if ( (room = get_room_index( vnum )) == NULL)
      continue;
    else if (room->room_armies.count < 1)
      continue;
    else if (rooms >= MAX_ROOMS){
      bug("armies.c>show_area_armies: MAX_ROOM overflow.\n\r", 0);
      break;
    }
    for (slot = INROOM_BASTION; slot < INROOM_MAXARMIES; slot++){
      if ( (pa = room->room_armies.armies[slot]) == NULL)
	continue;

      //set our bit
      if (pCabal && is_same_cabal( pCabal, pa->pCabal))
	fOur = TRUE;
      else
	fOur = FALSE;
      //set room and ID for this room
      rinfo[rooms].pRoom = room;
      if (rinfo[rooms].ID < 1)
	rinfo[rooms].ID = pa->ID;

      /* sort data */
      if (pa->in_slot == INROOM_BASTION){
	rinfo[rooms].pBas = pa;
	if (fOur)
	  tot_bas++;
      }
      else if (pa->in_slot < INROOM_ATTACKERS ){
	if (IS_ARMY(pa, ARMY_FLAG_GARRISON)){
	  rinfo[rooms].pGar[rinfo[rooms].gar++] = pa;
	  if (fOur){
	    tot_gar++;
	    sup_los += get_army_support( pa, area );
	  }
	}
	else{
	  rinfo[rooms].pPat[rinfo[rooms].pat++] = pa;
	  if (fOur)
	    tot_pat++;
	}
      }
      else
	rinfo[rooms].pAtt[rinfo[rooms].att++] = pa;
    }//end for army
    rooms++;
  }//end for room

  /* time to spit the data out */
  output = new_buf();
  sprintf( buf, " Focus   Bas  Gar  Pat    Attackers    Room\n\r" );
  add_buf( output, buf );
  for (i = 0; i < rooms; i++){
    /* GENREATE BASTION STRING*/
    if (rinfo[i].pBas){
      sprintf( bas_buf, "[`%c%c``]",
	       rinfo[i].pBas->pCabal->who_name[1],
	       GET_LETTER(rinfo[i].pBas));

    }
    else
      sprintf( bas_buf, " `1 `` " );

    /* GENREATE GARRISON STRING */
    if (rinfo[i].gar){
      int j;
      sprintf( gar_buf, "[");
      for (j = 0; j < INROOM_ATTACKERS - INROOM_DEFENDERS; j++){
	if (rinfo[i].pGar[j]){
	  sprintf( tem, "`%c%c``",
		   rinfo[i].pGar[j]->pCabal->who_name[1],
		   GET_LETTER(rinfo[i].pGar[j]));
	  strcat( gar_buf, tem );
	}
	else
	  strcat( gar_buf, "`1 ``");
      }
      strcat( gar_buf, "]");
    }//end if garrisons
    else
      sprintf( gar_buf, " `1 ```1 `` " );
    /* GENREATE patrol STRING */
    if (rinfo[i].pat){
      int j;
      sprintf( pat_buf, "[");
      for (j = 0; j < INROOM_ATTACKERS - INROOM_DEFENDERS; j++){
	if (rinfo[i].pPat[j]){
	  sprintf( tem, "`%c%c``",
		   rinfo[i].pPat[j]->pCabal->who_name[1],
		   GET_LETTER(rinfo[i].pPat[j]));
	  strcat( pat_buf, tem );
	}
	else
	  strcat( pat_buf, "`1 ``");
      }
      strcat( pat_buf, "]");
    }//end if patrols
    else
      sprintf( pat_buf, " `1 ```1 `` " );
    /* GENREATE STTACK STRING */
    if (rinfo[i].att){
      int j;
      sprintf( att_buf, "[");
      for (j = 0; j < INROOM_MAXARMIES - INROOM_ATTACKERS; j++){
	if (rinfo[i].pAtt[j]){
	  sprintf( tem, "`%c%c``",
		   rinfo[i].pAtt[j]->pCabal->who_name[1],
		   GET_LETTER(rinfo[i].pAtt[j]));
	  strcat( att_buf, tem );
	}
	else
	  strcat( att_buf, "`1 ``");
      }
      strcat( att_buf, "]");
    }//end if garrisons
    else
      sprintf( att_buf, " `1 ```1 ```1 `` " );

    //            " Focus   Bas  Gar   Pat   Attackers    Room\n\r"
    //            "[#1234]  [K] [K V] [K K]   [n n S] Central Square"
    sprintf(buf,"[#%-4d]  %-7.7s %-13.13s %-13.13s   %-19.19s  %-30.30s``\n\r",
	    rinfo[i].ID,
	    bas_buf,
	    gar_buf,
	    pat_buf,
	    att_buf,
	    rinfo[i].pRoom->name);
    add_buf( output, buf );
  }
  if (rooms < 1){
    sprintf( buf, "No units detected.\n\r");
    add_buf( output, buf );
  }
  if (can_reinforce(pCabal, area)){
    support = 100 * (area->support - sup_los) / UMAX(1, area->support);
    add_buf( output, "[R] " );
  }
  else{
    support = 0;
    add_buf( output, "    " );
  }


  //last line has info about the area
  sprintf( buf, "\n\r%-20.20s B:%d/%d  G:%d  P:%d  S:%d(%d%%)\n\r",
	   area->name,
	   tot_bas,
	   area->bastion_max,
	   tot_gar,
	   tot_pat,
	   support * area->support / 100,
	   support);
  add_buf( output, buf );

  page_to_char(buf_string(output),ch);
  free_buf(output);
}

/* shows list of armies based on owner name
 */
void show_armies( CHAR_DATA* ch, CABAL_DATA* pc, char* name ){
  BUFFER* buffer;
  ARMY_DATA* pa;
  int hash;
  char buf[MSL];
  int cnt = 0;

  buffer = new_buf();

  sprintf(buf, "   ID    Hp     State        Area       \n\r" );
  add_buf( buffer, buf );
  sprintf(buf, "------------------------------------------\n\r" );
  add_buf( buffer, buf );

  for (hash = 0; hash < ARMY_HASH_SIZE; hash++){
    for (pa = army_list[hash]; pa; pa = pa->next){
      if (IS_NULLSTR(pa->commander) || pa->commander[0] != name[0]
	  || str_cmp(pa->commander, name))
	continue;
      else
	cnt++;
      sprintf( buf, "%-5u    %-3d   %-10.10s  %-25.25s``\n\r",
	       pa->ID,
	       100 * pa->hit / pa->max_hit,
	       state_table[pa->state].name,
	       pa->in_room ? pa->in_room->area->name : "No Report");
      add_buf( buffer, buf );
    }
  }
  if (cnt)
    page_to_char(buf_string(buffer),ch);
  free_buf(buffer);
}

/*                    INTERFACE FUNCTIONS                    */
//reports an army report
void army_report( ARMY_DATA* pa, char* string, int type, bool fSave ){
  DESCRIPTOR_DATA* d;
  CABAL_DATA* pc;
  char* color;
  char buf[MIL];

  if (pa == NULL || IS_NULLSTR(string ))
    return;
  else
    pc = get_parent( pa->pCabal );

  switch( type ){
  case REPORT_GOOD:	color = "`@";	break;
  case REPORT_BAD:	color = "`!";	break;
  default:		color = "`8";	break;
  }
  sprintf( buf, "%s[#%-4d]%-12.12s``: %s``\n\r",
	   color,
	   pa->ID,
	   pa->noun,
	   string);

  /* DEBUG
     nlogf( "DEBUG: %s", buf );
  */
  /* echo the message */
  for ( d = descriptor_list; d != NULL; d = d->next ){
    CHAR_DATA* rch = d->character;
    bool fDemon = FALSE;

    if ( d->connected != CON_PLAYING
	 || rch == NULL
	 || rch->pCabal == NULL
	 || IS_AFFECTED2(rch, AFF_SILENCE)
	 || IS_SET(rch->comm, COMM_NOCABAL)
	 || IS_SET(rch->comm, COMM_QUIET)
	 || !is_same_cabal(rch->pCabal, pc) ){
      continue;
    }

    fDemon = IS_TELEPATH( rch );
    send_to_char( buf, rch );

    if (!IS_NPC(rch) && rch->pcdata->eavesdropped != NULL
	&& is_affected(rch->pcdata->eavesdropped,gsn_eavesdrop)
	&& !fDemon){

      send_to_char("A faint message transmits from the parasite.\n\r",rch->pcdata->eavesdropped);
      act_new(buf, rch, string, NULL, TO_CHAR, POS_DEAD);
    }
  }

  /* add the message onto the report queue */
  if (!fSave)
    return;
  add_army_report( &pc->report_q, buf );
  /* set the last focus from army */
  SET_QFOCUS( pc, pa->in_room );
}

//reports a cabal report (army report without unit)
void cabal_report( CABAL_DATA* pc, char* string, int type, bool fSave ){
  DESCRIPTOR_DATA* d;
  char* color;
  char buf[MIL];

  if (pc == NULL || IS_NULLSTR(string ))
    return;
  switch( type ){
  case REPORT_GOOD:	color = "`@";	break;
  case REPORT_BAD:	color = "`!";	break;
  default:		color = "`8";	break;
  }
  sprintf( buf, "%s[#%-4s]%-12.12s``: %s``\n\r",
	   color,
	   "----",
	   pc->name,
	   string);

  /* echo the message */
  for ( d = descriptor_list; d != NULL; d = d->next ){
    CHAR_DATA* rch = d->character;
    bool fDemon = FALSE;

    if ( d->connected != CON_PLAYING
	 || rch == NULL
	 || rch->pCabal == NULL
	 || IS_AFFECTED2(rch, AFF_SILENCE)
	 || IS_SET(rch->comm, COMM_NOCABAL)
	 || IS_SET(rch->comm, COMM_QUIET)
	 || !is_same_cabal(rch->pCabal, pc) ){
      continue;
    }

    fDemon = IS_TELEPATH( rch );
    send_to_char( buf, rch );

    if (!IS_NPC(rch) && rch->pcdata->eavesdropped != NULL
	&& is_affected(rch->pcdata->eavesdropped,gsn_eavesdrop)
	&& !fDemon){

      send_to_char("A faint message transmits from the parasite.\n\r",rch->pcdata->eavesdropped);
      act_new(buf, rch, string, NULL, TO_CHAR, POS_DEAD);
    }
  }

  /* add the message onto the report queue */
  if (!fSave)
    return;
  add_army_report( &pc->report_q, buf );
}

//shows the reports in the cabal's queue
void show_report_q(REPORT_QUEUE* q, CHAR_DATA* ch ){
  ARMY_REPORT* pRep;
  BUFFER* output;
  char buf[MIL];
  struct tm* time_data;
  bool fFound = FALSE;

  output = new_buf();

  for (pRep = q->top.prev; pRep != &q->bot; pRep = pRep->prev ){
    time_data = localtime( &pRep->time );

    fFound = TRUE;
    sprintf( buf, "%2d:%02d %s",
	     time_data->tm_hour,
	     time_data->tm_min,
	     pRep->string);
    add_buf( output, buf );
  }

  if (!fFound)
    send_to_char("No recent reports.\n\r", ch);
  else
    page_to_char(buf_string(output),ch);
  free_buf(output);
}

//returns maximum number of armies commanded at given level
int get_max_army( int level ){
  return (3 + UMAX(0, (level - 30) / 10));
}

//returns count of deployed armies by given character name
int get_army_count( char* name ){
  ARMY_DATA* pa;
  int sum = 0;
  int hash;

  for (hash = 0; hash < ARMY_HASH_SIZE; hash++){
    for (pa = army_list[hash]; pa; pa = pa->next){
      if (!IS_NULLSTR(pa->commander)
	  && pa->commander[0] == name[0]
	  && !str_cmp(pa->commander, name))
	sum++;
    }
  }

  return sum;
}

//checks if a character has a maximum number of armies
//TRUE if at or above max
bool check_army_max( CMEMBER_DATA* cm ){
  int max_armies = get_max_army( cm->level );
  int armies = get_army_count( cm->name ) + cm->armies[NOR] + cm->armies[UPG];

  if (IS_CABAL(cm->pCabal, CABAL_ELITE))
    max_armies += 2;
  if (armies >= max_armies){
    return TRUE;
  }
  return FALSE;
}

//Shows army index data in olc
void show_army_index( CHAR_DATA* ch, ARMY_INDEX_DATA* pai ){
  char buf[MSL];

  sprintf( buf, "%-12.12s: [%-12d] (%-12.12s)\n\r", "Vnum", pai->vnum, pai->area->name);
  send_to_char( buf, ch );
  sprintf( buf, "%-12.12s: [%-12.12s]\n\r", "Type", flag_string( army_types, pai->type ) );
  send_to_char( buf, ch );
  sprintf( buf, "%-12.12s: [%-12d]\n\r", "Cost", pai->cost);
  send_to_char( buf, ch );
  sprintf( buf, "%-12.12s: [%-12d] (%% of gain/bastion)\n\r", "Support", pai->support);
  send_to_char( buf, ch );
  sprintf( buf, "%-12.12s: [%-12s]\n\r", "Flags", flag_string( army_flags, pai->army_flags ) );
  send_to_char( buf, ch );
  sprintf( buf, "%-12.12s: [%3dd%-3d+%4d] (Avg %d)\n\r", "Off Dice",
	   pai->off_dice[DICE_NUMBER],
	   pai->off_dice[DICE_TYPE],
	   pai->off_dice[DICE_BONUS],
	   get_dice_avg(pai->off_dice[DICE_NUMBER], pai->off_dice[DICE_TYPE], pai->off_dice[DICE_BONUS]));
  send_to_char( buf, ch );
  sprintf( buf, "%-12.12s: [%3dd%-3d+%4d] (Avg %d)\n\r", "Hit Dice",
	   pai->hit_dice[DICE_NUMBER],
	   pai->hit_dice[DICE_TYPE],
	   pai->hit_dice[DICE_BONUS],
	   get_dice_avg(pai->hit_dice[DICE_NUMBER], pai->hit_dice[DICE_TYPE], pai->hit_dice[DICE_BONUS]));
  send_to_char( buf, ch );
  sprintf( buf, "%-12.12s: [%3dd%-3d+%4d] (Avg %d)\n\r", "Armor Dice",
	   pai->arm_dice[DICE_NUMBER],
	   pai->arm_dice[DICE_TYPE],
	   pai->arm_dice[DICE_BONUS],
	   get_dice_avg(pai->arm_dice[DICE_NUMBER], pai->arm_dice[DICE_TYPE], pai->arm_dice[DICE_BONUS]) );
  send_to_char( buf, ch );
  sprintf( buf, "%-12.12s: %-12s\n\r", "Noun", pai->noun);
  send_to_char( buf, ch );
  sprintf( buf, "%-12.12s: %-12s\n\r", "Short Desc", pai->short_desc);
  send_to_char( buf, ch );
  sprintf( buf, "%-12.12s: %-12s\n\r", "Long Desc", pai->long_desc);
  send_to_char( buf, ch );
  sprintf( buf, "%-12.12s:\n\r%s\n\r", "Description", pai->desc);
  send_to_char( buf, ch );
}

//Saves army index data in area file
void save_army_indexes( FILE *fp, AREA_DATA *pArea ){
  int i;
  ARMY_INDEX_DATA *pai;

  fprintf( fp, "#ARMIES\n" );

  for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
    if ( (pai = get_army_index( i )) ){
      fwrite_army_index( fp, pai );
    }
  fprintf( fp, "#0\n\n\n\n" );
  return;
}

//reads the army index data from area file
void fread_army_indexes( FILE* fp, AREA_DATA* pArea ){
  ARMY_INDEX_DATA *pai;

  for ( ; ; ){
    int vnum;
    char letter;
    letter                          = fread_letter( fp );
    if ( letter != '#' ){
      bug( "armies.c>fread_army_indexes: # not found.", 0 );
      exit( 1 );
    }
    vnum = fread_number( fp );

    /* terminated by #0 */
    if ( vnum < 1)
      break;

    pai = new_army_index();
    if (!fread_army_index(fp, pai, pArea, vnum)){
      free_army_index( pai );
      continue;
    }
    add_army_index( pai );
  }
}


void do_army( CHAR_DATA *ch, char *argument ) {
  CABAL_DATA* pc = get_parent(ch->pCabal);

  if (IS_NPC(ch) || pc == NULL){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  else if (!IS_CABAL(pc, CABAL_AREA)){
    send_to_char("Your cabal has no interest in direct conquest.\n\r", ch );
    return;
  }
  else if (GET_FOCUS(ch) != NULL){
    set_focus( ch, NULL );
    send_to_char("You roll up your maps.\n\r", ch);
    act("$n rolls up the tactical maps.", ch, NULL, NULL, TO_ROOM );
    return;
  }

  if (IS_NULLSTR(argument)){
    if (GET_LASTFOCUS(ch) == NULL)
      set_focus( ch,  ch->in_room);
    else
      set_focus( ch,  GET_LASTFOCUS( ch ));
  }
  else{
    if (GET_QFOCUS(ch->pCabal) == NULL){
      if (GET_LASTFOCUS(ch) == NULL)
	set_focus( ch,  ch->in_room);
      else
	set_focus( ch,  GET_LASTFOCUS( ch ));
    }
    else
      set_focus( ch, GET_QFOCUS( ch->pCabal ));
  }

  act("$n spreads open a tactical map of $t.", ch, GET_FOCUS(ch)->area->name, NULL, TO_ROOM);
  act("You spreads open a tactical map of $t.", ch,GET_FOCUS(ch)->area->name, NULL, TO_CHAR);
  army_interpret( ch, "" );
}

//recruits an army into given char's barracks (called in effect.c)
void recruit_army( CHAR_DATA* ch, bool fElite ){

  if (fElite)
    update_garrison( ch, -1, 1);
  else
    update_garrison( ch, 1, 0);

  if (fElite)
    send_to_char("Unit upgraded.  ", ch );
  else
    send_to_char("Unit recruited.  ", ch );
  sendf( ch, "Barracks at: %d.%d\n\r", GARR_NOR(ch), GARR_UPG(ch));
}

//updates the garrison data based on cabalmember data not ch
void update_garrison_cm( CMEMBER_DATA*  cm, int nor, int upg ){
  if (cm == NULL)
    return;
  cm->armies[NOR] = UMAX(0, cm->armies[NOR] + nor );
  cm->armies[UPG] = UMAX(0, cm->armies[UPG] + upg );
}

//adds a given amount of gain to normal and elite garrisons for character
void update_garrison( CHAR_DATA*  ch, int nor, int upg ){
  if (IS_NPC(ch))
    return;
  else if (ch->pcdata->member == NULL)
    return;

  update_garrison_cm( ch->pcdata->member, nor, upg );
}

ARMY_INDEX_DATA* get_army_index( int vnum ){
  ARMY_INDEX_DATA* pai = army_index_list.next;

  for(; pai; pai = pai->next){
    if (pai->vnum == vnum)
      return pai;
  }
  return NULL;
}

void examine_room_armies( CHAR_DATA* ch, ARMY_ROOM_DATA* pArd ){
  ARMY_DATA* pa;
  char buf[MSL];
  int i;

  sprintf( buf, "%-7.7s %-10.10s  %-3.3s     %-12.12s %-12.12s %s\n\r",
	   "  ID",
	   " Defense",
	   "Arm",
	   "Commander",
	   " Name",
	   "Status");
  send_to_char( buf, ch );
  for (i = 0; i < INROOM_MAXARMIES; i++){
    if ( (pa = pArd->armies[i]) == NULL)
      continue;

    sprintf(buf, "[%-4d] %4d%s %-3d %-3.3s %-12.12s %-12.12s %s``\n\r",
	    pa->ID,
	    pa->hit,
	    short_bar(pa->hit, pa->max_hit),
	    pa->armor,
	    IS_SET(pa->army_flags, ARMY_FLAG_FLYING ) ? "~v~" : "   ",
	    pa->commander,
	    pa->noun,
	    state_table[pa->state].name);
    send_to_char( buf, ch );
  }
}

struct show_room_s {
  ARMY_DATA* attacking; //whom are we fighting
  char* short_desc;//short desc
  char* long_desc;//long desc
  char* cabal;	//cabal who name
  bool fGar;	//garrisoned
  int type;	//INROOM TYPE
  int state;	//state
  int sub;	//subscript
  int cur;	//current hp of weakest (to be attacked) member
  int max;	//max total hp of weakest (to be attacked) member
};

//Shows armies in room to character
void show_room_armies(CHAR_DATA* ch, ARMY_ROOM_DATA* pArd ){
  BUFFER* buffer;
  ARMY_DATA* pa;
  char sub_buf[8];
  char cab_buf[16];
  char lon_buf[MSL];
  char buf[MSL];
  const int size = INROOM_MAXARMIES;
  struct show_room_s info[size];

  int i, j, bas = 0, def = 0, att = 0, type;
  memset( &info, 0, sizeof( struct show_room_s ) * size);

  /* we gather info in order of:
     - Bastion
     - Defenders
     - Attackers
     Same long_desc are kept together
  */

  for (i = 0; i < INROOM_MAXARMIES; i++){
    if ( (pa = pArd->armies[i]) == NULL)
      continue;
    /* search for same long desc, or first null spot */
    for (j = 0; j < size; j++){

      if (i == INROOM_BASTION){
	type = i;
	bas++;
      }
      else if(i > INROOM_BASTION && i < INROOM_ATTACKERS){
	type = INROOM_DEFENDERS;
	def++;
      }
      else{
	type = INROOM_ATTACKERS;
	att++;
      }

      if (IS_NULLSTR(info[j].long_desc)){
	info[j].long_desc  = pa->long_desc;
	info[j].short_desc  = pa->short_desc;
	info[j].cabal = pa->pCabal->who_name;
	info[j].type = type;
	info[j].attacking = pa->attacking;
	info[j].state = pa->state;
	info[j].cur = pa->hit;
	info[j].max = pa->max_hit;
      }
      else if (info[j].type != type
	       || str_cmp(info[j].long_desc, pa->long_desc)
	       || info[j].attacking != pa->attacking
	       || info[j].state != pa->state
	       || str_cmp(info[j].cabal, pa->pCabal->who_name))
	continue;
      else if (info[j].fGar && !IS_SET(pa->army_flags, ARMY_FLAG_GARRISON))
	continue;
      info[j].type = type;
      info[j].sub++;
      info[j].fGar = IS_SET(pa->army_flags, ARMY_FLAG_GARRISON) ? TRUE : FALSE;
      if (info[j].cur == 0 || pa->hit < info[j].cur){
	info[j].cur = pa->hit;
	info[j].max = pa->max_hit;
      }
      break;
    }
  }
  if (!bas && !def && !att)
    return;

  /* if there is a bastion, compound data from defenders into it */
  if (bas && def){
    for (i = INROOM_DEFENDERS; i < INROOM_ATTACKERS; i++){
      if (info[i].type == INROOM_DEFENDERS && info[i].fGar){
	info[i].long_desc = NULL;
	info[0].sub += info[i].sub;
	info[0].attacking = info[i].attacking;
	info[0].state = info[i].state;
	if (info[i].cur < info[0].cur){
	  info[0].cur = info[i].cur;
	  info[0].max = info[i].max;
	}
      }
    }
  }
  /* show each individual info record which has a string */
  buffer = new_buf();
  for (i = 0; i < INROOM_MAXARMIES; i++){
    if (IS_NULLSTR(info[i].long_desc))
      continue;
    if (info[i].sub > 1){
      sprintf(sub_buf, "(%d)", info[i].type == INROOM_BASTION ? info[i].sub - 1 : info[i].sub);
    }
    else
      sprintf(sub_buf, "   ");

    if (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_AREA))
      sprintf(cab_buf, "[%s]", info[i].cabal );
    else
      cab_buf[0] = 0;

    /* compose the long desc */
    if (info[i].state == AS_BESE){
      sprintf( lon_buf, "%s besieged by %s.",
	       capitalize(info[i].short_desc),
	       info[i].attacking ? info[i].attacking->pCabal->name :"Someone?");
    }
    else if (info[i].state == AS_FIGH ){
      if (info[i].type == INROOM_BASTION)
	sprintf( lon_buf, "%s being overrun by %s.",
		 capitalize(info[i].short_desc),
		 info[i].attacking->pCabal->name);
      else if (info[i].type == INROOM_DEFENDERS)
	sprintf( lon_buf, "%s defending from %s.",
		 capitalize(info[i].short_desc),
		 info[i].attacking->pCabal->name);
      else
	sprintf( lon_buf, "%s attacking %s.",
		 capitalize(info[i].short_desc),
		 info[i].attacking->pCabal->name);
    }
    else
      sprintf( lon_buf, "%s", info[i].long_desc);

    if (ch->pCabal == NULL || !IS_CABAL(ch->pCabal, CABAL_AREA)){
      sprintf(buf, "%s``\n\r",
	      lon_buf);
      add_buf( buffer, buf );
    }
    else{
      sprintf(buf, "%-15s`` %s%s %s``\n\r",
	      cab_buf,
	      short_bar(info[i].cur, info[i].max),
	      sub_buf,
	      lon_buf);
      add_buf( buffer, buf );
    }
  }
  send_to_char(buf_string(buffer),ch);
  free_buf(buffer);
}

/* updates states/combat of all armies */
void army_update(){
  ARMY_DATA* pa, *pa_next;
  CABAL_DATA* pCab;
  int hash;

  army_combat_update();



  for (pCab = cabal_list; pCab; pCab = pCab->next ){
    pCab->support[ECO_LAST_ARMY] = 0;
  }

  for (hash = 0; hash < ARMY_HASH_SIZE; hash++){
    for (pa = army_list[hash]; pa; pa = pa_next){
      pa_next = pa->next;

      /* add this armie's support if it is in a room and a garrison */
      if (pa->in_room && pa->in_slot >= 0 && pa->pCabal
	  && IS_ARMY(pa, ARMY_FLAG_GARRISON)){
	pa->pCabal->support[ECO_LAST_ARMY] += get_army_support( pa, pa->in_room->area);
      }
      /* ROOM REQUIRED UPDATES */
      if (pa->in_room
	  && pa->state != AS_BESE
	  && pa->state != AS_FIGH){
	if (pa->state == AS_DISB)
	  pa->hit = UMAX(0, pa->hit - pa->max_hit / 5);
	/* hp gain */
	else if (pa->hit < pa->max_hit
		 && can_reinforce( pa->pCabal, pa->in_room->area )){
	  /* bastions reinforce a bit slower */
	  if (pa->pIndexData->type == ARMY_TYPE_BASTION)
	    pa->hit = UMIN(pa->max_hit, 1 + pa->hit + pa->max_hit / 20);
	  else
	    pa->hit = UMIN(pa->max_hit, 1 + pa->hit + pa->max_hit / 10);
	}
      }

      /* If lifetime > 0 decrease count and do not check *
       * If lifetime == 0 check
       * If lifetime < 0 skip
       */

      if (pa->lifetime > 0)
	pa->lifetime --;
      if (pa->cabal_ai_life > 0)
	pa->cabal_ai_life --;

      /* check lifetime once  for decision making */
      if (pa->lifetime == 0){
	army_ai( pa );
      }
      /* and again for actual action */
      if (pa->lifetime == 0){
	army_action( pa );
      }
    }//end for each army in the list
  }//end for each hash cell
}

//echos a combat string based on armies fighting in the room
//called from room update in db.c
struct battle_info{
  CABAL_DATA* pAtt;
  CABAL_DATA* pDef;
  bool fSiege;
};
void army_battle_echo( ROOM_INDEX_DATA* room ){
  const int battle_max = 6;
  const int siege_max = 5;
  char* battle_msg[] =
  {
    "With a thunderous roar the armies meet and clash.",
    "As $t and $T battle screams of the wounded fill the area.",
    "$t's armies charge at $T with a mighty cry.",
    "$t's forces surge forward forcing $T to retreat.",
    "$t's forces fall back before $T's offense.",
    "The clash of metal fills the air as the battle rages.",
  };
  char* siege_msg[] =
  {
    "The ground shakes as $t siege engines take new position.",
    "Death errupts amidst $T's defenders as siege engines fire.",
    "$t's forces break like a wave against the defenses.",
    "$t's armies charge at $T defenses.",
    "Battlehorns bellow as $t regroups for another attack.",
  };

  ARMY_DATA* pa;
  char* msg;
  struct battle_info battles[INROOM_MAXARMIES];
  int i, j;

  if (room->people == NULL || room->room_armies.count < 1)
    return;

  memset( &battles, 0, sizeof( struct battle_info ) * INROOM_MAXARMIES );

  /* we run through any attackers in the room, marking cabas that fight */
  for (i = INROOM_ATTACKERS; i < INROOM_MAXARMIES; i++){
    if ( (pa = room->room_armies.armies[i]) == NULL)
      continue;
    else if (pa->attacking == NULL)
      continue;

    /* run through the battle info and fill in a non duplicate spot */
    for (j = 0; j < INROOM_MAXARMIES; j++){
      //initial entry
      if (battles[j].pAtt == NULL){
	battles[j].pAtt = pa->pCabal;
	battles[j].pDef = pa->attacking->pCabal;
	if (IS_ARMY(pa->attacking, ARMY_FLAG_GARRISON)
	    || pa->attacking->pIndexData->type == ARMY_TYPE_BASTION)
	  battles[j].fSiege = TRUE;
	break;
      }
      //matching entry
      else if (battles[j].pAtt == pa->pCabal
	       && battles[j].pDef == pa->attacking->pCabal){
	if (IS_ARMY(pa->attacking, ARMY_FLAG_GARRISON)
	    || pa->attacking->pIndexData->type == ARMY_TYPE_BASTION)
	  battles[j].fSiege = TRUE;
	break;
      }
      else
	break;
    }
  }

  /* spit out a message for each pair of cabals */
  for (j = 0; j < INROOM_MAXARMIES; j++){
    if (battles[j].pAtt == NULL)
      break;
    if (battles[j].fSiege)
      msg = siege_msg[rand() % siege_max];
    else
      msg = battle_msg[rand() % battle_max];
    act(msg,room->people, battles[j].pAtt->name, battles[j].pDef->name, TO_ALL );
  }
}

/* save all armies in the game */
void save_armies(){
  FILE* fp;
  char path[MIL];

  log_string("Saving Army Data...");
  fclose(fpReserve);
  sprintf(path, "%s%s",SAVEARMY_DIR, SAVEARMY_TEMPFILE);

  if ((fp = fopen(path,"w")) == NULL){
    bug("save_armies: could not open SAVEARMY_TEMPFILE for write.", 0);
    perror(path);
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }

/* have save file to be written too */
  if (!fwrite_armies( fp )){
    bug("save_armies: Error occured. Removing tempfile.", 0);
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    remove( path );
    log_string("Saving Armies: FAILURE...");

  }
  else{
    char truepath[MIL];
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    sprintf(truepath, "%s%s",SAVEARMY_DIR, SAVEARMY_FILE);
    rename( path, truepath );
  }
}

/* loads all armies in the game */
void load_armies( ){
  FILE* fp;
  char path[MIL];

  fclose(fpReserve);
  sprintf(path, "%s%s",SAVEARMY_DIR, SAVEARMY_FILE);

  if ((fp = fopen(path,"r")) == NULL){
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }

  fread_armies( fp );
  fix_armies();
  fread_influences( fp );
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
}


TOWER_DATA* new_tower(){

  TOWER_DATA *ptow = malloc( sizeof( *ptow ));

  ptow->next	= NULL;
  ptow->pCabal	= NULL;
  ptow->pArea	= NULL;
  ptow->towers	= 0;
  ptow->fRein	= FALSE;

  return ptow;
}

void free_tower( TOWER_DATA* ptow ){
  free( ptow );
}


void add_tower( TOWER_DATA* tower, AREA_DATA* area ){
  TOWER_DATA* ptow = area->towers;

  if (area->towers == NULL){
    area->towers = tower;
    tower->next = NULL;
  }
  else{
    while( ptow->next ){
      ptow = ptow->next;
    }

    ptow->next = tower;
    tower->next = NULL;
  }
  tower->pArea = area;
}

bool rem_tower( TOWER_DATA* tower, AREA_DATA* area ){
  TOWER_DATA* prev = area->towers;

  if (area->towers == NULL){
    bug("rem_tower: area towers null.", 0);
    return FALSE;
  }
  if (area->towers == tower )
    area->towers = tower->next;
  else{
    while (prev->next && prev->next != tower){
      prev = prev->next;
    }
  }
  if (prev == NULL){
    bug("rem_tower: tower not found.", 0);
    return FALSE;
  }
  prev->next = tower->next;
  tower->next = NULL;
  tower->pArea = NULL;
  return TRUE;
}

TOWER_DATA* get_tower_data( CABAL_DATA* pCabal, AREA_DATA* area ){
  TOWER_DATA* ptow;

  for (ptow = area->towers; ptow; ptow = ptow->next){
    if (ptow->pCabal == pCabal)
      return ptow;
  }
  return NULL;
}

/* changes the alliegence of given area and announces it */
void area_to_cabal( AREA_DATA* area, CABAL_DATA* pc, bool fSilent ){
  char buf[MIL];
  DESCRIPTOR_DATA* d;

  if (area == NULL || pc == NULL)
    return;
  else if (area->pCabal == pc )
    return;
  else if (area->pCabal == NULL){
    if (!fSilent){
      sprintf(buf, "\n\rA messenger hands you a scroll '[%s] has conquered %s!'\n\r",
	      pc->who_name, area->name);
      for ( d = descriptor_list; d; d = d->next ){
	if ( d->connected == CON_PLAYING ){
	  send_to_char( buf, d->character );
	}
      }
    }
    area->pCabal = pc;
/* add the support of this area to sup income */
    SUPPORT_GAIN(pc, ECO_INCOME, area->support );
  }
  else{
    if (!fSilent){
      sprintf(buf, "\n\rA messenger hands you a scroll '[%s] has taken %s from [%s]!'\n\r",
	      pc->who_name, area->name, area->pCabal->who_name);
      for ( d = descriptor_list; d; d = d->next ){
	if ( d->connected == CON_PLAYING ){
	  send_to_char( buf, d->character );
	}
      }
    }
/* remove the support of this area from sup income */
    SUPPORT_GAIN(area->pCabal, ECO_INCOME, -area->support );

/* remove reinforcement from this area, and any other connecting to it */
    unreinforce_area( area->pCabal, area, FALSE );

    area->pCabal = pc;
/* add the support of this area to sup income */
    SUPPORT_GAIN(area->pCabal, ECO_INCOME, area->support );
  }
  reinforce_area( pc, area );
}

/* changes the alliegence of given area to neutral */
void area_from_cabal( AREA_DATA* area,  bool fSilent ){
  CABAL_DATA* pc = NULL;
  char buf[MIL];
  DESCRIPTOR_DATA* d;

  if (area == NULL)
    return;
  else if (area->pCabal == NULL )
    return;
  else{
    if (!fSilent){
      sprintf(buf, "\n\rA messenger hands you a scroll '%s has successfuly revolted against [%s]!'\n\r",
	      area->name, area->pCabal->who_name);
      for ( d = descriptor_list; d; d = d->next ){
	if ( d->connected == CON_PLAYING ){
	  send_to_char( buf, d->character );
	}
      }
    }
    SUPPORT_GAIN(area->pCabal, ECO_INCOME, area->support );
    pc = get_parent( area->pCabal );
    area->pCabal = NULL;
  }
/* remove reinforcement from this area, and any other connecting to it */
  unreinforce_area( pc, area, FALSE );

}

TOWER_DATA* tower_to_area( CABAL_DATA* pCabal, AREA_DATA* area, bool fSilent ){
  CABAL_DATA* pc = get_parent( pCabal );
  TOWER_DATA* ptow;

  if (pc == NULL)
    return NULL;

  if ( (ptow = get_tower_data(pc, area)) != NULL){
    ptow->towers ++;
  }
  else{
    ptow = new_tower();

    ptow->pCabal	= pc;
    ptow->pArea		= area;
    ptow->towers	++;
    add_tower(ptow, area );
  }
  area->bastion_current = UMIN(area->bastion_current + 1, area->bastion_max);

/* check if we own majority */
  if ( (area->bastion_max - ptow->towers) < ptow->towers ){
    area_to_cabal(area, pc, fSilent );
    //refresh global reinforcements
    refresh_cabal_support();
  }
  return ptow;
}

/* reinforces the area, if owned by friendly reinforces all connected, unreinforced, unmarked areas */
void reinforce_area( CABAL_DATA* pCabal, AREA_DATA* area ){
  CABAL_DATA* pc = get_parent( pCabal );
  TOWER_DATA* ptow;
  EXIT_DATA* pe;

  if (pc == NULL)
    return;

  if ( (ptow = get_tower_data(pc, area)) != NULL){
    ptow->fRein  = TRUE;
  }
  else{
    ptow = new_tower();

    ptow->pCabal	= pc;
    ptow->pArea		= area;
    ptow->towers	= 0;
    ptow->fRein		= TRUE;
    add_tower(ptow, area );
  }

  /* mark this area */
  SET_BIT(area->area_flags, AREA_MARKED);

  /* if this area is friendly reinforce connected areas */
  if ( area->pCabal != NULL
       && is_friendly(area->pCabal, pCabal) == CABAL_FRIEND){
    /* check each exit from this area */
    for (pe = area->exits; pe; pe = pe->next_in_area ){
      if (pe->to_room == NULL || pe->to_room->area == NULL || pe->to_room->area == area)
	continue;
      if (IS_AREA(pe->to_room->area, AREA_MARKED))
	continue;
      /* do not check areas already reinforced */
      if ( (ptow = get_tower_data(pc, pe->to_room->area)) != NULL
	   && ptow->fRein)
	continue;

      /* reinforce this areas now */
      reinforce_area( pCabal, pe->to_room->area);
    }//end for
  }//END REINFORCE AREA CONNECTED TO THIS AREA

  REMOVE_BIT(area->area_flags, AREA_MARKED);
  return;
}

void tower_from_area( CABAL_DATA* pCabal, AREA_DATA* area ){
  CABAL_DATA* pc = get_parent( pCabal );
  TOWER_DATA* ptow;

  if (pc == NULL)
    return;

  if ( (ptow = get_tower_data(pc, area)) == NULL){
    bug("tower_from_area: could not find a tower for cabal %d", pc->vnum );
    return;
  }

  if (--ptow->towers < 0){
    ptow->towers = 0;
  }
  if (--area->bastion_current < 0)
    area->bastion_current = 0;
}

void reinforce_refresh( CABAL_DATA* pCabal ){
  AREA_DATA* pa;

  if (pCabal == NULL
      || pCabal->anchor == NULL
      || !IS_CABAL(pCabal, CABAL_AREA))
    return;

  /* reset this cabal's reinforcement on ALL areas */
  for (pa = area_first; pa; pa = pa->next ){
    if (pa->towers)
      unreinforce_area( pCabal, pa, TRUE );
  }

  /* start the chain reaction in the cabal */
  reinforce_area( pCabal, pCabal->anchor->area );
}

/* returns true if a refresh is needed */
bool unreinforce_area( CABAL_DATA* pCabal, AREA_DATA* area, bool fShort ){
  CABAL_DATA* pc = get_parent( pCabal );
  CABAL_DATA* pCab;
  TOWER_DATA* ptow;
  EXIT_DATA* pe;
  int cnt = 0;

  if (pc == NULL)
    return FALSE;

  if ( (ptow = get_tower_data(pc, area)) == NULL){
    return FALSE;
  }
  else
    ptow->fRein = FALSE;


  if (fShort)
    return FALSE;

  /* we now count how many reinforced areas this area connected to */
  for (pe = area->exits; pe; pe = pe->next_in_area ){
    if (pe->to_room == NULL || pe->to_room->area == NULL)
      continue;

    if ( (ptow = get_tower_data(pc, pe->to_room->area)) != NULL)
      cnt ++;
  }

  if (cnt > 1){
    for (pCab = cabal_list; pCab; pCab = pCab->next){
      if (!IS_CABAL(pCab, CABAL_AREA) || IS_CABAL(pCab, CABAL_SUBCABAL) || pCab->anchor == NULL)
	continue;
      reinforce_refresh( pCab );
    }
  }
  return cnt;
}

/* checks if a given cabal can reinforce the area with troops *
   - If owned by any cabal, then friendly cabals have reinforcement
   - otherwise check for flag on tower data
*/
bool can_reinforce( CABAL_DATA* pCabal, AREA_DATA* area ){
  TOWER_DATA* pt;
  CABAL_DATA* pCab = get_parent(pCabal);
/*
  if (mud_data.mudport == TEST_PORT)
    return TRUE;
  else
*/
  if (pCab == NULL || area == NULL)
    return FALSE;
  else if ( (pt = get_tower_data(pCab, area)) != NULL && pt->fRein)
    return TRUE;
  else
    return FALSE;
}

/* used on reboot and when bastion max is changed in olc */
void refresh_area_support(){
  AREA_DATA* pa;

  /* first we get the total number of bastions in the game */
  mud_data.tot_bastion = 1;

  for (pa = area_first; pa; pa = pa->next){
    mud_data.tot_bastion += pa->bastion_max;
  }

  /* now we run thruogh the areas and calculate the support values */
  for (pa = area_first; pa; pa = pa->next){
    pa->support = pa->bastion_max * WORLD_SUPPORT_VALUE / mud_data.tot_bastion;
  }

/* done */
}

/* returns a pointer to army if one is present in this room */
ARMY_DATA* get_army_room( CHAR_DATA* ch, ROOM_INDEX_DATA* room,char* name){
  CHAR_DATA* vch = ch;
  int i;

  if (room->room_armies.count < 1)
    return NULL;
  for (i = INROOM_BASTION; i < INROOM_MAXARMIES; i++){
    if (room->room_armies.armies[i] == NULL)
      continue;
    else if (!is_name(name, room->room_armies.armies[i]->noun))
      continue;
    else
      return (room->room_armies.armies[i]);
  }

  //auto
  if (IS_AFFECTED(ch, AFF_CHARM) && ch->leader)
    vch = ch->leader;
  else
    vch = ch;
  if (!IS_GAME(vch, GAME_AMOB))
    return NULL;
  for (i = INROOM_BASTION; i < INROOM_MAXARMIES; i++){
    if (room->room_armies.armies[i] == NULL)
      continue;
    else if (!is_auto_name(name, room->room_armies.armies[i]->noun))
      continue;
    else
      return (room->room_armies.armies[i]);
  }
  return NULL;
}

/* OLD */
/* removes any pExAlly armies garrisoned in pCab's areas with pCab's bastions */
bool break_alliance_check( CABAL_DATA* pCab, CABAL_DATA* pExAlly ){
  //  ARMY_DATA* pa;
  bool fFound = FALSE;

  /* run through all armies of pExAlly cabal */
  /* check if the army is in pCab area */

  return fFound;
}

