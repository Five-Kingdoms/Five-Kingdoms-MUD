>rand_prog 3~
if rand (33)
	say I am as strong as any man.
else
	if rand (50)
		wear sword
	else
		remove sword
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

