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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "raid.h"
#include "cabal.h"

/***************************************************************/
/*Following are city raid orientanted routines for Forsaken Lands  */
/*mud created by Virigoth circa Feb 2004.  Copyrighted for Forsaken*/
/*Lands mud Feb 4, 2004.  Do not use or copy without explicit */
/*permission of author. (Voytek Plawny aka Virigoth)	       */
/***************************************************************/

//table which holds special raider strings etc.
const struct raid_strings raid_table [RAID_MAX] = {
  { ACT_UNDEAD, 
    "undead",
    "bite",
    "rotten zombie",
    "a rotten zombie",
    "Covered in clotted veins and wrapped in stench a zombie stands here.\n\r"
  },
  { ACT_UNDEAD, 
    "undead",
    "frbite",
    "wraith",
    "a wraith",
    "Its eyes blazing with hate a wraith floats before you.\n\r"
  },
  { ACT_UNDEAD, 
    "undead",
    "tainted",
    "fallen ruler",
    "a fallen ruler",
    "Dressed in rotten regal clothing a fallen ruler awaits its victim.\n\r"
  },
  { ACT_UNDEAD, 
    "undead",
    "skdrain",
    "vampiric lord",
    "Vampiric blood lord",
    "Draped in fresh skin and blood a vampiric blood lord floats nearby.\n\r"
  },

  { 0,
    "demon",
    "bite",
    "gremlin",
    "a gremlin",
    "Scrambing wildly on all fours a gremlin circles the area.\n\r"
  },
  { ACT_MAGE,
    "demon",
    "magic",
    "demonic cultist",
    "a demonic cultist",
    "With a bloody dagger impaled upon his arm a demonic cultist lurks.\n\r"
  },
  { 0,
    "demon",
    "tainted",
    "demon",
    "a demon",
    "Covered in drool a mutated demon searches for a fresh victim.\n\r"
  },
  { 0,
    "demon",
    "blight",
    "tryst lord",
    "tryst Lord",
    "A mutilated torso in its claws a Tryst Lord feeds before you.\n\r"
  },

  { 0,
    "treant",
    "crush",
    "wild treant",
    "a wild treant",
    "A wild treant moans with ancient anguish.\n\r"
  },
  { 0,
    "lizard",
    "slash",
    "slith warrior",
    "a slith warrior",
    "A slith warrior stands before you, scales glistening with blood and muck.\n\r"
  },
  { ACT_MAGE,
    "faerie",
    "magic",
    "dryad",
    "a dryad",
    "A scantly clad dryad lingers here, muttering prayers to Gaia.\n\r"
  },
  { 0,
    "unique",
    "blast",
    "elemental",
    "an elemental",
    "A spirit of earthly power emits an aura of rage and anger. \n\r"
  },
};

/* PROTOTYPES */

/* PRIVATE */

RAID_DATA* new_raid(){
  RAID_DATA* pr = alloc_mem( sizeof( *pr ));

  return pr;
}

void free_raid( RAID_DATA* pr ){
  free_mem( pr, sizeof( *pr ));
}

//creates a raid in a given area
//if specRaid is >= 0 data from raid table are used.
bool create_raid( AREA_DATA* pArea, int vnum, int tot, int life, int specRaid, CABAL_DATA* pCab){
  RAID_DATA* pr;
  MOB_INDEX_DATA* pIndex;

  if (pArea == NULL || vnum < 1 || tot < 1 || life < 1)
    return FALSE;
  else if ( (pIndex = get_mob_index( vnum )) == NULL){
    bug("raid.c>create_raid: Could not load mob index %d", vnum );
    return FALSE;
  }
  
  if (specRaid >= 0 && specRaid >= RAID_MAX){
    bug("raid.c>create_raid: specRaid (%d) over max.", specRaid);
    specRaid = -1;
  }

  pr = new_raid();
  memset( pr, 0, sizeof( *pr ));

  pr->pCab = pCab;
  pr->mob_vnum = vnum;
  pr->mob_total = tot;
  pr->lifetime = life;

  pr->mob_spawned = 0;
  pr->duration = 0;

  if (specRaid >= 0){
    pr->race= race_lookup(raid_table[specRaid].race);
    if (pr->race < 0)
      pr->race = race_lookup("human");

    pr->attack= attack_lookup(raid_table[specRaid].attack);
    if (pr->attack < 0)
      pr->attack = attack_lookup("slash");

    pr->act = raid_table[specRaid].act;

    pr->name = raid_table[specRaid].name;
    pr->long_desc = raid_table[specRaid].long_desc;
    pr->short_desc = raid_table[specRaid].short_desc;
  }
  else{
    pr->race = pIndex->race;
    pr->act = 0;
    pr->name = pIndex->player_name;
    pr->long_desc = pIndex->long_descr;
    pr->short_desc = pIndex->short_descr;
  }

  if (pArea->raid != NULL)
    free_raid(pArea->raid);
  pArea->raid = pr;

  //set stamp on cabal for raids
  pCab->raid_stamp = mud_data.current_time;
  //set stamp on area
  pArea->last_raid = mud_data.current_time;
  return TRUE;
}

