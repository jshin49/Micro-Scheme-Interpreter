# Scheme-Interpreter
Micro-Scheme Interpreter bootstrapped with C++

## Usage

```
> (+ 1 2 3)
6
> (define (map f l) (if (not (null? l)) (cons (f (car l)) (map f (cdr l)))))
()
> (map (lambda (x) (* x x)) (cons 1 (cons 2 (cons 3 (quote ())))))
(1, 4, 9)
> (define (foreach f l) (if (not (null? l)) (begin (f (car l)) (foreach f (cdr l)))))
()
> (foreach display (cons 1 (cons foo (cons 2 (cons bar (quote ()))))))
1
foo
2
bar
()
> > (+ 2 3 4 5)
14
> (+ 2 3.5)
5.5
> (+ 2)
2
> (+)
0
> (+ (+ -2 5 -1) 3.5)
5.5
> (ceiling 4.7)
5
> (ceiling 4.0)
4
> (ceiling -4.7)
-4
> (if 5 7 8.3)
7
> (if 0 7 8.3)
8.3
> (if (+ 2 3) (ceiling 6.1) (+ 4 2.3 2))
7
> (if (+ 2 -2) (ceiling 6.1) (+ 4 2.3 2))
8.3
> (+ 1 (if (+ 2 -2) (ceiling 6.1) (+ 4 2.3 2)))
9.3

Library
(define printf (lambda () 
		 (print (quote Hello)) 
		 (print (quote World)) 
		 (print (quote !!))))

(define > (lambda (x y) (< y x)))

(define >= (lambda (x y) (not (< x y))))

(define <= (lambda (x y) (not (< y x))))

(define = (lambda (x y) (if (< x y) 0 (not (< y x)))))

(define positive? (lambda (x) (< 0 x)))

(define zero? (lambda (x) (= 0 x)))

(define abs (lambda (x) (if (< x 0) (- 0 x) x)))

(define factorial (lambda (x) (if (< x 2) 1
				  (* x (factorial (- x 1))))))

(define square (lambda (x) (print (* x x))))

(define for-each (lambda (f l) (f (car l))
			 (if (not (null? (cdr l)))
			     (for-each f (cdr l))
			     (print (quote END)))))

(define list-tail (lambda (list k) 
		    (if (= k 0)
			list
		       (list-tail 
			(cdr list) (- k 1)))))

(define list-ref (lambda (list k)
		   (if (= k 0)
		       (car list)
		       (list-ref
			(cdr list) (- k 1)))))

(define list-ref (lambda (list k)
		   (car
		    (list-tail list k))))

(define append-elem (lambda (list1 list2)
		      (if (null? list1)
			  list2
			  (cons
			   (car list1) 
			   (append-elem (cdr list1) list2)))))

(define reverse (lambda (list)
		  (if (null? list)
		      (quote ())
		      (append-elem 
		       (reverse (cdr list)) (car list)))))

(define equal? (lambda (obj1 obj2)
		 (if (not (list? obj1))
		     (if (not (list? obj2))
			 (= obj1 obj2)
			 0)
		     (if (list? obj2)
			 (if (list? (car obj1))
			     (if (list? (car obj2))
				 (if (not (equal? (car obj1) (car obj2)))
				     0
				     (if (not (null? (cdr obj1)))
					 (if (not (null? (cdr obj2)))
					     (equal? (cdr obj1) (cdr obj2))
					     0)
					 (if (null? (cdr obj2))
					     1
					     0)))
				 0)
			     (if (not (= (car obj1) (car obj2)))
				 0
				 (if (not (null? (cdr obj1)))
				     (if (not (null? (cdr obj2)))
					 (equal? (cdr obj1) (cdr obj2))
					 0)
				     (if (null? (cdr obj2))
					 1
					 0))))
			 0))))			   
 			 
(define assocl (lambda (obj list)
		 (if (not (list? list))
		     (quote (ERROR: not a list))
		     (if (not (list? (car list)))
			 (quote (ERROR: car is not a list))
			 (if (equal? obj (car (car list)))
			     (car list)
			     (if (list? (car (car list)))
				 (assocl obj (car list))
				 (if (null? (cdr list))
				     0
				     (assocl obj (cdr list)))))))))

(define list-partition (lambda (f list)
			 (if (f (car list))
			     (cons
			      (cons 
			       (car list) (car ((list-partition f (cdr list)))))
			       (cdr ((list-partition f (cdr list)))))
			     (cons
			      (car ((list-partition f (cdr list))))
			      (cons (car list)
				    (car (cdr ((list-partition f (cdr list))))))))))

(define list-sort (lambda (f list)
		    (append-elem (quote ()) (car (list-partition f list)))
		    (list-sort (cdr (list-partition f list)))))
