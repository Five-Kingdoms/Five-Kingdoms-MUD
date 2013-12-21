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

/* Written by Virigoth sometime circa april 2000 for Age of Mirlan mud.*/
/* This is the implementation of the selectable skills code		*/
/* NOT TO BE USED OR REPLICATED WITHOUT EXPLICIT PERMISSION OF AUTHOR	*/
/* umplawny@cc.umanitoba.ca						*/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "tome.h"
#include "interp.h"
#include "recycle.h"
#include "cabal.h"
#include "vote.h"
#include "save_mud.h"




//topic struct
struct tome_topic_s {
  char		subject[MIL];		//name of topic
  int		topicSize;		//number of entries in this topic

  TOME_DATA*	tomes;			//list of entries in this topic
  TOME_DATA*	last;			//ptr to last tome in list
};


//LOCAL GLOBALS
static TOME_TOPIC	topic_list[MAX_TOPIC];
static int		lastTopic;		//number of topics used
static int		maxTome;		//number of tomes


//PRIVATE FUNCTIONS



//gives handle to topic, 0 on fail
int TopicHandle( char* subject ){
  int i = 0;

  if (IS_NULLSTR(subject))
    return i;

  for (i = 0; i < MAX_TOPIC; i++){
    if (LOWER(subject[0]) != LOWER(topic_list[i].subject[0]))
      continue;
    else if (!strcasecmp(subject, topic_list[i].subject))
      return (i + 1);
  }
  return (0);
}

//gives pointer to topic from handle
TOME_TOPIC* HandleToTopic( int handle ){
  if (handle < 1)
    return NULL;
  else if (topic_list[handle - 1].topicSize < 1)
    return NULL;
  else
    return (&topic_list[handle - 1]);
}

//gives handle based on pointer
int TopicToHandle( TOME_TOPIC* topic ){
  if (topic == NULL)
    return 0;
  else
    return ((topic - topic_list) / sizeof( TOME_TOPIC ) + 1);
}

//Allocates tome with intent TO FREE IT LATER
//Use AddTome for adding tomes to global list
TOME_DATA* NewTome(){
  TOME_DATA* tome = (TOME_DATA*) malloc(sizeof(TOME_DATA));

  if (tome == NULL){
    bug("tome.c:NewTome> malloc returned NULL", 0);
    return NULL;
  }

  //reset the tome data
  memset(tome, 0, sizeof(TOME_DATA));
  tome->canFree = TRUE;

  //return pointer
  return (tome);
}

//Free a tome allocated with NewTome (not with alloc_perm)
void FreeTome( TOME_DATA* tome ){
  if (tome == NULL)
    return;

  //free common things between perm/malloc tomes
  if (!IS_NULLSTR(tome->text))
    free_string(tome->text);

  //Beyond this only malloc tomes
  if (!tome->canFree){
    return;
  }
  //free the mallocated data
  free( tome );
}

//Adds a new tome with given author, title, and text under given subject
TOME_DATA* AddTome(char* subject, char* title, char* author, char* text){
  int handle = TopicHandle( subject );
  TOME_TOPIC* topic = HandleToTopic( handle );
  TOME_DATA* tome;

  if (topic == NULL){
    //try to add a new topic
    if (lastTopic >= MAX_TOPIC){
      bug("tome.c:AddTome> Cannot add topic, MAX_TOPIC of %d reached.", lastTopic);
      return NULL;
    }
    else
      topic = &topic_list[lastTopic++];
  }

  //add the new tome and try to attatch it to topic
  tome = (TOME_DATA*) alloc_perm(sizeof(TOME_DATA));

  if (tome == NULL){
    bug("tome.c:AddTome> alloc_perm returned NULL", 0);
    return NULL;
  }

  //reset the tome data
  memset(tome, 0, sizeof(TOME_DATA));

  if (topic->tomes == NULL){
    topic->last = tome;
    topic->tomes = tome;
  }
  else{
    topic->last->next = tome;
    topic->last = tome;
  }
  tome->topic = topic;
  topic->topicSize++;

  //copy stuff over
  strcpy(tome->title, title);
  strcpy(tome->author, author);
  strcpy(topic->subject, subject);
  tome->text = str_dup(text);

  return (tome);
}

