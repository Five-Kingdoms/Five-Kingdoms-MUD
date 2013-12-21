>greet_prog 25~
if rand(100)
	say Greetings. Would you care for a drink or appetizer?
	say I have chilled juice, wine, caviar, or some grapes.
endif
~

>speech_prog p juice~
if isnpc($n) 
  break
else
	load obj 4706
	give juice $n
	say There you go, $n.
	bow $n
endif
~

>speech_prog p wine~
if isnpc($n) 
  break
else
	load obj 4707
	give glass $n
	say There you are, $n.
	bow $n
endif
~

>speech_prog p caviar~
if isnpc($n) 
  break
else
	load obj 4708
	give caviar $n
	say There you are, $n.
	bow $n
endif
~

>speech_prog p grapes~
if isnpc($n) 
	break
else
	load obj 4709
	give grapes $n
	say There you are, $n.
	bow $n
endif
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
pmote looks at you expectantly.
say May I serve you?
say Would you like juice, wine, caviar, or grapes?
~
|
