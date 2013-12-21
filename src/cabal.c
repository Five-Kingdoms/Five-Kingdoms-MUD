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

/* NOTE: If you are looking for old cabal.c its in cabal_old.c */
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <malloc.h>
#include "merc.h"
#include "recycle.h"
#include "cabal.h"
#include "vote.h"
#include "olc.h"
#include "db.h"
#include "save_mud.h"
#include "bounty.h"
#include "armies.h"
#include "clan.h"

/***************************************************************/
/*Following are cabal orientanted routines for Age of Mirlan  */
/*mud created by Virigoth circa 2001.  Copyrighted for Forsaken*/
/*Lands mud Nov 1, 2001.  Do not use or copy without explicit */
/*permisson of author. (Voytek Plawny aka Virigoth)	       */
/***************************************************************/

extern bool format_room_name(ROOM_INDEX_DATA* room, char* room_name);
void do_cstat( CHAR_DATA* ch, char* argument );

/* list of all cabals created */
CABAL_DATA* cabal_list = NULL;
CABAL_INDEX_DATA *	cabal_index_list = NULL;

/* global index for pacts */
PACT_DATA* pact_list = NULL;
int	max_cabal_index = 0;
int	max_cabal = 0;

/* global array with letters that are universal for each rank */
char*	const	rank_letters [RANK_MAX] =
/* [I]nitiate, [M]ember, [V]eteran, [T]rusted, [E]lder, [L]eader */
{  "I",		"M",	  "V",	     "T",	"E",	 "L"};


/* returns support cost of croom */
int get_croom_cost( int level ){
  return (croom_table[level].cost * CABAL_FULL_SUPPORT * WORLD_SUPPORT_VALUE / 10000);
}

/* returns cp cost of croom to build*/
int get_croom_bcost( int level ){
  int cost = croom_table[level].cost * CABAL_FULL_SUPPORT * WORLD_SUPPORT_VALUE / 3000;
  if (IS_SET(croom_table[level].flags, CABAL_COFFER) && cost > 500)
    cost = 500;
  return (cost);
}

/* shows cabal's rooms and their status to a person */
void show_cabal_rooms(CHAR_DATA* ch, CABAL_DATA* pCabal ){
  CABAL_DATA* pCab = get_parent( pCabal );
  /*
    CVROOM_DATA* pcv;
    int croom_active[CROOM_LEVELS];
    int croom_unactive[CROOM_LEVELS];
    int i, total_cost = 0, tot_rooms = 0;
  */
  bool fCapture = FALSE;

  /* NOT NEEDED
  for (i = 0; i < CROOM_LEVELS; i++){
    croom_active[i] = croom_unactive[i] = 0;
  }

  //run through the rooms and add up active and inactive rooms
  for (pcv = pCab->cvrooms; pcv; pcv = pcv->next){
    if (!pcv->fComplete)
      continue;

    //active
    if (croom_table[pcv->level].flags == 0 || IS_CABAL(pCab, croom_table[pcv->level].flags)){
      croom_active[pcv->level] += 1;
      total_cost += get_croom_cost( pcv->level );
    }
    else
      croom_unactive[pcv->level] += 1;
    tot_rooms++;
  }

  send_to_char("Active	Sup	Type	      Pow   Name\n\r", ch );
  send_to_char("---------------------------------------------\n\r", ch);
  for (i = 0; i < CROOM_LEVELS; i++){
    char* pow;

    if (IS_SET(croom_table[i].flags, CABAL_ALLOW_2))
      pow = "Vet";
    else if (IS_SET(croom_table[i].flags, CABAL_ALLOW_3))
      pow = "Tru";
    else if (IS_SET(croom_table[i].flags, CABAL_ALLOW_4))
      pow = "Eld";
    else if (IS_SET(croom_table[i].flags, CABAL_ALLOW_5))
      pow = "Lea";
    else
      pow = "   ";

    if (croom_active[i] || croom_unactive[i]){
      sendf(ch, "%2d/%-2d  %-5d %-15.15s  %s %-35.35s\n\r",
	    croom_active[i],
	    croom_active[i] + croom_unactive[i],
	    UMAX(1, croom_active[i]) * get_croom_cost( i ),
	    croom_table[i].name,
	    pow,
	    pCab->pIndexData->crooms[i]->pRoomIndex->name);
    }
    else{
      sendf(ch, " ---   %-5d %-15.15s  %s %-35.35s\n\r",
	    get_croom_cost( i ),
	    croom_table[i].name,
	    pow,
	    pCab->pIndexData->crooms[i]->pRoomIndex->name);
    }
  }
  sendf(ch, "Total of %d rooms costing %d support.\n\r",
	tot_rooms, total_cost);
  */

  if (is_captured( pCab))
    fCapture = TRUE;
  sendf(ch, "Active Powers: %s%s%s%s\n\r",
	IS_CABAL(pCab, CABAL_ALLOW_2) && !fCapture ? "Veteran " : "",
	IS_CABAL(pCab, CABAL_ALLOW_3) && !fCapture ?  "Trusted " : "",
	IS_CABAL(pCab, CABAL_ALLOW_4 )&& !fCapture ?  "Elder " : "",
	IS_CABAL(pCab, CABAL_ALLOW_5) && !fCapture ?  "Leader" : ""
	);
  if (fCapture)
    send_to_char("Your item has been CAPTURED!\n\r", ch);
}


/* shows cabal info to a single member */
void show_cabal_info( CHAR_DATA* ch, CABAL_DATA* pCabal ){
  CABAL_DATA* pCab = get_parent( pCabal );
  CMEMBER_DATA* cMem;

  char buf[MSL];
  int cnt = 0;
  bool fEld = FALSE;

/* leader/imm check */
  if (IS_IMMORTAL(ch) && (!str_cmp(ch->name, pCab->immortal) || !str_cmp(ch->name, pCab->immortal)))
    fEld = TRUE;
  else if (IS_ELDER( ch ) && ch->pCabal && get_parent(ch->pCabal) == pCab)
    fEld = TRUE;
  else if (get_trust(ch) > CREATOR)
    fEld = TRUE;

  /* NOT NEEDED
  health_prompt( buf, UMAX(0, ECONOMY_UPDATE - (mud_data.current_time - pCab->room_stamp)), ECONOMY_UPDATE, FALSE );
  */
  sendf( ch, "[%s]  (%d %ss at %d%%)\n\r", /* RENT: %s\n\r", */
	 pCab->who_name,
	 GET_CAB_CP( pCab),
	 pCab->currency,
	 pCab->cp_tax);
	 //	 buf);

  sendf( ch, "Cabal Immortal: %s\n\r", pCab->immortal);
  if (IS_CABAL(pCab, CABAL_RAID)){
    if (mud_data.current_time - pCab->raid_stamp < CAB_RAID_WAIT){
      int hour = (CAB_RAID_WAIT + pCab->raid_stamp - mud_data.current_time) / 30;
      int days = hour / MAX_HOURS;
      sendf(ch, "Next Raid: %d %s%s.\n\r",
	    days > 1 ? days : hour,
	    days > 1 ? "day" : "hour",
	    (days  == 1 || hour == 1) ? "" : "s");
    }
    else
      send_to_char("Next Raid: READY\n\r", ch);
  }
  send_to_char("\n\r", ch);

  sendf( ch, "ECONOMY: Influence   %ss\n\r", capitalize(pCab->currency));
  sendf( ch, " Initial:   %-4d%%     %-4ld\n\r", 100 * pCab->support[ECO_INCOME] / CABAL_FULL_SUPPORT_VAL, pCab->cp_gain[ECO_INCOME] / CPTS);
  sendf( ch, " Bonus  :   %-4d%%        \n\r", 100 * pCab->support[ECO_LAST_BONUS] / CABAL_FULL_SUPPORT_VAL);
  sendf( ch, " Trade  :   %-4d      %-4ld\n\r", pCab->support[ECO_TRADE], pCab->cp_gain[ECO_TRADE] / CPTS);
  if (pCab->support[ECO_LAST_ARMY])
  sendf( ch, " Armies :   %-4d%%\n\r", 100 * pCab->support[ECO_LAST_ARMY] / CABAL_FULL_SUPPORT_VAL);
  //  sendf( ch, " Rooms  :  -%-4d\n\r", pCab->support[ECO_LAST_RENT]);
  //  sendf( ch, " Corru  :  -%-4d\n\r", pCab->support[ECO_LAST_CORR]);
  sendf( ch, "        -------------------\n\r");
  sendf( ch, " Gain   :   %-4d%%     %-4ld\n\r\n\r", 100 * pCab->support[ECO_GAIN] / CABAL_FULL_SUPPORT_VAL, pCab->cp_gain[ECO_GAIN] / CPTS);

  show_cabal_rooms(ch, pCab );

  if (fEld){
    sprintf( buf, "\n\rMembers: [%d/%d] Elders: [%d/%d]  Leaders [%d/%d]\n\r",
	     pCab->cur_member, pCab->max_member,
	     pCab->cur_elder,  pCab->max_elder,
	     pCab->cur_leader, pCab->max_leader);
    send_to_char( buf, ch );

    /* show members that you can see */
    for ( cnt = 0, cMem = pCab->members; cMem; cMem = cMem->next ){
      if (cMem->level > 50 && !IS_IMMORTAL(ch))
	continue;
      if ( cnt == 0 ){
	send_to_char( "Number     Name       Level   Rank   Days Ago  Cabal\n\r", ch );
	send_to_char( "------ ------------- ------- ------- --------- ------------\n\r", ch );
      }
      sprintf( buf, "[%4d] %-15.15s %-6d   %-5d %-5ld     %-10.10s``\n\r",
	       cnt,
	       cMem->name,
	       cMem->level,
	       cMem->rank,
	       (mud_data.current_time - cMem->last_login) / VOTE_DAY,
	       cMem->pCabal->who_name);
      send_to_char( buf, ch );
      cnt++;
    }
  }
  send_to_char("\n\r", ch );

  /*
  sendf( ch, "ROOMS: %d/%d possible costing %d support.\n\r",
	 pCab->cur_room,
	 pCab->pIndexData->max_room,
	 pCab->support[ECO_LAST_RENT] );
  */

  if (IS_CABAL(pCab, CABAL_TRADE) || IS_CABAL(pCab, CABAL_NAPACT)
      || IS_CABAL(pCab, CABAL_ALLIANCE) || IS_CABAL(pCab, CABAL_VENDETTA)){
    send_to_char("PACTS:\n\r", ch );
    show_pacts( ch, pCab );
  }
}

/* shows crooms cabal has */
void show_crooms( CHAR_DATA* ch, CROOM_DATA** crooms ){
  CROOM_DATA* croom;
  int i;
  int count = 0;
  char buf[MSL];

  send_to_char("  Room Type    Sup  Cps   Description\n\r", ch);
  send_to_char("------------------------------------------\n\r", ch );
  for (i = 0; i < CROOM_LEVELS; i++){
    if (crooms[i] != NULL)
      count++;
    croom = crooms[i];

    if (croom){
      sprintf( buf, "%2d. %-10.10s %-4d %-5d %s\n\r",
	       count,
	       croom_table[i].name,
	       get_croom_cost( i ),
	       get_croom_bcost( i ),
	       croom_table[i].desc);
    }
    else{
      sprintf( buf, "--. %-10.10s %-4d %-5d %s\n\r",
	       croom_table[i].name,
	       get_croom_cost( i ),
	       get_croom_bcost( i ),
	       croom_table[i].desc);
    }
    send_to_char( buf, ch );
  }
}

/* creates a new cabal index */
CABAL_DATA* new_cabal(){
  CABAL_DATA *pc = malloc( sizeof( *pc ));
  memset( pc, 0, sizeof( CABAL_DATA ));

  return pc;
}

/* frees existing cabal data */
void free_cabal( CABAL_DATA* pc ){
  CABAL_DATA* ca = pc;
  CVROOM_DATA* cVroom = ca->cvrooms;
  CMEMBER_DATA* cMem = ca->members;

  while ( ca->members ){
    cMem = ca->members;
    ca->members = ca->members->next;

    free_cmember( cMem );
  }

  while ( ca->cvrooms ){
    cVroom = ca->cvrooms;
    ca->cvrooms = ca->cvrooms->next;

    free_cvroom( cVroom );
  }

  if (!IS_NULLSTR(pc->name))
    free_string(pc->name );
  if (!IS_NULLSTR(pc->currency))
    free_string(pc->currency);
  if (!IS_NULLSTR(pc->who_name))
    free_string(pc->who_name);
  if (!IS_NULLSTR(pc->immortal))
    free_string(pc->immortal);

  free( pc );
}

/* adds a cabal onto the list of all cabals */
void add_cabal( CABAL_DATA *pc ){
  pc->next = cabal_list;
  cabal_list = pc;
  max_cabal ++;
}

/* removes a cabal from the list */
void rem_cabal( CABAL_DATA *pc ){
  CABAL_DATA* prev = cabal_list;

  if (prev == NULL){
   bug("cabal.c>rem_cabal : Cannot remove cabal as non exist.", 0);
   return;
  }
  else if (prev == pc ){
    cabal_list = NULL;
    max_cabal--;
    return;
  }

  for (; prev->next; prev = prev->next){
    if (prev->next == pc){
      prev->next = pc->next;
      max_cabal--;
      return;
    }
  }
  bug("cabal.c>rem_cabal : Cannot remove cabal as non exist.", 0);
  return;
}

/* creates a cabal based on passed index */
CABAL_DATA* create_cabal( CABAL_INDEX_DATA* pIndex ){
  int i=0;
  CABAL_DATA* pc;

  if (pIndex == NULL)
    return NULL;

  //get blank cabal data
  pc = new_cabal();

  pc->pIndexData		= pIndex;
  pc->vnum			= pIndex->vnum;
  pc->version			= pIndex->version;

  if (pIndex->anchor_vnum){
    pc->anchor			= get_room_index( pIndex->anchor_vnum);
    if (pc->anchor == NULL)
      bug("cabal.c>create_cabal: could not load anchor vnum %d.", pIndex->anchor_vnum);
  }
  if (!IS_NULLSTR(pIndex->city_name)){
    pc->city			= get_area_data_str( pIndex->city_name );
    if (pc->city == NULL)
      nlogf("cabal.c>create_cabal: could not load city %s.", pIndex->city_name);
  }
  if (!IS_NULLSTR(pIndex->clan)){
    pc->clan			= GetClanHandle( pIndex->clan );
    if (pc->clan < 1)
      nlogf("cabal.c>create_cabal: could not load clan %s.", pIndex->clan);
  }
  if (pIndex->guard_vnum){
    pc->guard			= get_mob_index( pIndex->guard_vnum );
    if (pc->guard == NULL)
      bug("cabal.c>create_cabal: could not load guard vnum %d.", pIndex->guard_vnum);
  }
  if (pIndex->pit_vnum){
    pc->pitroom			= get_room_index( pIndex->pit_vnum );
    if (pc->pitroom == NULL)
      bug("cabal.c>create_cabal: could not load pitroom vnum %d.", pIndex->pit_vnum);
  }
  pc->name			= str_dup( pIndex->name );
  pc->currency			= str_dup( pIndex->currency );
  pc->who_name			= str_dup( pIndex->who_name );
  pc->immortal			= str_dup( pIndex->immortal );

  pc->max_member		= pIndex->max_member;

  for (i = 0; i < MAX_CLASS; i ++){
    pc->class[i] = pIndex->class[i];
  }
  for (i = 0; i < MAX_PC_RACE; i ++){
    pc->race[i] = pIndex->race[i];
  }
  pc->fAvatar			= pIndex->fAvatar;
  pc->align			= pIndex->align;
  pc->ethos			= pIndex->ethos;
  pc->progress			= pIndex->progress;
  pc->flags			= pIndex->flags;
  pc->flags2			= pIndex->flags2;

  /* reset the report queue */
  init_repq(&pc->report_q);

  //add cabal to list of all cabals
  add_cabal( pc );
  return pc;
}

/* create a new cabal room data */
CROOM_DATA *new_croom(void){

  CROOM_DATA *cr = malloc( sizeof( *cr ));

/* reset initial things */
  cr->pRoomIndex	= NULL;
  cr->level		= 0;

  return cr;
};

void free_croom( CROOM_DATA* cr ){
  free( cr );
}

/* create a new cabal virtual room data */
CVROOM_DATA *new_cvroom(void){

  CVROOM_DATA *cvr = malloc( sizeof( *cvr ));

/* reset initial things */
  cvr->pRoom		= NULL;
  cvr->next		= NULL;
  cvr->fComplete	= FALSE;
  cvr->vnum		= 0;

  cvr->level		= 0;
  cvr->pos[0] =   cvr->pos[1] =   cvr->pos[2] = 0;
  return cvr;
};

void free_cvroom( CVROOM_DATA* cvr ){
  if (cvr == NULL)
    return;
/* Viri: this seems to cause problems
  if (cvr->pRoom)
    free_room_index( cvr->pRoom );
*/
  free( cvr );
}

void set_cvroom_flags( CVROOM_DATA* pcv, CABAL_DATA* pc, bool fAdd ){
  if (!pcv->fComplete)
    return;
  if (fAdd){
    SET_BIT(pc->flags, croom_table[pcv->level].flags);
    if (IS_SET(croom_table[pcv->level].flags2, CROOM_PITROOM)
	&& pc->pitroom == NULL){
      pc->pitroom = pcv->pRoom;
    }
  }
  else{
    /* remove the cabal flag */
    if (pcv->fComplete){
      REMOVE_BIT(pc->flags, croom_table[pcv->level].flags);
      if (IS_SET(croom_table[pcv->level].flags2, CROOM_PITROOM) && pc->pitroom != NULL){
	pc->pitroom = NULL;
      }
    }
  }
}


/* attatch a cvroom to a cabal */
void cvroom_to_cabal( CVROOM_DATA* pcv, CABAL_DATA* pc ){
  if (pcv == NULL ){
    bug("cvroom_to_cabal: null pcv passed.", 0);
    return;
  }
  else if (pc == NULL ){
    bug("cvroom_to_cabal: null pc passed.", 0);
    return;
  }
  else{
    pcv->next = pc->cvrooms;
    pc->cvrooms = pcv;
    if (pcv->pRoom)
      pcv->pRoom->pCabal = pc;
  }
  /* add the room flags */
  set_cvroom_flags( pcv, pc, TRUE);
}

void refresh_cvroom_flags( CABAL_DATA* pc ){
  CVROOM_DATA* pcv;

  for (pcv = pc->cvrooms; pcv; pcv = pcv->next ){
    set_cvroom_flags( pcv, pc, TRUE );
  }
}

/* remove a cvroom from cabal */
void cvroom_from_cabal( CVROOM_DATA* pcv, CABAL_DATA* pc ){
  CVROOM_DATA* prev;

  if (pcv == NULL ){
    bug("cvroom_from_cabal: null pcv passed.", 0);
    return;
  }
  else if (pc == NULL ){
    bug("cvroom_from_cabal: null pc passed.", 0);
    return;
  }
  else if (pc->cvrooms == NULL){
    bug("cvroom_from_cabal: cabal had no virtual rooms. cab.vnum: %d", pc->vnum );
    return;
  }
  else{
    prev = pc->cvrooms;
  }

  if (prev == pcv ){
    pc->cvrooms = prev->next;
  }
  else {
    while (prev->next && prev->next != pcv ){
      prev = prev->next;
    }
  }
  if (prev == NULL){
    bug("cvroom_from_cabal: room not found in cabal %d.", pc->vnum);
    return;
  }
  prev->next = pcv->next;
  pcv->next = NULL;

  set_cvroom_flags( pcv, pc, FALSE );
  refresh_cvroom_flags( pc );
}


/* save cabal's virtual rooms to file */
void save_cvrooms( CABAL_DATA* pCab ){
  FILE *fp;
  CVROOM_DATA* pcv;
  OBJ_DATA* pObj, *pObj_next;
  char path[MIL], old[MIL];

  fclose( fpReserve );
  sprintf( path, "%s%s", SAVECAB_DIR, TEMPCVR_FILE );
  sprintf( old, "%s%s.ovi", SAVECAB_DIR, capitalize(pCab->pIndexData->file_name) );
  if ( ( fp = fopen( path, "w" ) ) == NULL ){
    perror( path );
    fclose (fp);
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }

  /* start writting the vrooms to the file */
  for (pcv = pCab->cvrooms; pcv; pcv = pcv->next ){
    if (pcv->pRoom == NULL )
      continue;

    /* cvroom data first */
    fprintf( fp, "#VIRROOM\n%d %d %d\n%d %d\n\r",
	     pcv->pos[P_X], pcv->pos[P_Y], pcv->pos[P_Z],
	     pcv->vnum, pcv->fComplete);
    fprintf( fp, "%d\n", pcv->level );

    /* all the pcv data has been written, we now write the room */
    write_room( fp, pcv->pRoom );

    /* now write this room's resets, since we do not create area resets section. */
    write_resets( fp, pcv->pRoom );
    fprintf( fp, "S\n\n" );

    /* last thing to enter are objects if any */
    for (pObj = pcv->pRoom->contents; pObj; pObj = pObj_next ){
      pObj_next = pObj->next_content;

      if (!IS_OBJ_STAT(pObj, ITEM_MELT_DROP)
	  && pObj->in_room != NULL
	  && pObj->carried_by == NULL
	  && (IS_LIMITED(pObj)
	      || pObj->homevnum != pObj->in_room->vnum
	      || pObj->contains != NULL
	      || CAN_WEAR(pObj, ITEM_HAS_OWNER)) ){
	fwrite_obj(NULL, pObj, fp, 0, 0, FALSE);
      }
    }//end for objects
  }//end for cvrooms

/* terminate the file */
  fprintf( fp, "#End\n" );
  fclose (fp);
/* copy current .vir to .ovi, and the temporary file to .vir */
  sprintf( path, "%s%s.vir", SAVECAB_DIR, capitalize(pCab->pIndexData->file_name) );
  rename( path, old );
  sprintf( old, "%s%s", SAVECAB_DIR, TEMPCVR_FILE );
  rename( old, path );
  fpReserve = fopen( NULL_FILE, "r" );
}

/* reads in resets for a single cabal virtual room */
void read_cvresets( FILE* fp, ROOM_INDEX_DATA* pRoomIndex ){
  RESET_DATA *pReset = NULL;

  for ( ; ; ){
    char letter = '\0';

    letter = fread_letter (fp);

    if (letter == 'S') {
      break;
    }
    else if (letter == '*' ) {
      fread_to_eol( fp );
      continue;
    }
    pReset		= new_reset_data();
    pReset->command	= letter;
    fread_number( fp );
    pReset->arg1	= fread_number( fp );
    pReset->arg2	= fread_number( fp );
    pReset->arg3	= (letter == 'G' || letter == 'R') ? 0 : fread_number( fp );
    pReset->arg4	= (letter == 'P' || letter == 'M' || letter == 'T') ? fread_number(fp) : 0;
    fread_to_eol( fp );

    switch ( letter ){
    default:
      bug( "Load_cvresets: bad command '%c'.", letter );
      exit( 1 );
      break;
    case 'M':
      new_reset( pRoomIndex, pReset );
      break;
    case 'O':
      new_reset( pRoomIndex, pReset );
    case 'P':
      new_reset( pRoomIndex, pReset );
      break;
    case 'G':
    case 'E':
      new_reset( pRoomIndex, pReset );
      break;
    case 'R':
      if ( pReset->arg2 < 0 || pReset->arg2 > MAX_DIR ){
	bug( "Load_cvresets: 'R': bad exit %d.", pReset->arg2 );
	exit( 1 );
      }
      new_reset( pRoomIndex, pReset );
      break;
    case 'T':
      new_reset( pRoomIndex, pReset );
      break;
    }
  }
}
/* read a single cabal vroom from file */
bool read_cvroom( FILE* fp, CVROOM_DATA* pcv ){

  pcv->pos[P_X]		= fread_number( fp );
  pcv->pos[P_Y]		= fread_number( fp );
  pcv->pos[P_Z]		= fread_number( fp );

  pcv->vnum		= fread_number( fp );
  pcv->fComplete	= fread_number( fp );
  pcv->level		= fread_number( fp );

  /* get the room data */
  pcv->pRoom = new_room_index();
  read_room( fp, pcv->pRoom, NULL, 0 );

  /* get the reset data */
  read_cvresets( fp, pcv->pRoom );


  return TRUE;
}

/* save a single cabal to a file */
void save_cabal( CABAL_DATA* pCab){
  CMEMBER_DATA* cMem;

  FILE *fp;
  char path[MIL];

  char buf[MIL];
  int i = 0, flag = 0;
  fclose( fpReserve );

  sprintf( path, "%s%s", SAVECAB_DIR, TEMPCAB_FILE );
  if ( ( fp = fopen( path, "w" ) ) == NULL ){
    perror( path );
    fclose (fp);
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }

// economy info
  fprintf( fp, "Cp %ld ", pCab->cp);
  fprintf( fp, "CpTax %d\n", pCab->cp_tax);
  fprintf( fp, "BonSup %ld ", pCab->bonus_sup);
  fprintf( fp, "Time %ld\n", pCab->time_stamp);
  fprintf( fp, "RTime %ld\n",pCab->room_stamp);
  fprintf( fp, "MTime %ld\n",pCab->member_stamp);
  fprintf( fp, "Armies %d\n",pCab->armies_ready);
  fprintf( fp, "PresentTime %ld\n",pCab->present_stamp);

//  fprintf( fp, "RaTime %ld\n",pCab->raid_stamp);
//  fprintf( fp, "InCp %ld\n", pCab->cp_gain[ECO_INCOME]);
//  fprintf( fp, "InSup %d\n", pCab->support[ECO_INCOME]);

  fprintf( fp, "CurMem %d\n", pCab->cur_member);

  // Before writting the flags, we remove any flags in cabal_flag table that cannot be manualy set.
  flag = pCab->flags;
  for (i = 0; cabal_flags[i].name; i ++){
    if (cabal_flags[i].settable == FALSE)
      flag &= ~cabal_flags[i].bit;
  }

//  fprintf( fp, "Flags %s\n", fwrite_flag( flag, buf ) );
//  fprintf( fp, "Flags2 %s\n", fwrite_flag( pCab->flags2, buf ) );

  for( cMem = pCab->members; cMem; cMem = cMem->next){
    fprintf( fp, "Member %d %s~ %s~ %s~\n", cMem->pCabal->vnum, cMem->name, cMem->sponsor, cMem->sponsored);
    fprintf( fp, "%d %d %d %d %d %ld %ld\n", cMem->class, cMem->race, cMem->level, cMem->rank, cMem->kills, cMem->cp, cMem->cp_pen);
    fprintf( fp, "%d %d %d %ld %ld %ld\n", cMem->month, cMem->mhours, cMem->hours, cMem->last_update, cMem->time_stamp, cMem->last_login);
    fprintf( fp, "%d ", MAX_TRAIN_QUEUE );
    for (i = 0; i < MAX_TRAIN_QUEUE; i++){
      fprintf( fp, "%d ", cMem->armies[i] );
    }
    fprintf( fp, "\n" );
  }

  fprintf( fp, "End\n#0\n");
  fclose (fp);
  fpReserve = fopen( NULL_FILE, "r" );

//copy the cabal.cab to cabal.old and cabal.tmp to cabal.cab
  sprintf( buf, "%s%s.cab", SAVECAB_DIR, capitalize(pCab->pIndexData->file_name) );
  rename( path, buf);
}

/* save all cabals to a file */
void save_cabals(bool fChanged, CHAR_DATA* ch){
  CABAL_DATA* pCab = cabal_list;

/*  loop through each cabal and save it to its own file */
  for(pCab = cabal_list; pCab; pCab = pCab->next){
    if (fChanged){
      bool fSaved = FALSE;
      /* Useless conditional */
      if( fSaved == TRUE )
        fSaved = FALSE;
      if (IS_CABAL(pCab, CABAL_CHANGED)){
	save_cabal( pCab );
	fSaved = TRUE;
      }
      if (IS_CABAL(pCab, CABAL_VRCHANGED)){
	save_cvrooms( pCab );
	fSaved = TRUE;
      }
    }
    else{
      save_cabal( pCab );
      save_cvrooms( pCab );
    }
    REMOVE_BIT(pCab->flags, CABAL_CHANGED);
    REMOVE_BIT(pCab->flags, CABAL_VRCHANGED);
  }
  save_pacts( );
  if (ch)
    send_to_char("Pacts saved.\n\r", ch);
}


/* reads a single cabal from file into a structure */
bool fread_cabal( FILE *fp, CABAL_DATA* pCab ){
  char *word = NULL;
  bool fMatch = FALSE;

  for ( ; ; ){
    word   = feof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER(word[0]) ){
    case '*':
      fMatch = TRUE;
      fread_to_eol( fp );
      break;
    case 'A':
      KEY( "Armies",		pCab->armies_ready,	fread_number( fp ) );
      break;
    case 'B':
      KEY( "BonSup",		pCab->bonus_sup,	fread_number(fp)   );
    case 'C':
      KEY( "Cp",		pCab->cp,		fread_number(fp)   );
      KEY( "CpTax",		pCab->cp_tax,		fread_number(fp)   );
      KEY( "CurMem",		pCab->cur_member,	fread_number( fp ) );
      break;
    case 'E':
      if ( !str_cmp( word, "End" ) ){
	return TRUE;
      }
      break;
    case 'F':
      KEY( "Flags",		pCab->flags,		fread_flag(fp)	   );
      KEY( "Flags2",		pCab->flags2,		fread_flag(fp)	   );
      break;
    case 'G':
      break;
    case 'I':
      KEY( "InCp",		pCab->cp_gain[ECO_INCOME],fread_number( fp ) );
      KEY( "InSup",		pCab->support[ECO_INCOME],fread_number( fp ) );
      break;
    case 'M':
      if (!str_cmp( word, "Member" )){
	CMEMBER_DATA* cMem = new_cmember();
	int max, i;

	free_string( cMem->name);
	free_string( cMem->sponsor);
	free_string( cMem->sponsored);

	cMem->vnum      = fread_number( fp );
	cMem->name	= fread_string( fp );
	cMem->sponsor	= fread_string( fp );
	cMem->sponsored	= fread_string( fp );

	cMem->class	= fread_number( fp );
	cMem->race	= fread_number( fp );
	cMem->level	= fread_number( fp );
	cMem->rank	= fread_number( fp );
	cMem->kills	= fread_number( fp );
	cMem->cp	= fread_number( fp );
	cMem->cp_pen	= fread_number( fp );

	cMem->month	= fread_number( fp );
	cMem->mhours	= fread_number( fp );
	cMem->hours	= fread_number( fp );
	cMem->last_update= fread_number( fp );
	cMem->time_stamp= fread_number( fp );
	cMem->last_login= fread_number( fp );

	//army queues
	max		= fread_number( fp );
	max		= UMIN( MAX_TRAIN_QUEUE, max );
	for (i = 0; i < max; i++){
	  cMem->armies[i]	= fread_number( fp );
	}
	cmember_to_cabal( cMem, pCab );

	fMatch = TRUE;
	break;
      }
      KEY( "MTime",		pCab->member_stamp,	fread_number( fp ) );
      break;
    case 'N':
      break;
    case 'O':
      break;
    case 'R':
      KEY( "RTime",		pCab->room_stamp,	fread_number( fp ) );
      KEY( "RaTime",		pCab->raid_stamp,	fread_number( fp ) );
      break;
    case 'P':
      KEY( "PresentTime",	pCab->present_stamp,	fread_number( fp ) );
      break;
    case 'S':
      break;
    case 'T':
      KEY( "Time", pCab->time_stamp, fread_number( fp ) );
      break;
    case 'V':
      break;
    case 'W':
      break;
    }
  // cheack for key match
    if ( !fMatch ){
      nlogf("Fread_cabal: no match: %s", word);
      fread_to_eol(fp);
    }
  }
  return TRUE;

}



