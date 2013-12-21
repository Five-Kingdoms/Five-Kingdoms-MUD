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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <malloc.h>
#include "merc.h"
#include "vote.h"
#include "cabal.h"
#include "save_mud.h"
#include "recycle.h"
#include "interp.h"
#include "jail.h"
#include "olc.h"
#include "db.h"
#include "armies.h"
#include "tome.h"
#include "clan.h"


/***************************************************************/
/*Following are vote orientanted routines for Forsaken Lands  */
/*mud created by Virigoth circa 2002.  Copyrighted for Forsaken*/
/*Lands mud Feb 20, 2002.  Do not use or copy without explicit */
/*permission of author. (Voytek Plawny aka Virigoth)	       */
/***************************************************************/

extern void connect_char( DESCRIPTOR_DATA* d );

/* global vote list */
VOTE_DATA* vote_list = NULL;

/* table listing all the possible votes, entries MUST correspond to the "type" constant */
const struct vote_type	vote_table	[]	=
{
/* name					life_time		flags				*/
  {"none",				0,			0 },
  {"general vote",			VOTE_DUR_LONG,		VOTE_URGE | VOTE_SUBJ | VOTE_NOOWNER | VOTE_DUP  | VOTE_NOCAB_COST},
  {"cabal application",			VOTE_DUR_LONG,		VOTE_NOOWNER },
  {"promotion",				VOTE_DUR_VLONG,		VOTE_URGE },
  {"demotion",				VOTE_DUR_LONG,		0 },
  {"eldership",				VOTE_DUR_VLONG,		0 },
  {"leadership",			VOTE_DUR_VLONG,		0 },
  {"poll",				VOTE_DUR_MED,		VOTE_URGE | VOTE_SUBJ | VOTE_NOOWNER | VOTE_DUP | VOTE_NOCAB_COST},
  {"laws",				VOTE_DUR_VLONG,		VOTE_URGE },
  {"pact",				VOTE_DUR_VLONG,		VOTE_URGE },
  {"cabal tax",				VOTE_DUR_MED,		VOTE_URGE },
  {"build",				VOTE_DUR_MED,		VOTE_URGE },
  {"tithe",				VOTE_DUR_SHORT,		VOTE_URGE },
  {"expel",				VOTE_DUR_VLONG,		VOTE_URGE },
  {"sponsorship",			VOTE_DUR_LONG,		VOTE_URGE | VOTE_NOOWNER},
  {"tome",				VOTE_DUR_VLONG,		0 },

  {NULL, 0, 0}
};

int vote_lookup (const char *name ){
    int vote;
    for (vote = 0; vote_table[vote].name != NULL; vote++){
      if (LOWER(name[0]) == LOWER(vote_table[vote].name[0]) && !str_prefix(name, vote_table[vote].name))
	return vote;
    }
    return 0;
}

void show_votes( CHAR_DATA* ch ){
  int vote;
  for (vote = 1; vote_table[vote].name != NULL; vote++){
    sendf( ch, "%-2d. %-30s [%-2d days]\n\r",
	   vote,
	   vote_table[vote].name,
	   vote_table[vote].life_time / VOTE_DAY);
  }
}

/* creates destroys votes */
VOTE_DATA *vote_free;
VOTE_DATA *new_vote()
{
    VOTE_DATA *vote;
    if (vote_free == NULL)
	vote = alloc_perm(sizeof(*vote));
    else
    {
	vote = vote_free;
	vote_free = vote_free->next;
    }
    VALIDATE(vote);
    return vote;
}

void free_vote(VOTE_DATA *vote)
{
    if (!IS_VALID(vote))
	return;

    if (!IS_NULLSTR(vote->owner))
      free_string( vote->owner);

    if (!IS_NULLSTR(vote->from))
      free_string( vote->from );

    if (!IS_NULLSTR(vote->subject))
      free_string( vote->subject );

    if (!IS_NULLSTR(vote->string))
      free_string( vote->string );

    INVALIDATE(vote);
    vote->next = vote_free;
    vote_free   = vote;
}

/* adds a vote onto the vote list */
void add_vote( VOTE_DATA* pv ){
  CHAR_DATA* vch;
  if (vote_list == NULL){
    vote_list = pv;
    pv->next = NULL;
  }
  else{
/* we always insert the votes from oldest to newest */
    VOTE_DATA* last = vote_list;
    for (last = vote_list; last->next != NULL; last = last->next);
    pv->time_stamp = UMAX(last->time_stamp + 1, pv->time_stamp );
    last->next = pv;
    pv->next = NULL;
  }
  /*  Let char know he/they recived a vote */
  for ( vch = player_list; vch != NULL; vch = vch->next_player ){
    if (is_vote_to(vch, pv) )
      act_new("`8A messenger hands you a scroll marked \"vote\"``.", vch, NULL, NULL, TO_CHAR, POS_DEAD);
  }
}

/* removes a vote from the vote list */
bool rem_vote( VOTE_DATA* pv ){
  VOTE_DATA* prev = vote_list;
  if (vote_list == NULL){
    bug("rem_vote: vote_list null.", 0);
    return FALSE;
  }
  if (prev == pv )
    vote_list = pv->next;
  else{
    while (prev->next && prev->next != pv){
      prev = prev->next;
    }
  }
  if (prev == NULL){
    bug("rem_vote: vote note found.", 0);
    return FALSE;
  }
  prev->next = pv->next;
  pv->next = NULL;
  return TRUE;
}

/* writes the data of a single vote */
/* votes are saved in mudsave file */
void write_vote(FILE* fp, VOTE_DATA* vote){
  int i = 0;
  /* owner, subject, type */
  fprintf( fp, "%s~ %s~ %s~ %d\n", vote->owner, vote->from, vote->subject, vote->type);

  /* votes yes, no, pass, max vaote*/
  fprintf( fp, "%d %d %d %d\n", vote->yes, vote->no, vote->pass, vote->max_vote );

  /* time stamp, and lifetime */
  fprintf( fp, "%ld %ld\n", vote->time_stamp, vote->life_time);

  /* values */
  fprintf(fp, "%d ", VOTE_VALS);
  for (i = 0; i < VOTE_VALS; i++){
    fprintf( fp, "%d ", vote->value[i]);
  }
  fprintf( fp, "\n");

  /* text/string */
  fprintf( fp, "%s~", vote->string);

  fprintf( fp, "\n" );
}


/* reads the data of a single vote */
void read_vote(FILE* fp, VOTE_DATA* vote){
  int vals = VOTE_VALS;
  int i = 0;

  vote->owner		= fread_string( fp );
  vote->from		= fread_string( fp );
  vote->subject		= fread_string( fp );
  vote->type		= fread_number( fp );

  vote->yes		= fread_number( fp );
  vote->no		= fread_number( fp );
  vote->pass		= fread_number( fp );
  vote->max_vote	= fread_number( fp );

  vote->time_stamp	= fread_number( fp );
  vote->life_time	= fread_number( fp );

  vals			= fread_number( fp );
  for (i = 0; i < VOTE_VALS; i ++){
    if (i < vals)
      vote->value[i] = fread_number( fp );
    else
      vote->value[i] = 0;
  }
  vote->string		 = fread_string( fp );
  VALIDATE( vote );
}

/* sets up a vote weight based on cabal */
int get_max_vote( CABAL_DATA* pc, int type ){
  CMEMBER_DATA* cm;
  int mod = 1;
  int max_vote = 0;

  if (IS_CABAL(pc, CABAL_ROYAL) ){
    mod = 10;
/* Viri: Not used, citizens can try to sway the royal votes
   check if this is one of the votes that citizens vote on
    if (type == VOTE_CAB_APPLY
	|| type == VOTE_PACT
	|| type == VOTE_CAB_SPONS
	|| type == VOTE_LAW){
      max_vote += mud_data.pfiles / 15;
    }
*/
  }

  /* run through all members of the cabal and tally up the total */
  for (cm = get_parent(pc)->members; cm; cm = cm->next ){
    if (mud_data.current_time - cm->last_login > 4 * VOTE_DAY)
      continue;
    if (cm->rank == RANK_LEADER){
      if (str_cmp(cm->name, get_parent(cm->pCabal)->immortal))
	max_vote += VOTE_WEIGHT_LEADER * mod;
    }
    else if (cm->rank == RANK_ELDER)
      max_vote += VOTE_WEIGHT_ELDER * mod;
    else
      max_vote += VOTE_WEIGHT_NORM * mod;
  }

/*
  max_vote += (pc->cur_member - pc->cur_elder - pc->cur_leader) * VOTE_WEIGHT_NORM * mod;
  max_vote += pc->cur_elder * VOTE_WEIGHT_ELDER * mod;
  max_vote += pc->cur_leader * VOTE_WEIGHT_LEADER * mod;
*/

  max_vote = UMAX(1, max_vote);
  return max_vote;
}

/* creates a fresh vote based on info passed */
VOTE_DATA* init_vote( int type, char* owner, char* from, char* subject, char* string, int max_vote, int life_time){
  VOTE_DATA* pv = NULL;

/* safety */
  if (IS_NULLSTR(owner) || IS_NULLSTR(subject)){
    bug("init_vote: null owner or subject on vote type %d", type);
    return pv;
  }

  if (max_vote < 1)	max_vote = VOTE_DEFMAX;
  if (life_time < 1)	life_time = VOTE_DUR_MED;

/* get the vote pointer */
  if ( (pv = new_vote()) == NULL){
    bug("init_vote: could not get a new pointer", 0);
    return pv;
  }

/* reset data */
  pv->type	=	type;
  pv->owner	=	str_dup( owner );
  pv->from	=	str_dup( from );
  pv->subject	=	str_dup( subject );
  pv->string	=	str_dup( string );

  pv->yes	=	0;
  pv->no	=	0;
  pv->pass	=	0;
  pv->veto	=	FALSE;

  pv->max_vote	=	max_vote;
  pv->time_stamp=	mud_data.current_time;
  pv->life_time =	pv->time_stamp + life_time;

  return pv;
}

