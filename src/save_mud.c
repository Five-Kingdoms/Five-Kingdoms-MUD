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
#include "recycle.h"
#include "save_mud.h"
#include "vote.h"
#include "cabal.h"

/***************************************************************/
/*Following is mud status saving routines for Forsaken Lands  */
/*mud created by Virigoth circa 2001.  Copyrighted for Forsaken*/
/*Lands mud Nov 1, 2001.  Do not use or copy without explicit */
/*permission of author. (Voytek Plawny aka Virigoth)	       */
/***************************************************************/



/* sets current mud day/year based on Post-Cataclysm counting where
 * TIME_CATACLYSM is the real life time when cataclysm occured.
 *
 * Should be called ONLY when mud boots up to synchronize time.
 * Hours are NOT synchronize to perserve continous flow of mud time
 * across reboots.
 */
void update_mud_time( long real_time ){
  int buf;

  /* set mud date */
  //we use dummy for hour, in order to keep the hour saved before reboot
  rltom_date(real_time,
	     &buf,
	     &mud_data.time_info.day,
	     &mud_data.time_info.month,
	     &mud_data.time_info.year);

}


/* writes the mud data into the save file passed */
bool fwrite_mud( FILE* fp ){
  VOTE_DATA* pv;

/* version first THIS HAS TO BE FIRST*/
  fprintf( fp, "VERSION %d\n", SAVEMUD_VER );

/* lock status */
  if (mud_data.wizlock)
    fprintf( fp, "WIZLOCK\n");
  if (mud_data.newlock)
    fprintf( fp, "NEWLOCK\n");

/* system time data */
  fprintf( fp, "t_crash %d\n", (int) mud_data.crash_time );
  fprintf( fp, "t_save %d\n", (int)  mud_data.save_time );

/* mud time data */
  fprintf( fp, "t_mud %d %d %d %d\n",
	   mud_data.time_info.hour,
	   mud_data.time_info.day,
	   mud_data.time_info.month,
	   mud_data.time_info.year);

/* mud weather info */
  fprintf( fp, "weather %d %d %d %d\n",
	   mud_data.weather_info.mmhg,
	   mud_data.weather_info.change,
	   mud_data.weather_info.sky,
	   mud_data.weather_info.sunlight);

/* statistical info */
  fprintf( fp, "max_on %d\n", mud_data.max_on );
  fprintf( fp, "deaths %d\n", mud_data.deaths );
  fprintf( fp, "pfiles %d\n", mud_data.pfiles );
  fprintf( fp, "bounties %d\n", mud_data.bounties );
  fprintf( fp, "sieges %d\n", mud_data.siege_iden );

  fprintf( fp, "Questor %s~ %d %ld\n", mud_data.questor, UMAX(1, mud_data.max_quest), mud_data.last_questor );

/* votes */
  for (pv = vote_list; pv != NULL; pv = pv->next ){
    fprintf( fp, "Vote\n" );
    write_vote( fp, pv );
  }

/* terminator */
  fprintf( fp, "END\n");
  return TRUE;
}

/* opens the SAVEMUD_FILE for write calls the save functions, */
/* closes it when done					      */
void save_mud(){
  FILE *fp;
  char path[MIL];

  log_string("Saving Mud Data...");
  fclose(fpReserve);
  sprintf(path, "%s%s",SAVEMUD_DIR, SAVEMUD_FILE);
  if ((fp = fopen(path,"w")) == NULL){
    bug("save_mud: could not open SAVEMUD_FILE for write.", 0);
    perror(path);
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }
/* about to save print the time etc*/
  strcpy( mud_data.str_save_time, ctime( &mud_data.current_time ) );
  mud_data.save_time = mud_data.current_time;

/* have save file to be written too */
  if (!fwrite_mud( fp )){
    bug("save_mud: error occured while saving mud data. Removing savefile.", 0);
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    remove( path );
    log_string("Saving Mud Data: FAILURE...");
    return;
  }

/* close the file */
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
}

/* resets the global mud data to initial values */
void reset_mdata(){

/* bools */
  mud_data.wizlock	= FALSE;
  mud_data.newlock	= FALSE;

/* statistics */
  mud_data.max_on	= 0;
  mud_data.deaths	= 0;
  mud_data.pfiles	= 0;
  mud_data.bounties	= 0;
  mud_data.siege_iden	= 0;
  mud_data.tot_bastion	= 1;

/* time */
  log_string("Initializing MUD system time...");
  mud_data.crash_time = mud_data.current_time;
  mud_data.save_time = 0;

/* strings*/
  log_string("Initializing MUD system strings...");
  strcpy( mud_data.str_boot_time, ctime( &mud_data.crash_time ) );
  strcpy( mud_data.str_save_time, ctime( &mud_data.save_time ) );

/* mud time */
  log_string("Initializing MUD calendar/time...");
  mud_data.time_info.hour	= 0;
  update_mud_time( mud_data.current_time );

  log_string("Initializing MUD weather and lightlevel...");
  if      ( mud_data.time_info.hour <  5 )
    mud_data.weather_info.sunlight = SUN_DARK;
  else if ( mud_data.time_info.hour <  6 )
    mud_data.weather_info.sunlight = SUN_RISE;
  else if ( mud_data.time_info.hour < 19 )
    mud_data.weather_info.sunlight = SUN_LIGHT;
  else if ( mud_data.time_info.hour < 20 )
    mud_data.weather_info.sunlight = SUN_SET;
  else
    mud_data.weather_info.sunlight = SUN_DARK;

  mud_data.weather_info.change	= 0;
  mud_data.weather_info.mmhg	= 960;

  if ( mud_data.time_info.month >= 7 && mud_data.time_info.month <=12 )
    mud_data.weather_info.mmhg += number_range( 1, 50 );
  else
    mud_data.weather_info.mmhg += number_range( 1, 80 );

  if ( mud_data.weather_info.mmhg <=  980 )
    mud_data.weather_info.sky = SKY_LIGHTNING;
  else if ( mud_data.weather_info.mmhg <= 1000 )
    mud_data.weather_info.sky = SKY_RAINING;
  else if ( mud_data.weather_info.mmhg <= 1020 )
    mud_data.weather_info.sky = SKY_CLOUDY;
  else
    mud_data.weather_info.sky = SKY_CLOUDLESS;

/* QUESTS */
  mud_data.questor = str_dup( "" );
  mud_data.max_quest = 1;
  mud_data.last_questor = mud_data.current_time;

  log_string("Initialization complete...");
}


