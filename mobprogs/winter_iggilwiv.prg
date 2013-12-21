>fight_prog 19~
  pmote summons the might of his arcane power!
  cast 'hellstream'
~

>rand_prog 3~ 
mpasound The air sizzles with arcane energy as explosions are heard nearby.
if rand(10)
yell You fool!  You are a shame to your kind.  Begone fiend!
mpasound You hear an anguished death cry.  
endif
~

>death_prog 100~
 at 78 load mob 10319
~
|
