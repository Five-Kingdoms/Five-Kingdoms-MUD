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


#define _XOPEN_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "cabal.h"
#include "recycle.h"
#include "interp.h"
#include "jail.h"
#include "bounty.h"
#include "armies.h"
#include "comm.h"
#include "clan.h"

extern void show_cabal_help( CHAR_DATA* ch, char* argument, char* output );
extern bool bounty_covered( CHAR_DATA* victim );
extern void show_tracks( CHAR_DATA* ch, ROOM_INDEX_DATA* in_room );

char *  const   where_name      [] =
{
    "<used as light>     ",
    "<worn on finger>    ",
    "<worn on finger>    ",
    "<worn around neck>  ",
    "<worn around neck>  ",
    "<worn on torso>     ",
    "<worn on head>      ",
    "<worn on face>      ",
    "<worn on legs>      ",
    "<worn on feet>      ",
    "<worn on hands>     ",
    "<worn on arms>      ",
    "<worn as shield>    ",
    "<worn about body>   ",
    "<worn about waist>  ",
    "<worn around wrist> ",
    "<worn around wrist> ",
    "<wielded>           ",
    "<held>              ",
    "<floating nearby>   ",
    "<dual wielded>      ",
    "<worn as shroud>    ",
    "<tattooed>          ",
    "<worn on ear>       ",
    "<strung on back>    ",
    "<sheathed>          ",	//sheath locations are special, these strings
    "<sheathed>          ",	//is normaly replaced by "get_sheath_string"
    "<in the quiver>     ",
};

/* How equipment is shown */
int eq_positions[] =
{
WEAR_LIGHT,
WEAR_FINGER_L,
WEAR_FINGER_R,
WEAR_NECK_1,
WEAR_NECK_2,
WEAR_HEAD,
WEAR_EARRING,
WEAR_BODY,
WEAR_ARMS,
WEAR_HANDS,
WEAR_LEGS,
WEAR_FEET,
WEAR_SHIELD,
WEAR_ABOUT,
WEAR_WAIST,
WEAR_WRIST_L,
WEAR_WRIST_R,
WEAR_WIELD,
WEAR_SECONDARY,
WEAR_SHEATH_L,
WEAR_SHEATH_R,
WEAR_HOLD,
WEAR_FACE,
WEAR_SHROUD,
WEAR_FLOAT,
WEAR_RANGED,
WEAR_QUIVER,
WEAR_TATTOO,
};

/* shows info about character details */
void show_detail_info( CHAR_DATA* ch, CHAR_DATA* victim){
  char buf[MIL];
  BUFFER *output;
  int h, d, m, y;

  if (IS_NPC(victim))
    return;

  output = new_buf();

  rltom_date( victim->pcdata->birth_date, &h, &d, &m, &y );

  sprintf( buf,
	   "Born at: %s\n\r"\
	   "Aged   : %-4d (%s-handed) %s%s%s\n\r",
	   mud_date_string( h, d, m, y ),
	   get_age( victim ),
	   victim->pcdata->lefthanded ? "Left" : "Right",
	   victim->pcdata->perk_bits ? "(" : "",
	   victim->pcdata->perk_bits ? perk_bit_name( victim->pcdata->perk_bits ) : "",
	   victim->pcdata->perk_bits ? ")" : ""
	   );
  add_buf( output, buf );
  if (victim->pcdata->fDetail){
    sprintf( buf, "Face: %-12.12s Eyes : %-12.12s Facial-Hair: %-18.18s\n\r",
	     chargen_face_table[victim->pcdata->face_type].name,
	     chargen_eyes_table[victim->pcdata->eye_color].name,
	     chargen_hairf_table[victim->pcdata->facial_hair].name
	     );
    add_buf( output, buf );
    sprintf( buf, "Hair: %-12.12s Color: %-12.12s Texture: %-12.12s\n\r",
	     chargen_hairl_table[victim->pcdata->hair_length].name,
	     chargen_hair_table[victim->pcdata->hair_color].name,
	     chargen_hairt_table[victim->pcdata->hair_texture].name
	     );
    add_buf( output, buf );
    sprintf( buf, "Body: %-12.12s Skin : %-12.12s\n\r",
	     chargen_body_table[victim->pcdata->body_type].name,
	     chargen_skin_table[victim->pcdata->skin_color].name
	     );
    add_buf( output, buf );
  }
  add_buf( output, "Description:\n\r");
  add_buf( output, victim->description);


  page_to_char( buf_string(output), ch );
  free_buf(output);
}

char *pers( CHAR_DATA *Ch, CHAR_DATA *looker )
{
  AFFECT_DATA* paf;
  CHAR_DATA* ch = Ch;
  static char buf[MSL];
  buf[0] = '\0';

  if (can_see(looker,ch)){
    if (IS_NPC(ch) && !is_affected(ch, gsn_doppelganger))
      return (ch->short_descr);

    if (IS_AFFECTED2(ch,AFF_SHADOWFORM)){
      if (IS_IMMORTAL(looker)){
	sprintf(buf,"{`8%s``} a shadow", ch->name);
	return (buf);
      }
      else
	return ("a shadow");
    }
    if (IS_AFFECTED2(ch,AFF_TREEFORM)){
      if (IS_IMMORTAL(looker)){
	sprintf(buf,"{`8%s``} A large oak tree", ch->name);
	return (buf);
      }
      else
	return ("A large oak tree");
    }
    if ( (paf = affect_find(ch->affected,gen_conceal)) != NULL){
      if (IS_IMMORTAL(looker)
	  || (paf->modifier && is_same_cabal(ch->pCabal, looker->pCabal))){
	sprintf(buf,"{`8%s``} %s", ch->name, ch->short_descr);
	return (buf);
      }
      else
	return (ch->short_descr);
    }
    if (is_affected(ch,gsn_doppelganger) && ch->doppel){
      if (IS_IMMORTAL(looker) && !IS_NPC(ch)){
	sprintf(buf,"{`8%s``} %s", ch->name, ch->doppel->name);
	return (buf);
      }
      else if (ch->doppel->doppel == NULL)
	return (pers(ch->doppel, looker));
      else
	return (ch->doppel->name);
    }
    if (is_affected(ch,gsn_bat_form)
	|| is_affected(ch,gsn_wolf_form)
	|| is_affected(ch,gsn_mist_form)){
      if (IS_IMMORTAL(looker)){
	sprintf(buf,"{`8%s``} %s", ch->name, ch->short_descr);
	return (buf);
      }
      else
	return (ch->short_descr);
    }
    if (IS_IMMORTAL(ch) && ch->short_descr[0] && str_cmp(ch->name, ch->short_descr)){
      if (IS_IMMORTAL(looker)){
	sprintf(buf,"{`8%s``} %s", ch->name, ch->short_descr);
	return (buf);
      }
      else
	return (ch->short_descr);
    }
    return (ch->name);
  }
  if (IS_IMMORTAL(ch) && !IS_NPC(ch) && ch->incog_level)
    sprintf(buf,"an Immortal");
  else
    sprintf(buf,"someone");
  return (buf);
}

char *pers2( CHAR_DATA *ch )
{
  CHAR_DATA* vch = ch;
  //static char buf[MSL];
  //buf[0] = '\0';

  if (is_affected(ch,gsn_doppelganger) && ch->doppel){
    vch = ch->doppel;
  }
  if (IS_NPC(vch))
    return (vch->short_descr);

  if (is_affected(vch,gsn_bat_form)
      || is_affected(vch,gsn_wolf_form)
      || is_affected(vch,gsn_mist_form)
      || is_affected(vch, gen_conceal) )
    return (vch->short_descr);

  if (IS_IMMORTAL(ch) && ch->short_descr[0]
      && str_cmp(ch->name, ch->short_descr))
    return (ch->short_descr);
  return (ch->name);
}

char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MSL];
    int cond = get_curr_cond(obj);
    buf[0] = '\0';
    if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
	|| (obj->description == NULL || obj->description[0] == '\0'))
      return buf;
    if ( IS_OBJ_STAT(obj, ITEM_STAIN) )		strcat( buf, "<`!STAINED``> "      );
    if (obj->item_type == ITEM_PROJECTILE)	strcat( buf, get_proj_number( obj->condition ) );
    else if (obj->carried_by && cond < 50)	strcat( buf, get_condition(cond, TRUE));
    if ( can_see_trap(ch, obj->traps) ) strcat (buf, "`!+``");
    if ( is_affected_obj(obj, gsn_there_not_there) ) strcat (buf, "(Phased) ");
    if ( IS_OBJ_STAT(obj, ITEM_INVIS) ) strcat( buf, "(Invis) "      );
    if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)  && IS_OBJ_STAT(obj, ITEM_EVIL)  ) strcat( buf, "(Red Aura) ");
    if ( IS_AFFECTED(ch, AFF_DETECT_GOOD)  && IS_OBJ_STAT(obj, ITEM_BLESS) ) strcat( buf, "(Golden Aura) ");
    if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC) && IS_OBJ_STAT(obj, ITEM_MAGIC) ) strcat( buf, "(Magical) ");
    if ( IS_OBJ_STAT(obj, ITEM_GLOW)  ) strcat( buf, "(Glowing) "    );
    if ( IS_OBJ_STAT(obj, ITEM_HUM)   ) strcat( buf, "(Humming) "    );
    if ( fShort && obj->short_descr != NULL )
      strcat( buf, obj->short_descr );
    else if ( obj->description != NULL)
      strcat( buf, obj->description );
    return buf;
}

/*void send_color_status(CHAR_DATA *ch, char *a, char *c, byte b)
{
    sprintf(c,"%s %s %s\n\r",color_table[b],a,ANSI_NORMAL);
    send_to_char(c,ch);
}*/

/* shows list of psalms */
void do_psalm(CHAR_DATA* ch, char* argument){
  char psalm_list[MAX_PSALM][MIL];
//  int max_psalms = max_psalm(ch);
  int i = 0;
  int last_psalm = 0;

  if (IS_NPC(ch))
    return;

  for (i = 0; i < MAX_PSALM; i++){
/*changed this to show baptism which is given regardless
  if (ch->pcdata->psalms[i] < 1 || ch->pcdata->psalms[i] > max_psalms){
*/
    if (ch->pcdata->psalms[i] < 1){
      continue;
    }
    last_psalm++;
    sprintf(psalm_list[ch->pcdata->psalms[i] - 1],
	    "%3d. %-22s %8s %8s\n\r",
	    ch->pcdata->psalms[i],
	    psalm_table[i].name,
	    psalm_table[i].fGroup ? "[Group]" : "",
	    psalm_table[i].fTwo ? "" : "[no-double]");
  }
  if (last_psalm == 0){
    send_to_char("You have not learned any psalms.\n\r", ch);
    return;
  }
  else
    send_to_char("Psalm of:\n\r", ch);
  for (i = 0; i < last_psalm; i++){
    send_to_char(psalm_list[i], ch);
  }
}

/* checks if the area is city, and if so reports justice laws */
void do_law(CHAR_DATA *ch, char *argument){
  act("$n seems to scan the area $s eyes searching for guards.",
      ch, NULL, NULL, TO_ROOM);
  if (IS_SET(ch->in_room->area->area_flags, AREA_LAWFUL)){
    int crime = 0;
    send_to_char("The area about you observes Law in following ways:\n\r", ch );
    for (crime = 0; crime  < MAX_CRIME; crime ++ ){
      sendf(ch, "%-15s : %s\n\r",
	    crime_table[crime].name,
	    punish_table[ch->in_room->area->crimes[crime]].name);
    }
  }
  else
    send_to_char("You scan the area but find no trace of guards.\n\r", ch);
}

void do_color(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
        send_to_char("NPC's can't see colors!\n\r",ch);
        return;
    }
    if (IS_NULLSTR(argument)){
      TOGGLE_BIT(ch->color, COLOR_ON);
      send_to_char("Syntax:  color <flag>\n\rCurrent Settings:\n\r", ch);
      sendf(ch,
	    "color\t\t%s\n\r"\
	    "damage\t\t%s\n\r",
	    IS_COLOR(ch, COLOR_ON) ? "[ON]" : "[OFF]",
	    IS_COLOR(ch, COLOR_DAMAGE) ? "[ON]" : "[OFF]");
      return;
    }
    if (!str_prefix(argument, "color")){
      do_color(ch, "");
    }
    else if (!str_prefix(argument, "damage")){
      /* we toggle color since it will be toggled back bo do_color */
      REMOVE_BIT(ch->color, COLOR_ON);
      TOGGLE_BIT(ch->color, COLOR_DAMAGE);
      do_color(ch, "");
    }
    else{
      TOGGLE_BIT(ch->color, COLOR_ON);
      do_color(ch, "");
    }
}

void list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing, bool fShowAll )
{
    char buf[MSL];
    BUFFER *output;
    int nShow = 0, iShow, count = 0;
    char *pstrShow;
    char **prgpstrShow;
    int *prgnShow;
    OBJ_DATA *obj;
    bool fCombine;
    if ( ch->desc == NULL )
	return;
    output = new_buf();

    for ( obj = list; obj != NULL; obj = obj->next_content )
      count++;

    prgpstrShow = alloc_mem( count * sizeof(char *) );
    prgnShow    = alloc_mem( count * sizeof(int)    );

    for ( obj = list; obj != NULL; obj = obj->next_content )
      if ( obj->wear_loc == WEAR_NONE &&
	   (can_see_obj( ch, obj ) || fShowAll))
	{
	  pstrShow = format_obj_to_char( obj, ch, fShort );
	  fCombine = FALSE;
	  for ( iShow = nShow - 1; iShow >= 0; iShow-- )
	    if ( !strcmp( prgpstrShow[iShow], pstrShow ) ){
	      prgnShow[iShow]++;
	      fCombine = TRUE;
	      break;
	    }
	  if ( !fCombine ){
	    prgpstrShow [nShow] = str_dup( pstrShow );
	    prgnShow    [nShow] = 1;
	    nShow++;
	  }
        }
    for ( iShow = 0; iShow < nShow; iShow++ ){
      if (prgpstrShow[iShow][0] == '\0')
	{
	  free_string(prgpstrShow[iShow]);
	  continue;
	}
      if ( prgnShow[iShow] != 1 )
        {
	  sprintf( buf, "(%2d) ", prgnShow[iShow] );
	  add_buf(output,buf);
        }
      else
	add_buf(output,"     ");
      add_buf(output,prgpstrShow[iShow]);
      add_buf(output,"\n\r");
      free_string( prgpstrShow[iShow] );
    }
    if ( fShowNothing && nShow == 0 )
      {
        send_to_char( "     Nothing.\n\r", ch );
      }
    send_to_char(buf_string(output),ch);
    free_buf(output);
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );
}

void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing ){
  list_to_char(list, ch, fShort, fShowNothing , FALSE);
}


void char_to_char( CHAR_DATA *victim, CHAR_DATA *ch , bool fAll)
{
  CHAR_DATA* vch = victim;
  char buf[MSL], buf2[MSL];
  AFFECT_DATA* paf;

  buf[0] = '\0';
  if (IS_AFFECTED2(victim, AFF_CATALEPSY))
    return;
  if (IS_NPC(victim) && IS_NULLSTR(victim->long_descr)){
    if (IS_IMMORTAL(ch) && IS_SET(ch->act,PLR_HOLYLIGHT)){
      sendf(ch, "(%s)\n\r", victim->short_descr);
    }
    return;
  }
  if (!IS_NPC(victim) && IS_IMMORTAL(victim) && victim->long_descr[0] != '\0')
    {
      if (IS_IMMORTAL(ch) && IS_SET(ch->act,PLR_HOLYLIGHT))
	{
	  sprintf( buf2, "(%s) ", victim->name);
	  strcat( buf, buf2);
	}
      strcat( buf, victim->long_descr );
      send_to_char( buf, ch );
      return;
    }
  if ( IS_SET(victim->comm,COMM_AFK     )   ) strcat( buf, "[`1AFK``] " );
  if ( IS_IMMORTAL(ch) && victim->invis_level > 1 )
    {
      sprintf( buf2, "(Wizi %d) ", victim->invis_level);
      strcat( buf,buf2);
    }
  if ( IS_IMMORTAL(ch) && victim->incog_level > 1 )
    {
	sprintf( buf2, "(Incog %d) ", victim->incog_level);
	strcat( buf,buf2);
    }

  /* Mirror cloak reverses prefixes */
  if (ch != victim
      && ch->class != gcn_psi
      && !IS_SET(ch->act, PLR_HOLYLIGHT)
      && is_affected( victim, gsn_mirror_cloak))
    vch = ch;

  if ((paf = affect_find(victim->affected, gsn_mark_prey)) != NULL
      && paf->has_string && is_name(paf->string, ch->name)) strcat( buf, "(`!Marked``) " );
  else if (check_avenger(ch, victim) == CRUSADE_MATCH) strcat( buf, "(`!Marked``) " );
  if ( IS_AFFECTED(vch, AFF_INVISIBLE)   ) strcat( buf, "(`8Invisible``) " );
  if ( is_affected(vch, gsn_burrow)   )    strcat( buf, "(`3Burrowed``) " );
  if ( IS_AFFECTED2(vch, AFF_CAMOUFLAGE) ) strcat( buf, "(`2Camouflage``) " );
  if ( IS_AFFECTED(vch, AFF_HIDE)
       || is_affected(vch, gen_d_shroud))  strcat( buf, "(`8Hide``) " );
  if ( IS_AFFECTED(vch, AFF_CHARM)       ) strcat( buf, "(`8Charmed``) " );
  if (IS_AFFECTED2(vch,AFF_SHADOWFORM))    strcat( buf, "(`5Shadowformed``) ");
  if (IS_AFFECTED2(vch,AFF_TREEFORM))      strcat( buf, "(`2Treeformed``) ");
  if (!IS_NPC(victim)
      && IS_SET(victim->act, PLR_WANTED ) )strcat(buf, "(`1WANTED``) ");
  if (!IS_NPC(victim)
      && IS_SET(victim->act, PLR_OUTCAST) )strcat(buf, "(`1OUTCAST``) ");
  if (IS_EVIL(victim)
      && IS_AFFECTED(ch, AFF_DETECT_EVIL) )strcat( buf, "(`!Red Aura``) "   );
  if (IS_GOOD(victim)
      && IS_AFFECTED(ch, AFF_DETECT_GOOD) )strcat( buf, "(`#Golden Aura``) ");
  if (!IS_NPC(victim)
      && IS_AFFECTED2(ch,AFF_SENSE_EVIL)
      && IS_EVIL(victim)
      && !IS_IMMORTAL(victim)
      && !can_see(ch,victim) ){
    buf[0] = '\0';
    strcat( buf, "(`!Red Aura``) ");
  }

  /* can be hidden by shroud */
  if ( !is_affected(vch,skill_lookup("dark shroud")) ){
    if (IS_AFFECTED(vch, AFF_PASS_DOOR))    strcat( buf, "(`^Translucent``) ");
    if (IS_AFFECTED(vch, AFF_FAERIE_FIRE))  strcat( buf, "(`9Pink Aura``) "  );
    if (IS_AFFECTED(vch, AFF_FAERIE_FOG))   strcat( buf, "(`#Yellow Aura``) ");
    if (IS_AFFECTED(vch, AFF_SANCTUARY))    strcat( buf, "(`&White Aura``) " );
    if (is_affected(vch, gsn_lifeforce))    strcat( buf, "(`2Green Aura``) " );
    if (is_affected(vch, gsn_pyramid_of_force))strcat( buf, "(`0Pyramid``) " );
    if (is_affected(vch, gsn_aura)
	|| IS_AVATAR(vch))	            strcat( buf, "(`&Bright Aura``)" );
  }
  if ( IS_NPC(victim)
       && !IS_AFFECTED2(victim, AFF_SEVERED)
       && victim->position == victim->start_pos
       && victim->long_descr[0] != '\0'
       && can_see(ch,victim) ){
    strcat( buf, victim->long_descr );
    send_to_char( buf, ch );
    return;
  }
  if( (IS_AFFECTED2(victim, AFF_SEVERED))){
    sprintf(buf,"%s's upper torso is here, twitching.\n\r",PERS2(victim));
    send_to_char(buf,ch);
    return;
  }
  else if (IS_STONED( victim )){
    sprintf(buf,"A statue of %s is here.\n\r", PERS2(victim));
    send_to_char(buf,ch);
    return;
  }

  /* were beast flags */
  if (!IS_NPC(victim) && victim->race == grn_werebeast){
    char tag[MIL];
    if (is_affected(victim, gsn_weretiger)){
      sprintf(tag,"{`8%s``} ", "Tiger");
      strcat( buf, tag );
    }
    else if (is_affected(victim, gsn_werewolf)){
      sprintf(tag,"{`8%s``} ", "Wolf");
      strcat( buf, tag );
    }
    else if (is_affected(victim, gsn_werebear)){
      sprintf(tag,"{`8%s``} ", "Bear");
      strcat( buf, tag );
    }
    else if (is_affected(victim, gsn_werefalcon)){
      sprintf(tag,"{`8%s``} ", "Falcon");
      strcat( buf, tag );
    }
  }

  if (is_affected(victim, gsn_doppelganger) && victim->doppel){
    strcat(buf, ( fAll? PERS2(victim): PERS(victim, ch )));
    if (!IS_NPC(victim->doppel) && !IS_SET(ch->comm, COMM_BRIEF)
	&& victim->doppel->short_descr[0] == '\0')
      strcat(buf,
	     (fAll ? (IS_NPC(victim) ? "" : victim->pcdata->title) : victim->doppel->pcdata->title));
  }
  else{
    strcat( buf, (fAll? PERS2(victim) : PERS( victim, ch )) );
    if ( !IS_NPC(victim)
	 && !IS_SET(ch->comm, COMM_BRIEF)
	 && victim->position == POS_STANDING
	 && !IS_AFFECTED2(victim, AFF_SHADOWFORM)
	 && !IS_AFFECTED2(victim, AFF_TREEFORM)
	 && (can_see(ch,victim) || fAll)
	 && !is_affected(victim,gsn_bat_form)
	 && !is_affected(victim,gsn_wolf_form)
	 && !is_affected(victim,gsn_mist_form)
	 && !is_affected(victim,gen_conceal) )
      strcat( buf, victim->pcdata->title );
  }
  switch ( victim->position ){
  case POS_DEAD:     strcat( buf, " is DEAD!!" );              break;
  case POS_MORTAL:   strcat( buf, " is mortally wounded." );   break;
  case POS_INCAP:    strcat( buf, " is incapacitated." );      break;
  case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
  case POS_MEDITATE:  strcat( buf, " is sitting here meditating." ); break;
  case POS_SLEEPING:
    if (is_affected(victim, gsn_camp)) strcat( buf," is camped up here.");
    else if (is_affected(victim, gsn_coffin)) strcat( buf," is lying here in a coffin.");
    else if (is_affected(victim, gsn_entomb)) strcat( buf," is sleeping in a tomb.");
    else if (is_affected(victim, gen_watchtower)) strcat( buf," is sleeping in a watchtower.");
    else strcat( buf," is sleeping here.");	break;
  case POS_RESTING:
    if (is_affected(victim, gsn_mortally_wounded)) strcat( buf, " is kneeling on the ground." );
    else if (is_affected(victim,gsn_ecstacy)) strcat( buf," is resting here with a wide grin.");
    else if (is_affected(victim, gen_watchtower)) strcat( buf," is resting in a watchtower.");
    else if (is_affected(victim, gen_acraft)) strcat( buf," is swinging a hammer here.");
    else if (is_affected(victim, gen_wcraft)) strcat( buf," is working on a weapon here.");
    else if (is_affected(victim, gen_chant)) strcat( buf," is praying here.");
    else strcat( buf, " is resting here." ); break;
  case POS_SITTING:
    if (is_affected(victim,gsn_ecstacy)) strcat( buf," is sitting here with a wide grin.");
    else if (is_affected(victim, gen_watchtower)) strcat( buf," is sitting in a watchtower.");
    else strcat( buf, " is sitting here.");	 break;
  case POS_STANDING:
    if (!IS_NPC(victim) && victim->pcdata->pStallion != NULL){
      sprintf( buf2, " is here, sitting astride %s.", victim->pcdata->pStallion->short_descr);
      strcat( buf, buf2);
    }
    else if (is_affected(victim,gsn_ecstacy)) strcat( buf," is standing here with a wide grin.");
    else if (is_affected(victim, gen_watchtower)) strcat( buf," is here in a watchtower.");
    else strcat( buf, " is here." ); break;
  case POS_FIGHTING:
    strcat( buf, " is here, fighting " );
      if ( victim->fighting == NULL ) strcat( buf, "thin air??" );
      else if ( victim->fighting == ch ) strcat( buf, "YOU!" );
      else if ( victim->in_room == victim->fighting->in_room ){
	strcat( buf,
		(fAll? PERS2(victim->fighting): PERS( victim->fighting, ch )) );
	strcat( buf, "." );
      }
      else strcat( buf, "someone who left??" ); break;
  }
  strcat( buf, "\n\r" );
  buf[0] = UPPER(buf[0]);
  send_to_char( buf, ch );
  return;
}

/* returns the sheath equipment string */
char* sheath_str(CHAR_DATA* ch, OBJ_DATA* obj, bool fLeft){
  int pos = sheath_lookup(class_table[ch->class].name);

  /* thiefs only hide daggers otherwise its regular string */
  if ((obj->item_type != ITEM_WEAPON || obj->value[0] != WEAPON_DAGGER)
      && pos != 0
      && ch->class == class_lookup("thief"))
    pos = 0;

  if (fLeft)
    return sheath_table[pos].l_str;
  else
    return sheath_table[pos].r_str;
}

