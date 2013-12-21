>greet_prog 100~
if ispc($n)
if rand(75)
  emote sings, "Tiradel... Nir van estal..."
  emote sings, "Uran in sion de pr'an."
  emote strums the lyre a final time and stops singing.
  bow $n
   say May I be of service to you $n?
 else
    if rand(50)
      emote carfully picks a beautiful harmony.
      emote sings, "Rasazra qurrel lorn..."
      emote sings, "Firnir in sion de pr'an."
      emote puts down lyre and finishes singing.
        say Ahhhh, a lovely tune.
      emote takes a deep breath.
        say What can I do for you today $n? 
else
        if rand(25)
          emote sings, "Beronor yewm van estal..."
          emote sings, "Ishon it'lal lorn."
          emote strums the lyre one last time and stops singing.
            say How may I be of service to you $n?
          else
            emote sings, "Markul... Iob'ne uten."
            emote makes a sweeping chime with the lyre.
            emote sings, "In opr'an es yewm."
            emote looks up at you and stops singing.
            emote puts the lyre down.
              say May I help you $n?
         endif
     endif
   endif
endif
~
|
