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
#include "song.h"
#include "cabal.h"
#include "recycle.h"
#include "interp.h"
#include "s_skill.h"
#include "jail.h"
#include "bounty.h"

void lagprot_yell(CHAR_DATA* ch, CHAR_DATA* victim, int gsn, int shield)
{
  /* this composes the yells for differnt shields. */
  char buf[MIL];

  switch (shield)
    {
      /* PROTECTIVE SHIELD */
    case LAG_SHIELD_PROT:
      sprintf(buf, "Don't even think about trying to use %s on me, %s!",
	IS_GEN(gsn)? effect_table[GN_POS(gsn)].name : skill_table[gsn].name,
	PERS(ch,victim));
      break;

      /* FORCE FIELD*/
    case LAG_SHIELD_FFIELD:
      sprintf(buf, "Your feeble %s cannot penetrate my forcefield %s!",
        IS_GEN(gsn)? effect_table[GN_POS(gsn)].name : skill_table[gsn].name,
        PERS(ch,victim));
      break;

      /* ARMOR */
    case LAG_SHIELD_ARMOR:
      sprintf(buf, "Your feeble %s cannot penetrate my armor %s!",
        IS_GEN(gsn)? effect_table[GN_POS(gsn)].name : skill_table[gsn].name,
        PERS(ch,victim));
      break;

      /* NON-CORPOREAL */
    case LAG_SHIELD_NONCORP:
      sprintf(buf, "You can't %s me %s you clumsy fool!",
        IS_GEN(gsn)? effect_table[GN_POS(gsn)].name : skill_table[gsn].name,
        PERS(ch,victim));
      break;

      /* BALANCE */
    case LAG_SHIELD_BALANCE:
      /* IRON WILL */
    case LAG_SHIELD_IWILL:
      sprintf(buf, "Your %s is no match for my abilities %s!",
        IS_GEN(gsn)? effect_table[GN_POS(gsn)].name : skill_table[gsn].name,
        PERS(ch,victim));
      break;

      /* DEFAULT */
    default:
      sprintf(buf, "Help! %s is trying to %s me!",
        PERS(ch,victim),
        IS_GEN(gsn)? effect_table[GN_POS(gsn)].name : skill_table[gsn].name);
    }//END SWITCH
    j_yell(victim,buf);
}


void lagprot_msg(CHAR_DATA* ch, CHAR_DATA* victim, int sn, int shield, bool fYell)
{
  /* does the different combat messages. */

  //3 message buffers to be displayed at the end.
  char to_char[MIL];
  char to_vict[MIL];
  char to_room[MIL];

  char* noun;
  char* verb_tch;
  char* verb_tvic;
  char* verb_trm;

  int verb_pos = lagattack_lookup(sn);

  if (verb_pos == -1)
    {
      bug("lagport_msg run with incorrect skill!", 0);
      return;
    }
  noun = lagattack_table[verb_pos].noun;
  verb_tch = lagattack_table[verb_pos].to_char;
  verb_tvic = lagattack_table[verb_pos].to_vict;
  verb_trm = lagattack_table[verb_pos].to_room;

  switch (shield)
    {
      /* PROTECTIVE SHIELD */
    case LAG_SHIELD_PROT:
      sprintf(to_char, "Your %s is ineffective as %s a protective shield.", noun, verb_tch);
      sprintf(to_vict, "$n's %s %s protective shield.",noun, verb_tvic);
      sprintf(to_room, "$n's %s %s $N's protective shield.",noun, verb_trm);
      break;

      /* FORCE FIELD*/
    case LAG_SHIELD_FFIELD:
      sprintf(to_char, "Your %s is ineffective as %s an elastic shield of force.", noun, verb_tch);
      sprintf(to_vict, "$n's %s %s force field.",noun, verb_tvic);
      sprintf(to_room, "$n's %s %s $N's force field.",noun, verb_trm);
      break;

      /* REFLECTIVE FIELD*/
    case LAG_SHIELD_RFIELD:
      sprintf(to_char, "Your %s is weakened as %s the reflective shield.", noun, verb_tch);
      sprintf(to_vict, "$n's %s %s reflective field.",noun, verb_tvic);
      sprintf(to_room, "$n's %s %s $N's reflective field.",noun, verb_trm);
      break;

      /* FORCE FIELD*/
    case LAG_SHIELD_ARMOR:
      sprintf(to_char, "Your %s is ineffective as %s the Juggernaut Plate.", noun, verb_tch);
      sprintf(to_vict, "$n's %s %s Juggernaut Plate.",noun, verb_tvic);
      sprintf(to_room, "$n's %s %s $N's Juggernaut Plate.",noun, verb_trm);
      break;

      /* NON-CORPOREAL */
    case LAG_SHIELD_NONCORP:
      sprintf(to_char, "Your %s is inneffective against $N.", noun);
      sprintf(to_vict, "$n's %s is usless against you.",noun);
      sprintf(to_room, "$N seems uneffected by $n's %s",noun);
      break;

      /* BALANCE */
    case LAG_SHIELD_BALANCE:
      /* IRON WILL */
    case LAG_SHIELD_IWILL:
      sprintf(to_char, "%s", lagattack_table[verb_pos].ba_char);
      sprintf(to_vict, "%s", lagattack_table[verb_pos].ba_vict);
      sprintf(to_room, "%s", lagattack_table[verb_pos].ba_room);
      break;
    }//END SWITCH

  //Upcase the first letter just in case.
  to_char[0] = UPPER(to_char[0]);
  to_vict[0] = UPPER(to_vict[0]);
  to_room[0] = UPPER(to_room[0]);

  //show messages
  act(to_char, ch, NULL, victim, TO_CHAR);
  act(to_vict, ch, NULL, victim, TO_VICT);
  act(to_room, ch, NULL, victim, TO_NOTVICT);

  if (fYell)
    lagprot_yell(ch, victim, sn, shield);
}//END


/* executes checks for lag protection */
/* returns type of shield that is on victim. */
int get_lagprot(CHAR_DATA* ch)
{
  int shield_type = LAG_SHIELD_NONE;

  //check if person has a shield first.
  if ( IS_AFFECTED2(ch, AFF_PROTECT_SHIELD) )
    shield_type = LAG_SHIELD_PROT;
  else if(is_affected(ch, gsn_forcefield))
    shield_type = LAG_SHIELD_FFIELD;
  else if(is_affected(ch, gen_ref_shield))
    shield_type = LAG_SHIELD_RFIELD;
  else if(is_armor_enhanced(ch) > ENHANCE_BASIC)
    shield_type = LAG_SHIELD_ARMOR;
  else if (is_affected(ch, gsn_iron_will)
     && ch->mana > 0)
    shield_type = LAG_SHIELD_IWILL;
  else if (is_affected(ch, gsn_mist_form))
    shield_type = LAG_SHIELD_NONCORP;
  else if (get_skill(ch, gsn_balance) > 1
	   && ch->class == gcn_monk
	   && is_empowered_quiet(ch,1)
	   && ch->mana > 0)
    shield_type = LAG_SHIELD_BALANCE;
  else if (get_skill(ch, gsn_balance) > 1
	   && ch->class == gcn_blademaster
	   && is_empowered_quiet(ch,1)
	   && ch->mana > 0)
    shield_type = LAG_SHIELD_BALANCE;
  return shield_type;
}

/* preforms ALL lagsshield checks, displays messges and yells */
/* set yell to TRUE to yell */
/* pass how good the bash attack was using chance */
/* Pass amount of lag by address as it is changed in the function */
/* pass damage by address as it is changed as well. */
/* pass if lag attack was successful or not by fSuccess (needed for skills that only work if hit like balance.)*/
   /* returns TRUE if cahracter was shielded. */

bool do_lagprot(CHAR_DATA* ch, CHAR_DATA* victim, int chance, int* do_dam, int* lag, int sn, bool fSuccess, bool fYell)
{
  int shield = LAG_SHIELD_NONE;
  int dam = *do_dam;
  int vic_lag = *lag;

  if ( (shield = get_lagprot(victim)) != LAG_SHIELD_NONE)
    {
      bool fProt = FALSE;
      /* BALANCE */
      if (shield == LAG_SHIELD_BALANCE && fSuccess)
	{
	  if (number_percent() < get_skill(victim, gsn_balance))
	    {
	      if (sn == gsn_bash)
		{
		  check_improve(victim,gsn_balance,TRUE,60);
		  victim->mana -= URANGE(0,10, victim->mana);
		}
	      else if (sn == gsn_bodyslam)
		{
		  check_improve(victim,gsn_balance,TRUE,70);
		  victim->mana -= URANGE(0,15,victim->mana);
		}
	      else if (sn == gsn_grapple)
		{
		  check_improve(victim,gsn_balance,TRUE,40);
		  victim->mana -= URANGE(0,7,victim->mana);
		}
	      else if (sn == gsn_thrust)
		{
		  check_improve(victim,gsn_balance,TRUE,90);
		  victim->mana -= URANGE(0,18,victim->mana);
		}

	      else
		{
		  check_improve(victim,gsn_balance,TRUE,50);
		  victim->mana -= URANGE(0,10,victim->mana);
		}
	      fProt = TRUE;
	      vic_lag = 0;
	    }
	}//END BALANCE
      /* IRON WILL */
      else if (shield == LAG_SHIELD_IWILL && fSuccess)
	{
	  if (number_percent() < 100 + (victim->level - ch->level) * 5)
	    {
	      if (sn == gsn_bash)
		victim->mana -= URANGE(0,20,victim->mana);
	      else if (sn == gsn_bodyslam)
		victim->mana -= URANGE(0,30,victim->mana);
	      else if (sn == gsn_grapple)
		victim->mana -= URANGE(0,15,victim->mana);
	      else if (sn == gsn_thrust)
		victim->mana -= URANGE(0,40,victim->mana);
	      else
		victim->mana -= URANGE(0,20,victim->mana);

	      fProt = TRUE;
	      vic_lag = 0;
	    }
	}//END IRON WILL
      /* NORMAL SHIELDS */
      else if (shield == LAG_SHIELD_ARMOR)
	{
	  if (number_percent() < 25){
	    fProt = TRUE;
	    vic_lag = 0;
	    dam = 0;
	  }
	}
      else if (shield == LAG_SHIELD_RFIELD)
	{
	  fProt = TRUE;
          vic_lag = UMAX(PULSE_VIOLENCE, 2 * vic_lag / 3);
	  dam = dam / 2;
	}
      else if (shield == LAG_SHIELD_FFIELD)
	{
	  fProt = TRUE;
          vic_lag = 0;
	  dam = 0;
	}
      else if (shield == LAG_SHIELD_PROT)
	{
	  fProt = TRUE;
          vic_lag = 0;
	  dam = 0;
	}
      if (fProt)
	lagprot_msg(ch, victim, sn, shield, fYell);
      *lag = vic_lag;
      *do_dam = dam;
      return fProt;
    }//END LAGPROTECTION
  return FALSE;
}//end do_lagprot


void a_yell( CHAR_DATA *ch, CHAR_DATA *victim )
{
  char buf[MSL];
  if (ch == NULL || victim == NULL)
    return;
  if (ch->fighting != victim && victim->fighting != ch && (!IS_NPC(ch) || ch->master != NULL)){
    if (!IS_NPC(victim)){
      sprintf(buf,"%s attacked %s at %d", IS_NPC(ch) ? ch->short_descr : ch->name,
	      victim->name,ch->in_room->vnum);
      log_string(buf);
      wiznet(buf,NULL,NULL,WIZ_FIGHTING,0,0);
      continuum(victim, CONTINUUM_ATTACK);

/* MURDER CRIME CHECK */
      if (ch->in_room && is_crime(ch->in_room, CRIME_MURDER, victim)){
	set_crime(ch, victim, ch->in_room->area, CRIME_MURDER);

	/* check of simulacra for justices */
	if (is_affected(ch, gsn_doppelganger)){
	  char buf[MIL];
	  sprintf( buf, "Warning! Someone seems to be masquerading as %s!",
		   PERS2(ch));
	  cabal_echo_flag( CABAL_JUSTICE | CABAL_HEARGUARD, buf );
	}
      }
/* ATTACKING JUSTICE IN LAWFUL AREA IS ALWAYS AN OBSTRUCTION */
      else if (victim->pCabal && IS_CABAL(victim->pCabal, CABAL_JUSTICE) && IS_AREA(ch->in_room->area, AREA_LAWFUL)){
	set_crime(ch, victim, ch->in_room->area, CRIME_OBSTRUCT);
      }
      /* CONTINENCY */
      check_contingency(ch, NULL, CONT_ATTACK);
      check_contingency(victim, NULL, CONT_ATTACKED);

    }//END PC CHECK
    else {
      bool fCrime = FALSE;
/* ASSAULT CRIME CHECK */
      if (ch->in_room && is_crime(ch->in_room, CRIME_ASSAULT, victim)){
	set_crime(ch, victim, ch->in_room->area, CRIME_ASSAULT);
	fCrime = TRUE;
      }
/* ATTACKING JUSTICE MOBS IN LAWFUL AREA IS ALWYAS A CRIME */
      else if (victim->pCabal && IS_CABAL(victim->pCabal, CABAL_JUSTICE) && IS_AREA(ch->in_room->area, AREA_LAWFUL)){
	set_crime(ch, victim, ch->in_room->area, CRIME_OBSTRUCT);
	fCrime = TRUE;
      }
/* attackign charmed mobs requiers a yell as well */
      else if (IS_NPC(victim) && IS_AFFECTED(victim, AFF_CHARM))
	fCrime = TRUE;

      if (IS_SET(victim->off_flags,GUILD_GUARD) && victim->pCabal && IS_CABAL(victim->pCabal, CABAL_JUSTICE)){
	sprintf(buf, "%s: Alert! The jail is being attacked!", ch->in_room->area->name);
	cabal_echo_flag( CABAL_JUSTICE, buf );
	sprintf(buf, "Alert! The jail is being attacked by %s!",PERS(ch,victim));
      }
      else if (IS_SET(victim->off_flags,GUILD_GUARD))
	sprintf(buf, "Alert! The guild is being attacked by %s!",PERS(ch,victim));
      else
	sprintf(buf, "Help! I'm being attacked by %s!",PERS(ch,victim));

      //mark the cabal capture item timer if this is a cabal guard and members present
      if (victim->pCabal)
	set_capture_item_timer( victim );
      if (fCrime){
	REMOVE_BIT(victim->comm,COMM_NOCHANNELS);
	REMOVE_BIT(victim->comm,COMM_NOYELL);
	j_yell(victim,buf);
	SET_BIT(victim->comm,COMM_NOYELL);
	SET_BIT(victim->comm,COMM_NOCHANNELS);
      }
    }//END NPC CHECK
  }
}

void do_gain(CHAR_DATA *ch, char *argument)
{
    char arg[MIL];
    CHAR_DATA *trainer;
    if (IS_NPC(ch))
	return;
    if (ch->class == gcn_adventurer){
      send_to_char("Adventurers cannot convert train and practice sessions.\n\r", ch);
      return;
    }
    for ( trainer = ch->in_room->people; trainer != NULL; trainer = trainer->next_in_room)
	if (IS_NPC(trainer) && IS_SET(trainer->act,ACT_GAIN))
	    break;
    if (trainer == NULL || !can_see(ch,trainer))
    {
	send_to_char("You can't do that here.\n\r",ch);
	return;
    }
    if (is_exiled(ch, trainer->in_room->area->vnum )){
      char buf[MIL];
      sprintf( buf, "No one in %s will deal with you %s!", trainer->in_room->area->name, PERS2(ch));
      do_say( trainer, buf );
      return;
    }
    one_argument(argument,arg);
    if (arg[0] == '\0')
    {
	do_say(trainer,"Pardon me?");
	return;
    }
    if (!str_prefix(arg,"convert"))
    {
	if (ch->practice < 10)
	{
            act("$N tells you '`2You are not yet ready.``'", ch,NULL,trainer,TO_CHAR);
	    return;
	}
        act("$N helps you apply your practice to training.", ch,NULL,trainer,TO_CHAR);
	ch->practice -= 10;
	ch->train +=1 ;
	return;
    }
    if (!str_prefix(arg,"study"))
    {
	if (ch->train < 1)
	{
            act("$N tells you '`2You do not have enough training sessions.``'", ch,NULL,trainer,TO_CHAR);
	    return;
	}
        act("$N helps you apply your training to practices.", ch,NULL,trainer,TO_CHAR);
	ch->practice += 10;
	ch->train -=1 ;
	return;
    }
    act("$N tells you '`2I do not understand...``'",ch,NULL,trainer,TO_CHAR);
}

void do_spells(CHAR_DATA *ch, char *argument)
{
    char arg[MIL], spell_list[LEVEL_IMMORTAL + 2][MSL], spell_columns[LEVEL_IMMORTAL + 2], buf[MSL];
    int sn, level, sk_level, min_lev = 1, max_lev = LEVEL_HERO+1, mana;
    bool fAll = FALSE, found = FALSE;
    CSKILL_DATA* cSkill;

    /* Useless conditional */
    if( fAll != FALSE )
        fAll = FALSE;

    if (IS_NPC(ch))
        return;
    if (argument[0] != '\0')
    {
	fAll = TRUE;
	if (str_prefix(argument,"all"))
	{
	    argument = one_argument(argument,arg);
	    if (!is_number(arg))
	    {
		send_to_char("Arguments must be numerical or all.\n\r",ch);
		return;
	    }
	    max_lev = atoi(arg);
            if (max_lev < 1 || max_lev > LEVEL_HERO)
	    {
                sendf(ch,"Levels must be between 1 and %d.\n\r",LEVEL_HERO);
		return;
	    }
	    if (argument[0] != '\0')
	    {
		argument = one_argument(argument,arg);
		if (!is_number(arg))
		{
		    send_to_char("Arguments must be numerical or all.\n\r",ch);
		    return;
		}
		min_lev = max_lev;
		max_lev = atoi(arg);
                if (max_lev < 1 || max_lev > LEVEL_HERO)
		{
                    sendf(ch,"Levels must be between 1 and %d.\n\r",LEVEL_HERO);
		    return;
		}
		if (min_lev > max_lev)
		{
		    send_to_char("That would be silly.\n\r",ch);
		    return;
		}
	    }
	}
    }
    for (level = 0; level < LEVEL_IMMORTAL + 2; level++)
    {
        spell_columns[level] = 0;
        spell_list[level][0] = '\0';
    }
    for (sn = 0; sn < MAX_SKILL; sn++)
    {
        if (skill_table[sn].name == NULL )
	    break;
	cSkill = ch->pCabal ? get_cskill( ch->pCabal, sn ) : NULL;
	if (cSkill){
	  level = cSkill->pSkill->level;
	  sk_level = sklevel(ch,sn);
	}
	else
	  level = sk_level = sklevel(ch,sn);

	if ( (sk_level < LEVEL_IMMORTAL + 2 || sk_level == CABAL_COST_FAILED)
	     && level >= min_lev
	     && level <= max_lev
	     && skill_table[sn].spell_fun != spell_null
	     && ch->pcdata->learned[sn] > 0)
	  {
	    mana = _mana_cost(ch, sn);
	    found = TRUE;

	    if (ch->level < sk_level){
	      if (sk_level == CABAL_COST_FAILED)
	        sprintf(buf,"%-16s <`!%3d/%d``> m/cp  ",skill_table[sn].name, mana, get_cskill_cost(ch, ch->in_room, cSkill));
	      else
	    	sprintf(buf,"%-18s  n/a       ", skill_table[sn].name);
	    }
	    else
	    {
	      if (cSkill)
		sprintf(buf,"%-17s %3d/%d m/cp  ",skill_table[sn].name, mana, get_cskill_cost(ch, ch->in_room, cSkill));
	      else
		sprintf(buf,"%-19s %3d mana  ",skill_table[sn].name,mana);
	    }
	    if (spell_list[level][0] == '\0')
	      sprintf(spell_list[level],"\n\rLevel %2d: %s",level,buf);
            else
	      {
          	if ( ++spell_columns[level] % 2 == 0)
		  strcat(spell_list[level],"\n\r          ");
          	strcat(spell_list[level],buf);
	      }
	  }
    }
    if (!IS_NPC(ch) && ch->pcdata->newskills != NULL )
    {
	SKILL_DATA *nsk;
	for (nsk = ch->pcdata->newskills; nsk != NULL; nsk = nsk->next )
	{
	  if ((level=nsk->level) < LEVEL_IMMORTAL + 2 && level >= min_lev && level <= max_lev
	      &&skill_table[nsk->sn].spell_fun != spell_null)
	  {
	    found = TRUE;
	    if (ch->level < nsk->level)
	    	sprintf(buf,"%-18s  n/a       ", skill_table[nsk->sn].name);
	    else
	    {
                mana = UMAX(skill_table[nsk->sn].min_mana, 100/(2 + ch->level - nsk->level));
	        sprintf(buf,"%-19s %3d mana  ",skill_table[nsk->sn].name,mana);
	    }
	    if (spell_list[level][0] == '\0')
          	sprintf(spell_list[level],"\n\rLevel %2d: %s",level,buf);
            else
	    {
          	if ( ++spell_columns[level] % 2 == 0)
		    strcat(spell_list[level],"\n\r          ");
          	strcat(spell_list[level],buf);
	    }
	  }
	}
    }
    if (!found)
    {
      	send_to_char("No spells found.\n\r",ch);
      	return;
    }
    for (level = 0; level < LEVEL_IMMORTAL + 2; level++)
      	if (spell_list[level][0] != '\0')
            send_to_char(spell_list[level],ch);
    send_to_char("\n\r",ch);
}

