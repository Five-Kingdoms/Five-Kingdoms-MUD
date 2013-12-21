>fight_prog 30~
if rand(10)
	pmote churns and gurgles.
	pmote vomits out $n!
	teleport $n 11147
	at 11147 pmote vomits out $n!
else
	pmote churns and gurgles.
	pmote tries to vomit out $n, but fails.
endif
~

>act_prog p shudders and dies!~
if ispc($n)
	break
else
	pmote writhes and bleeds!
	echo `!You are expelled from a Craw Wurm's stomach!``
	teleport $n 11147    
endif
~

>death_prog 100~
pmote writhes and bleeds!
echo `!You are expelled from a Craw Wurm's stomach!``
at 11147 emote explodes from trauma!
teleport $n 11147
~
|

