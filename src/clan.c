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
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <memory.h>
#include "merc.h"
#include "clan.h"

/***************************************************************/
/*Following are army orientanted routines for Forsaken Lands  */
/*mud created by Virigoth circa Apr 2004.  Copyrighted for Forsaken*/
/*Lands mud Apr 04, 2004.  Do not use or copy without explicit */
/*permission of author. (Voytek Plawny aka Virigoth)	       */
/***************************************************************/

/* GLOBALS */
static CLAN_DATA clan_list[MAX_CLANS];

/* PROTOTYPES */
char* ClanFixName( char* name );

/* PRIVATE */
//zero the clan list
void Reset(){
  memset( clan_list, 0, sizeof( CLAN_DATA ) * MAX_CLANS);
}

//convert a clan name to handle, 0 on not found
int ClanNameToHandle( char* name ){
  int clan;

  for ( clan = 0; clan < MAX_CLANS; clan++){
    if (IS_NULLSTR(clan_list[clan].name) || str_cmp(name, clan_list[clan].name))
      continue;
    else
      return ( clan + 1);
  }
  return 0;
}

//convert handle to name, NULL on not found
char* ClanHandleToName( int handle ){
  static char* nul = "";

  if (handle < 1 || handle > MAX_CLANS || IS_NULLSTR(clan_list[handle - 1].name))
    return nul;
  else
    return clan_list[handle - 1].name;
}

//returns clan data based on handle
CLAN_DATA* GetClanData( int handle ){
  if (handle < 1 || handle > MAX_CLANS)
    return NULL;
  return ( &clan_list[handle - 1]);
}

 
int ClanInsertMember( int handle, CLAN_MEMBER* pCmd ){
  CLAN_DATA* pClan = GetClanData( handle );
  CLAN_MEMBER* prev, *new;

  if (pClan == NULL || pCmd == NULL){
    return 0;
  }
  else if ( (new = GetClanMember( handle, pCmd->name)) != NULL){
    return handle;
  }
  /* search for the end of the member list */
  for (prev = &pClan->members; prev->next; prev = prev->next);
  
  /* attach the new member */
  new = alloc_mem( sizeof( *new ));
  

  memset( new, 0, sizeof( *new ));
  new->name		= str_dup( pCmd->name );
  new->class		= pCmd->class;
  new->race		= pCmd->race;
  new->level		= pCmd->level;
  new->rank		= pCmd->rank;
  new->pts		= pCmd->pts;
  new->last_logon	= pCmd->last_logon;
  new->joined		= pCmd->joined;
  new->allowed		= pCmd->allowed;

  new->next = NULL;
  prev->next = new;

  
  pClan->max++;
  return handle;
}

//adds a member to a clan
int ClanAddMember( int handle, CHAR_DATA* ch){
  CLAN_MEMBER tmp;

  if (handle < 1 || handle > MAX_CLANS || ch == NULL)
    return 0;

  tmp.name		= ch->name;
  tmp.class		= ch->class;
  tmp.race		= ch->race;
  tmp.level		= ch->race;
  tmp.rank		= 0;
  tmp.last_logon	= ch->logoff;
  tmp.joined		= mud_data.current_time;
  tmp.allowed		= 0;
  tmp.pts		= ch->pcdata->clan_points + mud_data.mudport == TEST_PORT ? 1 : 0;

  return (ClanInsertMember( handle, &tmp ));
}

//removes a given member from a clan
bool ClanRemMember( int handle, char* name ){
  CLAN_DATA* pClan = GetClanData( handle );
  CLAN_MEMBER* prev, *tmp = NULL;

  if (pClan == NULL || IS_NULLSTR( name )){
    return FALSE;
  }

  /* search members untill we find target name */
  for (prev = &pClan->members; prev->next; prev = prev->next){
    if (!str_cmp(prev->next->name, name)){
      tmp = prev->next;
      break;
    }
  }

  if (tmp == NULL){
    bug("clan.c>ClanRemMember: name not found.", 0);
    return FALSE;
  }

  /* unlink the member */
  prev->next = tmp->next;

  /* free strings */
  free_string( tmp->name );

  /* free the member */
  free_mem( tmp, sizeof( *tmp ));

  pClan->max--;
  return TRUE;
}

//adds a clan to the list of clans, returns handle
int ClanAdd( CLAN_DATA* pClan){
  int handle;

  if (pClan == NULL || IS_NULLSTR(pClan->name))
    return 0;
  else if ( (handle = ClanNameToHandle( pClan->name )) > 0){
    return (handle);
  }
  else{
    /* try to find an empty spot */
    for (handle = 0; handle < MAX_CLANS; handle++){
      if (IS_NULLSTR(clan_list[handle].name)){
	memcpy(&clan_list[handle], pClan, sizeof( *pClan ));
	clan_list[handle].name	= str_dup( pClan->name );
	return (handle + 1);
      }
    }
    return 0;
  }
}

