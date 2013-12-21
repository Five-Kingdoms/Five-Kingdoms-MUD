#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "interp.h"
#include "cabal.h"
#include "jail.h"
#include "olc.h"
#include "mob_cmds.h"

// CARTEL SKILLS and SPELLS

void do_interrogate( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MSL];
    char arg[MIL];

    one_argument(argument, arg);

    if (IS_NPC(ch))
        return;
    if ( get_skill(ch,gsn_interrogate) < 2){
      send_to_char( "Pardon?\n\r", ch );
      return;
    }
    if ( arg[0] == '\0' )
    {
        send_to_char("Interrogate whom?\n\r", ch);
        return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL)
    {
        send_to_char("You cannot interrogate while on horseback.\n\r",ch);
        return;
    }
    if (IS_NPC(victim))
    {
        send_to_char("There's no point in interrogating a mob.\n\r",ch);
        return;
    }
    if (victim == ch)
    {
        send_to_char("Interrogate yourself? No challenge there.\n\r",ch);
        return;
    }
    if (!is_affected(victim, gsn_kidnap))
    {
        sendf(ch,"You don't really have the right leverage to interrogate %s.\n\r",victim->name);
        return;
    }
    if (( victim->in_room == NULL )
    || ( victim->fighting != NULL )
    || ( victim != ch && ( saves_spell( ch->level - 5, victim,DAM_OTHER,skill_table[gsn_interrogate].spell_type))))
    {
            act("Despite your best efforts, $N just won't crack.",ch,NULL,victim,TO_CHAR);
	    sendf(victim,"You stay strong, and resist %s's efforts to interrogate you.\n\r",ch->name);
	    check_improve(ch,gsn_interrogate,FALSE,1);
            return;
    }
    sendf(victim,"You succumb to %s's interrogation tactics, and spill your guts.\n\r",ch->name);
    sendf(ch,"You succeed in your interrogation tactics. %s starts to talk...\n\r",victim->name);
    switch(number_range(0,9))
    {
    case 0 :
	if (ch->pCabal != NULL)
	    sprintf( buf, "I am a member of %s .", victim->pCabal->who_name);
	else
            sprintf( buf, "I am do not belong to any cabal.");
        break;
    case 1 :
        sprintf( buf, "I belong to the %s guild.", class_table[victim->class].name);
        break;
    case 2 :
        sprintf( buf, "In my life, I have murdered %d people.", victim->pcdata->kpc);
        break;
    case 3 :
        sprintf( buf, "Over my years, I have sustained %d deaths.", victim->pcdata->dpc);
        break;
    case 4 :
        sprintf( buf, "I have a gold net worth of %ld, with %d of that in the bank.", victim->gold, victim->in_bank);
        break;
    case 5 :
        sprintf( buf, "I follow the %s ethos, and have been wanted by the law %d times.", ethos_table[victim->pcdata->ethos].name, victim->pcdata->flagged );
        break;
    case 6 :
        sprintf( buf, "I have %d max hit points.", victim->max_hit);
        break;
    case 7 :
        sprintf( buf, "I have %d max mana.", victim->max_mana);
        break;
    case 8 :
        sprintf( buf, "I have %d max moves.", victim->max_move);
        break;
    case 9 :
        sprintf( buf, "My hometown is %s.", hometown_table[victim->hometown].name);
        break;
    }
    check_improve(ch,gsn_interrogate,TRUE,3);
    victim->hit -= number_range(1,10);
    do_say(victim,buf);
}

void do_kidnap( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], arg2[MIL], buf[MSL];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;
    one_argument(argument, arg);
    if (IS_NPC(ch))
        return;
    if ( get_skill(ch,gsn_kidnap) < 2){
      send_to_char( "Pardon?\n\r", ch );
      return;
    }
    if ( arg[0] == '\0' )
    {
        send_to_char("Kidnap whom?\n\r", ch);
        return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL)
    {
        send_to_char("You cannot kidnap on a horse.\n\r",ch);
        return;
    }
    if (IS_NPC(victim))
    {
        send_to_char("Why would you kidnap a mob?\n\r",ch);
        return;
    }
    if (victim == ch)
    {
        send_to_char("Kidnap yourself?  What a great idea.\n\r",ch);
        return;
    }
    if ( victim->in_room == NULL
    || IS_SET(victim->in_room->room_flags, ROOM_NO_INOUT)
    || IS_SET(victim->in_room->room_flags, ROOM_NO_GATEOUT)
    || IS_SET(victim->in_room->room_flags, ROOM_NO_TELEPORTOUT)
    || is_affected(victim, gsn_tele_lock)
    || ( victim->fighting != NULL )
    || ( victim != ch && ( saves_spell( ch->level - 5, victim,DAM_OTHER,skill_table[gsn_kidnap].spell_type))))
    {
        act("You failed to kidnap $N.",ch,NULL,victim,TO_CHAR);
        sprintf(buf, "Help! %s just tried to kidnap me!",PERS(ch,victim));
        j_yell(victim,buf);
        damage(ch,victim,number_range(10, 20),gsn_kidnap,DAM_BASH,TRUE);
        return;
    }
    else
    {
        AFFECT_DATA af;
        sprintf(arg2,"2110");
        location = find_location( ch, arg2 );
        send_to_char("You are knocked unconscious by an unknown assailant.\n\r",victim);
        send_to_char("You succeed in your kidnap attempt!\n\r",ch);
        act( "$n is knocked out and carried off!", victim, NULL, NULL, TO_ROOM );
        act( "$n seems to have disappeared, strangely.", ch, NULL, NULL, TO_ROOM );
        char_from_room( ch );
        char_to_room( ch, location );
        char_from_room( victim );
        char_to_room( victim, location );
        act( "$n has arrived, with someone in tow.", ch, NULL, NULL, TO_ROOM );
        do_look( ch, "auto" );
        act( "You bring $n into the cell, bound tightly.", victim, NULL, NULL, TO_ROOM );
        send_to_char("You slowly come to only to find yourself bound.\n\r You look around at your new surroundings.\n\r",victim);
        do_look( victim, "auto" );
        ch->mana -= 50;
        check_improve(ch,gsn_kidnap,TRUE,1);
        af.type         = gsn_kidnap;
        af.level        = ch->level;
        af.duration     = ch->level /10;
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = 0;
        af.location  = APPLY_NONE;
        affect_to_char(victim,&af);
    }
}
