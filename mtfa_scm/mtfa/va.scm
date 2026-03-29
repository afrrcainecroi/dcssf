(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa va)
  ;;I miei moduli
  #:use-module (mtfa error-handler)
  #:use-module (mtfa utils)
  ;;#:use-module (mtfa simple_db)
  ;;#:use-module (mtfa certs)
  #:use-module (mtfa eis)
  
  ;;;i moduli di guile
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-18)
;; date & time rinomina per avere un current time che non si sovrappone
  #:use-module (srfi srfi-19)
  ;;#:use-module (srfi srfi-28)
  #:use-module (srfi srfi-43)
  #:use-module (srfi srfi-60)
  
  #:use-module (web uri)
  #:use-module (system foreign)

  #:use-module (rnrs bytevectors)
;;  #:use-module (rnrs io ports)

  #:use-module (ice-9 ftw)
  #:use-module (ice-9 rdelim)
  #:use-module (ice-9 pretty-print)
  #:use-module (ice-9 regex)
  #:use-module (ice-9 iconv)
  #:use-module (ice-9 vlist)
  #:use-module (ice-9 q)

  #:use-module (sxml simple)
  #:use-module (json)
  #:export (
            va::vaeng::run-string
            va::vaeng::AddToSet
            va::vaeng::DelFromSet
            va::vaeng::GetFromSet
	    va::vaeng::AddToDict 
	    va::vaeng::SendRawData
	    va::vaeng::CheckIfDeallocated
	    va::vaeng::fast-scan-external
	    va::vaeng::fast-scan-stringset
	    ))

;;queste le sovrascrivo la linguaggio C in modo da collegare le funzioni vere!
(define va::vaeng::AddToSet #nil) ;;per avercela comunque, anche se non linkate al codice VA
(define va::vaeng::AddToDict #nil)
(define va::vaeng::DelFromSet #nil)
(define va::vaeng::GetFromSet #nil)
(define va::vaeng::SendRawData #nil)
(define va::vaeng::CheckIfDeallocated #nil)
(define va::vaeng::fast-scan-external #nil)
(define va::vaeng::fast-scan-stringset #nil)

(define (va::vaeng::run-string str)
  (Show! str)
  (eval-string str)
  )

;;Inizializzazione del dispatcher: uno per tutte le classi di manage
(define-public dispatcher (mtfa-dispatcher::make eis::BaseLib::BlockSession))

(define-public (KamRun key old-return values pbuf)
  (Show "In your code you have to define the function KamRun as (define KamRun key old-return list-of-blank-separated-values pbuf)\nWhere the key identifies the operation you want to do")
  (primitive-exit -1))


;;Questo fa il parsing dei body multipart
(define-public (va::parse-multipart-body pbuf body)
  (mtfa-noerr #nil
	     ;;Se in pbuf il content-type è multipart, allora prendo i dati interpretati. Ma tanto questi lo sono già e quindi vedo subito la lista lmr
	     (if (nil? (mtfa-eis-get-current-multipart pbuf))
		 (begin
		   ;;il body è del tipo var-name! Lo splitto e torno la alist
		   ;;(Show "SPlit normale: " body)
		   (eis::SplitPostData body))
		 (begin
		   ;;il body è multipart, navigo la lista dei dati e prendo le coppie k,v
		   ;;per ora mi limito a prendere name=.. dalla parte valore e tutti i dati
		   (fold-right
		    (lambda (it prev)
		      ;;tre elementi: Headername, headerValue, Data. Prendo da headerValue la parte name= e tutta la parte dati
		      ;;(Show "I DATI: " it ". CAR: " (car it) ". CADR: " (cadr it) ". CADDR: " (caddr it))
		      (let* ((f (FindPattern "name=\"([^\"]*)\"" (cadr it)))
			     (d(Show "Ora f: " f))
			     (f (if f (match:substring f 1) #nil)))
			;;(Show "Trovata: " f)
			(if f
			    (acons
			     f
			     (bytevector->string (caddr it) "utf8")
			     prev)
			    #nil
			    )
			))
		    #nil
		    (mtfa-eis-get-current-multipart pbuf))))))


