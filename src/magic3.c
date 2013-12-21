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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "interp.h"

extern char *target_name;
extern bool effect_manalock(int sn, int level, CHAR_DATA* ch, CHAR_DATA* victim);
#define		WEP_NAMED	(dd)

void spell_mind_thrust( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA* victim = (CHAR_DATA *) vo;
    int dam = dice(level / 5, 5) + 2 * level;

    if (is_affected(ch, gsn_mindsurge))
      dam = 50 + 2 * level;

    if (level < 15
	|| (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
	    && saves_spell(level, victim, DAM_MENTAL, skill_table[sn].spell_type))
	){
      dam /= 2;
    }
    else{
      AFFECT_DATA af;

      af.where		= TO_AFFECTS;
      af.type		= sn;
      af.level		= ch->level;
      af.duration	= number_range(1, 2);
      af.location	= APPLY_SAVING_MENTAL;
      af.modifier	= number_range(1, 4);
      af.bitvector	= 0;
      affect_join(victim,&af);
      act_new( "You become less confident.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
      if ( ch != victim )
	act("$N becomes less confident.",ch,NULL,victim,TO_CHAR);
    }
    damage( ch, victim, dam, sn, DAM_MENTAL, TRUE);
}

void spell_mind_disruption( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = dice(level / 5, 5) + 2 * level;

    if (is_affected(ch, gsn_mindsurge))
      dam = 50 + 2 * level;

    if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
	&& saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type))
      dam /= 2;
    else if (dam > 1){
    //make it harder to concentrate on spells
      AFFECT_DATA* paf, af;
      if  ( (paf = affect_find(victim->affected, gsn_mind_disrupt)) == NULL
	    || paf->modifier > -60){
	af.type		= gsn_mind_disrupt;
	af.level		= ch->level;
	af.duration		= 1;
	af.where		= TO_AFFECTS;
	af.bitvector		= 0;
	af.location		= APPLY_NONE;
	af.modifier		= -20;
	affect_join( victim, &af );
	act("You disrupt $N's ability to concentrate.", ch, NULL, victim, TO_CHAR);
	act("$n disrupts your ability to concentrate.", ch, NULL, victim, TO_VICT);
      }
    }
    damage( ch, victim, dam, sn, DAM_MENTAL,TRUE);
}

void spell_mind_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = dice(level / 5, 5) + 3 * level;

    if (is_affected(ch, gsn_mindsurge))
      dam = 50 + 3 * level;

    if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
	&& saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type))
      dam /= 2;
    victim->mana = UMAX(0, victim->mana - dam / 2 );
    damage( ch, victim, dam, sn, DAM_MENTAL,TRUE);
}

void spell_psychic_crush( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = dice(4, 3) + level * 2;
    if (saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type))
	dam /= 2;
    if (!IS_SET(victim->act, ACT_UNDEAD) && number_percent() < level)
      {
	send_to_char("You feel you psyche slip.\n\r", victim);
	act("$N's eyes glaze over momentarly.", victim, NULL, victim, TO_ROOM);
	victim->mana = UMAX(0, victim->mana - dam / 2);
      }
    damage( ch, victim, dam, sn, DAM_MENTAL,TRUE);
}

void spell_psionic_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  CHAR_DATA *vch, *vch_next;
  int dam = 0;
  
  act("You send a blast of mental energy through the room.",ch,NULL,NULL,TO_CHAR);
  act("A blast of mental energy surges through the room.",ch,NULL,NULL,TO_ROOM);

  for (vch = ch->in_room->people; vch; vch = vch_next){
    vch_next = vch->next_in_room;
    
    if ( vch == ch || is_area_safe_quiet(ch,vch))
      continue;
    
    dam = dice(level / 5, 10) + 3 * level;
    if (is_affected(ch, gsn_mindsurge))
      dam = 100 + 3 * level;
    
    if (psiamp_failed(ch, vch, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
	&& saves_spell(level, vch, DAM_MENTAL,skill_table[sn].spell_type))
      dam /= 2;
    else if (!IS_UNDEAD(vch)){
      /* paralyze */
      if (!is_affected(vch, gsn_paralyze)){
	AFFECT_DATA af;
	act("$n has been paralyzed!", vch, NULL, NULL, TO_ROOM);
	act("You have been paralyzed!", vch, NULL, NULL, TO_CHAR);
	
	af.type      = gsn_paralyze;
	af.level     = level;
	af.duration  = number_range(1, 3);
	af.where     = TO_AFFECTS;
	af.bitvector = 0;
	af.location  = APPLY_NONE;
	af.modifier  = 3 * af.duration;
	affect_to_char( vch, &af );
      }
    }
    m_yell(ch,vch,FALSE);    
    damage( ch, vch, dam, sn, DAM_MENTAL,TRUE);
  }
}

/* mental spells do more damage */
void spell_psychic_purge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn )
	 || check_immune(victim, DAM_MENTAL, FALSE) == IS_VULNERABLE)
      {
	if (victim == ch)
	  act("You are already vulnerable to mental attacks.",ch,NULL,victim,TO_CHAR);
	else
	  act("$N is already vulnerable to mental attacks.",ch,NULL,victim,TO_CHAR);
	return;
      }

    if (IS_UNDEAD(victim)
	|| (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
	    && saves_spell( level, victim, DAM_MENTAL,skill_table[sn].spell_type))
	){
      act("$N resists your efforts.", ch, NULL, victim, TO_CHAR);
      return;
    }

    af.where	= TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/5;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You are now more vulnerable to mental damage.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
      act("$N is now vulnerable to mental damage.",ch,NULL,victim,TO_CHAR);
}

void spell_photon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  AFFECT_DATA af;

  if (is_affected(ch, gen_photon_wall)){
    send_to_char("You are already shielded.\n\r", ch);
    return;
  }

  af.where	= TO_AFFECTS;
  af.type      = gen_photon_wall;
  af.level     = level;
  af.duration  = 24;
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );
}


void spell_flesh_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ))
    {
	act("Your flesh has already been hardened.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where	= TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 2;
    if (ch->class == class_lookup("psionicist"))
      af.modifier  = -25 - level / 2;
    else
      af.modifier  = -20;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You cause your flesh to numb and thicken.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n's flesh becomes inanimate.",victim,NULL,NULL,TO_ROOM);
}

void spell_detect_aura( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ))
    {
	act("You can already detect auras.",ch,NULL,victim,TO_CHAR);
	return;
    }
    affect_strip(victim, skill_lookup("detect good"));
    affect_strip(victim, skill_lookup("detect evil"));
    affect_strip(victim, skill_lookup("detect magic"));
    affect_strip(victim, skill_lookup("detect invis"));

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level > 25 ? 48 : 24;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_DETECT_GOOD;
    affect_to_char( victim, &af );
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_DETECT_EVIL;
    affect_to_char( victim, &af );
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_DETECT_MAGIC;
    affect_to_char( victim, &af );
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_DETECT_INVIS;
    affect_to_char( victim, &af );
    act_new( "You can now detect auras.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
}

void spell_displacement( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ))
    {
	act("Your image is already projected away from your body.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where	= TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 8 + level;
    af.modifier  = -(10 + level);
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You project your image away from your body.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n seems to move away from where you thought $e was.",victim,NULL,NULL,TO_ROOM);
}

void spell_awaken( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    if ( ( victim = get_char_world( ch, target_name ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r",ch );
        return;
    }
    if (is_affected(ch, sn)){
      send_to_char("You are too tired for another attempt.\n\r", ch);
      return;
    }
    if ( IS_AWAKE(victim) )
    {   
      act("They are already awake.",ch,NULL,victim,TO_CHAR);
      return;
    }
    if (IS_UNDEAD(victim))
    {
        sendf(ch, "You can't awaken %s!\n\r", PERS(victim,ch));
        return;
    }
    act_new( "A voice in your head commands you to wake up.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    act("You have awakened $N.",ch,NULL,victim,TO_CHAR);
    affect_strip(victim, gsn_sleep);
    do_stand(victim,"");

    af.type = sn;
    af.level = level;
    af.duration = number_fuzzy(4);
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.modifier = 0;
    af.location = APPLY_NONE;
    affect_to_char(ch, &af);
}

void spell_mental_drain( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA* victim = (CHAR_DATA *) vo;
    int dam = UMAX(25, victim->mana / 5);
    if ( psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
	 && saves_spell( level, victim, DAM_MENTAL,skill_table[sn].spell_type) ){
      
      act("You failed to drain $N's mental power.",ch,NULL,victim,TO_CHAR);
      act_new("You feel something invading your mind.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
      return;
    }
    if (victim->mana < 1){
      send_to_char("You only sense emptiness.\n\r", ch);
      return;
    }

    ch->mana = UMIN(ch->max_mana, ch->mana + dam);
    victim->mana -= dam;
    victim->mana = UMAX(0, victim->mana);

    act_new("You feel your mental powers slipping away!",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    act("You have absorbed $N's mental powers!",ch,NULL,victim,TO_CHAR);
}

void spell_iron_will( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ))
    {
	act("Your self-control is already as strong as it can be.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where	= TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = ch->level/2;
    af.modifier  = -level/15;
    af.location  = APPLY_SAVING_SPELL;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "Your self-control increases.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
}

void spell_cell_adjustment( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA* victim = (CHAR_DATA *) vo;
    int chance = IS_NPC(ch) ? 100 : get_skill(ch,sn) - 25;
    send_to_char("Your body begins to right itself.\n\r", victim);
    if (is_affected(victim,gsn_blindness) && number_percent() < chance)
    {
        affect_strip(victim,gsn_blindness);
	send_to_char("Your can see again.\n\r",victim);
    }
    if (!IS_NPC(ch) && is_affected(victim,gsn_death_grasp) && number_percent() < chance)
    {
        affect_strip(victim,gsn_death_grasp);
	send_to_char("You regain some of your vitality.\n\r",victim);
    }
     if (is_affected(victim,gsn_poison) && number_percent() < chance)
    {
        affect_strip(victim,gsn_poison);
	send_to_char("You feel less sick.\n\r",victim);
    }
     if (is_affected(victim,gsn_plague) && number_percent() < chance)
    {
        affect_strip(victim,gsn_plague);
	send_to_char("Your sores vanish.\n\r",victim);
    }
     if (!IS_NPC(ch) && is_affected(victim,skill_lookup("dysentery")) && number_percent() < chance)
    {
        affect_strip(victim,skill_lookup("dysentery"));
	send_to_char("Your bowel movements returns to normal.\n\r",victim);
    }
     if (is_affected(victim,gsn_enfeeblement) && number_percent() < chance)
    {
        affect_strip(victim,gsn_enfeeblement);
	send_to_char("Your sores vanish.\n\r",victim);
    }
     if (is_affected(victim,gsn_embrace_poison) && number_percent() < chance)
       affect_strip(victim,gsn_embrace_poison);

}

void spell_bio_manipulation( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA* victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int type = ch->fighting ? number_range(0,3) : number_range(0,2);
    if (IS_UNDEAD(victim))
      {
	send_to_char("Strange, but the cells are totaly inert, almost as if dead...\n\r", ch);
	return;
      }

    switch (type)
    {
    case (0):
    	af.where		= TO_AFFECTS;
        af.type                 = gsn_gag;
        af.level                = ch->level;
        af.duration             = number_range(0,2);
        af.location             = 0;
        af.modifier             = 0;
        af.bitvector            = 0;
        affect_join(victim,&af);
	act_new( "Your vocal cords grow inanimate.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
	if ( ch != victim )
	    act("You impare $N's vocal cords.",ch,NULL,victim,TO_CHAR);
	break;
    case (1):
    	af.where		= TO_AFFECTS;
        af.type                 = gsn_silence;
        af.level                = ch->level;
        af.duration             = number_range(0,2);
        af.location             = 0;
        af.modifier             = 0;
        af.bitvector            = 0;
        affect_join(victim,&af);
	act_new( "Your hearing weakens.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
	if ( ch != victim )
	    act("You impare $N's ear drums.",ch,NULL,victim,TO_CHAR);
	break;
    case (2):
	af.where		= TO_AFFECTS;
        af.type                 = gsn_blindness;
        af.level                = ch->level;
        af.duration             = number_range(0,2);
        af.location             = 0;
        af.modifier             = 0;
        af.bitvector            = AFF_BLIND;
        affect_join(victim,&af);
	act_new( "Your vision begins to fail.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
	if ( ch != victim )
	    act("You impare $N's optic nerves.",ch,NULL,victim,TO_CHAR);
	break;
    case (3):
	WAIT_STATE(victim, 12 * number_range(1, 3) );
	act_new( "Your motor functions stop working.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
	if ( ch != victim )
	    act("You halt $N's motor functions.",ch,NULL,victim,TO_CHAR);
	break;
    }
}

void spell_ego_whip( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dam = number_fuzzy(level);
    int svs_mental = ch->savingspell + ch->savingmental;

    af.where		    = TO_AFFECTS;
    af.type                 = gsn_ego_whip;
    af.level                = ch->level;
    af.duration             = number_range(2,3);
    af.location             = APPLY_SAVING_MENTAL;
    af.modifier             = -2 * svs_mental / 10;
    af.bitvector            = 0;
    affect_join(victim,&af);
    act_new( "You become less confident.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
      act("$N becomes less confident.",ch,NULL,victim,TO_CHAR);
    damage( ch, victim, dam, sn, DAM_MENTAL,TRUE);
}

void spell_tele_lock( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (is_safe(ch,victim) && victim != ch){
      sendf(ch, "Not on %s.\n\r",PERS(victim,ch));
    return;
    }
    if ( is_affected(victim, gsn_tele_lock))
    {
	if (victim == ch)
	    act("You are already locked down.",ch,NULL,victim,TO_CHAR);
	else
	    act("$N is already locked down.",ch,NULL,victim,TO_CHAR);
        return;
    }
    else if (ch == victim){
      send_to_char("You failed.\n\r", ch);
      return;
    }
    if (!psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
	|| !saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type))
    {
    	af.where	= TO_AFFECTS;
        af.type                 = gsn_tele_lock;
        af.level                = ch->level;
        af.duration             = ch->level / 4;
        af.location             = 0;
        af.modifier             = 0;
        af.bitvector            = 0;
	affect_to_char( victim, &af );
	act_new( "You are now locked down",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
	if ( ch != victim )
	    act("$N is now locked down.",ch,NULL,victim,TO_CHAR);
    }
    else
      send_to_char("You failed.\n\r", ch);
}

void spell_enhance_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int fail = 50, result, i;
    char buf[MSL];
    if (obj->item_type != ITEM_ARMOR)
    {
        send_to_char("That isn't an armor.\n\r",ch);
        return;
    }   
    if (obj->wear_loc != -1)
    {
        send_to_char("The item must be carried to be enchanted.\n\r",ch);
        return;
    }
    if (IS_OBJ_STAT(obj,ITEM_PSI_ENHANCE))
    {
        send_to_char("This item is already psionically altered.\n\r",ch);
        return;
    }
    if (obj->value[0] == 0 && obj->value[1] == 0 && obj->value[2] == 0 && obj->value[3] == 0)
    {
        send_to_char("This item can not be psionically enhanced at all.\n\r",ch);
        return;
    }
    fail += obj->level;
    fail -= level;
    if (IS_OBJ_STAT(obj,ITEM_BLESS))
        fail -= 10;
    if (IS_OBJ_STAT(obj,ITEM_MAGIC))
        fail += 25;
    fail -= get_curr_stat(ch,STAT_LUCK) - 16;
    fail = URANGE(5,fail,85);   
    result = number_percent();
    SET_BIT(obj->extra_flags, ITEM_MAGIC);
    if (result < (fail / 5) && !IS_IMMORTAL(ch))
    {
        act("$p glows red hot and melts!",ch,obj,NULL,TO_ALL);
        sprintf( buf, "lump %s", obj->material);
        free_string( obj->name );
        obj->name = str_dup( buf );
        sprintf( buf, "A lump of %s", obj->material);
        free_string( obj->short_descr );
        obj->short_descr = str_dup( buf );
        sprintf( buf, "A lump of %s lies here.", obj->material);
        free_string( obj->description );
        obj->description = str_dup( buf );
	obj->wear_flags = ITEM_TAKE + ITEM_HOLD;
	for (i = 0; i < 4; i++)
	    obj->value[i] = 0;
        return;
    }
    if (result < (fail / 3) && !IS_IMMORTAL(ch))
    {
      SET_BIT(obj->extra_flags, ITEM_PSI_ENHANCE);
      act("$p becomes mis-shapened.",ch,obj,NULL,TO_ALL);
      for (i = 0; i < 4; i++)
	obj->value[i] = 0;
      obj->condition = 0;
      return;
    }
    if ( result <= fail && !IS_IMMORTAL(ch))
    {
        send_to_char("The molecules stir for a second but stop.\n\r",ch);
        return;
    }
    SET_BIT(obj->extra_flags, ITEM_PSI_ENHANCE);
    act("$p is enhanced to perfection!",ch,obj,NULL,TO_ALL);
    for (i = 0; i < 4; i++){
      obj->value[i] = UMIN(20, obj->value[i] + obj->value[i] * level / 100);
    }
}

void spell_there_not_there( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  OBJ_DATA *obj = (OBJ_DATA*) vo;
  AFFECT_DATA af;

  if ((!CAN_WEAR(obj, ITEM_TAKE)
       || (CAN_WEAR(obj, ITEM_NO_SAC) && obj->timer == 0)
       || obj->item_type == ITEM_CABAL 
       || obj->wear_flags == 0)
      && obj->timer == 0//time is here so requested items can be affected.
      //(request items will have above flags, but times is != 0)
      ){
    send_to_char("Not on that object!\n\r", ch);
    return;
  }
  
  if (is_affected_obj(obj, sn))
    {
        send_to_char("This item is already phased out.\n\r",ch);
        return;
    }
    if (is_affected(ch, sn))
      {
	send_to_char("You are not ready for another attempt.\n\r", ch);
	return;
      }

    if (obj->item_type == ITEM_CORPSE_PC
	|| obj->pIndexData->vnum == get_temple_pit(ch->in_room->vnum)){
      send_to_char("Not on that object.\n\r", ch);
      return;
    }

    af.type       = sn;
    af.level      = level;
    af.duration   = level / 4;
    af.location   = 0;
    af.modifier   = 0;
    af.where = TO_AFFECTS;
    af.bitvector  = 0;
    affect_to_char(ch, &af);
    affect_to_obj(obj, &af);

    act("You phase $p out of this dimension.", ch, obj, NULL, TO_CHAR);
    act("You see something disappear from $n's hands.", ch, NULL, NULL, TO_ROOM);
}

void spell_phase_shift( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    ROOM_INDEX_DATA *room;
    if (!IS_NULLSTR(target_name )){
      spell_mind_store( sn, level, ch, vo, target );
      return;
    }
    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL)
    {
      sendf(ch, "You cannot shift while on %s!\n\r",ch->pcdata->pStallion->short_descr);
      return;
    }
    if (ch->pcdata->beacon == 0 || (room = get_room_index(ch->pcdata->beacon)) == NULL)
	room = get_room_index(get_temple(ch));
    if ( IS_SET(room->room_flags, ROOM_SAFE)
    || IS_SET(room->room_flags, ROOM_PRIVATE)
    || IS_SET(room->room_flags, ROOM_SOLITARY)
    || IS_SET(room->room_flags, ROOM_NO_GATEIN)
    || IS_SET(ch->in_room->room_flags, ROOM_NO_GATEOUT)
    || IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)
    || is_affected(ch, gsn_tele_lock)
    || IS_SET(room->room_flags, ROOM_NO_INOUT))
    {
        send_to_char("You failed the phase shift.\n\r",ch);
        return;
    }
    else if (is_affected(ch,gen_ensnare)){
      send_to_char("You sense a powerful magical field preventing your departure.\n\r", ch);
      return;
    }
    act("$n closes $s eyes and vanishes.",ch,NULL,NULL,TO_ROOM);
    act("You close your eyes and vanish.",ch,NULL,NULL,TO_CHAR);
    char_from_room(ch);
    char_to_room(ch,room);
    act("$n suddenly appears and opens $s eyes.",ch,NULL,NULL,TO_ROOM);
    do_look(ch,"auto");
}

void spell_mind_store( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    char arg[MSL];
    one_argument(target_name, arg);
    if (arg[0] != '\0' && !str_cmp(arg, "none"))
    {
	ch->pcdata->beacon = 0;
	send_to_char("You empty your mind.\n\r",ch);
	return;
    }
    else
    {
        if (ch->in_room == NULL)   
            return;
        if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE)
        || IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
        || IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
        || IS_SET(ch->in_room->room_flags, ROOM_NO_GATEIN)
        || IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT))
        {
            send_to_char("You failed to memorize this room.\n\r",ch);
            return;
        }
        send_to_char("You memorize this room.\n\r",ch);
        ch->pcdata->beacon = ch->in_room->vnum;
    }
}

void spell_levitate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  CHAR_DATA *victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;

  if ( is_affected( victim, gen_levitation )){
    send_to_char("You are already levitated.\n\r", ch);
    return;
  }
  af.type      = gen_levitation;
  af.level     = level;
  af.duration  = URANGE(12, level - 2, 48);
  af.where	 = TO_AFFECTS;
  af.bitvector = AFF_FLYING;
  af.modifier  = APPLY_NONE;
  af.location  = 0;
  
  if (!IS_AFFECTED(ch, AFF_FLYING)){
    act_new( "Your feet rise off the ground.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n's feet rise off the ground.",victim,NULL,NULL,TO_ROOM);
  }
  affect_to_char( victim, &af );

}


void do_wish( CHAR_DATA* ch, char* argument){
  AFFECT_DATA* paf;
  CHAR_DATA* vch;
  const int dur = 24;
  const int gsn_wish = skill_lookup("wish");
  if ( (paf = affect_find(ch->affected, gsn_wish)) == NULL
       || paf->modifier){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_NULLSTR(argument)){
    send_to_char("You may wish for FOOD, TRAINING, PROTECTION, or HEALING.\n\r", ch);
    return;
  }
  else if (!str_prefix(argument, "food")){
    act("The magical flames roar with power as $n wishes for food.", ch, NULL, NULL, TO_ROOM);
    act("The magical flames roar with power as you wish for food.", ch, NULL, NULL, TO_CHAR);
    paf->modifier = TRUE;//completed wish
    paf->duration = dur;

    for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
      if (IS_NPC(vch) || !is_same_group(ch, vch))
	continue;
      if (vch->pCabal && IS_CABAL(vch->pCabal, CABAL_NOMAGIC))
	continue;
      send_to_char("You feel all traces of hunger and thirst disappear.\n\r", vch);
      vch->pcdata->condition[COND_HUNGER]  = 100;
      vch->pcdata->condition[COND_THIRST]  = 100;
    }
    WAIT_STATE2(ch, 2 * PULSE_VIOLENCE );
    return;
  }
  else if (!str_prefix(argument, "training")){
    CHAR_DATA* trainer;
    AFFECT_DATA af;
    act("The magical flames roar with power as $n wishes for training.", ch, NULL, NULL, TO_ROOM);
    act("The magical flames roar with power as you wish for training.", ch, NULL, NULL, TO_CHAR);
    paf->modifier = TRUE;//completed wish
    paf->duration = dur;
    trainer = create_mobile(get_mob_index(MOB_VNUM_WISH_TRAINER));
    char_to_room(trainer, ch->in_room);
    af.type = gsn_timer;
    af.level = paf->level;
    af.duration = 3;
    af.where = TO_NONE;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_char(trainer, &af);

    act("$n suddenly appears before you.", trainer, NULL, NULL, TO_ROOM);
    do_say(trainer, "What do you wish to practice or train?");
    WAIT_STATE2(ch, 2 * PULSE_VIOLENCE );
    return;
  }
  else if (!str_prefix(argument, "protection")){
    int sn = 0;
    act("The magical flames roar with power as $n wishes for protection.", ch, NULL, NULL, TO_ROOM);
    act("The magical flames roar with power as you wish for protection.", ch, NULL, NULL, TO_CHAR);
    paf->modifier = TRUE;//completed wish
    paf->duration = dur;

    /* select random spell */
    switch (number_range(1, 4)){
    case 1: sn = skill_lookup("armor");		break;
    case 2: sn = skill_lookup("shield");	break;
    case 3: sn = skill_lookup("stone skin");	break;
    default: sn = gsn_sanctuary;		break;
    }

    for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
      if (IS_NPC(vch) || !is_same_group(ch, vch))
	continue;
      if (vch->pCabal && IS_CABAL(vch->pCabal, CABAL_NOMAGIC))
	continue;
      (*skill_table[sn].spell_fun) ( sn, 60, vch, vch, 0);
    }
    WAIT_STATE2(ch, 2 * PULSE_VIOLENCE );
    return;
  }
  else if (!str_prefix(argument, "healing")){
    act("The magical flames roar with power as $n wishes for healing.", ch, NULL, NULL, TO_ROOM);
    act("The magical flames roar with power as you wish for healing.", ch, NULL, NULL, TO_CHAR);
    paf->modifier = TRUE;//completed wish
    paf->duration = dur;

    for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
      if (IS_NPC(vch) || !is_same_group(ch, vch))
	continue;
      if (vch->pCabal && IS_CABAL(vch->pCabal, CABAL_NOMAGIC))
	continue;
      if (vch->hit > 4 * vch->max_hit / 5){
	send_to_char("You lack any serious wounds to heal.\n\r", vch);
      }
      else{
	act("Some of $n's wounds disappear.", vch, NULL, NULL, TO_ROOM);
	send_to_char("Some of your wounds disappear.\n\r", vch);
	vch->hit = UMAX(4 * vch->max_hit / 5, vch->hit + vch->max_hit / 3);
	update_pos( vch );
      }

      if (number_percent() < 50){
	if (IS_AFFECTED(vch, AFF_BLIND)){
	  check_dispel(paf->level,vch,gsn_blindness);
	}	
	if (IS_AFFECTED(vch,AFF_POISON)){
	  check_dispel(paf->level,vch,gsn_poison);
	}
	if (IS_AFFECTED(vch, AFF_PLAGUE)){
	  check_dispel(paf->level,vch,gsn_plague);
	}
	if (IS_AFFECTED(vch, AFF_CURSE)){
	  check_dispel(paf->level,vch,gsn_curse);
	}
      }
    }
    WAIT_STATE2(ch, 2 * PULSE_VIOLENCE );
    return;
  }
  else
    do_wish( ch, "");
}


  
/* first part of the do_wish */
void spell_wish( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  CHAR_DATA *victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;

  if ( is_affected( victim, sn )){
    send_to_char("You are not ready to make another wish.\n\r", ch);
    return;
  }
  if (number_percent() > get_skill(ch, sn)){
    int dam = number_range(150, 250);
   send_to_char("Your self control slips for a moment!\n\r", ch );
   act("$n's self control slips for a moment!\n\r", ch, NULL, NULL, TO_ROOM);
   damage(ch, ch, dam, sn, DAM_INTERNAL, TRUE);
   return;
  }

  af.type      = sn;
  af.level     = level;
  af.duration  = 3;
  af.where     = TO_AFFECTS;
  af.bitvector = 0;
  af.modifier  = APPLY_NONE;
  af.location  = FALSE;//wish not made yet
  affect_to_char( victim, &af );
  act("A ring of blue flames erupts around $n.", ch, NULL, NULL, TO_ROOM );
  act("A ring of blue flames erupts around you.", ch, NULL, NULL, TO_CHAR );
  act("A voice says '`#I await your wish $n.``'", ch, NULL, NULL, TO_ALL );
  send_to_char("Use the \"wish\" command to complete your wish.\n\r", ch );
}

void spell_gravitate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected_obj( obj, sn))
    {
	send_to_char("That object is already gravitated.\n\r", ch);
	return;
    }
    if ( is_affected_obj( obj, gsn_levitate))
    {
	act("$p falls to the ground.",ch,obj,NULL,TO_CHAR);
	affect_strip_obj(obj, gsn_levitate);
	return;
    }

    if ( (!CAN_WEAR(obj, ITEM_TAKE)
	  || CAN_WEAR(obj, ITEM_NO_SAC)
	  || obj->weight > 1000
	  || obj->wear_flags == 0
	  || obj->item_type == ITEM_CABAL)
	 && obj->timer == 0
	  //time is here so requested items can be affected.
	  //(request items will have above flags, but times is != 0)
	 )
	{
	  send_to_char("The spell seems to fail.\n\r", ch);
	  return;
	}
    act("$p seems to gain weight and sinks into the ground.", ch, obj, NULL, TO_ALL);
    af.type       = sn;
    af.level      = level;  
    af.duration   = level;
    af.location   = 0;
    af.modifier   = 0;
    af.bitvector  = 0;
    affect_to_obj(obj, &af);
}

