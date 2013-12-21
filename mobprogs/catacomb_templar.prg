>rand_prog 3~
peer $r
~
>greet_prog 75~
if isgood($n)
	say Join my forces $N, and help me eradicate the evil.
else
	if isevil($n)
		if isnghost ($n)
			yell Slay $N the infidel!
			murder $n
		endif
	endif
endif
~
|

