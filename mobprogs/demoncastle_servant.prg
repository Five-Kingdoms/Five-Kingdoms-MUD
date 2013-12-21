>greet_prog 100~
if rand(100)
	say Greetings, $N.
	bow $n
endif
~

>speech_prog p food~
if isnpc($n) 
  break
else
	load obj 27022
	give hand $n
	say Sustenance, for you, Master $n.
	bow $n
endif
~

>speech_prog p wine~
if isnpc($n) 
  break
else
	load obj 4707
	give glass $n
	say To assuage your thirst, Master $n.
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
say May I serve you, Master?
~
|
