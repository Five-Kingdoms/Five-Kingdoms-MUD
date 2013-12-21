>rand_prog 3~
if rand(50)
	say You guys do not know as much about the forest as you think.
else
	load obj 1621
	emote places the potion on the shelf.
	say That one should come in handy.	
endif
~

>fight_prog 10~
	pmote closes in and with a powerfull grapple throws $N out the door.
	yell get out of my trading post $n!
	qtrans $N 1702
~
|







