(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa star-map)
  ;;I miei moduli
  #:use-module (mtfa utils)
  #:use-module (mtfa error-handler)
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

(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3.so" "init_mtfa_fs3")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3s.so" "init_mtfa_fs3s")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_db_mysql.so" "init_mtfa_db_mysql")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_ph.so" "init_mtfa_ph")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_lib.so" "init_mtfa_lib")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_netfilter_queue.so" "init_mtfa_netfilter_queue")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_su.so" "init_mtfa_su")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_cipher.so" "init_mtfa_ecipher")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_star.so" "init_mtfa_star")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;  STAR  MAP  ;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define-class-public <mtfa-star-map> ()
  ;;Le variabili di istanza e di classe
  ;;
  ;;la ds fs3
  (star #:init-thunk mtfa-star-make
       #:getter mtfa-stm::star
       #:setter mtfa-stm::star!
       )
  ;;
  ;;il mutex di famiglia
  (mtx #:init-thunk make-mutex
       #:getter mtfa-stm::mtx
       #:setter mtfa-stm::mtx!
       )
  ;;
  ;;Il vettore in cui inserisco i valori lisp passati nella insert
  (data #:init-form #nil
	#:getter mtfa-stm::data
	#:setter mtfa-stm::data!
   )
  ;;
  ;;L'indice in cui inserirò il prossimo elemento
  (index #:init-form 0
	#:getter mtfa-stm::index
	#:setter mtfa-stm::index!
   )
  ;; ;;
  ;; ;;Il prefisso del file in cui salvare i dati
  ;; (fsave #:init-form #nil
  ;;        #:init-keyword #:fsave
  ;;        #:getter mtfa-um::fsave
  ;;        #:setter mtfa-um::fsave!)
  ;; ;;
  ;; ;;Dove appendere operazione per operazione
  ;; (log #:init-form #nil
  ;;      ;;#:init-keyword #:log
  ;;      #:getter mtfa-um::log
  ;;      #:setter mtfa-um::log!)
  ;; ;;
  ;; ;;La serializzazione. Se presente, prima serializzazione e dopo il log
  ;; (serialized #:init-form #nil
  ;;      ;;#:init-keyword #:serialized
  ;;      #:getter mtfa-um::serialized
  ;;      #:setter mtfa-um::serialized!)
  ;; )
  )

;; ;;Appende come stringhe al file di log
;; (define (FileAppendUm fil cmd key data)
;;   ;;(Show! "Fil: " fil ", cmd: " cmd " key: " key " data: " data)
;;   (let ((output-port (open-file fil "a")))
;;     (display (mtfa-b64-encode cmd)  output-port) (newline output-port)
;;     (display (mtfa-b64-encode key)  output-port) (newline output-port)
;;     (display (mtfa-b64-encode data) output-port) (newline output-port)
;;     (close output-port))
;;   )

(define-method (initialize (ds <mtfa-star-map>) initargs)
  (next-method)
  (mtfa-stm::index! ds 0)
  (mtfa-stm::data! ds (make-vector 128))
  ;; (mtfa-assert (string? (mtfa-um::fsave ds))
  ;;              "<mtfa-um>: must be created with #:fsave filename parameter")

  ;; (mtfa-um::log! ds (string-append (mtfa-um::fsave ds) ".um.log"))
  ;; (mtfa-um::serialized! ds (string-append (mtfa-um::fsave ds) ".um.ser"))
  ;; ;;ora, faccio la reload dei dati e recupero eventuale file di log
  ;; (Show! "mtfa-um: Log " (mtfa-um::log ds) ". Ser: " (mtfa-um::serialized ds))
  ;; (mtfa-um::restore ds)
  )

;;allora, i dati sono organizzati nel seguente modo: quando inserisco ho, associato alla chiave, il cons (#t/f . v)
;;quando cancello ci metto #f

(define-method-public (mtfa-stm::add-rule (ds <mtfa-star-map>) (skey <string>) (jolly <string>) value)
  (with-mutex (mtfa-stm::mtx ds)
    (let ((rds (mtfa-stm::star ds))
	  (index (mtfa-stm::index ds))
	  (data (mtfa-stm::data ds))
	  (newdata #nil)
	  )
      ;;se l'index è >= delle dimensioni del vettore, allora faccio la resize del vettore
      (when (>= index (vector-length data))
	(set! newdata (make-vector (+ 128 (vector-length data))))
	(vector-move-left! data 0 128 newdata 0)
	(mtfa-stm::data! ds newdata)
	(set! data (mtfa-stm::data ds)))
      ;;
      ;;inserisco il valore nel vettore
      (vector-set! data index value)
      ;;
      ;;Aggiorno indice per la prossima insert
      (mtfa-stm::index! ds (1+ index))
      ;;
      ;;inserisco elemento nella star
      (mtfa-star-insert rds skey index jolly)
      ;;
      ;;Fine delle operazioni
      #t)))

(define-method-public (mtfa-stm::build (ds <mtfa-star-map>))
  (with-mutex (mtfa-stm::mtx ds)
    (let ((rds (mtfa-stm::star ds)))
      (mtfa-star-build rds)
      #t)))

(define-method-public (mtfa-stm::search (ds <mtfa-star-map>) (key <string>))
  (with-mutex (mtfa-stm::mtx ds)
    (let ((rds (mtfa-stm::star ds))
	  (data (mtfa-stm::data ds))
	  (rule #f)
	  (lindex #nil)
	  )
      (set! rule (mtfa-star-search rds key)) ;;se non trova, #f
      (when rule
	;;recupera tutti gli indici delle regole che matchano!
	(set! lindex (mtfa-star-get-rules rds rule))
	)
      (map (lambda (idx) (vector-ref data idx)) lindex))))