/* sets up the vote based on the type and info passed */
VOTE_DATA* create_vote(CHAR_DATA* ch, char* owner, char* subject, char* string, int type,
		       int v0, int v1, int v2, int v3, int v4){
  VOTE_DATA* pv = NULL;
  CABAL_DATA* pc = ch->pCabal;	//parent of cabal
  CABAL_DATA* po = ch->pCabal;	//original cabal
  CABAL_DATA* pc2 = ch->pCabal;	//second cabal as needed
  CABAL_DATA* po2 = ch->pCabal;	//second cabal as needed

  char from[MIL]; //this is almost never changed, unless its from a cabal name.

  char v_subj[MIL];

  int value[VOTE_VALS];
  int max_vote = 0;
  int life_time = 0;

  int i = 0;

  if (ch == NULL)
    return pv;
  else
    sprintf(from,"%s", ch->name);

/* reset values */
  for (i = 0; i < VOTE_VALS; i++){
    value[i] = 0;
  }

/* set values up based on type */
  switch ( type ){
  case VOTE_CAB_APPLY:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/CAB_APPLY: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    /* change subject to match now */
    sprintf(v_subj, "Admission to %s", pc->name);
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    value[0] = pc->vnum;
    value[1] = po->vnum;
    break;
  case VOTE_CAB_PROM:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/CAB_PROM: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    /* change subject to match now */
    sprintf(v_subj, "Promotion of %s", owner);
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    sprintf(from, "%s", pc->who_name);

    value[0] = po->vnum;
    value[1] = pc->vnum;
    break;
  case VOTE_CAB_DEMO:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/CAB_DEMO: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    /* change subject to match now */
    sprintf(v_subj, "Demotion of %s", owner);
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    sprintf(from, "%s", pc->who_name);

    value[0] = pc->vnum;
    break;
  case VOTE_CAB_EXPEL:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/CAB_EXPEL: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    /* change subject to match now */
    sprintf(v_subj, "Expulsion of %s from %s", owner, po->who_name);
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    sprintf(from, "%s", pc->who_name);

    value[0] = po->vnum;
    value[1] = pc->vnum;
    break;
  case VOTE_CAB_ELDER:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/CAB_ELDER: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    /* change subject to match now */
    sprintf(v_subj, "Proposed Eldership of %s", owner);
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    sprintf(from, "%s", pc->who_name);
    value[0] = po->vnum;
    value[1] = pc->vnum;
    break;
  case VOTE_CAB_LEADER:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/CAB_LEADER: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    /* change subject to match now */
    sprintf(v_subj, "Proposed Leadership of %s", owner);
    max_vote = get_max_vote( pc, type );
    sprintf(from, "%s", pc->who_name);

    value[0] = po->vnum;
    value[1] = pc->vnum;
    break;
  case VOTE_CAB_POLL:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/CAB_POLL: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    sprintf(v_subj, "%s", subject);
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    value[0] = po->vnum;
    value[1] = pc->vnum;
    break;
  case VOTE_TOME:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/TOME: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    /* create title for the author */
    sprintf(from, "%s %s%s%s",
	    get_crank(ch),
	    ch->name,
	    IS_NULLSTR(ch->pcdata->family)? "" : " ",
	    IS_NULLSTR(ch->pcdata->family)? "" : ch->pcdata->family);
    sprintf(v_subj, "%s", subject);
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    value[0] = po->vnum;
    value[1] = pc->vnum;
    break;
  case VOTE_LAW:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/LAW: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    sprintf(v_subj, "Proposed change of %s to %s", crime_table[v1].name, punish_table[v2].name);
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    sprintf(from, "%s", pc->who_name);

    value[0] = pc->vnum;
    value[1] = v1;
    value[2] = v2;
    break;
  case VOTE_CAB_TAX:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/TAX: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    sprintf(v_subj, "Proposed change of tax from %d%% to %d%%", pc->cp_tax, v1 );
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    sprintf(from, "%s", pc->who_name);

    value[0] = pc->vnum;
    value[1] = v1;
    break;
  case VOTE_CAB_SPONS:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/SPONSOR: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    sprintf(v_subj, "Proposed %s of sponsorship for inductions.", IS_CABAL(pc, CABAL_SPONSOR) ? "removal" : "requirement");
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    value[0] = pc->vnum;
    break;
  case VOTE_TITHE:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/TITHE: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    if ( (po2 = get_cabal_vnum( v1 )) == NULL){
      bug("create_vote/TITHE: could not get second cabal index.", 0);
      return pv;
    }
    pc2 = get_parent( po2 );

    sprintf(v_subj, "Proposed tithe of %d %s%s to %s", v2, pc->currency, v2 == 1 ? "" : "s", pc2->who_name);
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    sprintf(from, "%s", pc->who_name);

    value[0] = pc->vnum;
    value[1] = v1;
    value[2] = v2;
    break;
  case VOTE_PACT:
    /* get creator pointer */
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/PACT: could not get creator index.", 0);
      return pv;
    }
    else {
      pc = get_parent( po );
    }
    /* get benefactor index */
    if ( (po2 = get_cabal_vnum( v1 )) == NULL){
      bug("create_vote/PACT: could not get benefactor index.", 0);
      return pv;
    }
    else {
      pc2 = get_parent( po2 );
    }
/* vendetta and embargo votes do not create a mirror vote, rather the pact is completed as soon as the vote
   in one cabal is success */
    if (!IS_SET(v4, PACT_VENDETTA) && !IS_SET(v4, PACT_BREAK)){
      /* we first create a mirror image of this vote for the benefactor cabal */
      sprintf(v_subj, "Proposal of %s with [%s]",
	      IS_SET(v4, PACT_IMPROVE) ? flag_string(pact_flags, v2) : flag_string(pact_flags, v4),
	      pc->who_name );
      max_vote = get_max_vote( pc2, type );
      life_time = vote_table[type].life_time;
      sprintf(from, "%s", pc2->who_name);

      value[0] = pc2->vnum;	//creator vnum
      value[1] = pc->vnum;	//benefactor vnum

      /* Service Cabals give negative gain on all trade pacts to the other cabal */
      if (IS_CABAL(pc, CABAL_SERVICE) && !IS_SET(v4, PACT_IMPROVE)){
	value[2] = TRADE_CP_SERVE;		//cp % gain for benefactor
	value[3] = TRADE_SUP_SERVE;		//sup % gain for benefactor
      }
      else{
	value[2] = v2;		//cp % gain for benefactor
	value[3] = v3;		//sup % gain for benefactor
      }
      value[4] = v4;		//type of pact

      if (mud_data.mudport == TEST_PORT && IS_IMMORTAL(ch))
	life_time = VOTE_MINUTE;

      if ( (pv = init_vote( type, owner, from, v_subj, string, max_vote, life_time)) == NULL){
	bug("create_vote: VOTE_PACT could not initialize vote.(type %d)", type);
	return pv;
      }
      /* set the values up */
      for (i = 0; i < VOTE_VALS; i++){
	pv->value[i] = value[i];
      }
      /* attach the vote */
      add_vote( pv );
      /* reset values */
      for (i = 0; i < VOTE_VALS; i++){
	value[i] = 0;
      }
    }//END if not vendetta
    /* do the original now */

    sprintf(v_subj, "Proposal of %s with [%s]",
	    IS_SET(v4, PACT_IMPROVE) ? flag_string(pact_flags, v2) : flag_string(pact_flags, v4),
	    pc2->who_name );
    max_vote = get_max_vote( pc, type );
    life_time = vote_table[type].life_time;
    sprintf(from, "%s", pc->who_name);

    value[0] = pc->vnum;	//creator vnum
    value[1] = pc2->vnum;	//benefactor vnum
    /* service cabals do not give the other cabal any gain, but make the area lawful
       with exception of vendetta votes */
    if (IS_CABAL(pc2, CABAL_SERVICE) && !IS_SET(v4, PACT_VENDETTA) && !IS_SET(v4, PACT_IMPROVE) ){
      value[2] = TRADE_CP_SERVE;		//cp % gain for benefactor
      value[3] = TRADE_SUP_SERVE;		//sup % gain for benefactor
    }
    else{
      value[2] = v2;		//cp % gain for creator
      value[3] = v3;		//sup % gain for creator
    }
    value[4] = v4;		//type of pact
    break;
  case VOTE_BUILD:
    if ( (po = get_cabal_vnum( v0 )) == NULL){
      bug("create_vote/BUILD: could not get cabal index.", 0);
      return pv;
    }
    pc = get_parent( po );

    sprintf(v_subj, "%s", subject );
    max_vote = get_max_vote( pc, type );
    life_time = get_trust(ch) == IMPLEMENTOR ? VOTE_MINUTE : vote_table[type].life_time;
    value[0] = pc->vnum;
    value[1] = v1;
    value[2] = v2;
    value[3] = v3;
    value[4] = v4;
    break;
  default:
  case VOTE_GENERAL:
    sprintf(v_subj, "%s", subject);
    max_vote = 3 * mud_data.pfiles / 4;
    life_time = vote_table[type].life_time;
    break;
  }

  /* test port has very short lifetiems */
  if (mud_data.mudport == TEST_PORT)
    life_time = VOTE_MINUTE;

  if ( (pv = init_vote( type, owner, from, v_subj, string, max_vote, life_time)) == NULL){
    bug("create_vote: could not initialize vote.(type %d)", type);
    return pv;
  }
  /* set the values up */
  for (i = 0; i < VOTE_VALS; i++){
    pv->value[i] = value[i];
  }
  /* test for duplicate */
  if (is_duplicated( pv )){
    send_to_char("Duplicate vote.\n\r", ch);
    return NULL;

  }
  /* attach the vote */
  return pv;
}

/* shows a list of all votes */
void list_votes( CHAR_DATA* ch){
  VOTE_DATA* pv = vote_list;
  BUFFER *buffer;
  char buf[MSL];
  int counter = 0;

  buffer=new_buf();
  buf[0] = 0;

  for (;pv != NULL; pv = pv->next){
    if (!is_vote_to( ch, pv) && get_trust(ch) < IMPLEMENTOR && str_cmp(ch->name, pv->owner)){
      continue;
    }
    else
      counter++;
    if (IS_IMMORTAL(ch)){
      sprintf(buf, "%3d.[%-10s] %-40s: Y:%-3d N:%-3d P:%-3d Tot: %d [%ld D] [%s]\n\r",
	      counter,
	      pv->from,
	      pv->subject,
	      pv->yes, pv->no, pv->pass, pv->max_vote,
	      ( pv->life_time - mud_data.current_time) / VOTE_DAY,
	      pv->owner);
    }
    else{
      sprintf(buf, "%3d.[%-10s] %-40s:[%-2ld D]\n\r",
	      counter,
	      pv->from,
	      pv->subject,
	      ( pv->life_time - mud_data.current_time) / VOTE_DAY );

    }
    add_buf(buffer,buf);
  }
  if (IS_NULLSTR(buf)){
    send_to_char("No votes found.\n\r", ch);
  }
  else{
    page_to_char(buf_string(buffer),ch);
  }
  free_buf(buffer);
}

/* finds cabal vote data based on type NULL if not found*/
CVOTE_DATA* get_cvote( CABAL_INDEX_DATA* pCabal, int type ){
  CVOTE_DATA* cVote;
  CABAL_INDEX_DATA* pCab = pCabal;
  CABAL_INDEX_DATA* pPar;

  if (pCab == NULL)
    return NULL;

  /* we run through the cabal and its partents looking for the vote */
  while (pCab != NULL){
    for( cVote = pCab->votes; cVote; cVote = cVote->next ){
      if (cVote->vote == type)
	return cVote;
    }
    if ( pCab->parent_vnum
	 && (pPar = get_cabal_index(pCab->parent_vnum)) != NULL)
      pCab = pPar;
    else
      pCab = NULL;
  }
  return NULL;
}

/* checks if the player can create a given vote */
bool can_create_cvote( CHAR_DATA* ch, CVOTE_DATA* cVote){
  if (ch == NULL || cVote == NULL)
    return FALSE;

  if (ch->pcdata->rank < cVote->min || ch->pcdata->rank > cVote->max)
    return FALSE;

  return TRUE;
}


/* shows votes avaliable to character */
void show_cvotes( CHAR_DATA* ch, CABAL_INDEX_DATA* pCabal ){
  CVOTE_DATA* cVote;
  CABAL_INDEX_DATA* pCab = pCabal, *pPar;

  if (pCab == NULL)
    return;

  /* we run through the cabal and its partents looking for the vote */
  while (pCab != NULL){
    for( cVote = pCab->votes; cVote; cVote = cVote->next ){
      if (can_create_cvote( ch, cVote)){
	sendf( ch, "%-20s %-4d %s%-5s [%d days]\n\r",
	       vote_table[cVote->vote].name,
	       cVote->cost,
	       pCabal->currency, (cVote->cost) == 1 ? "" : "s",
	       vote_table[cVote->vote].life_time / VOTE_DAY);
      }
    }
    if ( pCab->parent_vnum
	 && (pPar = get_cabal_index(pCab->parent_vnum)) != NULL)
      pCab = pPar;
    else
      pCab = NULL;
  }
}

/* checks if the player can create a given type of vote, returns type or unkown */
int get_vote_type( CHAR_DATA* ch, const char* name ){
  int type = vote_lookup( name );
  CVOTE_DATA* cVote = get_cvote( ch->pCabal->pIndexData, type );

  if (cVote && can_create_cvote( ch, cVote ))
    return cVote->vote;
  else
    return VOTE_UNKNOWN;
}

/* attaches a few vote data to be filled in and created */
void attach_vote( CHAR_DATA* ch, int type ){
  VOTE_DATA* pv;
  int i = 0;


  if (ch->pcdata->pvote != NULL)
    return;

  pv = new_vote();
  pv->next	=	NULL;
  pv->owner	=	NULL;	//owner stays null untill valid target is assigned
  pv->subject	=	str_dup( "" );
  pv->string	=	str_dup( "" );
  pv->from	=	str_dup( ch->name );
  pv->type	=	type;

  for (i = 0; i < VOTE_VALS; i++){
    pv->value[i] = 0;
  }

  ch->pcdata->pvote = pv;
}
/* checks if this type of vote is allowed to be posted based on other votes that exist */
bool is_duplicated( VOTE_DATA* pVote ){
  VOTE_DATA* pv;

  if (IS_SET(vote_table[pVote->type].flag, VOTE_DUP))
    return FALSE;

  /* run through all the votes and check for duplicates */
  for (pv = vote_list; pv; pv = pv->next ){
    if (pv->type != pVote->type || str_cmp(pv->owner, pVote->owner))
      continue;
    switch ( pv->type ){
    case VOTE_CAB_PROM:
    case VOTE_CAB_DEMO:
    case VOTE_CAB_EXPEL:
    case VOTE_CAB_ELDER:
    case VOTE_CAB_LEADER:
    case VOTE_CAB_APPLY:
      if (pv->value[0] == pVote->value[0])
	return TRUE;
      break;
    case VOTE_LAW:
      if (pv->value[0] == pVote->value[0]
	&& pv->value[1] == pVote->value[1])
	return TRUE;
      break;
    case VOTE_CAB_TAX:
    case VOTE_CAB_SPONS:
      if (pv->value[0] == pVote->value[0])
	return TRUE;
    case VOTE_TITHE:
      if (pv->value[0] == pVote->value[0]
	  && pv->value[1] == pVote->value[1])
	return TRUE;
      break;
    case VOTE_TOME:
      if (pv->value[0] == pVote->value[0]
	  && !strcasecmp(pv->subject, pVote->subject))
	return TRUE;
      break;
    case VOTE_PACT:
      if ( pv->value[0] == pVote->value[0]
	   && pv->value[1] == pVote->value[1]
	   && pv->value[4] == pVote->value[4])
	return TRUE;
      break;
    case VOTE_BUILD:
      if (pv->value[0] == pVote->value[0]
	  && pv->value[1] == pVote->value[1]
	  && pv->value[2] == pVote->value[2]
	  && pv->value[3] == pVote->value[3])
	return TRUE;
      break;
    }
  }
  return FALSE;
}

