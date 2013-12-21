>Speech_prog p passage~
emote looks you over with his clouded gaze.
  if charobj(30010)
    if isclass($n) == 8
      say Arr, I once had a monk friend.. For you the trip to the dead waters will be but 5000 gold.
    else
      say The trip to the dead waters will cost you 50000 gold mate.
    endif
  else
    emote narrows his eyes and an evil aura surrounds the room.
    say Arr, get out of here before the dead reach you $n.
  endif
~
>bribe_prog 50000~
  emote looks you over with his clouded gaze.
  if charobj(30010)
    'heh, good thing you have that little trinket there, else your trip might have been rough.
    pmote gets the boat ready and in seconds $n is off.
    gtrans $n 10210
    at 10210 say Arr, you are on your own here, for this not be place for me.
    mpjunk gold
  else
    'Arr, hope you are stronger then you look, without one of those tablets...
    spit
    pmote gets the boat ready and in seconds $n is off.
    gtrans $n 10210
    at 10210 say Arr, you are on your own here, for this not be place for me.
    mpjunk gold
  endif
~

>bribe_prog 5000~
if isclass($n) == 8
  emote looks you over with his clouded gaze.
  if charobj(30010)
    'heh, good thing you have that little trinket there, else your trip might have been rough.
    pmote gets the boat ready and in seconds $n is off.
    gtrans $n 10210
    at 10210 say Arr, you are on your own here, for this not be place for me.
    mpjunk gold
  else
    'Arr, hope you are stronger then you look, without one of those tablets...
    spit
    pmote gets the boat ready and in seconds $n is off.
    gtrans $n 10210
    at 10210 say Arr, you are on your own here, for this not be place for me.
    mpjunk gold
  endif
else
    say Heh, thanks there mate.
    mpjunk gold
endif
~

>bribe_prog 1~
say Heh, thanks there mate.
    mpjunk gold
~
|
