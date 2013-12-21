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

#include<sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "merc.h"
#include "recycle.h"
#include "interp.h"
#include "vote.h"
#include "cabal.h"
#include "misc.h"


extern int	top_room;
extern bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote );

int total_paths = 0;

void	do_open		args( ( CHAR_DATA *ch, char *argument ) );
PATH_QUEUE* generate_path_list(ROOM_INDEX_DATA *src, ROOM_INDEX_DATA *target, int maxdist, bool fPassDoor, int* dist);

static struct bfs_queue_struct	*queue_head = NULL,
				*queue_tail = NULL,
				*room_queue = NULL,
				*path_queue = NULL;
static struct area_queue_struct *bfs_area_head = NULL,
				*bfs_area_tail = NULL,
				*area_head = NULL;

/* Utility macros */
#define MARK(room)	(SET_BIT(	(room)->room_flags, ROOM_MARK) )
#define UNMARK(room)	(REMOVE_BIT(	(room)->room_flags, ROOM_MARK) )
#define IS_MARKED(room)	(IS_SET(	(room)->room_flags, ROOM_MARK) )

#define P_MARK(room)	(SET_BIT(	(room)->room_flags, ROOM_PATH_MARK) )
#define P_UNMARK(room)	(REMOVE_BIT(	(room)->room_flags, ROOM_PATH_MARK) )
#define P_IS_MARKED(room) (IS_SET(	(room)->room_flags, ROOM_PATH_MARK) )

ROOM_INDEX_DATA *toroom( ROOM_INDEX_DATA *room, sh_int door )
{
    return (room->exit[door]->to_room);
}

bool valid_edge( ROOM_INDEX_DATA *room, sh_int door, bool fPassDoor )
{
    EXIT_DATA *pexit = room->exit[door];
    ROOM_INDEX_DATA *to_room;
    if ( pexit && (to_room = pexit->to_room) != NULL
	 && !IS_MARKED( to_room )
	 && (IS_SET(room->area->area_flags, AREA_MUDSCHOOL)
	     || !IS_SET(to_room->area->area_flags, AREA_MUDSCHOOL))
	 && (fPassDoor
	     || (!IS_SET(pexit->exit_info, EX_CLOSED)
		 && !IS_SET(pexit->to_room->room_flags2, ROOM_JAILCELL))) )
      return TRUE;
    else
      return FALSE;
}

bool valid_edge_path( ROOM_INDEX_DATA *room, sh_int door, bool fPassDoor ){
  EXIT_DATA *pexit = room->exit[door];
  ROOM_INDEX_DATA *to_room;
  if ( pexit && (to_room = pexit->to_room) != NULL
       && !P_IS_MARKED( to_room )
       && (IS_SET(room->area->area_flags, AREA_MUDSCHOOL)
	   || !IS_SET(to_room->area->area_flags, AREA_MUDSCHOOL))
       && (fPassDoor
	   || (!IS_SET(pexit->exit_info, EX_CLOSED)
	       && !IS_SET(pexit->to_room->room_flags2, ROOM_JAILCELL))) )
    return TRUE;
  else
    return FALSE;
}

void bfs_enqueue(ROOM_INDEX_DATA *room, PATH_QUEUE* path_data, char dir, int dist)
{
    struct bfs_queue_struct *curr;
    init_malloc("alloc bfs_enqueue");
    curr = alloc_mem( sizeof( *curr ));
    end_malloc("alloc bfs_enqueue");
    curr->room = room;
    curr->from_path = path_data;
    curr->dir = dir;
    curr->dist = dist;
    curr->next = NULL;
    if (queue_tail)
    {
        queue_tail->next = curr;
        queue_tail = curr;
    }
    else
        queue_head = queue_tail = curr;
}

void bfs_dequeue(void)
{
    struct bfs_queue_struct *curr;
    curr = queue_head;
    if (!(queue_head = queue_head->next))
        queue_tail = NULL;
    init_malloc("free_mem bfs_dequeue");
    free_mem(curr, sizeof( *curr ));
    end_malloc("free_mem bfs_dequeue");
}

void bfs_clear_queue(void)
{
    while (queue_head)
        bfs_dequeue();
}

//breadfirst search area queue
void bfs_area_enqueue(AREA_DATA *area, AREA_QUEUE* from, ROOM_INDEX_DATA* start_room ){
  struct area_queue_struct *curr;
  init_malloc("alloc bfs_area_enqueue");
  curr = alloc_mem( sizeof( *curr ));
  end_malloc("alloc bfs_area_enqueue");

  curr->area = area;
  curr->start_room = start_room;
  curr->from = from;
  curr->next = NULL;

  if (bfs_area_tail){
    bfs_area_tail->next = curr;
    bfs_area_tail = curr;
  }
  else
    bfs_area_head = bfs_area_tail = curr;
}

void bfs_area_dequeue(void){
  struct area_queue_struct *curr;
  curr = bfs_area_head;

  if (!(bfs_area_head = bfs_area_head->next))
    bfs_area_tail = NULL;

  init_malloc("free_mem bfs_area_dequeue");
  free_mem(curr, sizeof( *curr ));
  end_malloc("free_mem bfs_area_dequeue");
}

void bfs_area_clear_queue(void){
  while (bfs_area_head){
    bfs_area_dequeue();
  }
}

void room_enqueue(ROOM_INDEX_DATA *room )
{
    struct bfs_queue_struct *curr;
    init_malloc("alloc room_enqueue");
    curr = alloc_mem( sizeof( *curr ));
    end_malloc("alloc room_enqueue");
    curr->room = room;
    curr->next = room_queue;
    room_queue = curr;
}

void clean_room_queue(void)
{
    struct bfs_queue_struct *curr, *curr_next;
    for (curr = room_queue; curr; curr = curr_next )
    {
        UNMARK( curr->room );
        curr_next = curr->next;
        init_malloc("free_mem clean_room_queue");
        free_mem(curr, sizeof( *curr ));
        end_malloc("free_mem clean_room_queue");
    }
    room_queue = NULL;
}

AREA_QUEUE* area_enqueue(AREA_DATA *area, AREA_QUEUE* from, ROOM_INDEX_DATA* start_room ){
  AREA_QUEUE *curr;
  init_malloc("alloc area_enqueue");
  curr = alloc_mem( sizeof( *curr ));
  end_malloc("alloc area_enqueue");

  curr->area = area;
  curr->start_room = start_room;
  SET_BIT(curr->area->area_flags, AREA_MARKED );

  curr->from = from;
  curr->next = area_head;
  area_head = curr;

  return area_head;
}

void clean_area_queue(void){
  AREA_QUEUE *curr, *curr_next;

  for (curr = area_head; curr; curr = curr_next ){
    curr_next = curr->next;

    REMOVE_BIT(curr->area->area_flags, AREA_MARKED );

    init_malloc("free_mem clean_area_queue");
    free_mem(curr, sizeof( *curr ));
    end_malloc("free_mem clean_area_queue");
  }
  area_head = NULL;
}

PATH_QUEUE* path_enqueue(PATH_QUEUE* head, ROOM_INDEX_DATA *room, PATH_QUEUE* from_path, char from_dir)
{
    struct bfs_queue_struct *curr;
    init_malloc("alloc path_enqueue");
    curr = alloc_mem( sizeof( *curr ));
    end_malloc("alloc path_enqueue");

    curr->room = room;
    curr->from_path = from_path;
    curr->from_dir = from_dir;
    curr->next = head;

    total_paths++;
    return curr;
}

void clean_path_queue(PATH_QUEUE* head)
{
    struct bfs_queue_struct *curr, *curr_next;
    for (curr = head; curr; curr = curr_next )
    {
        P_UNMARK( curr->room );
        curr_next = curr->next;
	total_paths--;
        init_malloc("free_mem path_room_queue");
        free_mem(curr, sizeof( *curr ));
        end_malloc("free_mem path_room_queue");
    }
    head = NULL;
}

void clean_path(){
  clean_path_queue( path_queue );
  path_queue = NULL;
}

int find_first_step(ROOM_INDEX_DATA *src, ROOM_INDEX_DATA *target, int maxdist, bool fPassDoor, int* dist)
{
  int curr_dir;
    if ( !src || !target )
    {
        bug("Illegal value passed to find_first_step (misc.c)", 0 );
        return BFS_ERROR;
    }
    if (src == target)
        return BFS_ALREADY_THERE;
    room_enqueue( src );
    MARK(src);
    /* first, enqueue the first steps, saving which direction we're going. */
    for (curr_dir = 0; curr_dir < 10; curr_dir++)
        if (valid_edge(src, curr_dir, fPassDoor))
        {
            MARK(toroom(src, curr_dir));
	    room_enqueue(toroom(src, curr_dir) );
            bfs_enqueue(toroom(src, curr_dir), NULL, curr_dir, 1);
        }
    while (queue_head)
    {
      if ( queue_head->dist > maxdist)
        {
	    bfs_clear_queue();
	    clean_room_queue();
	    return BFS_NO_PATH;
        }
        if (queue_head->room == target)
	{
	    curr_dir = queue_head->dir;
	    if (dist)
	      *dist = queue_head->dist;
	    bfs_clear_queue();
	    clean_room_queue();
	    return curr_dir;
        }
	else
	{
            for (curr_dir = 0; curr_dir < 10; curr_dir++)
                if (valid_edge(queue_head->room, curr_dir, fPassDoor))
		{
                    MARK(toroom(queue_head->room, curr_dir));
	            room_enqueue(toroom(queue_head->room, curr_dir) );
	            bfs_enqueue(toroom(queue_head->room, curr_dir),
				NULL,
				queue_head->dir, queue_head->dist + 1);
                }
            bfs_dequeue();
        }
    }
    clean_room_queue();
    return BFS_NO_PATH;
}

