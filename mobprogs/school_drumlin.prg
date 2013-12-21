>greet_prog 100~
if charobj (7769)
	say Looks like you already have your diploma, $N.
	say Time for you to head to the Combat Arena.
	smile $n
else
	say Ah, congratulations, $N.
	say You've made it though the Academy unscathed!
	say The diploma is yours!
	load obj 7769
	give diploma $n
	say Have fun in the Arena.
	smile
endif
~
|

