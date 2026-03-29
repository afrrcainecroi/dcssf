(set! %load-extensions (cons ".sls" %load-extensions))
(add-to-load-path "/home/arcieri/.guix-profile/share/guile/site/3.0/")
(add-to-load-path "/new_devs/usr/local/mtfa-guile-libs/share/guile/site/3.0/")
(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa serializer)
  ;;I miei moduli
  #:use-module (mtfa error-handler)
  #:use-module (mtfa utils)
  #:use-module (mtfa extset)
  ;;
  #:use-module (pfds sets)
  ;;
  ;;i moduli di guile
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

  #:export ())

(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3.so" "init_mtfa_fs3")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3s.so" "init_mtfa_fs3s")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_db_mysql.so" "init_mtfa_db_mysql")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_ph.so" "init_mtfa_ph")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_lib.so" "init_mtfa_lib")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_netfilter_queue.so" "init_mtfa_netfilter_queue")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_su.so" "init_mtfa_su")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_cipher.so" "init_mtfa_ecipher")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_star.so" "init_mtfa_star")


;;questo poiché la set? definita come record qui non funziona
(define (set? x) (equal? '<set> (mtfa-typeof x)))


;;;;;;;;;;;;;;;;;;;;;;SERIALIZE E DESERIALIZE;;;;;;;;;;;;;;;;;;;;;;;;;
(define (serialize-hash-table obj)
  (serialize-list (hash-map->list list obj))
  )

(define (unserialize-hash-table sxml)
  (let ((ht (make-hash-table)))
    (for-each (lambda (x)
                (hash-set! ht (car x) (cadr x)))
              (unserialize-list sxml))
    ht))

(define (serialize-set obj)
  (map obj->sxml (set->list obj)))

(define* (mtfa-unserialize-set sxml #:optional (ordering <))
  (list->set (map sxml->obj sxml) ordering))
(export mtfa-unserialize-set)

(define (serialize-list obj)
  (map obj->sxml obj))

(define (unserialize-list sxml)
  (map sxml->obj sxml))

(define (serialize-pair obj)
  (list (obj->sxml (car obj)) (obj->sxml (cdr obj))))

(define (unserialize-pair sxml)
  (cons (sxml->obj (car sxml)) (sxml->obj (cadr sxml))))

;;non è previsto che un record contenga un altro record??
(define (serialize-record-type obj) ;; => (<employee> ((name "franco") (age 50) (salary 100)))
  (let* ((des (record-type-descriptor obj))
         (name (record-type-name des))
         (res (list (mtfa-to-string name)
           (map (lambda (x) 
                  (mtfa-serializer ((record-accessor des x) obj))
                  ;; ((record-accessor des x) obj)
                  )
                (record-type-fields des))))
         )
    ;; (Show! res)
    (obj->sxml res)))

(define (deserialize-record-type sxml)
  (let* ((deser1 (car (unserialize-list sxml))) ;; (d (Show! "sxml: " sxml "\n" "deser1: " deser1))
         (name   (first deser1))  ;;(d (Show! "name: " name))
         (fields (second deser1))  ;;(d (Show! "fields: " fields))
         (type (eval-string name))
         (vfields (map (lambda (i) 
			 ;;(Show! "i: " i)
                         (mtfa-deserializer i))
		       fields)))
    ;;(Show! "Vfields: " vfields ", type: " type)
    (apply (record-constructor type) vfields)))

;;(apply (record-constructor <employee>) '("ybi" 19 29))


;; (define (serialize-record-type obj)
;;   (let* ((des (record-type-descriptor (cdr obj)))
;;          (name (record-type-name des)))
;;     (list name (map (lambda (x) 
;;                       (list x ((record-accessor des x) (cdr obj))))
;;                     (record-type-fields des)))))

;; (define (serialize-class obj) #t)
;; (define (unserialize-class sxml) #t)

;; (define (serialize-procedure obj) #t)
;; (define (unserialize-procedure sxml) #t)

(define (serialize-vector obj) 
  (let ((ll (vector->list obj)))
    (serialize-list ll)))
(define (unserialize-vector sxml)
  (let ((ll (unserialize-list sxml)))
    (list->vector ll)))

(define (serialize-bytevector obj)
  (let ((ll (bytevector->u8-list obj)))
    (serialize-list ll)))
(define (unserialize-bytevector sxml)
  (let ((ll (unserialize-list sxml)))
    (list->u8vector ll)))

(define (serialize-charset obj) 
  (serialize-list (char-set->list obj)))
(define (unserialize-charset sxml)
  (list->char-set (unserialize-list sxml)))

(define (mtfa-ph-serializer obj)
  (serialize-list (mtfa-ph-get-all obj)))
(define (mtfa-ph-unserializer sxml)
  (let ((ds (mtfa-ph-make)))
    (for-each (lambda (c) (mtfa-ph-add ds (car c))) (unserialize-list sxml))
    ds))

(define (mtfa-su-serializer obj)
  (serialize-bytevector (mtfa-su-serialize obj)))
(define (mtfa-su-unserializer sxml)
  (let ((ds (mtfa-su-make)))
    (mtfa-su-deserialize ds (unserialize-bytevector sxml))
    ds))

(define (mtfa-fs3-serializer obj)
  (serialize-list (mtfa-fs3-get-all obj)))
(define (mtfa-fs3-unserializer sxml)
  (let ((ds (mtfa-fs3-make)))
    (for-each (lambda (c) (mtfa-fs3-add ds (car c) (cdr c))) (unserialize-list sxml))
    ds))

(define (mtfa-extset-serializer pbj)
  (serialize-list
   (list
    (symbol->string (class-name (class-of pbj)))
    (mtfa-extset::get-universal-all pbj)
    (mtfa-extset::get-all pbj))))

(define (mtfa-extset-unserializer sxml)
  (let* ((items (unserialize-list sxml))
         (cname (first items))
         (obj (mtfa-extset-make-instance cname)))
    (for-each (lambda (value)
                (mtfa-extset::set obj value #f))
              (second items))
    (for-each (lambda (value)
                (mtfa-extset::set obj value #t))
              (third items))
    obj))

(define (mtfa-umset-serializer pbj)
  (Show! "mtfa-umset-serializer to be defined")
  ;; (serialize-list
  ;;  ;;prendo tutte le chiavi e per ogni chiave serializzo la coppia
  ;;  (cons (mtfa-umset::class-name pbj) (mtfa-fs3-get-all (mtfa-umset::get-um pbj))))
  )

(define (mtfa-umset-unserializer sxml)
  (Show! "mtfa-umset-unserializer to be defined")
  ;; (let* ((items (unserialize-list sxml))
  ;;        (cname (car items))
  ;;        (res (make <mtfa-umset> cname))
  ;;        )
  ;;   (for-each (lambda (it)
  ;;               (mtfa-umset::add res (car it) (cdr it)))
  ;;             (cdr items)))
  )


(define (serialize-bitvector obj) 
  (serialize-list (map (lambda (v) (if v 1 0)) (bitvector->list obj))))
(define (unserialize-bitvector sxml)
  (list->bitvector (map (lambda (v) (if (zero? v) #f #t)) (unserialize-list sxml))))

(define (serialize-array obj)
  (serialize-list (list (car (array-dimensions obj)) (array->list obj))))

(define (unserialize-array sxml)
  (let* ((lista (unserialize-list sxml))
	 (ndim (car lista))
         (ll (cadr lista)))
    (list->array ndim ll)))

(define (number-serializer n)
  (number->string n))
(define (number-deserializer sn)
  (string->number (car sn)))

(define (char-serializer n)
  (number->string (char->integer n)))
(define (char-deserializer sn)
  (integer->char (string->number (car sn))))

(define (boolean-serializer n)
  (if n "1" "0"))
(define (boolean-deserializer sn)
  (if (string=? "0" (car sn)) #f #t))

(define (symbol-serializer n)
  (symbol->string  n))
(define (symbol-deserializer sn)
  (string->symbol (car sn)))

(define (keyword-serializer n)
  (symbol->string (keyword->symbol n)))
(define (keyword-deserializer sn)
  (symbol->keyword (string->symbol (car sn))))

(define generic-serializer identity)
(define (generic-deserializer id)
  (if (nil? id) #nil
      (car id)))

(define *type-list*
  `(
    (,number? a ,number-serializer ,number-deserializer)
    (,char? b ,char-serializer ,char-deserializer)
    (,string? c ,generic-serializer ,generic-deserializer)
    (,symbol? d ,symbol-serializer ,symbol-deserializer)
    (,list? e ,serialize-list ,unserialize-list)
    (,pair? f ,serialize-pair ,unserialize-pair)
    ;;(,class? class ,serialize-class ,unserialize-class)
    ;;(,procedure? procedure ,serialize-procedure ,unserialize-procedure)
    (,bytevector? g ,serialize-bytevector ,unserialize-bytevector)
    (,vector? h ,serialize-vector ,unserialize-vector)
    (,keyword? i ,keyword-serializer ,keyword-deserializer)
    (,char-set? j ,serialize-charset ,unserialize-charset)
    (,bitvector? k ,serialize-bitvector ,unserialize-bitvector)
    (,array? l ,serialize-array ,unserialize-array)
    (,boolean? m ,boolean-serializer ,boolean-deserializer)
    (,hash-table? n ,serialize-hash-table ,unserialize-hash-table)
    ;;
    ;;i set (Pure Functional Data Structures), prima dei record poiché contengono record
    (,set? u ,serialize-set ,mtfa-unserialize-set)
    ;;
    ;;i record
    (,record? t ,serialize-record-type ,deserialize-record-type)
    ;;
    ;;ora i tipi strutturati dal C++
    ;;
    (,mtfa-ph? o ,mtfa-ph-serializer ,mtfa-ph-unserializer)
    (,mtfa-fs3? p ,mtfa-fs3-serializer ,mtfa-fs3-unserializer)
    (,mtfa-su? q ,mtfa-su-serializer ,mtfa-su-unserializer)
    (,mtfa-extset? r ,mtfa-extset-serializer ,mtfa-extset-unserializer)
    ;;(,mtfa-umset? s ,mtfa-umset-serializer ,mtfa-umset-unserializer)
    )
  )

(define (get-type-ctx obj)
  (any (lambda (x) (and ((car x) obj) (cdr x))) *type-list*))

(define* (obj->sxml obj #:optional (serializer #f))
  (let* ((ctx (get-type-ctx obj))
	 ;; (dummy (Show! "!!!: " (mtfa-to-string ctx)))
         (type (car ctx))
         (ser (or serializer (cadr ctx))))
    (if ser
        `(,type ,(ser obj))
        (error "This type need a serializer!" type))))

(define (get-deserializer type)
  (any (lambda (x) (and (eqv? type (cadr x)) (cadddr x))) *type-list*))

(define* (sxml->obj sxml #:optional (deserializer #f) (ordering #f))
;;  (Show! "Deserializer: " deserializer ". Ordering: " ordering)
  (let* ((type (car sxml))
         (cont (cdr sxml))
         (deser (or deserializer (get-deserializer type))))
    (if deser
	(if ordering
            (deser cont ordering)
	    (deser cont))
        (error "This type can't be deserialiable, please give your own deserializer!" type))))

(define* (obj->xml obj #:optional (serializer #f)) 
  (mtfa-compress (call-with-output-string (lambda (s) (sxml->xml (obj->sxml obj serializer) s) s)) 6))

;;(define* (xml->obj obj #:optional (deserializer #f) (ordering #f))
(define* (xml->obj obj #:key (deserializer #f) (ordering #f))
  ;;(Show! "In deserializer...")
  (sxml->obj (cadr (xml->sxml (mtfa-uncompress obj))) deserializer ordering))

(define-public mtfa-serializer obj->xml)
(define-public mtfa-deserializer xml->obj)
(define-public (mtfa-serializer-b64 what) (mtfa-b64-encode (mtfa-serializer what)))
(define-public (mtfa-deserializer-b64 what) (mtfa-deserializer (mtfa-b64-decode-bv what)))
