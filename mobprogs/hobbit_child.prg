>time_prog 25~
if istime (21) = 6
	yawn
	pmote lies on the ground and curls up into a ball.
	sleep
else
	if isasleep($i)
		wake
		stretch
	endif
endif
~
>rand_prog 1~
if rand(50)
	say Do you want to play a game?
else
	if sex ($i) == 1
		pmote runs around and tags $R.
	else
		if sex ($i) == 2
			pmote skips around happily.
		endif
	endif
endif
~
|

