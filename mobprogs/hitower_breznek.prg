>give_prog 100~
if ispc($n)
 if mobobj(225)
  if mobobj(254)
   if mobobj(13270)
    if mobobj(214)
     if mobobj(222)
      if mobobj(9225)
       if mobobj(604)
        if mobobj(9556)
         if mobobj(13253)
          if mobobj(5902)
             say Immortality, it is mine! 
	     echo The chains of light around Athaekeetha bend and shift.
	     echo A bolt of white light shoots from the heavens, and Athaekeetha is no more. 
	     say I have done as I said, and have released him, now go!
	     say Athaekeetha knows what you have done, and you will be rewarded!	
	     bug $n has freed ATHAEKEETHA!
	     qtrans $N 1490
	     mpjunk all
           endif
          endif
         endif
        endif
       endif
      endif
     endif
    endif
   endif
  endif
 endif
endif
~

>rand_prog 20~
if rand(30)
	pmote thinks intently and scribbles arcane symbols in the air.
else
	if rand(40)
		pmote studies Athaekeetha intently.
	else
		if rand(50)
			sigh
			say Why is immortality so hard?  If the gods can have it, why not Breznek? 
		else
			say I would release you Athaekeetha, if I discovered the secret for immortality!
			say Why will you not speak?
		endif
	endif
endif
~
|
