>rand_prog 3~
if rand (20)
	pmote holds $l bleeding wounds.
else
	pmote writhes in agony.
endif
~
>greet_prog 5~
if rand (5)
	say Go nnnno....further, $n! Death only awaits you!
	pmote coughs up some more blood.
else
	say The creature nearly killed me! It will surely be your death, $n!
	wince
endif
~
|

