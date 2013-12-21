>give_prog 100~
if number($o) == 778
	say Bring me the answer to this riddle:
        say What turns without moving?
        mpjunk $o
	load obj 779
	say Give me the answer with this clue in your inventory.  
	gi clue $n
endif
if number($o) == 611
if charobj(779)
        say Go see my wisest cousin.
        mpjunk $o
	chown 'clue #10' $n
	mpjunk clue
	load obj 780
	gi clue $n
endif
endif
if number($o) == 5340
if charobj(779)
        say Go see my wisest cousin.
        mpjunk $o
	chown 'clue #10' $n
	mpjunk clue
	load obj 780
	gi clue $n
endif
endif
~
|
