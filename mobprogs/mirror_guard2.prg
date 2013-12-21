>rand_prog 3~
if isasleep ($i)
	wake
else
	if rand (50)
		pmote looks at a map.
	else
		if ishome ($i)
			sleep
		endif
	endif
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

