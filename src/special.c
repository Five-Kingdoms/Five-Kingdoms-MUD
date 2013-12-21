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
#include "cabal.h"
#include "recycle.h"
#include "interp.h"
#include "misc.h"

#define MARK(room)	(SET_BIT(	(room)->room_flags, ROOM_MARK) )
#define UNMARK(room)	(REMOVE_BIT(	(room)->room_flags, ROOM_MARK) )
#define IS_MARKED(room)	(IS_SET(	(room)->room_flags, ROOM_MARK) )

extern char *target_name;
extern char *flag_string               args ( ( const struct flag_type *flag_table, int bits ) );

DECLARE_SPEC_FUN(	spec_breath_any		);
DECLARE_SPEC_FUN(	spec_breath_acid	);
DECLARE_SPEC_FUN(	spec_breath_fire	);
DECLARE_SPEC_FUN(	spec_breath_frost	);
DECLARE_SPEC_FUN(	spec_breath_gas		);
DECLARE_SPEC_FUN(	spec_breath_lightning	);
DECLARE_SPEC_FUN(	spec_breath_death	);
DECLARE_SPEC_FUN(	spec_cast_adept		);
DECLARE_SPEC_FUN(	spec_cast_cleric	);
DECLARE_SPEC_FUN(	spec_cast_mage		);
DECLARE_SPEC_FUN(	spec_cast_undead	);
DECLARE_SPEC_FUN(	spec_executioner	);
DECLARE_SPEC_FUN(	spec_special_guard	);
DECLARE_SPEC_FUN(	spec_fido		);
DECLARE_SPEC_FUN(	spec_guard		);
DECLARE_SPEC_FUN(	spec_gate_guard		);
DECLARE_SPEC_FUN(	spec_janitor		);
DECLARE_SPEC_FUN(	spec_poison		);
DECLARE_SPEC_FUN(	spec_thief		);
DECLARE_SPEC_FUN(	spec_nasty		);
DECLARE_SPEC_FUN(	spec_cast_cabal_healer	);
DECLARE_SPEC_FUN(	spec_keeper_stay	);
DECLARE_SPEC_FUN(	spec_keeper_move	);
DECLARE_SPEC_FUN(	spec_virgil_guard	);
DECLARE_SPEC_FUN(	spec_shadowdemon	);
DECLARE_SPEC_FUN(	spec_mob_call		);
DECLARE_SPEC_FUN(	spec_magic_eye		);
DECLARE_SPEC_FUN(       spec_displacer_beast    );
DECLARE_SPEC_FUN(       spec_hunt_wanted	);
DECLARE_SPEC_FUN(       spec_vanguard		);
DECLARE_SPEC_FUN(       spec_servant_hide	);
DECLARE_SPEC_FUN(       spec_justice_sentinel		);
DECLARE_SPEC_FUN(       spec_hunter_nonallied		);
DECLARE_SPEC_FUN(       spec_hunter_nonallied_warp	);
DECLARE_SPEC_FUN(       spec_hunter_nonallied_cwarp	);
DECLARE_SPEC_FUN(       spec_hunter_allied		);
DECLARE_SPEC_FUN(       spec_hunter_allied_warp		);
DECLARE_SPEC_FUN(       spec_hunter_allied_cwarp	);
DECLARE_SPEC_FUN(       spec_terminator			);
DECLARE_SPEC_FUN(       spec_knight_seer		);
DECLARE_SPEC_FUN(       spec_nexus_nemesis		);
DECLARE_SPEC_FUN(       spec_path_walk			);
DECLARE_SPEC_FUN(       spec_raider_actions		);
DECLARE_SPEC_FUN(       spec_awakenlife_actions		);

const   struct  spec_type    spec_table[] =
{
    {	"spec_breath_any",		spec_breath_any		},
    {	"spec_breath_acid",		spec_breath_acid	},
    {	"spec_breath_fire",		spec_breath_fire	},
    {	"spec_breath_frost",		spec_breath_frost	},
    {	"spec_breath_gas",		spec_breath_gas		},
    {	"spec_breath_lightning",	spec_breath_lightning	},	
    {	"spec_breath_death",		spec_breath_death	},	
    {	"spec_cast_adept",		spec_cast_adept		},
    {	"spec_cast_cleric",		spec_cast_cleric	},
    {	"spec_cast_mage",		spec_cast_mage		},
    {	"spec_cast_undead",		spec_cast_undead	},
    {	"spec_executioner",		spec_executioner	},
    {	"spec_special_guard",		spec_special_guard	},
    {	"spec_fido",			spec_fido		},
    {	"spec_guard",			spec_guard		},
    {	"spec_gate_guard",		spec_gate_guard		},
    {	"spec_janitor",			spec_janitor		},
    {	"spec_poison",			spec_poison		},
    {	"spec_thief",			spec_thief		},
    {	"spec_nasty",			spec_nasty		},
    {	"spec_cast_cabal_healer",	spec_cast_cabal_healer	},
    {   "spec_keeper_stay",		spec_keeper_stay	},
    {   "spec_keeper_move",		spec_keeper_move	},
    {   "spec_virgil_guard",		spec_virgil_guard	},
    {   "spec_shadowdemon",		spec_shadowdemon	},
    {   "spec_mob_call",		spec_mob_call		},
    {   "spec_displacer_beast",         spec_displacer_beast    },
    {   "spec_hunt_wanted",		spec_hunt_wanted	},
    {   "spec_vanguard",		spec_vanguard		},
    {   "spec_servant_hide",		spec_servant_hide	},
    {   "spec_justice_sentinel",	spec_justice_sentinel	},
    {   "spec_hunter_nonallied",	spec_hunter_nonallied	},
    {   "spec_hunter_nonallied_warp",	spec_hunter_nonallied_warp	},
    {   "spec_hunter_nonallied_cwarp",	spec_hunter_nonallied_cwarp	},
    {   "spec_hunter_allied",		spec_hunter_allied		},
    {   "spec_hunter_allied_warp",	spec_hunter_allied_warp		},
    {   "spec_hunter_allied_cwarp",	spec_hunter_allied_cwarp	},
    {   "spec_terminator",		spec_terminator			},
    {   "spec_knight_seer",		spec_knight_seer		},
    {   "spec_nexus_nemesis",		spec_nexus_nemesis		},
    {   "spec_magic_eye",		spec_magic_eye			},
    {   "spec_raider_actions",		spec_raider_actions		},
    {   "spec_awakenlife_actions",	spec_awakenlife_actions		},
    {	NULL,				NULL				}
};

SPEC_FUN *spec_lookup( const char *name )
{
   int i;
   for ( i = 0; spec_table[i].name != NULL; i++)
        if (LOWER(name[0]) == LOWER(spec_table[i].name[0]) && !str_prefix( name,spec_table[i].name))
            return spec_table[i].function;
    return 0;
}

char *spec_name( SPEC_FUN *function)
{
    int i;
    for (i = 0; spec_table[i].function != NULL; i++)
        if (function == spec_table[i].function)
	    return spec_table[i].name;
    return NULL;
}

bool spec_nasty( CHAR_DATA *ch )
{
    CHAR_DATA *victim, *v_next;
    long gold;
    if (!IS_AWAKE(ch))
        return FALSE;
    if (ch->position != POS_FIGHTING)
    {
        for ( victim = ch->in_room->people; victim != NULL; victim = v_next)
        {
            v_next = victim->next_in_room;
            if (!IS_NPC(victim) && (victim->level > ch->level) && (victim->level < ch->level + 10))
            {
                do_backstab(ch,victim->name);
                if (ch->position != POS_FIGHTING)
                    do_murder(ch,victim->name);
                return TRUE;
            }
        }
        return FALSE;
    }
    if ( (victim = ch->fighting) == NULL)
        return FALSE;
    switch ( number_bits(2) )
    {
    case 0:
        act( "$n rips apart your coin purse, spilling your gold!", ch, NULL, victim, TO_VICT);
        act( "You slash apart $N's coin purse and gather $S gold.", ch, NULL, victim, TO_CHAR);
        act( "$N's coin purse is ripped apart!", ch, NULL, victim, TO_NOTVICT);
        gold = victim->gold / 10;
        victim->gold -= gold;
        ch->gold += gold;
        return TRUE;
    case 1:
        do_flee( ch, "");
        return TRUE;
    default: return FALSE;
    }
}

bool dragon( CHAR_DATA *ch, char *spell_name )
{
    CHAR_DATA *victim, *v_next;
    int sn;
    if ( ch->position != POS_FIGHTING )
	return FALSE;
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 3 ) == 0 )
	    break;
    }
    if ( victim == NULL )
	return FALSE;
    if ( ( sn = skill_lookup( spell_name ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim, TARGET_CHAR);
    return TRUE;
}

bool spec_breath_any( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING )
	return FALSE;
    switch ( number_bits( 3 ) )
    {
    case 0: return spec_breath_fire		( ch );
    case 1:
    case 2: return spec_breath_lightning	( ch );
    case 3: return spec_breath_gas		( ch );
    case 4: return spec_breath_acid		( ch );
    case 5: return spec_breath_death		( ch );
    case 6:
    case 7: return spec_breath_frost		( ch );
    }
    return FALSE;
}

bool spec_breath_acid( CHAR_DATA *ch )
{
    return dragon( ch, "acid breath" );
}

bool spec_breath_death( CHAR_DATA *ch )
{
    return dragon( ch, "death breath" );
}

bool spec_breath_fire( CHAR_DATA *ch )
{
    return dragon( ch, "fire breath" );
}

bool spec_breath_frost( CHAR_DATA *ch )
{
    return dragon( ch, "frost breath" );
}

bool spec_breath_gas( CHAR_DATA *ch )
{
    int sn;
    if ( ch->position != POS_FIGHTING )
	return FALSE;
    if ( ( sn = skill_lookup( "gas breath" ) ) < 0 )
	return FALSE;
    if (number_bits( 3 ) != 0 ) 
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, NULL,TARGET_CHAR);
    return TRUE;
}

bool spec_breath_lightning( CHAR_DATA *ch )
{
    return dragon( ch, "lightning breath" );
}

bool spec_cast_adept( CHAR_DATA *ch )
{
    CHAR_DATA *victim, *v_next;
    if ( !IS_AWAKE(ch) )
	return FALSE;
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 && !IS_NPC(victim) && victim->level < 11)
	    break;
    }
    if ( victim == NULL )
	return FALSE;
    switch ( number_bits( 4 ) )
    {
    case 0:
	act( "$n utters the word 'abrazak'.", ch, NULL, NULL, TO_ROOM );
	spell_armor( skill_lookup( "armor" ), victim->level,ch,victim,TARGET_CHAR);
	return TRUE;
    case 1:
	act( "$n utters the word 'fido'.", ch, NULL, NULL, TO_ROOM );
	spell_bless( skill_lookup( "bless"),ch->level/3,ch,victim,TARGET_CHAR);
	return TRUE;
    case 2:
	act("$n utters the words 'judicandus noselacri'.",ch,NULL,NULL,TO_ROOM);
        spell_cure_blindness( skill_lookup( "cure blindness" ), ch->level, ch, victim,TARGET_CHAR);
	return TRUE;
    case 3:
	act("$n utters the words 'judicandus dies'.", ch,NULL, NULL, TO_ROOM );
        spell_cure_light( skill_lookup( "cure light" ), victim->level,ch,victim,TARGET_CHAR);
	return TRUE;
    case 4:
	act( "$n utters the words 'judicandus sausabru'.",ch,NULL,NULL,TO_ROOM);
        spell_cure_poison( skill_lookup( "cure poison" ), ch->level, ch, victim,TARGET_CHAR);
	return TRUE;
    case 5:
	act("$n utters the word 'candusima'.", ch, NULL, NULL, TO_ROOM );
	spell_refresh(skill_lookup("refresh"),ch->level/2,ch,victim,TARGET_CHAR);
	return TRUE;
    case 6:
	act("$n utters the words 'judicandus eugzagz'.",ch,NULL,NULL,TO_ROOM);
        spell_cure_disease(skill_lookup("cure disease"), ch->level,ch,victim,TARGET_CHAR);
    }
    return FALSE;
}

bool spec_cast_cabal_healer( CHAR_DATA *ch )
{
    CHAR_DATA *victim, *v_next;
    if ( !IS_AWAKE(ch) )
	return FALSE;
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 && !IS_NPC(victim) 
	     && _is_cabal(victim->pCabal, ch->pCabal ))
	  break;
    }
    if ( victim == NULL )
	return FALSE;
    switch ( number_bits( 3 ) )
    {
    case 0:
        if ( !is_affected(victim, 4) )
	    return FALSE;
	if ( ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC))
	  send_to_char( "The Medic smears ointment over your eyes.\n\r",victim);
	else
	  act("$n utters the words 'judicandus noselacri'.",ch,NULL,NULL,TO_ROOM);
        spell_cure_blindness( skill_lookup( "cure blindness" ),ch->level, ch, victim,TARGET_CHAR);
	return TRUE;
    case 1:
	if ( !is_affected(victim, skill_lookup( "poison" )) ) 
	    return FALSE;
	if ( ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC))
	  send_to_char( "The Medic drains the venom from your veins.\n\r", victim);
	else
	  act( "$n utters the words 'judicandus sausabru'.",ch,NULL,NULL,TO_ROOM);
        spell_cure_poison( skill_lookup( "cure poison" ), ch->level, ch, victim,TARGET_CHAR);
	return TRUE;
    case 2:
      if ( ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC))
	return FALSE;
      act("$n utters the word 'candusima'.", ch, NULL, NULL, TO_ROOM);
      spell_refresh(skill_lookup("refresh"),victim->level,ch,victim,TARGET_CHAR);
	return TRUE;
    case 3:
	if ( !is_affected(victim, skill_lookup( "plague" )) )
	    return FALSE;
	if ( ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC))
	  send_to_char( "The Medic reduces your fever with some mixed herbs.\n\r", victim);
	else
	    act("$n utters the words 'judicandus eugzagz'.",ch,NULL,NULL,TO_ROOM);
        spell_cure_disease(skill_lookup("cure disease"), ch->level,ch,victim,TARGET_CHAR);
	return TRUE;
    case 4:
	if ( ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC))
	    return FALSE;
	if ( is_affected(victim, skill_lookup("armor")) )
	    return FALSE;
	act( "$n utters the word 'abrazak'.", ch, NULL, NULL, TO_ROOM );
	spell_armor( skill_lookup( "armor" ),victim->level,ch,victim,TARGET_CHAR);
	return TRUE;
    case 5:
      if ( ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC))
	return FALSE;
      act("$n utters the words 'judicandus dies'.", ch,NULL, NULL, TO_ROOM );
        spell_cure_light( skill_lookup( "cure light" ), victim->level, ch,victim,TARGET_CHAR);
	return TRUE;
    case 6:
      if ( ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC))
	return FALSE;
	if ( is_affected(victim, skill_lookup("bless")) )
	    return FALSE;
	act( "$n utters the word 'fido'.", ch, NULL, NULL, TO_ROOM );
	spell_bless( skill_lookup( "bless" ),victim->level,ch,victim,TARGET_CHAR);
	return TRUE;
    }
    return FALSE;
}

bool spec_cast_cleric( CHAR_DATA *ch )
{
  const int TARGETS = 6;
  CHAR_DATA *victim, *v_next;
  CHAR_DATA* targets[TARGETS];
  char *spell = NULL;
  int sn, max_tar = 0;

  if ( ch->position != POS_FIGHTING || is_affected(ch,gsn_blasphemy))
    return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next ){
    v_next = victim->next_in_room;
    if ( victim->fighting != ch)
      continue;
    targets[max_tar++] = victim;

    if (max_tar >= TARGETS)
      break;
  }

  if ( max_tar < 1 )
    return FALSE;
  else
    victim = targets[number_range(0, max_tar - 1)];

  for ( ;; ){
    int min_level;
    switch ( number_range(0, 12) ){
    case  0: min_level =  0; spell = "faerie fire";	break;
    case  1: min_level =  2; spell = "cause critical";	break;
    case  2: min_level =  5; spell = "curse";		break;
    case  3: min_level = 10; spell = "blindness";	break;
    case  4: min_level = 15; spell = "poison";		break;
    case  5: min_level = 20; spell = "plague";		break;
    case  6: min_level = 25; spell = "flamestrike";     break;
    case  7: min_level = 30; spell = "harm";		break;
    case  8: min_level = 35; spell = "dispel magic";	break;
    case  9: min_level = 40; spell = "frenzy";		break;
    case 10: min_level = 45; 
      if (number_percent() < 50)
	spell = "spell turning";
      else
	spell = "steel wall";
      break;
    default: min_level = 50; 
      if (IS_GOOD(ch))	spell = "ray of truth";
      else if (IS_EVIL(ch)) spell = "path of deceit";
      else spell = "harm";
							break;
    }
    if ( ( sn = skill_lookup( spell ) ) < 0 )
      continue;
    if ( ch->level >= min_level )
      break;
  }
  if (skill_table[sn].target == TAR_CHAR_DEFENSIVE
      || skill_table[sn].target == TAR_CHAR_SELF){
    act("$n narrows $s eyes.",ch,NULL,NULL,TO_ROOM);
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, ch,TARGET_CHAR);
  }
  else{
    act("$n closes $s eyes momentarily.",ch,NULL,NULL,TO_ROOM);
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
  }
  return TRUE;
}

bool spec_cast_mage( CHAR_DATA *ch )
{
  const int TARGETS = 6;
  CHAR_DATA *victim, *v_next;
  CHAR_DATA* targets[TARGETS];
  char *spell = NULL;
  int sn, max_tar = 0;

  if ( ch->position != POS_FIGHTING || IS_AFFECTED2(ch, AFF_SILENCE))
    return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next ){
    v_next = victim->next_in_room;
    if ( victim->fighting != ch)
      continue;
    targets[max_tar++] = victim;

    if (max_tar >= TARGETS)
      break;
  }

  if ( max_tar < 1 )
    return FALSE;
  else
    victim = targets[number_range(0, max_tar - 1)];
    for ( ;; )
    {
	int min_level;
	switch ( number_range(0, 12))
	{
	case  0: min_level =  0; spell = "magic missile";	break;
	case  1: min_level =  2; spell = "faerie fire";		break;
	case  2: min_level =  5; spell = "dispel magic";	break;
	case  3: min_level = 10; spell = "blindness";		break;
	case  4: min_level = 15; spell = "weaken";		break;
	case  5: min_level = 20; spell = "fireball";		break;
	case  6: min_level = 25; spell = "silence";		break;
	case  7: min_level = 30; spell = "flame arrow";		break;
	case  8: min_level = 35; spell = "fire shield";		break;
	case  9: min_level = 40; spell = "acid blast";		break;
	case 10: min_level = 45; spell = "mind disruption";	break;
	default: min_level = 50; spell = "psionic blast";	break;
	}
	if ( ch->level >= min_level )
	    break;
    }
    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    say_spell( ch, sn );
    if (skill_table[sn].target == TAR_CHAR_DEFENSIVE
	|| skill_table[sn].target == TAR_CHAR_SELF)
      (*skill_table[sn].spell_fun) ( sn, ch->level, ch, ch,TARGET_CHAR);
    else
      (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}

bool spec_cast_undead( CHAR_DATA *ch )
{
    CHAR_DATA *victim, *v_next;
    char *spell;
    int sn;

  if ( ch->position != POS_FIGHTING || IS_AFFECTED2(ch, AFF_SILENCE))
    return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
      v_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	break;
    }
  if ( victim == NULL )
    return FALSE;
  for ( ;; )
    {
	int min_level;
	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";		break;
	case  1: min_level =  4; spell = "curse";		break;
	case  2: min_level =  8; spell = "poison";		break;
	case  3: min_level = 12; spell = "plague";		break;
	case  4: min_level = 16; spell = "weaken";		break;
	case  5: min_level = 20; spell = "enfeeblement";	break;
	case  6: min_level = 24; spell = "blasphemy";		break;
	case  7: min_level = 28; spell = "silence";		break;
	case  8: min_level = 32; spell = "energy drain";	break;
	case  9: min_level = 36; spell = "chorus of anguish";	break;
	case 10: min_level = 40; spell = "dispel magic";	break;
	default: min_level = 45; spell = "blood gout";		break;
	}
	if ( ch->level >= min_level )
	  break;
    }
  if ( ( sn = skill_lookup( spell ) ) < 0 )
    return FALSE;
  (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
  return TRUE;
}



/* returns a criminal in the room */
CHAR_DATA* get_criminal( ROOM_INDEX_DATA* room, CHAR_DATA* ch ){
  CHAR_DATA* victim, *v_next;

  for ( victim = room->people; victim != NULL; victim = v_next ){
    v_next = victim->next_in_room;

    //look for raiders
    if (IS_NPC(victim) 
	&& IS_SET(victim->act, ACT_RAIDER) 
	&& IS_SET(ch->in_room->area->area_flags, AREA_CITY)){
      break;
    }
    else if (is_ghost(victim, 600))
      continue;
    else if (IS_NPC(ch) 
	     && ch->pIndexData->vnum != MOB_VNUM_SPECIAL_GUARD
	     && ch->pIndexData->vnum != MOB_VNUM_HOUND
	     && is_affected(victim, gsn_bribe))
      continue;
    else if (!IS_NPC(victim) && IS_WANTED(victim) && can_see(ch,victim))
      break;
  }
  if ( victim == NULL )
    return NULL;
  else
    return victim;
}

