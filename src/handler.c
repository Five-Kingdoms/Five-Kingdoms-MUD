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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "cabal.h"
#include "magic.h"
#include "recycle.h"
#include "interp.h"
#include "bounty.h"
#include "vote.h"
#include "save_mud.h"
#include "armies.h"
#include "comm.h"

// change the number as you add more ranges
#define GIMP_NUM	5

extern		int              nAllocString;
extern		int              sAllocString;
extern void unadorn_obj (OBJ_DATA* socket, OBJ_DATA* obj, CHAR_DATA *ch);
extern void adorn_obj (OBJ_DATA* socket, OBJ_DATA* obj, CHAR_DATA *ch);
int                     nAllocPerm;
int                     sAllocPerm;


/* removes a trap from the game */
void extract_trap(TRAP_DATA* trap){

  if (trap == NULL)
    return;
  if ( trap_list == trap )
    trap_list = trap->next;
  else{
    TRAP_DATA *prev;
    for ( prev = trap_list; prev != NULL; prev = prev->next ){
      if ( prev->next == trap ){
	prev->next = trap->next;
	break;
      }
    }
    if ( prev == NULL ){
      bug( "Extract_trap: trap not found.", 0 );
      return;
    }
  }
  if (trap->owner)
    trap_from_char( trap );
  if (trap->on_obj)
    trap_from_obj( trap );
  if (trap->on_exit)
    trap_from_exit( trap );

/* done */
  free_trap(trap);
}

/* inserts a given trap onto a list of character's traps */
void trap_to_char( TRAP_DATA* trap, CHAR_DATA* ch){
  int i = 0;
  int age = 0;
  int spot = 0;

  if (trap->owner)
    trap_from_char( trap );
  else
    trap->owner = ch;

  /* we find first empty spot, or the oldest one goes */
  for (i = 0; i < MAX_TRAPS; i++){
    if (ch->traps[i] == NULL){
      ch->traps[i] = trap;
      return;
    }
    /* get the smallest duration while we are doing this */
    if (ch->traps[i]->age >= 0 && ch->traps[i]->age > age){
      age = ch->traps[i]->age;
      spot = i;
    }
  }
  /* no empt spots found, we replace the one with least time left */
  extract_trap(ch->traps[spot]);
  ch->traps[spot] = trap;
}

/* removes a trap from character's list of traps */
void trap_from_char( TRAP_DATA* trap ){
  int i = 0;
  if (trap->owner == NULL){
    bug("trap_from_char: trap has no owner.", 0 );
    return;
  }
  for (i = 0; i  < MAX_TRAPS; i++){
    if (trap->owner->traps[i] == trap)
      trap->owner->traps[i] = NULL;
  }
  trap->owner = NULL;
  return;
}

/* attaches a trap onto an object */
void trap_to_obj( TRAP_DATA* trap, OBJ_DATA* obj ){
  trap->on_obj = obj;
  trap->next_trap = obj->traps;
  obj->traps = trap;
}

/* removes a trap from an object */
void trap_from_obj( TRAP_DATA* trap ){
  OBJ_DATA* obj;
  if ( (obj = trap->on_obj) == NULL){
    bug("trap_from_obj: trap not on an object.", 0);
    return;
  }
/* ez case */
  if (trap == obj->traps)
    obj->traps = NULL;
  else{
    TRAP_DATA* prev;
    for (prev = obj->traps; prev; prev = prev->next_trap){
      if (prev->next_trap == trap){
	prev->next_trap = trap->next_trap;
	break;
      }
    }
    if (prev == NULL){
      bug("trap_from_obj: trap not on the list.", 0);
      return;
    }
  }
  trap->on_obj = NULL;
  trap->next_trap = NULL;
}

/* attaches a trap onto an exit */
void trap_to_exit( TRAP_DATA* trap, EXIT_DATA* exit ){

  trap->on_exit = exit;
  trap->next_trap = exit->traps;
  exit->traps = trap;
}

/* removes a trap from an object */
void trap_from_exit( TRAP_DATA* trap ){
  EXIT_DATA* exit;
  if ( (exit = trap->on_exit) == NULL){
    bug("trap_from_exit: trap not on an exit.", 0);
    return;
  }
/* ez case */
  if (trap == exit->traps)
    exit->traps = NULL;
  else{
    TRAP_DATA* prev;
    for (prev = exit->traps; prev; prev = prev->next_trap){
      if (prev->next_trap == trap){
	prev->next_trap = trap->next_trap;
	break;
      }
    }
    if (prev == NULL){
      bug("trap_from_exit: trap not on the list.", 0);
      return;
    }
  }
  trap->on_exit = NULL;
  trap->next_trap = NULL;
}

/* returns an object that and object is in */
inline OBJ_DATA* in_obj(OBJ_DATA* obj){
  if (obj->in_obj == NULL)
    return obj;
  else
    return in_obj(obj->in_obj);
}

/* returns the character if any that this object is on*/
inline CHAR_DATA* in_char(OBJ_DATA* obj){
  OBJ_DATA* vobj = in_obj(obj);
  return vobj->carried_by;
}


/* OBJTCHAR_CHECK */
/* Called AFTER item is taken by a char. */
inline int OBJTOCHAR_CHECK( CHAR_DATA *ch, OBJ_DATA *obj)
{
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int result = 0;

  int fSuccess = TRUE;
//OBJECT ORIENTATED EFFECS>
  if (obj == NULL) {
    return fSuccess;
  }

//HAS_OWNER
  if (IS_SET(obj->wear_flags, ITEM_HAS_OWNER)) {
    fSuccess = run_effect_update(ch, NULL, obj, gen_has_owner, NULL,
				 NULL, TRUE, EFF_UPDATE_OBJTOCHAR);
  }

  for (paf = obj->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }

//GENS THAT ARE HANDLED SEPERATLY
    if (paf->type == gen_has_owner) {
      continue;
    }

    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(NULL, NULL, obj, paf->type, NULL,
				      NULL, TRUE, EFF_UPDATE_OBJTOCHAR)) != TRUE) {
	if (result == -1) {
	  return -1;
	}
	else {
	  fSuccess = result;
	}
      }
    }
  }

  return fSuccess;

}//end OBJTOCHAR_CHECK



/* LEAVE_CHECK */
/*called in handler.c only */
int LEAVE_CHECK(CHAR_DATA* ch, ROOM_INDEX_DATA* in_room)
{
  //This is the function for various checks on leace room.
  //there is 2 parts.  Checks done regardless of SPEC_UPDATE
  //and checks done ONLY with SPEC_UPDATE.

  CHAR_DATA* vch = ch;

  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int result = 0;

  /* Useless conditional */
  int fSuccess = TRUE;

  if( fSuccess == FALSE )
    return -1;

//CHAR EFFECTS

/* if trust is that of a mproged mob we return */
  if (ch->trust == 6969)
    return TRUE;
  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }

    //GENS THAT ARE HANDLED SEPERATLY
    if (paf->type == gen_gold_loss) {
      run_effect_update(vch, paf, NULL, paf->type, NULL, NULL, FALSE, EFF_UPDATE_LEAVE);
      continue;
    }

    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(vch, paf, in_room, paf->type,
				      NULL, NULL, TRUE, EFF_UPDATE_LEAVE)) != TRUE) {
	if (result == -1) {
	  return -1;
	}
	else {
	  fSuccess = result;
	}
      }
    }
  }


  //------PART 2----------/
  if (!IS_SET(in_room->room_flags2, ROOM_SPEC_UPDATE)) {
    return TRUE;
  }

//start SPEC_UPDATE updates here.//

//Firestorm
  if (is_affected(ch, gen_firestorm)) {
    run_effect_update(ch, in_room, NULL, gen_firestorm, NULL, NULL, TRUE, EFF_UPDATE_LEAVE);
  }

//Update the status of the spec flag.
  room_spec_update(in_room);
  return TRUE;
}//end LEAVE CHECK



/* ENTER_CHECK */
/*called in handler.c only*/
int ENTER_CHECK(CHAR_DATA* ch, ROOM_INDEX_DATA* to_room)
{
  //this is where checks are done when char enters a room.
  //there is 2 parts.  Checks done regardless of SPEC_UPDATE
  //and checks done ONLY with SPEC_UPDATE.

  CHAR_DATA* vch = ch;

  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int result = 0;

  /* Useless conditional */
  int fSuccess = TRUE;

  if( fSuccess == FALSE )
    return -1;

  /* cabal item handling */
  if (to_room->pCabal && to_room->pCabal->anchor && to_room->pCabal->anchor == to_room)
    handle_cabal_item_entrance( ch, to_room );

  /* if trust is that of a mproged mob we return */
  if (ch->trust == 6969)
    return TRUE;
//---------PART 1--------/
//CHAR EFFECTS

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }

    //GENS THAT ARE HANDLED SEPERATLY

    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(vch, NULL, to_room, paf->type, NULL,
				      NULL, TRUE, EFF_UPDATE_ENTER)) != TRUE) {
	if (result == -1) {
	  return -1;
	}
	else {
	  fSuccess = result;
	}
      }
    }
  }

//------PART 2----------/
  if (!IS_SET(to_room->room_flags2, ROOM_SPEC_UPDATE)) {
    return TRUE;
  }

//start SPEC_UPDATE updates here.//
//Firestorm & ANGER

  for (vch = to_room->people; vch != NULL; vch = vch->next_in_room) {
    if (is_affected(vch, gen_firestorm)) {
      run_effect_update(vch, ch, to_room, gen_firestorm, NULL, NULL, TRUE, EFF_UPDATE_ENTER);
    }
    if (is_affected(vch, gen_anger)
	&& !IS_AFFECTED(ch, AFF_SNEAK)
	&& !sneak_fade_check(ch) ) {
      run_effect_update(vch, ch, to_room, gen_anger, NULL, NULL, TRUE, EFF_UPDATE_ENTER);
    }
  }
  return TRUE;
}//end ENTER CHECK



/* OBJKILL_CHECK */
/* called in handler.c only */

int OBJKILL_CHECK(OBJ_DATA* obj)
{

  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int result = 0;
  int fSuccess = TRUE;

  //runs through nay gen checks needed when obj is destroyed.
  //OBJECT ORIENTATED EFFECS>
  if (obj == NULL ) {
    return fSuccess;
  }

  for (paf = obj->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }

    //GENS THAT ARE HANDLED SEPERATLY

    if (IS_GEN(paf->type)) {
      if ( !(IS_SET(effect_table[GN_POS(paf->type)].flags, EFF_F1_NOEXTRACT) && obj->extracted == TRUE)
	   && (result = run_effect_update(NULL, paf, obj, paf->type, NULL,
					  NULL, FALSE, EFF_UPDATE_KILL)) != TRUE) {
	if (result == -1) {
	  return -1;
	}
	else {
	  fSuccess = result;
	}
      }
    }
  }
  return fSuccess;
}//end OBJ_KILL


/* returns weight of object as chagned by character traits */
/* used only when put/removed from char */
inline int get_obj_weight_char( CHAR_DATA* ch, OBJ_DATA* obj){
  int weight = get_obj_weight( obj );

  if (obj->item_type == ITEM_ARMOR){
    bool fBasic = get_skill(ch, gsn_basic_armor) > 1;
    bool fAdv = get_skill(ch, gsn_adv_armor) > 1;
    if (fBasic && fAdv)
      weight	= 6 * weight / 10;
    else if (fBasic || fAdv)
      weight	= 8 * weight / 10;
  }

  return weight;
}

void do_autos( CHAR_DATA *ch )
{
    OBJ_DATA *corpse;
    if (ch->in_room == NULL)
	return;
    if (!IS_NPC(ch) && (corpse = get_obj_list(ch,"corpse",ch->in_room->contents)) != NULL
    && corpse->item_type == ITEM_CORPSE_NPC && can_see_obj(ch,corpse))
    {
        OBJ_DATA *coins;
        corpse = get_obj_list( ch, "corpse", ch->in_room->contents );
        if ( IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains) {
	    do_get( ch, "all corpse" );
	}
        if (IS_SET(ch->act,PLR_AUTOGOLD) &&
	    corpse && corpse->contains && !IS_SET(ch->act,PLR_AUTOLOOT)) {
	  if ((coins = get_obj_list(ch,"gcash",corpse->contains)) != NULL) {
	    do_get(ch, "all.gcash corpse");
	  }
	}
	if ( IS_SET(ch->act, PLR_AUTOSAC) ) {
	  if ( IS_SET(ch->act,PLR_AUTOLOOT) && corpse && corpse->contains) {
	    return;
	  }
	  else {
	    do_sacrifice( ch, "corpse" );
	  }
	}
    }
}

void do_obj_spell( OBJ_DATA *obj, CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_SPELL_DATA *spell, *start;
    CHAR_DATA *to, *to_next;

    if (obj->eldritched)
      start = obj->spell;
    else
      start = obj->pIndexData->spell;

    for ( spell = start; spell; spell = spell->next )
        if ( number_percent( ) <= spell->percent )
        {
	    for ( to = ch->in_room->people; to != NULL; to = to_next )
	    {
	        to_next = to->next_in_room;
	        if (can_see_obj( to, obj ))
		  act(spell->message, to, obj, NULL, TO_CHAR);
		else
		  act(spell->message2, to, obj, NULL, TO_CHAR);

	    }
            if (spell->target== SPELL_TAR_SELF)
                obj_cast_spell( spell->spell, (obj->level + ch->level)/2, ch, ch, NULL );
            if (spell->target== SPELL_TAR_VICT)
                obj_cast_spell( spell->spell,
		(ch->level / 3 + 3 * obj->level / 5), ch, victim, NULL );
        }
}

int flag_lookup (const char *name, const struct flag_type *flag_table)
{
    int flag;
    for (flag = 0; flag_table[flag].name != NULL; flag++)
        if (LOWER(name[0]) == LOWER(flag_table[flag].name[0]) && !str_prefix(name,flag_table[flag].name))
	    return flag_table[flag].bit;
    return 0;
}

/* returns position a a vnum in seed table */
int objseed_lookup (int vnum){
  int i;
  for (i = 0; objseed_table[i].vnum != 0; i++){
    if (objseed_table[i].vnum == vnum)
      return i;
  }
  return -1;
}

int position_lookup (const char *name)
{
   int pos;
   for (pos = 0; position_table[pos].name != NULL; pos++)
        if (LOWER(name[0]) == LOWER(position_table[pos].name[0]) && !str_prefix(name,position_table[pos].name))
	    return pos;
   return -1;
}

int sex_lookup (const char *name)
{
   int sex;
   for (sex = 0; sex_table[sex].name != NULL; sex++)
        if (LOWER(name[0]) == LOWER(sex_table[sex].name[0]) && !str_prefix(name,sex_table[sex].name))
	    return sex;
   return 0;
}

int ethos_lookup (const char *name)
{
   int ethos;
   for (ethos = 0; ethos_table[ethos].name != NULL; ethos++)
        if (LOWER(name[0]) == LOWER(ethos_table[ethos].name[0]) && !str_prefix(name,ethos_table[ethos].name))
	    return ethos;
   return -1;
}

int anatomy_lookup (const char *name)
{
  /* caching for faster track prints, we check a static pointer since the name is almost always from const table lookup */
  static char* last_name = NULL;
  static int last_ana = 0;
  int ana;

  if (name == last_name)
    return last_ana;

  for (ana = 0; anatomy_table[ana].name != NULL; ana++){
    if (LOWER(name[0]) == LOWER(anatomy_table[ana].name[0]) && !str_prefix(name,anatomy_table[ana].name)){
      last_name = anatomy_table[ana].name;
      last_ana = ana;
      return ana;
    }
  }
  return 0;
}

int size_lookup (const char *name)
{
   int size;
   for ( size = 0; size_table[size].name != NULL; size++)
        if (LOWER(name[0]) == LOWER(size_table[size].name[0]) && !str_prefix( name,size_table[size].name))
            return size;
   return -1;
}

int material_lookup (const char *name)
{
    return 0;
}

int race_lookup (const char *name)
{
   int race;
   for ( race = 0; race_table[race].name != NULL; race++)
        if (LOWER(name[0]) == LOWER(race_table[race].name[0]) && !str_prefix( name,race_table[race].name))
	    return race;
   return 0;
}

int liq_lookup (const char *name)
{
    int liq;
    for ( liq = 0; liq_table[liq].liq_name != NULL; liq++)
        if (LOWER(name[0]) == LOWER(liq_table[liq].liq_name[0]) && !str_prefix(name,liq_table[liq].liq_name))
	    return liq;
    return -1;
}

int weapon_lookup (const char *name)
{
    int type;
    for (type = 0; weapon_table[type].name != NULL; type++)
        if (LOWER(name[0]) == LOWER(weapon_table[type].name[0]) && !str_prefix(name,weapon_table[type].name))
	    return type;
    return -1;
}


int trap_lookup (const char *name){
  int i;
  for (i = 0; trap_table[i].name != NULL; i++){
    if (LOWER(name[0]) == LOWER(trap_table[i].name[0])
	&& !str_cmp(name, trap_table[i].name))
      return i;
  }
  return 0;
}

int sheath_lookup (const char *class){
  int i;
  for (i = 0; sheath_table[i].class != NULL; i++){
    if (LOWER(class[0]) == LOWER(sheath_table[i].class[0])
	&& !str_cmp(class, sheath_table[i].class))
      return i;
  }
  return 0;
}

int damtype_lookup (const char *name){
  int i;
  for (i = 0; damtype_table[i].name != NULL; i++){
    if (LOWER(name[0]) == LOWER(damtype_table[i].name[0])
	&& !str_cmp(name, damtype_table[i].name))
      return i;
  }
  return -1;
}

int item_lookup(const char *name)
{
    int type;
    for (type = 0; item_table[type].name != NULL; type++)
        if (LOWER(name[0]) == LOWER(item_table[type].name[0]) && !str_prefix(name,item_table[type].name))
            return item_table[type].type;
    return -1;
}

int attack_lookup  (const char *name)
{
    int att;
    for ( att = 0; attack_table[att].name != NULL; att++)
        if (LOWER(name[0]) == LOWER(attack_table[att].name[0]) && !str_prefix(name,attack_table[att].name))
	    return att;
    return 0;
}

int lagattack_lookup  (int sn)
{
    int att;
    for ( att = 0; lagattack_table[att].gsn != NULL; att++)
      if (*lagattack_table[att].gsn == sn)
	return att;
    return -1;
}

inline char* get_vir_attack(int dt)
{
  if (dt >= 5000 && dt <= 5000 + MAX_SONG)
    return song_table[dt - 5000].noun_damage;
  else if (dt >= TYPE_HIT &&  dt < TYPE_HIT + MAX_DAMAGE_MESSAGE)
    return attack_table[dt - TYPE_HIT].noun;
  else if (IS_GEN(dt))
    return effect_table[GN_POS(dt)].noun_damage;
  else if (dt < MAX_SKILL)
    return skill_table[dt].noun_damage;

  return attack_table[1].noun;
}

inline char* get_skill_name(int sn)
{
  if (sn >= 5000 && sn <= 5000 + MAX_SONG)
    return song_table[sn - 5000].name;
  else if (IS_GEN(sn))
    return effect_table[GN_POS(sn)].name;
  else if (sn < MAX_SKILL)
    return skill_table[sn].name;

  return skill_table[0].name;
}


long wiznet_lookup (const char *name)
{
    int flag;
    for (flag = 0; wiznet_table[flag].name != NULL; flag++)
        if (LOWER(name[0]) == LOWER(wiznet_table[flag].name[0]) && !str_prefix(name,wiznet_table[flag].name))
	    return flag;
    return -1;
}

int class_lookup (const char *name)
{
   int class;
   for ( class = 0; class_table[class].name != NULL && class < MAX_CLASS; class++)
        if (LOWER(name[0]) == LOWER(class_table[class].name[0]) && !str_prefix( name,class_table[class].name))
            return class;
   return -1;
}

int hometown_lookup (const char *name)
{
   int hometown;
   for ( hometown = 0; hometown_table[hometown].name != NULL && hometown < MAX_HOMETOWN; hometown++)
        if (LOWER(name[0]) == LOWER(hometown_table[hometown].name[0]) && !str_prefix( name,hometown_table[hometown].name))
            return hometown;
   return -1;
}

int jail_lookup (const char *name)
{
   int jail;
   for ( jail = 0; jail_table[jail].name != NULL && jail < MAX_HOMETOWN; jail++)
        if (LOWER(name[0]) == LOWER(jail_table[jail].name[0])
	    && !str_prefix( name, jail_table[jail].name))
            return jail;
   return 0;
}

bool is_friend(CHAR_DATA *ch,CHAR_DATA *victim)
{
    if (is_same_group(ch,victim))
	return TRUE;
    if (!IS_NPC(ch))
	return FALSE;
    if (!IS_NPC(victim))
    {
	if (IS_SET(ch->off_flags,ASSIST_PLAYERS))
	    return TRUE;
	else
	    return FALSE;
    }
    if (IS_AFFECTED(ch,AFF_CHARM))
	return FALSE;
    if (IS_SET(ch->off_flags,ASSIST_ALL))
	return TRUE;
    if (ch->group && ch->group == victim->group)
	return TRUE;
    if (IS_SET(ch->off_flags,ASSIST_VNUM) && ch->pIndexData == victim->pIndexData)
	return TRUE;
    if (IS_SET(ch->off_flags,ASSIST_RACE) && ch->race == victim->race)
	return TRUE;
    if (IS_SET(ch->off_flags,ASSIST_ALIGN) && !IS_SET(ch->act,ACT_NOALIGN) && !IS_SET(victim->act,ACT_NOALIGN)
    && ((IS_GOOD(ch) && IS_GOOD(victim)) || (IS_EVIL(ch) && IS_EVIL(victim)) || (IS_NEUTRAL(ch) && IS_NEUTRAL(victim))))
	return TRUE;
    return FALSE;
}

int weapon_type (const char *name)
{
    int type;
    for (type = 0; weapon_table[type].name != NULL; type++)
        if (LOWER(name[0]) == LOWER(weapon_table[type].name[0]) && !str_prefix(name,weapon_table[type].name))
            return weapon_table[type].type;
    return WEAPON_EXOTIC;
}

char *item_name(int item_type)
{
    int type;
    for (type = 0; item_table[type].name != NULL; type++)
	if (item_type == item_table[type].type)
	    return item_table[type].name;
    return "none";
}

char *weapon_name( int weapon_type)
{
  static char* exo = "exotic";
    int type;
    for (type = 0; weapon_table[type].name != NULL; type++)
        if (weapon_type == weapon_table[type].type)
            return weapon_table[type].name;
    return exo;
}

int weapon_sn_lookup( int sn ){
  int type;
  for (type = 0; weapon_table[type].name != NULL; type++){
    if (sn == *weapon_table[type].gsn){
      return type;
    }
  }
  return -1;
}

int check_immune(CHAR_DATA *ch, int dam_type, bool fSaves)
{
  /*
HOW THIS WORKS:
DAM_NONE and DAM_INTERNAL return immediatly.
fSaves = TRUE mana shield and barrier are ignored

1)
- If damage is mundane (PIERCE/SLASH.BASH)
we take the resisance to weapon
- If damage is magical we take res. to magic.

3)
- If this is not saves check, mana shield makes target immune to
magical damage except DAM_MENTAL, and HOLY in case of UNDEAD
- If this is not saves check, barrier makes target immune to
mundane damage.

4) Special Case: If Vulnerability AND (resistance OR immunity) to SAME
element is present, then result is normal.
Ex: Vuln to Holy and Resist Holy

* THIS IS DONE AT THE END and not affected by previous
operations. (IE: Manashield gives absolute magic immunity vs damage.)
  */


  int immune = IS_NORMAL;//holds current res. state
  int  bit;//holds the damage type bit

//bools
  bool fManaSh = (dam_type > 3
		  && dam_type != DAM_MENTAL
		  && (dam_type != DAM_HOLY || !IS_UNDEAD(ch))
		  && IS_AFFECTED2(ch,AFF_MANA_SHIELD)
		  && !fSaves);
  bool fBarrier =  (dam_type <= 3
		    && dam_type != DAM_MENTAL
		    && IS_AFFECTED2(ch,AFF_BARRIER)
		    && !fSaves);

//return on internal damage.
  if ( (dam_type == DAM_NONE) || (dam_type == DAM_INTERNAL) ){
    if (IS_SET(ch->imm_flags,IMM_WEAPON) && IS_SET(ch->imm_flags,IMM_MAGIC))
      return IS_IMMUNE;
    else
      return immune;
  }
//DEFAULT RESITANCE FOR WEAPONS
  if (dam_type <= 3)
    {
      if (IS_SET(ch->imm_flags,IMM_WEAPON))
	immune = IS_IMMUNE;
      else if ( IS_SET(ch->res_flags,RES_WEAPON))
	immune = IS_RESISTANT;
      else if (IS_SET(ch->vuln_flags,VULN_WEAPON))
	immune = IS_VULNERABLE;
    }//END BASH/SLASH/PIERCE CHECK
  else
//DEFAULT RESITANCE FOR MAFGIC
    {
      //CHECK RACE FLAGS
      if (IS_SET(ch->imm_flags,IMM_MAGIC))
	immune = IS_IMMUNE;
      else if (IS_SET(ch->res_flags,RES_MAGIC))
	immune = IS_RESISTANT;
      else if (IS_SET(ch->vuln_flags,VULN_MAGIC))
	immune = IS_VULNERABLE;
    }//END MAGIC RESISTANCE CHECK

//GET SPECIFIC RESISTANCE IF ANY
    //CHECK ATTACK TYPE
  /* The IMM_, RES_ and VULN_ bits overlap */
  switch (dam_type)
    {
	case(DAM_BASH):		bit = IMM_BASH;		break;
	case(DAM_PIERCE):	bit = IMM_PIERCE;	break;
	case(DAM_SLASH):	bit = IMM_SLASH;	break;
	case(DAM_FIRE):		bit = IMM_FIRE;		break;
	case(DAM_COLD):		bit = IMM_COLD;		break;
	case(DAM_LIGHTNING):	bit = IMM_LIGHTNING;	break;
	case(DAM_ACID):		bit = IMM_ACID;		break;
	case(DAM_POISON):	bit = IMM_POISON;	break;
	case(DAM_NEGATIVE):	bit = IMM_NEGATIVE;	break;
	case(DAM_HOLY):		bit = IMM_HOLY;		break;
	case(DAM_ENERGY):	bit = IMM_ENERGY;	break;
	case(DAM_MENTAL):	bit = IMM_MENTAL;	break;
	case(DAM_DISEASE):	bit = IMM_DISEASE;	break;
	case(DAM_DROWNING):	bit = IMM_DROWNING;	break;
	case(DAM_LIGHT):	bit = IMM_LIGHT;	break;
	case(DAM_CHARM):	bit = IMM_CHARM;	break;
	case(DAM_SOUND):	bit = IMM_SOUND;	break;
        case(DAM_IRON):		bit = IMM_IRON;	        break;
	case(DAM_MITHRIL):	bit = IMM_MITHRIL;	break;
	case(DAM_SILVER):	bit = IMM_SILVER;	break;
	case(DAM_WOOD):	 	bit = IMM_WOOD;	        break;
	case(DAM_AIR):	 	bit = IMM_AIR;	        break;
    default:		return immune;
    }
    //END DAMAGE TYPE->BIT CONVERSION


    // We have the global weapon/magic res. now.
    //we check for imm/res/vuln of teh damage now.

  if (IS_SET(ch->imm_flags,bit))
      {
	//IMM + IMM = IMM*
	//IMM + RES = IMM*
	//IMM + NOR = IMM*
	//IMM + VUL = RES
	//* = identity cases.
	if (immune == IS_VULNERABLE)
	  immune = IS_RESISTANT;
	else
	  immune = IS_IMMUNE;
      }
    else if (IS_SET(ch->res_flags,bit))
      {
	//RES + IMM = IMM
	//RES + RES = RES*
	//RES + NOR = RES*
	//RES + VUL = VUL
	//* = identity cases.
	if (immune == IS_IMMUNE)
	  immune = IS_IMMUNE;
	else if (immune == IS_VULNERABLE)
	  immune = IS_VULNERABLE;
	else
	  immune = IS_RESISTANT;
      }
  else if (IS_SET(ch->vuln_flags,bit))
      {
	//VUL + IMM = VUL*
	//VUL + RES = VUL*
	//VUL + NOR = VUL*
	//VUL + VUL = VUL*
	//* = identity cases.
	immune = IS_VULNERABLE;
      }


//SPECIALS THAT OVERRIDE VULNS
  if (IS_SET(ch->vuln_flags, bit)
      && (IS_SET(ch->res_flags, bit) || IS_SET(ch->imm_flags, bit))
      && immune == IS_VULNERABLE)
    immune = IS_NORMAL;


//SPECIAL THINGS FOR VAMPS AT MIDNIGHT/MORNING
    if (ch->class == class_lookup("vampire"))
      {
	//MIST FORM IMMUNE TO WEAPON
	if (is_affected(ch, gsn_mist_form)
	    && !vamp_day_check(ch)
	    && !fSaves
	    && dam_type <= 3)
	  immune = IS_IMMUNE;
	//vamps are res weapon during midnight and not-vuln.
	else if ( (mud_data.time_info.hour <= 1 || mud_data.time_info.hour >= 23)
	     && !vamp_day_check(ch))
	  {
	    if (dam_type <= 3 && immune > IS_IMMUNE)
	      immune = IS_RESISTANT;
	    else if (immune > IS_RESISTANT)
	      immune = IS_NORMAL;
	  }
	//vuln all during morning.
	else if ((mud_data.time_info.hour >= 7 && mud_data.time_info.hour <= 10)
		 && immune > IS_IMMUNE
		 && vamp_day_check(ch))
	  immune = IS_VULNERABLE;
	//Virigoth's tat cancles light vuln
	if (ch->tattoo == deity_lookup("cycle") && dam_type == DAM_LIGHT
	    && immune > IS_NORMAL)
	  immune = IS_NORMAL;
      }//END VAMP CHECKS

//NO BASH VULN FOR FOREFIELD
    if (dam_type == DAM_BASH && is_affected(ch, gsn_forcefield)
	&& !fSaves
	&& immune > IS_RESISTANT)
      immune = IS_NORMAL;
//BRAIN DEATH REMOVES RESIST TO MENTAL DAMAGE
    if ((dam_type == DAM_MENTAL || dam_type == DAM_CHARM)
	&& immune > IS_IMMUNE
	&& immune < IS_VULNERABLE
	&& is_affected(ch, gsn_unminding)){
      immune = IS_NORMAL;
    }
//SHIELDS LAST (OVERRIDE ALL)
    if (fBarrier || fManaSh)
      immune = IS_IMMUNE;

    //REUTN RESULT
    return immune;
}

bool is_fight_delay(CHAR_DATA *ch, int time)
{
    if (IS_IMMORTAL(ch))
	return FALSE;

    if (ch->pcdata->fight_delay == (time_t)NULL && ch->pcdata->pk_delay == (time_t)NULL)
	return FALSE;
    if (ch->pcdata->fight_delay != (time_t)NULL && ch->pcdata->fight_delay >= 0)
    {
	if (difftime(mud_data.current_time,ch->pcdata->fight_delay) < 0)
	{
	    ch->pcdata->fight_delay = (time_t)NULL;
            return FALSE;
	}
	if (difftime(mud_data.current_time,ch->pcdata->fight_delay) < time)
            return TRUE;
    }
    if (ch->pcdata->pk_delay != (time_t)NULL && ch->pcdata->pk_delay >= 0)
    {
	if (difftime(mud_data.current_time,ch->pcdata->pk_delay) < (time*3))
	    return TRUE;
    }
    return FALSE;
}

bool is_ghost(CHAR_DATA *ch, int time)
{
    if (IS_NPC(ch))
        return FALSE;
    if (ch->pcdata->ghost == (time_t)NULL)
        return FALSE;
    if (ch->pcdata->ghost && (difftime(mud_data.current_time,ch->pcdata->ghost) < time))
        return TRUE;
    else
        return FALSE;
}

int exp_diff(CHAR_DATA *ch, CHAR_DATA *victim)
{
    int expone, exptwo, expdiff, negfound = 0;
    expone = class_table[ch->class].extra_exp + pc_race_table[ch->race].class_mult;
    exptwo = class_table[victim->class].extra_exp + pc_race_table[victim->race].class_mult;
    expdiff = expone - exptwo;
    if ((expdiff % 150) != 0)
    {
	if (expdiff < 0)
	{
	    negfound++;
	    expdiff *= -1;
	}
	if ((expdiff % 150) >= 75)
	    expdiff += 75;
	if (negfound > 0)
	    expdiff *= -1;
    }
    return expdiff / 150;
}

