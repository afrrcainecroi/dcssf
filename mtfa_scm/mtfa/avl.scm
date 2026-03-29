(define-module (mtfa avl)
#:use-module  (mtfa error-handler)
#:use-module  (mtfa utils)
#:use-module  (mtfa serializer)
#:use-module  (mtfa unordered-set) ;;unordered set con chiavi (stringhe, numeri o sumimboli: tutto convertito in stringa). Persistente
#:use-module  (mtfa unordered-map) ;;unordered map con chiavi (stringhe) e valori (qualsiasi cosa). persistente
#:use-module  (mtfa star-map)      ;;Inserisce stringhe con o senza jolly, la stringa che definisce il jolly e il valore. Cerca le stringhe che matchano!
#:use-module  (mtfa simple_db)
#:use-module  (mtfa eis)
;;#:use-module  (mtfa fsm)
;;#:use-module  (mtfa va)
#:use-module  (mtfa extset)  ;;gestisce insiemi i cui elementi sono stringhe! consente operazioni di clone, set, check, get all.... Definisce una macro che consente di creare "al volo" una sottoclasse le cui istanze condividono gli stessi elementi.
#:use-module  (mtfa umset)   ;;è una unordered map (non persistente) che ha stringhe come chiavi e ha insiemi di stringhe come valori. Ogni insert aggiunge all'insieme corrispondente. Definisce inoltre la mtfa-umap-list che consente di mappare liste come chiavi e qualsiasi valore come valore
 ;;
 ;;i moduli di guile
#:use-module  (srfi srfi-1)
#:use-module  (srfi srfi-9)
 ;;(srfi srfi-18) ;;thread e mutex
 ;; date & time rinomina per avere un current time che non si sovrappone
#:use-module  (srfi srfi-19)
#:use-module  (srfi srfi-26)
 ;;(srfi srfi-28)
#:use-module  ((srfi srfi-43) #:prefix datetime:)
#:use-module  (srfi srfi-60)
#:use-module  (web uri)
#:use-module  (system foreign)

#:use-module  (rnrs bytevectors)
#:use-module  (rnrs arithmetic bitwise)
#:use-module  ((rnrs io ports)
  #:select (string->bytevector bytevector->string)
  #:prefix ioports:)

#:use-module  (ice-9 format)
#:use-module  (ice-9 ftw)
#:use-module  (ice-9 rdelim)
#:use-module  (ice-9 pretty-print)
#:use-module  (ice-9 regex)
#:use-module  (ice-9 iconv)
#:use-module  (ice-9 peg)
#:use-module  (ice-9 peg string-peg)
#:use-module  (ice-9 vlist)
#:use-module  (ice-9 q)
#:use-module  (ice-9 binary-ports)
#:use-module  (ice-9 threads)
#:use-module  (ice-9 hash-table)
#:use-module  (ice-9 receive)
#:use-module  (ice-9 control)
#:use-module  (ice-9 optargs)
#:use-module  (ice-9 match)

#:use-module  (oop goops)
;; #:use-module  (sxml simple)
;; #:use-module  (sxml ssax)
;; #:use-module  (sxml xpath)
#:use-module  (json)
)

;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;
(define-record-type <avl-node>
  (make-avl-node key value left right balance father)
  avl-node?
  (key avlKey avlKey!)
  (value avlValue avlValue!)
  (left  avlLC avlLC!)
  (right  avlRC avlRC!)
  (balance avlBalance avlBalance!)
  (father  avlFather avlFather!))

