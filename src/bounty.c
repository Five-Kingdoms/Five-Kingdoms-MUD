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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <malloc.h>
#include "merc.h"
#include "recycle.h"
#include "bounty.h"
#include "cabal.h"
#include "interp.h"
#include "jail.h"
#include "save_mud.h"

/***************************************************************/
/*Following are bounty orientanted routines for Forsaken Lands  */
/*mud created by Virigoth circa 2002.  Copyrighted for Forsaken*/
/*Lands mud June 1, 2002.  Do not use or copy without explicit */
/*permission of author. (Voytek Plawny aka Virigoth)	       */
/***************************************************************/


extern PACT_DATA* pact_list;
/* global bounty list */
BOUNTY_DATA* bounty_list = NULL;


/* attatches an objec to a bounty bid and puts it in a bounty room */
void obj_to_bbid( OBJ_DATA* obj, BBID_DATA* bbid ){
  
  if (obj == NULL || bbid == NULL)
    return;
  else if (obj->in_room )
    obj_from_room( obj );
  else if (obj->carried_by)
    obj_from_char( obj );
  else if (obj->in_obj )
    obj_from_obj ( obj );
  else if (obj->bbid )
    obj_from_bbid( obj );

  if (bbid->obj == NULL){
    bbid->obj = obj;
    bbid->obj->next_bidobj = NULL;
  }
  else{
    obj->next_bidobj = bbid->obj;
    bbid->obj = obj;
  }

  obj_to_room( obj, get_room_index( ROOM_VNUM_BOUNTY ));
  obj->bbid = bbid;
}

/* removes an object from bounty room and the bid list */
void obj_from_bbid( OBJ_DATA* obj ){
  BBID_DATA* bbid = obj->bbid;

  if (bbid == NULL){
    bug("obj_from_bbid: obj has no bounty bid.", 0);
    return;
  }
  if (obj->in_room && obj->in_room->vnum == ROOM_VNUM_BOUNTY )
    obj_from_room( obj );
  
  if ( obj == bbid->obj )
    bbid->obj = obj->next_bidobj;
  else{
    OBJ_DATA *prev;
    for ( prev = bbid->obj; prev; prev = prev->next_bidobj ){
      if ( prev->next_bidobj == obj ){
	prev->next_bidobj = obj->next_bidobj;
	break;
      }
    }
    if ( prev == NULL ){
      bug( "Obj_from_bbid: obj not found.", 0 );
      return;
    }
  }
  obj->next_bidobj = NULL;
  obj->bbid = NULL;
}

  
/* creates/destroys bounty bids, we do not use lists for this */
BBID_DATA* new_bbid(void){
  BBID_DATA* pBb = (BBID_DATA*) malloc( sizeof( *pBb ));

/* reset initial things */
  pBb->next		= NULL;
  pBb->bounty		= NULL;

  pBb->name		= str_dup("");
  pBb->level		= 0;
  pBb->pCabal		= NULL;

  pBb->cp		= 0;
  pBb->gp		= 0;

  pBb->obj		= NULL;

  return pBb;
};

void free_bbid( BBID_DATA* pBb ){
  if (pBb == NULL)
    return;

  free_string( pBb->name );
  
  pBb->next = NULL;

  free( pBb );
}

/* creates destroys bounties */
BOUNTY_DATA *bounty_free;
BOUNTY_DATA *new_bounty(){

    BOUNTY_DATA *pB;
    if (bounty_free == NULL)
	pB = alloc_perm(sizeof(*pB));
    else
    { 
      pB = bounty_free;
      bounty_free = bounty_free->next;
    }

    pB->next		= NULL;
    pB->bids		= NULL;

    pB->time_stamp	= 0;
    pB->name		= str_empty;
    
    VALIDATE(pB);
    return pB;
}

void free_bounty(BOUNTY_DATA *bounty)
{
  if (!IS_VALID(bounty))
    return;

  if (!IS_NULLSTR(bounty->name))
    free_string( bounty->name);

  /* free bids */
  while (bounty->bids){
    BBID_DATA* pBb = bounty->bids;
    bounty->bids = pBb->next;
    free_bbid( pBb );
  }

  INVALIDATE( bounty );

  bounty->next = bounty_free;
  bounty_free   = bounty;
}


/* adds a bounty onto the bounty list */
void add_bounty( BOUNTY_DATA* pb ){
  CHAR_DATA* victim;

  if (bounty_list == NULL){
    bounty_list = pb;
    pb->next = NULL;
  }
  else{
/* we always insert the bountys from oldest to newest */
    BOUNTY_DATA* last = bounty_list;
    for (last = bounty_list; last->next != NULL; last = last->next);
    pb->time_stamp = UMAX(last->time_stamp + 1, pb->time_stamp );
    last->next = pb;
    pb->next = NULL;
  }

  /* check if the target of this bounty is about */
  if ( (victim = char_file_active( pb->name)) != NULL){
    victim->pcdata->pbounty = pb;
  }
}
	

/* removes a bounty from the bounty list */
bool rem_bounty( BOUNTY_DATA* pb ){
  BOUNTY_DATA* prev = bounty_list;
  if (bounty_list == NULL){
    bug("rem_bounty: bounty_list null.", 0);
    return FALSE;
  }
  if (prev == pb )
    bounty_list = pb->next;
  else{
    while (prev->next && prev->next != pb){
      prev = prev->next;
    }
  }
  if (prev == NULL){
    bug("rem_bounty: bounty note found.", 0);
    return FALSE;
  }
  prev->next = pb->next;
  pb->next = NULL;
  return TRUE;
}

/* removes a single bid from a bounty */
void bid_from_bounty( BBID_DATA* bbid ){
  BOUNTY_DATA* bounty = bbid->bounty;

  if (bounty == NULL){
    bug("bid_from_bounty: bid not on bounty.", 0);
    return;
  }

  if (bounty->bids == bbid ){
    bounty->bids = bbid->next;
    bbid->next = NULL;
  }
  else{
    BBID_DATA* prev = bounty->bids;
    for (; prev; prev = prev->next){
      if (prev->next == bbid){
	prev->next = bbid->next;
	break;
      }
    }
    if ( prev == NULL ){
      bug( "bid_from_bounty: bid not found.", 0 );
	return;
    }
  }
  bbid->bounty = NULL;
  bbid->next = NULL;
}
  
/* adds a single bounty bid onto a bounty */
BBID_DATA* bid_to_bounty( BBID_DATA* bbid, BOUNTY_DATA* bounty ){
  if (bbid == NULL || bounty == NULL)
    return NULL;
  else if (bbid->bounty)
    bid_from_bounty( bbid );

  if (bounty->bids == NULL){
    bounty->bids = bbid;
    bbid->next = NULL;
  }
  else{
    BBID_DATA* last = bounty->bids;
    while (last->next)
      last = last->next;
    last->next = bbid;
    bbid->next = NULL;
  }
  bbid->bounty = bounty;
  return bbid;
}

/* returns bid from given bounty with given name */
BBID_DATA* get_bbid( BOUNTY_DATA* bounty, char* name ){
  BBID_DATA* bbid;
  int num = 0;
  int count = 0;

  if (bounty == NULL || IS_NULLSTR( name ))
    return NULL;
  else{
    bbid  = bounty->bids;
    num = atoi( name );
  }
  for (; bbid; bbid = bbid->next){
    if (num && ++count == num)
      return bbid;
    else if (LOWER(name[0]) == LOWER(bbid->name[0]) && !str_cmp(name, bbid->name))
      return bbid;
  }
  return NULL;
}

/* returns a bounty if found based on victim name */
BOUNTY_DATA* get_bounty( char* name ){
  BOUNTY_DATA* bounty = bounty_list;

  for (; bounty; bounty = bounty->next){
    if (LOWER(name[0]) == LOWER(bounty->name[0]) && !str_cmp(name, bounty->name))
      return bounty;
  }
  return NULL;
}

