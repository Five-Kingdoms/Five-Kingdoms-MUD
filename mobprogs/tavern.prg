>greet_prog 100~
if ispc($n)
     say Elo' $N! Welcome to the Lusty Lady Tavern.
     if goldamt($n) >= 50
          say I see you have some coin in your purse, $N. Care for a drink?
     else
          emote looks $N up and down.
          say Hrmph, another broke person taking up space in my tavern.
          sigh
     endif
endif
~
|
