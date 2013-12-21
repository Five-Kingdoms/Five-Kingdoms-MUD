>guild_prog 100~
say You are not a member of our guild, $N!
say I will not allow you to enter!
pmote blocks $n from entering the guild.
if isclass($n) == 7
  if level($n) == 30
    say You may "convert" at the doorstep of shaman or healer guild $N
  endif
endif


~
>bloody_prog 100~
say You are too bloody to enter our guild, $N!
say Take refuge elsewhere until a later time.
pmote blocks $n from entering the guild.
~
>justice_prog 100~
say Welcome $N, Upholder of the Laws.
say 
say We all do our part in keeping this town safe from crime.
pmote steps aside and allows $n to pass.
~
|

