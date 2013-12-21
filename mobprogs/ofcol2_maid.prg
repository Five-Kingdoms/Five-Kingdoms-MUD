>rand_prog 3~
if rand (50)
	pmote sweeps up the ground.
else
	pmote prepares some food.
endif
~
>greet_prog 5~
if rand (50)
	say You're ruining my floor!
else
	say Shoo!
	pmote hits $N with a broom.
endif
~
|

