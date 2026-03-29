(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa extset)
  ;;I miei moduli
  #:use-module (mtfa utils)
  #:use-module (mtfa error-handler)
  ;;
  ;;i moduli di guile
  ;;;i moduli di guile
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module ((srfi srfi-18) #:prefix srfi-18::) ;;thread e mutex ad alto livello
  ;; date & time rinomina per avere un current time che non si sovrappone
  #:use-module ((srfi srfi-19) #:prefix  datetime:)

  #:use-module (srfi srfi-43)
  #:use-module (srfi srfi-26)
  
  #:use-module (web uri)
  #:use-module (system foreign)
  #:use-module (rnrs bytevectors)
  #:use-module (srfi srfi-60)
  #:use-module (srfi srfi-111)
;;  #:use-module ((rnrs io ports) #:prefix ioports::)
  #:use-module (ice-9 ftw)
  #:use-module (ice-9 rdelim)
  #:use-module (ice-9 pretty-print)
  #:use-module (ice-9 regex)
  #:use-module (ice-9 iconv)
  #:use-module (ice-9 threads)
  #:use-module (ice-9 control)
  #:use-module (ice-9 receive)
  #:use-module (ice-9 binary-ports)
  #:use-module (ice-9 textual-ports)
  #:use-module (ice-9 q)

   ;;La libreria guile lib
  #:use-module (scheme kwargs)
  #:use-module (search basic)
  #:use-module (math primes)
  #:use-module (match-bind)
  #:use-module (graph topological-sort)

  #:use-module (sxml simple)
  #:use-module (json)
  #:use-module (oop goops)
  #:use-module (oop goops describe)
  #:use-module (ice-9 optargs)
  #:use-module (ice-9 match)
)

;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3.so" "init_mtfa_fs3")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3s.so" "init_mtfa_fs3s")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_db_mysql.so" "init_mtfa_db_mysql")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_ph.so" "init_mtfa_ph")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_lib.so" "init_mtfa_lib")
;;;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_netfilter_queue.so" "init_mtfa_netfilter_queue")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_su.so" "init_mtfa_su")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_cipher.so" "init_mtfa_ecipher")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_star.so" "init_mtfa_star")

