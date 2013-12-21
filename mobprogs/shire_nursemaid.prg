>speech_prog p Waah!~
if isnghost ($r)
if ispc ($r)
	yell Stay away from the toddlers, $R!
	murder $r
endif
endif
~
>rand_prog 3~
if rand (50)
	pmote changes a smelly diaper.
else
	sigh
endif
~
>greet_prog 5~
if istime (21) = 7
	say Shh..  Don't wake the toddlers.
endif
if israce($n) == 6
	say Which one of these is yours?
endif
~
|

