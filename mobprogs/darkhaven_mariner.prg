>speech_prog p passage~
emote looks you over with his clouded gaze.
'The trip to the dead waters be costin you 50000 gold mate.
~
>bribe_prog 50000~
  emote looks you over with his clouded gaze.
  say Thrice my family has been attacked by the shades, keep yer gold.
  give gold  1.
  if charobj(30010)
    'Heh, good thing you have that little trinket there, else your trip might have been rough.
    pmote gets the boat ready and in seconds you are off.
    gtrans $n 10210
    at 10210 say Arr, you are on your own here, for this not be place for me.
  else
    'Arr, hope you are stronger then you look, without one of those tablets...
    spit
    pmote gets the boat ready and in seconds you are off.
    gtrans $n 10210
    at 10210 say Arr, you are on your own here, for this not be place for me.
  endif
~
|