bool spec_special_guard( CHAR_DATA *ch ){
  char buf[MSL];
  CHAR_DATA *victim = NULL;
  CHAR_DATA* mob = ch;
  bool fInCombat = FALSE;

  if ( !IS_AWAKE(ch) 
       || !IS_NPC(ch)
       || ch->master == NULL
       || is_affected(ch, gsn_ecstacy) )
    return FALSE;
  
  if (ch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD
      && is_affected(ch->master, gsn_raise_morale)){
    SET_BIT(ch->off_flags, OFF_BASH);
  }
  else
    REMOVE_BIT(ch->off_flags, OFF_BASH);

  if (ch->fighting != NULL){
    fInCombat = TRUE;
    if (ch->pIndexData->vnum == MOB_VNUM_HOUND
	&& is_affected(ch->master, gsn_raise_morale)){
      if (!is_affected(ch->fighting, gsn_dirt)){
	ch->trust = 6969;
	do_dirt( ch, "" );
	if (ch->fighting == NULL)
	  return FALSE;
	ch->trust = ch->level;
      }
    }
    victim = ch->fighting;
  }

  if ( !fInCombat && (victim = get_criminal( ch->in_room, ch )) == NULL )
    return FALSE;
  else{
    MOB_INDEX_DATA* pMobIndex;
    
    int mod = 100;
    int hp_level = 0;
    int level = !IS_NPC(victim) ? ch->master->level : 55;
    int i = 0;
    int hitroll = 0;
    int damroll = 0;
    
    bool fWarrior = FALSE;
    bool fHalfWar = FALSE;
    bool fRouge = FALSE;
    bool fMage = FALSE;
    
    /* put lower limit on level */
    level = UMAX(21, level);

    if (!fInCombat){
      if (ch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD){
	sprintf( buf, "%s is a criminal!  Justice shall prevail!", PERS2(victim));
	REMOVE_BIT(ch->comm,COMM_NOYELL);
	j_yell( ch, buf );
	SET_BIT(ch->comm,COMM_NOYELL);
      }
      else if (ch->pIndexData->vnum == MOB_VNUM_HOUND){
	check_social(ch, "growl", victim->name);
      }
    }

/* Used to morph a mob to current victims level and power */
/* Assumes that the original stats for the mob were set for */
/* an opponent of level 50 vs a rouge 		   */



/* decide what type of character this is */
    if (IS_NPC(victim))
      fWarrior = TRUE;
    else if (victim->class == class_lookup("monk"))
      fHalfWar = TRUE;
    else if (victim->class == class_lookup("vampire"))
      fWarrior = TRUE;
    else{
      int attack = 1;
      int defense = 0;
      
/* Offense */
      if (victim->pcdata->learned[gsn_second_attack])
	attack++;
      if (victim->pcdata->learned[gsn_third_attack])
	attack++;
      if (victim->pcdata->learned[gsn_fourth_attack])
	attack++;
      if (victim->pcdata->learned[gsn_blades])
	attack += 2;
      if (victim->pcdata->learned[gsn_dan_blade])
	attack++;
      
/* defense */
      if (victim->pcdata->learned[gsn_parry])
	defense++;
      if (victim->pcdata->learned[gsn_dual_parry])
	defense++;
      if (victim->pcdata->learned[gsn_dodge])
	defense++;
      if (victim->pcdata->learned[gsn_shield_block]
	    || victim->pcdata->learned[gsn_2hands])
	defense++;
      if (victim->pcdata->learned[gsn_terra_shield])
	  defense++;
      if (victim->pcdata->learned[gsn_blades])
	defense++;
      if (victim->pcdata->learned[skill_lookup("blur")])
	defense++;
      
/* select which type this is */
      if (attack > 3 || defense > 3)
	fWarrior = TRUE;
      else if (attack > 2 || defense > 1){
	if (victim->pcdata->learned[gsn_hide]
	    || victim->pcdata->learned[gsn_camouflage])
	  fRouge = TRUE;
	else
	  fHalfWar = TRUE;
      }
      else
	fMage = TRUE;
    }
  
/* adjust things base on general archtype */
/* most of this is based on special guards plus few modifications */
    if ( ( pMobIndex = get_mob_index( mob->pIndexData->vnum)) == NULL){
      bug("spec_special_guard: mob index not found.", mob->pIndexData->vnum);
      return FALSE;
    }

/* common */
    REMOVE_BIT(ch->affected_by, AFF_HASTE );
/* class specific */

    if (fWarrior){
      level += 3;
      hitroll += hitroll / 2;
      damroll += level/6;
      if (IS_SET(pMobIndex->off_flags, OFF_FAST))
	SET_BIT(ch->off_flags, OFF_FAST);
      if (IS_SET(pMobIndex->off_flags, OFF_EXTRA_ATTACK))
	SET_BIT(ch->off_flags, OFF_EXTRA_ATTACK);
    }
    else if (fHalfWar){
      level += 2;
      hitroll += hitroll / 3;
      damroll += level/6;
      if (IS_SET(pMobIndex->off_flags, OFF_FAST))
	SET_BIT(ch->off_flags, OFF_FAST);
      if (IS_SET(pMobIndex->off_flags, OFF_EXTRA_ATTACK))
	SET_BIT(ch->off_flags, OFF_EXTRA_ATTACK);
    }
    else if (fRouge){
      level += 1;
      hitroll -= 0;
      damroll -= level/8;
      if (IS_SET(pMobIndex->off_flags, OFF_FAST))
	SET_BIT(ch->off_flags, OFF_FAST);
      REMOVE_BIT(ch->off_flags, OFF_EXTRA_ATTACK);
    }
    else if (fMage){
      level += 2;
      hitroll -= hitroll / 3;
      damroll -= level/10;
      REMOVE_BIT(ch->off_flags, OFF_FAST);
      REMOVE_BIT(ch->off_flags, OFF_EXTRA_ATTACK);
    }
    else{
      if (IS_SET(pMobIndex->off_flags, OFF_FAST))
	SET_BIT(ch->off_flags, OFF_FAST);
      if (IS_SET(pMobIndex->off_flags, OFF_EXTRA_ATTACK))
	SET_BIT(ch->off_flags, OFF_EXTRA_ATTACK);
    }

    if ( !fInCombat && level != mob->level){
      act("$n seems to change in size and shape to match yours.", 
	  mob, NULL, victim, TO_VICT);
      act("$n seems to change in size and shape to match $N's.", 
	  mob, NULL, victim, TO_NOTVICT);
    }
    mod = 100 * level / mob->level;    
    mob->level	    = level;

    if (!fInCombat){
      mob->size = victim->size;
      for (i=0;i < MAX_STATS; i++)  
	mob->perm_stat[i] = get_max_train(victim, i);
      for (i=0; i < 4; i++)
	mob->armor[i] = pMobIndex->ac[i] * mod / 100;
      hp_level = 100 * mob->hit / mob->max_hit;
      mob->max_hit = UMAX(1, mod * mob->max_hit / 100);
      mob->hit = UMAX(1, hp_level * mob->max_hit / 100);
    }
    damroll = pMobIndex->damage[DICE_BONUS] * mod / 100;
    hitroll = pMobIndex->hitroll * mod / 100;

    mob->hitroll    = hitroll;
    mob->damroll    = damroll;
    mob->damage[DICE_NUMBER]  = UMAX(1, mod * pMobIndex->damage[DICE_NUMBER] / 100);
    
    if (!fInCombat && ch->master->fighting != victim){
      multi_hit( ch, victim, TYPE_UNDEFINED );
    }
    return TRUE;
  }
  return FALSE;
}
bool spec_fido( CHAR_DATA *ch )
{
    OBJ_DATA *corpse, *c_next, *obj, *obj_next;
    if ( !IS_AWAKE(ch) )
	return FALSE;
    for ( corpse = ch->in_room->contents; corpse != NULL; corpse = c_next )
    {
	c_next = corpse->next_content;
	if ( corpse->item_type != ITEM_CORPSE_NPC )
	    continue;
	act( "$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM );
	for ( obj = corpse->contains; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    obj_from_obj( obj );
	    obj_to_room( obj, ch->in_room );
            if (is_affected_obj(obj, gsn_levitate))
                act( "$p slowly rises and stays suspended in mid-air.", ch, obj, NULL, TO_ALL );
	}
	extract_obj( corpse );
	return TRUE;
    }
    return FALSE;
}

/* assists players against other NPCS */
bool spec_guard( CHAR_DATA *ch ){
  char buf[MSL];
  CHAR_DATA *victim, *v_next, *ech = NULL;
  int max_evil = 300;
  
  if ( !IS_AWAKE(ch) || ch->fighting != NULL )
    return FALSE;
  
  for ( victim = ch->in_room->people; victim != NULL; victim = v_next ){
    v_next = victim->next_in_room;

    //look for raiders
    if (IS_NPC(victim) && IS_SET(victim->act, ACT_RAIDER) 
	&& IS_SET(ch->in_room->area->area_flags, AREA_CITY)){
      sprintf( buf, "For glory of %s!!", ch->in_room->area->name);
      do_yell( ch, buf );
      multi_hit( ch, victim, TYPE_UNDEFINED );
      return TRUE;
    }

    if (is_ghost(victim, 600))
      continue;
    if ( victim->fighting != NULL && victim->fighting != ch && victim->alignment < max_evil ){
      max_evil = victim->alignment;
      ech      = victim;
    }
  }
  if (ech == NULL && victim == NULL)
    return FALSE;
  else if (ech == NULL)
    ech = victim;
  
  if ( ech != NULL )
    {
      sprintf( buf, "PROTECT THE INNOCENT!!  BANZAI!!");
      do_say( ch, buf );
      multi_hit( ch, ech, TYPE_UNDEFINED );
      return TRUE;
    }
  return FALSE;
}



/* attacks criminals but does not announce to justice cabals */
bool spec_executioner( CHAR_DATA *ch ){
  char buf[MSL];
  CHAR_DATA* victim;

  if (!ch->in_room)
    return FALSE;
  else if ( !IS_AREA(ch->in_room->area, AREA_LAWFUL) 
	    || !IS_AWAKE(ch) 
	    || ch->fighting != NULL )
    return FALSE;
  else
    SET_BIT(ch->act2, ACT_LAWFUL );

  if ( (victim = get_criminal( ch->in_room, ch )) == NULL)
    return FALSE;

  if (IS_NPC(victim) && IS_SET(victim->act, ACT_RAIDER))
    sprintf( buf, "For glory of %s!", ch->in_room->area->name);
  else
    sprintf( buf, "You will pay for your crimes %s!", victim->name);
  REMOVE_BIT(ch->comm,COMM_NOYELL);
  j_yell( ch, buf );
  SET_BIT(ch->comm,COMM_NOYELL);

  sprintf( buf, "%s is at %s of %s!", PERS2(victim), victim->in_room->name, victim->in_room->area->name);
  cabal_echo_flag( CABAL_HEARGUARD, buf );

  /* check of simulacra for justices */
  if (is_affected(victim, gsn_doppelganger)){
    char buf[MIL];
    sprintf( buf, "Warning! Someone seems to be masquerading as %s!",
	     PERS2(victim));
    cabal_echo_flag( CABAL_JUSTICE | CABAL_HEARGUARD, buf );
  }
  multi_hit( ch, victim, TYPE_UNDEFINED );
  return TRUE;
}

/* attacks criminals AND announces on justice channels */
bool spec_gate_guard( CHAR_DATA *ch ){
  char buf[MSL];
  CHAR_DATA *victim;
  
  if ( !ch->in_room || !IS_AREA(ch->in_room->area, AREA_LAWFUL) || !IS_AWAKE(ch) || ch->fighting != NULL )
    return FALSE;
  else
    SET_BIT(ch->act2, ACT_LAWFUL );
  
  if ( (victim = get_criminal( ch->in_room, ch )) == NULL)
    return FALSE;
  
  if (IS_NPC(victim) && IS_SET(victim->act, ACT_RAIDER))
    sprintf( buf, "For glory of %s!", ch->in_room->area->name);
  else{
    sprintf( buf, "%s is a criminal!  Justice shall prevail!", PERS2(victim));
    sprintf( buf, "%s is at %s of %s!", PERS2(victim), victim->in_room->name, victim->in_room->area->name);
    cabal_echo_flag( CABAL_HEARGUARD, buf );
  }
  REMOVE_BIT(ch->comm,COMM_NOYELL);
  j_yell( ch, buf );
  SET_BIT(ch->comm,COMM_NOYELL);
  
  /* check of simulacra for justices */
  if (is_affected(victim, gsn_doppelganger)){
    char buf[MIL];
    sprintf( buf, "Warning! Someone seems to be masquerading as %s!",
	     PERS2(victim));
    cabal_echo_flag( CABAL_JUSTICE | CABAL_HEARGUARD, buf );
  }
  multi_hit( ch, victim, TYPE_UNDEFINED );
  return TRUE;
}

bool spec_janitor( CHAR_DATA *ch )
{
    OBJ_DATA *trash, *trash_next;
    if ( !IS_AWAKE(ch) )
	return FALSE;
    for ( trash = ch->in_room->contents; trash != NULL; trash = trash_next )
    {
	trash_next = trash->next_content;
        if ( !IS_SET( trash->wear_flags, ITEM_TAKE ) )
	    continue;
	if ( trash->item_type == ITEM_DRINK_CON || trash->item_type == ITEM_TRASH || trash->cost < 10 )
	{
	    act( "$n picks up some trash.", ch, NULL, NULL, TO_ROOM );
	    obj_from_room( trash );
	    obj_to_char( trash, ch );
	    return TRUE;
	}
    }
    return FALSE;
}

bool spec_poison( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    if ( ch->position != POS_FIGHTING || ( victim = ch->fighting ) == NULL
    || number_percent( ) > 20 + 2 * (ch->level-victim->level) )
	return FALSE;
    act( "You bite $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n bites $N!",  ch, NULL, victim, TO_NOTVICT );
    act( "$n bites you!", ch, NULL, victim, TO_VICT    );
    spell_poison( gsn_poison, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}

bool spec_thief( CHAR_DATA *ch )
{
    CHAR_DATA *victim, *v_next;
    long gold;
    if ( ch->position != POS_STANDING )
	return FALSE;
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( IS_NPC(victim) || victim->level >= LEVEL_IMMORTAL || number_bits( 5 ) != 0 || !can_see(ch,victim))
	    continue;
	if ( IS_AWAKE(victim) && number_range( 0, ch->level ) == 0 )
	{
            act( "You discover $n's hands in your wallet!", ch, NULL, victim, TO_VICT );
            act( "$N discovers $n's hands in $S wallet!", ch, NULL, victim, TO_NOTVICT );
	    return TRUE;
	}
	else
	{
            gold = victim->gold * UMIN(number_range(1,20),ch->level) / 100;
            gold = UMIN(gold, ch->level * ch->level * 20 );
	    ch->gold     += gold;
	    victim->gold -= gold;
	    return TRUE;
	}
    }
    return FALSE;
}

/* does the cabal services 
 * COMMENTS:
 * The services are divided into four categories:
 *
 * 1) SERV_HEAL : uncurse, heal, cure, restore
 * 2) SERV_ITEM : identify, bless, bind, reinforce, locate
 * 3) SERV_TRAIN: advance, train
 * 4) SERV_SKILL: teach-skill (non cabal)
 * 5) SERV_SPELL: teach-spell (non cabal)
 * 6) SERV_CABAL: teach-cabal (cabal skill/spell)
 * 7) SERV_OTHER: life insurance
 *
 * In order for the mob to offer particular group, its first mana die must have the SERV_XXX bit set.
 * For values of the bits look below, and OR the bit values setting the first mana die to the resultant
 * sum.
 */
#define SERV_HEAL	1
#define SERV_ITEM	2
#define SERV_TRAIN	4
#define SERV_SKILL	8
#define SERV_SPELL	16
#define SERV_CABAL	32
#define SERV_OTHER	64

void do_service(CHAR_DATA* ch, CHAR_DATA* mob, char *argument){
  OBJ_DATA* obj = NULL;
  SPELL_FUN *spell = NULL;

  char arg1[MIL];
  char arg2[MIL];
  char t_name[MIL];

  char* act = NULL;

  const int unit = 100;	//Used for scaling costs, 100 = normal
  int cost = 0;		//Cost subtracted in the end (CPS)
  int sn = 0;
  bool fFound = FALSE;

  int flag = mob->pIndexData->mana[DICE_NUMBER];

  argument = one_argument(argument, arg1);
  sprintf( t_name, "%s", argument );
  one_argument(argument, arg2);


  if (arg1[0] == '\0'){
    if (flag == 0 ){
      act("$N says '`#I offer no services. (SERVICE BITS NOT SET)``'", ch, NULL, mob, TO_CHAR);
      return;
    }
    act("$N says '`#I offer following services.``'", ch, NULL, mob, TO_CHAR);
    sendf(ch, "service <type> <target>:\n\r");
    if (IS_SET( flag, SERV_HEAL)){
      sendf( ch,
	     "  uncurse    <none>	: Remove a curse from body or item.\n\r"\
	     "  heal       <none>	: Heal body\n\r"\
	     "  cure       <none>	: Cure ilness\n\r"\
	     "  restore    <none>	: Restore stamina\n\r\n\r");
    }
    if (IS_SET( flag, SERV_ITEM)){
      sendf( ch,  
	     "  identify   <item>	: Identify object\n\r"\
	     "  bless      <item>	: Bless an item\n\r"\
	     "  bind       <item>	: Bind an owner to item\n\r"\
	     "  reinforce  <item>	: Make an item resistant to destruction\n\r"\
	     "  locate     <item>	: Locate a item in the lands.\n\r\n\r");
    }
    if (IS_SET( flag, SERV_TRAIN)){
      sendf( ch,	     
	     "  advance    <number>   : Provide experience\n\r"\
	     "  train      <stat>	: Provide physical and mental training\n\r\n\r");
    }
    if (IS_SET( flag, SERV_SKILL)|| IS_SET( flag, SERV_SPELL) || IS_SET( flag, SERV_CABAL) ){
      if (IS_SET(flag, SERV_SKILL))
	sendf( ch, "  teach      <skill>    : Teach and lecture on use of skills\n\r");
      if (IS_SET(flag, SERV_SPELL))
	sendf( ch, "  teach      <spell>    : Teach and lecture on use of spells\n\r");
      if (IS_SET(flag, SERV_CABAL))
	sendf( ch, "  teach      <cabal>    : Teach and lecture on use of cabal powers\n\r");
      send_to_char("\n\r", ch );
    }
    if (IS_SET( flag, SERV_OTHER)){
      sendf( ch,	     
	     "  life-insurance        : Secure your items in case of demise\n\r");
    }
    send_to_char("\n\rUse: \"service <type> cost\" for detalied costs.\n\r", ch);
    return;
  }
  
/* SERV_HEAL */
  if (IS_SET( flag, SERV_HEAL)){
    if (!str_prefix(arg1, "uncurse")){
      fFound = TRUE;
      cost = unit * 60 / 100;
      if (arg2[0] != '\0' && !str_cmp("cost", arg2)){
	sendf(ch, "Removes a curse upon a character and his items for %d %s%s per attempt.\n\r", 
	      cost, ch->pCabal->currency, (cost == 1? "" : "s") );
	return;
      }
      spell = spell_remove_curse;
      sn    = skill_lookup("remove curse");
      act = "$N sprinkles some dust and mutters a few incanations.";

      if (IS_AFFECTED(ch, AFF_CURSE)){
	sprintf( t_name, "%s", ch->name);
      }
      else if (IS_NULLSTR(argument)){
	act("$N says '`#You do not seem cursed. Which of your items shall I work on?``'", ch, NULL, mob, TO_CHAR);
	return;
      }
      sprintf( t_name, "%s %s", ch->name, argument );
    }
    else if (!str_prefix(arg1, "heal")){
      fFound = TRUE;
      cost = unit * 25 / 100;
      if (arg2[0] != '\0' && !str_cmp("cost", arg2)){
	sendf(ch, "Quickly restores health for %d %s%s per application.\n\r", 
	      cost, ch->pCabal->currency, (cost == 1? "" : "s") );
	return;
      }
      spell = spell_heal;
      sn    = skill_lookup("heal");
      act = "$N mixes an ointment.";
    }
    else if (!str_prefix(arg1, "cure")){
      fFound = TRUE;
      cost = unit * 28/ 100;
      if (arg2[0] != '\0' && !str_cmp("cost", arg2)){
	sendf(ch, "Removes many bodly harms at %d %s%s per attempt.\n\r", 
	      cost, ch->pCabal->currency, (cost == 1? "" : "s") );
	return;
      }
      spell = spell_cell_adjustment;
      sn    = skill_lookup("cell adjustment");
    act = "$N lights a few smelly candles and chants softly.";
    }
    else if (!str_prefix(arg1, "restore")){
      fFound = TRUE;
      cost = unit * 2 / 100;
      if (!str_cmp("cost", arg2)){
	sendf(ch, "Restores stamina at %d %s%s per application.\n\r", 
	      cost, ch->pCabal->currency, (cost == 1? "" : "s") );
	return;
      }
      spell = spell_refresh;
      sn    = skill_lookup("refresh");
      act = "$N mutters few words as $s eyes shimmer with power.";
    }
  }
/* SERV_ITEM */
  if (IS_SET( flag, SERV_ITEM)){
    if (!str_prefix(arg1, "identify")){
      fFound = TRUE;
      if (arg2[0] == '\0' || !str_cmp("cost", arg2)){
	sendf(ch, "Identifies item, cost equal to level of item.\n\r");
	return;
      }
      if ( (obj = get_obj_list(ch, arg2, ch->carrying)) == NULL){
	do_say(mob, "You carry no such item.");
	return;
      }
      cost = obj->level;
      if (GET_CP(ch) < cost)
      {
	act("$N says 'You do not have enough coin for my services.'", ch,NULL,mob,TO_CHAR);
	return;
      }
      spell = spell_identify;
      sn    = skill_lookup("identify");
      spell_identify(sn, mob->level, ch, obj, TARGET_OBJ);
      /* set sn to 0 so the regular spell function doesnt run. */
      sn = 0;
      spell = NULL;
      act = "$N places $s hands on $s temples and concentrates.";
    }
    else if (!str_prefix(arg1, "bless")){
      AFFECT_DATA af;
      const int dur = -1;
      fFound = TRUE;
      if (arg2[0] == '\0' || !str_cmp("cost", arg2)){
	sendf(ch, "Places a permament blessing on an item.\n\r"\
	      "Cost is 5 times the level of an item.\n\r");
	return;
      }
      if ( (obj = get_obj_list(ch, arg2, ch->carrying)) == NULL){
	do_say(mob, "You carry no such item.");
	return;
      }
      if (IS_OBJ_STAT(obj,ITEM_BLESS) || IS_OBJ_STAT(obj,ITEM_EVIL)){
	do_say(mob, "It is already blessed.");
	return;
      }
      cost = 5 * obj->level;
      if (GET_CP(ch) < cost)
	{
	  act("$N says 'You do not have enough coin for my services.'", ch,NULL,mob,TO_CHAR);
	  return;
	}
      spell = spell_null;
      sn    = 0;
      act = "$N begins to mutter incanations around $p.";
      af.type = skill_lookup("bless arms");
      af.level = mob->level;
      af.duration = dur;
      af.where = TO_OBJECT;
      af.bitvector = IS_EVIL(ch) ? ITEM_EVIL : ITEM_BLESS;
      af.modifier = 0;
      af.location = 0;
      affect_to_obj(obj, &af);
    }
    else if (!str_prefix(arg1, "bind")){
      AFFECT_DATA* paf;
      const int dur = 480;
      fFound = TRUE;
      
      if (arg2[0] == '\0' || !str_cmp("cost", arg2)){
	sendf(ch, "Binds the item to you, as its owner for %d days.\n\r"\
	      "Cost is 2 times the level of item.\n\r", dur / 24);
	return;
      }
      if ( (obj = get_obj_list(ch, arg2, ch->carrying)) == NULL){
	do_say(mob, "You carry no such item.");
	return;
      }
      if (IS_SET(obj->wear_flags,ITEM_HAS_OWNER)){
	do_say(mob, "That item is already bound to someone.");
	return;
      }
      cost = 2 * obj->level;
      if (GET_CP(ch) < cost)
	{
	  act("$N says 'You do not have enough coin for my services.'", ch,NULL,mob,TO_CHAR);
	  return;
	}
      spell = spell_null;
      sn    = 0;
      act = "Hammer and chisel in hand, $N quickly engraves something into $p.";
      set_owner(ch, obj, ch->pcdata->deity);
      /* set owner sets duration to -1, we wont it to dur */
      if ( (paf = affect_find(obj->affected, gen_has_owner)) != NULL)
	paf->duration = dur;
    }
    else if (!str_prefix(arg1, "reinforce")){
      fFound = TRUE;
    if (arg2[0] == '\0' || !str_cmp("cost", arg2)){
      sendf(ch, "Makes the object more resistant to destruction.\n\r"\
	    "Cost is 12 times the level of item.\n\r");
      return;
    }
    if ( (obj = get_obj_list(ch, arg2, ch->carrying)) == NULL){
      do_say(mob, "You carry no such item.");
      return;
    }
    if (IS_OBJ_STAT(obj, ITEM_BURN_PROOF)){
      do_say(mob, "It is already reinforced.");
      return;
    }
    cost = 12 * obj->level;
    if (GET_CP(ch) < cost)
      {
	act("$N says 'You do not have enough coin for my services.'", ch,NULL,mob,TO_CHAR);
      return;
      }
    spell = spell_null;
    sn    = 0;
    act = "$N sprinkles some strange reagents onto $p.";
    SET_BIT(obj->extra_flags, ITEM_BURN_PROOF);
    }
    else if (!str_prefix(arg1, "locate")){
      fFound = TRUE;
      cost = 60;
      if (arg2[0] == '\0' || !str_cmp("cost", arg2)){
	sendf(ch, "Locates an item, %d %s%s per attempt.\n\r", 
	      cost, ch->pCabal->currency, (cost == 1? "" : "s") );
	return;
      }
      if (GET_CP(ch) < cost)
	{
	  act("$N says 'You do not have enough coin for my services.'", ch,NULL,mob,TO_CHAR);
	  return;
	}
      spell = spell_locate_object;
      sn    = skill_lookup("locate object");
      target_name = argument;
      spell_locate_object(sn, mob->level, ch, obj, TARGET_OBJ);
      /* set sn to 0 so the regular spell function doesnt run. */
      sn = 0;
      spell = NULL;
      act = "$N places $s hands on $s temples and concentrates.";
    }
  }
/* SERV_TRAIN */
  if (IS_SET( flag, SERV_TRAIN)){
    if (!str_prefix(arg1, "advance")){
      int gain = 0;
      cost = atoi(arg2);
      gain = 33 * cost;

      fFound = TRUE;
      
      if (cost < 1){
	sendf(ch, "Grants 33 exp points per %s.\n\r", 
	      ch->pCabal->currency);
	return;
      }
      else if (GET_CP(ch) < cost)
	{
	  act("$N says 'You do not have enough coin for my services.'", ch,NULL,mob,TO_CHAR);
	  return;
	}
      else if (ch->level >= 45 ){
	act("$N says 'I'm sorry but there is nothing else I can teach you.'", ch,NULL,mob,TO_CHAR);
	return;
      }
      
      act = "$N begins to lecture on far away places and worldly knowledge.";
      sendf( ch, "You receive %d experience points.\n\r", gain );
      gain_exp(ch, gain);
      sn = 0;
      spell = NULL;
    }
    else if (!str_prefix(arg1, "train")){
      char* pOutput;
      char buf[MIL];
      int stat = 0;

      cost = 500;
      fFound = TRUE;

      if (arg2[0] == '\0' || !str_cmp("cost", arg2)){
	sendf(ch, "Raises a single statistic, %d %s%s per attempt.\n\r", 
	      cost, ch->pCabal->currency, (cost == 1? "" : "s") );
	return;
      }
      if (GET_CP(ch) < cost)
      {
	act("$N says 'You do not have enough coin for my services.'", ch,NULL,mob,TO_CHAR);
	return;
      }
      /* check if can raise stats and raise them */
      if ( !str_cmp( arg2, "str" ) )
	{	stat        = STAT_STR; 	pOutput     = "strength";    }
      else if ( !str_cmp( arg2, "int" ) )
	{	stat	    = STAT_INT; 	pOutput     = "intelligence";    }
      else if ( !str_cmp( arg2, "wis" ) )
	{	stat	    = STAT_WIS; 	pOutput     = "wisdom";    }
      else if ( !str_cmp( arg2, "dex" ) )
	{	stat  	    = STAT_DEX; 	pOutput     = "dexterity";    }
      else if ( !str_cmp( arg2, "con" ) )
	{	stat	    = STAT_CON; 	pOutput     = "constitution";    }
      else
	{
	  strcpy( buf, "You can train:" );
	  if ( ch->perm_stat[STAT_STR] < get_max_train(ch,STAT_STR))     strcat( buf, " str" );
	  if ( ch->perm_stat[STAT_INT] < get_max_train(ch,STAT_INT))     strcat( buf, " int" );
	  if ( ch->perm_stat[STAT_WIS] < get_max_train(ch,STAT_WIS))     strcat( buf, " wis" );
	  if ( ch->perm_stat[STAT_DEX] < get_max_train(ch,STAT_DEX))     strcat( buf, " dex" );
	  if ( ch->perm_stat[STAT_CON] < get_max_train(ch,STAT_CON))     strcat( buf, " con" );
	  if ( buf[strlen(buf)-1] != ':' )
	    {
	      strcat( buf, ".\n\r" );
	      send_to_char( buf, ch );
	    }
	  else
	    send_to_char( "You have nothing left to train", ch);
	  return;
	}
      if ( ch->perm_stat[stat]  >= get_max_train(ch,stat) )
	{
	act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
	}
      ch->perm_stat[stat]++;
      act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
      act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
      /* end training */
      
      act = "$N beams with pride.";
      sn = 0;
      spell = NULL;
    }
  }
/* SERV_SKILL/SPELL/CABAL */
  if (IS_SET( flag, SERV_SKILL) || IS_SET( flag, SERV_SPELL) || IS_SET( flag, SERV_CABAL)){
    if (!str_prefix(arg1, "teach")){
      SKILL_DATA *nsk;
      int skill = 0;
      int sn = 0;
      cost = 100;
      fFound = TRUE;

      if (arg2[0] == '\0' || !str_cmp("cost", arg2)){
	sendf(ch, "Raises a single skill by 1%%. %d %s%s per session.\n\r",
	      cost, ch->pCabal->currency, (cost == 1? "" : "s"));

	if (IS_SET(flag, SERV_CABAL)){
	  sendf(ch, "1/2 price for %s knowledge, 8 %ss for %s knowledge below average level.\n\r", 
		ch->pCabal->who_name,
		ch->pCabal->currency,
		ch->pCabal->who_name );
	}
	return;
      }
    /* check for skill */
      if ( (sn = skill_lookup(argument)) < 1){
	act("$N says 'I know nothing of such ability.'", ch,NULL,mob,TO_CHAR);
	return;
      }
      else if ( (skill = get_skill(ch, sn)) == 0){
	act("$N says 'You do not seem to be proficient with that ability.'", ch,NULL,mob,TO_CHAR);
	return;
      }
      else if (skill >= 95){
	act("$N says 'Your skill in that area far exceeds mine I'm afraid.'", ch,NULL,mob,TO_CHAR);
	return;
      }
      /* check for cabal powers */
    /* check for cabal skills */
      if (get_cskill( ch->pCabal, sn )){
	if (!IS_SET(flag, SERV_CABAL)){
	  act("$N says 'I know nothing of such exotic abilities I'm afraid.'", ch,NULL,mob,TO_CHAR);
	  return;
	}
	if (skill < 75)
	  cost = 8;
	else 
	  cost = 1 * cost / 2;
      }
      else{
	/* check for skills */
	if (skill_table[sn].spell_fun == spell_null && !IS_SET(flag, SERV_SKILL)){
	  act("$N says 'I know nothing of non magical matters I'm afraid.'", ch,NULL,mob,TO_CHAR);
	  return;
	}
	else if (skill_table[sn].spell_fun != spell_null && !IS_SET(flag, SERV_SPELL)){
	  act("$N says 'I know nothing of magical matters I'm afraid.'", ch,NULL,mob,TO_CHAR);
	  return;
	}

	if (skill < 2){
	  act("$N says 'You must first learn the basics of $t.'", ch, skill_table[sn].name ,mob,TO_CHAR);
	  return;
	}
      }

      if (GET_CP(ch) < cost)
	{
	  act("$N says 'You do not have enough coin for my services.'", ch,NULL,mob,TO_CHAR);
	  return;
	}
      /* check for granted skills first */
      if ( (nsk = nskill_find(ch->pcdata->newskills,sn)) != NULL)
	nsk->learned ++;
      else
	ch->pcdata->learned[sn] ++;
      act = "$N begins to drone on about various skills.";
      sendf( ch, "Your understanding of %s has increased.\n\r", 
	     skill_table[sn].name );
      sn = 0;
      spell = NULL;
    }
  }
  if (IS_SET( flag, SERV_OTHER)){
    if (!str_prefix(arg1, "life-insurance")){
      AFFECT_DATA af;
      const int dur = 72;
      
      cost = 5 * ch->level;
      fFound = TRUE;

      if (!str_cmp("cost", arg2)){
	sendf(ch, "Makes the transition to death more peaceful for %d days.\n\r"\
	      "Cost is %d now and once more upon event of death.\n\r", dur / 24, cost);
	return;
      }
      
      if (GET_CP(ch) < cost)
	{
	  act("$N says 'You do not have enough coin for my services.'", ch,NULL,mob,TO_CHAR);
	  return;
	}
      if (is_affected (ch, gsn_life_insurance)) {
	act("$N says 'You are already insured with us child, your death is assured... I mean insured!'",
	    ch, NULL, mob, TO_CHAR);
	return;
      }
      spell = spell_null;
      sn    = 0;
      act = "$N quickly scribes your name on a scroll, and hands it to his assistant.";
      af.where = TO_AFFECTS;
      af.type = skill_lookup("life-insurance");
      af.level = ch->level;
      af.duration = dur;
      af.location = APPLY_NONE;
      af.modifier = cost;
      af.bitvector = 0;
      affect_to_char(ch,&af);
    }
  }
  if (!fFound)
    {
      do_say(mob, "Type \"services\" for list of services I offer.");
      return;
    }
  if (GET_CP(ch) < cost)
    {
      act("$N says 'You do not have enough coin for my services.'", ch,NULL,mob,TO_CHAR);
      return;
    }
  WAIT_STATE(ch,PULSE_VIOLENCE*4);
  CP_GAIN(ch, -cost, TRUE);
  if (act){
    act("$n performs $s service.", mob, NULL, ch, TO_VICT);
    act(act , ch, obj, mob,TO_ALL);
  }
  
  if (spell != NULL && sn > 0){
    target_name = t_name;
    spell(sn, mob->level, mob, ch, TARGET_CHAR);
  }
}



void do_heal(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *mob;
    char buf[MSL], arg[MIL], t_name[MIL];
    int cost, sn;
    SPELL_FUN *spell;
    char *words;	
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
        if ( IS_NPC(mob) && IS_SET(mob->act, ACT_IS_HEALER) )
            break;
    if ( mob == NULL )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }
    if ( mob->pCabal ){
      if( _is_cabal(ch->pCabal, mob->pCabal)){
	do_service(ch, mob, argument);
      }
      else{
	sprintf( buf, "You must be a member of %s to receive services here %s.", mob->pCabal->name, PERS2( ch ));
	do_say( mob, buf );
      }
      return;
    }
    if (is_exiled(ch, mob->in_room->area->vnum )){
      char buf[MIL];
      sprintf( buf, "No one in %s will deal with you %s!", mob->in_room->area->name, PERS2(ch));
      do_say( mob, buf );
      return;
    }
    if ( ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC))
    {
	send_to_char("You are not allowed to use magic!\n\r", ch);
	return;
    }
    argument = one_argument(argument,arg);
    sprintf(t_name, "%s", argument );
    if (arg[0] == '\0')
    {
	act("$N says 'I offer the following spells:'",ch,NULL,mob,TO_CHAR);
        send_to_char("  light: cure light wounds      1000 + level*10 gold\n\r",ch);
        send_to_char("  serious: cure serious wounds  1600 + level*20 gold\n\r",ch);
        send_to_char("  critic: cure critical wounds  2500 + level*50 gold\n\r",ch);
        send_to_char("  heal: healing spell           5000 + level*100 gold\n\r",ch);
        send_to_char("  blind: cure blindness         2000 + level*20 gold\n\r",ch);
        send_to_char("  disease: cure disease         1500 + level*20 gold\n\r",ch);
        send_to_char("  poison:  cure poison          2500 + level*20 gold\n\r",ch); 
        send_to_char("  uncurse: person or item	      5000 + level*20 gold\n\r",ch);
        send_to_char("  refresh: restore movement      500 + level*10 gold\n\r",ch);
        send_to_char("  mana:  restore mana           1000 + level*20 gold\n\r",ch);
	send_to_char(" Type heal <type> to be healed.\n\r",ch);
	return;
    }
    if (!str_prefix(arg,"light"))
    {
        spell = spell_cure_light;
	sn    = skill_lookup("cure light");
	words = "judicandus dies";
        cost  = 1000 + (ch->level*10);
    }
    else if (!str_prefix(arg,"serious"))
    {
	spell = spell_cure_serious;
	sn    = skill_lookup("cure serious");
	words = "judicandus gzfuajg";
	cost  = 1600 + (ch->level*20);
    }
    else if (!str_prefix(arg,"critical"))
    {
	spell = spell_cure_critical;
	sn    = skill_lookup("cure critical");
	words = "judicandus qfuhuqar";
	cost  = 2500 + (ch->level*50);
    }
    else if (!str_prefix(arg,"heal"))
    {
	spell = spell_heal;
	sn = skill_lookup("heal");
	words = "pzar";
	cost  = 5000 + (ch->level*100);
    }
    else if (!str_prefix(arg,"blindness"))
    {
	spell = spell_cure_blindness;
	sn    = skill_lookup("cure blindness");
      	words = "judicandus noselacri";		
        cost  = 2000 + (ch->level*20);
    }
    else if (!str_prefix(arg,"disease"))
    {
	spell = spell_cure_disease;
	sn    = skill_lookup("cure disease");
	words = "judicandus eugzagz";
	cost = 1500 + (ch->level*20);
    }
    else if (!str_prefix(arg,"poison"))
    {
	spell = spell_cure_poison;
	sn    = skill_lookup("cure poison");
	words = "judicandus sausabru";
	cost  = 2500 + (ch->level*20);
    }
    else if (!str_prefix(arg,"uncurse") || !str_prefix(arg,"curse"))
    {
	spell = spell_remove_curse; 
	sn    = skill_lookup("remove curse");
	words = "candussido judifgz";
	cost  = 5000 + (ch->level*20);

	if (IS_AFFECTED(ch, AFF_CURSE)){
	  sprintf( t_name, "%s", ch->name);
	}
	else if (IS_NULLSTR(argument)){
	  act("$N says '`#Which of your items shall I remove the curse from?``'", ch, NULL, mob, TO_CHAR);
	  return;	
	}
	else
	  sprintf( t_name, "%s %s", ch->name, argument );
    }
    else if (!str_prefix(arg,"mana") || !str_prefix(arg,"energize"))
    {
        spell = NULL;
        sn = -1;
        words = "energizer";
        cost = 1000 + (ch->level*20);
    }
    else if (!str_prefix(arg,"refresh") || !str_prefix(arg,"moves"))
    {
	spell =  spell_refresh;
	sn    = skill_lookup("refresh");
	words = "candusima"; 
	cost  = 500 + (ch->level*20);
    }
    else 
    {
        act("$N says 'Type 'heal' for a list of spells.'", ch,NULL,mob,TO_CHAR);
	return;
    }
    if (cost > ch->gold)
    {
        act("$N says 'You do not have enough gold for my services.'", ch,NULL,mob,TO_CHAR);
	return;
    }
    WAIT_STATE(ch,PULSE_VIOLENCE*4);
    ch->gold  -= cost;
    mob->gold += cost;
    act("$n utters the words '$T'.",mob,NULL,words,TO_ROOM);
    if (spell == NULL)
    {
        ch->mana += dice(2,8) + mob->level / 4;
	ch->mana = UMIN(ch->mana,ch->max_mana);
	send_to_char("A warm glow passes through you.\n\r",ch);
	return;
    }
    if (sn == -1)
	return;
    target_name = t_name;
    spell(sn,mob->level,mob,ch,TARGET_CHAR);
}