// spawns a single raider and sets his path towards target area
bool raid_spawn( AREA_DATA* pArea, RAID_DATA* pr ){
  const int MAX_AREAS = 16;
  AREA_DATA* areas[MAX_AREAS];
  AREA_DATA* pTar, *pOri;
  ROOM_INDEX_DATA* pRTar, *pROri;
  CHAR_DATA* pMob;
  EXIT_DATA* pExit;
  PATH_QUEUE* path;

//  int sectors[2] = {SECT_CITY, SECT_INSIDE};
  int max_area = 0;
  int area_pool = 10;
  int dist = 0;

  if (pArea == NULL || pr == NULL)
    return FALSE;
  else
    pTar = pArea;

  
  /* first get the list of areas that connect to target area */
  for (pExit = pTar->exits; pExit; pExit = pExit->next_in_area ){
    //skip on blind exit, or exit to same area
    if (pExit->to_room == NULL || pExit->to_room->area == pTar)
      continue;
    //skip on cabal home areas
    else if (pExit->to_room->area->pCabal 
	     && pExit->to_room->area->pCabal->anchor->area == pExit->to_room->area)
      continue;
    //skip jails, restricted areas etc.
    else if (IS_AREA(pExit->to_room->area, AREA_MUDSCHOOL)
	     || IS_AREA(pExit->to_room->area, AREA_NOPATH)
	     || IS_AREA(pExit->to_room->area, AREA_RESTRICTED))
      continue;
    areas[max_area++] = pExit->to_room->area;
  }
//if no exists, then origin is target
  if (max_area < 1)
    pOri = pTar;
  else
    pOri = areas[number_range(0, max_area - 1)];

  //we now have origin and target area.  Try for random rooms in each.
  pROri =  get_rand_room(pOri->vnum,pOri->vnum,	//area range (0 to ignore)
			 0,0,			//room ramge (0 to ignore)
			 0,0,			//areas to choose from
			 NULL,0,		//areas to exclude
			 NULL,0,		//sectors required
			 NULL,0,		//sectors to exlude
			 NULL,0,		//room1 flags required
			 0,0,			//room1 flags to exclude
			 NULL,0,		//room2 flags required
			 NULL,0,		//room2 flags to exclude
			 area_pool,		//number of seed areas
			 TRUE,			//exit required?
			 FALSE,			//Safe?
			 NULL);			//Character for room checks
  
  if (pROri == NULL)
    return FALSE;

  pRTar =  get_rand_room(pTar->vnum,pTar->vnum,	//area range (0 to ignore)
			 0,0,			//room ramge (0 to ignore)
			 0,0,			//areas to choose from
			 NULL,0,		//areas to exclude
			 NULL,0,		//sectors required
			 NULL,0,		//sectors to exlude
			 NULL,0,		//room1 flags required
			 0,0,			//room1 flags to exclude
			 NULL,0,		//room2 flags required
			 NULL,0,		//room2 flags to exclude
			 area_pool,		//number of seed areas
			 TRUE,			//exit required?
			 FALSE,			//Safe?
			 NULL);			//Character for room checks
  if (pRTar == NULL || pRTar == pROri)
    return FALSE;

  //see if we can get a path between the two rooms
  if ( (path = generate_path( pROri, pRTar, 256, TRUE, &dist, NULL)) == NULL){
    clean_path();
    return FALSE;
  }
  else
    clean_path();
  
  //time to load the mob
  if ( (pMob = create_mobile( get_mob_index( pr->mob_vnum))) == NULL){
    clean_path_queue( path );
    return FALSE;
  }
  else{
    AFFECT_DATA af;

    af.type = gsn_timer;
    af.level= pMob->level;
    af.duration = 3 * pr->lifetime / 2;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier =  0;
    affect_to_char(pMob, &af);
  } 

  //set the path
  pMob->spec_path = path;
  //set cabal
  pMob->pCabal = pr->pCab;
  //set attack
  pMob->dam_type = pr->attack;

  //set the raid string
  pMob->act |= ACT_RAIDER | pr->act;

  //set strings
  free_string( pMob->name );
  pMob->name = str_dup(pr->name );
  free_string( pMob->long_descr);
  pMob->long_descr = str_dup(pr->long_desc );
  free_string( pMob->short_descr );
  pMob->short_descr = str_dup(pr->short_desc );

  //set mob's security to 6969 before putting it in room to prevent firestorm
  pMob->trust = 6969;
  char_to_room( pMob, pROri );
  act("$n enters the area.", pMob, NULL, NULL, TO_ROOM);
  pMob->trust = pMob->level;

  return TRUE;
}

