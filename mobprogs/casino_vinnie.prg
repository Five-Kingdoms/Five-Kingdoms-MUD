>rand_prog 20~
if rand(50)
	goto 21220
	pmote shuffles through some papers.
else
	goto 21220
	pmote rattles some gold around in a sack.
endif
~

>fight_prog 5~
say You're mine now, pretty boy!
bash $n
~

>speech_prog p Thanks, Vinnie!~
if ispc($n)
	break
else
	say Get out of here, woman. I'll deal with this punk.
	slap $n
	restore $n
	force $n flee
	say We got a problem here?
	emote crosses his massive arms, and glares at you.
endif
~

>act_prog p is DEAD!~
grumble
say B...b...be more careful next time.
pmote chuckles mirthlessly to himself.
pmote heads back up to work.
goto 21220
~
|

