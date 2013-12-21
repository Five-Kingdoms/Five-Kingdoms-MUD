>greet_prog 10~
if hitprcnt ($n) < 100
	say You look tense, $n.  For 100 gold, I'll give you an extra special massage.
endif
~
>rand_prog 3~
if rand (50)
	rub $r
else
	massage $r
endif
~
>bribe_prog 100~
if isaffected ($i)
	say Sorry, I need to rest first.
	give coins $n
else
	layon $n
	setquest $n mirror2 1
	mpjunk gold
endif
~
|