/* recursive list copy function to preserve order of lists */
/* copies ONLY path cells required for the path */
PATH_QUEUE* copy_list( PATH_QUEUE* curr, PATH_QUEUE* head ){
  if (curr == NULL)
    return head;
  else
    head = path_enqueue( head, curr->room, curr->from_path, curr->from_dir );
  return (copy_list( curr->from_path, head ));
}

/* catenate path1 to path2, returns new path which is path1+path2 */
PATH_QUEUE* cat_paths( PATH_QUEUE* p1, PATH_QUEUE* p2 ){
  return (copy_list( p1, p2 ));
}

/*
   Uses area exists to generate a bread first search work queue
   and a list of marked areas
*/
AREA_QUEUE* generate_area_list( AREA_DATA *src, ROOM_INDEX_DATA* src_room, AREA_DATA *target, CABAL_DATA* pc_only){
  AREA_QUEUE* curr;
  EXIT_DATA* pe;

  if (src == NULL || target == NULL)
    return NULL;

  //clean area queue before we start
  clean_area_queue();

  //add this area onto the list of marked areas


  //enqueue this area onto the search queue
  bfs_area_enqueue( src, area_enqueue( src, NULL, src_room ), NULL );

  while (bfs_area_head){
    curr = bfs_area_head;

    /* ez case */
    if (curr->area == target){
      //nuke the area list
      bfs_area_clear_queue();
      return curr->from;
    }
    else{
      /* loop through the area exists */
      for (pe = curr->area->exits; pe; pe = pe->next_in_area ){
	/* check if this area is valid for adding to work queue */
	if (pe->to_room  == NULL)
	  continue;
	else if (IS_SET(pe->to_room->area->area_flags, AREA_MARKED))
	  continue;
	else if (IS_SET(pe->to_room->area->area_flags, AREA_MUDSCHOOL))
	  continue;
	else if (IS_SET(pe->to_room->area->area_flags, AREA_NOPATH))
	  continue;
	/* cabalsupport */
	else if (pc_only && !can_reinforce(pc_only, pe->to_room->area))
	  continue;
	else{
	  bfs_area_enqueue( pe->to_room->area,
			    area_enqueue( pe->to_room->area, curr->from, pe->to_room ),
			    pe->to_room );
	}
      }
    }
    bfs_area_dequeue();
  }
  //nuke the area list
  bfs_area_clear_queue();
  //clean_area_queue must be called once the area list is no longer needed
  return NULL;
}

/* creates a persistent path from src to target
   1) Creates a list of areas from src to target
   2) Generates path from each area to anotehr
   3) catenates paths

   NOTE: All of his is done FROM target TO src
*/
PATH_QUEUE* generate_path( ROOM_INDEX_DATA *src, ROOM_INDEX_DATA *target, int maxdist, bool fPassDoor, int* dist, CABAL_DATA* pc_only){
  PATH_QUEUE* final_path = NULL;
  PATH_QUEUE* area_path;
  AREA_QUEUE* area_list;

  AREA_QUEUE* curr_area;
  ROOM_INDEX_DATA* curr_tar = target;

  int ldist = 0;

  if (src == NULL || target == NULL)
    return NULL;

  //generate area list
  if ((area_list = generate_area_list( src->area, src, target->area, pc_only )) == NULL)
    return NULL;


  curr_tar = target;
  for (curr_area = area_list; curr_area; curr_area = curr_area->from ){
    ldist = 0;
    area_path = generate_path_list(curr_area->start_room,
				   curr_tar,
				   maxdist,
				   fPassDoor,
				   &ldist );
    //case of error
    if (area_path == NULL){
      clean_path_queue( final_path );
      clean_path( );
      clean_area_queue();
      return NULL;
    }
    //catenate paths
    final_path = cat_paths( area_path, final_path );
    clean_path( );
    maxdist -= ldist;
    *dist += ldist;

    //reset target for next loop
    curr_tar = curr_area->start_room;
  }
  clean_area_queue();
  return final_path;
}

/* Returns pointer to the list representing the path to be taken using
   room->from_room link.
   This list does not persist between calls to generate_path!
*/
PATH_QUEUE* generate_path_list(ROOM_INDEX_DATA *src, ROOM_INDEX_DATA *target, int maxdist, bool fPassDoor, int* dist){
  PATH_QUEUE* from;
  int curr_dir;

  if ( !src || !target ){
    bug("Illegal value passed to generate_path_list (misc.c)", 0 );
    return NULL;
  }

  //reset room list
  clean_path();

  //put origin room on list
  from = NULL;
  path_queue = path_enqueue( path_queue, src, NULL, 0 );
  P_MARK(src);

  if (src == target)
    return path_queue;

  /* first, enqueue the first steps, saving which direction we're going. */
  for (curr_dir = 0; curr_dir < MAX_DOOR; curr_dir++){
    if (valid_edge_path(src, curr_dir, fPassDoor)){
      path_queue = path_enqueue(path_queue,
				toroom(src, curr_dir),
				from,
				curr_dir);
      P_MARK(toroom(src, curr_dir));
      bfs_enqueue(toroom(src, curr_dir),
		  path_queue,
		  curr_dir,
		  1);
    }
  }

//work from top of the queue adding rooms to the queue
  while (queue_head){
    if ( queue_head->dist > maxdist){
      bfs_clear_queue();
      clean_path();
      return NULL;
    }
    if (queue_head->room == target){
      curr_dir = queue_head->dir;
      if (dist)
	*dist = queue_head->dist;
      from = queue_head->from_path;
      bfs_clear_queue();
      return from;
    }
    else{
      //put all room we can check onto the work queue
      for (curr_dir = 0; curr_dir < MAX_DOOR; curr_dir++){
	if (valid_edge_path(queue_head->room, curr_dir, fPassDoor)){
	  P_MARK(toroom(queue_head->room, curr_dir));
	  path_queue = path_enqueue(path_queue,
				    toroom(queue_head->room, curr_dir),
				    queue_head->from_path,
				    curr_dir);
	  bfs_enqueue(toroom(queue_head->room, curr_dir),
		      path_queue,
		      queue_head->dir,
		      queue_head->dist + 1);
	}
      }
      bfs_dequeue();
    }
  }
  clean_path();
  return NULL;
}

void hunt_victim( CHAR_DATA *ch )
{
    extern char * const dir_name[];
    int roll, dir;
    bool found = FALSE;
    CHAR_DATA     *tmp;
    if( ch == NULL || ch->in_room == NULL || ch->hunting == NULL
	|| !IS_NPC(ch) || IS_NPC(ch->hunting)
	|| (ch->pIndexData->vnum >= MOB_VNUM_SERVANT_FIRE
	    && ch->pIndexData->vnum <= MOB_VNUM_SERVANT_EARTH))
      return;
    for( tmp = player_list; tmp && !found; tmp = tmp->next_player )
        if( ch->hunting == tmp )
	  found = TRUE;
    if( !found
	|| ch->hunting->in_room == NULL
	|| IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL))
    {
      act( "$n says '`#Damn!  I can't find $M!``'", ch, NULL, ch->hunting, TO_ROOM );
      ch->hunting = NULL;
      if (IS_SET(ch->act,ACT_AGGRESSIVE) && ch->in_room != NULL){
	act( "$n leaves the area.",ch,NULL,NULL,TO_ROOM );
	extract_char( ch, TRUE );
      }
      return;
    }
    WAIT_STATE( ch, skill_table[gsn_hunt].beats );
    roll = (number_range(1,100));

    if (roll < 33)
        act( "$n carefully sniffs the air.", ch, NULL, NULL, TO_ROOM );
    else if (roll < 66)
        act( "$n carefully checks the ground for tracks.", ch, NULL, NULL, TO_ROOM );
    else
        act( "$n listens carefully for some sounds.", ch, NULL, NULL, TO_ROOM );
    dir = find_first_step(ch->in_room, ch->hunting->in_room, 1024, TRUE, NULL);

    if (dir<0){
      if (ch->pIndexData->vnum == MOB_VNUM_SHADOW
	  || ch->pIndexData->vnum == MOB_VNUM_NEMESIS
	  || ch->pIndexData->vnum == MOB_VNUM_STALKER){
	if (ch->in_room != ch->hunting->in_room)
	  act("$n grows still as if awating something.", ch, NULL, NULL, TO_ROOM);
	return;
      }
      else{
	act( "$n says '`#Damn!  Lost $M!``'", ch, NULL, ch->hunting, TO_ROOM );
	ch->hunting = NULL;
	if (IS_SET(ch->act,ACT_AGGRESSIVE) && ch->in_room != NULL){
	  act( "$n slowly disappears.",ch,NULL,NULL,TO_ROOM );
	  extract_char( ch, TRUE );
	}
	return;
      }
    }
    if ( IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED )
	 && !(IS_AFFECTED(ch,AFF_PASS_DOOR) && !IS_SET(ch->in_room->exit[dir]->exit_info,EX_NOPASS))){
      do_open( ch, (char *) dir_name[dir] );
      return;
    }
    if (get_eq_char( ch, WEAR_RANGED) && get_eq_char( ch, WEAR_QUIVER)){
      char buf[MIL];
      sprintf( buf, "%s", ch->hunting->name );
      do_shoot( ch, buf );

      if (ch == NULL || ch->in_room == NULL)
	return;
    }

    if (!IS_SET(ch->in_room->exit[dir]->to_room->room_flags, ROOM_NO_MOB)
	&& ( !IS_SET(ch->act, ACT_STAY_AREA) || ch->in_room->exit[dir]->to_room->area == ch->in_room->area ) )
      move_char( ch, dir, FALSE );
}

