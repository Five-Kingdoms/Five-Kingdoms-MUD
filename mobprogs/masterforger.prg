>greet_prog 65~
if ispc($n)
    Say I am the Master Forger of Malag's Crossing. I run this forge.
        Say Welcome, feel free to look around.
else
  say Leave $n, I am busy right now.  Come back another time.
      open south
     force $n south     
     emote shoves $n out the door.
 close south
endif
~
|
