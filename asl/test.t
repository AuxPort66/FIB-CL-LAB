function f
  params
    v
  endparams

  vars
    c 10
    i 1
  endvars

     %1 = v
     %2 = 'e'
     writec %2
     %2 = 'n'
     writec %2
     %2 = ' '
     writec %2
     %2 = 'f'
     writec %2
     %2 = '.'
     writec %2
     %2 = ' '
     writec %2
     %2 = 'c'
     writec %2
     %2 = ':'
     writec %2
     %2 = ' '
     writec %2
     %3 = 0
     i = %3
  label startWhile1 :
     %4 = 10
     %5 = i < %4
  label while1 :
     ifFalse %5 goto endwhile1
     %7 = c
     %6 = %7[i]
     writei %6
     %8 = ' '
     writec %8
     %9 = 1
     %10 = i + %9
     i = %10
     goto startWhile1
  label endwhile1 :
     %11 = '\n'
     writec %11
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
     %8 = &d
     v = %8
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
     %7 = a
     %8 = 'd'
     writec %8
     %8 = 'e'
     writec %8
     %8 = 's'
     writec %8
     %8 = 'p'
     writec %8
     %8 = 'r'
     writec %8
     %8 = 'e'
     writec %8
     %8 = 's'
     writec %8
     %8 = ' '
     writec %8
     %8 = 'd'
     writec %8
     %8 = 'e'
     writec %8
     %8 = ' '
     writec %8
     %8 = 'b'
     writec %8
     %8 = '='
     writec %8
     %8 = 'a'
     writec %8
     %8 = '.'
     writec %8
     %8 = ' '
     writec %8
     %8 = 'b'
     writec %8
     %8 = ':'
     writec %8
     %8 = ' '
     writec %8
     %9 = 0
     i = %9
  label startWhile2 :
     %10 = 10
     %11 = i < %10
  label while2 :
     ifFalse %11 goto endwhile2
     %13 = b
     %12 = %13[i]
     writei %12
     %14 = ' '
     writec %14
     %15 = 1
     %16 = i + %15
     i = %16
     goto startWhile2
  label endwhile2 :
     %17 = '\n'
     writec %17
     %18 = 'd'
     writec %18
     %18 = 'e'
     writec %18
     %18 = 's'
     writec %18
     %18 = 'p'
     writec %18
     %18 = 'r'
     writec %18
     %18 = 'e'
     writec %18
     %18 = 's'
     writec %18
     %18 = ' '
     writec %18
     %18 = 'd'
     writec %18
     %18 = 'e'
     writec %18
     %18 = ' '
     writec %18
     %18 = 'b'
     writec %18
     %18 = '='
     writec %18
     %18 = 'a'
     writec %18
     %18 = '.'
     writec %18
     %18 = ' '
     writec %18
     %18 = 'a'
     writec %18
     %18 = ':'
     writec %18
     %18 = ' '
     writec %18
     %19 = 0
     i = %19
  label startWhile3 :
     %20 = 10
     %21 = i < %20
  label while3 :
     ifFalse %21 goto endwhile3
     %23 = a
     %22 = %23[i]
     writei %22
     %24 = ' '
     writec %24
     %25 = 1
     %26 = i + %25
     i = %26
     goto startWhile3
  label endwhile3 :
     %27 = '\n'
     writec %27
     %28 = &a
     pushparam %28
     call f
     popparam %28
     %29 = 'd'
     writec %29
     %29 = 'e'
     writec %29
     %29 = 's'
     writec %29
     %29 = 'p'
     writec %29
     %29 = 'r'
     writec %29
     %29 = 'e'
     writec %29
     %29 = 's'
     writec %29
     %29 = ' '
     writec %29
     %29 = 'd'
     writec %29
     %29 = 'e'
     writec %29
     %29 = ' '
     writec %29
     %29 = 'f'
     writec %29
     %29 = '('
     writec %29
     %29 = 'a'
     writec %29
     %29 = ')'
     writec %29
     %29 = '.'
     writec %29
     %29 = ' '
     writec %29
     %29 = 'a'
     writec %29
     %29 = ':'
     writec %29
     %29 = ' '
     writec %29
     %30 = 0
     i = %30
  label startWhile4 :
     %31 = 10
     %32 = i < %31
  label while4 :
     ifFalse %32 goto endwhile4
     %34 = a
     %33 = %34[i]
     writei %33
     %35 = ' '
     writec %35
     %36 = 1
     %37 = i + %36
     i = %37
     goto startWhile4
  label endwhile4 :
     %38 = '\n'
     writec %38
     pushparam %28
     call g
     popparam %28
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