void return_to_leader( CHAR_DATA *ch )
{
    extern char * const dir_name[];
    int dir;
    bool found = FALSE;
    CHAR_DATA *tmp;
    if( ch == NULL || ch->in_room == NULL || ch->leader == NULL || ch->summoner == NULL
    || !IS_NPC(ch) || IS_NPC(ch->summoner) || !IS_SET(ch->special,SPECIAL_RETURN))
        return;
    for( tmp = player_list; tmp && !found; tmp = tmp->next_player )
        if( ch->summoner == tmp )
            found = TRUE;
    if( !found || ch->summoner->in_room == NULL)
    {
        act( "$n says '`#I lost my master, $N!``'", ch, NULL, ch->summoner, TO_ROOM );
	REMOVE_BIT(ch->special,SPECIAL_RETURN);
        return;
    }
    if (ch->summoner->in_room == ch->in_room)
    {
	REMOVE_BIT(ch->special,SPECIAL_RETURN);
	return;
    }
    dir = find_first_step(ch->in_room, ch->summoner->in_room, 1024, TRUE, NULL);
    if (dir<0)
    {
        act( "$n says '`#I lost my master, $M!``'", ch, NULL, ch->summoner, TO_ROOM );
	REMOVE_BIT(ch->special,SPECIAL_RETURN);
        return;
    }
    if (IS_AFFECTED2(ch,AFF_MISDIRECTION))
    {
        do
        {
            dir = number_door();
        }
        while( ( ch->in_room->exit[dir] == NULL ) || ( ch->in_room->exit[dir]->to_room == NULL ) );
    }
    if ( IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED ) )
    {
        do_open( ch, (char *) dir_name[dir] );
        return;
    }
    move_char( ch, dir, TRUE );
}

void do_pipe( CHAR_DATA *ch, char *argument )
{
    char buf[5000];
    FILE *fp = popen( argument, "r" );
/*    if (ch != NULL)
    {
        fgetf( buf, 5000, fp );
        page_to_char( buf, ch );
    }*/
    fgetf( buf, 5000, fp );
    if (ch != NULL)
        page_to_char( buf, ch );
    else
	append_string( CRASH_FILE, buf );
    pclose( fp );
}

char *fgetf( char *s, int n, register FILE *iop )
{
    register int c = '\0';
    register char *cs = s;
    while( --n > 0 && (c = getc(iop)) != EOF)
	if ((*cs++ = c) == '\0')
	    break;
    *cs = '\0';
    return((c == EOF && cs == s) ? NULL : s);
}

bool find_bank(CHAR_DATA *ch, bool fQuiet)
{
/* This is where you tell which rooms are banks. *
   Just keep adding to it, whatever.             */
    if (ch->in_room->vnum != 3
	&& ch->in_room->vnum != 20793 // Val Miran
	&& ch->in_room->vnum != 7873 // Miruvhor
        && ch->in_room->vnum != 11483 // Rheydin
        && ch->in_room->vnum != 8027 // Illithids
        && ch->in_room->vnum != 21584 // Dwarf city
        && ch->in_room->vnum != 1299 // Xymerria
        && ch->in_room->vnum != 10971// Xymerria
        && ch->in_room->vnum != 21093) // Duergars
    {
      if (!fQuiet)
        send_to_char("You can't do that here!\n\r", ch);
      return FALSE;
    }
    return TRUE;
}

int subtract_gold( CHAR_DATA* ch, int gold){

  ch->gold -= gold;
  if (ch->gold < 0){
    ch->in_bank += ch->gold;
    sendf(ch, "%d gold has been withdrawn directly from your account.\n\r", abs(ch->gold));
    ch->gold = 0;
  }
  if (ch->in_bank < 0)
    ch->in_bank = 0;

  return ch->in_bank + ch->gold;
}

void find_money(CHAR_DATA *ch)
{
    if (ch->in_bank > 30000000)
	ch->in_bank = 30000000;
    if (ch->in_bank < -30000000)
        ch->in_bank = -30000000;
}

void do_deposit(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA* victim;
    char arg[MIL];
    argument = one_argument(argument, arg);
    if (!find_bank(ch, FALSE))
	return;
    if (IS_NPC(ch))
    {
        send_to_char("Mobs can't deposit money.\n\r", ch);
	return;
    }
    find_money(ch);
    if (arg[0] == '\0' || !is_number(arg) || atoi(arg) <= 0 )
    {
        send_to_char("Try deposit <amount> or deposit <amount> <char>\n\r", ch);
        return;
    }
    if (ch->gold < atoi(arg))
    {
        send_to_char("You don't have that much gold.\n\r", ch);
        return;
    }
    /* check if depositing to someone elses account */
    if (!argument[0])
      victim = ch;
    else{
      if( (victim = get_char(argument)) == NULL){
	send_to_char("Your beneficiary must be in the lands.\n\r", ch);
	return;
      }
      if (IS_NPC(victim)){
	send_to_char("Not on mobiles.\n\r", ch);
	return;
      }
    }

    if ((victim->in_bank + atoi(arg)) > 30000000)
    {
        send_to_char("I'm sorry, this bank has a 30 million gold limit.\n\r", ch);
   	return;
    }
    ch->gold -= atoi(arg);
    victim->in_bank += atoi(arg);
    if (victim == ch)
      sendf(ch, "Your balance is %d.\n\r", ch->in_bank);
    else{
      sendf(victim, "`8A Bank squire hands you a stub and gallops away.``\n\r Deposit: %d by: [%s].  Current total: %d\n\r",
	    atoi(arg), PERS2(ch), victim->in_bank);
      sendf(ch, "You transfer %d gold to %s's account.\n\r", atoi(arg), PERS2(victim));
    }
}

int locker_cost( OBJ_DATA* list, int cost){
  if (list == NULL)
    return cost;

  for (;list;list = list->next_content){
    if (list->contains)
      cost += locker_cost( list->contains, cost );
    cost += list->level / 2;
  }

  return cost;
}

/* calculates monthly cost of items in locker
   SUM of obj->level
*/
int get_locker_cost( OBJ_DATA* locker){
  int cost = 0;
  if (locker == NULL)
    return 0;
  cost = locker_cost( locker->contains, cost);

  if (cost == 0)
    return 0;
  else
    return (UMAX(cost, 500));
}



/*locker commands
  purchase
  store <item>
  retrieve <item>
*/
void do_locker(CHAR_DATA *ch, char *argument){
  char cmd[MIL];
  OBJ_DATA* locker;

  argument = one_argument( argument, cmd );
  //check for locker
  for (locker = ch->carrying; locker; locker = locker->next_content){
    if (locker->vnum == OBJ_VNUM_LOCKER)
      break;
  }

  if (IS_NULLSTR(cmd)){
    if (locker == NULL){
      send_to_char("You must first purchase a locker at a bank. Seek \"help locker\" for more.\n\r", ch);
      return;
    }
    send_to_char("Your locker currently contains:\n\r", ch);
    show_list_to_char( locker->contains, ch, TRUE, TRUE );
    sendf(ch,
	  "\n\rThere are %d/%d items in your locker.\n\r"\
	  "Your locker is currently costing you: %d gold per month.\n\r",
	  count_obj_inlist(locker->contains, 0), locker->value[3],
	  get_locker_cost( locker ));
    return;
  }
  //PURCHASE
  if (!str_prefix("purchase", cmd)){
    OBJ_INDEX_DATA* pIndex;
    if (locker != NULL){
      send_to_char("You already have a locker purchased.\n\r", ch);
      return;
    }
    else if (!find_bank(ch, FALSE))
      return;
    else if ( (pIndex = get_obj_index( OBJ_VNUM_LOCKER)) == NULL){
      send_to_char("Error getting locker index.\n\r", ch);
      return;
    }
    else if (ch->in_bank < pIndex->cost){
      sendf(ch, "You need at least %d gold in your bank to purchase a locker.\n\r", pIndex->cost);
      return;
    }
    else
      ch->in_bank -= pIndex->cost;

    if ( (locker = create_object( pIndex, 0)) == NULL){
      send_to_char("Error loading the locker.\n\r", ch);
      return;
    }

    obj_to_char( locker, ch );

    send_to_char("You now posses a bank locker.  Seek \"help locker\" for details.\n\r", ch);
    return;
  }
  //STASH
  else if (!str_prefix("stash", cmd)){
    OBJ_DATA* obj;
    int cost = 100;
    int items, items_in_locker;
    bool fRemote = FALSE;

    if (locker == NULL){
      send_to_char("You must first purchase a locker at a bank. Seek \"help locker\" for more.\n\r", ch);
      return;
    }
    else if ( (obj = get_obj_list( ch, argument, ch->carrying)) == NULL){
      send_to_char("You're not carrying that.\n\r", ch);
      return;
    }
    else if (obj == locker){
      send_to_char("You cannot stash your locker.\n\r", ch);
      return;
    }
    else if (obj->wear_loc != -1){
      send_to_char("The item must be in your inventory.\n\r", ch);
      return;
    }
    else if (IS_LIMITED(obj)){
      send_to_char("This item is too powerful to be stored in a locker.\n\r", ch);
      return;
    }
    else if (!can_drop_obj(ch, obj )){
      send_to_char("You can't seem to let go of it.\n\r", ch);
      return;
    }

    //get per item cost
    if (!find_bank(ch, TRUE)){
      if (is_fight_delay(ch, 120)){
	send_to_char("The bank refuses to send their servant due to recent combat.\n\r", ch);
	return;
      }
      else{
	fRemote = TRUE;
	cost = 1000;
      }
    }
    else
      cost = 50;

    //get number of items to store
    items = 1 + count_obj_inlist( obj->contains, 0);
    items_in_locker = count_obj_inlist( locker->contains, 0);
    cost *= items;

    if (items_in_locker + items > locker->value[3]){
      sendf(ch, "You may only store additional %d items. (You tried %d)\n\r", locker->value[3] - items_in_locker, items);
      return;
    }
    else if (ch->in_bank + ch->gold < cost){
      sendf(ch, "You will need at least %d gold to store these items.\n\r", cost);
      return;
    }
    else
      subtract_gold( ch, cost);

    obj_from_char( obj );
    obj_to_obj( obj, locker );

    if (fRemote){
      act("With a flash of magic a bank servant gates in, takes $p from $n and departs.", ch, obj, NULL, TO_ROOM);
      act("With a flash of magic a bank servant gates in, takes $p from you and departs.", ch, obj, NULL, TO_CHAR);
    }
    else{
      act("$n stores $p in $s locker.", ch, obj, NULL, TO_ROOM);
      act("You store $p in your locker.", ch, obj, NULL, TO_CHAR);
    }
    WAIT_STATE(ch, PULSE_VIOLENCE / 3);
    return;
  }
  //retrieve
  else if (!str_prefix("retrieve", cmd)){
    OBJ_DATA* obj;

    if (locker == NULL){
      send_to_char("You must first purchase a locker at a bank. Seek \"help locker\" for more.\n\r", ch);
      return;
    }
    else if (!find_bank(ch, FALSE))
      return;
    else if (locker->contains == NULL){
      send_to_char("Your locker is empty.\n\r", ch);
      return;
    }
    else if ( (obj = get_obj_list( ch, argument, locker->contains)) == NULL){
      send_to_char("You're not storing such item in your locker.\n\r", ch);
      return;
    }
    else if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) ){
      act( "$d: you can't carry that many items.",ch, NULL, obj->name, TO_CHAR );
      return;
    }
    else if (get_carry_weight(ch) + get_obj_weight_char(ch, obj) >= can_carry_w(ch)){
      act( "$d: you can't carry that much weight.",ch, NULL, obj->name, TO_CHAR );
      return;
    }

    obj_from_obj( obj );
    obj_to_char(obj, ch);

    act("$n retrieves $p in $s locker.", ch, obj, NULL, TO_ROOM);
    act("You retrieve $p in your locker.", ch, obj, NULL, TO_CHAR );
    return;
  }
  else
    do_locker(ch, "");
}
void do_balance(CHAR_DATA *ch, char *argument)
{
    if (!find_bank(ch, FALSE))
	return;
    if (IS_NPC(ch))
    {
        send_to_char("Mobs don't have accounts.\n\r", ch);
	return;
    }
    find_money(ch);
    sendf(ch, "You have %d gold in the bank.\n\r", ch->in_bank);
}

