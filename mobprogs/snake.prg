>rand_prog 3~
if rand(50)
	pmote slithers around on the ground.
else
	pmote flicks its tongue at $R.
endif
~
>time_prog 10~
if findmob(8702)
	force mongoose kill snake
endif
~
|

