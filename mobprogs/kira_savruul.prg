>greet_prog 60~
if rand(50)
	pmote looks at you, and speaks in a seductive voice.
	say You have entered the Dischordant's portal, $N.
        smirk $N
else
	say I'm surprised you made it this far. Let's hope you came prepared.
	smile self
	pmote smoothes her silky dress over her slender, curved body.
endif
~
>rand_prog 15~
if rand(33)
	say The God of Chaos entertains few guests. You must have impressed him.
	grin self
        say ...Or thoroughly angered him.
else
	if rand(50)
		say Can you feel the flow of true Chaos around you?
		pmote breathes deeply and seductively.
	else
		pmote walks the hard stone floor, her spiked heels clicking lightly.
		pmote whispers '`6Come to me, my Dischordant.``'
                emote closes her eyes in reverie.
	endif
endif
~
>speech_prog p crypticant~
if isnpc($n)
  break
else
     say You shall pass into the Keeper's Lair, $n.
     pmote walks seductively to the far door.
     unlock north
     open north
     say Go, $n. The Medium awaits.  
endif
~ 
|

