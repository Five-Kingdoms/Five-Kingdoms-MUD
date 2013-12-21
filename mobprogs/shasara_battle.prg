>speech_prog p Show your Guild Badges, Guildmasters.~
if ispc($n)
	break
else
	say Another day, another student, eh, Rastellagnar?
endif
~

>speech_prog p If you are a Guildmaster, you'd best get to your guild.~
if ispc($n)
        break
else
        say Another day, another student, eh, Rastellagnar?
endif
~

>greet_prog 100~
is isnpc($n)
	break
else
	say Welcome, $N!
	say Our doors are always open!
	if isclass($n) == 10
		nod $n
	else
		say Wait a minute...you're not a member of this guild!
		emote ushers you out the door.
		qtrans $n 8264
	endif
endif
~

>rand_prog 50~
if rand(25)
	say If you need help practicing, just ask.
	say I also sell fine Elven-made practice weaponry.
else
	emote mutters some incantations.
endif
~
>bloody_prog 100~
say You are too bloody to enter our guild, $N!
say Take refuge elsewhere until a later time.
pmote escorts $n out of the guild.
teleport $n 8255
~
>justice_prog 100~
say Welcome $N, Upholder of the Laws.
say 
say We all do our part in keeping this town safe from crime.
pmote bows before $n.
~
|

