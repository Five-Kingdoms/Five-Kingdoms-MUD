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
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "interp.h"
#include "jail.h"

extern int study_range_bonus( int level );
bool is_empowered( CHAR_DATA *ch, int type )
{
  if (type == 1 && ch->class == gcn_blademaster){
    if (is_affected(ch, gsn_deathweaver))
      return TRUE;
    else
      return FALSE;
  }
  else if (ch->class != gcn_monk)
    return TRUE;
  if (is_affected(ch,gsn_empower))
    {
      send_to_char("You are still charging up!\n\r",ch);
      return FALSE;
    }
  if (type == 0 && !is_affected(ch,gsn_offensive))
    {
      send_to_char("You have not empowered your offensive abilities!\n\r",ch);
      return FALSE;
    }
  else if (type == 1 && !is_affected(ch,gsn_defensive))
    {
      send_to_char("You have not empowered your defensive abilities!\n\r",ch);
      return FALSE;
    }
  else if (type == 3 && !is_affected(ch, gsn_defensive) && !is_affected(ch, gsn_offensive)){
    send_to_char("You have not empowered!\n\r",ch);
    return FALSE;
  }
  
  return TRUE;
}

bool is_empowered_quiet( CHAR_DATA *ch, int type )
{
  if (type == 1 && ch->class == gcn_blademaster){
    if (is_affected(ch, gsn_deathweaver))
      return TRUE;
    else
      return FALSE;
  }
  else if (ch->class != gcn_monk)
    return TRUE;
  if (is_affected(ch,gsn_empower))
    return FALSE;
  else if (type == 0 && !is_affected(ch,gsn_offensive))
    return FALSE;
  else if (type == 1 && !is_affected(ch,gsn_defensive))
    return FALSE;
  else if (type == 3 && !is_affected(ch, gsn_defensive) && !is_affected(ch, gsn_offensive))
    return FALSE;
  return TRUE;
}
  
void do_beads( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    int mana, chance;
    OBJ_DATA *beads;
    AFFECT_DATA *paf;
    if ((chance = get_skill(ch,gsn_beads)) == 0)
    {
        send_to_char("You don't know how to make prayer beads.\n\r",ch);
        return;
    }
    if (argument[0] == '\0')
    {
	send_to_char("How much power do you want to use?\n\r",ch);
	return;
    }
    argument = one_argument(argument,arg);
    if (!is_number(arg))
    {
	send_to_char("Argument must be numerical.\n\r",ch);
	return;
    }
    mana = atoi(arg);
    if (ch->mana < mana)
    {
        send_to_char("You don't have that much power.\n\r",ch);
        return;
    }
    if (mana < 50 || mana > 500)
    {
        send_to_char("You must use between 50 and 500 mana.\n\r",ch);
        return;
    }
    WAIT_STATE2( ch, skill_table[gsn_beads].beats );
    if( number_percent( ) < chance)
    {
      int level = ch->level;
      beads = create_object( get_obj_index( OBJ_VNUM_PRAYER_BEADS ), 0);
      beads->level = level;
      beads->timer = 168;
      paf = new_affect();

      paf->type       = gsn_beads;
      paf->level      = ch->level;
      paf->duration   = -1;
      paf->location   = APPLY_HITROLL;
      paf->modifier   = mana/50;
      paf->bitvector  = 0;
      paf->next       = beads->affected;
      beads->affected = paf;
      paf = new_affect();
      paf->type       = gsn_beads;
      paf->level      = level;
      paf->duration   = -1;
      paf->location   = APPLY_AC;
      paf->modifier   = 0 - mana/10;
      paf->bitvector  = 0;
      paf->next       = beads->affected;
      beads->affected = paf;
      paf = new_affect();
      paf->type       = gsn_beads;
      paf->level      = level;
      paf->duration   = -1;
      paf->location   = APPLY_SAVING_SPELL;
      paf->modifier   = 0 - mana/100;
      paf->bitvector  = 0;
      paf->next       = beads->affected;
      beads->affected = paf;
      paf = new_affect();
      paf->type       = gsn_beads;
      paf->level      = level;
      paf->duration   = -1;
      paf->location   = APPLY_LUCK;
      paf->modifier   = mana/100;
      paf->bitvector  = 0;
      paf->next       = beads->affected;
      beads->affected = paf;
      obj_to_char(beads, ch); 
      act( "You create $p.", ch, beads, NULL, TO_CHAR );
      act( "$n creates $p.", ch, beads, NULL, TO_ROOM );
      check_improve(ch,gsn_beads,TRUE,1);
      ch->mana -= mana;
      
    }
    else
      {   
        send_to_char("You failed to make prayer beads.\n\r",ch);
        check_improve(ch,gsn_beads,FALSE,2);
        ch->mana -= mana/2;
      }
}

void do_monkey(CHAR_DATA *ch, char *argument)
{
    int chance;
    if ((chance = get_skill(ch,gsn_monkey)) == 0)
    {
        send_to_char("You don't have that kind of control over your body.\n\r",ch);
        return;
    }
    if (!is_empowered(ch,0))
        return;
    if (!monk_good(ch, WEAR_ABOUT) || !monk_good(ch, WEAR_WAIST) || !monk_good(ch, WEAR_LEGS) || !monk_good(ch, WEAR_FEET)
 	|| !monk_good(ch, WEAR_ARMS) || !monk_good(ch, WEAR_HANDS) || !monk_good(ch, WEAR_BODY) || !monk_good(ch, WEAR_HEAD))
    {
        send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_monkey))
    {
        send_to_char("You are already moving quickly.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_tiger) || is_affected(ch,gsn_crane) || is_affected(ch,gsn_horse)
 	|| is_affected(ch,gsn_drunken) || is_affected(ch,gsn_buddha) || is_affected(ch,gsn_dragon))
    {
	send_to_char("You are already doing another stance.\n\r",ch);
	return;
    }
    if (ch->mana < 30) 
    {
        send_to_char("You lack the clarity of mind neccessary.\n\r",ch);
        return;
    }
    WAIT_STATE2( ch, skill_table[gsn_monkey].beats );
    if (number_percent() < chance)
    {
        AFFECT_DATA af;
	af.where	= TO_AFFECTS;
        af.type   = gsn_monkey;
        af.level  = ch->level;
        af.duration = 12;
        af.modifier = 1 + (ch->level >= 40);
        af.location = APPLY_DEX;
        af.bitvector = 0;
        affect_to_char(ch,&af);
        ch->mana -= 30;
        act("You move with the speed and agility of a monkey.",ch,NULL,NULL,TO_CHAR);
        act("$n moves with the speed and agility of a monkey.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_monkey,TRUE,1);
    }
    else
    {
        ch->mana -= 15;
        act("You failed to do the stance.",ch,NULL,NULL,TO_CHAR);
        act("$n's moves oddly for a while, then returns back to normal.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_monkey,FALSE,2);
    }
}

void do_catalepsy( CHAR_DATA *ch, char *argument )
{
    int chance;
    OBJ_DATA *corpse;
    if ((chance = get_skill(ch,gsn_catalepsy)) == 0)
    {
        send_to_char("You don't have that kind of control over your body.\n\r",ch);
        return;
    }
    if (!is_empowered(ch,1))
        return;
    if (ch->mana < 25)
    {
        send_to_char("You don't have enough power.\n\r",ch);
        return;
    }
    WAIT_STATE2( ch, skill_table[gsn_catalepsy].beats );
    if( number_percent( ) < chance)
    {
	char buf[MSL];
        act( "You induce catalepsy.", ch, NULL, NULL, TO_CHAR );
        act( "$n becomes still.", ch, NULL, NULL, TO_ROOM );
        check_improve(ch,gsn_catalepsy,TRUE,1);
	ch->position = POS_SLEEPING;
	SET_BIT(ch->affected2_by, AFF_CATALEPSY);
        ch->mana -= 25;
	corpse = create_object(get_obj_index(OBJ_VNUM_FAKE_CORPSE), 0);
        sprintf( buf, corpse->short_descr, ch->name );
	free_string( corpse->short_descr );
	corpse->short_descr = str_dup( buf );
	sprintf( buf, corpse->description, ch->name );
	free_string( corpse->description );
	corpse->description = str_dup( buf );
        sprintf( buf, corpse->name, ch->name);
	free_string( corpse->name );
	corpse->name = str_dup( buf );
	corpse->timer = -1;
	obj_to_room( corpse, ch->in_room );
    }
    else
    {   
        send_to_char("You failed to induce catalepsy.\n\r",ch);
        check_improve(ch,gsn_catalepsy,FALSE,2);
        ch->mana -= 12;
    }
}

void do_revive( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    OBJ_DATA *corpse;
    if (ch->position == POS_MEDITATE)
    {
	do_sit(ch,"");
	return;
    }
    if (!IS_AFFECTED2(ch, AFF_CATALEPSY))
    {
	send_to_char("You're already alive.\n\r", ch);
	return;
    }
    send_to_char("You wake up from the dead.\n\r", ch);
    ch->position = POS_RESTING;
    REMOVE_BIT(ch->affected2_by, AFF_CATALEPSY);
    sprintf(buf, "%s%s", ch->name, "isdabomb");
    corpse = get_obj_list( ch, buf, ch->in_room->contents );
    if ( corpse != NULL )
    {
	obj_from_room( corpse );
	extract_obj( corpse );
    }
}

void do_throw_monk( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    int chance;
    bool surprise = FALSE;
    one_argument(argument,arg);
    if ( (chance = get_skill(ch,gsn_throw)) == 0)
    {
        send_to_char("Throwing?  What's that?\n\r",ch);
        return;
    }
    if (ch->fighting == NULL)
	surprise = TRUE;
    if (arg[0] == '\0')
    {
        victim = ch->fighting; 
        if (victim == NULL)
        {
            send_to_char("But you aren't fighting anyone!\n\r",ch);
            return;
        }
    }
    else if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {   
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
    if (is_safe(ch,victim))
        return;
    if (IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG) )
    {
	send_to_char("They are too big to be thrown.\n\r", ch);
	return;
    }
    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL)
    {
      sendf(ch, "You can't throw while on %s.\n\r",ch->pcdata->pStallion->short_descr);
      return;
    }
    if ( victim == ch )
        return;
    if (is_safe(ch,victim))
        return;
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
        return;
    }
    if (!IS_NPC(victim))
        chance += ch->carry_weight/100 - victim->carry_weight/50;
    else
	chance += ch->carry_weight/100 - victim->level/10;
    chance += get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_STR);
    chance -= get_skill(victim,gsn_counter)/20;
    chance += (ch->level - victim->level);   
    chance += (ch->size - victim->size) * 5;
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    if (surprise)
	chance *= 1.5;
    a_yell(ch,victim);
    if (number_percent() < chance)
    {
	int diff;
	if (!IS_NPC(victim))
	    diff = victim->size*5 + victim->carry_weight/50 + get_curr_stat(ch, STAT_STR) - 15;
	else
	    diff = victim->size*5 + victim->level/5 + get_curr_stat(ch,STAT_STR) - 15;
	if (surprise)
	    diff *= 1.5;
        if (ch->fighting != victim && victim->fighting != ch)
        {
            sprintf(buf, "Help! %s just threw me!",PERS(ch,victim));
            j_yell(victim,buf);
        }
        if (number_percent() < get_skill(victim, gsn_balance) && is_empowered_quiet(victim,1) && victim->mana > 0)
        {
            act("$n throws you but you land on your feet!",ch,NULL,victim,TO_VICT);
            act("You throw $N but $E lands on $S feet!",ch,NULL,victim,TO_CHAR);
            act("$n throws $N but $E lands on $S feet.",ch,NULL,victim,TO_NOTVICT);
            check_improve(victim,gsn_balance,TRUE,30);  
	    victim->mana -= URANGE(0,40,victim->mana);
        }
	else
	{
            act("$n throws you and you land on your back!",ch,NULL,victim,TO_VICT);
            act("You throw $N and $E lands on $S back!",ch,NULL,victim,TO_CHAR);
            act("$n throws $N and $E lands on $S back.",ch,NULL,victim,TO_NOTVICT);
	    if (!is_affected(victim, gsn_iron_will) || number_percent() < 50 + ch->level - victim->level){
	      WAIT_STATE2(victim, UMIN(2,number_range(victim->size,1)) * PULSE_VIOLENCE);
	      victim->position = POS_RESTING;
	    }
	    else{
	      send_to_char("Through your force of will you remain upright.\n\r", victim);
	      act("$n somehow remains upright.", victim, NULL, NULL, TO_ROOM);
	      victim->mana -= URANGE(0,25,victim->mana);
	    }
	}
        WAIT_STATE2(ch,skill_table[gsn_throw].beats);
        damage(ch,victim,number_range(diff, diff + chance/10 + ch->level/2),gsn_throw,DAM_BASH,TRUE);
        check_improve(ch,gsn_throw,TRUE,1);
    }
    else
    {
        if (ch->fighting != victim && victim->fighting != ch)
        {
            sprintf(buf, "Help! %s tried to throw me!",PERS(ch,victim));
            j_yell(victim,buf);
        }
        damage(ch,victim,0,gsn_throw,DAM_BASH,TRUE);
        check_improve(ch,gsn_throw,FALSE,1);
        WAIT_STATE2(ch,skill_table[gsn_throw].beats*2/3);
    }       
}

void do_sense_motion(CHAR_DATA *ch, char *argument)
{
    int chance;
    if ((chance = get_skill(ch,gsn_sense_motion)) == 0)
    {
        send_to_char("You lack the concentration.\n\r",ch);
        return;
    }
    if (!monk_good(ch, WEAR_HEAD))
    {
        send_to_char("Your heavy equipment hinders your senses.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_sense_motion))
    {
        send_to_char("You are already as aware as you can get.\n\r",ch);
        return;
    }
    if (ch->mana < 25) 
    {
        send_to_char("You don't have enough power.\n\r",ch);
        return;
    }
    WAIT_STATE2( ch, skill_table[gsn_sense_motion].beats );
    if (number_percent() < chance)
    {
        AFFECT_DATA af;
	af.where	= TO_AFFECTS;
        af.type   = gsn_sense_motion;
        af.level  = ch->level;
        af.duration = ch->level / 2;
        af.modifier = 0;
        af.location = 0;
        af.bitvector = 0;
        affect_to_char(ch,&af);
        ch->mana -= 25;
        act("You become more aware of what's around you.",ch,NULL,NULL,TO_CHAR);
        check_improve(ch,gsn_sense_motion,TRUE,1);
    }
    else
    {
        ch->mana -= 12;
        act("Your concentration is disturbed.",ch,NULL,NULL,TO_CHAR);
        check_improve(ch,gsn_sense_motion,FALSE,1);
    }
}

void do_horse(CHAR_DATA *ch, char *argument)
{
    int chance;
    if ((chance = get_skill(ch,gsn_horse)) == 0)
    {
        send_to_char("You lack the control.\n\r",ch);
        return;
    }
    if (!is_empowered(ch,1))
        return;
    if (!monk_good(ch, WEAR_HEAD))
    {
        send_to_char("Your heavy equipment hinders your control.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_horse))
    {
        send_to_char("You are already in the horse stance.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_tiger) || is_affected(ch,gsn_crane) || is_affected(ch,gsn_monkey)
 	|| is_affected(ch,gsn_drunken) || is_affected(ch,gsn_buddha) || is_affected(ch,gsn_dragon))
    {
	send_to_char("You are already doing another stance.\n\r",ch);
	return;
    }
    if (ch->mana < 30) 
    {
        send_to_char("You don't have enough power.\n\r",ch);
        return;
    }
    WAIT_STATE2( ch, skill_table[gsn_horse].beats );
    if (number_percent() < chance)
    {
        AFFECT_DATA af;
	af.where	= TO_AFFECTS;
        af.type   = gsn_horse;
        af.level  = ch->level;
        af.duration = 12;
        af.modifier = ch->level / 15;
        af.location = APPLY_CON;
        af.bitvector = 0;
        affect_to_char(ch,&af);
        ch->mana -= 30;
        act("You position yourself with the stability and endurance of a horse.",ch,NULL,NULL,TO_CHAR);
        check_improve(ch,gsn_horse,TRUE,1);
    }
    else
    {
        ch->mana -= 15;
        act("You failed to do the stance.",ch,NULL,NULL,TO_CHAR);
        check_improve(ch,gsn_horse,FALSE,1);
    }
}

void do_aura(CHAR_DATA *ch, char *argument)
{
    int chance;
    if ((chance = get_skill(ch,gsn_aura)) == 0)
    {
        send_to_char("You lack the concentration.\n\r",ch);
        return;
    }
    if (!is_empowered(ch,1))
        return;
    if (is_affected(ch,gsn_aura))
    {
        send_to_char("There is already an aura surrounding you.\n\r",ch);
        return;
    }
    if (ch->mana < 50) 
    {
        send_to_char("You don't have enough power.\n\r",ch);
        return;
    }
    WAIT_STATE2( ch, skill_table[gsn_aura].beats );
    if (number_percent() < chance)
    {
        AFFECT_DATA af;
	af.where	= TO_AFFECTS;
        af.type   = gsn_aura;
        af.level  = ch->level;
        af.duration = ch->level / 2;
        af.modifier = 0 - ch->level;
        af.location = APPLY_AC;
        af.bitvector = 0;
        affect_to_char(ch,&af);
        ch->mana -= 50;
        act_new( "An aura surrounds you in a blanket of warmth and safety.", ch, NULL, NULL, TO_CHAR, POS_DEAD );
        act( "A blindingly bright aura surrounds $n.", ch, NULL, NULL, TO_ROOM );
        check_improve(ch,gsn_aura,TRUE,1);
    }
    else
    {
        ch->mana -= 25;
        act("Your concentration is disturbed.",ch,NULL,NULL,TO_CHAR);
        check_improve(ch,gsn_aura,FALSE,1);
    }
}

void do_healing(CHAR_DATA *ch, char *argument)
{
    int chance;
    CHAR_DATA* victim = ch;
    int level = victim->level;

    if ((chance = get_skill(ch,gsn_healing)) == 0)
    {
        send_to_char("You lack the knowledge.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_healing))
    {
        send_to_char("You cannot sustain another healing session at this time.\n\r",ch);
        return;
    }
    if (ch->mana < 50) 
    {
        send_to_char("You don't have enough power.\n\r",ch);
        return;
    }
    WAIT_STATE( ch, skill_table[gsn_healing].beats );
//Embrace check.
    if (is_affected(victim, gsn_embrace_poison))
      {
	if (embrace_heal_check(gsn_heal, level, ch, ch,  0))
	  affect_strip(victim, gsn_embrace_poison);
	else
	  return;
      }
    if (number_percent() < chance && !is_affected(ch, gen_unlife))
    {
        AFFECT_DATA af;
	af.where	= TO_AFFECTS;
        af.type   = gsn_healing;
        af.level  = ch->level;
        af.duration = 24;
        af.modifier = 0;
        af.location = 0;
        af.bitvector = 0;
        affect_to_char(ch,&af);
        act("A warm feeling floods your body and you feel much better.",ch,NULL,NULL,TO_CHAR);
        act("$n starts glowing and then $e fades.",ch,NULL,NULL,TO_ROOM);
        ch->hit = UMIN(ch->max_hit, ch->hit + (ch->max_hit - ch->hit) / 3 + ch->level );
        ch->mana -= 50;
        affect_strip(ch,gsn_death_grasp);
	if (IS_AFFECTED(ch, AFF_BLIND)){
	  check_dispel(level,ch,gsn_blindness);
	}	
	if (IS_AFFECTED(ch,AFF_POISON)){
	  check_dispel(level,victim,gsn_poison);
	}
	if (IS_AFFECTED(ch, AFF_PLAGUE)){
	  check_dispel(level,victim,gsn_plague);
	}
	if (IS_AFFECTED(ch, AFF_CURSE)){
	  check_dispel(level,victim,gsn_curse);
	}
        check_improve(ch,gsn_healing,TRUE,1);
        ch->pcdata->condition[COND_HUNGER] = UMIN( ch->pcdata->condition[COND_HUNGER], 5);
        ch->pcdata->condition[COND_THIRST] = UMIN( ch->pcdata->condition[COND_THIRST], 5);
    }
    else
    {
        ch->mana -= 25;
        act("Your concentration is disturbed.",ch,NULL,NULL,TO_CHAR);
        check_improve(ch,gsn_healing,FALSE,1);
    }
}

void do_empower(CHAR_DATA *ch, char *argument)
{
    char arg[MIL];
    AFFECT_DATA af, *paf;
    int times = 100;
    if (ch->class != class_lookup("monk"))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    for ( paf = ch->affected; paf != NULL; paf = paf->next )
        if (paf->type == gsn_offensive || paf->type == gsn_defensive)
	{
            times = 26 - paf->duration;
	    break;
	}
    if ((is_affected(ch,gsn_empower) || times < 8) && !IS_IMMORTAL(ch))
    {
	send_to_char("You are not ready to empower another ability yet.\n\r",ch);
	return;
    }
    if (argument[0] == '\0')
    {
	send_to_char("What do you want to empower?\n\r",ch);
	return;
    }
    argument = one_argument(argument,arg);
    if (!str_prefix(arg, "offense"))
    {
	if (is_affected(ch,gsn_offensive))
	{
	    send_to_char("You are still under offensive empowerment.\n\r",ch);
	    return;
	}
	if (is_affected(ch,gsn_defensive))
	    affect_strip(ch,gsn_defensive);
	if (is_affected(ch,gsn_sneak))
	    affect_strip(ch,gsn_sneak);
	if (is_affected(ch,gsn_horse))
	    affect_strip(ch,gsn_horse);
	if (is_affected(ch,gsn_buddha))
	    affect_strip(ch,gsn_buddha);
	if (is_affected(ch,gsn_mantis))
	    affect_strip(ch,gsn_mantis);
	if (is_affected(ch,gsn_drunken))
	    affect_strip(ch,gsn_drunken);
	if (is_affected(ch,gsn_aura))
	    affect_strip(ch,gsn_aura);
	af.where	= TO_AFFECTS;
        af.type   = gsn_offensive;
        af.level  = ch->level;
        af.duration = 26;
        af.modifier = 0;
        af.location = 0;
        af.bitvector = 0;
        affect_to_char(ch,&af);
	act("You begin empowering your offensive skills.",ch,NULL,NULL,TO_CHAR);	
	act("$n starts emitting a red battle aura.",ch,NULL,NULL,TO_ROOM);
    }
    else if (!str_prefix(arg, "defense"))
    {
	if (is_affected(ch,gsn_defensive))
	{
	    send_to_char("You are still under defensive empowerment.\n\r",ch);
	    return;
	}
	if (is_affected(ch,gsn_offensive))
	    affect_strip(ch,gsn_offensive);
	if (is_affected(ch,gsn_monkey))
	    affect_strip(ch,gsn_monkey);
	if (is_affected(ch,gsn_dragon))
	    affect_strip(ch,gsn_dragon);
	if (is_affected(ch,gsn_tiger))
	    affect_strip(ch,gsn_tiger);
	if (is_affected(ch,gsn_crane))
	    affect_strip(ch,gsn_crane);
	if (is_affected(ch,gsn_chii_bolt))
	    affect_strip(ch,gsn_chii_bolt);
	af.where	= TO_AFFECTS;
        af.type   = gsn_defensive;
        af.level  = ch->level;
        af.duration = 26;
        af.modifier = 0;
        af.location = 0;
        af.bitvector = 0;
        affect_to_char(ch,&af);
	act("You begin empowering your defensive skills.",ch,NULL,NULL,TO_CHAR);	
	act("$n starts emitting a blue peaceful aura.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
	send_to_char("You can't empower that.\n\r",ch);
	return;
    }
    if (!IS_IMMORTAL(ch))
    {
    af.where	= TO_AFFECTS;
    af.type   = gsn_empower;
    af.level  = ch->level;
    af.duration = 1;
    af.modifier = 0;
    af.location = 0;
    af.bitvector = 0;
    affect_to_char(ch,&af);
    }
}

void do_thrash(CHAR_DATA *ch, char *argument)
{
    char arg[MIL];
    AFFECT_DATA af;
    CHAR_DATA *victim;
    int chance;   
    bool fMount = FALSE;

    argument = one_argument(argument,arg);
    if ( (chance = get_skill(ch,gsn_thrash)) == 0)
    {
        send_to_char("You don't know how to air thrash.\n\r",ch);
        return;
    }
    if (!is_empowered(ch,0))
        return;
    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char("But you aren't fighting anyone!\n\r",ch);
            return;
        }
    }
    else if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
    if (is_safe(ch,victim))
        return;
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
        return;
    }
    if (!IS_NPC(victim) && victim->pcdata->pStallion != NULL && get_skill(victim, gsn_mounted) > 2)
      fMount = TRUE;
    if ((!IS_AFFECTED(victim,AFF_FLYING) && !fMount)
	|| is_affected(victim,gsn_thrash))
      {
        act("$E isn't flying.",ch,NULL,victim,TO_CHAR);
        return;
      }

    WAIT_STATE2(ch, skill_table[gsn_thrash].beats);
    chance = 4 * chance / 5;
    chance += (get_curr_stat(ch,STAT_STR) - get_curr_stat(victim, STAT_STR)) *2;
    chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX)) *2;
    chance -= get_skill(victim,gsn_dodge)/5;
    chance += (ch->level - victim->level);
    chance += affect_by_size(ch,victim);
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);

    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "thrash", skill_table[gsn_thrash].name))
      return;

    if (number_percent() < chance )
    {
      if (fMount)
	victim->pcdata->pStallion = NULL;

      if (ch->fighting != victim && victim->fighting != ch){
	char buf[MIL];
	sprintf(buf, "Help! %s just sent me to the ground!",PERS(ch,victim));
	j_yell(victim,buf);
      }
      if (is_affected(victim,gsn_mantis) && !IS_AFFECTED(victim,AFF_FLYING) && !is_affected(victim,gsn_thrash))
	{
	  act("You reversed $n's air thrash, and thrash $m to the ground!",ch,NULL,victim,TO_VICT);
	  act("$N reverses your air thrash, and thrashes you to the ground!",ch,NULL,victim,TO_CHAR);
	  act("$N reversed $n's air thrash, and thrashes $m to the ground.",ch,NULL,victim,TO_NOTVICT);
	  if (!(is_affected(ch, gsn_iron_will)  && ch->mana > 0) || number_percent() < 50 + victim->level - ch->level){
	    WAIT_STATE2(ch,24);
            ch->position = POS_RESTING;
	  }
	  else{
	    send_to_char("Through your force of will you remain upright.\n\r", ch);
	    act("$n somehow remains upright.", ch, NULL, NULL, TO_ROOM);
	    ch->mana -= URANGE(0,25,ch->mana);
	  }
    	    WAIT_STATE2(victim, 12);
    	    damage(victim,ch,number_range(10 + victim->level/2,5 * victim->size + chance/10 + (2 * victim->level /3)),gsn_thrash,DAM_BASH,TRUE);
            affect_strip(victim,gsn_mantis);
	    return;
	}
      if (number_percent() < get_skill(victim, gsn_balance) - chance/2 && is_empowered_quiet(victim,1) && victim->mana > 0)
        {
	  act("$n jumps in the air and thrashes you to the ground, but you keep your balance!",ch,NULL,victim,TO_VICT);
	  act("You jump in the air and thrash $N to the ground, but $E keeps $S balance!",ch,NULL,victim,TO_CHAR);
	  act("$n jumps in the air and thrashes $N to the ground, and $E keeps $S balance.",ch,NULL,victim,TO_NOTVICT);
	  check_improve(victim,gsn_balance,TRUE,50); 
	  victim->mana -= URANGE(0,30,victim->mana);
        }
      else
        {
	  act("$n jumps in the air and thrashes you to the ground!",ch,NULL,victim,TO_VICT);
	  act("You jump in the air and thrash $N to the ground!",ch,NULL,victim,TO_CHAR);
	  act("$n jumps in the air and thrashes $N to the ground!",ch,NULL,victim,TO_NOTVICT);
	  if (!(is_affected(victim, gsn_iron_will)  && victim->mana > 0) || number_percent() < 50 + ch->level - victim->level){
	    WAIT_STATE2(victim, 12);
	    victim->position = POS_RESTING;
	  }
	  else{
	    send_to_char("Through your force of will you remain upright.\n\r", victim);
	    act("$n somehow remains upright.", victim, NULL, NULL, TO_ROOM);
	    victim->mana -= URANGE(0,25,victim->mana);
	  }
        }
      damage(ch,victim,number_range(10 + ch->level/2,5 * ch->size + chance/10 + (2 * ch->level /3)),gsn_thrash,DAM_BASH,TRUE);
      if (number_percent() < chance/4 + ch->level/2)
	{
	  af.where	= TO_AFFECTS;
	  af.type   = gsn_thrash;
	  af.level  = ch->level;
	  af.duration = 0;
	  af.modifier = 0;
	  af.location = 0;
	  af.bitvector = 0;
	  affect_to_char(victim,&af);
	  act("You can't seem to get back into the air",victim,NULL,NULL,TO_CHAR);
	  act("$n can't seem to get back into the air",victim,NULL,NULL,TO_ROOM);
	}
      check_improve(ch,gsn_thrash,TRUE,1);
    }
    else
    {
      if (ch->fighting != victim && victim->fighting != ch){
	char buf[MIL];
	sprintf(buf, "Help! %s just tried to send me to the ground!",PERS(ch,victim));
	j_yell(victim,buf);
      }
      
      act("$N manages to evade your air thrash.", ch,NULL,victim,TO_CHAR);
      act("$N manages to evade $n's air thrash.", ch,NULL,victim,TO_NOTVICT);
      act("You manage to evade $n's air thrash.",ch,NULL,victim,TO_VICT);
      damage(ch,victim,0,gsn_thrash,DAM_BASH,TRUE);
      check_improve(ch,gsn_thrash,FALSE,1);
    }
}