bool is_pk(CHAR_DATA *ch, CHAR_DATA *victim)
{
    int diff, uprange, downrange;
    if (ch == NULL || victim == NULL)
	return FALSE;
    if ((is_ghost(ch,600) || is_ghost(victim,600)) && ch != victim)
	return FALSE;
    if (IS_NPC(ch) || IS_NPC(victim))
        return FALSE;
    if (victim->class == gcn_adventurer)
      return FALSE;
    if (ch->class == gcn_adventurer)
      return FALSE;
    if ( ch->level < 10 || victim->level < 10 )
        return FALSE;
    if ( ch->max_exp < 30000 || victim->max_exp < 30000 )
	return FALSE;
    if (ch == victim)
	return TRUE;
    /* HERO/MASTER can only attack pcs */
    if (IS_MASTER(ch) && !IS_MASTER(victim) && !IS_IMMORTAL(victim))
      return FALSE;
    if (IS_MASTER(victim) && !IS_MASTER(ch))
      return FALSE;
    if (ch->level == victim->level)
	return TRUE;
    diff = ch->level - victim->level;
    if ( diff < 0 )
        diff *= -1;
    if (diff > 8)
	return FALSE;
    diff = exp_diff(ch, victim);
    if ( diff < 0 )
	diff = UMAX( -((ch->level-1)/10), diff);
    else
	diff = UMIN( ((ch->level-1)/10), diff);
    uprange = ch->level + ((ch->level-1)/10) + 1 + diff;
    downrange = ch->level - ((ch->level-1)/10) - 1 + diff;
    if (victim->level > uprange || victim->level < downrange)
	return FALSE;
    diff = exp_diff(victim, ch);
    if ( diff < 0 )
        diff = UMAX( -((victim->level-1)/10), diff);
    else
        diff = UMIN( ((victim->level-1)/10), diff);
    uprange = victim->level + ((victim->level-1)/10) + 1 + diff;
    downrange = victim->level - ((victim->level-1)/10) - 1 + diff;
    if (ch->level <= uprange && ch->level >= downrange)
        return TRUE;
    return FALSE;
}

/* ignores ghost status */
bool is_pk_abs(CHAR_DATA *ch, CHAR_DATA *victim)
{
    int diff, uprange, downrange;
    if (ch == NULL || victim == NULL)
	return FALSE;
    if (IS_NPC(ch) || IS_NPC(victim))
        return FALSE;
    if (victim->class == gcn_adventurer)
      return FALSE;
    if (ch->class == gcn_adventurer)
      return FALSE;
    if ( ch->level < 10 || victim->level < 10 )
        return FALSE;
    if ( ch->max_exp < 30000 || victim->max_exp < 30000 )
	return FALSE;
    if (ch == victim)
	return TRUE;
    /* HERO/MASTER can only attack pcs */
    if (IS_MASTER(ch) && !IS_MASTER(victim) && !IS_IMMORTAL(victim))
      return FALSE;
    if (IS_MASTER(victim) && !IS_MASTER(ch))
      return FALSE;
    if (ch->level == victim->level)
	return TRUE;
    diff = ch->level - victim->level;
    if ( diff < 0 )
        diff *= -1;
    if (diff > 8)
	return FALSE;
    diff = exp_diff(ch, victim);
    if ( diff < 0 )
	diff = UMAX( -((ch->level-1)/10), diff);
    else
	diff = UMIN( ((ch->level-1)/10), diff);
    uprange = ch->level + ((ch->level-1)/10) + 1 + diff;
    downrange = ch->level - ((ch->level-1)/10) - 1 + diff;
    if (victim->level > uprange || victim->level < downrange)
	return FALSE;
    diff = exp_diff(victim, ch);
    if ( diff < 0 )
        diff = UMAX( -((victim->level-1)/10), diff);
    else
        diff = UMIN( ((victim->level-1)/10), diff);
    uprange = victim->level + ((victim->level-1)/10) + 1 + diff;
    downrange = victim->level - ((victim->level-1)/10) - 1 + diff;
    if (ch->level <= uprange && ch->level >= downrange)
        return TRUE;
    return FALSE;
}

int affect_by_size(CHAR_DATA *ch, CHAR_DATA *victim)
{
    int diff = ch->size - victim->size;

    if ( diff < -3 )		return -35;
    else if ( diff < -2 )	return -25;
    else if ( diff < -1 )	return -15;
    else if ( diff < 0 )	return -8;
    else if ( diff < 1 )	return 0;
    else if ( diff < 2 )	return 8;
    else if ( diff < 3 )	return 15;
    else			return 25;
}

bool is_old_mob(CHAR_DATA *ch)
{
    if (ch->pIndexData == NULL)
	return FALSE;
    else if (ch->pIndexData->new_format)
	return FALSE;
    return TRUE;
}

inline bool cabal_has_power(CABAL_DATA* pCabal, int rank ){
  CABAL_DATA* pCab = get_parent(pCabal);

  if (is_captured(pCab)){
    return FALSE;
  }
  switch( rank ){
  case 2: return IS_CABAL(pCab, CABAL_ALLOW_2) ? TRUE : FALSE;	break;
  case 3: return IS_CABAL(pCab, CABAL_ALLOW_3) ? TRUE : FALSE;	break;
  case 4: return IS_CABAL(pCab, CABAL_ALLOW_4) ? TRUE : FALSE;	break;
  case 5: return IS_CABAL(pCab, CABAL_ALLOW_5) ? TRUE : FALSE;	break;
  default:
    return TRUE;
  }
}

int sklevel(CHAR_DATA *ch, int sn)
{
  CSKILL_DATA* cSkill = NULL;

  if (IS_NPC(ch))
    return 1000;
  /* cabal check */
  else if (ch->pCabal && !IS_IMMORTAL(ch) && (cSkill = get_cskill(ch->pCabal, sn )) != NULL){
    int cp = GET_CP( ch ) ;
    /* rank requirement failed, skill is not shown */
    if (ch->pcdata->rank < cSkill->min_rank
	|| ch->pcdata->rank > cSkill->max_rank
	|| !cabal_has_power(ch->pCabal, cSkill->min_rank)){
      return 1000;
    }
    /* cost req. failed, skill is shown as lacking power */
    else if (cp < cSkill->min_cost || cp < get_cskill_cost(ch, ch->in_room, cSkill)){
      return CABAL_COST_FAILED;
    }
    else
      return cSkill->pSkill->level;
  }
  else{
    SKILL_DATA *nsk;
    if ((nsk = nskill_find(ch->pcdata->newskills,sn)) != NULL)
      return nsk->level;
    else if (skill_table[sn].skill_level[ch->class] > 50 && get_trust(ch) < DEITY)
      return 1000;
    else if (sn == gsn_hand_to_hand && ch->race == race_lookup("illithid"))
      return 1;
    else
      return skill_table[sn].skill_level[ch->class];
  }
}

int get_skill(CHAR_DATA *ch, int sn)
{
    int skill;
    SKILL_DATA *nsk;
    AFFECT_DATA* paf;

    if (sn == -1)
	skill = ch->level * 5 / 2;
    else if (sn < -1 || sn > MAX_SKILL)
    {
	bug("Bad sn %d in get_skill.",sn);
	skill = 0;
    }
    else if (is_affected(ch,gsn_ecstacy))
	return 0;
    else if (!IS_NPC(ch))
    {
	if (sklevel(ch,sn) > ch->level)
	    skill = 0;
	else
	    skill = ch->pcdata->learned[sn];
	if (skill == 0 && (nsk = nskill_find(ch->pcdata->newskills,sn)) != NULL && ch->level >= nsk->level)
	    skill = nsk->learned;
    }
    else
    {
        if (skill_table[sn].spell_fun != spell_null)
	    skill = 40 + 2 * ch->level;
	else if (sn == gsn_sneak || sn == gsn_hide)
	    skill = ch->level * 2 + 20;
	/* Verdigar Tattoo */
	else if (sn == gsn_quiet_movement && ch->master
		 && ch->master->tattoo == deity_lookup("nature"))
	  skill = 110;
        else if ((sn == gsn_dodge && IS_SET(ch->off_flags,OFF_DODGE))
		 || (sn == gsn_parry && IS_SET(ch->off_flags,OFF_PARRY))
		 || (sn == gsn_dual_parry && IS_SET(ch->off_flags,OFF_DPARRY))
		 || (sn == gsn_shield_block))
	  skill = ch->level * 2;
        else if (sn == gsn_second_attack && (IS_SET(ch->act,ACT_WARRIOR) || IS_SET(ch->act,ACT_THIEF)))
	  skill = 10 + 3 * ch->level;
	else if (sn == gsn_third_attack && IS_SET(ch->act,ACT_WARRIOR))
	  skill = 3 * ch->level - 40;
	else if (sn == gsn_fired && IS_SET(ch->act,ACT_WARRIOR))
	    skill = 50 +  3 * ch->level / 2;
	else if (sn == gsn_fired && IS_SET(ch->act,ACT_THIEF))
	    skill = 50 + 2 * ch->level;
	else if (sn == gsn_archery && IS_SET(ch->act,ACT_THIEF))
	    skill = 50 + 2 * ch->level;
	else if (sn == gsn_hand_to_hand)
	  skill = 40 + 2 * ch->level;
        else if ((sn == gsn_trip && IS_SET(ch->off_flags,OFF_TRIP))
		 || (sn == gsn_bash && IS_SET(ch->off_flags,OFF_BASH))
		 || (sn == gsn_dirt && IS_SET(ch->off_flags,OFF_KICK_DIRT))
		 || (sn == gsn_kick && IS_SET(ch->off_flags,OFF_KICK)))
	  skill = 10 + 3 * ch->level;
	else if (sn == gsn_disarm
		 && (IS_SET(ch->off_flags,OFF_DISARM)
		     || IS_SET(ch->act,ACT_WARRIOR)
		     || IS_SET(ch->act,ACT_THIEF)))
	  skill = 25 + ch->level;
	else if (sn == gsn_berserk && IS_SET(ch->off_flags,OFF_BERSERK))
	  skill = 3 * ch->level;
	else if (sn == gsn_backstab && IS_SET(ch->act,ACT_THIEF))
	  skill = 20 + 2 * ch->level;
	else if (sn == gsn_circle && IS_SET(ch->act,ACT_THIEF))
	  skill = 20 + 2 * ch->level;
	else if (sn == gsn_blackjack && IS_SET(ch->act,ACT_THIEF))
	  skill = 20 + 2 * ch->level;
	else if (sn == gsn_shield_disarm && IS_SET(ch->act,ACT_WARRIOR))
	  skill = 25 + ch->level;
	else if (sn == gsn_rake && IS_SET(ch->act,ACT_WARRIOR))
	  skill = 20 + 2 * ch->level;
  	else if (sn == gsn_rescue)
	  skill = 40 + ch->level;
	else if (sn == gsn_recall)
	  skill = 40 + ch->level;
	else if (sn == gsn_sword
		 || sn == gsn_dagger
		 || sn == gsn_spear
		 || sn == gsn_staff
		 || sn == gsn_mace
		 || sn == gsn_axe
		 || sn == gsn_flail
		 || sn == gsn_whip
		 || sn == gsn_polearm
		 || sn == gsn_fired)
	  skill = 50 + ch->level;
	else
	  skill = 0;

	/* mobprogs get alls kills */
	if (ch->trust == 6969){
	  if (sn == gsn_sneak
	      || sn == gsn_fade
	      || sn == gsn_quiet_movement)
	    sn = 0;
	  else
	    skill = (2 * ch->level) - 10;
	}
    }
    /* none of the stuff below happends if skill here is zero */
    if (skill == 0)
      return 0;

    /* Over All SKILL MODIFIERS */
    if ( !IS_NPC(ch)
	 && !IS_PERK(ch, PERK_ALCOHOLIC)
	 && !is_affected(ch,gsn_drunken)
	 && ch->pcdata->condition[COND_DRUNK]  > 30 )
      skill = 9 * skill / 10;

    if (ch->in_room
	&& IS_SET(ch->vuln_flags,VULN_DROWNING)
	&& (ch->in_room->sector_type == SECT_WATER_SWIM
	    || ch->in_room->sector_type == SECT_WATER_NOSWIM) )
      skill = 9 * skill / 10;


    skill = URANGE(0,skill, 110);

    /* BONUSES OVER 100*/
    if (ch->race == race_lookup("storm")
	&& (ch->in_room
	    && (ch->in_room->sector_type == SECT_WATER_SWIM
		|| ch->in_room->sector_type == SECT_WATER_NOSWIM) ) )
      skill = 11 * skill / 10;

    if (skill && !IS_NPC(ch)){
      if (sn == gsn_dodge
	  && get_skill(ch, gsn_orealts_1st) > 1)
	skill += 3;
      else if ((sn == gsn_hide || sn == gsn_camouflage)
	       && IS_PERK(ch, PERK_SHORT))
	skill += 3;
    }
    /* check of affects modified skill (only if we have the skill) */
    /* not done before as we want this togo over 100 slightly */
    if (!IS_NPC(ch) && skill)
      skill += ch->pcdata->to_learned[sn];

    /* NEGATIVE SKILL MODIFIERS LAST, FROM STRONGEST TO WEAKEST */
    if (!IS_NPC(ch) && skill <= 100){
      if ( is_affected(ch,gsn_hysteria) )
	skill = 3 * skill / 4;
      else if ( (paf = affect_find(ch->affected, gsn_fear)) != NULL){
	skill -= abs(paf->modifier);
      }
    }

    skill = URANGE(0, skill,110);
    return skill;
}


/* returns sn of the skill type */
int weapon_sn(OBJ_DATA* wield)
{
  if (wield == NULL || wield->item_type != ITEM_WEAPON)
    return gsn_hand_to_hand;
  else switch (wield->value[0])
    {
    default :               return -1;                break;
    case(WEAPON_SWORD):     return gsn_sword;         break;
    case(WEAPON_DAGGER):    return gsn_dagger;        break;
    case(WEAPON_SPEAR):     return gsn_spear;         break;
    case(WEAPON_STAFF):     return gsn_staff;         break;
    case(WEAPON_MACE):      return gsn_mace;          break;
    case(WEAPON_AXE):       return gsn_axe;           break;
    case(WEAPON_FLAIL):     return gsn_flail;         break;
    case(WEAPON_WHIP):      return gsn_whip;          break;
    case(WEAPON_POLEARM):   return gsn_polearm;       break;
    }
}

int get_weapon_sn(CHAR_DATA *ch, bool secondary)
{
  OBJ_DATA *wield;

  if (!secondary)
    wield = get_eq_char( ch, WEAR_WIELD );
  else
    wield = get_eq_char( ch, WEAR_SECONDARY);
  return weapon_sn(wield);
}

//returns skill based on obj passed.
int get_weapon_skill_obj(CHAR_DATA *ch, OBJ_DATA* obj)
{
  //data
  int sn = 0;
  int skill_mod = 0;
  int wep_skill = 0;

  //bool
  bool is2h = FALSE;

  if (ch->class == gcn_blademaster && is_affected(ch, gsn_puppet_master)){
    OBJ_DATA* wield = has_twohanded(ch);
    if (wield)
      obj = wield;
  }

  if (obj == NULL){
    wep_skill = get_weapon_skill(ch, gsn_hand_to_hand, FALSE);
    sn = gsn_hand_to_hand;
  }
  else if (obj->item_type != ITEM_WEAPON)
    return get_weapon_skill(ch, gsn_hand_to_hand, FALSE);
  else{
    sn = weapon_sn(obj);
    is2h = ( (IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS)
	      || is_affected(ch, gsn_double_grip))
	     && obj->value[0] != WEAPON_STAFF);
    wep_skill = get_weapon_skill(ch, sn, is2h);
  }

  /* exotic mastery */
  if (sn == -1){
    if (get_skill(ch, gsn_exotic_mastery) > 1)
      skill_mod += 7;
    if (get_skill(ch, gsn_exotic_expert) > 1)
      skill_mod += 3;
  }

  /* any addition beyond this isto be only done if skill is more then 1 */
  if (wep_skill < 2)
    return  UMIN(skill_mod + wep_skill, 120);


  if (get_skill(ch, gsn_2h_mastery) > 1){
    if (is2h || (obj && obj->value[0] == WEAPON_STAFF))
      skill_mod += 5;
    else
      skill_mod -= 3;
  }
  if (sn == gsn_hand_to_hand && ch->class == gcn_monk)
    skill_mod += 10;
  if ( obj && (attack_table[obj->value[3]].damage == DAM_FIRE || !str_cmp("fire", obj->material))
       && ch->race == race_lookup("fire") ){
    skill_mod += UMAX(0, 12 - skill_mod);
  }

  return  UMIN(skill_mod + wep_skill, 120);
}


int get_weapon_skill(CHAR_DATA *ch, int sn, bool fTwo)
{
  int skill;
  if (IS_NPC(ch))
    {
      if (sn == -1)
	skill = 3 * ch->level;
      else if (sn == gsn_hand_to_hand)
	skill = 40 + 2 * ch->level;
      else
	skill = 40 + 5 * ch->level / 2;
    }
  else
    {
      if (sn == -1)
	skill = 3 * ch->level;
      else if (sn == gsn_hand_to_hand)
	skill = get_skill(ch, sn);
      else
	skill = get_skill(ch,sn);

      // Twohanded affects 2/3 of the weapon skill.
      //need that one due to round off errors summing all up to 99
      if (fTwo)
	skill = 1 + ((skill / 3) +
		 (get_skill(ch, gsn_2hands) * 2 * skill / 300));
    }
  //Illthids get 75% skill in weapons they don't know
  if (skill < 1 && ch->race == grn_illithid)
    skill = 75;
  return URANGE(0,skill, 110);
}

void change_sex(CHAR_DATA *ch, int mod)
{
    if (IS_NPC(ch))
	return;
    if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
	ch->pcdata->true_sex = 0;
    if (mod != ch->pcdata->true_sex)
	ch->sex = mod;
}

void fixed_sex(CHAR_DATA *ch)
{
    int loc, mod;
    OBJ_DATA *obj;
    AFFECT_DATA *af;
    if (IS_NPC(ch))
	return;
    if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
	ch->pcdata->true_sex = 0;
    ch->sex		= ch->pcdata->true_sex;
    for (loc = 0; loc < MAX_WEAR; loc++)
    {
        obj = get_eq_char(ch,loc);
        if (obj == NULL)
            continue;
        if (!obj->enchanted)
	    for ( af = obj->pIndexData->affected; af != NULL; af = af->next )
            {
                mod = af->modifier;
                if (af->location == APPLY_SEX && ch->sex != mod)
		{
		    ch->sex = mod;
		    return;
		}
            }
        for ( af = obj->affected; af != NULL; af = af->next )
        {
            mod = af->modifier;
            if (af->location == APPLY_SEX && ch->sex != mod)
	    {
	    	ch->sex = mod;
		return;
	    }
	}
    }
    for (af = ch->affected; af != NULL; af = af->next)
    {
        mod = af->modifier;
        if (af->location == APPLY_SEX && ch->sex != mod)
	{
	    ch->sex = mod;
	    return;
	}
    }
    for (af = ch->affected2; af != NULL; af = af->next)
    {
        mod = af->modifier;
        if (af->location == APPLY_SEX && ch->sex != mod)
	{
	    ch->sex = mod;
	    return;
	}
    }
}

void reset_char(CHAR_DATA *ch)
{
    int loc, mod, stat;
    OBJ_DATA *obj;
    AFFECT_DATA *af;
    int i;
    if (IS_NPC(ch))
	return;
    if (ch->pcdata->perm_hit == 0
	|| ch->pcdata->perm_mana == 0
	|| ch->pcdata->perm_move == 0
	|| ch->pcdata->last_level == 0){
      for (loc = 0; loc < MAX_WEAR; loc++){
	obj = get_eq_char(ch,loc);
	if (obj == NULL)
	  continue;
	/* sheaths do not affect anything */
	else if (loc == WEAR_SHEATH_L || loc == WEAR_SHEATH_R)
	  continue;

	if (!obj->enchanted){
	  for ( af = obj->pIndexData->affected; af != NULL; af = af->next ){
	    mod = af->modifier;
	    switch(af->location){
	    case APPLY_MANA: ch->max_mana    -= mod; break;
	    case APPLY_HIT:  ch->max_hit     -= mod; break;
	    case APPLY_MOVE: ch->max_move    -= mod; break;
	    }
	  }
	}
	for ( af = obj->affected; af != NULL; af = af->next ){
	  mod = af->modifier;
	  switch(af->location){
	  case APPLY_MANA: ch->max_mana -= mod; break;
	  case APPLY_HIT:  ch->max_hit  -= mod; break;
	  case APPLY_MOVE: ch->max_move -= mod; break;
	  }
	}
      }
      ch->pcdata->perm_hit 	= ch->max_hit;
      ch->pcdata->perm_mana 	= ch->max_mana;
      ch->pcdata->perm_move	= ch->max_move;
      ch->pcdata->last_level	= ch->played/3600;
      if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
        {
	  if (ch->sex > 0 && ch->sex < 3)
	    ch->pcdata->true_sex = ch->sex;
	  else
	    ch->pcdata->true_sex = 0;
        }
    }
    /* End  reset if stats 0 */

    for (stat = 0; stat < MAX_STATS; stat++)
      ch->mod_stat[stat] = 0;
    for (stat = 0; stat < MAX_MODIFIER; stat++)
      ch->aff_mod[stat] = 0;

    if (ch->pcdata->true_sex < 0
	|| ch->pcdata->true_sex > 2)
      ch->pcdata->true_sex = 0;
    ch->sex		= ch->pcdata->true_sex;
    ch->max_hit 	= ch->pcdata->perm_hit;
    ch->max_mana	= ch->pcdata->perm_mana;
    ch->max_move	= ch->pcdata->perm_move;
    for (i = 0; i < 4; i++)
    	ch->armor[i]	= 100;
    ch->hitroll		= 0;
    ch->damroll		= 0;
    ch->savingaffl	= 0;
    ch->savingmaled	= 0;
    ch->savingmental	= 0;
    ch->savingbreath	= 0;
    ch->savingspell	= 0;
    for (loc = 0; loc < MAX_WEAR; loc++)
    {
      int mod = 0;
      obj = get_eq_char(ch,loc);
      if (obj == NULL)
	continue;
      if (get_skill(ch, gsn_adv_armor) > 1
	  && (loc == WEAR_BODY || loc == WEAR_HEAD || loc == WEAR_FACE
	      || loc == WEAR_ARMS || loc == WEAR_LEGS) )
	mod = 1;
      else
	mod = 0;
      for (i = 0; i < 4; i++)
	ch->armor[i] -= apply_ac( obj, loc, i, mod );
      if (!obj->enchanted){
	for ( af = obj->pIndexData->affected; af != NULL; af = af->next ){
	  mod = af->modifier;
	  /* ranged items do not increase hit/dam */
	  if (loc == WEAR_QUIVER )
	    continue;
	  /* sheaths do not affect anything */
	  if (loc == WEAR_SHEATH_L || loc == WEAR_SHEATH_R)
	    continue;
	  if ( af->where == TO_SKILL){
	    int sn;
	    if (IS_NPC(ch))
	      return;
	    sn = af->location;
	    if (sn < -1 || sn > MAX_SKILL)
	      {
		bug("reset_char: TO_SKILL with bad skill number (%d).",sn);
		return;
	      }
	    /* add on the modifier */
	    ch->pcdata->to_learned[sn] += mod;
	    continue;
	  }

	  switch(af->location){
	  case APPLY_STR:		ch->mod_stat[STAT_STR]	+= mod;	break;
	  case APPLY_DEX:		ch->mod_stat[STAT_DEX]	+= mod; break;
	  case APPLY_INT:		ch->mod_stat[STAT_INT]	+= mod; break;
	  case APPLY_WIS:		ch->mod_stat[STAT_WIS]	+= mod; break;
	  case APPLY_CON:		ch->mod_stat[STAT_CON]	+= mod; break;
	  case APPLY_LUCK:		ch->mod_stat[STAT_LUCK]	+= mod; break;
	  case APPLY_SEX:		change_sex(ch,mod); break;
	  case APPLY_AGE:	        ch->aff_mod[MOD_AGE]    += mod; break;
	  case APPLY_SIZE:		ch->size                += mod; break;
	  case APPLY_MANA:		ch->max_mana		+= mod; break;
	  case APPLY_HIT:		ch->max_hit		+= mod; break;
	  case APPLY_MOVE:		ch->max_move		+= mod; break;
	  case APPLY_AC:
	    for (i = 0; i < 4; i ++)
	      ch->armor[i] += mod;
	    break;
	  case APPLY_HITROLL:		ch->hitroll		+= mod; break;
	  case APPLY_DAMROLL:		ch->damroll		+= mod; break;
	  case APPLY_SAVING_AFFL:	ch->savingaffl		+= mod; break;
	  case APPLY_SAVING_MALED: 	ch->savingmaled		+= mod; break;
	  case APPLY_SAVING_MENTAL:	ch->savingmental	+= mod; break;
	  case APPLY_SAVING_BREATH: 	ch->savingbreath	+= mod; break;
	  case APPLY_SAVING_SPELL:	ch->savingspell		+= mod; break;
	  case APPLY_SPELL_AFFECT:                           break;
	  case APPLY_SPELL_LEVEL:	ch->aff_mod[MOD_SPELL_LEVEL]  += mod; break;
	  case APPLY_AFFL_LEVEL:	ch->aff_mod[MOD_AFFL_LEVEL]   += mod; break;
	  case APPLY_MALED_LEVEL:	ch->aff_mod[MOD_MALED_LEVEL]  += mod; break;
	  case APPLY_MENTAL_LEVEL:ch->aff_mod[MOD_MENTAL_LEVEL] += mod; break;
	  case APPLY_SPELL_COST:  ch->aff_mod[MOD_SPELL_COST]   += mod; break;
	  case APPLY_WAIT_STATE:	ch->aff_mod[MOD_WAIT_STATE]   += mod; break;
	  case APPLY_SKILL_LEARN:	ch->aff_mod[MOD_SKILL_LEARN]  += mod; break;
	  case APPLY_SPELL_LEARN:	ch->aff_mod[MOD_SPELL_LEARN]  += mod; break;
	  case APPLY_HIT_GAIN:	ch->aff_mod[MOD_HIT_GAIN]     += mod; break;
	  case APPLY_MANA_GAIN:	ch->aff_mod[MOD_MANA_GAIN]    += mod; break;
	  case APPLY_MOVE_GAIN:	ch->aff_mod[MOD_MOVE_GAIN]    += mod; break;
	  }
	}
      }
      for ( af = obj->affected; af != NULL; af = af->next ){
	mod = af->modifier;
	/* ranged items do not increase hit or dam */
	if (loc == WEAR_QUIVER)
	  continue;

	/* sheaths do not affect anything */
	if (loc == WEAR_SHEATH_L || loc == WEAR_SHEATH_R)
	  continue;

	if ( af->where == TO_SKILL){
	  int sn;
	  if (IS_NPC(ch))
	    return;
	  sn = af->location;
	  if (sn < -1 || sn > MAX_SKILL)
	    {
	      bug("reset_char: TO_SKILL with bad skill number (%d).",sn);
	      return;
	    }
	    /* add on the modifier */
	  ch->pcdata->to_learned[sn] += mod;
	  continue;
	}
	switch(af->location){
	case APPLY_STR:  ch->mod_stat[STAT_STR]  += mod; break;
	case APPLY_DEX:  ch->mod_stat[STAT_DEX]  += mod; break;
	case APPLY_INT:  ch->mod_stat[STAT_INT]  += mod; break;
	case APPLY_WIS:  ch->mod_stat[STAT_WIS]  += mod; break;
	case APPLY_CON:  ch->mod_stat[STAT_CON]  += mod; break;
	case APPLY_LUCK: ch->mod_stat[STAT_LUCK] += mod; break;
	case APPLY_SEX:  change_sex(ch,mod); break;
	case APPLY_AGE:	 ch->aff_mod[MOD_AGE]    += mod; break;
	case APPLY_SIZE: ch->size                += mod; break;
	case APPLY_MANA: ch->max_mana            += mod; break;
	case APPLY_HIT:  ch->max_hit             += mod; break;
	case APPLY_MOVE: ch->max_move            += mod; break;
	case APPLY_AC:
	  for (i = 0; i < 4; i ++)
	    ch->armor[i] += mod;
	  break;
	case APPLY_HITROLL:       ch->hitroll      += mod; break;
	case APPLY_DAMROLL:       ch->damroll      += mod; break;
	case APPLY_SAVING_AFFL:   ch->savingaffl   += mod; break;
	case APPLY_SAVING_MALED:  ch->savingmaled  += mod; break;
	case APPLY_SAVING_MENTAL: ch->savingmental += mod; break;
	case APPLY_SAVING_BREATH: ch->savingbreath += mod; break;
	case APPLY_SAVING_SPELL:  ch->savingspell  += mod; break;
	case APPLY_SPELL_AFFECT:                           break;
	case APPLY_SPELL_LEVEL:	ch->aff_mod[MOD_SPELL_LEVEL]  += mod; break;
	case APPLY_AFFL_LEVEL:	ch->aff_mod[MOD_AFFL_LEVEL]   += mod; break;
	case APPLY_MALED_LEVEL:	ch->aff_mod[MOD_MALED_LEVEL]  += mod; break;
	case APPLY_MENTAL_LEVEL:ch->aff_mod[MOD_MENTAL_LEVEL] += mod; break;
	case APPLY_SPELL_COST:  ch->aff_mod[MOD_SPELL_COST]   += mod; break;
	case APPLY_WAIT_STATE:	ch->aff_mod[MOD_WAIT_STATE]   += mod; break;
	case APPLY_SKILL_LEARN:	ch->aff_mod[MOD_SKILL_LEARN]  += mod; break;
	case APPLY_SPELL_LEARN:	ch->aff_mod[MOD_SPELL_LEARN]  += mod; break;
	case APPLY_HIT_GAIN:	ch->aff_mod[MOD_HIT_GAIN]     += mod; break;
	case APPLY_MANA_GAIN:	ch->aff_mod[MOD_MANA_GAIN]    += mod; break;
	case APPLY_MOVE_GAIN:	ch->aff_mod[MOD_MOVE_GAIN]    += mod; break;
	}
      }
    }
    for (af = ch->affected; af != NULL; af = af->next){
      mod = af->modifier;

      if ( af->where == TO_SKILL){
	int sn;
	if (IS_NPC(ch))
	  return;
	sn = af->location;
	if (sn < -1 || sn > MAX_SKILL)
	  {
	    bug("reset_char: TO_SKILL with bad skill number (%d).",sn);
	    return;
	  }
	/* add on the modifier */
	ch->pcdata->to_learned[sn] += mod;
	continue;
      }
      switch(af->location){
      case APPLY_STR:  ch->mod_stat[STAT_STR]  += mod; break;
            case APPLY_DEX:  ch->mod_stat[STAT_DEX]  += mod; break;
      case APPLY_INT:  ch->mod_stat[STAT_INT]  += mod; break;
      case APPLY_WIS:  ch->mod_stat[STAT_WIS]  += mod; break;
      case APPLY_CON:  ch->mod_stat[STAT_CON]  += mod; break;
      case APPLY_LUCK:  ch->mod_stat[STAT_LUCK] += mod; break;
      case APPLY_SEX:  change_sex(ch,mod); break;
      case APPLY_AGE:  ch->aff_mod[MOD_AGE]   += mod; break;
      case APPLY_SIZE: ch->size                += mod; break;
      case APPLY_MANA: ch->max_mana            += mod; break;
      case APPLY_HIT:  ch->max_hit             += mod; break;
      case APPLY_MOVE: ch->max_move            += mod; break;
      case APPLY_AC:
	for (i = 0; i < 4; i ++)
	  ch->armor[i] += mod;
	break;
      case APPLY_HITROLL:       ch->hitroll      += mod; break;
      case APPLY_DAMROLL:       ch->damroll      += mod; break;
      case APPLY_SAVING_AFFL:   ch->savingaffl   += mod; break;
      case APPLY_SAVING_MALED:  ch->savingmaled  += mod; break;
      case APPLY_SAVING_MENTAL: ch->savingmental += mod; break;
      case APPLY_SAVING_BREATH: ch->savingbreath += mod; break;
      case APPLY_SAVING_SPELL:  ch->savingspell  += mod; break;
      case APPLY_SPELL_AFFECT:                           break;
      case APPLY_SPELL_LEVEL:	ch->aff_mod[MOD_SPELL_LEVEL]  += mod; break;
      case APPLY_AFFL_LEVEL:	ch->aff_mod[MOD_AFFL_LEVEL]   += mod; break;
      case APPLY_MALED_LEVEL:	ch->aff_mod[MOD_MALED_LEVEL]  += mod; break;
      case APPLY_MENTAL_LEVEL:ch->aff_mod[MOD_MENTAL_LEVEL] += mod; break;
      case APPLY_SPELL_COST:  ch->aff_mod[MOD_SPELL_COST]   += mod; break;
      case APPLY_WAIT_STATE:	ch->aff_mod[MOD_WAIT_STATE]   += mod; break;
      case APPLY_SKILL_LEARN:	ch->aff_mod[MOD_SKILL_LEARN]  += mod; break;
      case APPLY_SPELL_LEARN:	ch->aff_mod[MOD_SPELL_LEARN]  += mod; break;
      case APPLY_HIT_GAIN:	ch->aff_mod[MOD_HIT_GAIN]     += mod; break;
      case APPLY_MANA_GAIN:	ch->aff_mod[MOD_MANA_GAIN]    += mod; break;
      case APPLY_MOVE_GAIN:	ch->aff_mod[MOD_MOVE_GAIN]    += mod; break;
      }
    }
    for (af = ch->affected2; af != NULL; af = af->next){
      mod = af->modifier;
      if ( af->where == TO_SKILL){
	int sn;
	if (IS_NPC(ch))
	  return;
	sn = af->location;
	if (sn < -1 || sn > MAX_SKILL)
	  {
	    bug("reset_char: TO_SKILL with bad skill number (%d).",sn);
	    return;
	  }
	/* add on the modifier */
	ch->pcdata->to_learned[sn] += mod;
	continue;
      }
      switch(af->location){
      case APPLY_STR:  ch->mod_stat[STAT_STR]  += mod; break;
      case APPLY_DEX:  ch->mod_stat[STAT_DEX]  += mod; break;
      case APPLY_INT:  ch->mod_stat[STAT_INT]  += mod; break;
      case APPLY_WIS:  ch->mod_stat[STAT_WIS]  += mod; break;
      case APPLY_CON:  ch->mod_stat[STAT_CON]  += mod; break;
      case APPLY_LUCK:  ch->mod_stat[STAT_LUCK] += mod; break;
      case APPLY_SEX:  change_sex(ch,mod); break;
      case APPLY_AGE:  ch->aff_mod[MOD_AGE]    += mod; break;
      case APPLY_SIZE: ch->size                += mod; break;
      case APPLY_MANA: ch->max_mana            += mod; break;
      case APPLY_HIT:  ch->max_hit             += mod; break;
      case APPLY_MOVE: ch->max_move            += mod; break;
      case APPLY_AC:
	for (i = 0; i < 4; i ++)
	  ch->armor[i] += mod;
	break;
      case APPLY_HITROLL:       ch->hitroll      += mod; break;
      case APPLY_DAMROLL:       ch->damroll      += mod; break;
      case APPLY_SAVING_AFFL:   ch->savingaffl   += mod; break;
      case APPLY_SAVING_MALED:  ch->savingmaled  += mod; break;
      case APPLY_SAVING_MENTAL: ch->savingmental += mod; break;
      case APPLY_SAVING_BREATH: ch->savingbreath += mod; break;
      case APPLY_SAVING_SPELL:  ch->savingspell  += mod; break;
      case APPLY_SPELL_AFFECT:                           break;
      case APPLY_SPELL_LEVEL:	ch->aff_mod[MOD_SPELL_LEVEL]  += mod; break;
      case APPLY_AFFL_LEVEL:	ch->aff_mod[MOD_AFFL_LEVEL]   += mod; break;
      case APPLY_MALED_LEVEL:	ch->aff_mod[MOD_MALED_LEVEL]  += mod; break;
      case APPLY_MENTAL_LEVEL:ch->aff_mod[MOD_MENTAL_LEVEL] += mod; break;
      case APPLY_SPELL_COST:  ch->aff_mod[MOD_SPELL_COST]   += mod; break;
      case APPLY_WAIT_STATE:	ch->aff_mod[MOD_WAIT_STATE]   += mod; break;
      case APPLY_SKILL_LEARN:	ch->aff_mod[MOD_SKILL_LEARN]  += mod; break;
      case APPLY_SPELL_LEARN:	ch->aff_mod[MOD_SPELL_LEARN]  += mod; break;
      case APPLY_HIT_GAIN:	ch->aff_mod[MOD_HIT_GAIN]     += mod; break;
      case APPLY_MANA_GAIN:	ch->aff_mod[MOD_MANA_GAIN]    += mod; break;
      case APPLY_MOVE_GAIN:	ch->aff_mod[MOD_MOVE_GAIN]    += mod; break;
      }
    }

    if (ch->sex < 0 || ch->sex > 2)
      ch->sex = ch->pcdata->true_sex;
    /* reset title for crusaders*/
    if (ch->level == 50 && ch->class == class_lookup("crusader")){
      char buf[MIL];
      sprintf( buf, " the %s",
	       title_table [ch->class] [ch->level] [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
/* we check if is a default level 50 title, if so we check it might change */
      if (!strcmp(buf, ch->pcdata->title)){
	sprintf(buf, "%s", get_crus_title(ch, 50));
	set_title(ch, buf);
      }
    }
}

int get_trust( CHAR_DATA *ch )
{
  if (!ch)
    return 6969;
  if ( ch->desc == NULL && IS_NPC(ch)
       && !IS_AFFECTED(ch,AFF_CHARM)
       && ch->trust && ch->trust == 6969
       && (ch->pIndexData->progtypes || ch->pIndexData->mprogs) )
    return MAX_LEVEL + 1;
  if ( ch->desc != NULL && ch->desc->original != NULL )
    ch = ch->desc->original;
  if (ch->trust)
    return ch->trust;
    else if ( IS_NPC(ch) && ch->level > LEVEL_HERO )
      return LEVEL_HERO;
  else
    return ch->level;
}

//returns true age of the person
int get_abs_age( CHAR_DATA *ch ){
  int age = 0;
  age += (mud_data.current_time - ch->pcdata->birth_date) / MAX_MONTHS / MAX_WEEKS / MAX_DAYS / MAX_HOURS / 30;
  age = UMAX(1, age);

  return (age);
}

//returns age with eq/spell modifications
int get_age( CHAR_DATA *ch ){
  int age = ch->aff_mod[MOD_AGE];
  if (IS_NPC(ch))
    return 18;
  age += get_abs_age( ch );
  return (age);
}

int get_curr_cond( OBJ_DATA *obj ){
  return URANGE(0, obj->condition, 100);
}

//returns modifier on stat based on age
int get_stat_agemod( int years, int race, int stat ){
  int max = 0;
  int age = 100 * years / pc_race_table[race].lifespan;

  if (stat == STAT_STR && age > 79)
    max -= 1;
  else if (stat == STAT_INT && age > 84)
    max += 1;
  else if (stat == STAT_DEX && age > 89)
    max -= 1;
  else if (stat == STAT_WIS && age > 94)
    max += 1;
  else if (stat == STAT_CON && age > 99)
    max -= 1;

  return max;
}

//returns current maximum based on skills etc
int get_max_stat(  CHAR_DATA *ch, int stat ){
  int max;

  if (IS_NPC(ch) || IS_IMMORTAL(ch))
    max = 25;
  else
    max = UMIN(get_max_train(ch,stat), 25);

  if (!IS_NPC(ch))
    max += get_stat_agemod( get_age( ch ), ch->race, stat );

  if (stat == STAT_STR && is_affected(ch, gsn_istrength)){
    max = UMIN(max + 1, 25);
  }

  /* were races */
  if (ch->race == grn_werebeast){
    if (stat == STAT_STR && is_affected(ch, gsn_weretiger)){
      max = UMIN(max + 2, 25);
    }
    if (stat == STAT_STR && is_affected(ch, gsn_werebear)){
      max = UMIN(max + 2, 25);
    }
    if (stat == STAT_STR && is_affected(ch, gsn_werewolf)){
      max = UMIN(max + 1, 25);
    }
    if (stat == STAT_DEX && is_affected(ch, gsn_werewolf)){
      max = UMIN(max + 1, 25);
    }
    if (stat == STAT_CON && is_affected(ch, gsn_werebear)){
      max = UMIN(max + 2, 25);
    }
    if (stat == STAT_DEX && is_affected(ch, gsn_werefalcon)){
      max = UMIN(max + 3, 25);
    }
    if (stat == STAT_DEX && is_affected(ch, gsn_werebear)){
      max = UMIN(max - 2, 25);
    }
  }
  return (URANGE(0, max, 25));
}

//returns absolute stat value
int get_abs_stat( CHAR_DATA *ch, int stat ){
  int cur = 0;
  cur = ch->perm_stat[stat] + ch->mod_stat[stat];

  if (stat == STAT_DEX && get_skill(ch, gsn_orealts_1st))
    cur++;
  if (stat == STAT_STR && is_affected(ch, gsn_istrength)){
    cur = get_max_stat(ch, stat );
  }
  if (!IS_NPC(ch))
    cur += get_stat_agemod( get_age( ch ), ch->race, stat );

  return (UMAX(0, cur));
}

int get_curr_stat( CHAR_DATA *ch, int stat )
{
  //shadowdancer
  if (stat == STAT_STR
      && ch->class == gcn_blademaster
      && is_affected(ch, gsn_shadowdancer)
      && has_twohanded(ch)){
    stat = STAT_DEX;
  }

  int max = get_max_stat( ch, stat );
  int cur = get_abs_stat( ch, stat );

  return URANGE(3, cur, max);
}

int get_max_train( CHAR_DATA *ch, int stat )
{
    int max;
    if (IS_NPC(ch) || ch->level > LEVEL_IMMORTAL)
	return 25;
    max = pc_race_table[ch->race].max_stats[stat];
    if (class_table[ch->class].attr_prime == stat)
      if (ch->race == race_lookup("human"))
	max += 1;

    /* Avatars get + 1 bonues to all */
    if (IS_SET(ch->act2, PLR_AVATAR))
      max ++;
    return UMIN(max,25);
}

int _roll_stat( CHAR_DATA *ch, int stat )
{
    int low = 8, temp, high;
    high = pc_race_table[ch->race].max_stats[stat];
    temp = number_range(13,18) + pc_race_table[ch->race].stats[stat] + class_table[ch->class].stats[stat];
    if (class_table[ch->class].attr_prime == stat)
      temp += 1;
    return URANGE(low,temp,high);
}

int roll_stat( CHAR_DATA *ch ){
  return ( _roll_stat( ch, STAT_STR)
	   + _roll_stat( ch, STAT_INT)
	   + _roll_stat( ch, STAT_WIS)
	   + _roll_stat( ch, STAT_DEX)
	   + _roll_stat( ch, STAT_CON));
}

int can_carry_n( CHAR_DATA *ch )
{
  int items;
  if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET) )
    return 0;
  if ( !IS_NPC(ch) && IS_IMMORTAL(ch) )
    return 1000;

  items = MAX_WEAR - 13 + get_curr_stat(ch,STAT_DEX) + ch->size;

  if (IS_PERK(ch, PERK_NIMBLE))
    items = 6 * items / 5;

  return ( items );
}

