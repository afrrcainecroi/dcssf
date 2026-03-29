;; onvif-client.scm. To be used only with the onvif python service
;;
(define-module (mtfa onvif-client)
  #:use-module (web client)
  #:use-module (web response)	     ; per response-code/response-port
  #:use-module (json)
  #:use-module (rnrs bytevectors)
  #:use-module (rnrs io ports)
  #:use-module (ice-9 hash-table)
  #:export (onvif-connect
            ;;onvif-move
            onvif-stop
            onvif-absolute
            ;;onvif-relative
            ;;onvif-presets
            ;;onvif-goto-preset
	    ))

(define (bv->utf8 bv) (utf8->string bv))

(define (%post-json url alist)  ; alist with STRING keys
  (let* ((json-text (scm->json-string alist))        ; -> "{\"host\":\"...\",\"port\":80,...}"
         (body-bytes (string->utf8 json-text))       ; send bytes, not a Scheme string literal
         (resp (http-post url
                          #:headers '((Content-Type . "application/json"))
                          #:body body-bytes
                          #:streaming? #t))
         (code (response-code resp))
         (txt  (bv->utf8 (get-bytevector-all (response-port resp)))))
    (if (and (>= code 200) (< code 300))
        (if (or (not txt) (string-null? txt)) '() (json-string->scm txt))
        (error "HTTP error" code url txt))))

(define (%get-json url)
  (let* ((resp (http-get url #:streaming? #t))
         (code (response-code resp))
         (txt  (bv->utf8 (get-bytevector-all (response-port resp)))))
    (if (and (>= code 200) (< code 300))
        (json-string->scm txt)
        (error "HTTP error" code url txt))))

;; --- API calls ---
(define (onvif-connect base host port user pass)
  (%post-json (string-append base "/connect")
              `(("host" . ,host) ("port" . ,port) ("user" . ,user) ("password" . ,pass))))

;; (define (onvif-move base pan tilt zoom duration)
;;   (%post-json (string-append base "/ptz/move")
;;               `(("pan" . ,pan) ("tilt" . ,tilt) ("zoom" . ,zoom) ("duration" . ,duration))))

(define (onvif-absolute base pan tilt zoom duration)
  (%post-json (string-append base "/ptz/absolute")
              `(("pan" . ,pan) ("tilt" . ,tilt) ("zoom" . ,zoom) ("duration" . ,duration))))

(define (onvif-stop base)
  (%post-json (string-append base "/ptz/stop") '()))

