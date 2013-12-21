>time_prog 66~
if istime (21) = 9
	yawn
	pmote lays down on the ground.
	sleep
else
	if isasleep($i)
		wake
		stretch
	endif
endif
~
>rand_prog 3~
if rand(50)
	pmote throws a boomerang at $R.
else
	innocent
endif
~
|

