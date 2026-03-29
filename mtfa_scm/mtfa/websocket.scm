(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa websocket)
  #:use-module  (mtfa error-handler)
  #:use-module  (mtfa utils)
  #:use-module  (mtfa error-handler)
  #:use-module  (mtfa serializer)
  #:use-module  (mtfa unordered-set) ;;unordered set con chiavi (stringhe, numeri o sumimboli: tutto convertito in stringa). Persistente
  #:use-module  (mtfa unordered-map) ;;unordered map con cpolyhiavi (stringhe) e valori (qualsiasi cosa). persistente
  #:use-module  (mtfa star-map)      ;;Inse2drisce stringhe con o senza jolly, la stringa che definisce il jolly e il valore. Cerca le stringhe che matchano!
  #:use-module  (mtfa simple_db)
  #:use-module  (mtfa eis)
  ;;(mtfa fsm)
  #:use-module  (mtfa va)
  #:use-module  (mtfa extset)  ;;gestisce insiemi i cui elementi sono stringhe! consente operazioni di clone, set, check, get all.... Definisce una macro che consente di creare "al volo" una sottoclasse le cui istanze condividono gli stessi elementi.
  #:use-module  (mtfa umset)   ;;è una unordered map (non persistente) che ha stringhe come chiavi e ha insiemi di stringhe come valori. Ogni insert aggiunge all'insieme corrispondente. Definisce inoltre la mtfa-umap-list che consente di mappare liste come chiavi e qualsiasi valore come valore
  #:use-module  (mtfa web)
  #:use-module  (mtfa brg)
  #:use-module  (mtfa avl)
  #:use-module  (mtfa eqt)
  #:use-module  (mtfa opencv)
  ;;
