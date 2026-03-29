(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa eis)
  ;;I miei moduli
  #:use-module (mtfa error-handler)
  #:use-module (mtfa utils)
  #:use-module (mtfa simple_db)
  
  ;;;i moduli di guile
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  ;; #:use-module (srfi srfi-18)
;; date & time rinomina per avere un current time che non si sovrappone
  #:use-module (srfi srfi-19)
  #:use-module ((srfi srfi-43) #:prefix  datetime:)
  
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
  #:export (
	    DoYourJob-nopar
	    ;;eis::function-pointer
	    ;; eis-parameters
	    ;; make-eis-parameters
	    ;; eis-parameters?
	    ;; eis-parameters-actions
	    ;; eis-parameters-current_query
	    ;; eis-parameters-current_headers
	    ;; eis-parameters-current_cookie
	    ;; eis-parameters-direction
	    ;; eis-parameters-the_body
	    ;; eis-parameters-s_loc_method
	    ;; eis-parameters-s_loc_protocol
	    ;; eis-parameters-s_loc_host
	    ;; eis-parameters-s_loc_port
	    ;; eis-parameters-s_loc_url
	    ;; eis-parameters-s_loc_pars
	    ;; eis-parameters-s_loc_tag
	    ;; eis-parameters-s_loc_user_passwd
	    ;; eis-parameters-mac_src
	    ;; eis-parameters-ip_src
	    eis::BaseLib::PassToServer
	    eis::BaseLib::NeedMoreData
	    eis::BaseLib::ContinueAfterAccept
	    eis::BaseLib::BlockSession
	    eis::GiveAnswerAndCloseAll
	    eis::GiveAnswerAndNotClose
	    eis::Redirect
	    eis::GiveFileAndClose
	    eis::GiveFileAndNotClose
	    eis::http-content-type-html
	    eis::http-content-type-text
	    eis::http-content-type-b64
	    eis::http-content-type-xml
            eis::http-content-type-javascript
	    eis::http-answer-ok
	    eis::http-content-length
	    eis::http-connection-close
	    eis::http-connection-keep-alive
	    eis::http-eoln
	    eis::http-end-of-headers
	    eis::BaseLib::BuildHTTPAnswer
	    eis::GiveHTTPAnswer
	    eis::GiveHTTPHtmlAnswer
	    eis::GiveHTTPXmlAnswer
	    eis::GiveHTTPTextAnswer
	    eis::GiveHTTPB64Answer
	    eis::GiveHTTPJSONAnswer
	    eis::GiveErrorHTTP404
	    eis::GiveErrorHTTP401
	    eis::GiveErrorHTTP500
	    eis::LoadPage
	    eis::SplitPostData
	    mtfa-eis-get-current-query
	    mtfa-eis-get-value-current-query
	    mtfa-eis-get-value-current-headers
	    mtfa-eis-get-current-headers
	    mtfa-eis-get-value-current-cookies
	    mtfa-eis-get-current-body
	    mtfa-eis-get-current-method
	    mtfa-eis-get-current-protocol
	    mtfa-eis-get-current-host
	    mtfa-eis-get-current-port
	    mtfa-eis-get-current-pars
	    mtfa-eis-get-current-tag
	    mtfa-eis-get-current-url
	    mtfa-eis-get-current-uri
	    mtfa-eis-get-current-user-passwd
	    mtfa-eis-get-current-mac-src
	    mtfa-eis-get-current-ip-src
	    mtfa-eis-get-current-ip-dst
	    mtfa-eis-get-current-port-src
	    mtfa-eis-get-current-port-dst
	    mtfa-eis-get-current-multipart
	    mtfa-eis-get-current-ssl-servername
	    mtfa-eis-get-ip-network-protocol
	    mtfa-eis-set-ip-real-data-size
	    mtfa-eis-get-ip-data-len
	    mtfa-eis-get-ip-id-probe
	    mtfa-eis-get-raw-data
    	    mtfa-eis-put-raw-data
	    mtfa-eis-enable-pbuf
	    ))

(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3.so" "init_mtfa_fs3")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_db_mysql.so" "init_mtfa_db_mysql")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_ph.so" "init_mtfa_ph")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_lib.so" "init_mtfa_lib")