int can_carry_w( CHAR_DATA *ch )
{
  int weight;
  if ( IS_NPC(ch)
       && (IS_SET(ch->act, ACT_PET) || IS_SET(ch->act2,ACT_DUPLICATE) )
       ){
    return 0;
  }
  if ( !IS_NPC(ch) && IS_IMMORTAL(ch) )
    weight = 32000;
  else
    weight = str_app[get_curr_stat(ch, STAT_STR)].carry * 10 + ch->level * 25;

  if (IS_PERK(ch, PERK_OVERWEIGHT))
    weight = 6 * weight / 5;

  return weight;
}

bool is_name ( char *str, char *namelist )
{
    char name[MIL], part[MIL];
    char *list, *string;
    if (namelist == NULL || namelist[0] == '\0' || str == NULL)
        return FALSE;
    if (str[0] == '\0')
        return TRUE;
    string = str;
    for ( ; ; )
    {
        str = one_argument(str,part);
        if (part[0] == '\0' )
            return TRUE;
        list = namelist;
        for ( ; ; )
        {
            list = one_argument(list,name);
            if (name[0] == '\0')
                return FALSE;
            if (!str_cmp(string,name))
                return TRUE;
	    if (!str_cmp(part,name))
                break;
        }
    }
}

bool is_auto_name ( char *str, char *namelist )
{
    char name[MIL], part[MIL];
    char *list, *string;

    if (namelist == NULL || namelist[0] == '\0')
        return FALSE;

    if (str[0] == '\0')
        return TRUE;
    string = str;

    for ( ; ; )
    {
        str = one_argument(str,part);
        if (part[0] == '\0' )
            return TRUE;
        list = namelist;

        for ( ; ; )
        {
            list = one_argument(list,name);
            if (name[0] == '\0')
                return FALSE;

            if (!str_prefix( string, name))
	    if (!str_prefix( part, name))
                break;
        }
    }
}

bool is_exact_name(char *str, char *namelist )
{
    char name[MIL];
    if (namelist == NULL)
	return FALSE;
    for ( ; ; )
    {
	namelist = one_argument( namelist, name );
	if ( name[0] == '\0' )
	    return FALSE;
	if ( !str_cmp( str, name ) )
	    return TRUE;
    }
}

void affect_enchant(OBJ_DATA *obj)
{
    if (!obj->enchanted)
    {
        AFFECT_DATA *paf, *af_new;
        obj->enchanted = TRUE;
        for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
        {
	    af_new = new_affect();
            af_new->next = obj->affected;
            obj->affected = af_new;
	    af_new->where	= paf->where;
            af_new->type        = UMAX(0,paf->type);
            af_new->level       = paf->level;
            af_new->duration    = paf->duration;
            af_new->location    = paf->location;
            af_new->modifier    = paf->modifier;
            af_new->bitvector   = paf->bitvector;
        }
    }
}

void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
    OBJ_DATA *wield;
    int mod = paf->modifier, i, weight;
    int sn = 0;
    if ( fAdd )
	switch (paf->where)
	{
        case TO_AFFECTS:
	  if (ch->race == race_lookup("demon")
	      && IS_SET(paf->bitvector, AFF_CURSE))
	    {
	      send_to_char("You cannot curse that which is already dammed.\n\r", ch);
	      break;
	    }
	  SET_BIT(ch->affected_by,paf->bitvector);  break;
        case TO_AFFECTS2:
	  SET_BIT(ch->affected2_by,paf->bitvector); break;
        case TO_IMMUNE:
	  SET_BIT(ch->imm_flags,paf->bitvector);    break;
        case TO_RESIST:
	  SET_BIT(ch->res_flags,paf->bitvector);    break;
        case TO_VULN:
	  SET_BIT(ch->vuln_flags,paf->bitvector);   break;
	case TO_SKILL:
	  if (IS_NPC(ch))
	    return;
	  sn = paf->location;
	  if (sn < -1 || sn > MAX_SKILL)
	    {
	      bug("affect_modify: TO_SKILL with bad skill number (%d).",sn);
	      return;
	    }
	  /* add on the modifier */
	  ch->pcdata->to_learned[sn] += mod;
	  break;
	}
    else
    {
        switch (paf->where)
        {
        case TO_AFFECTS:  REMOVE_BIT(ch->affected_by,paf->bitvector);  break;
        case TO_AFFECTS2: REMOVE_BIT(ch->affected2_by,paf->bitvector); break;
        case TO_IMMUNE:   REMOVE_BIT(ch->imm_flags,paf->bitvector);    break;
        case TO_RESIST:   REMOVE_BIT(ch->res_flags,paf->bitvector);    break;
        case TO_VULN:     REMOVE_BIT(ch->vuln_flags,paf->bitvector);   break;
	case TO_SKILL:
	  if (IS_NPC(ch))
	    return;
	  sn = paf->location;
	  if (sn < -1 || sn > MAX_SKILL)
	    {
	      bug("affect_modify: TO_SKILL with bad skill number (%d).",sn);
	      return;
	    }
	  /* remove on the modifier */
	  ch->pcdata->to_learned[sn] -= mod;
	  break;
        }
	if (paf->location == APPLY_SEX && !IS_NPC(ch))
	    mod = ch->pcdata->true_sex;
	else
	    mod = 0 - mod;
    }
    if (paf->location < APPLY_O_COND){
      switch ( paf->location )
	{
    default:
      if (paf->where != TO_SKILL && paf->where != TO_NONE){
	bug( "Affect_modify: unknown location on type %d.", paf->type );
	return;
      }
	case APPLY_NONE:                                  break;
	case APPLY_STR:    ch->mod_stat[STAT_STR] += mod; break;
	case APPLY_DEX:    ch->mod_stat[STAT_DEX] += mod; break;
	case APPLY_INT:    ch->mod_stat[STAT_INT] += mod; break;
	case APPLY_WIS:    ch->mod_stat[STAT_WIS] += mod; break;
	case APPLY_CON:    ch->mod_stat[STAT_CON] += mod; break;
	case APPLY_LUCK:   ch->mod_stat[STAT_LUCK] += mod; break;
	case APPLY_SEX:    change_sex(ch,mod); break;
	case APPLY_CLASS:                                 break;
	case APPLY_LEVEL:                                 break;
	case APPLY_AGE:	   ch->aff_mod[MOD_AGE]   += mod; break;
	case APPLY_SIZE:   ch->size               += mod; break;
	case APPLY_WEIGHT:                                break;
	case APPLY_MANA:   ch->max_mana           += mod; break;
	case APPLY_HIT:    ch->max_hit            += mod; break;
	case APPLY_MOVE:   ch->max_move           += mod; break;
	case APPLY_GOLD:                                  break;
	case APPLY_EXP:                                   break;
	case APPLY_AC:
	  for (i = 0; i < 4; i ++)
            ch->armor[i] += mod;
	  break;
	case APPLY_HITROLL:       ch->hitroll      += mod; break;
	case APPLY_DAMROLL:       ch->damroll      += mod; break;
	case APPLY_SAVING_AFFL:   ch->savingaffl   += mod; break;
	case APPLY_SAVING_MALED:  ch->savingmaled  += mod; break;
	case APPLY_SAVING_MENTAL: ch->savingmental += mod; break;
	case APPLY_SAVING_BREATH: ch->savingbreath += mod; break;
	case APPLY_SAVING_SPELL:  ch->savingspell  += mod; break;
	case APPLY_SPELL_AFFECT:                           break;
	case APPLY_SPELL_LEVEL:	ch->aff_mod[MOD_SPELL_LEVEL]  += mod; break;
	case APPLY_AFFL_LEVEL:	ch->aff_mod[MOD_AFFL_LEVEL]   += mod; break;
	case APPLY_MALED_LEVEL:	ch->aff_mod[MOD_MALED_LEVEL]  += mod; break;
	case APPLY_MENTAL_LEVEL:ch->aff_mod[MOD_MENTAL_LEVEL] += mod; break;
	case APPLY_SPELL_COST:  ch->aff_mod[MOD_SPELL_COST]   += mod; break;
	case APPLY_WAIT_STATE:	ch->aff_mod[MOD_WAIT_STATE]   += mod; break;
	case APPLY_SKILL_LEARN:	ch->aff_mod[MOD_SKILL_LEARN]  += mod; break;
	case APPLY_SPELL_LEARN:	ch->aff_mod[MOD_SPELL_LEARN]  += mod; break;
	case APPLY_HIT_GAIN:	ch->aff_mod[MOD_HIT_GAIN]     += mod; break;
	case APPLY_MANA_GAIN:	ch->aff_mod[MOD_MANA_GAIN]    += mod; break;
	case APPLY_MOVE_GAIN:	ch->aff_mod[MOD_MOVE_GAIN]    += mod; break;
	}
    }
    if (!IS_NPC(ch) && ch->pcdata->pStallion
	&& ch->pcdata->pStallion->size < ch->size ){
      sendf( ch, "You no longer fit upon %s!", ch->pcdata->pStallion->short_descr);
      do_dismount(ch, "");
    }
    if ( !IS_NPC(ch) && ( wield = get_eq_char( ch, WEAR_WIELD ) ) != NULL)
    {
    	weight = (str_app[get_curr_stat(ch,STAT_STR)].wield*10);
        if ((IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS)
	     && ch->size < SIZE_LARGE)
	    ||wield->value[0] == WEAPON_STAFF
	    || wield->value[0] == WEAPON_POLEARM
	    || wield->item_type == ITEM_INSTRUMENT
	    || is_affected(ch,gsn_double_grip))
	  weight = 3 * weight /2;
/* CRUSADER 2h Handling bonus */
	if (wield->item_type == ITEM_WEAPON
	    && get_skill(ch, gsn_2h_handling) > 0
	    && (IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS)
		|| is_affected(ch, gsn_double_grip)) )
	  weight = 3 * weight / 2;
    	if (get_obj_weight(wield) > weight
	    && wield->pIndexData->vnum != OBJ_VNUM_CHAOS_BLADE
	    && !is_affected(ch, gsn_graft_weapon)
	    && !is_affected_obj(wield, gen_dark_meta)
	    && !is_affected_obj(wield, gsn_curse_weapon))
	{
	  static int depth;
	  if ( depth == 0 )
	    {
	      depth++;
	      act( "$p is too heavy for you to wield.", ch, wield, NULL, TO_CHAR );
	      act( "$n almost drops $p.", ch, wield, NULL, TO_ROOM );
	      unequip_char( ch, wield );
	      if ( wield->pIndexData->message )
		{
		  act( wield->pIndexData->message->offself, ch, wield,NULL, TO_CHAR);
		  act( wield->pIndexData->message->offother,  ch, wield,NULL, TO_ROOM);
		}
	      depth--;
	    }
	}
    }
    if ( !IS_NPC(ch) && ( wield = get_eq_char( ch, WEAR_SECONDARY ) ) != NULL
	 && get_obj_weight(wield) > (str_app[get_curr_stat(ch,STAT_STR)].wield*10)
	 && wield->pIndexData->vnum != OBJ_VNUM_CHAOS_BLADE
	 && !is_affected(ch, gsn_graft_weapon)
	 && !is_affected_obj(wield, gen_dark_meta)
	 && !is_affected_obj(wield, gsn_curse_weapon))
      {
	static int depth;
	if ( depth == 0 )
	{
	    depth++;
	    act( "$p is too heavy for you to wield.", ch, wield, NULL, TO_CHAR );
	    act( "$n almost drops $p.", ch, wield, NULL, TO_ROOM );
	    unequip_char( ch, wield );
	    if ( wield->pIndexData->message )
	    {
	        act( wield->pIndexData->message->offself, ch, wield,NULL, TO_CHAR);
	        act( wield->pIndexData->message->offother,  ch, wield,NULL, TO_ROOM);
	    }
	    depth--;
	}
    }
    if ( !IS_NPC(ch) && ( wield = get_eq_char( ch, WEAR_SHIELD ) ) != NULL
    && get_obj_weight(wield) > (str_app[get_curr_stat(ch,STAT_STR)].wield*12))
    {
	static int depth;
	if ( depth == 0 )
	{
	    depth++;
	    act( "You no longer have the strength to hold up $p.", ch, wield, NULL, TO_CHAR );
	    act( "$n almost drops $p.", ch, wield, NULL, TO_ROOM );
	    unequip_char( ch, wield );
	    if ( wield->pIndexData->message )
	    {
	        act( wield->pIndexData->message->offself, ch, wield,NULL, TO_CHAR);
	        act( wield->pIndexData->message->offother,  ch, wield,NULL, TO_ROOM);
	    }
	    depth--;
	}
    }
    if ( !IS_NPC(ch) && ( wield = get_eq_char( ch, WEAR_HOLD ) ) != NULL
    && get_obj_weight(wield) > (str_app[get_curr_stat(ch,STAT_STR)].wield*10))
    {
	static int depth;
	if ( depth == 0 )
	{
	    depth++;
	    act( "You no longer have the strength to hold up $p.", ch, wield, NULL, TO_CHAR );
	    act( "$n almost drops $p.", ch, wield, NULL, TO_ROOM );
	    unequip_char( ch, wield );
	    if ( wield->pIndexData->message )
	    {
	        act( wield->pIndexData->message->offself, ch, wield,NULL, TO_CHAR);
	        act( wield->pIndexData->message->offother,  ch, wield,NULL, TO_ROOM);
	    }
	    depth--;
	}
    }
}

void affect_modify_obj( OBJ_DATA *obj, AFFECT_DATA *paf, bool fAdd ){
  CHAR_DATA* on_ch;
  OBJ_DATA* on_obj;
  int mod = fAdd ? paf->modifier : -paf->modifier;
  int i;


/* safety checks */
  if (obj->item_type != ITEM_WEAPON
      && obj->item_type != ITEM_THROW
      && paf->where == TO_WEAPON){
    bug("affect_modify_obj: TO_WEAPON on not weapon, ovnum: %d", obj->pIndexData->vnum);
    return;
  }

/* FLAGS (bitvector)*/
  if ( fAdd ){
    switch (paf->where){
    case TO_OBJECT:	SET_BIT(obj->extra_flags, paf->bitvector);	break;
    case TO_WEAPON:	SET_BIT(obj->value[4],paf->bitvector);		break;
    case TO_WEAR:	SET_BIT(obj->wear_flags,paf->bitvector);	break;
    }//END ADD SWITCH
  }
  else{
    switch (paf->where){
    case TO_OBJECT: REMOVE_BIT(obj->extra_flags, paf->bitvector);	break;
    case TO_WEAPON: REMOVE_BIT(obj->value[4],paf->bitvector);		break;
    case TO_WEAR:   REMOVE_BIT(obj->wear_flags,paf->bitvector);	break;
    }//END REMOVE SWITCH
  }

/* make sure various things are updated right */
  on_obj = in_obj(obj);
  if ( (on_ch = in_char(on_obj)) != NULL){
    int ac_mod = 0;
    int iWear = obj->wear_loc;
/* WEIGHT */
    on_ch->carry_weight -= get_obj_weight_char(on_ch, on_obj);
    if ((iWear == WEAR_BODY || iWear == WEAR_HEAD || iWear == WEAR_FACE
	 || iWear == WEAR_ARMS || iWear == WEAR_LEGS)
	&& get_skill(on_ch, gsn_adv_armor) > 1 )
      ac_mod = 1;
/* AC */
    if (obj->wear_loc != -1){
      for (i = 0; i < 4; i ++)
	on_ch->armor[i] += apply_ac( obj, iWear, i, ac_mod);
    }
  }

/* NUMERICS (modifier) */
  switch ( paf->location ){
  case APPLY_NONE:							break;
  case APPLY_O_COND:		obj->condition	+= mod;			break;
  case APPLY_O_LEVEL:		obj->level	+= mod;			break;
  case APPLY_O_WEIGHT:		obj->weight	+= mod;			break;
  case APPLY_O_COST:		obj->cost	+= mod;			break;
  case APPLY_O_VAL0:		obj->value[0]	+= mod;			break;
  case APPLY_O_VAL1:		obj->value[1]	+= mod;			break;
  case APPLY_O_VAL2:		obj->value[2]	+= mod;			break;
  case APPLY_O_VAL3:		obj->value[3]	+= mod;			break;
  case APPLY_O_VAL4:		obj->value[4]	+= mod;			break;
  case APPLY_O_DTYPE:
    if (paf->where == TO_WEAPON){
      if (fAdd)
	obj->value[3] = mod;
      else{
	OBJ_INDEX_DATA* pIndex = get_obj_index ( obj->vnum );
	obj->value[3] = pIndex != NULL ? pIndex->value[3] : obj->pIndexData->value[3];
      }
    }
    break;
  }

  /* make sure we update things that we took off before */
  if ( on_ch != NULL){
    int ac_mod = 0;
    int iWear = obj->wear_loc;
/* WEIGHT */
    on_ch->carry_weight += get_obj_weight_char(on_ch, on_obj);
/* AC */
    if ((iWear == WEAR_BODY || iWear == WEAR_HEAD || iWear == WEAR_FACE
	 || iWear == WEAR_ARMS || iWear == WEAR_LEGS)
	&& get_skill(on_ch, gsn_adv_armor) > 1 )
      ac_mod = 1;
    if (obj->wear_loc != -1){
      for (i = 0; i < 4; i ++)
	on_ch->armor[i] -= apply_ac( obj, iWear, i, ac_mod);
    }
  }
}

/* obj_spell_find */
/* Viri 05-00  */
OBJ_SPELL_DATA  *spell_obj_find(OBJ_SPELL_DATA *spell, int sn)
{
    OBJ_SPELL_DATA *spell_find;
    for ( spell_find =spell; spell_find != NULL; spell_find = spell_find->next )
    {
        if ( spell_find->spell == sn )
	return spell_find;
    }
    return NULL;
}


AFFECT_DATA  *affect_find(AFFECT_DATA *paf, int sn)
{
    AFFECT_DATA *paf_find;
    for ( paf_find = paf; paf_find != NULL; paf_find = paf_find->next )
    {
        if ( paf_find->type == sn )
	return paf_find;
    }
    return NULL;
}

/* returns first paf with given location */
AFFECT_DATA  *affect_loc_find(AFFECT_DATA *paf, int location)
{
    AFFECT_DATA *paf_find;
    for ( paf_find = paf; paf_find != NULL; paf_find = paf_find->next )
    {
        if ( paf_find->location == location )
	return paf_find;
    }
    return NULL;
}

/* returns sum of modifiers of given location (ex: all damroll modifiers) */
int affect_loc_add(AFFECT_DATA *paf, int location)
{
    AFFECT_DATA *paf_find;
    int sum = 0;
    for ( paf_find = paf; paf_find != NULL; paf_find = paf_find->next )
    {
        if ( paf_find->location == location )
	  sum += paf_find->modifier;
    }
    return sum;
}

void affect_check(CHAR_DATA *ch,int where,int vector)
{
    AFFECT_DATA *paf;
    OBJ_DATA *obj;
    if (where == TO_NONE || where == TO_OBJECT || where == TO_WEAPON
	|| where == TO_WEAR || vector == 0)
      return;
    /* Avatars Effects */
    if (IS_AVATAR(ch))
      update_avatar(ch, FALSE);

    ch->affected_by = ch->affected_by|race_table[ch->race].aff;
    for (paf = ch->affected; paf != NULL; paf = paf->next)
	if (paf->where == where && paf->bitvector == vector)
	{
	    switch (where)
	    {
                case TO_AFFECTS:  SET_BIT(ch->affected_by,vector);  break;
                case TO_AFFECTS2: SET_BIT(ch->affected2_by,vector); break;
                case TO_IMMUNE:   SET_BIT(ch->imm_flags,vector);    break;
                case TO_RESIST:   SET_BIT(ch->res_flags,vector);    break;
                case TO_VULN:     SET_BIT(ch->vuln_flags,vector);   break;
	    }
	    return;
        }
    for (paf = ch->affected2; paf != NULL; paf = paf->next)
	if (paf->where == where && paf->bitvector == vector)
	{
	    switch (where)
	    {
                case TO_AFFECTS:  SET_BIT(ch->affected_by,vector);  break;
                case TO_AFFECTS2: SET_BIT(ch->affected2_by,vector); break;
                case TO_IMMUNE:   SET_BIT(ch->imm_flags,vector);    break;
                case TO_RESIST:   SET_BIT(ch->res_flags,vector);    break;
                case TO_VULN:     SET_BIT(ch->vuln_flags,vector);   break;
	    }
	    return;
        }
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
	if (obj->wear_loc == -1)
	    continue;
        for (paf = obj->affected; paf != NULL; paf = paf->next)
            if (paf->where == where && paf->bitvector == vector)
            {
                switch (where)
                {
                    case TO_AFFECTS:  SET_BIT(ch->affected_by,vector);  break;
                    case TO_AFFECTS2: SET_BIT(ch->affected2_by,vector); break;
                    case TO_IMMUNE:   SET_BIT(ch->imm_flags,vector);    break;
                    case TO_RESIST:   SET_BIT(ch->res_flags,vector);    break;
                    case TO_VULN:     SET_BIT(ch->vuln_flags,vector);   break;
                }
                return;
            }
        if (obj->enchanted)
	    continue;
        for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
            if (paf->where == where && paf->bitvector == vector)
            {
                switch (where)
                {
                    case TO_AFFECTS:  SET_BIT(ch->affected_by,vector);  break;
                    case TO_AFFECTS2: SET_BIT(ch->affected2_by,vector); break;
                    case TO_IMMUNE:   SET_BIT(ch->imm_flags,vector);    break;
                    case TO_RESIST:   SET_BIT(ch->res_flags,vector);    break;
                    case TO_VULN:     SET_BIT(ch->vuln_flags,vector);   break;
                }
                return;
            }
    }

}
/* checks over damage type effects on weapons */
void weapon_affect_check(OBJ_DATA* obj, int location, int modifier){
  AFFECT_DATA* paf;

  if (location != APPLY_O_DTYPE
      || obj->item_type != ITEM_WEAPON)
    return;
/*
all we do is add the modifier of the dt removed onto any other one
to perserve same offset from index
*/
  for (paf = obj->affected; paf != NULL; paf = paf->next){
    if (paf->location == location){
      obj->value[3] = paf->modifier;
      return;
    }
  }
}

/* checks over pafs on the object and sockets */
void affect_check_obj(OBJ_DATA *obj, int where, int vector)
{
    AFFECT_DATA *paf;
    if ( (where != TO_NONE
	  && where != TO_OBJECT
	  && where != TO_WEAPON
	  && where != TO_WEAR) ||  vector == 0)
      return;

    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next){
      if (paf->where == where && paf->bitvector == vector){
	affect_modify_obj(obj, paf, TRUE);
      }
    }
    for (paf = obj->affected; paf != NULL; paf = paf->next){
      if (paf->where == where && paf->bitvector == vector){
	affect_modify_obj(obj, paf, TRUE);
      }
    }
    if (IS_OBJ_STAT(obj, ITEM_SOCKETABLE)
	&& obj->contains
	&& obj->contains->item_type == ITEM_SOCKET){
      unadorn_obj(obj->contains, obj, NULL);
      adorn_obj(obj->contains, obj, NULL);
    }
}

AFFECT_DATA* affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_new = new_affect();
    *paf_new		= *paf;