void spell_bio_feedback( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ))
    {
	act("You are already controlling your blood flow.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where	= TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 8 + level;
    af.modifier  = -30;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You control your blood flow.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
}

void spell_psychic_healing( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal = dice(3, 8) + level;
    victim->hit = UMIN( victim->hit + heal, victim->max_hit );
    update_pos( victim );
    act_new("You heal yourself.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
}

void spell_pyramid_of_force( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ))
    {
      act("You are already surrounded by a pulsating pyramid.",ch,NULL,victim,TO_CHAR);
      return;
    }
    if (is_affected(victim, gsn_sacred_runes)){
      send_to_char("The vile runes seem to negate the effect.\n\r", ch);
      return;
    }
    if (is_affected(victim, gsn_lifeforce))
      {
        if (victim == ch)
	  act("Not with a lifeforce surrounding you!",ch,NULL,victim,TO_CHAR);
        else
	  act("$N is already surrounded by a lifeforce.",ch,NULL,victim,TO_CHAR);
        return;
      } 
    if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
      {
	if (victim == ch)
	  act("You are already in sanctuary.",ch,NULL,victim,TO_CHAR);
	else
	  act("$N is already in sanctuary.",ch,NULL,victim,TO_CHAR);
	return;
      }
    af.where	 = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 12;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You are surrounded by a pulsating pyramid.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n is surrounded by a pulsating pyramid.",victim,NULL,NULL,TO_ROOM);
}

void spell_terror( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (ch == victim)
    {
	send_to_char("You scare yourself.\n\r",ch);
	return;
    }

    if ( is_affected(victim, gsn_fear)){
      act("$N is already cowering in fear.",ch,NULL,victim,TO_CHAR);
      return;
    }
    
    if (saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)
	|| IS_SET(victim->act, ACT_UNDEAD)){
      act_new( "You feel scared for a second.",ch, NULL, victim, TO_CHARVICT, POS_DEAD);
      act("You failed to induce terror in $N.",ch, NULL, victim,TO_CHAR); 
      return;
    }
    
    act_new("You start to panic as your mind runs away from you!",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
      act("$N starts to panic in terror!",ch,NULL,victim,TO_CHAR);

    af.where     = TO_AFFECTS; 
    af.type      = gsn_fear;
    af.level     = level;
    af.location  = 0;
    af.duration  = 1;
    af.bitvector = 0;
    af.modifier  = 20;
    affect_to_char(victim, &af);
}

