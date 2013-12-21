>greet_prog 100~
if charobj (10620)
	pmote whispers `6'I see you follow the truth of Chaos.'``
	say And now, $N, you shall reap the pain of Chaos.
	chown "chaos" $n
	pmote utters an indecipherable incantation.
	mpjunk chaos
	say With this rod, I create entrance to the Tower of the Winds.
	yell And so, $N passes into the Tower. We await you, Heart of Darkness!
	echo `6Thunder roils in the heavens.``
	say The God of Chaos awaits, $N.
	load obj 30055
	unlock north
	open north
	give key $n
	bow $N
	say Go in Chaos, my child.
	setquest $n chaos 1
else
	say Only those who posess the rod of Chaos may pass here!
	yell Die, $N, you shall never enter the Tower of the Winds!
	murder $n
endif
~
>fight_prog 50~
if rand(33)
        yell $n is attempting to enter the Tower of the Winds!
else
        if rand(10)
                yell You shall never enter this tower alive, $n!
        else
                if rand(20)
                        say `!Feel the wrath of the Gods,, $n!``
                        cast 'wrath'
                else
                        say `!Feel the arcane powers of```1 Hell```!,
$n!``
                        cast 'hellstream'
                endif   
endif
endif
~
|