//due to the ability of code to recycle affects
//the pointers tend to end up floating after reboots.
//So we stick in double safety system.
    paf_new->string = &str_empty[0];
    paf_new->has_string = FALSE;

    VALIDATE(paf_new);
    paf_new->next	= ch->affected;
    ch->affected	= paf_new;
    affect_modify( ch, paf_new, TRUE );

//GEN EFF_UPDATE_INIT
if (IS_GEN(paf->type))
  run_effect_update(ch, paf_new, NULL, paf_new->type, NULL, NULL, TRUE, EFF_UPDATE_INIT);
 return paf_new;
}

void song_affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_new = new_affect();
    *paf_new		= *paf;

//due to the ability of code to recycle affects
//the pointers tend to end up floating after reboots.
//So we stick in double safety system.
    paf_new->string = &str_empty[0];
    paf_new->has_string = FALSE;

    VALIDATE(paf_new);
    paf_new->next	= ch->affected2;
    ch->affected2	= paf_new;

    affect_modify( ch, paf_new, TRUE );
}


AFFECT_DATA* affect_to_obj(OBJ_DATA *obj, AFFECT_DATA *paf)
{

    AFFECT_DATA *paf_new = new_affect();
    *paf_new		= *paf;
//due to the ability of code to recycle affects
//the pointers tend to end up floating after reboots.
//So we stick in double safety system.
    paf_new->string = &str_empty[0];
    paf_new->has_string = FALSE;

    VALIDATE(paf);
    paf_new->next	= obj->affected;
    obj->affected	= paf_new;
    affect_modify_obj(obj, paf, TRUE);
    if (obj->carried_by != NULL && obj->wear_loc != -1)
      affect_modify( obj->carried_by, paf, TRUE );

//GEN EFF_UPDATE_INIT
    if (IS_GEN(paf->type))
      run_effect_update(NULL, paf_new, obj, paf->type, NULL, NULL, TRUE, EFF_UPDATE_INIT);
    return paf_new;
}


OBJ_SPELL_DATA* spell_to_obj(OBJ_DATA *obj, OBJ_SPELL_DATA *sp, bool bDup)
{

    OBJ_SPELL_DATA *sp_new = new_obj_spell();
    *sp_new		= *sp;
//check if item has pIndex spells.
    if (!obj->eldritched && (obj->pIndexData->spell != NULL))
      {
//copy spells over
	OBJ_SPELL_DATA *sp_over;
	OBJ_SPELL_DATA *sp_index;

	for (sp_index = obj->pIndexData->spell; sp_index != NULL; sp_index = sp_index->next)
	  {
	    sp_over = new_obj_spell();
	    sp_over->next = obj->spell;
	    obj->spell = sp_over;

	    sp_over->spell  = sp_index->spell;
	    sp_over->target  = sp_index->target;
	    sp_over->percent  = sp_index->percent;
	    //we dont have to copy messages are pointers have to exist.
	    sp_over->message  = sp_index->message;
	    sp_over->message2  = sp_index->message2;
	  }//end copy spells.
	obj->eldritched = TRUE;
      }//end if not eldritched.

//we reset the strings just in case.
    sp_new->message = NULL;
    sp_new->message2 = NULL;


//now we pin this spell up.
    //if no doubles.
    if (!bDup)
      {
	OBJ_SPELL_DATA* bsp;
	if ( (bsp = spell_obj_find(obj->spell, sp_new->spell)) != NULL)
	  {
	    bsp->target = sp_new->target;
	    bsp->percent = sp_new->percent;
	  }
	else
	  {
	    sp_new->next	= obj->spell;
	    obj->spell		= sp_new;
	  }
      }
    else
      {
	sp_new->next	= obj->spell;
	obj->spell		= sp_new;
      }
    obj->eldritched = TRUE;
    return sp_new;
}

void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf)
{
    int where, vector;
    if ( ch->affected == NULL )
    {
	bug( "Affect_remove: no affect from %d.", 0);
	return;
    }
    affect_modify( ch, paf, FALSE );

    where = paf->where;
    vector = paf->bitvector;
    if ( paf == ch->affected )
	ch->affected	= paf->next;
    else
    {
	AFFECT_DATA *prev;
	for ( prev = ch->affected; prev != NULL; prev = prev->next )
	    if ( prev->next == paf )
	    {
		prev->next = paf->next;
		break;
	    }
	if ( prev == NULL )
	{
	    bug( "Affect_remove: cannot find paf %d.", paf->type);
	    return;
	}
    }
    if (paf->location == APPLY_SEX)
	fixed_sex(ch);

//We kill effects AFTER they have been removed from char.
//GEN EFF_UPDATE_KILL
    if (IS_GEN(paf->type))
      if (!(IS_SET(effect_table[GN_POS(paf->type)].flags, EFF_F1_NOEXTRACT) && ch->extracted == TRUE) )
	run_effect_update(ch, paf, NULL, paf->type, NULL, NULL, TRUE, EFF_UPDATE_KILL);

    if (ch && ch->in_room && (paf->type == gsn_mist_form) && is_affected(ch, gsn_burrow))
      {
 //we check if thi is a mist form getting reverted..
        free_affect(paf);
        affect_check(ch,where,vector);
	do_unburrow(ch, NULL);
      }
    else
      {
//proceed normaly.
        free_affect(paf);
	if (ch && ch->in_room)
	  affect_check(ch,where,vector);
      }
}


void song_affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf)
{
    int where, vector;
    if ( ch->affected2 == NULL )
    {
	bug( "Affect_remove: no affect2 from %d.", 0);
	return;
    }
    affect_modify( ch, paf, FALSE );
    where = paf->where;
    vector = paf->bitvector;
    if ( paf == ch->affected2 )
	ch->affected2	= paf->next;
    else
    {
	AFFECT_DATA *prev;
	for ( prev = ch->affected2; prev != NULL; prev = prev->next )
	    if ( prev->next == paf )
	    {
		prev->next = paf->next;
		break;
	    }
	if ( prev == NULL )
	{
	    bug( "Affect_remove_song: cannot find paf %d.", 0);
	    return;
	}
    }
    if (paf->location == APPLY_SEX)
	fixed_sex(ch);
    free_affect(paf);
    affect_check(ch,where,vector);
}

void spell_obj_remove(OBJ_DATA* obj, OBJ_SPELL_DATA* spell)
{
//now we do removing.
    if ( spell == obj->spell )
      obj->spell    = spell->next;
    else
    {
        OBJ_SPELL_DATA *prev;
        for ( prev = obj->spell; prev != NULL; prev = prev->next )
            if ( prev->next == spell )
            {
                prev->next = spell->next;
                break;
            }
        if ( prev == NULL )
        {
            bug( "spell_obj_remove: cannot find spell.", 0 );
            return;
        }
    }
    free_obj_spell(spell);
}

/* spell_obj_strip */
/* written by viri */
void spell_obj_strip(OBJ_DATA* obj, int spell)
{
    OBJ_SPELL_DATA *sp, *sp_next;
//check if item has pIndex spells.
    if (!obj->eldritched)
      {
//copy spells over
	OBJ_SPELL_DATA *sp_over;
	OBJ_SPELL_DATA *sp_index;

	for (sp_index = obj->pIndexData->spell; sp_index != NULL; sp_index = sp_index->next)
	  {
	    sp_over = new_obj_spell();
	    sp_over->next = obj->spell;
	    obj->spell = sp_over;

	    sp_over->spell  = sp_index->spell;
	    sp_over->target  = sp_index->target;
	    sp_over->percent  = sp_index->percent;
	    //we dont have to copy messages are pointers have to exist.
	    sp_over->message  = sp_index->message;
	    sp_over->message2  = sp_index->message2;
	  }//end copy spells.
	obj->eldritched = TRUE;
      }//end if not eldritched.


    for ( sp = obj->spell; sp != NULL; sp = sp_next )
    {
	sp_next = sp->next;
	if ( sp->spell == spell )
	    spell_obj_remove( obj, sp);
    }
}

void affect_remove_obj( OBJ_DATA *obj, AFFECT_DATA *paf)
{
  int  where = paf->where;
  int  vector = paf->bitvector;
  int location = paf->location;
  int modifier = paf->modifier;

  if ( obj->affected == NULL ){
    bug( "Affect_remove_object: no affect.", 0 );
    return;
  }
  affect_modify_obj(obj, paf, FALSE);
  if (obj->carried_by != NULL && obj->wear_loc != -1)
    affect_modify( obj->carried_by, paf, FALSE );
  if ( paf == obj->affected )
    obj->affected	= paf->next;
  else{
    AFFECT_DATA *prev;
    for ( prev = obj->affected; prev != NULL; prev = prev->next )
      if ( prev->next == paf ){
	prev->next = paf->next;
	break;
      }
    if ( prev == NULL ){
      bug( "Affect_remove_obj: cannot find paf %d.", 0);
      return;
    }
  }
  if (paf->location == APPLY_SEX && obj->carried_by != NULL && obj->wear_loc != -1)
    fixed_sex(obj->carried_by);
//We kill effects AFTER they have been removed from obj.
//GEN EFF_UPDATE_KILL
  if (IS_GEN(paf->type))
    if ( !(IS_SET(effect_table[GN_POS(paf->type)].flags, EFF_F1_NOEXTRACT) && obj->extracted) )
      run_effect_update(NULL, paf, obj, paf->type, NULL, NULL, TRUE, EFF_UPDATE_KILL);

  free_affect(paf);
  affect_check_obj(obj, where, vector);
  weapon_affect_check(obj, location, modifier);
  if (obj->carried_by != NULL && obj->wear_loc != -1)
    affect_check(obj->carried_by,where,vector);
}

void affect_strip( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf, *paf_next;
    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
	paf_next = paf->next;
	if ( paf->type == sn )
	    affect_remove( ch, paf);
    }
}

void song_affect_strip( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf, *paf_next;
    for ( paf = ch->affected2; paf != NULL; paf = paf_next )
    {
	paf_next = paf->next;
	if ( paf->type == sn )
	    song_affect_remove( ch, paf);
    }
}

bool is_affected( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;
    for ( paf = ch->affected; paf != NULL; paf = paf->next )
	if ( paf->type == sn )
	    return TRUE;
    return FALSE;
}

bool is_song_affected( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;
    for ( paf = ch->affected2; paf != NULL; paf = paf->next )
	if ( paf->type == sn )
	    return TRUE;
    return FALSE;
}

bool is_affected_obj( OBJ_DATA *obj, int sn )
{
    AFFECT_DATA *paf;
    if (obj == NULL)
	return FALSE;
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
	if ( paf->type == sn )
	    return TRUE;
    return FALSE;
}

void affect_strip_obj( OBJ_DATA *obj, int sn )
{
    AFFECT_DATA *paf, *paf_next;
    for ( paf = obj->affected; paf != NULL; paf = paf_next )
    {
	paf_next = paf->next;
	if ( paf->type == sn )
	    affect_remove_obj( obj, paf );
    }
}

AFFECT_DATA* affect_join( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_old, *paf_next;
    for ( paf_old = ch->affected; paf_old != NULL; paf_old = paf_next ){
      paf_next = paf_old->next;
      if ( paf_old->type == paf->type
	   && paf->location == paf_old->location){
	paf->level = (paf->level + paf_old->level) / 2;
	paf->duration += paf_old->duration;
	paf->modifier += paf_old->modifier;
	affect_remove( ch, paf_old);
      }
    }
    return affect_to_char( ch, paf );
}

AFFECT_DATA* affect_join_obj( OBJ_DATA *obj, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_old;
    for ( paf_old = obj->affected; paf_old != NULL; paf_old = paf_old->next )
	if ( paf_old->type == paf->type
	     && paf->location == paf_old->location)
	{
	    /* Kaslai:  Someone was trying to be clever here, using a += instead of
                    a +. If issues arise around this, revert it.                */
	    paf->level = (paf->level + paf_old->level) / 2;
	    paf->duration += paf_old->duration;
	    paf->modifier += paf_old->modifier;
	    affect_remove_obj( obj, paf_old);
	    //	    break;
	}
    return affect_to_obj( obj, paf );
}

void song_affect_join( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_old;
    for ( paf_old = ch->affected2; paf_old != NULL; paf_old = paf_old->next )
	if ( paf_old->type == paf->type )
	{
	    /* Kaslai:  Someone was trying to be clever here, using a += instead of
                    a +. If issues arise around this, revert it.                */
	    paf->level = (paf->level + paf_old->level) / 2;
	    paf->duration += paf_old->duration;
	    paf->modifier += paf_old->modifier;
	    song_affect_remove( ch, paf_old);
	    break;
	}
    song_affect_to_char( ch, paf );
}

void ch_from_room( CHAR_DATA *ch ){
  OBJ_DATA* obj;

  if (!IS_NPC(ch)){
    --ch->in_room->area->nplayer;
  }

  /* Item Light */
  if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
       && obj->item_type == ITEM_LIGHT
       && obj->value[2] != 0)
    if (--ch->in_room->light < -20)ch->in_room->light = -20;

  /* AVATARS glow giving light */
  if (IS_SET(ch->act2, PLR_AVATAR))
    if (--ch->in_room->light < -20)ch->in_room->light = -20;

  if ( ch == ch->in_room->people )
    ch->in_room->people = ch->next_in_room;
  else{
    CHAR_DATA *prev;
    for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
      if ( prev->next_in_room == ch ){
	prev->next_in_room = ch->next_in_room;
	break;
      }
    if ( prev == NULL )
      bug( "Char_from_room: ch not found.", 0);
  }
  ch->in_room      = NULL;
  ch->next_in_room = NULL;
}

void char_from_room( CHAR_DATA *ch ){
  ROOM_INDEX_DATA* old_room =ch->in_room;

  if ( ch->in_room == NULL ){
    bug( "Char_from_room: NULL.", 0 );
    return;
  }
  //UPDATE_LEAVE (before the exit messages)
  //arg1 is the in_room room,
  if ( LEAVE_CHECK(ch, ch->in_room) == -1)
    bug("Error at: LEAVE_CHECK returned failure.\n\r", 0);

  if (ch == NULL || ch->in_room == NULL || ch->in_room != old_room)
    return;

  if ( !IS_NPC(ch) ){
    AFFECT_DATA *divine;
    if ((divine = affect_find(ch->affected,skill_lookup("divine retribution"))) != NULL){
      if (divine->duration > 0)
	divine->duration -= divine->duration > 1 ? number_range(1,2) : 1;
      else if (divine->duration <= 0){
	affect_strip(ch,skill_lookup("divine retribution"));
	send_to_char("You are no longer protected by the wrath of the gods.\n\r",ch);
      }
    }
    if (is_affected(ch,gsn_trap_silvanus)){
      send_to_char("Your trap falls apart!\n\r",ch);
      WAIT_STATE(ch,24);
      affect_strip(ch,gsn_trap_silvanus);
    }
    if (is_affected(ch,gsn_deadfall)){
      send_to_char("Your deadfall loses its potency.\n\r",ch);
      WAIT_STATE(ch,24);
      affect_strip(ch,gsn_deadfall);
    }
  }
  else{
    /* check if this is a mounted mob leaving its master in its room */
    if (ch->summoner
	&& !IS_NPC(ch->summoner)
	&& ch->summoner->pcdata->pStallion
	&& ch->summoner->pcdata->pStallion == ch->pIndexData
	&& ch->summoner->in_room == ch->in_room){
      send_to_char("You dismount.\n\r", ch->summoner);
      ch->summoner->pcdata->pStallion = NULL;
    }
  }

  ch_from_room( ch );
}

void ch_to_room( CHAR_DATA* ch, ROOM_INDEX_DATA* pRoomIndex){
  OBJ_DATA *obj;

  if ( pRoomIndex == NULL ){
    ROOM_INDEX_DATA *room;
    bug( "Char_to_room: NULL.", 0 );
    if ((room = get_room_index(ROOM_VNUM_TEMPLE)) != NULL)
      ch_to_room(ch,room);
    return;
  }
  if (pRoomIndex->vnum != ROOM_VNUM_LIMBO && !IS_VIRROOM( pRoomIndex ))
    ch->was_in_room  = pRoomIndex;
  ch->in_room		= pRoomIndex;
  ch->next_in_room	= pRoomIndex->people;
  pRoomIndex->people	= ch;

  if ( !IS_NPC(ch) ){
    ch->in_room->area->idle = 0;
    if (ch->in_room->area->empty){
      ch->in_room->area->empty = FALSE;
      ch->in_room->area->age = 0;
    }
    ++ch->in_room->area->nplayer;
  }

  if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
       && obj->item_type == ITEM_LIGHT
       && obj->value[2] != 0)
    if (++ch->in_room->light > 20)ch->in_room->light = 20;

  /* AVATARS glow giving light */
  if (IS_SET(ch->act2, PLR_AVATAR))
    if (++ch->in_room->light > 20)ch->in_room->light = 20;

}

void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
  ch_to_room( ch, pRoomIndex );

    if (!IS_NPC(ch)){
      if (IS_SET(ch->in_room->room_flags2, ROOM_ALARM)){
	char buf[MSL];
	sprintf(buf,"Alarm! Alarm! %s@%s has entered room %d!", ch->name, ch->desc->host, ch->in_room->vnum);
	log_string(buf);
	wiznet(buf,NULL,NULL,WIZ_LOGINS,0,get_trust(ch));
      }
    }
    /* If this is ALTAR, set the pointer for cabal */
    else if (ch->pCabal && ch->pIndexData->vnum == MOB_VNUM_ALTAR){
      if (ch->pCabal->altar != NULL && ch->pCabal->altar != ch){
	bug("char_to_room: multiple altars for cabal %d!", ch->pCabal->vnum);
      }
      else
	ch->pCabal->altar = ch;
    }

    if (IS_AFFECTED(ch,AFF_PLAGUE) || is_affected(ch, gsn_plague))
    {
      AFFECT_DATA *bpaf;
      AFFECT_DATA *paf = NULL;
      AFFECT_DATA *paf2 = NULL;
      AFFECT_DATA plague;
      CHAR_DATA *vch;


  /* we look for the plauge damage counter */
      for ( bpaf = ch->affected; bpaf != NULL; bpaf = bpaf->next ){
	if (bpaf->type == gsn_plague
	    && bpaf->where == TO_NONE
	    && bpaf->location == APPLY_NONE)
	  paf = bpaf;

    /* we look for the plauge effect  we only need this to increase
       level of effect*/
	if (bpaf->type == gsn_plague
	    && bpaf->where == TO_AFFECTS
	    && bpaf->location == APPLY_STR)
	  paf2 = bpaf;
  }//end plauge search

      if (paf == NULL || paf2 == NULL)
	{
	  REMOVE_BIT(ch->affected_by,AFF_PLAGUE);
	  return;
	}

      if (paf->level < 1)
	return;

  /* try to infect others */

      plague.where        = TO_AFFECTS;
      plague.type         = gsn_plague;
      plague.level        = UMIN(60, 2 * paf->level/ 3);
      plague.duration 	= paf->level / 4;
      plague.location     = APPLY_STR;
      plague.modifier 	= - paf->level / 10;
      plague.bitvector 	= AFF_PLAGUE;

      for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
	{
	  if (is_safe(ch,vch)
	      || (IS_NPC(ch) && vch->level < 10)
	      || saves_spell(plague.level,vch,DAM_DISEASE,skill_table[gsn_plague].spell_type)
	      || IS_IMMORTAL(vch)
	      || IS_AFFECTED(vch,AFF_PLAGUE)
	      || number_bits(4) == 0)
	    continue;

	  /* make plague weak for npc to pc spread */
	  if (IS_NPC(ch) && !IS_NPC(vch)){
	    plague.modifier = UMIN(20, plague.modifier);
	    plague.duration = UMIN(5, plague.duration);
	  }

	  act_new("You feel hot and feverish.",vch,NULL,NULL,TO_CHAR,POS_DEAD);
	  act("$n shivers and looks very ill.",vch,NULL,NULL,TO_ROOM);
	  affect_to_char(vch,&plague);

	  /* attach the damage counter */
	  plague.where = TO_NONE;
	  plague.level /= 2;
	  plague.location = APPLY_NONE;
	  plague.modifier 	= 1;
	  plague.bitvector 	= 0;
	  bpaf = affect_to_char(vch,&plague);
	  if (paf->has_string)
	    string_to_affect(bpaf, paf->string);
	}//end try to infect

    }



//UPDATE_ENTER
//arg1 is the in_room room, arg2 is the to_room
	if ( ENTER_CHECK(ch, ch->in_room) == -1)
	  bug("Error at: ENTER_CHECK returned failure.\n\r", 0);
}

void obj_to_ch( OBJ_DATA *obj, CHAR_DATA *ch )
{
/* safety */
  if (obj->in_room){
    bug("obj_to_ch: obj %d still in room.", obj->pIndexData->vnum);
    obj_from_room( obj );
  }
  if (obj->in_obj){
    bug("obj_to_ch: obj %d still in obj.", obj->pIndexData->vnum);
    obj_from_obj( obj );
  }
  if (obj->carried_by){
    bug("obj_to_ch: obj %d still on char.", obj->pIndexData->vnum);
    obj_from_char( obj );
  }

  obj->next_content	 = ch->carrying;
  ch->carrying	 = obj;
  obj->carried_by	 = ch;
  obj->in_room	 = NULL;
  obj->in_obj		 = NULL;

  if (obj->vnum != OBJ_VNUM_LOCKER){
    ch->carry_number	+= get_obj_number( obj );
    ch->carry_weight	+= get_obj_weight_char( ch, obj );
  }

  /* Cheat check: Viri */
  if (IS_SET(obj->wear_flags, ITEM_HAS_OWNER))
    REMOVE_BIT(obj->extra_flags, ITEM_STAIN);
  if ( (IS_IMMORTAL(ch) || (IS_NPC(ch) && ch->desc
			    && !is_affected(ch, gsn_linked_mind)))
       && IS_LIMITED(obj)
       && !IS_SET(obj->extra_flags, ITEM_STAIN)){
    char buf[MIL];
    SET_BIT(obj->extra_flags, ITEM_STAIN);
    sprintf(buf, "%s stained by %s.", obj->short_descr, PERS2(ch));
    send_to_char(buf, ch);
    log_string(buf);
  }
  else if (IS_SET(obj->extra_flags, ITEM_STAIN)
	   && !IS_IMMORTAL(ch)
	   && !IS_SET(ch->game, GAME_STAIN)){
    char buf[MIL];
    SET_BIT(ch->game, GAME_STAIN);
    sprintf(buf, "%s stained by %s.", PERS2(ch), obj->short_descr);
    send_to_char(buf, ch);
    log_string(buf);
  }

}

void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
    //Mirror dummy check
    if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_DUMMY)
      {
	act("$n seems to not notice $p and drops it.", ch, obj, NULL, TO_ROOM);
	send_to_char("You may not recive items.\n\r", ch);
	obj_to_room(obj, ch->in_room);
	return;
      }

    obj_to_ch(obj, ch);

    if ( !can_take ( ch, obj))
      {
	act( "You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR );
	act( "$n is zapped by $p and drops it.",  ch, obj, NULL, TO_ROOM );
	obj_from_char( obj );
	if (ch->in_room)
	  obj_to_room( obj, ch->in_room );
	return;
      }
    //OBJTOCHAR_CHECK
    if (OBJTOCHAR_CHECK(ch, obj) == -1)
      bug("OBJTOCHAR_CHECK: returned error.", 0);
}

void obj_from_char( OBJ_DATA *obj )
{
    CHAR_DATA *ch;
    if ( ( ch = obj->carried_by ) == NULL )
    {
	bugf( "Obj_from_char: null ch. - %s", obj->name );
	return;
    }
    if ( obj->wear_loc != WEAR_NONE )
	unequip_char( ch, obj );
    if (obj->bbid)
      obj_from_bbid( obj );
    if ( ch->carrying == obj )
	ch->carrying = obj->next_content;
    else
    {
	OBJ_DATA *prev;
	for ( prev = ch->carrying; prev != NULL; prev = prev->next_content )
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	if ( prev == NULL )
	    bug( "Obj_from_char: obj not in list.", 0 );
    }
    obj->carried_by	 = NULL;
    obj->next_content	 = NULL;

    if (obj->vnum != OBJ_VNUM_LOCKER){
      ch->carry_number	-= get_obj_number( obj );
      ch->carry_weight	-= get_obj_weight_char( ch, obj );
    }
    /* Cheat check: Viri */
    if (IS_SET(obj->wear_flags, ITEM_HAS_OWNER))
      REMOVE_BIT(obj->extra_flags, ITEM_STAIN);
    if ( (IS_IMMORTAL(ch) || (IS_NPC(ch) && ch->desc
			      && !is_affected(ch, gsn_linked_mind)))
	 && IS_LIMITED(obj)
	 && !IS_SET(obj->extra_flags, ITEM_STAIN)){
      char buf[MIL];
      SET_BIT(obj->extra_flags, ITEM_STAIN);
      sprintf(buf, "%s stained by %s.", obj->short_descr, PERS2(ch));
      send_to_char(buf, ch);
      log_string(buf);
    }
    else if (IS_SET(obj->extra_flags, ITEM_STAIN)
	     && !IS_IMMORTAL(ch)
	     && !IS_SET(ch->game, GAME_STAIN)){
      char buf[MIL];
      SET_BIT(ch->game, GAME_STAIN);
      sprintf(buf, "%s stained by %s.", PERS2(ch), obj->short_descr);
      send_to_char(buf, ch);
      log_string(buf);
    }
}
int apply_ac( OBJ_DATA *obj, int iWear, int type, int mod )
{
  int ac = 0;
  if ( obj->item_type != ITEM_ARMOR )
    return 0;

  switch ( iWear )
    {
    case WEAR_BODY:	ac =     (mod + 4) * obj->value[type];break;
    case WEAR_HEAD:	ac =     (mod + 2) * obj->value[type];break;
    case WEAR_FACE:     ac =     (mod + 1) * obj->value[type];break;
    case WEAR_LEGS:	ac =     (mod + 3) * obj->value[type];break;
    case WEAR_FEET:	ac =     (mod + 1) * obj->value[type];break;
    case WEAR_HANDS:	ac =     (mod + 1) * obj->value[type];break;
    case WEAR_ARMS:	ac =     (mod + 3) * obj->value[type];break;
    case WEAR_SHIELD:	ac =     (mod + 3) * obj->value[type];break;
    case WEAR_NECK_1:	ac =     (mod + 1) * obj->value[type];break;
    case WEAR_NECK_2:	ac =     (mod + 1) * obj->value[type];break;
    case WEAR_ABOUT:	ac =     (mod + 2) * obj->value[type];break;
    case WEAR_WAIST:	ac =     (mod + 1) * obj->value[type];break;
    case WEAR_WRIST_L:	ac =     (mod + 1) * obj->value[type];break;
    case WEAR_WRIST_R:	ac =     (mod + 1) * obj->value[type];break;
    case WEAR_HOLD:	ac =     (mod + 1) * obj->value[type];break;
    case WEAR_TATTOO:	ac =     (mod + 1) * obj->value[type];break;
    case WEAR_SHROUD:	ac =     (mod + 1) * obj->value[type];break;
    case WEAR_EARRING:  ac =     (mod + 1) * obj->value[type];break;
    default:		ac =     (mod + 1) * obj->value[type];break;
    }
/* apply conditon */
  ac = obj->condition * ac / 100;
  return ac;
}

OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
    OBJ_DATA *obj;
    if (ch == NULL)
      return NULL;

    if (ch->race == grn_vampire && iWear != WEAR_TATTOO){
      if (is_affected(ch,gsn_bat_form)
	  || is_affected(ch,gsn_wolf_form)
	  || is_affected(ch,gsn_mist_form)){
	return NULL;
      }
    }
    else if (iWear != WEAR_TATTOO && iWear != WEAR_LIGHT && ch->race == grn_werebeast){
      if (is_affected(ch, gsn_weretiger)
	  || is_affected(ch, gsn_werebear)
	  || is_affected(ch, gsn_werewolf)
	  || is_affected(ch, gsn_werefalcon)){
	return NULL;
      }
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	if ( obj->wear_loc == iWear )
	    return obj;
    return NULL;
}

inline bool can_take( CHAR_DATA *ch, OBJ_DATA *obj)
{
  /* small check to make sure items cannot be picked up by those that cant. */
  if (IS_MASTER(ch) && IS_LIMITED(obj))
    return FALSE;
  else if (IS_IMMORTAL(ch)){
    return TRUE;
  }
  else if (ch->class == gcn_adventurer && IS_LIMITED(obj)){
    return FALSE;
  }
  else if (obj->item_type == ITEM_KEY)
    return TRUE;
  else if (IS_OBJ_STAT(obj, ITEM_STAIN) && mud_data.mudport != TEST_PORT)
    return FALSE;
  /* cabal check */
  else if (obj->item_type == ITEM_CABAL
	   && obj->pCabal
	   && (ch->pCabal == NULL || is_friendly(obj->pCabal, ch->pCabal) == CABAL_NEUTRAL))
    return FALSE;
  else if (obj->pCabal && !_is_cabal(ch->pCabal, obj->pCabal)){
    return FALSE;
  }
  else if (IS_NPC( ch ) && IS_SET(ch->act, ACT_NOALIGN))
    return TRUE;
  else if ((IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)   )
	   || ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)   )
	   || ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch))
	   || ( ch->class == class_lookup("necromancer") && !str_cmp(obj->material,"life"))
	   || ( ch->race == race_lookup("elf") && !str_cmp(obj->material,"iron"))
	   || ( ch->race == race_lookup("drow") && !str_cmp(obj->material,"silver"))
	   || ( ch->race == race_lookup("drow") && !str_cmp(obj->material,"mithril"))
	   || ( ch->race == race_lookup("dwarf") && !str_cmp(obj->material,"water"))
	   || ( ch->race == race_lookup("duergar") && !str_cmp(obj->material,"water"))
	   || ( ch->race == race_lookup("stone") && !str_cmp(obj->material,"air"))
	   || ( ch->race == race_lookup("fire") && !str_cmp(obj->material,"ice"))
	   || ( ch->race == race_lookup("fire") && !str_cmp(obj->material,"cold"))
	   || ( ch->race == race_lookup("storm") && !str_cmp(obj->material,"wood"))
	   || ( ch->race == race_lookup("storm") && !str_cmp(obj->material,"earth"))
	   || (ch->race == grn_feral && !str_cmp(obj->material,"fire"))
	   || (ch->race == grn_feral && !str_cmp(obj->material,"flames"))
	   || (ch->race == grn_feral && !str_cmp(obj->material,"flame"))
	   || (ch->race == grn_werebeast && !str_cmp(obj->material,"silver"))
	   || ( ch->level < 15 && IS_LIMITED(obj) )
	   )
    return FALSE;
  return TRUE;
}


void equip_char(CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    char buf[MSL];
    AFFECT_DATA *paf;
    int i, mod = 0;
    if ( get_eq_char( ch, iWear ) != NULL )
    {
	bugf( "Equip_char: %s already equipped (%d).", ch->name, iWear );
	return;
    }
    if (obj->bbid)
      obj_from_bbid( obj );
    if (!check_sheath(ch, iWear))
      return;
/* Viri: Get rid of this for now, this check should be done before here
    if ( !can_take ( ch, obj))
    {
	act( "You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR );
	act( "$n is zapped by $p and drops it.",  ch, obj, NULL, TO_ROOM );
	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
    }
*/
    obj->wear_loc	 = iWear;
/* sheaths do not affect anything */
    if (obj->wear_loc == WEAR_SHEATH_L || obj->wear_loc == WEAR_SHEATH_R)
      return;
    if ( (ch->class == gcn_monk || ch->class == gcn_blademaster) && get_obj_weight(obj)  > get_monk(iWear))
      act("You feel encumbered.\n\r", ch, NULL, NULL, TO_CHAR);

/* bonus to armor multiplier */
    if ((iWear == WEAR_BODY || iWear == WEAR_HEAD || iWear == WEAR_FACE
	 || iWear == WEAR_ARMS || iWear == WEAR_LEGS)
	&& get_skill(ch, gsn_adv_armor) > 1 )
      mod = 1;
    else
      mod = 0;
    for (i = 0; i < 4; i++)
      ch->armor[i] -= apply_ac( obj, iWear,i, mod);
    if (!obj->enchanted)
	for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
	{
	  /* ranged weapons do not modify hit/dam on  char */
	  if (obj->item_type != ITEM_PROJECTILE){
	    if ( paf->location != APPLY_SPELL_AFFECT )
	      affect_modify( ch, paf, TRUE );
	  }
	  if (paf->bitvector)
            {
	      switch(paf->where)
                {
                case TO_AFFECTS: sprintf(buf,"You are now affected by %s.\n\r", affect_bit_name(paf->bitvector)); break;
                case TO_IMMUNE:  sprintf(buf,"You are now immune to %s.\n\r", imm_bit_name(paf->bitvector)); break;
                case TO_RESIST:  sprintf(buf,"You are now resistent to %s.\n\r", imm_bit_name(paf->bitvector)); break;
                case TO_VULN:    sprintf(buf,"You are now vulnerable to %s.\n\r", imm_bit_name(paf->bitvector)); break;
		default:
		  buf[0] = '\0';
                }
	      act(buf, ch, NULL, NULL, TO_CHAR);
            }
	}
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
      /* ranged weapons do not modify hit/dam on  char */
      if (obj->item_type != ITEM_PROJECTILE){
	if ( paf->location == APPLY_SPELL_AFFECT )
	  affect_to_char ( ch, paf );
	else
	  affect_modify( ch, paf, TRUE );
      }
      if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	  if (paf->bitvector)
            {
                switch(paf->where)
                {
                case TO_AFFECTS: sprintf(buf,"You are now affected by %s.\n\r", affect_bit_name(paf->bitvector)); break;
                case TO_IMMUNE:  sprintf(buf,"You are now immune to %s.\n\r", imm_bit_name(paf->bitvector)); break;
                case TO_RESIST:  sprintf(buf,"You are now resistent to %s.\n\r", imm_bit_name(paf->bitvector)); break;
                case TO_VULN:    sprintf(buf,"You are now vulnerable to %s.\n\r", imm_bit_name(paf->bitvector)); break;
		default:
		  buf[0] = '\0';
                }
		act(buf, ch, NULL, NULL, TO_CHAR);
            }
	}
    }
    if ( obj->item_type == ITEM_LIGHT
	 && obj->value[2] != 0
	 && ch->in_room != NULL)
      if (++ch->in_room->light > 20)ch->in_room->light = 20;

    if ( obj->pIndexData->message )
    {
	act( obj->pIndexData->message->onself, ch, obj, NULL, TO_CHAR );
	act( obj->pIndexData->message->onother,  ch, obj, NULL, TO_ROOM );
    }
}