void spell_body_weaponry( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ))
    {
	act("Your hands are already shaped like weapons.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( get_eq_char(ch,WEAR_WIELD) != NULL)
    {
	act("You can't be wielding a weapon.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( get_eq_char(ch,WEAR_SHIELD) != NULL)
    {
	act("You can't be holding a shield.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( get_eq_char(ch,WEAR_HOLD) != NULL)
    {
	act("You can't be holding anything.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where	= TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.modifier  = ch->level / 10;
    af.location  = APPLY_HITROLL;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You shape your hands into weapons.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n shapes $s hands into weapons.",victim,NULL,NULL,TO_ROOM);
}

void spell_ectoplasm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;   
    if (get_summoned(ch, MOB_VNUM_VAPOR) + get_summoned(ch, MOB_VNUM_SOLID) > 0)
    {
        send_to_char("You already have ectoplasm.\n\r",ch);
        return;
    }
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You can not conjure up more ectoplasm.\n\r",ch);
        return;
    }
    af.where	= TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    act ("You conjure up some ectoplasm!", ch, NULL, NULL, TO_CHAR);
    act ("$n conjures up some ectoplasm!", ch, NULL, NULL, TO_ROOM);
    if (number_percent() <= 50)
	ecto_vapor(ch, NULL);
    else
	ecto_solid(ch, NULL);
}

void ecto_vapor(CHAR_DATA *ch, CHAR_DATA *ecto)
{
  CHAR_DATA* mob;
  MOB_INDEX_DATA* pMobIndex;
  if (ecto == NULL)
    {
      mob = create_mobile( get_mob_index( MOB_VNUM_VAPOR ) );
      char_to_room(mob,ch->in_room);
      mob->level      = ch->level;
      mob->alignment = ch->alignment;
      mob->max_hit    = ch->level*10;
      mob->hit        = mob->max_hit;
      mob->hitroll    = (ch->level/10 + 20) / 2;
      mob->damroll    = (ch->level/10 + 20) / 2;
      mob->damage[DICE_NUMBER]    = 4;
      mob->damage[DICE_TYPE]      = ch->level/10 +1;
      SET_BIT(mob->affected_by, AFF_CHARM);
      mob->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
      mob->summoner = ch;
      add_follower(mob,ch);
      mob->leader = ch;
      return;
    }
  pMobIndex = get_mob_index( MOB_VNUM_VAPOR);
  
  ecto->max_hit /= 2;
  ecto->hit /= 2;
  ecto->hitroll /= 2;
  ecto->damroll /= 2;
  ecto->damage[DICE_NUMBER]    = 4;
  act ("$n vaporizes!", ecto, NULL, NULL, TO_ALL);
  ecto->spec_fun   = pMobIndex->spec_fun;


  free_string(ecto->name);
  ecto->name           = str_dup( pMobIndex->player_name );
  free_string(ecto->short_descr);
  ecto->short_descr    = str_dup( pMobIndex->short_descr );
  free_string(ecto->long_descr);
  ecto->long_descr     = str_dup( pMobIndex->long_descr );
  free_string(ecto->description);
  ecto->description    = str_dup( pMobIndex->description );
  
  if (pMobIndex->new_format)
    {
      ecto->act 		= pMobIndex->act;
      ecto->act2 		= pMobIndex->act2;
      REMOVE_BIT(ecto->affected_by, ecto->pIndexData->affected_by);
      SET_BIT(ecto->affected_by, pMobIndex->affected_by);
      ecto->alignment		= pMobIndex->alignment;
      ecto->dam_type		= pMobIndex->dam_type;
      
      ecto->off_flags		= pMobIndex->off_flags;
      ecto->imm_flags		= pMobIndex->imm_flags;
      ecto->res_flags		= pMobIndex->res_flags;
      ecto->vuln_flags		= pMobIndex->vuln_flags;
      
      ecto->sex		= pMobIndex->sex;
      if (ecto->sex == 3)              
	ecto->sex = number_range(1,2);
      ecto->race		= pMobIndex->race;
      ecto->form		= pMobIndex->form;
      ecto->parts		= pMobIndex->parts;
      ecto->size		= pMobIndex->size;
    }
  ecto->pIndexData = pMobIndex;
  interpret(ecto, "remove all");
  interpret(ecto, "drop all");

}

void ecto_solid(CHAR_DATA *ch, CHAR_DATA *ecto)
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    if (ecto == NULL)
    {
        mob = create_mobile( get_mob_index( MOB_VNUM_SOLID ) );
        char_to_room(mob,ch->in_room);
        mob->level      = ch->level;
	mob->alignment = ch->alignment;
        mob->max_hit    = ch->level*20;
        mob->hit        = mob->max_hit;
        mob->hitroll    = ch->level/10 + 20;
        mob->damroll    = ch->level/10 + 20;
        mob->damage[DICE_NUMBER]    = 8;
        mob->damage[DICE_TYPE]      = ch->level/10 +1;
        SET_BIT(mob->affected_by, AFF_CHARM);
        mob->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
        mob->summoner = ch;
        add_follower(mob,ch);
        mob->leader = ch;
	return;
    }

    pMobIndex = get_mob_index( MOB_VNUM_SOLID);
    act ("$n solidifies!", ecto, NULL, NULL, TO_ALL);
    ecto->max_hit *= 2;
    ecto->hit *= 2;
    ecto->hitroll *= 2;
    ecto->damroll *= 2;
    ecto->damage[DICE_NUMBER]    = 8;
    ecto->spec_fun   = pMobIndex->spec_fun;

    free_string(ecto->name);
    ecto->name           = str_dup( pMobIndex->player_name );
    free_string(ecto->short_descr);
    ecto->short_descr    = str_dup( pMobIndex->short_descr );
    free_string(ecto->long_descr);
    ecto->long_descr     = str_dup( pMobIndex->long_descr );
    free_string(ecto->description);
    ecto->description    = str_dup( pMobIndex->description );

    if (pMobIndex->new_format)
    {
	ecto->act 		= pMobIndex->act;
	ecto->act2 		= pMobIndex->act2;
	REMOVE_BIT(ecto->affected_by, ecto->pIndexData->affected_by);
	SET_BIT(ecto->affected_by, pMobIndex->affected_by);
	ecto->alignment		= pMobIndex->alignment;
	ecto->dam_type		= pMobIndex->dam_type;

	ecto->off_flags		= pMobIndex->off_flags;
	ecto->imm_flags		= pMobIndex->imm_flags;
	ecto->res_flags		= pMobIndex->res_flags;
	ecto->vuln_flags		= pMobIndex->vuln_flags;

	ecto->sex		= pMobIndex->sex;
        if (ecto->sex == 3)              
            ecto->sex = number_range(1,2);
	ecto->race		= pMobIndex->race;
	ecto->form		= pMobIndex->form;
	ecto->parts		= pMobIndex->parts;
	ecto->size		= pMobIndex->size;
    }
    ecto->pIndexData = pMobIndex;
}

void spell_hypnosis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( victim == ch )
    {
        send_to_char( "You can't place yourself under hypnosis!\n\r", ch );
        return; 
    }
    if (get_curr_stat(ch, STAT_INT ) < get_curr_stat(victim, STAT_INT))
    {
	send_to_char("You cannot hypnotize someone smarter than you.\n\r", ch);
        return;
    }
    if (!IS_AWAKE(victim) || is_affected(victim,gsn_ecstacy))
    {
        act("$N need to be conscious for you to take over $S mind.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( IS_AFFECTED(victim, AFF_SLEEP) )
    {
        act( "$N is already asleep.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (is_affected(victim, gen_watchtower)){
      sendf( ch, "%s is shielded within the Watchtower.\n\r", PERS(victim, ch ));
      return;
    }
    act("$n locks $N's gaze as beads of sweat form on $s forehead.", ch, NULL, victim, TO_NOTVICT);
    act("You lock $N's gaze and try to force $M to sleep.", ch, NULL, victim, TO_CHAR);
    if ( (IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG) )
	 || IS_UNDEAD(victim)
	 || (psiamp_failed(ch, victim, level, DAM_CHARM, skill_table[sn].spell_type, skill_table[sn].flags)
	     && saves_spell(level, victim, DAM_CHARM,skill_table[sn].spell_type))
	 )
      {
        act( "You failed.", ch, NULL, victim, TO_CHAR);
	return;
      }
    if (is_affected(victim, gsn_horse) 
	&& monk_good(victim, WEAR_HEAD) && !IS_NPC(victim) && !IS_IMMORTAL(ch))
      {
	act( "You reverse $n's attempt to charm you, and charm $M instead!", ch, NULL, victim, TO_VICT );
	act("$N reverses your charm, and charms you instead!", ch, NULL, victim, TO_CHAR );
	af.where     = TO_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  = number_fuzzy( level / 15) +1;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SLEEP;
	affect_to_char( ch, &af );
	if ( IS_AWAKE(ch) )
	  {
	    send_to_char( "You feel very sleepy...\n\r", ch );
	    do_sleep(ch,"");	
	  }
	return;
      }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 4;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SLEEP;
    affect_to_char( victim, &af );
    if ( IS_AWAKE(victim) )
      {
        send_to_char( "You feel very sleepy...\n\r", victim );
	do_sleep(victim,"");	
      }
}


/* field_of_fear effect */
inline bool effect_field_of_fear(CHAR_DATA* ch)
{
  AFFECT_DATA af;

  //bool
  //data
  int chance = 0;

  //const
  const int base_chance = 25;

  //EZ
  if (!IS_SET(ch->in_room->room_flags2, ROOM_PSI_FEAR)
      || IS_IMMORTAL(ch) 
      || is_affected(ch, gsn_mental_barrier) 
      || check_immune(ch, DAM_MENTAL, TRUE) < IS_NORMAL
      || IS_UNDEAD(ch)
      )
    return FALSE;
  //get the chance to scare them away
  chance = base_chance + 2 * (ch->level / 2 - ch->in_room->psitimer);
  chance += (get_curr_stat(ch, STAT_INT) - 20) * 5;

  if (number_percent() < chance)
    {
      act("The hairs on your body stand on end.",ch,NULL,NULL,TO_ROOM);   
      return FALSE;
    }
  
  if (is_affected(ch, skill_lookup("fear"))) {
    return FALSE;
  }

  //we scare them.
  send_to_char("You are overpowered with intense fear.\n\r", ch);
  act("$n's eyes widen in fear", ch, NULL, NULL, TO_ROOM);

/* attach effect to hurt skills */
  af.type = skill_lookup("fear");
  af.level = ch->in_room->psitimer;
  af.duration = number_range(0,1);
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);
  return TRUE;
}//END IF SUCCESS


void spell_field_of_fear( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  AFFECT_DATA af;
    int chance = int_app[get_curr_stat(ch,STAT_INT)].learn;
    CHAR_DATA *ich, *ich_next;

    if (is_affected(ch, sn))
      {
	send_to_char("You are not ready for another attempt.\n\r", ch);
	return;
      }

    if (IS_SET(ch->in_room->room_flags2, ROOM_PSI_FEAR))
    {
      send_to_char("The area looks too safe to place an effective enchantement.\n\r", ch);
      return;
    }

    for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
    {
	if (IS_NPC(ch))
	    chance -= 2;
	else
	    chance -= 5;
    }
    if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) 
	|| ch->in_room->sector_type == SECT_CITY)
      {
	send_to_char("This room is far to peacefull for that.\n\r", ch);
	return;
      }

    if (number_percent() > chance)
      {
        act("You failed to place a field of fear in this room!", ch, NULL, NULL, TO_CHAR );
        act("The hairs on your body tingle.",ch,NULL,NULL,TO_ROOM);   
	return;
    }
    act("You place a field of fear in this room!", ch, NULL, NULL, TO_CHAR );
    act("$n places a field of fear in this room!",ch,NULL,NULL,TO_ROOM);   

    SET_BIT(ch->in_room->room_flags2, ROOM_PSI_FEAR);
    ch->in_room->psitimer = ch->level / 2;

    for ( ich = ch->in_room->people; ich != NULL; ich = ich_next )
      {
        ich_next = ich->next_in_room;
        effect_field_of_fear(ich);
      }
   
    af.type = sn;
    af.level = level;
    af.duration = ch->level / 4;

    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_char(ch, &af);

}

void spell_control_flames( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj;
    bool strong = TRUE;
    int dam;
    if ((obj = get_eq_char( victim, WEAR_LIGHT )) == NULL)
    {
	strong = FALSE;
        if ((obj = get_eq_char( ch, WEAR_LIGHT )) == NULL)
	{
	    send_to_char("Neither you nor your opponent are using a light source.\n\r", ch);
	    return;
	}
    }
    dam = dice(2, 8) + UMAX(90, obj->level * 4);
    if (!strong)
	dam /= 2;
    if ( saves_spell( level, victim, DAM_FIRE,skill_table[sn].spell_type) )
        dam /= 2;
    act( "$p glows brightly and shoots forth a ball of fire at you!", victim, obj, NULL, TO_CHAR );
    act( "$p glows brightly and shoots forth a ball of fire at $n!", victim, obj, NULL, TO_ROOM );
    damage( ch, victim, dam, sn, DAM_FIRE ,TRUE);
}

void spell_forget( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ))
    {
	act("$N already forgot all $S spells.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type) 
	|| IS_UNDEAD(victim))
      if (saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type) 
	  ||IS_UNDEAD(victim))
	{
	  if (IS_UNDEAD(victim))
	    act("$N's mind seems cold and empty.", ch, NULL, victim, TO_CHAR);
	  act("You failed to make $N lose $S memory.",ch,NULL,victim,TO_CHAR);
	  return;
	}

    af.where	= TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 1 + (level > 38? 1 : 0);
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "Your memory falters and you forget all your spells.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
      act("$N forgets all $S spells.",ch,NULL,victim,TO_CHAR);
}