/* attempts to return a vote type, with given set of values */
VOTE_DATA* get_vote( int type, int v0, int v1, int v2, int v3, int v4 ){
  VOTE_DATA* pv;

  for (pv = vote_list; pv; pv = pv->next){
    if (pv->type == type
	&& pv->value[0] == v0
	&& pv->value[1] == v1
	&& pv->value[2] == v2
	&& pv->value[3] == v3
	&& pv->value[4] == v4)
      return pv;
  }
  return NULL;
}


/* handles rules and setting of the vote target based on vote type */
void set_vote_target( CHAR_DATA* ch, VOTE_DATA* pv, char* target ){
  CMEMBER_DATA* cMem;
  CABAL_DATA* pCab = ch->pCabal;
  CABAL_DATA* pPar = get_parent( pCab );
  AREA_DATA* pArea = NULL;

  char* argument = target;

  if (pPar == NULL){
    send_to_char("You must be a member of a cabal to create votes.\n\r", ch);
    return;
  }
  switch ( pv->type ){
/* votes with VOTE_NOOWNER get their owner set to character's name automaticly, this is just for clarity */
  default:
  case VOTE_GENERAL:
    if (!IS_NULLSTR(pv->owner ))
      free_string( pv->owner );
    pv->owner = str_dup( ch->name );
    send_to_char("Ok.\n\r", ch);
    break;
  case VOTE_CAB_APPLY:
    if (!IS_NULLSTR(pv->owner ))
      free_string( pv->owner );
    pv->owner = str_dup( pPar->name );
    send_to_char("Ok.\n\r", ch);
    break;
  case VOTE_CAB_POLL:
    if (!IS_NULLSTR(pv->owner ))
      free_string( pv->owner );
    pv->owner = str_dup( ch->name );
    send_to_char("Ok.\n\r", ch);
    if (!ch->pCabal){
      send_to_char("You are not in a cabal!\n\r",ch);
      return;
    }
    pv->value[0] = ch->pCabal->vnum;
    break;
  case VOTE_CAB_SPONS:
    pv->value[0] = pPar->vnum;
    if (!IS_NULLSTR(pv->owner ))
      free_string( pv->owner );
    pv->owner = str_dup( pPar->name );
    send_to_char("Ok.\n\r", ch);
    break;
  case VOTE_CAB_DEMO:
  case VOTE_CAB_EXPEL:
  case VOTE_CAB_PROM:
  case VOTE_CAB_ELDER:
  case VOTE_CAB_LEADER:
    if ( (pv->type == VOTE_CAB_PROM || pv->type == VOTE_CAB_ELDER
	  || pv->type == VOTE_CAB_LEADER)
	 && !IS_IMMORTAL(ch)){
      send_to_char("These votes can only be initiated by an Immortal.\n\r", ch);
      return;
    }
    if (IS_NULLSTR( argument )){
      send_to_char("Whom is to be the target of this vote?\n\r", ch);
      return;
    }
    if ( (cMem = get_cmember( target, get_parent(ch->pCabal))) == NULL){
      send_to_char("They are not a member of your organization.\n\r", ch);
      break;
    }
    /* do checks on the member data */
    else{
      CABAL_DATA* pCab = cMem->pCabal;
      CABAL_DATA* pPar = get_parent( pCab );
      int rank = cMem->rank;

/* first we check minimum hours for this rank */
      if (mud_data.mudport != TEST_PORT
	  && pv->type != VOTE_CAB_DEMO
	  && pv->type != VOTE_CAB_EXPEL
	  && !check_promo_hours( cMem, rank )){
	sendf(ch, "%s has not been present enough to be promoted to a new rank.\n\r", cMem->name);
	return;
      }
      else if (pv->type != VOTE_CAB_DEMO
	       && pv->type != VOTE_CAB_EXPEL
	       && !check_promo_req(ch, cMem, rank + 1))
	return;
/* check if there was a recent vote for this */
      else if (mud_data.current_time - cMem->time_stamp < VOTE_DAY){
	send_to_char("There has already been a recent vote regarding their rank.\n\r", ch);
	sendf(ch, "A new vote may be created in %ld hours.\n\r",
	      (VOTE_DAY - (mud_data.current_time - cMem->time_stamp)) / 3600);
	return;
      }
/* check if we can attempt to vote for new leader */
      else if (pv->type == VOTE_CAB_LEADER){
	if (rank != RANK_ELDER){
	  sendf( ch, "%s must hold the rank of %s in order to be promoted to a leader.\n\r",
		 cMem->name,
		 pCab->pIndexData->mranks[RANK_ELDER]);
	  return;
	}
	if (pPar->cur_leader + 1 > pPar->max_leader){
	  send_to_char("The Cabal cannot support another leader.\n\r", ch);
	  return;
	}
      }
      else if (pv->type == VOTE_CAB_ELDER){
	if (rank < RANK_ELDER - 1){
	  sendf( ch, "%s must be at the rank of %s in order to be promoted to %s.\n\r",
		 cMem->name, pCab->pIndexData->mranks[RANK_ELDER - 1], pCab->pIndexData->mranks[RANK_ELDER]);
	  return;
	}
	if (pPar->cur_elder + 1 > pPar->max_elder){
	  send_to_char("The Cabal cannot support another elder.\n\r", ch);
	  return;
	}
      }
      else if (pv->type == VOTE_CAB_PROM){
	if ((rank + 1) >= RANK_ELDER){
	  sendf( ch, "You must use Eldership and Leadership votes to promote %s further.\n\r", cMem->name);
	  return;
	}
      }
      else if (pv->type == VOTE_CAB_DEMO){
	if (rank <= RANK_NEWBIE){
	  sendf( ch, "You cannot demote %s any further.\n\r", cMem->name);
	  return;
	}
	else if (rank < RANK_ELDER && cMem->rank < RANK_ELDER){
	  send_to_char("You may only create demotion votes for Elders and Leaders.\n\r", ch);
	  return;
	}
      }
      else if (pv->type == VOTE_CAB_EXPEL){
	if (rank >= RANK_ELDER){
	  sendf( ch, "You cannot vote to expel an Elder or Leader, they must be demoted first.\n\r");
	  return;
	}
      }
      if (!IS_NULLSTR(pv->owner ))
	free_string( pv->owner );
      pv->owner = str_dup( cMem->name );
      pv->value[0] = cMem->pCabal->vnum;
      pv->value[1] = get_parent(cMem->pCabal)->vnum;
    }
    send_to_char("Ok.\n\r", ch);
    break;
  case VOTE_LAW:
    if (!IS_CABAL(pPar, CABAL_ROYAL)){
      send_to_char("Only royal cabals may use this vote.\n\r", ch);
      return;
    }
    /* check if city is lawful */
    if ( (pArea = pPar->city) == NULL){
      bug("set_vote_target: VOTE_LAW could not find an area for cabal vnum %d.", pv->value[0]);
      break;
    }
    else if (!IS_SET(pArea->area_flags, AREA_LAWFUL)){
      send_to_char("You must first forge a Trade Pact with a Justice cabal.\n\r", ch );
      return;
    }
    else if (IS_NULLSTR(argument)){
      send_to_char("vote target <crime> <new maximum penalty>\n\r(Use \"vote target list\" to see list of crimes)\n\r", ch);
      return;
    }
    else{
      int crime = 0;
      int pen = CRIME_ALLOW;
      char arg[MIL];

      argument = one_argument( argument, arg );
    /* argument is as follows <crime> <new max penalty> */
      if ( (crime = crime_lookup( arg )) < 0){
	int crime = 0;
	send_to_char("Criminal acts fall into following categories:\n\r", ch );
	for (crime = 0; crime_table[crime].name != NULL; crime++){
	  sendf( ch, "%s\n\r", crime_table[crime].name);
	}
	return;
      }
      else if (crime_table[crime].settable == FALSE){
	send_to_char("That crime category has been set by the gods and cannot be changed.\n\r", ch);
	return;
      }
      else if ( IS_NULLSTR(argument) || (pen = flag_lookup2( argument, punish_table)) == NO_FLAG){
	sendf( ch, "Following penalties are valid:\n\r" );
	show_flag_cmds( ch, punish_table );
	return;
      }
      else if (crime == CRIME_MURDER && pen < CRIME_LIGHT){
	send_to_char("You may not allow murder in a lawful city.\n\r", ch);
	return;
      }
      /* we now have the crime and the new penalty, setup the owner, and values */
      if (!IS_NULLSTR(pv->owner ))
	free_string( pv->owner );
      pv->owner = str_dup( pPar->name );
      pv->value[0] = pPar->vnum;
      pv->value[1] = crime;
      pv->value[2] = pen;
      send_to_char("Ok.\n\r", ch);
    }
    break;
  case VOTE_CAB_TAX:
    if (IS_NULLSTR(argument)){
      send_to_char("vote target <new tax rate>\n\r", ch);
      return;
    }
    else{
      int tax = atoi(argument);
      if (tax < 0 || tax > 50){
	send_to_char("The new tax rate must be between 0 and 50%%.\n\r", ch);
	return;
      }

      /* we now have the tax and the new penalty, setup the owner, and values */
      if (!IS_NULLSTR(pv->owner ))
	free_string( pv->owner );
      pv->owner = str_dup( pPar->name );
      pv->value[0] = pPar->vnum;
      pv->value[1] = tax;
      send_to_char("Ok.\n\r", ch);
    }
    break;
  case VOTE_TITHE:
    if (IS_NULLSTR(argument)){
      send_to_char("vote target <cabal> <amount>\n\r", ch);
      return;
    }
    else{
      char cabal[MIL];
      CABAL_DATA* pTar;
      int tithe = 0;

      argument = one_argument( argument, cabal );
      tithe = atoi(argument);

      if ( (pTar = get_cabal( cabal )) == NULL){
	send_to_char("No such cabal exists.\n\r", ch );
	return;
      }
      else if (IS_NULLSTR( argument )){
	send_to_char("vote target <cabal> <amount>\n\r", ch );
	return;
      }
      else if (tithe < 1 || tithe > GET_CAB_CP( pPar )){
	send_to_char("Your cabal cannot afford it.\n\r", ch);
	return;
      }

      /* we now have the tax and the new penalty, setup the owner, and values */
      if (!IS_NULLSTR(pv->owner ))
	free_string( pv->owner );
      pv->owner = str_dup( pTar->name );
      pv->value[0] = pPar->vnum;
      pv->value[1] = pTar->vnum;
      pv->value[2] = tithe;
      CP_CAB_GAIN( pPar, -tithe );
      CHANGE_CABAL( pPar );
      save_cabals( TRUE, NULL );
      send_to_char("Ok.\n\r", ch);
    }
    break;
  case VOTE_PACT:
    if (IS_NULLSTR(argument)){
      send_to_char("vote target <pact> <cabal>\n\r(Use \"vote target list\" to see list of pacts)\n\r", ch);
      return;
    }
    else{
      int pact = 0;
      CABAL_DATA* pBen = NULL;
      PACT_DATA* pPact = NULL;
      char arg[MIL];

      argument = one_argument( argument, arg );

    /* argument is as follows <pact> <cabal> */
      if ( (pact = flag_lookup2( arg, pact_flags)) == NO_FLAG){
	sendf( ch, "Following pacts may be declared:\n\r" );
	show_flag_cmds( ch, pact_flags );
	return;
      }
      else if (IS_NULLSTR( argument ) || (pBen = get_cabal( argument )) == NULL){
	send_to_char("You may not form a pact with that cabal.\n\r", ch );
	return;
      }
      else
	pBen = get_parent( pBen );

      if (is_same_cabal(pPar, pBen)){
	send_to_char("A pact with yourself? How useless.\n\r", ch );
	return;
      }
      /* we have enoough data to check if such pact exists */
      /* vendetta pact blocks all other pacts except for PEACE pact */
      if ( get_pact(pPar, pBen, PACT_VENDETTA, FALSE) != NULL && pact != PACT_PEACE){
	send_to_char("You may only create a Peace Pact while a Vendetta exists.\n\r", ch);
	return;
      }
      else if (pact == PACT_PEACE && pPar->enemy && pPar->enemy == pBen){
	send_to_char("You cannot make peace with your mortal enemies!\n\r", ch);
	return;
      }
      else if ( pact == PACT_PEACE
		&& ( (pPact = get_pact(pPar, pBen, PACT_VENDETTA, FALSE)) == NULL
		     || pPact->complete != PACT_COMPLETE) ){
	send_to_char("You may only create a Peace Pact while a Vendetta exists.\n\r", ch );
	return;
      }
      else if ( pact == PACT_BREAK
		&& ( (pPact = get_pact(pPar, pBen, PACT_TRADE, TRUE)) == NULL
		     || pPact->complete != PACT_COMPLETE) ){
	send_to_char("There is no trade between you to stop!\n\r", ch );
	return;
      }
      else if ( pact == PACT_IMPROVE
		&& ( (pPact = get_pact(pPar, pBen, PACT_TRADE, TRUE)) == NULL
		     || pPact->complete != PACT_COMPLETE) ){
	send_to_char("There is no trade between you to improve!\n\r", ch );
	return;
      }
      else if (pact == PACT_IMPROVE
	       && pPact->type == PACT_NAPACT
	       && IS_CABAL(pPar, CABAL_ALLIANCE)){
	send_to_char("You may only be allied with one cabal at a time.\n\r", ch);
	return;
      }
      else if (pact == PACT_IMPROVE && (pPact->Adv > pPact->type || pPact->type == PACT_ALLIANCE)){
	send_to_char("Your trade has already been approved for improvement.\n\r", ch);
	return;
      }
/* cannot delcare vendetta if there is advanced trade */
      else if ( pact == PACT_VENDETTA && get_pact(pPar, pBen, PACT_NAPACT, FALSE) != NULL){
	send_to_char("The Non-Aggression pact must first be cancelled with the Embargo Pact.\n\r", ch);
	return;
      }
      else if ( pact == PACT_VENDETTA && get_pact(pPar, pBen, PACT_ALLIANCE, FALSE) != NULL){
	send_to_char("The Alliance must first be cancelled with the Embargo Pact.\n\r", ch);
	return;
      }
      else if ( get_pact(pPar, pBen, pact, (pact == PACT_TRADE || pact == PACT_NAPACT || pact == PACT_ALLIANCE)) != NULL){
	send_to_char("That type of pact already exists between you.\n\r", ch);
	return;
      }
      /* we now have the pact and the cabal, setup the owner, and values */
      if (!IS_NULLSTR(pv->owner ))
	free_string( pv->owner );
      pv->owner = str_dup( pPar->name );
      pv->value[0] = pPar->vnum;
      pv->value[1] = pBen->vnum;
      pv->value[4] = pact;
      switch (pact){
      default:
      case PACT_TRADE:
	pv->value[2] = TRADE_CP_START;
	pv->value[3] = TRADE_SUP_START;
	break;
      case PACT_NAPACT:
	pv->value[2] = TRADE_CP_NA;
	pv->value[3] = TRADE_SUP_NA;
	break;
      case PACT_ALLIANCE:
	pv->value[2] = TRADE_CP_MAX;
	pv->value[3] = TRADE_SUP_MAX;
	break;
      case PACT_VENDETTA:
	pv->value[2] = TRADE_CP_WAR;
	pv->value[3] = TRADE_SUP_WAR;
	break;
      case PACT_IMPROVE:
	if (pPact == NULL ){
	  send_to_char("Error, no pact found.\n\r", ch );
	  return;
	}
	else if (pPact->type == PACT_TRADE ){
	  pv->value[2] = PACT_NAPACT;
	}
	else if (pPact->type == PACT_NAPACT ){
	  pv->value[2] = PACT_ALLIANCE;
	}
	else{
	  send_to_char("You cannot improve the trade any further.\n\r", ch);
	  return;
	}
	break;
      }
      send_to_char("Ok.\n\r", ch);
    }
    break;
  }
}