void acid_effect(void *vo, int level, int dam, int target)
{
    if (target == TARGET_CHAR)
    {
	CHAR_DATA *victim = (CHAR_DATA *) vo;
        if (!IS_AFFECTED(victim, AFF_FAERIE_FIRE)
	    && !saves_spell(3 * level /4 + dam / 15, victim, DAM_ACID,SPELL_AFFLICTIVE))
	{
	    AFFECT_DATA af;
            act("$n starts glowing strangely.",victim,NULL,NULL,TO_ROOM);
            act("The toxin has made you glow.",victim,NULL,NULL,TO_CHAR);
            af.where     = TO_AFFECTS;
            af.type      = skill_lookup("faerie fire");
            af.level     = level;
            af.duration  = 2;
            af.location  = APPLY_AC;
            af.modifier  = 25;
            af.bitvector = AFF_FAERIE_FIRE;
            affect_to_char( victim, &af );
	}
	if (!IS_NPC(victim))
	    gain_condition(victim,COND_HUNGER,dam/20);
    }
}

void cold_effect(void *vo, int level, int dam, int target)
{
    if (target == TARGET_CHAR)
    {
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	if (!saves_spell(3 * level /4 + dam / 15, victim, DAM_COLD,SPELL_AFFLICTIVE))
	{
	    AFFECT_DATA af, *paf;
	    const int gsn_ct = skill_lookup("chill touch");
            act("$n turns blue and shivers.",victim,NULL,NULL,TO_ROOM);
	    act("A chill sinks deep into your bones.",victim,NULL,NULL,TO_CHAR);
	    if ( (paf = affect_find(victim->affected, gsn_ct)) != NULL
		 && paf->modifier < -3)
	      return;
            af.where     = TO_AFFECTS;
	    af.type      = gsn_ct;
            af.level     = level;
            af.duration  = 0;
            af.location  = APPLY_STR;
            af.modifier  = -1;
            af.bitvector = 0;
            affect_join( victim, &af );
	}
	if (!IS_NPC(victim))
	  gain_condition(victim,COND_HUNGER,dam/20);
    }
}

void wound_effect(CHAR_DATA* ch, CHAR_DATA* victim,  int level, int dam){

  if (victim == NULL || saves_spell(level, victim, DAM_SLASH, SPELL_AFFLICTIVE))
    return;
  
  if ( !is_affected(victim, gen_move_dam)){
    AFFECT_DATA af, *paf;

    af.type = gen_move_dam;
    af.level = level;
    af.duration = number_range(0, 1);
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    paf = affect_to_char(victim, &af);
    if (!IS_NPC(ch)){
      string_to_affect(paf, ch->name);
    }

    act("$n's blow wounds $N gravely.", ch, NULL, victim, TO_NOTVICT);
    act("Your blow wounds $N gravely.", ch, NULL, victim, TO_CHAR);
    act("$n's blow wounds you gravely.", ch, NULL, victim, TO_VICT);
  }
}

