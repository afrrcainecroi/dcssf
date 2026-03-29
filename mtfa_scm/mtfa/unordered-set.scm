(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa unordered-set)
  ;;I miei moduli
  #:use-module (mtfa utils)
  #:use-module (mtfa serializer)
  #:use-module (mtfa error-handler)
  #:use-module (mtfa simple_db)
  ;;;i moduli di guile
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
;;  #:use-module (srfi srfi-18)
  ;; date & time rinomina per avere un current time che non si sovrappone
  #:use-module ((srfi srfi-19) #:prefix  datetime:)

  #:use-module (srfi srfi-43)
  
  #:use-module (web uri)
  #:use-module (system foreign)
  #:use-module (rnrs bytevectors)
  #:use-module (srfi srfi-60)
  #:use-module (rnrs io ports)
  #:use-module (ice-9 ftw)
  #:use-module (ice-9 rdelim)
  #:use-module (ice-9 pretty-print)
  #:use-module (ice-9 regex)
  #:use-module (ice-9 iconv)
  #:use-module (ice-9 threads)
  #:use-module (sxml simple)
  #:use-module (json)
  #:use-module (oop goops)
  #:use-module (oop goops describe)

  #:export (
	    ))

;; (load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3.so" "init_mtfa_fs3")
;; (load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3s.so" "init_mtfa_fs3s")
;; (load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_db_mysql.so" "init_mtfa_db_mysql")
;; (load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_ph.so" "init_mtfa_ph")
;; (load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_lib.so" "init_mtfa_lib")
;; (load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_netfilter_queue.so" "init_mtfa_netfilter_queue")
;; (load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_su.so" "init_mtfa_su")
;; (load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_cipher.so" "init_mtfa_ecipher")
;; (load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_star.so" "init_mtfa_star")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;UNORDERED SET;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define-class-public <mtfa-unordered-set> ()
  ;;Le variabili di istanza e di classe
  ;;
  ;;la ds fs3
  (fs3 #:init-thunk mtfa-fs3-make
       #:getter mtfa-us::fs3
       #:setter mtfa-us::fs3!
       )
  ;;
  ;;il mutex di famiglia
  (mtx #:init-thunk make-mutex
       #:getter mtfa-us::mtx
       #:setter mtfa-us::mtx!
       )
  ;;
  ;;Il prefisso del file in cui salvare i dati
  (fsave #:init-form #nil
         #:init-keyword #:fsave
         #:getter mtfa-us::fsave
         #:setter mtfa-us::fsave!)
  ;;
  ;;Dove appendere operazione per operazione
  (log #:init-form #nil
       ;;#:init-keyword #:log
       #:getter mtfa-us::log
       #:setter mtfa-us::log!)
  ;; ;;
  ;; ;;La serializzazione. Se presente, prima serializzazione e dopo il log
  ;; (serialized #:init-form #nil
  ;;      ;;#:init-keyword #:serialized
  ;;      #:getter mtfa-us::serialized
  ;;      #:setter mtfa-us::serialized!)
  )

;;Appende come stringhe al file di log
(define (DbAppend db str . others)
  (if (nil? db)
      #t  ;;come se avessi appeso
      (mtfa-noerr
       #f
       (cond
        ((string=? str "ADD")
         (db-interface::Sqlite3::Do db (string-append
                                        "INSERT OR REPLACE INTO log (key) values ('"
                                        (mtfa-b64-encode (car others))
                                        "')"
                                        )))
        ((string=? str "DEL")
         (db-interface::Sqlite3::Do db (string-append
                                        "DELETE FROM log WHERE key='"
                                        (mtfa-b64-encode (car others))
                                        "'"
                                        )))
        (#t (error "Accepted only ADD and DEL"))
        )
       #t)))

(define-method (initialize (ds <mtfa-unordered-set>) initargs)
  (next-method)
  (mtfa-assert (or (nil? (mtfa-us::fsave ds)) (string? (mtfa-us::fsave ds)))
               "<mtfa-us>: must be created with #:fsave filename parameter, string or #nil")
  (if (nil? (mtfa-us::fsave ds))
      (begin
        (mtfa-us::log! ds #nil)
        )
      (begin
        (mtfa-us::log! ds (string-append (mtfa-us::fsave ds) ".mtfa.us"))
        ;;Creo il DB, se non esiste già
        (mtfa-us::log! ds (db-interface::Sqlite3::Open (mtfa-us::log ds)))
        (db-interface::Sqlite3::Do (mtfa-us::log ds) "CREATE TABLE log (key TEXT PRIMARY KEY)")
        )
      )
  ;;ora, faccio la reload dei dati e recupero eventuale file di log
  ;;(Show! "mtfa-us:: Log " (mtfa-us::log ds))
  (mtfa-us::restore ds)
  )

(define-method-public (mtfa-us::add (ds <mtfa-unordered-set>) key)
  (with-mutex (mtfa-us::mtx ds)
    (let ((skey (mtfa-to-string key))
	  (rds (mtfa-us::fs3 ds))
	  )
      (if (DbAppend (mtfa-us::log ds) "ADD" skey)
	  (begin
	    (mtfa-fs3-update rds skey #t)
	    #t)
	  #f))))

(define-method-public (mtfa-us::del (ds <mtfa-unordered-set>) key)
  (with-mutex (mtfa-us::mtx ds)
    (let ((skey (mtfa-to-string key))(rds (mtfa-us::fs3 ds)))
      (if (DbAppend (mtfa-us::log ds) "DEL" skey)
	  (begin
	    (mtfa-fs3-update rds skey #f)
	    #t)
	  #f))))

(define-method-public (mtfa-us::for-each (ds <mtfa-unordered-set>) f_1)
  "parametro: una funzione con un solo parametro, applicata a tutti i valori. Torna #t"
  ;;
  ;; (with-mutex (mtfa-us::mtx ds)
    (let ((rds (mtfa-us::fs3 ds)))
      (mtfa-fs3-for-each
       rds
       (lambda (k v)
	 (when v
	   (f_1 k))))
      #t));; )

(define-method-public (mtfa-us::map (ds <mtfa-unordered-set>) f_1)
  "parametro: una funzione con un solo parametro, applicata a tutti i valori. Torna una lista
contenente tutti gli elementi presenti, modificati dalla funzione"
  ;;
  ;; (with-mutex (mtfa-us::mtx ds)
    (let ((lret #nil)(rds (mtfa-us::fs3 ds)))
      (mtfa-fs3-for-each
       rds
       (lambda (k v)
	 (when v
	   (set! lret (cons (f_1 k) lret)))))
      lret));; )

(define-method-public (mtfa-us::fold (ds <mtfa-unordered-set>) f_2 prec)
  "parametri: una funzione con due parametri, il valore corrente e il valore della chiamata precedente, come la fold,
il primo valore passato come precedente"
  ;;
  ;; (with-mutex (mtfa-us::mtx ds)
    (let ((lastv prec)(rds (mtfa-us::fs3 ds)))
      (mtfa-fs3-for-each
       rds
       (lambda (k v)
	 (when v
       	   (set! lastv (f_2 k lastv)))
	 ))
      lastv));; )

(define-method-public (mtfa-us::restore (ds <mtfa-unordered-set>))
  (if (nil? (mtfa-us::log ds))
      #t
      (mtfa-noerr
       #f
       (let ((rds  (mtfa-us::fs3 ds))
	     (db (mtfa-us::log ds)))
         (with-mutex (mtfa-us::mtx ds)
           (let ((data (db-interface::Sqlite3::Do db "SELECT key FROM log ORDER BY ROWID ASC")))
             (for-each (lambda (it) (mtfa-fs3-update rds (mtfa-b64-decode-s (car it)) #t)) data))))
       #t)))

