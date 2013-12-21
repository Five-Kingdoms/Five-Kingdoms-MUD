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
/*Written by Virigoth sometime circa april 2000 for FORSAKEN LANDS mud.*/
/*This is the implementation of the extended Effects code*/
/* NOT TO BE USED OR REPLICATED WITHOUT EXPLICIT PERMISSION OF AUTHOR */
/* NOTE: names of GEN's have to be non-conflicting with GSN's! */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "interp.h"
#include "effect.h"
#include "jail.h"
#include "olc.h"
#include "db.h"
#include "cabal.h"
#include "vote.h"
#include "armies.h"

extern int amodify_lookup_id(int ident );
extern int wmodify_lookup_id(int ident );
extern bool bounty_covered( CHAR_DATA* victim );
extern bool dprobe_check(CHAR_DATA* ch, CHAR_DATA* victim);

/*---------------------------------------------------------------------------*/
/*This is the skeleton for the EFF_UPDATE function type                      */
/*typedef int EFF_UPDATE_FUN args ( (CHAR_DATA* ch, void* arg1, void* arg2, int gn, int &int1, int &int2, int flag) );                                     */
/*                                                                           */
/*                                                                           */
/*Field explanations:                                                        */
/*ch: main focus of the function, character.                                 */
/*arg1: void pointer should be used for victim, object, room. affect.        */
/*arg2: same as arg1.                                                        */
/*gn: Game Number for gsn or gen of the spell/skill/effect.                  */
/*int1: first integer pointer maybe used to do calculations on damage passed or any other integer values.                                                   */
/*int2: same as int1.                                                        */
/*flag:int flag may be passed into the function.                            */
/*---------------------------------------------------------------------------*/





/*---------------------------------------------------------------------------*/
/*The following is the skeletion for the EFFECT_DATA cell of EFFECT_TABLE
structeffect_type
{
char* name;                 Name of skill

sh_int* pgen;               Pointer to associated gen

char* noun_damage;           Damage message

char*message_off;           StandardExpiry Message to_char
char*message_off2;          StandardExpiry Message to_room

long flags Bitwise flags    (RESERVED)
long flags2 Bitwise flags   (RESERVED)

-------------------------------UPDATE FUNCTIONS-----------------------

    EFFECT_UPDATE_FUN effect_update_fun [MAX_EFFECT_UPDATE];

    Function pointers (each one is NULL or points to effect_update_fun
    declared with DECLARE_EFF_UPDATE_FUN in effect.h for updates
    seperated as follows:
 EFF_UPDATE_NONE          0 //Just in case we need this.
 EFF_UPDATE_INIT          1 //Called to set the effect.
 EFF_UPDATE_KILL          2 //Called when effect is stripped by handler
 EFF_UPDATE_ENTER         3 //On entering a room
 EFF_UPDATE_LEAVE         4 //On leaving the room
 EFF_UPDATE_COMBAT     5 //At the begginging of a combat round
 EFF_UPDATE_POSTCOMBAT    6 //At the end of combat round.
 EFF_UPDATE_PREATTACK     7 //Before attack
 EFF_UPDATE_POSTATTACK    8 //After attack
 EFF_UPDATE_PRESPELL      9 //Before spell is cast.
 EFF_UPDATE_POSTSPELL    10 //After spell (after spell function is run)
 EFF_UPDATE_PRECOMMAND   11 //Before any command
 EFF_UPDATE_POSTCOMMAND  12 //After any command
 EFF_UPDATE_PREDAMAGE    13 //Before any damage is applied
 EFF_UPDATE_POSTDAMAGE   14 //After any damage is applied
 EFF_UPDATE_TICK         15 //On tick
 EFF_UPDATE_MOBTICK      16 //On mob update tick
 EFF_UPDATE_PREDEATH     17 //Right before death
 EFF_UPDATE_POSTDEATH    18 //Right after death
 EFF_UPDATE_PREKILL      19 //Before Kill
 EFF_UPDATE_POSTKILL     20 //Right after kill.
 EFF_UPDATE_PREEXP       21 //Before granting Exp/Gold
 EFF_UPDATE_POSTEXP      22 //After granting Exp/Gold
 EFF_UPDATE_PREVIOLENCE  23 //Before any combat
 EFF_UPDATE_POSTVIOLENCE 24 //After any combat
 EFF_UPDATE_OBJTICK	 25 //When updating objects on tick.
 EFF_UPDATE_OBJTOCHAR    26 //When item is transfered onto CHar.
 EFF_UPDATE_DODGE        27 //On successful dodge.
 EFF_UPDATE_PARRY        28 //On successfull parry/dual parry/monk parry
 EFF_UPDATE_SHBLOCK	 29 //On successfull Shield block.
 EFF_UPDATE_VIOTICK	 30 //On violence update
 EFF_UPDATE_MOVE	 31 //On moving out of a room
------------------------------END UPDATE FUNCTIONS----------------------

sh_intPreBeats;          RESERVED
sh_intbeats;             Waiting time after  use

int value1[MAX_VALUE]    RESERVED
int value2[MAX_VALUE]    RESERVED
int value3[MAX_VALUE]    RESERVED

AFFFECT_DATA* paf;       RESERVED

END OF THE EFFECT_DATA description
-----------------------------------------------------------------------------*/





/////////////////////////----===BEGIN EFFECT TABLE===----////////////////

const EFFECT_DATA   effect_table   [MAX_EFFECTS] =
{
  /* FIRST NULL CELL */
  {
    "reserved",               //NAME
    NULL,                     //Gen number pointer.
    "none",                   //Damage noun.
    "Off msg char",           //Turn off msg to self
    "Off msg room",           //Turn off msg to room.
    0,                     //Bitwise flags 1 (RESERVED)
    0,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      NULL,                   //UPDATE_INIT
      NULL,                   //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      NULL,                   //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,                   //UPDATE_PARRY
      NULL,                   //UPDATE_SHBLOCK
      NULL                   //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//END FIRST NULL CELL


  /* Firestorm */
  {
    "Firestorm",              //NAME
    &gen_firestorm,            //Gen number pointer.
    "Firestorm",               //Damage noun.
    "Your attention diverted, the firestorm collapses.",
    "The swirling vortex of fire weakens and goes out sending tons of ash flying.",
    EFF_F1_NOSAVE | EFF_F1_ALLOW_EXTEND,          //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      firestorm_init,         //UPDATE_INIT
      firestorm_kill,         //UPDATE_KILL
      firestorm_enter,        //UPDATE_ENTER
      firestorm_leave,        //UPDATE_LEAVE
      firestorm_precombat,    //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      NULL,                   //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//END Firestorm


  /* Mana Charge */
  {
    "Mana Pool",           //NAME
    &gen_mana_charge,        //Gen number pointer.
    "mana flare",            //Damage noun.
    "The charge in $p has been all but depleted.",
    "The arcane aura around $p slowly fades.",
    EFF_F1_NOEXTRACT,       //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      mana_charge_init,       //UPDATE_INIT
      mana_charge_kill,       //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      mana_charge_prespell,   //UPDATE_PRESPELL
      mana_charge_postspell,  //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      NULL,                   //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//END Manacharge

  /* Gold Loss (currently used in cutpurse)*/
  {
    "gold loss",             //NAME
    &gen_gold_loss,          //Gen number pointer.
    "",                      //Damage noun.
    "You realize there is a hole in your purse and close it up.", //msg to self
    "$n suddenly clutches his purse and swears loudly.",          //msg to room
    EFF_F1_NONE,           //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      gold_loss_init,         //UPDATE_INIT
      gold_loss_kill,         //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      gold_loss_init,         //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      gold_loss_init,         //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//End Gold Loss.


  /* guise (currently used in disguise)*/
  {
    "title change",                 //NAME
    &gen_guise,              //Gen number pointer.
    "",                      //Damage noun.
    "You once again begin to look and act like your good old self.", //msg to self
    "$n suddenly looks and acts a lot more like himself.",   //msg to room
    EFF_F1_NONE,           //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      guise_init,             //UPDATE_INIT
      guise_kill,             //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      NULL,                   //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//End disguise

  /* study skill for assassinate and others.. */
  {
    "victim study",          //NAME
    &gen_study,              //Gen number pointer.
    "",                      //Damage noun.
    "",
    "",   //msg to room
    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      study_none,             //UPDATE_NONE
      study_init,             //UPDATE_INIT
      study_kill,             //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      study_none,             //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      study_precommand,       //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      study_none,             //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      study_tick,             //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {5},                  //range of study (in rooms)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  }, //End study

  /* insect swarm effect */
  {
    "swarm",      //NAME
    &gen_insect_swarm,       //Gen number pointer.
    "insect swarm",                      //Damage noun.
    "The giant cloud of insects thins out and disperses",
        "The giant cloud of insects thins out and disperses",   //msg to room
    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      insect_swarm_init,      //UPDATE_INIT
      insect_swarm_kill,      //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      insect_swarm_tick,             //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//End swarm

  /* Panic effect */
  {
    "panic",      //NAME
    &gen_panic,                   //Gen number pointer.
    "",                           //dam noun.
    "Your vision returned you manage to calm down.",
    "$n seems to come to his senses and calms down.",
    EFF_F1_NOEXTRACT | EFF_F1_CMDBLOCK,      //Bitwise flags 1 (RESERVED)
    GN_INTERRUPT,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      panic_init,             //UPDATE_INIT
      panic_kill,             //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      panic_postcombat,       //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      panic_precommand,       //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      panic_tick,             //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//End Panic

  /* Malform Weapon */
  {
    "living weapon: malform",      //NAME
    &gen_malform,         //Gen number pointer.
    "evil",               //dam noun.
    "",
    "",
    EFF_F1_NONE | EFF_F1_NOEXTRACT,           //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      malform_none,           //UPDATE_NONE
      malform_init,           //UPDATE_INIT
      NULL,	              //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,		      //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,		      //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      NULL,	              //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      malform_predeath,       //UPDATE_PREDEATH
      NULL,		      //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      malform_postkill,       //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      malform_objtick,        //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,1,3,6,10,18,36,72,120,180, 200,0,0,0,0,0}, //value1 (levels for malform
    {10},                  //value2  //malform MAX LEVEL of the weapon.
    {100},                 //value3  //chance for weapon to weaken.
    NULL                  //void pointer (RESERVED)
  },//End malform

  /* HAS OWNER */
  {
    "has owner",              //NAME
    &gen_has_owner,            //Gen number pointer.
    "attack",                 //Damage noun.
    //Message displayed to char
   "Suddenly $p comes alive and contrary to your efforts falls to the ground.",
    //message to room
    "$n fumbles with $p momentarly and with a curse drops it.",
    EFF_F1_NOEXTRACT,     //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      has_owner_none,         //UPDATE_NONE
      has_owner_init,         //UPDATE_INIT
      has_owner_kill,         //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
//Viri: This cannot work properly      has_owner_none,         //UPDATE_TICK
      NULL,			//UPDATE TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      has_owner_none,         //UPDATE_OBJTICK
      has_owner_none,         //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//END HAS_OWNER

  /* Dark Metamorphosis*/
  {
    "weapon: dark metamorphosis",               //NAME
    &gen_dark_meta,                     //Gen number pointer.
    "hunger",                 //Damage noun.
    "You lose your control over $p.",           //Turn off msg to owner
    "$p's surface ripples and returns to normal.",//Turn off msg to room.
    EFF_F1_ALLOW_EXTEND | EFF_F1_NOEXTRACT,   //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      dark_met_init,          //UPDATE_INIT
      dark_met_kill,          //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      dark_met_postdamage,    //UPDATE_POSTDAMAGE
      NULL,                   //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {11},                  //chance for met to drain the victim.
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//END DARK METAMORPHOSIS

  /*Soul Reaver*/
  {
    "weapon: soul reaver",               //NAME
    &gen_soul_reaver,                   //Gen number pointer.
    "Soul drain",                      //Damage noun.
    "$p feeds on $N's soul as it cuts the flesh.",  //message to ownner when char hit.
    "A slow throbbing sound sends shivers down your spine.",
    0,                     //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      soul_reaver_init,      //UPDATE_INIT
      NULL,                   //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      soul_reaver_postdamage,//UPDATE_POSTDAMAGE
      NULL,                   //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      soul_reaver_parry,     //UPDATE_PARRY
      soul_reaver_parry,     //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {80},                 //chance for reaver to drain WHEN hit.
    {30},                  //chance to drain the weapon when parried.
    NULL                  //paf pointer (RESERVED)
  },//END Soul Reaver


  /*Hand of Chaos*/
  {
    "weapon: hand of chaos",               //NAME
    &gen_handof_chaos,                   //Gen number pointer.
    "Chaos",                      //Damage noun.
    "Reality twists and sheers around $p.",  //message to ownner when char hit.
    "Order and reality seems to twist and part around $p.",//message to room when char hti.
    0,                     //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      handof_chaos_init,      //UPDATE_INIT
      NULL,                   //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      handof_chaos_postdamage,//UPDATE_POSTDAMAGE
      NULL,                   //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      handof_chaos_parry,     //UPDATE_PARRY
      handof_chaos_parry,     //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {10},                 //chance to corrupt
    {4},                  //chance to destroy item.
    NULL                  //paf pointer (RESERVED)
  },//END hand of chaos

  /*request lag effect. */
  {
    "item request",          //NAME
    &gen_request,              //Gen number pointer.
    "",                      //Damage noun.
    "Interrupted you snap back to reality.",
    "$n's eyes fly open and he snaps back to reality.",   //msg to room
    EFF_F1_CMDBLOCK | EFF_F1_NOSAVE,       //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      request_end,	      //UPDATE_NONE
      request_init,           //UPDATE_INIT
      request_kill,           //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      request_none,           //UPDATE_LEAVE
      request_none,           //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      request_precommand,     //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      request_none,           //UPDATE_PREDAMAGE
      request_none,           //UPDATE_POSTDAMAGE
      request_tick,           //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    5,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (hp loss per tick %)
    {15},                  //value2  (mana loss per tick %)
    {10},                  //value3  (move loss per tick %)
    NULL                  //paf pointer (RESERVED)
  }, //End request

  /*UNLIFE */
  {
    "unlife",          //NAME
    &gen_unlife,             //Gen number pointer.
    "",                      //Damage noun.
    "The vile blood of undead purged your body and mind begins to heal.",
    "$n's skin takes on more healthy color and $e once again looks like a living being.",   //msg to room
    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_NODEATH,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      unlife_init,            //UPDATE_INIT
      unlife_kill,            //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      unlife_tick,           //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    5,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1
    {0},                  //value2  (mana loss per tick %)
    {0},                  //value3  (move loss per tick %)
    NULL                  //paf pointer (RESERVED)
  }, //End Unlife


  /*Seremon */
  {
    "sermon",          //NAME
    &gen_seremon,             //Gen number pointer.
    "seremons",                       //Damage noun.
    "The cursed booming voice finaly stops and your mind is silent.",
    "$n looks a little less annoyed.",   //msg to room
    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_HARMFUL,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      seremon_init,            //UPDATE_INIT
      seremon_kill,            //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      seremon_tick,           //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    2 * PULSE_VIOLENCE,    //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1
    {0},                  //value2
    {0},                  //value3
    NULL                  //paf pointer (RESERVED)
  }, //End seremon

  /*HOLY HANDS */
  {
    "holy touch",            //NAME
    &gen_holy_hands,             //Gen number pointer.
    "divine power",                      //Damage noun.
    "The aura of $g's power fades form your hands.",
    "The aura of $g's power fades from $n's hands.",   //msg to room
    EFF_F1_NOEXTRACT | EFF_F1_ALLOW_EXTEND,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      holy_hands_init,        //UPDATE_INIT
      holy_hands_kill,        //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      holy_hands_postdamage,  //UPDATE_POSTDAMAGE
      NULL,	              //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    5,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {100,50,50,50,50,50,50,50,50,30,30,30,30,0,0,0}, //chance of speical/deity
    {50},                  //value2  chance for sec. damage.
    {-5},                  //penalty to effect spell
    NULL                  //paf pointer (RESERVED)
  }, //Holy Hands


  /*Voodoo Doll (placed on character, gsn_voodo_spell si placed on ch and obj) */
  {
    "voodoo doll",            //NAME
    &gen_voodoo_doll,             //Gen number pointer.
    "voodoo",                      //Damage noun.
    "You are once again capable of creating a voodoo doll.",
    "",   //msg to room
    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      voodoo_doll_init,      //UPDATE_INIT
      voodoo_doll_kill,       //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      NULL,	              //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    5,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {100,50,50,50,50,50,50,50,50,30,30,30,30,0,0,0}, //NOT USED
    {50},                  //NOT USED
    {0},                  //NOT USED
    NULL                  //paf pointer (RESERVED)
  }, //Vodoo spell


/* CHARM PERSON EFFECT (ABUSE PROOF MORE OR LESS :)  */
  {
    "charm player",           //NAME
    &gen_pc_charm,            //Gen number pointer.
    "charm",                 //Damage noun.
    "You snap out of your daze and come too.",
    "$n's eyes lose the aura of indifference.",   //msg to room
     EFF_F1_NOSAVE | EFF_F1_CMDBLOCK,      //Bitwise flags 1 (RESERVED)
    GN_HAR,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      pc_charm_init,	      //UPDATE_INIT
      pc_charm_kill,	      //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,		      //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      pc_charm_precommand,    //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      pc_charm_predamage,      //UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      NULL,	              //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    5,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //NOT USED
    {0},                  //NOT USED
    {0},                  //NOT USED
    NULL                  //paf pointer (RESERVED)
  }, //hypnosis

/* BLADES EFFECT FOR BATTLEMAGES  */
  {
    "control blades",        //NAME
    &gen_blades,             //Gen number pointer.
    "scythe",                //Damage noun.

    "Your link to the deadly scythes weakens and breaks.",
    "A translucent outline of something wavers and disappears.",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      blades_init,	      //UPDATE_INIT
      blades_kill,	      //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      blades_precombat,       //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      blades_predamage,      //UPDATE_PREDAMAGE
      NULL,		            //UPDATE_POSTDAMAGE
      blades_tick,            //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      blades_previo,          //UPDATE_PREVOILENCE
      NULL,                  //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {5,5,10,15,20,25,28,31,33,35,35,35,0,0,0,0}, //+DAM%/HIT% mult / 5 levels.
    {12},                  //Upkeep
    {7},                  //base chance to reflect
    NULL                  //paf pointer (RESERVED)
  }, //blades

  /* DANCING BLADE */
  {
    "eldritch weapon",       //NAME
    &gen_dan_blade,          //Gen number pointer.
    "link",                //Damage noun.

    "",
    "",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      dan_blade_init,	      //UPDATE_INIT
      dan_blade_kill,	      //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      dan_blade_precombat,    //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      dan_blade_tick,         //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      dan_blade_previo,       //UPDATE_PREVOILENCE
      dan_blade_postvio,      //UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {200,202,203,204,205,210,215,250,270,280,290,300,300,300,300,300}, //+DAM%/HIT% mult / 3% > 75
    {17},                  //Upkeep / tick
    {0},                  //NOT USED
    NULL                  //paf pointer (RESERVED)
  }, //blades

/* ANGER EFFECT FOR WARMASTER */
  {
    "anger",             //NAME
    &gen_anger,          //Gen number pointer.
    "",                  //Damage noun.

    "You no longer have the energy to sustain your anger.",
    "The violence is wiped from the face of $n.",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      anger_search_room,      //UPDATE_INIT
      anger_kill,	      //UPDATE_KILL
      anger_search_room,      //UPDATE_ENTER
      anger_leave,            //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      anger_search_room,      //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //NOT USED
    {0},                  //NOT USED
    {0},                  //NOT USED
    NULL                  //paf pointer (RESERVED)
  }, //anger

  /* TERRA-SHIELD */
  {
    "terra-field",             //NAME
    &gen_terra_shield,          //Gen number pointer.
    "",                  //Damage noun.
    "",
    "",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      terra_shield_init,	//UPDATE_INIT
      terra_shield_kill,	//UPDATE_KILL
      terra_shield_enter,	//UPDATE_ENTER
      terra_shield_leave,	//UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      terra_shield_predamage,	//UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      terra_shield_tick,		//UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //NOT USED
    {15},                  //Upkeep
    {0},                  //NOT USED
    NULL                  //paf pointer (RESERVED)
  }, //terra shield

  /* REFLECTIVE-SHIELD */
  {
    "reflective-field",             //NAME
    &gen_ref_shield,          //Gen number pointer.
    "reflective field",                  //Damage noun.
    "Your reflective shield shatters and expands outwards.",
    "You are suddenly thrown back with huge force.",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      ref_shield_init,		//UPDATE_INIT
      ref_shield_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      ref_shield_precombat,   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      ref_shield_predamage,	//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      ref_shield_tick,		//UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //NOT USED
    {10},                  //Upkeep
    {50},                  //NOT USED
    NULL                  //paf pointer (RESERVED)
  }, //ref field


  /* PLUMBUM-MANUS */
  {
    "increase gravity",             //NAME
    &gen_plumbum,          //Gen number pointer.
    "plumbum manus",                  //Damage noun.
    "Your field of increased gravity fades away.",
    "You feel a bit lighter.",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      plumbum_init,		//UPDATE_INIT
      plumbum_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      plumbum_precombat,	//UPDATE_PRECOMBAT
      plumbum_postcombat,	//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      plumbum_tick,		//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,			//UPDATE_PREDEATH
      NULL,			//UPDATE_POSTDEATH
      NULL,			//UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,			//UPDATE_PREEXP
      NULL,			//UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,			//UPDATE_OBJTICK
      NULL,			//UPDATE_OBJTOCHAR
      NULL,			//UPDATE_DODGE
      NULL,			//UPDATE_PARRY
      NULL,			//UPDATE_SHBLOCK
      NULL			//UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //doble save check? T/F
    {8},                  //upkeep
    {100},                  //NOT USED
    NULL                  //paf pointer (RESERVED)
  }, //plumbum



  /* CLoak of Discord (CHAOS) */
  {
    "Field of Discord",    //NAME
    &gen_discord,          //Gen number pointer.
    "!Discord!",                  //Damage noun.
    "The influence of chaos fades from around you.",
    "$n's visage ripples slightly.",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      discord_init,		//UPDATE_INIT
      discord_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      NULL,			//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      discord_tick,		//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,			//UPDATE_PREDEATH
      NULL,			//UPDATE_POSTDEATH
      NULL,			//UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,			//UPDATE_PREEXP
      NULL,			//UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,			//UPDATE_OBJTICK
      NULL,			//UPDATE_OBJTOCHAR
      NULL,			//UPDATE_DODGE
      NULL,			//UPDATE_PARRY
      NULL,			//UPDATE_SHBLOCK
      NULL			//UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //NA
    {5},                  //upkeep
    {25},                  //chance flip state
    NULL                  //paf pointer (RESERVED)
  }, //Discord


  /*  (CHAOS) */
  {
    "Moloch Eye",    //NAME
    &gen_orb,          //Gen number pointer.
    "!Orb!",                  //Damage noun.
    "Moloch's attention shifts away from you.",
    "",

    EFF_F1_NOEXTRACT | EFF_F1_ALLOW_EXTEND,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      orb_init,		//UPDATE_INIT
      orb_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      orb_precombat,		//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,			//UPDATE_PREDEATH
      NULL,			//UPDATE_POSTDEATH
      NULL,			//UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,			//UPDATE_PREEXP
      NULL,			//UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,			//UPDATE_OBJTICK
      NULL,			//UPDATE_OBJTOCHAR
      NULL,			//UPDATE_DODGE
      NULL,			//UPDATE_PARRY
      NULL,			//UPDATE_SHBLOCK
      NULL			//UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,20,40,60,0,0,0,0,0,0,0,0,0,0,0,0}, //N/A
    {70},                   //Max points
    {20},                    //points/level
    NULL                    //paf pointer (RESERVED)
  }, //Orb

  /*  (CONCLAVE) */
  {
    "concealment",    //NAME
    &gen_conceal,          //Gen number pointer.
    "A hooded figure",    //String used for desc.
    "The thick cloak about you falls away.",
    "The thick cloak about $n falls away.",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      conceal_init,		//UPDATE_INIT
      conceal_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      conceal_precombat,	//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,			//UPDATE_PREDEATH
      NULL,			//UPDATE_POSTDEATH
      NULL,			//UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,			//UPDATE_PREEXP
      NULL,			//UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,			//UPDATE_OBJTICK
      NULL,			//UPDATE_OBJTOCHAR
      NULL,			//UPDATE_DODGE
      NULL,			//UPDATE_PARRY
      NULL,			//UPDATE_SHBLOCK
      NULL			//UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //N/A
    {3},                  //chance for hood to fall of in combat round.
    {0},                  //N/A
    NULL                  //paf pointer (RESERVED)
  }, //Concealment


  /*  (CONCLAVE) */
  {
    "death-shroud",    //NAME
    &gen_d_shroud,      //Gen number pointer.
    "!Shroud!",			//Damage noun
    "The deathly chill leaves you as you return to the living.",
    "Pale and bleak $n appears out of vanishing shadows.",


    EFF_F1_NOEXTRACT | EFF_F1_ALLOW_EXTEND,      //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      d_shroud_init,		//UPDATE_INIT
      d_shroud_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      NULL,			//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      d_shroud_precommand,	//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,			//UPDATE_PREDEATH
      NULL,			//UPDATE_POSTDEATH
      NULL,			//UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,			//UPDATE_PREEXP
      NULL,			//UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,			//UPDATE_OBJTICK
      NULL,			//UPDATE_OBJTOCHAR
      NULL,			//UPDATE_DODGE
      NULL,			//UPDATE_PARRY
      NULL,			//UPDATE_SHBLOCK
      NULL			//UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //N/A
    {0},                  //NA
    {0},                  //N/A
    NULL                  //paf pointer (RESERVED)
  }, //Death Shroud


  /* CHAOS SPAWN  [CHAOS] */
  {
    "chaos spawn",       //NAME
    &gen_cspawn,          //Gen number pointer.
    "Spawn",        //Damage noun.

    "Your feel your control over chaos lapse.",
    "",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      cspawn_init,	      //UPDATE_INIT
      cspawn_kill,	      //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      cspawn_precombat,		//UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      cspawn_tick,		//UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      cspawn_previo,		//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,0,5,10,15,15,15,15,20,20,20,20,0,0,0,0}, //+DAM%/HIT% mult / 5 levels.
    {20},                  //Chance for special / round of combat
    {6},                  //Initial Duration
    NULL                  //paf pointer (RESERVED)
  }, //cspawn


  /* Visitation */
  {
    "mark of visitation",       //NAME
    &gen_visit,          //Gen number pointer.
    "demonic servant",        //Damage noun.

    "You sense the demonic servants lose track of you.",
    "",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_HAR,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      visit_init,	      //UPDATE_INIT
      visit_kill,	      //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,			//UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      visit_tick,		//UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,		//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      visit_viol              //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {20},                  //chance to skip a visitation
    {4},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //visit

  /* Efuzan Warding  [SAVANT] */
  {
    "damage ward",       //NAME
    &gen_sav_ward,          //Gen number pointer.
    "!WARD!",        //Damage noun.

    "Efuzan's ward collapses.",
    "Strange runes flare around $n and fade.",

    EFF_F1_NOEXTRACT | EFF_F1_ALLOW_EXTEND,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      sav_ward_init,		//UPDATE_INIT
      sav_ward_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      NULL,			//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      sav_ward_predamage,	//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,			//UPDATE_PREDEATH
      NULL,			//UPDATE_POSTDEATH
      NULL,			//UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,			//UPDATE_PREEXP
      NULL,			//UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,			//UPDATE_OBJTICK
      NULL,			//UPDATE_OBJTOCHAR
      NULL,			//UPDATE_DODGE
      NULL,			//UPDATE_PARRY
      NULL,			//UPDATE_SHBLOCK
      NULL		       //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {150},                  //upper damage max with ward
    {4},                  //divisor for chance to break after v2[0]
    NULL                  //paf pointer (RESERVED)
  }, //savant ward


  /* CATALYST  [SAVANT] */
  {
    "catalyst effect",       //NAME
    &gen_sav_cata,          //Gen number pointer.
    "!CATA!",        //Damage noun.

    "The arcane energy seeps from your body",
    "$n's skin loses its magical glow.",

    EFF_F1_NOEXTRACT | EFF_F1_ALLOW_EXTEND,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      sav_cata_init,		//UPDATE_INIT
      sav_cata_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      NULL,			//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      sav_cata_tick,		//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,			//UPDATE_PREDEATH
      NULL,			//UPDATE_POSTDEATH
      NULL,			//UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,			//UPDATE_PREEXP
      NULL,			//UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,			//UPDATE_OBJTICK
      NULL,			//UPDATE_OBJTOCHAR
      NULL,			//UPDATE_DODGE
      NULL,			//UPDATE_PARRY
      NULL,			//UPDATE_SHBLOCK
      NULL		       //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {1000},                //Max hp with cata on.
    {4},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //savant catalyst

 /* Circle of Protection (Druids) */
/*
  {
    "circle of protection",       //NAME
    &gen_circle_of_protection,    //Gen number pointer.
    "!CIRCLE!",        //Damage noun.

    "You circle of protection collapses.",
    "$N attempts to create a circle of protection around $mself.",

    EFF_F1_NOEXTRACT | EFF_F1_ALLOW_EXTEND,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     	//UPDATE_NONE
      circle_of_protection_init,  	//UPDATE_INIT
      circle_of_protection_kill,  	//UPDATE_KILL
      NULL,                       	//UPDATE_ENTER
      NULL,                       	//UPDATE_LEAVE
      NULL,                     	//UPDATE_PRECOMBAT
      NULL,                     	//UPDATE_POSTCOMBAT
      NULL,                     	//UPDATE_PREATTACK
      NULL,                     	//UPDATE_POSTATTACK
      NULL,                     	//UPDATE_PRESPELL
      NULL,                     	//UPDATE_POSTSPELL
      NULL,                     	//UPDATE_PRECOMMAND
      NULL,                     	//UPDATE_POSTCOMMAND
      circle_of_protection_predamage,   //UPDATE_PREDAMAGE
      NULL,                     	//UPDATE_POSTDAMAGE
      NULL,                     	//UPDATE_TICK
      NULL,                     	//UPDATE_MOBTICK
      NULL,                     	//UPDATE_PREDEATH
      NULL,                     	//UPDATE_POSTDEATH
      NULL,                     	//UPDATE_PREKILL
      NULL,                     	//UPDATE_POSTKILL
      NULL,                     	//UPDATE_PREEXP
      NULL,                     	//UPDATE_POSTEXP
      NULL,                     	//UPDATE_PREVOILENCE
      NULL,                     	//UPDATE_POSTVOILENCE
      NULL,                     	//UPDATE_OBJTICK
      NULL,                     	//UPDATE_OBJTOCHAR
      NULL,                     	//UPDATE_DODGE
      NULL,                     	//UPDATE_PARRY
      NULL,                     	//UPDATE_SHBLOCK
      NULL                      	//UPDATE_VIOTICK
    },//End update functions.

    0,                      //PreBeats
    0,                      //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {250},                  //upper damage max with ward
    {4},                    //divisor for chance to break after v2[0]
    NULL                    //paf pointer (RESERVED)
  }, //circle of protection
*/

  /* [WARMASTER] CHALLENGE */
  {
    "challenge-effect",       //NAME
    &gen_challenge,          //Gen number pointer.
    "!CHALLANGE!",        //Damage noun.

    "A challenge has ended.",
    "",

    EFF_F1_NOSAVE,      //Bitwise flags 1 (RESERVED)
    GN_STA,                  //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      challenge_init,		//UPDATE_INIT
      challenge_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      NULL,			//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      challenge_tick,		//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,			//UPDATE_PREDEATH
      NULL,			//UPDATE_POSTDEATH
      NULL,			//UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,			//UPDATE_PREEXP
      NULL,			//UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,			//UPDATE_OBJTICK
      NULL,			//UPDATE_OBJTOCHAR
      NULL,			//UPDATE_DODGE
      NULL,			//UPDATE_PARRY
      NULL,			//UPDATE_SHBLOCK
      NULL		       //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //warmaster challange

  /* Druids - Yearning */
  {
    "yearning-effect",    //NAME
    &gen_yearning,        //Gen number pointer.
    "yearning for the forests",           //Damage noun.

    "Your yearning has been satisfied.",
    "",

    EFF_F1_NOSAVE,      //Bitwise flags 1 (RESERVED)
    GN_STA,                  //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      NULL,      		//UPDATE_INIT
      NULL,              	//UPDATE_KILL
      NULL,             	//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      NULL,			//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      yearning_tick,		//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,			//UPDATE_PREDEATH
      NULL,			//UPDATE_POSTDEATH
      NULL,			//UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,			//UPDATE_PREEXP
      NULL,			//UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,			//UPDATE_OBJTICK
      NULL,			//UPDATE_OBJTOCHAR
      NULL,			//UPDATE_DODGE
      NULL,			//UPDATE_PARRY
      NULL,			//UPDATE_SHBLOCK
      NULL		       //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //druids yearning
  /* ENSNARE EFFECT */
  {
    "ensnare-effect",    //NAME
    &gen_ensnare,        //Gen number pointer.
    "!ensnare!",           //Damage noun.

    "You are finaly free to leave the area.",
    "$n is finaly free to leave the area.",

    EFF_F1_NOSAVE,      //Bitwise flags 1 (RESERVED)
    GN_STA,                  //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      NULL,                     //UPDATE_INIT
      ensnare_kill,             //UPDATE_KILL
      NULL,                     //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      ensnare_tick,           //UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,                     //UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      NULL                     //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //ENSNARE

  /* RALLY */
  {
    "mob-generator",    //NAME
    &gen_mob_gen,        //Gen number pointer.
    "!mob-generator!",           //Damage noun.

    "",
    "",

    EFF_F1_NOSAVE,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                  //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      NULL,                     //UPDATE_INIT
      NULL,                     //UPDATE_KILL
      NULL,                     //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      mob_gen_tick,           //UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,                     //UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      NULL                     //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //MOB_GEN

  /* [JUSTICE] - JAIL GEN */
  {
    "incarceration",    //NAME
    &gen_jail,		//Gen number pointer.
    "",			//Damage noun.

    "\n\rYou leave the stink and dirt of jails behind you.",
    "",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_STA,                  //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      NULL,      		//UPDATE_INIT
      jail_kill,              	//UPDATE_KILL
      NULL,             	//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      NULL,			//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      jail_tick,		//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,			//UPDATE_PREDEATH
      NULL,			//UPDATE_POSTDEATH
      NULL,			//UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,			//UPDATE_PREEXP
      NULL,			//UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,			//UPDATE_OBJTICK
      NULL,			//UPDATE_OBJTOCHAR
      NULL,			//UPDATE_DODGE
      NULL,			//UPDATE_PARRY
      NULL,			//UPDATE_SHBLOCK
      NULL		        //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {10},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //JAIL

  /* watchtower */
  {
    "in watchtower",              //NAME
    &gen_watchtower,            //Gen number pointer.
    "!watchtower!",               //Damage noun.
    "You exit the watchtower.",
    "$n leaves $s watchtower.",
    0,          //Bitwise flags 1(RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      watch_init,             //UPDATE_INIT
      watch_kill,             //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      watch_leave,            //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      NULL,		      //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//END Watchtower

  /* batter */
  {
    "battering",		//NAME
    &gen_batter,		//Gen number pointer.
    "batter",			//Damage noun.
    "You stop concentrating on battering.",
    "$n stops landing staggering blows.",
    EFF_F1_NOSAVE,          //Bitwise flags 1(RESERVED)
    GN_INT,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      NULL,		      //UPDATE_INIT
      batter_kill,            //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,		      //UPDATE_PRECOMBAT
      batter_precombat,	      //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      batter_predamage,       //UPDATE_PREDAMAGE
      batter_postdamage,      //UPDATE_POSTDAMAGE
      NULL,		      //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      batter_kill,            //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//END Batter

  /* Drug Addiction */
  {
    "drug addiction",    //NAME
    &gen_addiction,        //Gen number pointer.
    "drug withdrawal",           //Damage noun.

    "You seem to have overcome your drug addiction.",
    "",

    0,				//Bitwise flags 1 (RESERVED)
    GN_STA,                  //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      NULL,                     //UPDATE_INIT
      NULL,                     //UPDATE_KILL
      NULL,                     //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      addiction_tick,           //UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,                     //UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      NULL                     //UPDATE_VIOTICK
     },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //addiction

  /*  LEVITATION */
  {
    "levitation",          //NAME
    &gen_levitation,       //Gen number pointer.
    "",   //damage noun
    "You fall to the ground.",                    //self msg
    "$n falls to the ground.",
    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN | GN_PSI,       //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNCTIONS
    {
      NULL,                   //UPDATE_NONE
      levitation_init,          //UPDATE_INIT
      levitation_kill,          //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,		      //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,	              //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      levitation_init,          //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,20,40,60,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {100},                  //max points
    {10},                  //points/level
    NULL                  //paf pointer (RESERVED)
  }, //End levitation

  /* PHANTASM */
  {
    "spook-effect",    //NAME
    &gen_phantasm,        //Gen number pointer.
    "phantasm",           //Damage noun.

    "You disbelieve the phantasm.",
    "$n manages to disbelieve the phantasm.",

    0,      //Bitwise flags 1 (RESERVED)
    GN_STA,                  //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      NULL,                     //UPDATE_INIT
      general_kill,		//UPDATE_KILL
      NULL,                     //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,                     //UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      phantasm_vtick            //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //SIEGE

  /* Photon Wall */
  {
    "photon wall",    //NAME
    &gen_photon_wall,        //Gen number pointer.
    "!photon!",           //Damage noun.

    "You no longer control the light around you.",
    "",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN | GN_PSI,                  //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      photon_init,              //UPDATE_INIT
      photon_kill,              //UPDATE_KILL
      NULL,                     //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,                     //UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      photon_init               //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //photon wall

  /* Crusade */
  {
    "crusade-effect",    //NAME
    &gen_crusade,    //Gen number pointer.
    "!crusade!",           //Damage noun.

    "Your Crusade against $t has come to an end!",
    "",

    EFF_F1_NOEXTRACT,     //Bitwise flags 1 (RESERVED)
    GN_STA,                    //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      crusade_init,              //UPDATE_INIT
      crusade_kill,              //UPDATE_KILL
      NULL,                     //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      crusade_tick,		//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      crusade_postkill,         //UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      NULL                     //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //CRUSADE

  /* AVENGER */
  {
    "avenger-effect",    //NAME
    &gen_avenger,	//Gen number pointer.
    "!avenger!",        //Damage noun.

    "Your rightous quest of retribution against $t has ended!",
    "",

    EFF_F1_NOEXTRACT,     //Bitwise flags 1 (RESERVED)
    GN_STA,                    //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      avenger_init,              //UPDATE_INIT
      avenger_kill,              //UPDATE_KILL
      NULL,                     //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      NULL,		//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      avenger_postkill,		//UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      NULL                     //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //AVENGER

  /* BLEEDING */
  {
    "bleeding",    //NAME
    &gen_bleed,	//Gen number pointer.
    "blood loss",        //Damage noun.

    "You manage to completely halt your bleeding.",
    "$n manages to cover $s wounds and halt the bleeding.",

    EFF_F1_NOEXTRACT,     //Bitwise flags 1 (RESERVED)
    GN_STA,                    //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      bleed_init,		//UPDATE_INIT
      bleed_kill,		//UPDATE_KILL
      bleed_enter,              //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      bleed_init,		//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      bleed_vtick               //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //BLEED

  /* ARMOR PIERCE */
  {
    "armor negation",    //NAME
    &gen_apierce,	//Gen number pointer.
    "armor piercing blow",        //Damage noun.

    "",
    "",

    EFF_F1_NOEXTRACT,     //Bitwise flags 1 (RESERVED)
    GN_STA,                    //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      apierce_init,		//UPDATE_INIT
      NULL,			//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      apierce_postviolence,     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      apierce_predamage,        //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      NULL			//UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {90},                  //chance to make the apierce blow
    NULL                  //paf pointer (RESERVED)
  }, //APIERCE
  /*chant lag effect. */
  {
    "chant psalm",          //NAME
    &gen_chant,              //Gen number pointer.
    "",                      //Damage noun.
    "Interrupted you snap back to reality.",
    "$n's eyes fly open and he snaps back to reality.",   //msg to room
    EFF_F1_CMDBLOCK,       //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      chant_end,	    //UPDATE_NONE
      chant_init,           //UPDATE_INIT
      chant_kill,           //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      chant_none,           //UPDATE_LEAVE
      chant_none,           //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      request_precommand,     //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      chant_none,           //UPDATE_PREDAMAGE
      chant_none,           //UPDATE_POSTDAMAGE
      request_tick,           //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    5,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (hp loss per tick %)
    {15},                  //value2  (mana loss per tick %)
    {10},                  //value3  (move loss per tick %)
    NULL                  //paf pointer (RESERVED)
  }, //End chant
  /* DIVINE VOID */
  {
    "divine void",    //NAME
    &gen_dvoid,	//Gen number pointer.
    "!error!",        //Damage noun.

    "The psalm of Divine Void leaves your mind.",
    "",

    EFF_F1_NOEXTRACT,     //Bitwise flags 1 (RESERVED)
    GN_STA,                    //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      dvoid_init,		//UPDATE_INIT
      dvoid_kill,		//UPDATE_KILL
      NULL,              //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      dvoid_tick,		//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      NULL               //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {30},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //Divine Void

  /* Sirants Triumph */
  {
    "spirit of equality",    //NAME
    &gen_strium,	//Gen number pointer.
    "!error!",        //Damage noun.

    "The psalm of Sirant's Triumph leaves your mind.",
    "",

    EFF_F1_NOEXTRACT,     //Bitwise flags 1 (RESERVED)
    GN_STA,                    //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      strium_init,		//UPDATE_INIT
      strium_kill,		//UPDATE_KILL
      NULL,              //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      strium_predam,            //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      NULL               //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {30},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //sirant's triumph

  /* Warding */
  {
    "warding",    //NAME
    &gen_ward,	//Gen number pointer.
    "!error!",        //Damage noun.

    "You $t ward collapses.",
    "$n's $t ward collapses",

    EFF_F1_NOEXTRACT,     //Bitwise flags 1 (RESERVED)
    GN_BEN,                    //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      ward_init,		//UPDATE_INIT
      ward_kill,		//UPDATE_KILL
      NULL,              //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,            //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      ward_tick,			//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      NULL               //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {30},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //warding
  /*armorcrat lag effect. */
  {
    "modify armor",          //NAME
    &gen_acraft,              //Gen number pointer.
    "",                      //Damage noun.
    "Interrupted you abort the work before destroying the piece.",
    "$n curses as $e stops working on a piece of armor.",   //msg to room
    EFF_F1_CMDBLOCK,       //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      acraft_end,	    //UPDATE_NONE
      acraft_init,          //UPDATE_INIT
      acraft_kill,          //UPDATE_KILL
      NULL,                 //UPDATE_ENTER
      acraft_none,           //UPDATE_LEAVE
      acraft_none,           //UPDATE_PRECOMBAT
      NULL,                 //UPDATE_POSTCOMBAT
      NULL,                 //UPDATE_PREATTACK
      NULL,                 //UPDATE_POSTATTACK
      NULL,                 //UPDATE_PRESPELL
      NULL,                 //UPDATE_POSTSPELL
      request_precommand,   //UPDATE_PRECOMMAND
      NULL,                  //UPDATE_POSTCOMMAND
      acraft_none,           //UPDATE_PREDAMAGE
      acraft_none,           //UPDATE_POSTDAMAGE
      request_tick,           //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    5,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (hp loss per tick %)
    {15},                  //value2  (mana loss per tick %)
    {10},                  //value3  (move loss per tick %)
    NULL                  //paf pointer (RESERVED)
  }, //End armorcraft

  /*armorcrat lag effect. */
  {
    "modify weapon",          //NAME
    &gen_wcraft,              //Gen number pointer.
    "",                      //Damage noun.
    "Interrupted you abort the work before destroying the weapon.",
    "$n curses as $e stops working on a weapon.",   //msg to room
    EFF_F1_CMDBLOCK,       //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      wcraft_end,	    //UPDATE_NONE
      wcraft_init,          //UPDATE_INIT
      wcraft_kill,          //UPDATE_KILL
      NULL,                 //UPDATE_ENTER
      wcraft_none,           //UPDATE_LEAVE
      wcraft_none,           //UPDATE_PRECOMBAT
      NULL,                 //UPDATE_POSTCOMBAT
      NULL,                 //UPDATE_PREATTACK
      NULL,                 //UPDATE_POSTATTACK
      NULL,                 //UPDATE_PRESPELL
      NULL,                 //UPDATE_POSTSPELL
      request_precommand,   //UPDATE_PRECOMMAND
      NULL,                  //UPDATE_POSTCOMMAND
      wcraft_none,           //UPDATE_PREDAMAGE
      wcraft_none,           //UPDATE_POSTDAMAGE
      request_tick,           //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    5,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (hp loss per tick %)
    {15},                  //value2  (mana loss per tick %)
    {10},                  //value3  (move loss per tick %)
    NULL                  //paf pointer (RESERVED)
  }, //End weaponcraft

  /* HOLY WEAPON update */
  {
    "holy weapon",          //NAME
    &gen_hwep,              //Gen number pointer.
    "",
    "",
    "",
    EFF_F1_NOEXTRACT,       //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      hwep_none,		//UPDATE_NONE
      NULL,			//UPDATE_INIT
      NULL,			//UPDATE_KILL
      NULL,                 //UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      NULL,			//UPDATE_PRECOMBAT
      NULL,                 //UPDATE_POSTCOMBAT
      NULL,                 //UPDATE_PREATTACK
      NULL,                 //UPDATE_POSTATTACK
      NULL,                 //UPDATE_PRESPELL
      NULL,                 //UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      NULL,		      //UPDATE_TICK
      NULL,           //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      hwep_objtick,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    5,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,1,3,9,12,24,36,48,64,90,100,0,0,0,0,0}, //value1 (levels for malform
    {9},                  //value2  //malform MAX LEVEL of the weapon.
    {0},                 //value3  //chance for weapon to weaken.
    NULL                  //paf pointer (RESERVED)
  }, //End weaponcraft

  /* APPROACH for pry/plant */
  {
    "approach",		//NAME
    &gen_approach,      //Gen number pointer.
    "",                 //Damage noun.
    "You step away from your victim.",
    "",
    EFF_F1_NOEXTRACT,       //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      NULL,			//UPDATE_INIT
      approach_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      approach_none,		//UPDATE_LEAVE
      approach_none,		//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      approach_tick,		//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    5,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,1,3,9,12,24,36,48,64,90,100,0,0,0,0,0}, //value1 (levels for malform
    {9},                  //value2  //malform MAX LEVEL of the weapon.
    {0},                 //value3  //chance for weapon to weaken.
    NULL                  //paf pointer (RESERVED)
  }, //End appraoch

  /* SUFFOCATE */
  {
    "suffocate",		//NAME
    &gen_suffocate,      //Gen number pointer.
    "asphyxiation",                 //Damage noun.
    "You can final breathe properly!",
    "",
    EFF_F1_NOEXTRACT,       //Bitwise flags 1 (RESERVED)
    GN_HAR,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      NULL,			//UPDATE_INIT
      suffocate_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      NULL,			//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      suffocate_tick,		//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      suffocate_vtick         //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,1,3,9,12,24,36,48,64,90,100,0,0,0,0,0}, //value1 not used
    {1},                  //value2  //min damage on vtick
    {3},                 //value3  //max damage on vtick
    NULL                  //paf pointer (RESERVED)
  }, //End suffocate


  /* CONTRACT */
  {
    "contract",		//NAME
    &gen_contract,      //Gen number pointer.
    "!contract!",                 //Damage noun.
    "Your contract has been cancelled.",
    "",
    EFF_F1_NOEXTRACT,       //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      NULL,			//UPDATE_INIT
      contract_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      NULL,			//UPDATE_PRECOMBAT
      NULL,			//UPDATE_POSTCOMBAT
      NULL,			//UPDATE_PREATTACK
      NULL,			//UPDATE_POSTATTACK
      NULL,			//UPDATE_PRESPELL
      NULL,			//UPDATE_POSTSPELL
      NULL,			//UPDATE_PRECOMMAND
      NULL,			//UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      NULL,			//UPDATE_POSTDAMAGE
      contract_tick,		//UPDATE_TICK
      NULL,			//UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL		//UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,1,3,9,12,24,36,48,64,90,100,0,0,0,0,0}, //value1 not used
    {1},                  //value2
    {3},                 //value3
    NULL                  //paf pointer (RESERVED)
  }, //End suffocate

  /* SPIRIT */
  {
    "bound spirits",       //NAME
    &gen_spirit,          //Gen number pointer.
    "Spirit",        //Damage noun.

    "Your control over the captured spirit falters.",
    "A transclucent form drifts away from $n.",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      NULL,	      //UPDATE_INIT
      spirit_kill,	      //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      spirit_precombat,		//UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      NULL,		//UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,		//UPDATE_PREVOILENCE
      NULL,			//UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {5,5,5,10,15,20,20,25,30,30,30,30,0,0,0,0}, //+DAM/HIT mult / 5 levels.
    {20},                  //Chance for special / round of combat
    {6},                  //Initial Duration
    NULL                  //paf pointer (RESERVED)
  }, //spirit

  /* COUP */
  {
    "coup",       //NAME
    &gen_coup,          //Gen number pointer.
    "!coup!",        //Damage noun.

    "Your chance for a coup has passed.",
    "",

    EFF_F1_NOEXTRACT | EFF_F1_NODEATH,      //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		      //UPDATE_NONE
      NULL,		      //UPDATE_INIT
      coup_kill,	      //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,		      //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      NULL,		      //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      coup_postdeath,         //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      coup_postkill,          //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,		      //UPDATE_PREVOILENCE
      NULL,		      //UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {5,5,5,10,15,20,20,25,30,30,30,30,0,0,0,0}, //+DAM/HIT mult / 5 levels.
    {20},                  //Chance for special / round of combat
    {6},                  //Initial Duration
    NULL                  //paf pointer (RESERVED)
  }, //spirit

  /* DIPLOAMCY */
  {
    "diplomatic immunity",       //NAME
    &gen_diplomacy,          //Gen number pointer.
    "!diplomatic imm.!",        //Damage noun.

    "Your diplomatic immunity has been lost.",
    "The royal entourage around $n departs.",

    EFF_F1_NOEXTRACT | EFF_F1_NODEATH,      //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      diplo_none,	      //UPDATE_NONE
      NULL,		      //UPDATE_INIT
      diplo_kill,	      //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,		      //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      diplo_tick,	      //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,		      //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,		      //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,		      //UPDATE_PREVOILENCE
      diplo_tick,	      //UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {5,5,5,10,15,20,20,25,30,30,30,30,0,0,0,0}, //+DAM/HIT mult / 5 levels.
    {20},                  //Chance for special / round of combat
    {6},                  //Initial Duration
    NULL                  //paf pointer (RESERVED)
  }, //Diplomacy

  /* REPLACE ME */
  {
    "replaceme",    //NAME
    &gen_recruit,        //Gen number pointer.
    "!recruit.!",        //Damage noun.

    "You stop recruiting armies.",
    "",

    EFF_F1_NOEXTRACT | EFF_F1_NODEATH,      //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,	      //UPDATE_NONE
      NULL,		      //UPDATE_INIT
      NULL,	      //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,		      //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      NULL,	      //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,		      //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,		      //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,		      //UPDATE_PREVOILENCE
      NULL,		      //UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {5,5,5,10,15,20,20,25,30,30,30,30,0,0,0,0}, //+DAM/HIT mult / 5 levels.
    {20},                  //Chance for special / round of combat
    {6},                  //Initial Duration
    NULL                  //paf pointer (RESERVED)
  }, //REcruit

  /* Vengeance */
  {
    "holy vengeance",             //NAME
    &gen_vengeance,          //Gen number pointer.
    "vengeance",                  //Damage noun.
    "You are no longer shielded by Vengeance of Light.",
    "The angry white aura around $n fades.",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,			//UPDATE_NONE
      veng_init,		//UPDATE_INIT
      veng_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,			//UPDATE_LEAVE
      veng_precombat,   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,			//UPDATE_PREDAMAGE
      veng_predamage,		//UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //NOT USED
    {0},                  //Upkeep
    {120},                  //max damage per combat round stored
    NULL                  //paf pointer (RESERVED)
  }, //vengeance

  /* CONJURE */
  {
    "conjure",       //NAME
    &gen_conjure,          //Gen number pointer.
    "!conjure!",        //Damage noun.

    "",
    "",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      conjure_end,	    //UPDATE_NONE
      conjure_init,           //UPDATE_INIT
      conjure_kill,           //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      conjure_none,           //UPDATE_LEAVE
      conjure_none,           //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      request_precommand,     //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      conjure_none,           //UPDATE_PREDAMAGE
      conjure_none,           //UPDATE_POSTDAMAGE
      request_tick,           //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {5,5,5,10,15,20,20,25,30,30,30,30,0,0,0,0}, //+DAM/HIT mult / 5 levels.
    {20},                  //Chance for special / round of combat
    {6},                  //Initial Duration
    NULL                  //paf pointer (RESERVED)
  }, //REcruit

  /* MAZE */
  {
    "mazed",       //NAME
    &gen_maze,          //Gen number pointer.
    "!mazed!",        //Damage noun.

    "",
    "",

    EFF_F1_NOEXTRACT | EFF_F1_NOSAVE,      //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		//UPDATE_NONE
      maze_init,        //UPDATE_INIT
      maze_kill,        //UPDATE_KILL
      NULL,             //UPDATE_ENTER
      NULL,		//UPDATE_LEAVE
      NULL,		//UPDATE_PRECOMBAT
      NULL,             //UPDATE_POSTCOMBAT
      NULL,             //UPDATE_PREATTACK
      NULL,             //UPDATE_POSTATTACK
      NULL,             //UPDATE_PRESPELL
      NULL,             //UPDATE_POSTSPELL
      NULL,		//UPDATE_PRECOMMAND
      NULL,             //UPDATE_POSTCOMMAND
      NULL,		//UPDATE_PREDAMAGE
      NULL,		//UPDATE_POSTDAMAGE
      NULL,		//UPDATE_TICK
      NULL,             //UPDATE_MOBTICK
      NULL,             //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {5,5,5,10,15,20,20,25,30,30,30,30,0,0,0,0}, //+DAM/HIT mult / 5 levels.
    {20},                  //Chance for special / round of combat
    {6},                  //Initial Duration
    NULL                  //paf pointer (RESERVED)
  }, //maze

  /* Chron Shield */
  {
    "chrono-shield",       //NAME
    &gen_chron,          //Gen number pointer.
    "past",        //Damage noun.

    "You catch up to the Chrono Shield conduit.",
    "A vortex of bright neon colors flickers and fades before $n.",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_HAR,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		//UPDATE_NONE
      NULL,		//UPDATE_INIT
      chron_kill,        //UPDATE_KILL
      NULL,             //UPDATE_ENTER
      NULL,		//UPDATE_LEAVE
      NULL,		//UPDATE_PRECOMBAT
      NULL,             //UPDATE_POSTCOMBAT
      NULL,             //UPDATE_PREATTACK
      NULL,             //UPDATE_POSTATTACK
      NULL,             //UPDATE_PRESPELL
      NULL,             //UPDATE_POSTSPELL
      NULL,		//UPDATE_PRECOMMAND
      NULL,             //UPDATE_POSTCOMMAND
      chron_predamage,	//UPDATE_PREDAMAGE
      NULL,		//UPDATE_POSTDAMAGE
      chron_tick,	//UPDATE_TICK
      NULL,             //UPDATE_MOBTICK
      NULL,             //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {5,5,5,10,15,20,20,25,30,30,30,30,0,0,0,0}, //+DAM/HIT mult / 5 levels.
    {20},                  //Chance for special / round of combat
    {6},                  //Initial Duration
    NULL                  //paf pointer (RESERVED)
  }, //chrons hield

  /* Paradox */
  {
    "paradox-effect",       //NAME
    &gen_paradox,          //Gen number pointer.
    "past",        //Damage noun.

    "With a low roar reality adjusts itself after the Paradox!.",
    "With a low roar reality adjusts itself after the Paradox!.",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_HAR,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		//UPDATE_NONE
      NULL,		//UPDATE_INIT
      paradox_kill,        //UPDATE_KILL
      NULL,             //UPDATE_ENTER
      NULL,		//UPDATE_LEAVE
      NULL,		//UPDATE_PRECOMBAT
      NULL,             //UPDATE_POSTCOMBAT
      NULL,             //UPDATE_PREATTACK
      NULL,             //UPDATE_POSTATTACK
      NULL,             //UPDATE_PRESPELL
      NULL,             //UPDATE_POSTSPELL
      NULL,		//UPDATE_PRECOMMAND
      NULL,             //UPDATE_POSTCOMMAND
      NULL,		//UPDATE_PREDAMAGE
      NULL,		//UPDATE_POSTDAMAGE
      paradox_tick,	//UPDATE_TICK
      NULL,             //UPDATE_MOBTICK
      NULL,             //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {5,5,5,10,15,20,20,25,30,30,30,30,0,0,0,0}, //+DAM/HIT mult / 5 levels.
    {20},                  //Chance for special / round of combat
    {6},                  //Initial Duration
    NULL                  //paf pointer (RESERVED)
  }, //Paradox
  /* TEMP AVENGER */
  {
    "temporal-avenger", //NAME
    &gen_temp_avenger,  //Gen number pointer.
    "attacks",		//Damage noun.

    "You catch up to the temporal conduit.",
    "A vortex of bright neon colors flickers and fades before $n.",

    EFF_F1_NOEXTRACT | EFF_F1_NODEATH,      //Bitwise flags 1 (RESERVED)
    GN_HAR,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,		//UPDATE_NONE
      NULL,		//UPDATE_INIT
      NULL,		//UPDATE_KILL
      NULL,             //UPDATE_ENTER
      NULL,		//UPDATE_LEAVE
      NULL,		//UPDATE_PRECOMBAT
      NULL,             //UPDATE_POSTCOMBAT
      NULL,             //UPDATE_PREATTACK
      NULL,             //UPDATE_POSTATTACK
      NULL,             //UPDATE_PRESPELL
      NULL,             //UPDATE_POSTSPELL
      NULL,		//UPDATE_PRECOMMAND
      NULL,             //UPDATE_POSTCOMMAND
      NULL,		//UPDATE_PREDAMAGE
      NULL,		//UPDATE_POSTDAMAGE
      temp_avenger_tick,	//UPDATE_TICK
      NULL,             //UPDATE_MOBTICK
      NULL,             //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,		      //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL		//UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {5,5,5,10,15,20,20,25,30,30,30,30,0,0,0,0}, //+DAM/HIT mult / 5 levels.
    {20},                  //Chance for special / round of combat
    {6},                  //Initial Duration
    NULL                  //paf pointer (RESERVED)
  }, //temporal avenger

  /* MOVEDAM */
  {
    "wounded",		 //NAME
    &gen_move_dam, 	 //Gen number pointer.
    "wounding",        //Damage noun.

    "You manage to mend your wounds.",
    "",

    EFF_F1_NOEXTRACT | EFF_F1_NOSAVE,     //Bitwise flags 1 (RESERVED)
    GN_STA,                    //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      NULL,			//UPDATE_INIT
      general_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      NULL,			//UPDATE_VIOTICK
      movedam_enter		//UPDATE_MOVE
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //MOVEDAM

  /* KINESIS */
  {
    "kinesis-effect",		 //NAME
    &gen_kinesis, 	 //Gen number pointer.
    "kinesis",        //Damage noun.

    "The kinesis has been depleted.",
    "The kinesis around $n has depleted itself.",

    EFF_F1_NOEXTRACT | EFF_F1_NOSAVE,     //Bitwise flags 1 (RESERVED)
    GN_BEN | GN_PSI,                    //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      NULL,			//UPDATE_INIT
      kinesis_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      kinesis_vtick,		//UPDATE_VIOTICK
      NULL			//UPDATE_MOVE
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //MOVEDAM

  /* CREATE OBJECT (vnum in modifier, chance to make in bitvector) */
  {
    "create_object",             //NAME
    &gen_create_object,          //Gen number pointer.
    "",				//Damage noun.
    "***STRING ON PAF****",	//msg to self
    "",				//msg to room
    EFF_F1_NONE,           //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      create_object_init,     //UPDATE_INIT
      create_object_kill,     //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      create_object_init,     //UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      create_object_init,         //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//End create object

  /* AMORPHOUS INFECTION (modifiers is number of re-infections before death)*/
  {
    "amorphous creature",             //NAME
    &gen_ainfection,          //Gen number pointer.
    "parasite",				//Damage noun.
    "The amorphous parasite vacates your felsh.",	//msg to self
    "",				//msg to room
    EFF_F1_NOEXTRACT,           //Bitwise flags 1 (RESERVED)
    GN_STATIC,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                   //UPDATE_NONE
      ainfect_init,          //UPDATE_INIT
      ainfect_kill,     //UPDATE_KILL
      NULL,             //UPDATE_ENTER
      NULL,		//UPDATE_LEAVE
      NULL,                   //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      ainfect_tick,          //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {0},                  //value2  (RESERVED)
    {0},                  //value3  (RESERVED)
    NULL                  //paf pointer (RESERVED)
  },//amorphous infection

  /* DREAMWALK */
  {
    "dreamwalk",		 //NAME
    &gen_dreamwalk, 	 //Gen number pointer.
    "nightmare",         //Damage noun.

    "You return from your dreamwalk.",
    "",

    EFF_F1_NOEXTRACT | EFF_F1_NOSAVE,     //Bitwise flags 1 (RESERVED)
    GN_STA,				 //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      NULL,			//UPDATE_INIT
      dreamwalk_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      dreamwalk_none,           //UPDATE_LEAVE
      dreamwalk_none,           //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      dreamwalk_tick,		//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      dreamwalk_vtick,		//UPDATE_VIOTICK
      NULL			//UPDATE_MOVE
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //DREAMWALK
  /* SVISE */
  {
    "spellvise",		 //NAME
    &gen_svise, 	 //Gen number pointer.
    "spell vise",         //Damage noun.
    "The pressure around your temples eases off.",
    "The blue ring around $n's head fades away.",
    EFF_F1_NOEXTRACT | EFF_F1_NOSAVE,     //Bitwise flags 1 (RESERVED)
    GN_HAR,				 //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      NULL,			//UPDATE_INIT
      svise_kill,		//UPDATE_KILL
      NULL,			//UPDATE_ENTER
      NULL,		        //UPDATE_LEAVE
      NULL,		        //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,			//UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      svise_vtick,		//UPDATE_VIOTICK
      NULL			//UPDATE_MOVE
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //SPELL VISE

  /* RECRUIT */
  {
    "replace me",       //NAME
    &gen_upgrade,          //Gen number pointer.
    "!recruit.!",        //Damage noun.

    "You stop upgrading armies.",
    "",

    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_STA,                     //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,	      //UPDATE_NONE
      NULL,		      //UPDATE_INIT
      NULL,	      //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,		      //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,                   //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,		      //UPDATE_POSTDAMAGE
      NULL,	      //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,		      //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,		      //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,		      //UPDATE_PREVOILENCE
      NULL,		      //UPDATE_POSTVOILENCE
      NULL,		      //UPDATE_OBJTICK
      NULL,                   //UPDATE_OBJTOCHAR
      NULL,                   //UPDATE_DODGE
      NULL,     	      //UPDATE_PARRY
      NULL,		      //UPDATE_SHBLOCK
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    //Begin the arrays of int. buffers/dummy data
    {5,5,5,10,15,20,20,25,30,30,30,30,0,0,0,0}, //+DAM/HIT mult / 5 levels.
    {20},                  //Chance for special / round of combat
    {6},                  //Initial Duration
    NULL                  //paf pointer (RESERVED)
  }, //Upgrade

  /*  METABOLIC BOOST */
  {
    "metabolic-boost",          //NAME
    &gen_metabolic_boost,       //Gen number pointer.
    "",   //damage noun
    "Your metabolism slows down.",                    //self msg
    "$n seems to move slower.",
    EFF_F1_NOEXTRACT,      //Bitwise flags 1 (RESERVED)
    GN_BEN | GN_PSI,       //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNCTIONS
    {
      NULL,                   //UPDATE_NONE
      mboost_init,          //UPDATE_INIT
      mboost_kill,          //UPDATE_KILL
      NULL,                   //UPDATE_ENTER
      NULL,                   //UPDATE_LEAVE
      NULL,		      //UPDATE_PRECOMBAT
      NULL,                   //UPDATE_POSTCOMBAT
      NULL,                   //UPDATE_PREATTACK
      NULL,                   //UPDATE_POSTATTACK
      NULL,                   //UPDATE_PRESPELL
      NULL,                   //UPDATE_POSTSPELL
      NULL,	              //UPDATE_PRECOMMAND
      NULL,                   //UPDATE_POSTCOMMAND
      NULL,                   //UPDATE_PREDAMAGE
      NULL,                   //UPDATE_POSTDAMAGE
      mboost_init,          //UPDATE_TICK
      NULL,                   //UPDATE_MOBTICK
      NULL,                   //UPDATE_PREDEATH
      NULL,                   //UPDATE_POSTDEATH
      NULL,                   //UPDATE_PREKILL
      NULL,                   //UPDATE_POSTKILL
      NULL,                   //UPDATE_PREEXP
      NULL,                   //UPDATE_POSTEXP
      NULL,                   //UPDATE_PREVOILENCE
      NULL,                   //UPDATE_POSTVOILENCE
      NULL,                   //UPDATE_OBJTICK
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL,                   //UPDATE_EXPANSION
      NULL                    //UPDATE_EXPANSION
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time)

    //Begin the arrays of int. buffers/dummy data
    {0,20,40,60,0,0,0,0,0,0,0,0,0,0,0,0}, //value1  (RESERVED)
    {100},                  //max points
    {10},                  //points/level
    NULL                  //paf pointer (RESERVED)
  }, //End metabolic_boost

  /* PHANTASM */
  {
    "dragon-spit",    //NAME
    &gen_dragon_blood, //Gen number pointer.
    "!dragon blood!",  //Damage noun.

    "The colored taint fades from your vision.",
    "$n's eyes return to their normal color.",

    0,				//Bitwise flags 1 (RESERVED)
    GN_BEN,                  //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      NULL,                     //UPDATE_INIT
      general_kill,		//UPDATE_KILL
      NULL,                     //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      dragon_blood_precom,      //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,                     //UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      NULL,		        //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //SIEGE

  /* REGENERATE */
  {
    "regenerate-effect",    //NAME
    &gen_regenerate,        //Gen number pointer.
    "!regenerate!",         //Damage noun.

    "You stop regenerating.",
    "",

    0,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                  //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      NULL,                     //UPDATE_INIT
      general_kill,		//UPDATE_KILL
      NULL,                     //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      NULL,                     //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,                     //UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,                     //UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      regenerate_vtick          //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {0},                  //NA
    {0},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //regenerate

  /* REGENERATE */
  {
    "temporal-storm",    //NAME
    &gen_temp_storm,        //Gen number pointer.
    "!regenerate!",         //Damage noun.

    "Your temporal storm collapses.",
    "The strange vortex of energies above $n fizzles out.",

    0,      //Bitwise flags 1 (RESERVED)
    GN_BEN,                  //Bitwise flags 2 (RESERVED)
    //BEGIN UPDATE FUNTCIONS
    {
      NULL,                     //UPDATE_NONE
      NULL,                     //UPDATE_INIT
      general_kill,		//UPDATE_KILL
      NULL,                     //UPDATE_ENTER
      NULL,                     //UPDATE_LEAVE
      NULL,                     //UPDATE_PRECOMBAT
      NULL,                     //UPDATE_POSTCOMBAT
      NULL,                     //UPDATE_PREATTACK
      NULL,                     //UPDATE_POSTATTACK
      NULL,                     //UPDATE_PRESPELL
      NULL,                     //UPDATE_POSTSPELL
      NULL,                     //UPDATE_PRECOMMAND
      NULL,                     //UPDATE_POSTCOMMAND
      NULL,                     //UPDATE_PREDAMAGE
      NULL,                     //UPDATE_POSTDAMAGE
      NULL,			//UPDATE_TICK
      temp_storm_tick,          //UPDATE_MOBTICK
      NULL,                     //UPDATE_PREDEATH
      NULL,                     //UPDATE_POSTDEATH
      NULL,                     //UPDATE_PREKILL
      NULL,                     //UPDATE_POSTKILL
      NULL,                     //UPDATE_PREEXP
      NULL,                     //UPDATE_POSTEXP
      NULL,			//UPDATE_PREVOILENCE
      NULL,                     //UPDATE_POSTVOILENCE
      NULL,                     //UPDATE_OBJTICK
      NULL,                     //UPDATE_OBJTOCHAR
      NULL,                     //UPDATE_DODGE
      NULL,                     //UPDATE_PARRY
      NULL,                     //UPDATE_SHBLOCK
      temp_storm_vupdate,       //UPDATE_VIOTICK
    },//End update functions.

    0,                     //PreBeats
    0,                     //beats (lag time inticks)

    {5,5,10,15,20,20,25,30,35,40,45,50,0,0,0,0}, //NA
    {25},                  //NA
    {25},                  //NA
    NULL                  //paf pointer (RESERVED)
  }, //temporal storm
  /*END OF EFFECTS TABLE */
};


//-------------=================EFFECT RELATED FUNCTIONS===========----------//

/* get_gen */
/* externed in  merc.h */
/* This should be used for all TABLE references to gen */
inline int gn_pos(sh_int gn)
{
  int gen;

//number below MAX_SKILL are gsn's
//Numbers above MAX_SKILL are gen's
  return ( (gen =  gn - MAX_SKILL) < 0 ? gn: gen) ;
}//end gn_pos


/* effect_lookup */
/* externed in merc.h */
/* This should be ALWAYS used to get a gen number */
inline int effect_lookup( const char *name )
{
    int gen;
    for ( gen = 0; gen < MAX_EFFECTS; gen++ )
      {
	if ( effect_table[gen].name == NULL )
	      break;
	if ( LOWER(name[0]) == LOWER(effect_table[gen].name[0]) && !str_prefix( name, effect_table[gen].name ) )
	      return MAX_SKILL + gen;
      }
    bug("GEN EFFECT NOT FOUND IN effect_lookup()", 0);
    return -1;
}//end effect_lookup

/* run_effect_update */
/* extrened in merc.h */
/* returns -1 on error */

int run_effect_update
(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag, int UPDATE_TYPE){
  int table_pos;

//This function calls the given EFF_UPDATE function for the gen given using the
//EFF_UPDATE const passed into the function.


//only one quick check.
  if ((gn < MAX_SKILL) || (UPDATE_TYPE < 0 || UPDATE_TYPE > MAX_EFF_UPDATE) )
    {
      bug("AT: run_effect_update(..) Invalid argument passed\n\r", 0);
      return -1;
    }
if (effect_table[table_pos=gn_pos(gn)].effect_update_fun[UPDATE_TYPE] == NULL)
    return TRUE;

  //Otherwise we run the funtcion.
 return  ((*effect_table[table_pos].effect_update_fun[UPDATE_TYPE]) (ch, arg1, arg2, gn, int1, int2, flag));
}//end run_eff_update

//--------================END EFFECT RELATED FUNCTIONS========--------------//







//---------------BEGIN EFFECT FUNCTIONS HERE-------------------------//

//GENERAL

int general_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off))
    act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off2))
    act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}//end general_kill

//FIRESTORM

/* Firestorm INIT */
/* link in effect.c only */
/* written by viri */
int firestorm_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//we only set the SPEC_UPDATE flag for the room.
return  SET_BIT(ch->in_room->room_flags2, ROOM_SPEC_UPDATE);

}//end inti_firestorm



/* Firestorm KILL */
/* link in effect.c only */
/* written by viri */
int firestorm_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  if (gn != gen_firestorm)
    return -1;
//we run the room_sepc_update to maybe remove the SPEC_UPDATE bit.
  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  room_spec_update(ch->in_room);
  return TRUE;
}//end firestorm_kill



/* Firestorm PRECOMBAT */
/* link in effect.c only */
/* written by viri */
int firestorm_precombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  CHAR_DATA* vch, *vch_next;

//This is the precombat update function called from fight.c
//It attacks all characters inroom every round of combat.

  int dam = ch->level;

//beggin damaging everyone in the room.
//we display some messages

  switch (number_range(1, 20))
    {
    case 1:
    case 2:
      act("The raging firestorm spews forth a cloud of fiery brimstone!", ch, NULL, NULL, TO_ROOM);
      act("Your firestorm spews forth a cloud of fiery brimstone!", ch, NULL, NULL, TO_CHAR);
      dam = (float) dam * 1.5;
      break;

    case 8:
    case 9:
      act("The firestorm spills sheets of flame all around!", ch, NULL, NULL, TO_ROOM);
      act("Your firestorm sends sheets of flame flying all around!", ch, NULL, NULL, TO_CHAR);
      dam = dam * 2;
      break;

    case 15:
    case 16:
      act("Hail of flaming rocks begins to fall setting the area aflame!", ch, NULL, NULL, TO_ROOM);
      act("The brimstone clouds begin to rain fiery hail!", ch, NULL, NULL, TO_CHAR);
      dam =  dam * 2.5;
	break;

    case 18:
    case 19:
      act("The raging storm of hellfire twists and sheers and then explodes in a fiery nova!", ch, NULL, NULL, TO_ROOM);
      act("The vortex of hellfire explodes suddenly!", ch, NULL, NULL, TO_CHAR);
      dam *=4;
      break;
    }//end selsect

  for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
      vch_next = vch->next_in_room;
      if ( saves_spell( ch->level, vch, DAM_FIRE,skill_table[skill_lookup("firestorm")].spell_type) )
	dam /= 2;
//Make sure we dont hit groupmates  charmies etc.
      if (!is_area_safe(ch,vch))
	{
	  m_yell(ch, vch, FALSE);
	  damage( ch, vch, dam, gen_firestorm, DAM_FIRE ,TRUE);
	}
    }//end for
  return TRUE;
}//end firestorm_precombat


/* firestorm ENTER */
/* link in effect.c only */
/* written by viri */
int firestorm_enter(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  char buf [MIL];
  //called when someone enters this room.
  //given room has SPEC_UPDATE set (checked before callng this)

  CHAR_DATA* vch = (CHAR_DATA*) arg1;
  //ch is the holder of firestorm
  //vch is the victim that walked in.

//We yell out
  if (!is_area_safe(ch,vch) && vch->fighting != ch)
    {
      act("Without cover $N is peleted with flaming debris!", vch, NULL, vch, TO_ROOM);
      act("You see a swirling vortex of fiery clouds. Seconds later flaming debris falls all around you.", vch, NULL, NULL, TO_CHAR);

      a_yell(ch, vch);
      sprintf(buf, "Help! I've just walked into %s's firestorm!",PERS(ch,vch));
      j_yell(vch,buf);
      return damage(ch, vch ,number_range(ch->level, 2 * ch->level), gen_firestorm, DAM_FIRE, TRUE);
    }
  return 0;
}//end firestorm_enter




/* Firestorm LEAVE */
/* link in effect.c only */
/* written by viri */
int firestorm_leave(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //called when character walks out of room with SPEC_UPDATE
  //and has gen_firestorm effect.
  //This function is meant to be quick and slim, with min saftey.


  if (!is_affected(ch, gen_firestorm)) return FALSE;

//affect_strip will run the update_kill_firestorm(..)for us.
  affect_strip(ch, gen_firestorm);

  return TRUE;
}//end firestorm_leave.



//MANACHARGE
/* getmobj */
/* link in effect.c only */
/* written by viri */
inline OBJ_DATA* get_mobj(CHAR_DATA* ch, int* mana, bool both)
{
  OBJ_DATA* obj;
  OBJ_DATA* prim;
  OBJ_DATA* sec;

  int item_mana = 0;
  int mana1 = 0;
  int mana2 = 0;

  //This is where we check for the items.
  if ((prim = get_eq_char(ch, WEAR_WIELD)) != NULL)
    mana1 = is_mcharged(prim);


  if ((sec = get_eq_char(ch, WEAR_HOLD)) != NULL)
    mana2 = is_mcharged(sec);
  else if ((sec = get_eq_char(ch, WEAR_SECONDARY)) != NULL)
    mana2 = is_mcharged(sec);

  //check if the items are even charged.
  if ((mana1 < 1) && (mana2 < 1) )
    return NULL;

  //check if both items are loaded.
  if (mana1 > 0 && mana2 > 0 && both)
    {
      //we destroy the object with more mana
      if (mana1 >= mana2)
	{
	  act("As you tap into your mana pools $p begins to emit a high pitched noise and explodes!", ch, prim, NULL, TO_CHAR);
	  act("Suddenly $p beggins to shiver and emit a high pitched sound, exploding in $n's hands!.", ch, prim, NULL, TO_ROOM);
	  mana_flare(ch, mana1);
	  extract_obj(prim);
	  //set the object to use the one left over.
	  obj = sec;
	  item_mana = (int) mana2;
	}//and if mana1
      else
	{
	  act("As you tap into your mana pools $p begins to emit a high pitched noise and explodes!", ch, sec, NULL, TO_CHAR);
	  act("Suddenly $p beggins to shiver and emit a high pitched sound, exploding in $n's hands!.", ch, sec, NULL, TO_ROOM);
	  mana_flare(ch, mana2);
	  extract_obj(sec);
	  //set the object to use the one left over.
	  obj = prim;
	  item_mana = (int) mana1;
	}//end else
    }//end if mana1&&mana2
  else
    {
//only one item was charged we check which.
      if (mana1 > 1)
	{obj = prim; item_mana = (int) mana1;}
      else
	{obj =  sec; item_mana = (int) mana2;}
    }//end else

  //DEBUG
  //    sendf(ch, "`3Prim charge:%d  Sec Charge:%d  Return Charge: %d\n\r``", mana1, mana2, (int)item_mana);

   *mana = item_mana;
  return obj;
}//end get_mobject





/* ManaCHarge Init */
/* link in effect.c only */
/* written by viri */
int mana_charge_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  OBJ_DATA* obj = (OBJ_DATA*) arg2;
//Called when gen is first applied.
  //EZ
  if (gn != gen_mana_charge)
    return FALSE;

 obj->enchanted = TRUE;
 SET_BIT(obj->extra_flags, ITEM_GLOW);
 SET_BIT(obj->extra_flags, ITEM_HUM);
 return TRUE;
}


/* ManaCHarge PreSpell */
/* link in effect.c only */
/* written by viri */
int mana_charge_prespell(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int both)
{
//This is the function called after mana calculation
//and before mana is subtracted
//Current spell level is passed through arg2
//the curent spell cost is passed through int1 by address
//the lagtime is passed through int2 by address

  OBJ_DATA* obj;
  AFFECT_DATA* paf;

  //Do a bit of pointer shuffle here to do the spell lag.

  int* int3 = (int*) arg2;

  int item_mana = 0;
  int spell_cost =(int) *int1;//for safety so we dont modify it by accident.
  int spell_lag  =(int) *int2;//for safety so we dont modify it by accident.
  int spell_lvl  =(int) *int3;//for safety so we dont modify it by accident.

  const int lvl_mod = 300;   //(+4 at 1200)
  const int cost_mod = 1700; //(70% at 1200)
  const int lag_mod =  2400; //(50% at 1200)
  const int control_pen = 20;//amount added onto cost spell if control is lost.
  /*

    1) Get the charge from held item.
    - if two items are held one blows
    2) Check item vs caster mana
    - lower item mana if neccesary. (ch mana lower then item AFTER casting the spell)
    3) calculate reductions based on current ammount
    4) if charge on item is less then 0 the gen is removed.
  */

//EZ case, ch needs manacharge to draw on mana.
  if (IS_NPC(ch))
    return FALSE;
if (!(get_skill(ch, gsn_mana_charge)) )
  return FALSE;


//This is where we check for the items.
if ( ( obj = get_mobj(ch, &item_mana, both)) == NULL)
  return -1;

//The left over item is now stored in obj, mana in item_mana
//calculate the benefits.
 spell_lvl += (item_mana / lvl_mod);
 spell_cost -= ( spell_cost * item_mana / cost_mod);
 spell_lag -=  ( spell_lag * item_mana / lag_mod);

//check if the mana pool in the item is going to be lowered.
  if (ch->mana < item_mana)
    {
//we reduce item mana by the reduced amount (original_cost - current_cost)
      item_mana = UMAX(0, (item_mana - (int) *int1 - spell_cost - control_pen));
      //and change it on the item.
      if ((paf = affect_find(obj->affected, gen_mana_charge)) != NULL)//safety
	paf->modifier = item_mana;
      //give a message:
      act("Unable to control $p's energy, you consume its raw resources.", ch, obj, NULL, TO_CHAR);
    }//end item reduction.
  else
    act("You tap into $p's mana pool and reinforce your spell.", ch, obj, NULL, TO_CHAR);

//Now we have reductions calculated, and item changed done if neccesary.
//we change the int1 and int2 and we are done.
  *int1 = spell_cost;
  *int2 = spell_lag;
  *int3 = spell_lvl;

//DEBUG
  //sendf(ch, "`#Reduced cost: %d, Reduced Lag: %d, Final Lvl: %d\n\r``", (int)*int1, (int)*int2, (int)*int3);
  return TRUE;
}//end mana_charge_prespell




/* mana_charge POSTSPELL */
/* link in effect.c only */
/* written by viri */
int mana_charge_postspell(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int both)
{
  //This is the function called at the end of any spell,
  //it checks if caster is capable of holding back the charge.
  //character casting spell is ch.

  OBJ_DATA* obj;
  AFFECT_DATA* paf;

  int item_mana = 0;
  const int control_pen = 20;//amount added onto cost spell if control is lost.
  /*
    1) check if caster has any charged item
    2)We check if the mana on the caster is less then on item
    - if caster has less mana, we flare the difference.
    note: we still check for the larges item.
    3) if item manna is 0 and has gen_mana_charge we crumble the item.
  */

//EZ case
  if (IS_NPC(ch))
    return FALSE;
  if (!get_skill(ch, gsn_mana_charge))
    return FALSE;


//we can pass the object to be check ed in arg1 (from do_manacharge)
if (arg1 != NULL)
  {
    obj = (OBJ_DATA*) arg1;
    item_mana = is_mcharged(obj);
  }
else
  {
    if ( ( obj = get_mobj(ch, &item_mana, both)) == NULL)
      return -1;
  }

//At this point if item_mana is -1 we return.
 if (item_mana == -1)
   return FALSE;
//Now we check if character should be burnt.
    if (ch->mana < item_mana)
      {
	int diff = item_mana - ch->mana;
	act("Your mind to weak to hold back $p's charge, you scream in pain.", ch, obj, NULL, TO_CHAR);
	act("$n cries out in pain and clutches his temples.", ch, NULL, NULL, TO_ROOM);
	mana_flare(ch, diff);
	//now we have to lower the mana on the object.
	if ( (paf = affect_find(obj->affected, gen_mana_charge)) != NULL)
	  {
	    item_mana =  UMAX(0, item_mana - diff - control_pen);
	    paf->modifier = item_mana;
	  }
      }

//last thing we check is if item has 0 charge in which case it crumples.
    if (item_mana < 10)
      {
	act("The runes on $p's slowly fade and it crumbles into dust.", ch, obj, NULL, TO_ALL);
	extract_obj(obj);
	return TRUE;
      }
//no burn
return FALSE;
}//end mana_charge_postspell


/* mcharge_info */
/* link in effect.c only */
/* written by viri */
bool mcharge_info(CHAR_DATA* ch, char* buf, OBJ_DATA* obj, AFFECT_DATA* paf, bool say)
{
  int cur_charge;
  int cur_cap;
  int msg_num = 0;
  int msg2_num = 0;
  //This function copies into the buffer the stat/iden. info
  //about mana charge.
  //All info strings are taken from tables.c

  //EZ
if ((buf == NULL) || (obj == NULL) || (paf == NULL))
  {bug("mcharge_info: invalid arg.\n\r", 0);return FALSE;}


//we get current charge.
 cur_charge = paf->modifier;

//and capacity
if ((cur_cap =  get_maxmcharge(obj)) < 1)
  return FALSE;
 cur_cap = 100 * cur_charge / cur_cap;

 //now we get the message numbers for cur_charge
 if (cur_charge < 100)
   msg_num = 0;
 else if (cur_charge < 200)
   msg_num = 1;
 else if (cur_charge < 300)
   msg_num = 2;
 else if (cur_charge < 400)
   msg_num = 3;
 else if (cur_charge < 500)
   msg_num = 4;
 else if (cur_charge < 600)
   msg_num = 5;
 else if (cur_charge < 700)
   msg_num = 6;
 else if (cur_charge < 800)
   msg_num = 7;
 else if (cur_charge < 900)
   msg_num = 8;
 else if (cur_charge < 1000)
   msg_num = 9;
 else if (cur_charge < 1100)
   msg_num = 10;
 else
   msg_num = 11;


 //select messages for capacity
 if (cur_cap < 10)
   msg2_num = 0;
 else if (cur_cap < 20)
   msg2_num = 1;
 else if (cur_cap < 30)
   msg2_num = 2;
 else if (cur_cap < 40)
   msg2_num = 3;
 else if (cur_cap < 50)
   msg2_num = 4;
 else if (cur_cap < 60)
   msg2_num = 5;
 else if (cur_cap < 70)
   msg2_num = 6;
 else if (cur_cap < 80)
   msg2_num = 7;
 else if (cur_cap < 90)
   msg2_num = 8;
 else if (cur_cap < 95)
   msg2_num = 9;
 else
   msg2_num = 10;

 //we start writing the stuff.
 sprintf(buf, "Has been infused with %s %scharge and is at %s %scapacity.\n\r",
  (mcharge_table[msg_num].desc == NULL ? "NULL" : mcharge_table[msg_num].desc),
  (say ? "`#" : ""),
  (maxmcharge_table[msg2_num].desc == NULL ? "NULL" : maxmcharge_table[msg2_num].desc),
  (say ? "`#" : "") );
 return TRUE;
}


/* mcharge kill */
/* link in effect.c only */
/* written by viri */
int mana_charge_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int both)
{
  OBJ_DATA* obj = (OBJ_DATA*) arg2;

  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int item_charge = 0;

  //Three possiblities of an object being somwhere.
  bool on_room = FALSE;
  bool on_char = FALSE;

  ROOM_INDEX_DATA* in_room;
  CHAR_DATA* vch = NULL;

  //if obj is on the char we hurt them, if its on the floor
  //we show a message to room,

  if( (obj == NULL) || (paf == NULL))
    return -1;

  //get the mana on item.
  if( (item_charge = paf->modifier) < 1 )
    return FALSE;

  //checkwhere the object is
  if ( (in_room = obj->in_room ) != NULL)
    on_room = TRUE;
  else  if ( (vch = obj->carried_by) != NULL)
    on_char = TRUE;
  else
      return FALSE;
  if (on_char)
    //we damage holder
    {
      //check if this is on quit or reboot.
      if (vch->extracted == TRUE)
	  return FALSE;
      act("The runs on $p flare with deadly power as it's charge is realeased!", vch, obj, NULL, TO_ALL);
      if (!IS_IMMORTAL(vch))
	mana_flare(vch, item_charge);
      return TRUE;
    }


  //first we find a char.
  if (on_room)
    {
      if ( (vch = obj->in_room->people) == NULL)
	return FALSE;
      else
	{
	  //we do a message to the whole room.
	  act("$p's runes flare with deadly arcane force, and you spring back just in time!", vch, obj, NULL, TO_ALL);
	  act("$p shatters in an explosion of magical power.", vch, obj, NULL, TO_ROOM);
	  return TRUE;
	}
      return -1;
    }
  return FALSE;
}






//GOLD LOSS (cutpurse)
/* gold_loss_init */
/* link in effect.c only */
/* written by viri */
int gold_loss_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int fTick)
{
  //ch is the char affected.
  //room to be spiiled into is ch->in_room
  //amount affected by fTick

  /*
    This is the function called when effect is inited as well as:
- each step (UPDATE_LEAVE)
- each tick (UPDATE_TICK)

the flag fTick simply decides how much gold we spill.
  */


  const int tick_mod = 8;
  const int gold_loss = number_range(ch->level / 2, ch->level);
  int todrop;

  if (ch == NULL || ch->in_room == NULL)
    return -1;

  todrop = UMIN(ch->gold, (fTick ? gold_loss * tick_mod : gold_loss));
  if (todrop > 0){
    ch->gold -= todrop;
    obj_to_room(create_money(todrop),ch->in_room);
    act("Some gold spills out of $n's purse.",ch,NULL,NULL,TO_ROOM);
  }
  return TRUE;
}

/* gold_loss_init */
/* link in effect.c only */
/* written by viri */
int gold_loss_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //called when cutpurse effect is stripped.
  //paf pointer alwasy passed though arg1
  if (gn != gen_gold_loss)
    return -1;
  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}



//GUISE


/* guise INIT */
/* called in effect.c only */
/* Created by Viri */

int guise_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  //The function changes current char. title to a random class/
  //cannot be done if has a custom title.

  char buf[MIL];

//constants
  const int lvl_mod = 4;

//data.
  int new_lvl = ch->level;
  int new_class = paf->modifier;
  int delta_lvl = 0;//data holder for the change in lvls

//Otherwise we get the lvl jump.


//we flip for positibe or negative change.
  if ( (number_percent() < 50) || (ch->level == LEVEL_HERO) )//lvl Down.
    delta_lvl = -1 * number_range(1, lvl_mod);
  else
    delta_lvl = number_range(1, lvl_mod);

  //we check if we are over 50.
  if ( (new_lvl + delta_lvl) > LEVEL_HERO)
    delta_lvl = -1 * delta_lvl;
  //end we add them up.
  new_lvl += delta_lvl;
  //now we have the new lvl, we select the class.
  //DEBUG
  //sendf(ch, "New class: %s, New Level: %d \n\r", class_table[new_class].name, new_lvl);
//Now we have both new lvl and class so we set the title.

  sprintf( buf, "the %s", title_table [new_class] [new_lvl] [ch->sex == SEX_FEMALE ? 1 : 0] );
  set_title( ch, buf );
  /* store it on the paf */
  paf->modifier = new_class;
  return TRUE;
}//all done.

/* guise KILL */
/* calle din effect.c only */
/* written by Viri */
int guise_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  char buf [MIL];
  //This is a very simple function.
  //we display the message and set the titile back to normal.

  if (gn != gen_guise)
    return -1;
  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  //adn we set the title.
  sprintf( buf, "the %s", title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
  set_title(ch, buf);
  return TRUE;
}//end guise_kill



//STUDY


inline void study_memorize(CHAR_DATA* ch, AFFECT_DATA* paf)
{
  //The second state function called from study_tick each tick.
  //The function transfers the learned amount (level) into memorised
  //amount (modifier) each tick as long as level > 0.

  //gen_study pointer passed through paf.

  /*
    0)if level <1  we bleed modifier.
    1) if level > 1
    - We rest and begin the trance, if the character get attacked,
    moves etc.. level is cleared.
    2) We calculate the amount moved from level to modifier.
    3) update paf.
    - snap out if level < 1;
  */


  //data
  int amount = 0; //hodls amount memorized.
  int chance = 0;

  //const
  const int skill_med = 75;  //benifit/penalty point for med. skill.
  const int skill_mod = 5;   //divisor for the skill aboove/blow median
  const int int_med = 18;    //median for benefit/penalty for int.
  const int int_mod = 2;     //divis for int

  const int forget_chance = 40; //chance to forget
  const int forget_mod = 2;//multiplier of int above median.
  const int forget_med = 17; //bonus/penalty to forget above this int.

  //ceiling for mem.
  const int max_mem = effect_table[GN_POS(gen_study)].value1[0];

//check if we are forgetting.
  if (paf->level < 1)
    {
      if (ch->position != POS_MEDITATE)
	{
//calculate the chance to forget
	  chance = forget_chance;
//The higher int the loweer chance.
	  chance -= ( (get_curr_stat(ch, STAT_INT) - forget_med) * forget_mod);
//Floor the value.
	  chance = UMAX(10, chance);
//make a roll if we forget.
	  if (number_percent() < chance)
	    paf->modifier --;
	}
      return;
    }

  //check if we are learing.
  else if (ch->position != POS_MEDITATE) //special case where we just start.
    {
      //we make sure we can meditate
      if (ch->position > POS_SLEEPING)
	{
	  ch->position = POS_MEDITATE;
	  sendf(ch, "You enter a deep trance as you attempt to memorize all you have learned about %s.\n\r", paf->string);
	  if (!IS_AFFECTED(ch, AFF_HIDE))
	    act( "$n sits down and begins to meditate.", ch, NULL, NULL, TO_ROOM );	}//end if learned and not meditating and able to meditate
      else
	{
	  send_to_char("Unable to meditate on what you have learned you quickly forget your observations.\n\r", ch);
	  paf->level  = 0;
	  return;
	}//edn if unable to meditate.
    }//end if learned and not mediating.
  //now we do learing if character is in meditaing.
  if (ch->position == POS_MEDITATE)
    {

//Find how much we memorize
//base amount from skill.
      amount = UMAX(1,(get_skill(ch, gsn_meditation) - skill_med) / skill_mod);
      check_improve(ch, gsn_meditation, TRUE, 15);
      amount += UMAX(0, (get_curr_stat(ch, STAT_INT) - int_med) / int_mod);
      send_to_char("You meditate on what you have noticed and gain more insight!\n\r", ch);

      if (paf->level <= amount)
	{
	  amount = paf->level;
	  paf->level = 0;
//we finish meditating.
	  send_to_char("All that you have learned now burnt into your memory you break the trance.\n\r", ch);
	  act("$n stops meditating and stands up.",ch,NULL,NULL,TO_ROOM);
	  ch->position = POS_STANDING;
	}
      else
	paf->level = paf->level - amount;

//add to total memory
      if(paf->modifier + amount > max_mem)
	{
	  paf->modifier = max_mem;
//we finish meditating if we havent done so.
	  paf->level = 0;
	  if (ch->position == POS_MEDITATE)
	    {
	      send_to_char("All that you have learned now burnt into your memory you break the trance.\n\r", ch);
	      if (!IS_AFFECTED(ch, AFF_HIDE))
		act("$n stops meditating and stands up.",ch,NULL,NULL,TO_ROOM);
	      ch->position = POS_STANDING;
	    }
	}
      else
	paf->modifier += amount;


//check skill.
      check_improve(ch, gsn_meditation, TRUE, 15);
      //DEBUG
      //      sendf(ch,"Transfered: %d, Remaning: %d, Current Total: %d.\n\r", amount, paf->level, paf->modifier);
    }//done if learing.
  else
    paf->level = 0;

  //final check.
  if (paf->modifier > max_mem)
    {
      paf->level = 0;
      paf->modifier = max_mem;
    }
}//end study_memorize

/* calculates study range bonus, also used in skills.3 for do study */
inline int study_range_bonus( int level ){
  int range;
  range =  ((level > 38? 1 : 0)
	    + (level > 40? 1 : 0)
	    + (level > 45? 1 : 0)
	    + (level > 46? 1 : 0)
	    + (level > 49? 1 : 0));
  return (range);
}

/* study_target     */
/* link to effect.c */
/* written by Viri  */
inline void study_learn(CHAR_DATA* ch, AFFECT_DATA* paf)
{
  //This is the function called in "study" state of gen_study
  //called each tick by the study_tick update if the state is set.
  //Learn is very much skill orientated.

  //gen affect data passed through paf.

  /*
    1) Check if victim is within "range" rooms.
    - if vicitm is not in range display message,return.
    2) If victim is in range, calculate amount "learned"
    3) Set the learned amount.
  */

  //pointers
  CHAR_DATA* vch;

//data holders.
  int fail = 0;
  int learned = 0;
  int dir = 0;//direction to target
//flags
  bool  fTargetFound = FALSE;

//const
 const int learn_mod = 6;
 const int lvl_mod = 5;
 const int fail_mod = 2;
 int range =  effect_table[GN_POS(gen_study)].value2[0];

 bool fFalcon = get_skill(ch, skill_lookup("falcon eye")) > 1;

 /* bonus to range by level */
 range += study_range_bonus(ch->level) + fFalcon ? 2 : 0;

 /* check if target in the game, if is, check if in range */
 if ( (vch = get_char(paf->string)) != NULL){
/* cut range down on blurring and sneak */
   if (is_affected(vch, skill_lookup("blur"))
       || IS_SET(race_table[vch->race].aff, AFF_SNEAK) )
     range = UMAX(1, range / 2);
   else if (vch->in_room && vch->in_room->area != ch->in_room->area)
     range = 0;

   /* We search BUT fPassDoor is FALSE so doors obstruct */
   dir = find_first_step(ch->in_room, vch->in_room, range, FALSE, NULL);

   if (is_affected(vch, gsn_coffin)){
     range = 0;
   }
   switch(dir)
     {
     case BFS_ERROR:
       send_to_char("Hmm... something seems to be wrong.\n\r", ch);
       fTargetFound = FALSE;
       break;
     case BFS_NO_PATH:
        send_to_char("You can't see your victim from here.\n\r", ch);
	fTargetFound = FALSE;
        break;
     case BFS_ALREADY_THERE:
     default:
       fTargetFound = TRUE;
     }//END error checking
 }//end Char still around.

 if (!fTargetFound || !can_see(ch, vch))
   {
     send_to_char("Your target absent you learn nothing.\n\r", ch);
     return;
   }
 else if (is_affected(vch, gen_watchtower)){
   act("Protected by the Justice Watchtower, you learn nothing of $N.", ch, NULL, vch, TO_CHAR);
   return;
 }
   /* check for shadow mastery */
 else if (get_skill(vch, gsn_shadowmaster) > 1){
   act("Try as you might you cannot spot a single weak spot in $N.", ch, NULL, vch, TO_CHAR);
   return;
 }

 /* BFS_HERE is -2, cant index into array withit */
 if (dir >= 0)
   act("You discretly turn your attention $t.", ch, dir_name[dir], NULL, TO_CHAR);
 else
   act("You discretly turn your attention to $N.", ch, NULL, vch, TO_CHAR);

//Target found, we study him.
//calculate the fail lvl.
  fail    = (100 - get_skill(ch, gsn_study)) * fail_mod;//(50 at 75%, 0 at 100%);

//incliude level diff.
  fail = UMAX(0, fail + ((vch->level - ch->level) * lvl_mod));
  fail = UMAX(0, fail + ch->level - 30);
//get the amount we learned.
  learned = UMAX(0, number_percent() - fail) / learn_mod; //(max of 20% at skill = 100);
  learned += UMAX(0, (ch->level - 35) / 5);
  //boost by 50% for falcon eye
  if (fFalcon){
    learned += learned / 2;
    learned = URANGE(3, learned, 25);
  }
  else
    learned = URANGE(1, learned, 25);



//we now display info message on how much info we learned.
  //check of ceiling.
  if (paf->modifier > 59)
    {
      act("You've learned as much as you can about $N.", ch, NULL, vch, TO_CHAR);
      learned = 0;
    }
  else if (learned < 5)
    send_to_char("You fail to notice anything of use.\n\r", ch);
  else if (learned <= 10)
    act("You think you have just noticed something useful about $N.", ch, NULL, vch, TO_CHAR);
      else if (learned <= 15)
	act("You notice a distinct pattern in $N's behavior.", ch, NULL, vch, TO_CHAR);
  else
    act("Observing $N you notice a potential weakness!.", ch, NULL, vch, TO_CHAR);

  //transfer what we have learned.
  paf->level += learned;

  //DEBUG
  //  sendf(ch, "fail: %d, learned: %d, tot learned: %d.\n\r", fail, learned, paf->level);

  check_improve(ch, gsn_study, TRUE, 15);


}//end study_learn




/* study INIT */
/* called in effect.c only */
/* Created by Viri */

int study_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  if (gn != gen_study)
    return -1;

  return TRUE;
}


/* study KILL */
/* called in effect.c only */
/* written by Viri */
int study_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_study)
    return -1;

  //we only show the message if this is not extraction.
  if (!ch->extracted)
    sendf(ch, "The last remaments of %s's behaviour patterns disappear from your memory.\n\r", paf->string);

  //we free the string.
  if (paf->string != NULL)
    free_string(paf->string);
  paf->string = NULL;
  return TRUE;
}


/* study_tick */
/* links into effect.c only */
/* written by Viri */
int study_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //This is the main function of the study skills.
  //There are 3 possible states
  /*
    1) learn (gsn_study set)                    //study_learn
    2) memorize (gsn_study NOT set, level != 0) //study_mem
    - if not resting, rest.
    3) forget(gsn_study NOT set, level == 0)    //study_forget
  */
  //Pointers
  AFFECT_DATA* paf;

//flags
  int wState = STUDY_MEMORIZE;

//First we select which state we are in.

//EZ
  if (ch == NULL)
    return -1;

  if ( (paf = affect_find(ch->affected, gen_study)) == NULL)
    return -1;

//Check if studing target.
  if (is_affected(ch, gsn_study))
    wState = STUDY_LEARN;

//Now we know what state we are in.
  switch (wState)
    {
    case STUDY_LEARN:
      study_learn(ch, paf);break;
    case STUDY_MEMORIZE:
      study_memorize(ch, paf);break;
    default:
      return FALSE;
    }//end state select.

//final check.
if (paf->modifier < 1 && wState != STUDY_LEARN)
  affect_strip(ch, gen_study);

 return TRUE;
}//end study_tick

/* study_none */
/* links into effect.c only */
/* written by Viri */
/* this is used to interrupt meditation, and lose what one has learned. */
int study_none(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf;
  if (gn != gen_study)
    return -1;

  if ( (paf = affect_find(ch->affected, gen_study)) != NULL)
    {
      if (paf->level > 0)
	{
	  paf->level = 0;
	  send_to_char("Your concentration broken, you quickly forget what you have not memorized.\n\r\n\r", ch);
	}
    }

  return TRUE;
}//end study_none

int study_precommand(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  int cmd = (int) *int1;
  AFFECT_DATA* paf;
  if (gn != gen_study)
    return -1;

  if (!(ch->position == POS_MEDITATE && cmd_table[cmd].meditate))
    return FALSE;

  if ( (paf = affect_find(ch->affected, gen_study)) != NULL)
    {
      if (paf->level > 0)
	{
	  paf->level = 0;
	  send_to_char("\n\rYour concentration broken, you quickly forget what you have not memorized.\n\r\n\r", ch);
	}
      ch->position = POS_STANDING;
    }

  return TRUE;
}//end study_none


/* study_info */
/* linked into act_wiz.c and act_info.c */
/* written by viri */
char* study_info(CHAR_DATA* ch, bool fImm, CHAR_DATA* vic)
{
  char buf [MIL];
  static char output[MIL];
  AFFECT_DATA* paf;
  int dMsg = 0;
  int ass_skill = 0;
  int chance = 0;

  output[0] = 0;
  if ( (paf = affect_find(ch->affected, gen_study)) == NULL)
    {
      send_to_char("PAF NOT FOUND\n\r",ch);
    return output;
    }
  //we get the assassinate skill for proper chance
  ass_skill = get_skill(ch,gsn_assassinate);
  chance = paf->modifier;

  //now we scale the chance according to skill.
	if (ass_skill < 50) chance = 10 * chance / 100;
	else if (ass_skill < 60) chance = 10 * chance / 100;
	else if (ass_skill < 70) chance = 20 * chance / 100;
	else if (ass_skill < 75) chance = 35 * chance / 100;
	else if (ass_skill < 80) chance = 55 * chance / 100;
	else if (ass_skill < 85) chance = 60 * chance / 100;
	else if (ass_skill < 90) chance = 85 * chance / 100;
	else if (ass_skill < 95) chance = 90 * chance / 100;
	else if (ass_skill < 99) chance = 95 * chance / 100;
	else if (ass_skill >= 100) chance = 110 * chance / 100;

 //we select which message we are inlucding.
  if (chance < 10)
    dMsg = 0;
  else if (chance < 20)
    dMsg = 1;
  else if (chance < 30)
    dMsg = 2;
  else if (chance < 40)
    dMsg = 3;
  else if (chance < 50)
    dMsg = 4;
  else if (chance < 55)
    dMsg = 5;
  else
    dMsg = 6;

  //no we string the message.
if (!fImm)
  sprintf(buf, "Assassinate:   %s%s.", study_msg_table[dMsg].desc, (dMsg == 6? "" : " assassinate %s"));
 else
   sprintf(buf, "Assassinate:   %%s at %d.", paf->modifier);

 //send out the string.
 sprintf(output, buf, paf->string);
 return (output);
}



//--------------INSECT SWARM-----------------//
/* insect swarm Init */
/* called in effect.c only */
/* Created by Viri */
int insect_swarm_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1,
int *int2, int flag)
{

return TRUE;
}


/* insect swarm Tick */
/* called in effect.c only */
/* Created by Viri */
int insect_swarm_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*
    -This is the damage portion of the insect swarm.
    - It happens on use and every tick as long as duration lasts.
    - it damages unless person is on water in which case it only does
    - the poison effect, otherwise none.
    - can do: none or poison or plauge or drain det. by constsnts.
  */
  static const sh_int dam_each[] =
  {
    0,
    0,   0,   0,   0,   0,  0,   0,   0,   0,   0,
    20,  21,  22,  23,  24, 25,  26,  27,  28,  29,
    30,  35,  40,  45,  50, 55,  60,  65,  70,  75,
    80,  80,  80,  80,  80, 80,  80,  80,  80,  80,
    81,  82,  83,  84,  85, 86,  87,  88,  89,  90
  };

  AFFECT_DATA* paf;
  AFFECT_DATA af;

  CHAR_DATA* och;

  char buf[MIL];

  //Const
  const int non_lvl = 20;
  const int poi_lvl = 50;
  const int pla_lvl = 90;

  //data
  int dam = 0;
  int type = 0; //0=NONE, 1=POISON, 2=PLAUGE, 3=DRAIN.
  int chance = 0;
  int sn = 0;
  int level = 0;

  //flags
  bool fUnlife = FALSE;
  bool fWater = FALSE;


 //EZ
  if (ch == NULL)
    return -1;


//check for swarm on the char.
  if ( (paf = affect_find(ch->affected, gen_insect_swarm)) == NULL)
    return FALSE;


//if original caster is not around we strip.
  if ( (och = get_char(paf->string))== NULL)
    {
      affect_strip(ch, gen_insect_swarm);
      return TRUE;
    }

//Damage calc.
  level= UMIN(och->level, sizeof(dam_each) / sizeof(dam_each[0]) - 1);
  level= UMAX(0, level);
  dam= number_range(dam_each[level] / 3, dam_each[level]);

  //DEBUG
  //  sendf(ch, "Name: %s\n\r", och->name);
//check for reasons not to go through.
//Water:
  if (ch->in_room->sector_type == SECT_WATER_SWIM
      || ch->in_room->sector_type == SECT_WATER_NOSWIM)
    fWater = TRUE;

//Unlife
  if (IS_SET(ch->act, ACT_UNDEAD)
      || ch->class == class_lookup("vampire")
      || ch->race == race_lookup("demon"))
    fUnlife = TRUE;

//Now we select which damage we do:
  chance = number_percent();

  if (chance < non_lvl) type = 0;
  else if (chance < poi_lvl) type = 1;
  else if (!IS_GOOD(och) && chance < pla_lvl) type = 2;
  else type = 3;

//Give the message.
  act("A swarming cloud of voracious insects descends upon $n!", ch, NULL, NULL, TO_ROOM);
  act("A swarming cloud of voracious insects descends upon you!", ch, NULL, NULL, TO_CHAR);


//now we check some special cases.
  // Druids are too peacfull with nature //
  if (ch->class == class_lookup("druid"))
    {
      act("The insects swarm around $n and then quickly disperse.", ch, NULL, NULL, TO_ROOM);
      act("The insects swarm around you and then quickly disperse sensing your peacefull disposition to nature.", ch, NULL, NULL, TO_CHAR);
      affect_strip(ch, gen_insect_swarm);
      return TRUE;
    }


//We yell.
  switch (type)
    {
    case 0: sprintf(buf, "Help! The locust of storms is after me!");break;
    case 1: sprintf(buf, "Help! The locust of gods is after me!");break;
    case 2: sprintf(buf, "Help! The swarm of ages is after me!");break;
    case 3: sprintf(buf, "Help! The wrath of gaia is upon me!");break;
    }
  j_yell(ch,buf);

//Duck under water.
  if (fWater)
    {
//check if this is not poison as poison insects bite under water :)
      if (type != 1)
	{
	  act("You duck underwater and avoid the deadly swarm.", ch, NULL, NULL, TO_CHAR);
	  act("$n ducks underwater and avoids the deadly swarm.", ch, NULL, NULL, TO_ROOM);
	  return TRUE;
	}//end water and not poison insects
      else
	{
	  act("You duck underwater in vain, as the horrible insects begin to fill the water around you!.", ch, NULL, NULL, TO_CHAR);
	  act("$n ducks underwater in vain as the insects begin to quickly saturate the water around you!.", ch, NULL, NULL, TO_ROOM);
	  //we make water vuln people suffer here.
	  if (check_immune(ch, DAM_DROWNING,TRUE) == IS_VULNERABLE)
	    dam *= 3;
	  else
	    dam = 3 * dam / 2;
	}//end water and poison insects
    }//end water.




//do the damage.
  // Unlife kills of the swarm
  if (fUnlife)
    {
      dam /= 5;
      damage(och,ch,dam,gen_insect_swarm,DAM_PIERCE,FALSE);
      act("The deadly insects seem to die in scores even as they gouge and bite.", ch, NULL, NULL, TO_ALL);
      affect_strip(ch, gen_insect_swarm);
      return TRUE;
    }

  //Do rest of effects here
//do the effects.
//check if there is an effect, if so we increase it other wise we set it.

  switch (type)
    {
      // POISON
    case 1:
      if (!is_affected(ch, gsn_poison))
	spell_poison(sn, och->level, och, ch, TARGET_CHAR);
      break;
// PLAUGE //
    case 2:
      if (!is_affected(ch, gsn_plague))
	spell_plague(gsn_plague, och->level, och, ch, TARGET_CHAR);
      break;

// DRAINED //
    case 3:
      af.type = gsn_drained;
      af.where = TO_AFFECTS;
      af.level = och->level;
      af.location = APPLY_NONE;
      af.duration = number_fuzzy(1);
      af.modifier = 0;
      af.bitvector = 0;
      if (is_affected(ch, gsn_drained))
	affect_join(ch,&af);
      else
	affect_to_char(ch, &af);
      act_new("Your vitality flees your body.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
      act("$n suddenly looks feverish and weak.",ch,NULL,NULL,TO_ROOM);
      break;
    }//end select type.

//now we damage

   damage(ch,ch,dam,gen_insect_swarm,DAM_PIERCE,FALSE);

//end we are done!
    return TRUE;
}//end insect_Swarm init


/* insect swarm INIT */
/* called in effect.c only */
/* Created by Viri */

int insect_swarm_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_insect_swarm)
    return -1;
//we run the room_sepc_update to maybe remove the SPEC_UPDATE bit.
  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  room_spec_update(ch->in_room);


return TRUE;
}



//=========PANIC==========//
/* panic_init */
/* linked into effect.c */
/* written by Viri */
int panic_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  //run during the init and only once.
  if (paf->location != APPLY_DAMROLL)
    return FALSE;

  act("A surge of fear induced power begins coursing through your veins.",ch , NULL, NULL, TO_CHAR);
  act("$n's eyes dart madly as $e panics.", ch, NULL, NULL, TO_ROOM);
  return TRUE;
}




/* panic_postcombat */
/* linked into effect.c */
/* written by Viri */
int panic_postcombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* This is the check for initilization of the panic effect
     for berzerkers.
     - The effect only is check if zerker is in combat and is blinded.
     - chance to go through is based on % of current health.
     - chance is current (100 - %health) * health_mod.
     so:
     100 =  0;
     80  =  40;
     60  =  80;
     40  = 120;
     20  = 160;
     - NOTE chance is PER  round of combat.
  */

  AFFECT_DATA af;
  AFFECT_DATA* paf;
//data
  int chance = 0;
  int per_health = 0;
//const
  const int health_mod = 2;

//bool
  bool fPanic = FALSE;
  bool fBlind = FALSE;


//First we check the two bools that we will need to use.
 if (IS_AFFECTED(ch, AFF_BLIND)) fBlind = TRUE;
 if (is_affected(ch, gen_panic)) fPanic = TRUE;

/*
- If blind and paniced do nothing ***
- If NOTBlind and NOTPaniced do notthing***

- If blind an NOT paniced we try to panic.
- If NOTBlind and paniced try to snap out.
*/

//This is the two most common states. (***)
 if ( !(fBlind || fPanic) || (fBlind && fPanic) )
   return FALSE;

//check the other two possibilites.
  per_health = 100 * ch->hit / ch->max_hit;

  if (fBlind && !fPanic)
    {
      //calculate the chance
      chance = (100 - per_health) * health_mod;
      //see if we run the effect.
      if (number_percent() < chance)
	{
	  int sn = 0;
	  //calm check.
	  if (is_affected(ch, sn = skill_lookup("calm")))
	    if (!check_dispel(ch->level,ch,sn))
	      {
		send_to_char("As you are about to panic, as sense of calm and peace comes over you.\n\r", ch );
		return FALSE;
	      }
	  //We need to find the duration of this blind.
	  //we run through all pafs looking for AFF_BLIND

	  //if we dont find anything, we set it to 5 just in case.
	  af.duration = 5;

	  for(paf=ch->affected;paf!=NULL;paf=paf->next)
	    if (paf->bitvector == AFF_BLIND)
	      {
		af.duration = paf->duration;
		break;
	      }

	  //set the effect.
	  af.type = gen_panic;
	  af.where = TO_AFFECTS;
	  af.level = ch->level;
	  af.bitvector = AFF_HASTE;

	  af.location = APPLY_DAMROLL;
	  af.modifier = ch->level / 10;
	  affect_to_char(ch, &af);

	  af.location = APPLY_HITROLL;
	  af.modifier = ch->level / 10;
	  affect_to_char(ch, &af);
	  return TRUE;
	}
      return FALSE;
    }//end if Blind and not panic.

  //try to snap out of panic.
  if (!fBlind && fPanic)
    if (number_percent() < per_health)
      {
	affect_strip(ch, gen_panic);
	return TRUE;
      }
  return FALSE;
}//end panic_postdamage


/* panic_tick */
/* linked into effect.c */
/* written by Viri */
int panic_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //This is a panic check. If character is paniced and not blind he snaps out based on %hp.

//data
  int per_health = 0;

//get health.
  per_health = 300 * ch->hit / ch->max_hit / 2;

//see if we strip the effect.
  if ( is_affected(ch, gen_panic) && !IS_AFFECTED(ch, AFF_BLIND) )
    if (number_percent() < per_health)
      {
	affect_strip(ch, gen_panic);
	return TRUE;
      }
  return FALSE;
}//end panic tick.


/* panic_precommand */
/* linked into effect.c */
/* written by Viri */
int panic_precommand(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //run only if the character is affected by panic and enters any commands
  // int1 carries the cmd.
  int cmd = (int) *int1;

  if (!(strcmp("rest", cmd_table[cmd].name)
      && strcmp("sleep", cmd_table[cmd].name)
      && strcmp("sit", cmd_table[cmd].name)
      && strcmp("camp", cmd_table[cmd].name)
      && strcmp("meditate", cmd_table[cmd].name)) )
    {
      send_to_char("You are too worked up to rest!\n\r", ch);
      return TRUE;
    }
  else
    return FALSE;
}//edn panic precommand.


/* panic_kill */
/* linked into effect.c */
/* written by Viri */
int panic_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_panic)
    return -1;
  //panic sts 2 effects, one dam one hit, we only show message on one.
  if (paf->location != APPLY_DAMROLL)
  return FALSE;
//we run the room_sepc_update to maybe remove the SPEC_UPDATE bit.
  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}





//----------------MALFORM WEAPON-------------//

//--------=========NONE GEN FUNCTIONS============----------//
/* LivWep_level_check */
/* written by viri */
  /* REUSABLE */
/* returns FALSE on error. */
bool LivWep_level_check(OBJ_DATA* obj, int gen)
{
  /*
    This is the main function that does decision checking on
    weather the weapon should increase in rank or not.
    Everything is flexible ONLY beacuse the lvl gain/loss is run
    through the EFF_UPDATE_NONE of the gen. Hence this can be used for
    any weapon, with exception of the set_level function which is called
    later down the chain and is hardcoded for malform.

    The level data is taken from the val1 array of the cell in
    effect_table specifed by the gen passed into the function.

    Max_Level is taken from first cell of val2;
  */
  AFFECT_DATA* paf;

  //data
  int MaxLevel = effect_table[GN_POS(gen)].value2[0];
//bool
  bool fRedLvl = FALSE;
  bool fIncLvl = FALSE;
  bool fSuccess = TRUE;

  //  char buf[MIL];


  if ( (paf = affect_find(obj->affected, gen)) == NULL)
    {
      bug("LivWep_level_check: LivingWeapon effect not found.", 0);
      return FALSE;
    }
  if (!IS_GEN(gen))
    {
      bug("LivWep_level_check: argument passed is not a GEN number.", 0);
      return FALSE;
    }

  //small safety here.
  paf->modifier = UMAX(0, paf->modifier);

  //check exp value on the weapon for lvl loss.
  fRedLvl = (paf->modifier < effect_table[GN_POS(gen)].value1[paf->level]? TRUE : FALSE);
  //check exp value on the weapon for lvl gain.
  fIncLvl = (paf->modifier > effect_table[GN_POS(gen)].value1[paf->level + 1] ? TRUE: FALSE);


  //DEBUG
  /*sprintf(buf, "cur lvl: %d, max: %d cur exp: %d, exp lowe: %d, exp high: %d, RED: %d, INC: %d\n\r",
    paf->level,
    MaxLevel,
    paf->modifier,
    effect_table[GN_POS(gen)].value1[paf->level],
    effect_table[GN_POS(gen)].value1[paf->level + 1],
    fRedLvl,
    fIncLvl);
    log_string(buf);
  */


//now we aculty do things.
  if (fRedLvl)
    {
      //the last argument decides if the level will be decreased or increased.
      fSuccess = run_effect_update(NULL, obj, paf, gen, NULL, NULL, FALSE, EFF_UPDATE_NONE);
      fSuccess = LivWep_level_check(obj, gen);

    }
  else if ( fIncLvl && (paf->level < MaxLevel) )
    {
      fSuccess = run_effect_update(NULL, obj, paf, gen, NULL, NULL, TRUE, EFF_UPDATE_NONE);
      fSuccess = LivWep_level_check(obj, gen);
    }

  //and as a last check we run the function over again.


  return fSuccess;

}



/* LivWep_basic_modify */
/* written by Viri */
  /* REUSABLE */
/* returns FALSE on error. */
bool LivWep_basic_modify(OBJ_DATA* obj, int die1, int die2, int Hit, int Dam)
{
  /*
this is the workhorse level function that changes basic weapon stats
as passed. die1/2 refers to the dice on weapon being changed.
If the enchantements dont exist they are set rather then modified and
weapon is made to be enchanted.
-This is totaly flexile function with no hardcoded references to malform weapon.
  */

  AFFECT_DATA* paf, *paf_next;
  AFFECT_DATA af;
  CHAR_DATA* ch = NULL;

//const
   const int gsn_enchant_weapon = skill_lookup("enchant weapon");
//bools
  bool dam_found = FALSE;
  bool hit_found = FALSE;
  bool eff_set = FALSE;
  bool is_worn = FALSE;

  //Safety
  if (obj->item_type != ITEM_WEAPON)
    {
      bug("LivWep_basic_modify: Modify attempted on a non-weapon", 0);
      return FALSE;
    }
  //Malformed weapons are ALWAYS enchanted.
  if (!obj->enchanted)
    {
      bug("LivWep_basic_modify: Target weapon not enchanted!", 0);
      return FALSE;
    }

  //Check if worn or carried.
  if ((is_worn = (obj->wear_loc == WEAR_NONE ? FALSE : TRUE)) == TRUE)
    ch = obj->carried_by;



  //Dice
  if (die1 != 0)
    {
      /* check for extreme avg damage */
      obj->value[1] = UMAX(1, obj->value[1] + die1);
      if ( ((1 + obj->value[2]) * obj->value[1] / 2) > 38)
	obj->value[1] = UMAX(1, obj->value[1] - die1);
      eff_set = TRUE;
    }
  if (die2 != 0)
    {
      obj->value[2] = UMAX(1, obj->value[2] + die2);
      if ( ((1 + obj->value[2]) * obj->value[1] / 2) > 38)
	obj->value[2] = UMAX(1, obj->value[2] - die2);
      eff_set = TRUE;
    }

  //enchantements.
  //first we try to find an existing enchantement.
  for ( paf = obj->affected; paf != NULL; paf = paf_next )
    {
      paf_next = paf->next;
      if ( paf->location == APPLY_DAMROLL )
	{
	  //If weapon is eq. we reset the modifiers on character.
	  if (is_worn && ch != NULL)
	    affect_modify(ch, paf, FALSE);

	  paf->modifier += Dam;
	  eff_set = TRUE;
	  dam_found = TRUE;

	  //End once again we apply effects if weapon is worn.
	  if (is_worn && ch != NULL)
	    affect_modify(ch, paf, TRUE);

	  if (paf->modifier == 0)
	    affect_remove_obj(obj, paf);
	}
      else if ( paf->location == APPLY_HITROLL )
	{
	  //If weapon is eq. we reset the modifiers on character.
	  if (is_worn && ch != NULL)
	    affect_modify(ch, paf, FALSE);

	  paf->modifier += Hit;
	  eff_set = TRUE;
	  hit_found = TRUE;

	  if (is_worn && ch != NULL)
	    affect_modify(ch, paf, TRUE);

	  //nuke the affect if zero
	  if (paf->modifier == 0)
	    affect_remove_obj(obj, paf);
	}
    }//end lookinfg for exisint effects.

  //now we add effects if needed.
  if ( (dam_found && hit_found) || (Dam == 0 && Hit == 0) )
      return (eff_set ? TRUE : FALSE);

  af.type = gsn_enchant_weapon;
  af.where = TO_OBJECT;
  af.duration = -1;
  af.level = obj->level;
  af.bitvector = 0;

  //the following effects are applied to char. automaticly.
  if (Dam  != 0 && !dam_found)
    {
      eff_set = TRUE;
      af.location = APPLY_DAMROLL;
      af.modifier = Dam;
      affect_to_obj(obj, &af);
    }
  if (Hit != 0 && !hit_found)
    {
      eff_set = TRUE;
      af.location = APPLY_HITROLL;
      af.modifier = Hit;
      affect_to_obj(obj, &af);
    }
  //END
  return (eff_set ? TRUE : FALSE);

}

//MALFORM LEVEL EFFECTS BY FUNCTION:


inline bool mal_level0(OBJ_DATA* obj, AFFECT_DATA* paf, bool fClear)
{
  /* set level 0 effects on weapon */
/*EFFECTS:
val[1]    val[2]    Hit   Dam              Other
-1          -1       -2   -2                NONE
*/
int dClear = (fClear? -1 : 1);
 int die1 = -1 * dClear;
 int die2 = -1 * dClear;
 int Hit  = -2 * dClear;
 int Dam  = -2 * dClear;


 if (!LivWep_basic_modify(obj, die1, die2, Hit, Dam))
   return FALSE;
return TRUE;
}//END LEVEL0


inline bool mal_level1(OBJ_DATA* obj, AFFECT_DATA* paf, bool fClear)
{
  /* set level 1 effects on weapon */
/*EFFECTS:
val[1]    val[2]    Hit   Dam              Other
 0           0       +2   +1                no_drop
*/
int dClear = (fClear? -1 : 1);
 int die1 =  0 * dClear;
 int die2 =  0 * dClear;
 int Hit  = +2 * dClear;
 int Dam  = +1 * dClear;


 if (!LivWep_basic_modify(obj, die1, die2, Hit, Dam))
   return FALSE;

 //Non basic Effects.
 if (fClear)
   REMOVE_BIT(obj->extra_flags,ITEM_NODROP);
 else
   SET_BIT(obj->extra_flags,ITEM_NODROP);

 return TRUE;
}//END LEVEL0


inline bool mal_level2(OBJ_DATA* obj, AFFECT_DATA* paf, bool fClear)
{
  /* set level 2 effects on weapon */
/*EFFECTS:
val[1]    val[2]    Hit   Dam              Other
 +1           0      +2   +1                no_remove
*/
int dClear = (fClear? -1 : 1);
 int die1 =  1 * dClear;
 int die2 =  0 * dClear;
 int Hit  = +2 * dClear;
 int Dam  = +1 * dClear;


 if (!LivWep_basic_modify(obj, die1, die2, Hit, Dam))
   return FALSE;

 //Non basic Effects.
 if (fClear)
   REMOVE_BIT(obj->extra_flags,ITEM_NOREMOVE);
 else
   SET_BIT(obj->extra_flags,ITEM_NOREMOVE);

 return TRUE;
}//END LEVEL1


inline bool mal_level3(OBJ_DATA* obj, AFFECT_DATA* paf, bool fClear)
{
  /* set level 1 effects on weapon */
/*EFFECTS:
val[1]    val[2]    Hit   Dam              Other
 0          +1       +1   +1                WEP_EFF_1-4
*/


  AFFECT_DATA af;
  AFFECT_DATA* buf;
  CHAR_DATA* ch = NULL;

//data
  int dClear = (fClear? -1 : 1);
  int die1 =  0 * dClear;
  int die2 = +1 * dClear;
  int Hit  = +1 * dClear;
  int Dam  = +1 * dClear;
  int effect = 0;

//bool
  bool is_worn = FALSE;
 if (!LivWep_basic_modify(obj, die1, die2, Hit, Dam))
   return FALSE;

 //Non basic Effects.
 //we get which effect it is.
 if (IS_SET(paf->bitvector, WEP_EFF_1))
   effect = 1;
 else if (IS_SET(paf->bitvector, WEP_EFF_2))
   effect = 2;
 else if (IS_SET(paf->bitvector, WEP_EFF_3))
   effect = 3;
 else if (IS_SET(paf->bitvector, WEP_EFF_4))
   effect = 4;
 else
   {
     bug("mal_level3: Proper bitvector flag not found.", 0);
     return FALSE;
   }

 //get teh effect data.
 switch (effect)
   {
   case 1:
     af.location = APPLY_STR;
     af.modifier = 1;
     break;
   case 2:
     af.location = APPLY_DEX;
     af.modifier = 1;
     break;
   case 3:
     af.location = APPLY_HIT;
     af.modifier = 5;
     break;
   case 4:
     af.location = APPLY_SAVING_SPELL;
     af.modifier = -1;
     break;
   }

 //bonus for 2h
 if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
   af.modifier *=2;

 //Check if worn or carried.
 if ((is_worn = (obj->wear_loc == WEAR_NONE ? FALSE : TRUE)) == TRUE)
   ch = obj->carried_by;

 //now we act on the effects.
 if (fClear)
   {
     AFFECT_DATA* buf_next;
     af.level = obj->level;
     af.modifier *= -1;
     //we look for the effects to be lowered.
     for ( buf = obj->affected; buf != NULL; buf = buf_next ){
       buf_next = buf->next;
       if ( buf->location == af.location )
	 {
	   //If weapon is eq. we reset the modifiers on character.
	   if (is_worn && ch != NULL)
	     affect_modify(ch, buf, FALSE);
	   buf->modifier += af.modifier;
	   buf->level = af.level;
	   //check if zero effect
	   if (buf->modifier == 0)
	     affect_remove_obj(obj, buf);
	   else
	     {
	       if (is_worn && ch != NULL)
		 affect_modify(ch, buf, TRUE);
	     }//end else
	 }//end if effec it found.
     }//end lower effect
   }//end remove effects.
 else
   {
     af.type = skill_lookup("enchant weapon");
     af.where = TO_NONE;
     af.level = obj->level;
     af.duration = -1;
     af.bitvector = 0;
     affect_to_obj(obj, &af);
     //If weapon is eq. we reset the modifiers on character.
     if (is_worn && ch != NULL)
       affect_modify(ch, paf, TRUE);
   }//end set effect.

 return TRUE;
}//END LEVEL3


inline bool mal_level4(OBJ_DATA* obj, AFFECT_DATA* paf, bool fClear)
{
  /* set level 4 effects on weapon */
/*EFFECTS:
val[1]    val[2]    Hit   Dam              Other
  0           0      +2   +1                no_uncurse
*/
int dClear = (fClear? -1 : 1);
 int die1 =  0 * dClear;
 int die2 =  0 * dClear;
 int Hit  = +2 * dClear;
 int Dam  = +1 * dClear;


 if (!LivWep_basic_modify(obj, die1, die2, Hit, Dam))
   return FALSE;

 //Non basic Effects.
 if (fClear)
   {
     REMOVE_BIT(obj->extra_flags,ITEM_NOUNCURSE);
     REMOVE_BIT(obj->extra_flags,ITEM_PSI_ENHANCE);
   }
 else
   {
     SET_BIT(obj->extra_flags,ITEM_NOUNCURSE);
     SET_BIT(obj->extra_flags,ITEM_PSI_ENHANCE);
   }

 return TRUE;
}//END LEVEL4


inline bool mal_level5(OBJ_DATA* obj, AFFECT_DATA* paf, bool fClear)
{
  /* set level 5 effects on weapon */
/*EFFECTS:
val[1]    val[2]    Hit   Dam              Other
 0          0       +2   +0                WEP_EFF_1-4
*/

  /* NOTE: is_worn, ch, and the check for obj carried has been remmed out
     as the spells changes are not affected by location of obj.
  */

  OBJ_SPELL_DATA sp;
  OBJ_SPELL_DATA* buf;
  //  CHAR_DATA* ch = NULL;  NOT USED HERE FOR NOW.

  //messages for activation of the spell
  char* SeeMsg = "$p emits a loud hum.";
  char* BlindMsg = "$p emits a loud hum.";

//data
  int dClear = (fClear? -1 : 1);
  int die1 =  0 * dClear;
  int die2 = +0 * dClear;
  int Hit  = +2 * dClear;
  int Dam  =  0 * dClear;

  int effect = 0;
//bool
  //  bool is_worn = FALSE; NOT USED HERE
 if (!LivWep_basic_modify(obj, die1, die2, Hit, Dam))
   return FALSE;

 //Non basic Effects.
 //we get which effect it is.
 if (IS_SET(paf->bitvector, WEP_EFF_5))
   effect = 5;
 else if (IS_SET(paf->bitvector, WEP_EFF_6))
   effect = 6;
 else if (IS_SET(paf->bitvector, WEP_EFF_7))
   effect = 7;
 else
   {
     bug("mal_level5: Proper bitvector flag not found.", 0);
     return FALSE;
   }

 //get the spell data
 switch (effect)
   {
   case 5:
     sp.spell = skill_lookup("poison");
     sp.target = SPELL_TAR_VICT;
     break;
   case 6:
     sp.spell = skill_lookup("plague");
     sp.target = SPELL_TAR_VICT;
     break;
   case 7:
     sp.spell = skill_lookup("faerie fire");
     sp.target = SPELL_TAR_VICT;
     break;
   }

 //safety
 if (sp.spell < 1 || sp.spell > MAX_SKILL)
   bug("mal_level5: No spell found.", 0);

 /*NOT USED IN HERE, we leave it just in case
 //Check if worn or carried.
 if ((is_worn = (obj->wear_loc == WEAR_NONE ? FALSE : TRUE)) == TRUE)
   ch = obj->carried_by;
 */

 //now we act on the effects.
 if (fClear)
   spell_obj_strip(obj, sp.spell);
 else
   {
     //set he spell info.
     sp.percent = LVL5_SPELL_FRE;
     //bonus for 2h
     if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
       sp.percent*=2;
     //set the spell using noduplication.
     buf = spell_to_obj(obj, &sp, FALSE);
     //safety
     if (buf == NULL)
       bug("mal_level5: spell_to_obj returned NULL", 0);
     //now that we have the pointer tothe spell we attach messages.
     buf->message = str_dup(SeeMsg);
     buf->message2 = str_dup(BlindMsg);
   }//end set spell

 return TRUE;
}//END LEVEL5


inline bool mal_level6(OBJ_DATA* obj, AFFECT_DATA* paf, bool fClear)
{
  /* set level 6 effects on weapon */
/*EFFECTS:
val[1]    val[2]    Hit   Dam              Other
  1           0      +1   +2                no_sac, has_owner
*/
int dClear = (fClear? -1 : 1);
 int die1 =  1 * dClear;
 int die2 =  0 * dClear;
 int Hit  = +1 * dClear;
 int Dam  = +2 * dClear;
 AFFECT_DATA* buf;
 AFFECT_DATA af;
 CHAR_DATA* ch;

 if (!LivWep_basic_modify(obj, die1, die2, Hit, Dam))
   return FALSE;

 //Non basic Effects.
 if (fClear)
   {
     if ( (buf = affect_find(obj->affected, gen_has_owner)) != NULL)
       affect_strip_obj( obj, gen_has_owner );
     REMOVE_BIT(obj->wear_flags,ITEM_NO_SAC);
     REMOVE_BIT(obj->wear_flags,ITEM_HAS_OWNER);
   }
 else
   {

     af.type = gen_has_owner;
     af.where = TO_NONE;
     af.level = obj->level;
     af.duration = -1;
     af.location = APPLY_NONE;
     af.modifier = 0;
     af.bitvector = 0;
     affect_to_obj(obj, &af);

     buf = affect_find(obj->affected, gen_has_owner);
     string_to_affect(buf, paf->string);
     SET_BIT(obj->wear_flags,ITEM_NO_SAC);
     SET_BIT(obj->wear_flags,ITEM_HAS_OWNER);

     //we send a message that he may brand it now.
     if ( (ch = obj->carried_by) != NULL)
       send_to_char("`@Your weapon has reached a level of power "\
		    "that deserves a proper name.\n\r Use cmd: "\
		    " \"brandweapon\" to give your item a proper name.\n\r``", ch);

   }

 return TRUE;
}//END LEVEL6



inline bool mal_level7(OBJ_DATA* obj, AFFECT_DATA* paf, bool fClear)
{
  /* set level 7 effects on weapon */
/*EFFECTS:
val[1]    val[2]    Hit   Dam              Other
 0          0        0    +2                WEP_EFF_8-11
*/

  /* NOTE: is_worn, ch, and the check for obj carried has been remmed out
     as the spells changes are not affected by location of obj.
  */

  OBJ_SPELL_DATA sp;
  OBJ_SPELL_DATA* buf;
  //  CHAR_DATA* ch = NULL;  NOT USED HERE FOR NOW.

  //messages for activation of the spell
  //messages for activation of the spell
  char* SeeMsg = "$p glows with an evil aura.";
  char* BlindMsg = "$p glows with an evil aura.";


//data
  int dClear = (fClear? -1 : 1);
  int die1 =  0 * dClear;
  int die2 = +0 * dClear;
  int Hit  =  0 * dClear;
  int Dam  = +2 * dClear;

  int effect = 0;

//bool
  //  bool is_worn = FALSE; NOT USED HERE
 if (!LivWep_basic_modify(obj, die1, die2, Hit, Dam))
   return FALSE;


 //get the new damage type based on religion
 //DEATH
 if (IS_SET(paf->bitvector, WEP_EFF_29))
   obj->value[3] = attack_lookup("kiss");
 //CHAOS
 else if (IS_SET(paf->bitvector, WEP_EFF_28))
   obj->value[3] = attack_lookup("tain");
 //KNOWLEDGE
 else if (IS_SET(paf->bitvector, WEP_EFF_27))
   obj->value[3] = attack_lookup("disrupt");
 //FAITH
 else
   obj->value[3] = attack_lookup("faith");

 //Non basic Effects. (8/9 Shadow, 10/11 Viri)
 //we get which effect it is.
 if (IS_SET(paf->bitvector, WEP_EFF_8))
   effect = 8;
 else if (IS_SET(paf->bitvector, WEP_EFF_9))
   effect = 9;
 else if (IS_SET(paf->bitvector, WEP_EFF_10))
   effect = 10;
 else if (IS_SET(paf->bitvector, WEP_EFF_11))
   effect = 11;
 else
   {
     bug("mal_level7: Proper bitvector flag not found.", 0);
     return FALSE;
   }

 //get the spell data
 switch (effect)
   {
   case 8:
     sp.spell = skill_lookup("blindness");
     sp.target = SPELL_TAR_VICT;
     break;
   case 9:
     sp.spell = skill_lookup("weaken");
     sp.target = SPELL_TAR_VICT;
     break;
   case 10:
     sp.spell = skill_lookup("curse");
     sp.target = SPELL_TAR_VICT;
     break;
   case 11:
     sp.spell = skill_lookup("energy drain");
     sp.target = SPELL_TAR_VICT;
     break;
   }

 //safety
 if (sp.spell < 1 || sp.spell > MAX_SKILL)
   bug("mal_level7: No spell found.", 0);

 /*NOT USED IN HERE, we leave it just in case
 //Check if worn or carried.
 if ((is_worn = (obj->wear_loc == WEAR_NONE ? FALSE : TRUE)) == TRUE)
   ch = obj->carried_by;
 */

 //now we act on the effects.
 if (fClear)
   spell_obj_strip(obj, sp.spell);
 else
   {
     //set he spell info.
     sp.percent = LVL7_SPELL_FRE;
     //bonus for 2h
     if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
       sp.percent*=2;
     //set the spell using noduplication.
     buf = spell_to_obj(obj, &sp, FALSE);
     //safety
     if (buf == NULL)
       bug("mal_level5: spell_to_obj returned NULL", 0);
     //now that we have the pointer tothe spell we attach messages.
     //different messages for gods.
     buf->message = str_dup(SeeMsg);
     buf->message2 = str_dup(BlindMsg);
   }//end set spell

 return TRUE;
}//END LEVEL7


inline bool mal_level8(OBJ_DATA* obj, AFFECT_DATA* paf, bool fClear)
{
  /* set level 8 effects on weapon */
/*EFFECTS:
val[1]    val[2]    Hit   Dam              Other
  0           1      0    +2                burnproof, invis
*/
int dClear = (fClear? -1 : 1);
 int die1 =  0 * dClear;
 int die2 =  1 * dClear;
 int Hit  =  0 * dClear;
 int Dam  = +2 * dClear;


 if (!LivWep_basic_modify(obj, die1, die2, Hit, Dam))
   return FALSE;

 //Non basic Effects.
 if (fClear)
   {
     REMOVE_BIT(obj->extra_flags,ITEM_BURN_PROOF);
     REMOVE_BIT(obj->extra_flags,ITEM_INVIS);
   }
 else
   {
     SET_BIT(obj->extra_flags,ITEM_BURN_PROOF);
     SET_BIT(obj->extra_flags,ITEM_INVIS);
   }

 return TRUE;
}//END LEVEL8



inline bool mal_level9(OBJ_DATA* obj, AFFECT_DATA* paf, bool fClear)
{
  /* set level 9 effects on weapon */
/*EFFECTS:
val[1]    val[2]    Hit   Dam              Other
 0          0       +1   +1                WEP_EFF_12-15
*/

  /* NOTE: is_worn, ch, and the check for obj carried has been remmed out
     as the spells changes are not affected by location of obj.
  */

  OBJ_SPELL_DATA sp;
  OBJ_SPELL_DATA* buf;
  //  CHAR_DATA* ch = NULL;  NOT USED HERE FOR NOW.

  //messages for activation of the spell
  char* SeeMsg = "The runes on $p squirm and glow.";
  char* BlindMsg = "The runes on $p squirm and glow.";

  //data
  int dClear = (fClear? -1 : 1);
  int die1 =  0 * dClear;
  int die2 = +0 * dClear;
  int Hit  = +1 * dClear;
  int Dam  = +1 * dClear;

  int effect = 0;
//bool
  //  bool is_worn = FALSE; NOT USED HERE
 if (!LivWep_basic_modify(obj, die1, die2, Hit, Dam))
   return FALSE;

 //Non basic Effects.
 //we get which effect it is.
 if (IS_SET(paf->bitvector, WEP_EFF_12))
   effect = 12;
 else if (IS_SET(paf->bitvector, WEP_EFF_13))
   effect = 13;
 else if (IS_SET(paf->bitvector, WEP_EFF_14))
   effect = 14;
 else if (IS_SET(paf->bitvector, WEP_EFF_15))
   effect = 15;
 else
   {
     bug("mal_level9: Proper bitvector flag not found.", 0);
     return FALSE;
   }

 //get the spell data
 switch (effect)
   {
   case 12:
     sp.spell = skill_lookup("dispel magic");
     sp.target = SPELL_TAR_VICT;
     break;
   case 13:
     sp.spell = skill_lookup("dispel good");
     sp.target = SPELL_TAR_VICT;
     break;
   case 14:
     sp.spell = skill_lookup("blasphemy");
     sp.target = SPELL_TAR_VICT;
     break;
   case 15:
     sp.spell = skill_lookup("vampiric touch");
     sp.target = SPELL_TAR_VICT;
     break;
   }

 //safety
 if (sp.spell < 1 || sp.spell > MAX_SKILL)
   bug("mal_level9: No spell found.", 0);

 /*NOT USED IN HERE, we leave it just in case
 //Check if worn or carried.
 if ((is_worn = (obj->wear_loc == WEAR_NONE ? FALSE : TRUE)) == TRUE)
   ch = obj->carried_by;
 */

 //now we act on the effects.
 if (fClear)
   spell_obj_strip(obj, sp.spell);
 else
   {
     //set he spell info.
     sp.percent = LVL9_SPELL_FRE;
     //bonus for 2h
     if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
       sp.percent*=2;
     //set the spell using noduplication.
     buf = spell_to_obj(obj, &sp, FALSE);
     //safety
     if (buf == NULL)
       bug("mal_level5: spell_to_obj returned NULL", 0);
     //now that we have the pointer tothe spell we attach messages.
     buf->message = str_dup(SeeMsg);
     buf->message2 = str_dup(BlindMsg);
   }//end set spell

 return TRUE;
}//END LEVEL9


void mal_high_name(OBJ_DATA* obj, int mal_type)
{
  /* converts the name of item to the high type */
  /*
    if WEP_NAMED bit is set we look for " the " and replace all after.
    else
    We send a message to brand the weapon.
  */

  CHAR_DATA* ch;
  AFFECT_DATA *paf;

  char short_buf [MIL];
  char name_buf [MIL];
  char high_name [MIL];
  char* item_name;

  char long_suf[] = " awaits its victim.";

  int name = 0;

  //check if on char.
  if ( (ch = obj->carried_by) == NULL)
    return;
  if ( (paf = affect_find(obj->affected, gen_malform)) == NULL)
    return;

  if (!IS_SET(paf->bitvector, WEP_NAMED))
    {
      send_to_char("`@Your weapon has reached a level of power "\
	   "that deserves a proper name.\n\r Use cmd: "\
	   " \"brandweapon\" to give your item a proper name.\n\r``", ch);
      return;
    }

  //select which string to apply
 //DEATH
 if (IS_SET(paf->bitvector, WEP_EFF_29))
   name = MAL_DEATH;
 //CHAOS
 else if (IS_SET(paf->bitvector, WEP_EFF_28))
   name = MAL_CHAOS;
 //KNOWLEDGE
 else if (IS_SET(paf->bitvector, WEP_EFF_27))
   name = MAL_KNOW;
 //FAITH
 else
   name = MAL_FAITH;

  //now since the item has already been named we reformat the name.
 sprintf(high_name, " %s", mal_name_table[name].higher);

  //now we have the post fix, we need to get the name of the item.
  if ( (item_name = strtok(obj->short_descr, " ")) == NULL)
    {
      sprintf(short_buf, "%s", high_name);
      sprintf(name_buf, "%s", high_name);
    }
  else
    {
      sprintf(short_buf, "%s the%s", item_name, high_name);
      sprintf(name_buf, "%s %s", item_name, high_name);
    }

  //now we begin changing the item once again.

//change short desc. (cap it as well)
  //  short_buf[0] = UPPER(short_buf[0]);
  free_string(obj->short_descr);
  obj->short_descr = str_dup(short_buf);

//change long descr
  strcat(short_buf, long_suf);
  free_string(obj->description);
  obj->description = str_dup(short_buf);

//chang name
  free_string(obj->name);
  obj->name = str_dup(name_buf);

  //End at teh end we do a little dance.
  act("$n raises $p high towards the heavens as aura of power radiates from the blade.", ch, obj, NULL ,TO_ROOM);
  act("Surrounded by aura of power you point $p high towards the heavens.", ch, obj, NULL ,TO_CHAR);

  sprintf(short_buf, "%s, %s will do your bidding in this realm!",
	  ch->pcdata->deity,
	  obj->short_descr);
  do_yell(ch, short_buf);

  sprintf(short_buf,"With %s's guidance, $N has obtained the final malformation of his weapon: %s.",
	  ch->pcdata->deity, obj->short_descr);
  wiznet(short_buf, ch, NULL,WIZ_PENALTIES,WIZ_SECURE, LEVEL_HERO);

  //we log the event.
  sprintf(short_buf,"%s has reached its final level of malformation", obj->short_descr);
  log_event(ch, short_buf);


}

inline bool mal_level10(OBJ_DATA* obj, AFFECT_DATA* paf, bool fClear)
{
  AFFECT_DATA af;
  //  AFFECT_DATA* paf;

    //this is only run ONCE and not removed.
  if (fClear
      ||  affect_find(obj->affected, gen_soul_reaver) != NULL
      ||  affect_find(obj->affected, gen_handof_chaos) != NULL)
    return TRUE;
  //selects between reaver DEATH or chaos CHAOS or randomly

  if (IS_SET(paf->bitvector, WEP_EFF_28))
    af.type = gen_handof_chaos;
  else if (IS_SET(paf->bitvector, WEP_EFF_29))
    af.type = gen_soul_reaver;
  else if (number_percent() < 50)
    af.type = gen_handof_chaos;
  else
    af.type = gen_soul_reaver;

  af.where = TO_NONE;
  af.location = APPLY_NONE;
  af.duration = -1;
  af.level = obj->level;
  af.modifier = 0;
  af.bitvector = 0;
  affect_to_obj(obj, &af);

  //we also reformat the name.
  mal_high_name(obj, af.type);
return TRUE;
}//END LEVEL10




/* mal_set_level */
/* used by maleform weapon */
/* Written by Viri */
/* returns FALSE on error. */
bool mal_set_level(OBJ_DATA* obj, AFFECT_DATA* paf, int level, bool fClear)
{
  /*
level_effect gives or clears the effect for maleform weapon
based on the level passed on.  the fClear flag is used for
decision weather the effect is set or cleared.

I've decided to hardcode the effects rather then use a more
flexible table as it offers lot more flexiiblity withthe effects
with far less work.
As this type of object effect is not meant to be common I think
its sound decision :)
  */

  switch (level)
    {
    case 0: return mal_level0(obj, paf, fClear);
    case 1: return mal_level1(obj, paf, fClear);
    case 2: return mal_level2(obj, paf, fClear);
    case 3: return mal_level3(obj, paf, fClear);
    case 4: return mal_level4(obj, paf, fClear);
    case 5: return mal_level5(obj, paf, fClear);
    case 6: return mal_level6(obj, paf, fClear);
    case 7: return mal_level7(obj, paf, fClear);
    case 8: return mal_level8(obj, paf, fClear);
    case 9: return mal_level9(obj, paf, fClear);
    case 10: return mal_level10(obj, paf, fClear);
    }//end switch.

  bug("mal_set_level: No level matched.", 0);
  return FALSE;
}







/* set_effects */
/* used in maleform weapon */
/* written bu Viri */
bool set_effects(CHAR_DATA* ch, OBJ_DATA* obj, AFFECT_DATA* paf, int path){
  /*
     this is one of the smaller sub functions of maleform weapon.
     It is responsible for initial decision / setup of the selectable
     effecs at lvls: 3, 5, 7, 9.

     Flags for each effect:
     WEP_EFF_NONE	EMPTY
     ----LVL 3-------------
     WEP_EFF_1	+1 Str
     WEP_EFF_2	+1 Dex
     WEP_EFF_3	+5 Hp
     WEP_EFF_4	-1 Svs
     ----LVL 5-------------
     WEP_EFF_5	poison
     WEP_EFF_6  plague
     WEP_EFF_7	faerie fire
     ----LVL 7------------
     WEP_EFF_8  blind
     WEP_EFF_9	weaken
     WEP_EFF_10	curse
     WEP_EFF_11	energy_drain
     ----LVL 9-----------
     WEP_EFF_12	dispel magic
     WEP_EFF_13	dispel good
     WEP_EFF_14	blasphemy
     WEP_EFF_15	vamp touch


*/

//data
  int flag  = WEP_EFF_NONE;
  int level_3 = WEP_EFF_NONE;
  int level_5 = WEP_EFF_NONE;
  int level_7 = WEP_EFF_NONE;
  int level_9 = WEP_EFF_NONE;


  level_3 = number_range(1, 4);
  level_5 = number_range(5, 7);
  /* spells */
  level_7 = number_range(8, 11);
  level_9 = number_range(12, 15);


//set the flags.
switch (level_3)
  {
  case 1: flag |= WEP_EFF_1;break;
  case 2: flag |= WEP_EFF_2;break;
  case 3: flag |= WEP_EFF_3;break;
  case 4: flag |= WEP_EFF_4;break;
  }

switch (level_5)
  {
  case 5: flag |= WEP_EFF_5;break;
  case 6: flag |= WEP_EFF_6;break;
  case 7: flag |= WEP_EFF_7;break;
  }

 switch (level_7){
  case  8: flag |= WEP_EFF_8; break;
  case  9: flag |= WEP_EFF_9; break;
  case 10: flag |= WEP_EFF_10;break;
  case 11: flag |= WEP_EFF_11;break;
  }

switch (level_9)
  {
  case 12: flag |= WEP_EFF_12;break;
  case 13: flag |= WEP_EFF_13;break;
  case 14: flag |= WEP_EFF_14;break;
  case 15: flag |= WEP_EFF_15;break;
  }

 switch (path){
 default:
 case PATH_FAITH:	flag |= WEP_EFF_26;	break;
 case PATH_DEATH:	flag |= WEP_EFF_29;	break;
 case PATH_CHANCE:	flag |= WEP_EFF_28;	break;
 case PATH_KNOW:	flag |= WEP_EFF_27;	break;
 }


//DEBUG:
//sendf(ch, "lvl1: %d, lvl3: %d, lvl5: %d, lvl7: %d Flag: %d\n\r",
//level_1, level_3, level_5, level_7, flag);

//we set the flag.
 paf->bitvector = flag;

 //We give teh weapon lvl 0 effects.
 if (!mal_set_level(obj,paf, 0, FALSE))
   {
     bug("set_effects: malform_level_effect returned error.", 0);
     return FALSE;
   }

  return TRUE;
}//end set_effect


/* malform_name */
/* by Viri */
/* REUSABLE BY ALL LIVING WEAPONS */
void do_brand_weapon( CHAR_DATA *ch, char *argument )
{
  /*
    This is the command used by those who took their
    living weapon to sufficently high rank to name then.

    argument carries the name of object, and branding name.
  */
  OBJ_DATA* obj;
  AFFECT_DATA* paf;

  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];
  char buf [MIL];//short descr
  char name_buf[MSL];//name string
  char long_suf[] = " awaits its master here.";


  int min_level = 0;
  int max_level = 0;
  int wep_type = LIVWEP_NONE;
  int name = 0;
//bool
  bool fConfirmed = FALSE;

  //EZ
  if (ch == NULL)
    return;

  argument = one_argument(argument, arg1);
  if (strcmp(arg1, "confirm"))
    strcpy(arg2, argument);
  else
    {
      argument = one_argument(argument, arg2);
      strcpy(arg3, argument);
      fConfirmed = TRUE;
      strcpy(arg1, arg2);
      strcpy(arg2, arg3);
    }

  if (arg1[0] == '\0' || arg2[0] == '\0')
    {
      send_to_char("Brand Weapon:\n\rSyntax: Brand Weapon <weapon> <name>"\
		   " \n\r Name must be a single word.  Please keep in mind "\
		   "that each name is\n\rsent for approval to Immortals "\
		   "with your name attached.\n\r", ch);

	return;
    }
  if ( (obj = get_obj_carry(ch, arg1, ch)) == NULL)
    if ( (obj = get_obj_wear(ch, arg1, ch)) == NULL)
      {
	send_to_char("The item must be worn or carried.\n\r", ch);
	return;
      }


  //check name given.
  arg2[0] = UPPER(arg2[0]);
  if (strchr(arg2, ' ') != NULL)
    {
      send_to_char("Enter ONE word for the name of item."\
		   " ex: Excalibur.\n\r", ch);
      sendf(ch, "If you feel that \"%s\" is a single word please contact"\
	    " an Immortal.\n\r", arg2);
      return;
    }

  //now we get the living weapon effect.
  if ( (paf = affect_find(obj->affected, gen_malform)) != NULL)
    {
      min_level = 6;
      max_level = effect_table[GN_POS(paf->type)].value2[0];
      wep_type = LIVWEP_MAL;
    }
  else if ( (paf = affect_find(obj->affected, gen_hwep)) != NULL)
    {
      min_level = 6;
      max_level = effect_table[GN_POS(paf->type)].value2[0];
      wep_type = LIVWEP_HOL;
    }
  else
    {
      send_to_char("This is not a Living Weapon and hence cannot"\
		   " be branded.\n\r", ch);
      return;
    }
  //now we know what type of Living weapon this is and we have a paf.

  //check level
  if (paf->level < min_level)
    {
      send_to_char("This item is not powerful enough to be given a name.\n\r", ch);
      return;
    }

  //check if the weapon has been branded already.
  if (IS_SET(paf->bitvector, WEP_NAMED))
    {
      send_to_char("This item has already been branded.\n\r", ch);
      return;
    }

//now we start naming the weapon
  switch (wep_type)
    {
    case LIVWEP_MAL:
      //select which string to apply for malforms
      //DEATH
      if (IS_SET(paf->bitvector, WEP_EFF_29))
	name = MAL_DEATH;
      //CHAOS
      else if (IS_SET(paf->bitvector, WEP_EFF_28))
	name = MAL_CHAOS;
      //KNOWLEDGE
      else if (IS_SET(paf->bitvector, WEP_EFF_27))
	name = MAL_KNOW;
      //FAITH
      else
	name = MAL_FAITH;

      if (paf->level >= max_level){
	    sprintf(buf, "'%s' the %s", arg2, mal_name_table[name].higher);
	    sprintf(name_buf, "%s %s ", arg2, mal_name_table[name].higher);
      }
      else
	{
	  sprintf(buf, "'%s' the %s", arg2, mal_name_table[name].lower);
	  sprintf(name_buf, "%s %s ", arg2, mal_name_table[name].lower);
	}
      break;
    case LIVWEP_HOL:
      sprintf(buf, "'%s' the %s", arg2, hol_name_table[obj->value[0]].lower);
      sprintf(name_buf, "%s %s ", arg2, hol_name_table[obj->value[0]].lower);
      break;
    }//end name selection
  /* Just so morts can see what name will look like we give them a second chance */
  if (!fConfirmed)
    {
      sendf(ch, "Given the above the weapon name will be:     %s\n\r"\
	    "And you will see it in the room as:     %s%s\n\r\n\r"\
	    "If this is to your liking typing \"`@brandweapon confirm <weapon> <name>``\"\n\r"\
	    "will brand the weapon with the <name>.\n\r", buf, buf, long_suf);
      return;
    }


  SET_BIT(paf->bitvector, WEP_NAMED);
//change short desc.
  free_string(obj->short_descr);
  obj->short_descr = str_dup(buf);

//change long descr
  strcat(buf, long_suf);
  free_string(obj->description);
  obj->description = str_dup(buf);

//chang name
  strcat(name_buf, obj->name);
  free_string(obj->name);
  obj->name = str_dup(name_buf);

  //End at teh end we do a little dance.
  act("$n raises $p high towards the heavens.", ch, obj, NULL ,TO_ROOM);
  act("You point $p high towards the heavens.", ch, obj, NULL ,TO_CHAR);
  sprintf(buf, "%s, with your blessing I call this weapon %s!",
	  ch->pcdata->deity,
	  obj->short_descr);
  do_yell(ch, buf);

  sprintf(buf,"Calling on his deity %s, $N has branded his weapon: %s.",
	  ch->pcdata->deity, arg2);
  wiznet(buf,ch, NULL,WIZ_PENALTIES,WIZ_SECURE, LEVEL_HERO);

  //we log the event.
  sprintf(buf,"branded his weapon: %s.", arg2);
  log_event(ch, buf);

  return;
}


//--------=========GEN FUNCTIONS============----------//
/* malform_none */
/* linked into effect.c */
/* written by Viri */
int malform_none(CHAR_DATA* not_used, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*
This is a linker function so the LivWep functions have a degree of flexibility and reusability.
The NONE update simply does the increase/decrease of the malform level based on the flag.
TRUE= INCREEASE.
  */

  OBJ_DATA* obj = (OBJ_DATA*) arg1;//object with malform
  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;//paf with malform.

  CHAR_DATA* ch = obj->carried_by;
//bool
  bool fSuccess = TRUE;


  //we simply clear/set effects now.
  if (flag) //INCREASE
    {
      paf->level++;
      obj->level = UMIN(55, obj->level + 3);
      fSuccess =  mal_set_level(obj, paf,  paf->level, FALSE);
      if (ch != NULL)
	{
	  act("Power and pure evil surges through your body as $p grows with unholy power!.", ch, obj, NULL, TO_CHAR);
	  act("You feel a sinister aura suffocate the area as $p seems to pulsate with evil in $n's hands.", ch, obj, NULL, TO_ROOM);
	}
    }
  else
    {
      fSuccess =  mal_set_level(obj, paf,  paf->level, TRUE);
      paf->level--;
      obj->level = UMAX(1, obj->level - 3);
      if (ch != NULL)
       	act("A slow throbbing sound sends shivers down your spine as $p writhes and smokes.", ch, obj, NULL, TO_ALL);
      else if (obj->in_room){
	for (ch = obj->in_room->people; ch != NULL; ch = ch->next_in_room)
	  act("A slow throbbing sound sends shivers down your spine as $p writhes and smokes.", ch, obj, NULL, TO_CHAR);
      }
      else if (obj->in_obj && obj->in_obj->in_room){
	for (ch = obj->in_obj->in_room->people; ch != NULL; ch = ch->next_in_room)
	  act("A slow throbbing sound sends shivers down your spine.", ch, obj, NULL, TO_CHAR);
      }
    }
  return fSuccess;
}//end NONE



/* malform_init */
/* linked into effect.c */
/* written by Viri */
int malform_init(CHAR_DATA* not_used, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* paf passed by arg1, obj passed by arg2, ch is NULL */

  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  OBJ_DATA* obj = (OBJ_DATA*) arg2;
  CHAR_DATA* ch;//character that is carring the obj.

  OBJ_SPELL_DATA* spell;//spells to be nuked.

  char buf[MSL];


  //get the char.
  if ( (ch = obj->carried_by) == NULL)
    return -1;

  //Zero all the flags and set to malform lvl 0.
  obj->extra_flags = ITEM_HUM | ITEM_EVIL | ITEM_ANTI_GOOD | ITEM_ANTI_NEUTRAL;
  paf-> level = 0;
  paf->modifier = 0;
  paf->bitvector = 0;

//make sure the weight is highenough so it can be removed by dropping str.
  obj->weight = UMAX(40, obj->weight );

//set the name of character on the paf.
  string_to_affect(paf, ch->name);

//we store the selected effects of the weapon on the gen.
  if (!set_effects(ch, obj, paf, deity_table[ch->pcdata->way].path ))
    bug("Malform_init: Error setting effects.\n\r", 0);


//clear all the spells on the obj. (affects are already done before init.)
  //Indexed
  obj->eldritched = TRUE;

  //Dynamic
  for (spell = obj->spell; spell != NULL; spell = spell->next)
    free_obj_spell(spell);
  obj->spell = NULL;


//Display the message.
  sprintf(buf, "A faint black figure appears in the moonlight as the very"\
	  " essence of evil\n\rseems to swirl and condensate around $p.  "\
	  "Then suddenly\n\rall is back to normal again, except for the dark "\
	  "aura of evil around the weapon.");
  act(buf, ch, obj, NULL, TO_ALL);
  return TRUE;
}

bool malform_abuse_check( CHAR_DATA* ch, CHAR_DATA* victim){
  AFFECT_DATA* safety, af;
  if ( (safety = affect_find(ch->affected, gsn_harm)) != NULL
       && safety->has_string){
    if (!str_cmp(safety->string, victim->name)){
      return TRUE;
    }
    else{
      string_to_affect(safety, victim->name);
    }
  }
  else{
    AFFECT_DATA* baf;
    af.type = gsn_harm;
    af.duration = -1;
    af.level = ch->level;
    af.where = TO_NONE;
    af.bitvector = 0;
    af.modifier = 0;
    af.location = APPLY_NONE;
    baf = affect_to_char(ch, &af);
    string_to_affect(baf, victim->name);
  }
  return FALSE;
}



/* malform_postkill */
/* linked into effect.c */
/* written by Viri */
int malform_postkill(CHAR_DATA* cha, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*
This is the exp gain portion of malform weapon.
The update cheks if exp should be granted then increaes is based on
victim level.
Finaly LivWep_levl_check is called to check for increase / decrease.
  */

  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  CHAR_DATA* ch = cha;
  AFFECT_DATA* paf_p = NULL, *paf_s = NULL;
  OBJ_DATA* prim, *sec;

//data
int gain = 1;

//const
  const int lvl38_gain = 2;
  const int lvl45_gain = 4;
  const int lvl50_gain = 6;

//bool
  bool fPrim = FALSE;
  bool fSec = FALSE;
  bool fSuccess = TRUE;
  if (IS_NPC(victim))
    return FALSE;

  /* charmie switch */
  if (IS_NPC(cha) && IS_AFFECTED(cha, AFF_CHARM)
      && cha->leader && !IS_NPC(cha->leader)
      && cha->in_room == cha->leader->in_room)
    ch = cha->leader;


//first we get weapons.
  if ( (prim = get_eq_char(ch,WEAR_WIELD)) != NULL)
    fPrim = TRUE;
  if ( (sec = get_eq_char(ch,WEAR_SECONDARY)) != NULL)
    fSec = TRUE;

  //DEBUG
  //  sendf(ch, "Prim: %d, Sec: %d.\n\r", fPrim, fSec);
  //check if we dont do anything

  if (!(fPrim || fSec || cha == victim))
    return FALSE;

  if (victim->level > 49)
    gain = lvl50_gain;
  else if (victim->level > 44)
    gain = lvl45_gain;
  else if (victim->level > 37)
    gain = lvl38_gain;

  if (fPrim)
    paf_p = affect_find(prim->affected, gen_malform);
  if (fSec)
    paf_s = affect_find(sec->affected, gen_malform);

/* abuse check */
  if ( (fPrim && paf_p)
       || (fSec && paf_s)){
    if ( malform_abuse_check( ch, victim)){
      act("Your weapon ignores $N due to $S previous demise.", ch, prim, victim, TO_CHAR);
      return FALSE;
    }
  }


  if (paf_p && paf_s)
    gain = UMAX(1, gain / 2);

  //now check for malforms.
  if (fPrim)
    {
      if (paf_p){
	  paf_p->modifier += gain;
	  act("As last signs of life fade from $N's body you bury $p deep in the corpse and watch it feed.",
	      ch, prim, victim, TO_CHAR);
	  act("As last signs of life fade from $N's body $n sinks $p into the corpse.",
	      ch, prim, victim, TO_NOTVICT);
	  act("You feel $p feeding on your very soul!", victim, prim, NULL, TO_CHAR);
	  fSuccess = LivWep_level_check(prim, gen_malform);
	}
    }
  if (fSec)
    {
      if ( paf_s){
	  paf_s->modifier += gain;
	  act("As last signs of life fade from $N's body you bury $p deep in the corpse and watch it feed.",
	      ch, sec, victim, TO_CHAR);
	  act("As last signs of life fade from $N's body $n sinks $p into the corpse.",
		ch, sec, victim, TO_NOTVICT);
	  act("You feel $p feeding on your very soul!", victim, sec, NULL, TO_CHAR);
	  fSuccess = LivWep_level_check(sec, gen_malform);
      }
    }
  return fSuccess;
}

/* malform_predeat */
/* linked into effect.c */
/* written by Viri */
int malform_predeath(CHAR_DATA* cha, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* MALFORM loses 10% of its experience */

  CHAR_DATA* ch = cha;

  AFFECT_DATA* paf;
  OBJ_DATA* obj, *obj_next;

//bool
  if (IS_NPC(ch))
    return FALSE;

  /* run through all uncontained objects and lower exp on malforms that
     belong to us
  */
  for (obj = ch->carrying; obj; obj = obj_next ){
    obj_next = obj->next_content;

    if (obj->item_type != ITEM_WEAPON)
      continue;
    else if ( (paf = affect_find(obj->affected, gen_malform)) == NULL)
      continue;
    else if (IS_NULLSTR(paf->string) || strcmp(paf->string, ch->name))
      continue;

    paf->modifier = 9 * paf->modifier / 10;
    act("$p emits a shrill whine as $n dies!", ch, obj, NULL, TO_ALL);
    LivWep_level_check(obj, gen_malform);
  }
  return TRUE;
}



/* malform_objtick */
/* linked into effect.c */
/* written by Viri */
int malform_objtick(CHAR_DATA* not_used, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*
This is the item tick update for the malformed weapon.
It is only called when item has gen_malform on it.
If the item is on owner (string) tehn nothing happens.
Otherwise there is "chance" percent that the exp will decrease.

arg1 is object, arg2 is paf.
  */

  OBJ_DATA* obj = (OBJ_DATA*) arg2;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  CHAR_DATA* ch;

//data
  const int chance = effect_table[GN_POS(gn)].value3[0]; //chance for decreas if not on char.

//bool
  bool fCarried = TRUE;
  if (!obj->carried_by && !obj->in_room && !obj->in_obj){
    nlogf("BUG***: malform_objtick: %s not carried/contained and not in room. (timer set for decomposition)", (obj->short_descr? obj->short_descr : "NULL"));
    obj->timer = 1;
    return FALSE;
  }

  if ( (ch = obj->carried_by) == NULL)
    fCarried = FALSE;

  //if it is on someone check who.
  if (fCarried)
    if (strcasecmp(ch->name, paf->string))
      fCarried = FALSE;

  /*
The reason we dont return here is that i want the tick update
to also check lvl of the weapon, just in case we compeup with something
new later that will lower the exp of the weapon arbitrarly.
  */
  //items with malform = or > lvl 8 are not weakened.

/* malforms of level more then 3 will return to their owner if the person is in the same room as the owner */
  if (!fCarried && obj->carried_by && paf->level >= 2){
    CHAR_DATA* victim = obj->carried_by;
    CHAR_DATA* ch = get_char( paf->string );

    if (ch && ch->in_room && victim && victim->in_room
	&& ch->in_room == victim->in_room){
      act("Emiting a shrill moan $p floats away from $n into $N's hand.", victim, obj, ch, TO_NOTVICT);
      act("Emiting a shrill moan $p floats away from you into $N's hand.", victim, obj, ch, TO_CHAR);
      act("Emiting a shrill moan $p floats away from $n into your hand.", victim, obj, ch, TO_VICT);

      obj_from_char( obj );
      obj_to_char( obj, ch );
      fCarried = TRUE;
    }
  }
  if(!fCarried && paf->level < 8)
    if (number_percent() < chance)
      {
	if ( (ch = obj->carried_by) != NULL)
	  act("$p shivers slightly.", ch, obj, NULL, TO_CHAR);
	else{
	  if (obj->in_room){
	    if ( (ch = obj->in_room->people) != NULL)
	      act("$p shivers slightly.", ch, obj, NULL, TO_ALL);
	  }
	  paf->modifier--;
	}
      }
    //safety check on lvl.
  return  LivWep_level_check(obj, gn);
}




//-------=======HAS OWNER =======----------//
/* has_owner_init */
/* written by viri */
int has_owner_init(CHAR_DATA* not_used, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*
    This is the function responsible for setting the ITEM_HAS_OWNER flag
    and recording the nameof the onwer. object must be on character for this,
    otherwise the name of owner must be set somehow else.
  */

  OBJ_DATA* obj = (OBJ_DATA*) arg2;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  CHAR_DATA* ch = NULL;

  //bools
  bool is_worn = FALSE;

  //Safety
  if (obj == NULL || paf==NULL)
    {
      bug("has_owner_init: no obj or no effect passed.", 0);
      return -1;
    }

  //check if is on character.
  if ( (ch = obj->carried_by) != NULL)
    is_worn = TRUE;

  //set the flag on item.
  SET_BIT(obj->wear_flags, ITEM_HAS_OWNER);

  //now we set the owner if possible.
  if (is_worn)
    string_to_affect(paf, ch->name);
  return TRUE;
}

/* has_owner_none */
/* by viri */
int has_owner_none(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*
This is check/message function run when:
- after obj_to_char(..) is run.
- on Obj tick.

It checks if the obj is on character, if it is
it chekcs for owner, if charachter is not woner a message is displayerd
and object is moved to room.

  */
  OBJ_DATA* obj = (OBJ_DATA*) arg2;
  AFFECT_DATA* paf;



  if (!IS_SET(obj->wear_flags, ITEM_HAS_OWNER) || ch == NULL)
    return FALSE;
  else if ( obj->carried_by == NULL)
    return FALSE;
  else if (is_owner(obj, ch))
    return FALSE;
  else if (IS_IMMORTAL(ch))
    return TRUE;

  //get which deity if any protects it.
  paf = affect_find(obj->affected, gen_has_owner);

  //now we display messages.
  /* there are two types of messages, deity and none. if modifier is not -1 its deity.*/
  if (paf->modifier < 0)
    {
      act_new(effect_table[GN_POS(gn)].msg_off,ch,obj,NULL,TO_CHAR,POS_DEAD);
      act(effect_table[GN_POS(gn)].msg_off2,ch,obj,NULL,TO_ROOM);
    }
  else
    {
      char buf[MIL];
      sprintf(buf, "Feeling %s's eyes and anger upon your insignificant self you quickly drop $p.",
	      deity_table[paf->modifier].god);
      act(buf, ch, obj, NULL, TO_CHAR);
      sprintf(buf, "Aura of %s's power fills the area $n quickly drops $p before its too late.",
	    deity_table[paf->modifier].god);
      act(buf, ch, obj, NULL, TO_ROOM);
    }

  act("$n drops $p.", ch, obj, NULL, TO_ROOM);
  act("You drop $p.", ch, obj, NULL, TO_CHAR);

  //move obj to floor.
  obj_from_char(obj);
  obj_to_room(obj, ch->in_room);
  return TRUE;
}//end NONE


/* has_owner_kill */
/* by Viri */
int has_owner_kill(CHAR_DATA* not_used, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* this is run just to strip the flag fromthe item. */
  OBJ_DATA* obj = (OBJ_DATA*) arg2;
  if (gn != gen_has_owner)
    return FALSE;
  if (obj == NULL)
    return FALSE;
  REMOVE_BIT(obj->wear_flags, ITEM_HAS_OWNER);
  return TRUE;
}


//--------=========DARK METAMORPHOSIS==========--------//
/* dark_met INIT */
/* link in effect.c only */
/* written by viri */
int dark_met_init(CHAR_DATA* not_used, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*
 This is passed during affect_to_obj
- arg2 is obj, arg1 is affect.
  */


  OBJ_DATA* obj = (OBJ_DATA*) arg2;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  CHAR_DATA* ch = NULL;



  //Safety
  if (obj == NULL || paf==NULL)
    {
      bug("dark_met_init: no obj or no effect passed.", 0);
      return -1;
    }

  //check if is on character.
  if ( (ch = obj->carried_by) == NULL)
    {
      bug("dark_met_init: Object not on character..", 0);
      return FALSE;
    }

  //set the flags.
  if (IS_OBJ_STAT(obj, ITEM_EVIL))
    SET_BIT(paf->bitvector, ITEM_EVIL);
  else
    SET_BIT(obj->extra_flags, ITEM_EVIL);

  if (IS_OBJ_STAT(obj, ITEM_NOREMOVE))
    SET_BIT(paf->bitvector, ITEM_NOREMOVE);
  else
    SET_BIT(obj->extra_flags, ITEM_NOREMOVE);

  if (IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL))
    SET_BIT(paf->bitvector, ITEM_ANTI_NEUTRAL);
  else
    SET_BIT(obj->extra_flags, ITEM_ANTI_NEUTRAL);

  if (IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))
    SET_BIT(paf->bitvector, ITEM_ANTI_GOOD);
  else
    SET_BIT(obj->extra_flags, ITEM_ANTI_GOOD);

  return TRUE;
}//end dark_met_init


/* dark_met_kill */
/* by Viri */
int dark_met_kill(CHAR_DATA* not_used, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* this is run just to strip the owner from item */
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  OBJ_DATA* obj = (OBJ_DATA*) arg2;
  CHAR_DATA* ch;


  if (gn != gen_dark_meta)
    return FALSE;
  if (!IS_SET(paf->bitvector, ITEM_EVIL))
    REMOVE_BIT(obj->extra_flags, ITEM_EVIL);
  if (!IS_SET(paf->bitvector, ITEM_NOREMOVE))
    REMOVE_BIT(obj->extra_flags, ITEM_NOREMOVE);
  if (!IS_SET(paf->bitvector, ITEM_ANTI_NEUTRAL))
    REMOVE_BIT(obj->extra_flags, ITEM_ANTI_NEUTRAL);
  if (!IS_SET(paf->bitvector, ITEM_ANTI_GOOD))
    REMOVE_BIT(obj->extra_flags, ITEM_ANTI_GOOD);

  //run messages
  if ( (ch = obj->carried_by) != NULL)
    {
      act(effect_table[GN_POS(gn)].msg_off,ch,obj,NULL,TO_CHAR);
      act(effect_table[GN_POS(gn)].msg_off2,ch,obj,NULL,TO_ROOM);
    }
  else if (obj->in_room &&  (ch = obj->in_room->people) != NULL)
    {
      act(effect_table[GN_POS(gn)].msg_off2,ch,obj,NULL,TO_ROOM);
      act(effect_table[GN_POS(gn)].msg_off2,ch,obj,NULL,TO_CHAR);
    }

  return TRUE;
}

/* dark_met_postdamage */
int dark_met_postdamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* run after the weapon causes damage and has gen_dark_met */
  //Attacker passed by ch
  //victim passed by arg1
  //obj passed by arg2
  //damage done by int1;
  //dam type by int2;
  //dt by flag


  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  OBJ_DATA *obj = ( OBJ_DATA*) arg2;
  bool fElder = get_skill(ch, gsn_unholy_gift) > 1;

//data
  int roll = number_percent();
  int dam = (int) *int1;
  int dam_type = (int) *int2;
  //  int dt = flag;
  int chance = 0;

  //  sendf(ch, "`!ch: %s, vch: %s, dam %d, dam_type %d.``\n\r", ch->name, victim->name, dam, dam_type);
  //if not undead
  if (IS_SET(victim->act,ACT_UNDEAD))
    return FALSE;

  //check if wounds are made.
  if (!fElder && !(dam_type <= 3) && !(dam_type >= 20 && dam_type <= 23))
    {
      //the weapon doesnt allow for blood to be spilled.
      send_to_char("Your weapon seems to cauterize the wounds.\n\r", ch);
      return FALSE;
    }

  //Make the test for PC
  if (!IS_NPC(victim)){
    chance = (23 + dam/4) + ch->level - victim->level;
  }
  else
    //NON-PC
    chance =  12 + dam/5;
  if (fElder)
    chance = 2 * chance;
  //roll if we feed.
  if (roll < chance)
    {
      if (is_affected(victim, gsn_ptrium)){
	act("$p smokes and recoils from the wound!",victim,obj,NULL,TO_ALL);
	return FALSE;
      }
      act("$p's surface ripples as it makes the wound.",victim,obj,NULL,TO_ALL);
      damage(ch, victim, (ch->level / 5), gn, DAM_NEGATIVE, TRUE);
      if (fElder)
	ch->hit = UMIN(ch->hit + dam/2, ch->max_hit);
      else
	ch->hit = UMIN(ch->hit + number_range(dam/4,dam/2),ch->max_hit);
      gain_condition( ch, COND_HUNGER, + 1 );
      //we roll for drain.
      if (number_percent() < effect_table[GN_POS(gn)].value2[0]
	  && !is_affected(victim, gsn_drained))
	{
	  AFFECT_DATA af;
	  af.type = gsn_drained;
	  af.where = TO_AFFECTS;
	  af.level = ch->level;
	  af.location = APPLY_STR;
	  af.duration = ch->level / 12;
	  af.modifier = -1;
	  af.bitvector = 0;
	  affect_to_char(victim, &af);
	  act("You feel your vitality drain as $p cuts your flesh.", victim, obj,NULL, TO_CHAR);
	  act("$n stumbles under his weight.", victim , NULL, NULL, TO_ROOM);
	}//end drain.
      return TRUE;
    }//END Feed

  return FALSE;
}



//--------=========Soul Reaver==========--------//

/* link in effect.c only */
/* written by viri */
int soul_reaver_init(CHAR_DATA* not_used, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*
 This is passed during affect_to_obj
- arg2 is obj, arg1 is affect.
  */


  OBJ_DATA* obj = (OBJ_DATA*) arg2;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  CHAR_DATA* ch = NULL;

  //Safety
  if (obj == NULL || paf==NULL)
    {
      bug("handof_chaos_init: no obj or no effect passed.", 0);
      return -1;
    }

  //check if is on character.
  if ( (ch = obj->carried_by) != NULL)
    {
      char buf[MIL];
      sprintf(buf, "All vitality leaves your body as spirit of $g descends into $p.");
      act(buf, ch, obj, NULL, TO_ALL);
      return TRUE;
    }
  return FALSE;
}



int soul_reaver_postdamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* run after the weapon causes damage and has gen_soul_reaver */
  //Attacker passed by ch
  //victim passed by arg1
  //obj passed by arg2
  //damage done by int1;
  //dam type by int2;
  //dt by flag

  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  OBJ_DATA *obj = ( OBJ_DATA*) arg2;


//data
  int roll = number_percent();
  int dam = (int) *int1;
  int chance = effect_table[GN_POS(gn)].value2[0];

  int hp_dr = dam / 10;
  int exp_dr = dam / 10;
  int mana_dr = dam / 10;
  int move_dr = dam / 20;

  if (IS_SET(victim->act, ACT_UNDEAD))
    return FALSE;

  //or npc
  if (IS_NPC(victim))
    return FALSE;

  //roll if we drain
  if (roll < chance)
    {
      AFFECT_DATA af, *paf;
      const int sn = skill_lookup("vampiric touch");

      act(effect_table[GN_POS(gn)].msg_off,ch,obj,victim,TO_CHAR);
      act(effect_table[GN_POS(gn)].msg_off2,ch,obj,victim,TO_ROOM);


      victim->mana = UMAX(0, victim->mana - mana_dr);
      victim->move = UMAX(0, victim->move - move_dr);
      sendf(victim, "The vile weapon feeds on your very soul!\n\rYou lose %d experience!\n\r", exp_dr);
      gain_exp(victim, -exp_dr);

      ch->mana = UMIN(ch->max_mana, ch->mana + mana_dr);
      ch->move = UMIN(ch->max_move, ch->move + move_dr);
      gain_exp(ch, exp_dr);

      /* drain hp */
      if ( (paf = affect_find(victim->affected, sn)) == NULL
	   || paf->modifier > -50){
	af.type = sn;
	af.where = TO_AFFECTS;
	af.bitvector = 0;
	af.level = ch->level;
	af.duration = is_affected(victim, sn) ? 2 : 24;
	af.modifier = -hp_dr;
	af.location = APPLY_HIT;
	affect_join(victim, &af);
      }
    }//end corrupt.


  return FALSE;
}


int soul_reaver_parry(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* run after the weapon is parried */
  //Attacker passed by ch
  //objm passed by arg1
  //victim obj passed by arg2
  //dam type by int1;


  OBJ_DATA *obj = ( OBJ_DATA*) arg1;
  OBJ_DATA* vobj = (OBJ_DATA*) arg2;

  AFFECT_DATA* paf;


//data
  int roll = number_percent();
  //  int dam_type = (int) *int1;
  int chance = effect_table[GN_POS(gn)].value3[0];

  if (obj == vobj)
    return FALSE;

  //check if target has malfomr on it.
  if ( (paf = affect_find(vobj->affected, gen_malform)) == NULL)
    return FALSE;

  //roll for drain.
  if (roll < chance)
    {
      char buf[MIL];
      sprintf(buf, "%s shivers slightly as it touches $n's surface.",vobj->short_descr);
      act(buf, ch, obj, NULL, TO_ALL);
      paf->modifier = UMAX(0, paf->modifier - 1);
    }
  return FALSE;
}//end PARRY







//--------=========HAND OF CHAOS==========--------//
/* hand of chaos INIT */
/* link in effect.c only */
/* written by viri */
int handof_chaos_init(CHAR_DATA* not_used, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*
 This is passed during affect_to_obj
- arg2 is obj, arg1 is affect.
  */


  OBJ_DATA* obj = (OBJ_DATA*) arg2;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  CHAR_DATA* ch = NULL;

  //Safety
  if (obj == NULL || paf==NULL)
    {
      bug("handof_chaos_init: no obj or no effect passed.", 0);
      return -1;
    }

  //check if is on character.
  if ( (ch = obj->carried_by) != NULL)
    {
      char buf[MIL];
      sprintf(buf, "Order and reality seems to bend around $p as spirit of $g descends into it.");
      act(buf, ch, obj, NULL, TO_ALL);
      return TRUE;
    }
  return FALSE;
}//END HANDOF_CHAOS_INIT



/* handof_chaospostdamage */
int handof_chaos_postdamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* run after the weapon causes damage and has gen_soul_reaver */
  //Attacker passed by ch
  //victim passed by arg1
  //obj passed by arg2
  //damage done by int1;
  //dam type by int2;
  //dt by flag

  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  OBJ_DATA *obj = ( OBJ_DATA*) arg2;

//data
  int roll = number_percent();
  //  int dam = (int) *int1;
  int chance_corrupt = effect_table[GN_POS(gn)].value2[0];

  if (victim->race == race_lookup("demon"))
    return FALSE;
  else if (IS_NPC(victim))
    return FALSE;

  //roll if we corrupt
  if (roll < chance_corrupt && !is_affected(victim, gsn_corrupt) )
    {
      AFFECT_DATA af;
      act(effect_table[GN_POS(gn)].msg_off,ch,obj,victim,TO_CHAR);
      act(effect_table[GN_POS(gn)].msg_off2,ch,obj,victim,TO_ROOM);

      af.type = gsn_corrupt;
      af.where = TO_AFFECTS;
      af.level = ch->level;
      af.location = APPLY_MANA;
      af.modifier = -(2 * ch->level);
      af.duration = 1;
      af.bitvector = 0;
      affect_to_char(victim, &af);
    }//end corrupt.


  return FALSE;
}

int handof_chaos_parry(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* run after the weapon is parried */
  //Attacker passed by ch
  //objm passed by arg1
  //victim obj passed by arg2
  //dam type by int1;


  OBJ_DATA *obj = ( OBJ_DATA*) arg1;
  OBJ_DATA* vobj = (OBJ_DATA*) arg2;

//data
  int roll = number_percent();
  //  int dam_type = (int) *int1;
  int chance = effect_table[GN_POS(gn)].value3[0];

  if (roll < chance){
    char buf[MIL];
    sprintf(buf, "As %s makes contact with $p Chaos takes over.", obj->short_descr);
    act(buf, ch, vobj, NULL, TO_ALL);
    if (IS_OBJ_STAT(vobj, ITEM_BURN_PROOF))
      act("$p resits Chaos!", ch, vobj, NULL, TO_ALL);
    else
      act("$p dissolves into a smoking pulp.", ch, vobj, NULL, TO_ALL);
    extract_obj( vobj );
  }//end destruction
  return TRUE;
}//end parry.


//------========REQUEST=========--------//

/* request INIT */
/* called in effect.c only */
/* Created by Viri */

int request_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  if (gn != gen_request)
    return -1;
  //we simply display few messages here, everything else has been already set on effects by do_request.
  if (!IS_NPC(ch))
    ch->pcdata->pStallion = NULL;

  do_rest(ch, "");
  act("You enter a deep prayer knowing that $g's eyes are on your soul.",ch, NULL, NULL, TO_CHAR);
  act("$n begins to meditate in deep prayer.", ch, NULL, NULL, TO_ROOM);
  return TRUE;
}

/* Created by Viri */
int request_end(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  OBJ_DATA* obj, *obj_next;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  AFFECT_DATA* buf;

  if (gn != gen_request)
    return -1;

  if (paf->duration != 0 )
    return FALSE;

  //called by kill routined if killed by update.c ie: duration = 0;
  act("A feeling of confidence enters your soul and you know that $g has judged you worthy.", ch, NULL, NULL, TO_CHAR);

  //now we get the item.
  for (obj = ch->in_room->contents; obj; obj = obj_next){
    obj_next = obj->next_content;

    if (is_name(paf->string, obj->name) && is_owner(obj, ch))
      break;
  }
  if (obj == NULL)
    {
      send_to_char("Unfortunatley your prize seems to have disappeared.\n\r", ch);
      return FALSE;
    }

  //reset the flags.
  if ( (buf = affect_find(obj->affected, gen_has_owner)) != NULL)
    {
      if (IS_SET(paf->bitvector, ITEM_HAD_TIMER))
	obj->timer = paf->level;
      else
	obj->timer = -1;

      if (!IS_SET(paf->bitvector, ITEM_NO_SAC))
	REMOVE_BIT(obj->wear_flags, ITEM_NO_SAC);
    }

  //we get it.
  act("You reach for $p.", ch, obj, NULL, TO_CHAR);
  get_obj(ch, obj, NULL);
  clear_owner(obj);
  do_stand(ch, "");
  //no we set the calm if the onbj was a rare or unique.
  if (CAN_WEAR(obj, ITEM_RARE) || CAN_WEAR(obj, ITEM_UNIQUE))
    {
      //calculate current penalty and duration.
      AFFECT_DATA af;
      int calm_pen = obj->level / 3;
      bool calm_found = FALSE;
      af.type = skill_lookup("calm");

      //if already affected by calm we simpy change the penalties to teh greater oen.

      for ( buf = ch->affected; buf != NULL; buf = buf->next )
	{
	  if (buf->type == af.type)
	    {
	      calm_found = TRUE;
	      buf->duration = UMAX(buf->duration, calm_pen);
	      buf->modifier = UMIN(buf->modifier, -calm_pen);
	    }
	}//end change existing calm.

      if (!calm_found)
	{
	  af.where = TO_AFFECTS;
	  af.level = obj->level;
	  af.duration = calm_pen;
	  af.location = APPLY_HITROLL;
	  af.modifier = -calm_pen;
	  af.bitvector = AFF_CALM;
	  affect_to_char(ch,&af);
	  af.location = APPLY_DAMROLL;
	  affect_to_char(ch,&af);
	}//end set calm.
    }//end if item was rare.
  return TRUE;
}




/* Created by Viri */
int request_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  int  hit_loss = effect_table[GN_POS(gn)].value1[0];
  int mana_loss = effect_table[GN_POS(gn)].value2[0];
  int move_loss = effect_table[GN_POS(gn)].value3[0];

  int hit = ch->hit;
  int man = ch->mana;
  int mov = ch->move;

  if (gn != gen_request
      && gn != gen_chant
      && gn != gen_conjure
      && gn != gen_wcraft
      && gn != gen_acraft)
    return -1;

  if (paf->duration == 0 )
    return FALSE;
  /* abuse check */
  if (!ch->desc)
    affect_strip(ch, gn);

  /* adjust the stats if needed */
  if (gn == gen_chant
      || gn == gen_conjure
      || gn == gen_wcraft
      || gn == gen_acraft){
    hit = UMAX(0, hit - ch->max_hit / 2);
    man = UMAX(0, man - ch->max_mana / 2);
    mov = UMAX(0, mov - ch->max_move / 2);
  }

  //we make character lose his stats.
  if (ch->hit > 0)
    ch->hit  -= hit_loss * hit / 100;
  if (ch->mana > 0)
    ch->mana -= mana_loss * man / 100;
  if (ch->move > 0)
    ch->move -= move_loss * mov / 100;
  update_pos( ch );

  if (gn == gen_chant){
    if (!psalm_table[paf->modifier].fGroup){
      CHAR_DATA* vch = get_group_world(ch, TRUE);
      if (vch){
	act("You begin but soon thoughts of $N's safety break your concentration.", ch, NULL, vch, TO_CHAR);
	paf->duration  = 1;
	affect_strip(ch, gn);
      }
    }
  }
/* show some messages */
  if (is_affected(ch, gen_chant)){
    if (number_percent() < 40)
      do_mpasound(ch, "You hear sounds of chanting near by.");
    check_improve(ch, gsn_chant, TRUE, 1);
    check_improve(ch, gsn_meditation, TRUE, 1);
  }
  else if (is_affected(ch, gen_conjure )){
    if( number_percent() < 40)
      do_mpasound(ch, "Ominous sounds of chanting and unearthly wails reach your ears.");
    check_improve(ch, skill_lookup("conjuration"), TRUE, 1);
  }
  else if (is_affected(ch, gen_acraft) && number_percent() < 40)
    do_mpasound(ch, "You hear a steady ring of a hammer near by.");
  else if (is_affected(ch, gen_wcraft) && number_percent() < 40)
    do_mpasound(ch, "You hear a steady grinding of a wheatstone near by.");
  return TRUE;
}


int request_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //this basicly nukes the item in anger.
  //as if the prayer was completed the item would not be on the ground now.
  OBJ_DATA* obj;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_request)
    return -1;

  //check if this is naturla end (get item) or not.
  if (paf->duration == 0)
    {
    run_effect_update(ch, paf, NULL, gen_request, NULL, NULL, TRUE, EFF_UPDATE_NONE);
    return TRUE;
    }

  //now we kill the item.
  for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content ){
    if (is_name( paf->string, obj->name ) )
      break;
  }
  if ( !obj)
    return FALSE;

  //Now we smash the item!
  act("You feel $g's anger at your weakness and shortcomings as the sky darkens above you.", ch, NULL, NULL, TO_CHAR);
  act("Seconds later a bolt of lightning arcs from the heavens and blows $p apart!", ch, obj, NULL, TO_CHAR);
  act("A blinding bolt of lightning arcs from the heavens and blows $p apart!", ch, obj, NULL, TO_ROOM);
  extract_obj(obj);
  return TRUE;
}

int request_none(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf;
  //  CHAR_DATA* victim = (CHAR_DATA*) arg1;
 //this is run whenever request is interrupted. we just show a message and kill
  send_to_char("Your prayer disturbed you snap back to reality.\n\r", ch);
  //we set duration to non-0 as to prevent the item being take if this was the last tick.

  if ( (paf = affect_find(ch->affected, gen_request)) != NULL)
    paf->duration = 1;

  affect_strip(ch, gen_request);
  return TRUE;
}

int request_precommand(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*returns true to cancel command */
  int cmd = (int) *int1;


  if ( cmd_table[cmd].level < IM
       && strcmp("help", cmd_table[cmd].name)
       && strcmp("tome", cmd_table[cmd].name)
       && strcmp("affects", cmd_table[cmd].name)
       && strcmp("score", cmd_table[cmd].name)
       && strcmp("who", cmd_table[cmd].name)
       && strcmp("bio", cmd_table[cmd].name)
       && strcmp("info", cmd_table[cmd].name)
       && strcmp("whois", cmd_table[cmd].name)
       && strcmp("save", cmd_table[cmd].name)
       && strcmp("interrupt", cmd_table[cmd].name)
       )
    {
      if (gn == gen_request)
	act("Knowing you are being judged by $g at this very moment?!", ch, NULL, NULL, TO_CHAR);
      else if (gn == gen_chant)
	act("You chant: 'Dies irae, dies illa solvet saeclum in favilla..'", ch, NULL, NULL, TO_CHAR);
      else if (gn == gen_acraft)
	send_to_char("You are far too busy with the hammer and the armor piece.\n\r", ch);
      else if (gn == gen_wcraft)
	send_to_char("You are far too busy with the hammer and the weapon piece.\n\r", ch);
      else if (gn == gen_conjure)
	send_to_char("You are locked in combat of wills with the demons!\n\r", ch);
      return TRUE;

    }
return FALSE;
}

//-------======== UNLIFE------==========//
int unlife_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//This is the effect buffer.
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  AFFECT_DATA af;
//some integers for calculations.
  int dur, mod= 0;
//finaly flags that affect if we outcast/align change.
  bool do_outcast = FALSE;
  bool do_align = FALSE;

  if (gn != gen_unlife)
    return -1;


  if ( (ch == NULL) || (ch->in_room == NULL)
       || paf->modifier < 0)
    return FALSE;

  if (paf->modifier == 0){
    dur = URANGE(1, 17 - mud_data.time_info.hour, 4);
    //we make the sucker fall asleep.
    af.where     = TO_AFFECTS;
    af.type      = gsn_sleep;
    af.level     = ch->level;
    af.duration  = dur;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SLEEP;
    affect_to_char( ch, &af );

    send_to_char("You suddenly fall into a heavy dreamless slumber.\n\r", ch);
    if ( IS_AWAKE(ch) )
      {
	act("$n suddenly rolls his eyes and passes out in a heavy deathlike slumber.", ch, NULL, NULL, TO_ROOM);
	do_sleep(ch,"");
      }
    send_to_char("As you drift dreamless, in horror you relize that with the blood of\n\rundead now mixed with your own, you are a stranger in your own land. ", ch);
  }
  //now we apply the unlife.
  //If this is an NPC we return, same with immortal.
  if (IS_NPC(ch) || IS_IMMORTAL(ch))
  return FALSE;
  //Undead
  SET_BIT(ch->act, PLR_UNDEAD);
  //And we test for outcast.
  if (!IS_SET(ch->act, PLR_OUTCAST) && paf->modifier == 0)
  do_outcast = TRUE;
  //And test for align change.
  if (ch->alignment != -750)
  do_align = TRUE;

//check if we make him evil unless already evil.
  if (do_align)
  {
//we mark mod that we should undo align when effect expires.
    wiznet("Through blood of undead $N now walks darker paths of life.",ch,NULL,WIZ_PENALTIES,WIZ_SECURE, LEVEL_HERO);
    mod +=1;
    //decrease align by 750 unless already evil.
    ch->alignment -= 750;
  }

//check if we outcast him.
  if (do_outcast)
  {

    wiznet("Through blood of undead $N is now an outcast to society.",ch,NULL,WIZ_PENALTIES,WIZ_SECURE, LEVEL_HERO);
    mod +=2;
    SET_BIT(ch->act, PLR_OUTCAST);
  }
  //we set the bitstate.
  paf->modifier = mod;
  if (IS_AVATAR(ch))
    divfavor_gain(ch, DIVFAVOR_LOSS);
  return TRUE;
}//end unlife._init



//this function undoes unlife effects based on the modifier of the effect.
int unlife_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//we will need data from the effect.
  AFFECT_DATA* unlife = (AFFECT_DATA*) arg1;

//and some misc data.
  int mod = 0;


  //we get the modifier to tell us what was changed.
  mod = unlife->modifier;

//if something was changed.
  if (mod > 0){

//now we restore stuff based on the modifier.
//3 means 1 || 2 which means both align (1) and outcast (2)
//I didnt feel like makind seprate flags for these..

//we remove outcast.
    if (mod & 2)
      {
	wiznet("The blood of undead purged, $N once again is a part of society.",ch,NULL,WIZ_PENALTIES,WIZ_SECURE, LEVEL_HERO);
	REMOVE_BIT(ch->act, PLR_OUTCAST);
    }

//increase align.
    if (mod & 1)
    {
      wiznet("The blood of undead purged, $N returns to his former path.",ch,NULL,WIZ_PENALTIES,WIZ_SECURE, LEVEL_HERO);
      ch->alignment +=750;
    }
  }
  //sho messages
  if (!IS_SET(race_table[ch->race].act, ACT_UNDEAD))
    REMOVE_BIT(ch->act, PLR_UNDEAD);
  if (mod != -1){
    act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
    act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  }
  return TRUE;
}//end unlife_kill

int unlife_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1,
int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if(gn != gen_unlife)
    return FALSE;
  /* dont process on modifier == -1 */
  if (paf->modifier < 0)
    return FALSE;

  //situation when characted doesnt get burnt.
  if (IS_NPC(ch))
    return FALSE;
  if (IS_IMMORTAL(ch))
    return FALSE;
  if (ch->in_room == NULL)
    return FALSE;
  if (is_affected(ch,gsn_covenant))
    return FALSE;
  if ( is_ghost(ch,600))
    return FALSE;
  if (IS_SET(ch->in_room->room_flags2,ROOM_NIGHT) )
    return FALSE;


  if( mud_data.time_info.hour > 11 && mud_data.time_info.hour < 13
      && !IS_SET(ch->in_room->room_flags, ROOM_DARK)
      &&!IS_SET(ch->in_room->room_flags,ROOM_INDOORS))
    {
      int dam2 = ch->level/2;
      send_to_char("The rays from the sun burn into your flesh.\n\r",ch);
      if (mud_data.time_info.hour < 12)
	dam2 = dam2*mud_data.time_info.hour/12;
      else if (mud_data.time_info.hour > 12)
	dam2 = dam2*(24 - mud_data.time_info.hour)/12;
      else
	dam2 = 3*dam2/2;
      damage(ch,ch,dam2, 1052, DAM_NONE,TRUE);
    }

  return TRUE;
}



//-----===SEREMO====------//

int seremon_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//This is the effect buffer.
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_seremon)
    return -1;

  /* all we do here is shoot out message, set the lag, and cut mana. */
  act("Out of no where, a long, boring, righteous seremon starts playing out in your head.", ch, NULL, NULL, TO_CHAR);
  WAIT_STATE2(ch, effect_table[GN_POS(gn)].value1[0]);
  ch->mana -= paf->modifier;
  paf->modifier = UMAX(5, paf->modifier - 5);
  return TRUE;
}


int seremon_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//This is the effect buffer.
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_seremon)
    return -1;

  /* all we do here is shoot out message, set the lag, and cut mana. */
  sendf(ch, "%s's booming voice inside your head begins the righteous seremon all over again.\n\r", paf->string);
  act("$n stops for a moment and curses loudly.", ch, NULL, NULL, TO_ROOM);
  WAIT_STATE(ch, number_range(effect_table[GN_POS(gn)].beats, 2 * effect_table[GN_POS(gn)].beats));
  ch->mana -= paf->modifier;
  ch->mana = UMAX(0, ch->mana);
  paf->modifier = UMAX(5, paf->modifier - 5);
  return TRUE;
}


int seremon_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  if (gn != gen_seremon)
    return -1;
//we run the room_sepc_update to maybe remove the SPEC_UPDATE bit.
  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}//end seremon_kill


//---------====HOLY HANDS===----------//
/* Individual god effects. */

/* FAITH */
inline bool holy_0(CHAR_DATA* ch, CHAR_DATA* victim, int dam, int gn)
{
  /*Idividiual attacks */

  int chance = effect_table[GN_POS(gn)].value2[0];
  //  int eff_chance =  effect_table[GN_POS(gn)].value1[0];
  int new_dam = number_range(dam, 2 * dam);

  if (number_percent() > chance)
    return FALSE;
  act("$n's hands flash with holy power!", ch, NULL, NULL, TO_ROOM);
  act("$g's power courses through your palms.", ch, NULL, NULL, TO_CHAR);
  damage(ch, victim, new_dam, gen_holy_hands, DAM_HOLY, TRUE);
  return TRUE;
}


/* */
inline bool holy_2(CHAR_DATA* ch, CHAR_DATA* victim, int dam, int gn)
{
  /*Idividiual attacks */

  int eff_chance =  effect_table[GN_POS(gn)].value1[2];

  if (number_percent() > eff_chance)
    return FALSE;
  act("$n's blur with speed as they weave a strange pattern.", ch, NULL, NULL, TO_ROOM);
  act("Your hands bluring with speed you beffudle $N.", ch, NULL, victim, TO_CHAR);
  act("$n beffudles you!", ch, NULL, NULL, TO_VICT);
  victim->mana  -= number_range(ch->level / 10,  ch->level / 3);
  return TRUE;
}//END HOLY2


/* KNOWLEDGE */
inline bool holy_3(CHAR_DATA* ch, CHAR_DATA* victim, int dam, int gn)
{
  /*Idividiual attacks */

  int chance = effect_table[GN_POS(gn)].value2[0];
  int eff_chance =  effect_table[GN_POS(gn)].value1[3];
  int pen =  effect_table[GN_POS(gn)].value3[0];
  int new_dam = number_range(dam, 2 * dam);

  if (number_percent() > chance)
    return FALSE;
  act("$n's hands flash holy power!", ch, NULL, NULL, TO_ROOM);
  act("$g's power courses through your palms.", ch, NULL, NULL, TO_CHAR);
  damage(ch, victim, new_dam, gen_holy_hands, DAM_ENERGY, TRUE);
  //speciual effect.
  if (number_percent() < eff_chance)
    {
      act("A shimmering shell of magic encases you momentarly.", victim, NULL, NULL, TO_CHAR);
      act("A shimmering shell of magic encases $n momentarly.", victim, NULL, NULL, TO_ROOM);
      spell_shrink(skill_lookup("shrink"),ch->level - pen, ch, (void *) victim,TARGET_CHAR);
      spell_slow(skill_lookup("slow"),ch->level - pen, ch, (void *) victim,TARGET_CHAR);
    }
  return TRUE;
}


/* LIFE */
inline bool holy_4(CHAR_DATA* ch, CHAR_DATA* victim, int dam, int gn)
{
  /*Idividiual attacks */

  int chance = effect_table[GN_POS(gn)].value2[0];
  int eff_chance =  effect_table[GN_POS(gn)].value1[4];
  int new_dam = number_range(dam, 2 * dam);

  if (number_percent() > chance)
    return FALSE;
  act("$n's hands flash holy power!", ch, NULL, NULL, TO_ROOM);
  act("$g's power courses through your palms.", ch, NULL, NULL, TO_CHAR);
  damage(ch, victim, new_dam, gen_holy_hands , DAM_LIGHT, TRUE);
  //speciual effect.
  if ( (number_percent() < eff_chance) && IS_SET(victim->act, ACT_UNDEAD))
    {
      AFFECT_DATA af;
      act("A glowing arua of light flashes around you.", victim, NULL, NULL, TO_CHAR);
      act("A glowing arua of light flashes around $n.", victim, NULL, NULL, TO_CHAR);
      af.type = gsn_drained;
      af.duration = number_fuzzy(ch->level / 12);
      af.level = ch->level;

      af.where = TO_AFFECTS;
      af.bitvector = 0;

      af.location = APPLY_NONE;
      af.modifier = 0;

      /* Useless conditional */
      if( af.modifier != 0 )
        af.modifier = 0;
    }
  return TRUE;
}//END HOLY4


/* CHANCE */
inline bool holy_5(CHAR_DATA* ch, CHAR_DATA* victim, int dam, int gn)
{
  /*Idividiual attacks */

  int chance = effect_table[GN_POS(gn)].value2[0];
  int eff_chance =  effect_table[GN_POS(gn)].value1[5];
  int new_dam = number_range(dam, 2 * dam);
  int pen =  effect_table[GN_POS(gn)].value3[0];

  if (number_percent() > chance)
    return FALSE;
  act("$n's hands flash holy power!", ch, NULL, NULL, TO_ROOM);
  act("$g's power courses through your palms.", ch, NULL, NULL, TO_CHAR);
  damage(ch, victim, new_dam, gsn_holy_hands, DAM_ENERGY, TRUE);
  //speciual effect.
  if (number_percent() < eff_chance)
    {
      act("The reality seems to bend around you for a second..", victim, NULL, NULL, TO_CHAR);
      act("For a moment reality parts around $n", victim, NULL, NULL, TO_ROOM);
      spell_dispel_magic(skill_lookup("dispel magic"),ch->level - pen, ch, (void *) victim,TARGET_CHAR);
    }
  return TRUE;
}//END HOLY5


/* DEATH */
inline bool holy_6(CHAR_DATA* ch, CHAR_DATA* victim, int dam, int gn)
{
  /*Idividiual attacks */

  int chance = effect_table[GN_POS(gn)].value2[0];
  int eff_chance =  effect_table[GN_POS(gn)].value1[6];
  int new_dam = number_range(dam, 2 * dam);
  int pen =  effect_table[GN_POS(gn)].value3[0];

  if (number_percent() > chance)
    return FALSE;
  if (IS_SET(victim->act, ACT_UNDEAD))
      return FALSE;

  act("$n's hands flash holy power!", ch, NULL, NULL, TO_ROOM);
  act("$g's power courses through your palms.", ch, NULL, NULL, TO_CHAR);
  damage(ch, victim, new_dam, gen_holy_hands, DAM_NEGATIVE, TRUE);
  //speciual effect.
  if (number_percent() < eff_chance)
    {
      int exp_dr = dam / 5;
      int mana_dr = dam / 5;
      int move_dr = dam / 5;

      act("All vitality escapes your body as forces of death tear at your soul.", victim, NULL, NULL, TO_CHAR);
      act("Shadows envelop $n.", victim, NULL, NULL, TO_ROOM);
      spell_plague(skill_lookup("plague"),ch->level - pen, ch, (void *) victim,TARGET_CHAR);

      victim->mana = UMAX(0, victim->mana - mana_dr);
      victim->move = UMAX(0, victim->move - move_dr);
      sendf(victim, "You lose %d experience!\n\r", exp_dr);
      gain_exp(victim, -exp_dr);

      ch->mana = UMIN(ch->max_mana, ch->mana + mana_dr);
      ch->move = UMIN(ch->max_move, ch->move + move_dr);
      sendf(ch, "You gain %d experience!\n\r", exp_dr);
      gain_exp(ch, exp_dr);
     }
  return TRUE;
}//END HOLY6




int holy_hands_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;
  AFFECT_DATA af;

  if (gn != gen_holy_hands)
    return -1;

  //send out the messages.
  act("Your hands tremble with force as aura of $g's power surrounds them.",ch, NULL, NULL, TO_CHAR);
  act("Aura of $g's power surrounds $n's hands.",ch, NULL, NULL, TO_ROOM);

  if (IS_NPC(ch))
    return TRUE;
//special things for Ath's god.
  if (ch->pcdata->way != deity_lookup("combat"))
    return TRUE;
  //we use gsn here so we dont get an infine loop.
  af.type =  gsn_holy_hands;
  af.level = paf->level;
  af.duration = paf->duration;
  af.where = TO_AFFECTS;
  af.bitvector = AFF_HASTE;
  af.location = APPLY_MANA;
  af.modifier = -150;
  affect_to_char(ch, &af);
  return TRUE;
}//end holyhands_init


int holy_hands_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_holy_hands)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  //strip of ath's effects.

  if (paf->modifier != deity_lookup("Athaekeetha"))
    return TRUE;
  if (is_affected(ch, gsn_holy_hands))
    affect_strip(ch, gsn_holy_hands);
  return TRUE;
}//end holyhands_kill


int holy_hands_postdamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* run after the weapon causes damage and has gen_holy_hands */
  //Attacker passed by ch
  //victim passed by arg1
  //obj passed by arg2
  //damage done by int1;
  //dam type by int2;
  //dt by flag;


  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  AFFECT_DATA *paf = ( AFFECT_DATA*) arg2;

//data
  int dam = (int) *int1;
  //  int dam_type = (int) *int2;
  int dt = flag;

//check if we are barehanded.
  if (get_eq_char(ch, WEAR_WIELD) != NULL)
    return FALSE;

  /* check that damage was done by hands of owner */
  if (dt != TYPE_HIT + attack_lookup("punch"))
    return FALSE;

  //We basicly just pick the god based on modifier
  switch (paf->modifier)
    {
    case PATH_FAITH : return holy_0(ch, victim, dam, gn);
    case PATH_KNOW  : return holy_3(ch, victim, dam, gn);
    case PATH_LIFE  : return holy_4(ch, victim, dam, gn);
    case PATH_CHANCE: return holy_5(ch, victim, dam, gn);
    case PATH_DEATH : return holy_6(ch, victim, dam, gn);
    default:
      return holy_0(ch, victim, dam, gn);
    }
  return TRUE;
}


//-----=====VOODOO DOLL======------//
//This GEN is place only on the character to avoid extra decisoins between char/obj versons.
//insted GSN_voodoo_spell is placed on the doll after creation, and on char after spell cast.

/* voodoo_doll_init */
int voodoo_doll_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_voodoo_doll)
    return -1;

  send_to_char("You feel the evil powers of doll making drain from your body.\n\r", ch);
  return TRUE;
}

/* voodoo_doll_kill */
int voodoo_doll_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_voodoo_doll)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}


//-----=====PC CHARM======------//

/* pc_charm_init */
int pc_charm_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_pc_charm)
    return -1;

  if (IS_IMMORTAL(ch)
      || (IS_NPC(ch) && IS_SET(ch->act,ACT_AGGRESSIVE))
      || IS_SET(ch->imm_flags,IMM_CHARM)
      || IS_UNDEAD(ch)
      )
    {
      send_to_char("You easly shrug off the charm attempt.\n\r", ch);
      affect_strip(ch, gn);
      return FALSE;
    }
  if (!IS_AFFECTED(ch, AFF_CHARM))
    SET_BIT(ch->affected_by, AFF_CHARM);

  send_to_char("You have been charmed!\n\r", ch);
  return TRUE;
}


/* pc_charm_kill */
int pc_charm_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_pc_charm)
    return -1;


  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

/* check if we are fighting a mob */
  if (ch->fighting && IS_NPC(ch->fighting)){
    char buf[MIL];
/* we pull a nasty little trick on the abuser of charm */
    if (ch->master && ch->master->in_room == ch->fighting->in_room){
      sprintf(buf, "Aha! A foul deception. Die %s!", PERS2(ch->master));
      do_say(ch->fighting, buf);
      set_fighting(ch->fighting, ch->master);
      stop_fighting(ch, FALSE);
    }
    else{
      sprintf(buf, "Aha! A foul deception. Go in peace %s.", PERS2(ch));
      do_say(ch->fighting, buf);
      stop_fighting(ch->fighting, FALSE);
      stop_fighting(ch, FALSE);

    }
  }
  stop_follower(ch);

  return TRUE;
}


/* prredamage */
int pc_charm_predamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//all we do here is strip the effect.
  affect_strip(ch, gn);
return TRUE;
}

/* precommand */
int pc_charm_precommand(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  /*returns true to cancel command */
  int cmd = (int) *int1;
  char argument[MIL], arg[MIL];

  strcpy(argument, (char*) arg2);
  one_argument(argument, arg);

/* pass the command on these actions. */
  if ( ((strcmp("get", cmd_table[cmd].name)
	&& strcmp("drop", cmd_table[cmd].name)
	&& strcmp("stand", cmd_table[cmd].name)
	&& strcmp("wake", cmd_table[cmd].name)
	&& strcmp("kill", cmd_table[cmd].name)
	&& strcmp("remove", cmd_table[cmd].name)
	&& strcmp("hit", cmd_table[cmd].name)
	&& strcmp("affects", cmd_table[cmd].name)
	&& strcmp("score", cmd_table[cmd].name)
	&& strcmp("who", cmd_table[cmd].name)
        && strcmp("whois", cmd_table[cmd].name))
	&& cmd_table[cmd].level < IM)
       || !strcmp("all", arg)
       )
    {
      send_to_char("The action is too complicated for your dazed mind.\n\r", ch);
    return TRUE;
    }
  //THERE IS SLIGHT WAIT ON COMMANDS HERE.
  WAIT_STATE2(ch, PULSE_VIOLENCE);
  return FALSE;
}



//--------========BLADES========------//
/* vlades_init */
int blades_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  CHAR_DATA* vch;

  if (gn != gen_blades)
    return -1;
  send_to_char("You summon a pair of deadly blades and phase them out of this reality.\n\r", ch);


//message to people
  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
      if (vch == ch)
	continue;
      if (IS_AFFECTED(vch, AFF_BLIND) || !can_see(vch, ch))
	act("Something large and dangerous fades into existance around $n and slowly disappears.", ch, NULL, vch, TO_VICT);
      else
	act("A pair of giant scythes fades into existance around $n and slowly disappear.", ch, NULL, vch, TO_VICT);

    }
  return TRUE;
}

/* blades_kill */
int blades_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_blades)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}


/* blades_previolence */
int blades_previo(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  CHAR_DATA* vch;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_blades)
    return -1;
//This is called BEFORE setting ch->fighting
//so we check if we are not in combat already.
  if (ch->fighting != NULL)
    return FALSE;

//Message to char.
 if (!IS_AFFECTED(ch, AFF_BLIND))
  act("Your deadly scythes fade into reality and begin to rotate quickly.", ch, NULL, NULL, TO_CHAR);
else
  act("You sense your blades fade into reality and begin to rotate quickly.", ch, NULL, NULL, TO_CHAR);

//message to people
  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
      if (vch == ch)
	continue;
      if (IS_AFFECTED(vch, AFF_BLIND) || !can_see(vch, ch))
	act("You sense something large begin to move quickly around $n", ch, NULL, vch, TO_VICT);
      else
	act("A pair of giant blades phases in around $n and begin their deadly dance.", ch, NULL, vch, TO_VICT);
    }

  //message to surronding rooms.
  broadcast(ch, "You hear a sound of large blades whistling through the air.\n\r");

  //We set a swtich on the paf so we know to turn them off on tick.
  paf->modifier = TRUE;
  return TRUE;
}

/* blades_tick */
int blades_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  CHAR_DATA* vch;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int upkeep = effect_table[GN_POS(gn)].value2[0];

  if (gn != gen_blades)
    return -1;

  //Upkeep.
  if (ch->mana < upkeep)
    {
      send_to_char("You no longer have the power control your blades.\n\r", ch);
      affect_strip(ch, gen_blades);
      return FALSE;
    }
  else
    ch->mana -= upkeep;

  if (ch->fighting != NULL)
    return FALSE;

  //Set the switch so we dont repeat
  if (!paf->modifier)
    return FALSE;
  else
    paf->modifier = FALSE;

  send_to_char("The blades conveniently phase away from this reality.\n\r", ch);

  //message to people
  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
      if (vch == ch)
	continue;
      if (IS_AFFECTED(vch, AFF_BLIND) || !can_see(vch, ch))
	act("You sense something slow down and fade away.", ch, NULL, vch, TO_VICT);
      else
	act("The deadly blades around $n slow down and fade away.", ch, NULL, vch, TO_VICT);
    }
  return TRUE;
}

/* blades_predamage */
int blades_predamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //Attacker passed by ch
  //victim passed by arg1
  //paf passed by arg2
  //damage done by int1;
  //damtype by int2
  //dt by flag;


  CHAR_DATA* vch;
  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  //  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;
  int dam_type = *int2;
  int dt = flag;

  //data
  int chance = effect_table[GN_POS(gn)].value3[0];

 //ez
 if (gn != gen_blades)
   return -1;

 //return on on damage
 if (*int1 < 1)
   return FALSE;

 //only block attacks (virtual attacks still have dt's in >= TYPE_HIT)
 if (dt < TYPE_HIT || dam_type == DAM_NONE || dam_type == DAM_INTERNAL)
   return FALSE;

 if (number_percent() > chance)
   return FALSE;

 //send messages out
 //Message to char.
 if (!IS_AFFECTED(victim, AFF_BLIND))
   act("The giant blade deflects $n's attack.", ch, NULL, victim, TO_VICT);
 else
   act("Something deflects $n's attack.", ch, NULL, victim, TO_VICT);

 //message to people
 for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
   {
     if (vch == victim)
       continue;
     if (IS_AFFECTED(vch, AFF_BLIND) || !can_see(vch, victim))
       act("Something deflects the attack away from $n.", victim, NULL, vch, TO_VICT);
     else
       act("One of the blades deflects the attack away from $n.", victim, NULL, vch, TO_VICT);
   }

 //Make it miss.
 *int1 = -1;
 return TRUE;
}


/* blades_precombat */
int blades_precombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  //  CHAR_DATA* victim = (CHAR_DATA*) arg2;
  CHAR_DATA* vch, *vch_next;

  /* notice blades dont have tahc0 calc. they hit, or they are blocked */

//data
  int dam_1 = 2;
  int dam_2 = 2;
  int dam = 0;
  int h_roll = effect_table[GN_POS(gn)].value1[URANGE(0, paf->level/5, 10)] / 2;
  int d_roll = effect_table[GN_POS(gn)].value1[URANGE(0, paf->level/5, 10)];

//consts
 const int skill_med = 75;
 const int skill_mod = 5;

 const int level_med = 30;
 const int level_mod = 5;

 const int h_roll_mod = 100; //multiplier for skill bonus to h_roll (%)

 const int dt1 = attack_lookup("slice") + TYPE_HIT;
 const int dt2 = attack_lookup("slash") + TYPE_HIT;

 const int dam_type = DAM_SLASH;

/*
   The hit is based on values from value1 (increases with level)
   and skill
   Damage on the level of the spell and
   the skill of the user in the spell
*/

//get the damage bonus here.
 dam_1 += UMAX(0, get_skill(ch, gsn_blades) - skill_med) / skill_mod;

 dam_2 += UMAX(0, paf->level - level_med) / level_mod;

 dam = dice (dam_1, dam_2) + d_roll;
 h_roll = h_roll_mod * h_roll / 100;
 // sendf(ch, "d_roll: %d, dam: %d, h_roll: %d\n\r", d_roll, dam, h_roll);
 //two attacks against anyone in combat with us.
  for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
      vch_next = vch->next_in_room;
      if ( (ch->fighting && ch->fighting == vch)
	   || (vch->fighting && vch->fighting == ch) ){

	if (ch->fighting != NULL
	    && vch->hit > 0
	    && vch->position > POS_STUNNED
	    && vch->in_room == ch->in_room
	    && !is_safe_quiet(ch, vch))
	  virtual_damage(ch, vch, NULL, dam, dt1, dam_type, h_roll, paf->level, TRUE, TRUE, gsn_blades);

	dam = dice (dam_1, dam_2) + d_roll;
	if (ch->fighting != NULL
	    && vch->hit > 0
	    && vch->position > POS_STUNNED
	    && vch->in_room == ch->in_room
	    && !is_safe_quiet(ch, vch))
	  virtual_damage(ch, vch, NULL, dam, dt2, dam_type, h_roll, paf->level, TRUE, TRUE, gsn_blades);
	check_improve(ch, gsn_blades, TRUE, 1);
      }//End if should hit
    }//end for
 return TRUE;
}


//--------========DANCING BLADE========------//


/* finds the dancing obj on character or room based on the key. */
OBJ_DATA* get_obj_key(OBJ_DATA* list, int key, int depth)
{
//Fairly simple recursive function.
//We search objects if found returns the dancing bladed object
//if false returns NULL. fRoom TRUE = search room.

  OBJ_DATA* vobj;
  OBJ_DATA* buf;
  AFFECT_DATA *paf;

  if (depth > 9) {
    return NULL;
  }
  if (list == NULL) {
    return NULL;
  }

  for (vobj = list; vobj != NULL; vobj = vobj->next_content)
  {

    if (vobj->item_type == ITEM_CONTAINER) {
      if ( (buf = get_obj_key(vobj->contains, key, depth++)) != NULL) {
	return buf;
      }
      else {
	continue;
      }
    }

    if (vobj->item_type != ITEM_WEAPON) {
      continue;
    }
    if ( (paf = affect_find(vobj->affected, gsn_dan_blade)) == NULL) {
      continue;
    }
    if (paf->modifier != key) {
      continue;
    }
    return vobj;
  }
return NULL;
}

/* this is the function that looks for the dancin blade in the room and character. */
OBJ_DATA* get_dan_blade(CHAR_DATA* ch, int key)
{
  //we simply look in the character and the room.
  OBJ_DATA* obj;
  if ((obj = get_obj_key(ch->carrying, key, 0)) != NULL) {
    return obj;
  }
  if (ch->in_room != NULL) {
    if ( (obj = get_obj_key(ch->in_room->contents, key, 0)) != NULL) {
      return obj;
    }
  }
  return NULL;
}




/* dancingblade_init */
int dan_blade_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  OBJ_DATA* obj;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  AFFECT_DATA* opaf;
  if (gn != gen_dan_blade)
    return -1;
//we look for weapon and  make sure the bitvector on object is 1 (weapon visible)

  if ( (obj = get_dan_blade(ch, paf->modifier)) == NULL)
    {
      send_to_char("Nothing seems to happend.\n\r", ch);
      return FALSE;
    }
  if ( (opaf = affect_find(obj->affected, gsn_dan_blade)) == NULL)
    {
      send_to_char("Nothing seems to happend.\n\r", ch);
      return FALSE;
    }
  opaf->bitvector = TRUE;
  act("You breathe intelligence into $p and make it your servant.", ch, obj, NULL, TO_CHAR);
  act("$p quivers, rises into the air before $n, and almost with a nod falls back into $s hands.",
      ch, obj, NULL, TO_ROOM);
  return TRUE;
}

/* dancing_blade_kill */
int dan_blade_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;
  AFFECT_DATA* bpaf;
  OBJ_DATA* obj;
  char buf[MIL];

  if (gn != gen_dan_blade)
    return -1;

//this time we get the name of the item from paf.
  sprintf(buf, "You sense your control over %s falter and break.", paf->string);
  act_new(buf, ch, NULL, NULL, TO_CHAR, POS_DEAD);
  for (obj=object_list; obj != NULL; obj= obj->next)
    {
      if (obj->item_type != ITEM_WEAPON)
	continue;
      if ( (bpaf = affect_find(obj->affected, gsn_dan_blade)) == NULL)
	continue;
      if (paf->modifier != bpaf->modifier)//KEY CHECK
	continue;
      if (!bpaf->has_string)
	continue;
      if (strcasecmp(ch->name, bpaf->string))
	continue;

      //we strip the object
      affect_strip_obj(obj, gsn_dan_blade);
      if (obj->carried_by != NULL)
	act("$p seems to shiver slightly.", obj->carried_by, obj, NULL, TO_CHAR);
      else if (obj->in_room != NULL)
	if (obj->in_room->people != NULL)
	  act("$p seems to shiver slightly.", obj->in_room->people, obj, NULL, TO_ALL);

    }//END FOR
  return TRUE;
}

/* dancing_blade_tick */
int dan_blade_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  AFFECT_DATA* bpaf;
  OBJ_DATA* obj;
  int upkeep = effect_table[GN_POS(gn)].value2[0];
  /*
1) check for object proximity
2) check for upkeep.
  */

    if (paf->type != gen_dan_blade)
    return -1;

  if ( (obj = get_dan_blade(ch, paf->modifier)) == NULL)
     {
       sendf(ch, "With %s not in near proximity your link to it collapses.\n\r"
	     , paf->string);
      affect_strip(ch, gen_dan_blade);
      return FALSE;
    }

  if ( (bpaf = affect_find(obj->affected, gsn_dan_blade)) != NULL)
    if (ch->fighting == NULL && !bpaf->bitvector)
      {
	//change this first so we can se the blade.
	bpaf->bitvector = TRUE;
	act("$p settles down and returns to you.", ch, obj,  NULL, TO_CHAR);
	act("$p settles down and returns to $n.", ch, obj, NULL, TO_ROOM);
      }

  //Upkeep.
  if (ch->mana < upkeep)
    {
      sendf(ch, "You no longer have the power to maintain a link to %s.\n\r", paf->string);
      affect_strip(ch, gen_dan_blade);
    }
  else
    ch->mana -= upkeep;
  return TRUE;
}

/* dancing_blade_previo */
int dan_blade_previo(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  AFFECT_DATA* bpaf;
  CHAR_DATA* victim = (CHAR_DATA* ) arg2;
  OBJ_DATA* obj;

  //bools
  bool in_obj = FALSE;
  bool in_char = FALSE;
  bool in_room = FALSE;


  //We check if item is around, if so we display messages based on where the item was.

  if (paf->type != gen_dan_blade)
    return -1;

  if ( (obj = get_dan_blade(ch, paf->modifier)) == NULL)
     {

      sendf(ch, "With %s not in near proximity your link to it collapses.\n\r",
	    paf->string);
      affect_strip(ch, gen_dan_blade);
      return FALSE;
    }

  //now we know the object is around, but not where.
  if (obj->carried_by != NULL)
    in_char = TRUE;
  else if (obj->in_obj != NULL)
    in_obj = TRUE;
  else if (obj->in_room != FALSE)
    in_room = TRUE;
  else
    {
      bug("dan_blade_previo: obj not in char, obj or room.", 0);
      return -1;
    }
  //show messages
  if ( (bpaf = affect_find(obj->affected, gsn_dan_blade)) == NULL)
    return - 1;

  //Just a spacer to accent the text.
  send_to_char("\n\r", ch);

  if (in_char)
    {
      if (obj->wear_loc != WEAR_NONE)
	{
	  //Check for no-remove
	  if (IS_OBJ_STAT(obj, ITEM_NOREMOVE))
	    {
	      act("$p lunges at $N with great force but you can't let go!", ch, obj, victim, TO_CHAR);
	      affect_strip(ch, gn);
	      return FALSE;
	    }
	  else
	    act("Against your best efforts $p flies out of your hands.", ch, obj, NULL, TO_CHAR);
	  unequip_char(ch, obj);
	}

      act("$p bobs in the air as it heads towards $n.", victim, obj, victim, TO_ROOM);
      act("$p bobs in the air as it heads towards you.", ch, obj, victim, TO_VICT);
    }
  else if (in_obj)
    {
      char buf[MIL];
      OBJ_DATA * cont;
      sprintf(buf, "Spilling its contents, %s rips out $p"
	      , obj->short_descr);
      act(buf, ch, obj->in_obj, NULL, TO_ALL);
      cont = obj->in_obj;
      obj_from_obj(obj);
      obj_to_char(obj, ch);
      //we empty container.
      empty_obj(cont);
      extract_obj(cont);

      act("$p bobs in the air as it heads towards $n.", victim, obj, victim, TO_ROOM);
      act("$p bobs in the air as it heads towards $n.", ch, obj, victim, TO_VICT);

    }
  else if (in_room)
    {
      obj_from_room(obj);
      obj_to_char(obj, ch);
      act("$p bobs in the air as it heads towards $n.", victim, obj, victim, TO_ROOM);
      act("$p bobs in the air as it heads towards $n.", ch, obj, victim, TO_VICT);
    }
  else
    {
      bug("dan_blade_previo: obj not in char, obj or room.", 0);
      return -1;
    }

  //Make the bvlade disappear form room/inv AFTER messgaes.
  bpaf->bitvector = FALSE;

  return TRUE;
}

int dan_blade_postvio(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //All we do is check if anyone in the roomis still fighting with us.
  //if not we shoe message and make obj visible again.
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  AFFECT_DATA* bpaf;

  CHAR_DATA* vch;
  OBJ_DATA* obj;
  bool in_combat = FALSE;



  if (paf->type != gen_dan_blade)
    return -1;

  if ( (obj = get_dan_blade(ch, paf->modifier)) == NULL)
     {
      sendf(ch, "With %s not in near proximity your link to it collapses.\n\r",
	    paf->string);

      affect_strip(ch, gen_dan_blade);
      return FALSE;
    }

  //check for combat.
  //This is run before the current opponent is cleared, so we
  //look for anyone except that oppononetn.
  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
      if (vch->fighting == ch && vch != ch->fighting)
	in_combat = TRUE;
    }



  if (!in_combat)
    {
      bpaf = affect_find(obj->affected, gsn_dan_blade);
      bpaf->bitvector = TRUE;
      act("$p settles down and returns to you.", ch, obj,  NULL, TO_CHAR);
      act("$p settles down and returns to $n.", ch, obj, NULL, TO_ROOM);

      return TRUE;
    }
  return FALSE;
}

/* special attack of dancing blade */
void dan_blade_special(CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj,
int spec, int dam, int dt,int dam_type, int h_roll, int level)
{
  OBJ_DATA* wield;
  char buf[MIL];
  int i = 0;
  //check which attack.
  switch (spec)
    {
/* TRIP */
    case 1:
      act("$p takes a vicious swipe at $N's legs!", victim, obj, victim, TO_ROOM);
      act("$p takes a vicious swipe at your legs!", victim, obj, victim, TO_CHAR);
      if (IS_AFFECTED(victim,AFF_FLYING)
	  && !is_affected(victim,gsn_thrash))
	{
	  act("$S feet aren't on the ground.",victim,NULL,victim,TO_ROOM);
	  act("Luckily your feet aren't on the ground.",ch,NULL,victim,TO_VICT);
	  break;
	}
      act( "You scream in pain and go down!",victim, NULL,victim,TO_CHAR);
      act("$n screams in pain and goes down.",victim, NULL,victim,TO_ROOM);
      if (ch->fighting != NULL
	  && victim->hit > 0
	  && victim->position > POS_STUNNED
	  && victim->in_room == ch->in_room
	  && !is_safe_quiet(ch, victim))
	virtual_damage(ch, victim, obj, dam, dt, dam_type, h_roll, level, TRUE, TRUE, gsn_dan_blade);
      WAIT_STATE2(victim, 2 * PULSE_VIOLENCE);
      break;
      /* END TRIP*/

    case 2:
/* DISARM */
      if ( (wield = get_eq_char( victim, WEAR_WIELD )) == NULL){
	break;
      }
      disarm(ch, victim );
      /* END DISARM */
    break;

/* CUSINART */
    case 3:
      sprintf(buf, "$p beings to spin in a deadly whirlwind of %s", obj->material);
      act(buf, ch, obj, NULL, TO_ALL);
      //start hitting
      for (i = 0; i < 4; i++)
	{
	  int new_dam = number_range(dam / 4, dam / 2);
	  if (ch->fighting != NULL
	      && victim->hit > 0
	      && victim->in_room == ch->in_room
	      && victim->position > POS_STUNNED
	      && !is_safe_quiet(ch, victim)
	      )
	      virtual_damage(ch, victim, obj, new_dam, dt, dam_type, h_roll, level, TRUE, TRUE, gsn_dan_blade);
	  else
	    break;
	}//end FOR
      break;
    }//END SWITCH
  return;
}


/* dan_blade_precombat */
int dan_blade_precombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* This is the main attack function for dancing blade.
1)
- If weapon not around we dont attack.
- If bitvector is not FALSE we dont attack.
2)
- Get THAC0 based on a Warrior mob of level = 1/2 spell lvl + skill - skill_mod
- GET ac of target.
- Roll for hit, if  miss set dam =  0

3)
- Hit roll based on lvl of weapon/2 + to_hit * the modifier from value1
- Dam roll based on weight/2 + to_Dam * value1 + damage dice
- Dt is weapon dt
- Dam_Type is weapon dam_type

4) There is (skill-95)*2 chance of doing 1 of 3 specials
I) > 95 : Lag attack
II) > 97: Disarm
III) > 99: Chopper

5) two attacks if at 100% of chance "double_chance"
  */

  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* victim = (CHAR_DATA*) arg2;
  OBJ_DATA* obj;
  AFFECT_DATA* bpaf;
  AFFECT_DATA* bpaf2;

//consts
 const int skill_med = 75;
 const int spec_mod = 85;

 const int spec_1 = 40;
 const int spec_2 = 80;
 const int double_chance = 30;

//data
 int thac0 = 0;
 int ac = 0;
 int dam = 0;
 int dam_type = DAM_BASH;
 int dt = 0;

 int chance = 0;
 int roll = 0;

 int skill = get_skill(ch, gsn_dan_blade);

 int h_d_mod = effect_table[GN_POS(gn)].value1[URANGE(0, (skill - skill_med) / 2, 12)];
 int h_roll = 0 ;
 int d_roll = 0 ;
 int diceroll = 0;

  if (paf->type != gen_dan_blade)
    return -1;


 //check if item is around.
  if ( (obj = get_dan_blade(ch, paf->modifier)) == NULL)
     {

      sendf(ch, "With %s not in near proximity your link to it collapses.\n\r",
	    paf->string);
      affect_strip(ch, gen_dan_blade);
      return FALSE;
    }
  if ( (bpaf = affect_find(obj->affected, gsn_dan_blade)) == NULL)
    {
      bug("dan_blade_precombat: bpaf not found..", 0);
      return -1;
    }

  //check ifincombat mode
  if (bpaf->bitvector)
    return FALSE;

  //get the dam/hit modifiers.
  if  (obj->enchanted)
    bpaf2 = obj->affected;
  else
    bpaf2 = obj->pIndexData->affected;
  for (; bpaf2 != NULL; bpaf2 = bpaf2->next)
    {
      if (bpaf2->location == APPLY_HITROLL)
	  h_roll = bpaf2->modifier;
      if (bpaf2->location == APPLY_DAMROLL)
	  d_roll = bpaf2->modifier;
    }//end hit/dam search


//now we increaes the hit/dam by bonus.
  h_roll = h_roll *  h_d_mod / 200;
  d_roll = d_roll *  h_d_mod / 100;

  //add other hit/dam modifiers. (weight is in 10 * units)
  d_roll += UMIN(obj->weight, 400) / 10;
  h_roll += obj->level / 3;
  //sendf(ch, "d_roll %d, h_Roll %d, dam %d\n\r", d_roll, h_roll, dam);

//skill
  d_roll += (skill - skill_med) / 3;

  // sendf(ch, "d_roll %d, h_Roll %d, dam %d\n\r", d_roll, h_roll, dam);

//add dice roll for damage
  dam = d_roll + dice(obj->value[DICE_TYPE], obj->value[DICE_BONUS]) * h_d_mod / 200;
  dam = UMIN(125, dam);
  // sendf(ch, "d_roll %d, h_Roll %d, dam %d\n\r", d_roll, h_roll, dam);

//set damage type and text.
  dam_type = attack_table[(obj->value[3] > TYPE_HIT? obj->value[3] - TYPE_HIT : obj->value[3])].damage;
  dt = (obj->value[3] + TYPE_HIT);

  //get thaco and AC
  thac0 =  get_THAC0(ch, victim, obj, dt, FALSE, IS_NPC(ch), ACT_WARRIOR, TRUE,
		     gsn_dan_blade, paf->level);
  ac = get_AC(ch, victim, dt, dam_type, TRUE);
  //sendf(ch, "d_roll %d, h_Roll %d, dam %d thac0 %d AC %d\n\r", d_roll, h_roll, dam, thac0, ac);
  //We roll for miss
  while ( ( diceroll = number_bits( 5 ) ) >= 20 );
  if ( diceroll == 0 || ( diceroll != 19 && diceroll < thac0 - ac ) )
    dam = 0;

  //and we make an attack.

//decloak the sword for duration of attack.
  bpaf->bitvector = TRUE;

  if (ch->fighting != NULL
      && victim->hit > 0
      && victim->position > POS_STUNNED
      && victim->in_room == ch->in_room
      && !is_safe_quiet(ch, victim))
    virtual_damage(ch, victim, obj, dam, dt, dam_type, h_roll, paf->level, TRUE, TRUE, gsn_dan_blade);

  //cast spells etc.

  if (skill >= 100 && number_percent() < double_chance){
    while ( ( diceroll = number_bits( 5 ) ) >= 20 );
    if ( diceroll == 0 || ( diceroll != 19 && diceroll < thac0 - ac ) )
      dam = 0;
    dam = d_roll + dice(obj->value[DICE_TYPE], obj->value[DICE_BONUS]) * h_d_mod / 200;
    if (ch->fighting != NULL
	&& victim->hit > 0
	&& victim->in_room == ch->in_room
	&& victim->position > POS_STUNNED
	&& !is_safe_quiet(ch, victim))
      virtual_damage(ch, victim, obj, dam, dt, dam_type, h_roll, paf->level, TRUE, TRUE, gsn_dan_blade);
    //cast spells etc.
  }



  //special attacks.
  chance = UMAX(0, skill - spec_mod) + (skill > 95? 5 : 1);
  if ( (roll = number_percent()) < chance
       &&  ch->fighting != NULL
       && victim->hit > 0
       && victim->position > POS_STUNNED
       && !is_safe_quiet(ch, victim)
       )
       {
	 if (roll < chance * spec_1 / 100) roll = 1;
	 else if (roll < chance * spec_2 / 100) roll = 2;
	 else  roll = 3;
	 dan_blade_special(ch, victim, obj,roll, dam, dt, dam_type, h_roll, paf->level);
       }
//cloak the sword again.
  check_improve(ch, gsn_dan_blade, TRUE, 1);
  if (ch->fighting != NULL)
    bpaf->bitvector = FALSE;
  return TRUE;
}


/* ANGER fnctions */
int anger_search_room (CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag) {

  CHAR_DATA *vch, *next_vch;

  if (gn != gen_anger) {
    return -1;
  }
  //safety check for imm's accidently killing playres.
  if (IS_IMMORTAL(ch))
    return FALSE;

  if (!is_affected (ch, gen_anger)
      || !IS_AWAKE (ch)
      || IS_AFFECTED (ch, AFF_CALM)){
    return FALSE;
  }

  if (ch->in_room == NULL) {
    return -1;
  }

  for (vch = ch->in_room->people; vch != NULL; vch = next_vch){
    next_vch = vch->next_in_room;
    if (is_safe_quiet_all (ch, vch)
	|| !can_see (ch, vch)
	|| (ch == vch)) {
      continue;
    }
    else if (!IS_NPC (vch)) {
      do_murder (ch, vch->name);
    }
  }

  return FALSE;
}

int anger_kill (CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag) {

  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_anger) {
    return -1;
  }

  if (paf->location == APPLY_DAMROLL) {
    act_new (effect_table [GN_POS(gn)].msg_off, ch, NULL, NULL,
	     TO_CHAR, POS_DEAD);
    act_new (effect_table [GN_POS(gn)].msg_off2, ch, NULL, NULL,
	     TO_ROOM, POS_DEAD);
  }

  room_spec_update(ch->in_room);
  return TRUE;
}

int anger_leave (CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag) {

  if (gn != gen_anger) {
    return -1;
  }

  room_spec_update (ch->in_room);
  return TRUE;
}



//------------------=====TERRA SHIELD===-------//
/* terra_shieldinit */
int terra_shield_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int skill = get_skill(ch, gsn_terra_shield);
  if (gn != gen_terra_shield)
    return -1;

//all we do is show a message and based on the level
  act("The ground seems to turn strangly animated at $n's feet.", ch, NULL, NULL, TO_ROOM);

  //we check if the shield will break if caster moves.
  if (skill * paf->level / 100 >= 30) //(75% need 40)
    {
      act("You weave magic and power around yourself creating a true Terra Shield.", ch, NULL, NULL, TO_CHAR);
      paf->modifier = TRUE;
    }
  else
    {
      act("You exert your will and control over the ground around you.", ch, NULL, NULL, TO_CHAR);
      paf->modifier = FALSE;
    }
  return TRUE;
}//END Terrashield INIT


/* terra_shield_kill */
int terra_shield_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_terra_shield)
    return -1;

  act("The ground grows still at $n's feet.", ch, NULL, NULL, TO_ROOM);
  if (paf->modifier)
    act_new("The terra shield loses its sentience.", ch, NULL, NULL, TO_CHAR, POS_DEAD);
  else
    act_new("Your attention broken, the ground grows still at your feet.", ch, NULL, NULL, TO_CHAR, POS_DEAD);

  return TRUE;
}


/* terra_shield_leave */
int terra_shield_leave(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

 if (gn != gen_terra_shield)
   return -1;
 if (!paf->modifier)
   {
     send_to_char("\n\rYou are unable to sustain your terra-shield while moving.\n\r", ch);
     affect_strip(ch, gen_terra_shield);
   }
 return TRUE;
}

/* terra shield Enter */
int terra_shield_enter(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //We check if the new room is cabaple of having terra shield.
  //  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int sector = ch->in_room->sector_type;

  if (gn != gen_terra_shield)
    return -1;
  //check for sector.
  if (sector == SECT_AIR || sector == SECT_WATER_SWIM
      || sector == SECT_WATER_NOSWIM)
    {
      send_to_char("There is no ground here for you to animate.\n\r", ch);
      affect_strip(ch, gen_terra_shield);
      return FALSE;
    }
  return TRUE;
}


/* terra_shield_predamage */
int terra_shield_predamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //Attacker passed by ch
  //victim passed by arg1
  //paf passed by arg2
  //damage done by int1;
  //damtype by int2
  //dt by flag;

  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;
  int dam = (int) *int1;
  int dam_type = (int) *int2;
  int dt = flag;

  //data
  //shield blocks better if sentient
  int chance =  3 * victim->level/(paf->modifier? 5 : 6);
  int h_roll;
  //const
  const int level_mod = (paf->modifier? 2 : 1);
  const int luck_mod  = 2;

  //  sendf(victim, "Terra: dam: %d, chance: %dm dt %d\n\r", dam, chance, dt);
  //ez
  if (gn != gen_terra_shield)
    return -1;

  //dont deflect misses :)
  if (dam < 1)
    return FALSE;

 //only block attacks (virtual attacks still have dt's in >= TYPE_HIT)
 if (dt < TYPE_HIT || dam_type == DAM_NONE || dam_type == DAM_INTERNAL)
   return FALSE;

 chance += (URANGE(-10, victim->level - ch->level, 5)) * level_mod;
 chance += (get_curr_stat(victim, STAT_LUCK) - get_curr_stat(ch, STAT_LUCK)) * luck_mod;
 h_roll = URANGE(0, (GET_HITROLL(ch) - 20) / 2, chance / 2);
 chance -= h_roll;

 if (number_percent() < chance)
   {
     act("The ground before $n heaves and deflects the attack.", victim, NULL, NULL, TO_ROOM);
     act("Your Terra shield deflects the attack.", victim, NULL, NULL, TO_CHAR);
     check_improve(victim, gsn_terra_shield, TRUE, 1);
     dam = -1;
   }
 //return damage;
 *int1 = dam;
 return TRUE;
}


/* tick update */
int terra_shield_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int upkeep = effect_table[GN_POS(gn)].value2[0];

  //ez
  if (gn != gen_terra_shield)
    return -1;
  //True shield costs less
  if (paf->modifier)
    upkeep /= 2;

  if (ch->mana < upkeep)
    {
      send_to_char("You are no longer able to sustain your terra shield.\n\r", ch);
      affect_strip(ch, gn);
      return FALSE;
    }
  else
    ch->mana -= upkeep;
  return TRUE;
}



//----===REFLECTIVE SHIELD===-----//
/* ref_shieldinit */
int ref_shield_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_ref_shield)
    return -1;

//we set the stored damage to 30 for first time
  paf->modifier = 30;
//all we do is show a message and based on the level
  act("The air around $n sizzles with energy.", ch, NULL, NULL, TO_ROOM);
  act("You pour energy into the air around you and create a reflective field of force.", ch, NULL, NULL, TO_CHAR);
  return TRUE;
}//END refshield INIT


/* ref_shield_kill */
int ref_shield_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_ref_shield)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}



/* ref_shield_predamage */
int ref_shield_predamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //Attacker passed by ch
  //victim passed by arg1
  //paf passed by arg2
  //damage done by int1;
  //damtype by int2
  //dt by flag;

  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;
  int dam = (int) *int1;
  int dam_type = (int) *int2;
  int dt = flag;

  /*
the shield collects 1/4 damage per round, then releases it at the beggining of
combat round of character.
  */
  //data
  int dam_abs = 0;
  int cost = 0;

  //const
  int const ref_dam = effect_table[GN_POS(gn)].value1[0];


  //ez
  if (gn != gen_ref_shield)
    return -1;

  //dont deflect misses :)
  if (dam < 1)
    return FALSE;

 //only block attacks that are mundane and not spells.
  if (dt < TYPE_HIT || dam_type == DAM_NONE)
    return FALSE;

  //we get dmage absorbed
  dam_abs = ref_dam * dam / 100;
  cost = 1 + dam_abs / 10;

  //drain mana for use.
  if (victim->mana < cost)
    return FALSE;
  else
    victim->mana-= cost;
  //absorb damage

  paf->modifier += dam_abs;
  dam -= dam_abs;

 *int1 = UMAX(1,dam);
 return TRUE;
}


/* tick update */
int ref_shield_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int upkeep = effect_table[GN_POS(gn)].value2[0];

  //ez
  if (gn != gen_ref_shield)
    return -1;

  if (ch->mana < upkeep)
    {
      send_to_char("You are no longer able to sustain your reflective shield.\n\r", ch);
      affect_strip(ch, gn);
      return FALSE;
    }
  else
    ch->mana -= upkeep;
  //lose the charge.
  if (!ch->fighting)
    paf->modifier = UMAX(0, paf->modifier - 1);
  return TRUE;
}

/* ref_shield_precombat */
int ref_shield_precombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* victim = (CHAR_DATA*) arg2;

  const int max = effect_table[GN_POS(gn)].value3[0] + UMAX(0, (get_skill(ch, gsn_ref_shield) - 75) * 2);
  int dam = paf->modifier;

  //al we do is flash the capped damage back tocurrent target.

  if (dam < 1)
    {
      dam = 0;
      return FALSE;
    }
  //damage
  //debug

  //we clear the store except for 1/3 so the shield can charge up.
  paf->modifier -= 6 * dam / 10;
  if (paf->modifier > max / 2)
    {
      act("$n's reflective shield pulsates wildly with force.", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your reflective field has reached a steady state.\n\r", ch);
      paf->modifier += 25 + (get_skill(ch, gsn_ref_shield)- 75) * 2;
    }
  else
    {
      send_to_char("Your reflective shield releases its charge!\n\r", ch);
      act("The sizzling field around $n flares briefly.", ch, NULL, NULL, TO_ROOM);
    }
 if (ch->fighting != NULL && victim->hit > 0 && victim->position > POS_STUNNED && !is_safe_quiet(ch, victim))
   damage(ch, victim, URANGE(5, dam, max), gn, DAM_ENERGY, TRUE);



  return TRUE;
}



//----===PLUMBUM MANUS===-----//
/* plumbum_init */
int plumbum_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_plumbum)
    return -1;

  send_to_char("You place a field of increased gravity around yourself.\n\r", ch);
  act("You feel a bit heavier.", ch, NULL, NULL, TO_ROOM);

  return TRUE;
}//END refshield INIT


/* plumbum_kill */
int plumbum_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_plumbum)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}



/* plumbum_tick */
int plumbum_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int upkeep = effect_table[GN_POS(gn)].value2[0];

  //ez
  if (gn != gen_plumbum)
    return -1;

  if (ch->mana < upkeep)
    {
      send_to_char("You can no longer sustanin the field of increased gravity.\n\r", ch);
      affect_strip(ch, gn);
      return FALSE;
    }
  else
    ch->mana -= upkeep;

  return TRUE;
}

/* plumbum_precombat */
int plumbum_precombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* vch;
  AFFECT_DATA af;

  bool fDouble =  effect_table[GN_POS(gn)].value1[0];
  if (gn != gen_plumbum)
    return -1;

/* we run through all targets fighting group members and apllly the weak/slow effect. */
  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
      if (!vch->fighting)
	continue;
      if (!is_same_group(ch, vch->fighting))
	continue;
      if (is_safe_quiet(ch, vch))
	continue;

      if (is_affected(vch, gsn_plumbum))
	continue;
//apply effects.
      if (!saves_spell(paf->level, vch, DAM_OTHER, skill_table[gsn_plumbum].spell_type ) )
	{
	  if (!saves_spell(paf->level, vch, DAM_OTHER, skill_table[gsn_plumbum].spell_type ) || !fDouble)
	    {

	      send_to_char("Your limbs gain the weight of lead.\n\r", vch);
	      //debug
	      act("$n seems sluggish.", vch, NULL, vch, TO_ROOM);
	      af.type = gsn_plumbum;
	      af.level = paf->level;
	      af.duration = 0;

	      af.where = TO_AFFECTS;
	      af.bitvector = AFF_SLOW;

	      af.location = APPLY_STR;
	      af.modifier = -number_range(1, 2);
	      affect_to_char(vch, &af);
	    }
	}//end slow check
    }//end for

  return TRUE;
}


  /* plumbum_postcombat */
int plumbum_postcombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
  {
/* run ONLY if character has GSN_plumbum not GEN.
ie: this is run only by victims of the spell.
*/
    //  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_plumbum)
    return -1;


  affect_strip(ch, gsn_plumbum);
  return TRUE;
}



//----===Cloak of Discord===-----//
/* discord_init */
int discord_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_discord)
    return -1;

  send_to_char("You call onto forces of chaos to surround you.\n\r", ch);
  act("The space around $n bends and ripples strangley.", ch, NULL, NULL, TO_ROOM);
  broadcast(ch, "Shapes around you blur and ripple.\n\rHow strange.\n\r");

  /* Starts random */
  paf->modifier = number_range(0, 1);
  if (!paf->modifier){
    send_to_char("The field of chaos around you pulsates angry red then fades.\n\r", ch);
    act("The air around $n pulsates angry red then fades.", ch, NULL, NULL, TO_ROOM);
  }
  else{
    send_to_char("The field of chaos around you emits a blue aura then fades.\n\r", ch);
    act("The air around $n emits a blue aura then fades.", ch, NULL, NULL, TO_ROOM);
  }
  return TRUE;
}//END refshield INIT


/* discord_kill */
int discord_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_discord)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}



/* discord_tick */
int discord_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int upkeep = effect_table[GN_POS(gn)].value2[0];

  int to_prot = effect_table[GN_POS(gn)].value3[0];
  int to_dam = effect_table[GN_POS(gn)].value3[0];


  //ez
  if (gn != gen_discord)
    return -1;

  if (ch->mana < upkeep)
    {
      send_to_char("With your powers drained the cloak of discord fades.\n\r", ch);
      affect_strip(ch, gn);
      return FALSE;
    }
  else
    ch->mana -= upkeep;

  /* switch the protection/damage (TRUE = Protect)*/

  if (paf->modifier && number_percent() < to_dam){
    send_to_char("The field of chaos around you pulsates angry red then fades.\n\r", ch);
    act("The air around $n pulsates angry red then fades.", ch, NULL, NULL, TO_ROOM);
    paf->modifier = FALSE;
  }
  else if (!paf->modifier && number_percent() < to_prot){
    send_to_char("The field of chaos around you emits a blue aura then fades.\n\r", ch);
    act("The air around $n emits a blue aura then fades.", ch, NULL, NULL, TO_ROOM);
    paf->modifier = TRUE;
  }

  return TRUE;
}




//----===Chaos Orb===-----//
/* orb_init */
int orb_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_orb)
    return -1;

  send_to_char("You sense a fraction of Moloch's power focus upon your actions.\n\r", ch);
  act("You sense a malvolent intelligence focus upon $n.", ch, NULL, NULL, TO_ROOM);

  broadcast(ch, "A spectre of a giant fiery eye fades in and out overhead.\n\r");

  return TRUE;
}//END refshield INIT


/* orb_kill */
int orb_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_orb)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}



/* orb_attack */
int orb_precombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int max_charge = effect_table[GN_POS(gn)].value2[0];
  int per_lvl = effect_table[GN_POS(gn)].value3[0];
  int old_lvl = paf->modifier / per_lvl;

  if (gn != gen_orb)
    return -1;

  /* check if in combat with pc */
  if (!ch->fighting || IS_NPC(ch->fighting)
      || is_same_cabal(ch->pCabal, ch->fighting->pCabal))
    return FALSE;

  /* if so we bump up the count once */
  if (paf->modifier < max_charge)
    paf->modifier += 3;

  if (paf->modifier / per_lvl > old_lvl)
    send_to_char("The Eye of Moloch has risen in power.\n\r", ch);

  return TRUE;
}



//----===Conceal===-----//
/* Conceal_init */
int conceal_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_conceal)
    return -1;

  act("Dark shadows form into a thick cloak around you hiding your identity.", ch, NULL, NULL,  TO_CHAR);
  if (!IS_AFFECTED(ch, AFF_HIDE)
      && !IS_AFFECTED2(ch, AFF_CAMOUFLAGE)
      && !is_affected(ch, gsn_death_shroud))
    act("A thick dark cloak forms around $n and conceals $s identity.", ch, NULL, NULL, TO_ROOM);

  /* copy the new desc. over */
  free_string( ch->short_descr );
  ch->short_descr = str_dup ( effect_table[GN_POS(gn)].noun_damage);

  return TRUE;
}//END refshield INIT


/* conceal_kill */
int conceal_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_conceal)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  if (!IS_AFFECTED(ch, AFF_HIDE)
      && !IS_AFFECTED2(ch, AFF_CAMOUFLAGE)
      && !is_affected(ch, gsn_death_shroud))
    act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  if (!str_cmp(effect_table[GN_POS(gn)].noun_damage, ch->short_descr)){
    free_string( ch->short_descr );
    ch->short_descr = str_dup ( "" );
  }
  return TRUE;
}



/* conceal_combat */
int conceal_precombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  int chance = effect_table[GN_POS(gn)].value2[0] * 10;

  if (gn != gen_conceal)
    return -1;
  if (IS_ELDER(ch))
    chance = 0;
  if (number_range(0, 1000) < chance){
    act("You feel your concealment slip from stress of combat!", ch, NULL, NULL, TO_CHAR);
    paf->duration = 0;
  }
  return TRUE;
}



//----===Death SHroud===-----//
/* Shroud_init */
int d_shroud_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  CHAR_DATA* vch;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_d_shroud)
    return -1;
  send_to_char("The world turns bleak grey as you enter the lands of the dead.\n\r", ch);
  for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
    if (IS_AFFECTED(vch, AFF_CHARM)
	&& vch != ch
	&& vch->master
	&& vch->master == ch)
      affect_to_char(vch, paf);
  }
  return TRUE;
}//END


/* d_shroud_kill */
int d_shroud_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  CHAR_DATA* vch;

  if (gn != gen_d_shroud)
    return -1;
/* nuke shroud from anyone else that is our charmie */
  for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
    if (IS_AFFECTED(vch, AFF_CHARM)
	&& vch != ch
	&& vch->master
	&& vch->master == ch)
      affect_strip(vch, gn);
  }
  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}

int d_shroud_precommand(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  /*returns true to cancel command */
  int cmd = (int) *int1;

  /* leaders can walk around */
/* cmd == 0 is passed from the check in act_move
   to make sure you cannot follow without running this check
*/
  if ( cmd == 0
       || cmd_table[cmd].hide
       || !strcmp("east", cmd_table[cmd].name)
       || !strcmp("west", cmd_table[cmd].name)
       || !strcmp("north", cmd_table[cmd].name)
       || !strcmp("south", cmd_table[cmd].name)
       || !strcmp("up", cmd_table[cmd].name)
       || !strcmp("down", cmd_table[cmd].name)
       ){
    CHAR_DATA* vch = ch;
    if (IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM) && ch->master)
      vch = ch->master;
    if (!IS_LEADER(vch))
      affect_strip(ch, gn);
    else if (--paf->duration < 1)
      affect_strip(ch, gn);
  }

return FALSE;
}

//-----==CHAOS SPAWN====----//


/* cspawn_init */
int cspawn_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  const int ini_dur = effect_table[GN_POS(gn)].value3[0];

  if (gn != gen_cspawn)
    return -1;

  send_to_char("You mold the raw essence of entropy into a Spawn of Chaos!\n\r", ch);
  act("The raw forces of disorder come to $n's bidding to form a Chaos Spawn!", ch, NULL, NULL, TO_ROOM);

  paf->duration = ini_dur;
  return TRUE;
}


/* cspawn_kill */
int cspawn_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_cspawn)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}

/* cspawn_tick */
int cspawn_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int chance = 80;
  const int upkeep = number_range(5,12);

  /*
if in combat duration is refunded (+1)
propabilty of extra incerase:
chance     of +1(PC *2 chance)
chance / 2 of +2
chance / 4 of +3
  */

  if (paf->type != gen_cspawn)
    return -1;

  if (ch->fighting){
    paf->duration++;
    if (number_percent() < IS_NPC(ch->fighting)? chance : 2 * chance){
      paf->duration++;
      if (number_percent() < chance){
	paf->duration++;
	if (number_percent() < chance)
	  paf->duration++;
      }//End +2
    }//End +1
  }//end if in combat

  /* set ceiling */
  if (paf->duration > 24)
    paf->duration = 24;

  //Upkeep.
  if (ch->mana < upkeep)
    {
      sendf(ch, "You no longer have the power to control the %s.\n\r", effect_table[GN_POS(gn)].noun_damage);
      affect_strip(ch, gen_cspawn);
    }
  else
    ch->mana -= upkeep;
  return TRUE;
}

/* cspawnprevio */
int cspawn_previo(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* victim = (CHAR_DATA* ) arg2;

  /* all we do is show message for combat */
  if (paf->type != gen_cspawn)
    return -1;

  act("\n\rForces of chaos begin to tear into $N.\n\r", victim, NULL, victim, TO_ROOM);
  act("\n\rForces of chaos begin to tear into you.\n\r", ch, NULL, victim, TO_VICT);
  return TRUE;
}

/* special attacks of chaos spawn */
void cspawn_special(CHAR_DATA* ch, CHAR_DATA* victim, int spec, int level)
{
  AFFECT_DATA af;
  OBJ_DATA* wield;
  CHAR_DATA* mob;

  char* name = effect_table[GN_POS(gen_cspawn)].noun_damage;
  int i = 0, j = 0;

  //check which attack.
  switch (spec)
    {
/* Corrupt */
    case 1:
      act("$t engulfs $N in raw chaos!", victim, name, victim, TO_ROOM);
      act("$t engulfs you in raw chaos!", ch, name, victim, TO_VICT);
      af.type = gsn_corrupt;
      af.level = level;
      af.duration = 0;

      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location	= APPLY_MANA;
      af.modifier = -20;
      if (!is_affected(victim, gsn_corrupt)){
	act("Your insides twist as you feel $t's tain upon you.",ch,name,victim,TO_VICT);
	affect_to_char(victim,&af);
      }
      case 2:
/* DISSOLVE */
      if ( (wield = get_eq_char( victim, WEAR_WIELD )) == NULL)
	if ( (wield = get_eq_char( victim, WEAR_SECONDARY )) == NULL )
	break;
      act("$t engulfs $N's weapon!", victim, name, victim, TO_ROOM);
      act("$t engulfs your weapon!", victim, name, victim, TO_CHAR);

      if ( IS_OBJ_STAT(wield,ITEM_BURN_PROOF)
	   || IS_SET(wield->wear_flags, ITEM_HAS_OWNER)
	   || number_percent() < 50)
	{
	  act("$p resists chaos.",victim,wield,victim,TO_ALL);
	  break;
	}
      if (number_percent() < 20){
	act( "$p dissolves into a smoking a pulp.", victim, wield, victim, TO_ALL);
	extract_obj(wield);
      }
      else{
	act( "$p seems to corrode quickly .", victim, wield, victim, TO_ALL);
	if (wield->value[1] > 1)
	  wield->value[1]  = UMAX(1, wield->value[1] - 1);
	else
	  wield->value[2]  = UMAX(1, wield->value[2] - 1);
      }
      break;

/* SUMMON*/
    case 3:
      act("\n\rPassage to the Nexus opens around $t.", ch, name, victim, TO_ALL);

      /* only max of 2 demons at a time. */
      if (get_summoned(ch, MOB_VNUM_SHADOWDEMON) > 1){
	act("Yet nothing comes out..", ch, NULL, NULL, TO_ALL);
	break;
      }

      for (j=0; j < 1; j++){
	mob = create_mobile( get_mob_index(MOB_VNUM_SHADOWDEMON) );
	for (i = 0; i < MAX_STATS; i++)
	  mob->perm_stat[i] = UMIN(25, (ch->level / 10) + 15);
	mob->max_hit = 4 * ch->level;
	mob->hit = mob->max_hit;
	mob->level = ch->level;
	for (i = 0; i < 3; i++)
	  mob->armor[i] = interpolate(mob->level/2,100,-100);
	mob->armor[3] = interpolate(mob->level/2,100,0);
	mob->damroll    = (ch->level /4);
	mob->damage[DICE_NUMBER]  = (ch->level /12);
	mob->damage[DICE_TYPE]    = 3;
	mob->sex = number_range(1,2);
	mob->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
	mob->summoner = ch;
	af.where     = TO_AFFECTS;
	af.type      = gsn_timer;
	af.level     = level;
	af.duration  = number_range(0, 1);
	af.modifier  = 0;
	af.location  = 0;
	af.bitvector = 0;
	affect_to_char( mob, &af );
	mob->summoner = ch;
	mob->master = ch;
	SET_BIT(mob->affected_by, AFF_CHARM);
	char_to_room(mob,victim->in_room);
	act("$n appears and roars at $N!", mob, NULL, victim, TO_ROOM);
	set_fighting(mob,victim);
      }//end FOR
      break;
    }//END SWITCH
  return;
}


/* cspawn_precombat */
int cspawn_precombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* victim = (CHAR_DATA*) arg2;

  /* notice blades dont have tahc0 calc. they hit, or they are blocked */

//data
  int dam_1 = 4;
  int dam_2 = 3;
  int dam = 0;
  int h_roll = effect_table[GN_POS(gn)].value1[URANGE(0, paf->level/5, 10)];
  int d_roll = effect_table[GN_POS(gn)].value1[URANGE(0, paf->level/5, 10)];
  int roll = number_percent();

//consts
  const int chance = effect_table[GN_POS(gn)].value2[0];
  const int level_med = 30;
  const int level_mod = 5;

  const int h_roll_mod = 0; //multiplier for skill bonus to h_roll

  const int dt1 = attack_lookup("corruption") + TYPE_HIT;
  const int dt2 = attack_lookup("tainted") + TYPE_HIT;

  const int dam_type = DAM_NEGATIVE;

  /*
     The hit is based on values from value1 (increases with level)
     and skill
     Damage on the level of the spell and
     the skill of the user in the spell
  */

  //get the damage bonus here.
  if (paf->level > 49)
    dam_1 ++;
  dam_2 += UMAX(0, paf->level - level_med) / level_mod;
  if (paf->level > 49)
    dam_2++;
  dam = dice (dam_1, dam_2) + d_roll;

  //bonus on hit for skill.
  h_roll += h_roll_mod * UMAX(0, dam_1- 2);

  // sendf(ch, "d_roll: %d, dam: %d, h_roll: %d\n\r", d_roll, dam, h_roll);
  //two attack
 if (ch->fighting != NULL
     && victim->in_room == ch->in_room
     && victim->hit > 0
     && victim->position > POS_STUNNED
     && !is_safe_quiet(ch, victim))
   virtual_damage(ch, victim, NULL, dam, dt1, dam_type, h_roll, paf->level, TRUE, TRUE, gen_cspawn);
  dam = dice (dam_1, dam_2) + d_roll;
  if (ch->fighting != NULL
      && victim->hit > 0
      && victim->position > POS_STUNNED
      && victim->in_room == ch->in_room
      && !is_safe_quiet(ch, victim))
    virtual_damage(ch, victim, NULL, dam, dt2, dam_type, h_roll, paf->level, TRUE, TRUE, gen_cspawn);

  //special attacks.


  if ( number_percent() < chance
       &&  ch->fighting != NULL
       && victim->hit > 0
       && victim->in_room == ch->in_room
       && victim->position > POS_STUNNED
       && !is_safe_quiet(ch, victim) ){
    int special = 0;
    /* Dissolve*/
    if (number_percent() < 2)
      special = 2;
    /* summon */
    else if (roll < 65)
      special = 3;
    /* Corrupt */
    else
      special = 1;
    //DEBUG
    //    sendf(ch, "roll: %d, special %d\n\r", roll, special);
    cspawn_special(ch, victim, special, paf->level);
      }
 return TRUE;
}

//====--VISITATION===---//
/* visit_init */
int visit_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;


  if (gn != gen_visit)
    return -1;

  send_to_char("You sense something evil and powerful has noticed you.\n\r", ch);
  return TRUE;
}


/* visit_kill */
int visit_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_visit)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}


/* visit_tick */
int visit_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_visit)
    return -1;

  if (paf->duration == 1)
    paf->modifier = 1;

  if (paf->duration > 0){
    switch (number_range(0, 3)){
    case 0:
      send_to_char("You feel something awful heading your way.\n\r", ch);
      break;
    case 1:
      send_to_char("The hair on your neck rises in sensation of terrible danger.\n\r", ch);
      break;
    case 2:
      send_to_char("A feeling of terrible danger overcomes you.\n\r", ch);
      break;
    case 3:
      send_to_char("Your stomach clenches in feeling of great danger approaching.\n\r", ch);
      break;
    }
  }
  return TRUE;
}

/* visit_viol */
int visit_viol(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* och;
  int dam = 1;
  const int skip = effect_table[GN_POS(gn)].value2[0];

  if (gn != gen_visit)
    return -1;
  /* this runs only if paf modifier is > 0 */

  if (paf->modifier < 1
      || !paf->has_string)
    return FALSE;

  /* only if originator is around */
  if ( (och = get_char(paf->string)) == NULL){
    affect_strip(ch, gn);
    return FALSE;
  }
  if (och != ch && is_safe(och, ch))
    return TRUE;

  switch (paf->modifier){
  case 1:
    send_to_char("A demonic force appears and attacks you!\n\r", ch);
    act("A demonic force attacks $n!", ch, NULL, NULL, TO_ROOM);
    dam = number_range(20, 65);
    damage(och, ch, dam, gn, DAM_INTERNAL, FALSE);
    /* maybe skip 2 */
    paf->modifier++;
    if (number_percent() < skip)
      paf->modifier++;
    break;
  case 2:
    send_to_char("The sadistic being seems to toy with you a bit more!\n\r", ch);
    act("With a sadistic smile the demon seems to toy with $n!", ch, NULL, NULL, TO_ROOM);
    dam = number_range(55, 75);
    damage(och, ch, dam, gn, DAM_INTERNAL, FALSE);
    /* maybe skip 3 */
    paf->modifier++;
    if (number_percent() < skip)
      paf->modifier++;
    break;
  case 3:
    send_to_char("In a final attack the demon consume you!\n\r", ch);
    act("In a final attack the demon consumes $n!", ch, NULL, NULL, TO_ROOM);
    dam = number_range(75, 110);
    damage(och, ch, dam, gn, DAM_INTERNAL, FALSE);
    act("The demonic force smiles.", ch, NULL, NULL, TO_ALL);
    affect_strip(ch, gn);
    break;
  default:
    affect_strip(ch, gn);
  }
  return TRUE;
}


//====--SAVANT WARD===---//
/* init */
int sav_ward_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;


  if (gn != gen_sav_ward)
    return -1;

  act("$t runes of magical power inscribe themselves about $n.",
      ch, paf->modifier? "Glowing" : "Dark", NULL, TO_ROOM);
  act("$t runes of magical power inscribe themselves about you.",
      ch, paf->modifier? "Glowing" : "Dark", NULL, TO_CHAR);
  return TRUE;
}


/* kill */
int sav_ward_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_sav_ward)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}


/* predamage */
int sav_ward_predamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //Attacker passed by ch
  //victim passed by arg1
  //paf passed by arg2
  //damage done by int1;
  //damtype by int2
  //dt by flag;

  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;
  int dam = (int) *int1;
  int dam_type = (int) *int2;
  int dt = flag;

  const int max_dam = effect_table[GN_POS(gn)].value2[0];
  const int fail_mod = effect_table[GN_POS(gn)].value3[0]; //divisor for weakening of the ward

  /*
- warding blocks only weapons (dt >= TYPE_HIT)
- depending on paf->modifier only normal (dam_type < 4 on FALSE)
- chance to dispel warding on any damage over current ceiling (max_dam)
  */

  int over = 0;	//damage over the current limit

  /* only if damage is weapon based */
  if (dt < TYPE_HIT)
    return FALSE;

  /* only if there is something to block */
  if (dam < 1 || ch == victim)
    return FALSE;

  /* check for damage based on type of warding */
  /* magic vs normal */
  if (paf->modifier && dam_type < 4)
    return FALSE;

  /* normal vs magic */
  else if (!paf->modifier && dam_type > 3)
    return FALSE;

  /* block damage */
  over = UMAX(0, dam - max_dam);

  /* check if we need to block anything */
  if (over){
    /* reduce damage being done */
    *int1 = max_dam;
    /* check if dispelled */
    send_to_char("Your warding absorbs part of the blow.\n\r", victim);
    if (number_percent() < over / fail_mod
	|| ch->mana < over / fail_mod)
      affect_strip(ch, gn);
    else
      ch->mana -= fail_mod;
    /* reduce duration by 2 */
    if (paf->duration - 2 < 1)
      affect_strip(ch, gn);
    else
      paf->duration -= 2;
    return TRUE;
  }
  return FALSE;
}



//====--SAVANT CATALYST===---//
/* init */
int sav_cata_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  const int max_hit = effect_table[GN_POS(gn)].value2[0];

  if (gn != gen_sav_cata)
    return -1;
    send_to_char("You tremble as your body is infused with arcane "\
		 "energy.\n\r",ch);
    act("A magical glow fills $n's flesh.", ch, NULL, NULL, TO_ROOM);
    if (ch->hit > max_hit)
      ch->hit = max_hit;
  return TRUE;
}


/* kill */
int sav_cata_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_sav_cata)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  ch->hit = UMAX(1, ch->hit - paf->modifier / 2);
  return TRUE;
}


/* sav_Cata_tick */
int sav_cata_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int max_hit = effect_table[GN_POS(gn)].value2[0];
  if (get_skill(ch, skill_lookup("catalyst")) > 100)
    max_hit += 50;
  if (ch->hit > max_hit){
    send_to_char("You sense the effects of catalyst limit your health.\n\r", ch);
    ch->hit = max_hit;
  }

  return TRUE;
}


//====--WARMASTER CHALLENGE===---//
/* init */
int challenge_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_challenge)
    return -1;
  return TRUE;
}


/* kill */
int challenge_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;
  CHAR_DATA* och;
  AFFECT_DATA* baf, *baf_next;

  if (gn != gen_challenge)
    return -1;

  /* we check if this is a full challenge, if it is, try to find the other challenger and strip them too */


  if ( (och = get_char(paf->string)) == NULL)
    return TRUE;
  for (baf = och->affected; baf; baf = baf_next){
    baf_next = baf->next;
    if (baf->type == gen_challenge
	&& !str_cmp(baf->string, ch->name))
      affect_remove(och, baf);
  }

  /* no messages on challenges waiting to be accepted/declined */
  if (paf->duration > 0)
    return TRUE;
  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}


/* challenge_tick */
int challenge_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  /* of path is set we countdown till we can attack */
  if (paf->modifier > 0){
    if (--paf->modifier < 1)
      send_to_char("`!You may now FIGHT!``", ch);
    return TRUE;
  }
/* auto accept for challenge (bitvector == TRUE)*/
  else if (paf->bitvector && paf->duration == 1 && paf->has_string){
    CHAR_DATA* och;
    AFFECT_DATA* baf, af;
    /* check for original challenger */
    if ( (och = get_char(paf->string)) == NULL){
      sendf(ch, "%s is no longer around to takeup your challenge.\n\r", paf->string);
      return FALSE;
    }
    /*  check if we have already challenged that person */
    for (baf = ch->affected; baf; baf = baf->next){
      if (baf->type != gen_challenge)
	continue;
      if (str_cmp(baf->string, och->name))
	continue;
      act_new("You have already challenged $N!", ch, NULL, och, TO_CHAR, POS_DEAD);
      /* remove the challange waiting to be acepted */
      affect_remove(ch, baf);
      return FALSE;
    }
    /* check if challnger is not already in challange with acceptor */
    act_new("`&You have accepted $N's challenge, $E may not initiate attack for 5 hours.``", ch, NULL, och, TO_CHAR, POS_DEAD);
    act_new("`&$N has accepted your challenge, you may not initiate attack for 5 hours.``", och, NULL, ch, TO_CHAR, POS_DEAD);
    /* we set this paf to infinite duration, make another on on original
       challanger, and set modifier to 5 so he cannot initiate combat. */
    paf->duration = -1;

    af.type = gen_challenge;
    af.level = ch->level;
    af.duration = -1;
    af.where = TO_NONE;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 5;
    paf = affect_to_char(och, &af);
    string_to_affect(paf, ch->name);
    return TRUE;
  }
  return TRUE;
}


//---Circle of Protection---//
/* init */
/*
int circle_of_protection_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;


  if (gn != gen_circle_of_protection)
    return -1;

  act("$n forms a $t circle of protection around $mself.",
      ch, paf->modifier? "red" : "blue" : "green" : "white" : "black",NULL, TO_ROOM);
  act("You form a $t circle of protection around yourself.",
      ch, paf->modifier? "red" : "blue" : "green" : "white" : "black",NULL, TO_CHAR);
  act("$n forms a $t circle of protection around $mself.",
      ch, paf->modifier? "red" : "blue" : "green" : "white" : "black",NULL, TO_ROOM);
  act("You form a $t circle of protection around yourself.",
      ch, paf->modifier? "red" : "blue" : "green" : "white" : "black",NULL, TO_CHAR);
  act("You form a $t circle of protection around yourself.",
      ch, paf->modifier? "red" : "blue" : "green" : "white" : "black",NULL, TO_CHAR);
  return TRUE;
}

*/
/* kill */
/*
int circle_of_protection_kill(CHAR_DATA* ch, void* arg1, void*
arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_circle_of_protection)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}
*/

/* predamage */
/*
int circle_of_protection_predamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //Attacker passed by ch
  //victim passed by arg1
  //paf passed by arg2
  //damage done by int1;
  //damtype by int2
  //dt by flag;

  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;
  int dam = (int) *int1;
  int dam_type = (int) *int2;
  int dt = flag;

  const int max_dam = effect_table[GN_POS(gn)].value2[0];
  const int fail_mod = effect_table[GN_POS(gn)].value3[0]; //divisor for weakening of the circle
*/
  /*
- warding blocks only weapons (dt >= TYPE_HIT)
- depending on paf->modifier only normal (dam_type < 4 on FALSE)
- chance to dispel warding on any damage over current ceiling (max_dam)
  */
/*
  int over = 0; //damage over the current limit
*/
  /* only if damage is weapon based */
/*
  if (dt < TYPE_HIT)
    return FALSE;
*/
  /* only if there is something to block */
/*
  if (dam < 1 || ch == victim)
    return FALSE;
*/
  /* check for damage based on type of warding */
  /* magic vs normal */
/*
  if (paf->modifier && dam_type < 4)
    return FALSE;
*/
  /* normal vs magic */
/*
    else if (!paf->modifier && dam_type > 3)
    return FALSE;
*/
  /* block damage */
/*
  over = UMAX(0, dam - max_dam);
*/
  /* check if we need to block anything */
/*  if (over){*/
    /* reduce damage being done */
/*    *int1 = max_dam;*/
    /* check if dispelled */
/*    if (number_percent() < over / fail_mod
        || ch->mana < over / fail_mod)
      affect_strip(ch, gn);
    else
      ch->mana -= fail_mod;*/
    /* reduce duration by 10 */
/*
    if (paf->duration - 10 < 1)
      affect_strip(ch, gn);
    else
      paf->duration -= 10;
    return TRUE;
  }
  return FALSE;
}
*/

// YEARNING - like hunger, but hits only after out of forest for a while
// druids auto-get this.  - ATH

int yearning_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
 *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  AFFECT_DATA* circ;
  bool fGreen = FALSE;
  if (gn != gen_yearning)
    return -1;

/* if room char is in is non-forest */
  if (ch->in_room->sector_type != SECT_FOREST
      && ch->in_room->vnum != ROOM_VNUM_LIMBO
      && !is_ghost(ch, 600)
      && !IS_IMMORTAL(ch)) {

    //try to find green circle of prot
    if ( (circ = affect_find( ch->affected, skill_lookup("circle of protection"))) != NULL
	 && circ->bitvector == 0)
      fGreen = TRUE;

    // if this is first non-forest room, send message
    /* not in jail */
    if (paf->modifier == 0
	&& is_pk(ch, ch)) {
      send_to_char ("As you leave the warm embrace of nature, your mind"\
		    " begins to yearn.\n\r", ch);
    }
    /* non-newbies only */
    if (IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL))
      return TRUE;
    // increase tick counter for out of forest

    // green circle has a chance to negate it
    if (!fGreen || number_percent() < 50)
      paf->modifier++;

    // if this is an even-numbered tick out of forest, send message
    if ((paf->modifier > 1) && (paf->modifier <=10) &&
	(paf->modifier % 2 == 0)) {
      send_to_char ("Your yearning for the warm embrace of the forest "\
                    "increases.\n\r", ch);
    }

    // if we've been out over 10 ticks, damage (increasing longer out)
    // also only if over lev 10
    if ((paf->modifier > 10) && (ch->level > 10)) {
      int dam = number_range(1+ (2*(paf->modifier-10)),ch->level/2 + (5*(paf->modifier-10)));

      //green circle
      if (fGreen)
	dam = UMIN(1, dam / 3 );
      damage( ch, ch, dam, gn, DAM_INTERNAL,TRUE);
    }
    return TRUE;
  }
/* else, we are in forest */
  else {
    // if this is first tick in forest, send message
    if (paf->modifier > 0) {
      send_to_char ("As you return to the warm embrace of nature, your"\
		    " yearning is satisfied.\n\r", ch);
    }

    // reset tick counter
    paf->modifier = 0;
  }
  return TRUE;
}

//ENSNARE

int ensnare_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag )
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* victim = NULL;

  if (gn != gen_ensnare)
    return -1;

  /* if the original creator of this ensnare is not in an area with us we strip it */
  if (IS_NULLSTR(paf->string) || (victim = get_char(paf->string)) == NULL
      || ch->in_room == NULL
      || victim->in_room == NULL
      || victim->in_room->area != ch->in_room->area)
    affect_strip( ch, gn );
  return TRUE;
}

int ensnare_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  char buf[MIL];
  if (gn != gen_ensnare)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  sprintf(buf, "%s is no longer ensnared.", PERS2(ch));
  cabal_echo_flag(CABAL_HEARGUARD, buf );
  return TRUE;
}


//MOB GENERATOR: Generaters a mob of vnum stored in "level" in area stored in "location"
//at a rate of "modifier / duration" for "duration" ticks

int mob_gen_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  AREA_DATA* pArea;
  CHAR_DATA* vch;
  MOB_INDEX_DATA* pMobIndex;
  int count = 0;

  if (gn != gen_mob_gen)
    return -1;

  /* try to get the area first */
  if ( (pArea = get_area_data( paf->location )) == NULL){
    bug("mob_gen_tick: could not get area %d.", paf->location );
    return FALSE;
  }
  /* try to get the mob index*/
  else if ( (pMobIndex = get_mob_index( paf->level )) == NULL){
    bug("mob_gen_tick: could not get mobindex %d.", paf->level );
    return FALSE;
  }
  else if (pMobIndex->count >= paf->modifier){
  /* check how many mobs have been summoned into this area */
    for (vch = char_list; vch; vch = vch->next ){
      if (IS_NPC(vch) && vch->pIndexData == pMobIndex && vch->in_room && vch->in_room->area == pArea)
	count++;
      if (count > paf->modifier){
	paf->duration ++;
	return FALSE;
      }
    }
  }
  else{
    /* at this point we know we can spawn a mob */
    ROOM_INDEX_DATA* pRoomIndex;
    int e_r1[] = {ROOM_NO_TELEPORTIN, ROOM_NO_INOUT};
    int spawn = 0;
    count = 0;

    for (spawn = 0; spawn < paf->bitvector; spawn ++ ){
      pRoomIndex =  get_rand_room(pArea->vnum,pArea->vnum,		//area range (0 to ignore)
				  0,0,			//room ramge (0 to ignore)
				  NULL,0,		//areas to choose from
				  NULL,0,		//areas to exclude
				  NULL,0,		//sectors required
				  NULL,0,		//sectors to exlude
				  NULL,0,		//room1 flags required
				  e_r1,2,		//room1 flags to exclude
				  NULL,0,		//room2 flags required
				  NULL,0,		//room2 flags to exclude
				  1,			//number of seed areas
				  TRUE,			//exit required?
				  FALSE,		//Safe?
				  ch);			//Character for room checks
      if (pRoomIndex == NULL){
	bug("gen_mob_tick: Could not get a random room in area %d.", pArea->vnum );
	continue;
      }
      /* create the mob and move it to the room */
      vch = create_mobile( pMobIndex );
      if (pRoomIndex->people)
	act("$N enters the room.", pRoomIndex->people, NULL, vch, TO_ALL );
      char_to_room( vch, pRoomIndex );
      count ++;
    }
    if (count)
      sendf( ch, "You've caused %d %s%s to enter %s.\n\r", count, pMobIndex->short_descr, count == 1 ? "" : "s", pArea->name );
  }
  return TRUE;
}

/* ///---- JAIL----/// */

bool execute_criminal( CHAR_DATA* ch, int type, char* name){
  CHAR_DATA* mob = NULL, *vch;
  ROOM_INDEX_DATA* exe_room;

  /* get the jail this guy is from based on current room. */
  int jail = jail_cell_lookup(ch->in_room->vnum);

  if (jail_table[jail].exe_mob == 0){
    char buf[MIL];
    send_to_char("No exe_mob found! Contact Immortal.\n\r", ch);
    sprintf(buf, "jail_kill: No exe_mob found for jail %d.", jail);
    bug(buf, 0);
    return FALSE;
  }
  /* get the room for execution */
  if ( (exe_room = get_room_index(jail_table[jail].exe_room)) == NULL){
    char buf[MIL];
    send_to_char("No exe_room found! Contact Immortal.\n\r", ch);
    sprintf(buf, "jail_kill: exe_room %d not found.", jail_table[jail].exe_room);
    bug(buf, 0);
    return FALSE;
  }
  /* Make sure there is the exe_mob in the room if not load one */
  for (vch = exe_room->people; vch; vch = vch->next_in_room){
    if (!IS_NPC(vch))
      continue;
    if (vch->pIndexData->vnum == jail_table[jail].exe_mob){
      mob = vch;
      break;
    }
  }
  /* if not there, create one */
  if (!mob){
    if ((mob = create_mobile( get_mob_index(jail_table[jail].exe_mob))) == NULL){
      char buf[MIL];
      send_to_char("No exe_mob found! Contact Immortal.\n\r", ch);
      sprintf(buf, "jail_kill: exe_mob %d could not be loaded.", jail_table[jail].exe_mob);
      bug(buf, 0);
      return FALSE;
    }
    char_to_room(mob, exe_room);
  }
  /* have the mob */
  jail_execute(ch, mob, type, name);
  return TRUE;
}

int jail_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_jail)
    return -1;

  if (ch->in_room == NULL || !IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL))
    paf->modifier = JAIL_NONE;

  if (paf->modifier == JAIL_RELEASE)
    unjail_char(ch);
  else if (paf->modifier == JAIL_EXECUTE && paf->duration == 0){
    return TRUE;
  }
  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR, POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}
/* safely clears the jail info if not in cell */
int jail_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int exe_chance = effect_table[GN_POS(gn)].value2[0];
  if (gn != gen_jail)
    return -1;
  if (!ch->in_room)
    return FALSE;
  /* can't drop link */
  if (!ch->desc
      && paf->modifier != JAIL_NONE
      && paf->modifier != JAIL_EXECUTE
      && paf->modifier != JAIL_TRANSFER
      && paf->modifier != JAIL_RELEASE){
    paf->duration++;
    if (number_percent() < exe_chance)
      set_sentence(ch, JAIL_EXECUTE, 1);
    return FALSE;
  }
  if (!IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL)){
    paf->modifier = JAIL_NONE;
    affect_strip(ch, gn);
    return TRUE;
  }
  /* Does various things depending on paf->modifier at 0*/
  if (paf->duration == 0){
    if (paf->modifier == JAIL_RELEASE
	|| paf->modifier == JAIL_NORMAL
	|| paf->modifier == JAIL_TRANSFER){
      unjail_char(ch);
    }
    else if (paf->modifier == JAIL_EXTENDED){
      unjail_char(ch);
    }
    else if (paf->modifier == JAIL_BAIL){
      /* Bail run out with out ch. bailing out, goto extended.
	 set_jail will take care of time calc.
      */
      act("You have failed to pay the bail in given time.  Remaining sentence will be carried out as normal.", ch, NULL, NULL, TO_CHAR);
      set_sentence(ch, JAIL_EXTENDED, paf->level);
      return TRUE;
    }
    else if (paf->modifier == JAIL_EXECUTE){
      execute_criminal( ch, paf->level, paf->has_string ? paf->string : "Justice");
      return TRUE;
    }
  }
  /* For JAIL_EXECUTION we make executor come over 1 tick before the deed */
  else if (paf->modifier == JAIL_EXECUTE && paf->duration == 1 ){
    CHAR_DATA* mob = NULL, *vch;
    ROOM_INDEX_DATA* exe_room;

    /* get the jail this guy is from based on current room. */
    int jail = jail_cell_lookup(ch->in_room->vnum);

    if (jail_table[jail].exe_mob == 0){
      char buf[MIL];
      send_to_char("No exe_mob found! Contact Immortal.\n\r", ch);
      sprintf(buf, "jail_kill: No exe_mob found for jail %d.", jail);
      bug(buf, 0);
      return FALSE;
    }
    /* get the room for execution */
    if ( (exe_room = get_room_index(jail_table[jail].exe_room)) == NULL){
      char buf[MIL];
      send_to_char("No exe_room found! Contact Immortal.\n\r", ch);
      sprintf(buf, "jail_kill: exe_room %d not found.", jail_table[jail].exe_room);
      bug(buf, 0);
      return FALSE;
    }
    /* Make sure there is the exe_mob in the room if not load one */
    for (vch = exe_room->people; vch; vch = vch->next_in_room){
      if (!IS_NPC(vch))
	continue;
      if (vch->pIndexData->vnum == jail_table[jail].exe_mob){
	mob = vch;
	break;
      }
    }
    /* if not there, create one */
    if (!mob){
      if ((mob = create_mobile( get_mob_index(jail_table[jail].exe_mob))) == NULL){
	char buf[MIL];
	send_to_char("No exe_mob found! Contact Immortal.\n\r", ch);
	sprintf(buf, "jail_kill: exe_mob %d could not be loaded.", jail_table[jail].exe_mob);
	bug(buf, 0);
	return FALSE;
      }
      char_to_room(mob, exe_room);
    }

    /* show message about arrival */
    act(execution_table[paf->level].pre_act, mob, NULL, ch, TO_ROOM);
    return TRUE;
  }
  /* we announce executions every 3 ticks */
  else if (paf->modifier == JAIL_EXECUTE && paf->duration > 1
	   && !(paf->duration % 3)){
    ROOM_INDEX_DATA* room;
    AFFECT_DATA* crime;
    int crimes = 0;
    int jail = jail_cell_lookup(ch->in_room->vnum);
    char crime_str[MIL];
    char buf[MIL];
    if ( (room = get_room_index(jail_table[jail].exe_room)) == NULL){
      sprintf(buf, "jail_tick: exe_room %d not found.", jail_table[jail].exe_room);
      bug(buf, 0);
      return FALSE;
    }
/* we make a bitvector of all the cimes the person has commited */
    for (crime = ch->affected; crime != NULL; crime = crime->next){
      if (crime->type == gsn_wanted){
	crimes |= crime->bitvector;
      }
    }
    sprintf( crime_str, "For the crimes of " );
    if (IS_SET(crimes, CRIME_FLAG_S)){
      strcat( crime_str, "Not sheathing" );
      strcat( crime_str, ", ");
    }
    if (IS_SET(crimes, CRIME_FLAG_L)){
      strcat( crime_str, crime_table[CRIME_LOOT].name );
      strcat( crime_str, ", ");
    }
    if (IS_SET(crimes, CRIME_FLAG_T)){
      strcat( crime_str, crime_table[CRIME_THEFT].name );
      strcat( crime_str, ", ");
    }
    if (IS_SET(crimes, CRIME_FLAG_A)){
      strcat( crime_str, crime_table[CRIME_ASSAULT].name );
      strcat( crime_str, ", ");
    }
    if (IS_SET(crimes, CRIME_FLAG_M)){
      strcat( crime_str, crime_table[CRIME_MURDER].name );
      strcat( crime_str, ", ");
    }

    sprintf(buf, "%sby the Law of Aabahran\n\r"\
	    "%s has been sentenced by %s %s\n\rCome ye to %s `6to witness the deed in %d hours!``",
	    crime_str,
	    PERS2(ch), paf->has_string ? paf->string : "Justice",
	    execution_table[paf->level].announce,
	    room->name, paf->duration);
    jail_announce(jail, buf);
    return TRUE;
  }
  return TRUE;
}

/* Watchtower INIT */
int watch_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  if (gn != gen_watchtower)
    return -1;

  act("On your order the guards quickly construct a watchtower.", ch, NULL, NULL, TO_CHAR );
  act("On $n's order the guards quickly construct a watchtower.", ch, NULL, NULL, TO_ROOM );
  return TRUE;
}

int watch_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_watchtower)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}

int watch_leave(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  // AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_watchtower)
    return -1;

  affect_strip(ch, gen_watchtower);
  return TRUE;
}

/*  --------====BATTER=========----- */
/* kill */
int batter_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_batter)
    return -1;
  if (ch->fighting){
    act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
    act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  }

  return TRUE;
}

int batter_predamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //Attacker passed by ch
  //victim passed by arg1
  //paf passed by arg2
  //damage done by int1;
  //damtype by int2
  //dt by flag;

//  CHAR_DATA* victim = (CHAR_DATA*) arg1;
//  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;
  int dam = (int) *int1;
  int dam_type = (int) *int2;
  int dt = flag;

/* all we do is cut the damage down to 2/3 if not blunt while battering */
/* must also be weapon damage */
  if (dt < TYPE_HIT)
    return FALSE;
  if (dam_type != DAM_BASH)
    dam = 2 * dam / 3;
/* then damage is further cut down based on skill */
  dam = get_skill(ch, gsn_batter) * dam / 100;
  *int1 = dam;
  check_improve(ch, gsn_batter, TRUE, 1);
  return TRUE;
}


int batter_postdamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /* run after the weapon causes damage and has gen_batter */
  //Attacker passed by ch
  //victim passed by arg1
  //obj passed by arg2
  //damage done by int1;
  //dam type by int2;
  //dt by flag;


  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  AFFECT_DATA *paf = ( AFFECT_DATA*) arg2;
  AFFECT_DATA *bat;

//data
  int dam = (int) *int1;
//  int dam_type = (int) *int2;
  int dt = flag;

/* only on weapon  damage */
   if (dt < TYPE_HIT)
     return FALSE;
/* this is all after damage has been dealt, so decrease is done in predamage */
/* all we do here is add it up */
  if ( (bat = affect_find(victim->affected, gsn_batter)) == NULL){
    AFFECT_DATA af;

    af.type = gsn_batter;
    af.level = paf->level;
    af.duration = 1;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = dam;
    affect_to_char(victim, &af);
  }
  else{
    bat->modifier += dam;
    bat->duration = UMAX(1, bat->duration);
  }
  check_improve(ch, gsn_batter, TRUE, 1);
  return TRUE;
}


int batter_precombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag){

  AFFECT_DATA* baf = (AFFECT_DATA*) arg2;
  AFFECT_DATA* paf;
  CHAR_DATA* victim = (CHAR_DATA*) arg1;

  const int threshold = victim->level * 4;

  if (gn != gen_batter)
    return -1;

  /* check if paf exists */
  if ( (paf = affect_find(victim->affected, gsn_batter)) == NULL)
    return FALSE;
/* now there is a chance to disarm  */
//    sendf(ch, "Current batter: %d, threashold: %d\n\r", paf->modifier, threshold);

/* check if we battered him enough */
  if (paf->modifier >= threshold){
    act("$N's eyes glaze over from the multiple concussions.",
	ch, NULL, victim, TO_CHAR);
    act("Shaken with the staggering blows, you loose grip on your weapon!",
	ch, NULL, victim, TO_VICT);
    disarm(ch, victim);
/* check if we battered enough to disarm sec. */
    if (number_percent() <  3 * (paf->modifier - threshold) / 2){
      if (get_eq_char(victim, WEAR_SECONDARY))
	  disarm_offhand(ch, victim);
      else if (get_eq_char(victim, WEAR_SHIELD))
	shield_disarm(ch, victim);
    }
    paf->modifier /= 4;
    /* chance for duration to run out */
    if (--baf->duration  < 0)
      affect_strip(ch, gn);
  }
  return TRUE;
}

/* This is what happens when you smoke too much. -Crypt */

int addiction_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_addiction)
    return -1;

  //if used drugs recently nothing happends
  if (is_affected(ch, gsn_drug_use)){
    paf->modifier = 0;
    return FALSE;
  }

  /* first timers */
  if (paf->modifier == 0) {
    send_to_char ("You feel overcome with drug withdrawals.\n\r", ch);
  }

  /* increase tick counter */
  paf->modifier++;

  /* if this is an even-numbered tick, send message */
  if ((paf->modifier > 1) && (paf->modifier <= 10) &&
      (paf->modifier % 2 == 0)) {
    send_to_char ("You shake and shudder profusely.\n\r", ch);
  }

  /* if we've been withdrawing over 10 ticks, damage */

  if (!IS_PERK(ch, PERK_ADDICT) && (paf->modifier > 10) && (ch->level > 10)){
    int dam = number_range( paf->modifier - 10, UMIN(50, paf->modifier));
    damage( ch, ch, dam, gn,DAM_INTERNAL,TRUE);
  }
  return TRUE;
}

//~~~~LEVITATION~~~~//
int levitation_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* orig_paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* vch;
  AFFECT_DATA* paf, af;

  if (gn != gen_levitation)
    return -1;

  /* we run through everyone grouped with the character and levitation them
     for 0 ticks
  */
  for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
    if (!is_same_group(vch, ch) || vch == ch)
      continue;
    else if ( vch->pCabal && IS_CABAL(get_parent(vch->pCabal), CABAL_NOMAGIC))
      continue;
    else if ( (paf = affect_find(vch->affected, gsn_levitate)) != NULL)
      paf->duration = 1;
    else if (IS_AFFECTED(vch, AFF_FLYING))
      continue;
    else{
      af.type = gsn_levitate;
      af.level = orig_paf->level;
      af.duration = 1;
      af.where	 = TO_AFFECTS;
      af.bitvector = AFF_FLYING;
      af.modifier  = APPLY_NONE;
      af.location  = 0;
      affect_to_char(vch, &af);
      act_new( "Your feet rise off the ground.",vch,NULL,NULL,TO_CHAR,POS_DEAD);
      act("$n's feet rise off the ground.",vch,NULL,NULL,TO_ROOM);
    }
  }
  return TRUE;
}//END

/* levitation_kill */
int levitation_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_levitation)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}

//--------------=============PHOTON WALL===========-----------
int photon_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_photon_wall)
    return -1;

  if (ch->fighting == NULL && !IS_AFFECTED(ch, AFF_INVISIBLE)){
    if (paf->modifier > 0){
      paf->modifier--;
      return TRUE;
    }
    spell_invis(gsn_invis, paf->level, ch, ch, 0);
    /* if we cannot invis for some reason, set a longer wait */
    if (!IS_AFFECTED(ch, AFF_INVISIBLE))
      paf->modifier = 10;
  }

  return TRUE;
}//END

/* research_kill */
int photon_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  if (gn != gen_photon_wall)
    return -1;
  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}

/* HOLY WEAPON */
/* adds experience on kill to holly weapon used in crusade/avenger */
void hwep_gain(CHAR_DATA* ch, CHAR_DATA* victim, int mult ){
  AFFECT_DATA* paf;
  OBJ_DATA* obj;

//data
  int gain = 1;

//const
  const int lvl38_gain = 2;
  const int lvl45_gain = 4;
  const int lvl50_gain = 6;

//bool

  //first we get weapons.
  if ( (obj = get_eq_char(ch,WEAR_WIELD)) == NULL)
    return;
  if (obj->pIndexData->vnum != OBJ_VNUM_WEAPONCRAFT)
    return;

  //get level increase.
/* special case for prayer gain when ch == victim */
  if (ch == victim)
    gain = 2;
  else if (victim->level > 49)
    gain = lvl50_gain;
  else if (victim->level > 44)
    gain = lvl45_gain;
  else if (victim->level > 37)
    gain = lvl38_gain;

  if (mult)
    gain = mult * gain / 100;

  /* check for paf */
  if ((paf = affect_find(obj->affected, gen_hwep)) == NULL){
    AFFECT_DATA af;
    af.type = gen_hwep;
    af.level = 0;
    af.duration = -1;
    af.where = TO_NONE;
    af.bitvector = 0;
    af.modifier = 0;
    af.location = APPLY_NONE;
    paf = affect_to_obj(obj, &af);
  }
  //now check for level
  paf->modifier += gain;
/*Viri: messages passed into hwep_none
  act("$p seems to grow warm for a moment.", ch, obj, NULL, TO_CHAR);
  act("$n's $p seems to grow warm for a moment.", ch, obj, NULL, TO_ROOM);
*/
  LivWep_level_check(obj, gen_hwep);
}

/* hwep_objtick */
/* linked into effect.c */
/* written by Viri */
int hwep_objtick(CHAR_DATA* not_used, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*
    All we do is check level on tick, as well as any other special abilities.
  */
//  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  OBJ_DATA* obj = (OBJ_DATA*) arg2;
  AFFECT_DATA* baf;

  LivWep_level_check(obj, gn);

  /* check for return */
  if ( (baf = affect_find(obj->affected, gsn_recall)) != NULL
       && baf->has_string && !IS_NULLSTR(baf->string)
       && number_percent() < 50){
    CHAR_DATA *ch;
    AFFECT_DATA *owner;

    /* check for owner setting in case it needs a fix */
    if ( (owner = affect_find(obj->affected, gen_has_owner)) != NULL
	 && owner->has_string && !IS_NULLSTR(owner->string))
      string_to_affect(baf, owner->string);

    /* try to find original owner first */
    if ( (ch = get_char(baf->string)) != NULL){
      /* check who we are carried by */
      CHAR_DATA* people = NULL;
      CHAR_DATA* vch = in_char( obj );
      OBJ_DATA* vobj = in_obj( obj );

      if (vch == NULL || vch != ch){
	/* try to get the pointer to where we can show message */
	if (vch && vch->in_room)
	  people = vch->in_room->people;
	else if (vobj && vobj->in_room)
	  people = vobj->in_room->people;
	else if (obj->in_room)
	  people = obj->in_room->people;
	else
	  people = NULL;
	if (people)
	  act("`&With a flash of light from the starstone $p disappears!``",
	      people, obj, NULL, TO_ALL);
	/* goto owner */
	if (obj->in_obj)
	  obj_from_obj( obj );
	else if (obj->carried_by)
	  obj_from_char( obj );
	else if (obj->in_room)
	  obj_from_room( obj );
	act("`&With a flash of light from the starstone $p appears!``",
	    ch, obj, NULL, TO_ALL);
	obj_to_char(obj, ch);
      }//END if not carried by owner
    }//END IF has recall owner
  }//END If recall
/* HEALING */
/*
   if modifier > 0 then we are in delay, decrease modifier by one till 0
   if modifier < 0  and less then -1 heal
   if modifier < 0 and -1 we were healing, set modifier to 12
   if modifier = 0 and hp < 20% then we can heal, set modifier to -1

*/

  if ( obj->wear_loc != WEAR_NONE
       && (baf = affect_find(obj->affected, gsn_heal)) != NULL){
    CHAR_DATA* vch = in_char ( obj );
    if (baf->modifier > 0)
      baf->modifier--;
    else if (baf->modifier < 0){
      if (baf->modifier < -1){
	act("$p glows softly in your hand.", vch, obj, NULL, TO_CHAR);
	act("$p glows softly in $n's hand.", vch, obj, NULL, TO_ROOM);
	baf->modifier ++;
	spell_cure_light(skill_lookup("cure light"), vch->level, vch, vch, 0);
      }
      else
	baf->modifier = 12;
    }
    else if (baf->modifier == 0 && vch && vch->hit < vch->max_hit / 4){
      baf->modifier = -5;
      spell_cure_light(skill_lookup("cure light"), vch->level, vch, vch, 0);
    }
  }
  return TRUE;
}

/* hwep_none */
/* linked into effect.c */
/* written by Viri */
int hwep_none(CHAR_DATA* not_used, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  /*
This is a linker function so the LivWep functions have a degree of flexibility and reusability.
The NONE update simply does the increase/decrease of the malform level based on the flag.
TRUE= INCREEASE.
  */

  OBJ_DATA* obj = (OBJ_DATA*) arg1;//object with hwep
  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;//paf with hwep
  CHAR_DATA* ch = in_char( obj );


  //we simply clear/set effects now.
  if (flag){ //INCREASE
    paf->level++;
    if (ch != NULL){
      act("$p seems to grow warm for a moment.", ch, obj, NULL, TO_CHAR);
      act("$n's $p seems to grow warm for a moment.", ch, obj, NULL, TO_ROOM);
    }
    if (ch != NULL && paf->level >= 6 && !IS_SET(paf->bitvector, WEP_NAMED)){
      send_to_char("`@Your weapon has reached a level of power "\
		   "that deserves a proper name.\n\r Use cmd: "\
		   " \"brandweapon\" to give your item a proper name.\n\r``", ch);
    }
  }
  else {
    paf->level--;
    if (ch != NULL){
      act("$p seems to grow colder for a moment.", ch, obj, NULL, TO_CHAR);
      act("$n's $p seems to grow colder for a moment.", ch, obj, NULL, TO_ROOM);
    }
  }
  return TRUE;
}//end NONE
//--------------============= CRUSADE ===========-----------
int crusade_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  DESCRIPTOR_DATA *d;

  char buf[MIL];
  char choice [MIL];

  if (gn != gen_crusade)
    return -1;

/* Cannot be on crusade grouped */
  stop_follower(ch);
  die_follower_pc(ch);

/* compose the name of the party to be crusaded against */
  sprintf(choice, "%s", show_crusade( paf ) );

/* we yell it out */
  sprintf(buf,"With the Might and Wisdom of %s upon me, I declare Crusade against %s!``", IS_NPC(ch) ? "The One God" : ch->pcdata->deity, choice);
  do_yell(ch, buf);

  /* and we create a global yell of sorts */
  sprintf(buf,"`6Hear ye! Hear ye! %s%s`6 has left on Crusade against %s!``",
	  ch->name,
	  IS_NPC(ch) ? "" : ch->pcdata->title,
	  choice);

  /* now we show it to the world */
  for ( d = descriptor_list; d; d = d->next ){
    if ( d->connected == CON_PLAYING
	 && d->character->in_room
	 && d->character->in_room->area != ch->in_room->area
	 && d->character != ch){
      act("A thin squire gallops by you yelling on top of his lungs:",
	  d->character, NULL, NULL, TO_CHAR);
      act(buf, d->character, NULL, NULL, TO_CHAR);
    }
  }
  return TRUE;
}//END

/* crusade_kill */
int crusade_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  DESCRIPTOR_DATA *d;

  char buf[MIL];
  char choice [MIL];

  if (gn != gen_crusade)
    return -1;

/* compose the name of the party to be crusaded against */
  sprintf(choice, "%s", show_crusade( paf ));

  /* we create a global yell of sorts */
  sprintf(buf,"`6Hear ye! Hear ye! %s%s`6 has ended the Crusade against %s!``",
	  ch->name,
	  IS_NPC(ch) ? "" : ch->pcdata->title,
	  choice);

  /* now we yell it out */
  for ( d = descriptor_list; d; d = d->next ){
    if ( d->connected == CON_PLAYING ){
      act("A thin squire gallops by you his face beet red from yelling:",
	  d->character, NULL, NULL, TO_CHAR);
      act(buf, d->character, NULL, NULL, TO_CHAR);
    }
  }

  act_new(effect_table[GN_POS(gn)].msg_off,ch, choice,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}

int crusade_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
 *int2, int flag)
{
//  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_crusade)
    return -1;

/* just a follower check followed by food check */
  stop_follower(ch);
  die_follower_pc(ch);

/* hunger check */
  if (IS_NPC(ch) || !IS_AWAKE(ch) || is_affected(ch, gsn_ecstacy)
      || ch->fighting)
    return TRUE;

  if ( !is_affected(ch, skill_lookup("dysentery"))
       && (ch->pcdata->condition[COND_HUNGER] < 10
	   || ch->pcdata->condition[COND_THIRST] < 10)){
    char* type[] = {"an elderly farmer", "a housewife", "a lanky youth",
		    "a traveler", "a soldier", "a beautiful lady",
		    "a courter", "a squire" };
    const int max = 7;
    int i = number_range(0, max);

    act("Suddenly $t approaches you, a small amount of food and drink in hand."
	, ch, type[i], NULL, TO_CHAR);
    act("Suddenly $t approaches $n, a small amount of food and drink in hand."
	, ch, type[i], NULL, TO_ROOM);
    act("$t says '`#The news of your Crusade has traveled far $n.``'",
	ch, type[i], NULL, TO_ALL);
    act("$t says '`#Take this food and water and be blessed on your way.``'\n\r",
	ch, type[i], NULL, TO_ALL);
    act("You quickly eat, and resume your travels.", ch, NULL, NULL, TO_CHAR);
    act("$n quickly eats, and resumes $s travels.", ch, NULL, NULL, TO_ROOM);

    ch->pcdata->condition[COND_HUNGER] = 60;
    ch->pcdata->condition[COND_THIRST] = 60;
  }
  return TRUE;
}

int crusade_postkill(CHAR_DATA* cha, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;
  DESCRIPTOR_DATA* d;

/* just showing some messages */
  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  CHAR_DATA* ch = cha;
  int gain = number_range(1000, 4000);
  int cpgain = number_range(10, 30);
  int mult = 0;

  char buf[MIL];
  char choice[MIL];

  /* charmie switch */
  if (IS_NPC(cha) && IS_AFFECTED(cha, AFF_CHARM)
      && cha->leader && !IS_NPC(cha->leader)
      && cha->in_room == cha->leader->in_room)
    ch = cha->leader;

/* make sure this was a crusade match */
  if (check_crusade(ch, victim) != CRUSADE_MATCH)
    return TRUE;

/* reset duration to 60 */
   paf->duration = 60;

/* compose the name of the party to be crusaded against */
  sprintf(choice, "%s", show_crusade( paf ));

  /* we create a global yell of sorts */
  sprintf(buf,"`6Hear ye! Hear ye! %s has triumphed over %s in the Crusade against %s!``",
	  PERS2(ch),
	  PERS2(victim),
	  choice);

  /* now we yell it out */
  for ( d = descriptor_list; d; d = d->next ){
    if ( d->connected == CON_PLAYING ){
      act("\n\rA thin squire gallops by you his face beet red from yelling:",
	  d->character, NULL, NULL, TO_CHAR);
      act(buf, d->character, NULL, NULL, TO_CHAR);
    }
  }
  /* palisons triumph */
  if (!IS_NPC(victim) && !IS_NPC(ch) && IS_UNDEAD(victim)
      && ch->pcdata->dall > 1 && is_affected(ch, gsn_ptrium)){
    act("As you put $N down, you watch in amazement as your wounds and scars heal!",
	ch, NULL, victim, TO_CHAR);
    act("As $N dies, you watch in amazement as $n's wounds and scars heal!",
	ch, NULL, victim, TO_CHAR);
    ch->pcdata->dall = UMAX(0, ch->pcdata->dall - 1);
    affect_strip(ch, gsn_ptrium);
    act_new(skill_table[gsn_ptrium].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  }

  /* check for double reward due to undeniable order */
  if (!IS_NPC(ch) && !IS_NPC(victim)
      && is_affected(ch, gsn_uorder)){
    if (IS_UNDEAD(victim)
	|| IS_DEMON(victim)
	|| (victim->pCabal && !IS_SET(victim->pCabal->align, ALIGN_GOOD)) ){
      cpgain *= 2;
      gain *= 2;
      mult = 100;
    }
    else{
      affect_strip(ch, gsn_uorder);
      act_new(skill_table[gsn_uorder].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
    }
  }
  /* reward them with exp and cp */
  if (ch->level < 50){
    sendf(ch, "You gain %d experience!\n\r", gain);
    gain_exp(ch, gain);
  }
  CP_GAIN(ch, cpgain, TRUE);
  hwep_gain(ch, victim, mult);
  return TRUE;
}


//--------------============= AVENGER ===========-----------
int avenger_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_avenger)
    return -1;

/* Cannot be grouped */
  stop_follower(ch);
  die_follower_pc(ch);

  return TRUE;
}//END

/* avenger_kill */
int avenger_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_avenger)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch, paf->string, NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}

int avenger_postkill(CHAR_DATA* cha, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;
  DESCRIPTOR_DATA* d;

/* just showing some messages */
  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  CHAR_DATA* ch = cha;
  int gain = number_range(4000, 8000);
  int cpgain = number_range(75, 250);
  int mult = 200;

  char buf[MIL];

  if (gn != gen_avenger)
    return -1;


  /* charmie switch */
  if (IS_NPC(cha) && IS_AFFECTED(cha, AFF_CHARM)
      && cha->leader && !IS_NPC(cha->leader)
      && cha->in_room == cha->leader->in_room)
    ch = cha->leader;

/* make sure this was a crusade match */
  if (check_avenger(ch, victim) != CRUSADE_MATCH)
    return TRUE;

 /* we create a global yell of sorts */
  sprintf(buf,"`6Hear ye! Hear ye! In act of Holy Retribution %s has triumphed over %s!``",
	  PERS2(ch),
	  PERS2(victim));

  /* now we yell it out */
  for ( d = descriptor_list; d; d = d->next ){
    if ( d->connected == CON_PLAYING ){
      act("\n\rA thin squire gallops by you his face beet red from yelling:",
	  d->character, NULL, NULL, TO_CHAR);
      act(buf, d->character, NULL, NULL, TO_CHAR);
    }
  }
  affect_strip(ch, gn);

/* all of the below is PC only */
  if (IS_NPC(victim) || IS_NPC(ch))
    return TRUE;

  /* check for double reward due to undeniable order */
  if (!IS_NPC(ch) && !IS_NPC(victim)
      && is_affected(ch, gsn_uorder)){
    if (IS_UNDEAD(victim)
	|| IS_DEMON(victim)
	|| !IS_SET(victim->pCabal->align, ALIGN_GOOD)){
      cpgain *= 2;
      gain *= 2;
      mult = 250;
    }
    else{
      affect_strip(ch, gsn_uorder);
      act_new(skill_table[gsn_uorder].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
    }
  }
  /* reward them with exp and cp */
  if (ch->level < 50){
    sendf(ch, "You gain %d experience!\n\r", gain);
    gain_exp(ch, gain);
  }
  CP_GAIN(ch, cpgain, TRUE);
  if (!IS_NPC(ch))
    hwep_gain(ch, victim, mult);

  /* palisons triumph */
  if (!IS_NPC(victim) && !IS_NPC(ch) && IS_UNDEAD(victim)
      && ch->pcdata->dall > 1 && is_affected(ch, gsn_ptrium)){
    act("As you put $N down, you watch in amazement as your wounds and scars heal!",
	ch, NULL, victim, TO_CHAR);
    act("As $N dies, you watch in amazement as $n's wounds and scars heal!",
	ch, NULL, victim, TO_CHAR);
    ch->pcdata->dall = UMAX(0, ch->pcdata->dall - 1);
    affect_strip(ch, gsn_ptrium);
    act_new(skill_table[gsn_ptrium].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  }
  return TRUE;
}

/* BLOOD LOSS */
int bleed_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int fTick)
{
  //ch is the char affected.
  //room to be bled into is ch->in_room

  /*
    This is the function called when effect is inited as well as:
    - each tick (UPDATE_TICK)
  */

  if (gn != gen_bleed)
    return -1;

  if (ch == NULL || ch->in_room == NULL)
    return -1;

  make_item_char(ch, OBJ_VNUM_BLOOD_POOL2, 12);
  return TRUE;
}

int bleed_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //paf pointer alwasy passed though arg1
  if (gn != gen_bleed)
    return -1;
  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}

/* bleed_vtick */
int bleed_vtick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* och;

/*
   the following is run for paf->modifier times, until modifier hits below 0
   or always in case of -1
*/

  if (gn != gen_bleed)
    return -1;

  /* this runs only if paf modifier is > 0 */
  if (!paf->has_string || paf->modifier == 0)
    return FALSE;

  if (paf->modifier != -1  && --paf->modifier < 1){
    send_to_char("You manage to stop the deadly flow of blood.\n\r", ch);
    return FALSE;
  }

  /* only if originator is around */
  if ( (och = get_char(paf->string)) == NULL){
    paf->modifier = 0;
    return FALSE;
  }
/* we do a tiny bit of damage */
/* bleeding message opposite of the one in fight.c */
  damage(och, ch, number_range(1, 3), gn, DAM_INTERNAL, FALSE);
  if ( ch->hit >= ch->max_hit / 4 )
    send_to_char( "You sure are BLEEDING!\n\r", ch );
  return TRUE;
}

/* bleed enter */
int bleed_enter(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

 if (gn != gen_bleed)
   return -1;
/* mark the room person entered with a bit of blood */
 if (ch == NULL || ch->in_room == NULL)
   return FALSE;

 make_item_char(ch, OBJ_VNUM_BLOOD_SPOT, 3);
 return TRUE;
}

/* ARMOR PIERCE */
/* BLOOD LOSS */
int apierce_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int fTick)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_apierce)
    return -1;

  /* we just make sure paf->modifier = 0 since it checks for piercing blow */
  paf->modifier = 0;
  return TRUE;
}

/* apierce_predamage */
int apierce_predamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //Attacker passed by ch
  //victim passed by arg1
  //paf passed by arg2
  //damage done by int1;
  //damtype by int2
  //dt by flag;


  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;

  int dam_type = *int2;
  int dam = *int1;
  int dt = flag;
  const int cost = skill_table[gsn_apierce].min_mana;


/* This returns TRUE if the piering blow is done. */
/* dt change is done in fight.c dependnig on return value */

  //data (chance for damaging blow)
  int chance = effect_table[GN_POS(gn)].value3[0];
  int success = 0;
  int cond = 0;

  //const
  const int luck_mod = 2;

 //ez
  if (gn != gen_apierce)
    return -1;

  //return only on damage
/* run this only if paf = 1 (means piercing blow was made in fight.c */
  if (*int1 < 1 || paf->modifier != 1)
    return FALSE;

  //only block attacks (virtual attacks still have dt's in >= TYPE_HIT)
  if (dt < TYPE_HIT || dam_type == DAM_NONE || dam_type == DAM_INTERNAL)
   return FALSE;


  chance += (get_curr_stat(victim, STAT_LUCK) - get_curr_stat(ch, STAT_LUCK)) * luck_mod;
  if ( (success = number_percent()) < chance){
    OBJ_DATA* pieces[1 + WEAR_SHIELD - WEAR_BODY];
    OBJ_DATA* obj;
    AFFECT_DATA af;
    int i;
    int max = 0;
    for (i = WEAR_BODY; i <= WEAR_SHIELD; i++){
      if ( (obj = get_eq_char(victim, i)) != NULL
	   && obj->condition > 0){
	pieces[max++] = obj;
      }
    }
    /* cost */
    if (ch->mana < cost){
      return FALSE;
    }
    else
      ch->mana -= cost;
    /* now we have an array of objects to choose from */
    /* cannot damage armor  on naked person */
    if (max < 1){
      if (!IS_NPC(victim))
	return FALSE;
      else{
	if (success < chance / 3){
	  act("Your blow passes right through $N's armor leaving a gaping hole!",
	      ch, obj, victim, TO_CHAR);
	  act("$n's blow passes right through $N's armor leaving a gaping hole!",
	      ch, obj, victim, TO_ROOM);
	  check_improve(ch, gsn_apierce, TRUE, 0);
	  /* boost damage */
	  dam = 22 * dam / 10;
	}
	else{
	  act("You land a well aimed blow and damage $N's armor slightly.",
	      ch, obj, victim, TO_CHAR);
	  act("$n lands a well aimed blow and damages $N's armor slightly.",
	      ch, obj, victim, TO_ROOM);
	  check_improve(ch, gsn_apierce, TRUE, 0);
	  dam = 15 * dam / 10;
	}
	*int1 = dam;
	/* set paf to -1 so this is not repeated */
	paf->modifier = -1;
	return TRUE;
      }
    }
    obj = pieces[number_range(0, max -1)];

    /* setup the effect */
    af.type = gsn_apierce;
    af.level = ch->level;
    af.duration = -1;
    af.where = TO_OBJECT;
    af.bitvector = 0;
    af.location = APPLY_O_COND;
    af.modifier = -10;

    cond = get_curr_cond ( obj );
    /* select between small damage and big damage */
    if (success < chance / 3){
      act("Your blow passes right through $p leaving a gaping hole!",
	  ch, obj, victim, TO_CHAR);
      act("$n's blow passes right through $p leaving a gaping hole!",
	  ch, obj, victim, TO_ROOM);
      /* destroy item */
      af.modifier = UMIN(-cond / 4, -10);
      if (cond + af.modifier < 0)
	af.modifier = -cond;
      /* boost damage */
      dam = 22 * dam / 10;
      check_improve(ch, gsn_apierce, TRUE, 0);
    }
    else{
      act("You land a well aimed blow and damage $p slightly.",
	  ch, obj, victim, TO_CHAR);
      act("$n lands a well aimed blow and damages $p slightly.",
	  ch, obj, victim, TO_ROOM);
      check_improve(ch, gsn_apierce, TRUE, 1);
      af.modifier = UMAX(-10, -cond);
      af.modifier = adjust_condition(obj, af.modifier);
      dam = 15 * dam / 10;
    }

    *int1 = dam;

    if (af.modifier != 0)
      affect_join_obj(obj, &af);

//set paf to -1 so this is not repeated

    paf->modifier = -1;

    return TRUE;
  }//end success
/* we reset modiier to 0 so we can try again */
  paf->modifier = 0;
  return FALSE;
}

/* postviolence */
int apierce_postviolence(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag){
/* we simply strip the affect here */
  if (gn != gen_apierce)
    return -1;
  affect_strip(ch, gn);
  return TRUE;
}

//------========CHANT=========--------//

/* CHANT INIT */
/* called in effect.c only */
/* Created by Viri */


/* Created by Viri */
int chant_end(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int sn = *psalm_table[paf->modifier].gsn;

  if (gn != gen_chant)
    return -1;

  if (paf->duration != 0 )
    return FALSE;

  //called by kill routine if killed by update.c ie: duration = 0;
  act_new("The psalm of $t fills your mind and soul with strength and focus.",
      ch, psalm_table[paf->modifier].name, NULL, TO_CHAR, POS_DEAD);
  act("$n ends the last verse of the psalm of $t.",
      ch, psalm_table[paf->modifier].name, NULL, TO_ROOM);
  do_stand(ch, "");

/* CALL THE SPELL FUNCTION HERE */
  (*skill_table[sn].spell_fun) ( sn, paf->level, ch, ch, TARGET_CHAR);
  check_improve(ch, gn, TRUE,1);
  return TRUE;
}


//------========CHANT=========--------//

/* CHANT INIT */
/* called in effect.c only */
/* Created by Viri */

int chant_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_chant)
    return -1;
  if (ch->position > POS_RESTING)
    do_rest(ch, "");
  act("You start to meditate and chant the psalm of $t",
      ch, psalm_table[paf->modifier].name, NULL, TO_CHAR);
  act("$n starts to meditate and chant the psalm of $t.",
      ch, psalm_table[paf->modifier].name, NULL, TO_ROOM);
  return TRUE;
}

int chant_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  AFFECT_DATA af;

  if (gn != gen_chant)
    return -1;

  //check if this is natural end or not (drained)
  if (paf->duration == 0){
    run_effect_update(ch, paf, NULL, gn, NULL, NULL, TRUE, EFF_UPDATE_NONE);
    return TRUE;
  }
  act("With your chant interrupted amidst deep meditation, your vitality suffers.", ch, NULL, NULL, TO_CHAR);
  send_to_char("You feel drained.\n\r", ch);
  af.type = gsn_drained;
  af.level = 60;
  af.duration = 6;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_STR;
  af.modifier = -2;
  if (!is_affected(ch, gsn_drained))
    affect_to_char(ch, &af);
  /* get rid of wait */
  affect_strip(ch, gsn_chant);
  return TRUE;
}

int chant_none(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf;
  //  CHAR_DATA* victim = (CHAR_DATA*) arg1;
 //this is run whenever request is interrupted. we just show a message and kill
  //we set duration to none 0 as to prevent the chant from happening if
  //distrubed on last tick

  send_to_char("Your chant disturbed you snap back to reality.\n\r", ch);
  if ( (paf = affect_find(ch->affected, gen_chant)) != NULL)
    paf->duration = 1;

  affect_strip(ch, gen_chant);
  return TRUE;
}

/* DVOID */
int dvoid_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_dvoid)
    return FALSE;

  //level used to keep track of how often to decrease the power
  paf->level = 0;
  return TRUE;
}

int dvoid_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //paf pointer alwasy passed though arg1
  if (gn != gen_dvoid)
    return -1;
  if (!is_affected(ch, gsn_dvoid)){
    act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
    act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  }
  return TRUE;
}

int dvoid_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
 *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_dvoid)
    return -1;

  if (ch->position == POS_MEDITATE){
    paf->level = UMAX(-3, paf->level - 4);
  }
  else  if (ch->position == POS_SLEEPING){
    paf->level = UMAX(-3, paf->level - number_range(1, 2));
  }
  else
    paf->level += 1;

  if (paf->level < 0){
    affect_modify( ch, paf, FALSE );
    if (ch->position == POS_MEDITATE)
      paf->modifier -= 2;
    else
      paf->modifier -= 1;
    if (paf->modifier < -15)
      paf->modifier = -15;
    else{
      send_to_char("Your mind descends deeper into the divine void.\n\r", ch);
    }
    affect_modify( ch, paf, TRUE );
    return TRUE;
  }

  if (paf->level > 0 && (paf->level % 4) == 0){
    affect_modify( ch, paf, FALSE );
    paf->modifier = UMIN(0, paf->modifier + 1);
    send_to_char("Your mind surfaces slightly from the divine void.\n\r", ch);
    affect_modify( ch, paf, TRUE );
  }
  if (paf->modifier >= 0)
    affect_strip(ch, gn);
  return TRUE;
}

/* Sirants Triumph */
int strium_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag){
//  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_strium)
    return FALSE;

  return TRUE;
}

int strium_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //paf pointer alwasy passed though arg1
  if (gn != gen_strium)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}

int strium_predam(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag){
  //Attacker passed by ch
  //victim passed by arg1
  //paf passed by arg2
  //damage done by int1;
  //damtype by int2
  //dt by flag;

  CHAR_DATA* vch;
  CHAR_DATA* victim = (CHAR_DATA*) arg1;
//  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;

  int dam_type = *int2;
  int dam = *int1;
  int dt = flag;

  int fpc = 0;
  int epc = 0;
  int fnpc = 0;
  int enpc = 0;
  int f = 0;
  int e = 0;

 //ez
  if (gn != gen_strium)
    return -1;

  //run only on damage
  if (*int1 < 1 )
    return FALSE;

  //only block attacks (virtual attacks still have dt's in >= TYPE_HIT)
  if (dt < TYPE_HIT || dam_type == DAM_NONE || dam_type == DAM_INTERNAL)
    return FALSE;
/* we count how many beings in attacker group and enemy group */
/* all npcs counted for attaacker, only 1 for victim */

  for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
    /* friend */
    if (is_same_group(vch, ch)){
      if (!IS_NPC(vch)) fpc++;
      else fnpc++;
    }
/* not friend */
    else if (is_same_group(vch, victim)){
      if (!IS_NPC(vch)) epc++;
      else if (enpc < 1) enpc++;
    }
  }
  /* now we have totals */
  f = fnpc + fpc;
  e = enpc + epc;
/* if more friendly then enemy get penalty of 1/3 */
  if ( f > e)
    dam /= 3;
  else if (epc > fpc){
    int mod = 15 * (epc - fpc);
    dam += mod * dam / 100;
  }
  else if (e > f){
    int mod = 5 * (e - f);
    dam += mod * dam / 100;
  }

//  sendf(ch, "f: %d, e: %d, predam: %d, postdam: %d\n\r", f, e, *int1, dam);
  *int1 = dam;
  return TRUE;
}

/* WARDING */
int ward_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_ward)
    return FALSE;

  //modifier reset to 0 since used later for recharge time
  paf->modifier = 0;
  return TRUE;
}

int ward_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //paf pointer alwasy passed though arg1
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_ward)
    return -1;

  if (paf->has_string){
    act_new(effect_table[GN_POS(gn)].msg_off,ch,paf->string,NULL,TO_CHAR,POS_DEAD);
    act(effect_table[GN_POS(gn)].msg_off2,ch,paf->string,NULL,TO_ROOM);
  }
  return TRUE;
}

int ward_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
 *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int sn = 0;

  if (gn != gen_ward)
    return -1;

  if (!paf->has_string || IS_NULLSTR(paf->string))
    return FALSE;

/* recharge counter */
  if (paf->modifier > 0){
    paf->modifier --;
    if (paf->modifier < 1)
      sendf(ch, "Your %s ward has recharged.", paf->string);
    return FALSE;
  }

/* test of presence of skill that was passed */
  if ( (sn = skill_lookup(paf->string)) < 1){
    bug("ward_tick: string passed is not a skill.", 0);
    return FALSE;
  }
  if (is_affected(ch, sn)){
    act("$n's $t ward flares with power and begins to recharge.",
	ch, paf->string, NULL, TO_ROOM);
    act("Your $t ward flares with power and begins to recharge.",
	ch, paf->string, NULL, TO_CHAR);
    affect_strip(ch, sn);
    if (!IS_GEN(sn)){
      if ( sn > 0 && skill_table[sn].msg_off )
	act_new(skill_table[sn].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
      if ( sn > 0 && skill_table[sn].msg_off2 )
	act(skill_table[sn].msg_off2,ch,NULL,NULL,TO_ROOM);
    }//end if not is gen
    paf->modifier = 12;
    return TRUE;
  }
  return FALSE;
}

/* ARMOR CRAFT */
int acraft_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int id  = amodify_lookup_id( paf->modifier);

  if (gn != gen_acraft)
    return -1;

  act("You begin the process of applying $t to the armor piece.",
      ch, amodify_table[id].name, NULL, TO_CHAR);
  act("$n begins the process of applying $t to the armor piece.",
      ch, amodify_table[id].name, NULL, TO_ROOM);
  if (ch->position > POS_RESTING)
    do_rest(ch, "");
  return TRUE;
}

int acraft_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;


  if (gn != gen_acraft)
    return -1;

  //check if this is natural end or not (drained)
  if (paf->duration == 0){
    run_effect_update(ch, paf, NULL, gn, NULL, NULL, TRUE, EFF_UPDATE_NONE);
    return TRUE;
  }
  act_new(effect_table[GN_POS(gn)].msg_off,ch,paf->string,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,paf->string,NULL,TO_ROOM);
  return TRUE;
}

int acraft_none(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf;
  //  CHAR_DATA* victim = (CHAR_DATA*) arg1;
 //this is run whenever request is interrupted. we just show a message and kill
  //we set duration to none 0 as to prevent the chant from happening if
  //distrubed on last tick

  if ( (paf = affect_find(ch->affected, gen_acraft)) != NULL)
    paf->duration = 1;

  affect_strip(ch, gen_acraft);
  return TRUE;
}

/* Created by Viri */
/*
   this is the meat and potatoes of the armocraft where all the nifty
   things get done.
*/
int acraft_end(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  OBJ_DATA* obj;
  AFFECT_DATA af;
  AFFECT_DATA *baf, *mark, *baf_next;
  int temp = 0;
  int vnum = 0;
  int key = 0;
  CHAR_DATA* vch = get_group_world(ch, TRUE);
  int id  = amodify_lookup_id( paf->modifier);

  if (gn != gen_acraft)
    return -1;

  if (paf->duration != 0 || !paf->has_string)
    return FALSE;

  if (vch){
    act("You begin but soon thoughs of $N's safety break your concentration.", ch, NULL, vch, TO_CHAR);
    return FALSE;
  }
  //get the key
  if ( (baf = affect_find(ch->affected, gsn_identify)) != NULL){
    key = baf->modifier;
    affect_strip(ch, gsn_identify);
  }
  //now we get the item.
  for (obj = ch->carrying; obj; obj = obj->next_content){
    if (is_name(paf->string, obj->name)){
      /* check for key */
      if ( (baf = affect_find(obj->affected, gsn_identify)) != NULL
	   && key == baf->modifier)
	break;
    }
  }
  if (obj == NULL){
    send_to_char("Unfortunatley your weapon seems to have disappeared.\n\r", ch);
    return FALSE;
  }
  affect_strip_obj(obj, gsn_identify);
  if (obj->wear_loc != WEAR_NONE)
    unequip_char(ch, obj );
  vnum = obj->pIndexData->vnum;

/* set common things on the afs */
  af.type = gsn_armorcraft;
  af.duration = -1;
  af.level = ch->level;
  af.where = TO_OBJECT;
  af.bitvector = 0;
  af.location = APPLY_O_LEVEL;
  af.modifier = 3;
  if (obj->level < 60)
    affect_join_obj(obj, &af);
  af.location = APPLY_NONE;
  af.modifier = 0;

  /* start the case statement assigning things */
  switch (paf->modifier ){
  default:
    return FALSE;
    /* repair */
  case 1:
    temp = get_curr_cond ( obj );
    if (temp > 39 + paf->level * 20){
      act("$p's condition is better then what you can repair.",
	  ch, obj, NULL, TO_CHAR);
      return FALSE;
    }
    af.location = APPLY_O_COND;
    af.modifier = (40 + paf->level * 20) - temp;
    if (af.modifier > 0){
      /* remove any kind of negative effects on the armor */
      for (baf = obj->affected; baf; baf = baf->next){
	if (baf->location == APPLY_O_COND && baf->modifier < 0)
	  baf->modifier = 0;
      }
      affect_join_obj(obj, &af);
    }
    break;
    /* RIVETS */
  case 2:
    af.where = TO_AFFECTS;
    af.location = APPLY_DAMROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* BALANCE */
  case 3:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* PADDING */
  case 4:
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL0;
    af.modifier = 2;
    affect_join_obj(obj, &af);
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL1;
    af.modifier = 2;
    affect_join_obj(obj, &af);
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL2;
    af.modifier = 2;
    affect_join_obj(obj, &af);
    break;
    /* CONVERSION */
  case 5:
    if (vnum != OBJ_VNUM_ARMORCRAFT){
      OBJ_DATA* vobj;
      char buf[MIL];
      int i = 0;
      if (IS_OBJ_STAT(obj, ITEM_BURN_PROOF)){
	act("$p is far to resilent to be converted. You efforts don't even make a dent!",
	    ch, obj, NULL, TO_CHAR);
	return FALSE;
      }
      vobj = create_object( get_obj_index( OBJ_VNUM_ARMORCRAFT ), 0);
      /* sockets */
      if (IS_OBJ_STAT(obj, ITEM_SOCKETABLE)
	  && obj->contains
	  && obj->contains->item_type == ITEM_SOCKET){
	OBJ_DATA* sock = obj->contains;
	obj_from_obj( sock );
	unadorn_obj(sock, obj, NULL);
	obj_to_char( sock, ch);
      }
      clone_object(obj, vobj);
      //We strip all the existing effecs except mods
      for (baf = vobj->affected; baf != NULL; baf = baf_next){
	baf_next = baf->next;
	if ( (baf->type != gsn_basic_armor && baf->type != gsn_armorcraft)
	     || baf->location == APPLY_O_WEIGHT)
	  affect_remove_obj(vobj, baf);
      }
      vobj->enchanted = TRUE;

      /* reset extra and wear flags */
      REMOVE_BIT(vobj->wear_flags, ITEM_RARE);
      REMOVE_BIT(vobj->wear_flags, ITEM_UNIQUE);
      vobj->extra_flags = 0;

      /* crusder only */
      vobj->class = class_lookup("crusader");

      /* lower level */
      vobj->level /= 2;

      /* lower the ac */
      for (i = 0; i < 4; i ++)
	vobj->value[i] = obj->value[i] / 2;

      /* reset the ac modifications so far */
      if ( (mark = affect_find(vobj->affected, gsn_basic_armor)) != NULL){
	REMOVE_BIT(mark->bitvector, C);
	REMOVE_BIT(mark->bitvector, E);
	REMOVE_BIT(mark->bitvector, F);
      }

      /* change short descr */
      sprintf(buf, "(modified) %s", obj->short_descr);
      free_string(vobj->short_descr);
      vobj->short_descr = str_dup ( buf );
      /* get rid of the old object */
      obj_from_char( obj );
      extract_obj( obj );
      obj_to_char( vobj, ch );
      obj = vobj;
    }
    else{
      bug("acraft_end: conversion done on convered item.", vnum);
      return FALSE;
    }
    break;
    /* PLATING */
  case 6:
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL0;
    af.modifier = 5;
    affect_join_obj(obj, &af);
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL1;
    af.modifier = 5;
    affect_join_obj(obj, &af);
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL2;
    af.modifier = 5;
    affect_join_obj(obj, &af);
    break;
    /* STARDUST */
  case 7:
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL3;
    af.modifier = 3;
    affect_join_obj(obj, &af);
    break;
    /* BARBS */
  case 8:
    af.where = TO_AFFECTS;
    af.location = APPLY_DAMROLL;
    af.modifier = 2;
    affect_join_obj(obj, &af);
    break;
    /* JOINTS */
  case 9:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    af.location = APPLY_DAMROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* STRAPS */
  case 10:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 2;
    affect_join_obj(obj, &af);
    break;
    /* DEATHWARD */
  case 11:
    af.where = TO_AFFECTS;
    af.location = APPLY_SAVING_MALED;
    af.modifier = -5;
    affect_join_obj(obj, &af);
    break;
    /* HELLWARD */
  case 12:
    af.where = TO_AFFECTS;
    af.location = APPLY_SAVING_AFFL;
    af.modifier = -5;
    affect_join_obj(obj, &af);
    break;
    /* MINDWARD */
  case 13:
    af.where = TO_AFFECTS;
    af.location = APPLY_SAVING_MENTAL;
    af.modifier = -5;
    affect_join_obj(obj, &af);
    break;
    /* ENHANCEMENTSS */
  case 14:
    if (CAN_WEAR(obj, ITEM_WEAR_HEAD)){
      af.where = TO_AFFECTS;
      af.location = APPLY_SAVING_SPELL;
      af.modifier = -2;
      affect_join_obj(obj, &af);
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_FACE)){
      af.where = TO_AFFECTS;
      af.location = APPLY_HITROLL;
      af.modifier = 1;
      affect_join_obj(obj, &af);
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_BODY)){
      AFFECT_DATA af2;
      af.where = TO_AFFECTS;
      af.location = APPLY_HIT;
      af.modifier = 25;
      affect_join_obj(obj, &af);
      af2.type = gsn_armor_enhance;
      af2.level = obj->level;
      af2.duration = -1;
      af2.where = TO_NONE;
      af2.bitvector = 0;
      af2.location = APPLY_NONE;
      af2.modifier = 0;
      if (get_skill(ch, gsn_armor_enhance) > 1){
	send_to_char("You magnify the armor's protective powers.\n\r", ch);
	affect_to_obj(obj, &af2);
      }
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_ARMS)){
      char buf[MIL];
      af.where = TO_AFFECTS;
      af.location = APPLY_DAMROLL;
      af.modifier = 1;
      affect_join_obj(obj, &af);
      if (!is_name("spiked", obj->name)){
	sprintf(buf, "%s spiked", obj->name);
	free_string( obj->name );
	obj->name = str_dup( buf );
      }
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_HANDS)){
      af.where = TO_SKILL;
      af.location = gsn_2hands;
      af.modifier = 2;
      affect_join_obj(obj, &af);
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_LEGS)){
      af.where = TO_AFFECTS;
      af.location = APPLY_DEX;
      af.modifier = 2;
      affect_join_obj(obj, &af);
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_FEET)){
      af.where = TO_SKILL;
      af.location = gsn_dodge;
      af.modifier = 1;
      affect_join_obj(obj, &af);
    }
    else{
      bug("acraft: improvoment used on invalid armor type.", vnum);
      return FALSE;
    }
    break;
    /* LAMINATION */
  case 15:
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL0;
    af.modifier =  5;
    affect_join_obj(obj, &af);
    af.location = APPLY_O_VAL1;
    af.modifier =  5;
    affect_join_obj(obj, &af);
    af.location = APPLY_O_VAL2;
    af.modifier =  5;
    affect_join_obj(obj, &af);
    break;
    /* STARRUNES */
  case 16:
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL3;
    af.modifier =  4;
    affect_join_obj(obj, &af);
    break;
    /* WARPLATES */
  case 17:
    af.where = TO_AFFECTS;
    af.location = APPLY_DAMROLL;
    af.modifier =  3;
    affect_join_obj(obj, &af);
    break;
    /* BATTLE MAIL */
  case 18:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier =  1;
    affect_join_obj(obj, &af);
    af.location = APPLY_DAMROLL;
    af.modifier =  2;
    affect_join_obj(obj, &af);
    break;
    /* STARSEAM MAIL */
  case 19:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier =  3;
    affect_join_obj(obj, &af);
    break;
    /* BLESSING */
  case 20:
    af.where = TO_OBJECT;
    af.bitvector = ITEM_BLESS;
    af.location = APPLY_NONE;
    af.modifier =  0;
    affect_to_obj(obj, &af);
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_SAVING_MALED;
    af.modifier =  -2;
    affect_join_obj(obj, &af);
    break;
    /* HEATPROOFING */
  case 21:
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_SAVING_AFFL;
    af.modifier =  -2;
    affect_join_obj(obj, &af);
    break;
    /* EMBOSSING */
  case 22:
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_SAVING_MENTAL;
    af.modifier =  -2;
    affect_join_obj(obj, &af);
    break;
    /* SPELLWARD */
  case 23:
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_SAVING_SPELL;
    af.modifier =  -5;
    affect_join_obj(obj, &af);
    break;
    /* ARMORCRAFT */
  case 24:
    if (CAN_WEAR(obj, ITEM_WEAR_HEAD)){
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_SAVING_SPELL;
      af.modifier = -3;
      affect_join_obj(obj, &af);
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_FACE)){
      af.where = TO_SKILL;
      af.location = gsn_parry;
      af.modifier = 3;
      affect_join_obj(obj, &af);
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_BODY)){
      /* JUGGERNAUT PLATE HERE */
      OBJ_DATA* vobj;
      vobj = create_object( get_obj_index( OBJ_VNUM_JUGGERNAUT ), 0);
      /* get rid of the old object */
      obj_from_char( obj );
      extract_obj( obj );
      obj_to_char( vobj, ch );
      if (!IS_NPC(ch))
	set_owner(ch, vobj, ch->pcdata->deity );
      obj = vobj;
      act("You finish applying your signature onto $p.", ch, obj, NULL, TO_CHAR);
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_ARMS)){
      char buf[MIL];
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_STR;
      af.modifier = 2;
      affect_join_obj(obj, &af);
      af.location = APPLY_HIT;
      af.modifier = 15;
      affect_join_obj(obj, &af);
      if (!is_name("spiked", obj->name)){
	sprintf(buf, "%s spiked", obj->name);
	free_string( obj->name );
	obj->name = str_dup( buf );
      }
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_HANDS)){
      char buf[MIL];
      af.where = TO_SKILL;
      af.location = gsn_2hands;
      af.modifier = 3;
      affect_join_obj(obj, &af);
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_DAMROLL;
      af.modifier = 1;
      affect_join_obj(obj, &af);
      if (!is_name("spiked", obj->name)){
	sprintf(buf, "%s spiked", obj->name);
	free_string( obj->name );
	obj->name = str_dup( buf );
      }
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_LEGS)){
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_MOVE_GAIN;
      af.modifier = 25;
      affect_join_obj(obj, &af);
    }
    else if (CAN_WEAR(obj, ITEM_WEAR_FEET)){
      af.where = TO_SKILL;
      af.location = gsn_dodge;
      af.modifier = 2;
      affect_join_obj(obj, &af);
    }
    else{
      bug("acraft: armorcraft used on invalid armor type.", vnum);
      return FALSE;
    }
    break;
    /* HELLPLATING */
  case 25:
    af.where = TO_OBJECT;
    af.bitvector = ITEM_BURN_PROOF;
    af.location = APPLY_NONE;
    af.modifier =  0;
    affect_to_obj(obj, &af);
    break;
    /* STARWEIGHT */
  case 26:
    af.where = TO_OBJECT;
    af.location = APPLY_O_WEIGHT;
    af.modifier =  obj->weight > 0 ? -obj->weight / 2 : 0;
    if (af.modifier != 0)
      affect_join_obj(obj, &af);
    break;
    /* STARPLATING */
  case 27:
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL0;
    af.modifier =  7;
    affect_join_obj(obj, &af);
    af.location = APPLY_O_VAL1;
    af.modifier =  7;
    affect_join_obj(obj, &af);
    af.location = APPLY_O_VAL2;
    af.modifier =  7;
    affect_join_obj(obj, &af);
    af.location = APPLY_O_VAL3;
    af.modifier =  12;
    affect_join_obj(obj, &af);
    break;
    /* STARMARK */
  case 28:
    af.where = TO_OBJECT;
    af.bitvector = ITEM_SOCKETABLE;
    af.location = APPLY_NONE;
    af.modifier =  0;
    affect_to_obj(obj, &af);
    break;
    /* SIGNATURE */
  case 29:
    if (!IS_NPC(ch))
      set_owner(ch, obj, ch->pcdata->deity );
    break;
  }
/* add weight as per table */
  af.where = TO_OBJECT;
  af.bitvector = 0;
  af.location = APPLY_O_WEIGHT;
  af.modifier = amodify_table[id].weight * obj->weight / 100;
  if (af.modifier == 0 && amodify_table[id].weight > 0)
    af.modifier = 1;
  if (af.modifier != 0)
    affect_join_obj(obj, &af);
  /* get or set the marker on the armor with the mod bit */
  mark = affect_find(obj->affected, gsn_basic_armor);
  if (mark)
    SET_BIT(mark->bitvector, amodify_table[id].bit);
  else{
    af.type = gsn_basic_armor;
    af.level = vnum;
    af.duration = -1;
    af.where = TO_NONE;
    af.bitvector = amodify_table[id].bit;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_obj(obj, &af);
  }
  act("You finish applying $T onto $p.", ch, obj, amodify_table[id].name, TO_CHAR);
  act("$n finishes applying $T onto $p.", ch, obj, amodify_table[id].name, TO_ROOM);
  return TRUE;
}

/* WEAPON CRAFT */
int wcraft_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int id  = wmodify_lookup_id( paf->modifier);

  if (gn != gen_wcraft)
    return -1;

  act("You begin the process of applying $t to the weapon.",
      ch, wmodify_table[id].name, NULL, TO_CHAR);
  act("$n begins the process of applying $t to the weapon.",
      ch, wmodify_table[id].name, NULL, TO_ROOM);
  if (ch->position > POS_RESTING)
    do_rest(ch, "");
  return TRUE;
}

int wcraft_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_wcraft)
    return -1;

  //check if this is natural end or not (drained)
  if (paf->duration == 0){
    run_effect_update(ch, paf, NULL, gn, NULL, NULL, TRUE, EFF_UPDATE_NONE);
    return TRUE;
  }
  act_new(effect_table[GN_POS(gn)].msg_off,ch,paf->string,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,paf->string,NULL,TO_ROOM);
  return TRUE;
}

int wcraft_none(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf;
  //  CHAR_DATA* victim = (CHAR_DATA*) arg1;
 //this is run whenever request is interrupted. we just show a message and kill
  //we set duration to none 0 as to prevent the chant from happening if
  //distrubed on last tick

  if ( (paf = affect_find(ch->affected, gen_wcraft)) != NULL)
    paf->duration = 1;

  affect_strip(ch, gen_wcraft);
  return TRUE;
}

/* Created by Viri */
/*
   this is the meat and potatoes of the weaponcraft where all the nifty
   things get done.
*/
int wcraft_end(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  OBJ_DATA* obj = NULL;
  AFFECT_DATA af;
  AFFECT_DATA *baf, *mark, *baf_next;
  int temp = 0;
  int vnum = 0;
  int key = 0;
  CHAR_DATA* vch = get_group_world(ch, TRUE);
  int id  = wmodify_lookup_id( paf->modifier);

  if (gn != gen_wcraft)
    return -1;

  if (paf->duration != 0 || !paf->has_string)
    return FALSE;

  if (vch){
    act("You begin but soon thoughs of $N's safety break your concentration.", ch, NULL, vch, TO_CHAR);
    return FALSE;
  }

  //get the key
  if ( (baf = affect_find(ch->affected, gsn_identify)) != NULL){
    key = baf->modifier;
    affect_strip(ch, gsn_identify);
  }
  //now we get the item.
  for (obj = ch->carrying; obj; obj = obj->next_content){
    if (is_name(paf->string, obj->name)){
      /* check for key */
      if ( (baf = affect_find(obj->affected, gsn_identify)) != NULL
	   && key == baf->modifier)
	break;
    }
  }
  if (obj == NULL){
    send_to_char("Unfortunatley your weapon seems to have disappeared.\n\r", ch);
    return FALSE;
  }
  affect_strip_obj(obj, gsn_identify);
  if (obj->wear_loc != WEAR_NONE)
    unequip_char(ch, obj );
  vnum = obj->pIndexData->vnum;

/* set common things on the afs */
  af.type = gsn_weaponcraft;
  af.duration = -1;
  af.level = ch->level;
  af.where = TO_OBJECT;
  af.bitvector = 0;
  af.location = APPLY_O_LEVEL;
  if (wmodify_table[id].hlevel > 0)
    af.modifier = 1;
  else
    af.modifier = 3;
  if (obj->level < 60)
    affect_join_obj(obj, &af);
  af.location = APPLY_NONE;
  af.modifier = 0;

  /* start the case statement assigning things */
  switch (paf->modifier ){
  default:
    return FALSE;
    /* repair */
  case 1:
    temp = get_curr_cond ( obj );
    if (temp > 39 + paf->level * 20){
      act("$p's condition is better then what you can repair.",
	  ch, obj, NULL, TO_CHAR);
      return FALSE;
    }
    af.location = APPLY_O_COND;
    af.modifier = (40 + paf->level * 20) - temp;
    if (af.modifier > 0){
      /* remove any kind of negative effects on the armor */
      for (baf = obj->affected; baf; baf = baf->next){
	if (baf->location == APPLY_O_COND && baf->modifier < 0)
	  baf->modifier = 0;
      }
      affect_join_obj(obj, &af);
    }
    break;
    /* GRINDING */
  case 2:
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_DAMROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* ADJUSTEMENT */
  case 3:
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_HITROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* CONSECRATION */
  case 4:
    if (vnum != OBJ_VNUM_WEAPONCRAFT){
      OBJ_DATA* vobj;
      char buf[MIL];
      const char *string = obj->short_descr;
      const char *pstr = string;
      char *pbuf = buf;
      bool fFound = FALSE;

      vobj = create_object( get_obj_index( OBJ_VNUM_WEAPONCRAFT ), 0);
/* take care of sockets */
      if (IS_OBJ_STAT(obj, ITEM_SOCKETABLE)
	  && obj->contains
	  && obj->contains->item_type == ITEM_SOCKET){
	OBJ_DATA* sock = obj->contains;
	obj_from_obj( sock );
	unadorn_obj(sock, obj, NULL);
	obj_to_char( sock, ch);
	string = obj->short_descr;
	pstr = string;
      }
      clone_object(obj, vobj);
      //We strip all the existing effecs except mods
      for (baf = vobj->affected; baf != NULL; baf = baf_next){
	baf_next = baf->next;
	if ( (baf->type != gsn_arms_main && baf->type != gsn_weaponcraft)
	     || baf->location == APPLY_O_WEIGHT)
	  affect_remove_obj(vobj, baf);
      }
      vobj->enchanted = TRUE;
      /* reset extra and wear flags */
      REMOVE_BIT(vobj->wear_flags, ITEM_RARE);
      REMOVE_BIT(vobj->wear_flags, ITEM_UNIQUE);
      vobj->extra_flags = 0;
      SET_BIT(vobj->extra_flags, ITEM_ANTI_EVIL );
      SET_BIT(vobj->extra_flags, ITEM_ANTI_NEUTRAL );
      /* class only */
      vobj->class = class_lookup("crusader");
      /* lower level */
      vobj->level /= 2;
      /* attatchment of a HOLY word, only if one not in a string */
      if (strstr(string, "holy"))
	sprintf(buf, "%s", string);
      else{
	/* look for a spot directly behind "a" "an" or "the" */
	/* Init */
	buf[0] = '\0';
	while (*pstr != '\0'){
	  /* need a space */
	  if (*pstr != 'a' && *pstr != 't'){
	    *pbuf++ = *pstr++;
	    continue;
	  }
	  /* we found space, look behind for "a" "an" or "the" */
	  /* one spaces forward "a ", two for "an " and three for "the " */
	  else if ( (*pstr =='a' && *(pstr + 1) != '\0' && *(pstr + 1) == ' ')){
	    *pbuf++ = *pstr++;
	    *pbuf = '\0';
	  strcat(pbuf, " holy");
	  strcat(pbuf, pstr);
	  fFound = TRUE;
	  break;
	  }
	  else if (*pstr =='a' && *(pstr + 1) != '\0' && *(pstr + 2) != '\0'
		   && *(pstr + 1) == 'n' && *(pstr + 2) == ' '){
	    *pbuf++ = *pstr++;
	    *pbuf++ = *pstr++;
	    *pbuf = '\0';
	    strcat(pbuf, " holy");
	    strcat(pbuf, pstr);
	    fFound = TRUE;
	    break;
	  }
	  else if (*pstr =='t'  && *(pstr + 1) != '\0' && *(pstr + 2) != '\0'
		   && *(pstr + 3) != '\0' && *(pstr + 1) == 'h'
		 && *(pstr + 2) == 'e' && *(pstr + 3) == ' '){
	    *pbuf++ = *pstr++;
	    *pbuf++ = *pstr++;
	    *pbuf++ = *pstr++;
	    *pbuf = '\0';
	    strcat(pbuf, " holy");
	  strcat(pbuf, pstr);
	  fFound = TRUE;
	  break;
	  }
	  else
	    *pbuf++ = *pstr++;
	}
	if (!fFound){
	  char buf2[MIL];
	  *pbuf = '\0';
	  sprintf(buf2, "a holy %s", buf);
	  strcpy(buf, buf2);
	}
      }
      free_string(vobj->short_descr);
      vobj->short_descr = str_dup ( buf );
      sprintf(buf, "%s holy", vobj->name);
      free_string(vobj->name);
      vobj->name = str_dup ( buf );

      /* get rid of the old object */
      obj_from_char( obj );
      extract_obj( obj );
      obj_to_char( vobj, ch );
      obj = vobj;
    }
    else{
      bug("acraft_end: consecration done on consecrated item.", vnum);
      return FALSE;
    }
    break;
    /* SHARPEN */
  case 5:
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_DAMROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* BALANCE */
  case 6:
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_HITROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* ORNATE HILT */
  case 7:
    af.where = TO_AFFECTS;
    af.location = APPLY_STR;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* ORNATE POMMEL */
  case 8:
    af.where = TO_AFFECTS;
    af.location = APPLY_DEX;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* HONE */
  case 9:
    af.where = TO_AFFECTS;
    af.location = APPLY_DAMROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* FOLD */
  case 10:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* LEATHERING */
  case 11:
    af.where = TO_AFFECTS;
    af.location = APPLY_HIT;
    af.modifier = 10;
    affect_join_obj(obj, &af);
    break;
    /* MINOR BLESSING */
  case 12:
    af.where = TO_AFFECTS;
    af.location = APPLY_HIT_GAIN;
    af.modifier = 4;
    affect_join_obj(obj, &af);
    break;
    /* LONG BLADE */
  case 13:
    af.where = TO_SKILL;
    af.location = (obj->value[0] == WEAPON_EXOTIC ?
		   gsn_hand_to_hand :
		   *weapon_table[weapon_lookup(weapon_name(obj->value[0]))].gsn);
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* WEIGHT */
  case 14:
    af.where = TO_SKILL;
    af.location = gsn_2hands;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* BLESSING */
  case 15:
    af.where = TO_OBJECT;
    af.bitvector = ITEM_BLESS;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_obj(obj, &af);
    break;
    /* STARGRIND */
  case 16:
    af.where = TO_WEAPON;
    af.bitvector = WEAPON_SHARP;
    af.location = APPLY_NONE;
    af.modifier = 0;
    if (IS_WEAPON_STAT(obj, WEAPON_DEADLY)){
      act("Try as you might you cannot make $p any sharper then it already is.",
	  ch, obj, NULL, TO_CHAR);
      return FALSE;
    }
    affect_to_obj(obj, &af);
    break;
    /* KNIFE GUARDS */
  case 17:
    af.where = TO_AFFECTS;
    af.location = APPLY_DAMROLL;
    af.modifier = 2;
    affect_join_obj(obj, &af);
    break;
    /* CARBIDE */
  case 18:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    af.location = APPLY_DAMROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* STARHILT */
  case 19:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 2;
    affect_join_obj(obj, &af);
    break;
    /* CUFFS */
  case 20:
    af.where = TO_OBJECT;
    af.bitvector = ITEM_NODROP;
    af.location = 0;
    af.modifier = 0;
    affect_to_obj(obj, &af);
    af.bitvector = ITEM_NOUNCURSE;
    affect_to_obj(obj, &af);
    break;
    /* STARFORGE */
  case 21:
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL1;
    af.modifier = 1;
    if ( (1 + obj->value[2]) * ((obj->value[1] + 1) / 2) < 40)
      affect_join_obj(obj, &af);
    break;
    /* BLESSING OF ACC */
  case 22:
    af.where = TO_SKILL;
    af.location = (obj->value[0] == WEAPON_EXOTIC ?
		   gsn_hand_to_hand :
		   *weapon_table[weapon_lookup(weapon_name(obj->value[0]))].gsn);
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* BLESSING OF RES */
  case 23:
    af.where = TO_SKILL;
    af.location = gsn_2hands;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* BLESSING  OF HEALTH*/
  case 24:
    af.where = TO_AFFECTS;
    af.location = APPLY_HIT;
    af.modifier = 10;
    affect_join_obj(obj, &af);
    break;
    /* BLESSING OF VIT */
  case 25:
    af.where = TO_AFFECTS;
    af.location = APPLY_HIT_GAIN;
    af.modifier = 4;
    affect_join_obj(obj, &af);
    break;
    /* SERRTIONS */
  case 26:
    af.where = TO_AFFECTS;
    af.location = APPLY_DAMROLL;
    af.modifier = 2;
    affect_join_obj(obj, &af);
    break;
    /* SPACERS */
  case 27:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    af.location = APPLY_DAMROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* ELONGATION */
  case 28:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 2;
    affect_join_obj(obj, &af);
    break;
    /* STARPLATING */
  case 29:
    af.where = TO_OBJECT;
    af.bitvector = ITEM_BURN_PROOF;
    af.location = 0;
    af.modifier = 0;
    affect_to_obj(obj, &af);
    break;
    /* CORROSION */
  case 30:
    af.where = TO_WEAPON;
    af.bitvector = WEAPON_VORPAL;
    af.location = 0;
    af.modifier = 0;
    affect_to_obj(obj, &af);
    break;
    /* PRAYER  OF HEALTH*/
  case 31:
    af.where = TO_AFFECTS;
    af.location = APPLY_HIT;
    af.modifier = 10;
    affect_join_obj(obj, &af);
    break;
    /* PRAYER OF VIT */
  case 32:
    af.where = TO_AFFECTS;
    af.location = APPLY_HIT_GAIN;
    af.modifier = 4;
    affect_join_obj(obj, &af);
    break;
    /* PRAYER OF ACC */
  case 33:
    af.where = TO_SKILL;
    af.location = (obj->value[0] == WEAPON_EXOTIC ?
		   gsn_hand_to_hand :
		   *weapon_table[weapon_lookup(weapon_name(obj->value[0]))].gsn);
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* PRAYER OF RES */
  case 34:
    af.where = TO_SKILL;
    af.location = gsn_2hands;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* PSALM OF SACRIFICE */
  case 35:
    af.where = TO_WEAR;
    af.bitvector = ITEM_NO_SAC;
    af.location = 0;
    af.modifier = 0;
    affect_to_obj(obj, &af);
    break;
    /* PSALM OF ORDER */
  case 36:
    af.where = TO_OBJECT;
    af.location = APPLY_O_VAL2;
    af.modifier = 1;
    if ( (2 + obj->value[2]) * (obj->value[1]) / 2 < 40)
      affect_join_obj(obj, &af);
    break;
    /* PSALM OF RETRIBUTION */
  case 37:
    af.where = TO_AFFECTS;
    af.location = APPLY_DAMROLL;
    af.modifier = 2;
    affect_join_obj(obj, &af);
    break;
    /* PSALM OF EQUALITY  */
  case 38:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    af.location = APPLY_DAMROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    break;
    /* PSALM OF BALANCE */
  case 39:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 2;
    affect_join_obj(obj, &af);
    break;
    /* REL.  OF HEALTH*/
  case 40:
    af.where = TO_AFFECTS;
    af.location = APPLY_HIT;
    af.modifier = 10;
    affect_join_obj(obj, &af);
    break;
    /* REL. OF VIT */
  case 41:
    af.where = TO_AFFECTS;
    af.location = APPLY_HIT_GAIN;
    af.modifier = 4;
    affect_join_obj(obj, &af);
    break;
    /* NAIL OF OREALT */
  case 42:
    af.where = TO_AFFECTS;
    af.location = APPLY_HIT_GAIN;
    af.modifier = 4;
    affect_join_obj(obj, &af);
    af.location = APPLY_HIT;
    af.modifier = 10;
    affect_join_obj(obj, &af);
    af.type = gsn_heal;
    af.level = 60;
    af.duration = -1;
    af.where = TO_NONE;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_join_obj(obj, &af);
    break;
    /* WIDEGUARD */
  case 43:
    af.where = TO_SKILL;
    af.location = gsn_parry;
    af.modifier = 3;
    affect_join_obj(obj, &af);
    break;
    /* TWO HANDS */
  case 44:
    af.where = TO_WEAPON;
    af.bitvector = WEAPON_TWO_HANDS;
    af.location = 0;
    af.modifier = 0;
    if (IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS)){
      send_to_char("You sheepshly notice the weapon is already two handed..\n\r", ch);
      return FALSE;
    }
    affect_to_obj(obj, &af);
    break;
    /* SPIKE */
  case 45:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    if (!is_name("spiked", obj->name)){
      char buf[MIL];
      sprintf(buf, "%s spiked", obj->name);
      free_string( obj->name );
      obj->name = str_dup( buf );
    }
    break;
    /* BARBED */
  case 46:
    af.where = TO_AFFECTS;
    af.location = APPLY_DAMROLL;
    af.modifier = 1;
    affect_join_obj(obj, &af);
    if (!is_name("barbed", obj->name)){
      char buf[MIL];
      sprintf(buf, "%s barbed", obj->name);
      free_string( obj->name );
      obj->name = str_dup( buf );
    }
    break;
    /* WIDEBLADE */
  case 47:
    af.where = TO_SKILL;
    af.location = gsn_batter;
    af.modifier = 10;
    affect_join_obj(obj, &af);
    break;
    /* GREATBLADE */
  case 48:
    af.where = TO_SKILL;
    af.location = gsn_armored_rush;
    af.modifier = 3;
    affect_join_obj(obj, &af);
    break;
    /* BLUNTTIP */
  case 49:
    af.where = TO_SKILL;
    af.location = skill_lookup("behead");
    af.modifier = 3;
    affect_join_obj(obj, &af);
    break;
    /* STAREDGE */
  case 50:
  /* remove sharp first if its there */
    if (IS_WEAPON_STAT(obj, WEAPON_SHARP)){
      AFFECT_DATA* baf, *baf_next;
      for (baf = obj->affected; baf; baf = baf_next){
	baf_next = baf->next;
	if (baf->where == TO_WEAPON && baf->bitvector == WEAPON_SHARP){
	  baf->bitvector = 0;
	  if (baf->location == APPLY_NONE && baf->modifier == 0){
	    affect_remove_obj(obj, baf);
	    break;
	  }
	}
      }
      REMOVE_BIT(obj->value[4], WEAPON_SHARP);
    }
    af.where = TO_WEAPON;
    af.bitvector = WEAPON_DEADLY;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_obj(obj, &af);
    break;
    /* STARBLADE  */
  case 51:
    af.where = TO_AFFECTS;
    af.location = APPLY_HITROLL;
    af.modifier = 3;
    affect_join_obj(obj, &af);
    af.location = APPLY_DAMROLL;
    af.modifier = 3;
    affect_join_obj(obj, &af);
    break;
    /* STARGUARD */
  case 52:
    af.where = TO_OBJECT;
    af.bitvector = ITEM_NOREMOVE;
    af.location = 0;
    af.modifier = 0;
    affect_to_obj(obj, &af);
    break;
    /* SIGNATURE */
  case 53:
    if (!IS_NPC(ch))
      if (!clear_owner(obj)){
	bug("weaponcraft: Error clearing owner.", 0);
      }
    set_owner(ch, obj, ch->pcdata->deity );
    break;
    /* STARMARK */
  case 55:
    af.where = TO_OBJECT;
    af.bitvector = ITEM_SOCKETABLE;
    af.location = APPLY_NONE;
    af.modifier =  0;
    affect_to_obj(obj, &af);
    break;
    /* RETURN */
  case 54:
    af.type = gsn_recall;
    af.level = 60;
    af.duration = -1;
    af.where = TO_NONE;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    baf = affect_to_obj(obj, &af);
    string_to_affect(baf, ch->name);
    break;
  }
/* add weight as per table */
  af.where = TO_OBJECT;
  af.bitvector = 0;
  af.location = APPLY_O_WEIGHT;
  af.modifier = wmodify_table[id].weight * obj->weight / 100;
  if (af.modifier == 0 && wmodify_table[id].weight > 0)
    af.modifier = 1;
  if (af.modifier != 0)
    affect_join_obj(obj, &af);
  /* get or set the marker on the armor with the mod bit */
  mark = affect_find(obj->affected, gsn_arms_main);
  if (mark)
    SET_BIT(mark->bitvector, wmodify_table[id].bit);
  else{
    af.type = gsn_arms_main;
    af.level = vnum;
    af.duration = -1;
    af.where = TO_NONE;
    af.bitvector = wmodify_table[id].bit;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_obj(obj, &af);
  }
  act("You finish applying $T onto $p.", ch, obj, wmodify_table[id].name, TO_CHAR);
  act("$n finishes applying $T onto $p.", ch, obj, wmodify_table[id].name, TO_ROOM);
  return TRUE;
}

/* APPROACH */
int approach_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  if (gn != gen_approach)
    return -1;
  return TRUE;
}//end kill


int approach_none(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf;
  if (gn != gen_approach)
    return -1;
  if ( (paf = affect_find(ch->affected, gn )) != NULL){
    if (paf->level > 0){
      if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off))
	act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
      if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off2))
	act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
      paf->level = 0;
    }
  }
  return TRUE;
}//end non (affstrip)

/* check if the victim in string is still here */
int approach_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
		  *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* vch;

  if (gn != gen_approach)
    return -1;

  if (!paf->has_string || IS_NULLSTR(paf->string))
    return FALSE;

  if ( (vch = get_char_room(ch, NULL, paf->string)) == NULL){
    affect_strip(ch, gn);
    return FALSE;
  }
  return TRUE;
}


/* SUFFOCATE */
int suffocate_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  if (gn != gen_suffocate)
    return -1;
  if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off))
    act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off2))
    act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}//end kill



int suffocate_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
		   *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_suffocate)
    return -1;
  if (paf->modifier == 2)
    paf->modifier = 1;

  if (IS_UNDEAD(ch) ||  IS_SET(ch->imm_flags, IMM_DROWNING)){
    return TRUE;
  }

  /* check if this is the last tick */
  if (paf->duration == 0){
    AFFECT_DATA taf;

    send_to_char("You fall unconscious.\n\r", ch );
    act("With great drama $n claws at $s neck, turns blue and falls unconscious.", ch, NULL, NULL, TO_ROOM );

    taf.where            = TO_AFFECTS;
    taf.type             = gsn_sleep;
    taf.level            = ch->level;
    taf.duration         = 1;
    taf.location         = APPLY_NONE;
    taf.modifier         = 0;
    taf.bitvector        = AFF_SLEEP;
    affect_to_char(ch, &taf);
    stop_fighting( ch, TRUE );
    affect_strip( ch, gn );
    ch->position = POS_SLEEPING;
    return TRUE;
  }
  return TRUE;
}

int suffocate_vtick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* och;

/* the damage done is based on value2 and value3 of effect */
  int min = effect_table[GN_POS(gn)].value2[0];
  int max = effect_table[GN_POS(gn)].value3[0];

  if (gn != gen_suffocate)
    return -1;

  if (IS_UNDEAD(ch) ||  IS_SET(ch->imm_flags, IMM_DROWNING)){
    return TRUE;
  }

  /* damage done ONLY if modfier == 1, 0 means no damage, 2 means delayed*/
  if (paf->modifier != 1)
    return TRUE;

/* check for the owner of this damage if caused by npc */
  if (paf->has_string && !IS_NULLSTR(paf->string)){
    if ( (och = get_char( paf->string)) == NULL){
      affect_strip( ch, gn );
    }
  }
  else
    och = ch;

/* we do the damage */
  damage(och, ch, number_range(min, max), gn, DAM_INTERNAL, FALSE);
  return TRUE;
}


/* CONTRACT */
int contract_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_contract)
    return -1;
  /* no mesage on bounty collect */
  if (paf->modifier == TRUE)
    return TRUE;

  if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off))
    act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off2))
    act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}//end kill



int contract_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
		   *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* vch;

  if (gn != gen_contract)
    return -1;

  if (paf->duration == 0)
    return FALSE;

  if ( (vch = get_char( paf->string)) == NULL){
    paf->duration = 0;
    return FALSE;
  }

  /* check if target is still not covered by bounty */
  if (!bounty_covered(vch ) && is_pk_abs(ch, vch))
    return TRUE;
  else
    paf->duration = 0;
  return TRUE;
}


//-----==SPIRIT BIND====----//


int spirit_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf= (AFFECT_DATA*) arg1;

  if (gn != gen_spirit)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}


/* spirit_precombat */
int spirit_precombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* victim = (CHAR_DATA*) arg2;

  /* notice blades dont have tahc0 calc. they hit, or they are blocked */

//data
  int dam_1 = 4;
  int dam_2 = 4;
  int dam = 0;
  int h_roll = effect_table[GN_POS(gn)].value1[URANGE(0, paf->level/5, 10)];
  int d_roll = effect_table[GN_POS(gn)].value1[URANGE(0, paf->level/5, 10)];

//consts
  const int level_med = 30;
  const int level_mod = 5;

  const int h_roll_mod = 0; //multiplier for skill bonus to h_roll

  const int dt1 = attack_lookup("drain") + TYPE_HIT;

  const int dam_type = DAM_NEGATIVE;

  /*
     The hit is based on values from value1 (increases with level)
     and skill
     Damage on the level of the spell and
     the skill of the user in the spell
  */

  //get the damage bonus here.
  if (paf->level > 49)
    dam_1 ++;
  dam_2 += UMAX(0, paf->level - level_med) / level_mod;
  if (paf->level > 49)
    dam_2++;
  dam = dice (dam_1, dam_2) + d_roll;

  //bonus on hit for skill.
  h_roll += h_roll_mod * UMAX(0, dam_1- 2);

  // sendf(ch, "d_roll: %d, dam: %d, h_roll: %d\n\r", d_roll, dam, h_roll);
  //two attack
 if (ch->fighting != NULL
     && victim->in_room == ch->in_room
     && victim->hit > 0
     && victim->position > POS_STUNNED
     && !is_safe_quiet(ch, victim))
   virtual_damage(ch, victim, NULL, dam, dt1, dam_type, h_roll, paf->level, TRUE, TRUE, gsn_spirit_bind);
  dam = dice (dam_1, dam_2) + d_roll;

 return TRUE;
}


/* coup_kill */
int coup_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_coup)
    return -1;

  if (paf->modifier == 1 ){
    send_to_char("You've succeeded in your Coup!\n\r", ch );
    return TRUE;
  }
  act_new(effect_table[GN_POS(gn)].msg_off,ch, paf->string, NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}

int coup_postkill(CHAR_DATA* cha, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf;// = (AFFECT_DATA*) arg2;

/* just showing some messages */
  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  CHAR_DATA* ch = cha;
  char buf[MIL];
  int diff = 0;

  if (gn != gen_coup)
    return -1;

//ch is the killer, victim the killed

  /* charmie switch */
  if (IS_NPC(cha)
      && cha->leader && !IS_NPC(cha->leader))
    ch = cha->leader;

/* make sure this is PC killing PC both in same cabals */
  if (IS_NPC(ch) || IS_NPC(victim)
      || ch->pCabal == NULL || victim->pCabal == NULL
      || !is_same_cabal(ch->pCabal, victim->pCabal))
    return FALSE;

  /* check if the rank is one above characters */
  diff = victim->pcdata->rank - ch->pcdata->rank;
  if (diff > 2 || diff <= 0)
    return FALSE;

  /* find the paf string if any */
  if ( (paf = affect_find(ch->affected, gen_coup)) == NULL)
    return FALSE;

  /* check if the victim is target of our coup */
  if (!paf->has_string || str_cmp(victim->name, paf->string))
    return FALSE;

  /* do the actual exchange */
  promote(victim, victim->pCabal, -diff );
  promote(ch, ch->pCabal, diff );

  /* fire off the messeges */
  send_to_char("You have been DEPOSED!\n\r",  victim );
  /* change paf to 1 so they get success message */
  paf->modifier = 1;

  /* news to cabal */
  sprintf(buf, "In a sudden power coup %s has taken %s's role as %s!", PERS2(ch), PERS2(victim),
	  get_crank_str(ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ));
  do_hal(get_parent(ch->pCabal)->name, "A coup has been attempted!", buf, NOTE_NEWS);
  affect_strip(ch, gn );
  return TRUE;
}

int coup_postdeath(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
/* ch is person who died, victim who kill */
  AFFECT_DATA* paf;// = (AFFECT_DATA*) arg2;

/* just showing some messages */
  CHAR_DATA* vch = (CHAR_DATA*) arg1;
  CHAR_DATA* victim = vch;
  char buf[MIL];

  if (gn != gen_coup)
    return -1;


  /* charmie switch */
  if (IS_NPC(vch) && IS_AFFECTED(vch, AFF_CHARM)
      && vch->leader && !IS_NPC(vch->leader))
    victim = vch->leader;

/* make sure this is PC killing PC both in same cabals */
  if (IS_NPC(ch) || IS_NPC(victim)
      || ch->pCabal == NULL || victim->pCabal == NULL
      || !is_same_cabal(ch->pCabal, victim->pCabal))
    return FALSE;

  /* check if the killer's rank is greater then characters who died*/
  if (victim->pcdata->rank <= ch->pcdata->rank)
    return FALSE;

  /* find the paf string if any */
  if ( (paf = affect_find(ch->affected, gen_coup)) == NULL)
    return FALSE;

  /* check if the victim is target of our coup */
  if (!paf->has_string || str_cmp(victim->name, paf->string))
    return FALSE;

  /* the ch player now gets moved TWO ranks down.  If he is at rank 1 or lower he gets kicked out */
  if (ch->pcdata->rank < 2 ){
    char_from_cabal( ch );
    /* set time to re-apply */
    ch->pcdata->last_app = mud_data.current_time + (VOTE_DAY * 28);
    send_to_char("You have staved off a Coup!\n\r", victim );

    sendf( ch, "You have failed in your coup and have been removed from ranks of [%s]!\n\r", victim->pCabal->who_name);
    sprintf(buf, "Due to a failed coup %s has been removed from our ranks.", PERS2(ch));
    do_hal(get_parent(victim->pCabal)->name, "A coup has been attempted!", buf, NOTE_NEWS);
    affect_strip( ch, gn );
    return TRUE;
  }
  /* promote two ranks down. */
  promote(ch, ch->pCabal, -2 );



  /* fire off the messeges */
  send_to_char("You have staved off a Coup!\n\r", victim );
  /* news to cabal */
  sprintf(buf, "In a failed power coup against %s, %s has been demoted to a rank of %s.\n\r", PERS2(victim), PERS2(ch),
	  get_crank_str(ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ));
  do_hal(get_parent(ch->pCabal)->name, "A coup has been attempted!", buf, NOTE_NEWS);
  affect_strip(ch, gn );
  send_to_char("You have been DEMOTED!\n\r", ch );
  return TRUE;
}


/* diplo_kill */
int diplo_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  if (gn != gen_diplomacy)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch, NULL, NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}

/* returns true if:
1) in home or allied city
2) in home or allied cabal
3) surrouned by rank -2 cabal members or allies
*/

int diplo_none(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  // AREA_DATA* area_cabal = NULL;
  //  AREA_DATA* area_city = NULL;
  CABAL_DATA* pCabal = get_parent(ch->pCabal );

  if (gn != gen_diplomacy)
    return -1;

  if (!IS_ROYAL(ch) && !IS_NOBLE(ch) && pCabal == NULL)
    return FALSE;
  else if (ch == NULL || ch->in_room == NULL)
    return TRUE;

  if (is_fight_delay(ch, 120))
    return FALSE;
  else if (IS_AREA(ch->in_room->area, AREA_CITY))
    return TRUE;
  else if (IS_AREA(ch->in_room->area, AREA_CABAL))
    return TRUE;
  return FALSE;
}


int diplo_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
	       *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_diplomacy)
    return -1;

  if (run_effect_update(ch, paf, NULL, paf->type, NULL, NULL, TRUE, EFF_UPDATE_NONE))
    return TRUE;
  else
    affect_strip(ch, gn );
  return TRUE;
}



int veng_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_vengeance)
    return -1;

//we set the stored damage to 0 for first time
  paf->modifier = 0;
  return TRUE;
}//END refshield INIT


int veng_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_vengeance)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}



int veng_predamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //Attacker passed by ch
  //victim passed by arg1
  //paf passed by arg2
  //damage done by int1;
  //damtype by int2
  //dt by flag;

  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;
  int dam = (int) *int1;
  int dam_type = (int) *int2;
//  int dt = flag;

  /*
the vengeance collects all damage that got past the defensive spells
in this round (resets to 0 after release) from all characters.
  */

  //ez
  if (gn != gen_vengeance)
    return -1;

  //dont deflect misses :)
  if (dam < 1)
    return FALSE;

 //only block attacks that not spells.
  if (dam_type == DAM_NONE || dam_type == DAM_INTERNAL || ch == victim)
    return FALSE;

  paf->modifier += dam;

  return TRUE;
}


int veng_precombat(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* victim = (CHAR_DATA*) arg2;
  CHAR_DATA* vch_next;

  const int max = effect_table[GN_POS(gn)].value3[0];
  int dam = number_range(paf->modifier / 2, paf->modifier);

  //al we do is flash the capped damage back tocurrent target and reset

  if (dam < 10){
    paf->modifier = 0;
    return FALSE;
  }
  if (ch->fighting == NULL)
    return FALSE;

  dam = URANGE( 5, dam, max );

  for (victim = ch->in_room->people; victim; victim = vch_next){
    vch_next = victim->next_in_room;

    if( victim->hit > 0
	&& victim->fighting == ch
	&& victim->position > POS_STUNNED
	&& !is_safe_quiet(ch, victim))
      damage(ch, victim, dam, gn, DAM_INTERNAL, TRUE);
  }

  paf->modifier = 0;

  return TRUE;
}




int conjure_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_conjure)
    return -1;

  if (ch->position > POS_RESTING)
    do_rest(ch, "");

  return TRUE;
}

int conjure_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  AFFECT_DATA af;

  if (gn != gen_conjure)
    return -1;

  //check if this is natural end or not (drained)
  if (paf->duration == 0){
    run_effect_update(ch, paf, NULL, gn, NULL, NULL, TRUE, EFF_UPDATE_NONE);
    return TRUE;
  }
  act("Your attention wavers for but a moment and $t attacks!", ch, paf->string, NULL, TO_CHAR);
  af.where     = TO_AFFECTS;
  af.type 	 = gen_visit;
  af.level	 = ch->level;
  af.duration = 0;
  af.location  = APPLY_NONE;
  af.modifier  = 1;
  af.bitvector = 0;
  paf = affect_to_char(ch,&af);
  string_to_affect(paf, ch->name);

  return TRUE;
}

int conjure_none(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf;
  //  CHAR_DATA* victim = (CHAR_DATA*) arg1;
 //this is run whenever request is interrupted. we just show a message and kill
  //we set duration to none 0 as to prevent the chant from happening if
  //distrubed on last tick

  send_to_char("Your conjuration disturbed you snap back to reality.\n\r", ch);
  if ( (paf = affect_find(ch->affected, gen_conjure)) != NULL)
    paf->duration = 1;

  affect_strip(ch, gen_conjure);
  return TRUE;
}

/* set various effects based on what was in modifier */
int conjure_end(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  AFFECT_DATA af;

  const int sn = skill_lookup("conjuration");
  const int chance = URANGE(5, get_skill(ch, sn) + (ch->level - paf->level) * 5, 90);
  const int level = ch->level;

  if (gn != gen_conjure)
    return -1;

  if (paf->duration != 0 )
    return FALSE;

  //called by kill routine if killed by update.c ie: duration = 0;
  act("As $n mumbles the last of the rites $t appears!", ch, paf->string, NULL, TO_ROOM );
  act("As you mumble the last of the rites $t appears!", ch, paf->string, NULL, TO_CHAR );
  do_stand(ch, "");

  if (number_percent() > chance){
    act("Your will falters and $t attacks!", ch, paf->string, NULL, TO_CHAR);
    act("$n's will falters and $t attacks!", ch, paf->string, NULL, TO_ROOM);

    af.where     = TO_AFFECTS;
    af.type 	 = gen_visit;
    af.level	 = level;
    af.duration = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 1;
    af.bitvector = 0;
    paf = affect_to_char(ch,&af);
    string_to_affect(paf, ch->name);
    return FALSE;
  }

  /* wait timer */
  af.type = skill_lookup("conjuration");;
  af.duration = 86;
  af.level = ch->level;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af );

  act("You best $t and gain a fraction of its power.", ch, paf->string, NULL, TO_CHAR);
  act("$n bests $t and gains a fraction of its power.", ch, paf->string, NULL, TO_ROOM);

  /* select based on choice */
  switch( paf->modifier ){
    /* NYOGTHA (DINV) */
  case 0:
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup("detect invis");
    af.level     = 60;
    af.duration  = 74;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_INVIS;
    affect_strip( ch, af.type );
    affect_to_char( ch, &af );
    act( "Your eyes tingle.", ch,NULL,NULL,TO_CHAR );
    act( "$n's eyes glow and fade.", ch,NULL,NULL,TO_ROOM );
    break;
    /* GROMEL (Enlarge) */
  case 1:
    if ( ch->size >= SIZE_GIANT){
      act("You are already enlarged.",ch,NULL,NULL,TO_CHAR);
    }
    af.where     = TO_AFFECTS2;
    af.type      = skill_lookup("enlarge");
    af.level     = 60;
    af.duration  = 72;
    af.location  = APPLY_SIZE;
    af.modifier  = 1;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    af.location  = APPLY_CON;
    af.modifier  = 2;
    affect_to_char( ch, &af );
    af.location  = APPLY_STR;
    af.modifier  = 2;
    affect_to_char( ch, &af );
    af.location  = APPLY_DAMROLL;
    af.modifier  = 3;
    affect_strip( ch, af.type );
    affect_to_char( ch, &af );
    act( "The world and everything around you seem to shrink.", ch,NULL,NULL,TO_CHAR );
    act("$n dwarfs everything around $m as $e grows.",ch,NULL,NULL,TO_ROOM);
    break;
    /* MISCREANT: mana/hp */
  case 2:
    af.type      = skill_lookup("demon summon");
    af.level     = 60;
    af.duration  = 74;
    af.where     = TO_AFFECTS;
    af.bitvector = 0;
    af.location  = APPLY_HIT;
    af.modifier  = 33;
    affect_strip( ch, af.type );
    affect_to_char( ch, &af );
    af.location  = APPLY_MANA;
    af.modifier  = 50;
    affect_to_char( ch, &af );
    act( "Your body and mind are fortified.", ch,NULL,NULL,TO_CHAR );
    act( "$n's body and mind become fortified.", ch,NULL,NULL,TO_ROOM );
    break;
    /* ABERRANT (PASS DOOR)) */
  case 3:
    af.type      = skill_lookup("pass door");
    af.level     = 60;
    af.duration  = 74;
    af.where     = TO_AFFECTS;
    af.bitvector = AFF_PASS_DOOR;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    affect_strip( ch, af.type );
    affect_to_char( ch, &af );
    act( "You phase out of existence.", ch, NULL, NULL, TO_CHAR );
    act( "$n phases out of existence.", ch, NULL, NULL, TO_ROOM );
    break;
    /* AZAZEL (pvsgood */
  case 4:
    af.type      = skill_lookup("protection");
    af.level     = 60;
    af.duration  = 74;
    af.where     = TO_AFFECTS;
    af.bitvector = AFF_PROTECT_GOOD;
    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = -1;
    affect_strip( ch, af.type );
    affect_to_char( ch, &af );
    act( "You feel aligned with darkness.",ch,NULL,NULL,TO_CHAR);
    act("$n is protected from good.",ch,NULL,NULL,TO_ROOM);
    break;
    /* MALIGNANT (flight) */
  case 5:
    af.type      = gsn_fly;
    af.level     = 60;
    af.duration  = 74;
    af.where     = TO_AFFECTS;
    af.bitvector = AFF_FLYING;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    affect_strip( ch, af.type );
    affect_to_char( ch, &af );
    act( "Your feet rise off the ground.", ch, NULL,NULL, TO_CHAR );
    act( "$n's feet rise off the ground.", ch, NULL, NULL, TO_ROOM );
    break;
  }
  return TRUE;
}


int maze_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  ROOM_INDEX_DATA* maze, *skeleton;
  char buf[MIL];
  int door = 0;

  if (gn != gen_maze)
    return -1;
/* we fashion a virtual room from a skeleton, load it, make all its exits point to itself, and
   move the character there
*/

  if ( (skeleton = get_room_index( ROOM_VNUM_MAZE)) == NULL){
    bug("maze_init: could not load the skeleton.\n\r", 0);
    return FALSE;
  }
  /* get new room info */
  maze = new_room_index();
  /* clone it */
  clone_room(skeleton, maze, TRUE );

  /* change name */
  sprintf( buf, maze->name, PERS2(ch) );
  format_room_name(maze, buf);



  load_vir_room( maze, ch->in_room->area );

  maze->watch_vnums = (Double_List *) malloc (sizeof (Double_List));
  maze->watch_vnums->cur_entry = (void *) (long) paf->modifier;
  maze->watch_vnums->prev_node = NULL;
  maze->watch_vnums->next_node = NULL;

  for (door = 0; door < MAX_DOOR; door++){
    maze->exit[door] = new_exit();
    maze->exit[door]->orig_door = door;
    maze->exit[door]->vnum	= maze->vnum;
    maze->exit[door]->to_room	= maze;
    maze->exit[door]->rs_flags	= 0;
    maze->exit[door]->exit_info = 0;
  }

  act("With a scream of tortured reality $n is MAZED!", ch, NULL, NULL, TO_ROOM );
  act("The tortured reality screams in protest as you are MAZED!", ch, NULL, NULL, TO_CHAR );


  stop_fighting( ch, TRUE );
  char_from_room( ch );
  char_to_room( ch, maze );

  do_look(ch, "auto" );
  paf->modifier = maze->vnum;

  return TRUE;
}

int maze_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  ROOM_INDEX_DATA* maze, *before;

  if (gn != gen_maze)
    return -1;

  /* we collapse the virtual room */
  if ( (maze = get_room_index( paf->modifier )) == NULL){
    bug("maze_kill: could not get index for maze %d", paf->modifier );
    return FALSE;
  }
  before = ch->in_room;

  /* collapse the maze */
  unload_vir_room( maze );

  if (ch->in_room != before ){
    act("With a scream of reality rending apart $n returns from $s maze!", ch, NULL, NULL, TO_ROOM);
    do_look(ch, "auto");
    WAIT_STATE(ch, PULSE_VIOLENCE );
  }

  return TRUE;
}


int chron_predamage(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //Attacker passed by ch
  //victim passed by arg1
  //paf passed by arg2
  //damage done by int1;
  //damtype by int2
  //dt by flag;

//  CHAR_DATA* victim = (CHAR_DATA*) arg1;
  AFFECT_DATA* paf = (AFFECT_DATA*) arg2;
  int dam = (int) *int1;
  const int max_hit = 1000;
  /*
the shield collects 1/4 of all damage, up to max_hit
back on 0th tick
  */

  //data
  int dam_abs = 0;

  //ez
  if (gn != gen_chron)
    return -1;

  //dont absrob misses :)
  if (dam < 1 )
    return FALSE;

  //we get dmage absorbed
  dam_abs = 1 * dam / 4;

  /* return f modifier and dam absorbed are too much */
  if (paf->modifier + dam_abs > max_hit)
    return FALSE;

  paf->modifier += dam_abs;
  dam -= dam_abs;

 *int1 = UMAX(1,dam);
 return TRUE;
}

/* flashes damage back */
int chron_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
		   *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_chron)
    return -1;

  if (paf->duration > 3 || paf->modifier < 1)
    return FALSE;

  /* show some warning */
  switch(paf->duration){
  case 3:
    send_to_char("You get a passing feeling of impending danger.\n\r", ch );
    break;
  case 2:
    act("Your vision splits and you see a translucent figure casting a spell.", ch, NULL, NULL, TO_ALL );
    break;
  case 1:
    send_to_char("The hair on your neck raises suddenly!\n\r", ch );
    break;
  case 0:
    /* do the damage */
    affect_strip(ch, gn );
    act("With a low roar a translucent vortex of bright colors winks in and out.", ch, NULL, NULL, TO_ALL );
    paf->modifier = UMIN(ch->hit - 1, paf->modifier);
    damage( ch, ch, paf->modifier, gn, DAM_INTERNAL, TRUE );
    break;
  }
  return TRUE;
}

int chron_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_chron)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}


int paradox_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_paradox)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}


int paradox_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
		   *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* vch, *vch_next;
  DESCRIPTOR_DATA* d;
  ROOM_INDEX_DATA* to_room;

  if (gn != gen_paradox)
    return -1;

  if (paf->duration > 3 || paf->modifier < 1)
    return FALSE;

  /* show some warning */
  switch(paf->duration){
  case 3:
    act("Your vision blurs and you see yourself walk past.", ch, NULL, NULL, TO_ALL);
    break;
  case 2:
    act("Reality shatters momentarly, and you witness myriad of your pasts and futures.", ch, NULL, NULL, TO_ALL);
    break;
  case 1:
    act("A small child appears, emits a cry of suprise and disappears midcry!", ch, NULL, NULL, TO_ALL);
    break;
  case 0:
    for ( d = descriptor_list; d != NULL; d = d->next ){
      if (d->connected == CON_PLAYING && d->character->in_room && d->character->in_room != ch->in_room)
	send_to_char("Your insides twist as time about you is sheared by forces of Paradox.\n\r", d->character);
    }
    if ( (to_room = get_room_index( paf->modifier )) == NULL)
      return FALSE;
    /* transfer the group,then character */
    for (vch = ch->in_room->people; vch; vch = vch_next){
      vch_next = vch->next_in_room;
      if (!is_same_group(vch, ch ))
	continue;
      stop_fighting( vch, TRUE );
      char_from_room( vch );
      char_to_room( vch, to_room );
    }
    stop_fighting( ch, TRUE );
    char_from_room( ch );
    char_to_room( ch, to_room );
    break;
  }
  return TRUE;
}
//move person to room on expiry
int temp_avenger_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
		   *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* och, *vch, *vch_next;

  if (gn != gen_temp_avenger)
    return -1;
  else if (paf->duration > 0)
    return FALSE;
  else if ( (och = get_char( paf->string)) == NULL)
    return FALSE;

  if (IS_NPC(och) || och->in_room == NULL || och->in_room == ch->in_room || IS_SET(och->in_room->area->area_flags, AREA_RESTRICTED)){
    send_to_char("Your temporal avenger spell failed.\n\r", ch);
    return FALSE;
  }

  act("Your vision splits and you see yourself casting the temporal avenger spell.", ch, NULL, NULL, TO_CHAR);
  act("$n disappears!", ch, NULL, NULL, TO_ROOM);

  /* transfer the group,then character */
  for (vch = ch->in_room->people; vch; vch = vch_next){
    vch_next = vch->next_in_room;
    if (!is_same_group(vch, ch ) || !IS_NPC(vch) || !IS_AFFECTED(vch, AFF_CHARM) || vch->summoner != ch)
      continue;
    stop_fighting( vch, TRUE );
    char_from_room( vch );
    char_to_room( vch, och->in_room );
  }
  stop_fighting( ch, TRUE );
  char_from_room( ch );
  char_to_room( ch, och->in_room );

  say_spell(ch, skill_lookup("temporal avenger"));
  do_look( ch, "auto" );

  WAIT_STATE(ch, PULSE_VIOLENCE);
  return TRUE;
}

//MOVE DAM
int movedam_enter(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  ROOM_INDEX_DATA* to_room = (ROOM_INDEX_DATA*) arg2;
  AFFECT_DATA* paf;
  CHAR_DATA* och;
  int move = 0;
/*
   damage done is based on move cost of the room we are in
*/

  if (gn != gen_move_dam)
    return -1;

  if (to_room == NULL)
    return FALSE;
  else if ( (paf = affect_find(ch->affected, gen_move_dam)) == NULL)
    return FALSE;
  /* only if originator is around */
  else if (!paf->has_string || IS_NULLSTR(paf->string))
    och = ch;
  else if ( (och = get_char(paf->string)) == NULL){
    return FALSE;
  }

  move = UMAX(1, movement_loss[to_room->sector_type]);
  move = URANGE(1, move / 2, 5);
  /* we do a tiny bit of damage */
  damage(och, ch, move, gn, DAM_INTERNAL, FALSE);

  return TRUE;
}

//------------KINESIS------------------//
int kinesis_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  if (gn != gen_kinesis)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}


int kinesis_vtick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* och;
  int sn;

  if (gn != gen_kinesis)
    return -1;


  /* if no string on paf return */
  if (!paf->has_string || IS_NULLSTR(paf->string))
    return FALSE;

  /* if modifier is not odd, also ignore */
  if (paf->modifier-- % 2 == 0)
    return FALSE;

  if ( (och = get_char( paf->string )) == NULL)
    return FALSE;


  /* get spell to cast */
  if ((sn = paf->bitvector) < 1
      || skill_table[sn].spell_fun == spell_null){
    return FALSE;
  }

  /* if not in combat ignore */
  if (och->fighting){
    if (number_percent() < get_skill(och, gsn_kinesis)){
/* we cast the spell */
      (*skill_table[sn].spell_fun) (sn, paf->level, och, ch, -1);
      check_improve(och, gsn_kinesis, TRUE, 1);
    }
    else
      check_improve(och, gsn_kinesis, FALSE, 1);
  }

  if (paf->modifier < 1)
    affect_strip(ch, gn );
  return TRUE;
}


//-------CREATE OBJECT-----------//
//cretes object vnum paf->modifier on tick/leave with paf->bitvector chance
int create_object_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int fTick)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  //ch is the char affected.
  //room to be spiiled into is ch->in_room
/*
    This is the function called when effect is inited as well as:
- each step (UPDATE_LEAVE)
- each tick (UPDATE_TICK)

  */
  if (gn != gen_create_object)
    return -1;

  if (ch == NULL || ch->in_room == NULL)
    return -1;
  if (number_percent() > paf->bitvector)
    return FALSE;
  make_item_char(ch, paf->modifier, 3);

  return TRUE;
}

/* written by viri */
int create_object_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  //called when cutpurse effect is stripped.
  //paf pointer alwasy passed though arg1
  if (gn != gen_create_object)
    return -1;
  if (paf->has_string && !IS_NULLSTR(paf->string))
    sendf(ch, "%s\n\r", paf->string);
  return TRUE;
}

//AMORPHOUS INFECTION //
int ainfect_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int fTick)
{
//  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  //ch is the char affected.

  if (gn != gen_ainfection)
    return -1;

  if (ch == NULL || ch->in_room == NULL)
    return -1;

  return TRUE;
}

/* written by viri */
int ainfect_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* vch, *och;

  if (gn != gen_ainfection)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  /* we look for a new host */
  if (!paf->has_string || IS_NULLSTR(paf->string))
    och = NULL;
  else
    och = get_char( paf->string );

  if (--paf->modifier < 1){
    if (och)
      send_to_char("Your amorphous parasite has died.\n\r", och);
    return TRUE;
  }

  /* try to find a PC first */
  for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
    if (vch == ch || IS_IMMORTAL(vch))
      continue;
    if (!IS_NPC(vch))
      break;
  }
  if (vch == NULL){
    for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
      if (vch == ch || IS_IMMORTAL(vch))
	continue;
      if (IS_NPC(vch))
	break;
    }
  }
  if (vch == NULL){
    if (och)
      send_to_char("Your amorphous parasite has died.\n\r", och);
    return TRUE;
  }
  else{
    AFFECT_DATA af, *baf;

    af.type = gn;
    af.level = paf->level;
    af.duration = -1;
    af.where = TO_NONE;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = paf->modifier;
    baf = affect_to_char(vch, &af);
    if (paf->has_string && !IS_NULLSTR(paf->string))
      string_to_affect(baf, paf->string);
    if (och){
      if (och == vch)
	act("Your amorphous parasite has entered you!", och, NULL, vch, TO_CHAR);
      else
	act("Your amorphous parasite has entered $N.", och, NULL, vch, TO_CHAR);
    }
  }
  return TRUE;
}

int ainfect_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
		   *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int loss = 0;

  if (gn != gen_ainfection)
    return -1;

  if (!IS_AWAKE(ch)
      && (IS_AFFECTED(ch, AFF_SLEEP) || is_affected(ch, gsn_ecstacy)))
    return TRUE;

  /* make sure person cannot be taken out by a voodoo harm */
  /* or that we kill them accidently if hp was taken down to very low level */
  if (ch->hit <= 150){
    paf->duration = 0;
    return TRUE;
  }
  //lower hp by bitvector% of max, increase by 2
  paf->bitvector += 2;
  loss = UMAX(1, paf->bitvector * ch->max_hit / 100);

  if (number_percent() < 30)
    send_to_char("Your skin feels funny..\n\r", ch);

  ch->hit -= loss;
  ch->hit = UMAX(1, ch->hit);

  update_pos( ch );

  //if host under 15% hp vacate
  if (ch->hit <= 15 * ch->max_hit / 100)
    paf->duration = 0;

  return TRUE;
}


/* DREAMWALK */
int dreamwalk_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_dreamwalk)
    return -1;

  if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off))
    act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off2))
    act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  /* send message to victim if the modifier was set to a dream */
  if (paf->modifier){
    CHAR_DATA* vch;
    if (!paf->has_string || IS_NULLSTR(paf->string))
      return TRUE;
    else if ( (vch = get_char_world(ch, paf->string)) == NULL){
      affect_strip(ch, gn);
      return TRUE;
    }
    else if (IS_AWAKE(vch))
      return TRUE;
    send_to_char("Your dreams return to normal.\n\r", vch);
  }
  return TRUE;
}//end kill


int dreamwalk_none(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//  AFFECT_DATA* paf;
  if (gn != gen_dreamwalk)
    return -1;
  affect_strip(ch, gn );
  return TRUE;
}

/* check if the victim in string is still here */
int dreamwalk_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int
		  *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* vch;
  int sn = 0;

  if (gn != gen_dreamwalk)
    return -1;

  if (!paf->has_string || IS_NULLSTR(paf->string))
    return FALSE;
  else if ( (vch = get_char_world(ch, paf->string)) == NULL){
    affect_strip(ch, gn);
    return FALSE;
  }
  else if (vch->in_room != ch->in_room && !dprobe_check(ch, vch)){
    affect_strip(ch, gn);
    return FALSE;
  }
  else if (IS_AWAKE(vch)){
    affect_strip(ch, gn );
    return FALSE;
  }
  /* get spell to cast */
  else if ((sn = paf->modifier) < 1
      || skill_table[sn].spell_fun == spell_null){
    return FALSE;
  }

/* we cast the spell */
  (*skill_table[sn].spell_fun) (sn, ch->level, ch, vch, -1);

  /* reset message count , give it -4 so there is some wait */
  paf->level = -3;

  check_improve(ch, gsn_telepathy, TRUE, 1);

  /* if duration is now 0 strip it */
  if (paf->duration == 0){
    WAIT_STATE(ch, PULSE_VIOLENCE);
    affect_strip(ch, gn );
  }
  return TRUE;
}

/* dreamwalk_vtick */
int dreamwalk_vtick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* vch;
  char* ch_msg;
  char* vi_msg;
/*
  checks for victim in room with us, also shows dream messages
*/

  if (gn != gen_dreamwalk)
    return -1;
  /* this runs only if paf modifier is > 0 */
  else if (!paf->has_string || IS_NULLSTR(paf->string))
    return FALSE;
  /* only if originator is around */
  else if ( (vch = get_char(paf->string)) == NULL){
    affect_strip(ch, gn );
    return FALSE;
  }
  else if (vch->in_room != ch->in_room && !dprobe_check(ch, vch)){
    affect_strip(ch, gn );
    return FALSE;
  }
  else if (IS_AWAKE(vch)){
    affect_strip(ch, gn );
    return FALSE;
  }

  /* do not show messages if no dream spell set */
  if (paf->modifier < 1 || paf->modifier == gsn_dreamprobe)
    return FALSE;

  /* show dream messages */
  switch (paf->level++){
  case 0:
    vi_msg = "You begin to dream...";
    ch_msg = "$N begins to dream...";
    break;
  case 1:
    vi_msg = "A cloaked figure seems to watch you from distance...";
    ch_msg = "You watch $M from distance in $S dream...";
    break;
  case 2:
    vi_msg = "Your dreams begin to turn dark and ominous...";
    ch_msg = "$N's dreams begin to turn dark and ominous...";
    break;
  case 3:
    vi_msg = "You try to run but you seem to be going in circles...";
    ch_msg = "$E tries to run but you are now in control of $S dream...";
    break;
  case 4:
    vi_msg = "You find yourself in a dark room the shadows swirling about you...";
    ch_msg = "You place $N in a dark room the shadows swirling about $M...";
    break;
  default:
    return FALSE;
  }
  send_to_char("(Dream)", ch);
  send_to_char("(Dream)", vch);
  act_new(ch_msg, ch, NULL, vch, TO_CHAR, POS_DEAD);
  act_new(vi_msg, ch, NULL, vch, TO_VICT, POS_DEAD);

  /* if the person is not under effects of sleep set it for rest of tick */
  if (!IS_AFFECTED(vch, AFF_SLEEP)){
    AFFECT_DATA af;
    af.where     = TO_AFFECTS;
    af.type      = gsn_sleep;
    af.level     = ch->level;
    af.duration  = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SLEEP;
    affect_to_char( vch, &af );
    vch->position = POS_SLEEPING;
  }
  return TRUE;
}


/* spell vise */
int svise_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
//  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  if (gn != gen_svise)
    return -1;

  if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off))
    act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  if (!IS_NULLSTR(effect_table[GN_POS(gn)].msg_off2))
    act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);

  return TRUE;
}

int svise_vtick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* baf, *paf = (AFFECT_DATA*) arg1;

  if (paf->modifier-- <= 0){
    affect_strip(ch, gn);
    return FALSE;
  }
  /* get a random spell and dispel it */
  if ( (baf = get_random_affect( ch->affected, GN_BEN)) == NULL){
    affect_strip(ch, gn);
    return FALSE;
  }
  else{
    CHAR_DATA* och;
  /* dispel the spell */
    if (!IS_NPC(ch))
      check_dispel(-99, ch, baf->type);
    if (!paf->has_string || IS_NULLSTR(paf->string))
      return FALSE;
    if ( (och = get_char( paf->string )) == NULL)
      return FALSE;
    damage(och, ch, number_range(1, 5), gn, DAM_MENTAL, FALSE );
  }
  return TRUE;
}

//~~~~METABOLIC BOOST~~~~//
int mboost_init(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* orig_paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* vch;
  AFFECT_DATA* paf, af;
  int gsn_mboost = skill_lookup("metabolic boost");
  if (gn != gen_metabolic_boost)
    return -1;

  /* we run through everyone grouped with the character and boost them
     for 0 ticks
  */
  for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
    if (!is_same_group(vch, ch) || vch == ch)
      continue;
    else if ( vch->pCabal && IS_CABAL(get_parent(vch->pCabal), CABAL_NOMAGIC))
      continue;
    else if ( (paf = affect_find(vch->affected, gsn_mboost)) != NULL)
      paf->duration = 1;
    else if (IS_AFFECTED(vch, AFF_HASTE))
      continue;
    else{
      af.type = gsn_mboost;
      af.level = orig_paf->level;
      af.duration = 1;
      af.where	 = TO_AFFECTS;
      af.bitvector = AFF_HASTE;
      af.modifier  = APPLY_NONE;
      af.location  = 0;
      affect_to_char(vch, &af);
      send_to_char( "You feel your metabolism accelerate.", vch);
      act("$n begins moving more quickly.", vch,NULL,NULL,TO_ROOM);
    }
  }
  return TRUE;
}//END

/* mboost_kill */
int mboost_kill(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{

  if (gn != gen_metabolic_boost)
    return -1;

  act_new(effect_table[GN_POS(gn)].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
  act(effect_table[GN_POS(gn)].msg_off2,ch,NULL,NULL,TO_ROOM);
  return TRUE;
}

int phantasm_vtick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* och;
  const int gsn_phantasm = skill_lookup("spook");

  if (gn != gen_phantasm)
    return -1;

  /* if not in combat ignore */
  /* if no string on paf return */
  if (!paf->has_string || IS_NULLSTR(paf->string))
    return FALSE;

  /* if modifier is not odd, also ignore */
  if (paf->modifier-- % 2 == 0)
    return FALSE;

  if ( (och = get_char( paf->string )) == NULL)
    return FALSE;



  if (ch->fighting == och
      && !saves_spell(paf->level, ch, DAM_MENTAL, skill_table[gsn_phantasm].spell_type)){
    bool  fWield = get_eq_char( ch, WEAR_WIELD) != NULL;
    bool  fShield = get_eq_char( ch, WEAR_SHIELD) != NULL;
    bool  fFear = is_affected(ch, gsn_fear) ;

    if (fWield || fShield || !fFear){
      act("A fearsome phantasm appears before you!", ch, NULL, NULL, TO_CHAR);
      act("$n stares in fear at something you cannot see.", ch, NULL, NULL, TO_ROOM);
      if (number_percent() < 50 || fFear){
	if (fWield){
	  send_to_char("You losen the grip on your weapon!\n\r", ch);
	  disarm( och, ch );
	}
	else if (fShield){
	  send_to_char("You losen the grip on your shield!\n\r", ch);
	  shield_disarm( och, ch );
	}
      }
      else
	spell_terror( gsn_fear, paf->level, och, ch, 0 );
    }
  }
  if (paf->modifier < 1)
    affect_strip(ch, gn );
  return TRUE;
}


int dragon_blood_precom(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  CHAR_DATA* victim = (CHAR_DATA*) arg2;

  const int chance = 33;
  int dt;


  /* damage type is stored in paf->modifier as a random value between DAM_FIRE and DAM_ACID
     - we select the sn and cast the spell.
     - level drops for each cast, spit only on even levels
  */

  if (number_percent() > chance)
    return FALSE;
  else if (paf->level > 10){
    paf->level--;
  }

  switch( paf->modifier ){
  default:
  case DAM_FIRE:	dt = skill_lookup("fire breath");	break;
  case DAM_COLD:	dt = skill_lookup("frost breath");	break;
  case DAM_LIGHTNING:	dt = skill_lookup("lightning breath");	break;
  case DAM_ACID:	dt = skill_lookup("acid breath");	break;
  }

  (*skill_table[dt].spell_fun) (dt, paf->level, ch, victim, TARGET_CHAR );
  return TRUE;
}


int regenerate_vtick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;

  if (gn != gen_regenerate)
    return -1;

  //do nothing if full hp
  if (ch->hit >= ch->max_hit)
    return TRUE;

  //cast cure light
  spell_cure_light(gsn_cure_light, paf->level, ch, ch, 0);

  if (--paf->modifier < 1)
    affect_strip(ch, gn );
  return TRUE;
}


/* tick update */
int temp_storm_tick(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag)
{
  //  AFFECT_DATA* paf = (AFFECT_DATA*) arg1;
  int upkeep = effect_table[GN_POS(gn)].value2[0];

  //ez
  if (gn != gen_temp_storm)
    return -1;

  if (ch->mana < upkeep)
    {
      send_to_char("You are no longer able to sustain your temporal storm.\n\r", ch);
      affect_strip(ch, gn);
      return FALSE;
    }
  else
    ch->mana -= upkeep;
  return TRUE;
}

int temp_storm_vupdate(CHAR_DATA* ch, void* arg1, void* arg2, int gn, int *int1, int *int2, int flag){
  AFFECT_DATA* paf = (AFFECT_DATA*) arg1, af;
  CHAR_DATA* vch, *vch_next;

  if (gn != gen_temp_storm)
    return -1;
  else if (ch->fighting == NULL)
    return FALSE;

  //cast on anyone that is in combat with us
  for (vch = ch->in_room->people; vch; vch = vch_next){
    vch_next = vch->next_in_room;

    if (vch->fighting != ch)
      continue;
    else if (is_affected(vch, gsn_temp_storm ))
      continue;

    if (saves_spell(paf->level, vch, DAM_OTHER, skill_table[gsn_temp_storm].spell_type))
      continue;

    af.type      = gsn_temp_storm;
    af.level     = paf->level;
    af.duration  = 4;
    af.where     = TO_AFFECTS;
    af.bitvector = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    affect_to_char( vch, &af );

    send_to_char("The temporal storm lashes out at you!\n\r", vch);
    act("The temporal storm lashes out at $n!", vch, NULL, vch, TO_ROOM);
    damage(ch, vch, number_range(paf->level, 2 * paf->level), gsn_temp_storm, DAM_OTHER, TRUE );
  }

  return TRUE;
}
