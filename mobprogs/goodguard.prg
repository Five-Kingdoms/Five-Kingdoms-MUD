>rand_prog 5~
if rand (50)
	pmote watches $R closely.
else
	pmote paces back and forth.
endif
~
>greet_prog 3~
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

