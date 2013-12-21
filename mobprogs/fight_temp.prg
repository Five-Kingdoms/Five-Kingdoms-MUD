>fight_prog 100~
say FIGHT PROG INITIATIED<======
if hitprcnt($n) < 30
  say less then 30
  if rand(50)
    say less then 30, 1
  endif
else
if hitprcnt($n) >= 30
  say more then 30
  if rand(20)
    say   1
  else
    if rand(25)
      say 2
    else
      if rand(30)
         say 3
      else
        if rand(40)
          say 4
        else
          if rand(50)
            say 5
          else
            say 6
          endif
        endif
      endif
    endif
  endif
endif
endif

~
|
  
