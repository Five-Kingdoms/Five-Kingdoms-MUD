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

/*Written by Virigoth sometime circa april 2000 for CURSED LANDS mud.*/
/* NOT TO BE USED WITHOUT EXPLICIT PERMISSION OF AUTHOR */
/*This is the implementation of the AVATAR sub race */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "interp.h"


const struct avatar_type  avatar_info [MAX_AVATAR] = 
{
  /* Level 0, automatic +1 to all stats, glowing body */
  {0, 0, {NULL, NULL, NULL, NULL}, "You have been blessed with $g's power."  },

  /* Level 1, call: sight, infrared */
  {500, AFF_INFRARED, {"sight", NULL, NULL, NULL}, "$g's power fills your soul."  },    

  /* Level 2, call: imm to corruption */
  {1500, 0, {"sight", NULL, NULL, NULL}, "$g's spirit courses through your veins."},


  /* Level 3, automatic resotre faith, call temple */
  {2500, 0, {"sight", "temple", NULL, NULL}, "You are $n the Righteous Wrath of $g."},    

  /* Level 4, call: purify and protection*/
  {3500, AFF_PROTECT_EVIL, {"sight", "temple", "purify", NULL}, "You are $n the Holy Vengence of $g."},    

  /* Level 5, flight */
  {4500, AFF_FLYING,{"sight", "temple", "purify", NULL}, "You are $n, $g's Holy Retribution."},    

  /* Level 6, call: empower */
  {5500, 0, {"sight", "temple", "purify", "empower"}, "You are an Avatar, the Divine Messenger of $g."}
};


/* Written by: Virigoth */
/* returns: NULL*/
/* Used in: update_avatar */
/* Comments:  sets efects that update_Avatar might have removed */

