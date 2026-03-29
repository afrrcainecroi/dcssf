(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa utils)
  ;;I miei moduli
  #:use-module (mtfa error-handler)
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
  #:use-module (container async-queue)

  #:use-module (sxml simple)
  #:use-module (json)
  #:use-module (oop goops)
  #:use-module (oop goops describe)
  #:use-module (ice-9 optargs)
  #:use-module (ice-9 match)

  #:export (
  	    ;; <mtfa-pfs3>
	    ;; mtfa-pfs3::restore
	    ;; mtfa-pfs3::store
	    ;; mtfa-pfs3::del
	    ;; mtfa-pfs3::for-each
	    ;; mtfa-pfs3::update
	    ;; mtfa-pfs3::add
	    
	    mtfa-nop
	    mtfa-serialize-simple-item
	    mtfa-deserialize-simple-item

	    mtfa-mpz-probab-prime-p
     	    mtfa-mpz-nextprime
     	    mtfa-mpz-gcdext
     	    mtfa-mpz-invert
     	    mtfa-mpz-factorize
	    mtfa-chinese-remainder 

	    ;; collect
	    ;; repeat
	    first-n
	    nth-cdr
	    nth-item
	    mtfa-http-get-page
	    file-exists
	    fs-io-to-bv
	    fs-io-from-bv 
	    fs-io-to-string
	    fs-io-to-list
	    fs-io-to-vector
	    fs-io-from-string
	    crlf-to-lf
	    ;; string-to-b64
	    ;; string-from-b64
	    list-to-string
	    mtfa-howmany-files
	    mtfa-rand-mat-range
	    mtfa-rand-range
	    congruential-generator-init
	    congruential-generator-next
	    congruential-generator-empty
	    mtfa-permute
	    parse-xml
	    OpenDb
	    CloseDb
	    BuildQueryStringFromList
	    SelectDb
	    InsertDb
	    SqlDb

	    mtfa-fs3-make
	    mtfa-fs3-add
	    mtfa-fs3-get
	    mtfa-fs3-update
	    mtfa-fs3-serialize
	    mtfa-fs3-deserialize
	    mtfa-fs3?
	    mtfa-fs3-get-all
	    mtfa-fs3-put-all
	    mtfa-fs3-for-each

	    mtfa-fs3s-make
	    mtfa-fs3s-add
	    mtfa-fs3s-get
	    mtfa-fs3s-update
	    mtfa-fs3s-serialize
	    mtfa-fs3s-deserialize
	    mtfa-fs3s?
	    mtfa-fs3s-get-all

	    mtfa-ph-make
	    mtfa-ph-add
	    mtfa-ph-get
	    mtfa-ph-get-val
	    mtfa-ph-get-all
	    mtfa-ph-for-each
	    mtfa-ph-serialize
	    mtfa-ph-deserialize
	    mtfa-ph?

	    mtfa-su-make
	    mtfa-su-reset
	    mtfa-su-size
	    mtfa-su-count
	    mtfa-su-set-all
	    mtfa-su-flip-all
	    mtfa-su-set
	    mtfa-su-flip
	    mtfa-su-get
	    mtfa-su-assign
	    mtfa-su-compare
	    mtfa-su-or
	    mtfa-su-or-equal
	    mtfa-su-and
	    mtfa-su-and-equal
	    mtfa-su-xor
	    mtfa-su-xor-equal
	    mtfa-su-diff
	    mtfa-su-diff-equal
	    mtfa-su-get-all
	    mtfa-su-to-string
	    mtfa-su-serialize
	    mtfa-su-deserialize
	    mtfa-su-resize
	    mtfa-su-clone
	    mtfa-su?

	    mtfa-db-mysql-open
	    mtfa-db-mysql-connect
	    mtfa-db-mysql-close
	    mtfa-db-mysql-disconnect
	    mtfa-db-mysql-start-transaction
	    mtfa-db-mysql-rollback
	    mtfa-db-mysql-commit
	    mtfa-db-mysql-do-sql
	    mtfa-db-mysql-last-id 
	    mtfa-db-mysql-p

	    mtfa-rand
	    mtfa-rand-d
	    mtfa-rand-ui
	    mtfa-rand-seed
	    mtfa-rand-mat
	    mtfa-rand-mat-ui
	    mtfa-rand-mat-d
	    mtfa-rand-mat-seed
	    mtfa-cypher
	    mtfa-http-parse
	    mtfa-b64-encode
	    mtfa-b64-decode-s
	    mtfa-b64-decode-bv
	    mtfa-b64-url-encode
	    mtfa-b64-url-decode-s
	    mtfa-b64-url-decode-bv
	    mtfa-run-ext-prog
	    mtfa-rand-string

	    mtfa-http-get-page
	    mtfa-http-get-page-ex
	    mtfa-http-decode-html-page
	    mtfa-rand-alfanum
	    mtfa-send-mail
	    mtfa-send-mail-prepare-annexes

;;	    mtfa-netfilter-queue-start
;;	    mtfa-netfilter-queue-stop
;;	    mtfa-netfilter-queue?
;;	    mtfa-netfilter-queue-add
;;	    mtfa-netfilter-queue-del
;;	    mtfa-netfilter-queue-list

	    mtfa-run-generator-string
	    mtfa-run-generator-bv
	    mtfa-cypher-generator
	    mtfa-base62
	    mtfa-debase62

	    StringIsEmptyOrNil
	    XmlEncode
	    XmlDecode
	    BuildXmlFlat
	    TimeStamp
	    mtfa-micros
	    ReplaceAll
	    ReplaceAllMultiple
	    GetValueFromPairsList
	    CompilePattern
	    FindPattern
	    FindCompiledPattern
	    Show
	    Show!
	    mtfa-dispatcher::make
	    mtfa-dispatcher::call
	    mtfa-dispatcher::add
	    mtfa-dispatcher::del
	    mtfa-sqlite3-do
	    mtfa-sqlite3-open
	    mtfa-sqlite3-close
	    mtfa-sqlite3-execute

	    PeriodicalTasks::Run
	    PeriodicalTasks::Stop
	    PeriodicalTasks::remove
	    PeriodicalTasks::add
	    PeriodicalTasks::GetAll

	    mtfa-strong-random
	    mtfa-make-rsa
	    mtfa-rsa-pub-cipher
	    mtfa-rsa-pub-decipher
     	    mtfa-rsa-priv-cipher
     	    mtfa-rsa-priv-decipher
     	    mtfa-rsa-sign
     	    mtfa-rsa-verify
     	    mtfa-rsa-make-key
     	    mtfa-rsa-make-key-p-q
     	    mtfa-rsa-get-pub-key
     	    mtfa-rsa-get-priv-key
     	    mtfa-rsa-set-pub-key
     	    mtfa-rsa-set-priv-key

	    mtfa-hash
	    mtfa-hmac
	    mtfa-hash-hs
	    mtfa-hmac-hs
	    mtfa-hash-b64
	    mtfa-hmac-b64

	    mtfa-string-split-size
	    mtfa-string-split-regex

	    mtfa-bv-to-hex
	    mtfa-bv-to-hex-string
	    mtfa-hex-string-to-bv

	    mtfa-cifra-aes-js-hex 
	    mtfa-decifra-aes-js-hex-s
	    mtfa-decifra-aes-js-hex-bv
	    mtfa-cifra-aes-js
	    mtfa-decifra-aes-js
	    mtfa-cifra-aes-js-b64 
	    mtfa-decifra-aes-js-b64-s 
	    mtfa-decifra-aes-js-b64-bv 

	    mtfa-bv-xor
	    mtfa-bv-or
	    mtfa-bv-and

	    mtfa-base32-encode
	    mtfa-base32-encode-bv
	    mtfa-base32-encode-s
	    mtfa-base32-decode
	    mtfa-base32-decode-bv
	    mtfa-base32-decode-s
	    
	    mtfa-seconds
	    mtfa-left-pad
	    mtfa-totp-gen

	    mtfa-ecipher-make
	    mtfa-ecipher?
	    mtfa-ecipher-mask
	    mtfa-ecipher-cipher
	    mtfa-ecipher-decipher-s
	    mtfa-ecipher-decipher-bv
	    mtfa-ecipher-decipher-number
	    mtfa-ecipher-crc64

	    mtfa-star-make
	    mtfa-star?
	    mtfa-star-insert
	    mtfa-star-search
	    mtfa-star-build
	    mtfa-star-get-rules
	    mtfa-star-serialize
	    mtfa-star-deserialize

	    mtfa-get-current-thread

	    mtfa-nequal?
	    mtfa-assert
	    ifnot
	    mtfa-to-string

	    mtfa-compress
	    mtfa-uncompress

	    mtfa-http-compress
	    mtfa-http-uncompress

	    mtfa-cookie-make

	    mtfa-compile-pattern
	    mtfa-find-all-positions
	    mtfa-find-all-matching-positions
	    mtfa-replace-all

	    mtfa-blum-make
	    mtfa-blum-get-values
	    mtfa-blum-get-next
	    mtfa-blum-get-prev
	    mtfa-blum-get-next-ith
	    mtfa-blum-get-prev-ith
	    mtfa-blum-get-next-ith-slow
	    mtfa-blum-set-values

	    mtfa-bv-to-num
	    mtfa-num-to-bv
	    mtfa-blum-make-token
	    mtfa-blum-identify-token

	    mtfa-fuzzy-find
	    mtfa-magic-mime
	    mtfa-mime-from-extension
	    mtfa-charset-detect

	    mtfa-sss-make
	    mtfa-sss?
	    mtfa-sss-insert
	    mtfa-sss-search
	    mtfa-sss-unlink
	    mtfa-sss-getval
	    mtfa-sss-getall
	    mtfa-sss-getallid
	    mtfa-sss-ls2bv
	    mtfa-sss-serialize
	    mtfa-sss-deserialize

	    mtfa-hash-keccak
	    ;;mtfa-json-to-rlp
    	    ;;mtfa-rlp-to-json
	    ;;mtfa-scmjson-to-rlp::internal
	    mtfa-rlp-to-json::internal

	    mtfa-eth-make-priv-key
	    mtfa-eth-get-pub-key
	    mtfa-eth-get-pub-key-serialized
	    mtfa-eth-serialize-signature
	    mtfa-eth-sign-msg
	    
	    mtfa-morton-16-encode
	    mtfa-morton-16-decode
	    mtfa-morton-32-encode
	    mtfa-morton-32-decode

	    mtfa-make-cimg
	    mtfa-cimg-p
	    mtfa-cimg-set-space
	    mtfa-cimg-set-linewidth
	    mtfa-cimg-erase
	    mtfa-cimg-flush
	    mtfa-cimg-close
	    mtfa-cimg-open
	    mtfa-cimg-set-linecolor-string
	    mtfa-cimg-set-linecolor
	    mtfa-cimg-set-fillcolor-string
	    mtfa-cimg-set-fillcolor
	    mtfa-cimg-set-filltype
	    mtfa-cimg-set-bg-string
	    mtfa-cimg-set-bg
	    mtfa-cimg-draw-box
	    mtfa-cimg-draw-label
	    mtfa-cimg-move
	    mtfa-cimg-draw-circle
	    mtfa-cimg-draw-line
	    mtfa-cimg-draw-point
	    mtfa-cimg-font-name
	    mtfa-cimg-font-size
	    mtfa-cimg-text-angle
	    mtfa-cimg-image

	    ;;mtfa-make-gfx
	    ;;mtfa-gfx-close
	    ;;mtfa-gfx-p
	    ;;mtfa-gfx-move-window
	    ;;mtfa-gfx-point
	    ;;mtfa-gfx-line
	    ;;mtfa-gfx-fill-arc
	    ;;mtfa-gfx-arc
	    ;;mtfa-gfx-rect
	    ;;mtfa-gfx-fill-rect
	    ;;mtfa-gfx-string
	    ;;mtfa-gfx-color
	    ;;mtfa-gfx-clear-color
	    ;;mtfa-gfx-clear
	    ;;mtfa-gfx-flush
	    ;;mtfa-gfx-event-waiting
	    ;;mtfa-gfx-wait
	    ;;mtfa-gfx-idle
	    ;;mtfa-gfx-xpos
	    ;;mtfa-gfx-ypos

	    ;;onvif cameras
	    mtfa-onvif-make
	    mtfa-onvif?
	    mtfa-onvif-connect
	    mtfa-onvif-move
	    ))

(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3.so" "init_mtfa_fs3")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3s.so" "init_mtfa_fs3s")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_db_mysql.so" "init_mtfa_db_mysql")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_ph.so" "init_mtfa_ph")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_lib.so" "init_mtfa_lib")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_ws.so" "init_mtfa_wss")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_netfilter_queue.so" "init_mtfa_netfilter_queue")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_su.so" "init_mtfa_su")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_cipher.so" "init_mtfa_ecipher")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_star.so" "init_mtfa_star")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_string_sequences.so" "init_mtfa_sss")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_onvif.so" "init_mtfa_onvif")
;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_gfx.so" "init_mtfa_gfx")