/* Command used to create/setup votes, view list of votes, and vote	*
 * vote list		shows list of votes (M/IMM)			*
 * vote vote		enters the vote booth (M/IMM)			*
 * vote <vote type>	creates a given vote type			*
 */

void do_vote( CHAR_DATA* ch, char* argument ){
  char arg[MIL];
  argument = one_argument(argument, arg);

  if (IS_NPC(ch))
    return;

  if (IS_NULLSTR(arg)){
    if (has_votes (ch)){
      ch->desc->connected = CON_VOTE;
      interpret_vote( ch, "");
      return;
    }
    else{
      send_to_char("You have no pending votes at this time.\n\r", ch);
      return;
    }
  }
  /* IMMORTAL COMMANDS HERE */
  else if (IS_IMMORTAL( ch ) && get_trust( ch ) >= CREATOR ){
    if (!str_prefix(arg, "delete")){
      int index = atoi( argument );
      int counter = 0;
      VOTE_DATA* pv = vote_list;

      if (index < 1){
	send_to_char("Specify numeric index\n\r", ch);
	return;
      }
      /* get the right vote */
      for (;pv != NULL; pv = pv->next){
	if (!is_vote_to( ch, pv)){
	  continue;
	}
	else
	  counter++;

	if (counter == index )
	  break;
      }
      if (pv == NULL){
	send_to_char("No vote with that index.\n\r", ch);
	return;
      }

/* we now nuke the vote */
      sendf( ch, "Vote \"%s\" deleted.\n\r", pv->subject);
      rem_vote( pv );
      free_vote( pv );
      save_mud();
      return;
    }
  }
  if (!str_prefix(arg, "list")){
    list_votes(ch);
    return;
  }
  if (ch->pCabal == NULL){
    send_to_char("You're not in a cabal.\n\r", ch);
    return;
  }
/* CREATE */
  else if (!str_prefix(arg, "create")){
    int type = 0;
    if (ch->pcdata->pvote != NULL){
      send_to_char("You are already working on another vote.\n\r", ch);
      return;
    }
/* get the type of vote to create */
    if (IS_NULLSTR( argument )
	|| (type = get_vote_type( ch, argument)) == VOTE_UNKNOWN){
      send_to_char("Following votes are avaliable to you:\n\r", ch );
      show_cvotes( ch, ch->pCabal->pIndexData );
      return;
    }
    attach_vote( ch, type );
    /* auto owner set to name of ch */
    if (ch->pcdata->pvote && IS_SET(vote_table[type].flag, VOTE_NOOWNER))
      set_vote_target( ch, ch->pcdata->pvote, ch->name );
    else
      send_to_char("Ok.\n\r", ch);
  }
/* CLEAR */
  else if (!str_prefix(arg, "clear")){
    if (ch->pcdata->pvote == NULL){
      send_to_char("You are not working on a vote.\n\r", ch);
      return;
    }
    free_vote( ch->pcdata->pvote );
    ch->pcdata->pvote = NULL;
    send_to_char("Ok.\n\r", ch);
  }
/* SHOW */
  else if (!str_prefix(arg, "show")){
    VOTE_DATA* pv = ch->pcdata->pvote;
    if (pv == NULL){
      send_to_char("You are not working on a vote.\n\r", ch);
      return;
    }
    sendf( ch, "%s: %-15s [Tar: %s]\n\r\n\r", pv->from, pv->subject, pv->owner );
    send_to_char( pv->string, ch );
    send_to_char( "\n\r", ch );
  }
/* FORMAT */
  else if (!str_prefix(arg, "format")){
    VOTE_DATA* pv = ch->pcdata->pvote;
    if (pv == NULL){
      send_to_char("You are not working on a vote.\n\r", ch);
      return;
    }
    ch->pcdata->pvote->string = format_string(ch->pcdata->pvote->string);
    send_to_char( "Text formatted\n\r", ch );
  }
/* SUBJ */
  else if (!str_prefix(arg, "subject")){
    VOTE_DATA* pv = ch->pcdata->pvote;
    if (pv == NULL){
      send_to_char("You are not working on a vote.\n\r", ch);
      return;
    }
    if (!IS_SET(vote_table[pv->type].flag, VOTE_SUBJ)){
      send_to_char("The subject will be created when you post the vote.\n\r", ch);
      return;
    }
    if (IS_NULLSTR( argument )){
      send_to_char("Enter what subject?\n\r", ch);
      return;
    }
    if (pv->subject)
      free_string( pv->subject );
    pv->subject = str_dup( argument );
    send_to_char("Ok.\n\r", ch);
  }
/* TARGET */
  else if (!str_prefix(arg, "target")){
    VOTE_DATA* pv = ch->pcdata->pvote;

    if (pv == NULL){
      send_to_char("You are not working on a vote.\n\r", ch);
      return;
    }
    if (IS_SET(vote_table[pv->type].flag, VOTE_NOOWNER)){
      send_to_char("Target of this vote is generated when you post the vote.\n\r", ch);
      return;
    }
    set_vote_target( ch, pv, argument );
  }
/* EDIT TEXT */
  else if (!str_prefix(arg, "edit")){
    VOTE_DATA* pv = ch->pcdata->pvote;

    if (pv == NULL){
      send_to_char("You are not working on a vote.\n\r", ch);
      return;
    }
    string_append( ch, &pv->string );
    return;
  }
/* SEND/POST */
  else if (!str_prefix(arg, "send") || !str_cmp(arg, "post")){
    VOTE_DATA* pv = ch->pcdata->pvote;
    CABAL_DATA* pc = get_parent( ch->pCabal );
    CVOTE_DATA* cVote = NULL;
    bool fUrge = !IS_NULLSTR(argument) && !str_prefix( argument, "urgent" );
    int cost = 0;

    if (pv == NULL){
      send_to_char("You are not working on a vote.\n\r", ch);
      return;
    }
    /* check over various things */
    else if (pv->type == VOTE_UNKNOWN){
      send_to_char("Unknown type.  Aborting..\n\r", ch);
      return;
    }

    else if (IS_NULLSTR(pv->from )){
      send_to_char("Unknown author. Aborting..\n\r", ch);
      return;
    }

    else if (IS_NULLSTR(pv->owner )){
      send_to_char("You must first specify the target of this vote (person, cabal etc).\n\r", ch);
      return;
    }

    else if (IS_SET(vote_table[pv->type].flag, VOTE_SUBJ) && IS_NULLSTR(pv->subject )){
      send_to_char("You need to provide a subject.\n\r",ch);
      return;
    }
    else if ( (cVote = get_cvote(ch->pCabal->pIndexData, pv->type )) == NULL){
      send_to_char("You are not allowed to create this vote.\n\r", ch);
      return;
    }
    else if (is_duplicated( pv )){
      send_to_char("A vote on this matter has already been created.\n\r", ch);
      return;
    }
    /* check cost */
    else if ( !IS_IMMORTAL(ch) && (cost = fUrge ? 2 * cVote->cost : cVote->cost) > GET_CP( ch)){
      sendf( ch, "You will need at least %d %s%s to send this vote%s.\n\r",
	     cost,
	     ch->pCabal->currency, (cost) == 1 ? "" : "s",
	     fUrge ? " urgently" : "");
      return;
    }
    /* cabal coffers are not affected by urgency */
    else if (!IS_IMMORTAL(ch)
	     && !IS_SET(vote_table[pv->type].flag, VOTE_NOCAB_COST)
	     && (pc == NULL || cVote->cost > GET_CAB_CP( pc )) ){
      sendf( ch, "[%s] will require at least %d %s%s in its coffers to send this vote.\n\r",
	     pc->who_name,
	     cVote->cost,
	     pc->currency, (cVote->cost) == 1 ? "" : "s" );
      return;
    }
    /* seems everything is ok now, try to create/post the vote */
    else{
      VOTE_DATA* pVote = create_vote( ch, pv->owner, pv->subject, pv->string, pv->type,
				      pv->value[0], pv->value[1], pv->value[2], pv->value[3], pv->value[4]);
      if (pVote == NULL){
	bug("do_vote: create: error creating vote type %d.", pv->type);
	send_to_char("Error!\n\r", ch);
	return;
      }
      /* check urgent flag */
      if (fUrge)
	pVote->life_time -= vote_table[pVote->type].life_time / 3;
      add_vote( pVote );

      /* free the vote we were working on */
      free_vote( pv );
      ch->pcdata->pvote = NULL;
      CP_GAIN( ch, fUrge ? -2 * cost : -cost, TRUE );
      if (get_trust(ch) != IMPLEMENTOR)
	CPS_CAB_GAIN( pc, -(cost * CPTS));
      send_to_char("Ok.\n\r", ch);
      save_mud();
      return;
    }
  }
  else{
    send_to_char("read \"help cabal votes\" for commands and specifics.\n\r", ch);
  }
}