/* decides if the victim's sheaths are shown */
bool show_sheath(CHAR_DATA* ch, CHAR_DATA* victim){
  int i = 0;

  if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
    return TRUE;

  if (ch == victim)
    return TRUE;

  if (get_skill(ch, gsn_peek) > 0)
    return TRUE;

  if ( (i = sheath_lookup(class_table[victim->class].name)) < 1
       || sheath_table[i].show)
    return TRUE;
  else
    return FALSE;
}

/* shows list of eq worn */
void show_equipment(CHAR_DATA* Victim, CHAR_DATA* ch, bool fMirror ){
  OBJ_DATA* obj;
  int i;
  char out[MSL];
  char buf[MIL];

/* dopple switch */
  CHAR_DATA* victim = is_affected(Victim, gsn_doppelganger) && Victim->doppel ? Victim->doppel : Victim;

  bool fBodyWep = is_affected(victim, gsn_body_weaponry);
  bool fGraft = is_affected(victim, gsn_graft_weapon);
  bool fShowSheath = show_sheath(ch, victim);
  bool fThief = victim->class == class_lookup("thief");

/* mirror switch */
  if (fMirror)
    victim = ch;
/* init buffer */
  out[0] = '\0';

  for (i = 0; i < sizeof(eq_positions)/sizeof(eq_positions[0]); i++){
    int pos = eq_positions[i];
    obj = get_eq_char(victim, pos);

    if (pos == WEAR_WIELD && fBodyWep){
      sprintf(buf , "%s hands shaped like weapons\n\r", where_name[pos]);
      strcat(out, buf);
      continue;
    }
    /* everything beyond this point needs an object */
    if (obj == NULL){
      /* show some position as nothing */
      if (ch == Victim
	  && pos != WEAR_SHEATH_L
	  && pos != WEAR_SHEATH_R
	  && pos != WEAR_WIELD
	  && pos != WEAR_SECONDARY
	  && pos != WEAR_SHIELD
	  && pos != WEAR_HOLD
	  && pos != WEAR_SHROUD
	  && pos != WEAR_TATTOO
	  && pos != WEAR_RANGED){
	sprintf( buf, "%s %s\n\r",
		 where_name[eq_positions[i]],
		 "nothing.");
	strcat(out, buf );
      }
      continue;
    }

    /* do not show hidden sheaths */
    if ( (pos == WEAR_SHEATH_L || pos == WEAR_SHEATH_L) && !fShowSheath){
      /* there is a special condition for thiefs and hidden sheathed weapons */
      if (!fThief || obj->value[0] == WEAPON_DAGGER)
	continue;
    }
    if (!can_see_obj(ch, obj)){
      if (ch == victim)
	sprintf( buf, "something.\n\r");
      else
	continue;
    }
    else if (pos == WEAR_WIELD && fGraft){
      sprintf( buf, "<grafted to hands>  %s\n\r",
	       format_obj_to_char( obj, ch, TRUE ));
    }
    else if (pos == WEAR_SHEATH_L){
      sprintf( buf, "%s %s\n\r",
	       sheath_str(victim, obj, TRUE),
	       format_obj_to_char( obj, ch, TRUE ));
    }
    else if (pos == WEAR_SHEATH_R){
      sprintf( buf, "%s %s\n\r",
	       sheath_str(victim, obj, FALSE),
	       format_obj_to_char( obj, ch, TRUE ));
    }
    else{
      sprintf( buf, "%s %s\n\r",
	       where_name[eq_positions[i]],
	       format_obj_to_char( obj, ch, TRUE ));
    }
    /* attatch the string to output */
    strcat(out, buf);
  }
  if (buf[0] != '\0'){
    sendf(ch, "\n\r%s is using:\n\r",
	  fMirror ? PERS(Victim, ch) : PERS(victim,ch) );
    send_to_char(out, ch);
  }
}

void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch ){
  char_to_char(victim, ch, FALSE );
}


void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch ){

  AFFECT_DATA* paf;
  bool fMirror = FALSE;

    CHAR_DATA *tmp_vict = ( is_affected(victim,gsn_doppelganger) && victim->doppel) ? victim->doppel : victim;
    char buf[MSL];
    int peek = 0, percent;
    if (!IS_NPC(victim) && IS_IMMORTAL(victim) && get_trust(ch) < get_trust(victim) && !IS_SET(victim->act,PLR_NOSUMMON))
    {
	send_to_char( "I don't think so.\n\r",ch);
	return;
    }
    if ( can_see( victim, ch ) )
    {
	if (ch == victim)
	    act( "$n looks at $mself.",ch,NULL,NULL,TO_ROOM);
	else
	{
	    act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
	    act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
	}
    }
    /* Mirror cloak reverses desc/eq onto the looker */
    if (ch != victim
	&& ch->class != gcn_psi
	&& !IS_SET(ch->act, PLR_HOLYLIGHT)
	&& is_affected( victim, gsn_mirror_cloak)){
      fMirror = TRUE;
    }

    if ( IS_AFFECTED2( victim, AFF_SHADOWFORM))
	send_to_char("This formless apparition seems to be alive.\n\r",ch);
    else if (is_affected(victim,gsn_bat_form))
	send_to_char("You see a large vampire bat.\n\r",ch);
    else if (IS_AFFECTED2(victim, AFF_TREEFORM))
        send_to_char("You see before you a large oak tree.\n\r",ch);
    else if (is_affected(victim,gsn_wolf_form))
	send_to_char("You see a large wolf.\n\r",ch);
    else if (is_affected(victim,gsn_mist_form))
	send_to_char("You see a cloud of mist.\n\r",ch);
    else if (is_affected(victim,gsn_weretiger))
      send_to_char("You see a fierce tiger standing upright on its hind legs.\n\r",ch);
    else if (is_affected(victim,gsn_werewolf))
      send_to_char("You see a fearsome wolf standing upright on its hind legs.\n\r",ch);
    else if (is_affected(victim,gsn_werebear))
      send_to_char("You see a massive bear standing upright on its hind legs.\n\r",ch);
    else if (is_affected(victim,gsn_werefalcon))
      send_to_char("A fierce Werefalcon spreads its wings before you.\n\r",ch);
    else if ( (paf = affect_find(victim->affected, gen_conceal)) != NULL
	      && (!paf->modifier || !is_same_cabal(ch->pCabal, victim->pCabal))
	      && !IS_IMMORTAL(ch)){
      send_to_char("The thick hooded cloak conceals any kind of detail.\n\r",ch);
      return;
    }
    else if ( !fMirror && tmp_vict->description[0] != '\0' )
      send_to_char( tmp_vict->description, ch );
    else if (fMirror && ch->description[0] != '\0' )
      send_to_char( ch->description, ch );
    else
      act( "You see nothing special about $M.", ch, NULL, tmp_vict, TO_CHAR );

    if (!IS_NPC(tmp_vict) && !IS_IMMORTAL(victim)){
      int quest_num = tmp_vict->pcdata->max_quests;
      if (!str_cmp(tmp_vict->name, mud_data.questor)){
	act("$N currently holds the title of `#Master Questor`` of -Mirlan-.", ch, NULL, tmp_vict, TO_CHAR);
      }
      else if (quest_num == 0 )
	act("$E has led a sheltered life.", ch, NULL, tmp_vict, TO_CHAR );
      else
	{
	  percent =  (100 * quest_num)  / mud_data.max_quest;
	  if (percent > 99)
	    act("$E is the ultimate explorer.", ch, NULL, tmp_vict, TO_CHAR );
	  else if (percent > 90)
	    act("$E is a master explorer.", ch, NULL, tmp_vict, TO_CHAR );
	  else if (percent > 80)
	    act("$E is an experienced explorer.", ch, NULL, tmp_vict, TO_CHAR );
	  else if (percent > 60)
	    act("$E has unlocked many secrets of the world.", ch, NULL, tmp_vict, TO_CHAR );
	  else if (percent > 40)
	    act("$E has travelled quite a lot.", ch, NULL, tmp_vict, TO_CHAR );
	  else if (percent > 20)
	    act("$E has done a fair amount of traveling.", ch, NULL, tmp_vict, TO_CHAR );
	  else
	    act("$E has just opened up $S eyes to the world.", ch, NULL, tmp_vict, TO_CHAR );
	}
    }

if ( victim->max_hit > 0 )
     percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;
    strcpy( buf, PERS(victim, ch) );
    if (percent >= 100)		strcat( buf, " is in excellent condition.\n\r");
    else if (percent >= 90)	strcat( buf, " has a few scratches.\n\r");
    else if (percent >= 75)	strcat( buf, " has some small wounds and bruises.\n\r");
    else if (percent >=  50)	strcat( buf, " has quite a few wounds.\n\r");
    else if (percent >= 30)	strcat( buf, " has some big nasty wounds and scratches.\n\r");
    else if (percent >= 15)	strcat( buf, " looks pretty hurt.\n\r");
    else if (percent >= 0 )	strcat( buf, " is in awful condition.\n\r");
    else			strcat( buf, " is bleeding to death.\n\r");
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );

    if (IS_AFFECTED2( victim, AFF_SHADOWFORM) && !IS_IMMORTAL(ch) && ch != victim)
    {
	send_to_char("\n\rYou can't see through the shadows.\n\r",ch);
	return;
    }
    if (IS_AFFECTED2( victim, AFF_TREEFORM) && !IS_IMMORTAL(ch) && ch != victim)
    {
        send_to_char("\n\rIts only a tree.\n\r",ch);
        return;
    }
    else if (is_affected(victim,gsn_bat_form) || is_affected(victim,gsn_wolf_form) || is_affected(victim,gsn_mist_form))
      return;

/* show eq */
    show_equipment(victim, ch, fMirror);

    peek = get_skill(ch,gsn_peek);
    if (peek > 0)
	peek += 2*(get_curr_stat(ch,STAT_LUCK) - 16);
    if ( victim != ch && !IS_NPC(ch) && (number_percent() < peek || IS_IMMORTAL(ch)))
    {
      if (!IS_NPC(victim) && IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
	return;
      send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
      check_improve(ch,gsn_peek,TRUE, 0);
      show_list_to_char( is_affected(victim,gsn_doppelganger) ? tmp_vict->carrying : victim->carrying, ch, TRUE, TRUE );
    }

    if (IS_AFFECTED (ch, AFF_DETECT_MAGIC) || IS_IMMORTAL(ch)) {
      if (ch->level / 5 * 10 > number_percent () || IS_IMMORTAL(ch)) {
	do_affects2 (ch, victim);
      }
    }


}

void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;
    bool found = FALSE;
    int number = 0;
    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch == ch )
	    continue;
	if ( get_trust(ch) < rch->invis_level)
	    continue;
	if ( can_see( ch, rch )
	     || (IS_AFFECTED2(ch, AFF_SENSE_EVIL)
		 && IS_EVIL(rch) && !is_affected(rch, gen_d_shroud))
	     || (check_avenger(ch, rch) == CRUSADE_MATCH
		 && !is_affected(rch, gen_d_shroud)) )
	{
	    show_char_to_char_0( rch, ch );
	    number++;
	}
	else if (!IS_UNDEAD(rch))
	  found = TRUE;
    }
    if (found && number < (ch->level / 10)){
      if (ch->race == grn_feral){
	send_to_char("You sense a hidden presence in this room.\n\r", ch);
      }
      else if (ch->class == gcn_monk
	       && is_affected(ch, gsn_sense_motion)
	       && get_skill(ch,gsn_sense_motion) > number_percent()
	       && monk_good(ch, WEAR_HEAD) ){
	send_to_char("You sense a hidden presence in this room.\n\r", ch);
      }
    }
}

bool check_blind( CHAR_DATA *ch )
{
    if (!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT))
	return TRUE;
//we check if char is blined and make sure that he cannot see using bat senses.
//Also we check if he is a bat, and can use his senses (bata can beblinded
//with silence.

    if(  (IS_AFFECTED(ch, AFF_BLIND) && !is_affected(ch, gsn_bat_form)) || (IS_AFFECTED(ch, AFF_BLIND) && bat_blind(ch)) || bat_blind(ch) )
      return FALSE;
    else
      return TRUE;
}

void do_scroll(CHAR_DATA *ch, char *argument)
{
    char arg[MIL];
    int lines;
    argument = one_argument( argument, arg );
    if (arg[0] == '\0')
    {
	if (ch->lines == 0)
	    send_to_char("You do not page long messages.\n\r",ch);
	else
	    sendf(ch, "You currently display %d lines per page.\n\r",ch->lines + 2);
	return;
    }
    if (!is_number(arg))
    {
	send_to_char("You must provide a number.\n\r",ch);
	return;
    }
    lines = atoi(arg);
    if (lines == 0)
    {
	send_to_char("Paging disabled.\n\r",ch);
	ch->lines = 0;
	return;
    }
    if (lines < 10 || lines > 100)
    {
	send_to_char("You must provide a reasonable number.\n\r",ch);
	return;
    }
    sendf(ch, "Scroll set to %d lines.\n\r", lines);
    ch->lines = lines - 2;
}

void do_socials(CHAR_DATA *ch, char *argument)
{
    int iSocial, col = 0;
    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
    {
	sendf(ch, "%-12s",social_table[iSocial].name);
	if (++col % 6 == 0)
	    send_to_char("\n\r",ch);
    }
    if ( col % 6 != 0)
	send_to_char("\n\r",ch);
}

void do_wizlist(CHAR_DATA *ch, char *argument)
{
    do_help(ch,"wizlist");
}

void do_credits(CHAR_DATA *ch, char *argument)
{
    do_help(ch,"diku");
}

void do_motd(CHAR_DATA *ch, char *argument)
{
    do_help(ch,"motd");
}

void do_imotd(CHAR_DATA *ch, char *argument)
{
    do_help(ch,"imotd");
}

void do_autolist(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    send_to_char("   action     status\n\r",ch);
    send_to_char("---------------------\n\r",ch);
    send_to_char("autoassist     ",ch);
    if (IS_SET(ch->act,PLR_AUTOASSIST)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);
    send_to_char("autoexit       ",ch);
    if (IS_SET(ch->act,PLR_AUTOEXIT)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);
    send_to_char("autogold       ",ch);
    if (IS_SET(ch->act,PLR_AUTOGOLD)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);
    send_to_char("autoloot       ",ch);
    if (IS_SET(ch->act,PLR_AUTOLOOT)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);
    send_to_char("autosac        ",ch);
    if (IS_SET(ch->act,PLR_AUTOSAC)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);
    send_to_char("autosplit      ",ch);
    if (IS_SET(ch->act,PLR_AUTOSPLIT)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);
    send_to_char("autoshow      ",ch);
    if (IS_SET(ch->comm,COMM_SHOW_AFFECTS))send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);
    send_to_char("autofire       ",ch);
    if (IS_GAME(ch, GAME_NOAUTOFIRE)) send_to_char("OFF\n\r", ch);
    else  send_to_char("ON\n\r", ch);
    send_to_char("autosex        ",ch);
    if (IS_GAME(ch, GAME_SEX)) send_to_char("ON\n\r", ch);
    else  send_to_char("OFF\n\r", ch);
    send_to_char("prompt         ",ch);
    if (IS_SET(ch->comm,COMM_PROMPT)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);
    send_to_char("autosheath     ",ch);
    if (IS_GAME(ch, GAME_ASHEATH)) send_to_char("ON\n\r", ch);
    else  send_to_char("OFF\n\r", ch);
    if (ch->class == class_lookup("warrior")
	|| ch->class == class_lookup("paladin")
	|| ch->class == gcn_blademaster
	|| ch->class == class_lookup("monk")){
      send_to_char("mercykill      ",ch);
      if (IS_GAME(ch, GAME_MERCY)) send_to_char("ON\n\r", ch);
      else  send_to_char("OFF\n\r", ch);
    }
    send_to_char("aliases        ",ch);
    if (IS_GAME(ch, GAME_NOALIAS)) send_to_char("OFF\n\r",ch);
    else send_to_char("ON\n\r",ch);
    send_to_char("experience     ",ch);
    if (IS_GAME(ch, GAME_NOEXP)) send_to_char("OFF\n\r",ch);
    else send_to_char("ON\n\r",ch);
    send_to_char("score          ",ch);
    if (!IS_GAME(ch, GAME_NEWSCORE)) send_to_char("OLD\n\r",ch);
    else send_to_char("NEW\n\r",ch);
    send_to_char("autoaim:    ",ch);
    if (IS_GAME(ch, GAME_AMOB)) send_to_char("MOB[ON] ", ch);
    else  send_to_char("MOB[OFF] ", ch);
    if (IS_GAME(ch, GAME_APC)) send_to_char("PC[ON] ", ch);
    else send_to_char("PC[OFF] ", ch);
    if (IS_GAME(ch, GAME_AOBJ)) send_to_char("OBJ[ON]\n\r", ch);
    else send_to_char("OBJ[OFF]\n\r", ch);

    if (ch->pCabal){
      send_to_char("autocabal      ",ch);
      if (IS_GAME(ch, GAME_SHOW_CABAL))send_to_char("ON\n\r", ch);
      else send_to_char("OFF\n\r", ch);
    }
    if (ch->pCabal){
      send_to_char("autoequip      ",ch);
      if (IS_GAME(ch, GAME_NOREQ_CABAL))send_to_char("OFF\n\r", ch);
      else send_to_char("ON\n\r", ch);
    }


    if (IS_SET(ch->act,PLR_NOSUMMON)) send_to_char("You can only be summoned within pk range.\n\r",ch);
    else send_to_char("You can be summoned by anyone.\n\r",ch);
    if (IS_SET(ch->act,PLR_NOFOLLOW)) send_to_char("You do not welcome followers.\n\r",ch);
    else send_to_char("You accept followers.\n\r",ch);
}

void do_autoaim(CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];

  if (IS_NPC(ch))
    return;

  if (argument[0] == '\0'){
    send_to_char("autoaim:    ",ch);
    if (IS_GAME(ch, GAME_AMOB)) send_to_char("MOB[ON] ", ch);
    else  send_to_char("MOB[OFF] ", ch);
    if (IS_GAME(ch, GAME_APC)) send_to_char("PC[ON] ", ch);
    else send_to_char("PC[OFF] ", ch);
    if (IS_GAME(ch, GAME_AOBJ)) send_to_char("OBJ[ON]\n\r", ch);
    else send_to_char("OBJ[OFF]\n\r", ch);
    send_to_char("Current Settings: (use autoaim <mob/pc/obj/all> to toggle)\n\r", ch);
    return;
  }

  argument = one_argument(argument, arg1);

  /* MOB */
  if (!str_prefix("mob", arg1)){
    if (IS_GAME(ch, GAME_AMOB))
      REMOVE_BIT(ch->game, GAME_AMOB);
    else
      SET_BIT(ch->game, GAME_AMOB);
  }//END MOB


  /* PC */
  else if (!str_prefix("pc", arg1)){
    if (IS_GAME(ch, GAME_APC))
      REMOVE_BIT(ch->game, GAME_APC);
    else
      SET_BIT(ch->game, GAME_APC);
  }//END PC

  /* OBJ */
  else if (!str_prefix("obj", arg1)){
    if (IS_GAME(ch, GAME_AOBJ))
      REMOVE_BIT(ch->game, GAME_AOBJ);
    else
      SET_BIT(ch->game, GAME_AOBJ);
  }//END OBJ

  /* ALL */
  else if (!str_prefix("all", arg1)){
    if (IS_GAME(ch, GAME_AMOB)){
      REMOVE_BIT(ch->game, GAME_AMOB);
      REMOVE_BIT(ch->game, GAME_APC);
      REMOVE_BIT(ch->game, GAME_AOBJ);
    }
    else{
      SET_BIT(ch->game, GAME_AMOB);
      SET_BIT(ch->game, GAME_APC);
      SET_BIT(ch->game, GAME_AOBJ);
    }
  }//END ALL
  do_autoaim(ch, "");
}

void do_autoassist(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (IS_SET(ch->act,PLR_AUTOASSIST))
    {
        send_to_char("Autoassist removed.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_AUTOASSIST);
    }
    else
    {
        send_to_char("You will now assist when needed.\n\r",ch);
        SET_BIT(ch->act,PLR_AUTOASSIST);
    }
}

void do_autoexit(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (IS_SET(ch->act,PLR_AUTOEXIT))
    {
        send_to_char("Exits will no longer be displayed.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_AUTOEXIT);
    }
    else
    {
        send_to_char("Exits will now be displayed.\n\r",ch);
        SET_BIT(ch->act,PLR_AUTOEXIT);
    }
}
void do_autoexp(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (IS_GAME(ch,GAME_NOEXP))
    {
      send_to_char("You will once again experience new things.\n\r",ch);
      REMOVE_BIT(ch->game, GAME_NOEXP);
    }
    else
    {
        send_to_char("You will no longer experience new things.\n\r",ch);
        SET_BIT(ch->game, GAME_NOEXP);
    }
}
void do_autoscore(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (IS_GAME(ch,GAME_NEWSCORE))
    {
      send_to_char("Switching to OLD score sheet.\n\r",ch);
      REMOVE_BIT(ch->game, GAME_NEWSCORE);
    }
    else
    {
      send_to_char("Switching to NEW score sheet.\n\r",ch);
      SET_BIT(ch->game, GAME_NEWSCORE);
    }
}
void do_autoalias(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (IS_GAME(ch,GAME_NOALIAS))
    {
      send_to_char("Aliases on.\n\r",ch);
      REMOVE_BIT(ch->game, GAME_NOALIAS);
    }
    else
    {
        send_to_char("Aliases off.\n\r",ch);
        SET_BIT(ch->game, GAME_NOALIAS);
    }
}

/* Toggles sex descriptors on whois list -Crypt */
void do_autosex(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (IS_SET(ch->game,GAME_SEX))
    {
        send_to_char("Sexes will no longer be visible on the whois list.\n\r",ch);
        REMOVE_BIT(ch->game,GAME_SEX);
    }
    else
    {
        send_to_char("Sexes will now be displayed on the whois list.\n\r",ch);
        SET_BIT(ch->game,GAME_SEX);
    }
}

/* Toggles firing of weapons in combat */
void do_autofire(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (IS_SET(ch->game,GAME_NOAUTOFIRE)){
      send_to_char("You will now fire your weapons in combat.\n\r",ch);
      REMOVE_BIT(ch->game,GAME_NOAUTOFIRE);
    }
    else{
      send_to_char("You will no longer fire your weapons in combat.\n\r",ch);
      SET_BIT(ch->game,GAME_NOAUTOFIRE);
    }
}

/* removes person from consideration for questor */
void do_forfeit(CHAR_DATA *ch, char *argument){
  char arg[MIL];

  argument = one_argument(argument, arg );

  if (IS_NULLSTR(arg)){
    send_to_char("Sytnax: forfeit questor\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "questor")){
    if (IS_GAME(ch, GAME_NOQUESTOR)){
      send_to_char("You've already made your choice.\n\r", ch);
      return;
    }
    else if (IS_NULLSTR(argument) || str_cmp(argument, "confirm")){
      send_to_char("WARNING: This will irreversibly prevent you from achiving status of Master Questor.\n\r"\
		   "Use \"forfeit questor confirm\" if you wish to continue.\n\r", ch );
      return;
    }
    send_to_char("You have decided to forfeit the status of Master Questor.\n\r", ch);
    SET_BIT(ch->game, GAME_NOQUESTOR);
    if (!str_cmp(ch->name, mud_data.questor)){
      free_string(mud_data.questor);
      mud_data.questor = str_dup( "" );
      mud_data.max_quest /= 2;
    }
    return;
  }
  else
    do_forfeit(ch, "");
}

/* toggels mercy kill for warriors with expert or master weapons */
void do_mercy(CHAR_DATA *ch, char *argument){
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->game,GAME_MERCY)){
    send_to_char("You will now fight to the DEATH.\n\r", ch);
    REMOVE_BIT(ch->game,GAME_MERCY);
    return;
  }
  if (!IS_PERK(ch, PERK_MERCY)
      && ch->class != class_lookup("warrior")
      && ch->class != class_lookup("paladin")
      && ch->class != gcn_blademaster
      && ch->class != class_lookup("monk")){
    send_to_char("Only warriors, blademasters, paladins, and monks are capable of this feat.\n\r", ch);
    return;
  }
  else if (ch->class == gcn_dk || ch->class == gcn_crusader){
    send_to_char("You lack the finesse to halt your deathblows.\n\r",ch);
    return;
  }
  else{
    send_to_char("You will now stay your deathblows when possible.\n\r", ch);
    SET_BIT(ch->game,GAME_MERCY);
  }
}

/* Viri: Toggles auto sheathing in cities*/
void do_autosheath(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->game,GAME_ASHEATH))
    {
     send_to_char("You will no longer pay attention to your weapons.\n\r", ch);
     REMOVE_BIT(ch->game,GAME_ASHEATH);
    }
  else
    {
      send_to_char("When possible, you will now sheath while entering a city.\n\r", ch);
      SET_BIT(ch->game,GAME_ASHEATH);
    }
}

void do_autocabal(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (!ch->pCabal){
      send_to_char("You are not in a cabal!\n\r", ch);
      return;
    }
    if (IS_GAME(ch, GAME_SHOW_CABAL)){
      if (!IS_CABAL(ch->pCabal, CABAL_CANHIDE)){
	send_to_char("You may not hide your affiliation.\n\r", ch);
	return;
      }
      send_to_char("Your affiliation will now be hidden.\n\r",ch);
      REMOVE_BIT(ch->game,GAME_SHOW_CABAL);
      return;
    }
    if (!IS_GAME(ch, GAME_SHOW_CABAL)){
      if (!IS_CABAL(ch->pCabal, CABAL_CANSHOW)){
	send_to_char("You may not show your affiliation.\n\r", ch);
	return;
      }
      send_to_char("Your affiliation will now be visible to all.\n\r",ch);
      SET_BIT(ch->game,GAME_SHOW_CABAL);
      return;
    }
}


