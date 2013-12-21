>give_prog 100~
if number($o) == 786
        say Return to the place where you first started, and search for the Cavern of Fate.
	say You must break the chains of fate, and create your own destiny.
        mpjunk $o
	load obj 935
	gi key $n
endif
~
|
