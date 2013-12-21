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
#include <ctype.h>
#include <time.h>
#include "merc.h"

char *                  mprog_type_to_name      args( ( int type ) );
ROOM_INDEX_DATA *       find_location   args( ( CHAR_DATA *ch, char *arg ) );

char last_mprog[MSL];

#ifdef DUNNO_STRSTR
char * strstr(s1,s2) const char *s1; const char *s2;
{
    char *cp;
    int i, j=strlen(s1)-strlen(s2), k=strlen(s2);
    if(j < 0)
        return NULL;
    for(i = 0; i <= j && strncmp(s1++,s2,k) != 0; i++);
        return (i>j) ? NULL : (s1-1);
}
#endif

char *  mprog_next_command args( ( char* clist ) );
bool    mprog_seval        args( ( char* lhs, char* opr, char* rhs ) );
bool    mprog_veval        args( ( int lhs, char* opr, int rhs ) );
bool    mprog_do_ifchck    args( ( char* ifchck, CHAR_DATA* mob, CHAR_DATA* actor, OBJ_DATA* obj, void* vo, CHAR_DATA* rndm ) );
char *  mprog_process_if   args( ( char* ifchck, char* com_list, CHAR_DATA* mob, CHAR_DATA* actor, OBJ_DATA* obj, void* vo, CHAR_DATA* rndm ) );
void    mprog_translate    args( ( char ch, char* t, CHAR_DATA* mob, CHAR_DATA* actor, OBJ_DATA* obj, void* vo, CHAR_DATA* rndm ) );
void    mprog_process_cmnd args( ( char* cmnd, CHAR_DATA* mob, CHAR_DATA* actor, OBJ_DATA* obj, void* vo, CHAR_DATA* rndm ) );
void    mprog_driver       args( ( char* com_list, CHAR_DATA* mob, CHAR_DATA* actor, OBJ_DATA* obj, void* vo ) );
void    death_cry          args( ( CHAR_DATA *ch , CHAR_DATA* killer) );

char *mprog_type_to_name( int type )
{
    switch ( type )
    {
    case IN_FILE_PROG:          return "in_file_prog";
    case ACT_PROG:              return "act_prog";
    case SPEECH_PROG:           return "speech_prog";
    case RAND_PROG:             return "rand_prog";
    case FIGHT_PROG:            return "fight_prog";
    case HITPRCNT_PROG:         return "hitprcnt_prog";
    case DEATH_PROG:            return "death_prog";
    case ENTRY_PROG:            return "entry_prog";
    case GREET_PROG:            return "greet_prog";
    case ALL_GREET_PROG:        return "all_greet_prog";
    case GIVE_PROG:             return "give_prog";
    case BRIBE_PROG:            return "bribe_prog";
    case GET_PROG:		return "get_prog";
    case TIME_PROG:		return "time_prog";
    case DROP_PROG:		return "drop_prog";
    case GUILD_PROG:		return "guild_prog";
    case BLOODY_PROG:		return "bloody_prog";
    case JUSTICE_PROG:		return "justice_prog";
    case TELL_PROG:             return "tell_prog";
    default:                    return "ERROR_PROG";
    }
}

char *mprog_next_command( char *clist )
{
    char *pointer = clist;
    while ( *pointer != '\n' && *pointer != '\0' )
        pointer++;
    if ( *pointer == '\n' )
        *pointer++ = '\0';
    if ( *pointer == '\r' )
        *pointer++ = '\0';
    return ( pointer );
}

/* These two functions do the basic evaluation of ifcheck operators.   *
 *  It is important to note that the string operations are not what    *
 *  you probably expect.  Equality is exact and division is substring. *
 *  remember that lhs has been stripped of leading space, but can      *
 *  still have trailing spaces so be careful when editing since:       *
 *  "guard" and "guard " are not equal.                                */
bool mprog_seval( char *lhs, char *opr, char *rhs )                    
{
    if ( !str_cmp( opr, "==" ) || !str_cmp(opr,"="))
        return ( bool )( !str_cmp( lhs, rhs ) );
    if ( !str_cmp( opr, "!=" ) )
        return ( bool )( str_cmp( lhs, rhs ) );
    if ( !str_cmp( opr, "/" ) )
        return ( bool )( !str_infix( rhs, lhs ) );
    if ( !str_cmp( opr, "!/" ) )
        return ( bool )( str_infix( rhs, lhs ) );
    bugf ( "Improper MOBprog operator opr: %s\n\r", opr);
    return 0;
}

bool mprog_veval( int lhs, char *opr, int rhs )
{
    if ( !str_cmp( opr, "==" ) || !str_cmp(opr,"="))
        return ( lhs == rhs );
    if ( !str_cmp( opr, "!=" ) )
        return ( lhs != rhs );
    if ( !str_cmp( opr, ">" ) )
        return ( lhs > rhs );
    if ( !str_cmp( opr, "<" ) )
        return ( lhs < rhs );
    if ( !str_cmp( opr, "<=" ) )
        return ( lhs <= rhs );
    if ( !str_cmp( opr, ">=" ) )
        return ( lhs >= rhs );
    if ( !str_cmp( opr, "&" ) )
        return ( lhs & rhs );
    if ( !str_cmp( opr, "|" ) )
        return ( lhs | rhs );
    bugf ( "Improper MOBprog operator opr: %s\n\r", opr);
    return 0;
}

/* This function performs the evaluation of the if checks.  It is     *
 * here that you can add any ifchecks which you so desire. Hopefully  *
 * it is clear from what follows how one would go about adding your   *
 * own. The syntax for an if check is: ifchck ( arg ) [opr val]       *
 * where the parenthesis are required and the opr and val fields are  *
 * optional but if one is there then both must be. The spaces are all *
 * optional. The evaluation of the opr expressions is farmed out      *
 * to reduce the redundancy of the mammoth if statement list.         *
 * If there are errors, then return -1 otherwise return boolean 1,0   */