void do_autoequip(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (!ch->pCabal){
      send_to_char("You are not in a cabal!\n\r", ch);
      return;
    }
    if (IS_GAME(ch, GAME_NOREQ_CABAL))
    {
      send_to_char("Your Cabal will now re-equip you upon your demise.\n\r",ch);
      REMOVE_BIT(ch->game,GAME_NOREQ_CABAL);
    }
    else
    {
      send_to_char("Your Cabal will no longer grant you help upon your demise.\n\r",ch);
      SET_BIT(ch->game,GAME_NOREQ_CABAL);
    }
}

void do_autogold(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (IS_SET(ch->act,PLR_AUTOGOLD))
    {
        send_to_char("Autogold removed.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_AUTOGOLD);
    }
    else
    {
        send_to_char("Automatic gold looting set.\n\r",ch);
        SET_BIT(ch->act,PLR_AUTOGOLD);
    }
}

void do_autoloot(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (IS_SET(ch->act,PLR_AUTOLOOT))
    {
        send_to_char("Autolooting removed.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_AUTOLOOT);
    }
    else
    {
        send_to_char("Automatic corpse looting set.\n\r",ch);
        SET_BIT(ch->act,PLR_AUTOLOOT);
    }
}

void do_autosac(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (IS_SET(ch->act,PLR_AUTOSAC))
    {
        send_to_char("Autosacrificing removed.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_AUTOSAC);
    }
    else
    {
        send_to_char("Automatic corpse sacrificing set.\n\r",ch);
        SET_BIT(ch->act,PLR_AUTOSAC);
    }
}

void do_autosplit(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;
    if (IS_SET(ch->act,PLR_AUTOSPLIT))
    {
        send_to_char("Autosplitting removed.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_AUTOSPLIT);
    }
    else
    {
        send_to_char("Automatic gold splitting set.\n\r",ch);
        SET_BIT(ch->act,PLR_AUTOSPLIT);
    }
}

void do_brief(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_BRIEF))
    {
        send_to_char("Full descriptions activated.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_BRIEF);
    }
    else
    {
        send_to_char("Short descriptions activated.\n\r",ch);
        SET_BIT(ch->comm,COMM_BRIEF);
    }
}

void do_compact(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_COMPACT))
    {
        send_to_char("Compact mode removed.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_COMPACT);
    }
    else
    {
        send_to_char("Compact mode set.\n\r",ch);
        SET_BIT(ch->comm,COMM_COMPACT);
    }
}

void do_show(CHAR_DATA *ch, char *argument)
{
    if (!IS_SET(ch->comm,COMM_SHOW_AFFECTS))
    {
        send_to_char("Affects will no longer be shown in score.\n\r",ch);
        SET_BIT(ch->comm,COMM_SHOW_AFFECTS);
    }
    else
    {
        send_to_char("Affects will now be shown in score.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_SHOW_AFFECTS);
    }
}

void do_prompt(CHAR_DATA *ch, char *argument)
{
   char buf[MSL];

   SET_BIT(ch->comm,COMM_PROMPT);
   if ( argument[0] == '\0' )
   {
     sendf(ch, "Current prompt: %s\n\r", ch->prompt );
     return;
   }
   else if (!str_cmp(argument, "off")){
     send_to_char("You will no longer see prompts.\n\r",ch);
     REMOVE_BIT(ch->comm,COMM_PROMPT);
   }
   else if( !strcmp( argument, "all" ) )
     strcpy( buf, "%n%P%C<%hhp %mm %vmv> ");
   else
     {
      if ( strlen(argument) > MIL )
	argument[MIL] = '\0';
      strcpy( buf, argument );
      smash_tilde( buf );
      if (str_suffix("%c",buf))
	strcat(buf," ");
     }
   free_string( ch->prompt );
   ch->prompt = str_dup( buf );
   sendf(ch, "Prompt set to %s\n\r", ch->prompt );
   return;
}

void do_nofollow(CHAR_DATA *ch, char *argument)
{
    if (IS_AFFECTED(ch,AFF_CHARM))
    {
        send_to_char("You can't do that right now!\n\r",ch);
        return;
    }
    if (IS_SET(ch->act,PLR_NOFOLLOW))
    {
        send_to_char("You now accept followers.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_NOFOLLOW);
    }
    else
    {
        send_to_char("You no longer accept followers.\n\r",ch);
        SET_BIT(ch->act,PLR_NOFOLLOW);

        die_follower( ch, TRUE );
	stop_follower( ch );
    }
}

void do_nosummon(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
        if (IS_SET(ch->imm_flags,IMM_SUMMON))
        {
            send_to_char("You can now be summoned by anyone.\n\r",ch);
            REMOVE_BIT(ch->imm_flags,IMM_SUMMON);
        }
        else
        {
            send_to_char("You can now only be summoned within pk range.\n\r",ch);
            SET_BIT(ch->imm_flags,IMM_SUMMON);
        }
    }
    else
    {
        if (IS_SET(ch->act,PLR_NOSUMMON))
        {
            send_to_char("You can now be summoned by anyone.\n\r",ch);
            REMOVE_BIT(ch->act,PLR_NOSUMMON);
        }
        else
        {
            send_to_char("You can now only be summoned within pk range.\n\r",ch);
            SET_BIT(ch->act,PLR_NOSUMMON);
        }
    }
}

/* Written by:	Virigoth
   returns:	NULL
   comments:	Simple function to allow players to check the are they are in.
*/
void do_skylook(CHAR_DATA* ch){
  if(  (IS_AFFECTED(ch, AFF_BLIND) && !is_affected(ch, gsn_bat_form))
       || (IS_AFFECTED(ch, AFF_BLIND) && bat_blind(ch))  || bat_blind(ch) )
    {
      send_to_char( "You can't see a thing!\n\r", ch );
      return;
    }
  if (IS_AFFECTED2(ch,AFF_TERRAIN))
    {
      send_to_char( "Darkness.. Darkness everywhere!\n\r",ch);
      return;
    }
  if (!IS_OUTSIDE(ch)){
    send_to_char("You can see little of the sky from indoors.\n\r", ch);
    return;
  }

  send_to_char("You lookup into the sky.\n\r", ch);
  act("$n looks up into the sky.", ch, NULL, NULL, TO_ROOM);
  if (mud_data.weather_info.sunlight != SUN_DARK){
    do_weather(ch, "");
  }
  sendf(ch, "Using the sky and the surroundings you decide you are in area of %s.\n\r",
	ch->in_room->area->name);
}


void do_look( CHAR_DATA *ch, char *argument )
{
    char arg1 [MIL], arg2 [MIL], arg3 [MIL];
    char *pdesc;
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    ARMY_DATA* pa;
    OBJ_DATA *obj, *tattoo;
    bool fSkip = FALSE;
    bool fAuto = !IS_NPC(ch) && IS_GAME(ch, GAME_AOBJ);
    int door, number, count;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /* Useless conditional */
    tattoo = NULL;
    if( tattoo != NULL )
        return;

    /* sku look to check the area */
    if (!str_prefix("sky", arg1)){
      do_skylook(ch);
      return;
    }
    if ( ch->desc == NULL && ( victim = get_char_room( ch, NULL, arg1 ) ) != NULL )
    {
        if ( can_see( victim, ch ) )
        {
            if (ch == victim)
                act( "$n looks at $mself.",ch,NULL,NULL,TO_ROOM);
            else
            {
                act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
                act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
            }
        }
	return;
    }

    if ( ch->position == POS_MEDITATE )
    {
	send_to_char( "You can't see anything, you're meditating!\n\r", ch );
	return;
    }
    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "You can't see anything but stars!\n\r", ch );
	return;
    }
    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }
    if ( !check_blind( ch ) ){
      send_to_char( "You can't see a thing!\n\r", ch );
      return;
    }

    tattoo = get_eq_char(ch, WEAR_TATTOO);

    if ( (!IS_NPC(ch) && !IS_SET(ch->act, PLR_HOLYLIGHT)
	  && room_is_dark( ch->in_room )
	  && !IS_AFFECTED(ch, AFF_INFRARED )
	  //	  && get_eq_char(ch, WEAR_LIGHT) == NULL
	  && !is_ghost(ch,600)))
    {
	send_to_char( "It is pitch black ... \n\r", ch );
	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    number = number_argument(arg1,arg3);
    count = 0;

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
      if (IS_AFFECTED2(ch,AFF_TERRAIN)){
	if (is_affected(ch, gsn_forest_mist)){
	  if (ch->in_room->sector_type != SECT_AIR
	      && ch->in_room->sector_type != SECT_DESERT
	      && ch->in_room->sector_type != SECT_CITY
	      && ch->in_room->sector_type != SECT_INSIDE){
	    send_to_char("Shrouded in Mist\n\r",ch);
	    send_to_char("  A forest mist obscures all sense of direction.\n\r",ch);
	    fSkip = TRUE;
	  }
	}
	else{
	  send_to_char("Surrounded by Darkness\n\r",ch);
	  send_to_char("  Thick walls of darkness veil you from the world..\n\r",ch);
	  fSkip = TRUE;
	}
      }

      if ( is_affected(ch, gsn_burrow) )
	send_to_char("You can't see anything while burrowed.\n\r",ch);
      else if (!fSkip)
        {
	  send_to_char( ch->in_room->name, ch );
	  if (IS_IMMORTAL(ch) && (IS_NPC(ch) || IS_SET(ch->act,PLR_HOLYLIGHT)))
	    sendf(ch, " [Room %d]",ch->in_room->vnum);
	  send_to_char( "\n\r", ch );

	  if ( arg1[0] == '\0' || ( !IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF) ) )
            {
	      send_to_char( "  ",ch);
	      if ((mud_data.time_info.hour >=5 && mud_data.time_info.hour <20) || !ch->in_room->description2[0])
		send_to_char( ch->in_room->description, ch );
	      else
		send_to_char( ch->in_room->description2, ch );
            }
	  if (IS_AREA(ch->in_room->area, AREA_RAID)){
	    send_to_char("Fires blaze throughout the city clouding the sky with smoke.\n\r", ch);
	  }
	  if (!IS_NULLSTR(ch->in_room->area->prefix)){
	    send_to_char(ch->in_room->area->prefix, ch);
	  }
	  send_to_char( "\n\r", ch );
	  show_tracks( ch, ch->in_room );
	  if ( IS_NPC(ch) || (!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT)) )
	    do_exits( ch, "auto" );
        }
      show_room_armies(ch, &ch->in_room->room_armies );
      show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
      show_char_to_char( ch->in_room->people, ch );



      if (IS_SET(ch->in_room->room_flags, ROOM_NEWBIES_ONLY)
	  && ch->level > 5 && !IS_IMMORTAL(ch)){
	send_to_char("\n\r`&You are no longer allowed in here.  Recalling...\n\r``", ch);
	spell_word_of_recall(skill_lookup("word of recall"), 60, ch, ch, 0);
      }
      return;
    }

    if ( !str_cmp( arg1, "i" ) || !str_cmp(arg1, "in")  || !str_cmp(arg1,"on"))
    {
      if ( arg2[0] == '\0' )
	{
	  send_to_char( "Look in what?\n\r", ch );
	  return;
        }

      if ( ( obj = get_obj_here( ch, NULL, arg2 ) ) == NULL )
	{
	  send_to_char( "You do not see that here.\n\r", ch );
	  return;
	}

      switch ( obj->item_type )
	{
	default:
	  send_to_char( "That is not a container.\n\r", ch );
	  break;
	case ITEM_DRINK_CON:
	  if ( obj->value[1] <= 0 )
	    {
	      send_to_char( "It is empty.\n\r", ch );
	      break;
	    }
	  sendf( ch, "It's %sfilled with a %s liquid.\n\r", obj->value[1] < obj->value[0] / 4 ? "less than half-" :
		 obj->value[1] < 3 * obj->value[0] / 4 ? "about half-" : "more than half-", liq_table[obj->value[2]].liq_color);
	  break;
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	  if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
	      send_to_char( "It is closed.\n\r", ch );
	      break;
	    }
	  act( "$p holds:", ch, obj, NULL, TO_CHAR );
	  show_list_to_char( obj->contains, ch, TRUE, TRUE );
	  break;
	}
      return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg1 ) ) != NULL )
    {
      OBJ_DATA* inst;
      //special handling for cabal altars
      if (IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_ALTAR){
	show_altar( ch, victim );
	return;
      }
      show_char_to_char_1( victim, ch );
      /* check for sheath for justice */
      if (victim->in_room && IS_AREA(victim->in_room->area, AREA_LAWFUL)
	  && victim->in_room->area->raid == NULL
	  && ch->pCabal
	  && (IS_CABAL(ch->pCabal, CABAL_JUSTICE) || IS_HIGHBORN(ch))
	  && !IS_WANTED(victim)
	  && ( ((inst = get_eq_char(victim, WEAR_WIELD) ) != NULL
		&& inst->item_type != ITEM_INSTRUMENT
		&& get_eq_char(victim, WEAR_SHEATH_L) == NULL)
	       || (get_eq_char(victim, WEAR_SECONDARY) != NULL
		   && get_eq_char(victim, WEAR_SHEATH_R) == NULL))
	  ){
	set_crime(victim, NULL, victim->in_room->area, CRIME_SHEATH);
	sendf( ch, "%s's weapon is DRAWN.\n\r", PERS(victim, ch));
      }
      if ( IS_NPC(victim) && HAS_TRIGGER_MOB( victim, TRIG_LOOK ) )
	p_percent_trigger( victim, NULL, NULL, ch, NULL, NULL, TRIG_LOOK );
      return;
    }
    //armies
    if ( (pa = get_army_room( ch, ch->in_room, arg1)) != NULL){
      send_to_char( pa->desc, ch );
      send_to_char( "\n\r", ch );
      return;
    }

    pdesc = get_auto_extra_descr(arg3,ch->in_room->extra_descr, fAuto);
    if (pdesc != NULL)
      if (++count == number)
	{
	  page_to_char(pdesc,ch);
	  return;
	}

    //objects on floor
    count = 0;
    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
      {
	if ( !can_see_obj( ch, obj ) )
	  continue;

	if ( (	pdesc = get_auto_extra_descr( arg3, obj->extra_descr, fAuto )) != NULL ) {
	  if (++count == number) {
	    page_to_char(pdesc,ch);
	    if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_LOOK ) )
	      p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_LOOK );
	    return;
	  }
	}
	else if ( (pdesc = get_auto_extra_descr( arg3, obj->pIndexData->extra_descr, fAuto )) != NULL ) {
	  if (++count == number) {
	    page_to_char(pdesc,ch);
	    if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_LOOK ) )
	      p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_LOOK );
	    return;
	  }
	}
	else if ( (fAuto  && is_auto_name(arg3, obj->name))
		  || is_name( arg3, obj->name )) {
	  if (++count == number) {
	    sendf(ch, "%s\n\r", obj->description);
	    if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_LOOK ) )
	      p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_LOOK );
	    return;
	  }
	}
      }//end for

    //objects in inventroy
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
      {
	if ( !can_see_obj( ch, obj ) )
	  continue;

	if ( (	pdesc = get_auto_extra_descr( arg3, obj->extra_descr, fAuto )) != NULL ) {
	  if (++count == number) {
	    page_to_char(pdesc,ch);
	    if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_LOOK ) )
	      p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_LOOK );
	    return;
	  }
	}
	else if ( (pdesc = get_auto_extra_descr( arg3, obj->pIndexData->extra_descr, fAuto )) != NULL ) {
	  if (++count == number) {
	    page_to_char(pdesc,ch);
	    if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_LOOK ) )
	      p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_LOOK );
	    return;
	  }
	}
	else if ( (fAuto && is_auto_name( arg3, obj->name )) || is_name(arg3, obj->name)){
	  if (++count == number) {
	    sendf(ch, "%s\n\r", obj->description);
	    if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_LOOK ) )
	      p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_LOOK );
	    return;
	  }
	}
      }//end FOR

    if (count > 0 && count != number)
      {
	if (count == 1)
	  sendf(ch,"You only see one %s here.\n\r",arg3);
	else
	  sendf(ch,"You only see %d of those here.\n\r",count);
	return;
      }

    if      ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
    else
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }
    if (can_see_trap(ch, ch->in_room->exit[door]->traps)){
      send_to_char("You spot a hint of something hidden!\n\r\n\r", ch);
    }
    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );
    if ( pexit->keyword != NULL && pexit->keyword[0] != '\0' && pexit->keyword[0] != ' ' )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
    }
}

void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MSL], arg[MIL];
    OBJ_DATA *obj;
    AFFECT_DATA* paf;
    int i = 0;
    int weight = 0;
    one_argument( argument, arg );

    if ( arg[0] == '\0' ){
      send_to_char( "Examine what?\n\r", ch );
      return;
    }
    else if (!str_prefix( "armies", arg)){
      examine_room_armies( ch, &ch->in_room->room_armies );
      return;
    }
    do_look( ch, arg );

    if ( ( obj = get_obj_here( ch, NULL, arg ) ) != NULL ){
      /* clone check */
      bool fExtract = FALSE;
      if (obj->pIndexData->vnum == OBJ_VNUM_CLONE){
	if (ch->class == class_lookup("thief"))
	  send_to_char("Hmm.. There is something funny about this item.\n\r\n\r", ch);
	fExtract = TRUE;
	obj = create_object( get_obj_index(obj->cost), obj->level);
      }
      //omnipotence identifies it
      if (is_affected(ch, gsn_omnipotence)){
	spell_identify(skill_lookup("identify"), ch->level, ch, obj, TARGET_OBJ);
      }

      switch ( obj->item_type ){
      default:
	break;
      case ITEM_MONEY:
	if (obj->value[0] == 0)
	  sendf(ch,"Odd...there's no coins in the pile.\n\r");
	else if (obj->value[0] == 1)
	  sendf(ch,"Wow. One gold coin.\n\r");
	else
	  sendf(ch,"There are %d gold coins in the pile.\n\r",obj->value[0]);
	break;
      case ITEM_WEAPON:
	/* get the minimum strength */
        weight = get_obj_weight_char(ch, obj);
        if ((IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
	     && (ch->size < SIZE_LARGE || obj->value[0] == WEAPON_STAFF))
	    || obj->value[0] == WEAPON_POLEARM
	    || obj->item_type == ITEM_INSTRUMENT )
	  weight = 2 * weight / 3;
	if (obj->item_type == ITEM_WEAPON
	    && get_skill(ch, gsn_2h_handling) > 0
	    && (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
		|| is_affected(ch, gsn_double_grip)) )
	  weight = 2 * weight / 3;
	for (i = 0; i < 26; i++){
	  if (weight <=	str_app[i].wield * 10)
	    break;
	}
	i = URANGE(1, i, 26);

	sendf(ch, "After a brief examination you decide it looks much like a %s%s. (%d Str)\n\r",
		IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS) ? "a two handed " : "",
	      weapon_name(obj->value[0]),
			  i );
/* MALFORM LEVEL */
	  if ( (paf = affect_find(obj->affected, gen_malform)) != NULL)
	    sendf(ch, "Judging by the aura of evil, it is malformed and at level %d.\n\r", paf->level);
/* HOLY LEVEL */
	  if ( (paf = affect_find(obj->affected, gen_hwep)) != NULL){
	    sendf(ch, "The holy aura indicates it is at %d%s circle of Consecration.\n\r",
		  paf->level,
		  paf->level == 1 ? "st." :
		  paf->level == 2 ? "nd." :
		  paf->level  == 3 ? "rd." :"th.");
	  }
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
	sendf(ch, "After a brief examination you decide its a projectile of type: %s\n\r", flag_string(projectile_type, obj->value[0]));
	break;
      case ITEM_RANGED:
	sendf(ch, "After a brief examination you decide its a ranged weapon for: %s\n\r", flag_string(projectile_type, obj->value[0]));
	break;
      case ITEM_SOCKET:
	if (IS_SOC_STAT(obj, SOCKET_ARMOR)
	    && IS_SOC_STAT(obj, SOCKET_WEAPON))
	    act("You think you could attach $p to a proper weapon or armor.",
		ch, obj, NULL, TO_CHAR);
	  else if (IS_SOC_STAT(obj, SOCKET_ARMOR))
	    act("You think you could attach $p to a proper piece of armor.",
		ch, obj, NULL, TO_CHAR);
	else if (IS_SOC_STAT(obj, SOCKET_WEAPON))
	  act("You think you could attach $p to a proper weapon.",
	      ch, obj, NULL, TO_CHAR);
	  else
	    act("You think you could attach $p to a proper weapon.",
		ch, obj, NULL, TO_CHAR);
	break;
      case ITEM_DRINK_CON:
      case ITEM_CONTAINER:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
	sprintf(buf,"in %s",argument);
	  do_look( ch, buf );
      }
/* mention of signature */
      if (IS_SET(obj->wear_flags, ITEM_HAS_OWNER)
	  && (paf = affect_find(obj->affected, gen_has_owner)) != NULL
	  && paf->has_string
	  && !IS_NULLSTR(paf->string)){
	sendf(ch, "A single signature graces its surface: \"%s\"", paf->string);
      }
      act("\n\rYou judge $P to be in $tcondition.", ch,
	  get_condition(get_curr_cond(obj), FALSE), obj, TO_CHAR);
      if (obj->pCabal || obj->race || obj->class >= 0)
	sendf(ch, "You have a feeling only a %s%s%s%s%s%scan use it.\n\r",
	      obj->race ? race_table[obj->race].name : "",
	      obj->race ? " " : "",
	      obj->class >= 0 ? class_table[obj->class].name : "",
	      obj->class >= 0 ? " ": "",
	      obj->pCabal ? obj->pCabal->name : "",
	      obj->pCabal ? " ": "");
      if (CAN_WEAR(obj, ITEM_WEAR_SHIELD)){
	/* get the minimum strength */
        weight = get_obj_weight_char(ch, obj);
	for (i = 0; i < 26; i++){
	  if (weight <=	str_app[i].wield * 12)
	    break;
	}
	i = URANGE(1, i, 25);
	sendf(ch, "You require %d Str to hold this shield.\n\r", i);
      }

      if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_USE ) )
	send_to_char("You have a vauge feeling this item can be used somehow.\n\r", ch);
      if (fExtract)
	extract_obj( obj );
    }
}

/* Written by:	Virigoth
   returns:	NULL
   comments:	Simple function to allow exits to be printed on prompt.
*/
void do_promptexit(CHAR_DATA* ch, char* str){
  EXIT_DATA *pexit;
  int door = 0;
  char buf[MIL];
  bool found = FALSE;

  sprintf(buf,"[Exits:");
  if ( !check_blind( ch ) && !is_affected(ch,gsn_bat_form)){
    sprintf(str,"[Exits: --- ]");
    return;
  }
  else if (IS_AFFECTED2(ch,AFF_TERRAIN))
    {
      if (is_affected(ch, gsn_forest_mist)){
	if (ch->in_room->sector_type != SECT_AIR
	    && ch->in_room->sector_type != SECT_DESERT
	    && ch->in_room->sector_type != SECT_CITY
	    && ch->in_room->sector_type != SECT_INSIDE){
	  sprintf(str,"[Exits: ??? ]");
	  return;
	}
      }
      else{
	sprintf(str,"[Exits: ??? ]");
	return;
      }
    }
  else
    sprintf(buf,"[Exits:");

  for ( door = 0; door <= 5; door++ )
    {
      bool paran = FALSE;
      if ( (pexit = ch->in_room->exit[door] ) == NULL
	   || pexit->to_room == NULL
	   || !can_see_room(ch,pexit->to_room)
	   || (!IS_SET(pexit->exit_info, EX_ISDOOR) && IS_SET(pexit->exit_info, EX_SECRET) && !IS_IMMORTAL( ch ))
	   || (IS_SET(pexit->exit_info, EX_CLOSED) && IS_SET(pexit->exit_info, EX_SECRET) && !IS_IMMORTAL( ch )) )
	continue;
      else{
	if (IS_SET(pexit->exit_info, EX_CLOSED))
	  paran = TRUE;
	found = TRUE;
	strcat( buf, " " );
	if (paran)
	  strcat(buf,"(");
	if (can_see_trap(ch, pexit->traps))
	  strcat(buf, "`!+``");
	strcat( buf, shrt_dir_name[door] );
	if (paran)
	  strcat(buf,")");
      }
    }
  if ( !found )
    strcat( buf,  " none");
  strcat( buf, "]" );
  sprintf( str, "%s",  buf);
}

