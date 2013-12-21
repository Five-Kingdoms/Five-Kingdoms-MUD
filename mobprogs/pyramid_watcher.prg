>rand_prog 3~
if rand(50)
	pmote eyes $R warily.
else
	say Looting is not tolerated.
endif
~
>time_prog 10~
if findmob(8704)
	yell You lousy thief!
	kill thief tomb
else
	if findmob(8705)
		yell You lousy thief!
		kill ali baba thief
	endif
endif
~
>greet_prog 5~
if isclass($r) == 5
	if isnghost ($r)
	yell Die $R, you lousy thief!
	murder $r
	endif
endif
~
|

