>give_prog 100~
if number($o) == 780
	say Bring me the answer to this riddle:
        say What force or strength cannot get through.
	say I, with gentle touch, can do.
	say And many in the street could stand.
	say Were I not, as a friend, at hand.
        mpjunk $o
	load obj 781
	say Give me the answer with this clue in your inventory.
	gi clue $n
endif
if objtype($o) == 18
if charobj(781)
        say Take your clue to my cousin, who is currently pondering the question to be asked of you.
        mpjunk $o
	chown 'clue #12' $n
	mpjunk clue
	load obj 782
	gi clue $n
endif
endif
~
|