//composes list of subjects with number of tomes in each
char* GetSubjectList(){
  static char list[MSL];
  char buf[4 * MIL];
  int i = 0;

  list[0] = 0;

  for (i = 0; i < lastTopic; i++){
    sprintf( buf, "%2d. %-28s [%2d]",
	     i + 1,
	     topic_list[i].subject,
	     topic_list[i].topicSize);
    //do 2 per line
    if (i % 2 == 0)
      strcat(buf, " " );
    else
      strcat(buf, "\n\r" );

    //add onto the list
    strcat(list, buf );
  }
  if (IS_NULLSTR(list)){
    sprintf(list, "None.\n\r");
  }
  else if (i % 2 != 0)
    strcat(list, "\n\r");
  return (list);
}

//composes list of titles in the subject
char* GetTitleList(char* subject){
  static char list[MSL];
  char buf[4 * MIL];
  TOME_TOPIC* topic;
  TOME_DATA* tome;
  int i = 0;

  list[0] = 0;

  if ( (topic = HandleToTopic(TopicHandle(subject))) != NULL){
    tome = topic->tomes;
    for (; tome; tome = tome->next, i++){
      sprintf( buf, "%2d. %-28s",
	       i + 1,
	       tome->title);
      //do 2 per line
      if (i % 2 == 0)
	strcat(buf, " " );
      else
	strcat(buf, "\n\r" );

      //add onto the list
      strcat(list, buf );
    }
  }
  if (IS_NULLSTR(list)){
    sprintf(list, "None.\n\r");
  }
  else if (i % 2 != 0)
    strcat(list, "\n\r");

  return (list);
}


//echos avaliability of a new tome
void EchoTome( TOME_DATA* newtome ){
  DESCRIPTOR_DATA *d;
  char buf[MIL];

  sprintf(buf, "A scribe hands you a note: <%s> %s is now avaliable in The Library.\n\r",
	  newtome->topic->subject, newtome->title);

  for ( d = descriptor_list; d; d = d->next ){
    if ( d->connected == CON_PLAYING )
      send_to_char( buf, d->character );
  }

  //Create a note on the subject
  sprintf(buf, warn_table[WARN_TOME].subj, newtome->topic->subject, newtome->title);
  do_hal("all", buf, warn_table[WARN_TOME].text, NOTE_NEWS);
}

//creates a new tome into pcdata for editing
void reset_tome( CHAR_DATA* ch ){
  ch->pcdata->ptome = NewTome();
  if (ch->pcdata->ptome == NULL)
    send_to_char("Error!\n\r", ch);
  else{
    //set author
    sprintf(ch->pcdata->ptome->author, "%s%s%s%s%s",
	    ch->pCabal ? get_crank(ch) : "",
	    ch->pCabal ? " " : "",
	    ch->name,
	    IS_NULLSTR(ch->pcdata->family)? "" : " ",
	    IS_NULLSTR(ch->pcdata->family)? "" : ch->pcdata->family);
    send_to_char("New Tome begun.\n\r", ch);
  }
}


//Saves a single tome
void WriteTome( FILE* fp, TOME_DATA* tome ){
  fprintf( fp, "%s~\n%s~\n%s~\n\n",
	   tome->author,
	   tome->title,
	   tome->text);
}

//reads a single tome
void ReadTome( FILE* fp, TOME_DATA* tome ){
  char* buf;

  buf = fread_string( fp );
  strcpy( tome->author, buf );
  free_string( buf );

  buf = fread_string( fp );
  strcpy( tome->title, buf );
  free_string( buf );

  tome->text = fread_string( fp );
}

//Saves a single topic
void WriteTopic( FILE* fp, TOME_TOPIC* topic ){
  TOME_DATA* tome = topic->tomes;
  int i = 0;

  fprintf(fp, "%s~ %d\n", topic->subject, topic->topicSize);

  for (; i < topic->topicSize; i++, tome = tome->next){
    WriteTome( fp, tome );
  }
}

//Reads a single topic
void ReadTopic( FILE* fp ){
  TOME_DATA tome;
  int i = 0, topicSize;
  char* buf;

  buf = fread_string( fp );
  topicSize = fread_number( fp );
  for (i = 0; i < topicSize; i++){
    ReadTome( fp, &tome );
    AddTome( buf, tome.title, tome.author, tome.text );
    free_string(tome.text);
  }
  free_string( buf );
}