;;for historical reasons
(define-public mtfa-eis-get-value-current_headers mtfa-eis-get-value-current-headers)

;;Inizializzazione delle strutture di puntamento e selezione.
(define eis::function-pointer (mtfa-fs3-make))
(define-public (eis::function-pointer-add k v)
  (mtfa-fs3-add eis::function-pointer k v))
(define-public (eis::function-pointer-get k)
  (mtfa-fs3-get eis::function-pointer k))

;;;
;;;Un mutex globale per le chiamate
;;;
(define DoYourJob-mutex (make-mutex))
;;;
;;La nuova funzione senza parametri
;;;
;; (defun DoYourJob-nopar-internal (actions pbufinout)
;;   (let* ((actionl (eis::GetActionsList actions))
;; 	 (action (cadr actionl))
;; 	 (p-function (if (nil? action) #nil (mtfa-fs3-get eis::function-pointer action))))
;;     (if (nil? p-function)
;; 	(eis::BaseLib::BlockSession)
;; 	(p-function actionl pbufinout))))
(defun DoYourJob-nopar-internal (actionl pbufinout)
  ;;(with-mutex DoYourJob-mutex
   (let* ((action (car actionl))
	  (p-function (if (nil? action) #nil (eis::function-pointer-get action))))
     (if (nil? p-function)
	 (eis::BaseLib::BlockSession)
	 (p-function (cdr actionl) pbufinout)))) ;;)

;;;In questo modo tutti gli errori sono catched
(defun DoYourJob-nopar (actions pbufinout)
  (mtfa-noerr
   (eis::BaseLib::BlockSession)
   ;;(list "A" "SFRUUC8xLjEgNDAxIFVuYXV0aG9yaXplZA0KQ29udGVudC1MZW5ndGg6IDIyNg0KQ29udGVudC10eXBlOiB0ZXh0L2h0bWw7IGNoYXJzZXQ9dXRmLTgNCkNvbm5lY3Rpb246IGNsb3NlDQoNCjwhRE9DVFlQRSBodG1sPjxodG1sPjxoZWFkPjxtZXRhIGNoYXJzZXQ9InV0Zi04Ij48dGl0bGU+Tm90IGF1dGhvcml6ZWQ8L3RpdGxlPjwvaGVhZD48Ym9keT48ZGl2IGFsaWduPSdjZW50ZXInPjxoMT48L2JyPjwvYnI+Tm90IEF1dGhvcml6ZWQ8L2gxPjwvZGl2PjwvYnI+PC9icj48ZGl2IGFsaWduPSdjZW50ZXInPjwvZGl2PjxkaXYgYWxpZ249ImNlbnRlciI+PC9kaXY+PC9ib2R5PjwvaHRtbD4=")
   (DoYourJob-nopar-internal actions pbufinout)
   ))

;; )


;;;FUNZIONI DI RITORNO verso eis
(defun eis::BaseLib::PassToServer (ip_dest port_dest host method headers url body)
  (begin
    ;; (display "Error in PassToServer!!!") (newline)
    (eis::GiveErrorHTTP401)) => 
    ;;Se in headers c'è Host, lo toglie
    (when (and (not (StringIsEmptyOrNil headers))
               (not (StringIsEmptyOrNil url)))
      (when (not (StringIsEmptyOrNil host))
	(set! headers (ReplaceAll headers "[hH][oO][sS][tT] *: *.*\\r{0,1}\\n{0,1}" (string-append "Host: " host "\r\n"))))
      ;; (set! headers (mtfa-replace-all "[hH][oO][sS][tT] *: *.*\\r{0,1}\\n{0,1}" headers
      ;; 				(string-append "Host: " host "\r\n") #f 0)))
      (set! headers (string-append method " " url " HTTP/1.1\r\n" headers)))
    ;; (for-each display (list "PassToServer: <" ip_dest "> <" port_dest "> <" host "> <" method "> <" headers "> <" url "> <" body ">" ))
    ;; (newline)
    (list "C" 
          ip_dest
          port_dest
          url
          headers
          host
          body
          method))

;; (define (eis::BaseLib::ExportAnswer answer)
;;   ;;(display (list "A" answer "\n"))
;;   (list "A" (mtfa-b64-encode answer))
;;   )

;; (define (eis::BaseLib::ExportAnswerAndKeepAlive answer)
;;   (list "B" (mtfa-b64-encode answer))
;;   )

(define-public (eis::BaseLib::ContinueDoNotClose)
  (list "N"))

(define-public (eis::BaseLib::Close)
  (list "O"))

(define (eis::BaseLib::ContinueAfterAccept)
  (list "T"))

(define (eis::BaseLib::NeedMoreData)
  (list "M"))

(define (eis::BaseLib::BlockSession . other)
  (eis::GiveErrorHTTP401 #t))

(define (eis::GiveAnswerAndCloseAll answer)
  ;; (Show "eis::GiveAnswerAndCloseAll:A: " answer)
  (list "A" (mtfa-b64-encode answer)))

(define (eis::GiveAnswerAndNotClose answer)
  ;; (Show "eis::GiveAnswerAndCloseAll:B: " answer)
  (list "B" (mtfa-b64-encode answer)))

(define* (eis::Redirect where #:optional (headers ""))
  (list "R" (string-append (mtfa-b64-encode where) ";" (mtfa-b64-encode headers))))

(define (eis::GiveFileAndClose http-answer headers filename)
  (list "g" (string-append (mtfa-b64-encode filename) ";" (mtfa-b64-encode headers) ";" (mtfa-b64-encode http-answer))))

(define (eis::GiveFileAndNotClose http-answer headers filename)
  (list "G" (string-append (mtfa-b64-encode filename) ";" (mtfa-b64-encode headers) ";" (mtfa-b64-encode http-answer))))

;;;FUNZIONI DI PREPARAZIONE DELLE RISPOSTE HTTP
(define eis::http-content-type-html "Content-Type: text/html; charset=utf-8")
(define eis::http-content-type-text "Content-Type: text/plain; charset=UTF-8")
(define eis::http-content-type-b64  "Content-Type: text/plain; charset=UTF-8\r\nContent-transfer-encoding: base64")
(define eis::http-content-type-xml "Content-Type: text/xml; charset=utf-8")
(define eis::http-content-type-json "Content-Type: application/json; charset=utf-8")
(define eis::http-content-type-javascript "Content-Type: application/javascript; charset=utf-8")
(define-public eis::http-content-type-css "Content-Type: text/css; charset=utf-8")

(define eis::http-answer-ok "HTTP/1.1 200 OK")

(define (eis::http-content-length l)
  (string-append "Content-Length: " (if (number? l) (number->string l) l)))

(define eis::http-connection-close "Connection: close")
(define eis::http-connection-keep-alive "Connection: keep-alive")
(define eis::http-eoln "\r\n")
(define eis::http-end-of-headers "\r\n\r\n")

(define (eis::BaseLib::BuildHTTPAnswer http-answer headers cookies body)
  ;;se il body è in bytevector, allora converto anche il resto in bytevector
  (if (bytevector? body)
      (begin
	(let* ((all-but-body (mtfa-string->bytevector 
			      (string-append 
			       http-answer 
			       eis::http-eoln
			       ;;Headers
			       (if (and (> (string-length headers) 0) (not (equal? "\n" (string-take-right headers 1))))
				   (string-append headers eis::http-eoln)
				   headers)
			       ;;SetCookies
			       (if (and (> (string-length cookies) 0) (not (equal? "\n" (string-take-right cookies 1))))
				   (string-append cookies eis::http-eoln)
				   cookies)
			       (eis::http-content-length (bytevector-length body))
			       eis::http-end-of-headers)))
	       (buf (make-bytevector (+ (bytevector-length body) (bytevector-length all-but-body)))))
          (bytevector-copy! all-but-body 0 buf 0 (bytevector-length all-but-body))
          (bytevector-copy! body 0 buf (bytevector-length all-but-body) (bytevector-length body))
	  buf))
      (begin
	(string-append http-answer eis::http-eoln
		       ;;Headers
		       (if (and (> (string-length headers) 0) (not (equal? "\n" (string-take-right headers 1))))
			   (string-append headers eis::http-eoln)
			   headers)
		       ;;SetCookies
		       (if (and (> (string-length cookies) 0) (not (equal? "\n" (string-take-right cookies 1))))
			   (string-append cookies eis::http-eoln)
			   cookies)
		       (eis::http-content-length (string-length body))
		       eis::http-end-of-headers
		       body)
	)
      )
  )

(define* (eis::GiveHTTPAnswer http-answer headers cookies body #:optional (doclose #t))
  (if doclose
      (eis::GiveAnswerAndCloseAll (eis::BaseLib::BuildHTTPAnswer http-answer headers cookies body))
      (eis::GiveAnswerAndNotClose (eis::BaseLib::BuildHTTPAnswer http-answer headers cookies body))))

(define* (eis::GiveHTTPHtmlAnswer page #:optional (doclose #t) #:key
				  (answer eis::http-answer-ok)
				  (headers (string-append eis::http-content-type-html eis::http-eoln eis::http-connection-close eis::http-eoln))
				  (cookies "")
				  (zip #f)
				  )
  (if doclose
      (eis::GiveAnswerAndCloseAll (eis::BaseLib::BuildHTTPAnswer answer
								 (if zip (string-append
									  "Content-Encoding: gzip\r\n" headers)
								     headers)
								 cookies
								 (if zip (mtfa-http-compress page 6) page)))
      (eis::GiveAnswerAndNotClose (eis::BaseLib::BuildHTTPAnswer answer
								 (if zip (string-append
									  "Content-Encoding: gzip\r\n" headers)
								     headers)
								 cookies
								 (if zip (mtfa-http-compress page 6) page)))))
(define* (eis::GiveHTTPXmlAnswer page  #:optional (doclose #t))
  (if doclose
      (eis::GiveAnswerAndCloseAll (eis::BaseLib::BuildHTTPAnswer eis::http-answer-ok
								 (string-append eis::http-content-type-xml eis::http-eoln
										eis::http-connection-close eis::http-eoln)
								 ""
								 page))
      (eis::GiveAnswerAndNotClose (eis::BaseLib::BuildHTTPAnswer eis::http-answer-ok
								 (string-append eis::http-content-type-xml eis::http-eoln
										eis::http-connection-keep-alive eis::http-eoln)
								 ""
								 page))))
(define* (eis::GiveHTTPTextAnswer page  #:optional (doclose #t))
  (if doclose
      (eis::GiveAnswerAndCloseAll (eis::BaseLib::BuildHTTPAnswer eis::http-answer-ok
								 (string-append eis::http-content-type-text eis::http-eoln
										eis::http-connection-close eis::http-eoln)
								 ""
								 page))
      (eis::GiveAnswerAndNotClose (eis::BaseLib::BuildHTTPAnswer eis::http-answer-ok
								 (string-append eis::http-content-type-text eis::http-eoln
										eis::http-connection-keep-alive eis::http-eoln)
								 ""
								 page))))

(define* (eis::GiveHTTPB64Answer page  #:optional (doclose #t))
  (if doclose
      (eis::GiveAnswerAndCloseAll (eis::BaseLib::BuildHTTPAnswer eis::http-answer-ok
								 (string-append eis::http-content-type-b64 eis::http-eoln
										eis::http-connection-close eis::http-eoln)
								 ""
								 page))
      (eis::GiveAnswerAndNotClose (eis::BaseLib::BuildHTTPAnswer eis::http-answer-ok
								 (string-append eis::http-content-type-b64 eis::http-eoln
										eis::http-connection-keep-alive eis::http-eoln)
								 ""
								 page))))

(define* (eis::GiveHTTPJSONAnswer data #:optional (doclose #t))
  (if doclose
      (eis::GiveAnswerAndCloseAll (eis::BaseLib::BuildHTTPAnswer eis::http-answer-ok
								 (string-append eis::http-content-type-json eis::http-eoln
										eis::http-connection-close eis::http-eoln
										"Access-Control-Allow-Origin: *" eis::http-eoln
										)
								 ""
								 (scm->json-string data)))
      (eis::GiveAnswerAndNotClose (eis::BaseLib::BuildHTTPAnswer eis::http-answer-ok
								 (string-append eis::http-content-type-json eis::http-eoln
										eis::http-connection-keep-alive eis::http-eoln
										"Access-Control-Allow-Origin: *" eis::http-eoln
										)
								 ""
								 (scm->json-string data)))))

(define data-for-error-401-404
  "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Not authorized</title></head><body><div align='center'><h1></br></br>Not Authorized</h1></div></br></br><div align='center'></div><div align=\"center\"></div></body></html>")

(define (data-for-error-500 msg)
  (string-append
   "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Internal Server Error</title></head><body><div align='center'><h1></br></br>Internal Server Error</h1></div></br></br><div align='center'><h2 class=\"font-bold\">"
   msg
   "</h2></div><div align=\"center\"></div></body></html>"))

(define* (eis::GiveErrorHTTP404  #:optional (doclose #t))
  (if doclose
      (eis::GiveAnswerAndCloseAll
       (string-append
	"HTTP/1.1 404 Not Found\r\n"
	"Content-Length: " (number->string (string-length data-for-error-401-404)) "\r\n"
	"Content-type: text/html; charset=utf-8\r\n"
	"Connection: close\r\n\r\n"
	data-for-error-401-404
	))
      (eis::GiveAnswerAndNotClose
       (string-append
	"HTTP/1.1 404 Not Found\r\n"
	"Content-Length: " (number->string (string-length data-for-error-401-404)) "\r\n"
	"Content-type: text/html; charset=utf-8\r\n"
	"Connection: keep-alive\r\n\r\n"
	data-for-error-401-404))))


(define* (eis::GiveErrorHTTP401  #:optional (doclose #t))
  (if doclose
      (eis::GiveAnswerAndCloseAll
       (string-append
	"HTTP/1.1 401 Unauthorized\r\n"
	"Content-Length: " (number->string (string-length data-for-error-401-404)) "\r\n"
	"Content-type: text/html; charset=utf-8\r\n"
	"Connection: close\r\n\r\n"
	data-for-error-401-404
	))
      (eis::GiveAnswerAndNotClose
       (string-append
	"HTTP/1.1 401 Unauthorized\r\n"
	"Content-Length: " (number->string (string-length data-for-error-401-404)) "\r\n"
	"Content-type: text/html; charset=utf-8\r\n"
	"Connection: keep-alive\r\n\r\n"
	data-for-error-401-404))))

(define* (eis::GiveErrorHTTP500  #:optional (msg "The request cannot be satisfied due to an internal server error.") (doclose #t))
  (if doclose
      (eis::GiveAnswerAndCloseAll
       (string-append
	"HTTP/1.1 500 Internal Server Error\r\n"
	"Content-Length: " (number->string (string-length (data-for-error-500 msg))) "\r\n"
	"Content-type: text/html; charset=utf-8\r\n"
	"Connection: close\r\n\r\n"
	(data-for-error-500 msg)
	))
      (eis::GiveAnswerAndNotClose
       (string-append
	"HTTP/1.1 500 Internal Server Error\r\n"
	"Content-Length: " (number->string (string-length (data-for-error-500 msg))) "\r\n"
	"Content-type: text/html; charset=utf-8\r\n"
	"Connection: keep-alive\r\n\r\n"
	(data-for-error-500 msg)))))

(define (eis::LoadPage page LbUser_Root_Web_Site)
  (let (
	(s-page (fs-io-to-string (string-append LbUser_Root_Web_Site page))))
    (if (StringIsEmptyOrNil s-page)
	#nil
	s-page)))


;; (define (eis::GetActionsList actions)
;;   (let* (
;; 	(actionl0 (if (string? actions) 
;; 		      (string-split actions #\;) 
;; 		      #nil))
;; 	(actionl1 (if (nil? actionl0) 
;; 		      #nil
;; 		      (cons (car actionl0) 
;; 			    (map (lambda (it)
;; 				   (mtfa-noerr
;; 				    #nil
;; 				    (mtfa-b64-decode-s it)
;; 				    ))
;; 				 (cdr actionl0))))))
;;     actionl1))

(define (eis::SplitPostData post-data)
  (let* (
	 ;;(dummy (pretty-print (list "POST DATA: " post-data)))
	 (l (if (string? post-data) (string-split post-data #\&) #nil))
	 (l (if (nil? l) 
		#nil
		(let (
		      (alist #nil))
		  (map 
		   (lambda (it) 
		     (let ((pars (string-split it #\=)))
		       (set! alist (acons (car pars) (uri-decode (cadr pars)) alist))))
		   l)alist))))
    l))

(defun mtfa-eis-content-type-for-unknown-file (filename)
  (£ "Content-Type: application/octet-stream") => ;;the most general one
 (let ((content-type (mtfa-mime-from-extension filename))
       (charset (mtfa-charset-detect filename)))
   
   (if (StringIsEmptyOrNil content-type)
       ""
       (if (StringIsEmptyOrNil charset)
           (£ "Content-Type: " content-type)
           (£ "Content-Type: " content-type "; charset=" charset)))))
(export mtfa-eis-content-type-for-unknown-file)
;;
;;Questo è un buon risponditore per file e altro
(define* (eis::GiveHttpMimeAnswer http-answer headers cookies body #:optional (doclose #t))
  (mtfa-noerr
   (begin
     (eis::GiveErrorHTTP401))
   (let ((body (mtfa-string->bytevector body))
         (all-but-body "")
         (content-type "")
         (charset "")
         )
     ;;
     ;;C'è il fine linea nella prima riga dell'header (http-answer)
     (when (not (equal? "\n" (string-take-right http-answer 1)))
       (set! http-answer (£ http-answer eis::http-eoln)))
     ;;
     ;;metto a posto eventuale fine linea dei cookies, se ci sono cookies
     (when (and cookies (not (StringIsEmptyOrNil cookies)) (not (equal? "\n" (string-take-right cookies 1))))
       (set! cookies (£ cookies eis::http-eoln)))
     ;;
     ;;lo stesso per gli headers
     (when (and headers (not (StringIsEmptyOrNil headers)) (not (equal? "\n" (string-take-right headers 1))))
       (set! headers (string-append headers eis::http-eoln)))
     ;;ora cookie e headers o sono vuoti, e quindi non influenzano, oppure terminano per \r\n
     ;;
     ;;Aggiungo content-length (anche il valore 0 va bene) agli headers, a meno che non ci sia già
     (when (not (FindPattern "[Cc]ontent-[Ll]ength: " headers))
       (set! headers (£ headers (eis::http-content-length (bytevector-length body)) eis::http-eoln))
       )
     ;;
     ;;Se body è presente, e se content-type non è presente, aggiungo content-type agli headers
     (set! content-type (FindPattern "Content-Type: ([^/]*)/" headers))
     (when (and body (not content-type))
       (set! content-type (mtfa-magic-mime body))
       (set! charset (if (FindPattern "text/" content-type)
                         (£ "; charset=" (mtfa-charset-detect body))
                         ""))
       (set! headers (£ headers "Content-Type: " content-type charset eis::http-eoln))
       )
     ;;
     ;;Termino, per semplicità, con close o no
     (if doclose
         (set! headers (£ headers eis::http-connection-close eis::http-eoln))
         (set! headers (£ headers eis::http-connection-keep-alive eis::http-eoln)))
     ;;
     ;;Costruisco la risposta completa
     (set! all-but-body (mtfa-string->bytevector (£ http-answer headers cookies eis::http-eoln)))
     ;;
     ;;Appendo il body e ritorno indietro i dati b64 al codice C chiamante
     (if (and body (> (bytevector-length body) 0))
         (begin
	   (let ((buf (make-bytevector (+ (bytevector-length body) (bytevector-length all-but-body)))))
            (bytevector-copy! all-but-body 0 buf 0 (bytevector-length all-but-body))
            (bytevector-copy! body 0 buf (bytevector-length all-but-body) (bytevector-length body))
            (begin
	      (list (if doclose "A" "B") (mtfa-b64-encode buf)))))
         (begin
	   (list (if doclose "A" "B") (mtfa-b64-encode all-but-body)))))))
(export eis::GiveHttpMimeAnswer)
