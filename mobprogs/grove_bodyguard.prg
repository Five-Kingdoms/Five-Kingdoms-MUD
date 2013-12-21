>rand_prog 3~
emote peers around, making sure the room is safe from assassins.
~
>greet_prog 100~
if isgood($n)
	say Join our forces $N, and help us eradicate the heathens.
else
	if iscabal($n) == 10
		if isnghost ($n)
			yell Protect the General!
			say To hell with you!
			murder $n
		endif
	else
	if iscabal($n) == 12
		if isnghost ($n)
			yell Protect the General!
			say To hell with you!
			murder $n
		endif
	else
	if isevil($n)
		if isnghost ($n)
			yell To death with the heathen!
			say To hell with you!
			murder $n
		endif
	endif
endif
endif
endif
~
|