//Gets tome based on number of topic/tome
TOME_DATA* GetTome  (int sub, int tom ){
  TOME_TOPIC* topic;
  TOME_DATA* tome;
  int i = 1;

  if (sub < 1 || tom < 1)
    return NULL;

  topic = HandleToTopic( sub );
  if (topic == NULL){
    return NULL;
  }
  else
    tome = topic->tomes;

  //find the tom'th topic in topic
  for (tome = topic->tomes; tome && i < tom && i < topic->topicSize; tome = tome->next, i++);

  return tome;
}

//INTERFACE FUNCTIONS

//Reads the library
void LoadTomes(){
  FILE* fp;
  char path[MIL];
  int i= 0;
  int lastTopic = 0;

  fclose( fpReserve );
  sprintf(path, "..%s", TOME_FILE);

  //try to open file for write
  if ( ( fp = fopen( path, "r" ) ) == NULL ){
    perror( path );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }

  //size of library
  lastTopic = fread_number( fp );

  //run through topics reading
  for (i = 0; i < lastTopic; i++){
    ReadTopic( fp );
  }
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
}


//Saves the library
void SaveTomes(){
  FILE* fp;
  char path[MIL];
  int i= 0;
  int size = 0;

  fclose( fpReserve );
  sprintf(path, "..%s", TOME_TEMP_FILE);

  //try to open file for write
  if ( ( fp = fopen( path, "w" ) ) == NULL ){
    perror( path );
    fclose (fp);
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }

  //size of library
  //run through topics counting
  for (i = 0; i < lastTopic; i++){
    if (topic_list[i].topicSize < 1)
      continue;
    else
      size++;
  }
  fprintf( fp, "%d\n", size);

  //run through topics writting
  for (i = 0; i < lastTopic; i++){
    if (topic_list[i].topicSize < 1)
      continue;
    else
      WriteTopic( fp, &topic_list[i] );
  }
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );

  //copy the file over
  rename(TOME_TEMP_FILE, TOME_FILE);
}

//resets tome data for further loading/use
void InitTomes(){

  //zero the topic array
  memset( &topic_list, 0, MAX_TOPIC * sizeof(TOME_TOPIC));

  //zero the statistics
  lastTopic = 0;
  maxTome = 0;

  //load saved data
  LoadTomes();
}
void do_read_tome( CHAR_DATA *ch, char *argument ){
  char arg[MIL];
  char buf[MIL];
  BUFFER* output;
  argument = one_argument(argument, arg );

  if (IS_NULLSTR(arg)){
    output = new_buf();
    sprintf(buf, "The Library of -Mirlan- has tomes in following subjects:\n\r");
    add_buf(output, buf );
    add_buf(output, GetSubjectList());

    sprintf(buf, "Use \"tome <subject #>\" for list of tomes in given subject.\n\r");
    add_buf(output, buf );
    page_to_char(buf_string(output), ch);
    free_buf( output );
  }
  else{
    TOME_TOPIC* topic;
    int sub = 0;
    int tom = 0;

    sub = atoi( arg );
    tom = atoi( argument );

    topic = HandleToTopic( sub );
    if (topic == NULL){
      sendf(ch, "Topic number %d is not avaliable.\n\r", sub );
      return;
    }

    //LIST TOMES IN TOPICS
    if (IS_NULLSTR(argument)){
      output = new_buf();
      sprintf(buf, "Following Tomes with subject \"%s\" are avaliable:\n\r", topic->subject);
      add_buf(output, buf );
      add_buf(output, GetTitleList( topic->subject ));

      sprintf(buf, "Use \"tome <subject #> <tome #>\" to read.\n\r");
      add_buf(output, buf );
      page_to_char(buf_string(output), ch);
      free_buf( output );
    }
    //SHOW TOME (HERALDS ANYWHERE, REST ONLY IN HERALD/LIBRARY)
    else{
      TOME_DATA* tome;

      /* CHECK IF WE CAN READ THIS HERE */
      if (!IS_SET(ch->in_room->room_flags2, ROOM_LIBRARY)
	  && (ch->in_room->pCabal == NULL
	      || !IS_CABAL(ch->in_room->pCabal, CABAL_HISTORY))
	  && (ch->pCabal == NULL
	      || !IS_CABAL(ch->pCabal, CABAL_HISTORY))){
	send_to_char("You may only read tomes in library, or Herald Cabal.\n\r", ch);
	return;
      }
      if ( (tome = GetTome(sub, tom)) == NULL){
	sendf(ch, "That selection seems to be unavailable.\n\r");
	return;
      }
      output = new_buf();
      sprintf(buf, "[%s] : ", tome->topic->subject );
      add_buf(output, buf );

      if (IS_NULLSTR(tome->title))
	sprintf(buf, "%s by ", "TITLE" );
      else
	sprintf(buf, "%s by ", tome->title );
      add_buf(output, buf );

      sprintf(buf, "%s\n\r", tome->author );
      add_buf(output, buf );

      if (!IS_NULLSTR(tome->text))
	add_buf(output, tome->text );

      page_to_char(buf_string(output), ch);
      free_buf( output );
      return;
    }
  }
}

