(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa brg)
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

#:use-module  (oop goops)
;; #:use-module  (sxml simple)
;; #:use-module  (sxml ssax)
;; #:use-module  (sxml xpath)
#:use-module  (json)

  #:export (
	mtfa-brg-make
	mtfa-brg?
	mtfa-brg-next
	mtfa-brg-get
	mtfa-brg-id
  )
)

(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_brg.so" "init_mtfa_brg")

;;Questo è 0xFFFFFFFFFFFFFFFF => 18446744073709551615
;;Questo è 0x8000000000000000 =>  9223372036854775808

(define-public (make-butterfly id seed)
  "Id, indica il nome del generatore. Il seed è un disturbatore"
  (let* ((brg (mtfa-brg-make id seed))
         (data (begin (mtfa-brg-next brg) (mtfa-brg-get brg)))
         (datastart 0)
         (appo #f)  ;;la uso come variabile di lavoro
         )
    (lambda ( . params)
      (cond
       ((or (zero? (length params)) (eqv? (car params) 'h)(eqv? (car params) 'help))
        (Show! "Commands: 'get <num of bytes> 'help")
        #t
        )
       ((eqv? (car params) 'get)
        (set! appo (second params))
        (let loop ((ret (make-bytevector appo)) (needed appo) (retpos 0))
          (if (<= needed (- (bytevector-length data) datastart))
              (begin
                (bytevector-copy! data datastart ret retpos needed)
                (set! datastart (+ datastart needed))
                ret)
              (begin
                ;;non sono sufficienti, distribuisco tra più chiamate
                (bytevector-copy! data datastart ret retpos (- (bytevector-length data) datastart))
                (set! retpos (+ retpos (- (bytevector-length data) datastart)))
                (set! appo (- needed (- (bytevector-length data) datastart)))
                (set! datastart 0)
                (mtfa-brg-next brg)
                (set! data (mtfa-brg-get brg))
                (loop ret appo retpos)))))
       (#t #f)))))
