>greet_prog 100~
if rand(100)
	say Greetings, $N. I can aid you in practice weaponry.
	say say the type of the weapon to choose. (dagger, glaive etc.)		
endif
~

>speech_prog p sword~
if isnpc($n) 
  break
endif
if charobj (9770)
	chown |9770| $n
	mpjunk |9770|
	say One practice sword, coming right up!
	pmote toils and sweats on the anvil for a bit.
	load obj 7782
	give sword $n
	say You have made your choice $n. Good luck.
else
	say Not without payment, $n!
endif
~

>speech_prog p flail~
if isnpc($n) 
  break
endif
if charobj (9770)
	chown |9770| $n
	mpjunk |9770|
	say One practice flail coming right up, $n.
	pmote toils on the forge, and hammers some rings together.
	load obj 7786
	give flail $n
	say You have made your choice $n. Good luck!
else
	say Not without payment, $n!
endif
~

>speech_prog p spear~
if isnpc($n) 
  break
endif
if charobj (9770)
	chown |9770| $n
	mpjunk |9770|
	say Aye, $n, one practice spear, coming right up.
	pmote hammers together a tip, and fastens it to a long wood pole.
	load obj 7783
	give spear $n
	say There, you go, $n. Good luck!
else
	say Not without payment, $n!
endif
~

>speech_prog p dagger~
if isnpc($n) 
  break
endif
if charobj (9770)
	chown |9770| $n
	mpjunk |9770|
	say One practice dagger, coming right up, $n.
	pmote forges an ingot into a short, blunt dagger.
	load obj 7781
	give dagger $n
	say You have made your choice $n. Good luck!
else
	say Not without payment, $n!
endif
~

>speech_prog p mace~
if isnpc($n) 
  break
endif
if charobj (9770)
	chown |9770| $n
	mpjunk |9770|
	say One practice mace, coming right up, $n.
	pmote hammers and toils over the anvil.
	load obj 7780
	give mace $n
	say You have made your choice $n. Good luck!
else
	say Not without payment, $n!
endif
~

>speech_prog p axe~
if isnpc($n) 
  break
endif
if charobj (9770)
	chown |9770| $n
	mpjunk |9770|
	say One practice axe, coming right up!
	echo sparks fly from the anvil as the axe comes into shape.
	load obj 7785
	give axe $n
	say You have made your choice $n. Good luck.
else
	say Not without payment, $n!
endif
~


>speech_prog p staff~
if isnpc($n) 
  break
endif
if charobj (9770)
	chown |9770| $n
	mpjunk |9770|
	say One practice staff, coming right up!
	pmote quickly carves out a staff for $n.
	load obj 7784
	give staff $n
	say You have made your choice $n. Good luck.
else
	say Not without payment, $n!
endif
~

>speech_prog p whip~
if isnpc($n) 
  break
endif
if charobj (9770)
	chown |9770| $n
	mpjunk |9770|
	say One practice whip, coming right up!
	pmote grabs a few loose strands of leather and fashions a whip.
	load obj 7787
	give whip $n
	say May it serve you well.
else
	say Not without payment, $n!
endif
~




>speech_prog p glaive~
if isnpc($n) 
  break
endif
if charobj (9770)
	chown |9770| $n
	mpjunk |9770|
	say One practice glaive, coming right up!
	pmote begins to hammer out a great halberd, the sparks showering the room.
	load obj 7788
	give glaive $n
	say May it serve you well.
else
	say Not without payment, $n!
endif
~


>speech_prog p polearm~
if isnpc($n) 
  break
endif
  say you mean a glaive maybe?
~

>speech_prog p death~
if isnpc($n) 
  break
endif
chuckle
'So be it.
pmote rips open a rift into the Ethereal Nexus!
echo Seems everyone around here is a demon...
teleport $n 7
~

>rand_prog 10~
pmote looks at you as if growing impatient.
say Hey, $n. I got work to do. What will it be?
say say the type of the weapon to choose. (dagger, glaive etc.)
cast 'comprehend language'
~
|