void do_songs(CHAR_DATA *ch, char *argument)
{
    char arg[MIL], spell_list[LEVEL_IMMORTAL + 2][MSL], spell_columns[LEVEL_IMMORTAL + 2], buf[MSL];
    int sn, level, min_lev = 1, max_lev = LEVEL_HERO+1, mana;
    bool fAll = FALSE, found = FALSE;

    /* Useless conditional */
    if( fAll != FALSE )
        fAll = FALSE;

    if (IS_NPC(ch))
        return;
    if (argument[0] != '\0')
    {
	fAll = TRUE;
	if (str_prefix(argument,"all"))
	{
	    argument = one_argument(argument,arg);
	    if (!is_number(arg))
	    {
		send_to_char("Arguments must be numerical or all.\n\r",ch);
		return;
	    }
	    max_lev = atoi(arg);
            if (max_lev < 1 || max_lev > LEVEL_IMMORTAL + (IS_IMMORTAL(ch) ? 1 : (-1)))
	    {
                sendf(ch,"Levels must be between 1 and %d.\n\r",LEVEL_IMMORTAL+(IS_IMMORTAL(ch) ? 1 : (-1)));
		return;
	    }
	    if (argument[0] != '\0')
	    {
		argument = one_argument(argument,arg);
		if (!is_number(arg))
		{
		    send_to_char("Arguments must be numerical or all.\n\r",ch);
		    return;
		}
		min_lev = max_lev;
		max_lev = atoi(arg);
                if (max_lev < 1 || max_lev > LEVEL_IMMORTAL + (IS_IMMORTAL(ch) ? 1 : (-1)))
		{
                    sendf(ch,"Levels must be between 1 and %d.\n\r",LEVEL_IMMORTAL+(IS_IMMORTAL(ch) ? 1 : (-1)));
		    return;
		}
		if (min_lev > max_lev)
		{
		    send_to_char("That would be silly.\n\r",ch);
		    return;
		}
	    }
	}
    }
    for (level = 0; level < LEVEL_IMMORTAL + 2; level++)
    {
        spell_columns[level] = 0;
        spell_list[level][0] = '\0';
    }
    for (sn = 0; sn < MAX_SONG; sn++)
    {
        if (song_table[sn].name == NULL )
	    break;
	if ((level=sglevel(ch,sn)) < LEVEL_IMMORTAL + 2 && level >= min_lev && level <= max_lev
        && song_table[sn].song_fun != song_null && ch->pcdata->songlearned[sn] > 0)
        {
	    found = TRUE;
	    if (ch->level < sglevel(ch,sn))
	    	sprintf(buf,"%-23s  n/a       ", song_table[sn].name);
	    else
	    {
                mana = UMAX(song_table[sn].min_mana, 100/(2 + ch->level - sglevel(ch,sn)));
	        sprintf(buf,"%-24s %3d mana  ",song_table[sn].name,mana);
	    }
	    if (spell_list[level][0] == '\0')
          	sprintf(spell_list[level],"\n\rLevel %2d: %s",level,buf);
            else
	    {
          	if ( ++spell_columns[level] % 2 == 0)
		    strcat(spell_list[level],"\n\r          ");
          	strcat(spell_list[level],buf);
	    }
	}
    }
    if (!found)
    {
      	send_to_char("No songs found.\n\r",ch);
      	return;
    }
    for (level = 0; level < LEVEL_IMMORTAL + 2; level++)
      	if (spell_list[level][0] != '\0')
            send_to_char(spell_list[level],ch);
    send_to_char("\n\r",ch);
}

void do_skills(CHAR_DATA *ch, char *argument)
{
    char arg[MIL], skill_list[LEVEL_IMMORTAL + 2][MSL], skill_columns[LEVEL_IMMORTAL + 2], buf[MSL];
    int sn, level, sk_level, min_lev = 1, max_lev = LEVEL_HERO+1;
    bool fAll = FALSE, found = FALSE;
    CSKILL_DATA* cSkill;

    /* Useless conditional */
    if( fAll != FALSE )
        fAll = FALSE;

    if (IS_NPC(ch))
        return;
    if (argument[0] != '\0')
    {
	fAll = TRUE;
	if (str_prefix(argument,"all"))
	{
	    argument = one_argument(argument,arg);
	    if (!is_number(arg))
	    {
		send_to_char("Arguments must be numerical or all.\n\r",ch);
		return;
	    }
	    max_lev = atoi(arg);
            if (max_lev < 1 || max_lev > LEVEL_HERO)
	    {
                sendf(ch,"Levels must be between 1 and %d.\n\r", LEVEL_HERO);
		return;
	    }
	    if (argument[0] != '\0')
	    {
		argument = one_argument(argument,arg);
		if (!is_number(arg))
		{
		    send_to_char("Arguments must be numerical or all.\n\r",ch);
		    return;
		}
		min_lev = max_lev;
		max_lev = atoi(arg);
                if (max_lev < 1 || max_lev > LEVEL_HERO)
		{
                    sendf(ch,"Levels must be between 1 and %d.\n\r",LEVEL_HERO);
		    return;
		}
		if (min_lev > max_lev)
		{
		    send_to_char("That would be silly.\n\r",ch);
		    return;
		}
	    }
	}
    }
    for (level = 0; level < LEVEL_IMMORTAL + 2; level++)
    {
        skill_columns[level] = 0;
        skill_list[level][0] = '\0';
    }
    for (sn = 0; sn < MAX_SKILL; sn++)
    {
        if (skill_table[sn].name == NULL )
	    break;

	cSkill = ch->pCabal ? get_cskill( ch->pCabal, sn ) : NULL;
	if (cSkill){
	  level = cSkill->pSkill->level;
	  sk_level = sklevel(ch,sn);
	}
	else
	  level = sk_level = sklevel(ch,sn);

	if ( (sk_level < LEVEL_IMMORTAL + 2 || sk_level == CABAL_COST_FAILED)
	     && level >= min_lev
	     && level <= max_lev
	     && skill_table[sn].spell_fun == spell_null
	     && ch->pcdata->learned[sn] > 0)
	  {
	    found = TRUE;
	    if (ch->level < sk_level){
	      if (sk_level == CABAL_COST_FAILED){
		sprintf(buf,"%-17s <`!%3d``>      ",skill_table[sn].name,
			URANGE(0, ch->pcdata->learned[sn] + ch->pcdata->to_learned[sn], 110));
	      }
	      else
		sprintf(buf,"%-18s  n/a      ", skill_table[sn].name);
	    }
	    else if (ch->pcdata->to_learned[sn] != 0)
	      sprintf(buf,"%-18s `%s%3d``      ",
		      skill_table[sn].name,
		      ch->pcdata->to_learned[sn] > 0 ? "@" : "1",
		      URANGE(0, ch->pcdata->learned[sn] + ch->pcdata->to_learned[sn], 110));
	    else
	      sprintf(buf,"%-18s %3d      ",skill_table[sn].name,
		      URANGE(0, ch->pcdata->learned[sn] + ch->pcdata->to_learned[sn], 110));
	    if (skill_list[level][0] == '\0')
          	sprintf(skill_list[level],"\n\rLevel %2d: %s",level,buf);
            else
	    {
          	if ( ++skill_columns[level] % 2 == 0)
		    strcat(skill_list[level],"\n\r          ");
          	strcat(skill_list[level],buf);
	    }
	}
    }
    /* add on any skill sets */
    if (HAS_SS(ch->class)){
      int ss = 0;
      /* put in each group of ss's at their level */
      if ( (ss = ss_lookup(class_table[ch->class].ss)) != 0){
	int i = 0;
	int picks = 0;
	/* check if we even have selections at this level */
	for (i = 0; ss_table[ss].level > 0 && i < MAX_SELECT; i++){
	  int level = ss_table[ss].level[i];

	  if (ss_table[ss].picks[i] == 0
	      || IS_NULLSTR(ss_table[ss].group[i]))
	    break;
	  /* check for level range dicatated by the command */
	  if (level < min_lev || level > max_lev)
	    continue;
	  /* check if we have any picks out of this group */
	  if ( (picks = can_group_select(ch, ss_group_lookup(ss_table[ss].group[i]), level)) == 0)
	    continue;
	  found = TRUE;
	  if (ch->level >= level)
	    sprintf(buf,"%-17s [%3d]     ",ss_table[ss].group[i], picks);
	  else
	    sprintf(buf,"%-17s [%3s]     ",ss_table[ss].group[i], " n/a");
	  if (skill_list[level][0] == '\0')
	    sprintf(skill_list[level],"\n\rLevel %2d: %s",level,buf);
	  else
	    {
	      if ( ++skill_columns[level] % 2 == 0)
		strcat(skill_list[level],"\n\r          ");
	      strcat(skill_list[level],buf);
	    }
	}
      }
    }
    if (!IS_NPC(ch) && ch->pcdata->newskills != NULL )
    {
	SKILL_DATA *nsk;
	for (nsk = ch->pcdata->newskills; nsk != NULL; nsk = nsk->next )
	{
	  if ((level=nsk->level) < LEVEL_IMMORTAL + 2
	      && level >= min_lev && level <= max_lev
	      && skill_table[nsk->sn].spell_fun == spell_null
	      && nsk->learned > 0)
	  {
	    found = TRUE;
	    if (ch->level < nsk->level)
	    	sprintf(buf,"%-18s  n/a      ", skill_table[nsk->sn].name);
	    else
                sprintf(buf,"%-18s %3d      ", skill_table[nsk->sn].name, nsk->learned);
	    if (skill_list[level][0] == '\0')
          	sprintf(skill_list[level],"\n\rLevel %2d: %s",level,buf);
            else
	    {
          	if ( ++skill_columns[level] % 2 == 0)
		    strcat(skill_list[level],"\n\r          ");
          	strcat(skill_list[level],buf);
	    }
	  }
	}
    }
    if (!found)
    {
      	send_to_char("No skills found.\n\r",ch);
      	return;
    }
    for (level = 0; level < LEVEL_IMMORTAL + 2; level++)
      	if (skill_list[level][0] != '\0')
            send_to_char(skill_list[level],ch);
    send_to_char("\n\r",ch);
}

int exp_per_level(CHAR_DATA *ch, int level)
{
    int temp;
    if (IS_NPC(ch))
	return 1000;
    temp = (pc_race_table[ch->race].class_mult + class_table[ch->class].extra_exp);
    return temp+temp*(level-1)*0.2;
}

long total_exp(CHAR_DATA *ch)
{
    int temp, i, total;
    if (IS_NPC(ch))
        return (ch->level+1)*1000;
    temp = (pc_race_table[ch->race].class_mult + class_table[ch->class].extra_exp);
    total = temp;
    for (i = 1; i <= ch->level; i++)
        total += exp_per_level(ch,i);
    return total;
}

void check_improve( CHAR_DATA *ch, int sn, bool success, int bonus_gain ){
  SKILL_DATA* nsk = NULL;
  CSKILL_DATA* cSkill = NULL;

  //const
  const int base_gain = int_app[get_curr_stat(ch,STAT_INT)].learn;
  const int smart_mod = 2;//given to races that learn faster
  const int dnd_mul = 300;//%multiplier of gain for dnd system spells

  int limit = 100;	   //how far can the skill be improved.
  int rating = 0;
  sh_int *learned = 0;
  int* progress = 0;
  int gain = base_gain;

  //npcs do not learn
  if (IS_NPC(ch))
    return;
  //spells cast on self do not case learning either
  else if (is_affected(ch, gsn_nolearn))
    return;
  //do not learn skills we cannot use yet
  else if (ch->level < sklevel(ch, sn) )
    return;

  /* get info on the skill we are going to learn */
  /* first check if we should look at dynamic skills */
  if ( ch->pCabal && (cSkill = get_cskill(ch->pCabal, sn)) != NULL){
    rating = cSkill->pSkill->rating;
    learned = &ch->pcdata->learned[sn];
  }
  else if ( (nsk = nskill_find(ch->pcdata->newskills,sn)) == NULL){
    rating = skill_table[sn].rating[ch->class];
    learned = &ch->pcdata->learned[sn];
  }
  else{
    rating = nsk->rating;
    learned = &nsk->learned;
  }

  //get skill limit
  limit = get_skillmax( ch, sn );

  /* easy abort cases */
  if (rating == 0 || *learned < 2 || *learned >= limit)
    return;
  else if (ch->level <= 10 && *learned >= 80)
    return;
  else
    progress = &ch->pcdata->progress[sn];

  /* calculate progress gain */
  //penalty from the skill_table 1 = normal, 9 = very hard
  //ratings of 10 (racial skills are treated as 1)
  if (rating == 10)
    rating = 1;
  rating = URANGE(1, rating, 9);
  gain = 2 * base_gain / (1 + rating);

  //multiply the basic gain by lag for this skill if any
  gain = gain * UMAX(1, skill_table[sn].beats / PULSE_VIOLENCE);

  //Double for skills that cannot be used in combat
  if (skill_table[sn].minimum_position > POS_FIGHTING)
    gain += gain;

  //due to legacy issues, we treat bonus of 1 as 0
  if (bonus_gain == 1)
    bonus_gain -=1;
  gain += bonus_gain;
  gain += UMAX(0, get_curr_stat(ch,STAT_WIS) - 20);
  if (ch->race == grn_half_elf || IS_PERK( ch, PERK_SICKLY))
    gain += smart_mod;
  if (ch->pcdata->condition[COND_HUNGER] <= 5)
    gain /=2;
  //failure gain
  if (!success)
    gain = 15 * gain / 10;
  gain = UMAX(1, gain);

  if (IS_DNDS(ch)
      && cSkill == NULL
      && skill_table[sn].spell_fun != spell_null
      && !IS_GNBIT(sn, GN_NO_DND)){
    gain = dnd_mul * gain / 100;
  }

  if (skill_table[sn].spell_fun == spell_null)
    gain = gain * (100 + ch->aff_mod[MOD_SKILL_LEARN]) / 100;
  else
    gain = gain * (100 + ch->aff_mod[MOD_SPELL_LEARN]) / 100;

  //force increase
  if (bonus_gain == -99)
    *progress = 0;
  //decrease progress
  *progress -= gain;

  /* check for increase of skill */
  if (*progress < 1){
    *learned += 1;
    *progress = get_skill_reps( *learned );
    *progress += (rand() % 11) * *progress / 100;

    sendf(ch,"You have become better at %s!\n\r", skill_table[sn].name);
    gain_exp(ch, number_range( rating, rating * 15));

    if (*learned == limit){
      sendf(ch, "You have now mastered <%s>!\n\r",skill_table[sn].name);
      *progress = 0;
    }
  }
}

void check_improve_old( CHAR_DATA *ch, int sn, bool success, int multiplier )
{
  SKILL_DATA* nsk = NULL;
  CSKILL_DATA* cSkill = NULL;
  //const
  const int base_chance = int_app[get_curr_stat(ch,STAT_INT)].learn;
  const int level_mod = 50; //(percent of lvl bonus)
  const int wis_mod = 50; //(percent of lvl bonus)
  const int smart_mod = 10;//given to smart races.
  const int dnd_mul = 600;//multiplier of chance for dnd spell system spells

  int limit = 100;	   //how far can the skill be improved.
  int mod = 0;

  int rating = 0;
  sh_int *learned = 0;
  int chance;
  bool dcheck = TRUE;

  if (IS_NPC(ch))
    return;
  else if (is_affected(ch, gsn_nolearn))
    return;

  if (sn > 5000){
    dcheck = FALSE;
    sn -= 5000;
  }

  if (ch->level < sklevel(ch,sn) )
    return;

  limit = get_skillmax( ch, sn );
  /* get info on the skill we are going to learn */
  /* first check if we should look at dynamic skills */
  if ( ch->pCabal && (cSkill = get_cskill(ch->pCabal, sn)) != NULL){
    rating = cSkill->pSkill->rating;
    learned = &ch->pcdata->learned[sn];
  }
  else if ( (nsk = nskill_find(ch->pcdata->newskills,sn)) == NULL){
    rating = skill_table[sn].rating[ch->class];
    learned = &ch->pcdata->learned[sn];
  }
  else{
    rating = nsk->rating;
    learned = &nsk->learned;
  }

/* check for skill stats now */

  if (rating == 0
      || *learned < 2
      || *learned >= limit)
    return;

  if (ch->level <= 10 && *learned >= 80)
    return;

  /* check if this is a dnd spell caster */
  if (multiplier == -99){
    sendf(ch,"You have become better at %s!\n\r", skill_table[sn].name);
    *learned += 1;
    gain_exp(ch, rating);
    return;
  }
  chance = base_chance;
  //benefit from multiplier.
  chance = (9 + UMAX(1, multiplier)) * chance / 10;
  //penalty from the skill_table
  chance = (11  - ( rating >= 10? 6: rating)) * chance / 10;
//level bonus
  chance += level_mod * ch->level / 100;
//smart races bonus
  if (ch->race == race_lookup("half-elf")
      || ch->race == race_lookup("faerie")
      || IS_PERK( ch, PERK_SICKLY)
      )
    chance += smart_mod;
//wisdom bonus
  chance += wis_mod *  get_curr_stat(ch,STAT_WIS) / 100;

  if (ch->pcdata->condition[COND_HUNGER] <= 5)
    chance /=2;

  if (!success)
    chance = 12 * chance / 10;
  chance = UMAX(1, chance);

  if (IS_DNDS(ch)
      && cSkill == NULL
      && skill_table[sn].spell_fun != spell_null
      && !IS_GNBIT(sn, GN_NO_DND)){
    chance += dnd_mul * chance / 100;
  }
  if (number_range(1,1000) > chance){
    if (number_percent() < 5 && dcheck)
      check_improve(ch,sn+5000,success,multiplier);
    return;
  }
  if (skill_table[sn].spell_fun == spell_null)
    mod = ch->aff_mod[MOD_SKILL_LEARN];
  else
    mod = ch->aff_mod[MOD_SPELL_LEARN];
  if (success){
    chance = URANGE(5,limit - *learned, 95);
    chance = chance * (100 + mod) / 100;
    if (number_percent() < chance){
      sendf(ch,"You have become better at %s!\n\r", skill_table[sn].name);
      *learned += 1;
      gain_exp(ch, number_range( rating, rating * 15));
    }
  }
  else{
    chance = URANGE(5, *learned / 2, 30);
    chance = chance * (100 + mod) / 100;
    if (number_percent() < chance){
      sendf(ch,"You learn from your mistakes, and your %s skill improves.\n\r",skill_table[sn].name);
      *learned += number_range(1,3);
      *learned = UMIN(*learned, limit);
      gain_exp(ch, rating * 20);
    }
  }
  if (*learned == limit)
    sendf(ch, "You have now mastered <%s>!\n\r",skill_table[sn].name);
}

int group_lookup( const char *name )
{
    int gn;
    for ( gn = 0; gn < MAX_GROUP; gn++ )
    {
        if ( group_table[gn].name == NULL )
            break;
        if ( LOWER(name[0]) == LOWER(group_table[gn].name[0]) && !str_prefix( name, group_table[gn].name ) )
            return gn;
    }
    return -1;
}

void gn_add( CHAR_DATA *ch, int gn)
{
    int i;
    ch->pcdata->group_known[gn] = TRUE;
    for ( i = 0; i < MAX_IN_GROUP; i++)
    {
        if (group_table[gn].spells[i] == NULL)
            break;
        group_add(ch,group_table[gn].spells[i],FALSE);
    }
}

void group_add( CHAR_DATA *ch, const char *name, bool deduct)
{
    int sn = skill_lookup(name), gn = group_lookup(name);
    if (IS_NPC(ch))
	return;
    if (sn != -1)
    {
        if (ch->pcdata->learned[sn] == 0 || ch->pcdata->learned[sn] == 1 )
	{
	    if (deduct)
	        ch->pcdata->learned[sn] = 100;
	    else
	        ch->pcdata->learned[sn] = 1;
	}
	return;
    }
    if (gn != -1)
    {
	if (ch->pcdata->group_known[gn] == FALSE)
	    ch->pcdata->group_known[gn] = TRUE;
        gn_add(ch,gn);
    }
}

