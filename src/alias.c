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
#include <ctype.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "merc.h"
#include "recycle.h"
#include "interp.h"
#include "alias.h"

/***************************************************************/
/*Following are alias orientanted routines for Forsaken Lands  */
/*mud created by Virigoth circa November 2002.  Copyrighted for Forsaken*/
/*Lands mud November 2, 2002.  Do not use or copy without explicit */
/*permission of author. (Voytek Plawny aka Virigoth)	       */
/***************************************************************/
static int loop_count = 0;

ALIAS_DATA* alias_free;
ALIAS_DATA* new_alias(){
  static ALIAS_DATA al_zero;
  ALIAS_DATA* al;

  if (alias_free == NULL){
    al = alloc_perm(sizeof(*al));
  }
  else{
    al = alias_free;
    alias_free = alias_free->next;
    alias_free->prev = NULL;
  }
  *al = al_zero;
  al->next = al->prev = NULL;
  return al;
}

void free_alias( ALIAS_DATA* al ){
  if (al->name != NULL)
    free_string(al->name);
  if (al->content != NULL)
    free_string(al->content);

  al->next = alias_free;
  al->prev = NULL;
}

/* creates an alias with given name and content */
ALIAS_DATA* create_alias(char* name, char* content){
  ALIAS_DATA* al;

  if ( (al = new_alias()) == NULL){
    bug("create_alias: Cound not create an alias", 0);
    return NULL;
  }
  
  al->name = str_dup( name );
  al->content = str_dup( content );

  return al;
}

/* adds an alias to character */
void add_alias( CHAR_DATA* ch, ALIAS_DATA* newal ){
  ALIAS_DATA* al;

  if (IS_NPC(ch))
    return;

  /* easy case if no aliases */
  if (ch->pcdata->aliases == NULL){
    ch->pcdata->aliases = newal;
    newal->next = newal;
    newal->prev = newal;
    return;
  }
  /* we insert aliases in sorted order by first letter to save on access */
  for (al = ch->pcdata->aliases; al && al->next != ch->pcdata->aliases; al = al->next){
    if (tolower(newal->name[0]) < tolower(al->name[0]))
      break;
  }

  /* we are now somewhere between first and last alias */
  /* insert */

  if (tolower(newal->name[0]) >= tolower(al->name[0])){
    /* add 'behind' al */
    newal->next = al->next;
    al->next = newal;
    newal->prev = al;
    newal->next->prev = newal;
  }
  else{
    /* add 'before' al */
    newal->next = al;
    newal->prev = al->prev;
    al->prev->next = newal;
    al->prev = newal;
    if (ch->pcdata->aliases == al)
      ch->pcdata->aliases = newal;

  }
}
  
/* unlinkes specific alias from char */
bool rem_alias( PC_DATA* pcdata, ALIAS_DATA* newal ){
  ALIAS_DATA* al;

  if (pcdata == NULL || pcdata->aliases == NULL){
    bug("rem_alias: alias not found.", 0);
    return FALSE;
  }
  for (al = pcdata->aliases; al; al = al->next){
    if (al == newal)
      break;
    else if ( al->next == pcdata->aliases)
      break;
  }
  if (al != newal){
    bug("rem_alias: alias not found.", 0);
    return FALSE;
  }

  (al->next)->prev = al->prev;
  (al->prev)->next = al->next;
  if (pcdata->aliases == al){
    if (al->next == al)
      pcdata->aliases = NULL;
    else
      pcdata->aliases = al->next;
  }

  return TRUE;
}

/* gets an alias with given name from char */
ALIAS_DATA* get_alias( CHAR_DATA* ch, char* name){
  ALIAS_DATA* al;

  if (IS_NPC(ch) || ch->pcdata->aliases == NULL)
    return NULL;

  for (al = ch->pcdata->aliases; al; al = al->next){
    if (tolower(name[0]) < tolower(al->name[0]))
      return NULL;
    else if (tolower(name[0]) == tolower(al->name[0]) && !strcmp(name, al->name))
      return al;
    else if( al->next == ch->pcdata->aliases)
      break;
  }
  return NULL;
}