/* udpates some odd info after loading all cabals */
void fix_cabals(){
  CABAL_INDEX_DATA* pci;
  CABAL_DATA* pCab = cabal_list;
  CMEMBER_DATA* cMem = pCab->members;
  CVROOM_DATA* pcv = pCab->cvrooms;
  CVROOM_DATA* pcv_next;
  int i;

/* FIX CABAL INDEXES */
  for (pci = cabal_index_list ;pci != NULL; pci = pci->next){
    if (pci->parent_vnum){
      continue;
    }
    /* get room pointers for crooms */
    for (i = 0; i < CROOM_LEVELS; i++){
      if (pci->crooms[i] == NULL){
	/* create default dummy rooms if they do not exist */
	if (i < 2){
	  pci->crooms[i] = new_croom();
	  pci->crooms[i]->level = i;
	}
	else
	  continue;
      }
      /* default level 0 and 1 rooms */
      if (i == 0)
	pci->crooms[i]->vnum	= ROOM_VNUM_CROOM_DUMMY;
      else if (i == 1)
	pci->crooms[i]->vnum	= ROOM_VNUM_CROOM_GARDEN;

      if ( pci->crooms[i]->vnum < 1){
	nlogf("cabal.c>fix_cabals:croom with 0 room vnum in cabal %d level %d",
	      pci->vnum,
	      i);
	continue;
      }
      else if ( (pci->crooms[i]->pRoomIndex = get_room_index(pci->crooms[i]->vnum)) == NULL){
	nlogf("cabal.c>fix_cabals: could not find room vnum %d for croom level %d in cabal %d.",
	      pci->crooms[i]->vnum,
	      i,
	      pci->vnum
	      );
	continue;
      }
    }
  }
/* FIX CABALS */
  for (pCab = cabal_list ;pCab != NULL; pCab = pCab->next){

    if (pCab->pIndexData->parent_vnum){
      pCab->parent = get_cabal_vnum( pCab->pIndexData->parent_vnum);
      if (pCab->parent == NULL)
	bug("cabal.c>fix_cabals: could not load parent vnum %d.", pCab->pIndexData->parent_vnum);
    }
    if (pCab->parent){
      SET_BIT(pCab->flags, CABAL_SUBCABAL);
    }
    else
      REMOVE_BIT(pCab->flags, CABAL_SUBCABAL);

    //enemy and enemy vendettas
    if (!IS_NULLSTR(pCab->pIndexData->enemy_name)){
      CABAL_DATA* enemy = get_cabal( pCab->pIndexData->enemy_name );

      if (enemy == NULL){
	bug("cabal.c>fix_cabals: could not load enemy for cabal %d", pCab->pIndexData->vnum );
      }
      else{
	PACT_DATA* pact;
	pCab->enemy = enemy;
	/* check for vendetta between these two */
	if ( pCab->parent == NULL){
	  if ( (pact = get_pact(pCab, enemy, PACT_VENDETTA, FALSE)) == NULL){
	    if ( (pact = create_pact( pCab, enemy, PACT_VENDETTA )) != NULL){
	      pact->complete = PACT_COMPLETE;
	      add_pact(pact);
	    }
	  }
	  else if (pact->complete != PACT_COMPLETE){
	    pact->complete = PACT_COMPLETE;
	  }
	}
      }
    }

    // adjust the max elder / leader
    pCab->max_elder = 1 + pCab->cur_member / 5;
    pCab->max_leader = 2 + pCab->cur_member / 25;

// cabal member data
    for (cMem = pCab->members; cMem; cMem = cMem->next){
      // cabal
      if (cMem->vnum){
	CABAL_DATA* pCabal = get_cabal_vnum( cMem->vnum);
	if (pCabal == NULL){
	  bug("fix_cabals (member): could not find cabal vnum: %d.", cMem->vnum);
	  continue;
	}
	else{
	  cMem->pCabal = pCabal;
	}
      }
    }//END for members

//CITY pointer for ROYALS
    if (IS_CABAL(pCab, CABAL_ROYAL) && pCab->city == NULL){
      bug("fix_cabal: ROYAL cabal with no city! (vnum %d)", pCab->vnum );
    }

// ANCHOR room
    if (pCab->anchor == NULL){
      if (pCab->pIndexData->parent_vnum == 0)
	bug("fix_cabals: Cabal vnum %d has NULL anchor.", pCab->vnum );
    }
    else if (pCab->parent == NULL){
      area_to_cabal( pCab->anchor->area, pCab, TRUE );
      pCab->anchor->area->bastion_max = 0;
      pCab->anchor->area->age = 31;//force a repop right away
      if (IS_CABAL(pCab, CABAL_JUSTICE))
	SET_BIT(pCab->anchor->area->area_flags, AREA_LAWFUL);
      SET_BIT(pCab->anchor->area->area_flags, AREA_CABAL);
    }

// GUARD
    if (pCab->guard && pCab->anchor == NULL){
      bug("fix_cabals: WARNING: Cabal guard set with no anchor room in cabal vnum %d.", pCab->vnum );
    }

// VCROOM DATA
    if (pCab->cvrooms && pCab->anchor == NULL){
      bug("fix_cabals: Cabal with virtual rooms and no anchor detected! vnum %d", pCab->vnum);
      continue;
    }
    else{
      RESET_DATA* pReset;
      MOB_INDEX_DATA* temp_index_mob = NULL;
      OBJ_INDEX_DATA* temp_index;
      /* Useless conditional */
      if( temp_index_mob != NULL )
        temp_index_mob = NULL;

      for (pcv = pCab->cvrooms; pcv; pcv = pcv->next ){
	// we update all vrooms with area
	if (pcv->pRoom == NULL){
	  bug("fix_cabals: vcroom with no room in cabal %d", pCab->vnum );
	  continue;
	}
	pcv->pRoom->area = pCab->anchor->area;
	pcv->pRoom->pCabal = pCab;

	// we only load the room/update resets if this is a complete room
	if (!pcv->fComplete){
	  continue;
	}

	//Load the actual room, this will take care of its vnum and resets
	load_vir_room( pcv->pRoom, pCab->anchor->area );

	// fix its flags now
	set_cvroom_flags( pcv, pCab, TRUE );

	// check over the resets
	for (pReset = pcv->pRoom->reset_first; pReset; pReset = pReset->next){
	  switch ( pReset->command ){
	  default:
	    bug( "fix_cabals: bad command '%c'.", pReset->command );
	    continue;
	    break;
	  case 'M':
	    temp_index_mob = get_mob_index  ( pReset->arg1 );
	    break;
	  case 'O':
	    temp_index = get_obj_index ( pReset->arg1 );
	    temp_index->reset_num++;
	  case 'P':
	    temp_index = get_obj_index ( pReset->arg1 );
	    temp_index->reset_num++;
	    break;
	  case 'G':
	  case 'E':
	    temp_index = get_obj_index  ( pReset->arg1 );
	    temp_index->reset_num++;
	    break;
	  case 'R':
	    if ( pReset->arg2 < 0 || pReset->arg2 > MAX_DIR ){
	      bug( "fix_cabals: 'R': bad exit %d.", pReset->arg2 );
	      continue;
	    }
	    break;
	  case 'T':
	    break;
	  }
	}
      }//end for pcvrooms

      // correct exits for rooms leading to other cvrooms
      for (pcv = pCab->cvrooms; pcv; pcv = pcv_next ){
	pcv_next = pcv->next;
	// we update all vrooms with area
	if (pcv->pRoom == NULL){
	  continue;
	}
	fix_vir_exits( pcv );
      }
    }
  }
}

void refresh_cabal_support(){
  CABAL_DATA* pCab;
 /* refresh support */
  for (pCab = cabal_list ;pCab != NULL; pCab = pCab->next){
    reinforce_refresh( pCab );
  }

}
/* loads cabal's virtual rooms */
void read_cvrooms( FILE* fp, CABAL_DATA* pCabal ){
  char letter;
  char* word;
  CVROOM_DATA* last_pcv = NULL;

  /* begin reading the cabal cvrooms */
  for (;;){
    letter		= fread_letter( fp );
    if ( letter != '#' ){
      bug( "read_cvrooms: # not found.", 0 );
      exit( 1 );
    }
    word   = feof( fp ) ? "End" : fread_word( fp );
    if ( !str_cmp(word, "End"))
      break;
    else if (!str_cmp(word, "O") || !str_cmp(word, "OBJECT")){
      if (last_pcv == NULL){
	bug("read_cvrooms: #O or #OBJECT encontered without previous cvroom.", 0);
	exit( 1 );
      }
      else{
	fread_obj( NULL, fp, last_pcv->pRoom );
	continue;
      }
    }
    else if (!str_cmp(word, "VIRROOM")){
      CVROOM_DATA* pcv = new_cvroom();
      /* read the cabal virtual room data */
      if (read_cvroom( fp, pcv )){
	last_pcv = pcv;
	/* hook up into the cabal */
	pcv->next = pCabal->cvrooms;
	pCabal->cvrooms = pcv;
      }
      /* the rooms are actualy loaded in, and reset in fix_cabals */
    }
    else{
      bug("read_cvrooms: Unknown token when reading for cabal %d", pCabal->vnum );
      exit( 1 );
    }
  }
}

/* loads virtual rooms for parent cabals */
void load_cvrooms(){
  CABAL_DATA* pCab;
  FILE* fp;
  char path[MIL];

  fclose( fpReserve );
  for (pCab = cabal_list; pCab; pCab = pCab->next){
    sprintf( path, "%s%s.vir", SAVECAB_DIR, capitalize(pCab->pIndexData->file_name));
    // load this cabals virtual rooms if any
    if ( ( fp = fopen( path, "r" ) ) != NULL ){
      read_cvrooms( fp, pCab );
      fclose( fp );
    }
  }
  fpReserve = fopen( NULL_FILE, "r" );
}

/* create cabals based on indexes, and load their files if any */
void load_cabals(){
  FILE* fpCabal;
  CABAL_INDEX_DATA* pci = cabal_index_list;
  CABAL_DATA* pCab;
  char path[MIL];


  fclose( fpReserve );
  for (; pci; pci = pci->next ){
// load cabal
    pCab = create_cabal( pci );
// get path to its save file
    sprintf( path, "%s%s.cab", SAVECAB_DIR, capitalize(pci->file_name));
// try to open the save file
    if ( ( fpCabal = fopen( path, "r" ) ) == NULL ){
      perror( path );
      continue;
    }
    nlogf("Loading cabal %s.", pci->file_name);
    fread_cabal( fpCabal, pCab );
    fclose( fpCabal );
  }
  fpReserve = fopen( NULL_FILE, "r" );
}

/* CABAL DATA FUNCTIONS (lookup etc) */

/* returns the parent of a cabal */
inline CABAL_DATA* get_parent( CABAL_DATA* pCab ){
  if (pCab == NULL || pCab->parent == NULL)
    return pCab;
  else
    return get_parent( pCab->parent );
}

/* returns pointer to the cabal based on name */
CABAL_DATA* get_cabal(const char *name){
  CABAL_DATA* pCab = cabal_list;
  for (; pCab; pCab = pCab->next){
    if (LOWER(name[0]) == LOWER(pCab->name[0]) && !str_cmp(name, pCab->name))
      return pCab;
  }
  return NULL;
}

/* returns pointer to cabal based on vnum */
CABAL_DATA* get_cabal_vnum(int vnum){
  CABAL_DATA* pCab = cabal_list;
  for (; pCab; pCab = pCab->next){
    if (pCab->vnum == vnum)
      return pCab;
  }
  return NULL;
}

/* low level function of is_cabal checks if pCab is a child of pCabal */
inline bool _is_cabal( CABAL_DATA* pCab, CABAL_DATA* pCabal ){
  if (pCab == NULL || pCabal == NULL)
    return FALSE;
  else if (pCab == pCabal)
    return TRUE;
  else if (pCab->parent)
    return _is_cabal( pCab->parent, pCabal );
  else
    return FALSE;
}
/* checks by vnum if pCab is a child of cabal with vnum */
bool is_cabal_vnum( CABAL_DATA* pCab, int vnum ){
  if (pCab == NULL)
    return FALSE;
  else if (pCab->vnum == vnum)
    return TRUE;
  else if (pCab->parent)
    return _is_cabal( pCab->parent, get_cabal_vnum( vnum ));
  else
    return FALSE;
}

/* checks if the characters is member of cabal (children inherit parent's cabal) */
bool is_cabal(CHAR_DATA* ch, char* const cabal ){
  if (ch->pCabal == NULL)
    return FALSE;
  return _is_cabal( ch->pCabal, get_cabal( cabal ) );
}

/* checks if pCab and pCabal are children of the same cabal */
bool is_same_cabal(CABAL_DATA* pCab, CABAL_DATA* pCabal ){
  if (pCab == NULL || pCabal == NULL)
    return FALSE;
  return (get_parent(pCab) == get_parent(pCabal));
}

/* returns cabal member data if it exists in the cabal */
CMEMBER_DATA* get_cmember(char* name, CABAL_DATA* cp ){
  CMEMBER_DATA* cm;

  if (cp == NULL)
    return NULL;

  for (cm = cp->members; cm; cm = cm->next){
    if (LOWER(name[0]) == LOWER(cm->name[0]) && !str_cmp(name,cm->name))
      return cm;
  }
  return NULL;
}


/* adds a member data to cabal */
void cmember_to_cabal(CMEMBER_DATA* cm, CABAL_DATA* cp ){

  if (cp->members == NULL)
    cp->members = cm;
  else{
    cm->next = cp->members;
    cp->members = cm;
  }
}

/* removes member data from cabal */
void cmember_from_cabal(CMEMBER_DATA* cm, CABAL_DATA* cp ){
  CMEMBER_DATA* pPrev = NULL;

  if (cm == NULL){
    bug("cmember_from_cabal: NULL members.", 0);
    return;
  }
  if (cp == NULL){
    bug("cmember_from_cabal: NULL cabal.", 0);
    return;
  }

  if (cp->members == cm ){
    cp->members = cm->next;

  }
  else{
    for (pPrev = cp->members; pPrev; pPrev = pPrev->next){
      if (pPrev->next == cm){
	pPrev->next = cm->next;
	break;
      }
    }
    if (pPrev == NULL){
      bug("cmember_from_cabal: member data not found.", 0);
      return;
    }
  }
  free_cmember( cm );
}

/* updats the cabal member data from character data */
void update_cmember( CHAR_DATA* ch ){

  if (IS_NPC(ch) || ch->pCabal == NULL){
    return;
  }
  else{
    if (ch->pcdata->member == NULL){
      ch->pcdata->member = add_cmember( ch, ch->pCabal );
    }
    else{
      ch->pcdata->member->connected = ch->desc ? ch->desc->connected == CON_PLAYING : FALSE;
      if (ch->pcdata->member->connected)
	ch->pcdata->member->last_login = mud_data.current_time;

      if (ch->pcdata->member->connected && ch->pcdata->member->cp_pen != 0){
	CP_GAIN( ch, ch->pcdata->member->cp_pen, TRUE );
	CHANGE_CABAL(get_parent(ch->pCabal));
	ch->pcdata->member->cp_pen  = 0;
      }
      if (ch->pcdata->member->garrison_changed){
	sendf( ch, "Garrison now at: <%d.%d>\n\r", GARR_NOR( ch),GARR_UPG(ch));
	ch->pcdata->member->garrison_changed = FALSE;
      }

      ch->pcdata->member->pCabal= ch->pCabal;
      ch->pcdata->member->cp	= GET_CP(ch);
      ch->pcdata->member->rank	= ch->pcdata->rank;
      ch->pcdata->member->level = ch->level;
      ch->pcdata->member->class = ch->class;
      ch->pcdata->member->race = ch->race;

      /* update the hours/mhours basedon last update */
      if (ch->timer <= 5  && ch->in_room->vnum != ROOM_VNUM_LIMBO){
	ch->pcdata->member->hours += (int) (mud_data.current_time - ch->pcdata->member->last_update);
	ch->pcdata->member->mhours += (int) (mud_data.current_time - ch->pcdata->member->last_update);
      }
      ch->pcdata->member->last_update = mud_data.current_time;
    }
  }
}

/* adds complete member data to a cabal */
CMEMBER_DATA* add_cmember( CHAR_DATA* ch, CABAL_DATA* pCab ){
  CMEMBER_DATA* cm = get_cmember( ch->name, pCab );

  if (pCab == NULL)
    return NULL;

  /* set the clan to cabal's clan */
  if (HAS_CLAN(ch) && pCab->clan && GET_CLAN(ch) != pCab->clan){
    CharFromClan( ch );
  }
  CharToClan( ch, pCab->clan );

  if (cm != NULL)
    return cm;
  else{
    cm = new_cmember();
    free_string( cm->name );
    cm->pCabal		= pCab;
    cm->name		= str_dup( ch->name );
    cm->class		= ch->class;
    cm->race		= ch->race;
    cm->level		= ch->level;
    cm->hours		= 0;
    cm->mhours		= 0;
    cm->month		= ch->pcdata->month;
    cm->cp		= GET_CP(ch);
    cm->rank		= ch->pcdata->rank;
    cm->connected	= ch->desc ? ch->desc->connected == CON_PLAYING : FALSE;

    cmember_to_cabal( cm, get_parent( pCab ) );
    ch->pcdata->member = cm;
    return cm;
  }
}

/* low level functions that adds/removes a character to cabal */
bool _char_to_cabal( CHAR_DATA* ch, CABAL_DATA* pCab ){

  pCab->cur_member  = UMIN(pCab->max_member, pCab->cur_member + 1);
  if (pCab->parent)
    _char_to_cabal( ch, pCab->parent );
  return TRUE;
}


bool char_to_cabal( CHAR_DATA* ch, CABAL_DATA* pCab ){
  if (pCab == NULL){
    bug("char_to_cabal: NULL cabal passed.", 0);
    return FALSE;
  }
  if (IS_NPC(ch)){
    bug("char_to_cabal: NPC passed.", 0);
    return FALSE;
  }
  if (ch->pCabal){
    char_from_cabal( ch );
  }

  if (_char_to_cabal( ch, pCab )){
    CABAL_DATA* pPar = get_parent(pCab);
    CMEMBER_DATA* cm = get_cmember( ch->name, pPar );

    ch->pcdata->rank = 0;
    ch->pCabal = pCab;

    /* either the member data is fresh new, or just updated */
    if (cm == NULL)
      ch->pcdata->member = add_cmember( ch, pCab );
    else{
      cm->pCabal	= pCab;
      cm->class		= ch->class;
      cm->race		= ch->race;
      cm->level		= ch->level;
      cm->month		= ch->pcdata->month;
      cm->cp		= ch->pcdata->cpoints;
      cm->rank		= ch->pcdata->rank;
      cm->connected	= ch->desc ? ch->desc->connected == CON_PLAYING : FALSE;
      ch->pcdata->member = cm;
    }

    if (!IS_CABAL(pCab, CABAL_CANHIDE))
      SET_BIT(ch->game, GAME_SHOW_CABAL);
    else
      REMOVE_BIT(ch->game, GAME_SHOW_CABAL);

    save_cabals( FALSE, NULL );
  }
  return TRUE;
}

/* low level function */
bool _char_from_cabal( CHAR_DATA* ch, CABAL_DATA* pCabal ){

  pCabal->cur_member  = UMAX(0, pCabal->cur_member - 1);
  if (ch->pcdata->rank == RANK_LEADER)
    pCabal->cur_leader  = UMAX(0, pCabal->cur_leader - 1);
  if (ch->pcdata->rank == RANK_ELDER)
    pCabal->cur_elder  = UMAX(0, pCabal->cur_elder - 1);

  if (pCabal->parent)
    _char_from_cabal( ch, pCabal->parent );
  return TRUE;
}

bool char_from_cabal( CHAR_DATA* ch ){
  CMEMBER_DATA* cm;

  if (IS_NPC(ch)){
    bug("char_From_cabal: NPC passed.", 0);
    return FALSE;
  }

  if (ch->pCabal == NULL){
    bug("char_from_cabal: NULL cabal passed.", 0);
    return FALSE;
  }
  /* clear sponsor if any */
  if (  (cm = sponsor_check(get_parent(ch->pCabal), ch->name )) != NULL){
    char buf[MIL];
    sprintf( buf, "Your obligations as a sponsor of %s have ended.\n\r", ch->name);
    do_hal( cm->name, "End of Sponsorship.", buf, NOTE_NEWS);
    free_string(cm->sponsored);
    cm->sponsored = str_dup( "" );
  }

  if (_char_from_cabal( ch, ch->pCabal ) ){
    CABAL_DATA* pPar = get_parent(ch->pCabal);
    CMEMBER_DATA* cm = get_cmember( ch->name, pPar );

    if (cm){
      cmember_from_cabal( cm, pPar );
      ch->pcdata->member = NULL;
    }
    ch->pcdata->rank = 0;
    ch->pCabal = NULL;
    REMOVE_BIT(ch->game, GAME_SHOW_CABAL);
    save_cabals( FALSE, NULL );
    return TRUE;
  }
  else
    return FALSE;
}

/* checks if person should be removed from cabal after extended leave */
bool purge_cabal(CMEMBER_DATA* cMem){
  struct tm *that_time;
  int month = 0;
  CABAL_DATA* pc = get_parent(cMem->pCabal);

  // Checks for hours played per month, if not enough, purge out of cabal
  that_time = gmtime(&mud_data.current_time);
  month = that_time->tm_mon;

  if (cMem->mhours < 0)
    cMem->mhours = 0;

  if(cMem->level <= 50 && cMem->month >= 0 && cMem->month != month ){
    DESCRIPTOR_DATA* d;
    CHAR_DATA* ch;
    bool fCon = cMem->connected;
    char buf[MIL];

    if ( cMem->mhours < 36000){	//10h per month minimum
      /* bring the player on */
      if (!fCon){
	if ( (d = bring(get_room_index( ROOM_VNUM_LIMBO ), cMem->name)) == NULL){
	  bug("purge_cabal: Could not find the applicant pfile.", 0);
	  cmember_from_cabal( cMem, pc );
	  return TRUE;
	}
	else{
	  ch = d->character;
	}
      }
      else if ( (ch = get_char( cMem->name )) == NULL)
	return FALSE;


      /* print the messges */
      sprintf( buf, "Due to lack of presence %s has been removed from %s.", ch->name, pc->who_name );
      log_string( buf );
      wiznet(log_buf,NULL,NULL,WIZ_LOGINS,0,get_trust(ch));
      cabal_echo( pc, buf );
      do_hal( pc->name, "Automatic expulsion.", buf, NOTE_NEWS);

      /* we set the messages for login since he was not online */
      SET_BIT(ch->pcdata->messages, MSG_CAB_TEXPEL);

      /* remove from cabal */
      ch->pcdata->last_app = mud_data.current_time + VOTE_DAY * 7;
      update_cabal_skills(ch, cMem->pCabal, TRUE, TRUE);
      if (ch->pCabal && ch->pCabal == cMem->pCabal){
	char_from_cabal( ch );
	save_char_obj( ch );
      }
      else
	cmember_from_cabal( cMem, get_parent( cMem->pCabal ) );

      if (!fCon)
	purge( ch );
      cMem->month = month;
      cMem->mhours = 0;
      return TRUE;
    }
    else{
      cMem->month = month;
      cMem->mhours = 0;
      return FALSE;
    }
  }
  return FALSE;
}


/* raises or lowers a rank within cabal as per "gain", returns new rank */
int promote( CHAR_DATA* ch, CABAL_DATA* pc, int gain ){
  bool was_leader = ch->pcdata->rank == RANK_LEADER;
  bool was_elder  = ch->pcdata->rank == RANK_ELDER;

  if (IS_NPC(ch)){
    bug("promote: NPC passed.", 0);
    return FALSE;
  }

  ch->pcdata->rank = URANGE(0, ch->pcdata->rank + gain, RANK_LEADER);

  if (ch->pcdata->member != NULL){
    ch->pcdata->member->rank = ch->pcdata->rank;
  }
  if (was_leader && !IS_LEADER( ch ))
    pc->cur_leader = UMAX( 0, pc->cur_leader - 1);
  else if (!was_leader && IS_LEADER( ch ))
    pc->cur_leader = UMIN( pc->max_leader, pc->cur_leader + 1);
  if (was_elder && !IS_ELDER( ch ))
    pc->cur_elder = UMAX( 0, pc->cur_elder - 1);
  else if (!was_elder && IS_ELDER( ch ))
    pc->cur_elder = UMIN( pc->max_elder, pc->cur_elder + 1);

  /* adjust the hours required for promotion if this is a demotion*/
  if (gain < 0){
    ch->pcdata->member->hours = ch->pcdata->rank * 36000;
  }
  save_cabals( FALSE, NULL );
  return ch->pcdata->rank;
}

/* checks if a player can select a given cabal as a sub calab */
CABAL_DATA* can_select_child( CHAR_DATA* ch, char* child ){
  CABAL_DATA* pChild = get_cabal( child );

  if (pChild == NULL || ch->pCabal == NULL || IS_NPC( ch ))
    return NULL;
  else if (pChild->parent == NULL || !_is_cabal(pChild, ch->pCabal)
	   || ch->pcdata->rank < 1
	   || ch->pcdata->rank < pChild->pIndexData->min_rank)
    return NULL;
  else
    return pChild;
}

/* checks if the player can choose any cabal children based on his rank and his cabal */
bool has_child_choice( CABAL_DATA* pCab, int rank ){

  if (pCab == NULL || rank == 0){
    return FALSE;
  }
  else{
    /* we run through all cabals, and look for ones that have pCab as parent */
    CABAL_DATA* pc = cabal_list;

    for (; pc != NULL; pc = pc->next ){
      if (pc->parent && pc->parent == pCab && rank >= pc->pIndexData->min_rank)
	return TRUE;
    }
    return FALSE;
  }
}

/* shows possible child choices to a player */
void show_child_choice( CHAR_DATA* ch, CABAL_DATA* pCab, int rank ){

  if (!has_child_choice( pCab, rank )){
    send_to_char("You have no possible cabal choices.\n\r", ch);
    return;
  }
  else{
    /* we run through all cabals, and look for ones that have pCab as parent */
    CABAL_DATA* pc = cabal_list;
    BUFFER *buffer;
    char buf[MSL];


    buffer=new_buf();
    buf[0] = 0;

    for (; pc != NULL; pc = pc->next ){
      if (pc->parent && _is_cabal(pc, pCab) ){
	if (rank >= pc->pIndexData->min_rank)
	  sprintf( buf, "%-15s %-15s [Rank: %s]\n\r", pc->parent->who_name, pc->who_name, clanr_table[pc->pIndexData->min_rank][0]);
	else
	  sprintf( buf, "%-15s %-15s `8[Rank: %s]``\n\r", pc->parent->who_name, pc->who_name, clanr_table[pc->pIndexData->min_rank][0]);
	add_buf(buffer, buf );
      }
    }
    if (IS_NULLSTR(buf)){
      send_to_char("You have no possible cabal choices.\n\r", ch);
    }
    else{
      page_to_char(buf_string(buffer),ch);
    }
    free_buf(buffer);
  }
}


char* get_crank( CHAR_DATA* ch ){
  static char* null = "";

  if (IS_NPC(ch) || ch->pCabal == NULL)
    return null;
  else
    return get_crank_str( ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank );
}

/* returns a proper string for cabal rank */
inline char* get_crank_str( CABAL_DATA* pCabal, int sex, int rank ){
  return sex == SEX_FEMALE ? pCabal->pIndexData->franks[rank] : pCabal->pIndexData->mranks[rank];
}


/* sets a reward for all cabal membeers, applied when they log on */
void reward( CABAL_DATA* pCab, int amount ){
  CMEMBER_DATA* cMem;
  CABAL_DATA* pPar = get_parent( pCab );
  char buf[MIL];

  for (cMem = pPar->members; cMem; cMem = cMem->next ){
    if (_is_cabal(cMem->pCabal, pCab))
      cMem->cp_pen = amount;
  }
  if (pCab == pPar)
    CP_CAB_GAIN( pPar, amount);

  sprintf( buf, "A %s of %d %s%s has been applied to your cabal.",
	   amount < 0 ? "penalty" : "reward",
	   amount,
	   pCab->currency, amount == 1 ? "" : "s");
  cabal_echo( pCab, buf );
}

/* IMMortal cabal commands */
void do_imm_cabal( CHAR_DATA *ch, char *argument ){
  CABAL_DATA* pCab;
  CHAR_DATA *victim;
  char arg1[MIL];

  argument = one_argument( argument, arg1 );
  if (IS_NULLSTR(arg1) || IS_NULLSTR(argument)){
    send_to_char( "Syntax:\n\r"\
		  "cabal info <cabal>\n\r"\
		  "cabal rooms <cabal>\n\r"\
		  "cabal penalty <cabal> <amount>\n\r"\
		  "cabal reward <cabal> <amount>\n\r"\
		  "cabal coffers <cabal> <amount>\n\r"\
		  "cabal <char> <cabal name>\n\r"\
		  "cabal <char> <promote/demote>\n\r", ch);
    return;
  }
  /* INFO */
  if (!str_prefix(arg1, "info")){
    if (IS_NULLSTR(argument)){
      send_to_char("Show info for which cabal?\n\r", ch);
      return;
    }
    else if ( (pCab = get_parent(get_cabal( argument ))) == NULL){
      send_to_char("No such cabal.\n\r", ch);
      return;
    }
    else
      show_cabal_info( ch, pCab );
    return;
  }
  /* ROOMS */
  if (!str_prefix(arg1, "rooms")){
    if (IS_NULLSTR(argument)){
      send_to_char("Show rooms for which cabal?\n\r", ch);
      return;
    }
    else if ( (pCab = get_parent(get_cabal( argument ))) == NULL){
      send_to_char("No such cabal.\n\r", ch);
      return;
    }
    else
      show_cabal_rooms( ch, pCab );
    return;
  }
/* PENALTY */
  else if ( !str_prefix( arg1, "penalty")){
    char cabal[MIL];
    int amount;
    argument = one_argument( argument, cabal );

    if ( (pCab = get_cabal( cabal )) == NULL){
      send_to_char("That cabal does not exists.\n\r", ch);
      return;
    }
    else if( (amount = atoi( argument )) < 1){
      send_to_char("Penalty must be non zero, and positive.\n\r", ch );
      return;
    }
    else{
      char buf[MIL];
      reward( pCab, -amount );

      sprintf( buf, "%s has applied a penalty of %d to %s.", ch->name, amount, pCab->who_name );
      do_hal( "Immortal", "Cabal Penalty", buf, NOTE_PENALTY );
      sendf( ch, "You have applied a penalty of %d to all members of %s.\n\r", amount, pCab->who_name );
      return;
    }
  }
/* REWARD */
  else if ( !str_prefix( arg1, "reward")){
    char cabal[MIL];
    int amount;
    argument = one_argument( argument, cabal );

    if ( (pCab = get_cabal( cabal )) == NULL){
      send_to_char("That cabal does not exists.\n\r", ch);
      return;
    }
    else if( (amount = atoi( argument )) < 1){
      send_to_char("Reward must be non zero, and positive.\n\r", ch );
      return;
    }
    else{
      char buf[MIL];
      reward( pCab, amount );

      sprintf( buf, "%s has applied a reward of %d to %s.", ch->name, amount, pCab->who_name );
      do_hal( "Immortal", "Cabal Reward", buf, NOTE_PENALTY );
      sendf( ch, "You have applied a reward of %d to all members of %s.\n\r", amount, pCab->who_name );
      return;
    }
  }
/* MAP */
  else if (!str_prefix(arg1, "map")){
    char cabal[MIL];
    argument = one_argument( argument, cabal );
    if ( (pCab = get_cabal( cabal )) == NULL){
      send_to_char("That cabal does not exists.\n\r", ch);
      return;
    }
    show_cabal_map( ch, pCab, atoi(argument) );
    return;
  }

/* COFFERS */
  else if ( !str_prefix( arg1, "coffers")){
    char cabal[MIL];
    int amount;
    argument = one_argument( argument, cabal );

    if ( (pCab = get_cabal( cabal )) == NULL){
      send_to_char("That cabal does not exists.\n\r", ch);
      return;
    }
    else if( (amount = atoi( argument )) == 0){
      send_to_char("Amount must be non zero.\n\r", ch );
      return;
    }
    else if (get_trust(ch) < CREATOR){
      send_to_char("Only the Implementor can set coffers.\n\r", ch);
      return;
    }
    else{
      CP_CAB_GAIN(pCab, amount);
      return;
    }
  }
  if ( ( victim = get_char_world( ch, arg1 ) ) == NULL ){
    send_to_char( "They aren't playing.\n\r", ch );
    return;
  }
  if ( IS_NPC(victim) ){
    send_to_char( "Why would you want to cabal a mob?\n\r",ch);
    return;
  }
  if (!str_prefix(argument,"none")){
    if (victim->pCabal == NULL){
      send_to_char("They are not in any cabal.\n\r", ch);
      return;
    }
    if (get_trust( ch ) < CREATOR
	&& str_cmp(get_parent(victim->pCabal)->immortal, ch->name)){
      send_to_char("You are not in charge of that cabal.\n\r", ch);
      return;
    }
    sendf(ch, "%s is now caballess.\n\r", PERS2(victim));
    send_to_char("You are no longer in a cabal!\n\r",victim);
    update_cabal_skills(victim, ch->pCabal, TRUE, TRUE);
    char_from_cabal( victim );
    return;
  }


  if ( ( victim = get_char_world( ch, arg1 ) ) == NULL ){
    send_to_char( "They aren't playing.\n\r", ch );
    return;
  }
  if ( IS_NPC(victim) ){
    send_to_char( "Why would you want to cabal a mob?\n\r",ch);
    return;
  }
  if (!str_prefix(argument,"none")){
    if (victim->pCabal == NULL){
      send_to_char("They are not in any cabal.\n\r", ch);
      return;
    }
    if (get_trust( ch ) < CREATOR
	&& str_cmp(get_parent(victim->pCabal)->immortal, ch->name)){
      send_to_char("You are not in charge of that cabal.\n\r", ch);
      return;
    }
    sendf(ch, "%s is now caballess.\n\r", PERS2(victim));
    send_to_char("You are no longer in a cabal!\n\r",victim);
    update_cabal_skills(victim, victim->pCabal, TRUE, TRUE);
    char_from_cabal( victim );
    return;
  }
  else if (!str_prefix(argument,"promote")){
    CMEMBER_DATA* cm;
    if (victim->pCabal == NULL){
      send_to_char("They are not cabaled.\n\r", ch );
      return;
    }
    if (get_trust( ch ) < CREATOR
	&& str_cmp(get_parent(victim->pCabal)->immortal, ch->name)){
      send_to_char("You are not in charge of that cabal.\n\r", ch);
      return;
    }

    if (victim->pcdata->rank + 1 >= RANK_ELDER
	&& IS_IMMORTAL(victim)
	&& get_trust(ch) < CREATOR){
      send_to_char("Immortals are limited to non leadership positions.\n\r", ch );
      return;
    }
    if (victim->pcdata->rank + 1 == RANK_ELDER
	&& get_trust(ch) < CREATOR
	&& victim->pCabal->cur_elder + 1 > victim->pCabal->max_elder){
      send_to_char("The cabal cannot support another elder.\n\r", ch);
      return;
    }
    if (victim->pcdata->rank + 1 == RANK_LEADER
	&& get_trust(ch) < CREATOR
	&& victim->pCabal->cur_leader + 1 > victim->pCabal->max_leader){
      send_to_char("The cabal cannot support another leader.\n\r", ch);
      return;
    }

    promote( victim, victim->pCabal, 1 );

    /* clear sponsor if any */
    if (  victim->pcdata->rank >= 3
	  && (cm = sponsor_check(get_parent(victim->pCabal), victim->name )) != NULL){
      char buf[MIL];
      sprintf( buf, "Your obligations as a sponsor of %s have ended.\n\r", victim->name);
      do_hal( cm->name, "End of Sponsorship.", buf, NOTE_NEWS);
      free_string(cm->sponsored);
      cm->sponsored = str_dup( "" );
    }
    sendf( ch, "Promoted to rank of %s (%d)\n\r",
	   get_crank( victim ),
	   victim->pcdata->rank);
    sendf( victim, "You have been promoted to %s of %s.\n\r",
	   get_crank( victim ),
	   victim->pCabal->name);
    if (has_child_choice( victim->pCabal, victim->pcdata->rank )){
      send_to_char("`@You are now able to choose further within your organization. (\"cabal join\")``\n\r", victim);
    }
    return;
  }
  else if (!str_prefix(argument,"demote")){
    if (victim->pCabal == NULL){
      send_to_char("They are not cabaled.\n\r", ch );
      return;
    }
    if (get_trust( ch ) < CREATOR
	&& str_cmp(get_parent(victim->pCabal)->immortal, ch->name)){
      send_to_char("You are not in charge of that cabal.\n\r", ch);
      return;
    }
    promote( victim, victim->pCabal, -1 );
    sendf( ch, "Demoted to rank of %s (%d)\n\r",
	   get_crank( victim ),
	   victim->pcdata->rank);
    sendf( victim, "You have been demoted to %s of %s.\n\r",
	   get_crank( victim ),
	   victim->pCabal->name);
    return;
  }
  else{
    if ( (pCab = get_cabal(argument)) == NULL){
      send_to_char("No such cabal exists.\n\r",ch);
      return;
    }
    if (get_trust( ch ) < CREATOR ){
      send_to_char("Need Imp acess.\n\r", ch);
      return;
    }
    if (victim->pCabal){
      update_cabal_skills(victim, ch->pCabal, TRUE, TRUE);
      char_from_cabal( victim );
    }
    if (char_to_cabal( victim, pCab ) ){
      sendf(ch,"%s is now a member of cabal %s.\n\r", PERS2(victim), capitalize(victim->pCabal->name));
      sendf(victim,"You are now a member of cabal %s.\n\r", capitalize(victim->pCabal->name));
      update_cabal_skills(victim, victim->pCabal, FALSE, TRUE);

    }
  }

}