void do_envenom(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    AFFECT_DATA af;
    bool fAdv = get_skill(ch, skill_lookup("lotus scourge")) > 1;

    int percent, skill, np = number_percent();
    if ((skill = get_skill(ch,gsn_envenom)) == 0)
    {
	send_to_char("Are you crazy? You'd poison yourself!\n\r",ch);
	return;
    }
    if (argument[0] == '\0')
    {
	send_to_char("Envenom what item?\n\r",ch);
	return;
    }
    obj =  get_obj_list(ch,argument,ch->carrying);
    if (obj== NULL)
    {
	send_to_char("You don't have that item.\n\r",ch);
	return;
    }
    if (ch->mana < 50)
    {
	send_to_char("Rest up a bit first.\n\r",ch);
	return;
    }
    ch->mana -= 50;
    if (fAdv){
      send_to_char("Using the lore of the Lotus Scourge you prepare a deadly toxin.\n\r", ch);
    }
    if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
    {
        if (UMAX(1,np + 2*(get_curr_stat(ch,STAT_LUCK) - 16)) < skill)
	{
	    act("$n treats $p with deadly poison.",ch,obj,NULL,TO_ROOM);
	    act("You treat $p with deadly poison.",ch,obj,NULL,TO_CHAR);
	    if (!obj->value[3])
	    {
		obj->value[3] = 1;
		check_improve(ch,gsn_envenom,TRUE,1);
	    }
	    else
		send_to_char("It's already poisoned.\n\r",ch);
	    WAIT_STATE2(ch,skill_table[gsn_envenom].beats);
	    return;
	}
	act("You fail to poison $p.",ch,obj,NULL,TO_CHAR);
	if (!obj->value[3])
	    check_improve(ch,gsn_envenom,FALSE,1);
	WAIT_STATE2(ch,skill_table[gsn_envenom].beats);
	return;
     }
     if (obj->item_type == ITEM_THROW)
     {
       if (obj->value[3] < 0 || attack_table[obj->value[3]].damage == DAM_BASH)
	 {
	   send_to_char("You can only envenom edged weapons.\n\r",ch);
	   return;
	 }
       if (IS_WEAPON_STAT(obj,WEAPON_POISON))
	 {
	   act("$p is already envenomed.",ch,obj,NULL,TO_CHAR);
	   return;
	 }
       percent = number_percent();
       percent -= 3 * (get_curr_stat(ch,STAT_LUCK) - 10);
       if (percent < 4 * skill / 5)
	 {
	   af.where     = TO_WEAPON;
	   af.type      = gsn_poison;
	   af.level     = fAdv ? ch->level + 5 : ch->level;
	   af.duration  = fAdv ? ch->level : ch->level * percent / 75;
	   af.location  = 0;
	   af.modifier  = 0;
	   af.bitvector = WEAPON_POISON;
	   affect_to_obj(obj,&af);
	   act("$n coats $p with deadly venom.",ch,obj,NULL,TO_ROOM);
	   act("You coat $p with venom.",ch,obj,NULL,TO_CHAR);
	   check_improve(ch,gsn_envenom,TRUE,3);
	   WAIT_STATE2(ch,skill_table[gsn_envenom].beats);
	   return;
	 }
       else
	 {
	   act("You fail to envenom $p.",ch,obj,NULL,TO_CHAR);
	   check_improve(ch,gsn_envenom,FALSE,3);
	   WAIT_STATE2(ch,skill_table[gsn_envenom].beats);
	   return;
	 }
     }
     act("You can't poison $p.",ch,obj,NULL,TO_CHAR);
}

void do_berserk( CHAR_DATA *ch, char *argument)
{
    int chance, hp_percent = 0;

    /* Useless conditional */
    if( hp_percent != 0 )
        hp_percent = 0;

    if ((chance = 3 * get_skill(ch,gsn_berserk) / 4) == 0)
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }
    if ((IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BERSERK))
    || (!IS_NPC(ch) && ch->level < skill_table[gsn_berserk].skill_level[ch->class]))
    {
	send_to_char("You turn red in the face, but nothing happens.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_BERSERK) || is_affected(ch,gsn_berserk) || is_affected(ch,skill_lookup("frenzy")))
    {
	send_to_char("You get a little madder.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_CALM) || is_affected(ch,gsn_prayer))
    {
	send_to_char("You can't get worked up in your calm state.\n\r",ch);
	return;
    }
    if (!IS_NPC(ch) && ch->mana < 50)
    {
	send_to_char("You can't get up enough energy.\n\r",ch);
	return;
    }
    if (ch->position == POS_FIGHTING && chance > 1)
	chance += 35;
    hp_percent = 100 * ch->hit/ch->max_hit;
    if (number_percent() < chance)
    {
	AFFECT_DATA af;
	WAIT_STATE2(ch,PULSE_VIOLENCE);

	ch->move = UMAX(ch->move - 50, 0);
	if (!IS_NPC(ch))
	  ch->mana -= 50;
	ch->hit += 2 * ch->level;

	act("Your pulse races as you are consumed by rage!",ch,NULL,NULL,TO_CHAR);
	act("$n gets a wild look in $s eyes.",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_berserk,TRUE,1);
	af.where	= TO_AFFECTS;
	af.type		= gsn_berserk;
	af.level	= ch->level;
        af.duration     = number_fuzzy(ch->level / 8);
	af.modifier	= UMAX(1,ch->level/8);
	af.bitvector 	= AFF_BERSERK;
	af.location	= APPLY_DAMROLL;
	affect_to_char(ch,&af);
	af.location	= APPLY_HITROLL;
	af.modifier	= UMAX(1, ch->level / 16);
	affect_to_char(ch,&af);
	af.modifier	= 40;
	af.location	= APPLY_AC;
	affect_to_char(ch,&af);
	af.modifier	= ch->level;
	af.location	= APPLY_HIT;
	affect_to_char(ch, &af);
    }
    else
    {
	WAIT_STATE2(ch,3 * PULSE_VIOLENCE);
	ch->mana -= 25;
	send_to_char("Your pulse speeds up, but nothing happens.\n\r",ch);
	check_improve(ch,gsn_berserk,FALSE,1);
    }
}

void do_bash( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;

//Data
    int chance = 4 * get_skill(ch,gsn_bash) /5;
    int dam = number_range(10 + ch->level/2, ch->size *  ch->level / 2);
    int vic_lag  = PULSE_VIOLENCE;
    int ch_lag = skill_table[gsn_bash].beats;

//bools
    bool fSuccess = FALSE;
    bool fYell = FALSE;
    one_argument(argument,arg);

    if (chance == 0)
    {
	send_to_char("Bashing? What's that?\n\r",ch);
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

    if ( victim == ch )
    {
      send_to_char("Self mutilation is not your style.\n\r",ch);
      return;
    }

    if (is_safe(ch,victim))
	return;

    //Get chance to bash.
    chance += ch->carry_weight/200 - victim->carry_weight/200;
    chance += get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_DEX);
    chance += GET_AC2(victim,AC_BASH) /25;
    chance -= get_skill(victim,gsn_dodge)/20;
    chance += (ch->level - victim->level);
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    if ( ch->race == race_lookup("avian")
	 || victim->race == race_lookup("faerie") )
      chance -=5;
    if ( victim->class == class_lookup("thief") )
      chance -= 5;
    if (is_affected(victim,gsn_horse))
      chance -= 5;
    if (!IS_NPC(victim) && victim->pcdata->pStallion && number_percent() < get_skill(victim, gsn_mounted)){
      chance -= 12;
      check_improve(victim, gsn_mounted, TRUE, 0);
    }
    chance += affect_by_size(ch,victim);

    //check if target should yell.
      fYell = (ch->fighting != victim && victim->fighting != ch);
    //sound attack to justice /imm
      a_yell(ch,victim);
//      sendf( ch, "chance: %d", chance );
    if (number_percent() < chance)
      {
	vic_lag  = URANGE(1, number_range(1, ch->size - victim->size + 3), 4) * PULSE_VIOLENCE;
	fSuccess = TRUE;
      }
    else
      {
	dam = 0;
      }

    //predict interdict
    if (predictCheck(ch, victim, "bash", skill_table[gsn_bash].name))
      return;

//MANTIS CHECK
    if (is_affected(victim,gsn_mantis) && fSuccess)
      {
	act("You use $n's momentum against him, and throw $m to the ground!",ch,NULL,victim,TO_VICT);
	act("$N reverses your attack, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
	act("$N reversed $n's attack, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
	WAIT_STATE2(ch, URANGE(1, number_range(1, victim->size), 3) * PULSE_VIOLENCE);
	damage(victim,ch,dam,gsn_mantis,DAM_BASH,TRUE);
	affect_strip(victim,gsn_mantis);
	return;
      }//END MANTIS

//check for lag protect
    if (!do_lagprot(ch, victim, chance, &dam, &vic_lag, gsn_bash, fSuccess, fYell))
      {
	if (fYell)
	  {
	    sprintf(buf, "Help! %s is trying to bash me!",PERS(ch,victim));
	    j_yell(victim,buf);
	  }
	if (fSuccess)
	  {

           act("$n sends you sprawling with a powerful bash!",ch,NULL,victim,TO_VICT);
	   act("You slam into $N, and send $M flying!",ch,NULL,victim,TO_CHAR);
	   act("$n sends $N sprawling with a powerful bash.",ch,NULL,victim,TO_NOTVICT);
	 }
      	else
	  {
	    act("You fall flat on your face!", ch,NULL,victim,TO_CHAR);
	    act("$n falls flat on $s face.", ch,NULL,victim,TO_NOTVICT);
	    act("You evade $n's bash, causing $m to fall flat on $s face.",ch,NULL,victim,TO_VICT);
	  }
      }//END IF NO PROTECTION


    if (!fSuccess || dam == 0 || vic_lag == 0)
    {
      check_improve(ch, gsn_bash, FALSE, 1);
      ch_lag = 3 * ch_lag / 2;
    }
    else
      check_improve(ch, gsn_bash, TRUE, 1);

    WAIT_STATE2(ch, ch_lag);
    if (vic_lag != 0 && dam != 0)
      WAIT_STATE2(victim, vic_lag);
    if (ch->race == race_lookup("fire") && number_percent() < 3 * ch->level / 2)
      damage(ch,victim,3 * dam / 2,gsn_bash ,DAM_FIRE,TRUE);
    else
      damage(ch,victim,dam,gsn_bash,DAM_BASH,TRUE);

    if (ch && victim && ch->fighting == victim && dam > 0)
      check_ccombat_actions( victim, ch, CCOMBAT_BASHED);
}


/* does the actual dirt kicking: */
/* wait = true    will lag the dirt kicker */
/* wait = false   will not lag the dirt kicker */
void do_dirting ( CHAR_DATA *ch, int wait, char *argument) {
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    int chance;
    one_argument(argument,arg);
    if ( (chance = get_skill(ch,gsn_dirt)) == 0)
    {
	send_to_char("You get your feet dirty.\n\r",ch);
	return;
    }
    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't in combat!\n\r",ch);
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
    if (IS_AFFECTED(victim,AFF_BLIND))
    {
	act("$E's already been blinded.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( victim == ch )
	return;


    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += (ch->level - victim->level);
    chance -= get_skill(victim,gsn_dodge)/10;
    chance += UMAX(0, victim->size - SIZE_MEDIUM) * 5;

    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    switch(ch->in_room->sector_type)
    {
	case(SECT_INSIDE):		chance -= 25;	break;
	case(SECT_CITY):		chance -= 15;	break;
	case(SECT_FIELD):		chance +=  5;	break;
	case(SECT_FOREST):				break;
	case(SECT_HILLS):				break;
	case(SECT_MOUNTAIN):		chance += 10;	break;
	case(SECT_WATER_SWIM):		chance  =  0;	break;
	case(SECT_WATER_NOSWIM):	chance  =  0;	break;
	case(SECT_AIR):			chance  =  0;  	break;
	case(SECT_DESERT):		chance += 25;   break;
    }
    if (chance == 0)
    {
	send_to_char("There isn't any dirt to kick.\n\r",ch);
	return;
    }
    if (wait == TRUE) {
      WAIT_STATE2(ch,skill_table[gsn_dirt].beats);
    }
    a_yell(ch,victim);

    //predict interdict
    if (predictCheck(ch, victim, "dirt", skill_table[gsn_dirt].name))
      return;

    chance = chance - 25 + UMIN(ch->level,25);
    if (is_affected(victim, gsn_mist_form))
      {
	act("You kick a few stones right through the mist.", ch, NULL, victim, TO_CHAR);
	act("$n kicks a bit of dirt right through $N.", ch, NULL, victim, TO_NOTVICT);
	act("$N kick a handfull of dirt through your non-corporeal form.", ch, NULL, victim, TO_CHAR);
	chance = 0;
      }
    if (number_percent() < chance)
    {
      bool fDirtProt =  (IS_EVIL(victim) && victim->race == race_lookup("slith") && number_percent() < 15 + victim->level);
	AFFECT_DATA af;
        if (ch->fighting != victim && victim->fighting != ch)
        {
            sprintf(buf, "Help! %s kicked dirt into my eyes!",PERS(ch,victim));
            j_yell(victim,buf);
        }
	act("$n is blinded by the dirt in $s eyes!",victim,NULL,NULL,TO_ROOM);
	act("$n kicks dirt in your eyes!",ch,NULL,victim,TO_VICT);
        damage(ch,victim,number_range(2,5),gsn_dirt,DAM_NONE,TRUE);
	send_to_char("You can't see a thing!\n\r",victim);
        check_improve(ch,gsn_dirt,TRUE,1);

	if (fDirtProt)
	  {
	    send_to_char("Your lizard ancestry serves you well and you easly recover.\n\r", victim);
	    return;
	  }

	af.where	= TO_AFFECTS;
	af.type 	= gsn_dirt;
	af.level 	= ch->level;
	af.duration	= 0;
	af.location	= APPLY_HITROLL;
        if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	  if (is_affected(victim, gsn_battletrance))
	    af.modifier      = 0;
	  else
            af.modifier      = -3;
	}
        else
            af.modifier      = -6;
	af.bitvector 	= AFF_BLIND;

	affect_to_char(victim,&af);

        af.location     = APPLY_AC;
        if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	  if (is_affected(victim, gsn_battletrance))
	    af.modifier      = 0;
	  else
            af.modifier      = +5;
	}
        else
            af.modifier      = +15;

	  affect_to_char(victim,&af);
    }
    else
    {
        if (ch->fighting != victim && victim->fighting != ch)
        {
            sprintf(buf, "Help! %s tried to kick dirt into my eyes!",PERS(ch,victim));
            j_yell(victim,buf);
        }
	damage(ch,victim,0,gsn_dirt,DAM_NONE,TRUE);
	check_improve(ch,gsn_dirt,FALSE,1);
        if (ch->fighting == NULL) {
	  if (wait == TRUE) {
	    WAIT_STATE2(ch,skill_table[gsn_dirt].beats);
	  }
	}
    }
}

void do_dirt( CHAR_DATA *ch, char *argument ) {
     do_dirting (ch, TRUE, argument);
}

void trip( CHAR_DATA *ch, CHAR_DATA* victim, int chance)
{
  bool fTrample = !IS_NPC(ch) && get_skill(ch, gsn_trample) > 1;
  int position = victim->position;

  if ((IS_AFFECTED(victim,AFF_FLYING)
       && !is_affected(victim,gsn_thrash))
      || (!IS_NPC(victim) && victim->pcdata->pStallion != NULL && get_skill(victim, gsn_mounted)))
    {
      act("$S feet aren't on the ground.",ch,NULL,victim,TO_CHAR);
      act("Luckily your feet aren't on the ground.",ch,NULL,victim,TO_VICT);
      return;
    }
    if (check_ccombat_actions( victim, ch, CCOMBAT_TRIPPED))
      return;

    if (number_percent() < get_skill(victim, gsn_balance)
	&& is_empowered_quiet(victim,1) && victim->mana > 0)
      {
	act("$n tries to trip you, but you keep your balance!",ch,NULL,victim,TO_VICT);
	act("You try to trip $N, but $E keeps $S balance!",ch,NULL,victim,TO_CHAR);
	act("$n tries to trip $N, but $E keeps $S balance.",ch,NULL,victim,TO_NOTVICT);
	check_improve(victim,gsn_balance,TRUE,50);
	victim->mana -= URANGE(0,25,victim->mana);

      }
    else
      {
	act("$n trips you and you go down!",ch,NULL,victim,TO_VICT);
	act("You trip $N and $E goes down!",ch,NULL,victim,TO_CHAR);
	act("$n trips $N, sending $M to the ground.",ch,NULL,victim,TO_NOTVICT);
	if (!is_affected(victim, gsn_iron_will)
	    || victim->mana < 1
	    || number_percent() < (95 + (victim->level - ch->level) * 3)){

	  if (fTrample && ch->size >= victim->size)
	    WAIT_STATE2(victim, ch->size * PULSE_VIOLENCE);
	  else
	    WAIT_STATE2(victim, UMIN(2,number_range(1, victim->size)) * PULSE_VIOLENCE);
	  position = POS_RESTING;
	}
	else{
	  send_to_char("Through your force of will you remain upright.\n\r", ch);
	  act("$n somehow remains upright.", victim, NULL, NULL, TO_ROOM);
	  victim->mana -= URANGE(0,25,victim->mana);
	}
      }
    if (fTrample && ch->size >= victim->size)
      damage(ch,victim, number_range(10,  20) * ch->size, gsn_trample, DAM_BASH, TRUE);
    else
      damage(ch,victim, number_range(2,  2 +  2 * victim->size), gsn_trip,DAM_BASH,TRUE);

    if (ch && ch->fighting && victim && victim->in_room){
      victim->position = position;
      check_ccombat_actions( ch, victim, CCOMBAT_TRIP);
    }

}

void do_trip( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    int chance;
    one_argument(argument,arg);
    if ( (chance = get_skill(ch,gsn_trip)) == 0)
    {
	send_to_char("Tripping?  What's that?\n\r",ch);
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
    if ((IS_AFFECTED(victim,AFF_FLYING) && !is_affected(victim,gsn_thrash))
	|| (!IS_NPC(victim) && victim->pcdata->pStallion != NULL && get_skill(victim, gsn_mounted)))
      {
	act("$S feet aren't on the ground.",ch,NULL,victim,TO_CHAR);
	act("Luckily your feet aren't on the ground.",ch,NULL,victim,TO_VICT);
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
    chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX))*3;
    chance += (ch->level - victim->level);
//bigger guys are easier to trip
    chance += UMAX(0, victim->size - SIZE_MEDIUM) * 5;
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));

    if (is_affected(victim,gsn_horse))
	chance -= 20;
    if (IS_AFFECTED2(victim,AFF_BLUR))
        chance /= 2;

    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "trip", skill_table[gsn_trip].name))
      return;

    if (number_percent() < chance)
    {
	if (is_affected(victim,gsn_mantis) && !(IS_AFFECTED(ch,AFF_FLYING) && !is_affected(ch,gsn_thrash)))
	{
    	    if (ch->fighting != victim && victim->fighting != ch)
    	    {
    	        sprintf(buf, "Help! %s just tried to tripped me!",PERS(ch,victim));
    	        j_yell(victim,buf);
    	    }
            act("You reversed $n's trip, and $e goes down!",ch,NULL,victim,TO_VICT);
	    act("$N reverses your trip, and you go down!",ch,NULL,victim,TO_CHAR);
            act("$N reversed $n's trip, sending $m to the ground.",ch,NULL,victim,TO_NOTVICT);
	    if (!(is_affected(ch, gsn_iron_will)  && ch->mana > 0) || number_percent() < 50 + victim->level - ch->level){
	      ch->position = POS_RESTING;
	        WAIT_STATE2(ch, UMIN(2,number_range(ch->size,1)) * PULSE_VIOLENCE);
	    }
	    else{
	      send_to_char("Through your force of will you remain upright.\n\r", ch);
	      act("$n somehow remains upright.", victim, NULL, NULL, TO_ROOM);
	      victim->mana -= URANGE(0,25,victim->mana);
	      ch->mana -= URANGE(0,25,ch->mana);
	    }
            damage(victim,ch,number_range(2, 2 +  2 * ch->size),gsn_trip,DAM_BASH,TRUE);
	    affect_strip(victim,gsn_mantis);
	    return;
	}
        if (ch->fighting != victim && victim->fighting != ch)
        {
            sprintf(buf, "Help! %s just tripped me!",PERS(ch,victim));
            j_yell(victim,buf);
        }
 	if (ch->class == class_lookup("monk"))
	{
	    act("$n trips you with a leg sweep and you go down!",ch,NULL,victim,TO_VICT);
	    act("You trip $N with a leg sweep and $E goes down!",ch,NULL,victim,TO_CHAR);
	    act("$n trips $N with a leg sweep, sending $M to the ground.",ch,NULL,victim,TO_NOTVICT);
	    if (!(is_affected(victim, gsn_iron_will) && victim->mana > 0) || number_percent() < (25 + victim->level - ch->level)){
	        WAIT_STATE2(victim, UMIN(2,number_range(victim->size,1)) * PULSE_VIOLENCE);
	    victim->position = POS_RESTING;
	    }
	    else{
	      send_to_char("Through your force of will you remain upright.\n\r", ch);
	      act("$n somehow remains upright.", victim, NULL, NULL, TO_ROOM);
	      victim->mana -= URANGE(0,25,victim->mana);
	    }
	    damage(ch,victim,number_range(2, 2 +  2 * victim->size),gsn_trip,DAM_BASH,TRUE);

	}
	else
	  trip(ch, victim, chance);
	WAIT_STATE2(ch,skill_table[gsn_trip].beats);
	check_improve(ch,gsn_trip,TRUE,1);
    }
    else
    {
        if (ch->fighting != victim && victim->fighting != ch)
        {
            sprintf(buf, "Help! %s tried to trip me!",PERS(ch,victim));
            j_yell(victim,buf);
        }
	damage(ch,victim,0,gsn_trip,DAM_BASH,TRUE);
        check_improve(ch,gsn_trip,FALSE,1);
	WAIT_STATE2(ch,2 * skill_table[gsn_trip].beats / 3);
    }
}

void do_lash( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    AFFECT_DATA af;                  // affect data (for air_thrash)

    one_argument(argument,arg);
    if (arg[0] == '\0')
    {
	victim = ch->fighting;
    }
    else if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
	return;
    }
    act("$n lashes at you with $s weapon and you go down!",ch,NULL,victim,TO_VICT);
    act("You lash at $N with your weapon and $N goes down!",ch,NULL,victim,TO_CHAR);
    act("$n lashes at $N with $s, sending $M to the ground.",ch,NULL,victim,TO_NOTVICT);

/* air thrash the victim if flying */
    if (IS_AFFECTED(victim, AFF_FLYING) && !is_affected(victim, gsn_thrash)) {
      af.where  = TO_AFFECTS;
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
}

void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    OBJ_DATA *obj, *obj2;
    int chance, dual_chance, wpnswitch;
    one_argument( argument, arg );
    if ((chance = get_skill(ch,gsn_backstab)) == 0)
    {
	send_to_char("You don't know how to backstab.\n\r",ch);
	return;
    }
    if (arg[0] == '\0')
    {
        send_to_char("Backstab whom?\n\r",ch);
        return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("You can't backstab while fighting.\n\r",ch);
	return;
    }
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
    if (victim->fighting != NULL)
    {
	sendf(ch, "%s won't hold still long enough.\n\r", PERS(victim,ch));
	return;
    }
    if (IS_NPC(victim) && IS_SET(victim->act, ACT_TOO_BIG) )
    {
	sendf(ch, "%s is too alert for you to attempt such an act.\n\r", PERS(victim,ch));
	return;
    }
    if ( victim == ch )
	return;
    if ( is_safe( ch, victim ) )
        return;
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
        return;
    }
    obj = get_eq_char( ch, WEAR_WIELD );
    obj2 = get_eq_char( ch, WEAR_SECONDARY);
    wpnswitch = FALSE;
    if (obj == NULL
	|| (obj->value[0] != WEAPON_DAGGER
	    && obj->value[0] != WEAPON_EXOTIC
	    && obj->value[0] != WEAPON_SPEAR))
    {
	wpnswitch = TRUE;
	if (obj2 == NULL
	    || (obj2->value[0] != WEAPON_DAGGER
		&& obj2->value[0] != WEAPON_EXOTIC
		&& obj2->value[0] != WEAPON_SPEAR))
	  {
	    send_to_char( "You need to wield a piercing weapon to backstab.\n\r", ch );
	    return;
	}
    }
    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
    {
	send_to_char("You can't seem to get up enough speed.\n\r",ch);
	return;
    }
    if ( !IS_NPC(ch) && victim->hit < (victim->max_hit * 0.85) )
    {
        sendf(ch, "%s is hurt and suspicious ... you can't sneak up.\n\r", PERS(victim,ch) );
	return;
    }
    dual_chance = chance;
    chance += get_curr_stat(ch,STAT_DEX)+get_curr_stat(ch,STAT_INT)/2;
    chance -= get_curr_stat(victim,STAT_DEX)+get_curr_stat(ch,STAT_WIS)/2;
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
        chance -= 10;
    chance += (ch->level - victim->level);
    chance += 2*(get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    if (!IS_AWAKE(victim) )
        chance += 10;
    WAIT_STATE2( ch, skill_table[gsn_backstab].beats );
    a_yell(ch,victim);

    //predict interdict
    if (predictCheck(ch, victim, "backstab", skill_table[gsn_backstab].name))
      return;

    if ( number_percent( ) < chance || !IS_AWAKE(victim) )
    {
	check_improve(ch,gsn_backstab,TRUE,1);
//MANTIS CHECK
	if ( (is_affected(victim,gsn_mantis) || get_skill(victim, gsn_shadowmaster) > 1) && IS_AWAKE(victim)){
	  act("You sense $n's movement and throw $m to the ground!",ch,NULL,victim,TO_VICT);
	  act("$N senses your attack, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
	  act("$N senses $n's attack, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
	  WAIT_STATE2(ch, URANGE(2, 1 + number_range(1, victim->size), 4) * PULSE_VIOLENCE);
	  do_visible(ch, "");

	  sprintf(buf, "Help!  %s just tried to backstab me!",PERS(ch,victim));
	  j_yell(victim,buf);
	  affect_strip(victim,gsn_mantis);
	  damage(victim,ch,10,gsn_mantis,DAM_BASH,TRUE);
	  return;
	}//END MATIS

        sprintf(buf, "Help! %s backstabbed me!",PERS(ch,victim));
    	j_yell(victim,buf);
	if (!wpnswitch)
	    one_hit( ch, victim, gsn_backstab, FALSE );
	if (wpnswitch)
	{
	    if (number_percent( ) < get_skill(ch, gsn_dual_wield))
		one_hit( ch, victim, gsn_backstab, TRUE );
	    else
		damage( ch, victim, 0, gsn_backstab, DAM_NONE, TRUE);
	}
  	dual_chance -= chance;
	if ((chance = get_skill(ch, gsn_dual_backstab)) != 0 && (!wpnswitch)
	    && (obj != NULL
		&& (obj->value[0] == WEAPON_DAGGER
		    || obj->value[0] == WEAPON_EXOTIC
		    || obj->value[0] == WEAPON_SPEAR))
	    && (obj2 != NULL
		&& (obj2->value[0] == WEAPON_DAGGER
		    || obj2->value[0] == WEAPON_EXOTIC
		    || obj2->value[0] == WEAPON_SPEAR)) )
	  {
	    if ((number_percent( ) < 3*(chance - dual_chance) /4
	    && number_percent( ) < get_skill(ch, gsn_dual_wield)) )
	    {
	    	check_improve(ch,gsn_dual_backstab,TRUE,1);
		one_hit( ch, victim, gsn_dual_backstab, TRUE );
	    }
	    else
		check_improve(ch,gsn_dual_backstab,FALSE,1);
	}
    }
    else
    {
        check_improve(ch,gsn_backstab,FALSE,1);
        if (ch->fighting == NULL)
        {
            sprintf(buf, "Help! %s tried to backstab me!",PERS(ch,victim));
            j_yell(victim,buf);
        }
	damage( ch, victim, 0, gsn_backstab,DAM_NONE,TRUE);
    	dual_chance -= chance;
	if ((chance = get_skill(ch, gsn_dual_backstab)) != 0 && (!wpnswitch)
    	&& (obj != NULL && (obj->value[0] == 2 || obj->value[0] == 3))
	&& (obj2 != NULL && (obj2->value[0] == 2 || obj2->value[0] == 3)))
	{
	    if ((number_percent( ) < (2 * (chance - dual_chance) /3)
	    && number_percent( ) < get_skill(ch, gsn_dual_wield)))
	    {
	    	check_improve(ch,gsn_dual_backstab,TRUE,1);
		one_hit( ch, victim, gsn_dual_backstab, TRUE );
	    }
	    else
		check_improve(ch,gsn_dual_backstab,FALSE,1);
	}
    }
}

void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim, *v_next, *ech, *fch = NULL;
    int chance = 0;
    int skill = get_skill(ch, gsn_rescue);
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim == ch )
	return;
/* psalm of one god */
    if (is_affected(ch, gsn_ogtrium))
      skill = 150;

    if (!IS_NPC(ch) && skill < 1)
    {
	send_to_char( "You don't know how to rescue.\n\r", ch );
	return;
    }
    if ( victim->fighting == NULL )
    {
	sendf(ch, "%s is not fighting right now.\n\r", PERS(victim,ch) );
	return;
    }
/* special guards */
    if (IS_NPC(ch)
	&& (ch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD
	    || ch->pIndexData->vnum == MOB_VNUM_HOUND)
	&& !IS_AREA(ch->in_room->area, AREA_LAWFUL)
	&& !IS_WANTED(victim->fighting))
      return;
    if (is_safe(ch, victim->fighting))
    	return;
    for ( ech = victim->in_room->people; ech != NULL; ech = v_next )
    {
        v_next = ech->next_in_room;
        if ( ech->fighting != NULL && ech->fighting == victim)
        {
    	    if ( IS_NPC(ch) && ch->leader != NULL && !IS_NPC(ch->leader) && is_safe(ch->leader, ech) )
    		continue;
	    else if (is_safe(ch,ech))
		continue;
	    fch = ech;
	    break;
	}
    }
    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }
    WAIT_STATE2( ch, skill_table[gsn_rescue].beats );
    WAIT_STATE2( victim, 2 * PULSE_VIOLENCE);
    if ( fch == NULL )
    {
	send_to_char( "You cannot rescue them.\n\r", ch );
	return;
    }
    chance = skill;
    if (chance > 0);
	chance += 2*(get_curr_stat(ch,STAT_LUCK) - 16);
    if ( number_percent() > chance)
    {
	send_to_char( "You failed the rescue.\n\r", ch );
	check_improve(ch,gsn_rescue,FALSE,1);
	return;
    }
    act( "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n rescues you!", ch, NULL, victim, TO_VICT    );
    act( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );
    check_improve(ch,gsn_rescue,TRUE,1);
    stop_fighting( fch, FALSE );
    if (victim->fighting == fch)
    	stop_fighting( victim, FALSE );
    if ( ch->class == class_lookup("paladin") && is_affected(ch,gsn_heroism))
	do_charge(ch,fch->name);
    set_fighting( ch, fch );
    set_fighting( fch, ch );
}

