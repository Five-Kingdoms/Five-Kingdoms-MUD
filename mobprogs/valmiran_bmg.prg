>fight_prog 75~
if rand(40)
  cast 'sharpmetal'
else
  if rand(35)
      cast 'rust'
  else
    if rand(20)
	cast 'thunderclap'
    else
      if rand(50)
	cast 'localize gravity' primary
      else
	cast 'localize gravity' secondary
      endif
    endif
  endif
endif
~
|