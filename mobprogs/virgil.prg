>give_prog 100~
if rand (90)
	pmote examines $O closely.
	identify $o
	give $o $n
else
	cackle $n
	say Thank you for your generous contribution, sucker!
endif
~
>rand_prog 5~
if rand (90)
	pmote slays you in cold blood!
	echo You turn into an invincible ghost for a few minutes.
	echo As long as you don't attack anything.
	c mass
else
	slay $r
endif
~
>bribe_prog 1000~
c sanc $n
~
>bribe_prog 500~
c bless $n
~
>bribe_prog 100~
c heal $n
~
>bribe_prog 1~
say You cheapskate!
smite $n
~
>greet_prog 10~
if rand (50)
	say Hey, mud addict!
else
	give 1 gold $n
endif
~
>time_prog 100~
random
~
|