void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
    char buf[MSL];
    AFFECT_DATA *paf = NULL, *lpaf = NULL, *lpaf_next = NULL;
    int i, mod = 0;
    int iWear = obj->wear_loc;

    if ( iWear == WEAR_NONE )
    {
	bug( "Unequip_char: already unequipped.", 0 );
	return;
    }
    obj->wear_loc	 = -1;
/* sheaths do not affect anything */
    if (iWear == WEAR_SHEATH_L || iWear == WEAR_SHEATH_R)
      return;
    if (is_affected(ch,gsn_double_grip) && iWear == WEAR_WIELD)
	affect_strip(ch,gsn_double_grip);
/* bonus to armor multiplier */
    if (get_skill(ch, gsn_adv_armor) > 1
	&& (iWear == WEAR_BODY || iWear == WEAR_HEAD
	    || iWear == WEAR_FACE
	    || iWear == WEAR_ARMS || iWear == WEAR_LEGS) )
      mod = 1;
    else
      mod = 0;
    for (i = 0; i < 4; i++)
      ch->armor[i]	+= apply_ac( obj, iWear,i, mod );

    if (!obj->enchanted)
	for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
	{
	    if ( paf->location == APPLY_SPELL_AFFECT )
	        for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
	        {
		    lpaf_next = lpaf->next;
                    if ((lpaf->type == paf->type) && (lpaf->level == paf->level) && (lpaf->location == APPLY_SPELL_AFFECT))
		    {
		        affect_remove( ch, lpaf);
			lpaf_next = NULL;
		    }
	        }
	    else
	    {
	      /* ranged weapons do not modify hit/dam on  char */
	      if (obj->item_type != ITEM_PROJECTILE){
		affect_modify( ch, paf, FALSE );
		if (paf->location == APPLY_SEX)
		  fixed_sex(ch);
		affect_check(ch,paf->where,paf->bitvector);
	      }
	    }
            if (paf->bitvector)
            {
                switch(paf->where)
                {
                case TO_AFFECTS: sprintf(buf,"You are no longer affected by %s.\n\r", affect_bit_name(paf->bitvector)); break;
                case TO_IMMUNE:  sprintf(buf,"You are no longer immune to %s.\n\r", imm_bit_name(paf->bitvector)); break;
                case TO_RESIST:  sprintf(buf,"You are no longer resistent to %s.\n\r", imm_bit_name(paf->bitvector)); break;
                case TO_VULN:    sprintf(buf,"You are no longer vulnerable to %s.\n\r", imm_bit_name(paf->bitvector)); break;
                }
		act(buf, ch, NULL, NULL, TO_CHAR);
            }
	}
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
	if ( paf->location == APPLY_SPELL_AFFECT )
	{
	    bug ( "Norm-Apply: %d", 0 );
	    for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
	    {
		lpaf_next = lpaf->next;
                if ((lpaf->type == paf->type) && (lpaf->level == paf->level) && (lpaf->location == APPLY_SPELL_AFFECT))
		{
		    bug ( "location = %d", lpaf->location );
		    bug ( "type = %d", lpaf->type );
		    affect_remove( ch, lpaf);
		    lpaf_next = NULL;
		}
	        if ( paf->location != APPLY_NONE && paf->modifier != 0 )
		{
	            if (paf->bitvector)
	            {
	                switch(paf->where)
	                {
	                case TO_AFFECTS: sprintf(buf,"You are no longer affected by %s.\n\r", affect_bit_name(paf->bitvector)); break;
        	        case TO_IMMUNE:  sprintf(buf,"You are no longer immune to %s.\n\r", imm_bit_name(paf->bitvector)); break;
                	case TO_RESIST:  sprintf(buf,"You are no longer resistent to %s.\n\r", imm_bit_name(paf->bitvector)); break;
	                case TO_VULN:    sprintf(buf,"You are no longer vulnerable to %s.\n\r", imm_bit_name(paf->bitvector)); break;
        	        }
			act(buf, ch, NULL, NULL, TO_CHAR);
	            }

		}
	    }
	}
	else
	{
	  /* ranged weapons do not modify hit/dam on  char */
	  if (obj->item_type != ITEM_PROJECTILE){
	    affect_modify( ch, paf, FALSE );
	    if (paf->location == APPLY_SEX)
	      fixed_sex(ch);
	    affect_check(ch,paf->where,paf->bitvector);
	  }
	}
    if ( obj->item_type == ITEM_LIGHT
	 && obj->value[2] != 0
	 && ch->in_room != NULL)
      if (--ch->in_room->light < -20)ch->in_room->light = -20;
}

int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
    OBJ_DATA *obj;
    int nMatch = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
	if ( obj->pIndexData == pObjIndex )
	    nMatch++;
    return nMatch;
}

void obj_from_room( OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *in_room;
    if ( ( in_room = obj->in_room ) == NULL )
    {
	bugf( "Obj_from_room: null room. - %s", obj->name );
	return;
    }
    if ( obj == in_room->contents )
	in_room->contents = obj->next_content;
    else
    {
	OBJ_DATA *prev;
	for ( prev = in_room->contents; prev; prev = prev->next_content )
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	if ( prev == NULL )
	{
	    bug( "Obj_from_room: obj not found.", 0 );
	    return;
	}
    }
    obj->in_room      = NULL;
    obj->next_content = NULL;
}

void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
/* safety */
  if (obj->in_room){
    bug("obj_to_room: obj %d still in room.", obj->pIndexData->vnum);
    obj_from_room( obj );
  }
  if (obj->in_obj){
    bug("obj_to_room: obj %d still in obj.", obj->pIndexData->vnum);
    obj_from_obj( obj );
  }
  if (obj->carried_by){
    bug("obj_to_room: obj %d still on char.", obj->pIndexData->vnum);
    obj_from_char( obj );
  }

  /* CABAL ITEM pointer setting */
  if (obj->item_type == ITEM_CABAL && obj->pCabal){
    if (obj->pCabal->citem != NULL && obj->pCabal->citem != obj)
      bug("obj_to_room: multiple cabal item for cabal %d!", obj->pCabal->pIndexData->vnum);
    else
      obj->pCabal->citem = obj;
  }

  obj->next_content		= pRoomIndex->contents;
  pRoomIndex->contents	= obj;
  obj->in_room		= pRoomIndex;
  obj->carried_by		= NULL;
  obj->in_obj			= NULL;
  obj->idle			= 0;
}

void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
/* safety */
  if (obj->in_room){
    bug("obj_to_obj: obj %d still in room.", obj->pIndexData->vnum);
    obj_from_room( obj );
  }
  if (obj->in_obj){
    bug("obj_to_obj: obj %d still in obj.", obj->pIndexData->vnum);
    obj_from_obj( obj );
  }
  if (obj->carried_by){
    bug("obj_to_obj: obj %d still on char.", obj->pIndexData->vnum);
    obj_from_char( obj );
  }

  obj->next_content		= obj_to->contains;
  obj_to->contains		= obj;
  obj->in_obj			= obj_to;
  obj->in_room		= NULL;
  obj->carried_by		= NULL;
  for ( ; obj_to != NULL; obj_to = obj_to->in_obj )
    if ( obj_to->carried_by != NULL )
      obj_to->carried_by->carry_weight += get_obj_weight( obj ) * WEIGHT_MULT(obj_to) / 100;
}

void obj_from_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_from;
    if ( ( obj_from = obj->in_obj ) == NULL )
    {
	bug( "Obj_from_obj: null obj_from.", 0 );
	return;
    }
    if ( obj == obj_from->contains )
	obj_from->contains = obj->next_content;
    else
    {
	OBJ_DATA *prev;
	for ( prev = obj_from->contains; prev; prev = prev->next_content )
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	if ( prev == NULL )
	{
	    bug( "Obj_from_obj: obj not found.", 0 );
	    return;
	}
    }
    obj->next_content = NULL;
    obj->in_obj       = NULL;
    for ( ; obj_from != NULL; obj_from = obj_from->in_obj )
	if ( obj_from->carried_by != NULL )
            obj_from->carried_by->carry_weight -= get_obj_weight( obj ) * WEIGHT_MULT(obj_from) / 100;
}

void extract_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_content, *obj_next;
    if ( obj == NULL )
	return;
    if ( obj->extracted )
    {
        bugf("Warning! Extraction of %s, vnum %d.", obj->name, obj->pIndexData->vnum);
	return;
    }
    else
	obj->extracted = TRUE;

    //CABAL ITEM pointer
    if (obj->item_type == ITEM_CABAL && obj->pCabal && obj->pCabal->citem == obj)
      obj->pCabal->citem = NULL;

//EFFECT KILL CHECK
    if ( OBJKILL_CHECK(obj) == -1){
      bug("Error in OBJKILL_CHECK in handler.c\n\r", 0);
    }

    if ( obj->in_room != NULL )
	obj_from_room( obj );
    else if ( obj->carried_by != NULL )
	obj_from_char( obj );
    else if ( obj->in_obj != NULL )
	obj_from_obj( obj );

    while (obj->traps){
      extract_trap( obj->traps );
    }

    for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
    {
	obj_next = obj_content->next_content;
	extract_obj( obj_content );
    }
    if ( object_list == obj )
	object_list = obj->next;
    else
    {
	OBJ_DATA *prev;
	for ( prev = object_list; prev != NULL; prev = prev->next )
	    if ( prev->next == obj )
	    {
		prev->next = obj->next;
		break;
	    }
	if ( prev == NULL )
	{
	    bug( "Extract_obj: obj %d not found.", obj->pIndexData->vnum );
	    return;
	}
    }
    --obj->pIndexData->count;
    free_obj(obj);
}

void extract_char( CHAR_DATA *ch, bool fPull )
{
    CHAR_DATA *wch, *wch_next;
    OBJ_DATA *obj, *obj_next;
    int hold_limit = 1;
    int i = 0;

    if (fPull)
    {
	if (ch->extracted)
	    return;
	else
	    ch->extracted = TRUE;
    }
    //log_string("Removing Challenge");
    affect_strip(ch, gen_challenge);
    //log_string("Removing Eavesdropping");
    if (!IS_NPC(ch) && ch->pcdata->eavesdropping != NULL){
      act("You stop eavesdropping on $N.",ch,NULL,ch->pcdata->eavesdropping,TO_CHAR);
      affect_strip(ch,gsn_eavesdrop);
      ch->pcdata->eavesdropping->pcdata->eavesdropped = NULL;
      ch->pcdata->eavesdropping = NULL;
    }
    if (!IS_NPC(ch) && ch->pcdata->eavesdropped != NULL){
      act("You stop eavesdropping on $N.",ch->pcdata->eavesdropped,NULL,ch,TO_CHAR);
      affect_strip(ch->pcdata->eavesdropped,gsn_eavesdrop);
      ch->pcdata->eavesdropped->pcdata->eavesdropping = NULL;
      ch->pcdata->eavesdropped = NULL;
    }

    /* remove traps */
    for (i = 0; i < MAX_TRAPS; i++){
      if (ch->traps[i] != NULL)
	extract_trap( ch->traps[i] );
    }
    //log_string("Nuking Pets");
    nuke_pets(ch);
    ch->pet = NULL;

    /* remove stallion */
    if (!IS_NPC(ch))
      ch->pcdata->pStallion = NULL;

    /* clear familiar/summoner relationship */
    if (ch->summoner && !IS_NPC(ch->summoner) && ch == ch->summoner->pcdata->familiar){
      sendf(ch->summoner, "Your link with %s has been broken.\n\r", PERS2( ch ));
      ch->summoner->pcdata->familiar = NULL;
    }

    //log_string("End Fight");
    stop_fighting( ch, TRUE );
    if (fPull)
      die_follower( ch, TRUE );
    else
      die_follower( ch, FALSE );

    if ((!IS_IMMORTAL(ch) && !is_affected(ch, gsn_life_insurance)) || fPull){
      for ( obj = ch->carrying; obj != NULL; obj = obj_next ){
	obj_next = obj->next_content;
	if (IS_LIMITED(obj) && !IS_NPC(ch) && ch->level < 15)
	  extract_obj( obj );
	else if (!IS_IMMORTAL(ch) && IS_LIMITED(obj) && !IS_NPC(ch) &&
		 obj->wear_loc == WEAR_NONE && hold_limit > (ch->level/10)-1)
	  extract_obj( obj );
	else if (IS_LIMITED(obj) && !IS_NPC(ch)){
	  OBJ_DATA *in_obj, *in_obj_next;
	  if (obj->wear_loc == WEAR_NONE)
	    hold_limit++;
	  obj_from_char(obj);
	  for (in_obj = obj->contains;in_obj != NULL;in_obj = in_obj_next){
	    in_obj_next = in_obj->next_content;
	    if (!IS_LIMITED(in_obj))
	      extract_obj(in_obj);
	    else{
	      obj_from_obj(in_obj);
	      obj_to_room(in_obj, get_room_index(ROOM_VNUM_LIMIT));
	    }
	  }
	  obj_to_room(obj, get_room_index(ROOM_VNUM_LIMIT));
	}
	else
	  extract_obj( obj );
      }
    }
    if ( ch->desc != NULL && ch->desc->original != NULL ){
      if (is_affected(ch->desc->original,gsn_linked_mind))
	{
	  affect_strip(ch->desc->original,gsn_linked_mind);
	  WAIT_STATE(ch,96);
	}
      do_break( ch, "" );
      ch->desc = NULL;
    }
    //log_string("Char from room");
    if (ch->in_room != NULL)
      char_from_room( ch );

    //Clear altar pointer for a cabal
    if (ch->pCabal && ch->pCabal->altar == ch)
      ch->pCabal->altar = NULL;
    //Clear guard pointer for a cabal
    if (ch->pCabal && ch->pCabal->cguard == ch)
      ch->pCabal->cguard = NULL;

    if ( !fPull ){
      ROOM_INDEX_DATA* room;

        if (ch->alignment > 0)
	  room= get_room_index( hometown_table[ch->hometown].g_vnum);
        else if (ch->alignment < 0)
	  room = get_room_index( hometown_table[ch->hometown].e_vnum);
        else
	  room = get_room_index( hometown_table[ch->hometown].n_vnum);

	if (room == NULL)
	  room = get_room_index( ROOM_VNUM_TEMPLE );

	char_to_room( ch, room );
	return;
    }
    if ( IS_NPC(ch) )
      {
	--ch->pIndexData->count;
	if (ch->pIndexData->vnum >= MOB_VNUM_SERVANT_FIRE
	    && ch->pIndexData->vnum <= MOB_VNUM_SERVANT_EARTH
	    && ch->summoner != NULL && !IS_NPC(ch->summoner))
	  {
            if (is_affected(ch->summoner,gsn_familiar_link))
	      {
                send_to_char("Your energy drains out as you lose link with your familiar.\n\r",ch->summoner);
                affect_strip(ch->summoner,gsn_familiar_link);
	      }
	    ch->summoner->pcdata->familiar = NULL;
	  }
      }
    //log_string("Canceling replyies and huntings");
    for ( wch = char_list; wch != NULL; wch = wch_next )
      {
	wch_next = wch->next;
	if ( wch->reply == ch )
	  wch->reply = NULL;
        if (IS_NPC(wch) && wch->hunting != NULL && wch->hunting == ch)
	  {
            wch->hunting = NULL;
	    if (IS_SET(wch->act,ACT_AGGRESSIVE) && wch->in_room != NULL)
	      {
                act( "$n slowly disappears.",wch,NULL,NULL,TO_ROOM );
           	extract_char( wch, TRUE );
	      }
	  }
      }
    //log_string("Removing from list");
    if ( ch == char_list )
      char_list = ch->next;
    else
      {
	CHAR_DATA *prev;
	for ( prev = char_list; prev != NULL; prev = prev->next )
	  if ( prev->next == ch )
	    {
	      prev->next = ch->next;
	      break;
	    }
	if ( prev  == NULL )
	  {
	    bug( "Extract_char: char not found.", 0 );
	    return;
	  }
      }
    if (!IS_NPC(ch))
      {
	if ( ch == player_list )
	  player_list = ch->next_player;
	else
	{
	  CHAR_DATA *prev;
	  for (prev = player_list; prev != NULL; prev = prev->next_player )
	    if (prev->next_player == ch)
	      {
		prev->next_player = ch->next_player;
		break;
	      }
	}
      }
    if ( ch->desc != NULL )
      ch->desc->character = NULL;
    //log_string("Freeing from list");
    free_char( ch );
  //log_string("Done extracting");
}

CHAR_DATA *get_char_room( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument )
{
    char arg[MIL];
    CHAR_DATA *rch;
    CHAR_DATA* vch;

    int count = 0, number = number_argument( argument, arg );

    if ( !str_cmp( arg, "self" ) || !str_cmp( arg, "me" ))
	return ch;

    if ( ch && room )
    {
	bug( "get_char_room received multiple types (ch/room)", 0 );
	return NULL;
    }
    if (!ch && !room)
    {
	bug( "get_char_room received NULL types (ch and room)", 0 );
	return NULL;
    }
    if ( ch )
	rch = ch->in_room->people;
    else
	rch = room->people;

    if (ch){
      if (is_song_affected(ch,gsn_disorient) && !IS_IMMORTAL(ch))
	{
	  CHAR_DATA *to_return = NULL;
	  count = 1;
	  for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	    {
	      if ( !can_see( ch, rch ))
	    	continue;
	      if (number_range(0,count) == 0)
		to_return = rch;
	      count++;
	    }
	  return to_return;
	}
    }

    /* regular match */
    for ( ; rch != NULL; rch = rch->next_in_room )
    {
      if (ch && !can_see( ch, rch ))
	continue;
      if (IS_NPC(rch) && (!ch || IS_IMMORTAL(ch) || IS_NPC(ch)) ){
	if (rch->pIndexData->vnum == MOB_VNUM_DECOY || rch->pIndexData->vnum == MOB_VNUM_DUMMY)
	  continue;
      }
      if (ch){
	if ((is_affected(rch,gsn_doppelganger) && !IS_IMMORTAL(ch) && !IS_NPC(ch)) ? !(rch->doppel && is_name( arg, rch->doppel->name)) : !is_name( arg, rch->name ) )
	  continue;
      }
      else if (!is_name(arg, rch->name))
	continue;
      if ( ++count == number )
	return rch;
    }
    if ( ch )
	rch = ch->in_room->people;
    else
      return NULL;
    /* make things easier later on here */
    if (IS_AFFECTED(ch, AFF_CHARM) && ch->leader)
      vch = ch->leader;
    else
      vch = ch;
    /* Auto (if any) match */
    if (!IS_GAME(vch, GAME_AMOB) && !IS_GAME(vch, GAME_APC))
      return NULL;
    for ( ; rch != NULL; rch = rch->next_in_room )
    {
      bool fAuto = fAuto = (IS_NPC(rch) ? IS_GAME(vch, GAME_AMOB)
			    : IS_GAME(vch, GAME_APC) );

      /* if not autoed we skip */
      if (!fAuto)
	continue;

      if ( !can_see( ch, rch ))
	continue;
      if (IS_NPC(rch) && (IS_IMMORTAL(ch) || IS_NPC(ch)) )
	if (rch->pIndexData->vnum == MOB_VNUM_DECOY || rch->pIndexData->vnum == MOB_VNUM_DUMMY)
	  continue;
      if ( (is_affected(rch,gsn_doppelganger) && !IS_IMMORTAL(ch) && !IS_NPC(ch)) ? !(rch->doppel && is_auto_name( arg, rch->doppel->name)) : !is_auto_name( arg, rch->name ) )
	continue;
      if ( ++count == number )
	return rch;
    }

    return NULL;
}

CHAR_DATA *get_char_world( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *wch;
    int count = 0, number = number_argument( argument, arg );
    int vnum = atoi(arg);

    if ( ch && ( wch = get_char_room( ch, NULL, argument ) ) != NULL )
	return wch;

    /* normal run */
    for ( wch = char_list; wch != NULL ; wch = wch->next )
    {
	if ( wch->in_room == NULL )
	    continue;
	if ( ch && !can_see( ch, wch ) )
	    continue;
	if (ch && IS_NPC(wch)){
	  if (wch->pIndexData->vnum == MOB_VNUM_DECOY || wch->pIndexData->vnum == MOB_VNUM_DUMMY)
	    continue;
	}
	if (ch){
	  if ( (is_affected(wch,gsn_doppelganger)
		&& !IS_IMMORTAL(ch)) ? !(wch->doppel && is_name( arg, wch->doppel->name)) : !is_name( arg, wch->name )
	       && !(ch && IS_IMMORTAL(ch) && IS_NPC(wch) && is_number(arg) && atoi(arg) == wch->pIndexData->vnum) ){
	    continue;
	  }
	}
	else{
	  if (vnum < 1 && !is_name(arg, wch->name) )
	    continue;
	  else if (vnum > 0 && IS_NPC(wch) && wch->pIndexData->vnum != vnum)
	    continue;
	}
	if ( ++count == number )
	  return wch;
    }

    if (!ch)
      return NULL;
    /* Auto (if any) match */
    if (!IS_GAME(ch, GAME_AMOB) && !IS_GAME(ch, GAME_APC))
      return NULL;
    for ( wch = char_list; wch != NULL ; wch = wch->next )
    {
      bool fAuto = (IS_NPC(wch) ? IS_GAME(ch, GAME_AMOB) : IS_GAME(ch, GAME_APC) );

      /* if not autoed we skip */
      if (!fAuto)
	continue;
      if ( wch->in_room == NULL )
	continue;
      if ( ch && !can_see( ch, wch ) )
	continue;
      if (IS_NPC(wch) && IS_IMMORTAL(ch))
	if (wch->pIndexData->vnum == MOB_VNUM_DECOY || wch->pIndexData->vnum == MOB_VNUM_DUMMY)
	  continue;
      if ( (is_affected(wch,gsn_doppelganger) && !IS_IMMORTAL(ch)) ? !(wch->doppel && is_auto_name( arg, wch->doppel->name)) : !is_auto_name( arg, wch->name )
	   && !(IS_IMMORTAL(ch) && IS_NPC(wch) && is_number(arg) && atoi(arg) == wch->pIndexData->vnum) )
	continue;
      if ( ++count == number )
	return wch;
    }
    return NULL;
}

CHAR_DATA *get_char( char *argument )
{
    char arg[MIL];
    CHAR_DATA *wch;
    int count = 0, number = number_argument( argument, arg );
    for ( wch = char_list; wch != NULL ; wch = wch->next )
    {
	if ( wch->in_room == NULL )
	    continue;
	if (IS_NPC(wch))
	  if (wch->pIndexData->vnum == MOB_VNUM_DECOY || wch->pIndexData->vnum == MOB_VNUM_DUMMY)
	    continue;
	if ( !is_name( arg, wch->name )
	     || (wch->invis_level || wch->incog_level))
	    continue;
	if ( ++count == number )
	    return wch;
    }
    return NULL;
}

OBJ_DATA *get_obj_type( OBJ_INDEX_DATA *pObjIndex, ROOM_INDEX_DATA* pRoom )
{
  ROOM_INDEX_DATA* in_room;
  OBJ_DATA *obj;
  for ( obj = object_list; obj != NULL; obj = obj->next ){
    if ( obj->pIndexData != pObjIndex)
      continue;
    else if ( (in_room = in_obj(obj)->in_room) == NULL
	      && (in_room = in_char( obj )->in_room) == NULL)
      continue;
    else if (in_room != pRoom)
      continue;
    else
      return obj;
    }
    return NULL;
}

OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
  CHAR_DATA* vch;
  char arg[MIL];
  OBJ_DATA *obj;
  int count = 0, number = number_argument( argument, arg );

    /* regular */
    for ( obj = list; obj != NULL; obj = obj->next_content ){
      if ( ch && !can_see_obj( ch, obj ))
	continue;
      if (is_name( arg, obj->name ) ){
	if ( ++count == number )
	  return obj;
      }
    }
    if (!ch)
      return NULL;
    if (IS_AFFECTED(ch, AFF_CHARM) && ch->leader)
      vch = ch->leader;
    else
      vch = ch;

    if (!IS_GAME(vch, GAME_AOBJ))
      return NULL;

    /* autoed */
    for ( obj = list; obj != NULL; obj = obj->next_content ){
      if ( can_see_obj( ch, obj ) && is_auto_name( arg, obj->name ) )
	if ( ++count == number )
		return obj;
    }
    return NULL;

}

//returns absolute number of items in items in the list
int count_obj_inlist( OBJ_DATA* list, int count ){

  if (list == NULL)
    return count;

  for (;list;list = list->next_content){
    if (list->contains)
      count += count_obj_inlist( list->contains, count);
    count++;
  }
  return count;
}


OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument, CHAR_DATA *viewer )
{
  CHAR_DATA* vch;
  char arg[MIL];
  OBJ_DATA *obj;
  int count = 0, number = number_argument( argument, arg );

    /* regular */
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ){
      if ( obj->wear_loc == WEAR_NONE && (viewer ? can_see_obj( viewer, obj ) : TRUE ) && is_name( arg, obj->name ) )
	if ( ++count == number )
	  return obj;
    }
    //rest requires a viewer.
    if (viewer == NULL)
      return NULL;

    if (IS_AFFECTED(viewer, AFF_CHARM) && viewer->leader)
      vch = viewer->leader;
    else
      vch = viewer;

    if (!IS_GAME(vch, GAME_AOBJ))
      return NULL;
    /* autoed */
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ){
      if ( obj->wear_loc == WEAR_NONE && (viewer ? can_see_obj( viewer, obj ) : TRUE ) && is_auto_name( arg, obj->name ) )
	if ( ++count == number )
	  return obj;
    }
    return NULL;
}

OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument, CHAR_DATA *viewer )
{
  CHAR_DATA* vch;
  char arg[MIL];
  OBJ_DATA *obj;
  int count = 0, number = number_argument( argument, arg );

    /* regular */
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ){
      if ( obj->wear_loc != WEAR_NONE && (viewer ? can_see_obj( viewer, obj ) : TRUE) && is_name( arg, obj->name ) )
	if ( ++count == number )
	  return obj;
    }
    if (viewer == NULL)
      return NULL;
    if (IS_AFFECTED(viewer, AFF_CHARM) && viewer->leader)
      vch = viewer->leader;
    else
      vch = viewer;
    if (!IS_GAME(vch, GAME_AOBJ))
      return NULL;
    /* autoed */
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ){
      if ( obj->wear_loc != WEAR_NONE && (viewer ? can_see_obj( viewer, obj ) : TRUE) && is_auto_name( arg, obj->name ) )
	if ( ++count == number )
	  return obj;
    }
    return NULL;
}

OBJ_DATA *get_obj_here( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument )
{
  OBJ_DATA *obj = NULL;
  int vnum = atoi( argument );

    if ( ch && room )
    {
	bug( "get_obj_here: received a ch and a room",0);
	return NULL;
    }
    if ( !ch && !room )
    {
	bug( "get_obj_here received no ch and room",0);
	return NULL;
    }

    if ( ch )
    {
	obj = get_obj_list( ch, argument, ch->in_room->contents );
	if ( obj != NULL )
	    return obj;
	if ( ( obj = get_obj_carry( ch, argument, ch ) ) != NULL )
	    return obj;
	if ( ( obj = get_obj_wear( ch, argument, ch) ) != NULL )
	    return obj;
    }
    else
    {
      char arg[MIL];
      int count = 0,  number = number_argument( argument, arg );

	for ( obj = room->contents; obj; obj = obj->next_content )
	{
	  if ( !is_name( arg, obj->name ) && (vnum == 0 || obj->pIndexData->vnum != vnum))
	    continue;
	  if ( ++count == number )
	    return obj;
	}
    }
    return NULL;
}

OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj;
    int count = 0, number = number_argument( argument, arg );
    if ( ch && ( obj = get_obj_here( ch, NULL, argument ) ) != NULL )
      return obj;
    for ( obj = object_list; obj != NULL; obj = obj->next ){
      if ( (ch ? can_see_obj( ch, obj )  : TRUE)
	   && (is_name( arg, obj->name )
	       || (is_number(arg) && atoi(arg) == obj->pIndexData->vnum)))
	if ( ++count == number )
	  return obj;
    }
    return NULL;
}

OBJ_DATA *create_money( int amount )
{
    char buf[MSL];
    OBJ_DATA *obj;
    if ( amount <= 0 )
    {
	bug( "Create_money: zero or negative money %d.", amount );
	amount = 1;
    }
    if ( amount == 1 )
        obj = create_object( get_obj_index( OBJ_VNUM_GOLD_ONE ), 0);
    else
    {
        obj = create_object( get_obj_index( OBJ_VNUM_GOLD_SOME ), 0);
	sprintf( buf, obj->short_descr, amount );
	free_string( obj->short_descr );
	obj->short_descr	= str_dup( buf );
	obj->value[0]		= amount;
	obj->cost		= amount;
    }
    return obj;
}

int get_obj_number( OBJ_DATA *obj )
{
    int number;
    if (CAN_WEAR(obj, ITEM_WEAR_TATTOO) || obj->item_type == ITEM_MONEY)
        number = 0;
    else
        number = 1;
    return number;
}

int get_obj_weight( OBJ_DATA *obj )
{
  int weight;
  OBJ_DATA *tobj;

  if (obj == NULL)
    return 0;
  else
    weight = obj->weight;
  for ( tobj = obj->contains; tobj != NULL; tobj = tobj->next_content )
    weight += get_obj_weight( tobj ) * WEIGHT_MULT(obj) / 100;
  return weight;
}


int get_true_weight(OBJ_DATA *obj)
{
    int weight = obj->weight;
    for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
        weight += get_obj_weight( obj );
    return weight;
}

bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
    if ( pRoomIndex == NULL )
        return TRUE;
    if ( pRoomIndex->light > 0 )
	return FALSE;
    if ( IS_SET(pRoomIndex->room_flags2, ROOM_DAY) )
	return FALSE;
    if ( IS_SET(pRoomIndex->room_flags, ROOM_DARK)
	 || IS_SET(pRoomIndex->room_flags2, ROOM_NIGHT)
	 || pRoomIndex->light < 0)
	return TRUE;
    if ( pRoomIndex->sector_type == SECT_INSIDE || pRoomIndex->sector_type == SECT_CITY )
	return FALSE;
    if ( mud_data.weather_info.sunlight == SUN_SET || mud_data.weather_info.sunlight == SUN_DARK )
	return TRUE;
    return FALSE;
}

bool room_has_sun( ROOM_INDEX_DATA *pRoomIndex )
{
    if ( pRoomIndex == NULL )
        return FALSE;
    if ( IS_SET(pRoomIndex->room_flags2, ROOM_DAY) )
	return TRUE;
    if ( IS_SET(pRoomIndex->room_flags, ROOM_DARK)
	 || IS_SET(pRoomIndex->room_flags2, ROOM_NIGHT)
	 || pRoomIndex->light < 0)
	return FALSE;
    if ( pRoomIndex->sector_type == SECT_INSIDE)
	return FALSE;
    if ( mud_data.weather_info.sunlight == SUN_SET || mud_data.weather_info.sunlight == SUN_DARK )
	return FALSE;
    return TRUE;
}

bool have_access(CHAR_DATA *ch, ROOM_INDEX_DATA *room)
{
  if ( IS_SET(room->room_flags, ROOM_IMP_ONLY) && !IS_TRUSTED(ch,IMPLEMENTOR))
    return FALSE;
  if ( IS_SET(room->room_flags, ROOM_GODS_ONLY) && !IS_IMMORTAL(ch))
    return FALSE;
  return TRUE;
}

bool room_is_private( ROOM_INDEX_DATA *pRoomIndex )
{
    CHAR_DATA *rch;
    int count = 0;
    for ( rch = pRoomIndex->people; rch != NULL; rch = rch->next_in_room )
	count++;
    if ( IS_SET(pRoomIndex->room_flags, ROOM_IMP_ONLY) )
	return TRUE;
    return FALSE;
}

