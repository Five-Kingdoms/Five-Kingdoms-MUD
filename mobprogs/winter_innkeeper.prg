>give_prog 100~
if ispc($n)
  if mobobj(10334) 
    if mobobj(10830)
       if mobobj(10620)
	say Must you free him?  As you wish.
	echo You feel a blast of power coming from the north.
	load obj 10348
	unlock north
	mpjunk all
       endif
    endif
  endif
endif
~

>rand_prog 4~
if rand(50)
	pmote think hard and scribbles something into a large book.
else
	sigh
	say I have seen better times.
endif
~
|
