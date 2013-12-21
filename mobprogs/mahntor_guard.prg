>greet_prog 10~
if isnghost ($n)
if ispc($n)
	yell No trespassing, $N!
	murder $n
endif
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