//wrapper commands for do_locker below, to cut down on conflict with "lock" command.
void do_stash(CHAR_DATA *ch, char *argument){
  char buf[MIL];

  if (IS_NULLSTR(argument))
    do_locker(ch, "" );
  else if (!str_cmp(argument, "purchase")){
    sprintf( buf, "purchase");
    do_locker(ch, buf );
  }
  else{
    sprintf( buf, "%s %s", "stash", argument );
    do_locker(ch, buf );
  }
}
void do_retrieve(CHAR_DATA *ch, char *argument){
  char buf[MIL];

  if (IS_NULLSTR(argument))
    do_locker(ch, "" );
  else{
    sprintf( buf, "%s %s", "retrieve", argument );
    do_locker(ch, buf );
  }
}

//performs monthly bank updates for lockers
void bankUpdate( CHAR_DATA* ch ){
  OBJ_DATA* locker;

  int last_year, last_month;
  int curr_year, curr_month;
  int buf, cost;

  //npc check, and a simple check to prevent rest of calcs unless ast least 1 month elapsed
  if (IS_NPC(ch))
    return;

  //reset stamp for people whom never had a bank update yet
  if (ch->pcdata->bank_stamp < 1)
    ch->pcdata->bank_stamp = mud_data.current_time;

  //check for minimum time elapsed
  if (mud_data.current_time - ch->pcdata->bank_stamp < 30 * MAX_HOURS * MAX_DAYS * MAX_WEEKS)
    return;

  rltom_date(ch->pcdata->bank_stamp, &buf, &buf, &last_month, &last_year);
  curr_year = mud_data.time_info.year;
  curr_month = mud_data.time_info.month;

  //get number of months that elapsed
  buf = (curr_year - last_year) * MAX_MONTHS + curr_month - last_month;

  if (buf < 1)
    return;
  else{
    ch->pcdata->bank_stamp = mud_data.current_time;
  }
  for (locker = ch->carrying; locker; locker = locker->next_content){
    if (locker->vnum == OBJ_VNUM_LOCKER){
      break;
    }
  }

  if (locker == NULL)
    return;

  if (ch->in_bank < 0){
    ch->in_bank += locker->cost / 2;
    obj_from_char( locker );
    extract_obj( locker );
    sendf(ch, "Your locker has been sold to cover your debts.\n\r");
    if (ch->in_bank < 0)
      ch->in_bank = 0;
    return;
  }
  cost = buf * get_locker_cost( locker );
  ch->in_bank -= cost;
  sendf(ch, "%d gold has been withdrawn from your bank to cover locker costs.  %d gold left.\n\r", cost, ch->in_bank);
  if (ch->in_bank < 0){
    send_to_char("You have one month to repay the balance, or your locker will be sold.\n\r", ch);
  }
}



void do_withdraw(CHAR_DATA *ch, char *argument)
{
    char arg[MIL];
    int amount = 0,  fee = 0;
    if (!find_bank(ch, FALSE))
	return;
    one_argument(argument, arg);
    if (IS_NPC(ch))
    {
        send_to_char("Mobs can't withdraw money.\n\r", ch);
	return;
    }
    find_money(ch);
    if (arg[0] == '\0' || !is_number(arg) || atoi(arg) <= 0)
    {
	send_to_char("Try withdraw <amount>\n\r", ch);
	return;
    }
    amount = atoi(arg);
    if (amount < 10){
      send_to_char("The bank will not bother for amounts less then 10 gold.\n\r", ch );
      return;
    }
    if (ch->in_bank < amount)
    {
        send_to_char("You don't have that much gold in the bank.\n\r", ch);
	return;
    }
    /* calculate the 10% cost for non greedy people */
    if (IS_PERK(ch, PERK_GREEDY)){
      fee = 0;
      if (ch->in_bank < amount ){
	amount = ch->in_bank;
      }
    }
    else{
      fee = amount / 10;
      if (ch->in_bank <  (amount + fee) ){
	amount = ch->in_bank / 1.1;
	fee = amount / 10;
      }
    }

    ch->in_bank -= amount;
    ch->in_bank -= fee;
    ch->gold += amount;
    ch->in_bank = UMAX(0, ch->in_bank);

    find_money(ch);
    sendf(ch, "You withdraw %d gold, minus %d for withdrawal fees.\n\r", amount, fee);
    sendf(ch, "You now have %d gold in the bank.\n\r", ch->in_bank);
}


/* adds a temporary quest: tempquest <char> <Quest Text>
 * ex: tempquest bob Killed the Green Dragon
 */
void do_tempquest( CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim;
  char arg1[MIL];
  argument = one_argument( argument, arg1 );

  if (IS_NULLSTR(argument)){
    send_to_char("Syntax: tempquest <char> <quest text>\n\r", ch);
    return;
  }
  if (!str_cmp(arg1, "self"))
    victim = ch;
  else if ( (victim = get_char( arg1 )) == NULL){
    send_to_char("Target character not found.\n\r", ch);
    return;
  }
  if (IS_NULLSTR(argument)){
    send_to_char("You must enter the text for the quest.\n\r", ch);
    return;
  }
/* add a quest with fSave FALSE (not saved) */
  add_quest(victim, argument, QUEST_TEMP);
}

/* adds a permament quest: permquest <char> <Quest Text>
 * ex: permquest bob Killed All the Dragons
 * NOTE: using permquest on temporary quest simply changes it to perm. quest
 */
/*
   NOTE: if char is not present this tries to bring them on, set the quest
   and remove them from the game.
*/
void do_permquest( CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim;
  DESCRIPTOR_DATA* d;
  char arg1[MIL];
  bool fRemote = FALSE;

  argument = one_argument( argument, arg1 );
  d = new_descriptor();

  if (IS_NULLSTR(argument)){
    send_to_char("Syntax: permquest <char> <quest text>\n\r", ch);
    return;
  }
  if (!str_cmp(arg1, "self"))
    victim = ch;
  else if ( (victim = get_char( arg1 )) == NULL){
    /* see if we can bring them on */
    d->editor = 69;
    if ( (load_char(d, arg1)) != TRUE){
      send_to_char("Target character not found.\n\r", ch);
      return;
    }
    else{
      fRemote = TRUE;
      victim = d->character;
      /* put character in a room for proper extraction */
      d->character->next = char_list;
      char_list = d->character;
      d->character->next_player = player_list;
      player_list = d->character;
      reset_char(d->character);
/* load objects */
      load_obj( d->character );
/* make sure some data is unaffected */
      d->character->pcdata->roomnum /= -1;
      d->character->logon = 0;
/* load to room */
      char_to_room (d->character, ch->in_room);
    }
  }
  if (IS_NULLSTR(argument)){
    send_to_char("You must enter the text for the quest.\n\r", ch);
    return;
  }
/* add a quest with fSave TRUE (saved) */
  add_quest(victim, argument, QUEST_PERM);
/* check if this was remote and hence we need to purge */
  if (fRemote){
    SET_BIT(victim->pcdata->messages, MSG_QUEST);
    d = victim->desc;
    save_char_obj( victim );
    extract_char( victim, TRUE );
    if ( d != NULL )
      close_socket( d );
  }
  else
    free_descriptor( d );
}

