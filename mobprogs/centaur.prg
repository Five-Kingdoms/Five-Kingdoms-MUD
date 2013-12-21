>rand_prog 3~
mpasound You hear the click-clack of horseshoes nearby.
if rand (50)
	pmote trots past you
else
	pmote swishes $l tail around.
endif
~
>greet_prog 5~
if rand (50)
	say Half horse, half man.  What could be better?
else
	pmote walks towards $N on all fours.
endif
~
|

