>rand_prog 3~
if rand (50)
	echo Maggots crawl all over $I's decaying body.
else
	echo Flies swarm around $I.
endif
~
>greet_prog 5~
if rand (50)
	say Time to kill.
else
	pmote swaggers towards $N.
endif
~
|