/* sets invisible quest */
void do_inviquest( CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim;
  DESCRIPTOR_DATA* d;
  char arg1[MIL];
  bool fRemote = FALSE;

  argument = one_argument( argument, arg1 );
  d = new_descriptor();

  if (IS_NULLSTR(argument)){
    send_to_char("Syntax: invisquest <char> <quest text>\n\r", ch);
    return;
  }
  if (!str_cmp(arg1, "self"))
    victim = ch;
  else if ( (victim = get_char( arg1 )) == NULL){
    /* see if we can bring them on */
    d->editor = 69;
    if ( (load_char(d, arg1)) != TRUE){
      send_to_char("Target character not found.\n\r", ch);
      return;
    }
    else{
      fRemote = TRUE;
      victim = d->character;
      /* put character in a room for proper extraction */
      d->character->next = char_list;
      char_list = d->character;
      d->character->next_player = player_list;
      player_list = d->character;
      reset_char(d->character);
/* load objects */
      load_obj( d->character );
/* make sure some data is unaffected */
      d->character->pcdata->roomnum /= -1;
      d->character->logon = 0;
/* load to room */
      char_to_room (d->character, ch->in_room);
    }
  }
  if (IS_NULLSTR(argument)){
    send_to_char("You must enter the text for the quest.\n\r", ch);
    return;
  }
/* add a quest with fSave TRUE (saved) */
  add_quest(victim, argument, QUEST_INVIS);
/* check if this was remote and hence we need to purge */
  if (fRemote){
    SET_BIT(victim->pcdata->messages, MSG_QUEST);
    d = victim->desc;
    save_char_obj( victim );
    extract_char( victim, TRUE );
    if ( d != NULL )
      close_socket( d );
  }
  else
    free_descriptor( d );
}

/* removes a quest from character
 * Either a name, or number on list or "all" can be specified.
 * syntax: remquest <char> <quest text/quest order>
 */

void do_remquest( CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim;
  char arg1[MIL];
  argument = one_argument( argument, arg1 );

  if (IS_NULLSTR(argument)){
    send_to_char("Syntax: remquest <char> <quest text>\n\r", ch);
    return;
  }
  if (!str_cmp(arg1, "self"))
    victim = ch;
  else if ( (victim = get_char( arg1 )) == NULL){
    send_to_char("Target character not found.\n\r", ch);
    return;
  }
  if (IS_NULLSTR(argument)){
    send_to_char("You must enter the text for the quest or its number.\n\r", ch);
    return;
  }
/* add a quest with fSave TRUE (saved) */
  del_quest(victim, argument);
}


/* these are here for sake of backward compatibility */
void do_setquest( CHAR_DATA *ch, char *argument )
{
  return;
}

void do_setquest2( CHAR_DATA *ch, char *argument )
{
  return;
}

int questnum( CHAR_DATA *ch, char *argument )
{
  return FALSE;
}

/* adds a psalm: addpsalm <char> <psalm name>
 * ex: addpsalm bob divine void
 */
void do_addpsalm( CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim;
  int sn = 0;
  char arg1[MIL];
  argument = one_argument( argument, arg1 );

  if (IS_NULLSTR(argument)){
    send_to_char("Syntax: addpsalm <char> <psalm>\n\r", ch);
    return;
  }
  if (!str_cmp(arg1, "self"))
    victim = ch;
  else if ( (victim = get_char( arg1 )) == NULL){
    send_to_char("Target character not found.\n\r", ch);
    return;
  }
  if (IS_NULLSTR(argument)){
    send_to_char("You must enter the name of psalm.\n\r", ch);
    return;
  }
  if ((sn = psalm_lookup( argument )) == 0){
    send_to_char("That's not a psalm\n\r", ch);
    return;
  }
  add_psalm(victim, sn);
}

/* removes a psalm, same syntax as above */
void do_rempsalm( CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim;
  int sn = 0;
  char arg1[MIL];
  argument = one_argument( argument, arg1 );

  if (IS_NULLSTR(argument)){
    send_to_char("Syntax: rempsalm <char> <psalm>\n\r", ch);
    return;
  }
  if (!str_cmp(arg1, "self"))
    victim = ch;
  else if ( (victim = get_char( arg1 )) == NULL){
    send_to_char("Target character not found.\n\r", ch);
    return;
  }
  if (IS_NULLSTR(argument)){
    send_to_char("You must enter the name of psalm.\n\r", ch);
    return;
  }
  if ((sn = psalm_lookup( argument )) == 0){
    send_to_char("That's not a psalm\n\r", ch);
    return;
  }
  remove_psalm(victim, sn);
}


/* Written by: Virigoth							*
 * Returns: static string with the prompt    			        *
 * Comment: returns an analog bar prompt shorter then health_prompt
 */
char* short_bar( int cur, int max ){
  static char output[10];
  char* color;
  int perc = 100 * cur / max;
  int i;

  if (perc  < 17)
    color = "`!";
  else if (perc  < 33)
    color = "`1";
  else if (perc  < 65)
    color = "`#";
  else
    color = "`2";

  sprintf( output, "[%s", color );

  for (i = 0; i < 6; i++){
    if (perc > (i * 16))
      strcat(output, "|");
    else
      strcat(output, "-");
  }
  strcat(output, "``]");

  return output;
}

/* Written by: Virigoth							*
 * Returns: static string with the prompt    			        *
 * Used: comm.c (bust-a-prompt)						*
 * Comment: returns a digital or analog health info of ch based on fDig	*
 * (TRUE for digital)							*/

void health_prompt(char* buf, int cur, int max, bool fDig){
  char* str = buf;
  char bar = '=';		//Character to use for each "bar" ofhealth
  char nobar ='-';		//Character to use for abscence of health
  char div = '|';		//Divisor for 25/50/75% marks;
  int percent = 0;
  int steps = 3;		//Bars per division (25%)

  if (max < 1){
    bug("health_prompt: division by zero passed. (misc.c)", 0);
    return;
  }

  percent = 100 * cur / max;

  if (fDig){
    sprintf(buf,"[%d]", percent);
  }
  else{
    int i = 0;
    *str++ = '[';
    /* start new color */
    if (percent < 75){
      *str++ = '`';
      *str++ = (percent < 5? '!' : (percent < 25? '1' : '3'));
    }

    for (i = 0; i < steps * 4; i ++){

      /* division */
      if (i != 0 && i % steps == 0){
	/* end previous color */
	*str++ = '`';
	*str++ = '`';

	/* divider */
	*str++ = div;

	/* continue color if there is health in the section */
	if (percent < 75){// && (i * 25 / steps) < percent){
	  *str++ = '`';
	  *str++ = (percent < 5? '!' : (percent < 25? '1' : '3'));
	}//END new color
      }//END division

      /* regular bar (health/no health)*/
      if ( (i * 25 / steps) < percent)
	*str++ = bar;
      else
	*str++ = nobar;

    }//END for

    /* end color if any */
    *str++ = '`';
    *str++ = '`';
    *str++ = ']';
    /* terminater string */
    *str++ = '\0';
  }//END bar health
}

/* creates a note with given text for address */
/* type is one of NOTE_XXX types that selects the spool */
void do_hal( char* to, char* subject, char* text, int type ){
  CHAR_DATA* vch;
  NOTE_DATA *note;

  char* sender = "`6Hermes``";
  char* strtime;

  if (IS_NULLSTR(to)){
    bug("do_hal: NULL to passed.", 0);
    return;
  }
  if (IS_NULLSTR(subject)){
    bug("do_hal: NULL subject passed.", 0);
    return;
  }
  if (IS_NULLSTR(text)){
    bug("do_hal: NULL text passed.", 0);
    return;
  }


  note = new_note();
  note->prefix = 0;
  note->next = NULL;
  note->type = type;
  note->sender = str_dup( sender );
  note->subject = str_dup( subject );
  note->text = str_dup( text );
  strtime                         = ctime( &mud_data.current_time );
  strtime[strlen(strtime)-1]      = '\0';
  note->date                 = str_dup( strtime );
  note->date_stamp           = mud_data.current_time;
  note->to_list = str_dup(to);
  append_note(note);
  /*  Let char know he/they recived a note */
  for ( vch = player_list; vch != NULL; vch = vch->next_player ){
    if (is_note_to(vch, note)){
      act_new("`8A messenger hands you a scroll marked $t``\n\r", vch,
	      type == NOTE_IDEA ? "\"idea\""  :
	      type == NOTE_NOTE ? "\"note\""  :
	      type == NOTE_APPLICATION ? "\"application\""  :
	      type == NOTE_PENALTY ? "\"penalty\""  :
	      type == NOTE_NEWS ? "\"news\""  :
	      type == NOTE_CHANGES ? "\"changes\""  : "\"note\"",
	      NULL, TO_CHAR, POS_DEAD);
    }
  }
}

/* trap function for resetting traps in a given room (DUMMY TRAP) */
bool dummy_trap( TRAP_DATA* pt ){
  ROOM_INDEX_DATA* room;
  OBJ_DATA* pObj;
  EXIT_DATA* pExit;
  TRAP_DATA* pTrap;

  int i = 0;
  int j = 0;
  bool fDelay = pt->value[4];

  for (i = 0; i < 4; i++){
    if ( (room = get_room_index( pt->value[i])) == NULL)
      continue;
    else{
/* OBJECTS */
      for (pObj = room->contents; pObj != NULL; pObj = pObj->next_content){
	for (pTrap = pObj->traps; pTrap != NULL; pTrap = pTrap->next_trap){
	  if (pTrap->type == TTYPE_DUMMY && !IS_TRAP( pTrap, TRAP_REARM))
	    continue;
	  if (!fDelay || IS_TRAP(pt, TRAP_DELAY))
	    pTrap->armed = TRUE;
	}
      }
/* EXITS */
      for (j = 0; j < MAX_DOOR; j++){
	if ( (pExit = room->exit[j]) == NULL)
	  continue;
	else{
	  for (pTrap = pExit->traps; pTrap != NULL; pTrap = pTrap->next_trap){
	    if (pTrap->type == TTYPE_DUMMY && !IS_TRAP( pTrap, TRAP_REARM))
	      continue;
	    if (!fDelay || IS_TRAP(pt, TRAP_DELAY))
	      pTrap->armed = TRUE;
	  }
	}
      }
    }
  }
  return FALSE;
}