/* adds/compounds a single bid based on victim's name */
BBID_DATA* add_bounty_bid( BBID_DATA* new_bbid, char* name){
  BOUNTY_DATA* bounty;
  BBID_DATA* bbid;

  if (new_bbid == NULL || IS_NULLSTR( name ))
    return NULL;

  /* we check if there is en existing bounty */
  if ( (bounty = get_bounty( name )) == NULL){
    bounty = new_bounty();

    bounty->next	= NULL;
    
    bounty->name	= str_dup( name );
    bounty->time_stamp	= mud_data.current_time;
    bounty->bids	= new_bbid;
    add_bounty( bounty );
    return bounty->bids;
  }
  else if ( (bbid = get_bbid( bounty, new_bbid->name )) != NULL){
    OBJ_DATA* obj, *obj_next;
    /* we add the info from new bid onto existing one */
    bounty->time_stamp	= mud_data.current_time;
    bbid->level		= new_bbid->level;
    bbid->pCabal	= new_bbid->pCabal;
    bbid->cp	       += new_bbid->cp;
    bbid->gp	       += new_bbid->gp;

/* add objects */
    for (obj = new_bbid->obj; obj; obj = obj_next){
      obj_next = obj->next_bidobj;
      obj_from_bbid( obj );
      obj_to_bbid( obj, bbid );
    }
    return bbid;
  }
  else
    return bid_to_bounty( new_bbid, bounty );
  
  return NULL;
}

/* creates a linked list of ordered bounties by gold/cp/obj */
BOUNTY_DATA* make_bounty_list( BOUNTY_DATA* bounties, bool fGold, bool fObj){
  BBID_DATA* bbid;
  BOUNTY_DATA* bounty;
  BOUNTY_DATA* new_bount;

  /* run through all bounties */
  for (bounty = bounty_list; bounty; bounty = bounty->next){
    int val = 0;
    new_bount = new_bounty();
    /* get the total bid value for this bounty */
    new_bount->name = str_dup(bounty->name);
    new_bount->next = NULL;
    for (bbid = bounty->bids; bbid; bbid = bbid->next){
      if (fGold)
	val += bbid->gp;
      else if (fObj){
	OBJ_DATA* obj;
	int max_obj = 1;
	int tot_level = 0;
	for (obj = bbid->obj; obj; obj = obj->next_bidobj ){
	  max_obj++;
	  tot_level += obj->level;
	}
	val = val + (tot_level / max_obj);
      }
      else
	val += bbid->cp;
    }
    /* at this point val has a total comparison for this bounty */
    /* we cheat and put it in new bounties time stamp */
    new_bount->time_stamp = val;
    
    /* ordered insert */
    if (bounties == NULL){
      bounties = new_bount;
      bounties->next = NULL;
    }
    else{
      BOUNTY_DATA* prev = bounties;
      BOUNTY_DATA* cur = bounties;
      while( cur ){
	if (val > cur->time_stamp)
	  break;
	else{
	  prev = cur;
	  cur = cur->next;
	}
      }
      if (prev == cur){
	new_bount->next = bounties;
	bounties = new_bount;
      }
      else{
	new_bount->next = prev->next;
	prev->next = new_bount;
      }
    }
  }
  return bounties;
}

/* show top 10 bounties */
void show_top_10( CHAR_DATA* ch ){
  const int max_10 = 10;
  char gold[max_10] [MIL];
  char cp[max_10] [MIL];
  char obj[max_10] [MIL];

  BOUNTY_DATA* bounty;
  BOUNTY_DATA* bounties = NULL;
  int count = 0;

  for (count = 0; count < max_10; count++){
    gold[count][0] = '\0';
    obj[count][0] = '\0';
    cp[count][0] = '\0';
  }
  /* we perform three similiar operations to build a linked list 
     with sorted entries according to: gold, cp, and objects
     After each build we enter the top 10 entries into the array
     destroy the list and do the next top 10
  */

  /* GOLD */
  bounties = make_bounty_list( bounties, TRUE, FALSE );
  count = 0;
  for (bounty = bounties; bounty; bounty = bounty->next){
    if (count >= max_10)
      break;
    else
      strcpy(gold[count++], bounty->name );
  }
  while (bounties){
    BOUNTY_DATA* b = bounties;
    bounties = b->next;
    free_bounty( b );
  }  
  /* CP */
  bounties = make_bounty_list( bounties, FALSE, FALSE );
  count = 0;
  for (bounty = bounties; bounty; bounty = bounty->next){
    if (count >= max_10)
      break;
    else
      strcpy(cp[count++], bounty->name );
  }
  while (bounties){
    BOUNTY_DATA* b = bounties;
    bounties = b->next;
    free_bounty( b );
  }  
  /*OBJ */
  bounties = make_bounty_list( bounties, FALSE, TRUE );
  count = 0;
  for (bounty = bounties; bounty; bounty = bounty->next){
    if (count >= max_10)
      break;
    else
      strcpy(obj[count++], bounty->name );
  }
  /* free the linked list */
  while (bounties){
    BOUNTY_DATA* b = bounties;
    bounties = b->next;
    free_bounty( b );
  }  
/* print the top out */
  send_to_char("TOP 10 BOUNTIES:\n\r", ch);
  send_to_char("Place  Gold      Cabal Pts.  Object Lvl.\n\r", ch);
  send_to_char("----------------------------------------\n\r", ch);
  for (count = 0; count < max_10; count++){
    sendf( ch, "%2d.    %-10s %-10s  %-10s\n\r",
	   count + 1,
	   gold[count] [0] ? gold[count] : "", 
	   cp[count] [0] ? cp[count] : "", 
	   obj[count] [0] ? obj[count] : "");
  }
}  
void do_top10( CHAR_DATA* ch, char* argument ){
  show_top_10( ch );
}


/* get a cost of buyout on a bounty		*
 * FORMULA for PLACE:
 140 + 60 * (10 - position)
 *
 * position = place in top10, hence:
 * - Person not in top10	: 140
 * - Person in 9th place	: 200
 * - Person in 6th place	: 380
 * - Person in 4th place	: 500
 *
 * 3rd-1st cannot be bought out.
 * position is determined as best of three categories.
 *
 * FORMULA for GOLD:
 * highest_gold_bid / 10000
 *
 * FORMULA for OBJECTS:
 * total object level * 2
 *
 * The highest worth out of Obj/Gold/Cp/Place is returned
 */

int get_buyout_cost( BOUNTY_DATA* pBounty ){
  const int max_10 = 10;
  BOUNTY_DATA* bounty;
  BOUNTY_DATA* bounties = NULL;
  BBID_DATA* bbid;
  OBJ_DATA* obj;

  int count = 0;
  int place = 99;

  int gold_cost = 0;
  int obj_cost = 0;
  int place_cost = 0;
  int cp_cost = 0;
  int cost = 0;
  


  /* get the highest place first */
  /* GOLD */
  bounties = make_bounty_list( bounties, TRUE, FALSE );
  for (bounty = bounties; bounty; bounty = bounty->next){
    if (count >= max_10)
      break;
    else if (!str_cmp(bounty->name, pBounty->name) && count < place){
      place = count;
      break;
    }
    count++;
  }
  while (bounties){
    BOUNTY_DATA* b = bounties;
    bounties = b->next;
    free_bounty( b );
  }  
  /* CP */
  bounties = make_bounty_list( bounties, FALSE, FALSE );
  count = 0;
  for (bounty = bounties; bounty; bounty = bounty->next){
    if (count >= max_10)
      break;
    else if (!str_cmp(bounty->name, pBounty->name) && count < place){
      place = count;
      break;
    }
    count++;
  }
  while (bounties){
    BOUNTY_DATA* b = bounties;
    bounties = b->next;
    free_bounty( b );
  }  
  /*OBJ */
  bounties = make_bounty_list( bounties, FALSE, TRUE );
  count = 0;
  for (bounty = bounties; bounty; bounty = bounty->next){
    if (count >= max_10)
      break;
    else if (!str_cmp(bounty->name, pBounty->name) && count < place){
      place = count;
      break;
    }
    count++;
  }
  /* free the linked list */
  while (bounties){
    BOUNTY_DATA* b = bounties;
    bounties = b->next;
    free_bounty( b );
  }  

  /* we have the lowest spot on top 10, if its 0, 1, or 2 we return negative */
  if (place < 3)
    return -1;
  else if (place == 99)
    place_cost = 140;
  else
    place_cost = 140 + 60 * (10 - place);

  /* get the other costs */
  for (bbid = pBounty->bids; bbid; bbid = bbid->next){
    int tot_level = 0;

    if (bbid->gp / 10000 > gold_cost)
      gold_cost = bbid->gp / 10000;
    if (bbid->cp > cp_cost)
      cp_cost = bbid->cp;

    for (obj = bbid->obj; obj; obj = obj->next_bidobj ){
      tot_level += obj->level;
    }
    if (tot_level * 2 > obj_cost)
      obj_cost = tot_level * 2;
  }


  /* return highest */
  cost = place_cost;
  if (gold_cost > cost)
    cost = gold_cost;
  if (obj_cost > cost)
    cost = obj_cost;
  if (cp_cost > cost)
    cost = cp_cost;

  return cost;
}  