void do_exits( CHAR_DATA *ch, char *argument )
{
    extern char * const dir_name[];
    char buf[MSL];
    EXIT_DATA *pexit;
    bool found = FALSE, fAuto = !str_cmp( argument, "auto" );
    int door;
    if ( !check_blind( ch ) && !is_affected(ch,gsn_bat_form)){
      send_to_char( "You can't see a thing!\n\r", ch );
      return;
    }
    if (IS_AFFECTED2(ch,AFF_TERRAIN))
    {
      if (is_affected(ch, gsn_forest_mist)){
	if (ch->in_room->sector_type != SECT_AIR
	    && ch->in_room->sector_type != SECT_DESERT
	    && ch->in_room->sector_type != SECT_CITY
	    && ch->in_room->sector_type != SECT_INSIDE){
	  send_to_char("You can't tell where the exits are.\n\r",ch);
	  return;
	}
      }
      else{
	  send_to_char("You can't tell where the exits are.\n\r",ch);
	  return;
      }
    }
    if (fAuto)
	sprintf(buf,"[Exits:");
    else if (IS_IMMORTAL(ch))
	sprintf(buf,"Obvious exits from room %d:\n\r",ch->in_room->vnum);
    else
	sprintf(buf,"Obvious exits:\n\r");

    for ( door = 0; door <= 5; door++ )
    {
	bool paran = FALSE;
	if ( (pexit = ch->in_room->exit[door] ) == NULL
	     || pexit->to_room == NULL
	     || !can_see_room(ch,pexit->to_room)
	     || (!IS_SET(pexit->exit_info, EX_ISDOOR) && IS_SET(pexit->exit_info, EX_SECRET) && !IS_IMMORTAL( ch ))
	     || (IS_SET(pexit->exit_info, EX_CLOSED) && IS_SET(pexit->exit_info, EX_SECRET) && !IS_IMMORTAL( ch )) )
	  continue;
	else{
	  if (IS_SET(pexit->exit_info, EX_CLOSED))
	    paran = TRUE;
	  found = TRUE;
	  if ( fAuto ){
	    strcat( buf, " " );
	    if (paran)
	      strcat(buf,"(");
	    if (can_see_trap(ch, pexit->traps))
	      strcat(buf, "`!+``");
	    strcat( buf, dir_name[door] );
	    if (paran)
	      strcat(buf,")");
	  }
	  else{
	    if (paran)
	      sprintf( buf + strlen(buf), "%s(%-5s) - %s",
		       can_see_trap(ch, pexit->traps) ? "`!+``" : "",
		       capitalize( dir_name[door] ),
		       room_is_dark(ch->in_room) ? "Too dark to tell." : pexit->to_room->name);
	    else
	      sprintf( buf + strlen(buf), " %s%-5s  - %s",
		       can_see_trap(ch, pexit->traps) ? "`!+``" : "",
		       capitalize( dir_name[door] ),
		       room_is_dark(ch->in_room) ? "Too dark to tell." : pexit->to_room->name);
	    if (IS_IMMORTAL(ch))
	      sprintf(buf + strlen(buf), " (room %d)\n\r",pexit->to_room->vnum);
	    else
	      sprintf(buf + strlen(buf), "\n\r");
	  }
	}
    }
    if ( !found )
      strcat( buf, fAuto ? " none" : "None.\n\r" );
    if ( fAuto )
      strcat( buf, "]\n\r" );
    send_to_char( buf, ch );
}

void do_worth( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch))
    {
        sendf(ch,"You have %ld gold.\n\r",ch->gold);
	return;
    }
    sendf(ch, "You have %ld gold, and %ld experience (%ld exp to level).\n\r", ch->gold, ch->exp, total_exp(ch) - ch->exp);
    if ( !IS_NPC(ch) && ch->pCabal)
      sendf(ch, "You currently possess %d %s%s for your service to %s.\n\r",
	    GET_CP(ch),
	    ch->pCabal->currency,
	    GET_CP(ch) == 1 ? "" : "s",
	    ch->pCabal->who_name);

    if (ch->pCabal){
      float rate = cp_calculate_timegain(ch);
      if (rate == 0){
	sendf(ch, "You currently are not gaining any %ss.%s\n\t",
	      ch->pCabal->currency,
	      is_affected(ch, gsn_stance) ? " [STANCE]" : "");
	return;
      }
	/* calculate ticks per cp */
      rate /= CPTS;

      /* check if we gain more then 1 cp / tick */
      if (rate < 1){
	rate = 1 / rate;
	sendf(ch, "You currently need %.1f hours for one %s.\n\r",
	      rate,
	      ch->pCabal->currency);
      }
      else
	sendf(ch, "You currently gain %.1f %s%s per hour.\n\r",
	      rate,
	      ch->pCabal->currency,
	      (rate != 1? "s" : ""));
    }//END DISPLAY CP RATE

}

char* get_stat_string( CHAR_DATA* ch, int stat ){
  static char output[32];
  char* stat_str, *color;

  output[0] = 0;

  if (ch == NULL)
    return output;

  switch( stat ){
  case STAT_STR:	stat_str = "Str";	break;
  case STAT_INT:	stat_str = "Int";	break;
  case STAT_WIS:	stat_str = "Wis";	break;
  case STAT_DEX:	stat_str = "Dex";	break;
  case STAT_CON:	stat_str = "Con";	break;
  case STAT_LUCK:	stat_str = "Lck";	break;
  default: sprintf(output, "%s13.13", "???"); return output;
  }

  if (stat != STAT_LUCK ){
    char over_string[3];
    int abs_stat =  get_abs_stat( ch, stat );
    int cur_stat =  get_curr_stat(ch, stat);
    int perm_stat =  UMIN(get_max_stat(ch, stat), ch->perm_stat[stat]);


    if (abs_stat > cur_stat){
      sprintf(over_string, "^%1.1d", abs_stat - cur_stat);
    }
    else
      over_string[0] = 0;

    if (abs_stat > perm_stat)
      color = "`@";
    else if (abs_stat < perm_stat)
      color = "`3";
    else
      color = "";

    sprintf( output, "%3.3s: %2.2d(%s%-2.2d%s)%-2.2s",
	     stat_str,
	     perm_stat,
	     color,
	     cur_stat,
	     abs_stat != perm_stat ? "``" : "",
	     over_string);
  }
  else{
    int cur = get_curr_stat(ch, stat);
    int max = get_max_stat(ch, stat);
    sprintf( output, "%3.3s: %s", stat_str, short_bar( cur - 3, max ) );
  }
  return output;
}

void add_score_line( BUFFER* buffer, char* str ){
  char buf[MIL];

  sprintf(buf, "| %-61.61s |\n\r", str );
  add_buf( buffer, buf );
}

/* CURRENT LAYOUT
1 +  15     +   2 +                    44                     + 1 = 63 chars
/---------------\/----------------------------------------------\
| Voytek        || The Hand of Pandemonium [MASQUE]             |
\---------------/\----------------------------------------------/
/ Str: 99(99)^9 || Level: 99            Age: 17(999h)           |
| Int: 99(99)^9 || Class: Dark-Knight   Ethos: Chaotic		|
| Wis: 99(99)^9 || Race:  Human(M)      Align: Evil		|
| Dex: 99(99)^9 || Hp: 9999/9999        Home : Miruvhor         |
| Con: 99(99)^9 || Mn: 9999/9999	Exp: 99999/999999	|
\ Lck: [|||---] \/ Mv: 9999/9999        ToHit: -99  ToDam: -99	/
/---------------/\------------------/\--------------------------\
|    ARMOR      ||      SAVES       || Deity : Avariel		|
| Pierce: -9999 || Spell      : -99 || Faith : Baaclance        |
| Slash : -9999 || Afflictive : -99 || Weight: 9999/9999	|
| Blunt : -9999 || Maledictive: -99 || Items : 9999/9999	|
| Magic : -9999 || Mental     : -99 || Prac  : 99    Train: 99	|
| Weapon: Normal||                  || Gold  : 9999k Cp: 9999k	|
\---------------\/------------------\/--------------------------/
*/
void new_score( CHAR_DATA* ch,  CHAR_DATA* victim, char* argument ){
  AFFECT_DATA* paf;
  BUFFER* buffer;
  char buf[MIL], buf1[MIL], buf2[MIL], buf3[MIL], buf4[MIL];
  int i, scroll;

  if (is_affected(victim,gsn_linked_mind))
    return;
  else
    buffer = new_buf();
/* LINE 1 */
  add_buf( buffer,
  "\n\r/---------------\\/----------------------------------------------\\\n\r");

/* LINE 2: name(13), title/cabal rank and cabal(44) */
  sprintf(buf1, "%-13s", victim->name );
  if (victim->pCabal){
    sprintf(buf2, "%s of %s%s",
	    get_crank(victim),
	    victim->pCabal->who_name,
	    IS_MASQUE(victim) ? " [MASQUE]" : "");
    sprintf(buf3, "%-48.48s", buf2);
  }
  else{
    sprintf(buf2, "%s%s",
	    IS_NPC(victim) ? "" : victim->pcdata->title,
	    IS_MASQUE(victim) ? " [MASQUE]" : "");
    sprintf(buf3, "%-44.44s", buf2);
  }
  sprintf(buf, "| %s || %s |\n\r", buf1, buf3);
  add_buf(buffer, buf );

/* LINE 3: */
  add_buf(buffer,
  "\\---------------/\\----------------------------------------------/\n\r");

/* LINE 4: Strength (13)  Level[Sex] (20)  Age (22) */
  switch (victim->sex){
  case SEX_NEUTRAL: sprintf( buf4, "%s", "N"); break;
  case SEX_MALE: sprintf( buf4, "%s", "M"); break;
  case SEX_FEMALE: sprintf( buf4, "%s", "F"); break;
  }
  sprintf( buf1, "%s", get_stat_string(victim, STAT_STR));
  sprintf( buf2, "Level: %2d Sex: %s", victim->level, buf4 );
  sprintf( buf3, "Age  : %2d(%dh)",
	   get_age(victim),
	   ( victim->played + (int) (mud_data.current_time - victim->logon) ) / 3600);
  sprintf( buf, "| %s || %-20.20s  %-22.22s |\n\r",
	   buf1, buf2, buf3);
  add_buf( buffer, buf );

/* LINE 5: Int (13) Class (20)  Ethos (22) */
  //sex
  sprintf( buf1, "%s", get_stat_string(victim, STAT_INT));
  sprintf( buf2, "Class: %s",
	   IS_NPC(victim) ? "mobile" : class_table[victim->class].name);

  sprintf( buf3, "Ethos: %s",
	   IS_NPC(victim) ? "None" : ethos_table[victim->pcdata->ethos].name);
  sprintf( buf, "| %s || %-20.20s  %-22.22s |\n\r",
	   buf1, buf2, buf3);
  add_buf( buffer, buf );

/* LINE 6: Wis (13) Race (20) Align (22) */
  sprintf( buf1, "%s", get_stat_string(victim, STAT_WIS));
  //race
  if (is_affected(victim, gen_unlife))
    sprintf(buf2, "Race : %s", "undead");
  else if (IS_QRACE(victim))
    sprintf(buf2, "Race : %s(%s)",
	    victim->pcdata->race, race_table[victim->race].name);
  else
    sprintf(buf2, "Race : %s", race_table[victim->race].name);
  //align
  if (IS_GOOD(victim))
    sprintf( buf3, "Align: %s", "good");
  else if (IS_EVIL(victim))
    sprintf( buf3, "Align: %s", "evil");
  else
    sprintf( buf3, "Align: %s", "neutral");

  sprintf( buf, "| %s || %-21.21s %-22.22s |\n\r",
	   buf1, buf2, buf3);
  add_buf( buffer, buf );

/* LINE 7: Dex (13) Hp (20) Exp (22)  */
  sprintf( buf1, "%s", get_stat_string(victim, STAT_DEX));
  sprintf( buf2, "Hp   : %4d/%-4d", victim->hit, victim->max_hit );
  sprintf( buf3, "Exp  : %ld/%-6ld",
	   total_exp(victim) - victim->exp, victim->exp);
  sprintf( buf, "| %s || %-20.20s  %-22.22s |\n\r",
	   buf1, buf2, buf3);
  add_buf( buffer, buf );

/* LINE 8: Con (13) Mana (20) Misc (22) */
  sprintf( buf1, "%s", get_stat_string(victim, STAT_CON));
  sprintf( buf2, "Mana : %4d/%-4d", victim->mana, victim->max_mana );

 /* race/class specific things here */
  if (!IS_NPC(victim) && victim->pcdata->enemy_race){
    sprintf( buf3, "Enemy: %s", pc_race_table[ch->pcdata->enemy_race].name);
  }
  else
    buf3[0] = 0;

  sprintf( buf, "| %s || %-20.20s  %-22.22s |\n\r",
	   buf1, buf2, buf3);
  add_buf( buffer, buf );

/* LINE 9: Luck (13) Move (20) ToHit (10) ToDam(11) */
  sprintf( buf1, "%s", get_stat_string(victim, STAT_LUCK));
  sprintf( buf2, "Move : %4d/%-4d", victim->move, victim->max_move );
  sprintf( buf3, "+Hit: %d", GET_HITROLL(victim) );
  sprintf( buf4, "+Dam: %d", GET_DAMROLL(victim) );
  sprintf( buf, "\\ %s \\/ %-20.20s  %-10.10s %-11.11s /\n\r",
	   buf1, buf2, buf3, buf4);
  add_buf( buffer, buf );

/* LINE 10 */
  add_buf( buffer,
  "/---------------/\\------------------/\\--------------------------\\\n\r");

/* LINE 11 Deity (16)*/
  sprintf(buf, "|    ARMOR      ||      SAVES       || Deity : %-16.16s |\n\r",
	  IS_NPC(victim) ? "Creator" : deity_table[victim->pcdata->way].god);
  add_buf( buffer, buf );

/* LINE 12: AC-Slash (13) Svs-Spell (17) Faith (24) */
  sprintf(buf1, "Slash : %5d", GET_AC(victim, AC_SLASH ));
  sprintf(buf2, "Spell      : %2d", victim->savingspell );
  if (IS_NPC(victim))
    sprintf(buf3, "Faith : None");
  else
    sprintf(buf3, "Faith : %s",
	    deity_table[victim->pcdata->way].way);

  sprintf(buf, "| %-13.13s || %-16.16s || %-24.24s |\n\r",
	  buf1, buf2, buf3);
  add_buf( buffer, buf );

/* LINE 13: AC-Pierce (13) Svs-Afflictive (17) Weight (24) */
  sprintf(buf1, "Pierce: %5d", GET_AC(victim, AC_PIERCE ));
  sprintf(buf2, "Afflictive : %2d", victim->savingaffl );
  sprintf(buf3, "Weight: %d/%-3d",
	  get_carry_weight(victim) / 10,
	  can_carry_w(victim) /10 );
  sprintf(buf, "| %-13.13s || %-16.16s || %-24.24s |\n\r",
	  buf1, buf2, buf3);
  add_buf( buffer, buf );

/* LINE 14: AC-Blunt (13) Svs-Maledictive (17) Items (24) */
  sprintf(buf1, "Blunt : %5d", GET_AC(victim, AC_BASH ));
  sprintf(buf2, "Maledictive: %2d", victim->savingmaled );
  sprintf(buf3, "Items : %d/%-3d",
	  victim->carry_number,
	  can_carry_n(victim));
  sprintf(buf, "| %-13.13s || %-16.16s || %-24.24s |\n\r",
	  buf1, buf2, buf3);
  add_buf( buffer, buf );

/* LINE 15: AC-Magic (13) Svs-Mental (17) Prac (13) Train (10) */
  sprintf(buf1, "Magic : %5d", GET_AC(victim, AC_EXOTIC ));
  sprintf(buf2, "Mental     : %2d", victim->savingmental );
  sprintf(buf3, "Prac  : %d", victim->practice );
  sprintf(buf4, "Train: %d", victim->train );
  sprintf(buf, "| %-13.13s || %-16.16s || %-13.13s %-10.10s |\n\r",
	  buf1, buf2, buf3, buf4);
  add_buf( buffer, buf );

/* LINE 16: Weapon Pos(13) Gold (13) Cp (10) */
  if (!IS_NPC(victim)
      && (get_skill(victim, gsn_2h_tactics) || get_skill(victim, gsn_battlestance))){
    sprintf(buf3, "Weapon: %s",
	    victim->pcdata->wep_pos == WEPPOS_HIGH ? "high" :
	    victim->pcdata->wep_pos == WEPPOS_LOW ? "low" :
	    victim->pcdata->wep_pos == WEPPOS_NORMAL? "norm" : "unkown");
  }
  else
    buf3[0] = 0;
  if (!IS_NPC(victim)
      && get_skill(victim, gsn_footwork)){
    sprintf(buf4, "Pos: %s",
	    victim->pcdata->bat_pos == BATPOS_LEFT ? "left" :
	    victim->pcdata->bat_pos == BATPOS_RIGHT ? "right" :
	    victim->pcdata->bat_pos == BATPOS_MIDDLE ? "middle" : "unkown");
  }
  else
    buf4[0] = 0;
  sprintf(buf1, "Gold  : %ld%s",
	  victim->gold > 9999 ? victim->gold / 1000 : victim->gold,
	  victim->gold > 9999 ? "k" : "" );
  if (victim->pCabal){
    int cp = GET_CP(victim);
    if (cp < 1000)
      sprintf(buf2, "CP: %d", cp);
    else
      sprintf(buf2, "CP: %.1fk", (float)cp/1000);
  }
  else
    sprintf(buf2, "%s", "" );
  sprintf(buf, "| %-13.13s || %-16.16s || %-13.13s %-10.10s |\n\r",
	 buf3, buf4, buf1, buf2);
  add_buf( buffer, buf );

/* LINE 17 */
  add_buf( buffer,
  "\\---------------\\/------------------\\/--------------------------/\n\r");

  if (!IS_NULLSTR(argument)){
    scroll = ch->lines;
    ch->lines = 0;
    page_to_char(buf_string(buffer),ch);
    ch->lines = scroll;
    free_buf(buffer);
    return;
  }

/* LINE 18: Condition */
  if (victim->pcdata->dall < 10  )
    sprintf(buf1, "You are in the prime of your health.");
  else if (victim->pcdata->dall < 20  )
    sprintf(buf1, "You have a few cuts and bruises.");
  else if (victim->pcdata->dall < 30  )
    sprintf(buf1, "You are a bit worn out from your wounds.");
  else if (victim->pcdata->dall < 40  )
    sprintf(buf1, "You are covered with deep scars and lacerations.");
  else if (victim->pcdata->dall < 50  )
    sprintf(buf1, "You have sustained mortal injuries in your travels.");
  else if (victim->pcdata->dall < 60  )
    sprintf(buf1, "Death is knocking at your door.");
  else
    sprintf(buf1, "The next death will send you beyond the abyss.");

  sprintf( buf, "/ Condition: %-50.50s \\\n\r", buf1 );
  add_buf( buffer, buf );

  /* monk anatomies */
  if ( (victim->class == gcn_monk || victim->class == gcn_blademaster) && !IS_NPC(victim)){
    OBJ_DATA *obj;
    add_score_line(buffer, "Anatomy :Mob Human Elf Dwarf DemiHum Giant Beast Flying Unique");
    sprintf(buf, "|          %3d  %3d  %3d  %3d    %3d    %3d   %3d   %3d    %3d  |\n\r",
	    victim->pcdata->anatomy[0],
	    victim->pcdata->anatomy[1],
	    victim->pcdata->anatomy[2],
	    victim->pcdata->anatomy[3],
	    victim->pcdata->anatomy[4],
	    victim->pcdata->anatomy[5],
	    victim->pcdata->anatomy[6],
	    victim->pcdata->anatomy[7],
	    victim->pcdata->anatomy[8]);
    add_buf( buffer, buf );

    sprintf(buf, "Encumber:");
    buf1[0] = 0;
    for (i = 0; i < 8; i++){
      if ((obj = get_eq_char(victim, monk_table[i].value)) != NULL && obj->weight > monk_table[i].weight * 10){
	sprintf(buf1, " %s", monk_table[i].wear);
	strcat(buf, buf1);
      }
    }
    if (buf1[0] != 0)
      add_score_line( buffer, buf );
  }

  /* application */
  if (HAS_CLAN(victim)){
    int sec = ClanSecondsToApply( victim);
    if (sec > 0){
      sprintf(buf, "You have %d days left to submit a cabal application.", sec / 86400);
      add_score_line( buffer, buf );
    }
  }
  /* promotion */
  if (!IS_NPC(victim) && victim->pCabal && victim->pcdata->member ){
    if (check_promo_hours( victim->pcdata->member,  victim->pcdata->rank )){
      sprintf(buf, "You are ready for a cabal promotion.");
      add_score_line( buffer, buf );
    }
    else{
      int diff = (victim->pcdata->member->rank + 1) * 10 - (victim->pcdata->member->hours / 3600);
      sprintf( buf, "%d more hours must pass before a cabal promotion.", diff);
      add_score_line( buffer, buf );
    }
  }

  /* alias */
  if (!IS_NPC(ch) && victim->pcdata->alias
      && victim->pcdata->alias[0] != '\0'){
    sprintf( buf, "Alias: The last thing you see is %s.", victim->pcdata->alias);
    add_score_line( buffer, buf );
  }

  /* Hunger, Thirst, and Drunkiness */
  if ( !IS_NPC(victim)){
    if (is_affected(victim, gsn_drunken)
	|| victim->pcdata->condition[COND_DRUNK]   > 10 )
      add_score_line(buffer, "You are drunk." );
    if ( victim->pcdata->condition[COND_THIRST] ==  0 )
      add_score_line(buffer,"You are thirsty.");
    if (victim->pcdata->condition[COND_HUNGER]   ==  0 )
      add_score_line(buffer,"You are hungry." );
    if ( is_ghost( victim, 300 ) )
      add_score_line(buffer, "You are a ghost.");

    /* SERVANTS */
    if (victim->pcdata->familiar != NULL){
      sprintf(buf, "Your servant task: %s", special_bit_name(victim->pcdata->familiar->special));
      add_score_line(buffer, buf);
    }
/* CRUSADE */
    if ( (paf = affect_find(victim->affected, gen_crusade)) != NULL){
      sprintf(buf, "You are on Crusade against %s", show_crusade( paf ));
      add_score_line( buffer, buf );
    }

/* CABAL AREA OWNERSHIPS */
    /* CABAL AREA EFFECTS */
    if (victim->in_room && victim->in_room->area->pCabal ){
      if (!str_cmp(victim->in_room->area->pCabal->name, "Knight")
	  && IS_GOOD(ch))
	sprintf(buf, "[%s] forces are aiding your healing.",
		victim->in_room->area->pCabal->who_name );
      else if (!str_cmp(victim->in_room->area->pCabal->name, "Savant")
	       && (class_table[victim->class].fMana
		   || class_table[victim->class].sMana ))
	sprintf( buf, "[%s] forces are aiding your mind.",
		 victim->in_room->area->pCabal->who_name );
      else if (!str_cmp(victim->in_room->area->pCabal->name, "Warmaster")
	       && !class_table[victim->class].fMana
	       && !class_table[victim->class].sMana)
	sprintf( buf, "[%s] forces are aiding your feet and stamina.",
		 victim->in_room->area->pCabal->who_name );
      else if (!str_cmp(victim->in_room->area->pCabal->name, "Nexus")
	       && IS_EVIL(victim))
	sprintf(buf, "[%s] forces are aiding your violent instincts.",
		victim->in_room->area->pCabal->who_name );
      else
	buf[0] = 0;

      if (buf[0] != 0){
	sprintf(buf1, "| %-65.65s |\n\r", buf );
	add_buf( buffer, buf1 );
      }
    }
    if(is_affected(victim, gen_study)){
      add_score_line(buffer, study_info(victim, FALSE, NULL));
    }
/* show abduction */
    if (is_affected(victim, gsn_abduct)){
      add_score_line(buffer, abduct_info( victim ) );
    }
/* show chaos orb */
    if ( (paf = affect_find(victim->affected, gen_orb)) != NULL){
      sprintf(buf1, "Your %s is at level %d",
	      effect_table[GN_POS(paf->type)].name,
	      paf->modifier / effect_table[GN_POS(paf->type)].value3[0] );
      add_score_line( buffer, buf1 );
    }
/* TRAP info */
    for (i = 0; i < MAX_TRAPS; i++){
      TRAP_DATA* pTrap = victim->traps[i];
      if (pTrap == NULL)
	continue;
      if (pTrap->on_obj){
	sprintf(buf1, "You've set %s on %s [%d]",
		pTrap->name,
		pTrap->on_obj->short_descr,
		pTrap->duration);
	add_score_line( buffer, buf1 );
      }
      else if (pTrap->on_exit){
	sprintf(buf1, "You've set %s on path leading to %s [%d]",
	      pTrap->name,
		pTrap->on_exit->to_room ? pTrap->on_exit->to_room->name : "error",
		pTrap->duration);
	sprintf(buf, "| %-65.65s |\n\r", buf1 );
	add_buf( buffer, buf );
      }
      else{
	sprintf(buf1, "You've set a trap but something is wrong. ");
	add_score_line( buffer, buf1 );
      }
    }
    /* show crimes */
    if (show_crimes(ch, victim, FALSE, TRUE ) != CRIME_ALLOW){
      add_score_line(buffer, "You are wanted for crimes.  Use \"crimes\" for details");
    }
  }
/* END */
  sprintf( buf,
   "\\---------------------------------------------------------------/\n\r");
  add_buf( buffer, buf );

  scroll = ch->lines;
  ch->lines = 0;
  page_to_char(buf_string(buffer),ch);
  ch->lines = scroll;
  free_buf(buffer);
}

void do_crimes( CHAR_DATA *ch, char *argument ){
  show_crimes(ch, ch, FALSE, FALSE );
}

