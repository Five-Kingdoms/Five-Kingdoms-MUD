>time_prog 66~
if istime (21) = 6
	yawn
	pmote lays down on her bed.
	sleep
else
	if isasleep($i)
		wake
		say Another day of taking care of the children.
	endif
endif
~
>rand_prog 25~
if rand(12)
	pmote cleans up the house.
else
	if isasleep ($r)
		wake $r
		say Wake up!
	endif
endif
~
|