void fire_effect(void *vo, int level, int dam, int target)
{
  if (target == TARGET_CHAR){
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if (!IS_AFFECTED(victim,AFF_BLIND) 
	&& !saves_spell(3 * level / 4 + dam / 15, victim,DAM_FIRE,SPELL_AFFLICTIVE))
      {
	AFFECT_DATA af;
	act("$n is blinded by smoke!",victim,NULL,NULL,TO_ROOM);
	act("Your eyes tear up from smoke...you can't see a thing!",victim,NULL,NULL,TO_CHAR);
	af.where        = TO_AFFECTS;
	af.type         = skill_lookup("fire breath");
	af.level        = level;
	af.duration     = 1;
	
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
  }
}

void poison_effect(void *vo,int level, int dam, int target)
{
    if (target == TARGET_CHAR)
    {
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        if (!saves_spell(3 * level / 4 + dam / 15, victim,DAM_POISON,SPELL_MALEDICTIVE))
        {
	    AFFECT_DATA af;
            send_to_char("You feel poison coursing through your veins.\n\r",victim);
            act("$n looks very ill.",victim,NULL,NULL,TO_ROOM);
            af.where     = TO_AFFECTS;
            af.type      = gsn_poison;
            af.level     = level;
            af.duration  = 1;
            af.location  = APPLY_STR;
            af.modifier  = -1;
            af.bitvector = AFF_POISON;
            affect_join( victim, &af );
        }
    }
}

void shock_effect(void *vo,int level, int dam, int target)
{
    if (target == TARGET_CHAR)
    {
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	if (!saves_spell(3 * level / 4 + dam / 15, victim,DAM_LIGHTNING,SPELL_AFFLICTIVE))
	{
	    send_to_char("Your muscles stop responding.\n\r",victim);
	    act("$n has been stunned by the shock!", victim, NULL, NULL, TO_ROOM);
	    WAIT_STATE2(victim, 1 * PULSE_VIOLENCE);
	}
    }
}

void  paralyze_effect(void *vo,int level, int dam, int target)
{
  if (target == TARGET_CHAR){
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if (IS_UNDEAD(victim) || !saves_spell(3 * level / 4 + dam / 15, victim,DAM_LIGHTNING,SPELL_AFFLICTIVE)){
      if (!is_affected(victim, gsn_paralyze)){
	AFFECT_DATA af;
	act("$n has been paralyzed!", victim, NULL, NULL, TO_ROOM);
	act("You have been paralyzed!", victim, NULL, NULL, TO_CHAR);
	
	af.type      = gsn_paralyze;
	af.level     = level;
	af.duration  = 1;
	af.where     = TO_AFFECTS;
	af.bitvector = 0;
	af.location  = APPLY_NONE;
	af.modifier  = 3;
	affect_to_char( victim, &af );
      }
    }
  }
}

void do_identify( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL], buf2[MSL];
    OBJ_DATA *obj;
    int i;
    AFFECT_DATA *paf;
    bool extract = FALSE;
    one_argument( argument, arg );
    if (arg[0] == '\0')
    {
        send_to_char("Identify what?\n\r",ch);
        return;
    }
    obj = get_obj_list(ch,arg,ch->carrying);
    if (obj== NULL)
    {
        send_to_char("You don't have that item.\n\r",ch);
        return;
    }
    /* clone check */
    if (obj->pIndexData->vnum == OBJ_VNUM_CLONE){
      extract = TRUE;
      obj = create_object( get_obj_index(obj->cost), obj->level);
    }
    sprintf( buf,"Object '%s' is type %s, material %s.", obj->name, item_name(obj->item_type), obj->material);
    do_say(ch,buf);
    sprintf( buf,"Extra flags: %s.", extra_bit_name( obj->extra_flags ));
    do_say(ch,buf);
    sprintf( buf,"Weight is %d, value is %d, level is %d.", obj->weight / 10, obj->cost, obj->level);
    do_say(ch,buf);
    switch ( obj->item_type )
    {
    case ITEM_SOCKET:
      if (IS_SOC_STAT(obj, SOCKET_ARMOR)
	  && IS_SOC_STAT(obj, SOCKET_WEAPON))
	do_say(ch, "Armor and Weapon socket.");
      else if (IS_SOC_STAT(obj, SOCKET_ARMOR))
	do_say(ch, "Armor only socket.");
      else if (IS_SOC_STAT(obj, SOCKET_WEAPON))
	do_say(ch, "Weapon only socket.");
      else 
	do_say(ch, "Weapon only socket.");
      if (obj->value[1]){
	sprintf(buf, "Adds following to extra flags: %s",
	      flag_string( extra_flags,  obj->value[1] ) );
	do_say(ch, buf);
      }
      if (obj->value[4]){
	sprintf(buf, "Adds following to weapon flags: %s",
	      flag_string( weapon_type2,  obj->value[4] ) );
	do_say(ch, buf);
      }
    break;
    case ITEM_SCROLL: 
    case ITEM_ARTIFACT:
    case ITEM_POTION:
    case ITEM_PILL:
	sprintf( buf, "Level %d spells of:", obj->value[0] );
        for ( i = 1; i <= 4; i++ )
            if ( obj->value[i] >= 0 && obj->value[i] < MAX_SKILL && skill_table[obj->value[i]].name != NULL)
            {
                strcat( buf, " '" );
                strcat( buf, skill_table[obj->value[i]].name );
                strcat( buf, "'" );
            }
	do_say(ch,buf);
	break;
    case ITEM_WAND: 
    case ITEM_STAFF: 
        if (skill_table[obj->value[3]].name == NULL)
            break;
        sprintf( buf, "Has %d charges of level %d", obj->value[2], obj->value[0] );
	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    strcat( buf, " '" );
	    strcat( buf, skill_table[obj->value[3]].name );
	    strcat( buf, "'" );
	}
	do_say(ch,buf);
	break;
    case ITEM_DRINK_CON:
        sprintf(buf,"It holds %s-colored %s.", liq_table[obj->value[2]].liq_color, liq_table[obj->value[2]].liq_name);
	do_say(ch,buf);
        break;
    case ITEM_CONTAINER:
	sprintf(buf,"Capacity: %d#  Maximum weight: %d#  flags: %s",
          obj->value[3], obj->value[0], cont_bit_name(obj->value[1]));
	do_say(ch,buf);
	if (obj->value[4] != 100)
	{
            sprintf(buf,"Weight multiplier: %d%%", obj->value[4]);
	    do_say(ch,buf);
	}
	break;
    case ITEM_WEAPON:
 	sprintf(buf, "Weapon type is ");
	if (IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS) )
	  strcat(buf, "two-handed ");
	switch (obj->value[0])
	{
        case(WEAPON_EXOTIC) : strcat(buf,"exotic.");       break;
        case(WEAPON_SWORD)  : strcat(buf,"sword.");        break; 
        case(WEAPON_DAGGER) : strcat(buf,"dagger.");       break;
        case(WEAPON_SPEAR)  : strcat(buf,"spear.");        break;
        case(WEAPON_MACE)   : strcat(buf,"mace/club.");    break;
        case(WEAPON_AXE)    : strcat(buf,"axe.");          break;
        case(WEAPON_FLAIL)  : strcat(buf,"flail.");        break;
        case(WEAPON_WHIP)   : strcat(buf,"whip.");         break;
        case(WEAPON_POLEARM): strcat(buf,"polearm.");      break;
        case(WEAPON_STAFF)  : strcat(buf,"staff.");        break;
        default             : strcat(buf,"unknown.");      break;
 	}
	do_say(ch,buf);
	if (obj->pIndexData->new_format)
	    sprintf(buf,"Damage is %dd%d (average %d).", obj->value[1],obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2);
	else
	    sprintf( buf, "Damage is %d to %d (average %d).", obj->value[1], obj->value[2], ( obj->value[1] + obj->value[2] ) / 2 );
	do_say( ch,buf );
	if ( IS_OBJ_STAT(obj, ITEM_SOCKETABLE))
	  do_say( ch, "Can be socketed.");
	break;
    case ITEM_ARMOR:
        sprintf( buf, "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.", 
          obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
	do_say( ch,buf );
	if ( IS_OBJ_STAT(obj, ITEM_SOCKETABLE))
	  do_say( ch, "Can be socketed.");
	break;
    case ITEM_THROW:
        sprintf( buf, "Has %d %s left.",obj->value[0],obj->short_descr);
        do_say( ch, buf );
        if (obj->pIndexData->new_format)
            sprintf(buf,"Damage is %dd%d (average %d).",
              obj->value[1],obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2);
        else
            sprintf( buf, "Damage is %d to %d (average %d).",
              obj->value[1], obj->value[2], ( obj->value[1] + obj->value[2] ) / 2 );
        do_say( ch, buf );
        break;
    case ITEM_RANGED:
      if (obj->value[3] == 0)
	sprintf(buf, "Can be used to fire: %s.", 
		flag_string(projectile_type, obj->value[0]));
      else{
	OBJ_INDEX_DATA* ammo = get_obj_index( obj->value[3] );
	sprintf(buf, "Can be used to fire: %s.", 
		ammo == NULL ? "NOT FOUND" : ammo->short_descr);
      }
      do_say(ch, buf);
      sprintf(buf, "Has accuracy of: %d%%, and rate of fire: %d",
	      obj->value[1], obj->value[2]);
      do_say(ch, buf);
      sprintf(buf, "Flags: %s",flag_string(ranged_type, obj->value[4]));
      do_say(ch, buf);
      break;
    case ITEM_PROJECTILE:
      sprintf(buf, "Is a projectile of type %s", 
	      flag_string(projectile_type, obj->value[0]));
      do_say(ch, buf);
      if (obj->pIndexData->new_format)
	sprintf(buf, "Damage is %dd%d (average %d).",
		obj->value[1],obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2);
      else
	sprintf( buf, "Damage is %d to %d (average %d).",
	       obj->value[1], obj->value[2], ( obj->value[1] + obj->value[2] ) / 2 );
      do_say(ch, buf);
    }
    if (!obj->enchanted)
      for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
        {
	    if ( paf->location != APPLY_NONE 
		 && (paf->location < APPLY_O_COND || paf->where == TO_SKILL)
		 && paf->modifier != 0 )
	    {
                sprintf( buf, "Affects %s by %d.", 
			 (paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location )), 
			 paf->modifier );
	        do_say( ch,buf );
            }
            if (paf->bitvector)
            {
                switch(paf->where)
                {
                case TO_AFFECTS: sprintf(buf,"Adds %s affect.", affect_bit_name(paf->bitvector)); break;
                case TO_OBJECT:  sprintf(buf,"Adds %s object flag.", extra_bit_name(paf->bitvector)); break;
                case TO_WEAPON:  sprintf(buf,"Adds %s weapon flags.", weapon_bit_name(paf->bitvector)); break;
                case TO_IMMUNE:  sprintf(buf,"Adds immunity to %s.", imm_bit_name(paf->bitvector)); break;
                case TO_RESIST:  sprintf(buf,"Adds resistance to %s.", imm_bit_name(paf->bitvector)); break;
                case TO_VULN:    sprintf(buf,"Adds vulnerability to %s.", imm_bit_name(paf->bitvector)); break;
		case TO_SKILL:   /* handled above */ break;
                default:         sprintf(buf,"Unknown bit %d: %d", paf->where,paf->bitvector); break;
                }
	        do_say( ch,buf );
            }
        }
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
	if ( paf->location != APPLY_NONE 
	     && (paf->location < APPLY_O_COND || paf->where == TO_SKILL)
	     && paf->modifier != 0 )
	{
            sprintf( buf, "Affects %s by %d", 
		     (paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location )),
 		     paf->modifier );
            if ( paf->duration > -1)
                sprintf(buf2,", %d hours.",paf->duration);
            else
                sprintf(buf2,".");
	    strcat (buf, buf2);
	    do_say( ch,buf );
            if (paf->bitvector)
            {
                switch(paf->where)
                {
                case TO_AFFECTS: sprintf(buf,"Adds %s affect.", affect_bit_name(paf->bitvector)); break;
                case TO_OBJECT:  sprintf(buf,"Adds %s object flag.", extra_bit_name(paf->bitvector)); break;
                case TO_WEAPON:  sprintf(buf,"Adds %s weapon flags.", weapon_bit_name(paf->bitvector)); break;
                case TO_IMMUNE:  sprintf(buf,"Adds immunity to %s.", imm_bit_name(paf->bitvector)); break;
                case TO_RESIST:  sprintf(buf,"Adds resistance to %s.", imm_bit_name(paf->bitvector)); break;
                case TO_VULN:    sprintf(buf,"Adds vulnerability to %s.", imm_bit_name(paf->bitvector)); break;
		case TO_SKILL:   /* handled above */ break;
                default:         sprintf(buf,"Unknown bit %d: %d", paf->where,paf->bitvector); break;
                }
	        do_say( ch,buf );
            }
	}//END if APPLY_NONE
  //MANA CHARGE
    if ( (paf = affect_find(obj->affected, gen_mana_charge)) != NULL)
      {
	
	char buf [MIL];
	if(mcharge_info(ch, buf, obj, paf, TRUE))
	  do_say(ch, buf);
      }
    if (obj->pCabal || obj->race || obj->class >= 0){
      sprintf(buf, "I have a feeling only a %s%s%s%s%s%scan use it.",
	    obj->race ? race_table[obj->race].name : "",
	    obj->race ? " " : "",
	    obj->class >= 0 ? class_table[obj->class].name : "",
		obj->class >= 0 ? " ": "",
	    obj->pCabal ? obj->pCabal->name : "",
	    obj->pCabal ? " ": "");
      do_say(ch, buf);
    }
    
    if (CAN_WEAR(obj, ITEM_UNIQUE))
        do_say(ch,"Unique item.");
    if (CAN_WEAR(obj, ITEM_RARE))
        do_say(ch,"Rare item.");
    if (CAN_WEAR(obj, ITEM_PARRY))
      do_say(ch,"Can be dual parried.");
    if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_USE ) )
      do_say(ch, "I have a feeling this item can be used somehow.");

    if (extract)
      extract_obj( obj );
}

void do_random( CHAR_DATA *ch, char *argument )
{
  RID* pR = get_rand_room(0,0,			//area range (0 to ignore)
			  0,0,			//room ramge (0 to ignore)
			  NULL,0,		//areas to choose from
			  NULL,0,		//areas to exclude
			  NULL,0,		//sectors required
			  NULL,0,		//sectors to exlude
			  NULL,0,		//room1 flags required
			  NULL,0,		//room1 flags to exclude
			  NULL,0,		//room2 flags required
			  NULL,0,		//room2 flags to exclude
			  5,			//number of seed areas
			  FALSE,		//exit required?
			  FALSE,		//Safe?
			  ch);			//Character for room checks

  send_to_char("You have been teleported!\n\r",ch);
  act( "$n vanishes!", ch, NULL, NULL, TO_ROOM );
  char_from_room( ch );
  char_to_room( ch, pR );
  if (ch == NULL || ch->in_room == NULL)
    return;
  act( "$n suddenly pops into existence.", ch, NULL, NULL, TO_ROOM );
  if (ch == NULL || ch->in_room == NULL)
    return;
  do_look( ch, "auto" );
}

void do_home( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    if (IS_NPC(ch))
	location = get_room_index(ch->homevnum);
    else
	location = get_room_index(get_temple(ch));
    if (location == NULL)
    {
	send_to_char("You are completely lost.\n\r",ch);
	return;
    }
    else if (location == ch->in_room)
    {
	send_to_char("You are already home.\n\r",ch);
	return;
    }
    else
    {
	if (IS_AFFECTED(ch,AFF_FLYING) && !is_affected(ch,gsn_thrash))
	    act("$n flies home.",ch,NULL,NULL,TO_ROOM);
	else
	    act("$n walks home.",ch,NULL,NULL,TO_ROOM);
	char_from_room(ch);
	char_to_room(ch,location);
	act("$n enters the room.",ch,NULL,NULL,TO_ROOM);
	do_look(ch,"auto");
    }
}

bool spec_keeper_stay(CHAR_DATA *ch)
{
    if (ch->fighting != NULL)  
        return FALSE;
    if (ch->position < POS_SLEEPING)
        return FALSE;
    if (ch->pIndexData->pShop == NULL || (ch->pIndexData->pShop->open_hour == 0 && ch->pIndexData->pShop->close_hour == 23))
	return FALSE;
    if (ch->status == 1 && ch->position != POS_SLEEPING)
    {
        act("$n yawns and goes to sleep.", ch, NULL, NULL, TO_ROOM);
        ch->position = POS_SLEEPING;
    }
    else if (mud_data.time_info.hour == ch->pIndexData->pShop->close_hour && ch->status == 0 && ch->position != POS_SLEEPING)
    {
        act("$n closes up the shop for the night.", ch, NULL, NULL, TO_ROOM);
        act("$n yawns and settles down for a few hours of sleep.", ch, NULL, NULL, TO_ROOM);
        ch->position = POS_SLEEPING;
        ch->status = 1;
        return TRUE;
    }
    else if (mud_data.time_info.hour == ch->pIndexData->pShop->open_hour && ch->status == 1 && ch->position == POS_SLEEPING)
    {
        ch->position = POS_STANDING;
        act("$n wakes up and gets ready for work.",ch, NULL, NULL, TO_ROOM);
	do_say(ch,"This shop is now opened for business!");
        ch->status = 0;
        return TRUE;
    }
    return FALSE;
}

bool spec_keeper_move(CHAR_DATA *ch)
{
   int open = 0;
   int close = 0;
   int time = 0;

   if (ch->pIndexData->pShop == NULL){
     nlogf("spec_keeper_move: mvnum: %d, error in shop info.", ch->pIndexData->vnum);
    return FALSE;
   }

   if (ch->pIndexData->pShop->open_hour < 0){
     nlogf("spec_keeper_move: mvnum: %d, error in shop info.", ch->pIndexData->vnum);
    return FALSE;
   }
  if (ch->pIndexData->pShop->close_hour < 0){
     nlogf("spec_keeper_move: mvnum: %d, error in shop info.", ch->pIndexData->vnum);
    return FALSE;
  }

  open = ch->pIndexData->pShop->open_hour;
  close = ch->pIndexData->pShop->close_hour;
  time = mud_data.time_info.hour;

  //  bool fReverse = (open > close);

  if (ch->fighting != NULL)  
    return FALSE;
 
 if (ch->position < POS_SLEEPING)
    return FALSE;

  if (ch->pIndexData->pShop == NULL || open == close ||
      (open == 0 && close == 0))
    return FALSE;

  /* auto sleep shop if woken */
  if (ch->status == 1 && ch->position != POS_SLEEPING)
    {
      check_social(ch, "grumble", "");
      act("$n yawns and goes to sleep.", ch, NULL, NULL, TO_ROOM);
      ch->position = POS_SLEEPING;
    }
  /* close shop */
  else if (  time == close && ch->status == 0 && ch->position != POS_SLEEPING)
    {
      act("$n closes up the shop for the night.", ch, NULL, NULL, TO_ROOM);
      move_char(ch, 4, FALSE);
      act("$n yawns and settles down for a few hours of sleep.", ch, NULL, NULL, TO_ROOM);
      ch->position = POS_SLEEPING;
      ch->status = 1;
      return TRUE;
    }
  else if ( time == open && ch->status == 1 && ch->position == POS_SLEEPING)
      {
        ch->position = POS_STANDING;
        act("$n wakes up and heads downstairs for work.", ch, NULL, NULL, TO_ROOM);
        move_char(ch, 5, FALSE);
	do_say(ch,"This shop is now opened for business!");
        ch->status = 0;
        return TRUE;
      }
  return FALSE;
}

bool spec_virgil_guard(CHAR_DATA *ch)
{
    return FALSE;
}
bool spec_shadowdemon(CHAR_DATA *ch)
{
    int chance = number_percent();
    CHAR_DATA *victim;
    if (ch->fighting != NULL)
	return spec_poison(ch);
    if (ch->master == NULL)
	return FALSE;
    if (!IS_AWAKE(ch))
	return FALSE;
    victim = ch->master;
    if (ch->position != POS_FIGHTING && ch->in_room == ch->master->in_room)
    {
    	if (chance < 2)
    	{
	    
	    act("$n snarls and attacks you for wasting its time.",ch,NULL,victim,TO_VICT);
	    act("$n snarls and lunges at $N.",ch,NULL,victim,TO_NOTVICT);
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	    return TRUE;
    	}
    	else if (chance < 10)
	{
	    act("$n growls with a hint of annoyance.",ch,NULL,NULL,TO_ALL);
	    return TRUE;
	}
	else return FALSE;
    }
    return FALSE;
}
	

