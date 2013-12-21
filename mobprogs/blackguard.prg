>greet_prog 100~
if isnpc($n)
  Kill $n
else
if isevil($n)
     Grin $n
     Say Welcome home $n, I trust all is well?
     Bow $n
else
if isgood($n)
       Say ahHHHHHhhh yes!, fresh doo-goodie meat to eat!
       Say Go to the Ivory Arch $n which is to the east, before you make me angry!
       Puke $n
    emote points east
  else
if neutral($n)
         Sneer $n
       Say Are you really that stupid $n?
       say Go back to the east! The Wishy-Washy Neutral Mages are over there.
  emote points east
endif
endif
endif
endif
~
|