//removes a clan from the list
bool ClanRem( int handle ){
  CLAN_DATA* pClan = GetClanData( handle );
  char path[MIL];

  if (pClan == NULL){
    bug("clan.c>ClanRem: Could not find clan %d", handle );
    return FALSE;
  }

  /* free members */
  while (pClan->members.next)
    ClanRemMember( handle, pClan->members.next->name );

  /* unlink save file */
  sprintf( path, "%s%s", CLAN_DIR, ClanFixName( pClan->name ) );
  unlink( path );
  /* free strings */
  free_string( pClan->name );
  pClan->name = NULL;
  pClan->max = 0;

  return TRUE;
}

//lists all the clans loaded
void ListClans( CHAR_DATA* ch ){
  int clan;

  for (clan = 0; clan < MAX_CLANS; clan++){
    if (IS_NULLSTR(clan_list[clan].name))
      continue;
    sendf(ch, "%-25.25s (%-3d)\n\r", clan_list[clan].name, clan_list[clan].max );
  }
}

//returns a string with clan data for one member
char* PrintMemberData( CLAN_MEMBER* pCmd, bool fImm ){
  static char out[MIL];

  out[0] = 0;

  /* IMM info */
  if (fImm){
    if (pCmd == NULL){
      sprintf( out, 
	       "Name             Race      Class       Rank     Lvl  Sen Log  Pts    \n\r"\
	       "----------------------------------------------------------------------");
    }
    else{
      sprintf( out, 
	       "%-15.15s %-10.10s %-10.10s %-10.10s  %-2d  %-3ld %-3ld %-3d",
	       pCmd->name,
	       pc_race_table[pCmd->race].name,
	       class_table[pCmd->class].name,
	       clanr_table[pCmd->rank][1],
	       pCmd->level,
	       (mud_data.current_time - pCmd->joined) / 86400,
	       (mud_data.current_time - pCmd->last_logon) / 86400,
	       pCmd->pts);
    }
  }
  /* NORMAL */
  else{
    if (pCmd == NULL){
      sprintf( out, 
	       "Name           Rank     Lvl   Days Ago\n\r"\
	       "----------------------------------------");
    }
    else{
      sprintf( out, 
	       "%-15.15s %-10.10s %-2d     %-3ld",
	       pCmd->name,
	       clanr_table[pCmd->rank][1],
	       pCmd->level,
	       (mud_data.current_time - pCmd->last_logon) / 86400);
      
    }
  }
  return ( out );
}

//lists members of a given clan
//types are: seniority, rank, pts (default)
void ListMembers( CHAR_DATA* ch, int handle, bool fImm, char* type ){
  CLAN_DATA* pClan;
  CLAN_MEMBER* pCmd;

  const int max_search	= 128;
  const int max_show	= 16;
  CLAN_MEMBER* members[max_search];
  int max_member = 0, i, j;

  bool fRank = FALSE, fSen = FALSE;

  if ( (pClan = GetClanData( handle )) == NULL)
    return;

  for (pCmd = pClan->members.next; pCmd; pCmd = pCmd->next){
    if ((mud_data.current_time - pCmd->last_logon) > MAX_INACTIVE)
      continue;
    /* Viri: Lets see for now if this pruning is really necessary
       else if (pCmd->rank < RANK_ELDER && pCmd->pts < 1)
      continue;
    */
    if (max_member < max_search)
      members[max_member++] = pCmd;
  }

  if (!str_prefix(type, "rank"))
    fRank = TRUE;
  if (!str_prefix(type, "seniority"))
    fSen = TRUE;
  
  /* sort */
  if (fRank){
    for (i = 0; i < max_member; i++){
      pCmd = members[i];
      for (j = i; j > 0 && members[j - 1]->rank < pCmd->rank; j--){
	members[j] = members[j - 1];
      }
      members[j] = pCmd;
    }
  }
  else if (fSen){
    for (i = 0; i < max_member; i++){
      pCmd = members[i];
      for (j = i; j > 0 && members[j - 1]->joined > pCmd->joined; j--){
	members[j] = members[j - 1];
      }
      members[j] = pCmd;
    }
  }
  else{
    for (i = 0; i < max_member; i++){
      pCmd = members[i];
      for (j = i; j > 0 && members[j - 1]->pts < pCmd->pts; j--){
	members[j] = members[j - 1];
      }
      members[j] = pCmd;
    }
  }

  /* show */
  sendf( ch, "%s\n\r", PrintMemberData( NULL, fImm ) );
  for (i = 0; i < max_member && i < max_show; i++){
    sendf( ch, "%s\n\r", PrintMemberData( members[i], fImm ) );
  }
}

