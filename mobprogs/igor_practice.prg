>greet_prog 100~
if rand (10)
	say For 15,000 gold, I can provide you with a practice weapon.
endif
~
>rand_prog 5~
if rand (50)
	say Mondan is a skilled craftsman of practice weapons.
	pmote points to a sign on the door.
else
	say I make real weapons, Mondan makes practice weapons.
	pmote points to a sign on the door.
endif
chuckle
~
>bribe_prog 15000~
mpjunk gold
load obj 9770
give token $n
say There you go. Give this to Mondan.
pmote points to the door south of you.
~
>bribe_prog 1~
mpjunk gold
say Thanks for the money, but practice weapons run for 15,000 gold.
~
|

