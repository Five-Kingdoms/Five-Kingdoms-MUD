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
#include <ctype.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"
#include "cabal.h"
#include "recycle.h"
#include "jail.h"

#define CD CHAR_DATA
CD *	find_keeper	args( (CHAR_DATA *ch ) );
CD *	find_history	args( (CHAR_DATA *ch ) );
#undef CD
#define OD OBJ_DATA
OD *    get_obj_keeper  args( (CHAR_DATA *ch,CHAR_DATA *keeper,char *argument));
#undef OD

void unadorn_obj (OBJ_DATA* socket, OBJ_DATA* obj, CHAR_DATA *ch);
extern char *target_name;
bool empty_obj(OBJ_DATA* obj)
{
  //check inv.
  if (obj->contains)
    {
      OBJ_DATA *t_obj, *next_obj;
      for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
        {
	  next_obj = t_obj->next_content;
	  obj_from_obj(t_obj);
	  if (obj->in_obj)
	    obj_to_obj(t_obj,obj->in_obj);
	  else if (obj->carried_by)
	    obj_to_char(t_obj,obj->carried_by);
	  else if (obj->in_room == NULL)
	    extract_obj(t_obj);
	  else
	    obj_to_room(t_obj,obj->in_room);
        }
      return TRUE;
    }
  return FALSE;
}

bool empty_char(CHAR_DATA* ch){
  OBJ_DATA* obj, *obj_next;
  if (!ch->carrying)
    return FALSE;

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	REMOVE_BIT(obj->extra_flags,ITEM_INVENTORY);
        obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
    }
    return TRUE;
}

void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    CHAR_DATA *gch, *victim;
    OBJ_DATA* vobj, *vobj_next;
    int members;
    bool split = TRUE;
    char buffer[100];

    if ( !CAN_WEAR(obj, ITEM_TAKE) && !IS_IMMORTAL(ch))
    {
	send_to_char( "You can't take that.\n\r", ch );
	return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
      {
        act( "$d: you can't carry that many items.",ch, NULL, obj->name, TO_CHAR );
	return;
      }
    if ((!obj->in_obj || obj->in_obj->carried_by != ch)
	&& (get_carry_weight(ch) + get_obj_weight_char(ch, obj) >= can_carry_w(ch)))
      {
        act( "$d: you can't carry that much weight.",ch, NULL, obj->name, TO_CHAR );
	return;
      }
    if (is_affected_obj(obj, gsn_gravitate))
      {
	act( "$p seems to have the weight of a mountain!",ch, obj, NULL, TO_CHAR );
	return;
      }
/* trap check */
    if (trip_traps(ch, obj->traps))
      return;
    if ( container != NULL ) {
      if (trip_traps(ch, container->traps))
	return;
      if ( (container->item_type == ITEM_WEAPON
	    || container->item_type == ITEM_ARMOR)
	   && IS_OBJ_STAT(container, ITEM_SOCKETABLE) ) {
	if (IS_OBJ_STAT(obj, ITEM_NOREMOVE) && !IS_IMMORTAL(ch)){
	  act("$p won't even budge!", ch, obj, NULL, TO_CHAR);
	  return;
	}
	obj_from_obj( obj );
	unadorn_obj(obj, container, ch);
	REMOVE_BIT(container->value[4],WEAPON_JEWELED);

	/* Put it before the detach phrase, mainly for syntax reasons.*/
	act( "You detach $p from $P.", ch, obj, container, TO_CHAR);
	if (!IS_AFFECTED(ch,AFF_SNEAK) ) {
          act( "$n detaches $p from $P.", ch, obj, container,TO_ROOM);
	}
      }
      else {
	if (container->pIndexData->vnum == OBJ_VNUM_PIT
	    && !CAN_WEAR(container, ITEM_TAKE)
	    && !IS_OBJ_STAT(obj,ITEM_HAD_TIMER)
	    && !IS_OBJ_STAT(container, ITEM_SOCKETABLE)) {
	  obj->timer = 0;
	}
        else{
	  act( "You get $p from $P.", ch, obj, container, TO_CHAR );
	  if (!IS_AFFECTED(ch,AFF_SNEAK) )
	    act( "$n gets $p from $P.", ch, obj, container, TO_ROOM );
	}
	REMOVE_BIT(obj->extra_flags,ITEM_HAD_TIMER);
	obj_from_obj( obj );
      }
    }
    else
      {
	act( "You get $p.", ch, obj, container, TO_CHAR );
	if (!IS_AFFECTED(ch,AFF_SNEAK))
	  act( "$n gets $p.", ch, obj, container, TO_ROOM );
	obj_from_room( obj );
      }
    if ( obj->item_type == ITEM_MONEY)
    {
    	if (ch->class == class_lookup("monk") && (ch->gold + obj->value[0]) > 20000)
    	{
	    int overflow = UMAX(1,ch->gold + obj->value[0] - 20000);
	    ch->gold = 20000;
	    obj->value[0] = overflow;
    	    send_to_char("You can't hold that much gold and drop it.\n\r",ch);
	    act( "$n drops some gold.", ch, NULL, NULL, TO_ROOM );
	    obj_to_room( obj,ch->in_room);
	    split = FALSE;
	}
        else if (ch->class == class_lookup("druid") && (ch->gold + obj->value[0]) > 20000)
        {
            int overflow = UMAX(1,ch->gold + obj->value[0] - 20000);
            ch->gold = 20000;
            obj->value[0] = overflow;
            send_to_char("You can't hold that much gold and drop it.\n\r",ch);
            act( "$n drops some gold.", ch, NULL, NULL, TO_ROOM );
            obj_to_room( obj,ch->in_room);
            split = FALSE;
        }
	else
	  ch->gold += obj->value[0];
        if (IS_SET(ch->act,PLR_AUTOSPLIT) && split)
        {
            members = 0;
            for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
                if (!IS_AFFECTED(gch,AFF_CHARM) && is_same_group( gch, ch ) )
                    members++;
            if ( members > 1 && obj->value[0] > 1 )
            {
                sprintf(buffer,"%d",obj->value[0]);
                do_split(ch,buffer);
            }
        }
	if (split)
	  extract_obj( obj );
    }
    else{
      obj_to_char( obj, ch );
    }
    if (!obj)
      return;
    /* OLD MPROG */
    for ( victim = ch->in_room->people; victim != NULL; victim=victim->next_in_room ){
      if (!IS_NPC(victim))
	continue;
      if (victim->pIndexData->progtypes & GET_PROG)
	mprog_get_trigger( victim, ch, obj );
      if ( HAS_TRIGGER_MOB( victim, TRIG_GET ) )
	p_give_trigger( victim, NULL, NULL, ch, obj, TRIG_GET );
    }

    /* NEW PROG */
    for (vobj = ch->in_room->contents; vobj; vobj = vobj_next){
      vobj_next = vobj->next_content;
      if (vobj == obj)
	continue;
      if ( HAS_TRIGGER_OBJ( vobj, TRIG_GET ) )
	p_give_trigger( NULL, vobj, NULL, ch, obj, TRIG_GET );
    }
    if ( HAS_TRIGGER_OBJ( obj, TRIG_GET ) )
      p_give_trigger( NULL, obj, NULL, ch, container ? container : obj, TRIG_GET );
    if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_GET ) )
      p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_GET );
}

void do_get( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL];
    OBJ_DATA *obj, *obj_next, *container;
    bool found;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if (!str_cmp(arg2,"from"))
	argument = one_argument(argument,arg2);
    if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_VAPOR)
	return;
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Get what?\n\r", ch );
	return;
    }

/* This is a BUG check. Basically, it will only allow you to socket
   items and detach items IN your inventory, to prefent people from
   getting infintine affects from sockets.
*/


//Wolf form check
    if (is_affected(ch, gsn_wolf_form)
	|| is_affected(ch, gsn_bat_form)
	|| is_affected(ch,gsn_mist_form)
	|| is_affected(ch,gsn_weretiger)
	|| is_affected(ch,gsn_werewolf)
	|| is_affected(ch,gsn_werebear)
	|| is_affected(ch,gsn_werefalcon)){
      send_to_char("You have no where to put that item your current state.\n\r", ch);
      return;
    }


    if ( arg2[0] == '\0' )
    {
	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    obj = get_obj_list( ch, arg1, ch->in_room->contents );
	    if ( obj == NULL )
	    {
		sendf(ch, "I see no %s here.\n\r", arg1);
		return;
	    }
	    get_obj( ch, obj, NULL );
	}
	else
	{
	    found = FALSE;
	    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	    {
	      obj_next = obj->next_content;
	      //safety check due to traps/oprogs etc.
	      if (obj->in_room == NULL || obj->in_room != ch->in_room)
		continue;
		if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) ) && can_see_obj( ch, obj ) )
		{
		    found = TRUE;
                    if (!IS_IMMORTAL(ch) && (obj->item_type == ITEM_CORPSE_NPC || obj->item_type == ITEM_CORPSE_PC))
                        continue;
		    if (!CAN_WEAR(obj, ITEM_TAKE))
			continue;
		    get_obj( ch, obj, NULL );
		}
	    }
	    if ( !found )
	    {
		if ( arg1[3] == '\0' )
		    send_to_char( "I see nothing here.\n\r", ch );
		else
		    sendf(ch, "I see no %s here.\n\r", &arg1[4]);
	    }
	}
    }
    else
    {
	if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	if ( ( container = get_obj_here( ch, NULL, arg2 ) ) == NULL )
	{
	    sendf(ch, "I see no %s here.\n\r", arg2);
	    return;
	}
	switch ( container->item_type )	{
	default:
	    send_to_char( "That's not a container.\n\r", ch );
	    return;
	case ITEM_CONTAINER:
        case ITEM_ARMOR:
        case ITEM_WEAPON:
	case ITEM_CORPSE_NPC:
	    break;
	case ITEM_CORPSE_PC:
	    if (ch->id != container->owner)
	    {
	      if (container->level < 15 && !IS_IMMORTAL(ch) )
	    	{
	    	    send_to_char("You can't loot from that corpse.\n\r",ch);
		    return;
	    	}

		if (ch->level < 15)
		{
		    send_to_char("You can't loot corpses yet.\n\r",ch);
		    return;
		}

		/* Vulture check, for first 2 ticks only following can loot the corpse: *
		 * Thiefs, Those in PK of victim, Those in group of victim, IMMS	*/
		/* Get original owner of the corpse */
		if (!can_loot(ch, container)){
		    act("$g's power shields $p from your greedy fingers.", ch, container, NULL, TO_CHAR);
		    return;
		}
		set_delay(ch, NULL);
		/* LOOT CRIME CHECK */
		if (ch->in_room && ch->id != container->owner && is_crime(ch->in_room, CRIME_LOOT, NULL)){
		  set_crime(ch, NULL, ch->in_room->area, CRIME_LOOT);
		}
	    }
	    break;
        }
	if (IS_SET(container->value[1], CONT_CLOSED)
	    && container->item_type != ITEM_WEAPON
	    && container->item_type != ITEM_ARMOR){
	    act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	    return;
	}

	if (IS_OBJ_STAT(container, ITEM_SOCKETABLE) &&
	    ((obj = get_obj_carry( ch, arg2, ch)) == NULL )) {
	  send_to_char( "You can only detach sockets from items in your inventory.\n\r" ,ch);
	  return;
	}

	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    obj = get_obj_list( ch, arg1, container->contains );
	    if ( obj == NULL )
	    {
		sendf(ch, "I see nothing like that in the %s.\n\r", arg2);
		return;
	    }
	    if (IS_SET(obj->wear_flags, ITEM_HAS_OWNER)
		&& !is_owner(obj, ch)
		&& !IS_IMMORTAL(ch) )
	      act("You cannot touch $p safely.", ch, obj, NULL, TO_CHAR);
	    else
	      get_obj( ch, obj, container );
	}
	else
	{
	    found = FALSE;
            if (container->pIndexData->vnum == OBJ_VNUM_PIT && !IS_IMMORTAL(ch))
	    {
		send_to_char("Don't be so greedy!\n\r",ch);
		return;
            }
	    for ( obj = container->contains; obj != NULL; obj = obj_next )
	    {
		obj_next = obj->next_content;
		//safety check due to traps/oprogs etc.
		if (obj->in_obj == NULL || obj->in_obj != container)
		  continue;
		if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) ) && can_see_obj( ch, obj ))
		{
		    found = TRUE;
		    if (IS_SET(obj->wear_flags, ITEM_HAS_OWNER)
			&& !is_owner(obj, ch)
			&& !IS_IMMORTAL(ch) )
		      act("You cannot touch $p safely.", ch, obj, NULL, TO_CHAR);
		    else
		      get_obj( ch, obj, container );

		    /* check for traps */
		    if (ch == NULL)
		      break;
		}
	    }
	    if ( !found )
	    {
		if ( arg1[3] == '\0' )
                    sendf(ch, "I see nothing in the %s.\n\r", arg2);
		else
                    sendf(ch, "I see nothing like that in the %s.\n\r", arg2);
	    }
	}
    }
}

/***** Removes strings from socketed weapons - Crypt ****/
void unadorn_obj (OBJ_DATA* socket, OBJ_DATA* obj, CHAR_DATA *ch) {
  AFFECT_DATA *paf = NULL, *paf_list = NULL, *paf_tmp = NULL, *paf_next = NULL;
  char item_buf[MIL];
  char * loc;

/* test for right item */
  if (socket->item_type != ITEM_SOCKET){
    bug("unadorn_obj: socket passed was not ITEM_SOCKET.",
	socket->pIndexData->vnum);
    return;
  }

  if (!IS_OBJ_STAT(obj, ITEM_SOCKETABLE)){
    bug("unadorn_obj: obj passed was not socketable.",
	obj->pIndexData->vnum);
    return;
  }

/* replace descriptions and names (the hard way) */

  /* short descr */
  sprintf(item_buf, "%s", obj->short_descr);
  loc = strstr (item_buf, " adorned with");
  if (loc)
    *(loc) = '\0';
  free_string (obj->short_descr);
  obj->short_descr = str_dup (item_buf);

  /* long descr */
  sprintf(item_buf, "%s is lying here.", obj->short_descr);
  free_string( obj->description );
  obj->description = str_dup(item_buf);

  /* name */
  sprintf(item_buf, "%s", obj->name);
  loc = strstr (item_buf, socket->name);
  if (loc)
    *(loc) = '\0';
  free_string (obj->name);
  obj->name = str_dup(item_buf);


/* replace descriptions and names (easy way)
    free_string (obj->short_descr);
    obj->short_descr = str_dup( obj->pIndexData->short_descr );
    free_string (obj->description);
    obj->description = str_dup( obj->pIndexData->description );
    free_string (obj->name);
    obj->name = str_dup( obj->pIndexData->name );
*/

/* first remove affects that are adding things to the object */
    for (paf = obj->affected; paf != NULL; paf = paf_next) {
      paf_next = paf->next;
      if (paf->type == skill_lookup("socketed object")
	  && (paf->where == TO_OBJECT || paf->where == TO_WEAPON) )
	affect_remove_obj(obj, paf);
    }

/* goal: remove gem added affects from the object */
  for (paf = obj->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    if (paf->type != skill_lookup("socketed object")) {
      paf_tmp = paf_list;
      if (paf_tmp == NULL) {
	paf_list = paf;
	paf->next = NULL;
	continue;
      }
      while (paf_tmp->next != NULL) {
	paf_tmp = paf_tmp->next;
      }
      paf_tmp->next = paf;
      paf->next = NULL;
      continue;
    }
    free_affect(paf);
  }
  obj->affected = paf_list;
  paf_list = NULL;

}

/***** Adds strings to socketed weapons - Crypt ****/
void adorn_obj (OBJ_DATA* socket, OBJ_DATA* obj, CHAR_DATA *ch)
{
    AFFECT_DATA *paf;
    AFFECT_DATA *af_new;
    char name[MIL], item_buf[MIL], socket_buf[MIL];
    /* test for right item */
    if (socket->item_type != ITEM_SOCKET){
      bug("adorn_obj: socket passed was not ITEM_SOCKET.",
          socket->pIndexData->vnum);
      return;
    }
    if (!IS_OBJ_STAT(obj, ITEM_SOCKETABLE)){
      bug("adorn_obj: obj passed was not socketable.",
	  obj->pIndexData->vnum);
      return;
    }
/* change name only for non-holy weapons
    if (obj->pIndexData->vnum != OBJ_VNUM_HOLY_WEAPON
	&& obj->pIndexData->vnum != OBJ_VNUM_HOLY_ARMOR
	&& obj->pIndexData->vnum != OBJ_VNUM_MAL_WEAPON){
*/
    if (obj->item_type == ITEM_WEAPON){
      sprintf(item_buf, "%s", obj->short_descr);
    }
    else{
      sprintf(item_buf, "%s", obj->short_descr);
    }
    sprintf(socket_buf, "%s", socket->short_descr);

    /* replace short desc */
    free_string( obj->short_descr );
    sprintf(name, "%s adorned with %s", item_buf, socket_buf);
    obj->short_descr = str_dup(name);

    /* replace long desc */
    free_string( obj->description );
    sprintf(name, "%s adorned with %c%s", item_buf + 2,
	    tolower (socket->description[0]), socket->description + 1);
    obj->description = str_dup(name);

    /* Add both object names to one object. */

    sprintf (name, "%s %s", obj->name, socket->name);
    obj->name = str_dup(name);

/* GOAL: swap gem affects onto the weapon */
/* if the obj had previous enchants, copy them over so we dont lose */
    if (!obj->enchanted) {
      AFFECT_DATA *af_new;
      obj->enchanted = TRUE;
      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
	af_new = new_affect();
	af_new->next = obj->affected;
	obj->affected = af_new;
	af_new->duration = paf->duration;
	af_new->level = paf->level;
	af_new->where = paf->where;
	af_new->bitvector = paf->bitvector;
	af_new->location = paf->location;
	af_new->modifier = paf->modifier;
	af_new->has_string = paf->has_string;
	af_new->string = paf->string;
	if ( obj->item_type == ITEM_WEAPON ){
	  af_new->type = UMAX(skill_lookup("enchant weapon"),paf->type);
	}
	else{
	  af_new->type = UMAX(skill_lookup("enchant armor"),paf->type);
	}
      }
    }

/* loop over the affects on the gem to object*/
    for ( paf = socket->pIndexData->affected; paf != NULL; paf = paf->next ) {

/* - for each affect, apply the same affect to the weapon */
      af_new = new_affect();
      af_new->next = obj->affected;
      obj->affected = af_new;

      af_new->type = skill_lookup ("socketed object");
      af_new->level = paf->level;
      af_new->duration = -1;

      af_new->location = paf->location;
      af_new->modifier = paf->modifier;
      af_new->where = paf->where;
      af_new->bitvector = paf->bitvector;
/*  	sendf( ch, "Adding affect: %s by %d, level %d.\n\r", (af_new->where == TO_SKILL ?  */
/*  		skill_table[af_new->location].name : affect_loc_name(af_new->location )),  */
/*  		af_new->modifier, af_new->level ); */

/* dont forget to set the obj enchanted! */
      obj->enchanted = TRUE;
    }
/* move over the extra and weapon flags if any on the socket */
    {
      int flag = 0;
      AFFECT_DATA af;

      af.type = skill_lookup ("socketed object");
      af.level = socket->level;
      af.duration = -1;
      af.where = TO_OBJECT;
      af.location = 0;
      af.modifier = 0;

      if (socket->value[1]){
	for (flag = 0; extra_flags[flag].name != NULL; flag++){
	  int bit = extra_flags[flag].bit;
	  /* we check if the particular bit is set on the socket
	   * and NOT set on the object we are socketing
	   */
	  if (IS_SET(socket->value[1], bit) && !IS_SET(obj->extra_flags, bit)){
	    af.bitvector = bit;
	    affect_to_obj(obj, &af);
	  }
	}
      }
/* do same for weapon flags */
      if (socket->value[4] && obj->item_type == ITEM_WEAPON){
	af.where = TO_WEAPON;
	for (flag = 0; weapon_type2[flag].name != NULL; flag++){
	  int bit = weapon_type2[flag].bit;
	  /* we check if the particular bit is set on the socket
	   * and NOT set on the object we are socketing
	   */
	  if (IS_SET(socket->value[4], bit) && !IS_SET(obj->value[4], bit)){
	    af.bitvector = bit;
	    affect_to_obj(obj, &af);
	  }
	}
      }
    }
}

