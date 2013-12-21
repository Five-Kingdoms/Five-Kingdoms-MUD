default:	clands
clean:		
		rm -f $(O_FILES)
all:		clean clands

CC      = gcc
PROF    = -ggdb
NOCRYPT = 
C_FLAGS =  -pipe -Wall -g $(PROF) $(NOCRYPT)
L_FLAGS =  $(PROF)
LIBS =

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o alias.o\
	  armies.o autoquest.o avatar.o bounty.o crypt.o cabal.o\
	  cabal2.o cabal3.o clan.o comm.o const.o effect.o db.o dndspell.o\
	  fight.o handler.o\
	  html.o interp.o jail.o losereply.o magic.o magic2.o magic3.o magic4.o misc.o\
	  mob_prog.o\
	  mob_prog_new.o mob_cmds.o save_mud.o olc.o olc_act.o olc_mpcode.o\
	  recycle.o raid.o\
	  save.o skills.o skills2.o skills3.o skills4.o skills5.o s_skill.o\
	  song.o tome.o \
	  tattoo.o special.o vote.o tables.o update.o \


#	$(CC) $(L_FLAGS) -o clands $(O_FILES) $(LIBS)

clands: $(O_FILES)
	rm -f clands
	$(CC) $(L_FLAGS) -o clands $(O_FILES) $(LIBS)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

