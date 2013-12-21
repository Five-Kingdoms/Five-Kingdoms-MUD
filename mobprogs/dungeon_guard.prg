>greet_prog 100~
if rand (10)
	nod $N
	say You's all done it now. Heheh.
	snicker self
	say Quickest way outta 'ere is to kill yerselves!
	chuckle
endif
if rand (33)
	say Gettin claustrophibic yet, fellas?
	snicker
	say Gonna rot in 'ere, I think.
endif
~
>rand_prog 5~
if rand (50)
	say Y'all gettin' hungry yet?
	chuckle
	load obj 20851
	drop slop
	say Chow down.
	laugh
else
	say Bet y'all could use a little supper, eh?
	load obj 20851
	drop slop
	say Yum yum.
	chortle
endif
~

>bribe_prog 200000~
mpjunk gold
say Well, I spoze we can work a deal, $N.
grin $N
pmote unlocks and opens the cell doors north of you.
say I don't wanna see you here again, $N.
pmote jingles some change in his pocket.
say Heh. Or maybe I do.
grin $N
setquest $N prison2 1
teleport $N 20896
snicker
~
>fight_prog 100~
say Oh, you wanna piece of me, $N?!
say No fighting in MY DUNGEON!
pmote `!breaks up the fight!``
peace
restore room
spit $N
say You sit like a good little inmate, $N.
smirk
~
>bribe_prog 1~
mpjunk gold
say What do I look like, a crook?!
dismay $N
~
|