void do_write_tome( CHAR_DATA *ch, char *argument ){
  char arg[MIL];

  if (ch == NULL)
    return;
  else if (IS_NPC(ch)
	   || (!IS_IMMORTAL(ch)
	       && get_skill(ch, skill_lookup("scribe")) < 2)
	   ){
    send_to_char("You may not scribe Tomes.\n\r", ch);
    return;
  }

  //get first command
  argument = one_argument( argument, arg );

/* NOTHING */
  if (IS_NULLSTR(arg)){
    BUFFER* output;
    char buf[MIL];
    TOME_DATA* tome = ch->pcdata->ptome;
    if (tome == NULL){
      send_to_char("scribe <new/clear/title/subject>\n\r", ch);
      return;
    }
    output = new_buf();
    sprintf(buf, "Tome: " );
    add_buf(output, buf );

    if (IS_NULLSTR(tome->title))
      sprintf(buf, "%s by ", "TITLE" );
    else
      sprintf(buf, "%s by ", tome->title );
    add_buf(output, buf );

    sprintf(buf, "%s\n\r", tome->author );
    add_buf(output, buf );

    if (!IS_NULLSTR(tome->text))
      add_buf(output, tome->text );

    page_to_char(buf_string(output), ch);
    free_buf( output );
    return;
  }
/* CREATE */
  else if (!str_prefix(arg, "new") || !str_prefix(arg, "create")){
    if (ch->pcdata->ptome != NULL){
      send_to_char("You are already working on a tome!\n\r", ch);
      return;
    }
    reset_tome( ch );
    return;
  }
/* DELETE */
  else if (!str_prefix(arg, "delete")){
    TOME_TOPIC* topic;
    TOME_DATA* tome = NULL, *prev;
    int sub = 0;
    int tom = 0;

    argument = one_argument(argument, arg );

    if (get_trust(ch) < CREATOR){
      send_to_char("Creator and above only.\n\r", ch);
      return;
    }
    else if (IS_NULLSTR(argument)){
      send_to_char("delete <subject #> <tome #>\n\r", ch);
      return;
    }
    sub = atoi( arg );
    tom = atoi( argument );

    topic = HandleToTopic( sub );
    if (topic == NULL){
      sendf(ch, "Topic number %d is not avaliable.\n\r", sub );
      return;
    }
    if ( (tome = GetTome(sub, tom)) == NULL){
      sendf(ch, "That selection seems to be unavailable.\n\r");
      return;
    }

    //find previous
    prev = tome->topic->tomes;
    if (prev == tome){
      tome->topic->tomes = prev->next;
      if (tome->topic->last == prev)
	tome->topic->last = prev->next;
      tome->next = NULL;
    }
    else{
      while (prev && prev->next != tome ){
	prev = prev->next;
      }
      if (prev == NULL){
	send_to_char("Error, should have found the tome but didn't.\n\r", ch);
	return;
      }
      if (tome->topic->last == tome)
	tome->topic->last = prev;
      prev->next = prev->next->next;
    }
    tome->topic->topicSize--;
    FreeTome( tome );
    send_to_char("Deleted.\n\r", ch);
    return;
  }
/* EDIT */
  else if (!str_prefix(arg, "edit")){
    TOME_TOPIC* topic;
    TOME_DATA* tome = NULL;
    int sub = 0;
    int tom = 0;

    argument = one_argument(argument, arg );

    if (get_trust(ch) < CREATOR){
      send_to_char("Creator and above only.\n\r", ch);
      return;
    }
    else if (IS_NULLSTR(argument)){
      send_to_char("edit <subject #> <tome #>\n\r", ch);
      return;
    }
    sub = atoi( arg );
    tom = atoi( argument );

    topic = HandleToTopic( sub );
    if (topic == NULL){
      sendf(ch, "Topic number %d is not avaliable.\n\r", sub );
      return;
    }
    if ( (tome = GetTome(sub, tom)) == NULL){
      sendf(ch, "That selection seems to be unavailable.\n\r");
      return;
    }
    string_append(ch, &tome->text);
  }
/* CLEAR */
  else if (!str_prefix(arg, "clear") || !str_prefix(arg, "delete")){
    if (ch->pcdata->ptome == NULL){
      send_to_char("You are not working on a tome!\n\r", ch);
      return;
    }
    FreeTome(ch->pcdata->ptome);
    ch->pcdata->ptome = NULL;
    send_to_char("Ok.\n\r", ch);
    return;
  }
/* TITLE */
  else if (!str_prefix(arg, "title")){
    if (ch->pcdata->ptome == NULL){
      reset_tome( ch );
      if (ch->pcdata->ptome == NULL)
	return;
    }
    else if (IS_NULLSTR(argument)){
      send_to_char("What will be the title of the tome?\n\r", ch);
      return;
    }
    else if (strlen(argument) > 28){
      send_to_char("The title must be under 28 characters long.\n\r", ch);
      return;
    }
    strcpy(ch->pcdata->ptome->title, argument);
    send_to_char("Ok.\n\r", ch);
  }
  /* TEXT */
  else if (!str_prefix(arg, "text")){
    if (ch->pcdata->ptome == NULL){
      reset_tome( ch );
      if (ch->pcdata->ptome == NULL)
	return;
    }
    send_to_char("Enter the contents of the tome.\n\r", ch);
    string_append(ch, &ch->pcdata->ptome->text);
    return;
  }
/* SEND/POST/SUBJECT */
  else if (!str_prefix(arg, "send")
	   || !str_prefix(arg, "post")
	   || !str_prefix(arg, "print")
	   || !str_prefix(arg, "subject")
	   ){
    TOME_DATA* tome, *newtome;
    TOME_TOPIC* topic;
    char subject[MIL];

    if ((tome = ch->pcdata->ptome) == NULL){
      send_to_char("You are not working on a tome!\n\r", ch);
      return;
    }
    else if (IS_NULLSTR(argument)){
      send_to_char("Print the Tome under what subject?\n\r", ch);
      return;
    }
    else if (IS_NULLSTR(tome->title)){
      send_to_char("You lack the title.\n\r", ch);
      return;
    }
    else if (IS_NULLSTR(tome->text)){
      send_to_char("You lack the body.\n\r", ch);
      return;
    }
    //Convert numeric subject if required
    if ( (topic = HandleToTopic(atoi(argument))) != NULL)
      strcpy(subject, topic->subject);
    else
      strcpy(subject, argument);

    /* Immortals avoid the vote */
    if (IS_IMMORTAL(ch)){
      if ( (newtome = AddTome(subject, tome->title, tome->author, tome->text)) == NULL){
	send_to_char("Error adding the tome!\n\r", ch);
	return;
      }
      EchoTome( newtome );
    }
    else if (ch->pCabal == NULL){
      send_to_char("You must be part of a cabal to scribe a tome.\n\r", ch);
      return;
    }
    else{
      const int cost = 150;
      CABAL_DATA* pc = get_parent( ch->pCabal );
      char buf[3 * MIL];
      /*  cost first */

      if ( cost > GET_CP( ch)){
	sendf( ch, "You will need at least %d %s%s to scribe this tome.\n\r",
	       cost,
	       ch->pCabal->currency, (cost) == 1 ? "" : "s");
	return;
      }
      /* cabal coffers are not affected by urgency */
      else if (mud_data.mudport != TEST_PORT
	       && (pc == NULL || cost > GET_CAB_CP( pc )) ){
	sendf( ch, "[%s] will require at least %d %s%s in its coffers to scribe this tome.\n\r",
	       pc->who_name,
	       cost,
	       pc->currency, (cost) == 1 ? "" : "s" );
	return;
      }
      else{
	VOTE_DATA* pVote;
	  /* ready to go */
	  //merge subject/title to be seperated later using : as seperator
	  sprintf(buf, "%s:%s", subject, tome->title);

	pVote = create_vote(ch, ch->name, buf, tome->text, VOTE_TOME, ch->pCabal->vnum, 0, 0, 0, 0);
	if (pVote == NULL){
	  bug("do_write_tome: send: error creating vote.", 0);
	  send_to_char("Error!\n\r", ch);
	  return;
	}
	/* check urgent flag */
	add_vote( pVote );
	save_mud();
	//subtract the cost
	CP_GAIN( ch, -cost, TRUE );
	CPS_CAB_GAIN( pc, -(cost * CPTS));
	send_to_char("Ok.\n\r", ch);
      }
    }
    FreeTome(tome);
    ch->pcdata->ptome = NULL;
    SaveTomes();
    return;
  }
  else
    send_to_char("scribe <new/clear/title/send>\n\r", ch);
}