void do_put( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL];
    OBJ_DATA *container, *obj_next, *things ;
    OBJ_DATA *obj = NULL, *vobj, *vobj_next;
    CHAR_DATA* victim;
    int things_in_container = 0;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if (!str_cmp(arg2,"in") || !str_cmp(arg2,"on"))
	argument = one_argument(argument,arg2);
//Form check
    if (is_affected(ch, gsn_wolf_form)
	|| is_affected(ch, gsn_bat_form)
	|| is_affected(ch, gsn_mist_form)
	|| is_affected(ch,gsn_weretiger)
	|| is_affected(ch,gsn_werewolf)
	|| is_affected(ch,gsn_werebear)
	|| is_affected(ch,gsn_werefalcon)){
      send_to_char("Huh?\n\r", ch);
      return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Put what in what?\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }
    if ( ( container = get_obj_here( ch, NULL, arg2 ) ) == NULL )
    {
	sendf(ch, "I see no %s here.\n\r", arg2);
	return;
    }
    /* trap check */
    if (trip_traps(ch, container->traps))
      return;
/* START THE SOCKET PART - Crypt */
    if (( container->item_type == ITEM_WEAPON )
	|| (container->item_type == ITEM_ARMOR)){
      if ( IS_OBJ_STAT(container, ITEM_SOCKETABLE)) {
	if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
        if (obj->item_type != ITEM_SOCKET)
        {
            send_to_char("That is not a socketable item.\n\r", ch);
	    return;
	}
        if ( IS_WEAPON_STAT(container,WEAPON_JEWELED) )
        {
            send_to_char("There is already something in the socket.\n\r",ch);
	    return;
	}
	if ( ( container = get_obj_carry( ch, arg2, ch ) ) == NULL ) {
	  send_to_char( "You can only socket items in your inventory.\n\r", ch);
	  return;
	}
	if ( (IS_LIMITED(obj) || obj->pIndexData->pCabal)
	     && !IS_LIMITED(container)
	     && !container->pIndexData->pCabal){
	  act("$p seems to be far too intricate to fit properly.", ch, obj, NULL, TO_CHAR);
	  return;
	}
	if (!IS_SOC_STAT(obj, SOCKET_ARMOR)
	    && container->item_type == ITEM_ARMOR){
	  act("$p cannot be affixed to armors.", ch, obj, NULL, TO_CHAR);
	  return;
	}
	if (!IS_SOC_STAT(obj, SOCKET_WEAPON)
	    && IS_SOC_STAT(obj, SOCKET_ARMOR)
	    && container->item_type == ITEM_WEAPON){
	  act("$p cannot be affixed to weapons.", ch, obj, NULL, TO_CHAR);
	  return;
	}
        obj_from_char( obj );
        obj_to_obj( obj, container );
	act( "You affix $p to $P.", ch, obj, container, TO_CHAR );
	if (!IS_AFFECTED(ch,AFF_SNEAK)) {
	  act( "$n affixes $p to $P.", ch, obj, container, TO_ROOM );
	}
        adorn_obj(obj, container, ch);
        SET_BIT(container->value[4],WEAPON_JEWELED);
	return;
      }
    }

/* END SOCKET PART - Crypt */

    if ( container->item_type != ITEM_CONTAINER )
    {
	send_to_char( "That's not a container.\n\r", ch );
	return;
    }
    if ( IS_SET(container->value[1], CONT_CLOSED) )
    {
	act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	return;
    }
    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
	if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
	if ( obj == container )
	{
	    send_to_char( "You can't fold it into itself.\n\r", ch );
	    return;
	}
	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}
        if (WEIGHT_MULT(obj) != 100)
    	{
            send_to_char("You have a feeling that would be a bad idea.\n\r",ch);
            return;
        }
	if (is_affected_obj( obj, gen_malform) || is_affected_obj( obj, gen_hwep)){
	  send_to_char("You can not put such a powerful item in a container.\n\r",ch);
	  return;
	}
	if ( (IS_OBJ_STAT(obj, ITEM_HOLDS_RARE) || IS_LIMITED(obj))
	     && !IS_OBJ_STAT(container,ITEM_HOLDS_RARE)){
	  send_to_char("You can not put such a powerful item in a container.\n\r",ch);
	  return;
	}
	if (IS_SET(obj->wear_flags, ITEM_HAS_OWNER)){
	  send_to_char("You can not put such a powerful item in a container.\n\r",ch);
	  return;
	}
	/* changed for now: Viri
	if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
	{
	    send_to_char("That item is too worthless to put in a container.\n\r",ch);
	    return;
	}
	*/
	for ( things = container->contains; things != NULL;
	      things = things->next_content ) {
	  things_in_container++;
	}

        if (get_obj_weight(container) > (container->value[0] * 10)){
	  send_to_char("Its too heavy to be put in there.\n\r", ch);
	  return;
	}
        if ( things_in_container >= container->value[3]){
	    send_to_char( "It won't fit.\n\r", ch );
	    return;
	}
	if (IS_OBJ_STAT (container, ITEM_HOLDS_RARE)) {
	  if (obj->item_type != ITEM_WEAPON) {
	    send_to_char ( "That item is not a weapon.\n\r", ch);
	    return;
	  }
	}
	if (container->pIndexData->vnum == OBJ_VNUM_PIT && !CAN_WEAR(container,ITEM_TAKE))
        {
	    if (obj->timer)
		SET_BIT(obj->extra_flags,ITEM_HAD_TIMER);
	    else
	        obj->timer = number_range(100,200);
        }
	obj_from_char( obj );
	obj_to_obj( obj, container );
	if (IS_SET(container->value[1],CONT_PUT_ON))
	{
    	    act("You put $p on $P.",ch,obj,container, TO_CHAR);
	    if (!IS_AFFECTED(ch,AFF_SNEAK) )
            	act("$n puts $p on $P.",ch,obj,container, TO_ROOM);
	}
	else
	{
	    act( "You put $p in $P.", ch, obj, container, TO_CHAR );
	    if (!IS_AFFECTED(ch,AFF_SNEAK) )
	    	act( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
	}
	/* NEW PROG */
	for ( victim = ch->in_room->people; victim; victim = victim->next_in_room ){
	  if (!IS_NPC(victim))
	    continue;
	  if ( HAS_TRIGGER_MOB( victim, TRIG_PUT ) )
	    p_give_trigger( victim, NULL, NULL, ch, obj, TRIG_PUT );
	}

	for (vobj = ch->in_room->contents; vobj; vobj = vobj_next){
	  vobj_next = vobj->next_content;
	  if (vobj == obj)
	    continue;
	  if ( HAS_TRIGGER_OBJ( vobj, TRIG_PUT ) )
	    p_give_trigger( NULL, vobj, NULL, ch, obj, TRIG_PUT );
	}
	if ( HAS_TRIGGER_OBJ( obj, TRIG_PUT ) )
	  p_give_trigger( NULL, obj, NULL, ch, container ? container : obj, TRIG_PUT );
	if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_PUT ) )
	  p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_PUT );
    }
/* group put */
    else{
      int count = 0;
      for ( things = container->contains; things != NULL;
	    things = things->next_content ) {
	count++;
      }
      for ( obj = ch->carrying; obj != NULL; obj = obj_next ){
	obj_next = obj->next_content;
	if ( ( arg1[3] == '\0'
	       || is_name( &arg1[4], obj->name ) )
	     && can_see_obj( ch, obj )
	     && WEIGHT_MULT(obj) == 100
	     && !IS_LIMITED(obj)
	     /* Changed for now: Viri
		&& !IS_OBJ_STAT(obj,ITEM_MELT_DROP)
	     */
	     && obj->wear_loc == WEAR_NONE
	     && obj != container
	     && can_drop_obj( ch, obj )
	     && get_obj_weight( container ) < (container->value[0] * 10)
	     && str_cmp(obj->material,"warflesh")){

	  if (container->pIndexData->vnum == OBJ_VNUM_PIT && !CAN_WEAR(obj, ITEM_TAKE) ){
	    if (obj->timer)
	      SET_BIT(obj->extra_flags,ITEM_HAD_TIMER);
	    else
	      obj->timer = number_range(100,200);
	  }
	  if (++count >= container->value[3])
	    break;
	  obj_from_char( obj );
	  obj_to_obj( obj, container );
	  if (IS_SET(container->value[1],CONT_PUT_ON))
	    {
	      act("You put $p on $P.",ch,obj,container, TO_CHAR);
	      if (!IS_AFFECTED(ch,AFF_SNEAK) )
		act("$n puts $p on $P.",ch,obj,container, TO_ROOM);
	    }
	  else
	    {
	      act( "You put $p in $P.", ch, obj, container, TO_CHAR );
	      if (!IS_AFFECTED(ch,AFF_SNEAK) )
		act( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
	    }
	  /* NEW PROG */
	  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room ){
	    if (!IS_NPC(victim))
	      continue;
	    if ( HAS_TRIGGER_MOB( victim, TRIG_PUT ) )
	      p_give_trigger( victim, NULL, NULL, ch, obj, TRIG_PUT );
	  }

	  for (vobj = ch->in_room->contents; vobj; vobj = vobj_next){
	    vobj_next = vobj->next_content;
	    if (vobj == obj)
	      continue;
	    if ( HAS_TRIGGER_OBJ( vobj, TRIG_PUT ) )
	      p_give_trigger( NULL, vobj, NULL, ch, obj, TRIG_PUT );
	  }
	  if ( HAS_TRIGGER_OBJ( obj, TRIG_PUT ) )
	    p_give_trigger( NULL, obj, NULL, ch, obj, TRIG_PUT );
	  if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_PUT ) )
	    p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_PUT );
	}
      }
    }
}

OBJ_DATA* get_proj(OBJ_DATA* omit, OBJ_DATA* list, int vnum, char* name, int spec){
  OBJ_DATA*  obj, *vobj;

  if (list == NULL)
    return NULL;

  for (obj = list; obj != NULL; obj = obj->next_content){
    if (omit && obj == omit)
      continue;
    if (obj->item_type == ITEM_PROJECTILE){
    /* if name is specified then it takes priority */
      if (!IS_NULLSTR(name) && !is_auto_name(name, obj->name) && !is_name(name, obj->name))
	continue;
      if (IS_SET(spec, obj->value[0])
	  || (vnum > 0 && obj->pIndexData->vnum == vnum))
	return obj;
    }
    if (obj->contains && (vobj = get_proj(omit, obj->contains, vnum, name, spec)) != NULL)
      return vobj;
  }
  return NULL;
}


/* returns a first projectile in char's inventory */
OBJ_DATA* get_projectile(CHAR_DATA* ch, OBJ_DATA* container){
  int vnum = 0;
  char* name = "";

  /* specific reload */
  if (container->contains){
    vnum = container->contains->pIndexData->vnum;
    name = container->contains->name;
  }
  else if (container->value[3])
    vnum = container->value[3];

  /* search for first specific or matching ammo type */
  return get_proj(container, ch->carrying, vnum, name, container->value[0]);
}


/* unloads a ranged item putting items into a charac. inventory */
void unload (CHAR_DATA* ch, OBJ_DATA* container){
  OBJ_DATA* obj, *obj_next;

  for (obj = container->contains; obj != NULL; obj = obj_next){
    obj_next = obj->next_content;
    obj_from_obj( obj );
    obj_to_ch( obj, ch );
  }
  act("You unload $p.", ch, container, NULL, TO_CHAR);
  if (!IS_AFFECTED(ch,AFF_SNEAK)
      && !IS_AFFECTED(ch, AFF_HIDE)
      && !IS_AFFECTED2(ch, AFF_CAMOUFLAGE))
    act( "$n unloads $p.", ch, container, NULL, TO_ROOM );
}


void do_drop( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj, *obj_next;
    CHAR_DATA *victim;
    OBJ_DATA* vobj, *vobj_next;
    bool found;
    argument = one_argument( argument, arg );
//Form check
    if (is_affected(ch, gsn_wolf_form)
	|| is_affected(ch, gsn_bat_form)
	|| is_affected(ch, gsn_mist_form)
	|| is_affected(ch,gsn_weretiger)
	|| is_affected(ch,gsn_werewolf)
	|| is_affected(ch,gsn_werebear)
	|| is_affected(ch,gsn_werefalcon)){
      send_to_char("Huh?\n\r", ch);
      return;
    }
    if ( arg[0] == '\0' )
    {
	send_to_char( "Drop what?\n\r", ch );
	return;
    }
    if ( is_number( arg ) )
    {
        int amount;
	amount   = atoi(arg);
	argument = one_argument( argument, arg );
        if ( amount <= 0 || ( str_cmp( arg, "coins" ) && str_cmp( arg, "coin" ) && str_cmp( arg, "gold"  ) ) )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}
        if (ch->gold < amount)
        {
            send_to_char("You don't have that much gold.\n\r",ch);
            return;
        }
        ch->gold -= amount;
	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    switch ( obj->pIndexData->vnum )
	    {
            case OBJ_VNUM_GOLD_ONE:
                amount += 1;
                extract_obj( obj );
		break;
            case OBJ_VNUM_GOLD_SOME:
                amount += obj->value[0];
                extract_obj( obj );
		break;
	    }
	}
        obj_to_room( create_money( amount ), ch->in_room );
	if (!IS_AFFECTED(ch,AFF_SNEAK))
            act( "$n drops some gold.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "OK.\n\r", ch );
	return;
    }
    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}
	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	if (!IS_AFFECTED(ch,AFF_SNEAK))
	    act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
	act( "You drop $p.", ch, obj, NULL, TO_CHAR );

	/* OLD MOBPROG */
	for ( victim = ch->in_room->people; victim != NULL; victim=victim->next_in_room ){
	  if (!IS_NPC(victim))
	    continue;
	  if (victim->pIndexData->progtypes & DROP_PROG)
	    mprog_drop_trigger( victim, ch, obj );
	  if ( HAS_TRIGGER_MOB( victim, TRIG_DROP ) )
	    p_give_trigger( victim, NULL, NULL, ch, obj, TRIG_DROP );
	}

	/* PROGS */
	for (vobj = ch->in_room->contents; vobj; vobj = vobj_next){
	  vobj_next = vobj->next_content;
	  if (vobj == obj)
	    continue;
	  if ( HAS_TRIGGER_OBJ( vobj, TRIG_DROP ) )
	    p_give_trigger( NULL, vobj, NULL, ch, obj, TRIG_DROP );
	}
        if ( HAS_TRIGGER_OBJ( obj, TRIG_DROP ) )
	  p_give_trigger( NULL, obj, NULL, ch, obj, TRIG_DROP );
        if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_DROP ) )
	  p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_DROP );

	if ((!str_cmp(obj->material,"glass"))
	    || (!str_cmp(obj->material,"crystal")))
	{
	  act("$p shatters into a million pieces!", ch, obj, NULL,TO_ALL);
	  extract_obj(obj);
	  obj = create_object( get_obj_index(OBJ_VNUM_GLASS), 50);
	  obj->timer = 3;
	  obj_to_room(obj, ch->in_room);
	}

	if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
	{
	    act("$p dissolves into smoke.",ch,obj,NULL,TO_ALL);
            extract_obj(obj);
	}
    }
    else
    {
	found = FALSE;
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
            && can_see_obj( ch, obj ) && obj->wear_loc == WEAR_NONE && can_drop_obj( ch, obj ) )
	    {
		found = TRUE;
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
		act( "You drop $p.", ch, obj, NULL, TO_CHAR );
	        for ( victim = ch->in_room->people; victim != NULL; victim=victim->next_in_room )
		    if (IS_NPC(victim) && victim->pIndexData->progtypes & DROP_PROG)
		        mprog_drop_trigger( victim, ch, obj );
         	if ((!str_cmp(obj->material,"glass")) || (!str_cmp(obj->material,"crystal")))
        	{
            	    act("$p shatters into a million pieces!",ch,obj,NULL,TO_ALL);
            	    extract_obj(obj);
		    obj = create_object( get_obj_index(OBJ_VNUM_GLASS), obj->level);
		    obj->timer = 3;
		    obj_to_room(obj,ch->in_room);
        	}
	       	if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
        	{
            	    act("$p dissolves into smoke.",ch,obj,NULL,TO_ALL);
                    extract_obj(obj);
        	}
	    }
	}
	if ( !found )
	{
	    if ( arg[3] == '\0' )
                act( "You are not carrying anything.",ch, NULL, arg, TO_CHAR );
	    else
                sendf(ch, "You are not carrying any %s.\n\r", &arg[4]);
	}
    }
}

void do_give( CHAR_DATA *ch, char *argument )
{
    char arg1 [MIL], arg2 [MIL], buf[MSL];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Give what to whom?\n\r", ch );
	return;
    }
    if ( is_number( arg1 ) )
    {
	int amount;
	amount   = atoi(arg1);
        if ( amount <= 0 || ( str_cmp( arg2, "coins" ) && str_cmp( arg2, "coin" ) && str_cmp( arg2, "gold"  ) ) )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}
	argument = one_argument( argument, arg2 );
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Give what to whom?\n\r", ch );
	    return;
	}
	if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

        if ( ch->gold < amount )
	{
            send_to_char( "You haven't got that much gold.\n\r", ch );
	    return;
	}
	if (victim->class == class_lookup("monk") && (victim->gold + amount) > 20000)
	{
	    act("$N can't hold that much gold.",ch,NULL,victim,TO_CHAR);
	    return;
    	}
        ch->gold     -= amount;
	victim->gold += amount;
        sprintf(buf,"$n gives you %d gold.",amount);
	act( buf, ch, NULL, victim, TO_VICT    );
        act( "$n gives $N some gold.",  ch, NULL, victim, TO_NOTVICT );
        sprintf(buf,"You give $N %d gold.",amount);
        act( buf, ch, NULL, victim, TO_CHAR    );

	/* mirrors and decoys cant recive gold */
	if (IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_DUMMY)
	  {
	    act("The coins spill out of $n's hands.",victim, NULL, NULL, TO_ROOM);
	    send_to_char("You cannot recive gold.\n\r", victim);
	    obj_to_room(create_money(amount),victim->in_room);
	    victim->gold -= amount;
	  }

        if (IS_NPC(victim)){
	  if (victim->pIndexData->progtypes & BRIBE_PROG)
	    mprog_bribe_trigger( victim, ch, amount );
	  if (HAS_TRIGGER_MOB(victim, TRIG_BRIBE))
	    p_bribe_trigger( victim, ch,  amount );
	}
        return;
    }
    if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }
    if ( obj->wear_loc != WEAR_NONE )
    {
	send_to_char( "You must remove it first.\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

// change back
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL && obj->pIndexData->vnum != 775)
    {
	sendf(ch,"%s tells you '`2Sorry, you'll have to sell that.``'\n\r",PERS(victim,ch));
	ch->reply = victim;
	return;
    }
    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }
    if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
    {
	act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
	return;
    }
    if (get_carry_weight(victim) + get_obj_weight_char(ch, obj) > can_carry_w( victim ) )
    {
	sendf(ch, "%s can't carry that much weight.\n\r", PERS(victim,ch));
	return;
    }
    if ( !can_see_obj( victim, obj ) )
    {
	sendf(ch, "%s can't see it.\n\r", PERS(victim,ch));
	return;
    }

    obj_from_char( obj );
    obj_to_char( obj, victim );
    act( "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT );
    act( "$n gives you $p.",   ch, obj, victim, TO_VICT    );
    act( "You give $p to $N.", ch, obj, victim, TO_CHAR    );

    if (obj->pIndexData->vnum == OBJ_VNUM_GOLD_SOME && !IS_NPC(victim)){
      victim->gold += obj->value[0];
      extract_obj(obj);
      return;
    }

    /* PROGS */
    if ( HAS_TRIGGER_OBJ( obj, TRIG_GIVE ) )
      p_give_trigger( NULL, obj, NULL, ch, obj, TRIG_GIVE );
    if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_GIVE ) )
      p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_GIVE );

    /* OLD/NEW MOBPROG */
    if (IS_NPC(victim)){
      if (HAS_TRIGGER_MOB(victim, TRIG_GIVE))
	p_give_trigger( victim, NULL, NULL, ch, obj, TRIG_GIVE);
      if (victim->pIndexData->progtypes & GIVE_PROG)
        mprog_give_trigger( victim, ch, obj );
    }
}