void kick( CHAR_DATA* ch, CHAR_DATA* victim, int chance ){
  int dam = number_range( ch->level/2, 8 * ch->level / 5);

  chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
  chance += (ch->level - victim->level);
  chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));

  if ( number_percent( ) < chance )
    {
      int chance2 = get_skill(ch,gsn_double_kick);
      if (is_affected(victim,gsn_mantis))
	{
	  act("You step out of $n's kick, and deliver one of your own!",ch,NULL,victim,TO_VICT);
	  act("$N steps out of your kick, and delivers one of $S own!",ch,NULL,victim,TO_CHAR);
	  act("$N steps out of $n's kick, and delivers one of $S own.",ch,NULL,victim,TO_NOTVICT);
	  damage(victim,ch,dam/2,gsn_kick,DAM_BASH,TRUE);
	  affect_strip(victim,gsn_mantis);
	  return;
	}
      chance2 += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
      chance2 += (ch->level - victim->level);
      chance2 += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
      damage(ch,victim,dam,gsn_kick,DAM_BASH,TRUE);
      if (ch->fighting == victim && number_percent() < 3*get_skill(ch,gsn_double_kick)/4)
	{
	  if (is_affected(ch,gsn_crane))
	    chance2 += 20;
	  if (number_percent() < chance2)
	    {
              chance = 3*((ch->level /2) + (3*get_skill(ch,gsn_double_kick)/4) )/4;
              if (ch->class == class_lookup("monk") && number_percent() < chance)
		{
                  if (is_affected(ch,gsn_monkey))
		    {
                      send_to_char( "You unleash the technique of Fu Manchu.\n\r", ch );
                      damage(ch,victim,3*dam/4, gsn_double_kick,DAM_BASH,TRUE);
		      if (victim->position > POS_RESTING && !is_safe(ch, victim))
			damage(ch,victim,2*dam/3, gsn_double_kick,DAM_BASH,TRUE);
		      if (victim->position > POS_RESTING && !is_safe(ch, victim))
			damage(ch,victim,2*dam/3, gsn_double_kick,DAM_BASH,TRUE);
		    }
                  else if (is_affected(ch,gsn_tiger))
		    {
                      send_to_char("You tear with the claws of the tiger.\n\r", ch);
                      damage(ch,victim,11*dam/10, gsn_double_kick,DAM_PIERCE,TRUE);
		    }
                  else if (is_affected(ch,gsn_buddha))
		    {
                      send_to_char("You invoke the essence of the soul.\n\r", ch);
                      damage(ch,victim,20*dam/10, gsn_double_kick,DAM_MENTAL, TRUE);
		    }
                  else if (is_affected(ch,gsn_horse))
		    {
                      send_to_char("You kick with the strength of the horse.\n\r",ch);
                      damage(ch,victim,12*dam/10, gsn_double_kick,DAM_BASH,TRUE);
		    }
                  else if (is_affected(ch,gsn_dragon))
		    {
                      send_to_char("You summon the spirit of the dragon.\n\r",ch);
                      damage(ch,victim,2*dam/1, gsn_double_kick,DAM_ENERGY,TRUE);
		    }
		  else if (is_affected(ch,gsn_drunken))
		    {
		      damage(ch,victim,number_range( 2*dam/3, dam),gsn_double_kick,DAM_BASH,TRUE);
		      if ( number_percent( ) < 3 * chance / 2 )
			{
			  send_to_char("You stagger into your opponent like a blind drunk.\n\r",ch);
			  act("$n sweeps your legs with a staggering motion!",ch,NULL,victim,TO_VICT);
			  act("$n sweeps $N's legs with a staggering motion!",ch,NULL,victim,TO_NOTVICT);
			  if ((IS_AFFECTED(victim,AFF_FLYING)
			       && !is_affected(victim,gsn_thrash))
			      || (!IS_NPC(victim) && victim->pcdata->pStallion != NULL && get_skill(victim, gsn_mounted)))
			    {
			      act("$S feet aren't on the ground.",ch,NULL,victim,TO_CHAR);
			      act("Luckily your feet aren't on the ground.",ch,NULL,victim,TO_VICT);
			    }
			  else
			    WAIT_STATE2(victim, UMIN(2,number_range(victim->size,1)) * PULSE_VIOLENCE);
			}
		    }
		  else
                    damage(ch,victim,number_range( 2*dam/3, dam),gsn_double_kick,DAM_BASH,TRUE);
		}
	      else
		damage(ch,victim,number_range( dam/2, dam), gsn_double_kick,DAM_BASH,TRUE);
	      check_improve(ch,gsn_double_kick,TRUE,10);
	    }
	  else
	    {
	      damage( ch, victim, 0, gsn_double_kick,DAM_BASH,TRUE);
	      check_improve(ch,gsn_double_kick,FALSE,10);
	    }
	}
      check_improve(ch,gsn_kick,TRUE,10);
    }
  else
    {
      damage( ch, victim, 0, gsn_kick,DAM_BASH,TRUE);
      check_improve(ch,gsn_kick,FALSE,10);
    }
}
void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim = ch->fighting;
    int chance;
    if ( (chance = get_skill(ch,gsn_kick)) == 0)
    {
        send_to_char("You better leave the martial arts to fighters.\n\r", ch );
	return;
    }
    if (victim == NULL ){
        send_to_char( "You aren't fighting anyone.\n\r", ch );
        return;
    }
    WAIT_STATE2( ch, skill_table[gsn_kick].beats );
    kick(ch, victim, chance );
}

bool disarm( CHAR_DATA *ch, CHAR_DATA *victim ){

  OBJ_DATA *obj, *wield = get_eq_char(ch, WEAR_WIELD);
  int skill = get_skill(victim, gsn_double_grip);
  bool fStr = FALSE;
  bool fSeize = FALSE;
  bool fExoSeize = FALSE;
  bool fPGrip = FALSE;

  if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL
       && ( obj = get_eq_char( victim, WEAR_SECONDARY ) ) == NULL )
    return FALSE;
  if ( IS_STONED(victim)
       || IS_OBJ_STAT(obj,ITEM_NOREMOVE)
       || is_affected(victim, gsn_graft_weapon)){
    act("$S weapon won't budge!",ch,NULL,victim,TO_CHAR);
    act("$n tries to disarm you, but your weapon won't budge!",ch,NULL,victim,TO_VICT);
    act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
    return FALSE;
  }

  if (obj != NULL
      && (obj->value[0] == WEAPON_POLEARM
	  || obj->value[0] == WEAPON_STAFF
	  || obj->value[0] == WEAPON_AXE)
      && get_skill(victim, skill_lookup("power grip")) > 1)
    skill += 15;
  else if (is_affected(victim, gsn_ironarm) && get_eq_char(victim, WEAR_SECONDARY) != NULL){
    fStr = TRUE;
    skill += 15;
  }
  if (is_affected(victim, gsn_istrength) && has_twohanded(victim) != NULL){
    fStr = TRUE;
    skill = 200;
  }

  if ( (is_affected(victim,gsn_double_grip)  || fStr)
       && number_percent() < (4 * skill / 3) - 75) {
    act("You fail to disarm $N's weapon from $S powerfull grasp.",ch,NULL,victim,TO_CHAR);
    act("$n tries to disarm you, but your grip is too strong.",ch,NULL,victim,TO_VICT);
    act("$n tries to disarm $N, but $E grips $S weapon tightly.",ch,NULL,victim,TO_NOTVICT);
    check_improve(ch,gsn_disarm, FALSE,1);
    check_improve(victim,gsn_double_grip, FALSE, 33);
    return FALSE;
  }

  act( "$n disarms you and sends your weapon flying!", ch, NULL, victim, TO_VICT    );
  act( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
  act( "$n disarms $N!",  ch, NULL, victim, TO_NOTVICT );

  if (check_ccombat_actions( ch, victim, CCOMBAT_DISARM ))
    return FALSE;

  /* check for power grip (MUST be before obj_from_char) */
  if (obj != NULL
      && (obj->value[0] == WEAPON_POLEARM
	  || obj->value[0] == WEAPON_STAFF
	  || obj->value[0] == WEAPON_AXE)
      && is_affected(victim, gsn_double_grip)
      && get_skill(victim, skill_lookup("power grip")) > 1){
    fPGrip = TRUE;
  }

  obj_from_char( obj );

  /* check for advanced disarms */
  if ( wield != NULL
       && (wield->value[0] == WEAPON_EXOTIC || wield->value[0] == WEAPON_WHIP)){
    int sn = skill_lookup("weapon seize");
    if (number_percent() < get_skill(ch, sn)){
      check_improve(ch, sn, TRUE, 0);
      fSeize = TRUE;
      /* check for exotic seize */
      if (wield->value[0] == WEAPON_EXOTIC
	  && get_skill(ch, gsn_exotic_mastery) > 1)
	fExoSeize = TRUE;
    }
    else
      check_improve(ch, sn, FALSE, -99);
  }

  if (!fExoSeize
      && (IS_OBJ_STAT(obj,ITEM_NODROP)
	  || IS_OBJ_STAT(obj,ITEM_INVENTORY)
	  || fPGrip) ){
    obj_to_char( obj, victim );
  }
  else if (fSeize){
    act("With a flick of $p $n grabs the weapon.", ch, wield, NULL, TO_ROOM);
    act("With a flick of $p you grab the weapon.", ch, wield, NULL, TO_CHAR);
    obj_to_char( obj, ch );
  }
  else{
    obj_to_room( obj, victim->in_room );
    if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj))
      get_obj(victim,obj,NULL);
  }
  return TRUE;
}

void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    int chance, hth = 0, ch_weapon, vict_weapon, ch_vict_weapon;
    bool fIronArm = TRUE;

    if ((chance =  get_skill(ch,gsn_disarm)) == 0){
      send_to_char( "Huh?\n\r", ch );
      return;
    }
    else if ( (obj = getBmWep( ch, TRUE )) == NULL
	      && (obj = getBmWep( ch, FALSE )) == NULL
	      && ((hth = get_skill(ch,gsn_hand_to_hand)) == 0
		  || (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_DISARM))) )
      {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
      }
    else if ( ( victim = ch->fighting ) == NULL ){
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
    }
    else if ( ( obj2 = get_eq_char( victim, WEAR_WIELD ) ) == NULL
	      && ( obj2 = get_eq_char( victim, WEAR_SECONDARY ) ) == NULL ){
      sendf(ch, "%s is not wielding a weapon.\n\r", PERS(victim,ch) );
      return;
    }

    ch_weapon = get_weapon_skill_obj(ch, obj);
    vict_weapon = get_weapon_skill_obj(victim, obj2);
    ch_vict_weapon = get_weapon_skill_obj(ch, obj2);

    if (IS_NPC(victim))
	vict_weapon = UMAX(victim->level * 2, 75);

    if ( obj == NULL){
      if (ch->class == class_lookup("monk"))
	chance = chance * hth/100;
      else
	chance = chance * hth/150;
    }
    else
      chance = chance * ch_weapon/100;

    chance += get_weapon_obj_rating( obj, WEAPON_WITHDIS ) * 4;
    chance += get_weapon_obj_rating( obj2, WEAPON_TOBEDIS ) * 4;

    if (ch->class != gcn_blademaster || !is_affected(ch, gsn_ironarm) || get_eq_char(ch, WEAR_SECONDARY) == NULL){
      fIronArm = FALSE;
      ch_vict_weapon = 100;
    }
    chance += (ch_vict_weapon - vict_weapon) / 2;
    chance += 2 * (get_curr_stat(ch,STAT_DEX)-get_curr_stat(victim,STAT_STR));
    chance += 2*(get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    chance += (ch->level - victim->level);
    chance -= get_skill(victim,gsn_parry)/10;
    if (IS_NPC(victim))
      chance -= victim->level/10;

    if (ch->class == class_lookup("monk")
	&& is_affected(ch,gsn_dragon)
	&& is_empowered(ch,0))
      chance += 15;
    if (number_percent() < chance
	&& (victim->level - ch->level) < 10){
      WAIT_STATE2( ch, skill_table[gsn_disarm].beats );
      check_improve(ch,gsn_disarm,TRUE,1);
      if ( fIronArm){
	WAIT_STATE2(victim, PULSE_VIOLENCE );
	damage(ch, victim, UMIN(victim->hit, number_range(3, 10)), gsn_ironarm, DAM_INTERNAL, TRUE);
      }
      disarm( ch, victim );
    }
    else{
      WAIT_STATE2(ch,skill_table[gsn_disarm].beats);
      act("You fail to disarm $N.",ch,NULL,victim,TO_CHAR);
      act("$n tries to disarm you, but fails.",ch,NULL,victim,TO_VICT);
      act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
      check_improve(ch,gsn_disarm,FALSE,1);
    }
}

