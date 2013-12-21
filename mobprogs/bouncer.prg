>greet_prog 100~
if rand (10)
	say Cover charge is 5000 gold tonight.
	say Don't start anything you want ME to finish, $N.
	grin $n
endif
if rand (33)
	say Cover's 5000 gold tonight.
	say I want no trouble from you tonight, $N.
	nod $n
endif
~
>rand_prog 5~
if rand (50)
	pmote looks around, watchfully, his arms folded over his chest.
else
	pmote grumbles and growls about work, drunks, and late hours.
endif
~         
>bribe_prog 5000~
mpjunk gold
say Enjoy yourself. No fighting inside.
pmote takes a shiny key from his pocket.
pmote unlocks the heavy doors west of you.
open west
nod $n
teleport $n 20736
echo $n walks west.
close west
pmote puts a shiny key back in his pocket.
~
>fight_prog 100~
yell Guards! Guards! $N is disturbing the peace at the White Lion!
say You're done for now, $N.
~
|