//fixes a clan name to be ok for a filename
char* ClanFixName( char* name ){
  static char out[MIL];
  char* ptr = out;

  *ptr = 0;

  while (*name ){
    switch( *name ){
    default:	
      if (isalpha( *name ))
	*ptr = *name;		
      else
	*ptr = '_';
      break;

    case ' ':	*ptr = '_';		break;
    }
    ptr++;
    name++;
  }
  *ptr = 0;
  return out;
}
      

//writes a single clan's data to file
void fWriteClan( FILE *fp, CLAN_DATA* pClan ){
  CLAN_MEMBER* pCmd;

  //start the file off with the name
  fprintf( fp, "CLAN %s~\n", pClan->name );

  for (pCmd = pClan->members.next; pCmd; pCmd = pCmd->next){
    fprintf( fp, 
	     "MEMBER %s~\n"\
	     "RACE %d CLASS %d LEVEL %d\n"\
	     "RANK %d PTS %d\n"\
	     "JOIN %ld LAST %ld ALLOWED %ld\n"\
	     "MEMBER_END\n",
	     pCmd->name,
	     pCmd->race, pCmd->class, pCmd->level,
	     pCmd->rank, pCmd->pts,
	     pCmd->joined, pCmd->last_logon, pCmd->allowed);
  }
  fprintf( fp, "END\n\n");
}

//reads a single clan's data and loads the clan
void fReadClan( FILE* fp ){
  CLAN_DATA clan;
  CLAN_MEMBER member;
  char* word;
  int handle;
  bool fMatch;

  if ( (word = fread_word( fp )) == NULL){
    bug("clan.c>fReadClan: Error accessing file.\n\r", 0);
    return;
  }
  else if (str_cmp(word, "CLAN")){
    bug("can.c>fReadClan: Expected CLAN not found.", 0);
    return;
  }
  memset( &clan, 0, sizeof( CLAN_DATA ));
  //get name first
  clan.name	= fread_string( fp );
  if ( (handle = ClanAdd( &clan )) < 1){
    bug("clan.c>fReadClan: Could not get a handle for clan.", 0);
    return;
  }

  //reset the name data for use
  memset( &member, 0, sizeof( CLAN_MEMBER ));

  for (;;){
    fMatch = FALSE;

    if (feof( fp) || (word = fread_word( fp )) == NULL)
      word = "END";

    switch( UPPER(word[0])){
    case 'A':
      KEY( "ALLOWED",	member.allowed,		fread_number( fp ));
      break;
    case 'C':
      KEY( "CLASS",	member.class,		fread_number( fp ));
      break;
    case 'E':
      if (!str_cmp(word, "END")){
	fMatch = TRUE;
	return;
      }
      break;
    case 'J':
      KEY( "JOIN",	member.joined,		fread_number( fp ));
      break;
    case 'L':
      KEY( "LEVEL",	member.level,		fread_number( fp ));
      KEY( "LAST",	member.last_logon,		fread_number( fp ));
      break;
    case 'M':
      KEYS( "MEMBER",	member.name,		fread_string( fp ));
      if (!str_cmp(word, "MEMBER_END")){
	if (ClanInsertMember( handle, &member) < 1){
	  nlogf("[*****] BUG: clan.c>fReadClan: Could not insert member %s", member.name);
	  continue;
	}
	fMatch = TRUE;
	break;
      }
      break;
    case 'P':
      KEY( "PTS",	member.pts,		fread_number( fp ));
      break;
    case 'R':
      KEY( "RACE",	member.race,		fread_number( fp ));
      KEY( "RANK",	member.rank,		fread_number( fp ));
      break;
    }
    if (!fMatch){
      nlogf("[*****] BUG: clan.c>fReadClan: Can't match \"%s\"", word);
    }
  }
}
      

  

  