/* adds or changes an alias on character */
ALIAS_DATA* alias_to_char( CHAR_DATA* ch, char* name, char* content){
  ALIAS_DATA* al;

  if (IS_NPC(ch))
    return NULL;

  if ( (al = get_alias(ch, name)) != NULL){
    if (al->content != NULL)
      free_string(al->content);
    al->content = str_dup(content);
    return al;
  }
  else if ( (al = create_alias(name, content)) == NULL){
    bug("alias_to_char: error creating alias.", 0);
    return NULL;
  }
  else{
    add_alias(ch, al );
    return al;
  }
}
    
/* removes alias from char */
ALIAS_DATA* alias_from_char( CHAR_DATA* ch, char* name ){
  ALIAS_DATA* al;

  if (IS_NPC(ch))
    return NULL;

  if ( (al = get_alias( ch, name)) == NULL)
    return NULL;

  rem_alias( ch->pcdata, al );
  return al;
}


/* manages character's aliases 
   alias <key> <content>	//adds
   alias rem   <key>		//rems
   alias list			//lists
*/
void do_alias( CHAR_DATA *ch, char *argument ){
  char arg1[MIL];
  
  argument = one_argument( argument, arg1);

  
  if ( IS_NPC(ch) )
    {
      send_to_char( "Why would you set an alias for a mob?\n\r",ch);
      return;
    }
  else if (!IS_NULLSTR(arg1) && !str_prefix(arg1, "rem")){
    ALIAS_DATA* al;
    int pos = atoi( argument );
    int count = 0;

    if (IS_NULLSTR(argument)){
      send_to_char("Syntax: alias <alias> <commands> (\"help alias\" for other commands)\n\r", ch);
      return;
    }
    if ( pos > 0){
      for (al = ch->pcdata->aliases;al; al = al->next){
	if (++count == pos || al->next == ch->pcdata->aliases)
	  break;
      }
      if (pos != count){
	send_to_char("No alias with that position.\n\r", ch);
	return;
      }
      else
	alias_from_char(ch, al->name);
    }
    else if ( (al = alias_from_char(ch, argument)) == NULL){
      send_to_char("No such alias, use \"alias list\" for a list.\n\r", ch);
      return;
    }
    free_alias( al );
    send_to_char("Alias removed.\n\r", ch);
  }
  else if (!str_prefix(arg1, "list") || !str_prefix(arg1, "show")){
    ALIAS_DATA* al;
    int count = 0;
    send_to_char( "\"help alias\" for info on commands.\n\r",ch);
    if (ch->pcdata->aliases == NULL){
      send_to_char("No aliases defined.\n\r", ch);
      return;
    }
    sendf(ch, "Alias                     Commands            \n\r");
    sendf(ch,"-----------------------------------------------\n\r");
    for (al = ch->pcdata->aliases; al ; al = al->next){
      sendf(ch, "%-2d. %-15s= %s\n\r", ++count, al->name, al->content);
      if (al->next == ch->pcdata->aliases)
	break;
    }
  }
  else if ( IS_NULLSTR(argument)){
    send_to_char( "Syntax: alias <alias> <commands>\n\r",ch);
    return;
  }
  else{
    ALIAS_DATA* al, *old_al = NULL;
    char content[MIL];
    int pos = atoi( arg1 );
    int count = 0;
    const int MAX_ALIAS = 32;

    /* quick check for maximum aliases */
    for (al = ch->pcdata->aliases; al ; al = al->next){
      count++;
      if (al->next == ch->pcdata->aliases)
	break;
    }
    if (count > MAX_ALIAS){
      sendf(ch, "The alias system is still fairly new and being tested.\n\r"\
	    "Maximum of %d aliases allowed for now.\n\r", MAX_ALIAS);
      return;
    }
    if ( pos > 0){
      for (al = ch->pcdata->aliases; al ; al = al->next){
	if (++count == pos || al->next == ch->pcdata->aliases)
	  break;
      }
      if (pos != count){
	send_to_char("No alias with that position.\n\r", ch);
	return;
      }
      else
	old_al = al;
    }
    strncpy(content, argument, MIL);
    content[MIL - 1] = '\0';

    if ( (al = alias_to_char( ch, old_al ? old_al->name : arg1, content)) == NULL){
      send_to_char("Error adding alias.\n\r", ch);
      return;
    }
    else{
      sendf(ch, "Alias added: %s = '%s'\n\r", al->name, al->content);
      return;
    }
  }
}