bool mprog_do_ifchck( char *ifchck, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, CHAR_DATA *rndm)
{
    char buf[MIL], arg[MIL], opr[MIL], val[MIL], newbuf[MIL];
    CHAR_DATA *vict = (CHAR_DATA *) vo;
    OBJ_DATA *v_obj = (OBJ_DATA  *) vo;
    char *bufpt = buf, *argpt = arg, *oprpt = opr, *valpt = val, *point = ifchck;
    int lhsvl, rhsvl;
    if ( *point == '\0' ) 
    {
        bug ( "Mob: %d null ifchck", mob->pIndexData->vnum ); 
        return -1;
    }   
    while ( *point == ' ' )
        point++;
    while ( *point != '(' ) 
    {
        if ( *point == '\0' )
        {
            bug ( "Mob: %d ifchck syntax error", mob->pIndexData->vnum ); 
            return -1;
        }   
        else if ( *point == ' ' && str_prefix( "quest", buf ) )
            point++;
        else 
            *bufpt++ = *point++; 
    }
    *bufpt = '\0';
    point++;
    while ( *point != ')' ) 
    {
        if ( *point == '\0' )
        {
            bug ( "Mob: %d ifchck syntax error", mob->pIndexData->vnum ); 
            return -1;
        }   
        else if ( *point == ' ' )
            point++;
        else
            *argpt++ = *point++;
    }
    *argpt = '\0';
    point++;
    while ( *point == ' ' )
        point++;
    if ( *point == '\0' ) 
    {
      *opr = '\0';
      *val = '\0';
    }   
    else
    {
        while ( ( *point != ' ' ) && ( !isalnum( *point ) ) ) 
        {
            if ( *point == '\0' )
            {
                bug ( "Mob: %d ifchck operator without value",mob->pIndexData->vnum ); 
                return -1;
            }   
            else
                *oprpt++ = *point++; 
        }
        *oprpt = '\0';
        while ( *point == ' ' )
            point++;
        for( ; ; )
	{
            if ( ( *point != ' ' ) && ( *point == '\0' ) )
                break;
            else
                *valpt++ = *point++; 
	}
        *valpt = '\0';
    }
    bufpt = buf;
    argpt = arg;
    oprpt = opr;
    valpt = val;
    /* Ok... now buf contains the ifchck, arg contains the inside of the  *
     *  parentheses, opr contains an operator if one is present, and val  *
     *  has the value if an operator was present.                         *
     *  So.. basically use if statements and run over all known ifchecks  *
     *  Once inside, use the argument and expand the lhs. Then if need be *
     *  send the lhs,opr,rhs off to be evaluated.                         */
    if ( !str_cmp( buf, "rand" ) )
        return ( number_percent() <= atoi(arg) );
    if ( !str_cmp( buf, "ishome" ) )
    {
	if (mob->homevnum == mob->in_room->vnum)
	    return 1;
	else
	    return 0;
    }
    if ( !str_cmp( buf, "isindoor" ) )
    {
	if ( !IS_OUTSIDE(mob) )
	    return 1;
	else
	    return 0;
    }
    if ( !str_cmp( buf, "findmob" ) )
    {
	CHAR_DATA *rch;
	bool found = FALSE;
	for ( rch = mob->in_room->people; rch != NULL; rch = rch->next_in_room )
	    if ( IS_NPC(rch) && can_see( mob, rch ) && rch->pIndexData->vnum == atoi(arg) )
		found = TRUE;
	if (found)
	    return 1;
	else
	    return 0;
    }
    if ( !str_cmp( buf, "isweather" ) )
    {
	if (atoi(arg) == weather_info.sky)
	    return 1;
	else
	    return 0;
    }
    if ( !str_prefix( "quest", buf ) )
    {
	bufpt = one_argument(bufpt, newbuf);
	nlogf("%s %s", bufpt, buf);
        switch ( arg[1] )
	{
        case 'n': if ( actor && !IS_NPC(actor) ) return mprog_veval( atoi(val), opr, questnum(actor, bufpt ));
                  else return FALSE;
        case 't': if ( vict && !IS_NPC(vict) ) return mprog_veval( atoi(val), opr, questnum(vict, bufpt ));
                  else return FALSE;
        case 'r': if ( rndm && !IS_NPC(rndm) ) return mprog_veval( atoi(val), opr, questnum(rndm, bufpt ));
                  else return FALSE;
        default:  bug ( "Mob: %d bad argument to 'quest'", mob->pIndexData->vnum ); return 0;
        }
    }
    if ( !str_cmp( buf, "iscabal" ) )
        switch ( arg[1] )
	{
        case 'n': if ( actor && !IS_NPC(actor) ) return mprog_veval( atoi(val), opr, actor->cabal );
                  else return FALSE;
        case 't': if ( vict && !IS_NPC(vict) ) return mprog_veval( atoi(val), opr, vict->cabal );
                  else return FALSE;
        case 'r': if ( rndm && !IS_NPC(rndm) ) return mprog_veval( atoi(val), opr, rndm->cabal );
                  else return FALSE;
        default:  bug ( "Mob: %d bad argument to 'iscabal'", mob->pIndexData->vnum ); return 0;
        }
    if ( !str_cmp( buf, "isclass" ) )
        switch ( arg[1] )
        {
        case 'n': if ( actor && !IS_NPC(actor)) return ( atoi(val) == actor->class );
                  else return 0;
        case 't': if ( vict && !IS_NPC(vict)) return ( atoi(val) == vict->class );
                  else return 0;
        case 'r': if ( rndm && !IS_NPC(rndm)) return ( atoi(val) == rndm->class );
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'isclass'", mob->pIndexData->vnum ); return 0;
        }
    if ( !str_cmp( buf, "israce" ) )
        switch ( arg[1] )
        {
        case 'n': if ( actor && !IS_NPC(actor)) return ( atoi(val) == actor->race );
                  else return 0;
        case 't': if ( vict && !IS_NPC(vict)) return ( atoi(val) == vict->race );
                  else return 0;
        case 'r': if ( rndm && !IS_NPC(rndm)) return ( atoi(val) == rndm->race );
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'israce'", mob->pIndexData->vnum ); return 0;
        }
    if ( !str_cmp( buf, "roomobj" ) )
    {
	OBJ_DATA *obj;
	bool found = FALSE;
	for ( obj = mob->in_room->contents; obj != NULL; obj = obj->next_content )
	    if ( obj->pIndexData->vnum == atoi(arg) )
		found = TRUE;
	if (found)
	    return 1;
	else
	    return 0;
    }
    if ( !str_cmp( buf, "mobobj" ) )
    {
	OBJ_DATA *obj;
	bool found = FALSE;
	for ( obj = mob->carrying; obj != NULL; obj = obj->next_content )
	    if ( obj->pIndexData->vnum == atoi(arg) )
		found = TRUE;
	if (found)
	    return 1;
	else
	    return 0;
    }
    if ( !str_cmp( buf, "charwear" ) )
    {
	OBJ_DATA *obj;
	for ( obj = actor->carrying; obj != NULL; obj = obj->next_content )
	    if ( obj->pIndexData->vnum == atoi(arg) && obj->wear_loc != WEAR_NONE )
		return 1;
	return 0;
    }
    if ( !str_cmp( buf, "charobj" ) )
    {
	OBJ_DATA *obj;
	bool found = FALSE;
	for ( obj = actor->carrying; obj != NULL; obj = obj->next_content )
	    if ( obj->pIndexData->vnum == atoi(arg) )
		found = TRUE;
	if (found)
	    return 1;
	else
	    return 0;
    }
    if ( !str_cmp( buf, "ischar" ) )
    {
	if (rndm)
	    return 1;
	else
	    return 0;
    }
    if ( !str_cmp( buf, "isnghost") )
    {
        switch ( arg[1] )
	{
	case 'i': return 0;
        case 'n': if ( actor ) return ( !is_ghost( actor, 600 ) );
                  else return 1;
        case 't': if ( vict ) return ( !is_ghost( vict, 600 ) );
                  else return 1;
        case 'r': if ( rndm ) return ( !is_ghost( rndm,600 ) );
                  else return 1;
        default:  bug ( "Mob: %d bad argument to 'isnghost'", mob->pIndexData->vnum ); return 1;
	}
    }
    if ( !str_cmp( buf, "ispc" ) )
        switch ( arg[1] )
	{
	case 'i': return 0;
        case 'n': if ( actor ) return ( !IS_NPC( actor ) );
                  else return 0;
        case 't': if ( vict ) return ( !IS_NPC( vict ) );
                  else return 0;
        case 'r': if ( rndm ) return ( !IS_NPC( rndm ) );
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'ispc'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "isnpc" ) )
        switch ( arg[1] )
	{
	case 'i': return 1;
        case 'n': if ( actor ) return IS_NPC( actor );
	          else return 0;
        case 't': if ( vict ) return IS_NPC( vict );
	          else return 0;
        case 'r': if ( rndm ) return IS_NPC( rndm );
	          else return 0;
        default:  bug ("Mob: %d bad argument to 'isnpc'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "isawake" ) )
        switch ( arg[1] )
	{
	case 'i': return IS_AWAKE( mob );
        case 'n': if ( actor ) return IS_AWAKE( actor );
	          else return 0;
        case 't': if ( vict ) return IS_AWAKE( vict );
	          else return 0;
        case 'r': if ( rndm ) return IS_AWAKE( rndm );
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'isawake'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "isasleep" ) )
        switch ( arg[1] )
	{
	case 'i': return (!IS_AWAKE( mob ));
        case 'n': if ( actor ) return (!IS_AWAKE( actor ));
	          else return 0;
        case 't': if ( vict ) return (!IS_AWAKE( vict ));
	          else return 0;
        case 'r': if ( rndm ) return (!IS_AWAKE( rndm ));
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'isasleep'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "ischaotice" ) )
        switch ( arg[1] )
	{
        case 'n': if ( actor && !IS_NPC(actor)) return (actor->pcdata->ethos == ETHOS_CHAOTIC);
	          else return 0;
        case 't': if ( vict && !IS_NPC(vict)) return (vict->pcdata->ethos == ETHOS_CHAOTIC);
	          else return 0;
        case 'r': if ( rndm && !IS_NPC(rndm)) return (rndm->pcdata->ethos == ETHOS_CHAOTIC);
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'ischaotice'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "islawfule" ) )
        switch ( arg[1] )
	{
        case 'n': if ( actor && !IS_NPC(actor)) return (actor->pcdata->ethos == ETHOS_LAWFUL);
	          else return 0;
        case 't': if ( vict && !IS_NPC(vict)) return (vict->pcdata->ethos == ETHOS_LAWFUL);
	          else return 0;
        case 'r': if ( rndm && !IS_NPC(rndm)) return (rndm->pcdata->ethos == ETHOS_LAWFUL);
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'islawfule'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "isneutrale" ) )
        switch ( arg[1] )
	{
        case 'n': if ( actor && !IS_NPC(actor)) return (actor->pcdata->ethos == ETHOS_NEUTRAL);
	          else return 0;
        case 't': if ( vict && !IS_NPC(vict)) return (vict->pcdata->ethos == ETHOS_NEUTRAL);
	          else return 0;
        case 'r': if ( rndm && !IS_NPC(rndm)) return (rndm->pcdata->ethos == ETHOS_NEUTRAL);
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'isneutrale'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "istrueneutral" ) )
        switch ( arg[1] )
	{
        case 'n': if ( actor && !IS_NPC(actor)) return (actor->pcdata->ethos == ETHOS_NEUTRAL && IS_NEUTRAL(actor));
	          else return 0;
        case 't': if ( vict && !IS_NPC(vict)) return (vict->pcdata->ethos == ETHOS_NEUTRAL && IS_NEUTRAL(vict));
	          else return 0;
        case 'r': if ( rndm && !IS_NPC(rndm)) return (rndm->pcdata->ethos == ETHOS_NEUTRAL && IS_NEUTRAL(rndm));
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'istrueneutral'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "isgood" ) )
        switch ( arg[1] )
	{
	case 'i': return IS_GOOD( mob );
        case 'n': if ( actor ) return IS_GOOD( actor );
	          else return 0;
        case 't': if ( vict ) return IS_GOOD( vict );
	          else return 0;
        case 'r': if ( rndm ) return IS_GOOD( rndm );
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'isgood'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "isevil" ) )
        switch ( arg[1] )
	{
	case 'i': return IS_EVIL( mob );
        case 'n': if ( actor ) return IS_EVIL( actor );
	          else return 0;
        case 't': if ( vict ) return IS_EVIL( vict );
	          else return 0;
        case 'r': if ( rndm ) return IS_EVIL( rndm );
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'isevil", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "neutral" ) )
        switch ( arg[1] )
	{
	case 'i': return IS_NEUTRAL( mob );
        case 'n': if ( actor ) return IS_NEUTRAL( actor );
	          else return 0;
        case 't': if ( vict ) return IS_NEUTRAL( vict );
	          else return 0;
        case 'r': if ( rndm ) return IS_NEUTRAL( rndm );
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'isneutral", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "isfight" ) )
        switch ( arg[1] )
	{
	case 'i': return ( mob->fighting ) ? 1 : 0;
        case 'n': if ( actor ) return ( actor->fighting ) ? 1 : 0;
	          else return 0;
        case 't': if ( vict ) return ( vict->fighting ) ? 1 : 0;
	          else return 0;
        case 'r': if ( rndm ) return ( rndm->fighting ) ? 1 : 0;
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'isfight'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "isimmort" ) )
        switch ( arg[1] )
	{
	case 'i': return ( get_trust( mob ) > LEVEL_IMMORTAL && !mob->invis_level);
        case 'n': if ( actor ) return ( get_trust( actor ) > LEVEL_IMMORTAL && !actor->invis_level);
  	          else return 0;
        case 't': if ( vict ) return ( get_trust( vict ) > LEVEL_IMMORTAL && !vict->invis_level);
                  else return 0;
        case 'r': if ( rndm ) return ( get_trust( rndm ) > LEVEL_IMMORTAL && !rndm->invis_level);
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'isimmort'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "ischarmed" ) )
        switch ( arg[1] )
	{
        case 'i': if (IS_AFFECTED( mob, AFF_CHARM )) return TRUE;
                  else return FALSE;
        case 'n': if ( actor )
                      if (IS_AFFECTED( actor, AFF_CHARM )) return TRUE;
                      else return FALSE;
	          else return 0;
	case 't': if ( vict )
                      if (IS_AFFECTED( actor, AFF_CHARM )) return TRUE;
                      else return FALSE;
	          else return 0;
	case 'r': if ( rndm )
                      if (IS_AFFECTED( actor, AFF_CHARM )) return TRUE;
                      else return FALSE;
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'ischarmed'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "isfollow" ) )
        switch ( arg[1] )
	{
        case 'i': return ( mob->master != NULL && mob->master->in_room == mob->in_room );
        case 'n': if ( actor ) return ( actor->master != NULL && actor->master->in_room == actor->in_room );
	          else return 0;
        case 't': if ( vict ) return ( vict->master != NULL && vict->master->in_room == vict->in_room );
	          else return 0;
        case 'r': if ( rndm ) return ( rndm->master != NULL && rndm->master->in_room == rndm->in_room );
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'isfollow'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "isaffected" ) )
        switch ( arg[1] )
	{
	case 'i': return ( mob->affected != NULL );
        case 'n': if ( actor ) return ( actor->affected != NULL );
	          else return 0;
        case 't': if ( vict ) return ( vict->affected != NULL);
	          else return 0;
        case 'r': if ( rndm ) return ( rndm->affected != NULL );
	          else return 0;
        default:  bug ( "Mob: %d bad argument to 'isaffected'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "hitprcnt" ) )
        switch ( arg[1] )
	{
	case 'i': lhsvl = (mob->hit * 100)/ mob->max_hit;
	          rhsvl = atoi( val );
         	  return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
                  {
                      lhsvl = (actor->hit * 100)/ actor->max_hit;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else return 0;
	case 't': if ( vict )
	          {
                      lhsvl = (vict->hit * 100)/ vict->max_hit;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 'r': if ( rndm )
	          {
                      lhsvl = (rndm->hit * 100)/ rndm->max_hit;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'hitprcnt'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "inroom" ) )
        switch ( arg[1] )
	{
	case 'i': lhsvl = mob->in_room->vnum;
	          rhsvl = atoi(val);
	          return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
                      lhsvl = actor->in_room->vnum;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 't': if ( vict )
	          {
                      lhsvl = vict->in_room->vnum;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 'r': if ( rndm )
	          {
                      lhsvl = rndm->in_room->vnum;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'inroom'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "sex" ) )
        switch ( arg[1] )
	{
	case 'i': lhsvl = mob->sex;
	          rhsvl = atoi( val );
	          return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
                      lhsvl = actor->sex;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 't': if ( vict )
	          {
                      lhsvl = vict->sex;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 'r': if ( rndm )
	          {
                      lhsvl = rndm->sex;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'sex'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "position" ) )
        switch ( arg[1] )
	{
	case 'i': lhsvl = mob->position;
	          rhsvl = atoi( val );
	          return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
                      lhsvl = actor->position;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 't': if ( vict )
	          {
                      lhsvl = vict->position;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 'r': if ( rndm )
	          {
                      lhsvl = rndm->position;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'position'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "istime" ) )
    {
	int temp;
	temp = time_info.hour;
	lhsvl = atoi(arg);
	rhsvl = atoi(val);
	if ( lhsvl <= rhsvl )
	    return ( (lhsvl <= temp) && (temp <= rhsvl ) );
	else
	    return ( ( (lhsvl <= temp) && (rhsvl <= temp ) ) || ( ( lhsvl >= temp && rhsvl >= temp ) ) );
    }

    if ( !str_cmp( buf, "level" ) )
        switch ( arg[1] )
	{
	case 'i': lhsvl = mob->level;
	          rhsvl = atoi( val );
	          return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
                      lhsvl = actor->level;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 't': if ( vict )
	          {
                      lhsvl = vict->level;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 'r': if ( rndm )
	          {
                      lhsvl = rndm->level;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'level'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "class" ) )
        switch ( arg[1] )
	{
	case 'i': lhsvl = mob->class;
	          rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
                      lhsvl = actor->class;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 't': if ( vict )
	          {
                      lhsvl = vict->class;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 'r': if ( rndm )
	          {
                      lhsvl = rndm->class;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'class'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "goldamt" ) )
        switch ( arg[1] )
	{
	case 'i': lhsvl = mob->gold;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
	          {
                      lhsvl = actor->gold;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 't': if ( vict )
	          {
                      lhsvl = vict->gold;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 'r': if ( rndm )
	          {
                      lhsvl = rndm->gold;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'goldamt'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "objtype" ) )
        switch ( arg[1] )
	{
	case 'o': if ( obj )
	          {
                      lhsvl = obj->item_type;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                 else return 0;
	case 'p': if ( v_obj )
	          {
                      lhsvl = v_obj->item_type;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
	          else
                      return 0;
        default:  bug ( "Mob: %d bad argument to 'objtype'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "objval0" ) )
        switch ( arg[1] )
	{
	case 'o': if ( obj )
	          {
                      lhsvl = obj->value[0];
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else return 0;
	case 'p': if ( v_obj )
	          {
                      lhsvl = v_obj->value[0];
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default: bug ( "Mob: %d bad argument to 'objval0'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "objval1" ) )
        switch ( arg[1] )
	{
	case 'o': if ( obj )
	          {
                      lhsvl = obj->value[1];
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 'p': if ( v_obj )
	          {
                      lhsvl = v_obj->value[1];
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default: bug ( "Mob: %d bad argument to 'objval1'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "objval2" ) )
        switch ( arg[1] )
	{
	case 'o': if ( obj )
	          {
                      lhsvl = obj->value[2];
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 'p': if ( v_obj )
	          {
                      lhsvl = v_obj->value[2];
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default: bug ( "Mob: %d bad argument to 'objval2'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "objval3" ) )
        switch ( arg[1] )
	{
	case 'o': if ( obj )
	          {
                      lhsvl = obj->value[3];
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 'p': if ( v_obj ) 
	          {
                      lhsvl = v_obj->value[3];
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'objval3'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "number" ) )
        switch ( arg[1] )
	{
	case 'i': lhsvl = mob->gold;
	          rhsvl = atoi( val );
	          return mprog_veval( lhsvl, opr, rhsvl );
	case 'n': if ( actor )
		  {
                      if IS_NPC( actor )
                      {
                          lhsvl = actor->pIndexData->vnum;
                          rhsvl = atoi( val );
                          return mprog_veval( lhsvl, opr, rhsvl );
                      }
		  }
                  else return 0;
	case 't': if ( vict )
		  {
                      if IS_NPC( vict )
                      {
                          lhsvl = vict->pIndexData->vnum;
                          rhsvl = atoi( val );
                          return mprog_veval( lhsvl, opr, rhsvl );
                      }
		  }
                  else return 0;
	case 'r': if ( rndm )
		  {
                      if IS_NPC( rndm )
                      {
                          lhsvl = rndm->pIndexData->vnum;
                          rhsvl = atoi( val );
                          return mprog_veval( lhsvl, opr, rhsvl );
                      }
		  }
                 else return 0;
	case 'o': if ( obj )
	          {
                      lhsvl = obj->pIndexData->vnum;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
	case 'p': if ( v_obj )
	          {
                      lhsvl = v_obj->pIndexData->vnum;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
		  }
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'number'", mob->pIndexData->vnum ); return 0;
	}
    if ( !str_cmp( buf, "name" ) )
        switch ( arg[1] )
	{
	case 'i': return mprog_seval( mob->name, opr, val );
        case 'n': if ( actor ) return mprog_seval( actor->name, opr, val );
                  else return 0;
        case 't': if ( vict ) return mprog_seval( vict->name, opr, val );
                  else return 0;
        case 'r': if ( rndm ) return mprog_seval( rndm->name, opr, val );
                  else return 0;
        case 'o': if ( obj ) return mprog_seval( obj->name, opr, val );
                  else return 0;
        case 'p': if ( v_obj ) return mprog_seval( v_obj->name, opr, val );
                  else return 0;
        default:  bug ( "Mob: %d bad argument to 'name'", mob->pIndexData->vnum ); return 0;
	}
    bug ( "Mob: %d unknown ifchck", mob->pIndexData->vnum ); 
    return 0;
}

/* Quite a long and arduous function, this guy handles the control      *
 * flow part of MOBprograms.  Basicially once the driver sees an        *
 * 'if' attention shifts to here.  While many syntax errors are         *
 * caught, some will still get through due to the handling of break     *
 * and errors in the same fashion.  The desire to break out of the      *
 * recursion without catastrophe in the event of a mis-parse was        *
 * believed to be high. Thus, if an error is found, it is bugged and    *
 * the parser acts as though a break were issued and just bails out     *
 * at that point. I havent tested all the possibilites, so I'm speaking *
 * in theory, but it is 'guaranteed' to work on syntactically correct   *
 * MOBprograms, so if the mud crashes here, check the mob carefully!    */
char *mprog_process_if( char *ifchck, char *com_list, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, CHAR_DATA *rndm )
{
    char null[1], buf[MIL];
    char *morebuf = '\0', *cmnd = '\0';
    bool loopdone = FALSE, flag = FALSE;
    int legal;
    null[0] = '\0';
    if ( ( legal = mprog_do_ifchck( ifchck, mob, actor, obj, vo, rndm ) ) )
    {
        if ( legal == 1 )
            flag = TRUE;
        else
            return NULL;
    }
    while( loopdone == FALSE )
    {
        cmnd     = com_list;
        com_list = mprog_next_command( com_list );
        while ( *cmnd == ' ' )
            cmnd++;
        if ( *cmnd == '\0' )
        {
            bug ( "Mob: %d no commands after IF/OR", mob->pIndexData->vnum ); 
            return NULL;
        }
        morebuf = one_argument( cmnd, buf );
        if ( !str_cmp( buf, "or" ) )
        {
            if ( ( legal = mprog_do_ifchck( morebuf,mob,actor,obj,vo,rndm ) ) )
            {
                if ( legal == 1 )
                    flag = TRUE;
                else
                    return NULL;
            }
        }
        else
            loopdone = TRUE;
    }
    if ( flag )
        for ( ; ; )
        {
            if ( !str_cmp( buf, "if" ) )
            { 
                com_list = mprog_process_if(morebuf,com_list,mob,actor,obj,vo,rndm);
		if (com_list == NULL)
		  com_list = null;
                while ( *cmnd==' ' )
                    cmnd++;
                if ( *com_list == '\0' )
                    return NULL;
                cmnd     = com_list;
                com_list = mprog_next_command( com_list );
                morebuf  = one_argument( cmnd,buf );
                continue;
            }
            if ( !str_cmp( buf, "break" ) )
                return NULL;
            if ( !str_cmp( buf, "endif" ) )
                return com_list; 
            if ( !str_cmp( buf, "else" ) ) 
            {
                while ( str_cmp( buf, "endif" ) ) 
                {
                    cmnd     = com_list;
                    com_list = mprog_next_command( com_list );
		    if (com_list == NULL)
		      com_list = null;
                    while ( *cmnd == ' ' )
                        cmnd++;
                    if ( *cmnd == '\0' )
                    {
                        bug ( "Mob: %d missing endif after else", mob->pIndexData->vnum );
                        return NULL;
                    }
                    morebuf = one_argument( cmnd,buf );
                }
                return com_list; 
            }
            mprog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );
            cmnd     = com_list;
            com_list = mprog_next_command( com_list );
	    if (com_list == NULL)
	      com_list = null;
            while ( *cmnd == ' ' )
                cmnd++;
            if ( *cmnd == '\0' )
            {
                bug ( "Mob: %d missing else or endif", mob->pIndexData->vnum ); 
                return NULL;
            }
            morebuf = one_argument( cmnd, buf );
        }
        else
        {
            while ( ( str_cmp( buf, "else" ) ) && ( str_cmp( buf, "endif" ) ) )
            {
                cmnd     = com_list;
                com_list = mprog_next_command( com_list );
		if (com_list == NULL)
		  com_list = null;
                while ( *cmnd == ' ' )
                    cmnd++;
                if ( *cmnd == '\0' )
                {
                    bug ( "Mob: %d missing an else or endif", mob->pIndexData->vnum ); 
                    return NULL;
                }
                morebuf = one_argument( cmnd, buf );
            }
            if ( !str_cmp( buf, "endif" ) )
                return com_list;
            cmnd     = com_list;
            com_list = mprog_next_command( com_list );
	    if (com_list == NULL)
	      com_list = null;
            while ( *cmnd == ' ' )
                cmnd++;
            if ( *cmnd == '\0' )
            { 
                bug ( "Mob: %d missing endif", mob->pIndexData->vnum ); 
                return NULL;
            }
            morebuf = one_argument( cmnd, buf );
            for ( ; ; )
            {
                if ( !str_cmp( buf, "if" ) )
                { 
                    com_list = mprog_process_if( morebuf, com_list, mob, actor, obj, vo, rndm );
		    if (com_list == NULL)
		      com_list = null;
                    while ( *cmnd == ' ' )
                        cmnd++;
                    if ( *com_list == '\0' )
                        return NULL;
                    cmnd     = com_list;
                    com_list = mprog_next_command( com_list );
                    morebuf  = one_argument( cmnd,buf );
                    continue;
                }
            if ( !str_cmp( buf, "else" ) ) 
            {
                bug ( "Mob: %d found else in an else section", mob->pIndexData->vnum ); 
                return NULL;
            }
            if ( !str_cmp( buf, "break" ) )
                return NULL;
            if ( !str_cmp( buf, "endif" ) )
                return com_list; 
            mprog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );
            cmnd     = com_list;
            com_list = mprog_next_command( com_list );
	    if (com_list == NULL)
	      com_list = null;
            while ( *cmnd == ' ' )
                cmnd++;
            if ( *cmnd == '\0' )
            {
                bug ( "Mob:%d missing endif in else section", mob->pIndexData->vnum ); 
                return NULL;
            }
            morebuf = one_argument( cmnd, buf );
        }
    }
}

/* This routine handles the variables for command expansion.        *
 * If you want to add any go right ahead, it should be fairly       *
 * clear how it is done and they are quite easy to do, so you       *
 * can be as creative as you want. The only catch is to check       *
 * that your variables exist before you use them. At the moment,    *
 * using $t when the secondary target refers to an object           *
 * i.e. >prog_act drops~<nl>if ispc($t)<nl>sigh<nl>endif<nl>~<nl>   *
 * probably makes the mud crash (vice versa as well) The cure       *
 * would be to change act() so that vo becomes vict & v_obj.        *
 * but this would require a lot of small changes all over the code. */
void mprog_translate( char ch, char *t, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, CHAR_DATA *rndm )
{
    static char *he_she        [] = { "it",  "he",  "she" };
    static char *him_her       [] = { "it",  "him", "her" };
    static char *his_her       [] = { "its", "his", "her" };
    CHAR_DATA   *vict             = (CHAR_DATA *) vo;
    OBJ_DATA    *v_obj            = (OBJ_DATA  *) vo;
    *t = '\0';
    switch ( ch )
    {
    case 'i': one_argument( mob->name, t ); break;
    case 'I': strcpy( t, mob->short_descr ); break;
    case 'n': if ( actor )
              {
                  one_argument( actor->name, t );
                  if ( !IS_NPC( actor ) )
                      *t = UPPER( *t );
              }
              break;
    case 'N': if ( actor ) 
              {
                  if ( IS_NPC( actor ) )
                      strcpy( t, PERS2(actor) );
                  else
                      strcpy( t, PERS2(actor) );
              }
              break;
    case 't': if ( vict )
              {
                  one_argument( vict->name, t );
                  if ( !IS_NPC( vict ) )
                      *t = UPPER( *t );
              }
              break;
    case 'T': if ( vict ) 
              {
                  if ( IS_NPC( vict ) )
                      strcpy( t, PERS2(vict) );
                  else
                      strcpy( t, PERS2(vict) );
              }
              break;
    case 'r': if ( rndm )
              {
                  one_argument( rndm->name, t );
                  if ( !IS_NPC( rndm ) )
                      *t = UPPER( *t );
              }
              break;
    case 'R': if ( rndm ) 
              {
                  if ( IS_NPC( rndm ) )
                      strcpy(t,PERS2(rndm) );
                  else
		    strcpy( t, PERS2(rndm) );
              }
              break;
    case 'e': if ( actor ) can_see( mob, actor ) ? strcpy( t, he_she[ actor->sex ] ) : strcpy( t, "someone" ); break;
    case 'm': if ( actor ) can_see( mob, actor ) ? strcpy( t, him_her[ actor->sex ] ) : strcpy( t, "someone" ); break;
    case 's': if ( actor ) can_see( mob, actor ) ? strcpy( t, his_her[ actor->sex ] ) : strcpy( t, "someone's" ); break;
    case 'E': if ( vict )  can_see( mob, vict ) ? strcpy( t, he_she[ vict->sex ] ) : strcpy( t, "someone" ); break;
    case 'M': if ( vict )  can_see( mob, vict ) ? strcpy( t, him_her[ vict->sex ] ) : strcpy( t, "someone" ); break;
    case 'S': if ( vict )  can_see( mob, vict ) ? strcpy( t, his_her[ vict->sex ] ) : strcpy( t, "someone's" ); break;
    case 'j': strcpy( t, he_she[ mob->sex ] ); break;
    case 'k': strcpy( t, him_her[ mob->sex ] ); break;
    case 'l': strcpy( t, his_her[ mob->sex ] ); break;
    case 'J': if ( rndm )  can_see( mob, rndm ) ? strcpy( t, he_she[ rndm->sex ] ) : strcpy( t, "someone" ); break;
    case 'K': if ( rndm )  can_see( mob, rndm ) ? strcpy( t, him_her[ rndm->sex ] ) : strcpy( t, "someone" ); break;
    case 'L': if ( rndm )  can_see( mob, rndm ) ? strcpy( t, his_her[ rndm->sex ] ) : strcpy( t, "someone's" ); break;
    case 'o': if ( obj )   can_see_obj( mob, obj ) ? one_argument( obj->name, t ) : strcpy( t, "something" ); break;
    case 'O': if ( obj )   can_see_obj( mob, obj ) ? strcpy( t, obj->short_descr ) : strcpy( t, "something" ); break;
    case 'p': if ( v_obj ) can_see_obj( mob, v_obj ) ? one_argument( v_obj->name, t ) : strcpy( t, "something" ); break;
    case 'P': if ( v_obj ) can_see_obj( mob, v_obj ) ? strcpy( t, v_obj->short_descr ) : strcpy( t, "something" ); break;
    case 'a': if ( obj )
                  switch ( *( obj->name ) )
                  {
                  case 'a': case 'e': case 'i': case 'o': case 'u': strcpy( t, "an" ); break;
                  default: strcpy( t, "a" );
                  }
              break;
    case 'A': if ( v_obj ) 
                  switch ( *( v_obj->name ) )
                  {
                  case 'a': case 'e': case 'i': case 'o': case 'u': strcpy( t, "an" ); break;
                  default: strcpy( t, "a" );
                  }
              break;
    case '$': strcpy( t, "$" ); break;
    default:  bug( "Mob: %d bad $var", mob->pIndexData->vnum ); break;
    }
    return;
}

/* This procedure simply copies the cmnd to a buffer while expanding *
 * any variables by calling the translate procedure.  The observant  *
 * code scrutinizer will notice that this is taken from act()        */
void mprog_process_cmnd( char *cmnd, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, CHAR_DATA *rndm )
{
    char buf[MIL], tmp[MIL];
    char *str, *i;
    char *point;
    point = buf;
    str = cmnd;
    while ( *str != '\0' )
    {
        if ( *str != '$' )
        {
            *point++ = *str++;
            continue;
        }
        str++;
        mprog_translate( *str, tmp, mob, actor, obj, vo, rndm );
        i = tmp;
        ++str;
        while ( ( *point = *i ) != '\0' )
            ++point, ++i;
    }
    *point = '\0';
    interpret( mob, buf );
}

/* The main focus of the MOBprograms.  This routine is called        *
 *  whenever a trigger is successful.  It is responsible for parsing *
 *  the command list and figuring out what to do. However, like all  *
 *  complex procedures, everything is farmed out to the other guys.  */
void mprog_driver ( char *com_list, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo)
{
    char tmpcmndlst[MSL], buf[MIL];
    char null = '\0';
    char *morebuf, *command_list, *cmnd;
    CHAR_DATA *rndm = NULL, *vch = NULL;
    int count = 0;
    char *strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    sprintf (last_mprog, "Mobprog: [%5d] %s in [%5d] %s: (%s)",
      mob->pIndexData->vnum, mob->short_descr, mob->in_room ? mob->in_room->vnum : 0,
      mob->in_room ? mob->in_room->name : "(not in a room)", strtime);
    if (mob->pIndexData->mobprogs->dowhen == 0 && ( mob->in_room == NULL || mob->in_room->vnum != mob->homevnum ) )
	return;
    /* switched */
    if (IS_NPC(mob) && mob->desc)
      return;
    if IS_AFFECTED( mob, AFF_CHARM )
        return;
    if (actor && IS_IMMORTAL(actor) && (actor->invis_level || actor->incog_level))
	return;
    if ( (mob && mob->in_room == NULL) || (actor && actor->in_room == NULL))
	return;
    REMOVE_BIT(mob->comm,COMM_NOCHANNELS);
    REMOVE_BIT(mob->comm,COMM_NOYELL);
    REMOVE_BIT(mob->comm,COMM_NOTELL);
    mob->trust = 6969;
    for ( vch = mob->in_room->people; vch; vch = vch->next_in_room )
        if ( can_see( mob, vch ) && vch != mob && !vch->invis_level)
        {
            if ( number_range( 0, count ) == 0 )
                rndm = vch;
            count++;
        }
    if (rndm == NULL)
	rndm = mob;
    strcpy( tmpcmndlst, com_list );
    command_list = tmpcmndlst;
    cmnd         = command_list;
    if (command_list == NULL)
      command_list = &null;
    command_list = mprog_next_command( command_list );
    while ( *cmnd != '\0' )
    {
        morebuf = one_argument( cmnd, buf );
        if ( !str_cmp( buf, "if" ) )
            command_list = mprog_process_if( morebuf, command_list, mob, actor, obj, vo, rndm );
        else
            mprog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );
	if (command_list == NULL)
	  command_list = &null;
        cmnd         = command_list;
        command_list = mprog_next_command( command_list );
    }
    SET_BIT(mob->comm,COMM_NOCHANNELS);
    SET_BIT(mob->comm,COMM_NOYELL);
    SET_BIT(mob->comm,COMM_NOTELL);
    mob->trust = mob->level;
    sprintf (last_mprog, "Mobprog: (Finished) [%5d] %s in [%5d] %s: (%s)",
      mob->pIndexData->vnum, mob->short_descr, mob->in_room ? mob->in_room->vnum : 0,
      mob->in_room ? mob->in_room->name : "(not in a room)", strtime);
}

/* The next two routines are the basic trigger types. Either trigger *
 *  on a certain percent, or trigger on a keyword or word phrase.    *
 *  To see how this works, look at the various trigger routines..    */
void mprog_wordlist_check( char *arg, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int type )
{
    char temp1[MSL], temp2[MIL], word[MIL];
    MPROG_DATA *mprg;
    char *list, *start, *dupl, *end;
    int i;
    for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
        if ( mprg->type & type )
        {
            strcpy( temp1, mprg->arglist );
            list = temp1;
            for ( i = 0; i < strlen( list ); i++ )
                list[i] = LOWER( list[i] );
            strcpy( temp2, arg );
            dupl = temp2;
            for ( i = 0; i < strlen( dupl ); i++ )
                dupl[i] = LOWER( dupl[i] );
            if ( ( list[0] == 'p' ) && ( list[1] == ' ' ) )
            {
                list += 2;
                while ( ( start = strstr( dupl, list ) ) )
                {
                    if ( (start == dupl || *(start-1) == ' ' )
                    && ( *(end = start + strlen( list ) ) == ' ' || *end == '\n' || *end == '\r' || *end == '\0' ) )
                    {
                        mprog_driver( mprg->comlist, mob, actor, obj, vo );
                        break;
                    }
                    else
                        dupl = start+1;
                }
            }
            else
            {
                list = one_argument( list, word );
                for( ; word[0] != '\0'; list = one_argument( list, word ) )
                    while ( ( start = strstr( dupl, word ) ) )
                    {
                        if ( ( start == dupl || *(start-1) == ' ' )
                        && ( *(end = start + strlen( word ) ) == ' ' || *end == '\n' || *end == '\r' || *end == '\0' ) )
                        {
                            mprog_driver( mprg->comlist, mob, actor, obj, vo );
                            break;
                        }
                        else
                            dupl = start+1;
                    }
            }
        }
}

void mprog_percent_check( CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int type)
{
    MPROG_DATA * mprg;
    for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
        if ( ( mprg->type & type ) && ( number_percent( ) < atoi( mprg->arglist ) ) )
        {
            mprog_driver( mprg->comlist, mob, actor, obj, vo );
            if ( type != GREET_PROG && type != ALL_GREET_PROG )
                break;
        }
}

/* The triggers.. These are really basic, and since most appear only       *
 * once in the code (hmm. i think they all do) it would be more efficient  *
 * to substitute the code in and make the mprog_xxx_check routines global. *
 * However, they are all here in one nice place at the moment to make it   *
 * easier to see what they look like. If you do substitute them back in,   *
 * make sure you remember to modify the variable names to the ones in the  *
 * trigger calls.                                                          */
void mprog_bribe_trigger( CHAR_DATA *mob, CHAR_DATA *ch, int amount )
{
    char buf[MSL];
    MPROG_DATA *mprg;
    OBJ_DATA *obj;
    if ( IS_AWAKE(mob) && IS_NPC( mob ) && ( mob->pIndexData->progtypes & BRIBE_PROG ) )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_GOLD_SOME ), 0);
        sprintf( buf, obj->short_descr, amount );
        free_string( obj->short_descr );
        obj->short_descr = str_dup( buf );
        obj->value[0] = amount;
        obj_to_char( obj, mob );
        mob->gold -= amount;
        for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
            if ( ( mprg->type & BRIBE_PROG ) && ( amount >= atoi( mprg->arglist ) ) && mob->position != POS_FIGHTING )
            {
                mprog_driver( mprg->comlist, mob, ch, obj, NULL );
                break;
            }
    }
}

void mprog_time_trigger( CHAR_DATA *mob )
{
    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & TIME_PROG ) && mob->position != POS_FIGHTING )
        mprog_percent_check( mob, NULL, NULL, NULL, TIME_PROG );
}

void mprog_death_trigger( CHAR_DATA *ch, CHAR_DATA *mob )
{
    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & DEATH_PROG ) )
        mprog_percent_check( mob, ch, NULL, NULL, DEATH_PROG );
    if (!IS_SET(mob->form,FORM_INSTANT_DECAY))
	death_cry( mob , ch );
}

void mprog_entry_trigger( CHAR_DATA *mob )
{
    if ( IS_AWAKE(mob) && IS_NPC( mob ) && ( mob->pIndexData->progtypes & ENTRY_PROG ) && mob->position != POS_FIGHTING)
        mprog_percent_check( mob, NULL, NULL, NULL, ENTRY_PROG );
}

void mprog_fight_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & FIGHT_PROG ) )
        mprog_percent_check( mob, ch, NULL, NULL, FIGHT_PROG );
}

void mprog_give_trigger( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( IS_AWAKE(mob) && IS_NPC( mob ) && ( mob->pIndexData->progtypes & GIVE_PROG ) && mob->position != POS_FIGHTING )
        mprog_percent_check( mob, ch, obj, NULL, GIVE_PROG );
}

void mprog_get_trigger( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( IS_AWAKE(mob) && IS_NPC( mob ) && ( mob->pIndexData->progtypes & GET_PROG ) && mob->position != POS_FIGHTING )
        mprog_percent_check( mob, ch, obj, NULL, GET_PROG );
}

void mprog_drop_trigger( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( IS_AWAKE(mob) && IS_NPC( mob ) && ( mob->pIndexData->progtypes & DROP_PROG ) && mob->position != POS_FIGHTING)
        mprog_percent_check( mob, ch, obj, NULL, DROP_PROG );
}

void mprog_greet_trigger( CHAR_DATA *ch )
{
    CHAR_DATA *vmob, *vnext;
    for ( vmob = ch->in_room->people; vmob != NULL; vmob = vnext )
    {
        vnext = vmob->next_in_room;
        if ( IS_NPC( vmob ) && ch != vmob && can_see( vmob, ch ) && ( vmob->fighting == NULL )
	&& vmob->position != POS_FIGHTING && IS_AWAKE( vmob ) && ( vmob->pIndexData->progtypes & GREET_PROG) )
            mprog_percent_check( vmob, ch, NULL, NULL, GREET_PROG );
        else if ( IS_NPC( vmob ) && ( vmob->fighting == NULL ) && IS_AWAKE( vmob )
        && ( vmob->pIndexData->progtypes & ALL_GREET_PROG ) )
            mprog_percent_check(vmob,ch,NULL,NULL,ALL_GREET_PROG);
    }
}

void mprog_hitprcnt_trigger( CHAR_DATA *mob, CHAR_DATA *ch)
{
    MPROG_DATA *mprg;
    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & HITPRCNT_PROG ) )
    for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
        if ( ( mprg->type & HITPRCNT_PROG ) && ( ( 100*mob->hit / mob->max_hit ) < atoi( mprg->arglist ) ) )
        {
            mprog_driver( mprg->comlist, mob, ch, NULL, NULL );
            break;
        }
}

void mprog_random_trigger( CHAR_DATA *mob )
{
    if ( mob->pIndexData->progtypes & RAND_PROG && mob->position != POS_FIGHTING )
        mprog_percent_check(mob,NULL,NULL,NULL,RAND_PROG);
}

void mprog_act_trigger( char *buf, CHAR_DATA *ch, CHAR_DATA *mob, OBJ_DATA *obj, void *vo)
{
    CHAR_DATA *vmob;
    if (mob->in_room == NULL)
	return;
    for ( vmob = mob->in_room->people; vmob != NULL; vmob = vmob->next_in_room )
        if ( IS_AWAKE(vmob) && IS_NPC( vmob ) && ( vmob->pIndexData->progtypes & ACT_PROG ) )
            mprog_wordlist_check( buf, vmob, mob, NULL, NULL, ACT_PROG );
}

void mprog_speech_trigger( char *txt, CHAR_DATA *ch, CHAR_DATA *mob )
{
    if ( IS_AWAKE(mob) && IS_NPC( mob ) && ( mob->pIndexData->progtypes & SPEECH_PROG ) )
        mprog_wordlist_check( txt, mob, ch, NULL, NULL, SPEECH_PROG );
}

void mprog_tell_trigger( char *txt, CHAR_DATA *mob )
{
    CHAR_DATA *vmob;
    for ( vmob = mob->in_room->people; vmob != NULL; vmob = vmob->next_in_room )
        if ( IS_AWAKE(vmob) && IS_NPC( vmob ) && ( vmob->pIndexData->progtypes & TELL_PROG ) )
            mprog_wordlist_check( txt, vmob, mob, NULL, NULL, TELL_PROG );
}

void mprog_guild_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & GUILD_PROG )  )
        mprog_percent_check( mob, ch, NULL, NULL, GUILD_PROG );
}

void mprog_bloody_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & BLOODY_PROG )  )
        mprog_percent_check( mob, ch, NULL, NULL, BLOODY_PROG );
}

void mprog_justice_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & JUSTICE_PROG )  )
        mprog_percent_check( mob, ch, NULL, NULL, JUSTICE_PROG );
}

void do_mpstat( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    MPROG_DATA *mprg;
    CHAR_DATA  *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "MobProg stat whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( !IS_NPC( victim ) )
    {
	send_to_char( "Only Mobiles can have Programs!\n\r", ch);
	return;
    }
    if ( !( victim->pIndexData->progtypes ) )
    {
	send_to_char( "That Mobile has no Programs set.\n\r", ch);
	return;
    }
    sendf( ch, "Name: %s.  Vnum: %d.\n\r", victim->name, victim->pIndexData->vnum );
    sendf( ch, "Short description: %s.\n\rLong  description: %s",
      victim->short_descr, victim->long_descr[0] != '\0' ? victim->long_descr : "(none).\n\r" );
    for ( mprg = victim->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
    {
        sendf( ch, ">%s %s\n\r%s\n\r",
          mprog_type_to_name( mprg->type ), mprg->arglist, mprg->comlist );
    }
}

/* prints the argument to all the rooms aroud the mobile */
void do_mpasound( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in_room;
    int door;
    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        bug( "Mpasound - No argument: vnum %d.", ch->pIndexData->vnum );
	return;
    }
    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
        EXIT_DATA *pexit;
        if ( ( pexit = was_in_room->exit[door] ) != NULL && pexit->u1.to_room != NULL && pexit->u1.to_room != was_in_room )
        {
            ch->in_room = pexit->u1.to_room;
            MOBtrigger  = FALSE;
            act_new( argument, ch, NULL, NULL, TO_ROOM, POS_DEAD );
        }
    }
    ch->in_room = was_in_room;
}

/* Removes a mob from the game */
void do_disappear( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch))
	return;
    extract_char(ch,TRUE);
}


/* lets the mobile destroy an object in its inventory   *
 * it can also destroy a worn object and it can destroy *
 * items using all.xxxxx or just plain all of them      */
void do_mpjunk( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj, *obj_next;
    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }
    one_argument( argument, arg );
    if ( arg[0] == '\0')
    {
        bug( "Mpjunk - No argument: vnum %d.", ch->pIndexData->vnum );
	return;
    }
    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
        if ( ( obj = get_obj_wear( ch, arg, ch ) ) != NULL )
        {
            unequip_char( ch, obj );
            extract_obj( obj );
            return;
        }
        if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
            return; 
        extract_obj( obj );
    }
    else
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
            {
                if ( obj->wear_loc != WEAR_NONE)
                    unequip_char( ch, obj );
                extract_obj( obj );
            } 
        }
}

void do_if( CHAR_DATA *ch, char *argument )
{
}

void do_endif( CHAR_DATA *ch, char *argument )
{
}
