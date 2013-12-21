>greet_prog 100~
if rand (10)
        say Sorry, $N, but the Halloween fun is over now.
	advance $N 1
	say Farewell.
	wave $N
	slay $N
	get corpse
	eat corpse
else
        say Halloween's over, $N. Time to play fair, now.
	advance $N 1
	say Farewell.
	wave $N
	slay $N
	get corpse
	eat corpse
endif
~
>rand_prog 50~
if rand (10)
        say So much to do, so little time.
	say I am the Adjuster! Prepare to be adjusted!
	cackle
else
        say You're here to be adjusted, $N. So here we go...
	advance $N 1
	say Farewell, $N.
	wave $N
	slay $N
	get corpse
	eat corpse
endif
~   
|