/* INTERFACE */
//spawns raiders based on raid data
void raid_update( AREA_DATA* pArea, RAID_DATA* pr ){
  int mobs;
  int mobs_expected = 0;
  int count = 0;

  if (pArea == NULL || pr == NULL)
    return;

  if (pr->duration > pr->lifetime){
    char buf[MIL], buf1[MIL];
    CHAR_DATA* ch;
    free_raid( pr );
    REMOVE_BIT(pArea->area_flags, AREA_RAID);

    sprintf( buf, "A messenger yells '`6%s`6 has defeated the %ss!``'\n\r",
	     pArea->name,
	     pArea->raid->name); 
  sprintf(buf1, "A guard yells '`6The %ss are falling back! Chaaarge!!``'\n\r", 
	  pArea->raid->name);
    for (ch = player_list; ch; ch = ch->next_player){
      if (!IS_AWAKE( ch))
	continue;
      else if (ch->in_room->area == pArea)
	send_to_char( buf1, ch );
      else if (IS_AREA(pArea, AREA_CABAL) && is_same_cabal(ch->pCabal, pArea->pCabal))
	send_to_char( buf, ch );
    }

    pArea->raid = NULL;
    return;
  }
  else
    pr->duration += 2;

  //we get mobs expected by following linear relationship
  mobs_expected = pr->mob_total * pr->duration / pr->lifetime;
  //we get mobs to spawn by subtracting mobs spawned so far 
  //from expected
  mobs = mobs_expected - pr->mob_spawned;
  count = 0;
  while (mobs && count++ < 10){
    count++;
    if (raid_spawn( pArea, pr)){
      mobs--;
      pr->mob_spawned++;
    }
  }
}
    
    
//sets raid in an area
//type is one of the RAID_XXXX constants
void set_raid(AREA_DATA* pArea, int type, CABAL_DATA* pCab, int level){
  CHAR_DATA* ch;
  char buf[MIL];
  char buf1[MIL];

  int specRaid = RAID_NORMAL;
  int tot;
  int life;
  int vnum;
  int offset = 0;

  //get vnum
  if (level < 0)
    level = number_range(0, 3);

  switch (level){
  case 0:
    vnum = MOB_VNUM_RAID_WEAK;	break;
    offset = 0;
  case 1:
    offset = 1;
    vnum = MOB_VNUM_RAID_MEDI;	break;
  case 2:
    offset = 2;
    vnum = MOB_VNUM_RAID_NORM;	break;
  default:
    offset = 3;
    vnum = MOB_VNUM_RAID_STRO;	break;
  };


  //get lifetime and number of mobs
  if (IS_AREA(pArea, AREA_CABAL)){
    life = number_range(24, 48);
    tot = number_range(12, 24);
  }
  else{
    life = number_range(12, 24);
    tot = number_range(24, 48);
  }
    
  if (type >= 0 && type < RAID_MAX)
    specRaid = UMIN(type + offset, RAID_MAX);
  
  if (!create_raid( pArea, vnum, tot, life, specRaid, pCab)){
    bug("raid.c>set_raid: Could not create raid.", 0);
    return;
  }

  sprintf( buf, "A messenger yells '`6%s`6 is under attack by %ss!``'\n\r",
	   pArea->name,
	   pArea->raid->name);
  sprintf(buf1, "A guard yells '`6Scouts report %ss marching upon us! To the walls!``'\n\r", pArea->raid->name);

  for (ch = player_list; ch; ch = ch->next_player){
    if (!IS_AWAKE( ch))
      continue;
    else if (ch->in_room->area == pArea)
      send_to_char( buf1, ch );
    else if (IS_AREA(pArea, AREA_CABAL) && is_same_cabal(ch->pCabal, pArea->pCabal))
      send_to_char( buf, ch );
    else
      send_to_char( buf, ch );
  }

}

