>rand_prog 3~
if rand (50)
	cackle
else
	pmote works on his battle plans.
endif
~
>greet_prog 5~
if rand (50)
	if isnghost ($n)
	say Who let you in here?  I'm busy.
	murder $n
else
	cackle
	say Revelsport will be mine!
endif
endif
~
|