/* shows a bounty to a character with all its bids */
void show_bounty( CHAR_DATA* ch, BOUNTY_DATA* bounty, bool fGold, bool fObj ){
  BUFFER* buffer;
  BBID_DATA* bbid;
  BBID_DATA* bbids = NULL;

  char buf[MSL];

  long tot_gp = 0;
  long tot_cp = 0;
  int count = 0;

/* we create a temorary linked list of bounty data's by ordered inserts */
  for (bbid = bounty->bids; bbid; bbid = bbid->next){
    BBID_DATA* new_bid = new_bbid();
    int val = 0;

    new_bid->name	= str_dup( bbid->name );
    new_bid->level	= ++count;
    new_bid->pCabal	= bbid->pCabal;
    new_bid->cp		= bbid->cp;
    new_bid->gp		= bbid->gp;
    new_bid->obj	= bbid->obj; //we do not copy objects since we only need info to print later

    if (fGold)
      val = new_bid->gp;
    else if (fObj){
      OBJ_DATA* obj;
      int max_obj = 1;
      int tot_level = 0;

      for (obj = new_bid->obj; obj; obj = obj->next_bidobj ){
	max_obj++;
	tot_level += obj->level;
      }
      val = tot_level / max_obj;
    }
    else
      val = new_bid->cp;

    /* we have a value for comparison and new bbid, find insertion point */
    if (bbids == NULL){
      bbids = new_bid;
      bbids->next = NULL;
    }
    else{
      BBID_DATA* prev = bbids;
      BBID_DATA* cur = bbids;
      while( cur ){
	int key = 0;

	if (fGold)
	  key = cur->gp;
	else if (fObj){
	  OBJ_DATA* obj;
	  int max_obj = 1;
	  int tot_level = 0;
	  
	  for (obj = cur->obj; obj; obj = obj->next_bidobj ){
	    max_obj++;
	    tot_level += obj->level;
	  }
	  key = tot_level / max_obj;
	}
	else
	  key = cur->cp;
	
	/* we now have a key and value to compare */
	if (val > key)
	  break;
	else{
	  prev = cur;
	  cur = cur->next;
	}
      }
      if (prev == cur){
	new_bid->next = bbids;
	bbids = new_bid;
      }
      else{
	new_bid->next = prev->next;
	prev->next = new_bid;
      }
    }
  }

  /* at this point we have a linked list bbids full of sorted bids, we show them. */
  buffer = new_buf();
  sprintf( buf, "Bids set for %s's head listed in order of %s.\n\r",
	   bounty->name,
	   fObj ? "AVERAGE OBJECT RANK" : fGold ? "GOLD AMOUNT" : "CABAL POINTS");
  add_buf( buffer, buf );
  for (bbid = bbids; bbid; bbid = bbid->next){
    OBJ_DATA* obj;
    tot_cp += bbid->cp;
    tot_gp += bbid->gp;
    sprintf( buf, "%2d: Gp: %-10ld Cp: %-10d\n\r", bbid->level, bbid->gp, bbid->cp);
    add_buf( buffer, buf );
    if (bbid->obj){
      add_buf(buffer, "    ");
      for (obj = bbid->obj; obj; obj = obj->next_bidobj){
	sprintf(buf, "%-12.12s, ", obj->short_descr);
	add_buf( buffer, buf );
      }
      add_buf( buffer, "\n\r");
    }
  }
  sprintf(buf, "This bounty will expire in %ld days.\n\r",
	  (BOUNTY_LIFETIME - mud_data.current_time + bounty->time_stamp) / 86400);
  add_buf( buffer, buf );
  page_to_char(buf_string(buffer),ch);
  free_buf(buffer);

  /* free the linked list */
  while (bbids){
    bbid = bbids;
    bbids = bbid->next;
    bbid->obj = NULL; //we do not free objets since we only linked to them
    free_bbid( bbid );
  }
}
      
    
	

      

  
void show_bbid( CHAR_DATA* ch, BBID_DATA* bbid ){
  OBJ_DATA* obj;
  char buf[MSL];

  if (ch == NULL || bbid == NULL)
    return;

  sendf(ch, 
	 "Gold  : %-10ld %s%s: %-10d\n\r",
	bbid->gp,
	ch->pCabal ? capitalize(ch->pCabal->currency) : "CP",
	ch->pCabal ? bbid->cp == 1 ? "" : "s" : "",
	bbid->cp);
  if (bbid->obj == NULL){
    return;
  }
  else{
    int count = 0;
    sprintf(buf, "Objects:\n\r");
    for (obj = bbid->obj; obj; obj = obj->next_bidobj ){
      sprintf( buf + strlen( buf ), "%d. %s\n\r",
	       ++count,
	       obj->short_descr);
    }
    send_to_char( buf, ch );
    send_to_char("\n\r", ch );
  }
}
  
    
  