void spell_graft_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    OBJ_DATA *obj;

    if ( is_affected( victim, sn ))
    {
	act("Your weapon is already grafted to your hands.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ((obj = get_eq_char(ch,WEAR_WIELD)) == NULL)
    {
	act("You have to be wielding a weapon.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_OBJ_STAT(obj, ITEM_PSI_ENHANCE))
      {
	act("$p resists your attempts!", ch, obj, NULL, TO_CHAR);
	return;
      }

    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.where	 = TO_AFFECTS;
    af.bitvector = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    affect_to_char( victim, &af );

    act_new( "You graft $p to your hands.",victim,obj,NULL,TO_CHAR,POS_DEAD);
    act("$n grafts $p to $s hands.",victim,obj,NULL,TO_ROOM);
}

void spell_ballistic_attack( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    CHAR_DATA *victim;
    int dam;
    int weight = get_obj_weight(obj);

    if ((victim = ch->fighting) == NULL)
    {
	send_to_char("You don't have the anger.\n\r", ch);
	return;
    }
    if (!CAN_WEAR(obj, ITEM_TAKE) 
	|| CAN_WEAR(obj, ITEM_NO_SAC)
	|| obj->item_type == ITEM_CABAL
	|| is_affected_obj(obj, gsn_gravitate)
	|| weight > 1000

	)

    {
	send_to_char("You cannot lift that object.\n\r", ch);
	return;
    }

    dam = dice(level / 10, 8);
    if (is_affected_obj(obj, gsn_levitate))
      dam += UMIN(weight, 500) / 5;
    else
      dam += UMIN(weight, 500) / 3;
    if ( saves_spell( level, victim,DAM_BASH,skill_table[sn].spell_type) )
      dam /= 2;
    act( "$p rises in the air and suddenly flies straight at you!", victim, obj, ch, TO_CHAR );
    act( "$p rises in the air and suddenly flies straight at $n!", victim, obj, ch, TO_ROOM );
    damage( ch, victim, dam, sn, DAM_BASH ,TRUE);
    /* move the object from where it is to the ground */
    if (!ch->in_room)
      return;
    if (obj->carried_by)
      obj_from_char(obj);
    else if (obj->in_obj)
      obj_from_obj(obj);
    else if (obj->in_room)
      obj_from_room(obj);
    obj_to_room(obj, ch->in_room);
}

void spell_mental_barrier( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ))
    {
	act("You have already created a mental barrier",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where	= TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.modifier  = -25;
    af.location  = APPLY_SAVING_MENTAL;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You create a mental barrier.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
}

void spell_telekinetic_barrier( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
}

void spell_psychic_shout( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *vch, *vch_next;
    int dam = dice(5, 8) + level * 2;
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;
        if ( saves_spell( level, vch, DAM_MENTAL,skill_table[sn].spell_type) )
            dam /= 2;
        if ( vch != ch && !is_area_safe(ch,vch))
        {
            m_yell(ch,vch,FALSE);
            damage( ch, vch, dam, sn, DAM_MENTAL ,TRUE);
        }
    }
}

void spell_clairvoyance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *orig;
    ROOM_INDEX_DATA *location;
    if ( ( victim = get_char_world( ch, target_name ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r",ch );
        return;
    }
    if (ch == victim)
      {
	send_to_char("Why not simply open your eyes?\n\r", ch);
	return;
      }

    if ((location = victim->in_room) == NULL)
        return;
    if ( room_is_private( location ) && get_trust(ch) < MAX_LEVEL)
    {
        send_to_char( "That room is private right now.\n\r", ch );
        return;
    }
    if ( !have_access(ch,location) || IS_IMMORTAL(victim) || is_safe_quiet(ch, victim))
    {
        send_to_char( "Something blocks your attempts.\n\r",ch);
        return;
    }       
    if ( saves_spell( level, victim, DAM_MENTAL,skill_table[sn].spell_type ) )
      {
	send_to_char("You failed.\n\r", ch);
	send_to_char("Your eyes feel funny.\n\r", victim);
	return;
      }

    if (IS_UNDEAD(victim) && number_percent() < 60)
      {
	AFFECT_DATA af;
	act("As begin to look through $N's eyes horros of undeath and forces of Limbo tear into you!", ch, NULL, victim, TO_CHAR);
	damage(ch, ch, victim->level * 4, sn, DAM_NEGATIVE, TRUE);
	send_to_char("Unable to stand the strain you collapse.\n\r", ch);
	act("$n's eyes roll back and he collapses.", ch, NULL, NULL, TO_ROOM);

	af.type = gsn_sleep;
	af.level = level;
	af.duration = 1 + victim->level > level? 1: 0 
	  + victim->level-2 > level? 1 : 0 
	  + victim->level - 6 > level? 2: 0;
	af.where = TO_AFFECTS;
	af.bitvector = AFF_SLEEP;
	af.location = APPLY_NONE;
	af.modifier = 0;
	affect_to_char(ch, &af);
	do_sleep(ch, "");
      }

    send_to_char("You look and feel through the faculties of your victim.\n\r", ch);
    orig = victim->desc;
    victim->desc = ch->desc;
    if (number_percent() < 35)
	interpret (victim, "score");
    else
	interpret (victim, "eq");
    victim->desc = orig;
}

void spell_ghoul_touch( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (is_affected(victim,sn))
    {
	send_to_char("They're already sluggish.\n\r",ch);
	return;
    } 
    act("A green glow envelops $n's hands.",ch,NULL,victim,TO_ROOM);
    act("A green glow envelops your hands.",ch ,NULL,NULL, TO_CHAR);

    if (saves_spell(level-2,victim,DAM_NEGATIVE,skill_table[sn].spell_type))
    {
	act("$N wasn't affected by the ghoul touch.",ch,NULL,victim,TO_CHAR);
	if ( ch != victim)
	  act("You shake off $n's ghoul touch.",ch,NULL,victim,TO_VICT);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/5;
    af.location  = 0;
    af.modifier  = 5;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new("You feel very sluggish.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
	act("$N looks very sluggish.",ch,NULL,victim,TO_CHAR);
}

void spell_eternal_day( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    ROOM_INDEX_DATA * room;
    AFFECT_DATA af;
    int np = number_percent();
    if (is_affected(ch,sn))
    {
	send_to_char("You cannot cast this spell yet.\n\r",ch);
	return;
    }
    if (ch->in_room == NULL)
        return;
    room = ch->in_room;
    if (room->pCabal)
    {
        send_to_char("Not in a cabal.\n\r",ch);
        return;
    }
    if (np - 2*(get_curr_stat(ch,STAT_LUCK) - 16) < 50)
    {
        send_to_char("You failed to bathe the room with sunlight.\n\r",ch);
        af.where     = TO_AFFECTS;
        af.type      = sn;
        af.level     = level;
        af.location  = 0;
        af.duration  = 24;
        af.bitvector = 0;
        af.modifier  = 0;
        affect_to_char(ch,&af);
        return;
    }
    if (IS_SET(room->room_flags2,ROOM_DAY))
    {
        send_to_char("This room is already bathed in sunlight.\n\r",ch);
        return;
    }
    send_to_char("You bathe the room with eternal sunlight.\n\r",ch);
    act("$n bathes the room with eternal sunlight.",ch,NULL,NULL,TO_ROOM);
    SET_BIT(room->room_flags2,ROOM_DAY);
    REMOVE_BIT(room->room_flags,ROOM_DARK);
    REMOVE_BIT(room->room_flags2,ROOM_NIGHT);
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.location  = 0;
    af.duration  = 200;
    af.bitvector = 0;
    af.modifier  = 0;
    affect_to_char(ch,&af);
}

void spell_everlasting_night( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    ROOM_INDEX_DATA * room;
    AFFECT_DATA af;
    int np = number_percent();
    if (is_affected(ch,sn))
    {
	send_to_char("You cannot cast this spell yet.\n\r",ch);
	return;
    }
    if (ch->in_room == NULL)
        return;
    room = ch->in_room;
    if (room->pCabal)
    {
        send_to_char("Not in a cabal.\n\r",ch);
        return;
    }
    if (np - 2*(get_curr_stat(ch,STAT_LUCK) - 16) < 50)
    {
        send_to_char("You failed to induce the room into night.\n\r",ch);
        af.where     = TO_AFFECTS;
        af.type      = sn;
        af.level     = level;
        af.location  = 0;
        af.duration  = 24;
        af.bitvector = 0;
        af.modifier  = 0;
        affect_to_char(ch,&af);
        return;
    }
    if (IS_SET(room->room_flags2,ROOM_DAY))
    {
        send_to_char("This room is already stopped in nighttime.\n\r",ch);
        return;
    }
    if (mud_data.time_info.hour > 6 && mud_data.time_info.hour < 19)
    {
    	send_to_char("The sun disappears, never to appear in this room again.\n\r",ch);
    	act("The sun disappears, never to appear in this room again.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
    	send_to_char("The night halts dead in its tracks.\n\r",ch);
    	act("$n stops the night in its tracks.",ch,NULL,NULL,TO_ROOM);
    }
    SET_BIT(room->room_flags2,ROOM_NIGHT);
    SET_BIT(room->room_flags,ROOM_DARK);
    REMOVE_BIT(room->room_flags2,ROOM_DAY);
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.location  = 0;
    af.duration  = 200;
    af.bitvector = 0;
    af.modifier  = 0;
    affect_to_char(ch,&af);
}

void spell_apathey( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (is_affected(victim,sn))
    {
	send_to_char("They're already apathetic.\n\r",ch);
	return;
    } 
    if (saves_spell(level-2,victim,DAM_MENTAL,skill_table[sn].spell_type))
    {
	act("$N wasn't affected by your apathey.",ch,NULL,victim,TO_CHAR);
	if ( ch != victim)
	    act("You resist $n's attempt to apathetize you.",ch,NULL,victim,TO_VICT);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/5;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new("You feel apathetic.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
	act("$N looks around without a care.",ch,NULL,victim,TO_CHAR);
    stop_follower( ch );
}

void spell_feedback( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (is_affected(victim,sn))
    {
	send_to_char("They are already affected by feedback.\n\r",ch);
	return;
    }
    if (saves_spell(level-2,victim,DAM_MENTAL,skill_table[sn].spell_type))
    {
	act("You failed to set a feedback on $N.",ch,NULL,victim,TO_CHAR);
	if ( ch != victim)
	    act("$n failed to set a feedback on you.",ch,NULL,victim,TO_VICT);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/5;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new("Your are now set to feedback.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
	act("$N is set to feedback.",ch,NULL,victim,TO_CHAR);
}

void spell_virtuous_light( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dur; 
    if(is_affected(victim,sn))
    {
        send_to_char("You are already filled with a virtuous light.\n\r",victim);
        return;
    }


    dur = number_fuzzy(level/2);
    af.where	 = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = dur;
    af.bitvector = 0;
 
    af.location	 = APPLY_AC;
    af.modifier	 = -2*(level-10)/3;
    affect_to_char( victim, &af );
    
    af.location	 = APPLY_LUCK;
    af.modifier	 = number_range( 1, 2);
    affect_to_char( victim, &af );


    send_to_char("The Light of Virtue fills you with conviction.\n\r",victim);
    act("$n bows $s head and is filled with a virtuous light.",ch,NULL,NULL,TO_ROOM);

}

void spell_assist( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim;
    bool gate_pet;
    if (IS_AFFECTED2(ch, AFF_SHADOWFORM))
    {
        REMOVE_BIT(ch->affected2_by, AFF_SHADOWFORM);
        affect_strip(ch, gsn_shadowform);
        act("Your body regains its substance, and you materialize into existence.",ch,NULL,NULL,TO_CHAR);
        act("$n's body regains its substance, and $e materializes into existence.",ch,NULL,NULL,TO_ROOM);
        WAIT_STATE2(ch, 24);
    }
    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL)
    {
        send_to_char("Your mount refuses to enter the gate.\n\r",ch);
        return;
    }
    if ( (victim = get_char_world( ch, target_name ) ) == NULL
    || victim == ch || victim->in_room == NULL || !can_see_room(ch,victim->in_room)
    || IS_NPC(victim) || !is_same_group(ch,victim)
    || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    || IS_SET(victim->in_room->room_flags, ROOM_SAFE)  
    || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY) 
    || IS_SET(victim->in_room->room_flags, ROOM_NO_GATEIN)
    || IS_SET(ch->in_room->room_flags, ROOM_NO_GATEOUT)  
    || IS_SET(victim->in_room->room_flags, ROOM_NO_INOUT)
    || IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)
    || (!IS_IMMORTAL(ch) && IS_IMMORTAL(victim))
    || (!IS_NPC(victim) && IS_SET(victim->act,PLR_NOSUMMON) && !is_pk(ch,victim))   
    || is_affected(ch, gsn_tele_lock) || is_affected(victim,gsn_catalepsy) )
    {
        send_to_char("You failed to create a gate.\n\r",ch);   
        return;
    }
    else if (is_affected(ch,gen_ensnare)){
      send_to_char("You sense a powerful magical field preventing your departure.\n\r", ch);
      return;
    }
    if (ch->pet != NULL && ch->in_room == ch->pet->in_room) 
        gate_pet = TRUE;
    else
        gate_pet = FALSE;
    if (ch->invis_level < LEVEL_HERO)
    {
    act("$n steps through a gate and vanishes.",ch,NULL,NULL,TO_ROOM);
    act("You step through a gate and vanish.",ch,NULL,NULL,TO_CHAR);
    }
    char_from_room(ch);
    char_to_room(ch,victim->in_room);
    if (ch->invis_level < LEVEL_HERO)
        act("$n has arrived through a gate.",ch,NULL,NULL,TO_ROOM);
    do_look(ch,"auto");  
    if (gate_pet)
    {
        act("$n steps through a gate and vanishes.",ch->pet,NULL,NULL,TO_ROOM);
        act("You step through a gate and vanish.",ch->pet,NULL,NULL,TO_CHAR);
        char_from_room(ch->pet);
        char_to_room(ch->pet,victim->in_room);
        act("$n has arrived through a gate.",ch->pet,NULL,NULL,TO_ROOM);
        do_look(ch->pet,"auto");
    }
}

void spell_unholy_strength(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    char arg[MIL];
    AFFECT_DATA af, *paf;

//data
    int sklevel = 0;//current level of the spell.
    int gsn = 0 ;
//const
    const int amplvl_mod = 10; //amount added onto lvl when duration is extended.
    const int dur_mod = 5;  //amount of duration extended.
    const int mod_mod = 3; //amount added onto str/dam/svs for each additional cast.

    const int lvl_mod = 10; //divisor for comparin lvl of spell to caster lvl.

//bool
    bool is_message = FALSE;

    one_argument(target_name, arg);

    if (is_affected(ch, gsn = skill_lookup("calm")))
      if (!check_dispel(level,ch,gsn))
	{
	  send_to_char( "Your mind flares with demonic visions.\n\r", ch );
	  return;
	}

    if (is_affected(ch,gsn_drained))
    {
	send_to_char("You are too drained right now to try that.\n\r",ch);
	return;
    }

    if (arg[0] == '\0')
    {    
        if (!is_affected(ch,sn))
        {    

            af.type      = sn;
            af.where     = TO_AFFECTS;
            af.level     = 1;
            af.bitvector = 0;
	    af.duration  = (level / lvl_mod);
            af.modifier  = UMAX(1, (level / lvl_mod) - 1);
            af.location  = APPLY_DAMROLL;
            affect_to_char(ch,&af);

            af.where     = TO_AFFECTS;
            af.level     = 1;
	    af.duration  = (level / lvl_mod);
            af.bitvector = 0;
            af.modifier  = UMAX(1, (level / lvl_mod) - 1);
            af.location  = APPLY_HITROLL;
            affect_to_char(ch,&af);


            af.where     = TO_AFFECTS;
            af.bitvector = 0;
	    af.duration  = (level / lvl_mod);
            af.level     = 1;
            af.modifier  = UMIN(-1,-(level / lvl_mod - 1));
            af.location  = APPLY_SAVING_SPELL;
            affect_to_char(ch,&af);
            send_to_char("Vengence of the Eternal Army surges through your forsaken soul.\n\r",ch);
            act("$n's body surges with unholy energy!",ch,NULL,NULL,TO_ROOM);
        }
        else 
        {
            for(paf=ch->affected;paf!=NULL;paf=paf->next)
	    {
                if(paf->type == sn)
                {
                    sklevel = paf->level;
                    break;
                }
	    }
	    if (sklevel == 0)
		return;
            else if(sklevel%lvl_mod > level/lvl_mod)
            {
                send_to_char("Your body has already reached the breaking point.\n\r",ch);
                return;
            }
            else if(sklevel%lvl_mod > 0)
            {
		send_to_char("You bring your body closer to the breaking point.\n\r",ch);
                act("$n screams in agony as his body surges with power!",ch,NULL,NULL,TO_ROOM);
                for(paf=ch->affected;paf!=NULL;paf=paf->next)
		{
                    if(paf->type == sn)
		    {
                        paf->level+= mod_mod;
                        if (paf->location == APPLY_HITROLL)
			{
                            ch->hitroll+=mod_mod;
			    paf->modifier+=mod_mod;
			}
			if (paf->location == APPLY_DAMROLL)
			{
			    ch->damroll+=mod_mod;
			    paf->modifier+=mod_mod;
			}
			if (paf->location == APPLY_SAVING_SPELL)
			{
			    ch->savingspell-= mod_mod;
			    paf->modifier-= mod_mod;
			}
                    }
		}
            }    
        }
    } 
    else if (!str_cmp(arg,"amplify"))
    {
        for(paf=ch->affected;paf!=NULL;paf=paf->next)
	{
            if(paf->type == sn)
            {
                sklevel=paf->level;
                if (sklevel/lvl_mod > level/lvl_mod)
                {  
                    send_to_char("Your body has already reached the breaking point.\n\r",ch);
                    return;
                }
                paf->level +=amplvl_mod;
                paf->duration +=dur_mod;
	        is_message = TRUE;
            }
	}
	if (is_message)
	{
            send_to_char("You amplify your allegiance with the Eternal Army.\n\r",ch);
            act("$n screams in agony as his body surges with power!",ch,NULL,NULL,TO_ROOM);
	}
	else
	    send_to_char("You are not affected by unholy strength to amplify it.\n\r",ch);
    }
    else
        send_to_char("To amplify your allegiance with the Eternal Army: <syntax: ca 'unholy strength' amplify>.\n\r",ch);
}

void spell_hex( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected(victim, sn) ) 
    {
        act("$N is already jinxed by a hex.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( saves_spell( level,victim,DAM_NEGATIVE,skill_table[sn].spell_type) )
    {
        act("You failed to jinx $N with a hex.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 6*(ch->level + get_curr_stat(ch, STAT_INT) + get_curr_stat(ch, STAT_WIS))
       -(victim->level + get_curr_stat(victim, STAT_INT) + get_curr_stat(victim, STAT_WIS));
    af.location  = APPLY_LUCK;
    af.modifier  = (1+get_curr_stat(ch,STAT_LUCK))/-6;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You have been jinxed by a voodoo hex.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n has been jinxed by a voodoo hex.", victim, NULL, NULL, TO_ROOM );
}



////////////////////
//gout_heal_msg////
///////////////////
//This is a gunction made for clarity that displays the various
//msg strings for the amount of healing form blood_gout.

inline void gout_heal_msg(CHAR_DATA* ch, int tot_fed)
{
  //Few constants for tweaking:
  int max_msg = 80;//the first amount
  int msg_slope = 2;//fraction that the next amounts drop by.
  //we simply run through a if tree and display messages.

  //MORE THEN 100!!
  if (tot_fed >= max_msg)
    {
    act("\n\r`!A vortex of blood surrounds $n and $s wounds close instantly.``", ch, NULL, NULL, TO_ROOM);
    act("\n\r`!A swirling wall of blood surrounds you and your body regenerates.``", ch, NULL, NULL, TO_CHAR);
    }//END >1
  else if (tot_fed > max_msg/msg_slope)
    {
      act("\n\r`!Gouts of blood shoot straight into $n's mouth!``", ch, NULL, NULL, TO_ROOM);
      act("\n\r`!You direct the jets of blood straight down your throat. What a feast!``", ch, NULL, NULL, TO_CHAR);
    }//end >1/2
  else if (tot_fed > max_msg/(2*msg_slope))
    {
      act("\n\rA thick mist of blood surrounds $n and slowly dissipates.", ch, NULL, NULL, TO_ROOM);
      act("\n\rYou gather the blood around yourself and feed leisurely.", ch, NULL, NULL, TO_CHAR);
    }//end 1/4
  else if(tot_fed > max_msg/(4*msg_slope))
    {
      act("Fine mist of blood drops surrounds $n and quickly disappears.\n\r", ch,NULL,NU\
LL,TO_ROOM);
      send_to_char("You coerce the blood to swirl around you and feed quickly.\n\r", ch);
    }//End 1/8
  else if(tot_fed > max_msg/(8*msg_slope))
    {
      act("\n\r$n licks the blood from $s face and smacks $s lips ", ch, NULL, NULL, TO_ROOM);
      act("\n\rYou lick the blood from your face and smack your lips.", ch, NULL, NULL, TO_CHAR);
    }//end 1/16
  else if(tot_fed > max_msg/(16*msg_slope))
    {
      act("\n\r$n licks a few droplets of blood from $s lips.", ch, NULL, NULL, TO_ROOM);
      act("\n\rFew drop of blood land on your lips.", ch, NULL, NULL, TO_CHAR);
    }//end if 1/32
  else if (tot_fed != 0)
    {
      act("\n\r$n flares $s nostrils at the sent of blood in the air.", ch, NULL, NULL, TO_ROOM);
      act("\n\rYou breath deeply, savouring the scent of blood in the air.", ch, NULL, NULL, TO_CHAR);
    }//end not 0
}//end goutmsg.


////////////////////
//make_item_char///
///////////////////
//Creats an item given by "vnum" in the current location of character.
//the life specifes if neccasary the tick life of athe object.

void make_item_char(CHAR_DATA* ch, int vnum, int life)
{
  //Create an onject in the room based on vnum.
  OBJ_DATA *obj, *gobj;

//Create an object with data as per vnum
  obj = create_object( get_obj_index(vnum ), ch->level);
  //set life if not negative.
  if (life >= 0)
    obj->timer = life;

//Cycle froom teh room and put it at then end of a list.
  for ( gobj = ch->in_room->contents; gobj != NULL; gobj = gobj->next_content )
    {    
      if (gobj->pIndexData->vnum == vnum) 
	{   
	  obj_from_room( gobj );    
	  extract_obj(gobj); 
	}//end if
    }//end for
  obj_to_room(obj, ch->in_room);
}//end make_item_char

////////////////////
//gout_dam_msg    //
////////////////////
//displays the damage message of blood_gout accoring to the amount of damage done.

inline void gout_dam_msg(CHAR_DATA* ch, CHAR_DATA* vch, int dam)
{

  //we dont do any of this if damage is 0
  if (dam == 0)
    return;

//We display the damage dependant messages:
	    if (dam > 199)
	      {
//Sent to the room but not ch or vch
		act("\n\r`!With crackle and snap of bursting tissue, $N's body explodes into a crimson geyser of blood!''", ch,NULL,vch,TO_NOTVICT);
		
//Sent to vch only
		act("\n\r`!The world fades crimson as your body explodes into a bloody pulp painting the room crimson.``", vch, NULL, NULL, TO_CHAR);

//Sent to ch only
		act("\n\r`!Every inch of tissue exploding in orgy of blood, you turn $N into a gory corpse.``", ch, NULL, vch, TO_CHAR);

//We create the pools of blood item.
		make_item_char(ch, OBJ_VNUM_BLOOD_POOL, 24);
	      }//END 
	    else if (dam > 174)
	      {
		//Sent to the room but not ch or vch
		act("\n\r$N screams in agony as $S arteries burst with fountains of blood!", ch,NULL,vch,TO_NOTVICT);

		//Sent to vch only
		act("\n\rYou scream in agony as $N causes your veins to explode with jets of blood.", vch, NULL, ch, TO_CHAR);

		//Sent to ch only
		act("\n\rAt your command $N becomes a bloody sprinkler.", ch, NULL, vch, TO_CHAR);
	      }//END 
	    else if (dam > 149)
	      {
		//Sent to the room but not ch or vch
		act("\n\r$N writhes on the ground as blood pours from every inch of $s body!", ch,NULL,vch,TO_NOTVICT);
		
		//Sent to vch only
		act("\n\rYou writhe in pain as blood bursts forth from every inch of your skin.", vch, NULL, ch, TO_CHAR);

		//Sent to ch only
		act("\n\rBlood bursts from $N's every pore.", ch, NULL, vch, TO_CHAR);
	      }//END 
	    else if (dam > 124)
	      {
		act("\n\rColor drains from $N's face as $S minor arteries open up.", ch, NULL, vch, TO_NOTVICT);
		
		//Sent to vch only
		act("\n\rYou almost faint as few of your minor arteries burst forth with bloody streams.", vch, NULL, ch, TO_CHAR);

		//Sent to ch only
		act("\n\rYou cause $N's minor arteries to explode with streams of blood.", ch, NULL, vch, TO_CHAR);
	      }//END 
	    else if (dam > 74)
	      {
		//Sent to the room but not ch or vch
		act("\n\r$N's eyes and mouth shoot with gouts of blood!.", ch,NULL,vch,TO_NOTVICT);
		
		//Sent to vch only
		act("\n\rYou choke and lose your vision as your mouth and eyes well up with streams of blood.", vch, NULL, ch, TO_CHAR);

		//Sent to ch only
		act("\n\rYou force $N's blood through every pore on his face.", ch, NULL, vch, TO_CHAR);
	      }//end 50
	    else if (dam > 50)
	      {
		//Sent to the room but not ch or vch
		act("\n\r$N begins to bleed profusely.", ch,NULL,vch,TO_NOTVICT);

		//Sent to vch only
		act("\n\rSuddenly you begin to bleed from every pore in your body.", vch, NULL, ch, TO_CHAR);

		//Sent to ch only
		act("\n\rThe sweet scent of blood fills the air.", ch, NULL, vch, TO_CHAR);
	      }//end 
	    else 
	      {
		//Sent to the room but not ch or vch
		act("\n\r$N face turns crimson and $S nose begins to bleed rapidly.", ch,NULL,vch,TO_NOTVICT);

		//Sent to vch only
		act("\n\rYou feel lightheaded as your blood pressure skyrockets.", vch, NULL, ch, TO_CHAR);
		
		//Sent to ch only
		act("\n\rYou force $N's blood pressure through roof.", ch, NULL, vch, TO_CHAR); 
	      }//end else <25
}//END gout_dam_msg



////////////////////
//spell_blood_gout//
////////////////////
//Room damage spell, negative, hals the caster from each damaged
//tarrget if high enough rank.

void spell_blood_gout(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
//data used for room damage effect.
  CHAR_DATA *vch, *vch_next;

//integer to hold the damage that WILL be done so the damage messages come
//out properly.
  int temp_dam;

//we need one more bool to check to not spam the broacast.
  bool did_broadcast = FALSE;

//damage median is in the table, and used for min/max calculation fo the
//acutal damage caused by the spell.
//the spell does regular damage to all in the room, cuttin it down
//by: spell_damage+=spell_Damage*(22-vch->con)/15 (floored at con=14)
//and: 75% if saved.
//Chance to feed: damage/(feed_mod + (feed_med - char_lvl)) 
//Adjust the feed_mod to change the fraction of damage
//healed untill at feed_med. 

  int feed_mod =25;    //fraction of damage healed at 38.
  int feed_med = 38;   //level at which feed_mod is uncahnged by rank.
  int tot_fed=0;      //total amount of damage healed, used for messages.
  int max_fed = 100;   //ceiling on the max amount fed from each spell.
//Damage for each lvl table
    static const sh_int dam_each[] = 
    {
      0,
      0,   0,   0,   0,   0,  0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,  0,   0,   0,   0,   0,
      0,   0,   0,   0,   0, 60,  60,  60,  60,  60,
      65,  65,  65,  65, 65, 70, 71, 72, 73, 74,
      75, 80, 85, 95, 105,115, 125, 130, 135, 140
    };

//use it to store the damage.
    int dam;

//calculats the size of the table for portability
    level= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);

//makes sure that level is within bounds.
    level= UMAX(0, level);


// Calculate the feed_mod based on charachters 
//damage healed is 1/feed_mod of caused, then 
//chaning depending on difference of lvl's from feed_med to char->level.
    //the -feed_mod+1 makes sure that max ratio is 1:1, or there is no x/0
    feed_mod += UMAX(-feed_mod + 1, feed_med-ch->level);
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next){
      
      int con = UMAX(14, get_curr_stat(vch,STAT_CON));
      
      vch_next = vch->next_in_room;
      dam = number_range( 2 * dam_each[level] / 3, dam_each[level]);
      dam = dam * 20 / con;

      if ( saves_spell( level, vch, DAM_NEGATIVE,skill_table[sn].spell_type)){
	dam = 2 * dam / 3;
      }

      if ( vch != ch && !is_area_safe(ch,vch)){
	if(!IS_SET(vch->act, ACT_UNDEAD)){
	  m_yell(ch,vch,FALSE);

	  if ( (temp_dam = get_spell_damage(ch, vch, dam, DAM_NEGATIVE))  > 0)
	    gout_dam_msg(ch, vch,  temp_dam);	  

	  //get_spell damage returns n've on some cases, so we need to take care of that little bit.
	  temp_dam = UMAX(0, temp_dam);

	  if( (!did_broadcast) && (temp_dam > (20 * dam_each[50] / 14)) ){
	    broadcast(ch, "Wet sounds of rupturing felsh and scent of fresh blood drifts in.\n\r");
	    did_broadcast = TRUE;
	  }

	  tot_fed += UMIN(number_range((2*temp_dam)/(3*feed_mod), temp_dam/feed_mod), vch->max_hit/feed_mod);
	  damage( ch, vch, dam, sn, DAM_NEGATIVE ,TRUE);
	  /* we feed a bit */
	  gain_condition( ch, COND_HUNGER, 2);
	}//end undead check.
	else{
	  act("$N seems unaffected for some reason.", ch, NULL, vch, TO_CHAR);
	  act("$N seems to shrug off the spell for some reason.", ch, NULL, vch, TO_NOTVICT);
	  act("Your skin tingles momentarly.", vch, NULL, NULL, TO_CHAR);
	}//end else to undead.
      }//END IF AREA SAFE
    }//END FOR LOOP

    if (tot_fed > 0){
      tot_fed = UMIN(max_fed, tot_fed);
      gout_heal_msg(ch, tot_fed);
      ch->hit += URANGE(0, tot_fed, ch->max_hit);
      update_pos( ch );
    }//end feedign.
}//end spell_blod gout


///////////////
//spell_behead//
//////////////
//basicly a fancy slay command.

void spell_behead(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  //little fancy cheating here :)
  if (strcmp(ch->name, "Virigoth"))
    {
      act("`&Carnifex Pax `8seems to suddenly take life of its own and turns against you!``", ch, NULL, NULL, TO_CHAR);
      act("`&Carnifex Pax `8seems to suddenly take life of its own and turns against $n!``", ch, NULL, NULL, TO_ROOM);
      vo = ch;
    }

//we pring out a message, and kill target
  if (vo != NULL)
    {
      act("`8The chipped, rusty blade takes another head!``", ch , NULL, vo, TO_ROOM);
      act("`8The chipped, rusty blade takes another head!``", ch , NULL, vo, TO_CHAR);
      act("`8$N's head flies off as the mighty blade cleaves $S neck in twain.\n\r``", ch, NULL, vo, TO_NOTVICT);
      act("`8The last thing you feel is the rusty, chipped blade bite into your neck.``", ch, NULL, vo, TO_VICT);
      act("`8You `7have been `&BEHEADED!!``.", ch, NULL, vo, TO_VICT);
      raw_kill(ch, vo);
    }
}

/* spell_malform_weapon */
/*written by Virigoth */
void spell_malform_weapon(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  /* 
     This is the main initilization of the spell.
     1) Check for EZ's, check location/time, and other conditions.
     2)Run the effect, the gen_init will select and calculate the effects
     3)Set the gsn so the skill cannot be used again in  a while.
  */

  AFFECT_DATA af;
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  OBJ_DATA* weapon;
//  AFFECT_DATA* paf, *paf_next;

  const int full_dur = 400;
  const int fail_dur = 150;
  //bool
  bool fAutoPass = IS_IMMORTAL(ch);
//EZ
  if (ch==NULL)
    return;
  if (get_skill(ch, sn) < 75)
    {
      send_to_char("Without proper skill this would surly kill you outright.\n\r", ch);
      return;
    }

//first we check if this is a weapon
  if (obj->item_type != ITEM_WEAPON)
    {
      send_to_char("That item is not a weapon.\n\r",ch);
      return;
    }
//Carried?
  if (obj->wear_loc != -1)
    {
      send_to_char("The item must be carried to be enchanted.\n\r",ch);
      return;
    }

//Misc factors
  if (affect_find(obj->affected, gen_malform))
    {
      act("$p already carries the dark gift.", ch, obj, NULL, TO_CHAR);
      return;
    }

  if ( (ch->alignment > EVIL_THRESH) || IS_SET(ch->act, PLR_OUTCAST) 
       || is_affected(ch, skill_lookup("calm")) )
    {
      send_to_char("You lack the malice neccassary.", ch);
      return;
    }
  if (IS_SET(obj->extra_flags, ITEM_STAIN)){
    send_to_char("Cannot malform `!STAINED`` item.\n\r", ch);
    return;
  }

  if (IS_OBJ_STAT(obj,ITEM_BLESS)){
    send_to_char("The blessing on the item seems to block your attempts.", ch);
    return;
  }
//rediness?
  if (is_affected(ch, sn) && !fAutoPass)
    {
      send_to_char("You are not ready yet for another attempt.\n\r", ch);
      return;
    }
//These cause the gsn to be set.
//The effect is not actuly applied till something fails.
  af.type = sn;
  af.level = level;
  af.duration = fail_dur;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.modifier = 0;
  af.location = APPLY_NONE;

  
//Time
  
  if (mud_data.time_info.hour != 0 && !fAutoPass)
    {
      send_to_char("You go through the motions yet you sense your timing is all wrong.\n\r", ch);
      affect_to_char(ch, &af);
      return;
    }
  
//Place
  if (ch->in_room->vnum != deity_table[ch->pcdata->way].shrine && !fAutoPass)
    {
      send_to_char("You feel emptiness even as you begin the enchantement."\
		   "\n\rIts as if the dark powers were absent.\n\r", ch);
      affect_to_char(ch, &af);
      return;
    }
//we copy the data onto the dummy object and then destroy the old one.
  weapon = create_object( get_obj_index( OBJ_VNUM_MAL_WEAPON ), 0);
  clone_object(obj, weapon);
  
//minor changes here.
  weapon->cost = 1000;
  //  SET_BIT(weapon->wear_flags, ITEM_UNIQUE);
  obj_to_char(weapon, ch);

  //no we get rid of the duplicate removing from char first.
  //(gen's dont fire off that way.)
  obj_from_char(obj);
  extract_obj(obj);


//We strip all the exisign effecs. Rest is done byt the gen.
  while (weapon->affected)
    affect_remove_obj(weapon, weapon->affected);
  weapon->enchanted = TRUE;

//we set normal duration.
  af.duration = full_dur;
  affect_to_char(ch, &af);

//We start the gen up.
   af.type = gen_malform;
   af.where = TO_NONE;
   af.duration = -1;
   affect_to_obj(weapon, &af);
}//end spell_maleform_weapon



/* spell_soul_transfer */
/*written by Virigoth */
void spell_soul_transfer(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  AFFECT_DATA af;
  AFFECT_DATA* to_paf, *from_paf;
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  OBJ_DATA* wield = get_eq_char(ch, WEAR_WIELD);
  OBJ_DATA* weapon;

  const int full_dur = 400;
  const int fail_dur = 150;
  //bool
  bool fAutoPass = IS_IMMORTAL(ch);

//EZ
  if (ch==NULL)
    return;

  if (get_skill(ch, sn) < 75){
    send_to_char("Without proper skill this would surly kill you outright.\n\r", ch);
    return;
  }

/* look for held malformed weapon */
  if (wield == NULL 
      || (from_paf = affect_find(wield->affected, gen_malform)) == NULL){
    send_to_char("You must hold the malformed weapon from which to draw the souls.\n\r", ch);
    return;
  }

/* check target is a weapon */
  if (obj->item_type != ITEM_WEAPON){
    send_to_char("That item is not a weapon.\n\r",ch);
    return;
    }
/* make sure its not worn */
  if (obj->wear_loc != -1){
    send_to_char("The weapon to recive the transfer cannot be worn.\n\r",ch);
    return;
  }

/* check for no-malform on the weapon to be transfered to */
  if ( affect_find(obj->affected, gen_malform) != NULL){
    act("$p already carries the dark gift.", ch, obj, NULL, TO_CHAR);
    return;
  }

  if ( (ch->alignment > EVIL_THRESH) || IS_SET(ch->act, PLR_OUTCAST) 
       || is_affected(ch, skill_lookup("calm")) )
    {
      send_to_char("You lack the malice neccassary.", ch);
      return;
    }
  if (IS_SET(obj->extra_flags, ITEM_STAIN)){
    send_to_char("Cannot malform `!STAINED`` item.\n\r", ch);
    return;
  }

  if (IS_OBJ_STAT(obj,ITEM_BLESS)){
    send_to_char("The blessing on the item seems to block your attempts.", ch);
    return;
  }
//rediness?
  if (is_affected(ch, sn) && !fAutoPass)
    {
      send_to_char("You are not ready yet for another attempt.\n\r", ch);
      return;
    }

//These cause the gsn to be set.
//The effect is not actuly applied till something fails.
  af.type = sn;
  af.level = level;
  af.duration = fail_dur;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.modifier = 0;
  af.location = APPLY_NONE;

  
//Time
  if (mud_data.time_info.hour != 0 && !fAutoPass){
    send_to_char("You go through the motions yet you sense your timing is all wrong.\n\r", ch);
    affect_to_char(ch, &af);
    return;
  }
  
//Place
  if (ch->in_room->vnum != deity_table[deity_lookup(ch->pcdata->deity)].shrine && !fAutoPass){
    send_to_char("You feel emptiness even as you begin the enchantement."\
		 "\n\rIts as if the dark powers were absent.\n\r", ch);
    affect_to_char(ch, &af);
    return;
  }
//we copy the data onto the dummy object and then destroy the old one.
  weapon = create_object( get_obj_index( OBJ_VNUM_MAL_WEAPON ), 0);
  clone_object(obj, weapon);
  
//minor changes here.
  weapon->cost = 1000;
  //  SET_BIT(weapon->wear_flags, ITEM_UNIQUE);
  obj_to_char(weapon, ch);

  //now we get rid of the duplicate removing from char first.
  //(gen's dont fire off that way.)
  obj_from_char(obj);
  extract_obj(obj);
  

//We strip all the exising effects. Rest is done byt the gen.
  while (weapon->affected)
    affect_remove_obj(weapon, weapon->affected);
  weapon->enchanted = TRUE;

//we set normal duration.
  af.duration = full_dur;
  affect_to_char(ch, &af);

//We start the gen up.
   af.type = gen_malform;
   af.where = TO_NONE;
   af.duration = -1;
   to_paf = affect_to_obj(weapon, &af);
/* all we need to do to make sure the weapon is the same, is copy the 
   bitvector which stores the future modifications chosen */
   to_paf->bitvector = from_paf->bitvector;
/* 1/4 of the level is lost */
   to_paf->modifier = 3 * from_paf->modifier / 4;
/* get rid of the brand flag */
   REMOVE_BIT(to_paf->bitvector, WEP_NAMED);
   act("\n\rIn a scream of everlasting oblivion the souls are transfered into $p!",
       ch, obj, NULL, TO_CHAR);
   obj_from_char( wield );
   act("$p falls apart into ashes.", ch, wield, NULL, TO_ALL);
   extract_obj( wield );
}//end spell_maleform_weapon


void spell_mimic( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    if (is_affected(ch, gsn_steel_wall)){
      send_to_char("You must first get rid of the steel wall protecting you.\n\r", ch);
      return;
    }
    if (is_affected(ch,gsn_mimic))
    {
        send_to_char("The reflective shield is already in existance around you.\n\r", ch);
        return;
    }	
    af.type   = sn;
    af.level  = level;
    af.duration = level/10;
    af.location = APPLY_NONE;
    af.modifier = number_range(1, 5);
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    affect_to_char(ch,&af);
    send_to_char("You create a shimmering reflective shield around your persona.\n\r",ch);
    act("Your visage is reflected a thousand times as a shimmering mirror like sphere surrounds $n.", ch, NULL, NULL, TO_ROOM);
}


void spell_metabolic_boost( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if ( is_affected( victim, gen_metabolic_boost ) ){
    if (victim == ch)
      act("You can't move any faster!",ch,NULL,victim,TO_CHAR);
    else
      act("$N is already moving as fast as $E can.", ch,NULL,victim,TO_CHAR);
    return;
  }
  af.where     = TO_AFFECTS;
  af.type      = gen_metabolic_boost;
  af.level     = level;
  if (victim == ch)
    af.duration  = level / 4;
  else
    af.duration  = level / 4;
  af.location  = APPLY_DEX;
  af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
  af.bitvector = AFF_HASTE;
  affect_to_char( victim, &af );
  act_new( "You feel your metabolism accelerate.", victim,NULL,NULL,TO_CHAR,POS_DEAD);
  act("$n begins moving more quickly.",victim,NULL,NULL,TO_ROOM);
}

void spell_metabolic_brake( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if ( is_affected( victim, sn ) 
       || IS_AFFECTED(victim,AFF_SLOW) ){
    if (victim == ch)
      act("You can't move any slower!",ch,NULL,victim,TO_CHAR);
    else
      act("$N is already moving as slow as $E can.", ch,NULL,victim,TO_CHAR);
    return;
  }
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  if (victim == ch)
    af.duration  = level / 8;
  else
    af.duration  = level / 8;
  af.location  = APPLY_DEX;
  af.modifier  = -1 - (level >= 18) - (level >= 25) - (level >= 32);
  af.bitvector = AFF_SLOW;
  affect_to_char( victim, &af );
  act_new( "You feel your metabolism slow to crawl.", victim,NULL,NULL,TO_CHAR,POS_DEAD);
  act("$n begins moving very slowly.",victim,NULL,NULL,TO_ROOM);
}

/* allows a psi to cancel any PSI spell that affects him */
/* syntax: cancel <spell name> */
void do_cancel( CHAR_DATA* ch, char* argument ){
  AFFECT_DATA* paf, *paf_next;
  int csn = 0;

  if (ch->class != gcn_psi){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (!IS_NULLSTR(argument)){
    if ( (csn = skill_lookup(argument)) < 1){
      send_to_char("No such spell or skill.\n\r",ch);
      return;
    }
    else if (!IS_GEN(csn) && get_skill(ch, csn) < 2){
      send_to_char("You are not proficient in that.\n\r", ch);
      return;
    }
  }
  else
    csn = 0; //cancel all spells

  /* csn checks */
  if (csn){
    char *spell_name = IS_GEN(csn) ? effect_table[GN_POS(csn)].name : skill_table[csn].name;
    if (!IS_GNBIT(csn, GN_CANCEL) || !IS_GNBIT(csn, GN_PSI)){
      sendf(ch, "Spell of %s cannot be cancelled.\n\r", spell_name);
      return;
    }
    if (!is_affected(ch, csn)){
      sendf(ch, "You are not affected by %s.\n\r", spell_name);
      return;
    }
  }

  /* now we cancel the spell/spells */
  /* check if there is anything to dispel */
  if (csn < 1){
    for (paf = ch->affected; paf; paf = paf_next){
      paf_next = paf->next;

      /* check if we can dispel */
      if (IS_GNBIT(paf->type, GN_CANCEL) && IS_GNBIT(paf->type, GN_PSI))
	break;
    }
    if (paf == NULL){
      send_to_char("There are no more spells to cancel.\n\r", ch);
      return;
    }
  }

  if (cancel_psi_spells( ch, csn ))
    send_to_char("Ok.\n\r", ch );
  else
    send_to_char("Failed.\n\r", ch );
    
}

/* removes a PSI CANCEL able spell from the list.
   csn of 0 means all
   csn 0 < 0 means only first spell
   otherwise try to remove given spell
*/
bool cancel_psi_spells( CHAR_DATA* ch, int csn ){
  AFFECT_DATA* paf, *paf_next;
  bool found = FALSE;


  /* csn checks */
  if (csn){
    if (!IS_GNBIT(csn, GN_CANCEL) || !IS_GNBIT(csn, GN_PSI)){
      return FALSE;
    }
    if (!is_affected(ch, csn)){
      return FALSE;
    }
  }

  /* now we cancel the spell/spells */
  /* check if there is anything to dispel */
  if (csn < 1){
    for (paf = ch->affected; paf; paf = paf_next){
      paf_next = paf->next;

      /* check if we can dispel */
      if (IS_GNBIT(paf->type, GN_CANCEL) && IS_GNBIT(paf->type, GN_PSI))
	break;
    }
    if (paf == NULL){
      return FALSE;
    }
  }
  
  /* begin checking spells */
  for (paf = ch->affected; paf; paf = paf_next){
    paf_next = paf->next;
    /* always skip to the last duplicate affect */
    if (paf_next && paf_next->type == paf->type)
      continue;
    if (csn && paf->type != csn)
      continue;

    /* check if we can dispel */
    if (!IS_GNBIT(paf->type, GN_CANCEL) || !IS_GNBIT(paf->type, GN_PSI))
      continue;
    
    /* can dispel, check */
    if (check_dispel(-99, ch, paf->type)){
      found = TRUE;
      /* if negative csn we only dispel the first spell on the list */
      if (csn < 0)
	return found;
    }
  }
  if (found){
    return TRUE;
  }
  else
    return FALSE;
}


void spell_magic_eye( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *eye;

    if (get_summoned(ch, MOB_VNUM_MAGIC_EYE) >= 1 && !IS_IMMORTAL(ch)){
      send_to_char("You can control only a single eye at a time.\n\r", ch);
      return;
    }


    eye = create_mobile( get_mob_index(MOB_VNUM_MAGIC_EYE) );
    char_to_room(eye,ch->in_room);
    eye->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;

    /* Only leader, no charm etc. Hence cannot be ordered */
    eye->leader = ch;
    eye->summoner = ch;
    SET_BIT(eye->special, SPECIAL_WATCH);

    /* expires in 8h */
    af.where     = TO_AFFECTS;
    af.type      = gsn_timer;
    af.level     = level;
    af.duration  = 8;
    af.modifier  = 0;  
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( eye, &af );

    send_to_char("You create a magic eye to observe the area.\n\r", ch);
    act("A large eye fades out of sight above $n.", ch, NULL, NULL, TO_ROOM);
}

/* allows to cast spells using mind alone */
void spell_subvocalize( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn)){
    if (ch == victim){
      send_to_char("You are already subvocalizing spells.\n\r", ch);
    }
    else
      send_to_char("They are already subvocalizing spells.\n\r", ch);
  }

  af.type = sn;
  af.level = level;
  af.duration = 24;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char( victim, &af);
  send_to_char("You will now subvocalize spells.\n\r", victim);
}

/* mass dispel magic */
void spell_remove_magic (int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  CHAR_DATA *vch, *vch_next;

  /* first hit the target */
  spell_dispel_magic( gsn_dispel_magic, level, ch, victim, target) ;

  /* then anyone grouped with target */
  for (vch = victim->in_room->people; vch; vch = vch_next){
    vch_next = vch->next_in_room;
    
    if (!is_same_group(vch, victim))
      continue;
    else if (vch == victim)
      continue;
    else if (is_area_safe_quiet( vch, ch))
      continue;
    else
      spell_dispel_magic( gsn_dispel_magic, level, vch, victim, target) ;
    m_yell(ch,vch,FALSE);
    if (vch->fighting == NULL)
      set_fighting(vch, ch );
  }
}

/* USES GN_PSI_STRONG/MEDIUM/WEAK to check if save ignore should be made *
 * WEAK    succeeds on < 55 (-15) saves mental
 * MEDIUM  succeeds on < 70 (-30) saves mental
 * STRONG  succeeds on < 85 (-45) saves mental

 * Save calculation is made as per saves_spell
 */
bool psiamp_failed( CHAR_DATA* ch, CHAR_DATA* victim, int level, int dam_type, int spell_type, int gn_bits ){
  int saves = 0;
  bool fAmp = TRUE;

  if (spell_type == SPELL_MALEDICTIVE)
    return TRUE;
  else if (number_percent() > get_skill(ch, gsn_psi_amp)){
    check_improve(ch, gsn_psi_amp, FALSE, 10);
    return TRUE;
  }
  else
    saves = calc_saves(level, victim, dam_type, spell_type );

  check_improve(ch, gsn_psi_amp, TRUE, 5);

  if (IS_SET(gn_bits, GN_PSI_STR)){
    if (saves < 85)
      fAmp =  FALSE;
  }
  else if (IS_SET(gn_bits, GN_PSI_MED)){
    if (saves < 70)
      fAmp =  FALSE;
  }
  else if (IS_SET(gn_bits, GN_PSI_WEE)){
    if (saves < 55)
      fAmp =  FALSE;
  }

  if (mud_data.mudport == TEST_PORT)
    sendf( ch, "Amplified? %s\n\r", fAmp ? "NO" : "YES" );
  return fAmp;
}
  

/* used with psi_amp_failed(..) to ignore save checks when active */
/* Viri: AUTOMATIC NOW
void spell_psi_amp (int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn)){
    if (ch == victim){
      send_to_char("You are already amplifying spells.\n\r", ch);
    }
    else
      send_to_char("They are already amplifying spells.\n\r", ch);
  }

  af.type = sn;
  af.level = level;
  af.duration = number_fuzzy(7);
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char( victim, &af);
  send_to_char("You begin to amplify your spells.\n\r", victim);
}
*/

void spell_mindsurge (int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn)){
    if (ch == victim){
      send_to_char("You are already under effects of mindsurge.\n\r", ch);
    }
    else
      send_to_char("They are already under effects of mindsurge.\n\r", ch);
  }
  
  af.type = sn;
  af.level = level;
  af.duration = 6;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_MANA_GAIN;
  af.modifier = -25;
  affect_to_char( victim, &af);
  send_to_char("Your mind begins to surge with destructive forces.\n\r", victim);
}

void kinesis_setup( CHAR_DATA* ch, CHAR_DATA* victim, int sn, int level, int target){
  AFFECT_DATA af, *paf;

  /* additional effects based on target (-1 == autocast) */
  if (target >= 0){
    if ( (paf = affect_find(victim->affected, gen_kinesis)) != NULL){
      send_to_char("You recharge the kinesis.\n\r", ch);
      paf->modifier = level / 5;
      paf->bitvector = sn;
      paf->duration = 1;
      string_to_affect(paf, ch->name);
    }
    else{
      af.type = gen_kinesis;
      af.level = level;
      af.duration = 1;
      af.where = TO_NONE;
      af.bitvector = sn;//spell to cast
      af.location = APPLY_NONE;
      af.modifier = level / 5;//auto-cast on odd numbers counting down
      paf = affect_to_char(victim, &af);
      string_to_affect(paf, ch->name);
    }
  }
}

void spell_pyro_kinesis(int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam = dice(5, 10) + 3 * level / 2;

  if (is_affected(ch, gsn_mindsurge))
    dam = 50 + 3 * level / 2;

  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    dam /= 2;
  }

  damage( ch, victim, dam, sn, DAM_FIRE,target > 0 ? TRUE : FALSE);  
  kinesis_setup(ch, victim, sn, level, target );
}


void spell_cryo_kinesis(int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam = dice(5, 10) + 3 * level / 2;

  if (is_affected(ch, gsn_mindsurge))
    dam = 50 + 3 * level / 2;

  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    dam /= 2;
  }

  damage( ch, victim, dam, sn, DAM_COLD,target > 0 ? TRUE : FALSE);  
  kinesis_setup(ch, victim, sn, level, target );
}


void spell_hydro_kinesis(int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam = dice(5, 10) + 3 * level / 2;

  if (is_affected(ch, gsn_mindsurge))
    dam = 50 + 3 * level / 2;

  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    dam /= 2;
  }

  damage( ch, victim, dam, sn, DAM_DROWNING,target > 0 ? TRUE : FALSE);  
  kinesis_setup(ch, victim, sn, level, target );

}


void spell_electro_kinesis(int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam = dice(5, 10) + 3 * level / 2;
  int armor = 0;

  if (is_affected(ch, gsn_mindsurge))
    dam = 50 + 3 * level / 2;  

  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    dam /= 2;
  }

  armor = GET_AC(victim, AC_PIERCE) + GET_AC(victim, AC_BASH) + GET_AC(victim, AC_SLASH);
  armor /= -3;
  if (armor > 225){
    act("Sparks arc around $n's armor!", victim, NULL, NULL, TO_ROOM);
    act("Sparks arc around your armor!", victim, NULL, NULL, TO_CHAR);
  }
  dam += UMIN(50, armor / 10);

  damage( ch, victim, dam, sn, DAM_LIGHTNING,target > 0 ? TRUE : FALSE);  
  kinesis_setup(ch, victim, sn, level, target );
}


void spell_tele_kinesis(int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam = dice(5, 10) + 3 * level / 2;

  if (is_affected(ch, gsn_mindsurge))
    dam = 50 + 3 * level / 2;

  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    dam /= 2;
  }

  damage( ch, victim, dam, sn, DAM_AIR, target > 0 ? TRUE : FALSE);  
  kinesis_setup(ch, victim, sn, level, target );
}

