>greet_prog 5~
if israce($n) == 3
	tell self shit
else
	say You do not belong here.
endif
~
>rand_prog 3~
if rand(50)
	pmote punches an imaginary foe.
else
	if mobobj (5104)
		pmote snaps a snake headed whip in the air.
	else
		if mobobj (5105)
			pmote twirls a noble's longsword in $l hand.
		endif
	endif
endif
~
|