/* MAIN BOUNTY FUNCTION TO CREATE BOUNTY BIDS */
/* Syntax:

   bounty cp <amount>	: set amount of cabal points on your bid
   bounty gold <amount>	: set amount of gold on your bid
   bounty add <object>	: add object to the bid
   bounty rem <object>	: remove object from the bid
   bounty place <name>	: place current bounty bid onto character <name>

   bounty <name>	: show your bids on character <name>
   bounty		: show current bid
*/
void do_bounty( CHAR_DATA* ch, char* argument ){
  char arg[MIL];
  int val;

  if (IS_NPC(ch))
    return;
  else
    argument = one_argument( argument, arg );
  
  if (IS_NULLSTR( arg )){
    if (ch->pcdata->pbbid){
      send_to_char("Your current bounty bid is composed of:\n\r", ch );
      show_bbid(ch, ch->pcdata->pbbid);
    }
    send_to_char("Bounty <cp/gold/add/rem/place/buyout> (\"help bounty\" for details).\n\r", ch );
    return;
  }
  else if (!str_prefix( arg, "cp")){
    if (ch->pCabal == NULL){
      send_to_char("You are not in a cabal!\n\r", ch );
      return;
    }
    if (ch->pcdata->pbbid == NULL)
      ch->pcdata->pbbid = new_bbid();

    if ( (val = atoi( argument )) < 30){
      sendf(ch, "You must place at last 30 %ss.\n\r", ch->pCabal->currency );
      return;
    }
    else if (val > GET_CP(ch)){
      int dif = val - GET_CP( ch );
      sendf( ch, "You are short %d %s%s.\n\r", dif, ch->pCabal->currency, dif == 1 ? "" : "s" );
      return;
    }
    else{
      ch->pcdata->pbbid->cp = val;
      sendf( ch, "Your bounty now includes %d %s%s.\n\r", ch->pcdata->pbbid->cp, 
	     ch->pCabal->currency, 
	     ch->pcdata->pbbid->cp == 1 ? "" : "s" );
    }
    return;
  }
  else if (!str_prefix( arg, "gold")){
    if (ch->pcdata->pbbid == NULL)
      ch->pcdata->pbbid = new_bbid();

    /* get minimum bid for this character */
    if ( (val = atoi( argument )) < 10000){
      send_to_char("You must place at least 10k gold.\n\r", ch );
      return;
    }
    else if (val > ch->in_bank){
      int dif = val - ch->in_bank;
      sendf( ch, "You are short %d gold in your bank account.", dif);
      return;
    }
    else{
      ch->pcdata->pbbid->gp = val;
      sendf( ch, "Your bounty now includes %ld gold.\n\r", ch->pcdata->pbbid->gp );
    }
    return;
  }
  else if (!str_prefix( arg, "add")){
    OBJ_DATA* obj, *pObj;
    int count = 0;


    if (IS_NULLSTR( argument )){
      send_to_char("Add what object to the bounty?\n\r", ch );
      return;
    }
    else if ( (obj = get_obj_carry( ch, argument, ch )) == NULL){
      send_to_char("You do not seem to be carrying that object.\n\r", ch );
      return;
    }
    else if (!IS_LIMITED( obj )){
      send_to_char("That object is too common to be placed as a bounty.\n\r", ch );
      return;
    }
    else if (obj->bbid != NULL){
      send_to_char("That object already seems to be part of your bounty.\n\r", ch );
      return;
    }
    else if (!can_drop_obj(ch, obj )){
      send_to_char("You must be able to part with the item first.\n\r", ch );
      return;
    }
    else if (obj->pIndexData->vnum == OBJ_VNUM_CLONE){
      send_to_char("Placing a counterfit item as a bounty is a sure way to die.\n\r", ch);
      return;
    }
    else if (obj->contains && !IS_OBJ_STAT(obj, ITEM_SOCKETABLE)) {
      send_to_char("You may not place objects that are containers, or loaded.\n\r", ch );
      return;
    }

    if (ch->pcdata->pbbid == NULL)
      ch->pcdata->pbbid = new_bbid();

    /* count how many objects on this bid */
    for (pObj = ch->pcdata->pbbid->obj; pObj; pObj = pObj->next_bidobj )
      count++;
    if (count + 1 > 5){
      send_to_char("You may place a maximum of five objects as a bounty.\n\r", ch );
      return;
    }

    /* we attach the object into this bid data BUT DO NOT remove it from character, "place" command will do all that */
    if (ch->pcdata->pbbid->obj == NULL){
      obj->next_bidobj = NULL;
      ch->pcdata->pbbid->obj = obj;
    }
    else{
      obj->next_bidobj = ch->pcdata->pbbid->obj;
      ch->pcdata->pbbid->obj = obj;
    }
    obj->bbid = ch->pcdata->pbbid;
    sendf( ch, "You have added %s to the bounty.\n\r", obj->short_descr);

    /* we store the count in bbid->level to be used in "place" command */
    ch->pcdata->pbbid->level = count + 1;
    return;
  }
  else if (!str_prefix( arg, "rem")){
    OBJ_DATA* obj;

    if (ch->pcdata->pbbid == NULL || ch->pcdata->pbbid->obj == NULL){
      send_to_char("You have not placed any objects as a bounty.\n\r", ch);
      return;
    }
    else if (IS_NULLSTR( argument )){
      send_to_char("Remove what object from the bounty?\n\r", ch );
      return;
    }
    else if ( (obj = get_obj_carry( ch, argument, ch )) == NULL){
      send_to_char("You do not seem to be carrying that object.\n\r", ch );
      return;
    }
    else if (obj->bbid == NULL){
      send_to_char("That object is has not been placed as a bounty.\n\r", ch );
      return;
    }
    else if (obj->bbid != ch->pcdata->pbbid){
      send_to_char("That object has not been placed into the bounty you are working on.\n\r", ch );
      return;
    }
    
    obj_from_bbid( obj );
    ch->pcdata->pbbid->level --;

    sendf( ch, "You have removed %s from the bounty.\n\r", obj->short_descr);
    return;
  }
  /* none of the commands matched, try to place the bounty */
  else if (!str_prefix( arg, "place")){
    CHAR_DATA* victim;
    CHAR_DATA* mob;
    DESCRIPTOR_DATA* d;
    BBID_DATA* bbid, *oldBbid;
    BOUNTY_DATA* oldBounty;

    int coffers = 0, cost = 0;
    bool fPurge = FALSE;

    long min_gp = 0, min_cp = 0;

    for (mob = ch->in_room->people; mob; mob = mob->next_in_room ){
      if (IS_NPC(mob) && IS_SET(mob->act, ACT_CONTRACTER))
	break;
    }
    if (mob == NULL){
      send_to_char("There is no one here to record your bounty.\n\r", ch);
      return;
    }
    else if (ch->pcdata->pbbid == NULL){
      do_bounty( ch, "" );
      return;
    }
    else if (IS_NULLSTR( argument)){
      send_to_char("Place this bounty on who?\n\r", ch );
      return;
    }
    else
      bbid = ch->pcdata->pbbid;

    /* try to get the victim of this bounty */
    if ( (victim = char_file_active (argument)) == NULL){
      if ( (d = bring( ch->in_room, argument )) != NULL){
	victim = d->character;
	fPurge = TRUE;
	victim->pcdata->roomnum *= -1;
      }
      else{
	send_to_char ("That character does not exist.\n\r", ch);
	return;
      }
    }

    /* we now have the victm of this bounty, do some checks */
    if (IS_NPC(victim)){
      send_to_char("Why would you want to place a bounty on a mobile?\n\r", ch );
      return;
    }
    else if (mud_data.current_time < victim->pcdata->last_bounty + BOUNTY_WAITTIME){
      send_to_char("Their bounty has been recently claimed. Try again later.\n\r", ch);
      if (fPurge)
	purge( victim );
      return;
    }

    /* calculate minimums */
    min_gp = 100 * victim->level + (victim->pcdata->kpc / UMAX(1, victim->pcdata->dpc)) * 10000;
    min_cp = UMAX(30, min_gp / 1000);

    /* find old bid if any */
    if ( (oldBounty = get_bounty( victim->name )) != NULL
       && (oldBbid = get_bbid( oldBounty, ch->name )) != NULL){
      min_gp -= oldBbid->gp;
      min_cp -= oldBbid->cp;
    }
    min_gp = UMAX(5000, min_gp );
    min_cp = UMAX(30, min_cp );

    if (victim->level < 15)
      send_to_char("You may not place bounties on victims of rank lower then 15th.\n\r", ch );
    else if (ch == victim)
      send_to_char("You cannot place bounties on yourself.\n\r", ch );
    else if (mud_data.mudport != TEST_PORT && ch->pCabal && IS_CABAL(ch->pCabal, CABAL_ROUGE))
      send_to_char("Your job is to collect the bounties, not place them.\n\r", ch );
    else if (victim->pCabal && IS_CABAL(victim->pCabal, CABAL_ROUGE))
      act("$N seems to ignore your request.", ch, NULL, mob, TO_CHAR);
    else if (mud_data.mudport != TEST_PORT && (IS_IMMORTAL( ch ) || IS_IMMORTAL(victim)) )
      send_to_char("Gods are above such petty behavior.  Event logged.\n\r", ch );
    else if (bbid->obj && victim->level < 40 && victim->pCabal == NULL)
      send_to_char("In order to place equipment as a bounty your victim must be of rank 40+ or cabaled.\n\r", ch );
    /* minimum gold */
    else if (bbid->gp && bbid->gp < min_gp){
      sendf(ch, "The minimum gold bid on %s is %ld gold.\n\r", victim->name, min_gp);
    }
    else if (ch->pCabal && bbid->cp && bbid->cp < min_cp){
      sendf(ch, "The minimum %s bid on %s is %ld %ss.\n\r", 
	    ch->pCabal->currency, 
	    victim->name, 
	    min_cp,
	    ch->pCabal->currency);
    }
    else if (ch->in_bank < bbid->gp)
      send_to_char("You do not have enough gold in the bank.\n\r", ch );
    else if (ch->pCabal && GET_CP( ch ) < bbid->cp )
      sendf( ch, "You do not have enough %s%s.", ch->pCabal->currency, bbid->cp == 1 ? "" : "s" );
    else if (bbid->obj == NULL && bbid->cp == 0 && bbid->gp == 0)
      send_to_char("Your bid does has no value.\n\r", ch );
    else{
      /* we can now create a real bid, and transfer stuff over */
      BBID_DATA* new_bid;
      BOUNTY_DATA* bounty = NULL;
      OBJ_DATA* obj, *obj_next;

      /* check if we can add objects into this bid */
      if (bbid->obj
	  && (bounty = get_bounty( victim->name)) != NULL
	  && (new_bid = get_bbid( bounty, ch->name)) != NULL){
	int count = 0;
	for (obj = new_bid->obj; obj; obj = obj->next_bidobj )
	  count ++;

	if (count + bbid->level > 5){
	  if (5 - count >= 0)
	    sendf(ch, "You have previously placed %d items as a bounty and may place only %d more.\n\r", count, 5 - count);
	  else
	    send_to_char("You may only place up to 5 items as a bounty.\n\r", ch );
	  if (fPurge)
	    purge( victim );
	  return;
	}
      }

      new_bid = new_bbid();
      new_bid->name	= str_dup( ch->name );
      new_bid->level	= ch->level;
      new_bid->pCabal	= ch->pCabal;
      new_bid->cp	= ch->pCabal ? bbid->cp : 0;
      new_bid->gp	= bbid->gp;

      /* subtract gold, cp's */
      /* check if cabal can cover half */
      cost = bbid->cp;
      if (ch->pCabal && IS_CABAL(get_parent(ch->pCabal), CABAL_ROYAL)){
	CABAL_DATA* pCab = get_parent(ch->pCabal);
	if (GET_CAB_CP(pCab) - 500 > bbid->cp / 2){
	  coffers = bbid->cp / 2;
	  cost = bbid->cp / 2;
	}

	if (coffers){
	  char buf[MIL];
	  sprintf( buf, "%s has been given %d %s%s from our coffers.",
		   PERS2( ch ),
		   coffers,
		   pCab->currency,
		   coffers == 1 ? "" : "s");
	  cabal_echo( pCab, buf );
	  CP_CAB_GAIN( pCab, -coffers );
	}
      }
	
      CP_GAIN( ch, -cost, TRUE );
      ch->in_bank -= new_bid->gp;

      for (obj = bbid->obj; obj; obj = obj_next){
	obj_next = obj->next_bidobj;
	obj_from_bbid( obj );
	obj_from_char( obj );
	obj_to_bbid( obj, new_bid );
      }

      /* the new_bid now contains all the info, we get rid of the old one */
      free_bbid( ch->pcdata->pbbid );
      ch->pcdata->pbbid = NULL;

      /* we now add the bid for the victim */
      if ( (new_bid = add_bounty_bid( new_bid, victim->name )) == NULL){
	send_to_char("There was a problem setting your bounty.\n\r", ch );
	return;
      }
      else
	save_bounties();
      /* send a message to the victim */
      if (victim->pCabal && IS_CABAL(get_parent(victim->pCabal), CABAL_SPIES) ){
	if (!fPurge)
	  sendf( victim, "[%s] contacts in the underworld report %s's bounty on your head.\n\r",
		 victim->pCabal->who_name, ch->name);
	else{
	  char buf[MIL];
	  SET_BIT(victim->pcdata->messages, MSG_BOUNTY );
	  sprintf( buf, "[%s] contacts in the underworld report %s's bounty on your head.\n\r",
		   victim->pCabal->who_name, ch->name);
	  do_hal(victim->name, "A bounty has been set upon your head.", buf, NOTE_NOTE);
	}
      }
      sendf( ch, "Your current bounty for %s:\n\r", PERS2( victim ));
      show_bbid( ch, new_bid );
      send_to_char( "\n\r", ch );
      if (mob){
	act("$N nods ever so slightly and turns $S back on you.", ch, NULL, mob, TO_CHAR);
	act("$N nods ever so slightly and turns $S back on $n.", ch, NULL, mob, TO_ROOM);
      }
    }
    if (fPurge )
      purge( victim );
  }
  else if (!str_prefix( arg, "clear")){
    if (ch->pcdata->pbbid == NULL){
      send_to_char("You are not working on a bounty.\n\r", ch );
      return;
    }
    else{
      OBJ_DATA* obj, *obj_next;

      for (obj = ch->pcdata->pbbid->obj; obj; obj = obj_next){
	obj_next = obj->next_bidobj;
	obj_from_bbid( obj );
      }
      free_bbid( ch->pcdata->pbbid);
      ch->pcdata->pbbid = NULL;
      send_to_char("Bounty data cleared.\n\r", ch );
    }
    return;
  }
/* BUYOUT */
  else if (!str_prefix( arg, "buyout")){
    BOUNTY_DATA* bounty;
    CABAL_DATA* pCab = get_parent(ch->pCabal);
    CABAL_DATA* pRouge = NULL;
    PACT_DATA* pp;
    int cost = 0, coffers = 0;
    char buf[MIL];


    if (pCab == NULL){
      send_to_char("You must be a member of a cabal.\n\r", ch);
      return;
    }
    else if (!IS_CABAL(pCab, CABAL_ALLIANCE)){
      send_to_char("You lack the contacts neccessary.\n\r", ch);
      return;
    }
    /* check for an alliance with a rouge cabal */
    for (pp = pact_list; pp; pp = pp->next){
      if (pp->complete != PACT_COMPLETE)
	continue;
      if ( pp->type != PACT_ALLIANCE )
	continue;
      if (pp->benefactor == pCab &&  IS_CABAL(pp->creator, CABAL_ROUGE)){
	pRouge = pp->creator;
	break;
      }
      else if(IS_CABAL(pp->benefactor, CABAL_ROUGE) && pp->creator == pCab){
	pRouge = pp->benefactor;
	break;
      }
    }
    
    if (pRouge == NULL){
      send_to_char("You lack the contacts neccessary.\n\r", ch);
      return;
    }
    else if ( (bounty = ch->pcdata->pbounty) ==  NULL){
      send_to_char("There are no bounties associated with you.\n\r", ch );
      return;
    }
    
    /* get the cost */
    cost = get_buyout_cost( bounty );

    if (cost < 1){
      send_to_char("You may not buy out your bounty.\n\r", ch);
      return;
    }

    if (IS_CABAL(pCab, CABAL_ROYAL) && GET_CAB_CP(pCab) - 500 > cost / 2){
      cost = cost / 2;
      coffers = cost;
    }

    if (IS_NULLSTR(argument)){
      if (coffers){
	sendf(ch, "The bribe will cost you %d %s%s with %d being covered by [%s] coffers.\n\r", 
	      cost, 
	      pCab->currency,
	      cost == 1 ? "" : "s",
	      coffers,
	      pCab->who_name);
      }
      else{
	sendf(ch, "The bribe will cost %d %s%s.\n\r",
	      cost, 
	      pCab->currency,
	      cost == 1 ? "" : "s");
      }
      send_to_char("Use \"bounty buyout confirm\" to end the transaction.\n\r", ch);
    }
    else{
      if (GET_CP( ch ) < cost){
	send_to_char("You lack the funds neccessary.\n\r", ch);
	return;
      }
      else if (coffers && GET_CAB_CP(pCab) < coffers){
	send_to_char("Your cabal lacks the funds neccessary.\n\r", ch);
	return;
      }

      /* remove the bounty */
      rem_bounty( ch->pcdata->pbounty );
      free_bounty( ch->pcdata->pbounty );
      save_bounties();
      ch->pcdata->pbounty = NULL;
      /* mark time for re-biounty wait */
      ch->pcdata->last_bounty = mud_data.current_time;

      sprintf( buf, "%s has graciously donated %d %ss to our cause.",
	       PERS2( ch ),
	       cost + coffers,
	       pCab->currency);
      cabal_echo( pRouge, buf );

      if (coffers){
	sprintf( buf, "%s has been given %d %s%s from our coffers.",
		 PERS2( ch ),
		 coffers,
		 pCab->currency,
		 coffers == 1 ? "" : "s");
	cabal_echo( pCab, buf );
	CP_CAB_GAIN( pCab, -coffers );
      }
      CP_GAIN( ch, -cost, TRUE );
      CP_CAB_GAIN(pRouge, coffers + cost );
      save_char_obj( ch );
    }
    return;
  }
  /* check if we have any bounties on this person */
  else if (!IS_NULLSTR( arg )){
    BOUNTY_DATA* bounty = get_bounty( arg );
    BBID_DATA* bbid;

    if (bounty == NULL || (bbid = get_bbid( bounty, ch->name )) == NULL){
      sendf( ch, "You have not placed any bounties for their death.\n\r");
      return;
    }
    else{
      sendf( ch, "Your current bounty for %s:\n\r", bounty->name );
      show_bbid( ch, bbid );
      sendf(ch, "This bounty will expire in %ld days.\n\r",
	    (BOUNTY_LIFETIME - mud_data.current_time + bounty->time_stamp) / 86400);


    }
    return;
  }
  else
    do_bounty( ch, "" );
}