//Imm command for raids
//creates a raid of given type in area of given room
//raid <room number> [type]
void do_immraid( CHAR_DATA* ch, char* argument ){
  char arg[MIL];
  ROOM_INDEX_DATA* room;
  int vnum;
  int type;

  argument = one_argument(argument, arg );

  if (IS_NULLSTR(arg)){
    send_to_char("raid <room number> [normal/nature/undead/demon]\n\r", ch);
    return;
  }
  else if ( (vnum = atoi( arg)) < 1
	    || (room = get_room_index( vnum)) == NULL){
    send_to_char("Unknown room vnum.\n\r", ch);
    return;
  }

  if (!str_cmp( argument, "nature"))
    type = RAID_NATURE_1;
  else if (!str_cmp( argument, "demon"))
    type = RAID_DEMON_1;
  else if (!str_cmp( argument, "undead"))
    type = RAID_UNDEAD_1;
  else 
    type = RAID_NORMAL;

  set_raid(room->area, type, ch->pCabal, -1);

  if (room->area->raid == NULL){
    send_to_char("Error creating raid.\n\r", ch);
  }
  else
    sendf( ch, "Raid set:\n\r"\
	   "Raiders: %d\n\r"\
	   "Duration: %dh\n\r"\
	   "Vnum: %d (%s)\n\r",
	   room->area->raid->mob_total,
	   room->area->raid->lifetime,
	   room->area->raid->mob_vnum,
	   room->area->raid->short_desc);
}