void do_score( CHAR_DATA *ch, char *argument ){

  int i;
  AFFECT_DATA* paf;

  if (is_affected(ch,gsn_linked_mind))
    return;
  if (IS_GAME(ch, GAME_NEWSCORE )){
    new_score( ch, ch, argument );
    return;
  }
  sendf( ch, "You are %s",  ch->name);
  if (!IS_NPC(ch))
    sendf(ch, ", loyal believer in way of %s and powers of %s.",
	  deity_table[ch->pcdata->way].way,
	  path_table[deity_table[ch->pcdata->way].path].name);

  sendf(ch, "\n\rLevel %d, %d years old (%d hours).\n\r", ch->level, get_age(ch),
	( ch->played + (int) (mud_data.current_time - ch->logon) ) / 3600);
  if ( get_trust( ch ) != ch->level )
    sendf( ch, "You are trusted at level %d.\n\r", get_trust( ch ) );
  if (!IS_NPC(ch) && ch->pcdata->alias && ch->pcdata->alias[0] != '\0')
    sendf( ch, "Alias: The last thing you see is %s.\n\r", ch->pcdata->alias);
  sendf(ch, "Race: %s %s%s%s Sex: %s  Class: %s",
	IS_QRACE(ch) ? ch->pcdata->race :
	race_table[is_affected(ch, gen_unlife) ?
		  race_lookup("undead") : ch->race].name,
	IS_QRACE(ch) ? "(" : "",
	IS_QRACE(ch) ? race_table[is_affected(ch, gen_unlife) ?
				 race_lookup("undead") : ch->race].name :
	"",
	IS_QRACE(ch) ? ") " : "",
	ch->sex == 0
	? "sexless" : ch->sex == 1 ? "male" : "female",
	IS_NPC(ch) ? "mobile" : class_table[ch->class].name);

  if(!IS_NPC(ch) &&  IS_SET(ch->act2, PLR_MASQUERADE))
    sendf(ch, "  `5[`%%MASQUE`5]``\n\r");
  else
    send_to_char("\n\r",ch);
  sendf(ch, "Hometown: %s     Alignment: %s    Ethos: %s",
	IS_NPC(ch) ? "None" : hometown_table[ch->hometown].name,
	ch->alignment >= GOOD_THRESH ? "Good" : ch->alignment < GOOD_THRESH && ch->alignment > EVIL_THRESH ? "Neutral" : ch->alignment <= EVIL_THRESH ? "Evil" : "Wrong for a mortal",
	IS_NPC(ch) ? "None" : ethos_table[ch->pcdata->ethos].name);
  sendf(ch, "\n\r");
  if ( !IS_NPC(ch) && ch->pCabal){
    sendf( ch, "%s of [%s] with %d %s%s to your name.\n\r",
	   get_crank( ch ),
	   ch->pCabal->who_name,
	   GET_CP(ch),
	   ch->pCabal->currency,
	   GET_CP(ch) == 1 ? "" : "s");
  }
  if (!IS_NPC(ch) && ch->pCabal && ch->pcdata->member ){
    if ((ch->pcdata->member->hours / 3600) >= (ch->pcdata->member->rank + 1) * 10){
      sendf(ch, "You have been cleared for a promotion in the ranks of [%s]\n\r", ch->pCabal->who_name);
    }
    else{
      int diff = (ch->pcdata->member->rank + 1) * 10 - (ch->pcdata->member->hours / 3600);
      sendf(ch, "%d more hours must pass, before your cabal rank may change.\n\r", diff);
    }

  }
  sendf( ch, "You have %d/%d hit, %d/%d mana, %d/%d movement.\n\r",
	 ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move);
  sendf( ch, "You have %d practices and %d training sessions.\n\r", ch->practice, ch->train);
  sendf( ch, "You are carrying %d/%d items with weight %d/%d pounds.\n\r",
	 ch->carry_number, can_carry_n(ch), get_carry_weight(ch) / 10, can_carry_w(ch) /10 );
  sendf( ch, "Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
	 ch->perm_stat[STAT_STR], get_curr_stat(ch,STAT_STR), ch->perm_stat[STAT_INT], get_curr_stat(ch,STAT_INT),
	 ch->perm_stat[STAT_WIS], get_curr_stat(ch,STAT_WIS), ch->perm_stat[STAT_DEX], get_curr_stat(ch,STAT_DEX),
	 ch->perm_stat[STAT_CON], get_curr_stat(ch,STAT_CON) );
  sendf( ch, "You have scored %ld exp, and have %ld gold coins.\n\r",ch->exp,ch->gold );
  if (!IS_NPC(ch))
    {
      int luck;
      send_to_char("Condition: ",ch);
      if (ch->pcdata->dall < 10  )        send_to_char("You are in the prime of your health.\n\r",ch);
      else if (ch->pcdata->dall < 20  )   send_to_char("You have a few cuts and bruises.\n\r",ch);
      else if (ch->pcdata->dall < 30  )   send_to_char("You are a bit worn out from your wounds.\n\r",ch);
      else if (ch->pcdata->dall < 40  )   send_to_char("You are covered with deep scars and jagged lacerations.\n\r",ch);
      else if (ch->pcdata->dall < 50  )   send_to_char("You have sustained some mortal injuries in your travels.\n\r",ch);
      else if (ch->pcdata->dall < 60  )   send_to_char("Death is knocking at your door.\n\r",ch);
      else				    send_to_char("The next death will send you beyond the abyss.\n\r",ch);
      luck = get_curr_stat(ch,STAT_LUCK);
      send_to_char("Luck: ",ch);
      if (luck < 8) 			send_to_char("You have hit rock bottom on your luck.\n\r",ch);
      else if (luck < 12)		send_to_char("Things are just not going your way.\n\r",ch);
      else if (luck < 15)		send_to_char("You are down on your luck.\n\r",ch);
      else if (luck < 18)		send_to_char("Your luck can turn either way.\n\r",ch);
      else if (luck < 21)		send_to_char("You feel lucky.\n\r",ch);
      else if (luck < 24)		send_to_char("You feel very lucky.\n\r",ch);
      else 				send_to_char("Lady Luck is going your way.\n\r",ch);
    }

  /* Show Avatar Level description. */
  if (!IS_NPC(ch) && IS_AVATAR(ch))
    act(get_avatar_desc(ch), ch, NULL, NULL, TO_CHAR);

  if (!IS_NPC(ch) && ch->level < LEVEL_HERO)
    sendf (ch, "You need %ld exp to level.\n\r",total_exp(ch) - ch->exp);
  sendf( ch, "Wimpy set to %d hit points.\n\r", ch->wimpy );
  if (!IS_NPC(ch) && ch->pcdata->guarding != NULL)
    sendf(ch, "Guarding: %s\n\r",ch->pcdata->guarding->name);
  if (!IS_NPC(ch) && ch->pcdata->guarded_by != NULL)
    sendf(ch, "Guarded by: %s\n\r",ch->pcdata->guarded_by->name);
  if ( !IS_NPC(ch) && (is_affected(ch,gsn_drunken) || ch->pcdata->condition[COND_DRUNK]   > 10 ))
    send_to_char( "You are drunk.\n\r",   ch );
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
    send_to_char( "You are thirsty.\n\r", ch );
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER]   ==  0 )
    send_to_char( "You are hungry.\n\r",  ch );
  if ( !IS_NPC(ch) && is_ghost( ch, 300 ) )
    send_to_char( "You are a ghost.\n\r",  ch);
  switch ( ch->position )
    {
    case POS_DEAD:
      send_to_char( "You are DEAD!!\n\r",             ch );	break;
    case POS_MORTAL:
      send_to_char( "You are mortally wounded.\n\r",  ch );	break;
    case POS_INCAP:
	send_to_char( "You are incapacitated.\n\r",     ch );	break;
    case POS_STUNNED:
	send_to_char( "You are stunned.\n\r",           ch );	break;
    case POS_MEDITATE:
	send_to_char( "You are meditating.\n\r",          ch );	break;
    case POS_SLEEPING:
	if (IS_AFFECTED2(ch, AFF_CATALEPSY))
	{   send_to_char( "You are in a state of self induced catalepsy.\n\r",          ch );	break;}
	else
	{   send_to_char( "You are sleeping.\n\r",          ch );	break;}
    case POS_RESTING:
	send_to_char( "You are resting.\n\r",           ch );	break;
    case POS_SITTING:
	send_to_char( "You are sitting.\n\r",           ch );	break;
    case POS_STANDING:
	send_to_char( "You are standing.\n\r",          ch );	break;
    case POS_FIGHTING:
	send_to_char( "You are fighting.\n\r",          ch );	break;
    }
    if (!IS_NPC(ch) && ch->pcdata->familiar != NULL)
        sendf(ch, "Your servant has been commanded the following: %s\n\r",special_bit_name(ch->pcdata->familiar->special));
    if (ch->level >= 25)
	sendf( ch, "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
          GET_AC(ch,AC_PIERCE), GET_AC(ch,AC_BASH), GET_AC(ch,AC_SLASH), GET_AC(ch,AC_EXOTIC));
    if (ch->level >= 30)
	sendf( ch, "Save vs  Spell: %d  Affliction: %d  Malediction: %d  Mental: %d\n\r", ch->savingspell,ch->savingaffl,ch->savingmaled,ch->savingmental);
    for (i = 0; i < 4; i++)
    {
	char * temp;
	switch(i)
	{
	    case(AC_PIERCE):    temp = "piercing";      break;
	    case(AC_BASH):      temp = "bashing";       break;
	    case(AC_SLASH):     temp = "slashing";      break;
	    case(AC_EXOTIC):    temp = "magic";         break;
	    default:            temp = "error";         break;
	}
	send_to_char("You are ", ch);
        if (GET_AC(ch,i) >=  101 )    sendf(ch,"hopelessly vulnerable to %s.\n\r",temp);
        else if (GET_AC(ch,i)>=80)    sendf(ch,"defenseless against %s.\n\r",temp);
        else if (GET_AC(ch,i)>=60)    sendf(ch,"barely protected from %s.\n\r",temp);
        else if (GET_AC(ch,i)>=40)    sendf(ch,"slightly armored against %s.\n\r",temp);
        else if (GET_AC(ch,i)>=20)    sendf(ch,"somewhat armored against %s.\n\r",temp);
        else if (GET_AC(ch,i)>=0)     sendf(ch,"armored against %s.\n\r",temp);
        else if (GET_AC(ch,i)>=-40)   sendf(ch,"well-armored against %s.\n\r",temp);
        else if (GET_AC(ch,i)>=-80)   sendf(ch,"very well-armored against %s.\n\r",temp);
        else if (GET_AC(ch,i)>=-120)  sendf(ch,"heavily armored against %s.\n\r",temp);
        else if (GET_AC(ch,i)>=-160)  sendf(ch,"superbly armored against %s.\n\r",temp);
        else if (GET_AC(ch,i)>=-200)  sendf(ch,"supremely armored against %s.\n\r",temp);
        else if (GET_AC(ch,i)>=-250)  sendf(ch,"almost invulnerable to %s.\n\r",temp);
        else if (GET_AC(ch,i)>=-300)  sendf(ch,"divinely armored against %s.\n\r",temp);
        else                          sendf(ch,"utterly invincible against %s.\n\r",temp);
    }
    if (IS_IMMORTAL(ch))
    {
        send_to_char("Holy Light: ",ch);
        if (IS_SET(ch->act,PLR_HOLYLIGHT))
            send_to_char("on",ch);
        else
            send_to_char("off",ch);
        if (ch->invis_level)
            sendf( ch, "  Invisible: level %d",ch->invis_level);
        if (ch->incog_level)
            sendf( ch, "  Incognito: level %d",ch->incog_level);
        send_to_char("\n\r",ch);
    }
    if ( ch->level >= 15 )
        sendf( ch, "Hitroll: %d  Damroll: %d.\n\r",GET_HITROLL(ch),GET_DAMROLL(ch));
    if (!IS_NPC(ch)
	&& (get_skill(ch, gsn_2h_tactics) && has_twohanded(ch)) ){
      sendf(ch, "Your weapon is held %s.\n\r",
	    ch->pcdata->wep_pos == WEPPOS_HIGH ? "high" :
	    ch->pcdata->wep_pos == WEPPOS_LOW ? "low" :
	    ch->pcdata->wep_pos == WEPPOS_NORMAL? "normal" : "unkown");
    }
    if ( (paf = affect_find(ch->affected, gen_crusade)) != NULL){
      sendf(ch, "You are on a Crusade against %s\n\r", show_crusade( paf ));
    }
    /* CABAL AREA EFFECTS */
    if (ch->in_room && ch->in_room->area->pCabal ){
      if (!str_cmp(ch->in_room->area->pCabal->name, "Knight") && IS_GOOD(ch))
	sendf( ch, "[%s] forces are aiding your healing.\n\r", ch->in_room->area->pCabal->who_name );
      else if (!str_cmp(ch->in_room->area->pCabal->name, "Savant")
	       && (class_table[ch->class].fMana || class_table[ch->class].sMana ))
	sendf( ch, "[%s] forces are aiding your mind.\n\r", ch->in_room->area->pCabal->who_name );
      else if (!str_cmp(ch->in_room->area->pCabal->name, "Warmaster")
	       && !class_table[ch->class].fMana && !class_table[ch->class].sMana)
	sendf( ch, "[%s] forces are aiding your feet and stamina.\n\r", ch->in_room->area->pCabal->who_name );
      else if (!str_cmp(ch->in_room->area->pCabal->name, "Nexus") && IS_EVIL(ch))
	sendf( ch, "[%s] forces are aiding your darker instincts.\n\r", ch->in_room->area->pCabal->who_name );
    }

    if ((ch->class == gcn_monk || ch->class == gcn_blademaster) && !IS_NPC(ch))
    {
	OBJ_DATA *obj;
	send_to_char("Anatomy:", ch);
        for (i = 0; anatomy_table[i].name != NULL; i++)
	    sendf(ch, "  %s: %d", anatomy_table[i].name, ch->pcdata->anatomy[i]);
	send_to_char("\n\r", ch);
	send_to_char("Encumberance:", ch);
	for (i = 0; i < 8; i++)
	    if ((obj = get_eq_char(ch, monk_table[i].value)) != NULL && obj->weight > monk_table[i].weight * 10)
	        sendf(ch, " %s", monk_table[i].wear);
	send_to_char("\n\r", ch);
    }

//Last thing we show before affects is chance to assasinate:
    if(is_affected(ch, gen_study))
      sendf(ch, "%s\n\r", study_info(ch, FALSE, NULL));
/* show abduction */
    sendf(ch, "%s\n\r", abduct_info(ch));
/* show chaos orb */
    if ( (paf = affect_find(ch->affected, gen_orb)) != NULL){
      sendf(ch, "Your %s is at level %d\n\r",
	    effect_table[GN_POS(paf->type)].name,
	    paf->modifier / effect_table[GN_POS(paf->type)].value3[0] );
    }
/* show crimes */
    show_crimes(ch, ch, FALSE, FALSE );
/* TRAP info */
    for (i = 0; i < MAX_TRAPS; i++){
      TRAP_DATA* pTrap = ch->traps[i];
      if (pTrap == NULL)
	continue;
      if (pTrap->on_obj)
	sendf(ch, "You've set %s on %s [%d]\n\r",
	      pTrap->name,
	      pTrap->on_obj->short_descr,
	      pTrap->duration);
      else if (pTrap->on_exit)
	sendf(ch, "You've set %s on path leading to %s [%d]\n\r",
	      pTrap->name,
	      pTrap->on_exit->to_room ? pTrap->on_exit->to_room->name : "error",
	      pTrap->duration);
      else
	sendf(ch, "You've set a trap but something is wrong. ");
    }
    if (IS_SET(ch->comm,COMM_SHOW_AFFECTS))
        send_to_char( "\n\r", ch);
    else
	do_affects(ch, "");
}

void show_affects(CHAR_DATA* ch, CHAR_DATA* victim ){
  AFFECT_DATA *paf, *paf_last = NULL;
  char buf[MSL];
  bool fAffected = FALSE;

//Check if there is anythhing to show.
  if ( victim->affected != NULL || victim->affected2 != NULL)
    {
      for ( paf = victim->affected; paf != NULL; paf = paf->next )
	if (paf->where != TO_NONE)
	  {fAffected = TRUE;break;}
      if(!fAffected)
	for ( paf = victim->affected2; paf != NULL; paf = paf->next )
	    if (paf->where != TO_NONE)
	      {fAffected = TRUE;break;}
    }//end show check.

  if (fAffected)
    {
      if (ch == victim)
	send_to_char( "You are affected by the following:\n\r", ch );
      else
	send_to_char( "You sense following auras:\n\r", ch );
      for ( paf = victim->affected; paf != NULL; paf = paf->next )
	{
//Viri: 04-1
//if paf->valid is false we do not show the effect.
//if paf->where = TO_NONE we do not show it eithter.
//Viri 04-18 added GN_POS() for poper lookups with gen's
//also if APPLY_NONE is set we only show the name and duration.

	  if (paf->type == gsn_cutpurse
	      || paf->type == gsn_noquit
	      || paf->type == gsn_sneak
	      || !paf->valid
	      || paf->where == TO_NONE)
	    continue;
	  if (paf_last != NULL && paf->type == paf_last->type)
            {
	      if (ch->level >= 20)
		sprintf( buf, "                      ");
	      else
		continue;
            }
	  else if (paf->type == gsn_tarot)
	    {
	      if (paf->level == 69)
		sprintf( buf, "Tarot: %-15s", tarot_table[paf->modifier].name );
	      else
		{
		  AFFECT_DATA *npaf;
		  for ( npaf = victim->affected; npaf != NULL; npaf = npaf->next )
		    if (npaf->type == gsn_tarot && npaf->level == 69)
		      break;
		  if (npaf == NULL)
		    {
		      if (IS_GEN(npaf->type))
			sprintf( buf, "Spell: %-15s", effect_table[GN_POS(npaf->type)].name );
		      else
			sprintf( buf, "Spell: %-15s", skill_table[npaf->type].name );
		    }
		  else
		    sprintf( buf, "Tarot: %-15s", tarot_table[npaf->modifier].name );
		}
	    }
	  else
	    if (IS_GEN(paf->type))
	      sprintf( buf, "Spell: %-15s", effect_table[GN_POS(paf->type)].name );
	    else
	      sprintf( buf, "Spell: %-15s", skill_table[paf->type].name );

	  send_to_char( buf, ch );
	  if ( ch->level >= 20 )
	    {
//IF APPLY NONE THEN WE DONT SHOW JUNK
	      if (paf->location != APPLY_NONE) {
		sendf( ch, ": modifies %s by %d ",
		       paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location ),
		       paf->modifier);
	      }
	      else {
                sendf( ch, ": ");
	      }

	      if ( paf->duration == -1 )
		sendf( ch, "permanently" );
	      else
		sendf( ch, "for %d hours", paf->duration );
	    }
	    send_to_char( "\n\r", ch );
	    paf_last = paf;
	}
      for ( paf = victim->affected2; paf != NULL; paf = paf->next )
	{
	  if (paf->where==TO_NONE)
	    continue;
	  if (paf_last != NULL && paf->type == paf_last->type)
            {
	      if (ch->level >= 20)
		sprintf( buf, "                      ");
	      else
		continue;
            }
	  else
	    sprintf( buf, "Song:  %-15s", song_table[paf->type].name );
	    send_to_char( buf, ch );
	    if ( ch->level >= 20 )
	      {
//If song is APPLY_NONE WE DONT SHOW ZEROS
		if (paf->location != APPLY_NONE)
                sendf( ch, ": modifies %s by %d ", affect_loc_name( paf->location ), paf->modifier);
		else
		  sendf( ch, ": ");
		if ( paf->duration == -1 )
		  sendf( ch, "permanently" );
		else
		  sendf( ch, "for %d hours", paf->duration );
	      }
	    send_to_char( "\n\r", ch );
	    paf_last = paf;
	}
    }
  else {
    if (ch == victim)
      send_to_char( "You are affected by the following:\n\r", ch );
    else
      send_to_char( "You sense following auras:\n\r", ch );
    }
}

struct show_aff_data{
  bool fOn;
  int  dur;
};

/* new affect show function */
void show_custom_affects( CHAR_DATA* ch, char* argument ){
  AFFECT_DATA* paf, *paf_last = NULL;
  AFF_DISPLAY* cus_aff;
  char buf[MIL];
  char custom1[MIL];
  char custom2[MIL];

  static struct show_aff_data data_table[MAX_SKILL + MAX_EFFECTS];
  int i;
  bool fCustom = FALSE;
  bool fFull = FALSE;

  if (IS_NPC(ch))
    return;
  else cus_aff = ch->pcdata->custom_aff;

  if (!IS_NULLSTR( argument ))
    fFull = TRUE;

  /* reset the data table */
  memset(data_table, 0, sizeof( data_table ));

  /* first run through all the pafs we have and fill in the data */
  for (paf = ch->affected; paf; paf = paf->next ){
    if (paf->type == gsn_cutpurse
	|| paf->type == gsn_noquit
	|| paf->type == gsn_sneak
	|| !paf->valid
	|| paf->where == TO_NONE)
      continue;

    //get the lowest duration only and turn it on
    if (!data_table[paf->type].fOn
	|| paf->duration < data_table[paf->type].dur){
      data_table[paf->type].dur = paf->duration;
    }
    data_table[paf->type].fOn = TRUE;
  }

  /* we now have a table of all affects */
  /* compose the custom effects, and remove them from table */
  custom1[0] = 0;
  custom2[0] = 0;

  if (!fFull){
    for (i = 0; i < MAX_AFF_DISPLAY; i++){
      if (IS_NULLSTR(cus_aff[i].name)
	|| cus_aff[i].gsn < 1
	  || cus_aff[i].gsn > (MAX_SKILL + MAX_EFFECTS))
	continue;
      fCustom = TRUE;

      //designation
      sprintf( buf, "%-3.3s ", cus_aff[i].name );
      strcat( custom2, buf );

      //duration
      if (!data_table[cus_aff[i].gsn].fOn)
	sprintf(buf, " %c  ", '-' );
      //duration (different alignment for 2 digits and 3 digits
      else if (data_table[cus_aff[i].gsn].dur == -1)
	sprintf(buf, " %c  ", '+' );
      else if (data_table[cus_aff[i].gsn].dur < 2)
	sprintf(buf, "`! %d``  ", data_table[cus_aff[i].gsn].dur);
      else if (data_table[cus_aff[i].gsn].dur / 10 < 1)
	sprintf(buf, " %d  ", data_table[cus_aff[i].gsn].dur);
      else if (data_table[cus_aff[i].gsn].dur / 10 < 10)
	sprintf(buf, "%-2d  ", data_table[cus_aff[i].gsn].dur);
      else
	sprintf(buf, "%-3d ", data_table[cus_aff[i].gsn].dur);
      strcat(custom1, buf);

    //remove from data table
      data_table[cus_aff[i].gsn].fOn = FALSE;
      data_table[cus_aff[i].gsn].dur = 0;
    }

    /* show both custom strings */
    if (fCustom){
      sendf( ch, "Affects:\n\r"\
	     " %s\n\r"\
	     " %s\n\r",
	     custom1,
	     custom2);
    }
  }
  else
    send_to_char("You are affected by the following:\n\r", ch);

  if (!fCustom){
    send_to_char("You are affected by the following:\n\r", ch);
  }

  /* show affects that custom scan did not pick up */
  for (paf = ch->affected; paf; paf = paf->next ){
    if (!data_table[paf->type].fOn)
      continue;

    if (paf_last != NULL && paf->type == paf_last->type){
      sprintf( buf, "                      ");
    }
    else if (paf->type == gsn_tarot){
      if (paf->level == 69)
	sprintf( buf, "Tarot: %-15s", tarot_table[paf->modifier].name );
      else{
	AFFECT_DATA *npaf;
	for ( npaf = ch->affected; npaf != NULL; npaf = npaf->next )
	  if (npaf->type == gsn_tarot && npaf->level == 69)
	    break;
	if (npaf == NULL){
	  if (IS_GEN(npaf->type))
	    sprintf( buf, "Spell: %-15s", effect_table[GN_POS(npaf->type)].name );
	  else
	    sprintf( buf, "Spell: %-15s", skill_table[npaf->type].name );
	}
	else
	  sprintf( buf, "Tarot: %-15s", tarot_table[npaf->modifier].name );
      }
    }
    else
      if (IS_GEN(paf->type))
	sprintf( buf, "Spell: %-15s", effect_table[GN_POS(paf->type)].name );
      else
	sprintf( buf, "Spell: %-15s", skill_table[paf->type].name );
    send_to_char( buf, ch );

    if (paf->location != APPLY_NONE) {
      sendf( ch, ": modifies %s by %d ",
	     paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location ),
	     paf->modifier);
    }
    else {
      sendf( ch, ": ");
    }

    if ( paf->duration == -1 )
      sendf( ch, "permanently" );
    else
      sendf( ch, "for %d hours", paf->duration );

    send_to_char( "\n\r", ch );
    paf_last = paf;
  }

  //show songs
  for ( paf = ch->affected2; paf != NULL; paf = paf->next ){
    if (paf->where==TO_NONE)
      continue;
    if (paf_last != NULL && paf->type == paf_last->type){
      sprintf( buf, "                      ");
    }
    else
      sprintf( buf, "Song:  %-15s", song_table[paf->type].name );
    send_to_char( buf, ch );
    if (paf->location != APPLY_NONE)
      sendf( ch, ": modifies %s by %d ", affect_loc_name( paf->location ), paf->modifier);
    else
      sendf( ch, ": ");
    if ( paf->duration == -1 )
      sendf( ch, "permanently" );
    else
      sendf( ch, "for %d hours", paf->duration );

    send_to_char( "\n\r", ch );
    paf_last = paf;
  }
}