/* returns a single bid from a bounty to its owner */
void return_bbid( BBID_DATA* bbid, char* name ){
  CHAR_DATA* owner;
  DESCRIPTOR_DATA* d;
  ROOM_INDEX_DATA* to_room;
  OBJ_DATA* obj, *obj_next;

  bool fPurge = FALSE;
  int count = 0;
  
  if (bbid == NULL || IS_NULLSTR(name))
    return;

  /* get transfer room first */
  if ( (to_room = get_room_index(ROOM_VNUM_LIMBO)) == NULL){
    bug("return_bbid: could not get limbo room.", 0);
    return;
  }
  else if ( (owner = char_file_active (name)) == NULL){
    if ( (d = bring( to_room, name )) != NULL){
      owner = d->character;
      fPurge = TRUE;
      owner->pcdata->roomnum *= -1;
    }
    else{
      owner = NULL;
    }
  }
  /* at this point we stop if pfile no longer exists */
  if (owner == NULL){
    return;
  }

  /* transfer items from bid */
  for (obj = bbid->obj; obj; obj = obj_next){
    obj_next = obj->next_bidobj;
    
    obj_from_bbid( obj );

    if (!can_take(owner, obj)){
      extract_obj( obj );
      continue;
    }
    else{
      obj_to_char( obj, owner );
      count++;
    }
  }

  /* return gold */
  owner->in_bank += bbid->gp;
  CP_GAIN(owner, bbid->cp, TRUE );
  
  if (fPurge && count)
    SET_BIT(owner->pcdata->messages, MSG_BOUNTY_RETURN);
  else if (count){
    sendf( owner, "`@Your bounty on %s has been returned.``\n\r",
	   bbid->bounty->name);
  }
  if (fPurge)
    purge( owner );
}
/* returns an entire bounty to their owners */
void return_bounty( BOUNTY_DATA* bounty, BBID_DATA* exempt ){
  BBID_DATA* bbid, *bbid_next;

  for (bbid = bounty->bids; bbid; bbid = bbid_next){
    bbid_next = bbid->next;

    if (exempt && bbid == exempt)
      continue;
    return_bbid(bbid, bbid->name );
  }
}

