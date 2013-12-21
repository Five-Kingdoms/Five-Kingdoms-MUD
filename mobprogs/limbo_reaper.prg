>speech_prog p death~
if isnpc($n)
  break
endif
chuckle
'So be it.
pmote slices off your head with a giant sickle!
echo You collapse to the ground as the last visions of life leave your dying body.
slay $n
pray $n has chosen death. More souls to the fire!
get all corpse
c 'soul capture' corpse
~

>rand_prog 10~
pmote looks at you with mock pity.
say It can be so much easier, $n. Simply say 'death', and the pain will end.
cast 'comprehend language'
restore self
~
|