void do_preserve( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "What did you want to preserve?\n\r", ch );
        return;
    }
    if ((ch->class == class_lookup("thief")) || (ch->class == class_lookup("ninja")))
    {
      if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    	{
	  if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	    {
	      send_to_char( "You do not have that item.\n\r", ch );
	      return;
	    }
	  if (obj->item_type != ITEM_HERB)
	    {
	      send_to_char( "You can't preserve that.\n\r",ch);
	      return;
	    }
	  if (!IS_AFFECTED(ch,AFF_SNEAK))
	    act( "$n wraps $p to preserve it.", ch, obj, NULL, TO_ROOM );
	  act( "You wrap $p carefully, preserving it.", ch, obj, NULL, TO_CHAR );
	  obj->timer = -1;
/* The have to have SOME lag. */
	  WAIT_STATE2(ch, PULSE_VIOLENCE * 2);
    	}
    }
    else
      {
	send_to_char( "You have no idea how to preserve things.\n\r",ch);
	return;
      }
}

void do_fill( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    OBJ_DATA *obj, *fountain;
    bool found = FALSE;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Fill what?\n\r", ch );
	return;
    }
    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }
    for ( fountain = ch->in_room->contents; fountain != NULL; fountain = fountain->next_content )
	if ( fountain->item_type == ITEM_FOUNTAIN )
	{
	    found = TRUE;
	    break;
	}
    if ( !found )
    {
	send_to_char( "There is no fountain here!\n\r", ch );
	return;
    }
    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "You can't fill that.\n\r", ch );
	return;
    }
    if ( obj->value[1] != 0 && obj->value[2] != fountain->value[2] )
    {
	send_to_char( "There is already another liquid in it.\n\r", ch );
	return;
    }
    if ( obj->value[1] >= obj->value[0] )
    {
	send_to_char( "Your container is full.\n\r", ch );
	return;
    }
    sprintf(buf,"You fill $p with %s from $P.",liq_table[fountain->value[2]].liq_name);
    act( buf, ch, obj,fountain, TO_CHAR );
    sprintf(buf,"$n fills $p with %s from $P.",liq_table[fountain->value[2]].liq_name);
    act(buf,ch,obj,fountain,TO_ROOM);
    obj->value[2] = fountain->value[2];
    obj->value[1] = obj->value[0];
    obj->value[3] = fountain->value[3];
}

void do_pour (CHAR_DATA *ch, char *argument)
{
    char arg[MSL], buf[MSL];
    OBJ_DATA *out, *in;
    CHAR_DATA *vch = NULL;
    int amount;
    argument = one_argument(argument,arg);
    if (arg[0] == '\0' || argument[0] == '\0')
    {
	send_to_char("Pour what into what?\n\r",ch);
	return;
    }
    if ((out = get_obj_carry(ch,arg, ch)) == NULL)
    {
	send_to_char("You don't have that item.\n\r",ch);
	return;
    }
    if (out->item_type != ITEM_DRINK_CON)
    {
	send_to_char("That's not a drink container.\n\r",ch);
	return;
    }
    if (out->value[1] < 0){
      send_to_char("You cannot do that with this item.\n\r",ch);
      return;
    }
    if (!str_cmp(argument,"out"))
    {
	if (out->value[1] == 0)
	{
	    send_to_char("It's already empty.\n\r",ch);
	    return;
	}
	out->value[1] = 0;
	out->value[3] = 0;
        sprintf(buf,"You invert $p, spilling %s all over the ground.",liq_table[out->value[2]].liq_name);
	act(buf,ch,out,NULL,TO_CHAR);
        sprintf(buf,"$n inverts $p, spilling %s all over the ground.",liq_table[out->value[2]].liq_name);
	act(buf,ch,out,NULL,TO_ROOM);
	return;
    }
    if ((in = get_obj_here(ch, NULL, argument)) == NULL)
    {
	vch = get_char_room(ch, NULL, argument);
	if (vch == NULL)
	{
	    send_to_char("Pour into what?\n\r",ch);
	    return;
	}
	in = get_eq_char(vch,WEAR_HOLD);
	if (in == NULL)
	{
	    send_to_char("They aren't holding anything.",ch);
 	    return;
	}
    }
    if (in->item_type != ITEM_DRINK_CON)
    {
	send_to_char("You can only pour into other drink containers.\n\r",ch);
	return;
    }
    if (in == out)
    {
	send_to_char("You cannot change the laws of physics!\n\r",ch);
	return;
    }
    if (in->value[1] > 0 && in->value[2] != out->value[2])
    {
	send_to_char("They don't hold the same liquid.\n\r",ch);
	return;
    }
    if (out->value[1] == 0)
    {
	act("There's nothing in $p to pour.",ch,out,NULL,TO_CHAR);
	return;
    }
    if (in->value[1] >= in->value[0])
    {
	act("$p is already filled to the top.",ch,in,NULL,TO_CHAR);
	return;
    }
    amount = UMIN(out->value[1],in->value[0] - in->value[1]);
    in->value[1] += amount;
    out->value[1] -= amount;
    in->value[2] = out->value[2];
    if (vch == NULL)
    {
        sprintf(buf,"You pour %s from $p into $P.",liq_table[out->value[2]].liq_name);
    	act(buf,ch,out,in,TO_CHAR);
        sprintf(buf,"$n pours %s from $p into $P.",liq_table[out->value[2]].liq_name);
    	act(buf,ch,out,in,TO_ROOM);
    }
    else
    {
        sprintf(buf,"You pour some %s for $N.",liq_table[out->value[2]].liq_name);
        act(buf,ch,NULL,vch,TO_CHAR);
        sprintf(buf,"$n pours you some %s.",liq_table[out->value[2]].liq_name);
	act(buf,ch,NULL,vch,TO_VICT);
        sprintf(buf,"$n pours some %s for $N.",liq_table[out->value[2]].liq_name);
        act(buf,ch,NULL,vch,TO_NOTVICT);
    }
}

void do_drink( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj;
    int amount, liquid;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	    if ( obj->item_type == ITEM_FOUNTAIN )
		break;
	if ( obj == NULL )
	{
	    send_to_char( "Drink what?\n\r", ch );
	    return;
	}
    }
    else
	if ( ( obj = get_obj_here( ch, NULL, arg ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 50 )
      {
	send_to_char( "You fail to reach your mouth.  *Hic*\n\r", ch );
	return;
      }
    if (is_affected(ch,gsn_fasting))
    {
	send_to_char("You are fasting! You don't feel like drinking.\n\r",ch);
	return;
    }
    if (is_affected(ch, gsn_gag)){
      send_to_char("The gag stops you from drinking.\n\r", ch);
      return;
    }
    switch ( obj->item_type )
    {
    default:
	send_to_char( "You can't drink from that.\n\r", ch );
	return;
    case ITEM_FOUNTAIN:
        if ( ( liquid = obj->value[2] )  < 0 )
        {
            bug( "Do_drink: bad liquid number %d.", liquid );
            liquid = obj->value[2] = 0;
        }
	amount = liq_table[liquid].liq_affect[4] * 3;
	break;
    case ITEM_DRINK_CON:
	if ( obj->value[1] <= 0 && obj->value[1] != -1 )
	{
	    send_to_char( "It is already empty.\n\r", ch );
	    return;
	}
	if ( ( liquid = obj->value[2] )  < 0)
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->value[2] = 0;
	}
        amount = liq_table[liquid].liq_affect[4];
	if (obj->value[1] != -1)
	  amount = UMIN(amount, obj->value[1]);
	break;
    }
    WAIT_STATE2(ch, PULSE_VIOLENCE / 3);
    act( "$n drinks $T from $p.",ch, obj, liq_table[liquid].liq_name, TO_ROOM );
    act( "You drink $T from $p.",ch, obj, liq_table[liquid].liq_name, TO_CHAR );
    if (trip_traps(ch, obj->traps))
      return;
    if (IS_UNDEAD(ch) && !is_affected(ch, gen_unlife)){
      if ( !str_cmp(liq_table[liquid].liq_name,"holy water"))
	{
	  send_to_char("The water burns in your mouth!\n\r",ch);
	  damage( ch, ch, ch->level, gsn_curse, DAM_HOLY ,TRUE);
	  spell_curse(gsn_curse, ch->level, ch, (void *) ch, TARGET_CHAR);
	}
      else if ( !str_cmp(liq_table[liquid].liq_name,"blood") && ch->race == race_lookup("vampire")){
	send_to_char("Aaaah, the sweet taste of blood.\n\r",ch);
	gain_condition( ch, COND_HUNGER, 5 );
	if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER] > 40 )
	  send_to_char( "You are full.\n\r", ch );
      }
      else{
	send_to_char("It's tastless.\n\r", ch);
	return;
      }
    }
    else
    {
    gain_condition( ch, COND_DRUNK,amount * liq_table[liquid].liq_affect[COND_DRUNK] / 36 );
    gain_condition( ch, COND_THIRST,amount * liq_table[liquid].liq_affect[COND_THIRST] / 10 );
    if (!is_affected(ch,skill_lookup("dysentery")))
    	gain_condition(ch, COND_HUNGER,amount * liq_table[liquid].liq_affect[COND_HUNGER] / 2 );
    if (!is_affected(ch,skill_lookup("nausea")))
        gain_condition(ch, COND_HUNGER,amount * liq_table[liquid].liq_affect[COND_HUNGER] / 2 );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]  > 10 )
	send_to_char( "You feel drunk.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER] > 40 )
	send_to_char( "You are full.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
	send_to_char( "Your thirst is quenched.\n\r", ch );
    if ( !str_cmp(liq_table[liquid].liq_name,"holy water"))
    {
	if (ch->alignment == 750)
	{
	    send_to_char("The sweetness of the water uplifts your spirits.\n\r",ch);
	    spell_bless(skill_lookup("bless"), ch->level/2, ch, (void *) ch, TARGET_CHAR);
	    send_to_char("You feel a little better!\n\r",ch);
	    ch->hit = UMIN( ch->hit + ch->level/10, ch->max_hit );
	}
	if (ch->alignment == -750)
	{
	    send_to_char("The water burns in your mouth!\n\r",ch);
	    damage( ch, ch, ch->level/2, gsn_curse, DAM_HOLY ,TRUE);
	    spell_curse(gsn_curse, ch->level, ch, (void *) ch, TARGET_CHAR);
	}
    }
    if ( obj->value[3] != 0 )
    {
	AFFECT_DATA af;
	act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You choke and gag.\n\r", ch );
	af.where     = TO_AFFECTS;
	af.type      = gsn_poison;
        af.level     = number_fuzzy(amount);
	af.duration  = UMAX(3, obj->level / 4);
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_POISON;
	if (!is_affected(ch, af.type)){
	  AFFECT_DATA* paf = affect_to_char( ch, &af );
	  if (!IS_NPC(ch))
	    string_to_affect( paf, ch->name );
	}
    }
    }
    if (obj->value[0] > 0 && obj->value[0] != -1){
      obj->value[1] = UMAX(obj->value[1] - amount, 0);
    }

}

void do_eat( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA* vch = NULL;
  char arg[MIL];
  OBJ_DATA *obj;
  int flevel = 0;
  one_argument( argument, arg );
  if ( arg[0] == '\0' )
    {
      send_to_char( "Eat what?\n\r", ch );
      return;
    }
  if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
    }

  if ( !IS_IMMORTAL(ch) )
    {
      if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL )
	{
	  send_to_char( "That's not edible.\n\r", ch );
	  return;
	}
      if (is_affected(ch,gsn_fasting))
	{
	  send_to_char("You are fasting! You don't feel like eating.\n\r",ch);
	  return;
	}
      if (!IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER] > 40 && ch->class != class_lookup("vampire"))
        {
	  send_to_char("You are too full to eat more.\n\r",ch);
	  return;
        }
      if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_PHOENIX
	  && ch->practice > 20)
	return;
      if (is_affected(ch, gsn_gag)){
	send_to_char("The gag stops you from eating.\n\r", ch);
	return;
      }
      if (is_affected(ch,skill_lookup("dysentery")))
	{
	  send_to_char("I don't think you should be eating while you have dysentery.\n\r",ch);
	  ch->position = POS_SITTING;
	  update_pos(ch);
	  act("You crouch on the ground as $p is ejected from your rear.",ch,obj,NULL,TO_CHAR);
	  act("$n crouchs on the ground wincing with agonizing pain.",ch,NULL,NULL,TO_ROOM);
	  if (obj->item_type == ITEM_FOOD)
	    damage(ch,ch,obj->value[1],skill_lookup("dysentery"),DAM_OTHER,TRUE);
	  defecate(ch);
	  extract_obj( obj );
	  return;
	}
    }
  WAIT_STATE2(ch, PULSE_VIOLENCE / 3);
  if (trip_traps(ch, obj->traps))
    return;
  act( "$n eats $p.",  ch, obj, NULL, TO_ROOM );
  act( "You eat $p.", ch, obj, NULL, TO_CHAR );
  switch ( obj->item_type )
    {
    case ITEM_FOOD:
      if (IS_UNDEAD(ch) && !is_affected(ch, gen_unlife)){
	send_to_char("It's tasteless.\n\r", ch);
	break;
      }

      if ( !IS_NPC(ch) )
	{
	  int condition;
	  int base = obj->value[1];
	  condition = ch->pcdata->condition[COND_HUNGER];

	  /* adjust for size */
	  base = 2 * base / UMAX(1, ch->size );

	  if (IS_PERK(ch, PERK_UNDERWEIGHT))
	    base = 4 * base / 3;

	  /* Illithids prefer brains */
	  if (ch->race == race_lookup("illithid")){
	    if (obj->pIndexData->vnum == OBJ_VNUM_BRAINS
		|| !str_prefix("brain", obj->material)){
	      send_to_char("Mmm, brains...\n\r", ch);
	      gain_condition( ch, COND_HUNGER, 3 * base);
	    }
	    else{
	      act("You feed on $p with disgust.", ch, obj, NULL, TO_CHAR);
	      gain_condition( ch, COND_HUNGER, base / 4);
	    }
	  }
	  else{
	    for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
	      if (!IS_NPC(vch) && is_same_group(vch, ch)
		  && is_affected(vch, gsn_preserve)){
		break;
	      }
	    }//END character loop
	  }
	  /* preservation effect */
	  if (vch){
	    if (vch == ch)
	      act("Your skills of preservation pay off as you marvel at the taste.", ch, obj, vch, TO_CHAR);
	    else
	      act("$N's skills of preservation pay off as you marvel at the taste.", ch, obj, vch, TO_CHAR);
	    gain_condition( ch, COND_HUNGER, 3 * base);
	  }
	  else
	    gain_condition( ch, COND_HUNGER, base);

	  if ( condition <= 0 && ch->pcdata->condition[COND_HUNGER] > 0 )
	    send_to_char( "You are no longer hungry.\n\r", ch );
	  else if ( ch->pcdata->condition[COND_HUNGER] > 40 )
	    send_to_char( "You are full.\n\r", ch );
	}
      else if (ch->pIndexData->vnum == MOB_VNUM_PHOENIX)
	{
	  ch->practice += obj->value[1];
	  ch->train    += obj->value[1];
    	}
      if ( obj->value[3] != 0
	   && (obj->pIndexData->vnum != OBJ_VNUM_BRAINS
	       || ch->race != race_lookup("illithid")) )
	{
	  AFFECT_DATA af;
	  act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
	  act( "You choke and gag.", ch, NULL, NULL, TO_CHAR );
	  af.where     = TO_AFFECTS;
	  af.type      = gsn_poison;
	  af.level     = number_fuzzy(obj->value[0]);
	  af.duration  = UMIN(3, obj->value[0] / 5);
	  af.location  = APPLY_NONE;
	  af.modifier  = 0;
	  af.bitvector = AFF_POISON;
	  if (!is_affected(ch, af.type))
	    affect_to_char( ch, &af );
	}
      break;
    case ITEM_PILL:
      WAIT_STATE(ch,6);
      if (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC)){
	if (!str_cmp(obj->material,"warflesh"))
	  flevel = ch->level - 3;
	else{
	  send_to_char("You are not allowed to use magic!\n\r", ch);
	  return;
	}
      }
      else
	flevel = obj->value[0];
      obj_cast_spell( obj->value[1], flevel, ch, ch, NULL );
      if (ch == NULL)
	return;
      obj_cast_spell( obj->value[2], flevel, ch, ch, NULL );
      if (ch == NULL)
	return;
      obj_cast_spell( obj->value[3], flevel, ch, ch, NULL );
      if (ch == NULL)
	return;
      break;
    case ITEM_HERB:
      if (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC)){
	if (!str_cmp(obj->material,"warflesh"))
	  flevel = ch->level - 3;
	else{
	  send_to_char("You are not allowed to use magic!\n\r", ch);
	  return;
	}
      }
      WAIT_STATE(ch,6);
      obj_cast_spell( obj->value[1], obj->level, ch, ch, NULL );
      break;
    }
  extract_obj( obj );
}

void do_smoke( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af;
  char arg[MIL];
  OBJ_DATA *obj;
  const int dur = 24;
  int level = 0;

  one_argument( argument, arg );

  if ( arg[0] == '\0' ){
    send_to_char( "Smoke what?\n\r", ch );
    return;
  }
  if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL ){
    send_to_char( "You do not have that item.\n\r", ch );
    return;
  }
  if ( obj->item_type != ITEM_HERB ){
    send_to_char( "You can't smoke that.\n\r", ch );
    return;
  }

  WAIT_STATE2(ch, PULSE_VIOLENCE / 2);
  act( "$n lights $p and smokes it.",  ch, obj, NULL, TO_ROOM );
  act( "You light $p and begin to smoke it.", ch, obj, NULL, TO_CHAR );

  if (IS_UNDEAD(ch)){
    send_to_char("Its tasteless.\n\r", ch);
    return;
  }
  if (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC) && !str_cmp(obj->material,"warflesh"))
    level = ch->level - 3;
  else
    level = obj->level + 10;

  obj_cast_spell( obj->value[4], UMIN(47, level), ch, ch, NULL );

  af.where     = TO_AFFECTS;
  af.type      = gsn_drug_use;
  af.level     = dur;
  af.duration  = number_fuzzy(obj->value[0]);
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = 0;

  affect_strip( ch, af.type );
  affect_to_char( ch, &af );

  spell_drug_use(gsn_drug_use, ch->level, ch, (void *) ch, TARGET_CHAR);
  extract_obj( obj );
}

/* checks if class can double sheath a given object */
bool can_doublesheath(CHAR_DATA* ch, OBJ_DATA* obj, bool fSilent ){

  if(ch->class == class_lookup("thief")
     && obj->value[0] != WEAPON_DAGGER){
    if (!fSilent)
      send_to_char("You may only double sheath daggers.\n\r", ch);
    return FALSE;
  }
  else if (ch->class == class_lookup("ninja")
	   && obj->value[0] != WEAPON_EXOTIC){
    if (!fSilent)
      send_to_char("You may only double sheath exotics.\n\r", ch);
    return FALSE;
  }
  return TRUE;
}

bool check_sheath(CHAR_DATA* ch, int iWear){
  OBJ_DATA* obj;

/* check for weapon equip with sheaths */
  if (iWear == WEAR_WIELD && (obj = get_eq_char(ch, WEAR_SHEATH_L)) != NULL){
    if (obj->item_type != ITEM_WEAPON){
      send_to_char("You may only sheath weapons.\n\r", ch);
      return FALSE;
    }
    if (get_skill(ch, gsn_sheath) < 1){
      send_to_char("You already have a weapon sheathed.\n\r", ch);
      return FALSE;
    }
    if (!can_doublesheath(ch, obj, FALSE)){
      return FALSE;
    }
  }
  /* secondary is more complicated */
  if (iWear == WEAR_SECONDARY
      || iWear == WEAR_SHIELD
      || iWear == WEAR_HOLD){
    /* check if we have something sheathed near secondary arm */
    if ((obj = get_eq_char(ch, WEAR_SHEATH_R)) != NULL){
      /* check if we are advanced enough to allowed this */
      int pos = sheath_lookup(class_table[ch->class].name);
      if (obj->item_type != ITEM_WEAPON){
	send_to_char("You may only sheath weapons.\n\r", ch);
	return FALSE;
      }
      if (get_skill(ch, gsn_sheath) < 1 || !sheath_table[pos].both){
	send_to_char("Your sheathed weapon gets in the way.\n\r", ch);
	return FALSE;
      }
      if (!can_doublesheath(ch, obj, FALSE)){
	return FALSE;
      }
    }
  }
  return TRUE;
}

bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	return TRUE;
    if ( !fReplace )
      return FALSE;
    if (CAN_WEAR(obj, ITEM_WEAR_TATTOO) && ch->tattoo>0)
        return FALSE;
    if (( !IS_IMMORTAL(ch) && IS_SET(obj->extra_flags, ITEM_NOREMOVE)) || (iWear == WEAR_WIELD && is_affected(ch, gsn_graft_weapon)) )
    {
	act( "You can't remove $p.", ch, obj, NULL, TO_CHAR );
	return FALSE;
    }
    unequip_char( ch, obj );
    act( "$n stops using $p.", ch, obj, NULL, TO_ROOM );
    act( "You stop using $p.", ch, obj, NULL, TO_CHAR );
    if ( obj->pIndexData->message )
    {
        act( obj->pIndexData->message->offself, ch, obj, NULL, TO_CHAR );
        act( obj->pIndexData->message->offother,  ch, obj, NULL, TO_ROOM );
    }
    return TRUE;
}

bool check_secondary( CHAR_DATA* ch, OBJ_DATA* obj, OBJ_DATA* wield){
  int obj_w = 0;
  int wie_w = 0;
  bool fMino = FALSE;

  if (ch->race == race_lookup("minotaur")
      && wield->value[0] == WEAPON_AXE
      && obj->value[0] == WEAPON_AXE){
    send_to_char("You managed to wield both axes in your hands.\n\r",ch);
    fMino = TRUE;
  }
  else if ((IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
	    || obj->value[0] == WEAPON_STAFF
	    || obj->value[0] == WEAPON_POLEARM
	    || obj->item_type == ITEM_INSTRUMENT)
	   && (IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS)
	       || wield->value[0] == WEAPON_STAFF
	       || wield->value[0] == WEAPON_POLEARM
	       || wield->item_type == ITEM_INSTRUMENT))
    {
      send_to_char("There's no way you can dual-wield two two-handed weapons!\n\r",ch);
      return FALSE;
    }
  else if ((ch->size < SIZE_LARGE &&
	    (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
	     || IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS)))
	   || obj->value[0] == WEAPON_STAFF
	   || obj->item_type == ITEM_INSTRUMENT
	   || wield->item_type == ITEM_INSTRUMENT
	   || obj->value[0] == WEAPON_POLEARM
	   || wield->value[0] == WEAPON_STAFF
	   || wield->value[0] == WEAPON_POLEARM
	   || is_affected(ch,gsn_double_grip))
    {
      if (wield->value[0] == WEAPON_POLEARM
	  || obj->value[0] == WEAPON_POLEARM )
	send_to_char("A polearm cannot be dual wieled with any other weapon.\n\r",ch);
      else
	send_to_char("You need two hands free for that weapon.\n\r",ch);
      return FALSE;
    }
  obj_w = get_obj_weight(obj);
  wie_w = get_obj_weight(wield);

  //if obj is between wie_w and 0.75 wie_w and obj is light or person has bladework skill allow it.
  if (obj_w <= wie_w
      && obj_w >= 3 * wie_w / 4
      && (obj_w <= 100 || get_skill(ch, gsn_bladework) > 2))
    return TRUE;
  //if obj is smaller then 0.75 wie_w allow it always
  else if (obj_w < 3 * wie_w / 4)
    return TRUE;
  else if (!fMino){
    act("$p is too heavy to be used as a secondary weapon.",ch, obj, NULL, TO_CHAR);
    return FALSE;
  }
  else
    return TRUE;
}

