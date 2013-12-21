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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "cabal.h"
#include "magic.h"
#include "recycle.h"
#include "interp.h"
#include "jail.h"


const struct warlord_type warlord_table[] =
{
    {	"cure blindness"	},
    {	"cure disease"		},
    {	"cure poison"		},
    {	"know alignment"	},
    {	"refresh"		},
    {	"remove curse"		},
    {	"word of recall"	},
    {	"cancellation"		},
    {	"doppelganger"		},
    {	"slow"			},
    {	NULL			}
};

char *target_name;
extern bool is_sheathed(CHAR_DATA* ch);
bool check_spellblast( CHAR_DATA* ch, CHAR_DATA* victim, int sn, int cost, int level );


//blademaster snakespeed check for spell counter, ch is caster, victim the blademaster
bool snakespeedCheck(CHAR_DATA* ch, CHAR_DATA* victim){
  OBJ_DATA* obj;

  int sn = skill_lookup("snakespeed");
  int ini_dam, dam;

  if (ch == NULL || victim == NULL)
     return FALSE;

  ini_dam = dam = dice(6, 6) + number_range(victim->level, 2 * victim->level);
  /* Useless conditional */
  if( ini_dam != dam )
    ini_dam = dam;

  if (victim->fighting != NULL)
    return FALSE;
  else if (IS_IMMORTAL(ch))
    return FALSE;
  else if (IS_NPC(ch) && ch->hit < ch->max_hit / 2)
    return FALSE;
  else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD)
    return FALSE;
  else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_HOUND)
    return FALSE;
  else if ( (obj = has_twohanded(victim)) == NULL)
    return FALSE;
  else if (!can_see(victim, ch) && !is_affected(victim, gsn_battlefocus)){
    return FALSE;
  }
  else if (!monk_good(victim, WEAR_HANDS))
    return FALSE;
  else if (number_percent() > get_skill(victim, sn)){
    check_improve(victim, sn, FALSE, 10);
    return FALSE;
  }
  else if (victim->position < POS_STANDING && victim->position > POS_STUNNED){
    const int gsn_vigil = skill_lookup("vigil");
    //blademaster vigil
    if (!IS_AFFECTED(victim, AFF_SLEEP) && number_percent() < get_skill(victim, gsn_vigil)){
      send_to_char("You sense an impeding attack!\n\r", victim);
      act("$n springs up suddenly $s weapons flashing in a deadly arc!", victim, NULL, NULL, TO_ROOM);
      check_improve(victim, gsn_vigil, TRUE, 30);
      dam = 3 * dam / 2;
      do_wake(victim, "");
    }
    else
      return FALSE;
  }

  check_improve(victim, sn, TRUE, 5);

  act("With the speed of a serpent you strike and interrupt $N!",victim,NULL,ch,TO_CHAR);
  act("With the speed of a serpent $n strikes and interrupts $N!",victim,NULL,ch,TO_NOTVICT);
  act("With the speed of a serpent $n strikes and interrupts you!",victim,NULL,ch,TO_VICT);

  damage(victim, ch, dam, sn, attack_table[obj->value[3]].damage,  TRUE );
  return TRUE;
}

int PRESPELL_CHECK(CHAR_DATA* ch, char* argument, int* sn, int* cost, int* lag, int* lvl)
{
  //includes all updates before mana and lag is set.
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int fSuccess = TRUE;
  int result = 0;

  if (skill_table[*sn].spell_type != SPELL_MALEDICTIVE){
    int old_lvl = *lvl;
    run_effect_update(ch, sn, lvl,  gen_mana_charge, cost, lag, TRUE, EFF_UPDATE_PRESPELL);
    /* non afflictive spells do not have their levels changed */
    if (skill_table[*sn].spell_type != SPELL_AFFLICTIVE)
      *lvl = old_lvl;
  }

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }

//SPECIAL GENS HERE
    if (paf->type == gen_mana_charge) {
      continue;
    }

    if (IS_GEN(paf->type)) {
      if ((result =  run_effect_update(ch, sn, lvl,  paf->type, cost,
				       lag, TRUE, EFF_UPDATE_PRESPELL)) != TRUE) {
	if (result == -1) {
	  return -1;
	}
	else {
	  fSuccess = result;
	}
      }
    }
  }

  *lag = *lag * (100 + ch->aff_mod[MOD_WAIT_STATE]) / 100;
  return fSuccess;
}//and PRESPELL

int POSTSPELL_CHECK(CHAR_DATA* ch, char* argument, int sn, int cost, int lag)
{
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int fSuccess = TRUE;
  int result = 0;

  run_effect_update(ch, NULL, NULL, gen_mana_charge, &cost, &lag, TRUE, EFF_UPDATE_POSTSPELL);

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }

//SPECIAL GENS HERE
    if (paf->type == gen_mana_charge) {
      continue;
    }

    if (IS_GEN(paf->type)) {
      if ((result =  run_effect_update(ch, &sn, NULL,  paf->type, &cost,
				       &lag, TRUE, EFF_UPDATE_POSTSPELL)) != TRUE) {
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

}//end post spell

/* Viri:
Calculates the amount of exp. gained for casting a spell
*/
int spell_expgain(int sn, int skill, int level){
  int min = skill_table[sn].min_mana / 8;
  int max = skill_table[sn].min_mana / 4;
  int val  = 0;

  if (skill < 2 || skill > 99 || level > 38)
    return 0;

  min = UMAX(1, min);
  max = UMAX(1, max);
  val =  number_range(min, max);

  /* adjust for level */
  val = val / UMAX(1, (level - 30) / 2);
  return val;
}


void m_yell( CHAR_DATA *ch, CHAR_DATA *vch, bool force ) {
  char buf[MSL];
  if (((ch->fighting != vch && vch->fighting != ch && ch != vch)
       || force) && (!IS_NPC(ch) || ch->master != NULL )) {
    if (!IS_NPC(vch)) {
      sprintf(buf,"`&%s attacked %s at %d``", ch->name, vch->name,ch->in_room->vnum);
      log_string(buf);
      wiznet(buf,NULL,NULL,WIZ_FIGHTING,0,0);
      continuum(vch, CONTINUUM_ATTACK);
/* MURDER CRIME CHECK */
      if (ch->in_room && is_crime(ch->in_room, CRIME_MURDER, vch)){
	set_crime(ch, vch, ch->in_room->area, CRIME_MURDER);
      }
/* ATTACKING JUSTICE MOBS IN LAWFUL AREA IS ALWYAS A CRIME */
      else if (vch->pCabal && IS_CABAL(vch->pCabal, CABAL_JUSTICE) && IS_AREA(ch->in_room->area, AREA_LAWFUL)){
	set_crime(ch, vch, ch->in_room->area, CRIME_MURDER);
      }
      if (!can_see(vch, ch))
	sprintf(buf,"Help someone is attacking me!");
      else
	sprintf(buf, "Die! %s, you sorcerous dog!",PERS(ch,vch));
      j_yell(vch, buf);

      /* CONTINENCY */
      check_contingency(ch, NULL, CONT_ATTACK);
      check_contingency(vch, NULL, CONT_ATTACKED);
    }//END PC
    else {
      bool fCrime = FALSE;
/* ASSAULT CRIME CHECK */
      if (ch->in_room && is_crime(ch->in_room, CRIME_ASSAULT, vch)){
	fCrime = TRUE;
	set_crime(ch, vch, ch->in_room->area, CRIME_ASSAULT);
      }
/* ATTACKING JUSTICE MOBS IN LAWFUL AREA IS ALWYAS A CRIME */
      else if (vch->pCabal && IS_CABAL(vch->pCabal, CABAL_JUSTICE) && IS_AREA(ch->in_room->area, AREA_LAWFUL)){
	set_crime(ch, vch, ch->in_room->area, CRIME_MURDER);
	fCrime = TRUE;
      }
      if (IS_SET(vch->off_flags,GUILD_GUARD) && vch->pCabal && IS_CABAL(vch->pCabal, CABAL_JUSTICE)){
	sprintf(buf, "%s: Alert! The jail is being attacked!", ch->in_room->area->name);
	cabal_echo_flag( CABAL_JUSTICE, buf );
	sprintf(buf, "Alert! The jail is being attacked by %s!",PERS(ch,vch));
      }
      else if (IS_SET(vch->off_flags,GUILD_GUARD))
	sprintf(buf, "Alert! The guild is being attacked by %s!",PERS(ch,vch));
      else if (!can_see(vch, ch))
	sprintf(buf,"Help someone is attacking me!");
      else
	sprintf(buf, "Die! %s, you sorcerous dog!",PERS(ch,vch));

      if (vch->hit > 0 && (fCrime || IS_SET(vch->off_flags,GUILD_GUARD)) ) {
	REMOVE_BIT(vch->comm,COMM_NOYELL);
	REMOVE_BIT(vch->comm,COMM_NOCHANNELS);
	j_yell(vch, buf);
	SET_BIT(vch->comm,COMM_NOYELL);
	SET_BIT(vch->comm,COMM_NOCHANNELS);
      }
    }
  }//END NPC
}

int skill_lookup( const char *name ) {
  int sn;
  for ( sn = 0; sn < (MAX_SKILL + MAX_EFFECTS); sn++ ) {
    if (IS_GEN(sn)) {
      if ( effect_table[GN_POS(sn)].name == NULL ) {
	break;
      }

      if ( LOWER(name[0]) == LOWER(effect_table[GN_POS(sn)].name[0])
	   && !str_prefix( name, effect_table[GN_POS(sn)].name ) ) {
	      return sn;
      }
    }

    else {
      if ( skill_table[sn].name == NULL ) {
	continue;
      }

      if ( LOWER(name[0]) == LOWER(skill_table[sn].name[0])
	   && !str_prefix( name, skill_table[sn].name ) ) {
	return sn;
      }
    }
  }//end for

  return -1;
}

int find_spell( CHAR_DATA *ch, const char *name )
{
    int sn, found = -1;
    if (IS_NPC(ch))
	return skill_lookup(name);
    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if (skill_table[sn].name == NULL)
	    break;
	if (LOWER(name[0]) == LOWER(skill_table[sn].name[0])
        && !str_prefix(name,skill_table[sn].name))
	{
            if ( found == -1)
		found = sn;
	    if (ch->level >= sklevel(ch,sn) && get_skill(ch,sn) > 0)
                return sn;
	}
    }
    return found;
}

int slot_lookup( int slot )
{
/*    extern bool fBootDb;
    int sn;
    if ( slot <= 0 )
	return -1;
    for ( sn = 0; sn < MAX_SKILL; sn++ )
	if ( slot == skill_table[sn].slot )
	    return sn;
    if ( fBootDb )
    {
	bug( "Slot_lookup: bad slot %d.", slot );
	abort( );
    } */
    return slot;
}


void say_spell( CHAR_DATA *ch, int sn )
{
    char buf [MSL], buf2 [MSL];
    CHAR_DATA *rch;
    char *pName;
    int iSyl, length;
    struct syl_type
    {
	char *	old;
	char *	new;
    };
    static const struct syl_type syl_table[] =
    {
	{ " ",		" "		},
	{ "ar",		"abra"		},
	{ "au",		"kada"		},
	{ "bless",	"fido"		},
	{ "blind",	"nose"		},
	{ "bur",	"mosa"		},
	{ "cu",		"judi"		},
	{ "de",		"oculo"		},
	{ "en",		"unso"		},
	{ "light",	"dies"		},
	{ "lo",		"hi"		},
	{ "mor",	"zak"		},
	{ "move",	"sido"		},
	{ "ness",	"lacri"		},
	{ "ning",	"illa"		},
	{ "per",	"duda"		},
	{ "ra",		"gru"		},
	{ "fresh",	"ima"		},
	{ "re",		"candus"	},
	{ "son",	"sabru"		},
	{ "tect",	"infra"		},
	{ "tri",	"cula"		},
	{ "ven",	"nofo"		},
	{ "a", "a" }, { "b", "b" }, { "c", "q" }, { "d", "e" },
	{ "e", "z" }, { "f", "y" }, { "g", "o" }, { "h", "p" },
	{ "i", "u" }, { "j", "y" }, { "k", "t" }, { "l", "r" },
	{ "m", "w" }, { "n", "i" }, { "o", "a" }, { "p", "s" },
	{ "q", "d" }, { "r", "f" }, { "s", "g" }, { "t", "h" },
	{ "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
	{ "y", "l" }, { "z", "k" },
	{ "", "" }
    };
    buf[0] = '\0';
    for ( pName = skill_table[sn].name; *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(syl_table[iSyl].old)) != 0; iSyl++ )
	    if ( !str_prefix( syl_table[iSyl].old, pName ) )
	    {
		strcat( buf, syl_table[iSyl].new );
		break;
	    }
	if ( length == 0 )
	    length = 1;
    }
    sprintf( buf2, "$n utters the words, '%s'.", buf );
    sprintf( buf,  "$n utters the words, '%s'.", skill_table[sn].name );
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	if ( rch != ch )
            act(((!IS_NPC(rch) && ch->class==rch->class) || is_affected(rch, gsn_com_lan)) ? buf : buf2,ch, NULL, rch, TO_VICT );
}

int calc_saves(int level, CHAR_DATA* victim, int dam_type, int spell_type ){
  int save;

  if (IS_NPC(victim)){
    save = 40 - victim->savingspell;
    save += (( victim->level - level) * 5);
  }
  else{
    save = 40 - victim->savingspell;
    save += (( victim->level - level) * 3);
  }
  if (IS_AFFECTED(victim,AFF_BERSERK) || IS_AFFECTED2(victim,AFF_RAGE))
    save += victim->level/10;
  if (spell_type == SPELL_MALEDICTIVE){
    save -= victim->savingmaled;
    save += get_curr_stat(victim,STAT_CON) - 19;
  }
  else if (spell_type == SPELL_AFFLICTIVE){
    save -= victim->savingaffl;
    save += get_curr_stat(victim,STAT_STR) - 19;
  }
  else{
    save -= victim->savingmental;
    save += get_curr_stat(victim,STAT_INT) - 19;
  }
  if (is_affected(victim,gsn_buddha)
      && (dam_type == DAM_MENTAL || dam_type == DAM_DISEASE || dam_type == DAM_POISON)){
    save += 10;
    }

  switch(check_immune(victim,dam_type, TRUE)){
  case IS_IMMUNE:         return 999;
  case IS_RESISTANT:      save += 15;      break;
  case IS_VULNERABLE:     save -= 15;      break;
  }
  save += (get_curr_stat(victim,STAT_LUCK) - 16);
  save = URANGE( 5, save, 95 );

  return (save);
}

bool saves_spell( int level, CHAR_DATA *victim, int dam_type, int spell_type )
{
  int save = calc_saves(level, victim, dam_type, spell_type );

  if (spell_type == -1)
    return FALSE;

  if (!IS_NPC(victim) && IS_SET(victim->act2,PLR_GIMP))
    return FALSE;

  return number_percent( ) < save;
}

bool saves_dispel( int dis_level, int spell_level, int duration)
{
    int save;
    if (duration == -1)
        return TRUE;
    save = 50 + (spell_level - dis_level) * 5;
    save = URANGE( 5, save, 95 );
    return number_percent( ) < save;
}

bool check_dispel( int dis_level, CHAR_DATA *victim, int sn){
  AFFECT_DATA *af;
  for ( af = victim->affected; af != NULL; af = af->next ){
    if ( af->type == sn ){
      if (dis_level == -99 || !saves_dispel(dis_level,af->level,af->duration)){
	if (sn == gsn_duplicate){
	  if (IS_NPC(victim) && IS_SET(victim->act2,ACT_DUPLICATE)){
	    act("$n is dispelled.",victim,NULL,NULL,TO_ROOM);
	    char_from_room(victim);
	    extract_char(victim,TRUE);
	    return TRUE;
	  }
	  else
	    return FALSE;
	}
	affect_strip(victim,sn);
	if(!IS_GEN(sn)){
	  act_new(skill_table[sn].msg_off, victim, NULL, NULL, TO_CHAR, POS_DEAD);
	  act(skill_table[sn].msg_off2, victim, NULL, NULL, TO_ROOM);
	}
	return TRUE;
      }
      else
	af->level -= 1;
    }
  }
  return FALSE;
}





/////////////////
//spell_lvl_mod//
/////////////////
//Factors class modifires into the spell power
//as much as I would like a bit more fine control
//of the bonuses, teh cleanes way to do this
//is to give lvl bonus to the spell.

inline int spell_lvl_mod(CHAR_DATA* ch, int spell_number)
{
  int lvl_mod = 0;
  int level = 0;
  int af_mod = 0;



  if (ch->class == class_lookup("vampire") && !str_cmp(skill_table[spell_number].name, "energy drain") )
    {
      level	= UMIN(ch->level, sizeof(energy_drain_bonus)/sizeof(energy_drain_bonus[0]) - 1);
      lvl_mod	+= UMAX(0, energy_drain_bonus[level]);
    }

  if (skill_table[spell_number].spell_type == SPELL_MALEDICTION){
    if ( ch->class != class_lookup("shaman"))
      lvl_mod -= 3;
  }
  else if (spell_number == gsn_dispel_magic
	   || spell_number == gsn_ecstacy
	   || spell_number == gsn_sleep)
    lvl_mod -= 3;

  /* Tattoos */
  if (ch->tattoo == deity_lookup("knowledge")){
    if (skill_table[spell_number].spell_type != SPELL_MALEDICTION)
      lvl_mod += 1;
  }
  /* check effects */
  af_mod += UMIN(2, ch->aff_mod[MOD_SPELL_LEVEL]);
  if (skill_table[spell_number].spell_type == SPELL_AFFLICTIVE)
    af_mod +=  UMIN(2, ch->aff_mod[MOD_AFFL_LEVEL]);
  if (skill_table[spell_number].spell_type == SPELL_MENTAL)
    af_mod += UMIN(2, ch->aff_mod[MOD_MENTAL_LEVEL]);
  if (skill_table[spell_number].spell_type == SPELL_MALEDICTION)
    af_mod += UMIN(2, ch->aff_mod[MOD_MALED_LEVEL]);
  lvl_mod += UMIN(3, af_mod);
//RETURN FINAL SPELL LVL
      return ch->level + lvl_mod;
}//END spell_lvl_mod



//////////////
//_mana_cost//
/////////////
//_mana_cost will replace above function as ssoon as I can
//confirm that it will not affect anything else.
//Calculates cost of a spell, talking into effect any race/
//class modifiers.

inline int _mana_cost(CHAR_DATA* ch, int spell_number)
{
  int mana,  mana_mod=0, skill;

//We set the modifer now depending on the class/race
//In the future if there are multiple additions here
//Then an LUT will have to be setup.


//First calculate true cost so it can be factored into calculations below
//if needed.



  if (ch->level +2 == sklevel(ch, spell_number))
    mana = 50;
  else
    mana = UMAX(skill_table[spell_number].min_mana, 100/(2 + ch->level - sklevel(ch, spell_number)));

//NOW calculate the bonus/penalty.
  if (ch->class == class_lookup("vampire") && !str_cmp(skill_table[spell_number].name, "energy drain") )
       mana_mod += -10;
   else if (ch->class == class_lookup("vampire") && !str_cmp(skill_table[spell_number].name, "vampiric touch") )
       mana_mod += -10;
   else if (ch->class == class_lookup("dark-knight") && spell_number == gsn_poison )
       mana_mod += +5;
   else if (ch->class == class_lookup("dark-knight") && spell_number == gsn_plague )
       mana_mod += +10;
   else if (ch->class == class_lookup("shaman") && spell_number == gsn_plague )
       mana_mod += -5;
   else if (ch->class == class_lookup("shaman") && spell_number == gsn_blindness )
       mana_mod += -10;
   else if (ch->class == class_lookup("shaman") && spell_number == gsn_poison )
       mana_mod += -10;

  //OTHER BONUSES

  /* Tattoos */
  if (ch->tattoo == deity_lookup("knowledge"))
    mana_mod += -mana / 4;

  if (is_affected(ch, gsn_corrupt))
    mana_mod += 5 + mana / 3;
  if (is_affected(ch, gsn_ocularis))
    mana_mod += mana / 4;
  /* vortex tap */
  if ( (skill = get_skill(ch, gsn_ancient_lore)) > 1){
    mana_mod -=( skill * 33 * (mana + mana_mod) / 10000);
  }


  mana_mod = mana_mod * (100 + ch->aff_mod[MOD_SPELL_COST]) / 100;
//RETURN THE FINAL MANA COST.

  return (UMAX(1, mana+mana_mod));
}