;;operazioni generiche sui set (creazione, test se un certo valore è presente, ecc)
(define-class-public <mtfa-extset> ()
  ;;Le variabili di istanza e di classe
  ;;
  ;;la ds ph, shared tra tutte le istanze
  (ph  #:init-value (mtfa-ph-make)
       #:getter mtfa-es::ph
       #:setter mtfa-es::ph!
       #:allocation #:each-subclass
       )
  ;;
  ;;la ds set generale (per poter gestire l'insieme universale, ogni elemento che inserisco le metto qui!)
  (pset #:init-value (mtfa-su-make)
        #:getter mtfa-es::pset
        #:setter mtfa-es::pset!
        #:allocation #:each-subclass
        )
  ;;
  ;;il mutex di famiglia, per evitare accessi concorrenti tra oggetti alla stuttura ph
  (mtx-ph #:init-value (make-mutex)
          #:getter mtfa-es::ph-mtx
          #:setter mtfa-es::ph-mtx!
          )
  ;;
  ;;la ds set
  (set #:init-thunk mtfa-su-make
       #:getter mtfa-es::set
       #:setter mtfa-es::set!
       )
  ;;
  ;;il mutex di famiglia, per evitare accessi concorrenti all'iterno dello stesso oggetto alla stuttura set
  (mtx-set #:init-thunk make-mutex
           #:getter mtfa-es::set-mtx
           #:setter mtfa-es::set-mtx!
           )
  )

(define-method (initialize (ds <mtfa-extset>) initargs)
  (next-method)

  ;; (mtfa-assert (string? (mtfa-es::fsave ds)) "<mtfa-um>: must be created with #:fsave filename parameter")
  ;; (mtfa-um::log! ds (string-append (mtfa-um::fsave ds) ".um.log"))
  ;; (mtfa-um::serialized! ds (string-append (mtfa-um::fsave ds) ".um.ser"))
  ;; (mtfa-um::restore ds)
  )

(define-method-public (mtfa-extset::clone-empty (ds <mtfa-extset>))
  (let ((cl (make <mtfa-extset>)))
    (mtfa-es::pset! cl (mtfa-es::pset ds))
    (mtfa-es::ph! cl (mtfa-es::ph ds))
    cl))


(define-method-public (mtfa-extset::set (ds <mtfa-extset>) (skey <string>) (setunset <boolean>))
  "2 params: key può essere stringa, numero o simbolo. setunset: #t or #f"
  ;; (with-mutex (mtfa-es::ph-mtx ds)  ;;con il mutex della struttura ph condivisa tra tutte le istanze della classe
  ;;   (with-mutex (mtfa-es::set-mtx ds)  ;;con il mutex della struttura set dell'istanza
      (let (;; (skey (mtfa-to-string key))
            (rph (mtfa-es::ph ds))
            (rset (mtfa-es::set ds))
            (pset (mtfa-es::pset ds))
            )
        ;; (Show! rph ", " pset ", " rset)
        (let ((val (mtfa-ph-add rph skey)))
          (mtfa-su-set rset val setunset)
          (mtfa-su-set pset val #t)
          ds))) ;; ))

(define-method-public (mtfa-extset::set (ds <mtfa-extset>) (lskey <list>) (setunset <boolean>))
  "2 params: lskey è una lista di stringhe, numeri o simboli. setunset: #t or #f"
  ;; (with-mutex (mtfa-es::ph-mtx ds)  ;;con il mutex della struttura ph condivisa tra tutte le istanze della classe
  ;;   (with-mutex (mtfa-es::set-mtx ds)  ;;con il mutex della struttura set dell'istanza
      (let (;; (skey (mtfa-to-string key))
            (rph (mtfa-es::ph ds))
            (rset (mtfa-es::set ds))
            (pset (mtfa-es::pset ds))
            )
        (for-each (lambda (item)
                    (let ((val (mtfa-ph-add rph item)))
                      (mtfa-su-set rset val setunset)
                      (mtfa-su-set pset val #t)
                      ))
                  lskey
                  )
        ds)) ;; ))

(define-method-public (mtfa-extset::check (ds <mtfa-extset>) (skey <string>))
  ;; (with-mutex (mtfa-es::ph-mtx ds)  ;;con il mutex della struttura ph condivisa tra tutte le istanze della classe
  ;;   (with-mutex (mtfa-es::set-mtx ds)  ;;con il mutex della struttura set dell'istanza
      (let* (;; (skey (mtfa-to-string key))
            (rph (mtfa-es::ph ds))
            (rset (mtfa-es::set ds))
            (gset (mtfa-es::pset ds))
            (val (mtfa-ph-get rph skey))
            )
        ;; (Show! "local  set: "(mtfa-su-get-all rset))
        ;; (Show! "local  set: "(mtfa-su-to-string rset))
        ;; (Show! "global set: "(mtfa-su-get-all gset))
        ;; (Show! "global set: "(mtfa-su-to-string gset))
        ;; (Show! "global PH: " (mtfa-ph-get-all rph))
        ;; (Show! "Cerca con " skey " per " val " che è numero? " (number? val))
        (mtfa-su-get rset val))) ;; ))

(define-method-public (mtfa-extset::get-all (ds <mtfa-extset>))
  "nessun parametro"
  ;; (with-mutex (mtfa-es::ph-mtx ds)  ;;con il mutex della struttura ph condivisa tra tutte le istanze della classe
  ;;   (with-mutex (mtfa-es::set-mtx ds)  ;;con il mutex della struttura set dell'istanza
      (let ((ids (mtfa-su-get-all (mtfa-es::set ds))))
        (map (lambda (id)
               (mtfa-ph-get-val (mtfa-es::ph ds) id))
             (reverse ids))));; ))

(define-method-public (mtfa-extset::get-universal-all (ds <mtfa-extset>))
  "nessun parametro"
  ;; (with-mutex (mtfa-es::ph-mtx ds)  ;;con il mutex della struttura ph condivisa tra tutte le istanze della classe
  ;;   (with-mutex (mtfa-es::set-mtx ds)  ;;con il mutex della struttura set dell'istanza
      (let ((ids (mtfa-su-get-all (mtfa-es::pset ds))))
        (map (lambda (id)
               (mtfa-ph-get-val (mtfa-es::ph ds) id))
             (reverse ids))));; ))

(define-method-public (mtfa-extset::and (ds <mtfa-extset>) (ds1 <mtfa-extset>))
  "2 parametri, due strutture mtfa-extset"
  ;; (with-mutex (mtfa-es::ph-mtx ds)
  ;;   (with-mutex (mtfa-es::set-mtx ds)
  ;;     (with-mutex (mtfa-es::ph-mtx ds1)
  ;;       (with-mutex (mtfa-es::set-mtx ds1)
          (let ((rset (mtfa-es::set ds))
                (rset1 (mtfa-es::set ds1))
                (out (mtfa-extset::clone-empty ds)))
            ;; (Show (mtfa-su-get-all rset))
            ;; (Show (mtfa-su-get-all rset1))
            (mtfa-su-and (mtfa-es::set out) rset rset1)
            out));; ))))

(define-method-public (mtfa-extset::or (ds <mtfa-extset>) (ds1 <mtfa-extset>))
  "1 parametro, una struttura mtfa-extset"
  ;; (with-mutex (mtfa-es::ph-mtx ds)
  ;;   (with-mutex (mtfa-es::set-mtx ds)
  ;;     (with-mutex (mtfa-es::ph-mtx ds1)
  ;;       (with-mutex (mtfa-es::set-mtx ds1)
          (let ((rset (mtfa-es::set ds))
                (rset1 (mtfa-es::set ds1))
                (out (mtfa-extset::clone-empty ds)))
            (mtfa-su-or (mtfa-es::set out) rset rset1)
            out));; ))))

(define-method-public (mtfa-extset::xor (ds <mtfa-extset>) (ds1 <mtfa-extset>))
  "1 parametro, una struttura mtfa-extset"
  ;; (with-mutex (mtfa-es::ph-mtx ds)
  ;;   (with-mutex (mtfa-es::set-mtx ds)
  ;;     (with-mutex (mtfa-es::ph-mtx ds1)
  ;;       (with-mutex (mtfa-es::set-mtx ds1)
          (let ((rset (mtfa-es::set ds))
                (rset1 (mtfa-es::set ds1))
                (out (mtfa-extset::clone-empty ds)))
            (mtfa-su-xor (mtfa-es::set out) rset rset1)
            out));; ))))

(define-method-public (mtfa-extset::not (ds <mtfa-extset>))
  "1 parametro, una struttura mtfa-extset"
  ;; (with-mutex (mtfa-es::ph-mtx ds)
  ;;   (with-mutex (mtfa-es::set-mtx ds)
      (let ((rset (mtfa-es::set ds))
            (out (mtfa-extset::clone-empty ds)))
        (mtfa-es::set! out (mtfa-su-clone rset))
        (mtfa-su-flip-all (mtfa-es::set out))
        (mtfa-su-and-equal (mtfa-es::set out) (mtfa-es::pset out))
        out));; ))

(define-method-public (mtfa-extset::get-set (ds <mtfa-extset>))
  (mtfa-es::set ds))

(define-method-public (mtfa-extset? (ds <mtfa-extset>))
  #t
)

(define-method-public (mtfa-extset::serialize-locals (ds <mtfa-extset>))
  "1 parametro, una struttura mtfa-extset"
  ;; (with-mutex (mtfa-es::ph-mtx ds)
  ;;   (with-mutex (mtfa-es::set-mtx ds)
      (mtfa-serializer (mtfa-es::set ds)));; ))

(define-method-public (mtfa-extset::serialize-globals (ds <mtfa-extset>))
  "1 parametro, una struttura mtfa-extset"
  ;; (with-mutex (mtfa-es::ph-mtx ds)
  ;;   (with-mutex (mtfa-es::set-mtx ds)
      (cons (mtfa-serializer (mtfa-es::ph ds))
            (mtfa-serializer (mtfa-es::pset ds))));; ))

;; (define-syntax-public mtfa-extset-make-instance
;;   (lambda (x)
;;     (syntax-case x ()
;;       ((_ name)
;;        #'(let ((cname ""))
;;            ;;(Show! "CNAME: " 'cname ". IsString?: " (string? 'cname))
;;            (if (string? 'name) (set! cname name) (set! cname (symbol->string 'name)))
;;            (when-false (member cname (map (lambda(i) (symbol->string (class-name i)))(class-subclasses <mtfa-extset>)))
;;              ;;(Show "Crea nuova classe: <" (string-append "(define-class-public " cname "( <mtfa-extset> ))") ">")
;;              (let ((newclass (eval-string (string-append "(define-class-public " cname "( <mtfa-extset> ))"))))
;;                ;; (Show "modifica gli elementi di base <" (string-append "(class-slot-set! " cname " 'pset (mtfa-su-make))"
;;                ;;                 "(class-slot-set! " cname " 'ph (mtfa-ph-make))") ">")
;;                (eval-string
;;                 (string-append "(class-slot-set! " cname " 'pset (mtfa-su-make))"
;;                                "(class-slot-set! " cname " 'ph (mtfa-ph-make))"))))
;;            (eval-string (string-append "(make " cname " )")))))))

(define-public (mtfa-extset-make-instance cname)
  (when-false (member cname (map (lambda(i) (symbol->string (class-name i)))(class-subclasses <mtfa-extset>)))
              (let ((newclass (eval-string (string-append "(define-class-public " cname "( <mtfa-extset> ))"))))
                (eval-string
                 (string-append "(class-slot-set! " cname " 'pset (mtfa-su-make))"
                                "(class-slot-set! " cname " 'ph (mtfa-ph-make))"))))
  (eval-string (string-append "(make " cname " )")))
