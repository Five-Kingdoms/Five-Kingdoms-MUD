>greet_prog 25~
if isnghost ($n)
if isevil($n)
	yell Halt, evil $N!  Do not bother the master of goodness.
	murder $n
endif
endif
~
>rand_prog 3~
if rand(50)
	say Good shall overcome evil and save the world.
else
	say Do you walk in the light?
endif
~
|

