>fight_prog 5~
say Waah!
~
>time_prog 75~
if istime (21) = 7
	yawn
	sleep
else
	if isasleep($i)
		wake
	endif
endif
~
>rand_prog 3~
if rand (50)
	pmote crawls around on the floor.
else
	cry
endif
~
>greet_prog 5~
if sex ($n) == 2
	say ma ma?
else
	say da da?
endif
~
|

