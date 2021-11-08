function f
  params
    v
  endparams

  vars
    c 10
    i 1
  endvars

     c = &v
     %1 = 'e'
     writec %1
     %1 = 'n'
     writec %1
     %1 = ' '
     writec %1
     %1 = 'f'
     writec %1
     %1 = '.'
     writec %1
     %1 = ' '
     writec %1
     %1 = 'c'
     writec %1
     %1 = ':'
     writec %1
     %1 = ' '
     writec %1
     %2 = 0
     i = %2
  label startWhile1 :
     %3 = 10
     %4 = i < %3
  label while1 :
     ifFalse %4 goto endwhile1
     %6 = c
     %5 = %6[i]
     writei %5
     %7 = ' '
     writec %7
     %8 = 1
     %9 = i + %8
     i = %9
     goto startWhile1
  label endwhile1 :
     %10 = '\n'
     writec %10
     return
endfunction

function g
  params
    v
  endparams

  vars
    d 10
    i 1
  endvars

     %1 = 0
     i = %1
  label startWhile1 :
     %2 = 10
     %3 = i < %2
  label while1 :
     ifFalse %3 goto endwhile1
     %4 = 1
     %5 = - %4
     d[i] = %5
     %6 = 1
     %7 = i + %6
     i = %7
     goto startWhile1
  label endwhile1 :
     v = &d
     return
endfunction

function main
  vars
    a 10
    b 10
    i 1
    j 1
  endvars

     %1 = 0
     i = %1
  label startWhile1 :
     %2 = 10
     %3 = i < %2
  label while1 :
     ifFalse %3 goto endwhile1
     a[i] = i
     %4 = 0
     b[i] = %4
     %5 = 1
     %6 = i + %5
     i = %6
     goto startWhile1
  label endwhile1 :
     b = &a
     %7 = 'd'
     writec %7
     %7 = 'e'
     writec %7
     %7 = 's'
     writec %7
     %7 = 'p'
     writec %7
     %7 = 'r'
     writec %7
     %7 = 'e'
     writec %7
     %7 = 's'
     writec %7
     %7 = ' '
     writec %7
     %7 = 'd'
     writec %7
     %7 = 'e'
     writec %7
     %7 = ' '
     writec %7
     %7 = 'b'
     writec %7
     %7 = '='
     writec %7
     %7 = 'a'
     writec %7
     %7 = '.'
     writec %7
     %7 = ' '
     writec %7
     %7 = 'b'
     writec %7
     %7 = ':'
     writec %7
     %7 = ' '
     writec %7
     %8 = 0
     i = %8
  label startWhile2 :
     %9 = 10
     %10 = i < %9
  label while2 :
     ifFalse %10 goto endwhile2
     %12 = b
     %11 = %12[i]
     writei %11
     %13 = ' '
     writec %13
     %14 = 1
     %15 = i + %14
     i = %15
     goto startWhile2
  label endwhile2 :
     %16 = '\n'
     writec %16
     %17 = 'd'
     writec %17
     %17 = 'e'
     writec %17
     %17 = 's'
     writec %17
     %17 = 'p'
     writec %17
     %17 = 'r'
     writec %17
     %17 = 'e'
     writec %17
     %17 = 's'
     writec %17
     %17 = ' '
     writec %17
     %17 = 'd'
     writec %17
     %17 = 'e'
     writec %17
     %17 = ' '
     writec %17
     %17 = 'b'
     writec %17
     %17 = '='
     writec %17
     %17 = 'a'
     writec %17
     %17 = '.'
     writec %17
     %17 = ' '
     writec %17
     %17 = 'a'
     writec %17
     %17 = ':'
     writec %17
     %17 = ' '
     writec %17
     %18 = 0
     i = %18
  label startWhile3 :
     %19 = 10
     %20 = i < %19
  label while3 :
     ifFalse %20 goto endwhile3
     %22 = a
     %21 = %22[i]
     writei %21
     %23 = ' '
     writec %23
     %24 = 1
     %25 = i + %24
     i = %25
     goto startWhile3
  label endwhile3 :
     %26 = '\n'
     writec %26
     %27 = &a
     pushparam %27
     call f
     popparam 
     %28 = 'd'
     writec %28
     %28 = 'e'
     writec %28
     %28 = 's'
     writec %28
     %28 = 'p'
     writec %28
     %28 = 'r'
     writec %28
     %28 = 'e'
     writec %28
     %28 = 's'
     writec %28
     %28 = ' '
     writec %28
     %28 = 'd'
     writec %28
     %28 = 'e'
     writec %28
     %28 = ' '
     writec %28
     %28 = 'f'
     writec %28
     %28 = '('
     writec %28
     %28 = 'a'
     writec %28
     %28 = ')'
     writec %28
     %28 = '.'
     writec %28
     %28 = ' '
     writec %28
     %28 = 'a'
     writec %28
     %28 = ':'
     writec %28
     %28 = ' '
     writec %28
     %29 = 0
     i = %29
  label startWhile4 :
     %30 = 10
     %31 = i < %30
  label while4 :
     ifFalse %31 goto endwhile4
     %33 = a
     %32 = %33[i]
     writei %32
     %34 = ' '
     writec %34
     %35 = 1
     %36 = i + %35
     i = %36
     goto startWhile4
  label endwhile4 :
     %37 = '\n'
     writec %37
     %38 = &a
     pushparam %38
     call g
     popparam 
     %39 = 'd'
     writec %39
     %39 = 'e'
     writec %39
     %39 = 's'
     writec %39
     %39 = 'p'
     writec %39
     %39 = 'r'
     writec %39
     %39 = 'e'
     writec %39
     %39 = 's'
     writec %39
     %39 = ' '
     writec %39
     %39 = 'd'
     writec %39
     %39 = 'e'
     writec %39
     %39 = ' '
     writec %39
     %39 = 'g'
     writec %39
     %39 = '('
     writec %39
     %39 = 'a'
     writec %39
     %39 = ')'
     writec %39
     %39 = '.'
     writec %39
     %39 = ' '
     writec %39
     %39 = 'a'
     writec %39
     %39 = ':'
     writec %39
     %39 = ' '
     writec %39
     %40 = 0
     i = %40
  label startWhile5 :
     %41 = 10
     %42 = i < %41
  label while5 :
     ifFalse %42 goto endwhile5
     %44 = a
     %43 = %44[i]
     writei %43
     %45 = ' '
     writec %45
     %46 = 1
     %47 = i + %46
     i = %47
     goto startWhile5
  label endwhile5 :
     %48 = '\n'
     writec %48
     return
endfunction


