# Scheme-Interpreter
Micro-Scheme Interpreter bootstrapped with C++

Usage
> (+ 1 2 3)
6
> (define (map f l) (if (not (null? l)) (cons (f (car l)) (map f (cdr l)))))
()
> (map (lambda (x) (* x x)) (cons 1 (cons 2 (cons 3 (quote ())))))
(1, 4, 9)
> (define (foreach f l) (if (not (null? l)) (begin (f (car l)) (foreach f (cdr l)))))
()
> (foreach display (list 1 foo 2 bar)) 
1
foo
2
bar
()