/* writes a single bounty bid to a file */
void fwrite_bbid( FILE* fp, BBID_DATA* bbid ){
  OBJ_DATA* obj;

  fprintf( fp, "#BBID %s~ %d %d %d %ld\n",
	  bbid->name,
	  bbid->pCabal ? bbid->pCabal->vnum : 0,
	  bbid->level,
	  bbid->cp,
	  bbid->gp );

  /* write objects if any */
  for (obj = bbid->obj; obj; obj = obj->next_bidobj ){
    fwrite_obj(NULL, obj, fp, 0, 0, FALSE);
  }
/* terminate */
  fprintf( fp, "#End\n" );
}

/* reads a single bounty bid from file */
void fread_bbid( FILE* fp, BBID_DATA* bbid ){
  ROOM_INDEX_DATA* bounty_room;
  OBJ_DATA* obj;

  char letter;
  char* word;

  bbid->name		= fread_string( fp );
  bbid->pCabal		= get_cabal_vnum( fread_number( fp ) );
  bbid->level		= fread_number( fp );
  bbid->cp		= fread_number( fp );
  bbid->gp		= fread_number( fp );

  /* make sure we can get the bounty room */
  if ( (bounty_room = get_room_index( ROOM_VNUM_BOUNTY)) == NULL){
    bug("fread_bbid: could not get bounty room.", 0);
    exit( 1 );
  }

/* start the loop to read objects */
  for (;;){
    letter		= fread_letter( fp );
    if ( letter != '#' ){
      bug( "read_bbid: # not found.", 0 );
      exit( 1 );
    }
    word   = feof( fp ) ? "End" : fread_word( fp );
    if ( !str_cmp(word, "End"))
      break;
    else if (!str_cmp(word, "O") || !str_cmp(word, "OBJECT")){
      if ( (obj = fread_obj( NULL, fp, bounty_room )) == NULL){
	bug("fread_bbid: could not load object.", 0);
      }
      else{
	obj_to_bbid( obj, bbid );
      }
      continue;
    }
  }
}

/* writes a single bounty */
void fwrite_bounty( FILE* fp, BOUNTY_DATA* bounty ){
  BBID_DATA* bbid;
  struct stat Stat;	//File statistics
  char pFile [MIL];

  /* check if this player file exists */
  /* Get info/pointer to file entry*/
  sprintf(pFile, PLAYER_DIR"%s", bounty->name);
  if ( -1 == stat( pFile, &Stat))
    return;
  
  fprintf( fp, "#BOUNTY %s~ %ld\n", bounty->name, bounty->time_stamp );

  /* save bids */
  for (bbid = bounty->bids; bbid; bbid = bbid->next ){
    fwrite_bbid( fp, bbid );
  }
  
  /* terminate */
  fprintf( fp, "#End\n" );

}

/* reads a single bounty */
void fread_bounty( FILE* fp, BOUNTY_DATA* bounty ){
  char letter;
  char* word;

  bounty->name		= fread_string( fp );
  bounty->time_stamp	= fread_number( fp );

  /* read in the individual bids */
  for (;;){
    letter		= fread_letter( fp );
    if ( letter != '#' ){
      bug( "read_bounty: # not found.", 0 );
      exit( 1 );
    }
    word   = feof( fp ) ? "End" : fread_word( fp );
    if ( !str_cmp(word, "End"))
      break;
    else if (!str_cmp(word, "BBID")){
      BBID_DATA* bbid = new_bbid();
      fread_bbid( fp, bbid );
      bid_to_bounty( bbid, bounty );
    }
  }
}


/* saves all the bounties */
void save_bounties( ){
  FILE *fp;
  BOUNTY_DATA* bounty = bounty_list;
  char path[128];

  fclose( fpReserve );
  sprintf( path, "%s%s", SAVEBOUNTY_DIR, SAVEBOUNTY_FILE);
  if ( ( fp = fopen( path, "w" ) ) == NULL ){
    fp = fopen( NULL_FILE, "r" );
    fclose (fp);
    perror( path );
    return;
  }
/* loop through bounties and save */
  for (; bounty; bounty = bounty->next ){
    fwrite_bounty( fp, bounty );
  }
  fprintf( fp, "#END\n" );
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
}

/* checks bounties after loading in order to free the timed out ones */
void fix_bounties(){
  BOUNTY_DATA* bounty = bounty_list, *bounty_next;

/* loop through bounties and check */
  for (; bounty; bounty = bounty_next ){
    bounty_next = bounty->next;
    /* check if this bounty should be released */
    if (mud_data.current_time -  bounty->time_stamp > BOUNTY_LIFETIME){
      return_bounty( bounty, NULL );
      rem_bounty( bounty );
      free_bounty( bounty );
    }
  }
  save_bounties();
}

/* reads all bounties */
void load_bounties(){
  BOUNTY_DATA* bounty;
  FILE *fp;
  char path[128];
  char letter;
  char* word;

  fclose( fpReserve );
  sprintf( path, "%s%s", SAVEBOUNTY_DIR, SAVEBOUNTY_FILE);
  if ( ( fp = fopen( path, "r" ) ) == NULL ){
    fp = fopen( NULL_FILE, "r" );
    fclose (fp);
    perror( path );
    return;
  }
  
  for (;;){
    letter		= fread_letter( fp );
    if ( letter != '#' ){
      bug( "load_bounties: # not found.", 0 );
      exit( 1 );
    }
    word   = feof( fp ) ? "END" : fread_word( fp );
    if ( !str_cmp(word, "END"))
      break;
    else if (!str_cmp(word, "BOUNTY")){
      bounty = new_bounty();
      fread_bounty( fp, bounty );
      add_bounty( bounty );
    }
  }
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );

  fix_bounties();
}



