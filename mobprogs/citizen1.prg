>greet_prog 75~
if ispc($n)
     say Hello.
     if goldamt($n) >= 100
          say Can you please spare some food?  My son is hungry and I need to feed him. 
       else
          emote looks $N up and down.
          say You look worse off than me.
          mutter
            shrug
     endif
endif
~
|