void spell_photon_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = dice(level / 5, 12) + 4 * level;

    if (is_affected(ch, gsn_mindsurge))
      dam = 120 + 4 * level;

    if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
	&& saves_spell(level, victim, DAM_LIGHT,skill_table[sn].spell_type))
      dam /= 2;
    if (IS_UNDEAD(victim)){
      /* drain on failed save */
      AFFECT_DATA af, *paf;
      if ( (paf = affect_find(victim->affected, gsn_drained)) == NULL
	   || paf->duration >= 0){
	af.type		= gsn_drained;
	af.level	= ch->level;
	af.duration	= 3;
	af.where	= TO_AFFECTS;
	af.bitvector	= 0;
	af.location	= APPLY_NONE;
	af.modifier	= 0;
	affect_join( victim, &af );
	if (paf == NULL){
	  send_to_char("You feel weak...\n\r",victim);
	  act("$n stumbles under $s weight.", victim, NULL, NULL, TO_ROOM);
	}
	else{
	  send_to_char("You feel weaker...\r",victim);
	  act("$n seems weaker.", victim, NULL, NULL, TO_ROOM);
	}
      }
    }
    damage( ch, victim, dam, sn, DAM_HOLY, TRUE);
}


void spell_singularity( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam = dice(5, 10) + 7 * level;
  bool fSave = FALSE;
  bool fFlier = IS_SET(race_table[victim->race].aff, AFF_FLYING) ? TRUE : FALSE;
  if (is_affected(ch, gsn_mindsurge))
    dam = 50 + 7 * level;

  /* affect the damage based on size */
  dam = dam * victim->size / SIZE_MEDIUM;

  act("With a howl of wind a sphere of pulsing black forms below $n!", victim, NULL, victim, TO_ROOM);
  send_to_char("With a howl of wind a sphere of pulsing black forms below you!\n\r", victim);

  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    dam /= 2;
    fSave = TRUE;
  }
  
  if ( check_immune(victim, DAM_BASH, TRUE) > IS_IMMUNE){
    /* fliers get broken wings */
    if (fFlier ){
      if ( !is_affected(victim, gen_move_dam)){
	AFFECT_DATA af, *paf;
	af.type = gen_move_dam;
	af.level = level;
	af.duration = number_range(2, 4);
	af.where = TO_AFFECTS;
	af.bitvector = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	paf = affect_to_char(victim, &af);
	if (!IS_NPC(ch)){
	  string_to_affect(paf, ch->name);
	}
	act("$n's wings are almost torn off!", victim, NULL, victim, TO_ROOM);
	act("Your wings snap and break!", ch, NULL, victim, TO_VICT);
      }
      fSave = FALSE;
    }
    if (!fSave 
	&& IS_AFFECTED(victim, AFF_FLYING) && !is_affected(victim,gsn_thrash)){
      AFFECT_DATA af;
      int dam = 100 + dice(7, 7);

      af.where	= TO_AFFECTS;
      af.type   = gsn_thrash;
      af.level  = ch->level;
      af.duration = fFlier ? 12 : 6;
      af.modifier = 0;
      af.location = 0;
      af.bitvector = 0;
      affect_to_char(victim,&af);

      dam = URANGE(0, victim->hit - 5, dam);
      damage( victim, victim, dam, gsn_kinesis, DAM_INTERNAL, TRUE);  

      act("You can't seem to get back into the air",victim,NULL,NULL,TO_CHAR);
      act("$n can't seem to get back into the air",victim,NULL,NULL,TO_ROOM);
    }    
  }
  damage( ch, victim, dam, sn, DAM_BASH, TRUE);  
}