/* The following functions handle vote creation for based on an application */
/* the return values control if the app/note is posted, FALSE = yes */
bool create_app_vote( CHAR_DATA* ch, char* to, char* subject, char* text ){
  CABAL_DATA* pc = NULL;
  VOTE_DATA* pv;
  char cabal[MIL];

  one_argument(to, cabal);

  /* Viri: changed this to not require the subject line, check if this is a cabal app
  if (is_auto_name("cabal", subject) && is_auto_name("app", subject)){

  if ( (pc = get_cabal( cabal )) == NULL){
      sendf(ch, "There seems to be no organization named %s.\n\r", cabal);
      return VOTE_ERROR;
    }
  */
  if ( (pc = get_cabal( cabal )) != NULL){
/* check if we already belong to a cabal */
    if (ch->pCabal){
      send_to_char("You've already been inducted to a cabal!\n\r", ch );
      return VOTE_ERROR;
    }
/* check if this is a child */
    if (pc->parent){
      sendf(ch, "You must direct your application to %s.\n\r", pc->parent->name);
      return VOTE_ERROR;
    }
    if (ch->pcdata->last_cabal && ch->pcdata->last_cabal != pc){
      send_to_char("You are allowed only to try for a single cabal in your life.\n\r", ch);
      return VOTE_ERROR;
    }
    if (ch->pcdata->last_app && ch->pcdata->last_app + VOTE_TIME_TO_REAPPLY > mud_data.current_time){
      sendf(ch, "You will be allowed another chance to apply in %ld days.\n\r",
	    (ch->pcdata->last_app + VOTE_TIME_TO_REAPPLY - mud_data.current_time) / VOTE_DAY);
      return VOTE_ERROR;
    }
    else if (pc->clan && GET_CLAN(ch) != pc->clan){
      sendf( ch, "Only members of %s clan may apply.\n\r", GetClanName(pc->clan));
      return VOTE_ERROR;
    }
    else if (!ClanApplicationCheck(ch, pc))
      return VOTE_ERROR;


    /* we have the cabal he was applying to now, check if we can take on a new member */
    if ( !check_cabal_req( ch, pc ) )
      return VOTE_ERROR;

    /* create the app */
    if ( (pv = create_vote(ch, ch->name, cabal, text, VOTE_CAB_APPLY, pc->vnum, 0, 0, 0, 0)) == NULL){
      send_to_char("Error creating vote.\n\r", ch);
      return VOTE_ERROR;
    }
/* check for duplicated votes */
    if ( is_duplicated( pv )){
      send_to_char("You already have a vote pending.\n\r", ch );
      free_vote( pv );
      return VOTE_ERROR;
    }
    /* set  character cabal choice and waiting time for re-apply */
    ch->pcdata->last_app = mud_data.current_time;
    ch->pcdata->last_cabal = pc;
    save_char_obj( ch );
    add_vote( pv );
    send_to_char("Your application has been accepted.\n\r", ch);
    return VOTE_ACCEPTED;
  }
  else
    return VOTE_NONE;
}


/* checks if the particular vote is direcetd to a character */
bool is_vote_to(CHAR_DATA* ch, VOTE_DATA* pv){
  CABAL_DATA* pCab = NULL;

  if (ch == NULL || IS_NPC(ch) || pv== NULL)
    return FALSE;

  switch ( pv->type ){
  case VOTE_GENERAL:
    return TRUE;
    break;
  case VOTE_CAB_APPLY:
    /* citizens can vote on this */
    pCab = get_cabal_vnum(pv->value[0]);
    if (pCab && !IS_IMMORTAL(ch) && !str_cmp(hometown_table[ch->hometown].name, pCab->name))
      return TRUE;
  case VOTE_BUILD:
  case VOTE_CAB_POLL:
  case VOTE_CAB_TAX:
  case VOTE_TITHE:
  case VOTE_TOME:
    if (str_cmp(ch->name, pv->owner) && is_same_cabal( ch->pCabal, get_cabal_vnum(pv->value[0])))
      return TRUE;
    else
      return FALSE;
  case VOTE_PACT:
  case VOTE_CAB_SPONS:
    pCab = get_cabal_vnum(pv->value[0]);
    if (is_same_cabal( ch->pCabal, get_cabal_vnum(pv->value[0]))
	|| (pCab && !IS_IMMORTAL(ch) && !str_cmp(hometown_table[ch->hometown].name, pCab->name))){
      return TRUE;
    }
    else
      return FALSE;
    break;
  case VOTE_CAB_PROM:
  case VOTE_CAB_DEMO:
  case VOTE_CAB_ELDER:
  case VOTE_CAB_LEADER:
  case VOTE_CAB_EXPEL:
    if (str_cmp(ch->name, pv->owner) && is_same_cabal( ch->pCabal, get_cabal_vnum(pv->value[0])))
      return TRUE;
    else
      return FALSE;
    break;
  case VOTE_LAW:
    pCab = get_cabal_vnum(pv->value[0]);
    if ( (!IS_IMMORTAL(ch) && !str_cmp(hometown_table[ch->hometown].name, pCab->name))
	 || is_same_cabal( ch->pCabal, pCab))
      return TRUE;
    else
      return FALSE;
    break;
  default:
    return FALSE;
  }
  return FALSE;
}

/* checks if the particular vote should be shown to a character or not */
/* used when voting to select next vote to vote on */
bool hide_vote( CHAR_DATA* ch, VOTE_DATA* pv ){

  if (pv->time_stamp <= ch->pcdata->last_vote)
    return TRUE;
  if (!str_cmp(ch->name, pv->owner))
    return TRUE;
  if (!is_vote_to(ch, pv))
    return TRUE;

  return FALSE;
}

/* checks if a person should vote, and throws them into the vote interp */
int has_votes( CHAR_DATA* ch ){
  VOTE_DATA* pv = vote_list;
  int count = 0;

  if (ch->level < 20)
    return count;

  for (;pv != NULL; pv = pv->next){
    if (!hide_vote(ch, pv))
      count ++;
  }
  if (count){
    sendf(ch, "You have %d votes awaiting your decision.  Entering voting booth...\n\r", count);
  }
  return count;
}

VOTE_DATA* next_vote( CHAR_DATA* ch ){
  VOTE_DATA* pv = vote_list;

  while (pv && hide_vote(ch, pv)){
    pv = pv->next;
  }
  return pv;
}

/* returns the weight of a vote based on the character and the vote */
int get_voteweight( CHAR_DATA* ch, VOTE_DATA* pv ){
  int val = 1;
  switch (pv->type ){
  default:
  case VOTE_GENERAL:
    val = 1;
    break;
  case VOTE_CAB_APPLY:
  case VOTE_CAB_PROM:
  case VOTE_CAB_DEMO:
  case VOTE_CAB_EXPEL:
  case VOTE_CAB_ELDER:
  case VOTE_CAB_LEADER:
  case VOTE_CAB_POLL:
  case VOTE_CAB_TAX:
  case VOTE_CAB_SPONS:
  case VOTE_BUILD:
  case VOTE_TITHE:
  case VOTE_TOME:
  case VOTE_PACT:
  case VOTE_LAW:
    if (IS_ELDER(ch))
      val  = VOTE_WEIGHT_ELDER;
    else if (IS_LEADER(ch)){
      if (IS_IMMORTAL(ch))
	val  = 2 * VOTE_WEIGHT_LEADER;
      else
	val  = VOTE_WEIGHT_LEADER;
    }
    else
      val = VOTE_WEIGHT_NORM;
    if (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_ROYAL))
      val *= 10;
    break;
  }
  return val;
}

/* checks if the character is a leader of a given cabal vote */
bool can_veto( CHAR_DATA* ch, VOTE_DATA* pv ){
  CABAL_DATA *pSec;

  switch (pv->type ){
  default:
  case VOTE_GENERAL:
    return FALSE;
    break;
  case VOTE_CAB_APPLY:
/* royal cabal imms can always veto on apps */
    if (IS_IMMORTAL(ch) && ch->pCabal && IS_CABAL(ch->pCabal, CABAL_ROYAL)
	&& !str_cmp(ch->name, (get_parent(ch->pCabal))->immortal))
      return TRUE;
  case VOTE_CAB_PROM:
    if (ch->pcdata->member && !IS_NULLSTR(ch->pcdata->member->sponsored)
	&& !str_cmp(ch->pcdata->member->sponsored, pv->owner))
      return TRUE;
  case VOTE_CAB_ELDER:
  case VOTE_CAB_LEADER:
  case VOTE_CAB_TAX:
  case VOTE_CAB_SPONS:
  case VOTE_BUILD:
  case VOTE_TITHE:
  case VOTE_TOME:
  case VOTE_LAW:
  case VOTE_PACT:
    pSec = get_cabal_vnum(pv->value[0]);
    if (ch->pCabal
	&& pSec
	&& IS_LEADER(ch)
	&& is_same_cabal( ch->pCabal, pSec)){
      return TRUE;
    }
    else{
      return FALSE;
    }
  case VOTE_CAB_DEMO:
  case VOTE_CAB_EXPEL:
    if (ch->pCabal && IS_LEADER(ch)
	&& is_same_cabal( ch->pCabal, get_cabal_vnum(pv->value[0])) )
      return TRUE;
    else
      return FALSE;
  }
  return FALSE;
}

/* vote interpreter, handles all the commands in the vote booth */
void interpret_vote(CHAR_DATA* ch, char* argument ){
  char* strtime;
  VOTE_DATA* pv = next_vote( ch );
  while ( isspace(*argument) ) {
    argument++;
  }

  if ( pv == NULL ){
    send_to_char("You have no remaining votes, returning to normal mode.\n\r", ch);
    if (ch->desc->connected == CON_VOTE)
      ch->desc->connected = CON_PLAYING;
    else{
      connect_char( ch->desc );
      do_save( ch, "");
    }
    return;
  }
  if (IS_NULLSTR( argument ) ){
    strtime = ctime( &pv->time_stamp );
    sendf( ch, "%s: %s\n\r%s\n\r", pv->from, pv->subject, strtime );
    send_to_char( pv->string, ch );
    send_to_char( "\n\r", ch );
  }
  else if (!str_cmp("yes", argument) ){
    pv->yes += get_voteweight(ch, pv );
    ch->pcdata->vote_skip = 0;
    ch->pcdata->last_vote = pv->time_stamp;
    check_vote( pv );
    interpret_vote(ch, "" );
    return;
  }
  else if (!str_cmp("no", argument) ){
    pv->no += get_voteweight(ch, pv );
    ch->pcdata->vote_skip = 0;
    ch->pcdata->last_vote = pv->time_stamp;
    check_vote ( pv );
    interpret_vote(ch, "" );
    return;
  }
  else if (!str_cmp("pass", argument) ){
    pv->pass += get_voteweight(ch, pv );
    ch->pcdata->vote_skip = 0;
    ch->pcdata->last_vote = pv->time_stamp;
    check_vote( pv );
    interpret_vote(ch, "" );
    return;
  }
  else if (!str_cmp("force", argument) ){
    if (get_trust(ch) < IMPLEMENTOR){
      send_to_char("You cannot force votes.\n\r",ch);
      return;
    }
    pv->yes = 666;
    ch->pcdata->vote_skip = 0;
    ch->pcdata->last_vote = pv->time_stamp;
    check_vote( pv );
    interpret_vote(ch, "" );
    return;
  }
  else if (!str_cmp("quit", argument) ){
    if (ch->pcdata->vote_skip >= VOTE_MAXSKIP){
      send_to_char("You must vote at least once before quitting.\n\r", ch);
    }
    else{
      if (!IS_IMMORTAL(ch))
	ch->pcdata->vote_skip++;
      if (ch->desc->connected == CON_VOTE){
	ch->desc->connected = CON_PLAYING;
      }
      else{
	connect_char( ch->desc );
	do_save( ch, "");
      }
      return;
    }
  }
  else if (!str_cmp(argument, "help")){
    do_help(ch, "vote");
  }
  else if (!str_cmp(argument, "veto")){
    if (!can_veto(ch, pv))
      return;
    pv->veto = TRUE;
    ch->pcdata->last_vote = pv->time_stamp;
    check_vote( pv );
    interpret_vote(ch, "" );
    return;
  }
/* command prompt */
  send_to_char("You may \"quit\" from this vote and come back to it at later time.\n\r", ch );
  sendf(ch, "Cmds: quit, help, yes, no, pass%s>  ", can_veto(ch, pv) ? ", veto" : "");


}

/* the following functions are used to decide if the vote passed or failed	*/


/* Decides if a single vote has passed or failed according to following:	*
 *
 * 1) yes == no  mud selects randomly.
 * 2) yes > no vote passes
 * 3) yes < no vote fails
 * 4) if Vetoed the vote always fails
 *
 * Returns TRUE for pass, FALSE for fail
 *										*/