void do_print( CHAR_DATA* ch, char* argument ){
  OBJ_DATA* obj;
  TOME_DATA* tome = NULL;
  CHAR_DATA* mob;
  char arg[MIL];
  char buf[MIL];
  int sub = 0;
  int tom = 0;

  if (!IS_SET(ch->in_room->room_flags2, ROOM_LIBRARY)){
    send_to_char("You may request for Tomes to be printed only in the library.\n\r", ch);
    return;
  }
  //Look for a mob
  for (mob = ch->in_room->people; mob; mob = mob->next_in_room){
    if (IS_NPC(mob) && IS_SET(mob->act, ACT_NONCOMBAT)
	&& mob->leader == NULL
	&& mob->master == NULL){
      break;
    }
  }
  if (mob == NULL){
    send_to_char("There is no one here to take your order.\n\r", ch);
    return;
  }
  argument = one_argument(argument, arg);

  if (!can_see(mob, ch)){
    do_say(mob, "I don't deal with people I can't see.");
    return;
  }
  if (IS_NULLSTR(arg)){
    do_say(mob, "I'm sorry, print which subject and tome number?");
    return;
  }
  sub = atoi( arg );
  tom = atoi( argument );

  if (sub < 1 || tom < 1){
    do_say(mob, "I'm sorry, print which subject and tome number?");
    return;
  }

  if ( (tome = GetTome(sub, tom)) == NULL){
    sprintf( buf, "We do not have a Tome under subject %d, index %d.", sub, tom);
    do_say(mob, buf );
    return;
  }

  /* cost of 1k */
  if (ch->gold < 1000){
    sprintf(buf, "You seem to lack the gold %s.", PERS2(ch));
    do_say(mob, buf );
    return;
  }
  ch->gold -= 1000;
  act("$n takes 1000 gold from you.", mob, NULL, ch, TO_VICT);

  //make the object
  obj = create_object( get_obj_index( OBJ_VNUM_TOME ), 0);

  //set name
  sprintf(buf, obj->name, tome->title );
  free_string(obj->name );
  obj->name = str_dup( buf );

  //set short desc
  sprintf(buf, obj->short_descr, tome->title, tome->author );
  free_string(obj->short_descr );
  obj->short_descr = str_dup( buf );

  //set long desc
  sprintf(buf, obj->description, tome->title, tome->author );
  free_string(obj->description );
  obj->description = str_dup( buf );

  //add extra desc based on tome
  obj->extra_descr = new_extra_descr();
  obj->extra_descr->keyword = str_dup(tome->title);
  obj->extra_descr->description = str_dup(tome->text);
  obj->extra_descr->next = NULL;

  obj_to_char( obj, ch );

  act("$n quickly retrieves a copy of your request.", mob, NULL, ch, TO_VICT);
  act("$n quickly retrieves a copy of $N's request.", mob, NULL, ch, TO_NOTVICT);
  act("$n gives $p to you.", mob, obj, ch, TO_VICT );
  act("$n gives $p to $N.", mob, obj, ch, TO_NOTVICT );
}

