>greet_prog 3~
setquest $n moria 1
say What are you doing here?
pmote picks $N up.
pmote tosses $N out of the cave.
teleport $n 4042
~
>rand_prog 3~
if rand(50)
	pmote tosses a rock around.
else
	pmote tosses a rock in front of $R.
endif
~
|

