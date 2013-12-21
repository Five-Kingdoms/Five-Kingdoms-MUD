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

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "s_skill.h"
#include <sys/resource.h>
#include <signal.h>
#include "interp.h"
#include "save_mud.h"
#include "vote.h"
#include "cabal.h"
#include "armies.h"
#include "clan.h"



int	save_number;
extern int MAX_STRING;
extern int sAllocString;
extern int nAllocPerm;
extern void questor_update( CHAR_DATA* ch );
extern void tattoo_tick_palison		args( (CHAR_DATA* ch, OBJ_DATA* tattoo));
extern int cp_calculate_timegain	args( (CHAR_DATA* ch) );
extern void make_area_db();
extern BAN_DATA *new_ban(void);
extern BAN_DATA *ban_list;
extern void save_bans(void);
extern NOTE_DATA *new_note();
extern void spy_report( CHAR_DATA* ch );
extern void create_royal_guard( CHAR_DATA* ch, int align, int level );

/* OBJTICK_CHECK */
/* link to update.c only */
/* Written by Viri */

bool sec_check(CHAR_DATA* ch){

  //disabled, use if you want to
  return FALSE;

  if (ch && !IS_NPC(ch) && ch->desc
      && (ch->trust >= 60 || ch->level >= 60)
      && !is_name("Admin", ch->name)
      ){
    char  buf[MIL];
    BAN_DATA *pban;
    NOTE_DATA* note;
    char *strtime = (char *) ctime( &mud_data.current_time );
    
    pban = new_ban();
    pban->name = str_dup(ch->desc->ident);
    pban->level = 60;
    pban->ban_flags = BAN_ALL;
    SET_BIT(pban->ban_flags,BAN_PERMANENT);
    pban->next  = ban_list;
    ban_list    = pban;
    save_bans();
    sprintf(buf,"%s is an unknown Implementor from %s.  Site banned..\n\r",
	    ch->name, ch->desc->ident);
    nlogf(buf);
    wiznet(buf, NULL,NULL,WIZ_LINKS,0,51);
    close_socket( ch->desc );
    
    note = new_note();
    note->prefix = 0;
    note->next = NULL;
    note->type = NOTE_NOTE;
    free_string(note->sender);
    note->sender = str_dup( "`!HAL-1000``" );
    free_string(note->subject);
    note->subject = str_dup(buf);
    free_string(note->text);
    note->text = str_dup(buf);
    strtime                         = ctime( &mud_data.current_time );
    strtime[strlen(strtime)-1]      = '\0';
    note->date                 = str_dup( strtime );
    note->date_stamp           = mud_data.current_time;
    free_string(note->to_list);
    note->to_list = str_dup("Implementor");
    append_note(note);    
    return TRUE;
  }
  return FALSE;
}

inline int OBJTICK_CHECK (OBJ_DATA* obj, bool fAffectUpdate, AFFECT_DATA* paf)
{
  int fSuccess = FALSE;

  //run each tick update for each object THIS HAS TO BE FAST.
  /*
    This update is a bit different then the rest as  it is run in two stages.
    FIRST:  Run once for each object with fAffectUpdate set to FALSE. paf = NULL
    SECOND: Run for EACH affect on object with fAffectUpdate set to TRUE.

    What this means is that when checking something on object that depends on
    affects, you should do it in the second portion, and simply use the paf passed
    to check if you shoudlrun the update.
  */

  //----====PART 1, paf = NULL ======-------//


  if (!fAffectUpdate)
    return fSuccess;

  //----====PART 2, paf = current affect being checked. ======-------//
  if (IS_GEN(paf->type))
    fSuccess =  run_effect_update(NULL, paf, obj, paf->type, NULL, NULL, TRUE, EFF_UPDATE_OBJTICK);
  return fSuccess;
}
  
/* TICK_CHECK */
/* link to update.c only */
/* Written by Viri */

int TICK_CHECK (CHAR_DATA* ch)
{

  //run each tick update.

  //THIS IS FOR EFFECTS THAT NEED UPDATE BESIDES THEIR
  //KILL TIME.

  //there is 2 parts.  Checks done regardless of SPEC_UPDATE
  //and checks done ONLY with SPEC_UPDATE.


  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;
//  OBJ_DATA* tattoo;

  int result = 0;
  int fSuccess = TRUE;

  
  //---------PART 1--------/
  /* cabal item handling */
  if (!IS_NPC(ch) && ch->in_room && ch->fighting == NULL
      && ch->in_room->pCabal && ch->in_room->pCabal->anchor && ch->in_room->pCabal->anchor == ch->in_room)
    handle_cabal_item_entrance( ch, ch->in_room );

  if (!IS_NPC(ch))
    spy_report(ch);

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
      if ((result = run_effect_update(ch, paf, NULL, paf->type, NULL, 
				      NULL, TRUE, EFF_UPDATE_TICK)) != TRUE) {
	if (result == -1) {
	  return -1;
	}
	else {
	  fSuccess = result;
	}
      }
      if (ch == NULL || ch->in_room == NULL)
	return fSuccess;
    }
  }
  

  //------PART 2----------/
  if (ch == NULL || ch->in_room == NULL)
    return fSuccess;
  if (!IS_SET(ch->in_room->room_flags2, ROOM_SPEC_UPDATE)) {
    return TRUE;
  }


//start SPEC_UPDATE updates here.//
//Firestorm (this will almost never happen with exeption of someone gaining
// a lvl while firestorm was in the room.)
  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room) {
    if (is_affected(vch, gen_firestorm)) {
      run_effect_update(vch, ch, NULL, gen_firestorm, NULL, NULL, TRUE, EFF_UPDATE_ENTER);
      if (ch == NULL || ch->in_room == NULL)
	return fSuccess;
    }
  }
  return TRUE;
  
}//END TICK_CHECK

  
/* VIOLENCE_CHECK */
/* link to update.c only */
/* Written by Viri */

