>rand_prog 3~
if findmob (5335)
	pmote ruffles the child's hair.
else
	pmote stands here looking stern.
endif
~
>greet_prog 5~
if isgood($n)
	tip $n
else
	if isevil($n)
		say I'm keeping my eye on you, $N.
	else
		exa $n
	endif
endif
~
|