/* trap function for creating mobs and making them attack the victim */
/* used for MOB trap */
bool mob_trap( CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj, TRAP_DATA* pt, int type){
  MOB_INDEX_DATA* pIndex;
  CHAR_DATA* pMob1;
  CHAR_DATA* pMob2;
  bool fSave = FALSE;
  int dam_type = DAM_OTHER;
  AFFECT_DATA af;

  if (victim->in_room == NULL)
    return FALSE;

  if (IS_TRAP(pt, TRAP_SAVES)){
    if (saves_spell( pt->level, victim, dam_type, SPELL_AFFLICTIVE )){
      if (!IS_TRAP(pt, TRAP_SILENT)){
	act("Sensing $t's aura you hurt the creatures badly as they appear!",
	    victim, pt->name, NULL, TO_CHAR);
      }
      fSave = TRUE;
    }
  }

/* perp the timer */
  af.type	= gsn_timer;
  af.level	= pt->level;
  af.duration	= pt->value[4];
  af.where	= TO_NONE;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;

  if (pt->value[0]){
    if ( (pIndex = get_mob_index( pt->value[0] )) == NULL){
      bug("mob_trap: could not load mob index %d", pt->value[0]);
      return FALSE;
    }
    if ( (pMob1 = create_mobile( pIndex )) == NULL){
      bug("mob_trap: could not load mob %d", pt->value[0]);
      return FALSE;
    }
    /* make sure we set trust up so they don't trip firestorm etc */
    pMob1->trust = 6969;
    /* scale hp */
    pMob1->hit = pt->value[1] * pMob1->hit / 100;
    pMob1->max_hit = pMob1->hit;
    pMob1->level = pt->level;
    if (fSave)
      pMob1->hit /= 2;
    affect_to_char(pMob1, &af);
    char_to_room(pMob1, victim->in_room);
    pMob1->trust = pMob1->level;
    if (!IS_TRAP(pt, TRAP_SILENT))
    act("Suddenly $n appears!", pMob1, NULL, NULL, TO_ROOM);
  }
  else
    pMob1 = NULL;

  /* load and move second mob */
  if (pt->value[2]){
    if ( (pIndex = get_mob_index( pt->value[2] )) == NULL){
      bug("mob_trap: could not load mob index %d", pt->value[0]);
      return FALSE;
    }
    if ( (pMob2 = create_mobile( pIndex )) == NULL){
      bug("mob_trap: could not load mob %d", pt->value[2]);
      return FALSE;
    }
    /* make sure we set trust up so they don't trip firestorm etc */
    pMob2->trust = 6969;
    /* scale hp */
    pMob2->hit = pt->value[3] * pMob2->hit / 100;
    pMob2->max_hit = pMob2->hit;
    pMob2->level = pt->level;
    if (fSave)
      pMob2->hit /= 2;
    affect_to_char(pMob2, &af);
    char_to_room(pMob2, victim->in_room);
    pMob2->trust = pMob2->level;
    if (!IS_TRAP(pt, TRAP_SILENT))
      act("Suddenly $n appears!", pMob2, NULL, NULL, TO_ROOM);
  }
  else
    pMob2 = NULL;
/* make both of them attack the person */
  if (pMob1)
    multi_hit(pMob1, victim, TYPE_UNDEFINED);
  if (pMob2)
    multi_hit(pMob2, victim, TYPE_UNDEFINED);
  return TRUE;
}


/* trap function for casting spells */
/* standard workhorse function that causes damage from traps */
bool spell_trap( CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj, TRAP_DATA* pt, int type){
  int level = pt->level;
  int dam_type = 0;


/* scale level by level difference */
  level += URANGE(-10, (pt->level - victim->level), 10) / 2;

  if (type == TTYPE_SPELL){
    dam_type  = pt->value[4];
  }
  else
    dam_type = DAM_OTHER;

/* saves */
  if (IS_TRAP(pt, TRAP_SAVES)){
    if (saves_spell( pt->level, victim, dam_type, SPELL_AFFLICTIVE )){
      if (!IS_TRAP(pt, TRAP_SILENT)){
	act("Sensing $t's magical aura you partialy resist its effect.",
	    victim, pt->name, NULL, TO_CHAR);
      }
      level -= 5;
    }
  }

/* do the spells */
  if (type == TTYPE_SPELL){
    if (skill_table[pt->value[0]].spell_fun != NULL)
      (*skill_table[pt->value[0]].spell_fun)
	(pt->value[0], UMAX(0, level + pt->value[1]), ch, victim, TARGET_CHAR);
    if (victim == NULL || victim->in_room == NULL)
      return TRUE;
    /* check if we can cast again */
    if (!is_safe_quiet(ch, victim)
	&& skill_table[pt->value[2]].spell_fun != NULL)
      (*skill_table[pt->value[2]].spell_fun)
	(pt->value[2], UMAX(0, level + pt->value[3]), ch, victim, TARGET_CHAR);
    return TRUE;
  }
  return TRUE;
}


/* standard workhorse function that causes damage from traps */
bool damage_trap( CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj, TRAP_DATA* pt, int type){
  int dam = 0;
  int dt = attack_lookup("slash");
  int dam_type = DAM_SLASH;
  int hroll = pt->level;

/* get detailst first */
  switch (type ){
  default:
  case TTYPE_DAMAGE:
    dt = pt->value[0];
    dam_type = attack_table[dt].damage;
    dam  = pt->value[1] + dice(pt->value[2], pt->value[3]);
    /* make it no_blockhit */
    dt += TYPE_NOBLOCKHIT;
    break;
  case TTYPE_XDAMAGE:
    dt = pt->value[0];
    dam_type = pt->value[4];
    dam  = pt->value[1] + dice(pt->value[2], pt->value[3]);
    break;
  }

/* scale damage by level difference */
  dam += URANGE(-10, (pt->level - victim->level), 10) * dam / 20;

/* saves */
  if (IS_TRAP(pt, TRAP_SAVES)){
    if (saves_spell( pt->level, victim, dam_type, SPELL_AFFLICTIVE )){
      if (!IS_TRAP(pt, TRAP_SILENT)){
	act("Sensing $t's magical aura you partialy resist its effect.",
	    victim, pt->name, NULL, TO_CHAR);
      }
      dam /= 2;
    }
  }
/* do the damage */
  virtual_damage(ch, victim, obj, dam, dt, dam_type, hroll, pt->level, FALSE,
		 obj ? TRUE : FALSE, 0);
  return TRUE;
}

/* guts of the traps this thing checks for pk, scales level and
   works the traps depending on their type
*/
bool trap_damage( CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj, TRAP_DATA* pt){
  CHAR_DATA* vch, *vch_next;
  int chance = number_percent() < get_skill(victim, gsn_dodge);
  char* msg = obj ? obj->short_descr : ch->short_descr;
  bool fRet = FALSE;

  CHAR_DATA* ch_buf = ch->fighting;
  int ch_pos = ch->position;
  CHAR_DATA* vic_buf = victim->fighting;
  int vic_pos = victim->position;

  if (!pt->armed)
    return FALSE;
  if (is_safe_quiet(ch,  victim))
    return FALSE;

/* no we've sprung the trap */
  if (!IS_TRAP(pt, TRAP_SILENT)){
    char strln[] = "\n\r";
    if (IS_NULLSTR( pt->echo ))	act("$t springs!", victim, msg, NULL, TO_CHAR);
    else			act( pt->echo, victim, strln, ch, TO_CHAR );

    if (IS_NULLSTR( pt->oEcho ))act("$n springs $t!", victim, msg, NULL, TO_ROOM);
    else			act( pt->oEcho, victim, strln, ch, TO_ROOM );
    WAIT_STATE(victim, PULSE_VIOLENCE);
  }
  if (pt->owner && IS_TRAP(pt, TRAP_NOTIFY)){
    act_new("$N just got nailed by $t.", pt->owner, pt->name, victim,  TO_CHAR, POS_DEAD);
  }

/* disarm the trap */
  pt->armed = FALSE;
/* if this is non-owned trap and was delay, we remove delay bit so it
   doesn't rearm till next init */
  if (pt->owner == NULL)
    REMOVE_BIT(pt->flags, TRAP_DELAY);
  /* dodge check */
  if (!IS_TRAP(pt, TRAP_NODODGE) && chance){
    /* chance is now all up to dex */
    chance = 25 + 10 * (get_curr_stat(victim, STAT_DEX) - 20);
    chance += URANGE(-8, (victim->level - pt->level), 8) * 5;
    if (number_percent() < chance){
      check_improve(victim, gsn_dodge, TRUE, 1);
      if (!IS_TRAP(pt, TRAP_SILENT)){
	act("At the last moment you dodge the trap!", victim, NULL, NULL, TO_CHAR);
	act("At the last moment $n dodges the trap!", victim, NULL, NULL, TO_ROOM);
      }
      check_improve(victim, gsn_dodge, TRUE, 1);
      /* makesure to remove owner traps */
      if (!pt->armed && pt->owner && !IS_TRAP(pt, TRAP_PERMAMENT)){
	extract_trap( pt );
      }
      return FALSE;
    }
    else
      check_improve(victim, gsn_dodge, FALSE, 1);
  }
  /* end dodge check */
  /* spring the trap, on group if neccessary */
  vch = victim->in_room->people;
  for (; vch; vch = vch_next){
    bool fHit = FALSE;
    vch_next = vch->next_in_room;

/* normal traps only hit the victim, others hit group mates */
    if (!IS_TRAP(pt, TRAP_AREA) && vch != victim)
      continue;
    if (!is_same_group(vch, victim) ||  is_safe_quiet(ch,  vch))
      continue;

    switch( pt->type ){
    default:
      fHit = FALSE;					break;
    case TTYPE_DAMAGE:
      fHit = damage_trap(ch, vch, obj, pt, pt->type);	break;
    case TTYPE_XDAMAGE:
      fHit = damage_trap(ch, vch, obj, pt, pt->type);	break;
    case TTYPE_SPELL:
      fHit = spell_trap(ch, vch, obj, pt, pt->type);	break;
    case TTYPE_MOB:
      fHit = mob_trap(ch, vch, obj, pt, pt->type);	break;
    case TTYPE_DUMMY:
      fHit = dummy_trap(pt );				break;
    }

/* check if we change return value */
    if (fHit)
      fRet = TRUE;
  }

/* check if we should remove the trap */
  if (!pt->armed && pt->owner && !IS_TRAP(pt, TRAP_PERMAMENT)){
    extract_trap( pt );
  }
  if (ch->fighting)
    ch->fighting = ch_buf;
  ch->position = ch_pos;
  if (victim->fighting)
    victim->fighting = vic_buf;
  victim->position = vic_pos;
  return fRet;
}

