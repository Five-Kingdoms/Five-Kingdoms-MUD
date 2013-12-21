>greet_prog 25~
if isnghost ($n)
yell Halt, $N!  Do not bother the master of the black robes.
murder $n
endif
~
>rand_prog 3~
if rand(50)
	say Evil will consume the world and become victorious.
else
	pmote scowls at $R.
endif
~
|