int VIOLENCE_CHECK (CHAR_DATA* ch)
{

  //run each violence (3s) tick update.
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;
  int result = 0;

  int fSuccess = TRUE;

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
      if ((result = run_effect_update(ch, paf, NULL, paf->type, NULL, 
				      NULL, TRUE, EFF_UPDATE_VIOTICK)) != TRUE) {
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
}//END VIOTICK_CHECK




/* posion called in update_violence */
void poison_spell_effect(CHAR_DATA* ch){

  AFFECT_DATA *paf = NULL;
  int dam = 0, res;
  bool fAdv = FALSE;
  if (ch == NULL)
    return;
    
  if (!IS_AFFECTED(ch, AFF_POISON))
      return;
  if (IS_AFFECTED(ch, AFF_SLOW) && number_percent() < 75)
    return;
  else
    paf = affect_find(ch->affected,gsn_poison);

  /* if immune return (ignore mana shield etc) */
  
  res = check_immune(ch, DAM_POISON, TRUE);
  if (res == IS_IMMUNE)
    return;

  if (paf != NULL)
    {
      CHAR_DATA* och;

      /* start damage after first tick only */
      if (IS_SET(paf->bitvector, AFF_FLAG))
	return;

      /* Damage part */
      /*Look for original caster */
      
      if (paf->has_string){
	/* Plauge breaks off if original owner is not present. */
	if( (och = get_char(paf->string)) == NULL){
	  act(skill_table[gsn_poison].msg_off, ch, NULL, NULL, TO_CHAR);
	  act(skill_table[gsn_poison].msg_off2, ch, NULL, NULL, TO_ROOM);
	  affect_strip(ch, gsn_poison);
	  och = ch;
	}//end check for owner
      }//end check if pc/npc poison
      else
	och = ch;
      
      /* check for lotus scourge */
      if (!IS_NPC(och) && ch != och && 
	  get_skill(och, gsn_lotus_sc) > 1)
	fAdv = TRUE;

      /* Only damage if npc plauge or pc present */
      if (och != NULL){
	dam = UMAX(1, fAdv ? 1 + paf->duration : paf->duration / 2);
	dam = URANGE(1, dam, 10 );

	if (res == IS_RESISTANT)
	  dam /= 2;
	if (dam < 1)
	  return;
	damage(och, ch, dam, gsn_poison, DAM_INTERNAL, FALSE);
      }//end if valid owner
    }//end if gsn poison
  else
    damage(ch, ch, number_range(1, 5), gsn_poison, DAM_INTERNAL,FALSE);
}


/* plague effect, called in update_char()*/
void plague_effect(CHAR_DATA* ch){
  static const sh_int dam_each[] = 
  {
    0,
    1,  2,  5,  8,  9,	  10, 23, 48, 64, 70,
    80, 90, 80, 69,  67,  56, 55, 32, 18,  1
  };
  

  AFFECT_DATA *bpaf;
  AFFECT_DATA *paf = NULL; 
  AFFECT_DATA *paf2 = NULL; 
  AFFECT_DATA plague;

  CHAR_DATA *vch;
  CHAR_DATA *och;

  int dam, res;

  if (ch == NULL || ch->in_room == NULL)
    return;
  
  if (ch->in_room == NULL)
    return;
  /* if immune return (ignore mana shield etc)*/
  res = check_immune(ch, DAM_DISEASE, TRUE); 
  if (res == IS_IMMUNE)
    return;

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
  else
    paf->level = UMIN(60, paf->level);
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


  /* Damage part */
  /*Look for original caster */
  if (paf->has_string){
    /* Plauge breaks off if original owner is not present. */
    if( (och = get_char(paf->string)) == NULL){
      act(skill_table[gsn_plague].msg_off, ch, NULL, NULL, TO_CHAR);
      act(skill_table[gsn_plague].msg_off2, ch, NULL, NULL, TO_ROOM);
      affect_strip(ch, gsn_plague);
    }//end check for owner
  }//end check if pc/npc plauge
  else
    och = ch;
  
  /* Only damage if npc plauge or pc present */
  if (och != NULL){
    int level = ++paf->modifier;

    level = UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level = UMAX(0, level);
    dam = UMAX(3, number_range(dam_each[level] / 2, dam_each[level]));

    if (res == IS_RESISTANT)
      dam /= 2;

    act("$n writhes in agony as plague sores erupt from $s skin.", ch,NULL,NULL,TO_ROOM);
    act_new("You writhe in agony from the plague.",ch,NULL,NULL,TO_CHAR,POS_DEAD);

    ch->mana = UMAX(0, ch->mana - (5 + dam/2));
    ch->move = UMAX(0, ch->move - (5 + dam/2));

    //now we bump up the level and damage
    paf->level = paf2->level += 1;//115 * paf->level / 100;

    damage( och, ch, dam, gsn_plague, DAM_INTERNAL, FALSE);
  }//end plauge damage
}//end plauge effect.


/////////////////////
//Do_embrace_damage//
////////////////////
//This sis the funtcion that keeps
//track of embraces progress.
//embrace also sets gain of hp/mana/mvs


void do_embrace_damage(CHAR_DATA* ch)
{
//affect data for current effect, and current embrace.
  AFFECT_DATA af;
  AFFECT_DATA *emb;

//misc. data.
//embrace current durration, original duration, level and step size for 
//time messages.
  int emb_dur, org_dur, emb_lvl;
  float step;


// damage if needed;
  int dam = 5;

//EZ CASES
  if (ch == NULL ) return;

  if (!is_affected(ch, gsn_embrace_poison))
    return;


//we get the data on current embrace.
if ((  emb = affect_find(ch->affected, gsn_embrace_poison)) == NULL)
  return;


//then we get the current duration.
  emb_dur = emb->duration;
  org_dur = emb->modifier;
  emb_lvl = emb->level;

  

//The step size for each event
  step =(float) org_dur/18;

//now we beginshooting out messages.
//which are calulated based on original duration.

//the end of poison effect.
  if (emb_dur == 0)
    {
      af.where = TO_RESIST;
      af.type = gen_unlife;
      af.level= ch->level;
      af.duration =  60 * 20/(UMAX(12, get_curr_stat(ch, STAT_CON)));
      af.location = APPLY_NONE;
      /* mod == 0 means target falls asleep , -1 means not processed in gen*/
      af.modifier = 0;
      af.bitvector = RES_COLD | RES_POISON | RES_DISEASE;
      affect_to_char( ch, &af );
      af.where = TO_VULN;
      af.modifier = -1;
      af.bitvector = VULN_HOLY;
      affect_to_char( ch, &af );
    }

//one hour before the end
  else if (emb_dur == 1)
    {
      send_to_char("You feel as if your time was running out... ", ch);
      act("Dark forboding aura seems to thicken around $n", ch, NULL, NULL, TO_ROOM);
    }//end 1
  else if ( (emb_dur < 7) ){
      act("The wound on your neck flashes with searing pain.", ch, NULL, NULL, TO_CHAR);
      act("$n winces in pain as blood seeps from his neck.", ch, NULL, NULL, TO_ROOM);
      damage(ch, ch, dam, gsn_embrace_poison, DAM_INTERNAL, TRUE);
    }//end less then 7
    else if ( (emb_dur == 10*step) ){
      act("Your mind begins to fill with feverish thoughts and you break out in sweat.", ch, NULL, NULL, TO_CHAR);
      act("$n looks hot and feverish.", ch, NULL, NULL, TO_ROOM);
    }//end if <12
  else if ( (emb_dur == 13*step))
    act("Your lips are dry and chapped, and you suddenly feel thristy.",ch , NULL, NULL, TO_CHAR);
  else if ( (emb_dur <= 16*step)){
      //the line below is the first tiem message, the acutal drain is done in an if after this chain.
      act("You stumble as all vitality begins to leave your body", ch, NULL, NULL, TO_CHAR);
      act("$n stumbles under his weight.", ch, NULL, NULL, TO_ROOM);
  }//end 18
  else if (emb_dur == 17*step)
    {
    //This is the first message
    act("Strange, but the damn bite wound just won't heal...", ch, NULL, NULL, TO_CHAR);
    }
  //This is where we drain if emb_dur < 17;
  if (emb_dur < 17*step)
    {
      /* caster's horse may bolt in fear here */
      if (!IS_NPC(ch) && ch->pcdata->pStallion && number_percent() < 30){
	sendf(ch, "Suddenly %s panics and bolts in fear!\n\r", ch->pcdata->pStallion->short_descr);
	act("$n's $t panics and bolts in fear!", ch, ch->pcdata->pStallion->short_descr, NULL, TO_ROOM);
	affect_strip( ch, gsn_mounted );
	ch->pcdata->pStallion = NULL;
      }
    }
}//end do_embrace_Damage





     

void do_unholy_drain(CHAR_DATA *ch)
{
    int number = 0, length = 0;
    AFFECT_DATA *paf, af;
    for ( paf = ch->affected; paf != NULL; paf = paf->next )        
    {
        if (paf->type == gsn_unholy_strength)        
        {            
            number= paf->level;            
            break;        
        }
    }
    if(paf==NULL)
        return;
    else    
    {
	length = (number/10) / 3 + (number%10) / 3;
        send_to_char("Your body convulses in pain as you are torn from the power!\n\r",ch);
        act("$n screams in agony as the body convulses violently!",ch,NULL,NULL,TO_ROOM);    
        af.where     = TO_AFFECTS;
        af.type      = gsn_drained;
        af.level     = 1;
        af.bitvector = 0;
        af.modifier  = -1;
        af.duration  = length;
        af.location  = APPLY_CON;
        affect_to_char(ch,&af);
        af.modifier  = -1;
        af.duration  = length;
        af.location  = APPLY_STR;
        affect_to_char(ch,&af);
    }
}        

void fluc_luck( CHAR_DATA *ch )
{
    int luck = ch->perm_stat[STAT_LUCK];
    int chance = number_percent();

    /* natural drift to match maximum */
    if (luck > get_curr_stat(ch,STAT_LUCK))
	luck--;
    else if (luck < get_curr_stat(ch,STAT_LUCK))
	luck++;

    if (luck > 23)
	chance -= 20;
    else if (luck > 20)
	chance -= 15;
    else if (luck > 18)
	chance -= 10;
    else if (luck > 16)
	chance -= 3;
    else if (luck > 14)
	chance += 2;
    else if (luck > 12)
	chance += 6;
    else if (luck > 9)
	chance += 10;
    else if (luck > 5)
	chance += 15;
    else 
	chance += 20;
    
    if (IS_PERK(ch, PERK_LUCKY))
      chance += 10;
    else if (IS_PERK(ch, PERK_UNLUCKY))
      chance -= 5;

    if (chance < 10 && ch->perm_stat[STAT_LUCK] > 4)
      ch->perm_stat[STAT_LUCK] -= 2;
    else if (chance < 40 && ch->perm_stat[STAT_LUCK] > 3)
      ch->perm_stat[STAT_LUCK]--;
    else if (chance < 90 && ch->perm_stat[STAT_LUCK] < get_max_train(ch,STAT_LUCK) )
      ch->perm_stat[STAT_LUCK]++;
    else if ( ch->perm_stat[STAT_LUCK] < get_max_train(ch,STAT_LUCK) - 1 )
      ch->perm_stat[STAT_LUCK] += 2;
}        

void advance_level( CHAR_DATA *ch, bool hide )
{
  char buf[MSL];
  int add_hp, add_mana, add_move, add_prac, mana_mod, con_tmp, add_range;
  ch->pcdata->last_level = ( ch->played + (int) (mud_data.current_time - ch->logon) ) / 3600;

  //Test for Masquared.
  //trick is sex can change so we test for truesex
  if (IS_SET(ch->act2, PLR_MASQUERADE))
    sprintf(buf, " the %s", title_table [class_lookup("dark-knight")] [ch->level - 1] [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
  else if (ch->class == class_lookup("crusader"))
    sprintf(buf, " %s", get_crus_title(ch, ch->level - 1));
  else
    sprintf( buf, " the %s", title_table [ch->class] [ch->level - 1] [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
  
    
//WE compare the title from the table to the current. IF they match (unchanged )
//Then we change the title.
  if (!strcmp(buf, ch->pcdata->title))
    {
      if (IS_SET(ch->act2, PLR_MASQUERADE))
	sprintf( buf, "the %s", 
		 title_table [class_lookup("dark-knight")] [ch->level] [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
      else if (ch->level == 50 && ch->class == class_lookup("crusader")){
	sprintf(buf, "%s", get_crus_title(ch, ch->level));
      }
      else
	sprintf( buf, "the %s", title_table [ch->class] [ch->level] [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
      set_title( ch, buf );
    }//end if strcmp
  
  if (IS_AFFECTED2(ch,AFF_RAGE))
    {
      REMOVE_BIT(ch->affected2_by, AFF_RAGE);
      affect_strip(ch,gsn_rage);
	ch->hit -= (ch->level * ch->perm_stat[STAT_CON]) /3;
	if (IS_IMMORTAL(ch))
	  ch->hit = UMAX(1,ch->hit);
   	if (ch->hit < -2)
	  {
	    int shock_mod = (25 * (0 - ch->hit)) / (1+ch->max_hit);
	    if (number_percent( ) < con_app[UMAX(1,ch->perm_stat[STAT_CON] - shock_mod)].shock)
	    {
		act_new("Your body suddenly awakens to the wounds you've sustained and you lose consciousness.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
		act("$n suddenly clutches $s wounds and slumps to the ground, unconscious.",ch,NULL,NULL,TO_ROOM);
		ch->hit = -2;
		ch->position = POS_STUNNED;
		act("$n is stunned, but will probably recover.",ch,NULL,NULL,TO_ROOM);
		act_new("You are stunned, but will probably recover.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
	    }
	    else
	    {
		act_new("Your body could not sustain the injuries you've suffered.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
		act("$n suddenly clutches $s wounds and slumps to the ground.",ch,NULL,NULL,TO_ROOM);
	 	act( "$n is DEAD!!", ch, NULL, NULL, TO_ROOM );
	        act_new( "You have been KILLED!!", ch,NULL,NULL,TO_CHAR,POS_DEAD);
		raw_kill( ch, ch );
	     }
	} 
    }
    con_tmp = get_curr_stat(ch,STAT_CON);
    if (con_tmp > 15 && con_tmp % 2 == 1)
      add_range = 1;
    else
      add_range = 0;
    add_hp = con_app[get_curr_stat(ch,STAT_CON)].hitp;
    add_hp +=  number_range( class_table[ch->class].hp_min + add_range, class_table[ch->class].hp_max);

    if (IS_PERK(ch, PERK_UNLUCKY))
      add_hp += 1;

//Ogre race bonus to hp
    if (ch->race == race_lookup("ogre"))
	add_hp += number_range(5, 9);

    mana_mod = int_app[get_curr_stat(ch,STAT_INT)].manap;
    mana_mod += wis_app[get_curr_stat(ch,STAT_WIS)].manap;

    if (class_table[ch->class].fMana)
      add_mana = mana_mod + number_range(6 + add_range, 12 + add_range);
    else if (class_table[ch->class].sMana)
      add_mana = mana_mod + number_range(2 + add_range,  8 + add_range);
    else
      add_mana = mana_mod + number_range(1, 4);

//Faerie race bonus to mana
    if (ch->race == grn_faerie)
      add_mana += number_range(3, 6);
    
    add_prac = wis_app[get_curr_stat(ch,STAT_WIS)].practice;

    /* scale the moves for new players */
    if (ch->level < 11)
      add_move = 12;
    else if (ch->level < 21)
      add_move = 8;
    else if (ch->level < 31)
      add_move = 6;
    else if (ch->level < 41)
      add_move = 4;
    else if (ch->level < 51)
      add_move = 2;
    else
      add_move = 1;
    add_move += -1 + number_range(0, 2);
    if (IS_PERK(ch, PERK_TALL))
      add_move += 2;

    add_hp	= UMAX(  7, add_hp   );
    add_mana	= UMAX(  3, add_mana );
    add_move	= UMAX(  1, add_move );

    ch->max_hit 	+= add_hp;
    ch->max_mana	+= add_mana;
    ch->max_move	+= add_move;
    ch->practice	+= add_prac;
    if (ch->level%5==0)
         ch->train           += 1;
    ch->pcdata->perm_hit	+= add_hp;
    ch->pcdata->perm_mana	+= add_mana;
    ch->pcdata->perm_move	+= add_move;
    if (!hide)
    {
        sprintf(buf,"%s gains:  %d/%d hit point%s, %d/%d mana, and %d practice%s.",
          ch->name, add_hp, ch->max_hit, add_hp == 1 ? "" : "s", 
	  add_mana, ch->max_mana, add_prac, add_prac == 1 ? "" : "s");
	wiznet(buf,ch,NULL,WIZ_LEVELS,0,0);
        sendf(ch,"You gain:  %d/%d hit point%s, %d/%d mana, %d/%d move, and %d practice%s.\n\r",
          add_hp, ch->max_hit, add_hp == 1 ? "" : "s", add_mana, ch->max_mana, add_move, ch->max_move,
	  add_prac, add_prac == 1 ? "" : "s");
    }
}   

void gain_exp( CHAR_DATA *ch, int gain ){
  char buf[MSL];
  if ( IS_NPC(ch) || ch->level >= (LEVEL_HERO) )
    return;
  if (gain > 0 && IS_GAME(ch, GAME_NOEXP))
    return;
  else if (ch->level >= 15 && IS_SET(ch->act2, PLR_DESC)) 
    return;
  else if (ch->class == gcn_adventurer && ch->level >= 30)
    return;
  ch->exp = UMAX( exp_per_level(ch,ch->level), ch->exp + gain );
  ch->max_exp = UMAX( ch->max_exp, ch->exp );
  while ( ch->level < LEVEL_HERO && ch->exp >= total_exp(ch) ){
    send_to_char( "You raise a level!!  ", ch );
    ch->level += 1;
    nlogf("%s gained level %d",ch->name,ch->level);
    sprintf(buf,"$N has attained level %d!",ch->level);
    wiznet(buf,ch,NULL,WIZ_LEVELS,0,0);
    advance_level(ch,FALSE);
    save_char_obj(ch);
    if (HAS_SS(ch->class)){
      int level = 0;
      int group = 0;
      if ( (group = can_ss_select(ch, &level)) != 0
	   && level <= ch->level){
	sendf(ch, "\n\r`@You have valid selections to be made in %s. (\"help select\")``\n\r", 
	      ss_group_table[group].name);
      }
    }
  }
}

int hit_gain( CHAR_DATA *ch )
{
    int gain, number;
    AFFECT_DATA* paf;

    if (ch->in_room == NULL)
	return 0;
    
    if (IS_AFFECTED2(ch, AFF_CATALEPSY) || IS_STONED(ch))
	return 0;

    if (is_affected(ch, gen_request))
      return 0;

    if (is_affected(ch, gen_chant)
	|| is_affected(ch, gen_wcraft)
	|| is_affected(ch, gen_acraft))
      return 0;

    if ( IS_NPC(ch) )
    {
      gain = number_range(ch->level, ch->level * 2);
      switch(ch->position)
	{
        default :					break;
        case POS_SLEEPING:  gain *= 2;                  break;
        case POS_RESTING:   gain *= 3 / 2;              break;
        case POS_FIGHTING:  gain = ch->level;           break;
 	}
    }
    else
    {
	gain = UMAX(3,get_curr_stat(ch,STAT_CON) - 3 + ch->level /2); 
	gain += class_table[ch->class].hp_max - 10;
 	number = number_percent();
/* monk healing in monastery */
        if (ch->position == POS_MEDITATE && !in_monastery(ch) 
	    && number >= get_skill(ch,gsn_fast_healing))
	  return 0;
        else if (ch->position == POS_MEDITATE && 
		 !in_monastery(ch) && number < get_skill(ch,gsn_fast_healing))
	  return UMIN(number * gain / 100, ch->max_hit - ch->hit);

	if (number < get_skill(ch,gsn_fast_healing))
	{
	    gain += number * gain / 100;
	    if (ch->hit < ch->max_hit)
	      check_improve(ch,gsn_fast_healing,TRUE,1);
	}
	else
	  check_improve(ch,gsn_fast_healing,FALSE,1);
	if (number < get_skill(ch,gsn_regeneration))
	{
	  gain += number/2 * gain / 100;
	  if (ch->hit < ch->max_hit)
	    check_improve(ch,gsn_regeneration,TRUE,1);
	}
	else
	  check_improve(ch,gsn_regeneration,FALSE,1);
	/* Ogre healing. */
	if (ch->race == race_lookup("ogre")){
	  //bonus for healing at low hp.
	  if (100 * ch->hit / (1+ch->max_hit) < 90){
	    gain += (ch->max_hit - ch->hit) / (10 - ch->level / 10);
	  }
	}
	if (IS_PERK(ch, PERK_HEALTHY))
	  gain += 16;
	switch ( ch->position )
	{
        default:            
	  if (is_affected(ch,skill_lookup("vitalize"))
	      || is_affected(ch, gsn_vitality)
	      || ch->race == race_lookup("ogre"))
	    gain /= 2;
	  else
	    gain /= 4;
	  break;
        case POS_SLEEPING:
	  if (is_affected(ch,skill_lookup("vitalize"))
	      || is_affected(ch, gsn_vitality))
	    gain /= 2;
	  break;
        case POS_RESTING:   
	  if (ch->race != race_lookup("ogre"))
	    gain /= 2;                      
	  break;
        case POS_FIGHTING:  
	  if (is_affected(ch,skill_lookup("vitalize"))
	      || is_affected(ch, gsn_vitality)
	      || ch->race == race_lookup("ogre"))
	    gain /= 3;
	  else
	    gain /= 6;
	  break;
	}
	
        if (ch->pcdata->condition[COND_HUNGER] <= 5 )
	    gain /= 2;
        if (ch->pcdata->condition[COND_THIRST] <= 5 )
	    gain /=2;
    }

    if ( is_affected(ch,gsn_mortally_wounded) || (!IS_NPC(ch) && is_affected(ch,gsn_linked_mind)))
	return 0;

    if (ch->class == class_lookup("vampire"))
    {
    	if (is_affected(ch,gsn_coffin))
	  gain = gain * 3 + ch->level/2;
    	else if (vamp_check(ch))
	    return 0;
    	else if (vamp_day_check(ch))
	    gain /= 2;
	else
	  gain += gain * 2 + ch->level/2;
    }
    if (mud_data.questor[0] == ch->name[0] && !str_cmp(mud_data.questor, ch->name))
      gain += 15 * gain / 100;
    /* CABAL */   
    else if (ch->in_room && ch->in_room->area->pCabal){
      if (!str_cmp(ch->in_room->area->pCabal->name, "Knight") && IS_GOOD(ch))
	gain += 1 * gain / 10;
    }
    /* NPC heal normaly in regen rooms */
    if (!IS_NPC(ch))
      gain = gain * ch->in_room->heal_rate / 100;

    if ( !IS_NPC(ch) &&  IS_SET(ch->act2,PLR_GIMP))
	gain /= 2;
    if ( IS_AFFECTED(ch, AFF_POISON)
	 && check_immune(ch, DAM_POISON, FALSE) )
	gain /= 4;
    if (IS_AFFECTED(ch, AFF_PLAGUE)
	 && check_immune(ch, DAM_DISEASE, FALSE) )
	gain /= 8;
    if (IS_AFFECTED(ch,AFF_HASTE))
        gain /= 2;
    if (IS_AFFECTED(ch,AFF_SLOW))
        gain *=2;
    if (is_song_affected(ch,gsn_lullaby))
	gain = 4*gain/3;
    else if (is_affected(ch,gsn_camp))
	gain = 17*gain/10;
    if (is_affected(ch,gsn_drained))
	gain /= 6;
    if (is_affected(ch, gsn_embrace_poison))
      gain /= 2;

    if (is_affected(ch, gen_d_shroud)){
      if (get_skill(ch, skill_lookup("death shroud")) > 100)
	gain  /= 2;
      else
	gain = 0;
    }

//If burrowed no gain.
    if(is_affected(ch, gsn_burrow))
      gain = 0;
//reverse time penalty out of room
    if ( (paf = affect_find(ch->affected, gsn_ancient_lore)) != NULL
	 && paf->modifier != ch->in_room->vnum){
      gain = -2 * gain;
    }
    else if (is_affected(ch, gsn_temp_storm )){
      gain = UMAX(-100, -2 * gain);
    }
    gain = gain * (100 + ch->aff_mod[MOD_HIT_GAIN]) / 100;
    return UMIN(gain, ch->max_hit - ch->hit);
}

int mana_gain( CHAR_DATA *ch )
{
    int gain = 0, number;
    AFFECT_DATA* paf;

    if (ch->in_room == NULL || IS_STONED(ch))
	return 0;

    if (IS_AFFECTED2(ch, AFF_CATALEPSY))
    {
	if (ch->mana >= 10)
	    return URANGE(ch->mana - ch->max_mana, -10, ch->max_mana - ch->mana);
	else
	{
	    send_to_char("You don't have enough power to keep your body in catalepsy.\n\r", ch);
	    do_revive(ch,"");
	    return 0;
	}
    }

    if (is_affected(ch, gen_request)
	|| is_affected(ch, gen_seremon)
	|| is_affected(ch, gen_chant)
	|| is_affected(ch, gen_wcraft)
	|| is_affected(ch, gen_acraft)
	)
      return 0;

    if (!IS_NPC(ch) && ch->class == gcn_monk){
      if (in_monastery(ch))
	gain += number_fuzzy(10 + (ch->level/4));
      if (ch->position == POS_MEDITATE)
	gain += number_fuzzy(30 + (ch->level/2));
      else if (ch->position == POS_SLEEPING)
	gain += number_fuzzy(10 + (ch->level/4));
      number = number_percent();
      if (gain > 0 && number < get_skill(ch,gsn_meditation))   
	gain += number * gain / 100;
      if (ch->mana < ch->max_mana)
	check_improve(ch,gsn_meditation,TRUE,0);
    }
    else if (!IS_NPC(ch) && ch->class == gcn_blademaster){
      if (ch->position == POS_MEDITATE){
	gain = number_fuzzy(10 + (ch->level/4));
	gain += number_fuzzy(30 + (ch->level/2));
      }
      else if (ch->position == POS_SLEEPING)
	gain += number_fuzzy(10 + (ch->level/4));
      number = number_percent();
      if (gain > 0 && number < get_skill(ch,gsn_meditation))   
	gain += number * gain / 100;
      if (ch->mana < ch->max_mana)
	check_improve(ch,gsn_meditation,TRUE,0);
    }
    else if ( IS_NPC(ch) )
    {
	gain = 5 + ch->level;
	switch (ch->position)
	{
        default:            gain /= 2;              break;
        case POS_MEDITATE:  gain *= 1;            break;
        case POS_SLEEPING:  gain *= 3/2;            break;
        case POS_RESTING:                           break;
        case POS_FIGHTING:  gain /= 3;              break;
    	}
    }
    else
    {
      gain =  number_range(ch->level / 5, ch->level / 3);
      gain += int_app[get_curr_stat(ch,STAT_INT)].manap * 2;
      gain += wis_app[get_curr_stat(ch,STAT_WIS)].manap * 2;
      gain = 50 * gain / UMAX(10, ch->level);
      number = number_percent();
      if (number < get_skill(ch,gsn_meditation))
	{
	  gain += number * gain / 100;
	  if (ch->mana < ch->max_mana)
	    check_improve(ch,gsn_meditation,TRUE,1);
	}
      else
	check_improve(ch,gsn_meditation,FALSE,1);
      number = number_percent();
      if (number < get_skill(ch,gsn_trance))
	{
	  gain += number * gain / 100;
	  if (ch->mana < ch->max_mana)
	    check_improve(ch,gsn_trance,TRUE,1);
	}
      else
	check_improve(ch,gsn_trance,FALSE,1);

      if (class_table[ch->class].fMana)
	gain *= 2;
      if (class_table[ch->class].sMana)
	gain = 3 * gain / 2;
      switch ( ch->position )
	{
        default:            gain /= 4;                      break;
        case POS_SLEEPING:                                  break;
        case POS_RESTING:   gain /= 2;                      break;
        case POS_FIGHTING:  gain /= 6;                      break;
        case POS_MEDITATE:  
	  if (IS_DNDS(ch)) gain /= 2; 
	  else gain/=4;					    break;
	}
      if (ch->pcdata->condition[COND_HUNGER] <= 5 )
	gain /= 2;
    }
    if ( is_affected(ch,gsn_mortally_wounded) 
	 || (!IS_NPC(ch) &&is_affected(ch,gsn_linked_mind)))
      return 0;
    if (ch->class == class_lookup("vampire"))
    {
    	if (is_affected(ch,gsn_coffin))
	    gain *= 3;
    	else if (vamp_check(ch))
	    return 0;
    	else if (vamp_day_check(ch))
	    gain /= 2;
	else
	    gain *= 2;
    }
    if (mud_data.questor[0] == ch->name[0] && !str_cmp(mud_data.questor, ch->name))
      gain += 15 * gain / 100;
    /* CABAL */   
    else if (ch->in_room && ch->in_room->area->pCabal){
      /* svants boost mana gain for non pure warriors */
      if (!str_cmp(ch->in_room->area->pCabal->name, "Savant") 
	  && (class_table[ch->class].fMana || class_table[ch->class].sMana))
	gain += 1 * gain / 10;
    }
    /* PCS only */
    if (!IS_NPC(ch))
      gain = gain * ch->in_room->mana_rate / 100;
    if ( IS_AFFECTED(ch, AFF_POISON)
	 && check_immune(ch, DAM_POISON, FALSE) )
	gain /= 4;
    if (IS_AFFECTED(ch, AFF_PLAGUE)
	 && check_immune(ch, DAM_DISEASE, FALSE) )
	gain /= 8;
    if (is_affected(ch, gsn_insomnia))
        gain /= 10;
    if (IS_AFFECTED(ch,AFF_HASTE))
        gain /= 2;
    if (IS_AFFECTED(ch,AFF_SLOW))
        gain *=2;
    if (is_song_affected(ch,gsn_lullaby))
	gain = 4*gain/3;
    else if (is_affected(ch,gsn_camp))
	gain = 17*gain/10;
    else if (is_affected(ch,gsn_prayer))
	gain *= 6/5;
    if (is_affected(ch,gsn_drained))
        gain /= 2;

    if (is_affected(ch, gen_d_shroud)){
      if (get_skill(ch, skill_lookup("death shroud")) > 100)
	gain  /= 2;
      else
	gain  = 0;
    }

//If burrowed no gain.
    if(is_affected(ch, gsn_burrow))
      gain = 0;
    if (is_affected(ch,gsn_unholy_strength) && !IS_NPC(ch))
    {    
	int number2 = 0;
	AFFECT_DATA *paf;
    	for ( paf = ch->affected; paf != NULL; paf = paf->next )        
	{
    	    if (paf->type == gsn_unholy_strength)        
    	    {            
    	        number2 = (paf->level%10) * 2 + (paf->level/10) * 2;            
    	        break;        
    	    }    
	}
    	if (ch->mana < number2)    
    	{       
    	    send_to_char("Your body can no longer substain such great power.\n\r", ch);              
    	    do_unholy_drain(ch);
	    affect_strip(ch,gsn_unholy_strength);
    	}    
    	else         
    	    gain -= number2;       
    }
    if ( (paf = affect_find(ch->affected, gsn_ancient_lore)) != NULL
	 && paf->modifier != ch->in_room->vnum){
      gain = -2 * gain;
    }
    else if (is_affected(ch, gsn_temp_storm )){
      gain = -2 * gain;
    }
    gain = gain * (100 + ch->aff_mod[MOD_MANA_GAIN]) / 100;
    return UMIN(gain, ch->max_mana - ch->mana);
}

int move_gain( CHAR_DATA *ch )
{
    int gain;
    AFFECT_DATA* paf;
    if (ch->in_room == NULL)
	return 0;

    if (ch->class == gcn_adventurer && ch->played > MAX_ADVENTURER_TIME){
      send_to_char("You have spent the maximum allowed time as an adventurer.\n\r"\
		   "Please choose your profession of choice with \"guild\" command.\n\r", ch);
      return 0;
    }
    if (is_affected(ch, gen_request)
	|| is_affected(ch, gen_chant)
	|| is_affected(ch, gen_wcraft)
	|| is_affected(ch, gen_acraft))
      return 0;

    if (IS_AFFECTED2(ch, AFF_CATALEPSY) || IS_STONED(ch))
	return 0;
    if ( IS_NPC(ch) )
	gain = ch->level;
    else
    {
	gain = UMAX( 15, ch->level );
	switch ( ch->position )
	{
	case POS_SLEEPING: gain += 2 * get_curr_stat(ch,STAT_DEX);	break;
	case POS_RESTING:  gain += get_curr_stat(ch,STAT_DEX);		break;
	case POS_STANDING: if (ch->pcdata->pStallion == NULL) gain /= 2; break;
	}
        if (ch->pcdata->condition[COND_THIRST] <= 5 )
	    gain /= 2;
    }
    if ( is_affected(ch,gsn_mortally_wounded) 
	 || (!IS_NPC(ch) &&is_affected(ch,gsn_linked_mind)))
      return 0;
    if ( is_song_affected(ch,gsn_dance_song))
      return 0;
    if (ch->class == class_lookup("vampire"))
    {
    	if (is_affected(ch,gsn_coffin))
	    gain *= 3;
    	else if (vamp_check(ch))
	    return 0;
    	else if (vamp_day_check(ch))
	    gain /= 2;
	else
	    gain *= 2;
    }
    if (mud_data.questor[0] == ch->name[0] && !str_cmp(mud_data.questor, ch->name))
      gain += 15 * gain / 100;
    /* CABAL */   
    else if (ch->in_room && ch->in_room->area->pCabal){
      /* warmasters boost movement gain */
      if (!str_cmp(ch->in_room->area->pCabal->name, "Warmaster") 
	  && !class_table[ch->class].fMana 
	  && class_table[ch->class].sMana)
	gain += 2 * gain / 10;
    }
    /* pcs heal normaly */
    if (!IS_NPC(ch))
      gain = gain * UMAX(20, ch->in_room->heal_rate) / 100;
    if ( IS_AFFECTED(ch, AFF_POISON)
	 && check_immune(ch, DAM_POISON, FALSE) )
	gain /= 8;
    if (IS_AFFECTED(ch, AFF_PLAGUE)
	 && check_immune(ch, DAM_DISEASE, FALSE) )
	gain /= 16;
    if (IS_AFFECTED(ch,AFF_HASTE))
        gain /=2;
    if (IS_AFFECTED(ch,AFF_SLOW))
        gain *=2;
    if (is_song_affected(ch,gsn_lullaby))
      gain = 4*gain/3;
    else if (is_affected(ch,gsn_camp))
      gain = 17*gain/10;
    if (is_affected(ch,gsn_drained))
      gain /= 10;
    if (is_affected(ch, gsn_embrace_poison))
      gain /= 8;
    if (is_affected(ch,gsn_caltraps))
      gain /= 4;

    if (is_affected(ch, gen_d_shroud)){
      if (get_skill(ch, skill_lookup("death shroud")) > 100)
	gain  /= 2;
      else
	gain = 0;
    }
//If burrowed no gain.
    if(is_affected(ch, gsn_burrow))
      gain = 0;
    if ( (paf = affect_find(ch->affected, gsn_ancient_lore)) != NULL
	 && paf->modifier != ch->in_room->vnum){
      gain = -2 * gain;
    }
    else if (is_affected(ch, gsn_temp_storm )){
      gain = -2 * gain;
    }
    gain = gain * (100 + ch->aff_mod[MOD_MOVE_GAIN]) / 100;
    return UMIN(gain, ch->max_move - ch->move);
}

void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;
    int hung;
    int thir;
    if ( value == 0 || IS_NPC(ch) || IS_AFFECTED2(ch, AFF_CATALEPSY) || IS_STONED(ch))
	return;
    if (ch->in_room != NULL && 
	(IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
	 || IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL)
	 || IS_SET(ch->in_room->room_flags2, ROOM_NOHUNGER)))
      return;

    condition = ch->pcdata->condition[iCond];
    if (condition == -99)
        return;
    if (value > 0 && condition < 0){
      condition = 0;
    }
    ch->pcdata->condition[iCond] = URANGE( -24, condition + value, 48 );

    hung = ch->pcdata->condition[COND_HUNGER];
    thir = ch->pcdata->condition[COND_THIRST];

    if ( ch->pcdata->condition[iCond] <= 5 && ch->desc != NULL && !is_affected(ch, gsn_fasting))
	switch ( iCond )
	{
        case COND_HUNGER: 
	if (ch->pcdata->condition[COND_HUNGER] <= -19 )
        {
	    send_to_char( "You are starving to death!\n\r", ch );
		if (is_pk(ch,ch) && ch->pcdata->condition[COND_HUNGER] <= 0 && ch->timer < 15 && !IS_AFFECTED(ch,AFF_SLEEP))
            	    damage( ch, ch, number_range(1,ch->level/2) - (ch->level * hung /25), 1046, DAM_INTERNAL,TRUE);
 	}
	else if (ch->pcdata->condition[COND_HUNGER] <= -5 )
	{
	    send_to_char( "You are starving.\n\r", ch );
		if (is_pk(ch,ch) && ch->pcdata->condition[COND_HUNGER] <= 0 && ch->timer < 15 && !IS_AFFECTED(ch,AFF_SLEEP))
            	    damage( ch, ch, number_range(1,ch->level/2) - (ch->level * hung /50), 1046, DAM_INTERNAL,TRUE);
 	}
	else
	{
 	    send_to_char( "You are hungry.\n\r", ch );
		if (is_pk(ch,ch) && ch->pcdata->condition[COND_HUNGER] <= 0 && ch->timer < 15 && !IS_AFFECTED(ch,AFF_SLEEP))
            	    damage( ch, ch, number_range(1,ch->level/2), 1046, DAM_INTERNAL,TRUE);
 	}
	break;
        case COND_THIRST: 
	if (ch->pcdata->condition[COND_THIRST] <= -19)
	{
	    send_to_char( "You are dying of thirst!\n\r", ch ); 
		if (is_pk(ch,ch) && ch->pcdata->condition[COND_THIRST] <= 0 && ch->timer < 15 && !IS_AFFECTED(ch,AFF_SLEEP))
            	    damage( ch, ch, number_range(1,ch->level/2) - (ch->level * thir /25), 1047, DAM_INTERNAL,TRUE);
	}
	else if (ch->pcdata->condition[COND_THIRST] <= -5)
	{
	    send_to_char( "You are parched.\n\r", ch ); 
		if (is_pk(ch,ch) && ch->pcdata->condition[COND_THIRST] <= 0 && ch->timer < 15 && !IS_AFFECTED(ch,AFF_SLEEP))
            	    damage( ch, ch, number_range(1,ch->level/2) - (ch->level * thir /50), 1047, DAM_INTERNAL,TRUE);
	}
	else
	{
  	    send_to_char( "You are thirsty.\n\r", ch ); 
		if (is_pk(ch,ch) && ch->pcdata->condition[COND_THIRST] <= 0 && ch->timer < 15 && !IS_AFFECTED(ch,AFF_SLEEP))
                    damage( ch, ch, number_range(1,ch->level/2), 1047, DAM_INTERNAL,TRUE);
	}
	break;
	}
    if ( ch->pcdata->condition[COND_DRUNK] == 1 && iCond == COND_DRUNK)
      send_to_char( "You are sober.\n\r", ch );
    if ( ch->pcdata->condition[COND_DRUNK] < 0 )
      ch->pcdata->condition[COND_DRUNK] = 0;
    
    if ((!is_affected (ch, gen_yearning)) && 
	(ch->class == class_lookup ("druid"))) {  
      AFFECT_DATA af;
      
      af.where     = TO_NONE;
      af.type      = gen_yearning;
      af.level     = 1;
      af.duration  = -1;
      af.location  = 0;
      af.modifier  = 0;
      af.bitvector = 0;
      affect_to_char( ch, &af );
    }

}


/* Mob autonomous action.                          *
 * This function takes 25% to 35% of all cpu time. */
void mobile_update( void )
{
    CHAR_DATA *ch, *ch_next;
    EXIT_DATA *pexit;
    int door;
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next = ch->next;
	if ( !IS_NPC(ch) || ch->in_room == NULL )
	    continue;
//Viri: fast mobs track at max speed
	if (IS_SET(ch->off_flags,OFF_FAST))
	  ch->hunttime = 999;

	if (IS_SET(ch->special, SPECIAL_RETURN) && ch->fighting == NULL && ch->in_room != ch->summoner->in_room
	&& ch->position == POS_STANDING && IS_AWAKE(ch) && !IS_AFFECTED2(ch, AFF_HOLD))
	{
	    ch->hunttime = 0;
	    return_to_leader(ch);
	}
	if (ch->extracted )
	{
	    ch->extracted = FALSE;
	    extract_char(ch,TRUE);
	}
        if (ch->hunting != NULL 
	    && (ch->hunttime++) >= 5-ch->level/10
	    && ch->fighting == NULL && ch->in_room != ch->hunting->in_room
	    && !IS_SET(ch->act, ACT_SENTINEL) 
	    && !IS_SET(ch->off_flags, GUILD_GUARD) 
	    && ch->pIndexData->pShop == NULL 
	    && IS_AWAKE(ch) 
	    && ch->position == POS_STANDING 
	    && !IS_AFFECTED2(ch, AFF_HOLD) )
	  {
	    ch->hunttime = 0;
            hunt_victim(ch);
	    if (ch == NULL || ch->in_room == NULL)
	      continue;
	  }
	if (ch->in_room->area->empty && !IS_SET(ch->act,ACT_UPDATE_ALWAYS))
	    continue;
	if (ch->pIndexData->progtypes & RAND_PROG)
            mprog_random_trigger( ch );
	if (ch == NULL || ch->in_room == NULL)
	  continue;
	if (ch->spec_path)
	  spec_path_walk( ch );
	if (ch == NULL || ch->in_room == NULL)
	  continue;
	if ( ch->spec_fun != 0 ){
	  if ( ((*ch->spec_fun) ( ch )) == TRUE )
	    continue;
	}
	if ( ch->position != POS_STANDING )
	    continue;
	if ( !IS_SET(ch->act, ACT_SENTINEL) && IS_SET(ch->act, ACT_WANDER)
	     && ch->spec_path == NULL
	     && ( door = number_bits( 5 ) ) <= 5 && ( pexit = ch->in_room->exit[door] ) != NULL
	     && pexit->to_room != NULL && !IS_SET(pexit->exit_info, EX_CLOSED)
	     && !(IS_SET(pexit->to_room->area->area_flags, AREA_MUDSCHOOL)
		  && !IS_SET(ch->in_room->area->area_flags, AREA_MUDSCHOOL) )
	     && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) 
	     && !IS_SET(ch->act, ACT_NO_WANDER)
	     && ( !IS_SET(ch->act, ACT_STAY_AREA) || pexit->to_room->area == ch->in_room->area )
	     && ( !IS_SET(ch->act, ACT_OUTDOORS) || !IS_SET(pexit->to_room->room_flags,ROOM_INDOORS)) 
	     && ( !IS_SET(ch->act, ACT_INDOORS) || IS_SET(pexit->to_room->room_flags,ROOM_INDOORS)))

	{
	    move_char( ch, door, FALSE );
	    if (ch == NULL || ch->in_room == NULL)
	      continue;
	    else if ( ch->position < POS_STANDING )
	      continue;
	}
	if ( !IS_SET(ch->act, ACT_SENTINEL) && number_bits(4) == 0 && ( door = number_bits( 5 ) ) <= 5
	     && ch->spec_path == NULL
	     && ( pexit = ch->in_room->exit[door] ) != NULL && pexit->to_room != NULL
	     && !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
	     && !IS_SET(ch->act, ACT_NO_WANDER)
	     && ( !IS_SET(ch->act, ACT_STAY_AREA) || pexit->to_room->area == ch->in_room->area ) 
	     && ( !IS_SET(ch->act, ACT_OUTDOORS) || !IS_SET(pexit->to_room->room_flags,ROOM_INDOORS)) 
	     && ( !IS_SET(ch->act, ACT_INDOORS) || IS_SET(pexit->to_room->room_flags,ROOM_INDOORS)))
	  {
	    move_char( ch, door, FALSE );
            if ( ch->position < POS_STANDING )
                 continue;
	}
	if (ch == NULL || ch->in_room == NULL)
	  continue;
	if ( IS_SET(ch->act, ACT_SCAVENGER) && ch->in_room && ch->in_room->contents != NULL && number_bits( 6 ) == 0 )
	{
	    OBJ_DATA *obj, *obj_best = NULL;
	    int max = 1;
	    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
                if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max  && obj->cost > 0)
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    if ( obj_best )
	    {
	      act( "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
	      obj_from_room( obj_best );
	      if (ch->pIndexData->pShop != NULL)
		obj_to_keeper( obj_best, ch );
	      else
		obj_to_char( obj_best, ch );
	    }
	}
    }
}

void weather_update( void )
{
    char buf[MSL];
    DESCRIPTOR_DATA *d;
    int diff;
    buf[0] = '\0';
    switch ( ++mud_data.time_info.hour )
    {
    case  4:
        strcat( buf, "The night is about to end.\n\r" ); break;
    case  6: mud_data.weather_info.sunlight = SUN_RISE;
        strcat( buf, "The day has begun.\n\r" ); break;
    case  7: mud_data.weather_info.sunlight = SUN_LIGHT;
        strcat( buf, "The sun rises in the east.\n\r" ); break;
    case 18: mud_data.weather_info.sunlight = SUN_SET;
        strcat( buf, "The sun slowly disappears in the west.\n\r" ); break;
    case 19: mud_data.weather_info.sunlight = SUN_DARK;
        strcat( buf, "The night has begun.\n\r" ); break;
    case MAX_HOURS: mud_data.time_info.hour = 0;
        mud_data.time_info.day++; break;
    }
    if ( mud_data.time_info.day   >= MAX_DAYS * MAX_WEEKS )
    {
	mud_data.time_info.day = 0;
	mud_data.time_info.month++;
    }
    if ( mud_data.time_info.month >= MAX_MONTHS )
    {
	mud_data.time_info.month = 0;
	mud_data.time_info.year++;
    }

    if ( mud_data.time_info.month >= 9 && mud_data.time_info.month <= 16 )
	diff = mud_data.weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = mud_data.weather_info.mmhg > 1015 ? -2 : 2;
    mud_data.weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    mud_data.weather_info.change    = UMAX(mud_data.weather_info.change, -12);
    mud_data.weather_info.change    = UMIN(mud_data.weather_info.change,  12);
    mud_data.weather_info.mmhg += mud_data.weather_info.change;
    mud_data.weather_info.mmhg  = UMAX(mud_data.weather_info.mmhg,  960);
    mud_data.weather_info.mmhg  = UMIN(mud_data.weather_info.mmhg, 1040);

  for ( d = descriptor_list; d != NULL; d = d->next )
  {  
   if ( d->connected == CON_PLAYING && IS_OUTSIDE(d->character) && IS_AWAKE(d->character) && d->character->in_room != NULL)
   {
    int temp = temp_adjust(d->character->in_room->temp);
    switch ( mud_data.weather_info.sky )
    {
    default:
        bug( "Weather_update: bad sky %d.", mud_data.weather_info.sky );
        mud_data.weather_info.sky = SKY_CLOUDLESS;
	break;
    case SKY_CLOUDLESS:
        if ( mud_data.weather_info.mmhg <  990 || ( mud_data.weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "`&The sky is getting cloudy.``\n\r" );
	    mud_data.weather_info.sky = SKY_CLOUDY;
	}
	break;
    case SKY_CLOUDY:
        if ( mud_data.weather_info.mmhg <  970 || ( mud_data.weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{
	    if (temp < 30)
		strcat( buf, "`&It starts to snow.``\n\r" );
	    else if (temp < 36)
		strcat( buf, "`&It starts to sleet.``\n\r" );
	    else
		strcat( buf, "`&It starts to rain.``\n\r" );
	    mud_data.weather_info.sky = SKY_RAINING;
	}
	if ( mud_data.weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "`&The clouds disappear.``\n\r" );
	    mud_data.weather_info.sky = SKY_CLOUDLESS;
	}
	break;
    case SKY_RAINING:
	if ( mud_data.weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    if (temp < 30)
		strcat( buf, "`&The snowstorm becomes a blizzard.``\n\r" );
	    else if (temp < 36)
		strcat( buf, "`&The sleet turns into hail.``\n\r" );
	    else
		strcat( buf, "`&Lightning flashes in the sky.``\n\r" );
	    mud_data.weather_info.sky = SKY_LIGHTNING;
	}
        if ( mud_data.weather_info.mmhg > 1030 || ( mud_data.weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    if (temp < 30)
		strcat( buf, "`&The snow stopped.``\n\r" );
	    else if (temp < 36)
		strcat( buf, "`&The sleet stopped.``\n\r" );
	    else
		strcat( buf, "`&The rain stopped.``\n\r" );
	    mud_data.weather_info.sky = SKY_CLOUDY;
	}
	break;
    case SKY_LIGHTNING:
        if ( mud_data.weather_info.mmhg > 1010 || ( mud_data.weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    if (temp < 30)
		strcat( buf, "`&The blizzard has slowed down.``\n\r" );
	    else if (temp < 36)
		strcat( buf, "`&The hail stops and it starts sleeting.``\n\r" );
	    else
		strcat( buf, "`&The lightning has stopped.``\n\r" );
	    mud_data.weather_info.sky = SKY_RAINING;
	    break;
	}
	break;
    }
    if ( buf[0] != '\0' )
	send_to_char( buf, d->character );
   }
  }
}


//Main update function.
void char_update( void )
{   
    CHAR_DATA *ch, *ch_next, *ch_quit, *questor = NULL;
    CABAL_DATA* cp;
    AFFECT_DATA *baf;
    static int master_questor_count;
    bool is_rage = FALSE, is_timer_exp = FALSE, is_linked_mind = FALSE,
	is_insomnia = FALSE, is_tarot_death = FALSE, is_end_leech = FALSE,
      transform_phoenix = FALSE;
    bool fRevTime = FALSE;
    bool fBael = FALSE;
    bool fTempStorm = FALSE;
    bool fWanted = FALSE;

    int pnumber = 0;
    ch_quit = NULL;

    if (save_number++ > 29)
	save_number = 0;

    for ( ch = char_list; ch != NULL; ch = ch_next)
    {
	AFFECT_DATA *paf, *paf_next;
	fWanted = FALSE;
	fRevTime = FALSE;
	fBael = FALSE;
	fTempStorm = FALSE;
  
	ch_next = ch->next;
	is_rage = FALSE;
	is_timer_exp = FALSE;
	is_linked_mind = FALSE;
	is_insomnia = FALSE;
	is_tarot_death = FALSE;
	is_end_leech = FALSE;
	transform_phoenix = FALSE;

	sec_check(ch);
//EZ case if ch is not anywhere.
	if (ch->in_room == NULL)
	{
	    extract_char(ch,TRUE);
	    continue;
	}
//Check for null char.
	if (ch == NULL)
	    continue;
	/* JAIL ABUSE DECREASE */
	jail_abuse_check(ch, FALSE);
	/* CP gain */


	/* NPC STUFF ONLY */
	if (!IS_NPC(ch)){

	  /* royal guard replinishment */
	  if(IS_ROYAL(ch) || IS_NOBLE(ch)){
	    create_royal_guard( ch, ch->alignment, ch->pcdata->rank );
	  }

	  /* lower AOS level */
	  if (ch->pcdata->race_kills[0]++ > AOS_IMPROVE_TICKS){
	    int i;
	    ch->pcdata->race_kills[0] = 0;

	    /* lower all the racial levels except for racial enemy */
	    for (i = 0; i < MAX_PC_RACE; i++){
	      if (i == ch->pcdata->enemy_race)
		continue;
	      else if (ch->pcdata->race_kills[i] < 1)
		continue;
	      ch->pcdata->race_kills[i]--;
	    }
	  }

	  /* Cabal Points update (timed)*/
	  CPS_GAIN(ch, cp_calculate_timegain(ch), FALSE);

	  /* Quest expiration */
	  QuestTimeCheck( ch );

	  /* cabal member count */
	  if (ch->pCabal){
	    /* count active member */
	    cp = get_parent( ch->pCabal );
	    if (!IS_IMMORTAL(ch))
	      cp->count_present ++;
	    if (cp != ch->pCabal)
	      ch->pCabal->count_present ++;
	    /* update cabal member data */
	    update_cmember( ch );

	    /* justice present check */
	    if (IS_CABAL(ch->pCabal, CABAL_JUSTICE) 
		&& ch->in_room && ch->in_room->area->pCabal){
	      affect_justice_relations(ch->in_room->area->pCabal, 1);	
	    }       
	  }
	  
	  if (ch->level < 20 && ch->pcdata->kpc > 5 
	      && mud_data.mudport == MAIN_PORT
	      && !IS_GAME(ch, GAME_PKWARN)){
	    char buf[MIL];
	    sprintf(buf, warn_table[WARN_PK].subj, ch->name);
	    SET_BIT(ch->game, GAME_PKWARN);
	    do_hal(ch->name, buf, warn_table[WARN_PK].text, NOTE_NOTE);
	    do_save(ch, "");
	    do_hal("immortal", buf, warn_table[WARN_PK_IMM].text, NOTE_PENALTY);
	  }
	  else if (ch->level >= 25 
		   && mud_data.mudport == MAIN_PORT
		   && IS_GAME(ch, GAME_PKWARN)){
	    char buf[MIL];
	    sprintf(buf, warn_table[WARN_PK_END].subj, ch->name);
	    REMOVE_BIT(ch->game, GAME_PKWARN);
	    do_save(ch, "");
	    do_hal("immortal", buf, warn_table[WARN_PK_END].text,NOTE_PENALTY);
	  }
	}//END OF PC ONLY
//Check for character needs master.
	if (IS_NPC(ch) && IS_SET(ch->act2, ACT_NEED_MASTER) && !IS_SET(ch->special,SPECIAL_SENTINEL)
	    && !IS_SET(ch->special,SPECIAL_COWARD)
	    && (!ch->master || !ch->leader || !IS_AFFECTED(ch, AFF_CHARM))){
	  do_say(ch, "Where is my master?");
	  act("$n slowly disappears.\n\r", ch, NULL, NULL, TO_ROOM);
	  extract_char(ch, TRUE);
	  continue;
	}
//MOB STUFF
	if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_SOLID && number_percent() < 20)
	  ecto_vapor(NULL, ch);
	else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_VAPOR && number_percent() < 20)
	  ecto_solid(NULL, ch);

	if (IS_NPC(ch) && ch->pIndexData->progtypes & TIME_PROG)
	    mprog_time_trigger( ch );
	/* New Progs */
	if ( ch 
	     && IS_NPC(ch) 
	     && ch->in_room 
	     && (!ch->in_room->area->empty || IS_SET(ch->act, ACT_UPDATE_ALWAYS)) )
	{
	  if (ch->mprog_delay > 0 )
	    {
	      if ( --ch->mprog_delay <= 0 &&  HAS_TRIGGER_MOB( ch, TRIG_DELAY ))
		p_percent_trigger( ch, NULL, NULL, ch, NULL, NULL, TRIG_DELAY );
	    }
	  if (ch == NULL || ch->in_room == NULL)
	    continue;
	  if (HAS_TRIGGER_MOB( ch, TRIG_RANDOM ) )
	    p_percent_trigger( ch, NULL, NULL, NULL, NULL, NULL, TRIG_RANDOM );
	  if (ch == NULL || ch->in_room == NULL)
	    continue;
	}

	//Jailcell escape check
	if (ch->in_room && IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL)
	    && !jail_check(ch->in_room))
	  send_to_char("The magical barrier sputters due to lack of focus offering a way to freedom!\n\r", ch);

//No Quit check.
        if ( ch->timer > 29 && !(!IS_NPC(ch) && IS_SET(ch->act2,PLR_NOQUIT)) && 
	    !is_affected(ch,gsn_noquit) && !is_affected(ch,gen_ensnare))
            ch_quit = ch;

// Sever Raw Kill
	if(IS_SET(ch->affected2_by,AFF_SEVERED)) {
	    REMOVE_BIT(ch->affected2_by,AFF_SEVERED);
	    act("With a last gasp of breath, $n dies due to massive upper body trauma.", ch,NULL,NULL,TO_ROOM);
	    if(!IS_NPC(ch))
	      send_to_char("Your injuries prove too much, and you die from loss of blood.\n\r",ch);
	    raw_kill(ch, ch);
	    continue;
	}
    
//Divine gain
	if (!IS_NPC(ch) && IS_AVATAR(ch) && get_avatar_level(ch) < 1)
	  divfavor_gain(ch,  1);

//Avatar tickupdate
	if (IS_AVATAR(ch))
	  avatar_tick(ch);

	if (is_affected(ch, gsn_ancient_lore))
	  fRevTime = TRUE;
	if (is_affected(ch, gsn_temp_storm)){
	  fTempStorm = TRUE;
	}

//Gain hitpoints
	if ( ch->position >= POS_STUNNED )
	{
	  if ( ch->hit < ch->max_hit || fRevTime || fTempStorm ){
	    int gain = hit_gain(ch);
	    if (gain < 0){
	      if (ch->hit + gain < 0)
		gain = UMIN(0, -ch->hit );
	      if (gain)
		damage(ch, ch, -gain, gsn_regeneration, DAM_INTERNAL, FALSE );
	    }
	    else
	      ch->hit += gain;
	  }
	  else
	    ch->hit = ch->max_hit;
//Gain manna
	  if (( ch->mana < ch->max_mana 
		&& !IS_AFFECTED2(ch,AFF_BARRIER) 
		&& !IS_AFFECTED2(ch, AFF_SHADOWFORM) 
		&& !IS_AFFECTED2(ch,AFF_TREEFORM) 
		&& !is_affected(ch, gsn_burrow)) 
	      || fRevTime
	      || fTempStorm
	      || ch->class == class_lookup("monk"))
	    ch->mana = UMAX(0, ch->mana + mana_gain(ch));
	  else if (IS_AFFECTED2(ch, AFF_SHADOWFORM))
	    {
//MANA loss due to shadowform
	      ch->mana -= 20;
//If we run out of mana for shadowform.
	      if (ch->mana < 0)
		{
		  ch->mana = 0;
		  REMOVE_BIT(ch->affected2_by, AFF_SHADOWFORM);
		  affect_strip(ch, gsn_shadowform);
		  act_new("Your body regains its substance, and you materialize into existence.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
		  act("$n's body regains its substance, and $e materializes into existence.",ch,NULL,NULL,TO_ROOM);
		  WAIT_STATE(ch, 24);
		}//end if mana<0
	    }//end if is shadowformed
            else if (IS_AFFECTED2(ch, AFF_SHADOWFORM))
	      {
//MANA loss due to treeform

                ch->mana -= 20;
//If we run out of mana for treeform.
                if (ch->mana < 0)
                {
                    ch->mana = 0;
                    REMOVE_BIT(ch->affected2_by, AFF_TREEFORM);
                    affect_strip(ch, gsn_treeform);
                    act_new("Your body returns to its normal shape.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
                    act("$n's body returns to its normal shape.",ch,NULL,NULL,TO_ROOM);
                    WAIT_STATE(ch, 24);
                }//end if mana<0
            }//end if is treeformed 
 //Check for mist form.
	    else if (is_affected(ch,gsn_mist_form))
	    {
	      if (is_affected(ch, gsn_burrow))
		ch->mana -= 50;
	      else
		ch->mana -= 25;
		if (ch->mana < 0)
		{
		    ch->mana = 0;
                    affect_strip(ch, gsn_mist_form);
	            free_string( ch->short_descr );
    		    ch->short_descr = str_dup ( "" );
    		    ch->dam_type = 17;
    		    send_to_char("You revert back to your natural form.\n\r",ch);
    		    act("$n reverts back to $s natural form.",ch,NULL,NULL,TO_ROOM);
    		    WAIT_STATE( ch, 12);
		}//end if mana<0
	    }//end if mistform
//check barrier.
	    else if (IS_AFFECTED2(ch,AFF_BARRIER))
	    {
		ch->mana -= 20;
		if (ch->mana < 0)
//Strip barrier if no manna
		{
                    ch->mana = 0;
                    REMOVE_BIT(ch->affected2_by, AFF_BARRIER);
                    affect_strip(ch,skill_lookup("barrier"));
                    send_to_char("Your barrier dissipates.\n\r",ch);
                    act("$n's barrier dissipates.",ch,NULL,NULL,TO_ROOM);
                    WAIT_STATE(ch, 12);
		}//end if mana<0
	    }//end if barrier.
	    else{
	      //We regain mana normaly.
	      if ( ch->mana < ch->max_mana || fRevTime || fTempStorm )
		ch->mana += mana_gain(ch);
	      else
		ch->mana = ch->max_mana;
	    }
//Same for moves
	  if ( ch->move < ch->max_move || fTempStorm || fRevTime)
	    ch->move += move_gain(ch);
	  else
	    ch->move = ch->max_move;
	  ch->move = UMAX(0, ch->move);
	}
	
//check for dupes
	if (IS_NPC(ch) && IS_SET(ch->act2,ACT_DUPLICATE) && ch->master != NULL && ch->in_room != NULL 
	    && ch->master->in_room != NULL && ch->in_room != ch->master->in_room)
	{
	    act("$n loses it's energy source and disappears.",ch,NULL,NULL,TO_ROOM);
	    char_from_room(ch);
	    extract_char(ch,TRUE);
	    continue;
	}

//mirror image drain/existence check 
	if (IS_NPC(ch) && ch->master && ch->pIndexData->vnum== MOB_VNUM_DUMMY){
	  CHAR_DATA* och = ch->master;
	  const int cost = 10;
	  if (!ch->in_room || !och->in_room  || ch->in_room != och->in_room
	      || och->mana < cost){
	    act("$n loses its focus and disappears.", ch, NULL, NULL, TO_ROOM);
	    extract_char(ch,TRUE);
	    continue;
	  }
	  else
	    och->mana -= cost;
	}

//Sunburn check
	if (!IS_NPC(ch) && vamp_check(ch) && !is_ghost(ch,600)
	    && (ch->tattoo != deity_lookup("cycle") || mud_data.time_info.hour == 12))
	{
	    int dam2 = ch->level;
	    send_to_char("The rays from the sun burns into your flesh.\n\r",ch);
	    if (mud_data.time_info.hour < 12)
		dam2 = dam2* UMAX(1,(mud_data.time_info.hour < 10? mud_data.time_info.hour - 4 : mud_data.time_info.hour))/12;
	    else if (mud_data.time_info.hour > 12)
		dam2 = dam2*(24 - mud_data.time_info.hour)/12;
	    else
		dam2 = 3*dam2/2;
	    damage(ch,ch,dam2, 1052, DAM_LIGHT,TRUE);
	    if (!is_affected(ch,gsn_drained))
	      {
		AFFECT_DATA af;
		send_to_char("You feel weak...\n\r",ch);
		af.where            = TO_AFFECTS;
		af.type             = gsn_drained;
		af.level            = ch->level;
		af.duration         = -1;
		af.location         = 0;
		af.modifier         = 0;
		af.bitvector        = 0;
		affect_to_char(ch,&af);   
	      }
	}

	if ( ch->position == POS_STUNNED 
	     || ch->hit < 1)
	    update_pos( ch );

//Misc checks
	if (!IS_NPC(ch))
	{
	  if (ch->idle < 99)
	    ch->idle++;
	  gain_condition( ch, COND_DRUNK,  -1 );
	  if (number_percent() < 15 || tarot_find(ch->affected) == 11)
	    fluc_luck(ch);
	}

//More character checks, this has to be cleaned up some day..
	if ( !IS_NPC(ch) && ch->level < AVATAR && ch->in_room != NULL)
	{
	    OBJ_DATA *obj;
	    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] > 0 )
	    {
		if ( --obj->value[2] == 0 && ch->in_room != NULL )
		{
		    --ch->in_room->light;
		    act( "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( "$p flickers and goes out.", ch, obj, NULL, TO_CHAR );
		    extract_obj( obj );
		}
	 	else if ( obj->value[2] <= 5 && ch->in_room != NULL)
		    act("$p flickers.",ch,obj,NULL,TO_CHAR);
	    }

	    if (IS_IMMORTAL(ch)
		|| IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL))
	      ch->timer = 0;

//Idle logout
	    if ( ++ch->timer >= 14 
		 && ch->in_room 
		 && !IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL)
		 && !is_affected(ch,gsn_noquit) 
		 && !is_affected(ch,gen_ensnare) 
		 && !(!IS_NPC(ch) && IS_SET(ch->act2,PLR_NOQUIT)) )
	    {
		if ( ch->in_room != NULL && ch->in_room->vnum != ROOM_VNUM_LIMBO)
		{
		  if ( ch->fighting != NULL )
		    stop_fighting( ch, TRUE );
		  act( "$n disappears into the void.", ch, NULL, NULL, TO_ROOM );
		  act_new( "You disappear into the void.", ch, NULL, NULL, TO_CHAR, POS_DEAD );
		  if (ch->level > 1)
		    save_char_obj( ch );
		  char_from_room( ch );
		  char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }

	    if (ch->pcdata->fight_delay != (time_t)NULL && (int)difftime(mud_data.current_time,ch->pcdata->fight_delay) < 0)
		ch->pcdata->fight_delay = mud_data.current_time;
	    if (ch->pcdata->pk_delay != (time_t)NULL && (int)difftime(mud_data.current_time,ch->pcdata->pk_delay) < 0)
		ch->pcdata->pk_delay = mud_data.current_time;

//Hunger and etc.
	    if (is_affected(ch,skill_lookup("vitalize")))
	      {
		if (!IS_UNDEAD(ch))
		  gain_condition( ch, COND_THIRST, -5);
		gain_condition( ch, COND_HUNGER, -5);
	      }
	    else 
	      {
		int chance = 100;
		//slow down condition for newbies.
		//under weight people hunger a bit less too
		if (ch->level < 15)
		  chance = ch->level * 6;
		else if (IS_PERK(ch, PERK_UNDERWEIGHT))
		  chance = 75;
		if (chance >= 100 || number_percent() < chance){
		  if (!IS_UNDEAD(ch))
		    gain_condition( ch, COND_THIRST, -1 );
		  if (is_affected(ch, skill_lookup("bloodlust"))){
		    if (get_skill(ch, gsn_unholy_gift) > 1)
		      gain_condition( ch, COND_HUNGER, -2);
		    else
		      gain_condition( ch, COND_HUNGER, -4);
		  }
		  else
		    gain_condition( ch, COND_HUNGER, -1);
		}
	      }
	}
	if (is_song_affected(ch,gsn_lust) && IS_AWAKE(ch) && number_percent() < 5)
	{
	    act("You have the sudden urge to strip naked!",ch,NULL,NULL,TO_CHAR);
	    act("$n suddenly strips naked!",ch,NULL,NULL,TO_ROOM);
	    do_remove(ch,"all");
	}

//Tomagachi
	if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_PHOENIX && is_song_affected(ch, gsn_phoenix))
	{
	    if (ch->master == NULL)
	    {
		act("$n has died off without $s master.",ch,NULL,NULL,TO_ROOM);
		char_from_room(ch);
		extract_char(ch,TRUE);
		continue;
	    }
	    if (ch->master->in_room != ch->in_room)
	    {
		ch->practice = 0;
		ch->train = 0;
	    }
	    else if (ch->practice < 1)
	    {
		ch->train = UMAX(0,ch->train - 5);
		if (IS_AWAKE(ch))
		{
		    act("$n squawks madly at $N!",ch,NULL,ch->master,TO_NOTVICT);
		    act("$n squawks madly at you!",ch,NULL,ch->master,TO_VICT);
		}
	    }
	    else
		ch->practice = UMAX(0,ch->practice - 2);
	}

/* check a few things for affects here */
	if ( (baf = affect_find(ch->affected, gsn_torment_bind)) != NULL
	     && baf->location == APPLY_HITROLL)
	  fBael = TRUE;


//AFFECT CHECKS!!!
	for ( paf = ch->affected; paf != NULL; paf = paf_next )
	  {
	    paf_next	= paf->next;
	    /* wanted check */
	    if (paf->type == gsn_wanted)
	      fWanted = TRUE;
//AFFECT UPDATE
	    /* temporal storm */
	    if (paf->duration > 0 && fTempStorm){
	      if (paf->duration == 2)
		paf->duration = 0;
	      else 
		paf->duration -= 1;
	    }
	    if ( paf->duration > 0 )
	    {
//MIND LINK	
	      if ( paf->type == gsn_linked_mind && is_affected(ch, gsn_linked_mind) && paf->duration == 1)
		    send_to_char("You should return to your body soon.\n\r",ch);
	      if (paf->type != gsn_ancient_lore && fRevTime )
		paf->duration += 4;
//TORMENT BIND
	      /* torment bind bael causes prot. shield,and force field to not go down */
	      else if ( fBael && (paf->type == gsn_protective_shield || paf->type == gsn_forcefield))
		paf->duration ++;
//TRANSFORM (goes up one tick each tick instead of dropping if transformed)
	      else if (paf->type == gsn_transform){
		int sn = 0;
		if (is_affected(ch, gsn_weretiger)){
		  sn = gsn_weretiger;
		}
		else if (is_affected(ch, gsn_werewolf)){
		  sn = gsn_werewolf;
		}
		else if (is_affected(ch, gsn_werebear)){
		  sn = gsn_werebear;
		}
		else if (is_affected(ch, gsn_werefalcon)){
		  sn = gsn_weretiger;
		}
		if (sn){
		  if(paf->modifier != 0){
		    paf->duration += 1;
		    paf->modifier = 0;
		  }
		  else{
		    paf->duration += 2;
		    paf->modifier = 1;
		  }
		}
	      }
//DIVINE HEALING, WANTED AND NORMAL DURATION DROP
	      /* divine healing only goes down if not full */
	      /* wanted mark only goes down when not in jail cells */
	      if ( (paf->type != gsn_dheal || ch->hit < ch->max_hit)
		   && (paf->type != gsn_wanted || ch->in_room == NULL || !IS_SET(ch->in_room->room_flags, ROOM_JAILCELL)) ){
		paf->duration--;
	      }
            }
//AFFECT EXPIRY
	    else if ( paf->duration == 0 ){
	      if ( paf_next == NULL || paf_next->type != paf->type || paf_next->duration > 0 ){
//These are the turn off affects.
//GEN effects expiry updates are done in UPDATE_KILL
//end handled automaticly.
		if (!IS_GEN(paf->type))
		  {
		    if ( paf->type > 0 && skill_table[paf->type].msg_off )
		      act_new(skill_table[paf->type].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
		    if ( paf->type > 0 && skill_table[paf->type].msg_off2 )
		      act(skill_table[paf->type].msg_off2,ch,NULL,NULL,TO_ROOM);
		    }//end if not is gen

		if (paf->type == gsn_mounted && !IS_NPC(ch))
		  ch->pcdata->pStallion = NULL;
		if (paf->type == gsn_shadowform && IS_AFFECTED2(ch,AFF_SHADOWFORM))
		  WAIT_STATE(ch, 24);
		if (paf->type == gsn_treeform && IS_AFFECTED2(ch,AFF_TREEFORM))
		  WAIT_STATE(ch, 24);
		if (paf->type == gsn_rage && !IS_NPC(ch) && IS_AFFECTED2(ch, AFF_RAGE) && ch->class == class_lookup("berserker"))
		  is_rage = TRUE;
		if (paf->type == gsn_linked_mind && is_affected(ch,gsn_linked_mind)) 
		  is_linked_mind = TRUE;
		if (paf->type == gsn_timer && is_affected(ch,gsn_timer)) 
		  is_timer_exp = TRUE;
		if (paf->type == gsn_insomnia && is_affected(ch, gsn_insomnia))
		  is_insomnia = TRUE;
		if (paf->type == gsn_tarot && is_affected(ch, gsn_tarot) && paf->level == 69 && tarot_find(ch->affected) == 14)
		  is_tarot_death = TRUE;
		if (paf->type == gsn_leech && is_affected(ch, gsn_leech) && ch->race == race_lookup("illithid"))
		  is_end_leech = TRUE;
		if (paf->type == gsn_unholy_strength && is_affected(ch, gsn_unholy_strength) ){
		  do_unholy_drain(ch);
		}
		if ((paf->type == gsn_weretiger && is_affected(ch, gsn_weretiger)) 
		    || (paf->type == gsn_werewolf && is_affected(ch, gsn_werewolf))
		    || (paf->type == gsn_werebear && is_affected(ch, gsn_werebear)) 
		    || (paf->type == gsn_werefalcon && is_affected(ch, gsn_werefalcon)) ){
		  free_string( ch->short_descr );
		  ch->short_descr = str_empty;
		  ch->dam_type = 17;
		}
		if ((paf->type == gsn_bat_form && is_affected(ch,gsn_bat_form)) 
		    || (paf->type == gsn_wolf_form && is_affected(ch,gsn_wolf_form)) 
		    || (paf->type == gsn_mist_form && is_affected(ch,gsn_mist_form))){
		  if (!IS_NPC(ch)){
		    free_string( ch->short_descr );
		    ch->short_descr = str_empty;
		    ch->dam_type = 17;
		  }
		}
		if (paf->type == gsn_eavesdrop && is_affected(ch, gsn_eavesdrop)){
		  if (!IS_NPC(ch) && ch->pcdata->eavesdropping != NULL){
		    act("You stop eavesdropping on $N.",ch,NULL,ch->pcdata->eavesdropping,TO_CHAR);
		  ch->pcdata->eavesdropping->pcdata->eavesdropped = NULL;
		  ch->pcdata->eavesdropping = NULL;
		}
		}//end evesdrop
		//ANd burrow if needed.
		if ( (paf->type == gsn_burrow) && is_affected(ch, gsn_burrow) ){
		  do_unburrow(ch, NULL);
		  continue;
		}
		
		/* stop follower will remove charm person */
		  if (paf->type == gsn_charm_person 
		      || paf->type == gsn_attract){
		    if (IS_NPC(ch)){
		      if (empty_char(ch))
			act("$n spills some items on the ground.", ch, NULL, NULL, TO_ROOM);
		    }
		    REMOVE_BIT( ch->affected_by, AFF_CHARM );
		    if ( IS_NPC(ch) 
			 && ch->master 
			 && can_see( ch->master, ch ) 
			 && !IS_IMMORTAL(ch) 
			 && ch->in_room != NULL) {
		      act( "$n stops following you.",     ch, NULL, ch->master, TO_VICT    );
		      act( "You stop following $N.",      ch, NULL, ch->master, TO_CHAR    );
		      ch->leader = NULL;
		      ch->master = NULL;
		    }
		  }
	      }
	      affect_remove( ch, paf);
	      if (paf->type == gsn_coffin)
		do_stand(ch,"");
	      /* ANYTHING BEYOND THIS POINT CANNOT USE "paf" */
	    }//End checks for affects expiry.
	  }
	/* remove wanted */
	if (fWanted && IS_WANTED(ch) && !IS_OUTLAW(ch)){
	  AFFECT_DATA* pWant;
	  char buf[MIL];
	  /* check if we are no longer wanted */
	  for (pWant = ch->affected; pWant; pWant = pWant->next){
	    if (pWant->type == gsn_wanted
		&& pWant->bitvector != 0)
	      break;
	  }
	  /* if we found a wanted affect with a non zero bitvector then still wanted */
	  if (pWant == NULL){
	    REMOVE_BIT( ch->act, PLR_WANTED );
	    send_to_char( "You are no longer WANTED!\n\r", ch );
	    sprintf(buf,"NOTICE: Records of %s's crimes have been lost.", ch->name);
	    cabal_echo_flag( CABAL_JUSTICE, buf );
	  }
	}
	
//SECNOD CHECK
	for ( paf = ch->affected2; paf != NULL; paf = paf_next )
	  {
	    paf_next	= paf->next;
	    if ( paf->duration > 0 )
	      {
		paf->duration--;
		if (number_range(0,4) == 0 && paf->level > 0)
		  paf->level--;
	      }
	    else if ( paf->duration == 0 )
	      {
		if ( paf_next == NULL || paf_next->type != paf->type || paf_next->duration > 0 )
		  {
		    if ( paf->type > 0 && song_table[paf->type].msg_off )
		      act_new(song_table[paf->type].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
		    if ( paf->type > 0 && song_table[paf->type].msg_off2 )
		      act(song_table[paf->type].msg_off2,ch,NULL,NULL,TO_ROOM);
		    if (paf->type == gsn_phoenix && is_song_affected(ch, gsn_phoenix) && IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_PHOENIX)
		      {
			transform_phoenix = TRUE;
			pnumber = paf->modifier;
		      }
		  } 
		song_affect_remove( ch, paf);
	      }
	  }
	if (ch != NULL && !IS_NPC(ch) && is_rage)
	  {
	    ch->hit -= (ch->level * ch->perm_stat[STAT_CON]) /3;
	    if (IS_IMMORTAL(ch))
	      ch->hit = UMAX(1,ch->hit);
	    if (ch->hit < -2)
	      {
		int shock_mod = (25 * (0 - ch->hit)) / (1+ch->max_hit);
		if (number_percent() < con_app[UMAX(1,ch->perm_stat[STAT_CON] - shock_mod)].shock + 2*(get_curr_stat(ch,STAT_LUCK) -16))
		  {
		    act_new("Your body suddenly awakens to your wounds and you lose consciousness.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
		    act("$n suddenly clutches $s wounds and slumps to the ground, unconscious.",ch,NULL,NULL,TO_ROOM);
		    ch->hit = -2;
		    ch->position = POS_STUNNED;
		    act("$n is stunned, but will probably recover.",ch,NULL,NULL,TO_ROOM);
		    act_new("You are stunned, but will probably recover.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
		  }
		else
		  {
		    CHAR_DATA *victim = ch->fighting;
		    act_new("Your body could not sustain the injuries you've suffered.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
		    act("$n suddenly clutches $s wounds and slumps to the ground.",ch,NULL,NULL,TO_ROOM);
		    if (!IS_NPC(victim) 
			&& ( (!IS_NPC(ch) 
			      && is_challenger(ch,victim))
			     || (IS_NPC(ch) 
				 && ch->master
				 && is_challenger(ch->master,victim)) ))
		      {
			act( "$n is mortally wounded!!", ch, NULL, NULL, TO_ROOM );
	                act_new( "You are mortally wounded!!", ch, NULL, NULL, TO_CHAR, POS_DEAD );
			mortal_wound(victim, ch);
		      }
		    else
		      {
	             	act( "$n is DEAD!!", ch, NULL, NULL, TO_ROOM );
	                act_new( "You have been `1KILLED``!!", ch, NULL, NULL, TO_CHAR, POS_DEAD );
	            	raw_kill( ch, ch );
			continue;
		      }
		  }
	      }
	  }
	
//END OF AFFECTS
    



//BEGIN CHARACTER AFFECT TICK UPDATES//
//ALL THIS WILL EVENTULY BE MOVED INTO TICK_CHECK//

	//Run GEN Tick updates
	if (TICK_CHECK(ch) == -1)
	  bug("Error at: TICK_CHECK returned failure.\n\r", 0);
	if (ch == NULL || ch->in_room == NULL)
	  continue;
//END GEN TICK UPDATES

        if ( ch != NULL && !IS_NPC(ch) && is_linked_mind)
	{
	    send_to_char("The time to return to your body has expired.\n\r",ch);
	    raw_kill( ch, ch );
	    continue;
	}
        if ( ch != NULL && !IS_IMMORTAL(ch) && !IS_NPC(ch) && is_tarot_death)
	{
	    send_to_char("Your time in this world has expired.\n\r",ch);
	    act("A reaper appears from the shadows to collect your soul.",ch,NULL,NULL,TO_CHAR);
	    act("A reaper appears from the shadows to collect $n's soul.",ch,NULL,NULL,TO_ROOM);
	    raw_kill( ch, ch );
	    continue;
	}
        if ( ch != NULL && !IS_NPC(ch) && is_end_leech)
	{
	    AFFECT_DATA taf;
	    taf.where             = TO_AFFECTS;
    	    taf.type             = gsn_drained;
    	    taf.level            = ch->level;
    	    taf.duration         = number_fuzzy(4);
    	    taf.location         = APPLY_NONE;
    	    taf.modifier         = 0;
    	    taf.bitvector        = 0;
    	    affect_to_char(ch,&taf);
	    clear_nskill(ch);
	}	    
        if ( ch != NULL && is_timer_exp)
	{
	    if (IS_NPC(ch))
	    {
	    	act("$n disappears.",ch,NULL,NULL,TO_ROOM);
		char_from_room(ch);
		extract_char( ch, TRUE );
		continue;
	    }
	}
	
	if (ch != NULL && !IS_NPC(ch) && is_affected(ch,gsn_coffin))
	{
	    AFFECT_DATA *tpaf1, *tpaf2 = NULL;
	    if ((tpaf1 = affect_find(ch->affected,gsn_coffin)) != NULL )
	    	if (tpaf1->duration == 0 && (tpaf2 = affect_find(ch->affected,gsn_drained)) != NULL )
	    	    tpaf2->duration = 0;
	}
	if (ch != NULL && is_insomnia && !IS_UNDEAD(ch))
	{
	    AFFECT_DATA taf;
	    stop_fighting( ch, TRUE );
	    ch->position = POS_SLEEPING;
	    taf.where             = TO_AFFECTS;
    	    taf.type             = gsn_sleep;
    	    taf.level            = ch->level;
    	    taf.duration         = number_fuzzy(4);
    	    taf.location         = APPLY_NONE;
    	    taf.modifier         = 0;
    	    taf.bitvector        = AFF_SLEEP;
    	    affect_to_char(ch,&taf);
	}

	/* Plague */
	plague_effect(ch);
	if (ch == NULL || ch->in_room == NULL)
	  continue;

	/* clear poison to start damage */
	if (IS_AFFECTED(ch, AFF_POISON)){
	  AFFECT_DATA *paf = affect_find(ch->affected, gsn_poison);
	  if (paf != NULL)
	    REMOVE_BIT(paf->bitvector, AFF_FLAG);
	}

	if (ch != NULL && is_affected(ch,gsn_insomnia))
	{
	    if (ch->position > POS_SLEEPING)
	    	send_to_char("You can't focus from your deprivation of sleep.\n\r",ch);
	    else
		send_to_char("You toss and turn but can't manage to sleep soundly.\n\r",ch);
	    ch->mana = UMAX(0,(ch->mana - (ch->level/2)));
	}
	if (ch != NULL && !IS_NPC(ch) && is_affected(ch,skill_lookup("dysentery")))
	{
	    send_to_char("You drop to your knees, defecating violently.\n\r",ch);
	    ch->position = POS_SITTING;
	    ch->pcdata->condition[COND_HUNGER] = -1;
	    ch->pcdata->condition[COND_THIRST] = -1;
	    update_pos( ch );	    
	}
	if ( ch != NULL && ch->position == POS_INCAP && number_range(0,1) == 0)
            damage( ch, ch, 1, TYPE_HIT, DAM_INTERNAL,TRUE);
	else if ( ch != NULL && ch->position == POS_MORTAL )
            damage( ch, ch, 1, TYPE_HIT, DAM_INTERNAL,TRUE);
	else if ( ch != NULL && ch->position == POS_DEAD )
	{
	    ch->position = POS_MORTAL;
            damage( ch, ch, 1000, TYPE_HIT, DAM_INTERNAL,TRUE);
	    continue;
	}

        if ( ch != NULL && IS_AFFECTED(ch,AFF_SLEEP) && ch->position > POS_SLEEPING )
        {
	    REMOVE_BIT(ch->affected_by, AFF_SLEEP);
	    affect_strip(ch,gsn_sleep);
	}

	do_embrace_damage(ch);
	if (ch == NULL || ch->in_room == NULL)
	  continue;

    	if (ch != NULL && IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_SHADOW 
	    && room_has_sun(ch->in_room))
	  {
	    act("$n disappears in the daylight.",ch,NULL,NULL,TO_ROOM);
	    char_from_room(ch);
    	    extract_char( ch, TRUE );
	    continue;
    	}

	if (ch != NULL && is_affected(ch,gsn_tarot) && tarot_find(ch->affected) == 4 && number_percent() > 50)
	{
	    if (ch->gold < 100 && ch->in_bank > 500)
	    	ch->in_bank -= number_range(0,ch->in_bank/10);
	    else
	    	ch->gold -= number_range(0,ch->gold/10);
	
	}

	if (ch != NULL && is_affected(ch,gsn_tarot) && tarot_find(ch->affected) == 5 && number_percent() > 50)
	{
	    send_to_char("You spot a pile of gold in the corner of your eyes, this must be your lucky day!\n\r",ch);
	    ch->gold += number_range(100,ch->level*20);
	}

	if (ch != NULL && transform_phoenix)
	{
	    int i = 0;
	    char buf[MSL];
	    if (ch->train < 200)
	    {
		act("$n flashes brightly but quickly fades into a frail shriveled form.",ch,NULL,NULL,TO_ROOM);
		ch->train = 0;
		ch->practice = 0;
		ch->level = 1;
		ch->size = SIZE_TINY;
		ch->hit = 100;
		ch->max_hit = ch->hit;
    		for (i=0;i < MAX_STATS; i++)
    		    ch->perm_stat[i] = 3;
    		for (i=0; i < 4; i++)
    		    ch->armor[i] = 150;
		ch->hitroll = 0;
		ch->damroll = 0;
		ch->damage[DICE_NUMBER] = 1;
		free_string( ch->short_descr );
		sprintf(buf,"a malformed phoenix");
		ch->short_descr = str_dup(buf);
		free_string( ch->long_descr );
		sprintf(buf,"A malformed phoenix crawls around sickly.\n\r");
		ch->long_descr = str_dup(buf);
		act("$n looks sadly at $N.",ch,NULL,ch->master,TO_ROOM);
		act("$n looks sadly at you.",ch,NULL,ch->master,TO_NOTVICT);
	    }
	    else if (pnumber == 0)
	    {
		AFFECT_DATA af;
		act("$n bursts into a ball of flames, and reforms into the adolescent form!",ch,NULL,NULL,TO_ROOM);
		ch->train = 0;
		ch->practice = 0;
		ch->level = ch->master->level/2;
		ch->size = SIZE_SMALL;
		ch->hit = ch->level * 30;
		ch->max_hit = ch->hit;
    		for (i=0;i < MAX_STATS; i++)
    		    ch->perm_stat[i] = URANGE(15,ch->level/2,25);
    		for (i=0; i < 4; i++)
    		    ch->armor[i] = 0 - (4 * ch->level);
		ch->hitroll = ch->level/4;
		ch->damroll = ch->level/5;
		ch->damage[DICE_NUMBER] = ch->level/5;
    		af.where     = TO_AFFECTS;
    		af.type      = gsn_phoenix;
    		af.level     = ch->level;   
    		af.duration  = 100;
    		af.modifier  = 1;
    		af.location  = 0;
    		af.bitvector = 0;
    		song_affect_to_char( ch, &af );		
		sprintf(buf,"an adolescent phoenix");
		ch->short_descr = str_dup(buf);
		free_string( ch->long_descr );
		sprintf(buf,"An adolescent phoenix walks behind his master.\n\r");
		ch->long_descr = str_dup(buf);
	    }
	    else if (pnumber == 1)
	    {
		act("$n bursts into a ball of flames, and reforms into the adult form!",ch,NULL,NULL,TO_ROOM);
		ch->train = 0;
		ch->practice = 0;
		ch->level = ch->master->level;
		ch->size = SIZE_MEDIUM;
		ch->hit = ch->level * 30;
		ch->max_hit = ch->hit;
    		for (i=0;i < MAX_STATS; i++)
    		    ch->perm_stat[i] = URANGE(20,ch->level/2,25);
    		for (i=0; i < 4; i++)
    		    ch->armor[i] = 0 - (4 * ch->level);
		ch->hitroll = ch->level/4 + 5;
		ch->damroll = ch->level/5 + 5;
		ch->damage[DICE_NUMBER] = ch->level/5 + 2;
		SET_BIT(ch->affected_by,AFF_FLYING);
		sprintf(buf,"an adult phoenix");
		ch->short_descr = str_dup(buf);
		free_string( ch->long_descr );
		sprintf(buf,"An adult phoenix soars along the side his master.\n\r");
		ch->long_descr = str_dup(buf);
		act("$n fully spreads $s wings out.",ch,NULL,NULL,TO_ROOM);
	    }
	    else
		act("$n looks around confused.",ch,NULL,NULL,TO_ROOM);		
	}

	if (ch != NULL && ch->in_room
	    && IS_SET(ch->in_room->room_flags2, ROOM_PSI_FEAR))
	  effect_field_of_fear(ch);
    }

    for ( ch = player_list; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next_player;
        if (ch->desc != NULL && ch->desc->descriptor % 30 == save_number)
	    save_char_obj(ch);
        if ( ch == ch_quit )
            do_quit( ch, "" );
	else if (!IS_IMMORTAL(ch) && !IS_GAME(ch, GAME_NOQUESTOR)){
	  /* master questor check */
	  if (questor == NULL)
	    questor = ch;
	  else if (ch->pcdata->max_quests == questor->pcdata->max_quests
		   && ch->logon < questor->logon)
	    questor = ch;
	  else if (ch->pcdata->max_quests > questor->pcdata->max_quests)
	    questor = ch;
	}
    }
    /* record questor if any */
    if (questor && master_questor_count-- <= 0){
      master_questor_count = 5;
      questor_update(questor);
    }

    /* update the last count and reset all the cabal count_present */
    for (cp = cabal_list; cp; cp = cp->next){
      cp->present = cp->count_present;
      cp->count_present = 0;
      if (cp->parent == NULL && cp->present == 0 
	  && !cp->fGate && IS_CABAL(cp, CABAL_GATE))
	lock_cabal( cp, TRUE);
      else if (cp->parent == NULL && cp->present > 0){
	if (cp->fGate)
	  lock_cabal( cp, FALSE);
      }
    }
}

void obj_update( void )
{   
    OBJ_DATA *obj, *obj_next;
    AFFECT_DATA *paf, *paf_next;
    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
	CHAR_DATA *rch;
	char *message;
	obj_next = obj->next;

	if (obj->in_room != NULL 
	    && (obj->in_room->vnum == ROOM_VNUM_LIMIT || obj->in_room->vnum == ROOM_VNUM_BOUNTY))
	  continue;
	


//OBJTICK_CHECK
	if (OBJTICK_CHECK (obj, FALSE, NULL) == -1)
	  bug("Error at: OBJTICK_CHECK\n\r", 0);

	if (obj->in_room)
	  obj->idle++;
	else
	  obj->idle = 0;
	/* CABAL ITEM CHECK */
	if (obj->idle > 30 && obj->item_type == ITEM_CABAL && obj->in_room && obj->pCabal){
	  CABAL_DATA* pCab = obj->pCabal;
	  if (obj->in_room->people)
	    act("$p disappears.", obj->in_room->people, NULL, NULL, TO_ALL);
	  extract_obj( obj );
	  reset_altar( NULL, pCab );//this will reset altar and citem as necessary
	  continue;
	}
/* OWNER/REQUESTED ITEM IDLE CHECK */
	else if (obj->idle > 120 
		 && obj->in_room 
		 && CAN_WEAR(obj, ITEM_HAS_OWNER) 
		 && obj->item_type != ITEM_CABAL){
	  if (( rch = obj->in_room->people ) != NULL )
	    act( "$p becomes part of the ground.", rch, obj, NULL, TO_ALL );
	  obj_from_room(obj);
	  extract_obj(obj);
	  continue;
	}
	else if ( obj->in_room
		  && (obj->in_room->vnum != obj->homevnum 
		      || IS_SET(obj->extra_flags, ITEM_STAIN))
		  && obj->contains == NULL 
		  && CAN_WEAR(obj, ITEM_TAKE)
		  /* Need even cabal items to go. Viri
		     && obj->item_type != ITEM_CABAL
		     && !CAN_WEAR(obj, ITEM_NO_SAC) 
		     && !IS_OBJ_STAT(obj, ITEM_NOPURGE) 
		  */
		  && (obj->idle >= 480 
		      || IS_SET(obj->extra_flags, ITEM_STAIN)) )
	{
	  if (( rch = obj->in_room->people ) != NULL )
	    act( "$p becomes part of the ground.", rch, obj, NULL, TO_ALL );
	  obj_from_room(obj);
	  extract_obj(obj);
	  continue;
	}
        for ( paf = obj->affected; paf != NULL; paf = paf_next )
	  {
            paf_next = paf->next;

//OBJTICK_CHECK
	    if (OBJTICK_CHECK (obj, TRUE, paf) == -1)
	      bug("Error at: OBJTICK_CHECK\n\r", 0);
	    if (obj == NULL)
	      break;
            if ( paf->duration > 0 )
            {
                paf->duration--;
                if (number_range(0,4) == 0 && paf->level > 0)
                    paf->level--;
            }
            else if ( paf->duration == 0 )
                affect_remove_obj( obj, paf );
        }

/* Oprog triggers! */
	if ( obj->in_room || (obj->carried_by && obj->carried_by->in_room))
	{
	  CHAR_DATA *ch = obj->carried_by;
	    if ( obj->oprog_delay > 0 )
	    {
	      if ( --obj->oprog_delay <= 0 && HAS_TRIGGER_OBJ( obj, TRIG_DELAY ))
		p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_DELAY );
	    }
	    if (obj == NULL)
	      continue;
	    if ( (obj->carried_by 
		  || (obj->in_room && !obj->in_room->area->empty)
		  || IS_OBJ_STAT(obj, ITEM_UPDATE_ALWAYS))
		 && HAS_TRIGGER_OBJ( obj, TRIG_RANDOM ) )
	      p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_RANDOM );
	}
	/* Make sure the object is still there before proceeding */
	if ( !obj )
	    continue;

	/* falling object check */

	if (obj->in_room  
	    && (obj->wear_flags & ITEM_TAKE)
	    && obj->in_room->exit[5] 
	    && obj->in_room->exit[5]->to_room
	    && !IS_SET(obj->in_room->exit[5]->exit_info, EX_CLOSED) 
	    && obj->item_type != ITEM_CABAL
	    && !IS_SET(obj->wear_flags, ITEM_HAS_OWNER)
	    && number_range(1,100) > 95 )
	  {
	    ROOM_INDEX_DATA *new_room = obj->in_room->exit[5]->to_room;
	    if (( rch = obj->in_room->people ) != NULL )
	      act( "$p falls away.", rch, obj, NULL, TO_ALL );
	    obj_from_room(obj);
	    obj_to_room(obj, new_room);
	    if (( rch = obj->in_room->people ) != NULL )
	      act( "$p falls by.", rch, obj, NULL, TO_ALL );
	  }

	/* decay */
	if (obj->item_type == ITEM_FOOD ){
	  CHAR_DATA* och = in_char( obj );
	  /* shoppie check */
	  if (och != NULL 
	      && IS_NPC(och)
	      && och->pIndexData->pShop != NULL)
	    obj->timer ++;
	  else if (och && och->in_room){
	    CHAR_DATA* vch;
	    for (vch = och->in_room->people; vch; vch = vch->next_in_room){
	      if (!IS_NPC(vch) && is_same_group(vch, och)
		  && is_affected(vch, gsn_preserve)){
		obj->timer ++;
		break;
	      }
	    }//END character loop
	  }//END if in room
	}//END if FOOD
	/* Decay skipped  unless this is change from 1 to 0 */
	if ( obj->timer <= 0 || --obj->timer > 0 )
	  continue;
	switch ( obj->item_type )
	{
	default:              message = "$p crumbles into dust.";  break;
	case ITEM_FOUNTAIN:   message = "$p dries up.";         break;
	case ITEM_CORPSE_NPC: message = "$p decays into dust."; break;
	case ITEM_CORPSE_PC:  message = "$p decays into dust."; break;
	case ITEM_FOOD:       message = "$p decomposes.";	break;
	case ITEM_HERB:       message = "$p is no longer fresh.";break;
        case ITEM_POTION:     message = "$p has evaporated from disuse."; break;
	case ITEM_CONTAINER: 
            if (CAN_WEAR(obj,ITEM_WEAR_FLOAT))
            {
	        if (obj->contains)
                    message = "$p flickers and vanishes, spilling its contents on the floor.";
	        else
	            message = "$p flickers and vanishes.";
            }
	    else
	        message = "$p crumbles into dust.";
	    break;
	}
	if ( obj->carried_by != NULL )
	{
	    if (IS_NPC(obj->carried_by) && obj->carried_by->pIndexData->pShop != NULL)
                obj->carried_by->gold += obj->cost/5;
	    else
	    {
	    	act( message, obj->carried_by, obj, NULL, TO_CHAR );
		if ( obj->wear_loc == WEAR_FLOAT)
		    act(message,obj->carried_by,obj,NULL,TO_ROOM);
	    }
	}
        else if ( obj->in_room != NULL && ( rch = obj->in_room->people ) != NULL )
	    if (! (obj->in_obj && obj->in_obj->pIndexData->vnum == OBJ_VNUM_PIT && !CAN_WEAR(obj->in_obj,ITEM_TAKE)))
	    {
	    	act( message, rch, obj, NULL, TO_ROOM );
	    	act( message, rch, obj, NULL, TO_CHAR );
	    }
	    /* create a spirit for player corpse */
	if (obj->item_type == ITEM_CORPSE_PC){
	  char* name = strstr(obj->short_descr, "of ");
	  if (name)
	    create_spirit_mob( obj->in_room, obj->recall, (name + 3) );
	}
        if ((obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC) && obj->contains){
	  OBJ_DATA *t_obj, *next_obj;
	  
	  for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj){
	    next_obj = t_obj->next_content;

	    //remove object from corpse
	    obj_from_obj(t_obj);	
    
	    if (t_obj->item_type == ITEM_CABAL && t_obj->pCabal){
	      CABAL_DATA* pCab = t_obj->pCabal;
	      extract_obj( t_obj );
	      if (pCab->altar)
		reset_cabal_item(pCab->altar, pCab );
	      continue;
	    }
	    if (obj->in_obj)
	      obj_to_obj(t_obj,obj->in_obj);
	    else if (obj->carried_by)
	      obj_to_char(t_obj,obj->carried_by);
	    else if (obj->in_room == NULL){
	      extract_obj(t_obj);
	      continue;
	    }
	    else if (obj->item_type == ITEM_CORPSE_PC && obj->pCabal && obj->pCabal->pitroom){
	      OBJ_DATA* pit = get_cabal_pit( obj->pCabal );
	      if (pit)
		obj_to_obj(t_obj, pit );
	      else
		obj_to_room( t_obj, obj->pCabal->pitroom );
	    }
	    else if (obj->item_type == ITEM_CORPSE_PC){
	      OBJ_DATA *altar;
	      bool found = FALSE;
	      if (obj->recall == 0)
		obj_to_room(t_obj,get_room_index(1));
	      else{
		ROOM_INDEX_DATA* room = get_room_index( obj->recall);
		if (room == NULL)
		  room = get_room_index( ROOM_VNUM_LIMBO );
		for ( altar = room->contents; altar != NULL; altar = altar->next_content){
		  if (altar->pIndexData->vnum == get_temple_pit(obj->recall)){
		    obj_to_obj(t_obj, altar);
		    found = TRUE;
		  }
		}
		if (!found){
		  if ( (room = get_room_index( obj->recall )) == NULL)
		    room = get_room_index( ROOM_VNUM_LIMBO );
		  obj_to_room(t_obj,get_room_index(obj->recall));
		}
	      }
	    }
	    else
	      obj_to_room(t_obj,obj->in_room);
	  }
	}
	extract_obj( obj );
    }
}

/* trap update */
extern void init_trap(TRAP_DATA* tr, TRAP_INDEX_DATA* pTrapIndex);
void trap_update( void ){
  TRAP_DATA* pt, *pt_next;

  for (pt = trap_list; pt; pt = pt_next){
    AREA_DATA* pArea = pt->area;
    pt_next = pt->next;

    /* increase age */
    pt->age ++;

    /* if trap is nowhere get rid of it */
    if (pt->on_obj == NULL && pt->on_exit == NULL){
      bug("trap_update: trap vnum %d is nowhere. (removed)", pt->vnum);
      extract_trap( pt );
      continue;
    }

    /* lower duration, increase age etc. Only done if duraion */
    if (pt->duration > 0 && --pt->duration == 0){
      if (pt->owner){
	sendf(pt->owner, "You sense %s has fallen apart.\n\r", pt->name);
      }
      extract_trap( pt );
      continue;
    }
    /* check for rearm */
    if (IS_TRAP(pt, TRAP_REARM))
      pt->armed = !IS_TRAP(pt, TRAP_DELAY);

    /* rest of the update is done on NON OWNER traps only */
    if (pt->owner)
      continue;
    /* make sure its in its homeroom if on object */
    if (pt->on_obj && (pt->on_obj->in_room == NULL || 
		       pt->on_obj->homevnum != pt->on_obj->in_room->vnum))
      continue;
	
    /* check for age, and need to refresh the trap */
    if ( pArea == NULL || pt->age < 3)
      continue;
    /* Areas with flag of "NOREPOP" do not reset if player is inside */
    if (IS_SET(pArea->area_flags, AREA_NOREPOP)
	&& pArea->nplayer > 0)
      continue;
    /* now we reset the trap if needed */
/* these updates are a tad earlier then area resets since otherwise areas area always "empty"*/
    if ( (pArea->nplayer == 0 || pt->age >= 15)
	 || pt->age >= 30){
      init_trap( pt, pt->pIndexData );
    }
  }
}

/* Aggress - aggress on some random PC                    *
 * This function takes 25% to 35% of ALL Merc cpu time.   *
 * Unfortunately, checking on each PC move is too tricky, *
 *   because we don't the mob to just attack the first PC *
 *   who leads the party into the room.                   */
void aggr_update( void )
{
    int count;
    CHAR_DATA *wch, *wch_next, *ch = NULL, *ch_next, *vch, *vch_next, *victim;
    char buf[MSL];
    for ( wch = player_list; wch != NULL; wch = wch_next )
    {
        wch_next = wch->next_player;
	if ( wch->in_room == NULL || wch->in_room->area->empty || IS_SET(wch->in_room->room_flags,ROOM_SAFE))
	  continue;
	for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
	{
	  int range = IS_PERK(wch, PERK_FEARSOME) ? 3 : 5;
	  ch_next = ch->next_in_room;
	  if ( !IS_NPC(ch) 
	       || (!IS_SET(ch->off_flags, CABAL_GUARD) && !can_see( ch, wch )) 
	       || !IS_AWAKE(ch)
	       || IS_STONED(ch)
	       || IS_AFFECTED(ch, AFF_CALM) 
	       || IS_AFFECTED(ch, AFF_CHARM) 
	       || ch->fighting
	       || IS_AFFECTED2(ch, AFF_HOLD) )
	    continue;
	  /* Ok we have a 'wch' player character and a 'ch' npc aggressor. *

             * Now make the aggressor fight a RANDOM pc victim in the room,  *
             *   giving each 'vch' an equal chance of selection.             */
            if (ch->hunting != NULL 
		&& ch->hunting == wch 
		&& ch->fighting == NULL 
		&& !IS_AFFECTED(ch->hunting, AFF_SLEEP) 
		&& !IS_STONED(ch->hunting)
		&& !IS_AFFECTED2(ch->hunting,AFF_SHADOWFORM) 
		&& !IS_AFFECTED2(ch->hunting,AFF_TREEFORM) 
		&& !is_ghost(ch->hunting,600)
		&& !(IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_SKIN)
		&& !IS_SET(ch->act, ACT_NONCOMBAT))
            {
                sprintf( buf, "%s!  Now you die!",PERS(ch->hunting,ch));
                REMOVE_BIT(ch->comm,COMM_NOYELL);
                do_yell( ch, buf );
                SET_BIT(ch->comm,COMM_NOYELL);
	        ch->hunting = check_guard(ch->hunting,ch);
                multi_hit( ch, ch->hunting, TYPE_UNDEFINED );
                ch->hunting = NULL;
                break;
            }
	    /* Attack On Sight */
	    if (IS_NPC(ch) 
		&& !IS_NPC(wch) 
		&& !IS_IMMORTAL(wch)
		&& ch->race < MAX_PC_RACE
		&& ch->level + range > wch->level
		&& wch->level > 15
		&& ch->level <= 50
		&& ch->pCabal == NULL
		&& ch->pIndexData->pShop == NULL
		&& wch->pcdata->race_kills[ch->race] > AOS_LEVEL
		&& !IS_SET(ch->act, ACT_TRAIN)
		&& !IS_SET(ch->act, ACT_PRACTICE)
		&& !IS_SET(ch->act, ACT_IS_HEALER)
		&& !IS_SET(ch->act, ACT_GAIN)
		&& !IS_SET(ch->act, ACT_WIMPY)
		&& !IS_SET(ch->off_flags, GUILD_GUARD)
		&& !IS_SET(ch->off_flags, CABAL_GUARD)
		&& !IS_SET(ch->act2, ACT_LAWFUL)
		&& !IS_AREA(ch->in_room->area, AREA_CITY )
		&& !IS_AREA(ch->in_room->area, AREA_LAWFUL )
		&& !IS_AREA(ch->in_room->area, AREA_CABAL )
		&& !IS_SET(ch->act, ACT_NONCOMBAT)
		&& !IS_SET(ch->off_flags, CABAL_GUARD)
		&& number_percent() < 5
		&& !is_ghost(wch,600)
		){
	      sprintf( buf, "%s!  Your hide is mine!",PERS(wch, ch));
	      REMOVE_BIT(ch->comm,COMM_NOYELL);
	      do_yell( ch, buf );
	      SET_BIT(ch->comm,COMM_NOYELL);
	      multi_hit( ch, wch, TYPE_UNDEFINED );
	      break;
	    }
            if ((IS_SET(ch->act, ACT_AGGRESSIVE) 
		 && !IS_SET(ch->act, ACT_NONCOMBAT)
		 && !IS_SET(ch->act, ACT_WIMPY) 
		 && !IS_AFFECTED2(ch, AFF_HOLD)
		 && ch->fighting == NULL 
		 && ch->hunting == NULL 
		 && !ch->desc)
		|| (IS_SET(ch->off_flags, CABAL_GUARD)))
	      {
                count   = 0;
                victim      = NULL;
                for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
                {
		  int range = IS_PERK(vch, PERK_FEARSOME) ? 3 : 5;
		  vch_next = vch->next_in_room;
		    
                    if ( ( !IS_NPC(vch) 
			   && IS_SET(ch->act, ACT_AGGRESSIVE) 
			   && ch->level >= vch->level - range
			   && !(IS_GOOD(ch) && IS_GOOD(vch))
			   && !IS_AFFECTED(vch, AFF_SLEEP )
			   && !IS_STONED(vch)
			   && (IS_SET(ch->off_flags, CABAL_GUARD) || can_see( ch, vch ) )
			   && !is_ghost(vch,600))
			 || ( (!IS_NPC(vch) || IS_SET(vch->act, ACT_RAIDER))
			      && !IS_IMMORTAL(vch)
			      && !IS_AFFECTED(vch, AFF_SLEEP )
			      && IS_SET(ch->off_flags, CABAL_GUARD)
			      && ch->pCabal
			      && vch->pCabal
			      && !is_captured_by(vch->pCabal, ch->pCabal)
			      && is_friendly(ch->pCabal, vch->pCabal) == CABAL_ENEMY) 
			 ){
		      if ( number_range( 0, count ) == 0 )
			victim = vch;
		      count++;
                    }
                }
                if ( victim == NULL )
		  continue;
		else{
		  victim = check_guard(victim, ch);
		  if (ch->fighting != victim)
		    multi_hit( ch, victim, TYPE_UNDEFINED );
		}
	      }
        }
    }
}

/* objseed update, some objects may get receded */
void seed_update( void ){
  int pos = 0;
  char* msg;
  DESCRIPTOR_DATA *d;

  if ( (pos = objseed_lookup( OBJ_VNUM_STARSTONE )) >= 0){
    msg = "A brilliant shower of shooting stars streaks across the sky!";
    for ( d = descriptor_list; d; d = d->next ){
      if ( d->connected == CON_PLAYING && IS_AWAKE(d->character)){
	sendf( d->character, "%s\n\r", msg );
      }
    }
    seed_object( pos );
  }
}
/* MUD data updaet (html lists, cabal info etc..)  */
void data_update( void ){

  /* create who list, FALSE to not reveal hidden etc.. char */
  /* MAIN  port only */
  if (mud_data.mudport == MAIN_PORT){
/*Disabled to prevent peeking before login - Viri
    make_htmlwho(FALSE);
*/

    make_area_db();
  }
}



/* Handle all kinds of updates.                            *
 * Called once per pulse from game loop.                   *
 * Random times to defeat tick-timing clients and players. */
void update_handler( void )
{
    char buf[MSL];
    DESCRIPTOR_DATA *d, *d_next;
    static  int     pulse_area;
    static  int     pulse_mobile;
    static  int     pulse_violence;
    static  int     pulse_point;
    static  int     pulse_aggr;
    static  int     pulse_data;
    static  int     pulse_reboot;
    static  int     pulse_seed;
    static  int     pulse_msave;
    static  int     pulse_armysave;
    static  int     pulse_vote;
    static  int     pulse_cabal;
    static  int     pulse_army;

    if ( ++pulse_reboot > PULSE_REBOOT ){
      nlogf("Scheduled Reboot Reached. Rebooting in 5 ticks.");
      sprintf( buf, "`&Mud Control`8: ``Scheduled Worldshift Commencing." );
      do_echo(NULL,buf);
      do_reboot(NULL,"5");
      pulse_reboot = 0;
    }
    if ( --pulse_army   <= 0 ){ /* 15 seconds */
      pulse_army      = number_range( 5 * PULSE_ARMY / 6, 7 * PULSE_ARMY / 6 );
      init_loop("army");
      army_update	( );
      end_loop("army");
    }
    /* cabal update */
    if ( --pulse_cabal     <= 0 ) /* PULSE_CABAL   approx. 30 sec */
    {
      pulse_cabal      = PULSE_CABAL;
      init_loop("cabal");
      cabal_update( );
      end_loop("cabal");
    }
    /* vote update */
    if ( --pulse_vote     <= 0 ) /* PULSE_REBOOT / 2 approx. 2.5h */
    {
      pulse_vote      = PULSE_VOTE;
      init_loop("vote");
      vote_update	( );
      end_loop("vote");
    }
    /* seeding of certain objects */
    if ( ++pulse_seed > PULSE_SEED ){ /* 2hours (7200 sec) seconds */
      pulse_seed      = 0;
      init_loop("seed");
      seed_update	( );
      end_loop("seed");
    }
    if ( --pulse_data     <= 0 ) /* 5min (300 sec) seconds */
    {
      pulse_data      = PULSE_DATA;
      init_loop("data");
      data_update	( );
      end_loop("data");
    }
    if ( --pulse_msave     <= 0 ) /* 15min (900 sec) seconds */
    {
      pulse_msave      = PULSE_MSAVE;
      init_loop("msave");
      save_mud	( );
      end_loop("msave");
      init_loop("clan save");
      SaveClans	( );
      end_loop("clan save");
    }
    if ( --pulse_armysave     <= 0 ) /* 15min (900 sec) seconds */
    {
      pulse_armysave      = PULSE_ARMYSAVE;
      init_loop("armysave");
      save_armies( );
      //cabals marked as changed will be saved here */
      save_cabals( TRUE, NULL);
      end_loop("armysave");
    }
    if ( --pulse_area     <= 0 ) /* 60 seconds */
    {
        pulse_area      = number_range( 5 * PULSE_AREA / 6, 7 * PULSE_AREA / 6 );
	init_loop("area");
	area_update	( );
	end_loop("area");
    }
    if ( --pulse_mobile   <= 0 ) /* 10 seconds */
    {
	pulse_mobile	= PULSE_MOBILE;
	init_loop("mobile");
	mobile_update	( );
	end_loop("mobile");
    }
    if ( --pulse_violence <= 0 ) /* 3 seconds */
    {
	pulse_violence	= PULSE_VIOLENCE;
	init_loop("violence");
	violence_update	( );
	end_loop("violence");
    }
    if ( --pulse_point    <= 0 ) /* 30 seconds */
    {
	init_loop("timer");
        for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;
            if ( d != NULL 
		 && (d->connected == CGEN_MAIN 
		     || d->connected == CGEN_ENTER 
		     || d->connected == CGEN_CREATE)
		 && d->timer++ > PULSE_LOGIN )
	      {
		write_to_descriptor( d->descriptor, "\n\r*** Login timed out ***\n\r", 0 );
		nlogf("disconnecting socket %d",d->descriptor);
		close_socket( d );
		nlogf("socket closed");
	      }
	}
	end_loop("timer");
	wiznet("TICK!",NULL,NULL,WIZ_TICKS,0,0);
        pulse_point     = number_range( 5 * PULSE_TICK / 6 , 7 * PULSE_TICK / 6 );
        reboot_tick_counter--;
        if (reboot_tick_counter>0)
        {
            sprintf( buf, "[`&The Voice of God`7]`8 - World shift in %d hour%s.``", reboot_tick_counter, reboot_tick_counter == 1 ? "" : "s");
            do_echo(NULL,buf);
        }
        else if (!reboot_tick_counter)
            do_reboot(NULL, "0");
	init_loop("weather");
	weather_update	( );
	end_loop("weather");
	init_loop("char");
	char_update	( );
	end_loop("char");
	init_loop("obj");
	obj_update	( );
	end_loop("obj");
	init_loop("trap");
	trap_update	( );
	end_loop("trap");
	init_loop("memory");
	if (sAllocString > 4*MAX_STRING/5 && reboot_tick_counter < 0)
	{
	    nlogf("Shared string usage is %d of %d.  Rebooting in a few ticks.", sAllocString, MAX_STRING * 4 / 5);
            sprintf( buf, "`&Mud Control`8: ``Shared strings near max allowed.  Rebooting soon." );
            do_echo(NULL,buf);
	    do_reboot(NULL,"5");
	}
	if (nAllocPerm > 4 * MAX_PERM_BLOCK / 5 && reboot_tick_counter < 0)
	{
	    nlogf("Perm block usage is %d of %d.  Rebooting in a few ticks.", nAllocPerm, MAX_PERM_BLOCK * 4 / 5);
            sprintf( buf, "`&Mud Control`8: ``Perm blocks near max allowed.  Rebooting soon." );
            do_echo(NULL,buf);
	    do_reboot(NULL,"5");
	}
	end_loop("memory");
    }
    if ( --pulse_aggr   <= 0 ) /* 0.5 seconds */
    {
	pulse_aggr	= PULSE_AGGR;
        init_loop("aggr");
        aggr_update( );
        end_loop("aggr");
    }
    if (reboot_tick_counter < -1)
      reboot_tick_counter = -1;
    tail_chain( );
}

/* Called in violence_update() of fight.c */
void update_violence(CHAR_DATA* ch)
{
//  AFFECT_DATA* paf;

  /* mounts */
  if (IS_NPC(ch)
      && ch->summoner 
      && !IS_NPC(ch->summoner)
      && ch->summoner->pcdata->pStallion != NULL 
      && ch->summoner->pcdata->pStallion == ch->pIndexData
      && ch->summoner->in_room != ch->in_room)
    {
      send_to_char("You dismount.\n\r", ch->summoner);
      ch->summoner->pcdata->pStallion = NULL;
    }
  if (  VIOLENCE_CHECK (ch) == -1)
    bug("Error at: VIOLENCE_CHECK returned failure.\n\r", 0);
  if ( ch == NULL || ch->in_room == NULL )
    return;

/* undead fear effect */
  if (ch->fighting && !IS_NPC(ch->fighting) && IS_UNDEAD(ch->fighting)
      && ch->fighting->race == race_lookup("undead") ){
    CHAR_DATA* victim = ch->fighting;
    int chance = 10 + (victim->pcdata->kpc / 3);
    if (number_percent() < chance 
	&& !saves_spell( victim->level, ch, DAM_MENTAL, SPELL_MENTAL ) 
	&& !is_affected(ch, gsn_fear) ){
      AFFECT_DATA af;
      //we scare them.
      if (is_affected(ch, gsn_ptrium)){
	send_to_char("Your divine strength resists the unholy fear.\n\r", ch);
	act("$n seems to ignore the unholy fear.", ch, NULL, NULL, TO_ROOM);
      }
      else{
	send_to_char("You are overpowered with intense fear.\n\r", ch);
	act("$n's eyes widen in fear!", ch, NULL, NULL, TO_ROOM);
	
	/* attach effect to hurt skills */
	af.type = gsn_fear;
	af.level = victim->level;
	af.duration = number_range(0,1);
	af.where = TO_AFFECTS;
	af.bitvector = 0;
	af.location = APPLY_NONE;
	af.modifier = 20;
	affect_to_char(ch, &af);
      }
    }
  }

//DARK RIFT DAMAGE
  if (ch->in_room != NULL 
      && (IS_SET(ch->in_room->room_flags, ROOM_DARK_RIFT)
	  || IS_SET(ch->in_room->room_flags, ROOM_DAMAGE))
      && (!IS_NPC(ch) || ch->homevnum != ch->in_room->vnum)
      && !IS_IMMORTAL(ch)){
      damage(ch,ch,number_range(ch->level / 10, ch->level / 5), 1051, DAM_INTERNAL,TRUE);
      if ( ch == NULL || ch->in_room == NULL )
	return;
  }

    
  //low mana for use of blademaster stances
  if (!IS_NPC(ch) && ch->class == gcn_blademaster && ch->fighting){
    AFFECT_DATA* paf;
    for (paf = ch->affected; paf; paf = paf->next){
      if (paf->type == gsn_deathweaver
	  || paf->type == gsn_doomsinger
	  || paf->type == gsn_shadowdancer
	  || paf->type == gsn_puppet_master
	  || paf->type == gsn_bladestorm
	  || paf->type == gsn_ironarm
	  || paf->type == gsn_iron_curtain){
	if (ch->mana < 1){
	  check_dispel(-99, ch, paf->type);
	  break;
	}
	else
	  ch->mana = UMAX(0, ch->mana - 2);
      }
    }
  }

//Poson damage
  poison_spell_effect(ch);
  if ( ch == NULL || ch->in_room == NULL )
    return;
  /* jailcell justice update (prevents sitting in jail if no gen_jail)*/
  jail_abuse_check(ch, TRUE);

  if (!IS_NPC(ch) && ch->hit * 100 / (1 + ch->max_hit) < 30)
    check_contingency(ch, NULL, CONT_HP30);
  if (!IS_NPC(ch) && ch->mana * 100 / (1 + ch->max_mana) < 30)
    check_contingency(ch, NULL, CONT_MN30);

  /* memorization */
  dnds_memorize_update( ch );
}

//04-2 20:00 Viri: Added do_embrace_Damage(..)
//04-02 22:00 Viri: Added do_unlife(..)
//04-23 21:00 Viri: Changed damage from hungers plauge poison etc.. to DAM_NONE so it would go through sanc and mana shield.
//05-10-00 Viri: Mdified gain_exp to restric lvling without deity.
//07-12-00 Viri: Added update violence for rare, often made updates.
//12-02-00 Ath: Added code so that druids get perm "yearning" on them
