>speech_prog p Hug me!~
if isnpc($n)
	hug $n
endif
~
>speech_prog p Squeeze me!~
if isnpc($n)
	squeeze $n
endif
~
>speech_prog p Go to sleep!~
if isnpc($n)
	say Okay, good night.
	sleep
endif
~
>rand_prog 3~
if inroom ($i) == 6606
	defecate
else
	if istime (21) == 7
		wake
		pmote sneaks around in the darkness.
	else
		pmote runs in cirles around $R.
		say Do you want to play a game?
	endif
endif
~
>fight_prog 5~
yell Nanny, nanny!  $N is hurting me!
~
>greet_prog 5~
if isnpc($n)
	tell self shit
else
	if israce($n) == 4
		smile $n
	else
		point $n
		yell Stranger!
	endif
endif
~
>time_prog 75~
if istime (21) == 7
	if findmob(6606)
		say Shh.. I'm supposed to be asleep.
	endif
endif
if istime (21) == 7
	yawn
	sleep
else
	if isasleep($i)
		wake
		say It's time to play!
	endif
endif
~
|

