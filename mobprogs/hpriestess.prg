>greet_prog 100~
if ispc($n)
     say Greetings $N! Welcome to the Temple of Pajumet.
     smile $n
endif
if isimmort($n)
     bow $n
endif
if goldamt($n) > 1000
     say I see you have excess gold, care to make a donation $N?
endif
~
|