/* cabal commands */
void do_cabal( CHAR_DATA *ch, char *argument ){
  char arg1[MIL];


  if (IS_IMMORTAL( ch )){
    do_imm_cabal( ch, argument );
    return;
  }
  if (IS_NPC( ch ) || ch->pCabal == NULL){
    send_to_char("Huh?\n\r", ch);
    return;
  }
/* normal mortal cabal commands */
  if (IS_NULLSTR(argument)){
    send_to_char( "Syntax:\n\r"\
		  "sponsor <character>\n\r"\
		  "cabal info\n\r"\
		  "cabal info <cabal>\n\r"\
		  "cabal rooms\n\r"\
		  "cabal donate\n\r"\
		  "cabal map\n\r"\
		  "cabal promotion\n\r"
		  "cabal <join> <sub-cabal>\n\r"\
		  "cabal leave\n\r", ch);
    return;
  }
  argument = one_argument( argument, arg1 );
  if (!str_cmp(arg1, "join")){
    CABAL_DATA* pCab = NULL;

    if (IS_NULLSTR( argument )){
      show_child_choice( ch, ch->pCabal, ch->pcdata->rank);
      return;
    }
    else if ( (pCab = can_select_child( ch, argument)) == NULL){
      send_to_char("That is not a valid choice.\n\r", ch);
      return;
    }
    else if (!check_cabal_req( ch, pCab )){
      return;
    }
    else{
      char buf[MIL];

      ch->pCabal = pCab;
      update_cmember( ch );
      update_cabal_skills( ch, ch->pCabal, FALSE, TRUE );
      save_char_obj( ch );
      save_cabals( FALSE, NULL );

      sendf( ch, "You have joined the ranks of %s!\n\r", ch->pCabal->who_name);
      sprintf( buf, "%s has chosen to join the ranks of %s.", PERS2( ch ), ch->pCabal->who_name );
      cabal_echo( ch->pCabal, buf );

    }
  }
  else if (!str_cmp(arg1, "leave")){
    char buf[MIL];
    if (str_cmp(argument, "confirm")){
      send_to_char("THIS COMMAND WILL REMOVE YOU FROM THE CABAL!\n\rUse \"cabal leave confirm\" to proceed.\n\r", ch);
      return;
    }
    sprintf(buf, "%s has decided to quit our ranks.", PERS2(ch));
    cabal_echo(get_parent(ch->pCabal), buf );
    do_hal(get_parent(ch->pCabal)->name, buf, buf, NOTE_NEWS);

    CHANGE_CABAL( ch->pCabal );
    update_cabal_skills(ch, ch->pCabal, TRUE, TRUE);
    char_from_cabal( ch );
    ch->pcdata->last_app = mud_data.current_time + VOTE_DAY * 14;

    save_char_obj( ch );
    save_cabals( TRUE, NULL );
    return;
  }
  else if (!str_prefix(arg1, "map")){
    show_cabal_map( ch, ch->pCabal, -999 );
    return;
  }
  else if (!str_prefix(arg1, "promotion")){
    if (check_promo_hours( ch->pcdata->member,  ch->pcdata->rank )){
      sendf(ch, "You have been present enough to be promoted.\n\r");
    }
    else{
      int diff = (ch->pcdata->member->rank + 1) * 10 - (ch->pcdata->member->hours / 3600);
      sendf( ch, "%d more hours must pass before your cabal promotion.\n\r", diff);
    }
    check_promo_req( ch, ch->pcdata->member, ch->pcdata->member->rank + 1);
    return;
  }
  else if (!str_prefix(arg1, "donate")){
    CABAL_DATA* pCab = get_parent(ch->pCabal );
    int val = atoi(argument);
    char buf[MIL];

    if (val < 1){
      sendf(ch, "Donate how many %ss to the coffers?\n\r", ch->pCabal->currency);
      return;
    }
    else if (GET_CP(ch) < val ){
      sendf(ch, "You do not have that many %ss.\n\r", ch->pCabal->currency);
      return;
    }
    else if (GET_CAB_CP(pCab) + (val / 2) > 500){
      val = (500 - GET_CAB_CP(pCab)) * 2;
    }
    if (val < 1){
      send_to_char("The coffers cannot accept your donation.\n\r", ch );
      return;
    }
    CP_GAIN(ch, -val, TRUE );
    CP_CAB_GAIN(pCab, val / 2 );
    sprintf(buf, "As a result of %s's donation  the coffers are now at %d %ss.", PERS2(ch),
	    GET_CAB_CP(pCab), pCab->currency);
    cabal_echo(pCab, buf );
    return;
  }
  else if (!str_prefix(arg1, "sponsor")){
    CABAL_DATA* pCab = get_parent(ch->pCabal );
    CHAR_DATA* victim;

    if (!IS_CABAL(pCab, CABAL_SPONSOR)){
      send_to_char("Your organization does not use the sponsor system.\n\r", ch);
      return;
    }
    else if (IS_NULLSTR(argument)){
      send_to_char("Sponsor who?\n\r", ch );
      return;
    }
    else if (ch->pcdata->member == NULL){
      send_to_char("You are not a member of any cabal.\n\r", ch);
      return;
    }
    else if (!IS_NULLSTR(ch->pcdata->member->sponsored)
	     && get_cmember(ch->pcdata->member->sponsored, get_parent( ch->pCabal)) != NULL){
      sendf( ch, "You already have %s under your guidance.\n\r", ch->pcdata->member->sponsored);
      return;
    }

/* check for sponsorship and exsiting vote  */

    if (!IS_NULLSTR(ch->pcdata->member->sponsored)){
      VOTE_DATA* pv;
      for (pv = vote_list; pv; pv = pv->next ){
	if (pv->type == VOTE_CAB_APPLY && pv->value[0] == pCab->vnum && !str_cmp(pv->owner, ch->pcdata->member->sponsored)){
	  sendf(ch, "%s's application vote is still undecided!\n\r", ch->pcdata->member->sponsored);
	  return;
	}
      }
    }

    if (GET_CP(ch) < 50){
      sendf( ch, "You must have at least 50 %ss before you can sponsor.\n\r", ch->pCabal->currency);
      return;
    }
    else if (ch->pcdata->rank < 2){
      sendf(ch, "You must hold the rank of %s before you can sponsor.\n\r",
	    get_crank_str( ch->pCabal, ch->pcdata->true_sex, 2 ));
      return;
    }
    else if ( (victim = get_char_room( ch, NULL, argument)) == NULL){
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    else if (IS_NPC(victim)){
      send_to_char("Not on mobiles.\n\r", ch );
      return;
    }
    else if (victim == ch ){
      send_to_char("There is no point in sponsoring yourself.\n\r", ch );
      return;
    }
    else{
      if (!IS_NULLSTR(ch->pcdata->member->sponsored))
	free_string( ch->pcdata->member->sponsored );
      ch->pcdata->member->sponsored = str_dup( victim->name );
      sendf(ch, "You are now sponsoring %s.\n\r", PERS2( victim ));
      CP_GAIN(ch, -50, TRUE );
    }
  }
  else if (!str_prefix(arg1, "info")){
    CABAL_DATA* pCab = get_parent( ch->pCabal );
    if (IS_NULLSTR(argument)){
      /* sponsor data if any */
      if (!IS_NPC(ch) && ch->pcdata->member){
	if (!IS_NULLSTR(ch->pcdata->member->sponsor))
	  sendf(ch, "You are currently being sponsored by %s.\n\r", ch->pcdata->member->sponsor);
	if (!IS_NULLSTR(ch->pcdata->member->sponsored))
	  sendf(ch, "You are currently sponsoring %s.\n\r", ch->pcdata->member->sponsored);
      }

      /* show cabal info */
      show_cabal_info( ch, pCab );
    }
    else if (!IS_CABAL(pCab, CABAL_SPIES)){
      send_to_char("Your organization has not gained trust of the crime world.\n\r", ch);
      return;
    }
    else if ( (pCab = get_parent(get_cabal( argument ))) == NULL
	      || IS_CABAL(pCab, CABAL_ROUGE)){
      send_to_char("Your contacts lack any information on that subject.\n\r", ch );
      return;
    }
    else
      show_cabal_info( ch, pCab );
  }
  else if (!str_prefix(arg1, "rooms")){
    CABAL_DATA* pCab = get_parent( ch->pCabal );
    /* show cabal info */
    show_cabal_rooms( ch, pCab );
  }
  else
    do_cabal(ch, "");
}

/* echos a string to all cabal members with given cabal flag */
void cabal_echo_flag(int flag, char* string){
  DESCRIPTOR_DATA* d;
  char buf[MIL];

  for ( d = descriptor_list; d != NULL; d = d->next ){
    CHAR_DATA* rch = d->character;
    bool fDemon = FALSE;

    if ( d->connected != CON_PLAYING
	 || rch == NULL
	 || rch->pCabal == NULL
	 || IS_AFFECTED2(rch, AFF_SILENCE)
	 || IS_SET(rch->comm, COMM_NOCABAL)
	 || IS_SET(rch->comm, COMM_QUIET) ){
      continue;
    }
    fDemon = IS_TELEPATH( rch );

    if (IS_CABAL(get_parent(rch->pCabal), flag) ){
      sprintf(buf, "[%s]: '`0$t``'", rch->pCabal->who_name);
      act_new(buf, rch, string, NULL, TO_CHAR, POS_DEAD);

      if (!IS_NPC(rch) && rch->pcdata->eavesdropped != NULL
	  && is_affected(rch->pcdata->eavesdropped,gsn_eavesdrop)
	  && !fDemon){

	send_to_char("A faint message transmits from the parasite.\n\r",rch->pcdata->eavesdropped);
	act_new(buf, rch, string, NULL, TO_CHAR, POS_DEAD);
      }
    }
  }
}

/* echos a string to all cabal members of pCabal or all cabals that have "Flag" set */
void cabal_echo( CABAL_DATA* pCabal, char* string){
  DESCRIPTOR_DATA* d;
  char buf[MIL];

  sprintf(buf, "[%s]: '`0$t``'", pCabal->who_name);

  for ( d = descriptor_list; d != NULL; d = d->next ){
    CHAR_DATA* rch = d->character;
    bool fDemon = FALSE;

    if ( d->connected != CON_PLAYING
	 || rch == NULL
	 || rch->pCabal == NULL
	 || IS_AFFECTED2(rch, AFF_SILENCE)
	 || IS_SET(rch->comm, COMM_NOCABAL)
	 || IS_SET(rch->comm, COMM_QUIET)
	 || !is_same_cabal(rch->pCabal, pCabal) ){
      continue;
    }

    fDemon = IS_TELEPATH( rch );
    act_new(buf, rch, string, NULL, TO_CHAR, POS_DEAD);

    if (!IS_NPC(rch) && rch->pcdata->eavesdropped != NULL
	&& is_affected(rch->pcdata->eavesdropped,gsn_eavesdrop)
	&& !fDemon){

      send_to_char("A faint message transmits from the parasite.\n\r",rch->pcdata->eavesdropped);
      act_new(buf, rch, string, NULL, TO_CHAR, POS_DEAD);
    }
  }
}


/* sets or clears all cabal's skills and/or its parent's */
void update_cabal_skills( CHAR_DATA* ch, CABAL_DATA* pCabal, bool fRemove, bool fParent ){
  CSKILL_DATA* cSkill;
  CABAL_INDEX_DATA* pIndex;


  if (IS_NPC( ch ))
    return;

  if (pCabal == NULL)
    return;
  else if ( (pIndex = pCabal->pIndexData) == NULL)
    return;
  else if (pIndex->skills == NULL)
    return;
  else
    cSkill =  pIndex->skills;

  /* run through all cabal skills and set or remove them */
  for( cSkill = pIndex->skills; cSkill; cSkill = cSkill->next){
    if (fRemove
	&& (skill_table[cSkill->pSkill->sn].skill_level[ch->class] < 1
	    || skill_table[cSkill->pSkill->sn].skill_level[ch->class] > 50)
	){
      ch->pcdata->learned[cSkill->pSkill->sn] = 0;
    }
    else if ( ch->pcdata->learned[cSkill->pSkill->sn] < cSkill->pSkill->learned)
      ch->pcdata->learned[cSkill->pSkill->sn] = UMAX(2, cSkill->pSkill->learned);
  }
/* check if we move onto parent */
  if (fParent && pCabal->parent)
    update_cabal_skills( ch, pCabal->parent, fRemove, fParent);
}


/* finds a cabal skill in cabal data */
inline CSKILL_DATA *cskill_find(CSKILL_DATA *csk, int sn){
    CSKILL_DATA *csk_find;

    if (csk == NULL)
      return FALSE;

    for ( csk_find = csk; csk_find != NULL; csk_find = csk_find->next )
    {
        if ( csk_find->pSkill->sn == sn )
	return csk_find;
    }
    return NULL;
}

/* checks for a cabal skill in cabal's data */
inline CSKILL_DATA* get_cskill
( CABAL_DATA* pCab, int sn ){
  CSKILL_DATA* cSkill = NULL;

  if (pCab == NULL)
    return FALSE;
  if ( (cSkill = cskill_find( pCab->pIndexData->skills, sn )) != NULL){
    return cSkill;
  }
  else
    return get_cskill(pCab->parent, sn );
}

/* checks if someone in the cabal has sponsorted this character */
CMEMBER_DATA* sponsor_check( CABAL_DATA* pCab, char* name ){
  CMEMBER_DATA* cm;

  if (pCab == NULL || name == NULL)
    return NULL;

  for (cm = get_parent(pCab)->members; cm; cm = cm->next){
    if (is_name(name, cm->sponsored))
      return cm;
  }
  return NULL;
}

/* checks if a player can join a given cabal */
bool check_cabal_req( CHAR_DATA* ch, CABAL_DATA* pc ){
  int ethos = 0;
  int votes = 0;
  VOTE_DATA* pv;

  if (ch == NULL || pc == NULL )
    return FALSE;

  /* count application votes to this cabal */
  for (pv = vote_list; pv; pv = pv->next ){
    if (pv->type == VOTE_CAB_APPLY && pc->vnum == pv->value[0])
      votes++;
  }
  if ((pc->cur_member + votes) >= pc->max_member){
    sendf(ch, "You cannot join %s as it's at full capacity.\n\r", pc->who_name);
    return FALSE;
  }
  if (IS_CABAL(pc, CABAL_SPONSOR) && pc->cur_member >= 4 && !sponsor_check( pc, ch->name )){
    sendf(ch, "You cannot join %s as you have not been sponsored. (\"help sponsor\")\n\r", pc->who_name);
    return FALSE;
  }
  if (ch->level < pc->pIndexData->min_lvl){
    sendf(ch, "You cannot join %s as your rank is less then the requirement (%d - %d).\n\r", pc->who_name, pc->pIndexData->min_lvl, pc->pIndexData->max_lvl);
    return FALSE;
  }
  if (ch->level > pc->pIndexData->max_lvl){
    sendf(ch, "You cannot join %s as your rank is more then the requirement (%d - %d).\n\r", pc->who_name, pc->pIndexData->min_lvl, pc->pIndexData->max_lvl);
    return FALSE;
  }

  /* check align */
  if (IS_GOOD(ch) && !IS_SET(pc->align, CALIGN_GOOD)){
    sendf(ch, "You cannot join as Lightwalkers are not wanted in %s.\n\r", pc->who_name);
    return FALSE;
  }
  else if (IS_NEUTRAL(ch) && !IS_SET(pc->align, CALIGN_NEUTRAL)){
    sendf(ch, "Your application was rejected as Neutrals are not wanted in %s.\n\r", pc->who_name);
    return FALSE;
  }
  else if (IS_EVIL(ch) && !IS_SET(pc->align, CALIGN_EVIL)){
    sendf(ch, "Your cannot join as Evil is not wanted in %s.\n\r", pc->who_name);
    return FALSE;
  }

  //get ethos.
  switch (ch->pcdata->ethos)
    {
    case 0: ethos = DIETY_ETHOS_LAWFUL;break;
    case 1: ethos = DIETY_ETHOS_NEUTRAL;break;
    case 2: ethos = DIETY_ETHOS_CHAOTIC;break;
    }

  /* check ethos */
  if ( !IS_SET(ethos, pc->ethos)){
    sendf(ch, "You cannot join as your ethos is not wanted in %s.\n\r", pc->who_name);
    return FALSE;
  }

  /* check race */
  if (pc->race[0] >= 0
      && pc->race[ch->race] == FALSE){
    sendf(ch, "You cannot join as %ss are not wanted in %s.\n\r", race_table[ch->race].name, pc->who_name);
    return FALSE;
  }
  /* check avatar */
  if (!pc->fAvatar && IS_AVATAR( ch )){
    sendf(ch, "You cannot join as %ss are not wanted in %s.\n\r", "Avatar", pc->who_name);
    return FALSE;
  }

  /* check class */
  if (pc->class[0] >= 0
      && pc->class[ch->class] == FALSE){
    sendf(ch, "You cannot join as %ss are not wanted in %s.\n\r", class_table[ch->class].name, pc->who_name);
    return FALSE;
  }

  return TRUE;
}

/* check if given character has met all promotion requirements for this cabal*/
/* NULL ch for silent check */
bool check_promo_req( CHAR_DATA* ch, CMEMBER_DATA* cm, int rank ){
  CABAL_DATA* pCab;
  DESCRIPTOR_DATA* d;
  CHAR_DATA* victim = NULL;

  bool fPurge = FALSE;
  int sn = 0;
  int skills = 0;
  int spells = 0;

  if (cm == NULL){
    bug("check_promo_req: null cabal or member data.", 0);
    return FALSE;
  }
  else
    pCab = cm->pCabal;
  if (pCab->progress == 0)
    return TRUE;
  /* check if we need the character for anything */
  if (IS_SET(pCab->progress, PROGRESS_SKILL)
      || IS_SET(pCab->progress, PROGRESS_SPELL)
      || IS_SET(pCab->progress, PROGRESS_QUEST)){
    /* try to load the character */
    if ( (victim = char_file_active (cm->name)) == NULL){
      if ( (d = bring( get_room_index(ROOM_VNUM_LIMBO), cm->name )) != NULL){
	victim = d->character;
	fPurge = TRUE;
	victim->pcdata->roomnum *= -1;
      }
      else{
	bug("check_promo_req: character does not exist!", 0);
	return FALSE;
      }
    }
  }

  /* do the actual checks now */
  if (IS_SET(pCab->progress, PROGRESS_CP)){
    if (cm->cp < rank * 300){
      if (ch)
	sendf( ch, "[%s] requires %s to have %d %ss before being promoted.\n\r",
	       pCab->who_name,
	       cm->name,
	       rank * 300,
	       pCab->currency);
      if (fPurge)
	purge(victim);
      return FALSE;
    }
  }
  if (IS_SET(pCab->progress, PROGRESS_QUEST)){
    if (victim->pcdata->max_quests < (rank * 18 * mud_data.max_quest / 100)){
      if (ch)
	sendf( ch, "[%s] requires %s to have %d quests before being promoted.\n\r",
	       pCab->who_name,
	     cm->name,
	       rank * (18 * mud_data.max_quest / 100));
      if (fPurge)
	purge(victim);
      return FALSE;
    }
  }

  if (IS_SET(pCab->progress, PROGRESS_SKILL) || IS_SET(pCab->progress, PROGRESS_SPELL)){
    /* count skills/spells at this point */
    for ( sn = 0; sn < MAX_SKILL; sn++ ){
      SKILL_DATA* nsk = NULL;
      CSKILL_DATA* csk = NULL;
      sh_int learned = 0;

      if ( skill_table[sn].name == NULL )
	break;
      else if (skill_table[sn].skill_level[victim->class] > 50
	       && (nsk = nskill_find(victim->pcdata->newskills,sn)) == NULL
	       && (csk = get_cskill(get_parent(victim->pCabal), sn)) == NULL)
	continue;
      /* for cabal skills check starting level */
      if (csk && csk->pSkill->learned >= 100)
	continue;
      if (nsk != NULL)
	learned = nsk->learned;
      else
	learned = victim->pcdata->learned[sn];
      if (learned < 100)
	continue;
      if (skill_table[sn].spell_fun == spell_null)
	skills++;
      else
	spells++;
    }

    if (IS_SET(pCab->progress, PROGRESS_SKILL)){
      int cnt = IS_SET(pCab->progress, PROGRESS_SPELL) ? skills + spells : skills;
      if (cnt < rank * 3){
	if (ch)
	  sendf( ch, "[%s] requires %s to have %d skills%s mastered before being promoted. (Current: %d)\n\r",
		 pCab->who_name,
		 cm->name,
		 rank * 3,
		 IS_SET(pCab->progress, PROGRESS_SPELL) ? " or spells" : "",
		 cnt);
	if (fPurge)
	  purge(victim);
	return FALSE;
      }
    }
    else if (IS_SET(pCab->progress, PROGRESS_SPELL)){
      int cnt = IS_SET(pCab->progress, PROGRESS_SKILL) ? skills + spells : spells;
      if (cnt < rank * 3){
	if (ch)
	  sendf( ch, "[%s] requires %s to have %d spells%s mastered before being promoted.(Current: %d)\n\r",
		 pCab->who_name,
		 cm->name,
		 rank * 3,
		 IS_SET(pCab->progress, PROGRESS_SKILL) ? " or skills" : "",
		 cnt);
	if (fPurge)
	  purge(victim);
	return FALSE;
      }
    }
  }
  if (IS_SET(pCab->progress, PROGRESS_CHALL)){
    int cnt = cm->kills;
    int req  = 10 * UMIN(2, rank);
    req += 20 * UMAX(0, rank - 2);

    if (cnt < req ){
      if (ch)
	sendf(ch, "[%s] requries %s to have %d more challenges.\n\r",
	      pCab->who_name,
	      cm->name,
	      req - cm->kills);
      if (fPurge)
	purge(victim);
      return FALSE;
    }
  }

  /* All passed, purge character if we need to */
  if (fPurge)
    purge(victim);
  return TRUE;
}

/* runs through all the areas connected to the cabal and marks their distance from the cabal */
void CalcAreaDistances( AREA_DATA* origin, int start_distance, int cabal ){
  EXIT_DATA* pe, *pe_back;

  //ez case
  if (origin->cabal_distance[cabal] <= start_distance)
    return;
  else
    origin->cabal_distance[cabal] = start_distance;

  for (pe = origin->exits; pe; pe = pe->next_in_area ){
    if (pe->to_room == NULL || pe->to_room->area == NULL || pe->to_room->area == origin)
      continue;
    else if (IS_AREA(pe->to_room->area, AREA_RESTRICTED))
      continue;
    else if (IS_AREA(pe->to_room->area, AREA_NOPATH))
      continue;

    //make sure there is an exit back
    for (pe_back = pe->to_room->area->exits; pe_back; pe_back = pe_back->next_in_area){
      if (pe_back->to_room == NULL || pe_back->to_room->area == NULL)
	continue;
      if (pe_back->to_room->area == origin)
	break;
    }
    if (pe_back == NULL)
      continue;

    CalcAreaDistances( pe->to_room->area, start_distance + 1, cabal );
  }
}

void CalcCabalDistances(){
  CABAL_DATA* pCab;

  for (pCab = cabal_list; pCab; pCab = pCab->next){
    if (pCab->parent != NULL)
      continue;
    else if (pCab->anchor == NULL)
      continue;
    CalcAreaDistances( pCab->anchor->area, 1, pCab->vnum );
  }
}


void AreaInfluenceUpdate( AREA_DATA* area ){
  CABAL_DATA* pCab;
  EXIT_DATA* pe, *pe_back;
  int i, delta;
  int count = 0, tot_inf = 0;

  int high = 0, sec_high = 0, high_cab = 0, sec_high_cab = 0;

  /* Useless conditional */
  if( sec_high_cab != 0 )
    sec_high_cab = 0;

  //ez cases for areas not meant to be taken over
  if (IS_AREA(area, AREA_RESTRICTED))
    return;
  else if (IS_AREA(area, AREA_NOPATH))
    return;
  else if (IS_AREA(area, AREA_CABAL))
    return;

  /* PASS 0:
     - Count number of non zero cabal influences
     - Count total influence/dist of all cabals

     PASS 1:
     - Run through area exits that have cabal owners and are unmarked
     - Add influence according to formula
     - Mark the exit's area

     if not held by armies
     PASS 2:
     - Find highest and second highest inluence
     - Change alliegence if highest is 20% > then second highest

  */

  /* PASS 0 */
  for (pe = area->exits; pe; pe = pe->next_in_area ){
    if (pe->to_room == NULL || pe->to_room->area == NULL || pe->to_room->area == area)
      continue;
    else if (IS_AREA(pe->to_room->area, AREA_RESTRICTED))
      continue;
    else if (IS_AREA(pe->to_room->area, AREA_NOPATH))
      continue;
    else if ( (pCab = pe->to_room->area->pCabal) == NULL)
      continue;
    else if (IS_AREA(pe->to_room->area, AREA_MARKED))
      continue;
    else if (area->cabal_distance[pCab->vnum] == 999)
      continue;

    //make sure there is an exit back
    for (pe_back = pe->to_room->area->exits; pe_back; pe_back = pe_back->next_in_area){
      if (pe_back->to_room == NULL || pe_back->to_room->area == NULL)
	continue;
      if (pe_back->to_room->area == area)
	break;
    }
    if (pe_back == NULL)
      continue;
    //make sure to not count the same area twice
    SET_BIT(pe->to_room->area->area_flags, AREA_MARKED);

    count++;
    tot_inf += UMAX(0, (pCab->support[ECO_GAIN] - CABAL_FULL_SUPPORT_VAL)) / UMAX(1, area->cabal_distance[pCab->vnum]);
  }

  if (count < 1)
    return;
  else
    tot_inf /= count;

  //PASS 1
  for (pe = area->exits; pe; pe = pe->next_in_area ){
    bool fMark = FALSE;

    if (pe->to_room == NULL || pe->to_room->area == NULL || pe->to_room->area == area)
      continue;

    //fault toleran MARK remove
    if (!IS_AREA(pe->to_room->area, AREA_MARKED))
      continue;
    else{
      fMark = TRUE;
      REMOVE_BIT(pe->to_room->area->area_flags, AREA_MARKED);
    }

    if (IS_AREA(pe->to_room->area, AREA_RESTRICTED))
      continue;
    else if (IS_AREA(pe->to_room->area, AREA_NOPATH))
      continue;
    else if ( (pCab = pe->to_room->area->pCabal) == NULL)
      continue;

    if (fMark){
      i = (pCab->support[ECO_GAIN] - CABAL_FULL_SUPPORT_VAL) / UMAX(1, area->cabal_distance[pCab->vnum]);
      delta = i - tot_inf + (i / count);
      area->cabal_influence[pCab->vnum] = URANGE(0, area->cabal_influence[pCab->vnum] + delta, 32767);

    }
  }

  /* check if the current cabal owner holds the area with armies */
  if (area->pCabal){
    CABAL_DATA* pc = get_parent( area->pCabal );
    TOWER_DATA* ptow;

/* check if we own majority */
    if ( (ptow = get_tower_data(pc, area)) != NULL
	 && (area->bastion_max - ptow->towers) < ptow->towers ){
      return;
    }
  }


  //Area not held by force, we check for influence
  /* PASS 2 */
  for (i = 0; i < MAX_CABAL; i++){
    if (area->cabal_influence[i] > high){
      sec_high = high;
      sec_high_cab = high_cab;
      high = area->cabal_influence[i];
      high_cab = i;
    }
  }

  //No positive influence at all
  if (high_cab < 1){
    area->pCabal = NULL;
  }
  //Enough influence change
  else if (high > (2 * CABAL_FULL_SUPPORT_VAL) && high > 12 * sec_high / 10 && (area->pCabal == NULL || area->pCabal->vnum != high_cab)){
    CABAL_DATA* pCab = get_cabal_vnum( high_cab );
    /* check if this cabal already gained an area this cabal tick */
    if (pCab && !IS_CABAL2(pCab, CABAL2_AREAGAIN)){
      area_to_cabal(area, pCab, FALSE );
      SET_BIT(pCab->flags2, CABAL2_AREAGAIN);
    }
    return;
  }

  return;
}
/* --------------================CABAL POINT RELATED ROUTINES=======----------- */


/* IMPORTANT */
/* IN ORDER TO SCALE THE CABAL POINTS, THE AMOUNT OF CURRENCY		*/
/* A CHAR HAS (CP's) IS OBTAINED BY DIVIDING CPOINTS BY CPTS		*/
/* FROM HERE ON, CP's ARE ACUAL CURRENCY, CPOINTS IS THE LONG INT USED	*/
/* TO STORE THE UNDIVIDED TOTAL						*/

/* Written by: Virigoth							*
 * Returns: amount of cp's						*
 * Used: cabal.c, act_obj.c						*
 * Comment: Ratio of cp's to currency returned is controled by CP_RATIO */

inline int GET_CP(CHAR_DATA* ch){

  if (!ch){
    bug("GET_CP: argument NULL.", 0);
    return 0;
  }
  if (IS_NPC(ch) || !ch->pCabal)
    return 0;

  return ch->pcdata->cpoints / CPTS;
}

/* Written by: Virigoth							*
 * Returns: amount of cp's for cabals					*
 * Used: cabal.c, act_obj.c						*
 * Comment: Ratio of cp's to currency returned is controled by CP_RATIO */

inline int GET_CAB_CP(CABAL_DATA* pc){
  CABAL_DATA* pCab = get_parent( pc );
  if (!pCab){
    bug("GET_CAB_CP: argument NULL.", 0);
    return 0;
  }
  return pCab->cp / CPTS;
}

/* adds or removes support from cabal, returns current income */
int SUPPORT_GAIN( CABAL_DATA* pCabal, int eco_sector, int gain ){
  CABAL_DATA* pCab = get_parent( pCabal );

  if (pCab == NULL)
    return 0;

  pCab->support[eco_sector] += gain;
  if (eco_sector != ECO_GAIN)
    pCab->support[ECO_GAIN] += gain;

  return     pCab->support[ECO_GAIN];
}


/* Written by: Virigoth							*
 * Returns: amount of cpoints's char has after update.			*
 * Used: cabal.c, act_obj.c						*
 * Comment: amount returned is curent amount of CP's, if fshow the gain	*
 * is signaled to player. CP_GAIN takes amount in currency, CPS_GAIN in *
 * cpoints								*/

int CPS_GAIN(CHAR_DATA* ch, int gain, bool fshow){

  if (!ch){
    bug("CPS_GAIN: ch argument NULL.", 0);
    return 0;
  }
  if (IS_NPC(ch) || !ch->pCabal || gain == 0)
    return 0;

  if (fshow){
    if (abs(gain)  < CPTS){//if not a whole point
      if (gain > 0)
	sendf(ch, "You have gained some worth in service of %s.\n\r",
	      ch->pCabal->who_name);
      else if (gain < 0)
	sendf(ch, "You have lost some worth in service of %s.\n\r",
	      ch->pCabal->who_name);
    }
    else{
      if (gain > 0)
	sendf(ch, "You have gained %d %s%s in service of %s.\n\r",
	      gain / CPTS,
	      ch->pCabal->currency,
	      (abs(gain) >= 2 * CPTS ? "s" : ""),
	      ch->pCabal->who_name);
      else if (gain < 0)
	sendf(ch, "You have lost %d %s%s in service of %s.\n\r",
	      abs(gain / CPTS),
	      ch->pCabal->currency,
	      (abs(gain) >= 2 * CPTS ? "s" : ""),
	      ch->pCabal->who_name);

    }
  }
  ch->pcdata->cpoints = UMAX(0, ch->pcdata->cpoints + gain);
  if (ch->pcdata->member)
    ch->pcdata->member->cp = GET_CP(ch);
  return GET_CP(ch);
}

int CP_GAIN(CHAR_DATA* ch, int gain, bool fshow){
  return CPS_GAIN(ch, gain * CPTS, fshow);
}

/* adds gain in cp */
int CP_CAB_GAIN(CABAL_DATA* pc, int gain ){
  return CPS_CAB_GAIN( pc, gain * CPTS );
}

/* adds gain in cps, returns in cp */
int CPS_CAB_GAIN(CABAL_DATA* pc, int gain ){
  CABAL_DATA* pCab = get_parent(pc );

  pCab->cp +=  gain;

/* set ceiling based on coffer status */
  if (IS_CABAL(pCab, CABAL_COFFER) && pCab->cp > ECONOMY_COFF_MAX)
    pCab->cp = ECONOMY_COFF_MAX;
  else if (!IS_CABAL(pCab, CABAL_COFFER) && pCab->cp > ECONOMY_NOCOFF_MAX){
    pCab->cp -= gain;
    return (pCab->cp);
  }
  else if (pCab->cp < ECONOMY_COFF_MIN )
    pCab->cp = ECONOMY_COFF_MIN;

  return GET_CAB_CP( pCab );
}


/* returns cp cost of a skill when used in this room */
inline int get_cskill_cost( CHAR_DATA* ch, ROOM_INDEX_DATA* room, CSKILL_DATA* cSkill ){

  if (room == NULL || cSkill == NULL)
    return 1000;
  /* check here if the area belongs to the cabal of the player */
  /*  normal cost for non-owner areas, 1/2 cost for friendly/allied */
  if (room->area->pCabal && is_friendly(ch->pCabal, room->area->pCabal) == CABAL_FRIEND)
    return cSkill->cost / 2;
  else
    return cSkill->cost;
}


/* this function handles subtraction of cps when power is used */
void handle_skill_cost( CHAR_DATA* ch, CABAL_DATA* pCabal, int sn ){
  CSKILL_DATA* pSkill = get_cskill( pCabal, sn );

  if (pSkill == NULL)
    return;
  else
    CP_GAIN( ch, -get_cskill_cost(ch, ch->in_room, pSkill), FALSE );
}

/* this function handles subtraction of cps but with %cent multiplier of
cost, 100 = normal
*/
void handle_skill_cost_percent( CHAR_DATA* ch, CABAL_DATA* pCabal, int sn, int percent ){
  CSKILL_DATA* pSkill = get_cskill( pCabal, sn );

  if (pSkill == NULL)
    return;
  else
    CP_GAIN( ch, percent * -get_cskill_cost(ch, ch->in_room, pSkill) / 100, FALSE );
}


/* Written by: Virigoth							*
 * Returns: VOID							*
 * Used: cabal.c,							*
 * comment: Distributes "gain" CPS to members of "cabal". If "fScale"	*
 * "ecabal" presence is used to scale the points.			*/

void distribute_cps(CHAR_DATA* ch, CABAL_DATA* pCab, int init_gain, bool fScale){
  CHAR_DATA* vch;
//  CMEMBER_DATA* cm;
  CABAL_DATA* pCabal = pCab;
  int tax = init_gain > 0 ? pCab->cp_tax * init_gain / 100 : init_gain;	//amount of cps moved to coffers
  int gain = tax > 0 ? init_gain - tax : init_gain;			//amount of gain given to char
  int scaled_gain = gain;						//amount of gained distributed to members

  if (pCab == NULL)
    return;
  else if (init_gain == 0 )
    return;
  else{
    pCabal = get_parent( pCab );
  }

  if (fScale){
    scaled_gain /= UMAX(1, pCabal->present);
  }

/* add tax to the cabal coffers */
  CPS_CAB_GAIN( pCabal, tax );

/* if ch is passed, then ch recives full gain, and rest players according to fScale */
  for ( vch = player_list; vch != NULL; vch = vch->next_player ){
    /* if link dead omit, not in cabal, or same ch omit */
    if (!IS_NPC(vch) && vch->desc
	&& vch->pCabal
	&& !IS_IMMORTAL(vch)
	&& is_same_cabal(pCabal, vch->pCabal)){
      if (ch && ch == vch)
	CPS_GAIN(vch, gain, TRUE);
      else
	CPS_GAIN(vch, scaled_gain, TRUE);
    }
  }
 /* now run through unpresent members and reward them with scaled_gain / total members */
  /* disabled Viri.
  for (cm = pCabal->members; cm; cm = cm->next){
    if (cm->connected == FALSE)
      cm->cp_pen += (scaled_gain / UMAX(1, pCabal->cur_member)) / CPTS;
  }
  */
}


/* Written by: Virigoth							*
 * Returns: amount of cps char recives on update.			*
 * Used: cabal.c, update.c						*
 * Comment: amount returned is in cps and used for scaling based on	*
 * number of cabal members on.						*/

int cp_calculate_timegain(CHAR_DATA* ch){
  int gain_mod = 100;   //base gain is 100% of basegain
  CABAL_DATA* pc = get_parent( ch->pCabal );

  /* safety */
  if (ch == NULL){
    bug("cp_calculate_gain: NULL argument passed.", 0);
    return 0;
  }
  /* safety checks */
  else if (IS_NPC(ch) || !ch->pCabal || !ch->desc || !ch->in_room || IS_IMMORTAL(ch))
    return 0;
  /* captured item results in 0 gain */
  if (is_captured(ch->pCabal))
    return 0;
  /* stance cancels time gain */
  else if (is_affected(ch, gsn_stance))
    return 0;
  /* abuse checks */
  else if (ch->timer >= 5  || ch->in_room->vnum == ROOM_VNUM_LIMBO)
    return 0;
  /* return the gain values in CPS */
  if (IS_CABAL(pc, CABAL_VENDETTA))
    gain_mod += 50;			//VENDETTA gives 50% boost to gain
  if (IS_CABAL(pc, CABAL_HIGHGAIN))	//Rooms boost gain by another 50%
    gain_mod += 50;
  //boost the cp gain by influence
  gain_mod = gain_mod * pc->support[ECO_GAIN] / CABAL_FULL_SUPPORT_VAL;

  return gain_mod * CP_BASEGAIN / 100;
}


/* cp rewards but for armies *
 * returns amount of cps used for reward
 */
int cp_event_army( ARMY_DATA* pa, CABAL_DATA* pEn, void* vo, int event ){
  CABAL_DATA* pc = get_parent(pa->pCabal);
  CMEMBER_DATA* cm;
  char buf[MSL];
  int gain;
  bool fTalk = FALSE;
  bool fScale = TRUE;

  if (pa == NULL){
    bug("cp_event_army: NULL army passed.", 0);
    return 0;
  }
  else if (event < CP_EVENT_MILITARY){
    bug("cp_event_army: Non-Military event (%d) passed.", event );
    return 0;
  }

  switch ( event ){
  default:
    bug("cp_Event_army: Unknown event. (%d)", event );
    return 0;
/* DESTRUCTION OF UNIT */
  case CP_EVENT_VICTORY:
    gain = CP_EVENT_VICTORY_COST;
    fScale = FALSE;
    fTalk = FALSE;
    if ( pEn )
      affect_cabal_relations(pc, pEn, -gain / 10, TRUE );
    break;
/* DESTRUCTION OF BASTION */
  case CP_EVENT_SIEGE:
    gain = CP_EVENT_SIEGE_COST;
    fScale = TRUE;
    fTalk = FALSE;
    if ( pEn )
      affect_cabal_relations(pc, pEn,  -gain / 10, TRUE );
    break;
/* CAPTURING NEUTRAL AREA */
  case CP_EVENT_AREA:
    gain = CP_EVENT_AREA_COST;
    fScale = TRUE;
    fTalk = TRUE;

    if (vo){
      AREA_DATA* area = (AREA_DATA*) vo;
      sprintf(buf, "Area secured: %s.", area->name );
    }
    else
      sprintf(buf, "Area secured.");
    break;
  case CP_EVENT_CONQUER:
    gain = CP_EVENT_CONQUER_COST;
    fScale = TRUE;
    fTalk = TRUE;

    if (vo && pEn){
      AREA_DATA* area = (AREA_DATA*) vo;
      /* Loser report */
      sprintf(buf, "Area Lost: %s taken by [%s].", area->name, pc->who_name );
      cabal_report( pEn, buf, REPORT_BAD, TRUE );

      /* winner report */
      sprintf(buf, "Area gained: %s taken from [%s].", area->name, pEn->who_name );
    }
    else
      fTalk = FALSE;
    affect_cabal_relations(pc, pEn, -gain / 10, TRUE );
    break;
  }

/* check if pEn and ch->pCabal has a vendetta, if so give +50% */
  if (gain > 0 && pEn
      && IS_CABAL(pc, CABAL_VENDETTA)
      && get_pact(pEn, pc, PACT_VENDETTA, FALSE) != NULL)
    gain = 3 * gain / 2;

  /* if there is army data, then we put the reward in owner's cp_pen stash */
  if (pa && (cm = get_cmember(pa->commander, pc)) != NULL
      && !cm->connected){
    cm->cp_pen += gain / CPTS;
  }

/* we announce this to the cabal of ch */
  if (fTalk && !IS_NULLSTR( buf )){
    cabal_report( pc, buf, REPORT_GOOD, TRUE );
  }
  /* scaled distribution if fscale */
  if (gain != 0)
    distribute_cps(NULL, pc, gain, fScale );

  return gain;
}

/* Written by: Virigoth							*
 * Returns: cp's gained							*
 * Used: cabal.c, act_obj.c, fight.c					*
 * comment: ch is player who performed event, victim/obj used as	*
 * neccesary, event is one of events in cabal.h				*
 * The cabal relations are affected roughly by 1/10 the reward for event */

int cp_event(CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj, int event){
  /* calclate events based on even pased */
  CABAL_DATA* pc = get_parent( ch->pCabal );
  CABAL_DATA* pEn;	//pointer to enemy cabal, used at the end to check for vendetta bonus
  char buf[MIL];

  int gain = 0;
  bool fTalk = FALSE;
  bool fScale = TRUE;

  buf[0] = '\0';

  /* Safety checks */
  if ((event >= CP_EVENT_OBJ) && (event < CP_EVENT_CHAR) && (!obj)){
    bug("cp_event: obj NULL on object event.", 0);
    return 0;
  }
  else if (event >= CP_EVENT_CHAR && event < CP_EVENT_MILITARY && !victim){
    bug("cp_event: victim NULL on victim event.", 0);
    return 0;
  }
  else if (IS_NPC(ch) && event < CP_EVENT_MILITARY){
    bug("cp_event: NPC passed.", 0);
    return 0;
  }
  else if (event >= CP_EVENT_MILITARY){
    bug("cp_event: Military event passed.", 0);
    return 0;
  }
  else if (!pc){
    bug("cp_event: Character not cabaled.", 0);
    return 0;
  }
  /* no bonus for killing self etc. */
  else if (ch == victim)
    return 0;
  else if (IS_IMMORTAL(ch) || IS_IMMORTAL(victim))
    return 0;

  /* assign the enemy cabal based on obj or victim */
  pEn = victim ? get_parent(victim->pCabal) : obj ? get_parent(obj->pCabal) : NULL;

  /* start case by case process */
  switch (event){
/* BOUNTY REWARD */
  case CP_EVENT_BOUNTY:
    gain = CP_EVENT_BOUNTY_COST;
    if (victim->pCabal && IS_LEADER(victim))
      gain += CP_EVENT_LEADER_COST;
    else if (victim->pCabal && IS_ELDER(victim))
      gain += CP_EVENT_ELDER_COST;
    else if (victim->pCabal)
      gain += CP_EVENT_PC_COST;
    sprintf(buf, "%s has claimed a bounty!", PERS2(ch));
    fScale = TRUE;
    fTalk = TRUE;
    break;
/* KILL PC */
  case CP_EVENT_PC:
    sprintf(buf, "%s has triumphed over %s!", PERS2(ch), PERS2(victim));
    if (IS_LEADER(victim))
      gain = CP_EVENT_LEADER_COST;
    else if (IS_ELDER(victim))
      gain = CP_EVENT_ELDER_COST;
    else
      gain = CP_EVENT_PC_COST;
    fTalk = TRUE;
    fScale = TRUE;

    /* if this is self defence of bountied person vs. rouge we do not do this. */
    if (victim->pcdata->pbounty != NULL && IS_CABAL(ch->pCabal, CABAL_ROUGE))
      break;
    if (ch->pcdata->pbounty != NULL && IS_CABAL(victim->pCabal, CABAL_ROUGE))
      break;

    if ( pEn ){
      bool fLaw = FALSE;
      if ((IS_WANTED(victim) || IS_OUTLAW(victim))
	  && (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_JUSTICE)) ){
	fLaw = TRUE;
      }
      /* check if we have a NA-PACT or an ALLIANCE */
      if (!fLaw && IS_CABAL( pc, CABAL_NAPACT) && get_pact(pEn, pc, PACT_NAPACT, FALSE) != NULL){
	char buf[MIL];
	sprintf( buf, "%s has violated the Non-Aggression Pact between %s and %s!", ch->name, pc->who_name, pEn->who_name );
	cabal_echo( pc, buf );
	cabal_echo( pEn, buf );
	reward( pc, -gain / CPTS );
	affect_cabal_relations(pc, pEn, -gain / CPTS, TRUE );
	fTalk = FALSE;

	/* if person is out of cps we demote them */
	/*we subtract the gain since reward takes effect on tick, not instantenously */
	if ((GET_CP( ch ) - gain/CPTS)  < 1 && ch->pcdata->rank > RANK_NEWBIE ){
	  char sub[MIL];
	  sprintf( buf, "Due to disregard for peace treaties, %s has been demoted.", PERS2(ch) );
	  cabal_echo( pc, buf );
	  strcat( buf, "\n\r");
	  sprintf( sub, "%s's demotion.", PERS2(ch));
	  do_hal( pc->name, sub, buf, NOTE_NEWS );
	  promote( ch, ch->pCabal, -1 );
	  if (ch->pcdata->member)
	    ch->pcdata->member->time_stamp = mud_data.current_time;
	}
	gain = 0;
      }
      else if (!fLaw && IS_CABAL( pc, CABAL_ALLIANCE) && get_pact(pEn, pc, PACT_ALLIANCE, FALSE) != NULL){
	char buf[MIL];
	sprintf( buf, "%s has violated the Alliance betwen %s and %s!", ch->name, pEn->who_name, pc->who_name );
	cabal_echo( pc, buf );
	cabal_echo( pEn, buf );
	reward( pc, -2* gain / CPTS );
	affect_cabal_relations(pc, pEn, -2 * gain / CPTS, TRUE );
	fTalk = FALSE;

	/* if person is out of cps we demote them */
	/*we subtract the gain since reward takes effect on tick, not instantenously */
	if ( (GET_CP( ch ) - gain/CPTS)  < 1 && ch->pcdata->rank > RANK_NEWBIE ){
	  char sub[MIL];
	  sprintf( buf, "Due to disregard for peace treaties, %s has been demoted.", PERS2(ch) );
	  cabal_echo( pc, buf );
	  strcat( buf, "\n\r");
	  sprintf( sub, "%s's demotion.", PERS2(ch));
	  do_hal( pc->name, sub, buf, NOTE_NEWS );
	  promote( ch, ch->pCabal, -1 );
	  if (ch->pcdata->member)
	    ch->pcdata->member->time_stamp = mud_data.current_time;
	}
	gain = 0;
      }
      else{
	affect_cabal_relations(pc, pEn, -4 * gain / CPTS, !fLaw );
      }
    }
    /* check of 1/2 gain for ROUGE killing cabal pcs since they also get bounties */
    if (IS_CABAL(pc, CABAL_ROUGE) && gain > 0)
      gain /= 3;
    break;
/* WANTED APPREHENSION */
  case CP_EVENT_CAPTURE:
    sprintf(buf, "%s has captured %s!", PERS2(ch), PERS2(victim));
    gain = (URANGE(1, victim->pcdata->flagged, 20)) * CP_EVENT_CAPTURE_COST / 10;
    fScale = TRUE;
    fTalk = TRUE;
    break;
/* WANTED FLAG CORRECT */
  case CP_EVENT_WANTED:
    gain = CP_EVENT_WANTED_COST;
    fScale = TRUE;
    fTalk = FALSE;
    if ( pEn )
      affect_cabal_relations(pc, pEn, -gain / (10 * CPTS), TRUE );
    break;
/* WANTED FLAG PENALTY (not correct) */
  case CP_EVENT_WANTED_FALSE:
    gain = (1 + ch->pcdata->rank) * CP_EVENT_WAN_PEN_COST;	//this is a negative value
    fScale = TRUE;
    fTalk = FALSE;
    break;
    if ( pEn )
      affect_cabal_relations(pc, pEn, gain / (10 * CPTS), TRUE );
    break;
  case CP_EVENT_STANDARD:
    gain = CP_EVENT_STANDARD_COST;
    fScale = TRUE;
    fTalk = FALSE;
    if ( pEn )
      affect_cabal_relations(pc, pEn, -gain / (10 * CPTS), TRUE );
    break;
  case CP_EVENT_RETURN:
    gain = CP_EVENT_RETURN_COST;
    fScale = TRUE;
    fTalk = FALSE;
    break;
  default:
    break;
  }


/* check if pEn and ch->pCabal has a vendetta, if so give +50% to the gains
*/
  if (gain > 0 && pEn && IS_CABAL(ch->pCabal, CABAL_VENDETTA) && get_pact(pEn, ch->pCabal, PACT_VENDETTA, FALSE) != NULL)
    gain = 3 * gain / 2;

  //give bonus penalty etc.
  pc->bonus_sup += gain / 60;
  if (pEn)
    pEn->bonus_sup -= gain / 60;

/* we announce this to the cabal of ch */
  if (fTalk && !IS_NULLSTR( buf )){
    cabal_echo(  ch->pCabal, buf );
  }
  /* scaled distribution if fscale */
  if (gain != 0)
    distribute_cps(ch, get_parent(ch->pCabal), gain, fScale );
  return gain;
}