;;Just to make things faster
(define-syntax avlCH
  (syntax-rules () 
    ((_ avl 0) (avlLC avl))
    ((_ avl #t) (avlLC avl))
    ((_ avl 1) (avlRC avl))
    ((_ avl #f) (avlRC avl))
    ((_ avl idx) (if (or (eqv? 0 idx) (eqv? #t idx)) (avlLC avl) (avlRC avl)))
    ))
(define-syntax avlCH!
  (syntax-rules () 
    ((_ avl v 0) (avlLC! avl v))
    ((_ avl v #t) (avlLC! avl v))
    ((_ avl v 1) (avlRC! avl v))
    ((_ avl v #f) (avlRC! avl v))
    ((_ avl v idx) (if (or (eqv? 0 idx) (eqv? #t idx)) (avlLC! avl v) (avlRC! avl v)))
    ))

;;
(define-public (mtfa-avl-make less-proc)
  "Makes an avl-tree with search, delete, insert, .... Wants the less function.
usage: (define avl (mtfa-avl-make <))
"
  (cons (make-avl-node '() '() '() '() 0 '()) less-proc))  ;;Il balance della root è il numero di nodi (ma non ora)

;;La procedura di ribilanciamento
(define (rebalance father)
  ;;father è il nodo sbilanciato. Qui ho diversi casi
  (let* ((ancestor (avlFather father))
	 (leftAncestor (eqv? (avlLC ancestor) father))
	 (y father)
	 (w '())
	 (x '()))
    (if (= -2 (avlBalance y))
	(begin
	  (set! x (avlLC y))
	  (if (= -1 (avlBalance x))
	      (begin
		(set! w x)
		(avlLC! y (avlRC x))
		(unless (null? (avlLC y)) (avlFather! (avlLC y) y))
		(avlRC! x y)
		(avlFather! y x)
		(avlBalance! x 0)
		(avlBalance! y 0)
		(if leftAncestor (avlLC! ancestor x)(avlRC! ancestor x))
		(avlFather! x ancestor)
		#t)
	      (begin
		(set! w (avlRC x))
		(avlRC! x (avlLC w))
		(unless (null? (avlRC x)) (avlFather! (avlRC x) x))
		(avlLC! w x)
		(avlFather! x w)
		(avlLC! y (avlRC w))
		(unless (null? (avlLC y)) (avlFather! (avlLC y) y))
		(avlRC! w y)
		(avlFather! y w)
		(if (= -1 (avlBalance w))
		    (begin
		      (avlBalance! x 0)
		      (avlBalance! y 1))
		    (if (zero? (avlBalance w))
			(begin
			  (avlBalance! x 0)
			  (avlBalance! y 0))
			(begin
			  (avlBalance! x -1)
			  (avlBalance! y 0))))
		(avlBalance! w 0)
		(if leftAncestor (avlLC! ancestor w)(avlRC! ancestor w))
		(avlFather! w ancestor)
		#t)))
	(if (= 2 (avlBalance y))
	    (begin
	      (set! x (avlRC y))
	      (if (= 1 (avlBalance x))
		  (begin
		    (set! w x)
		    (avlRC! y (avlLC x))
		    (unless (null? (avlRC y)) (avlFather! (avlRC y) y))
		    (avlLC! x y)
		    (avlFather! y x)
		    (avlBalance! x 0)
		    (avlBalance! y 0)
		    (if leftAncestor (avlLC! ancestor x)(avlRC! ancestor x))
		    (avlFather! x ancestor)
		    #t)
		  (begin
		    (set! w (avlLC x))
		    (avlLC! x (avlRC w))
		    (unless (null? (avlLC x)) (avlFather! (avlLC x) x))
		    (avlRC! w x)
		    (avlFather! x w)
		    (avlRC! y (avlLC w))
		    (unless (null? (avlRC y)) (avlFather! (avlRC y) y))
		    (avlLC! w y)
		    (avlFather! y w)
		    (if (= 1 (avlBalance w))
			(begin
			  (avlBalance! x 0)
			  (avlBalance! y -1))
			(if (zero? (avlBalance w))
			    (begin
			      (avlBalance! x 0)
			      (avlBalance! y 0))
			    (begin
			      (avlBalance! x 1)
			      (avlBalance! y 0))
			    )
			)
		    (avlBalance! w 0)
		    (if leftAncestor (avlLC! ancestor w)(avlRC! ancestor w))
		    (avlFather! w ancestor)
		    #t)))))))
;;
;;termina quando arriva alla radice oppure l'altezza del nodo non cresce
;;oppure quando dovrà ribilanciare
(define (rebalance-ancestors key father less)
  (if (null? (avlFather father))
      #t 	;;sono arrivato alla radice. Mi devo fermare.
      (begin
	;;che ramo ho incrementato? sinistro o destro?
	(if (less key (avlKey father))
	    (avlBalance! father (1- (avlBalance father)))
	    (avlBalance! father (1+ (avlBalance father))))
	(if (zero? (avlBalance father))
	    #t ;;finiamola qui, tutto ok
	    (begin
	      ;;Ora mi preoccupo del ribilanciamento!!!
	      (if (or (= -2 (avlBalance father)) (= 2 (avlBalance father)))
		  (begin
		    ;;devo ribilanciare!!
		    (rebalance father)
		    )
		  (begin
		    ;;non devo ribilanciare, continuo la salita!!
		    (rebalance-ancestors key (avlFather father) less))))))))

;;
(define*-public (avl-search avl key . less)
  (define (-avl-search node key less)
    (if (null? node)
	'()
	(if (less key (avlKey node))
	    (-avl-search (avlLC node) key less)
	    (if (less (avlKey node) key)
		(-avl-search (avlRC node) key less)
		node))))
  (-avl-search (avlLC (car avl)) key (if (null? less) (cdr avl) (car less))))
;;
(define*-public (avl-insert avl key value)
  "Returns '() if the key alread exists, otherwise return the node"
  (define (-avl-insert node key value father from-left less)
    (if (null? node)
	(begin
	  (set! node (make-avl-node key value '() '() 0 father))
	  (avlCH! father node from-left)
	  (rebalance-ancestors key father less)
	  (values #t node))
	(if (less key (avlKey node))
	    (-avl-insert (avlLC node) key value node #t less)
	    (if (less (avlKey node) key)
		(-avl-insert (avlRC node) key value node #f less)
		(values #f node)))))
  (-avl-insert (avlLC (car avl))    key value (car avl) #t     (cdr avl)))
;;
;;current-node key  father from-left less-proc (the external-root is the node that points to the avl root)
(define-public (avl-delete avl key)
  (define (-avl-delete root key father from-left less)
    (let* ((p '())
	   (q '())
	   (r '())
	   (s '())
	   (dir 0)  ;;Or 1
	   (x '())
	   (w '())
	   (y '())
	   (item #f)
	   (external-root father)
	   )
      (set! p (avlLC external-root))
      (let loop ()
	(if (null? p)
	    #t  ;;solo per uscire dal loop
	    (if (less key (avlKey p))
		(begin
		  (set! p (avlLC p))
		  (set! dir 0)
		  (loop))
		(if (less (avlKey p) key)
		    (begin
		      (set! p (avlRC p))
		      (set! dir 1)
		      (loop))
		    (begin
		      ;;trovato!
		      #t)))))
      (if (null? p) ;;non presente, non puoi eliminare
	  '()
	  (begin
	    (set! item p) ;;item è l'elemento che alla fine avrò eliminato!!
	    ;;
	    ;;p è il nodo che devo eliminare
	    (set! q (avlFather p))
	    ;;
	    ;;se q è la root esterna dell'albero, allora sto eliminando la radice dell'albero!!!
	    (when (eq? external-root q)
	      ;; (set! q (avlLC external-root))
	      (set! dir 0))  ;;0 left , 1 right
	    ;;
	    ;;se p non ha figli destri, il suo sinistro lo tiro su
	    (if (null? (avlRC p))
		(begin
		  (avlCH! q (avlLC p) dir)
		  (unless (null? (avlCH q dir))
		    (avlFather! (avlCH q dir) (avlFather p))))
		(begin
		  (set! r (avlRC p))
		  (if (null? (avlLC r))
		      (begin
			(avlLC! r (avlLC p))
			(avlCH! q r dir)
			(avlFather! r (avlFather p))
			(unless (null? (avlLC r)) (avlFather! (avlLC r) r))
			(avlBalance! r (avlBalance p))
			(set! q r)
			(set! dir 1))
		      (begin
			(set! s (avlLC r))
			(while (not (null? (avlLC s))) (set! s (avlLC s)))
			(set! r (avlFather s))
			(avlLC! r (avlRC s))
			(avlLC! s (avlLC p))
			(avlRC! s (avlRC p))
			(avlCH! q s dir)
			(unless (null? (avlLC s)) (avlFather! (avlLC s) s))
			(avlFather! (avlRC s) s)
			(avlFather! s (avlFather p))
			(unless (null? (avlLC r)) (avlFather! (avlLC r) r))
			(avlBalance! s (avlBalance p))
			(set! q r)
			(set! dir 0)))))
	    (set! p '())
	    ;;
	    (while (not (eq? external-root q))  ;;mentre q non è la radice dell'AVL
	      (set! y q)
	      (if (not (eq? (avlFather y) external-root))
		  (set! q (avlFather y))
		  (set! q external-root))
	      (if (zero? dir)
		  (begin
		    (if (eq? y (avlLC q)) (set! dir 0) (set! dir 1))
		    (avlBalance! y (1+ (avlBalance y)))
		    (when (= 1 (avlBalance y))
		      (break #t))
		    ;;
		    (if (= 2 (avlBalance y))
			(begin
			  (set! x (avlRC y))
			  (if (= -1 (avlBalance x))
			      (begin
				(set! w (avlLC x))
				(avlLC! x (avlRC w))
				(avlRC! w x)
				(avlRC! y (avlLC w))
				(avlLC! w y)
				(if (= 1 (avlBalance w))
				    (begin (avlBalance! x 0) (avlBalance! y -1))
				    (if (zero? (avlBalance w))
					(begin (avlBalance! x 0) (avlBalance! y 0))
					(begin (avlBalance! x 1) (avlBalance! y 0))))
				(avlBalance! w 0)
				(avlFather! w (avlFather y))
				(avlFather! x w)
				(avlFather! y w)
				(unless (null? (avlLC x)) (avlFather! (avlLC x) x))
				(unless (null? (avlRC y)) (avlFather! (avlRC y) y))
				(avlCH! q w dir)
				)
			      (begin
				(avlRC! y (avlLC x))
				(avlLC! x y)
				(avlFather! x (avlFather y))
				(avlFather! y x)
				(unless (null? (avlRC y)) (avlFather! (avlRC y) y))
				(avlCH! q x dir)
				(if (zero? (avlBalance x))
				    (begin
				      (avlBalance! x -1)
				      (avlBalance! y 1)
				      (break #t))
				    (begin
				      (avlBalance! x 0)
				      (avlBalance! y 0)
				      (set! y x))))))))
		  (begin
		    (if (eq? y (avlLC q)) (set! dir 0) (set! dir 1))
		    (avlBalance! y (1- (avlBalance y)))
		    (if (= -1 (avlBalance y))
			(break #t))
		    (if (= -2 (avlBalance y))
			(begin
			  (set! x (avlLC y))
			  (if (= 1 (avlBalance x))
			      (begin
				(set! w (avlRC x))
				(avlRC! x (avlLC w))
				(avlLC! w x)
				(avlLC! y (avlRC w))
				(avlRC! w y)
				(if (= -1 (avlBalance w))
				    (begin
				      (avlBalance! x 0)
				      (avlBalance! y 1))
				    (if (zero? (avlBalance w))
					(begin
					  (avlBalance! x 0)
					  (avlBalance! y 0))
					(begin
					  (avlBalance! x -1)
					  (avlBalance! y 0))))
				(avlBalance! w 0)
				(avlFather! w (avlFather y))
				(avlFather! x w)
				(avlFather! y w)
				(unless (null? (avlRC x))
				  (avlFather! (avlRC x) x))
				(unless (null? (avlLC y))
				  (avlFather! (avlLC y) y))
				(avlCH! q w dir)
				)
			      (begin
				(avlLC! y (avlRC x))
				(avlRC! x y)
				(avlFather! x (avlFather y))
				(avlFather! y x)
				(unless (null? (avlLC y))
				  (avlFather! (avlLC y) y))
				(avlCH! q x dir)
				(if (zero? (avlBalance x))
				    (begin
				      (avlBalance! x 1)
				      (avlBalance! y -1)
				      (break #t))
				    (begin
				      (avlBalance! x 0)
				      (avlBalance! y 0)
				      (set! y x))))))))))
	    item))))
  (-avl-delete (avlLC (car avl))  key (car avl) #t     (cdr avl))
  avl)
;;
(define-public (avl-dump avl)
  (define (-avl-dump avl)
    (if (null? avl)
	'()
	(append (list (avlKey avl) (avlValue avl) (avlBalance avl) (avlKey (avlFather avl)))
		(list (-avl-dump (avlLC avl))
		      (-avl-dump (avlRC avl))))))
  (-avl-dump (avlLC (car avl))))
;;
(define-public (avl-print avl)
  (define (-avl-print avl tabs)
    (if (null? avl)
	'()
	(begin
	  (Show! (make-string tabs #\ ) (avlKey avl) ", " (avlValue avl))
	  (-avl-print (avlLC avl)  (+ tabs 2))
	  (-avl-print (avlRC avl)  (+ tabs 2)))))
  (-avl-print (avlLC (car avl)) 0))
;;
(define-public (avl-map avl f)
  (define (-avl-map r f)
    (let ((rl '())
	  (ll '())
	  (cc '())
	  (ret '())
	  )
      (if (not (null? r))
	  (begin
	    (when (not (null? (avlLC r)))
	      (set! ll (-avl-map (avlLC r) f)))
	    (set! cc (f (cons (avlKey r) (avlValue r))))
	    (when (not (null? (avlRC r)))
	      (set! rl (-avl-map (avlRC r) f)))
	    (append ll (cons cc '()) rl))
	  '())))
  (-avl-map (avlLC (car avl)) f))
;;
(define-public (avl-for-each avl f)
  (define (-avl-for-each r f)
    (if (null? r)
	'()
	(begin
	  (when (not (null? (avlLC r)))
	    (-avl-for-each (avlLC r) f))
	  (f (cons (avlKey r) (avlValue r)))
	  (when (not (null? (avlRC r)))
	    (-avl-for-each (avlRC r) f))
	  '())))
  (-avl-for-each (avlLC (car avl)) f))
;;
(define-public (avl-clear avl)
  (avlLC! (car avl) '())
  (avlRC! (car avl) '())
  (avlKey! (car avl) '())
  (avlValue! (car avl) '())
  (avlBalance! (car avl) 0)
  (avlFather! (car avl) '())
  avl)
;;
;;questo è per una visita ordinata, dal più piccolo al più grande
(define-public (avl-next node)
  ;;dato un nodo, il prossimo è, se esiste,
  ;;il più piccolo a destra. se vuoto, altrimenti è il padre ma solo se lui viene da sinistra
  ;;quando il padre è la root esterna, non ci sono più nodi da percorrere
  (if (null? node)
      '()
      (if (not (null? (avlRC node)))
	  (begin
	    (let loop ((n (avlRC node)))
	      (if (null? (avlLC n))
		  n
		  (loop (avlLC n))))
	    )
	  (begin
	    (while (and (not (null? (avlFather node))) (not (null? (avlFather (avlFather node))))
		      (eq? node (avlRC (avlFather node))))
	      (set! node (avlFather node)))
	    ;;se il padre è vuoto, allora ho terminato sono sulla external-root
	    (if (or (null? (avlFather node)) (null? (avlFather (avlFather node))))
		'()
		(avlFather node))))))
;;
;;
(define-public (avl-prec node)
  ;;dato un nodo, il precedente è, se esiste,
  ;;il più grande a sinistra. se vuoto, altrimenti è il padre ma solo se lui viene da destra
					;,quando il padre è la root esterna, non ci sono più nodi da percorrere
  (if (null? node)
      '()
      (if (not (null? (avlLC node)))
	  (let loop ((n (avlLC node)))
	    (if (null? (avlRC n))
		n
		(loop (avlRC n))))
	  (begin
	    (while (and (not (null? (avlFather node)))
		      (eq? node (avlLC (avlFather node))))
	      (set! node (avlFather node)))
	    ;;se il padre è vuoto, allora ho terminato sono sulla external-root
	    (if (null? (avlFather node))
		'()
		(avlFather node))))))
;;
(define-public (avl-value node)
  (avlValue node))
(define-public (avl-value! node value)
  (avlValue! node value))
;;
(define-public (avl-key node)
  (avlKey node))
(define-public (avl-key! node key)
  (avlKey! node key))
;;
(define-public (avl-kv node)
  (cons (avlKey node) (avlValue node)))
;;
(define-public (avl->list avl)
  (let loop ((n (avl-first avl))) (if (null? n) '() (cons (cons (avl-key n) (avl-value n)) (loop (avl-next n))))))
;;
(define-public (avl-clone avl)
  (let ((avl1 (mtfa-avl-make (cdr avl))))
    (avl-for-each avl (lambda (kv) (avl-insert avl1 (car kv) (cdr kv))))
    avl1))
;;
(define-public (avl-first avl)
  (let loop ((r (avlLC (car avl))))
    (if (or (null? r) (null? (avlLC r)))
	r
	(loop (avlLC r)))))
;;
(define-public (avl-last avl)
  (let loop ((r (avlLC (car avl))))
    (if (or (null? r) (null? (avlRC r)))
	r
	(loop (avlRC r)))))
;;
(define*-public (avl-union avl1 avl2 . less)
"When keys are in both, the value of the second tree is stolen"
  (let ((avl (avl-clone avl1)))
    (avl-for-each avl2 (lambda (kv) (avl-insert avl (car kv) (cdr kv))))
    avl))
;;
(define-public (avl-intersection avl1 avl2 . less)
  ;;sono tutti e due root
  (let* ((avl (mtfa-avl-make (if (null? less) (cdr avl1) less)))
	 (less (cdr avl))
	 (n1 (avl-first avl1))
	 (n2 (avl-first avl2)))
    (while (and (not (null? n1)) (not (null? n2)))
      (when (less (avl-key n1) (avl-key n2))
	(set! n1 (avl-next n1))
	(continue))
      (when (less (avl-key n2) (avl-key n1))
	(set! n2 (avl-next n2))
	(continue))
      (avl-insert avl (avl-key n1) (avl-value n1))
      (set! n1 (avl-next n1))
      (set! n2 (avl-next n2))
      )
    avl))

(define-public (avl-before avl k)
  "Find the node whose key the highest among the smaller than k"
  (if (and (null? avl) (null? (avlLC (car avl))))
      '()  ;;empty tree
      (let loop ((r (avlLC (car avl))) (less (cdr avl)))
	(if (null? r)
	    '()
	    (if (less (avl-key r) k)
		(begin
		  ;;vado a destra fino a che ci sono nodi oppure ne trovo uno più grande
		  ;;r è più piccolo di k e quindi va bene, se non trovo nulla, allora torno r
		 (let ((found (loop (avlRC r) less)))
		  (if (null? found)
		      r
		      found)))
		(begin
		  ;;ok, ho trovato una chiave che non è più piccola di k, potrebbe essere maggiore o uguale
		  (if (less k (avl-key r))
		      (begin
			;;ho trovato una chiave più grande di k, vado a sinistra per lo stesso motivo di prima
			;,se non troverò nulla, allora non posso tornare r che è più grande di k
			;;devo tornare null e quindi se l'ho trovato, ok altrimenti non c'era
			(loop (avlLC r) less))
		      (begin
			;;le due chiavi sono uguali, devo andare verso sinistra e vale lo stesso discorso, se
			;;non trovo nulla, nulla c'è
			(loop (avlLC r) less)))))))))
;;
(define-public (avl-after avl k)
  "Find the node whose key the lowest among the bigger ones than k"
  (if (and (null? avl) (null? (avlLC (car avl))))
      '()  ;;empty tree
      (let loop ((r (avlLC (car avl))) (less (cdr avl)))
	(if (null? r)
	    '()
	    (if (less k (avl-key r))
		(begin
		  ;;vado a sinistra fino a che ci sono nodi oppure ne trovo uno più grande
		  ;;r è più piccolo di k e quindi va bene, se non trovo nulla, allora torno r
		 (let ((found (loop (avlLC r) less)))
		  (if (null? found)
		      r
		      found)))
		(begin
		  ;;ok, ho trovato una chiave che non è più piccola di k, potrebbe essere maggiore o uguale
		  (if (less (avl-key r) k)
		      (begin
			;;ho trovato una chiave più grande di k, vado a sinistra per lo stesso motivo di prima
			;,se non troverò nulla, allora non posso tornare r che è più grande di k
			;;devo tornare null e quindi se l'ho trovato, ok altrimenti non c'era
			(loop (avlRC r) less))
		      (begin
			;;le due chiavi sono uguali, devo andare verso sinistra e vale lo stesso discorso, se
			;;non trovo nulla, nulla c'è
			(loop (avlRC r) less)))))))))

(define-public (avl-size avl)
  (let ((count 0)) (avl-for-each avl (lambda (i) (set! count (1+ count))))))

;;Test
#|
(load "mtfa-eqt.scm")
(define avl1 (mtfa-avl-make <))
(define avl2 (mtfa-avl-make <))
(define keys1 (collect 20 (remainder (mtfa-rand-ui) 100)))
(define keys2 (collect 20 (remainder (mtfa-rand-ui) 100)))
(for-each (lambda (i) (avl-insert avl1 i (mtfa-rand-ui))) keys1)
(for-each (lambda (i) (avl-insert avl2 i (mtfa-rand-ui))) keys2)
(avl-map avl1 (lambda (k) k)) 
(define avl3 (avl-clone avl1))
(avl-map avl3 (lambda (k) k))
(avl-clear avl3)
(avl-map avl3 (lambda (k) k))
(avl-delete avl1 59)
(let ((n (avl-first avl1))) (while (not (null? n)) (Show! (avl-key n))(set! n (avl-next n))))
...(avl->list (avl-intersection avl1 avl2))

|#