;;(mtfa-rand-mat-seed 0) ;;;Inizializzo il seed, per le prove
;;(mtfa-rand-seed 0) ;;;Inizializzo il seed, per le prove

;;Abbiamo bisogno di un mutex per la show
(define Show::mutex (make-mutex))

(define-syntax define-syntax-public
  (syntax-rules ()
    ((define-syntax-public name . body)
     (begin
       (define-syntax name . body)
       (export-syntax name)))))

(export-syntax define-syntax-public)

;;Esempio: (string-case stringa ("Aaa" (Show! 1)) ("aaa" (Show! 2)) ("cia" (Show! 3)) (else (Show! 100)))
(define-syntax-public string-case
  (syntax-rules (else)
    ((_ str (else e1 ...))
     (begin e1 ...))
    ((_ str (e1 e2 ...))
     (when (string=? str e1) e2 ...))
    ((_ str (e1 e2 ...) c1 ...)
     (if (string=? str e1)
         (begin e2 ...)
         (string-case str c1 ...)))))

(define-syntax-public £
  (syntax-rules ()
    ((_ s1)
     s1)
    ((_ s1 s2 ...)
     (string-append s1 (£ s2 ...)))))

(define-syntax-public define-class-public
  (syntax-rules ()
    ((define-class-public name . body)
     (begin
       (define-class name . body)
       (export name)
       ))))

(define-syntax-public define-method-public
  (syntax-rules ()
    ((define-method-public (name . args) . body)
     (begin
       (define-method (name . args) . body)
       (export name)
       ))))