void shield_disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;
    if ( (obj = get_eq_char( victim, WEAR_SHIELD ) )== NULL)
	return;
    if ( IS_STONED(victim) || IS_OBJ_STAT(obj,ITEM_NOREMOVE))
    {
        act("$S shield won't budge!",ch,NULL,victim,TO_CHAR);
        act("$n tries to disarm your shield, but it won't budge!",ch,NULL,victim,TO_VICT);
        act("$n tries to disarm $N's shield, but fails.",ch,NULL,victim,TO_NOTVICT);
	return;
    }
    act( "$n disarms your shield and it falls to the ground!", ch, NULL, victim, TO_VICT    );
    act( "You disarm $N's shield!",  ch, NULL, victim, TO_CHAR    );
    act( "$n disarms $N's shield!",  ch, NULL, victim, TO_NOTVICT );
    obj_from_char( obj );
    if ( IS_OBJ_STAT(obj,ITEM_NODROP) || IS_OBJ_STAT(obj,ITEM_INVENTORY) )
	obj_to_char( obj, victim );
    else
    {
	obj_to_room( obj, victim->in_room );
	if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj))
            get_obj(victim,obj,NULL);
    }
}

void do_shield_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance, ch_weapon, hth;

    if ((chance = 4 * get_skill(ch,gsn_shield_disarm) / 5) == 0)
    {
        send_to_char( "You don't know how to disarm shields.\n\r", ch );
	return;
    }
    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( ( obj = get_eq_char( victim, WEAR_SHIELD ) ) == NULL )
    {
        sendf(ch, "%s is not wearing a shield.\n\r", PERS(victim,ch) );
	return;
    }
    //bonus for bless
    if (obj && IS_GOOD(victim) && IS_OBJ_STAT(obj,ITEM_BLESS))
      chance -= 15;
    else if (obj && IS_EVIL(victim) && IS_OBJ_STAT(obj,ITEM_DARK))
      chance -= 15;

    hth = get_skill(ch,gsn_hand_to_hand);
    ch_weapon = get_weapon_skill_obj(ch, obj);

    if (IS_NPC(ch))
      chance /= 2;

    if ( obj == NULL){
      if (ch->class == class_lookup("monk"))
	chance = chance * hth/100;
      else
	chance = chance * hth/150;
    }
    else
      chance = chance * ch_weapon/100;

    chance += get_weapon_obj_rating( obj, WEAPON_WITHSDIS ) * 4;
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_STR);
    chance += (ch->level - victim->level);
    chance -= get_skill(victim,gsn_shield_block)/4;
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));

    if (ch->class == class_lookup("monk") && is_affected(ch,gsn_dragon) && is_empowered(ch,0))
      chance += 15;

    if (number_percent() < chance)
    {
        WAIT_STATE2( ch, skill_table[gsn_shield_disarm].beats );
        shield_disarm( ch, victim );
        check_improve(ch,gsn_shield_disarm,TRUE,1);
    }
    else
    {
        WAIT_STATE2(ch,skill_table[gsn_shield_disarm].beats);
        act("You fail to disarm $N's shield.",ch,NULL,victim,TO_CHAR);
        act("$n tries to disarm your shield, but fails.",ch,NULL,victim,TO_VICT);
        act("$n tries to disarm $N's shield, but fails.",ch,NULL,victim,TO_NOTVICT);
        check_improve(ch,gsn_shield_disarm,FALSE,1);
    }
}

