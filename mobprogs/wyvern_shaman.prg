>time_prog 100~
if istime (20) = 20
	pmote comes to life.
else
	if istime (4) = 4
		pmote turns into stone.
	endif
endif
~
>rand_prog 3~
if istime (20) = 4
	pmote yells a gargoyle war chant.
endif
~
>fight_prog 5~
load mob 1603
force angry murder $n
~
|