/* ---------======== PACT ORIENTATED STUFF =======--------- */
/* write a pact to a file */
void save_pact( FILE* fp, PACT_DATA* pp ){
  char buf[MIL];
/* type, complete, rating, cre_vnu, bene_vnum, c_cp, c_sup, b_cp, b_sup, time_stamp */
  fprintf( fp, "#%d %s ", PACT_MARKER, fwrite_flag( pp->type, buf ) );
  fprintf( fp, "%s %d %d %d %d %d %d %d %d %ld\n",
	   fwrite_flag( pp->Adv, buf ),
	   pp->complete,
	   pp->rating,
	   pp->creator->vnum,
	   pp->benefactor->vnum,
	   pp->c_cp, pp->c_sup,
	   pp->b_cp, pp->b_sup,
	   pp->time_stamp );
}

bool read_pact( FILE* fp, PACT_DATA* pp ){
  int vnum = 0;
  int fSuccess = TRUE;

  pp->type		= fread_flag ( fp );
  pp->Adv		= fread_flag ( fp );
  pp->complete		= fread_number ( fp );
  pp->rating		= fread_number ( fp );

  vnum			= fread_number ( fp );
  if ( (pp->creator = get_cabal_vnum ( vnum )) == NULL){
    bug("read_pact: could not get creator cabal vnum %d", vnum );
    fSuccess = FALSE;
  }
  vnum			= fread_number ( fp );
  if ( (pp->benefactor = get_cabal_vnum ( vnum )) == NULL){
    bug("read_pact: could not get benefactor cabal vnum %d", vnum );
    fSuccess = FALSE;
  }

  pp->c_cp		= fread_number ( fp );
  pp->c_sup		= fread_number ( fp );
  pp->b_cp		= fread_number ( fp );
  pp->b_sup		= fread_number ( fp );
  pp->time_stamp	= fread_number ( fp );

  return fSuccess;
}

/* saves all pacts this MUST be written AFTER cabals */
void save_pacts( ){
  PACT_DATA* pp = pact_list;
  FILE* fp;
  char path[MIL];

  /* save the cabal pacts */
  fclose (fpReserve);
  sprintf( path, "%s%s", SAVECAB_DIR, PACT_FILE );
  if ( ( fp = fopen( path, "w" ) ) == NULL ){
    perror( path );
    fpReserve = fopen( NULL_FILE, "r" );
    fclose (fp);
  }

  for (; pp; pp = pp->next ){
    save_pact( fp, pp );
  }
/* terminate the file */
  fprintf( fp, "#0" );


  fclose (fp);
  fpReserve = fopen( NULL_FILE, "r" );

}

PACT_DATA *new_pact(void){

  PACT_DATA *pp = malloc( sizeof( *pp ));

/* reset initial things */
  pp->next		= NULL;
  pp->type		= PACT_TRADE;
  pp->complete		= PACT_NONE;
  pp->Adv		= 0;

  pp->creator		= NULL;
  pp->benefactor	= NULL;

  return pp;
};

void free_pact( PACT_DATA* pp ){

  free( pp );
}


/* creates a pact given a type, and two cabal vnums */
PACT_DATA* create_pact(CABAL_DATA* pCre, CABAL_DATA* pBen, int type ){
  PACT_DATA* pp = NULL;

  if ( pCre == NULL ){
    bug("create_pact: null creator", 0);
    return pp;
  }
  else if ( pBen == NULL){
    bug("create_pact: null benefactor.", 0);
    return pp;
  }
  else if ( (pp = new_pact()) == NULL){
    bug("create_pact: could not get a new pact.", 0);
    return pp;
  }

  pp->type		= type;
  pp->creator		= pCre;
  pp->benefactor	= pBen;
  pp->c_cp		= 0;
  pp->b_cp		= 0;
  pp->c_sup		= 0;
  pp->b_sup		= 0;
  pp->rating		= PACT_START_RATING;
  pp->complete		= PACT_NONE;
  pp->time_stamp	= mud_data.current_time;

  return pp;
}


PACT_DATA* create_pact_vnum( int cre, int ben, int type ){
  CABAL_DATA* pCre, *pBen;

  if ( (pCre = get_cabal_vnum( cre )) == NULL){
    bug("create_pact_vnum: could not get creator vnum %d.", cre );
    return NULL;
  }
  else if ( (pBen = get_cabal_vnum( ben )) == NULL){
    bug("create_pact_vnum: could not get benefactor vnum %d.", cre );
    return NULL;
  }
  else
    return create_pact( pCre, pBen, type );
}

/* sets proper pact flags based on pact type, if the vote is complete */
void set_pact_flags( PACT_DATA* pp, bool fRemove ){
  int flags = 0;
  if (pp->complete != PACT_COMPLETE)
    return;
  else if (IS_SET(pp->type, PACT_TRADE))
    flags |= CABAL_TRADE;
  else if (IS_SET(pp->type, PACT_NAPACT)){
    flags |= CABAL_NAPACT;
    /* check if one of the cabals was a rouge cabal, then we add the spy flag */
    if (pp->creator
	&& pp->benefactor
	&& (IS_CABAL(pp->creator, CABAL_ROUGE) || IS_CABAL(pp->benefactor, CABAL_ROUGE)) )
      flags |= CABAL_SPIES;
  }
  else if (IS_SET(pp->type, PACT_ALLIANCE)){
    flags |= CABAL_ALLIANCE;
    /* check if one of the cabals was a rouge cabal, then we add the spy flag */
    if (pp->creator
	&& pp->benefactor
	&& (IS_CABAL(pp->creator, CABAL_ROUGE) || IS_CABAL(pp->benefactor, CABAL_ROUGE)) )
      flags |= CABAL_SPIES;
  }
  else if (IS_SET(pp->type, PACT_VENDETTA))
    flags |= CABAL_VENDETTA;

  if (pp->creator)
    fRemove ? REMOVE_BIT(pp->creator->flags, flags) : SET_BIT(pp->creator->flags, flags);
  if (pp->benefactor)
    fRemove ? REMOVE_BIT(pp->benefactor->flags, flags) : SET_BIT(pp->benefactor->flags, flags);

  /* AREA flags */
  if (IS_SET(flags, CABAL_TRADE) || IS_SET(flags, CABAL_NAPACT) || IS_SET(flags, CABAL_ALLIANCE) ){
    if ( IS_CABAL(pp->creator, CABAL_ROYAL) && IS_CABAL(pp->benefactor, CABAL_JUSTICE) && pp->creator->city){
      fRemove ? REMOVE_BIT(pp->creator->city->area_flags, AREA_LAWFUL) : SET_BIT(pp->creator->city->area_flags, AREA_LAWFUL);
    }
    else if ( IS_CABAL(pp->creator, CABAL_JUSTICE) && IS_CABAL(pp->benefactor, CABAL_ROYAL) && pp->benefactor->city){
      fRemove ? REMOVE_BIT(pp->benefactor->city->area_flags, AREA_LAWFUL) : SET_BIT(pp->benefactor->city->area_flags, AREA_LAWFUL);
    }
  }
}

/* refreshes all pacts flags from all complete pacts */
void refresh_pact_flags(){
  PACT_DATA* pp = pact_list;

  for (; pp; pp = pp->next){
    if (pp->complete == PACT_COMPLETE)
      set_pact_flags( pp, FALSE );
  }
}

/* adds a pact onto the list of all pacts */
void add_pact( PACT_DATA* pp ){
  if (pact_list == NULL){
    pact_list = pp;
  }
  else{
    pp->next = pact_list;
    pact_list = pp;
  }

  /* set proper flags */
  if (pp->complete == PACT_COMPLETE)
    set_pact_flags( pp, FALSE );
}

/* removes a pact from the pact list */
bool rem_pact( PACT_DATA* pp ){
  PACT_DATA* prev = pact_list;
  if (pact_list == NULL){
    bug("rem_pact: pact_list null.", 0);
    return FALSE;
  }
  if (prev == pp )
    pact_list = pp->next;
  else{
    while (prev->next && prev->next != pp){
      prev = prev->next;
    }
  }
  if (prev == NULL){
    bug("rem_pact: pact not found.", 0);
    return FALSE;
  }
  prev->next = pp->next;
  pp->next = NULL;

  /* remove proper flags */
  if (pp->complete == PACT_COMPLETE){
    set_pact_flags( pp, TRUE );
    refresh_pact_flags();
  }
  return TRUE;
}


//reads the cabal index data from area file
void fread_pacts(){
  FILE* fp;
  char path[MIL];

  fclose( fpReserve );

// get path to pact file
  sprintf( path, "%s%s", SAVECAB_DIR, PACT_FILE );
  if ( ( fp = fopen( path, "r" ) ) == NULL ){
    perror( path );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }
  nlogf("Loading pacts." );

  for ( ; ; ){
    int vnum;
    char letter;
    letter                          = fread_letter( fp );
    if ( letter != '#' ){
      bug( "cabal.c>fread_pacts: # not found.", 0 );
      exit( 1 );
    }
    vnum = fread_number( fp );

    /* terminated by #0 */
    if ( vnum != PACT_MARKER)
      break;
    else
      load_pact( fp );
  }
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
}

/* reads and loads a valid pact */
void load_pact( FILE* fp ){
  PACT_DATA* pp = NULL;

  if ( (pp = new_pact()) == NULL){
    bug("load_pact: could not get new pact.", 0);
    return;
  }
  if (read_pact( fp, pp )){
    add_pact( pp );
  }
  else
    free_pact( pp );
}

void load_pacts(){
  fread_pacts();
}

/* returs COMPLETE pact */
/* returns a pact between two given cabals, and of given type, if fTrade then trade, na, and alliance are same type */
/* assumption is made that only ONE pact of given type exists */
PACT_DATA* get_pact( CABAL_DATA* pCabal1, CABAL_DATA* pCabal2, int type, bool fTrade ){
  PACT_DATA* pp;
  CABAL_DATA* pCab1 = get_parent( pCabal1 );
  CABAL_DATA* pCab2 = get_parent( pCabal2 );

  for (pp = pact_list; pp; pp = pp->next){
    if (pp->complete != PACT_COMPLETE)
      continue;
    if ( (pp->type == type
	  || (fTrade && pp->type >= PACT_TRADE &&  pp->type <= PACT_ALLIANCE))
	 &&  ( (pp->benefactor == pCab1 && pp->creator == pCab2)
	       ||  (pp->benefactor == pCab2 && pp->creator == pCab1)) ){
      return pp;
    }
  }
  return NULL;
}

/* returns ANY pact */
PACT_DATA* get_pact_abs( CABAL_DATA* pCabal1, CABAL_DATA* pCabal2, int type, bool fTrade ){
  PACT_DATA* pp;
  CABAL_DATA* pCab1 = get_parent( pCabal1 );
  CABAL_DATA* pCab2 = get_parent( pCabal2 );

  for (pp = pact_list; pp; pp = pp->next){
    if ( (pp->type == type
	  || (fTrade && pp->type >= PACT_TRADE &&  pp->type <= PACT_ALLIANCE))
	 &&  ( (pp->benefactor == pCab1 && pp->creator == pCab2)
	       ||  (pp->benefactor == pCab2 && pp->creator == pCab1)) ){
      return pp;
    }
  }
  return NULL;
}
/* returns actual cp or support gain based on both cabals in the pact */
long get_trade_bonus( PACT_DATA* pp, bool fCre, bool fCp ){
  int gain = fCp ? (fCre ? pp->c_cp : pp->b_cp) : (fCre ? pp->c_sup : pp->b_sup);
  long c_in = fCp ? pp->creator->cp_gain[ECO_INCOME] : pp->creator->support[ECO_INCOME];
  long b_in = fCp ? pp->benefactor->cp_gain[ECO_INCOME] : pp->benefactor->support[ECO_INCOME];

  if (c_in < 0) c_in = 0;
  if (b_in < 0) b_in = 0;

  if (pp->type == PACT_VENDETTA){
    return (long) fCre ? c_in * gain / 100 : b_in * gain / 100;
  }
  else
    return (long) fCre ? b_in * gain / 100 : c_in * gain / 100;
}


/* shows the status of pacts to character in given cabal, uses health bar for rating */
void show_pacts(CHAR_DATA* ch, CABAL_DATA* pCab ){
  PACT_DATA* pp = pact_list;

  for (; pp; pp = pp->next ){
    if (pp->complete == PACT_COMPLETE && (pp->creator == pCab || pp->benefactor == pCab)){
      char buf[MIL];
      if (pp->type != PACT_VENDETTA )
	health_prompt( buf, pp->rating, PACT_MAX_RATING, FALSE );
      else{
	/* we invert the prompt, the higher the rating, the lower the bar */
	health_prompt( buf, PACT_MAX_RATING - pp->rating, PACT_MAX_RATING, FALSE );
      }
      sendf( ch, "[%-15s] %-10s %-15s (%-4ldCp)\n\r",
	     pp->creator == pCab ? pp->benefactor->who_name : pp->creator->who_name,
	     buf,
	     flag_string( pact_flags, pp->type),
	     get_trade_bonus(pp, pp->creator == pCab ? TRUE : FALSE, TRUE) / CPTS

	     );
    }
  }
}

/* checks if cabal is hostile or friendly to a given character */
/* return values are:
CABAL_FRIEND
CABAL_NEUTRAL
CABAL_ENEMY
*/
int is_friendly( CABAL_DATA* pCabal, CABAL_DATA* pEnemy ){
  CABAL_DATA* pCab = NULL;
  CABAL_DATA* pEne = NULL;

  if (pEnemy == NULL || pCabal == NULL)
    return CABAL_NEUTRAL;

  pCab = get_parent( pCabal );
  pEne = get_parent (pEnemy );

  /* check for same cabal */
  if (_is_cabal( pEne, pCab ))
    return CABAL_FRIEND;
  else if (IS_CABAL( pCab, CABAL_ALLIANCE ) && IS_CABAL( pEne, CABAL_ALLIANCE )
	   && get_pact(pCab, pEne, PACT_ALLIANCE, FALSE))
    return CABAL_FRIEND;
  else if (IS_CABAL( pCab, CABAL_VENDETTA ) && IS_CABAL( pEne, CABAL_VENDETTA )
	   && get_pact(pCab, pEne, PACT_VENDETTA, FALSE))
    return CABAL_ENEMY;
  else
    return CABAL_NEUTRAL;
}

/* closes a cabal down, and removes anyone left inside */
void lock_cabal( CABAL_DATA* pCab, bool fON ){
  CHAR_DATA* vch, *vch_next;

  if (pCab->anchor == NULL){
    bug("lock_cabal: Cabal %d had no anchor.", pCab->vnum );
    return;
  }
  else if (!IS_CABAL(pCab, CABAL_GATE))
    return;

  pCab->fGate = fON;

  if (fON){
    /* run through players in the game, and look for ones inside the cabal */
    for (vch = player_list; vch; vch = vch_next ){
      vch_next = vch->next_player;

      if (!IS_IMMORTAL( vch )
	  && vch->in_room
	  && vch->in_room->area == pCab->anchor->area
	  && vch->in_room != pCab->anchor ){
	act_new("A pair of armed $t guards escort you to the entrance.", vch, pCab->who_name, NULL, TO_CHAR, POS_DEAD);
	act("A pair of armed $t guards escort $n to the entrance.", vch, pCab->who_name, NULL, TO_ROOM);
	char_from_room( vch );
	char_to_room( vch, pCab->anchor );
      }
    }
    if (pCab->anchor->people)
      act(pCab->pIndexData->gate_on, pCab->anchor->people, NULL, NULL, TO_ALL );
  }
  else{
    if (pCab->anchor->people)
      act(pCab->pIndexData->gate_off, pCab->anchor->people, NULL, NULL, TO_ALL );
  }
}


/* ------------============= ROOM BUILDING FUNCTIONS ==============------  */

/* returns croom data based on number, or name */
CROOM_DATA* get_croom( CABAL_DATA* pCabal, char* name, int number ){
  CABAL_DATA* pc = get_parent( pCabal );
  CROOM_DATA* pcr;
  int counter = 0;
  int i;

  if (pc == NULL)
    return NULL;

  for (i = 0; i < CROOM_LEVELS; i++){
    pcr = pc->pIndexData->crooms[i];
    if (pcr == NULL)
      continue;
    else
      counter++;
    if (number > 0 && counter == number)
      return pcr;
    else if (LOWER(name[0]) == LOWER(pcr->pRoomIndex->name[0]) && !str_prefix(name,pcr->pRoomIndex->name))
      return pcr;
  }
  return NULL;
}

/* returns virtual cabal room data (cvroom) based on room vnum and cabal */
CVROOM_DATA* get_cvroom_vnum( int vnum, CABAL_DATA* pCabal){
  CABAL_DATA* pc = get_parent( pCabal );
  CVROOM_DATA* pcv;

  if (vnum == 0 || pc == NULL)
    return NULL;

  for (pcv = pc->cvrooms; pcv; pcv = pcv->next ){
    if (pcv->pRoom && pcv->pRoom->vnum == vnum )
      return pcv;
  }
  return NULL;
}

/* returns virtual cabal room data (cvroom) based on its coordinates */
CVROOM_DATA* get_cvroom_xyz( int x, int y, int z, CABAL_DATA* pCabal, bool fComplete, bool fNotComplete){
  CABAL_DATA* pc = get_parent( pCabal );
  CVROOM_DATA* pcv;

  if (pc == NULL)
    return NULL;

  for (pcv = pc->cvrooms; pcv; pcv = pcv->next ){
    if (pcv->fComplete && !fComplete )
      continue;
    else if (!pcv->fComplete && !fNotComplete)
      continue;
    else if ( pcv->pos[P_X] == x && pcv->pos[P_Y] == y && pcv->pos[P_Z] == z )
      return pcv;
  }
  return NULL;
}

/* returns virtual cabal room data (cvroom) based on its skeleton's vnum and cabal */
CVROOM_DATA* get_cvroom_cvvnum( int vnum, CABAL_DATA* pCabal){
  CABAL_DATA* pc = get_parent( pCabal );
  CVROOM_DATA* pcv;

  if (vnum == 0 || pc == NULL)
    return NULL;

  for (pcv = pc->cvrooms; pcv; pcv = pcv->next ){
    if ( pcv->vnum == vnum )
      return pcv;
  }
  return NULL;
}

/* returns vritual cabal room data (cvroom) basedon room index past, null if not found */
CVROOM_DATA* get_cvroom( ROOM_INDEX_DATA* room, CABAL_DATA* pCabal){
  CABAL_DATA* pc = get_parent( pCabal );
  CVROOM_DATA* pcv;

  if (room == NULL || pc == NULL)
    return NULL;

  for (pcv = pc->cvrooms; pcv; pcv = pcv->next ){
    if (pcv->pRoom && pcv->pRoom == room )
      return pcv;
  }
  return NULL;
}

/* returns a vritual rooms 3d coordiantes, or 0,0,0 if anchor of cabal, false on error */
bool get_coordinates( int* pos, ROOM_INDEX_DATA* pRoom ){
  CVROOM_DATA* pcv;

  if (pRoom->pCabal == NULL)
    return FALSE;
  else if (pRoom->pCabal->anchor == NULL)
    return FALSE;
  else if (pRoom == pRoom->pCabal->anchor){
    pos[P_X] = 0;
    pos[P_Y] = 0;
    pos[P_Z] = 0;
    return TRUE;
  }
  else if ( (pcv = get_cvroom( pRoom, pRoom->pCabal)) != NULL){
    pos[P_X] = pcv->pos[P_X];
    pos[P_Y] = pcv->pos[P_Y];
    pos[P_Z] = pcv->pos[P_Z];
    return TRUE;
  }
  else
    return FALSE;
}


/* handles3d coordinates based on direction of travel from x/y/z in direction dir */
void get_new_coordinates( int* pos, int dir ){
  switch ( dir ){
  case DIR_EAST:	pos[P_X] += 1;	break;
  case DIR_WEST:	pos[P_X] -= 1;	break;
  case DIR_NORTH:	pos[P_Y] += 1;	break;
  case DIR_SOUTH:	pos[P_Y] -= 1;	break;
  case DIR_UP:		pos[P_Z] += 1;	break;
  case DIR_DOWN:	pos[P_Z] -= 1;	break;
  }
}

/* fixes a single cvroom's exits */
void fix_vir_exits( CVROOM_DATA* pcv ){
  EXIT_DATA* pExit;
  CVROOM_DATA* to_cvroom;
  int door = 0;
  int to_pos[3];
  bool fExit = FALSE;

  for (door = 0; door < MAX_DOOR; door++){
    if ( (pExit = pcv->pRoom->exit[door]) == NULL)
      continue;
    fExit = TRUE;
    /* check if this exit leads to another cvroom, if not it should be the anchor */
    if (!IS_VIRVNUM(pExit->vnum)){
      pExit->to_room = pcv->pRoom->pCabal->anchor;
      continue;
    }

    /* now we have an exit from cvroom to cvroom, we get that room by our position + exit direction */
    to_pos[P_X]	= pcv->pos[P_X];
    to_pos[P_Y]	= pcv->pos[P_Y];
    to_pos[P_Z]	= pcv->pos[P_Z];

    get_new_coordinates( to_pos, door );
    if ( (to_cvroom = get_cvroom_xyz(to_pos[P_X], to_pos[P_Y], to_pos[P_Z], pcv->pRoom->pCabal, TRUE, FALSE)) == NULL){
      char buf[MIL];
      sprintf( buf, "fix_vir_exits: cvroom (%d, %d, %d) exit %s lead nowhere.",
	       pcv->pos[P_X], pcv->pos[P_Y], pcv->pos[P_Z], dir_name[door]);
      bug( buf, 0);
      pExit->vnum = 0;
    }
    else{
      pExit->vnum = to_cvroom->pRoom->vnum;

      /* since uncomplete rooms never get loaded, we link the rooms oneway to prepare for later load */
      if (!pcv->fComplete)
	pExit->to_room = to_cvroom->pRoom;
    }
  }
/* cvrooms with no exits are automaticly unloaded */
  if (!fExit){
    CABAL_DATA* pc = pcv->pRoom->pCabal;
    char buf[MIL];

    sprintf( buf, "fix_vir_exits: %s had no valid exits, and was removed.", pcv->pRoom->name );
    bug( buf, 0);

    if (pcv->pRoom->vnum != 0)
      unload_vir_room( pcv->pRoom );
    cvroom_from_cabal( pcv, pc );
    free_cvroom( pcv );
  }
}