/* checks if a character can sense a trap */
TRAP_DATA* see_trap( CHAR_DATA *ch, TRAP_DATA* pt ){
  int skill;
  if (IS_NPC(ch))
    return pt;

  /* rest applies to PC ONLY */
  if (IS_SET(ch->act, PLR_HOLYLIGHT))
    return pt;

  /* disarmed traps do not usualy appear */
  if (!pt->armed || (IS_TRAP(pt, TRAP_DELAY) && IS_TRAP(pt, TRAP_HARD)))
    return NULL;

  if (IS_TRAP(pt, TRAP_INVIS))
    return NULL;

  if (IS_TRAP(pt, TRAP_VIS))
    return pt;

  /* makers see their own traps unless invisible above*/
  if (pt->owner && pt->owner == ch)
    return pt;

  /* pk check */
  if (pt->owner != NULL && is_safe_quiet(pt->owner, ch))
    return NULL;

  /* wisdom check (NON OWNED traps only )*/
  if (!pt->owner && pt->level / 5 < (get_curr_stat(ch, STAT_WIS) - 15))
    return pt;

  /* detect magic check */
  if (IS_AFFECTED(ch, AFF_DETECT_MAGIC) && IS_TRAP(pt, TRAP_SAVES))
    return pt;

  /* skill check THIEVES get bonus */
  if (pt->level * 2 <= (skill = get_skill(ch, gsn_detect_hidden)))
    return pt;
  else if (pt->level * 2 <= (skill + 15) && ch->class == class_lookup("thief"))
    return pt;


 /* detect trap check */
  if (pt->level - 10 < ch->level && is_affected(ch, gsn_detect_traps))
    return pt;
  return NULL;
}

/* checks if one of traps on the list can be seen */
TRAP_DATA* can_see_trap( CHAR_DATA *ch, TRAP_DATA* list ){
  TRAP_DATA* pt, *ret = NULL;

  for (pt = list; pt; pt = pt->next_trap){
    if ( (ret = see_trap(ch, pt)) != NULL)
      return ret;
  }
  return NULL;
}

bool can_see_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    if (IS_SET(pRoomIndex->room_flags, ROOM_NEWBIES_ONLY)
	&&  ((!IS_NPC(ch) && ch->level > LEVEL_NEWBIE
	      && !IS_IMMORTAL(ch))
	     || (IS_NPC(ch)
		 && is_affected(ch,gsn_linked_mind))))
      return FALSE;
    if (IS_NPC(ch))
      return TRUE;
    if (IS_SET(pRoomIndex->room_flags, ROOM_IMP_ONLY) && get_trust(ch) < MAX_LEVEL)
	return FALSE;
    if (IS_SET(pRoomIndex->room_flags2, ROOM_NO_NEWBIES) &&  ch->level < 10 && !IS_IMMORTAL(ch))
	return FALSE;
    if (IS_SET(pRoomIndex->room_flags, ROOM_GODS_ONLY) && !IS_IMMORTAL(ch))
	return FALSE;
    if (IS_SET(pRoomIndex->room_flags, ROOM_IMP_ONLY) && get_trust(ch) < MAX_LEVEL)
	return FALSE;
    if (is_affected(ch,gsn_burrow))
	return FALSE;
    return TRUE;
}

bool can_see( CHAR_DATA *ch, CHAR_DATA *victim )
{
  //    OBJ_DATA *light;
  AFFECT_DATA* paf;
  if (IS_NPC(ch) && ch->trust == 6969 && ch->pIndexData->mprogs)
    return TRUE;
  if (get_trust(ch) < victim->invis_level)
    return FALSE;
  if (get_trust(ch) < victim->incog_level && ch->in_room != victim->in_room)
    return FALSE;
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
      return TRUE;
    if(  (IS_AFFECTED(ch, AFF_BLIND) && !is_affected(ch, gsn_bat_form)) || (IS_AFFECTED(ch, AFF_BLIND) && bat_blind(ch)) || bat_blind(ch) )
      return FALSE;
    if ( ch == victim )
      return TRUE;
    if (is_affected(ch,gsn_burrow))
      return FALSE;
    if ( IS_AFFECTED(victim, AFF_FAERIE_FOG ) || IS_AFFECTED(victim,AFF_FAERIE_FIRE))
      return TRUE;
    if (!IS_NPC(ch)
	&& (paf = affect_find(victim->affected, gsn_mark_prey)) != NULL
	&& paf->has_string && is_name(paf->string, ch->name))
      return TRUE;
    if (check_avenger(ch, victim)
	&& !is_affected(victim, gen_d_shroud))
      return TRUE;
    if (!IS_UNDEAD(ch) && !IS_NPC(ch) && !is_ghost(ch,600)
	&& room_is_dark( ch->in_room )){
      if (!IS_AFFECTED(ch, AFF_INFRARED) || IS_SET(victim->form, FORM_COLD_BLOOD))
	return FALSE;
    }
    if (is_affected(victim,gsn_burrow))
	return FALSE;
    if ( IS_AFFECTED(victim, AFF_HIDE) && !IS_AFFECTED(ch, AFF_DETECT_HIDDEN) )
	return FALSE;
    if ( IS_AFFECTED(victim, AFF_INVISIBLE) && !IS_AFFECTED(ch, AFF_DETECT_INVIS))
      return FALSE;
    if ( IS_AFFECTED2(victim, AFF_CAMOUFLAGE) && !IS_AFFECTED2(ch, AFF_KEEN_SIGHT))
	return FALSE;
    if (IS_NPC(victim))
      if (victim->pIndexData->vnum == MOB_VNUM_SHADOW
	  && victim->hunting != ch
	  && victim->fighting == NULL)
	return FALSE;
    if (is_affected(victim, gen_d_shroud)
	&& !is_same_cabal(ch->pCabal, victim->pCabal)
	&& !is_affected(ch, skill_lookup("bloodlust")))
      return FALSE;
    return TRUE;
}

bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
  //    OBJ_DATA *light;
    AFFECT_DATA* paf;
    if (obj->vnum == OBJ_VNUM_LOCKER && (mud_data.mudport != TEST_PORT || !IS_IMMORTAL(ch)))
      return FALSE;
    if (IS_NPC(ch) && ch->trust == 6969 && ch->pIndexData->mprogs)
      return TRUE;
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) )
      return TRUE;
    if ( IS_SET(obj->extra_flags,ITEM_VIS_DEATH))
      return FALSE;

    if ( (paf = affect_find(obj->affected, gsn_dan_blade)) != NULL)
      if (!paf->bitvector)
	return FALSE;

    if ( IS_AFFECTED( ch, AFF_BLIND ) &&
	!((obj->item_type == ITEM_POTION || obj->item_type == ITEM_PILL || obj->item_type == ITEM_ARTIFACT)
	  && ( obj->value[1] == 22 || obj->value[2] == 22 || obj->value[3] == 22 ) ))
	return FALSE;
    if (is_affected(ch,gsn_burrow))
	return FALSE;
    if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
	return TRUE;
    if ( IS_SET(obj->extra_flags, ITEM_INVIS) && !IS_AFFECTED(ch, AFF_DETECT_INVIS) )
        return FALSE;
    if ( is_affected_obj(obj, gsn_there_not_there) )
    {
	if (IS_NPC(ch))
	  return FALSE;
	if (!is_affected(ch, gsn_detect_aura) && !IS_IMMORTAL(ch))
	  return FALSE;
    }
    if ( IS_OBJ_STAT(obj,ITEM_GLOW))
	return TRUE;
    if ( room_is_dark( ch->in_room ) && !IS_AFFECTED(ch, AFF_INFRARED) && !is_ghost(ch,600))
      /*        if ( ( light = get_eq_char( ch, WEAR_LIGHT ) ) == NULL || light->item_type != ITEM_LIGHT) */
      return FALSE;
    return TRUE;
}

bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
  /* mobs on mob progs can always drop */
  if (IS_NPC(ch) && ch->trust == 6969)
    return TRUE;
  if ( !IS_SET(obj->extra_flags, ITEM_NODROP) )
    return TRUE;
  if ( IS_IMMORTAL(ch) )
    return TRUE;
  return FALSE;
}

char *affect_loc_name( int location )
{
  /* we run through table matching and catenating. */
  static char buf [MSL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; apply_flags[i].bit >= 0; i++){
    if (location == apply_flags[i].bit){
      strcat(buf, " ");
      strcat(buf, apply_flags[i].name);
    }
  }
  if (buf[0] == '\0')
    bug( "Affect_location_name: unknown location %d.", location );
  return ( buf[0] != '\0' ) ? buf+1 : "unknown";
/*
  switch ( location ){
  case APPLY_NONE:		return "none";
  case APPLY_STR:		return "strength";
  case APPLY_DEX:		return "dexterity";
  case APPLY_INT:		return "intelligence";
  case APPLY_WIS:		return "wisdom";
  case APPLY_CON:		return "constitution";
  case APPLY_LUCK:		return "luck";
  case APPLY_SEX:		return "sex";
  case APPLY_CLASS:		return "class";
  case APPLY_LEVEL:		return "level";
  case APPLY_AGE:		return "age";
  case APPLY_SIZE:            return "size";
  case APPLY_WEIGHT:          return "weight";
  case APPLY_MANA:		return "mana";
  case APPLY_HIT:		return "hp";
  case APPLY_MOVE:		return "moves";
  case APPLY_GOLD:		return "gold";
  case APPLY_EXP:		return "experience";
  case APPLY_AC:		return "armor class";
  case APPLY_HITROLL:		return "hit roll";
  case APPLY_DAMROLL:		return "damage roll";
  case APPLY_SAVING_AFFL:	return "save vs afflictive";
  case APPLY_SAVING_MALED:	return "save vs maledictive";
  case APPLY_SAVING_MENTAL:	return "save vs mental";
  case APPLY_SAVING_BREATH:	return "save vs breath";
  case APPLY_SAVING_SPELL:	return "save vs spell";
  case APPLY_SPELL_AFFECT:	return "spell affect";
  case APPLY_SPELL_LEVEL:	return "all spells";
  case APPLY_AFFL_LEVEL:	return "afflictive spells";
  case APPLY_MALED_LEVEL:	return "maledictive spells";
  case APPLY_MENTAL_LEVEL:	return "mental spells";
  case APPLY_SPELL_COST:	return "spell cost (%)";
  case APPLY_WAIT_STATE:	return "casting speed (%)";
  case APPLY_SKILL_LEARN:	return "skill progress (%)";
  case APPLY_SPELL_LEARN:	return "spell progress (%)";
  case APPLY_HIT_GAIN:	return "health regeneration (%)";
  case APPLY_MANA_GAIN:	return "mana regeneration (%)";
  case APPLY_MOVE_GAIN:	return "moves regeneration (%)";
  }
  bug( "Affect_location_name: unknown location %d.", location );
  return "(unknown)";
*/
}

