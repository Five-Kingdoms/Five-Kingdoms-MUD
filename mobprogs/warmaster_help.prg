>greet_prog 100~
if rand(100)
  if iscabal($n) == 4 
	say Greetings, $n. I can aid you in the use of your skills.
	say Say which skill you would like to learn more about.
	say There is a list on the wall behind me.		
  else
	growl $n
	say You don't belong here!
  endif
endif
~

>speech_prog p truesight~
if isnpc($n)
  break
endif
if iscabal($n) != 4 
  break
else
	say Truesight:  
	say Syntax: truesight
	say Truesight enables the Warmaster to see those of his enemies that normally cannot be seen due to their magic.
endif
~

>speech_prog p deathblow~
if isnpc($n)
  break
endif 
if iscabal($n) != 4 
  break
else
	say Deathblow:
	say Syntax: <automatic>
	say When performed, this skill lets a Warmaster strike his enemy so hard that even the most powerful protections will seem as nothing.
endif
~

>speech_prog p ram~
if isnpc($n)
  break
endif 
if iscabal($n) != 4
  break
else
	say Ram:
	say Syntax: ram <direction>
	say Bereft of magical means of opening doors, a Warmaster can simply use his strength to ram it open.
endif
~

>speech_prog p stance~
if isnpc($n)
  break
endif 
if iscabal($n) != 4
  break
else
	say Stance:
	say Syntax: stance
	say When in a combat stance a Warmaster can endure enormous amounts of damage, his superb techniques protecting him from harm and magical ailments.
endif
~

>speech_prog p anger~
if isnpc($n)
  break
endif 
if iscabal($n) != 4
  break
else
	say Anger:
	say Syntax: anger
	say By letting loose his boundless anger a Warmaster can enhance him combat capabilities. While in anger he will also attack anyone, friend or foe, that happens to be near him.
endif
~

>speech_prog p forge~
if isnpc($n)
  break
endif 
if iscabal($n) != 4
  break
else
	say Forge:
	say Syntax: forge <weapon>
	say Most Warmasters become capable weaponsmiths. This skill is what they use to hone their weapons to deadly perfection.
endif
~

>speech_prog p climb~
if isnpc($n)
  break
endif 
if iscabal($n) != 4
  break
else
	say Climb:
	say Syntax: <automatic>
	say While Warmasters cannot fly, they can climb where fly would otherwise be required, allowing their enemies no avenue of escape.
endif
~

>speech_prog p warmaster code~
if isnpc($n)
  break
endif 
if iscabal($n) != 4
  break
else
	say Warmaster Code:
	say This sacred ritual reinforces a Warmaster's pledge to Lord Athaekeetha, the supreme god of combat. It is an honored tradition amongst the masters of war.
endif
~

>speech_prog p challenge~
if isnpc($n)
  break
endif 
if iscabal($n) != 4
  break
else
	say Challenge:
	say Syntax: challenge <name>
	say This skill is used by Warmasters who want to challenge an opponent to combat, usually with a non-lethal ending. It is a Warmaster's way of testing someone's skill.
endif
~

>speech_prog p deathmark~
if isnpc($n)
  break
endif 
if iscabal($n) != 4
  break
else
	say Deathmark:
	say Syntax: deathmark <name>
	say When a Warmaster wants to see the blood of an enemy spilt on the ground, he will often place a deathmark on said enemy. A deathmark will only be lifted by a death, the Warmaster's or his enemy's.
endif
~

>speech_prog p trophy~
if isnpc($n)
  break
endif 
if iscabal($n) != 4
  break
else
	say Trophy:
	say Syntax: trophy <corpse>
	say Given the dead body of an opponent, a Warmaster can use this skill to create a trophy for all to see. Some Warmasters carry trophies of skilled enemies with them for years.
endif
~

>speech_prog p forms~
if isnpc($n)
  break
endif 
if iscabal($n) != 4
  break
else
	say Forms:
	say Syntax: <automatic>
	say A skilled Warmaster, after much extensive training with every weapon type, will eventually learn certain forms useful to him in combat. For each weapon, there is a different associated form that can be used:
	say Strike of the Titan: Used for all sword and axe weapons.
	say Claw of the Lion: Used for all staff and polearm weapons.
	say Ogre's Fury: Used for all whip and flail weapons.
	say Curse of the Serpent: Used for all mace weapons.
	say Lash of the Bull: Used for all spear and dagger weapons.
	say Wrath of the Cobra: Useful in disarming opponents.
	say Desert Sandstorm: Useful for blinding opponents.
	say Flight of the Hawk: Useful for evading attacks.
endif
~

>speech_prog p fatality~
if isnpc($n)
  break
endif 
if iscabal($n) != 4
  break
else
	say Fatality:
	say Syntax: fatality <opponent>
	say Once an opponent has been defeated through a challenge or deathmark, a Warmaster often shows his pride by finishing his opponent in a glorious way. Spoils of fatality are often worn by skilled Warmasters.
endif
~


>rand_prog 10~
cast 'comprehend language'
~
|
