>guild_prog 100~
say HALT! Only Justice may pass beyond this point $N!
pmote blocks $n's way into the jail.	
if isclass($n) == 7
  if level($n) == 30
    say You may "convert" at the doorstep of shaman or healer guild $N
  endif
endif


~
>bloody_prog 100~
say You are too bloody to enter the Jail, $N!
pmote blocks $n's way into the jail.	
~
>justice_prog 100~
say Welcome $N, Upholder of the Laws of Aabahran.
say Execute one of those bastards for me eh?
pmote steps aside and allows $n to pass.
~
|

