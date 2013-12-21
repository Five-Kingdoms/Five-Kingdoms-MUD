>greet_prog 5~
if mobobj (2350)
	pmote whips out the gate key.
	say See this?  You'll need this to get in.
endif
~
>rand_prog 3~
if rand(50)
	pmote marches back and forth to keep warm.
else
	shiver
endif
~
|