(define-syntax-public define-method*
  (lambda (x)
    (syntax-case x ()
      ((_ (generic arg-spec ... . tail) body ...)
       (receive (required-arg-specs other-arg-specs)
           (break (compose keyword? syntax->datum)
                  #'(arg-spec ...))
         #`(define-method (generic #,@required-arg-specs . rest)
             (apply (lambda* (#,@other-arg-specs . tail)
                      body ...)
                    rest)))))))
;;
(define-syntax-public define-method*-public
  (lambda (x)
    (syntax-case x ()
      ((_ (generic arg-spec ... . tail) body ...)
       (receive (required-arg-specs other-arg-specs)
           (break (compose keyword? syntax->datum)
                  #'(arg-spec ...))
         #`(define-method-public (generic #,@required-arg-specs . rest)
             (apply (lambda* (#,@other-arg-specs . tail)
                      body ...)
                    rest)))))))


(define-syntax-public repeat
  (syntax-rules ()
    ((repeat c e ...)
     (let loop ((n c))
       (if (zero? n)
	   #t 
	   (begin
	     (begin e ...)
	     (loop (1- n))))))))

(define-syntax-public collect
  (syntax-rules ()
    ((collect c e ...)
     (let loop ((n c))
       (if (zero? n)
	   '()
	   (cons (begin e ...) (loop (1- n))))))))

;;Ottima dichiarazione di funzione con return, tieni presente che deve essere la return anche all'uscita
(define-syntax-public return
  (lambda (x)
    (syntax-case x ()
      ((_ mvalues )
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
       #'(call-with-values (lambda () mvalues) xreturn))))))

(define-syntax-public defun
  (lambda (x)
    (syntax-case x (=>)
      ((_ name () ret-error => body ... )
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
         #'(define (name) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))
      ((_ name (p1) ret-error => body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define (name p1) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))
      ((_ name (p1 ...) ret-error => body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define (name p1 ...) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))

      ((_ name () body ... )
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
         #'(define (name) (call/ec (lambda (xreturn) body ...)))))
      ((_ name (p1) body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define (name p1) (call/ec (lambda (xreturn) body ...)))))
      ((_ name (p1 ...) body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define (name p1 ...) (call/ec (lambda (xreturn) body ...)))))

      )))

(define-syntax-public defun-public
  (lambda (x)
    (syntax-case x (=>)
      ((_ name () ret-error => body ... )
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
         #'(define-public (name) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))
      ((_ name (p1) ret-error => body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define-public (name p1) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))
      ((_ name (p1 ...) ret-error => body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define-public (name p1 ...) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))

      ((_ name () body ... )
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
         #'(define-public (name) (call/ec (lambda (xreturn) body ...)))))
      ((_ name (p1) body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define-public (name p1) (call/ec (lambda (xreturn) body ...)))))
      ((_ name (p1 ...) body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define-public (name p1 ...) (call/ec (lambda (xreturn) body ...)))))

      )))


(define-syntax-public defun*
  (lambda (x)
    (syntax-case x (=>)
      ((_ name () ret-error => body ... )
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
         #'(define* (name) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))
      ((_ name (p1) ret-error => body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define* (name p1) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))
      ((_ name (p1 ...) ret-error => body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define* (name p1 ...) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))

      ((_ name () body ... )
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
         #'(define* (name) (call/ec (lambda (xreturn) body ...)))))
      ((_ name (p1) body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define* (name p1) (call/ec (lambda (xreturn) body ...)))))
      ((_ name (p1 ...) body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define* (name p1 ...) (call/ec (lambda (xreturn) body ...)))))
      
      )))

(define-syntax-public defun*-public
  (lambda (x)
    (syntax-case x (=>)
      ((_ name () ret-error => body ... )
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
         #'(define*-public (name) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))
      ((_ name (p1) ret-error => body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define*-public (name p1) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))
      ((_ name (p1 ...) ret-error => body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define*-public (name p1 ...) (call/ec (lambda (xreturn) (mtfa-noerr ret-error body ...))))))

      ((_ name () body ... )
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
         #'(define*-public (name) (call/ec (lambda (xreturn) body ...)))))
      ((_ name (p1) body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define*-public (name p1) (call/ec (lambda (xreturn) body ...)))))
      ((_ name (p1 ...) body ...)
       (with-syntax ((xreturn (datum->syntax x 'xreturn)))
	 #'(define*-public (name p1 ...) (call/ec (lambda (xreturn) body ...)))))
      
      )))

(define-syntax-rule (!= op1 op2 ...)
  (not (equal? op1 op2 ...)))
(export !=)

(define-syntax-public document!
  (syntax-rules ()
    ((_ fun str) (set-procedure-property! fun 'documentation str))))

(define-syntax-public when-true
  (syntax-rules ()
    ((when-true condition exp ...)
     (when condition
         exp ...))))

(define-syntax-public when-false
  (syntax-rules ()
    ((when-false condition exp ...)
     (unless condition
       exp ...))))

;;Per altre utilities, tipo coroutines ecc, vedi https://github.com/shanecelis/emacsy.git

(define-public (Show!eol . l)
  (catch #t 
    (lambda()
      (with-mutex Show::mutex
	(for-each (lambda (it) (display it)) l)
	(force-output (current-output-port))
	)
      )
    (lambda (key . args)
      #t
      )))

(define (Show! . l)
  (catch #t 
    (lambda()
      (with-mutex Show::mutex
	(for-each (lambda (it) (display it)) l)
	(newline)
	(force-output (current-output-port))
	)
      )
    (lambda (key . args)
      #t
      )))


;;Questa è locale!
(define-syntax mtfa-catenate
  (lambda (x)
  (syntax-case x ()
    ((_ v1)        #'(cons 'v1 '()))
    ((_ v1 v2)     #'(list 'v1 'v2))
    ((_ v1 v2 ...) #'(cons 'v1 (mtfa-catenate v2 ...))))))

(define (Show . l)
  (catch #t 
    (lambda()
      (with-mutex Show::mutex
	(display (string-append (datetime:date->string (datetime:current-date)) " " ))
	(for-each (lambda (it) (display it)) l)
	(newline)
	(force-output (current-output-port))
	)
      )
    (lambda (key . args)
      #t
      )))

(define-syntax mtfa-nequal?
  (syntax-rules ()
    ((_ a ...)
     (not (equal? a ...)))))

(define-syntax-public not-nil?
  (syntax-rules ()
    ((_ expr)
     (not (null? expr)))))
(define-syntax-public not-null?
  (syntax-rules ()
    ((_ expr)
     (not (null? expr)))))

(define-syntax ifnot
  (syntax-rules ()
    ((_ condition then else)
     (if condition
	 else
	 then))))

(define-syntax mtfa-to-string
  (lambda (x)
    (syntax-case x ()
      ((_ id)
       #'(cond
          ((catch #t
	     (lambda()
	       (procedure? id))
	     (lambda (key . args)
	       #f)) (symbol->string 'id))
          ((symbol? 'id)
	   (catch #t 
	     (lambda()
	       (call-with-output-string (lambda (s) (display id s))))
	     (lambda (key . args)
	       (symbol->string 'id))))
	  ((number? 'id) (number->string 'id))
	  ((string? 'id)  'id)
	  (#t
	   (catch #t
	     (lambda ()
	       (call-with-output-string (lambda (s) (display id s))))
	     (lambda (k . a)
	       (call-with-output-string (lambda (s) (display 'id s)))))))))))

;;Finds a pattern in a string and returns it
(defun-public mtfa-get-pattern (str pat)
  (let ((found (mtfa-find-all-positions str pat 1)))
    (if found
        (substring str (caar found) (cdar found))
        #f)))
;;Gives the type of a scheme variable
(defun-public mtfa-typeof (v)
  (class-name (class-of v)))


(define-syntax mtfa-assert
  (lambda (x)
    (syntax-case x ()
      ((_ condition msg ...)
       #'(let* ((lista (mtfa-catenate msg ...))
	       (str (call-with-output-string
	       (lambda (s)
	 	 (let loop ((l lista))
	 	      (if (null? l)
	 		  '()
	 		  (begin
			    (if (and (symbol? (car l)) (defined? (car l)))
	 			(display (primitive-eval (car l)) s)
	 			(display (car l) s)
				)
	 		    (loop (cdr l)))))))))
	     (when (not condition) (throw 'assert str)))))))

(define-public (mtfa-set-subsets s)
  (if (null? s) (list '())
      (let [(restl (mtfa-set-subsets (cdr s)))]
        (append restl (map (λ (x) (cons (car s) x)) restl)))))

(define-public (mtfa-memoize f)
  (let ((table (make-hash-table)))
    (lambda (x)
      (let ((previously-computed-result (hash-ref table x)))
	(or previously-computed-result
	    (let ((result (f x))) (hash-set! table x result) result))))))

(define (StringIsNotNilOrEmpty v) (if (and (not (nil? v)) (not (null? v)) (string? v) (> (string-length v) 0)) #t #f))
(define (StringIsEmptyOrNil s   ) (if (or       (nil? s)       (null? s)  (not (string? s)) (= 0 (string-length s)) ) #t #f))


(define-syntax-rule (first-n l n)
  (if (> n (length l))
      (take l (length l))
      (take l n)
      )
  )

(define-syntax-rule (nth-cdr l n)
  (if (> n (length l))
      (drop l (length l))
      (drop l n)
      )
  )

(define-syntax-rule (nth-item l n)
  (if (null? l)
      '()
      (list-ref l n)))

(define (mtfa-http-get-page host port proto method url headers data proxy-name proxy-port timeout)
  (cadr (mtfa-http-get-page-ex host port proto method url headers data proxy-name proxy-port timeout))
  )

(define-syntax-rule (file-exists fn)
  (access? fn 0))

(define (fs-io-to-bv fn)
  (let ((data (if (file-exists fn)
		  (call-with-input-file fn get-bytevector-all)
		  '())))
    data))

(define (fs-io-from-bv fn bv) 
  (call-with-output-file fn 
    (lambda(p) (put-bytevector p bv))))


(define (fs-io-to-string fn)
  (if (file-exists fn)
      (call-with-input-file fn
    	(lambda (p)
	  (let loop ((one-line (read-line p 'concat))
		     (result '()))
	    (if (eof-object? one-line)
            	(apply string-append (reverse result))
	    	(loop (read-line p 'concat)
		      (cons one-line result))))))
      ""))


(define (fs-io-to-list fn)
  (if (file-exists fn)
      (call-with-input-file fn
    	(lambda (p)
	  (let loop (
		     (one-line (read-line p))
		     (result '())
		     )
	    (if (eof-object? one-line)
		(reverse result)
		(loop (read-line p) (cons one-line result))))))
      '()))

(define (fs-io-to-vector fn)
  (if (file-exists fn)
      (let* (
	     (lines (call-with-input-file fn
		      (lambda (p)
			(let loop (
				   (one-line (read-line p))
				   (result 0)
				   )
			  (if (eof-object? one-line)
			      result
			      (loop (read-line p) (1+ result)))))))
	     (vettore (make-vector lines))
	     )
	(call-with-input-file fn
	  (lambda (p)
	    (let loop (
		       (one-line (read-line p))
		       (line 0)
		       )
	      (if (eof-object? one-line)
		  vettore
		  (begin
		    (vector-set! vettore line one-line)
		    (loop (read-line p) (1+ line))))))))
      0
      ))

(define (fs-io-from-string fn str) (call-with-output-file fn (lambda(p) (put-string p str))))
(define-public (fs-io-from-list-of-string fn lofstr) (call-with-output-file fn (lambda(p) (for-each (lambda (s)
												 (put-string p s)
												 (newline p))
											       lofstr))))

(define (crlf-to-lf str)
  (regexp-substitute/global #f "\r\n" str 'pre "\n" 'post)
  )

(define (list-to-string l)
  (let (
	(res "")
	)
    (for-each
     (lambda (it)
       (cond 
	((number? it) (set! res (string-append res (number->string it))))
	((string? it) (set! res (string-append res it)))
	( #t (set! res (string-append res "--ERROR--")))
	)
       )
     l)
    res
    ))

;;Conta quanti file sono in una directory
(define (mtfa-howmany-files folder-name)
  (length (scandir folder-name (lambda(it) (if (or (equal? it ".") (equal? it "..")) #f #t)))))


(define (mtfa-rand-mat-range low high)
  (let (
        (range (- high low))
        )
    (+ low (euclidean-remainder (mtfa-rand-mat) range))
    )
  )

(define (mtfa-rand-range low high)
  (let (
        (range (- high low))
        )
    (+ low (euclidean-remainder (mtfa-rand) range))
    )
  )

(define (congruential-generator-init space)
  (make-bitvector space #f))

(define (congruential-generator-next cg)
  (define (set-bit pos cg)  ;;Mette a #t il primo #f da pos in poi in modo modulare
    (let ((found (bitvector-position cg #f pos )))
      (if (boolean? found)  ;;quindi se found=#f altrimenti è un numero
          (let ((found (bitvector-position cg #f 0)))
            (bitvector-set-bit! cg found)
            found)
          (begin
            (bitvector-set-bit! cg found)
            found))))
  (let ((pos (mtfa-rand-mat-range 0 (bitvector-length cg))))
    (if (zero? (bitvector-count cg))
	(bitvector-set-bit! cg pos)
        (set! pos (set-bit pos cg)))
    pos))

(define (congruential-generator-empty cg)
  (zero? (bitvector-count cg))
  )

(define (mtfa-permute l)
  (let*
      ((v (list->vector l))
       (i 0)
       (len (vector-length v))
       )
    (while (not (= i len))
      (vector-swap! v i (remainder (mtfa-rand-mat) len))
      (set! i (1+ i)))
    (vector->list v)
    )
  )

;; (define-syntax inc
;;  (lambda (x)
;;    (syntax-case x ()
;;      ((_ v)
;;       #'(if (number? 'x)
;; 	  (1+ x)
;; 	  (case (syntax-local-binding #'v)
;;             ((lexical displaced-lexical) #'(begin (Show! "lexical, displaced-lexical") 1))
;;             ((macro) #'(begin (Show! "macro") 2))
;; 	    ((syntax-parameter) #'(begin (Show! "syntax-parameter") 3))
;; 	    ((pattern-variable) #'(begin (Show! "pattern-variable") 4))
;; 	    ((ellipsis) #'(begin (Show! "ellipsis") 5))
;; 	    ((other) #'(begin (Show! "other") 6))
;; 	    ((global) #'(begin (Show! "global") 7))
;;             (else #'(begin (Show! "else") 8))))))))


(define (CompilePattern pattern) (make-regexp pattern))

(define (FindPattern regex str)
  (let ((compiled-regex (make-regexp regex)))
    (FindCompiledPattern compiled-regex str)))

(define (FindCompiledPattern compiled-regex str)
  (regexp-exec compiled-regex str))

;;ricorda che pattern può essere semplice o compilato
(define (ReplaceAll string pattern subst)
  (regexp-substitute/global #f pattern string 'pre subst 'post)
  )

(define (ReplaceAllMultiple string pattern-substitute-list)
  (if (or (null? string) (null? pattern-substitute-list))
      string
      (let* (
	     ;;Per prima cosa costruisco il pattern matching reale
	     (patterns "")
	     (substitutions '())
	     (d (for-each (lambda(it)
			    (set! patterns (string-append patterns "(" (car it) ")|"))
			    (set! substitutions (append substitutions (cdr it))))
			  pattern-substitute-list))
	     (patterns (string-drop-right patterns 1)) ;;toglie ultimo "|"
	     )
	;; (pretty-print patterns)
	;; (pretty-print substitutions)
	(regexp-substitute/global #f
				  patterns string
				  'pre
				  (lambda(m)
				    (let (
					  (index (list-index (lambda(it) (not (equal? -1 (car it)))) (cddr (vector->list m)))))
				      ;; (pretty-print (cddr (vector->list m)))
				      (nth-item substitutions index)))
				  'post))))

;;UTF-8, ISO-8859-1
(define mtfa-http-decode-html-page::charset-pattern (CompilePattern "charset=[\"]{0,1}([a-zA-Z0-9-]*)"))

(define (mtfa-http-decode-html-page-internal u8-page headers encoding)
  ;;se headers esiste, prova a cercare se è presente il charset: Content-Type: application/json; charset=UTF-8
  (when (and (null? encoding) (not (null? headers)))
    (let ((found (FindCompiledPattern mtfa-http-decode-html-page::charset-pattern headers)))
      (when found
	(set! encoding (match:substring found 1)))))
  (if (null? encoding)
      '()
      (bytevector->string u8-page encoding)))

(define (mtfa-http-decode-html-page-noerr f u8-page headers encoding)
  (catch #t
    (lambda() (f u8-page headers encoding))
    (lambda (key . parameters)
      ;;(pretty-print (list "Recovered from fatal error: " key parameters))
      (display-error '() (current-output-port) (car parameters) (cadr parameters) (caddr parameters) (cadddr parameters))
      ;;Ci riprovo con altra decodifica
      '()
      )))

(define (mtfa-http-decode-html-page u8-page headers encoding)
  (mtfa-http-decode-html-page-noerr mtfa-http-decode-html-page-internal u8-page headers encoding))



(define (GetValueFromPairsList key l)
  ;;(display (list key l "\n"))
  (let (
    	(kv (assoc key l)))
    (if (null? kv)
    	'()
    	(cdr kv))))



;;
;;Esempio di ftw o scandir
;;(scandir "/" (lambda(it) (if (or (equal? it ".") (equal? it "..")) #f #t))) => elenca tutti i file tranne . e ..
;;

(define (parse-xml xml-string)
  (cdaddr  (xml->sxml (regexp-substitute/global #f ">[ \t\n]+<" xml-string 'pre "><" 'post))))

(define (XmlEncode value)
  (if (null? value)
      '()
      (let* (
             (value (regexp-substitute/global #f "&" value 'pre "&amp;" 'post))
             (value (regexp-substitute/global #f "<" value 'pre "&lt;" 'post))
             (value (regexp-substitute/global #f ">" value 'pre "&gt;" 'post))
             (value (regexp-substitute/global #f "\"" value 'pre "&quot;" 'post))
             (value (regexp-substitute/global #f "'" value 'pre "&apos;" 'post)))
        value)))

(define (XmlDecode value)
  (if (null? value)
      '()
      (let* (
             (value (regexp-substitute/global #f "&amp;" value 'pre "&" 'post))
             (value (regexp-substitute/global #f "&lt;" value 'pre "<" 'post))
             (value (regexp-substitute/global #f "&gt;" value 'pre ">" 'post))
             (value (regexp-substitute/global #f "&quot;" value 'pre "\"" 'post))
             (value (regexp-substitute/global #f "&apos;" value 'pre "'" 'post)))
        value)))

(define (BuildXmlFlat list-of-tags list-of-values)
  (let
      ((header "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\r\n")
       (data "")
       (tail "</root>")
       )
    (map (lambda (t v)
           (set! data
                 (string-append
                  data
                  (string-append
                   "<" t ">" (XmlEncode v) "</" t ">\r\n"
                   )
                  )))
         list-of-tags list-of-values)
    (string-append header data tail)))

(define (TimeStamp)
  (let* (
         (now (datetime:current-time))
         (s-second (number->string (datetime:time-second now)))
         (s-nanosecond (number->string (datetime:time-nanosecond now)))
         (timestamp (string-append s-second (make-string (- 9 (string-length s-nanosecond)) #\0) s-nanosecond)))
    timestamp))

(define (mtfa-micros)
  (let* (
         (now (datetime:current-time))
         (s-second (* 1000000 (datetime:time-second now)))
         (s-nanosecond (/ (datetime:time-nanosecond now) 1000))
	 )
    (+ s-second s-nanosecond)))


;;;un accesso semplificato ai db
;;;
(define (OpenDb ip user passwd name port)
  (let* (
	 (db (mtfa-db-mysql-open))
	 (dummy (mtfa-db-mysql-connect db ip user passwd name port)))
    db)
  )

(define (CloseDb db)
  (mtfa-db-mysql-close db))

(define (BuildQueryStringFromList the-list the-separator the-extractor)
  (if (= 0 (length the-list))
      ""
      (let (
	    (res (list-to-string (map (lambda(it) (list-to-string (list (the-extractor it) the-separator))) the-list)))
	    )
	(substring res 0 (- (string-length res) (string-length the-separator))))))

(define (SelectDb db listOfTables listOfAttr listOfAttrOpValues last-stat)
  ;;Costruisce clausola where
  (let* (
	 (where (BuildQueryStringFromList listOfAttrOpValues " and " (lambda(it) (list-to-string (list (car it) " " (cadr it) " '" (caddr it) "'")))))
	 ;;Costruisce clausola select
	 (select (BuildQueryStringFromList listOfAttr ", " (lambda(it) it)))
	 ;;Costruisce tables
  	 (tables (BuildQueryStringFromList listOfTables ", " (lambda(it) it)))
	 (sqlcmd (list-to-string (list "select " select " from " tables 
				       (if (= 0 (string-length where)) "" (string-append " where " where)) last-stat)))
	 )
    ;;(pretty-print sqlcmd)
    (mtfa-db-mysql-do-sql db sqlcmd)
    ))

(define (InsertDb db table listOfKeyValues)
  (define (MakeString l)
    (if (null? l)
	""
	(if (null? (cdr l))
	    (car l)
	    (list-to-string (list (car l) ", " (MakeString (cdr l)))))))
  (let* (
	 (keys (map (lambda(it) (car it)) listOfKeyValues))
	 (values (map (lambda(it) (list-to-string (list "'" (cadr it) "'"))) listOfKeyValues))
	 (sqlcmd (list-to-string 
		  (list 
		   "insert into " table " ( " (MakeString keys) " ) values (" (MakeString values) ")"
		   )))
	 )
    (mtfa-db-mysql-do-sql db sqlcmd)))

(define (SqlDb db sqlcmd)
  (mtfa-db-mysql-do-sql db sqlcmd))


;;;Operazioni sui file
					; Esiste ed è una directory? (equal? 'regular (stat:type (stat "/tmp/a1.txt")))

;;mtfa-send-mail
;;      const char * smtpserver,
;;      const char * username,
;;      const char * password,
;;      const char * from,
;;      const list<string> params,  //ivi compreso to, cc, ccn eccetera
;; //    const char * to,
;; //    const char * cc,
;; //    const char * ccn,
;;      const char * msg,
;;      const bool check_server_cert,
;;      const char * personal_ca_path,
;;      const bool check_server_hostname

;;smtpserver,
;;username,
;;password,
;;from,
;;to,cc,ccn,
;;msg,
;;check_server_cert,
;;personal_ca_path,
;;check_server_hostname

(define (mtfa-send-mail-gen-id n)
  (mtfa-rand-alfanum n "abcdef0123456789"))

;; (define msg (mtfa-send-mail-prepare-annexes "" "" (list (list "franco.png" "pdf" (mtfa-b64-encode (fs-io-to-bv "/tmp/franco.png")) #t) (list "" "text" "Ciao, sono io" #t) )))
;; (define elenco (list "franco.rcr@gmail.com" "franco.arcieri@uniroma2.it" "franco.arcieri@inuitroma2.it" "geografo1@libero.it" "andrea.dimitri@uniroma2.it") )
;; (define elenco (list "franco.rcr@gmail.com" ))
;; (define (invia) 
;;   (for-each (lambda (dest) (mtfa-send-mail "smtp://smtpauth.uniroma2.it:25" "rcrfnc00" "shetfict" "franco.arcieri@uniroma2.it" (list dest) (string-append "From: (Carta Multiservizi di Ateneo)CartaMultiservizidiAteneo\r\n" "To: " dest "\r\n" "Subject: important message\r\n" msg) #nil 0)) elenco)
;;   )

;;Binary data enters in b64, domain is the email sender domain
(define (mtfa-send-mail-prepare-annexes domain body-text list-of-name-type-data-inline)
  (define boundary (mtfa-send-mail-gen-id 28))
  (define whole-msg "")
  (mtfa-noerr
   ""
  (set! whole-msg (string-append "MIME-Version: 1.0\r\nContent-Type: multipart/mixed; boundary=" boundary "\r\n\r\n"))
  (set! whole-msg 
   (string-append
   whole-msg
   (fold-right
    (lambda (item prev)
      (let (
	    (name (car item))
	    (type (cadr item))
	    (data (caddr item))
	    (inline (cadddr item))
	    (content-id (mtfa-send-mail-gen-id 12))
	    )
	(cond
	 ((string-ci=? type "pdf") ;;In questo caso inline non conta
	  ;;(Show "Sono in pdf e appendo: " data)
	  (string-append prev
			 "--" boundary "\r\n"
			 "Content-Type: application/pdf; name=\"" name "\"" "\r\n"
			 "Content-Disposition: " (if inline "inline" "attachment") "; filename=\"" name "\"" "\r\n"
			 "Content-Transfer-Encoding: base64" "\r\n"
			 "Content-ID: <" content-id ">" "\r\n"
			 "X-Attachment-Id: " content-id "\r\n"
			 "\r\n"
			 data
			 "\r\n"
			 )
	  )
	 ((string-ci=? type "text") ;;in questo caso inline indica che sta nel body e non è un attachment
	  (string-append prev
			 "--" boundary "\r\n"
			 "Content-Type: text/plain; charset=UTF-8" "\r\n"
			 (if inline
			     (string-append
			      "\r\n"
			      data
			      "\r\n"
			      "\r\n"
			      )
			     (string-append
			      "Content-Disposition: attachment; filename=\"" name "\"" "\r\n"
			      "Content-Transfer-Encoding: 8bit" "\r\n"
			      "Content-ID: <" content-id ">" "\r\n"
			      "X-Attachment-Id: " content-id "\r\n"
			      "\r\n"
			      data
			      "\r\n"
			      "\r\n"
			      )
			     )
			 )
	  )
	 ((string-ci=? type "html")
	  (string-append prev
			 "--" boundary "\r\n"
			 "Content-Type: text/html; charset=UTF-8" "\r\n"
			 "Content-Transfer-Encoding: 8bit" "\r\n"
			 (if inline
			     (string-append
			      "\r\n"
			      data
			      "\r\n"
			      "\r\n"
			      )
			     (string-append
			      "Content-Disposition: attachment; filename=\"" name "\"" "\r\n"
			      "Content-Transfer-Encoding: 8bit" "\r\n"
			      "Content-ID: <" content-id ">" "\r\n"
			      "X-Attachment-Id: " content-id "\r\n"
			      "\r\n"
			      data
			      "\r\n"
			      "\r\n"
			      )
			     )
			 )
	  )
	 )
	)
      )
    ""
    list-of-name-type-data-inline
    )))
  (string-append whole-msg "--" boundary "--" "\r\n")
  )
   )

;;un dispatcher per chiamare una funzione in corrispondenza di una stringa...
(define (mtfa-dispatcher::make not-found-function)
  (cons (mtfa-fs3-make) not-found-function))

(define (mtfa-dispatcher::call ds string-caller v-parameters)
  ;;(Show "call: " string-caller)
  (let ((to-be-called (mtfa-fs3-get (car ds) string-caller)))
    ;;(Show "to-be-called: " to-be-called)
    (if (null? to-be-called)
	((cdr ds) v-parameters)
	(to-be-called v-parameters)
	)
    ))

(define (mtfa-dispatcher::add ds string-caller function-to-be-called)
  (mtfa-fs3-update (car ds) string-caller function-to-be-called)
  ;;(Show "mtfa-dispatcher::add Get: " string-caller (mtfa-fs3-get (car ds) string-caller))
  )

(define (mtfa-dispatcher::del ds string-caller)
  (mtfa-fs3-update (car ds) string-caller '()))

;;Gestione di chiamate periodiche!
;;la variabile che contiene le funzioni chiamate periodicamente
(define PeriodicalTasks::threadid '())
(define PeriodicalTasks::data (mtfa-fs3-make))
(define PeriodicalTasks::data-mutex (make-mutex))
(define (PeriodicalTasks::GetAll)
  (with-mutex PeriodicalTasks::data-mutex
    (let ((res (filter (lambda (i) (not (null? i)))
		       (map cdr (mtfa-fs3-get-all PeriodicalTasks::data)))))
      ;;(Show res)
      res)))

(define (PeriodicalTasks::add id task parameters period-microsecs)
  (with-mutex PeriodicalTasks::data-mutex
    (mtfa-fs3-update PeriodicalTasks::data id (list id task parameters period-microsecs (mtfa-micros)))))

(define (PeriodicalTasks::remove id)
  (with-mutex PeriodicalTasks::data-mutex
    (mtfa-fs3-update PeriodicalTasks::data id '())))

(define (PeriodicalTasks::Stop)
  (when (not (null? PeriodicalTasks::threadid))
    (cancel-thread PeriodicalTasks::threadid)
    (set! PeriodicalTasks::threadid '())))

;;Il thread che periodicamente chiama funzioni
(define (PeriodicalTasks::InternalRun microsecs)
  (set! PeriodicalTasks::threadid (current-thread))
  (while #t
    (usleep microsecs)
    ;;per ogni funzione registrata, verifica che il tempo sia terminato e, se fosse, la chiama
    (map (lambda (it)
	   (let (
		 (id      (first it))
		 (task    (second it))
		 (pars    (third it))
		 (periodo (fourth it))
		 (tempo   (fifth it))
		 )
	     (when (> (mtfa-micros) (+ tempo periodo))
	       (catch #t
		 (lambda() (task pars))
		 (lambda (key . parameters) '())) ;(Show "Recovered from fatal error: " key " " parameters)
	       (PeriodicalTasks::add id task pars periodo)
	       )
	     ))
	 (PeriodicalTasks::GetAll)
	 )
    ))

(define (PeriodicalTasks::Run microsecs)
  (begin-thread (PeriodicalTasks::InternalRun microsecs)))

(define* (mtfa-string-split-regex s r #:optional (separator #\nul))
  (let ((s (ReplaceAll s r (string separator))))
    (string-split s separator)))

(define (mtfa-string-split-size s len)
  (fold-matches (make-string len #\.) s '() (lambda(c p) (cons (match:substring c) p))))

;;(define local-convert-byte-to-hex (vector "0" "1" "2" "3" "4" "5" "6" "7" "8" "9" "a" "b" "c" "d" "e" "f"))
(define (mtfa-bv-to-hex-string bv)
  (let* ((v (number->string (mtfa-bv-to-num bv) 16))
	 (len (- (* 2 (bytevector-length bv)) (string-length v) )))
    ;;se la lunghezza di v è inferiore ...ma non potrei fare un loop?
    (if (> len 0)
	(string-append (make-string len #\0) v)
	v)))

;;la stringa deve essere multipla di 2
(define (mtfa-hex-string-to-bv hs)
  (if (zero? (string-length hs))
      (make-bytevector 0)
      (begin
	(when (not (zero? (remainder (string-length hs) 2)))
	  (set! hs (string-append "0" hs)))
	(let* ((bv (mtfa-num-to-bv (string->number hs 16)))
	       (len (- (/ (string-length hs) 2) (bytevector-length bv))))
	  (if (> len 0)
	      (begin
		;;Devo aggiungere tanti 0 quanti ne mancano in testa al bv
		(u8-list->bytevector (append (make-list len 0) (bytevector->u8-list bv))))
	      bv)))))

(define (mtfa-hash-hs hash msg)
  (mtfa-bv-to-hex-string (mtfa-hash hash msg)))

(define (mtfa-hash-b64 hash msg)
  (mtfa-b64-encode (mtfa-hash hash msg)))

(define (mtfa-hmac-hs hash key msg)
  (mtfa-bv-to-hex-string (mtfa-hmac hash key msg)))

(define (mtfa-hmac-b64 hash key msg)
  (mtfa-b64-encode (mtfa-hmac hash key msg)))

;;mtfa-cifra-aes-js msg key
;;mtfa-decifra-aes-js msg key
;;
(define (mtfa-cifra-aes-js-hex msg key)
  (mtfa-bv-to-hex-string (mtfa-cifra-aes-js msg key)))

(define (mtfa-decifra-aes-js-hex-s msg key)
  (bytevector->string (mtfa-decifra-aes-js (mtfa-hex-string-to-bv msg) key) "utf-8"))

(define (mtfa-decifra-aes-js-hex-bv msg key)
  (mtfa-decifra-aes-js (mtfa-hex-string-to-bv msg) key))

(define (mtfa-cifra-aes-js-b64 msg key)
  (mtfa-b64-encode (mtfa-cifra-aes-js msg key)))

(define (mtfa-decifra-aes-js-b64-s msg key)
  (bytevector->string (mtfa-decifra-aes-js (mtfa-b64-decode-bv msg) key) "utf-8"))

(define (mtfa-decifra-aes-js-b64-bv msg key)
  (mtfa-decifra-aes-js (mtfa-b64-decode-bv msg) key))

(define (mtfa-bv-xor bv1 bv2)
  (let ((res (make-bytevector (bytevector-length bv1) 0)))
    (array-map! res (lambda (v1 v2) (logxor v1 v2)) bv1 bv2) res))

(define (mtfa-bv-or bv1 bv2)
  (let ((res (make-bytevector (bytevector-length bv1) 0)))
    (array-map! res (lambda (v1 v2) (logior v1 v2)) bv1 bv2) res))

(define (mtfa-bv-and bv1 bv2)
  (let ((res (make-bytevector (bytevector-length bv1) 0)))
    (array-map! res (lambda (v1 v2) (logand v1 v2)) bv1 bv2) res))

(define (mtfa-base32-encode-bv bv-or-s)
  (mtfa-base32-encode bv-or-s))

(define (mtfa-base32-decode-bv bv-or-s)
  (mtfa-base32-decode bv-or-s))

(define (mtfa-base32-encode-s bv-or-s)
  (bytevector->string (mtfa-base32-encode bv-or-s) "utf-8"))

(define (mtfa-base32-decode-s bv-or-s)
  (bytevector->string (mtfa-base32-decode bv-or-s) "utf-8"))


;;La generazione e il controllo di OTP
;; var epoch = Math.round(new Date().getTime() / 1000.0);
;; var time = leftpad(dec2hex(Math.floor(epoch / 30)), 16, "0");
;; var hmac = DoHMAC(time, secret);
;; var offset = hex2dec(hmac.substring(hmac.length - 1));
;; var otp = (hex2dec(hmac.substr(offset * 2, 8)) & hex2dec("7fffffff")) + "";
;; otp = (otp).substr(otp.length - 6, 6);

(define (mtfa-seconds)
  (floor-quotient (mtfa-micros) 1000000))

(define (mtfa-left-pad s l c)
  (let* (
	 (len (string-length s))
	 )
    (when (< len l)
      (set! s (string-append (make-string (- l len) c) s)))
    s))

(define* (mtfa-totp-gen secret length time-offset period)
  (let* (
	 (epoch (floor-quotient (- (mtfa-seconds) time-offset) period))
	 ;;(d (Show "Epoch: " epoch))
	 (dec2hex-epoch (number->string epoch 16))
	 ;;(d (Show "dec2hex-epoch: " dec2hex-epoch))
	 (time (mtfa-left-pad dec2hex-epoch 16 #\0))
	 ;;(d (Show "time: " time ))
	 (hmac (mtfa-hmac-hs 2 secret time))
	 ;;(d (Show "hmac: " hmac))
	 (offset (string->number (substring hmac (1- (string-length hmac))) 16))
	 ;;(d (Show "Offset: " offset))
	 (otp (number->string (logand (string->number (substring hmac (* 2 offset) (+ (* 2 offset) 8)) 16) #x7FFFFFFF)))
	 ;;(d (Show "Otp: " otp))
	 (otp (string-take-right (mtfa-left-pad otp length #\ ) length))
	 ;;(d (Show "Otp: " otp))
	 )
    otp
    ))


(define (mtfa-chinese-remainder primi moduli)
(mtfa-noerr '()
  ;;Calcolo il prodotto dei primi
  (let* (
	 (prodotto-primi (fold (lambda (item prev) (* item prev)) 1 primi))
	 (risultato (fold (lambda (pri mod prev)
			    (let* ((p (quotient prodotto-primi pri)))
			    (+ prev (* mod
				       (mtfa-mpz-invert p pri)
				       p))))
			  0 primi moduli)))
    (remainder risultato prodotto-primi))))

;;Il non determinismo!!
(define amb-fail 
  (lambda ()
	(error "Amb tree exhausted")))

(define-syntax ambl
  (syntax-rules () 
	((ambl lista)
	#'(eval (cons 'amb lista) (interaction-environment))
	)))

(define-syntax amb
  (syntax-rules () 
    ((amb) (amb-fail))                      ; Two shortcuts.
    ((amb expression) expression)
    
    ((amb expression ...) 
     (let ((fail-save amb-fail))
          ((call-with-current-continuation ; Capture a continuation to
            (lambda (k-success)           ;   which we return possibles.
                    (call-with-current-continuation
                     (lambda (k-failure)       ; K-FAILURE will try the next
                             (set! amb-fail k-failure)   ;   possible expression.
                             (k-success              ; Note that the expression is
                              (lambda ()             ;   evaluated in tail position
                                      expression))))       ;   with respect to AMB.
                    ...
                    (set! amb-fail fail-save)      ; Finally, if this is reached,
                    fail-save)))))))            ;   we restore the saved FAIL.

(define (amb-require condition)
   	(if (not condition)
          (amb-fail)))

(define-syntax amb-possibility-list
   (syntax-rules () 
     ((amb-possibility-list expression)
      (let ((value-list '())) 
        ;; This requires that AMB try its sub-forms left-to-right.
        (amb (let ((value expression)) 
               (set! value-list (cons value value-list))
               (amb-fail))
             (reverse value-list))))))

(export-syntax amb ambl amb-possibility-list)
(export amb-fail amb-require)

(defun mtfa-cimg-image () 
  #f =>
  (let ((im #f)
	(s-win-size #f)
	(n-win-size #f)
	(s-geo-size #f)
	(n-geo-size #f))
    ;;
    (define (set-line-color im color-line)
      (when (not-nil? color-line)
	(if (string? color-line)
	    (mtfa-cimg-set-linecolor-string im color-line)
	    (mtfa-cimg-set-linecolor im (first color-line) (second color-line) (third color-line)))))
    (define (set-bg-color im color)
      (when (not-nil? color)
	(if (string? color)
	    (mtfa-cimg-set-bg-string im color)
	    (mtfa-cimg-set-bg im (first color) (second color) (third color)))))
    (define (set-fill-color im color-fill)
      (when (not-nil? color-fill)
	(if (string? color-fill)
	    (mtfa-cimg-set-fillcolor-string im color-fill)
	    (mtfa-cimg-set-fillcolor im (first color-fill) (second color-fill) (third color-fill)))))
    (define (Help)
      (Show "Commands\r\n"
	    "'create win-size geo-size\n"
	    "'box    x0 y0 x1 y0 color-line color-fill\n"
	    "'line   x0 y0 x1 y0 color-line color-fill\n"
	    "'point  x0 y0 color-line\n"
	    "'circle x0 y0 radius color-line\n"
	    "'quit\n"
	    "'erase\n"
	    "'flush\n"
	    "'text x0 y0 text angle color\n"
	    "'font name size\n"
	    "'filltype num\n"
	    "'linewidth num\n"
	    "'background color\n"))
    (lambda (cmd . params)
      (mtfa-noerr
       (begin
	 (when im (mtfa-cimg-close im))
	 (set! im #f)
	 (Help)
	 #f)
       (match (cons cmd params)
	 ;;
	 (('create win-size geo-size)
	  (Show win-size ", " geo-size)
	  (set! s-win-size (number->string win-size))
	  (set! n-win-size win-size)
	  (set! s-geo-size (number->string geo-size))
	  (set! n-geo-size geo-size)
	  (set! im (mtfa-make-cimg 
		    `(("BITMAPSIZE" . ,(string-append s-win-size "x" s-win-size))
		      ("X_AUTO_FLUSH" . "no") ("USE_DOUBLE_BUFFERING" . "no") ("VANISH_ON_DELETE" . "yes"))))
	  (mtfa-cimg-set-space im 0 0 n-geo-size n-geo-size)
	  (mtfa-cimg-set-linewidth im 1)
	  (mtfa-cimg-set-filltype im 1)
	  (mtfa-cimg-erase im))
	 ;;
	 (('box  x0 y0 x1 y1 color-line color-fill)
	  (set-line-color im color-line)
	  (set-fill-color im color-fill)
	  (mtfa-cimg-draw-box im x0 (- n-geo-size y0 1) x1  (- n-geo-size y1 1)))
	 ;;
	 (('line  x0 y0 x1 y1 color-line)
	  (set-line-color im color-line)
	  (mtfa-cimg-draw-line im x0 (- n-geo-size y0 1) x1  (- n-geo-size y1 1)))
	 ;;
	 (('point  x0 y0 color-line)
	  (set-line-color im color-line)
	  (mtfa-cimg-draw-point im x0 (- n-geo-size y0 1)))
	 ;;
	 (('circle x0 y0 radius color-line color-fill)
	  (set-line-color im color-line)
	  (set-fill-color im color-fill)
	  (mtfa-cimg-draw-circle im x0 (- n-geo-size y0 1) radius)
	  )
	 ;;
	 (('quit) (mtfa-cimg-close im) (set! im #f))
	 (('erase) (mtfa-cimg-erase im))
	 (('flush) (mtfa-cimg-flush im))
	 ;;
	 (('text x0 y0 text angle color-line)
	  (set-line-color im color-line)
	  (mtfa-cimg-text-angle im angle)
	  (mtfa-cimg-move im x0 (- n-geo-size y0 1))
	  (mtfa-cimg-draw-label im 0 0 text))
	 ;;
	 (('font name size)
	  (mtfa-cimg-font-name im name)
	  (mtfa-cimg-font-size im size))
	 ;;
	 (('filltype num)
	  (mtfa-cimg-set-filltype im num))
	 ;;
	 (('linewidth num)
	  (mtfa-cimg-set-linewidth im num))
	 ;;
	 (('background color)
	  (set-bg-color im color))
	 ;;
	 (_ (Help)))))))

(define-public mtfa-char-set-alfa-lower "abcdefghijklmnopqrstuvwxyz")
(define-public mtfa-char-set-alfa-upper "ABCDEFGHIJKLMNOPQRSTUVWXYZ")
(define-public mtfa-char-set-alfa "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
(define-public mtfa-char-set-alfanum-lower "0123456789abcdefghijklmnopqrstuvwxyz")
(define-public mtfa-char-set-alfanum-upper "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
(define-public mtfa-char-set-alfanum "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
(define-public mtfa-char-set-hex-lower "0123456789abcdef")
(define-public mtfa-char-set-hex-upper "0123456789ABCDEF")
(define-public mtfa-char-set-all-bv (u8-list->bytevector (iota 256)))
(define-public mtfa-char-set-all (bytevector->string (u8-list->bytevector (iota 256)) "iso-8859-1"))
(define-public mtfa-char-set-half-bv (u8-list->bytevector (iota 128)))
(define-public mtfa-char-set-half (bytevector->string (u8-list->bytevector (iota 128)) "utf-8"))

(define-syntax-public mtfa-m-add
  (syntax-rules ()
    ((_ v)   (begin (set! v (1+ v)) v))
    ((_ v n) (begin (set! v (+ n v)) v))))
(define-syntax-public mtfa-m-sub
  (syntax-rules ()
    ((_ v)   (begin (set! v (1- v)) v))
    ((_ v n) (begin (set! v (- v n)) v))))
(define-syntax-public mtfa-m-mul
  (syntax-rules ()
    ((_ v n) (begin (set! v (* v n)) v))))
(define-syntax-public mtfa-m-div
  (syntax-rules ()
    ((_ v n) (begin (set! v (/ v n)) v))))
(define-syntax-public +=
  (syntax-rules ()
    ((_ v)   (begin (set! v (1+ v)) v))
    ((_ v n) (begin (set! v (+ n v)) v))))
(define-syntax-public -=
  (syntax-rules ()
    ((_ v)   (begin (set! v (1- v)) v))
    ((_ v n) (begin (set! v (- v n)) v))))
(define-syntax-public *=
  (syntax-rules ()
    ((_ v n) (begin (set! v (* v n)) v))))
(define-syntax-public /=
  (syntax-rules ()
    ((_ v n) (begin (set! v (/ v n)) v))))

(define-syntax-public mtfa-m-cons
  (lambda (x)
    (syntax-case x ()
      ((_ a b)
       #'(begin (set! b (cons a b)) b)))))
;;(document! mtfa-m-cons   "(mtfa-m-cons a b) => (begin (set! b (cons a b)) b)")
(define-syntax-public mtfa-m-append
  (lambda (x)
    (syntax-case x ()
      ((_ a b)
       #'(begin (set! a (append a b)) a)))))

;;Locali a questo codice
(define ver-prime 3456773411378589083)
(define ver-key "4a8nx7mda1vWMpqQQ6ObFRLQbaHTy6Te")
;;
;;Crea un timestamp verificabile, utilizzando un primo segreto per invertire il valore dei microsecondi per
;;non mostrare che è un timestamp. Calcola l'hmac secondo lo standard
(define (MakeStdhmac key value)
  (mtfa-hmac-hs 1 key value))
(define-public (mtfa-time-stamp-make duration-in-secs)
  "Durata in secondi del timestamp verificabile"
  (let* (
	 (current-timestamp (mtfa-micros))
	 (nv1 (mtfa-mpz-invert current-timestamp ver-prime))
	 (nv2 (mtfa-mpz-invert (logxor nv1 (* 1000000 (1+ duration-in-secs))) ver-prime))
	 (v1 (number->string nv1))
	 (v2 (number->string nv2))
	 (v (string-append v1 "-" v2))
	 )
    (string-append (MakeStdhmac ver-key v) "-" v )))

;;
;;verifica che il timestamp creato con MakeVerifiableTimeStamp, sia effettivamente
;;stato creato con tale funzione e che il tempo trascorso, in microsecondi, non sia ancora
;;superiore a un valore dato
(define-public (mtfa-time-stamp-check ts)
  (mtfa-noerr
   #f
   (let* ((lts (string-split ts #\-))
	  (current-timestamp (mtfa-micros))
	  (nv1 (string->number (second lts)))
	  (v1 (mtfa-mpz-invert nv1 ver-prime))
	  (v2 (logxor nv1 (mtfa-mpz-invert (string->number (third  lts)) ver-prime)))
	  (check (MakeStdhmac ver-key (string-append (second lts) "-" (third lts)))))
     (if (string=? check (first lts))
	 (if (> (+ v1 v2) (mtfa-micros))
	     #t
	     #f
	     )
	 #f)
     )
   ))

(define-public (seconds) (datetime:time-second (datetime:current-time)))


(define-public (mtfa-entropy bv)
  ;;faccio un vettore di 256 tutti a zero
  (mtfa-assert (bytevector? bv) "need a bytevector")
  (let ((v (make-vector 256 0))
        (values (bytevector->u8-list bv))
        (len (bytevector-length bv))
        )
    ;;calcolo le probabilità come a partire da bv
    (for-each (lambda (it) (vector-set! v it (1+ (vector-ref v it)))) values)
    ;;Calcola ora somma p * log(p) per ogni elemento
    (/ (* -1 (fold (lambda (cur prev)
                   (if (zero? cur)
                       prev
                       (+ prev (* (/ cur len) (log (/ cur len))))
                       )
                   ) 0 (vector->list v))) (log 256.0))
    )
  )


(defun mtfa-string->bytevector (s) #f =>
  (when (bytevector? s) (return s))
  (mtfa-noerr
   (mtfa-noerr
    (return #f)
    (return (string->bytevector s "ISO-8859-1"))
    )
   (return (string->bytevector s "UTF-8"))))
(defun mtfa-bytevector->string (s) #f =>
  (when (string? s) s)
  (mtfa-noerr
   (mtfa-noerr
    (return #f)
    (return (bytevector->string s "ISO-8859-1"))
    )
   (return (bytevector->string s "UTF-8"))))

(export mtfa-string->bytevector mtfa-bytevector->string)

;;(define-public (mtfa-scmjson-to-rlp s)
	       ;;(mtfa-noerr (Show "The structure <" s "> cannot be converted in json string") (mtfa-json-to-rlp::internal (scm->json-string (scm->json-string s))))
;;	       (mtfa-json-to-rlp::internal (scm->json-string s)))

;;una funzione che prende una lista e la converte in intero, dove il primo elemento è il più significativo
(defun L2I (l)
  (fold (lambda (c p)
	  (+ c (arithmetic-shift p 8)))
	0
	l)
  )

(define-public (mtfa-rlp-to-scmjson s)
  (let ((bvret (mtfa-rlp-to-json::internal s)))  ;;questo torna un bytevector
    ;;per fare la conversione, ora utilizzo lo split delle stringhe in caratteri e poi riporto a stringhe hex!!!
    ;;per ora ottengo una struttura del tipo: #((#\soh) (#\w #\5 #\224 #\nul) (#\R #\backspace) (#\? #\234 #\enq #\207 #\rs #\» #\Ô #\) #\` #\207 #\p #\£ #\8 #\, #\A #\R #\211 #\X #\õ #\É) (#\# #\206 #\ò #\o #\Á #\nul #\nul) () (#\&) (#\K #\tab #\\ #\¦ #\6 #\214 #\J #\224 #\sub #\y #\ï #\@ #\dc4 #\em #\@ #\B #\page #\! #\Û #\etb #\vtab #\t #\, #\$ #\¤ #\å #\x #\L #\' #\: #\Æ #\gs) (#\* #\@ #\} #\dc1 #\¬ #\6 #\e #\# #\U #\¾ #\> #\s #\si #\newline #\K #\return #\£ #\C #\E #\rs #\â #\ñ #\dle #\£ #\¾ #\backspace #\eot #\l #\j #\B #\÷ #\%))
    ;;devo ora convertire una lista 
    (vector-map (lambda (i v) (number->string (L2I (map char->integer (string->list v))) 16))
		(json-string->scm (bytevector->string bvret "ISO-8859-1")))))

;;
;;Macro per definire la sintassi is e isnt
(define-syntax infix/postfix
    (syntax-rules ()
      ((infix/postfix x somewhat?)
       (somewhat? x))

      ((infix/postfix left related-to? right)
       (related-to? left right))

      ((infix/postfix left related-to? right . likewise)
       (let ((right* right))
         (and (infix/postfix left related-to? right*)
              (infix/postfix right* . likewise))))))

  (define-syntax extract-placeholders
    (syntax-rules (_)
      ((extract-placeholders final () () body)
       (final (infix/postfix . body)))

      ((extract-placeholders final () args body)
       (lambda args (final (infix/postfix . body))))

      ((extract-placeholders final (_ op . rest) (args ...) (body ...))
       (extract-placeholders final rest (args ... arg) (body ... arg op)))

      ((extract-placeholders final (arg op . rest) args (body ...))
       (extract-placeholders final rest args (body ... arg op)))

      ((extract-placeholders final (_) (args ...) (body ...))
       (extract-placeholders final () (args ... arg) (body ... arg)))

      ((extract-placeholders final (arg) args (body ...))
       (extract-placeholders final () args (body ... arg)))))

  (define-syntax identity-syntax
    (syntax-rules ()
      ((identity-syntax form)
       form)))

  (define-syntax-public is
    (syntax-rules ()
      ((is . something)
       (extract-placeholders identity-syntax something () ()))))

  (define-syntax-public isnt
    (syntax-rules ()
      ((isnt . something)
       (extract-placeholders not something () ()))))


;;Nuove funzioni e macro, aggiunte 11 febbraio 2021
;;Definisce un simbolo pari a sé stesso quotato
(define-syntax-public mtfa-define-public-symbol
  (lambda (x)
    (syntax-case x ()
      ((_ s)
       #'(begin
	   (define s 's)))
      ((_ s s1 ...)
       #'(begin
	   (define s 's)
	   (mtfa-define-public-symbol s1 ...))))))
;;
;;Definisce un simbolo e gli associa un valore
(define-syntax-public mtfa-define-public-symbol-value
  (lambda (x)
    (define gen-id
      (lambda (template-identifier . args)
	(datum->syntax
	 template-identifier
	 (string->symbol
	  (apply string-append
		 (map (lambda (x)
			(cond
			 ((string? x) x)
			 ((or (symbol? x) (identifier? x)) (symbol->string (syntax->datum x)))
			 (#t (let ((a (eval (syntax->datum x) (interaction-environment))))
			       (if (string? a)
				   a
				   (symbol->string a))))))
		      args))))))
    (syntax-case x ()
      ((_ prefix (k . v))
       (with-syntax
	   ((sym (gen-id x #'prefix "::" #'k)))
	 #'(define sym v))))))
;;
(define-syntax-public mtfa-define-public-symbol-value-from-assoc-list
  (lambda (x)
    (syntax-case x ()
      ((_ prefix lkv)
       #'(for-each (lambda (kv)
		     (eval-string
		      (string-append "(define-public " prefix "::" (car kv) " " (cond 
									  ((string? (cdr kv)) (string-append "\"" (cdr kv) "\""))
									  ((vector? (cdr kv)) (string-append "'" (mtfa-to-string (vector->list (cdr kv)))))
									  (#t (mtfa-to-string (cdr kv)))) ")")))
		   lkv)))))
;;
(define-public (mtfa-define-symbol-value-pairs-from-json prefix fname)
  (let ((lkv (with-input-from-file fname (lambda () (json->scm)))))
    (mtfa-define-public-symbol-value-from-assoc-list prefix lkv)))

(define-syntax-public json-let
  (syntax-rules ()
    ((_ json (n1 ...) exp exp* ...)
     (let ((n1 (assoc-ref json (symbol->string 'n1))) ...) exp exp* ...))))
;;
;;
(define-syntax-public json-make
  (syntax-rules ()
    ((_ n1) (list `(,(symbol->string 'n1) . ,n1)))
    ((_ n1 n2 ...) (cons `(,(symbol->string 'n1) . ,n1) (json-make n2 ...)))))
;;
(define-syntax-public json-make-proc
  (lambda (x)
    (define gen-id
      (lambda (template-identifier . args )
        (datum->syntax
         template-identifier
         (string->symbol
          (apply string-append
                 (map (lambda (x)
                        (if (string? x)
                            x
                            (symbol->string (syntax->datum x ))))
                      args ))))))
    (syntax-case x ()
      ((_ title name ...)
       (with-syntax ((sym (gen-id #'title "json::" #'title)))
	 #'(begin
	     (define (sym name ...)
	       (scm->json-string `((,(symbol->string 'name) . ,name) ...)))))))))
;;
(define-syntax-public mtfa-multiple-sort
  (lambda (x)
    (syntax-case x ()
      ((_ object (f1 ...) (c1 ...))
       #'((lambda ()
	    (define-syntax mtfa-multiple-if
	      (lambda (xx)
		(with-ellipsis :::
			       (syntax-case xx ()
				 ((_ v1x v2x () ())
				  #'#t)
				 ((_ v1x v2x (f1x f2x :::) (c1x c2x :::))
				  #'(if (c1x (f1x v1x) (f1x v2x))
					#t
					(if (equal? (f1x v1x) (f1x v2x))
					    (mtfa-multiple-if v1x v2x (f2x :::) (c2x :::))
					    #f)))))))
	    (sort object (lambda (v1 v2) (mtfa-multiple-if v1 v2 (f1 ...) (c1 ...))))))))))
;;
(define-syntax-public mtfa-record-make
  (lambda (x)
    (define gen-id
      (lambda (template-identifier . args )
	(datum->syntax
	 template-identifier
	 (string->symbol
	  (apply string-append
		 (map (lambda (x)
			(if (string? x)
			    x
			    (symbol->string (syntax->datum x ))))
		      args ))))))
    (syntax-case x ()
      ((_ name f1 ...)
       (with-syntax
	   ((rec-type (gen-id #'name "<" #'name ">"))
	    (constructor (gen-id #'name #'name "-make"))
	    (predicate (gen-id #'name #'name "?"))
	    (from-json (gen-id #'name #'name "-from-json"))
	    (to-json (gen-id #'name #'name "-to-json"))
	    (from-json-string (gen-id #'name #'name "-from-json-string"))
	    (to-json-string (gen-id #'name #'name "-to-json-string"))
	    (clone (gen-id #'name #'name "-clone"))
	    ((field-list ...)
	     (map (lambda (x)
		    (gen-id x x))
		  #'(f1 ...)))
	    ((string-field-list ...)
	     (map (lambda (x)
		    (symbol->string (syntax->datum x)))
		  #'(f1 ...)))
	    ((getter-putter-list ...)
	     (map (lambda (x)
		    (list (gen-id x x) (gen-id x #'name "-" x) (gen-id x #'name "-" x "!")))
		  #'(f1 ...)))
	    ((getters ...)
	     (map (lambda (x)
		    (gen-id x #'name "-" x))
		  #'(f1 ...)))
	    )
	 #'(begin
	     (define-record-type rec-type (constructor field-list ...) predicate getter-putter-list ...)
	     (define from-json-string (lambda (json-string)
					(let* ((data (json-string->scm json-string))
					       (values (map (lambda (k) (assoc-ref data k)) (list string-field-list ...))))
					  (apply constructor values))))
	     (define from-json (lambda (json-data)
				 (let* ((values (map (lambda (k) (assoc-ref json-data k)) (list string-field-list ...))))
				   (apply constructor values))))
	     (define to-json-string (lambda (rec)
				      (let* ((pairs (map (lambda (key getter) (cons key (getter rec))) (list string-field-list ...) (list getters ...))))
					(scm->json-string pairs))))
	     (define to-json (lambda (rec)
			       (let* ((pairs (map (lambda (key getter) (cons key (getter rec))) (list string-field-list ...) (list getters ...))))
				 pairs)))
	     (define clone (lambda (rec)
			     (let* ((pairs (map (lambda (getter) (getter rec)) (list getters ...))))
			       (apply constructor pairs))))))))))


;;Per generare una closure e pubblicarne i comandi
(define-syntax-public mtfa-make-closure
  (lambda (x)
    (define gen-id
      (lambda (template-identifier . args )
	(datum->syntax
	 template-identifier
	 (string->symbol
	  (apply string-append
		 (map (lambda (x)
			(if (string? x)
			    x
			    (symbol->string (syntax->datum x))))
		      args ))))))
    (syntax-case x ()
      ((_ name prefix pars-list assignment-list expr ...)  ;;expr: ((add p1 p2) (+ p1 1) (+ p2 2) ...) ...
       (with-syntax (((expr-list ...)
		      (map (lambda (x)
			     (let ((e1 (caar (syntax->datum x))))
			       (list (datum->syntax x 'define) (gen-id x #'prefix "::" e1)
				     (datum->syntax x (list 'quote e1)))))
			   #'(expr ...)))
		     ((matchers-list ...)
		      (map (lambda (x)
			     (let ((e-1 (car (syntax->datum x)))
				   (e-rest (cdr (syntax->datum x))))
			       (cons (datum->syntax x (cons (list 'quote (car e-1)) (cdr e-1)))
				     (datum->syntax x e-rest))))
			   #'(expr ...))))
	 #'(begin
	     expr-list ...
	     (define name (lambda pars-list
			     (let* assignment-list
			       ;; (define* (doit @pars-list #(map car assignment-list)))
			       (lambda (cmd . pars)
			         (monitor
			          (match (cons cmd pars)
				    matchers-list ...
			            (_ (display "Command not recognized") (newline))))))))))))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;UNA CODA ASINCRONA;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;Ripresa da guile lib
(define-class <mtfa-async-queue> ()
  (queue #:init-form (make-q) #:getter queue)
  (condv #:init-form (make-condition-variable) #:getter condv)
  (mutex #:init-form (make-mutex) #:getter mutex)
  (waiting-threads #:init-value 0 #:accessor waiting-threads))

(define-public (mtfa-asq-make)
  "Create a new asynchronous queue."
  (make <mtfa-async-queue>))

(define-public (mtfa-asq-enq! q elt)
  "Enqueue @var{elt} into @var{q}."
  (with-mutex (mutex q)
    (enq! (queue q) elt)
    (if (> (waiting-threads q) 0)
        (signal-condition-variable (condv q)))))

(define-public (mtfa-asq-deq! q)
  "Dequeue a single element from @var{q}. If the queue is empty, the
calling thread is blocked until an element is enqueued by another
thread."
  (with-mutex (mutex q)
    (cond ((q-empty? (queue q))
           (set! (waiting-threads q) (+ (waiting-threads q) 1))
           (let loop ()
             (cond ((q-empty? (queue q))
                    (wait-condition-variable (condv q) (mutex q))
                    (loop))))
           (set! (waiting-threads q) (- (waiting-threads q) 1))))
    (deq! (queue q))))

(define-public (mtfa-asq-top! q)
  "Get a single element from @var{q}. If the queue is empty, the
calling thread is blocked until an element is enqueued by another
thread."
  (with-mutex (mutex q)
    (cond ((q-empty? (queue q))
           (set! (waiting-threads q) (+ (waiting-threads q) 1))
           (let loop ()
             (cond ((q-empty? (queue q))
                    (wait-condition-variable (condv q) (mutex q))
                    (loop))))
           (set! (waiting-threads q) (- (waiting-threads q) 1))))
    (q-front (queue q))))

(define-public (mtfa-asq-empty? q)
  "Check if @var{q} is empty."
  (with-mutex (mutex q)
    (q-empty? (queue q))))

(define-public (mtfa-asq-len q)
  "Get @var{q} length."
  (with-mutex (mutex q)
    (q-length (queue q))))
;;

;;Un log manager con una coda
(define mtfa-logger-is-running #f)
(define (mtfa-make-logger logfilename)
  (if mtfa-logger-is-running
      #f
      (let ((logger::aq (make-async-queue))
	    (logfile (if (null? logfilename) '() logfilename)))
	(call-with-new-thread (lambda ()
		       (let ((gm (make-mutex))
			     (outfile (if (null? logfile) '() (open-file logfile "al")))) ;;Append and buffering line
			 (set! mtfa-logger-is-running #t)
			 (let loop ((data (async-dequeue! logger::aq)))
			   (with-mutex gm
			     (display (cdr data))
			     (when (and (not (null? outfile)) (eqv? #t (car data)))
			       (display (cdr data) outfile)))
			   (loop (async-dequeue! logger::aq))))))
	(lambda (cmd . l)
	  (let ((micros (gettimeofday)))
	    (cond
	     ((eqv? cmd 'flog)
	      (async-enqueue! logger::aq
			      `(#t . ,(apply format (append (list #f (string-append "~a.~6,'0d: " (fold (lambda(c p) (string-append " ~a" p)) "\n" (iota (length l)))))
							    (append `(,(car micros) ,(cdr micros)) l))))))
	     ((eqv? cmd 'flog!)
	      (async-enqueue! logger::aq
			      `(#t . ,(apply format (append (list #f (string-append "~a.~6,'0d: " (fold (lambda(c p) (string-append "~a" p)) "\n" (iota (length l)))))
							    (append `(,(car micros) ,(cdr micros)) l))))))
	     ((eqv? cmd 'flog-)
	      (async-enqueue! logger::aq
			      `(#t . ,(apply format (append (list #f (string-append "~a.~6,'0d: " (fold (lambda(c p) (string-append " ~a" p)) "" (iota (length l)))))
							    (append `(,(car micros) ,(cdr micros)) l))))))
	     ((eqv? cmd 'flog!-)
	      (async-enqueue! logger::aq
			      `(#t . ,(apply format (append (list #f (string-append "~a.~6,'0d: " (fold (lambda(c p) (string-append "~a" p)) "" (iota (length l)))))
							    (append `(,(car micros) ,(cdr micros)) l))))))
	     
	     ((eqv? cmd 'nflog)
	      (async-enqueue! logger::aq
			      `(#f . ,(apply format (append (list #f (string-append "~a.~6,'0d: " (fold (lambda(c p) (string-append " ~a" p)) "\n" (iota (length l)))))
							    (append `(,(car micros) ,(cdr micros)) l))))))
	     ((eqv? cmd 'nflog!)
	      (async-enqueue! logger::aq
			      `(#f . ,(apply format (append (list #f (string-append "~a.~6,'0d: " (fold (lambda(c p) (string-append "~a" p)) "\n" (iota (length l)))))
							    (append `(,(car micros) ,(cdr micros)) l))))))
	     ((eqv? cmd 'nflog-)
	      (async-enqueue! logger::aq
			      `(#f . ,(apply format (append (list #f (string-append "~a.~6,'0d: " (fold (lambda(c p) (string-append " ~a" p)) "" (iota (length l)))))
							    (append `(,(car micros) ,(cdr micros)) l))))))
	     ((eqv? cmd 'nflog!-)
	      (async-enqueue! logger::aq
			      `(#f . ,(apply format (append (list #f (string-append "~a.~6,'0d: " (fold (lambda(c p) (string-append "~a" p)) "" (iota (length l)))))
							    (append `(,(car micros) ,(cdr micros)) l))))))))))))
(define mtfa-logger #f)
(define mtfa-logger-filename #f)
(define-public (mtfa-start-logger . filename)
  (if (null? filename)
      (set! mtfa-logger-filename '())
      (set! mtfa-logger-filename (car filename))))

(define-syntax-rule (check-mtfa-logger)
  (unless mtfa-logger
      (set! mtfa-logger (mtfa-make-logger mtfa-logger-filename))))

;;(export-syntax mtfa-start-logger)
(define-public FLOG-EMERG 'FLOG-EMERG)
(define-public FLOG-ALERT 'FLOG-ALERT)
(define-public FLOG-CRIT 'FLOG-CRIT)
(define-public FLOG-ERROR 'FLOG-ERROR)
(define-public FLOG-WARN 'FLOG-WARN)
(define-public FLOG-NOTICE 'FLOG-NOTICE)
(define-public FLOG-INFO 'FLOG-INFO)
(define-public FLOG-DEBUG 'FLOG-DEBUG)
(define-public FLOG-TRACE0 'FLOG-TRACE0)
(define-public FLOG-TRACE1 'FLOG-TRACE1)
(define-public FLOG-TRACE2 'FLOG-TRACE2)
(define-public FLOG-TRACE3 'FLOG-TRACE3)
(define-public FLOG-TRACE4 'FLOG-TRACE4)
(define-public FLOG-TRACE5 'FLOG-TRACE5)
(define-public FLOG-TRACE6 'FLOG-TRACE6)
(define-public FLOG-TRACE7 'FLOG-TRACE7)
(define-public FLOG-TRACE8 'FLOG-TRACE8)
(define-public FLOG-TRACE9 'FLOG-TRACE9)
#|
I tipi di log
emerg	System is unusable
alert	Action must be taken immediately
crit	Critical conditions
error	Error conditions
warn	Warning conditions
notice	Normal, but significant conditions
info	Informational messages
debug	Debugging messages
trace0 – trace9	Trace messages with gradually increasing levels of detail
|#
(define-syntax flog   (lambda (x) (syntax-case x () ((_ v ...)  #'(ifnot mtfa-logger-filename #f (begin (check-mtfa-logger) (mtfa-logger 'flog v ...)))))))
(define-syntax flog!  (lambda (x) (syntax-case x () ((_ v ...)  #'(ifnot mtfa-logger-filename #f (begin (check-mtfa-logger) (mtfa-logger 'flog! v ...)))))))
(define-syntax flog-  (lambda (x) (syntax-case x () ((_ v ...)  #'(ifnot mtfa-logger-filename #f (begin (check-mtfa-logger) (mtfa-logger 'flog- v ...)))))))
(define-syntax flog!- (lambda (x) (syntax-case x () ((_ v ...)  #'(ifnot mtfa-logger-filename #f (begin (check-mtfa-logger) (mtfa-logger 'flog!- v ...)))))))

(define-syntax nflog   (lambda (x) (syntax-case x () ((_ v ...)  #'(ifnot mtfa-logger-filename #f (begin (check-mtfa-logger) (mtfa-logger 'nflog v ...)))))))
(define-syntax nflog!  (lambda (x) (syntax-case x () ((_ v ...)  #'(ifnot mtfa-logger-filename #f (begin (check-mtfa-logger) (mtfa-logger 'nflog! v ...)))))))
(define-syntax nflog-  (lambda (x) (syntax-case x () ((_ v ...)  #'(ifnot mtfa-logger-filename #f (begin (check-mtfa-logger) (mtfa-logger 'nflog- v ...)))))))
(define-syntax nflog!- (lambda (x) (syntax-case x () ((_ v ...)  #'(ifnot mtfa-logger-filename #f (begin (check-mtfa-logger) (mtfa-logger 'nflog!- v ...)))))))

(define-syntax-rule (flog-emergency v ...) (flog 'EMERG v ...))
(define-syntax-rule (flog-alert v ...) (flog 'ALERT v ...))
(define-syntax-rule (flog-critical v ...) (flog 'CRITC v ...))
(define-syntax-rule (flog-error v ...) (flog 'ERROR v ...))
(define-syntax-rule (flog-warning v ...) (flog 'WARNG v ...))
(define-syntax-rule (flog-notice v ...) (flog 'NOTIC v ...))
(define-syntax-rule (flog-info v ...) (flog 'INFOR v ...))
(define-syntax-rule (flog-debug v ...) (flog 'DEBUG v ...))
(define-syntax-rule (flog-trace-0 v ...) (flog 'TRAC0 v ...))
(define-syntax-rule (flog-trace-1 v ...) (flog 'TRAC1 v ...))
(define-syntax-rule (flog-trace-2 v ...) (flog 'TRAC2 v ...))
(define-syntax-rule (flog-trace-3 v ...) (flog 'TRAC3 v ...))
(define-syntax-rule (flog-trace-4 v ...) (flog 'TRAC4 v ...))
(define-syntax-rule (flog-trace-5 v ...) (flog 'TRAC5 v ...))
(define-syntax-rule (flog-trace-6 v ...) (flog 'TRAC6 v ...))
(define-syntax-rule (flog-trace-7 v ...) (flog 'TRAC7 v ...))
(define-syntax-rule (flog-trace-8 v ...) (flog 'TRAC8 v ...))
(define-syntax-rule (flog-trace-9 v ...) (flog 'TRAC9 v ...))

(define-syntax-rule (flog-emergency! v ...) (flog! 'EMERG v ...))
(define-syntax-rule (flog-alert! v ...) (flog! 'ALERT v ...))
(define-syntax-rule (flog-critical! v ...) (flog! 'CRITC v ...))
(define-syntax-rule (flog-error! v ...) (flog! 'ERROR v ...))
(define-syntax-rule (flog-warning! v ...) (flog! 'WARNG v ...))
(define-syntax-rule (flog-notice! v ...) (flog! 'NOTIC v ...))
(define-syntax-rule (flog-info! v ...) (flog! 'INFOR v ...))
(define-syntax-rule (flog-debug! v ...) (flog! 'DEBUG v ...))
(define-syntax-rule (flog-trace-0! v ...) (flog! 'TRAC0 v ...))
(define-syntax-rule (flog-trace-1! v ...) (flog! 'TRAC1 v ...))
(define-syntax-rule (flog-trace-2! v ...) (flog! 'TRAC2 v ...))
(define-syntax-rule (flog-trace-3! v ...) (flog! 'TRAC3 v ...))
(define-syntax-rule (flog-trace-4! v ...) (flog! 'TRAC4 v ...))
(define-syntax-rule (flog-trace-5! v ...) (flog! 'TRAC5 v ...))
(define-syntax-rule (flog-trace-6! v ...) (flog! 'TRAC6 v ...))
(define-syntax-rule (flog-trace-7! v ...) (flog! 'TRAC7 v ...))
(define-syntax-rule (flog-trace-8! v ...) (flog! 'TRAC8 v ...))
(define-syntax-rule (flog-trace-9! v ...) (flog! 'TRAC9 v ...))

(define-syntax-rule (flog-emergency- v ...) (flog- 'EMERG v ...))
(define-syntax-rule (flog-alert- v ...) (flog- 'ALERT v ...))
(define-syntax-rule (flog-critical- v ...) (flog- 'CRITC v ...))
(define-syntax-rule (flog-error- v ...) (flog- 'ERROR v ...))
(define-syntax-rule (flog-warning- v ...) (flog- 'WARNG v ...))
(define-syntax-rule (flog-notice- v ...) (flog- 'NOTIC v ...))
(define-syntax-rule (flog-info- v ...) (flog- 'INFOR v ...))
(define-syntax-rule (flog-debug- v ...) (flog- 'DEBUG v ...))
(define-syntax-rule (flog-trace-0- v ...) (flog- 'TRAC0 v ...))
(define-syntax-rule (flog-trace-1- v ...) (flog- 'TRAC1 v ...))
(define-syntax-rule (flog-trace-2- v ...) (flog- 'TRAC2 v ...))
(define-syntax-rule (flog-trace-3- v ...) (flog- 'TRAC3 v ...))
(define-syntax-rule (flog-trace-4- v ...) (flog- 'TRAC4 v ...))
(define-syntax-rule (flog-trace-5- v ...) (flog- 'TRAC5 v ...))
(define-syntax-rule (flog-trace-6- v ...) (flog- 'TRAC6 v ...))
(define-syntax-rule (flog-trace-7- v ...) (flog- 'TRAC7 v ...))
(define-syntax-rule (flog-trace-8- v ...) (flog- 'TRAC8 v ...))
(define-syntax-rule (flog-trace-9- v ...) (flog- 'TRAC9 v ...))

(define-syntax-rule (flog-emergency!- v ...) (flog!- 'EMERG v ...))
(define-syntax-rule (flog-alert!- v ...) (flog!- 'ALERT v ...))
(define-syntax-rule (flog-critical!- v ...) (flog!- 'CRITC v ...))
(define-syntax-rule (flog-error!- v ...) (flog!- 'ERROR v ...))
(define-syntax-rule (flog-warning!- v ...) (flog!- 'WARNG v ...))
(define-syntax-rule (flog-notice!- v ...) (flog!- 'NOTIC v ...))
(define-syntax-rule (flog-info!- v ...) (flog!- 'INFOR v ...))
(define-syntax-rule (flog-debug!- v ...) (flog!- 'DEBUG v ...))
(define-syntax-rule (flog-trace-0!- v ...) (flog!- 'TRAC0 v ...))
(define-syntax-rule (flog-trace-1!- v ...) (flog!- 'TRAC1 v ...))
(define-syntax-rule (flog-trace-2!- v ...) (flog!- 'TRAC2 v ...))
(define-syntax-rule (flog-trace-3!- v ...) (flog!- 'TRAC3 v ...))
(define-syntax-rule (flog-trace-4!- v ...) (flog!- 'TRAC4 v ...))
(define-syntax-rule (flog-trace-5!- v ...) (flog!- 'TRAC5 v ...))
(define-syntax-rule (flog-trace-6!- v ...) (flog!- 'TRAC6 v ...))
(define-syntax-rule (flog-trace-7!- v ...) (flog!- 'TRAC7 v ...))
(define-syntax-rule (flog-trace-8!- v ...) (flog!- 'TRAC8 v ...))
(define-syntax-rule (flog-trace-9!- v ...) (flog!- 'TRAC9 v ...))


(define-syntax-rule (nflog-emergency v ...) (nflog 'EMERG v ...))
(define-syntax-rule (nflog-alert v ...) (nflog 'ALERT v ...))
(define-syntax-rule (nflog-critical v ...) (nflog 'CRITC v ...))
(define-syntax-rule (nflog-error v ...) (nflog 'ERROR v ...))
(define-syntax-rule (nflog-warning v ...) (nflog 'WARNG v ...))
(define-syntax-rule (nflog-notice v ...) (nflog 'NOTIC v ...))
(define-syntax-rule (nflog-info v ...) (nflog 'INFOR v ...))
(define-syntax-rule (nflog-debug v ...) (nflog 'DEBUG v ...))
(define-syntax-rule (nflog-trace-0 v ...) (nflog 'TRAC0 v ...))
(define-syntax-rule (nflog-trace-1 v ...) (nflog 'TRAC1 v ...))
(define-syntax-rule (nflog-trace-2 v ...) (nflog 'TRAC2 v ...))
(define-syntax-rule (nflog-trace-3 v ...) (nflog 'TRAC3 v ...))
(define-syntax-rule (nflog-trace-4 v ...) (nflog 'TRAC4 v ...))
(define-syntax-rule (nflog-trace-5 v ...) (nflog 'TRAC5 v ...))
(define-syntax-rule (nflog-trace-6 v ...) (nflog 'TRAC6 v ...))
(define-syntax-rule (nflog-trace-7 v ...) (nflog 'TRAC7 v ...))
(define-syntax-rule (nflog-trace-8 v ...) (nflog 'TRAC8 v ...))
(define-syntax-rule (nflog-trace-9 v ...) (nflog 'TRAC9 v ...))

(define-syntax-rule (nflog-emergency! v ...) (nflog! 'EMERG v ...))
(define-syntax-rule (nflog-alert! v ...) (nflog! 'ALERT v ...))
(define-syntax-rule (nflog-critical! v ...) (nflog! 'CRITC v ...))
(define-syntax-rule (nflog-error! v ...) (nflog! 'ERROR v ...))
(define-syntax-rule (nflog-warning! v ...) (nflog! 'WARNG v ...))
(define-syntax-rule (nflog-notice! v ...) (nflog! 'NOTIC v ...))
(define-syntax-rule (nflog-info! v ...) (nflog! 'INFOR v ...))
(define-syntax-rule (nflog-debug! v ...) (nflog! 'DEBUG v ...))
(define-syntax-rule (nflog-trace-0! v ...) (nflog! 'TRAC0 v ...))
(define-syntax-rule (nflog-trace-1! v ...) (nflog! 'TRAC1 v ...))
(define-syntax-rule (nflog-trace-2! v ...) (nflog! 'TRAC2 v ...))
(define-syntax-rule (nflog-trace-3! v ...) (nflog! 'TRAC3 v ...))
(define-syntax-rule (nflog-trace-4! v ...) (nflog! 'TRAC4 v ...))
(define-syntax-rule (nflog-trace-5! v ...) (nflog! 'TRAC5 v ...))
(define-syntax-rule (nflog-trace-6! v ...) (nflog! 'TRAC6 v ...))
(define-syntax-rule (nflog-trace-7! v ...) (nflog! 'TRAC7 v ...))
(define-syntax-rule (nflog-trace-8! v ...) (nflog! 'TRAC8 v ...))
(define-syntax-rule (nflog-trace-9! v ...) (nflog! 'TRAC9 v ...))

(define-syntax-rule (nflog-emergency- v ...) (nflog- 'EMERG v ...))
(define-syntax-rule (nflog-alert- v ...) (nflog- 'ALERT v ...))
(define-syntax-rule (nflog-critical- v ...) (nflog- 'CRITC v ...))
(define-syntax-rule (nflog-error- v ...) (nflog- 'ERROR v ...))
(define-syntax-rule (nflog-warning- v ...) (nflog- 'WARNG v ...))
(define-syntax-rule (nflog-notice- v ...) (nflog- 'NOTIC v ...))
(define-syntax-rule (nflog-info- v ...) (nflog- 'INFOR v ...))
(define-syntax-rule (nflog-debug- v ...) (nflog- 'DEBUG v ...))
(define-syntax-rule (nflog-trace-0- v ...) (nflog- 'TRAC0 v ...))
(define-syntax-rule (nflog-trace-1- v ...) (nflog- 'TRAC1 v ...))
(define-syntax-rule (nflog-trace-2- v ...) (nflog- 'TRAC2 v ...))
(define-syntax-rule (nflog-trace-3- v ...) (nflog- 'TRAC3 v ...))
(define-syntax-rule (nflog-trace-4- v ...) (nflog- 'TRAC4 v ...))
(define-syntax-rule (nflog-trace-5- v ...) (nflog- 'TRAC5 v ...))
(define-syntax-rule (nflog-trace-6- v ...) (nflog- 'TRAC6 v ...))
(define-syntax-rule (nflog-trace-7- v ...) (nflog- 'TRAC7 v ...))
(define-syntax-rule (nflog-trace-8- v ...) (nflog- 'TRAC8 v ...))
(define-syntax-rule (nflog-trace-9- v ...) (nflog- 'TRAC9 v ...))

(define-syntax-rule (nflog-emergency!- v ...) (nflog!- 'EMERG v ...))
(define-syntax-rule (nflog-alert!- v ...) (nflog!- 'ALERT v ...))
(define-syntax-rule (nflog-critical!- v ...) (nflog!- 'CRITC v ...))
(define-syntax-rule (nflog-error!- v ...) (nflog!- 'ERROR v ...))
(define-syntax-rule (nflog-warning!- v ...) (nflog!- 'WARNG v ...))
(define-syntax-rule (nflog-notice!- v ...) (nflog!- 'NOTIC v ...))
(define-syntax-rule (nflog-info!- v ...) (nflog!- 'INFOR v ...))
(define-syntax-rule (nflog-debug!- v ...) (nflog!- 'DEBUG v ...))
(define-syntax-rule (nflog-trace-0!- v ...) (nflog!- 'TRAC0 v ...))
(define-syntax-rule (nflog-trace-1!- v ...) (nflog!- 'TRAC1 v ...))
(define-syntax-rule (nflog-trace-2!- v ...) (nflog!- 'TRAC2 v ...))
(define-syntax-rule (nflog-trace-3!- v ...) (nflog!- 'TRAC3 v ...))
(define-syntax-rule (nflog-trace-4!- v ...) (nflog!- 'TRAC4 v ...))
(define-syntax-rule (nflog-trace-5!- v ...) (nflog!- 'TRAC5 v ...))
(define-syntax-rule (nflog-trace-6!- v ...) (nflog!- 'TRAC6 v ...))
(define-syntax-rule (nflog-trace-7!- v ...) (nflog!- 'TRAC7 v ...))
(define-syntax-rule (nflog-trace-8!- v ...) (nflog!- 'TRAC8 v ...))
(define-syntax-rule (nflog-trace-9!- v ...) (nflog!- 'TRAC9 v ...))

(export-syntax flog-emergency flog-alert flog-critical flog-error flog-warning flog-notice flog-info flog-debug flog-trace-0 flog-trace-1 flog-trace-2 flog-trace-3 flog-trace-4 flog-trace-5 flog-trace-6 flog-trace-7 flog-trace-8 flog-trace-9 flog-emergency!  flog-alert!  flog-critical!  flog-error!  flog-warning!  flog-notice!  flog-info!  flog-debug!  flog-trace-0!  flog-trace-1!  flog-trace-2!  flog-trace-3!  flog-trace-4!  flog-trace-5!  flog-trace-6!  flog-trace-7!  flog-trace-8!  flog-trace-9!  flog-emergency- flog-alert- flog-critical- flog-error- flog-warning- flog-notice- flog-info- flog-debug- flog-trace-0- flog-trace-1- flog-trace-2- flog-trace-3- flog-trace-4- flog-trace-5- flog-trace-6- flog-trace-7- flog-trace-8- flog-trace-9- flog-emergency!- flog-alert!- flog-critical!- flog-error!- flog-warning!- flog-notice!- flog-info!- flog-debug!- flog-trace-0!- flog-trace-1!- flog-trace-2!- flog-trace-3!- flog-trace-4!- flog-trace-5!- flog-trace-6!- flog-trace-7!- flog-trace-8!- flog-trace-9!- nflog-emergency nflog-alert nflog-critical nflog-error nflog-warning nflog-notice nflog-info nflog-debug nflog-trace-0 nflog-trace-1 nflog-trace-2 nflog-trace-3 nflog-trace-4 nflog-trace-5 nflog-trace-6 nflog-trace-7 nflog-trace-8 nflog-trace-9 nflog-emergency!  nflog-alert!  nflog-critical!  nflog-error!  nflog-warning!  nflog-notice!  nflog-info!  nflog-debug!  nflog-trace-0!  nflog-trace-1!  nflog-trace-2!  nflog-trace-3!  nflog-trace-4!  nflog-trace-5!  nflog-trace-6!  nflog-trace-7!  nflog-trace-8!  nflog-trace-9!  nflog-emergency- nflog-alert- nflog-critical- nflog-error- nflog-warning- nflog-notice- nflog-info- nflog-debug- nflog-trace-0- nflog-trace-1- nflog-trace-2- nflog-trace-3- nflog-trace-4- nflog-trace-5- nflog-trace-6- nflog-trace-7- nflog-trace-8- nflog-trace-9- nflog-emergency!- nflog-alert!- nflog-critical!- nflog-error!- nflog-warning!- nflog-notice!- nflog-info!- nflog-debug!- nflog-trace-0!- nflog-trace-1!- nflog-trace-2!- nflog-trace-3!- nflog-trace-4!- nflog-trace-5!- nflog-trace-6!- nflog-trace-7!- nflog-trace-8!- nflog-trace-9!- )




; ;;macro per gestire strutture nidificate json
; (define-syntax-rule (json-get json-struct key ...)
;   (lens::focus (lens::in key ...) json-struct))
; ;;
; (define-syntax-rule (json-put json-struct value key ...)
;   (lens::put (lens::in key ...) value json-struct))

; (export json-get json-put)

;;La gestione dei token a standard JWT 
(define-public (mtfa-make-jwt info duration)
  "info in json format and duration, in seconds"
  (let* ((header '(("alg" . "HS256") ("typ" . "JWT")))
	 (expiration (+ duration (seconds)))
	 (payload (json-make info expiration))
	 (b64-header (mtfa-b64-url-encode (scm->json-string header)))
	 (b64-payload (mtfa-b64-url-encode (scm->json-string payload)))
	 (signature (mtfa-b64-url-encode (mtfa-hmac 2 (string-append b64-header "." b64-payload) "AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"))))
    (string-append b64-header "." b64-payload "." (mtfa-b64-url-encode signature))))
;;
(define-public (mtfa-verify-jwt jwt)
  "the token received by the client"
  (let ((spl (string-split jwt #\.)))
    (receive (b64-header b64-payload b64-signature) (apply values spl)
      ;;verifico la firma
      (let* ((new-b64-signature (mtfa-b64-url-encode (mtfa-hmac 2 (string-append b64-header "." b64-payload) "AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow")))
	     (json (json-string->scm (mtfa-b64-url-decode-s b64-payload)))
	     (info (assoc-ref json "info"))
	     (expiration (assoc-ref json "expiration")))
	(and (string<> new-b64-signature b64-signature)
	     (> expiration (seconds))
	     info)))))


#|
(define-public (mtfa-wss-make cb-for-payload)
  "La callback è chiamata quando c'è un payload nei dati ricevuti da rete. I dati ricevuti da rete, incapsulati nel protocolo websocket, sono pushed utilizzando la mtfa-wss-io"
  (mtfa_wss_make cb-for-payload))
(define-public (mtfa-wss-p wss)
  "per verificare che sia effettivamente un wss"
  (mtfa_wss_p wss))
(define-public (mtfa-wss-close wss)
  "fa la close del websocket"
  (mtfa_wss_close wss))
(define-public (mtfa-wss-push wss bv-recvd-from-wsclient)
  "per inviare i dati codificati wss e farli decodificare. Quando trova un payload, chiama la callback cb-for-payload. Torna i dati, se ci sono, da inviare sulla rete"
  (mtfa_wss_push wss bv-recvd-from-wsclient))
(define-public (mtfa-wss-pop wss)
  "per ricevere dati da inviare al client"
  (mtfa_wss_pop wss))
(define-public (mtfa-wss-prepare-to-send wss payload-to-send)
  "incapsula i dati grezzi nel protocollo websocket e torna i dati da inviare sulla rete."
  (mtfa_wss_prepare_to_send wss payload-to-send))
|#

;; LE COROUTINES!!!
(define-public (make-coroutine proc . args)
  (define tag (make-prompt-tag))
  (define (abort-handler cont ret)
    (define* (resume #:optional arg)
      (call-with-prompt
	  tag
	(lambda () (cont arg))
	abort-handler))
    (values resume ret))
  ;; 'arg' is ignored - it is provided only for consistency with the interface of resume
  (lambda* (#:optional arg)
    (call-with-prompt
	tag
      (lambda ()
	(values #f
		(apply proc
		       (lambda* (#:optional arg)
			 (abort-to-prompt tag arg))
		       args)))
      abort-handler)))
