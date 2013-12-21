>rand_prog 3~
mpasound You hear a low, guttural growl in the distance.
if rand(50)
        pmote hisses and writhes.
else
        pmote carefully looks at her eggs.
endif
~

>act_prog p gets a white, leathery egg.~
scream $n
pmote lunges at you, and snatches her egg back!
chown egg $n
pmote places the egg back in the nest.
murder $n
~

>act_prog p explodes from trauma!~
if ispc($n)
	break
else
	pmote shivers and shudders!
	echo `!A Craw Wurm's stomach is sliced open!
	echo `!A Craw Wurm expires from internal injuries!``
	slay !wurm!
endif
~

>fight_prog 50~
if rand(10)
	pmote hisses, and unhinges her jaw!
	pmote swallows $n whole!
	teleport $n 11049
	echo You see the figure of $n slide down into the Craw Wurm's belly!
else
	pmote hisses, and unhinges her jaw!
	pmote tries to swallow $n, but fails.
endif
~

>death_prog 100~
at 11049 pmote shudders and dies!
at 11049 echo `!The worm's stomach dies as her body is slain from outside!``
at 11049 echo `!You spill out of her stomach as she is slashed open!``
echo `!A Craw Wurm falls to the ground, and her stomach's contents spill out.``
echo `3A horrible stench fills the room.``
~
|