void do_lore( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj;
    int chance;
    int sn = skill_lookup("history of armaments");
    int sn2 = skill_lookup("arms maintenance");
    bool fExtract  = FALSE;

    one_argument( argument, arg );
    if (ch->fighting != NULL)
    {
        send_to_char("You're too busy fighting!\n\r",ch);
        return;
    }
    if (arg[0] == '\0')
    {
        send_to_char("Lore what?\n\r",ch);
        return;
    }
    obj = get_obj_list(ch,arg,ch->carrying);
    if (obj== NULL || CAN_WEAR(obj,ITEM_WEAR_TATTOO))
    {
        send_to_char("You don't have that item.\n\r",ch);
        return;
    }
    if ((chance = get_skill(ch,gsn_lore)) <= 1)
    {
        send_to_char("It looks like what it appears to be.\n\r",ch);
        return;
    }
    if (ch->mana < 30)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    WAIT_STATE(ch,skill_table[gsn_lore].beats);

    ch->mana -= 30;
    act("You examine $p closely.",ch,obj,NULL,TO_CHAR);
    act("$n examines $p closely.",ch,obj,NULL,TO_ROOM);

    if (obj->pIndexData->vnum == OBJ_VNUM_CLONE){
      fExtract = TRUE;
      obj = create_object( get_obj_index(obj->cost), obj->level);
    }


    if (obj->item_type == ITEM_ARMOR
	&& number_percent() < get_skill(ch, sn)){
      send_to_char("You dip into your knowledge of weapons and armor.\n\r", ch);
      check_improve(ch, sn, TRUE, 5);
      spell_identify(skill_lookup("identify"), ch->level, ch, obj, TARGET_OBJ);
    sendf(ch, "Rumor has it that you can find it in %s.\n\r", obj->pIndexData->area->name);
      if (fExtract)
	extract_obj ( obj );
      return;
    }
    else if (obj->item_type == ITEM_WEAPON
	     && number_percent() < get_skill(ch, sn2)){
      send_to_char("You dip into your knowledge of weapons.\n\r", ch);
      check_improve(ch, sn2, TRUE, 5);
      spell_identify(skill_lookup("identify"), ch->level, ch, obj, TARGET_OBJ);
      sendf(ch, "Rumor has it that you can find it in %s.\n\r", obj->pIndexData->area->name);
      if (fExtract)
	extract_obj ( obj );
      return;
    }
    else
      check_improve(ch, sn, FALSE, 5);
    chance += 25;
    chance += 5 * (get_curr_stat(ch,STAT_LUCK) - 14);
    if (number_percent() > chance)
    {
        send_to_char("The meaning of this object escapes you at the moment.\n\r",ch);
	if (fExtract)
	  extract_obj ( obj );
        return;
    }
    chance = get_skill(ch,gsn_lore);
    chance += get_curr_stat(ch,STAT_WIS) + get_curr_stat(ch,STAT_INT) - 40;
    chance += UMAX(0, (ch->level - obj->level) / 2);
    chance = UMAX(0, 110 - chance);
    chance = number_range(-chance, chance);
    if (ch->class == class_lookup("bard"))
      chance = 0;

    sendf( ch,"Object is type %s, material %s.\n\rWeight is %d, value is %d, level is %d.\n\r",
	   item_name(obj->item_type),
	   obj->material,
	   obj->weight / 10 + (obj->weight * chance / 1000),
	   obj->cost + (obj->cost *  chance / 100),
	   obj->level + (obj->level * chance / 100));
    switch ( obj->item_type )
    {
    case ITEM_SOCKET:
      if (IS_SOC_STAT(obj, SOCKET_ARMOR)
	  && IS_SOC_STAT(obj, SOCKET_WEAPON))
	act("Armor and Weapon socket.",
	    ch, obj, NULL, TO_CHAR);
      else if (IS_SOC_STAT(obj, SOCKET_ARMOR))
	act("Armor only socket.",
	    ch, obj, NULL, TO_CHAR);
      else if (IS_SOC_STAT(obj, SOCKET_WEAPON))
	act("Weapon only socket.",
	    ch, obj, NULL, TO_CHAR);
      else
	act("Weapon only socket.",
	    ch, obj, NULL, TO_CHAR);
      break;
    case ITEM_DRINK_CON:
      sendf(ch,"It holds %s-colored %s.\n\r",
	    liq_table[obj->value[2]].liq_color,
	    liq_table[obj->value[2]].liq_name);
      break;
    case ITEM_CONTAINER:
	sendf(ch,"Capacity: %d#  Maximum weight: %d#\n\r",
	      obj->value[3] + (obj->value[3] * chance/100),
	      obj->value[0] + (obj->value[0] * chance/100));
	break;
    case ITEM_WEAPON:
      send_to_char("Weapon type is ",ch);
      if (IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS)
	  || obj->value[0] == WEAPON_STAFF)
	send_to_char("two-handed ", ch);
      switch (obj->value[0])
	{
        case(WEAPON_EXOTIC) : send_to_char("exotic.\n\r",ch);       break;
        case(WEAPON_SWORD)  : send_to_char("sword.\n\r",ch);        break;
        case(WEAPON_DAGGER) : send_to_char("dagger.\n\r",ch);       break;
        case(WEAPON_SPEAR)  : send_to_char("spear.\n\r",ch);  break;
        case(WEAPON_STAFF)  : send_to_char("staff.\n\r",ch);  break;
        case(WEAPON_MACE)   : send_to_char("mace/club.\n\r",ch);    break;
        case(WEAPON_AXE)    : send_to_char("axe.\n\r",ch);          break;
        case(WEAPON_FLAIL)  : send_to_char("flail.\n\r",ch);        break;
        case(WEAPON_WHIP)   : send_to_char("whip.\n\r",ch);         break;
        case(WEAPON_POLEARM): send_to_char("polearm.\n\r",ch);      break;
        default             : send_to_char("unknown.\n\r",ch);      break;
 	}
      if (obj->pIndexData->new_format)
	sendf(ch,"Average damage is %d.\n\r",
              (1 + obj->value[2])* obj->value[1]/ 2 +(1 + obj->value[2]) * (obj->value[1]/ 2) * chance / 100);
	else
	  sendf( ch, "Average damage is %d.\n\r",
		 (obj->value[1] + (obj->value[2]) / 2) +
		 (obj->value[1] + (obj->value[2]) / 2) * chance/100);
      if ( IS_OBJ_STAT(obj, ITEM_SOCKETABLE))
	{
	  if (obj->contains){
	    act( "$p has the following affixed to it:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	  }
	  else
	    act( "$p seems to have a space to attach something to it.", ch, obj, NULL, TO_CHAR );
	}
      break;
    case ITEM_ARMOR:
      sendf( ch, "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.\n\r",
	     obj->value[0] + obj->value[0] * chance/100,
	     obj->value[1] + obj->value[1] * chance/100,
	     obj->value[2] + obj->value[2] * chance/100,
	     obj->value[3] + obj->value[3] * chance/100);
      if ( IS_OBJ_STAT(obj, ITEM_SOCKETABLE))
	{
	  if (obj->contains){
	    act( "$p has the following affixed to it:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	  }
	  else
	    act( "$p seems to have a space to attach something to it.", ch, obj, NULL, TO_CHAR );
	}

      break;
    case ITEM_PROJECTILE:
      sendf(ch, "Is a projectile of type %s\n\r",
	    flag_string(projectile_type, obj->value[0]));
      if (obj->pIndexData->new_format)
	sendf(ch,"Average damage is %d.\n\r",
              (1 + obj->value[2])* obj->value[1]/ 2 +(1 + obj->value[2]) * (obj->value[1]/ 2) * chance / 100);
      else
	sendf( ch, "Average damage is %d.\n\r",
	       (obj->value[1] + (obj->value[2]) / 2) +
	       (obj->value[1] + (obj->value[2]) / 2) * chance/100);
      break;
    case ITEM_RANGED:
      if (obj->value[3] == 0)
	sendf(ch, "Can be used to fire: %s.\n\r",
	      flag_string(projectile_type, obj->value[0]));
      else{
	OBJ_INDEX_DATA* ammo = get_obj_index( obj->value[3] );
	sendf(ch, "Can be used to fire: %s.\n\r",
	      ammo == NULL ? "NOT FOUND" : ammo->short_descr);
      }
      sendf(ch, "Has capacity of: %d, and rate of fire: %d\n\r",
	    chance * obj->value[1] / 100, chance * obj->value[2] / 100);
      break;
    }
    if (obj->pCabal || obj->race || obj->class >= 0){
      sendf(ch, "You have a feeling only a %s%s%s%s%s%scan use it.\n\r",
	    obj->race ? race_table[obj->race].name : "",
	    obj->race ? " " : "",
	    obj->class >= 0 ? class_table[obj->class].name : "",
	    obj->class >= 0 ? " ": "",
	    obj->pCabal ? obj->pCabal->name : "",
	    obj->pCabal ? " ": "");
    }
    if (CAN_WEAR(obj, ITEM_UNIQUE))
      send_to_char("Unique item.\n\r",ch);
    if (CAN_WEAR(obj, ITEM_RARE))
      send_to_char("Rare item.\n\r",ch);
    if (CAN_WEAR(obj, ITEM_PARRY))
      send_to_char("Can be dual parried.\n\r",ch);
    sendf(ch, "Rumor has it that you can find it in %s.\n\r", obj->pIndexData->area->name);
    check_improve(ch,gsn_lore,TRUE,1);
    if (fExtract)
      extract_obj ( obj );
}

void do_lay_on_hands(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MIL];
    sh_int chance;
    AFFECT_DATA af;
    const int cost = ch->class == class_lookup("crusader") ? 100 : 50;
    one_argument (argument, arg);
    if ((chance = get_skill (ch, gsn_lay_on_hands)) == 0)
    {
        send_to_char("You do not know the secrets of this skill.\n\r", ch);
        return;
    }
    if (is_affected(ch,gsn_lay_on_hands))
    {
	send_to_char("You can't sustain another lay on hands at this time.\n\r",ch);
	return;
    }
    victim = get_char_room(ch, NULL, arg);
    if ( victim == NULL )
    {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }
    if ( victim == ch )
    {
        send_to_char("You can't lay on hands yourself.\n\r",ch);
        return;
    }
    if (ch->mana < cost)
    {
	send_to_char("You don't have enough mana to perform a lay on hands.\n\r",ch);
	return;
    }
    if (number_percent() > chance)
    {
        act("Something disturbs your concentration and you fail the lay on hands.", ch,NULL,NULL,TO_CHAR);
	act("$n's hands glow with a soft light, but it fades suddenly.",ch,NULL,NULL,TO_ROOM);
        ch->mana -= 25;
	check_improve(ch,gsn_lay_on_hands,FALSE,1);
    }
    else
    {
      if (ch->class == class_lookup("crusader")){
	act("You chant an ancient psalm as you press your hands against $N's back.",ch,NULL,victim,TO_CHAR);
	act("$n chants an ancient psalm as $e presses $s hands against $N's back.",ch,NULL,victim,TO_NOTVICT);
	act("$n chants an ancient psalm as $e presses $s hands against your back.",ch,NULL,victim,TO_VICT);
      }
      else{
	act("Your hands glow with a soft light as you place them on $N's wounds.",ch,NULL,victim,TO_CHAR);
	act("$n's hands glow with a soft light as $e places them on your wounds.",ch,NULL,victim,TO_VICT);
	act("$n's hands glow with a soft light as $e places them on $N's wounds.",ch,NULL,victim,TO_NOTVICT);
      }
      ch->mana -= cost;

//Embrace check.
      if (is_affected(victim, gsn_embrace_poison))
	{
	  if (embrace_heal_check(gsn_lay_on_hands, ch->level, ch, victim,  0))
	    affect_strip(victim, gsn_embrace_poison);
	}
      else
	{
	  int gain = victim->max_hit - victim->hit;
	  gain = ch->level * 2 + number_range(gain / 4, gain / 2);
	  gain = UMIN(gain, ch->level * 10);

	  victim->hit = UMIN(victim->max_hit, victim->hit + gain );

	  check_dispel( -99, victim, gsn_blindness);
	  check_dispel( -99, victim, gsn_death_grasp);
	  check_dispel( -99, victim, gsn_poison);
	  check_dispel( -99, victim, gsn_plague);
	  check_dispel( -99, victim, skill_lookup("dysentery"));
	  check_dispel( -99, victim, gsn_curse);
	  update_pos( victim );
	}//end embrace check
      check_improve(ch,gsn_lay_on_hands,TRUE,1);
    }
    af.type         = gsn_lay_on_hands;
    af.level        = ch->level;
    if (IS_NPC(ch))
      af.duration     = 1;
    else
      af.duration     = ch->class == class_lookup("crusader") ? 12 : 24;
    af.location     = 0;
    af.modifier     = 0;
    af.bitvector    = 0;
    affect_to_char(ch,&af);
    if (!IS_NPC(ch))
      {
	ch->pcdata->condition[COND_HUNGER] = 5;
        ch->pcdata->condition[COND_THIRST] = 5;
      }
    WAIT_STATE2(ch,skill_table[gsn_lay_on_hands].beats);
}

void do_detect_hidden( CHAR_DATA *ch, char *argument )
{
    int chance;
    AFFECT_DATA af;
    if ( (chance = get_skill(ch,gsn_detect_hidden)) == 0 )
    {
        send_to_char("Huh?\n\r",ch);
	return;
    }
    if (ch->mana < 10)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    if ( IS_AFFECTED(ch, AFF_DETECT_HIDDEN) )
    {
        send_to_char("You are already as alert as you can be. \n\r",ch);
        return;
    }
    WAIT_STATE2(ch,skill_table[gsn_detect_hidden].beats);
    if (number_percent() > chance)
    {
	ch->mana -= 5;
	send_to_char("You peer around intently, but the shadows seem unrevealing.\n\r",ch);
        check_improve(ch,gsn_detect_hidden,FALSE,1);
	return;
    }
    ch->mana -= 10;
    af.where     = TO_AFFECTS;
    af.type      = gsn_detect_hidden;
    af.level     = ch->level;
    af.duration  = ch->level;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( ch, &af );
    send_to_char( "Your awareness improves.\n\r", ch );
    check_improve(ch,gsn_detect_hidden,TRUE,1);
}

void do_cleave( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance, success;
    one_argument( argument, arg );
    if ((chance = get_skill(ch,gsn_cleave)) == 0)
    {
	send_to_char("You don't know how to cleave.\n\r",ch);
	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("Not while you're fighting.\n\r",ch);
	return;
    }
    if (arg[0] == '\0')
    {
        send_to_char("Who do you wish to cleave?\n\r",ch);
        return;
    }
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
    if (victim->fighting != NULL)
    {
	sendf(ch, "%s won't hold still long enough.\n\r", PERS(victim,ch));
	return;
    }
    if (IS_NPC(victim) && IS_SET(victim->act, ACT_TOO_BIG) )
    {
	sendf(ch, "%s is too alert for you to attempt such an act.\n\r", PERS(victim,ch));
	return;
    }
    if ( victim == ch )
	return;
    if ( is_safe( ch, victim ) )
        return;
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
        return;
    }
    obj = get_eq_char( ch, WEAR_WIELD );
    if (obj == 0 || (obj->value[0] != WEAPON_SWORD
		     && obj->value[0] != WEAPON_AXE
		     && obj->value[0] != WEAPON_POLEARM))
    {
	send_to_char( "How do you expect to cleave with that weapon.\n\r", ch );
	return;
    }
    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
    {
	send_to_char("You can't seem to get up enough speed.\n\r",ch);
	return;
    }

    chance -= get_curr_stat(victim,STAT_DEX)+get_curr_stat(ch,STAT_INT)/2;
    chance -= get_curr_stat(victim,STAT_DEX)+get_curr_stat(ch,STAT_WIS)/2;
    if (IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
      chance -= 10;
    chance += (ch->level - victim->level);
//smaller people are easier to cleave
    chance += affect_by_size(ch,victim);
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    success = number_percent( );

    //now little bonuses for type of weapon.

    if ( (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
	 || is_affected(ch,gsn_double_grip) )
      success -= 3;
    if (obj->value[3] == WEAPON_AXE)
      success -= 2;
    else if (obj->value[3] == WEAPON_POLEARM)
      success -= 5;
    if (get_curr_stat(victim, STAT_CON) < 19)
      success -= 2;

    if ( (ch->alignment > GOOD_THRESH)
	 && (IS_OBJ_STAT(obj,ITEM_BLESS)) )
      success -=1;
    else if ( (ch->alignment < EVIL_THRESH)
	      && (IS_OBJ_STAT(obj,ITEM_EVIL)) )
      success -=1;

    WAIT_STATE2( ch, skill_table[gsn_cleave].beats );


    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "cleave", skill_table[gsn_cleave].name))
      return;

    if (is_affected(victim, gsn_cleave)){
      sendf(ch, "%s is hurt and suspicious ... you can't sneak up.\n\r", PERS(victim,ch));
      success = 999;
    }
    else{
      AFFECT_DATA af;

      af.type = gsn_cleave;
      af.duration = 3;
      af.level = ch->level;

      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier = 0;
      affect_to_char(victim, &af);
    }

    if ( (success < 10 || (IS_IMMORTAL(ch) && ch->wimpy == 69) )
	 && !IS_IMMORTAL(victim)
	 && IS_AWAKE( victim )
	 && !IS_UNDEAD( victim )
	 && !is_affected(victim, gsn_mist_form))
    {
      int dam_type = attack_table[obj->value[3]].damage;
      int dam = 3 * ch->level + 3 * UMAX(0, get_skill(ch, gsn_cleave) - 75);


      sprintf(buf, "Die! %s, you butchering fool!",PERS(ch,victim));
      j_yell(victim,buf);
      check_improve(ch,gsn_cleave,TRUE,1);

      /* counter */
      if (counter_check(ch, victim, get_skill(victim, gsn_counter), dam, dam_type, gsn_cleave)){
	return;
      }
      damage( ch, victim, dam, gsn_cleave,dam_type,TRUE);

      if (victim && !is_safe_quiet( ch, victim )){
	AFFECT_DATA af;
	act("Unable to withstand the shock $n crumples to the ground.", victim, NULL, NULL, TO_ROOM );
	act("Unable to withstand the shock you crumple to the ground.", victim, NULL, NULL, TO_CHAR );
	act("Your blow has knocked $N unconscious!", ch, NULL, victim, TO_CHAR );
	stop_fighting( victim, TRUE);

	af.where     = TO_AFFECTS;
	af.type      = gsn_sleep;
	af.level     = ch->level;
	af.duration  = 1;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SLEEP;
	affect_to_char( victim, &af );
	do_sleep(victim,"");
      }
      return;
    }
    if ( success < chance || !IS_AWAKE(victim) )
    {
      int dam_type = attack_table[obj->value[3]].damage;
      int dam = 3 * ch->level + 3 * UMAX(0, get_skill(ch, gsn_cleave) - 75);
      dam += (success < 50 ? ch->level : 0);

      sprintf(buf, "Die! %s, you butchering fool!",PERS(ch,victim));
      j_yell(victim,buf);
      check_improve(ch,gsn_cleave,TRUE,1);
      /* counter */
      if ( !counter_check(ch, victim, get_skill(victim, gsn_counter), dam, dam_type, gsn_cleave)){
	damage( ch, victim, dam, gsn_cleave,dam_type,TRUE);
      }
    }
    else
    {
        sprintf(buf, "Die! %s, you butchering fool!",PERS(ch,victim));
        j_yell(victim,buf);
        check_improve(ch,gsn_cleave,FALSE,1);
	multi_hit( ch, victim,obj->value[3]+TYPE_HIT);
    }
}

void do_assassinate( CHAR_DATA *ch, char *argument )
{
  /* Assassinate skill */
  /*
     The skill uses the study skill as a requirement for a successful
     attack.
     if no weapon return.
     if modifier is <= 5 then assasinate fails automaticly.
     each attempt on current studied target decrements
     chance to 5%.

     The gen_study stores the amount learned about target in "modifier".
     - modifier has ceiling of 60.
  */

  char arg[MIL], buf[MSL];
  CHAR_DATA *victim;
  CHAR_DATA* och, *och_next;
  AFFECT_DATA* paf = NULL;
  OBJ_DATA *obj;

//data
  int base_chance = 0;
  int chance = 0;
  int  weapon_skill = 0;
  int ass_skill = 0;

//const
  const int haste_mod = 30;   //bonus percent for haste.
  const int level_mod = 3;    //multiplier for the lvl difference.
  const int dex_med = 18;     //median for dexterity of attacker
  const int dex_mod = 1;      //multiplier for dex.
  const int size_mod = 5;     //size multiplier.
  const int luck_mod = 1;     //luck multipler
  const int luck_med = 14;    //luck median
  const float sleep_mod = 1.3;//mulitplier for sleeping
  const float see_mod = 1.3;  //multiplier for not seeing attacker if not sleeping.
  const float group_mod = 1.4; //bonus for being part of same group.
  const int weapon_med =  90; //median past which we get bonus for weapon skill
  const int dodge_mod = 10;    //divisor for target dodge penalty.
  const int fight_mod = 75;//multiplier for chance if target incombat.

//flags
  bool fNpc = FALSE;
  bool fAutoFail = FALSE;
  bool fKiss = get_skill(ch, gsn_death_kiss) > 1;

//get our argument.
    one_argument( argument, arg );

//check for skill
    if ((ass_skill = get_skill(ch,gsn_assassinate)) == 0)
      {
	send_to_char("You can't expect to assassinate someone without proper training.\n\r",ch);
	return;
      }

//check argument.
    if (arg[0] == '\0')
      {
	if(is_affected(ch, gen_study))
	  sendf(ch, "%s\n\r", study_info(ch, FALSE, NULL));
        send_to_char("Attempt to assassinate who?\n\r",ch);
        return;
      }

//look for target.
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
      {
        send_to_char("They aren't here.\n\r",ch);
        return;
      }

//too big check
    if ((fNpc = IS_NPC(victim)) && IS_SET(victim->act, ACT_TOO_BIG) )
      {
        sendf(ch, "%s is too alert for you to attempt such an act.\n\r", PERS(victim,ch));
        return;
      }

    if ( victim == ch )
      {
	send_to_char("Self mutilation is frowned upon by the gods\n\r", ch);
	return;
      }

//check weapon
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
      {
	send_to_char("You need a weapon first.\n\r", ch);
	return;
      }

//now we know if we are safe to try to attack.
    if ( is_safe( ch, victim ) )
      return;

    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
      {
	send_to_char("You can't seem to get up enough speed.\n\r",ch);
	return;
      }
//watchtower check
    if (is_affected(victim, gen_watchtower)){
      sendf( ch, "%s is shielded within the Watchtower.\n\r", PERS(victim, ch ));
      fAutoFail = TRUE;
    }
//check if studied at all. (PC only)
    if (!fNpc)
      {
	if ( (paf = affect_find(ch->affected , gen_study)) == NULL)
	  {
	    send_to_char("Without preparation you are bound to fail.\n\r", ch);
	    fAutoFail = TRUE;
	  }
	else if (strcmp(paf->string, victim->name))
	  {
//if studied check if this is the studied target.
	    sendf(ch, "You have studied %s and know nothing of use about %s.\n\r", paf->string, victim->name);
	    fAutoFail = TRUE;
	  }
	else if ( (base_chance = paf->modifier) <= 5)
//check if study is at 5 or less
	  fAutoFail = TRUE;
      }//end if PC


    //------------------CHANCE CALCULATION---//
    if (fNpc && !fAutoFail)//NPC
      {
	if (victim->level > ch->level) fAutoFail = TRUE;
	else
	  chance = UMAX(0, (ass_skill - 80) + ((ch->level - victim->level) * 3));
      }//end NPC chance
    else if (!fAutoFail)//PC
      {
//Begin calculating chances. (multipliers first then bonuses)
	chance = base_chance;
//we set the chance depending on the ass_skill
	if (ass_skill < 50) chance = 10 * chance / 100;
	else if (ass_skill < 60) chance = 10 * chance / 100;
	else if (ass_skill < 70) chance = 20 * chance / 100;
	else if (ass_skill < 75) chance = 35 * chance / 100;
	else if (ass_skill < 80) chance = 55 * chance / 100;
	else if (ass_skill < 85) chance = 70 * chance / 100;
	else if (ass_skill < 90) chance = 85 * chance / 100;
	else if (ass_skill < 95) chance = 95 * chance / 100;
	else if (ass_skill < 99) chance = 100 * chance / 100;
	else if (ass_skill >= 100) chance = 105 * chance / 100;

	//sendf(ch,"Chance after first mod: %d\n\r", chance);
//sleeping
	if (!IS_AWAKE(victim))
	  chance *= sleep_mod;
	else
//seeing
	  if (!can_see(victim, ch))
	    chance *= see_mod;

	//sendf(ch,"Chance after 2 mod: %d\n\r", chance);
//group
	if (is_same_group(ch, victim))
	  chance *= group_mod;
	//sendf(ch,"Chance after 3 mod: %d\n\r", chance);
//Penalty for in combat (more aware)
	if (victim->fighting != NULL)
	  chance = chance * fight_mod / 100;
	//sendf(ch,"Chance after 4 mod: %d\n\r", chance);
//weapon
	weapon_skill = get_weapon_skill_obj(ch, obj);
	chance = chance * weapon_skill / weapon_med;
	//sendf(ch,"Chance after 5 mod: %d\n\r", chance);
//Haste
	if (IS_AFFECTED(ch, AFF_HASTE))
	  chance += haste_mod;
	//sendf(ch,"Chance after 6 mod: %d\n\r", chance);
//--------END MULTIPLIERS BEGIN BONUSES-------//

//Level
	chance += (ch->level - victim->level) * level_mod;
	//sendf(ch,"Chance after 7 mod: %d\n\r", chance);
//Dex
	chance += (get_curr_stat(ch, STAT_DEX) - dex_med) * dex_mod;
	//sendf(ch,"Chance after 8 mod: %d\n\r", chance);
//size (large victim is harder to assassinae)
	chance += affect_by_size(ch,victim) * size_mod;
	//sendf(ch,"Chance after 9 mod: %d\n\r", chance);
////luck
	chance += (get_curr_stat(ch,STAT_LUCK) - luck_med) * luck_mod;
	//sendf(ch,"Chance after 10 mod: %d\n\r", chance);
//dodge
	chance -= get_skill(victim, gsn_dodge) / dodge_mod;
	//sendf(ch,"Chance after 11 mod: %d\n\r", chance);
//hitroll
	if ( GET_HITROLL( ch ) < 30 ){
	  chance  = GET_HITROLL( ch ) * chance / 35;
	}
//Now we floor the chance.
	chance = URANGE(5, chance, 95);
	//sendf(ch,"Chance after floor mod: %d\n\r", chance);
      }//END PC CHANCE

//Waitstate
    WAIT_STATE2( ch, skill_table[gsn_assassinate].beats );
    a_yell(ch,victim);

    //predict interdict
    if (predictCheck(ch, victim, "assassinate", skill_table[gsn_assassinate].name))
      return;

    //Calculate the hit
    //DEBUG
    //    sendf(ch, "study: %d, chance: %d, NPC? %d, aF: %d.\n\r", base_chance, chance, fNpc, fAutoFail);

//------ASSASSINATE---//

    if ((number_percent() < chance) && !IS_IMMORTAL(victim) && !fAutoFail)
      {

//MANTIS CHECK
	if (is_affected(victim,gsn_mantis))
	  {
	    act("You sense $n's movement and throw $m to the ground!",ch,NULL,victim,TO_VICT);
	    act("$N senses your attack, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
	    act("$N senses $n's attack, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
	    WAIT_STATE2(ch, URANGE(2, 1 + number_range(1, victim->size), 4) * PULSE_VIOLENCE);
	    do_visible(ch, "");

	    sprintf(buf, "Help! %s tried to assassinate me!",PERS(ch,victim));
	    j_yell(victim,buf);
	    affect_strip(victim,gsn_mantis);
	    damage(victim,ch,10,gsn_mantis,DAM_BASH,TRUE);
	    return;
	  }//END MATIS
/* undead check */
	if (!IS_NPC(victim) && IS_UNDEAD( victim ) ){
	  for ( och = victim->in_room->people; och != NULL; och = och_next ){
	    och_next = och->next_in_room;
	    if ( IS_NPC(och) && IS_UNDEAD(och)
		 && IS_AFFECTED(och, AFF_CHARM)
		 && och->master == victim){
	      act("$n calmly steps into harms way.", och, NULL, NULL, TO_ROOM);
	      check_social(och, "snarl", "");
	      victim = och;
	      break;
	    }
	  }
	}
/* justice check */
	else if (!IS_NPC(victim)
		 && IS_SET(victim->in_room->area->area_flags,AREA_LAWFUL)
		 && victim->pCabal
		 && IS_CABAL(victim->pCabal, CABAL_JUSTICE)){
	  for ( och = victim->in_room->people; och != NULL; och = och_next ){
	    och_next = och->next_in_room;
	    if ( IS_NPC(och) && och->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD
		 && IS_AFFECTED(och, AFF_CHARM)
		 && och->master == victim){
	      act("$n's eyes gleam with fanatic loyalty as $e throws $mself into harms way!", och, NULL, NULL, TO_ROOM);
	      REMOVE_BIT(och->comm,COMM_NOCHANNELS);
	      REMOVE_BIT(och->comm,COMM_NOYELL);
	      do_yell(och, "For Glory of Law I lay down my life!");
	      SET_BIT(och->comm,COMM_NOYELL);
	      SET_BIT(och->comm,COMM_NOCHANNELS);
	      victim = och;
	      break;
	    }
	  }
	}
	check_improve(ch,gsn_assassinate,TRUE,1);
	//use up the study
	if (paf != NULL)
	  paf->modifier = 1 * paf->modifier / 6;

	act("With lethal speed, you strike from the shadows and `1+++ ASSASSINATE +++`` $N!",ch,NULL,victim,TO_CHAR);
        sprintf(buf,"With lethal speed, %s strikes from the shadows and `1+++ ASSASSINATES +++`` you!\n\r",PERS(ch,victim));
	send_to_char(buf,victim);
	act("$n moves with lethal speed, and `1+++ ASSASSINATES +++`` $N!",ch,NULL,victim,TO_NOTVICT);
	if (!IS_NPC(ch) && ch->pcdata->alias[0] && !can_see(victim, ch))
	  act_new("The last thing you see is $t.\n\r", ch, ch->pcdata->alias, victim, TO_VICT, POS_DEAD);

	if ( !IS_NPC(victim)
	     && victim->pcdata->pbounty
	     && !is_affected(victim, gsn_mortally_wounded)
	     && can_collect(victim->pcdata->pbounty, ch )){
	  act( "$n is mortally wounded!!", victim, NULL, NULL, TO_ROOM);
	  act_new( "You have been mortally wounded!!", victim, NULL, NULL, TO_CHAR, POS_DEAD );
	  mercy_effect(ch, victim, 4);
	}
	else
	{
        	act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
		//        	send_to_char( "You have been ASSASSINATED!!\n\r\n\r", victim );
		raw_kill(ch, victim);
	}
	do_autos(ch);
	victim->hit = 1;
	victim->mana = 1;
	return;
      }//end success.
    else
      {
	check_improve(ch,gsn_assassinate,FALSE,1);
	if (paf != NULL)
	  paf->modifier = 3 *  paf->modifier / 5;
	if (fKiss){
	  send_to_char("Noticing an error in your studies you pull back to reconsider.\n\r", ch);
	  return;
	}
	do_visible(ch, "");
        sprintf(buf, "Help! %s tried to assassinate me!",PERS(ch,victim));
        j_yell(victim,buf);
	multi_hit( ch, victim, obj->value[3]+TYPE_HIT);
      }//end fail.
}//END ASSASIANTE


void do_bodyslam( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;


//Data
    int chance = 4 * get_skill(ch,gsn_bodyslam) / 5;
    int dam = number_range(10 + ch->level/2, ch->size *  ch->level / 2);
    int vic_lag  = PULSE_VIOLENCE;
    int ch_lag = skill_table[gsn_bash].beats;

//bools
    bool fSuccess = FALSE;
    bool fYell = FALSE;
    one_argument(argument,arg);

    if ( chance == 0)
      {
	send_to_char("You don't want to abuse your body like that.\n\r",ch);
	return;
      }

    if (arg[0] == '\0')
      {

        if ((victim  = ch->fighting) == NULL)
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

    if ( victim == ch )
    {
      send_to_char("Self mutilation is not your style.\n\r",ch);
      return;
    }

    if (is_safe(ch,victim))
      return;

    //Get chance to bash.
    chance += get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_DEX);
    chance += GET_AC2(victim,AC_BASH) /25;
    chance -= get_skill(victim,gsn_dodge)/20;
    chance += (ch->level - victim->level);
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    if (IS_AFFECTED2(ch, AFF_RAGE))
	chance += 5;
    if ( victim->race == race_lookup("avian")
	 || victim->race == race_lookup("faerie") )
        chance -=10;
    if (is_affected(victim,gsn_horse))
      chance -= 5;
    if (!IS_NPC(victim) && victim->pcdata->pStallion && number_percent() < get_skill(victim, gsn_mounted)){
      chance -= 12;
      check_improve(victim, gsn_mounted, TRUE, 0);
    }
    chance += affect_by_size(ch,victim);

//check if target should yell.
    fYell = (ch->fighting != victim && victim->fighting != ch);
//sound attack to justice /imm
    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "bodyslam", skill_table[gsn_bodyslam].name))
      return;

    if (number_percent() < chance)
      {
	fSuccess = TRUE;
	vic_lag = URANGE(1, number_range(2, ch->size - victim->size + 3), 4) * PULSE_VIOLENCE;
      }
    else
      {
	dam = 0;
      }

//MANTIS CHECK
    if (is_affected(victim,gsn_mantis) && fSuccess)
      {
	act("You use $n's momentum against him, and throw $m to the ground!",ch,NULL,victim,TO_VICT);
	act("$N reverses your attack, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
	act("$N reversed $n's attack, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
	WAIT_STATE2(ch, URANGE(1, number_range(1, victim->size), 3) * PULSE_VIOLENCE);
	damage(victim,ch,dam,gsn_mantis,DAM_BASH,TRUE);
	affect_strip(victim,gsn_mantis);
	return;
      }//END MATIS

//check for lag protect
    if (!do_lagprot(ch, victim, chance, &dam, &vic_lag, gsn_bodyslam, fSuccess, fYell))
      {
	if (fYell)
	  {
            sprintf(buf, "Help! %s just bodyslammed into me!",PERS(ch,victim));
	    j_yell(victim,buf);
	  }
	if (fSuccess)
	  {
	    act("$n knocks you senseless with a full on bodyslam!",ch,NULL,victim,TO_VICT);
	    act("You slam into $N, knocking $M senseless!",ch,NULL,victim,TO_CHAR);
            act("$n knocks $N senseless with a full on bodyslam.",ch,NULL,victim,TO_NOTVICT);
	  }
      	else
	  {
	    act("You fall flat on your face!", ch,NULL,victim,TO_CHAR);
	    act("$n falls flat on $s face.", ch,NULL,victim,TO_NOTVICT);
	    act("You evade $n's bodyslam, causing $m to fall flat on $s face.",ch,NULL,victim,TO_VICT);
	  }
      }//END IF NO PROTECTION


    if (!fSuccess || dam == 0 || vic_lag == 0)
    {
      check_improve(ch,gsn_bodyslam,FALSE,1);
      ch_lag = 3 * ch_lag / 2;
    }
    else
      check_improve(ch,gsn_bodyslam,TRUE,1);

    WAIT_STATE2(ch, ch_lag);
    if (vic_lag != 0 && dam != 0)
      WAIT_STATE2(victim, vic_lag);
    if (ch->race == race_lookup("fire") && number_percent() < 3 * ch->level / 2)
      damage(ch,victim,3 * dam / 2, gsn_bodyslam, DAM_FIRE,TRUE);
    else
      damage(ch,victim,dam,gsn_bodyslam,DAM_BASH,TRUE);

    if (ch && victim && ch->fighting == victim && dam > 0)
      check_ccombat_actions( victim, ch, CCOMBAT_BASHED);
}

void do_grapple( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;

//Data
    int chance = 3 * get_skill(ch,gsn_grapple) / 4;
    int dam = get_curr_stat(ch,STAT_STR) + ch->level;
    int vic_lag  = PULSE_VIOLENCE;
    int ch_lag = skill_table[gsn_bash].beats;

//bools
    bool fSuccess = FALSE;
    bool fYell = FALSE;
    one_argument(argument,arg);

    if (chance == 0)
      {
	send_to_char("You don't know how to grapple.\n\r",ch);
	return;
      }
    if ((victim = ch->fighting) == NULL)
      {
	send_to_char("But you aren't fighting anyone!\n\r",ch);
	return;
      }
    if (is_safe(ch,victim))
      return;

    //Get chance to grapple
    chance += (get_curr_stat(ch,STAT_STR) - get_curr_stat(victim, STAT_STR)) *2;
    chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX)) *2;
    chance -= get_skill(victim,gsn_dodge)/5;
    chance += (ch->level - victim->level);
    if (is_affected(victim,gsn_horse))
        chance -= 5;
    if ( victim->race == race_lookup("avian") )
        chance -=10;
    if (!IS_NPC(victim) && victim->pcdata->pStallion && number_percent() < get_skill(victim, gsn_mounted)){
      chance -= 12;
      check_improve(victim, gsn_mounted, TRUE, 0);
    }
// bigger people are easier to throw hence - not + affect_by_size
    chance -= affect_by_size(ch,victim);
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);


//check if target should yell.
    fYell = (ch->fighting != victim && victim->fighting != ch);
//sound attack to justice /imm
    a_yell(ch,victim);

    if (number_percent() < chance)
      {
	fSuccess = TRUE;
	vic_lag =  URANGE(1, number_range(1, victim->size), 3)
	  * PULSE_VIOLENCE;
      }
    else
      {
	dam = 0;
      }

//MANTIS CHECK
    if (is_affected(victim,gsn_mantis) && fSuccess)
      {
	act("You reversed $n's grapple, and throw $m to the ground!",ch,NULL,victim,TO_VICT);
	act("$N reverses your grapple, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
	act("$N reversed $n's grapple, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
	WAIT_STATE2(ch, URANGE(1, number_range(1, victim->size), 3) * PULSE_VIOLENCE);
	damage(victim,ch,(get_curr_stat(victim,STAT_STR) + victim->level)/2,gsn_grapple,DAM_BASH,TRUE);
	affect_strip(victim,gsn_mantis);
	return;
      }//END MATIS

//check for lag protect
    if (!do_lagprot(ch, victim, chance, &dam, &vic_lag, gsn_grapple, fSuccess, fYell))
      {
	if (fYell)
	  {
            sprintf(buf, "Help! %s just bodyslammed into me!",PERS(ch,victim));
	    j_yell(victim,buf);
	  }
	if (fSuccess)
	  {
            act("$n maneuvers to grapple you, and throws you to the ground!",ch,NULL,victim,TO_VICT);
	    act("You maneuver to grapple $N, and throw $M to the ground!",ch,NULL,victim,TO_CHAR);
            act("$n maneuvers to grapple $N, and throws $M to the ground.",ch,NULL,victim,TO_NOTVICT);
	  }
      	else
	  {
	    act("$N manages to evade your grapple.", ch,NULL,victim,TO_CHAR);
	    act("$N manages to evade $n's grapple.", ch,NULL,victim,TO_NOTVICT);
	    act("You manage to evade $n's grapple.",ch,NULL,victim,TO_VICT);
	  }
      }//END IF NO PROTECTION


    if (!fSuccess || dam == 0 || vic_lag == 0)
    {
      check_improve(ch,gsn_grapple,FALSE,1);
      ch_lag = 3 * ch_lag / 2;
    }
    else
      check_improve(ch,gsn_grapple,TRUE,1);

    WAIT_STATE2(ch, ch_lag);
    if (vic_lag != 0 && dam != 0)
      WAIT_STATE2(victim, vic_lag);
    damage(ch,victim,dam,gsn_grapple,DAM_BASH,TRUE);

    if (ch && victim && ch->fighting == victim && dam > 0)
      check_ccombat_actions( victim, ch, CCOMBAT_BASHED);
}

void do_charge( CHAR_DATA *ch, char *argument )
{
  char arg[MIL], buf[MSL];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  int chance;
  int dam  = number_range(3 * ch->level / 2, 3 * ch->level);
  bool fAdv = FALSE;

  one_argument( argument, arg );


  if ((chance = get_skill(ch,gsn_charge)) < 2)
    {
      send_to_char("You don't know how to charge someone.\n\r",ch);
      return;
    }
  if (arg[0] == '\0')
    {
      send_to_char("Charge at who?\n\r",ch);
      return;
    }
  if (ch->fighting != NULL)
    {
      send_to_char("Not while you're fighting!\n\r",ch);
      return;
    }
  else if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
      send_to_char("They aren't here.\n\r",ch);
      return;
    }
  if ( victim == ch )
    return;
  if ( is_safe( ch, victim ) )
    return;
  if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
      sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
      return;
    }
  obj = get_eq_char( ch, WEAR_WIELD );
  if ((obj == NULL || (obj->value[0] != WEAPON_SWORD
		       && obj->value[0] !=  WEAPON_SPEAR
		       && obj->value[0] !=  WEAPON_AXE
		       && obj->value[0] != WEAPON_POLEARM))
      && ch->race != race_lookup("minotaur"))
    {
      send_to_char( "You need an appropriate weapon to charge.\n\r", ch );
      return;
    }
  if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
    {
      send_to_char("You can't seem to get up enough speed.\n\r",ch);
      return;
    }
  chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
  if (!IS_NPC(ch)
      && !is_affected(victim, gsn_charge)
      && ch->pcdata->pStallion
      && number_percent() < get_skill(ch, gsn_mounted)
      && (obj->value[0] == WEAPON_SPEAR || obj->value[0] == WEAPON_POLEARM)){
    check_improve(ch, gsn_mounted, TRUE, 0);
    fAdv = TRUE;
    dam += UMAX(0, get_skill(ch, gsn_mounted) - 75) * 3;
    act("Saddled atop $t, $n charges you at full speed!", ch, ch->pcdata->pStallion->short_descr, victim, TO_VICT);
    act("Saddled atop $t, $n charges $N at full speed!", ch, ch->pcdata->pStallion->short_descr, victim, TO_NOTVICT);
    act("Saddled atop $t, you charge $N at full speed!", ch, ch->pcdata->pStallion->short_descr, victim, TO_CHAR);
  }
  if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE) || fAdv)
    chance += 10;
  if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
    chance -= 10;
  chance += (ch->level - victim->level);
  chance += affect_by_size(ch,victim);
  chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
  if (!IS_AWAKE(victim) )
    chance += 10;

  WAIT_STATE2( ch, skill_table[gsn_charge].beats );
  a_yell(ch,victim);

  //predict interdict
  if (predictCheck(ch, victim, "charge", skill_table[gsn_charge].name))
    return;

  if ( number_percent( ) < chance || !IS_AWAKE(victim)){
    check_improve(ch,gsn_charge,TRUE,1);

    sprintf(buf, "Help! %s just charged into me!",PERS(ch,victim));
    j_yell(victim,buf);

    if (ch->race == race_lookup("minotaur"))
      WAIT_STATE2(victim, skill_table[gsn_charge].beats);
    else if (!is_affected(victim, gsn_charge)){
      AFFECT_DATA af;
      if (fAdv)
	WAIT_STATE2(victim, number_range(2, 3) * PULSE_VIOLENCE);
      else
	WAIT_STATE2(victim, skill_table[gsn_charge].beats / 2);

      af.type = gsn_charge;
      af.level = victim->level;
      af.duration = number_range(0, 2);

      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier = 0;
      affect_to_char(victim, &af);
    }
    else
      act("$N avoids your charge and you fail to stun $M.", ch, NULL, victim, TO_CHAR);

    /* counter */
    if ( counter_check(ch, victim, get_skill(victim, gsn_counter), dam, DAM_BASH, gsn_charge)){
      return;
    }

    if (ch->race == race_lookup("minotaur") && ch->class == class_lookup("berserker")){
      act("You charge into $N with your horns and toss him into the air!",ch,NULL,victim,TO_CHAR);
      act("$n charges into $N with $s horns and tosses $m into the air!",ch,NULL,victim,TO_NOTVICT);
      act("$n charges into you with $s horns and tosses you into the air!",ch,NULL,victim,TO_VICT);
      damage(ch,victim,dam,gsn_charge,DAM_BASH,TRUE);

      if (ch->in_room == victim->in_room && obj != NULL && obj->value[0] == WEAPON_AXE)
	{
	  act("You slash at $N with $p while $E is in the air!",ch,obj,victim,TO_CHAR);
	  act("$n slashes at $N with $p while $E is in the air!",ch,obj,victim,TO_NOTVICT);
	  act("$n slashes at you with $p while you are in the air!",ch,obj,victim,TO_VICT);
	  multi_hit(ch,victim,TYPE_UNDEFINED);
	}
    }
    else{
      if (ch->race == race_lookup("fire") && number_percent() < 3 * ch->level / 2)
	damage(ch,victim,3 * dam / 2, gsn_charge, DAM_FIRE, TRUE);
      else
	damage(ch,victim,dam,gsn_charge,DAM_BASH,TRUE);
    }

  }
  else
    {
      check_improve(ch,gsn_charge,FALSE,1);
      sprintf(buf, "Help! %s tried to charge into me!",PERS(ch,victim));
      j_yell(victim,buf);

      damage( ch, victim, 0, gsn_charge,DAM_NONE,TRUE);
    }
}