/* a singular test of an armed trap, returns TRUE if	*
 * it was set off					*/
bool trip_trap( CHAR_DATA* Victim, TRAP_DATA* pt){
  CHAR_DATA* ch;
  CHAR_DATA* victim = Victim;
  OBJ_DATA* oprox = NULL;
  bool fMob = FALSE;
  bool fObj = FALSE;
  bool fRet = FALSE;

  if (pt == NULL)
    return FALSE;
  if (Victim->in_room == NULL)
    return FALSE;

/* charmie switch */
  if (IS_NPC(Victim) && IS_AFFECTED(Victim, AFF_CHARM) && Victim->master
      && !IS_NPC(Victim->master))
    victim = Victim->master;
  else
    victim = Victim;

  if (IS_NPC(victim) && victim->desc == NULL)
    return FALSE;
  else
    ch = pt->owner;

/* do a quick test here for pk on owners if any so we don't do that below */
  if (pt->owner && (is_safe_quiet(pt->owner, victim) || victim == pt->owner))
    return FALSE;

/* check for delay trap, since we do not do much on those when not armed */
  if (IS_TRAP(pt, TRAP_DELAY) && !pt->armed){
    pt->armed = TRUE;
    if (pt->owner && IS_TRAP(pt, TRAP_NOTIFY))
      act_new("$N just armed $t.", pt->owner, pt->name, victim, TO_CHAR, POS_DEAD);
    return FALSE;
  }

/* here we test if we need to create a trap-proxy mob
for cases of no-owner or trap-proxy object for nice
looking damage messages for owner */
/*no owner, create a mob proxy */
  if (ch == NULL){
    ch = create_mobile( get_mob_index( MOB_VNUM_TRAP_PROXY ));
    if (ch == NULL){
      bug("trip_trap: could not load the mob proxy.", 0);
      return FALSE;
    }
    /* all we do is set name and pass it on */
    free_string(ch->short_descr);
    ch->short_descr = str_dup( pt->name );
    free_string(ch->name);
    ch->name = str_dup( pt->name );
    ch->trust = 6969;
    char_to_room( ch, victim->in_room);
    fMob = TRUE;
  }
  else{
    oprox = create_object( get_obj_index( OBJ_VNUM_TRAP_PROXY ), pt->level);
    if (oprox == NULL){
      bug("trip_trap: could not load the obj proxy.", 0);
      return FALSE;
    }
    /* all we do is set name and pass it on */
    free_string(oprox->short_descr);
    oprox->short_descr = str_dup( pt->name );
    obj_to_room(oprox, victim->in_room);
    fObj = TRUE;
  }
/* and now we pass it onto the really important function */
  fRet =  trap_damage(ch, victim, oprox, pt);

/* clean up, cleanup, everybody clean up */
  if (fMob){
    if ( ch && ch->in_room ){
      char_from_room( ch );
      extract_char( ch, TRUE );
    }
  }
  if (fObj){
    obj_from_room( oprox );
    extract_obj( oprox );
  }
  return fRet;
}


/* universal check for an object or exit	*
 * checks for armed traps and trips them untill *
 * all are disarmed, TRUE if a trap was tripped	*/
bool trip_traps(CHAR_DATA* ch, TRAP_DATA* list ){
  TRAP_DATA* pt, *pt_next;
  bool fTrip = FALSE;

  if (IS_IMMORTAL(ch) && (ch->invis_level || ch->incog_level))
    return FALSE;

  for (pt = list; pt != NULL; pt = pt_next){
    pt_next = pt->next_trap;
/* delay traps are run when not armed, but trip_trap wont return true */
    if ((pt->armed || IS_TRAP(pt, TRAP_DELAY)) && trip_trap(ch, pt))
      fTrip = TRUE;
  }
  return fTrip;
}


/* There are followings ways to mount

1) You have a mob that follows you with ACT_MOUNT set
2) You have a gen_mount with its modifier set to mobindex of mob to use.

*/

void do_mount(CHAR_DATA *ch, char *argument){
  AFFECT_DATA* paf;

  CHAR_DATA *victim;


  if (IS_NPC(ch))
    return;

  if (ch->pcdata->pStallion != NULL){

    sendf(ch, "You are already mounted on %s!\n\r",ch->pcdata->pStallion->short_descr);
    return;
  }
  if ( IS_NULLSTR(argument)){
    MOB_INDEX_DATA* pIndex = NULL;
    if ((paf = affect_find(ch->affected, gsn_mounted)) == NULL){
      send_to_char("Mount what?\n\r", ch);
      return;
    }
    else if ( (pIndex = get_mob_index(paf->modifier)) == NULL
	      || !IS_SET(pIndex->act, ACT_MOUNT)){
      sendf(ch, "You can't mount %s.\n\r", pIndex->short_descr);
      return;
    }
    else if (pIndex->size < ch->size ){
      sendf( ch, "You would not fit upon %s!", pIndex->short_descr);
      return;
    }
    else if ( (IS_UNDEAD(ch) && !IS_SET(pIndex->act, ACT_UNDEAD))
	      || (IS_DEMON(ch) && pIndex->race != race_lookup("demon")) ){
      sendf(ch, "As you near it, %s bolts in fear!\n\r", pIndex->short_descr);
      act("As $n nears it, $t bolts in fear!", ch, pIndex->short_descr, NULL, TO_ROOM);
      affect_strip( ch, gsn_mounted );
      return;
    }
    act("You mount upon the back of $t.",ch,pIndex->short_descr, NULL,TO_CHAR);
    act("$n mounts on the back of $t.",ch,pIndex->short_descr, NULL,TO_ROOM);

    ch->pcdata->pStallion = pIndex;
  }
  else{
    if ( ( victim = get_char_room( ch, NULL, argument ) ) == NULL ){
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }
    else if (!IS_NPC(victim) || !IS_SET(victim->act, ACT_MOUNT)){
      send_to_char("You can't mount them.\n\r",ch);
      return;
    }
    else if (victim->size < ch->size){
      act("You would not fit upon $N!.", ch, NULL, victim, TO_CHAR);
      return;
    }
    if (victim->summoner != ch || victim->master != ch || victim->leader != ch){
      send_to_char("They do not seem to belong to you.\n\r",ch);
      return;
    }
    else if ( (IS_UNDEAD(ch) && !IS_UNDEAD(victim))
	      || (IS_DEMON(ch) && !IS_DEMON(victim)) ){
      act( "As you near it, $N bolts in fear!", ch, NULL, victim, TO_CHAR);
      act("As $n nears it, $N bolts in fear!", ch, NULL, victim, TO_ROOM);
      stop_follower( victim );
      extract_char(victim, TRUE);
      return;
    }
    act("You mount upon the back of $N.",ch,NULL,victim,TO_CHAR);
    act("$n mounts on the back of $N.",ch,NULL,victim,TO_ROOM);

    ch->pcdata->pStallion = victim->pIndexData;
  }
}

void do_dismount(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
	return;
    if (ch->pcdata->pStallion != NULL)
    {
	act("You dismount from $t.",ch,ch->pcdata->pStallion->short_descr, NULL,TO_CHAR);
	act("$n dismounts from $t.",ch,ch->pcdata->pStallion->short_descr, NULL,TO_ROOM);
	ch->pcdata->pStallion = NULL;
	return;
    }
    else
    {
      send_to_char("You are not mounted on anything.\n\r",ch);
      return;
    }
}

/* CHALLENGE FUNTIONS */
CHALLENGE_DATA* chal_list;

void add_challenge( CHALLENGE_DATA* chal ){
  if (chal_list == NULL){
    chal_list = chal;
    chal->next = NULL;
  }
  else{
    /* check if there is already a challenge for this character*/
    chal->next = chal_list;
    chal_list = chal;
  }
}

bool rem_challenge( CHALLENGE_DATA* chal ){
  CHALLENGE_DATA* prev = chal_list;

  if (chal_list == NULL){
    bug("rem_chal: chal_list null.", 0);
    return FALSE;
  }
  if (prev == chal )
    chal_list = chal->next;
  else{
    while (prev->next && prev->next != chal){
      prev = prev->next;
    }
  }
  if (prev == NULL){
    bug("rem_chal: chal not found.", 0);
    return FALSE;
  }
  prev->next = chal->next;
  chal->next = NULL;
  return TRUE;
}