char *affect_bit_name( int flags )
{
  /* we run through table matching and catenating. */
  static char buf [MSL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; affect_flags[i].bit > 0; i++){
    if (IS_SET(flags, affect_flags[i].bit)){
      strcat(buf, " ");
      strcat(buf, affect_flags[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
/*
    static char buf[512];
    buf[0] = '\0';
    if ( vector & AFF_BLIND         ) strcat( buf, " blind"         );
    if ( vector & AFF_INVISIBLE     ) strcat( buf, " invisible"     );
    if ( vector & AFF_DETECT_EVIL   ) strcat( buf, " detect_evil"   );
    if ( vector & AFF_DETECT_GOOD   ) strcat( buf, " detect_good"   );
    if ( vector & AFF_DETECT_INVIS  ) strcat( buf, " detect_invis"  );
    if ( vector & AFF_DETECT_MAGIC  ) strcat( buf, " detect_magic"  );
    if ( vector & AFF_DETECT_HIDDEN ) strcat( buf, " detect_hidden" );
    if ( vector & AFF_SANCTUARY     ) strcat( buf, " sanctuary"     );
    if ( vector & AFF_FAERIE_FIRE   ) strcat( buf, " faerie_fire"   );
    if ( vector & AFF_INFRARED      ) strcat( buf, " infrared"      );
    if ( vector & AFF_CURSE         ) strcat( buf, " curse"         );
    if ( vector & AFF_FAERIE_FOG    ) strcat( buf, " faerie_fog"    );
    if ( vector & AFF_POISON        ) strcat( buf, " poison"        );
    if ( vector & AFF_PROTECT_EVIL  ) strcat( buf, " prot_evil"     );
    if ( vector & AFF_PROTECT_GOOD  ) strcat( buf, " prot_good"     );
    if ( vector & AFF_SLEEP         ) strcat( buf, " sleep"         );
    if ( vector & AFF_SNEAK         ) strcat( buf, " sneak"         );
    if ( vector & AFF_HIDE          ) strcat( buf, " hide"          );
    if ( vector & AFF_CHARM         ) strcat( buf, " charm"         );
    if ( vector & AFF_FLYING        ) strcat( buf, " flying"        );
    if ( vector & AFF_PASS_DOOR     ) strcat( buf, " pass_door"     );
    if ( vector & AFF_BERSERK	    ) strcat( buf, " berserk"	    );
    if ( vector & AFF_CALM	    ) strcat( buf, " calm"	    );
    if ( vector & AFF_HASTE	    ) strcat( buf, " haste"	    );
    if ( vector & AFF_SLOW          ) strcat( buf, " slow"          );
    if ( vector & AFF_PLAGUE	    ) strcat( buf, " plague" 	    );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
*/
}

char *affect2_bit_name( int flags )
{
  /* we run through table matching and catenating. */
  static char buf [MSL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; affect2_flags[i].bit > 0; i++){
    if (IS_SET(flags, affect2_flags[i].bit)){
      strcat(buf, " ");
      strcat(buf, affect2_flags[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
/*
    static char buf[512];
    buf[0] = '\0';
    if ( vector & AFF_PROTECT_SHIELD) strcat( buf, " protect_shield");
    if ( vector & AFF_MANA_SHIELD   ) strcat( buf, " mana_shield"   );
    if ( vector & AFF_BARRIER       ) strcat( buf, " barrier"       );
    if ( vector & AFF_HOLD          ) strcat( buf, " hold"          );
    if ( vector & AFF_BLUR          ) strcat( buf, " blur"          );
    if ( vector & AFF_TERRAIN       ) strcat( buf, " terrain"       );
    if ( vector & AFF_MISDIRECTION  ) strcat( buf, " misdirection"  );
    if ( vector & AFF_SHADOWFORM    ) strcat( buf, " shadowform"    );
    if ( vector & AFF_TREEFORM	    ) strcat( buf, " treeform"      );
    if ( vector & AFF_RAGE	    ) strcat( buf, " rage" 	    );
    if ( vector & AFF_CAMOUFLAGE    ) strcat( buf, " camouflage"    );
    if ( vector & AFF_KEEN_SIGHT    ) strcat( buf, " keen_sight"    );
    if ( vector & AFF_SILENCE  	    ) strcat( buf, " silence"  	    );
    if ( vector & AFF_SENSE_EVIL    ) strcat( buf, " sense_evil"    );
    if ( vector & AFF_CATALEPSY     ) strcat( buf, " catalepsy"     );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
*/
}

char *extra_bit_name( int flags )
{
  /* we run through table matching and catenating. */
  static char buf [MSL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; extra_flags[i].bit > 0; i++){
    if (IS_SET(flags, extra_flags[i].bit)){
      strcat(buf, " ");
      strcat(buf, extra_flags[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";

  /*

    static char buf[512];
    buf[0] = '\0';
    if ( extra_flags & ITEM_GLOW         ) strcat( buf, " glow"         );
    if ( extra_flags & ITEM_HUM          ) strcat( buf, " hum"          );
    if ( extra_flags & ITEM_DARK         ) strcat( buf, " dark"         );
    if ( extra_flags & ITEM_LOCK         ) strcat( buf, " lock"         );
    if ( extra_flags & ITEM_EVIL         ) strcat( buf, " evil"         );
    if ( extra_flags & ITEM_INVIS        ) strcat( buf, " invis"        );
    if ( extra_flags & ITEM_MAGIC        ) strcat( buf, " magic"        );
    if ( extra_flags & ITEM_NODROP       ) strcat( buf, " nodrop"       );
    if ( extra_flags & ITEM_BLESS        ) strcat( buf, " bless"        );
    if ( extra_flags & ITEM_ANTI_GOOD    ) strcat( buf, " anti-good"    );
    if ( extra_flags & ITEM_ANTI_EVIL    ) strcat( buf, " anti-evil"    );
    if ( extra_flags & ITEM_ANTI_NEUTRAL ) strcat( buf, " anti-neutral" );
    if ( extra_flags & ITEM_NOREMOVE     ) strcat( buf, " noremove"     );
    if ( extra_flags & ITEM_INVENTORY    ) strcat( buf, " inventory"    );
    if ( extra_flags & ITEM_NOPURGE	 ) strcat( buf, " nopurge"	);
    if ( extra_flags & ITEM_VIS_DEATH	 ) strcat( buf, " vis_death"	);
    if ( extra_flags & ITEM_PSI_ENHANCE	 ) strcat( buf, " psi_enhance"	);
    if ( extra_flags & ITEM_ROT_DEATH	 ) strcat( buf, " rot_death"	);
    if ( extra_flags & ITEM_NOLOCATE	 ) strcat( buf, " no_locate"	);
    if ( extra_flags & ITEM_SELL_EXTRACT ) strcat( buf, " sell_extract" );
    if ( extra_flags & ITEM_BURN_PROOF	 ) strcat( buf, " burn_proof"	);
    if ( extra_flags & ITEM_NOUNCURSE	 ) strcat( buf, " no_uncurse"	);
    if ( extra_flags & ITEM_CABALTAKE	 ) strcat( buf, " removed from altar"	);
    return ( buf[0] != '\0' ) ? buf+1 : "none";
  */
}

char *act_bit_name( int flags, bool first, bool npc )
{
    static char buf[512];
    buf[0] = '\0';
    if (first)
    {
      if (IS_SET(flags, ACT_IS_NPC) || npc){
	return flag_string( act_flags, flags );
      }
      else
    	{
	    strcat(buf," player");
	    if (flags & PLR_AUTOASSIST	) strcat(buf, " autoassist");
	    if (flags & PLR_AUTOEXIT	) strcat(buf, " autoexit");
	    if (flags & PLR_AUTOLOOT	) strcat(buf, " autoloot");
	    if (flags & PLR_AUTOSAC	) strcat(buf, " autosac");
	    if (flags & PLR_AUTOGOLD	) strcat(buf, " autogold");
	    if (flags & PLR_AUTOSPLIT	) strcat(buf, " autosplit");
	    if (flags & PLR_HOLYLIGHT	) strcat(buf, " holy_light");
	    if (flags & PLR_HEARALL	) strcat(buf, " hearall");
	    if (flags & PLR_NOSUMMON	) strcat(buf, " no_summon");
	    if (flags & PLR_NOFOLLOW	) strcat(buf, " no_follow");
	    if (flags & PLR_FREEZE	) strcat(buf, " frozen");
            if (flags & PLR_WANTED      ) strcat(buf, " wanted");
            if (flags & PLR_DOOF        ) strcat(buf, " doof");
	    if (flags & PLR_UNDEAD	) strcat(buf, " undead");
    	}
    }
    else
    {
      if (npc){
      	return flag_string( act2_flags, flags );
      }
    	else
    	{
	    strcat(buf," player");
	    if (flags & PLR_NOQUIT	    ) strcat(buf, " noquit");
	    if (flags & PLR_NOWHO	    ) strcat(buf, " nowho");
	    if (flags & PLR_ANNOY	    ) strcat(buf, " annoy");
	    if (flags & PLR_GIMP	    ) strcat(buf, " gimp");
	    if (flags & PLR_GARBLE	    ) strcat(buf, " garble");
	    if (flags & PLR_EQLIMIT	    ) strcat(buf, " eqlimit");
	    if (flags & PLR_DESC	    ) strcat(buf, " description");
	    if (flags & PLR_ROYAL	    ) strcat(buf, " royal");
	    if (flags & PLR_NOBLE	    ) strcat(buf, " noble");
	    if (flags & PLR_CODING	    ) strcat(buf, " coding");
	    if (flags & PLR_MASQUERADE  ) strcat(buf, " masquerade");
	    if (flags & PLR_AVATAR      ) strcat(buf, " avatar");
    	}
    }
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char* game_bit_name(int flags ){
  /* we run through table matching and catenating. */
  static char buf [MSL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; game_flags[i].bit > 0; i++){
    if (IS_SET(flags, game_flags[i].bit)){
      strcat(buf, " ");
      strcat(buf, game_flags[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char* perk_bit_name(int flags ){
  /* we run through table matching and catenating. */
  static char buf [MSL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; chargen_perk_table[i].int1 > 0; i++){
    if (IS_SET(flags, chargen_perk_table[i].int1)){
      strcat(buf, " ");
      strcat(buf, chargen_perk_table[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char* color_bit_name(int flags ){
  /* we run through table matching and catenating. */
  static char buf [MSL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; color_flags[i].bit > 0; i++){
    if (IS_SET(flags, color_flags[i].bit)){
      strcat(buf, " ");
      strcat(buf, color_flags[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}


char *comm_bit_name(int flags)
{
  /* we run through table matching and catenating. */
  static char buf [MSL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; comm_flags[i].bit > 0; i++){
    if (IS_SET(flags, comm_flags[i].bit)){
      strcat(buf, " ");
      strcat(buf, comm_flags[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *imm_bit_name(int flags)
{
/*    static char buf[512];
    buf[0] = '\0';
    if (imm_flags & IMM_SUMMON		) strcat(buf, " summon");
    if (imm_flags & IMM_CHARM		) strcat(buf, " charm");
    if (imm_flags & IMM_MAGIC		) strcat(buf, " magic");
    if (imm_flags & IMM_WEAPON		) strcat(buf, " weapon");
    if (imm_flags & IMM_BASH		) strcat(buf, " blunt");
    if (imm_flags & IMM_PIERCE		) strcat(buf, " piercing");
    if (imm_flags & IMM_SLASH		) strcat(buf, " slashing");
    if (imm_flags & IMM_FIRE		) strcat(buf, " fire");
    if (imm_flags & IMM_COLD		) strcat(buf, " cold");
    if (imm_flags & IMM_LIGHTNING	) strcat(buf, " lightning");
    if (imm_flags & IMM_ACID		) strcat(buf, " acid");
    if (imm_flags & IMM_POISON		) strcat(buf, " poison");
    if (imm_flags & IMM_NEGATIVE	) strcat(buf, " negative");
    if (imm_flags & IMM_HOLY		) strcat(buf, " holy");
    if (imm_flags & IMM_ENERGY		) strcat(buf, " energy");
    if (imm_flags & IMM_MENTAL		) strcat(buf, " mental");
    if (imm_flags & IMM_DISEASE         ) strcat(buf, " disease");
    if (imm_flags & IMM_DROWNING	) strcat(buf, " drowning");
    if (imm_flags & IMM_LIGHT		) strcat(buf, " light");
    if (imm_flags & VULN_IRON		) strcat(buf, " iron");
    if (imm_flags & VULN_SOUND		) strcat(buf, " sound");
    if (imm_flags & VULN_WOOD		) strcat(buf, " wood");
    if (imm_flags & VULN_SILVER         ) strcat(buf, " silver");
    if (imm_flags & VULN_MITHRIL         ) strcat(buf, " mithril");
    return ( buf[0] != '\0' ) ? buf+1 : "none";
*/
  /* we run through table matching and catenating. */
  static char buf [MSL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; imm_flags[i].bit > 0; i++){
    if (IS_SET(flags, imm_flags[i].bit)){
      strcat(buf, " ");
      strcat(buf, imm_flags[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *wear_bit_name(int flags)
{
  /* we run through table matching and catenating. */
  static char buf [MSL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; wear_flags[i].bit > 0; i++){
    if (IS_SET(flags, wear_flags[i].bit)){
      strcat(buf, " ");
      strcat(buf, wear_flags[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
/*
    static char buf[512];
    buf [0] = '\0';
    if (wear_flags & ITEM_TAKE		) strcat(buf, " take");
    if (wear_flags & ITEM_WEAR_FINGER	) strcat(buf, " finger");
    if (wear_flags & ITEM_WEAR_NECK	) strcat(buf, " neck");
    if (wear_flags & ITEM_WEAR_BODY	) strcat(buf, " torso");
    if (wear_flags & ITEM_WEAR_HEAD	) strcat(buf, " head");
    if (wear_flags & ITEM_WEAR_FACE     ) strcat(buf, " face");
    if (wear_flags & ITEM_WEAR_LEGS	) strcat(buf, " legs");
    if (wear_flags & ITEM_WEAR_FEET	) strcat(buf, " feet");
    if (wear_flags & ITEM_WEAR_HANDS	) strcat(buf, " hands");
    if (wear_flags & ITEM_WEAR_ARMS	) strcat(buf, " arms");
    if (wear_flags & ITEM_WEAR_SHIELD	) strcat(buf, " shield");
    if (wear_flags & ITEM_WEAR_ABOUT	) strcat(buf, " body");
    if (wear_flags & ITEM_WEAR_WAIST	) strcat(buf, " waist");
    if (wear_flags & ITEM_WEAR_WRIST	) strcat(buf, " wrist");
    if (wear_flags & ITEM_WIELD		) strcat(buf, " wield");
    if (wear_flags & ITEM_HOLD		) strcat(buf, " hold");
    if (wear_flags & ITEM_NO_SAC	) strcat(buf, " nosac");
    if (wear_flags & ITEM_WEAR_FLOAT	) strcat(buf, " float");
    if (wear_flags & ITEM_UNIQUE        ) strcat(buf, " unique");
    if (wear_flags & ITEM_WEAR_TATTOO   ) strcat(buf, " tattoo");
    if (wear_flags & ITEM_WEAR_SHROUD   ) strcat(buf, " shroud");
    if (wear_flags & ITEM_KNIGHT_ONLY   ) strcat(buf, " knight");
    if (wear_flags & ITEM_RARE          ) strcat(buf, " rare");
    if (wear_flags & ITEM_PARRY         ) strcat(buf, " parry");
    if (wear_flags & ITEM_WEAR_EARRING  ) strcat(buf, " earring");
    if (wear_flags & ITEM_HAS_OWNER     ) strcat(buf, " has_owner");
    return ( buf[0] != '\0' ) ? buf+1 : "none";
*/
}

extern const struct flag_type special_flags[];
char *special_bit_name(int flags)
{
  return flag_string( special_flags, flags );
}

char *form_bit_name(int form_flags)
{
    static char buf[512];
    buf[0] = '\0';
    if (form_flags & FORM_POISON	) strcat(buf, " poison");
    else if (form_flags & FORM_EDIBLE	) strcat(buf, " edible");
    if (form_flags & FORM_MAGICAL	) strcat(buf, " magical");
    if (form_flags & FORM_INSTANT_DECAY	) strcat(buf, " instant_rot");
    if (form_flags & FORM_OTHER		) strcat(buf, " other");
    if (form_flags & FORM_ANIMAL	) strcat(buf, " animal");
    if (form_flags & FORM_SENTIENT	) strcat(buf, " sentient");
    if (form_flags & FORM_UNDEAD	) strcat(buf, " undead");
    if (form_flags & FORM_CONSTRUCT	) strcat(buf, " construct");
    if (form_flags & FORM_MIST		) strcat(buf, " mist");
    if (form_flags & FORM_INTANGIBLE	) strcat(buf, " intangible");
    if (form_flags & FORM_BIPED		) strcat(buf, " biped");
    if (form_flags & FORM_CENTAUR	) strcat(buf, " centaur");
    if (form_flags & FORM_INSECT	) strcat(buf, " insect");
    if (form_flags & FORM_SPIDER	) strcat(buf, " spider");
    if (form_flags & FORM_CRUSTACEAN	) strcat(buf, " crustacean");
    if (form_flags & FORM_WORM		) strcat(buf, " worm");
    if (form_flags & FORM_BLOB		) strcat(buf, " blob");
    if (form_flags & FORM_MAMMAL	) strcat(buf, " mammal");
    if (form_flags & FORM_BIRD		) strcat(buf, " bird");
    if (form_flags & FORM_REPTILE	) strcat(buf, " reptile");
    if (form_flags & FORM_SNAKE		) strcat(buf, " snake");
    if (form_flags & FORM_DRAGON	) strcat(buf, " dragon");
    if (form_flags & FORM_AMPHIBIAN	) strcat(buf, " amphibian");
    if (form_flags & FORM_FISH		) strcat(buf, " fish");
    if (form_flags & FORM_COLD_BLOOD 	) strcat(buf, " cold_blooded");
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *part_bit_name(int part_flags)
{
    static char buf[512];
    buf[0] = '\0';
    if (part_flags & PART_HEAD		) strcat(buf, " head");
    if (part_flags & PART_ARMS		) strcat(buf, " arms");
    if (part_flags & PART_LEGS		) strcat(buf, " legs");
    if (part_flags & PART_HEART		) strcat(buf, " heart");
    if (part_flags & PART_BRAINS	) strcat(buf, " brains");
    if (part_flags & PART_GUTS		) strcat(buf, " guts");
    if (part_flags & PART_HANDS		) strcat(buf, " hands");
    if (part_flags & PART_FEET		) strcat(buf, " feet");
    if (part_flags & PART_FINGERS	) strcat(buf, " fingers");
    if (part_flags & PART_EAR		) strcat(buf, " ears");
    if (part_flags & PART_EYE		) strcat(buf, " eyes");
    if (part_flags & PART_LONG_TONGUE	) strcat(buf, " long_tongue");
    if (part_flags & PART_EYESTALKS	) strcat(buf, " eyestalks");
    if (part_flags & PART_TENTACLES	) strcat(buf, " tentacles");
    if (part_flags & PART_FINS		) strcat(buf, " fins");
    if (part_flags & PART_WINGS		) strcat(buf, " wings");
    if (part_flags & PART_TAIL		) strcat(buf, " tail");
    if (part_flags & PART_CLAWS		) strcat(buf, " claws");
    if (part_flags & PART_FANGS		) strcat(buf, " fangs");
    if (part_flags & PART_HORNS		) strcat(buf, " horns");
    if (part_flags & PART_SCALES	) strcat(buf, " scales");
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *weapon_bit_name(int weapon_flags)
{
  /* we run through table matching and catenating. */
  static char buf [MIL];
  int i = 0;
  buf[0] = '\0';
  for (i = 0; weapon_type2[i].bit > 0; i++){
    if (IS_SET(weapon_flags, weapon_type2[i].bit)){
      strcat(buf, " ");
      strcat(buf, weapon_type2[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
/*
    static char buf[512];
    buf[0] = '\0';
    if (weapon_flags & WEAPON_FLAMING	) strcat(buf, " flaming");
    if (weapon_flags & WEAPON_FROST	) strcat(buf, " frost");
    if (weapon_flags & WEAPON_VAMPIRIC	) strcat(buf, " vampiric");
    if (weapon_flags & WEAPON_SHARP	) strcat(buf, " sharp");
    if (weapon_flags & WEAPON_VORPAL	) strcat(buf, " vorpal");
    if (weapon_flags & WEAPON_TWO_HANDS ) strcat(buf, " two-handed");
    if (weapon_flags & WEAPON_SHOCKING 	) strcat(buf, " shocking");
    if (weapon_flags & WEAPON_POISON	) strcat(buf, " poison");
    if (weapon_flags & WEAPON_JEWELED   ) strcat(buf, " jeweled");
    if (weapon_flags & WEAPON_LOADED    ) strcat(buf, " loaded");
    return ( buf[0] != '\0' ) ? buf+1 : "none";
*/
}

char *room_bit_name(int room_flags)
{
     static char buf[512];
     buf[0]='\0';
     if (room_flags & ROOM_DARK         ) strcat(buf, " dark");
     if (room_flags & ROOM_MARK         ) strcat(buf, " mark");
     if (room_flags & ROOM_NO_MOB       ) strcat(buf, " no_mob");
     if (room_flags & ROOM_INDOORS      ) strcat(buf, " indoors");
     if (room_flags & ROOM_PRIVATE      ) strcat(buf, " private");
     if (room_flags & ROOM_SAFE         ) strcat(buf, " safe");
     if (room_flags & ROOM_SOLITARY     ) strcat(buf, " solitary");
     if (room_flags & ROOM_PET_SHOP     ) strcat(buf, " pet_shop");
     if (room_flags & ROOM_NO_QUIT      ) strcat(buf, " no_quit");
     if (room_flags & ROOM_NO_INOUT     ) strcat(buf, " no_inout");
     if (room_flags & ROOM_IMP_ONLY     ) strcat(buf, " imp_only");
     if (room_flags & ROOM_GODS_ONLY    ) strcat(buf, " gods_only");
     if (room_flags & ROOM_NEWBIES_ONLY ) strcat(buf, " newbies_only");
     if (room_flags & ROOM_NO_GHOST     ) strcat(buf, " no_ghost");
     if (room_flags & ROOM_NOWHERE      ) strcat(buf, " nowhere");
     if (room_flags & ROOM_REGISTRY     ) strcat(buf, " registry");
     if (room_flags & ROOM_NO_GATEIN    ) strcat(buf, " no_gatein");
     if (room_flags & ROOM_NO_GATEOUT   ) strcat(buf, " no_gateout");
     if (room_flags & ROOM_NO_SUMMONIN  ) strcat(buf, " no_summonin");
     if (room_flags & ROOM_NO_SUMMONOUT ) strcat(buf, " no_summonout");
     if (room_flags & ROOM_NO_RECALL    ) strcat(buf, " no_recall");
     if (room_flags & ROOM_NO_VORTEX    ) strcat(buf, " no_vortex");
     if (room_flags & ROOM_NO_SCAN      ) strcat(buf, " no_scan");
     if (room_flags & ROOM_NO_TELEPORTIN) strcat(buf, " no_teleportin");
     if (room_flags & ROOM_NO_TELEPORTOUT) strcat(buf, " no_teleportout");
     if (room_flags & ROOM_DARK_RIFT    ) strcat(buf, " dark_rift");
     return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *room2_bit_name(int flags)
{
  /* we run through table matching and catenating. */
  static char buf [MIL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; room_flags2[i].bit > 0; i++){
    if (IS_SET(flags, room_flags2[i].bit)){
      strcat(buf, " ");
      strcat(buf, room_flags2[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
  /* Abover is better
     static char buf[512];
     buf[0]='\0';
     if (room_flags2 & ROOM_NO_MAGIC         ) strcat(buf, " no_magic");
     if (room_flags2 & ROOM_PSI_FEAR         ) strcat(buf, " psi_fear");
     if (room_flags2 & ROOM_DAY              ) strcat(buf, " day");
     if (room_flags2 & ROOM_NIGHT            ) strcat(buf, " night");
     if (room_flags2 & ROOM_NO_NEWBIES	     ) strcat(buf, " no_newbies");
     if (room_flags2 & ROOM_ALARM	     ) strcat(buf, " alarm");
      return ( buf[0] != '\0' ) ? buf+1 : "none";
  */
}

char *sector_bit_name(int sector_flags)
{
     static char buf[512];
     buf[0]='\0';
     if (sector_flags == SECT_INSIDE     ) strcat(buf, " inside");
     if (sector_flags == SECT_CITY       ) strcat(buf, " city");
     if (sector_flags == SECT_FIELD      ) strcat(buf, " field");
     if (sector_flags == SECT_FOREST     ) strcat(buf, " forest");
     if (sector_flags == SECT_HILLS      ) strcat(buf, " hills");
     if (sector_flags == SECT_MOUNTAIN   ) strcat(buf, " mountain");
     if (sector_flags == SECT_WATER_SWIM ) strcat(buf, " water_swim");
     if (sector_flags == SECT_WATER_NOSWIM) strcat(buf," water_noswim");
     if (sector_flags == SECT_SWAMP      ) strcat(buf, " swamp");
     if (sector_flags == SECT_AIR        ) strcat(buf, " air");
     if (sector_flags == SECT_DESERT     ) strcat(buf, " desert");
     if (sector_flags == MAX_SECT        ) strcat(buf, " max");
     return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *cont_bit_name( int cont_flags)
{
    static char buf[512];
    buf[0] = '\0';
    if (cont_flags & CONT_CLOSEABLE	) strcat(buf, " closable");
    if (cont_flags & CONT_PICKPROOF	) strcat(buf, " pickproof");
    if (cont_flags & CONT_CLOSED	) strcat(buf, " closed");
    if (cont_flags & CONT_LOCKED	) strcat(buf, " locked");
    return (buf[0] != '\0' ) ? buf+1 : "none";
}

char *off_bit_name(int flags){
  /* we run through table matching and catenating. */
  static char buf [MSL];
  int i = 0;

  buf[0] = '\0';

  for (i = 0; off_flags[i].bit > 0; i++){
    if (IS_SET(flags, off_flags[i].bit)){
      strcat(buf, " ");
      strcat(buf, off_flags[i].name);
    }
  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

CHAR_DATA *char_file_active (char *argument )
{
    CHAR_DATA *wch;
    char name_cap1[MIL];
    char *name_cap2;
    sh_int count;
    for ( wch = player_list; wch != NULL ; wch = wch->next_player )
    {
        name_cap2 = capitalize(argument);
        for (count = 0; name_cap2[count]; count++)
            name_cap1[count] = name_cap2[count];
        name_cap1[count] = 0;
        if (!strcmp (name_cap1, capitalize (wch->name)))
            return wch;
    }
    return NULL;
}

/* returns a random area exit from given area */
AREA_DATA* get_rand_aexit(AREA_DATA* pArea){
  EXIT_DATA *exit;
  ROOM_INDEX_DATA *to_room, *room;
  AREA_DATA* list[top_area];
  int max = 0;
  int i = 0;
  int j = 0;
  int k = 0;

  for (j = 0; j < MAX_KEY_HASH; j++){
    for (room = room_index_hash[j]; room != NULL; room = room->next){
      for (i = 0; i < 6; i++){
	bool fFound = FALSE;
	if ( (exit = room->exit[i]) == NULL)
	  continue;
	if ( (to_room = exit->to_room) == NULL)
	  continue;
	/* check for different areas on each side */
	/* check for normal two way exits/entrance */
	if ( room->area == pArea
	     && to_room->area != pArea
	     && to_room->exit[rev_dir[i]]
	     && to_room->exit[rev_dir[i]]->to_room == room ){
	  for (k = 0; k < max; k++){
	    if (list[k] == to_room->area){
	      fFound = TRUE;
	      break;
	    }
	  }
	  if (!fFound)
	    list[max++] = to_room->area;
	}//end area exit found
      }//end exits
    }//end for room
  }//end for hash table

  if (max < 1)
    return NULL;
  else
    return list[number_range(0, max - 1)];
}


/* this is a  function that returns a random area based on passed prerequistes.
   min_av: minimum (>=) AREA vnum to choose from (0 to ignore)
   max_av: max AREA (<=) vnum to choose from (0 to ignore)

   min_rv: minimum ROOM vnum to that area must contain
   max_av: max ROOM vnum to that area must contain

   includ: array of area names to include (NULL to ignore)
   i_num : length of above array

   exclud: array of area names to exclude (NULL to ignore)
   e_num : length of above array

If all of the above are satisfied, an area is added to a list.
then a single area on the list is chosen and returned, or NULL for none
*/

AREA_DATA* get_rand_area(int min_av, int max_av,
			   int min_rv, int max_rv,
			   char** includ, int i_num,
			   char** exclud, int e_num){
  AREA_DATA* pArea;
  AREA_DATA* list[top_area];
  int max = 0;
  int i = 0;

  for ( pArea = area_first; pArea != NULL; pArea = pArea->next ){
    if ( IS_SET(pArea->area_flags, AREA_MUDSCHOOL)
	 || IS_SET(pArea->area_flags, AREA_RESTRICTED)
	 || IS_SET(pArea->area_flags, AREA_CABAL))
      continue;
    /* check for area vnum in range */
    if (pArea->vnum < min_av 	|| (max_av > 0 && pArea->vnum > max_av))
      continue;
    /* check for room in range */
    if ( (max_rv > 0 && pArea->min_vnum > max_rv) || pArea->max_vnum < min_rv)
      continue;
    /* check of includes */
    if (i_num > 0 && includ != NULL){
      bool fSkip = TRUE;
      for (i = 0; i < i_num && includ[i] != NULL; i++){
	if (!str_cmp(includ[i], pArea->name)){
	  fSkip = FALSE;
	  break;
	}
      }
      if (fSkip)
	continue;
    }
    if (e_num > 0 && exclud != NULL){
      bool fSkip = FALSE;
      /* check of excludes */
      for (i = 0; i < e_num && exclud[i] != NULL; i++){
	if (!str_cmp(exclud[i], pArea->name)){
	  fSkip = TRUE;
	  break;
	}
      }
      if (fSkip)
	continue;
    }
    /* seems all good */
    list[max++] = pArea;
  }//end for
  if (max == 0)
    return NULL;
/* select a single random area from list */
  return list[number_range(0, max - 1)];
}

/*
   this is a  function that returns a random room based on passed prerequistes.
   min_av: minimum (>=) AREA vnum to choose from (0 to ignore)
   max_av: max AREA (<=) vnum to choose from (0 to ignore)

   min_rv: minimum ROOM vnum to that area must contain
   max_av: max ROOM vnum to that area must contain

   i_area: array of area names to include	(NULL to ignore)
   ia_num: length of the includ array

   e_area: array of area names to exclude	(NULL to ignore)
   ea_num: length of the includ array

   i_sect: array of sectors requried		(NULL to ignore)
   is_num: length of the sector array

   e_sect: array of sectors excluded		(NULL to ignore)
   es_num: length of the sector array

   i_rom1: array of room1 bits required		(NULL to ignore)
   ir1_num:length of above array

   e_rom1: array of room1 bits excluded		(NULL to ignore)
   er1_num: length of above array

   i_rom2: array of room1 bits required		(NULL to ignore)
   ir2_num:length of above array

   e_rom2: array of room1 bits excluded		(NULL to ignore)
   er2_num: length of above array

   int pool: number of areas to use as a pool (top_area/10 maximum)

   fExit: TRUE to check for avaliable exit in the room as a requirement

   ch:	Optional character pointer, if passed then rooms are checked
   for characters ability to enter or see.

   The function cheats a bit by selecting rooms only out of a pool
   of random areas, rather then out of the whole mud (far to costly)
   Then a list of rooms using above requierements is compiled and one room
   is seleceted.

   FOLLOWING AUTOMATICLY REJECT A ROOM/AREA
   - area: mudschool
   - area: system (restricted)
   - room: safe
   - room: private
   - room: jailcell
   - room: solitary
   with character passed, following are auto rejected:
   - area: nogate
   - room: no_inout
   - room: no_ghost (and ghosted)
   - room: no_newbie (and newbie)
   - room: cannot see the room
*/
extern int			top_vir_room; //used to loop through virrooms
ROOM_INDEX_DATA  *get_rand_room
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
 ){
  int areas = URANGE(1, pool, top_area / 10);
  char* e_area2[ea_num + areas];//exclude list used to pick the seed areas
  int ea_num2 = 0;

  AREA_DATA* a_list[areas];	//list of areas we will pluck the rooms from
  int amax = 0;			//count of usuable areas

  const int max_rooms = 1000;	//total maximum of rooms we will select from
  ROOM_INDEX_DATA* r_list[max_rooms];
  int rmax = 0;

  int i = 0;

/* copy currect exclude areas into the second array */
  for (i = 0; e_area != NULL && i < ea_num; i++)
    e_area2[ea_num2++] = e_area[i];

/* first get the pool of areas */
  /* this also takes care of all the conditions up to sectors */
  for (i=0; i < areas; i++){
    AREA_DATA* pArea = get_rand_area(min_av,max_av, min_rv,max_rv,
				     i_area, ia_num, e_area2, ea_num2);
    if (pArea == NULL)
      break;
    /* we add this area onto exclude list so its not chosen next time */
    e_area2[ea_num2++] = pArea->name;
    /* and add it to the list as well */
    a_list[amax++] = pArea;
  }
/* at this point check if we have at least one usuable area */
  if (amax < 1)
    return NULL;

/* areas prepared, get ready for room selection */
/*
   seems the only sane way of doing this is getting rooms out of the hash
   table using get_room_index.  Worst case scenario is about top_room / 1024
   loops in get_room_index per room chosen here. :(

   We handle virtual rooms by then running through ALL the virtual room
   indexes since their vnums are not part of the area min/max
*/
  /* this loops runs through all the areas selected */
  for (i = 0; i < amax; i++){
    AREA_DATA* pArea  = a_list[i];
    ROOM_INDEX_DATA* pRoom;
    int vnum = 0, cur_vnum;


    /* We make this complicated while loop in order to run through:
       1) All room vnums in the area
       2) All virtual rooms starting at lowest virroom vnum
          and ending on the highest ever assigned
    */

    //prime the first vnum
    vnum = pArea->min_vnum;
    while (vnum){
      //assign our current working vnum
      cur_vnum = vnum;
      //select the next vnum to work on
      if (vnum < pArea->max_vnum)
	vnum++;
      else if (vnum == pArea->max_vnum)
	vnum = MAX_ROOM_VNUM;
      else if (vnum < MAX_ROOM_VNUM + top_vir_room)
	vnum++;
      else
	vnum = 0; //stop the loop after this iteration

      if ( ( pRoom = get_room_index( cur_vnum )) == NULL
	   || pRoom->area != pArea
	   || IS_SET(pRoom->area->area_flags, AREA_MUDSCHOOL)
	   || IS_SET(pRoom->area->area_flags, AREA_RESTRICTED)
	   || IS_SET(pRoom->area->area_flags, AREA_CABAL)
	   || IS_SET(pRoom->room_flags, ROOM_JAILCELL)
	   || IS_SET(pRoom->room_flags, ROOM_SOLITARY)
	   || IS_SET(pRoom->room_flags, ROOM_SAFE)
	   || IS_SET(pRoom->room_flags, ROOM_PRIVATE) )
	continue;
      /* room exclude */
    /* check for room in range */
    /* check for room in range */
      if ( (max_rv > 0 && pRoom->vnum > max_rv) || pRoom->vnum < min_rv)
	continue;
      /* includes/excludes for each type */
      /* SECTOR (OR) */
      if (is_num > 0 && i_sect != NULL){
	bool fSkip = TRUE;
	int i = 0;
	/* check of list */
	for (i = 0; i < is_num; i++){
	  if (i_sect[i] == pRoom->sector_type){
	    fSkip = FALSE;
	    break;
	  }
	}
	if (fSkip)
	  continue;
      }
      /* (OR) */
      if (es_num > 0 && e_sect != NULL){
	bool fSkip = FALSE ;
	int i = 0;
	/* check of list */
	for (i = 0; i < es_num; i++){
	  if (e_sect[i] == pRoom->sector_type){
	    fSkip = TRUE;
	    break;
	  }
	}
	if (fSkip)
	  continue;
      }

      /* ROOM1 (OR) */
      if (ir1_num > 0 && i_rom1 != NULL){
	bool fSkip = TRUE;
	int i = 0;
	/* check of list */
	for (i = 0; i < ir1_num; i++){
	  if (IS_SET(pRoom->room_flags, i_rom1[i])){
	    fSkip = FALSE;
	    break;
	  }
	}
	if (fSkip)
	  continue;
      }
      /* (OR) */
      if (er1_num > 0 && e_rom1 != NULL){
	bool fSkip = FALSE ;
	int i = 0;
	/* check of list */
	for (i = 0; i < er1_num; i++){
	  if (IS_SET(pRoom->room_flags, e_rom1[i])){
	    fSkip = TRUE;
	    break;
	  }
	}
	if (fSkip)
	  continue;
      }

      /* ROOM2 (OR) */
      if (ir2_num > 0 && i_rom2 != NULL){
	bool fSkip = TRUE;
	int i = 0;
	/* check of list */
	for (i = 0; i < ir2_num; i++){
	  if (IS_SET(pRoom->room_flags2, i_rom2[i])){
	    fSkip = TRUE;
	    break;
	  }
	}
	if (fSkip)
	  continue;
      }
      /* (OR) */
      if (er2_num > 0 && e_rom2 != NULL){
	bool fSkip = FALSE ;
	int i = 0;
	/* check of list */
	for (i = 0; i < er2_num; i++){
	  if (IS_SET(pRoom->room_flags2, e_rom2[i])){
	    fSkip = TRUE;
	    break;
	  }
	}
	if (fSkip)
	  continue;
      }
      /* CHAR check */
      if (ch  && (IS_SET(pRoom->area->area_flags, AREA_RESTRICTED)
		  || IS_SET(pRoom->room_flags, ROOM_NO_INOUT)
		  || (IS_SET(pRoom->room_flags, ROOM_NO_GHOST)
		      && is_ghost(ch, 600))
		  || (IS_SET(pRoom->room_flags, ROOM_NEWBIES_ONLY)
		      && ch->level <= LEVEL_NEWBIE)
		  || !can_see_room(ch, pRoom)) )
	continue;

      /* EXIT check */
      /* we unroll this loop so its a wee bit faster */
      if (fExit){
	EXIT_DATA** pE = pRoom->exit;
	if ( (pE[0] == NULL || IS_SET((pE[0])->exit_info, EX_CLOSED))
	     && (pE[1] == NULL || IS_SET((pE[1])->exit_info, EX_CLOSED))
	     && (pE[2] == NULL || IS_SET((pE[2])->exit_info, EX_CLOSED))
	     && (pE[3] == NULL || IS_SET((pE[3])->exit_info, EX_CLOSED))
	     && (pE[4] == NULL || IS_SET((pE[4])->exit_info, EX_CLOSED))
	     && (pE[5] == NULL || IS_SET((pE[5])->exit_info, EX_CLOSED)) )
	  continue;
      }
      if (fSafe && pRoom->people)
	continue;
      /* all conditions met, add the room */
      r_list[rmax++] = pRoom;
    }//END OF ROOM LOOP
  }//END OF AREA LOOP

  /* check if we got any rooms out of this */
  if (rmax < 1)
    return NULL;
  else
    return r_list[number_range(0, rmax - 1)];
}




ROOM_INDEX_DATA  *get_random_room(CHAR_DATA *ch, bool fArea, bool fGimp)
{
  int gimp_ranges [GIMP_NUM][2] =
    {
      { 400,	599},
      { 10300,	10547},
      { 10600,	10694},
      { 10800,	10893},
      { 12001,	12100}
    };

    ROOM_INDEX_DATA *room;
    int i, random;
    bool gimp = FALSE;

    if (fGimp && number_percent() < 33)
      gimp = TRUE;

    for (i = 0; i < 100; i++)
      {
	if (gimp)
	  {
	    random = number_range( 0, GIMP_NUM - 1 );
	    room = get_room_index( number_range( gimp_ranges[random][0], gimp_ranges[random][1] ) );
	}
	else if (fArea)
	    room = get_room_index( number_range( ch->in_room->area->min_vnum, ch->in_room->area->max_vnum ) );
	else
            room = get_room_index( number_range( 0, 32767 ) );

        if ( room != NULL )
            if ( can_see_room(ch,room) && !room_is_private(room) && !IS_SET(room->room_flags, ROOM_PRIVATE)
            && !IS_SET(room->room_flags, ROOM_SOLITARY) && !IS_SET(room->room_flags, ROOM_SAFE)
	    && !IS_SET(room->room_flags, ROOM_NO_INOUT))
                break;
	room = NULL;
    }
    return room;
}


int get_charmed_by (CHAR_DATA *ch)
{
    int i = 0;
    CHAR_DATA *vch;
    for (vch = char_list; vch; vch = vch->next)
        if (IS_AFFECTED (vch, AFF_CHARM) && vch->master == ch)
            i++;
    if (get_summoned(ch,MOB_VNUM_SHADOWDEMON) > 0)
      i = UMAX(0, i -1);
    return i;
}


int get_summoned (CHAR_DATA *ch, int vnum)
{

    int i = 0;
    CHAR_DATA *vch;
    for (vch = char_list; vch; vch = vch->next)
        if (IS_NPC(vch) && vch->pIndexData->vnum == vnum && vch->summoner == ch)
            i++;
    return i;
}

int get_leadered (CHAR_DATA *ch, int vnum)
{
    int i = 0;
    CHAR_DATA *vch;
    for (vch = char_list; vch; vch = vch->next)
        if (IS_NPC(vch) && vch->pIndexData->vnum == vnum && vch->leader == ch)
            i++;
    return i;
}

int get_control (CHAR_DATA *ch, int vnum)
{
    int i = 0;
    CHAR_DATA *vch;
    for (vch = char_list; vch; vch = vch->next)
        if (IS_NPC(vch) && vch->pIndexData->vnum == vnum && vch->master == ch && IS_AFFECTED(vch,AFF_CHARM))
            i++;
    return i;
}

int get_temple (CHAR_DATA *ch)
{
  if (IS_GOOD(ch))
    return hometown_table[ch->hometown].g_vnum;
  else if (IS_EVIL(ch))
    return hometown_table[ch->hometown].e_vnum;
  else if (IS_NEUTRAL(ch) )
    return hometown_table[ch->hometown].n_vnum;
  else
    return ROOM_VNUM_LIMBO;
}


/*returns pointer to a cabal pit */
OBJ_DATA* get_cabal_pit( CABAL_DATA* pc ){
  CABAL_DATA* pPar = get_parent( pc );
  OBJ_DATA* obj;

  if (pPar->pitroom == NULL)
    return NULL;

/* look through items here and find first notake container */
  for (obj = pPar->pitroom->contents; obj; obj = obj->next_content ){
    if (obj->pCabal && obj->pCabal == pPar && obj->item_type == ITEM_CONTAINER && !CAN_WEAR(obj, ITEM_TAKE))
      return obj;
  }
  return NULL;
}


int get_temple_pitroom (CHAR_DATA *ch)
{
  if (ch->alignment > 0)
    return hometown_table[ch->hometown].pitr_g_vnum;
  else if (ch->alignment < 0)
    return hometown_table[ch->hometown].pitr_e_vnum;
  else if (ch->alignment == 0 )
    return hometown_table[ch->hometown].pitr_n_vnum;
  else
    return ROOM_VNUM_LIMBO;
}


/* finds the pit vnum of pit room based on hometown_table cabal_table */
int get_temple_pit (int pitroom){
  int city = 0;

  for (city = 0; city < MAX_HOMETOWN; city++){
    if (hometown_table[city].pitr_g_vnum == pitroom)
      return hometown_table[city].pit_g_vnum;
    if (hometown_table[city].pitr_n_vnum == pitroom)
      return hometown_table[city].pit_n_vnum;
    if (hometown_table[city].pitr_e_vnum == pitroom)
      return hometown_table[city].pit_e_vnum;
  }

  return ROOM_VNUM_LIMBO;
}


bool is_ignore(CHAR_DATA *ch,CHAR_DATA *victim)
{
    if (IS_NPC(victim) || IS_NPC(ch))
	return FALSE;
    if (IS_IMMORTAL(victim))
	return FALSE;
    if (ch->pcdata->ignore[0] == '\0')
    	return FALSE;
    if (ch == victim)
	return FALSE;
    if (!is_name(victim->name,ch->pcdata->ignore))
	return FALSE;
    return TRUE;
}

bool vamp_check(CHAR_DATA *ch)
{
    if (IS_NPC(ch))
        return FALSE;
    if (IS_IMMORTAL(ch) || IS_MASTER(ch))
        return FALSE;
    if (ch->class != class_lookup("vampire"))
      return FALSE;
    if (ch->in_room == NULL)
	return FALSE;
    if (is_affected(ch,gsn_covenant))
	return FALSE;
    if (IS_SET(ch->in_room->room_flags2,ROOM_NIGHT) )
	return FALSE;
    if (IS_SET(ch->in_room->room_flags2,ROOM_DAY))
      return TRUE;
    if ( (mud_data.time_info.hour > 6 && mud_data.time_info.hour < 19)
	&& !IS_SET(ch->in_room->room_flags, ROOM_DARK)
	&& !IS_SET(ch->in_room->room_flags, ROOM_INDOORS)
	&& ch->in_room->sector_type !=  SECT_INSIDE)
      return TRUE;
    return FALSE;
}

bool vamp_day_check(CHAR_DATA *ch)
{
    if (IS_NPC(ch))
        return FALSE;
    if (IS_IMMORTAL(ch) || IS_MASTER(ch))
        return FALSE;
    if (ch->class != class_lookup("vampire"))
      return FALSE;
    if (is_affected(ch,gsn_covenant))
	return FALSE;
    if (IS_SET(ch->in_room->room_flags2,ROOM_NIGHT) )
	return FALSE;
    if (IS_SET(ch->in_room->room_flags2,ROOM_DAY) )
	return TRUE;
    if (mud_data.time_info.hour > 6 && mud_data.time_info.hour < 19)
      return TRUE;
    return FALSE;
}

bool in_monastery(CHAR_DATA *ch)
{
  int iGuild;
    if (ch->in_room->vnum == get_temple(ch))
	return TRUE;
    if (ch->in_room->vnum == (get_temple_pitroom(ch)))
	return TRUE;
    for ( iGuild = 0; iGuild < MAX_GUILD; iGuild ++) {
      if ( ch->in_room->vnum == class_table[ch->class].guild[iGuild] )
	return TRUE;
      if ( ch->in_room->vnum == class_table[ch->class].guild[iGuild]+1 )
	return TRUE;
    }
    return FALSE;
}

int get_monk(int value)
{
    int i;
    for (i = 0; i < 8; i++)
	if (monk_table[i].value == value)
	    return monk_table[i].weight * 10;
    return 10000;
}

bool monk_good(CHAR_DATA *ch, int value)
{
    OBJ_DATA *obj;
    obj = get_eq_char(ch, value);
    if (obj == NULL)
      return TRUE;
    if (get_obj_weight(obj) <= get_monk(value))
      return TRUE;
    return FALSE;
}

void set_delay(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if (ch == NULL)
	return;
    if (!IS_NPC(ch) && victim == NULL)
    {
	ch->pcdata->pk_delay = mud_data.current_time;
	return;
    }
    if (IS_NPC(ch) && victim == NULL)
	return;
    if (!IS_NPC(ch))
    {
	if (!IS_NPC(victim))
	{
	    ch->pcdata->pk_delay = mud_data.current_time;
	    victim->pcdata->pk_delay = mud_data.current_time;
	}
	else
	    ch->pcdata->fight_delay = mud_data.current_time;
    }
    else if (!IS_NPC(victim))
        victim->pcdata->fight_delay = mud_data.current_time;
}

void WAIT_STATE2(CHAR_DATA *ch, int npulse)
{
    AFFECT_DATA *paf;
    if ((paf = affect_find(ch->affected,gsn_ghoul_touch)) != NULL)
    {
    	paf->modifier--;
    	if (paf->modifier == 0)
    	{
	    send_to_char("You no longer feel sluggish.\n\r",ch);
	    act("$n doesn't look as sluggish.",ch,NULL,NULL,TO_ROOM);
	    affect_remove(ch,paf);
    	}
    	else
	  send_to_char("You feel sluggish.\n\r",ch);
    	npulse += UMIN(npulse, PULSE_VIOLENCE);
    }
    else if ((paf = affect_find(ch->affected,gsn_paralyze)) != NULL)
    {
    	paf->modifier--;
    	if (paf->modifier == 0)
    	{
	  act(skill_table[gsn_paralyze].msg_off, ch, NULL, NULL, TO_CHAR);
	  act(skill_table[gsn_paralyze].msg_off2, ch, NULL, NULL, TO_ROOM);
	  affect_remove(ch,paf);
    	}
    	else
	  send_to_char("You can barely move!.\n\r",ch);
    	npulse += UMIN(npulse, PULSE_VIOLENCE);
    }
    WAIT_STATE(ch,npulse);
}

int tarot_find(AFFECT_DATA *paf)
{
    AFFECT_DATA *paf_find;
    for ( paf_find = paf; paf_find != NULL; paf_find = paf_find->next )
    {
        if ( paf_find->type == gsn_tarot && paf_find->level == 69 )
	return paf_find->modifier;
    }
    return 0;
}


SKILL_DATA  *nskill_find(SKILL_DATA *nsk, int sn)
{
    SKILL_DATA *nsk_find;
    for ( nsk_find = nsk; nsk_find != NULL; nsk_find = nsk_find->next )
    {
        if ( nsk_find->sn == sn )
	return nsk_find;
    }
    return NULL;
}

void nskill_to_char( CHAR_DATA *ch, SKILL_DATA *nsk )
{
    if (IS_NPC(ch))
	return;
    else
    {
    SKILL_DATA *nsk_new = new_skill();
    *nsk_new		= *nsk;
    VALIDATE(nsk);
    nsk_new->next	= ch->pcdata->newskills;
    ch->pcdata->newskills	= nsk_new;
    }
}


void nskill_remove( CHAR_DATA *ch, SKILL_DATA *nsk)
{
    if (IS_NPC(ch))
	return;
    if ( ch->pcdata->newskills == NULL )
    {
	bug( "nskills_remove: no skills from %d.", 0);
	return;
    }
    if ( nsk == ch->pcdata->newskills )
	ch->pcdata->newskills	= nsk->next;
    else
    {
	SKILL_DATA *prev;
	for ( prev = ch->pcdata->newskills; prev != NULL; prev = prev->next )
	    if ( prev->next == nsk )
	    {
		prev->next = nsk->next;
		break;
	    }
	if ( prev == NULL )
	{
	    bug( "nskills_remove: cannot find skills %d.", 0);
	    return;
	}
    }
    free_skill(nsk);
}

bool has_nskill( CHAR_DATA *ch, int sn )
{
    SKILL_DATA *nsk;
    if (IS_NPC(ch))
	return FALSE;
    for ( nsk = ch->pcdata->newskills; nsk != NULL; nsk = nsk->next )
	if ( nsk->sn == sn )
	    return TRUE;
    return FALSE;
}

void clear_nskill( CHAR_DATA *ch )
{
    SKILL_DATA *nsk, *nsk_next;
    if (IS_NPC(ch))
	return;
    for ( nsk = ch->pcdata->newskills; nsk != NULL; nsk = nsk_next )
    {
        nsk_next = nsk->next;
        if ( nsk->keep == 0 )
            nskill_remove( ch, nsk);
    }
}


/* has_custom_title */
/* linked into skills3.c, act_wiz.c, update.c */
/* written by Viri */
bool has_custom_title(CHAR_DATA* ch)
{
  char buf[MIL];
//returns TRUE if char has a proper title for his rank.
//we get a proper char title.
//since sex can change over time we need to compare both

  if (IS_SET(ch->act2, PLR_MASQUERADE))
    sprintf( buf, " the %s",
	     title_table [class_lookup("dark-knight")] [ch->level] [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
  else if (ch->class == class_lookup("crusader"))
    sprintf(buf, " %s", get_crus_title(ch, ch->level));
  else
    sprintf( buf, " the %s",
	     title_table [ch->class] [ch->level] [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );


  //and we compare with current.
  if (strcmp(buf, ch->pcdata->title))
    return TRUE;
  else
    return FALSE;
}//end has_custom title


inline void string_to_affect(AFFECT_DATA* paf, char* str)
{
  char buf [MIL];

  strcpy(buf, str);

  //replacing the string.
  string_from_affect(paf);
  paf->string = str_dup(buf);
  paf->has_string = TRUE;
}

inline void string_from_affect(AFFECT_DATA* paf)
{
  /* NOTE: Due to codes ability to recycle affects
     the data inside recycled affects tends to float after reboots.
     hence we have a double safety system for the string release.
     (pointer might float, the bool shouldn't */
//  if (paf->has_string)
  free_string(paf->string);
  paf->string = &str_empty[0];
  paf->has_string = FALSE;

}


int deity_lookup( const char *name )
{
    int god;
    for ( god = 0; god < (MAX_DIETY); god++ )
    {
      if ( deity_table[god].god == NULL )
	continue;
      if ( LOWER(name[0]) == LOWER(deity_table[god].god[0]) && !str_prefix( name, deity_table[god].god ) )
	return god;
    }//end for
    return -1;
}

int way_lookup( const char *name )
{
    int god;
    for ( god = 0; god < (MAX_DIETY); god++ )
    {
      if ( deity_table[god].way == NULL )
	continue;
      if ( LOWER(name[0]) == LOWER(deity_table[god].way[0]) && !str_prefix( name, deity_table[god].way) )
	return god;
    }//end for
    return -1;
}

/* is_owner */
/* return TRUE if obj is ownerd by ch */
/* written by viri */
bool is_owner(OBJ_DATA* obj, CHAR_DATA* ch)
{
  /*
 this is a small function to check if given char. is
the owner of the object.
  */
  AFFECT_DATA* paf;

  if ( (paf = affect_find(obj->affected, gen_has_owner)) == NULL)
    return TRUE;

  if (paf->has_string
      && !IS_NULLSTR(paf->string)
      && strcasecmp(paf->string, ch->name) == 0)
    return TRUE;
  return FALSE;

}

bool can_loot(CHAR_DATA* ch, OBJ_DATA* container){
  CHAR_DATA* wch;

  /* Vulture check, for first 2 ticks only following can loot the corpse: *
   * Thiefs, Those in PK of victim, Those in group of victim, IMMS	*/
  /* Get original owner of the corpse */
  for ( wch = player_list; wch != NULL; wch = wch->next_player ){
    if (wch->id == container->owner){
      break;
    }
  }
  /* If owner not found all bets are off, otherwise we check */
  if (wch
      && container->idle < 3  /* This is fickle indicator since its reset each time corpse is moved *
			       * Which can be done by few spells, so we check again for very old corpses */
      && container->timer > 20
      && !IS_IMMORTAL(ch)
      && (IS_NPC(ch) ? (ch->master ? !is_pk_abs(ch->master, wch) : TRUE) : !is_pk_abs(ch, wch))
      && (IS_NPC(ch) ? (ch->master ? !is_same_group(ch->master, wch) : TRUE) : !is_same_group(ch, wch))
      && ch->class != class_lookup("thief") ){
    return FALSE;
  }
  return TRUE;
}

int dir_lookup(char* arg){
  if      (!str_cmp(arg, "n") || !str_cmp(arg, "north")) return DIR_NORTH;
  else if (!str_cmp(arg, "e") || !str_cmp(arg, "east"))  return DIR_EAST;
  else if (!str_cmp(arg, "s") || !str_cmp(arg, "south")) return DIR_SOUTH;
  else if (!str_cmp(arg, "w") || !str_cmp(arg, "west"))  return DIR_WEST;
  else if (!str_cmp(arg, "u") || !str_cmp(arg, "up" ))   return DIR_UP;
  else if (!str_cmp(arg, "d") || !str_cmp(arg, "down"))  return DIR_DOWN;
  else
    return -1;
}

/* returns first groupmate in the room */
/* if fCharm then ignores the charmed ones */
CHAR_DATA* get_group_room(CHAR_DATA* ch, bool fCharm){
  CHAR_DATA* vch;
  for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
    if (vch == ch || (IS_AFFECTED(vch, AFF_CHARM) && fCharm))
      continue;
    else if (is_same_group(ch, vch))
      return vch;
  }
  return NULL;
}

/* returns first groupmate in the room */
/* if fCharm then ignores the charmed ones */
CHAR_DATA* get_group_world(CHAR_DATA* ch, bool fCharm){
  CHAR_DATA* vch;
  for (vch = char_list; vch; vch = vch->next){
    if (vch == ch || (IS_AFFECTED(vch, AFF_CHARM) && fCharm))
      continue;
    else if (is_same_group(ch, vch))
      return vch;
  }
  return NULL;
}

/* checks if the character is using two hands for his weapon */
OBJ_DATA* has_twohanded(CHAR_DATA* ch){
  OBJ_DATA* obj;
  if ( (obj = get_eq_char(ch, WEAR_WIELD)) == NULL)
    return NULL;

//Item check (must be 2 h weapon)
  if (!IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
      && !is_affected(ch, gsn_double_grip))
    return NULL;

//heck if held in 2 hands (second hand empty.)
  if (get_eq_char(ch,WEAR_SHIELD)
      || get_eq_char(ch,WEAR_SECONDARY)
      || get_eq_char(ch,WEAR_HOLD))
    return NULL;
  return obj;
}

/* checks if a quest "quest" already exists on character */
/* returns pointer to quest if so, null otherwise */
QUEST_DATA* has_quest(CHAR_DATA* ch, char* quest){
  QUEST_DATA* q;
  if (IS_NPC(ch)){
    return NULL;
  }
/* check the quest list */
  for (q = ch->pcdata->quests; q; q = q->next){
    if (!str_cmp(q->quest, quest))
      return q;
  }
  return NULL;
}

 /* updates current questor if neccessary */
void questor_update( CHAR_DATA* ch ){
  CHAR_DATA* vch = NULL;

  if (IS_NPC(ch) || IS_IMMORTAL(ch) || IS_GAME(ch, GAME_NOQUESTOR))
    return;
  //record max number of quests
  if (ch->pcdata->max_quests > mud_data.max_quest ){
    mud_data.max_quest = UMAX(1, ch->pcdata->max_quests);
    mud_data.last_questor = mud_data.current_time;
  }
  if (!str_cmp(mud_data.questor, ch->name))
    return;

  if (!IS_NULLSTR(mud_data.questor))
    free_string(mud_data.questor);
  mud_data.questor = str_dup( ch->name );

  for (vch = player_list; vch; vch = vch->next_player ){
    if (vch->desc)
      act_new("Let it be known that $t has been granted the title of `#Master Questor`` of A-Mirlan-!",
	      vch, ch->name, ch, TO_CHAR, POS_DEAD);
  }
}


/* adds the quest string "quest" onto the list of quests completed	*/
/* if fSave == QUEST_PERM/QUEST_INVIS the quest is saved when char	*/
/*  is saved		*/

QUEST_DATA* add_quest(CHAR_DATA* ch, char* quest, int fSave){
  QUEST_DATA* newq, *q, *bufq;

  if (IS_NPC(ch)){
    return NULL;
  }

/* check if quest exists already */
  if ( (bufq = has_quest(ch, quest)) != NULL){
    /* check to incremeant quest total */
    if (bufq->save != fSave)
      ch->pcdata->max_quests = ch->pcdata->max_quests + (fSave == QUEST_PERM ? 1 : -1);
    bufq->save = fSave;
    return bufq;
  }
/* if it doesnt exist we create it */
/* for cosmetic reasons, we do it at the END of the list */
  newq = new_quest();
  newq->save = fSave;
  newq->quest = str_dup(quest);

  q = ch->pcdata->quests;
  if (q == NULL){
    newq->next = NULL;
    ch->pcdata->quests = newq;
  }
  else{
    while (q->next){
      q = q->next;
    }
    newq->next = NULL;
    q->next = newq;
  }
  /* update total quests */
  if (fSave == QUEST_PERM)
    ch->pcdata->max_quests++;
  return newq;
}

/* removes the quest "quest" from list of quests char has */
/* returns FALSE if not found				  */
bool del_quest(CHAR_DATA* ch, char* quest){
  QUEST_DATA* q, *qold = NULL;
  int i = 0;
  int order = 0;

  if (IS_NPC(ch)){
    return FALSE;
  }

  if ( ch->pcdata->quests == NULL)
    return FALSE;
/* check for how we are removign this quest */
  if (is_number(quest))
    order = atoi(quest);

/* find the node to be deleting while keeping the perceding node */
  qold = NULL;
  q = ch->pcdata->quests;
  while(q != NULL){
    ++i;
    if (i == order || !str_cmp(q->quest, quest)){
      if (q->save == QUEST_PERM)
	ch->pcdata->max_quests--;
/* special case for top node */
      if (qold == NULL){
	ch->pcdata->quests = q->next;
	free_quest (q);
      }
      else{
	qold->next = q->next;
	free_quest (q);
      }
      return TRUE;
    }
    qold = q;
    q = q->next;
  }
  return FALSE;
}

int GET_AC(CHAR_DATA* ch, int type){
  int ac = ch->armor[type];

  if (IS_AWAKE(ch))
    ac += dex_app[get_curr_stat(ch,STAT_DEX)].defensive;
  if (get_skill(ch, gsn_basic_armor) > 1)
    ac = 115 * ac / 100;
  if (get_skill(ch, gsn_adv_armor) > 1)
    ac = 110 * ac / 100;
  return ac;
}

int GET_AC2(CHAR_DATA* ch, int type){
  int ac = ch->armor[type];

  if (ac > 125)
    ac = ((ac - 125) / 2) + 125;
  if (IS_AWAKE(ch))
    ac += dex_app[get_curr_stat(ch,STAT_DEX)].defensive;
  if (get_skill(ch, gsn_basic_armor) > 1)
    ac = 115 * ac / 100;
  if (get_skill(ch, gsn_adv_armor) > 1)
    ac = 110 * ac / 100;
  return ac;
}

bool is_armor_enhanced(CHAR_DATA* ch){
  OBJ_DATA* obj = get_eq_char(ch, WEAR_BODY);

  if (obj == NULL)
    return ENHANCE_NONE;
  else if (get_skill(ch, gsn_armor_enhance) < 2)
    return ENHANCE_NONE;
  else if (obj->pIndexData->vnum == OBJ_VNUM_JUGGERNAUT)
    return ENHANCE_JUGGER;
  else if (is_affected_obj(obj, gsn_armor_enhance))
    return ENHANCE_BASIC;
  else
    return ENHANCE_NONE;
}

/* returns the ammo string based on condition */
char* get_proj_number( int number ){
  static char out[7];

  if (number > 999)
    number = 999;
  else if (number < 0)
    number = 0;

  sprintf( out, "(%d) ", number );
  return out;
}

/* returns string condition based on percent */
/* fParen = TRUE means add brackets */
char* get_condition(int condition, bool fParen){
  static char buf[MIL];
  char* b = buf;
  char* i;

  if (fParen){
    if (condition > 90)	        i = "(perf) ";
    else if (condition > 75)    i = "(good) ";
    else if (condition > 50)    i = "(avrg) ";
    else if (condition > 25)    i = "(worn) ";
    else if (condition > 10)    i = "(dmgd) ";
    else if (condition >  0)    i = "(brkn) ";
    else			i = "(ruin) ";
  }
  else{
    if (condition > 90)	      i = "perfect ";
    else if (condition > 75)    i = "good ";
    else if (condition > 50)    i = "average ";
    else if (condition > 25)    i = "worn ";
    else if (condition > 10)    i = "damaged ";
    else if (condition >  0)    i = "broken ";
    else			i = "ruined ";
  }
  /* character pump */
  while ( (*b = *i) != '\0'){
    b++,i++;
  };
  return buf;
}

/* adjusts condition changed based on object */
/* so unique and more resilent items deteriorate less */
int adjust_condition(OBJ_DATA* obj, int val){
  int mod = 0;
  if (val == 0)
    return val;
  if (IS_OBJ_STAT(obj,ITEM_BURN_PROOF))
    return val < 0 ? -1 : val;
  if (CAN_WEAR(obj, ITEM_RARE))
    mod = val * UMIN(0, obj->cost / 3 - 5) / 10;
  else if (CAN_WEAR(obj, ITEM_UNIQUE))
    mod = val * UMAX(0, obj->cost - 10) / 10;
  if (IS_OBJ_STAT(obj,ITEM_BLESS) || IS_OBJ_STAT(obj,ITEM_EVIL))
    mod += 2;
  if (IS_OBJ_STAT(obj,ITEM_MAGIC))
    mod += 1;
  if (IS_OBJ_STAT(obj,ITEM_NONMETAL))
    mod += 3 * val / 2;
  return URANGE(-obj->condition, val + mod, 100 - obj->condition);
}

/* returns maximum amount of psalms a person may have */
int max_psalm(CHAR_DATA* ch ){
  int max = 0;
  if (IS_IMMORTAL(ch) && get_trust(ch) >= IMPLEMENTOR)
    return MAX_PSALM;
  else
    max = 1 + UMAX(0, (ch->level - 15) / 5);
  if (get_skill(ch, gsn_psalm_master) > 1)
    max  = 3 * max / 2;

  return URANGE(0, max - ch->pcdata->psalm_pen, MAX_PSALM);
}

/* returns proper spot in psal_table by name*/
int psalm_lookup(const char* name){
  int ps;
  for (ps= 0; psalm_table[ps].name != NULL && ps < MAX_PSALM; ps++){
    if (LOWER(name[0]) == LOWER(psalm_table[ps].name[0])
	&& !str_prefix(name, psalm_table[ps].name))
      return ps;
  }
  return 0;
}

/* returns proper spot in psal_table by sn*/
int psalm_lookup_sn(int sn){
  int ps;
  for (ps= 0; psalm_table[ps].name != NULL && ps < MAX_PSALM; ps++){
    if (psalm_table[ps].gsn != NULL && *psalm_table[ps].gsn == sn)
      return ps;
  }
  return 0;
}

/* returns proper spot in psal_table by check */
int psalm_check_lookup_sn(int sn){
  int ps;
  for (ps= 0; psalm_table[ps].name != NULL && ps < MAX_PSALM; ps++){
    if (psalm_table[ps].check != NULL && *psalm_table[ps].check == sn)
      return ps;
  }
  return 0;
}

/* returns spot in the psalm_table if the character has this psalm */
/* integer position, or name can be given */
int get_psalm(CHAR_DATA* ch, const char* name){
  int pos = atoi( name );
  int psalm = 0;

  if (IS_NPC(ch))
    return 0;

  if (pos > 0){
    for (psalm = 0; psalm < MAX_PSALM; psalm ++){
      if (ch->pcdata->psalms[psalm] == pos
	  && pos <= max_psalm(ch))
	return psalm;
    }
    return 0;
  }
  else{
    psalm = psalm_lookup( name );
    if (ch->pcdata->psalms[psalm] > max_psalm(ch))
      return 0;
    else
      return ch->pcdata->psalms[psalm] > 0 ? psalm : 0;
  }
}
/* checks if character has a psalm */
bool has_psalm(CHAR_DATA* ch, int sn){
  int i = psalm_lookup_sn( sn );
  if (IS_NPC(ch) || i > max_psalm(ch))
    return FALSE;
  return (ch->pcdata->psalms[i] > 0);
}

/* adds a psalm */
void add_psalm( CHAR_DATA* ch, int sn ){
  int j = 0;
  int or = 0;
  if (sn == 0){
    bug("add_psalm: sn passed not a psalm", sn);
    return;
  }
  /* get the order of this entry */
  for (j = 0; j < MAX_PSALM; j++){
    if (ch->pcdata->psalms[j] > or)
      or = ch->pcdata->psalms[j];
  }
  or ++;
  if (or > max_psalm(ch) && sn != psalm_lookup("baptism")){
    send_to_char("You are incapable of memorizing more psalms.\n\r", ch);
    return;
  }
  /* add the entry */
  if (  ch->pcdata->psalms[sn]  < 1)
    ch->pcdata->psalms[sn] = or;
}

/* removes a psalm */
void remove_psalm( CHAR_DATA* ch, int sn ){
  int or = 0;
  int j = 0;
  if (sn == 0){
    bug("add_psalm: sn passed not a psalm", sn);
    return;
  }
  /* get order of this entry */
  or = ch->pcdata->psalms[sn];
  /* rem the entry */
  ch->pcdata->psalms[sn] = 0;
  if (or < 1)
    return;
  /* lower every psalm higher then order by one now */
  for (j = 0; j < MAX_PSALM; j++){
    if (ch->pcdata->psalms[j] > or)
      ch->pcdata->psalms[j]--;
  }
}

char* get_crus_title(CHAR_DATA* ch, int level){
  static char buf[MIL];
/* INIT */
  buf[0] = '\0';

  if (IS_NPC(ch))
    return buf;

/* less then level 50 titles */
  if (level < 50)
    sprintf( buf, "the %s", title_table [ch->class] [level]
	     [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
  else if (get_skill(ch, gsn_behead) > 1)
    sprintf( buf, "the %s", title_table [ch->class] [51]
	     [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
  else if (get_skill(ch, skill_lookup("pommel smash")) > 1)
    sprintf( buf, "the %s", title_table [ch->class] [52]
	     [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
  else if (get_skill(ch, gsn_apierce) > 1)
    sprintf( buf, "the %s", title_table [ch->class] [53]
	     [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
  else if (get_skill(ch, gsn_high_sight) > 1)
    sprintf( buf, "the %s", title_table [ch->class] [54]
	     [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
  else if (get_skill(ch, skill_lookup("pious")) > 1)
    sprintf( buf, "the %s", title_table [ch->class] [55]
	     [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
  else if (get_skill(ch, gsn_fourth_attack) > 1)
    sprintf( buf, "the %s", title_table [ch->class] [56]
	     [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
  else
    sprintf( buf, "the %s", title_table [ch->class] [ch->level]
	     [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
  return buf;
}

char* show_crusade(AFFECT_DATA* paf){
  static char choice [MIL];
  strcpy(choice, "none");

  if ( paf->type != gen_crusade)
    return choice;

/* compose the name of the party to be crusaded against */
  if (paf->level == CRUSADE_RACE){
    if (paf->modifier == race_lookup("fire")
	|| paf->modifier == race_lookup("storm")
	|| paf->modifier == race_lookup("stone"))
      sprintf(choice, "%s Giants", race_table[paf->modifier].name);
    else if (paf->modifier == -1)
      sprintf(choice, "avatars");
    else if (paf->modifier == race_lookup("lich"))
      sprintf(choice, "%ses", race_table[paf->modifier].name);
    else
      sprintf(choice, "%ss", race_table[paf->modifier].name);
  }
  else if (paf->level == CRUSADE_CLASS)
    sprintf(choice, "%ss", class_table[paf->modifier].name);
  else if (paf->level == CRUSADE_CABAL){
    CABAL_DATA* pCab = get_cabal_vnum( paf->modifier );
    if (pCab){
      sprintf(choice, "%s", pCab->name );
    }
    else
      sprintf(choice, "%s", "heathens");
  }
  else
    sprintf(choice, "%s", "heathens");
  choice[0] = UPPER(choice[0]);
  return choice;
}

/* returns skill maximum for the pc */
int get_skillmax(CHAR_DATA* ch, int sn){
  int max = 100;
  int i = 0;

  if (IS_NPC(ch))
    return max;

  /* get the maximum learning level for this skill */
  if ( (i = weapon_sn_lookup( sn )) != -1){
    if (get_skill(ch, *weapon_table[i].gsn_mastery) > 1)
      max += 5;
    if (get_skill(ch, *weapon_table[i].gsn_expert) > 1)
      max += 2;
  }
  /* racial innate skills */
  for (i = 0; i < 5; i++){
    if (IS_NULLSTR(pc_race_table[ch->race].skills[i]))
      break;
    if (skill_lookup(pc_race_table[ch->race].skills[i]) == sn){
      max += 3;
      break;
    }
  }
/* specific skills */
  if (sn == gsn_shield_block && get_skill(ch, gsn_shield_expert) > 1)
    max += 3;
  return max;
}

/* checks if GN bit is set on given paf */
bool IS_GNBIT( int sn, int bit ){
  if (IS_GEN(sn) ){
    if ((effect_table[GN_POS(sn)].flags2 & bit) == bit)
      return TRUE;
    else
      return FALSE;
  }
  else{
    if ((skill_table[sn].flags & bit) == bit )
      return TRUE;
    else
      return FALSE;
  }
}

AFFECT_DATA* get_random_affect( AFFECT_DATA* affected, int gn_bit ){
  const int MAX_SPELLS = 32;
  AFFECT_DATA* spells[MAX_SPELLS], *paf;
  int spell_count = 0;

  /* store the spells on character int the array */
  for (paf = affected; paf; paf = paf->next){
    if (paf->next && paf->type == paf->next->type)
      continue;
    else if (!IS_GNBIT(paf->type, gn_bit))
      continue;
    else if (spell_count >= MAX_SPELLS)
      continue;
    spells[spell_count++] = paf;
  }

  if (spell_count < 1)
    return NULL;
  else
    return (spells[number_range(0, spell_count - 1)]);
}

/* gets average roll based on:
   number	: number of dice being rolled
   type		: how many sides per dice
   bonus	: bonus amount added onto the roll
*/
int get_dice_avg( int number, int type, int bonus ){
  return (bonus + ((1 + type) * number / 2));
}


/* returns the season based on month/day */
int get_season( int month, int day ){
  int season = (month * MAX_DAYS * MAX_WEEKS + day) / 90;

  if (season < 1)
    return SEASON_SPRING;
  else if (season < 2)
    return SEASON_SUMMER;
  else if (season < 3)
    return SEASON_FALL;
  else
    return SEASON_WINTER;
}
/* Sets the hour/day/month/year ints to values calculated based on
   real_seconds value.  Hours are always based on mud time, rather
   then the real_seconds

   converts real life time, to a mud date, negative year = BC
*/
void rltom_date( long real_time, int* h, int* d, int* m, int* y ){
  //June 1st 2000
  struct tm time_cataclysm = {0, 0, 0, 1, 6, 100, 0, 0, 0};
  const long TIME_CATACLYSM = (time_t) mktime( &time_cataclysm );
  int yer_pc, mon_pc, day_pc, hour_pc;

  //One real life second is 120 FL seconds = 2 minutes
  long minx2_pc = (real_time - TIME_CATACLYSM);

  yer_pc = minx2_pc / (30 * MAX_HOURS * MAX_DAYS * MAX_WEEKS * MAX_MONTHS);
  minx2_pc -= yer_pc * MAX_MONTHS * MAX_WEEKS * MAX_DAYS * MAX_HOURS * 30;
  mon_pc = minx2_pc / (30 * MAX_HOURS * MAX_DAYS * MAX_WEEKS);
  minx2_pc -= mon_pc * MAX_WEEKS * MAX_DAYS * MAX_HOURS * 30;
  day_pc = minx2_pc / (30 * MAX_HOURS);
  minx2_pc -= day_pc * MAX_HOURS * 30;
  hour_pc = minx2_pc / 30;

  //adjust for BC time
  if (mon_pc < 0)
    mon_pc += MAX_MONTHS - 1;
  if (day_pc < 0)
    day_pc += MAX_DAYS * MAX_WEEKS - 1;
  if (hour_pc < 0)
    hour_pc += MAX_HOURS - 1;
  *y = yer_pc;
  *m = mon_pc;
  *d = day_pc;
  *h = hour_pc;

  /* DEBUG
  nlogf( "Min: %ld, Day %d Mon %d Yer %d\n",
	 (real_time - TIME_CATACLYSM) * 2, day_pc, mon_pc, yer_pc );
  */

}
/* returns string showing date based on given hour/day/month/year */
char* mud_date_string( int hour, int Day, int month, int year ){
  static char output[MIL];
  char* suf;
  char* sea;

  int day = 1 + Day;
  int season = get_season( month, Day);

  if      ( day > 4 && day <  20 ) suf = "th";
  else if ( day % 10 ==  1       ) suf = "st";
  else if ( day % 10 ==  2       ) suf = "nd";
  else if ( day % 10 ==  3       ) suf = "rd";
  else                             suf = "th";

  if (season == SEASON_SPRING)		sea = "Spring";
  else if (season == SEASON_SUMMER)	sea = "Summer";
  else if (season == SEASON_FALL)	sea = "Fall";
  else if (season == SEASON_WINTER)	sea = "Winter";
  else					sea = "Unknown";

  sprintf( output, "%d%s, %s the %d%s of %s (%s) %d %s.",
	   hour % 12 == 0 ? 12 : hour % 12,
	   hour < 13 ? "am" : "pm",
	   day_name[(day - 1) % MAX_DAYS],
	   day,
	   suf,
	   month_name[month % MAX_MONTHS],
	   sea,
	   year< 0 ? -year : year,
	   year < 0 ? "BC" : "PC"
	   );
  return output;
}
/* returns a string representing the mud time and date in format of:
   12am, Sunday the 32nd of Life (Winter)
*/
char* mud_date( ){
  return (mud_date_string( mud_data.time_info.hour,
			   mud_data.time_info.day,
			   mud_data.time_info.month,
			   mud_data.time_info.year ));
}

int get_hitroll( CHAR_DATA* ch ){
  int hit = ch->hitroll + str_app[get_curr_stat(ch,STAT_STR)].tohit;
  if (IS_PERK(ch, PERK_ATHLETIC))
    hit += 1;

  return hit;
}


int get_damroll( CHAR_DATA* ch ){
  int dam = ch->damroll + str_app[get_curr_stat(ch,STAT_STR)].todam;
  if (IS_PERK(ch, PERK_ATHLETIC))
    dam += 1;

  return dam;
}

/* calculates how many use repetitions each % skill requires */
int get_skill_reps( int skill_level ){
  int level = UMIN(20, skill_level / 5);
  return (progress_table[level] * 10);
}

/* returns rating for the weapon on scale -5 to 5 in a given category */
/* H2H is considered sword */
int get_weapon_rating( int weapon_type, int rating_category ){
  int rate;

  switch( rating_category ){
  case WEAPON_ATT:	rate = weapon_rating[weapon_type].attack;	break;
  case WEAPON_DEF:	rate = weapon_rating[weapon_type].defense;	break;
  case WEAPON_WITHDIS:	rate = weapon_rating[weapon_type].with_dis;	break;
  case WEAPON_TOBEDIS:	rate = weapon_rating[weapon_type].tobe_dis;	break;
  case WEAPON_WITHSDIS:	rate = weapon_rating[weapon_type].with_sdis;	break;
  default:
    rate = 0;
  }

  return rate;
}

//returns given object's weapon rating
int get_weapon_obj_rating( OBJ_DATA* obj, int category ){
  AFFECT_DATA* paf;
  int type = WEAPON_EXOTIC;
  int rating = 0;

  //h2h case
  if (obj == NULL)
    type = WEAPON_SWORD;
  else if (obj->item_type != ITEM_WEAPON)
    return 0;
  else
    type = obj->value[0];

  if ( obj
       && (paf = affect_find(obj->affected, gsn_puppet_master)) != NULL
       && obj->carried_by
       && has_twohanded(obj->carried_by)){
    type = paf->modifier;
  }

  rating = get_weapon_rating( type, category );

  //check for bless/evil for good/evil chars
  if (obj && obj->carried_by){
    int mod = 0;
    if (IS_GOOD(obj->carried_by) && IS_OBJ_STAT(obj, ITEM_BLESS))
      mod += 2;
    else if (IS_EVIL(obj->carried_by) && IS_OBJ_STAT(obj, ITEM_EVIL))
      mod += 2;

    if (category == WEAPON_TOBEDIS)
      mod *= -1;

    rating += mod;
  }

  return (rating);
}



//sets a path on a mob from source to dest, returns distance or -1 for fail
//if pCab is specificed path is made through this cabal's lands only
int set_path( CHAR_DATA* mob, ROOM_INDEX_DATA* src, ROOM_INDEX_DATA* dest, int max_dist, CABAL_DATA* pCab){
  PATH_QUEUE* path;
  int dist = 0;

  if ( (path = generate_path( src, dest, max_dist, TRUE, &dist, pCab)) == NULL){
    clean_path();
    return -1;
  }
  else
    clean_path();

  mob->spec_path = path;
  return dist;
}

//returns the room the path leads to
ROOM_INDEX_DATA* get_path_dest( PATH_QUEUE* path ){
  PATH_QUEUE* cur = path;

  if (cur == NULL)
    return NULL;

  while (cur->next)
    cur = cur->next;

  return cur->room;
}

//returns room vnum a mob's path leads to
int get_path_vnum( CHAR_DATA* mob){
  ROOM_INDEX_DATA* room;

  if (mob->spec_path == NULL)
    return 0;
  else
    room = get_path_dest( mob->spec_path );

  return (room ? room->vnum : 0);
}

//returns the allowed number of object
int get_maxcount( OBJ_INDEX_DATA* pIndex){
  int max = 0;

  if (IS_SET(pIndex->wear_flags, ITEM_RARE)){
    max = mud_data.pfiles * pIndex->cost / OBJCOUNT_PFILE_NUMBER;
  }
  else if (IS_SET(pIndex->wear_flags, ITEM_UNIQUE)){
    max = pIndex->cost;
  }
  else
    max = 32767;

  return ( UMAX(max, 1));
}

//checks if an object can be spawned
bool can_spawn( OBJ_INDEX_DATA* pIndex ){
  return( pIndex->count < get_maxcount( pIndex ));
}

//check if an object can exist (not over a count)
bool can_exist( OBJ_INDEX_DATA* pIndex ){
  return (pIndex->count <= get_maxcount( pIndex ));
}


/* returns weapon position based on character's battle facing, and primary/secondary selection */
int getBmWearLoc( CHAR_DATA* ch, bool fPrim){
  int wear_loc = fPrim ?  WEAR_WIELD : WEAR_SECONDARY;
  int pos;

  if (IS_NPC(ch) || ch->class != gcn_blademaster || !monk_good(ch, WEAR_FEET))
    return (wear_loc);
  else
    pos = ch->pcdata->bat_pos;

  /*
    1) If LEFT and LEFT handed, check primary
    2) If LEFT and RIGHT handed, check secondary
    3) If RIGHT and LEFT handed, check secondary
    4) If RIGHT and RIGHT handed, check primary
    5) If MIDDLE and LEFT handed, check primary
    6) If MIDDLE and RIGHT handed, check secondary
    7) if no weapon in the location, return the other location
  */
  if (pos == BATPOS_LEFT && ch->pcdata->lefthanded)		wear_loc = WEAR_WIELD;
  else if (pos == BATPOS_LEFT && !ch->pcdata->lefthanded)	wear_loc = WEAR_SECONDARY;
  else if (pos == BATPOS_RIGHT && ch->pcdata->lefthanded)	wear_loc = WEAR_SECONDARY;
  else if (pos == BATPOS_RIGHT && !ch->pcdata->lefthanded)	wear_loc = WEAR_WIELD;
  else if (pos == BATPOS_MIDDLE)				wear_loc = fPrim ? WEAR_WIELD : WEAR_SECONDARY;


  if (get_eq_char(ch, wear_loc) == NULL){
    wear_loc = wear_loc == WEAR_WIELD ? WEAR_SECONDARY : WEAR_WIELD;
  }
  return (wear_loc);
}

/* returns the equivalent primary or secondary weapon for blademasters
   based on set prefered battle position
*/
OBJ_DATA* getBmWep( CHAR_DATA* ch, bool fPrim ){
  int wear_loc = getBmWearLoc( ch, fPrim);
  return (get_eq_char(ch, wear_loc));
}

//checks if victim can use counter on ch
bool counter_check( CHAR_DATA* ch, CHAR_DATA* victim, int skill, int ini_dam, int dam_type, int sn){
  int cost;
  int dam = ini_dam;
  if (ch == NULL || victim == NULL)
     return FALSE;
  else if (IS_IMMORTAL(ch))
    return FALSE;
  else if (victim->fighting != NULL)
    return FALSE;
  else if (IS_AFFECTED(victim, AFF_SLOW) && !IS_AFFECTED(victim, AFF_HASTE))
    return FALSE;
  else if (IS_NPC(ch) && ch->hit < ch->max_hit / 2)
    return FALSE;
  else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD)
    return FALSE;
  else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_HOUND)
    return FALSE;
  else if (!can_see(victim, ch) && !is_affected(ch, gsn_battlefocus))
    return FALSE;
  if (number_percent() > skill){
    check_improve(victim,gsn_counter,FALSE, 10);
    return FALSE;
  }
  else if (victim->position < POS_FIGHTING && victim->position > POS_STUNNED){
    const int gsn_vigil = skill_lookup("vigil");
    //blademaster vigil
    if (!IS_AFFECTED(victim, AFF_SLEEP) && number_percent() < get_skill(victim, gsn_vigil)){
      send_to_char("You sense an impeding attack!\n\r", victim);
      act("$n springs up suddenly $s weapons flashing in a deadly arc!", victim, NULL, NULL, TO_ROOM);
      check_improve(victim, gsn_vigil, TRUE, 30);
      sn = gsn_vigil;
      dam = 3 * dam / 2;
      do_wake(victim, "");
    }
    else
      return FALSE;
  }

  check_improve(victim,gsn_counter,TRUE,5);

  //get mana cost
  if (victim->class == gcn_warrior)
    cost = 5;
  else
    cost = 10;

  if (victim->mana < cost)
    return FALSE;
  else
    victim->mana -= cost;

  act("You turn $n's attack against $m.",ch,NULL,victim,TO_VICT);
  act("$N turns your attack against you.",ch,NULL,victim,TO_CHAR);
  act("$N turns $n's attack against $m.",ch,NULL,victim,TO_NOTVICT);

  set_fighting( victim, ch );
  damage(victim, ch, 3 * dam / 2, sn, dam_type,  TRUE );
  if (victim->fighting != ch)
    return TRUE;

  //second counter
  if (get_eq_char(victim, WEAR_SECONDARY) && get_eq_char(victim, WEAR_WIELD) ){
    int gsn_twin_counter = skill_lookup("twin counter");
    if (number_percent() > get_skill(victim, gsn_twin_counter)){
      check_improve(victim,gsn_twin_counter,FALSE,10);
      return TRUE;
    }
    else if (monk_good(victim, WEAR_HANDS)){
      check_improve(victim,gsn_twin_counter,TRUE,10);
      damage(victim, ch, 6 * ini_dam / 5, gsn_twin_counter, dam_type,  TRUE );
    }
  }
  return TRUE;
}

//returns max anatomy
int get_max_anat( CHAR_DATA* ch, int anat ){
  int max = 100;

  if (race_table[ch->race].pc_race
      && anat == anatomy_lookup(pc_race_table[ch->race].anatomy))
    max += 10;

  //this is hardwired since we never use integers for anatomies anyway
  if (anat == 0){
    if (get_skill(ch, gsn_mob_expert))
      max += 10;
    if (get_skill(ch, gsn_mob_master))
      max += 20;
  }
  else if (anat == 1){
    if (get_skill(ch, gsn_human_expert))
      max += 10;
    if (get_skill(ch, gsn_human_master))
      max += 20;
  }
  else if (anat == 2){
    if (get_skill(ch, gsn_elf_expert))
      max += 10;
    if (get_skill(ch, gsn_elf_master))
      max += 20;
  }
  else if (anat == 3){
    if (get_skill(ch, gsn_dwarf_expert))
      max += 10;
    if (get_skill(ch, gsn_dwarf_master))
      max += 20;
  }
  else if (anat == 4){
    if (get_skill(ch, gsn_demihuman_expert))
      max += 10;
    if (get_skill(ch, gsn_demihuman_master))
      max += 20;
  }
  else if (anat == 5){
    if (get_skill(ch, gsn_giant_expert))
      max += 10;
    if (get_skill(ch, gsn_giant_master))
      max += 20;
  }
  else if (anat == 6){
    if (get_skill(ch, gsn_beast_expert))
      max += 10;
    if (get_skill(ch, gsn_beast_master))
      max += 20;
  }
  else if (anat == 7){
    if (get_skill(ch, gsn_flying_expert))
      max += 10;
    if (get_skill(ch, gsn_flying_master))
      max += 20;
  }
  else if (anat == 8){
    if (get_skill(ch, gsn_unique_expert))
      max += 10;
    if (get_skill(ch, gsn_unique_master))
      max += 20;
  }

  return (max);
}