void do_ambush( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    int chance;
    one_argument( argument, arg );
    if ((chance = get_skill(ch,gsn_ambush)) == 0)
    {
	send_to_char("You don't know how to ambush.\n\r",ch);
	return;
    }
    if (arg[0] == '\0')
    {
        send_to_char("Ambush who?\n\r",ch);
        return;
    }
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("You can't ambush while you are fighting.\n\r", ch);
	return;
    }
    if (victim->hit < ( 4 * victim->max_hit / 5))
    {
        sendf(ch, "%s is hurt and suspicious ... you can't sneak up.\n\r", PERS(victim,ch) );
	return;
    }
    if (!IS_NPC(ch)
	&& ch->in_room->sector_type != SECT_FOREST
	&& ch->in_room->sector_type != SECT_HILLS
	&& ch->in_room->sector_type != SECT_SNOW
	&& ch->in_room->sector_type != SECT_MOUNTAIN
	&& ch->in_room->sector_type != SECT_FIELD){
      send_to_char("You're not comfortable enough here to ambush.\n\r",ch);
      return;
    }
    if (IS_NPC(victim) && IS_SET(victim->act, ACT_TOO_BIG) )
    {
        sendf(ch, "%s is too alert for you to attempt such an act.\n\r", PERS(victim,ch));
	return;
    }
    if ( victim == ch )
	return;
    if ( is_safe( ch, victim ) )
        return;
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
        return;
    }
    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
    {
	send_to_char("You can't seem to get up enough speed.\n\r",ch);
	return;
    }
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_WIS) /2;
    chance -= get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
        chance -= 10;
    chance += (ch->level - victim->level);
    chance += affect_by_size(ch,victim);
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    if (IS_WEREBEAST(ch->race))
	chance += 10;
    if (!IS_AWAKE(victim) )
        chance += 10;
    WAIT_STATE2( ch, skill_table[gsn_ambush].beats );

    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "ambush", skill_table[gsn_ambush].name))
      return;

    if ( number_percent( ) < chance || !IS_AWAKE(victim))
    {
      check_improve(ch,gsn_ambush,TRUE,1);
      sprintf(buf, "Help! %s just ambushed me!",PERS(ch,victim));
      j_yell(victim,buf);
      one_hit( ch, victim, gsn_ambush, FALSE );

      /* check for fired weapons use */
      if (ch->fighting == victim && get_skill(ch, gsn_fired) > 1){
	OBJ_DATA* launcher;
	if ( (launcher = get_eq_char( ch, WEAR_RANGED )) != NULL
	     && IS_WEAPON_STAT( launcher, RANGED_FAST)
	     && get_eq_char( ch, WEAR_QUIVER ) != NULL){
	  do_shoot(ch, "");
	}
      }
    }
    else
      {
        check_improve(ch,gsn_ambush,FALSE,1);
        sprintf(buf, "Help! %s tried to ambush me!",PERS(ch,victim));
        j_yell(victim,buf);
	damage( ch, victim, 0, gsn_ambush,DAM_NONE,TRUE);
      }
}

void do_circle( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  OBJ_DATA *obj, *obj2, *sheath;
  char buf[MIL];
  int chance, circ, wpnswitch;
  int sh_skill = 4 * get_skill(ch, gsn_sheath) / 5;

  if ((chance = get_skill(ch,gsn_circle)) == 0){
    send_to_char("You don't know how to circle.\n\r",ch);
    return;
  }

  /* get target */
  if (IS_NULLSTR(argument)){
    victim = ch->fighting;
    if (victim == NULL){
      send_to_char("But you aren't in combat!\n\r",ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, NULL, argument)) == NULL){
    send_to_char("They aren't here.\n\r",ch);
    return;
  }
  if (victim->fighting == NULL){
    send_to_char("Your victim must be distracted by combat.\n\r", ch);
    return;
  }

  if (!IS_NPC(ch) && (victim->fighting == ch || IS_SET(victim->off_flags,OFF_AREA_ATTACK))){
    send_to_char("You cannot circle while defending yourself.\n\r",ch);
    return;
  }

  if ( is_safe( ch, victim ) )
    return;

  wpnswitch = FALSE;
  obj = get_eq_char( ch, WEAR_WIELD );
  obj2 = get_eq_char( ch, WEAR_SECONDARY);
  sheath = get_eq_char( ch, WEAR_SHEATH_L);

  if (obj == NULL || (obj->value[0] != WEAPON_DAGGER
		      && obj->value[0] != WEAPON_SPEAR)){
    wpnswitch = TRUE;
    if (obj2 == NULL || (obj2->value[0] != WEAPON_DAGGER
			 && obj2->value[0] != WEAPON_SPEAR)){
      send_to_char( "You need to wield a piercing weapon to circle.\n\r", ch );
      return;
    }
  }
  if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE)){
    send_to_char("You can't seem to get up enough speed.\n\r",ch);
    return;
  }

  chance = 4 * chance / 5;
  chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_WIS);
  chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
  if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
    chance += 10;
  if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
    chance -= 10;
  chance += (ch->level - victim->level);
//larger people are easier to hit
  chance -= affect_by_size(ch,victim);
  chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);

  WAIT_STATE2( ch, skill_table[gsn_circle].beats );

  if (IS_NPC(ch))
    circ = attack_lookup("sidestep") + TYPE_NOBLOCKHIT;
  else
    circ = gsn_circle;

  /* yell */
  a_yell(ch, victim);
  if (ch->fighting != victim && victim->fighting != ch){
    sprintf(buf, "Help! %s just put a dagger in my side!",PERS(ch,victim));
    j_yell(victim,buf);
  }
  if ( number_percent( ) < chance ){
    check_improve(ch,gsn_circle,TRUE,1);
    if (!wpnswitch){
      one_hit( ch, victim, circ, FALSE );
      if (ch->fighting != victim)
	return;
    }
    if (wpnswitch){
      if (number_percent( ) < get_skill(ch, gsn_dual_wield)){
	check_improve(ch,gsn_dual_wield,TRUE,1);
	one_hit( ch, victim, circ, TRUE );
	if (ch->fighting != victim)
	  return;
      }
      else{
	check_improve(ch,gsn_dual_wield,FALSE,1);
	damage( ch, victim, 0, circ, DAM_NONE, TRUE);
      }
    }
    /* check for double */
    if (sheath && sheath->value[0] == WEAPON_DAGGER){
      if (!IS_LIMITED(sheath))
	sh_skill /= 2;
      if (number_percent() < sh_skill){
	check_improve(ch, gsn_sheath, TRUE, 1);
	act("You draw $p from you sleeve and aim for the spleen.",
	    ch, sheath, NULL, TO_CHAR);
	act("$n steps to the side as $e draws something from $s sleeves.",
	    ch, sheath, NULL, TO_ROOM);
	circ = gsn_sheath;
	one_hit( ch, victim, circ, TRUE );
      }
      else
	check_improve(ch, gsn_sheath, FALSE, 2);
    }
  }
  else{
    check_improve(ch,gsn_circle,FALSE,1);
    damage( ch, victim, 0, circ,DAM_NONE,TRUE);
  }
}

void do_warcry( CHAR_DATA *ch, char *argument)
{
    int chance;
    char buf[MSL];
    if ((chance = get_skill(ch,gsn_warcry)) == 0)
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }
    if (is_affected(ch,gsn_warcry) || is_affected(ch,skill_lookup("bless")))
    {
	send_to_char("You already feel righteous.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_CALM))
    {
	send_to_char("You can't get worked up in your calm state.\n\r",ch);
	return;
    }
    if (IS_SET(ch->comm,COMM_NOYELL))
    {
	send_to_char("You cannot yell.\n\r",ch);
	return;
    }
    if (ch->mana < 30)
    {
	send_to_char("You can't get up enough energy.\n\r",ch);
	return;
    }
    if (ch->position == POS_FIGHTING)
	chance += 30;
    if (number_percent() < chance)
    {
      char *god = capitalize(ch->pcdata->deity[0] == '\0' ?
			     "Athaekeetha" : ch->pcdata->deity);
    AFFECT_DATA af;
    WAIT_STATE2(ch,PULSE_VIOLENCE);
    ch->mana -= 30;
    act("You feel righteous as you yell out your warcry.",ch,NULL,NULL,TO_CHAR);
    act("$n yells out $s warcry.",ch,NULL,NULL,TO_ROOM);

    if (ch->class == class_lookup("warrior"))
      {
	sprintf(buf, "%s, bless me with your power!", god);
	do_yell(ch,buf);
      }
    else if (ch->class == class_lookup("berserker"))
      {
	sprintf(buf, "%s, I bring forth your fury!", god);
	do_yell(ch,buf);
      }
    else if (ch->class == class_lookup("ranger"))
      {
	god = capitalize(ch->pcdata->deity[0] == '\0' ?
			  "Chueh" : ch->pcdata->deity);
	sprintf(buf, "%s heed my call!", god);
	do_yell(ch,buf);
      }
    else if (ch->class == class_lookup("shaman"))
      {
	god = capitalize(ch->pcdata->deity[0] == '\0' ?
			  "Crypticant" : ch->pcdata->deity);
	sprintf(buf, "%s, bring forth your destruction!", god);
	do_yell(ch,buf);
      }
    else if (ch->class == class_lookup("druid"))
      {
	god = capitalize(ch->pcdata->deity[0] == '\0' ?
			  "Chueh" : ch->pcdata->deity);
	sprintf(buf, "%s, bless this servant of your cause!", god);
	do_yell(ch,buf);
      }
    else{
      sprintf(buf, "%s, grant me your blessing!", god);
      do_yell(ch,buf);
    }
	check_improve(ch,gsn_warcry,TRUE,1);
	af.where        = TO_AFFECTS;
	af.type		= gsn_warcry;
	af.level	= ch->level;
        af.duration     = ch->level /2;
	af.modifier	= UMAX(1, ch->level / 8);;
	af.location	= APPLY_HITROLL;
  	af.bitvector	= 0;
	affect_to_char(ch,&af);
	af.location	= APPLY_DAMROLL;
	af.modifier	= UMAX(1, ch->level / 16);;
	affect_to_char(ch,&af);
	af.modifier	= 0 - (ch->level /8);
	af.location	= APPLY_SAVING_SPELL;
	affect_to_char(ch,&af);
    }
    else
    {
	WAIT_STATE2(ch,2 * PULSE_VIOLENCE);
	ch->mana -= 15;
	act("You grunt softly.",ch,NULL,NULL,TO_CHAR);
	act("$n makes some grunting noises.",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_warcry,FALSE,1);
    }
}

void do_barkskin( CHAR_DATA *ch, char *argument)
{
    int chance;
    if ((chance = get_skill(ch,gsn_barkskin)) == 0)
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }
    if (is_affected(ch,gsn_barkskin))
    {
	send_to_char("Your skin is already as tough as bark.\n\r",ch);
	return;
    }
    if (ch->mana < 50)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_barkskin].beats);
    if (number_percent() < chance)
    {
	AFFECT_DATA af;
	ch->mana -= 50;
	act("Your skin becomes covered with bark.",ch,NULL,NULL,TO_CHAR);
	act("$n's skin becomes covered with bark.",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_barkskin,TRUE,1);
	af.type		= gsn_barkskin;
	af.level	= ch->level;
        af.duration     = 24;
	af.modifier	= - (1.5 * ch->level);
	af.location	= APPLY_AC;
  	af.bitvector	= 0;
	affect_to_char(ch,&af);
    }
    else
    {
	ch->mana -= 25;
	send_to_char("You failed to alter your skin texture.\n\r",ch);
	check_improve(ch,gsn_barkskin,FALSE,1);
    }
}

void do_endure( CHAR_DATA *ch, char *argument)
{
    int chance;
    if ((chance = get_skill(ch,gsn_endure)) == 0)
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }
    if (is_affected(ch,gsn_endure))
    {
	send_to_char("You can't endure any more concentration.\n\r",ch);
	return;
    }
    if (ch->mana < 30)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_endure].beats);
    if (number_percent() < chance)
    {
	AFFECT_DATA af;
	ch->mana -= 30;
	act("You meditate briefly then resume your position.",ch,NULL,NULL,TO_CHAR);
	act("$n meditates briefly then resumes $s position.",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_endure,TRUE,1);
	af.type		= gsn_endure;
	af.level	= ch->level;
        af.duration     = 12;
	af.modifier	= 0 - (chance * ch->level / 500) - 2;
	af.location	= APPLY_SAVING_SPELL;
  	af.bitvector	= 0;
	affect_to_char(ch,&af);
    }
    else
    {
	ch->mana -= 15;
	send_to_char("Something disturbs your concentration and you fail to endure.\n\r",ch);
	check_improve(ch,gsn_endure,FALSE,1);
    }
}
void do_chii( CHAR_DATA *ch, char *argument)
{
    int chance, cost = 20;
    AFFECT_DATA *paf = NULL;
    if ((chance = get_skill(ch,gsn_chii)) == 0)
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }
    if (is_affected(ch,gsn_chii))
    {
	send_to_char("Your body is already in equilibrium.\n\r",ch);
	return;
    }
    if (ch->class == class_lookup("monk"))
    {
    	for ( paf = ch->affected; paf != NULL; paf = paf->next )
    	    if (paf->type == gsn_chii_bolt && paf->modifier >= 3)
    	    {
		send_to_char("Your body cannot circulate more chii.\n\r",ch);
    	        return;
    	    }
    }
    if (paf != NULL)
	cost += paf->modifier * 10;
    if (ch->mana < cost)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_chii].beats);
    if (number_percent() < chance)
    {
	AFFECT_DATA af;
	ch->mana -= cost;
	act("You focus inward and regulate your body's circulation.",ch,NULL,NULL,TO_CHAR);
	act("$n closes $s eyes for a moment and regulates $s breathing.",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_chii,TRUE,1);
	af.type		= gsn_chii;
	af.level	= ch->level;
        af.duration     = 24;
	af.modifier	=  - 2 * ch->level / 3;
	af.location	= APPLY_AC;
  	af.bitvector	= 0;
	affect_to_char(ch,&af);
    }
    else
    {
	ch->mana -= 10;
	send_to_char("Something disturbs your meditation.\n\r",ch);
	check_improve(ch,gsn_chii,FALSE,1);
    }
}

