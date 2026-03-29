(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa umset)
  ;;I miei moduli
  #:use-module (mtfa extset)
  #:use-module (mtfa unordered-map) ;;unordered map con chiavi (stringhe) e valori (qualsiasi cosa). persistente
  #:use-module (mtfa utils)
  #:use-module (mtfa error-handler)
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

;;Qui definiamo la classe che consente di gestire una unordered map di insiemi di stringhe
;;Consente di associare a una specifica stringa un insieme di stringhe, senza duplicazioni
;;esempio (uno => (alfa, bera, gamma))
;;        (due => (alfa, delta, epsilon, iota)
;;        ...
(define-class-public <mtfa-umset> ()
  ;;Le variabili di istanza e di classe
  ;;La umset ha una mtfa-um con chiavi che indirizzano mtfa-extset, ovviamente tutti dello stesso tipo base ma con localset differenti
  (um  #:init-form (make <mtfa-unordered-map>)
       #:getter mtfa-umset::um
       #:setter mtfa-umset::um!
       )
  
  ;;Il tipo di extset che devo creare (stringa)
  (extset-stype #:init-keyword #:extset-stype
                #:getter mtfa-umset::extset-stype
                #:setter mtfa-umset::extset-stype!)
  )

(define-method (initialize (ds <mtfa-umset>) initargs)
  (next-method)

  (mtfa-assert (and (string? (mtfa-umset::extset-stype ds)) (< 0 (string-length (mtfa-umset::extset-stype ds))))
               "<mtfa-umset>: must be created with #:extset-stype symbol parameter: string"))

(define-method-public (mtfa-umset? (ds <mtfa-umset>)) #t)

(define-method-public (mtfa-umset::class-name (ds <mtfa-umset>))
  (mtfa-umset::extset-stype ds))

(define-method-public (mtfa-umset::add (ds <mtfa-umset>) (key <string>) (value <string>))
  (mtfa-noerr
   #nil
   (let* ((um (mtfa-umset::um ds))
          (extset (mtfa-um::get um key)))
     (ifnot extset
            (begin
              (set! extset (mtfa-extset-make-instance (mtfa-umset::extset-stype ds)))
              (mtfa-um::add um key extset)
              (mtfa-extset::set extset value #t)
              )
            (mtfa-extset::set extset value #t)))))

(define-method-public (mtfa-umset::del (ds <mtfa-umset>) (key <string>) (value <string>))
  (mtfa-noerr
   #nil
   (let* (
          (um (mtfa-umset::um ds))
          (extset (mtfa-um::get um key)))
     (if (not extset)
         #t
         (begin
           (mtfa-extset::set extset value #f)
           #t)))))

(define-method-public (mtfa-umset::check (ds <mtfa-umset>) (key <string>) (value <string>))
  (let* ((um (mtfa-umset::um ds))
         (extset (mtfa-um::get um key)))
    (if (not extset)
        #f
        (mtfa-extset::check extset value))))

(define-method-public (mtfa-umset::get-values-from-key (ds <mtfa-umset>) (key <string>))
  (let* ((um (mtfa-umset::um ds))
         (extset (mtfa-um::get um key)))
    (if (not extset)
        #nil
        (mtfa-extset::get-all extset))))

(define-method-public (mtfa-umset::get-all-key-values (ds <mtfa-umset>))
  (let* ((um (mtfa-umset::um ds))
         (res #nil)
         )
    (mtfa-um::for-each um (lambda (k v)
                            (set! res (cons (list k (mtfa-extset::get-all v)) res))))
    (reverse res)))

(define-method-public (mtfa-umset::get-all-keys (ds <mtfa-umset>))
  (let ((um (mtfa-umset::um ds)))
    (mtfa-um::map um (lambda (k v) k))))

(define-method-public (mtfa-umset::get-um (ds <mtfa-umset>))
  (mtfa-umset::um ds))

(define-method-public (mtfa-umset::get-extset (ds <mtfa-umset>) (key <string>))
  (mtfa-noerr
   #nil
   (let* ((um (mtfa-umset::um ds)))
     (mtfa-um::get um key))))


(define-public (mtfa-umset-make-instance name)
  (make <mtfa-umset> #:extset-stype name))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;Aggiungo qui una classe che accetta come input liste di stringhe e come output indica esistenza o 
;;meno di queste liste, anche parziali ma nell'ordine dovuto.
;;potremmo fare che in base alla make si definisce se il match è solo parziale oppure è totale

(define-class-public <mtfa-umap-list> ()
  (um  #:init-form (make <mtfa-unordered-map>)
       #:getter mtfa-umap-list::um
       #:setter mtfa-umap-list::um!
       )
  )

(define-method (initialize (ds <mtfa-umap-list>))
  (next-method)
)

(define-method-public (mtfa-umap-list::add (ds <mtfa-umap-list>) (key <list>) value)
  ;;per prima cosa converto in stringa la lista, inserendo dei separatori: #\0
  (let ((s (fold (lambda (a p)
                   (string-append p "\x00" (mtfa-to-string a)))
                 (mtfa-to-string (car key))
                 (cdr key))))
    (mtfa-um::add (mtfa-umap-list::um ds) s (cons #t value))))

(define-method-public (mtfa-umap-list::del (ds <mtfa-umap-list>) (key <list>))
  ;;per prima cosa converto in stringa la lista, inserendo dei separatori: #\0
  (let* ((s (fold (lambda (a p)
                   (string-append p "\x00" (mtfa-to-string a)))
                 (mtfa-to-string (car key))
                 (cdr key)))
         (ds (mtfa-umap-list::um ds))
         )
    (mtfa-um::del ds s)))


(define-method-public (mtfa-umap-list::get (ds <mtfa-umap-list>) (key <list>))
  ;;per prima cosa converto in stringa la lista, inserendo dei separatori: #\0
  (let* ((s (fold (lambda (a p)
                   (string-append p "\x00" (mtfa-to-string a)))
                 (mtfa-to-string (car key))
                 (cdr key)))
         (ds (mtfa-umap-list::um ds)))
    (mtfa-um::get ds s)))

;; (define-method-public (mtfa-umap-list::get-all (ds <mtfa-umap-list>))
;;   ;;(Show! (mtfa-fs3-get-all (mtfa-umap-list::um ds)))
;;   (map (lambda (it)
;;          ;;il cdr è lo stesso, il car è una stringa che converto in lista
;;          (cons (string-split (car it) #\00) (cdr it))
;;          )
;;        (fold (lambda (it p)
;;                (cons it p)
;;                p
;;                )
;;              #nil
;;              (mtfa-um::map (mtfa-umap-list::um ds) (lambda (i j) (cons i j))))))