/* stuffs commands based on alias, ";" used as seperators */
char* parse_alias( CHAR_DATA* ch, ALIAS_DATA* al, char* arguments ){
  static char cmd[4 * MIL];
  char* ptr, *pcmd, *pfirst;
  int start = strlen(ch->desc->inbuf);
  int semi_count = 0;	//counts times a semi was used
  int arg_count = 0;	//counts number of argument symbols
  int skip = 0;		//used in parse loop to skip x spaces when inserting

  cmd[0] = 0;
  ptr = al->content;
  pcmd = &cmd[0];
  pfirst = pcmd;

  /* parse through the content:
     "%" quote next char=> copy and skip one character
     ";" means newline	=> '\n'
     "$" means arguments=> arguments
  */
  /* safety */
  while (*ptr != '\0'){
    /* select what to do based on character */
    switch( *ptr ){
    case '%':	*pcmd = *(++ptr);	skip = 1;			break;
    case ';':	*pcmd = '\n';		skip = 1;
		if (++semi_count < 2)	pfirst = pcmd + 1;		break;
    case '$':	strcpy(pcmd, arguments);skip = strlen(arguments);	
		arg_count++;						break;
    default:	*pcmd = *ptr;		skip = 1;			break;
    }
    /* skip if replacement was more then 1 char */
    while (skip-- > 0){
      pcmd++;
    }
    ptr++;
  }
  /* if there were no semi colons, we append arguments automaticly */
  if (semi_count < 1 && arg_count < 1 && !IS_NULLSTR(arguments)){
    char buf[2 * MIL];
    *pcmd = 0;
    sprintf(buf, " %s", arguments);
    strcat(cmd, buf);
    pcmd += strlen( buf );
  }

  //add a return onto the list of aliases
  *pcmd++ = '\n';

  if (pfirst == cmd)
    pfirst = pcmd;
  
  /* intput safety */
  if (start + strlen( pfirst )  > sizeof(ch->desc->inbuf) - 10){
    send_to_char("Input overflow! Alias aborted.\n\r", ch);
    return NULL;
  }

  /* copy current commands into spot behind alias commands */
  memmove( &ch->desc->inbuf[pcmd - pfirst], ch->desc->inbuf, start);

  /* stuff the alias commands into front of the old commands */
  memcpy(ch->desc->inbuf, pfirst, (pcmd - pfirst));

  /* terminate the string */
  ch->desc->inbuf[start + pcmd - pfirst] = 0;

  /* DEBUG
     sendf(ch, "CONTENTS:\n\r%s", ch->desc->inbuf );
  */

  //stuff a terminator at pfirst point and return a single first command
  *(pfirst - 1) = 0;
  return cmd;
}

/* checks aliases for give command */
char* check_alias(CHAR_DATA* ch, char* command, char* arguments){
  ALIAS_DATA* al;

  if (loop_count++ > 1)
    return NULL;
  else if (IS_NPC(ch) || ch->pcdata->aliases == NULL)
    return NULL;
  else if (!str_cmp(command, "alias"))
    return NULL;
  else if ( (al = get_alias(ch, command)) != NULL){
    return (parse_alias( ch, al, arguments ));
  }
  else
    return NULL;
}

void AliasClearSafety(){
  loop_count = 0;
}