void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, bool twohands )
{
  /* race check */
  if (!IS_NPC(ch)
      && obj->race && obj->race != ch->race){
    act("Its size and style make it useless to you.", ch, NULL, NULL, TO_CHAR);
    return;
  }
  /* class */
  if (!IS_NPC(ch)
      && obj->class >= 0 && obj->class != ch->class){
    act("Its size and style make it useless to you.", ch, NULL, NULL, TO_CHAR);
    return;
  }
  if ( obj->pIndexData ->vnum == OBJ_VNUM_PRAYER_BEADS && ch->class != class_lookup("monk"))
      {
	act( "Only monks may hold prayer beads.",ch, NULL, NULL, TO_CHAR);
        return;
      }
    if (is_affected(ch,gsn_tiger)
	|| is_affected(ch,gsn_crane)
	|| is_affected(ch,gsn_monkey)
 	|| is_affected(ch,gsn_dragon)
	|| is_affected(ch,gsn_horse)
	|| is_affected(ch,gsn_buddha)
	|| is_affected(ch,gsn_drunken))
      {
	int loc = wear_loc(obj->wear_flags, 1);
	if (get_obj_weight(obj) > get_monk(loc)){
	  act("$p will encumber you too much while in a stance.", ch, obj, NULL, TO_CHAR);
	  return;
	}
      }
    if ( obj->item_type == ITEM_LIGHT )
    {
      if (ch->class == class_lookup("vampire"))
	{
	    act("You prefer to stay in the shadows.",ch, NULL, NULL, TO_CHAR);
	    return;
	}
      if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
	return;
      act( "$n lights $p and holds it.", ch, obj, NULL, TO_ROOM );
      act( "You light $p and hold it.",  ch, obj, NULL, TO_CHAR );
      equip_char( ch, obj, WEAR_LIGHT );
      return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
      {
	if ( get_eq_char( ch, WEAR_FINGER_L ) != NULL && get_eq_char( ch, WEAR_FINGER_R ) != NULL
	     && !remove_obj( ch, WEAR_FINGER_L, fReplace ) && !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
	  return;
	if ( get_eq_char( ch, WEAR_FINGER_L ) == NULL )
	  {
	    act( "$n slips $p on to $s left finger.",    ch, obj, NULL, TO_ROOM );
	    act( "You slip $p on to your left finger.",  ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_FINGER_L );
	    return;
	  }
	if ( get_eq_char( ch, WEAR_FINGER_R ) == NULL )
	  {
	    act( "$n slips $p on to $s right finger.",   ch, obj, NULL, TO_ROOM );
	    act( "You slip $p on to your right finger.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_FINGER_R );
	    return;
	  }
	bug( "Wear_obj: no free finger.", 0 );
	send_to_char( "You already wear two rings.\n\r", ch );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
      {
	if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL && get_eq_char( ch, WEAR_NECK_2 ) != NULL
	     && !remove_obj( ch, WEAR_NECK_1, fReplace ) && !remove_obj( ch, WEAR_NECK_2, fReplace ) )
	  return;
	if ( get_eq_char( ch, WEAR_NECK_1 ) == NULL )
	{
	  act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
	  act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
	  equip_char( ch, obj, WEAR_NECK_1 );
	  return;
	}
	if ( get_eq_char( ch, WEAR_NECK_2 ) == NULL )
	  {
	    act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
	    act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_NECK_2 );
	    return;
	  }
	bug( "Wear_obj: no free neck.", 0 );
	act( "You already wear two neck items.", ch, NULL, NULL, TO_CHAR );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
    {
	if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
	    return;
	act( "$n slides $p over $s torso.",   ch, obj, NULL, TO_ROOM );
	act( "You slide $p over your torso.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_BODY );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
    {
	if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
	    return;
	act( "$n wears $p on $s head.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your head.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_HEAD );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_FACE ) )
    {
        if ( !remove_obj( ch, WEAR_FACE, fReplace ) )            return;
        act( "$n wears $p on $s face.",   ch, obj, NULL, TO_ROOM );
        act( "You wear $p on your face.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_FACE );
        return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
    {
	if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
	    return;
	act( "$n wears $p on $s legs.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your legs.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_LEGS );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
    {
	if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
	    return;
	act( "$n ties $p on $s feet.",   ch, obj, NULL, TO_ROOM );
	act( "You tie $p on your feet.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_FEET );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
    {
	if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
	    return;
	act( "$n slips $p on to $s hands.",   ch, obj, NULL, TO_ROOM );
	act( "You slip $p on to your hands.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_HANDS );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
    {
	if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
	    return;
	act( "$n wears $p on $s arms.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your arms.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_ARMS );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
    {
	if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
	    return;
	act( "$n wears $p about $s body.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p about your body.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_ABOUT );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
    {
	if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
	    return;
	act( "$n buckles $p around $s waist.",   ch, obj, NULL, TO_ROOM );
	act( "You buckle $p around your waist.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_WAIST );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
    {
	if ( get_eq_char( ch, WEAR_WRIST_L ) != NULL && get_eq_char( ch, WEAR_WRIST_R ) != NULL
        && !remove_obj( ch, WEAR_WRIST_L, fReplace ) && !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
	    return;
	if ( get_eq_char( ch, WEAR_WRIST_L ) == NULL )
	{
            act( "$n wears $p around $s left wrist.", ch, obj, NULL, TO_ROOM );
            act( "You wear $p around your left wrist.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_WRIST_L );
	    return;
	}
	if ( get_eq_char( ch, WEAR_WRIST_R ) == NULL )
	{
            act( "$n wears $p around $s right wrist.", ch, obj, NULL, TO_ROOM );
            act( "You wear $p around your right wrist.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_WRIST_R );
	    return;
	}
	bug( "Wear_obj: no free wrist.", 0 );
	act( "You already wear two wrist items.", ch, NULL, NULL, TO_CHAR );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
    {
	OBJ_DATA *weapon;
	if ( is_affected(ch,gsn_body_weaponry))
	{
	    act("Your hands can't hold a shield.",ch, NULL, NULL, TO_CHAR);
	    return;
	}
	if (!check_sheath(ch, WEAR_SHIELD))
	  return;
	if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
	    return;
	weapon = get_eq_char(ch,WEAR_WIELD);
        if ( !IS_NPC(ch) && get_obj_weight_char(ch, obj) > (str_app[get_curr_stat(ch,STAT_STR)].wield * 12))
	{
	  act( "It is too heavy for you to wield.", ch, NULL, NULL, TO_CHAR );
	  return;
	}
        if (get_eq_char( ch, WEAR_SECONDARY) != NULL
	    && !remove_obj( ch, WEAR_SECONDARY, fReplace)){
	  act("You can't use a shield and two weapons.", ch, NULL, NULL, TO_CHAR);
	  return;
	}
        if (weapon != NULL
	    && ((IS_WEAPON_STAT(weapon,WEAPON_TWO_HANDS)
		 && (ch->size < SIZE_LARGE || obj->value[0] == WEAPON_STAFF))
		|| weapon->value[0] == WEAPON_POLEARM
		|| weapon->item_type == ITEM_INSTRUMENT
		|| is_affected(ch,gsn_double_grip)))
	  {
	    if (is_affected(ch, gsn_double_grip)){
	      act("Your hands are tied up!", ch, NULL, NULL, TO_CHAR);
	      return;
	    }
	    else if ( !remove_obj( ch, WEAR_WIELD, fReplace ) )
	      return;
	    act("`!You are now without primary weapon!``", ch, NULL, NULL, TO_CHAR);
	}
        if (weapon != NULL && get_eq_char(ch, WEAR_HOLD) != NULL)
	  {
	    if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
	    return;
        }
	if ( !remove_obj( ch, WEAR_RANGED, fReplace ) ){
	  act("You cannot use a shield and a ranged weapon.", ch, NULL, NULL, TO_CHAR);
	  return;
	}
	act( "$n wears $p as a shield.", ch, obj, NULL, TO_ROOM );
	act( "You wear $p as a shield.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_SHIELD );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WIELD ) )
    {
	int sn, skill, weight;
	OBJ_DATA *second;

	if (ch->race == race_lookup("illithid"))
	{
	  act("You cannot wield anything!", ch, NULL, NULL, TO_CHAR);
	  return;
	}
	if (is_affected(ch, gen_holy_hands))
	  {
	    act("But your hands are empowered!", ch, NULL, NULL, TO_CHAR);
	    return;
	  }
	if ( is_affected(ch,gsn_body_weaponry))
	{
	  act("Your hands can't hold anything.", ch, NULL, NULL, TO_CHAR);
	  return;
	}
	if ( is_affected(ch,gsn_graft_weapon) && obj->item_type == ITEM_WEAPON)
	{
	  act("You already have a weapon grafted to your hands.", ch, NULL, NULL, TO_CHAR);
	  return;
	}
	if (!check_sheath(ch, WEAR_WIELD))
	  return ;
	if ( is_affected(ch,gsn_double_grip) && !twohands)
	{
	  act( "You're holding your weapon too tight to replace it.", ch, NULL, NULL, TO_CHAR);
	  return;
	}
	weight = (str_app[get_curr_stat(ch,STAT_STR)].wield * 10);
        if ((IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
	     && (ch->size < SIZE_LARGE || obj->value[0] == WEAPON_STAFF))
	     || obj->value[0] == WEAPON_POLEARM
	     || obj->item_type == ITEM_INSTRUMENT
	     || twohands)
	    weight = 3 * weight /2;
	if (obj->item_type == ITEM_WEAPON
	    && get_skill(ch, gsn_2h_handling) > 0
	    && (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
		|| is_affected(ch, gsn_double_grip)) )
	weight = 3 * weight / 2;
        if ( !IS_NPC(ch) && get_obj_weight_char(ch, obj) > weight)
	{
	  act( "It is too heavy for you to wield.", ch, NULL, NULL, TO_CHAR);
	  return;
	}

        if ( ((IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
	       && (ch->size < SIZE_LARGE || obj->value[0] == WEAPON_STAFF))
	       || obj->value[0] == WEAPON_POLEARM
	       || obj->item_type == ITEM_INSTRUMENT)
	      && get_eq_char(ch,WEAR_SHIELD) != NULL)
	{
	  if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
	    return;
	}
        if (((IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
	      && (ch->size < SIZE_LARGE || obj->value[0] == WEAPON_STAFF))
	      || obj->value[0] == WEAPON_POLEARM
	      || obj->item_type == ITEM_INSTRUMENT)
	     && get_eq_char(ch,WEAR_HOLD) != NULL)
	{
	  if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
	    return;
	}
        if (get_eq_char(ch, WEAR_HOLD) != NULL && get_eq_char(ch, WEAR_SHIELD) != NULL)
        {
	  act("You're already holding two items.", ch, NULL, NULL, TO_CHAR);
	  return;
        }
        second = get_eq_char(ch,WEAR_SECONDARY);
        if (second ){
	  /* check for dual wielding restrictions */
	  if (!check_secondary(ch, second, obj)
	      && !remove_obj( ch, WEAR_SECONDARY, fReplace))
	    return;
//safety for no-remove seconds.
	  if (!IS_IMMORTAL(ch)
	      && IS_SET(obj->extra_flags, ITEM_NOREMOVE)){
	    act("You need to be able to remove $p before wielding that.", ch, second, NULL, TO_CHAR);
	    return;
	  }
	}
	if ( !remove_obj( ch, WEAR_WIELD, fReplace ) )
	  return;
	if (IS_NPC(ch) || ch->pcdata->wep_pos == WEPPOS_NORMAL){
	  act( "$n wields $p.", ch, obj, NULL, TO_ROOM );
	  act( "You wield $p.", ch, obj, NULL, TO_CHAR );
	}
	else if (ch->pcdata->wep_pos == WEPPOS_HIGH){
	  act( "$n wields $p and holds it high.", ch, obj, NULL, TO_ROOM );
	  act( "You wield $p and hold it high.", ch, obj, NULL, TO_CHAR );
	}
	else if (ch->pcdata->wep_pos == WEPPOS_LOW){
	  act( "$n wields $p and holds it low.", ch, obj, NULL, TO_ROOM );
	  act( "You wield $p and hold it low.", ch, obj, NULL, TO_CHAR );
	}
	else{
	  act( "$n wields $p.", ch, obj, NULL, TO_ROOM );
	  act( "You wield $p.", ch, obj, NULL, TO_CHAR );
	}
	equip_char( ch, obj, WEAR_WIELD );
        sn = get_weapon_sn(ch, FALSE);
	if (sn == gsn_hand_to_hand)
	   return;
        skill = get_weapon_skill_obj(ch, obj);

        /* Useless conditional */
        if( skill != 0 )
            skill *= 1;

/* Viri: disabled to cut down on spam
         if (skill > 112) act("No mortal can match you with $p in your hand.",ch,obj,NULL,TO_CHAR);
        else if (skill > 100) act("Your skill with $p is unmatched in the land.",ch,obj,NULL,TO_CHAR);
        else if (skill > 95) act("$p feels like a part of you!",ch,obj,NULL,TO_CHAR);
        else if (skill > 85) act("You feel quite confident with $p.",ch,obj,NULL,TO_CHAR);
        else if (skill > 70) act("You are skilled with $p.",ch,obj,NULL,TO_CHAR);
        else if (skill > 50) act("Your skill with $p is adequate.",ch,obj,NULL,TO_CHAR);
        else if (skill > 25) act("$p feels a little clumsy in your hands.",ch,obj,NULL,TO_CHAR);
        else if (skill > 1)  act("You fumble and almost drop $p.",ch,obj,NULL,TO_CHAR);
        else                 act("You don't even know which end is up on $p.",ch,obj,NULL,TO_CHAR);
*/
        if (!str_cmp(obj->material,"practice"))
	  act("You feel that you can improve upon your skills with such a fine practice weapon.", ch, NULL, NULL, TO_CHAR);
	return;
    }
    if ( CAN_WEAR( obj, ITEM_HOLD ) )
    {
	OBJ_DATA *weapon;
	if ( is_affected(ch,gsn_body_weaponry))
	{
	  act("Your hands can't hold anything.", ch, NULL, NULL, TO_CHAR);
	  return;
	}
	if (!check_sheath(ch, WEAR_HOLD))
	  return ;
        if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
	    return;
	weapon = get_eq_char(ch,WEAR_WIELD);
        if (get_eq_char(ch, WEAR_SECONDARY) != NULL)
        {
	  act("You can't hold an item and use two weapons.", ch, NULL, NULL, TO_CHAR);
	  return;
        }
	if (weapon != NULL
	    && ((IS_WEAPON_STAT(weapon,WEAPON_TWO_HANDS)
		 && (ch->size < SIZE_LARGE || obj->value[0] == WEAPON_STAFF))
		|| weapon->value[0] == WEAPON_POLEARM
		|| weapon->item_type == ITEM_INSTRUMENT
		|| is_affected(ch,gsn_double_grip)))
	  {
	    act("Your hands are tied up!", ch, NULL, NULL, TO_CHAR);
	    return;
	  }
        if (weapon != NULL && get_eq_char(ch, WEAR_SHIELD) != NULL)
	  {
            act("You're already holding two items.", ch, NULL, NULL, TO_CHAR);
            return;
	  }
	act( "$n holds $p in $s hand.",   ch, obj, NULL, TO_ROOM );
	act( "You hold $p in your hand.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_HOLD );
	return;
    }
    if ( CAN_WEAR(obj,ITEM_WEAR_FLOAT) )
    {
      if (!remove_obj(ch,WEAR_FLOAT, fReplace) )
	return;
      if (!remove_obj(ch,WEAR_RANGED, fReplace) )
	return;
      act("$n releases $p to float next to $m.",ch,obj,NULL,TO_ROOM);
      act("You release $p and it floats next to you.",ch,obj,NULL,TO_CHAR);
      equip_char(ch,obj,WEAR_FLOAT);
      return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_SHROUD ) && (ch->class == class_lookup("necromancer") || IS_IMMORTAL(ch)))
    {
        if ( !remove_obj( ch, WEAR_SHROUD, fReplace ) )
	    return;
        act( "$n releases the $p.", ch, obj, NULL, TO_ROOM );
        act( "You release $p.",  ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_SHROUD );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_EARRING ) )
    {
	if ( !remove_obj( ch, WEAR_EARRING, fReplace ) )
	    return;
	act( "$n attaches $p to $s earlobes.",   ch, obj, NULL, TO_ROOM );
	act( "You attach $p to your earlobes.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_EARRING );
	return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_RANGED) )
    {
      if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) ){
	act("You cannot use a ranged weapon with a shield in hand.",ch, obj, NULL, TO_CHAR);
	return;
      }
      if ( !remove_obj( ch, WEAR_FLOAT, fReplace ) ){
	act("You cannot use a ranged weapon with an item floating about.",ch, obj, NULL, TO_CHAR);
	return;
      }
      if ( !remove_obj( ch, WEAR_RANGED, fReplace ) )
	return;
      act( "$n slings $p across $s back.",   ch, obj, NULL, TO_ROOM );
      act( "You sling $p across your back.", ch, obj, NULL, TO_CHAR );
      equip_char( ch, obj, WEAR_RANGED );

      if (obj->level > ch->level && ch->level < 50)
	act("You're not quite sure how to handle $p properly.", ch, obj, NULL, TO_CHAR);
      return;
    }
    if ( CAN_WEAR( obj, ITEM_WEAR_QUIVER) )
    {
      OBJ_DATA* ammo = get_eq_char( ch, WEAR_QUIVER );

      /* check if we can add */
      if (ammo && ammo->vnum == obj->vnum && !str_cmp( ammo->name, obj->name) && ammo->condition < 100){
	int transfer = UMIN(obj->condition, 100 - ammo->condition);
	ammo->condition += transfer;
	obj->condition -= transfer;
      }
      else{
	if ( !remove_obj( ch, WEAR_QUIVER, fReplace ) )
	  return;
	equip_char( ch, obj, WEAR_QUIVER );
      }
      act( "$n put $p in $s quiver.",   ch, obj, NULL, TO_ROOM );
      act( "You put $p in your quiver.", ch, obj, NULL, TO_CHAR );

      if (obj->level > ch->level)
	act("You're not quite sure how to handle $p properly.", ch, obj, NULL, TO_CHAR);

      if (obj->condition < 1)
	extract_obj( obj );

      return;
    }
    if ( fReplace )
      act( "You can't wear, wield, or hold that.", ch, NULL, NULL, TO_CHAR);
}

void do_wear( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj;
    one_argument( argument, arg );
    if (is_affected(ch,gsn_bat_form)
	|| is_affected(ch,gsn_wolf_form)
	|| is_affected(ch,gsn_mist_form)
	|| is_affected(ch,gsn_weretiger)
	|| is_affected(ch,gsn_werewolf)
	|| is_affected(ch,gsn_werebear)
	|| is_affected(ch,gsn_werefalcon)){
      send_to_char("Not while you're morphed.\n\r",ch);
      return;
    }
    if ( arg[0] == '\0' )
    {
	send_to_char( "Wear, wield, or hold what?\n\r", ch );
	return;
    }
    if ( !str_cmp( arg, "all" ) )
    {
	OBJ_DATA *obj_next;
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
		wear_obj( ch, obj, FALSE, FALSE );
	}
	return;
    }
    else
    {
	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
	wear_obj( ch, obj, TRUE, FALSE );
    }
}

void do_remove( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj;
    one_argument( argument, arg );
    if (is_affected(ch,gsn_bat_form)
	|| is_affected(ch,gsn_wolf_form)
	|| is_affected(ch,gsn_mist_form)
	|| is_affected(ch,gsn_weretiger)
	|| is_affected(ch,gsn_werewolf)
	|| is_affected(ch,gsn_werebear)
	|| is_affected(ch,gsn_werefalcon)){
      send_to_char("Not while you're morphed.\n\r",ch);
      return;
    }
    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove what?\n\r", ch );
	return;
    }
    if ( !str_cmp( arg, "all" ) )
    {
        OBJ_DATA *obj, *temp = get_eq_char( ch, WEAR_WIELD );
        int iWear;
        for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
	{
            if ( !( obj = get_eq_char( ch, iWear ) ) )
                continue;
            remove_obj( ch, obj->wear_loc, TRUE );
	}
        if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) != NULL && obj != temp)
            remove_obj( ch, obj->wear_loc, TRUE );
	return;
    }
    if ( ( obj = get_obj_wear( ch, arg, ch) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }
    remove_obj( ch, obj->wear_loc, TRUE );
}

void do_sacrifice( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buffer[100];

    OBJ_DATA *obj;
    CHAR_DATA* och;
    AFFECT_DATA* paf;

    int gold, members = 0;
    bool split = TRUE;
    bool fOwnerProt = FALSE;
    CHAR_DATA *gch;
    one_argument( argument, arg );
    if ( arg[0] == '\0' || !str_cmp( arg, ch->name ) )
    {
      if (ch->tattoo)
	{

	  act( "$n offers $mself to $t, who graciously declines.",ch, deity_table[ch->tattoo].god, NULL, TO_ROOM );
	  act( "$t appreciates your offer and may accept it later.",ch, deity_table[ch->tattoo].god, NULL, TO_CHAR );
	}
      else
	{
	  act( "$n offers $mself to $g, who graciously declines.",ch,NULL, NULL, TO_ROOM );
	  act( "$g appreciates your offer and may accept it later.",ch,NULL, NULL, TO_CHAR );
	}

      return;
    }
    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
      {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }
    if ( obj->item_type == ITEM_CORPSE_PC )
    {
      if (ch->tattoo)
        sendf(ch, "%s wouldn't like that.\n\r",deity_table[ch->tattoo].god);
      else
	act( "$g wouldn't like that.",ch, NULL, NULL, TO_CHAR);
      return;
    }
    /* check for owner being ghost */
    if (IS_SET(obj->wear_flags, ITEM_HAS_OWNER)
	&& (paf = affect_find(obj->affected, gen_has_owner)) != NULL
	&& (paf->has_string
	    && (och = get_char(paf->string)) != NULL)
	&& (is_ghost(och, 600)
	    || (obj->in_obj && obj->in_obj->in_room->pCabal)
	    || (obj->in_room->pCabal) )
	&& !IS_IMMORTAL(ch))
      fOwnerProt = TRUE;

    if ( !CAN_WEAR(obj, ITEM_TAKE) || CAN_WEAR(obj, ITEM_NO_SAC) || fOwnerProt)
    {
      act( "You feel $g's anger even as you think about it.", ch, obj, 0, TO_CHAR );
      return;
    }
    if ((obj->item_type == ITEM_CORPSE_NPC
	 || obj->item_type == ITEM_CONTAINER)
	&& obj->contains)
    {
        OBJ_DATA *t_obj, *next_obj;
        for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
        {
            next_obj = t_obj->next_content;
            obj_from_obj(t_obj);
            if (obj->in_obj)
                obj_to_obj(t_obj,obj->in_obj);
            else if (obj->in_room == NULL)
                extract_obj(t_obj);
	    else if (IS_OBJ_STAT(t_obj,ITEM_MELT_DROP))
	    {
	        act("$p dissolves into smoke.",ch,t_obj,NULL,TO_ALL);
	        extract_obj(t_obj);
	    }
            else
                obj_to_room(t_obj,obj->in_room);
        }
    }
    gold = UMAX(1,obj->level * 2);
    if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
        gold = UMIN(gold,obj->cost);
    if (IS_PERK(ch, PERK_PIOUS))
      gold = 3 * gold / 2;
    if (gold == 1)
      {
	if (ch->tattoo)
	  sendf(ch, "%s gives you one gold coin for your sacrifice.\n\r",deity_table[ch->tattoo].god);
	else
	  act("$g gives you one gold coin for your sacrifice.",ch, NULL, NULL, TO_CHAR);
      }
    else
      {
	if (ch->tattoo)
	  sendf(ch,"%s gives you %d gold coins for your sacrifice.\n\r",deity_table[ch->tattoo].god,gold);
	else
	  sendf(ch,"%s gives you %d gold coins for your sacrifice.\n\r",(IS_NPC(ch)? "The One God": (ch->pcdata->deity[0] == '\0'? "The One God" : ch->pcdata->deity)),gold);
      }
    if (ch->class == class_lookup("monk") && (ch->gold + gold) > 20000 && gold > 0)
    {
	int overflow = UMAX(1,ch->gold + gold - 20000);
	ch->gold = 20000;
	send_to_char("You can't hold that much gold and drop it.\n\r",ch);
	obj_to_room( create_money( overflow ), ch->in_room );
	split = FALSE;
    }
    else
    	ch->gold += gold;
    if (ch->tattoo)
      act( "$n sacrifices $p to $T.", ch, obj, deity_table[ch->tattoo].god, TO_ROOM );
    else
      act( "$n sacrifices $p in the name of $T.", ch, obj, path_table[deity_table[IS_NPC(ch) ? 0 : ch->pcdata->way].path].name, TO_ROOM );
    if (IS_SET(ch->act,PLR_AUTOSPLIT) && split)
    {
        for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
            if ( is_same_group( gch, ch ) )
                members++;
        if ( members > 1 && gold > 1)
	{
            sprintf(buffer,"%d",gold);
	    do_split(ch,buffer);
	}
    }
    extract_obj( obj );
}

void do_quaff( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch, *vch_next;
    char arg[MIL];
    OBJ_DATA *obj;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Quaff what?\n\r", ch );
	return;
    }
    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL && ( obj = get_obj_wear( ch, arg, ch) ) == NULL )
    {
	send_to_char( "You do not have that potion.\n\r", ch );
	return;
    }
    if (is_affected(ch, gsn_bat_form) || is_affected(ch, gsn_mist_form) ){
      send_to_char("You cannot do that while morphed.\n\r", ch);
      return;
    }
    /* trap check */
    if (trip_traps(ch, obj->traps)){
      WAIT_STATE2(ch, 4);
      return;
    }
    if ( obj->item_type != ITEM_POTION )
    {
	send_to_char( "You can quaff only potions.\n\r", ch );
	return;
    }
    if ( ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC))
    {
        send_to_char("You are not allowed to use magic!\n\r", ch);
        return;
    }
    /* race check */
    if (!IS_NPC(ch)
	&& obj->race && obj->race != ch->race){
      act("Its size and style make it useless to you.", ch, NULL, NULL, TO_CHAR);
      return;
    }
    /* class */
    if (!IS_NPC(ch)
	&& obj->class >= 0 && obj->class != ch->class){
      act("Its size and style make it useless to you.", ch, NULL, NULL, TO_CHAR);
      return;
    }
    if (is_affected(ch, gsn_gag)){
      send_to_char("The gag stops you from drinking.\n\r", ch);
      return;
    }
    if ( ch->fighting != NULL )
    {
	bool found = FALSE;
	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;
	    if (vch->fighting == ch)
	    {
		found=TRUE;
		continue;
	    }
	}
	if (found)
	{
    	    if ( ( obj = get_obj_wear( ch, arg, ch) ) != NULL )
	    {
		if ( obj->item_type != ITEM_POTION)
	    	{
		    send_to_char( "You need to hold the potion in order to quaff it while being attacked.\n\r", ch);
	    	    return;
		}
		else if (number_percent() >= (get_curr_stat(ch,STAT_DEX) * 4) )
		{
		    WAIT_STATE(ch,6);
		    send_to_char("You fumble and drop the potion, spilling its contents.\n\r",ch);
		    obj_from_char(obj);
		    return;
		}
	    }
	    else
	    {
		send_to_char( "You need to hold the potion in order to quaff it while being attacked.\n\r", ch);
	    	return;
	    }
	}
    }
    WAIT_STATE(ch,4);
    act( "$n quaffs $p.", ch, obj, NULL, TO_ROOM );
    act( "You quaff $p.", ch, obj, NULL ,TO_CHAR );
    obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
    obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
    obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
    extract_obj( obj );
}

void do_recite( CHAR_DATA *ch, char *argument )
{
  char arg1[MIL];
    CHAR_DATA *victim, *vch, *vch_next;
    OBJ_DATA *scroll, *obj;
    argument = one_argument( argument, arg1 );

    if ( ch->pCabal && IS_CABAL(get_parent(ch->pCabal), CABAL_NOMAGIC))
    {
        send_to_char("That is not how a Warlord should behave!\n\r", ch);
        return;
    }
    if ( ch->fighting != NULL )
    {
	send_to_char("You can't concentrate enough.",ch);
	return;
    }
    if ( (scroll = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	send_to_char( "You do not have that scroll.\n\r", ch );
	return;
    }
    if ( scroll->item_type != ITEM_SCROLL )
    {
	send_to_char( "You can recite only scrolls.\n\r", ch );
	return;
    }
    if ( (!IS_NPC(ch) && ch->level < scroll->level) || (IS_NPC(ch) && get_trust(ch) < scroll->level) )
    {
        send_to_char("This scroll is too complex for you to comprehend.\n\r",ch);
	return;
    }
    obj = NULL;
    if ( IS_NULLSTR(argument))
      victim = ch;
    else if ( ( victim = get_char_room ( ch, NULL, argument ) ) == NULL
	      && ( obj = get_obj_here( ch, NULL, argument ) ) == NULL )
      {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    target_name = argument;

    if (obj != NULL && CAN_WEAR(obj,ITEM_WEAR_TATTOO))
    {
	send_to_char( "You can't find it.\n\r",ch);
	return;
    }
    if ( ch->fighting != NULL )
    {
	bool found = FALSE;
	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;
	    if (vch->fighting == ch)
	    {
		found = TRUE;
		continue;
	    }
	}
	if (found)
	{
	    send_to_char( "You cannot recite scrolls while being attacked.\n\r", ch);
	    return;
	}
    }
    if ( get_skill(ch,gsn_scrolls) == 0)
    {
	send_to_char("You do not possess the skill to use scrolls.\n\r",ch);
	return;
    }
    if (trip_traps(ch, scroll->traps))
      return;
    act( "$n recites $p.", ch, scroll, NULL, TO_ROOM );
    act( "You recite $p.", ch, scroll, NULL, TO_CHAR );
    WAIT_STATE(ch,8);
    if (number_percent() - (get_curr_stat(ch,STAT_LUCK)-16) >= 20 + get_skill(ch,gsn_scrolls) * 4/5)
    {
	send_to_char("You mispronounce a syllable.\n\r",ch);
	check_improve(ch,gsn_scrolls,FALSE, 20);
    }
    else
    {
    	obj_cast_spell( scroll->value[1], scroll->value[0], ch, victim, obj );
    	obj_cast_spell( scroll->value[2], scroll->value[0], ch, victim, obj );
    	obj_cast_spell( scroll->value[3], scroll->value[0], ch, victim, obj );
	check_improve(ch,gsn_scrolls,TRUE, 5);
    }
    extract_obj( scroll );
}

void do_brandish( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch, *vch_next;
    OBJ_DATA *staff, *obj;
    int sn;
    int chance = get_skill(ch,gsn_staves);

    if ( ch->pCabal && IS_CABAL(get_parent(ch->pCabal), CABAL_NOMAGIC))
    {
        send_to_char("You are not allowed to use magic!\n\r", ch);
        return;
    }
    if ( ( staff = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }
    if ( staff->item_type != ITEM_STAFF )
    {
	send_to_char( "You can brandish only with a staff.\n\r", ch );
	return;
    }
    if ( ( sn = staff->value[3] ) < 0 || sn >= MAX_SKILL || skill_table[sn].spell_fun == 0 )
    {
	bug( "Do_brandish: bad sn %d.", sn );
	return;
    }
    if (IS_OBJ_STAT(staff, ITEM_USE))
      chance = UMIN(3 * ch->level, 95);

    if ( chance == 0)
    {
	send_to_char("You do not possess the skill to use staves.\n\r",ch);
	return;
    }
    WAIT_STATE( ch, 18 );
    if (trip_traps(ch, staff->traps))
      return;
    if ( staff->value[2] > 0 || staff->value[2] == -1 )
    {
	act( "$n brandishes $p.", ch, staff, NULL, TO_ROOM );
	act( "You brandish $p.",  ch, staff, NULL, TO_CHAR );
        if ( ch->level < staff->level || number_percent() - (get_curr_stat(ch,STAT_LUCK)-16) >= 20 + chance * 4/5)
 	{
	    act ("You fail to invoke $p.",ch,staff,NULL,TO_CHAR);
	    act ("...and nothing happens.",ch,NULL,NULL,TO_ROOM);
	    check_improve(ch,gsn_staves,FALSE,20);
	}
	else for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next	= vch->next_in_room;
	    switch ( skill_table[sn].target )
	    {
	    default:
		bug( "Do_brandish: bad target for sn %d.", sn );
		return;
	    case TAR_IGNORE:
		if ( vch != ch )
		    continue;
		break;
	    case TAR_CHAR_OFFENSIVE:
		if ( IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch) )
		    continue;
		break;
	    case TAR_CHAR_DEFENSIVE:
		if (!is_same_group(ch,vch))
		    continue;
		if ( vch->pCabal && IS_CABAL(get_parent(vch->pCabal), CABAL_NOMAGIC))
		  continue;
		break;
	    case TAR_CHAR_SELF:
		if ( vch != ch )
		    continue;
		break;
	    case TAR_OBJ_INV:
	      if (IS_NULLSTR(argument)){
		send_to_char("Brandish on what?\n\r", ch);
		return;
	      }
	      else if ( (obj = get_obj_carry(ch, argument, ch)) == NULL){
		send_to_char("You do carry that.\n\r", ch);
		return;
	      }
	      obj_cast_spell( staff->value[3], staff->value[0], ch, NULL, obj );
	      check_improve(ch,gsn_staves,TRUE,5);
	      if ( staff->value[2] != -1 && --staff->value[2] <= 0 )
		{
		  act( "$n's $p blazes bright and is gone.", ch, staff, NULL, TO_ROOM );
		  act( "Your $p blazes bright and is gone.", ch, staff, NULL, TO_CHAR );
		  extract_obj( staff );
		}
	      return;
	    }
	    obj_cast_spell( staff->value[3], staff->value[0], ch, vch, NULL );
	    check_improve(ch,gsn_staves,TRUE,5);
	}
    }
    if ( staff->value[2] != -1 && --staff->value[2] <= 0 )
      {
	act( "$n's $p blazes bright and is gone.", ch, staff, NULL, TO_ROOM );
	act( "Your $p blazes bright and is gone.", ch, staff, NULL, TO_CHAR );
	extract_obj( staff );
      }
}

void do_use( CHAR_DATA *ch, char *argument ){
  char name[MIL];
  OBJ_DATA *wand;

  argument = one_argument( argument, name );
  if (IS_NULLSTR(name)){
    send_to_char("syntax: use <object> [target]\n\r", ch);
    return;
  }
  /*
    check in following order:
    - held item
    - worn item
    - item in room
  */
  if ( ( wand = get_eq_char( ch, WEAR_HOLD ) ) != NULL
       && is_name(name, wand->name)){
/* TRAP check */
    if (trip_traps(ch, wand->traps))
      return;
    if ( wand->item_type == ITEM_WAND )
      do_zap(ch, argument);
    else if ( wand->item_type == ITEM_STAFF)
      do_brandish(ch, argument);
    else if ( wand && HAS_TRIGGER_OBJ( wand, TRIG_USE ) ){
      p_percent_trigger( NULL, wand, NULL, ch, NULL, NULL, TRIG_USE );
      WAIT_STATE2( ch, PULSE_VIOLENCE);
    }
    else
      send_to_char("This item cannot be used in such way.\n\r", ch);
    return;
  }
  /* check for worn object */
  if ( (wand = get_obj_wear( ch, name, ch )) != NULL){
    act("You attempt to use $p.", ch, wand, NULL, TO_CHAR);
    act("$n attempts to use $p.", ch, wand, NULL, TO_ROOM);
/* TRAP check */
    if (trip_traps(ch, wand->traps))
      return;
    if ( wand && HAS_TRIGGER_OBJ( wand, TRIG_USE ) )
      p_percent_trigger( NULL, wand, NULL, ch, NULL, NULL, TRIG_USE );
    WAIT_STATE2( ch, PULSE_VIOLENCE);
  }
  /* check room for object */
  else if ( (wand = get_obj_list( ch, name, ch->in_room->contents )) != NULL){
    act("You attempt to use $p.", ch, wand, NULL, TO_CHAR);
    act("$n attempts to use $p.", ch, wand, NULL, TO_ROOM);
/* TRAP check */
    if (trip_traps(ch, wand->traps))
      return;
    if ( wand && HAS_TRIGGER_OBJ( wand, TRIG_USE ) )
      p_percent_trigger( NULL, wand, NULL, ch, NULL, NULL, TRIG_USE );
    WAIT_STATE2( ch, PULSE_VIOLENCE);
  }
  else if ( (wand = get_obj_carry( ch, name, ch )) != NULL){
    send_to_char("You may only use objects that are worn or are on the ground.\n\r", ch);
    return;
  }
  else
    send_to_char("You don't see that here.\n\r", ch);
}

void do_zap( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    OBJ_DATA *wand, *obj;
    int chance = get_skill(ch, gsn_wands);
    argument = one_argument( argument, arg );

    if ( ch->pCabal && IS_CABAL(get_parent(ch->pCabal), CABAL_NOMAGIC))
    {
        send_to_char("You are not allowed to use magic!\n\r", ch);
        return;
    }
    if ( arg[0] == '\0' && ch->fighting == NULL )
    {
	send_to_char( "Zap whom or what?\n\r", ch );
	return;
    }
    if ( ( wand = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }
    if ( wand->item_type != ITEM_WAND )
    {
	send_to_char( "You can zap only with a wand.\n\r", ch );
	return;
    }
    obj = NULL;
    if ( arg[0] == '\0' )
    {
	if ( ch->fighting != NULL )
	    victim = ch->fighting;
	else
	{
	    send_to_char( "Zap whom or what?\n\r", ch );
	    return;
	}
    }
    else if ( ( victim = get_char_room ( ch, NULL, arg ) ) == NULL && ( obj = get_obj_here( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    if (IS_OBJ_STAT(wand, ITEM_USE))
      chance = UMIN(3 * ch->level, 95);

    if ( chance == 0)
    {
	send_to_char("You do not possess the skill to use wands.\n\r",ch);
	return;
    }
    WAIT_STATE( ch, PULSE_VIOLENCE / 2);
    if (trip_traps(ch, wand->traps))
      return;
    if ( wand->value[2] > 0  || wand->value[2] == -1 )
    {
	if ( victim != NULL )
	{
	    act( "$n zaps $N with $p.", ch, wand, victim, TO_NOTVICT );
	    act( "You zap $N with $p.", ch, wand, victim, TO_CHAR );
	    act( "$n zaps you with $p.",ch, wand, victim, TO_VICT );
	}
	else
	{
	    act( "$n zaps $P with $p.", ch, wand, obj, TO_ROOM );
	    act( "You zap $P with $p.", ch, wand, obj, TO_CHAR );
	}
        if (ch->level < wand->level || number_percent() - (get_curr_stat(ch,STAT_LUCK)-16) >= 20 + chance * 4/5)
	{
            act( "Your efforts with $p produce only smoke and sparks.",ch,wand,NULL,TO_CHAR);
            act( "$n's efforts with $p produce only smoke and sparks.",ch,wand,NULL,TO_ROOM);
	    check_improve(ch,gsn_wands,FALSE, 20);
	}
	else
	{
	    obj_cast_spell( wand->value[3], wand->value[0], ch, victim, obj );
	    check_improve(ch,gsn_wands,TRUE,5);
	}
    }
    if ( wand->value[2] != -1 && --wand->value[2] <= 0 )
    {
	act( "$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM );
	act( "Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR );
	extract_obj( wand );
    }
}

void do_steal( CHAR_DATA *ch, char *argument )
{
    char buf[MSL], arg1[MIL], arg2[MIL];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ((percent = get_skill(ch,gsn_steal)) == 0)
    {
	send_to_char("You're in the wrong profession.\n\r",ch);
	return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Steal what from whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG) )
    {
	sendf(ch, "%s is too alert for you to steal from them.\n\r", PERS2(victim));
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "That's pointless.\n\r", ch );
	return;
    }
    if (is_safe(ch,victim))
	return;
    if (!IS_NPC(victim) && victim->level < 15)
    {
	send_to_char("Not that victim.\n\r", ch);
	return;
    }
    if (!IS_NPC(victim) && ch->level < 15)
    {
	send_to_char("Not at your stage.\n\r", ch);
	return;
    }
    if (victim->position == POS_FIGHTING)
    {
        send_to_char( "You'd better not -- you might get hit.\n\r",ch);
	return;
    }
    /* THEFT CRIME CHECK */
    if (ch->in_room && is_crime(ch->in_room, CRIME_THEFT, victim)){
      set_crime(ch, victim, ch->in_room->area, CRIME_THEFT);
    }
    if (IS_NPC(victim) && !IS_AWAKE(victim) && victim->pIndexData->pShop != NULL)
    {
	send_to_char( "Oops.\n\r", ch );
	act( "$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
        victim->position = POS_STANDING;
        act("$n wakes up, $s face a mask of rage.",victim,NULL,NULL,TO_ROOM);
        sprintf( buf, "Keep your hands out of there, %s!", PERS(ch,victim));
	REMOVE_BIT(victim->comm,COMM_NOCHANNELS);
	REMOVE_BIT(victim->comm,COMM_NOYELL);
	j_yell(victim,buf);
	SET_BIT(victim->comm,COMM_NOYELL);
	SET_BIT(victim->comm,COMM_NOCHANNELS);
	multi_hit(victim,ch,TYPE_UNDEFINED);
    	return;
    }
    WAIT_STATE( ch, skill_table[gsn_steal].beats );
    percent = 3 * percent /4;
    if (!IS_AWAKE(victim))
      percent = 100;
    else if (!can_see(victim,ch))
      percent += 15;
    percent += 2*(get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    if (is_affected(victim, gen_watchtower)){
      sendf( ch, "%s is shielded within the Watchtower.\n\r", PERS(victim, ch ));
      sendf( victim, "%s just tried to steal from you.\n\r", PERS2(ch));
      percent = 0;
    }

    if ( (ch->level + 8 < victim->level ) || ( !IS_NPC(ch) && number_percent() > percent) )
      {
	send_to_char( "Oops.\n\r", ch );
	do_visible(ch, "");
	act( "$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT    );
	act( "$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
	switch(number_range(0,3)){
	case 0 :
	  sprintf( buf, "%s is a lousy thief!", PERS(ch,victim));
	  break;
        case 1 :
	  sprintf( buf, "%s couldn't rob %s way out of a paper bag!",PERS(ch,victim),(ch->sex == 2) ? "her" : "his");
	  break;
	case 2 :
	  sprintf( buf,"%s tried to rob me!",PERS(ch,victim));
	  break;
	case 3 :
	  sprintf(buf,"Keep your hands out of there, %s!",PERS(ch,victim));
	  break;
        }
	if (IS_AWAKE(victim)){
	  if (IS_NPC(victim)){
	    REMOVE_BIT(victim->comm,COMM_NOCHANNELS);
	    REMOVE_BIT(victim->comm,COMM_NOYELL);
	    j_yell(victim,buf);
	    SET_BIT(victim->comm,COMM_NOYELL);
	    SET_BIT(victim->comm,COMM_NOCHANNELS);
	  }
	  else
	    j_yell(victim, buf );
	}
	if ( !IS_NPC(ch) ){
	  if ( IS_NPC(victim) ){
	    check_improve(ch,gsn_steal,FALSE,2);
	    do_murder(victim, ch->name);
	  }
	  else{
	    sprintf(buf,"$N tried to steal from %s.",victim->name);
	    wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
	  }
	}
	if (!IS_NPC(victim))
	  set_delay(ch,victim);
	return;
      }
    set_delay(ch,NULL);
    if ( !str_cmp( arg1, "coin"  ) || !str_cmp( arg1, "coins" ) || !str_cmp( arg1, "gold"  ) )
      {
        int amount;
        amount = victim->gold * number_range(1, ch->level) / 100;
        if ( amount <= 0 )
	  {
            send_to_char( "You couldn't get any gold.\n\r", ch );
	    return;
	  }
        ch->gold        += amount;
        victim->gold    -= amount;
        sendf( ch, "Bingo!  You got %d gold coins.\n\r",amount);
	check_improve(ch,gsn_steal,TRUE,2);
	return;
      }
    if ( ( obj = get_obj_carry( victim, arg1, ch ) ) == NULL )
      {
	send_to_char( "You can't find it.\n\r", ch );
	return;
      }
    if ( !can_drop_obj( ch, obj ) || IS_SET(obj->extra_flags, ITEM_INVENTORY) )
    {
	send_to_char( "You can't pry it away.\n\r", ch );
	return;
    }
    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	send_to_char( "You have your hands full.\n\r", ch );
	return;
    }
    if ( ch->carry_weight + get_obj_weight_char(ch, obj ) > can_carry_w( ch ) )
    {
	send_to_char( "You can't carry that much weight.\n\r", ch );
	return;
    }
    obj_from_char( obj );
    obj_to_char( obj, ch );
    check_improve(ch,gsn_steal,TRUE,2);
    send_to_char( "Got it!\n\r", ch );
}

void do_request( CHAR_DATA *ch, char *argument )
{
    char buf [MSL];
    char arg1 [MIL], arg2 [MIL];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    AFFECT_DATA* paf;

    /*
LAG TIME:
the const. are base amounts, beyond that we add +1 for each 3 level above ch or
- 1 for each level below.
    */

//const
    const int reg_lag = 2;
    const int rare_lag = 4;
    const int uniq_lag = 6;
    const int high_mod = 3;//divisor for positive elvel dif.
    const int low_mod = 10;//divisor for negative level diff.
//data
    int lag_time = 0;
    int level_dif = 0;

    OBJ_DATA *obj, *obj2;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if (IS_NPC(ch))
	return;
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Request what from whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "That's pointless.\n\r", ch );
	return;
    }
    if (!IS_NPC(victim))
    {
	send_to_char("Why don't you just try asking.\n\r",ch);
	return;
    }

    if (victim->position == POS_FIGHTING)
    {
	sendf(ch, "%s is a bit occupied at the moment.\n\r", PERS2(victim));
	return;
    }
    if (!IS_AWAKE(victim))
    {
	sendf(ch, "%s is asleep right now.\n\r", PERS2(victim));
	return;
    }
    act("$n attempts to request from $N.",ch,NULL,victim,TO_ROOM);
    if (!can_see(victim,ch))
    {
	do_say(victim, "Eh?  Who's there?");
	return;
    }
    WAIT_STATE(ch, 12);
    if (!IS_GOOD(victim))
    {
	do_say(victim,"I'm not about to give you anything!");
	return;
    }
    if (!IS_GOOD(ch))
    {
	do_say(victim, "I won't give anything to someone so impure.");
	return;
    }
    if (ch->hit < 3 * (ch->level / 2) || ch->move < (50 + ch->level))
    {
	do_say(victim, "You look a bit tired, why don't you rest for a while.");
	return;
    }
    if ( (ch->level + 9 < victim->level && ch->level < 50) || victim->level >= ch->level *2)
    {
	do_say(victim, "In due time my child.");
	return;
    }
    //check for item on char.

    if ( ( obj = get_obj_wear( victim, arg1, victim) ) == NULL)
    {

      if   ( ( obj = get_obj_carry(victim, arg1, ch)) == NULL)
	{
	  do_say(victim, "Sorry, I'm afraid I don't have that item.");
	  return;
	}
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL
	&& IS_OBJ_STAT(obj,ITEM_INVENTORY)){
      do_say(victim, "You will have to buy it from me.");
      return;
    }
    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	do_say(victim, "Your hands are full.");
	return;
    }
    if ( ch->carry_weight + get_obj_weight_char(ch, obj ) > can_carry_w( ch ) )
    {
	do_say(victim, "You are carrying too much weight.");
	return;
    }
    if (!can_see_obj(ch, obj))
    {
	do_say(victim, "But you can't see it!");
	return;
    }
    if (CAN_WEAR(obj, ITEM_UNIQUE) || CAN_WEAR(obj, ITEM_RARE)){
      if (obj->wear_loc != WEAR_NONE)
	unequip_char(victim, obj);
      obj2 = obj;
    }
    else
      obj2 = create_object( obj->pIndexData, obj->level );

    act("$N places $p on the ground before $n and mutters a blessing.",ch,obj2,victim,TO_ROOM);
    act("$N places $p on the ground before you and blesses it.",ch,obj2,victim,TO_CHAR);
    sprintf(buf, "May %s find you worthy of my gift.", (ch->pcdata->deity[0] == '\0'? "The One God": ch->pcdata->deity));

    do_say(victim, buf);

    //we set the owner, including the deity.
    set_owner(ch, obj2, ch->pcdata->deity);

    //calculate the base lag
    if (CAN_WEAR(obj2, ITEM_UNIQUE))
      lag_time = uniq_lag;
    else if (CAN_WEAR(obj2, ITEM_RARE))
      lag_time = rare_lag;
    else
      lag_time = reg_lag;
    //include factores for lvl diff.

    level_dif = victim->level - ch->level;
    lag_time += (level_dif > 0? level_dif / high_mod : level_dif / low_mod);

    //we set the request gen on the character.
    af.type = gen_request;
    af.where = TO_NONE;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.duration = UMAX(0, lag_time);
    af.level = victim->level;
    af.bitvector = 0;


    //now we set tings on the bitvector if needed.
    if (obj2->timer > 0)
      {
	af.level = obj2->timer;
	SET_BIT(af.bitvector, ITEM_HAD_TIMER);
      }
    /* Not needed : Viri
    else
      obj2->timer = lag_time * 2;
    */

    if (CAN_WEAR(obj2, ITEM_NO_SAC))
      SET_BIT(af.bitvector, ITEM_NO_SAC);
    else
      SET_BIT(obj2->wear_flags, ITEM_NO_SAC);

    //now we begin request on character.
    paf = affect_to_char(ch, &af);
    //and we enter the name of obj2,
    if (paf != NULL)
      string_to_affect(paf, obj2->name);

    if (obj2->carried_by){
      obj_from_char( obj2 );
    }
      obj_to_room( obj2, ch->in_room );
}

CHAR_DATA *find_keeper( CHAR_DATA *ch )
{
    char buf[MSL];
    CHAR_DATA *keeper;
    SHOP_DATA *pShop;
    pShop = NULL;
    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
	if ( IS_NPC(keeper) && IS_AWAKE(keeper) && (pShop = keeper->pIndexData->pShop) != NULL)
	    break;
    if ( pShop == NULL )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return NULL;
    }
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_WANTED)
	 && keeper->in_room && IS_SET(keeper->in_room->area->area_flags, AREA_LAWFUL)
	 && !is_same_cabal(keeper->pCabal, ch->pCabal))
    {
      if (guard_fade_check(ch))
	send_to_char("You manage to fade into shadows and avoid detection.\n\r", ch);
      else{
	sprintf(buf,"Criminals are not welcome!" );
        do_say( keeper, buf);
        sprintf( buf, "%s the criminal is over here!\n\r", ch->name );
        REMOVE_BIT(keeper->comm,COMM_NOYELL);
	j_yell( keeper, buf );
        SET_BIT(keeper->comm,COMM_NOYELL);
	return NULL;
      }
    }
    if ( mud_data.time_info.hour < pShop->open_hour )
    {
	if (IS_AWAKE(keeper))
	{
	    sprintf(buf, "Sorry, I am closed. Come back later." );
	    do_say( keeper,buf);
	}
	return NULL;
    }
    if ( mud_data.time_info.hour > pShop->close_hour )
    {
	if (IS_AWAKE(keeper))
	{
	    sprintf(buf,"Sorry, I am closed. Come back tomorrow." );
	    do_say( keeper, buf);
	}
	return NULL;
    }
    if ( !can_see( keeper, ch ) && !IS_IMMORTAL(ch))
    {
	sprintf(buf,"I don't trade with folks I can't see." );
	do_say( keeper, buf);
	return NULL;
    }
    return keeper;
}

CHAR_DATA *find_history( CHAR_DATA *ch )
{
    char buf[MSL];
    CHAR_DATA *keeper;
    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
	if ( IS_NPC(keeper) && IS_AWAKE(keeper) && IS_SET(keeper->act2,ACT_HISTORY))
	    break;
    if ( keeper == NULL )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return NULL;
    }
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_WANTED)
	 && !is_same_cabal(keeper->pCabal,ch->pCabal) )
    {
      if (guard_fade_check(ch))
	send_to_char("You manage to fade into shadows and avoid detection.\n\r", ch);
      else{
	sprintf( buf,"Criminals are not welcome!" );
        do_say( keeper, buf);
        sprintf( buf, "%s the criminal is over here!\n\r", ch->name );
        REMOVE_BIT(keeper->comm,COMM_NOYELL);
	j_yell( keeper, buf );
        SET_BIT(keeper->comm,COMM_NOYELL);
	return NULL;
      }
    }
    if ( !can_see( keeper, ch ) && !IS_IMMORTAL(ch))
    {
	sprintf(buf,"I don't talk to folks I can't see." );
	do_say( keeper, buf);
	return NULL;
    }
    return keeper;
}

void obj_to_keeper( OBJ_DATA *obj, CHAR_DATA *ch )
{
    OBJ_DATA *t_obj, *t_obj_next;
    for (t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next)
    {
	t_obj_next = t_obj->next_content;
	if (obj->pIndexData == t_obj->pIndexData && !str_cmp(obj->short_descr,t_obj->short_descr))
	{
	    if (IS_OBJ_STAT(t_obj,ITEM_INVENTORY))
	    {
		extract_obj(obj);
		return;
	    }
            obj->cost = t_obj->cost;
	    break;
	}
    }
    if (t_obj == NULL)
    {
	obj->next_content = ch->carrying;
	ch->carrying = obj;
    }
    else
    {
	obj->next_content = t_obj->next_content;
	t_obj->next_content = obj;
    }
    obj->carried_by      = ch;
    obj->in_room         = NULL;
    obj->in_obj          = NULL;
    if (obj->vnum != OBJ_VNUM_LOCKER){
      ch->carry_number    += get_obj_number( obj );
      ch->carry_weight    += get_obj_weight_char(ch, obj );
    }
}

OBJ_DATA *get_obj_keeper( CHAR_DATA *ch, CHAR_DATA *keeper, char *argument)
{
    char arg[MIL];
    OBJ_DATA *obj;
    int number = number_argument( argument, arg ), count = 0;

    for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
        if (obj->wear_loc == WEAR_NONE
	    && can_see_obj( keeper, obj )
	    && can_see_obj(ch,obj)
	    && (is_name(arg, obj->name) || is_auto_name( arg, obj->name )) ){
	  if ( ++count == number )
	    return obj;
	  while (obj->next_content != NULL
		 && obj->pIndexData == obj->next_content->pIndexData
		 && !str_cmp(obj->short_descr,obj->next_content->short_descr))
	    obj = obj->next_content;
        }
    return NULL;
}

int get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
    SHOP_DATA *pShop;
    int cost = 0;
    if ( obj == NULL || ( pShop = keeper->pIndexData->pShop ) == NULL )
	return 0;
    if ( fBuy ){
	cost = obj->cost * pShop->profit_buy  / 100;
	/* Global Cabal modification */
	if (keeper->group)
	  cost = UMAX(1, CABAL_PROFIT * cost / 100);
    }
    else
    {
	OBJ_DATA *obj2;
	int itype;
	for ( itype = 0; itype < MAX_TRADE; itype++ )
	    if ( obj->item_type == pShop->buy_type[itype] )
	    {
		cost = obj->cost * pShop->profit_sell / 100;
		break;
	    }
	if (!IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT))
	    for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
	    	if ( obj->pIndexData == obj2->pIndexData && !str_cmp(obj->short_descr,obj2->short_descr) )
                {
	 	    if (IS_OBJ_STAT(obj2,ITEM_INVENTORY))
			cost /= 2;
		    else
                    	cost = cost * 3 / 4;
                }
	/* cant sell cabal items */
	if (obj->pIndexData->pCabal)
	  return 0 ;
    }
    if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
    {
	if (obj->value[1] == 0)
	    cost /= 4;
	else
	    cost = cost * obj->value[2] / obj->value[1];
    }
    return cost;
}

void pet_shop(CHAR_DATA *ch, char *argument ){
  char arg[MIL], buf[MIL];
  CHAR_DATA *pet;
  ROOM_INDEX_DATA *pRoomIndexNext, *in_room;
  int cost;
  int roll = number_percent();

  if ( IS_NPC(ch) )
    return;
  else if (is_exiled(ch, ch->in_room->area->vnum )){
    char buf[MIL];
      sprintf( buf, "No one will deal with you in %s!", ch->in_room->area->name);
      send_to_char(buf, ch );
      return;
  }
  argument = one_argument(argument,arg);
  if (ch->in_room->vnum == 9621)
    pRoomIndexNext = get_room_index(9706);
  else
    pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
  if ( pRoomIndexNext == NULL )
    {
      bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
      send_to_char( "Sorry, you can't buy that here.\n\r", ch );
      return;
    }
  in_room     = ch->in_room;
  ch->in_room = pRoomIndexNext;
  pet         = get_char_room( ch, NULL, arg );
  ch->in_room = in_room;
  if ( pet == NULL || !IS_SET(pet->act, ACT_PET) )
    {
      send_to_char( "Sorry, you can't buy that here.\n\r", ch );
      return;
    }
  if ( ch->pet != NULL )
    {
      send_to_char("You already own a pet.\n\r",ch);
      return;
    }

  if (ch->pcdata->familiar)
    {
	act("You already have $N as a personal follower.\n\r", ch, NULL, ch->pcdata->familiar, TO_CHAR);
	return;
    }

    cost = 10 * pet->level * pet->level;
    if ( ch->gold < cost )
      {
	send_to_char( "You can't afford it.\n\r", ch );
	return;
      }
    if ( ch->level < pet->level )
      {
	send_to_char("You're not powerful enough to master this pet.\n\r", ch );
	return;
      }
    if (get_charmed_by(ch) >= 3)
      {
	send_to_char("Are you sure you need a whole army at your command?\n\r", ch);
	return;
      }
    if (roll < get_skill(ch,gsn_haggle))
      {
	cost = cost / 2 + (cost * roll / 200);
	sendf(ch, "You haggle the price down to %d coins.\n\r",cost);
	check_improve(ch,gsn_haggle,TRUE,1);
      }
    ch->gold           -= cost;
    pet			= create_mobile( pet->pIndexData );
    SET_BIT(pet->act, ACT_PET);
    SET_BIT(pet->affected_by, AFF_CHARM);
    pet->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
    sprintf( buf, "%sA neck tag says 'I belong to %s'.\n\r", pet->description, ch->name );
    free_string( pet->description );
    pet->description = str_dup( buf );
    char_to_room( pet, ch->in_room );
    add_follower( pet, ch );
    pet->leader = ch;
    pet->summoner = ch;
    ch->pet = pet;
    send_to_char( "Enjoy your pet.\n\r", ch );
    act( "$n bought $N as a pet.", ch, NULL, pet, TO_ROOM );
}

void do_sell( CHAR_DATA *ch, char *argument ){
  char buf[MSL], arg[MIL];
  CHAR_DATA *keeper;
  OBJ_DATA *obj;
  int cost, roll = number_percent();
  one_argument( argument, arg );

  if ( arg[0] == '\0' ){
    send_to_char( "Sell what?\n\r", ch );
    return;
  }

    if ( ( keeper = find_keeper( ch ) ) == NULL )
      return;
    else if (is_exiled(ch, keeper->in_room->area->vnum )){
      char buf[MIL];
      sprintf( buf, "No one in %s will deal with you %s!", keeper->in_room->area->name, PERS2(ch));
      do_say( keeper, buf );
      return;
    }
    else if ( keeper->pCabal && !_is_cabal( ch->pCabal, keeper->pCabal )){
      char buf[MIL];
      sprintf( buf, "You must be a member of %s`# to shop here %s.", keeper->pCabal->who_name, PERS2( ch ));
      do_say( keeper, buf );
      return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL ){
      sendf(ch, "%s tells you '`2You don't have that item.``'\n\r", PERS(keeper,ch));
      return;
    }
    else if ( !can_drop_obj( ch, obj ) ){
      send_to_char( "You can't let go of it.\n\r", ch );
      return;
    }
    else if (!can_see_obj(keeper,obj)){
      sendf(ch, "%s doesn't see what you are offering.\n\r",PERS(keeper,ch));
      return;
    }
    else if (IS_LIMITED(obj) || IS_SET(obj->wear_flags, ITEM_HAS_OWNER)){
      sendf(ch, "%s tells you '`2I am not worthy to own such a powerful item.``'\n\r", PERS(keeper,ch));
      return;
    }
    else if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 ){
      act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
      return;
    }
    else if ( cost > keeper->gold ){
      act( "$n tells you '`2I'm afraid I don't have enough gold to buy $p.``'", keeper,obj,ch,TO_VICT);
      return;
    }
    roll -= 2 * (get_curr_stat(ch,STAT_LUCK) - 14);
    if (is_song_affected(ch,gsn_reputation))
      roll -= 20;
    roll = UMAX(1,roll);
    act( "$n sells $p.", ch, obj, NULL, TO_ROOM );

    if (!IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT)
	&& roll < get_skill(ch,gsn_haggle)){
      send_to_char("You haggle with the shopkeeper for a better price.\n\r",ch);
      int haggle_cost = UMAX(0, roll * (obj->cost - cost) / 100);
      cost += haggle_cost;
      cost = UMIN(cost, keeper->gold);
      check_improve(ch,gsn_haggle,TRUE,0);
    }
    sprintf( buf, "You sell $p for %d gold piece%s.", cost, cost == 1 ? "" : "s" );
    act( buf, ch, obj, NULL, TO_CHAR );
    if (ch->class == class_lookup("monk") && (ch->gold + cost) > 20000 && cost > 0)
      {
	int overflow = UMAX(1,ch->gold + cost - 20000);
	ch->gold = 20000;
        send_to_char("You can't hold that much gold and drop it.\n\r",ch);
	act( "$n drops some gold.", ch, NULL, NULL, TO_ROOM );
	obj_to_room( create_money( overflow ), ch->in_room );
      }
    else
      ch->gold     += cost;
    keeper->gold -= cost;
    if ( keeper->gold < 0 )
      keeper->gold = 0;
    if ( obj->item_type == ITEM_TRASH || IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT))
      extract_obj( obj );
    else
      {
	obj_from_char( obj );
	if (obj->timer)
	  SET_BIT(obj->extra_flags,ITEM_HAD_TIMER);
	else
	  obj->timer = number_range(50,100);
	obj_to_keeper( obj, keeper );
      }
}


void do_buy( CHAR_DATA *ch, char *argument ){
  CHAR_DATA *keeper;
  OBJ_DATA *obj, *tobj;
  char buf[MIL], *item, buf1[MIL], buf2[MIL];
  int cost, roll, extort_chance, i;
  int amount;

  item = one_argument( argument, buf );

  if ( IS_NULLSTR(buf)){
    send_to_char( "Buy what or how many of what item?\n\r", ch );
    return;
  }
  else if ( (amount = atoi(buf)) > 0){
    argument = item;
  }
  else
    amount = 1;

  /* get haggle roll */
  roll = number_percent();
  roll -= 2* ( get_curr_stat(ch,STAT_LUCK) - 14 );

  if (!IS_NULLSTR(mud_data.questor)
      && mud_data.questor[0] ==  ch->name[0]
      && !str_cmp( ch->name, mud_data.questor))
    roll -= 50;
  else if(is_song_affected(ch,gsn_reputation))
    roll -= 20;

  roll = UMAX(1, roll);

  /* PET SHOP */
  if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) ){
    pet_shop( ch, argument );
    return;
  }
  /* NORMAL ITEMS */
  if ( ( keeper = find_keeper( ch ) ) == NULL )
    return;
  else if (is_exiled(ch, keeper->in_room->area->vnum )){
    char buf[MIL];
    sprintf( buf, "No one in %s will deal with you %s!", keeper->in_room->area->name, PERS2(ch));
    do_say( keeper, buf );
    return;
  }
  /* CABAL CHECK */
  if ( keeper->pCabal && !_is_cabal( ch->pCabal, keeper->pCabal )){
    char buf[MIL];
    sprintf( buf, "You must be a member of %s`# to shop here %s.", keeper->pCabal->who_name, PERS2( ch ));
    do_say( keeper, buf );
    return;
  }

  if ( (obj  = get_obj_keeper( ch, keeper, argument )) == NULL){
    sendf(ch, "%s tells you '`2I don't sell that -- try 'list'.``'\n\r",PERS(keeper,ch));
    return;
  }
  /* count if we have enough objects in case of non-reset items*/
  if (!IS_SET( obj->extra_flags, ITEM_INVENTORY ) ){
    int counter = 0;
    for (tobj = keeper->carrying; tobj; tobj = tobj->next_content){
      if (tobj->pIndexData == obj->pIndexData)
	counter++;
    }
    amount = UMIN(amount, counter);
  }

  cost = amount * get_cost( keeper, obj, TRUE );

  if ( cost <= 0 || !can_see_obj( ch, obj ) ){
    sendf(ch, "%s tells you '`2I don't sell that -- try 'list'.``'\n\r",PERS(keeper,ch));
    return;
  }
  else if ( obj->level > ch->level ){
    act( "$n tells you '`2You can't use $p yet.``'", keeper, obj, ch, TO_VICT );
    return;
  }
  else if (ch->carry_number + amount * get_obj_number(obj) > can_carry_n(ch)){
    int num = can_carry_n(ch) - ch->carry_number;
    if (num == 0)
      send_to_char( "You can't carry any more items.\n\r", ch );
    else
      sendf(ch, "You can carry only %d more items.\n\r", num);
    return;
  }
  else if ( ch->carry_weight + amount * get_obj_weight_char(ch, obj) > can_carry_w(ch)){
    int num = (can_carry_w( ch ) - ch->carry_weight) / UMAX(1, get_obj_weight_char( ch, obj ));
    if (num == 0)
      send_to_char( "You lack the strength to hold more of these items.\n\r", ch );
    else
      sendf(ch, "You only have the strength to carry %d more.\n\r", num);
    return;
  }
  /* TEST FOR COST */
  else if ((obj->pIndexData->pCabal? GET_CP(ch) :  ch->gold) < cost ){
    sprintf( buf, "$n tells you '`2You can't afford %d %s%s to buy $p.``'",
	     cost,
	     obj->pIndexData->pCabal ? obj->pIndexData->pCabal->currency: "gold",
	     obj->pIndexData->pCabal ? "s": "");
    act( buf, keeper, obj, ch, TO_VICT );
    return;
  }
  /* Cant extort/haggle cabal items */
  extort_chance = get_skill(ch,gsn_extort);
  extort_chance = extort_chance / UMAX(3, cost / (ch->level * 8 * amount));

  if (!obj->pIndexData->pCabal && roll < extort_chance){
    cost = 0;
    act( "You intimidate $N into handing over $p.",ch,obj,keeper,TO_CHAR);
    act( "$n intimidates $N with $s threats.",ch,NULL,keeper,TO_ROOM);
    check_improve(ch,gsn_extort,TRUE,1);
    act( "$n extorts $p from $N.", ch, obj, keeper, TO_ROOM );
    act( "You extort $p from $N.", ch, obj, keeper, TO_CHAR );
  }
  else {
    if (!IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT)
	&& roll < get_skill(ch,gsn_haggle)
	&& !obj->pCabal){
      int sell_cost = amount * get_cost(keeper, obj, FALSE);
      int save = UMAX(0, roll * (cost - sell_cost ) / 100);

      cost -= save;
      sendf(ch,"You haggle the price down to %d coins.\n\r",cost);
      check_improve(ch,gsn_haggle,TRUE,1);
    }

    if (amount > 1){
      sprintf( buf, "%s", obj->short_descr );
      item = strchr( buf, ' ');

      if (item){
	*item++ = 0;
	if (str_cmp(buf, "a") && str_cmp(buf, "an") && str_cmp(buf, "the") )
	  item = buf;
      }
      else
	item = buf;
      sprintf( buf1, "You buy %d $ts.", amount );
      sprintf( buf2, "$n buys %d $ts.", amount );

    }
    else{
      item = obj->short_descr;
      sprintf( buf1, "You buy $t." );
      sprintf( buf2, "$n buys $t." );
    }
    act( buf1 , ch, item, NULL, TO_CHAR );
    act( buf2, ch, item, NULL, TO_ROOM );
  }
  if (obj->pIndexData->pCabal)
    CP_GAIN(ch, -cost, TRUE);
  else{
    ch->gold     -= cost;
    keeper->gold += cost;
  }
  for (i = 0; i < amount; i++){
    if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
      tobj = create_object( obj->pIndexData, obj->level);
    else if ( (tobj = get_obj_keeper( ch, keeper, obj->name )) != NULL)
      obj_from_char( tobj );
    else
      continue;

    if (tobj->timer > 0
	&& !IS_OBJ_STAT(tobj,ITEM_HAD_TIMER)
	&& tobj->item_type != ITEM_FOOD)
      tobj->timer = 0;
    REMOVE_BIT(tobj->extra_flags,ITEM_HAD_TIMER);
    obj_to_char( tobj, ch );
    if (cost / amount < tobj->cost)
      tobj->cost = cost / amount;
  }
}



void do_browse( CHAR_DATA *ch, char *argument ){
  CHAR_DATA *keeper;
  OBJ_DATA *obj;
  char buf[MIL];

  if ( argument[0] == '\0' ){
    send_to_char( "Browse what?\n\r", ch );
    return;
  }

  /* PET SHOP */
  if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) ){
    send_to_char("There is nothing to gain by browsing pets.\n\r", ch);
    return;
  }
  if ( ( keeper = find_keeper( ch ) ) == NULL )
    return;
  /* NORMAL ITEMS */
  if ( keeper->pCabal && !_is_cabal( ch->pCabal, keeper->pCabal )){
    char buf[MIL];
    sprintf( buf, "You must be a member of %s`# to shop here %s.", keeper->pCabal->who_name, PERS2( ch ));
    do_say( keeper, buf );
    return;
  }

  /* get the object we are talking about*/
  obj  = get_obj_keeper( ch, keeper, argument );

  if (!obj || !can_see_obj( ch, obj ) ){
    sendf(ch, "%s tells you '`2I don't sell that -- try 'list'.``'\n\r",PERS(keeper,ch));
    ch->reply = keeper;
    return;
  }

  obj = create_object( obj->pIndexData, obj->level);
  obj_to_ch( obj, ch );
  strcpy (buf, obj->name);

  /* check which skill to use */
    if (get_skill(ch, gsn_identify) > 1){
      send_to_char("You try to identify it..\n\r", ch);
      spell_identify(gsn_identify, ch->level, ch, obj, 0);
    }
    else if (get_skill(ch, skill_lookup("lore")) > 1){
      do_lore(ch, buf);
    }
    else{
      do_examine(ch, buf);
    }

    if (obj){
      obj_from_char( obj );
      extract_obj( obj );
    }

}

void do_list( CHAR_DATA *ch, char *argument )
{
    /* used for sorting */
    const int MAX_ITEM = 256;
    char list[MAX_ITEM] [MIL];
    int key[MAX_ITEM];
    int end_key[MAX_ITEM];
    int last = 0;
    int i, j;

    /* SHOP */
    if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
    {
	ROOM_INDEX_DATA *pRoomIndexNext;
	CHAR_DATA *pet;
	bool found = FALSE;
        if (ch->in_room->vnum == 9621)
            pRoomIndexNext = get_room_index(9706);
        else
            pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
	if ( pRoomIndexNext == NULL )
	{
	    bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}
	for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room ){
	  if ( IS_SET(pet->act, ACT_PET) ){
	    if ( !found ){
	      found = TRUE;
	      send_to_char( "Pets for sale:\n\r", ch );
	    }

	    sprintf( list[last], "[%2d] %8d - %s\n\r",
		     pet->level, 10 * pet->level * pet->level,
		     pet->short_descr );
	    key[last++] = 10 * pet->level * pet->level;
	    if (last >= MAX_ITEM)
	      last = MAX_ITEM - 1;
	  }
	}
	if ( !found )
	  send_to_char( "Sorry, we're out of pets right now.\n\r", ch );
    }
    else{
      CHAR_DATA *keeper;
      OBJ_DATA *obj;
      int cost, count;
      bool found = FALSE;
      char arg[MIL];
      if ( ( keeper = find_keeper( ch ) ) == NULL )
	return;

      if ( keeper->pCabal && !_is_cabal( ch->pCabal, keeper->pCabal )){
	char buf[MIL];
	sprintf( buf, "You must be a member of %s`# to shop here %s.", keeper->pCabal->who_name, PERS2( ch ));
	do_say( keeper, buf );
	return;
      }

      one_argument(argument,arg);
      for ( obj = keeper->carrying; obj; obj = obj->next_content ){
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj )
	     && ( cost = get_cost( keeper, obj, TRUE ) ) > 0 && ( arg[0] == '\0' || is_name(arg,obj->name) )){
	  if ( !found ){
	    found = TRUE;
	    sendf( ch, "[Lv Price Coin Qty] Item\n\r");
	  }
	  if (IS_OBJ_STAT(obj,ITEM_INVENTORY)){
	    sprintf(list[last], "[%2d %5d %s  -- ] %s\n\r",
		    obj->level,
		    cost,
		    (obj->pIndexData->pCabal? " CP" : " GO"),
		    obj->short_descr);
	    key[last++] = cost;
	    if (last >= MAX_ITEM)
	      last = MAX_ITEM - 1;
	  }
	  else{
	    count = 1;
	    while (obj->next_content != NULL && obj->pIndexData == obj->next_content->pIndexData
		   && !str_cmp(obj->short_descr, obj->next_content->short_descr))
	      {
		obj = obj->next_content;
		count++;
	      }
	    sprintf(list[last], "[%2d %5d %s  %2d ] %s\n\r",
		    obj->level,
		    cost,
		    (obj->pIndexData->pCabal? " Cp" : " Go"),
		    count,
		    obj->short_descr);
	    key[last++] = cost;
	    if (last >= MAX_ITEM)
	      last = MAX_ITEM - 1;
	  }
	}
      }
      if ( !found )
	send_to_char( "You can't buy anything here.\n\r", ch );
    }
/* prepare endkey for sorting */
    for (i = 0; i < last; i++)
      end_key[i] = i;
/* sort the list with good ole bubble sort */
    for (i = 0; i < last; i ++){
      for (j = 0; j < last - 1; j++){
	if (key[j] < key[j + 1]){
	  int buf = key[j];
	  key[j] = key[j + 1];
	  key [j + 1] = buf;
	  buf = end_key[j];
	  end_key[j] = end_key[j + 1];
	  end_key[j + 1] = buf;
	}
      }
    }
/* show the list */
    for(i = 0; i < last; i ++){
      send_to_char(list[end_key[i]], ch);
    }


}

void do_value( CHAR_DATA *ch, char *argument )
{
    char buf[MSL], arg[MIL];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Value what?\n\r", ch );
	return;
    }
    if ( ( keeper = find_keeper( ch ) ) == NULL )
	return;
    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        sendf(ch, "%s tells you '`2You don't have that item.``'\n\r",PERS(keeper,ch));
	ch->reply = keeper;
	return;
    }
    if (!can_see_obj(keeper,obj))
    {
        sendf(ch, "%s doesn't see what you are offering.\n\r",PERS(keeper,ch));
        return;
    }
    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }
    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
	act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
	return;
    }
    sprintf( buf, "$n tells you '`2I'll give you %d gold coins for $p.``'", cost);
    act( buf, keeper, obj, ch, TO_VICT );
}


