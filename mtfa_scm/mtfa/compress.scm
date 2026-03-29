;; Copyright (C) 2013 Daniel Hartwig <mandyke@gmail.com>
;;
;; This program is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

(define-module (mtfa compress)
  #:use-module (rnrs bytevectors)
  #:use-module (rnrs io ports)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-26)
  #:use-module (ice-9 receive)
  #:export (lzw-compress
            lzw-uncompress
            %lzw-compress
            %lzw-uncompress))

;; This procedure adapted from an example in the Guile Reference
;; Manual.
(define (make-serial-number-generator start end)
  (let ((current-serial-number (- start 1)))
    (lambda ()
      (and (< current-serial-number end)
           (set! current-serial-number (+ current-serial-number 1))
           current-serial-number))))

(define (put-u16 port k)
  ;; Little endian.
  (put-u8 port (logand k #xFF))
  (put-u8 port (logand (ash k -8) #xFF)))

(define (get-u16 port)
  ;; Little endian.  Order of evaluation is important, use 'let*'.
  (let* ((a (get-u8 port))
         (b (get-u8 port)))
    (if (any eof-object? (list a b))
        (eof-object)
        (logior a (ash b 8)))))

(define (%lzw-compress in out done? table-size)
  (let ((codes (make-hash-table table-size))
        (next-code (make-serial-number-generator 0 table-size))
        (universe (iota 256))
        (eof-code #f))
    ;; Populate the initial dictionary with all one-element strings
    ;; from the universe.
    (for-each (lambda (obj)
                (hash-set! codes (list obj) (next-code)))
              universe)
    (set! eof-code (next-code))
    (let loop ((cs '()))
      (let ((c (in)))
        (cond ((done? c)
               (unless (null? cs)
		       (out (hash-ref codes cs)))
               (out eof-code)
               (values codes))
              ((hash-ref codes (cons c cs))
               (loop (cons c cs)))
              (else
               (and=> (next-code)
                      (cut hash-set! codes (cons c cs) <>))
               (out (hash-ref codes cs))
               (loop (cons c '()))))))))

(define (ensure-bv-input-port bv-or-port)
  (cond ((port? bv-or-port)
         bv-or-port)
        ((bytevector? bv-or-port)
         (open-bytevector-input-port bv-or-port))
        (else
         (scm-error 'wrong-type-arg "ensure-bv-input-port"
                    "Wrong type argument in position ~a: ~s"
                    (list 1 bv-or-port) (list bv-or-port)))))

(define (for-each-right proc lst)
  (let loop ((lst lst))
    (unless (null? lst)
	    (loop (cdr lst))
	    (proc (car lst)))))

(define (open-bit-output-port bits-per-entry)
  (let ((current 0)
	(location 0))
    (call-with-values
	(lambda ()
	  (open-bytevector-output-port))
      (lambda (port get-bytevector)
	(let ((write-to-bv (lambda (val)
			     ;; (format #t "Entering write-to-bv: current ~a location ~a val ~a bpe ~a~%" current location val bits-per-entry)
			     (set! current (logior current (ash val location)))
			     (set! location (+ location bits-per-entry))
			     (while (> location 8)
				    ;; (format #t "Writing ~a~%" (logand current #xff))
				    (put-u8 port (logand current #xff))
				    (set! current (ash current -8))
				    (set! location (- location 8)))
			     ;; (format #t "Leaving write-to-bv: current ~a location ~a~%" current location)
			     ))
	      (get-bv (lambda ()
			(put-u8 port current)
			(get-bytevector))))
	  (values write-to-bv get-bv))))))

(define (open-bit-input-port bv bits-per-entry)
  (let ((current 0)
	(location 0)
	(eof #f))
    (call-with-values
	(lambda ()
	  (open-bytevector-input-port bv))
      (lambda (port)
	;; Return the read procedure, which begins here
	(lambda ()
	  ;; (format #t "Entering read-from-bv: current ~x location ~a~%" current location)
	  (let loop ((u8 (get-u8 port)))
	    ;; (format #t "Read ~a~%" u8)
	    (if (eof-object? u8)
		(if (> location 0)
		    (begin
		      (let ((output (bit-extract current 0 bits-per-entry)))
			(set! current (ash current (- bits-per-entry)))
			(set! location (- location bits-per-entry))
			;; (format #t "EOF Leaving read-from-bv: current ~x location ~a output ~x~%" current location output)
			output))
		    (begin
		      ;; (format #t "EOF Leaving read-from-bv: <eof>~%")
		      (eof-object)))
		      
		;; else
		(begin
		  (set! current (logior current (ash u8 location)))
		  (set! location (+ location 8))
		  (if (< location bits-per-entry)
		      (begin
			;; (format #t "Looping in read-from-bv: current ~x location ~a~%" current location)
			(loop (get-u8 port)))
		      ;; else
		      (let ((output (bit-extract current 0 bits-per-entry)))
			(set! current (ash current (- bits-per-entry)))
			(set! location (- location bits-per-entry))
			;; (format #t "Leaving read-from-bv: current ~x location ~a output ~x~%" current location output)
			output))))))))))

#|
	(lambda ()
	  ;; (format #t "Entering read-from-bv: current ~x location ~a~%" current location)
	  (if eof
	      (eof-object)
	      ;;else
	      (begin 
		(while (< location bits-per-entry)
		       ;; (format #t "Looping in read-from-bv: current ~x location ~a~%" current location)
		       (let ((u8 (get-u8 port)))
			 ;; (format #t "Read ~a~%" u8)
			 (if (eof-object? u8)
			     (begin
			       (set! eof #t)
			       (break))
			     ;; else
			     (begin
			       (set! current (logior current (ash u8 location)))
			       (set! location (+ location 8))))))
		;; (format #t "After loop in read-from-bv: current ~x location ~a~%" current location)
		(let ((output (bit-extract current 0 bits-per-entry)))
		  (set! current (ash current (- bits-per-entry)))
		  (set! location (- location bits-per-entry))
		  ;; (format #t "Leaving read-from-bv: current ~x location ~a output ~x~%" current location output)
		  output))))))))
|#

(define (%lzw-uncompress in out done? table-size)
  (let ((strings (make-hash-table table-size))
        (next-code (make-serial-number-generator 0 table-size))
        (universe (iota 256))
        (eof-code #f))
    (for-each (lambda (obj)
                (hash-set! strings (next-code) (list obj)))
              universe)
    (set! eof-code (next-code))
    (let loop ((previous-string '()))
      (let ((code (in)))
        (unless (or (done? code)
                    (= code eof-code))
		(unless (hash-ref strings code)
			(hash-set! strings
				   code
				   (cons (last previous-string) previous-string)))
		(for-each-right out
				(hash-ref strings code))
		(let ((cs (hash-ref strings code)))
		  (and=> (and (not (null? previous-string))
			      (next-code))
			 (cut hash-set! strings <> (cons (last cs)
							 previous-string)))
		  (loop cs)))))))

(define (lzw-compress-inner bv table-size dictionary)
  (call-with-values
      (lambda ()
        (open-bytevector-output-port))
    (lambda (output-port get-result)
      (let ((dict (%lzw-compress (cute get-u8 (ensure-bv-input-port bv))
                                 (cute put-u16 output-port <>)
                                 eof-object?
                                 table-size)))
        (if dictionary
            (values (get-result) dict)
            (get-result))))))

(define* (lzw-compress bv #:key (table-size 65536) dictionary)
  (let ((bv (lzw-compress-inner bv table-size dictionary)))
    (receive (write-to-bv get-bv)
	     (open-bit-output-port (integer-length (1- table-size)))
	     ;; (write (bytevector->uint-list bv (endianness little) 2)) (newline)
	     (for-each write-to-bv (bytevector->uint-list bv (endianness little) 2))
	     (get-bv))))


(define* (lzw-uncompress-inner bv table-size dictionary)
  ;; (format #t "lzw-uncompress: table-size ~a~%" table-size)
  (call-with-values
      (lambda ()
	(open-bytevector-output-port))
    (lambda (output-port get-result)
      (let ((dict (%lzw-uncompress (cute get-u16 (open-bytevector-input-port bv))
				   (cute put-u8 output-port <>)
				   eof-object?
				   table-size)))
	(if dictionary
	    (values (get-result) dict)
	    (get-result))))))

(define* (lzw-uncompress bv #:key (table-size 65536) dictionary)
  (let* ((get-val (open-bit-input-port bv (integer-length (1- table-size))))
	 (u16lst (let loop ((x (get-val))
			    (lst '()))
		   (if (eof-object? x)
		       lst
		       (loop (get-val) (append lst (list x)))))))
    (lzw-uncompress-inner (uint-list->bytevector u16lst (endianness little) 2) table-size dictionary)))

