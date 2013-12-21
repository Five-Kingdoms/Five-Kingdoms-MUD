>give_prog 100~
if number($o) == 784
	say Bring me the answer to this riddle:
        say I am just two and two, I am warm, I am cold, and the parent of numbers that cannot be told. 
	say I am lawful, unlawful- a duty, a fault, 
	say I am often sold dear, good for nothing when bought.
	say An extraordinary boon, and matter of course.
	say And yielded with pleasure when taken by force.
        mpjunk $o
	load obj 785
	say Give me the answer with this clue in your inventory.
	say But beware, the wrong answer will cost you dear.
	gi clue $n
else
if charobj(785)
	say Why are you giving me such worthless junk $n!
	chown 'clue #16' $n
	mpjunk clue
	mpjunk $o
	smite $n
	ca damnation $n
	load obj 4632
	recite teleport $n
endif
endif
~
>act_prog p kisses you.~
if charobj(785)
        chown 'clue #16' $n
        mpjunk clue
	if level ($n) < 30
		bug $n has completed the lowbie quest.
		saying Congratulations on completing the quest young one, your completion has been logged.
	else
		say A cannibal holds the key, to the sisters three.
		load obj 786
		gi clue $n
	endif
endif
~
|
