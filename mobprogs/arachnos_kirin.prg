>greet_prog 5~
if isgood($n)
	say Help me rescue the slaves, $N.
else
	if isnghost ($n)
	if isevil($n)
		say Begone, evil $N!
		murder $n
	else
			say Will you help me rescue the slaves, $N?
		endif
	endif
endif
~
|