void cast_new( CHAR_DATA *ch, char *argument, bool iscommune, bool fIgnorePos )
{
  CHAR_DATA *victim = NULL;
  OBJ_DATA *obj = NULL;
  AFFECT_DATA* paf;

  char arg1[MIL], arg2[MIL];

  int spell_lag = 0;
  int spell_lvl = 0;
  int skill = 0;

  void *vo = NULL;
  bool fyell = FALSE, syell = TRUE, havepower = TRUE;
  int mana, sn, target = TARGET_NONE, door, concen;

  if (ch->in_room == NULL){
    bug( "cast_new: mob not in room.", 0 );
    return;
  }
  if (IS_GAME(ch, GAME_NOSPELLS))
    havepower = FALSE;
  else if (ch->pCabal && ch->class != gcn_blademaster && IS_CABAL(ch->pCabal, CABAL_NOMAGIC))
    havepower = FALSE;

  if (!IS_NPC(ch) && !IS_IMMORTAL(ch) && !havepower){
    send_to_char("You have lost your powers!\n\r",ch);
    return;
  }
  else if (is_affected(ch, gsn_forget)){
    send_to_char("You seemed to have forgotten all your spells.\n\r",ch);
    return;
  }
  else if (is_affected(ch, gsn_headbutt)){
    send_to_char("Your head is still spinning, you can't focus enough.\n\r",ch);
    return;
  }
  else if (!IS_IMMORTAL(ch) && IS_SET(ch->in_room->room_flags2,ROOM_NO_MAGIC)){
    send_to_char("Something is interfering with your casting abilities.\n\r",ch);
    return;
  }
  else if (vamp_day_check(ch)){
    send_to_char("You lack the power.\n\r",ch);
    return;
  }

  /* cost of spell for corrupt is increased in _mana_cost */
  if (is_affected(ch, gsn_corrupt))
    send_to_char("Your mind is taxed as you overcome influence of Chaos around you.\n\r", ch);
  target_name = one_argument( argument, arg1 );
  one_argument( target_name, arg2 );

  if ( arg1[0] == '\0' ){
    send_to_char( "Cast which what where?\n\r", ch );
    return;
  }
  else if ((sn = find_spell(ch,arg1)) < 1
	   || skill_table[sn].spell_fun == spell_null
	   || ((skill = get_skill(ch,sn)) < 1 && !IS_NPC(ch)) ){
    send_to_char( "You don't know any spells of that name.\n\r", ch );
    return;
  }
  else if (skill < 2 ){
    send_to_char("You lack the skill neccessary.\n\r", ch);
    return;
  }
  else if ( !fIgnorePos && ch->position < skill_table[sn].minimum_position ){
    send_to_char( "You can't concentrate enough.\n\r", ch );
    return;
  }
  else if (!fIgnorePos && IS_GNBIT(sn, GN_NOCAST)){
    send_to_char("This spell cannot be cast this way.\n\r", ch);
    return;
  }
  /* mental knife prevents beneficial spells */
  else if (IS_GNBIT(sn, GN_BEN) && is_affected(ch, gsn_mental_knife) ){
    send_to_char("Your mind flares with pain and you abort your efforts.\n\r", ch);
    return;
  }
//calculate the new manacost factoring in the race/class bonuses.
  mana = _mana_cost(ch, sn);

  switch ( skill_table[sn].target ){
  default:
    bug( "Do_cast: bad target for sn %d.", sn );
    return;
  case TAR_IGNORE:
    if ( arg2[0] == '\0' && sn == 59)
	{
	  send_to_char("Locate what?\n\r",ch);
	  return;
	}
    if (IS_AFFECTED2(ch,AFF_SHADOWFORM) && sn == 43)
      {
	send_to_char("You cannot affect others in your ethereal state.\n\r",ch);
	return;
      }
    if (IS_AFFECTED2(ch,AFF_TREEFORM))
      {
	send_to_char("You cannot do this in your treeformed state.\n\r",ch);
	return;
      }
    break;
  case TAR_CHAR_OFFENSIVE:
    if ( arg2[0] == '\0' )
      {
	if ( ( victim = ch->fighting ) == NULL )
	  {
	    send_to_char( "Cast the spell on whom?\n\r", ch );
	    return;
	  }
      }
    else if ( ( victim = get_char_room( ch, NULL, target_name ) ) == NULL )
      {
	send_to_char( "They aren't here.\n\r", ch );
	return;
      }
    if (IS_AFFECTED2(ch,AFF_SHADOWFORM) && victim != ch)
      {
	send_to_char("You cannot affect others in your ethereal state.\n\r",ch);
	return;
      }
    if (IS_AFFECTED2(ch,AFF_TREEFORM) && victim != ch)
      {
	send_to_char("You cannot affect others in your treeformed state.\n\r",ch);
	return;
      }
    if (is_safe(ch,victim) && victim != ch)
      {
	sendf(ch, "Not on %s.\n\r",PERS(victim,ch));
	return;
      }
    if (is_sheathed(ch)){
      send_to_char("Your weapons are sheathed!\n\r", ch);
      return;
    }
    if ( !IS_NPC(ch) && victim == ch && sn != 76 && sn != 150 && ch->pcdata->learned[sn] > 0)// && !(IS_IMMORTAL (ch)))
      {
	send_to_char("Hurting yourself doesn't help.\n\r",ch);
	if (!is_affected(ch, gsn_nolearn)){
	  AFFECT_DATA af;
	  af.type = gsn_nolearn;
	  af.level = 60;
	  af.duration = 0;
	  af.where = TO_NONE;
	  af.bitvector = 0;
	  af.location = APPLY_NONE;
	  af.modifier = 0;
	  affect_to_char(victim, &af);
	  ch->mana /= 2;
	}
      }
    vo = (void *) victim;
    target = TARGET_CHAR;
    break;
  case TAR_CHAR_DEFENSIVE:
    if ( arg2[0] == '\0' )
      victim = ch;
    else if ( ( victim = get_char_room( ch, NULL, target_name ) ) == NULL )
        {
	  send_to_char( "They aren't here.\n\r", ch );
	  return;
	}
    if (IS_AFFECTED2(ch,AFF_SHADOWFORM) && victim != ch)
      {
	send_to_char("You cannot affect others in your ethereal state.\n\r",ch);
	return;
      }
    if (IS_AFFECTED2(ch,AFF_TREEFORM) && victim != ch)
      {
	send_to_char("You cannot affect others in your treeformed state.\n\r", ch);
	return;
      }
    vo = (void *) victim;
    if (skill_table[sn].spell_type == SPELL_PROTECTIVE)
      {
          /* Kaslai: I DID SOMETHING DANGEROUS - Original line:
                     if (!is_same_group && (victim->level > ch->level + 9 || victim->level < ch->level -9)) */
	if (!is_same_group(ch,victim) && (victim->level > ch->level + 9 || victim->level < ch->level -9))
	  {
	    send_to_char("You cannot cast this spell on that person.\n\r",ch);
	    return;
	    }
	if ((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_GOOD(victim) && IS_EVIL(ch)))
	  {
		send_to_char("Why would you help that person?\n\r",ch);
		return;
	  }
      }
    if (skill_table[sn].spell_type == SPELL_CURATIVE)
      {
	if (!IS_NPC(victim) && !is_same_group(ch,victim) && is_fight_delay(victim,40) && (victim->level > ch->level + 9 || victim->level < ch->level -9))
	  {
	    send_to_char("You cannot cast this spell on that person yet.\n\r",ch);
	    return;
	  }
	if ((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_GOOD(victim) && IS_EVIL(ch)))
	  {
	    send_to_char("Why would you help that person?\n\r",ch);
	    return;
	  }
      }
    if (skill_table[sn].spell_type == SPELL_RESTORATIVE)
      {
	if (!IS_NPC(victim) && !is_same_group(ch,victim) && is_fight_delay(victim,40) && (victim->level > ch->level + 9 || victim->level < ch->level -9))
	  {
	    send_to_char("You cannot cast this spell on that person.\n\r",ch);
	    return;
	  }
      }
    target = TARGET_CHAR;
    break;
  case TAR_CHAR_SELF:
    if ( arg2[0] != '\0' && !is_name( target_name, ch->name ) )
      {
	send_to_char( "You cannot cast this spell on another.\n\r", ch );
	return;
      }
    vo = (void *) ch;
    break;
  case TAR_OBJ_WORN:
    if ( arg2[0] == '\0' )
      {
	send_to_char( "What should the spell be cast upon?\n\r", ch );
	return;
      }

    if ( ( obj = get_obj_wear( ch, target_name, ch) ) == NULL )
      {
	send_to_char( "You are not wearing that.\n\r", ch );
	return;
      }

    vo = (void *) obj;
    target = TARGET_OBJ;
    break;

  case TAR_OBJ_INV:
    if ( arg2[0] == '\0' )
      {
	send_to_char( "What should the spell be cast upon?\n\r", ch );
	return;
      }
    if ( ( obj = get_obj_carry( ch, target_name, ch ) ) == NULL )
      {
	send_to_char( "That item is not on your person.\n\r", ch );
	return;
      }
    vo = (void *) obj;
    target = TARGET_OBJ;
    break;
  case TAR_OBJ_ROOM:
    if ( arg2[0] == '\0' )
      {
	send_to_char( "What should the spell be cast upon?\n\r", ch );
	    return;
      }
    if ( ( obj = get_obj_list( ch, arg2, ch->in_room->contents ) ) == NULL )
      {
	send_to_char( "You can't find it.\n\r", ch );
	return;
      }
    vo = (void *) obj;
    target = TARGET_OBJ;
    break;
  case TAR_DOOR:
    if ( arg2[0] == '\0' )
      {
	send_to_char( "What should the spell be cast upon?\n\r", ch );
	return;
      }
    door = find_door(ch,arg2);
    vo = (void *) (long long)door;
    target = TARGET_DOOR;
    break;
  case TAR_OBJ_CHAR_OFF:
    if (arg2[0] == '\0')
      {
	if ((victim = ch->fighting) == NULL)
	  {
	    send_to_char("Cast the spell on whom or what?\n\r",ch);
	    return;
	  }
	target = TARGET_CHAR;
      }
    else if ((victim = get_char_room(ch, NULL, target_name)) != NULL)
      target = TARGET_CHAR;

    if (IS_AFFECTED2(ch,AFF_SHADOWFORM) && victim != ch)
      {
	send_to_char("You cannot affect others in your ethereal state.\n\r",ch);
	return;
      }
    if (IS_AFFECTED2(ch,AFF_TREEFORM) && victim != ch)
      {
	send_to_char("You cannot affect others in your treeformed state.\n\r",ch);
	return;
      }
    if (target == TARGET_CHAR)
      {
	if(is_safe(ch,victim) && victim != ch)
	  {
	    sendf(ch, "Not on %s.\n\r",PERS(victim,ch));
	    return;
	  }
	vo = (void *) victim;
      }
    else if ((obj = get_obj_here(ch, NULL, target_name)) != NULL)
      {
	vo = (void *) obj;
	target = TARGET_OBJ;
      }
    else
      {
	send_to_char("You don't see that here.\n\r",ch);
	return;
      }
    break;
  case TAR_OBJ_CHAR_DEF:
    if (arg2[0] == '\0')
      {
	vo = (void *) ch;
	target = TARGET_CHAR;
      }
    else if ((victim = get_char_room(ch, NULL, target_name)) != NULL)
      {
	vo = (void *) victim;
	target = TARGET_CHAR;
      }
    else if ((obj = get_obj_carry(ch,target_name,ch)) != NULL)
      {
	vo = (void *) obj;
	target = TARGET_OBJ;
      }
    else
      {
	send_to_char("You don't see that here.\n\r",ch);
	return;
      }
    break;
  case TAR_ROOM_CHAR_DEF:
    if (arg2[0] == '\0')
      {
	vo = (void *) ch;
	target = TARGET_CHAR;
      }
    else if ((victim = get_char_room(ch, NULL, target_name)) != NULL)
      {
	vo = (void *) victim;
	target = TARGET_CHAR;
      }
    else if ( ( obj = get_obj_list( ch, target_name, ch->in_room->contents ) ) != NULL )
      {
	vo = (void *) obj;
	target = TARGET_OBJ;
      }
    else
      {
	send_to_char("You don't see that here.\n\r",ch);
	return;
      }
    break;
  }
  if (IS_DNDS(ch)
      && (ch->pCabal == NULL || get_cskill(ch->pCabal, sn) == NULL)){
    DNDS_DATA* dndsd;
    //cabal powers ignore this
    if ( (dndsd = dnds_lookup(ch->pcdata, sn, -1)) == NULL
	 || GET_DNDMEM(ch->pcdata, sn) < 1){
      send_to_char("You do not have that spell ready.\n\r", ch);
      return;
    }
  }
  if ( !IS_NPC(ch) && ch->mana < mana && !IS_IMMORTAL(ch))
    {
      send_to_char( "You don't have enough mana.\n\r", ch );
      return;
    }
  if ( str_cmp( skill_table[sn].name, "ventriloquate" ) && ch->invis_level < LEVEL_HERO)
    {
      if (iscommune)
	{
	  if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
	       || (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR))
	    act("$n narrows $s eyes.",ch,NULL,NULL,TO_ROOM);
	  else
	    act("$n closes $s eyes momentarily.",ch,NULL,NULL,TO_ROOM);
	}
      else if (is_affected(ch, gsn_subvocal))
	{
	  if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
	       || (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR))
	    act("Beads of perspiration form upon $n's forehead.",ch,NULL,NULL,TO_ROOM);
	  else
	    act("$n concentrates deeply.",ch,NULL,NULL,TO_ROOM);
	}
      else
	say_spell( ch, sn );
    }

//CABAL_NOMAGIC CHECK HERE, ALLOWS ONLY HARMFUL TYPES OF SPELLS
  if (target == TARGET_CHAR
      && (skill_table[sn].target != TAR_IGNORE
	  && skill_table[sn].target != TAR_CHAR_OFFENSIVE
	  && skill_table[sn].target != TAR_OBJ_CHAR_OFF)
      && ((CHAR_DATA*) vo)->pCabal
      && IS_CABAL( ((CHAR_DATA*) vo)->pCabal, CABAL_NOMAGIC)){
    act("$N would not appreciate that.", ch, NULL, (CHAR_DATA*) vo, TO_CHAR);
    return;
  }

//SPELL DATA
    spell_lvl =  spell_lvl_mod(ch, sn);
    spell_lag =  skill_table[sn].beats;

    if ((skill_table[sn].target == TAR_CHAR_OFFENSIVE
	 || (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR)) && victim != ch )
      {
	CHAR_DATA *vch, *vch_next;
	for ( vch = ch->in_room->people; vch; vch = vch_next )
	  {
	    vch_next = vch->next_in_room;
	    if (victim == NULL || ch == NULL || vch == NULL)
	      break;
	    if ( sn == 14 && IS_AFFECTED(vch,AFF_CHARM) && (is_same_group(ch,vch) || vch->master == ch || ch->master == ch))
		break;
            if (victim == vch && ch->fighting != vch && vch->fighting != ch && !IS_NPC(ch))
	      fyell=TRUE;
	  }
      }
    concen = skill;
    if (concen > 0){
      concen += get_curr_stat(ch,STAT_LUCK) - 16;
      if (is_affected(ch,gsn_prayer))
	concen += UMIN(5, 3 * (100 - concen) / 10);
      if (is_song_affected(ch,gsn_concen)){
	concen += concen/3;
	song_strip(ch,gsn_concen);
      }
    }
    if ( (paf = affect_find(ch->affected, gsn_mind_disrupt)) != NULL)
      concen += paf->modifier;
//PRESPELL_CHECK

    if (PRESPELL_CHECK(ch, argument, &sn, &mana, &spell_lag, &spell_lvl) == -1)
      bug("Error returned in PRESPELL\n\r", 0);


    if (!IS_IMMORTAL(ch))
      WAIT_STATE2( ch, spell_lag);
    /* cabal cost */
    if (ch->pCabal)
      handle_skill_cost( ch, ch->pCabal, sn );

    if ( (IS_NPC(ch) && ch->desc == NULL && !ch->pIndexData->progtypes)
	 || number_percent() > concen
	 || check_spellblast(ch, victim, sn, mana, ch->level)
	 ){
      /* Learn from spell casting */
      gain_exp(ch, 3 * spell_expgain(sn, get_skill(ch, sn), ch->level));
      if (IS_IMMORTAL(ch)){
	sendf(ch, "Spell exp gain: %d\n\r", spell_expgain(sn, skill, ch->level));
      }
      send_to_char( "You lost your concentration.\n\r", ch );
      check_improve(ch,sn,FALSE, 2 * (5 + mana / 10));
      if (!IS_IMMORTAL(ch))
	{
	  ch->mana -= mana / 2;
	}
    }
    else
      {
	if (fyell && IS_AWAKE(victim))
	  {
	    m_yell(ch,victim,TRUE);
	    syell = FALSE;
	}
	if (!IS_IMMORTAL(ch)){
	  AFFECT_DATA* paf;
	  ch->mana -= mana;
	  if ( (paf = affect_find(ch->affected, gsn_ocularis)) != NULL
	       && paf->has_string){
	    CHAR_DATA* och = get_char( paf->string );
	    int m_extra = UMAX(1, mana / 5);
	    int mv_extra = number_range(2, 7);
	    if (och && och != ch){
	      sendf(och, "The Eye of Magic channels %d mana and %d stamina to you.\n\r", m_extra, mv_extra );
	      och->mana = UMIN(och->max_mana, och->mana + m_extra );
	      och->move = UMIN(och->max_move, och->move + mv_extra );
	      send_to_char("It seems to take great effort to form the spell.\n\r", ch);
	      ch->move = UMAX(0, ch->move - mv_extra);
	    }
	  }
	}

	/* use up the DNDS spell */
	if (IS_DNDS(ch))
	  useup_dnds( ch->pcdata, sn );

	/* spell and mana trap */
	if (skill_table[sn].target == TAR_CHAR_OFFENSIVE
	    && victim
	    && victim != ch
	    && !IS_NPC(ch)
	    && !IS_NPC(victim)){
	  if (is_affected(victim, gsn_mana_trap)){
	    int mana_gain = number_range(mana / 2, mana);
//	    sendf(victim, "You gain %d mana.\n\r", mana_Gain );
	    victim->mana = UMIN(victim->mana + mana_gain, 110 * victim->max_mana / 100 );
	  }
	  if (is_affected(victim, gsn_spell_trap )){
	    int level = sklevel(ch, sn );
	    DNDS_HEADER* dndsh = &victim->pcdata->dndspells[UMIN(4, level / 10)];
	    memorize_spells(dndsh, 1 );
	  }
	}

/* Learn from spell casting */
	gain_exp(ch, spell_expgain(sn, skill, ch->level));
	if (IS_IMMORTAL(ch)){
	  sendf(ch, "Spell exp gain: %d\n\r", spell_expgain(sn, skill, ch->level));
	}

	if ((skill_table[sn].target == TAR_CHAR_OFFENSIVE
	     || skill_table[sn].target == TAR_CHAR_DEFENSIVE)
	    && ( (paf = affect_find(victim->affected,gsn_mimic))  != NULL)
	    && sn != gsn_breach
	    && ch != victim && victim->position > POS_SITTING
	    && number_percent() < (get_skill(victim, gsn_mimic) - 15))
	{
	  CHAR_DATA* buf;
	  act("Your reflective shield flares and fades as $N's spell is reflected back at $M.",victim,NULL,ch,TO_CHAR);
	  act("$n's shield of spell turning reflects $N's spell back at $M.",victim,NULL,ch,TO_NOTVICT);
	  act("$n's shield of spell turning reflects your spell back at you",victim,NULL,ch,TO_VICT);
	  if (--paf->modifier < 1){
	    affect_strip(victim,gsn_mimic);
	    act("Your shield of spell turning collapses.", victim, NULL, NULL, TO_CHAR);
	    act( "The shimmering, reflecting shield around $n fades away.", victim, NULL, NULL, TO_ROOM);
	  }

	  //we flip targets around.
	  buf = (CHAR_DATA*) vo;
	  vo = (void *) ch;
	  ch = buf;
	  spell_lvl = 2 * spell_lvl / 3;
	}

	//check for blademaster snakespeed
	if (skill_table[sn].target == TAR_CHAR_OFFENSIVE && snakespeedCheck(ch, victim))
	  return;

	//check for spell killer
	if (ch->fighting && ch->fighting->fighting == ch && spellkillerCheck(ch, ch->fighting, sn)){
	  return;
	}

///////////////////////
//CAST THE SPELL HERE//
//////////////////////

	(*skill_table[sn].spell_fun) ( sn, spell_lvl, ch, vo,target);
	check_improve(ch,sn,TRUE, 3 + mana / 10);

//POSTSPELL_CHECK
if (POSTSPELL_CHECK(ch, argument, sn, mana, spell_lag) == -1)
  bug("Error returned in POSTSPELL\n\r", 0);
      }
    check_improve(ch,gsn_ancient_lore,TRUE,1);
    if ((skill_table[sn].target == TAR_CHAR_OFFENSIVE
    || (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR)) && victim != ch )
    {
	CHAR_DATA *vch, *vch_next;
	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next = vch->next_in_room;
	    if (victim == NULL || ch == NULL || vch == NULL)
		break;
	    if ( sn == 14 && IS_AFFECTED(vch,AFF_CHARM) &&(is_same_group(ch,vch) || vch->master == ch))
		break;

	    if ( victim == vch && victim->fighting == NULL )
            {
		if (syell)
		    m_yell(ch,victim,FALSE);
		if (IS_AWAKE(victim)
		    && !is_affected(victim,gsn_ecstacy)
		    && !is_affected(victim,gsn_mortally_wounded)
		    && IS_AWAKE(ch)
		    && !is_affected(ch,gsn_ecstacy)
		    && !is_affected(ch,gsn_mortally_wounded)
		    && !is_ghost(victim,600))
		{
		    set_fighting( ch, victim );
		    set_fighting( victim, ch );
		    multi_hit( victim, ch, TYPE_UNDEFINED );
		}
		else
		  set_delay(ch, victim);
		break;
	    }

	    if (victim == vch && ch->fighting == NULL )
	    {
		if (syell)
		    m_yell(ch,victim,FALSE);
		if (IS_AWAKE(victim)
		    && !is_affected(victim,gsn_mortally_wounded)
		    && !is_affected(victim,gsn_ecstacy)
		    && IS_AWAKE(ch)
		    && !is_affected(ch, gsn_mortally_wounded)
		    && !is_affected(ch,gsn_ecstacy)
		    && !is_ghost(victim,600))
		{
		    set_fighting( ch, victim );
		    set_fighting( victim, ch );
		    multi_hit( victim, ch, TYPE_UNDEFINED );
		}
		else
		  set_delay(victim, ch);
		break;
	    }
	}
    }
}

void do_cast( CHAR_DATA *ch, char *argument )
{
    if (get_trust(ch) < LEVEL_IMMORTAL
	&& (ch->class == class_lookup("healer") ||
	    ch->class == class_lookup("shaman") ||
	    ch->class == class_lookup("cleric") ||
	    ch->class == class_lookup("paladin")||
	    ch->class == class_lookup("druid")))
      {
	send_to_char("You need to commune to your god.\n\r",ch);
	return;
      }
    if ( !IS_IMMORTAL(ch) && (ch->class == class_lookup("necromancer")
	  || ch->class == class_lookup("dark-knight"))
	&& !IS_EVIL(ch) )
    {
	send_to_char("You've lost your powers for straying from your alignment.\n\r",ch);
	return;
    }
    if (IS_AFFECTED2(ch, AFF_SILENCE)
	&& !is_affected(ch, gsn_subvocal))
    {
	send_to_char("As you utter your incantation, no sound comes out!\n\r",ch);
	return;
    }
    if (is_affected(ch, gsn_gag)
	&& !is_affected(ch, gsn_subvocal)){
      send_to_char("You try to work your mouth but the gag holds fast.\n\r", ch);
      return;
    }
    if (is_affected(ch,gsn_uppercut))
    {
	send_to_char("Your jaws hurts too much to move.\n\r",ch);
	return;
    }
    if (!IS_IMMORTAL(ch) && IS_SET(ch->in_room->room_flags, ROOM_DARK_RIFT))
    {
	send_to_char("All your ties to reality have been severed here, your magical energy is drained.\n\r",ch);
	return;
    }
    cast_new(ch,argument,FALSE, FALSE);
}

void do_commune( CHAR_DATA *ch, char *argument )
{
    if (ch->class != class_lookup("healer") &&
	ch->class != class_lookup("shaman") &&
	ch->class != class_lookup("cleric") &&
	ch->class != class_lookup("druid")  &&
	ch->class != class_lookup("paladin") && get_trust(ch) < LEVEL_IMMORTAL)
    {
    	send_to_char("You don't know how to commune.\n\r",ch);
	return;
    }
    if ( !IS_IMMORTAL(ch) && ((( ch->class == class_lookup("healer")
	    || ch->class == class_lookup("paladin"))
	  && !IS_GOOD(ch) )
	|| (ch->class == class_lookup("shaman")
	  && !IS_EVIL(ch))) )
    {
	send_to_char("You've lost your powers for straying from your alignment.\n\r",ch);
	return;
    }
    if (!IS_IMMORTAL(ch) && IS_SET(ch->in_room->room_flags, ROOM_DARK_RIFT))
    {
	send_to_char("All your ties to reality have been severed here, your faith falters.\n\r",ch);
	return;
    }
    if (!IS_IMMORTAL(ch) && IS_SET(ch->in_room->room_flags2,ROOM_NO_MAGIC))
    {
	send_to_char("Your prayers have been blocked off to your gods.\n\r",ch);
	return;
    }
    if (!IS_NPC(ch) && deity_table[ch->pcdata->way].shrine == 0){
      send_to_char("Your lack of faith prevents you from communing.\n\r",ch);
      if (ch->pcdata->way == 0)
	send_to_char("Consult scrolls of \"help religion\".\n\r", ch);
      return;
    }
    if (is_affected(ch,gsn_blasphemy) )
    {
	send_to_char("Your lack of faith prevents you from communing.\n\r",ch);
	return;
      }
    if (is_affected(ch, gsn_corrupt))
      {
	send_to_char("Your prayers fail as the reality seems to weave and bend around you.\n\r", ch);
	return;
      }
    if ( argument[0] == '\0' )
    {
	send_to_char( "Commune which what where?\n\r", ch );
	return;
    }
    cast_new(ch,argument,TRUE, FALSE);
}