bool judge_vote( VOTE_DATA* pv ){

  if (pv == NULL){
    bug("judge_vote: null vote passed", 0);
    return FALSE;
  }

  if (pv->veto)
    return FALSE;
  else if (pv->yes < pv->no )
    return FALSE;
  else if (pv->yes > pv->no )
    return TRUE;
  else if ( (pv->yes ||  pv->no || pv->pass) && number_range(1, 1000) <= 500)
    return TRUE;
  else
    return FALSE;
}

/* based on the value of the fPassed executes the effects of the vote based on type and its data */
void execute_vote( VOTE_DATA* pv, bool fPass ){
  CHAR_DATA* ch;
  CMEMBER_DATA* cMem;
  CABAL_DATA* pc = NULL;
  CABAL_DATA* pc2 = NULL;
  DESCRIPTOR_DATA* d;
  char buf[MSL];

  bool fPurge = FALSE;

/* standard check */
  if (pv == NULL)
    return;

/* meat and potatoes of the function, does work based on the type of vote and if it failed. */
  switch (pv->type ){
  default:
  case VOTE_GENERAL:
  case VOTE_CAB_POLL:
    sprintf( buf, "The vote %s created by %s has %s.\n\rResults:\n\r"\
	     "For: %d Against: %d Pass: %d\n\r",
	     pv->subject, pv->from,
	     pv->veto ? "been VETOED" : !fPass ? "FAILED" : "PASSED",
	     pv->yes, pv->no, pv->pass);
    do_hal( pv->owner, pv->subject, buf, NOTE_NEWS );
    break;
  case VOTE_CAB_APPLY:
    if ( (pc = get_cabal_vnum( pv->value[0] )) == NULL){
      bug("execute_vote: CAB_APPLY could not find cabal index %d.", pv->value[0]);
      break;
    }
    sprintf( buf, "A vote has been completed: %s's %s has been %s.\n\rResults:\n\r"\
	     "%d For, %d Against, %d Pass\n\r",
	     pv->owner, pv->subject,
	     pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	     pv->yes, pv->no, pv->pass);
    do_hal( pc->name, pv->subject, buf, NOTE_NEWS );
    sprintf( buf, "The %s of %s have %s your %s.\n\rResults:\n\r"\
	     "%d For, %d Against, %d Pass\n\r%s\n\r",
	     pv->veto ? "leader" : "members",
	     pc->name,
	     pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	     pv->subject,
	     pv->yes, pv->no, pv->pass,
	     !fPass ? "You may re-apply in fourteen days." : "Seek further guidance amongst your new faction." );
    do_hal( pv->owner, pv->subject, buf, NOTE_NEWS );
    if (!fPass){
      CMEMBER_DATA* cm;
      /* clear sponsor if any */
      if (  (cm = sponsor_check(pc, pv->owner )) == NULL)
	break;
      sprintf( buf, "Your obligations as a sponsor of %s have ended.\n\r", pv->owner);
      do_hal( cm->name, "End of Sponsorship.", buf, NOTE_NEWS);
      free_string(cm->sponsored);
      cm->sponsored = str_dup( "" );
      CHANGE_CABAL( pc );
      save_cabals( TRUE, NULL );
      break;
    }
    /* check if player is in the lands */
    if ( (ch = get_char( pv->owner )) == NULL){
      /* check if we can bring the player on */
      if ( (d = bring(pc->anchor ? pc->anchor :  get_room_index( ROOM_VNUM_LIMBO ), pv->owner)) == NULL){
	bug("execute_vote: Could not find the applicant pfile.", 0);
	break;
      }
      else{
	ch = d->character;
	/* we set the messages for login since he was not online */
	SET_BIT(ch->pcdata->messages, !fPass ? MSG_CAB_REJECT : MSG_CAB_ACCEPT);
	/* if the cabal had an achnor room we make sure that is the room person will log into */
	fPurge = TRUE;
      }
    }

    /* approved, we check if member of cabal already, if not we make it so */
    if (ch->pCabal == NULL){
      CMEMBER_DATA* cm;

      char_to_cabal( ch, pc );	//saves cabals
      /* make sure the person has enough hours for this month */
      ch->pcdata->member->mhours = 36000;
      update_cabal_skills(ch, ch->pCabal, FALSE, TRUE);

      /* check for sponsorship */
      if (  (cm = sponsor_check(pc, pv->owner )) != NULL){
	if (!IS_NULLSTR(ch->pcdata->member->sponsor))
	  free_string(ch->pcdata->member->sponsor);
	ch->pcdata->member->sponsor = strdup( cm->name );
	/* increase hours and promote once due to sponsorship */
	ch->pcdata->member->hours = 36000;
	promote(ch, pc, 1 );
      }
      CHANGE_CABAL( pc );
      save_cabals( TRUE, NULL );
      save_char_obj( ch );
    }

    /* char is present */
    sprintf( buf, "By your decision %s has been allowed into the ranks of %s.\n\r", PERS2( ch ), pc->who_name );
    cabal_echo( ch->pCabal, buf );

    /* check if we need to purge the character */
    if (fPurge){
      purge( ch );
    }
    break;
  case VOTE_CAB_PROM:
  case VOTE_CAB_DEMO:
  case VOTE_CAB_ELDER:
  case VOTE_CAB_LEADER:
  case VOTE_CAB_EXPEL:
    if ( (pc = get_cabal_vnum( pv->value[0] )) == NULL){
      bug("execute_vote: CAB_PROMOTIONS could not find cabal index %d.", pv->value[0]);
      break;
    }
    else
      pc = get_parent( pc );
    /* get their cmember data, and update timestamp if possible */
    if ( (cMem = get_cmember( pv->owner, pc  )) == NULL){
      bug("execute_vote: CAB_PROMOTIONS could not find cmember data.",0);
      break;
    }
    else{
      /* set application wait time for demotions (success) and promotions (failure)*/
      if (fPass && (pv->type == VOTE_CAB_EXPEL || pv->type == VOTE_CAB_DEMO))
	cMem->time_stamp = mud_data.current_time;
      else if (!fPass && (pv->type == VOTE_CAB_PROM || pv->type == VOTE_CAB_ELDER || pv->type == VOTE_CAB_LEADER))
	cMem->time_stamp = mud_data.current_time;
    }
    sprintf( buf, "A vote has been completed: %s's %s has been %s.\n\rResults:\n\r"\
	     "%d For, %d Against, %d Pass\n\r",
	     pv->owner, pv->subject,
	     pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	     pv->yes, pv->no, pv->pass);
    do_hal( pc->name, pv->subject, buf, NOTE_NEWS );
    if (pv->type == VOTE_CAB_EXPEL )
      sprintf( buf, "The %s of %s have %s your explulsion from %s.\n\rResults:\n\r"\
	       "%d For, %d Against, %d Pass\n\r",
	       pv->veto ? "leader" : "members",
	       pc->name,
	       pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	       pc->who_name,
	       pv->yes, pv->no, pv->pass);
    else
      sprintf( buf, "The %s of %s have %s your %s.\n\rResults:\n\r"\
	       "%d For, %d Against, %d Pass\n\r%s\n\r",
	       pv->veto ? "leader" : "members",
	       pc->name,
	       pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	       pv->subject,
	       pv->yes, pv->no, pv->pass,
	       !fPass ? "" : "Seek further guidance amongst your new faction." );
    do_hal( pv->owner, pv->subject, buf, NOTE_NEWS );
    if (!fPass){
      break;
    }
    /* check if player is in the lands */
    if ( (ch = get_char( pv->owner )) == NULL){
      /* check if we can bring the player on */
      if ( (d = bring(get_room_index( ROOM_VNUM_LIMBO ), pv->owner)) == NULL){
	bug("execute_vote: Could not find the applicant pfile.", 0);
	break;
      }
      else{
	ch = d->character;
	/* we set the messages for login since he was not online */
	SET_BIT(ch->pcdata->messages, pv->type == VOTE_CAB_DEMO ? MSG_CAB_DEMO : pv->type == VOTE_CAB_EXPEL ? MSG_CAB_EXPEL : MSG_CAB_PROM);
	fPurge = TRUE;
      }
    }
    if (ch->pCabal == NULL){
      bug("execute_vote: CABAL_PROMOTIONS: character was not member of a cabal.\n\r", 0);
    }
    else{
      CMEMBER_DATA* cm;

      /* if expel, we kick them out */
      if (pv->type == VOTE_CAB_EXPEL){
	char buf[MIL];
	sprintf( buf, "By decision of majority %s has been removed from ranks of %s.", ch->name, ch->pCabal->who_name);
	cabal_echo( ch->pCabal, buf );

	/* clear sponsor if any */
	if (  (cm = sponsor_check(pc, pv->owner )) != NULL){
	  sprintf( buf, "Your obligations as a sponsor of %s have ended.\n\r", pv->owner);
	  do_hal( cm->name, "End of Sponsorship.", buf, NOTE_NEWS);
	  free_string(cm->sponsored);
	  cm->sponsored = str_dup( "" );
	  CHANGE_CABAL( pc );
	  save_cabals( TRUE, NULL );
	}

	/* remove the person */
	ch->pcdata->last_app = mud_data.current_time + VOTE_DAY * 7;
	update_cabal_skills(ch, ch->pCabal, TRUE, TRUE);
	char_from_cabal( ch );
	save_char_obj( ch );
	CHANGE_CABAL( pc );
	save_cabals( TRUE, NULL );
      }
      else{
	int old_rank = ch->pcdata->rank;
      /* approved we promote or demote them accordingly */
	int rank = promote(ch, ch->pCabal, pv->type == VOTE_CAB_DEMO ? -1 : 1); //saves cabals

	/* clear sponsor if any */
	if (  old_rank < rank && rank >= 2){
	  if ( (cm = sponsor_check(pc, pv->owner )) != NULL){
	    char buf[MIL];
	    sprintf( buf, "Your obligations as a sponsor of %s have ended.\n\r", pv->owner);
	    do_hal( cm->name, "End of Sponsorship.", buf, NOTE_NEWS);
	    free_string(cm->sponsored);
	    cm->sponsored = str_dup( "" );
	  }
	  if (!IS_NULLSTR(ch->pcdata->member->sponsor )){
	    free_string( ch->pcdata->member->sponsor );
	    ch->pcdata->member->sponsor = str_dup( "" );
	  }
	  CHANGE_CABAL( pc );
	  save_cabals( TRUE, NULL );
	}
	save_char_obj( ch );
	sendf( ch, "You have been %s to the rank of %s!\n\r",
	       pv->type == VOTE_CAB_DEMO ? "demoted" : "promoted",
	       ch->pcdata->true_sex == SEX_FEMALE ? ch->pCabal->pIndexData->franks[rank] : ch->pCabal->pIndexData->mranks[rank] );
      }
      if (!fPurge){
	/* char is present */
	char buf[MIL];
	sprintf( buf, "By your decision %s has been %s to the rank of %s.\n\r",
		 PERS2( ch ),
		 pv->type == VOTE_CAB_DEMO ? "demoted" : "promoted",
		 get_crank( ch ));
	cabal_echo( ch->pCabal, buf );
	if (has_child_choice( ch->pCabal, ch->pcdata->rank )){
	  send_to_char("`2You are now able to choose further within your organization. (\"cabal join\")``\n\r", ch);
	}
      }
    }
    /* check if we need to purge the character */
    if (fPurge){
      purge( ch );
    }
    break;
  case VOTE_LAW:
    if ( (pc = get_cabal_vnum( pv->value[0] )) == NULL){
      bug("execute_vote: VOTE_LAW could not find cabal index %d.", pv->value[0]);
      break;
    }
    else{
      AREA_DATA* pArea;
      char to [MIL];
      sprintf( buf, "The %s of %s have %s %s.\n\rResults:\n\r"\
	       "%d For, %d Against, %d Pass\n\r%s\n\r",
	       pv->veto ? "leader" : "members",
	       pc->name,
	       pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	       pv->subject,
	       pv->yes, pv->no, pv->pass,
	       !fPass ? "" : "Seek further guidance in help scrolls about the new penalty." );
      sprintf( to, "%s city", pv->owner );
      do_hal( to, pv->subject, buf, NOTE_NEWS );
      if (!fPass)
	break;
      /* we change the cities status */
      if ( (pArea = pc->city ) == NULL){
	bug("execute_vote: VOTE_LAW could not find an area for cabal vnum %d.", pc->vnum);
	break;
      }
      pArea->crimes[pv->value[1]] = pv->value[2];
      save_area( pArea );
    }
    break;
  case VOTE_CAB_TAX:
    if ( (pc = get_cabal_vnum( pv->value[0] )) == NULL){
      bug("execute_vote: VOTE_CAB_TAX could not find cabal index %d.", pv->value[0]);
      break;
    }
    else{
      char to [MIL];
      sprintf( buf, "The %s of %s have %s %s.\n\rResults:\n\r"\
	       "%d For, %d Against, %d Pass\n\r%s\n\r",
	       pv->veto ? "leader" : "members",
	       pc->name,
	       pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	       pv->subject,
	       pv->yes, pv->no, pv->pass,
	       !fPass ? "" : "Seek further guidance in help scrolls about the effects of tax." );
      sprintf( to, "%s", pv->owner );
      do_hal( to, pv->subject, buf, NOTE_NEWS );
      if (!fPass)
	break;
      /* we change the tax status */
      pc->cp_tax = pv->value[1];
      CHANGE_CABAL( pc );
      save_cabals( TRUE, NULL );
    }
    break;
  case VOTE_CAB_SPONS:
    if ( (pc = get_cabal_vnum( pv->value[0] )) == NULL){
      bug("execute_vote: VOTE_SPONS could not find cabal index %d.", pv->value[0]);
      break;
    }
    else{
      char to [MIL];
      sprintf( buf, "The %s of %s have %s %s.\n\rResults:\n\r"\
	       "%d For, %d Against, %d Pass\n\r%s\n\r",
	       pv->veto ? "leader" : "members",
	       pc->name,
	       pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	       pv->subject,
	       pv->yes, pv->no, pv->pass,
	       !fPass ? "" : "Seek further guidance in help scrolls about the effects of sponsorship." );
      sprintf( to, "%s", pc->name );
      do_hal( to, pv->subject, buf, NOTE_NEWS );
      if (!fPass)
	break;
      /* we toggle sponsorship */
      TOGGLE_BIT(pc->flags, CABAL_SPONSOR);
      CHANGE_CABAL( pc );
      save_cabals( TRUE, NULL );
    }
    break;
  case VOTE_TITHE:
    if ( (pc = get_cabal_vnum( pv->value[0] )) == NULL){
      bug("execute_vote: VOTE_TITHE could not find cabal index %d.", pv->value[0]);
      break;
    }
    if ( (pc2 = get_cabal_vnum( pv->value[1] )) == NULL){
      bug("execute_vote: VOTE_TITHE could not find second cabal index %d.", pv->value[1]);
      break;
    }
    else{
      char to [MIL];
      sprintf( buf, "The %s of %s have %s %s.\n\rResults:\n\r"\
	       "%d For, %d Against, %d Pass\n\r%s\n\r",
	       pv->veto ? "leader" : "members",
	       pc->name,
	       pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	       pv->subject,
	       pv->yes, pv->no, pv->pass,
	       !fPass ? "" : "Seek further guidance in help scrolls about the effects of tithe." );
      sprintf( to, "%s %s", pc->name, pc2->name );
      do_hal( to, pv->subject, buf, NOTE_NEWS );
      if (!fPass){
	CP_CAB_GAIN( pc, pv->value[2] );
	break;
      }
      CP_CAB_GAIN( pc2, pv->value[2] );
      affect_cabal_relations(pc, pc2, pv->value[2] / 4, TRUE );
      CHANGE_CABAL( pc2 );
      save_cabals( TRUE, NULL );
    }
    break;
  case VOTE_TOME:
    if ( (pc = get_cabal_vnum( pv->value[0] )) == NULL){
      bug("execute_vote: VOTE_TOME could not find cabal index %d.", pv->value[0]);
      break;
    }
    else{
      char to [MIL];
      sprintf( buf, "The %s of %s have %s %s.\n\rResults:\n\r"\
	       "%d For, %d Against, %d Pass\n\r",
	       pv->veto ? "leader" : "members",
	       pc->name,
	       pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	       pv->subject,
	       pv->yes, pv->no, pv->pass);
      sprintf( to, "%s", pv->owner );
      do_hal( to, pv->subject, buf, NOTE_NEWS );
      if (!fPass){
	break;
      }
      //add the new vote HERE
      else{
	TOME_DATA* tome;
	char sub[MIL];
	char tit[MIL];
	char* ptr;

	/* extract the subject/title from subject line */
	if ( (ptr = strchr(pv->subject, ':')) == NULL){
	  bug("execute_vote: VOTE_TOME could not find subject terminator.", 0);
	  return;
	}
	*ptr = 0;
	strcpy(tit, ptr + 1);
	strcpy(sub, pv->subject);

	if ( (tome = AddTome(sub, tit, pv->from, pv->string )) != NULL){
	  EchoTome( tome );
	  SaveTomes();
	}
      }
    }
  case VOTE_BUILD:
    if ( (pc = get_cabal_vnum( pv->value[0] )) == NULL){
      bug("execute_vote: VOTE_BUILD could not find cabal index %d.", pv->value[0]);
      break;
    }
    else{
      char to [MIL];
      CVROOM_DATA* pcv, *to_cvroom;
      CVROOM_DATA* old;
      EXIT_DATA* pExit;
      int door = 0;
      int to_pos[3];
      bool fSell = pv->value[4] > 0;

      sprintf( buf, "The %s of %s have %s %s.\n\rResults:\n\r"\
	       "%d For, %d Against, %d Pass\n\r",
	       pv->veto ? "leader" : "members",
	       pc->name,
	       pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	       pv->subject,
	       pv->yes, pv->no, pv->pass);
      sprintf( to, "%s", pc->name );
      do_hal( to, pv->subject, buf, NOTE_NEWS );

      /* SELL */
      if (fSell){
	int doors = 0;
	EXIT_DATA* pexit;

	if (!fPass)
	  break;
	/* get a complete cvroom that this vote referes to */
	pcv = get_cvroom_xyz( pv->value[1], pv->value[2], pv->value[3], pc, TRUE, FALSE );

	if (pcv == NULL){
	  sprintf( buf, "execute_vote: CAB_BUILD: could not get cvroom %d %d %d to sell",
		   pv->value[1], pv->value[2], pv->value[3] );
	  bug( buf, 0);
	  break;
	}
	/* give cabal the cost due */
	if (pcv->pRoom->vnum != ROOM_VNUM_CROOM_DUMMY)
	  CP_CAB_GAIN( pcv->pRoom->pCabal, pv->value[4] );

	/* now we know which room we are selling, we check if this room has more then 1 exit */
	for (door = 0; door  < MAX_DOOR; door ++ ){
	  if ( (pexit = pcv->pRoom->exit[door]) != NULL && pexit->to_room != NULL)
	    doors++;
	}

	sell_room( pcv, pc, pv->value[4] );

	VRCHANGE_CABAL( pc );
	CHANGE_CABAL( pc );
	save_cabals( TRUE, NULL );
	break;
      }
      /* EDIT/BUILD */
      /* get an incomplete cvroom that this vote referes to */
      pcv = get_cvroom_xyz( pv->value[1], pv->value[2], pv->value[3], pc, FALSE, TRUE );

      if (pcv == NULL){
	sprintf( buf, "execute_vote: CAB_BUILD: could not get cvroom %d %d %d",
		 pv->value[1], pv->value[2], pv->value[3] );
	bug( buf, 0);
	break;
      }
      if (!fPass){
	CP_CAB_GAIN( pc, -pv->value[4] );
	cvroom_from_cabal( pcv, pc );
	VRCHANGE_CABAL( pc );
	free_cvroom( pcv );
	break;
      }
      /* find if this overlaps an existing room (edit), and remove it */
      if ( (old = get_cvroom_xyz( pv->value[1], pv->value[2], pv->value[3], pc, TRUE, FALSE )) != NULL){
	unload_vir_room( old->pRoom );
	cvroom_from_cabal( old, pc );
	VRCHANGE_CABAL( pc );
	free_cvroom( old );
      }

      /* we change the rooms status and load it */
      pcv->pRoom->area = pc->anchor->area;
      pcv->fComplete = TRUE;
      fix_vir_exits( pcv );
      load_vir_room( pcv->pRoom, pc->anchor->area );

      /* we check if we have to update exits on rooms waiting to be approved */
      for (door = 0; door < MAX_DOOR; door++){
	if ( (pExit = pcv->pRoom->exit[door]) == NULL)
	  continue;

	/* check if this exit leads to another cvroom */
	if (!IS_VIRVNUM(pExit->vnum))
	  continue;

	/* now we have an exit from cvroom to cvroom, we get that room by our position + exit direction */
	to_pos[P_X]	= pcv->pos[P_X];
	to_pos[P_Y]	= pcv->pos[P_Y];
	to_pos[P_Z]	= pcv->pos[P_Z];

	get_new_coordinates( to_pos, door );
	/* we now look for INCOMPLETE room in that position */
	if ( (to_cvroom = get_cvroom_xyz(to_pos[P_X], to_pos[P_Y], to_pos[P_Z], pcv->pRoom->pCabal, FALSE, TRUE)) == NULL
	     || to_cvroom->pRoom->exit[rev_dir[door]] != NULL)
	  continue;
	else{
	  to_cvroom->pRoom->exit[rev_dir[door]] = new_exit();
	  to_cvroom->pRoom->exit[rev_dir[door]]->vnum = pcv->pRoom->vnum;
	  to_cvroom->pRoom->exit[rev_dir[door]]->key = 0;
	  to_cvroom->pRoom->exit[rev_dir[door]]->orig_door = rev_dir[door];
	  to_cvroom->pRoom->exit[rev_dir[door]]->exit_info = pExit->rs_flags;
	  to_cvroom->pRoom->exit[rev_dir[door]]->rs_flags = pExit->rs_flags;
	  if (!IS_NULLSTR(pExit->keyword))
	    to_cvroom->pRoom->exit[rev_dir[door]]->keyword = str_dup(pExit->keyword);
	  if (!IS_NULLSTR(pExit->description))
	    to_cvroom->pRoom->exit[rev_dir[door]]->description = str_dup(pExit->description);
	}
      }
      refresh_cvroom_flags( pc );
      VRCHANGE_CABAL( pc );
      CHANGE_CABAL( pc );
    }
    break;
  case VOTE_PACT:
    if ( (pc = get_cabal_vnum( pv->value[0] )) == NULL){
      bug("execute_vote: VOTE_PACT could not find cabal index %d.", pv->value[0]);
      break;
    }
    else if ( (pc2 = get_cabal_vnum( pv->value[1] )) == NULL){
      bug("execute_vote: VOTE_PACT could not find cabal index %d.", pv->value[0]);
      break;
    }
    else{
      char to [MIL];
      if (pv->value[4] == PACT_VENDETTA){
	/* send a message out to the other cabal that a vendetta has been declared */
	sprintf( buf, "The members of [%s] have declared a Vendetta against you!\n\r",
		 pc->who_name);
	do_hal( pc2->name, "A Vendetta has been declared!", buf, NOTE_NEWS );
	sprintf( to, "%s", pc->name );
      }
      /* on embargo send a message out that all trade has stopped */
      else if (pv->value[4] == PACT_BREAK){
	/* send a message out to the other cabal that a vendetta has been declared */
	sprintf( buf, "The members of [%s] have stopped all trade with you.\n\r",
		 pc->who_name);
	do_hal( pc2->name, pv->subject, buf, NOTE_NEWS );
	/* we setup "to" so that only the creatore of this embargo gets the next hal note */
	sprintf( to, "%s", pc->name );
      }
      else
	sprintf( to, "%s %s", pc->name, pc2->name );
      /* safeguard: PACT_IMPROVE fails if we try to improve a pact to
	 an alliance with an alliance already in cabal
      */
      if (pv->value[4] == PACT_IMPROVE
	  && pv->value[2] == PACT_ALLIANCE
	  && (IS_CABAL(pc, CABAL_ALLIANCE) || IS_CABAL(pc2, CABAL_ALLIANCE))
	  ){
	fPass = FALSE;
	pv->veto = TRUE;
      }

      sprintf( buf, "The %s of %s have %s %s.\n\rResults:\n\r"\
	       "%d For, %d Against, %d Pass\n\r%s\n\r",
	       pv->veto ? "leader" : "members",
	       pc->name,
	       pv->veto ? "VETOED" : !fPass ? "REJECTED" : "APPROVED",
	       pv->subject,
	       pv->yes, pv->no, pv->pass,
	       fPass ? "" : "Seek further guidance in help scrolls about effects of this pact." );
      do_hal( to, pv->subject, buf, NOTE_NEWS );

      /* vote failed, we remove the pact and matching votes if needed */
      if (!fPass){
	PACT_DATA* pp = get_pact_abs( pc, pc2, pv->value[4], FALSE);
	VOTE_DATA* pVote = NULL;
	if (pp){
	  rem_pact( pp );
	  free_pact ( pp );
	  save_cabals( FALSE, NULL );
	}
	/* remove matching trade vote if this was a trade vote */
	if (pv->value[4] >= PACT_TRADE && pv->value[4] <= PACT_ALLIANCE){
	  if ( (pVote =  get_vote( VOTE_PACT, pc->vnum, pc2->vnum, 0, 0, PACT_TRADE)) != NULL){
	    rem_vote ( pVote );
	    free_vote( pVote );
	  }
	  if ( (pVote =  get_vote( VOTE_PACT, pc2->vnum, pc->vnum, 0, 0, PACT_TRADE)) != NULL){
	    rem_vote ( pVote );
	    free_vote( pVote );
	  }
	}
	/* remove a matching peace vote if this was a peace vote */
	else if (pv->value[4] == PACT_PEACE){
	  if ( (pVote =  get_vote( VOTE_PACT, pc->vnum, pc2->vnum, 0, 0, PACT_PEACE)) != NULL){
	    rem_vote ( pVote );
	    free_vote( pVote );
	  }
	}
	/* remove a matching peace vote if this was an improvement vote */
	else if (pv->value[4] == PACT_IMPROVE){
	  if ( (pVote =  get_vote( VOTE_PACT, pc->vnum, pc2->vnum, 0, 0, PACT_IMPROVE)) != NULL){
	    rem_vote ( pVote );
	    free_vote( pVote );
	  }
	  break;
	}
      }
      /* vote sucess, we either create incomplete pact, or complete a pact */
      else{
	PACT_DATA* pp = get_pact_abs( pc, pc2, pv->value[4], FALSE);

	/* we check if the pact exist or is already completed */
	if ( pp == NULL ){
	  if ( (pp = create_pact(pc, pc2, pv->value[4])) == NULL){
	    bug("execute_vote: VOTE_PACT could not create a pact type %d", pv->value[4]);
	    break;
	  }

	  /* we have an empty pact now, set the gains on it */
	  /* SPECIAL CASES: */

	  /* Vendetta automaticly creates a complete pact */
	  if (IS_SET(pv->value[4], PACT_VENDETTA)){
	    VOTE_DATA* pVote;
	    PACT_DATA* pTrade = NULL;

	    pp->complete= PACT_COMPLETE;
	    pp->b_cp	= pv->value[2];
	    pp->b_sup	= pv->value[3];

	    /* remove a vendetta and trade votes from the other cabal if they exists */
	    if ( (pVote =  get_vote( VOTE_PACT, pc2->vnum, pc->vnum, 0, 0, PACT_VENDETTA)) != NULL){
	      rem_vote ( pVote );
	      free_vote( pVote );
	    }
	    /* remove trade votes */
	    if ( (pVote =  get_vote( VOTE_PACT, pc->vnum, pc2->vnum, 0, 0, PACT_TRADE)) != NULL){
	      rem_vote ( pVote );
	      free_vote( pVote );
	    }
	    if ( (pVote =  get_vote( VOTE_PACT, pc2->vnum, pc->vnum, 0, 0, PACT_TRADE)) != NULL){
	      rem_vote ( pVote );
	      free_vote( pVote );
	    }
	    /* we clean out all trade pacts between cabals as well */
	    if ( (pTrade = get_pact_abs( pc, pc2, PACT_TRADE, TRUE)) != NULL){
	      rem_pact( pTrade);
	      free_pact( pTrade );
	    }
	    /* we hurt relations between cabals and their trade partners */
	    affect_cabal_relations(pc, pc2, -1000, FALSE );
	  }
	  /* EMBARGO automaticly destroyes all trade pacts between cabals, and then expires (does not become a pact) */
	  else if (IS_SET(pv->value[4], PACT_BREAK)){
	    PACT_DATA* pTrade = NULL;
	    /* we hurt relations between cabals and their trade partners */
	    affect_cabal_relations(pc, pc2, -500, FALSE );

	    /* check for alliance first, in order to dissolve armies */
	    if ( (pTrade = get_pact_abs( pc, pc2, PACT_ALLIANCE, FALSE)) != NULL){
	      if (break_alliance_check( pc, pc2 )){
		char buf[MIL];
		sprintf(buf, "The armies stationed with %s bastions have been dismissed.", pc->name );
		do_hal( pc2->name, "Some of your armies have been dismissed.", buf, NOTE_NEWS );
	      }
	      if (break_alliance_check( pc2, pc )){
		char buf[MIL];
		sprintf(buf, "The armies stationed with %s bastions have been dismissed.", pc2->name );
		do_hal( pc->name, "Some of your armies have been dismissed.", buf, NOTE_NEWS );
	      }

	      rem_pact( pTrade);
	      free_pact( pTrade );
	      free_pact( pp );
	      CHANGE_CABAL( pc );
	      CHANGE_CABAL( pc2 );
	      save_cabals( TRUE, NULL );
	      break;
	    }
	    if ( (pTrade = get_pact_abs( pc, pc2, PACT_TRADE, TRUE)) != NULL){
	      rem_pact( pTrade);
	      free_pact( pTrade );
	      free_pact( pp );
	      save_cabals( TRUE, NULL );
	      break;
	    }
	    else{
	      free_pact( pp );
	      save_cabals( TRUE, NULL );
	      break;
	    }
	  }
	  else
	    pp->complete	= PACT_INCOMPLETE;
	  pp->c_cp	= pv->value[2];
	  pp->c_sup	= pv->value[3];
	  add_pact( pp );
	  save_cabals( FALSE, NULL );
	}
/* this should never happen */
	else if ( pp->complete == PACT_NONE ){
	  pp->complete	= PACT_INCOMPLETE;
	  pp->c_cp	= pv->value[2];
	  pp->c_sup	= pv->value[3];
	}
/* VOTE WAS COMPLETED ONLY BY ONE SIDE THE OTHER SIDE NOW COMPLETED A VOTE AS WELL */
	else{
	  AREA_DATA* pArea = NULL;
	  /* we complete the vote */
	  pp->complete = PACT_COMPLETE;
	  pp->b_cp	= pv->value[2];
	  pp->b_sup	= pv->value[3];
	  refresh_pact_flags();

	  /* special behavior for completion */

	  /* improvement of trade sets the maximum of trade between cabals to its value[2] value */
	  if (IS_SET(pv->value[4], PACT_IMPROVE)){
	    PACT_DATA* pTrade = get_pact( pc, pc2, PACT_TRADE, TRUE);

	    if (pTrade)
	      pTrade->Adv = pv->value[2];
	    /* expire the improve pact right away */
	    rem_pact( pp );
	    free_pact( pp );
	  }
	  /* if this is a peace vote we remove any vendettas between the cabals, and votes for them then expires */
	  else if (IS_SET(pv->value[4], PACT_PEACE)){
	    PACT_DATA* pVen = get_pact_abs( pc, pc2, PACT_VENDETTA, FALSE);
	    VOTE_DATA* pVote;

	    /* remove any vendetta  votes */
	    if ( (pVote =  get_vote( VOTE_PACT, pc2->vnum, pc->vnum, 0, 0, PACT_VENDETTA)) != NULL){
	      rem_vote ( pVote );
	      free_vote( pVote );
	    }
	    if ( (pVote =  get_vote( VOTE_PACT, pc->vnum, pc2->vnum, 0, 0, PACT_VENDETTA)) != NULL){
	      rem_vote ( pVote );
	      free_vote( pVote );
	    }
	    /* remove a vendetta pact between the cabals */
	    if (pVen){
	      rem_pact( pVen );
	      free_pact ( pVen );
	      save_cabals( FALSE, NULL );
	    }
	    /* expire the peace pact right away */
	    rem_pact( pp );
	    free_pact( pp );
	    /* we improve relations between cabals and their trade partners */
	    affect_cabal_relations(pc, pc2, 200, FALSE );
	    affect_cabal_relations(pc2, pc, 200, FALSE );
	  }
	  /* EMBARGO automaticly destroyes all trade pacts between cabals, and then expires (does not become a pact) */
	  else if (IS_SET(pv->value[4], PACT_BREAK)){
	    PACT_DATA* pTrade = NULL;
	    /* we hurt relations between cabals and their trade partners */
	    affect_cabal_relations(pc, pc2, -500, FALSE );

	    /* check for alliance first, in order to dissolve armies */
	    if ( (pTrade = get_pact_abs( pc, pc2, PACT_ALLIANCE, FALSE)) != NULL){
	      if (break_alliance_check( pc, pc2 )){
		char buf[MIL];
		sprintf(buf, "The armies stationed with %s bastions have been dismissed.", pc->name );
		do_hal( pc2->name, "Some of your armies have been dismissed.", buf, NOTE_NEWS );
	      }
	      if (break_alliance_check( pc2, pc )){
		char buf[MIL];
		sprintf(buf, "The armies stationed with %s bastions have been dismissed.", pc2->name );
		do_hal( pc->name, "Some of your armies have been dismissed.", buf, NOTE_NEWS );
	      }

	      rem_pact( pTrade);
	      free_pact( pTrade );
	      rem_pact( pp);
	      free_pact( pp );
	      save_cabals( TRUE, NULL );
	      break;
	    }
	    if ( (pTrade = get_pact_abs( pc, pc2, PACT_TRADE, TRUE)) != NULL){
	      rem_pact( pTrade);
	      free_pact( pTrade );
	      rem_pact( pp);
	      free_pact( pp );
	      save_cabals( TRUE, NULL );
	      break;
	    }
	    else{
	      rem_pact( pp);
	      free_pact( pp );
	      save_cabals( TRUE, NULL );
	      break;
	    }
	  }
	  /* we change the cities status if this was a trade pact with justice */
	  else if (IS_CABAL(pc2, CABAL_JUSTICE) && IS_CABAL(pc, CABAL_ROYAL)){
	    if ( (pArea = pc->city) == NULL){
	      bug("execute_vote: VOTE_PACT could not find an area for cabal vnum %d.", pv->value[0]);
	      break;
	    }
	    else{
	      /* send a note to the area about lawfulness */
	      char buf[MIL];
	      char to[MIL];
	      sprintf(buf, warn_table[WARN_LAWFUL].subj, pArea->name, pc2->name);
	      sprintf(to, "%s city", pc->name );
	      do_hal(to, buf, warn_table[WARN_LAWFUL].text, NOTE_NEWS);
	      SET_BIT(pArea->area_flags, AREA_LAWFUL );
	      affect_cabal_relations(pc, pc2, 200, FALSE );
	      affect_cabal_relations(pc2, pc, 200, FALSE );
	    }
	  }
	  else if (IS_CABAL(pc, CABAL_JUSTICE) && IS_CABAL(pc2, CABAL_ROYAL)){
	    if ( (pArea = pc2->city) == NULL){
	      bug("execute_vote: VOTE_PACT could not find an area for cabal vnum %d.", pv->value[0]);
	      break;
	    }
	    else{
	      /* send a note to the area about lawfulness */
	      char buf[MIL];
	      char to[MIL];
	      sprintf(buf, warn_table[WARN_LAWFUL].subj, pArea->name, pc->name);
	      sprintf(to, "%s city", pc2->name );
	      do_hal(to, buf, warn_table[WARN_LAWFUL].text, NOTE_NEWS);
	      SET_BIT(pArea->area_flags, AREA_LAWFUL );
	      affect_cabal_relations(pc2, pc, 200, FALSE );
	      affect_cabal_relations(pc, pc2, 200, FALSE );
	    }
	  }
	  save_cabals( FALSE, NULL );
	}//END COMPLETE PACT
      }//END if vote was success
    }//END BOTH CABALS FOUND
    break;
  }
/* we now nuke the vote */
  rem_vote( pv );
  free_vote( pv );
  save_mud();
  save_cabals( TRUE, NULL );
}

