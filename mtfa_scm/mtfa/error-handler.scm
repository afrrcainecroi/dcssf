(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa error-handler)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
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
  #:use-module (ice-9 control)
  #:use-module (ice-9 exceptions)
  #:use-module (sxml simple)
  #:use-module (json)
  ;;#:use-module (mtfa utils)
  #:export (
		try-catch
		mtfa-noerr
	    ))

(define-syntax try-catch-new   ;;a volte non funziona la display-backtrace!!!
  (syntax-rules ()
    ((_ __error-return exp ...)
     (let/ec cancel
       (with-exception-handler
           (lambda (err)
             (display-backtrace (make-stack #t) (current-output-port) 3 1)
	     ;;(display "kind: ") (display (exception-kind err)) (newline)
	     ;;(display "origin: ") (display (exception-origin err)) (newline)
	     ;;(display "message: ") (display (exception-message err)) (newline)
	     ;;(display "Format: ") (simple-format #f (exception-message err) (cddr (exception-args err))) (newline)
	     ;;(display (exception-args err)) (newline)
	     (let* ((args (third (exception-args err)))
		    (exmsg (exception-message err))
		    (num-args (string-count exmsg #\~))
		    (msg (cond
			  ((zero? num-args) (simple-format #f exmsg))
			  ((= 1 num-args) (simple-format #f exmsg (first args)))
			  ((= 2 num-args) (simple-format #f exmsg (first args) (second args)))
			  ((= 3 num-args) (simple-format #f exmsg (first args) (second args) (third args)))
			  ((= 4 num-args) (simple-format #f exmsg (first args) (second args) (third args) (fourth args)))
			  ((= 5 num-args) (simple-format #f exmsg (first args) (second args) (third args) (fourth args) (fifth args)))
			  ((= 6 num-args) (simple-format #f exmsg (first args) (second args) (third args) (fourth args) (fifth args) (sixth args)))
			  ((= 7 num-args) (simple-format #f exmsg (first args) (second args) (third args) (fourth args) (fifth args) (sixth args) (seventh args)))
			  ((= 8 num-args) (simple-format #f exmsg (first args) (second args) (third args) (fourth args) (fifth args) (sixth args) (seventh args) (eighth args)))
			  (#t (simple-format #f exmsg (first args) (second args) (third args) (fourth args) (fifth args) (sixth args) (seventh args) (eighth args) (ninth args)))
			  )))
               (display "Exception Kind  : " (exception-kind err)) (newline)
               (display "Exception Origin: " (exception-origin err)) (newline)
               (display "Message         : " msg) (newline))
             (cancel __error-return))
         (lambda () exp ...))))))

(define-syntax mtfa-noerr
  (syntax-rules ()
    ((_ __error-return exp ...)
     (let/ec cancel
       (with-exception-handler
           (lambda (err)
             (cancel __error-return))
         (lambda () exp ...))))))

;; (define-syntax-rule (mtfa-noerr error-return exp ...)   ;;a volte non funziona la display-backtrace??? Verificare
;;   (with-exception-handler
;;       (lambda (err)
;; 	error-return)
;;     (lambda () exp ...) #:unwind? #t))



(define-syntax try-catch
  (syntax-rules ()
    ((try-catch __error-return exp ...)
     (let
	 ((__st #f))
       (catch #t
	 (lambda() exp ...)
	 (lambda (k . p)
	   (display "Catched Error: " (current-output-port))
	   (display k (current-output-port))
	   (display " . " (current-output-port))
	   (display p (current-output-port))
	   (newline (current-output-port))
	   __error-return
	   )
	 (lambda (k . p)
	   (set! __st (make-stack #t))
	   (when __st (display-backtrace __st (current-output-port)))))))))

;; (define-syntax mtfa-noerr
;;   (syntax-rules ()
;;     ((_ __error-return exp ...)
;;      (let
;; 	 ((__st #f))
;;        (catch #t
;; 	 (lambda() exp ...)
;; 	 (lambda (k . p) __error-return)
;; 	 )))))