void avatar_affect_check(CHAR_DATA *ch,int where,int vector)
{
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

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

/* Written by: Virigoth */
/* returns: Level of AVATAR */
/* Used in: many avatar orientated functions */
/* Comments:  uses the avatar_info table to find level. */
int get_avatar_level(CHAR_DATA* ch){
  int level = 0;

  if (!ch){
    bug("NULL character passed to: get_avatar_level",0);
    return 0;
  }

  if (IS_NPC(ch))
    return 0;

  for(level = 0; level < MAX_AVATAR; level++){
    if (ch->pcdata->divfavor < avatar_info[level].min_favor)
      return UMAX(0, level - 1);
  }

  return MAX_AVATAR - 1;
}

/* Written by: Virigoth */
/* returns: VOID */
/* Used in: avatar.c handler.c*/
/* Comments:  uses the avatar_info table to find the flags to be applied
   the boolean fRem specifies if flags should be removed instead of added. 
   NOTE:This is also used in affect_check of handler.c
*/

void update_avatar(CHAR_DATA* ch, bool fRem){
  AFFECT_DATA af;

  int level = 0;
  int cur_level = 0;

  if (!ch){
    bug("NULL character passed to: updateavatar",0);
    return;
  }
  if (IS_NPC(ch)){
    bug("NPC passed to: update_avatar", 0);
    return;
  }
  cur_level = get_avatar_level(ch);

  /* dummy affect so we can apply flags properly */
  af.type = 0;
  af.duration = -1;
  af.level = ch->level;
  af.location = APPLY_NONE;
  af.modifier = 0;

  af.where = TO_AFFECTS;
  af.bitvector = 0;

  /* we run through levels setting bits or removing them depending on the flag and level */
  for (level =0; level < MAX_AVATAR; level++){
    af.bitvector =  avatar_info[level].flags;
    if ( (fRem || level > cur_level) && af.bitvector){
      affect_modify(ch, &af, FALSE);
      /* check if we removed something that should stay. */
      avatar_affect_check(ch, af.where, af.bitvector);
    }
    else if ( (level <= cur_level) && af.bitvector)
      affect_modify(ch, &af, TRUE);
  }


}

/* Written by: Virigoth */
/* returns: returns string describing level of avatar*/
/* Used in: act_wiz.c act_info.c */
/* Comments:  uses the avatar_info table to find level/string. */
char* get_avatar_desc(CHAR_DATA* ch){

  if (!ch){
    bug("NULL character passed to: get_avatar_desc",0);
    return str_empty;
  }
  if (IS_NPC(ch)){
    bug("NPC passed to: get_avatar_desc", 0);
    return str_empty;
  }

  return avatar_info[get_avatar_level(ch)].desc;
}


/* Written by: Virigoth */
/* returns: VOID */
/* Used in: fight.c, update.c, skills5.c  (any time divinefavor is changed) */
/* Comments:  Changes divine favor total on character, showing appropriate messages */
void divfavor_gain(CHAR_DATA* ch, int gain){

  const int max_avatar = 6000;//maximum at which any gain can occur
  const int abs_max_avatar = 7500;//absolute maximum for divine favor of avatar

  if (!ch || gain == 0){
    bug("Null character or 0 gain passed to: divfavor_gain", 0);
    return;
  }

  if (IS_NPC(ch)){
    bug("NPC character passed to: divfavor_gain", 0);
    return;
  }

  /* show message (only if a significant amount is gained)*/
  if (abs(gain) > 5){
    if (gain > 0)
      act("You sense your favor with $g increase.", ch, NULL, NULL, TO_CHAR);
    else
      act("You sense your favor with $g decrease.", ch, NULL, NULL, TO_CHAR);
  }


  /* We handle each race/class using divine favor seperatly */

/* AVATAR */
  if (IS_AVATAR(ch)){
    int pre_lvl = get_avatar_level(ch);
    int post_lvl = 0;
    /* avatars only increase if not at 6500, to maximum of 7500 */
    if (ch->pcdata->divfavor <= max_avatar || gain < 0)
      ch->pcdata->divfavor = URANGE(0, ch->pcdata->divfavor + gain, abs_max_avatar);
    update_avatar(ch, FALSE);
    post_lvl = get_avatar_level(ch);
    /* show messages about levels. */
    if (post_lvl > pre_lvl)
      act("You feel $g's spirit empower your soul as you achive new level of righteous power.", ch, NULL, NULL, TO_CHAR);
    else if (post_lvl < pre_lvl)
      act("You feel a part of $g's blessing leave your soul.", ch, NULL, NULL, TO_CHAR);
  }

}


/* Written by: Virigoth */
/* returns: VOID */
/* Used in: fight.c */
/* Comments:  Handles gain/loss of divine favor on PC kill. */
void divfavor_update(CHAR_DATA* ch, CHAR_DATA* victim){
  CHAR_DATA* ava = ch;
  if (!ch || !victim){
    bug("NULL argument passed to: divfavor_update", 0);
    return;
  }

  /* NPC's dont do anything */
  if ((IS_NPC(ch) && !IS_AFFECTED(ch, AFF_CHARM) && !ch->master)
      || IS_NPC(victim))
    return;

  /* Handle each case */

/* VICTIM */
  /* AVATAR */
  if (IS_AVATAR(victim))
      divfavor_gain(victim, DIVFAVOR_LOSS);


  /* if kill was made by charmie we substitue the master */
  if (IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM)
      && ch->master)
    ava = ch->master;

/* CHARACTER */
  /* AVATAR */
  if (IS_AVATAR(ava)){
    /* penalties first */
    if (IS_GOOD(ava) && IS_GOOD(victim))
      divfavor_gain(ava, -ava->pcdata->divfavor);
    else if (IS_GOOD(ava) && IS_NEUTRAL(victim))
      divfavor_gain(ava, DIVFAVOR_LOSS);
    else if (IS_EVIL(ava) && IS_NEUTRAL(victim))
      divfavor_gain(ava, DIVFAVOR_LOSS);
    else if (IS_EVIL(ava) && IS_EVIL(victim))
      divfavor_gain(ava, -ava->pcdata->divfavor);

    /* Now benefits */
    if ( (IS_GOOD(ava) && IS_EVIL(victim))
	 || (IS_EVIL(ava) && IS_GOOD(victim)) ){
      /* Healers get 4x on undead only*/
      if (ava->class == class_lookup("healer")){
	if (IS_UNDEAD(victim))
	  divfavor_gain(ava, 4 * DIVFAVOR_GAIN);
	else
	  divfavor_gain(ava, DIVFAVOR_GAIN);
      }//END HEALER

      /* Normal avatars get 3x on demons */
      else if (IS_DEMON(victim))
	divfavor_gain(ava, 3 * DIVFAVOR_GAIN);
      else
	divfavor_gain(ava, DIVFAVOR_GAIN);
    }
   }//END AVATAR
}

/* Written by: Virigoth */
/* returns: VOID */
/* Used in: update.c */
/* Comments:  Handles special tick udpates for avatars */
void avatar_tick(CHAR_DATA* ch){
  const int level = ch->level;
  const int restore_faith = URANGE(5, 2 * (level - 20) + get_avatar_level(ch) * 5, 90);

  const int res_cost = 125;

  /* check if faith should be restored */
  if (is_affected(ch, gsn_blasphemy) && number_percent() < restore_faith){
    act("The strength of $g's spirit floods your body and your faith renews itself.", ch, NULL, NULL, TO_CHAR);
    act("An aura of $g's power briefly surrounds $n.", ch, NULL, NULL, TO_ROOM);
    divfavor_gain(ch,  -res_cost);
    affect_strip(ch, gsn_blasphemy);
  }
}