bool spec_mob_call( CHAR_DATA *ch )
{
    CHAR_DATA *victim, *wolf;
    AFFECT_DATA af;
    if ( ch->position != POS_FIGHTING || !IS_AFFECTED(ch,AFF_CHARM))
	return FALSE;
    if ( (victim = ch->fighting) == NULL)
	return FALSE;
    if ( is_affected(ch,gsn_mind_link) )
	return FALSE;

    if ( ch->pIndexData->vnum == MOB_VNUM_DISPLACER 
	 && victim->in_room->sector_type != SECT_FIELD 
	 && victim->in_room->sector_type != SECT_FOREST 
	 && victim->in_room->sector_type != SECT_HILLS 
	 && victim->in_room->sector_type != SECT_MOUNTAIN )
      return FALSE;
    if ( ch->master == NULL  || (number_percent() > get_skill(ch->master,gsn_mind_link)))
      return FALSE;
    if ( number_bits( 3 ) != 0 )
      return FALSE;
    act("$n shimmers and seems to split in half!",ch,NULL,NULL,TO_ROOM);
    wolf = create_mobile(ch->pIndexData);
    clone_mobile(ch,wolf);
    wolf->master = NULL;
    wolf->leader = NULL;
    REMOVE_BIT(wolf->affected_by,AFF_CHARM);
    REMOVE_BIT(wolf->act2, ACT_NEED_MASTER);
    char_to_room(wolf,ch->in_room);
    af.where            = TO_AFFECTS;
    af.type             = gsn_timer;
    af.level            = ch->level;
    af.duration         = 3;
    af.location         = APPLY_NONE;
    af.modifier         = 0;
    af.bitvector        = 0;
    affect_to_char(wolf,&af);
    af.type		= gsn_mind_link;
    af.duration		= 12;
    affect_to_char(ch,&af);
    multi_hit(wolf,victim,TYPE_UNDEFINED);
    check_improve(ch->master,gsn_mind_link,TRUE,1);
    return TRUE;
}


bool spec_displacer_beast( CHAR_DATA *ch )
{
  //this function does all the wierd things for the displacer beast.
  int chance = 0;
  //const
  //beast just emotes, other wise it toggles vis/invis
  const int non_chance = 95;
  const int pop_chance = 98;

//bools
  bool is_invis = FALSE;

  /* run the mob spec for duplicating displacer */
  spec_mob_call( ch );

  if ( (chance = number_percent()) < non_chance)
    return FALSE;

//First we check if visible
  if (IS_AFFECTED(ch, AFF_INVISIBLE))
    is_invis = TRUE;

//Now we decide what to do.
  if (chance < pop_chance)
    act("$n suddenly disappears only to reappear in a new spot seconds later.", ch, NULL, NULL, TO_ALL);
  else
    {
      if (is_invis)
	{do_visible(ch, NULL);act("$n appears suddenly!", ch, NULL, NULL, TO_ALL);}
      else
	{
	  act("$ns shape blurs and shimmers out of existence.", ch, NULL, NULL, TO_ALL);
	  SET_BIT( ch->affected_by, AFF_INVISIBLE);
	}//end if visible
    }//end if not pop
return TRUE;
}//end spec_displacer_beast.

/* if the mob is not hunting anyone, it looks for any wanted/outlaws in area, selects a random one and hunts them */
bool spec_hunt_wanted(CHAR_DATA *ch)
{
  const int max_char = 8;
  CHAR_DATA* chars[max_char];
  CHAR_DATA* vch;

  int last_char = 0;
  int i = 0;

  if (ch->hunting != NULL)
    return FALSE;
  else
    SET_BIT(ch->act2, ACT_LAWFUL );

  for (i = 0; i < max_char; i ++)
    chars[i] = NULL;

/* select players to choose from */
  for (vch = player_list; vch; vch = vch->next_player ){
    if (!IS_NPC(vch) && IS_WANTED(vch) && vch->in_room && vch->in_room->area == ch->in_room->area
	&& can_see(ch, vch ))
      chars[last_char++] = vch;
  }

  if (last_char  < 1)
    return FALSE;
  else
    ch->hunting = chars[number_range(0, last_char - 1 )];

  return TRUE;
}

/* Written by: Virigoth							*
 * Returns: true if somethign was done					*
 * Comment: Vanguards behavior is coded here, there is two functions	*
 * vanguard_behave and vanguard_fight					*
 * Former handles non combat behavior, latter combat behavior		*/


bool vanguard_fight(CHAR_DATA* ch, CHAR_DATA* victim){

  /* first and foremost:
     - Vangaurd cannot fight mobs or lawfuls or non-evils
     - doing so results in it leaving the master.
     
     combat behavior:
     
 I] Health above 25%:
 - Spell (50)
 --  turn undead if undead
 --  Banishment if demon
 --  lightning
 --  flamearrow
 --  Sear if capable
 -- dispel evil if nothing else cast.

 - Non spell
 -- Grapple
 -- Bodyslam
 -- Sidestep
 -- Disarm/shielddisarm/offhand
 
- Other
 -- Rescues if master below 30
  */
  char buf[MIL];

  const int level = ch->level;
  int chance = 0;

  const int do_combat = 75;		//Control chance that any actiosn are done.
  const int do_spell = 75;

  const int low_health = 30;	//% health at wich behavior changes.
  const int to_taunt = 30;	//chance top taunt the oppoennt/owner
  int to_spell = 50;		//chance to use a spell, otherwise use skills.

  int to_turn = 0;		//used only if undead target
  int to_banish = 0;		//used only if demon target
  int to_sear = 0;		//only if cabable, else it is skipped.
  int to_light = 50;		//lighting
  int to_flame = 80;		//flame arrow
  //otheriwise dispel evil

  int to_grapple = 20;		//grapple lag skill (increaed if < low_health)
  int to_slam = 50;		//bodyslam (100 if < low health)
  int to_trip = 70;		//trip, ruled by low_health
  int to_disarm = 90;		//disarm (if primary no_remove or empty checks sec./shield)
  //kick if nothing else selected.

  int hp_rescue = 40;		//if health > rescue health% to_rescue = 0
  int to_rescue = 40;		//chance to attempt to rescue if < res_health

  bool fHasYelled = FALSE;		//Used to mark if yelled as to not make too many yells/round

  if (!ch || !victim)
    return FALSE;

  if (!IS_AFFECTED(ch, AFF_CHARM)
      || !ch->master)
    return FALSE;

  /* Check who we are fighting */
/* Viri: removed for now
  if ((!IS_EVIL(victim) && !victim->master)
      || (IS_NPC(victim) 
	  && !victim->master 
	  && victim->pCabal == NULL
	  && !IS_AFFECTED(victim, AFF_CHARM))      ){
    if (!IS_IMMORTAL(ch->master)){
      sprintf(buf, "I will have none of your petty squables %s, fight you own battles!", ch->master->name);
      REMOVE_BIT(ch->comm, COMM_NOYELL);
      do_yell(ch, buf);
      fHasYelled = TRUE;
      SET_BIT(ch->comm, COMM_NOYELL);      
      die_follower(ch, TRUE);
      return TRUE;
    }
  }
*/
  /* combat behavior */

  /* check rescue first */
  if ( (ch->master->hit * 100 / ch->master->max_hit < hp_rescue)
       && ch->master->fighting
       && ch->master->fighting->fighting == ch->master){
    if (number_percent() < to_rescue){
      check_social(ch, "snort","");
      sprintf(buf, "You seem to be in need of a rescue %s. Turning into a damsel are you?", PERS(ch->master, ch));
      do_say(ch, buf);
      sprintf(buf, ch->master->name);
      do_rescue(ch, buf);
      return TRUE;
    }
  }

  /* specific stuff */
  if (victim->race == race_lookup("demon")
      && !is_affected(victim, skill_lookup("minor banishment"))
      ){
    to_banish = 100;
    to_spell = 100;
  }
  else if (IS_UNDEAD(victim)
	   && !is_affected(victim, gsn_turn_undead)
	   )
    to_turn = 100;
  else if ( IS_OUTSIDE(ch) 
	    && !IS_SET(ch->in_room->room_flags, ROOM_DARK)
	    && mud_data.time_info.hour > 7 
	    && mud_data.time_info.hour < 18){

	    if (mud_data.weather_info.sky < SKY_RAINING)
	      to_sear = 30;
	    else if (mud_data.weather_info.sky < SKY_CLOUDY)
	      to_sear = 100;
  }
  
  /* test for special behavior when traget is weak. */
  if ( 100 * victim->hit  / victim->max_hit < low_health){
    /* we do not cast spells, but lag */
    to_spell = 0;
    /* check if protective shield is on. */
    if (get_lagprot(victim) != LAG_SHIELD_NONE){
      to_grapple = 0;
      to_slam = 0;
      if (!IS_AFFECTED(victim, AFF_FLYING))
	to_trip = 100;
      else{
	/* kick/disarm will be performed */
	to_trip = 0;
	to_disarm = 30;
      }
    }
    /* no shield set */
    else{
      /* grapple or slam */
      to_grapple = 50;
      to_slam = 100;
    }

    /* no spell check */
    if (is_affected(ch, gsn_blasphemy))
      to_spell = 0;

    /* check if we yell something sarcastic*/
    if (number_percent() < to_taunt){
      bool to_yell = FALSE;
      switch (number_range(0, 10)){
      default:
      case 0:
	sprintf(buf, "Surely you can do better then this %s?", PERS(victim, ch));
	break;
      case 1:
	sprintf(buf, "Truly the death of your seed will be a benefit to any %s.",
		race_table[victim->race].name);
	break;
      case 2:
	sprintf(buf, "You seem to be bleeding quite profusely %s.", PERS(victim, ch));
	check_social(ch, "cackle", "");
	break;
      case 3:
	sprintf(buf, "You seem quite hurt %s. Perhaps one more bodyslam is in order?", PERS(victim, ch));
	break;
      case 4:
	sprintf(buf, "If that is best you have %s, next time bring an army.", 
		PERS(victim, ch));
	break;
      case 5:
	sprintf(buf, "Put some muscle into it %s, I cannot do all the work myself.",
		PERS(ch->master, ch));
	check_social(ch, "snort", "");
	break;
      case 6:
	sprintf(buf, "Run while there is still breath in your lungs %s, for soon it will be gone!",
		PERS(victim, ch));
	to_yell = TRUE;
	break;
      case 7:
	sprintf(buf, "%s you pathetic %s! Even a mudschool monster hits harder!",
		PERS(victim, ch),
		race_table[victim->race].name);
	to_yell = TRUE;
	break;
      case 8:
	sprintf(buf, "Surely %s was drunk when it created you %s!",
		IS_NPC(victim) ? "The One God" : victim->pcdata->deity,
		PERS(victim, ch));
	to_yell = TRUE;
	break;
      case 9:
	sprintf(buf, "All those that wish to witness a dead %s come to me!",
		race_table[victim->race].name);
	to_yell = TRUE;
	break;
      case 10:
	sprintf(buf, "Know %s's wrath %s you pitiful %s!",
		ch->master->pcdata->deity,
		PERS(victim, ch),
		race_table[victim->race].name);
	to_yell = TRUE;
	break;
      }//end say/yell select
      if (to_yell){
	REMOVE_BIT(ch->comm, COMM_NOYELL);
	do_yell(ch, buf);
	fHasYelled = TRUE;
	SET_BIT(ch->comm, COMM_NOYELL);
      }
      else
	do_say(ch, buf);
    }//END Taunting.
  }//END behavior if opponent hurt
  /* stuff said if regular combat */
  else if (number_percent() < to_taunt){
      bool to_yell = FALSE;
      switch (number_range(0, 10)){
      default:
      case 0:
	sprintf(buf, "Tis but a flesh wound.  I'm sure you've had worse.");
	break;
      case 1:
	sprintf(buf, "Parry, dodge, counter, parry.. So predictable.");
	check_social(ch, "sigh", "");
	break;
      case 2:
	act("$n turns $s slit-less helm towards $N.", ch, NULL, ch->master, TO_NOTVICT);
	act("$n turns $s slit-less helm towards $N.", ch, NULL, ch->master, TO_VICT);
	act("You turn your slit-less helm towards $N.", ch, NULL, ch->master, TO_CHAR);
	check_social(ch, "snort", "");
	sprintf(buf, "And who taught you to fence %s?", PERS(ch->master, ch));
	break;
      case 3:
	sprintf(buf, "The wind must be strong %s, you keep missing.", PERS(victim, ch));
	break;
      case 4:
	do_pmote(ch, "Straightens in obvious disdain.");
	if (victim->master){
	  sprintf(buf, "Are you so weak %s that you depend on %s to be your bitch?.", 
		  PERS(victim->master, ch),
		  PERS(victim, ch));
	}
	else if (victim->pCabal == get_cabal("nexus")
		 || victim->race == race_lookup("demon"))
	  sprintf(buf, "Use the Chaos %s...",
		  PERS(victim, ch));
	else if (IS_UNDEAD(victim))
	  sprintf(buf, "Lost any body parts lately %s? ",
		  PERS(victim, ch));
	else if (victim->race == race_lookup("ogre")
		 || ch->master->race == race_lookup("ogre"))
	  sprintf(buf, "Damn creature, spit that thing out of your mouth so I can hear what you're saying!");
	else if (victim->race == race_lookup("dwarf")
		 || victim->race == race_lookup("duergar")
		 || victim->race == race_lookup("halfling")){
	  switch (number_range(0, 2)){
	  case 0:
	    sprintf(buf, "My back is starting to hurt from bending over to fight you %s!",
		    PERS(ch->master, ch));break;
	  case 1:
	    sprintf(buf, "I will not fight a fat child!  Ohh its a %s!",
		    race_table[victim->race].name);break;
	  case 2:
	    sprintf(buf, "You going to stand up and fight like a man shortie?");break;
	    break;
	  }//END SELECT
	}//END short races
	else
	  sprintf(buf, "What %s has seen in you %s is beyond me.",
		  ch->master->pcdata->deity,
		  PERS(ch->master, ch));
	break;
      case 5:
	sprintf(buf, "Put some muscle into it %s, I cannot do all the work myself.",
		PERS(ch->master, ch));
	check_social(ch, "snort", "");
	break;
      case 6:
	sprintf(buf, "For glory of %s!",
		ch->master->pcdata->deity);
	to_yell = TRUE;
	break;
      case 7:
	sprintf(buf, "The light shall be ever victorious!");
	to_yell = TRUE;
	break;
      case 8:
	sprintf(buf, "Cry havoc, and let loose the dogs of war!");
	to_yell = TRUE;
	break;
      case 9:
	sprintf(buf, "All those that wish to witness a dead %s come to me!",
		race_table[victim->race].name);
	to_yell = TRUE;
	break;
      case 10:
	sprintf(buf, "Pain is but a trivial price for your demise %s!",
		PERS(victim, ch));
	to_yell = TRUE;
	break;
      }//end say/yell select
      if (to_yell){
	REMOVE_BIT(ch->comm, COMM_NOYELL);
	do_yell(ch, buf);
	fHasYelled = TRUE;
	SET_BIT(ch->comm, COMM_NOYELL);
      }
      else
	do_say(ch, buf);
    }//END Regular Taunting.


  /* Execute the combat stuff */

  if (number_percent() < to_spell && number_percent() < do_spell){
    if ( (chance = number_percent()) < to_turn){
      do_pmote(ch, "raises its armored hands to they sky as it calls upon its master's power.");
      spell_turn_undead(gsn_turn_undead, level, ch, (void*) victim, TARGET_CHAR);
    }
    else if (chance < to_banish){
      if (!fHasYelled){
	sprintf(buf, "%s! By %s's power I banish you from these lands!", 
		PERS(victim, ch),
		ch->master->pcdata->deity);
	REMOVE_BIT(ch->comm, COMM_NOYELL);
	fHasYelled = TRUE;
	do_yell(ch, buf);
	SET_BIT(ch->comm, COMM_NOYELL);
      }        
      spell_mbanish(skill_lookup("minor banishment"), level, ch, (void*) victim, TARGET_CHAR);
    }
    else if (chance < to_sear){
      act("$n's armor flares with blinding light!", ch, NULL, NULL,  TO_ROOM);
      act("Your armor flares with blinding light!", ch, NULL, NULL,  TO_ROOM);
      broadcast(ch, "The area is suddenly lit up by a bright flare of light.\n\r");
      spell_sear(skill_lookup("sear"), level, ch, (void*) victim, TARGET_CHAR);
    }
    else if (chance < to_light){
      act("Air crackles around $n as $e quickly locks $N in its armored grasp!",
	  ch, NULL, victim, TO_NOTVICT);
      act("Air crackles around $n as $e locks you in $s armored grasp!",
	  ch, NULL, victim, TO_VICT);
      act("Air crackles around you as you lock $N in your armored grasp!",
	  ch, NULL, victim, TO_CHAR);
      broadcast(ch, "Crackle of energy followed by screams and smell of burnt flesh drift into the room.\n\r");
      spell_shocking_grasp(skill_lookup("shocking grasp"), level, ch, (void*) victim, TARGET_CHAR);
    }
    else if (chance < to_flame){
      act("A wave of heat washes by you as sheets of flame shoot forth from $n's gauntlets!", ch, NULL, NULL, TO_ROOM);
      act("A wave of heat washes by you as sheets of flame shoot forth from your gauntlets!", ch, NULL, NULL, TO_CHAR);
      broadcast(ch, "A wave of heat washes by you.\n\r");
      spell_flame_arrow(skill_lookup("flame arrow"), level, ch, (void*) victim, TARGET_CHAR);
    }
    else{
      act("$n points $s weapon at $N as it releases a beam of silver light!", ch, NULL, victim, TO_NOTVICT);
      act("$n points $s weapon at you as it releases a beam of silver light!", ch, NULL, victim, TO_VICT);
      act("You point your weapon at $N as it releases a beam of silver light!", ch, NULL, victim, TO_CHAR);
      spell_dispel_evil(skill_lookup("dispel evil"), level, ch, (void*) victim, TARGET_CHAR);
    }
  }//End spell
  else if (number_percent() < do_combat) {
    if ( (chance = number_percent()) < to_grapple){
      do_grapple(ch, "");
    }
    else if (chance < to_slam){
      act("Amidst great clanking of metal, $n charges at $N.", ch, NULL, victim, TO_NOTVICT);
      act("Amidst great clanking of metal, $n charges at you.", ch, NULL, victim, TO_VICT);
      act("Amidst great clanking of metal, you charge at $N.", ch, NULL, victim, TO_CHAR);
      do_bodyslam(ch,"");
    }
    else if (chance < to_trip){
      do_say(ch, "On your knees heathen!");
      do_trip(ch,"");
    }
    else if (chance < to_disarm){
      OBJ_DATA* wield;
      if ( (wield = get_eq_char( ch, WEAR_WIELD )) == NULL
	    || IS_OBJ_STAT(wield,ITEM_NOREMOVE)
	    || is_affected_obj(wield, gsn_graft_weapon)
	    || is_affected_obj(wield, gen_dark_meta))
	{
	  
	  if ( (wield = get_eq_char( ch, WEAR_SECONDARY )) != NULL)
	    do_offhand_disarm(ch, "");
	  else if ( (wield = get_eq_char( ch, WEAR_SHIELD )) != NULL)
	    do_shield_disarm(ch, "");
	}//End if cannot diarm prim.
      else
	do_disarm(ch, "");
    }
    else
      do_kick(ch, "");
  }//End regular combat.
  return TRUE;
}//END vanguard combat.

bool vanguard_behave(CHAR_DATA* ch){
  /* Rules the non-combat behavior of vangaurd */
  CHAR_DATA* mch =  NULL; //master
  CHAR_DATA* rch = NULL; //random
  CHAR_DATA* vch;

  char buf[MIL];

  const int max_chars = 12;//max characters to select form
  int tot_chars = 0;//current total char.
  CHAR_DATA* chars[max_chars];

  const int to_act = 10;	//chance to do anything.
  int to_area = 30;	//Checks says stuff about area.
  int to_action = 50;	//Performs actions
  int to_say = 100;	//Says soemthing
  //  int to_yell = 100;	//yells out

  bool fHasYelled = FALSE;
  bool fSay = FALSE;
  bool fYell = FALSE;

  int chance = number_percent();
  /* first check if master is around */
  for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
    if (!can_see(ch, vch) || IS_IMMORTAL(vch))
      continue;
    if (vch != ch && vch != ch->master && tot_chars < max_chars)
      chars[tot_chars++] = vch;
    if (vch != ch->master)
      continue;
    mch = vch;
    break;
  }
  /* Return to master occasionly */
  if (!mch && !IS_SET(ch->special,SPECIAL_SENTINEL)
      && ch->master 
      && ch->master->in_room
      && !IS_ROOM(ch->master->in_room, ROOM_NO_INOUT)
      && !IS_ROOM(ch->in_room, ROOM_NO_INOUT)
      && number_percent() < 20){
    act("With great fanfare and cloud of sparks shedding from $s armor $n departs!", ch, NULL, NULL, TO_ALL);
    char_from_room(ch);
    char_to_room(ch, ch->master->in_room);
    act("With great fanfare and cloud of sparks shedding from $s armor $n arrives!", ch, NULL, NULL, TO_ALL);
  }
