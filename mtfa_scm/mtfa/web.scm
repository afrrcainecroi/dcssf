(add-to-load-path "/volume1/sources/NEW_DEVS/DEPLOYED/")
(set! %load-compiled-path (cons "/volume1/sources/NEW_DEVS/DEPLOYED/" %load-compiled-path))

(define-module (mtfa web)
#:use-module  (mtfa error-handler)
#:use-module  (mtfa utils)
#:use-module  (mtfa serializer)
;;#:use-module  (mtfa unordered-set) ;;unordered set con chiavi (stringhe, numeri o sumimboli: tutto convertito in stringa). Persistente
;;#:use-module  (mtfa unordered-map) ;;unordered map con chiavi (stringhe) e valori (qualsiasi cosa). persistente
;;#:use-module  (mtfa star-map)      ;;Inserisce stringhe con o senza jolly, la stringa che definisce il jolly e il valore. Cerca le stringhe che matchano!
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
#:use-module (ice-9 match)


#:use-module  (oop goops)
;; #:use-module  (sxml simple)
;; #:use-module  (sxml ssax)
;; #:use-module  (sxml xpath)
#:use-module  (json)

)

;;
;;la classe blum di base per generare o scambiare codici cifrati
(define-class-public <mtfa-web-security-base-blum> ()
  (blum-q #:init-value 13390486608323471273067934336893295887407911548146346268857942602271283052699655080646007180442083962239825480387431895159817241576801387010931399503295443
               #:getter mwsb::blum-q)
  ;;
  (blum-m #:init-value (* 13390486608323471273067934336893295887407911548146346268857942602271283052699655080646007180442083962239825480387431895159817241576801387010931399503295443 8163880775989783788904844076199153159963674320489254360049766294792952506866056342357566552165477009293091323259570950620357696492960851306422242915610443)
               #:getter mwsb::blum-m)
  ;;
  (blum-p #:init-value 8163880775989783788904844076199153159963674320489254360049766294792952506866056342357566552165477009293091323259570950620357696492960851306422242915610443
               #:getter mwsb::blum-p)
  ;;
  (blum #:init-value (let ((bb (mtfa-blum-make 16)))
                      (mtfa-blum-set-values
                       bb
                       (* 13390486608323471273067934336893295887407911548146346268857942602271283052699655080646007180442083962239825480387431895159817241576801387010931399503295443 8163880775989783788904844076199153159963674320489254360049766294792952506866056342357566552165477009293091323259570950620357696492960851306422242915610443)
                       13390486608323471273067934336893295887407911548146346268857942602271283052699655080646007180442083962239825480387431895159817241576801387010931399503295443
                       8163880775989783788904844076199153159963674320489254360049766294792952506866056342357566552165477009293091323259570950620357696492960851306422242915610443
                       )
                      bb)
             #:getter mwsb::blum)
  )

(define-method (initialize (ds <mtfa-web-security-base-blum>))
  (next-method)
  )

(defun MakeResidue (num blum) #f =>
          (when (< num 13390486608323471273067934336893295887407911548146346268857942602271283052699655080646007180442083962239825480387431895159817241576801387010931399503295443)
            (set! num (string->number (mtfa-rand-string 256))))
          ;;ora posso passare a generare un residuo quadratico
          (let loop ((residuo num) (next (mtfa-blum-get-next num blum)))
            (if (= residuo (mtfa-blum-get-prev next blum))
                residuo
                (loop next (mtfa-blum-get-next next blum)))))

(defun Cypher (bv blum) #f =>
          ;;a partire dal bv faccio un residuo quadratico e poi, con questo, cifro in xor
          ;;il bytevector. Utilizzo i primi 8 bit di ogni elevamento a quadrato
          (let loop ((code (MakeResidue (mtfa-bv-to-num bv) blum))
                     (cyphered (bytevector-copy bv))
                     (i 0)
                     (len (bytevector-length bv))
                     )
            (if (zero? len)
                (cons cyphered code)
                ;;prendo i primi 8 bit di code e cifro la stringa!
                (begin
                  (bytevector-u8-set! cyphered i (logxor (bytevector-u8-ref cyphered i) (logand code #xFF)))
                  (loop (mtfa-blum-get-next code blum) cyphered (1+ i) (1- len)))
                )))
(defun Decypher (cyphered code blum) #f =>
          (let loop ((code (mtfa-blum-get-prev code blum))
                     (decyphered (bytevector-copy cyphered))
                     (len (bytevector-length cyphered)))
            (if (zero? len)
                (begin
                  decyphered)
                (begin
                  (bytevector-u8-set! decyphered (1- len) (logxor (bytevector-u8-ref decyphered (1- len)) (logand code #xFF)))
                  (loop (mtfa-blum-get-prev code blum) decyphered (1- len)))
                )
            )
          )

(define-method-public (mwsb::make-residue (ds <mtfa-web-security-base-blum>) (num <number>))
  (MakeResidue num (mwsb::blum ds)))

(define-method-public (mwsb::next (ds <mtfa-web-security-base-blum>) (num <number>))
  (mtfa-blum-get-next num (mwsb::blum ds)))

(define-method-public (mwsb::next-ith (ds <mtfa-web-security-base-blum>) (num <number>) (idx <number> ))
  (mtfa-blum-get-next-ith num idx (mwsb::blum ds)))

(define-method-public (mwsb::prev (ds <mtfa-web-security-base-blum>) (num <number>))
  (mtfa-blum-get-prev num (mwsb::blum ds)))

(define-method-public (mwsb::prev-ith (ds <mtfa-web-security-base-blum>) (num <number>) (idx <number> ))
  (mtfa-blum-get-prev-ith num idx (mwsb::blum ds)))

(define-method-public (mwsb::make-token (ds <mtfa-web-security-base-blum>) (base <string>) (timeout <number> ))
  (mtfa-blum-make-token base (+ (seconds) timeout) (mwsb::blum ds)))

(define-method-public (mwsb::identify-token (ds <mtfa-web-security-base-blum>) token)
  (let ((values (mtfa-blum-identify-token token (mwsb::blum ds))))
    ;; (Show! values)
    (if (and values (<= (seconds) (cdr values)))
        (car values)
        #f)))

(define-method-public (mwsb::cypher (ds <mtfa-web-security-base-blum>) (bv <bytevector>))
  (Cypher bv (mwsb::blum ds)))
(define-method-public (mwsb::decypher (ds <mtfa-web-security-base-blum>) (bv <bytevector>) (code <number>))
  (Decypher bv code (mwsb::blum ds)))

;;
;;la classe blum per generare o scambiare codici cifrati
(define-class-public <mtfa-web-security-blum> ()
  (blum-q    #:init-form #nil   #:init-keyword #:p    #:getter mwsb::blum-q #:setter mwsb::blum-q!)
  ;;
  (blum-m    #:init-form #nil   #:init-keyword #:m    #:getter mwsb::blum-m #:setter mwsb::blum-m!)
  ;;
  (blum-p    #:init-form #nil   #:init-keyword #:q    #:getter mwsb::blum-p #:setter mwsb::blum-p!)
  ;;
  (blum-bits #:init-form #nil   #:init-keyword #:bits #:getter mwsb::blum-bits #:setter mwsb::blum-bits!)
  ;;
  (blum      #:init-form #nil                       #:getter mwsb::blum #:setter mwsb::blum!)
  )

(define-method (initialize (ds <mtfa-web-security-blum>) initargs)
  (next-method)
  ;;posso inizializzare nulla oppure p e q oppure m
  (let ((m-is-not-set (nil? (mwsb::blum-m ds)))
        (bit-is-not-set (nil? (mwsb::blum-bits ds)))
        (pq-are-not-set (and (nil? (mwsb::blum-p ds)) (nil? (mwsb::blum-q ds))))
        (how-many-set 0)
        )
    ;; (Show! "m-is-not-set: " m-is-not-set)
    ;; (Show! "bit-is-not-set: " bit-is-not-set)
    ;; (Show! "pq-are-not-set: " pq-are-not-set)
    (when (not m-is-not-set) (set! how-many-set (1+ how-many-set)))
    (when (not bit-is-not-set) (set! how-many-set (1+ how-many-set)))
    (when (not pq-are-not-set) (set! how-many-set (1+ how-many-set)))
    (mtfa-assert (= how-many-set 1)
                 "<mtfa-web-security-blum> must be created with one and only one of #:bits or #:m or #p and #q parameters"))
  ;;inizializzo il blum
  (if (mwsb::blum-bits ds)
      (mwsb::blum! ds (mtfa-blum-make (mwsb::blum-bits ds)))
      (if (mwsb::blum-m ds)
          (let ((m (mwsb::blum-m ds))) ;;per ricordarne il valore
            (mwsb::blum! ds (mtfa-blum-make 16))
            (mwsb::blum-m! ds m)
            (mtfa-blum-set-values (mwsb::blum ds) m 0 0)
            )
          (let ((m (* (mwsb::blum-p ds) (mwsb::blum-p ds)))) ;;per ricordarne il valore
            (mwsb::blum! ds (mtfa-blum-make 16))
            (mwsb::blum-m! ds m)
            (mtfa-blum-set-values (mwsb::blum ds) m (mwsb::blum-p ds) (mwsb::blum-p ds))
            )
          )))

(define-method-public (mwsb::make-residue (ds <mtfa-web-security-blum>) (num <number>))
  (MakeResidue num (mwsb::blum ds)))

(define-method-public (mwsb::next (ds <mtfa-web-security-blum>) (num <number>))
  (mtfa-blum-get-next num (mwsb::blum ds)))

(define-method-public (mwsb::next-ith (ds <mtfa-web-security-blum>) (num <number>) (idx <number> ))
  (mtfa-blum-get-next-ith num idx (mwsb::blum ds)))

(define-method-public (mwsb::prev (ds <mtfa-web-security-blum>) (num <number>))
  (mtfa-blum-get-prev num (mwsb::blum ds)))

(define-method-public (mwsb::prev-ith (ds <mtfa-web-security-blum>) (num <number>) (idx <number> ))
  (mtfa-blum-get-prev-ith num idx (mwsb::blum ds)))

(define-method-public (mwsb::make-token (ds <mtfa-web-security-blum>) (base <string>) (timeout <number> ))
  (mtfa-blum-make-token base (+ (seconds) timeout) (mwsb::blum ds)))

(define-method-public (mwsb::identify-token (ds <mtfa-web-security-blum>) token)
  (let ((values (mtfa-blum-identify-token token (mwsb::blum ds))))
    ;;(Show! values)
    (if (and values (<= (seconds) (cdr values)))
        (car values)
        #f)))

(define-method-public (mwsb::cypher (ds <mtfa-web-security-blum>) (str <bytevector>))
  (Cypher str (mwsb::blum ds)))
(define-method-public (mwsb::decypher (ds <mtfa-web-security-blum>) (bv <bytevector>) (code <number>))
  (Decypher bv code (mwsb::blum ds)))

;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;
;;;;END BLUM. START COOKIE ;;;;
;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;

;;
;;
;;cookie maker & checker
(define-class-public <mtfa-web-cookie-maker> ()
  (strong        #:init-form #nil   #:init-keyword #:strong         #:getter mwcm::strong)
  ;;
  (use-blum-base #:init-form #nil   #:init-keyword #:use-blum-base  #:getter mwcm::use-blum-base)
  ;;
  (blum-class    #:init-form #nil                                 #:getter mwcm::blum-class #:setter mwcm::blum-class!)
  )

(define-method (initialize (ds <mtfa-web-cookie-maker>) initargs)
  (next-method)
  ;; #:strong = #t/#f. Se #f allora non usa blum. Se #t usa blum
  ;; #:use-blum-base se #t usa la classe blum base, altrimenti la inizializza randomly a 256 (fast e good)
  (let ((is-strong?     (mwcm::strong ds))
        (use-blum-base? (mwcm::use-blum-base ds))
        )
    
    ;; (Show! "sgtrong: " is-strong? ", blum-base: " use-blum-base?)
    (mtfa-assert (or (mtfa-nequal? #nil is-strong?) (mtfa-nequal? #nil use-blum-base?))
                 "<mtfa-web-cookie-maker> must be created with one and only one of #:strong (#t/#f use strong crypt or standard random), #:use-blum-base (#t/#f) uses global blum or creates one ad hoc")
    (if is-strong?
      (if use-blum-base?
          (mwcm::blum-class! ds (make <mtfa-web-security-base-blum>))
          (mwcm::blum-class! ds (make <mtfa-web-security-blum> #:bits 256))))))

;;make_cookie(SCM key, SCM value, SCM expiration, SCM howmanysecs, SCM path, SCM domain, SCM http_only, SCM secure)
;; SessionCookie, NeverExpires, ExpiresInSeconds
(define-method-public (mwcm::make-a-cookie (ds <mtfa-web-cookie-maker>)
                                           (key <string>)
                                           (expiration <string>)
                                           (duration <number>)
                                           (path <string>)
                                           (domain <string>)
                                           (http_only <boolean>)
                                           (secure <boolean>)
                                           )
  (let ((value #f))
    (if (mwcm::strong ds)
        (let ((value (number->string (mwsb::make-token (mwcm::blum-class ds) key duration))))
          ;;uso blum
          (cons value (mtfa-cookie-make key value expiration duration path domain http_only secure)))
        (let ((value (mtfa-time-stamp-make duration)))
          ;;uso random standard
          (cons value (mtfa-cookie-make key value expiration duration path domain http_only secure))))))

(define-method-public (mwcm::check-a-cookie (ds <mtfa-web-cookie-maker>) (value <string>))
  (if (mwcm::strong ds)
      (mwsb::identify-token (mwcm::blum-class ds) (string->number value))
      (mtfa-time-stamp-check value)))

;;
;;Ora è necessario definire un contenitore per una struttura record con una chiave
;;dichiarata. Quindi key, value dove value può essere un tipo strutturato
;;Questa non mi serve poiché la ho già: è la mtfa-unordered-map
;;Invece qualcosa che mi consenta di esemplificare il lavoro delle dichiarazioni ecc per
;;quanto riguarda la gestione dei servizi json associati a URL? va-key è il termine utilizzato
;;dalla va quando riconosce una specifica url

;;va-key corrisponde a quanto indicato nelle validation rule
;;request-manager ha come interfaccia: (request-manager k v pbuf set-names)
;;format-error ha la struttura (define-public (format-error msg) `((success . #f)(message . ,msg)))

;;JSON GENERAL ANSWERS
(define-public (GENERAL-JSON-ANSWER::error msg)
  `((success . #f)(message . ,msg)))

(define-public (GENERAL-JSON-ANSWER::success msg data_name data)
  `((success . #t)(message . ,msg) (data . ((,data_name . ,data)))))

(define*-public (mtfa-web-make-json-requests-manager va-key #:key (format-error GENERAL-JSON-ANSWER::error) (auxiliary-info '()))
  ;;funzioni interne di gestione
  (define kv (mtfa-fs3-make)) ;;per la registrazione delle funzioni e per la loro chiamata
  (define (ManageRequest k v pbuf set-names)
    (mtfa-noerr #f
	       ;;(Show! "ManageRequest. enter with: " k ", " v ", " pbuf ", " set-names)
	       ;;
	       ;;ora il gestore dei comandi          
	       ;;
	       (let ((evaluator (mtfa-fs3-get kv k)))
		 (if (nil? evaluator)
		     (begin
		       ;;(Show! "mtfa-manage-json-requests: command " k " not registered.")
		       (eis::GiveHTTPJSONAnswer (format-error (string-append "command " k " not found!"))))
		     (evaluator k v pbuf set-names auxiliary-info)))))
  (define (ManageRequests jparsed pbuf set-names)
    ;;la richiesta dovrebbe essere unica. Se non lo fosse, al primo errore si ferma!
    (let ((result #nil) (continue #t))
      ;;il parsed può essere tante cose. Mi aspetto hash o alist (key value)
      (if (hash-table? jparsed)
	  (hash-for-each
           (lambda (k v)
	     (if continue
		 (begin
	           (set! result (ManageRequest k v pbuf set-names))
	           (when (nil? result) (set! continue #f)))
		 #f))
           jparsed)
	  (for-each
	   (lambda (it)
	     (if continue
		 (begin
	           (set! result (ManageRequest (car it) (cdr it) pbuf set-names))
	           (when (nil? result) (set! continue #f)))
		 #f))
	   jparsed))
      result))
  (define (Manage actionl pbuf)
    (mtfa-noerr
     (eis::GiveHTTPJSONAnswer(format-error "Expected a JSON request"))
     (let (
           (body #nil)
           (jparsed #nil)
           (current-method (mtfa-eis-get-current-method pbuf)))
       ;;Prendo il metodo, se option rispondo in modo adeguato
       (cond
        ;;
        ;;OPTIONS
        ((string-ci=? "options" current-method)
         (eis::GiveAnswerAndCloseAll
          (string-append
           "HTTP/1.1 200 OK\r\n"
           "Access-Control-Allow-Origin: "
           (if (nil? (mtfa-eis-get-value-current_headers pbuf "Origin"))
               "*"
               (mtfa-eis-get-value-current_headers pbuf "Origin")) "\r\n"
	       "Access-Control-Allow-Methods: " "GET, POST\r\n"
	       "Access-Control-Allow-Headers: " "accept, content-type\r\n"
	       "\r\n")))
        ;;
        ;;POST, con json nei dati
        ((string-ci=? "post" current-method)
         (set! body  (mtfa-eis-get-current-body pbuf #t))
         (if (StringIsEmptyOrNil body)
             (eis::BaseLib::NeedMoreData)
             (begin
	       ;;(Show "POST: <" body ">, " actionl)
	       (set! jparsed (json-string->scm body))
	       ;;(Show jparsed)
	       (if jparsed
	           (ManageRequests jparsed pbuf actionl)
	           (eis::GiveHTTPJSONAnswer (format-error "Expected a JSON request"))))))
        ;;
        ;;GET (default), dati nella url come json
        (#t
         ;;Qui mi aspetto json in accompagnamento della chiave req
         (set! body (mtfa-eis-get-current-pars pbuf))
	 ;;(Show "PARS: " body)
         ;;
         (set! jparsed (json-string->scm (uri-decode body)))
         (if jparsed
	     (ManageRequests jparsed pbuf actionl)
	     (eis::GiveHTTPJSONAnswer (format-error "Expected a JSON request"))))))))
  ;;
  ;;istanzia la chiamata e formula la funzione di gestione
  (eis::function-pointer-add va-key Manage)
  (lambda (cmd . params)
    (cond
     ((eqv? 'add-handler cmd)
      (mtfa-fs3-add kv (first params) (second params)))
     ((eqv? 'del-handler cmd)
      (mtfa-fs3-add kv (first params) #nil))
     ((eqv? 'list)
      (mtfa-fs3-get-all kv)
      )
     ;;per ora altri comandi non previsti!
     (#t (Show! "Command: " cmd " not handled!"))
     )
    )
  )


;; ;;
;; ;;Un esempio di session manager
;; ;;definisce id di sessione con una durata prefissata e gli associa dati
;; (define mtfa-http-sessions (make-hash-table))
;; (define mtfa-http-sessions-mutex (make-mutex))
;; (define (mtfa-http-session-manager-purge)
;;   (mtfa-noerr
;;    #f
;;    (let loop ()
;;       (let ((elenco #nil))
;; 	(with-mutex mtfa-http-sessions-mutex
;; 	  (set! elenco (hash-map->list (lambda (k v) k) mtfa-http-sessions))
;; 	  )
;; 	;;ora che ho la lista, per ogni elemento, lo tolgo
;; 	(for-each (lambda (k)
;; 		    (unless (mtfa-time-stamp-check k)
;; 		      (with-mutex mtfa-http-sessions-mutex
;; 			(hash-remove! mtfa-http-sessions k))))
;; 		  elenco))
;;       (sleep 10)  ;;ogni 10 secondi, purga!
;;       (loop))))

;; (define launched-mtfa-http-session-manager-purge #f)
;; ;;
;; ;;A simple HTTP session manager, it can be customizable, adding data!!
;; (define-public (mtfa-http-session-manager cmd . rest)
;;   "Creates, removes and checks sessions id: commands 'get <duration in seconds> <optional data>, 'check <sid> => returns data, 'del <sid>.
;; Returns #f on error"
;;   (mtfa-noerr
;;    #f
;;    (unless launched-mtfa-http-session-manager-purge
;;      (call-with-new-thread mtfa-http-session-manager-purge)
;;      (set! launched-mtfa-http-session-manager-purge #t))
;;    (with-mutex mtfa-http-sessions-mutex
;;      (cond
;;       ((eqv? cmd 'check)
;;        (let ((value (hash-ref mtfa-http-sessions (car rest))))
;; 	 (if (and value (mtfa-time-stamp-check (car rest)))
;; 	     value
;; 	     (begin
;; 	       (hash-remove! mtfa-http-sessions (car rest))
;; 	       #f))))
;;       ((eqv? cmd 'get)
;;        (cond  ;;nel caso in cui il richiedente non avesse inserito parametri o ne avesse messi meno del necessario
;; 	((zero? (length rest)) (set! rest '(1800 #t)))  ;;per default la sessione dura al massimo 1/2 ora
;; 	((= 1 (length rest))   (set! rest (cons (car rest) #t))))
;;        (let ((sid (let loop ((sid (mtfa-time-stamp-make (car rest))))
;; 		    (ifnot (hash-ref mtfa-http-sessions sid)
;; 			   sid
;; 			   (loop (mtfa-time-stamp-make (car rest)))))))
;; 	 (hash-set! mtfa-http-sessions sid (cdr rest))
;; 	 sid))
;;       ((eqv? cmd 'del)
;;        (hash-remove! mtfa-http-sessions (car rest)))
;;       ((eqv? cmd 'count)
;;        (hash-count (const #t) mtfa-http-sessions))
;;       (#t #f)))))

;;dei sid abbastanza veloci!!!
(define-public (mtfa-session-id-make duration stringa)
  (mtfa-noerr #f
	     (let ((timestamp (mtfa-time-stamp-make duration)))
	       ;;qui concateno il timestamp con la serializzazione della parte dati, compressa e cifrata con il timestamp
	       (string-append timestamp "-" (mtfa-cifra-aes-js-hex stringa timestamp)))))

(define-public (mtfa-session-id-check full-sid)
  (mtfa-noerr #f
	     (let* ((split (string-index-right full-sid #\-))
		    (sid (substring full-sid 0 split))
		    (data (substring full-sid (1+ split))))
	       (cons (mtfa-time-stamp-check sid) (mtfa-decifra-aes-js-hex-s data sid) ))))


;;
;;Ora la parte di conversione html
;;
(define %void-elements
 '(area
   base
   br
   col
   command
   embed
   hr
   img
   input
   keygen
   link
   meta
   param
   source
   track
   wbr))

(define (void-element? tag)
 "Return #t if TAG is a void element."
 (pair? (memq tag %void-elements)))

(define %escape-chars
 (alist->hash-table
  '((#\" . "quot")
    ;; (#\& . "amp")
    (#\' . "apos")
    (#\< . "lt")
    (#\> . "gt")
    (#\¡ . "iexcl")
    (#\¢ . "cent")
    (#\£ . "pound")
    (#\¤ . "curren")
    (#\¥ . "yen")
    (#\¦ . "brvbar")
    (#\§ . "sect")
    (#\¨ . "uml")
    (#\© . "copy")
    (#\ª . "ordf")
    (#\« . "laquo")
    (#\¬ . "not")
    (#\® . "reg")
    (#\¯ . "macr")
    (#\° . "deg")
    (#\± . "plusmn")
    (#\² . "sup2")
    (#\³ . "sup3")
    (#\´ . "acute")
    (#\µ . "micro")
    (#\¶ . "para")
    (#\· . "middot")
    (#\¸ . "cedil")
    (#\¹ . "sup1")
    (#\º . "ordm")
    (#\» . "raquo")
    (#\¼ . "frac14")
    (#\½ . "frac12")
    (#\¾ . "frac34")
    (#\¿ . "iquest")
    (#\À . "Agrave")
    (#\Á . "Aacute")
    (#\Â . "Acirc")
    (#\Ã . "Atilde")
    (#\Ä . "Auml")
    (#\Å . "Aring")
    (#\Æ . "AElig")
    (#\Ç . "Ccedil")
    (#\È . "Egrave")
    (#\É . "Eacute")
    (#\Ê . "Ecirc")
    (#\Ë . "Euml")
    (#\Ì . "Igrave")
    (#\Í . "Iacute")
    (#\Î . "Icirc")
    (#\Ï . "Iuml")
    (#\Ð . "ETH")
    (#\Ñ . "Ntilde")
    (#\Ò . "Ograve")
    (#\Ó . "Oacute")
    (#\Ô . "Ocirc")
    (#\Õ . "Otilde")
    (#\Ö . "Ouml")
    (#\× . "times")
    (#\Ø . "Oslash")
    (#\Ù . "Ugrave")
    (#\Ú . "Uacute")
    (#\Û . "Ucirc")
    (#\Ü . "Uuml")
    (#\Ý . "Yacute")
    (#\Þ . "THORN")
    (#\ß . "szlig")
    (#\à . "agrave")
    (#\á . "aacute")
    (#\â . "acirc")
    (#\ã . "atilde")
    (#\ä . "auml")
    (#\å . "aring")
    (#\æ . "aelig")
    (#\ç . "ccedil")
    (#\è . "egrave")
    (#\é . "eacute")
    (#\ê . "ecirc")
    (#\ë . "euml")
    (#\ì . "igrave")
    (#\í . "iacute")
    (#\î . "icirc")
    (#\ï . "iuml")
    (#\ð . "eth")
    (#\ñ . "ntilde")
    (#\ò . "ograve")
    (#\ó . "oacute")
    (#\ô . "ocirc")
    (#\õ . "otilde")
    (#\ö . "ouml")
    (#\÷ . "divide")
    (#\ø . "oslash")
    (#\ù . "ugrave")
    (#\ú . "uacute")
    (#\û . "ucirc")
    (#\ü . "uuml")
    (#\ý . "yacute")
    (#\þ . "thorn")
    (#\ÿ . "yuml")
    (#\Œ . "OElig")
    (#\œ . "oelig")
    (#\Š . "Scaron")
    (#\š . "scaron")
    (#\Ÿ . "Yuml")
    (#\ƒ . "fnof")
    (#\ˆ . "circ")
    (#\˜ . "tilde")
    (#\Α . "Alpha")
    (#\Β . "Beta")
    (#\Γ . "Gamma")
    (#\Δ . "Delta")
    (#\Ε . "Epsilon")
    (#\Ζ . "Zeta")
    (#\Η . "Eta")
    (#\Θ . "Theta")
    (#\Ι . "Iota")
    (#\Κ . "Kappa")
    (#\Λ . "Lambda")
    (#\Μ . "Mu")
    (#\Ν . "Nu")
    (#\Ξ . "Xi")
    (#\Ο . "Omicron")
    (#\Π . "Pi")
    (#\Ρ . "Rho")
    (#\Σ . "Sigma")
    (#\Τ . "Tau")
    (#\Υ . "Upsilon")
    (#\Φ . "Phi")
    (#\Χ . "Chi")
    (#\Ψ . "Psi")
    (#\Ω . "Omega")
    (#\α . "alpha")
    (#\β . "beta")
    (#\γ . "gamma")
    (#\δ . "delta")
    (#\ε . "epsilon")
    (#\ζ . "zeta")
    (#\η . "eta")
    (#\θ . "theta")
    (#\ι . "iota")
    (#\κ . "kappa")
    (#\λ . "lambda")
    (#\μ . "mu")
    (#\ν . "nu")
    (#\ξ . "xi")
    (#\ο . "omicron")
    (#\π . "pi")
    (#\ρ . "rho")
    (#\ς . "sigmaf")
    (#\σ . "sigma")
    (#\τ . "tau")
    (#\υ . "upsilon")
    (#\φ . "phi")
    (#\χ . "chi")
    (#\ψ . "psi")
    (#\ω . "omega")
    (#\ϑ . "thetasym")
    (#\ϒ . "upsih")
    (#\ϖ . "piv")
    (#\  . "ensp")
    (#\  . "emsp")
    (#\  . "thinsp")
    (#\– . "ndash")
    (#\— . "mdash")
    (#\‘ . "lsquo")
    (#\’ . "rsquo")
    (#\‚ . "sbquo")
    (#\“ . "ldquo")
    (#\” . "rdquo")
    (#\„ . "bdquo")
    (#\† . "dagger")
    (#\‡ . "Dagger")
    (#\• . "bull")
    (#\… . "hellip")
    (#\‰ . "permil")
    (#\′ . "prime")
    (#\″ . "Prime")
    (#\‹ . "lsaquo")
    (#\› . "rsaquo")
    (#\‾ . "oline")
    (#\⁄ . "frasl")
    (#\€ . "euro")
    (#\ℑ . "image")
    (#\℘ . "weierp")
    (#\ℜ . "real")
    (#\™ . "trade")
    (#\ℵ . "alefsym")
    (#\← . "larr")
    (#\↑ . "uarr")
    (#\→ . "rarr")
    (#\↓ . "darr")
    (#\↔ . "harr")
    (#\↵ . "crarr")
    (#\⇐ . "lArr")
    (#\⇑ . "uArr")
    (#\⇒ . "rArr")
    (#\⇓ . "dArr")
    (#\⇔ . "hArr")
    (#\∀ . "forall")
    (#\∂ . "part")
    (#\∃ . "exist")
    (#\∅ . "empty")
    (#\∇ . "nabla")
    (#\∈ . "isin")
    (#\∉ . "notin")
    (#\∋ . "ni")
    (#\∏ . "prod")
    (#\∑ . "sum")
    (#\− . "minus")
    (#\∗ . "lowast")
    (#\√ . "radic")
    (#\∝ . "prop")
    (#\∞ . "infin")
    (#\∠ . "ang")
    (#\∧ . "and")
    (#\∨ . "or")
    (#\∩ . "cap")
    (#\∪ . "cup")
    (#\∫ . "int")
    (#\∴ . "there4")
    (#\∼ . "sim")
    (#\≅ . "cong")
    (#\≈ . "asymp")
    (#\≠ . "ne")
    (#\≡ . "equiv")
    (#\≤ . "le")
    (#\≥ . "ge")
    (#\⊂ . "sub")
    (#\⊃ . "sup")
    (#\⊄ . "nsub")
    (#\⊆ . "sube")
    (#\⊇ . "supe")
    (#\⊕ . "oplus")
    (#\⊗ . "otimes")
    (#\⊥ . "perp")
    (#\⋅ . "sdot")
    (#\⋮ . "vellip")
    (#\⌈ . "lceil")
    (#\⌉ . "rceil")
    (#\⌊ . "lfloor")
    (#\⌋ . "rfloor")
    (#\〈 . "lang")
    (#\〉 . "rang")
    (#\◊ . "loz")
    (#\♠ . "spades")
    (#\♣ . "clubs")
    (#\♥ . "hearts")
    (#\♦ . "diams"))))

(define (string->escaped-html s port)
 "Write the HTML escaped form of S to PORT."
 (define (escape c)
   ;; (Show! "Carattere: " c " => " (hash-ref %escape-chars c))
   (let ((escaped (hash-ref %escape-chars c)))
     (if escaped
         (format port "&~a;" escaped)
         (display c port))))
 (string-for-each escape s))

(define (object->escaped-html obj port)
 "Write the HTML escaped form of OBJ to PORT."
 (string->escaped-html
  (call-with-output-string (cut display obj <>))
  port))

(define (attribute-value->html value port)
 "Write the HTML escaped form of VALUE to PORT."
 (if (string? value)
     (string->escaped-html value port)
     (object->escaped-html value port)))

(define (attribute->html attr value port)
 "Write ATTR and VALUE to PORT."
 (format port "~a=\"" attr)
 (attribute-value->html value port)
 (display #\" port))

(define (attribute->html-nv attr port)
 "Write ATTR to PORT."
 (format port "~a" attr)
)

(define (element->html tag attrs body port)
 "Write the HTML TAG to PORT, where TAG has the attributes in the
list ATTRS and the child nodes in BODY."
 (format port "<~a" tag)
 (for-each (match-lambda
            ((attr value)
             (display #\space port)
             (attribute->html attr value port))
            ((attr)
             (display #\space port)
             (attribute->html-nv attr port))
	    )
           attrs)
 (if (and (null? body) (void-element? tag))
     (display " />" port)
     (begin
       (display #\> port)
       (for-each (cut sxml->html <> port) body)
       (format port "</~a>" tag))))

(define (doctype->html doctype port)
 (format port "<!DOCTYPE ~a>" doctype))

(define (sxml->html tree port)
  "Returns the serialized HTML form of TREE"
  (match tree
    (() *unspecified*)
    (('doctype type)
     (doctype->html type port))
    ;; Unescaped, raw HTML output
    (('raw html)
     (display html port))
    (((? symbol? tag) ('@ attrs ...) body ...)
     (element->html tag attrs body port))
    (((? symbol? tag) body ...)
     (element->html tag '() body port))
    ((nodes ...)
     (for-each (cut sxml->html <> port) nodes))
    ((? string? text)
     (string->escaped-html text port))
    ;; Render arbitrary Scheme objects, too.
    (obj (object->escaped-html obj port))))

(define-public (mtfa-sxml2html tree)
 "Returns the serialized HTML form of TREE
EXAMPLES
(sxml2html `(div (@ (class \"p-2 hoverable border rounded\") (style \"height: auto;width: auto;\"))
		  (div (@ (class \"mb-0\")) ,(string-append label \"(\" units \")\"))
		  (input (@ (style \"width: 90%;\")(type \"text\") (id ,id) (name ,id) (class \"form-control\") (autocomplete \"nope\") (value \"\")))))
(sxml2html `(div (@ (class \"p-2 range-field hoverable border rounded\") (style \"height: auto;width: auto;\"))
			  (div (@ (class \"row mb-3\"))
			       (div (@ (class \"mb-0 col-9\")) ,(string-append label \"(\" units \")\"))
			       (fiv (@ (class \"mb-0 col-2\") (id ,(string-append \"slider-value-\" id))) \"\"))
			  (input (@ (value \"-1\") (type \"range\") (min ,(vector-ref min-max 0)) (max ,(vector-ref min-max 1)) 
				    (id ,id) (name ,id) (class \"a-slider form-control col-12\")))))
(mtfa-sxml2html '((doctype \"html\")(html (head (raw \"<meta http-equiv=\\\"Content-Type\\\" content=\\\"text/html; charset=UTF-8\\\">\") (meta (@ (charset \"UTF-8\"))) (title \"Titolo\")) (body ((uno 1)(due 2))(tre (@ (a 10))3)))))
"
  (call-with-output-string 
	(lambda (port)
	  ;;(Show! "Tree: " tree  ". Encoding: " (port-encoding port))
	  (sxml->html tree port))))