/* handles setup, and listing of custom affects strings */
void do_custom_affects(CHAR_DATA *ch, char *argument ){
  char buf[MIL];
  int i, sn;

  one_argument(argument, buf );

  if (IS_NPC(ch))
    return;
  else if (IS_NULLSTR(buf)){
    send_to_char("affect list or affect <#> <designation> <affect>\n\r", ch);
    return;
  }
  else if (!str_cmp(buf, "list")){
    send_to_char("Pos  Des  Affect Name\n\r", ch);
    for (i = 0; i < MAX_AFF_DISPLAY; i++){
      if (IS_NULLSTR(ch->pcdata->custom_aff[i].name))
	continue;
      sendf(ch,  "%2d.  %-3.3s  ", i + 1, ch->pcdata->custom_aff[i].name);
      if (IS_GEN(ch->pcdata->custom_aff[i].gsn))
	send_to_char( effect_table[GN_POS(ch->pcdata->custom_aff[i].gsn)].name, ch );
      else
	send_to_char( skill_table[GN_POS(ch->pcdata->custom_aff[i].gsn)].name, ch );
      send_to_char("\n\r", ch);
    }
    return;
  }
  else if (is_number(buf)){
    char des[MIL];

    argument = one_argument(argument, buf);
    argument = one_argument_2(argument, des);

    if ( (i = atoi(buf)) < 1){
      sendf(ch, "Position number must be %d-%d.\n\r", 1, MAX_AFF_DISPLAY);
      return;
    }
    else if ( i > MAX_AFF_DISPLAY){
      sendf(ch, "Position number must be %d-%d.\n\r", 1, MAX_AFF_DISPLAY);
      return;
    }
    else if (IS_NULLSTR(argument)){
      ch->pcdata->custom_aff[i - 1].name[0] = 0;
      ch->pcdata->custom_aff[i - 1].gsn = 0;

      do_custom_affects(ch, "list");
      return;
    }
    else if ( (sn = skill_lookup( argument )) < 1){
      send_to_char("No such affect.\n\r", ch);
      return;
    }
    sprintf(buf, "%s", des);
    buf[3] = 0;

    strcpy(ch->pcdata->custom_aff[i - 1].name, buf );
    ch->pcdata->custom_aff[i - 1].gsn = sn;

    do_custom_affects(ch, "list");
    return;
  }
  else
    do_custom_affects(ch, "");
}

/* Show SELF */
void do_affects(CHAR_DATA *ch, char *argument ){
  if (!IS_NULLSTR(argument) && str_prefix(argument, "full")){
    do_custom_affects( ch, argument );
    return;
  }
  show_custom_affects(ch, argument);
}

/* Show others (detect magic) */
void do_affects2(CHAR_DATA *ch, CHAR_DATA *victim )
{
  show_affects(ch, victim );
}



void do_dice( CHAR_DATA *ch, char *argument ){
  char buf[MIL];
  int number = number_range(1, 6);
  int die = IS_EVIL(ch) ? 2 : IS_GOOD(ch) ? 0 : 1;
  static char * const dice  [] = { "ivory",  "stone",  "bone" };

  act("With a brief shake $n rolls $s $t die.", ch, dice[die], NULL, TO_ROOM);
  act("With a brief shake you roll your $t die.", ch, dice[die], NULL, TO_CHAR);
  sprintf(buf, "The die stops on %d!", number);
  act(buf, ch, NULL, NULL, TO_ALL);
  WAIT_STATE(ch, PULSE_VIOLENCE / 3);
}





void do_time( CHAR_DATA *ch, char *argument )
{
    int days, hours, minutes, seconds;
    long timediff;

    sendf( ch, "It is %s\n\r", mud_date());

    timediff = mud_data.current_time - mud_data.crash_time;
    days = timediff / (60*60*24);
    timediff = (timediff - days*60*60*24);
    hours = timediff / (60*60);
    timediff = (timediff - hours*60*60);
    sendf( ch, "Age of Mirlan has been up for %d day%s %d hour%s.\n\r",
	   days, days == 1 ? "" : "s", hours, hours == 1 ? "" : "s");

    if (!IS_IMMORTAL(ch))
      return;
    send_to_char( "\n\r", ch);
    sendf( ch, "Age of Mirlan started up at %s\r",(char *) ctime(&mud_data.crash_time ) );
    sendf( ch, "Last reboot at %s\rThe system time is %s\r", mud_data.str_boot_time, (char *) ctime( &mud_data.current_time ) );

    if (mud_data.deaths == 0)
    {
	send_to_char("There have been no PKs yet.\n\r",ch);
	return;
    }
    timediff = mud_data.current_time - mud_data.crash_time;
    timediff /= mud_data.deaths;
    minutes = timediff / 60;
    timediff = (timediff - minutes*60);
    seconds = timediff;
    sendf( ch, "With %d PKs so far, we are averaging one every %d minute%s and %d second%s.\n\r",
      mud_data.deaths, minutes, minutes == 1 ? "" : "s", seconds, seconds == 1 ? "" : "s");
    sendf(ch, "Current maximum number of quests: %d\n\r", mud_data.max_quest);
}

void do_weather( CHAR_DATA *ch, char *argument )
{
    char sky[MIL], heat[MIL];
    int temp = temp_adjust(ch->in_room->temp);
    if (mud_data.weather_info.sky == 0)
        sprintf(sky, "cloudless");
    else if (mud_data.weather_info.sky == 1)
	sprintf(sky, "cloudy");
    else if (mud_data.weather_info.sky == 2 && temp < 30)
	sprintf(sky, "snowing");
    else if (mud_data.weather_info.sky == 2 && temp < 36)
	sprintf(sky, "sleeting");
    else if (mud_data.weather_info.sky == 2)
	sprintf(sky, "rainy");
    else if (mud_data.weather_info.sky == 3 && temp < 30)
	sprintf(sky, "snowing like a blizzard");
    else if (mud_data.weather_info.sky == 3 && temp < 36)
	sprintf(sky, "hailing");
    else if (mud_data.weather_info.sky == 3)
	sprintf(sky, "lit by flashes of lightning");
    else
	sprintf(sky, "calm");
    if (temp < 0)
	sprintf(heat, "bitter cold");
    else if (temp < 30)
	sprintf(heat, "freezing");
    else if (temp < 45)
	sprintf(heat, "cold");
    else if (temp < 60)
	sprintf(heat, "cool");
    else if (temp < 70)
	sprintf(heat, "mild");
    else if (temp < 80)
	sprintf(heat, "balmy");
    else if (temp < 90)
	sprintf(heat, "warm");
    else if (temp < 110)
	sprintf(heat, "hot");
    else
	sprintf(heat, "baking hot");
    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You can't see the weather indoors.\n\r", ch );
	return;
    }
    sendf( ch, "The sky is %s and a %s %s.\n\r",
      sky, heat, mud_data.weather_info.change >= 0 ? "southerly breeze blows" : "northern gust blows");
}

void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
    char argall[MIL], argone[MIL];
    int level;
    output = new_buf();
    if ( argument[0] == '\0' )
	argument = "summary";
    argall[0] = '\0';
    while (argument[0] != '\0' )
    {
	argument = one_argument(argument,argone);
	if (argall[0] != '\0')
	    strcat(argall," ");
	strcat(argall,argone);
    }
    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
      level = pHelp->level;
      if (level == 99)
	continue;
      if (level < 100 && level > get_trust( ch ) )
	continue;

      /* restricted helps */
      if (!IS_IMMORTAL(ch)){
	switch (level){
	case HELP_RACE:
	  /* special races helps */
	  if (pHelp->type < 0){
	    if (pHelp->type == -1 && !IS_AVATAR(ch))
	      continue;
	  }
	  else if (pHelp->type != ch->race)
	    continue;
	  break;
	case HELP_CLASS:
	  if (pHelp->type != ch->class)
	    continue;
	  break;
	case HELP_CABAL:
	  if (ch->pCabal == NULL || !_is_cabal(ch->pCabal, get_cabal_vnum(pHelp->type)))
	    continue;
	  break;
	case HELP_PSALM:
	  if (IS_NPC(ch) || ch->pcdata->psalms[pHelp->type] < 1)
	    continue;
	  break;
	default:
	case HELP_ALL:
	  break;
	}
      }
      if ( is_name( argall, pHelp->keyword ) )
	{
	  if (found)
	    add_buf(output,"\n\r============================================================\n\r\n\r");
	  if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) )
	    {
	      add_buf(output,pHelp->keyword);
	      add_buf(output,"\n\r");
	    }
	  if ( pHelp->text[0] == '.' )
	    add_buf(output,pHelp->text+1);
	  else
	    add_buf(output,pHelp->text);
	  found = TRUE;
	  if (ch->desc != NULL && ch->desc->connected != CON_PLAYING)
	    break;

	}
    }
    if (!found)
      send_to_char( "No help on that word.\n\r", ch );
    else{
      char buf[MIL];
      CABAL_DATA* pCab = get_cabal( argall );
      buf[0] = '\0';
      if ( (ch->pCabal == NULL || _is_cabal( pCab, ch->pCabal)) && ch->pCabal != pCab ){
	show_cabal_help( ch, argall, buf );
	add_buf(output, buf );
      }
      page_to_char(buf_string(output),ch);
    }
    free_buf(output);
}

void do_herblore( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    BUFFER *output;
    char argall[MIL],argone[MIL];
    bool found=FALSE;
    int level;
    output = new_buf();
    argall[0] = '\0';
    while (argument[0] != '\0' )
    {
	argument = one_argument(argument,argone);
	if (argall[0] != '\0')
	    strcat(argall," ");
	strcat(argall,argone);
    }
    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
	level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;
	if (level != 99)
	    continue;
	if ( is_name( argall, pHelp->keyword ) )
	{
	    add_buf(output,pHelp->keyword);
	    add_buf(output,"\n\r");
	    if ( pHelp->text[0] == '.' )
		add_buf(output,pHelp->text+1);
	    else
		add_buf(output,pHelp->text);
	    found=TRUE;
	}
        if (ch->desc != NULL && ch->desc->connected != CON_PLAYING)
	    break;
    }
    if (!found)
	send_to_char( "There is no such herb.\n\r", ch );
    else
	page_to_char(buf_string(output),ch);
    free_buf(output);
}

void do_whois (CHAR_DATA *ch, char *argument) {
    char arg[MIL];
    BUFFER *output;
    char buf[MSL];
    CHAR_DATA *wch, *vch = NULL;
    char* race_str;
    int nMatch = 0;
    int found = FALSE;
    int race;
    int race_undead  = race_lookup("undead");
    int race_human  = race_lookup("human");

    /* Useless conditional */
    if( vch != NULL )
        return;

    one_argument(argument,arg);
    if (arg[0] == '\0')
    {
	send_to_char("You must provide a name.\n\r",ch);
	return;
    }
    output = new_buf();
    for ( wch = player_list; wch != NULL; wch = wch->next_player )
    {
	char const *class;
	if (wch->desc != NULL && wch->desc->connected != CON_PLAYING)
	    continue;
	if (!can_see(ch,wch))
	    continue;
        if ( IS_AFFECTED2(wch,AFF_SHADOWFORM) && !IS_IMMORTAL(ch) && wch != ch)
            continue;
        if ( IS_AFFECTED2(wch,AFF_TREEFORM) && !IS_IMMORTAL(ch) && wch != ch)
            continue;
	if ( IS_SET(wch->act2,PLR_NOWHO) )
	    continue;
	if (!str_prefix(arg,wch->name))
	{
	    found += 1;
	    vch = wch;
	    class = class_table[wch->class].who_name;

	    /* race setting */
	    if (IS_SET(wch->act2, PLR_MASQUERADE) && !IS_IMMORTAL(ch)
		&& ch != wch)
	      race = race_human;
	    else if (is_affected(wch, gen_unlife))
	      race = race_undead;
	    else
	      race = wch->race;

	    /* race string */
	    if (IS_QRACE(wch)){
	      race_str = wch->pcdata->race;
	    }
	    else if (race < MAX_PC_RACE)
	      race_str = (char*)pc_race_table[race].who_name;
	    else
	      race_str = "Unknw";

	    switch(wch->level)
	    {
		case MAX_LEVEL - 0 : class = "IMP";     break;
		case MAX_LEVEL - 1 : class = "CRE";     break;
		case MAX_LEVEL - 2 : class = "SUP";     break;
		case MAX_LEVEL - 3 : class = "DEI";     break;
		case MAX_LEVEL - 4 : class = "GOD";     break;
		case MAX_LEVEL - 5 : class = "IMM";     break;
		case MAX_LEVEL - 6 : class = "DEM";     break;
		case MAX_LEVEL - 7 : class = "ANG";     break;
		case MAX_LEVEL - 8 : class = "AVA";     break;
		case MAX_LEVEL - 9 : class = "HER";	break;
	    }
	    nMatch++;
            if (IS_IMMORTAL(ch) || IS_IMMORTAL(wch)
		|| IS_MASTER(wch) || ch == wch)
	      sprintf( buf, "[%2d %5s %s%s%s %s] ",
		       wch->level,
		       race_str,
		       IS_QRACE(wch) ? "(" : "",
		       IS_QRACE(wch) ? pc_race_table[race].who_name : "",
		       IS_QRACE(wch) ? ")" : "",
		       class);
            else{
	      sprintf( buf, "[   %5s %s%s%s] ",
		       race_str,
		       IS_QRACE(wch) ? "(" : "",
		       IS_QRACE(wch) ? pc_race_table[race].who_name : "",
		       IS_QRACE(wch) ? ")" : "");
	    }//end ELSE
            add_buf(output,buf);
	    if (wch->pCabal
		&& (!IS_IMMORTAL(wch) || IS_IMMORTAL(ch))
		&& ( IS_IMMORTAL(ch)
		     || IS_GAME(wch, GAME_SHOW_CABAL)
		     || is_same_cabal(ch->pCabal, wch->pCabal)) ){
	      sprintf( buf, "[%s %s(%s)] ", wch->pCabal->who_name,
		       get_crank_str(wch->pCabal, wch->pcdata->true_sex, wch->pcdata->rank),
		       rank_letters[wch->pcdata->rank]);

	      add_buf(output,buf);
	    }
	    if (!IS_NPC(wch) && wch->desc != NULL && wch->desc->pEdit != NULL)
		add_buf(output, "[OLC] ");
	    if ( IS_IMMORTAL(ch) && IS_GAME(wch, GAME_STAIN) )
		add_buf(output, "<`!STAINED``> ");
	    if ( is_pk(ch,wch) ){
	      if (is_affected(wch, gen_diplomacy))
		add_buf(output, "<`8PK``> ");
	      else
		add_buf(output, "<`1PK``> ");
	    }
	    if (IS_IMMORTAL(ch) && wch->invis_level > 1 )
	    {
		sprintf( buf, "(W %d) ", wch->invis_level);
		add_buf(output,buf);
	    }
	    if (IS_IMMORTAL(ch) && wch->incog_level > 1 )
	    {
		sprintf( buf, "(I %d) ", wch->incog_level);
		add_buf(output,buf);
	    }
	    if (HAS_CLAN(wch) && (GET_CLAN(wch) == GET_CLAN(ch) || IS_IMMORTAL(ch)) ){
	      sprintf( buf, "`3(`8%s [%s]`3)`` ",
		       GetClanName( GET_CLAN(wch)),
		       clanr_table[wch->pcdata->clan_rank][0]);
	      add_buf(output,buf);
	    }
	    if (IS_IMMORTAL(ch) && IS_GAME(wch, GAME_PKWARN))
	    {
		sprintf( buf, "[`!W``] ");
		add_buf(output,buf);
	    }
	    if ( (IS_IMMORTAL(ch) || ch == wch )&& IS_SET(wch->act2, PLR_DESC))
	    {
		sprintf( buf, "[`#D``] ");
		add_buf(output,buf);
	    }
	    if ( (IS_IMMORTAL(ch) || ch == wch )&& IS_SET(wch->act2, PLR_MASQUERADE))
	    {
	      sprintf( buf, "`%%[`5MASQUE`%%]`` ");
	      add_buf(output,buf);
	    }
	    if (wch->pcdata->pbounty
		&& ch->pCabal
		&& (IS_CABAL(get_parent(ch->pCabal), CABAL_ROUGE) || IS_CABAL(get_parent(ch->pCabal), CABAL_SPIES)) )
            {
	      if (is_bounty_priority( wch->pcdata->pbounty, ch->pCabal))
		sprintf( buf, "`3(`!$!`3)`` ");
	      else
		sprintf( buf, "`3(`#$`3)`` ");
	      add_buf(output,buf);
	    }
    	    if ((wch->sex == 1 ) && (IS_GAME(ch, GAME_SEX)))
	    {
	        sprintf( buf, "[`$Male``] ");
		add_buf(output,buf);
	    }
            if ((wch->sex == 2 ) && (IS_GAME(ch, GAME_SEX)))
            {
                sprintf( buf, "[`!Female``] ");
                add_buf(output,buf);
            }
            if ((wch->sex == 0 ) && (IS_GAME(ch, GAME_SEX)))
            {
                sprintf( buf, "[`3Neuter``] ");
                add_buf(output,buf);
            }
	    if ( IS_IMMORTAL(ch) && IS_SET(wch->act, PLR_DOOF))
	    {
		sprintf( buf, "(FORSAKEN) ");
		add_buf(output,buf);
	    }
	    if ( IS_SET(wch->act2, PLR_CODING))
	    {
		sprintf( buf, "`3{`#CODING`3}`` ");
		add_buf(output,buf);
	    }
	    if ( IS_SET(wch->act2, PLR_ROYAL))
	      {
		sprintf( buf, "`3<`#ROYAL`3>`` ");
		add_buf(output,buf);
	      }
	    if (IS_SET(wch->act2, PLR_NOBLE))
            {
		sprintf( buf, "`3<`!NOBLE`3>`` ");
		add_buf(output,buf);
	    }
	    if (IS_SET(wch->comm, COMM_AFK))
	    {
		sprintf( buf, "[AFK] ");
		add_buf(output,buf);
	    }
	    if (IS_OUTLAW( wch )){
	      sprintf( buf, "(`#OUTLAW``) ");
	      add_buf(output,buf);
	    }
            if (IS_SET(wch->act, PLR_WANTED))
            {
                sprintf( buf, "(`1WANTED``) ");
                add_buf(output,buf);
            }
            if (IS_SET(wch->act, PLR_OUTCAST))
            {
                sprintf( buf, "(`1OUTCAST``) ");
                add_buf(output,buf);
            }
	    if ( wch->pCabal && IS_GAME(wch, GAME_SHOW_CABAL) && !IS_IMMORTAL(wch)){
	      if (!IS_NULLSTR(wch->pcdata->member->sponsor) && !IS_NULLSTR(wch->pCabal->pIndexData->sprefix)){
		sprintf( buf, "%s ", wch->pCabal->pIndexData->sprefix);
		add_buf(output,buf);
	      }
	      else if (wch->sex == 1 && !IS_NULLSTR(wch->pCabal->pIndexData->mprefix)){
		sprintf( buf, "%s ", wch->pCabal->pIndexData->mprefix);
		add_buf(output,buf);
	      }
	      else if (wch->sex == 2 && !IS_NULLSTR(wch->pCabal->pIndexData->fprefix)){
		sprintf( buf, "%s ", wch->pCabal->pIndexData->fprefix);
		add_buf(output,buf);
	      }
	      else if (!IS_NULLSTR(wch->pCabal->pIndexData->mprefix)){
		sprintf( buf, "%s ", wch->pCabal->pIndexData->mprefix);
		add_buf(output,buf);
	      }
	    }

/* NAME AND TITLE HERE */
	    sprintf( buf, "%s%s%s%s",
		     wch->name, wch->pcdata->family[0] == '\0' ? "" : " ",
		     wch->pcdata->family[0] == '\0' ? "" : wch->pcdata->family,
		     IS_NPC(wch) ? "" : wch->pcdata->title );
            add_buf(output,buf);
	    sprintf( buf, "\n\r");
	    add_buf(output,buf);
	}
    }
    sprintf(buf,"Players matched: %d\n\r", nMatch );
    add_buf(output,buf);
    if (found < 1)
    {
	send_to_char("No one of that name is playing.\n\r",ch);
	return;
    }
    page_to_char(buf_string(output),ch);
    free_buf(output);
}