void spell_entropic_touch( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af, *paf;

  act("With a deft motion you place your palm against $N's armor!", ch, NULL, victim, TO_CHAR);
  act("With a deft motion $n places $s palm against $N's armor!", ch, NULL, victim, TO_NOTVICT);
  act("With a deft motion $n places $s palm against your armor!", ch, NULL, victim, TO_VICT);

  if (is_affected(victim, sn)){
    send_to_char("Their armor has already been melted away!\n\r", ch);
    damage(ch, victim, 0, sn, DAM_ENERGY, TRUE);
    return;
  }

  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    damage(ch, victim, 0, sn, DAM_ENERGY, TRUE);
    return;
  }
  act("With a hiss of toxic fumes your armor turns to sludge!", victim, NULL, victim, TO_CHAR);
  act("With a hiss of toxic fumes $n's armor turns to sludge!", victim, NULL, victim, TO_ROOM);

  af.type	= sn;
  af.duration	= 12;
  af.level	= level;
  af.where	= TO_AFFECTS;
  af.bitvector	= AFF_FAERIE_FIRE;
  af.location	= APPLY_AC;
  af.modifier	= URANGE(20, -(GET_AC(victim, AC_PIERCE) + GET_AC(victim, AC_SLASH) + GET_AC(victim, AC_BASH)) / 3, 500);
  affect_to_char( victim, &af);
  
  af.type	= gen_create_object;
  af.duration	= UMAX(2, af.modifier / 50);
  af.where	= TO_NONE;
  af.bitvector	= 50;
  af.location	= APPLY_NONE;
  af.modifier	= OBJ_VNUM_ENTROPIC_SLUDGE;
  paf = affect_to_char( victim, &af);
  string_to_affect(paf, "Your armor stops dripping toxic sludge.");
}