/* check for halberd */
  if (get_eq_char( ch, WEAR_WIELD) == NULL){
    OBJ_DATA* wield;
    do_say(ch, "Where is that damn halberd?!");
    /* equip it */
    wield = create_object( get_obj_index(OBJ_VNUM_VANGUARD_WEP), ch->level );
    obj_to_ch(wield, ch);
    wear_obj(ch, wield, TRUE, FALSE);
  }

  /* get the random character. */
  if (tot_chars > 0)
    rch = chars[number_range(0, tot_chars - 1)];

  if (number_percent()  > to_act)
    return FALSE;

  /* say thing on area */
  if (chance < to_area){
    fYell = FALSE;
    fSay = FALSE;
    switch (number_range(0, 12)){
      /* CABALS */
    case 0:
    case 1:
    case 12:
      if (ch->in_room->pCabal && ch->in_room->pCabal == get_parent(ch->pCabal)){
	fSay = TRUE;
	do_emote(ch, "seems to relax its stance slightly.");
	sprintf(buf, "Ahh, home sweet home, bloody righteous as usual.");
      }
      else if (ch->in_room->pCabal && ch->in_room->pCabal == get_cabal("nexus")){
	fSay = TRUE;
	check_social(ch, "snort", "");
	sprintf(buf, "This place reeks of scared demons.  They must know I am about.");
      }
      else if (ch->in_room->pCabal == get_cabal("warmaster")){
	fSay = TRUE;
	do_emote(ch, "adjusts his halberd.");
	sprintf(buf, "Why are we here? All I sense is old sweat and lack of intelligence..");
      }
      else if (ch->in_room->pCabal && ch->in_room->pCabal == get_cabal("syndicate")){
	fYell = TRUE;
	if (number_percent() < 50)
	  sprintf(buf, "Come out, come out, where ever you are!");
	else{
	  do_emote(ch, "slams $s gauntlet into $s armored chest making a loud clank");
	  sprintf(buf, "Bring out yer dead! Bring out yer dead!"); 
	}
      }
      else if (ch->in_room->pCabal && ch->in_room->pCabal == get_cabal("savant")){
	check_social(ch, "chuckle", "");
	fSay = TRUE;
	sprintf(buf, "Seeking magical aid in your \"performace\", are you?");
      }
      else if (ch->in_room->pCabal && IS_CABAL(ch->in_room->pCabal, CABAL_JUSTICE)){
	fSay = TRUE;
	sprintf(buf, "And what are we doing here? Wish you a pretty wanted flag?");
      }
      else if (ch->in_room->pCabal && IS_CABAL(ch->in_room->pCabal, CABAL_ROYAL)){
	fYell = TRUE;
	sprintf(buf, "Hail the Royal House of [%s]!", ch->in_room->pCabal->who_name);
      }
      else{
	if (mch){
	  act("In a strange metalic voice $n belches out: `#Danger $N $t, Danger!``", 
	      ch, 
	      (mch->pcdata->family[0]? mch->pcdata->family : ""),
	      mch, TO_ROOM);
	  act("In a strange metalic voice you belche out: `#Danger $N $t, Danger!``", 
	      ch, 
	      (mch->pcdata->family[0]? mch->pcdata->family : ""),
	      mch, TO_CHAR);
	}
	else{
	  fSay = TRUE;
	  sprintf(buf, "Pretty little area.. Could use a few shrines to Palison though.");
	}
      }
      break;
      /* GENERAL AREA */
    case 2:
    case 3:
      fSay = TRUE;
      check_social(ch, "chuckle", "");
      sprintf(buf, "%s.. I've laid more then few demons to rest around here.",ch->in_room->area->name);
      break;
    case 4:
    case 5:
      do_emote(ch, "scans all around.");
      break;
    case 6:
    case 7:
      if (mch){
	fSay = TRUE;
	sprintf(buf, "Well %s.. We've been here for a while now. What now?", mch->name);
      }
      else
	check_social(ch, "grumble", "");
      break;
    case 8:
    case 9:
      act("$n looks up into the sky.", ch, NULL, NULL, TO_ROOM);
      act("You look up into the sky.", ch, NULL, NULL, TO_CHAR);
      break;
    case 10:
    case 11:
      act("You notice $n discreetly try to clean a bird dropping from $s armor.", ch, NULL, NULL, TO_ROOM);
      act("You try to discreetly clean a bird dropping from your armor.", ch, NULL, NULL, TO_CHAR);
      break;
    }
    if (fYell && !fHasYelled){
      REMOVE_BIT(ch->comm, COMM_NOYELL);
      do_yell(ch, buf);
      fHasYelled = TRUE;
      SET_BIT(ch->comm, COMM_NOYELL);
    }
    else if (fSay)
      do_say(ch, buf);
  }//END of AREA actions.
  else if (chance < to_action){
    fYell = FALSE;
    fSay = FALSE;
    switch (number_range(0, 5)){
    case 0:
      act("Little sparks of energy seem to crackle around $n's armor.", ch, NULL, NULL, TO_ROOM);
      act("Little sparks of energy seem to crackle around your armor.", ch, NULL, NULL, TO_CHAR);
      broadcast(ch, "In the distance you hear a slight crackle of energy.");
      break;
    case 1:
      if (rch){
	act("$n looks $N over briefly as $s fists slam together with a loud clank.", ch, NULL, rch, TO_NOTVICT);
	act("$n looks you over briefly as $s fists slam together with a loud clank.", ch, NULL, rch, TO_VICT);
	act("You look $N over briefly and slam your fists together with a loud clank.", ch, NULL, rch, TO_CHAR);
      }
      else{
	act("$n slams $s armored fists together in a sign of impatience.", ch, NULL, NULL, TO_ROOM);
	act("You slam your armored fists together in a sign of impatience.", ch, NULL, NULL, TO_CHAR);
      }
      break;
    case 2:
      if (rch){
	act("$n's halaberd whistles through the air only to pass an inch from $N's head!", ch, NULL, rch, TO_NOTVICT);
	act("$n's halaberd whistles through the air only to pass an inch from your head!", ch, NULL, rch, TO_VICT);
	check_social(ch, "chuckle", "");
      }
      else
	act("$n pits $s halberd into a deadly spin in a dazzling show of skill.",
	    ch, NULL, NULL, TO_ROOM);
      break;
    case 3:
      do_emote(ch, "loses all signs of life becoming a large armored statue.");
      break;
    case 4:
      act("$n's visor less helm spins full circle about. Creepy..", ch, NULL, NULL, TO_ROOM);
      break;
    case 5:
      if (rch){
	act("Armor creaks and sizzles with heat as $n towers over $N.", ch, NULL, rch, TO_NOTVICT);
	act("Armor creaks and sizzles with heat as $n towers over you.", ch, NULL, rch, TO_VICT);
      }
      else
	act("Armor creaks and sizzles with heat as $n straightens to $s full height.", ch, NULL,NULL, TO_ROOM);
      break;
    }
    if (fYell && !fHasYelled){
      REMOVE_BIT(ch->comm, COMM_NOYELL);
      do_yell(ch, buf);
      fHasYelled = TRUE;
      SET_BIT(ch->comm, COMM_NOYELL);
    }
    else if (fSay)
      do_say(ch, buf);
    }//END ACTION
  /* Says */
  else if (chance < to_say){
    fYell = FALSE;
    fSay = FALSE;
    switch (number_range(0, 3)){
    case 0:
    case 1:
      /* Race specific stuff skipped to case 2 if no rch*/
      if (rch){
	act("$n turns towards $N briefly.", ch, NULL, rch, TO_NOTVICT);
	act("$n turns towards you briefly.", ch, NULL, rch, TO_VICT);
	if (rch->race == race_lookup("human")){
	  fSay = TRUE;
	  sprintf(buf, "%s.. How average..", race_table[rch->race].name);
	}
	else if (rch->race == race_lookup("illithid")){
	  fSay = TRUE;
	  sprintf(buf, "Hope your intelligence makes up for your looks %s", 
		  PERS(rch, ch));
	}
	else if (rch->race == race_lookup("illithid")){
	  fSay = TRUE;
	  if (number_percent() < 50)
	    sprintf(buf, "Hope your intelligence makes up for your looks %s.", 
		    PERS(rch, ch));
	  else{
	    check_social(ch, "chuckle", "");
	    sprintf(buf, "Ever seen calamari %s?", 
		    PERS(rch, ch));
	  }
	}
	else if (rch->race == race_lookup("duergar")
		 || rch->race == race_lookup("dwarf")){
	  fSay = TRUE;
	  sprintf(buf, "Watch my legplating %s, else you impale your face on a spikard.", 
		  PERS(rch, ch));
	}
	else if (IS_UNDEAD(rch)){
	  fSay = TRUE;
	  sprintf(buf, "You look like you missed out on your last embalming session %s.", 
		  PERS(rch, ch));
	}
	else{
	  fSay = TRUE;
	  sprintf(buf, "Not bad.. not bad at all...");
	}
	break;
      }//END if random char
    case 2:
    case 3:
      fSay = TRUE;
      if (rch)
	  sprintf(buf, "Is there not something you need to do %s?", 
		  PERS(rch, ch));
      else
	sprintf(buf, "Is there not something we should be doing?");
      break;
    }//end switch
    if (fYell && !fHasYelled){
      REMOVE_BIT(ch->comm, COMM_NOYELL);
      do_yell(ch, buf);
      fHasYelled = TRUE;
      SET_BIT(ch->comm, COMM_NOYELL);
    }
    else if (fSay)
      do_say(ch, buf);
  }//END SAYS
  return TRUE;
}

bool spec_vanguard(CHAR_DATA *ch)
{
  int trust = ch->trust;
  if (!ch)
    return FALSE;
  if (!ch->in_room)
    return FALSE;
  if (!IS_AWAKE(ch))
    return FALSE;
  /* enable mobprog like access tos kills */
  ch->trust = 6969;
  if (ch->fighting)
    vanguard_fight(ch, ch->fighting);
  else
    vanguard_behave(ch);
  ch->trust = trust;
  return TRUE;
}

bool spec_servant_hide(CHAR_DATA *ch){

  if (!ch)
    return FALSE;
  if (ch->pIndexData->vnum != MOB_VNUM_SERVANT_AIR)
    return FALSE;

  if (ch->fighting)
    return FALSE;

  /* Hide if SENTRY */
  if (IS_SET(ch->special, SPECIAL_SENTINEL)
      && !IS_AFFECTED(ch, AFF_HIDE)){
    act("$n seems to disperse into nothingness", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->affected_by, AFF_HIDE);
  }
  else if (!IS_SET(ch->special, SPECIAL_SENTINEL))
    REMOVE_BIT(ch->affected_by, AFF_HIDE);

  return TRUE;
}

bool spec_justice_sentinel(CHAR_DATA *ch){
  /* hides and invises if not incombat,
     scans area for visible wanted and reports to 
     master.
  */
  CHAR_DATA* victim;
  AFFECT_DATA* paf;
  char buf[MIL];
  char buf1[MIL];
  char buf2[MIL];

  const int to_act = 40;
  bool fFound = FALSE;

  /* Hide */
  if (!ch->fighting){
    if (!IS_AFFECTED(ch, AFF_HIDE))
      SET_BIT(ch->affected_by, AFF_HIDE);
    if (!IS_AFFECTED(ch, AFF_INVISIBLE))
      SET_BIT(ch->affected_by, AFF_INVISIBLE);
  }

  if (IS_AFFECTED(ch, AFF_CHARM))
    return FALSE;
/* cannot be following someone */
  if (ch->master)
    return FALSE;
  if (number_percent() > to_act)
    return FALSE;

  if ( (paf = affect_find(ch->affected, gsn_timer)) != NULL 
       && paf->duration <= 1){
    sprintf(buf, "Reporting end of duty at %s in %s.", ch->in_room->name, ch->in_room->area->name);
    cabal_echo_flag( CABAL_JUSTICE, buf );
    extract_char(ch, TRUE);
    return FALSE;
  }
  if (ch->fighting){
    sprintf(buf, "Reporting an attack by %s.  Engaging.", PERS(ch->fighting, ch));
    cabal_echo_flag( CABAL_JUSTICE, buf );
    return TRUE;
  }

  /* scan area for baddies */
    if(  (IS_AFFECTED(ch, AFF_BLIND) && !is_affected(ch, gsn_bat_form)) 
	 || (IS_AFFECTED(ch, AFF_BLIND) && bat_blind(ch))  || bat_blind(ch) )
    {
	send_to_char( "You can't see a thing!\n\r", ch );
	return FALSE;
    }
    if (IS_AFFECTED2(ch,AFF_TERRAIN))
    {
        send_to_char( "Darkness.. Darkness everywhere!\n\r",ch);
        return FALSE;
    }
    if (IS_SET(ch->in_room->room_flags,ROOM_NOWHERE) && !IS_IMMORTAL(ch))
    {
	send_to_char( "You can't see the area out of this room.\n\r",ch);
	return FALSE;
    }

    /* scan now */
    if (number_percent() < 15){
      act("You sense a hidden presence scan the area.", ch, NULL, NULL, TO_ROOM);
      broadcast(ch, "You sense someones eyes on your back...\n\r");
    }

    buf[0] = '\0';
    buf1[0] = '\0';
    buf2[0] = '\0';
    for ( victim = player_list; victim != NULL; victim = victim->next_player ){
      if (IS_SET(victim->act, PLR_WANTED)
	  && (!IS_AFFECTED2(victim,AFF_SHADOWFORM) 
	      || IS_IMMORTAL(ch))
	  && (!IS_AFFECTED2(victim,AFF_TREEFORM))
	  && (!IS_NPC(victim) 
	      || is_affected(victim,gsn_doppelganger)) 
	  && !IS_IMMORTAL(victim) 
	  && victim->in_room != NULL
	  && !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
	  && !room_is_private(victim->in_room)
	  && victim->in_room->area == ch->in_room->area
	  && !is_affected(victim,gsn_bat_form) 
	  && !is_affected(victim,gsn_coffin) 
	  && !is_affected(victim,gsn_entomb)
	  && !IS_AFFECTED2(victim,AFF_CATALEPSY)
	  && can_see( ch, victim )  )
	{
	  sprintf(buf2, "%s%s", (fFound? ", ":""), PERS(victim, ch));
	  fFound = TRUE;
	  strcat(buf1, buf2);
	}
    }
    if (fFound){
      REMOVE_BIT(ch->affected_by, AFF_HIDE);
      sprintf(buf, "Reporting %s in area of %s.", buf1, ch->in_room->area->name);
      cabal_echo_flag( CABAL_JUSTICE, buf );
      SET_BIT(ch->affected_by, AFF_HIDE);
      return TRUE;
    }
    return FALSE;
}

/* checks if the person has been marked by this eye yet */
bool can_meye_report(CHAR_DATA* victim, char* name, int sn){
  AFFECT_DATA* paf;

  for (paf = victim->affected; paf; paf = paf->next ){
    if (paf->type != sn || !paf->has_string || str_cmp(paf->string, name))
      continue;
    else
      break;
  }
  if (paf)
    return FALSE;
  else
    return TRUE;
}
    
bool spec_magic_eye(CHAR_DATA *ch){
  /* hides camoes and invises if not incombat,
     scans area for visible pk threats of leader and reports to 
     leader.
  */
  CHAR_DATA* victim;
  AFFECT_DATA* paf;
  char buf[MIL];
  char buf1[MIL];
  char buf2[MIL];

  const int to_act = 100;
  const int gsn_magic_eye = skill_lookup("magic eye");
  bool fFound = FALSE;

  /* Hide */
  if (!ch->fighting){
    if (!IS_AFFECTED(ch, AFF_HIDE))
      SET_BIT(ch->affected_by, AFF_HIDE);
    if (!IS_AFFECTED2(ch, AFF_CAMOUFLAGE))
      SET_BIT(ch->affected2_by, AFF_CAMOUFLAGE);
    if (!IS_AFFECTED(ch, AFF_INVISIBLE))
      SET_BIT(ch->affected_by, AFF_INVISIBLE);
  }

  if (IS_AFFECTED(ch, AFF_CHARM))
    return FALSE;
/* cannot be following someone */
  if (ch->master)
    return FALSE;
  if (ch->leader == NULL)
    return FALSE;
  if (number_percent() > to_act)
    return FALSE;

  if ( (paf = affect_find(ch->affected, gsn_timer)) != NULL 
       && paf->duration <= 1){
    send_to_char("Your magic eye has expired.\n\r", ch->leader);
    act("$n disappears in a flash of energy.", ch, NULL, NULL, TO_ROOM);
    extract_char(ch, TRUE);
    return FALSE;
  }

  /* scan area for baddies */
  if(IS_AFFECTED(ch, AFF_BLIND)){
    return FALSE;
  }
  if (IS_AFFECTED2(ch,AFF_TERRAIN)){
    return FALSE;
  }
  if (IS_SET(ch->in_room->room_flags,ROOM_NOWHERE) && !IS_IMMORTAL(ch)){
    return FALSE;
  }

  /* scan now */
  if (number_percent() < 15){
    act("You sense a hidden presence scan the area.", ch, NULL, NULL, TO_ROOM);
    broadcast(ch, "You sense someones eyes on your back...\n\r");
  }

  buf[0] = '\0';
  buf1[0] = '\0';
  buf2[0] = '\0';
  for ( victim = player_list; victim != NULL; victim = victim->next_player ){
    if (!IS_NPC(victim)
	&& !IS_AFFECTED2(victim,AFF_SHADOWFORM)
	&& !IS_AFFECTED2(victim,AFF_TREEFORM)
	&& !IS_IMMORTAL(victim) 
	&& victim != ch->leader
	&& victim->in_room != NULL
	&& !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
	&& !room_is_private(victim->in_room)
	&& victim->in_room->area == ch->in_room->area
	&& !is_affected(victim,gsn_coffin) 
	&& !is_affected(victim,gsn_entomb)
	&& !IS_AFFECTED2(victim,AFF_CATALEPSY)
	&& is_pk(ch->leader, victim) 
	&& can_see( ch, victim )
	&& can_meye_report(victim, ch->leader->name, gsn_magic_eye))
	{
	  AFFECT_DATA af;
	  sprintf(buf2, "%s%s", (fFound? ", ":""), PERS(victim, ch));
	  fFound = TRUE;
	  strcat(buf1, buf2);

	  /* mark the person so we do not report them for a while */
	  af.type	= gsn_magic_eye;
	  af.level	= ch->level;
	  af.duration	= 3;
	  af.bitvector	= 0;
	  af.where	= TO_NONE;
	  af.location	= APPLY_NONE;
	  af.modifier	=0;
	  paf = affect_to_char( victim, &af );
	  string_to_affect(paf, ch->leader->name );
	}
  }
  if (fFound){
    sprintf(buf, "%s: Spotted %s.\n\r", PERS2(ch), buf1);
    send_to_char(buf, ch->leader);
    return TRUE;
  }
  return FALSE;
}

/* The scan_for_victim function recursivly looks through rooms in near
   area up to depth of "depth"

   NOTE:
   THE BEHAVIOR OF THE SCAN AGAINST PC/MOBS IS DICTATED BY:

   PC_ONLY:		: chance to only target pc according to rand < PC_ONLY

   CABAL		: If vnum of cabal, those cabal members will not
			  be hunted.

   PREY_TYPE		: Selector for PREY:
   0	:	NONE
   1	:	RACE 
   2	:	RACE_ONLY (HUNTS THAT RACE AND NOTHING ELSE)
   3	:	CLASS
   4	:	CLASS_ONLY (HUNTS THAT CLASS AND NOTHING ELSE)
   5	:	CABAL
   6	:	CABAL_ONLY (HUNTS THAT CABAL AND NOTHING ELSE)

   PREY			: Integer reference to specific prey based on 
			  PREY_TYPE   Should reflect xxx_lookup 
			  from game tables.

*/
  

CHAR_DATA* scan_for_victim(
			   CHAR_DATA*		ch, 
			   ROOM_INDEX_DATA*	room, 
			   const int		max_depth, 
			   int			depth, 
			   int			PC_ONLY, 
			   bool			ALIGN_CHECK, 
			   int			CABAL, 
			   int			PREY_TYPE, 
			   int			PREY
			   ){
  CHAR_DATA* vch;
  int i;

/* control varaibles */
  bool fPC_ONLY = number_percent() <= PC_ONLY ? TRUE : FALSE;

/* E-Z */
  if (depth > max_depth )
    return NULL;

/* check for characters in room */
  for (vch = room->people; vch; vch = vch->next_in_room){
/* following always reject a character */
    if (vch->level < 20 
	|| ch == vch
	|| (IS_NPC(vch) && ch->group == vch->group)
	|| (fPC_ONLY && IS_NPC(vch))
	|| (IS_NPC(vch) && ch->pIndexData->vnum == vch->pIndexData->vnum)
	|| (ch->summoner != NULL && ch->summoner == vch)
	|| IS_IMMORTAL(vch)
	|| !can_see(ch, vch)
	|| is_ghost(vch, 600)
	|| is_safe_quiet(ch, vch))
      continue;

/* FRIENDLY Cabal overrides anything else as well */
    if (CABAL
	&& vch->pCabal
	&& is_same_cabal(get_cabal_vnum( CABAL), vch->pCabal ))
      continue;

/* prey selection overrides fALLIED selection */
    if (PREY_TYPE){
      bool fONLY = FALSE;
      int vch_prey = -1;
      int prey_cab = vch->pCabal ? vch->pCabal->vnum : 0;

      switch (PREY_TYPE){
/* RACE */
      case 1: vch_prey = vch->race; break;
      case 2: vch_prey = vch->race; fONLY = TRUE; break;
/* CLASS */
      case 3: vch_prey = vch->class; break;
      case 4: vch_prey = vch->class; fONLY = TRUE; break;
/* CABAL */
      case 5: vch_prey = prey_cab; break;
      case 6: vch_prey = prey_cab; fONLY = TRUE; break;

      default: vch_prey = -1;
      }
/* vch_prey now contains the match that we are hunting if any */
      if (vch_prey != -1
	  && vch_prey == PREY)
	return vch;
/* At this point we did not match a prey, check if this was a prey ONLY */
/* if so, we do not look for any other match and check next char */
      if (fONLY)
	continue;
    }

/* Finaly align check rejection */
    if (ALIGN_CHECK){
      if ( (IS_EVIL(ch) && IS_EVIL(vch) && number_percent() < 40)
	   || (IS_GOOD(ch) && IS_GOOD(vch) && number_percent() < 40)
	   || ch->race == vch->race
	   || (IS_UNDEAD(ch) && IS_UNDEAD(vch)) )
	   continue;
    }
    return vch;
  }

/* No valid characters found in this room, move on to adjacent rooms */
  for ( i= 0; i < MAX_DOOR; i++){
    EXIT_DATA *pexit;
    if ((pexit = room->exit[i]) == NULL)
      continue;
    if (pexit->to_room == NULL
	|| IS_MARKED( pexit->to_room  ))
      continue;
    if (IS_SET(pexit->to_room->area->area_flags, AREA_MUDSCHOOL))
      continue;

/* Mark the room for rejection */
    MARK( room );
    vch = scan_for_victim(ch, pexit->to_room, max_depth, depth + 1, 
			  PC_ONLY, ALIGN_CHECK, CABAL, PREY_TYPE, PREY);
    UNMARK( room );
    if (vch)
      return vch;
  }
  return NULL;
}