void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj )
{
    void *vo = NULL;
    int target = TARGET_NONE;
    if ( sn <= 0 )
	return;
    level = UMAX(1,level);
    if ( sn >= MAX_SKILL || skill_table[sn].spell_fun == 0 )
    {
	bug( "Obj_cast_spell: bad sn %d.", sn );
	return;
    }
    switch ( skill_table[sn].target )
    {
    default:
	bug( "Obj_cast_spell: bad target for sn %d.", sn );
	return;
    case TAR_IGNORE:
	vo = NULL;
	break;
    case TAR_CHAR_OFFENSIVE:
	if ( victim == NULL )
	    victim = ch->fighting;
	if ( victim == NULL )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	if (is_safe(ch,victim) && ch != victim)
	    return;
	vo = (void *) victim;
	target = TARGET_CHAR;
	break;
    case TAR_CHAR_DEFENSIVE:
    case TAR_CHAR_SELF:
	if ( victim == NULL )
	    victim = ch;
	vo = (void *) victim;
	target = TARGET_CHAR;
	break;
    case TAR_OBJ_INV:
    case TAR_OBJ_ROOM:
	if ( obj == NULL )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	vo = (void *) obj;
	target = TARGET_OBJ;
	break;
    case TAR_OBJ_CHAR_OFF:
        if ( victim == NULL && obj == NULL)
	{
	    if (ch->fighting != NULL)
		victim = ch->fighting;
	    else
	    {
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }
	    if (victim != NULL)
	    {
		if (is_safe(ch,victim) && ch != victim)
		{
		    send_to_char("Somehting isn't right...\n\r",ch);
		    return;
		}
		vo = (void *) victim;
		target = TARGET_CHAR;
	    }
	    else
	    {
	    	vo = (void *) obj;
	    	target = TARGET_OBJ;
	    }
	}
        break;
    case TAR_OBJ_CHAR_DEF:
	if (victim == NULL && obj == NULL)
	{
	    vo = (void *) ch;
	    target = TARGET_CHAR;
	}
	else if (victim != NULL)
	{
	    vo = (void *) victim;
	    target = TARGET_CHAR;
	}
	else
	{
	    vo = (void *) obj;
	    target = TARGET_OBJ;
	}
	break;
    }

//Remmed out to allow passing of targets    target_name = "";
    (*skill_table[sn].spell_fun) ( sn, level, ch, vo,target);
    if ( (skill_table[sn].target == TAR_CHAR_OFFENSIVE
    || (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR)) && victim != ch && victim->master != ch )
    {
	CHAR_DATA *vch, *vch_next;
	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next = vch->next_in_room;
	    if ( victim == vch && victim->fighting == NULL )
	    {
		m_yell(ch,victim,TRUE);
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	}
    }
}

void spell_acid_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = 2 * level + dice( 40, 3 );
    if ( saves_spell( level, victim, DAM_ACID,skill_table[sn].spell_type ) )
	dam /= 2;
    else
      acid_effect(victim, level, dam, TARGET_CHAR);
    damage( ch, victim, dam, sn,DAM_ACID,TRUE);
}

void spell_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_EVIL(ch) && IS_GOOD(victim))) && level < LEVEL_IMMORTAL)
    {
	sendf(ch, "It would go against your beliefs to aid %s.\n\r", PERS(victim,ch));
	return;
    }
    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
            act("You are already armored.",ch,NULL,victim,TO_CHAR);
        else
            act("$N is already armored.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where	 = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 24 * UMAX(1, level / 25);
    af.modifier  = -20;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You feel someone protecting you.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
	act("$N is protected by your magic.",ch,NULL,victim,TO_CHAR);
}

void spell_bless( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int loc1 = APPLY_NONE;
    int loc2 =  APPLY_NONE;
    int loc3 = APPLY_NONE;
    int mod1 = 0;
    int  mod2 = 0;
    int  mod3 = 0;

    int g_type = 0;

    if (!IS_NPC(ch)
	&& ( (IS_GOOD(ch) && IS_EVIL(victim))
	     || (IS_GOOD(victim) && IS_EVIL(ch)) ) )
    {
      act("$g doesn't seem to like $N.", ch, NULL, victim, TO_CHAR);
      return;
    }
    if ( is_affected( victim, gsn_blasphemy ) )
    {
	sendf(ch, "%s's faith have been lost.\n\r", PERS(victim,ch));
	return;
    }
    if ( is_affected( victim, sn ) || is_affected(victim, gsn_warcry))
    {
	if (victim == ch)
	  act( "You are already blessed.",ch,NULL,victim,TO_CHAR);
	else
	  act("$N already has divine favor.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 6+level;
    af.bitvector = 0;

    if(IS_NPC(ch))
      {
	af.location  = APPLY_HITROLL;
	af.modifier  = level / 12;
	affect_to_char( victim, &af );
	af.location  = APPLY_DAMROLL;
	af.modifier  = level / 12;
	affect_to_char( victim, &af );
	af.location  = APPLY_SAVING_SPELL;
	af.modifier  = 0 - level / 12;
	affect_to_char( victim, &af );
	act( "You feel righteous.", ch,NULL,victim,TO_CHARVICT);
	if ( ch != victim )
	  act("You grant $N $g's favor.",ch,NULL,victim,TO_CHAR);
	return;
      }

    g_type = deity_table[IS_NPC(ch) ? 0 : ch->pcdata->way].path;

    act( "You feel $g's strength in your soul.", ch,NULL,victim,TO_CHARVICT);
    if ( ch != victim )
      act("You grant $N $g's favor.",ch,NULL,victim,TO_CHAR);

    /* LIFE */
    if (g_type == PATH_LIFE){
      loc1 = APPLY_HIT;
      mod1 =  level/2;

      loc2 = APPLY_HIT_GAIN;
      mod2 = level / 5;

      loc3 = APPLY_HITROLL;
      mod3 = level / 8;
    }
    /* DEATH */
    else if (g_type == PATH_DEATH){
      loc1 = APPLY_SAVING_MALED;
      mod1 =  -level/4;
    }
    /* CHANCE */
    else if (g_type == PATH_CHANCE){
      /* first modifier may be hitroll or damroll or saves */
      switch( number_range(0, 7)){
      case 0:
      case 1:
	loc1 = APPLY_HITROLL;
	mod1 =  level/8;
	break;
      case 2:
      case 3:
	loc1 = APPLY_DAMROLL;
	mod1 =  level/8;
	break;
      case 4:
      case 5:
	loc1 = APPLY_AC;
	mod1 =  -level;
	break;
      case 6:
      case 7:
	loc1 = APPLY_SAVING_SPELL;
	mod1 =  -level/12;
	break;
      }
      loc2 = APPLY_LUCK;
      mod2 = level / 12;
    }
    /* KNOWLEDGE */
    else if (g_type == PATH_KNOW){
      /* select between warriro types and mage types */
      if (class_table[victim->class].fMana){
	loc1 = APPLY_MANA_GAIN;
	mod1 = level / 5;

	loc2 = APPLY_SAVING_SPELL;
	mod2 = -level / 8;
      }
      else{
	OBJ_DATA* obj = get_eq_char( victim, WEAR_WIELD );
	af.where = TO_SKILL;
	af.location = (obj == NULL || obj->value[0] == WEAPON_EXOTIC ?
		       gsn_hand_to_hand :
		       *weapon_table[weapon_lookup(weapon_name(obj->value[0]))].gsn);
	if (ch != victim)
	  af.modifier = 1;
	else
	  af.modifier = level / 16;
	affect_to_char(victim, &af);
	af.where = TO_AFFECTS;
	loc1 = APPLY_NONE;
	mod1 =  0;

	loc2 = APPLY_DAMROLL;
	mod2 = level / 6;
      }
    }
    /* OTHER */
    else{
      loc1 = APPLY_SAVING_SPELL;
      mod1 = -level / 12;
      loc2 = APPLY_DAMROLL;
      mod2 = level / 12;
      loc3 = APPLY_HITROLL;
      mod3 = level / 12;
    }
    //now we apply them.
    if (loc1 != APPLY_NONE)
      {
	af.location = loc1;
	af.modifier = mod1;
	affect_to_char(victim, &af);
      }
    if (loc2 != APPLY_NONE)
      {
	af.location = loc2;
	af.modifier = mod2;
	affect_to_char(victim, &af);
      }
    if (loc3 != APPLY_NONE)
      {
	af.location = loc3;
	af.modifier = mod3;
	affect_to_char(victim, &af);
      }
}

void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_BLIND))
    {
        act("$N is already blinded.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( saves_spell(level,victim,DAM_MALED,skill_table[sn].spell_type))
    {
        act("You fail to blind $N.",ch,NULL,victim,TO_CHAR);
	return;
    }

    /* Palison tattoo check */
    if (ch->tattoo == deity_lookup("compassion")
	&& number_percent() < 75){
      OBJ_DATA* tattoo = get_eq_char( ch, WEAR_TATTOO);
      if (tattoo)
	act("$p flares with a brilant aura.", ch, tattoo, victim, TO_ALL);
      return;
    }


    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.location  = APPLY_HITROLL;
    af.duration  = level/10 + 4;
    af.bitvector = AFF_BLIND;
    if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
      if (is_affected(victim, gsn_battletrance))
	af.modifier      = 0;
      else
	af.modifier      = -5;
    }
    else
        af.modifier      = -10;
    affect_to_char(victim,&af);
    af.location     = APPLY_AC;
    if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
      if (is_affected(victim, gsn_battletrance))
	af.modifier      = 0;
      else
        af.modifier      = +15;
    }
    else
        af.modifier      = +25;
    affect_to_char(victim,&af);
    act("You are blinded!", victim,NULL,NULL,TO_CHAR);
    act("$n appears to be blinded.",victim,NULL,NULL,TO_ROOM);
}

void spell_burning_hands(int sn,int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] =
    {
	 0,
	 2,  2,  5,  8,  12,	17, 20, 23, 26, 29,
	29, 29, 30, 30,	31,	31, 32, 32, 33, 33,
	34, 34, 35, 35,	36,	36, 37, 37, 38, 38,
	39, 39, 40, 40,	41,	41, 42, 42, 43, 43,
	44, 44, 45, 45,	46,	46, 47, 47, 48, 48
    };
    int dam;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim,DAM_FIRE,skill_table[sn].spell_type) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_FIRE,TRUE);
}

void spell_call_lightning( int sn, int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *vch, *vch_next;
    static const sh_int dam_each[] =
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,  30,	 35,  40,  45,  50,  55,
	 60,  70,  80,  85,  90,	 92,  94,  96,  98, 100,
	100, 101, 102, 103, 104,	105, 106, 107, 108, 109,
	110, 112, 114, 115, 115,	115, 115, 115, 120, 120
    };
    int dam;
    int armor = 0;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( 3 * dam_each[level]/4, 3 * dam_each[level] / 2 );

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You must be out of doors.\n\r", ch );
	return;
    }
    if ( ( mud_data.weather_info.sky < SKY_RAINING
	   || temp_adjust( ch->in_room->temp ) <= 35)
	 && !IS_IMMORTAL(ch))
    {
	send_to_char( "You need bad weather.\n\r", ch );
	return;
    }

    act( "Deep thunder rolls and sheets of lightning strike your foes!", ch, NULL, NULL, TO_CHAR );
    act( "$n shouts an invocation and sheets of lightning strike from above!",ch, NULL, NULL, TO_ROOM );
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room && vch != ch && !is_area_safe(ch,vch))
	{
	    m_yell(ch,vch,FALSE);
	    if ( saves_spell( level, vch, DAM_LIGHTNING,skill_table[sn].spell_type) )
	      dam /= 2;
//We give bonus to the spell for the ac person has. (ac is n've so we subtr.)
//bonus is 1/2 value over 150, and regular over 225.
	    armor = GET_AC(vch, AC_PIERCE) + GET_AC(vch, AC_BASH) + GET_AC(vch, AC_SLASH);
	    armor /= -3;
	    if (armor > 225){
	      act("Sparks arc around $n's armor!", vch, NULL, NULL, TO_ROOM);
	      act("Sparks arc around your armor!", vch, NULL, NULL, TO_CHAR);
	    }
	    armor = UMIN(300, armor);
	    dam += UMAX(0 ,(armor-150)/2);
	    dam += UMAX(0, (armor - 225));
	    dam = UMIN(300, dam);

	    //And we apply damage.
	    damage( ch, vch, dam, sn,DAM_LIGHTNING,TRUE);
	    //And a little bonus to those with huger armor.
	    if (armor >= 300)
	      {
		act("The hair on your neck rises as air becomes charged around you.", ch, NULL, vch, TO_VICT);
		act("$N's armor glows with ghostly blue hue and the air crackles with charge.", vch, NULL, vch, TO_ROOM);
		damage( ch, vch, dam/3, sn,DAM_LIGHTNING,TRUE);
	      }
	}
    }
}

void spell_calm( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  CHAR_DATA *vch;
  AFFECT_DATA af;
  int gsn_frenzy = skill_lookup("frenzy");

  send_to_char("You send a powerful aura of peace and calm radiating through the area.\n\r", ch);
  act("$n sends a powerful aura of peace and calm radiating through the area.\n\r", ch, NULL, NULL, TO_ROOM);
  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room){
    bool fSave = FALSE;

    if (IS_AFFECTED(vch,AFF_CALM))
      continue;
    else if (is_same_group(ch, vch))
      continue;
    else if (is_area_safe_quiet(ch,vch))
      continue;

    if (IS_DEMON(vch) || (IS_NPC(vch) && IS_SET(vch->act, ACT_TOO_BIG))){
      act("$N ignores your efforts at calming $M.", ch, NULL, vch, TO_CHAR);
      act("You ignore $n's efforts to calm you.", ch, NULL, vch, TO_VICT);
      fSave = TRUE;
    }
    else if (saves_spell( IS_UNDEAD(vch) ? level + 3 : level, vch, DAM_MENTAL, skill_table[sn].spell_type) ){
      fSave = TRUE;
    }
    m_yell( ch, vch, FALSE );
    if (fSave){
      if( IS_AWAKE(vch)
	  && vch->fighting != ch
	  && ch->fighting != vch
	  && !is_affected(vch,gsn_ecstacy)
	  && !is_ghost(vch,600))
	{
	  if (ch->fighting == NULL || vch->fighting == NULL)
	    multi_hit( vch, ch, TYPE_UNDEFINED );
	  set_fighting( vch, ch );
	}
      continue;
    }

    act("$n has been calmed.", vch, NULL, NULL, TO_ROOM);
    send_to_char("You feel unnatural calm settle on your mind.\n\r",vch);

/* mobs the group is fighting may be calmed */


    if (vch->fighting && is_same_group(ch, vch->fighting))
      stop_fighting(vch, TRUE);

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level/6;
    af.location = APPLY_HITROLL;
    af.modifier = -level / 10;
    af.bitvector = AFF_CALM;
    affect_to_char(vch,&af);
    af.location = APPLY_DAMROLL;
    affect_to_char(vch,&af);

    if (IS_AFFECTED(vch,AFF_BERSERK)){
      affect_strip(vch, gsn_berserk);
      REMOVE_BIT(vch->affected_by, AFF_BERSERK);
      act(skill_table[gsn_berserk].msg_off, vch, NULL, NULL, TO_CHAR);
    }
    if (is_affected(vch, gsn_frenzy )){
      affect_strip(vch, gsn_frenzy );
      act(skill_table[gsn_frenzy].msg_off, vch, NULL, NULL, TO_CHAR);
    }

    if (IS_AFFECTED2(vch,AFF_RAGE)){
      REMOVE_BIT(vch->affected2_by,AFF_RAGE);
      affect_strip(vch,gsn_rage);
      act("You snap out of your madness.",vch,NULL,NULL,TO_CHAR);
      act("$n gasps for air as $e snaps out of $s insanity.",vch,NULL,NULL,TO_ROOM);

      vch->hit -= (ch->level * ch->perm_stat[STAT_CON]) /3;

      if (IS_IMMORTAL(vch))
	vch->hit = UMAX(1,vch->hit);

      if (vch->hit < -2){
	int shock_mod = (25 * (0 - vch->hit)) / vch->max_hit;
	if (number_percent( ) < con_app[UMAX(1,vch->perm_stat[STAT_CON] - shock_mod)].shock + 2*(get_curr_stat(vch,STAT_LUCK) - 16)){
	  act_new("Your body suddenly awakens to the wounds you've sustained and you lose consciousness.",vch,NULL,NULL,TO_CHAR,POS_DEAD);
	  act("$n suddenly clutches $s wounds and slumps to the ground, unconscious.",vch,NULL,NULL,TO_ROOM);
	  vch->hit = -2;
	  vch->position = POS_STUNNED;
	  act( "$n is stunned, but will probably recover.",vch, NULL, NULL, TO_ROOM );
	  act_new("You are stunned, but will probably recover.",vch, NULL, NULL, TO_CHAR, POS_DEAD );
	}
	else{
	  act_new("Your body could not sustain the injuries you've suffered.",vch,NULL,NULL,TO_CHAR,POS_DEAD);
	  act("$n suddenly clutches $s wounds and slumps to the ground.",vch,NULL,NULL,TO_ROOM);
	  act( "$n is DEAD!!",vch, 0, 0, TO_ROOM );
	  act_new( "You have been `1KILLED``!!", vch,NULL,NULL,TO_CHAR,POS_DEAD);
	  raw_kill(vch, vch );
	  continue;
	}
      }//end rage death
    }//end if rage
  }//end for
}

void spell_cause_light( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    damage( ch, (CHAR_DATA *) vo, dice(1, 8) + level / 3, sn,DAM_HARM,TRUE);
}

void spell_cause_critical(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    damage( ch, (CHAR_DATA *) vo, dice(3, 8) + level - 6, sn,DAM_HARM,TRUE);
}

void spell_cause_serious(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    damage( ch, (CHAR_DATA *) vo, dice(2, 8) + level / 2, sn,DAM_HARM,TRUE);
}

void spell_chain_lightning(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA* paf;
    int sn_ms = skill_lookup("mana shield");

    if ( (paf = affect_find(victim->affected, sn_ms)) == NULL){
      act("$N doesn't have mana shield active.", ch, NULL, victim, TO_CHAR);
      return;
    }

    act("A thousand small lightning bolts being to between you and $N!",
	ch, NULL, victim, TO_CHAR);
    act("A thousand small lightning bolts being to between $n and $N!",
	ch, NULL, victim, TO_NOTVICT);
    act("A thousand small lightning bolts being to between $n and you!",
	ch, NULL, victim, TO_VICT);

    if (!check_dispel(level, victim, paf->type)){
      paf->level -= 3;
      act("You manage to weaken $N's $t.", ch, skill_table[paf->type].name, victim, TO_CHAR);
    }
    damage( ch, victim, 5, sn, DAM_INTERNAL, TRUE );

}//end if not is gen


void spell_change_sex( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int sex = 0;
    if (number_percent() < 15)
	sex = 0;
    else if (victim->sex == 1)
	sex = 2;
    else if (victim->sex == 2)
	sex = 1;
    if ( is_affected( victim, sn ))
    {
	if (victim == ch)
            act("You've already been changed.",ch,NULL,victim,TO_CHAR);
	else
            act("$N has already had $s sex changed.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (saves_spell(level , victim,DAM_OTHER,skill_table[sn].spell_type))
	return;
    act("$n doesn't look like $mself anymore...",victim,NULL,NULL,TO_ROOM);
    act_new("You feel different.", victim,NULL,NULL,TO_CHAR,POS_DEAD );
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 2 * level;
    af.location  = APPLY_SEX;
    af.modifier  = sex;
    af.bitvector = 0;
    affect_to_char( victim, &af );
}

void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int charmies = ch->pet ? 2 : 1;
    if (is_affected(ch, sn)){
      send_to_char("Lacking power to charm another creature, you fail.\n\r", ch);
      return;
    }
    if (!can_follow(ch, ch))
      return;
    if (is_safe(ch,victim))
	return;
    if (get_charmed_by(ch) >= charmies && !IS_IMMORTAL(ch)){
	send_to_char("You are already controlling as many as you can.\n\r",ch);
	return;
    }
    if (!IS_NPC(victim) && !IS_IMMORTAL(ch))
	return;
    if (is_affected(victim,gsn_ecstacy)){
      act("$N's mind must be open to suggestion.",ch,NULL,victim,TO_CHAR);
      return;
    }
    if ( victim == ch ){
      send_to_char( "You like yourself even better!\n\r", ch );
      return;
    }
/* awake the person if they were sleeping */
    if (!IS_AWAKE( victim )){
      REMOVE_BIT(victim->affected_by, AFF_SLEEP );
      do_wake( victim, "");
    }

    if ( IS_AFFECTED(victim, AFF_CHARM) || IS_AFFECTED(ch, AFF_CHARM) || level < victim->level || victim->fighting != NULL
	 || (IS_NPC(victim) && IS_SET(victim->act,ACT_AGGRESSIVE)) || IS_SET(victim->imm_flags,IMM_CHARM)
	 || IS_UNDEAD(victim) || is_affected(victim, gsn_hypnosis)){
      send_to_char("You failed.\n\r", ch);
      return;
    }
    if ( (check_immune(victim, DAM_CHARM, TRUE) < IS_NORMAL
	  || victim->level > ch->level)
	 && saves_spell( level, victim,DAM_CHARM,skill_table[sn].spell_type) ){
      send_to_char("You failed.\n\r", ch);
      return;
    }



    if (is_affected(victim, gsn_horse) && monk_good(victim, WEAR_HEAD) && !IS_NPC(victim) && !IS_IMMORTAL(ch))
    {
        ch->leader = victim;
        af.where     = TO_AFFECTS;
        af.type      = sn;
        af.level     = level;
        af.duration  = (IS_NPC(ch) ? number_fuzzy( level / 4 ) : number_fuzzy( level / 8 ) );
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = AFF_CHARM;
        affect_to_char( ch, &af );
        act( "You reverse $n's attempt to charm you, and charm $M instead!", ch, NULL, victim, TO_VICT );
        act("$N reverse your charm, and charms you instead!", ch, NULL, victim, TO_CHAR );
        if ( ch->master )
	    stop_follower( ch );
        add_follower( ch, victim );
        act("$N looks at you with adoring eyes.",victim,NULL,ch,TO_CHAR);
        act("$N looks around with adoring eyes.",victim,NULL,ch,TO_NOTVICT);
	return;
    }
    if ( victim->master )
      stop_follower( victim );
    add_follower( victim, ch );
    victim->leader = ch;
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = (IS_NPC(victim) ? number_fuzzy( level / 4 ) : number_fuzzy( level / 8 ) );
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
    act( "Isn't $n just so nice?", ch, NULL, victim, TO_VICT );
    act("$N looks at you with adoring eyes.",ch,NULL,victim,TO_CHAR);
    act("$N looks around with adoring eyes.",ch,NULL,victim,TO_NOTVICT);
    if (IS_NPC(victim)){
      victim->alignment = ch->alignment;
      victim->hunting = ch;
    }
/* Set a wait affect on caster */
    af.bitvector = 0;
    affect_to_char(ch, &af);
}

void spell_chill_touch( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] =
    {
	 0,
	 0,  0,  6,  7,  8,	 9, 12, 13, 13, 13,
	14, 14, 14, 15, 15,	15, 16, 16, 16, 17,
	17, 17, 18, 18, 18,	19, 19, 19, 20, 20,
	20, 21, 21, 21, 22,	22, 22, 23, 23, 23,
	24, 24, 24, 25, 25,	25, 26, 26, 26, 27
    };
    AFFECT_DATA af;
    int dam;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( !saves_spell( level, victim,DAM_COLD,skill_table[sn].spell_type ) )
    {
	act("$n turns blue and shivers.",victim,NULL,NULL,TO_ROOM);
	af.where     = TO_AFFECTS;
	af.type      = sn;
        af.level     = level;
	af.duration  = 6;
	af.location  = APPLY_STR;
	af.modifier  = -2;
	af.bitvector = 0;
	if (!is_affected(ch, af.type))
	  affect_to_char( victim, &af );
    }
    else
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_COLD,TRUE );
}