/* handles creation, testing, and assignment of exits */
void build_exits( CHAR_DATA* ch, CVROOM_DATA* pcvroom, ROOM_INDEX_DATA* in_room, char* argument ){
  ROOM_INDEX_DATA* pRoom = pcvroom ? pcvroom->pRoom : NULL;
  CVROOM_DATA* in_cvroom = get_cvroom( in_room, in_room->pCabal );
  char arg[MIL];
  char arg1[MIL];
  int door = 0;

   if (ch == NULL || pRoom == NULL || in_room == NULL)
    return;
  else if (IS_NULLSTR( argument )){
    send_to_char("exits <direction>        : New path with given direction FROM CURRENT room TO EDITED room.\n\r"\
		 "exits <direction> link   : Link room EDITED in given direction.\n\r"\
		 "exits <direction> delete : Delete a particular exit.\n\r"\
		 "exits <direction> desc   : Enter exit description.\n\r"\
		 "exits <direction> name   : Enter door name.\n\r"\
		 "exits <direction> closed : Toggle initial state of the door.\n\r"\
		 "exits <direction> door   : Create a door.\n\r"\
		 "exits <direction> nopass : Toggle unpassable door.\n\r"\
		 "exits <direction> secret : Set door as invisible when closed.\n\r", ch );
    return;
  }
  /* get the direction */
  argument = one_argument( argument, arg );
  one_argument( argument, arg1 );

  if ( (door = dir_lookup( arg )) < 0 || door >= MAX_DOOR){
    send_to_char("That is not a direction. (up, down, east, west, north, south)\n\r", ch);
    return;
  }
/* CREATE A NEW PATH */
  else if (IS_NULLSTR( argument )){
    int rev = rev_dir[door];
    EXIT_DATA* pExit;

    /* check if our current room already has a path in the opposite direction */
    if ( in_room->exit[door] != NULL ){
      sendf(ch, "%s has an exit leading %s.\n\r", in_room->name, dir_name[door]);
      return;
    }
    /* check the room we are building */
    else if ( pRoom->exit[rev] != NULL ){
      sendf(ch, "%s has an exit leading %s.\n\r", pRoom->name, dir_name[rev]);
      return;
    }
/* check if the new exit agrees with rooms coordinates */
    else if (pcvroom->pos[P_X] != 0 || pcvroom->pos[P_Y] != 0 || pcvroom->pos[P_Z] != 0){
      int to_pos[3];

      /*
	 the comparison is simple, we get the 3d cords of the room we are linking to,
	 then affect them by the direction of the new exit from the room we are linking to, towards
	 currently edited room.
	 If the adjusted coordinates do not match, or are already taken we cannot build the door
      */
/* debug
      sendf (ch, "%d %d %d\n\r", to_pos[P_X], to_pos[P_Y], to_pos[P_Z] );
      get_coordinates( to_pos, in_room );
      sendf (ch, "%d %d %d\n\r", to_pos[P_X], to_pos[P_Y], to_pos[P_Z] );
      get_new_coordinates( to_pos, door );
      sendf (ch, "%d %d %d\n\r", to_pos[P_X], to_pos[P_Y], to_pos[P_Z] );
*/
      get_coordinates( to_pos, in_room );
      get_new_coordinates( to_pos, door );

      if (to_pos[P_X] != pcvroom->pos[P_X]
	  || to_pos[P_Y] != pcvroom->pos[P_Y]
	  || to_pos[P_Z] != pcvroom->pos[P_Z]){
	sendf(ch, "A path %s from %s does not lead to %s.\n\r",
	      dir_name[door],
	      in_room->name,
	      pRoom->name);
	return;
      }
      if (!pcvroom->fComplete
	  && get_cvroom_xyz( to_pos[P_X], to_pos[P_Y], to_pos[P_Z], pcvroom->pRoom->pCabal, TRUE, TRUE ) != NULL){
	send_to_char("A room has already been built there, or awaits approval for that position.\n\r", ch);
	return;
      }
    }
    /* set the coordinates if not set yet */
    if (pcvroom->pos[P_X] == 0 && pcvroom->pos[P_Y] == 0 && pcvroom->pos[P_Z] == 0){
      /* either this is linked or origin (anchor, in_cvroom== NULL) or some other vroom */
      int to_pos[3];
      if (in_cvroom == NULL){
	to_pos[P_X] = 0;
	to_pos[P_Y] = 0;
	to_pos[P_Z] = 0;
	get_new_coordinates( to_pos, door );
      }
      else{
	to_pos[P_X]	=	in_cvroom->pos[P_X];
	to_pos[P_Y]	=	in_cvroom->pos[P_Y];
	to_pos[P_Z]	=	in_cvroom->pos[P_Z];
	get_new_coordinates( to_pos, door );
      }
      /* last safety check */
      if (get_cvroom_xyz( to_pos[P_X], to_pos[P_Y], to_pos[P_Z], pcvroom->pRoom->pCabal, TRUE, TRUE ) != NULL){
	send_to_char("A room has already been built there, or awaits approval for that position.\n\r", ch);
	return;
      }
      send_to_char("Room coordinates updated.\n\r", ch );
      pcvroom->pos[P_X] = to_pos[P_X];
      pcvroom->pos[P_Y] = to_pos[P_Y];
      pcvroom->pos[P_Z] = to_pos[P_Z];
    }
    /* now we know we can link the rooms */
    pExit                   = new_exit();
    pExit->to_room	      = in_room;
    pExit->vnum	      = in_room->vnum;
    pExit->orig_door	      = rev;
    pRoom->exit[rev]        = pExit;

    sendf(ch, "Exit from %s %s added.\n\rExit %s from %s will exist when room is built.\n\r",
	  pRoom->name, dir_name[rev], dir_name[door], in_room->name);
    return;
  }
  /* LINK */
  else if (!str_prefix(argument, "link")){
    int to_pos[3];
    int rev = rev_dir[door];
    CVROOM_DATA* to_cvroom;
    EXIT_DATA* pExit;

    /* check the room we are building */
    if ( pRoom->exit[door] != NULL ){
      sendf(ch, "%s has an exit leading %s.\n\r", pRoom->name, dir_name[door]);
      return;
    }

    /* get coordinates of the room we are linking to */
    to_pos[P_X] = pcvroom->pos[P_X];
    to_pos[P_Y] = pcvroom->pos[P_Y];
    to_pos[P_Z] = pcvroom->pos[P_Z];

    get_new_coordinates( to_pos, door );

    if ( (to_cvroom = get_cvroom_xyz( to_pos[P_X], to_pos[P_Y], to_pos[P_Z], pRoom->pCabal, TRUE, FALSE )) == NULL){
      send_to_char("There does not appear to be a room in that direction.\n\r", ch);
      return;
    }
    /* check if that room can have an exit in opposite location */
    else if ( to_cvroom->pRoom->exit[rev] != NULL ){
      sendf(ch, "%s has an exit leading %s.\n\r", to_cvroom->pRoom->name, dir_name[rev]);
      return;
    }
    /* we now create the exit from edited room towards the direction specified */
    pExit			= new_exit();
    pExit->to_room		= to_cvroom->pRoom;
    pExit->vnum			= to_cvroom->pRoom->vnum;
    pExit->orig_door		= door;
    pRoom->exit[door]		= pExit;

    sendf(ch, "Exit from %s %s added.\n\rExit %s from %s will exist when room is built.\n\r",
	  pRoom->name, dir_name[door], dir_name[rev], to_cvroom->pRoom->name);
    return;
  }
  /* DELETE */
  else if (!str_prefix(argument, "delete")){
    /* check if our current room already has a path in the opposite direction */
    if ( pRoom->exit[door] == NULL ){
      sendf(ch, "%s has no exit leading %s.\n\r", pRoom->name, dir_name[door]);
      return;
    }
    else{
      int i = 0;
      free_exit( pRoom->exit[door] );
      pRoom->exit[door] = NULL;
      send_to_char("ok.\n\r", ch);
      /* check if there are no other doors, if so we reset coordinates to  0 0 0 */
      for (i = 0; i < MAX_DOOR; i ++){
	if (pRoom->exit[i] != NULL)
	  return;
      }
      /* we do not reset coordiates for rooms being edited (complete), this way they have to stay in same "place" */
      if (pcvroom->fComplete)
	return;
      pcvroom->pos[P_X] = 0;
      pcvroom->pos[P_Y] = 0;
      pcvroom->pos[P_Z] = 0;
      return;
    }
  }
/* DOOR */
  else if (!str_prefix(argument, "door")){
    if ( pRoom->exit[door] == NULL ){
      sendf(ch, "%s has no exit leading %s.\n\r", pRoom->name, dir_name[door]);
      return;
    }
    else{
      TOGGLE_BIT(pRoom->exit[door]->rs_flags,  EX_ISDOOR);
      send_to_char("Door toggled.\n\r", ch);
      if (!IS_SET(pRoom->exit[door]->rs_flags,  EX_ISDOOR)){
	send_to_char("Removing other door flags.\n\r", ch);
	REMOVE_BIT(pRoom->exit[door]->rs_flags,  EX_NOPASS);
	REMOVE_BIT(pRoom->exit[door]->rs_flags,  EX_CLOSED);
	REMOVE_BIT(pRoom->exit[door]->rs_flags,  EX_SECRET);
      }
      return;
    }
  }
/* NOPASS */
  else if (!str_prefix(argument, "nopass")){
    if ( pRoom->exit[door] == NULL ){
      sendf(ch, "%s has no exit leading %s.\n\r", pRoom->name, dir_name[door]);
      return;
    }
    else if (!IS_SET(pRoom->exit[door]->rs_flags,  EX_ISDOOR)){
      send_to_char("You must first create a door there.\n\r", ch);
      return;
    }
    else{
      TOGGLE_BIT(pRoom->exit[door]->rs_flags,  EX_NOPASS);
      send_to_char("Nopass toggled.\n\r", ch);
      return;
    }
  }
/* SECRET */
  else if (!str_prefix(argument, "secret")){
    if ( pRoom->exit[door] == NULL ){
      sendf(ch, "%s has no exit leading %s.\n\r", pRoom->name, dir_name[door]);
      return;
    }
    else if (!IS_SET(pRoom->exit[door]->rs_flags,  EX_ISDOOR)){
      send_to_char("You must first create a door there.\n\r", ch);
      return;
    }
    else{
      TOGGLE_BIT(pRoom->exit[door]->rs_flags,  EX_SECRET);
      send_to_char("Secret toggled.\n\r", ch);
      return;
    }
  }
/* CLOSED */
  else if (!str_prefix(argument, "closed")){
    if ( pRoom->exit[door] == NULL ){
      sendf(ch, "%s has no exit leading %s.\n\r", pRoom->name, dir_name[door]);
      return;
    }
    else if (!IS_SET(pRoom->exit[door]->rs_flags,  EX_ISDOOR)){
      send_to_char("You must first create a door there.\n\r", ch);
      return;
    }
    else{
      TOGGLE_BIT(pRoom->exit[door]->rs_flags,  EX_CLOSED);
      send_to_char("Initial open/closed state toggled.\n\r", ch);
      return;
    }
  }
/* DESCRIPTION */
  else if (!str_prefix(argument, "description")){
    if ( pRoom->exit[door] == NULL ){
      sendf(ch, "%s has no exit leading %s.\n\r", pRoom->name, dir_name[door]);
      return;
    }
    else{
      string_append( ch, &pRoom->exit[door]->description );
      return;
    }
  }
/* NAME */
  else if (!str_prefix(arg1, "name")){
    argument = one_argument( argument, arg1 );
    if ( pRoom->exit[door] == NULL ){
      sendf(ch, "%s has no exit leading %s.\n\r", pRoom->name, dir_name[door]);
      return;
    }
    else{
      if (!IS_NULLSTR(pRoom->exit[door]->keyword))
	free_string(pRoom->exit[door]->keyword);
      pRoom->exit[door]->keyword = str_dup( argument );
      sendf( ch, "Exit name now: %s.\n\r", pRoom->exit[door]->keyword );
      return;
    }
  }
  else
    build_exits(ch, pcvroom, in_room, "");
}


/* creates a complete vote with info for the room being voted on */
VOTE_DATA* create_build_vote( CHAR_DATA* ch, CVROOM_DATA* pcv, int cost ){
  EXIT_DATA* pexit;
  VOTE_DATA* pv;

  char buf[MIL];
  char subj[MIL];
  char owner[MIL];
  char string[MSL];

  int door;
  bool found = FALSE;

  if (pcv == NULL || pcv->pRoom == NULL || pcv->pRoom->pCabal == NULL)
    return NULL;

  /* fill in the basics */
  sprintf( owner, "%s", pcv->pRoom->pCabal->who_name);
  sprintf( subj, "Proposed construction of %s", pcv->pRoom->name );

  /* begin filling in the text describing the room */
  sprintf( string, "%s\n\r\n\r", croom_table[pcv->level].desc );

  sprintf( buf, "Build Cost: %-3d Support: %-3d\n\r",
	   get_croom_bcost( pcv->level ),
	   get_croom_cost( pcv->level )
	   );
  strcat( string, buf);
  sprintf( buf, "\n\r\n\r" );
  strcat( string, buf );

  /* the day/night description */
  strcat( string, pcv->pRoom->name );
  strcat( string, "\n\r" );
  strcat( string, "DAY:\n\r" );
  strcat( string, pcv->pRoom->description);
  if (!IS_NULLSTR(pcv->pRoom->description2)){
    strcat( string, "\n\rNIGHT\n\r" );
    strcat( string, pcv->pRoom->description2);
  }
  strcat(string, "\n\r" );

  /* exits */
  strcat(string,"Exits:\n\r");
  for ( door = 0; door <= 5; door++ ){
    bool paran = FALSE;
    int pos[3];

    pos[P_X]	=	pcv->pos[P_X];
    pos[P_Y]	=	pcv->pos[P_Y];
    pos[P_Z]	=	pcv->pos[P_Z];

    get_new_coordinates( pos, door );
    if ( (pexit = ch->pcdata->pcvroom->pRoom->exit[door] ) == NULL
	 || pexit->to_room == NULL){
      CVROOM_DATA* adj_cvroom = get_cvroom_xyz(pos[P_X], pos[P_Y], pos[P_Z], ch->pcdata->pcvroom->pRoom->pCabal, TRUE, FALSE);
      if (IS_ORIGIN(ch->pcdata->pcvroom->pos))
	continue;
      else if (adj_cvroom){
	sprintf( buf, "%-5s - Adjacent room: %s.\n\r", capitalize( dir_name[door] ),
		 adj_cvroom->pRoom->name);
	strcat( string, buf );
      }
      else if (IS_ORIGIN(pos)){
	sprintf( buf, "%-5s - Adjacent room: %s.\n\r", capitalize( dir_name[door] ),
		 ch->pcdata->pcvroom->pRoom->pCabal->anchor->name);
	strcat( string, buf );
      }
      continue;
    }
    else{
      if (IS_SET(pexit->exit_info, EX_CLOSED))
	paran = TRUE;
      found = TRUE;

      if (paran)
	sprintf( buf, "(%-5s) - %s [%-15s]\n\r",
		 capitalize( dir_name[door] ),
		 pexit->to_room->name,
		 flag_string(exit_flags, pexit->rs_flags));
      else
	sprintf( buf, " %-5s  - %s [%-15s]\n\r",
		 capitalize( dir_name[door] ),
		 pexit->to_room->name,
		 flag_string(exit_flags, pexit->rs_flags));
      strcat( string, buf );
    }//end else
  }//end for
  if ( !found )
    strcat(string, "No exits.\n\r");
  /* got everything */
  if ( (pv = create_vote(ch, owner, subj, string, VOTE_BUILD,
			 pcv->pRoom->pCabal->vnum,
			 pcv->pos[P_X], pcv->pos[P_Y], pcv->pos[P_Z], cost)) == NULL){
    bug("create_build_vote: could not create a vote.", 0);
    return NULL;
  }
  else{
    add_vote( pv );
    save_mud();
    return pv;
  }

}

/* creates a complete vote with info for the room being edited and voted on */
VOTE_DATA* create_edit_vote( CHAR_DATA* ch, CVROOM_DATA* pcv, int cost ){
  EXIT_DATA* pexit;
  VOTE_DATA* pv;
  CVROOM_DATA* old;

  char buf[MIL];
  char subj[MIL];
  char owner[MIL];
  char string[MSL];

  int door;
  bool found = FALSE;

  if (pcv == NULL || pcv->pRoom == NULL || pcv->pRoom->pCabal == NULL)
    return NULL;
  else if ( (old = get_cvroom_xyz( pcv->pos[P_X], pcv->pos[P_Y], pcv->pos[P_Z], pcv->pRoom->pCabal, TRUE, FALSE)) == NULL){
    bug("create_edit_vote: could not get original room.", 0);
    return NULL;
  }

  /* fill in the basics */
  sprintf( owner, "%s", pcv->pRoom->pCabal->who_name);
  sprintf( subj, "Proposed renovation of %s", pcv->pRoom->name );

  /* begin filling in the text describing the room */
  sprintf( string, "%s\n\r\n\r", croom_table[pcv->level].desc );

  sprintf( buf, "Change in: Support: %-3d",
	   get_croom_cost(old->level) - get_croom_cost( pcv->level));
  strcat( string, buf);
  sprintf( buf, "\n\r\n\r" );
  strcat( string, buf );

  /* the day/night description */
  strcat( string, pcv->pRoom->name );
  strcat( string, "\n\r" );
  strcat( string, "DAY:\n\r" );
  strcat( string, pcv->pRoom->description);
  if (!IS_NULLSTR(pcv->pRoom->description2)){
    strcat( string, "\n\rNIGHT\n\r" );
    strcat( string, pcv->pRoom->description2);
  }
  strcat(string, "\n\r" );

  /* exits */
  strcat(string,"Exit Changes:\n\r");
  for ( door = 0; door <= 5; door++ ){
    bool paran = FALSE;

    if ( (pexit = pcv->pRoom->exit[door] ) == NULL
	 || pexit->to_room == NULL){
      if (old->pRoom->exit[door] != NULL && old->pRoom->exit[door]->to_room != NULL){
	sprintf( buf, "Exit to %s removed.\n\r", old->pRoom->exit[door]->to_room->name);
	strcat( string, buf );
      }
      continue;
    }
    else{
      bool fAdd = FALSE;
      if (IS_SET(pexit->exit_info, EX_CLOSED))
	paran = TRUE;
      found = TRUE;
      if (old->pRoom->exit[door] == NULL || old->pRoom->exit[door]->to_room ==  NULL)
	fAdd = TRUE;
      if (paran)
	sprintf( buf, "(%-5s) - %s [%-15s]%s\n\r",
		 capitalize( dir_name[door] ),
		 pexit->to_room->name,
		 flag_string(exit_flags, pexit->rs_flags),
		 fAdd ? " NEW" : "");
      else
	sprintf( buf, " %-5s  - %s [%-15s]%s\n\r",
		 capitalize( dir_name[door] ),
		 pexit->to_room->name,
		 flag_string(exit_flags, pexit->rs_flags),
		 fAdd ? " NEW" : "");
      strcat( string, buf );
    }//end else
  }//end for
  if ( !found )
    strcat(string, "No exits.\n\r");
/* at this point we make this room "incomplete", when the vote is complete the code wil check if it overlaps
   an existing room and deal with it appropriately
*/
  pcv->fComplete = FALSE;
  /* got everything */
  if ( (pv = create_vote(ch, owner, subj, string, VOTE_BUILD, pcv->pRoom->pCabal->vnum,
			 pcv->pos[P_X], pcv->pos[P_Y], pcv->pos[P_Z], cost)) == NULL){
    bug("create_build_vote: could not create a vote.", 0);
    return NULL;
  }
  else{
    add_vote( pv );
    save_mud();
    return pv;
  }
}

/* creates a complete vote with info for the room being voted on for sale*/
VOTE_DATA* create_sell_vote( CHAR_DATA* ch, CVROOM_DATA* pcv, int cost ){
  VOTE_DATA* pv;

  char buf[MIL];
  char subj[MIL];
  char owner[MIL];
  char string[MSL];

  if (pcv == NULL || pcv->pRoom == NULL || pcv->pRoom->pCabal == NULL)
    return NULL;


  /* fill in the basics */
  sprintf( owner, "%s", pcv->pRoom->pCabal->who_name);
  sprintf( subj, "Proposed SALE of %s", pcv->pRoom->name );

  /* begin filling in the text describing the room */
  sprintf( string, "%s\n\r\n\r", croom_table[pcv->level].desc );


  sprintf( buf, "Sale Profit: %-3d Support change: %-3d\n\r\n\r",
	   pcv->vnum == ROOM_VNUM_CROOM_DUMMY   ? 0 : get_croom_bcost(pcv->level) / 2,
	   get_croom_cost(pcv->level));
  strcat( string, buf);


  /* got everything */
  if ( (pv = create_vote(ch, owner, subj, string, VOTE_BUILD, pcv->pRoom->pCabal->vnum,
			 pcv->pos[P_X], pcv->pos[P_Y], pcv->pos[P_Z], cost)) == NULL){
    bug("create_build_vote: could not create a vote.", 0);
    return NULL;
  }
  /* check for existing vote */
  if (is_duplicated( pv )){
    free_vote( pv );
    return NULL;
  }
  else{
    add_vote( pv );
    save_mud();
    return pv;
  }
}

/* fills a supplied string with a representation of a map of cabal rooms */
/* z variable refers to the z level of the map z = 0 means anchor or ground
 * level */
void cabal_map(char* buffer, int buffer_size, CABAL_DATA* pCabal, int z){
  CABAL_DATA* pCab = get_parent( pCabal );
  CVROOM_DATA* pcv;
  char* ptr = buffer;

  int ceil_x = 70;
  int ceil_y = 70;

  int min_x = ceil_x;
  int min_y = ceil_y;
  int max_x = -999;
  int max_y = -999;

  int org_x = ceil_x / 2;
  int org_y = ceil_y / 2;

  int i, j;

  int map[ceil_x] [ceil_y]; //array which holds the map characters

  if (pCab == NULL || pCab->cvrooms == NULL){
    return;
  }

  /* zero the map */
  for (i = 0; i < ceil_x; i++){
    for (j = 0; j < ceil_y; j++){
      map[i][j] = ' ';
    }
  }

  /* first run through the rooms and find teh lower coordinate values */
  for (pcv = pCab->cvrooms; pcv; pcv = pcv->next ){
    int x = pcv->pos[P_X] * 4 + org_x; //*4 due one space for -[x]
    int y = (-pcv->pos[P_Y]) * 2 + org_y; //*2

    if (!pcv->fComplete || pcv->pos[P_Z] != z)
      continue;
    /* check for out of bounds */
    if (x < 0 || x >= ceil_x || y < 0 || y >= ceil_y)
      continue;

    if (x < min_x)
      min_x = x;
    if (y < min_y)
      min_y = y;

    if (x > max_x)
      max_x = x;
    if (y > max_y)
      max_y = y;

    /* mark down the room and exists */
    if (pcv->pRoom->exit[DIR_UP] != NULL && pcv->pRoom->exit[DIR_DOWN] != NULL)
      map[x][y] = 'x';
    else if (pcv->pRoom->exit[DIR_UP] != NULL)
      map[x][y] = '^';
    else if (pcv->pRoom->exit[DIR_DOWN] != NULL)
      map[x][y] = 'v';
    else
      map[x][y] = '#';

    if (x > 0)
      map[x - 1] [y] = '[';
    if (x < ceil_x)
      map[x + 1] [y] = ']';

    /* fill in the exits arounds x,y */
    if (x - 2 > 0 && pcv->pRoom->exit[DIR_WEST] != NULL)
      map[x - 2][y] = '-';
    if (x + 2 <= ceil_x && pcv->pRoom->exit[DIR_EAST] != NULL)
      map[x + 2][y] = '-';
    if (y - 1 > 0 && pcv->pRoom->exit[DIR_NORTH] != NULL)
      map[x][y - 1] = '|';
    if (y + 1 <= ceil_y && pcv->pRoom->exit[DIR_SOUTH] != NULL)
      map[x][y + 1] = '|';
  }

  /* fill in the origin */
  if (z == 0){
    map[org_x][org_y] = 'O';
    map[org_x][org_y] = 'O';
    map[org_x - 1][org_y] = '[';
    map[org_x + 1][org_y] = ']';
  }
  if (org_x < min_x)
    min_x = org_x;
  if (org_y < min_y)
    min_y = org_y;
  if (org_x > max_x)
    max_x = org_x;
  if (org_y > max_y)
    max_y = org_y;

  /* show the map now */
  /* print horizontal rows from the map into the buffer */
  for (j = min_y; j <= max_y; j++){
    for (i = 0; i < ceil_x; i++){
      *(ptr++) = map[i][j];
      if ((ptr - buffer + 2) >= buffer_size){
	sprintf( ptr, "\n\r" );
	return;
      }
    }
    if ((ptr - buffer + 3) >= buffer_size){
      sprintf( ptr, "\n\r" );
      return;
    }
    sprintf( ptr, "\n\r");
    ptr += 2;
  }
}
/* shows a map of a cabal, based on z-level that character is in
 * if forcez is != -999 we force that level
 */
void show_cabal_map( CHAR_DATA* ch, CABAL_DATA* pCabal, int ForceZ ){
CVROOM_DATA* pcv;
CABAL_DATA* pCab;
  BUFFER* buffer;
  char buf[2 * MSL];
  int z = 0;

  if ( (pCab = get_parent(pCabal)) == NULL){
    send_to_char("You are not a member of a cabal.\n\r", ch);
    return;
  }
  /* get the z-level */
  if (ForceZ != -999)
    z = ForceZ;
  else if (ch->in_room->pCabal == NULL || !is_same_cabal(pCab, ch->in_room->pCabal))
    z = 0;
  else if ( (pcv = get_cvroom_vnum(ch->in_room->vnum, pCab)) == NULL)
    z = 0;
  else
    z = pcv->pos[P_Z];

  sendf(ch, "Map of [%s] %s level %d.\n\r\n\r",
	pCab->who_name,
	z  < 0 ? "dungeon" : "floor",
	abs(z));

  cabal_map( buf, 2 * MSL, pCab, z );
  buffer=new_buf();
  add_buf( buffer, buf);
  page_to_char(buf_string(buffer),ch);
  free_buf( buffer );
  send_to_char("\n\r", ch);
}



