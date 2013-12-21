>greet_prog 50~
say How did you get in here?  Obviously I didn't make the maze hard enough.
~
>rand_prog 50~
if number ($r) == 6516
	tell self shit
else
	say Begone!
	if rand(33)
		teleport $r 6548
	else
		if rand(50)
			teleport $r 6549
		else
			teleport $r 6550
		endif
	endif
endif
~
|