void do_chii_bolt(CHAR_DATA *ch, char *argument)
{
    char arg[MIL];
    char buf[MIL];
    AFFECT_DATA *paf;
    CHAR_DATA *victim;
    int chance, level = 0;
    argument = one_argument(argument,arg);
    if ( (chance = get_skill(ch,gsn_chii_bolt)) == 0)
    {
        send_to_char("You don't know how to chii bolt.\n\r",ch);
        return;
    }
    if (!is_empowered(ch,0))
        return;
    if (!is_affected(ch,gsn_chii_bolt))
    {
	send_to_char("You have no chii stored up.\n\r",ch);
	return;
    }
    for ( paf = ch->affected; paf != NULL; paf = paf->next )
        if (paf->type == gsn_chii_bolt)
	{
            level = paf->modifier;
	    break;
	}
    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char("But you aren't fighting anyone!\n\r",ch);
            return;
        }
    }
    else if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
    if (is_safe(ch,victim))
        return;
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
        return;
    }
    if (number_percent() > chance)
    {
	send_to_char("You failed to control your chii and it diffuses from your body.\n\r",ch);
    	damage(ch,victim,0,gsn_chii_bolt,DAM_BASH,TRUE);
    	check_improve(ch,gsn_chii_bolt,FALSE,1);
	return;
    }
    WAIT_STATE2(ch, PULSE_VIOLENCE);
    act("You release your stored chii at $N!",ch,NULL,victim,TO_CHAR);
    act("$n releases $s stored chii at $N!",ch,NULL,victim,TO_NOTVICT);
    act("$n release $s stored chii at you!",ch,NULL,victim,TO_VICT);

    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "bolt", skill_table[gsn_chii_bolt].name))
      return;

    if (ch->fighting != victim && victim->fighting != ch){
    
      sprintf(buf, "Help! %s released chii into me!",PERS(ch,victim));
      j_yell(victim,buf);
    }
    switch(level)
    {
	case (1) :
	    act("Your chii bolt knocks the wind out of $N.",ch,NULL,victim,TO_CHAR);
	    act("$n's chii bolt knocks the wind out of $N.",ch,NULL,victim,TO_NOTVICT);
	    act("$n's chii bolt knocks the wind out of you.",ch,NULL,victim,TO_VICT);
	    WAIT_STATE2(victim, 2 * PULSE_VIOLENCE);
	    damage(ch,victim,number_range(1,ch->level/4),gsn_chii_bolt,DAM_BASH,TRUE);
	    break;
	case (2) :
	    act("Your chii bolt knocks $N staggering backwards!",ch,NULL,victim,TO_CHAR);
	    act("$n's chii bolt knocks $N staggering backwards!",ch,NULL,victim,TO_NOTVICT);
	    act("$n's chii bolt knocks you staggering backwards!",ch,NULL,victim,TO_VICT);
	    WAIT_STATE2(victim, 3 * PULSE_VIOLENCE);
	    damage(ch,victim,number_range(ch->level/2,ch->level),gsn_chii_bolt,DAM_BASH,TRUE);
	    break;
	case (3) :
	  if (is_affected(victim, skill_lookup("armor")))
	    {
	      act("Your chii bolt collides into $N's armor and neutralizes it.",ch,NULL,victim,TO_CHAR);
	      act("$n's chii bolt collides into $N's armor and neutralizes it.",ch,NULL,victim,TO_NOTVICT);
	      act("$n's chii bolt collides into your armor and neutralizes it.",ch,NULL,victim,TO_VICT);
	      affect_strip(victim, skill_lookup("armor"));
	    }
	  if (is_affected(victim, skill_lookup("shield")))
	    {
	      act("Your chii bolt collides into $N's shield and neutralizes it.",ch,NULL,victim,TO_CHAR);
	      act("$n's chii bolt collides into $N's shield and neutralizes it.",ch,NULL,victim,TO_NOTVICT);
	      act("$n's chii bolt collides into your shield and neutralizes it.",ch,NULL,victim,TO_VICT);
	      affect_strip(victim, skill_lookup("shield"));
	    }
	  if (is_affected(victim, skill_lookup("protective shield")))
	    {
	      act("Your chii bolt collides into $N's protective shield and neutralizes it.",ch,NULL,victim,TO_CHAR);
	      act("$n's chii bolt collides into $N's protective shield and neutralizes it.",ch,NULL,victim,TO_NOTVICT);
	      act("$n's chii bolt collides into your protective shield and neutralizes it.",ch,NULL,victim,TO_VICT);
	      affect_strip(victim, gsn_protective_shield);
	    }
	  if (is_affected(victim, skill_lookup("barrier")))
	    {
	      act("Your chii bolt collides into $N's barrier and neutralizes it.",ch,NULL,victim,TO_CHAR);
	      act("$n's chii bolt collides into $N's barrier and neutralizes it.",ch,NULL,victim,TO_NOTVICT);
	      act("$n's chii bolt collides into your barrier and neutralizes it.",ch,NULL,victim,TO_VICT);
	      affect_strip(victim, skill_lookup("barrier"));
	    }
	  if (is_affected(victim, skill_lookup("force field"))){
	      act("Your chii bolt collides into $N's force field and collapses it.",ch,NULL,victim,TO_CHAR);
	      act("$n's chii bolt collides into $N's force field and collapses it.",ch,NULL,victim,TO_NOTVICT);
	      act("$n's chii bolt collides into your force field and collapses it.",ch,NULL,victim,TO_VICT);
	      affect_strip(victim, skill_lookup("force field"));
	  }
	  if (is_affected(victim, skill_lookup("reflective shield"))){
	      act("Your chii bolt collides into $N's reflective field and collapses it.",ch,NULL,victim,TO_CHAR);
	      act("$n's chii bolt collides into $N's reflective field and collapses it.",ch,NULL,victim,TO_NOTVICT);
	      act("$n's chii bolt collides into your reflective field and collapses it.",ch,NULL,victim,TO_VICT);
	      affect_strip(victim, skill_lookup("reflective shield"));
	  }
	  if (is_affected(victim, skill_lookup("spell turning"))){
	      act("Your chii bolt collides into $N's spell shield and cancells it.",ch,NULL,victim,TO_CHAR);
	      act("$n's chii bolt collides into $N's spell shield and cancells it.",ch,NULL,victim,TO_NOTVICT);
	      act("$n's chii bolt collides into your spell shield and cancells it.",ch,NULL,victim,TO_VICT);
	      affect_strip(victim, skill_lookup("spell turning"));
	  }
	  WAIT_STATE2(victim, number_range(1,3) * PULSE_VIOLENCE);
	  damage(ch,victim,number_range(1,ch->level/2),gsn_chii_bolt,DAM_BASH,TRUE);
	    break;
	default:
	    send_to_char("Nothing happens...\n\r",ch);
	    break;
    }
    check_improve(ch,gsn_chii_bolt,TRUE,1);
    affect_strip(ch,gsn_chii_bolt);    

}

void do_store_chii(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af, *paf;
    int chance, level = 0;
    if ( (chance = get_skill(ch,gsn_chii_bolt)) == 0)
    {
        send_to_char("You don't know how to store chii.\n\r",ch);
        return;
    }
    if (!is_empowered(ch,0))
        return;
    if (!is_affected(ch,gsn_chii))
    {
	send_to_char("You are not circulating your chii.\n\r",ch);
	return;
    }
    for ( paf = ch->affected; paf != NULL; paf = paf->next )
        if (paf->type == gsn_chii_bolt)
	{
            level = paf->modifier;
	    break;
	}
    if (level >= 3)
    {
	send_to_char("You are not able to store anymore chii.\n\r",ch);
	return;
    }
    if (paf != NULL && paf->duration < 2)
    {
	send_to_char("There is not enough duration in your chii.\n\r",ch);
	return;
    }
    affect_strip(ch,gsn_chii);
    if (number_percent() > chance)
    {
	send_to_char("You failed to store your chii.\n\r",ch);
    	check_improve(ch,gsn_chii_bolt,FALSE,1);
	return;
    }
    if (level > 0)
    {
	level++;
	sendf(ch,"You have increased your chii bolt to level %d.\n\r",level);
	act("$n increases $s chii bolt's level.",ch,NULL,NULL,TO_ROOM);
        paf->modifier++;
    }
    else
    {
	act("You create a chii bolt from your chii.",ch,NULL,NULL,TO_CHAR);
	act("$n creates a chii bolt from $s chii.",ch,NULL,NULL,TO_ROOM);
	af.where	= TO_AFFECTS;
    	af.type   = gsn_chii_bolt;
    	af.level  = ch->level;
    	af.duration = ch->level / 4;
    	af.modifier = 1;
    	af.location = 0;
    	af.bitvector = 0;
    	affect_to_char(ch,&af);
    }
    if (level == 2)
	send_to_char("Your chii forms a magical aura about your hands.\n\r",ch);
    check_improve(ch,gsn_chii_bolt,TRUE,1);
    WAIT_STATE2(ch,12);
}