void secondary(CHAR_DATA* ch, OBJ_DATA* obj){
  OBJ_DATA* wield = get_eq_char(ch,WEAR_WIELD);
  int sn, skill;
  if (obj == NULL)
    {
      send_to_char("You do not have that weapon.\n\r",ch);
      return;
    }
  /* race check */
  if (!IS_NPC(ch)
      && obj->race && obj->race != ch->race){
    send_to_char("Its size and style make it useless to you.\n\r", ch);
    return;
  }
  /* class */
  if (!IS_NPC(ch)
      && obj->class >= 0 && obj->class != ch->class){
    send_to_char("Its size and style make it useless to you.\n\r", ch);
    return;
  }
  if (!check_sheath(ch, WEAR_SECONDARY))
    return ;
  if ((get_eq_char(ch,WEAR_SHIELD) != NULL
       && !remove_obj(ch, WEAR_SHIELD, TRUE))
      || (get_eq_char(ch,WEAR_HOLD) != NULL
	  && !remove_obj(ch, WEAR_HOLD, TRUE)) )
    {
      send_to_char("You cannot use a secondary weapon while using a shield or holding an item.\n\r",ch);
      return;
    }
  if (wield == NULL)
    {
      send_to_char("You need to wield a primary weapon, before using a secondary one.\n\r",ch);
      return;
    }
  if (!check_secondary(ch, obj, wield)){
    return;
  }
  if (!remove_obj(ch,WEAR_SECONDARY,TRUE))
    return;
  if ( CAN_WEAR( obj, ITEM_WIELD ) ){
      act("$n wields $p in $s off-hand.",ch,obj,NULL,TO_ROOM);
      act("You wield $p in your off-hand.",ch,obj,NULL,TO_CHAR);
      equip_char(ch,obj,WEAR_SECONDARY);
      sn = get_weapon_sn(ch, TRUE);
      if (sn == gsn_hand_to_hand)
           return;
      skill = get_skill(ch,sn);

      /* Useless conditional */
      if( skill != 0 )
        skill *= 1;
/*
  if    (skill >= 100) act("$p feels like a part of you!",ch,obj,NULL,TO_CHAR);
  else if (skill > 85) act("You feel quite confident with $p.",ch,obj,NULL,TO_CHAR);
  else if (skill > 70) act("You are skilled with $p.",ch,obj,NULL,TO_CHAR);
  else if (skill > 50) act("Your skill with $p is adequate.",ch,obj,NULL,TO_CHAR);
  else if (skill > 25) act("$p feels a little clumsy in your hands.",ch,obj,NULL,TO_CHAR);
  else if (skill > 1)  act("You fumble and almost drop $p.",ch,obj,NULL,TO_CHAR);
  else                 act("You don't even know which end is up on $p.",ch,obj,NULL,TO_CHAR);
*/
  }
  else{
    act("$p cannot be wielded.", ch, obj, NULL, TO_CHAR);
  }
}

