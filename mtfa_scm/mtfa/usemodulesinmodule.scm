(define-module (DIRECTORY FILE)
  #:use-module  (mtfa error-handler)
  #:use-module  (mtfa utils)
  #:use-module  (mtfa serializer)
  #:use-module  (mtfa unordered-set) ;;unordered set con chiavi (stringhe, numeri o sumimboli: tutto convertito in stringa). Persistente
  #:use-module  (mtfa unordered-map) ;;unordered map con chiavi (stringhe) e valori (qualsiasi cosa). persistente
  #:use-module  (mtfa star-map)      ;;Inserisce stringhe con o senza jolly, la stringa che definisce il jolly e il valore. Cerca le stringhe che matchano!
  #:use-module  (mtfa simple_db)
  #:use-module  (mtfa eis)
  #:use-module  (mtfa va)
  #:use-module  (mtfa extset)  ;;gestisce insiemi i cui elementi sono stringhe! consente operazioni di clone, set, check, get all.... Definisce una macro che consente di creare "al volo" una sottoclasse le cui istanze condividono gli stessi elementi.
  #:use-module  (mtfa umset)   ;;è una unordered map (non persistente) che ha stringhe come chiavi e ha insiemi di stringhe come valori. Ogni insert aggiunge all'insieme corrispondente. Definisce inoltre la mtfa-umap-list che consente di mappare liste come chiavi e qualsiasi valore come valore
  #:use-module (mtfa web)      ;;creazione di token di sicurezza, cookie, ecc.
  #:use-module (mtfa brg)
  ;;#:use-module  (mtfa fsm)
  ;;

  ;;i moduli di guile
  #:use-module  (srfi srfi-1)
  #:use-module  (srfi srfi-9)
  #:use-module  (srfi srfi-11)
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
  #:use-module  (ice-9 control)

  #:use-module  (oop goops)
  ;; #:use-module  (sxml simple)
  ;; #:use-module  (sxml ssax)
  ;; #:use-module  (sxml xpath)
  #:use-module  (json)
  ;;
  ;;
  )