/* INTERFACE */ 
void LoadClans(){
  FILE* fp;
  DIR *directory;		//Directory
  struct dirent *dir_ent;	//Directory entry
  struct stat Stat;		//File statistics
  char path[MIL];		//Current file name

  Reset();

  /* Open dir, get first entry */  
  if ( (directory = opendir(CLAN_DIR)) == NULL){
    bug("clan.c>LoadClans: Could not access CLAN_DIR", 0);
    return;
  }
  else if ( (dir_ent = readdir(directory)) == NULL){
    bug("clan.c>LoadClans: Could not access first directory entry.", 0);
    closedir( directory );
    return;
  }

  fclose( fpReserve );

  while( dir_ent != NULL){
    sprintf( path, "%s%s", CLAN_DIR, dir_ent->d_name );

    if ( -1 == stat( path, &Stat)){
      dir_ent = readdir( directory );
      continue;
    }
    else if(!S_ISREG(Stat.st_mode)){
      dir_ent = readdir( directory );
      continue;
    }
    nlogf("Loading clan %s", dir_ent->d_name );
    if ( (fp = fopen( path, "r")) == NULL){
      perror( path );
      dir_ent = readdir( directory );
      continue;
    }
    fReadClan( fp );
    fclose( fp );
    dir_ent = readdir( directory );
  }
  closedir( directory );

  fpReserve = fopen( NULL_FILE, "r" );
}

//saves a single clan
void ClanSave( int handle ){
  CLAN_DATA* pClan = GetClanData( handle );
  FILE* fp;
  char path[MIL], buf[MIL];

  if (pClan == NULL){
    bug("clan.c>ClanSave: Could not get pointer to clan %d", handle );
    return;
  }

  sprintf( path, "%s%s", CLAN_DIR, CLAN_TMP_FILE );

  if ( (fp = fopen( path, "w")) == NULL){
    perror( path );
    return;
  }
  fWriteClan( fp, pClan );
  fclose( fp );

  sprintf( buf, "%s%s", CLAN_DIR, ClanFixName( pClan->name ));
  rename( path, buf );
}

void SaveClans(){
  int clan;

  fclose( fpReserve );
  for ( clan = 0; clan < MAX_CLANS; clan++){
    if (!IS_NULLSTR(clan_list[clan].name))
      ClanSave( clan + 1 );
  }
  fpReserve = fopen( NULL_FILE, "r" );
}