void do_second( CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *obj;

  if (is_affected(ch,gsn_bat_form)
      || is_affected(ch,gsn_wolf_form)
      || is_affected(ch,gsn_mist_form)
      || is_affected(ch,gsn_weretiger)
      || is_affected(ch,gsn_werewolf)
      || is_affected(ch,gsn_werebear)
      || is_affected(ch,gsn_werefalcon)){
    send_to_char("Not while you're morphed.\n\r",ch);
    return;
  }
  if (get_skill(ch,gsn_dual_wield)  <= 1 && get_skill(ch, gsn_bladework) < 2){
    send_to_char("You do not possess enough fighting prowess to dual wield.\n\r",ch);
    return;
  }
  if (argument[0] == '\0')
    {
      send_to_char( "Wear which weapon in your off-hand?\n\r",ch);
      return;
    }
  obj = get_obj_carry(ch,argument, ch);
  secondary(ch, obj );
}

void do_outfit ( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    bool found = FALSE;
    int iWear;

// Jindrak - disabled this - not check to see if objects exist.
    return;

    if (ch->level > 5 || IS_NPC(ch))
    {
	send_to_char("Find it yourself!\n\r",ch);
	return;
    }
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
	if ( ( obj = get_eq_char( ch, iWear ) ) != NULL && iWear != WEAR_TATTOO)
	    found = TRUE;
    if (found)
    {
	send_to_char("You must be totally naked to be outfitted.\n\r",ch);
	return;
    }
    if ( ch->carry_number + 4 > can_carry_n( ch ) )
    {
	send_to_char("Your hands are too full.\n\r",ch);
	return;
    }
    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) == NULL )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0);
	obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_LIGHT );
    }
    if ( ( obj = get_eq_char( ch, WEAR_BODY ) ) == NULL )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0);
	obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_BODY );
    }
    if (ch->pcdata->start_wep >= 0
	&& (obj = get_eq_char(ch,WEAR_WIELD)) == NULL){
      obj = create_object( get_obj_index(weapon_table[ch->pcdata->start_wep].vnum), 0);
     	obj_to_char(obj,ch);
    	equip_char(ch,obj,WEAR_WIELD);
    }
    if (((obj = get_eq_char(ch,WEAR_WIELD)) == NULL
	 || !IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
    && (obj = get_eq_char( ch, WEAR_SHIELD ) ) == NULL )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0);
	obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_SHIELD );
    }
    obj = create_object( get_obj_index(OBJ_VNUM_VALMIRAN_MAP), 0);
    obj_to_char( obj, ch );
    obj = create_object( get_obj_index(OBJ_VNUM_RHEYDIN_MAP), 0);
    obj_to_char( obj, ch );
    obj = create_object( get_obj_index(OBJ_VNUM_E_THERA_MAP), 0);
    obj_to_char( obj, ch );
    obj = create_object( get_obj_index(OBJ_VNUM_W_THERA_MAP), 0);
    obj_to_char( obj, ch );
    if (ch->class == class_lookup("bard"))
    {
    	obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_RECORDER), 0);
    	obj_to_char( obj, ch );
    }
    send_to_char("You have been equipped by the Gods.\n\r",ch);
}