/* handles all the misc. tasks after a bountied victim is killed */
void fulfill_bounty( CHAR_DATA *ch, CHAR_DATA *victim, char* name){
  CHAR_DATA* bidder;
  DESCRIPTOR_DATA* d;
  OBJ_DATA* obj, *obj_next;
  BBID_DATA* bbid;
  CABAL_DATA* pAlly = NULL;
  AFFECT_DATA* paf;

  const int cab_share = 75;	//percent taken by cabal,  if not allied kill
  
  long gold	= 0;
  int	cp	= 0;
  int cp_cab	= 0;
  int count	= 0;
  int corp_count= 0;

  bool fPurge = FALSE;

  char buf[MSL];
  char to[MSL];


/* safety */
  if (IS_NPC(ch) || IS_NPC(victim))
    return;
  else if (victim->pcdata->pbounty == NULL){
    bug("fulfill_bounty: victim with no bounty.\n\r", 0 );
    return;
  }
/* try to get the bid by name */
  if ( (bbid = get_bbid( victim->pcdata->pbounty, name)) == NULL){
    if ( (bbid = victim->pcdata->pbounty->bids) == NULL){
      bug("fulfill_bounty: bounty with no bids.", 0);
      return;
    }
  }
   
  /* set transfer for cp/gp, if this is not an allied kill, then cabal takes cab_share% to coffers */
  gold = pAlly ? bbid->gp : cab_share * bbid->gp / 100;
  cp_cab = pAlly ? 0 : cab_share * bbid->cp / 100;
  cp = bbid->cp - cp_cab;

  /* transfer bounty items to killer */
  for (obj = bbid->obj; obj; obj = obj_next){
    obj_next = obj->next_bidobj;
    
    obj_from_bbid( obj );

    if (!can_take(ch, obj)){
      sendf( ch, "You could not use %s.\n\rIt has been sold to help widowed families of %s%s%s.\n\r",
	     obj->short_descr, 
	     ch->pCabal ? "[" : "",
	     ch->pCabal ? ch->pCabal->who_name : victim->name,
	     ch->pCabal ? "]" : "");
      extract_obj( obj );
      continue;
    }
    else{
      obj_to_char( obj, ch );
      count++;
    }
  }
  if (ch->pCabal){
    /* do cp event bounty to reward */
    cp_event( ch, victim, NULL, CP_EVENT_BOUNTY);

    if (cp_cab > 0){
      sprintf( buf, "%d %s%s %s been directed to our coffers from %s's bounty.",
	       cp_cab, 
	       ch->pCabal->currency,
	       cp_cab == 1 ? "" : "s",
	       cp_cab == 1 ? "has" : "have",
	       PERS2(victim));
      cabal_echo( get_parent( ch->pCabal ), buf);
      CP_CAB_GAIN( ch->pCabal, cp_cab );
    }
    if (cp)
      CP_GAIN( ch, cp, TRUE );
  }
  /* transfer best three rares to the killer */
  corp_count = get_random_rares( ch, victim, 3, FALSE );

  /* if the bountier exists and is of rank > 40 he gets the rest of rares up to 18 */
  if ( (bidder = char_file_active (bbid->name)) == NULL){
    if ( (d = bring( ch->in_room, bbid->name )) != NULL){
      bidder = d->character;
      fPurge = TRUE;
      bidder->pcdata->roomnum *= -1;
    }
    else{
      bidder = NULL;
    }
  }
  if (bidder && bidder->level >= 40){
    int bidder_count = get_random_rares(bidder, victim, 18, FALSE );
    if (fPurge && bidder_count)
      SET_BIT(bidder->pcdata->messages, MSG_BOUNTY_REWARD);
    else if (bidder_count)
      sendf( ch, "`@You have recived %d items as a result of %s's death to a bounty hunter.``\n\r",
	     bidder_count, victim->name);
  }
  if (fPurge && bidder)
    purge( bidder );
  ch->in_bank += gold;
  if (gold || count || corp_count)
    sendf( ch, "You've recived %ld gold, %d items from the bounty and %d items from the corpse.\n\r",
	   gold, count, corp_count );

  /* we create a single note to all the people that bid on this bounty */
  to[0] = '\0';
  for (bbid = victim->pcdata->pbounty->bids; bbid; bbid = bbid->next){
    strcat(to, bbid->name);
    strcat(to, " ");
  }
  sprintf( buf, warn_table[WARN_BOUNTY].subj, victim->name );
  
  do_hal( to, buf, warn_table[WARN_BOUNTY].text, NOTE_NOTE);

  /* get rid of contract */
  if ( (paf = affect_find( ch->affected, gen_contract)) != NULL){
    paf->modifier = TRUE;
    affect_remove( ch, paf );
  }
  /* return rest of the bids */
  return_bounty( victim->pcdata->pbounty, bbid);

  /* we clear the bounty for the victim */
  rem_bounty( victim->pcdata->pbounty );
  free_bounty( victim->pcdata->pbounty );
  save_bounties();
  victim->pcdata->pbounty = NULL;
  /* mark time for re-biounty wait */
  victim->pcdata->last_bounty = mud_data.current_time;

  /* increae the count of bounties collected */
  mud_data.bounties++;
/* if this is a cabaled killer from bounty cabal, increase bounty count */
  if (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_ROUGE))
    ch->pcdata->member->kills++;
  save_mud();
}