void spell_amorphous_infection( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af, *paf;
  int sp_level = level;

  act("You attempt to summon the creature to feed on $N.", ch, NULL, victim, TO_CHAR);

  if (is_affected(victim, sn)){
    send_to_char("They have already been infected!\n\r", ch);
    return;
  }

  /* increase spell level for psiamp check here to boost the amplification
     one category for unnatural creatures
  */
  if (IS_UNDEAD(victim) || IS_DEMON(victim) || IS_AVATAR(victim))
    sp_level += 10; //(+30 svs spell means 2 category from WEAK to STRONG)
  
  if (psiamp_failed(ch, victim, sp_level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    send_to_char("You failed.\n\r", ch);
    return;
  }

  act("The creature burrows into $N!",ch, NULL, victim, TO_CHAR);
  send_to_char("Your skin feels funny for a moment..", victim);

  af.type	= gen_ainfection;
  af.duration	= -1;
  af.level	= level;
  af.where	= TO_NONE;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 10; //times the creature will leave host before death
  paf = affect_to_char( victim, &af);
  string_to_affect(paf, ch->name );
}

void spell_disintegrate( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo, *vch;
  AFFECT_DATA af;
  int chance = 0;
  int dam = 0;

  if (is_affected(victim, sn)){
    send_to_char("You failed.\n\r", ch);
    damage(ch, victim, number_range(20, 100), sn, DAM_INTERNAL, TRUE);
    return;
  }

  act("$N screams in agony as $S molecules come apart!", ch, NULL, victim, TO_CHAR);
  act("$N screams in agony as $S molecules come apart!", ch, NULL, victim, TO_NOTVICT);
  act("You scream in agony as your molecules to come apart!", ch, NULL, victim, TO_VICT);

  for ( vch = char_list; vch != NULL; vch = vch->next ){
    if (vch->in_room && vch->in_room->area == victim->in_room->area
	&& vch->in_room != victim->in_room)
      act("You hear $N's distant scream of horrible agony.", vch, NULL, victim, TO_CHAR);
  }

  dam = victim->max_hit / 4;

  af.type	= sn;
  af.level	= level;
  af.duration	= 12;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_HIT;
  af.modifier	= -dam;

  affect_to_char( victim, &af);

  chance = 50 + (get_curr_stat(victim, STAT_CON) - 19) * 10;
  if (number_percent() < chance){
    send_to_char("You manage to partially resist the disintegration.\n\r", victim);
  }
  else{
    af.type	= gsn_drained;
    af.level	= level;
    af.duration	= 12;
    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location	= APPLY_CON;
    af.modifier	= -10;
    affect_to_char( victim, &af);
  }
  damage(ch, victim, dam, sn, DAM_INTERNAL, TRUE );

}

void spell_breach( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo, *vch;
  AFFECT_DATA* paf, *paf_next;
  bool found = FALSE;


  act("A lance of blinding blue energy streaks from you to $N!", ch, NULL, victim, TO_CHAR);
  act("A lance of blinding blue energy streaks from $n to you!", ch, NULL, victim, TO_VICT);
  act("A lance of blinding blue energy streaks from $n to $N!", ch, NULL, victim, TO_NOTVICT);

  for ( vch = char_list; vch != NULL; vch = vch->next ){
    if (vch->in_room && vch->in_room->area == victim->in_room->area
	&& vch->in_room != victim->in_room)
      act("You see a flash of blinding blue light in near distance.", vch, NULL, victim, TO_CHAR);
  }

  /* quick check to make sure there is anything left to dispel */
  for (paf = victim->affected; paf; paf = paf_next){
    paf_next = paf->next;
    /* look for beneficial protective skills, or any beneficial GEN */
    if (IS_GNBIT(paf->type, GN_BEN)
	&& (IS_GEN(paf->type) 
	    || skill_table[paf->type].spell_type == SPELL_PROTECTIVE)){
      break;
    }
  }
  if (paf == NULL || IS_NPC(victim)){
    send_to_char("There is nothing to breach.\n\r", ch);
    damage(ch, victim, number_range(50, 100), sn, DAM_INTERNAL, TRUE);
    return;
  }

  /* look for mana lock */
  if (is_affected(victim, gsn_mana_lock)){
    check_dispel(-99, victim, gsn_mana_lock);
    damage(ch, victim, number_range(50, 100), sn, DAM_INTERNAL, TRUE);
    return;
  }
  
  /* begin checking spells */
  for (paf = victim->affected; paf; paf = paf_next){
    paf_next = paf->next;
    /* always skip to the last duplicate affect */
    if (paf_next && paf_next->type == paf->type)
      continue;
    /* check if we can dispel */
    if (IS_GNBIT(paf->type, GN_BEN)
	&& (IS_GEN(paf->type) 
	    || skill_table[paf->type].spell_type == SPELL_PROTECTIVE)){

      /* can dispel, check */
      if (check_dispel(-99, victim, paf->type)){
	found = TRUE;
      }
    }
  }

  if (found){
    /* CONTINENCY */
    check_contingency(victim, NULL, CONT_DISPEL);
  }
  else
    send_to_char("Spell failed.\n\r",ch);
  damage(ch, victim, number_range(50, 100), sn, DAM_INTERNAL, TRUE);
}


/* 
sets up the given spell as the dream cast spell, increases level on existing
*/
void dream_setup( CHAR_DATA* ch, int sn){
  AFFECT_DATA *paf;

  if ( (paf = affect_find(ch->affected, gen_dreamwalk)) == NULL){
    bug("dream_setup: Could not find dreamwalk effect.", 0);
    return;
  }
  paf->modifier = sn;
  paf->level = 0;
  paf->duration = 2;
}

void spell_omen(int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;

  if (target >= 0){
    if ( (victim = get_char( target_name )) == NULL){
      send_to_char("You can't seem to enter the dream.\n\r", ch );
      return;
    }
    if (is_affected(victim, gsn_hysteria)){
      send_to_char("They are as scared as they are going to get.\n\r", ch);
      return;
    }
    dream_setup( ch, sn );
    act("With some effort you give life to $N's personal nightmare.", ch, NULL, victim, TO_CHAR);
    return;
  }
  else{
    AFFECT_DATA* paf, af;
    send_to_char("(Dream)Your greatest fears come to life before you!\n\r", victim);
    act("$N cries out in $S sleep.", victim, NULL, victim, TO_ROOM);

    /* check for strong enough fear */
    if ( (paf = affect_find(victim->affected, gsn_fear)) != NULL
	 && paf->modifier >= 30){

      act_new("You start to panic as you enter a state of hysteria!",ch,NULL,victim, TO_VICT,POS_DEAD);
      act("$N starts to panic from $S hysteria!",ch,NULL,victim,TO_CHAR);

      af.type		= gsn_hysteria;
      af.level		= level;
      af.duration	= 8;
      af.where		= TO_AFFECTS; 
      af.bitvector	= 0;
      af.location	= APPLY_NONE;
      af.modifier	= 0;
      affect_to_char(victim, &af);      
      affect_strip(victim, gsn_fear);
      affect_strip(ch, gen_dreamwalk);
    }
    else{
      af.type		= gsn_fear;
      af.level		= level;
      af.duration	= 3;
      af.where		= TO_AFFECTS; 
      af.bitvector	= 0;
      af.location	= APPLY_NONE;
      af.modifier	= 10;
      affect_join(victim, &af);      
    }
  }
}
  


void spell_deathmare(int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;

  if (target >= 0){
    if ( (victim = get_char( target_name )) == NULL){
      send_to_char("You can't seem to enter the dream.\n\r", ch );
      return;
    }
    dream_setup( ch, sn );
    act("With some effort you give a more interesting twist to $N's dreams.", ch, NULL, victim, TO_CHAR);
    return;
  }
  else{
    AFFECT_DATA af, *paf;

    /* In Order of appearance 
       1) Blindness(eyes)
       2) Weakness (arms)
       3) Enfeeble (legs)
       4) Wounding (stomach)
    */
    send_to_char("(Dream)You find yourself upon a table, a cloaked figure beside you.\n\r", victim);
    act_new("(Dream)You place $N in a torture chamber and stand above the rack.", ch, NULL, victim, TO_CHAR, POS_DEAD);
    /* EYES */
    if (!is_affected(victim, gsn_blindness)){
      af.type		= gsn_blindness;
      af.level		= level;
      af.duration	= 8;
      af.where		= TO_AFFECTS;
      af.bitvector	= AFF_BLIND;
      af.location	= APPLY_NONE;
      af.modifier	= 0;
      affect_to_char(victim, &af);
      send_to_char("(Dream)The cloaked figure lowers a red hot poker towards your face!\n\r", victim );
      act_new("(Dream)You conjure a red hot poker and aim at $N's eyes!", ch, NULL, victim, TO_CHAR, POS_DEAD);
      act_new("You are blinded!", victim,NULL,NULL,TO_CHAR, POS_DEAD);
      act("$n's eyes begins to bleed.", victim, NULL, NULL, TO_ROOM);
      return;
    }
    else if (!is_affected(victim, gsn_weaken)){
      af.type		= gsn_weaken;
      af.level		= level;
      af.duration	= 8;
      af.where		= TO_AFFECTS;
      af.bitvector	= 0;
      af.location	= APPLY_STR;
      af.modifier	= -1 * (level / 12);
      affect_to_char(victim, &af);
      act_new("(Dream)The cloaked figure swings a massive hammer at your wrists!", victim, NULL, victim, TO_CHAR, POS_DEAD);
      act_new("(Dream)You conjure a large hammer and aim at $N's wrists!", ch, NULL, victim, TO_CHAR, POS_DEAD);
      act_new("You cry in agony as your wrists are pulverized!", victim,NULL,NULL,TO_CHAR, POS_DEAD);
      act("$n's wrists begins to bleed.", victim, NULL, NULL, TO_ROOM);
      return;
    }
    else if (!is_affected(victim, gsn_enfeeblement)){
      af.type		= gsn_enfeeblement;
      af.level		= level;
      af.duration	= 8;
      af.where		= TO_AFFECTS;
      af.bitvector	= 0;
      af.location	= APPLY_DEX;
      af.modifier	= -1 * (level / 12);
      affect_to_char(victim, &af);
      act_new("(Dream)The cloaked figure swings a massive hammer at your knees!", victim, NULL, victim, TO_CHAR, POS_DEAD);
      act_new("(Dream)You conjure a large hammer and aim at $N's knees!", ch, NULL, victim, TO_CHAR, POS_DEAD);
      act_new("You cry in agony as your knees are shattered!", victim,NULL,NULL,TO_CHAR, POS_DEAD);
      act("$n's knees begins to bleed.", victim, NULL, NULL, TO_ROOM);
      return;
    }
    /* STOMACH */
    else if (!is_affected(victim, gen_move_dam)){
      af.type		= gen_move_dam;
      af.level		= level;
      af.duration	= 3;
      af.where		= TO_AFFECTS;
      af.bitvector	= 0;
      af.location	= APPLY_NONE;
      af.modifier	= 0;
      paf = affect_to_char(victim, &af);
      if (!IS_NPC(ch)){
	string_to_affect(paf, ch->name);
      }
      act_new("(Dream)The cloaked figure makes a cut in your gut and reaches in!", victim, NULL, victim, TO_CHAR, POS_DEAD);
      act_new("(Dream)You begin to draw $N's intestines out onto the table!", ch, NULL, victim, TO_CHAR, POS_DEAD);
      act_new("You cry in agony as your intestines are drawn!", victim,NULL,NULL,TO_CHAR, POS_DEAD);
      act("$n's stomach begins to bleed.", victim, NULL, NULL, TO_ROOM);
    }
    else{
      send_to_char("You moan in agony.\n\r", victim);
      send_to_char("There is nothing else you can do to them.\n\r", ch );
    }
  }
}

void spell_mindmelt(int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA* paf;

  if (target >= 0){
    if ( (victim = get_char( target_name )) == NULL){
      send_to_char("You can't seem to enter the dream.\n\r", ch );
      return;
    }
    if ( (paf = affect_find(victim->affected, sn)) != NULL
	 && paf->modifier >= 20){
      send_to_char("You cannot weaken their mind further.\n\r", ch);
      return;
    }
    dream_setup( ch, sn );
    act("You begin to weaken $N's mental defences.", ch, NULL, victim, TO_CHAR);
    return;
  }
  else{
    AFFECT_DATA af;
    send_to_char("(Dream)You feel an immense pressure upon your temples.\n\r", victim);
    act_new("(Dream)You bore into $N's mind.\n\r", ch, NULL, victim, TO_CHAR, POS_DEAD);
    act("$N squirms in $S sleep.", victim, NULL, victim, TO_ROOM);
    af.type		= sn;
    af.level		= level;
    af.duration		= 12;
    af.where		= TO_AFFECTS; 
    af.bitvector	= 0;
    af.location		= APPLY_SAVING_MENTAL;
    af.modifier		= 5;
    affect_join(victim, &af);      
  }
}

void dreamprobe_cast( CHAR_DATA* victim, char* msg ){
  DESCRIPTOR_DATA *d;
  AFFECT_DATA* paf;
  char buf[MIL];

  if (victim == NULL)
    return;
  else if (msg == NULL){
    if (!IS_AWAKE(victim))
      sprintf(buf, "(Dreamprobe)%s has fallen asleep.\n\r", PERS2(victim));
    else
      sprintf(buf, "(Dreamprobe)%s has awaken.\n\r", PERS2(victim));
    msg = buf;
  }
  for ( d = descriptor_list; d; d = d->next ){
    if ( d->connected == CON_PLAYING
	 && victim->in_room != d->character->in_room
	 && (paf = affect_find(d->character->affected, gsn_dreamprobe)) != NULL
	 && paf->has_string
	 && !str_cmp(paf->string, victim->name)){
      send_to_char(msg, d->character);
    }
  }//end for
}  
/* shows info to dreamwalker in order:
   1) Armor 
   2) Hit/Mana/Move
   3) Hitroll/Damroll 
   4) Saves
   Also allows sleep/wake messages on last dreamprobe victim, and remote
   dreamwalk
*/
void spell_dreamprobe(int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;

  if (target >= 0){
    if ( (victim = get_char( target_name )) == NULL){
      send_to_char("You can't seem to enter the dream.\n\r", ch );
      return;
    }
    dream_setup( ch, sn );
    act("You begin to probe $N's mind.", ch, NULL, victim, TO_CHAR);
    return;
  }
  else{
    AFFECT_DATA af, *paf;
    /* attach the dreamprobe for future wake/sleep messages */
    if ( (paf = affect_find(ch->affected, sn)) == NULL){
      af.type		= sn;
      af.level		= level;
      af.duration	= 48;
      af.where		= TO_AFFECTS;
      af.bitvector	= 0;
      af.location	= APPLY_NONE;
      af.modifier	= 0;
      paf = affect_to_char(ch, &af);
    }
    act_new("Your dreamprobe uncovers something useful.", ch, NULL, victim, TO_CHAR, POS_DEAD);
    /* reset dreamprobe link target and duration */
    string_to_affect(paf, victim->name );
    paf->duration = 48;

    /* show stats */
    switch(paf->modifier++){
    case 0:
      sendf( ch, "%s's Armor:\n\rPierce: %d  Bash: %d  Slash: %d  Magic: %d\n\r",
	     PERS2(victim),
	     GET_AC(victim, AC_PIERCE),
	     GET_AC(victim, AC_BASH),
	     GET_AC(victim, AC_SLASH),
	     GET_AC(victim, AC_EXOTIC));
      break;
    case 1:
      sendf( ch, "%s has %d/%d hit, %d/%d mana, %d/%d movement.\n\r",
	     PERS2(victim),
	     victim->hit, victim->max_hit,
	     victim->mana, victim->max_mana,
	     victim->move, victim->max_move);
      break;
    case 2:
      sendf( ch, "%s's Hitroll: %d  Damroll: %d.\n\r",
	     PERS2(victim),
	     GET_HITROLL(victim),
	     GET_DAMROLL(victim));
      break;
    case 3:
	sendf( ch, "%s's Save vs\n\rSpell: %d  Affliction: %d  Malediction: %d  Mental: %d\n\r", 
	       PERS2(victim),
	       victim->savingspell,
	       victim->savingaffl,
	       victim->savingmaled,
	       victim->savingmental);
	break;
    default:
      paf->modifier = 0;
    }
  }
}

/* Written by: Virigoth							*
 * Returns: void							*
 * Used: magic.c, magic3.c						*
 * Comment: A dispell magic like spell, except on failed save prevents  *
 * any defensive spells (TAR_SELF, TAR_CHAR_DEFENSIVE) from casting.	*/

void spell_mental_knife( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam = number_range(1, 5) * level / 2;

  
  /* dispel magic like effect on failed save */
  if (ch == victim){
    send_to_char("You failed.\n\r", ch);
    return;
  }
  
  af.type               = skill_lookup("mind blast");
  affect_strip(victim, af.type );
  af.level              = level; 
  af.duration           = number_range(1, 3);
  af.location           = APPLY_INT;
  af.modifier           = -5;
  af.bitvector          = 0;
  affect_to_char(victim,&af);

  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && !saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    damage(ch,victim, dam ,sn,DAM_MENTAL, TRUE);
    return;
  }
    
  act("Your mental knife sears $N's mind!",ch,NULL,victim,TO_CHAR);
  act("$n's mental knife sears your mind!",ch,NULL,victim,TO_VICT);
  act("$n's mental knife sears $N's mind!",ch,NULL,victim,TO_NOTVICT);
  
  /* attach the no-recast effect */
  af.type		= sn;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  af.duration	= number_range(1,3);
  affect_strip(victim, sn );
  affect_to_char(victim, &af );
  damage(ch,victim, dam ,sn,DAM_MENTAL, TRUE);
}

   
void spell_spell_vise( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af, *paf;
  int to_dispel = 4;

  /* look for mana lock */
  if (is_affected(victim, gsn_mana_lock)){
    if (!effect_manalock(sn, level, ch, victim))
      return;
    else
      send_to_char("Ok.\n\r", ch );
  }
  if (is_affected(victim, gen_svise)){
    send_to_char("You failed.\n\r", ch);
    return;
  }
  /* on save we cut number of spells to dispel by half */
  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && !saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type))
    to_dispel /= 2;

  act("A sparkling blue ring appears around $n's head.", victim, NULL, NULL, TO_ROOM);
  act("A sparkling blue ring appears around your head.", victim, NULL, NULL, TO_CHAR);
  af.type		= gen_svise;
  af.level		= level;
  af.duration		= 1;
  af.where		= TO_AFFECTS;
  af.bitvector	= 0;
  af.location		= APPLY_NONE;
  af.modifier		= to_dispel;//how many random spells to take off
  paf = affect_to_char( victim, &af);
  string_to_affect(paf, ch->name );
}  


void spell_mana_trap (int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn)){
    if (ch == victim){
      send_to_char("You are already trapping mana from spells.\n\r", ch);
    }
    else
      send_to_char("They are already trapping mana from spells.\n\r", ch);
  }
  
  af.type = sn;
  af.level = level;
  af.duration = number_fuzzy(8);
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char( victim, &af);
  send_to_char("You construct a mana trap and arm it.\n\r", victim);
}

void spell_spell_trap (int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn)){
    if (ch == victim){
      send_to_char("You are already trapping spells.\n\r", ch);
    }
    else
      send_to_char("They are already trapping spells.\n\r", ch);
    return;
  }
  
  af.type = sn;
  af.level = level;
  af.duration = number_fuzzy(8);
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char( victim, &af);
  send_to_char("You construct a spell trap and arm it.\n\r", victim);
}

/* transfers portion of victims hp/mana/move to caster */
void spell_soul_pump( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam = number_range(10, 50);
  int dur = number_fuzzy(12);

  int hit_mul, hit_dam;
  int move_mul, move_dam;
  int mana_mul, mana_dam;

  if (is_affected(victim, sn )){
    send_to_char("You failed.\n\r", ch);
    return;
  }

  /* gain is multiplied by the amount of %hp caster has lost */
  /* than scaled by % of hp that victim has */

  hit_mul = UMAX(0, 10 * (ch->max_hit - ch->hit) / (1 + ch->max_hit));
  hit_dam = UMAX(1, victim->hit * (dam * hit_mul) / (1 + victim->max_hit));

  move_mul = UMAX(0, 10 * (ch->max_move - ch->move) / ( 1 + ch->max_move));
  move_dam = UMAX(1, victim->move * (dam * move_mul) / (1 + victim->max_move));

  mana_mul = UMAX(0, 10 * (ch->max_mana - ch->mana) / (1 + ch->max_mana));
  mana_dam = UMAX(1, victim->mana * (dam * mana_mul) / (1 + victim->max_mana));

  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && !saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    act("$N resists your soul pump.", ch, NULL, victim, TO_CHAR);
    act("You resists $n's soul pump.", ch, NULL, victim, TO_VICT);
    hit_dam /= 2;
    move_dam /= 2;
    mana_dam /= 2;
    dur /= 2;
  }

  /* cap the damage */
  hit_dam = UMIN(victim->hit, hit_dam);
  hit_dam = UMIN(350, hit_dam);
  move_dam = UMIN(victim->move, move_dam);
  move_dam = UMIN(350, move_dam);
  mana_dam = UMIN(victim->mana, mana_dam);
  mana_dam = UMIN(500, mana_dam);
  

  if ( damage( ch, victim, hit_dam, sn, DAM_INTERNAL, TRUE)){
    AFFECT_DATA af;
    sendf(ch, "You gain %d health, %d mana and %d stamina!\n\r", 
	  hit_dam, mana_dam, move_dam );
    send_to_char("You feel your soul slipping away!\n\r", victim);
    ch->hit = UMIN( ch->max_hit, ch->hit + hit_dam);
    ch->move = UMIN( ch->max_move, ch->move + move_dam);
    victim->move -= move_dam;
    ch->mana = UMIN( ch->max_mana, ch->mana + mana_dam);
    victim->mana -= mana_dam;
    update_pos( ch );

    af.type	= sn;
    af.level	= level;
    af.duration	= dur;
    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location	= APPLY_HIT_GAIN;
    af.modifier	= -25;
    affect_to_char( victim, &af);
    af.location	= APPLY_MANA_GAIN;
    affect_to_char( victim, &af);
    af.location	= APPLY_MOVE_GAIN;
    affect_to_char( victim, &af);
  }
}

/* spell blast effect, checks if the spell was blasted and negated */
bool check_spellblast( CHAR_DATA* ch, CHAR_DATA* victim, int sn, int cost, int level ){
  AFFECT_DATA* paf;

  if (ch == NULL || victim == NULL)
    return FALSE;
  else if ( (paf = affect_find(victim->affected, gsn_spell_blast)) == NULL)
    return FALSE;

  /* check saves */
  if (psiamp_failed(victim, ch, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, ch, DAM_MENTAL,skill_table[sn].spell_type)){
    return FALSE;
  }
  paf->modifier -= cost * 3;
  act("Your spell blast destroys $t.",
      victim,
      skill_table[sn].name,
      ch,
      TO_CHAR);
  act("$n spell blast destroys $N's spell.",
      victim,
      NULL,
      ch,
      TO_NOTVICT);
  act("$n's spell blast destroys your $t.",
      victim,
      skill_table[sn].name,
      ch,
      TO_VICT);
  /* check for failure of effect on lack of mana */
  if (paf->modifier <= 0){
    if ( skill_table[paf->type].msg_off )
      act_new(skill_table[paf->type].msg_off,victim,NULL,NULL,TO_CHAR,POS_DEAD);
    if ( skill_table[paf->type].msg_off2 )
      act(skill_table[paf->type].msg_off2,victim,NULL,NULL,TO_ROOM);
    affect_strip(victim, paf->type);
  }
  return TRUE;
}

void spell_spell_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = ch;
  AFFECT_DATA af, *paf;
  int gain = UMIN(victim->max_mana / 3, atoi( target_name ));

  gain = URANGE(1, victim->mana, gain );

  if ( (paf = affect_find(victim->affected, sn )) != NULL){
    paf->modifier = UMIN(victim->max_mana / 3, paf->modifier + gain );
    victim->mana -= gain;
    sendf(victim, "Spell blast reserve now at %d mana.\n\r", paf->modifier );
    paf->modifier = 8;
    return;
  }

  af.type	= sn;
  af.level	= level;
  af.duration	= number_fuzzy(8);
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= gain;
  victim->mana -= gain;
  affect_to_char(victim, &af);
  sendf(victim, "Spell blast reserve now at %d mana.\n\r", af.modifier );  
}

/* checks if masochism kicks in */
/* returns the character that should damage the ch, or NULL */
CHAR_DATA* masochism_effect( CHAR_DATA* ch, int dam, int dt ){
  AFFECT_DATA* paf;
  CHAR_DATA* och;

  if ( dt < TYPE_HIT
      || dam < 1
      || (paf = affect_find(ch->affected, gsn_masochism)) == NULL
      || !paf->has_string
      || IS_NULLSTR(paf->string)
      || number_percent() > 35
      || (och = get_char( paf->string )) == NULL
      ){
    return NULL;
  }
  else
    return och;
}

