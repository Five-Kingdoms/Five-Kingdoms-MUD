>rand_prog 3~
if rand (50)
	cackle
else
	grin $r
endif
~
>greet_prog 5~
if rand (50)
	say Time to have fun.
else
	say I'm not as ugly as you, $N.
endif
~
>speech_prog p Kill him!~
if ispc ($r)
	if isnghost ($r)
		murder $r
	endif
endif
~
>speech_prog p Kill her!~
if ispc ($r)
	murder $r
endif
~
|

