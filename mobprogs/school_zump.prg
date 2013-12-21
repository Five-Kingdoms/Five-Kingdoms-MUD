>rand_prog 10~
if rand(50)
	say Please read all signs.  They are important.
else
	say If you need help, use the newbie channel with 'newbie <mesage>'.
endif
~
>greet_prog 50~
if rand(50)
	say Welcome, $N!  If you want to practice your skills and spells or train your stats, go south.
else
	say Welcome, $N!  If you want to practice fighting, go west.
endif
~
|