/* function responsible for hunter killer behavior 
   NOTE:
   THE BEHAVIOR OF THE SCAN AGAINST PC/MOBS IS DICTATED BY:

   PC_ONLY:		: chance to only target pc according to rand < PC_ONLY

   CABAL		: If vnum of cabal, those cabal members will not
			  be hunted.

   PREY_TYPE		: Selector for PREY:
   0	:	NONE (PREY is the movement range)
   1	:	RACE 
   2	:	RACE_ONLY (HUNTS THAT RACE AND NOTHING ELSE)
   3	:	CLASS
   4	:	CLASS_ONLY (HUNTS THAT CLASS AND NOTHING ELSE)
   5	:	CABAL
   6	:	CABAL_ONLY (HUNTS THAT CABAL AND NOTHING ELSE)

   PREY			: Integer reference to specific prey based on 
			  PREY_TYPE   Should reflect xxx_lookup 
			  from game tables.

*/
bool hunter_killer(CHAR_DATA* ch, int PC_ONLY, bool fALLIED, bool fWARP, bool fCITYWARP, int CABAL, int PREY_TYPE, int PREY){
  extern char * const dir_name[];
  ROOM_INDEX_DATA* to_room, *last_room;  

  int max_depth = PREY_TYPE == 0 ? PREY  : 15;	//Maximum rooms away that new prey will found
  const int max_hunt = 30;	//Max distance for detecting hunted
  int max_steps = PREY_TYPE == 0 ? PREY : 10;	//Max number of steps to be taken at once

  int steps = 0;
  int door = 0;
  
  CHAR_DATA* victim = NULL;

/* 
   The function checks all rooms surrounding it up to depth of "depth"
   in recursive fashion.  If a valid target is found (ch of level > 20)
   the mob fakes "moving" there by shoting few messages, and then teleporting 
   in.
*/
  if (ch->fighting)
    return TRUE;

/* 
all this is skipped if we are already hunting someone.
In such case, we check if the hunted is within the max_hut range to follow him
and if so, we follow max_fol steps.  Otherwise we stop hunting
*/
  if (ch->hunting != NULL){
/* check for distance */
    victim = ch->hunting;
    if (find_first_step(ch->in_room, victim->in_room, max_hunt, TRUE, NULL) < 0){
      act("$n begins to look for a new prey.", ch, NULL, NULL, TO_ROOM);
      act("You begin to look for a new prey.", ch, NULL, NULL, TO_CHAR);
      ch->hunting = NULL;
      return TRUE;
    }
  }
  else
    victim = scan_for_victim(ch, ch->in_room, max_depth, 0, PC_ONLY, fALLIED,
			     CABAL, PREY_TYPE, PREY);

/* At this point if a victim is not found we look for pc's are left in area */
  if (victim == NULL ){
    CHAR_DATA* vch;
    if (ch->in_room->area->nplayer > 0){
      for ( vch = player_list; vch != NULL; vch = vch->next_player ){
	if (!IS_IMMORTAL(vch)
	    && vch->in_room && ch->in_room
	    && vch->in_room->area == ch->in_room->area)
	  break;
      }
    }
    else
      vch = NULL;
/* now we know if there are ANY valid pc's in the area */
/* if there is no one in the area and the mob is set to warp or warpcity */
/* the mob moves to any area */

    if (vch == NULL && (fWARP || fCITYWARP)){
      AREA_DATA* pArea;
      AREA_DATA* areas[top_area];
      int max_area = -1;

/* run through area vnums and make sure they are ok to be selected */
      for (pArea = area_first; pArea; pArea = pArea->next){
	if (IS_SET(pArea->area_flags, AREA_MUDSCHOOL)
	    || IS_SET(pArea->area_flags, AREA_RESTRICTED))
	  continue;
	if (fCITYWARP && !IS_SET(pArea->area_flags, AREA_CITY))
	  continue;
	areas[++max_area] = pArea;
      }
/* Now we warp to a random room in the area selected */
      if (max_area > -1){
	AREA_DATA* pArea = areas[number_range(0, max_area)];
	ROOM_INDEX_DATA* pRoomIndex;
	ROOM_INDEX_DATA* rooms[pArea->max_vnum - pArea->min_vnum];
	int max_room = -1;
	int vnum = 0;

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ ){
	  if ( ( pRoomIndex = get_room_index( vnum ) ) != NULL)
	    rooms[++max_room] = pRoomIndex;
	}
/* now we have an array full of valid rooms */
	if (max_room > -1){
/* warp away */
	  ROOM_INDEX_DATA* pRoomIndex = rooms[number_range(0, max_room)];
	  act("$n disappears suddenly!", ch, NULL, NULL, TO_ROOM);
	  char_from_room(ch);
	  act("$n appears suddenly!", ch, NULL, NULL, TO_ROOM);
	  char_to_room(ch, pRoomIndex);
	  return TRUE;
	}
      }
    }
  }
  
/* If we have not warped we wander */
  if (victim == NULL && !IS_SET(ch->act, ACT_WANDER))
    return TRUE;

  /* get the room to move to */
  if (victim)
    to_room = victim->in_room;
  else{
/* select a random exit to leave through */
    EXIT_DATA* pexit;
    int pre_doors[MAX_DOOR];
    int doors[MAX_DOOR];
    int pre_max_door = -1;
    int max_door = -1;
    int last_door = -1 * ch->status;
    int i = 0;

/* put valid exits into array */
    for (i = 0; i < MAX_DOOR; i++){
      if ((pexit = ch->in_room->exit[i]) == NULL)
	continue;
      if (pexit->to_room == NULL)
	continue;
      if (IS_SET(pexit->to_room->area->area_flags, AREA_MUDSCHOOL)
	  || IS_SET(pexit->to_room->area->area_flags, AREA_RESTRICTED))
	continue;
      if (IS_SET(pexit->exit_info, EX_NOPASS))
	continue;
      pre_doors[++pre_max_door] = i;
    }
/* 
   we use status to preven doubling back to the room we came from
   unless that is the only valid exit (only if there is more then 1 exit) 
*/
    last_door = rev_dir[last_door];

    for (i = 0; i <= pre_max_door; i++){
      if (pre_max_door > 0 
	  && pre_doors[i] == last_door)
	continue;
      else
	doors[++max_door] = pre_doors[i];
    }

    /* now we have all the valid doors in array */
    if (max_door < 0)
      return TRUE;
    else{
      int door = doors[number_range(0, max_door)];
/* status used above to make sure the mob doesnt walk back and forth */
      ch->status = -1 * door;
      to_room = ch->in_room->exit[door]->to_room;
    }
  }
/* start moving */
  while (ch->in_room != to_room && steps < max_steps){
    steps++;
    door = UMAX(0, find_first_step(ch->in_room, to_room, max_hunt, TRUE, NULL));
    if (door < 0 || door > MAX_DOOR)
      return FALSE;
    if ( ch->in_room->exit[door]
	 && ch->in_room->exit[door]->to_room
	 && IS_SET( ch->in_room->exit[door]->exit_info, EX_CLOSED ) 
	 && !(IS_AFFECTED(ch,AFF_PASS_DOOR) 
	      && !IS_SET(ch->in_room->exit[door]->exit_info,EX_NOPASS)) ){
      do_open( ch, (char *) dir_name[door] );
    }
    if (IS_SET(ch->in_room->exit[door]->to_room->room_flags, ROOM_NO_MOB))
      return TRUE;
    last_room = ch->in_room;
    move_char( ch, door, FALSE );
/* check if died */
    if (!ch->in_room)
      return FALSE;
    if (ch->in_room == last_room)
      return TRUE;
  }
  

/* attack if had victim */
  if (victim == NULL)
    return TRUE;
  if (ch->in_room != victim->in_room)
    return TRUE;
/*
  if (IS_NPC(victim)){
    REMOVE_BIT(victim->comm,COMM_NOCHANNELS);
    REMOVE_BIT(victim->comm,COMM_NOYELL);
  }
*/
/* instead of attackign we run a TRIG_SPECIAL check, and let the
   mobprog do whatever it wants
*/
  if (IS_NPC(ch)){
    if (HAS_TRIGGER_MOB(ch, TRIG_SPECIAL))
      p_percent_trigger( ch, NULL, NULL, victim, NULL, NULL, TRIG_SPECIAL);
  }
  else
    do_murder(ch, victim->name);
/*
  if (IS_NPC(victim)){
    SET_BIT(victim->comm,COMM_NOCABAL);
    SET_BIT(victim->comm,COMM_NOYELL);
  }
*/
  return TRUE;
  }


/* 
   This is a spec function for mobs that will cause them to look
   for victims in immediate area
   NOTE:
   THE BEHAVIOR OF THE HUNTER AGAINST PC/MOBS IS DICTATED BY MANA and GROUP 

   GROUP		: If vnum of cabal, non of those cabal members will
			  be hunted.

   mana[DICE_NUMBER]	: Chance for PC ONLY
   mana[DICE_TYPE]	: Selector for DICE BONUS:
   0	:	NONE (DICE_BONUS becomes the movement range)
   1	:	RACE 
   2	:	CLASS
   3	:	CABAL
   4	:	RACE _ONLY
   5	:	CLASS_ONLY
   6	:	CABAL_ONLY
   mana[DICE_BONUS]	: Integer reference to specific prey based on 
			  mana[DICE_TYPE].   Should reflect xxx_lookup 
			  from game tables.

*/

bool spec_hunter_nonallied(CHAR_DATA *ch){
  bool fALLIED = FALSE;
  bool fCITYWARP = FALSE;
  bool fWARP = FALSE;
  int PC_ONLY = ch->pIndexData->mana[DICE_NUMBER];

  int CABAL = ch->group;
  int PREY_TYPE = ch->pIndexData->mana[DICE_TYPE];
  int PREY = ch->pIndexData->mana[DICE_BONUS];

  return hunter_killer(ch, PC_ONLY, fALLIED, fWARP, 
		       fCITYWARP, CABAL, PREY_TYPE, PREY);
}

bool spec_hunter_nonallied_warp(CHAR_DATA *ch){
  bool fALLIED = FALSE;
  bool fCITYWARP = FALSE;
  bool fWARP = TRUE;
  int PC_ONLY = ch->pIndexData->mana[DICE_NUMBER];

  int CABAL = ch->group;
  int PREY_TYPE = ch->pIndexData->mana[DICE_TYPE];
  int PREY = ch->pIndexData->mana[DICE_BONUS];

  return hunter_killer(ch, PC_ONLY, fALLIED, fWARP, 
		       fCITYWARP, CABAL, PREY_TYPE, PREY);
}

bool spec_hunter_nonallied_cwarp(CHAR_DATA *ch){
  bool fALLIED = FALSE;
  bool fCITYWARP = TRUE;
  bool fWARP = TRUE;
  int PC_ONLY = ch->pIndexData->mana[DICE_NUMBER];

  int CABAL = ch->group;
  int PREY_TYPE = ch->pIndexData->mana[DICE_TYPE];
  int PREY = ch->pIndexData->mana[DICE_BONUS];

  return hunter_killer(ch, PC_ONLY, fALLIED, fWARP, 
		       fCITYWARP, CABAL, PREY_TYPE, PREY);
}

bool spec_hunter_allied(CHAR_DATA *ch){
  bool fALLIED = TRUE;
  bool fCITYWARP = FALSE;
  bool fWARP = FALSE;
  int PC_ONLY = ch->pIndexData->mana[DICE_NUMBER];

  int CABAL = ch->group;
  int PREY_TYPE = ch->pIndexData->mana[DICE_TYPE];
  int PREY = ch->pIndexData->mana[DICE_BONUS];

  return hunter_killer(ch, PC_ONLY, fALLIED, fWARP, 
		       fCITYWARP, CABAL, PREY_TYPE, PREY);
}

bool spec_hunter_allied_warp(CHAR_DATA *ch){
  bool fALLIED = TRUE;
  bool fCITYWARP = FALSE;
  bool fWARP = TRUE;
  int PC_ONLY = ch->pIndexData->mana[DICE_NUMBER];

  int CABAL = ch->group;
  int PREY_TYPE = ch->pIndexData->mana[DICE_TYPE];
  int PREY = ch->pIndexData->mana[DICE_BONUS];

  return hunter_killer(ch, PC_ONLY, fALLIED, fWARP, 
		       fCITYWARP, CABAL, PREY_TYPE, PREY);
}

bool spec_hunter_allied_cwarp(CHAR_DATA *ch){
  bool fALLIED = TRUE;
  bool fCITYWARP = TRUE;
  bool fWARP = TRUE;
  int PC_ONLY = ch->pIndexData->mana[DICE_NUMBER];

  int CABAL = ch->group;
  int PREY_TYPE = ch->pIndexData->mana[DICE_TYPE];
  int PREY = ch->pIndexData->mana[DICE_BONUS];

  return hunter_killer(ch, PC_ONLY, fALLIED, fWARP, 
		       fCITYWARP, CABAL, PREY_TYPE, PREY);
}

/* makes the mob change damage type to suit the vuln */
bool spec_terminator(CHAR_DATA *ch){
  CHAR_DATA* vch;
  int vuln = 0;
  int norm = 0;
  int i = 0;

  if (ch->fighting == NULL)
    return FALSE;
  else 
    vch = ch->fighting;
  if (get_eq_char(ch,WEAR_WIELD) != NULL)
    return FALSE;

  /* we run through the hit table looking for vuln match */
  for (i = 0; i < MAX_DAMAGE_MESSAGE; i++ ){
    int type = attack_table[i].damage;
    if (type < 0 || type == DAM_INTERNAL)
      continue;
    if (check_immune(vch, type, FALSE) > IS_NORMAL){
      vuln = i;
      break;
    }
    if (norm)
      continue;
    if (check_immune(vch, type, FALSE) == IS_NORMAL){
      norm = i;
    }
  }
  /* now we have either norm or vuln */
  if (vuln){
    if (vuln != ch->dam_type){
      act("$n changes $s weapon.", ch, NULL, NULL, TO_ROOM);
      act("You change your weapon.", ch, NULL, NULL, TO_CHAR);
      ch->dam_type = vuln;
    }
  }
  else if (norm && norm != ch->dam_type){
    act("$n changes $s weapon.", ch, NULL, NULL, TO_ROOM);
    act("You change your weapon.", ch, NULL, NULL, TO_CHAR);
    ch->dam_type = norm;
  }
  return TRUE;
}


void do_star_seer( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af;
  OBJ_DATA* obj;
  AREA_DATA* pArea;
  char buf[MIL];
  char* text;
  const int max_obj = 16;
  int max = 0;
  OBJ_DATA* objects[max_obj];


  const int sn = skill_lookup("locate object");

  if (!IS_NPC(ch)){
    return;
  }

  if (is_affected(ch, sn)){
    do_say(ch, "I fear I am too tired child.  Seek my help in few days time.");
    return;
  }
  af.type = sn;
  af.level = 60;
  af.duration = 12;
  af.where = TO_NONE;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;



  for ( obj = object_list; obj != NULL; obj = obj->next ){
    if (obj->pIndexData->vnum == OBJ_VNUM_STARSTONE
	&& obj->in_room != NULL
	&& max < max_obj)
      objects[max++] = obj;
  }

  act("$n closes her eyes and kneels and begins to chant a holy psalm.",
      ch, NULL, NULL, TO_ROOM);
  if (max && number_percent() < 75){
    do_say(ch, "Darkness I see.. Darkness everywhere.. Wait!");
    affect_to_char(ch, &af);
  }
  else{
    do_say(ch, "Darkness I see.. Darkness everywhere..");
    act("$n opens here eyes and stands up.", ch, NULL, NULL, TO_ROOM);
    do_say(ch, "I'm sorry child, but He did not grant me a vision.");
    af.duration /= 4;
    affect_to_char(ch, &af);
    return;
  }
/* get random obj */
  obj = objects[number_range(0, max - 1)];
/* get random area to mention */
  if ( (pArea = get_rand_aexit(obj->in_room->area)) == NULL)
    pArea = obj->in_room->area;

/* compose the text to say */
  switch (obj->in_room->sector_type){
  default:		text = "lies on the ground"	; break;
  case SECT_INSIDE:	text = "lies indoors"		; break;
  case SECT_CITY:	text = "rests on a sidewalk"	; break;
  case SECT_FIELD:	text = "lies in grass"		; break;
  case SECT_FOREST:	text = "lies in bush"		; break;
  case SECT_HILLS:	text = "lies on a hill"		; break;
  case SECT_MOUNTAIN:	text = "lies amongst stones"	; break;
  case SECT_WATER_SWIM:	text = "lies in depths"		; break;
  case SECT_WATER_NOSWIM:text = "lies in water"	; break;
  case SECT_SWAMP:	text = "lies in mud"		; break;
  case SECT_AIR:	text = "falls through air"	; break;
  case SECT_DESERT:	text = "lies in sand"		; break;
  case SECT_LAVA:	text = "lies in fire"		; break;
  case SECT_SNOW:	text = "lies in ice"		; break;
  }

  sprintf(buf, "I.. I see a starstone.. It %s and a specter of %s looms on the horizon.",
	  text, pArea->name);
  do_say(ch, buf);
  act("$n arises and opens $s eyes.", ch, NULL, NULL, TO_ROOM);
  do_say(ch, "I'm sorry child, but that is all He has shown me.");
}

void do_obelisk_seer( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af;
  OBJ_DATA* obj;
  AREA_DATA* pArea;
  char buf[MIL];
  char* text;
  const int max_obj = 32;
  int max = 0;
  OBJ_DATA* objects[max_obj];


  const int sn = skill_lookup("locate object");

  if (!IS_NPC(ch)){
    return;
  }

  if (is_affected(ch, sn)){
    do_say(ch, "I fear I am too tired child.  Seek my help in few days time.");
    return;
  }
  af.type = sn;
  af.level = 60;
  af.duration = 12;
  af.where = TO_NONE;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;



  for ( obj = object_list; obj != NULL; obj = obj->next ){
    if (is_name("crusader_obelisk", obj->name)
	&& obj->in_room != NULL
	&& max < max_obj)
      objects[max++] = obj;
  }

  act("$n closes her eyes and kneels and begins to chant a holy psalm.",
      ch, NULL, NULL, TO_ROOM);
  if (max && number_percent() < 75){
    do_say(ch, "Darkness I see.. Darkness everywhere.. Wait!");
    affect_to_char(ch, &af);
  }
  else{
    do_say(ch, "Darkness I see.. Darkness everywhere..");
    act("$n opens here eyes and stands up.", ch, NULL, NULL, TO_ROOM);
    do_say(ch, "I'm sorry child, but He did not grant me a vision.");
    af.duration /= 4;
    affect_to_char(ch, &af);
    return;
  }
/* get random obj */
  obj = objects[number_range(0, max - 1)];
/* get random area to mention */
  if ( (pArea = get_rand_aexit(obj->in_room->area)) == NULL)
    pArea = obj->in_room->area;

/* compose the text to say */
  switch (obj->in_room->sector_type){
  default:		text = "stands on the ground"	; break;
  case SECT_INSIDE:	text = "stands indoors"		; break;
  case SECT_CITY:	text = "stands on a sidewalk"	; break;
  case SECT_FIELD:	text = "stands in grass"	; break;
  case SECT_FOREST:	text = "stands in bush"		; break;
  case SECT_HILLS:	text = "stands on a hill"	; break;
  case SECT_MOUNTAIN:	text = "stands amongst stones"	; break;
  case SECT_WATER_SWIM:	text = "rests in depths"	; break;
  case SECT_WATER_NOSWIM:text = "stands in water"	; break;
  case SECT_SWAMP:	text = "stands in mud"		; break;
  case SECT_AIR:	text = "floats through air"	; break;
  case SECT_DESERT:	text = "stands in sand"		; break;
  case SECT_LAVA:	text = "stands in fire"		; break;
  case SECT_SNOW:	text = "stands in ice"		; break;
  }

  sprintf(buf, "I.. I see a giant obelisk.. It %s in %s..",
	  text, obj->in_room->name);
  do_say(ch, buf);
  sprintf(buf, "and a specter of %s looms on the horizon.", pArea->name);
  do_say(ch, buf);
  act("$n arises and opens $s eyes.", ch, NULL, NULL, TO_ROOM);
  do_say(ch, "I'm sorry child, but that is all He has shown me.");
}