void do_swing( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim, *vch, *vch_next;
    OBJ_DATA *obj, *obj2;
    int success, chance;
    if ((chance = get_skill(ch, gsn_swing)) == 0)
    {
	send_to_char("You'd feel silly swinging your weapon about like a barbarian.\n\r",ch);
	return;
    }
    if ((victim = ch->fighting) == NULL)
    {
	send_to_char("But you aren't fighting anyone!\n\r",ch);
	return;
    }
    obj = get_eq_char( ch, WEAR_WIELD );
    obj2 = get_eq_char( ch, WEAR_SECONDARY );
    if (obj == NULL && !IS_NPC(ch) )
    {
	send_to_char("You need a weapon to swing around.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
    {
	send_to_char("You can't seem to get up enough speed.\n\r",ch);
	return;
    }
    if (IS_AFFECTED2(ch, AFF_RAGE) || is_affected(ch, gsn_windmill))
    {
	send_to_char("You are already fighting with reckless abandonment.\n\r",ch);
	return;
    }
    WAIT_STATE2( ch, skill_table[gsn_swing].beats );
    act("You let loose a flurry of wild swings.",ch,NULL,NULL,TO_CHAR);
    act("$n lets loose a flurry of wild swings!",ch,NULL,NULL,TO_ROOM);
    for (vch = ch->in_room->people; vch !=NULL; vch = vch_next)
    {
	vch_next = vch->next_in_room;
        if (vch->fighting == ch && vch != victim)
	{
	    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(vch,STAT_DEX);
	    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
		chance += 10;
	    if (IS_SET(vch->off_flags,OFF_FAST) || IS_AFFECTED(vch,AFF_HASTE))
	        chance -= 10;
	    chance += (ch->level - vch->level);
    	    success = number_percent ( );
	    if ( success < chance )
	    {
		check_improve(ch,gsn_swing,TRUE,1);
		one_hit(ch, vch, IS_NPC(ch) ? TYPE_UNDEFINED : TYPE_HIT + obj->value[3], FALSE);
		if (success < chance /2 && obj2 != NULL && success < get_skill(ch,gsn_dual_wield))
		{
		    check_improve(ch,gsn_swing,TRUE,1);
		    if (IS_NPC(ch) && !obj2)
		      one_hit(ch, vch, TYPE_HIT + ch->dam_type, TRUE);
		    else
		      one_hit(ch, vch, TYPE_HIT + obj2->value[3], TRUE);
		}
	    }
	    else
	    {
		check_improve(ch, gsn_swing, FALSE, 1);
		damage(ch, vch, 0, IS_NPC(ch) ? TYPE_UNDEFINED : TYPE_HIT + obj->value[3], DAM_NONE, TRUE);
	    }
	}
    }
}

void do_headbutt( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int chance, success;
    //cosnt
    const int str_med = 18;
    const int str_mod = 10;
    //bool
    bool fRage = FALSE;

    if ( (chance = get_skill(ch,gsn_headbutt)) == 0)
    {
        send_to_char("Ouch!.\n\r", ch );
	return;
    }
    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char( "You aren't fighting anyone.\n\r", ch );
        return;
    }
    chance = 4 * chance /5;
    success = number_percent( );
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += (ch->level - victim->level);
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    if (IS_AFFECTED2(ch, AFF_RAGE))
      {fRage = TRUE;success -= 20;}
//smaller people are easier to haymaker
    success -= affect_by_size(ch, victim) * 2;
    chance += affect_by_size(ch,victim);
    WAIT_STATE2( ch, skill_table[gsn_headbutt].beats );

    if ( success < chance)
      {
	int shield = get_lagprot(victim);
	if (shield == LAG_SHIELD_PROT || shield == LAG_SHIELD_FFIELD)
	  {
	    //check for force of the blow.
	    if(number_percent() < ((fRage ? 20 : 0) + (get_curr_stat(ch, STAT_STR) - str_med) * str_mod))
	      {
		act("Lacking the momentum neccessary you fail to penetrate $N's shield.", ch, NULL, victim, TO_CHAR);
		act("$n's headbutt bounces off of the shield!", ch, NULL, victim, TO_ROOM);
		success = 100;
	      }
	    else
	      {
		act("Striking with great force your $N's shield.", ch, NULL, victim, TO_CHAR);
		act("$n's headbutt goes right through the shield!", ch, NULL, victim, TO_ROOM);
	      }
	  }

	damage(ch,victim,number_range( ch->level / 2, UMAX(0, 70 - success)), gsn_headbutt,DAM_BASH,TRUE);
	check_improve(ch,gsn_headbutt,TRUE,8);

	if (IS_SET(victim->act, ACT_UNDEAD)
	    || victim->class == class_lookup("vampire")
	    || victim->class == class_lookup("lich")
	    || check_immune(victim, DAM_BASH, FALSE) == IS_IMMUNE)
	  {
	    act("$N seems to shake off the blow!", ch, NULL, victim, TO_NOTVICT);
	    act("You easly shake off the blow.", ch, NULL, victim, TO_VICT);
	    act("$N seems to not even notice the blow.", ch, NULL, victim, TO_ROOM);

	    chance = 0;
	  }
	if (success < chance)
	  {
	    act("You feel dizzy from the massive blow to the head.",victim, NULL, NULL, TO_CHAR);
	    act("$n looks dizzy from the blow to the head.", victim, NULL, NULL, TO_ROOM);
	    if (!is_affected(victim, gsn_headbutt))
	      {
		af.type		= gsn_headbutt;
		af.level		= ch->level;
		af.duration     	= 0;
		af.modifier		= 0;
		af.location		= APPLY_NONE;
		af.bitvector	= 0;
		affect_to_char(victim,&af);
	      }
	  }
      }//end if success
    else
      {
	damage( ch, victim, 0, gsn_headbutt,DAM_BASH,TRUE);
	check_improve(ch,gsn_headbutt,FALSE,8);
      }
}//end do_headbutt

void do_keen_sight( CHAR_DATA *ch, char *argument )
{
    int chance;
    AFFECT_DATA af;
    if ( (chance = get_skill(ch,gsn_keen_sight)) == 0 )
    {
        send_to_char("Huh?\n\r",ch);
	return;
    }
    if (ch->mana < 10)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    if ( is_affected(ch, gsn_keen_sight) )
    {
        send_to_char("You vision is already as sharp as it can get.\n\r",ch);
        return;
    }
    WAIT_STATE2(ch,skill_table[gsn_keen_sight].beats);
    if (number_percent() > chance)
    {
	ch->mana -= 5;
	send_to_char("You squint your eyes, but fail to adjust your vision.\n\r",ch);
        check_improve(ch,gsn_keen_sight,FALSE,1);
	return;
    }
    ch->mana -= 10;
    af.where	 = TO_AFFECTS2;
    af.type      = gsn_keen_sight;
    af.level     = ch->level;
    af.duration  = ch->level;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_KEEN_SIGHT;
    affect_to_char( ch, &af );
    send_to_char( "Your vision sharpens.\n\r", ch );
    check_improve(ch,gsn_keen_sight,TRUE,1);
}

void do_pilfer( CHAR_DATA *ch, char *argument ){

  CHAR_DATA *victim;
  OBJ_DATA *obj;
  char arg[MIL];
  int chance, roll = number_percent();
  if ( (chance = get_skill(ch,gsn_pilfer)) == 0)
    {
      send_to_char("You are not skilled enough to pilfer.\n\r", ch );
      return;
    }
  if ( ( victim = ch->fighting ) == NULL )
    {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
    }
  one_argument( argument, arg );
  if ( arg[0] == '\0' )
    {
      send_to_char( "Pilfer what?\n\r", ch );
      return;
    }
  if ( IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG) )
    {
      sendf(ch, "%s is too alert for you to pilfer from them.\n\r", PERS2(victim));
      return;
    }
  if (!can_see(victim,ch))
    chance += 25;
  chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX));
  chance += (ch->level - victim->level);
  chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));

  if (!can_see(ch,victim))
    chance /= 3;
  chance = URANGE(0,chance,75);
  WAIT_STATE2( ch, skill_table[gsn_pilfer].beats );

  if ( roll < chance && !(ch->level + 8 < victim->level))
    {
      damage(ch,victim,number_range( 2*ch->level/3,3*ch->level/2), gsn_pilfer,DAM_NONE,TRUE);
      if ( ( obj = get_obj_carry( victim, arg, ch ) ) == NULL ){
	send_to_char( "You can't find it.\n\r", ch );
	return;
      }
      if ( !can_drop_obj( ch, obj ) || IS_SET(obj->extra_flags, ITEM_INVENTORY) )
    	{
	  send_to_char( "You can't knock it loose.\n\r", ch );
	  return;
    	}
      act("You knock $p loose from $N's inventory.",ch,obj,victim,TO_CHAR);
      act("$n knocks $p loose from $N's inventory.",ch,obj,victim,TO_NOTVICT);
      act("$n knock $p loose from your inventory.",ch,obj,victim,TO_VICT);
      obj_from_char( obj );
      obj_to_room( obj, victim->in_room );
      if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj))
	get_obj(victim,obj,NULL);
      check_improve(ch,gsn_pilfer,TRUE,1);
    }
  else if ( roll < chance*2 )
    {
      damage(ch,victim,number_range( ch->level/2, 6*ch->level/5), gsn_pilfer,DAM_NONE,TRUE);
      check_improve(ch,gsn_pilfer,TRUE,1);
    }
  else
    {
      damage( ch, victim, 0, gsn_pilfer,DAM_NONE,TRUE);
      check_improve(ch,gsn_pilfer,FALSE,1);
    }
}


void do_thrust( CHAR_DATA *ch, char *argument )
{

    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;

//Data
    int chance = 4 * get_skill(ch,gsn_thrust) /5;
    int dam = number_range(10 + ch->level/2,(2 * ch->level));
    int vic_lag  = PULSE_VIOLENCE;
    int ch_lag = skill_table[gsn_thrust].beats;

//bools
    bool fSuccess = FALSE;
    bool fYell = FALSE;
    bool fElder = get_skill(ch, gsn_unholy_gift) > 1;

//modifiers.
    int size = 0;

    if (is_affected(ch, gsn_wolf_form)){
      do_tackle( ch, argument );
      return;
    }
    one_argument(argument,arg);
    if (chance == 0)
    {
	send_to_char("Thrust? What's that?\n\r",ch);
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

    if ( victim == ch )
    {
      send_to_char("Self mutilation is not your style.\n\r",ch);
      return;
    }

    if (is_safe(ch,victim))
	return;

    chance += get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_DEX);
    chance += GET_AC2(victim,AC_BASH) /25;
    chance -= get_skill(victim,gsn_dodge)/20;
    chance += ch->level - victim->level;
    size = affect_by_size(ch,victim);

    if (fElder && size < 0)
      size = 0;
    chance +=  size;

    if (is_affected(victim,gsn_horse))
      chance -= 10;
    if ( victim->race == race_lookup("avian")
	 || victim->race == race_lookup("faerie") )
      chance -= 10;
    if ( victim->class == class_lookup("thief") )
      chance -= 5;
    if (!IS_NPC(victim) && victim->pcdata->pStallion){
      if(number_percent() < get_skill(victim, gsn_mounted)){
	chance -= 12;
	check_improve(victim, gsn_mounted, TRUE, 0);
      }
    }
    //check if target should yell.
      fYell = (ch->fighting != victim && victim->fighting != ch);

      a_yell(ch,victim);
      //predict interdict
      if (predictCheck(ch, victim, "thrust", skill_table[gsn_thrust].name))
	return;

      act("You extend an open palm toward $N. and a powerful force hurls at $M!",ch,NULL,victim,TO_CHAR);
      act("$n extends an open palm toward $N.",ch,NULL,victim,TO_NOTVICT);
      act("$n extends an open palm toward you, and a powerful force hurls at you!",ch,NULL,victim,TO_VICT);

    if (number_percent() < chance)
      {
	vic_lag  = number_range(2,4) * PULSE_VIOLENCE;
	fSuccess = TRUE;
      }
    else
      {
	dam = 0;
      }

//MANTIS CHECK
    if (is_affected(victim,gsn_mantis) && fSuccess)
      {
	act("You use $n's momentum against him, and throw $m to the ground!",ch,NULL,victim,TO_VICT);
	act("$N reverses your attack, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
	act("$N reversed $n's attack, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
	WAIT_STATE2(ch, URANGE(1, number_range(1, victim->size), 3) * PULSE_VIOLENCE);
	damage(victim,ch,dam,gsn_mantis,DAM_BASH,TRUE);
	affect_strip(victim,gsn_mantis);
	return;
      }//END MANTIS

//check for lag protect
    if (!do_lagprot(ch, victim, chance, &dam, &vic_lag, gsn_thrust, fSuccess, fYell))
      {
	if (fYell)
	  {
            sprintf(buf, "Help! %s just knocked me flying!",PERS(ch,victim));
            j_yell(victim,buf);
	  }
	if (fSuccess)
	  {

        act("$n knocks you flying with a powerful thrust!",ch,NULL,victim,TO_VICT);
     	  act("Your thrust knocks $N flying!",ch,NULL,victim,TO_CHAR);
        act("$n knocks $N flying with a powerful thrust.",ch,NULL,victim,TO_NOTVICT);
	 }
      	else
	  {
        act("$N evades your thrust!", ch,NULL,victim,TO_CHAR);
        act("$N evades $n's thrust.", ch,NULL,victim,TO_NOTVICT);
        act("You evade $n's thrust.",ch,NULL,victim,TO_VICT);
	  }
      }//END IF NO PROTECTION


    if (!fSuccess || dam == 0 || vic_lag == 0)
    {
      check_improve(ch, gsn_thrust, FALSE, 1);
      ch_lag = 3 * ch_lag / 2;
    }
    else{
      check_improve(ch, gsn_thrust, TRUE, 1);
      if (!IS_NPC(victim))
	victim->pcdata->pStallion = NULL;
    }

    WAIT_STATE2(ch, ch_lag);
    if (vic_lag != 0 && dam != 0)
      WAIT_STATE2(victim, vic_lag);
    damage(ch,victim,dam,gsn_thrust,DAM_BASH,TRUE);

    if (ch && victim && ch->fighting == victim && dam > 0)
      check_ccombat_actions( victim, ch, CCOMBAT_BASHED);
}

void disarm_offhand( CHAR_DATA *ch, CHAR_DATA *victim ){

  OBJ_DATA *obj, *wield;
  bool fSeize = FALSE;
  bool fExoSeize = FALSE;

  if ( ( obj = get_eq_char( victim, WEAR_SECONDARY ) ) == NULL )
    return;
  if ( IS_STONED(victim) || IS_OBJ_STAT(obj,ITEM_NOREMOVE) || is_affected(victim, gsn_graft_weapon))
    {
      act("$S's secondary weapon won't budge!",ch,NULL,victim,TO_CHAR);
      act("$n tries to disarm you, but your secondary weapon won't budge!",ch,NULL,victim,TO_VICT);
      act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
      return;
    }
  act( "$n disarms you and sends your secondary weapon flying!", ch, NULL, victim, TO_VICT    );
  act( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
  act( "$n disarms $N!",  ch, NULL, victim, TO_NOTVICT );
  obj_from_char( obj );

  /* check for advanced disarms */
  if ( (wield = get_eq_char(ch, WEAR_WIELD)) != NULL
     && (wield->value[0] == WEAPON_EXOTIC || wield->value[0] == WEAPON_WHIP)){
    int sn = skill_lookup("weapon seize");
    if (number_percent() < get_skill(ch, sn)){
      check_improve(ch, sn, TRUE, 0);
      fSeize = TRUE;
      /* check for exotic seize */
      if (wield->value[0] == WEAPON_EXOTIC
	  && get_skill(ch, gsn_exotic_mastery) > 1)
	fExoSeize = TRUE;
    }
    else
      check_improve(ch, sn, FALSE, -99);
  }
  if (!fExoSeize
      && (IS_OBJ_STAT(obj,ITEM_NODROP) || IS_OBJ_STAT(obj,ITEM_INVENTORY)) )
    obj_to_char( obj, victim );
  else if (fSeize){
    act("With a flick of $p $n grabs the weapon.", ch, wield, NULL, TO_ROOM);
    act("With a flick of $p you grab the weapon.", ch, wield, NULL, TO_CHAR);
    obj_to_char( obj, ch );
  }
  else{
    obj_to_room( obj, victim->in_room );
    if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj))
      get_obj(victim,obj,NULL);
  }
}

void do_offhand_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *obj2;

    int chance, hth = 0, ch_weapon, vict_weapon, ch_vict_weapon;

    if ((chance = get_skill(ch,gsn_offhand_disarm)) == 0)
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }
    if ( (obj = get_eq_char( ch, WEAR_WIELD )) == NULL
	 && ( obj = get_eq_char( ch, WEAR_SECONDARY )) == NULL
	 && ((hth = get_skill(ch,gsn_hand_to_hand)) == 0
	     || (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_DISARM))))
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( ( obj2 = get_eq_char( victim, WEAR_SECONDARY ) ) == NULL )
    {
	sendf(ch, "%s is not wielding a secondary weapon.\n\r", PERS(victim,ch) );
	return;
    }

    ch_weapon = get_weapon_skill_obj(ch, obj);
    vict_weapon = get_weapon_skill_obj(victim, obj2);
    ch_vict_weapon = get_weapon_skill_obj(ch, obj2);

    if (IS_NPC(victim))
	vict_weapon = UMAX(victim->level * 2, 75);
    if ( get_eq_char(ch,WEAR_WIELD) == NULL)
	chance = chance * hth/150;
    else
	chance = chance * ch_weapon/100;

    chance += get_weapon_obj_rating( obj, WEAPON_WITHDIS ) * 4;
    chance += get_weapon_obj_rating( obj2, WEAPON_TOBEDIS ) * 4;
    chance += (ch_vict_weapon - vict_weapon) / 2;

    chance += 2*(get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_STR));
    chance += (ch->level - victim->level);
    chance -= get_skill(victim,gsn_dual_parry)/10;
    if (IS_NPC(victim))
	chance -= victim->level/10;
    chance += 2*(get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    if (ch->class == class_lookup("monk") && is_affected(ch,gsn_dragon) && is_empowered(ch,0))
	chance += 25;
    if (number_percent() < chance && (victim->level - ch->level) < 10)
    {
      int skill = get_skill(victim, gsn_double_grip);
      bool fStr = FALSE;
      if (is_affected(victim, gsn_istrength) && has_twohanded(victim) != NULL){
	fStr = TRUE;
	skill = 110;
      }
      WAIT_STATE2( ch, skill_table[gsn_offhand_disarm].beats );
      if ( (is_affected(victim,gsn_double_grip) || fStr)
	   && number_percent() < ((4 * skill / 3) - 75) ){
	act("You fail to disarm $N's weapon from $S powerfull grasp.",ch,NULL,victim,TO_CHAR);
	act("$n tries to disarm you, but your grip is too strong.",ch,NULL,victim,TO_VICT);
	act("$n tries to disarm $N, but $E grips $S weapon tightly.",ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_offhand_disarm,FALSE,1);
	check_improve(victim,gsn_double_grip, FALSE, -99);
	return;
      }
      disarm_offhand( ch, victim );
      check_improve(ch,gsn_offhand_disarm,TRUE,1);
    }
    else
    {
	WAIT_STATE2(ch,skill_table[gsn_offhand_disarm].beats);
	act("You fail to disarm $N.",ch,NULL,victim,TO_CHAR);
	act("$n tries to disarm you, but fails.",ch,NULL,victim,TO_VICT);
	act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_offhand_disarm,FALSE,1);
    }
}



//03-25 xxxxx Viri: Modifed do_spells(..) to peoperly calculate manna costs
//using _mana_cost(..) from magic.c.
//05-02-00 Viri: rewrote do_assasinate
//05-30-00 Viri: Added get_lagshield