void do_who( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    CHAR_DATA *wch;
    AFFECT_DATA* paf;
//    DESCRIPTOR_DATA *d;
    BUFFER *output;
    int iRace, nMatch = 0, count = 0;
    bool rgfRace[MAX_PC_RACE];
    bool fRaceRestrict = FALSE, fCabalOnly = FALSE,
      fImmortalOnly = FALSE, fPKOnly = FALSE, fGroupOnly = FALSE,
      fWantedOnly = FALSE, fOutlawOnly = FALSE,fNewbieOnly = FALSE,
      fClassOnly = FALSE, fDescOnly = FALSE, fBountyOnly = FALSE,
      fAvatarOnly = FALSE, fClanOnly = FALSE, fWarnOnly = FALSE,
      fReligionOnly = FALSE;
    bool fQuest = FALSE;

    int contra = 0, race, race_human = race_lookup("human"),
      race_undead = race_lookup("undead");
    char* race_str;

    /* Useless conditional */
    if( fOutlawOnly == TRUE )
        return;

    for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
	rgfRace[iRace] = FALSE;
    for ( ; ; )
    {
	char arg[MSL];
	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	    break;
	else
	{
	    if (!str_prefix(arg,"immortals"))		fImmortalOnly = TRUE;
	    else if (!str_prefix(arg,"pk"))		fPKOnly = TRUE;
	    else if (!str_prefix(arg,"group"))		fGroupOnly = TRUE;
	    else if (!str_prefix(arg,"cabal"))		fCabalOnly = TRUE;
	    else if (!str_prefix(arg,"clan"))		fClanOnly = TRUE;
	    else if (!str_prefix(arg,"wanted"))		fWantedOnly = TRUE;
	    else if (!str_prefix(arg,"outlaw"))		fOutlawOnly = TRUE;
	    else if (!str_prefix(arg,"newbie"))		fNewbieOnly = TRUE;
	    else if (!str_prefix(arg,"class"))		fClassOnly = TRUE;
	    else if (!str_prefix(arg,"religion"))	fReligionOnly = TRUE;
	    else if (!str_prefix(arg,"description"))	fDescOnly = IS_IMMORTAL(ch);
	    else if (!str_prefix(arg,"warned"))		fWarnOnly = IS_IMMORTAL(ch);
	    else if (!str_prefix(arg,"avatar"))		fAvatarOnly = (IS_IMMORTAL(ch) || IS_AVATAR(ch));
	    else if (!str_prefix(arg,"bounty"))
	      fBountyOnly = (ch->pCabal && (IS_CABAL(get_parent(ch->pCabal), CABAL_ROUGE) || IS_CABAL(get_parent(ch->pCabal), CABAL_SPIES)));
	    else
	    {
		iRace = race_lookup(arg);
                if (iRace == -1 || iRace >= MAX_PC_RACE)
		{
                    send_to_char("That's not a valid race.\n\r",ch);
                    return;
		}
		else
		{
		    fRaceRestrict = TRUE;
		    rgfRace[iRace] = TRUE;
		}
	    }
	}
    }
    buf[0] = '\0';
    output = new_buf();
    for ( wch = player_list; wch != NULL; wch = wch->next_player )
    {
      int fake_class = wch->class;
      char const *class;
      char title_buf[MIL];

      if (!(IS_IMMORTAL(wch) && !can_see(ch,wch)))
	count++;
      if (!can_see(ch,wch))
	continue;
      if ( IS_SET(wch->act2,PLR_NOWHO) )
	continue;
      if (wch->desc != NULL && wch->desc->connected != CON_PLAYING)
	continue;
      if ( IS_AFFECTED2(wch,AFF_SHADOWFORM) && !IS_IMMORTAL(ch))
	continue;
      if ( IS_AFFECTED2(wch,AFF_TREEFORM) && !IS_IMMORTAL(ch))
	continue;

      /* select race etc */
      if (IS_SET(wch->act2, PLR_MASQUERADE) && !IS_IMMORTAL(ch)
	  && ch != wch)
	race = race_human;
      else if (is_affected(wch, gen_unlife))
	race = race_undead;
      else
	race = wch->race;

      /* race string */
      if (IS_QRACE(wch)){
	race_str = wch->pcdata->race;
      }
      else if (race < MAX_PC_RACE)
	race_str = (char*)pc_race_table[race].who_name;
      else
	race_str = "Unknw";

      if (fClassOnly && !IS_IMMORTAL(ch)){
	if ( (paf = affect_find(wch->affected, gen_guise)) != NULL)
	  fake_class = paf->modifier;
	else if (IS_MASQUE(wch) && !IS_IMMORTAL(ch) && wch != ch)
	  fake_class = class_lookup("dark-knight");
      }
      if ( ( fImmortalOnly && wch->level < LEVEL_IMMORTAL )
	   || ( fPKOnly && !is_pk(ch,wch) )
	   || ( fGroupOnly && (ch->level+8 < wch->level
			       || ch->level-8 > wch->level
			       || (IS_GOOD(ch) && IS_EVIL(wch))
			       || (IS_GOOD(wch) && IS_EVIL(ch))))
	   || ( fNewbieOnly && is_pk(wch,wch) && !IS_GAME(wch, GAME_NEWBIE))
	   || ( fWantedOnly && !IS_SET(wch->act, PLR_WANTED) )
	   || ( fCabalOnly
		&& (wch->pCabal == NULL
		    || IS_IMMORTAL(wch)
		    || !is_same_cabal(ch->pCabal, wch->pCabal)))
	   || ( fClanOnly && GET_CLAN(ch) != GET_CLAN(wch))
	   || ( fRaceRestrict && !rgfRace[race])
	   || ( fClassOnly && ch->class != fake_class)
	   || ( fDescOnly && !IS_SET(wch->act2, PLR_DESC))
	   || ( fWarnOnly && !IS_GAME(wch, GAME_PKWARN))
	   || ( fBountyOnly && !IS_NPC(wch) && wch->pcdata->pbounty == NULL)
	   || ( fReligionOnly && !IS_NPC(wch)
		&& wch->pcdata->way != ch->pcdata->way)
	   || ( fAvatarOnly && !IS_AVATAR(wch)) )
	continue;
      nMatch++;

      /* count how many contracts */
      if (!IS_NPC(wch)
	  && wch->pcdata->pbounty
	  && wch != ch
	  && is_pk_abs(ch, wch)
	  && !bounty_covered( wch ))
	contra++;

      class = class_table[fake_class].who_name;
      switch ( wch->level )
	{
	default: break;
	  {
	  case MAX_LEVEL - 0 : class = "IMP";     break;
	  case MAX_LEVEL - 1 : class = "CRE";     break;
	  case MAX_LEVEL - 2 : class = "SUP";     break;
	  case MAX_LEVEL - 3 : class = "DEI";     break;
	  case MAX_LEVEL - 4 : class = "GOD";     break;
	  case MAX_LEVEL - 5 : class = "IMM";     break;
	  case MAX_LEVEL - 6 : class = "DEM";     break;
	  case MAX_LEVEL - 7 : class = "ANG";     break;
	  case MAX_LEVEL - 8 : class = "AVA";     break;
	  case MAX_LEVEL - 9 : class = "HER";     break;
	  }
	}
      if (IS_IMMORTAL(ch) || IS_IMMORTAL(wch) || IS_MASTER(wch) || ch == wch)
	sprintf( buf, "[%2d %5s %s] ",
		 wch->level,
		 race_str,
		 class);
      else{
	sprintf( buf, "[   %5s    ] ", race_str );
      }//end ELSE
      add_buf(output,buf);
      if (!IS_NPC(wch) && wch->desc != NULL && wch->desc->pEdit != NULL)
	add_buf(output, "[OLC] ");
      if ( IS_IMMORTAL(ch) && IS_GAME(wch, GAME_STAIN) )
	add_buf(output, "<`!STAINED``> ");
      if ( is_pk(ch,wch) ){
	if (is_affected(wch, gen_diplomacy))
		add_buf(output, "<`8PK``> ");
	else
	  add_buf(output, "<`1PK``> ");
      }
      if (IS_IMMORTAL(ch) && wch->invis_level > 1 )
	{
	  sprintf( buf, "(W %d) ", wch->invis_level);
	  add_buf(output,buf);
	}
      if (IS_IMMORTAL(ch) && wch->incog_level > 1 )
	{
	  sprintf( buf, "(I %d) ", wch->incog_level);
	  add_buf(output,buf);
	}
      if (wch->pCabal
	  && (!IS_IMMORTAL(wch) || IS_IMMORTAL(ch))
	  && (IS_IMMORTAL(ch)
	      || IS_GAME(wch, GAME_SHOW_CABAL)
	      || is_same_cabal(ch->pCabal, wch->pCabal)) ){
	CABAL_DATA* pPar = get_parent(wch->pCabal);
	sprintf( buf, "[%s] ", pPar->who_name);
	add_buf(output,buf);
      }
      if ( IS_IMMORTAL(ch) && IS_GAME(wch, GAME_PKWARN))
	{
	  sprintf( buf, "[`!W``] ");
	  add_buf(output,buf);
	}
      if ( (IS_IMMORTAL(ch) || ch == wch )&& IS_SET(wch->act2, PLR_DESC))
	{
	  sprintf( buf, "[`#D``] ");
	  add_buf(output,buf);
	}

      if ( (IS_IMMORTAL(ch) || ch == wch )&& IS_SET(wch->act2, PLR_MASQUERADE))
	{
	  sprintf( buf, "`%%[`5MASQUE`%%]`` ");
	  add_buf(output,buf);
	}
      if (wch->pcdata->pbounty
	  && ch->pCabal
	  && (IS_CABAL(get_parent(ch->pCabal), CABAL_ROUGE) || IS_CABAL(get_parent(ch->pCabal), CABAL_SPIES)) )
      {
	if (is_bounty_priority( wch->pcdata->pbounty, ch->pCabal))
	  sprintf( buf, "`3(`!$`3)`` ");
	else
	  sprintf( buf, "`3(`#$`3)`` ");
	  add_buf(output,buf);
        }
      if (HAS_CLAN(wch) && (GET_CLAN(wch) == GET_CLAN(ch) || IS_IMMORTAL(ch)) ){
	sprintf( buf, "`3(`8%s [%s]`3)`` ",
		 GetClanName( GET_CLAN(wch)),
		 clanr_table[wch->pcdata->clan_rank][0]);
	add_buf(output,buf);
      }
      if ( IS_IMMORTAL(ch) && IS_SET(wch->act, PLR_DOOF))
	{
	  sprintf( buf, "(FORSAKEN) ");
	  add_buf(output,buf);
	}
      if (  IS_SET(wch->act2, PLR_CODING))
	{
	  sprintf( buf, "`3{`#CODING`3}`` ");
	  add_buf(output,buf);
	}
      //NOBLE and ROYAL added here. -Shadow
      if ( IS_SET(wch->act2, PLR_ROYAL))
	{
	  sprintf( buf, "`3<`#ROYAL`3>`` ");
	  add_buf(output,buf);
	}
      if (IS_SET(wch->act2, PLR_NOBLE))
	{
	  sprintf( buf, "`3<`!NOBLE`3>`` ");
	  add_buf(output,buf);
	}
      if (IS_SET(wch->comm, COMM_AFK))
        {
	  sprintf( buf, "[AFK] ");
	  add_buf(output,buf);
        }
      if (IS_OUTLAW( wch )){
	sprintf( buf, "(`#OUTLAW``) ");
	add_buf(output,buf);
      }
      if (IS_SET(wch->act, PLR_WANTED))
        {
	  sprintf( buf, "(`1WANTED``) ");
	  add_buf(output,buf);
        }
      if (IS_SET(wch->act, PLR_OUTCAST))
        {
	  sprintf( buf, "(`1OUTCAST``) ");
	  add_buf(output,buf);
        }
/* reset the title buffer to empty in case the sponsor title needs to be used */
      title_buf[0] = '\0';
      if ( wch->pCabal && IS_GAME(wch, GAME_SHOW_CABAL) && !IS_IMMORTAL(wch)){
	if (!IS_NULLSTR(wch->pcdata->member->sponsor) && !IS_NULLSTR(wch->pCabal->pIndexData->sprefix)){
	  sprintf( title_buf, " the %s of %s", wch->pCabal->pIndexData->sprefix, wch->pcdata->member->sponsor);
	}
	else if (wch->sex == 1 && !IS_NULLSTR(wch->pCabal->pIndexData->mprefix)){
	  sprintf( buf, "%s ", wch->pCabal->pIndexData->mprefix);
	  add_buf(output,buf);
	}
	else if (wch->sex == 2 && !IS_NULLSTR(wch->pCabal->pIndexData->fprefix)){
	  sprintf( buf, "%s ", wch->pCabal->pIndexData->fprefix);
	  add_buf(output,buf);
	}
	else if (!IS_NULLSTR(wch->pCabal->pIndexData->mprefix)){
	  sprintf( buf, "%s ", wch->pCabal->pIndexData->mprefix);
		add_buf(output,buf);
	}
      }

      if (!IS_NULLSTR(mud_data.questor) && mud_data.questor[0] == wch->name[0] && !str_cmp(wch->name, mud_data.questor))
	fQuest = TRUE;
      else
	fQuest = FALSE;

      sprintf( buf, "%s%s%s%s%s%s",
	       fQuest ? "`#" : "",
	       wch->name,
	       fQuest ? "``" : "",
	       wch->pcdata->family[0] == '\0' ? "" : " ",
	       wch->pcdata->family[0] == '\0' ? "" : wch->pcdata->family,
	       IS_NPC(wch) ? "" : title_buf[0] == '\0' ? wch->pcdata->title : title_buf);
      add_buf(output,buf);
      sprintf( buf, "\n\r");
      add_buf(output,buf);
    }
    if ( argument[0] == '\0' )
      {
        sprintf(buf,"Players matched: %d\n\r", nMatch );
        add_buf(output,buf);
      }
/*    for ( d = descriptor_list; d != NULL; d = d->next )
      if (d->connected != CON_PLAYING)
      count++; */
    mud_data.max_on = UMAX(count,mud_data.max_on);
    if (mud_data.max_on == count)
      sprintf(buf,"There %s %d character%s on, the most so far since startup.\n\r",
	      count == 1 ? "is" : "are", count,count == 1 ? "" : "s" );
    else
      sprintf(buf,"There %s %d character%s on, the most on since startup was %d.\n\r",
	      count == 1 ? "is" : "are", count,count == 1 ? "" : "s", mud_data.max_on );
    add_buf(output,buf);
    sprintf( buf, "The title of `#Master Questor`` of -Mirlan- is currently held by %s.\n\r", mud_data.questor);
    add_buf(output,buf);
    if (contra){
      sprintf( buf, "There are %d contracts avaliable to you.\n\r", contra);
      add_buf(output,buf);
    }
    page_to_char( buf_string(output), ch );
    free_buf(output);
}

void do_count ( CHAR_DATA *ch, char *argument )
{
    int count = 0;
    CHAR_DATA *victim;
//    DESCRIPTOR_DATA *d;
    for ( victim = player_list; victim != NULL; victim = victim->next_player )
    {
	if (IS_IMMORTAL(victim) && !can_see( ch, victim ) )
	    continue;
        count++;
    }
/*    for ( d = descriptor_list; d != NULL; d = d->next )
	if (d->connected != CON_PLAYING)
	    count++; */
    mud_data.max_on = UMAX(count,mud_data.max_on);
    if (mud_data.max_on == count)
        sendf(ch,"\n\rThere %s %d character%s on, the most so far since startup.\n\r",
	  count == 1 ? "is" : "are", count,count == 1 ? "" : "s" );
    else
        sendf(ch,"\n\rThere %s %d character%s on, the most on since startup was %d.\n\r",
	  count == 1 ? "is" : "are", count,count == 1 ? "" : "s", mud_data.max_on );
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You are carrying:\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
}

void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;
    int i;
    bool found = FALSE;

    /* show eq */
    show_equipment(ch, ch, FALSE);
    return;
    send_to_char( "You are using:\n\r", ch );
    for (i = 0; i < sizeof(eq_positions)/sizeof(eq_positions[0]); i++)
    {
        if (i == 17 && is_affected(ch, gsn_body_weaponry))
	{
            sendf( ch, "%s hands shaped like weapons\n\r", where_name[eq_positions[i]]);
	    found = TRUE;
	    continue;
	}
	else if ( ( obj = get_eq_char( ch, eq_positions[i] ) ) == NULL )
	    continue;
        if (i == 17 && is_affected(ch, gsn_graft_weapon))
            sendf( ch, "<grafted to hands>  ");
	else
	    send_to_char( where_name[eq_positions[i]], ch );
	if ( can_see_obj( ch, obj ) )
	    sendf(ch, "%s\n\r", format_obj_to_char( obj, ch, TRUE ));
	else
	    send_to_char( "something.\n\r", ch );
	found = TRUE;
    }
    if ( !found )
	send_to_char( "Nothing.\n\r", ch );
    return;
}

void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL];
    OBJ_DATA *obj1, *obj2;
    int value1 = 0, value2 = 0;
    char *msg = NULL;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }
    if ( ( obj1 = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }
    if (arg2[0] == '\0')
    {
	for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
	    if (obj2->wear_loc != WEAR_NONE && can_see_obj(ch,obj2) && obj1->item_type == obj2->item_type
		&& (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE & ~ITEM_UNIQUE & ~ITEM_RARE & ~ITEM_PARRY) != 0 )
                break;
	if (obj2 == NULL)
	{
	    send_to_char("You aren't wearing anything comparable.\n\r",ch);
	    return;
	}
    }
    else if ( (obj2 = get_obj_carry(ch,arg2,ch) ) == NULL )
    {
	send_to_char("You do not have that item.\n\r",ch);
	return;
    }
    if ( obj1 == obj2 )
	msg = "You compare $p to itself.  It looks about the same.";
    else if ( obj1->item_type != obj2->item_type )
	msg = "You can't compare $p and $P.";
    else
	switch ( obj1->item_type )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;
	case ITEM_ARMOR:
	    value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
	    break;
	case ITEM_WEAPON:
	    if (obj1->pIndexData->new_format)
		value1 = (1 + obj1->value[2]) * obj1->value[1];
	    else
		value1 = obj1->value[1] + obj1->value[2];
	    if (obj2->pIndexData->new_format)
		value2 = (1 + obj2->value[2]) * obj2->value[1];
	    else
		value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    if ( msg == NULL )
    {
        if      ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }
    act( msg, ch, obj1, obj2, TO_CHAR );
}

void where( CHAR_DATA* ch, AREA_DATA* area, char* argument ){
  char arg[MIL];
  char buf[MIL], name[MIL];
  CHAR_DATA *victim;
  bool found = FALSE;
  one_argument( argument, arg );
  buf[0] = 0;

  if ( arg[0] == '\0' ){
    for ( victim = player_list; victim != NULL; victim = victim->next){
      if (!IS_NPC(victim)
	  && victim->in_room->area == area
	  && !IS_IMMORTAL(victim) && victim->in_room != NULL
	  && !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
	  && !room_is_private(victim->in_room)
	  && (!IS_AFFECTED2(victim,AFF_SHADOWFORM)
	      || IS_IMMORTAL(ch))
	  && !IS_AFFECTED2(victim,AFF_TREEFORM)
	  && !is_affected(victim,gsn_bat_form)
	  && !is_affected(victim,gsn_coffin)
	  && !is_affected(victim,gsn_entomb)
	  && !IS_AFFECTED2(victim,AFF_CATALEPSY)
	  && (can_see( ch, victim )
	      || (check_avenger(ch, victim) == CRUSADE_MATCH
		  && !is_affected(victim, gen_d_shroud))
	      || (!can_see(ch,victim) && IS_AFFECTED2(ch,AFF_SENSE_EVIL)
		  && IS_EVIL(victim)))
	  ){
	found = TRUE;
	buf[0] = 0;
	if (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_ROUGE)
	    && !IS_NPC(victim)
	    && victim->pcdata->pbounty)
	  sprintf( name, "`#$``");
	else
	  sprintf( name, "`@ ``");

	if (!can_see(ch,victim)
	    && IS_AFFECTED2(ch,AFF_SENSE_EVIL) && IS_EVIL(victim)
	    && (victim->invis_level < 1 || victim->incog_level < 1)){
	  sprintf( name + strlen(name), "%s", "(Red Aura) Someone");
	}
	else{
	  sprintf( name + strlen(name), "%s", PERS2(victim));
	}
	if (IS_WANTED(victim)){
	  sprintf(buf, "%12s", "(`!WANTED``)");

	}
	else
	  sprintf(buf, "%12s", "`!``");
	if (IS_SET(victim->in_room->area->area_flags, AREA_LAWFUL)){
	  /* justice */
	  if ( victim->pCabal
	       && IS_GAME(victim, GAME_SHOW_CABAL)
	       && IS_CABAL(victim->pCabal, CABAL_JUSTICE)
	       ){
	    sprintf(buf + strlen(buf), "[%s]",
		    get_parent(victim->pCabal)->who_name);
	  }
	  /* royal/nobles */
	  else if (IS_ROYAL(victim)){
	    sprintf(buf + strlen(buf), "[%s]", "`#ROYAL``");
	  }
	  else if (IS_NOBLE(victim)){
	    sprintf(buf + strlen(buf), "[%s]", "`#NOBLE``");
	  }
	}
	else
	  sprintf(buf + strlen(buf), "`!``");

	if (victim->invis_level < 1 || victim->incog_level < 1)
	  sendf( ch, "%s %-18s %-26s %s\n\r",
		 is_pk(ch,victim) ? is_affected(victim, gen_diplomacy) ? "<`8PK``> " : "<`1PK``> " : "     ",
		 name, buf, victim->in_room->name);
      }
    }
    if ( !found )
      send_to_char( "None\n\r", ch );
  }
  else if ( !str_cmp(arg,"pk") ){
    found = FALSE;
    for ( victim = player_list; victim != NULL; victim = victim->next_player ){
      if ((!IS_NPC(victim) || is_affected(victim,gsn_doppelganger))
	  && is_pk(ch,victim) && !IS_IMMORTAL(victim) && victim->in_room != NULL
	  && !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
	  && !room_is_private(victim->in_room)
	  && victim->in_room->area == area
	  && !is_affected(victim,gsn_bat_form)
	  && !is_affected(victim,gsn_coffin)
	  && !is_affected(victim,gsn_entomb)
	  && !IS_AFFECTED2(victim,AFF_CATALEPSY)
	  && (!IS_AFFECTED2(victim,AFF_SHADOWFORM) || IS_IMMORTAL(ch))
	  && !IS_AFFECTED2(victim,AFF_TREEFORM)
	  && can_see( ch, victim ) )
	{
	  found = TRUE;
	  buf[0] = 0;
	  if (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_ROUGE)
	      && !IS_NPC(victim)
	      && victim->pcdata->pbounty)
	    sprintf( name, "`#$``");
	  else
	    sprintf( name, "`# ``");
	  sprintf( name + strlen(name), "%s", PERS2(victim));

	  if (IS_WANTED(victim)){
	    sprintf(buf, "%12s", "(`!WANTED``)");
	  }
	  else
	    sprintf(buf, "%12s", "`!``");
	  if (IS_SET(victim->in_room->area->area_flags, AREA_LAWFUL)){
	    /* justice */
	    if ( victim->pCabal
		 && IS_GAME(victim, GAME_SHOW_CABAL)
		 && IS_CABAL(victim->pCabal, CABAL_JUSTICE)
		 ){
	      sprintf(buf + strlen(buf), "[%s]",
		      get_parent(victim->pCabal)->who_name);
	    }
	    /* royal/nobles */
	    else if (IS_ROYAL(victim)){
	      sprintf(buf + strlen(buf), "[%s]", "`#ROYAL``");
	    }
	    else if (IS_NOBLE(victim)){
	      sprintf(buf + strlen(buf), "[%s]", "`#NOBLE``");
	    }
	  }
	  else
	    sprintf(buf + strlen(buf), "`!``");
	  sendf( ch, "%s %-18s %-26s %s\n\r",
		 is_pk(ch,victim) ? is_affected(victim, gen_diplomacy) ? "<`8PK``> " : "<`1PK``> " : "     ",
		 name, buf, victim->in_room->name);
	}
    }
    if ( !found )
      send_to_char( "None\n\r", ch );
  }
  else{
    found = FALSE;
    for ( victim = char_list; victim != NULL; victim = victim->next ){
      if ( victim->in_room != NULL && victim->in_room->area == area
	   && victim->race != race_lookup("elf") && victim->race != race_lookup("drow")
	   && !is_affected(victim,gsn_bat_form)
	   && !is_affected(victim,gsn_coffin)
	   && !is_affected(victim,gsn_entomb)
	   && !is_affected(victim,gen_conceal)
	   && !IS_AFFECTED2(victim,AFF_CATALEPSY)
	   && !IS_IMMORTAL(victim)
	   && !IS_AFFECTED2(victim,AFF_SHADOWFORM)
	   && !(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_SNEAK))
	   && !IS_AFFECTED2(victim,AFF_TREEFORM)
	   && is_name( arg, victim->name )
	   && can_see( ch, victim ) )
	{
	  found = TRUE;
	  sprintf( name, "%s", PERS2(victim));

	  if (IS_WANTED(victim)){
	    sprintf(buf, "%12s", "(`!WANTED``)");
	  }
	  else
	    sprintf(buf, "%12s", "`!``");
	  if (IS_SET(victim->in_room->area->area_flags, AREA_LAWFUL)){
	    /* justice */
	    if ( victim->pCabal
		 && IS_GAME(victim, GAME_SHOW_CABAL)
		 && IS_CABAL(victim->pCabal, CABAL_JUSTICE)
		 ){
	      sprintf(buf + strlen(buf), "[%s]",
		      get_parent(victim->pCabal)->who_name);
	    }
	    /* royal/nobles */
	    else if (IS_ROYAL(victim)){
	      sprintf(buf + strlen(buf), "[%s]", "`#ROYAL``");
	    }
	    else if (IS_NOBLE(victim)){
	      sprintf(buf + strlen(buf), "[%s]", "`#NOBLE``");
	    }
	  }
	  else
	    sprintf(buf + strlen(buf), "`!``");
	  sendf( ch, "%s %-12s %-26s %s\n\r",
		 is_pk(ch,victim) ? is_affected(victim, gen_diplomacy) ? "<`8PK``> " : "<`1PK``> " : "     ",
		 name, buf, victim->in_room->name);
	  break;
	}
    }
    if ( !found )
      sendf(ch, "You didn't find any %s.\n\r", arg);
  }
}

void do_where( CHAR_DATA *ch, char *argument ){
  if(  (IS_AFFECTED(ch, AFF_BLIND) && !is_affected(ch, gsn_bat_form))
       || (IS_AFFECTED(ch, AFF_BLIND) && bat_blind(ch))  || bat_blind(ch) )
    {
      send_to_char( "You can't see a thing!\n\r", ch );
      return;
    }
  if (IS_AFFECTED2(ch,AFF_TERRAIN))
    {
      send_to_char( "Darkness.. Darkness everywhere!\n\r",ch);
      return;
    }
  if (IS_SET(ch->in_room->room_flags,ROOM_NOWHERE) && !IS_IMMORTAL(ch))
    {
      send_to_char( "You can't see the area out of this room.\n\r",ch);
      return;
    }
  if (ch->in_room->area->pCabal)
    sendf(ch, "[%s] controlled.  Players near you:\n\r", ch->in_room->area->pCabal->who_name);
  else
    send_to_char( "Players near you:\n\r", ch );
  where( ch, ch->in_room->area, argument );
}

void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    char *msg;
    CHAR_DATA *victim;
    int diff;
    one_argument( argument, arg );
    if (IS_AFFECTED2(ch, AFF_SHADOWFORM))
    {
	REMOVE_BIT(ch->affected2_by, AFF_SHADOWFORM);
	affect_strip(ch, gsn_shadowform);
	act("Your body regains its substance, and you materialize into existence.",ch,NULL,NULL,TO_CHAR);
	act("$n's body regains its substance, and $e materializes into existence.",ch,NULL,NULL,TO_ROOM);
	WAIT_STATE(ch, 24);
    }
    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }
    if (can_see(victim, ch))
      act("$n seems to measure you up and consider your size.", ch, NULL, victim, TO_VICT);
    diff = victim->level - ch->level;
    if (IS_NPC(victim) && IS_SET(victim->act, ACT_NONCOMBAT)) msg = "You cannot attack $N directly.";
    else if ( diff <= -10 ) msg = "You can kill $N naked and weaponless.";
    else if ( diff <=  -5 ) msg = "$N is no match for you.";
    else if ( diff <=  -2 ) msg = "$N looks like an easy kill.";
    else if ( diff <=   1 ) msg = "The perfect match!";
    else if ( diff <=   4 ) msg = "You would need a little luck.";
    else if ( diff <=   9 ) msg = "You would need A LOT of luck!";
    else if ( diff <=  16 ) msg = "You would be throwing your life away.";
    else if ( diff <=  25 ) msg = "Don't bother, you wouldn't even be able to scratch $N.";
    else                    msg = "Why don't you just try to kill a god?";
    act( msg, ch, NULL, victim, TO_CHAR );

if (IS_GOOD(ch))
      {
    if (IS_GOOD(victim))
        sendf(ch, "This %s greets you warmly.\n\r", race_table[victim->race].name);
    else if (IS_NEUTRAL(victim))
        sendf(ch, "This %s looks very disinterrested.\n\r", race_table[victim->race].name);
    else    sendf(ch, "This %s grins at you evilly.\n\r", race_table[victim->race].name);
      }
    else if (IS_NEUTRAL(ch))
    {
      if (IS_GOOD(victim))
        sendf(ch, "This %s beams a smile at you.\n\r", race_table[victim->race].name);
    else if (IS_NEUTRAL(victim))
        sendf(ch, "This %s looks just as disinterrested as you.\n\r", race_table[victim->race].name);
    else   sendf(ch, "This %s grins at you evilly.\n\r", race_table[victim->race].name);
    }
    else
      {
    if (IS_GOOD(victim))
        sendf(ch, "This %s smiles, hoping you will turn from your evil path.\n\r", race_table[victim->race].name);
    else if (IS_NEUTRAL(victim))
        sendf(ch, "This %s looks very disinterrested.\n\r", race_table[victim->race].name);
    else     sendf(ch, "This %s grins evilly with you.\n\r", race_table[victim->race].name);
      }


    if (ch->size == victim->size)
      msg = "$N is about the same size as you.";
    else if (ch->size - victim->size < -1 )
      msg = "$N is much larger then you.";
    else if (ch->size - victim->size > 1 )
      msg = "$N is much smaller then you.";
    else if (ch->size - victim->size < 0 )
      msg = "$N is larger then you.";
    else if (ch->size - victim->size > 0 )
      msg = "$N is smaller then you.";
    act( msg, ch, NULL, victim, TO_CHAR);
}