void spell_colour_spray( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] =
    {
	 0,
	 0,  0,  0,  0,  0,	 0,  0,  0, 30, 41,
	42, 43, 44, 44, 45,	45, 46, 46, 47, 48,
	49, 50, 51, 51, 52,	52, 53, 53, 54, 55,
	56, 57, 58, 58, 59,	59, 60, 60, 61, 62,
	63, 64, 65, 65, 66,	66, 67, 67, 68, 69
    };
    int dam;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level] / 2,  dam_each[level] * 2 );
    if ( saves_spell( level, victim,DAM_LIGHT,skill_table[sn].spell_type) )
	dam /= 2;
    else
        spell_blindness(skill_lookup("blindness"),level/2,ch,(void *) victim,TARGET_CHAR);
    damage( ch, victim, dam, sn, DAM_LIGHT,TRUE );
}

void spell_continual_light(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    OBJ_DATA *light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), 0);
    obj_to_room( light, ch->in_room );
    act( "$n twiddles $s thumbs and $p appears.",   ch, light, NULL, TO_ROOM );
    act( "You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR );
}

void spell_control_weather(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    if (!target_name[0] || !str_cmp( target_name, "better" ) )
    {
	mud_data.weather_info.change += dice( level, 4 );
        send_to_char( "The sky seems to clear up a bit.\n\r", ch );
    }
    else if ( !str_cmp( target_name, "worse" ) )
    {
	mud_data.weather_info.change -= dice( level, 4 );
        send_to_char( "The sky seems to get cloudier.\n\r", ch );
    }
    else
	send_to_char ("Do you want it to get better or worse?\n\r", ch );
}

void spell_create_food( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    char* food[] = {"mushroom", "berry", "apple", "steak", "pig roast", "meal"};
    const int i = number_range(0, 5);
    char buf[MIL];

    OBJ_DATA *mushroom = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), 0);
    mushroom->value[0] = level / 2;
    mushroom->value[1] = level;

    /* change the short long and name */
    sprintf(buf, mushroom->name, food[i]);
    free_string(mushroom->name);
    mushroom->name = str_dup(buf);

    sprintf(buf, mushroom->short_descr, food[i]);
    free_string(mushroom->short_descr);
    mushroom->short_descr = str_dup(buf);

    sprintf(buf, mushroom->description, food[i]);
    free_string(mushroom->description);
    mushroom->description = str_dup(buf);

    obj_to_room( mushroom, ch->in_room );
    act( "$p suddenly appears.", ch, mushroom, NULL, TO_ALL );
}

void spell_create_spring(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    OBJ_DATA *spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), 0);
    spring->timer = level;
    obj_to_room( spring, ch->in_room );
    act( "$p flows from the ground.", ch, spring, NULL, TO_ALL );
}

void spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int water;
    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "It is unable to hold water.\n\r", ch );
	return;
    }
    if ( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
    {
	send_to_char( "It contains some other liquid.\n\r", ch );
	return;
    }
    water = UMIN( level * (mud_data.weather_info.sky >= SKY_RAINING ? 4 : 2), obj->value[0] - obj->value[1]);
    if ( water > 0 )
    {
	obj->value[2] = LIQ_WATER;
	obj->value[1] += water;
	if ( !is_name( "water", obj->name ) )
	{
	    char buf[MSL];
	    sprintf( buf, "%s water", obj->name );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	}
        act( "$p fills with water.", ch, obj, NULL, TO_CHAR );
    }
    if ( obj->value[1] >= obj->value[0] )
      {
	act( "$p is now full to the brim.", ch, obj, NULL, TO_CHAR );
	return;
      }
}

