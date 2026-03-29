(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa unordered-map)
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
;;;;;;;;;;;;;;;;;;;;;;;;;;UNORDERED MAP;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (ToString s)
  (with-output-to-string (lambda () (write s))))
(define (FromString s)
  (with-input-from-string s (lambda () (read))))

(define-class-public <mtfa-unordered-map> ()
  ;;Le variabili di istanza e di classe
  ;;
  ;;la ds fs3
  (fs3 #:init-thunk mtfa-fs3-make
       #:getter mtfa-um::fs3
       #:setter mtfa-um::fs3!
       )
  ;;
  ;;il mutex di famiglia
  (mtx #:init-thunk make-mutex
       #:getter mtfa-um::mtx
       #:setter mtfa-um::mtx!
       )
  ;;
  ;;Il prefisso del file in cui salvare i dati
  (fsave #:init-form #nil
         #:init-keyword #:fsave
         #:getter mtfa-um::fsave
         #:setter mtfa-um::fsave!)
  ;;
  ;;Dove appendere operazione per operazione
  (log #:init-form #nil
       ;;#:init-keyword #:log
       #:getter mtfa-um::log
       #:setter mtfa-um::log!)
  )

;;Appende come stringhe al file di log
(define (DbAppendUm db cmd . others)
  ;;(Show! "Entra in DbAppendUm: " cmd " => " others)
  (if (nil? db)
      #t ;;come se avessi inserito o eliminato dal DB
      (mtfa-noerr
       #f
       (cond
        ((string=? cmd "ADD")
         (db-interface::Sqlite3::Do db (string-append
                                        "INSERT OR REPLACE INTO log (key, value) values ('"
                                        (car others)
                                        "', '"
                                        (mtfa-b64-encode (cadr others))
                                        "')"
                                        )))
        ((string=? cmd "DEL")
         (db-interface::Sqlite3::Do db (string-append
                                        "DELETE FROM log WHERE key='"
                                        (car others)
                                        "'"
                                        )))
        (#t (error "Accepted only ADD and DEL"))
        )
       #t)))

(define-method (initialize (ds <mtfa-unordered-map>) initargs)
  (next-method)
  (mtfa-assert (or (nil? (mtfa-um::fsave ds)) (string? (mtfa-um::fsave ds)))
               "<mtfa-um>: must be created with #:fsave filename parameter, string or #nil")
  (if (nil? (mtfa-um::fsave ds))
      (begin
       (mtfa-um::log! ds #nil))
      (begin
        (mtfa-um::log! ds (string-append (mtfa-um::fsave ds) ".um.log"))
        ;;Creo il DB, se non esiste già
        (mtfa-um::log! ds (db-interface::Sqlite3::Open (mtfa-um::log ds)))
        (db-interface::Sqlite3::Do (mtfa-um::log ds) "CREATE TABLE log (key TEXT PRIMARY KEY, value TEXT)")

	(mtfa-um::restore ds))))

;;allora, i dati sono organizzati nel seguente modo: quando inserisco ho, associato alla chiave, il cons (#t/f . v)
;;quando cancello ci metto #f

(define-method-public (mtfa-um::add (ds <mtfa-unordered-map>) key value)
  (with-mutex (mtfa-um::mtx ds)
    (let ((skey (ToString key))
	  (rds (mtfa-um::fs3 ds)))
      (if (DbAppendUm (mtfa-um::log ds) "ADD" skey (mtfa-serializer value))
	  (begin
	    (mtfa-fs3-update rds skey (cons #t value))
	    #t)
	  (begin
            #f)))))

(define-method-public (mtfa-um::get (ds <mtfa-unordered-map>) key)
  (with-mutex (mtfa-um::mtx ds)
    (let* ((skey (ToString key))
	   (rds (mtfa-um::fs3 ds))
           (value (mtfa-fs3-get rds skey)))
      (if (nil? value)
          #f
          (if (car value)
              (cdr value)
              #f)))))

(define-method-public (mtfa-um::del (ds <mtfa-unordered-map>) key)
  (with-mutex (mtfa-um::mtx ds)
    (let ((skey (ToString key))
	  (rds (mtfa-um::fs3 ds)))
      (if (DbAppendUm (mtfa-um::log ds) "DEL" skey)
	  (begin
	    (mtfa-fs3-update rds skey '(#f . #f))
	    #t)
	  #f))))

(define-method-public (mtfa-um::for-each (ds <mtfa-unordered-map>) f_1)
  ;; (with-mutex (mtfa-um::mtx ds)
    (let ((rds (mtfa-um::fs3 ds)))
      (mtfa-fs3-for-each
       rds
       (lambda (k v)
	 (when (car v)
	   (f_1 (FromString k) (cdr v)))))
      #t))

(define-method-public (mtfa-um::map (ds <mtfa-unordered-map>) f_1)
  ;; (with-mutex (mtfa-um::mtx ds)
    (let ((lret #nil)(rds (mtfa-um::fs3 ds)))
      (mtfa-fs3-for-each
       rds
       (lambda (k v)
	 (when (car v)
	   (set! lret (cons (f_1 (FromString k) (cdr v)) lret)))))
      lret))

(define-method-public (mtfa-um::fold (ds <mtfa-unordered-map>) f_2 prec)
  ;; (with-mutex (mtfa-um::mtx ds)
    (let ((lastv prec)(rds (mtfa-um::fs3 ds)))
      (mtfa-fs3-for-each
       rds
       (lambda (k v)
	 (when (car v)
       	   (set! lastv (f_2 (cons (FromString k) (cdr v)) lastv)))))
      lastv))

;;
;;Quando fai la restore, poi devi cancellare il log e ripristinarlo solo con i dati presenti e non con i vecchi dati
;;ma non serve poiché la delete elimina anche dal log
(define-method-public (mtfa-um::restore (ds <mtfa-unordered-map>))
  (if (nil? (mtfa-um::log ds))
      #t
      (mtfa-noerr
       #f
       (let ((rds  (mtfa-um::fs3 ds))
	     (db (mtfa-um::log ds)))
         (with-mutex (mtfa-um::mtx ds)
           (let ((items (db-interface::Sqlite3::Do db "SELECT key, value FROM log ORDER BY ROWID ASC"))
                 (key #f)
                 (data #f))
             (for-each (lambda (it)
		         (set! key (car it))
		         (set! data (mtfa-b64-decode-bv (cadr it)))
			 (mtfa-fs3-update rds key (cons #t (mtfa-deserializer data))))
                       items)))
         #t))))