//cabal ability to cause raids
//type of raid and cost decided randomly
//must target a city
void do_raid( CHAR_DATA* ch, char* argument ){
  AREA_DATA* pArea;
  CABAL_DATA* pCab;

  char arg[MIL], buf[MIL];
  int count = 0, city = 0;
  int level, cost, cabal_cost;
  int type;

  if (ch->pCabal == NULL){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else
    pCab = get_parent(ch->pCabal);

  //switch to imm command
  if (IS_IMMORTAL(ch) && get_trust(ch) >= CREATOR){
    do_immraid( ch, argument );
    return;
  }
  else if (!IS_CABAL(pCab, CABAL_RAID)){
    send_to_char("Huh?\n\r", ch);
    return;
  }    
  else if (is_captured( pCab)){
    send_to_char("Your altar lacks the power of your Standard.\n\r", ch);
    return;
  }

  argument = one_argument( argument, arg );

  if (IS_NULLSTR(arg)){
    send_to_char("Raid <area #> <strength 1-4>\n\rAreas:\n\r", ch);
    buf[0] = 0; count = 0;
    for (pArea = area_first; pArea; pArea = pArea->next){
      if (!IS_AREA(pArea, AREA_CITY)
	  && (!IS_CABAL(pCab, CABAL_ROUGE) || !IS_AREA(pArea, AREA_CABAL) || IS_AREA(pArea, AREA_RESTRICTED))
	  )
	continue;
      sprintf(buf, "%2d. %-20.20s (%s)\n\r", 
	      ++count, 
	      pArea->name, 
	      pArea->pCabal ? pArea->pCabal->name : "none" );
      send_to_char( buf, ch );
    }
    return;
  }
  else if ( (city = atoi(arg)) < 1){
    send_to_char("Raid <area #>\n\r", ch);
    return;
  }
  for (pArea = area_first; pArea; pArea = pArea->next){
    if (!IS_AREA(pArea, AREA_CITY)
	&& (!IS_CABAL(pCab, CABAL_ROUGE) || !IS_AREA(pArea, AREA_CABAL)))
      continue;
    if (++count != city)
      continue;
    else
      break;
  }
  if (pArea == NULL){
    send_to_char("No such area.  Use \"raid\" for list.\n\r", ch);
    return;
  }
  if (pArea->raid){
    send_to_char("That area is already under attack!\n\r", ch);
    return;
  }
  else if (mud_data.current_time - pArea->last_raid < RAID_WAIT){
    int hour = (RAID_WAIT + pArea->last_raid - mud_data.current_time) / 30;
    int days = hour / MAX_HOURS;

    sendf(ch, "The area's defenses will still be altert for %d %s%s.\n\r", 
	  days > 1 ? days : hour,
	  days > 1 ? "day" : "hour",
	  (days  == 1 || hour == 1) ? "" : "s");
    return;
  }
  else if (mud_data.current_time - pCab->raid_stamp < CAB_RAID_WAIT){
    int hour = (CAB_RAID_WAIT + pCab->raid_stamp - mud_data.current_time) / 30;
    int days = hour / MAX_HOURS;

    sendf(ch, "%s will not be ready to raid for another %d %s%s.\n\r", 
	  pCab->name,
	  days > 1 ? days : hour,
	  days > 1 ? "day" : "hour",
	  (days  == 1 || hour == 1) ? "" : "s");
    return;
  }
  else if (pArea->pCabal && is_friendly(pArea->pCabal, pCab) != CABAL_ENEMY){
    send_to_char("You must declare a vendetta first.\n\r", ch);
    return;
  }
  //get level of the mobs to use
  level = URANGE(1, atoi(argument), 4) - 1;
  //get cost
  cost = 50 + (level * 25);
  cabal_cost = UMIN(GET_CAB_CP(pCab), 500 + (level * 200));
  if (cabal_cost < 0)
    cabal_cost = 0;

  if (GET_CP(ch) < cost){
    sendf(ch, "You need at least %d %ss to order a raid.\n\r",
	  cost, ch->pCabal->currency);
    return;
  }
  CP_GAIN(ch, -cost, TRUE );
  CP_CAB_GAIN(pCab, -cabal_cost );

  //select type
  if (!str_cmp(pCab->name, "Watcher"))
    type = RAID_NATURE_1;
  else if (IS_UNDEAD(ch))
    type = RAID_UNDEAD_1;
  else if (IS_DEMON(ch))
    type = RAID_DEMON_1;
  else
    type = RAID_NORMAL;

  set_raid(pArea, type, pCab, level);
  if (pArea->raid == NULL){
    send_to_char("Error.  Contact Staff.\n\r", ch);
    return;
  }
  sprintf(buf, "On %s's orders %ss march on %s.",
	  ch->name, pArea->raid->name, pArea->name);
  cabal_echo( pCab, buf );
  if (cabal_cost){
    sprintf(buf, "%d %ss have been drawn from coffers to pay for the raid.",
	    cabal_cost, pCab->currency);
    cabal_echo( pCab, buf );
  }
}

//checks if a kill on raid mob should increase the killer's cabal sup/cps
//or grant the killer cps
void raidmob_check( CHAR_DATA* vch, CHAR_DATA* victim){
  CHAR_DATA* ch = vch, *wch;
  CABAL_DATA* pCab;
  char buf[MIL];
  int gold;

  if (victim == NULL || vch == NULL || victim->in_room == NULL)
    return;
  else if (!IS_NPC(victim) || !IS_SET(victim->act, ACT_RAIDER))
    return;
  pCab = victim->in_room->area->pCabal;

  //charmie to owner switch
  if (IS_AFFECTED(vch, AFF_CHARM) && IS_NPC(vch) && vch->master)
    ch = vch->master;
  else
    ch = vch;

  if (IS_NPC(ch))
    return;

  //always get some gold unless member of same cabal
  for (wch = ch->in_room->people; wch; wch = wch->next_in_room){
    if (wch->fighting != victim)
      continue;
    else if (wch->pCabal != NULL 
	     && is_same_cabal(wch->pCabal, victim->pCabal))
      continue;
    gold = number_range(300, 1000);
    sprintf( buf, "The bank rewards you with %d gold.\n\r", gold );
    send_to_char( buf, wch );
    wch->in_bank +=  gold;
    if (wch->pCabal && !is_same_cabal(pCab, wch->pCabal)){
      CP_GAIN(wch, number_range(1, 3), TRUE );
    }
  }

  //cabal related rewards
  if (pCab){
    long sup_dam, cps_dam;
    
    sup_dam = number_range(1, 12 * CABAL_FULL_SUPPORT) / 6;
    cps_dam = number_range( CPTS , 3 * CPTS);      
    
    distribute_cps( ch, pCab, cps_dam, TRUE );
    
    //If person member of justice cabal and is in lawful city gain cps
    //Or if person is in lawful city belonging to their cabal
    if (IS_CABAL(pCab, CABAL_JUSTICE) 
	&& IS_AREA(victim->in_room->area, AREA_LAWFUL) ){
      justice_raid_gain( sup_dam, cps_dam );
    }
    //The raided cabal gains back some damage
    if (IS_AREA(victim->in_room->area, AREA_CITY)){
      pCab->raid_cps += cps_dam;
      pCab->raid_sup += sup_dam;
    }
  }
}
      
