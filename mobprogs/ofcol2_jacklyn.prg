>rand_prog 3~
if rand (33)
	say When magic and good fighting skills are combined, it is unstoppable.
else
	if rand (50)
		wear ranseur
	else
		remove ranseur
	endif
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