/* commands:
   clan list			//clan member only
   clan list clans		//imm only
   clan create <clan>		//imm only
   clan delete <clan>		//imp only
   clan draft  <victim>		//clan leader only
   clan draft  <victim> <clan>	//imm/mob only
   clan remove <victim>		//clan leader only
   clan remove <victim> <clan>	//imm/mob only
   clan prom <victim>		//elder only
   clan prom <victim> <clan>	//imm/mob only
   clan demo <victim>		//elder only
   clan demo <victim> <clan>	//imm/mob only
*/
void do_clan( CHAR_DATA* ch, char* argument ){
  int handle;
  char arg1[MIL], arg2[MIL], buf[MIL];
  
  argument = one_argument( argument, arg1 );

  if (IS_NULLSTR(arg1)){
    send_to_char("Common clan commands are: draft, remove, prom, demo.\n\r", ch);
    return;
  }

  /* CREATE */
  if (!str_prefix( arg1, "create")){
    CLAN_DATA clan;

    memset(&clan, 0, sizeof( clan ));

    if (!IS_IMMORTAL(ch)){
      send_to_char("You cannot create clans.\n\r", ch);
      return;
    }
    else if (IS_NULLSTR(argument)){
      send_to_char( "Create what clan?\n\r", ch);
      return;
    }
    
    sprintf( buf, "%s", argument );
    buf[0] = UPPER(buf[0]);
    clan.name = buf;

    if ( (handle = ClanNameToHandle( buf )) > 0){
      send_to_char("That clan already exists.\n\r", ch);
      return;
    }
    else if ( (handle = ClanAdd( &clan )) < 0){
      send_to_char("Error adding clan.\n\r", ch);
      return;
    }
    else
      sendf( ch, "Clan \"%s\" created.\n\r", ClanHandleToName( handle ));
    ClanSave( handle );
    return;
  }
  /* DELETE */
  if (!str_prefix( arg1, "delete")){

    if (get_trust(ch) < CREATOR){
      sendf( ch, "Requires level %d trust\n\r", CREATOR);
      return;
    }
    else if (IS_NULLSTR(argument)){
      send_to_char( "Delete what clan?\n\r", ch);
      return;
    }
    else if ( (handle = ClanNameToHandle( argument )) < 1){
      send_to_char("That clan doesn't exists.\n\r", ch);
      return;
    }
    else if ( !ClanRem( handle ) ){
      send_to_char("Error deleting clan.\n\r", ch);
      return;
    }
    else{
      send_to_char("Removed.\n\r", ch);
    }
    return;
  }
  /* LIST */
  if (!str_prefix( arg1, "list")){
    /* IMMORTAL LIST */
    if (IS_IMMORTAL(ch)){
      if (IS_NULLSTR(argument)){
	if (GET_CLAN(ch) < 1){
	  send_to_char( "List which clan?\n\r", ch);
	  return;
	}
	else
	  handle = GET_CLAN(ch);
      }
      if (!str_cmp(argument, "clans")){
	ListClans(ch);
	return;
      }
      else if ( (handle = ClanNameToHandle( argument )) < 1){
	send_to_char("No such clan.\n\r", ch);
	return;
      }
      argument = one_argument( argument, arg1);
      ListMembers( ch, handle, TRUE, "Pts");
      return;
    }
    /* MORTAL LIST */
    else if ( (handle = GET_CLAN(ch)) < 1){
      send_to_char("You do not belong to a clan.\n\r", ch );
      return;
    }
    else if (!CLAN_ELDER(ch)){
      send_to_char("You lack the privilige.\n\r", ch);
      return;
    }
    ListMembers( ch, handle, FALSE, "Pts");
    return;    
  }
  /* RANKS */
  else if (!str_prefix( arg1, "rank")){
    /* IMMORTAL LIST */
    if (IS_IMMORTAL(ch)){
      if (IS_NULLSTR(argument)){
	if (GET_CLAN(ch) < 1){
	  send_to_char( "List which clan?\n\r", ch);
	  return;
	}
	else
	  handle = GET_CLAN(ch);
      }
      if ( (handle = ClanNameToHandle( argument )) < 1){
	send_to_char("No such clan.\n\r", ch);
	return;
      }
      argument = one_argument( argument, arg1);
      ListMembers( ch, handle, TRUE, "Rank");
      return;
    }
    /* MORTAL LIST */
    else if ( (handle = GET_CLAN(ch)) < 1){
      send_to_char("You do not belong to a clan.\n\r", ch );
      return;
    }
    else if (!CLAN_ELDER(ch)){
      send_to_char("You lack the privilige.\n\r", ch);
      return;
    }
    ListMembers( ch, handle, FALSE, "Rank");
    return;    
  }
  /* SENIOR */
  else if (!str_prefix( arg1, "senior")){
    /* IMMORTAL LIST */
    if (IS_IMMORTAL(ch)){
      if (IS_NULLSTR(argument)){
	if (GET_CLAN(ch) < 1){
	  send_to_char( "List which clan?\n\r", ch);
	  return;
	}
	else
	  handle = GET_CLAN(ch);
      }
      if ( (handle = ClanNameToHandle( argument )) < 1){
	send_to_char("No such clan.\n\r", ch);
	return;
      }
      argument = one_argument( argument, arg1);
      ListMembers( ch, handle, TRUE, "Senior");
      return;
    }
    /* MORTAL LIST */
    else if ( (handle = GET_CLAN(ch)) < 1){
      send_to_char("You do not belong to a clan.\n\r", ch );
      return;
    }
    else if (!CLAN_ELDER(ch)){
      send_to_char("You lack the privilige.\n\r", ch);
      return;
    }
    ListMembers( ch, handle, FALSE, "Senior");
    return;    
  }
  /* DRAFT */
  else if (!str_prefix( arg1, "draft")){
    CHAR_DATA* victim;

    argument = one_argument( argument, arg2);

    if ( IS_NULLSTR(arg2)){
      send_to_char("Draft whom?\n\r", ch);
      return;
    }
    else if ((victim = get_char_room( ch, NULL, arg2)) == NULL){
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    else if (IS_NPC(victim)){
      send_to_char("Not on mobs.\n\r", ch);
      return;
    }

    /* IMMORTAL/NPC */
    if (IS_IMMORTAL(ch) || (IS_NPC(ch) && get_trust(ch) >= IMPLEMENTOR)){
      if (IS_NULLSTR(argument)){
	if ( (handle = GET_CLAN(ch)) < 1){
	  send_to_char("Draft them into which clan?\n\r", ch);
	  return;
	}
	
      }
      if ( (handle = ClanNameToHandle( argument)) < 1){
	send_to_char("That clan does not exist.\n\r", ch);
	return;
      }
    }
    /* MORTAL */
    else if ( (handle = GET_CLAN(ch)) < 1){
      send_to_char("You do not belong to a clan.\n\r", ch);
      return;
    }
    else if (!CLAN_ELDER(ch)){
      send_to_char("You lack the privilige.\n\r", ch);
      return;
    }
    else if (victim->master != ch && !IS_IMMORTAL(ch)){
      send_to_char("They are not following you.\n\r", ch);
      return;
    }
    if (HAS_CLAN(victim)){
      int cur = GET_CLAN(victim);
      ClanRemMember( cur, victim->name);
      ClanSave( cur );
    }
    if ( (victim->pcdata->clan_handle = ClanAddMember( handle, victim )) < 1){
      send_to_char("Error removing member.\n\r", ch);
      return;
    }
    ClanSave( handle );
    act("$n is now a member of $t.", victim, ClanHandleToName( GET_CLAN(victim)), NULL, TO_ROOM);
    sendf( victim, "You are now a member of %s.\n\r", ClanHandleToName( GET_CLAN(victim)) );
    return;
  }
  /* REMOVE */
  else if (!str_prefix( arg1, "remove")){
    CHAR_DATA* victim;

    argument = one_argument( argument, arg2);

    if ( IS_NULLSTR(arg2)){
      send_to_char("Remove whom?\n\r", ch);
      return;
    }

    /* IMMORTAL/NPC */
    if (IS_IMMORTAL(ch)){
      if (IS_NULLSTR(argument)){
	if ( (handle = GET_CLAN(ch)) < 1){
	  send_to_char("Remove them from which clan?\n\r", ch);
	  return;
	}
      }
      if ( (handle = ClanNameToHandle( argument)) < 1){
	send_to_char("That clan does not exist.\n\r", ch);
	return;
      }
    }
    /* MORTAL */
    else if ( (handle = GET_CLAN(ch)) < 1){
      send_to_char("You do not belong to a clan.\n\r", ch);
      return;
    }
    else if (!CLAN_LEADER(ch)){
      send_to_char("You lack the privilige.\n\r", ch);
      return;
    }
    if (!ClanRemMember( handle, arg2)){
      send_to_char("No such member in the clan.\n\r", ch);
      return;
    }
    if ( (victim = get_char( arg2 )) != NULL && !IS_NPC(victim)){
      victim->pcdata->clan_handle = 0;
      send_to_char("You have been removed from your clan.\n\r", victim);
    }
    ClanSave( handle );
    send_to_char("Removed.\n\r", ch);
    return;
  }
  /* PROMOTE */
  else if (!str_prefix( arg1, "promote")){
    CLAN_MEMBER* pCmd = NULL;

    argument = one_argument( argument, arg2);

    if ( IS_NULLSTR(arg2)){
      send_to_char("Promote whom?\n\r", ch);
      return;
    }

    /* IMMORTAL/NPC */
    if (IS_IMMORTAL(ch)){
      if (IS_NULLSTR(argument)){
	if ( (handle = GET_CLAN(ch)) < 1){
	  send_to_char("Promote them in which clan?\n\r", ch);
	  return;
	}
      }
      else if ( (handle = ClanNameToHandle( argument)) < 1){
	send_to_char("That clan does not exist.\n\r", ch);
	return;
      }
      if ( (pCmd = GetClanMember(handle, arg2)) == NULL){
	send_to_char("No such member in that clan.\n\r", ch);
	return;
      }
    }
    /* MORTAL */
    else {
      if ( (handle = GET_CLAN(ch)) < 1){
	send_to_char("You do not belong to a clan.\n\r", ch);
	return;
      }
      else if (!CLAN_ELDER(ch)){
	send_to_char("You lack the privilige.\n\r", ch);
	return;
      }
      else if ( (pCmd = GetClanMember( handle, arg2)) == NULL){
	send_to_char("They are not part of your clan.\n\r", ch);
	return;
      }
      else if (pCmd->rank + 1 >= ch->pcdata->clan_rank){
	send_to_char("You cannot promote them any further.\n\r", ch);
	return;
      }
    }
    if (pCmd){
      CHAR_DATA* vch = get_char( pCmd->name );
      if (pCmd->rank >= RANK_LEADER){
	send_to_char("They cannot be promoted further.\n\r", ch);
	pCmd->rank = RANK_LEADER;
      }
      else{
	pCmd->rank++;
	sendf(ch, "Promoted to %s.\n\r", clanr_table[pCmd->rank][1]);
      }
      if (vch && !IS_NPC(vch)){
	SynchClanData( vch );
	send_to_char("You have been promoted in within your clan!\n\r", vch);
      }
    }
    ClanSave( handle );
    return;
  }
  /* DEMOTE */
  else if (!str_prefix( arg1, "demote")){
    CLAN_MEMBER* pCmd = NULL;

    argument = one_argument( argument, arg2);

    if ( IS_NULLSTR(arg2)){
      send_to_char("Demote whom?\n\r", ch);
      return;
    }

    /* IMMORTAL/NPC */
    if (IS_IMMORTAL(ch)){
      if (IS_NULLSTR(argument)){
	if ( (handle = GET_CLAN(ch)) < 1){
	  send_to_char("Demote them in which clan?\n\r", ch);
	  return;
	}
      }
      else if ( (handle = ClanNameToHandle( argument)) < 1){
	send_to_char("That clan does not exist.\n\r", ch);
	return;
      }
      if ( (pCmd = GetClanMember(handle, arg2)) == NULL){
	send_to_char("No such member in that clan.\n\r", ch);
	return;
      }
    }
    /* MORTAL */
    else{
      if ( (handle = GET_CLAN(ch)) < 1){
      send_to_char("You do not belong to a clan.\n\r", ch);
      return;
      }
      else if (!CLAN_LEADER(ch)){
	send_to_char("You lack the privilige.\n\r", ch);
	return;
      }
      else if ( (pCmd = GetClanMember( handle, arg2)) == NULL){
	send_to_char("They are not part of your clan.\n\r", ch);
	return;
      }
      else if (pCmd->rank >= ch->pcdata->clan_rank){
	send_to_char("You cannot demote them.\n\r", ch);
	return;
      }
    }
    if (pCmd){
      CHAR_DATA* vch = get_char( pCmd->name );
      if (pCmd->rank <= RANK_NEWBIE){
	send_to_char("They cannot be demoted further.\n\r", ch);
	pCmd->rank = RANK_NEWBIE;
      }
      else{
	pCmd->rank--;
	sendf(ch, "Demoted to %s.\n\r", clanr_table[pCmd->rank][1]);
      }
      if (vch && !IS_NPC(vch)){
	SynchClanData( vch );
	send_to_char("You have been demoted in within your clan!\n\r", vch);
      }
    }
    ClanSave( handle );
    return;
  }
  /* ALLOW */
  else if (!str_prefix( arg1, "allow")){
    CABAL_DATA* pCab;
    CLAN_MEMBER* pCmd = NULL;
    CHAR_DATA* victim;

    argument = one_argument( argument, arg2);

    if (!IS_IMMORTAL(ch)){
      send_to_char("You are not an Immortal.\n\r", ch);
      return;
    }
    else if ( IS_NULLSTR(arg2)){
      send_to_char("Allow whom to apply to their clan's cabal?\n\r", ch);
      return;
    }
    if (IS_NULLSTR(argument)){
      if ( (handle = GET_CLAN(ch)) < 1){
	send_to_char("Allow them in which clan?\n\r", ch);
	return;
      }
    }
    else if ( (handle = ClanNameToHandle( argument)) < 1){
      send_to_char("That clan does not exist.\n\r", ch);
      return;
    }
    //min member check
    for (pCab = cabal_list; pCab; pCab = pCab->next){
      if (pCab->clan == handle && pCab->cur_member >= pCab->max_member){
	send_to_char("The cabal is at max. capacity.\n\r", ch);
	return;
      }
    }
    if ( (pCmd = GetClanMember(handle, arg2)) == NULL){
      send_to_char("No such member in that clan.\n\r", ch);
      return;
    }
    pCmd->allowed = mud_data.current_time;
    if ( (victim = get_char( pCmd->name)) != NULL){
      send_to_char("`@You have seven days to apply to your clan's cabal.``\n\r", victim);
    }
    else{
      DESCRIPTOR_DATA* d;
      if ( (d = bring(ch->in_room, pCmd->name)) != NULL){
	d->character->pcdata->roomnum *= -1;
	SET_BIT(d->character->pcdata->messages, MSG_CABAL_ALLOW);
	purge(d->character );
      }
    }
    send_to_char("They have been allowed to vote for next 7 days.\n\r", ch);
    ClanSave( handle );
    return;
  }
  else
    do_clan( ch, "");
}

//returns clan's handle from name
int GetClanHandle( char* name ){
  return (ClanNameToHandle( name ));
}

//returns name of clan based onhandle
char* GetClanName( int handle ){
  return(ClanHandleToName( handle ));
}

//synchronizes clan and character data
void SynchClanData( CHAR_DATA* ch ){
  int handle;
  CLAN_MEMBER* pCmd;

  if ( (handle = GET_CLAN(ch)) < 1)
    return;
  else if ( (pCmd = GetClanMember( handle, ch->name)) == NULL)
    return;

  //synch
  pCmd->level		= ch->level;
  pCmd->class		= ch->class;
  pCmd->race		= ch->race;
  pCmd->last_logon	= ch->logoff;
  pCmd->pts		= ch->pcdata->clan_points;

  if (ch->pCabal){
    pCmd->rank		= UMAX(ch->pcdata->rank, pCmd->rank);
    ch->pcdata->clan_points = 0;
    pCmd->pts		= 0;
    pCmd->allowed	= 0;
  }
  ch->pcdata->clan_rank = pCmd->rank;

}
	    

//returns member data based on handle and name
CLAN_MEMBER* GetClanMember( int handle, char* name ){
  CLAN_MEMBER* pCmd;
  CLAN_DATA* pClan = GetClanData( handle );

  if (pClan == NULL || IS_NULLSTR(name))
    return NULL;
  for (pCmd = pClan->members.next; pCmd; pCmd = pCmd->next){
    if (!str_cmp(name, pCmd->name))
      return pCmd;
  }
  return NULL;
}

void CharToClan( CHAR_DATA* ch, int handle ){
  if (IS_NPC(ch))
    return;
  ch->pcdata->clan_handle = ClanAddMember( handle, ch);
}

void CharFromClan( CHAR_DATA* ch ){
  int handle;
  if ( (handle = GET_CLAN( ch )) < 1)
    return;
  else if (ClanRemMember( handle, ch->name ))
    ClanSave( handle );
}

void do_brownie( CHAR_DATA* ch, char* argument ){
  CHAR_DATA* victim;
  char name[MIL];
  int range;

  argument = one_argument( argument, name );
  range = atoi( argument );

  if (IS_NULLSTR(argument)){
    send_to_char("Give brownie points to whom and on what scale (-2 to 2)?\n\r", ch);
    return;
  }
  else if ( (victim = get_char( name )) == NULL){
    send_to_char("Character not found.\n\r", ch);
    return;
  }
  else if (IS_NPC(victim)){
    send_to_char("Not on mobs.\n\r", ch);
    return;
  }
  else if (range == 0){
    sendf( ch, "%s has %d points.  Use scale of -2 to 2 to remove or add points.\n\r", PERS2(victim), victim->pcdata->clan_points);
    return;
  }
  else if (range < -2 || range > 2){
    send_to_char("Use a scale of -2 to 52for the amount of reward.\n\r", ch);
    return;
  }

  switch (range){
  case -2:  victim->pcdata->clan_points -= 100;	break;
  case -1:  victim->pcdata->clan_points -= 50;	break;
  case  2:  victim->pcdata->clan_points += 100;	break;
  case  1:  victim->pcdata->clan_points += 50;	break;
  default:	break;
  }
  SynchClanData( victim );
  sendf( ch, "%s's clan points now at %d\n\r", PERS2(victim), victim->pcdata->clan_points);

  save_char_obj( victim );
  ClanSave( GET_CLAN(victim ));
}

int ClanSecondsToApply( CHAR_DATA* ch ){
  int handle = GET_CLAN(ch);
  CLAN_MEMBER* pCmd;

  if (handle < 1){
    return 0;
  }
  else if ( (pCmd = GetClanMember( handle, ch->name)) == NULL){
    return 0;
  }
  else if (pCmd->allowed < 1){
    return 0;
  }
  else if (mud_data.current_time - pCmd->allowed > CLAN_APPLY_WIN){
    return 0;
  }
  else
    return (CLAN_APPLY_WIN - (mud_data.current_time - pCmd->allowed));
}

bool ClanApplicationCheck( CHAR_DATA* ch, CABAL_DATA* pCab){
  int handle = GET_CLAN(ch);
  CLAN_MEMBER* pCmd;

  if (handle < 1){
    sendf(ch, "You cannot join %s beacuse you're a member of %s.\n\r", pCab->name, GetClanName(pCab->clan));
    return FALSE;
  }
  else if ( (pCmd = GetClanMember( handle, ch->name)) == NULL){
    sendf(ch, "You cannot join %s beacuse you're a member of %s.\n\r", pCab->name, GetClanName(pCab->clan));
    return FALSE;
  }
  else if (pCmd->allowed < 1){
    sendf(ch, "%s has not allowed you to apply to %s.\n\r", pCab->immortal, pCab->name );
    return FALSE;
  }
  else if (mud_data.current_time - pCmd->allowed > CLAN_APPLY_WIN){
    sendf(ch, "You have taken over seven days to apply.  %s must allow you to do so once more.\n\r", pCab->immortal);
    return FALSE;
  }
  return TRUE;
}

void ClanEcho( int handle, char* string ){
  DESCRIPTOR_DATA* d;

  for ( d = descriptor_list; d != NULL; d = d->next ){
    if ( d->connected == CON_PLAYING 
	 && !IS_AFFECTED2(d->character, AFF_SILENCE)
	 && !IS_NPC(d->character)
	 && !is_affected(d->character,gsn_silence)
	 && GET_CLAN(d->character) == handle
	 ){
      sendf(d->character,"[`8%s``]: '`0%s``'\n\r", GetClanName(handle), string);
    }
  }
}

bool ClanRename(int handle, char* current, char* new ){
  CLAN_MEMBER* cm;
    
  if (handle < 1 || IS_NULLSTR(current) || IS_NULLSTR(new))
    return FALSE;

  if ( (cm = GetClanMember(handle, current)) == NULL)
    return FALSE;
  
  free_string(cm->name);
  cm->name = str_dup (capitalize (new));

  return TRUE;
}