const bool REMOVE_ALL_OLD_CHALLENGES = FALSE;
//removes useless challenges
void prune_chall(){
  CHALLENGE_DATA* cur_chal = chal_list, *cur_chal_next, *chal, *chal_next;
  struct stat Stat;
  char pFile[MIL];
  char* curr_name;

  for (;cur_chal; cur_chal = cur_chal_next){
    int records = 0;
    cur_chal_next = cur_chal->next;
    curr_name = cur_chal->name;

    //check if the pfile exists, if it does, continue
    sprintf(pFile, PLAYER_DIR"%s", curr_name);
    if ( -1 != stat( pFile, &Stat)){
      continue;
    }

    //we have a non existant player, total up their records
    for (chal = chal_list; chal; chal = chal->next){
      if (str_cmp(curr_name, chal->name))
	continue;
      else
	records += chal->win + chal->loss + chal->tie;
    }
    //we remove the records if there is less then 50 of them
    //on a non existant pfile
    if (records >= 50 && !REMOVE_ALL_OLD_CHALLENGES)
      continue;
    nlogf("prune_chall: %s's challenges removed (%d challenges)",curr_name, records);
    //remove all the challenges with curr_name
    for (chal = chal_list; chal; chal = chal_next){
      chal_next = chal->next;
      if (str_cmp(curr_name, chal->name))
	continue;
      else
	rem_challenge( chal );
    }
    //restart the main list
    cur_chal = chal_list;
  }
}

/* returns challenge data by name */
CHALLENGE_DATA* get_chal( char* name, char* record ){
  CHALLENGE_DATA* chal = chal_list;

  for (;chal; chal = chal->next){
    if (str_cmp(name, chal->name))
      continue;
    else if (!IS_NULLSTR(record) && str_cmp(chal->record, record))
      continue;
    return chal;
  }
  return NULL;
}

/* adds/updates a challenge */
void update_challenge( CHAR_DATA* ch, char* name, char* record, int win, int loss, int tie, int refused ){
  CHALLENGE_DATA* chal;

  if (IS_NULLSTR( name ) || IS_NULLSTR( record ))
    return;
  else if ( (chal = get_chal( name, record )) == NULL){
    chal = new_challenge();
    chal->name	= str_dup( name );
    chal->record= str_dup( record );
    chal->win = win;
    chal->loss = loss;
    chal->tie =  tie;
    chal->refused = refused;
    add_challenge( chal );
  }
  else{
    chal->win = UMAX(0, chal->win + win);
    chal->loss = UMAX(0, chal->loss + loss);
    chal->tie = UMAX(0, chal->tie + tie);
    chal->refused = UMAX(0, chal->refused + refused);
  }

  if (ch && !IS_NPC(ch) && ch->pCabal != NULL && IS_SET(ch->pCabal->progress, PROGRESS_CHALL)){
    ch->pcdata->member->kills += refused + UMAX(1, chal->win + chal->loss + chal->tie);
    CHANGE_CABAL( get_parent(ch->pCabal) );
    save_cabals( TRUE, NULL );
  }
  save_challenges();
}

/* writes a single challenge */
void fwrite_challenge( FILE* fp, CHALLENGE_DATA* chal ){
  fprintf( fp, "#CHAL %s~ %s~ %d %d %d %d\n",
	   chal->name,
	   chal->record,
	   chal->win,
	   chal->loss,
	   chal->tie,
	   chal->refused);
}

/* reads a single challenge */
void fread_challenge( FILE* fp, CHALLENGE_DATA* chal ){
  chal->name	=	fread_string( fp );
  chal->record	=	fread_string( fp );
  chal->win	=	fread_number( fp );
  chal->loss	=	fread_number( fp );
  chal->tie	=	fread_number( fp );
  chal->refused	=	fread_number( fp );
}

/* saves all challenges */
void save_challenges(){
  FILE *fp;
  CHALLENGE_DATA* chal = chal_list;
  char path[128];

  fclose( fpReserve );
  sprintf( path, "%s%s", CHALLENGE_DIR, CHALLENGE_FILE);
  if ( ( fp = fopen( path, "w" ) ) == NULL ){
    fp = fopen( NULL_FILE, "r" );
    fclose (fp);
    perror( path );
    return;
  }
/* loop through challenges and save */
  for (; chal; chal = chal->next ){
    fwrite_challenge( fp, chal );
  }
  fprintf( fp, "#END\n" );
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
}

/* reads all challenges */
void load_challenges(){
  CHALLENGE_DATA* chal;
  FILE *fp;
  char path[128];
  char letter;
  char* word;

  fclose( fpReserve );
  sprintf( path, "%s%s", CHALLENGE_DIR, CHALLENGE_FILE);
  if ( ( fp = fopen( path, "r" ) ) == NULL ){
    fp = fopen( NULL_FILE, "r" );
    fclose (fp);
    perror( path );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }

  for (;;){
    letter		= fread_letter( fp );
    if ( letter != '#' ){
      bug( "load_challenge: # not found.", 0 );
      exit( 1 );
    }
    word   = feof( fp ) ? "END" : fread_word( fp );
    if ( !str_cmp(word, "END"))
      break;
    else if (!str_cmp(word, "CHAL")){
      chal = new_challenge();
      fread_challenge( fp, chal );
      add_challenge( chal );
    }
  }
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );

  prune_chall();
}

/* shows a challenge record */
void do_record( CHAR_DATA* ch, char* argument ){
  CHALLENGE_DATA* chal;
  BUFFER* buffer;
  char buf[MIL];
  bool fFound = FALSE;

  int tot_ref = 0;
  int tot_fight = 0;

  if (IS_NULLSTR(argument)){
    send_to_char("Who's challenge record would you like to view?\n\r", ch);
    return;
  }
  else if ( (chal = get_chal( argument, "" )) == NULL){
    send_to_char("No record exists of that challenger.\n\r", ch);
    return;
  }

  buffer = new_buf();
  sprintf( buf, "Name            Won    Lost   Refused\n\r");
  add_buf( buffer, buf );
  sprintf( buf, "-------------------------------------\n\r");
  add_buf( buffer, buf );
  for (chal = chal_list ; chal ; chal = chal->next ){
    if (str_cmp(chal->name, argument))
      continue;
    sprintf(buf, "%-15s:  %-2d    %-2d     %-2d\n\r", chal->record, chal->win, chal->loss, chal->refused);
    add_buf( buffer, buf );
    fFound = TRUE;
    tot_ref += chal->refused;
    tot_fight += chal->win;
    tot_fight += chal->loss;
    tot_fight += chal->tie;
  }
  sprintf (buf, "Fought %d        Refused: %d\n\r", tot_fight, tot_ref );
  add_buf( buffer, buf );
  if (!fFound){
    send_to_char("Record indicates no challenges as of yet.\n\r", ch);
  }
  else
    page_to_char(buf_string(buffer),ch);
  free_buf(buffer);
}


/*
   creates spirit mob, and generates its path to the temple storing it
   int ->spec_data
*/
void create_spirit_mob( ROOM_INDEX_DATA* from, int to_room, char* name ){
  MOB_INDEX_DATA* pIndex;
  CHAR_DATA* spirit;
  ROOM_INDEX_DATA* to;
  PATH_QUEUE* path;
  char buf[MIL];
  int dist = 0;

  if (from == NULL || IS_NULLSTR(name))
    return;
  else if(( pIndex = get_mob_index(MOB_VNUM_PLAYER_SPIRIT)) == NULL)
    return;

  /* check for other spirits of this player before makign one */
  sprintf( buf, pIndex->player_name, name );
  if (get_char( buf) != NULL)
    return;

  /* try for the path first before making a spirit */
  if ( (to = get_room_index( to_room )) == NULL)
    return;

  path = generate_path( from, to, 128, TRUE, &dist, NULL);
  clean_path();

  if (path == NULL)
    return;

  if ( (spirit = create_mobile( pIndex)) == NULL)
    return;

  /* set the path, short/long,  set special names */
  spirit->spec_path = path;

  sprintf( buf, spirit->name, name );
  free_string( spirit->name );
  spirit->name = str_dup( buf );

  sprintf( buf, spirit->short_descr, name );
  free_string( spirit->short_descr );
  spirit->short_descr = str_dup( buf );

  sprintf( buf, spirit->long_descr, name );
  free_string( spirit->long_descr );
  spirit->long_descr = str_dup( buf );

  spirit->trust = 6969;
  char_to_room( spirit, from );

  act("With a faintly audible sigh $n appears in the room.", spirit, NULL, NULL, TO_ALL );
  spirit->trust = spirit->level;
}

void do_reputation( CHAR_DATA* ch, char* argument ){
  CHAR_DATA* victim;
  char* str;
  int race;

  if (IS_NULLSTR(argument))
    victim = ch;
  else if ( (victim = get_char_world(ch, argument)) == NULL){
    send_to_char("They aren't about.\n\r", ch);
    return;
  }
  else if (IS_NPC(victim)){
    send_to_char("Why would you wish to know reputation of such mindless creature.\n\r", ch);
    return;
  }

  if (ch == victim)
    send_to_char("Your current reputations:\n\r", ch);
  else
    send_to_char("Their current reputations:\n\r", ch);
  for (race = 1; race < MAX_PC_RACE; race++){
    if (pc_race_table[race].show != TRUE)
      continue;
    int per = 100 * victim->pcdata->race_kills[race] / AOS_LEVEL;

    if (per < 1)	str = "excellent";
    else if (per < 25)	str = "good";
    else if (per < 50)	str = "neutral";
    else if (per < 75)	str = "bad";
    else if (per < 100)	str = "awful";
    else		str = "`!hate``";

    sendf(ch, "%-15.15s: %s\n\r", capitalize( pc_race_table[race].name ), str );
  }
}

//checks if the character's ip has voted for all choices
bool hasVoted( DESCRIPTOR_DATA* d ){
  FILE* fp;
  char path[MIL];
  unsigned char b;

  //disabled
  return TRUE;

  if (mud_data.mudport == TEST_PORT)
    return TRUE;
  else if (IS_NULLSTR(d->ident))
    return FALSE;
  else
    sprintf( path, "%s%s", VOTE_DIR, d->ident );

  if ( (fp = fopen( path, "r")) == NULL)
    return FALSE;

  /* Useless conditional */
  if( fread( &b, 1, 1, fp ) <= 0 )
    fclose( fp );
  else
    fclose( fp );

  if (b >= 6)
    return TRUE;
  else
    return FALSE;
}

