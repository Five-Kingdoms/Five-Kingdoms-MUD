>hitprcnt_prog 1000~
if hitprcnt($n) <= 20
	if rand(25)
		say Flee $N, before you die!
	endif
endif
~
>greet_prog 25~
if rand(50)
	say Do you think you're ready to graduate, $N?  I am your final test.
else
	pmote shows $N a diploma.
	say Kill me for your diploma, $N.
endif
~
>rand_prog 5~
pmote polishes a mud school diploma.
~
|

