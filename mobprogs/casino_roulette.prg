>greet_prog 75~
say Hey there, $N.
if rand(50)
	say Care for a turn at the roulette tables?
else
	say Care to tango with lady luck?
endif
say These are red chip tables, only, $n. Blue chips tables are to the north!
emote gestures north to the Blue Chip tables.
say Here, you can place bets on black, red, quadruples, or doubles.
say If you are confused, type 'LOOK STAKES'.
say Which will it be, $n?
~
>rand_prog 2~
say Dare to try your luck at my roulette table?
~
>speech_prog black~
if isnpc($n)  
break
endif
if charobj (21200)	
	chown |21200| $n
	mpjunk |21200|
	say $n places bets on the black squares!
	pmote moves your red chip to the black squares.
	pmote spins the roulette wheel!
	say Round and round she goes!
	echo The ball bounces around, and finally comes to rest.
	if rand(50)
		echo The ball comes to land on a black square!
		say You've won, $n! Single odds pay out one to one.
		load obj 21200
		give |21200| $n
		say And, the one you wagered...
		load obj 21200
		give |21200| $n
		say Congratulations, $n. Care to play again?
	else
		echo The ball comes to land on a red square!
		say Oh, too bad, $n. Care to try again?
	endif
else
	say No one plays without chips, $n!
	say If you need chips, buy them at the door!
endif~

>speech_prog red~
if isnpc($n)  
break
endif
if charobj (21200)	
	chown |21200| $n
	mpjunk |21200|
	say $n places bets on the red squares!
	pmote moves your red chip to the red squares.
	pmote spins the roulette wheel!
	say Round and round she goes!
	echo The ball bounces around, and finally comes to rest.
	if rand(50)
		echo The ball comes to land on a red square!
		say You've won, $n! Single odds pay out one to one.
		load obj 21200
		give |21200| $n
		say And the one you wagered...
		load obj 21200
		give |21200| $n
	else
		echo The ball comes to land on a black square!
		say Oh, too bad, $n. Care to try again?
	endif
else
	say No one plays without chips, $n!
	say If you need chips, buy them at the door!
endif~

>speech_prog doubles~
if isnpc($n)  
break
endif
if charobj (21200)	
	chown |21200| $n
	mpjunk |21200|
	say Ah, risky, $n! $n places bets on doubles!
	pmote moves your red chip to the doubles.
	pmote spins the roulette wheel!
	say Round and round she goes!
	echo The ball bounces around, and finally comes to rest.
	if rand(25)
		echo The ball comes to land on a Red 12!
		say Oh, too bad, $n. Red 11 is not on your doubles square!
		say Care to try again?
	endif
	if rand(25)
		echo The ball comes to land on a Black 23!
		say Oh, too bad, $n. Black 23 is not on your doubles square!
		say Care to try again?
	endif
	if rand(25)
		echo The ball comes to land on a Red 30!
		say Oh, too bad, $n. Red 30 is not on your doubles square!
		say Care to try again?
	else
		echo The ball comes to land on your doubles square!
		say You are a winner, $n!
		say Double bets pay out two to one.
		load obj 21200
		load obj 21200
		give |21200| $n
		give |21200| $n
		say And, the one you wagered...
		load obj 21200
		give |21200| $n		
		say Congratulations! Care to play again?
	endif
else
	say No one plays without chips, $n!
	say If you need chips, buy them at the door!
endif~

>speech_prog quadruples~
if isnpc($n)  
break
endif
if charobj (21200)	
	chown |21200| $n
	mpjunk |21200|
	say We got a high roller here, folks!
	say $n places bets on quadruples!
	pmote moves your red chip to the quadruples.
	pmote spins the roulette wheel!
	say Round and round she goes!
	echo The ball bounces around, and finally comes to rest.
	if rand(12)
		echo The ball comes to land on Black 17.
		say Sorry, $n. Black 17 isn't your quad.
		say Care to try again?
	endif
	if rand(12)
		echo The ball comes to land on Red 22.
		say Sorry, $n. Red 22 isn't your quad.
		say Care to try again?
	endif
	if rand(12)
		echo The ball comes to land on Black 13.
		say Sorry, $n. Black 13 isn't your quad.
		say Care to try again?
	endif
	if rand(12)
		echo The ball comes to land on Red 18.
		say Sorry, $n. Red 18 isn't your quad.
		say Care to try again?
	endif
	if rand(12)
		echo The ball comes to land on Black 19.
		say Sorry, $n. Black 19 isn't your quad.
		say Care to try again?
	endif
	if rand(12)
		echo The ball comes to land on Red 28.
		say Sorry, $n. Red 28 isn't your quad.
		say Care to try again?
	endif
	if rand(12)
		echo The ball comes to land on Black 15.
		say Sorry, $n. Black 15 isn't your quad.
		say Care to try again?
	else
		echo The ball comes to land on your quad!
		say You've won, $n! Quadruple odds pay out four to one.
		load obj 21200
		load obj 21200
		load obj 21200
		load obj 21200
		give |21200| $n
		give |21200| $n
		give |21200| $n
		give |21200| $n
		say And, the one you wagered...
		load obj 21200
		give |21200| $n
		say Congratulations! Care to play again?
	endif
else
	say No one plays without chips, $n!
	say If you need chips, buy them at the door!
endif
~
|
