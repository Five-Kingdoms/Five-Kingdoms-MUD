>greet_prog 5~
if rand (50)
	say Go Away!
else
	mutter
endif
~
>rand_prog 5~
if roomobj (309)
	pmote turns the rabbit on its spit.
else
	yell Hey... Who stole my food?
	load obj 309
	drop rabbit roast
	endif
endif
~
>get_prog 100~
if number ($o) == 309
	if isnghost ($n)
	yell Don't steal my food, $N!
	stand
	murder $n
	endif
endif
~
|