/* read the mud data from the save file passed */
bool fread_mud( FILE* fp ){
  int version = SAVEMUD_VER;
//  char buf[MSL];
  char *word = NULL;
  bool fMatch = FALSE;

  /* Useless conditional */
  if( version != SAVEMUD_VER )
    version = SAVEMUD_VER;

  for ( ; ; ){
    /* check for eof */
    word   = feof( fp ) ? "End" : fread_word( fp );

    if (word == NULL)
      return FALSE;

    /* reset match */
    fMatch = FALSE;

    /* match current word */
    switch ( UPPER(word[0]) ){
    case '*':
      fMatch = TRUE;
      fread_to_eol( fp );
      break;
    case 'B':
      KEY( "bounties", mud_data.bounties, fread_number( fp ) );
      break;
    case 'D':
      KEY( "deaths", mud_data.deaths, fread_number( fp ) );
      break;
    case 'E':
      if (!str_cmp(word, "END"))
	return TRUE;
      break;
    case 'M':
      KEY( "max_on", mud_data.max_on, fread_number( fp ) );
      break;
    case 'N':
      KEY( "NEWLOCK", mud_data.newlock, TRUE );
      break;
    case 'P':
      if (!str_cmp(word, "Pact")){
	load_pact( fp );
	fMatch = TRUE;
	break;
      }
      KEY( "pfiles", mud_data.pfiles, fread_number( fp ) );
      break;
    case 'Q':
      if (!str_cmp(word, "Questor")){
	mud_data.questor	= fread_string( fp );
	mud_data.max_quest	= fread_number( fp );
	mud_data.last_questor	= fread_number( fp );
	mud_data.max_quest = UMAX(1, mud_data.max_quest);
	fMatch = TRUE;
	break;
      }
      break;
    case 'S':
      KEY( "sieges", mud_data.siege_iden, fread_number( fp ) );
    case 'T':
      KEY( "t_crash", mud_data.crash_time, fread_number( fp ) );
      KEY( "t_save", mud_data.save_time, fread_number( fp ) );
      if (!str_cmp(word, "t_mud")){
	mud_data.time_info.hour	= fread_number( fp );
	mud_data.time_info.day	= fread_number( fp );
	mud_data.time_info.month= fread_number( fp );
	mud_data.time_info.year	= fread_number( fp );

	update_mud_time( mud_data.current_time );
	fMatch = TRUE;
	break;
      }
      break;
    case 'V':
      KEY( "VERSION", version, fread_number( fp ) );
      if (!str_cmp(word, "Vote")){
	VOTE_DATA* pv = new_vote();
	read_vote( fp, pv );
	add_vote( pv );
	fMatch = TRUE;
	break;
      }
      break;
    case 'W':
      KEY( "WIZLOCK", mud_data.wizlock, TRUE );
      if (!str_cmp(word, "weather")){
	mud_data.weather_info.mmhg	= fread_number( fp );
	mud_data.weather_info.change	= fread_number( fp );
	mud_data.weather_info.sky	= fread_number( fp );
	mud_data.weather_info.sunlight	= fread_number( fp );
	fMatch = TRUE;
	break;
      }
      break;
    }
    if ( !fMatch ){
      bugf("Fread_mud: [%s] no match.", word);
      fread_to_eol( fp );
    }
  }
  return FALSE;
}

/* opens the SAVEMUD_FILE for read calls the read functions, */
/* closes it when done					      */
void read_mud(){
  FILE *fp;
  char path[MIL];

  log_string("Restoring Mud Data...");
/* look for save file */
  fclose(fpReserve);
  sprintf(path, "%s%s",SAVEMUD_DIR, SAVEMUD_FILE);

  if ((fp = fopen(path,"r")) == NULL){
    bug("read_mud: SAVEMUD_FILE not found, continuing with defaults.", 0);
    perror(path);
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }

/* begin reading */
  if (!fread_mud( fp )){
    bug("read_mud: error occured while reading mud data.", 0);
    log_string("Restoring Mud Data: FAILURE...");
  }

/* close the file */
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
}