/* main rom building function, works similiar to creating a vote */
void do_build( CHAR_DATA* ch, char* argument ){
  CABAL_DATA* pc = get_parent(ch->pCabal);
  CVOTE_DATA* cv;

  char arg[MIL];

  if (pc == NULL || IS_NPC( ch )){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if ( (cv = get_cvote( ch->pCabal->pIndexData, VOTE_BUILD)) == NULL
	    || !can_create_cvote( ch, cv)){
    send_to_char("You have no power over expansion of your organization.\n\r", ch);
    return;
  }
  else if (IS_NULLSTR( argument )){
    send_to_char("Syntax: Build <map/create/edit/clear/desc/nightdesc/name/indoor/exits/send>\n\r", ch);
    return;
  }
  /* get the command, and leave rest in argument */
  argument = one_argument( argument, arg );

/* MAP */
  if (!str_prefix(arg, "map")){
  }
/* CREATE */
  if (!str_prefix(arg, "create")){
    CROOM_DATA* cr;
    /* check some misc. stuff */
    if (ch->pcdata->pcvroom){
      send_to_char("You are alrady working on another room.\n\r", ch);
      return;
    }
    else if (!_is_cabal(ch->in_room->pCabal, pc)){
      send_to_char("You may only build from rooms that belong to your organization.\n\r", ch );
      return;
    }
    else if (pc->anchor == NULL || pc->anchor->area != ch->in_room->area){
      send_to_char("You may only build from rooms in the same area as your ogranization.\n\r", ch);
      return;
    }
    else if ( (cr = get_croom(pc, argument, atoi(argument))) == NULL){
      send_to_char("You may choose following rooms to build.\n\rUse build create <number>.\n\r", ch);
      show_crooms( ch, pc->pIndexData->crooms );
      return;
    }
/* check for no duplicate rooms */
    else if ( IS_SET(croom_table[cr->level].flags2, CROOM_NODUP)
	      && get_cvroom_cvvnum( cr->vnum, pc ) != NULL){
      send_to_char("This type of room may only be built once in the cabal.\n\r", ch);
      return;
    }
    else{
      ROOM_INDEX_DATA* pRoom = new_room_index();
      CVROOM_DATA* pcvroom = new_cvroom();

      /* copy virtual room data from original */
      pcvroom->pRoom	=	pRoom;
      pcvroom->vnum	=	cr->pRoomIndex->vnum;

      pcvroom->level	=	cr->level;

      pcvroom->pos[P_X]	=	0;
      pcvroom->pos[P_Y]	=	0;
      pcvroom->pos[P_Z]	=	0;
      ch->pcdata->pcvroom = pcvroom;

      /* we copy all the info from the skeleton room into our room */
      /* the vnum is not assigned untill we actuly post the room, then the resets are refreshed as well */
      clone_room( cr->pRoomIndex, pRoom, TRUE );
      pRoom->area = pc->anchor->area;
      pRoom->pCabal = pc;

      /* take off the noreset flags */
      REMOVE_BIT(pRoom->room_flags2, ROOM_NO_RESET);
      sendf(ch, "%s created.\n\r", pRoom->name);
    }
  }
/* EDIT */
  else if (!str_prefix( arg, "edit")){
    CVROOM_DATA* pcv;
    /* check some misc. stuff */
    if (ch->pcdata->pcvroom){
      send_to_char("You are alrady working on another room.\n\r", ch);
      return;
    }
    else if (!_is_cabal(ch->in_room->pCabal, pc)){
      send_to_char("You may only edit rooms that belong to your organization.\n\r", ch );
      return;
    }
    else if (pc->anchor == NULL || pc->anchor->area != ch->in_room->area){
      send_to_char("You may only edit rooms in the same area as your ogranization.\n\r", ch);
      return;
    }
    else if ( (pcv = get_cvroom( ch->in_room, ch->in_room->pCabal )) == NULL){
      send_to_char("This room was not built by your organization.\n\r", ch);
      return;
    }
    else{
      /* we now have a pointer to the cvroom data of the room we are about to edit */
      /* we copy the cvroom data, and clone the room */

      ROOM_INDEX_DATA* pRoom = new_room_index();
      CVROOM_DATA* pcvroom = new_cvroom();
      EXIT_DATA* pexit;
      int door;

      /* copy virtual room data from original */
      pcvroom->pRoom	=	pRoom;
      pcvroom->vnum	=	pcv->vnum;
      pcvroom->fComplete=	pcv->fComplete;


      pcvroom->level	=	pcv->level;

      pcvroom->pos[P_X]	=	pcv->pos[P_X];
      pcvroom->pos[P_Y]	=	pcv->pos[P_Y];
      pcvroom->pos[P_Z]	=	pcv->pos[P_Z];

      clone_room( ch->in_room, pRoom, TRUE );

      /* now copy the existing exits */
      for (door = 0; door < MAX_DOOR; door++){
	if ( (pexit = pcv->pRoom->exit[door]) == NULL || pexit->to_room == NULL)
	  continue;
	else{
	  EXIT_DATA* pExit = new_exit();

	  pExit->to_room	  = pexit->to_room;
	  pExit->vnum		  = pexit->vnum;
	  pExit->orig_door	  = door;
	  pExit->exit_info	 = pexit->rs_flags;
	  pExit->rs_flags	 = pexit->rs_flags;

	  if (!IS_NULLSTR(pexit->keyword))
	    pExit->keyword = str_dup( pexit->keyword );
	  if (!IS_NULLSTR(pexit->description))
	    pExit->keyword = str_dup( pexit->description );

	  pcvroom->pRoom->exit[door] = pExit;
	}
      }

      ch->pcdata->pcvroom = pcvroom;
      sendf(ch, "Now editing %s.\n\r", pRoom->name);
    }
    return;
  }
/* REPLACE */
  else if (!str_prefix( arg, "replace")){
    CVROOM_DATA* pcv;
    CROOM_DATA* cr;

    /* check some misc. stuff */
    if (ch->pcdata->pcvroom){
      send_to_char("You are alrady working on another room.\n\r", ch);
      return;
    }
    else if (!_is_cabal(ch->in_room->pCabal, pc)){
      send_to_char("You may only replace rooms that belong to your organization.\n\r", ch );
      return;
    }
    else if (pc->anchor == NULL || pc->anchor->area != ch->in_room->area){
      send_to_char("You may only replace rooms in the same area as your ogranization.\n\r", ch);
      return;
    }
    else if ( (pcv = get_cvroom( ch->in_room, ch->in_room->pCabal )) == NULL){
      send_to_char("This room was not built by your organization.\n\r", ch);
      return;
    }
    else if ( (cr = get_croom(pc, argument, atoi(argument))) == NULL){
      send_to_char("You may choose following rooms to use as replacements.\n\rUse build replace <number>.\n\r", ch);
      show_crooms( ch, pc->pIndexData->crooms );
      return;
    }
/* check for no duplicate rooms */
    else if ( IS_SET(croom_table[cr->level].flags2, CROOM_NODUP)
	      && get_cvroom_cvvnum( cr->vnum, pc ) != NULL){
      send_to_char("This type of room may only be built once in the cabal.\n\r", ch);
      return;
    }
    else{
      /* we now have a pointer to the cvroom data of the room we are about to edit */
      /* we copy the cvroom data, and clone the new replacement room */

      ROOM_INDEX_DATA* pRoom = new_room_index();
      CVROOM_DATA* pcvroom = new_cvroom();
      EXIT_DATA* pexit;
      int door = 0;

      /* copy virtual room data from original */
      pcvroom->pRoom	=	pRoom;
      pcvroom->vnum	=	cr->pRoomIndex->vnum;
      pcvroom->fComplete=	pcv->fComplete;

      pcvroom->level	=	cr->level;

      pcvroom->pos[P_X]	=	pcv->pos[P_X];
      pcvroom->pos[P_Y]	=	pcv->pos[P_Y];
      pcvroom->pos[P_Z]	=	pcv->pos[P_Z];

      clone_room( cr->pRoomIndex, pcvroom->pRoom, TRUE );
      pcvroom->pRoom->pCabal=	pc;

      /* now copy the existing exits */
      for (door = 0; door < MAX_DOOR; door++){
	if ( (pexit = pcv->pRoom->exit[door]) == NULL || pexit->to_room == NULL)
	  continue;
	else{
	  EXIT_DATA* pExit = new_exit();

	  pExit->to_room	 = pexit->to_room;
	  pExit->vnum		 = pexit->vnum;
	  pExit->orig_door	 = door;
	  pExit->exit_info	 = pexit->rs_flags;
	  pExit->rs_flags	 = pexit->rs_flags;

	  if (!IS_NULLSTR(pexit->keyword))
	    pExit->keyword = str_dup( pexit->keyword );
	  if (!IS_NULLSTR(pexit->description))
	    pExit->keyword = str_dup( pexit->description );

	  pcvroom->pRoom->exit[door] = pExit;
	}
      }

      ch->pcdata->pcvroom = pcvroom;
      sendf(ch, "Replacement ready.\n\r");
    }
    return;
  }
/* SELL */
  else if (!str_cmp(arg, "sell")){
    /* check some misc. stuff */
    if (ch->pcdata->pcvroom == NULL){
      send_to_char("You must edit a room first.\n\r", ch);
      return;
    }
    else if (ch->pcdata->pcvroom->fComplete == FALSE ){
      send_to_char("You may only sell existing rooms.\n\r", ch);
      return;
    }
    else if (IS_NULLSTR(argument)){
      sendf( ch, "The room %s will be sold if this vote passes.  Use \"build sell confirm\" to confirm.\n\r",
	     ch->pcdata->pcvroom->pRoom->name);
      return;
    }
/* get the current vote info for building a room */
    else if ( (cv = get_cvote( ch->pCabal->pIndexData, VOTE_BUILD)) == NULL
	      || !can_create_cvote( ch, cv)){
      send_to_char("You have no power over expansion of your organization.\n\r", ch);
      return;
    }
    else if ( GET_CP( ch ) < cv->cost ){
      int cost = cv->cost;
      sendf( ch, "You require at least %d %s%s to create the vote to sell this room.\n\r",
	     cost, pc->currency, cost == 1 ? "" : "s" );
      return;
    }
    else{
      int cost = UMAX(1, get_croom_bcost(ch->pcdata->pcvroom->level / 2));
      /* we create a sell vote, rest is taken care off in execute_vote */
      /* create vote here */
      if (!create_sell_vote( ch, ch->pcdata->pcvroom, cost )){
	send_to_char("You cannot sell a room when there still exists a vote regarding it.\n\r", ch);
      }
      else{
	int cost = cv->cost;
	CP_GAIN(ch, -cost, TRUE );
	sendf( ch, "Vote for sale of %s has been created.\n\r", ch->pcdata->pcvroom->pRoom->name );
	free_cvroom(ch->pcdata->pcvroom);
	ch->pcdata->pcvroom = NULL;
      }
      return;
    }
  }
/* SHOW */
  else if (!str_prefix(arg, "show" )){
    if (ch->pcdata->pcvroom == NULL){
      send_to_char("You are not working on a room.\n\r", ch);
      return;
    }
    else{
      EXIT_DATA* pexit;
      ROOM_INDEX_DATA* in_room = ch->pcdata->pcvroom->pRoom;
      char buf[MSL];
      int door = 0;
      bool found = FALSE;

      /* ROOM NAME + COORDINATES */
      send_to_char( in_room->name, ch );
      if (IS_ORIGIN( ch->pcdata->pcvroom->pos))
	send_to_char(" [-, -, -]", ch);
      else{
	sendf( ch, " [%d, %d, %d]",
	       ch->pcdata->pcvroom->pos[P_X], ch->pcdata->pcvroom->pos[P_Y], ch->pcdata->pcvroom->pos[P_Z]);
      }
      send_to_char(IS_SET(in_room->room_flags, ROOM_INDOORS) ? " (indoors)" : " (outside)", ch );
      send_to_char("\n\r", ch);

      /* ROOM DESC */
      send_to_char( "  ",ch);
      send_to_char( in_room->description, ch );

      if (!IS_NULLSTR( in_room->description2 )){
	send_to_char("\n\rNIGHT DESC:\n\r", ch);
	send_to_char( in_room->description2, ch );
      }
      send_to_char( "\n\r", ch );

      /* exits */
      sprintf(buf,"Current exits:\n\r");

      for ( door = 0; door <= 5; door++ ){
    	bool paran = FALSE;
	int pos[3];

	pos[P_X]	=	ch->pcdata->pcvroom->pos[P_X];
	pos[P_Y]	=	ch->pcdata->pcvroom->pos[P_Y];
	pos[P_Z]	=	ch->pcdata->pcvroom->pos[P_Z];

	get_new_coordinates( pos, door );
	if ( (pexit = ch->pcdata->pcvroom->pRoom->exit[door] ) == NULL
	     || pexit->to_room == NULL){
	  CVROOM_DATA* adj_cvroom = get_cvroom_xyz(pos[P_X], pos[P_Y], pos[P_Z], ch->pcdata->pcvroom->pRoom->pCabal, TRUE, FALSE);
	  if (IS_ORIGIN(ch->pcdata->pcvroom->pos))
	    continue;
	  else if (adj_cvroom){
	    sendf( ch, "%-5s - Adjacent room: %s.\n\r", capitalize( dir_name[door] ),
		   adj_cvroom->pRoom->name);
	  }
	  else if (IS_ORIGIN(pos)){
	    sendf( ch, "%-5s - Adjacent room: %s.\n\r", capitalize( dir_name[door] ),
		   ch->pcdata->pcvroom->pRoom->pCabal->anchor->name);
	  }
	  continue;
	}
	else{
	  if (IS_SET(pexit->exit_info, EX_CLOSED))
	    paran = TRUE;
	  found = TRUE;

	  if (paran)
	    sprintf( buf + strlen(buf), "(%-5s) - %s (%d, %d, %d) [%-15s]",
		     capitalize( dir_name[door] ),
		     pexit->to_room->name,
		     pos[P_X], pos[P_Y], pos[P_Z],
		     flag_string(exit_flags, pexit->rs_flags));
	  else
	    sprintf( buf + strlen(buf), " %-5s  - %s (%d, %d, %d) [%-15s]",
		     capitalize( dir_name[door] ),
		     pexit->to_room->name,
		     pos[P_X], pos[P_Y], pos[P_Z],
		     flag_string(exit_flags, pexit->rs_flags));
	  sprintf( buf + strlen( buf ), "  %-15s", pexit->keyword );
	  sprintf(buf + strlen(buf), "\n\r");
	}//end else
      }//end for
      if ( !found )
	send_to_char("No exits.\n\r", ch);
      else
	send_to_char(buf, ch );
    }//end else
  }
/* CLEAR */
  else if (!str_prefix(arg, "clear")){
    if (ch->pcdata->pcvroom == NULL){
      send_to_char("You are not working on a room.\n\r", ch);
      return;
    }
    else{
      free_cvroom( ch->pcdata->pcvroom );
      ch->pcdata->pcvroom = NULL;
      send_to_char("Ok.\n\r", ch );
      return;
    }
  }
/* NAME */
  else if (!str_prefix(arg, "name")){
    if (ch->pcdata->pcvroom == NULL){
      send_to_char("You are not working on a room.\n\r", ch);
      return;
    }
    else if (IS_SET( croom_table[ch->pcdata->pcvroom->level].flags2, CROOM_FIXEDNAME)){
      send_to_char("You are not allowed to change this room's name.\n\r", ch );
      return;
    }
    else if (IS_NULLSTR( argument )){
      send_to_char("build name <name>\n\r", ch);
      return;
    }
    else{
      format_room_name(ch->pcdata->pcvroom->pRoom, argument);
      send_to_char("Ok.\n\r", ch);
      return;
    }
  }
/* OUTDOOR */
  else if (!str_prefix(arg, "indoors")){
    if (ch->pcdata->pcvroom == NULL){
      send_to_char("You are not working on a room.\n\r", ch);
      return;
    }

    TOGGLE_BIT( ch->pcdata->pcvroom->pRoom->room_flags, ROOM_INDOORS);
    send_to_char("Indoor/Outdoor status toggled.\n\r", ch);
    return;
  }
/* DESC */
  else if (!str_prefix(arg, "description")){
    if (ch->pcdata->pcvroom == NULL){
      send_to_char("You are not working on a room.\n\r", ch);
      return;
    }
    else{
      string_append( ch, &ch->pcdata->pcvroom->pRoom->description );
      return;
    }
  }
/* NIGHTDESC */
  else if (!str_prefix(arg, "nightdescription")){
    if (ch->pcdata->pcvroom == NULL){
      send_to_char("You are not working on a room.\n\r", ch);
      return;
    }
    else{
      string_append( ch, &ch->pcdata->pcvroom->pRoom->description2 );
      return;
    }
  }
/* EXITS */
  else if (!str_prefix(arg, "exits")){
    if (ch->pcdata->pcvroom == NULL){
      send_to_char("You are not working on a room.\n\r", ch);
      return;
    }
    else{
      build_exits( ch, ch->pcdata->pcvroom, ch->in_room, argument );
      return;
    }
  }
/* SEND/END */
  else if (!str_prefix(arg, "send") || !str_prefix(arg, "post") ){
    int cost = 0;
    CVROOM_DATA* old_cvr = NULL;

    if (ch->pcdata->pcvroom == NULL){
      send_to_char("You are not working on a room.\n\r", ch);
      return;
    }
    /* check if the room has exits = non origin coordinates */
    else if (ch->pcdata->pcvroom->pos[P_X] == 0
	     && ch->pcdata->pcvroom->pos[P_Y] == 0
	     && ch->pcdata->pcvroom->pos[P_Z] == 0){
      send_to_char("You must first create a valid exit for this room.\n\r", ch );
      return;
    }
/* get the current vote info for building a room */
    else if ( (cv = get_cvote( ch->pCabal->pIndexData, VOTE_BUILD)) == NULL
	      || !can_create_cvote( ch, cv)){
      send_to_char("You have no power over expansion of your organization.\n\r", ch);
      return;
    }
/* check for no duplicate rooms */
    else if ( IS_SET(croom_table[ch->pcdata->pcvroom->level].flags2, CROOM_NODUP)
	      && get_cvroom_cvvnum( ch->pcdata->pcvroom->vnum, pc ) != NULL){
      send_to_char("This type of room may only be built once in the cabal.\n\r", ch);
      return;
    }
/* check for overlappign positions for new rooms (edited rooms skip this) */
    else if (!ch->pcdata->pcvroom->fComplete
	     && (old_cvr = get_cvroom_xyz( ch->pcdata->pcvroom->pos[P_X],
				ch->pcdata->pcvroom->pos[P_Y],
				ch->pcdata->pcvroom->pos[P_Z], pc, TRUE, FALSE )) != NULL){
      send_to_char("A room has already been built there, or awaits approval for that position.\n\r", ch);
      return;
    }
/* check for overlappign positions for edited rooms (new rooms skip this) */
    else if (ch->pcdata->pcvroom->fComplete
	     && (old_cvr = get_cvroom_xyz( ch->pcdata->pcvroom->pos[P_X],
				ch->pcdata->pcvroom->pos[P_Y],
				ch->pcdata->pcvroom->pos[P_Z], pc, FALSE, TRUE )) != NULL){
      send_to_char("A room has already been built there, or awaits approval for that position.\n\r", ch);
      return;
    }
/* check room misc. stuff */
    else if ( IS_NULLSTR( ch->pcdata->pcvroom->pRoom->name )){
      send_to_char("You must first choose a name for this room.\n\r", ch );
      return;
    }
    else if ( IS_NULLSTR( ch->pcdata->pcvroom->pRoom->description )){
      send_to_char("You must first create a description for this room.\n\r", ch );
      return;
    }
    /* cost of replacement is that of the new room */
    if (old_cvr && old_cvr->vnum != ch->pcdata->pcvroom->vnum)
      cost = UMAX(1, get_croom_bcost(ch->pcdata->pcvroom->level)) / 10;
    /* check cost (editing a room costs 1/10 its cost) */
    else
      cost = UMAX(1, get_croom_bcost(ch->pcdata->pcvroom->level));

    if (GET_CAB_CP( pc ) < cost ) {
      sendf( ch, "%s needs at least %d %s%s in its coffers to attempt to %s this room.\n\r"\
	     "Currently you only have %d.\n\r ",
	     pc->who_name, cost,
	     pc->currency, (cost) == 1 ? "" : "s",
	     ch->pcdata->pcvroom->fComplete ? "renovate" : "construct",
	     GET_CAB_CP( pc ) );
      return;
    }
    else if ( GET_CP( ch ) < cv->cost ){
      sendf( ch, "You require at least %d %s%s to create the vote for this room.\n\r",
	     cost, pc->currency, cost == 1 ? "" : "s" );
      return;
    }
/* attach the room to the cabal, and create the vote */
    else{
      int cost = UMAX(1, get_croom_bcost(ch->pcdata->pcvroom->level));
      /* attach the room to the cabal */
      cvroom_to_cabal( ch->pcdata->pcvroom, pc );

      CHANGE_CABAL( pc );
      VRCHANGE_CABAL( pc );

      /* create vote here */
      if (ch->pcdata->pcvroom->fComplete )
	create_edit_vote( ch, ch->pcdata->pcvroom, -cost );
      else
	create_build_vote( ch, ch->pcdata->pcvroom, -cost );

      /* deduct the cost */
      CP_CAB_GAIN( pc, -cost);
      CP_GAIN(ch, -cv->cost, TRUE );

/* DEBUG: connect the room
      ch->pcdata->pcvroom->fComplete = TRUE;
      load_vir_room( ch->pcdata->pcvroom->pRoom, pc->anchor->area );
*/
      sendf( ch, "Vote for construction of %s has been created.\n\r", ch->pcdata->pcvroom->pRoom->name );
      ch->pcdata->pcvroom = NULL;
      save_cabals( TRUE, NULL );
      return;
    }
  }
  else{
    do_build( ch, "");
  }
}
/* ------------============= PACT UPDATE FUNCTIONS ================------  */
  /* The following rules apply to various pact types

      - Positive rating gain is done in such way that full rating for the pact type is achived
     only after PACT_X_DURATION hours
     - Vendetta benefits never change
     - Trade Pacts between a CABAL and a SERVICE cabal never result in change for CABAL
     this means that trade pact between two service cabals never changes in benefit
  */


/* updates a status of a pact based on its rating , returns FALSE if pact no longer exists */
bool update_pact_status( PACT_DATA* pp ){
  bool fRet = TRUE;
  char to[MIL];
  char buf[MIL];
  int ori_type = pp->type;

  /* check if we crossed over the fail rating */
  if (pp->rating < PACT_FAIL_RATING ){
    pp->rating = PACT_MAX_RATING - PACT_START_RATING;
    /* pact has failed, we check various pact types and act approprietly */
    switch (pp->type){
    case PACT_ALLIANCE:
      pp->type	= PACT_NAPACT;		break;
    case PACT_NAPACT:
      pp->type	= PACT_TRADE;		break;
    case PACT_VENDETTA:
      if (pp->creator->enemy && pp->creator->enemy == pp->benefactor)
	return TRUE;
    default:
    case PACT_TRADE:
      fRet	= FALSE;		break;
    }
    /* check if we remove the pact (vendettas between enemies do not fail)*/
    if (!fRet){
      sprintf( buf, "Without active support the %s between [%s] and [%s]\n\rhas been cancelled.\n\r",
	       flag_string(pact_flags, ori_type), pp->creator->who_name, pp->benefactor->who_name);
      sprintf( to, "%s %s", pp->creator->name, pp->benefactor->name);
      do_hal( to, "A pact has been cancelled.", buf, NOTE_NEWS );
      rem_pact( pp );
      free_pact( pp );
    }
    else if (pp->type != ori_type) {
      char new_pact [MIL];
      sprintf( new_pact, "%s", flag_string(pact_flags, pp->type));
      sprintf( buf, "Due to suffering relations between [%s] and [%s]\n\ryour %s has been reduced to a mere %s.\n\r",
	       pp->creator->who_name, pp->benefactor->who_name,
	       flag_string(pact_flags, ori_type), new_pact );
      sprintf( to, "%s %s", pp->creator->name, pp->benefactor->name);
      do_hal( to, "A pact has declined.", buf, NOTE_NEWS );
    }
    return fRet;
  }
/* check if pact is at max rating */
  else if (pp->rating >= PACT_MAX_RATING){
    /* pact is at max check if we upgrade the pact (trade pacts) */
    switch (pp->type){
    case PACT_TRADE:
      //cannot advance to alliance if we already have one
      if (pp->Adv >= PACT_NAPACT)	pp->type = PACT_NAPACT;		break;
    case PACT_NAPACT:
      if (pp->Adv >= PACT_ALLIANCE
	  && !IS_CABAL(pp->creator, CABAL_ALLIANCE)
	  &&  !IS_CABAL(pp->benefactor, CABAL_ALLIANCE))
	pp->type = PACT_ALLIANCE;	break;
    default:
      break;
    }
    /* check if the type of pact has changed */
    if (pp->type != ori_type){
      char new_pact[MIL];

      pp->rating = PACT_START_RATING;

      sprintf( new_pact, "%s", flag_string(pact_flags, pp->type));
      sprintf( buf, "With the improving relations between [%s] and [%s]\n\ryour %s has flowered into %s!\n\r",
	       pp->creator->who_name, pp->benefactor->who_name,
	       flag_string(pact_flags, ori_type), new_pact);
      sprintf( to, "%s %s", pp->creator->name, pp->benefactor->name);
      do_hal( to, "A pact has improved.", buf, NOTE_NEWS );
    }
    return fRet;
  }
  else
    return TRUE;
}



/* updates a single pacts rating accorind to its type/duration */
void update_pact_rating( PACT_DATA* pp ){
  int sec_per_rating = 0;
  int sec = 0;
  int rating = UMAX(1, PACT_MAX_RATING);
  int gain = 0;

  switch ( pp->type ){
  default:
  case PACT_TRADE:
    sec	=	PACT_NORMAL_DUR;	break;
  case PACT_NAPACT:
    sec	=	PACT_NAPACT_DUR;	break;
  case PACT_ALLIANCE:
    sec	=	PACT_ALLIANCE_DUR;	break;
  case PACT_VENDETTA:
    sec	=	PACT_VENDETTA_DUR;	break;
  }

  /* convert hours to seconds */
  sec = sec * 3600;
  /* calculate amount of seconds before a single rating increase */
  sec_per_rating = UMAX(1, sec / rating );

/* calculate how much we gain */
  /* (current time - last time) / (time/gain) = gain */
  gain = ((mud_data.current_time - pp->time_stamp) / sec_per_rating);
  /* Vendetta LOSSES rating over time (if there are no hostile actions it expires) */
  if (pp->type == PACT_VENDETTA)
    gain *= -1;

  if (gain != 0){
    pp->rating = URANGE(0, pp->rating + gain, PACT_MAX_RATING);
    pp->time_stamp = mud_data.current_time;
  }
}





/* update single pacts economic benefits */
void update_pact_economy( PACT_DATA* pp ){
  int run = PACT_MAX_RATING;
  int cy_min = 0, cy_max = 0;	//cp bonus range
  int sy_min = 0, sy_max = 0;	//support bonus range
  int x = pp->rating;

  /* special cases */
  if (pp->complete != PACT_COMPLETE || pp->creator == NULL || pp->benefactor == NULL)
    return;

  /* get y_min and y_max of a pact */
  switch ( pp->type ){
  default:
  case PACT_TRADE:
    cy_min	= TRADE_CP_START;
    cy_max	= TRADE_CP_TRADE;
    sy_min	= TRADE_SUP_START;
    sy_max	= TRADE_SUP_TRADE;
    break;
  case PACT_NAPACT:
    cy_min	= TRADE_CP_TRADE;
    cy_max	= TRADE_CP_NA;
    sy_min	= TRADE_SUP_TRADE;
    sy_max	= TRADE_SUP_NA;
    break;
  case PACT_ALLIANCE:
    cy_min	= TRADE_CP_NA;
    cy_max	= TRADE_CP_MAX;
    sy_min	= TRADE_SUP_NA;
    sy_max	= TRADE_SUP_MAX;
    break;
  case PACT_VENDETTA:
    cy_min	= TRADE_CP_WAR;
    cy_max	= TRADE_CP_WAR;
    sy_min	= TRADE_SUP_WAR;
    sy_max	= TRADE_SUP_WAR;
    break;
  }

/* we have all the data we need to calculate bonuses.  do each cabal now */
/* CREATOR BONUS */
/* If cabal1 is a service cabal then cabal2 gets a constant economic "cost" of service */
  if (IS_CABAL(pp->benefactor, CABAL_SERVICE) && pp->type != PACT_VENDETTA){
    pp->c_cp	= TRADE_CP_SERVE;
    pp->c_sup	= TRADE_SUP_SERVE;
  }
  else{
/* otherwise we calculate current bonus level with: bonus = mx + b = ((y_max - y_min)/run)x + b */
    pp->c_cp = (x * (cy_max - cy_min) / run) + cy_min;
    pp->c_sup = (x * (sy_max - sy_min) / run) + sy_min;
  }

/* BENEFACTOR BONUS */
/* If cabal2 is a service cabal then cabal1 gets a constant economic "cost" of service */
  if (IS_CABAL(pp->creator, CABAL_SERVICE) && pp->type != PACT_VENDETTA){
    pp->b_cp	= TRADE_CP_SERVE;
    pp->b_sup	= TRADE_SUP_SERVE;
  }
  else{
/* otherwise we calculate current bonus level with: bonus = mx + b = ((y_max - y_min)/run)x + b */
    pp->b_cp = (x * (cy_max - cy_min) / run) + cy_min;
    pp->b_sup = (x * (sy_max - sy_min) / run) + sy_min;
  }

/* udpate the benefactor and creator's "trade" stats with these numbers */
//support is not traded
  pp->creator->cp_gain[ECO_TRADE] += get_trade_bonus(pp, TRUE, TRUE );
  pp->benefactor->cp_gain[ECO_TRADE] += get_trade_bonus(pp, FALSE, TRUE );
}

/* updates all pacts, run inside cabal update, right before cabals are updated */
void pact_update(){
  PACT_DATA* pPact, *pPact_next;

  for (pPact = pact_list; pPact; pPact = pPact_next){
    pPact_next = pPact->next;

    /* update the pact rating */
    update_pact_rating( pPact );

    /* check if the pact should be broken, or changed in some way.  TRUE if pact still exists */
    if (update_pact_status( pPact ) == FALSE)
      continue;

    /* update pact flags */
    set_pact_flags( pPact, FALSE );

    /* update the economic bonuses */
    update_pact_economy( pPact );
  }
}

/* affects a pact between two cabals of given type by gain, returns new value or -1 if not found */
/* notice that TRADE, NA, and ALLIANCE are all treated as the same type in this */
int pact_gain( CABAL_DATA* pCabal1, CABAL_DATA* pCabal2, int type, int gain ){
  PACT_DATA* pp = get_pact( pCabal1, pCabal2, type, (type >= PACT_TRADE && type <= PACT_ALLIANCE ));
  if (pp == NULL)
    return 0;
  if (pp->complete == PACT_COMPLETE)
    pp->rating = UMAX(0, pp->rating + gain);
  return pp->rating;
}

/* affects relation ships between all justice cabals that have pact with give royal cabal*/
void affect_justice_relations( CABAL_DATA* pRoyal, int gain ){
  PACT_DATA* pp = pact_list;

  for (pp = pact_list; pp; pp = pp->next){
    if (pp->complete != PACT_COMPLETE || pp->type < PACT_TRADE || pp->type > PACT_ALLIANCE)
      continue;
    else if (pp->benefactor == pRoyal && IS_CABAL(pp->creator, CABAL_JUSTICE) && pp->creator->present > 0)
      affect_cabal_relations( pp->creator, pRoyal, gain, TRUE );
    else if (pp->creator == pRoyal && IS_CABAL(pp->benefactor, CABAL_JUSTICE) && pp->benefactor->present > 0)
      affect_cabal_relations( pp->benefactor, pRoyal, gain, TRUE );
  }
}

/* affects relationship between two cabals by boosting or hurting their of pacts by "gain" amount */
/* NOTE: pc1 Is the "cause" of the gain shift, and ONLY its pacts with partners of pc2 will be affected
 * if fSelf, then the gain is also applied to the pc1 and pc2 cabal's trade or vendetta pact whichever exists */
/* NOTICE: gain is a relative shift in cabal to cabal relations, not exact shift in pacts. + is "better relations" */
void affect_cabal_relations( CABAL_DATA* Pc1, CABAL_DATA* Pc2, int gain, bool fSelf ){

  /* COMMENTS:
     + pos. gain means positive shift in relations (trade increases, vendetta's decrease)
     - if either of cabals have alliances with common partners those pacts are affected by 50% of gain

  */
  CABAL_DATA* partners[max_cabal];		//holds a list of partners of both cabals
  int data[max_cabal];				//holds data about the partners

  const int CAB1_TRADE		= 1;			//bits representing cabal1's relations with the partner
  const int CAB1_VENDETTA	= 2;
  const int CAB2_TRADE		= 1024;			//bits repersenting cabal2's relations with the partner
  const int CAB2_VENDETTA	= 2048;

  int i = 0;
  bool f1 = FALSE;
  PACT_DATA* pp;
  CABAL_DATA* pPartner = NULL;
  CABAL_DATA* pc1 = get_parent( Pc1 );
  CABAL_DATA* pc2 = get_parent( Pc2 );

  if (pc1 == NULL || pc2 == NULL || pc1 == pc2)
    return;


/* Zero the partners array */
  for (i = 0; i < max_cabal; i ++){
    partners[i] = NULL;
    data[i] = 0;
  }
  /* we first run through ALL the pacts, and being filling all the partners and data */
  for (pp = pact_list; pp; pp = pp->next ){
    /* skip any not completed pacts */
    if (pp->complete != PACT_COMPLETE)
      continue;
    /* we insert the partner that is not pc1 and not pc2 into partners array */
    else if ( pp->creator == pc1){
      pPartner = pp->benefactor;
      f1 = TRUE;
    }
    else if ( pp->benefactor == pc1){
      pPartner = pp->creator;
      f1 = TRUE;
    }
    else if (pp->creator == pc2){
      pPartner = pp->benefactor;
      f1 = FALSE;
    }
    else if (pp->benefactor == pc2){
      pPartner = pp->creator;
      f1 = FALSE;
    }
    else
      continue;

    /* if we have a valid partner, we also check if the pact is trade or vendetta */
    partners[pPartner->vnum] = pPartner;
    if (f1){
      if (pp->type >= PACT_TRADE && pp->type <= PACT_ALLIANCE)
	SET_BIT(data[pPartner->vnum], CAB1_TRADE);
      if (pp->type == PACT_VENDETTA)
	SET_BIT(data[pPartner->vnum], CAB1_VENDETTA);

    }
    else{
      if (pp->type >= PACT_TRADE && pp->type <= PACT_ALLIANCE)
	SET_BIT(data[pPartner->vnum], CAB2_TRADE);
      if (pp->type == PACT_VENDETTA)
	SET_BIT(data[pPartner->vnum], CAB2_VENDETTA);
    }
  }//end for

/* at this point the partners array has all the pact partners of either pc1 or pc2
 * and data array has bits that can tell us if pc1 and pc2 have pacts with that partner.
 * we run over the data array, and affect the pats of pc1->partner if pc2->partner pact exists
 * (both  PC1_XX and PC2_XX bits set)
 */
/* the pact (if any) between pc1 and pc2 exists in partner cabal as well under their vnums
 * hence we toggle the sign on the gain due to the pact between pc2->pc1
 */
  if (IS_SET(data[pc2->vnum], CAB1_VENDETTA)){
    if ( fSelf ){
      pact_gain(pc1, pc2, PACT_VENDETTA, -gain );
//      cabal_echo(pc1, "vendetta self");
    }
  }
  else if ( fSelf ){
//    cabal_echo(pc1, "trade self");
    pact_gain(pc1, pc2, PACT_TRADE, gain );
  }

/* now we run through rest of partners, and toggle/add gain based on their pacts */
  for (i = 0; i < max_cabal; i++){
    if (partners[i] == NULL)
      continue;
    else if (partners[i] == pc1 || partners[i] == pc2)
      continue;
    else if ( (!IS_SET(data[i], CAB1_TRADE) && !IS_SET(data[i], CAB1_VENDETTA))
	      || (!IS_SET(data[i], CAB2_TRADE) && !IS_SET(data[i], CAB2_VENDETTA)) )
      continue;
    /* at this point we know we have a common partner to both pc1 and pc2 cabal
     * we now apply 50% +/- gain depending on trade or vendetta pacts with the partner cabal
     */
    else{
      int new_gain = gain / 2;
      int type = PACT_TRADE;
//      char buf[MIL];
      /* we now toggle the sign based on the pc1->partner pact and pc2->partner pact*/
      /* we treat trade as pos've and vendettas as neg've */

      /* effect table:
	 partner->pc1  partner->pc2   multiplication sign
	 Ven              Ven             +
	 Ven              Tra             -
	 Tra              Ven             -
	 Tra              Tra             +
      */

      if (IS_SET(data[i], CAB1_VENDETTA) && IS_SET(data[i], CAB2_TRADE)){
	new_gain *= -1;
	type = PACT_VENDETTA;
      }
      else if (IS_SET(data[i], CAB1_TRADE) && IS_SET(data[i], CAB2_VENDETTA)){
	new_gain *= -1;
      }

      /* at this point we now have final sign on the gain between pc1->partner
       * so we apply the gain
       */

      pact_gain( pc1, partners[i], type, new_gain );
      /* debug
      sprintf( buf, "Gained %d rating with %s.\n\r", new_gain, partners[i]->who_name);
      cabal_echo(pc1, buf );
      */

    }//END gain with partners
  }//end for
}//end cabal relations


/* sells a single cabal room */
bool sell_room( CVROOM_DATA* pcv, CABAL_DATA* pCab, int cost ){
  EXIT_DATA* pexit;
  int door;
  int doors = 0;

  if (pcv->pRoom->vnum != ROOM_VNUM_CROOM_DUMMY)
    CP_CAB_GAIN( pCab, cost );

  /* we check if this room has more then 1 exit */
  for (door = 0; door  < MAX_DOOR; door ++ ){
    if ( (pexit = pcv->pRoom->exit[door]) != NULL && pexit->to_room != NULL)
      doors++;
  }
  /* check if we can remove the room right out, or if we need to put in an ampty hallway */
  if (doors < 2){
    unload_vir_room( pcv->pRoom );
    cvroom_from_cabal( pcv, pCab );
    free_cvroom( pcv );
    return TRUE;
  }
  else{
    /* We need to clone an empty hallway and copy its exits */
    ROOM_INDEX_DATA* src = get_room_index( ROOM_VNUM_CROOM_DUMMY );
    CVROOM_DATA* pcvroom;
    if (src == NULL){
      bug("sell_room: Could not get the dummy room!", 0);
      return FALSE;
    }
    /* we now create a new cabal room which is a clone of a skeleton room */
    pcvroom = new_cvroom();

    /* copy virtual room data from original */
    pcvroom->pRoom	=	new_room_index();
    pcvroom->vnum		=	ROOM_VNUM_CROOM_DUMMY;
    pcvroom->fComplete	=	TRUE;


    pcvroom->level	=	0;

    pcvroom->pos[P_X]	=	pcv->pos[P_X];
    pcvroom->pos[P_Y]	=	pcv->pos[P_Y];
    pcvroom->pos[P_Z]	=	pcv->pos[P_Z];

    clone_room( src, pcvroom->pRoom, TRUE );
    pcvroom->pRoom->pCabal = pCab;

    /* now copy the existing exits */
    for (door = 0; door < MAX_DOOR; door++){
      if ( (pexit = pcv->pRoom->exit[door]) == NULL || pexit->to_room == NULL)
	continue;
      else{
	EXIT_DATA* pExit = new_exit();

	pExit->to_room	= pexit->to_room;
	pExit->vnum	= pexit->vnum;
	pExit->orig_door	= door;
	pExit->exit_info	= pexit->rs_flags;
	pExit->rs_flags	= pexit->rs_flags;

	if (!IS_NULLSTR(pexit->keyword))
	  pExit->keyword = str_dup( pexit->keyword );
	if (!IS_NULLSTR(pexit->description))
	  pExit->keyword = str_dup( pexit->description );

	pcvroom->pRoom->exit[door] = pExit;
      }
    }
    /* everything is copied, remove and get rid of the old room and put in the new one */
    unload_vir_room( pcv->pRoom );
    cvroom_from_cabal( pcv, pCab );
    free_cvroom( pcv );

    cvroom_to_cabal( pcvroom, pCab );
    fix_vir_exits( pcvroom );
    load_vir_room( pcvroom->pRoom, pCab->anchor->area );
    return TRUE;
  }
}

/* sells rooms from cabal untill cp is > 0 or no more rooms */
void sell_rooms( CABAL_DATA* pCab ){
  const int max_rooms = pCab->pIndexData->max_room + 1;
  CVROOM_DATA* cvrooms[max_rooms];
  CVROOM_DATA* pcv, *pcv_next;
  int max_cvr = 0;
  int i= 0;

  char text[MSL];
  char buf[MIL];

  /* we first get a list of all rooms in the cabal */
  for (pcv = pCab->cvrooms; pcv; pcv = pcv->next){
    if (pcv->vnum != ROOM_VNUM_CROOM_DUMMY && get_croom_cost(pcv->level) > 1)
      cvrooms[max_cvr++] = pcv;
  }
  if (max_cvr < 1)
    return;

  /* now we sort the rooms in order of decreasing cost */
  for (i = 1; i < max_cvr; i++){
    int j = 0;
    for (j = 0; j  < max_cvr - i; j ++ ){
      pcv = cvrooms[j];
      pcv_next = cvrooms[j + 1];

      if (get_croom_cost(pcv_next->level) > get_croom_cost(pcv->level)){
	cvrooms[j + 1] = pcv;
	cvrooms[j] = pcv_next;
      }
    }
  }
  sprintf( text, "Due to lack of funds for support of your cabal, the following rooms have been sold for profit.\n\r");
  /* now we sell the rooms untill we get pos. cp */
  i = 0;
  while (pCab->cp < 0 && i < max_cvr){
    int cost = get_croom_bcost( cvrooms[i]->level );
    sprintf( buf, "%d. %-25s sold for %-4d %s%s.\n\r", i + 1,
	     cvrooms[i]->pRoom->name, cost / 2, pCab->currency, cost == 1 ? "" : "s");
    strcat( text, buf );
    sell_room( cvrooms[i], pCab, cost );
    i++;
  }
  strcat( text, "\n\r" );

  do_hal( pCab->name, "Some of your rooms have been sold.", text, NOTE_NEWS );
  VRCHANGE_CABAL( pCab );
  CHANGE_CABAL( pCab );
}


int get_support_cp_bonus(CABAL_DATA* pCab, int support ){
  int bonus = 0;

  if (support > 50){
    bonus = support / 4;
  }
  else if (support < 0 )
    bonus = 10 * support;

  return bonus;
}

//checks if cabal member has enough hours to be promoted
bool check_promo_hours( CMEMBER_DATA* cMem, int rank ){

  if (cMem->hours / 3600 > (rank + 1) * 10)
    return TRUE;
  else
    return FALSE;
}

/* auto-promotes a character */
void auto_promote( CMEMBER_DATA* cMem ){
  CABAL_DATA* pc;
  DESCRIPTOR_DATA* d;
  CHAR_DATA* ch;
  int old_rank = 0, rank = 0;
  bool fPurge = FALSE;

  if (cMem == NULL)
    return;
  else
    pc = get_parent( cMem->pCabal );

  /* check if player is in the lands */
  if ( (ch = get_char( cMem->name )) == NULL){
    /* check if we can bring the player on */
    if ( (d = bring(get_room_index( ROOM_VNUM_LIMBO ), cMem->name)) == NULL){
      bug("auto_promote: Could not find the member pfile.", 0);
      return;
    }
    else{
      ch = d->character;
      /* we set the messages for login since he was not online */
      SET_BIT(ch->pcdata->messages, MSG_CAB_PROM);
      fPurge = TRUE;
    }
  }

  /* safeties in case of duplicate entry in two cabals */
  if (!is_same_cabal(cMem->pCabal, ch->pCabal)){
    cmember_from_cabal(cMem, get_parent( cMem->pCabal ) );
    purge( ch );
    return;
  }
  old_rank = ch->pcdata->rank;
  rank = promote(ch, ch->pCabal, 1); //saves cabals

  /* clear sponsor if any */
  if (  old_rank < rank && rank >= 2){
    CMEMBER_DATA* cm;
    if ( (cm = sponsor_check(pc, cMem->name )) != NULL){
      char buf[MIL];
      sprintf( buf, "Your obligations as a sponsor of %s have ended.\n\r", cMem->name);
      do_hal( cm->name, "End of Sponsorship.", buf, NOTE_NEWS);
      free_string(cm->sponsored);
      cm->sponsored = str_dup( "" );
    }
    if (!IS_NULLSTR(ch->pcdata->member->sponsor )){
      free_string( ch->pcdata->member->sponsor );
      ch->pcdata->member->sponsor = str_dup( "" );
    }
    CHANGE_CABAL( get_parent(cMem->pCabal) );
  }

  save_char_obj( ch );
  sendf( ch, "You have been %s to the rank of %s!\n\r",
	 "promoted",
	 ch->pcdata->true_sex == SEX_FEMALE ? ch->pCabal->pIndexData->franks[rank] : ch->pCabal->pIndexData->mranks[rank] );

  if (!fPurge){
    /* char is present */
    char buf[MIL];
    sprintf( buf, "%s has been %s to the rank of %s.\n\r",
	     PERS2( ch ),
	     "promoted",
	     get_crank( ch ));
    cabal_echo( ch->pCabal, buf );
    if (has_child_choice( ch->pCabal, ch->pcdata->rank )){
      send_to_char("`2You are now able to choose further within your organization. (\"cabal join\")``\n\r", ch);
    }
  }

  /* check if we need to purge the character */
  if (fPurge){
    update_cmember( ch );
    purge( ch );
  }
  save_cabals( TRUE, NULL );
}


/* used below in cabal_update */
bool member_update( CMEMBER_DATA* cMem, bool fPurge ){
  CABAL_DATA* pCab = get_parent( cMem->pCabal );

  if (fPurge && purge_cabal( cMem ))
    return FALSE;

  pCab->cur_member ++;
  if (cMem->rank == RANK_ELDER)
    pCab->cur_elder ++;
  else if (cMem->rank == RANK_LEADER)
    pCab->cur_leader ++;

  /* promotion check */
  if (cMem->rank < RANK_TRUSTED - 1
      && check_promo_hours( cMem, cMem->rank )
      && check_promo_req( NULL, cMem, cMem->rank + 1)){
    auto_promote( cMem );
    CHANGE_CABAL( get_parent(cMem->pCabal));
  }
  return TRUE;
}

/* handles army queue updates on cabal member */
bool handle_army_queue( CMEMBER_DATA* cMem ){
  CABAL_DATA* pCab = get_parent( cMem->pCabal );
  bool fAccess = FALSE;

  if (pCab == NULL)
    return FALSE;

  //EZ case.
  if (!cMem->connected)
    return fAccess;
  else if (pCab->pIndexData->army < 1  || pCab->pIndexData->army_upgrade < 1)
    return fAccess;

  //NORMAL QUEUE
  if (cMem->armies[NORQ]){
    ARMY_INDEX_DATA* pai;
    int start_dur = IS_CABAL(pCab, CABAL_SWARM) ? ARMY_DUR_SREC : ARMY_DUR_REC;

    if ( (pai = get_army_index( pCab->pIndexData->army)) == NULL)
      return fAccess;

    //Check for pause->unpause
    if (cMem->armies[TIMN] < 0){
      if ( cMem->cp >= pai->cost
	   && (cMem->armies[OTIN] != start_dur || pCab->armies_ready > 0)
	   && !check_army_max( cMem ))
	cMem->armies[TIMN] = UMAX(0, cMem->armies[OTIN]);
    }
    //Check for unpause->pause
    else if (cMem->cp < pai->cost
	     || (cMem->armies[TIMN] == start_dur && pCab->armies_ready < 1)
	     || check_army_max( cMem )){
      cMem->armies[OTIN] = cMem->armies[TIMN];
      cMem->armies[TIMN] = -1;
    }
    //do this only if unpaused
    if (cMem->armies[TIMN] >= 0){
      //check for a new units starting to be recruited
      if (cMem->armies[TIMN] == start_dur){
	pCab->armies_ready--;
	cMem->cp_pen -= pai->cost;
      }
      //recruit an army
      if (cMem->armies[TIMN]-- < 1){
	cMem->armies[NORQ]--;
	cMem->armies[NOR]++;
	cMem->garrison_changed = TRUE;
	fAccess = TRUE;

	//reset timer
	cMem->armies[TIMN] = start_dur;
      }
    }//End if not paused
  }//END NORMAL QUEUE


  //UPGRADE QUEUE
  if (cMem->armies[UPGQ]){
    ARMY_INDEX_DATA* pai;
    int start_dur = IS_CABAL(pCab, CABAL_SWARM) ? ARMY_DUR_SUPG : ARMY_DUR_UPG;

    if ( (pai = get_army_index( pCab->pIndexData->army_upgrade)) == NULL)
      return fAccess;

    //Check for pause->unpause
    if (cMem->armies[TIMU] < 0){
      if ( cMem->cp >= pai->cost
	   && (cMem->armies[OTIU] != start_dur || cMem->armies[NOR] > 0)){
	   //Viri: This should not be necessary, upgrades are limited by avaliable normals, which are limited by army max   && !check_army_max( cMem )){
	cMem->armies[TIMU] = UMAX(0, cMem->armies[OTIU]);
      }
    }
    //Check for unpause->pause
    else if (cMem->cp < pai->cost
	     || (cMem->armies[TIMU] == start_dur && cMem->armies[NOR] < 1)
	     ){
      cMem->armies[OTIU] = cMem->armies[TIMU];
      cMem->armies[TIMU] = -1;
    }
    //do this only if unpaused
    if (cMem->armies[TIMU] >= 0){
      //check for a new units starting to be recruited
      if (cMem->armies[TIMU] == start_dur){
	cMem->armies[NOR]--;
	cMem->garrison_changed = TRUE;
	cMem->cp_pen -= pai->cost;
      }
      //recruit an army
      if (cMem->armies[TIMU]-- < 1){
	cMem->armies[UPGQ]--;
	cMem->armies[UPG]++;
	cMem->garrison_changed = TRUE;

	//reset timer
	cMem->armies[TIMU] = start_dur;
      }
    }//End if not paused
  }//END NORMAL QUEUE
  return fAccess;
}

/* performs necessary cabal_tick updates on members of cabal */
void cabal_member_update( CABAL_DATA* pCab, bool fPurge ){
  CMEMBER_DATA* cMem, *cMem_next;

  /* reset totals so member_update can count them up */
  pCab->cur_member = 0;
  pCab->cur_elder = 0;
  pCab->cur_leader = 0;

  /*
     we do two loops, from last_recruit to end, then from start to last_recruit
     in order to implement simple clock alg. to prevent deadlock on army queues
  */

  /* LOOP  1, from last_recruit to end */
  if (pCab->last_recruit){
    for (cMem = pCab->last_recruit; cMem; cMem = cMem_next){
      cMem_next = cMem->next;

      if (!member_update( cMem, fPurge ))
	continue;
      /* check for army recruitment */
      handle_army_queue( cMem );
    }
  }
  /* LOOP 2, from start to last recruit */
  for (cMem = pCab->members; cMem != pCab->last_recruit; cMem = cMem_next){
    cMem_next = cMem->next;
    if (!member_update( cMem, fPurge ))
      continue;
    /* check for army recruitment */
    handle_army_queue( cMem );
  }
}
//resets standard gaurd
void reset_standard_guard( ROOM_INDEX_DATA* pRoom, CABAL_DATA* pCab ){
  CHAR_DATA* mob, *pMob;

  if (pRoom == NULL || pCab == NULL || pCab->anchor == NULL || pCab->guard == NULL)
    return;

  for (mob = pCab->anchor->people; mob != NULL; mob = mob->next_in_room){
    if (!IS_NPC(mob))
      continue;
    if (IS_SET(mob->off_flags, CABAL_GUARD) && mob->pIndexData->vnum == MOB_VNUM_CABAL_GUARD){
      if (pCab->citem == NULL)
	pCab->citem = reset_cabal_item( mob, pCab );
      return;
    }
  }

  pMob = create_mobile( get_mob_index(MOB_VNUM_CABAL_GUARD) );
  pMob->homevnum = pRoom->vnum;
  pMob->pCabal = pCab;
  pCab->cguard = pMob;

  if ( room_is_dark( pRoom ) )
    SET_BIT(pMob->affected_by, AFF_INFRARED);
  char_to_room( pMob, pRoom );
  act("$n takes position before the entrance.", pMob, NULL, NULL, TO_ROOM);
  if (pCab->citem == NULL)
    pCab->citem = reset_cabal_item( pMob, pCab );
}

//removes the standard guard mob from cabal entrance.
void remove_standard_guard( CABAL_DATA* pCab ){
  CHAR_DATA* mob;

  if (pCab == NULL)
    return;
  for (mob = pCab->anchor->people; mob != NULL; mob = mob->next_in_room){
    if (IS_NPC(mob) && mob->pIndexData->vnum == MOB_VNUM_CABAL_GUARD){
      break;
    }
  }
  if (mob == NULL)
    return;
  act("$n leaves $s post heading for the nearest tavern.", mob, NULL, NULL, TO_ROOM);
  char_from_room( mob );
  extract_char( mob, TRUE );
}

//resets the cabal guard
void reset_cabal_guard( ROOM_INDEX_DATA* pRoom, CABAL_DATA* pCab ){
  CHAR_DATA* pMob, *mob;

  if (pRoom == NULL || pCab == NULL || pCab->anchor == NULL || pCab->guard == NULL)
    return;
  pRoom->pCabal = pCab;

  remove_standard_guard( pCab );

  for (mob = pCab->anchor->people; mob != NULL; mob = mob->next_in_room){
    if (!IS_NPC(mob))
      continue;
    if (pCab->guard && mob->pIndexData == pCab->guard){
      if (pCab->citem == NULL)
	pCab->citem = reset_cabal_item( mob, pCab );
      return;
    }
  }

  pMob = create_mobile( pRoom->pCabal->guard );
  pMob->homevnum = pRoom->vnum;
  pMob->pCabal = pCab;
  pCab->cguard = pMob;

  if ( room_is_dark( pRoom ) )
    SET_BIT(pMob->affected_by, AFF_INFRARED);
  char_to_room( pMob, pRoom );
  act("$n takes position before the entrance.", pMob, NULL, NULL, TO_ROOM);

  if (pCab->citem == NULL)
    pCab->citem = reset_cabal_item( pMob, pCab );
}
//removes the cabal guard mob from cabal entrance.
void remove_cabalguard( CABAL_DATA* pCab ){
  CHAR_DATA* mob;
  if (pCab->guard == NULL)
    return;
  for (mob = pCab->anchor->people; mob != NULL; mob = mob->next_in_room){
    if (IS_NPC(mob) && mob->pIndexData == pCab->guard){
      break;
    }
  }
  if (mob != NULL){
    act("$n leaves $s post heading for the nearest tavern.", mob, NULL, NULL, TO_ROOM);
    char_from_room( mob );
    extract_char( mob, TRUE );
  }
  reset_standard_guard( pCab->anchor, pCab );
}

/* Cabal UPDATE FUNCTION, updates all cabal stats using various routines */
void cabal_update(){
  CABAL_DATA* pCab;
  //CVROOM_DATA* pcv;
  //CVROOM_DATA* croom_levels[CROOM_LEVELS];
  //AREA_DATA* pArea;
  int i;


  /* reset trade and flags for all cabals */
  for (pCab = cabal_list; pCab; pCab = pCab->next){
    pCab->support[ECO_TRADE] = 0;
    pCab->cp_gain[ECO_TRADE] = 0;
    //allows cabal to grab new area on next area tick
    REMOVE_BIT(pCab->flags2, CABAL2_AREAGAIN);
  }

  /* update the pacts before cabals */
  pact_update();

  /* run through all the cabals */
  for (pCab = cabal_list; pCab; pCab = pCab->next){
    int gain = 0, item_count = 0;

    /* child cabals are not updated */
    if (pCab->parent != NULL)
      continue;

    /* reset cabal item if its gone */
    if (pCab->citem == NULL){
      if (IS_CABAL(pCab, CABAL_GUARDMOB))
	reset_cabal_guard( pCab->anchor, pCab );
      else
	reset_standard_guard( pCab->anchor, pCab );
      if (pCab->citem != NULL)
	cabal_echo( pCab, "Our Standard has been returned to us.");
    }
    /* check for items on floor */
    handle_cabal_item_floor( pCab->cguard  );

    /* count how many items we posses */
    if (pCab->altar){
      OBJ_DATA* obj;
      for (obj = pCab->altar->carrying; obj; obj = obj->next_content){
	if (obj->item_type == ITEM_CABAL && obj->pCabal){
	  if (!is_same_cabal(pCab, obj->pCabal))
	    item_count++;
	}
      }
    }

    /* move cps and support from raides into cabal data */
    if (abs(pCab->raid_cps) >= 10 * CPTS || abs(pCab->raid_sup) >= 10){
      char buf[MIL];
      CHAR_DATA* vch;
      sprintf( buf, "We've %s %d %ss and %s %d support in recent raids.",
	       pCab->raid_cps < 0 ? "lost" : "won",
	       abs(pCab->raid_cps / CPTS),
	       pCab->currency,
	       pCab->raid_sup < 0 ? "lost" : "won",
	       abs(pCab->raid_sup));
      if (pCab->raid_sup < 0)
	pCab->bonus_sup += pCab->raid_sup;
      CPS_CAB_GAIN( pCab, pCab->raid_cps);
      cabal_echo(pCab, buf );

      /* watcher members get some cps whenever raids destroy stuff */
      if (!str_cmp(pCab->name, "watcher")){
	for (vch = player_list; vch; vch = vch->next_player){
	  if (vch->pCabal && is_same_cabal(vch->pCabal, pCab))
	    CPS_GAIN(vch, pCab->raid_cps / 5, FALSE );
	}
      }
      pCab->raid_sup = pCab->raid_cps = 0;
    }

    /* ECONOMY PART */
    //reset support income

    /* if we have our item, we get full support, otherwise 0 */
    if (is_in_altar(pCab)){
      pCab->support[ECO_INCOME] = CABAL_FULL_SUPPORT * WORLD_SUPPORT_VALUE / 100;
    }
    else if (is_captured( pCab )){
      pCab->support[ECO_INCOME] = 0;
    }

    /* NOT NEEDED
    // run through the areas, and add up support
    for (pArea = area_first; pArea; pArea = pArea->next ){
      if (pArea->pCabal == pCab)
	pCab->support[ECO_INCOME] += pArea->support;
    }
    */

    // Increase recruits ready in cabal if we have our own item
    if (!is_captured( pCab )){
      pCab->ready_gain += IS_CABAL( pCab, CABAL_SWARM ) ? 6 : 3;
      //    give more units to those without much land
      if (pCab->support[ECO_INCOME] < WORLD_SUPPORT_VALUE / 8)
	pCab->ready_gain += 6 * (1 - 8 * pCab->support[ECO_INCOME] / WORLD_SUPPORT_VALUE);
      gain = pCab->ready_gain / 10;
      pCab->ready_gain -= gain * 10;
      pCab->armies_ready = URANGE(0, pCab->armies_ready + gain, 10);
    }
    // if no item in altar, decrease the recuits
    else
      pCab->armies_ready = URANGE(0, pCab->armies_ready -1, 10);


    /* MEMBER update if needed */
    if (mud_data.current_time - pCab->member_stamp > CMEMBER_PULSE){
      //check with purge
      cabal_member_update( pCab, TRUE );
      pCab->member_stamp = mud_data.current_time;
    }
    else
      cabal_member_update( pCab, FALSE );

    /* NOT NEEDED
    //reset the crooms
    for (i = 0; i < CROOM_LEVELS; i++){
      croom_levels[i] = NULL;
    }

    pCab->support[ECO_LAST_RENT] = 0;

    //run through the rooms and add up cost of rent
    for (pcv = pCab->cvrooms; pcv; pcv = pcv->next){
      if (!pcv->fComplete)
	continue;
      croom_levels[pcv->level] = pcv;
      //charge only for active rooms
      if (croom_table[pcv->level].flags == 0
	  || IS_CABAL(pCab, croom_table[pcv->level].flags))
	pCab->support[ECO_LAST_RENT] += get_croom_cost(pcv->level);
    }

    //get corruption
    pCab->support[ECO_LAST_CORR] = get_corruption_support( pCab, pCab->support[ECO_INCOME] );
    //make sure bonus support is right

    //support bonus for having item in altar
    if (is_in_altar( pCab )){
      if (IS_CABAL(pCab, CABAL_ROYAL)
	  && pCab->city && IS_AREA(pCab->city, AREA_LAWFUL)){
	pCab->support[ECO_INCOME]	+= 2 * CABAL_BASE_SUPPORT;//LAWFUL
      }
      else
	pCab->support[ECO_INCOME] += CABAL_BASE_SUPPORT;
     }
    */

    /* bonus support gain/loss */
    if (is_captured(pCab)){
      pCab->bonus_sup -= 10;
    }
    else if (item_count > 0 && is_in_altar(pCab) && pCab->bonus_sup < CABAL_FULL_SUPPORT_VAL + 500 * item_count){
      pCab->bonus_sup += 10;
    }
    else if (pCab->bonus_sup > 0){
      pCab->bonus_sup -= 1;
    }
    else if (pCab->bonus_sup < 0){
      pCab->bonus_sup += URANGE(1, abs(pCab->bonus_sup) / 100, 20);
    }

    /* set range */
    pCab->bonus_sup = URANGE( -CABAL_FULL_SUPPORT_VAL, pCab->bonus_sup, 2 * CABAL_FULL_SUPPORT_VAL);

    //store the bonus
    pCab->support[ECO_LAST_BONUS] = pCab->bonus_sup;

    //Get total income
    pCab->support[ECO_GAIN]  = pCab->support[ECO_INCOME];
    pCab->support[ECO_GAIN] += pCab->support[ECO_TRADE];
    pCab->support[ECO_GAIN] += pCab->support[ECO_LAST_BONUS];
    pCab->support[ECO_GAIN] += pCab->support[ECO_LAST_ARMY];
    pCab->support[ECO_GAIN] -= pCab->support[ECO_LAST_RENT];
    pCab->support[ECO_GAIN] -= pCab->support[ECO_LAST_CORR];



    //Cp gain is what support gain is
    pCab->cp_gain[ECO_INCOME] = pCab->support[ECO_GAIN] * CPTS / 1000;

    pCab->cp_gain[ECO_GAIN] = 0;
    pCab->cp_gain[ECO_GAIN] += pCab->cp_gain[ECO_INCOME];
    pCab->cp_gain[ECO_GAIN] += pCab->cp_gain[ECO_TRADE];

    /* ROOM ACTIVATION/SHUTDOWN */
    /* new */
    {
      int temp_level = pCab->support[ECO_GAIN];
      int percent = 100 * temp_level / (CABAL_FULL_SUPPORT * WORLD_SUPPORT_VALUE / 100);
      char buf[MIL];
      /* deactivate all rooms above current percent level */

      for (i = 0; i < CROOM_LEVELS; i++ ){
	if (croom_table[i].flags == 0)
	  continue;
	if (croom_table[i].percent < 1)
	  continue;

	/* activate */
	if (percent > croom_table[i].percent && !IS_SET(pCab->flags, croom_table[i].flags)){
	  SET_BIT(pCab->flags, croom_table[i].flags);
	  if (pCab->pIndexData->crooms[i]){
	    sprintf( buf, "Reactivated: %s.",pCab->pIndexData->crooms[i]->pRoomIndex->name);
	    cabal_echo( pCab, buf );
	  }
	  if (IS_SET(croom_table[i].flags, CABAL_GUARDMOB) && pCab->anchor)
	    reset_cabal_guard(pCab->anchor, pCab );
	}
	/* deactivate */
	else if (percent < croom_table[i].percent && IS_SET(pCab->flags, croom_table[i].flags)){
	  REMOVE_BIT(pCab->flags, croom_table[i].flags);
	  if (pCab->pIndexData->crooms[i]){
	    sprintf( buf, "Not enough support to activate %s.",
		     pCab->pIndexData->crooms[i]->pRoomIndex->name);
	    cabal_echo( pCab, buf );
	  }
	}
      }
    }

    /* OLD (disabled)
    {
      int temp_level = pCab->support[ECO_GAIN];
      char buf[MIL];

      if (temp_level < 0){
	//count down from highest level and turn rooms off untill we
	//achive positive gain
	for (i = CROOM_LEVELS - 1; i >= 0; i-- ){
	  //rooms with no cabal flags cannot be turned off
	  if (croom_table[i].flags == 0)
	    continue;
	  //rooms already turned off are skipped
	  else if (!IS_SET(pCab->flags, croom_table[i].flags))
	    continue;
	  else
	    temp_level += get_croom_cost( i );

	  //turn off its bits
	  REMOVE_BIT(pCab->flags, croom_table[i].flags);

	//message
	sprintf( buf, "Not enough support to activate %s.",
	croom_levels[i]->pRoom->name);
	cabal_echo( pCab, buf );

	if (temp_level >= 0)
	break;
	}
      }

      //POSITIVE SUPPORT LEVEL, TRY TO TURN ON ROOMS THAT ARE OFF (from cheapest to highest)
      for (i = 0; i < CROOM_LEVELS; i++ ){
	//rooms with no cabal flags cannot be turned on
	if (croom_table[i].flags == 0)
	  continue;
	//rooms already turned on are skipped
	else if (IS_SET(pCab->flags, croom_table[i].flags))
	  continue;
	//turn room only if we have support for it
	else if (temp_level - get_croom_cost( i ) < 0)
	  continue;
	else
	  temp_level -= get_croom_cost( i );

	SET_BIT(pCab->flags, croom_table[i].flags);
	if (pCab->pIndexData->crooms[i]){
	  sprintf( buf, "Reactivated: %s.",pCab->pIndexData->crooms[i]->pRoomIndex->name);
	  cabal_echo( pCab, buf );
	}
	if (IS_SET(croom_table[i].flags, CABAL_GUARDMOB) && pCab->anchor)
	  reset_cabal_guard(pCab->anchor, pCab );
      }
    }//end room cycle
    */
    //guard removal
    if (!IS_SET(pCab->flags, CABAL_GUARDMOB) || is_captured( pCab ))
      remove_cabalguard(pCab);
    /* we now have the cps gain, we calculate the actual gain per update */
    /* (current time - last time) / (time/gain) = gain */
    gain = (pCab->cp_gain[ECO_GAIN] * (mud_data.current_time - pCab->time_stamp) / ECONOMY_UPDATE);
    if (gain != 0){
      CPS_CAB_GAIN( pCab, gain );
      pCab->time_stamp = mud_data.current_time;
      CHANGE_CABAL( pCab );
    }

    /* see if we shoud do a room check */
    if (mud_data.current_time - pCab->room_stamp > CABAL_ROOM_PULSE){
      /* INSERT ROOM CHECK HERE */
      pCab->room_stamp = mud_data.current_time;
      /* check if we have negative cps */
      if (pCab->cp < ECONOMY_MIN){
	sell_rooms( pCab );
      }
    }
  }
}


/* shows cabal requirements */
void show_cabal_help( CHAR_DATA* ch, char* argument, char* output ){
  CABAL_DATA* pCab = get_cabal( argument);
  CABAL_DATA* pPar = get_parent( pCab );
  VOTE_DATA* pv;
  char buf[MSL], string[MSL];
  int i = 0;
  int votes = 0;


  if (pCab == NULL)
    return;
  else
    sprintf( buf, "\n\r\n\r[%s] ENTRANCE REQUIREMENTS:\n\r\n\r", pCab->who_name);
  strcat( output, buf );

  buf[0] = '\0';
  if (IS_SET(pCab->align, CALIGN_GOOD)){
    strcat(buf, "Good");
    if (IS_SET(pCab->align, CALIGN_NEUTRAL) || IS_SET(pCab->align, CALIGN_EVIL)){
      strcat(buf, "\\");
    }
  }
  if (IS_SET(pCab->align, CALIGN_NEUTRAL)){
    strcat(buf, "Neutral");
    if (IS_SET(pCab->align, CALIGN_EVIL)){
      strcat(buf, "\\");
    }
  }
  if (IS_SET(pCab->align, CALIGN_EVIL))
    strcat(buf, "Evil");
  sprintf( string, "You must be between %d and %d in your rank and of %s alignment.\n\r",
	 pCab->pIndexData->min_lvl, pCab->pIndexData->max_lvl, buf );
  strcat( output, string );

  buf[0] = '\0';
  if (IS_SET(pCab->ethos, ETHOS_LAWFUL)){
    strcat(buf, "Lawful");
    if (IS_SET(pCab->ethos, ETHOS_NEUTRAL) || IS_SET(pCab->ethos, ETHOS_CHAOTIC)){
      strcat(buf, "\\");
    }
  }
  if (IS_SET(pCab->ethos, ETHOS_NEUTRAL)){
    strcat(buf, "Neutral");
    if (IS_SET(pCab->ethos, ETHOS_CHAOTIC)){
      strcat(buf, "\\");
    }
  }
  if (IS_SET(pCab->ethos, ETHOS_CHAOTIC))
    strcat(buf, "Chaotic");
  sprintf( string, "Following %s races and classes will be considered:\n\r\n\r", buf );
  strcat( output, string );


  if (pCab->race[0] < 0){
    sprintf(buf, "Races: All");
  }
  else{
    sprintf(buf, "Race: ");
    for (i = 0; i < MAX_PC_RACE; i++){
      char temp[MIL];
      if (pCab->race[i] > 0){
	sprintf(temp, "%-5s ", race_table[i].name);
	strcat(buf, temp);
      }
    }
  }
  strcat( output, buf );
  strcat( output, "\n\r");

  if (pCab->class[0] < 0){
    sprintf(buf, "Class: All");
  }
  else{
    sprintf(buf, "Class: ");
    for (i = 0; i < MAX_CLASS; i++){
      char temp[MIL];
      if (pCab->class[i] > 0 ){
	sprintf(temp, "%-5s ", class_table[i].who_name);
	strcat(buf, temp);
      }
    }
  }
  strcat( output, buf );
  strcat( output, "\n\r\n\r");


  /* count application votes to this cabal */
  for (pv = vote_list; pv; pv = pv->next ){
    if (pv->type == VOTE_CAB_APPLY && pPar->vnum == pv->value[0])
      votes++;
  }

  if ( pCab->cur_member + votes < pCab->max_member){
    if (pCab != pPar && pCab->pIndexData->min_rank && pCab->pIndexData->min_rank < RANK_MAX)
      sprintf( buf, "You must hold the rank of %s of %s before being able to join %s.\n\r",
	       get_crank_str( pPar, ch->pcdata->true_sex, pCab->pIndexData->min_rank ),
	       pPar->who_name,
	       pCab->who_name);
    else
      sprintf(buf, "Scribe an \"application to %s\" if you wish to be considered for membership.\n\r",
	      pCab->name);
  }
  else
    sprintf(buf, "%s is currently NOT ACCEPTING applications.\n\r",
	    pPar->name);
  strcat( output, buf );
  strcat( output, "\n\rSee also: HELP CABAL, CABALS\n\r");

}

/* sets the beggining of a coup */
void do_coup(CHAR_DATA *ch, char *argument){
  CHAR_DATA* victim;
  AFFECT_DATA af, *paf;
  char arg[MIL];
  argument = one_argument( argument, arg );

  if (ch->pCabal == NULL)
    return;
  else if (!IS_SET(ch->pCabal->progress, PROGRESS_COUP)){
    send_to_char("Your organization does not tolerate coups.\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(arg)){
    send_to_char("Attempt a coup against whom?\n\r", ch);
    return;
  }
  else if (is_affected(ch, gen_coup)){
    send_to_char("One coup at a time.\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument) || str_cmp( "confirm", argument)){
    sendf(ch, "Are you sure you wish to attempt a coup on %s?\n\r(\"coup <target> confirm\" to confirm.\n\r", arg );
    return;
  }
  if (GET_CP(ch) < 50){
    sendf(ch, "You must be able to expand 50 %ss upon various 'fees' required by a coup.\n\r",  ch->pCabal->currency);
    return;
  }
  CP_GAIN(ch, -50, TRUE );
  if ( (victim = get_char( arg)) == NULL){
    send_to_char("They do not seem to be present.\n\r", ch);
    return;
  }
  else if (!is_same_cabal(ch->pCabal, victim->pCabal)){
    send_to_char("The coup may be attempted only against member of your own cabal!\n\r", ch );
    CP_GAIN(ch, 50, TRUE );
    return;
  }
  else if (IS_NPC(victim)){
    send_to_char("Not on mobs.\n\r", ch);
    CP_GAIN(ch, 50, TRUE );
    return;
  }
  else if (victim->pcdata->rank <= ch->pcdata->rank){
    send_to_char("What is the point of that?\n\r", ch);
    CP_GAIN(ch, 50, TRUE );
    return;
  }
  else if (ch->pcdata->rank < RANK_ELDER - 1 && IS_ELDER(victim)){
    send_to_char("You can coup Elders and Leaders from rank of Trusted only.\n\r", ch);
    CP_GAIN(ch, 50, TRUE );
    return;
  }
  af.type	= gen_coup;
  af.level	= 60;
  af.duration	= 72;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  paf = affect_to_char( ch, &af );
  string_to_affect( paf, victim->name );
  sendf(ch, "You have %d hours to complete the coup.\n\r", paf->duration);
}

/* calculates corruption for a cabal */
int get_corruption_support( CABAL_DATA* pCabal, int area_support ){
  PACT_DATA* pp;
  int pacts = 0;
  /* percent of area support lost without trade */
  int per_loss = 150 * area_support / WORLD_SUPPORT_VALUE;

  /* count how many alliances we got */
  for (pp = pact_list; pp; pp = pp->next){
    if (pp->complete != PACT_COMPLETE)
      continue;
    if (pp->type != PACT_TRADE && pp->type != PACT_NAPACT && pp->type != PACT_ALLIANCE)
      continue;
    if (pp->benefactor == pCabal && IS_CABAL(pp->creator, CABAL_ROYAL) )
      pacts++;
    else if (pp->creator == pCabal && IS_CABAL(pp->benefactor, CABAL_ROYAL))
      pacts++;
    else if (pp->benefactor == pCabal && IS_CABAL(pp->creator, CABAL_JUSTICE) )
      pacts += 6;
    else if (pp->creator == pCabal && IS_CABAL(pp->benefactor, CABAL_JUSTICE))
      pacts += 6;
  }
  /* lower the loss by 15% for each ROYAL/JUSTICE alliance */
  per_loss = UMAX(0, per_loss - (15 * pacts));

  //  return (per_loss * area_support / 100 );
  return 0;
}


/* Written by: Virigoth							*
 * Returns: void							*
 * Used: fight.c							*
 * comment:  resets the cabal respawn eq (based on cabal_table) onto ch *
 * based on their CP and autorequip flag.				*/

void cabal_requip(CHAR_DATA* ch){
//  OBJ_DATA* obj;
  int cp = 0;
//  int vnum = 0;
  bool fDone = FALSE;

  if (IS_NPC(ch) || !ch->pCabal)
    return;
  if (IS_GAME(ch, GAME_NOREQ_CABAL))
    return;

  cp = GET_CP(ch);
  /* run down the minimum costs and reset eq. */

  /* WEAPON */

/* THIS NEEDS TO BE RE-WRITTENT! */
  return;
  if (cp >= CP_RESPAWN_1){
    /* check for vnum
    if ( !(vnum  = cabal_table[ch->cabal].respawn[0])){
      nlogf("cabal_requip: %s has no respawn[0]", cabal_table[ch->cabal].name);
      return;
    }
    */
    /* check for item
    if ( (obj = create_object( get_obj_index( vnum), 0)) == NULL){
      nlogf("cabal_requip: %d does not exists.", vnum);
      return;
    }
    */
    /* move item to char, and equip it
    obj_to_char(obj, ch);
    if (!CAN_WEAR(obj, ITEM_WIELD)){
      bug("cabal_requip: RESPAWN_1 is not a weapon!", 0);
      extract_obj(obj);
      return;
    }
    fDone = TRUE;
    equip_char(ch, obj, WEAR_WIELD);
    */
  }

  /* TORSO */
  if (cp >= CP_RESPAWN_2){
    /* check for vnum
    if ( !(vnum  = cabal_table[ch->cabal].respawn[1])){
      nlogf("cabal_requip: %s has no respawn[1]", cabal_table[ch->cabal].name);
      return;
    }
    */
    /* check for item
    if ( (obj = create_object( get_obj_index( vnum), 0)) == NULL){
      nlogf("cabal_requip: %d does not exists.", vnum);
      return;
    }
    */
    /* move item to char, and equip it
    obj_to_char(obj, ch);
    if (!CAN_WEAR(obj, ITEM_WEAR_BODY)){
      bug("cabal_requip: RESPAWN_2 is not a torso!", 0);
      extract_obj(obj);
      return;
    }
    fDone = TRUE;
    equip_char(ch, obj, WEAR_BODY);
    */
  }

    /* HEAD */
  if (cp >= CP_RESPAWN_3){
    /* check for vnum
    if ( !(vnum  = cabal_table[ch->cabal].respawn[2])){
      nlogf("cabal_requip: %s has no respawn[2]", cabal_table[ch->cabal].name);
      return;
    }
    */
    /* check for item
    if ( (obj = create_object( get_obj_index( vnum), 0)) == NULL){
      nlogf("cabal_requip: %d does not exists.", vnum);
      return;
    }
    */
    /* move item to char, and equip it
    obj_to_char(obj, ch);
    if (!CAN_WEAR(obj, ITEM_WEAR_HEAD)){
      bug("cabal_requip: RESPAWN_3 is not a helm!", 0);
      extract_obj(obj);
      return;
    }
    fDone = TRUE;
    equip_char(ch, obj, WEAR_HEAD);
    */
  }

  /* ARMS */
  if (cp >= CP_RESPAWN_4){
    /* check for vnum
    if ( !(vnum  = cabal_table[ch->cabal].respawn[3])){
      nlogf("cabal_requip: %s has no respawn[3]", cabal_table[ch->cabal].name);
      return;
    }
    */
    /* check for item
    if ( (obj = create_object( get_obj_index( vnum), 0)) == NULL){
      nlogf("cabal_requip: %d does not exists.", vnum);
      return;
    }
    */
    /* move item to char, and equip it
    obj_to_char(obj, ch);
    if (!CAN_WEAR(obj, ITEM_WEAR_LEGS)){
      bug("cabal_requip: RESPAWN_4 is not a leg armor!", 0);
      extract_obj(obj);
      return;
    }
    fDone = TRUE;
    equip_char(ch, obj, WEAR_LEGS);
    */
  }

  /* LIGHT */

  if (cp >= CP_RESPAWN_5 && ch->race != race_lookup("vampire")){
    /* check for vnum
    if ( !(vnum  = cabal_table[ch->cabal].respawn[4])){
      nlogf("cabal_requip: %s has no respawn[4]", cabal_table[ch->cabal].name);
      return;
    }
    */
    /* check for item
    if ( (obj = create_object( get_obj_index( vnum), 0)) == NULL){
      nlogf("cabal_requip: %d does not exists.", vnum);
      return;
    }
    */
    /* move item to char, and equip it
    obj_to_char(obj, ch);
    if (!CAN_WEAR(obj, ITEM_LIGHT)){
      bug("cabal_requip: RESPAWN_5 is not a light!", 0);
      extract_obj(obj);
      return;
    }
    fDone = TRUE;
    equip_char(ch, obj, WEAR_LIGHT);
    */
  }

  if (fDone)
    sendf(ch, "You have been re-equiped by forces of %s.\n\r",
	  ch->pCabal->who_name);
}



/* write single cabal index to file */
void fwrite_cabal_index( FILE* fp, CABAL_INDEX_DATA* pCab){
  CSKILL_DATA* cSkill;
  CVOTE_DATA* cVote;
  CROOM_DATA* cRoom;
  char buf[MIL];
  int i = 0;


  fprintf( fp, "#%d\n", pCab->vnum);
  fprintf( fp, "VERSION %d\n", SAVECAB_VER );

  if (!IS_NULLSTR(pCab->file_name) )
    fprintf( fp, "FileName %s~\n", pCab->file_name);
  if (!IS_NULLSTR(pCab->name) )
    fprintf( fp, "Name %s~\n", pCab->name);
  if (!IS_NULLSTR(pCab->who_name) )
    fprintf( fp, "WhoName %s~\n", pCab->who_name );
  if (!IS_NULLSTR(pCab->immortal) )
    fprintf( fp, "Imm %s~\n", pCab->immortal );
  if (!IS_NULLSTR(pCab->currency) )
    fprintf( fp, "Currency %s~\n", pCab->currency );
  if (!IS_NULLSTR(pCab->city_name) )
    fprintf( fp, "City %s~\n", pCab->city_name );
  if (!IS_NULLSTR(pCab->enemy_name) )
    fprintf( fp, "Enemy %s~\n", pCab->enemy_name );
  if (!IS_NULLSTR(pCab->gate_on))
    fprintf( fp, "OnGate %s~\n", pCab->gate_on);
  if (!IS_NULLSTR(pCab->gate_off))
    fprintf( fp, "OffGate %s~\n", pCab->gate_off);
  if (!IS_NULLSTR(pCab->gate_msg))
    fprintf( fp, "MsgGate %s~\n", pCab->gate_msg );
  if (!IS_NULLSTR(pCab->mprefix))
    fprintf( fp, "PrefixM %s~\n", pCab->mprefix);
  if (!IS_NULLSTR(pCab->fprefix))
    fprintf( fp, "PrefixF %s~\n", pCab->fprefix);
  if (!IS_NULLSTR(pCab->sprefix))
    fprintf( fp, "PrefixS %s~\n", pCab->sprefix);
  if (!IS_NULLSTR(pCab->clan))
    fprintf( fp, "Clan %s~\n", pCab->clan);

  if (pCab->parent_vnum){
    fprintf( fp, "Parent %d\n", pCab->parent_vnum );
  }
  if (pCab->anchor_vnum)
    fprintf( fp, "Anchor %d\n", pCab->anchor_vnum );
  if (pCab->guard_vnum)
    fprintf( fp, "Guard %d\n", pCab->guard_vnum );
  if (pCab->pit_vnum)
    fprintf( fp, "Pit %d\n", pCab->pit_vnum );
  if (pCab->army)
    fprintf( fp, "Army %d\n", pCab->army);
  if (pCab->army_upgrade)
    fprintf( fp, "Army2 %d\n", pCab->army_upgrade);
  if (pCab->tower)
    fprintf( fp, "Tower %d\n", pCab->tower);
  if (pCab->tower_upgrade)
    fprintf( fp, "Tower2 %d\n", pCab->tower_upgrade);
  if (pCab->max_room)
    fprintf( fp, "MaxRoom %d\n", pCab->max_room);

  fprintf( fp, "MinRank %d\n", pCab->min_rank);
  fprintf( fp, "MaxMem %d\n", pCab->max_member );
  fprintf( fp, "MinLvl %d MaxLvl %d\n", pCab->min_lvl, pCab->max_lvl );

  fprintf( fp, "Flags %s\n",  fwrite_flag( pCab->flags, buf ) );
  fprintf( fp, "Flags2 %s\n", fwrite_flag( pCab->flags2, buf ) );
  fprintf( fp, "Align %s\n", fwrite_flag( pCab->align, buf ) );
  fprintf( fp, "Ethos %s\n", fwrite_flag( pCab->ethos, buf ) );

  fprintf( fp, "Progress %s\n", fwrite_flag( pCab->progress, buf ) );

  fprintf( fp, "MRanks %d ", RANK_MAX);
  for(i = 0; i < RANK_MAX; i++){
    fprintf( fp, "%s~ ", pCab->mranks[i] );
  }
  fprintf( fp, "\n" );
  fprintf( fp, "FRanks %d ", RANK_MAX);
  for(i = 0; i < RANK_MAX; i++){
    fprintf( fp, "%s~ ", pCab->franks[i] );
  }
  fprintf( fp, "\n" );


  fprintf( fp, "Race %d ", MAX_PC_RACE);
  for(i = 0; i < MAX_PC_RACE; i++){
    fprintf( fp, "%d ", pCab->race[i] );
  }
  fprintf( fp, "\n" );

  if (pCab->fAvatar)
    fprintf( fp, "Avatar %d\n", pCab->fAvatar);

  fprintf( fp, "Class %d ", MAX_CLASS);
  for(i = 0; i < MAX_CLASS; i++){
    fprintf( fp, "%d ", pCab->class[i] );
  }
  fprintf( fp, "\n" );


  for( cSkill = pCab->skills; cSkill; cSkill = cSkill->next){
    // regular skill data part
    fprintf( fp, "Skill '%s' %3d %3d %3d %3d\n",
	     skill_table[cSkill->pSkill->sn].name,
	     cSkill->pSkill->level,
	     cSkill->pSkill->rating,
	     cSkill->pSkill->learned,
	     cSkill->pSkill->keep);
    //cabal part
    fprintf( fp, "%d %d %d %d %s\n",
	     cSkill->min_rank,
	     cSkill->max_rank,
	     cSkill->min_cost,
	     cSkill->cost,
	     fwrite_flag( cSkill->flag, buf ) );
  }


  for( cVote = pCab->votes; cVote; cVote = cVote->next){
    if (cVote->vote == VOTE_CAB_PROM)
      cVote->min = cVote->max = 5;
    if (cVote->vote == VOTE_CAB_ELDER)
      cVote->min = cVote->max = 5;
    if (cVote->vote == VOTE_CAB_LEADER)
      cVote->min = cVote->max = 5;
    fprintf( fp, "Vote %s~ %d %d %d\n", vote_table[cVote->vote].name, cVote->min, cVote->max, cVote->cost);
  }


  for( i = 0; i < CROOM_LEVELS; i++){
    if ( (cRoom = pCab->crooms[i]) == NULL)
      continue;
    fprintf( fp, "CRoom %d %d\n",
	     i,
	     cRoom->pRoomIndex->vnum);
  }
  fprintf( fp, "End\n");
}

/* reads a single cabal index from file into a structure */
bool fread_cabal_index( FILE *fp, CABAL_INDEX_DATA* pCab, AREA_DATA* pArea, int vnum ){
  char *word = NULL;
  bool fMatch = FALSE;

  for ( ; ; ){
    word   = feof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER(word[0]) ){
    case '*':
      fMatch = TRUE;
      fread_to_eol( fp );
      break;
    case 'A':
      KEY( "Align",		pCab->align,		fread_flag(fp)	   );
      KEY( "Anchor",		pCab->anchor_vnum,	fread_number(fp)   );
      KEY( "Army",		pCab->army,		fread_number(fp)   );
      KEY( "Army2",		pCab->army_upgrade,	fread_number(fp)   );
      KEY( "Avatar",		pCab->fAvatar,		fread_number(fp)   );
      break;
    case 'C':
      KEYS( "City",		pCab->city_name,	fread_string( fp ) );
      if (!str_cmp( word, "Class" )){
	int size = fread_number( fp );
	int i = 0;
	for(i = 0; i < size && i < MAX_CLASS; i++){
	  pCab->class[i] = fread_number( fp );
	}
	fMatch = TRUE;
	break;
      }
      if (!str_cmp( word, "CRoom" )){
	int i			= fread_number( fp );
	pCab->crooms[i]		= new_croom();
	pCab->crooms[i]->vnum	= fread_number( fp );
	pCab->crooms[i]->level	= i;

	fMatch = TRUE;
	break;
      }
      KEYS( "Currency",		pCab->currency,		fread_string( fp ) );
      KEYS( "Clan",		pCab->clan,		fread_string( fp ) );
      break;
    case 'E':
      KEYS( "Enemy",		pCab->enemy_name,	fread_string( fp ) );
      KEY( "Ethos",		pCab->ethos,		fread_flag(fp)	   );
      if ( !str_cmp( word, "End" ) ){
	pCab->pArea = pArea;
	pCab->vnum = vnum;
	return TRUE;
      }
      break;
    case 'F':
      KEYS( "FileName",		pCab->file_name,		fread_string( fp ) );
      KEY( "Flags",		pCab->flags,		fread_flag(fp)	   );
      KEY( "Flags2",		pCab->flags2,		fread_flag(fp)	   );
      if (!str_cmp( word, "FRanks" )){
	int size = fread_number( fp );
	int i = 0;
	for(i = 0; i < size && i < RANK_MAX; i++){
	  if (!IS_NULLSTR(pCab->franks[i])){
	    free_string( pCab->franks[i] );
	  }
	  pCab->franks[i] = fread_string( fp );
	}
	fMatch = TRUE;
	break;
      }
      break;
    case 'G':
      KEY( "Guard",		pCab->guard_vnum,	fread_number(fp)   );
      break;
    case 'I':
      KEYS( "Imm",		pCab->immortal,		fread_string( fp ) );
      break;
    case 'M':
      KEY( "MaxLvl",		pCab->max_lvl,		fread_number( fp ) );
      KEY( "MaxMem",		pCab->max_member,	fread_number( fp ) );
      KEY( "MinLvl",		pCab->min_lvl,		fread_number( fp ) );
      KEY( "MinRank",		pCab->min_rank,		fread_number( fp ) );
      KEY( "MaxRoom",		pCab->max_room,		fread_number( fp ) );
      if (!str_cmp( word, "MRanks" )){
	int size = fread_number( fp );
	int i = 0;
	for(i = 0; i < size && i < RANK_MAX; i++){
	  if (!IS_NULLSTR(pCab->mranks[i])){
	    free_string( pCab->mranks[i] );
	  }
	  pCab->mranks[i] = fread_string( fp );
	}
	fMatch = TRUE;
	break;
      }
      KEYS("MsgGate",		pCab->gate_msg,		fread_string( fp ) );
      break;
    case 'N':
      KEYS( "Name",		pCab->name,		fread_string( fp ) );
      break;
    case 'O':
      KEYS( "OnGate",		pCab->gate_on,		fread_string( fp ) );
      KEYS( "OffGate",		pCab->gate_off,		fread_string( fp ) );
      break;
    case 'R':
      if (!str_cmp( word, "Race" )){
	int size = fread_number( fp );
	int i = 0;
	for(i = 0; i < size && i < MAX_PC_RACE; i++){
	  pCab->race[i] = fread_number( fp );
	}
	fMatch = TRUE;
	break;
      }
      break;
    case 'P':
      KEY( "Parent",		pCab->parent_vnum,		fread_number( fp ) );
      KEY( "Pit",		pCab->pit_vnum,			fread_number(fp)   );
      KEY( "Progress",		pCab->progress,			fread_flag(fp)	   );
      KEYS( "PrefixM",		pCab->mprefix,			fread_string( fp ) );
      KEYS( "PrefixF",		pCab->fprefix,			fread_string( fp ) );
      KEYS( "PrefixS",		pCab->sprefix,			fread_string( fp ) );
      break;
    case 'S':
      if (!str_cmp( word, "Skill" )){
	char *temp = fread_word( fp ) ;
	int sn = skill_lookup(temp);
	if (sn < 0)
	  bug("Fread_Cabal: unknown new skill.",0);
	else{
	  CSKILL_DATA* cSkill = malloc( sizeof( *cSkill ) );
	  cSkill->next = NULL;
	  cSkill->pSkill = new_skill();

	  cSkill->pSkill->sn		= UMAX(0, sn);
	  cSkill->pSkill->level		= fread_number( fp );
	  cSkill->pSkill->rating	= fread_number( fp );
	  cSkill->pSkill->learned	= fread_number( fp );
	  cSkill->pSkill->keep		= fread_number( fp );

	  cSkill->min_rank		= fread_number( fp );
	  cSkill->max_rank		= fread_number( fp );
	  cSkill->min_cost		= fread_number( fp );
	  cSkill->cost			= fread_number( fp );
	  cSkill->flag			= fread_flag( fp );

	  if (pCab->skills == NULL){
	    pCab->skills = cSkill;
	    pCab->skills->next = NULL;
	  }
	  else{
	    cSkill->next = pCab->skills;
	    pCab->skills = cSkill;
	  }
	}
	fMatch = TRUE;
	break;
      }
      break;
    case 'T':
      KEY( "Tower",		pCab->tower,		fread_number(fp)   );
      KEY( "Tower2",		pCab->tower_upgrade,	fread_number(fp)   );
      break;
    case 'V':
      KEY( "VERSION", pCab->version, fread_number( fp ) );
      if (!str_cmp( word, "Vote" )){
	char *temp = fread_string( fp ) ;
	int vote = vote_lookup(temp);
	if (vote < 1)
	  bug("Fread_Cabal: unknown vote.",0);
	else{
	  CVOTE_DATA* cVote = malloc( sizeof( *cVote ) );
	  cVote->next = NULL;

	  cVote->vote			= vote;
	  cVote->min			= fread_number( fp );
	  cVote->max			= fread_number( fp );
	  cVote->cost			= fread_number( fp );

	  if (pCab->votes == NULL){
	    pCab->votes = cVote;
	    pCab->votes->next = NULL;
	  }
	  else{
	    CVOTE_DATA* last = pCab->votes;
	    while (last->next)
	      last = last->next;
	    cVote->next = NULL;
	    last->next = cVote;
	  }
	}
	fMatch = TRUE;
	break;
      }
      break;
    case 'W':
      KEYS( "WhoName",		pCab->who_name,		fread_string( fp ) );
      break;
    }
  // cheack for key match
    if ( !fMatch ){
      bug("Fread_cabal: no match.",0);
      fread_to_eol(fp);
    }
  }
  return TRUE;
}
//Saves cabal index data in area file
void save_cabal_indexes( FILE *fp, AREA_DATA *pArea ){
  CABAL_INDEX_DATA *pci;

  fprintf( fp, "#CABALS\n" );

  for ( pci = cabal_index_list; pci; pci = pci->next ){
    if (pci->pArea == pArea)
      fwrite_cabal_index( fp, pci );
  }
  fprintf( fp, "#0\n\n\n\n" );
  return;
}

//reads the cabal index data from area file
void fread_cabal_indexes( FILE* fp, AREA_DATA* pArea ){
  CABAL_INDEX_DATA *pci;

  for ( ; ; ){
    int vnum;
    char letter;
    letter                          = fread_letter( fp );
    if ( letter != '#' ){
      bug( "cabal.c>fread_cabal_indexes: # not found.", 0 );
      exit( 1 );
    }
    vnum = fread_number( fp );

    /* terminated by #0 */
    if ( vnum < 1)
      break;
    pci = new_cabal_index();
    if (get_cabal_index( vnum ) != NULL){
      bug( "cabal.c>fread_cabal_indexes: duplicate vnum %d.", vnum );
      fread_cabal_index(fp, pci, pArea, vnum);
      free_cabal_index( pci );
      continue;
    }
    else if (!fread_cabal_index(fp, pci, pArea, vnum)){
      free_cabal_index( pci );
      continue;
    }
    top_cabal_index++;
    pci->next = cabal_index_list;
    cabal_index_list = pci;
  }
}

//adds given number of support and cps to justice's raid stats
void justice_raid_gain( long sup, long cps ){
  CABAL_DATA* pCab;

  for (pCab = cabal_list; pCab; pCab = pCab->next){
    if (pCab->parent == NULL
	&& IS_CABAL(pCab, CABAL_JUSTICE)
	&& pCab->present > 0){
      pCab->raid_sup += sup;
      pCab->raid_cps += cps;
    }
  }
}

//rewards a killer if they are in Stalker cabal and killed unnatural
void stalker_reward( CHAR_DATA* vch, CHAR_DATA* victim){
  CABAL_DATA* pCab;
  CHAR_DATA* ch = vch;
  char buf[MIL];

  if (ch == NULL || victim == NULL || IS_NPC(victim))
    return;
  if (IS_NPC(ch) && ch->master && !IS_NPC(ch->master))
    ch = ch->master;
  if ( (pCab = ch->pCabal) == NULL)
    return;
  else if (str_cmp(pCab->name, "stalker"))
    return;
  else if (!IS_UNDEAD(victim) && !IS_DEMON(victim) && !IS_AVATAR(victim))
    return;

  CP_GAIN(ch, number_range(CABAL_STALKER_REWARD / 2, CABAL_STALKER_REWARD), TRUE );
  sprintf( buf, "%s has put %s to rest.\n\r", PERS2(ch), PERS2(victim));
  cabal_echo( get_parent(pCab), buf );
}

//resets a cabal item into an altar
OBJ_DATA* reset_cabal_item( CHAR_DATA* altar, CABAL_DATA* pCab ){
  OBJ_DATA* citem;
  char buf[MIL];

  if (altar == NULL || pCab == NULL)
    return NULL;
  else if (pCab->citem != NULL){
    bug("cabal.c>reset_cabal_item: Cabal item already exists for cabal %d", pCab->pIndexData->vnum);
    return NULL;
  }
  else if ( (citem = create_object( get_obj_index( OBJ_VNUM_CABAL_ITEM), 25)) == NULL){
    bug("cabal.c>reset_cabal_item: Could not load a cabal item vnum %d", OBJ_VNUM_CABAL_ITEM);
    return NULL;
  }

  /* set misc data */
  pCab->citem = citem;
  citem->pCabal = pCab;
  citem->homevnum = altar->homevnum;

  /* set the strings */
  sprintf( buf, citem->name, pCab->name );
  free_string( citem->name );
  citem->name = str_dup( buf );

  sprintf( buf, citem->short_descr, pCab->name );
  free_string( citem->short_descr );
  citem->short_descr = str_dup( buf );

  sprintf( buf, citem->description, pCab->name );
  free_string( citem->description);
  citem->description = str_dup( buf );

  /* put the item in altar */
  obj_to_ch( citem, altar );
  citem->wear_loc = WEAR_HOLD;

  return citem;
}

//resets a cabal altar into the room
CHAR_DATA* reset_altar( ROOM_INDEX_DATA* room, CABAL_DATA* pCab ){
  CHAR_DATA* altar;
  char buf[MIL];

  if (pCab == NULL)
    return NULL;
  else if (room == NULL){
    if (pCab->anchor == NULL)
      return NULL;
    else
      room = pCab->anchor;
  }
  if (pCab->altar == NULL){
    if ( (altar = create_mobile( get_mob_index( MOB_VNUM_ALTAR))) == NULL){
      bug("cabal.c>reset_altar: Could not load an altar mob vnum %d", MOB_VNUM_ALTAR);
      return NULL;
    }
    /* set misc data */
    pCab->altar = altar;
    altar->pCabal = pCab;
    altar->homevnum = room->vnum;

    /* set the strings */
    sprintf( buf, altar->name, pCab->name );
    free_string( altar->name );
    altar->name = str_dup( buf );

    sprintf( buf, altar->short_descr, pCab->name );
    free_string( altar->short_descr );
    altar->short_descr = str_dup( buf );

    sprintf( buf, altar->long_descr, pCab->name );
    free_string( altar->long_descr);
    altar->long_descr = str_dup( buf );

    /* put the altar in room */
    altar->trust = 6969; //prevent firestorm kicking in
    char_to_room( altar, room );
    altar->trust = altar->level;
  }
  else
    altar = pCab->altar;
  /* if this cabal's cabal item doesn't exists, reset it */
  /* items are now reset on guards not altars
  if (pCab->citem == NULL)
    pCab->citem = reset_cabal_item( altar, pCab );
  */
  return altar;
}

//shows item that are in the altar when it is looked at
void show_altar( CHAR_DATA* ch, CHAR_DATA* altar ){
  OBJ_DATA* obj;

  act( "$n looks at you.", ch, NULL, altar, TO_VICT    );
  act( "$n looks at the $t altar.",  ch, altar->pCabal ? altar->pCabal->name : "Cabal", altar, TO_NOTVICT );

  if ( altar->description[0] != '\0' )
    send_to_char( altar->description, ch );
  else
    act( "You see nothing special about $M.", ch, NULL, altar, TO_CHAR );
  send_to_char("Placed on the altar are:\n\r\n\r", ch );

  for (obj = altar->carrying; obj; obj = obj->next_content ){
    sendf(ch, "  %s\n\r", obj->short_descr);
  }

  if ( altar->max_hit > 0 ){
    int percent = ( 100 * altar->hit ) / altar->max_hit;
    char buf[MIL];

    strcpy( buf, PERS(altar, ch) );
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
  }
}

void heal_altar( CHAR_DATA* altar ){
  AFFECT_DATA* paf, *paf_next, af;
  act("A new $n quickly takes a position before the altar.", altar, NULL, NULL, TO_ALL);
  for (paf = altar->affected; paf; paf = paf_next ){
    paf_next = paf->next;
    if (IS_GNBIT(paf->type, GN_BEN))
      continue;
    else
      affect_remove(altar, paf );
  }
  altar->hit = altar->max_hit;
  altar->hunting = NULL;
  update_pos( altar );

  //slap on 2 tick clam to prevent instant re-attack
  af.type	=	skill_lookup("calm");
  af.level	=	60;
  af.duration	=	1;
  af.where	=	TO_AFFECTS;
  af.bitvector	=	AFF_CALM;
  af.location	=	APPLY_NONE;
  af.modifier	=	0;
  affect_to_char( altar, &af );
}

//handles what happends when you destroy an altar
/* When killed the following happends:
   1) If altar holds allied objects, they are given to killer
   2) If killer has not taken any allied object he gets the altar cabal's object if it was in altar
   and the enemy cabal's members are present
   3) Altar is healed;
*/
void cabal_altar_death_ch( CHAR_DATA* ch, CHAR_DATA* altar){
  OBJ_DATA* obj, *obj_next, *citem = NULL;
  char buf[MIL];
  bool fAllied= FALSE;



  if (IS_NPC(ch) || ch->pCabal == NULL || altar->pCabal == NULL || is_same_cabal(ch->pCabal, altar->pCabal)){
    return;
  }

  /* You get any items that are friendly to you, and the altar's item if it exists and you are in war */
  for (obj = altar->carrying; obj; obj = obj_next){
    obj_next = obj->next_content;

    if (obj->item_type != ITEM_CABAL || obj->pCabal == NULL)
      continue;
    else if (is_same_cabal(altar->pCabal, obj->pCabal))
      citem = obj;

    if (is_friendly(obj->pCabal, ch->pCabal) == CABAL_FRIEND){
      fAllied = TRUE;
      obj_from_char( obj );
      obj_to_ch( obj, ch );

      act("You've retrieved $p!", ch, obj, NULL, TO_CHAR);
      act("$n retrieves $p!", ch, obj, NULL, TO_ROOM);

      //messages
      sprintf( buf, "%s has retrieved our item from %s.", PERS2(ch), altar->pCabal->name);
      cabal_echo( obj->pCabal, buf );
      sprintf( buf, "%s has retrieved %s from our altar!", PERS2(ch), obj->short_descr);
      cabal_echo( altar->pCabal, buf );

      //set timer on the item
      obj->timer = CABAL_STANDARD_TIMER;
    }
  }
  /* if we took allied items, then thats all */
  if (!fAllied){
    /* otherwise if altar holds its item, we giev it to attacker if they are at war */
    if (citem && is_friendly(citem->pCabal, ch->pCabal) == CABAL_ENEMY){

      if (citem->pCabal->present < 1 && mud_data.current_time - citem->pCabal->present_stamp > CABAL_PRESENT_CAPUTREWAIT){
	send_to_char("You cannot capture the Standard without defenders present.\n\r", ch);
      }
      else if (!is_in_altar(get_parent(ch->pCabal))){
	send_to_char("You cannot capture the Standard with your own missing from your altar.\n\r", ch);
      }
      else{
	act("You've captured $p!", ch, citem, NULL, TO_CHAR);
	act("$n captures $p!", ch, citem, NULL, TO_ROOM);

	obj_from_char( citem );
	obj_to_ch( citem, ch );

	//set timer on the item
	citem->timer = CABAL_STANDARD_TIMER;

	//force a repop to replace the guard
	ch->in_room->area->age = 31;

	//messages
	sprintf( buf, "%s has captured our item!", PERS2(ch));
	cabal_echo( citem->pCabal, buf );
	sprintf( buf, "%s has captured %s!", PERS2(ch), citem->short_descr);
	cabal_echo( get_parent(ch->pCabal), buf );

      }
    }
  }
}

/* called when an altar or cabal guard is killed */
void cabal_altar_death( CHAR_DATA* och, CHAR_DATA* altar){
  CHAR_DATA* ch, *vch;

  if (altar->pCabal == NULL)
    return;
  else if (IS_NPC(och) && IS_AFFECTED(och, AFF_CHARM) && och->master)
    ch = och->master;
  else
    ch = och;

  stop_fighting( altar, TRUE );
  check_dispel( -99, och, gsn_rage);

  for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
    if (IS_NPC(vch) || vch->pCabal == NULL || !is_same_group(vch, ch))
      continue;
    else if (IS_IMMORTAL(vch))
      continue;
    else
      cabal_altar_death_ch( vch, altar );
  }
  /* restore the altar */
  if (altar->pIndexData->vnum == MOB_VNUM_ALTAR){
    heal_altar( altar );
  }
  /* replace the guard with sub guard */
  else{
    CABAL_DATA* pCabal = altar->pCabal;
    extract_char( altar, TRUE );
    reset_standard_guard(pCabal->anchor, pCabal );
  }
}


//picks up items left on the ground in order for captured items to be saved across reboots
void handle_cabal_item_floor( CHAR_DATA* altar ){
  OBJ_DATA* obj, *obj_next;

  if (altar == NULL || altar->in_room == NULL || altar->pCabal == NULL)
    return;
  for (obj = altar->in_room->contents; obj; obj = obj_next){
    obj_next = obj->next_content;

    if (obj->item_type != ITEM_CABAL || obj->pCabal == NULL || !IS_SET(altar->off_flags, CABAL_GUARD))
      continue;
      /* enemy items go into the altar */
    else if (is_friendly(altar->pCabal, obj->pCabal) == CABAL_ENEMY){
      if (altar->pCabal->altar){
	act("$n puts $p on the altar.", altar, obj, NULL, TO_ROOM);
	obj_from_room( obj );
	obj_to_ch( obj, altar->pCabal->altar );
	obj->timer = 0;
      }
    }
    else if (is_same_cabal( altar->pCabal, obj->pCabal)){
      act("$n takes $p from the ground.", altar, obj, NULL, TO_ROOM);
      obj_from_room( obj );
      obj_to_ch( obj, altar );
      obj->wear_loc = WEAR_HOLD;
      obj->timer = 0;
    }
  }
}


//handles cabal items when person enters anchor room with altar.
/* Three possibilities
   1) Char has altar's item->take it and restore powers
   2) Char has allied items->take them and restore them to their altars
   3) Char has an enemy item->take it and drain powers, put in the guard
*/
void handle_cabal_item_entrance( CHAR_DATA* ch, ROOM_INDEX_DATA* anchor ){
  CHAR_DATA* altar, *guard;
  OBJ_DATA* obj, *obj_next;
  char buf[MIL];


  if (ch == NULL || anchor == NULL || anchor->pCabal == NULL || anchor->pCabal->anchor != anchor)
    return;
  else if (ch->fighting)
    return;

  if ( (altar = anchor->pCabal->altar) == NULL
       || altar->in_room != anchor
       || altar->pCabal == NULL
       || !can_see(ch, altar))
    altar = NULL;

  if ( (guard = anchor->pCabal->cguard) == NULL
       || guard->in_room != anchor
       || guard->pCabal == NULL
       || !can_see(ch, guard))
    guard = NULL;

  if (ch->pCabal
      && ((altar && is_friendly(ch->pCabal, altar->pCabal) == CABAL_ENEMY) || (guard && is_friendly(ch->pCabal, guard->pCabal) == CABAL_ENEMY))
      )
    return;
  //we are now in an anchor room with a cabal altar in it, check the items
  for (obj = ch->carrying; obj; obj = obj_next){
    obj_next = obj->next_content;
    if (obj->item_type != ITEM_CABAL || obj->pCabal == NULL)
      continue;
    /* OUR ITEM */
    else if (is_same_cabal(obj->pCabal, anchor->area->pCabal)){
      if (guard == NULL)
	continue;

      obj_from_char( obj );
      obj_to_ch( obj, guard );
      obj->wear_loc = WEAR_HOLD;

      //force a repop to replace the guard
      guard->in_room->area->age = 31;

      obj->timer = 0;

      act("You restore $p to its rightful place.", ch, obj, guard, TO_CHAR );
      act("$N hands $p to $n.", guard, obj, ch, TO_ROOM );

      sprintf( buf, "%s has restored our powers!", PERS2(ch));
      cabal_echo( guard->pCabal, buf );
      cp_event(ch, guard, obj, CP_EVENT_RETURN);
    }
    /* ENEMY ITEM */
    else if (altar && is_friendly(obj->pCabal, altar->pCabal) == CABAL_ENEMY ){
      obj_from_char( obj );
      obj_to_ch( obj, altar );

      obj->timer = 0;

      act("You place $p on the altar.", ch, obj, altar, TO_CHAR );
      act("$N places $p on the altar.", altar, obj, ch, TO_ROOM );

      sprintf( buf, "We have drained %s of its power.", obj->short_descr);
      cabal_echo( altar->pCabal , buf);
      sprintf( buf, "%s have drained %s of its power!", altar->pCabal->name, obj->short_descr);
      cabal_echo( obj->pCabal, buf );
      cp_event(ch, obj->pCabal->altar, obj, CP_EVENT_STANDARD);
    }
    /* ALLIED ITEM */
    else if (altar && is_friendly(obj->pCabal, altar->pCabal) == CABAL_FRIEND){
      /* try to reset the allied altar if necessary */
      if (obj->pCabal->cguard == NULL){
	if (obj->pCabal->anchor == NULL)
	  continue;
	else if (IS_CABAL(obj->pCabal, CABAL_GUARDMOB))
	  reset_cabal_guard( obj->pCabal->anchor, obj->pCabal);
	else
	  remove_cabalguard( obj->pCabal );
      }
      //check that we have guard now
      if (obj->pCabal->cguard){
	obj_from_char( obj );
	obj_to_ch( obj, obj->pCabal->cguard );

	//force a repop to replace the guard
	obj->pCabal->cguard->in_room->area->age = 31;

	obj->timer = 0;

	act("An Ambassador of $t carries their item off.", altar, obj->pCabal->who_name, NULL, TO_ALL );

	sprintf( buf, "%s has restored our powers!", PERS2(ch));
	cabal_echo( obj->pCabal, buf );
	cp_event(ch, NULL, obj, CP_EVENT_RETURN);
      }
    }
  }//END FOR
}



//checks if the cabal's item is captured
bool is_captured( CABAL_DATA* pCabal ){
  CABAL_DATA* pCab = get_parent(pCabal);
  if (pCab == NULL || pCab->citem == NULL)
    return FALSE;
  else if (pCab->citem->carried_by
	   && IS_NPC(pCab->citem->carried_by )
	   && pCab->citem->carried_by->pCabal
	   && pCab->citem->carried_by->pCabal->altar
	   && pCab->citem->carried_by->pCabal->altar == pCab->citem->carried_by
	   && !is_same_cabal(pCab, pCab->citem->carried_by->pCabal))
    return TRUE;
  else
    return FALSE;
}

//checks if the cabal's item is captured
bool is_captured_by( CABAL_DATA* pCabal, CABAL_DATA* pCapturee ){
  CABAL_DATA* pCab = get_parent(pCabal);
  CABAL_DATA* pCapt = get_parent(pCapturee);

  if (pCab == NULL || pCab->citem == NULL || pCapt == NULL)
    return FALSE;
  else if (pCab->citem->carried_by
	   && IS_NPC(pCab->citem->carried_by )
	   && pCab->citem->carried_by->pCabal
	   && pCab->citem->carried_by->pCabal->altar
	   && pCab->citem->carried_by->pCabal->altar == pCab->citem->carried_by
	   && is_same_cabal(pCapt, pCab->citem->carried_by->pCabal)
	   && !is_same_cabal(pCab, pCab->citem->carried_by->pCabal))
    return TRUE;
  else
    return FALSE;
}

//checks if the cabal's item is in its altar
bool is_in_altar( CABAL_DATA* pCab ){
  if (pCab == NULL || pCab->citem == NULL)
    return FALSE;
  else if (pCab->citem->carried_by
	   && IS_NPC(pCab->citem->carried_by )
	   && pCab->citem->carried_by->pCabal
	   && pCab->citem->carried_by->pCabal->cguard
	   && pCab->citem->carried_by->pCabal->cguard == pCab->citem->carried_by
	   && is_same_cabal(pCab, pCab->citem->carried_by->pCabal))
    return TRUE;
  else
    return FALSE;
}



//handles transfer of cabal item on a kill of another player
void handle_ctf_kill( CHAR_DATA* tch, CHAR_DATA* victim ){
  CHAR_DATA* ch;
  OBJ_DATA* obj, *obj_next;

  if (tch == NULL || victim == NULL)
    return;
  else if (IS_NPC(tch) && IS_AFFECTED(tch, AFF_CHARM) && tch->master)
    ch = tch->master;
  else
    ch = tch;

  if (IS_NPC(ch) || ch->pCabal == NULL || ch == victim || ch->in_room != victim->in_room)
    return;

  //search victim for allied or enemy cabal items
  for (obj = victim->carrying; obj; obj = obj_next){
    obj_next = obj->next_content;

    if (obj->item_type == ITEM_CABAL && obj->pCabal && is_friendly(ch->pCabal, obj->pCabal) != CABAL_NEUTRAL){
      act("You have captured $p from $N!", ch, obj, victim, TO_CHAR );
      act("$n has captured $p from $N!", ch, obj, victim, TO_ROOM );
      obj_from_char( obj );
      obj_to_ch( obj, ch );
      obj->timer = CABAL_STANDARD_TIMER;
    }
  }
}

//checks if the cabal's capture item counter should be set.
void set_capture_item_timer( CHAR_DATA* victim ){
  CABAL_DATA* pCab = get_parent( victim->pCabal );

  if (pCab == NULL)
    return;
  else if (pCab->cguard == NULL || pCab->cguard != victim)
    return;
  else if (pCab->present > 0)
    pCab->present_stamp = mud_data.current_time;

  return;
}
