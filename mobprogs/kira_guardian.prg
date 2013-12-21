>greet_prog 75~
if isgood($n)
        say Shadowspawn has little tolerance for Lightwalkers. I'd watch
	yourself, $N.
	else
        say The Dark Lord will be with you shortly, $N.
endif
~
>rand_prog 15~
if rand(25)
       emote paces the ebony floors, her long dresses trailing.
else
       if rand(33)
               emote gazes at you. You feel your heart flutter.
       else
              if rand(50)
                      emote toys with a small trinket about her neck.
	      else
                      emote folds her arms across her chest.
                      say I would be careful in Lord Shadowspawn's
		      presence, $N. He is in a foul mood today.
	      endif
       endif
endif
~

