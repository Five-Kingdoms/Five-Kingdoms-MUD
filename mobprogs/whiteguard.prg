>greet_prog 100~
if isnpc($n)
     Kill $n
else
if isevil($n)
     Yell Leave this sacred place $n and take your foulness from here!
        glare $n
          Say Go that way $n
            emote points west
else
if isgood($n)
      Bow $n
        Say Welcome back $n.
          Shake $n
            Say You may pass through this gate and head up. 
else
if neutral($n)
         curse $n
            Say Did they not teach you proper manners $n?
               Say Go to the west and go in peace $n.
                emote points west    
         endif
      endif
   endif
endif
~
|
