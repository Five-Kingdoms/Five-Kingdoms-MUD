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


#ifndef __MISC_H__
#define __MISC_H__

typedef struct bfs_queue_struct  PATH_QUEUE;
typedef struct area_queue_struct  AREA_QUEUE;

struct bfs_queue_struct{
  ROOM_INDEX_DATA *room;
  PATH_QUEUE *from_path;
  char   from_dir;
  char   dir;
  int	   dist;
  struct bfs_queue_struct *next;
};

struct area_queue_struct{
  AREA_QUEUE* next;

  AREA_DATA *area;
  AREA_QUEUE *from;

  ROOM_INDEX_DATA *start_room;
};

PATH_QUEUE* generate_path( ROOM_INDEX_DATA *src, ROOM_INDEX_DATA *target, int maxdist, bool fPassDoor, int* dist, CABAL_DATA* pc_only);
void clean_path();
void clean_path_queue(PATH_QUEUE* head);
void clean_area_queue(void);

#endif