void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MSL];
    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }
    if (!str_cmp(title, "default")){
      if (ch->class == class_lookup("crusader"))
	sprintf(buf, " %s", get_crus_title(ch, ch->level));
      else
	sprintf( buf, " the %s", title_table [ch->class] [ch->level] [ch->pcdata->true_sex == SEX_FEMALE ? 1 : 0] );
    }
    else if ( title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?' )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
      strcpy( buf, title );
    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
}

void do_description( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    if ( argument[0] != '\0' )
    {
	buf[0] = '\0';
	smash_tilde( argument );
	if ( !str_prefix("format", argument)){
	  ch->description = format_string(ch->description);
	  send_to_char("Description formatted.\n\r", ch);
	  return;
	}
	if ( !str_cmp("edit", argument))
	  {
	    string_append( ch, &ch->description );
	    return;
	  }
	if (argument[0] == '-')
	{
	    int len;
	    bool found = FALSE;
	    if (ch->description == NULL || ch->description[0] == '\0')
	    {
		send_to_char("No lines left to remove.\n\r",ch);
		return;
	    }
	    strcpy(buf,ch->description);
	    for (len = strlen(buf); len > 0; len--)
		if (buf[len] == '\r')
		{
                    if (!found)
		    {
			if (len > 0)
			    len--;
			found = TRUE;
		    }
                    else
		    {
			buf[len + 1] = '\0';
			free_string(ch->description);
			ch->description = str_dup(buf);
			sendf(ch, "Your description is:\n\r%s", ch->description ? ch->description : "(None).\n\r");
			return;
		    }
		}
	    buf[0] = '\0';
	    free_string(ch->description);
	    ch->description = str_dup(buf);
	    send_to_char("Description cleared.\n\r",ch);
	    return;
	}
	if ( argument[0] == '+' )
	{
	    if ( ch->description != NULL )
		strcat( buf, ch->description );
	    argument++;
	    while ( isspace(*argument) )
		argument++;
	}
	if ( strlen(buf) + strlen(argument) >= MSL)
	{
	    send_to_char( "Description too long.\n\r", ch );
	    return;
	}
	if ( strlen(argument) > MIL)
	{
	    send_to_char("Line too long.\n\r",ch);
	    return;
	}
	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }
    sendf(ch, "Your description is:\n\r%s", ch->description ? ch->description : "(None).\n\r");
}

void do_report( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    int hit, mana;
    if (ch->max_hit == 0)
        ch->max_hit++;
    if (ch->max_mana == 0)
        ch->max_mana++;
    if (ch->max_move == 0)
        ch->max_move++;
    hit = ch->hit*100/ch->max_hit;
    mana = ch->mana*100/ch->max_mana;
    sprintf( buf,"%s while %s.",
	     hit > 99 ? "I'm in excellent health" :
	     hit > 90 ? "I have few scratches" :
	     hit > 75 ? "I have small wounds" :
	     hit > 50 ? "I'm seriously hurt" :
	     hit > 30 ? "I have terrible wounds" :
	     hit > 15 ? "I'm hurt badly" : "Death is upon me",
	     mana > 99 ? "my mind is perfectly clear" :
	     mana > 90 ? "my mind is clear" :
	     mana > 75 ? "my mind is clouded" :
	     mana > 50 ? "my mind is strained" :
	     mana > 30 ? "my mind suffers" :
	     mana > 15 ? "i can barely think" : "I cannot even concentrate");
    do_say(ch,buf);
}

void do_practice( CHAR_DATA *ch, char *argument )
{
    int sn = 0;
    int rating = 0;
    sh_int *learned;
    SKILL_DATA* nsk = NULL;
    CSKILL_DATA* csk = NULL;

    if ( IS_NPC(ch) )
	return;
    if ( argument[0] == '\0' ){
      int col = 0;
      int level = 0;

      for ( sn = 0; sn < MAX_SKILL; sn++ ){
	SKILL_DATA* nsk = NULL;
	sh_int learned = 0;

	if ( skill_table[sn].name == NULL )
	  break;

	if (skill_table[sn].skill_level[0] != 69
 	    && (skill_table[sn].skill_level[ch->class] > 50 || skill_table[sn].skill_level[ch->class] < 1)
	    && (nsk = nskill_find(ch->pcdata->newskills,sn)) == NULL
	    && (csk = get_cskill(ch->pCabal, sn)) == NULL)
	  continue;

	if (nsk != NULL)
	  learned = nsk->learned;
	else
	  learned = ch->pcdata->learned[sn];

	level = sklevel(ch,sn);

	if ( learned > 0 &&  (level <= ch->level || level == CABAL_COST_FAILED)){
	  if (ch->pcdata->to_learned[sn] != 0)
	    sendf( ch, "%-19s `%s%3d``   ",
		   skill_table[sn].name,
		   ch->pcdata->to_learned[sn] > 0 ? "@" : "1",
		   URANGE(0, learned + ch->pcdata->to_learned[sn], 110));
	  else
	    sendf( ch, "%-19s %3d   ",skill_table[sn].name,
		   URANGE(0, learned + ch->pcdata->to_learned[sn], 110));
	  if ( ++col % 3 == 0 )
	    send_to_char( "\n\r", ch );
	}
      }
      if ( col % 3 != 0 )
	send_to_char( "\n\r", ch );
      sendf( ch, "You have %d practice sessions left.\n\r",ch->practice );

    }
    else{
      CHAR_DATA *mob;
      CSKILL_DATA* cSkill = NULL;
      int level = 0;
      if ( !IS_AWAKE(ch) ){
	send_to_char( "In your dreams, or what?\n\r", ch );
	return;
      }
      for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room ){
	if ( ch->class == class_lookup("monk")
	     && IS_NPC(mob)
	     && IS_SET(mob->act, ACT_IS_HEALER))
	  if (mob->pIndexData->vnum == get_temple(ch)
	      || mob->pIndexData->vnum == (get_temple(ch) + 1))
	    break;
	if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
	  break;
      }
      if ( mob == NULL )
	{
	  send_to_char( "You can't do that here.\n\r", ch );
	  return;
	}
      if (is_exiled(ch, mob->in_room->area->vnum )){
	char buf[MIL];
	sprintf( buf, "No one in %s will deal with you %s!", mob->in_room->area->name, PERS2(ch));
	do_say( mob, buf );
	return;
      }
      if ( ch->practice <= 0 )
	{
	  send_to_char( "You have no practice sessions left.\n\r", ch );
	  return;
	}
      /* outcasts pay for practice */
      if ( IS_SET(ch->act,PLR_OUTCAST) )
	{
	  int cost = 40 * ch->level;
	  if (ch->gold < cost){
	    sendf(ch, "It will cost you %d gold to bribe the guildmaster.\n\r",
		  cost);
	    return;
	  }
	  else{
	    sendf(ch, "You carefully bribe %s with %d gold.\n\r",
		  PERS2(mob), cost);
	    ch->gold -=  cost;
	  }
	}
      if ( ( sn = find_spell( ch, argument ) ) < 0 ){
	send_to_char( "You can't practice that.\n\r", ch );
	return;
      }

/* get skill level */
      cSkill = ch->pCabal ? get_cskill( ch->pCabal, sn ) : NULL;
      level = cSkill ? cSkill->pSkill->level : sklevel(ch,sn);
      if ( level > ch->level) {
	send_to_char("You are not yet powerful enough.\n\r", ch);
	return;
      }

      /* get info on the skill we are going to learn */
      /* first check if we should look at dynamic skills */
      if ((nsk = nskill_find(ch->pcdata->newskills,sn)) == NULL
	  && level > 50){
	send_to_char( "You can't practice that.\n\r", ch );
	return;
      }
      if (nsk != NULL){
	rating = nsk->rating;
	learned = &nsk->learned;
      }
      else{
	rating = skill_table[sn].rating[ch->class];
	learned = &ch->pcdata->learned[sn];
      }
/* check for skill stats now */
      if (rating == 0
	  || *learned < 1){
	send_to_char( "You can't practice that.\n\r", ch );
	return;
      }


      if ( *learned >= 75 )
	{
	  sendf( ch, "You are already learned at %s.\n\r",skill_table[sn].name );
	}
      else
	{
	  ch->practice--;
	  if (cSkill)
	    *learned += int_app[get_curr_stat(ch,STAT_INT)].practice / cSkill->pSkill->rating;
	  else
	    *learned += int_app[get_curr_stat(ch,STAT_INT)].practice;
	  if ( *learned < 75 )
	    {
	      act( "You practice $T.", ch, NULL, skill_table[sn].name, TO_CHAR );
	      act( "$n practices $T.", ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	  else
	    {
	      *learned = 75;
	      act( "You are now learned at $T.", ch, NULL, skill_table[sn].name, TO_CHAR );
	      act( "$n is now learned at $T.", ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	}
      //set the starting point at full
      ch->pcdata->progress[sn] = get_skill_reps( *learned );
    }
}

void do_rehearse( CHAR_DATA *ch, char *argument )
{
    int sn;
    if ( IS_NPC(ch) )
	return;
    if ( argument[0] == '\0' )
    {
	int col = 0;
	for ( sn = 0; sn < MAX_SONG; sn++ )
	{
	    if ( song_table[sn].name == NULL )
		break;
	    if ( ch->pcdata->songlearned[sn] > 0 && sglevel(ch,sn) <= ch->level )
	    {
                sendf( ch, "%-19s %3d%%  ",song_table[sn].name, ch->pcdata->songlearned[sn] );
	        if ( ++col % 3 == 0 )
		    send_to_char( "\n\r", ch );
	    }
	}
	if ( col % 3 != 0 )
	    send_to_char( "\n\r", ch );
        sendf( ch, "You have %d practice sessions left.\n\r",ch->practice );
    }
    else
    {
	CHAR_DATA *mob;
	if ( !IS_AWAKE(ch) )
	{
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    return;
	}
	for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
	    if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
		break;
	if ( mob == NULL )
	{
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}
	if ( ch->practice <= 0 )
	{
	    send_to_char( "You have no practice sessions left.\n\r", ch );
	    return;
	}
	/* outcasts pay for practice */
	if ( IS_SET(ch->act,PLR_OUTCAST) )
	{
	  int cost = 40 * ch->level;
	  if (ch->gold < cost){
	    sendf(ch, "It will cost you %d gold to bribe the guildmaster.\n\r",
		  cost);
	    return;
	  }
	  else{
	    sendf(ch, "You carefully bribe %s with %d gold.\n\r",
		  PERS2(mob), cost);
	    ch->gold -=  cost;
	  }
	}

	if ( ( sn = find_song( ch,argument ) ) < 0 || ch->pcdata->songlearned[sn] < 1
	|| song_table[sn].name == NULL || song_table[sn].rating == 0 || sglevel(ch,sn) > ch->level)
	{
	    send_to_char( "You can't rehearse that.\n\r", ch );
	    return;
	}
        if ( ch->pcdata->songlearned[sn] >= 75 )
	{
	    sendf( ch, "You are already learned at %s.\n\r",song_table[sn].name );
	}
	else
	{
	    ch->practice--;
	    ch->pcdata->songlearned[sn] += int_app[get_curr_stat(ch,STAT_INT)].practice / song_table[sn].rating;
            if ( ch->pcdata->songlearned[sn] < 75 )
	    {
                act( "You rehearse $T.", ch, NULL, song_table[sn].name, TO_CHAR );
                act( "$n rehearses $T.", ch, NULL, song_table[sn].name, TO_ROOM );
	    }
	    else
	    {
                ch->pcdata->songlearned[sn] = 75;
                act( "You are now learned at $T.", ch, NULL, song_table[sn].name, TO_CHAR );
                act( "$n is now learned at $T.", ch, NULL, song_table[sn].name, TO_ROOM );
	    }
	}
    }
}

void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    int wimpy;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );
    if ( wimpy < 0 )
    {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }
    if ( wimpy > ch->max_hit/2 )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }
    ch->wimpy = wimpy;
    sendf( ch, "Wimpy set to %d hit points.\n\r", wimpy );
}

void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL];
    char *pArg, *pwdnew, *p;
    char cEnd;
    if ( IS_NPC(ch) )
	return;
    pArg = arg1;
    while ( isspace(*argument) )
	argument++;
    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;
    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';
    pArg = arg2;
    while ( isspace(*argument) )
	argument++;
    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;
    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: password <old> <new>.\n\r", ch );
	return;
    }
    if (strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }
    if ( strlen(arg2) < 5 )
    {
        send_to_char("New password must be at least five characters long.\n\r", ch );
	return;
    }
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
	if ( *p == '~' )
	{
	    send_to_char("New password not acceptable, try again.\n\r", ch );
	    return;
	}
    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( ch );
    send_to_char( "New password set.\n\r", ch );
}

void do_status (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    int count_imms = 0, count_players = 0, count_connections = 0;
    for ( victim = player_list; victim != NULL; victim = victim->next_player )
    {
      if (victim->desc != NULL && can_see (ch, victim))
	count_connections++;
      if (victim->desc != NULL && (victim->desc->connected == CON_PLAYING) && !IS_SET(victim->act2,PLR_NOWHO) && IS_IMMORTAL (victim) && (can_see (ch, victim)))
	count_imms++;
      else if (!IS_IMMORTAL (victim))
	count_players++;
    }
    do_time(ch,"");
    sendf (ch, "There are currently %d immortals, %d players, and %d connections.\n\r",count_imms, count_players, count_connections);
    if (mud_data.newlock)
	send_to_char("The realms is newlocked\n\r",ch);
    if (mud_data.wizlock)
	send_to_char("The realms is wizlocked\n\r",ch);
}

void do_hometown(CHAR_DATA *ch, char *argument)
{
    int amount = (ch->level * ch->level * 1000) + 1000, home;
    if (IS_NPC(ch))
    {
        send_to_char("You can't change your hometown!\n\r", ch);
        return;
    }
    if (!IS_SET(ch->in_room->room_flags, ROOM_REGISTRY))
    {
	send_to_char("You have to be in the Registry to change your hometown.\n\r",ch);
        return;
    }
    if (argument[0] == '\0')
    {
      send_to_char("The following hometowns are available to you:  ",ch);
      for ( home = 0; hometown_table[home].name && home < MAX_HOMETOWN; home++ ){
	if (!IS_NULLSTR(hometown_table[home].race)
	    && ch->race != race_lookup( hometown_table[home].race )){
	  continue;
	}
	else if (IS_EVIL(ch)){
	  if (hometown_table[home].e_vnum < 1)
	    continue;
	}
	else if (IS_GOOD(ch)){
	  if (hometown_table[home].g_vnum < 1)
	    continue;
	}
	else if (hometown_table[home].n_vnum < 1)
	  continue;
	sendf(ch, "%s ", hometown_table[home].name);
      }
      sendf(ch, "\n\rIt will cost you %d gold.\n\r", amount);
    }
    if (ch->gold < amount)
    {
        sendf(ch,"It'll cost you %d gold to change hometowns!\n\r",amount);
        return;
    }
    if (argument[0] == '\0')
	return;

    if ( (home = hometown_lookup(argument)) < 0){
      send_to_char("Unknown hometown.\n\r", ch);
      return;
    }
    if (!IS_NULLSTR(hometown_table[home].race)
	&& ch->race != race_lookup( hometown_table[home].race )){
      send_to_char("You cannot live there.\n\r", ch);
      return;
    }
    else if (IS_EVIL(ch)){
      if (hometown_table[home].e_vnum < 1){
	send_to_char("You cannot live there.\n\r", ch);
	return;
      }
    }
    else if (IS_GOOD(ch)){
      if (hometown_table[home].g_vnum < 1){
	send_to_char("You cannot live there.\n\r", ch);
	return;
      }
    }
    else if (hometown_table[home].n_vnum < 1){
      send_to_char("You cannot live there.\n\r", ch);
      return;
    }
    else if (home == ch->hometown){
      send_to_char("But you already live there!\n\r",ch);
      return;
    }
    sendf(ch, "Your hometown is changed to %s.\n\r", hometown_table[home].name);
    ch->hometown = home;
    ch->gold -= amount;
}

void show_ignore(CHAR_DATA *ch)
{
    if (IS_NPC(ch))
	return;
    if (ch->pcdata->ignore[0] == '\0')
    {
	send_to_char("You are not ignoring anyone at the moment.\n\r",ch);
	return;
    }
    sendf(ch, "You are ignoring:%s\n\r",ch->pcdata->ignore);
}

void do_ignore(CHAR_DATA *ch, char *argument)
{
    char buf1[MSL], buf2[MSL], arg[MSL];
    argument = one_argument( argument, arg );
    if (IS_NPC(ch))
	return;
    if (arg[0] == '\0')
    {
	show_ignore(ch);
	return;
    }
    if (!str_cmp(arg,"clear"))
    {
	ch->pcdata->ignore = str_dup("");
	send_to_char("Ignore list cleared.\n\r",ch);
	return;
    }
    if (!str_cmp(arg,"all"))
    {
	send_to_char("Try using 'quiet' instead.\n\r",ch);
	return;
    }
    sprintf(buf1," %s",arg);
    if (ch->pcdata->ignore[0] == '\0')
    {
    	ch->pcdata->ignore = str_dup(buf1);
	send_to_char("Argument added to ignore list.\n\r",ch);
	show_ignore(ch);
	return;
    }
    sprintf(buf2, "%s", ch->pcdata->ignore);
    if (!is_name(buf1,buf2))
    {
        if ( strlen(buf2) > 255 )
        {
            send_to_char("Your ignore list is too long.\n\r",ch);
            return;
        }
	strcat(buf2,buf1);
	send_to_char("Argument added to ignore list.\n\r",ch);
    }
    else
    {
	str_replace( buf2, buf1,"" );
	send_to_char("Argument removed from ignore list.\n\r",ch);
    }
    free_string( ch->pcdata->ignore );
    ch->pcdata->ignore = str_dup(buf2);
    show_ignore(ch);
}

void do_quest(CHAR_DATA *ch, char *argument)
{
  char arg[MSL];
  CHAR_DATA* vch = ch;

  argument = one_argument( argument, arg );
  if (arg[0] == '\0')
    vch = ch;
  else if ((vch = get_char(arg)) == NULL)
    {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
  if (!IS_IMMORTAL(ch))
    vch = ch;

  show_quest( ch, vch );
}

void show_quest( CHAR_DATA *ch, CHAR_DATA *victim)
{
  QUEST_DATA* q;
  int i = 0;
  char* prefix;
  char buf [MIL];

  if (IS_NPC(victim))
    return;
  if ( (q = victim->pcdata->quests) == NULL){
    send_to_char("No completed quests.\n\r", ch);
    return;
  }
  if (ch == victim)
    send_to_char("You have..\n\r", ch);
  else
    act_new("$N has..", ch, NULL, victim, TO_CHAR, POS_DEAD);

  for (q = victim->pcdata->quests; q; q = q->next){
    if (q->save == QUEST_INVIS &&!IS_IMMORTAL(ch))
      continue;
    buf[0] = 0;
    sprintf(buf, "%s", q->quest);
    buf[0] = UPPER(buf[0]);

    if (q->save == QUEST_TEMP)
      prefix = "<T>";
    else if (q->save == QUEST_INVIS)
      prefix = "[I]";
    else
      prefix = "";
    sendf(ch, "%s%-2d. %s\n\r", prefix, ++i, buf);
  }

  /* autoquest */
  if (HAS_AQUEST(victim)){
    sendf(ch, "%s%-2d. to %s.\n\r", "[T]", ++i, GetQuestString( &victim->pcdata->aquest ));
  }
}

void do_proficiency( CHAR_DATA *ch, char *argument )
{
    int sn, learned = 0, max, cur;
    char arg[MIL], prompt[MIL];
    CHAR_DATA *victim;
    if ( IS_NPC(ch) )
	return;
    if ( argument[0] == '\0' )
    {
      send_to_char("Syntax: proficiency <target> <skill/\"weapon\"/\"mastered\">\n\r", ch);
      return;
    }
    argument = one_argument( argument, arg );
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
	send_to_char("Show proficiency to whom?\n\r",ch);
	return;
    }
    if (!str_cmp(argument, "weapon") || !str_cmp(argument, "weapons")){
      OBJ_DATA* obj;
      if ( (obj = get_eq_char(ch, WEAR_WIELD)) != NULL){
	sendf(victim, "%s%s skill with %s (%s) is: `#%d``\n\r",
	      ch == victim ? "Your" : PERS2(ch),
	      ch == victim ? "" : "'s",
	      obj->short_descr,
	      weapon_name(obj->value[0]),
	      get_weapon_skill_obj(ch, obj) );
	if (get_skill(ch, gsn_parry)){
	  sendf(victim, "%s%s skill in parry is: `#%d``\n\r",
		ch == victim ? "Your" : PERS2(ch),
		ch == victim ? "" : "'s",
		get_skill(ch, gsn_parry));
	}
	if ( (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS) || is_affected(victim, gsn_double_grip))
	     && get_eq_char(victim,WEAR_SHIELD) == NULL
	     &&  get_eq_char(victim,WEAR_SECONDARY) == NULL
	     && get_eq_char(victim,WEAR_HOLD) == NULL){

	  if (get_skill(ch, gsn_2hands)){
	    sendf(victim, "%s%s skill in weapon block is: `#%d``\n\r",
		  ch == victim ? "Your" : PERS2(ch),
		  ch == victim ? "" : "'s",
		  get_skill(ch, gsn_2hands));
	  }
	}
      }
      else{
	sendf(victim, "%s%s skill in hand to hand combat is: `#%d``\n\r",
	      ch == victim ? "Your" : PERS2(ch),
	      ch == victim ? "" : "'s",
	      get_weapon_skill_obj(ch, NULL) );
      }

      if ( (obj = get_eq_char(ch, WEAR_SECONDARY)) != NULL){
	sendf(victim, "%s%s skill with %s (%s) is: `#%d``\n\r",
	      ch == victim ? "Your" : PERS2(ch),
	      ch == victim ? "" : "'s",
	      obj->short_descr,
	      weapon_name(obj->value[0]),
	      get_weapon_skill_obj(ch, obj) );
      }
      if (get_skill(ch, gsn_first_parry)){
	sendf(victim, "%s%s skill in first parry is: `#%d``\n\r",
	      ch == victim ? "Your" : PERS2(ch),
	      ch == victim ? "" : "'s",
	      get_skill(ch, gsn_first_parry));
      }
      if (get_skill(ch, gsn_second_parry) && is_empowered_quiet(ch, 1)){
	sendf(victim, "%s%s skill in second parry is: `#%d``\n\r",
	      ch == victim ? "Your" : PERS2(ch),
	      ch == victim ? "" : "'s",
	      get_skill(ch, gsn_second_parry));
      }
      return;
    }
    else if (!str_cmp(argument, "mastered")){
      SKILL_DATA* nsk = NULL;
      CSKILL_DATA* csk = NULL;
      sh_int learned = 0;
      int count = 0;

      if (ch == victim)
	send_to_char("You have mastered the following:\n\r", victim);
      else
	sendf( victim, "%s has mastered the following:\n\r", PERS2(ch));
      for ( sn = 0; sn < MAX_SKILL; sn++ ){
	if ( skill_table[sn].name == NULL )
	  break;

	if (skill_table[sn].skill_level[0] != 69
 	    &&  skill_table[sn].skill_level[ch->class] > 50
	    && (nsk = nskill_find(ch->pcdata->newskills,sn)) == NULL
	    && (csk = get_cskill(ch->pCabal, sn)) == NULL)
	  continue;

	if (nsk != NULL)
	  learned = nsk->learned;
	else
	  learned = ch->pcdata->learned[sn];

	if (learned < get_skillmax( ch, sn))
	  continue;
	sendf( victim, "%2d. %s\n\r", ++count, skill_table[sn].name );
      }
      return;
    }
    sn = find_spell( ch,argument );
    learned = get_skill(ch, sn);
    if ( sn < 0 || learned < 1
	 || skill_table[sn].name == NULL
	 || skill_table[sn].rating[ch->class] == 0
	 || sklevel(ch,sn) > ch->level)
      {
	send_to_char( "You are not proficient at that.\n\r", ch );
	return;
      }
    /* get progress bar */
    max = get_skill_reps( learned );
    cur = UMAX(0, max - ch->pcdata->progress[sn]);
    health_prompt( prompt, cur, max, FALSE );
    if (victim == ch)
	sendf( ch, "Your proficiency at %s is `#%d%%``.\n\rProgress: %s\n\r",
	       skill_table[sn].name,
	       learned,
	       prompt
	       );
    else
    {
      sendf( ch, "You show your proficiency at %s to %s.\n\r",
	     skill_table[sn].name,PERS(victim,ch));
      sendf( victim, "%s's proficiency at %s is `#%d%%``.\n\r",
	       PERS(ch,victim),
	       skill_table[sn].name,
	       learned
	       );
    }
}

////////////////////
//batform_see//////
///////////////////
//checks if he character can see trhough batform.

inline bool bat_blind(CHAR_DATA* ch)
{
  return (is_affected(ch, gsn_bat_form) && IS_AFFECTED2(ch, AFF_SILENCE));
}


//shows biography/info of a character
void do_bio( CHAR_DATA* ch, char* argument ){
  CHAR_DATA* victim;

  if (IS_NULLSTR(argument) || (victim = get_char_world(ch, argument)) == NULL){
    send_to_char("Show whose biography?\n\r", ch);
    return;
  }
  else if (IS_NPC(victim)){
    send_to_char("Mobs lack any such specifics.\n\r", ch);
    return;
  }
  show_detail_info( ch, victim );
}