/* checks a single vote and decides if the vote should be judged for pass/fail
 * CRITERIA for vote judgement:
 *
 * 1) If yes + no + pass >= max_vote
 * 2) life_time < current_time
 * 3) If yes + no + pass > 3 * max_vote / 4 && (time_stamp + life_time) / 2 < current_time //allows for early majority vote
 * 4) vote has been VETOED
 *
 * If above are satisfied judge_vote is called.					*/
void check_vote( VOTE_DATA* pv ){
  int rem_votes = 0;

  if (pv == NULL)
    return;
  else
    rem_votes = pv->max_vote - pv->yes - pv->no - pv->pass;

  if (pv->veto
      || (pv->yes + pv->no + pv->pass) >= pv->max_vote	//all votes done
      || pv->life_time < mud_data.current_time		//vote expired
      || pv->yes > pv->no + rem_votes
      || pv->no > pv->yes + rem_votes
      || ( (pv->yes + pv->no + pv->pass) >= 3 * pv->max_vote / 4
	   && (pv->time_stamp + pv->life_time) / 2 < mud_data.current_time) ){
    execute_vote( pv, judge_vote( pv ) );
  }
}

/* this function is called once per PULSE_VOTE, and on each reboot to refresh votes */
void vote_update( ){
  VOTE_DATA* pv, *pv_next;

  for (pv = vote_list; pv != NULL; pv = pv_next){
    pv_next = pv->next;
    check_vote( pv );
  }
}