/* handles selection and collection of bounty */
/* SYNTAX:
   collect <name>		: collects first avaliable bounty
   collect <name> <number>	: collects the selected bounty
   collect list <name>		: list all bids by cp
   collect gold <name>		: list by gold
   collect obj <name>		: list by obj
   collect list <name> <number>	: list the bid in detail

*/
void do_bounty_collect( CHAR_DATA *ch, char *argument ){

  AFFECT_DATA* paf = NULL;
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  
  char arg[MIL];
  char buf[MSL];
  
  argument = one_argument( argument, arg);
  
  if ((ch->pCabal == NULL || !IS_CABAL(ch->pCabal, CABAL_ROUGE))
      && ( (paf = affect_find(ch->affected, gen_contract)) == NULL
	   || !paf->has_string
	   || IS_NULLSTR(paf->string)) ){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  else if (IS_NULLSTR( arg )){
    send_to_char("Seek scrolls of \"help collect\" for details.\n\r", ch );
    return;
  }
/* LIST */
  else if (!str_cmp( "list", arg) || !str_cmp( "gold", arg) || !str_cmp( "obj", arg)){
      BOUNTY_DATA* bounty;
      BBID_DATA* bbid;
      char arg2[MIL];
      argument = one_argument( argument, arg2);
  
      if (paf)
	strcpy(arg2, paf->string);
	  
      if (IS_NULLSTR( arg2 )){
	send_to_char("List the bounty for who?\n\r", ch );
	return;
      }
      else if ( (bounty = get_bounty( arg2)) == NULL){
	send_to_char("There is no price on their head.\n\r", ch);
	return;
      }
      /* VIEW */
      if (!IS_NULLSTR( argument )){
	if (atoi( argument ) < 1){
	  send_to_char("View which bounty? (number)\n\r", ch );
	  return;
	}
	else if ( (bbid = get_bbid( bounty, argument)) == NULL){
	  sendf(ch, "No such bounty foundf or %s.\n\r", bounty->name );
	  return;
	}
	else{
	  sendf(ch, "Victim: %-15s  Bounty: %d\n\r", bounty->name, atoi(argument) );
	  show_bbid( ch, bbid );
	}
	return;
      }
      else
	show_bounty( ch, bounty, !str_cmp(arg, "gold"), !str_cmp(arg, "obj") );
      return;
    }
    else if ( (victim = get_char_room( ch, NULL, arg ) ) == NULL ){
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }
  else if (!can_collect(victim->pcdata->pbounty, ch)){
    sendf(ch, "You cannot collect bounty on %s\n\r.", PERS2(victim) );
    return;
  }
  else if (IS_NPC(ch) || IS_NPC(victim)){
    send_to_char("Not on mobiles.\n\r", ch );
    return;
  }
  else if (!is_pk_abs(ch, victim)){
    send_to_char("The gods will not allow you to harm them.\n\r", ch);
    return;
  }
  else if ( !is_affected(victim, gsn_mortally_wounded) ){
    sendf(ch, "%s is not standing still enough for you to collect a bounty.\n\r", PERS2(victim));
    return;
  }
  
  sprintf(buf,"AAAAAAGGGGGHHHHH!!!!");
  do_yell(victim, buf);
  
  obj = create_object(get_obj_index(OBJ_VNUM_SEVERED_HEAD),ch->level);
  
  sprintf(buf, obj->short_descr, victim->name);
  free_string( obj->short_descr );
  obj->short_descr = str_dup( buf );
  
  sprintf(buf, obj->description, victim->name);
  free_string( obj->description );
  obj->description = str_dup( buf );
  
  obj_to_char(obj, ch);
  
  act("You slice off $N's head and place it in a bag.",ch,NULL,victim,TO_CHAR);
  act("$n slices off $N's head and places it in a bag.",ch,NULL,victim,TO_NOTVICT);
  act("$n slices off your head!",ch,NULL,victim,TO_VICT);
  act("$n's headless body twitches violently.",victim,NULL,NULL,TO_ROOM);
  act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
  send_to_char( "You have been `1KILLED`` by a Bounty Hunter!\n\r",victim );

  fulfill_bounty( ch, victim, IS_NULLSTR( argument ) ? "1" : argument );
  
  _raw_kill( ch, victim, FALSE );
  affect_strip(ch, skill_lookup("analyze"));
}


/* checks if a given bounty has allied priority when looked at by pCabal */
CABAL_DATA* is_bounty_priority( BOUNTY_DATA* bounty, CABAL_DATA* pCabal ){
  CABAL_DATA* pCab = get_parent( pCabal );
  BBID_DATA* bbid;

  if (bounty == NULL || pCab == NULL)
    return NULL;
  else if (!IS_CABAL(pCab, CABAL_ROUGE) || !IS_CABAL(pCab, CABAL_ALLIANCE))
    return NULL;

  /* check bids in the bounty for alliances with this rouge cabal */
  for (bbid = bounty->bids; bbid; bbid = bbid->next ){
    if (bbid->pCabal && IS_CABAL(bbid->pCabal, CABAL_ALLIANCE) && get_parent(bbid->pCabal) != pCab 
	&& get_pact(bbid->pCabal, pCab, PACT_ALLIANCE, FALSE) != NULL)
      return bbid->pCabal;
  }
  return NULL;
}

/* checks if given character can be covered by any rouge cabal memebers */
bool bounty_covered( CHAR_DATA* victim ){
  if (IS_NPC(victim) || victim->pcdata->pbounty == NULL)
    return TRUE;
  else{
    CHAR_DATA* vch;
    for (vch = player_list; vch; vch = vch->next_player){
      if (vch->pCabal && !IS_IMMORTAL(vch) && IS_CABAL(vch->pCabal, CABAL_ROUGE)
	  && is_pk_abs(vch, victim))
	return TRUE;
    }
    return FALSE;
  }
}
  

/* command used to get a contract on a bounty rouge cabals cannot cover */
void do_contract( CHAR_DATA* ch, char* argument ){
  CHAR_DATA* vch;
  CHAR_DATA* mob;
  int count = 0;
  
  if (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_ROUGE)){
    send_to_char("Use \"who bounty\" to see bounties avaliable.\n\r", ch);
    return;
  }
  for (mob = ch->in_room->people; mob; mob = mob->next_in_room ){
    if (IS_NPC(mob) && IS_SET(mob->act, ACT_CONTRACTER))
      break;
  }

  if (IS_NULLSTR(argument)){
    bool fFound = FALSE;
    send_to_char("Following bounties can be contracted out to you:\n\r", ch );
    for (vch = player_list; vch; vch = vch->next_player){
      if (vch->pcdata->pbounty && vch != ch && is_pk_abs(ch, vch) 
	  && can_see(ch, vch) && !bounty_covered( vch )){
	sendf( ch, "%2d. %s\n\r", ++count, vch->name);
	fFound = TRUE;
      }
    }
    if (!fFound)
      send_to_char("None.\n\r", ch);
    return;
  }
  else if (is_affected(ch, gen_contract)){
    send_to_char("You are still bound by previous contract.\n\r", ch);
    return;
  }
  else if ( (vch = get_char_world( ch, argument)) == NULL){
    send_to_char("They do not seem to be present.\n\r", ch );
    return;
  }
  else if (ch == vch){
    send_to_char("You cannot contract yourself.\n\r", ch);
    return;
  }
  else if (bounty_covered( vch) || !is_pk_abs(ch, vch)){
    send_to_char("Their bounty is not avaliable to be contracted out by you.\n\r", ch);
    return;
  }
  else{
    AFFECT_DATA af, *paf;
    
    af.type	= gen_contract;
    af.level	= ch->level;
    af.duration	= 72;
    
    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location	= APPLY_NONE;
    af.modifier	= FALSE;

    paf = affect_to_char( ch, &af );
    string_to_affect( paf, vch->name );

    if (mob ){
      act("$N tells you 'You have 72 hours to do the deed, or until someone becomes avaliable.'", ch, NULL, mob, TO_CHAR);
      act("$N nods ever so slightly and turns $S back on $n.", ch, NULL, mob, TO_ROOM);
    }
    else{
      send_to_char("There is no one here to take your contract.\n\r", ch);
    }
  }
}

/* checks if the person can collect the bounty */
bool can_collect( BOUNTY_DATA* bounty, CHAR_DATA* ch ){
  CHAR_DATA* och = ch;
  AFFECT_DATA* paf;

  if (IS_NPC(ch) && ch->master)
    och = ch->master;
  else if (IS_NPC(ch) && ch->leader)
    och = ch->leader;

  if (IS_NPC(och) || bounty == NULL)
    return FALSE;
  else if ( och->pCabal != NULL && IS_CABAL(och->pCabal, CABAL_ROUGE) )
    return TRUE;
  else if ( (paf =  affect_find(och->affected, gen_contract)) != NULL 
	    && paf->has_string 
	    && !IS_NULLSTR(paf->string) 
	    && is_name(paf->string, bounty->name)){
    return TRUE;
  }
  else
    return FALSE;
}
	    
bool check_rouge_bounty( CHAR_DATA* och, int pen ){
  CHAR_DATA* ch = och;
  CHAR_DATA* vch;
  CABAL_DATA* pc; 
  char buf[MIL];
  int sn = skill_lookup("analyze");

  if (IS_NPC(och) && och->master)
    ch = och->master;

  if (ch->pCabal == NULL || !IS_CABAL(ch->pCabal, CABAL_ROUGE))
    return FALSE;
  else 
    pc = get_parent( ch->pCabal);

/* penalty for not collecting bounties */
  for (vch = player_list; vch; vch = vch->next_player){
    if (vch->pcdata->pbounty 
	&& !is_affected(vch, gsn_noquit )
	&& can_see(ch, vch) 
	&& is_pk(ch, vch)
	&& can_collect(vch->pcdata->pbounty, ch))
      break;
  }
  
  return (vch != NULL);

/* Viri: no longer used */
  if (vch != NULL){
    AFFECT_DATA *paf;
    int strike = 0;

    // give them three strikes 
    if ( (paf = affect_find(ch->affected, sn)) == NULL){
      AFFECT_DATA af;

      af.type	= sn;
      af.level	= 60;
      af.duration = -1;
      af.where = TO_NONE;
      af.bitvector = 0;
      af.location	= APPLY_NONE;
      af.modifier = 1;
      affect_to_char(ch, &af);
      strike = 1;
    }
    else{
      paf->modifier++;
      strike = paf->modifier;
    }

    if (strike > 3){
      pen = (strike - 3) * pen;
      strike++;
      sprintf( buf, "%s has ignored a bounty!", ch->name);
      cabal_echo( pc, buf );
      distribute_cps(ch, ch->pCabal, pen * CPTS, TRUE );
    
      // if person is out of cps we demote them 
      // we subtract the gain since reward takes effect on tick, not instantenously 
      if ((GET_CP( ch ) < 1) && ch->pcdata->rank > RANK_NEWBIE ){
	char sub[MIL];
	sprintf( buf, "Due to disregard for bounties, %s has been demoted.", PERS2(ch) );
	cabal_echo( pc, buf );
	strcat( buf, "\n\r");
	sprintf( sub, "%s's demotion.", PERS2(ch));
	do_hal( pc->name, sub, buf, NOTE_NEWS );
	promote( ch, ch->pCabal, -1 );
	if (ch->pcdata->member)
	  ch->pcdata->member->time_stamp = mud_data.current_time;
      }
    }
    else{
      sprintf( buf, "%s can ignore %d more %s.", 
	       ch->name,
	       (3 - strike),
	       (3 - strike) == 1 ? "bounty" : "bounties");
      cabal_echo( pc, buf );      
    }
    return TRUE;
  }
  else
    affect_strip(ch, sn );
  return FALSE;
}