void spell_cure_blindness(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if ( !is_affected( victim, gsn_blindness ) )
    {
        if (victim == ch)
            act("You aren't blind.",ch,NULL,victim,TO_CHAR);
        else
            act("$N doesn't appear to be blinded.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if (check_dispel(level,victim,gsn_blindness))
    {
        act_new("Your vision returns!",ch,NULL,victim,TO_VICT,POS_DEAD );
        act("$N is no longer blinded.",ch,NULL,victim,TO_CHAR);
    	if (!IS_NPC(ch) && !IS_NPC(victim) && ch->class == class_lookup("healer") &&  ch != victim)
	    gain_exp(ch,number_range(1,ch->level/8));
    }
    else
        act("You failed to cure $N's blindness.",ch,NULL,victim,TO_CHAR);
}

void spell_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal = dice(3, 8) + level - 6;
//Embrace check.
    if (is_affected(victim, gsn_embrace_poison))
      {
	if (embrace_heal_check(sn, level, ch, victim,  heal))
	  affect_strip(victim, gsn_embrace_poison);
	else
	  return;
      }

    if (!IS_NPC(ch) && !IS_NPC(victim) && ch->class == class_lookup("healer") &&  ch != victim && victim->hit < victim->max_hit)
      gain_exp(ch,number_range(2,ch->level/7));
    victim->hit = UMIN( victim->hit + heal, victim->max_hit );
    update_pos( victim );


    act_new("You feel a lot better!", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
    if ( ch != victim )
      act("You cure $N's critical wounds.", ch,NULL,victim,TO_CHAR );

}

void spell_cure_disease( int sn, int level, CHAR_DATA *ch,void *vo,int target)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  bool fSick = FALSE;

  if ( is_affected( victim, gsn_plague ) ){
    fSick = TRUE;
    if (check_dispel(level,victim,gsn_plague)){
      if (!IS_NPC(ch)
	  && !IS_NPC(victim)
	  && ch->class == class_lookup("healer")
	  &&  ch != victim)
	gain_exp(ch,number_range(1,ch->level/8));
    }
    else
      act("You failed to remove $N's disease.",ch,NULL,victim,TO_CHAR);
  }
  if (is_affected( victim, gsn_drug_use) ){
    fSick = TRUE;
    if (check_dispel(level,victim,gsn_drug_use)){
      act("$n looks relieved as $e overcomes $s addiction.",victim,NULL,NULL,TO_ROOM);
    }
    else
      act("You failed to remove $N's addiction.",ch,NULL,victim,TO_CHAR);
  }
  if (!fSick){
    if (victim == ch)
      act("You aren't ill.",ch,NULL,victim,TO_CHAR);
    else
      act("$N doesn't appear to be suffering.",ch,NULL,victim,TO_CHAR);
    return;
  }
}

void spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal = dice(1, 8) + level / 3;
    if ((ch->class == class_lookup("dark-knight") || ch->class == class_lookup("ninja")) && victim != ch)
    {
	send_to_char("You can't use your talents to aid others.\n\r",ch);
	return;
    }
    if (!IS_NPC(ch) && !IS_NPC(victim) && ch->class == class_lookup("healer") &&  ch != victim && victim->hit < victim->max_hit)
	gain_exp(ch,number_range(1,ch->level/8));
    victim->hit = UMIN( victim->hit + heal, victim->max_hit );
    update_pos( victim );

//Embrace check.
    if (is_affected(victim, gsn_embrace_poison))
      {
	if (embrace_heal_check(sn, level, ch, victim,  heal))
	  affect_strip(victim, gsn_embrace_poison);
	else
	  return;
      }
    else
      {
	act_new("You feel a little better.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
	if ( ch != victim )
	  act("You cure $N's light wounds.", ch,NULL,victim,TO_CHAR );
      }

}

void spell_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if ( !is_affected( victim, gsn_poison ) )
    {
        if (victim == ch)
            act("You aren't poisoned.",ch,NULL,victim,TO_CHAR);
        else
            act("$N doesn't appear to be poisoned.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if (check_dispel(level,victim,gsn_poison))
    {
        act_new("A warm feeling runs through your body.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
        act("$n looks much better.",victim,NULL,NULL,TO_ROOM);
    	if (!IS_NPC(ch) && !IS_NPC(victim) && ch->class == class_lookup("healer") &&  ch != victim)
	    gain_exp(ch,number_range(1,ch->level/8));
    }
    else
        act("You failed to cure $N of $S poison.",ch,NULL,victim,TO_CHAR);
}

void spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal = dice(2, 8) + level /2 ;
//Embrace check.
    if (is_affected(victim, gsn_embrace_poison))
      {
	if (embrace_heal_check(sn, level, ch, victim,  heal))
	  affect_strip(victim, gsn_embrace_poison);
	else
	  return;
      }

    act_new("You feel better.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
    if ( ch != victim )
      act("You cure $N's serious wounds.", ch,NULL,victim,TO_CHAR );
    if (!IS_NPC(ch) && !IS_NPC(victim) && ch->class == class_lookup("healer") &&  ch != victim && victim->hit < victim->max_hit)
      gain_exp(ch,number_range(1,ch->level/7));
    victim->hit = UMIN( victim->hit + heal, victim->max_hit );
    update_pos( victim );


}

void spell_curse( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (IS_AFFECTED(victim,AFF_CURSE))
    {
        act("$N is already cursed.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim->race == race_lookup("demon"))
      {
	send_to_char("You cannot curse that which is already dammed.\n\r", ch);
	return;
      }

    if (saves_spell(level,victim,DAM_NEGATIVE,skill_table[sn].spell_type))
    {
        act("You fail to curse $N.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 20 + level/5;
    af.location  = APPLY_HITROLL;
    af.modifier  = -1 * (level / 8);
    af.bitvector = AFF_CURSE;
    affect_to_char( victim, &af );
    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = level / 8;
    affect_to_char( victim, &af );
    act_new("You feel unclean.", ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
	act("$N looks very uncomfortable.",ch,NULL,victim,TO_CHAR);
}

void spell_demonfire(int sn, int level, CHAR_DATA *ch, void *vo,int target){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam =  2 * level + dice(2, 15);

  if (!IS_EVIL(ch) || IS_AFFECTED(ch, AFF_CALM)){
    send_to_char("You lack the malice neccessary.\n\r", ch);
    return;
  }
  if (IS_EVIL(ch) && IS_EVIL(victim) && number_percent( ) < 50){
    victim = ch;
    send_to_char("The demons turn upon you!\n\r",ch);
  }
  if (victim != ch ){
    act("$n calls forth the demons of Hell upon $N!", ch,NULL,victim,TO_ROOM);
    act_new("$n has assailed you with the demons of Hell!", ch,NULL,victim,TO_VICT,POS_DEAD);
    act("You conjure forth the demons of Hell!",ch,NULL,victim,TO_CHAR);
  }
  if (ch != victim && victim->race == race_lookup("demon")){
    act("Something is not right...", victim, NULL, NULL, TO_ROOM);
    send_to_char("You easly turn the demons away.\n\r", ch);
    victim = ch;
    }
  if ( saves_spell( level, victim, DAM_NEGATIVE,skill_table[sn].spell_type))
    dam /= 2;
  damage( ch, victim, dam, sn, DAM_NEGATIVE ,TRUE);
}

void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED(victim, AFF_DETECT_EVIL) )
    {
	if (victim == ch)
            act("You can already sense evil.",ch,NULL,victim,TO_CHAR);
	else
            act("$N can already detect evil.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_EVIL;
    affect_to_char( victim, &af );
    act_new( "Your eyes tingle.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
    if ( ch != victim )
        act( "$N can now detect evil.", ch,NULL,victim,TO_CHAR );
}

void spell_omnipotence( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected(victim, sn)){
      act("You're already omnipotent.",ch,NULL,victim,TO_CHAR);
      return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level / 5;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    act_new( "Your brain seems to itch for a moment.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
}

void spell_detect_good( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED(victim, AFF_DETECT_GOOD) )
    {
        if (victim == ch)
            act("You can already sense good.",ch,NULL,victim,TO_CHAR);
        else
            act("$N can already detect good.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_GOOD;
    affect_to_char( victim, &af );
    act_new( "Your eyes tingle.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
    if ( ch != victim )
        act( "$N can now detect good.", ch,NULL,victim,TO_CHAR );
}

void spell_detect_invis( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED(victim, AFF_DETECT_INVIS) )
    {
        if (victim == ch)
            act("You can already see invisible objects.",ch,NULL,victim,TO_CHAR);
        else
            act("$N can already see invisible things.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = UMAX(8, level);
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_INVIS;
    affect_to_char( victim, &af );
    act_new( "Your eyes tingle.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
    if ( ch != victim )
        act("$N can now see invisible things.",ch,NULL,victim,TO_CHAR);
}

void spell_detect_magic( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED(victim, AFF_DETECT_MAGIC) )
    {
        if (victim == ch)
            act("You can already sense magical auras.",ch,NULL,victim,TO_CHAR);
        else
            act("$N can already detect magical auras.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_MAGIC;
    affect_to_char( victim, &af );
    act_new( "Your eyes tingle.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
    if ( ch != victim )
        act("$N can now detect magical auras.",ch,NULL,victim,TO_CHAR);
}

void spell_detect_traps( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected(victim, sn))
    {
        if (victim == ch)
	  act("You can already sense traps.",ch,NULL,victim,TO_CHAR);
        else
	  act("$N can already detect traps.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level > 50 ? 72 : 24;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "Your eyes tingle.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
    if ( ch != victim )
      act("$N can now detect traps.",ch,NULL,victim,TO_CHAR);
}

void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    if ( !IS_NPC(ch) && IS_EVIL(ch) )
	victim = ch;
    if ( IS_GOOD(victim) )
    {
	act( "$N is protected by $S goodness.", ch, NULL, victim, TO_ROOM );
	return;
    }
    if ( IS_NEUTRAL(victim) )
    {
	act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
	return;
    }
    if (ch->class == class_lookup("healer"))
    	dam = dice( level/5, 14 );
    else
    	dam = dice( level, 3 );
    if ( saves_spell( level, victim,DAM_HOLY,skill_table[sn].spell_type) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_HOLY ,TRUE);
}

void spell_dispel_good( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    if ( !IS_NPC(ch) && IS_GOOD(ch) )
        victim = ch;
    if ( IS_EVIL(victim) )
    {
        act( "$N is protected by $S evil.", ch, NULL, victim, TO_ROOM );
        return;
    }
    if ( IS_NEUTRAL(victim) )
    {
        act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
        return;
    }
    if (victim->hit > (ch->level * 4))
        dam = dice( level, 3 );
    else
        dam = UMAX(victim->hit, dice(level,3));
    if ( saves_spell( level, victim,DAM_NEGATIVE,skill_table[sn].spell_type) )
        dam /= 2;
    damage( ch, victim, dam, sn, DAM_NEGATIVE ,TRUE);
}

void spell_earthquake( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *vch, *vch_next;
    AFFECT_DATA* paf;
    bool fBurrow = FALSE;

    act( "The earth trembles beneath your feet!", ch, NULL, NULL, TO_CHAR );
    act( "$n makes the earth tremble and shiver.", ch, NULL, NULL, TO_ROOM );
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( (paf = affect_find(vch->affected, gsn_burrow)) != NULL)
	  {
	    if (paf->modifier == ch->in_room->vnum)
	      fBurrow = TRUE;
	  }

	if ( vch->in_room == ch->in_room || fBurrow)
	{
	    if ( vch != ch && !is_area_safe_quiet(ch,vch))
            {
	      if (fBurrow)
		send_to_char("The ground begins to heave and collapse around you!\n\r", vch);
	      else
		m_yell(ch,vch,FALSE);
	      if (IS_AFFECTED(vch,AFF_FLYING) && !is_affected(vch,gsn_thrash) && !fBurrow)
                    damage(ch,vch,0,sn,DAM_BASH,TRUE);
	      else if (is_affected(vch,gsn_burrow))
		damage( ch,vch,2*(level + dice(2, 8)), sn, DAM_BASH,TRUE);
	      else
		damage( ch,vch,level + dice(2, 8), sn, DAM_BASH,TRUE);
            }
	    continue;
	}
	if ( vch->in_room->area == ch->in_room->area )
	  send_to_char( "The earth trembles and shivers.\n\r", vch );
    }
}

void spell_enchant_armor( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{

//object to be enchanted
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  AFFECT_DATA *paf;

  //bool
  bool  fHum = FALSE;
//dice
  int result;

//base chanc to fail
  int fail = 10;


//bace ac bonus
  int ac_bonus = 0;


//buffer used for ac added.
  int added;

//bool flag
  bool ac_found = FALSE;
  bool raise = FALSE;

//MODIFERS
  const int fail_per_ac = 3;      //Chance increase per point of AC enchanted.
  const int lvl_mod =     1;      //level mod
  const int glow_bon =   -5;      //bonus for glow
  const int bless_bon =  15;      //bless bon.
  const int luck_mod =    2;      //luck
  const int skill_mod =	  1;	//Divisor for skill > 75

//failure levels
  const int shatter_lvl = 6;   //Item shatters if below chance/shatter_lvl
  const int fade_lvl =    5;   //item fades
  const int lower_lvl =   2;   //ac bonus is lowered by one.


//We check first if the item is armor
  if (obj->item_type != ITEM_ARMOR)
    {
      send_to_char("That item is not an armor piece.\n\r",ch);
      return;
    }


//and has to be in inv.
  if (obj->wear_loc != -1)
    {
      send_to_char("The item must be carried to be enchanted.\n\r",ch);
      return;
    }
  if (is_affected(ch, skill_lookup("calm")))
    {
      send_to_char("You are in far to peacfull mood to think of violence.", ch);
      return;
    }
  if (obj->pIndexData->vnum == OBJ_VNUM_ARMORCRAFT){
    send_to_char("It seems to resist your magics without effort.\n\r", ch);
    return;
  }

//roll for items not enchanted yet.
  if (!obj->enchanted)
    {
      for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
	{
//we run through the affects checking for the one for ac.
	  if ( paf->location == APPLY_AC )
	    {
//make the pointers point to the ac_bonus so we can change it later.
	      ac_bonus = paf->modifier;
	      ac_found = TRUE;
//chance to fail, lower is better.
	      if (ac_bonus > 5)
		fHum =TRUE;
	      fail += (ac_bonus * ac_bonus) * fail_per_ac;
	    }//end if APPLY_AC
	  else
	    fail += 7;
	}//end for
    }//if not enchanted


//check for objects already enchanted
  for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
      if ( paf->location == APPLY_AC )
	{
	  ac_bonus = paf->modifier;
	  ac_found = TRUE;
//we increase chacne to fail based on what ac is now.
	  if (ac_bonus > 5)
	    fHum =TRUE;
	  fail += (ac_bonus * ac_bonus) * fail_per_ac;
	}//and if APPLY_AC
      else
//first enchant, we add base amount fail
	fail += 7;
    }//end for



//we begin calculatin chances.
  fail -= level * lvl_mod;

//bonus to blessed items.
  if (IS_OBJ_STAT(obj,ITEM_BLESS))
    fail -= glow_bon;

//bonus to glowing/already enchatned items.
  if (IS_OBJ_STAT(obj,ITEM_GLOW))
    fail -= bless_bon;

//luck modifier
  fail -= (get_curr_stat(ch,STAT_LUCK) - 16)*luck_mod;

 //skill mod
  fail -= (get_skill(ch, sn) - 75) / skill_mod;

//ceiling.
  fail = URANGE(5,fail,85);


//we caluclate the chance
  result = number_percent();

//now we applie results.
  if (result < (fail / shatter_lvl ))
    {
      act("$p flares blindingly... and evaporates!",ch,obj,NULL,TO_ALL);
      extract_obj(obj);
      return;
    }
  else if (result < (fail / fade_lvl))
    {
      AFFECT_DATA *paf_next;
      act("$p glows brightly, then fades.",ch,obj,NULL,TO_ALL);
      //we remove all affects.
      obj->enchanted = TRUE;
      for (paf = obj->affected; paf != NULL; paf = paf_next)
	{
	  paf_next = paf->next;
	  //not the owner effect.
	  if (paf->type == gen_has_owner)
	    continue;

	  free_affect(paf);
	}
      obj->affected = NULL;
      REMOVE_BIT(obj->extra_flags,ITEM_GLOW);
      REMOVE_BIT(obj->extra_flags,ITEM_HUM);
      REMOVE_BIT(obj->extra_flags,ITEM_MAGIC);
      return;
    }
  //increase ac.
  else if ( result < (fail/lower_lvl))
    raise = TRUE;
  else if ( result <= fail)
    {
      send_to_char("Nothing seemed to happen.\n\r",ch);
      return;
    }
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
  if (result <= (90 - level/5))
    {
      if (!raise)
	{
//we roll for a chance to really raise it.
	  if (number_percent() < 75)
	    {
	      act("$p shimmers with a gold aura.",ch,obj,NULL,TO_ALL);
	      added = -1;
	    }
	  else
	    {
	  act("$p shivers and sizzles with sudden heat!",ch,obj,NULL,TO_ALL);
	  added = 1;
	    }
	}
      else
	{
	  act("$p begins to shimmer with a gold aura then suddenly begins to smoke!.",ch,obj,NULL,TO_ALL);
	  added = 1;
	}
      SET_BIT(obj->extra_flags, ITEM_MAGIC);
    }
  else
    {
      if (!raise)
	{
	  act("$p glows a brillant gold!",ch,obj,NULL,TO_ALL);
	  if (!IS_SET(obj->extra_flags,ITEM_GLOW))
	    SET_BIT(obj->extra_flags,ITEM_GLOW);
	  else
	    SET_BIT(obj->extra_flags,ITEM_HUM);
	  added = -2;

	}
      else
	{
	  act("$p shivers and sizzles with heat!",ch,obj,NULL,TO_ALL);
	  added = 2;
	}
      SET_BIT(obj->extra_flags, ITEM_MAGIC);
    }
  if (obj->level < LEVEL_HERO)
    obj->level = UMIN(LEVEL_HERO - 1,obj->level + 1);
  if (ac_found)
    {
      for ( paf = obj->affected; paf != NULL; paf = paf->next)
	if ( paf->location == APPLY_AC)
	  {
	    paf->type = sn;
	    paf->modifier += added;
	    if (fHum && added < 0)
	      SET_BIT(obj->extra_flags,ITEM_HUM);
	    paf->level = UMAX(paf->level,level);
	  }
    }
  else
    {
      paf = new_affect();
      paf->where= TO_OBJECT;
      paf->type= sn;
      paf->level= level;
      paf->duration= -1;
      paf->location= APPLY_AC;
      paf->modifier=  added;
      paf->bitvector  = 0;
      paf->next= obj->affected;
      obj->affected= paf;
    }
}//end enchant armor


void spell_enchant_weapon(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  AFFECT_DATA* paf;
  char buf [MIL];
  char color [MIL];

//Data for the process
  int enchant_bonus = 0;
  int total_enchant = 0;
  int hit_bonus = 0;
  int dam_bonus = 0;
  int tot_dam = 0;
  int tot_hit = 0;
  int chance = number_percent();
//spell fails if chance is greater then fail
  int fail = 5;
//holds which type of enchant we are doing.
  int enchant_type = APPLY_NONE;

//modifiers
  const int lvl_mod = 1;
  const int Bless_mod = 15;
  const int EvilItem_mod = 15;
  const int glow_mod = 10;
  const int hum_mod = 5;
  const int intwis_mod = 2;
  const int luck_mod = 2;
  const int skill_mod = 1;   //Skill mod
  const int limit_mod = 5;   //modifier for over 6 enchant (over 6)^2*limit_mod
  const int good_mod = -20;  //modifer in percent for chance to get damage if good.
  const int evil_mod = 20;   //modifer in percent for chance to get damage if evil.
 int align_med = 50;  //median


//levels in percent
  const int shatter_lvl = 3;//destroy
  const int fry_lvl = 15;//lower DAMTYPE or DAMDICE
  const int fade_lvl = 30;//fade weapon
  const int lower2_lvl = 45;//lower affect by 1 or 2
  const int lower_lvl = 65;//lower affect by 1 or 2
  const int nothing_lvl = 100;//nothin
//Level to enchant single is >fail
  //  const int single_lvl = 60;//single poit raise to hit or dam
  int double_lvl = 100 - level/5;//double raise to hit or dam


//flags
  bool blow = FALSE;
  bool lower = FALSE;
  bool enchant_found = FALSE;
  bool ch_evil = FALSE;
  bool ch_good = FALSE;


//first we check if this is a weapon

  if (obj->item_type != ITEM_WEAPON){
      send_to_char("That item is not a weapon.\n\r",ch);
      return;
    }

  if (obj->wear_loc != WEAR_NONE)
    {
      send_to_char("The item must be carried to be enchanted.\n\r",ch);
      return;
    }

  if (is_affected(ch, skill_lookup("calm")))
    {
      send_to_char("You are in far to peaceful mood to think of violence.", ch);
      return;
    }

  //Malform weapon check.
  if (affect_find(obj->affected, gen_malform)
      || obj->pIndexData->vnum == OBJ_VNUM_WEAPONCRAFT)
    {
      act("$p seems to soak the spell right up with no other visible effect.", ch, obj, NULL, TO_ALL);
      return;
    }

//now we check align of char.
//and modifiy chance for damroll enchantement.
//and the color stored in color
  if (ch->alignment <= EVIL_THRESH)
    {
      ch_evil = TRUE;
      align_med += evil_mod;
    }
  else if (ch->alignment >GOOD_THRESH)
    {
      ch_good = TRUE;
      align_med += good_mod;

    }

//we roll if this is going to be enchant of hit or damage
  if (chance < align_med)
    {
      enchant_type = APPLY_DAMROLL;
      sprintf(color, "red aura");
    }
  else
  {
      sprintf(color, "blue aura");
      enchant_type = APPLY_HITROLL;
  }
  //we randomize again to be sure of good distribution.
  chance = number_percent();


//we check if an enchatement exits already
  if (!obj->enchanted)
    {
      for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
	if ( paf->location == APPLY_DAMROLL )
	  {
	    if (paf->location == enchant_type)
	      enchant_found = TRUE;

	    //we add all the existing enchants up.
	    dam_bonus = paf->modifier;
	    tot_dam += dam_bonus;
	    fail += 2 * (dam_bonus * dam_bonus);
	  }
	else if ( paf->location == APPLY_HITROLL )
	  {
	    if (paf->location == enchant_type)
	      enchant_found = TRUE;

	    //we add all the existing enchants up.
	    hit_bonus = paf->modifier;
	    tot_hit += hit_bonus;
	    fail += 2 * (hit_bonus * hit_bonus);
	  }

    }

//We scan regular way now to get the total of effects on obj.
  for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
      if ( paf->location == APPLY_DAMROLL )
	{
	    if (paf->location == enchant_type)
	      enchant_found = TRUE;

	    //we add all the existing enchants up.
	    dam_bonus = paf->modifier;
	    tot_dam += dam_bonus;
	    fail += 2 * (dam_bonus * dam_bonus);
	}
      else if ( paf->location == APPLY_HITROLL )
	{
	    if (paf->location == enchant_type)
	      enchant_found = TRUE;

	    //we add all the existing enchants up.
	    hit_bonus = paf->modifier;
	    tot_hit += hit_bonus;
	    fail += 2 * (hit_bonus * hit_bonus);
	}
    }
  //we average out the enchants
  total_enchant = (tot_hit + tot_dam)/2;

  //we now have total enchants already on the item.
  //start caluclating chances.
  //the higher "fail" the smaller chance to enchant and greater risk.

//level bonus
  fail -= level * lvl_mod;

 //skill mod
  fail -= (get_skill(ch, sn) - 75) / skill_mod;

//smarts bonus
  fail -=(  ((get_curr_stat(ch, STAT_INT) - 21) + (get_curr_stat(ch, STAT_WIS) - 21) ) / 2) * intwis_mod;
//penalty for objects of higher level then caster.
  if (obj->level > level)
    fail += (obj->level - level) * lvl_mod;


//Align things.
  if (IS_OBJ_STAT(obj,ITEM_BLESS) && ch_good)
    {fail -= Bless_mod;chance+=Bless_mod/2;}
  if (IS_OBJ_STAT(obj,ITEM_EVIL) && ch_evil)
    {fail -= EvilItem_mod;chance+=EvilItem_mod/2;}

  if (IS_OBJ_STAT(obj,ITEM_GLOW))
    fail += glow_mod;
  if (IS_OBJ_STAT(obj,ITEM_HUM))
    fail += hum_mod;


//penalty for superduper lvl enchant.
//penalty for average, or high singulars.
  if (total_enchant >= 5 || (tot_hit > 5 || tot_dam > 5))
    fail+= (total_enchant - 4) * (total_enchant - 4) * limit_mod;

//ceiling for super dupers enchants.
  if (total_enchant >= 7 || (tot_hit >=7 || tot_dam >= 7))
    blow = TRUE;

//check for manacharge
if ((paf = affect_find(obj->affected, gen_mana_charge)) != NULL)
  blow = TRUE;


//luck bonus
  fail -= (get_curr_stat(ch,STAT_LUCK) - 16) * luck_mod;

//ceiling
  fail = URANGE(5,fail,90);
  chance = URANGE(0,chance,100);
  //DEBUG
  //sprintf(buf, "Enchnat: Chance= %d, Fail = %d \n\r", chance, fail);
  //send_to_char(buf, ch);
//begin enchanting, from worst to best.

//shatter
  if ( (chance < (shatter_lvl * fail /100)) || (chance <= 3) || (blow && (chance >= nothing_lvl * fail / 100)) )
    {
      act("$p shivers violently and shatters into a million pieces!",ch,obj,NULL,TO_ALL);
      extract_obj(obj);
      return;
    }
//fry ie: lower weapon dice.
  else if  (chance < (fry_lvl * fail /100) || (chance <= 8))
    {
//50:50
      act("$p shivers and smokes with sudden heat.",ch,obj,NULL,TO_ALL);

      if (number_percent() < 50)
	{
	  //we try to fand first die, if cant we try second.
	  if (obj->value[1] > 1) obj->value[1]--;
	  else   if (obj->value[2] > 1) obj->value[2]--;
	}
      else
	{
	  //we try to fadie SECOND die if not then try first.
	  if (obj->value[2] > 1) obj->value[2]--;
	  else   if (obj->value[1] > 1) obj->value[1]--;
	}
      return;
    }
//fade, remove all enchants
  else if  (chance < (fade_lvl * fail /100))
    {
      AFFECT_DATA *paf_next;

      act("$p flares with blinding light, then quickly fades.",ch,obj,NULL,TO_ALL);
      obj->enchanted = TRUE;
      for (paf = obj->affected; paf != NULL; paf = paf_next)
	{
	  paf_next = paf->next;
	  //not the owner effect.
	  if (paf->type == gen_has_owner)
	    continue;
	  free_affect(paf);
	}
      obj->affected = NULL;
      REMOVE_BIT(obj->extra_flags,ITEM_GLOW);
      REMOVE_BIT(obj->extra_flags,ITEM_HUM);
      REMOVE_BIT(obj->extra_flags,ITEM_MAGIC);
      return;
    }
//lower, the enchant is inverted
  else if  (chance < (lower_lvl * fail /100))
     lower = TRUE;
//nothing
  else if  (chance < (nothing_lvl * fail /100))
    {
      send_to_char("Nothing seemed to happen.\n\r",ch);
      return;
    }


//begin enchanting, inverse or regular.notice these are not elseif now.
  //double enchant with a check for double fade.
  if ((chance >= double_lvl) || (lower && (chance < (lower2_lvl * fail /100))) )
    {
//jackpot check
      if (chance >=99)
	{
	  sprintf(buf, "The room is painted with a %s as $p flares with arcane power!",color);
	  enchant_bonus = 3;
	}//end if jackpot, begin regualr.
      else
	{
	  enchant_bonus = 2;
	  if (lower)
	    {
	      enchant_bonus *= -1;
	      sprintf(buf, "$p glows with a %s then shudders violently and fades.",color);
	    }
	  else
	    sprintf(buf, "$p glows with a brilliant %s.",color);
	}//end else if not jackpot
//we shoot out the message.
      act(buf,ch,obj,NULL,TO_ALL);
    }//end double enchant
//regular 1 point enchant,
  else if ( (chance >= fail) || lower)
    {
      enchant_bonus = 1;
//we include the color of enchantement here.
      if (lower)
	{
	  enchant_bonus *= -1;
	  sprintf(buf, "$p glows with a %s then quickly fades to gray.",color);
	}
      else
	sprintf(buf, "$p glows with a %s.",color);

//we shoot out the message.
      act(buf,ch,obj,NULL,TO_ALL);


    }//end single enchant


//now we actualuy modify the effects.
//first set the glow/hum/magic flags.

  if (obj->level < LEVEL_HERO - 1)
    obj->level = UMIN(LEVEL_HERO - 1,obj->level + 1);

//always set
  SET_BIT(obj->extra_flags, ITEM_MAGIC);

//set only if bonus is greater then 1
  if (enchant_bonus > 1)
    SET_BIT(obj->extra_flags,ITEM_GLOW);

//set if total enchant is more then 4 on obj,
  if (total_enchant > 4)
    SET_BIT(obj->extra_flags,ITEM_HUM);

//now we slap down the effects.
//If object was not previously affected we copy affects.
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
	  af_new->has_string = paf->has_string;
	  af_new->string = paf->string;
	}
    }

  if (enchant_found)
    {
      for ( paf = obj->affected; paf != NULL; paf = paf->next)
        if ( paf->location == enchant_type)
	  {
	    paf->type = sn;
	    paf->modifier += enchant_bonus;
	    paf->level = UMAX(paf->level,level);
	  }
    }//end if already enchatned.
  else
    {
      paf = new_affect();
      paf->where= TO_OBJECT;
      paf->type= sn;
      paf->level= level;
      paf->location  = enchant_type;
      paf->duration= -1;
      paf->modifier = enchant_bonus;
      paf->bitvector  = 0;
      paf->next= obj->affected;
      obj->affected= paf;
    }//done if no similiar enchants.
}//edn spell_enchant_weapon


void spell_energy_drain( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    if ( saves_spell( level , victim,DAM_NEGATIVE,skill_table[sn].spell_type) )
    {
	act("You failed to drain $N's energy.",ch,NULL,victim,TO_CHAR);
	act_new("You feel a momentary chill.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
	return;
    }
    if ( victim->level <= 2 ){
      act("$n withers before your eyes as all his energy is sucked away.", victim, NULL, NULL, TO_ROOM);
      dam		 = ch->hit + 1;
    }
    else
    {
	gain_exp( victim, 0 - number_range( level/2, 3 * level / 2 ) );
	victim->mana	/= 2;
	victim->move	/= 2;
	dam		 = dice(1, level);
	ch->hit		+= dam;
    }
    act_new("You feel your life slipping away!",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    act("What a rush!",ch,NULL,victim,TO_CHAR);
    damage( ch, victim, dam, sn, DAM_NEGATIVE ,TRUE);
}

void spell_drained( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn))
      return;

    if ( ch != victim && saves_spell( level , victim,DAM_NEGATIVE,skill_table[sn].spell_type) )
    {
      act("You failed to drain $N's vitality.",ch,NULL,victim,TO_CHAR);
      act_new("You feel a momentary weakness.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
      return;
    }
    act_new("You feel your vitality slipping away!",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if (ch != victim)
      act("You drain $N of $S vitality!",ch,NULL,victim,TO_CHAR);
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level / 8;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char( victim, &af );
}


void spell_fireball( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  int dam;
  int save;

  CHAR_DATA *vch, *vch_next;
  static const sh_int dam_each[] =
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,  30,	 35,  40,  45,  50,  55,
	 56,  57,  58,  59,  60,	 61,  62,  63,  64,  65,
	 67,  69,  71,  73,  75,  	 80,  85,  90,  95, 100,
	101, 102, 103, 104, 105,	106, 107, 108, 109, 110
    };
  level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
  level	= UMAX(0, level);


  for (vch = ch->in_room->people; vch != NULL; vch = vch_next){
    int res = check_immune(vch, DAM_FIRE, FALSE);

    vch_next = vch->next_in_room;
    dam		= number_range(dam_each[level], dam_each[level] * 2 );
    if (res != IS_VULNERABLE && ch->race == grn_demon)
      dam *= 1.2;

    //the higher the victim saves, the less damage fireball does
    save = URANGE(25, calc_saves(level, vch, DAM_FIRE, SPELL_AFFLICTIVE), 95);

    //adjust saves for magic vulnerability, they already take +50% damage as is
    if (res == IS_VULNERABLE)
      save += 15;

    dam =  (200 - 2 * save) * dam / 100;

    if ( vch != ch && !is_area_safe(ch,vch)){
      m_yell(ch,vch,FALSE);
      damage( ch, vch, dam, sn, DAM_FIRE ,TRUE);
    }
  }
}

void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  CHAR_DATA *vch, *vch_next;
  int dam;
  bool fAdv = ch->class == class_lookup("paladin");

  for (vch = ch->in_room->people; vch; vch = vch_next ){
    vch_next = vch->next_in_room;
    if (vch == ch || vch == victim)
      continue;
    else if (!fAdv && vch != victim)
      continue;

    if ( (is_same_group(victim, vch) || (vch->fighting && vch->fighting == ch))
	 && !is_area_safe_quiet(ch, vch )){
      if (vch != victim)
	m_yell(ch, vch, FALSE );

      dam = level + dice(level / 5, 6);

      if ( saves_spell( level, vch,DAM_FIRE,skill_table[sn].spell_type) )
	dam /= 2;

      fire_effect(vch, ch->level, dam, TARGET_CHAR);
      damage( ch, vch, dam, sn, DAM_FIRE ,TRUE);
    }
  }
/* make sure we are fighting the victim when this ends */
  if ( saves_spell( level, victim,DAM_FIRE,skill_table[sn].spell_type) )
    dam /= 2;

  dam = level + dice(level / 5, 6);

  set_fighting( ch, victim );
  fire_effect(victim, ch->level, dam, TARGET_CHAR);
  damage( ch, victim, dam, sn, DAM_FIRE ,TRUE);


}

void spell_faerie_fire( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) || victim->race == race_lookup("faerie"))
    {
        if (victim == ch)
            act("You are already outlined in pink.",ch,NULL,victim,TO_CHAR);
        else
            act("$N is already outlined in pink.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( saves_spell( level, victim,DAM_OTHER,skill_table[sn].spell_type) )
    {
	act("You failed to outline $N in pink.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 10;
    af.location  = APPLY_AC;
    af.modifier  = level;
    af.bitvector = AFF_FAERIE_FIRE;
    affect_to_char( victim, &af );
    affect_strip ( victim, gsn_invis			);
    affect_strip ( victim, gsn_mass_invis		);
    if ( IS_NPC(victim) || !IS_SET(race_table[victim->race].aff,AFF_SNEAK) )
    {
    	affect_strip ( victim, gsn_sneak			);
	REMOVE_BIT   ( victim->affected_by, AFF_SNEAK	);
    }
    REMOVE_BIT   ( victim->affected_by, AFF_HIDE	);
    REMOVE_BIT   ( victim->affected_by, AFF_INVISIBLE	);
    affect_strip(victim,gsn_camouflage);
    REMOVE_BIT   ( victim->affected2_by, AFF_CAMOUFLAGE	);
    act_new( "You are surrounded by a pink outline.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n is surrounded by a pink outline.", victim, NULL, NULL, TO_ROOM );
}

void spell_faerie_fog( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *ich;
    act( "$n conjures a cloud of yellow dust.", ch, NULL, NULL, TO_ROOM );
    act( "You conjure a cloud of yellow dust.", ch, NULL, NULL, TO_CHAR );
    for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
    {
        AFFECT_DATA af;
        if ( IS_AFFECTED(ich, AFF_FAERIE_FOG) || ich->race == race_lookup("faerie"))
	  continue;
	if (ich->invis_level > 0)
	    continue;
	if ( ich == ch || is_safe(ch,ich) || saves_spell( level, ich,DAM_OTHER,skill_table[sn].spell_type) )
	    continue;
	a_yell( ch, ich );
	affect_strip ( ich, gsn_invis			);
	affect_strip ( ich, gsn_mass_invis		);
	if ( IS_NPC(ich) || !IS_SET(race_table[ich->race].aff,AFF_SNEAK) )
	{
	    affect_strip ( ich, gsn_sneak			);
	    REMOVE_BIT   ( ich->affected_by, AFF_SNEAK	);
	}
	REMOVE_BIT   ( ich->affected_by, AFF_HIDE	);
	REMOVE_BIT   ( ich->affected_by, AFF_INVISIBLE	);
    	affect_strip(ich,gsn_camouflage);
    	REMOVE_BIT   ( ich->affected2_by, AFF_CAMOUFLAGE	);
        af.where     = TO_AFFECTS;
        af.type      = sn;
        af.level     = level;
        af.duration  = 10;
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = AFF_FAERIE_FOG;
        affect_to_char( ich, &af );
        act_new( "You start glowing.", ich, NULL, NULL, TO_CHAR, POS_DEAD );
        act( "$n starts glowing.", ich, NULL, NULL, TO_ROOM );
    }
}

void spell_fly( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_EVIL(ch) && IS_GOOD(victim))) && !IS_IMMORTAL(ch))
    {
	sendf(ch, "It would go against your beliefs to aid %s.\n\r", PERS(victim,ch));
	return;
    }
    if ( IS_AFFECTED(victim, AFF_FLYING) )
    {
	if (victim == ch)
            act("You are already airborne.",ch,NULL,victim,TO_CHAR);
	else
            act("$N doesn't need your help to fly.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level + 3;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_FLYING;
    affect_to_char( victim, &af );
    act_new( "Your feet rise off the ground.", victim, NULL,NULL, TO_CHAR, POS_DEAD );
    act( "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
}

void spell_frenzy(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (is_affected(victim,sn) || IS_AFFECTED(victim,AFF_BERSERK))
    {
	if (victim == ch)
            act("You are already in a frenzy.",ch,NULL,victim,TO_CHAR);
	else
            act("$N is already in a frenzy.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (is_affected(victim,skill_lookup("calm")) || is_affected(victim,gsn_prayer))
    {
	if (victim == ch)
            act("Why don't you just relax for a while?",ch,NULL,victim,TO_CHAR);
	else
            act("$N doesn't look like $e wants to fight anymore.", ch,NULL,victim,TO_CHAR);
	return;
    }
    if ((IS_GOOD(ch) && !IS_GOOD(victim)) || (IS_NEUTRAL(ch) && !IS_NEUTRAL(victim)) || (IS_EVIL(ch) && !IS_EVIL(victim)) )
    {
	act("Your god doesn't seem to like $N",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( is_affected( victim, gsn_blasphemy ) )
    {
	sendf(ch, "%s's faith have been lost.\n\r", PERS(victim,ch));
	return;
    }
    af.where     = TO_AFFECTS;
    af.type 	 = sn;
    af.level	 = level;
    af.duration	 = level / 3;
    af.modifier  = level / 6;
    af.bitvector = 0;
    af.location  = APPLY_HITROLL;
    affect_to_char(victim,&af);
    af.location  = APPLY_DAMROLL;
    affect_to_char(victim,&af);
    af.modifier  = 10 * (level / 12);
    af.location  = APPLY_AC;
    affect_to_char(victim,&af);
    act_new("You are filled with holy wrath!",victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
}

void spell_gate( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim;
    bool gate_pet;
    if (IS_AFFECTED2(ch, AFF_SHADOWFORM))
    {
	REMOVE_BIT(ch->affected2_by, AFF_SHADOWFORM);
	affect_strip(ch, gsn_shadowform);
	act("Your body regains its substance, and you materialize into existence.",ch,NULL,NULL,TO_CHAR);
	act("$n's body regains its substance, and $e materializes into existence.",ch,NULL,NULL,TO_ROOM);
	WAIT_STATE(ch, 24);
    }
    if ( ( victim = get_char_world( ch, target_name ) ) == NULL
	 || victim == ch || victim->in_room == NULL || !can_see_room(ch,victim->in_room)
	 || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	 || ((IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	      || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	      || IS_SET(victim->in_room->room_flags, ROOM_NO_GATEIN)
	      || IS_SET(ch->in_room->room_flags, ROOM_NO_GATEOUT)
	      || IS_SET(victim->in_room->room_flags, ROOM_NO_INOUT)
	      || IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)
	      || IS_SET(ch->in_room->area->area_flags, AREA_NOMORTAL)
	      || IS_SET(victim->in_room->area->area_flags, AREA_NOMORTAL)
	      || IS_SET(ch->in_room->area->area_flags, AREA_RESTRICTED)
	      || IS_SET(victim->in_room->area->area_flags, AREA_RESTRICTED)
	      || victim->level >= level + 5
	      || (!IS_NPC(victim) && victim->level > LEVEL_HERO)
	      || (!IS_IMMORTAL(ch) && IS_IMMORTAL(victim))
	      || (IS_NPC(victim) && (IS_AFFECTED(victim,AFF_CHARM)
				     && victim->master != NULL
				     && victim->in_room == victim->master->in_room
				     && !is_pk(ch,victim->master)) )
	      || (!IS_NPC(victim) && IS_SET(victim->act,PLR_NOSUMMON) && !is_pk(ch,victim))
	      || is_affected(victim,gen_watchtower)
	      || is_affected(victim,gsn_coffin)
	      || is_affected(victim,gsn_entomb)
	      || is_affected(ch, gsn_tele_lock)
	      || is_affected(victim,gsn_catalepsy)
	      || (saves_spell( level- 5, victim,DAM_OTHER,skill_table[sn].spell_type) ) ) && !IS_IMMORTAL(ch)))
      {
        send_to_char("Something blocks the spell.\n\r",ch);
        return;
      }
    else if (is_affected(ch,gen_ensnare)){
      send_to_char("You sense a powerful magical field preventing your departure.\n\r", ch);
      return;
    }
    else if (!IS_NPC(victim) && !IS_IMMORTAL(ch)){
      send_to_char("Your target moves away before you complete the spell.\n\r", ch);
      return;
    }
    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL)
    {
      send_to_char("You mount refuses to enter the gate.\n\r",ch);
      return;
    }
    if (ch->pet != NULL
	&& ch->in_room == ch->pet->in_room
	&& ch->pet->pIndexData->vnum != MOB_VNUM_SPECIAL_GUARD)
      gate_pet = TRUE;
    else
      gate_pet = FALSE;
    if (ch->invis_level < LEVEL_HERO)
      {
    	act("$n steps through a gate and vanishes.",ch,NULL,NULL,TO_ROOM);
    	act("You step through a gate and vanish.",ch,NULL,NULL,TO_CHAR);
    }
    else
    {
    	act("$n steps through a gate and vanishes.",ch,NULL,NULL,TO_IMMROOM);
    	act("You step through a gate and vanish.",ch,NULL,NULL,TO_CHAR);
    }
    char_from_room(ch);
    char_to_room(ch,victim->in_room);
    if (ch->invis_level < LEVEL_HERO)
    	act("$n has arrived through a gate.",ch,NULL,NULL,TO_ROOM);
    else
    	act("$n has arrived through a gate.",ch,NULL,NULL,TO_IMMROOM);
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

void spell_giant_strength(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (ch->class == class_lookup("psionicist") && ch != victim)
    {
	send_to_char("You can only cast this spell on yourself.\n\r", ch);
	return;
    }
    if (((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_EVIL(ch) && IS_GOOD(victim))) && !IS_IMMORTAL(ch))
    {
        sendf(ch, "It would go against your beliefs to aid %s.\n\r", PERS(victim,ch));
	return;
    }
    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
            act("You are already as strong as you can get!",ch,NULL,victim,TO_CHAR);
	else
            act("$N can't get any stronger.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = level / 10;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "Your muscles surge with heightened power!", victim,NULL,NULL,TO_CHAR,POS_DEAD );
    act("$n's muscles surge with heightened power.",victim,NULL,NULL,TO_ROOM);
}

void spell_harm( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = UMAX(  20, victim->hit - dice(1,4) );
    if ( saves_spell( level, victim,DAM_HARM,skill_table[sn].spell_type) )
	dam = UMIN( 50, dam / 2 );
    dam = UMIN( 100, dam );
    damage( ch, victim, dam, sn, DAM_HARM ,TRUE);
}

void spell_haste( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_EVIL(ch) && IS_GOOD(victim))) && !IS_IMMORTAL(ch))
    {
        sendf(ch, "It would go against your beliefs to aid %s.\n\r", PERS(victim,ch));
	return;
    }
    if ( is_affected( victim, sn ) || IS_AFFECTED(victim,AFF_HASTE) || IS_SET(victim->off_flags,OFF_FAST))
    {
	if (victim == ch)
            act("You can't move any faster!",ch,NULL,victim,TO_CHAR);
 	else
            act("$N is already moving as fast as $E can.", ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    if (victim == ch)
        af.duration  = 1 + level/2;
    else
        af.duration  = 1 + level/4;
    af.location  = APPLY_DEX;
    af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
    af.bitvector = AFF_HASTE;
    affect_to_char( victim, &af );
    act_new( "You feel yourself moving more quickly.", victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n is moving more quickly.",victim,NULL,NULL,TO_ROOM);
}

void spell_heal( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
//Embrace check.
    if (is_affected(victim, gsn_embrace_poison))
      {
	if (embrace_heal_check(sn, level, ch, victim,  100))
	  affect_strip(victim, gsn_embrace_poison);
	else
	  return;
      }
    act_new( "A warm feeling fills your body.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
    if ( ch != victim )
      act( "You heal $N.", ch,NULL,victim,TO_CHAR );
    if (!IS_NPC(ch) && !IS_NPC(victim) && ch->class == class_lookup("healer") &&  ch != victim && victim->hit < victim->max_hit)
      gain_exp(ch,number_range(2,ch->level/5));
    victim->hit = UMIN( victim->hit + 100, victim->max_hit );
    update_pos( victim );
}

/* used in spell_wrath */
void spell_holy_word(int sn, int level, CHAR_DATA *ch, void *vo,int target){
  AFFECT_DATA af;
  if (is_affected(ch, sn)){
    send_to_char("You've already spoken the Divine Word.\n\r", ch);
    return;
  }
  act("$n's eyes glow slightly as $e utters a Divine Word!",ch,NULL,NULL,TO_ROOM);
  act("Your mind fills with holy wrath as the Divine Word escapes your lips!",ch,NULL,NULL,TO_CHAR);

  af.type = sn;
  af.level = level;
  af.duration = level / 6;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_MANA_GAIN;
  af.modifier = -25;
  affect_to_char(ch, &af);
}

/* SPELL HURRICANE
   General effects: light damage, possible air_thrash to flying, area.
   Coded By:        Athaekeetha
*/

void spell_hurricane( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *vch, *vch_next;       // victim char data
  AFFECT_DATA af;                  // affect data (for air_thrash)
  int dam = 0;

/*  loop  over each player in the room */
  for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
      vch_next = vch->next_in_room;

/* if victim is safe, continue to next victim */
      if (is_safe(ch,vch))
	continue;

/* if victim is charmie's master, goto next victim */
      if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == vch )
	{
	  sendf(ch, "%s is your beloved master.\n\r", PERS(vch,ch) );
	  continue;
	}

/* if victim is not caster, and they are not in a safe area, victim is hit */
      if ( vch == ch || is_area_safe(ch,vch))
	continue;

      m_yell(ch,vch,FALSE);

      dam = 50 + dice( 6, 3 ) * level / 6;

      /* if the victim saves vs. the spell, goto next victim */
      if ( saves_spell( level, vch, DAM_AIR, skill_table[sn].spell_type) ) {
	dam /= 2;
      }

/* damage the victim */
      damage( ch, vch, dam, sn, DAM_AIR, TRUE);

/* air thrash the victim if flying */
      if (IS_AFFECTED(vch, AFF_FLYING) && !is_affected(vch, gsn_thrash)){
	af.where  = TO_AFFECTS;
	af.type   = gsn_thrash;
	af.level  = ch->level;
	af.duration = 0;
	af.modifier = 0;
	af.location = 0;
	af.bitvector = 0;
	affect_to_char(vch,&af);
	act("You can't seem to get back into the air",vch,NULL,NULL,TO_CHAR);
	act("$n can't seem to get back into the air",vch,NULL,NULL,TO_ROOM);
      }
    }
}

void spell_identify( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  OBJ_DATA *obj = (OBJ_DATA*) vo;
  int i;
  AFFECT_DATA *paf;
  AFFECT_DATA *pbuf = NULL;
  bool extract = FALSE;

  if (obj == NULL){
    if (IS_NULLSTR(target_name)){
      send_to_char("Identify what object in room or inventory?\n\r", ch);
      return;
    }
    else if ( (obj = get_obj_carry(ch, target_name, ch)) == NULL
	      && (obj = get_obj_list( ch, target_name, ch->in_room->contents )) == NULL){
      send_to_char("It doesn't appear to be here.\n\r", ch);
      return;
    }
  }
  if (obj->pIndexData->vnum == OBJ_VNUM_CLONE){
    extract = TRUE;
    obj = create_object( get_obj_index(obj->cost), obj->level);
  }
    sendf( ch,"Object '%s' is type %s, material %s.\n\rExtra flags: %s.\n\rWeight is %d, value is %d, level is %d.\n\r",
      obj->name, item_name(obj->item_type), obj->material, extra_bit_name( obj->extra_flags ),
      obj->weight / 10, obj->cost, obj->level);
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
      if (obj->value[1])
	sendf(ch, "Adds following to extra flags: %s\n\r",
	      flag_string( extra_flags,  obj->value[1] ) );
      if (obj->value[4])
	sendf(ch, "Adds following to weapon flags: %s\n\r",
	      flag_string( weapon_type2,  obj->value[4] ) );
      break;
    case ITEM_SCROLL:
    case ITEM_ARTIFACT:
    case ITEM_POTION:
    case ITEM_PILL:
      sendf( ch, "Level %d spells of:", obj->value[0] );
      for ( i = 1; i <= 4; i++ )
	if ( obj->value[i] >= 0 && obj->value[i] < MAX_SKILL && skill_table[obj->value[i]].name != NULL)
	  sendf(ch, " '%s'", skill_table[obj->value[i]].name);
      send_to_char( ".\n\r", ch );
      break;
    case ITEM_WAND:
    case ITEM_STAFF:
      if (skill_table[obj->value[3]].name == NULL)
	break;
      sendf( ch, "Has %d charges of level %d", obj->value[2], obj->value[0] );
	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	  sendf(ch, " '%s'.\n\r", skill_table[obj->value[3]].name);
	break;
    case ITEM_THROW:
      sendf( ch, "Has %d %s left.\n\r",obj->value[0],obj->short_descr);
      if (obj->pIndexData->new_format)
	sendf(ch,"Damage is %dd%d (average %d).\n\r",
              obj->value[1],obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2);
      else
	sendf( ch, "Damage is %d to %d (average %d).\n\r",
	       obj->value[1], obj->value[2], ( obj->value[1] + obj->value[2] ) / 2 );
      break;
    case ITEM_DRINK_CON:
      sendf(ch,"It holds %s-colored %s.\n\r",
	    liq_table[obj->value[2]].liq_color, liq_table[obj->value[2]].liq_name);
        break;
    case ITEM_CONTAINER:
      sendf(ch,"Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
	    obj->value[3], obj->value[0], cont_bit_name(obj->value[1]));
      if (obj->value[4] != 100)
	sendf(ch,"Weight multiplier: %d%%\n\r", obj->value[4]);
      break;
    case ITEM_WEAPON:
      send_to_char("Weapon type is ",ch);
      if (IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS) )
	send_to_char("two-handed ", ch);
      switch (obj->value[0])
	{
        case(WEAPON_EXOTIC) : send_to_char("exotic.\n\r",ch);       break;
        case(WEAPON_SWORD)  : send_to_char("sword.\n\r",ch);        break;
        case(WEAPON_DAGGER) : send_to_char("dagger.\n\r",ch);       break;
        case(WEAPON_SPEAR)  : send_to_char("spear.\n\r",ch); 	    break;
        case(WEAPON_STAFF)  : send_to_char("staff.\n\r",ch);  	    break;
        case(WEAPON_MACE)   : send_to_char("mace/club.\n\r",ch);    break;
        case(WEAPON_AXE)    : send_to_char("axe.\n\r",ch);          break;
        case(WEAPON_FLAIL)  : send_to_char("flail.\n\r",ch);        break;
        case(WEAPON_WHIP)   : send_to_char("whip.\n\r",ch);         break;
        case(WEAPON_POLEARM): send_to_char("polearm.\n\r",ch);      break;
        default             : send_to_char("unknown.\n\r",ch);      break;
 	}
      if (obj->pIndexData->new_format)
	sendf(ch,"Damage is %dd%d (average %d).\n\r",
              obj->value[1],obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2);
      else
	sendf( ch, "Damage is %d to %d (average %d).\n\r",
	       obj->value[1], obj->value[2], ( obj->value[1] + obj->value[2] ) / 2 );
      if ( IS_OBJ_STAT(obj, ITEM_SOCKETABLE))
	{
	  if (obj->contains){
	    act( "$p has the following affixed to it:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	  }
	  else
	    act( "Can be socketed.", ch, obj, NULL, TO_CHAR );
	}
      break;
    case ITEM_ARMOR:
      sendf( ch, "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.\n\r",
	     obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
      if ( IS_OBJ_STAT(obj, ITEM_SOCKETABLE))
	{
	  if (obj->contains){
	    act( "$p has the following affixed to it:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	  }
	    else
	      act( "Can be socketed.", ch, obj, NULL, TO_CHAR );
	}
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
      sendf(ch, "Has accuracy of: %d%%, and rate of fire: %d\n\rFlags: %s\n\r",
	    obj->value[1], obj->value[2],
	    flag_string(ranged_type, obj->value[4]));
      break;
    case ITEM_PROJECTILE:
      sendf(ch, "Is a projectile of type %s\n\r",
	    flag_string(projectile_type, obj->value[0]));
      if (obj->pIndexData->new_format)
	sendf(ch,"Damage is %dd%d (average %d).\n\r",
              obj->value[1],obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2);
      else
	sendf( ch, "Damage is %d to %d (average %d).\n\r",
	       obj->value[1], obj->value[2], ( obj->value[1] + obj->value[2] ) / 2 );
    }
    if (!obj->enchanted)
      for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
        {
	  if ( paf->location != APPLY_NONE
	       && (paf->location < APPLY_O_COND || paf->where == TO_SKILL)
	       && paf->modifier != 0 )
                sendf( ch, "Affects %s by %d.\n\r",
		       (paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location )),
		       paf->modifier );
	  if (paf->bitvector)
            {
	      switch(paf->where)
                {
                case TO_AFFECTS: sendf(ch,"Adds %s affect.\n", affect_bit_name(paf->bitvector)); break;
                case TO_OBJECT:  sendf(ch,"Adds %s object flag.\n", extra_bit_name(paf->bitvector)); break;
                case TO_IMMUNE:  sendf(ch,"Adds immunity to %s.\n", imm_bit_name(paf->bitvector)); break;
                case TO_RESIST:  sendf(ch,"Adds resistance to %s.\n\r", imm_bit_name(paf->bitvector)); break;
                case TO_VULN:    sendf(ch,"Adds vulnerability to %s.\n\r", imm_bit_name(paf->bitvector)); break;
		case TO_SKILL:   /* handled above */ break;
                default:         sendf(ch,"Unknown bit %d: %d\n\r", paf->where,paf->bitvector); break;
                }
            }
        }
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
//We store mana charge in a buffer so we can show it last.
	if ( paf->location != APPLY_NONE
	     && (paf->location < APPLY_O_COND || paf->where == TO_SKILL)
	     && paf->modifier != 0 )
	{
            sendf( ch, "Affects %s by %d",
		   (paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location )),
		   paf->modifier );
            if ( paf->duration > -1)
                sendf(ch,", %d hours.\n\r",paf->duration);
            else
                sendf(ch,".\n\r");
            if (paf->bitvector)
            {
                switch(paf->where)
                {
                case TO_AFFECTS: sendf(ch,"Adds %s affect.\n", affect_bit_name(paf->bitvector)); break;
                case TO_OBJECT:  sendf(ch,"Adds %s object flag.\n", extra_bit_name(paf->bitvector)); break;
                case TO_WEAPON:  sendf(ch,"Adds %s weapon flags.\n", weapon_bit_name(paf->bitvector)); break;
                case TO_IMMUNE:  sendf(ch,"Adds immunity to %s.\n", imm_bit_name(paf->bitvector)); break;
                case TO_RESIST:  sendf(ch,"Adds resistance to %s.\n\r", imm_bit_name(paf->bitvector)); break;
                case TO_VULN:    sendf(ch,"Adds vulnerability to %s.\n\r", imm_bit_name(paf->bitvector)); break;
		case TO_SKILL:   /* handled above */ break;
                default:         sendf(ch,"Unknown bit %d: %d\n\r", paf->where,paf->bitvector); break;
                }
            }
	}//end if not APPLY_NONE

 //MANA CHARGE MESSAGE

	if ( (pbuf = affect_find(obj->affected, gen_mana_charge)) != NULL)
	     {

	       char buf [MIL];
	       if(mcharge_info(ch, buf, obj, pbuf, FALSE))
		 send_to_char(buf, ch);
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
    if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_USE ) )
      send_to_char("You have a vauge feeling this item can be used somehow.\n\r", ch);
    if (extract)
      extract_obj(obj);
}

void spell_infravision( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_EVIL(ch) && IS_GOOD(victim))) && !IS_IMMORTAL(ch))
    {
        sendf(ch, "It would go against your beliefs to aid %s.\n\r", PERS(victim,ch));
	return;
    }
    if ( IS_AFFECTED(victim, AFF_INFRARED) )
    {
	if (victim == ch)
            act("You can already see in the dark.",ch,NULL,victim,TO_CHAR);
	else
            act("$N can already see in the dark,",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where	 = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level + 10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INFRARED;
    affect_to_char( victim, &af );
    act_new( "Your eyes glow red.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n's eyes glow red.", victim, NULL, NULL, TO_ROOM );
}

void spell_invis( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (ch->class == class_lookup("psionicist") && ch != victim)
      {
	send_to_char("You can only alter perceptions about your person.\n\r", ch);
	return;
      }
    if (((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_EVIL(ch) && IS_GOOD(victim))) && !IS_IMMORTAL(ch))
      {
        sendf(ch, "It would go against your beliefs to aid %s.\n\r", PERS(victim,ch));
	return;
      }

    if ( IS_AFFECTED(victim, AFF_INVISIBLE) )
      {
	if (victim == ch)
            act("You are already invisible.",ch,NULL,victim,TO_CHAR);
	else
            act("$N is already invisible.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( IS_AFFECTED(victim, AFF_FAERIE_FOG) || IS_AFFECTED(victim, AFF_FAERIE_FIRE))
    {
	if (victim == ch)
            act("You can't become invisible while glowing.",ch,NULL,victim,TO_CHAR);
	else
            act("$N is glowing right now.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level + 12;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INVISIBLE;
    affect_to_char( victim, &af );

    act_new( "You fade out of existence.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n fades out of existence.", victim, NULL, NULL, TO_ROOM );
}

void spell_know_alignment(int sn,int level,CHAR_DATA *ch,void *vo,int target )
{
  CHAR_DATA *victim;

  if (IS_NULLSTR(target_name)){
    send_to_char("Whom do you wish to target?\n\r", ch);
    return;
  }
  else if ( (victim = get_char_world(ch,target_name )) == NULL){
    send_to_char("They aren't here.\n\r", ch );
    return;
  }
  else if (IS_NPC(victim)){
    send_to_char("Why would you wish to know mob's religion?\n\r", ch);
    return;
  }

  sendf( ch, "%s is a loyal believer in way of %s and powers of %s.",
	 PERS(victim, ch),
	 deity_table[victim->pcdata->way].way,
	 path_table[deity_table[victim->pcdata->way].path].name);


}

void spell_lightning_bolt(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] =
    {
	 0,
	 0,  0,  0,  0,  0,	 0,  0,  0, 25, 28,
	31, 34, 37, 40, 45,	50, 52, 52, 53, 54,
	54, 55, 56, 56, 57,	58, 58, 59, 60, 60,
	60, 60, 60, 60, 60,	60, 60, 60, 60, 60,
	60, 60, 60, 60, 60,	65, 65, 65, 65, 65
    };
    int dam;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim,DAM_LIGHTNING,skill_table[sn].spell_type) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_LIGHTNING ,TRUE);
}

void spell_locate_object( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    char buf[MSL];
    BUFFER *buffer;
    OBJ_DATA *obj, *in_obj;
    bool found = FALSE;
    int number = 0, max_found = IS_IMMORTAL(ch) ? 200 : 2 * level, chance = number_percent();
    buffer = new_buf();
    for ( obj = object_list; obj != NULL; obj = obj->next )
      {
	if ( !can_see_obj( ch, obj )
	     || (obj->carried_by && !can_see(ch, obj->carried_by))
	     || !is_name( target_name, obj->name )
	     || IS_OBJ_STAT(obj,ITEM_NOLOCATE)
	     || UMIN(chance - 2*(get_curr_stat(ch,STAT_LUCK)-17),100) > (2 * level)
	     || (obj->carried_by != NULL && IS_IMMORTAL(obj->carried_by) && !IS_IMMORTAL(ch))
	     || ch->level < obj->level
	     || ( obj->in_room != NULL && (obj->in_room->vnum == ROOM_VNUM_LIMIT || obj->in_room->vnum == ROOM_VNUM_BOUNTY))
	     || ( obj->in_room != NULL && obj->in_room->vnum <= 150))
	  continue;
	found = TRUE;
        number++;

	for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj );
        {
	  if ( in_obj->carried_by != NULL && IS_IMMORTAL(in_obj->carried_by) && !IS_IMMORTAL(ch))
	    continue;

	  if ( in_obj->carried_by != NULL && can_see(ch,in_obj->carried_by))
	    {
	      if (in_obj->carried_by->desc && in_obj->carried_by->desc->connected != CON_PLAYING)
		continue;
	      sprintf( buf, "%s is carried by %s\n\r", obj->short_descr,PERS(in_obj->carried_by, ch) );
	    }
	  else
	    {
	      if (IS_IMMORTAL(ch) && in_obj->in_room != NULL)
		sprintf( buf, "%s is in %s [Room %d]\n\r", obj->short_descr,in_obj->in_room->name, in_obj->in_room->vnum);
	      else
                    sprintf( buf, "%s is in %s\n\r", obj->short_descr,in_obj->in_room == NULL ? "somewhere" : in_obj->in_room->name );
	    }
	  buf[0] = UPPER(buf[0]);
	  add_buf(buffer,buf);
	  if (number >= max_found)
	    break;
        }
    }
    if ( !found )
      send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
    else
      page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void spell_magic_missile( int sn, int level, CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] =
    {
	 0,
	 3,  3,  4,  4,  5,	 6,  6,  6,  6,  6,
	 8, 10, 18, 20, 22,	25,  28, 28, 29, 30,
	 31, 32, 33, 34, 35,	36,  37, 38, 39, 40,
	 40, 40, 40, 40, 40,	40,  40, 40, 40, 40,
	 40, 40, 40, 40, 40,	40,  40, 40, 40, 40
    };
    int dam, i;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    for(i = UMAX(1,level/12); i > 0 && victim != NULL; i--)
    {
        dam = number_range( 2 * dam_each[level] / 3, dam_each[level] );
        if (victim != NULL && (ch->in_room != victim->in_room || is_ghost(victim,600)))
            break;
        if ( saves_spell( level, victim,DAM_ENERGY,skill_table[sn].spell_type) )
	    dam /= 2;
        damage( ch, victim, dam, sn, DAM_ENERGY ,TRUE);
    }
}

void spell_mass_healing(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *gch;
    int heal_num = skill_lookup("heal"), refresh_num = skill_lookup("refresh");
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
      if ( gch->pCabal && IS_CABAL(get_parent(gch->pCabal), CABAL_NOMAGIC))
	continue;
      if (is_same_group(ch, gch) && !IS_IMMORTAL(gch))
	{
	  spell_heal(heal_num,level,ch,(void *) gch,TARGET_CHAR);
	    spell_refresh(refresh_num,level,ch,(void *) gch,TARGET_CHAR);
	}
    }
}

void spell_mass_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *gch;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
      if (ch->pCabal && IS_SET(ch->pCabal->progress, PROGRESS_CHALL))
	continue;
	if ( !is_same_group( gch, ch ) || IS_AFFECTED(gch, AFF_INVISIBLE) )
	    continue;
	else if ( gch->pCabal && IS_CABAL(get_parent(gch->pCabal), CABAL_NOMAGIC))
	  continue;
        if ( IS_AFFECTED(gch, AFF_FAERIE_FOG) || IS_AFFECTED(gch, AFF_FAERIE_FIRE))
            continue;
	act( "$n slowly fades out of existence.", gch, NULL, NULL, TO_ROOM );
	act_new( "You slowly fade out of existence.", gch, NULL, NULL, TO_CHAR, POS_DEAD );
	af.where     = TO_AFFECTS;
	af.type      = sn;
    	af.level     = level/2;
	af.duration  = 24;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_INVISIBLE;
	affect_to_char( gch, &af );
    }
}

void spell_null( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    send_to_char( "That's not a spell!\n\r", ch );
}

void spell_pass_door( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED(victim, AFF_PASS_DOOR) )
    {
	if (victim == ch)
            act("You are already out of phase.",ch,NULL,victim,TO_CHAR);
	else
            act("$N is already shifted out of phase.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level / 4 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PASS_DOOR;
    affect_to_char( victim, &af );
    act_new( "You phase out of existence.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n phases out of existence.", victim, NULL, NULL, TO_ROOM );
}

void spell_plague( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    AFFECT_DATA* paf;

    const int skill = get_skill(ch, sn);
    int dur = 0;

    if ( is_affected( victim, sn ) )
    {
	act("$N is already plagued.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (check_immune(ch, DAM_DISEASE, TRUE) == IS_VULNERABLE
	&& number_percent() < 10){
      send_to_char("You feel your immune system give in.\n\r", ch);
      victim = ch;
    }

    if (ch == victim || saves_spell(level,victim,DAM_DISEASE,skill_table[sn].spell_type)
	|| (IS_NPC(victim) && IS_SET(victim->act,ACT_UNDEAD)))
    {
        act_new("You feel momentarily ill, but it passes.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	if (ch != victim)
	  act("$N seems to be unaffected.",ch,NULL,victim,TO_CHAR);
	return;
    }
/* skill calcs etc. all done only if the spell is cast by a char with skill */
    if (skill > 0){
      if (!IS_NPC(ch) && ch->class != class_lookup("shaman")
	  && ch->class != class_lookup("necromancer"))
	level -= level/3;

      dur = level / 10 + (skill > 77? 1: 0) + (skill > 84? 1: 0) + (skill > 88? 1: 0)
	+ (skill > 92? 2: 0) + (skill > 95? 2: 0) + + (skill > 99? 3: 0);
    if (ch->class == class_lookup("shaman"))
      dur +=3;
    }
    else
      dur = level / 4;
    af.where     = TO_AFFECTS;
    af.type 	 = sn;
    af.level	 = level;
    af.duration  = dur;
    af.location  = APPLY_STR;
    af.modifier  = -(level/10);
    af.bitvector = AFF_PLAGUE;
    affect_to_char(victim,&af);

    /* now damage counter */
    af.where     = TO_NONE;
    af.type 	 = sn;
    af.level	 = level;
    af.location  = APPLY_NONE;
    if (ch->class == class_lookup("shaman"))
      af.modifier  = 5;
    else
      af.modifier  = 0;
    af.bitvector = 0;
    paf = affect_to_char(victim,&af);
    if (!IS_NPC(ch))
      string_to_affect(paf, ch->name);

    act_new("You scream in agony as plague sores erupt from your skin.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n screams in agony as plague sores erupt from $s skin.",victim,NULL,NULL,TO_ROOM);
}

void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    AFFECT_DATA* paf;
    int skill = get_skill(ch, sn);
    int dur = 0;
    bool fAdv = get_skill(ch, skill_lookup("lotus scourge")) > 1;

    if (is_affected(victim,sn) )
    {
	act("$N is already poisoned.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( ch == victim || saves_spell( level, victim,DAM_POISON,skill_table[sn].spell_type) )
    {
	act_new("You feel momentarily ill, but it passes.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	if (ch != victim)
	    act("$n turns slightly green, but it passes.",victim,NULL,NULL,TO_ROOM);
	return;
    }
    if (skill > 0){
      if (!IS_NPC(ch) && !fAdv && ch->class != class_lookup("shaman") && ch->class != class_lookup("necromancer"))
	level -= level/3;

      dur = level / 10 + (skill > 80? 1: 0) + (skill > 84? 1: 0)
	+ (skill > 88? 1: 0) + (skill > 95? 1: 0) + (skill > 99? 1: 0);
    }
    else
      dur = level / 6;
    if (fAdv)
      dur = UMAX(dur, 6);
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = dur;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = AFF_POISON;
    paf = affect_to_char( victim, &af );
    if (!IS_NPC(ch))
      string_to_affect(paf, ch->name);
    /* Set bit preventing damage till first tick. */
    if (ch->class != class_lookup("shaman"))
      SET_BIT(paf->bitvector, AFF_FLAG);

    act_new( "You feel very sick.", victim,NULL,NULL,TO_CHAR,POS_DEAD );
    act("$n looks very ill.",victim,NULL,NULL,TO_ROOM);
}

void spell_protection(int sn,int level,CHAR_DATA *ch,void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL) || IS_AFFECTED(victim, AFF_PROTECT_GOOD))
    {
        if (victim == ch)
            act("You are already protected.",ch,NULL,victim,TO_CHAR);
        else
            act("$N is already protected.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( is_affected( victim, gsn_blasphemy ) )
    {
	sendf(ch, "%s's faith have been lost.\n\r", PERS(victim,ch));
	return;
    }
    if (IS_GOOD(ch))
    {
	af.where     = TO_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  = 23;
	af.location  = APPLY_SAVING_SPELL;
	af.modifier  = -1;
	af.bitvector = AFF_PROTECT_EVIL;
	affect_to_char( victim, &af );
	act_new( "You feel holy and pure.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
	if ( ch != victim )
	    act("$N is protected from evil.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_EVIL(ch))
    {
	af.where     = TO_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  = 24;
	af.location  = APPLY_SAVING_SPELL;
	af.modifier  = -1;
	af.bitvector = AFF_PROTECT_GOOD;
	affect_to_char( victim, &af );
	act_new( "You feel aligned with darkness.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
	if ( ch != victim )
	    act("$N is protected from good.",ch,NULL,victim,TO_CHAR);
	return;
    }
    else
    {
      send_to_char( "You lack the moral commitment to recieve protection.\n\r",victim);
      return;
    }
}

void spell_protection_evil(int sn,int level,CHAR_DATA *ch,void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_NEUTRAL(victim)){
      send_to_char( "You lack the moral commitment.\n\r",victim);
      return;
    }
    if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL)
	 || IS_AFFECTED(victim, AFF_PROTECT_GOOD))
      {
        if (victim == ch)
            act("You are already protected.",ch,NULL,victim,TO_CHAR);
        else
            act("$N is already protected.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = -1;
    af.bitvector = AFF_PROTECT_EVIL;
    affect_to_char( victim, &af );
    act_new( "You feel holy and pure.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
        act("$N is protected from evil.",ch,NULL,victim,TO_CHAR);
}

void spell_protection_good(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (IS_NEUTRAL(victim)){
      send_to_char( "You lack the moral commitment.\n\r",victim);
      return;
    }
    if ( IS_AFFECTED(victim, AFF_PROTECT_GOOD)
	 || IS_AFFECTED(victim, AFF_PROTECT_EVIL))
    {
        if (victim == ch)
            act("You are already protected.",ch,NULL,victim,TO_CHAR);
        else
            act("$N is already protected.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = -1;
    af.bitvector = AFF_PROTECT_GOOD;
    affect_to_char( victim, &af );
    act_new( "You feel aligned with darkness.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
        act("$N is protected from good.",ch,NULL,victim,TO_CHAR);
}

void spell_ray_of_truth (int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if (IS_EVIL(ch) )
    {
        victim = ch;
        send_to_char("The energy explodes inside you!\n\r",ch);
    }
    if (victim != ch)
    {
      act("A blinding ray of magic shots from $n's hands and envelops $N.", ch, NULL, victim, TO_NOTVICT);
      act("As the ray of $g's power shoots forth,  $N cries out in remorse.", ch, NULL, victim, TO_CHAR);
      send_to_char("Your life flashes before your eyes and you are crushed by the error of your ways.\n\r", victim);
    }
    if (IS_NPC(victim))
      dam = dice(UMIN(100, victim->level * 2), 3);
    else
      dam = number_range(0, 30)
	+ 2 * level
	+ UMIN(10, victim->pcdata->kpc) *  3
	+ UMIN(3, victim->pcdata->flagged) *  10;

    if (!IS_GOOD(ch))
	dam /= 2;
    if ( saves_spell( level, victim, DAM_LIGHT, skill_table[sn].spell_type) )
        dam /= 2;
    else
      {
	level  = UMIN( 0, level );
	switch (number_range(1, 3))
	  {
	  case 1: spell_blindness(gsn_blindness,level, ch, (void *) victim,TARGET_CHAR);
	    break;
	  case 2: spell_shrink(skill_lookup("shrink"),level, ch, (void *) victim,TARGET_CHAR);
	    break;
	  case 3: spell_silence(skill_lookup("silence"),level - 3, ch, (void *) victim,TARGET_CHAR);
	    break;
	  }
      }
    damage( ch, victim, dam, sn, DAM_ENERGY ,TRUE);
}


void spell_path_of_deceit (int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if (IS_GOOD(ch) )
    {
        victim = ch;
        send_to_char("The energy explodes inside you!\n\r",ch);
    }
    if (victim != ch)
    {
      act("You call onto $g's power and envelop $N in the very stuff of evil.", ch, NULL, victim, TO_CHAR);
      act("A sinister dark aura envelops $N and $E falls to $S knees.", ch, NULL, victim, TO_NOTVICT);
      send_to_char("The vile taint of evil begins to suffocate your soul.\n\r", victim);
    }
    if (IS_NPC(ch))
      dam = dice(UMIN(100, victim->level * 2), 3);
    else
      dam = number_range(0, 30)	+ (2 * level) + (UMIN(10, ch->pcdata->kpc) *  3) + (UMIN(3, ch->pcdata->flagged) *  10);

    if (!IS_EVIL(ch))
	dam /= 3;
    if ( saves_spell( level, victim,DAM_NEGATIVE,skill_table[sn].spell_type) )
        dam /= 2;
    else
      {
	level  = UMIN(50, level);
	switch (number_range(1, 3))
	  {
	  case 1: spell_poison(gsn_poison,level, ch, (void *) victim,TARGET_CHAR);
	    break;
	  case 2: spell_plague(gsn_plague,level, ch, (void *) victim,TARGET_CHAR);
	    break;
	  case 3: spell_blasphemy(gsn_blasphemy,level, ch, (void *) victim,TARGET_CHAR);
	    break;
	  }
      }
    damage( ch, victim, dam, sn, DAM_NEGATIVE ,TRUE);
}



void spell_refresh( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    victim->move = UMIN( victim->move + level, victim->max_move );
    if (victim->max_move == victim->move)
        send_to_char("You feel fully refreshed!\n\r",victim);
    else
        send_to_char( "You feel less tired.\n\r", victim );
    if ( ch != victim )
        sendf(ch, "You refresh %s.\n\r", PERS(victim,ch) );
}

/* syntax for this spell is: <object> or <victim> <object> */
void spell_remove_curse( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;


    //mobs can cure curse on players
    if (IS_NPC(ch) && (victim = get_char_room(ch, NULL, target_name)) != NULL){
      if (!IS_AFFECTED(victim, AFF_CURSE)){
	act("But $N is not cursed!", ch, NULL, victim, TO_CHAR);
	return;
      }
      if (check_dispel(level, victim, gsn_curse)){
	act("$n glows blue.", victim, NULL, victim, TO_ROOM);
	act("You glow blue.", victim, NULL, victim, TO_CHAR);
      }
      else{
	act("The curse on $N has withstood your power.",ch, NULL, victim,TO_CHAR);
      }
      return;
    }


    /* check if the first argument matches an object we are carrying */
    if ( (obj = get_obj_carry(ch, target_name, ch )) == NULL){
      obj = get_obj_wear(ch, target_name, ch );
    }

    /* now we either work on the object, or handle another character */
    if (obj != NULL){
      if (IS_OBJ_STAT(obj,ITEM_NODROP)
	  || IS_OBJ_STAT(obj,ITEM_NOREMOVE)
	  || is_affected_obj(obj, gsn_curse_weapon)){
	if (!IS_OBJ_STAT(obj,ITEM_NOUNCURSE) && !saves_dispel(level + 2, obj->level, 0)){
	  REMOVE_BIT(obj->extra_flags,ITEM_NODROP);
	  REMOVE_BIT(obj->extra_flags,ITEM_NOREMOVE);
	  affect_strip_obj(obj, gsn_curse_weapon);
	  act("$p glows blue.",ch,obj,NULL,TO_ALL);
	  return;
	}
	act("The curse on $p has withstood your power.",ch,obj,NULL,TO_CHAR);
	return;
      }
      act("There doesn't seem to be a curse on $p.",ch,obj,NULL,TO_CHAR);
      return;
    }
    /* no object of such name, we handle the victim now */
    else{
      char name[MIL];
      target_name = one_argument( target_name, name );

      if ( (victim = get_char_room( ch, NULL, name )) == NULL){
	send_to_char("There is no such item or person here.\n\r", ch);
	return;
      }
      else if (IS_NULLSTR( target_name )){
	sendf(ch, "Remove curse from what item on %s?\n\r", PERS( victim, ch ));
	return;
      }
      else if (!IS_NPC(ch)
	  && victim->master != ch
	  && victim != ch
	  && !is_same_group(victim, ch)
	  && !IS_IMMORTAL(ch)){
	send_to_char("They are not following you.\n\r", ch );
	return;
      }
      /* the the object now like we did before */
      else if ( (obj = get_obj_carry(victim, target_name, ch )) == NULL
		&& (obj = get_obj_wear(victim, target_name, ch )) == NULL){
	sendf( ch, "%s does not seem to be carrying or using that.\n\r", PERS( victim, ch ));
	return;
      }
      /* now we either work on the object, or handle another character */
      if (IS_OBJ_STAT(obj,ITEM_NODROP)
	  || IS_OBJ_STAT(obj,ITEM_NOREMOVE)
	  || is_affected_obj(obj, gsn_curse_weapon)){
	if (!IS_OBJ_STAT(obj,ITEM_NOUNCURSE) && !saves_dispel(level + 2, obj->level, 0)){
	  REMOVE_BIT(obj->extra_flags,ITEM_NODROP);
	  REMOVE_BIT(obj->extra_flags,ITEM_NOREMOVE);
	  affect_strip_obj(obj, gsn_curse_weapon);
	  act("$p glows blue.",ch,obj,NULL,TO_ALL);
	  return;
	}
	act("The curse on $p has withstood your power.",ch,obj,NULL,TO_CHAR);
	return;
      }
      act("There doesn't seem to be a curse on $p.",ch,obj,NULL,TO_CHAR);
      return;
    }
}

void spell_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dur = 1;
    if (ch->class == gcn_adventurer && ch != victim){
      send_to_char("You cannot cast this spell on another.\n\r", ch);
      return;
    }
    if (((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_EVIL(ch) && IS_GOOD(victim))) && !IS_IMMORTAL(ch))
    {
        sendf(ch, "It would go against your beliefs to aid %s.\n\r", PERS(victim,ch));
	return;
    }
    if (is_affected(victim, gsn_pyramid_of_force)){
      send_to_char("Strangly nothing happens...\n\r", ch);
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
    /* duration, goods get a bonus */
    if (IS_GOOD(ch)){
      dur = (level > 30 ? 1 : 0) + number_fuzzy( level / 5);
    }
    else
      dur = number_fuzzy( level / 6);

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = dur;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SANCTUARY;
    affect_to_char( victim, &af );
    act_new( "You are surrounded by a white aura.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n is surrounded by a white aura.", victim, NULL, NULL, TO_ROOM );
}

void spell_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_EVIL(ch) && IS_GOOD(victim))) && !IS_IMMORTAL(ch))
    {
        sendf(ch, "It would go against your beliefs to aid %s.\n\r", PERS(victim,ch));
	return;
    }
    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
            act("You are already shielded from harm.",ch,NULL,victim,TO_CHAR);
	else
            act("$N is already protected by a shield.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24 * UMAX(1, level / 25);
    af.location  = APPLY_AC;
    af.modifier  = -20;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You are surrounded by a force shield.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n is surrounded by a force shield.", victim, NULL, NULL, TO_ROOM );
}

void spell_shocking_grasp(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const int dam_each[] =
    {
	 0,
	 2,  4,  5,  7, 12,	15, 20, 25, 29, 33,
	36, 39, 39, 39, 40,	40, 41, 41, 42, 42,
	43, 43, 44, 44, 45,	45, 46, 46, 47, 47,
	48, 48, 49, 49, 50,	50, 51, 51, 52, 52,
	53, 53, 54, 54, 55,	55, 56, 56, 57, 57
    };
    int dam;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level] / 2, 3 * dam_each[level] /2 );
    if ( saves_spell( level, victim,DAM_LIGHTNING,skill_table[sn].spell_type) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_LIGHTNING ,TRUE);
}

void spell_sleep( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED(victim, AFF_SLEEP) )
    {
        act( "$N is already asleep.", ch, NULL, victim, TO_CHAR);
	return;
    }
    set_delay(ch, victim);

    if (is_affected(victim, gen_watchtower)){
      sendf( ch, "%s is shielded within the Watchtower.\n\r", PERS(victim, ch ));
      return;
    }
    if ( (IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG) )
	 || IS_UNDEAD(victim)
	 || saves_spell( level, victim,DAM_CHARM,skill_table[sn].spell_type))
    {
        act( "You fail to cause $N to fall asleep.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (is_affected(victim, gsn_horse) && monk_good(victim, WEAR_HEAD) && !IS_NPC(victim) && !IS_IMMORTAL(ch))
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
    af.duration  = number_fuzzy( level / 15) +1;
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

void spell_slow( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    bool fFriend = FALSE;

    /* quick check for in combat */
    if (IS_NULLSTR(target_name) && ch->fighting)
      victim = ch->fighting;
    fFriend = is_same_group(ch, victim);

    if (!fFriend){
      /* checks for pk etc */
      if (is_safe(ch,victim) && victim != ch){
	sendf(ch, "Not on %s.\n\r",PERS(victim,ch));
	return;
      }
      if (is_safe(ch, victim))
	return;
      /* these are the checks for yelling and attacking */
      if (victim->fighting != ch
	  && ch->fighting != victim
	  && !IS_NPC(ch)
	  && IS_AWAKE(victim))
	m_yell(ch, victim, TRUE);

      /* check for start of combat */
      if (IS_AWAKE(victim)
	  && (victim->fighting == NULL || ch->fighting == NULL)
	  && !is_affected(victim,gsn_ecstacy)
	  && !is_ghost(victim,600))
	{
	  set_fighting( ch, victim );
	  set_fighting( victim, ch );
	  multi_hit( victim, ch, TYPE_UNDEFINED );
	}
/* and saves check */
      if (saves_spell( level, victim,DAM_MENTAL,skill_table[sn].spell_type)){
	send_to_char("You failed.\n\r", ch);
	return;
      }
    }
    if ( is_affected( victim, sn ) || IS_AFFECTED(victim,AFF_SLOW))
    {
        if (victim == ch)
            act("You can't move any slower!",ch,NULL,victim,TO_CHAR);
        else
            act("$N can't get any slower than that.",ch,NULL,victim,TO_CHAR);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/4;
    af.location  = APPLY_DEX;
    af.modifier  = -1 - (level >= 18) - (level >= 25) - (level >= 32);
    af.bitvector = AFF_SLOW;
    affect_to_char( victim, &af );
    act_new( "You feel yourself moving more slowly.", victim,NULL,NULL,TO_CHAR,POS_DEAD );
    act("$n starts to move in slow motion.",victim,NULL,NULL,TO_ROOM);
}

void spell_stone_skin( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( ch, sn ) )
    {
	if (victim == ch)
            act("Your skin is already as hard as a rock.",ch,NULL,victim,TO_CHAR);
	else
            act("$N is already as hard as $E can be.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.location  = APPLY_AC;
    af.modifier  = -40;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "Your skin turns to stone.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n's skin turns to stone.", victim, NULL, NULL, TO_ROOM );
}

void spell_summon( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim, *och, *och_next;
    if (( victim = get_char_world( ch, target_name ) ) == NULL){
      send_to_char("You cannot locate them.\n\r", ch);
      return;
    }
    if ( (victim == ch
	  || victim->in_room == NULL
	  || (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL)
	  || (IS_NPC(victim) && IS_SET(victim->act,ACT_AGGRESSIVE))
	  || (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
	  || IS_SET(ch->in_room->area->area_flags, AREA_RESTRICTED)
	  || IS_SET(victim->in_room->area->area_flags, AREA_RESTRICTED)
	  || IS_SET(victim->imm_flags,IMM_SUMMON)
	  || IS_SET(ch->in_room->room_flags, ROOM_SAFE)
	  || IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	  || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	  || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	  || IS_SET(victim->in_room->room_flags, ROOM_NO_INOUT)
	  || IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)
	  || IS_SET(victim->in_room->room_flags, ROOM_NO_SUMMONOUT)
	  || IS_SET(ch->in_room->room_flags, ROOM_NO_SUMMONIN)
	  || victim->level > level + 5
	  || victim->fighting != NULL
	  || (!IS_NPC(victim)
	      && IS_SET(victim->act,PLR_NOSUMMON)
	      && !is_pk(ch,victim))
	  || ((!IS_NPC(victim)
	       || IS_AFFECTED(victim,AFF_CHARM))
	      && victim->in_room->area != ch->in_room->area)
	  || (IS_NPC(victim) && IS_AFFECTED(victim,AFF_CHARM)
	      && victim->master != NULL
	      && victim->in_room == victim->master->in_room &&
	      !is_pk(ch,victim->master))
	  || ((ch->in_room->exit[0] == NULL
	       || IS_SET((ch->in_room->exit[0])->exit_info, EX_CLOSED))
	      && (ch->in_room->exit[1] == NULL
		  || IS_SET((ch->in_room->exit[1])->exit_info, EX_CLOSED))
	      && (ch->in_room->exit[2] == NULL
		  || IS_SET((ch->in_room->exit[2])->exit_info, EX_CLOSED))
	      && (ch->in_room->exit[3] == NULL
		  || IS_SET((ch->in_room->exit[3])->exit_info, EX_CLOSED))
	      && (ch->in_room->exit[4] == NULL
		  || IS_SET((ch->in_room->exit[4])->exit_info, EX_CLOSED))
	      && (ch->in_room->exit[5] == NULL
		  || IS_SET((ch->in_room->exit[5])->exit_info, EX_CLOSED)))
	  || is_affected(victim,gsn_coffin)
	  || is_affected(victim,gen_watchtower)
	  || is_affected(victim,gsn_entomb)
	  || is_affected(victim, gsn_tele_lock)
	  || saves_spell( IS_SET(victim->vuln_flags,VULN_SUMMON) ? level + 3 : level - 3 ,
			  victim,DAM_OTHER,skill_table[sn].spell_type))
	 && !IS_IMMORTAL(ch))
      {
        act( "You failed the summon.", ch, NULL, NULL, TO_CHAR );
	return;
      }
    if (number_percent() < get_skill(victim,gsn_mind_link))
    {
	for ( och = ch->in_room->people; och != NULL; och = och_next )
    	{
    	    och_next = och->next_in_room;
    	    if ( IS_AFFECTED(och, AFF_CHARM) && IS_NPC(och) && och->master == ch && och->pIndexData->vnum == MOB_VNUM_DISPLACER)
	    {
    	        victim = och;
		break;
    	    }
    	}
    }
    a_yell( ch, victim );
    act( "$n disappears suddenly.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    if (!IS_NPC(victim) && victim->pcdata->pStallion!=NULL)
    {
      victim->pcdata->pStallion = NULL;
    }
    act( "$n arrives suddenly.", victim, NULL, NULL, TO_ROOM );
    act( "$n has summoned you!", ch, NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    /* CONTINENCY */
    check_contingency(victim, NULL, CONT_SUMMON);
}

void spell_teleport( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  ROOM_INDEX_DATA *pRoomIndex;
  bool fSafe = FALSE;

  const int bad_max = 5;
  int bad_num = 0;
  /* Age of Mirlan MUST BE LAST */
  char* bad_areas[5] = {"winter", "faction of steel", "faction of blood",
			"faction of magic", "Age of Mirlan"};
  int e_r1[] = {ROOM_NO_TELEPORTIN};
  int area_pool = 10;

  if (victim != ch){
    send_to_char("You cannot teleport others.\n\r", ch);
    return;
  }

  if ( victim->in_room == NULL
       || IS_SET(victim->in_room->room_flags, ROOM_NO_INOUT)
       || IS_SET(victim->in_room->room_flags, ROOM_NO_TELEPORTOUT)
       || ( victim != ch && IS_SET(victim->imm_flags,IMM_SUMMON))
       || is_affected(victim, gsn_tele_lock)
       || is_affected(victim,gen_ensnare)
       || ( !IS_NPC(ch) && victim->fighting != NULL )
       || (IS_NPC(ch) && ch->pIndexData->pShop != NULL)
       || ( victim != ch && ( saves_spell( level - 5, victim,DAM_OTHER,skill_table[sn].spell_type)))){
    if (victim == ch){
      send_to_char("You failed.\n\r",ch);
      return;
    }
  }
/* try to get a safe room if skill allows */
  if (number_percent() < (get_skill(ch, sn) - 50))
    fSafe = TRUE;
  else{
    /* exclude Age of Mirlan from higher up players */
    if (ch->level < 45)
      bad_num = bad_max;
    else
      bad_num = UMAX(0, bad_max - 1);
  }
  pRoomIndex =  get_rand_room(0,0,		//area range (0 to ignore)
			      0,0,		//room ramge (0 to ignore)
			      bad_areas,bad_num,//areas to choose from
			      NULL,0,		//areas to exclude
			      NULL,0,		//sectors required
			      NULL,0,		//sectors to exlude
			      NULL,0,		//room1 flags required
			      e_r1,1,		//room1 flags to exclude
			      NULL,0,		//room2 flags required
			      NULL,0,		//room2 flags to exclude
			      area_pool,	//number of seed areas
			      TRUE,		//exit required?
			      fSafe,		//Safe?
			      ch);		//Character for room checks

/* check if we failed getting a safe room */
  if (fSafe && pRoomIndex == NULL)
    pRoomIndex =  get_rand_room(0,0,		//area range (0 to ignore)
				0,0,		//room ramge (0 to ignore)
				bad_areas,bad_num,//areas to choose from
				NULL,0,		//areas to exclude
				NULL,0,		//sectors required
				NULL,0,		//sectors to exlude
				NULL,0,		//room1 flags required
				e_r1,1,		//room1 flags to exclude
				NULL,0, 	//room2 flags required
				NULL,0,		//room2 flags to exclude
				area_pool,	//number of seed areas
				TRUE,		//exit required?
				fSafe,		//Safe?
				ch		);//Character for room checks

    if (pRoomIndex == NULL || IS_SET(pRoomIndex->area->area_flags, AREA_NOMORTAL)
){
      send_to_char("You failed.\n\r", ch);
      return;
    }
    if (victim != ch)
      send_to_char("You have been teleported!\n\r",victim);
    act( "$n vanishes!", victim, NULL, NULL, TO_ROOM );
    /* remove ghost */
    if (!IS_NPC(ch) && ch->pcdata->ghost != (time_t)NULL){
      send_to_char("You are no longer a ghost.\n\r", ch);
      ch->pcdata->ghost = (time_t)NULL;
    }
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    act( "$n suddenly pops into existence.", victim, NULL, NULL, TO_ROOM );
    do_look( victim, "auto" );
}

void spell_ventriloquate( int sn, int level, CHAR_DATA *ch,void *vo,int target)
{
    char buf1[MSL], buf2[MSL], speaker[MIL];
    CHAR_DATA *vch;
    target_name = one_argument( target_name, speaker );
    sprintf( buf1, "%s says '`#%s``'\n\r", speaker, target_name );
    sprintf( buf2, "Someone makes %s say '`#%s``'\n\r", speaker, target_name );
    buf1[0] = UPPER(buf1[0]);
    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	if (!is_exact_name( speaker, vch->name) && IS_AWAKE(vch))
	    send_to_char( saves_spell(level,vch,DAM_OTHER,skill_table[sn].spell_type) ? buf2 : buf1, vch );
}

void spell_weaken( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ) )
    {
        if (victim == ch)
            act("You are already in a weakened state.",ch,NULL,victim,TO_CHAR);
        else
            act("$N is already in a weakened state.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (saves_spell( level, victim,DAM_MALED,skill_table[sn].spell_type) && victim != ch)
    {
        act("You fail to weaken $N.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 3;
    af.location  = APPLY_STR;
    af.modifier  = -1 * (level / 8);
    af.bitvector = AFF_WEAKEN;
    affect_to_char( victim, &af );
    act_new( "You feel your strength slip away.", victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n looks tired and weak.",victim,NULL,NULL,TO_ROOM);
}

void spell_word_of_recall( int sn, int level, CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *location;
    if (!IS_NPC(victim))
    	location = get_room_index(get_temple(victim));
    else if (victim->master != NULL && victim->master == ch)
    	location = get_room_index(get_temple(ch));
    else
	return;
    if (location == NULL)
    {
	send_to_char("You are completely lost.\n\r",victim);
	return;
    }
    if (!IS_NPC(ch) && victim->master != ch && victim != ch && !is_same_group(victim, ch) && !IS_IMMORTAL(ch))
    {
	sendf(ch, "But %s isn't following you.\n\r", PERS2(victim));
    	return;
    }
    if (IS_SET(victim->in_room->room_flags,ROOM_NO_RECALL)
	|| IS_SET(victim->in_room->room_flags,ROOM_NO_INOUT)
	|| IS_AFFECTED(victim,AFF_CURSE)
	|| (!IS_NPC(victim) && (IS_SET(victim->act,PLR_DOOF) ))
	|| is_exiled(victim, location->area->vnum)
	|| is_affected(victim,gsn_damnation) )
    {
        send_to_char( "The Gods have forsaken you.\n\r", victim );
        return;
    }
    if (is_affected(victim, gsn_tele_lock)){
      send_to_char("Something seems to be blocking the spell.\n\r", ch);
      return;
    }
    else if (is_affected(victim,gen_ensnare)){
      send_to_char("You sense a powerful magical field preventing your departure.\n\r", ch);
      return;
    }
    if (victim->fighting != NULL)
    {
        if ( IS_NPC(victim->fighting)
	       && !IS_SET(victim->fighting->act2, ACT_LAWFUL)
	       && !IS_SET(victim->fighting->act, ACT_RAIDER)
	       && !IS_SET(victim->fighting->off_flags,CABAL_GUARD)){
	  if (!IS_NPC(victim))
	    victim->fighting->hunting = victim;
	  else if (victim->master != NULL)
	    victim->fighting->hunting = victim->master;
	  else
	    victim->fighting->hunting = victim;
	  victim->fighting->hunttime = 0;
        }
	stop_fighting(victim,TRUE);
    }
    victim->move /= 2;
    if (!IS_NPC(victim) && victim->pcdata->pStallion != NULL )
      {
        victim->pcdata->pStallion = NULL;
      }
    if ( IS_SET(victim->act,PLR_OUTCAST)){
      AFFECT_DATA af;
      int exp_dr = -1 *  number_range(5 * level, 20 * level);

      act("$g exacts a heavy toll on your mind.", victim, NULL, NULL, TO_CHAR);

      if (victim->level < 50){
	sendf(victim, "You lose %d experience!\n\r", exp_dr);
	gain_exp(victim, exp_dr);
      }

      af.type = sn;
      af.level = level;
      af.duration = 6;
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_HIT_GAIN;
      af.modifier = -10;
      affect_join(victim, &af);
    }
    /* lag the victim if he is bloody */
    if (!IS_NPC(victim) && is_fight_delay(victim, 180)){
      WAIT_STATE(victim, PULSE_TICK / 2 );
    }
    act("$n disappears.",victim,NULL,NULL,TO_ROOM);
    char_from_room(victim);
    char_to_room(victim,location);
    act("$n appears in the room.",victim,NULL,NULL,TO_ROOM);
    do_look(victim,"auto");
}

void spell_acid_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam, hpch;
    act("$n spits acid at $N.",ch,NULL,victim,TO_NOTVICT);
    act("$n spits a stream of corrosive acid at you.",ch,NULL,victim,TO_VICT);
    act("You spit acid at $N.",ch,NULL,victim,TO_CHAR);
    hpch = UMAX(10,ch->hit);
    dam = number_range( hpch/16+1, hpch/8 );
    if (saves_spell(level,victim,DAM_ACID,skill_table[sn].spell_type))
    {
	acid_effect(victim,level/2,dam/4,TARGET_CHAR);
        damage(ch,victim,dam/2,sn,DAM_ACID,TRUE);
    }
    else
    {
	acid_effect(victim,level,dam,TARGET_CHAR);
        damage(ch,victim,dam,sn,DAM_ACID,TRUE);
    }
}

void spell_death_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *vch, *vch_next;
    int dam, hpch;

    act("$n breathes forth a blast of black fire.",ch,NULL, NULL,TO_ROOM);
    act("You breath forth a blast of black fire.",ch,NULL,NULL,TO_CHAR);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next){
      vch_next = vch->next_in_room;

      if ( vch == ch || is_area_safe(ch,vch))
	continue;
      hpch = UMAX( 10, ch->hit );
      dam = number_range( hpch/16+1, hpch/8 );

      if (saves_spell(level,vch,DAM_NEGATIVE,skill_table[sn].spell_type))
	{
	  paralyze_effect(vch,level/2,dam/4,TARGET_CHAR);
	  damage(ch,vch,dam/2,sn,DAM_NEGATIVE,TRUE);
	}
      else
	{
	  paralyze_effect(vch,level,dam,TARGET_CHAR);
	  damage(ch,vch,dam,sn,DAM_NEGATIVE,TRUE);
	}
    }
}

void spell_fire_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam, hpch;
    act("$n breathes forth a cone of fire.",ch,NULL,victim,TO_NOTVICT);
    act("$n breathes a cone of hot fire over you!",ch,NULL,victim,TO_VICT);
    act("You breath forth a cone of fire.",ch,NULL,NULL,TO_CHAR);

    hpch = UMAX( 10, ch->hit );
    dam = number_range( hpch/16+1, hpch/8 );

    if (saves_spell(level,victim,DAM_FIRE,skill_table[sn].spell_type))
    {
	fire_effect(victim,level/2,dam/4,TARGET_CHAR);
        damage(ch,victim,dam/2,sn,DAM_FIRE,TRUE);
    }
    else
    {
	fire_effect(victim,level,dam,TARGET_CHAR);
        damage(ch,victim,dam,sn,DAM_FIRE,TRUE);
    }
}

void spell_frost_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam, hpch;
    act("$n breathes out a freezing cone of frost!",ch,NULL,victim,TO_NOTVICT);
    act("$n breathes a freezing cone of frost over you!", ch,NULL,victim,TO_VICT);
    act("You breath out a cone of frost.",ch,NULL,NULL,TO_CHAR);
    hpch = UMAX(10,ch->hit);
    dam = number_range( hpch/16+1, hpch/8 );
    if (saves_spell(level,victim,DAM_COLD,skill_table[sn].spell_type))
    {
	cold_effect(victim,level/2,dam/4,TARGET_CHAR);
        damage(ch,victim,dam/2,sn,DAM_COLD,TRUE);
    }
    else
    {
	cold_effect(victim,level,dam,TARGET_CHAR);
        damage(ch,victim,dam,sn,DAM_COLD,TRUE);
    }
}

void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *vch, *vch_next;
    int dam, hpch;
    act("$n breathes out a cloud of poisonous gas!",ch,NULL,NULL,TO_ROOM);
    act("You breath out a cloud of poisonous gas.",ch,NULL,NULL,TO_CHAR);
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
	vch_next = vch->next_in_room;
        if ( vch == ch || is_area_safe(ch,vch))
	    continue;
	hpch = UMAX( 10, ch->hit );
	dam = number_range( hpch/16+1, hpch/8 );
	if (saves_spell(level,vch,DAM_POISON,skill_table[sn].spell_type))
	{
	    poison_effect(vch,level/2,dam/4,TARGET_CHAR);
            damage(ch,vch,dam/2,sn,DAM_POISON,TRUE);
	}
	else
	{
	    poison_effect(vch,level,dam,TARGET_CHAR);
            damage(ch,vch,dam,sn,DAM_POISON,TRUE);
	}
    }
}

void spell_lightning_breath(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam, hpch;
    act("$n breathes a bolt of lightning at $N.",ch,NULL,victim,TO_NOTVICT);
    act("$n breathes a bolt of lightning at you!",ch,NULL,victim,TO_VICT);
    act("You breathe a bolt of lightning at $N.",ch,NULL,victim,TO_CHAR);
    hpch = UMAX(10,ch->hit);
    dam = number_range( hpch/16+1, hpch/8 );
    if (saves_spell(level,victim,DAM_LIGHTNING,skill_table[sn].spell_type))
    {
	shock_effect(victim,level/2,dam/4,TARGET_CHAR);
        damage(ch,victim,dam/2,sn,DAM_LIGHTNING,TRUE);
    }
    else
    {
	shock_effect(victim,level,dam,TARGET_CHAR);
        damage(ch,victim,dam,sn,DAM_LIGHTNING,TRUE);
    }
}

void spell_curse_weapon(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    int result, fail = 30, hit_bonus = 0, dam_bonus = 0, added;
    bool hit_found = FALSE, dam_found = FALSE;
    bool fCurse = FALSE;

    if (obj->item_type != ITEM_WEAPON)
    {
	send_to_char("That isn't a weapon.\n\r",ch);
	return;
    }
    if (obj->wear_loc != -1)
    {
	send_to_char("The item must be carried to be cursed.\n\r",ch);
	return;
    }
    if (!obj->enchanted)
    	for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    	{
            if ( paf->location == APPLY_HITROLL )
            {
	    	hit_bonus = paf->modifier;
		hit_found = TRUE;
	    	fail += 2 * (hit_bonus * hit_bonus);
 	    }
	    else if (paf->location == APPLY_DAMROLL )
	    {
	    	dam_bonus = paf->modifier;
		dam_found = TRUE;
	    	fail += 2 * (dam_bonus * dam_bonus);
	    }
            else
	    	fail += 25;
    	}
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location == APPLY_HITROLL )
  	{
	    hit_bonus = paf->modifier;
	    hit_found = TRUE;
	    fail += 2 * (hit_bonus * hit_bonus);
	}
	else if (paf->location == APPLY_DAMROLL )
  	{
	    dam_bonus = paf->modifier;
	    dam_found = TRUE;
	    fail += 2 * (dam_bonus * dam_bonus);
	}
        else
	    fail += 25;
    }
    fail -= level;
    if (IS_OBJ_STAT(obj,ITEM_BLESS))
	fail -= 20;
    if (IS_OBJ_STAT(obj,ITEM_GLOW))
	fail -= 10;
    fail = URANGE(5,fail,95);
    result = number_percent();
    result += 2*(get_curr_stat(ch,STAT_LUCK) - 16);
/* for mob shoppies etc */
    if (IS_NPC(ch)){
      fail = 0;
      result = 101;
    }
    if (result < (fail / 3) && !IS_IMMORTAL(ch) )
    {
        act("$p shivers violently and explodes!",ch,obj,NULL,TO_ALL);
	extract_obj(obj);
	return;
    }
    if ( result <= fail && !IS_IMMORTAL(ch) )
    {
	send_to_char("Nothing seemed to happen.\n\r",ch);
	return;
    }
    if (!obj->enchanted)
    {
	AFFECT_DATA *af_new;
	obj->enchanted = TRUE;
	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
	{
	    af_new = new_affect();
	    af_new->next = obj->affected;
	    obj->affected = af_new;
	    af_new->where	= paf->where;
	    af_new->type 	= UMAX(0,paf->type);
	    af_new->level	= paf->level;
	    af_new->duration	= paf->duration;
	    af_new->location	= paf->location;
	    af_new->modifier	= paf->modifier;
	    af_new->bitvector	= paf->bitvector;
	}
    }
    if (result <= (100 - level))
    {
        act("$p fades gray.",ch,obj,NULL,TO_ALL);
	SET_BIT(obj->extra_flags, ITEM_MAGIC);
	added = -1;
    }
    else
    {
        act("$p blackens completely!",ch,obj,NULL,TO_ALL);
	added = -2;
	fCurse = TRUE;
    }
    if (obj->level < LEVEL_HERO - 1)
	obj->level = UMIN(LEVEL_HERO - 1,obj->level + 1);
    if (dam_found)
    {
	for ( paf = obj->affected; paf != NULL; paf = paf->next)
	    if ( paf->location == APPLY_DAMROLL)
	    {
		paf->type = sn;
		paf->modifier += added;
		paf->level = UMAX(paf->level,level);
		if (paf->modifier > 4)
		    SET_BIT(obj->extra_flags,ITEM_HUM);
	    }
    }
    else
    {
	paf = new_affect();
	paf->where	= TO_OBJECT;
	paf->type	= sn;
	paf->level	= level;
	paf->duration	= -1;
	paf->location	= APPLY_DAMROLL;
	paf->modifier	=  added;
	paf->bitvector  = 0;
    	paf->next	= obj->affected;
    	obj->affected	= paf;
    }
    if (hit_found)
    {
        for ( paf = obj->affected; paf != NULL; paf = paf->next)
            if ( paf->location == APPLY_HITROLL)
            {
		paf->type = sn;
                paf->modifier += added;
                paf->level = UMAX(paf->level,level);
                if (paf->modifier > 4)
                    SET_BIT(obj->extra_flags,ITEM_HUM);
            }
    }
    else
    {
        paf = new_affect();
        paf->type       = sn;
        paf->level      = level;
        paf->duration   = -1;
        paf->location   = APPLY_HITROLL;
        paf->modifier   =  added;
        paf->bitvector  = 0;
        paf->next       = obj->affected;
        obj->affected   = paf;
    }
    if (fCurse){
      AFFECT_DATA af;
      af.type = sn;
      af.level = level;
      af.duration = -1;
      af.where = TO_OBJECT;
      af.bitvector = ITEM_NODROP;
      af.modifier = 0;
      af.location = 0;
      affect_to_obj(obj, &af);
      af.bitvector = ITEM_NOREMOVE;
      affect_to_obj(obj, &af);
      af.bitvector = ITEM_MAGIC;
      affect_to_obj(obj, &af);
    }
}

void spell_divine_retribution(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (is_affected(victim,sn))
    {
	send_to_char("You are already protected by the wrath of the gods.\n\r",ch);
	return;
    }
    if (IS_AFFECTED2(victim,AFF_BARRIER))
    {
	send_to_char("You must get rid of the barrier first.\n\r",ch);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level/6);
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You are protected by the wrath of the gods.\n\r",ch);
    act( "A globe of white light encases $n.", ch, NULL, NULL, TO_ROOM );
}

void spell_dispel_undead( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dam;
    if ( !IS_UNDEAD(victim) )
    {
	act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
	return;
    }
    act("You attempt to banish $G's power over $N.", ch, NULL, victim, TO_CHAR);
    dam = dice( 10 + level, 5 );
    if ( saves_spell( level, victim,DAM_HOLY,skill_table[sn].spell_type) )
      {
	send_to_char("You failed.\n\r", ch);
	return;
      }
    if (is_affected(victim, sn))
      {
	send_to_char("Prepared for the attack you easly fortify your power and resist.\n\r", victim);
	act("$N resists the spell!", ch, NULL, victim, TO_CHAR);
	dam /=5;
      }
    else
      {
	act("You feel $G's link to you disappear momentarly.", ch, NULL, victim, TO_VICT);
	af.type = sn;
	af.where = TO_AFFECTS;
	af.bitvector = 0;
	af.duration = number_fuzzy(2);
	af.level = level;
	af.location = APPLY_NONE;
	af.modifier = 0;
	affect_to_char(victim, &af);
      }
    damage( ch, victim, dam, sn, DAM_HOLY ,TRUE);
}

void spell_cure_mana( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal = dice(1, 8) + level / 3;
        if (ch->in_room->sector_type != SECT_FOREST)
    	{
        send_to_char("Your mana can only be healed within a forest!\n\r",ch);
        return;
    	}
    victim->mana = UMIN( victim->mana + heal, victim->max_mana );
    act_new("Harnessing natural forces, you restore a little mana.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
}



/////////////////////
//embrace_heal_check//
/////////////////////
//checks if removal of gsn_embrace_poison if successful
//Returns TRUE on success

inline bool embrace_heal_check(int sn, int level, CHAR_DATA* ch, CHAR_DATA* vic, int healed)
{
//Checks if the embrace effect was removed.
//Embrace can be removed only through healing spells, with the amount healed being
//cut in half to obtain the chance to heal, modified by rank ratio of caster and*/
//target.
//Cannot be cast on self

//EZ cases.

  if (ch == NULL)
    return FALSE;

//Check if this spell can cure this condition.
  if ( sn != skill_lookup("heal")
       && sn != gsn_heal
       && sn != skill_lookup("cure light")
       && sn != skill_lookup("cure serious")
       && sn != skill_lookup("cure critical")
       && sn != gsn_lay_on_hands )
    return FALSE;

  if ( (ch == vic) && is_affected(ch, gsn_embrace_poison))
    {
      send_to_char("Your efforts have been tainted, and the healing fails.\n\r", ch);
      return FALSE;
    }

//Lay on hands get a large bonus :)
  if (sn == gsn_lay_on_hands)
    healed = ch->level * 3;
  else if (sn == skill_lookup("heal"))
    healed = healed / 2;
  else
    {
      healed /= 4;
//chance to cure is positivly affected by rank diff. between caster and target.
      healed *=(float) ch->level / vic->level;
    }

//And we roll if success.

  if (number_percent() < healed)
//SUCCESS
    {
//Different messges for lay on hands and regular healing.
      if(sn == gsn_lay_on_hands)
	act("You purge the taint from $N's blood and soul", ch, NULL, vic, TO_CHAR);
      else
	act("The power of your healing purges the taint from $N's body and soul.", ch, NULL, vic, TO_CHAR);
 //And the single healing message.

      act("The bite wound sizzles, smokes and then quickly heals and disappears.", ch, NULL, vic, TO_ROOM);
      return TRUE;
    }
  else
    {
//FAILURE has its price.
      act("Your efforts are in vain, the foul mark remains.", ch, NULL, vic, TO_CHAR);
      act("The bite wound smokes momentarly then explodes in a fountain of puss.", ch, NULL, vic, TO_ROOM);
      damage(vic, vic, healed, gsn_embrace_poison, DAM_POISON, TRUE);
    }

  return FALSE;
}//end heal_embrace_check.

void spell_timer( int sn, int level, CHAR_DATA *ch, void *vo,int target){
  char arg[MIL];
  int dur;
  CHAR_DATA* victim;

  target_name = one_argument( target_name, arg );
  if (IS_NULLSTR(target_name))
    return;
  victim = get_char_room( ch, NULL, target_name );
  dur = atoi( arg );

  if (victim == NULL)
    return;
  else{
    AFFECT_DATA af;

    af.type = gsn_timer;
    af.level = ch->level;
    af.duration = dur;
    af.where = TO_NONE;
    af.bitvector = 0;
    af.modifier = 0;
    af.location= APPLY_NONE;
    affect_join(victim, &af);
  }
}
