>give_prog 100~
if number($o) == 782
	say Bring me the answer to this riddle:
        say Lives in winter, dies in summer, and grows with it's root upwards.
        mpjunk $o
	load obj 783
	say Give me the answer with this clue in your inventory.
	gi clue $n
endif
if number($o) == 9227
if charobj(783)
        say I think it's time you visited the Great and Ancient Sphinx.
        mpjunk $o
	chown 'clue #14' $n
	mpjunk clue
	load obj 784
	gi clue $n
endif
endif
~
|