void do_mantis(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int chance;
    if ( (chance = get_skill(ch,gsn_mantis)) == 0)
    {
        send_to_char("You don't know how to do a reversal.\n\r",ch);
        return;
    }
    if (!is_empowered(ch,1))
        return;
    if (is_affected(ch,gsn_mantis))
    {
	send_to_char("You are already waiting for a victim to make its move.\n\r",ch);
	return;
    }
    if (ch->mana < 15)
    {
        send_to_char("You lack the clarity of mind neccessary.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_mantis].beats);
    if (number_percent() > chance)
    {
    	ch->mana -= 7;
	send_to_char("You were not able to prepare for a counter move.\n\r",ch);
        act("$n's moves oddly for a while, then returns back to normal.",ch,NULL,NULL,TO_ROOM);
    	check_improve(ch,gsn_mantis,FALSE,2);
	return;
    }
    ch->mana -= 15;
    act("You prepare for a counter move like a preying mantis waits for its prey.",ch,NULL,NULL,TO_CHAR);
    act("$n prepares to counter a move.",ch,NULL,NULL,TO_ROOM);
    af.where	= TO_AFFECTS;
    af.type   = gsn_mantis;
    af.level  = ch->level;
    af.duration = ch->level/3;
    af.modifier = 0;
    af.location = 0;
    af.bitvector = 0;
    affect_to_char(ch,&af);
    check_improve(ch,gsn_mantis,TRUE,1);
    WAIT_STATE2(ch,12);
}

void entomb( CHAR_DATA* ch, CHAR_DATA* victim ){
  AFFECT_DATA af;

  if (is_affected(victim,gsn_entomb))
    return;

  if (!IS_NPC(victim) || !IS_UNDEAD(victim) || victim->master != ch)
    return;

  if (ch->mana < 50)
    return;

  ch->mana -= 50;

  act("You order $N to enter $S tomb.",ch,NULL,victim,TO_CHAR);
  act("$n orders $N to enter $S tomb.",ch,NULL,victim,TO_ROOM);
  do_sleep(victim,"");

  af.where		= TO_AFFECTS;
  af.type             = gsn_entomb;
  af.level            = ch->level;
  af.duration         = -1;
  af.location         = 0;
  af.modifier         = 0;
  af.bitvector        = AFF_SLEEP;
  affect_to_char(victim,&af);
}

void do_entomb(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim = NULL;
    int chance;
    bool fAll = FALSE;

    if ( (chance = get_skill(ch,gsn_entomb)) == 0)
    {
        send_to_char("You don't know how to entomb.\n\r",ch);
        return;
    }
    if (!str_cmp(argument, "all")){
      fAll = TRUE;
    }
    else{
      if ((victim = get_char_room(ch, NULL, argument)) == NULL){
	send_to_char("They aren't here.\n\r",ch);
	return;
      }
      if (is_affected(victim,gsn_entomb)){
	send_to_char("They are already entombed.\n\r",ch);
	return;
      }
      if (!IS_NPC(victim) || !IS_UNDEAD(victim) || victim->master != ch){
	send_to_char("You cannot entomb that.\n\r",ch);
	return;
      }
    }

    if (ch->mana < 50){
      send_to_char("You don't have enough energy to entomb your minions.\n\r",ch);
      return;
    }
    ch-> mana -= 50;
    WAIT_STATE2(ch,skill_table[gsn_entomb].beats );

    if (chance < number_percent())
    {
	send_to_char("You failed to entomb your minions.\n\r",ch);
	act("$n failed to entomb $s minions.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_entomb,FALSE,1);	
	return; 
    }

    if (!fAll && victim)
      entomb(ch, victim);
    else{
      CHAR_DATA* vch, *vch_next;

      for (vch = ch->in_room->people; vch; vch = vch_next){
	vch_next = vch->next_in_room;

	entomb(ch, vch);
      }
    }
}

void do_untomb(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MIL];
    if ( get_skill(ch,gsn_entomb) == 0)
    {
        send_to_char("You don't know how to untomb.\n\r",ch);
        return;
    }
    if (!str_cmp(argument, "all")){
      CHAR_DATA* vch, *vch_next;
      for (vch = ch->in_room->people; vch; vch = vch_next){
	vch_next = vch->next_in_room;
	if (vch->master == ch && is_affected(vch, gsn_entomb)){
	  affect_strip(vch,gsn_entomb);
	  act("$n rises from $s tomb.",vch,NULL,NULL,TO_ROOM);
	  do_stand(vch,"");
	  WAIT_STATE2(ch,skill_table[gsn_entomb].beats );
	}
      }
    }
    else{
      if ((victim = get_char_room(ch, NULL, arg)) == NULL)
	{
	  send_to_char("They aren't here.\n\r",ch);
	  return;
	}
      if (!IS_NPC(victim) || victim->master != ch)
	{
	  send_to_char("You cannot untomb that.\n\r",ch);
	  return;
	}
      if (!is_affected(victim,gsn_entomb))
	{
	  send_to_char("They are not entombed.\n\r",ch);
	  return;
	}
      WAIT_STATE2(ch,skill_table[gsn_entomb].beats );
      affect_strip(victim,gsn_entomb);
      act("$n rises from $s tomb.",victim,NULL,NULL,TO_ROOM);
      do_stand(victim,"");
    }
}

void do_tiger(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int chance;
    if ((chance = get_skill(ch,gsn_tiger)) == 0)
    {
        send_to_char("You don't don't know to do that stance",ch);
        return;
    }
    if (!is_empowered(ch,0))
        return;
    if( !monk_good(ch, WEAR_LEGS) || !monk_good(ch, WEAR_WIELD) || !monk_good(ch, WEAR_ARMS) || !monk_good(ch, WEAR_BODY))
    {
        send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_tiger))
    {
        send_to_char("You are already moving like a tiger.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_monkey) || is_affected(ch,gsn_crane) || is_affected(ch,gsn_horse)
 	|| is_affected(ch,gsn_drunken) || is_affected(ch,gsn_buddha) || is_affected(ch,gsn_dragon))
    {
	send_to_char("You are already doing another stance.\n\r",ch);
	return;
    }
    if (ch->mana < 30)
    {
        send_to_char("You lack the clarity of mind neccessary.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_tiger].beats);
    if (number_percent() > chance)
    {
    	ch->mana -= 15;
	send_to_char("You failed to do the stance.\n\r",ch);
        act("$n's moves oddly for a while, then returns back to normal.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_tiger,FALSE,2);
	return;
    }
    ch->mana -= 30;
    af.where	= TO_AFFECTS;
    af.type   = gsn_tiger;
    af.level  = ch->level;
    af.duration = 12;
    af.modifier = ch->level;
    af.location = APPLY_AC;
    af.bitvector = 0;
    affect_to_char(ch,&af);    
    af.modifier = ch->level/3;
    af.location = APPLY_DAMROLL;
    affect_to_char(ch,&af);    
    af.modifier = - ch->level/10;
    af.location = APPLY_HITROLL;
    affect_to_char(ch,&af);    
    af.modifier = ch->level/12;
    af.location = APPLY_STR;
    affect_to_char(ch,&af);
    act("You roar with the strength of the tiger!",ch,NULL,NULL,TO_CHAR);    
    act("$n roars with the strength of the tiger!",ch,NULL,NULL,TO_ROOM);    
    check_improve(ch,gsn_tiger,TRUE,1);
}

void do_crane(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int chance;
    if ((chance = get_skill(ch,gsn_crane)) == 0)
    {
        send_to_char("You don't know how to do that stance",ch);
        return;
    }
    if (!is_empowered(ch,0))
        return;
    if (!monk_good(ch, WEAR_ARMS) || !monk_good(ch, WEAR_LEGS) || !monk_good(ch, WEAR_HANDS) || !monk_good(ch, WEAR_FEET))
    {
        send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_crane))
    {
        send_to_char("You are already moving like a crane.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_tiger) || is_affected(ch,gsn_monkey) || is_affected(ch,gsn_horse)
 	|| is_affected(ch,gsn_drunken) || is_affected(ch,gsn_buddha) || is_affected(ch,gsn_dragon))
    {
	send_to_char("You are already doing another stance.\n\r",ch);
	return;
    }
    if (ch->mana < 30)
    {
        send_to_char("You lack the clarity of mind neccessary.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_crane].beats);
    if (number_percent() > chance)
    {
    	ch->mana -= 15;
	send_to_char("You failed to do the stance.\n\r",ch);
        act("$n's moves oddly for a while, then returns back to normal.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_crane,FALSE,2);
	return;
    }
    ch->mana -= 30;
    af.where	= TO_AFFECTS;
    af.type   = gsn_crane;
    af.level  = ch->level;
    af.duration = 12;
    af.modifier = 0 - ch->level/10;
    af.location = APPLY_DAMROLL;
    af.bitvector = 0;
    affect_to_char(ch,&af);  
    af.modifier = 5 + ch->level / 5;
    af.location = APPLY_HITROLL;
    affect_to_char(ch,&af);  
    act("You move with the swiftness and the accuracy of the crane.",ch,NULL,NULL,TO_CHAR);  
    act("$n moves with the swiftness and the accuracy of the crane.",ch,NULL,NULL,TO_ROOM);  
    check_improve(ch,gsn_crane,TRUE,1);
}

void do_drunken(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int chance;
    if ((chance = get_skill(ch,gsn_drunken)) == 0)
    {
        send_to_char("You don't know how to do that stance",ch);
        return;
    }
    if (!is_empowered(ch,1))
        return;
    if (!monk_good(ch, WEAR_ABOUT) || !monk_good(ch, WEAR_WAIST) || !monk_good(ch, WEAR_LEGS) || !monk_good(ch, WEAR_FEET)
 	|| !monk_good(ch, WEAR_ARMS) || !monk_good(ch, WEAR_HANDS) || !monk_good(ch, WEAR_BODY) || !monk_good(ch, WEAR_HEAD))

    {
        send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_drunken))
    {
        send_to_char("You are already moving like a drunk.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_tiger) || is_affected(ch,gsn_crane) || is_affected(ch,gsn_horse)
 	|| is_affected(ch,gsn_monkey) || is_affected(ch,gsn_buddha) || is_affected(ch,gsn_dragon))
    {
	send_to_char("You are already doing another stance.\n\r",ch);
	return;
    }
    if (ch->mana < 30)
    {
        send_to_char("You lack the clarity of mind neccessary.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_drunken].beats);
    if (number_percent() > chance)
    {
    	ch->mana -= 15;
	send_to_char("You failed to do the stance.\n\r",ch);
        act("$n's moves oddly for a while, then returns back to normal.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_drunken,FALSE,2);
	return;
    }
    ch->mana -= 30;
    af.where	= TO_AFFECTS;
    af.type   = gsn_drunken;
    af.level  = ch->level;
    af.duration = 12;
    af.modifier = 0 - ch->level/2;
    af.location = APPLY_AC;
    af.bitvector = 0;
    affect_to_char(ch,&af);
    act("You start staggering around like a drunk.",ch,NULL,NULL,TO_CHAR);
    act("$n starts staggering around like a drunk.",ch,NULL,NULL,TO_ROOM);
    check_improve(ch,gsn_drunken,TRUE,1);
}

void do_dragon(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int chance;
    if ((chance = get_skill(ch,gsn_dragon)) == 0)
    {
        send_to_char("You don't know how to do that stance",ch);
        return;
    }
    if (!is_empowered(ch,0))
        return;
    if (!monk_good(ch, WEAR_ABOUT) || !monk_good(ch, WEAR_WAIST) || !monk_good(ch, WEAR_LEGS) || !monk_good(ch, WEAR_FEET)
 	|| !monk_good(ch, WEAR_ARMS) || !monk_good(ch, WEAR_HANDS) || !monk_good(ch, WEAR_BODY) || !monk_good(ch, WEAR_HEAD))

    {
        send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_dragon))
    {
        send_to_char("You already possess the spirit of the dragon.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_tiger) || is_affected(ch,gsn_crane) || is_affected(ch,gsn_horse)
 	|| is_affected(ch,gsn_drunken) || is_affected(ch,gsn_monkey) || is_affected(ch,gsn_buddha))
    {
	send_to_char("You are already doing another stance.\n\r",ch);
	return;
    }
    if (ch->mana < 30)
    {
        send_to_char("You lack the clarity of mind neccessary.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_dragon].beats);
    if (number_percent() > chance)
    {
    	ch->mana -= 15;
	send_to_char("You failed to do the stance.\n\r",ch);
        act("$n's moves oddly for a while, then returns back to normal.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_dragon,FALSE,2);
	return;
    }
    ch->mana -= 30;
    af.where	= TO_AFFECTS;
    af.type   = gsn_dragon;
    af.level  = ch->level;
    af.duration = 12;
    af.modifier = ch->level * 2;
    af.location = APPLY_HIT;
    af.bitvector = 0;
    affect_to_char(ch,&af);
    //apply hp
    ch->hit += ch->level * 2;
    af.modifier = 1;
    af.location = APPLY_SIZE;
    affect_to_char(ch,&af);
    act("Your body enlarges as you possess the spirit of the dragon.",ch,NULL,NULL,TO_CHAR);
    act("$n's body starts to enlarge into the image of a dragon.",ch,NULL,NULL,TO_ROOM);
    check_improve(ch,gsn_dragon,TRUE,1);
}

void do_buddha(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int chance;
    if ((chance = get_skill(ch,gsn_buddha)) == 0)
    {
        send_to_char("You don't know how to do that stance",ch);
        return;
    }
    if (!is_empowered(ch,1))
        return;
    if (!monk_good(ch, WEAR_HEAD))
    {
        send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_buddha))
    {
      send_to_char("You already possess the peace and tranquility of a buddha.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_tiger) || is_affected(ch,gsn_crane) || is_affected(ch,gsn_horse)
 	|| is_affected(ch,gsn_drunken) || is_affected(ch,gsn_monkey) || is_affected(ch,gsn_dragon))
    {
	send_to_char("You are already doing another stance.\n\r",ch);
	return;
    }
    if (ch->mana < 30)
    {
        send_to_char("You lack the clarity of mind neccessary.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_buddha].beats);
    if (number_percent() > chance)
    {
    	ch->mana -= 15;
	send_to_char("You failed to do the stance.\n\r",ch);
        act("$n's moves oddly for a while, then returns back to normal.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_buddha,FALSE,2);
	return;
    }
    ch->mana -= 30;
    af.where	= TO_RESIST;
    af.type   = gsn_buddha;
    af.level  = ch->level;
    af.duration = 12;
    af.modifier = 0 - ch->level/2;
    af.location = APPLY_SAVING_SPELL;
    af.bitvector = RES_MAGIC;
    affect_to_char(ch,&af);
    act("You are in full control of your mind.",ch,NULL,NULL,TO_CHAR);
    act("$n takes full control of $s mind.",ch,NULL,NULL,TO_ROOM);
    check_improve(ch,gsn_buddha,TRUE,1);
}

void do_double_grip(CHAR_DATA *ch, char *argument)
{
    int chance;
    OBJ_DATA *wield = NULL;
    AFFECT_DATA af;
    bool towear = FALSE;
    char arg[MIL];
    one_argument(argument,arg);
    if ((chance = get_skill(ch,gsn_double_grip)) == 0)
    {
        send_to_char("You are not skilled enough to double grip",ch);
        return;
    }
    if ( arg[0] != '\0' )
    {
	if ((wield = get_obj_carry( ch, arg, ch )) == NULL)
	{
	    send_to_char("You can't find it.\n\r",ch);
	    return;
	}
    	if (wield->item_type != ITEM_WEAPON)
    	{
	    send_to_char("You can't use that as a weapon!\n\r",ch);
	    return;
    	}
	towear = TRUE;
    }
    if (is_affected(ch,gsn_double_grip))
    {
        send_to_char("You already have both your hands tightly around your weapon.\n\r",ch);
        return;
    }
    if (get_eq_char(ch,WEAR_SHIELD) != NULL || get_eq_char(ch,WEAR_HOLD) != NULL
	|| get_eq_char(ch,WEAR_SECONDARY) != NULL)
    {
        send_to_char("Your hands are tied up!\n\r",ch);
        return;
    }
    af.where	= TO_AFFECTS;
    af.type   = gsn_double_grip;
    af.level  = ch->level;
    af.duration = 12;
    af.modifier = 0;
    af.location = 0;
    af.bitvector = 0;
    affect_to_char(ch,&af);
    if (towear && wield != NULL)
	wear_obj(ch,wield,TRUE,TRUE);
    wield = get_eq_char(ch,WEAR_WIELD);
    if ( wield == NULL )
    {
        send_to_char( "You can't do that.\n\r", ch );
	affect_strip(ch,gsn_double_grip);
        return;
    }
    if (!IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS) && wield->value[0] != WEAPON_SWORD && wield->value[0] != WEAPON_STAFF
	&& wield->value[0] != WEAPON_POLEARM && wield->value[0] != WEAPON_AXE && wield->value[0] != WEAPON_SPEAR && wield->value[0] != WEAPON_MACE)
    {
	send_to_char("You cannot double grip that weapon.\n\r",ch);
	if (towear)
	    unequip_char(ch,wield);
	affect_strip(ch,gsn_double_grip);
	return;
    }
    act("You tighten your grip around $p.",ch,wield,NULL,TO_CHAR);
    act("$n tightens $s grip around $p.",ch,wield,NULL,TO_ROOM);
}

void do_cone(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *vch, *vch_next;
    AFFECT_DATA af, *paf;
    char buf[MSL];
    int skill = get_skill( ch, gsn_cone );
    const int cost = skill_table[gsn_cone].min_mana;

    if (skill < 2){
      send_to_char("Huh?\n\r", ch);
      return;
    }
    else
      skill = 4 * skill / 5;

    if (ch->level < 15){
      send_to_char("Your mind is not powerfull enough yet.\n\r", ch);
      return;
    }
    if ( (paf = affect_find(ch->affected,gsn_cone)) != NULL && paf->modifier >= (ch->level / 10) - 1)
      {
	send_to_char("You are not ready to use this ability again.\n\r",ch);
	return;
      }
    if (ch->mana < cost)
      {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
      }
    ch->mana -= cost;
    WAIT_STATE2(ch, skill_table[gsn_cone].beats);
    if (number_percent() < skill){
      send_to_char("You failed.\n\r", ch);
      return;
    }

    if (paf)
      paf->modifier ++;
    else{
      af.where	= TO_AFFECTS;
      af.type   = gsn_cone;
      af.level  = ch->level;
      af.duration = 24;
      af.location = APPLY_NONE;
      af.modifier = 1;
      af.bitvector = 0;
      affect_to_char(ch,&af);
    }
    act("You send a surge of mental energy through the room.",ch,NULL,NULL,TO_CHAR);
    act("A wave of mental energy surges through the room.",ch,NULL,NULL,TO_ROOM);
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;
	if ( vch != ch && !is_area_safe(ch, vch) )
	{
            if ( ch->fighting != vch && vch->fighting != ch)
            {
	      a_yell(ch, vch);
	      //predict interdict
	      if (predictCheck(ch, vch, "coneofforce", skill_table[gsn_cone].name))
		continue;
               
                sprintf(buf, "Help! %s is blasting my brains!",PERS(ch,vch));
                j_yell(vch,buf);
		damage(ch,vch,number_range(ch->level,ch->level*2),gsn_cone,DAM_MENTAL,TRUE);
		if (!saves_spell(ch->level + 5,vch,DAM_MENTAL,SPELL_MENTAL)){
		  act("You are paralized by $n's cone of mental force!", ch, NULL, vch, TO_VICT);
		  act("$N is paralized by your cone of mental force!", ch, NULL, vch, TO_CHAR);
		  act("$N is paralized by $n's cone of mental force!", ch, NULL, vch, TO_NOTVICT);
		  WAIT_STATE2(vch,36);
		}
		else{
		  act("$N is resist your cone of mental force!", ch, NULL, vch, TO_CHAR);
		  WAIT_STATE2(vch,12);
		}
	    }
	}
    }
}		    

void do_unburrow(CHAR_DATA *ch, char *argument)
{
  //EFFECT data for unburrow.
  AFFECT_DATA* af;

    if (IS_NPC(ch))
	return;
    if (!is_affected(ch,gsn_burrow))
    {
	send_to_char("You are not burrowed underground!\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,12);
    //quick check as we get the effect.
    if ( (af = affect_find(ch->affected, gsn_burrow)) == NULL)
      return;

//orgin room vnum is store in modifier.
    char_from_room(ch);
    char_to_room(ch, get_room_index(af->modifier));

    send_to_char("You seep backup to the surface.\n\r",ch);
    act("A cloud of mist begins to seep out of underground suddenly.", ch, NULL, NULL, TO_ROOM);

//and we strip
    affect_strip(ch,gsn_burrow);
    REMOVE_BIT(ch->comm,COMM_QUIET);
//and we revert so they dont keep mist form.
    affect_strip(ch, gsn_mist_form);
    free_string( ch->short_descr );
    ch->short_descr = str_dup ( "" );
    ch->dam_type = 17;
}

void do_burrow(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    if (ch->in_room == NULL)
	return;
    if (IS_NPC(ch))
    {
	send_to_char("Mobs can't burrow!\n\r",ch);
	return;
    }

    if (ch->class != class_lookup("vampire"))
    {
	send_to_char("You dig in the ground with a stick a little and feel silly.\n\r",ch);
        act("$n digs in a ground with a small stick.  Doesn't $e just look silly?", ch, NULL, NULL, TO_ROOM);
	return;
    }

    if (is_affected(ch, gsn_covenant))
      {
	send_to_char("You have not been blessed by Covenant to sleep!\n\r", ch);
	  return;
      }

    //vamps can only do this atday
    if (mud_data.time_info.hour < 6 || mud_data.time_info.hour > 18)
    {
	send_to_char("Vampires do not rest at night!\n\r",ch);
	return;
    }

    if( (is_affected(ch, gsn_bat_form)) || (is_affected(ch, gsn_wolf_form)) )
   {
     send_to_char("Not in your current form!\n\r", ch);
     return;
   }

    if (ch->pcdata->pStallion != NULL)
    {
      sendf(ch, "You can't burrow while on %s.\n\r",ch->pcdata->pStallion->short_descr);
      return;
    }

    if ( IS_AFFECTED(ch, AFF_FAERIE_FOG) || IS_AFFECTED(ch, AFF_FAERIE_FIRE))
    {
        send_to_char( "The glowing aura seem to interfere with the process.\n\r", ch);
        return;
    }
    if (ch->in_room->sector_type == SECT_AIR)
    {
        send_to_char("You can't burrow in the air!\n\r",ch);
        return;
    }
    if (ch->in_room->sector_type == SECT_WATER_SWIM || ch->in_room->sector_type == SECT_WATER_NOSWIM)
    {
        send_to_char("You can't burrow in the water!\n\r",ch);
        return;
    }
    if (ch->in_room->sector_type == SECT_CITY
	|| IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL))
    {
        send_to_char("The ground is too hard for even your mist form to seep into.\n\r",ch);
        return;
    }
    /* Only if not attacked recently */
    if (is_fight_delay(ch, 30)){
      act("With the air of violence still about you, your concentration fails.", ch, NULL, NULL, TO_CHAR);
      return;
    }
    if (is_affected(ch,gsn_burrow))
    {
	send_to_char("You have already burrowed underground!\n\r",ch);
	return;
    }
    if (ch->mana < skill_table[gsn_burrow].min_mana)
    {
        send_to_char("You don't have enough mana.\n\r",ch);
        return;
    }

    affect_strip(ch, gen_conceal);
    WAIT_STATE2(ch, skill_table[gsn_burrow].beats);
    ch->mana -= skill_table[gsn_burrow].min_mana;
    if (number_percent() > ch->level*2)
    {
if (is_affected(ch, gsn_mist_form))
{
	send_to_char("You can't seem to find a suitable place underground.\n\r",ch);
        act("$n swirls and billows near the ground.", ch, NULL, NULL, TO_ROOM);
        return;
}//end if mist form
else
{
	send_to_char("You take on the mist form but can't seem to find a suitable place underground.\n\r",ch);
        act("$n form grows transparent as $e turns into a cloud of mist.  Moments later $e takes human form once again.", ch, NULL, NULL, TO_ROOM);
	return;
}//end regular.
}//end failed burrowed.


if (is_affected(ch, gsn_mist_form))
{
    send_to_char("You seep into an underground pocket..\n\r",ch);
    act("$n hovers near the ground billowing in the breeze and then it's gone.",ch,NULL,NULL,TO_ROOM);
}//end if mist
else
{
    send_to_char("You take on mist form and seep into an underground pocket..\n\r",ch);
    act("$n evaporates into a fine cloud of mist.",ch,NULL,NULL,TO_ROOM);
    //we turn into a mist
    //this is hardcoded as we dont want the vamp faling mist.  if he passed the check for burrow then that is good enough.
    	af.where     = TO_AFFECTS;
    	af.type      = gsn_mist_form;
    	af.level     = ch->level;
    	af.duration  = 11;
    	af.modifier  = 0;  
    	af.location  = APPLY_NONE;
    	af.bitvector = AFF_PASS_DOOR;
    	affect_to_char( ch, &af );
    	af.bitvector = 0;
    	af.location  = APPLY_DAMROLL;
    	af.modifier  = -30;  
    	affect_to_char( ch, &af );
    	af.location  = APPLY_HITROLL;
    	af.modifier  = -30;  
    	affect_to_char( ch, &af );
    	af.location  = APPLY_CON;
    	af.modifier  = -5;  
    	affect_to_char( ch, &af );
    	af.location  = APPLY_DEX;
    	af.modifier  = -5;  
    	affect_to_char( ch, &af );
    	af.location  = APPLY_STR;
    	af.modifier  = -10;  
    	affect_to_char( ch, &af );
	ch->short_descr = str_dup("a cloud of mist");
	check_improve(ch,gsn_mist_form,TRUE,1);
//end of turning into mist.
    act("A cloud of mist hovers near the ground billowing in the breeze and then it's gone.",ch,NULL,NULL,TO_ROOM);
}//end if else.

    af.where	= TO_AFFECTS;
    af.type   = gsn_burrow;
    af.level  = ch->level;
    af.duration = 11;
    af.modifier = ch->in_room->vnum;
    af.location = 0;
    af.bitvector = 0;
    affect_to_char(ch,&af);


    SET_BIT(ch->comm,COMM_QUIET);
    char_from_room(ch);
    char_to_room(ch, get_room_index(ROOM_VNUM_BURROW));
}

void do_dark_ritual(CHAR_DATA *ch, char *argument)
{
    int chance;
    AFFECT_DATA af;
    bool fElder = get_skill(ch, gsn_unholy_gift) > 1;
    const int cost = fElder ? 50 : 100;

    if ((chance = get_skill(ch,gsn_dark_ritual)) == 0)
    {
        send_to_char("You don't know how to perform a dark ritual.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_dark_ritual))
    {
	send_to_char("You are not ready to perform another dark ritual.\n\r",ch);
	return;
    }
    if (ch->mana < cost)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    WAIT_STATE2( ch, skill_table[gsn_dark_ritual].beats );
    if (number_percent() > chance)
    {
	send_to_char("You failed to perform a dark ritual.\n\r",ch);
	check_improve(ch,gsn_dark_ritual,FALSE,2);
	ch->mana -= cost / 2;
	return;
    }
    act("You perform an ancient ritual to the Covenant of Blood.",ch,NULL,NULL,TO_CHAR);
    act("$n performs an ancient ritual to the Covenant of Blood.",ch,NULL,NULL,TO_ROOM);
    ch->mana -= cost;
    af.where	= TO_AFFECTS;
    af.type   = gsn_dark_ritual;
    af.level  = ch->level;
    af.duration = fElder ? 18 : 30;
    af.modifier = 0;
    af.location = 0;
    af.bitvector = 0;
    affect_to_char(ch,&af);
    af.type   = gsn_covenant;
    af.duration = 12;
    affect_to_char(ch,&af);
    send_to_char("The Covenant of Blood has given you their dark blessings.\n\r",ch);
    check_improve(ch,gsn_dark_ritual,TRUE,1);
}

void do_insect_swarm(CHAR_DATA *ch, char *argument)
{

  /*
Insect Swarm:
- Ranger summons and directs a swarm of insects upon a single target.
- The target is hit, and keeps getting hit each tick afterwards.
- THe swarm has a chance to do any of:poison or plauge or drain.
- All the effects are done through gen_insect_swarm
the skill only sets the effect.
  */

  char arg[MIL];
  char buf[MIL];
  CHAR_DATA* victim;

  AFFECT_DATA af;
  AFFECT_DATA* paf;  
//const
  const int skill_med = 75;
  const int skill_mod = 2;
  const int base_chance = 40;
  const int duration_gsn = number_fuzzy (56 - ch->level);
  const int min_mana = skill_table[gsn_insect_swarm].min_mana;
  
//data
  int chance = base_chance;
  int skill = 0;
  int duration_gen = 0;

 
//flags
  bool fAutoFail = FALSE;

//EZ
  if (ch == NULL)
    return;

//skill check
  if ((skill = get_skill(ch,gsn_insect_swarm)) == 0)
    {
      send_to_char("You don't know how to summon insects.\n\r",ch);
      return;
    }
  
//arg check
  argument = one_argument(argument,arg);
  if (arg[0] == '\0')
    {
      send_to_char("Whom would you like to plauge with the swarm?\n\r", ch);
      return;
    }

//check if we can see target
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
      {
        send_to_char("They aren't here.\n\r",ch);
        return;
      }
    
//check self.
    if (victim == ch)
      {
	send_to_char("Why not just sit on an anthill?\n\r", ch);
	return;
      }

//pk check
    if (is_safe(ch,victim))
      return;

//Check min mama
    if (ch->mana < min_mana)
      {
	send_to_char("You lack the strength.\n\r", ch);
	return;
      }
    else
      ch->mana -= min_mana;

//message
    act("$n tosses a small handfull of thick pollen at $N.", ch, NULL, victim, TO_ROOM);
    act("You toss a small handfull of thick pollen at $N.", ch, NULL, victim, TO_CHAR);

    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "insectswarm", skill_table[gsn_insect_swarm].name))
      return;

    //yell and attack.
    sprintf(buf, "Help! %s just tried to send a swarm after me!", PERS(ch, victim));
    j_yell(victim,buf);

//end we attack.
    damage(ch, victim, 1, gsn_insect_swarm, DAM_PIERCE, TRUE);





//check for auto fails.
    if ( (is_affected(ch, gsn_insect_swarm)) 
	 || (ch->in_room->sector_type == SECT_CITY
	     || ch->in_room->sector_type == SECT_INSIDE
	     || ch->in_room->sector_type == SECT_WATER_SWIM
	     || ch->in_room->sector_type == SECT_WATER_NOSWIM) )
      fAutoFail = TRUE;




    WAIT_STATE2( ch, skill_table[gsn_insect_swarm].beats );

//check for misc. failures.
    if (fAutoFail){
	send_to_char("You attempt to summon the insects but only few bumble bees fly by.\n\r", ch);
	check_improve(ch,gsn_insect_swarm,FALSE,1);
	return;
      }

//get chance of success.
    chance += (skill - skill_med) * skill_mod;

//set waitstate


    
//set effect for another use.
    if (number_percent() > chance)
      {
	send_to_char("You summon a swarm but without adequate skill it disperses quicly.\n\r",ch);
	act("A swarm of insects hovers above $N and then quickly disperses.", ch, NULL, victim, TO_NOTVICT);
	act("A swarm of insects hovers above you and then quickly disperses.", ch, NULL, NULL, TO_NOTVICT);
	check_improve(ch,gsn_insect_swarm,FALSE,2);
	return;
      }
    check_improve(ch,gsn_insect_swarm,TRUE,1);
    

    af.type = gsn_insect_swarm;
    af.where = TO_AFFECTS;
    af.level = ch->level;
    af.duration = duration_gsn;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = 0;
    affect_to_char(ch, &af);

//begin the gen.
    //if already paluged we just extend dur.
    if ( (paf = affect_find(ch->affected, gen_insect_swarm)) == NULL)
      {
	//duration is dependant on skill.
	duration_gen += number_fuzzy((skill - skill_med) / 8);
	af.type = gen_insect_swarm;
	af.where = TO_AFFECTS;
	af.level = ch->level;
	af.duration = UMAX(0, duration_gen);
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = 0;
	affect_to_char(victim, &af);
      }
    else
      paf->duration +=1;
//now we apply the name of caster as gen needs it.
    paf = affect_find(victim->affected, gen_insect_swarm);
    string_to_affect(paf, ch->name);

//now that the name is in there we run the  damage part of 
//insect swarm, mainly update_tick
run_effect_update(victim, ch, NULL, gen_insect_swarm, NULL, NULL, TRUE, EFF_UPDATE_TICK);
}//end do_insect swarm

void do_cutpurse(CHAR_DATA *ch, char *argument)
{
    char arg[MIL];
    int chance;
    AFFECT_DATA af;
    CHAR_DATA *victim;
    argument = one_argument(argument,arg);
    if ((chance = 3*get_skill(ch,gsn_cutpurse)/4) == 0)
    {
        send_to_char("Cutpurse? What's what?\n\r",ch);
        return;
    }
    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char("But you aren't fighting anyone!\n\r",ch);
            return;
        }
    }
    else if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
    if (is_safe(ch,victim))
        return;
    if (is_affected(victim,gen_gold_loss))
    {
	act("$S purse is already slashed.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
        return;
    }
/* THEFT CRIME CHECK */
    if (ch->in_room && is_crime(victim->in_room, CRIME_THEFT, victim)){
      set_crime(ch, victim, ch->in_room->area, CRIME_THEFT);
    }
    WAIT_STATE2( ch, skill_table[gsn_cutpurse].beats );

    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "cutpurse", skill_table[gsn_cutpurse].name))
      return;

    if (IS_AWAKE(victim)){
      damage(ch,victim,1,gsn_cutpurse,DAM_SLASH,TRUE);
      
      if ( ch->fighting != victim && victim->fighting != ch){
	char buf[MIL];
	
	sprintf(buf, "Help! %s just cut my purse apart!",PERS(ch,victim));
	j_yell(victim,buf);
      }

      if (!can_see(victim,ch))
	chance += 20;

      if (number_percent() > chance)
	{
	  act("You failed to slash $S purse.",ch,NULL,victim,TO_CHAR);
	  check_improve(ch,gsn_cutpurse,FALSE,2);
	  return;
	}
    }
    act("You slash a hole in $N's purse!",ch,NULL,victim,TO_CHAR);
    af.where	= TO_NONE;
    af.type   = gen_gold_loss;
    af.level  = ch->level;
    af.duration = 6;
    af.modifier = 0;
    af.location = 0;
    af.bitvector = 0;
    affect_to_char(victim,&af);
    check_improve(ch,gsn_cutpurse,TRUE,1);
}

void power_tiger( CHAR_DATA* ch, bool fSuccess ){
  CHAR_DATA* victim;

  if ( (victim = ch->fighting) == NULL){
    send_to_char("But you're not fighting!\n\r", ch);
    return;
  }

  one_hit(ch, victim, attack_lookup("charge") + TYPE_NOBLOCKHIT, FALSE);  
  if (!fSuccess || ch->fighting == NULL || ch->fighting != victim)
    return;

  one_hit(ch, victim, attack_lookup("bite") + TYPE_HIT, FALSE);  
  if (ch->fighting == NULL || ch->fighting != victim)
    return;

  one_hit(ch, victim, attack_lookup("claw") + TYPE_HIT, FALSE);  
}

void power_wolf(CHAR_DATA* ch, bool fSuccess ){
  const int mag_att = attack_lookup("tainted");
  const int nor_att = attack_lookup("bite");

  if (ch->dam_type == mag_att){
    send_to_char("You retract your claws.\n\r", ch);
    act("With a soft hiss $n retracts $s claws.", ch, NULL, NULL, TO_ROOM);
    ch->dam_type = nor_att;
    affect_strip(ch, gsn_werepower );
  }
  else{
    AFFECT_DATA* paf, af;

    paf = affect_find(ch->affected, gsn_werewolf );

    af.type = gsn_werepower;
    af.level = ch->level;
    af.duration = paf ? paf->duration : 0;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_HITROLL;
    af.modifier = ch->level / 10;
    affect_to_char( ch, &af);
    af.location = APPLY_DAMROLL;
    affect_to_char( ch, &af);
    send_to_char("You extend your claws.\n\r", ch);
    act("With a soft hiss $n extends $s claws.", ch, NULL, NULL, TO_ROOM);
    ch->dam_type = mag_att;
  }
}


void power_bear( CHAR_DATA* ch, bool fSuccess ){
  CHAR_DATA* victim;
  int dam = number_range(3 * ch->level / 2, 5 * ch->level / 2);

  if ( (victim = ch->fighting) == NULL){
    send_to_char("But you're not fighting!\n\r", ch);
    return;
  }
  damage(ch, victim, dam, gsn_werebear, DAM_BASH, TRUE );
  if (ch->fighting == NULL || ch->fighting != victim)
    return;
  if (number_percent() < ch->level / 2)
    wound_effect(ch, victim, ch->level, dam );

}

void power_falcon( CHAR_DATA* ch, bool fSuccess, char* argument ){
  EXIT_DATA* pe;

  if (ch->fighting){
    send_to_char( "No way!  You are still fighting!\n\r", ch);
    return;
  }
  else if (ch->in_room->sector_type == SECT_INSIDE || IS_SET(ch->in_room->room_flags, ROOM_INDOORS)){
    if (ch->move < 15){
      send_to_char("You could not find a way out into the sky.\n\r", ch);
      return;
    }
    else{
      send_to_char("You fly around for a while before spotting a passage up into the sky.\n\r", ch);
      ch->move -= 25;
    }
  }
  send_to_char("You soar up into the sky!\n\r", ch);
  act("$n soars high up into the sky!", ch, NULL, NULL, TO_ROOM);

  sendf(ch, "\n\r%s\n\r", ch->in_room->area->name );
  where(ch, ch->in_room->area, argument );

  if (!fSuccess){
    send_to_char("You failed to get enough alttitude.\n\r", ch);
    return;
  }

  for (pe = ch->in_room->area->exits; pe; pe = pe->next_in_area ){
    if (pe->to_room == NULL || pe->to_room->area == NULL)
      continue;
    else if (IS_AREA(pe->to_room->area, AREA_MARKED))
      continue;
    else if (pe->to_room->area->nplayer < 1)
      continue;
    SET_BIT(pe->to_room->area->area_flags, AREA_MARKED);

    sendf(ch, "\n\r%s\n\r", pe->to_room->area->name );
    where(ch, pe->to_room->area, argument );
  }

  for (pe = ch->in_room->area->exits; pe; pe = pe->next_in_area ){
    if (pe->to_room == NULL || pe->to_room->area == NULL)
      continue;
    REMOVE_BIT(pe->to_room->area->area_flags, AREA_MARKED);
  }
}



void do_werepower(CHAR_DATA *ch, char *argument){
  AFFECT_DATA* paf;
  const int sn = skill_lookup("werepower");
  int cost;
  int lag;
  const int skill = get_skill(ch, sn);
  int chance;

  bool fSuccess = FALSE;

  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  for (paf = ch->affected; paf; paf = paf->next){
    if (paf->type == gsn_weretiger
	|| paf->type == gsn_werewolf
	|| paf->type == gsn_werebear
	|| paf->type == gsn_werefalcon){
      break;
    }
  }

  if (paf == NULL){
    send_to_char("You are not in your beastly form!\n\r", ch);
    return;
  }
  cost = skill_table[paf->type].min_mana;
  lag = skill_table[paf->type].beats;

  if (ch->mana < cost){
    send_to_char("You don't have enough mana.\n\r",ch);
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2(ch, lag );

  chance = 4 * skill / 5;
  chance += (get_curr_stat(ch, STAT_LUCK) - 12) * 2;

  if (number_percent() < chance){
    check_improve(ch, sn, TRUE, 1);
    fSuccess = TRUE;
  }else{
    check_improve(ch, sn, FALSE, 1);
    fSuccess = FALSE;
  }

  if (paf->type == gsn_weretiger)	power_tiger( ch, fSuccess );
  else if (paf->type == gsn_werewolf)	power_wolf( ch, fSuccess );
  else if (paf->type == gsn_werebear)	power_bear( ch, fSuccess );
  else if (paf->type == gsn_werefalcon)	power_falcon( ch, fSuccess, argument );
  else
    send_to_char("No such werebeast.\n\r", ch);
}

void do_transform(CHAR_DATA *ch, char *argument){
  const int sn = gsn_transform;
  int cost = skill_table[sn].min_mana;
  int lag = skill_table[sn].beats;
  const int skill = get_skill(ch, sn);

  int chance;

  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (IS_NPC(ch)){
    send_to_char("Mobs can't transform.\n\r",ch);
    return;
  }
  else if (is_affected(ch,gsn_transform)){
    send_to_char("You are not ready to use this ability again.\n\r",ch);
    return;
  }
  if (ch->fighting){
    cost /= 2;
    lag /= 2;
  }
  if (ch->mana < cost){
    send_to_char("You don't have enough mana.\n\r",ch);
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2(ch, lag );

  if (ch->fighting)
    chance = 4 * skill / 5;
  else
    chance = skill / 2;

  chance += 30 - (30 * ch->hit / ch->max_hit);
  chance += (get_curr_stat(ch, STAT_LUCK) - 14) * 2;

  if (number_percent() > chance ){
    send_to_char("You feel some discomfort but fail to transform.\n\r",ch);
    act("$n looks a little uncomfortable, but nothing happens.",ch,NULL,NULL,TO_ROOM);
    check_improve(ch, sn, FALSE, 1);
    return;
  }

  check_improve(ch, sn, TRUE, 1);
  beast_transform(ch);
}

void tiger_form(CHAR_DATA *ch ){
  AFFECT_DATA af;    

  ch->dam_type = attack_lookup("slice");

  af.type      = gsn_weretiger;
  af.level     = ch->level;
  af.duration  = 72;

  af.where     = TO_AFFECTS;
  af.bitvector = 0;
  af.location  = APPLY_STR;
  af.modifier  = 2;
  affect_to_char( ch, &af );
  
  af.location  = APPLY_HIT;
  af.modifier  = ch->level;
  affect_to_char( ch, &af );

  af.location  = APPLY_HIT_GAIN;
  af.modifier  = 10 + ch->level / 2;
  affect_to_char( ch, &af );
  
  send_to_char("The strength of the tiger fills your veins.\n\r", ch);
}

void wolf_form(CHAR_DATA *ch ){
    AFFECT_DATA af;
    ch->dam_type = attack_lookup("bite");

    af.type      = gsn_werewolf;
    af.level     = ch->level;
    af.duration  = 72;

    af.where     = TO_RESIST;
    af.bitvector = RES_WEAPON;
    af.location  = APPLY_STR;
    af.modifier  = 1;
    affect_to_char( ch, &af );

    af.location  = APPLY_DEX;
    af.modifier  = 1;
    affect_to_char( ch, &af );

    af.location  = APPLY_AC;
    af.modifier  = -ch->level;
    affect_to_char( ch, &af );
    send_to_char("You are now resistant to non-magical damage.\n\r", ch);
}

void bear_form(CHAR_DATA *ch ){
  AFFECT_DATA af;
  ch->dam_type = attack_lookup("claw");
  
  affect_strip(ch, skill_lookup("enlarge"));

  af.type      = gsn_werebear;
  af.level     = ch->level;
  af.duration  = 72;
  
  af.where     = TO_AFFECTS;
  af.bitvector = 0;
  af.location  = APPLY_STR;
  af.modifier  = 3;
  affect_to_char( ch, &af );

  af.where     = TO_AFFECTS;
  af.bitvector = 0;
  af.location  = APPLY_DEX;
  af.modifier  = -2;
  affect_to_char( ch, &af );

  af.location  = APPLY_CON;
  af.modifier  = 2;
  affect_to_char( ch, &af );
  
  af.location  = APPLY_SIZE;
  af.modifier  = 1;
  affect_to_char( ch, &af );

  send_to_char("You nearly double in size as your bones and flesh expands.\n\r", ch);
}

void falcon_form(CHAR_DATA *ch ){
  AFFECT_DATA af;
  ch->dam_type = attack_lookup("peck");
  
  af.type      = gsn_werefalcon;
  af.level     = ch->level;
  af.duration  = 72;
  
  af.where     = TO_AFFECTS;
  af.bitvector = AFF_FLYING;
  af.location  = APPLY_DEX;
  af.modifier  = 3;
  affect_to_char( ch, &af );
  
  af.location  = APPLY_MOVE;
  af.modifier  = ch->level * 3;
  affect_to_char( ch, &af );
  ch->move += af.modifier;
  af.where = TO_SKILL;
  af.location = gsn_dodge;
  af.modifier = ch->level / 5;
  affect_to_char( ch, &af );

  send_to_char("You spread your wings and take to the sky.\n\r", ch);
}

void beast_transform(CHAR_DATA *ch ){
  AFFECT_DATA af;
  int type = IS_NPC(ch) ? ch->level % BEAST_RACE_MAX : ch->pcdata->beast_type;

  send_to_char("You scream out in agonizing pain as your body starts to deform.\n\r",ch);
  act("$n screams out in agonizing pain as $s body starts to deform.",ch,NULL,NULL,TO_ROOM);

  switch(type){
  case (BEAST_RACE_TIGER):
    tiger_form(ch); break;
  case (BEAST_RACE_WOLF):
    wolf_form(ch); break;
  case (BEAST_RACE_BEAR):
    bear_form(ch); break;
  case (BEAST_RACE_FALCON):
    falcon_form(ch); break;
  default:
    send_to_char("You can't transform into anything.\n\r",ch);
    return;
  }

  af.type      = gsn_transform;
  af.level     = ch->level;
  af.duration  = 1;
  af.where     = TO_AFFECTS;
  af.bitvector = 0;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  affect_to_char( ch, &af );
}

void do_shed(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    char buf[MSL];
    CHAR_DATA *skin, *fch;
    if (IS_NPC(ch))
    {
	send_to_char("Mobs can't shed!\n\r",ch);
	return;
    }
    if (ch->race != race_lookup("slith"))
    {
	send_to_char("You can't shed your skin!\n\r",ch);
	return;
    }
    if (ch->fighting == NULL)
    {
	send_to_char("You're not fighting anyone!\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_shed_skin))
    {
	send_to_char("You are not ready to shed again.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,8);
    af.where     = TO_AFFECTS;
    af.type      = gsn_shed_skin;
    af.level     = ch->level;
    af.duration  = 60;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    act("You cast off your old skin in your place.",ch,NULL,NULL,TO_CHAR);

    skin = create_mobile( get_mob_index(MOB_VNUM_SKIN));
    skin->sex = ch->sex;
    skin->max_hit = UMAX(10, ch->max_hit );
    skin->hit = ch->max_hit;
    skin->alignment = ch->alignment;
    sprintf( buf, "a cast off skin of %s", ch->name);
    free_string( skin->short_descr );
    skin->short_descr = str_dup( buf );
    sprintf( buf, "A cast off skin of %s is here.\n\r", ch->name);
    free_string( skin->long_descr );
    skin->long_descr = str_dup( buf );
    char_to_room(skin,ch->in_room);
    skin->fighting = ch->fighting;
    skin->position = POS_FIGHTING;
    ch->fighting = NULL;
    ch->position = POS_STANDING;
    update_pos( ch );
    affect_strip( ch, gsn_phantasmal_griffon );
    affect_strip( ch, gsn_illusionary_spectre );
    affect_strip( ch, gsn_phantom_dragon );
    affect_strip( ch, gsn_throw );
    affect_strip( ch, skill_lookup("faerie fire") );
    affect_strip( ch, skill_lookup("faerie fog") );
    REMOVE_BIT(ch->affected_by, AFF_FAERIE_FIRE );

    set_delay(ch, ch);
    af.where     = TO_AFFECTS;
    af.type      = gsn_timer;
    af.level     = ch->level;
    af.duration  = 3;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( skin, &af );
    for (fch = ch->in_room->people; fch != NULL; fch = fch->next)
	if (fch->fighting == ch)
	    fch->fighting = skin;
}	    

void do_swallow(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *obj;
  char arg[MIL];
  one_argument(argument,arg);
  if (ch->race != race_lookup("slith") || !IS_NEUTRAL(ch))
    {
      send_to_char("You can't swallow that.\n\r",ch);
      return;
    }
  if ( arg[0]=='\0' )
    {
      send_to_char("What do you want to swallow?\n\r",ch);
      return;
    }
    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    if ( (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC )
	 || obj->condition < 2)
    {
      send_to_char("You don't like the taste of that.\n\r",ch);
      return;
    }
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER] > 30)
    {
	send_to_char("You are too full to eat more.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,12);
    act("You unhinge your jaws and swallow $p whole.",ch,obj,NULL,TO_CHAR);
    act("$n unhinges $s jaws and swallow $p whole.",ch,obj,NULL,TO_ROOM);
    free_string(obj->description);
    obj->description = str_dup("A digested corpse lies here.");
    obj->condition = 1;
    gain_condition( ch, COND_HUNGER, obj->level );
}

void do_probe(CHAR_DATA *ch, char *argument)
{
    EXIT_DATA * pexit;
    ROOM_INDEX_DATA * room;
    extern char * const dir_name[];
    char arg[MIL];   
    int dir, skill;
    CHAR_DATA *vch;
    bool danger = FALSE;

    if (IS_NPC(ch))
	return;

    if ( (skill = get_skill(ch, gsn_probe)) < 1){
      send_to_char("Huh?\n\r", ch);
      return;
    }


    argument = one_argument(argument, arg);
    if      (!str_cmp(arg, "n") || !str_cmp(arg, "north")) dir = 0;
    else if (!str_cmp(arg, "e") || !str_cmp(arg, "east"))  dir = 1;
    else if (!str_cmp(arg, "s") || !str_cmp(arg, "south")) dir = 2;
    else if (!str_cmp(arg, "w") || !str_cmp(arg, "west"))  dir = 3;
    else if (!str_cmp(arg, "u") || !str_cmp(arg, "up" ))   dir = 4;
    else if (!str_cmp(arg, "d") || !str_cmp(arg, "down"))  dir = 5;
    else
    {
        send_to_char("Which way do you wish to probe?\n\r",ch);
        return;
    }
    WAIT_STATE2(ch, 6);
    act("$n probes $T.",ch,NULL,dir_name[dir],TO_ROOM);

    if (number_percent() > skill){
      act("You probe $T, but your instincts fail you.",ch,NULL,dir_name[dir],TO_CHAR);
      check_improve( ch, gsn_probe, FALSE, 1);
      return;
    }
    else
      check_improve( ch, gsn_probe, TRUE, 1);

    pexit = ch->in_room->exit[dir];
    if ((pexit == NULL) || (pexit->to_room == NULL) || (IS_SET(pexit->exit_info, EX_CLOSED)))
    {
	send_to_char("You cannot probe in that direction.\n\r",ch);
	return;
    }
    room = pexit->to_room;
    if (room->exit[0] == NULL && room->exit[1] == NULL && room->exit[2] == NULL 
	&& room->exit[3] == NULL && room->exit[4] == NULL && room->exit[5] == NULL)
	danger = TRUE;
    else if (room->pCabal && ch->pCabal && room->pCabal != ch->pCabal)
	danger = TRUE;
    else 
    {
	for (vch = room->people; vch != NULL; vch = vch->next_in_room )
	{
	    if (IS_NPC(vch) && IS_SET(vch->act,ACT_AGGRESSIVE) && vch->level > ch->level - 5)
	    {
		danger = TRUE;
		break;
	    }
	}
    }
    if (danger)
	act("You probe $T, and your instincts warn you of danger!",ch,NULL,dir_name[dir],TO_CHAR);
    else
	act("You probe $T, and your instincts detects nothing dangerous.",ch,NULL,dir_name[dir],TO_CHAR);
}

void do_decoy(CHAR_DATA *ch, char *argument)
{
    int chance;
    CHAR_DATA *decoy;
    char buf[MSL];
    if (IS_NPC(ch))
	return;
    if ((chance = get_skill(ch,gsn_decoy)) == 0)
    {
        send_to_char("You don't know how to set a decoy.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_decoy))
    {
        send_to_char("You are not ready to set another decoy.\n\r",ch);
        return;
    }
    if (ch->mana < 30) 
    {
        send_to_char("Rest up first.\n\r",ch);
        return;
    }
    WAIT_STATE2( ch, skill_table[gsn_decoy].beats );
    if (number_percent() < chance)
    {
        AFFECT_DATA af;
	af.where	= TO_AFFECTS;
        af.type   = gsn_decoy;
        af.level  = ch->level;
        af.duration = 12;
        af.modifier = 0;
        af.location = 0;
        af.bitvector = 0;
        affect_to_char(ch,&af);
        ch->mana -= 30;
	act("You set a decoy of yourself.",ch,NULL,NULL,TO_CHAR);
	if (!IS_AFFECTED(ch, AFF_HIDE) && !IS_AFFECTED(ch, AFF_SNEAK)){
	  act("$n sets a decoy of $mself.",ch,NULL,NULL,TO_ROOM);
	}
	decoy = create_mobile( get_mob_index(MOB_VNUM_DECOY) );
	free_string(decoy->name);
        free_string(decoy->short_descr);
        free_string(decoy->long_descr);
	sprintf(buf,"%s", ch->name);
        decoy->name = str_dup(buf);

	sprintf(buf,"%s", ch->name);
        decoy->short_descr = str_dup(buf);

	sprintf(buf,"%s%s is here.\n\r",ch->name,ch->pcdata->title);
	sprintf(buf,"%s%s is here.\n\r",ch->name,ch->pcdata->title);
        decoy->long_descr = str_dup(buf);
        decoy->sex = ch->sex;
	decoy->level = ch->level + 1;
        char_to_room(decoy,ch->in_room);

        af.where = TO_AFFECTS;
        af.type = gsn_decoy;  
        af.level     = ch->level;
        af.duration = -1;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char(decoy,&af); 

        af.type = gsn_timer;  
        af.level     = ch->level;
        af.duration = ch->level/4;
        affect_to_char(decoy,&af); 
	decoy->summoner = ch;
        check_improve(ch,gsn_decoy,TRUE,1);
    }
    else
    {
        ch->mana -= 15;
        act("You failed to set a decoy.",ch,NULL,NULL,TO_CHAR);
        check_improve(ch,gsn_decoy,FALSE,2);
    }
}

void do_crusade_old( CHAR_DATA *ch, char *argument)
{
    int chance;
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    bool found = FALSE;
    if ((chance = get_skill(ch,gsn_crusade)) == 0)
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
    if (is_affected(ch,gsn_blasphemy))
    {
	send_to_char("You have no religion to crusade for.\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_crusade))
    {
	send_to_char("You are already on a crusade.\n\r",ch);
	return;
    }
    if (ch->mana < 30)
    {
	send_to_char("You better rest up before the crusade.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_CALM))
    {
        send_to_char("You can't get worked up in your calm state.\n\r",ch);
        return;
    }
    if (ch->leader != NULL)
    {
	send_to_char("But you're not the leader of your group!\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_crusade].beats);
    if (number_percent() > chance)
    {
    	ch->mana -= 15;
	send_to_char("You failed to start on your crusade.\n\r",ch);
	check_improve(ch,gsn_crusade,FALSE,2);
	return;
    }
    ch->mana -= 30;
    sprintf(buf,"Gods of Light, guide us in our holy path!");
    do_yell(ch,buf);
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next)
    {
	if (!is_same_group(vch,ch) || vch == ch || vch->leader != ch)
	    continue;
    	if (IS_AFFECTED(vch,AFF_CALM) || is_affected(vch,gsn_blasphemy) || is_affected(vch,gsn_crusade))
	    continue;
	if (IS_EVIL(vch))
	    continue;
	found = TRUE;
	act("You are filled with the righteousness of purpose.",vch,NULL,NULL,TO_CHAR);
	act("$n is filled with the righteousness of purpose.",vch,NULL,NULL,TO_ROOM);
    	af.where		= TO_AFFECTS;
    	af.type			= gsn_crusade;
    	af.duration		= ch->level/2;
    	af.level		= ch->level;
    	af.bitvector		= 0;
    	af.location		= APPLY_SAVING_SPELL;
    	af.modifier		= 0 - (ch->level/10 - 2);
    	affect_to_char(vch,&af);
    	af.modifier		= 1;
    	af.location		= APPLY_LUCK;
    	affect_to_char(vch,&af);
    }
    if (!found)
    {
	send_to_char("No one is here to join you on your crusade.\n\r",ch);
	return;
    }
    af.where		= TO_AFFECTS;
    af.type		= gsn_crusade;
    af.duration		= ch->level/2;
    af.level		= 0;
    af.modifier		= 0;
    af.location		= 0;
    af.bitvector	= 0;
    affect_to_char(ch,&af);
    check_improve(ch,gsn_crusade,TRUE,1);
}
	
void do_heroism( CHAR_DATA *ch, char *argument)
{
    int chance;
    AFFECT_DATA af;
    CHAR_DATA *fch, *fch_next;

    if ((chance = get_skill(ch,gsn_heroism)) == 0)
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
    if (is_affected(ch,gsn_blasphemy))
    {
	send_to_char("You faith is too shaky.\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_heroism))
    {
	send_to_char("You are already divinely blessed.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_BERSERK))
    {
	send_to_char("You get a little madder.\n\r",ch);
	return;
    }
    if (ch->mana < 30)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_CALM))
    {
        send_to_char("You can't get worked up in your calm state.\n\r",ch);
        return;
    }
    WAIT_STATE2(ch,skill_table[gsn_heroism].beats);
    if (number_percent() > chance)
    {
    	ch->mana -= 15;
	send_to_char("You fail to receive divine favor.\n\r",ch);
	check_improve(ch,gsn_heroism,FALSE,2);
	return;
    }

    act("You have been divinely empowered!",ch,NULL,NULL,TO_CHAR);
    act("$n has been divinely empowered!",ch,NULL,NULL,TO_ROOM);
    ch->mana -= 30;
    af.where		= TO_AFFECTS;
    af.type		= gsn_heroism;
    af.duration		= 24;
    af.level		= ch->level;
    af.modifier		= number_range(2,5);
    af.location		= APPLY_LUCK;
    af.bitvector	= AFF_BERSERK;
    affect_to_char(ch,&af);
    af.modifier		= ch->level / 8;
    af.location		= APPLY_HITROLL;
    af.bitvector	= 0;
    affect_to_char(ch,&af);
    af.location		= APPLY_DAMROLL;
    affect_to_char(ch,&af);
    af.location		= APPLY_AC;
    af.modifier		= 10 * (ch->level / 12);
    affect_to_char(ch,&af);

    stop_follower(ch);

    /* remove non justice pets */
    if ( ch->master != NULL ){
      stop_follower( ch );
    }
    for ( fch = char_list; fch != NULL; fch = fch_next ){
      fch_next = fch->next;
      if (IS_NPC(fch) 
	  && (fch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD 
	      || fch->pIndexData->vnum == MOB_VNUM_HOUND))
	continue;
      if ( fch->master == ch )
	stop_follower( fch );
      if ( fch->leader == ch)
	fch->leader = fch;
    }    
    check_improve(ch,gsn_heroism,TRUE,1);
}

void do_tarot( CHAR_DATA *ch, char *argument)
{
    int chance, number, card = 0;
    AFFECT_DATA af;
    char arg[MIL], arg2[MIL]; 
    CHAR_DATA *victim;
    argument = one_argument(argument,arg);
    argument = one_argument(argument,arg2);

    if (((chance = get_skill(ch,gsn_tarot)) == 0) && (!IS_NPC(ch)))
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
    if (arg[0] == '\0')
    {
	send_to_char("Whose fortune do you wish to fortell?\n\r",ch);
	return;
    }
    if (ch->mana < 100)
    {
	send_to_char("Rest up first.\n\r",ch);
	return;
    }
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
    if (IS_NPC(victim))
    {
	send_to_char("Why would you care for the fate of a mob?\n\r",ch);
	return;
    }
    if (!is_same_group(victim,ch) && victim->master != ch && !IS_IMMORTAL(ch) && !IS_NPC(ch))
    {
	send_to_char("They have to be following you.\n\r",ch);
        return;
    }
    if (is_affected(victim,gsn_tarot))
    {
    	if (ch == victim)
	    send_to_char("Your fate is already determined.\n\r",ch);
	else
	    send_to_char("Their fate is already determined.\n\r",ch);
	return;
    }
    ch->mana -= 100;
    if ((number_percent() > chance) && (!IS_NPC(ch)))
    {
	send_to_char("You failed to interpret the tarot cards.\n\r",ch);
    	check_improve(ch,gsn_tarot,FALSE,1);
	return;
    }
    if ((IS_IMMORTAL(ch) && arg2[0] != '\0') || (IS_NPC(ch) && arg2[0] != '\0'))
    {
	if (!is_number(arg2))
	{
	    send_to_char("Must be numeric.\n\r",ch);
	    return;
	}	
	card = atoi(arg2);
	if (card < 1 || card > 22)
	{
	    send_to_char("Pick a number from 1 to 22.\n\r",ch);
	    return;
	}

    }	
    else
    {
    number = number_range(0,1000);
    if (number > 950)
	card = 1;
    else if (number > 900)
	card = 2;
    else if (number > 850)
	card = 3;
    else if (number > 800)
	card = 4;
    else if (number > 750)
	card = 5;
    else if (number > 740)
	card = 6;
    else if (number > 690)
	card = 7;
    else if (number > 630)
	card = 8;
    else if (number > 580)
	card = 9;
    else if (number > 570)
	card = 10;
    else if (number > 500)
	card = 11;
    else if (number > 498)
	card = 12;
    else if (number > 444)
	card = 13;
    else if (number > 442)
	card = 14;
    else if (number > 363)
	card = 15;
    else if (number > 393)
	card = 16;
    else if (number > 213)
	card = 17;
    else if (number > 143)
	card = 18;
    else if (number > 73)
	card = 19;
    else if (number > 3)
	card = 20;
    else if (number > 1)
	card = 21;
    else 
	card = 22;
    }
    if (card == 0)
	return;
    if (ch == victim)
    {
    	act("$n pulls out a tarot card for $mself.",ch,NULL,NULL,TO_ROOM);
    	sendf(ch,"You draw a tarot card it reveals %s.\n\r",tarot_table[card].name);
    }
    else
    {
    	act("$n pulls out a tarot card for $N.",ch,NULL,victim,TO_NOTVICT);
    	sendf(ch,"You draw a tarot card and reveal %s to %s.\n\r",tarot_table[card].name, PERS(victim,ch));
    	sendf(victim,"%s draws a tarot card and reveals %s to you.\n\r",PERS(ch,victim),tarot_table[card].name);
    }
    af.where	= TO_AFFECTS;
    af.type   	= gsn_tarot;
    af.level  	= ch->level;
    af.duration = 24;
    af.bitvector = 0;
    WAIT_STATE2(ch,skill_table[gsn_tarot].beats); 
    switch (card)
    {
	default:
	    send_to_char("That is not a valid card!\n\r",ch);
	    return;
	// [ Fool ] -dex	    
	case (1):
	    af.modifier = - victim->level/10;
            af.location = APPLY_DEX;
    	    affect_to_char(victim,&af);
	    send_to_char("A burden has been placed upon you...\n\r",victim);
	    break;	    
	// [ Magician ] +int
	case (2):
	    af.modifier = victim->level/10;
            af.location = APPLY_INT;
    	    affect_to_char(victim,&af);
	    send_to_char("You shall be gifted with intelligence...\n\r",victim);
	    break;
	// [ High Priestess ] +wis
	case (3):
	    af.modifier = victim->level/10;
            af.location = APPLY_WIS;
    	    affect_to_char(victim,&af);
	    send_to_char("You shall be blessed with wisdom...\n\r",victim);
	    break;
	// [ Empress ] chance to lose gold per tick
	case (4):
	    send_to_char("Keep an eye on your gold...\n\r",victim);
	    break;
	// [ Emperor ] chance to gain gold per tick
	case (5):
	    send_to_char("You shall increase your fortunes...\n\r",victim);
	    break;
	// [ Hierophant ] cannot leave area
	case (6):
	    af.duration = 12;
	    send_to_char("You mobility shall be restricted...\n\r",victim);
	    break;
	// [ Lovers ] +luck
	case (7):
	    af.modifier = victim->level/5;
            af.location = APPLY_LUCK;
    	    affect_to_char(victim,&af);
	    send_to_char("Luck will be by your side...\n\r",victim);
	    break;
	// [ Chariot ] +move
	case (8):
	    af.modifier = victim->level*2;
            af.location = APPLY_MOVE;
    	    affect_to_char(victim,&af);
	    victim->move += victim->level*2;
	    send_to_char("You shall be able to travel to greater distances...\n\r",victim);
	    break;
	// [ Strength ] +str
	case (9):
	    af.modifier = victim->level/10;
            af.location = APPLY_STR;
    	    affect_to_char(victim,&af);
	    send_to_char("Power has been granted to you...\n\r",victim);
	    break;
	// [ Hermit ] can't group
	case (10):
	    send_to_char("Your travels shall be in solitude...\n\r",victim);
	    break;
	// [ Wheel of Fortune ] luck changes rapidly
	case (11):
	    af.duration = 12;
	    send_to_char("Vague is the outcome...",victim);
	    break;
	// [ Justice ] chance same dam dealt will be dealt back 1/1000
	case (12):
	    send_to_char("An eye for an eye...\n\r",victim);
	    break;
	// [ Hanged Man ] -luck
	case (13):
	    af.modifier = - victim->level/5;
            af.location = APPLY_LUCK;
    	    affect_to_char(victim,&af);
	    send_to_char("Luck has abandoned you...\n\r",victim);
	    break;
	// [ Death ] will die eventually 1/500
	case (14):
	    af.duration = 100;
	    send_to_char("The end, is coming...\n\r",victim);
	    break;
	// [ Temperance ] +hp
	case (15):
	    af.modifier = victim->level/2;
            af.location = APPLY_HIT;
    	    affect_to_char(victim,&af);
	    victim->hit += victim->level/2;
	    send_to_char("You shall be blessed with life...\n\r",victim);
	    break;
	// [ Devil ] cursed
	case (16):
	    af.modifier = - victim->level/10;
            af.location = APPLY_HITROLL;
	    af.bitvector = AFF_CURSE;
    	    affect_to_char(victim,&af);
	    send_to_char("The Gods frown upon you...\n\r",victim);
	    break;
	// [ Tower ] +con
	case (17):
	    af.modifier = victim->level/10;
            af.location = APPLY_CON;
    	    affect_to_char(victim,&af);
	    send_to_char("You shall stand tall and face your difficulties...\n\r",victim);
	    break;
	// [ Star ] save vs
	case (18):
	    af.modifier = - victim->level/5;
            af.location = APPLY_SAVING_SPELL;
    	    affect_to_char(victim,&af);
	    send_to_char("The stars twinkles mystically above you...\n\r",victim);
	    break;
	// [ Moon ] +hit - dam
	case (19):
	    af.modifier = - victim->level/10;
            af.location = APPLY_DAMROLL;
    	    affect_to_char(victim,&af);
	    af.modifier = victim->level/10;
            af.location = APPLY_HITROLL;
    	    affect_to_char(victim,&af);
	    send_to_char("The softness of the moon shines down on you...\n\r",victim);
	    break;
	// [ Sun ] -hit + dam
	case (20):
	    af.modifier = victim->level/10;
            af.location = APPLY_DAMROLL;
    	    affect_to_char(victim,&af);
	    af.modifier = - victim->level/10;
            af.location = APPLY_HITROLL;
    	    affect_to_char(victim,&af);
	    send_to_char("The energy of the sun flows within you...\n\r",victim);
	    break;
	// [ Judgement ] escape death once 1/1000
	case (21):
	    send_to_char("A chance will be given, when judgement day arrives...\n\r",victim);
	    break;    
	// [ World ] +luck +hit +dam +hp 1/1000
	case (22):
	    af.modifier = victim->level/15;
            af.location = APPLY_LUCK;
    	    affect_to_char(victim,&af);
	    af.modifier = victim->level/15;
            af.location = APPLY_DAMROLL;
    	    affect_to_char(victim,&af);
	    af.modifier = victim->level/15;
            af.location = APPLY_HITROLL;
    	    affect_to_char(victim,&af);
	    af.modifier = victim->level/3;
            af.location = APPLY_HIT;
    	    affect_to_char(victim,&af);
	    send_to_char("The world is at your feet...",victim);
	    break;
    }
    af.level	= 69;
    af.modifier = card;
    af.location = 0;
    af.bitvector = 0;
    affect_to_char(victim,&af);
    check_improve(ch,gsn_tarot,TRUE,1);
}

void do_leech( CHAR_DATA *ch, char *argument)
{
    int chance = number_percent(), sn = 0, sn2 = 0, level = 0, count = 0;
    AFFECT_DATA af;
    char arg[MIL], buf[MSL]; 
    CHAR_DATA *victim;
    SKILL_DATA nsk;
    struct skill_type tsk;
    bool found = FALSE;
    argument = one_argument(argument,arg);
    if (IS_NPC(ch))
	return;
    if (ch->race != race_lookup("illithid"))
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
    if (ch->level < 15)
    {
	send_to_char("You are too inexperienced to leech.\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_leech))
    {
	send_to_char("You can only leech one spell at a time.\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_drained))
    {
	send_to_char("You are too drained to leech anymore at the moment.\n\r",ch);
	return;
    }
    if (arg[0] == '\0')
    {
	send_to_char("Whose memory do you want to leech from?\n\r",ch);
	return;
    }
    if (ch->mana < 50)
    {
	send_to_char("Rest up first.\n\r",ch);
	return;
    }
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
    if (victim->race == race_lookup("illithid"))
    {
	send_to_char("You cannot leech from another illithid.\n\r",ch);
	return;
    }
    if (is_affected(victim,gsn_leech))
    {
	send_to_char("You can't leech from that person yet.\n\r",ch);
	return;
    }
    if (IS_NPC(victim))
    {
	send_to_char("You can learn nothing from a mob.\n\r",ch);
	return;
    }
    if (victim->fighting != NULL)
    {
        sendf(ch, "%s won't hold still long enough.\n\r",PERS(victim,ch));
	return;
    }
    if (is_safe(ch,victim))
	return;
    sprintf(buf, "Help, %s is frying my brains!",PERS(ch,victim));
    j_yell(victim,buf);
    act("You wrap your tentacles around $N's head, and emit a light shock.",ch,NULL,victim,TO_CHAR);
    act("$n wraps $s tentacles around your head, and emit a shock of electricity!",ch,NULL,victim,TO_VICT);
    act("$n wraps $s tentacles around $N's head.",ch,NULL,victim,TO_NOTVICT);
    damage(ch,victim,number_range(0,ch->level),gsn_leech,DAM_LIGHTNING,TRUE);
    WAIT_STATE2(ch,skill_table[gsn_leech].beats); 
    ch->mana -= 50;
    if ( chance < 10 )
    {
	af.where	= TO_AFFECTS;
        af.type   	= gsn_hysteria;
        af.level  	= ch->level;
        af.duration 	= 2;
        af.modifier 	= 0;
        af.location 	= 0;
        af.bitvector 	= 0;
        affect_join(victim,&af);
	act("$n looks slightly disoriented.",victim,NULL,NULL,TO_ROOM);
	send_to_char("You panick as your mind is fried.\n\r",ch);
    }
    if ( chance > ch->level/2 + 40 || (victim->class != class_lookup("battlemage") && victim->class != class_lookup("invoker")
	&& victim->class != class_lookup("necromancer") && victim->class != class_lookup("psionicist")))
    {
	send_to_char("You failed.\n\r",ch);
	return;
    }
    act("You can feel $n probe inside your mind!",ch,NULL,victim,TO_VICT);
    if (ch->class == victim->class)
    {
	act("There's nothing that $E knows that you don't.",ch,NULL,victim,TO_CHAR);
	return;
    }
    for (sn = 0; sn < MAX_SKILL; sn++ )
    {
	tsk = skill_table[sn];
	level = tsk.skill_level[victim->class];
	if (level <= victim->level && level <= ch->level - 5
	    && tsk.skill_level[0] != 69 && tsk.spell_fun != spell_null
	    && tsk.min_mana <= 50 && victim->pcdata->learned[sn] > 0 && ch->pcdata->learned[sn] < 1)
	{
	    if (sn == skill_lookup("hellstream") || sn == skill_lookup("acid blast")
		|| sn == skill_lookup("duplicate") || sn == gsn_sleep)
		continue;
            if ( number_range( 0, count ) == 0 )
            {  
                sn2 = sn;
                found = TRUE;
            }
            count++;
	}
    }
    if (found)
    {
	af.where	= TO_AFFECTS;
        af.type   	= gsn_leech;
        af.level  	= ch->level;
        af.duration 	= 6;
        af.modifier 	= 0;
        af.location 	= 0;
        af.bitvector 	= 0;
        affect_to_char(ch,&af);
        af.level  	= victim->level;
        af.duration 	= 48;
        affect_to_char(victim,&af);
        nsk.keep        = 0;
        nsk.sn          = sn2;
        nsk.level       = skill_table[sn2].skill_level[victim->class];
        nsk.rating      = skill_table[sn2].rating[victim->class];
        nsk.learned     = number_range(3*victim->pcdata->learned[sn2]/4,4*victim->pcdata->learned[sn2]/5);
        nskill_to_char(ch,&nsk);
	act("You have learned '$t' from $N's memory!.",ch,skill_table[sn2].name,victim,TO_CHAR);
    }
    else
	act("You failed to leech anything from $N's memory.",ch,NULL,victim,TO_CHAR);
}

void do_tko( CHAR_DATA *ch, char *argument)
{
   char arg[MIL], buf[MSL];
   CHAR_DATA *victim;
   AFFECT_DATA af;
   int chance, tko_duration;
   one_argument(argument,arg);
   if ( (chance = get_skill(ch,gsn_tko)) ==0)
   {
      send_to_char("You don't know how to knock someone out.\n\r",ch);
      return;
   }
   if (arg[0] == '\0')
   {
      send_to_char("Who do you want to knock out?\n\r",ch);
      return;   
   }
   if ((victim = get_char_room(ch, NULL, arg)) == NULL)
   {
     send_to_char("They're not here.\n\r",ch);
     return;
   }
   if (ch->fighting != NULL)
   {
	send_to_char("You can't do that while fighting.\n\r",ch);
	return;
   }
   if (victim->fighting != NULL)
   {
      	sendf(ch, "%s won't hold still long enough.\n\r",PERS(victim,ch));
	return;
   }
   if (IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG) )
   {
      	sendf(ch, "%s is too big for you to attempt such an act.\n\r", PERS(victim,ch));
	return;
   }
   if (victim == ch)
      return;
   if (is_safe(ch, victim) )
      return; 
   if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
   {
      send_to_char("You can't seem to get up enough speed.\n\r",ch);
	return;
   }
   if ( victim->hit < victim->max_hit)
   {
        sendf(ch, "%s is too alert to be knocked out.\n\r", PERS(victim,ch) );
	return;
   }
   if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
   {
      sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
      return;
   }
   if ( IS_AFFECTED2(victim, AFF_PASS_DOOR) || is_affected(victim, gsn_mist_form) || (get_skill(victim, gsn_balance) && is_empowered_quiet(victim,1) && victim->mana > 0) )
   {
      	sprintf(buf, "Don't even try to knock me out, %s!",PERS(ch,victim));
      	j_yell(victim,buf);
	set_fighting(victim,ch);
      
      	act("$n's tries to knock you out but it passes through you.",ch,NULL,victim,TO_VICT);
      	act("Your knock out passes through the translucent target.",ch,NULL,NULL,TO_CHAR);
      	act("$n's knock out passes through $N.",ch,NULL,victim,TO_NOTVICT);
      	damage(ch,victim,0,gsn_tko,DAM_BASH,TRUE);
      	check_improve(ch,gsn_tko,FALSE,1);
	WAIT_STATE2(ch,skill_table[gsn_tko].beats * 3/2); 
      	return;
    }
   chance = 3 * chance /5;
   chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
   chance += URANGE(-10, (ch->level - victim->level), 10);
   chance -= get_skill(victim,gsn_dodge)/25;
   chance += GET_AC2(victim,AC_BASH)/25;
   chance -= get_skill(victim,gsn_counter)/25;
   chance += 4 * (get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK));
   if (!IS_NPC(victim) && victim->pcdata->pStallion != NULL)
     chance -= 12;
   
   a_yell(ch,victim);
   //predict interdict
   if (predictCheck(ch, victim, "knockout", skill_table[gsn_tko].name))
     return;

   tko_duration = number_range(1, 2);
   af.type		= gsn_tko;
   af.level		= ch->level;
   af.duration	= tko_duration +2;
   af.location	= APPLY_NONE;
   af.modifier	= 0;
   af.bitvector	= 0;
   affect_to_char(victim,&af);
   af.where		= TO_AFFECTS;
   af.duration		= tko_duration;
   af.bitvector	= AFF_SLEEP;
   act("You knock $N back with a stunning blow.",ch,NULL,victim,TO_CHAR);
   act("$n knocks $N back with a stunning blow.",ch,NULL,victim,TO_NOTVICT);
   
   if ((number_percent() < (chance/3) || (IS_IMMORTAL(ch) && ch->wimpy == 69) ) && !IS_IMMORTAL(victim))
   {
      if (IS_AWAKE(victim))
      {
           affect_to_char(victim,&af);
	   send_to_char("The world turns black as you hit the floor.\n\r",victim);
	   do_sleep(victim,"");
	   set_delay(ch, victim);
      }	
      WAIT_STATE2(ch,skill_table[gsn_tko].beats);
      check_improve(ch,gsn_tko,TRUE,1);
    }
//*****************
   else if (number_percent() < chance )
   {
      	sprintf(buf, "Help! %s is trying to knock me out!",PERS(ch,victim));
      	j_yell(victim,buf);
      	act("$n knock you off-balance with $s punch!",ch,NULL,victim,TO_VICT);
	act("You knock $N off-balance with your punch!",ch,NULL,victim,TO_CHAR);
      	act("$n knocks $N off-balance with $s punch!",ch,NULL,victim,TO_NOTVICT);
	WAIT_STATE2(victim, URANGE(1, number_range(1,ch->size - victim->size + 3), 3) * PULSE_VIOLENCE);
	WAIT_STATE2(ch,skill_table[gsn_tko].beats);
      	damage(ch,victim,number_range(10 + ch->level/2,5 * ch->size + chance/10 + (2 * ch->level /3)),gsn_tko,DAM_BASH,TRUE);
	victim->position = POS_RESTING;
	check_improve(ch,gsn_tko,TRUE,1);
    }
    else
    {
       sprintf(buf, "Help! %s is trying to knock me out!",PERS(ch,victim));
       j_yell(victim,buf);
       damage(ch,victim,0,gsn_tko,DAM_BASH,TRUE);
       act("You fail to stun $N with a knock out.", ch,NULL,victim,TO_CHAR);
       act("$n fails to stun $N with a knock out.", ch,NULL,victim,TO_NOTVICT);
       act("$n triest to knock you out, but you stay on your feet.",ch,NULL,victim,TO_VICT);
       check_improve(ch,gsn_tko,FALSE,1);
       WAIT_STATE2(ch,skill_table[gsn_tko].beats * 3/2); 
    }
}

void do_create_torch( CHAR_DATA *ch, char *argument)
{
   OBJ_DATA *torch;
   int chance, sn = skill_lookup("create torch");
   if ( (chance = get_skill(ch,sn)) ==0)
   {
      send_to_char("You don't know how to create a torch.\n\r",ch);
      return;
   }
   if (ch->mana < 40)
   {
	send_to_char("Rest up first.\n\r",ch);
	return;
   }
   WAIT_STATE(ch,skill_table[sn].beats); 
   ch->mana -= 40;
   if (chance < number_percent())
   {
	send_to_char("You failed to make a torch.\n\r",ch);
	act("$n tries to make a torch, but fails.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,sn,FALSE,1);
	return;
   }
   torch = create_object( get_obj_index( OBJ_VNUM_TORCH ), 0);
   torch->timer = 48;
   obj_to_room( torch, ch->in_room );
   send_to_char("You gather some materials and create a torch.\n\r",ch);
   act("$n has created a torch!",ch,NULL,NULL,TO_ROOM);
   check_improve(ch,sn,TRUE,1);
}

void do_analyze( CHAR_DATA *ch, char *argument)
{   
    char arg[MIL];
    CHAR_DATA *victim;
    int chance, sn = skill_lookup("analyze");
    argument = one_argument(argument,arg);
    if ( (chance = get_skill(ch,sn)) ==0)
    {
      	send_to_char("You don't know how to analysis a person.\n\r",ch);
      	return;
    }
    if (ch->mana < 30)
    {
	send_to_char("Rest up first.\n\r",ch);
	return;
    }
    if (arg[0] == '\0')
    {
      	send_to_char("Who do you want to analyze?\n\r",ch);
      	return;   
    }
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
     	send_to_char("They're not here.\n\r",ch);
     	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("You can't do that while fighting.\n\r",ch);
	return;
    }
    if (victim->fighting != NULL)
    {
        sendf(ch, "%s won't hold still long enough.\n\r",PERS(victim,ch));
	return;
    }
    if (ch == victim)
    {
	send_to_char("What's the point of that?\n\r",ch);
	return;
    }
    if (is_safe(ch,victim))
	return;
    WAIT_STATE2(ch,skill_table[sn].beats); 
    ch->mana -= 30;
    act("$n peers intently at $N for a moment.", ch, NULL, victim, TO_NOTVICT);
    act("$n peers intently at you for a moment.", ch, NULL, victim, TO_VICT);

    if (chance < number_percent() || victim->level > ch->level || (IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG)))
    {
	sendf(ch,"The nature of %s eludes you.\n\r",PERS(victim,ch));
	check_improve(ch,sn,FALSE,1);
	return;
    }
    sendf(ch,"Name: %s   Rank: %d\n\r",PERS(victim,ch), victim->level);
    if (victim->imm_flags)
        sendf(ch, "Immune    : %s\n\r",imm_bit_name(victim->imm_flags));
    if (victim->res_flags)
        sendf(ch, "Resistant : %s\n\r", imm_bit_name(victim->res_flags));
    if (victim->vuln_flags)
        sendf(ch, "Vulnerable: %s\n\r", imm_bit_name(victim->vuln_flags));
    check_improve(ch,sn,TRUE,1);
}

void do_panhandle( CHAR_DATA *ch, char *argument)
{   
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    int chance, sn = skill_lookup("panhandle"), gold = 0;
    argument = one_argument(argument,arg);
    if ( (chance = get_skill(ch,sn)) ==0)
    {
      	send_to_char("Nobody sympathysis with you.\n\r",ch);
      	return;
    }
    if (ch->mana < 50)
    {
	send_to_char("Rest up first.\n\r",ch);
	return;
    }
    if (arg[0] == '\0')
    {
      	send_to_char("Who do you want to panhandle?\n\r",ch);
      	return;   
    }
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
     	send_to_char("They're not here.\n\r",ch);
     	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("You can't do that while fighting.\n\r",ch);
	return;
    }
    if (victim->fighting != NULL)
    {
        sendf(ch, "%s won't hold still long enough.\n\r",PERS(victim,ch));
	return;
    }
    if (IS_NPC(victim))
    {
	send_to_char("Mobs don't show any sympathey.\n\r",ch);
	return;
    }
    if (ch == victim)
    {
	send_to_char("What's the point of that?\n\r",ch);
	return;
    }
    if (is_safe(ch,victim))
	return;
    WAIT_STATE2(ch,skill_table[sn].beats); 
    ch->mana -= 50;
    chance = 2*chance/3;
    chance += ch->level - victim->level;
    chance += (get_curr_stat(ch,STAT_LUCK) - 17)*2;
    chance += get_curr_stat(ch,STAT_INT) - get_curr_stat(victim,STAT_INT);
    act("You beg $N for any spare change.",ch,NULL,victim,TO_CHAR);
    act("$n begs you for any spare change.",ch,NULL,victim,TO_VICT);
    act("$n begs $N for any spare change.",ch,NULL,victim,TO_NOTVICT);
    if (chance < number_percent() || victim->level > ch->level || victim->gold < 100 || IS_EVIL(victim))
    {
	act("$N does not show any signs of sympathy for you.",ch,NULL,victim,TO_CHAR);
	act("You ignore $n's begging.",ch,NULL,victim,TO_VICT);
	check_improve(ch,sn,FALSE,1);
	if (!IS_GOOD(victim))
	{
	    sprintf(buf,"I'm not about to give you any money %s!",PERS(ch,victim));
	    j_yell(victim,buf);
	    multi_hit(victim,ch,TYPE_UNDEFINED);
        }
	return;
    }
    act("$N falls victim to your pleading, and gives you some money.",ch,NULL,victim,TO_CHAR);
    act("You fall victim to $n's pleading, and feel obligated to donate some money.",ch,NULL,victim,TO_VICT);
    gold = victim->gold;
    gold = UMIN(1000,gold/100);
    victim->gold -= gold;
    ch->gold += gold;
    act("$N gives you some gold.",ch,NULL,victim,TO_CHAR);
    act("You give $n some gold.",ch,NULL,victim,TO_VICT);
    act("$N gives $n some gold.",ch,NULL,victim,TO_NOTVICT);
    check_improve(ch,sn,TRUE,1);
}

void do_attract( CHAR_DATA *ch, char *argument)
{   
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int chance, sn = gsn_attract;
    argument = one_argument(argument,arg);
    if ( (chance = get_skill(ch,sn)) ==0)
    {
      	send_to_char("You do not possess enough charm.\n\r",ch);
      	return;
    }
    if (ch->mana < 30)
    {
	send_to_char("Rest up first.\n\r",ch);
	return;
    }
    if (arg[0] == '\0')
    {
      	send_to_char("Who do you want to attract?\n\r",ch);
      	return;   
    }
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
     	send_to_char("They're not here.\n\r",ch);
     	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("You can't do that while fighting.\n\r",ch);
	return;
    }
    if (victim->fighting != NULL)
    {
        sendf(ch, "%s won't hold still long enough.\n\r",PERS(victim,ch));
	return;
    }
    if (IS_NPC(victim))
    {
	act("$N looks disinterested.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (ch == victim)
    {
	send_to_char("What's the point of that?\n\r",ch);
	return;
    }
    if (is_safe(ch,victim))
	return;
    ch->mana -= 30;
    chance = 2*chance/3;
    chance += ch->level - victim->level;
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK))*2;
    chance += (get_curr_stat(ch,STAT_INT) - get_curr_stat(victim,STAT_INT))*2;
    act("You put your charm on $N.",ch,NULL,victim,TO_CHAR);
    act("$n starts appealing to your temptations.",ch,NULL,victim,TO_VICT);
    act("$n puts out $s charm on $N.",ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE2(ch,skill_table[sn].beats); 
    if (chance < number_percent() || victim->level > ch->level 
	|| IS_GOOD(victim) || is_affected(victim,sn) || IS_UNDEAD(victim))
    {
	act("$N does not show any signs of temptation.",ch,NULL,victim,TO_CHAR);
	act("You ignore $n's temptations.",ch,NULL,victim,TO_VICT);
	check_improve(ch,sn,FALSE,1);
	sprintf(buf,"I'm not about to fall for that bullshit %s!",PERS(ch,victim));
	j_yell(victim,buf);
	multi_hit(victim,ch,TYPE_UNDEFINED);
	return;
    }
    act("$N gives in to your temptations, and follows you.",ch,NULL,victim,TO_CHAR);
    act("$N follows $m mindlessly.",ch,NULL,victim,TO_NOTVICT);
    act("You give in to $n's temptations, and follows $m mindlessly",ch,NULL,victim,TO_VICT);
    WAIT_STATE2(victim,24);
    stop_follower(victim);
    add_follower(victim,ch);
    af.where	= TO_AFFECTS;
    af.type   = sn;
    af.level  = ch->level;
    af.duration = 1;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char(victim,&af);
    check_improve(ch,sn,TRUE,1);
}

void do_taunt(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    CHAR_DATA *victim;
    int chance;   
    if ( (chance = get_skill(ch,gsn_taunt)) == 0)
    {
        send_to_char("You don't know how to taunt.\n\r",ch);
        return;
    }
    victim = ch->fighting;
    if (victim == NULL)
    {
        send_to_char("But you aren't fighting anyone!\n\r",ch);
        return;
    }
   if (is_affected(victim,gsn_taunt))
   {
	act("$N's has already been taunted.",ch,NULL,victim,TO_CHAR);
	return;
   }
    chance = 3 * chance /4;
    chance += (get_curr_stat(ch,STAT_STR) - get_curr_stat(victim, STAT_STR)) *2;
    chance += (get_curr_stat(ch,STAT_INT) - get_curr_stat(victim,STAT_INT)) *2;
    chance += (ch->level - victim->level);
    chance -= affect_by_size(ch,victim);
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    WAIT_STATE2(ch,skill_table[gsn_taunt].beats); 

    act("You throw a few verbal jabs at $N.", ch, NULL, victim, TO_CHAR);
    act("$n throws a few verbal jabs at you.", ch, NULL, victim, TO_VICT);
    act("$n throws a few verbal jabs at $N.", ch, NULL, victim, TO_NOTVICT);

    if (number_percent() < chance ){
      act("$N looks intimidated by your taunts.",ch,NULL,victim,TO_CHAR);
      act("You feel intimidated by $n's taunts.",ch,NULL,victim,TO_VICT);
      af.where	= TO_AFFECTS;
      af.type   = gsn_taunt;
      af.level  = ch->level;
      af.duration = 2;
      af.modifier = ch->level;
      af.location = APPLY_AC;
      af.bitvector = 0;
      affect_to_char(victim,&af);
      af.modifier = - UMIN(1,ch->level/12);
      af.location = APPLY_DAMROLL;
      affect_to_char(victim,&af);
      check_improve(ch,gsn_taunt,TRUE,1);
    }
}

void do_duet( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MIL];
    int sn = skill_lookup("duet");
    if (IS_NPC(ch))
	return;
    argument = one_argument(argument,arg);
    if (arg[0] == '\0')
    {
      	send_to_char("Who do you want to duet with?\n\r",ch);
      	return;   
    }
    if ( get_skill(ch,sn) ==0)
    {
      	send_to_char("You don't know how to sing a duet.\n\r",ch);
      	return;
    }
    if (!str_cmp(arg,"none"))
    {
	send_to_char("You currently have no one to duet with.\n\r",ch);
	ch->pcdata->duet = NULL;
	return;
    }
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
     	send_to_char("They're not here.\n\r",ch);
     	return;
    }
    if (ch == victim)
    {
	send_to_char("What is the point of that?\n\r",ch);
	return;
    }
    if (IS_NPC(victim))
    {
	act("$n gives you a wierd look.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( get_skill(victim,sn) ==0)
    {
      	act("$N doesn't know how to sing a duet.",ch,NULL,victim,TO_CHAR);
      	return;
    }
    if (victim->pcdata->duet != NULL && victim->pcdata->duet != ch)
    {
	act("$N has already chosen someone else to be $S duet partner.",ch,NULL,victim,TO_CHAR);
	return;
    }
    act("You choose $N to be your duet partner.",ch,NULL,victim,TO_CHAR);
    act("$n chooses you to be $s duet partner.",ch,NULL,victim,TO_VICT);
    ch->pcdata->duet = victim;
}

void do_uppercut( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int chance, success = number_percent();
    if ( (chance = get_skill(ch,gsn_uppercut)) ==0)
    {
        send_to_char("You don't know how to throw an uppercut.\n\r",ch);
        return;
    }
    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char("But you aren't fighting anyone!\n\r",ch);
        return;
    }
    chance = 4 * chance / 5;
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += (ch->level - victim->level);
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    chance += get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_DEX);
    chance += GET_AC2(victim,AC_BASH) /20;
    chance -= get_skill(victim,gsn_dodge)/20;
    if (victim->race == race_lookup("illithid") )
        chance -=10;
    act("You throw an uppercut aimed at $N's jaws.",ch,NULL,victim,TO_CHAR);
    if (IS_SET(victim->act, ACT_UNDEAD) 
	|| victim->class == class_lookup("vampire") 
	|| victim->class == class_lookup("lich")
	|| check_immune(ch, DAM_BASH, FALSE) == IS_IMMUNE)
      {
	act("$N seems to shake off the blow!", ch, NULL, victim, TO_NOTVICT);
	act("You easly shake off the blow.", ch, NULL, victim, TO_VICT);
	act("$N seems to not even notice the blow.", ch, NULL, victim, TO_ROOM);
      }
    else
      act("$n knocks $N back with a stunning uppercut.",ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE2(ch,skill_table[gsn_uppercut].beats);
    if (success > chance )
    {
        damage(ch,victim,0,gsn_uppercut,DAM_BASH,TRUE);
        check_improve(ch,gsn_uppercut,FALSE,1);
    }
    else
    {
      int shield = get_lagprot(victim);
      if (shield == LAG_SHIELD_PROT || shield == LAG_SHIELD_FFIELD)
	{
	  act("Your uppercut is stopped short by $N's shield.", ch, NULL, victim, TO_CHAR);
	  act("$n's uppercut bounces off of the magical shield!", ch, NULL, victim, TO_ROOM);
	  return;
	}
      damage(ch,victim,number_range(10 + ch->level/2,5 * ch->size + chance/10 + (2 * ch->level /3)),gsn_uppercut,DAM_BASH,TRUE);
      if (success < chance/2 && !is_affected(victim,gsn_uppercut))
	{
       	    act("$n dislocates your jaw with an stunning uppercut!",ch,NULL,victim,TO_VICT);
   	    act("You dislocate $N's jaw with a stunning uppercut!",ch,NULL,victim,TO_CHAR);
   	    act("$n dislocates $N's jaw with a stunning uppercut!",ch,NULL,victim,TO_NOTVICT);
   	    af.type		= gsn_uppercut;
   	    af.level		= ch->level;
   	    af.duration		= number_range(1, 2);
   	    af.location		= APPLY_NONE;
   	    af.modifier		= 0;
   	    af.bitvector	= 0;
   	    affect_to_char(victim,&af);
	}
	else if (success < 2*chance/3)
	{
	    act("You are stunned from the blow.",victim,NULL,NULL,TO_CHAR);
	    act("$n looks stunned from the blow.",victim,NULL,NULL,TO_ROOM);
	    WAIT_STATE2(victim,24);
	}
        check_improve(ch,gsn_uppercut,TRUE,1);
    }
}



void do_haymaker( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  AFFECT_DATA af;
//data
    int chance = 0;
    int success = 0;
    int level = 90 * ch->level / 100;
//const
    const int str_mod = 3;
    const int str_med = 18;
    const int str_ps_mod = 10;
    const int dex_mod = 1;
    const int lvl_mod = 1;
    const int luck_mod = 2;
    const int dodge_mod = 10;



    const int dispel_mod = 60;//chance for dispell to be checked.


//bool
    bool fRage = FALSE;
    bool fAdv = get_skill(ch, skill_lookup("skull crusher")) > 1;

    if ( (chance = get_skill(ch,gsn_haymaker)) ==0)
    {
        send_to_char("You don't know how to throw a good haymaker.\n\r",ch);
        return;
    }
    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char("But you aren't fighting anyone!\n\r",ch);
        return;
    }

    if (is_affected(ch, gsn_haymaker))
      {
	send_to_char("Your hand has not healed yet!\n\r", ch);
	return;
      }
//base skill chance.
    chance = 2*chance/3;
//dex
    chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX)) * dex_mod;
//level
    chance += (ch->level - victim->level) * lvl_mod;
//Luck
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK)) / luck_mod;
//Str
    chance += (get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_DEX)) * str_mod;
//ac
    chance += GET_AC2(victim,AC_BASH) /20;
//dodge
    chance -= get_skill(victim,gsn_dodge) / dodge_mod;
//Size
    chance += affect_by_size(ch, victim);

//reage    
    if (IS_AFFECTED2(ch, AFF_RAGE))
      {chance += 20;fRage = TRUE;}
//race things.
    if (victim->race == race_lookup("illithid"))
      chance -=20;

    act("You throw a mighty haymaker at $N's jaws.",ch,NULL,victim,TO_CHAR);
    act("$n throws a mighty haymaker at your jaws.",ch,NULL,victim,TO_VICT);
    WAIT_STATE2(ch,skill_table[gsn_haymaker].beats);
    if ( (success = number_percent()) > chance)
      {
        damage(ch,victim,0,gsn_haymaker,DAM_BASH,TRUE);
        check_improve(ch,gsn_haymaker,FALSE,1);
      }//end miss
    else
      {
	int shield = get_lagprot(victim);
	if (shield == LAG_SHIELD_PROT || shield == LAG_SHIELD_FFIELD)
	  {
	    //check for force of the blow.
	    if( (number_percent() < (get_curr_stat(ch, STAT_STR) - str_med) * str_ps_mod))
	      {
		act("Lacking the momentum neccessary you fail to penetrate $N's shield.", ch, NULL, victim, TO_CHAR);
		act("$n's haymaker stops short on the magical shield!", ch, NULL, victim, TO_ROOM);
		check_improve(ch,gsn_haymaker,FALSE,1);
		return;
	      }
	    else
	      {
		act("Striking with great force you penetrate $N's shield.", ch, NULL, victim, TO_CHAR);
		act("$n's haymaker goes right through the magical shield!", ch, NULL, victim, TO_ROOM);
	      }
	  }
//shield
	if( !fAdv && get_eq_char( victim, WEAR_SHIELD ) != NULL )
	  {
	    if (number_percent() < (8 * get_skill(victim, gsn_shield_block) / 10))
	      {
		act("$N blocks your punch with his shield and you curse as pain erupts in your fist.", ch, NULL, victim, TO_CHAR);
		act("You block $n's haymaker with your shield.", ch, NULL, victim, TO_VICT);
		act("$n curses in pain as $N blocks his haymaker with a shield.", ch, NULL, victim, TO_NOTVICT);
//we set the affect.
		af.type = gsn_haymaker;
		af.where = TO_AFFECTS2;
		af.level = ch->level;
		af.duration = number_fuzzy(2);
		af.location = APPLY_DEX;
		af.modifier = -2;
		af.bitvector = 0;
		affect_to_char(ch, &af);
		return;
	      }
	  }

	if (IS_UNDEAD(victim)
	    || victim->class == class_lookup("vampire") 
	    || victim->class == class_lookup("lich")
	    || check_immune(victim, DAM_BASH, FALSE) == IS_IMMUNE)
	  {
	    act("$N seems to shake off the blow!", ch, NULL, victim, TO_NOTVICT);
	    act("You easly shake off the blow.", ch, NULL, victim, TO_VICT);
	    act("$N seems to not even notice the blow.", ch, NULL, victim, TO_ROOM);
	    damage(ch,victim, 5 ,gsn_haymaker,DAM_BASH,TRUE);
	    return;
	  }
	else
	  act("$n sends $N reeling with a mighty haymaker.",ch,NULL,victim,TO_NOTVICT);	
          check_improve(ch,gsn_haymaker,TRUE,1);
    
	damage(ch,victim,number_range(10 + ch->level/2,5 * ch->size + chance/10 + (2 * ch->level /3)),gsn_haymaker,DAM_BASH,TRUE);
    //we dispell some.
	act("$N falls back, stunned by the mighty blow.", ch, NULL, victim, TO_CHAR);
	act("$N falls back, stunned by the mighty blow.", ch, NULL, victim, TO_NOTVICT);
	
	if ( (fRage ? success -30 : success) <= dispel_mod){
	  AFFECT_DATA* paf, *paf_next;
	  /* begin checking spells */
	  for (paf = victim->affected; paf; paf = paf_next){
	    paf_next = paf->next;
	    /* always skip to the last duplicate affect */
	    if (paf_next && paf_next->type == paf->type)
	      continue;
	    /* check if we can dispel */
	    if (!IS_GNBIT(paf->type, GN_INTERRUPT))
	      continue;

	    /* can dispel, check */
	    if (check_dispel(level, victim, paf->type)){
	    }
	  }
	}
      }//end success.
}
//////////////////
//embrace_chance//
/////////////////
//Calculates the chance for embrace to succeed.

int embrace_chance(CHAR_DATA* ch, CHAR_DATA* victim)
{
//Variables for to_hit calculation
  const int base_chance = 20;

//This is the point where you begin to get bonus for your skill.
  const int skill_med = 65;

//This is the stat multipliers.
//NOTICE: all teh multipliers are first perfromed on a statstic then added or 
//subtrt. to preven roundoff errors.  little more work for cpu but it should be
//little.
  const int str_mod = 2;//str
  const int dex_mod = 3;//dex
  const int night_mod = 10;//if the attack is made at night
  const int luck_mod = 1/5;//luck
  const int combat_mod = 10;//if attack is made on someone already in combat.
  const int haste_mod = 2;//this is the mult. of dex when hasted.
  const int slow_mod = 1/2;//mult of dex if slowed.
  const int lvl_mod = 2;
  const int size_mod = 2;

//Actual chance to .
  int chance  = base_chance;

//temporary holders for haste/slow modifed dex.
  int temp_dex_ch, temp_dex_vic;

//Current embrace skill.
  int embrace_skill = get_skill(ch,gsn_embrace);



//Now we start calculating success.

//-------------CHANCE TO BITE-----------------//

//First we check if victim is sleeping or ecstacied as that is instant hit.
if ( (is_affected(victim, skill_lookup("ecstacy"))) || !IS_AWAKE(victim) )
  chance = 100;
else
  {

//Next a quick check if the victim is in combat as it gives us a bonus.
      if (victim->fighting != NULL)
	chance+=combat_mod;

//Then a check for day or night, as it is another bonus/penalty.
      if(vamp_day_check(ch))
	chance-=night_mod;
      else
	chance+=night_mod;

//Now we calculate the str. bonus to hit.
      chance+= GET_HITROLL(ch)/2; 
      chance +=(get_curr_stat(ch, STAT_STR) * str_mod) - (get_curr_stat(victim, STAT_STR) * str_mod);

//Then the dex mod.
      temp_dex_ch = (get_curr_stat(ch, STAT_DEX));
      temp_dex_vic = (get_curr_stat(victim, STAT_DEX));
    
//check first for us being hasted (cant be slowed as then we cant attack)
      if (IS_AFFECTED(ch, AFF_HASTE) && !IS_AFFECTED(ch, AFF_SLOW))
	temp_dex_ch *= haste_mod;

//Then check for victim being hasted and not slowed.
      if (IS_AFFECTED(victim, AFF_HASTE) && !IS_AFFECTED(victim, AFF_SLOW))
	temp_dex_vic *= haste_mod;

//The check for victim being slowed and not hasted.
      if (!IS_AFFECTED(victim, AFF_HASTE) && IS_AFFECTED(victim, AFF_SLOW))
	temp_dex_vic *= slow_mod;

//FINALLY we can calculate Dex mod.
      chance +=(temp_dex_ch * dex_mod) - (temp_dex_vic * dex_mod);

//Now the luck.
      chance +=(get_curr_stat(ch, STAT_LUCK) * luck_mod - get_curr_stat(victim, STAT_LUCK) * luck_mod);

// And the size difference between us and the vicim (0tiny-5big)
     chance += affect_by_size(ch, victim) * size_mod;

 //Lastly we add bonus/penalty for skill.
      if (embrace_skill > skill_med)
	{
	  if (embrace_skill  < 80) chance+=(embrace_skill - skill_med)*0.5;
	  else if (embrace_skill <= 85) chance+=(embrace_skill - skill_med)* 1;
	  else if (embrace_skill <= 93) chance+=(embrace_skill - skill_med)* 1.5;
	  else if (embrace_skill <= 96) chance+=(embrace_skill - skill_med)* 2;
	  else if (embrace_skill <= 99) chance+=(embrace_skill - skill_med)* 2.5;
	  else if (embrace_skill ==100) chance+=(embrace_skill - skill_med)* 4;
	}
      else
	chance += (embrace_skill - skill_med);

//End FINALY a lvl bonus.
      chance += (ch->level - victim->level)* lvl_mod;

//finaly we cap the bite chances.
      chance = URANGE(5, chance, 95);
  }//End ELSE from check for sleep or ecstacy.

 return chance;
}//end embrace_chance


//////////////////
//embrace_poison//
//////////////////
//This is the function that actualy poisons
//It runs the check how strong the poisonis, and sets all the
//affects.  There is no save agaisnt this, if the bite hits
//successful save only lowers strength.

void embrace_poison(CHAR_DATA* ch, CHAR_DATA* victim)
{
//Affects data.
  AFFECT_DATA af;
//Modifiers
  const int con_mod = 4;//con
  const int lvl_mod = 3;//level
  const int svs_mod = 5;
        int base_dur = 18;

  int dur_mod = 100; //start strength

//E-Z cases
  if ( (victim == NULL) || is_affected(victim, gsn_embrace_poison) || is_affected(victim, gen_unlife) )
    return;

  if (is_affected(victim, gsn_ptrium)){
    send_to_char("Your bite seems to have no effect!\n\r", ch);
    act("$n's bite seems to have no effect!", ch, NULL, NULL, TO_ROOM);
    return;
  }

//Now we start caluclations:
dur_mod +=  (get_curr_stat(victim, STAT_CON) - 20) * con_mod;
dur_mod +=  (victim->level - ch->level) * lvl_mod;
//saves check, reduced strength by anywhere from 35-60%
if (saves_spell(spell_lvl_mod(ch, gsn_embrace), victim, DAM_MALED, SPELL_MALEDICTIVE)) dur_mod += 35 + number_percent() / svs_mod; 

//now that we got all the reductions.
//duration can be at most doubled or cut in half (10 - 32 ticks)
dur_mod = URANGE(50, dur_mod, 200);

//and now we get the real duration.
base_dur =(float) base_dur * dur_mod / 100;


//And now we apply the effect.
    af.type		= gsn_embrace_poison;
    af.where            = TO_NONE;
    af.level		= ch->level;
    af.duration		= base_dur;
    af.location		= APPLY_NONE;
//We store the original duration here.
    af.modifier		= base_dur;
    af.bitvector	= 0;
    affect_to_char(victim,&af);
  if (IS_AVATAR(ch))
    divfavor_gain(victim, DIVFAVOR_LOSS / 2);
}//end poison.



//////////////
//DO_EMBRACE//
//////////////
//Bites the victim an infects with unlife.

void do_embrace(CHAR_DATA *ch, char *argument)
{
//chance to embrace.
  int chance = 0;
//damage done
  int dam = 0;
//Target of embrace
  CHAR_DATA *victim;
//Affect data
  AFFECT_DATA af;
//Misc strings.
  char arg[MIL], buf[MSL];


//E-Z cases:

//First we check if the character has a skill.
  if (get_skill(ch,gsn_embrace) == 0)
    {
      send_to_char("Embrace??  Maybe you should try \"hug\" instead.\n\r",ch);
	return;
    }//END skill check.

//The interpretor handles the check for min position.
//So now we check for arguments.

//Extract the argument.
  one_argument( argument, arg );
  if (arg[0] == '\0')
  {
    send_to_char("Whom do you wish to bless with your dark gift?\n\r",ch);
    return;
  }

//End we check if the target is in the room.
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
      send_to_char("They aren't here.\n\r",ch);
        return;
    }

//Check if trying to bite self.
    if ( victim == ch )
      {
	act("You try to sink your fangs in your own neck, but you seem to be lacking the flexibility.\n\r",ch, NULL, NULL, TO_CHAR);
	return;
      }

//And we check if the victim is a vampire or has been infected.
  if (victim->class == class_lookup("vampire") || is_affected(victim, gsn_embrace_poison) || is_affected(victim, gen_unlife) )
  {
    act("$N is already blessed with the unlife.\n\r", ch, NULL, victim, TO_CHAR);
    return;
  }

//Check if not already bitten to preven hit and run.
  if (is_affected(victim, gsn_embrace))
  {
    act("$N's throat is still a bloody mess and he is far too cautious for another snack.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (IS_NPC(victim))
    {
      act("Surley $E is not worthy of such gift..", ch, NULL, victim, TO_CHAR);
      return;
    }

/*This should be handled in damage..
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
        return;
    }
*/


//Cant bite if you are slowed.
//but we do get  a bonus if target is slowed...
    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
    {
	send_to_char("You are moving far to slow for such dexterous act.\n\r",ch);
	return;
    }


//PK check
    if (is_safe(ch,victim))
      return;

    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "embrace", skill_table[gsn_embrace].name))
      return;

//Seems we can bite...
//Set lag.
    WAIT_STATE2( ch, skill_table[gsn_embrace].beats );
//And set the effect to preven run and hit tactics.
    af.type		= gsn_embrace;
    af.level		= ch->level;
    af.duration		= 3;
    af.location		= APPLY_NONE;
    af.modifier		= 0;
    af.bitvector	= 0;
    affect_to_char(victim,&af);

  if (number_percent() < (chance = embrace_chance(ch, victim)) )
    {
      if (number_percent() < 20)
	check_improve(ch,gsn_embrace,TRUE,-99);
      else
	check_improve(ch,gsn_embrace,TRUE,1);
//First we check if we can attack this person ahead of time
      if (get_spell_damage(ch, victim, 10, DAM_PIERCE) > 0)
	{
//Two messages, one for sleeping/ecstacied one for regular.
//if they were knocked out, then chance will be 100, otherwise its capped.
	  if (chance == 100)
	    {
	      act("Your victim helpless, you leisurely sink your fangs into $S throat.", ch, NULL, victim, TO_CHAR);
	      act("Taking advantage of $N's weakness, $n rips into $S throat.\n\rOh god the `!BLOOD``!.", ch, NULL, victim, TO_NOTVICT);

//If he is sleeping.
	      if (!IS_AWAKE(victim))
		send_to_char("An agonizing pain seers through your throat!", victim);
	      else //He must be ecstacied.
		act("Through your dreams of happiness you watch as $n sinks his fangs into your throat.", ch, NULL, victim, TO_VICT);
	    }//END if chance ==100 ( BITTEN IF SLEEP/ECSTASY)
	  else 
	    {
	      if (victim->fighting != NULL)
		//Fighting.
		{
		  act("Taking advantage of $N's lack of attention  you rip into $S neck.", ch, NULL, victim, TO_CHAR);
		  act("$N's attention focused on the fight, $n rips into $S neck sending blood spraying.", ch, NULL, victim, TO_NOTVICT);
		  act("Cursing your inattention you scream as $n tears your neck apart with $s fangs.", ch, NULL, victim, TO_VICT);
		}//End if fighting	
	      else 
		{

//Messages if we simply attacked. (IE: NOT sleeping, and NOT fighting.
		  act("You lock $N in an iron grip and rip into $S neck.", ch, NULL, victim, TO_CHAR);
		  act("With inhuman strength $n locks $N in an iron grip and bites into $S neck.", ch, NULL, victim, TO_NOTVICT);
		  act("You scream as $n rips your neck apart with $s fangs.", ch, NULL, victim, TO_VICT);
		}//end else (if fighting)
	    }//End else(if embraced)
	}//end if chech_dam > 0 (we end the messages.)
      
      //Scream for HIT
      sprintf(buf, "Die! %s you blood sucking vermin!",PERS(ch,victim));
// quick damage calc.
      dam = number_range(ch->level/2, ch->level*3/2);
      if (!vamp_day_check(ch))
        dam *=1.5;
      
      j_yell(victim,buf);
//Now we apply the poison effects
      if (get_spell_damage(ch, victim, dam, DAM_PIERCE) != -1)
      embrace_poison(ch, victim);
/* Need to apply the poison first so if he gets killed right away the */
/*effect is properly stripped.*/
      damage( ch, victim, dam, gsn_embrace,DAM_PIERCE,TRUE);
//and we feed a bit.
      gain_condition( ch, COND_HUNGER, dam/5 );
    }//End if Hit.
    else
      {
//He missed!
        act("$N notices your approach and manouvers out of your grasp.", ch, NULL, victim, TO_CHAR);
	act("$n strikes with with his fangs bared just as $N skill fully manouvers out of harms way", ch, NULL, victim, TO_NOTVICT);
	act("You notice $n's actions just in time to evade $s bite!", ch, NULL, victim, TO_VICT);

//Now we yell.
        sprintf(buf, "%s can't even suck properly!",PERS(ch,victim));
	a_yell(ch, victim);
        j_yell(victim,buf);
      	damage( ch, victim, 0, skill_lookup("embrace"),DAM_PIERCE,TRUE);
	gain_condition( ch, COND_HUNGER, - 10 );
      }//End if miss

//End we check improvement.
  if (number_percent() < 20)
    check_improve(ch,gsn_embrace,FALSE,-99);
  else
    check_improve(ch,gsn_embrace,FALSE,1);
}//End do_ embrace.

/*do_seep*/
void do_seep(CHAR_DATA *ch, char *argument)
{
  
//vampire mist form skill that works much like knock.
//direction passed in the argument.
  char arg[MIL];
  CHAR_DATA *gch;
  int door,chance = 0;
  one_argument( argument, arg );
  
//check for vamp mist form
  if (!is_affected(ch, gsn_mist_form))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

//check for proper arg.
  if ( arg[0] == '\0' )
    {
      send_to_char( "Seep where?\n\r", ch );
      return;
    }
  
//check for guardians
  for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 10 < gch->level )
      {
	sendf(ch, "%s is standing too close to the lock.\n\r", PERS(gch,ch));
	return;
      }//end if mob check.
  
//we give it same lag as mist form for efficiency's sake.
  WAIT_STATE2( ch, skill_table[gsn_mist_form].beats );
   
//get base chance
  chance = get_skill(ch,gsn_mist_form);
  
//now we do the seeping.
  if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
//LOCAL DECL.
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit = ch->in_room->exit[door], *pexit_rev;

      act( "Tendrils of $n extend towards the $d.", ch, NULL, pexit->keyword, TO_ROOM );
      if ( !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL(ch))
	{ send_to_char( "It's not closed.\n\r",        ch ); return; }
      if ( pexit->key < 0 && !IS_IMMORTAL(ch))
	{ send_to_char( "The lock doesn't seem to operate anymore.\n\r",ch ); return; }
      if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	{ send_to_char( "It's already unlocked.\n\r",  ch ); return; }
      if ( IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL(ch))
	{ send_to_char( "You seep into the lock but its beyond your efforts.\n\r",             ch ); return; }
      if ( !IS_SET(pexit->exit_info, EX_EASY) )
	chance = 3*chance/2;
      if ( !IS_SET(pexit->exit_info, EX_HARD) )
	chance = 2*chance/3;
      if ( !IS_SET(pexit->exit_info, EX_INFURIATING) )
	chance /= 2;    
      if ( !IS_NPC(ch) && chance < number_percent())
	{
	  send_to_char( "Your efforts fail.\n\r", ch);
	  act("$n seems to grow agitated momentarly.", ch, NULL, NULL, TO_ROOM);
	  check_improve(ch,gsn_mist_form,FALSE,2);
	  return;
	}//end if fialed.
      REMOVE_BIT(pexit->exit_info, EX_LOCKED);
      send_to_char( "You spring the mechanism with satisfying click.\n\r", ch );
      act( "You hear a silent *click* from the $d.", ch, NULL, pexit->keyword, TO_ROOM );
      check_improve(ch,gsn_mist_form,TRUE,2);
      if ( ( to_room = pexit->to_room ) != NULL && ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	   && pexit_rev->to_room == ch->in_room )
	REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
    }//end for
}//end do_seep


/* is_mcharged */
/* written: Viri 04-21-00 */
/* links to skills3.c, effects.c   */
/* used for checking objects for manacharge */
inline int is_mcharged(OBJ_DATA* obj)
{
  AFFECT_DATA* paf;
  /*
    checks the given for:
    1) Ability to be charged
    2) Current Charge.
    returns: -1 if not chargable
             0 if not charged.
	     or the amount of charge.
  */
  
//EZ for NULL obj.
  if (obj == NULL)
    return -1;

  /* Items that can be charged:
     - Any weapon. (staffs get a bonus)
     - Wands (bonus)
     - Staffs (non-weapon)
     - Treasure
  */
//check if this obj can be charged.
  if (get_maxmcharge(obj) == -1)
    return -1;

//Now we know the item is there and can be charged.
//check for gen.
if ( (paf = affect_find(obj->affected, gen_mana_charge)) == NULL)
  return 0;
else
  return UMAX(1, paf->modifier);
}//end is charged.

/* get_maxmcharge */
/* written: Viri 04-21-00 */
/* links to skills3.c     */
/* used calculate maximum charge for on object. */
inline int get_maxmcharge(OBJ_DATA* obj)
{
  /*
    returns:
    -1 if object cannot be charged.
    max charge otherwise
  */
  const int charge_wep = 400;
  const int charge_other = 800;
  const int charge_wand = 1000;
  const int charge_staff = 1200;

  if( (obj->pIndexData->item_type == ITEM_TREASURE)
      ||  (obj->pIndexData->item_type == ITEM_JEWELRY)
      ||  (obj->pIndexData->item_type == ITEM_GEM))
    return charge_other;
  if (obj->pIndexData->item_type == ITEM_WEAPON)
    {
  //we cehck if this is a staff.
    if(obj->value[0] == WEAPON_STAFF)
      return charge_staff;
    else
      return charge_wep;
    }
  if (obj->pIndexData->item_type == ITEM_STAFF)
    return charge_staff;

  if (obj->pIndexData->item_type == ITEM_WAND)
    return charge_wand;

 return -1;
}//end get_maxmcharge



/* mana_flare */
/* written: Viri 04-21-00 */
/* links to skills3.c     */
/* used for manacharge overflow damage */
inline int mana_flare(CHAR_DATA* ch, int mana)
{
  const int flare_mod = 2;
//does manaflare damage based on the mana overflowed.
if (is_ghost(ch, 600) ||( ch->level < 10) )
  {
    act("The gods take pity on you spare you the pain.\n\r", ch, NULL, NULL, TO_CHAR);
    act("$n's god protects $m from the explosion of arcane energy.\n\r", ch, NULL, NULL, TO_ROOM);
    return FALSE;
  }
  return damage(ch, ch, UMAX(1, mana/flare_mod), gen_mana_charge, DAM_NONE, TRUE);
}




/* do_mana_charge */
/* written: Viri 04-21-00 */
/* links to skills3.c      */
void do_mana_charge(CHAR_DATA *ch, char *argument)
{
  
//This is the main function for the mana charge invoker spell.
/* The skill allows a charachter to attach gen_manacharge to
   an item, allowing it to reduce casting cost and lag for spells.
   The syntax is: mana charge <item> <amount>
   <item> is any holdable object.
   <amount> is the amount of mana to put into the skill.
*/


  /* Items that can be charged:
     - Any weapon. (staffs get a bonus)
     - Wands (bonus)
     - Staffs (non-weapon)
     - Treasure
  */

//string buffers.
  char item[MSL];
  char amount[MSL];

  
//pointers
  OBJ_DATA* obj;
  AFFECT_DATA af;
  AFFECT_DATA* paf;

//data holders.
  int int_buf = 0;
  int charge_skill = 0;       // skill
  int charge_set = 0;         //requested charge
  int cur_charge = 0;         //Charge on item, (0 = not charged)
  int item_max = 0;           //max xharge on item.
  int fail = 0;               //chance to fail the process.

  //max charge a char can put through.
  int max_transcharge = 50 + (get_curr_stat(ch, STAT_INT) - 16) * 50 + number_percent();
  int flare_buf = 0;        //holds the amount of mana that flares.

  int lag_dur = 0;         //duration before use.


//Constants.
//increase of max_transcharge
  const int charge_per_skill = 25 + UMAX(0, get_curr_stat(ch, STAT_INT) - 20)* 4;
  const int trans_ratio = 3;         //ration of mana spent vs charged
  const int lag_ratio = 20;           //mana spent  vs lag before use
  const int normal_lag = 125;         //level for normal lag cost.
  const int half_lag = 250;           //level for half lag cost.


//EZ
  if (ch == NULL)
    return;
  
  if ((charge_skill = get_skill(ch, gsn_mana_charge)) == 0)
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

//get the args.
  argument =  one_argument(argument,item);
  one_argument(argument,amount);

  if (item[0] == '\0' || amount[0] == '\0')
    {
      send_to_char("Charge what with how much of a charge?\n\r", ch);
      return;
    }

//now we convert the args.
//Item first.
  if ((obj = get_obj_wear(ch, item, ch)) == NULL
      || (obj->wear_loc != WEAR_HOLD
	  && obj->wear_loc != WEAR_SECONDARY
	  && obj->wear_loc != WEAR_WIELD) )
    {
      send_to_char("You must be holding the item to be able to transfer the charge.\n\r", ch);
      return;
    }
  

//then the amount
  if ((charge_set = atoi(amount)) < 1)
    {
      send_to_char("Specify how much mana you wish to transfer.\n\r", ch);
      return;
  }
//check if not ready.
if (is_affected(ch, gsn_mana_charge))
  {
    send_to_char("You still not ready to attempt such feat of magic.\n\r", ch);
    return;
  }

//check for min mana.
if (charge_set > ch->mana)
  {
    send_to_char("You don't have the magic reserve neccesary.\n\r", ch);
    return;
  }

//The check if we can do this on the item.
if( (cur_charge = is_mcharged(obj)) == -1)
  {
    send_to_char("This item is not capable of holding such a potent arcane charge\n\r", ch);
    return;
  }

//check if character is not transfering more thenhe can.
//max transfer charge is: max_transcharge + ((skill % - 75%) x charge_per_skill
  max_transcharge += ( (charge_skill - 75) * charge_per_skill);



//DEBUG
  //sendf(ch, "`@damage: %s\n\ritem: %s, charge: %s charge(int): %d skill: %d, max: %d``\n\r",effect_table[gn_pos(gen_mana_charge)].noun_damage, item, amount, charge_set, charge_skill, max_transcharge);
//END DEBUG
  

  
  if ( (flare_buf = charge_set - max_transcharge) > 0 )
    {
      send_to_char("You begin the process but quickly sense the strain is too much and you abort.\n\r", ch);
      act("$n suddenly cries out in pain and clutches his temples!\n\r", ch, NULL, NULL, TO_ROOM);
      //we make him loose the stuff he couldnt handle..
      if (!IS_IMMORTAL(ch))
	ch->mana -= flare_buf;
      mana_flare(ch, flare_buf);
//We run the post spell update which will check if he should get burnt
      run_effect_update(ch, NULL, NULL, gen_mana_charge, NULL, NULL, TRUE, EFF_UPDATE_POSTSPELL);
      //Skill chceck
      if (number_percent() < 10 && charge_skill < 85)
	check_improve(ch, gsn_mana_charge, FALSE, -99);
      else
	check_improve(ch, gsn_mana_charge, FALSE, 1);
      return;
    }//end flare check.

  
//Now we know how much mana is being charged, and how much is on item.
  //We calculate the lag time first.
if (charge_set >= half_lag)
  {
    int_buf = charge_set - half_lag;
    lag_dur  =  int_buf / 2;
  }

if((charge_set - int_buf) >= normal_lag)
  {
    lag_dur +=  charge_set - int_buf - normal_lag;
    int_buf +=  charge_set - int_buf - normal_lag; 
  }
  
 lag_dur += 2 * (  charge_set - int_buf);

//finaly we cut the lag by the ratio.
 lag_dur /= lag_ratio;
 
//we take the mana
 if (!IS_IMMORTAL(ch))
   ch->mana-= charge_set;

 //DEBUG
 //lag_dur = 0;

//and lag the caster.
    af.where        = TO_AFFECTS;
    af.type         = gsn_mana_charge;
    af.level        = ch->level;
    af.duration     = lag_dur;
    af.location     = APPLY_NONE;
    af.modifier     = 0;
    af.bitvector    = 0;
//we apply the effect a bit later.

//now we check for success.

//fail is affected by skill and it rises sharply after 800.
    fail = ( (115 - charge_skill) * charge_set / UMIN(max_transcharge, 800) );


//we send out some actions that the caster is chargin the item.
    act("$n mutters ancient incanations as he begins drawing runes upon $p's surface.", ch, obj, NULL, TO_ROOM);
    act("You begin to weave the long and labourious enchantement onto $p", ch, obj, NULL, TO_CHAR);

    if (number_percent() < fail)
      {
	act("$n suddenly cries out in pain!", ch, NULL, NULL, TO_ROOM);
	act("Your control slips for but an instance and the searing flood enters your mind.", ch, NULL, NULL, TO_CHAR);
	af.duration /=3;
	affect_to_char(ch,&af);
	mana_flare(ch, charge_set);
//We run the post spell update which will check if he should get burnt
	run_effect_update(ch, NULL, NULL, gen_mana_charge, NULL, NULL, TRUE, EFF_UPDATE_POSTSPELL);

	if (number_percent() < 20 && charge_skill < 85)
	  check_improve(ch, gsn_mana_charge, FALSE, -99);
	else
	  check_improve(ch, gsn_mana_charge, FALSE, 8);
	return;
      }
//Succesful encahntement.
    charge_set /= trans_ratio;
    item_max = get_maxmcharge(obj);
//check for overfill
    if( (cur_charge + charge_set) > (item_max))
      {
	act("$p begins to shudder an emit a high pitch just as $n cries out in pain!", ch, obj, NULL, TO_ROOM);
	act("$p overflows with mana and the feedback begins to rend your brain apart.", ch, obj, NULL, TO_CHAR);
	mana_flare(ch, (charge_set + cur_charge - item_max) * trans_ratio);
      }
    affect_to_char(ch,&af);

    //DEBUG
    //    WAIT_STATE2( ch, skill_table[gsn_mana_charge].beats * PULSE_TICK );
    WAIT_STATE2( ch, skill_table[gsn_mana_charge].beats);

    cur_charge = UMIN(item_max, cur_charge + charge_set);
//safety
    if (cur_charge == -1)
      {
	bug("Error at maxcharge in do_manacharge\n\r", 0);
	return;
      }

//We attach the charge.
//DEBUHG
    //    sendf(ch, "`3charge: %d, lag: %d fail: %d result charge: %d``\n\r", charge_set, lag_dur, fail, cur_charge);
//End DEBUG
  if (!obj->enchanted)
    {
      AFFECT_DATA *af_new;
      obj->enchanted = TRUE;
      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) 
	{
	  af_new = new_affect();
	  af_new->next = obj->affected;
	  obj->affected = af_new;
	  af_new->where= paf->where;
	  af_new->type = UMAX(0,paf->type);
	  af_new->level= paf->level;
	  af_new->duration= paf->duration;
	  af_new->location= paf->location;
	  af_new->modifier= paf->modifier;
	  af_new->bitvector= paf->bitvector;
	}
    }


    //check if the charge exsts if so we dont do much.
    if ( (paf = affect_find(obj->affected, gen_mana_charge)) != NULL)
      paf->modifier = cur_charge;
    else
      {
	//we need a new affect.
	paf  = new_affect();
	paf->where = TO_OBJECT;
	paf->type = gen_mana_charge;
	paf->duration = -1;
	paf->modifier = cur_charge;
	paf->bitvector = ITEM_MAGIC;
	paf->level = ch->level;
	paf->location = APPLY_NONE;
	affect_to_obj(obj, paf);
	free_affect(paf);
      }
    //all done.
//We run the post spell update which will check if he should get burnt
    run_effect_update(ch, NULL, NULL, gen_mana_charge, NULL, NULL, TRUE, EFF_UPDATE_POSTSPELL);
	if (number_percent() < 10 && charge_skill < 85)
	  check_improve(ch, gsn_mana_charge, TRUE, -99);
	else
	  check_improve(ch, gsn_mana_charge, TRUE, 4);
  
}//end do_mana_charge


/* do_disguise */
/* links into skills3.c */
/* created by Viri */
void do_disguise(CHAR_DATA *ch, char *argument)
{
  //give the thief ability to change his title 
  //in both ranks and class.
  //argument is totaly ingnored.

  AFFECT_DATA af;
  char arg[MIL];

//flags
  bool is_changed = FALSE;//is name currently changed (gen_guise)
  bool is_used = FALSE;   //is the skill still in effect (gsn_disguise)
  bool fTurnOn = FALSE;


//consts
  const int base_chance = 30;
  const int hide_mod = 20;
  const int lvl_mod = 2;
  const int skill_mod = 1;
  const int skill_med = 50;//point at which skill bonus starts applying. 

 //variables.
  int chance = base_chance;
  int skill = 0;

//EZ cases.
  if (ch== NULL)
    return;

if ( (skill = get_skill(ch, gsn_disguise)) < 1)
  {
    send_to_char("But you don't have any makeup!\n\r", ch);
    return;
  }

//check for status,
  if (is_affected(ch, gsn_disguise))
    is_used = TRUE;
  if (is_affected(ch, gen_guise))
    is_changed = TRUE;

//now we say different things based on status.

  
//If skills has been used, and already turned off.
  if (is_used && !is_changed)
    {
      send_to_char("It is still to early to disguise yourself again, you are bound to fail.\n\r", ch);
      return;
    }

//If skill has been used and name is still changed, we want to turn it off.
  if (!is_used)
    fTurnOn  = TRUE;

//First we handle the fTurnOn :)
  if (fTurnOn)
    {
      int class = 0;
      /* check for proper class */
      argument = one_argument(argument, arg);
      if (IS_NULLSTR(arg)){
	send_to_char("Which class do you wish to disguse as?\n\r", ch);
	return;
      }
      class = class_lookup(arg);
  //thieves can only disguise into some clases.
      switch (class)
	{
	case 0:  break;//Warrior
	case 1:  break;//Berserker
	case 2:  break;//Ranger
	case 3:  break;//Paladin
	case 4:  break;//DK
	case 5:  break;//Thief
	case 6:  break;//Ninja
	case 7:  break;//Cleric
	case 8:  break;//Monk
	case 9:  break;//Invoker
	case 10:  break;//Bmg
	case 11:  break;//Necro
	case 13:  break;//Healer
	case 14:  break;//Shaman
	case 16:  break;//Bard
	default: 
	  send_to_char("You know too litle about that profession.\n\r", ch);
	  return;
    }//end case

//we strip existing name change if its there.
	if (is_changed)
	  affect_strip(ch, gen_guise);

//first we check if this is a custom title.
	if (has_custom_title(ch) || IS_IMMORTAL(ch))
	  {
	    send_to_char("Your fame precedes your name in the land, you have no hope of disguising your self.", ch);
	    return;
	  }

//We make a skill roll.
      if (IS_AFFECTED(ch, AFF_HIDE))
	chance += hide_mod;
      chance += ch->level / lvl_mod;
      chance += URANGE(0, (skill - skill_med) * skill_mod, 50);

//skill
      af.where = TO_AFFECTS;
      af.type = gsn_disguise;
      af.level = ch->level;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.duration  = 500 / (ch->level + 1);
      af.bitvector = 0;
      affect_to_char(ch,&af);
//Add lag.
      WAIT_STATE2( ch, skill_table[gsn_disguise].beats );

      if (number_percent() < chance)
	{
	  send_to_char("You begin to alter your appearance and behavior to match your new identity.\n\r", ch);
	  act("$n slowly begins to act very much unlike himself.", ch, NULL, NULL, TO_ROOM);

	  //effect
	    af.where = TO_AFFECTS;
	    af.type = gen_guise;
	    af.level = ch->level;
	    af.location = APPLY_NONE;
	    af.modifier = class;
	    af.duration  = ch->level/3;
	    af.bitvector = 0;
	    affect_to_char(ch,&af);
   //We check improv
	    check_improve(ch, gsn_disguise, TRUE, 1);
	}//end if success
      else
	send_to_char("You just can't quite get it right.\n\r", ch);
      	    check_improve(ch, gsn_disguise, FALSE, 1);
      return;
    }//end if fTurnOn


//Now we handle reminaing case where skill is turned on and name change is still there.
    affect_strip(ch, gen_guise);

}//end do_disgusie.


/* do_push */
/* links into skills3.c */
/* written by Viri */
void do_push(CHAR_DATA *ch, char *argument)
{
  //this is a forced flee for the target
  //may be done outside of combat.


  CHAR_DATA* victim;
  CHAR_DATA* vch;
//args
  char arg1[MIL];//target
  char arg2[MIL];//Direction.
  char buf[MIL];

 //consts
  const int skill_med = 50; //percent skill at which point we get benifit
  const int skill_mod = 2;//multiplire for skill above/bleow skill_med
  const int str_mod = 3;
  const int dex_mod = 2;
  const int lvl_mod = 2;
  const int luck_mod = 4;
  const int size_mod = 5;//note this is further increased by return value
  const int slow_mod = 30;
  const int haste_mod = 10;
  const int dodge_mod = 10;
  const int h2h_mod = 10;
  
  //data
  int shield = 0;
  int door = 0;//door in which we are pushing.
  int chance = 20;
  int dam = 5;
  int cost = skill_table[gsn_push].min_mana;

  //bool
  bool haste = FALSE;
  bool slow = FALSE;
  bool fInRoom = FALSE;//used to check if victim was able to move in pushed dir.
  bool fWall = FALSE;
 /*
   1) check target and dir.
   2) calculate success 
   3) push victim if success
   4) check if victim in room
   - if is attack
   - if not, follow to the target dir and attack.
 */

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
 
//EZ cases
  if (ch == NULL)
    return;
//check for skill
if (get_skill(ch, gsn_push) == 0)
  {
    send_to_char("Push? what's that?\n\r", ch);
    return;
  }

//check for args.
  if (arg1[0] == '\0')
    {
      send_to_char("Push whom, where?\n\r", ch);
      return;
    }

//check for proper target.
  if ((victim = get_char_room(ch, NULL, arg1)) == NULL)
    {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

//check for direction arg.
  if (arg2[0] == '\0')
    {
      sendf(ch, "Push %s in what direction?", victim->name);
      return;
    }
  //cehc for self
  if (ch == victim)
    {
      send_to_char("You sure you can't simply walk there?\n\r", ch);
      return;
    }
//check for valid direction.
    if      ( !str_cmp( arg2, "n" ) || !str_cmp( arg2, "north" ) ) door = 0;
    else if ( !str_cmp( arg2, "e" ) || !str_cmp( arg2, "east"  ) ) door = 1;
    else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "south" ) ) door = 2;
    else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "west"  ) ) door = 3;
    else if ( !str_cmp( arg2, "u" ) || !str_cmp( arg2, "up"    ) ) door = 4;
    else if ( !str_cmp( arg2, "d" ) || !str_cmp( arg2, "down"  ) ) door = 5;
    else
      door = -1;

    if ( (door < 0 || door > 5) || (ch->in_room->exit[door] == NULL) )
      fWall = TRUE;

    //PK check
    if (is_safe(ch,victim))
      return;

//MANA
  if (ch->mana < cost)
    {
      send_to_char("You can't concentrate sufficently to find a a good spot.\n\r", ch);
      return;
    }
  ch->mana -= cost;

//now we calculate the chance
//First few checks for haste/slow:
  if (IS_AFFECTED(ch, AFF_HASTE))
    haste = TRUE;
  if (IS_AFFECTED(ch, AFF_SLOW))
    slow = TRUE;
  if(haste && slow)
    haste = slow = FALSE;

  //modifiers
  chance += (get_skill(ch, gsn_push) - skill_med) * skill_mod;
  chance += get_skill(ch, gsn_hand_to_hand) / h2h_mod;
  chance += (get_curr_stat(ch, STAT_STR) - get_curr_stat(victim, STAT_DEX)) * str_mod;
  chance += (get_curr_stat(ch, STAT_DEX) - get_curr_stat(victim, STAT_STR)) * dex_mod;
  chance += (get_curr_stat(ch, STAT_LUCK) - get_curr_stat(victim, STAT_LUCK)) * luck_mod;
  chance += (ch->level - victim->level) * lvl_mod;
  chance += affect_by_size(ch, victim) * size_mod;
  chance -= get_skill(victim, gsn_dodge) / dodge_mod;

  if (haste)
    chance += haste_mod;
  if (slow)
    chance -= slow_mod;

//Quick check for mistform
if (is_affected(victim, gsn_mist_form))
  {
    send_to_char("Your attack passes right through!\n\r", ch);
    act("$n attempts to place you off balance with a strong push but he passes right through you.", ch, NULL, victim, TO_VICT);
    chance = 0;
  }

//Mob checks
 if (IS_NPC(victim)
     && !IS_AFFECTED(victim, AFF_CHARM)
     && (IS_SET(victim->act, ACT_TOO_BIG) 
	 || IS_SET(victim->act, ACT_PRACTICE)
	 || IS_SET(victim->act, ACT_TRAIN)
	 || IS_SET(victim->act, ACT_NOPURGE)
	 || IS_SET(victim->act, ACT_IS_HEALER)
	 || IS_SET(victim->act, ACT_GAIN)
	 || IS_SET(victim->act, ACT_AGGRESSIVE)
	 || ((victim->level - ch->level) >= 8)
	 || IS_SET(victim->act, ACT_NO_WANDER)
	 )
     )
   {
     act("$N won't even budge!", ch, NULL, victim, TO_CHAR);
     chance = 0;
   }
 else if (is_affected(victim, gen_watchtower)){
   sendf( ch, "%s is shielded within the Watchtower.\n\r", PERS(victim, ch ));
   chance = 0;
 }
 
    
//check for area move of bos
    if (!fWall && IS_NPC(victim) && !IS_AFFECTED(victim, AFF_CHARM) 
	&& victim->in_room->exit[door] != NULL)
      if ( victim->in_room->area->vnum != victim->in_room->exit[door]->to_room->area->vnum)
	{
	  act("$N won't even budge!", ch, NULL, victim, TO_CHAR);
	  chance = 0;
	}
    
    if ( ( shield = get_lagprot(victim)) == LAG_SHIELD_BALANCE
	 || shield == LAG_SHIELD_IWILL)
      {
	act("$N easly ignores your push and sends you tumbling.", ch, NULL, victim, TO_CHAR);
	act("You easly ignore $n's push and sends $m tumbling.", ch, NULL, victim, TO_VICT);
	chance = 0;
      }

    a_yell(ch, victim);
    //predict interdict
    if (predictCheck(ch, victim, "push", skill_table[gsn_push].name))
      return;


    //DEBUG
    //sendf(ch, "Chance to push: %d, Size dif: %d \n\r", chance,  affect_by_size(ch, victim));
//done we roll now. 
  if (number_percent() < chance)
    {
//succesful push!
      if (!fWall){
	WAIT_STATE2(ch,skill_table[gsn_push].beats/2);
	WAIT_STATE2(victim,skill_table[gsn_push].beats/2);
      }
      else{
	WAIT_STATE2(ch,skill_table[gsn_push].beats/2);
      }
//We move the victim first, then check if he is in the room, if he is we attack if not
//we move in the pushed direction.

      if (ch->master == victim)
	stop_follower(ch);

      move_char(victim, door, TRUE);
//Yell out
      if (ch->fighting != victim && victim->fighting != ch)
	{
	  sprintf(buf, "Help! %s just pushed me!",PERS(ch,victim));
	  j_yell(victim,buf);
	}
//no we check if he is still here or was he able to move?
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
	if (vch == victim)
	  fInRoom = TRUE;

//and we decide if he could move or not.
      if (fInRoom)
	{
	  //check for walls
	  if (fWall)
	    {
	      act("With a single well placed strike you send $N reeling into an obstruction.", ch, NULL, victim, TO_CHAR);
	      act("Using $N's weight and poisition against $M, $n sends $M reeling right into a nearby obstruction!", ch, NULL, victim, TO_NOTVICT);
	      act("Catching you off balance and unprepared $n sends you reeling towards an obstruction with a forcefull push!", ch, NULL, victim, TO_VICT);
	      dam *= URANGE(1,get_skill(ch, gsn_push) - 75, 18);
	    }
	  else
	    {
	      act("With a single well placed strike you sends $N tumbling onto the ground.", ch, NULL, victim, TO_CHAR);
	      act("Using $N's weight and poisition against $M, $n send $M sprawling.", ch, NULL, victim, TO_NOTVICT);
	      act("Catching you off balance and unprepared $n sends you sprawling with a forcefull push!", ch, NULL, victim, TO_VICT);
	    }//end not wall.
	  damage(ch, victim, dam, gsn_push, DAM_BASH, TRUE);
	}//end character pushed but couldnt move
      else
	{
	  damage(ch, victim, dam, gsn_push, DAM_BASH, TRUE);
	  act("With a single well placed strike you sends $N tumbling out of the area.", ch, NULL, victim, TO_CHAR);
	  act("Using $N's weight and poor balance against $M, $n sends $M sprawling out of the area.", ch, NULL, victim, TO_NOTVICT);
	  act("Catching you off balance and unprepared $n sends you sprawling with a forcefull push!", ch, NULL, victim, TO_VICT);
	  
//small change here, we stay in the room if we were succesful
//otherise this skill would be just too good :)
	  stop_fighting(victim, TRUE);
	}//end character pushed and moved
      check_improve(ch, gsn_push, TRUE, 1);
      return;
    }//end success push
  else
    {
      if (ch->fighting != victim && victim->fighting != ch)
	{
	  sprintf(buf, "Keep your hands to yourself %s!",PERS(ch,victim));
	  j_yell(victim,buf);
	}
      damage(ch, victim, 0, gsn_push, DAM_BASH, TRUE);
      if (!fWall)
	{
	  ROOM_INDEX_DATA* old_room;
	  send_to_char("You miscalculate your push and clumsily stumble out of the room.\n\r", ch);
	  act("$n tries to place you off balance but you easly redirect $s momentum and $e stumbles out.", ch, NULL, victim, TO_VICT);
	  //we make him visible and push him instead.
	      do_visible(ch, NULL);
	      old_room = ch->in_room;
	      move_char(ch, door, FALSE);
	      //we only stop fights if he did stumble out.
	      if (old_room != ch->in_room)
		stop_fighting(ch, TRUE);
	    }
	  else
	    do_visible(ch, NULL);
	  check_improve(ch, gsn_push, FALSE, 1);
	  return;
    }//edn fail
}//end do push



/* do_study */
/* only skills3.c */
/* Written by Viri */
void do_study( CHAR_DATA *ch, char *argument )
{
  //The skill is the study ability of ninjas that
  //gives bonus chance of success for assasinante.
  //all that the acutal skill does is set up the 
  //gen effect, after that all is done by gen.

  char arg[MIL];
  CHAR_DATA* victim;
  AFFECT_DATA* paf;
  AFFECT_DATA af;
  bool  fNewTar = FALSE;
  int range =  effect_table[GN_POS(gen_study)].value2[0];
  bool fFalcon = get_skill(ch, skill_lookup("falcon eye")) > 1;

  /* bonus to range by level */
  range += study_range_bonus(ch->level) + fFalcon ? 2  :0;

  /*
    1) validate the target
    2) check if its a new target.
    - set hunting if appropriate
    - reset gen if appropriate
  */

  one_argument(argument, arg);
  
//EZ
  if (ch == NULL)
    return;

 if (get_skill(ch, gsn_study) < 1)
    {
      act("Study? who wants to study?.\n\r", ch, NULL, NULL, TO_CHAR);
      return;
    }
 
 /* If gsn is set, we turn study off. (will start meditation on next tick.) */
 if (is_affected(ch, gsn_study)){
   send_to_char("You stop studying your target and prepare to meditate.\n\r", ch);
   affect_strip(ch, gsn_study);
   return;
 }

 if (arg[0]=='\0')
   {
     send_to_char("Study whom?\n\r", ch);
     return;
   }

 if ((victim = get_char_world(ch, arg)) == NULL)
   {
	send_to_char("You can't seem to sense them.\n\r",ch);
	return;
   }
 if (ch == victim)
   {
     send_to_char("Still same old self.\n\r", ch);
     return;
   }
 
 if (IS_NPC(victim))
   {
     act("Why would you waster your time on a mere mob?", ch, NULL, victim, TO_CHAR);
     return;
   }
 if (IS_IMMORTAL(victim) && !IS_IMMORTAL(ch)){
   send_to_char("Do you think yourself capable of understanding habits of gods?\n\r", ch);
   return;
 }

 //check mana 
 if (ch->mana < skill_table[gsn_study].min_mana)
   {
     send_to_char("You lack the mental strength required.\n\r", ch);
     return;
   }
 //we take the mana and set waitstate.
 ch->mana = UMAX(0, ch->mana - skill_table[gsn_study].min_mana);
 
 //we strip gsn
 if(is_affected(ch, gsn_study))
   affect_strip(ch, gsn_study);

/* blur affects range */
 if (is_affected(victim, skill_lookup("blur"))
     || IS_SET(race_table[victim->race].aff, AFF_SNEAK) )
   range = UMAX(1, range / 2);
 
 //reset the gsn.
 af.type = gsn_study;
 af.where = TO_AFFECTS;
 af.level = ch->level;
 af.modifier = 0;
 af.duration = 5;
 af.location = APPLY_NONE;
 af.bitvector = 0;
 affect_to_char(ch, &af);
 
/* unghost */
 ch->pcdata->ghost = (time_t)NULL;

 //check if there is no studies.
 if ( (paf = affect_find(ch->affected, gen_study)) == NULL )
   fNewTar = TRUE;
 
 //if there is a study, check if target is still same.
 if (!fNewTar)
   if (strcmp(paf->string, victim->name))
     fNewTar = TRUE;
 
 //DEBUG
 //  sendf(ch, "Previous target: %s, New Target:%s. \n\r", paf->string, victim->name);
 
//We do the new target first.
 if (fNewTar)
   //two cases, totaly new, or new target.
   {
     if (paf == NULL)//totaly new
       {
	 //set gen first.
	 af.type = gen_study;
	 af.where = TO_NONE;
	 af.level = 0;//no studies
	 af.modifier = 0;//no memorised info.
	 af.duration = -1;
	 af.location = APPLY_NONE;
	 af.bitvector = 0;
	 affect_to_char(ch, &af);
	 
	 //now we stick in the name of victim, as we have an address for the gen.
	 paf = affect_find(ch->affected, gen_study);
	 string_from_affect(paf);
	 string_to_affect(paf, victim->name);	
	 
	 
	 //send a message
	 sendf(ch, "You begin to study %s.\n\r(Current range %d rooms.)\n\r",
	       PERS(victim, ch), range);
       }//end if whole new gen.
     else //gen exists we reset and change name.
       {
	 //send a message before we loose previous name.
	 sendf(ch, "You forget about %s and begin to concentrate on %s.\n\r(Current range %d rooms.)\n\r", 
	       (paf->string == NULL ? "<@@@>" : paf->string),victim->name, range); 
	 
	 //reset the gen.
	 paf->level = 0;//reset studies
	 paf->modifier = 0;//reset total memorised
	 //free up the current string so we dont leak here.
	 string_from_affect(paf);
	 string_to_affect(paf, victim->name);	
       }//end new target, existing study
   }//end new target.
 else// continue study same target
   {
     //message
     act("Once again you focus your attention on $N's behaviour.", ch, NULL, victim, TO_CHAR);
      paf->level = 0; //reset study
      sendf(ch, "(Current range: %d rooms.)\n\r", range); 
   }//end study same target.
 return;
}//end do_study.




//03-25 11:20 Viri: Changed duration of gsn_covenant in do_darkritual
//from 6 to 12, and changed gsn_darkritual from 48 to 30.

//03-28 13:00 Viri: Added a dummy function for bite for now, do the reast later:)

//03-30 xx Viri: added do_embrace, and embrace_chance.

//03-31 14:00 Viri: added embrace_posion

//04-21 6:00 Viri: added mana charge, aman_flare, is_mcharged, get_mmaxcharge

//04-27-00 Viri: added do_disguise for thives

//04-27-00 Viri: Added do_push for ninjas.

//04-30-00 Viri: Added study for ninjas.
//05-05-00 Viri: Added Insect Swarm
//05-06-00 Viri: Modified Insect to be multi ranger friendly.
//05-08-00 Viri: Added haymayker.
