>greet_prog 100~
if ispc($n)
	say Welcome, $n. How my I help you?
	break
else
	say Show your Guild Badges, Guildmasters.
	emote beckons to all of the Guildmasters.
endif
~

>rand_prog 50~
if rand(10)
	emote pores over a large tome, scribbling notes.
	emote frowns to himself.
endif
~

>act_prog p Keruggan walks in.~
if ispc($n)
	break
else
	say Have a fair day, Keruggan.
	echo Keruggan heads off to the Warrior Guild.
	teleport Keruggan 8256
endif
~

>act_prog p Julistha walks in.~
if ispc($n)
        break
else
	say Have a good day, my sweet.
	echo Julistha smiles, and heads off to her guild.
        teleport Julistha 8257
endif
~

>act_prog p Macy walks in.~
if ispc($n)
        break
else
	say Good day to ye, Macy.
	echo Macy nods sleepily, and heads off to his guild.
        teleport Macy 8260
endif
~    
 
>act_prog p Moredhein walks in.~
if ispc($n)
        break
else
	say Teach well, Moredhein.
	echo Moredhain nods, and heads down to his guild.
        teleport Moredhein 8259
endif
~

>act_prog p Aelian flies in.~
if ispc($n)
        break
else
        say Fair day to you, Aelian.
        echo Aelian nods, and heads off to his guild.
        teleport Aelian 8262
endif
~      

>act_prog p Bloonit walks in.~
if ispc($n)
        break
else
        say Have a good day, Bloonit.
        echo Bloonit grumbles, and heads off to his guild.
        teleport Bloonit 8263
endif
~  

>act_prog p Kettick walks in.~
if ispc($n)
        break
else
        nod Kettick
        echo Kettick sighs absently, and heads to work.
        teleport Kettick 8297
endif
~  

>act_prog p Madour walks in.~
if ispc($n)
        break
else
        say Have a good day, Madour.
        echo Madour nods silently.
        teleport Madour 8298
endif
~  
|