/* checks if any demons, undead, or vendetta cabaled chars are in the first area 
this char's cabal connects to
*/
bool spec_knight_seer(CHAR_DATA *ch){
  AREA_DATA* pa = NULL;
  CHAR_DATA* vch;
  EXIT_DATA* pExit;
  AFFECT_DATA af;

  int door = 0;

  bool fEnemy = FALSE;
  bool fDemon = FALSE;
  bool fUndead = FALSE;

  char buf[MIL];

  if (ch->pCabal == NULL || ch->pCabal->present < 1)
    return FALSE;
  else if (ch->pCabal->anchor == NULL)
    return FALSE;
  else if (is_affected(ch, gsn_identify))
    return FALSE;

  /* get the area this cabal connects to */
  for ( door = 0; door  < MAX_DOOR; door ++ ){
    pExit = ch->pCabal->anchor->exit[door];

    if (pExit == NULL || pExit->to_room == NULL)
      continue;
    if (pExit->to_room->area == ch->pCabal->anchor->area)
      continue;
    pa = pExit->to_room->area;
    break;
  }
  if (pa == NULL || pa->nplayer < 1)
    return FALSE;

  /* we have the area, run through players that are in there */
  for (vch = player_list; vch; vch = vch->next_player ){
    if (vch->in_room == NULL || vch->in_room->area != pa )
      continue;
    if (IS_UNDEAD(vch))
      fUndead = TRUE;
    else if (IS_DEMON(vch))
      fDemon = TRUE;

    if (vch->pCabal && is_friendly(vch->pCabal, ch->pCabal) == CABAL_ENEMY)
      fEnemy = TRUE;

    if ( fUndead && fDemon && fEnemy)
      break;
  }
  if (!fUndead && !fDemon && !fEnemy)
    return FALSE;
    
/* print the message */
  if (fEnemy)
    sprintf( buf, "[%s]: 'Beware.. An enemy draws near!'", PERS2(ch));
  else if (fUndead && fDemon)
    sprintf( buf, "[%s]: 'Beware.. An Unholy and Demonic presence draws near!'", PERS2(ch));
  else if (fUndead)
    sprintf( buf, "[%s]: 'Beware.. An Unholy presence draws near!'", PERS2(ch));
  else if (fDemon)
    sprintf( buf, "[%s]: 'Beware.. A Demonic presence draws near!'", PERS2(ch));
  else
    sprintf( buf, "[%s]: 'I sense a presence that I should not!'", PERS2(ch));

  cabal_echo(ch->pCabal, buf );

  /* wait so we do not spam */
  af.type = gsn_identify;
  af.level = 60;
  af.duration = 12;
  af.where = TO_NONE;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char( ch, &af);

  return TRUE;
}

bool spec_nexus_nemesis(CHAR_DATA *ch){
  CHAR_DATA* victim = NULL;
  ROOM_INDEX_DATA* pRoom = NULL;
  /* decides the behavior of nemesis, teleports to rooms near its target if not in same area */

  if (ch->hunting == NULL && ch->fighting == NULL && (ch->in_room && ch->in_room->vnum != ch->homevnum)){
    act("$n grins with satisfaction as its form looses all detail and fades away.", ch, NULL, NULL, TO_ROOM );
    extract_char( ch, TRUE );
    return TRUE;
  }
  else{
    victim = ch->hunting;
    ch->hunttime = 999;
  }

  if (victim == NULL || victim->in_room == NULL || victim->in_room->vnum == ROOM_VNUM_LIMBO)
    return FALSE;

  /* now the acutal hunt part */
  if (victim->in_room->area != ch->in_room->area && number_percent() < 35){
    /* we get a random room that leads into this room to move to */
    ROOM_INDEX_DATA* exits[MAX_DOOR];
    EXIT_DATA* pe;
    int last_exit = 0;
    int door = 0;

    for (door = 0; door < MAX_DOOR; door ++ ){
      if ( (pe = victim->in_room->exit[door]) == NULL)
	continue;
      if (pe->to_room->exit[rev_dir[door]] == NULL || pe->to_room->exit[rev_dir[door]]->to_room != victim->in_room)
	continue;
      exits[last_exit++] = pe->to_room;
    }
    if (last_exit < 1)
      pRoom = NULL;
    else
      pRoom = exits[number_range(0, last_exit - 1)];
    
    if (pRoom){
      act("$n phases out and disappears.", ch, NULL, NULL, TO_ROOM);
      char_from_room( ch );
      char_to_room( ch, pRoom );
      act("$n phases in and flashes a terrible grin.", ch, NULL, NULL, TO_ROOM);
      return FALSE;
    }
  }
  return FALSE;
}
		   
void path_walk( CHAR_DATA* ch ){
  PATH_QUEUE* path, *curr, *prev = NULL;
  ROOM_INDEX_DATA* room_old;
  int temp_trust = ch->trust;

  path = ch->spec_path;

  if (ch->fighting || path == NULL)
    return;
  //make sure pets with a path stop if they enter their master's room
  else if (IS_AFFECTED(ch, AFF_CHARM) && ch->master && ch->master->in_room == ch->in_room){
    clean_path_queue(ch->spec_path);
    ch->spec_path = NULL;
    return;
  }

  /* exit this room, and enter next room */
  curr = path;
  prev = curr;

  /* try to use move_char first, if that doesn't work force the action */
  if (ch->in_room != curr->room){
    ch->trust = 6969;
    room_old = ch->in_room;
    move_char( ch, (int)curr->from_dir, TRUE );
  
    if (ch->in_room == NULL)
      return;
    else if (ch->fighting)
      return;
    else if (ch->in_room == room_old || ch->in_room != curr->room){
      act("$n leaves $t.", ch, dir_name[(int)path->from_dir], NULL, TO_ALL);
      char_from_room( ch );
      char_to_room( ch, curr->room );
      act("$n enters.", ch, dir_name[(int)path->from_dir], NULL, TO_ALL);
    }
    ch->trust = temp_trust;

    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master && ch->master->in_room == ch->in_room){
      clean_path_queue(ch->spec_path);
      ch->spec_path = NULL;
      return;
    }
  }

  /* repoint current path for next run*/
  ch->spec_path = curr->next;
  if (prev){
    prev->next = NULL;
    clean_path_queue( path );
  }
  /* run spec prog if we are at end */
  if ( (ch->spec_path == NULL || ch->spec_path->room == NULL)
       && HAS_TRIGGER_MOB(ch, TRIG_SPECIAL)){
    p_percent_trigger( ch, NULL, NULL, ch, NULL, NULL, TRIG_SPECIAL);    
  }
  return;
}
  
/* 
   walk along set path in ->spec_data pointer, takes ->mana[DICE_NUMBER] steps
*/
bool spec_path_walk(CHAR_DATA *ch){
  int i;

  for (i = 0; i < UMAX(1, ch->pIndexData->mana[DICE_NUMBER]); i++){
    path_walk( ch );
    if (ch == NULL || ch->in_room == NULL)
      return FALSE;
  }
  return TRUE;
}

/* controls behavior of raider mobs
 * In order of decision:
 * 0) If area is not city do nothing
 * 1) If a lawful mob is present attack it
 * 2) If a mob is present of level <= 50 attack it
 * 3) if we roll success vs torch_chance, torch the area
 *    - All mobs run away/die
 *    - A VNUM_OBJ_RAID is created in room
 *    - ch->pCabal gains cps and support
 *    - cabal owner of area loses cps and support
 *    - if area lawful, justice loses cps and support
 * 4) If we have no path, select new destination in area
 */
bool spec_raider_actions(CHAR_DATA *ch){
  CHAR_DATA* vch, *vch_next, *lawful = NULL, *target = NULL;
  char buf[MIL];
  OBJ_DATA* obj;
  int torch_chance = 20;
  const int dur = 24;
  int cps_dam;
  int sup_dam;

  //from this point on we have to be in a city or cabal home
  if ( !IS_AREA(ch->in_room->area, AREA_CITY) && !IS_AREA(ch->in_room->area, AREA_CABAL))
    return FALSE;
  if (ch->in_room == NULL)
    return FALSE;
  else if (ch->fighting)
    return FALSE;

  //check for mobs present in the room
  for (vch = ch->in_room->people; vch; vch = vch->next_in_room ){
    if (!IS_NPC(vch) 
	|| IS_AFFECTED(vch, AFF_CHARM) 
	|| vch == ch
	|| IS_SET(vch->act, ACT_RAIDER)
	|| is_same_cabal(ch->pCabal, vch->pCabal))
      continue;
    else if (!can_see( ch, vch))
      continue;
    else if (vch->level > 50 || IS_SET(vch->act, ACT_NOPURGE)
	     || IS_SET(vch->act, ACT_TOO_BIG)){
      torch_chance = 100;
      continue;
    }
    else if (IS_SET(vch->act2, ACT_LAWFUL))
      lawful = vch;
    else if (vch->pCabal && !is_same_cabal(ch->pCabal, vch->pCabal))
      target = vch;
    else
      target = vch;
  }

  //check if we have a lawful mob to attack
  if (lawful)
    vch = lawful;
  else if (target)
    vch = target;
  else
    vch = NULL;
  
  if (vch){
    sprintf(buf, "Help! I am being attacked by %s!",PERS(ch,vch));
    REMOVE_BIT(vch->comm, COMM_NOYELL);
    do_yell(vch, buf);
    SET_BIT(vch->comm, COMM_NOYELL);
    multi_hit( ch, vch, TYPE_UNDEFINED );
    return TRUE;
  }
  for (obj = ch->in_room->contents; obj; obj = obj->next_content){
    if (obj->vnum == OBJ_VNUM_RAID_OBJECT)
      break;
  }
  //in cabals the raiders can re-troch
  if (obj == NULL 
      || (IS_AREA(ch->in_room->area, AREA_CABAL) && obj->timer < dur - 2)){
    /* no targets to attack */
    if (number_percent() > torch_chance)
      return FALSE;
    
    /* torch the place */
    if (obj != NULL){
      obj->timer = dur;
      act("$n happily continues torching the surroundings.", ch, NULL, NULL, TO_ROOM);
    }
    else{
      act("Cackling with glee, $n sets the area ablaze.", ch, NULL, NULL, TO_ROOM);
      //create some rubble
      make_item_char(ch, OBJ_VNUM_RAID_OBJECT, dur);
    }

    //hurt reations
    if (ch->pCabal && ch->in_room->area->pCabal){
      affect_cabal_relations(ch->pCabal, ch->in_room->area->pCabal, -PACT_MAX_RATING / 20, TRUE );
    }

    //set area flag 
    if (ch->in_room->area->raid != NULL)
      SET_BIT(ch->in_room->area->area_flags, AREA_RAID);

    //force npcs to die in the blaze
    for (vch = ch->in_room->people; vch; vch = vch_next){
      vch_next = vch->next_in_room ;
      if (!IS_NPC(vch) 
	  || IS_AFFECTED(vch, AFF_CHARM) 
	  || vch == ch
	  || (vch->pCabal && vch->pCabal->altar == vch)
	  || IS_SET(vch->off_flags,CABAL_GUARD) 
	  || IS_SET(vch->act, ACT_RAIDER)
	  || is_same_cabal(ch->pCabal, vch->pCabal))
	continue;
      act("$n dies, trapped in the blazing rubble!", vch, NULL, NULL, TO_ROOM);
      extract_char( vch, TRUE );
    }
    
    sup_dam = number_range(1, 6 * CABAL_FULL_SUPPORT) / 6;
    cps_dam = number_range( CPTS, 2 * CPTS);

    /* HANDLE support-cp gain/loss */

    sup_dam = number_range(1, 6 * CABAL_FULL_SUPPORT) / 6;
    cps_dam = number_range( CPTS, 2 * CPTS);

    /* LOSS-GAIN  */
    if (ch->pCabal 
	&& ch->in_room->area->pCabal
	&& is_friendly(ch->pCabal, ch->in_room->area->pCabal) == CABAL_ENEMY){

      //if target area has cabal with no members lower gain
      if (ch->in_room->area->pCabal->present < 1){
	sup_dam = sup_dam / 3;
	cps_dam = cps_dam / 3;
      }

      /* LOSS */
      if (ch->in_room->area->pCabal->present > 0){
	ch->in_room->area->pCabal->raid_sup -= sup_dam;
	ch->in_room->area->pCabal->raid_cps -= cps_dam;
      }

    /* GAIN */
      ch->pCabal->raid_sup += sup_dam;
      ch->pCabal->raid_cps += cps_dam;
    }
    if (IS_AREA(ch->in_room->area, AREA_LAWFUL)){
      justice_raid_gain( -sup_dam / 2, -cps_dam / 2 );
    }
  }
  //set a new path
  else if (ch->spec_path == NULL){
    ROOM_INDEX_DATA* pTo;
    ROOM_INDEX_DATA* pFrom = ch->in_room;
    PATH_QUEUE* path;

    int sectors[2] = {SECT_CITY, SECT_INSIDE};
    int area_pool = 10;
    int dist = 0;

    pTo =  get_rand_room(pFrom->area->vnum,pFrom->area->vnum,//area range (0 to ignore)
			 0,0,			//room ramge (0 to ignore)
			 0,0,			//areas to choose from
			 NULL,0,		//areas to exclude
			 sectors,2,		//sectors required
			 NULL,0,		//sectors to exlude
			 NULL,0,		//room1 flags required
			 0,0,			//room1 flags to exclude
			 NULL,0,		//room2 flags required
			 NULL,0,		//room2 flags to exclude
			 area_pool,		//number of seed areas
			 TRUE,			//exit required?
			 FALSE,			//Safe?
			 NULL);			//Character for room checks
    if (pTo == NULL || pTo == pFrom)
      return FALSE;
    //see if we can get a path between the two rooms
    if ( (path = generate_path( pFrom, pTo, 256, TRUE, &dist, NULL)) == NULL){
      clean_path();
      return FALSE;
    }
    else
      clean_path();  
    ch->spec_path = path;
    return FALSE;
  }
  return FALSE;
};

//handles the destruction of a room
bool awakenlife_roomdestroy( CHAR_DATA* ch, ROOM_INDEX_DATA* room ){
  CHAR_DATA* vch, *vch_next, *lawful = NULL, *target = NULL;
  OBJ_DATA* obj;
  char buf[MIL];
  const int dur = 24;
  int cps_dam, sup_dam;

  /* Rules for destruction:
     1) Any mobs of rank 50 and lower are attacked
     2) Mobs higher then 50 or ones that have NO_PURGE are destroyed
     3) CP gain/loss is 1:1 for victim cabal vs owner of mob
  */

  //check for mobs present in the room
  for (vch = room->people; vch; vch = vch->next_in_room ){
    if (!IS_NPC(vch) 
	|| IS_SET(vch->act, ACT_NONCOMBAT)
	|| IS_AFFECTED(vch, AFF_CHARM) 
	|| vch == ch
	|| IS_SET(vch->act, ACT_RAIDER)
	|| is_same_cabal(ch->pCabal, vch->pCabal))
      continue;
    else if (!can_see( ch, vch))
      continue;
    else if (vch->level > 50 
	     || IS_SET(vch->act, ACT_NOPURGE)
	     || IS_SET(vch->act, ACT_TOO_BIG)){
      continue;
    }
    else if (IS_SET(vch->act2, ACT_LAWFUL))
      lawful = vch;
    else if (vch->pCabal 
	     && !is_same_cabal(ch->pCabal, vch->pCabal)
	     && is_friendly(ch->pCabal, vch->pCabal) == CABAL_ENEMY)
      target = vch;
    else
      target = vch;
  }

  //check if we have a lawful mob to attack
  if (lawful)
    vch = lawful;
  else if (target)
    vch = target;
  else
    vch = NULL;
  
  //If we have a mob to attack attack it now
  if (vch){
    sprintf(buf, "Help! I am being attacked by %s!",PERS(ch,vch));
    REMOVE_BIT(vch->comm, COMM_NOYELL);
    do_yell(vch, buf);
    SET_BIT(vch->comm, COMM_NOYELL);
    multi_hit( ch, vch, TYPE_UNDEFINED );
    return FALSE;
  }

  /* BELOW THIS POINT HAPPENDS ONLY IF THERE ARE NO MOBS TO ATTACK */
  //check for rubble from previous destruction
  for (obj = ch->in_room->contents; obj; obj = obj->next_content){
    if (obj->vnum == OBJ_VNUM_RAID_OBJECT)
      break;
  }

  //in cabals the monster can re-troch
  if (obj == NULL || (IS_AREA(ch->in_room->area, AREA_CABAL) && obj->timer < dur - 2)){
    /* torch the place */
    if (obj != NULL){
      obj->timer = dur;
      act("Roaring with rage $n destroys the remains of the area..", ch, NULL, NULL, TO_ROOM);
    }
    else{
      act("Roaring with rage $n reduces the area to rubble.", ch, NULL, NULL, TO_ROOM);
      //create some rubble
      make_item_char(ch, OBJ_VNUM_RAID_OBJECT, dur);
    }

    //hurt reations
    if (ch->pCabal && ch->in_room->area->pCabal){
      affect_cabal_relations(ch->pCabal, ch->in_room->area->pCabal, -PACT_MAX_RATING / 20, TRUE );
    }

    //force npcs to die in the blaze
    for (vch = ch->in_room->people; vch; vch = vch_next){
      vch_next = vch->next_in_room ;
      if (!IS_NPC(vch) 
	  || (vch->pCabal && vch->pCabal->altar == vch)
	  || IS_SET(vch->act, ACT_NONCOMBAT)
	  || IS_SET(vch->act, ACT_IS_HEALER)
	  || IS_AFFECTED(vch, AFF_CHARM) 
	  || vch == ch
	  || IS_SET(vch->off_flags,CABAL_GUARD) 
	  || IS_SET(vch->act, ACT_RAIDER)
	  || (vch->pCabal && is_friendly(ch->pCabal, vch->pCabal) != CABAL_ENEMY))
	continue;
      act("$n dies, trapped in the blazing rubble!", vch, NULL, NULL, TO_ROOM);
      extract_char( vch, TRUE );
    }

    /* HANDLE support-cp gain/loss */

    if (IS_AREA(ch->in_room->area, AREA_CITY) || IS_AREA(ch->in_room->area, AREA_CABAL)){
      sup_dam = number_range(1, 10);
      cps_dam = number_range( 2 * CPTS, 4 * CPTS);
    }
    else{
      sup_dam = 0;
      cps_dam = 0;
    }
    /* LOSS-GAIN  */
    if (ch->pCabal 
	&& ch->in_room->area->pCabal
	&& is_friendly(ch->pCabal, ch->in_room->area->pCabal) == CABAL_ENEMY){

      //if target area has cabal with no members lower gain
      if (ch->in_room->area->pCabal->present < 1){
	sup_dam = sup_dam / 3;
	cps_dam = cps_dam / 3;
      }

      /* LOSS */
      if (ch->in_room->area->pCabal->present > 0){
	ch->in_room->area->pCabal->raid_sup -= sup_dam;
	ch->in_room->area->pCabal->raid_cps -= cps_dam;
      }

    /* GAIN */
      ch->pCabal->raid_sup += sup_dam;
      if (ch->summoner != NULL)
	CPS_GAIN( ch->summoner, cps_dam, TRUE );
    }

    //justice loss
    if (IS_AREA(ch->in_room->area, AREA_LAWFUL)){
      justice_raid_gain( -sup_dam / 2, -cps_dam / 2 );
    }

  }
  return TRUE;
}

//handles targetting of new destination for RAMPAGE state.
void awakenlife_rampage( CHAR_DATA* ch){
  ROOM_INDEX_DATA* pTo;
  ROOM_INDEX_DATA* pFrom = ch->in_room;
  
  int area_pool = 10;

  pTo =  get_rand_room(pFrom->area->vnum, pFrom->area->vnum,//area range (0 to ignore)
		       0,0,			//room ramge (0 to ignore)
		       0,0,			//areas to choose from
		       NULL,0,			//areas to exclude
		       NULL,0,			//sectors required
		       NULL,0,		//sectors to exlude
		       NULL,0,		//room1 flags required
		       0,0,			//room1 flags to exclude
		       NULL,0,		//room2 flags required
		       NULL,0,		//room2 flags to exclude
		       area_pool,		//number of seed areas
		       TRUE,			//exit required?
		       FALSE,			//Safe?
		       NULL);			//Character for room checks

  if (pTo == NULL || pTo == pFrom)
    return;
  else
    set_path( ch, pFrom, pTo, 256, NULL);

  return;
}

//handles behavior of the awaken life monsters.
//current orders are stored in pc's status variable.
bool spec_awakenlife_actions( CHAR_DATA *ch ){
  int state;

  if (ch == NULL || ch->in_room == NULL)
    return FALSE;
  else
    state = ch->status;

  //safety to make sure the monsters are always linked.
  if (ch->summoner == NULL || IS_NPC(ch->summoner) || ch->summoner->pcdata->familiar != ch){
    act("Not sensing its master's presence $N returns to its homeland.", ch, NULL, NULL, TO_ROOM);
    stop_fighting( ch, TRUE );
    char_from_room( ch );
    extract_char( ch, TRUE );
    return TRUE;
  }
  else if (ch->pCabal && is_captured(ch->pCabal)){
    act("With the $t Standard captured, $n returns to its homeland.", ch, ch->pCabal->name, NULL, TO_ROOM);
    stop_fighting( ch, TRUE );
    char_from_room( ch );
    extract_char( ch, TRUE );
    return TRUE;
  }    
  //do nothing if we are fighting
  if (ch->fighting != NULL)
    return FALSE;
  // we do nothing while moving except while on rampage
  else if (ch->spec_path != NULL && state != AWAKENLIFE_RAMP)
    return FALSE;

  
  switch (state){
    //move or retreat states do nothing except move
  default:
  case AWAKENLIFE_NONE:
    break;
  case AWAKENLIFE_MOVE:
  case AWAKENLIFE_RETR:
    sendf( ch->summoner, "[%s]: Has reached the target destination.\n\r", PERS2(ch));
    ch->status = AWAKENLIFE_NONE;
    break;
    //destroy and rampage state destroys the room
  case AWAKENLIFE_DEST:
  case AWAKENLIFE_RAMP:
    if (awakenlife_roomdestroy( ch, ch->in_room )){
      if (ch != NULL && ch->in_room != NULL && state == AWAKENLIFE_DEST){
	sendf( ch->summoner, "[%s]: Target destroyed.\n\r", PERS2(ch));
	ch->status = AWAKENLIFE_NONE;
      }
    }
    //stop further execution if this mob is dead now
    if (ch == NULL || ch->in_room == NULL)
      return TRUE;
    //if this is rampage, get new destination if we have no path
    if (state == AWAKENLIFE_RAMP && ch->fighting == NULL)
      awakenlife_rampage( ch );
    break;
  }
  return FALSE;
}