;;  #:use-module  (pfds sets)
  ;;
  #:use-module  (gnutls)
  ;;
  ;;La libreria guile lib
  #:use-module  (logging logger)
  #:use-module  (logging rotating-log)
  #:use-module  (logging port-log)
  #:use-module  (scheme kwargs)
  #:use-module  (search basic)
  #:use-module  (math primes)
  #:use-module  (match-bind)
  #:use-module  (graph topological-sort)
  ;;
  ;;i moduli di guile
  ;;((rnrs records syntactic) #:prefix rnrs::)
  #:use-module  (rnrs bytevectors)
  #:use-module  (rnrs arithmetic bitwise)
  ;; ((rnrs io ports)
  ;;  #:select (string->bytevector bytevector->string)
  ;;  #:prefix ioports::)
  ;;((rnrs) :version (6))
  ;;
  #:use-module  (srfi srfi-1)
  #:use-module  (srfi srfi-9)
  #:use-module  (srfi srfi-9 gnu)
  #:use-module  (srfi srfi-11)
  #:use-module  ((srfi srfi-18)
		 #:prefix srfi-18::) ;;thread e mutex
  ;; date & time rinomina per avere un current time che non si sovrappone
  #:use-module  (srfi srfi-19)
  #:use-module  (srfi srfi-26)
  ;;(srfi srfi-28)
  #:use-module  (srfi srfi-41) ;;streams
  #:use-module  (srfi srfi-42) ;;Eager Comprehensions
  #:use-module  (srfi srfi-43)
  #:use-module  (srfi srfi-45)
  #:use-module  (srfi srfi-60)
  #:use-module  (srfi srfi-111)  ;;Boxes
  #:use-module  (srfi srfi-171)
  ;;
  #:use-module  (web uri)
  #:use-module  (system foreign)
  ;;
  #:use-module  (ice-9 format)
  #:use-module  (ice-9 ftw)
  #:use-module  (ice-9 rdelim)
  #:use-module  (ice-9 pretty-print)
  #:use-module  (ice-9 regex)
  #:use-module  (ice-9 iconv)
  #:use-module  (ice-9 string-fun)
  #:use-module  (ice-9 peg)
  #:use-module  (ice-9 peg string-peg)
  #:use-module  (ice-9 vlist)
  #:use-module  (ice-9 q)
  #:use-module  (ice-9 binary-ports)
  #:use-module  (ice-9 textual-ports)
  #:use-module  (ice-9 threads)
  #:use-module  (ice-9 hash-table)
  #:use-module  (ice-9 control)
  #:use-module  (ice-9 match)
  #:use-module  (ice-9 receive)
  #:use-module  (ice-9 eval-string)
  #:use-module  (ice-9 textual-ports)
  #:use-module  (ice-9 arrays)
  #:use-module  (ice-9 popen)
  #:use-module  (ice-9 exceptions)
  #:use-module  (ice-9 optargs)
  ;;
  #:use-module  (oop goops)
  #:use-module  (oop goops describe)
  ;; (sxml simple)
  ;; (sxml ssax)
  ;; (sxml xpath)
  #:use-module  (json)
  #:use-module  (system syntax)
  #:use-module  (system foreign)
  ;;
  ;; ((fibers web server) #:prefix fibers::)
  ;; (fibers)
  ;; (fibers operations)
  ;; (fibers channels)
  ;; (fibers timers)
  ;; (fibers conditions)
  ;; (fibers io-wakeup)
  ;; (fibers scheduler)
  ;;
  #:use-module  (web server)
  #:use-module  (web request)
  #:use-module  (web response)
  #:use-module  (web uri)
  ;;
  #:use-module  (web client)
  ;;
  #:use-module  (mtfa base64)
  #:use-module  (mtfa sha-1)
  )

;; (mtfa-rand-seed (mtfa-micros))
;;
;;
(define (HexDump data)
  (when (string? data)
    (set! data (string->bytevector data "ISO-8859-1")))
  )
;;
(define (ToBV s)
  (if (bytevector? s)
      s
      (string->bytevector s "ISO-8859-1")))
(define (ToS s)
  (if (string? s)
      s
      (bytevector->string s "ISO-8859-1")))
(define (ReverseString s)
  (list->string (reverse (string->list s))))
(define (ReverseBv bv)
  ;; (Show! "Entering ReverseBv")
  (let ((v (u8-list->bytevector (reverse (bytevector->u8-list bv)))))
    ;; (Show! "leabving ReverseBv")
    v))
;;
;;Manipolazione di bytevector
(define* (bv-append-slow . bvl) (u8-list->bytevector (apply append (map bytevector->u8-list bvl))))
(define* (AppendBV . bvl) (let loop ((bbv (make-bytevector (fold (lambda (c p) (+ p (bytevector-length c))) 0 bvl)))
				      (bvl bvl) (p 0))
			     (if (null? bvl)
				 bbv
				 (let* ((bv (car bvl)) (len-bv (bytevector-length bv)))
				   (bytevector-copy! bv 0 bbv p len-bv)
				   (loop bbv (cdr bvl) (+ p len-bv))))))
(define (TakeBV bv start len) (let ((bvout (make-bytevector len)))
				 (bytevector-copy! bv start bvout 0 len)
				 bvout))
(define (SplitBV bv len)
  ;; (Show! "Entering split")
  (let ((bv1 (make-bytevector len))
	(bv2 (make-bytevector (- (bytevector-length bv) len))))
    (bytevector-copy! bv 0 bv1 0 len)
    (bytevector-copy! bv len bv2 0 (- (bytevector-length bv) len))
    ;; (Show! "Leaving split")
    (values bv1 bv2)))
;;
;; ;; LE COROUTINES!!!
;; (define (make-coroutine proc . args)
;;   (define tag (make-prompt-tag))
;;   (define (abort-handler cont ret)
;;     (define* (resume #:optional arg)
;;       (call-with-prompt
;; 	  tag
;; 	(lambda () (cont arg))
;; 	abort-handler))
;;     (values resume ret))
;;   ;; 'arg' is ignored - it is provided only for consistency with the interface of resume
;;   (lambda* (#:optional arg)
;;     (call-with-prompt
;; 	tag
;;       (lambda ()
;; 	(values #f
;; 		(apply proc
;; 		       (lambda* (#:optional arg)
;; 			 (abort-to-prompt tag arg))
;; 		       args)))
;;       abort-handler)))
;;
;;Costruzione di una porta da un bytevector e gestione degli append sui dati
(define (make-bytevector-input-port bv-source name)
  (let ((bv-source (bytevector-copy bv-source)))
    (define position 0)
    (define length (bytevector-length bv-source))
    (define save-position #f)
    (define save-length #f)
    (define (read! bv start count)
      ;;(Show! "Called read! from " start " per " count " bytes")
      (let ((count (min count (- length position))))
	(bytevector-copy! bv-source position bv start count)
	(set! position (+ position count))
	count))
    (define (get-position)
      ;;(Show! "Called get-position: " position)
      position
      )
    (define (set-position! new-position)
      ;;(Show! "Called set-position!: " new-position)
      (set! position new-position))
    (lambda (cmd . pars)
      (match (cons cmd pars)
	(('dump)
	 (Show! "THIS IS A DUMP" "\n" "\tPosition: " position "\n"
		"\tLength: " length "\n"
		"\tData: " (list-tail (bytevector->u8-list bv-source) position)))
	(('open) (make-custom-binary-input-port name read! get-position set-position! #f))
	(('append bv)
	 (let* ((len-bv-source (bytevector-length bv-source))
		(len-bv (bytevector-length bv))
		(len-tot (- (+ len-bv-source len-bv) position))
		(bvnew (make-bytevector len-tot)))
	   (bytevector-copy! bv-source position bvnew 0 (- len-bv-source position))
	   (bytevector-copy! bv 0 bvnew (- len-bv-source position) len-bv)
	   (set! bv-source bvnew)
	   (set! length len-tot)
	   (set! position 0)))
	(('restart)
	 (set! position 0))
	(('get-length)(- length position))
	(('save)
	 (set! save-position position)
	 (set! save-length length))
	(('restore)
	 (set! position save-position)
	 (set! length save-length))
	(('consolidate)
	 (let* ((len-new-bv-source (- (bytevector-length bv-source) position))
		(new-bv-source (make-bytevector len-new-bv-source)))
	   (bytevector-copy! bv-source position new-bv-source 0 len-new-bv-source)
	   (set! bv-source new-bv-source)
	   (set! length len-new-bv-source)
	   (set! position 0)))))))
;;
;;
;;Invia su libevent buffer!!!
(define (SendData pb data)
  (if (va::vaeng::CheckIfDeallocated pb)
      (begin
	;;lo dovrebbe togliere toglie dall'hash
	#f
	)
      (begin
	;;(Show! "SendData: " pb " - " (va::vaeng::CheckIfDeallocated pb))
	(mtfa-eis-enable-pbuf pb '() #t)
	(mtfa-eis-put-raw-data pb data)
	#t
	)))
;;
(define-public (EnableWebSocket service-name handler)
  ;;
  ;;I dati relativi alle continuations aborted to prompt
  (define um (make-hash-table))	;;chiave pb, valore la continuation
  ;; See section 1.3 - Opening Handshake
  (define %handshake-guid "258EAFA5-E914-47DA-95CA-C5AB0DC85B11")
  ;;
  (define (make-accept-key client-key)
    "Return a WebSocket accept key based on CLIENT-KEY, a base64 encoded string."
    (base64-encode
     (sha-1->bytevector
      (sha-1
       (string->utf8
	(string-append client-key %handshake-guid))))))
  ;;
  (define (make-handshake-response client-key)
    "Return an HTTP response object for upgrading to a WebSocket
connection for the client whose key is CLIENT-KEY, a base64 encoded
string."
    ;; See section 4.2.2.
    (let ((accept-key (make-accept-key (string-trim-both client-key))))
      (build-response #:code 101
                      #:headers `((upgrade . ("websocket"))
                                  (connection . (upgrade))
                                  (sec-websocket-accept . ,accept-key)))))
  ;;
  ;;La coroutine che usa i dati e implementa il protocollo
  (define serve-client (make-coroutine
			(lambda (yield)
			  (let* ((bvinport (make-bytevector-input-port (make-bytevector 0) service-name))
				 (inport (bvinport 'open))
				 (dummy (setvbuf inport 'none))
				 (pb #f)
				 (data-in #f)
				 (request #f)
				 (frame #f))
			    ;;
			    (letrec-syntax ((return-to-caller
					     (syntax-rules ()
					       ((return-to-caller data)
						(let ((pb-data-in (yield data)))
						  (set! pb (car pb-data-in))
						  (set! data-in (cdr pb-data-in))
						  (bvinport 'append data-in)))))
					    (continue-until-ok
					     (syntax-rules ()
					       ((continue-until-ok exp ...)
						(begin
						  ;; (bvinport 'consolidate)
						  (let loop ((ok #f))
						    (unless ok
						      (set! ok (false-if-exception
								(begin
								  ;;(Show! "in false-if-ex: " (bvinport 'get-length))
								  exp
								  ...))))
						    ;;(Show! "ok is: " ok)
						    (unless ok
						      ;;(bvinport 'restart)
						      (return-to-caller 'st-need-more-data)
						      (loop ok))
						    ;;(bvinport 'consolidate)
						    ok))))))
			      ;;
			      ;;
			      ;;LE ROUTINES PER I FRAME!!!
			      ;;
			      ;;
			      ;; WebSocket frames
			      ;;
			      (define-record-type <frame>
				(make-frame final? type masking-key data)
				frame?
				(final? frame-final?)
				(type frame-type)
				(masking-key frame-masking-key)
				(data frame-data))
			      ;;
			      (define (display-frame frame port)
				(Show! "Called display-frame")
				(format port "#<frame final?: ~a type: ~a masking-key: ~a length: ~d>"
					(frame-final? frame)
					(frame-type frame)
					(frame-masking-key frame)
					(frame-length frame)))
			      ;;
			      (set-record-type-printer! <frame> display-frame)
			      ;;
			      (define* (make-ping-frame bv #:optional masking-key)
				"Return a \"ping\" control frame containing the contents of the
bytevector BV, masked with MASKING-KEY.  By default, the data is
unmasked."
				(make-frame #t 'ping masking-key bv))
			      ;;
			      (define* (make-pong-frame bv #:optional masking-key)
				"Return a \"pong\" control frame containing the contents of the
bytevector BV, masked with MASKING-KEY.  By default, the data is
unmasked."
				(make-frame #t 'pong masking-key bv))
			      ;;
			      (define* (make-close-frame bv #:optional masking-key)
				"Return a \"close\" control frame containing the contents of the
bytevector BV, masked with MASKING-KEY.  By default, the data is
unmasked."
				(make-frame #t 'close masking-key bv))
			      ;;
			      (define* (make-text-frame text #:optional masking-key)
				"Return a text data frame containing the string TEXT, masked with MASKING-KEY.
By default, the text is unmasked."
				(make-frame #t 'text masking-key (string->utf8 text)))
			      ;;
			      (define* (make-binary-frame bv #:optional masking-key)
				"Return a binary data frame containing the contents of the
bytevector BV, masked with MASKING-KEY.  By default, the data is
unmasked."
				(make-frame #t 'binary masking-key bv))
			      ;;
			      (define (continuation-frame? frame)
				"Return #t if FRAME is a continuation frame."
				(eq? (frame-type frame) 'continuation))
			      ;;
			      (define (text-frame? frame)
				"Return #t if FRAME is a text frame."
				(eq? (frame-type frame) 'text))
			      ;;
			      (define (binary-frame? frame)
				"Return #t if FRAME is a binary frame."
				(eq? (frame-type frame) 'binary))
			      ;;
			      (define (close-frame? frame)
				"Return #t if FRAME is a close frame."
				(eq? (frame-type frame) 'close))
			      ;;
			      (define (ping-frame? frame)
				"Return #t if FRAME is a ping frame."
				(eq? (frame-type frame) 'ping))
			      ;;
			      (define (pong-frame? frame)
				"Return #t if FRAME is a pong frame."
				(eq? (frame-type frame) 'pong))
			      ;;
			      ;; See section 5.4 - Fragmentation
			      (define (fragment-frame? frame)
				"Return #t if FRAME is an incomplete message."
				(or (continuation-frame? frame)
				    (not (frame-final? frame))))
			      ;;
			      (define (first-fragment-frame? frame)
				"Return #t if FRAME is the first piece of a fragmented message."
				(and (not (frame-final? frame))
				     (data-frame? frame)))
			      ;;
			      (define (final-fragment-frame? frame)
				"Return #t if FRAME is the final piece of a fragmented message."
				(and (frame-final? frame)
				     (continuation-frame? frame)))
			      ;;
			      ;; See section 5.5 - Control Frames
			      (define (control-frame? frame)
				"Return #t if FRAME is a control frame."
				(or (close-frame? frame)
				    (ping-frame? frame)
				    (pong-frame? frame)))
			      ;;
			      ;; See section 5.6 - Data Frames
			      (define (data-frame? frame)
				"Return #t if FRAME is a data frame."
				(or (text-frame? frame)
				    (binary-frame? frame)))
			      ;;
			      (define (frame-length frame)
				"Return the length of the data bytevector in FRAME."
				(bytevector-length (frame-data frame)))
			      ;;
			      (define (text-frame->string frame)
				"Convert FRAME, an unfragmented text frame, into a string."
				(utf8->string (frame-data frame)))
			      ;;
			      (define (frame-concatenate frames)
				"Concatenate the data in FRAMES, a list of fragmented frames, into a
single bytevector."
				(let ((bv (make-bytevector (reduce + 0 (map frame-length frames)))))
				  (let loop ((frames frames)
					     (offset 0))
				    (match frames
				      (() bv)
				      ((frame . rest)
				       (let ((length (frame-length frame)))
					 (bytevector-copy! (frame-data frame) 0 bv offset length)
					 (loop rest (+ offset length))))))))
			      ;;
			      (define (text-frames->string frames)
				"Convert FRAMES, a list of fragmented text frames, into a single
concatenated string."
				(utf8->string (frame-concatenate frames)))
			      ;;
			      ;; (define (call-with-input-bytevector bv proc)
			      ;;   "Call PROC with one argument: an open input port that reads from the
			      ;; bytevector BV."
			      ;;   (let ((port (open-bytevector-input-port bv)))
			      ;;     (dynamic-wind
			      ;;       (const #t)
			      ;;       (lambda ()
			      ;;         (proc port))
			      ;;       (lambda ()
			      ;;         (close-port port)))))
			      ;;
			      (define (close-frame->status frame)
				"Convert FRAME, a close frame, into a pair.  The \"car\" of the pair
is a positive integer status code, and the \"cdr\" is a string
containing the explanation, if present."
				(define (read-status-code)
				  (match (bytevector-sint-ref (frame-data frame) 0 (endianness big) 2)
				    ;; See section 7.4
				    ((and (or 1005 1006 1015) status)
				     (websocket-error "invalid use of reserved status code: " status))
				    (status status)))
				;;
				(let ((length (frame-length frame)))
				  (cond
				   ((zero? length) ; unspecified closing status
				    '(1005 . ""))
				   ((= length 2) ; status code only
				    (cons (read-status-code) ""))
				   (else ; status + reason
				    (cons (read-status-code)
					  (call-with-input-bytevector (frame-data frame)
								      (lambda (port)
									;; Throw away the status code.
									(get-u8 port)
									(get-u8 port)
									;; Now read the reason.
									(read-string port))))))))
			      ;;
			      ;; Frame reader
			      ;;
			      ;; See section 5.3 - Client-to-Server Masking
			      (define (mask-bytevector! bv masking-key)
				"Apply the WebSocket masking algorithm to the bytevector BV using
MASKING-KEY."
				(let loop ((i 0))
				  (when (< i (bytevector-length bv))
				    (let ((masked (logxor (u8vector-ref bv i)
							  (u8vector-ref masking-key (modulo i 4)))))
				      (u8vector-set! bv i masked)
				      (loop (1+ i))))))
			      ;;
			      (define (websocket-error message . args)
				(apply error message args))
			      ;;
			      ;; See section 5.2 - Base Framing Protocol
			      (define (read-frame bvinport port) ;; port
				(define (opcode->frame-type opcode)
				  (match opcode
				    (#x0 'continuation)
				    (#x1 'text)
				    (#x2 'binary)
				    (#x8 'close)
				    (#x9 'ping)
				    (#xA 'pong)
				    (else
				     (Show! "Opcode non valido: " opcode)
				     (websocket-error "invalid opcode: " opcode))))
				;;
				(define (control-frame? type)
				  (memq type '(close ping pong)))
				;;
				(define (parse-fin-bit octet)
				  ;; Test the first bit of the octet.
				  (not (zero? (logand #x80 octet))))
				;;
				(define (parse-opcode octet final?)
				  ;; The opcode is stored in the least significant nibble of the
				  ;; octet.
				  (let ((type (opcode->frame-type (logand #x0f octet))))
				    ;; Section 5.5 specifies that control frames must not be
				    ;; fragmented.
				    (when (and (not final?) (control-frame? type))
				      ;;(Show! "Errore: fragmented control frame")
				      (websocket-error "fragmented control frame: " type))
				    type))
				;;
				(define (parse-mask-bit octet)
				  (not (zero? (logand #x80 octet))))
				;;
				(define (parse-length octet)
				  ;; For lengths <= 125, the frame length is encoded in the last 7
				  ;; bits of the octet.  If this number is 126, then the true length
				  ;; is encoded in the following 16 bits.  If the number is 127,
				  ;; then the true length is encoded in the following 64 bits.
				  (match (logand #x7f octet)
				    (126
				     ;;Mi servono due byte, se non ci sono, attendo!!
				     ;;(Show! "A-Ci sono 2 byte nel frame? " (bvinport 'get-length))
					;,(bvinport 'dump)
				     (continue-until-ok (>= (bvinport 'get-length) 2))
				     ;;(bvinport 'dump)
				     (bytevector-u16-ref (get-bytevector-n port 2) 0 (endianness big)))
				    (127
				     ;;Mi servono 8 byte, se non ci sono, attendo!!
				     ;;(Show! "B-Ci sono 8 byte nel frame? " (bvinport 'get-length))
				     ;;(bvinport 'dump)
				     (continue-until-ok (>= (bvinport 'get-length) 8))
				     ;;(bvinport 'dump)
				     (bytevector-u64-ref (get-bytevector-n port 8) 0 (endianness big)))
				    (length length)))
				;;
				(define (parse-masking-key)
				  ;; Masking keys are always 32 bits.
				  ;;(Show! "C-Ci sono 4 byte nel frame? " (bvinport 'get-length))
				  (continue-until-ok (>= (bvinport 'get-length) 4))
				  (get-bytevector-n port 4))
				;;
				(define (read-data type masking-key length)
				  ;; Section 5.5 specifies that control frame bodies may not exceed
				  ;; 125 bytes.
				  ;;(Show! "Start to read data..." type " - " masking-key " - " length)
				  (when (and (> length 125)
					     (control-frame? type))
				    (websocket-error "control frame too large: " type length))

				  ;;(Show! "D-Ci sono  " length " byte nel frame? " (bvinport 'get-length))
				  (continue-until-ok (>= (bvinport 'get-length) length))
				  ;;(Show! "Letti: " (bvinport 'get-length) " byte")
				  (let ((bv (get-bytevector-n port length)))
				    ;;(Show! "masking-key: " masking-key ". bv: " bv)
				    (when masking-key
				      ;;(Show! "applica la mask")
				      (mask-bytevector! bv masking-key))
				    ;;(Show! "Esce da read-data con bv: " bv)
				    bv))
				;;
				;;INIZIA LA READ-FRAME
				;;(Show! "Ci sono 2 byte nel frame? " (bvinport 'get-length))
				(continue-until-ok (>= (bvinport 'get-length) 2))
				;;(Show! "E ora ci sono 2 byte nel frame? " (bvinport 'get-length))
				;;(bvinport 'dump)
				(let* (	;; (dummy (Show! "1-E ora quanti byte nel frame? " (bvinport 'get-length)))
				       (type-byte (get-u8 port))
				       ;; (dummy (Show! "2-E ora quanti byte nel frame? " (bvinport 'get-length)))
				       (length-byte (get-u8 port))
				       ;; (dummy (Show! "3-E ora quanti byte nel frame? " (bvinport 'get-length)))
				       (final? (parse-fin-bit type-byte))
				       ;; (dummy (Show! "type" type-byte ", " final?))
				       (type (parse-opcode type-byte final?))
				       ;; (dummy (Show! "mask?"))
				       (mask? (parse-mask-bit length-byte))
				       ;; (dummy (Show! "length"))
				       (length (parse-length length-byte))
				       ;; (dummy (Show! "masking-key"))
				       (masking-key (and mask? (parse-masking-key)))
				       ;; (dummy (Show! "data"))
				       (data (read-data type masking-key length)))
				  (make-frame final? type masking-key data)))
			      ;;
			      ;;Qui terminano le routine sulla lettura dei frame.
			      ;;Ora la scrittura
			      ;;
			      ;; Frame writer
			      ;;
			      (define* (write-frame frame #:optional (port (current-output-port)))
				;; Packs an unsigned integer into a bytevector in network byte
				;; order.
				(define (uint->bytevector n size)
				  (uint-list->bytevector (list n) (endianness big) size))
				;;
				(define (masked-data mask data)
				  (let ((bv (bytevector-copy data)))
				    (mask-bytevector! bv mask)
				    bv))
				;;
				(let ((length (frame-length frame))
				      (mask   (frame-masking-key frame))
				      (data   (frame-data frame)))
				  ;; Write FIN bit and opcode.
				  (put-u8 port
					  (logior (if (frame-final? frame) #x80 #x00)
						  (match (frame-type frame)
						    ('continuation #x00)
						    ('text         #x01)
						    ('binary       #x02)
						    ('close        #x08)
						    ('ping         #x09)
						    ('pong         #x0A))))
				  ;;
				  ;; Write mask bit and length.
				  (put-u8 port
					  (logior (if mask #x80 #x00)
						  (cond
						   ((< length 126) length)
						   ((< length (expt 2 16)) 126)
						   (else 127))))
				  ;;
				  ;; Write true size when size is greater than 125.
				  (cond
				   ((< length 126) #f)
				   ((< length (expt 2 16))
				    (put-bytevector port (uint->bytevector length 2)))
				   (else
				    (put-bytevector port (uint->bytevector length 8))))
				  ;;
				  ;; Write masking key, if present.
				  (when mask (put-bytevector port mask))
				  ;;
				  ;; Write data, potentially masked.
				  (put-bytevector port (if mask (masked-data mask data) data))))
			      ;;
			      ;;Qui inizia il codice di gestione del protocollo WS
			      ;;
			      (define (send-msg pb msg)
				(let ((response (cond
						 ((string? msg)
						  (make-text-frame msg))
						 ((bytevector? msg)
						  (make-binary-frame msg))
						 (#t #f))))
				  (when response
				    (let ((bvp (call-with-output-bytevector
						(lambda (p)
						  (write-frame response p)))))
				      (SendData pb bvp)))))
			      ;;
			      (define (handle-data-frame type data)
				(let* ((result   (handler
						  (lambda (pb) (hash-ref um pb)) ;;check se un pb è ancora attivo
						  (lambda (pb msg) (send-msg pb msg))
						  pb
						  (match type
						    ('text   (utf8->string data))
						    ('binary data)))))
				  (send-msg pb result)))
			      ;;      (response (cond
			      ;; 		  ((string? result)
			      ;; 		   (make-text-frame result))
			      ;; 		  ((bytevector? result)
			      ;; 		   (make-binary-frame result))
			      ;; 		  ((not result)
			      ;; 		   #f))))

			      ;; (when response
			      ;;   (let ((bvp (call-with-output-bytevector
			      ;; 		(lambda (p)
			      ;; 		  (write-frame response p)))))
			      ;;     ;;(Show! "Kind of output frame: " (mtfa-typeof bvp))
			      ;;     ;;(Show! "Content of output frame: " bvp)
			      ;;     (mtfa-eis-put-raw-data pb bvp)))))
			      ;;
			      (define (read-frame-maybe)
				;;se non ci sono dati, li prende dalla rete, altrimenti prova a utilizzarli.
				;;In caso di errore, allora ricomincia prendendoli sempre dalla rete
				;;La port-eof non ci serve poiché quando non ci sono più dati butta via la continuation
				(read-frame bvinport inport))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;; Q U I   I N I Z I A N O   I   G I O C H I ;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			      ;;
			      ;; Perform the HTTP handshake and upgrade to WebSocket protocol.
			      ;;se non ci sono dati a sufficienza, attende i dati
			      ;;
			      ;;prima chiamata. torna immediatamente poichè siamo nella inizializzazione
			      (return-to-caller 'st-started) ;;I dati sono appesi alla porta corrente
			      ;;
			      (continue-until-ok (set! request (read-request inport))) ;;Qui legge l'header della richiesta
			      ;;
			      (let* ((client-key (assoc-ref (request-headers request) 'sec-websocket-key))
				     (response (make-handshake-response client-key))
				     (bvresponse (call-with-output-bytevector (lambda (p) (write-response response p)))))
				;;
				;;Invia la risposta al client. Scrive la risposta su un bytevector
				;; (Show! "Response: " (bytevector->string bvresponse "ISO-8859-1"))
				(SendData pb bvresponse)
				;; (write-response response client-socket)
				(let loop ((fragments '())
					   (type #f))
				  ;;
				  (let ((frame (read-frame-maybe)))
				    (cond
				     ;; EOF - port is closed.
				     ((not frame)
				      ;;Chiudere la porta!!
				      (yield 'st-ended)
				      ;; (close-port client-socket)
				      )
				     ;; Per section 5.4, control frames may appear interspersed
				     ;; along with a fragmented message.
				     ((close-frame? frame)
				      ;; Per section 5.5.1, echo the close frame back to the
				      ;; client before closing the socket.  The client may no
				      ;; longer be listening.
				      (false-if-exception
				       (write-frame (make-close-frame (frame-data frame)) inport)) ;; client-socket
				      ;; (close-port client-socket)
				      (yield 'st-ended))
				     ((ping-frame? frame)
				      ;; Per section 5.5.3, a pong frame must include the exact
				      ;; same data as the ping frame.
				      (write-frame (make-pong-frame (frame-data frame)) inport) ;;client-socket
				      (loop fragments type))
				     ((pong-frame? frame) ; silently ignore pongs
				      (loop fragments type))
				     ((first-fragment-frame? frame) ; begin accumulating fragments
				      (loop (list frame) (frame-type frame)))
				     ((final-fragment-frame? frame) ; concatenate all fragments
				      (handle-data-frame type (frame-concatenate (reverse fragments)))
				      (loop '() #f))
				     ((fragment-frame? frame) ; add a fragment
				      (loop (cons frame fragments) type))
				     ((data-frame? frame) ; unfragmented data frame
				      ;; (Show! "handle the data frame")
				      ;;(bvinport 'dump)
				      (handle-data-frame (frame-type frame) (frame-data frame))
				      (loop '() #f)))))))))))
  ;;
  ;;
  ;;Mentre arrivano dati, li prendo e richiamo il task coroutine
  (define (ManageWSS actionl pb)
    (mtfa-noerr
     (begin
       (hash-remove! um pb)
       (eis::BaseLib::Close))
     (let ((task (hash-ref um pb))
	   (din (mtfa-eis-get-raw-data pb)))
       (unless task
	 (set! task (serve-client))
	 (hash-set! um pb task))
       ;;
       (if din
	   (begin
	     ;;(Show! "ricevuti " (bytevector-length din) " byte\n" (mtfa-bv-to-hex-string din))
	     (receive (proc d-out) (task `(,pb . ,din))
	       (hash-set! um pb proc) ;;il task può avere altri punti di ritorno!
	       (cond
		((va::vaeng::CheckIfDeallocated pb)
		 (hash-remove! um pb)
		 ;; (Show! "Closed!!!")
		 (eis::BaseLib::Close))
		((eqv? d-out 'st-started)
		 ;; (Show! "Started!!!")
		 (eis::BaseLib::ContinueDoNotClose))
		((eqv? d-out 'st-need-more-data)
		 ;; (Show! "Attende per nuovi dati!!! eis::BaseLib::ContinueDoNotClose")
		 ;;(Show! "RecvData: " (va::vaeng::CheckIfDeallocated pb))
		 (mtfa-eis-enable-pbuf pb #t '())
		 (eis::BaseLib::ContinueDoNotClose))
		((eqv? d-out 'st-ended)
		 (hash-remove! um pb)
		 ;; (Show! "Closed!!!")
		 (eis::BaseLib::Close))
		(d-out
		 (SendData pb d-out)
		 ;; (Show! "Sent!!!")
		 (eis::BaseLib::ContinueDoNotClose))
		(#t ;; (Show! "other: wait..")
		 (eis::BaseLib::ContinueDoNotClose)))))
	   (begin
	     ;; (Show! "No data in input: wait..")
	     (eis::BaseLib::ContinueDoNotClose))))))
  ;;
  (eis::function-pointer-add service-name ManageWSS)
  ;;
  ;;L'hash contiene pairs: (task, bvinport). Lo pulisco ogni secondo
  ;;inutile mantenere i dati di socket chiusi
  (make-thread (lambda ()
		 (let loop ()
		   (sleep 1)
		   (for-each
		    (lambda (p)
		      (when (va::vaeng::CheckIfDeallocated (car p))
			(Show! "Removes a pb")
			(hash-remove! um (car p))))
		    (hash-map->list (lambda (x y) `(,x . ,y)) um))))))

;; (EnableWebSocket "SWS_0" (lambda (x) ;; (Show! "Eval: " x)
;; 				 x))
