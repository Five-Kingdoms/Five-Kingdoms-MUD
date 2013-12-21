>greet_prog 100~
if charobj (303)
	say Give me that jar of blood, $N.
	chown "blood jar" $n
	pmote drinks blood from a dark red jar.
	mpjunk jar
	say I'm awfully tired now...
	sleep
	snore
	setquest $n plains 1
else
	say Give me blood!
endif
~
|