void spell_masochism( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af, *paf;

  if ( is_affected( victim, sn )){
    if (victim == ch)
      act("You are already under effects of masochism.",ch,NULL,victim,TO_CHAR);
    else
      act("$N is already under effects of masochism.",ch,NULL,victim,TO_CHAR);
    return;
  }

  if (IS_UNDEAD(victim)
      || (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
	  && saves_spell( level, victim, DAM_MENTAL,skill_table[sn].spell_type))
      ){
    act("$N refuses your subconscious suggestion.", ch, NULL, victim, TO_CHAR);
    return;
  }

  af.where	= TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = number_fuzzy(6);
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = 0;
  paf = affect_to_char( victim, &af );
  string_to_affect( paf, ch->name );
  act_new( "You begin to wonder if it would feel good to hurt yourself.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
  if ( ch != victim )
    act("$N begins to subconsciously pinch $s skin.",ch,NULL,victim,TO_CHAR);
}
void spell_brain_death( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int res = 0;

  if ( is_affected( victim, sn )){
    send_to_char("They've already been lobotomized.\n\r", ch);
    return;
  }
  if ( (res = check_immune(victim, DAM_MENTAL, TRUE)) > IS_IMMUNE){
    CHAR_DATA* vch;
    for ( vch = char_list; vch != NULL; vch = vch->next ){
      if (vch->in_room && vch->in_room->area == victim->in_room->area
	  && vch->in_room != victim->in_room)
	act("You hear $N's distant agonizing scream.", vch, NULL, victim, TO_CHAR);
    }
    af.type	= sn;
    af.level	= level;
    af.duration	= number_fuzzy(14);
    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location	= APPLY_INT;
    af.modifier	= -1 * (UMAX(10, get_max_stat(victim, STAT_INT) / 2));
    affect_to_char( victim, &af );
    
    af.type	= gsn_forget;
    af.duration	= number_range(1, 2);
    af.where	= TO_AFFECTS;
    af.location	= APPLY_NONE;
    af.modifier	= 0;
    affect_to_char( victim, &af );

    send_to_char( "You have been LOBOTOMIZED!!\n\r\n\r", victim );  
    act("$n has been LOBOTOMIZED!!", victim, NULL, NULL, TO_ROOM );
  }
  damage(ch, victim, number_range(35, 75), sn, DAM_INTERNAL, TRUE );
}

/* restores all spells but removes total recall */
void spell_total_recall( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  DNDS_DATA* dnds;

  if (IS_NPC(victim))
    return;

  send_to_char("Nothing seems to have happened.\n\r", ch);
  act("$n's eyes flare brilliant blue for an instant.", ch, NULL, NULL, TO_ROOM);
  forget_dndtemplates( ch->pcdata );
  add_memticks(victim->pcdata, 1000, victim->class, victim->level);

  while ( (dnds = dnds_lookup( ch->pcdata, sn, -1 )) != NULL
	  && GET_DNDMEM(ch->pcdata, sn) > 0){
    useup_dnds( ch->pcdata, sn );
  }
}

void spell_unminding( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo, *vch;
  AFFECT_DATA af;
  int dam = 0;
  int res = 0;

  if ( is_affected( victim, sn )){
    send_to_char("They've already been unminded.\n\r", ch);
    return;
  }
  if ( (res = check_immune(victim, DAM_MENTAL, TRUE)) > IS_IMMUNE){
    for ( vch = char_list; vch != NULL; vch = vch->next ){
      if (vch->in_room && vch->in_room->area == victim->in_room->area
	  && vch->in_room != victim->in_room)
	act("You hear $N's distant agonizing scream.", vch, NULL, victim, TO_CHAR);
    }
    af.type	= sn;
    af.level	= level;
    af.duration	= number_fuzzy(14);
    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location	= APPLY_WIS;
    af.modifier	= -1 * (UMAX(10, get_max_stat(victim, STAT_WIS) / 2));
    affect_to_char( victim, &af );
    
    act("$n screams and tears at $s head!", victim, NULL, NULL, TO_ROOM);
    send_to_char("You scream in pain as your memories and experiences are turned against you!\n\r", victim);

    if (res == IS_RESISTANT){
      send_to_char("Your mental resistance has been compromised.\n\r", victim);
      act("$N's mental resistance has been compromised.", ch, NULL, victim, TO_CHAR);
    }
    if ( (res = check_immune(victim, DAM_CHARM, TRUE)) == IS_RESISTANT){
      send_to_char("Your resistance to charms has been compromised.\n\r", victim);
      act("$N's resistance to charms has been compromised.", ch, NULL, victim, TO_CHAR);
    }
  }
  if (IS_NPC(victim))
    dam = 50 + 8 * level;
  else
    dam = 50 + (pc_race_table[victim->race].class_mult - 1500 + class_table[victim->class].extra_exp) / 3;
  damage(ch, victim, dam, sn, DAM_INTERNAL, TRUE );
}

//fakes weapon type in defense checks
void spell_false_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = ch;
  AFFECT_DATA af;
  int weapon_type = number_range(0, WEAPON_STAFF );
  int val;

  if (is_affected(victim, sn)){
    send_to_char("You already have an illusion around your weapons.\n\r", victim);
    return;
  }
  else if ( (val = weapon_lookup( target_name )) >= 0)
    weapon_type = URANGE(0, val, WEAPON_STAFF);

  af.type	= sn;
  af.level	= level;
  af.duration	= number_fuzzy(level / 4);
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= weapon_table[weapon_type].type;
  affect_to_char( victim, &af );
  sendf( ch, "Your weapons will now appear as %ss.\n\r", weapon_table[weapon_type].name);
}

//causes perfect flee, and auto-invis once fled
void spell_shadow_door( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  /* after combat auto-invis part */
  if (target < 0){
    AFFECT_DATA* paf;
    //check if we can use photon wall to auto-invis
    if ( (paf = affect_find(victim->affected, gen_photon_wall)) != NULL){
      paf->modifier = 0;
      run_effect_update(ch, paf, NULL, paf->type, NULL, NULL, TRUE, EFF_UPDATE_INIT);
      return;
    }
    else{
      //cast invis and reduce duration to 1 
      spell_invis(gsn_invis, level, ch, ch, 0);
      if ( (paf = affect_find(victim->affected, gsn_invis)) != NULL)
	paf->duration = 1;
    }
    return;
  }
  else if (is_affected(victim, sn)){
    send_to_char("You're already affected by shadow door.\n\r", victim);
    return;
  }

  af.type	= sn;
  af.level	= level;
  af.duration	= number_fuzzy(level / 5);
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( victim, &af );
  send_to_char("You will now flee combat through a shadow door.\n\r", ch);
}
void spell_mirror_cloak( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;


  if (is_affected(victim, sn)){
    send_to_char("You're already surrounded by the cloak of mirrors.\n\r", victim);
    return;
  }

  af.type	= sn;
  af.level	= level;
  af.duration	= number_fuzzy(level / 2);
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( victim, &af );
  send_to_char("You begin to mirror appearance of all whom look upon you.\n\r", ch);
}

void spell_numbness( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, gsn_numbness)){
    check_dispel( -99, victim, gsn_nerve_amp);
  }
  if (is_affected(victim, sn)){
    send_to_char("Their nerves have already been numbed.\n\r", ch);
    return;
  }
  /* check saves */
  if (psiamp_failed(ch,victim,  level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    send_to_char("You failed.\n\r", ch);
    return ;
  }
  af.type	= sn;
  af.level	= level;
  af.duration	= number_fuzzy(level / 10);
  af.where	= TO_NONE;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( victim, &af );

  if (ch != victim)
    act("You subvert $N's pain receptors.", ch, NULL, victim, TO_CHAR);
  else
    send_to_char("You subvert your own pain receptors.\n\r", ch );
}
void spell_nerve_amp( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, gsn_numbness)){
    check_dispel( -99, victim, gsn_numbness);
  }
  if (is_affected(victim, sn)){
    send_to_char("Their nerves have already been stimulated.\n\r", ch);
    return;
  }
  /* check saves */
  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    send_to_char("You failed.\n\r", ch);
    return ;
  }
  af.type	= sn;
  af.level	= level;
  af.duration	= number_fuzzy(level / 10);
  af.where	= TO_NONE;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( victim, &af );

  if (ch != victim)
    act("You stimulate $N's pain receptors.", ch, NULL, victim, TO_CHAR);
  else
    send_to_char("You stimulate your own pain receptors.\n\r", ch );
}

void spell_mirage( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;


  if (is_affected(victim, sn)){
    send_to_char("You've already created a mirage of your weapons.\n\r", victim);
    return;
  }

  af.type	= sn;
  af.level	= level;
  af.duration	= number_fuzzy(level / 6);
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( victim, &af );
  send_to_char("You create a mirage of your weapons.\n\r", ch);
}
void spell_blink( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;


  if (is_affected(victim, sn)){
    send_to_char("You're already prepared to phase out.\n\r", victim);
    return;
  }

  af.type	= sn;
  af.level	= level;
  af.duration	= number_fuzzy(level / 6);
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( victim, &af );
  send_to_char("You will now phase out of existance to avoid attacks.\n\r", ch);
}

void spell_phantasm( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af, *paf;

  if (is_affected(victim, gen_phantasm)){
    send_to_char("They are already hunted by phantasms.\n\r", ch);
    return;
  }
  else if (ch == victim){
    send_to_char("You failed.\n\r", ch);
    return;
  }
  /* check saves */
  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    send_to_char("You failed.\n\r", ch);
    return ;
  }

  af.type	= gen_phantasm;
  af.level	= level;
  af.duration	= number_fuzzy(1);
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= number_range(5, 11);//how many times phantasm scares
  paf =  affect_to_char( victim, &af );
  string_to_affect( paf, ch->name );


  act("A blurred apparition briefly appears before you.", victim, NULL, NULL, TO_CHAR);
  act("A blurred apparition briefly appears before $n.", victim, NULL, NULL, TO_ROOM);
}

void spell_duplicate( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo,*clone;
  AFFECT_DATA af;
  char buf[MSL];
  int charmies = 1, duration, chance = number_percent();

  if (ch == victim){
    send_to_char("You can't cast this spell on yourself.\n\r",ch);
    return;
  }

  if (get_charmed_by(ch) >= charmies && !IS_IMMORTAL(ch)){
    send_to_char("You are already controlling as many as you can.\n\r",ch);
    return;
  }
  if (is_affected(victim,sn)){
    send_to_char("You cannot duplicate that mob yet.\n\r",ch);
    return;
  }
  if (IS_AFFECTED(victim, AFF_CHARM) 
      || victim->level - level > 5 
      || victim->fighting != NULL
      || (IS_NPC(victim) && IS_SET(victim->act,ACT_AGGRESSIVE))
      || IS_SET(victim->imm_flags,IMM_CHARM)
      || IS_UNDEAD(victim) 
      || !IS_AWAKE(victim)
      || is_affected(victim, gsn_hypnosis)){
    send_to_char("You cannot duplicate this creature.\n\r", ch);
    return;
  }
  if (saves_spell( level -3, victim,DAM_CHARM,skill_table[sn].spell_type)){
    send_to_char("You failed.\n\r",ch);
    return;
  }

  duration = level / 6;
  duration += URANGE(0, (ch->level - victim->level) / 2, 5);
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = duration;
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = 0;

  if (IS_NPC(victim)){
    clone = create_mobile(get_mob_index(MOB_VNUM_CLONE));
    clone_mobile( victim, clone );
    clone_mobile(victim,clone);
    clone->group = 0;
    clone->level = UMAX(2, clone->level);
    sprintf(buf,"illusion %s",victim->name);
    free_string(clone->name);
    clone->name		= str_dup(buf);
    char_to_room(clone,ch->in_room);
  }
  else{
    clone = create_mobile(get_mob_index(MOB_VNUM_CLONE));
    sprintf(buf,"illusion %s",victim->name);
    free_string(clone->name);
    clone->name		= str_dup(buf);
    free_string(clone->description);
    clone->description	= str_dup(victim->description);
    clone->sex		= victim->sex;
    clone->level	= level;
    clone->race		= victim->race;	
    char_to_room(clone,ch->in_room);
  }
  affect_to_char( victim, &af );
  af.bitvector = AFF_PASS_DOOR;
  affect_to_char( clone, &af );
  sprintf( buf, "An illusion of %s", IS_NPC(victim) ? victim->short_descr : victim->name);
  free_string( clone->short_descr );
  clone->short_descr = str_dup( buf );

  af.where     = TO_AFFECTS;
  af.type      = gsn_timer;
  af.level     = level;
  af.duration  = duration;
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = 0;
  affect_to_char( clone, &af );

  af.type      = gsn_charm_person;
  af.bitvector = AFF_CHARM;
  affect_to_char( clone, &af );
  SET_BIT(clone->act2,ACT_DUPLICATE);

  if ( chance < 30 ){
    sprintf( buf, "A hazy illusion of %s guards its master.\n\r",IS_NPC(victim) ? victim->short_descr : victim->name);
    free_string( clone->long_descr );
    clone->long_descr = str_dup( buf );
    clone->level /= 2;
    clone->hit /= 3;
    clone->max_hit /= 3;
    clone->hitroll /= 2;
    clone->damroll /= 2;
    clone->damage[DICE_NUMBER] /= 2;
    clone->damage[DICE_TYPE] /= 2;
    act("$n appears in a hazy smoke!",clone,NULL,NULL,TO_ROOM);
  }
  else if ( chance < 60 ){
    sprintf( buf, "A blurred illusion of %s guards its master.\n\r",IS_NPC(victim) ? victim->short_descr : victim->name);
    free_string( clone->long_descr );
    clone->long_descr = str_dup( buf );
    clone->level = 3 * clone->level/4;
    clone->max_hit = 3 * clone->hit / 4;
    clone->hit = clone->max_hit;
    clone->hitroll -= clone->damroll/4;
    clone->damroll -= clone->damroll/4;
    clone->damage[DICE_NUMBER]--;
    clone->damage[DICE_TYPE]--;
    act("$n appears in a blurred light!",clone,NULL,NULL,TO_ROOM);
  }
  else{
    sprintf( buf, "An illusion of %s guards its master.\n\r",IS_NPC(victim) ? victim->short_descr : victim->name);
    free_string( clone->long_descr );
    clone->long_descr = str_dup( buf );
    act("$n appears in a bright flash of light!",clone,NULL,NULL,TO_ROOM);
  }
  if (clone->master)
    stop_follower(clone);
  add_follower(clone,ch);
  clone->leader = ch;
  clone->gold = 0;
}

void spell_time_compression( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;


  if (is_affected(victim, sn)){
    send_to_char("You're already compressing time around yourself.\n\r", victim);
    return;
  }

  af.type	= sn;
  af.level	= level;
  af.duration	= number_fuzzy(level / 8);
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( victim, &af );
  send_to_char("You begin to compress time around yourself.\n\r", ch);
  act("$n's figure begins to leave a distinct after image.", ch, NULL, NULL, TO_ROOM);
}

void spell_molecular_leash( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af, *paf;

  if (is_affected(victim, gen_phantasm)){
    send_to_char("They are already been leashed.\n\r", ch);
    return;
  }
  else if (ch == victim){
    send_to_char("You failed.\n\r", ch);
    return;
  }
  /* check saves */
  if (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
      && saves_spell(level, victim, DAM_MENTAL,skill_table[sn].spell_type)){
    send_to_char("You failed.\n\r", ch);
    return ;
  }
  
  af.type	= sn;
  af.level	= level;
  af.duration	= 0;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= number_range(4, 9);//how many flees before leash breaks
  paf = affect_to_char( victim, &af );
  string_to_affect( paf, ch->name );

  act("A glowing blue collar appears around your neck.", victim, NULL, NULL, TO_CHAR);
  act("A glowing blue collar appears around $n's neck.", victim, NULL, NULL, TO_ROOM);
}
void spell_planar_rift( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA *blade;
  AFFECT_DATA af;
  
  //Data
  int skill = get_skill(ch, sn);
  
  int dice1 = 5;
  int dice2 = 5;

  int dam =  1;
  int hit =  3;
  
  int weight = 0;

  const int dur = 12;
  /*
    We create a rift blade with dam roll /bonues based on level and skill.
    blade is no-rem/no-drop/no-uncurse by default, and poofs on drop.
    weight is 10 to forstall removing by dropping str
  */
  
  if (IS_NPC(ch))
    return;
  
  if (is_affected(ch, sn)){
    send_to_char("You lack the strength to create a new planar blade.\n\r",ch);
    return;
  }
  
  if (get_eq_char(ch, WEAR_WIELD) != NULL
      || (get_eq_char(ch, WEAR_HOLD) != NULL && get_eq_char(ch, WEAR_SHIELD) != NULL)){
    send_to_char("Your hands must be ready to wield the blade the moment it is created.\n\r", ch);
    return;
  }
  
  blade = create_object( get_obj_index(OBJ_VNUM_PLANAR_BLADE),level);
  
  /* Set hit /dam roll*/
  dice1 += (skill > 80? 1 : 0) + (skill > 85? 1 : 0);
  dice2 += (skill > 90? 1 : 0) + (skill > 95? 1 : 0);

  dam += UMAX(0, dice1 - 5);
  hit += UMAX(0, dice2 - 5);
  if (skill > 99)
    hit += 2;
		 
  
  /* set timer */
  blade->timer = dur;
  
  /* set dice */
  blade->value[1] = dice1;
  blade->value[2] = dice2;
  
  /* set level, weight, etc */
  blade->level = UMIN(60, 10 + level);
  blade->weight = weight;
  blade->cost = 0;
  
  /* set dam / hit */
  af.type = sn;
  af.level = level;
  af.duration = -1;
  
  af.where = TO_NONE;
  af.bitvector = 0;
  
  af.location = APPLY_DAMROLL;
  af.modifier = dam;
  affect_to_obj(blade, &af);
  
  af.location = APPLY_HITROLL;
  af.modifier = hit;
  affect_to_obj(blade, &af);
  
  
  /* set effect on character */
  af.type = sn;
  af.level = level;
  af.duration = dur;
  
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  
  af.location = APPLY_NONE;
  af.modifier = 0;
  
  /* final check if we can hold the blade */
  if (IS_WEAPON_STAT(blade, WEAPON_TWO_HANDS)
      && (get_eq_char(ch, WEAR_SHIELD) != NULL
	  || get_eq_char(ch, WEAR_SECONDARY) != NULL
	  || get_eq_char(ch, WEAR_HOLD) != NULL)){
    send_to_char("You must have both hands free to recive this weapon.\n\r", ch);
    return;
  }
  
  act("A shimmering plane of force extends from your hand.",ch,blade,NULL,TO_CHAR);
  act("A shimmering plane of force extends from $n's hand.",ch,blade,NULL,TO_ROOM);
  obj_to_char(blade,ch);
  
/* check if it zapped and dropped */
  if (blade->carried_by != ch)
    extract_obj( blade );
  else
    equip_char(ch, blade, WEAR_WIELD);
  
  if (!IS_IMMORTAL(ch))
    affect_to_char(ch, &af);
}    

void spell_paralyze( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn)){
    send_to_char("They are already been paralyzed.\n\r", ch);
    return;
  }
  else if (ch == victim){
    send_to_char("You failed.\n\r", ch);
    return;
  }

  af.type      = gsn_paralyze;
  af.level     = level;
  af.duration  = number_fuzzy(1);
  af.where     = TO_AFFECTS;
  af.bitvector = 0;
  af.location  = APPLY_NONE;
  af.modifier  = number_range(10, 15);
  affect_to_char( victim, &af );

  act("$n has been paralyzed!", victim, NULL, NULL, TO_ROOM);
  act("You have been paralyzed!", victim, NULL, NULL, TO_CHAR);  
}

void spell_petrify( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int res = 0;

  if ( is_affected( victim, sn )){
    send_to_char("They've already been petrified.\n\r", ch);
    return;
  }
  if ( (res = check_immune(victim, DAM_CHARM, TRUE)) > IS_IMMUNE){
    CHAR_DATA* vch;
    for ( vch = char_list; vch != NULL; vch = vch->next ){
      if (vch->in_room && vch->in_room->area == victim->in_room->area
	  && vch->in_room != victim->in_room)
	act("$N's agonizing scream pierces the air and then cuts off abruptly.", vch, NULL, victim, TO_CHAR);
    }
    af.type	= sn;
    af.level	= level;
    af.duration	= number_fuzzy(4);
    af.where	= TO_AFFECTS2;
    af.bitvector= AFF_PETRIFY;
    af.location	= APPLY_NONE;
    af.modifier	= 0;
    affect_to_char( victim, &af );
    
    stop_fighting(victim, TRUE );
    send_to_char( "You have been PETRIFIED!!\n\r\n\r", victim );  
    act("$n has been PETRIFIED!!", victim, NULL, NULL, TO_ROOM );
  }
  else
    send_to_char("You failed.\n\r", ch);
}
