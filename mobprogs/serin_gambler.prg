>greet_prog 75~
say Hey there, $N.
if rand(50)
	say Dare to play this dangerous game with me?
else
	say Dare play spin the roulette of death?
endif
say It is only 5000 gold per spin!
~
>rand_prog 2~
say Dare to try your luck at my roulette table?
~
>bribe_prog 5000~
mpjunk gold
emote spins her roulette!
if rand(25)
	echo The ball falls on a `8black 9`7!
	say A black 9!
	say Too bad. A black 9 will get you nothing.
	snicker
	say Thanks for playing. Better luck next time.
else
	if rand(50)
		echo The ball falls on a `!red 24`7!
		say A red 24! And you get...
		say A recall potion!
		load obj 4455
		give recall $n
		say Maybe you'll get something better next time.
		grin
	else
		if rand(25)
			echo The ball falls on a `8black 15`7!
			say Congratulations! You have just lost your gold!
			laugh $n
		else
			if rand(67)
				echo The ball falls on a `!red 28`7!
				say You got red 28! And you get...
				say A free trip about the realms!
				load obj 4632
				wave
				say Farewell!				
				recite teleport $n
				cackle
		else
				if rand(40)
					echo The ball falls on a `!red 14`7!
					say A red 14! Most unfortunate indeed!
					say You have lost! And along with the defeat, 
					say You will also pay with your life!
					slay $n
					get all corpse
					say Most unfortunate indeed.
					snicker self
				else
					if rand(75)
						echo The ball falls on a `#gold 11`7!
						say A gold 11? Not bad!
						say You have won.
						load obj 3652
						give trinket $n
						say I bet you can't beat the roulette again!
					else
						if rand (60)
							echo The ball falls on a `#gold 7`7!
							say A gold 7? What a lucky number!
							say You have won.
							yell Come here, gimp!
							at 3670 force boneez get bag
							trans boneez
							chown bag boneez
							slay boneez
							drop bag
							sac bag
							force $n get roulette
							get roulette
							say I dare you to try your luck again!
						else
							echo The ball falls on a `#gold 21`7!
							say A gold 21?!? What is this?
							say I have not seen this one before!
							sigh
							say You have won.
							yell Come here, gimp!
							at 3670 force boneez get chest
							trans boneez 		
							chown chest boneez
							slay boneez
							drop chest
							sac chest
							force $n get fortune
							get fortune
							scream
							say There, now leave!
							piss
						endif
					endif
				endif
			endif
		endif
	endif
endif
~
|