void defecate( CHAR_DATA *ch )
{
    char buf[MSL];
    char *name;
    OBJ_DATA *obj;
    if ( IS_NPC(ch) )
	name = str_dup(ch->short_descr);
    else
	name = str_dup(ch->name);
    obj = create_object( get_obj_index(OBJ_VNUM_FECES), ch->level);
    obj->level = ch->level;
    sprintf(buf, "%s %s",obj->name,name);
    free_string( obj->name );
    obj->name = str_dup(buf);
    obj->timer = 3;
    sprintf(buf, "A swarm of flies circle around a pile of %s's dung.",name);
    free_string( obj->description );
    obj->description = str_dup(buf);
    sprintf(buf, "a pile of %s's dung",name);
    free_string( obj->short_descr );
    obj->short_descr = str_dup(buf);
    obj_to_room(obj,ch->in_room);
}

void do_defecate( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    if (ch->in_room == NULL)
	return;
    if (IS_NPC(ch))
    {
        send_to_char("You squat down and drop your load.\n\r",ch);
        act("$n squats down and drops $s load.",ch,NULL,NULL,TO_ROOM);
        defecate(ch);
	return;
    }
    if (is_affected(ch,gsn_defecate))
    {
	send_to_char("You don't feel the urge to relieve yourself yet.\n\r",ch);
	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("Not while you are fighting!\n\r",ch);
	return;
    }
    if (ch->pcdata->condition[COND_HUNGER] < 15)
    {
	send_to_char("You are not full enough to take a dump.\n\r",ch);
	return;
    }
    send_to_char("You squat down and drop your load.\n\r",ch);
    act("$n squats down and drops $s load.",ch,NULL,NULL,TO_ROOM);
    WAIT_STATE(ch,12);
    ch->position = POS_SITTING;
    update_pos(ch);
    defecate(ch);
    if (!IS_IMMORTAL(ch))
    {
    	ch->pcdata->condition[COND_HUNGER] -= 10;
    	af.where     = TO_AFFECTS;
    	af.type      = gsn_defecate;
    	af.level     = ch->level;
    	af.duration  = 24;
    	af.location  = 0;
    	af.modifier  = 0;
    	af.bitvector = 0;
    	affect_to_char( ch, &af );
    }
}

void vomit( CHAR_DATA *ch )
{
    char buf[MSL];
    char *name;
    OBJ_DATA *obj;
    if ( IS_NPC(ch) )
        name = str_dup(ch->short_descr);
    else
        name = str_dup(ch->name);
    obj = create_object( get_obj_index(OBJ_VNUM_VOMIT), ch->level);
    obj->level = ch->level;
    sprintf(buf, "%s %s",obj->name,name);
    free_string( obj->name );
    obj->name = str_dup(buf);
    obj->timer = 3;
    sprintf(buf, "A foul, acrid stench surrounds a pile of %s's vomit.",name);
    free_string( obj->description );
    obj->description = str_dup(buf);
    sprintf(buf, "a reeking pile of %s's vomit",name);
    free_string( obj->short_descr );
        obj->short_descr = str_dup(buf);
    obj_to_room(obj,ch->in_room);
}

void do_vomit( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int level= 2 * ch->level / 3;

    if (level < 0) {
      level = 0;
    }

    if (ch->in_room == NULL)
    {
        return;
    }
    if (IS_NPC(ch))
    {
        send_to_char("You stick your finger down your throat and spew vomit everywhere!\n\r",ch);
        act("$n sticks $s finger down $n throat and vomits.",ch,NULL,NULL,TO_ROOM);
        vomit(ch);
        return;
    }
    if (is_affected(ch,gsn_vomit))
    {
        send_to_char("You don't feel much like vomiting again.\n\r",ch);
        return;
    }
    if (ch->fighting != NULL)
    {
        send_to_char("Not while you are fighting!\n\r",ch);
        return;
    }
    if (ch->pcdata->condition[COND_HUNGER] < 15)
    {
        send_to_char("There is nothing in your stomach to vomit out.\n\r",ch);
        return;
    }
    send_to_char("You drop to your knees and vomit violently!\n\r",ch);
    act("$n drops to $s knees and vomits.",ch,NULL,NULL,TO_ROOM);
    WAIT_STATE(ch,12);
    ch->position = POS_SITTING;
    update_pos(ch);
    vomit(ch);

    if (check_dispel(level,ch,gsn_poison))
    {
        act_new("You manage to vomit out the poison.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
        act("$n looks much better.",ch,NULL,NULL,TO_ROOM);
    }

    if (!IS_IMMORTAL(ch))
    {
        ch->pcdata->condition[COND_HUNGER] -= 10;
        af.where     = TO_AFFECTS;
        af.type      = gsn_vomit;
        af.level     = ch->level;
        af.duration  = 24;
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char( ch, &af );
    }
}

void do_drain( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj;
    one_argument( argument, arg );
    if (ch->class != class_lookup("vampire"))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if ( arg[0] == '\0' )
    {
        send_to_char( "Drain what?\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_here( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    if ( ( obj->item_type != ITEM_CORPSE_PC && obj->item_type != ITEM_CORPSE_NPC)
	 || obj->condition < 2)
    {
	send_to_char( "There is no blood in that.\n\r",ch);
	return;
    }
    if (obj->item_type == ITEM_CORPSE_PC && obj->level < 15 && !IS_IMMORTAL(ch))
    {
        send_to_char("Not that corpse.\n\r",ch);
        return;
    }
    if (obj->pIndexData->vnum == OBJ_VNUM_FAKE_CORPSE)
    {
        send_to_char("Not that corpse.\n\r",ch);
        return;
    }
    if (is_affected(ch, gsn_gag)){
      send_to_char("The gag stops you from feeding.\n\r", ch);
      return;
    }
    act("You drain the blood from $p.",ch,obj,NULL,TO_CHAR);
    act("$n drains the blood from $p.",ch,obj,NULL,TO_ROOM);
    if ( !IS_NPC(ch) )
    {
        int condition;
        condition = ch->pcdata->condition[COND_HUNGER];
        gain_condition( ch, COND_HUNGER,(30*obj->level)/ch->level);
        if ( condition <= 0 && ch->pcdata->condition[COND_HUNGER] > 0 )
            send_to_char( "You are no longer hungry.\n\r", ch );
        else if ( ch->pcdata->condition[COND_HUNGER] > 40 )
             send_to_char( "You are full.\n\r", ch );
    }
    act("You feel a little better.", ch,NULL,NULL,TO_CHAR);
    ch->hit = UMIN( ch->hit + number_range(obj->level, 3*obj->level), ch->max_hit );
    update_pos( ch );
    free_string(obj->description);
    obj->description = str_dup("An eviscerated corpse lies here.");
    obj->condition = 1;
}

/* function does the drawing of weapon */
bool draw(CHAR_DATA* ch, bool fLeft){
  OBJ_DATA* obj;
  int pos = fLeft ? WEAR_SHEATH_L : WEAR_SHEATH_R;

  if (ch == NULL)
    return FALSE;

  if ( (obj = get_eq_char(ch, pos)) == NULL){
    return FALSE;
  }
  unequip_char(ch, obj);
  if (!fLeft)
    secondary(ch, obj);
  else
    wear_obj(ch, obj, TRUE, FALSE);
  return TRUE;
}
/* function that does the sheathing */
bool sheath(CHAR_DATA* ch, OBJ_DATA* obj, bool fLeft){
  int pos = fLeft ? WEAR_SHEATH_L : WEAR_SHEATH_R;

  if (ch == NULL || obj == NULL)
    return FALSE;

  if (get_eq_char(ch, pos) != NULL){
    bug("sheath: tried sheathing into a full sheath.", 0);
    return FALSE;
  }
  if (obj->wear_loc != WEAR_NONE)
    unequip_char(ch, obj);
  equip_char(ch, obj, pos);
  return TRUE;
}

/* does the simple case of drawing weapons, used again
in fight.c do draw weapon when needed.
returns TRUE if change of weapons took place
*/
bool do_combatdraw(CHAR_DATA* ch, OBJ_DATA* prim, OBJ_DATA* sec){
  char buf[MIL];
  OBJ_DATA* left = NULL;
  OBJ_DATA* right = NULL;

  bool fPrim = prim == NULL;
  bool fSec = sec == NULL;
  int count = 0;
  int slots = 0;
/* check for left sheath holding something */
  if (get_eq_char(ch, WEAR_SHIELD) != NULL)
    slots++;
  if (get_eq_char(ch, WEAR_HOLD) != NULL)
    slots++;

  /* two handed check */
  if ( prim){
    if ( (ch->size < SIZE_LARGE && IS_WEAPON_STAT(prim,WEAPON_TWO_HANDS))
	 || prim->value[0] == WEAPON_STAFF
	 || prim->value[0] == WEAPON_POLEARM
	 || is_affected(ch,gsn_double_grip)){
      fSec = FALSE;
    }
  }


  left = get_eq_char(ch, WEAR_SHEATH_L);
  /* right needs check for shield etc */
  if ( fSec && (right = get_eq_char(ch, WEAR_SHEATH_R)) != NULL){
    if (slots > 0)
      fSec = FALSE;
  }
  if ( fPrim && (left = get_eq_char(ch, WEAR_SHEATH_L)) != NULL){
    if (slots > 1)
      fPrim = FALSE;
  }
  /* count total for message */
  count = (fPrim && left) + (fSec && right);

  if (count){
    sprintf(buf, "weapon%s", count > 1 ? "s" : "");
    act("You draw your $t.", ch, buf, NULL, TO_CHAR);
    if (!IS_AFFECTED(ch, AFF_SNEAK))
      act("$n draws $s $t.", ch, buf, NULL, TO_ROOM);
    if (fPrim && left)
      draw(ch, TRUE);
    if (fSec && right)
      draw(ch, FALSE);
    return TRUE;
  }
  return FALSE;
}

/* draws the weapons in sheaths */
void do_draw( CHAR_DATA *ch, char *argument ){
  OBJ_DATA* left = get_eq_char(ch, WEAR_SHEATH_L);
  OBJ_DATA* right = get_eq_char(ch, WEAR_SHEATH_R);

  int count = 0;

  bool fPrim = FALSE;
  bool fSec = FALSE;

  const int sn = gsn_sheath;
  const int pos = sheath_lookup(class_table[ch->class].name);
  bool fAdv = get_skill(ch, sn);

  if (!left && !right){
    send_to_char("You have no sheathed weapons to draw!\n\r", ch);
    return;
  }
  /* normal case */
  if (!fAdv){
    char buf[MIL];
    /* check which spots are empty */
    if (get_eq_char(ch, WEAR_WIELD) == NULL
	&& (get_eq_char(ch, WEAR_SHIELD) == NULL
	    || get_eq_char(ch, WEAR_HOLD) == NULL))
      fPrim = TRUE;
    if (get_eq_char(ch, WEAR_SECONDARY) == NULL
	&& get_eq_char(ch, WEAR_SHIELD) == NULL
	&& get_eq_char(ch, WEAR_HOLD) == NULL)
      fSec = TRUE;
    /* count total for message */
    count = (fPrim && left) + (fSec && right);
    if (count){
      sprintf(buf, "weapon%s", count > 1 ? "s" : "");
      act("You draw your $t.", ch, buf, NULL, TO_CHAR);
      if (!IS_AFFECTED(ch, AFF_SNEAK))
	act("$n draws $s $t.", ch, buf, NULL, TO_ROOM);
    }
    if (fPrim && left)
      draw(ch, TRUE);
    if (fSec && right)
      draw(ch, FALSE);
    return;
  }
  else{
    OBJ_DATA* prim = NULL, *sec = NULL;
    char buf[MIL];

/* for advanced case we simply check if we have weapons and attempt to */
/* rotate */
    /* count total for message */
    count = (left != NULL) + (right !=NULL  && sheath_table[pos].both);
    if (count){
      sprintf(buf, "weapon%s", count > 1 ? "s" : "");
      act("You draw your $t.", ch, buf, NULL, TO_CHAR);
      if (!IS_AFFECTED(ch, AFF_SNEAK))
	act("$n draws $s $t.", ch, buf, NULL, TO_ROOM);
    }
    /* remove both primary weapons */
    if ( (prim = get_eq_char(ch, WEAR_WIELD)) != NULL){
      unequip_char( ch, prim);
    }
    /* remove secondary weapons */
    if ( (sec = get_eq_char(ch, WEAR_SECONDARY)) != NULL
	 || (sec = get_eq_char(ch, WEAR_SHIELD)) != NULL
	 || (sec = get_eq_char(ch, WEAR_HOLD)) != NULL){
      unequip_char( ch, sec);
    }

    /* both prim/sec slots are now empty if possible, put sheathed weapons there */
    if (get_eq_char(ch, WEAR_WIELD) == NULL){
      draw(ch, TRUE);
    }
    if (get_eq_char(ch, WEAR_SECONDARY) == NULL){
      draw(ch, FALSE);
    }

    /* put original weapons in sheaths if possible */
    if (prim && get_eq_char(ch, WEAR_SHEATH_L) == NULL
	&& can_doublesheath(ch, prim, FALSE)){
      sheath(ch, prim, TRUE);
    }
    if (sec
	&& sec->item_type == ITEM_WEAPON
	&& get_eq_char(ch, WEAR_SHEATH_R) == NULL
	&& can_doublesheath(ch, sec, FALSE)){
      sheath(ch, sec, FALSE);
    }
    return;
  }
}


/* sheaths your weapons, or specific weapon */
/* anyone can do nullstring part, only people
skilled in sheath can select items */
void do_sheath( CHAR_DATA *ch, char *argument ){
  OBJ_DATA* obj;
  int count = 0;
  int w_loc = 0;

  const int sn = gsn_sheath;
  const int pos = sheath_lookup(class_table[ch->class].name);
  int fAdv = get_skill(ch, sn);

  /* check for things that might prevent this */
  if (is_affected(ch, gsn_graft_weapon)
      || is_affected(ch, gsn_holy_hands)
      || is_affected(ch, gsn_body_weaponry)){
    return;
  }
  /* two modes, normal, and advanced */
  if (IS_NULLSTR(argument) || !fAdv){
    char buf[MIL];
    obj = get_eq_char(ch, WEAR_WIELD);
    if (sheath(ch, obj, TRUE))
      count ++;
    obj = get_eq_char(ch, WEAR_SECONDARY);
    if (sheath(ch, obj, FALSE))
      count ++;
    if (count){
      sprintf(buf, "weapon%s", count > 1 ? "s" : "");
      act("You sheath your $t.", ch, buf, NULL, TO_CHAR);
      if (!IS_AFFECTED(ch, AFF_SNEAK))
	act("$n sheathes $s $t.", ch, buf, NULL, TO_ROOM);
    }
    return;
  }/* END NORMAL*/
  else{
    char arg1[MIL];
    bool fDouble = FALSE;
    argument = one_argument(argument, arg1);

    /* check if this is double sheath or just sheathing */
    w_loc = IS_NULLSTR(argument) ? WEAR_SHEATH_L : WEAR_SHEATH_R;
    if (w_loc == WEAR_SHEATH_L && get_eq_char(ch, WEAR_WIELD) != NULL){
      fDouble = TRUE;
    }
    else if (w_loc == WEAR_SHEATH_R
	     && (get_eq_char(ch, WEAR_WIELD) != NULL
		 || get_eq_char(ch, WEAR_SHIELD) != NULL
		 || get_eq_char(ch, WEAR_SECONDARY) != NULL)){
      fDouble = TRUE;
    }
    /* check for secondary */
    if (fDouble && w_loc == WEAR_SHEATH_R && !sheath_table[pos].both){
      send_to_char("You are incapable of double sheathing secondary weapons.\n\r", ch);
      return;
    }

    if ( (obj = get_obj_carry(ch, arg1, ch)) == NULL){
      sendf(ch, "You do not seem to be carrying %s.\n\r", arg1);
      return;
    }
    if (obj->item_type != ITEM_WEAPON){
      send_to_char("You can only sheath a weapon.\n\r", ch);
      return;
    }

    if (fDouble && !can_doublesheath(ch, obj, FALSE))
      return;
    /* check for empty sheaths */
    if (get_eq_char(ch, w_loc) != NULL){
      send_to_char("You are already storing a weapon there!\n\r", ch);
      return;
    }
    /* skill check */
    if (fDouble && number_percent() > 4 * fAdv / 5){
      send_to_char("Your weapon gets in the way and you fail.\n\r", ch);
      return;
    }
    /* sheath it */
    if (sheath(ch, obj, w_loc == WEAR_SHEATH_L)){
      act("You $tsheath your weapon.",
	  ch, fDouble ? "double" : "", NULL, TO_CHAR);
      act("$n $tsheathes $s weapon.",
	  ch, fDouble ? "double" : "", NULL, TO_ROOM);
    }
  }
}

//05-01 10:15 GO: commented out fasting
//11-25 15:30 ATH: Added code to deal with "holdsrare" flag on containers
//01-08 15:30 ATH: Added cant put non-rare holdsrare items in sacs
