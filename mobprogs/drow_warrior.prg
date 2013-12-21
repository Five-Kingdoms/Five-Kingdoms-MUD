>greet_prog 20~
if israce($n) == 3
	bow $n
else
	if isnghost ($n)
		yell Protect the matron mother!  Kill $N!
		murder $n
	endif
endif
~
>rand_prog 3~
if rand(50)
	pmote marches back and forth.
else
	if mobobj (5101)
		pmote twirls a noble's longsword in $l hand.
	endif
endif
~
|

