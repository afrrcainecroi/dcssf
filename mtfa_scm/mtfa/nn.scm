(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa nn)
#:use-module  (mtfa error-handler)
#:use-module  (mtfa utils)
#:use-module  (mtfa serializer)
#:use-module  (mtfa unordered-set)
#:use-module  (mtfa unordered-map)
#:use-module  (mtfa star-map)
#:use-module  (mtfa simple_db)
#:use-module  (mtfa eis)
;;#:use-module  (mtfa fsm)
;;#:use-module  (mtfa va)
#:use-module  (mtfa extset)
#:use-module  (mtfa umset)
#:use-module  (mtfa web)
#:use-module  (mtfa brg)
#:use-module  (mtfa opencv)
 ;;
;; #:use-module  (pfds sets)
 ;;
#:use-module  (gnutls)
 ;;i moduli di guile
 ;;((rnrs records syntactic) #:prefix rnrs::)
#:use-module  (rnrs bytevectors)
#:use-module  (rnrs arithmetic bitwise)
#:use-module  ((rnrs io ports)
  #:select (string->bytevector bytevector->string)
  #:prefix ioports:)
 ;;
#:use-module  (srfi srfi-1)
#:use-module  (srfi srfi-4)
#:use-module  (srfi srfi-9)
#:use-module  (srfi srfi-11)
#:use-module  ((srfi srfi-18)
  #:prefix srfi-18::) ;;thread e mutex
 ;; date & time 
#:use-module  (srfi srfi-19)
#:use-module  (srfi srfi-26)
 ;;(srfi srfi-28)
#:use-module  (srfi srfi-43)
#:use-module  (srfi srfi-60)
#:use-module  (srfi srfi-171)
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
#:use-module  (ice-9 threads)
#:use-module  (ice-9 hash-table)
#:use-module  (ice-9 control)
#:use-module  (ice-9 match)
#:use-module  (ice-9 receive)
#:use-module  (ice-9 eval-string)
#:use-module  (ice-9 textual-ports)
#:use-module  (ice-9 arrays)
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
 ;;
#:use-module (dsv)
#:use-module  (web client)
 ;;
)

;;
(define-syntax-rule (I2E n)
  (inexact->exact (round n)))
(define-syntax-rule (E2I n)
  (exact->inexact n))
(define-syntax-rule (/I a b)
  (E2I (/ a b)))
(define-syntax-rule (/E a b)
  (I2E (/ a b)))
;;
(export I2E E2I /I /E)
(define-public (->float n) (if (exact? n) (exact->inexact n) n))
;;
(define-public (s32vector->pointer sv)
  (bytevector->pointer sv))
;;
(define (make-weights-mat-fast rows cols)
  (let* ((size (* rows cols))
         (bytes-needed (* size 4))
         (bv (make-bytevector bytes-needed))
         (entropy (gnutls-random random-level/random bytes-needed))
	 (std-dev (sqrt (/ 2.0 cols)))
         (gpu-mat (mtfa-opencv-make-gpu-mat rows cols)))

    (let loop ((i 0))
      (when (< i size)
        (let* ((offset (* i 4))
               (raw-val (bytevector-u32-native-ref entropy offset))
               (u1 (/ raw-val 4294967295.0))
               (weight (* (- (* u1 2.0) 1.0) std-dev)))
          (bytevector-ieee-single-native-set! bv offset (exact->inexact weight))
          (loop (1+ i)))))

    ;; ATTENZIONE ALL'ORDINE: matrice, puntatore, righe, colonne
    ;; Se in C++ hai (rows, cols), qui devi passare (rows cols)
    (mtfa-opencv-gpumat-upload-batch gpu-mat (bytevector->pointer bv) rows cols)
    gpu-mat))

;; (define (ispeziona-matrice etichetta gpu-mat righe colonne)
;;   (format #t "~%--- ISPEZIONE: ~a ---~%" etichetta)
;;   (let* ((viste (mtfa-opencv-gpumat-col-range gpu-mat 0 colonne))
;;          ;; Trasformiamo la ROI GPU in una lista Scheme (CPU)
;;          (lista (mtfa-opencv-convert-gpumat-to-lolod viste)))
;;     (format #t "Prime ~a colonne (flat): ~a~%" colonne lista)
;;     (format #t "Dimensioni reali: ~ax~a~%" 
;;             (mtfa-opencv-gpumat-get-rows gpu-mat)
;;             (mtfa-opencv-gpumat-get-cols gpu-mat))
;;     (force-output)))
;;
(define-class-public <mtfa-gann> ()
  ;;Le variabili di istanza e di classe
  ;;
  (rstate #:init-thunk (lambda () (seed->random-state (mtfa-strong-random 128)))
	  #:getter mtfa-gann::rstate
	  #:setter mtfa-gann::rstate!)
  ;;
  (input-size #:init-form #nil
              #:init-keyword #:input-size
              #:getter mtfa-gann::input-size
              #:setter mtfa-gann::input-size!)
  ;;
  (input-bias #:init-form #f
              #:init-keyword #:input-bias
              #:getter mtfa-gann::bias
              #:setter mtfa-gann::bias!)
  ;;
  (bias-value #:init-form 1
              #:init-keyword #:bias-value
              #:getter mtfa-gann::bias-value
              #:setter mtfa-gann::bias-value!)
  ;;
  (learning-rates #:init-form #nil
		  #:init-keyword #:learning-rates
		  #:getter mtfa-gann::learning-rates
		  #:setter mtfa-gann::learning-rates!)
  ;;
  (layer-sizes #:init-form #nil
               #:init-keyword #:layer-sizes
               #:getter mtfa-gann::layer-sizes
               #:setter mtfa-gann::layer-sizes!)
  ;;
  (error-type #:init-form 'linear-error
              #:init-keyword #:error-type
              #:getter mtfa-gann::error-type
              #:setter mtfa-gann::error-type!)
  ;;
  ;;Accomulatori utilizzati per i calcoli interni.
  (accumulators #:init-thunk make-hash-table
		#:getter mtfa-gann::acs
		#:setter mtfa-gann::acs!)
  ;;
  ;;le weights sono tante quanto i layer, compreso output
  (weights #:init-form '()
	   #:getter mtfa-gann::weights
	   #:setter mtfa-gann::weights!)
  ;;
  ;;I vettori hidden e anche l'output
  (hvectors #:init-form '()
	    #:getter mtfa-gann::hvectors
	    #:setter mtfa-gann::hvectors!)
  ;;
  ;;I vettori tmph, come gli hidden o gli errori
  (tmph #:init-form '()
	#:getter mtfa-gann::tmph
	#:setter mtfa-gann::tmph!)
  ;;
    
  (error-no-bias   #:accessor mtfa-gann::error-no-bias   #:init-value #f)

  ;;
  ;;Gli errori
  (errors #:init-form '()
	  #:getter mtfa-gann::errors
	  #:setter mtfa-gann::errors!)
  ;;
  (input-all-1-last-0 #:init-form '()
		      #:getter mtfa-gann::input-all-1-last-0
		      #:setter mtfa-gann::input-all-1-last-0!)
  (input-all-0-last-1 #:init-form '()
		      #:getter mtfa-gann::input-all-0-last-1
		      #:setter mtfa-gann::input-all-0-last-1!)
  ;;
  ;;Il metodo di inizializzazione delle matrici dei pesi, default truncated_normal
  (initialization-method #:init-form 'truncated_normal
			 #:init-keyword #:init-method
			 #:getter mtfa-gann::init-method
			 #:setter mtfa-gann::init-method!			 
			 )
  ;;
  ;;La costante per la funzione di attivazione lineare
  (mtfa-gann::identity-constant #:init-form '()
				#:getter mtfa-gann::identity-constant
				#:setter mtfa-gann::identity-constant!)
  ;;
  ;;La costante per la funzione di attivazione ReLU
  (mtfa-gann::elu-constant #:init-form '()
			   #:getter mtfa-gann::elu-constant
			   #:setter mtfa-gann::elu-constant!)
  ;;
  ;;Gli algoritmi di attivazione, applicati in ordine di layer
  (activations-algo  #:init-form #nil
		     #:init-keyword #:activations-algo
		     #:getter mtfa-gann::activations-algo
		     #:setter mtfa-gann::activations-algo!)
  ;; Aggiungi questi due per eliminare i warning
  (mask-clear-bias #:accessor mtfa-gann::mask-clear-bias #:init-value #f)
  (mask-set-bias   #:accessor mtfa-gann::mask-set-bias   #:init-value #f)
  (leaky-relu-alpha #:init-value 0.01 #:getter mtfa-gann::leaky-relu-alpha)
  (batch-size       #:init-value 64   #:getter mtfa-gann::batch-size)

  (input-mat  #:init-value #f #:getter mtfa-gann::input-mat #:setter mtfa-gann::input-mat!)
  (target-mat #:init-value #f #:getter mtfa-gann::target-mat #:setter mtfa-gann::target-mat!)
  )

;;
;; Funzione di utilità per estrarre valori dalle keyword (#:key)
(define (keyword-ref keywords target default)
  (let ((kv (memq target keywords)))
    (if (and kv (not (null? (cdr kv))))
        (cadr kv)
        default)))
;;
;;L'inizializzatore della classe ann
(define-method (initialize (gann <mtfa-gann>) initargs)
  (next-method)
  (let ((batch-size (or (keyword-ref initargs #:batch-size #f) 64)))
    (slot-set! gann 'batch-size batch-size)
    
    ;; 1. Validazione Iniziale
    (mtfa-assert (and (> (mtfa-gann::input-size gann) 0)
                      (pair? (mtfa-gann::learning-rates gann))
                      (pair? (mtfa-gann::layer-sizes gann))
                      (pair? (mtfa-gann::activations-algo gann))
                      (= (length (mtfa-gann::learning-rates gann))
                         (length (mtfa-gann::layer-sizes gann))
                         (length (mtfa-gann::activations-algo gann))))
                 "Parametri <mtfa-gann> mancanti o inconsistenti.")

    ;; 2. GESTIONE BIAS (Deve avvenire PRIMA di ogni allocazione)
    (when (mtfa-gann::bias gann)
      ;; Incrementiamo l'input-size per far posto al bit del bias
      (mtfa-gann::input-size! gann (1+ (mtfa-gann::input-size gann)))
      (let ((l (mtfa-gann::layer-sizes gann)))
        ;; Incrementiamo tutti i layer tranne l'ultimo (output)
        (mtfa-gann::layer-sizes! gann (append (map 1+ (drop-right l 1)) (list (last l))))))

    ;; 3. ALLOCAZIONE MATRICI DI STATO (Hidden Vectors, Errori, TMPH)
    ;; Ora usano le dimensioni già corrette (col bias) e il batch-size
    ;; Passiamo 0.0 per FORZARE l'allocazione immediata in VRAM
    (mtfa-gann::hvectors! gann 
			  (map (lambda (rows) (mtfa-opencv-make-gpu-mat rows batch-size 0.0))
			       (mtfa-gann::layer-sizes gann)))
    
    (mtfa-gann::errors! gann 
			(map (lambda (rows) (mtfa-opencv-make-gpu-mat rows batch-size 0.0))
			     (mtfa-gann::layer-sizes gann)))
    
    (mtfa-gann::tmph! gann 
		      (map (lambda (rows) (mtfa-opencv-make-gpu-mat rows batch-size 0.0))
			   (mtfa-gann::layer-sizes gann)))

    (mtfa-gann::input-mat! gann (mtfa-opencv-make-gpu-mat (mtfa-gann::input-size gann) batch-size))
    (mtfa-gann::target-mat! gann (mtfa-opencv-make-gpu-mat 10 batch-size))

    ;; 4. CREAZIONE MASCHERE BIAS (Larghe quanto il batch)
    (when (mtfa-gann::bias gann)
      (let* ((input-sz (mtfa-gann::input-size gann))
             (m-clear (mtfa-opencv-make-gpu-mat input-sz batch-size 1.0))
             (m-set   (mtfa-opencv-make-gpu-mat input-sz batch-size 0.0)))
        
        ;; Configuriamo l'ultima riga per tutte le colonne del batch
        (let loop ((c 0))
          (when (< c batch-size)
            (mtfa-opencv-gpumat-set m-clear (1- input-sz) c 0.0)
            (mtfa-opencv-gpumat-set m-set (1- input-sz) c (->float (mtfa-gann::bias-value gann)))
            (loop (1+ c))))
        
        (slot-set! gann 'input-all-1-last-0 m-clear)
        (slot-set! gann 'input-all-0-last-1 m-set)))

    ;; 5. INIZIALIZZAZIONE PESI
    (match (mtfa-gann::init-method gann)
      ('truncated_normal
       (mtfa-gann::weights! gann 
          (map (lambda (prec-size size) 
                 (make-weights-mat-fast size prec-size))
               ;; PREVIOUS SIZES: Input + tutti i layer tranne l'ultimo
               (cons (mtfa-gann::input-size gann) 
                     (drop-right (mtfa-gann::layer-sizes gann) 1)) ;; <--- AGGIUNTO 1
               ;; CURRENT SIZES: Tutti i layer (hidden + output)
               (mtfa-gann::layer-sizes gann))))
      (_ (error "Metodo di inizializzazione non supportato.")))

    ;; 6. VALIDAZIONE ALGORITMI (Case-Insensitive)
    (for-each (lambda (algo)
                (unless (memq algo '(identity sigmoid tanh relu elu leaky-relu softmax))
                  (error "Algoritmo non supportato (attenzione alle maiuscole):" algo)))
              (mtfa-gann::activations-algo gann))))
;;
;;I metodi, interni, per ottimizzare l'uso della memoria
;;
;; 1. NUOVA FUNZIONE DI ALLOCAZIONE RAW
;; 1. NUOVA FUNZIONE DI ALLOCAZIONE RAW
(define-method (mtfa-gann::getaccumulator-raw (gann <mtfa-gann>) rows cols key)
  (let ((acs (mtfa-gann::acs gann)))
    (or (hash-ref acs key)
        ;; Passiamo 0.0 per forzare la VRAM
        (let ((new-mat (mtfa-opencv-make-gpu-mat rows cols 0.0)))
          (hash-set! acs key new-mat)
          new-mat))))
;;
;; 2. FIX METODO PRINCIPALE
(define-method (mtfa-gann::getaccumulator-for-mat (gann <mtfa-gann>) layer-idx is-training? algo-name batch-size)
  (let* ((sizes (mtfa-gann::layer-sizes gann))
         (key (list layer-idx algo-name batch-size))
         (rows (if (< layer-idx (length sizes))
                   (list-ref sizes layer-idx)
                   (error "Indice layer fuori range in getaccumulator:" layer-idx))))
    (mtfa-gann::getaccumulator-raw gann rows batch-size key)))
;;
;; 3. FIX METODO DELEGATO (il vero "meno codice, meno bug")
(define-method (mtfa-gann::getaccumulator-for-mat (gann <mtfa-gann>) mat (from <symbol>) (index <integer>))
  (receive (rows cols) (mtfa-opencv-gpumat-rows-cols mat)
    (mtfa-gann::getaccumulator-raw gann rows cols (list from index rows cols))))
;;
(define-method-public (mtfa-gann::initialize-xavier (gann <mtfa-gann>))
  (format #t "~%--- INIZIALIZZAZIONE XAVIER (CPU -> GPU) ---~%")
  (let ((weights (mtfa-gann::weights gann)))
    (for-each 
      (lambda (w-mat layer-idx)
        (let* ((rows (mtfa-opencv-gpumat-get-rows w-mat)) ;; Fan-Out
               (cols (mtfa-opencv-gpumat-get-cols w-mat)) ;; Fan-In
               
               ;; Formula Xavier: sqrt(6) / sqrt(in + out)
               (limit (/ (sqrt 6.0) (sqrt (+ rows cols)))))
          
          (format #t "   Layer ~a [~ax~a] -> Inizializzo tra +/- ~6,4f~%" 
                  layer-idx rows cols limit)
          
          ;; Chiama il wrapper che genera su CPU e carica su GPU
          (mtfa-opencv-gpumat-rand-uniform w-mat (- limit) limit)))
      
      weights
      (iota (length weights))))
  (format #t "--- Pesi pronti. Loss sbloccata. ---~%"))
;;
;; Definizione dell'inizializzazione He/Kaiming (Perfetta per ReLU/ELU)
(define-method-public (mtfa-gann::initialize-he (gann <mtfa-gann>))
  (format #t "~%--- INIZIALIZZAZIONE HE/KAIMING (CPU -> GPU) ---~%")
  (let ((weights (mtfa-gann::weights gann)))
    
    (for-each 
      (lambda (w-mat layer-idx)
        (let* ((rows (mtfa-opencv-gpumat-get-rows w-mat)) ;; Fan-Out
               (cols (mtfa-opencv-gpumat-get-cols w-mat)) ;; Fan-In
               
               ;; Formula He (Uniforme): sqrt(6 / fan_in)
               ;; Ignora il fan_out e si concentra solo sugli input per compensare la ReLU
               (limit (sqrt (/ 6.0 cols))))
          
          (format #t "   Layer ~a [~ax~a] -> Inizializzo tra +/- ~6,4f~%" 
                  layer-idx rows cols limit)
          
          ;; Chiama il wrapper che genera su CPU e carica su GPU
          (mtfa-opencv-gpumat-rand-uniform w-mat (- limit) limit)))
      
      weights
      (iota (length weights))))
  (format #t "--- Pesi pronti. Loss sbloccata. ---~%"))
;;
(define-method (mtfa-gann::getaccumulator-for-mat (gann <mtfa-gann>) mat (from <symbol>) (index <integer>))
  (receive (rows cols) (mtfa-opencv-gpumat-rows-cols mat)
    ;; Deleghiamo al metodo sopra: meno codice, meno bug.
    (mtfa-gann::getaccumulator-for-mat gann rows cols from index)))
;;
;; Tabella delle coppie Canoniche (Accorpamento)
;; Se l'ultima attivazione è X e l'errore è Y, il gradiente è sempre (Output - Target)
;; Registro espanso delle fusioni (Activation . Loss)
(define *fused-gradient-pairs*
  '(;; Coppie CANONICHE (Gradiente = Output - Target)
    ;; Queste tre coprono il 99% dei casi d'uso reali
    ((sigmoid  . cross-entropy) . #t)
    ((softmax  . cross-entropy) . #t)
    ((identity . square)        . #t)
    
    ;; Coppie ESPLICITE (Richiedono derivata separata)
    ((relu       . square) . #f)
    ((leaky-relu . square) . #f)
    ((elu        . square) . #f)
    ((tanh       . square) . #f)))
;;
;; Funzione per verificare se possiamo accorpare l'ultimo livello
(define (can-fuse? activation-algo error-type)
  (assoc-ref *fused-gradient-pairs* (cons activation-algo error-type)))
;;
;;Act functions: https://ml-cheatsheet.readthedocs.io/en/latest/activation_functions.html#linear
;;identity
(define-method (mtfa-gann::identity (gann <mtfa-gann>) dst mat dummy idx)
  (mtfa-opencv-gpumat-math-mul dst mat (mtfa-gann::identity-constant gann) 1.0))
;;
(define-method (mtfa-gann::identity' (gann <mtfa-gann>) dst mat dummy idx) ;;la derivata di x è 1, quindi, tutti 1
  (mtfa-opencv-gpumat-math-mul dst dst 0. 0.)
  (mtfa-opencv-gpumat-math-add dst dst (mtfa-gann::identity-constant gann)))
;;
;;sigmoide
(define-method (mtfa-gann::sigmoid (gann <mtfa-gann>) dst mat dummy idx)
  (mtfa-opencv-gpumat-math-mul dst mat -1. 1.) ;;-x
  (mtfa-opencv-gpumat-math-exp dst dst)  ;;e^-x
  (mtfa-opencv-gpumat-math-add dst dst 1.) ;;1+e^-x
  (mtfa-opencv-gpumat-math-div dst 1. dst 1.)) ;;1/(1+e^-x)
;;
;;La derivata della sigmoide sig'(x)=sig(x)*(1-sig(x))
;;entra la sigmoide
(define-method (mtfa-gann::sigmoid' (gann <mtfa-gann>) dst sigm dummy idx )
  (mtfa-opencv-gpumat-math-sub dst 1. sigm) ;;1-sig(x)
  (mtfa-opencv-gpumat-math-mul dst dst sigm 1.)) ;;(1-sig (x))*sig (x)
;;
(define-method (mtfa-gann::ReLU (gann <mtfa-gann>) dst mat dummy idx)
  ;;la regola è: max(x, 0)
  (mtfa-opencv-gpumat-math-max dst mat 0.))
;;
(define-method (mtfa-gann::ReLU' (gann <mtfa-gann>) dst mat dummy idx)
  ;; dst = (mat > 0) ? 1 : 0
  (mtfa-opencv-gpumat-math-compare dst mat 0.0 1)) ;; 1 è CMP_GT
;;
(define-method (mtfa-gann::ELU (gann <mtfa-gann>) dst mat dummy idx)
  ;; Usiamo idx e il batch-size corrente per recuperare l'area di memoria corretta
  (let ((a0 (mtfa-gann::getaccumulator-for-mat gann idx #f 'ELU (mtfa-gann::batch-size gann))))
    
    ;; Regola: per x > 0 vale x, altrimenti alpha(e^x - 1)
    
    ;; 1. Gestione parte negativa
    (mtfa-opencv-gpumat-math-min a0 mat 0.0) ;; Prende solo i valori < 0
    (mtfa-opencv-gpumat-math-exp a0 a0)      ;; e^x
    (mtfa-opencv-gpumat-math-sub a0 a0 1.0)  ;; e^x - 1
    (mtfa-opencv-gpumat-math-mul a0 a0 (mtfa-gann::elu-constant gann) 1.0) ;; alpha(e^x - 1)
    
    ;; 2. Gestione parte positiva
    (mtfa-opencv-gpumat-math-max dst mat 0.0) ;; Prende solo i valori > 0 (come una ReLU)
    
    ;; 3. Unione dei risultati
    (mtfa-opencv-gpumat-math-add dst dst a0)
    dst))
;;
(define-method (mtfa-gann::ELU' (gann <mtfa-gann>) dst elu dummy idx) ;; <--- Aggiunto idx
  (let ((batch-sz (mtfa-gann::batch-size gann)))
    ;; Usiamo chiavi univoche (simboli diversi) per non sovrapporre mask e tmp nello stesso layer
    (let ((mask (mtfa-gann::getaccumulator-for-mat gann idx #t 'ELU-mask batch-sz))
          (tmp  (mtfa-gann::getaccumulator-for-mat gann idx #t 'ELU-tmp  batch-sz)))
      
      ;; 1. Creiamo maschera: mask = (elu > 0.0) ? 1.0 : 0.0
      ;; Se elu > 0, la derivata è 1. Se elu <= 0, la derivata è elu + alpha
      (mtfa-opencv-gpumat-math-compare mask elu 0.0 1) ;; 1 = CMP_GT (Greater Than)
      
      ;; 2. Calcoliamo il ramo negativo: tmp = elu + alpha
      ;; Ricorda: la derivata di alpha(e^x - 1) è alpha*e^x. 
      ;; Dato che nel forward 'elu' (output) è alpha(e^x - 1), allora alpha*e^x = elu + alpha.
      (mtfa-opencv-gpumat-math-add tmp elu (mtfa-gann::elu-constant gann))
      
      ;; 3. Fusione rami: dst = mask + (1.0 - mask) * tmp
      ;; Se mask è 1 (x>0), dst = 1 + 0 = 1
      ;; Se mask è 0 (x<=0), dst = 0 + 1 * (elu + alpha) = elu + alpha
      (mtfa-opencv-gpumat-math-sub dst 1.0 mask)
      (mtfa-opencv-gpumat-math-mul dst dst tmp 1.0)
      (mtfa-opencv-gpumat-math-add dst dst mask)
      
      dst)))
;;la funzione softmax
;; ---------------------------------------------------------------------
;; SOFTMAX E DERIVATA (Con supporto GPU batch broadcasting)
;; ---------------------------------------------------------------------

;; la funzione softmax
(define-method (mtfa-gann::softmax (gann <mtfa-gann>) dst vec dummy idx)
  (let* ((batch-size (mtfa-gann::batch-size gann))
         (sizes      (mtfa-gann::layer-sizes gann))
         (rows       (list-ref sizes idx)))
    
    (let ((max-vec  (mtfa-gann::getaccumulator-raw gann 1 batch-size (list idx 'softmax-max batch-size)))
          (sum-vec  (mtfa-gann::getaccumulator-raw gann 1 batch-size (list idx 'softmax-sum batch-size)))
          (acc      (mtfa-gann::getaccumulator-for-mat gann idx #f 'softmax-acc batch-size))
          (ones-col (mtfa-gann::getaccumulator-raw gann rows 1 (list idx 'ones-col)))
          (max-rep  (mtfa-gann::getaccumulator-raw gann rows batch-size (list idx 'max-rep)))
          (sum-rep  (mtfa-gann::getaccumulator-raw gann rows batch-size (list idx 'sum-rep))))

      ;; Inizializza la colonna 'ones-col' con tutti 1.0 (azzeriamo e sommiamo 1)
      (mtfa-opencv-gpumat-math-mul ones-col ones-col 0.0 0.0)
      (mtfa-opencv-gpumat-math-add ones-col ones-col 1.0)

      ;; 1. Stabilità numerica: calcolo il max di ogni colonna
      (mtfa-opencv-gpumat-reduce vec max-vec 0 2) ;; 0=Dimensione (colonna), 2=REDUCE_MAX
      
      ;; 2. Espansione broadcasting per max-vec: ones-col (Rows x 1) * max-vec (1 x Batch)
      (mtfa-opencv-gpumat-gemm max-rep ones-col #f max-vec #f 1.0 max-rep #f 0.0)

      ;; 3. Sottrazione legale (hanno la stessa dimensione)
      (mtfa-opencv-gpumat-math-sub acc vec max-rep)
      
      ;; 4. Esponenziale
      (mtfa-opencv-gpumat-math-exp acc acc)
      
      ;; 5. Somma colonne
      (mtfa-opencv-gpumat-reduce acc sum-vec 0 0) ;; 0=Dimensione (colonna), 0=REDUCE_SUM
      
      ;; 6. Espansione broadcasting per sum-vec: ones-col (Rows x 1) * sum-vec (1 x Batch)
      (mtfa-opencv-gpumat-gemm sum-rep ones-col #f sum-vec #f 1.0 sum-rep #f 0.0)

      ;; 7. Divisione finale legale
      (mtfa-opencv-gpumat-math-div dst acc sum-rep 1.0)
      dst)))

;; la derivata della softmax
(define-method (mtfa-gann::softmax' (gann <mtfa-gann>) dst sm output-err idx)
  (let ((err-type (mtfa-gann::error-type gann)))
    (if (eq? err-type 'cross-entropy)
        (begin
          ;; FIX: Dobbiamo copiare il gradiente (output-err) dentro dst!
          ;; Se hai una funzione di copia nativa come (mtfa-opencv-gpumat-copy dst output-err) usala.
          ;; Altrimenti, usiamo una banale addizione con 0.0 per forzare la copia sicura in GPU:
          (mtfa-opencv-gpumat-math-add dst output-err 0.0)
          dst)
        (mtfa-gann::compute-explicit-softmax-batch-derivative gann dst sm output-err idx))))

;; calcolo effettivo derivata
(define-method (mtfa-gann::compute-explicit-softmax-batch-derivative (gann <mtfa-gann>) dst sm output-err idx)
  (let* ((batch-size (mtfa-gann::batch-size gann))
         (sizes      (mtfa-gann::layer-sizes gann))
         (rows       (list-ref sizes idx)))
         
    (let ((dot-product (mtfa-gann::getaccumulator-raw gann 1 batch-size (list idx 'sm-dot batch-size)))
          (temp-mat    (mtfa-gann::getaccumulator-for-mat gann idx #t 'sm-temp batch-size))
          (ones-col    (mtfa-gann::getaccumulator-raw gann rows 1 (list idx 'ones-col)))
          (dot-rep     (mtfa-gann::getaccumulator-raw gann rows batch-size (list idx 'sm-dot-rep))))
      
      ;; Calcolo dot product su ogni sample del batch
      (mtfa-opencv-gpumat-math-mul temp-mat sm output-err 1.0)
      (mtfa-opencv-gpumat-reduce temp-mat dot-product 0 0)
      
      ;; Inizializza la colonna 'ones-col' con tutti 1.0
      (mtfa-opencv-gpumat-math-mul ones-col ones-col 0.0 0.0)
      (mtfa-opencv-gpumat-math-add ones-col ones-col 1.0)
      
      ;; Espansione broadcasting del dot-product
      (mtfa-opencv-gpumat-gemm dot-rep ones-col #f dot-product #f 1.0 dot-rep #f 0.0)

      ;; Sottrazione e prodotto legale (tutte matrici [Righe x Batch-Size])
      (mtfa-opencv-gpumat-math-sub output-err dot-rep dst)
      (mtfa-opencv-gpumat-math-mul dst sm dst 1.0)
      dst)))
;;
;;
;;Le altre funzioni di attivazione
;; Tanh: Output tra -1 e 1
;; Tanh e Tanh'
(define-method (mtfa-gann::tanh (gann <mtfa-gann>) dst mat dummy idx)
  (mtfa-opencv-gpumat-math-tanh dst mat))

(define-method (mtfa-gann::tanh' (gann <mtfa-gann>) dst tanh-out dummy idx)
  (let ((acc (mtfa-gann::getaccumulator-for-mat gann idx #t 'tanh-acc (mtfa-gann::batch-size gann))))
    (mtfa-opencv-gpumat-math-sqr acc tanh-out)
    (mtfa-opencv-gpumat-math-sub dst 1.0 acc)))

;; Leaky-ReLU e Leaky-ReLU'
(define-method (mtfa-gann::Leaky-ReLU (gann <mtfa-gann>) dst mat dummy idx)
  (let ((batch-sz (mtfa-gann::batch-size gann)))
    (let ((neg (mtfa-gann::getaccumulator-for-mat gann idx #f 'lrelu-neg batch-sz))
          (pos (mtfa-gann::getaccumulator-for-mat gann idx #f 'lrelu-pos batch-sz)))
      (mtfa-opencv-gpumat-math-min neg mat 0.0)
      (mtfa-opencv-gpumat-math-mul neg neg (mtfa-gann::leaky-relu-alpha gann) 1.0)
      (mtfa-opencv-gpumat-math-max pos mat 0.0)
      (mtfa-opencv-gpumat-math-add dst pos neg))))

(define-method (mtfa-gann::Leaky-ReLU' (gann <mtfa-gann>) dst mat dummy idx)
  (let ((mask (mtfa-gann::getaccumulator-for-mat gann idx #t 'lrelu-mask (mtfa-gann::batch-size gann))))
    (mtfa-opencv-gpumat-math-compare mask mat 0.0 1)
    (mtfa-opencv-gpumat-math-sub dst 1.0 mask)
    (mtfa-opencv-gpumat-math-mul dst dst (mtfa-gann::leaky-relu-alpha gann) 1.0)
    (mtfa-opencv-gpumat-math-add dst dst mask)))
;;
;; Funzione,Range,Utilizzo Tipico
;; Identity,"(−∞,∞)",Regressione / Output lineare
;; Sigmoid,"(0,1)",Classificazione binaria
;; Tanh,"(−1,1)",Layer nascosti (vecchia scuola) / RNN
;; ReLU,"[0,∞)",Layer nascosti (standard moderno)
;; Leaky ReLU,"(−∞,∞)",Layer nascosti (evita neuroni morti)
;; ELU,"(−α,∞)",Layer nascosti (più fluida della ReLU)
;; Softmax,"[0,1]",Classificazione Multiclasse (MNIST)

;;Il calcolo dell'errore quadratico
(define-method (mtfa-gann::eval-square-error (gann <mtfa-gann>) error target output)
  (mtfa-opencv-gpumat-math-sub error target output)
  (mtfa-opencv-gpumat-math-sqr error error)
  (receive (rows cols) (mtfa-opencv-gpumat-rows-cols error)
    (mtfa-opencv-gpumat-math-mul error error 1.0 0.5)))
;;
;;Il calcolo dell'errore quadratico con radice quadrata
(define-method (mtfa-gann::eval-root-square-error (gann <mtfa-gann>) error target output)
    (mtfa-opencv-gpumat-math-sub error target output)
    (mtfa-opencv-gpumat-math-sqr error error)
    (receive (rows cols) (mtfa-opencv-gpumat-rows-cols error)
      (mtfa-opencv-gpumat-math-mul error error 1.0 0.5)
      (mtfa-opencv-gpumat-math-sqrt error error)))
;;
;;Il calcolo dell'errore lineare abs(t-o)
(define-method (mtfa-gann::eval-absolute-error (gann <mtfa-gann>) error target output)
  (mtfa-opencv-gpumat-math-sub error target output)
  (mtfa-opencv-gpumat-math-abs error error)
  ;; (receive (rows cols) (mtfa-opencv-gpumat-rows-cols error)
  ;;   (mtfa-opencv-gpumat-math-mul error error 1.0 0.5))
  )
;;
;;Il calcolo dell'errore lineare (t-o)
(define-method (mtfa-gann::eval-linear-error (gann <mtfa-gann>) error target output)
  (mtfa-opencv-gpumat-math-sub error target output)
  ;; (receive (rows cols) (mtfa-opencv-gpumat-rows-cols error)
  ;;   (mtfa-opencv-gpumat-math-mul error error 1.0 0.5))
  )
;;
;; ;;Il calcolo dell'errore cross-entropy
;; (define-method (mtfa-gann::eval-cross-entropy-error (gann <mtfa-gann>) error target output)
;;   ;; Gradiente = - (target / output)
;;   (mtfa-opencv-gpumat-math-div error target output 1.0)
;;   (mtfa-opencv-gpumat-math-mul error error -1.0 1.0))
;; Il calcolo NOMINALE dell'errore cross-entropy (per la stampa a video)
(define-method (mtfa-gann::eval-cross-entropy-nominal (gann <mtfa-gann>) error target output)
  ;; 1. Calcoliamo ln(output). Grazie al tuo C++, log(0) è già gestito internamente!
  (mtfa-opencv-gpumat-math-log error output) 
  
  ;; 2. Moltiplichiamo per il target: target * ln(output)
  (mtfa-opencv-gpumat-math-mul error target error 1.0)
  
  ;; 3. Invertiamo il segno (moltiplichiamo per -1)
  (mtfa-opencv-gpumat-math-mul error error -1.0 1.0))
;;
;; Nominale: Huber Loss (delta = 1.0)
;; Nominale: Huber Loss (delta = 1.0)
(define-method (mtfa-gann::eval-huber-error (gann <mtfa-gann>) error target output)
  (receive (rows cols) (mtfa-opencv-gpumat-rows-cols output)
    (let ((tmp-diff  (mtfa-gann::getaccumulator-raw gann rows cols '(huber-diff)))
          (tmp-large (mtfa-gann::getaccumulator-raw gann rows cols '(huber-large))))
      
      ;; 1. tmp-diff = |target - output|
      (mtfa-opencv-gpumat-math-sub tmp-diff target output)
      (mtfa-opencv-gpumat-math-abs tmp-diff tmp-diff)
      
      ;; 2. Ramo "lineare": tmp-large = max(|diff| - 1.0, 0.0)
      (mtfa-opencv-gpumat-math-sub tmp-large tmp-diff 1.0)
      (mtfa-opencv-gpumat-math-max tmp-large tmp-large 0.0)
      
      ;; 3. Ramo "quadratico": error = 0.5 * min(|diff|, 1.0)^2
      (mtfa-opencv-gpumat-math-min error tmp-diff 1.0)
      (mtfa-opencv-gpumat-math-sqr error error)
      (mtfa-opencv-gpumat-math-mul error error 1.0 0.5)
      
      ;; 4. Uniamo tutto: error = error + tmp-large
      (mtfa-opencv-gpumat-math-add error error tmp-large))))
;;
;; Derivata: Errore Quadratico (MSE)
(define-method (mtfa-gann::eval-square-error-derivative (gann <mtfa-gann>) gradient target output)
  ;; dE/do = output - target
  (mtfa-opencv-gpumat-math-sub gradient output target))

;; Derivata: Errore Assoluto (MAE)
(define-method (mtfa-gann::eval-absolute-error-derivative (gann <mtfa-gann>) gradient target output)
  ;; dE/do = sign(output - target)
  (mtfa-opencv-gpumat-math-sub gradient output target)
  (mtfa-opencv-gpumat-math-sign gradient gradient)) ;; Richiede che math-sign sia mappato in OpenCV

;; Derivata: Errore Lineare (t - o)
(define-method (mtfa-gann::eval-linear-error-derivative (gann <mtfa-gann>) gradient target output)
  ;; dE/do = -1.0
  ;; Se non hai un metodo gpumat-set, moltiplichiamo per 0 e sottraiamo 1
  (mtfa-opencv-gpumat-math-mul gradient gradient 0.0 0.0)
  (mtfa-opencv-gpumat-math-add gradient gradient -1.0))

;; Derivata: RMSE
(define-method (mtfa-gann::eval-root-square-error-derivative (gann <mtfa-gann>) gradient target output)
  ;; dE/do = sign(output - target) / sqrt(2)
  (mtfa-opencv-gpumat-math-sub gradient output target)
  (mtfa-opencv-gpumat-math-sign gradient gradient)
  (mtfa-opencv-gpumat-math-div gradient gradient 1.41421356 1.0))

;; --- NUOVA: Derivata Binary Cross-Entropy ---
;; dE/do = (output - target) / (output * (1 - output))
;; Derivata: Binary Cross-Entropy
(define-method (mtfa-gann::eval-bce-error-derivative (gann <mtfa-gann>) gradient target output)
  (receive (rows cols) (mtfa-opencv-gpumat-rows-cols output)
    (let ((tmp (mtfa-gann::getaccumulator-raw gann rows cols '(bce-tmp))))
      ;; 1. Calcolo denominatore: tmp = output * (1 - output)
      (mtfa-opencv-gpumat-math-sub tmp 1.0 output)
      (mtfa-opencv-gpumat-math-mul tmp tmp output 1.0)
      ;; 2. Aggiungiamo epsilon
      (mtfa-opencv-gpumat-math-add tmp tmp 0.0000001)
      ;; 3. Calcolo numeratore: gradient = output - target
      (mtfa-opencv-gpumat-math-sub gradient output target)
      ;; 4. Divisione finale
      (mtfa-opencv-gpumat-math-div gradient gradient tmp 1.0))))
;;
;; Derivata: Huber Loss (delta = 1.0)
(define-method (mtfa-gann::eval-huber-error-derivative (gann <mtfa-gann>) gradient target output)
  ;; 1. gradient = output - target
  (mtfa-opencv-gpumat-math-sub gradient output target)
  
  ;; 2. Tagliamo i valori inferiori a -1.0
  (mtfa-opencv-gpumat-math-max gradient gradient -1.0)
  
  ;; 3. Tagliamo i valori superiori a 1.0
  (mtfa-opencv-gpumat-math-min gradient gradient 1.0))
;;
(define*-public (ShowGpuMat title mat #:optional (nopp #f))
  (if nopp
      (Show! title ": <" (mtfa-opencv-convert-gpumat-to-lolod mat) ">")
      (begin
	(Show! title ": <" (mtfa-opencv-convert-gpumat-to-lolod mat) ">"))))
;;
(define*-public (ShowGpuMatRC title mat)
  (receive (r c) (mtfa-opencv-gpumat-rows-cols mat)
    (Show! title r ", " c)))
;;
(define (GetActivationFunctionFromAlgo algo)
  (match algo
    ('identity   (lambda (gann dst src dummy idx) (mtfa-gann::identity gann dst src dummy idx)))
    ('sigmoid    (lambda (gann dst src dummy idx) (mtfa-gann::sigmoid gann dst src dummy idx)))
    ('softmax    (lambda (gann dst src dummy idx) (mtfa-gann::softmax gann dst src dummy idx)))
    ('relu       (lambda (gann dst src dummy idx) (mtfa-gann::ReLU gann dst src dummy idx)))
    ('elu        (lambda (gann dst src dummy idx) (mtfa-gann::ELU gann dst src dummy idx)))
    ('leaky-relu (lambda (gann dst src dummy idx) (mtfa-gann::Leaky-ReLU gann dst src dummy idx)))
    ('tanh       (lambda (gann dst src dummy idx) (mtfa-gann::tanh gann dst src dummy idx)))
    ))
;;
(define (GetActivationFunctionFromAlgo' algo)
  (match algo
    ('identity   (lambda (gann dst src dummy idx) (mtfa-gann::identity' gann dst src dummy idx)))
    ('sigmoid    (lambda (gann dst src dummy idx) (mtfa-gann::sigmoid' gann dst src dummy idx)))
    ('softmax    (lambda (gann dst src dummy idx) (mtfa-gann::softmax' gann dst src dummy idx)))
    ('relu       (lambda (gann dst src dummy idx) (mtfa-gann::ReLU' gann dst src dummy idx)))
    ('elu        (lambda (gann dst src dummy idx) (mtfa-gann::ELU' gann dst src dummy idx)))
    ('leaky-relu (lambda (gann dst src dummy idx) (mtfa-gann::Leaky-ReLU' gann dst src dummy idx)))
    ('tanh       (lambda (gann dst src dummy idx) (mtfa-gann::tanh' gann dst src dummy idx)))
    ))
;;
;; Dispatcher per il calcolo del valore nominale della Loss
(define (GetErrorFunctionFromType type)
  (match type
    ('square         (lambda (gann error target output) (mtfa-gann::eval-square-error gann error target output)))
    ('root-square    (lambda (gann error target output) (mtfa-gann::eval-root-square-error gann error target output)))
    ('absolute       (lambda (gann error target output) (mtfa-gann::eval-absolute-error gann error target output)))
    ('linear-error   (lambda (gann error target output) (mtfa-gann::eval-linear-error gann error target output)))
    ('cross-entropy  (lambda (gann error target output) (mtfa-gann::eval-cross-entropy-nominal gann error target output)))
    ('huber          (lambda (gann error target output) (mtfa-gann::eval-huber-error gann error target output)))
    (_ (error "Funzione di errore non supportata o tipo errato:" type))))
;;
(define (GetErrorDerivativeFunctionFromType type)
  (match type
    ('square         (lambda (gann gradient target output) (mtfa-gann::eval-square-error-derivative gann gradient target output)))
    ('root-square    (lambda (gann gradient target output) (mtfa-gann::eval-root-square-error-derivative gann gradient target output)))
    ('absolute       (lambda (gann gradient target output) (mtfa-gann::eval-absolute-error-derivative gann gradient target output)))
    ('linear-error   (lambda (gann gradient target output) (mtfa-gann::eval-linear-error-derivative gann gradient target output)))
    ('cross-entropy  (lambda (gann gradient target output) (mtfa-gann::eval-bce-error-derivative gann gradient target output)))
    ('huber          (lambda (gann gradient target output) (mtfa-gann::eval-huber-error-derivative gann gradient target output)))
    (_ (error "Derivata dell'errore non supportata o tipo errato:" type))))
;;
(define-public mtfa-gann::error-square 'square)
(define-public mtfa-gann::error-root-square 'root-square)
(define-public mtfa-gann::error-absolute 'absolute)
(define-public mtfa-gann::error-linear 'linear-error)
(define-public mtfa-gann::error-cross-entropy 'cross-entropy)
(define-public mtfa-gann::error-huber 'huber)
;;
(define-method (mtfa-gann::calculate-batch-loss (gann <mtfa-gann>) predicted target)
  (let* ((err-type   (mtfa-gann::error-type gann))
         (err-func   (GetErrorFunctionFromType err-type))
         (batch-size (mtfa-opencv-gpumat-get-cols predicted))
         (rows       (mtfa-opencv-gpumat-get-rows predicted))
         ;; Recuperiamo una matrice temporanea dalla VRAM per non allocare ad ogni ciclo
         (err-mat    (mtfa-gann::getaccumulator-raw gann rows batch-size (list 'loss-mat err-type))))
    
    ;; 1. Calcoliamo l'errore puro usando la TUA funzione (es. eval-square-error)
    ;; La funzione scriverà i risultati dentro err-mat
    (err-func gann err-mat target predicted)
    
    ;; 2. Portiamo la matrice su CPU per sommarne i valori
    ;; (Siccome lo facciamo solo per stampare a schermo ogni tot epoche, non impatta le performance)
    (let* ((err-lista (mtfa-opencv-convert-gpumat-to-lolod err-mat))
           ;; err-lista è una lista di liste. Appiattiamola e sommiamo tutto.
           (flat-list (apply append err-lista))
           (total-sum (apply + flat-list))
           (num-elements (* rows batch-size)))
      
      ;; 3. Restituiamo la media (Loss media)
      (/ total-sum (->float num-elements)))))
;;
(define-method (mtfa-gann::apply-bias (gann <mtfa-gann>) input-vector)
  (when (mtfa-gann::bias gann)
    ;; 1. Azzeriamo l'ultima riga (il posto del bias) 
    ;; Input = Input * (1, 1, ..., 0)
    (mtfa-opencv-gpumat-math-mul input-vector input-vector (mtfa-gann::mask-clear-bias gann) 1.0)
    
    ;; 2. Sommiamo il valore del bias nell'ultima riga
    ;; Input = Input + (0, 0, ..., bias-value)
    (mtfa-opencv-gpumat-math-add input-vector input-vector (mtfa-gann::mask-set-bias gann))))
;;
;; Wrapper per ottenere solo le righe
(define-public (mtfa-opencv-gpumat-get-rows mat)
  (call-with-values 
    (lambda () (mtfa-opencv-gpumat-rows-cols mat))
    (lambda (rows cols) rows)))

;; Wrapper per ottenere solo le colonne
(define-public (mtfa-opencv-gpumat-get-cols mat)
  (call-with-values 
    (lambda () (mtfa-opencv-gpumat-rows-cols mat))
    (lambda (rows cols) cols)))
;
(define-method (mtfa-gann::apply-bias! (gann <mtfa-gann>) input-mat)
  (when (mtfa-gann::bias gann)
    (let ((rows (mtfa-opencv-gpumat-get-rows input-mat)))
      ;; Verifica di sicurezza: l'input deve avere lo spazio per il bias
      (if (= (mtfa-gann::input-size gann) rows)
          (begin
            ;; 1. Azzera l'ultima riga (dove andrà il bias)
            (mtfa-opencv-gpumat-math-mul input-mat input-mat (mtfa-gann::input-all-1-last-0 gann) 1.0)
            ;; 2. Inserisce il bias-value (se diverso da 0)
            (unless (zero? (mtfa-gann::bias-value gann))
              (mtfa-opencv-gpumat-math-add input-mat input-mat (mtfa-gann::input-all-0-last-1 gann))))
          (error "Dimensioni input-mat non coerenti con il bias della rete")))))
;;
(define-method-public (mtfa-gann::make-input-vector-from-list (gann <mtfa-gann>) (input-list <list>))
  (when (mtfa-gann::bias gann)
    ;;aggiungo 1/0 in fondo all'input
    (set! input-list (append input-list `((,(mtfa-gann::bias-value gann))))))
  (mtfa-opencv-make-gpumat-from-lolod input-list))
;;
(define-method-public (mtfa-gann::make-target-vector-from-list (gann <mtfa-gann>) (target-list <list>))
  (mtfa-opencv-make-gpumat-from-lolod target-list))
;;
;;
(define-method (mtfa-gann::calculate-accuracy (gann <mtfa-gann>) output-mat targets batch-size)
  (let ((predicted-indices (make-s32vector batch-size))) ;; Buffer per i risultati
    
    ;; 1. Otteniamo gli indici predetti dalla GPU
    (mtfa-opencv-gpumat-argmax-cols output-mat 
                                    (s32vector->pointer predicted-indices) 
                                    batch-size)
    
    ;; 2. Confrontiamo con i target originali
    (let loop ((i 0) (correct 0))
      (if (= i batch-size)
          (* (/ correct batch-size) 100.0) ;; Restituisce la percentuale
          (let ((pred (s32vector-ref predicted-indices i))
                (actual (list-ref targets i)))
            (loop (1+ i) (if (= pred actual) (1+ correct) correct)))))))
;;
(define-method (mtfa-gann::train-batch (gann <mtfa-gann>) input-batch target-batch)
  (let* ((bs             (mtfa-opencv-gpumat-get-cols input-batch))
         (inv-bs         (/ 1.0 bs))
         (weights-vec    (list->vector (mtfa-gann::weights gann)))
         (hvecs-vec      (list->vector (mtfa-gann::hvectors gann)))
         (errors-vec     (list->vector (mtfa-gann::errors gann)))
         (tmphs-vec      (list->vector (mtfa-gann::tmph gann)))
         (algos-vec      (list->vector (mtfa-gann::activations-algo gann)))
         (lrates-vec     (list->vector (mtfa-gann::learning-rates gann)))
         (num-layers     (vector-length weights-vec))
         (last-idx       (1- num-layers)))

    ;; 1. FORWARD PASS
    (let loop-f ((i 0) (current-in input-batch))
      (when (< i num-layers)
        (let ((hv-v (mtfa-opencv-gpumat-col-range (vector-ref hvecs-vec i) 0 bs)))
          (if hv-v
              (begin
                (mtfa-opencv-gpumat-gemm hv-v (vector-ref weights-vec i) #f current-in #f 1.0 #f #f 0.0)
                ((GetActivationFunctionFromAlgo (vector-ref algos-vec i)) gann hv-v hv-v #f i)
                
                ;; >>> FIX BIAS HIDDEN LAYER <<<
                ;; Riscriviamo forzatamente l'ultima riga di hv-v (post attivazione) per usarla come bias nel prossimo livello
                (when (and (mtfa-gann::bias gann) (< i last-idx))
                  (let ((last-row (mtfa-opencv-gpumat-row-range hv-v (1- (mtfa-opencv-gpumat-get-rows hv-v)) (mtfa-opencv-gpumat-get-rows hv-v))))
                    (mtfa-opencv-gpumat-math-mul last-row last-row 0.0 1.0) ;; azzeriamo
                    (mtfa-opencv-gpumat-math-add last-row last-row (->float (mtfa-gann::bias-value gann))))) ;; inseriamo l'1.0

                (loop-f (1+ i) hv-v))
              (format #t "ERRORE: Forward ROI fallita~%")))))
    ;;
    ;; 2. BACKWARD PASS
    (let* ((actual-out (mtfa-opencv-gpumat-col-range (vector-ref hvecs-vec last-idx) 0 bs))
           (out-err-v  (mtfa-opencv-gpumat-col-range (vector-ref errors-vec last-idx) 0 bs))
           (last-algo  (vector-ref algos-vec last-idx))
           (err-type   (mtfa-gann::error-type gann)))
      
      (if (and actual-out out-err-v target-batch)
          (begin
            
            ;; --- INIZIO LOGICA DINAMICA DEL GRADIENTE ---
            ;; Verifichiamo se possiamo usare l'accorpamento super-veloce
            (if (can-fuse? last-algo err-type)
                ;; Fast Path Canonico (es. Softmax+CE, Identity+MSE)
                ;; dE/do = output - target
                (mtfa-opencv-gpumat-math-sub out-err-v actual-out target-batch)
                
                ;; Dynamic Path: Derivata pura dell'errore
                (let ((err-deriv-func (GetErrorDerivativeFunctionFromType err-type)))
                  (err-deriv-func gann out-err-v target-batch actual-out)))
            ;; --- FINE LOGICA DINAMICA ---

            ;; 3. BACKWARD LOOP
            (let loop-b ((i last-idx) (delta-v out-err-v))
              (when (>= i 0)
                (let* ((raw-tmph (vector-ref tmphs-vec i))
                       (tmph-v   (or (mtfa-opencv-gpumat-col-range raw-tmph 0 bs) raw-tmph))
                       (curr-w   (vector-ref weights-vec i))
                       (algo     (vector-ref algos-vec i))
                       (raw-in   (if (= i 0) input-batch (vector-ref hvecs-vec (1- i))))
                       (in-v     (if (= i 0) raw-in (or (mtfa-opencv-gpumat-col-range raw-in 0 bs) raw-in))))

                  (if (and tmph-v in-v)
                      (begin
                        (let ((local-delta 
                               (if (and (= i last-idx) (can-fuse? algo (mtfa-gann::error-type gann)))
                                   delta-v
                                   (let* ((raw-h (vector-ref hvecs-vec i))
                                          (hv-v  (or (mtfa-opencv-gpumat-col-range raw-h 0 bs) raw-h)))
                                     ((GetActivationFunctionFromAlgo' algo) gann tmph-v hv-v delta-v i)
                                     (mtfa-opencv-gpumat-math-mul tmph-v delta-v tmph-v 1.0)
                                     tmph-v))))

                          (let ((prev-e-v #f))
                            (unless (= i 0)
                              (let* ((raw-e (vector-ref errors-vec (1- i)))
                                     (e-v (or (mtfa-opencv-gpumat-col-range raw-e 0 bs) raw-e)))
                                (if e-v
                                    (begin
                                      (mtfa-opencv-gpumat-gemm e-v curr-w #t local-delta #f 1.0 #f #f 0.0)
                                      (set! prev-e-v e-v))
                                    (format #t "ERRORE: Backward ROI fallita~%"))))

                            (let* ((w-rows  (mtfa-opencv-gpumat-get-rows curr-w))
                                   (w-cols  (mtfa-opencv-gpumat-get-cols curr-w))
                                   (key-sym (string->symbol (string-append "delta-w-" (number->string i))))
                                   (delta-w (mtfa-gann::getaccumulator-raw gann w-rows w-cols key-sym))
                                   (lr-scalato (* (vector-ref lrates-vec i) inv-bs)))

                              ;; 1. Calcoliamo il gradiente
                              (mtfa-opencv-gpumat-gemm delta-w local-delta #f in-v #t 
                                                       lr-scalato 
                                                       #f #f 0.0)
                              ;; 2. Sottrazione
                              (mtfa-opencv-gpumat-math-sub curr-w curr-w delta-w))
                            
                            (when prev-e-v
                              (loop-b (1- i) prev-e-v)))))
                      (format #t "ERRORE: Matrici nulle~%"))))))))))

;;Conversioni e generazioni varie
;;
(define-public (mtfa-gann::number->list-of-lbit num size-in-bytes)
  (map (compose (cut list <>) (cut - <> 48) (cut char->integer <>))
       (string->list (let ((s (number->string num 2)))
		       (if (< (string-length s) (* size-in-bytes 8) )
			   (string-append (make-string (- (* size-in-bytes 8) (string-length s)) #\0) s)
			   s)))))
(define-public (mtfa-gann::list-of-numbers->list-of-lbit lofnum size-in-bytes)
  (map (compose (cut list <>) (cut - <> 48) (cut char->integer <>))
       (append-map
	(lambda (num)
	  (string->list (let ((s (number->string num 2)))
			  (if (< (string-length s) (* size-in-bytes 8) )
			      (string-append (make-string (- (* size-in-bytes 8) (string-length s)) #\0) s)
			      s))))
	lofnum)))
(define-public (mtfa-gann::list-of-lbit-approximate bl)
  (map (lambda (i) (cons (inexact->exact (round (car i))) '())) bl))
(define-public (mtfa-gann::list-of-lbit->number bl)
  (fold (lambda (c p)
	  (match c
	    ((0) (arithmetic-shift p 1))
	    (_ (logior 1 (arithmetic-shift p 1)))))
	0
	bl))
;;
;; Questo accetta la matrice all-test-x dal loop
(define-method-public (mtfa-gann::query-batch (gann <mtfa-gann>) input-mat)
  (let* ((num-layers  (length (mtfa-gann::layer-sizes gann)))
         (last-idx    (1- num-layers))
         (batch-size          (mtfa-opencv-gpumat-get-cols input-mat))
         ;; >>> FIX: Convertiamo le liste in vettori per l'accesso rapido ad indice! <<<
         (hvecs-vec   (list->vector (mtfa-gann::hvectors gann)))
         (weights-vec (list->vector (mtfa-gann::weights gann)))
         (algos-vec   (list->vector (mtfa-gann::activations-algo gann))))
    
    ;; FORWARD PASS
    (let loop-f ((l 0) (current-in input-mat))
      (if (< l num-layers)
          (let ((hv-v (mtfa-opencv-gpumat-col-range (vector-ref hvecs-vec l) 0 batch-size)))
            
            ;; A. Moltiplicazione matriciale
            (mtfa-opencv-gpumat-gemm hv-v (vector-ref weights-vec l) #f current-in #f 1.0 #f #f 0.0)
            
            ;; B. Attivazione
            ((GetActivationFunctionFromAlgo (vector-ref algos-vec l)) gann hv-v hv-v #f l)
            
            ;; C. Iniezione del Bias per i layer nascosti
            (when (and (mtfa-gann::bias gann) (< l last-idx))
              (let ((last-row (mtfa-opencv-gpumat-row-range hv-v (1- (mtfa-opencv-gpumat-get-rows hv-v)) (mtfa-opencv-gpumat-get-rows hv-v))))
                (mtfa-opencv-gpumat-math-mul last-row last-row 0.0 1.0) 
                (mtfa-opencv-gpumat-math-add last-row last-row (->float (mtfa-gann::bias-value gann)))))
            
            (loop-f (1+ l) hv-v))
          
          ;; Ritorna l'ultimo layer
          (mtfa-opencv-gpumat-col-range (vector-ref hvecs-vec last-idx) 0 batch-size)))))
;;
;;Una versione che taglia quello troppo poco luminoso
(define (mtfa-gann::convert-bytevector-to-list-of-lbit bv)
  (reverse (let loop ((bv bv) (index 0))
	     (if (>= index (bytevector-length bv))
		 '()
		 (cons (if (> (bytevector-u8-ref bv index) 10) '(1) '(0))
		       (loop bv (1+ index)))))))

;;
;; =================================================================
;; API DI ALTO LIVELLO (Stile Keras / PyTorch)
;; =================================================================

;; FIT: Addestra la rete tagliando automaticamente il dataset in batch
;; FIT: Addestra la rete tagliando automaticamente il dataset in batch
(define-method-public (mtfa-gann::fit (gann <mtfa-gann>) x-mat y-mat epochs print-freq)
  (let* ((total-samples (mtfa-opencv-gpumat-get-cols x-mat))
         (net-bs        (mtfa-gann::batch-size gann)))
    
    (format #t "~%=== INIZIO ADDESTRAMENTO ===~%")
    (format #t "Dataset: ~a sample | Max Batch Size: ~a | Epoche: ~a~%" total-samples net-bs epochs)
    
    (let loop-epoch ((epoch 1))
      (when (<= epoch epochs)
        
        ;; Ciclo automatico di slicing del dataset (Forward + Backward)
        (let loop-batch ((i 0))
          (when (< i total-samples)
            (let* ((end (min (+ i net-bs) total-samples))
                   (batch-x (mtfa-opencv-gpumat-col-range x-mat i end))
                   (batch-y (mtfa-opencv-gpumat-col-range y-mat i end)))
              
              (mtfa-gann::train-batch gann batch-x batch-y)
              (loop-batch end))))
        
        ;; Log periodico (Calcolo della Loss)
        (when (= (remainder epoch print-freq) 0)
          (let* ((start-last (max 0 (- total-samples net-bs)))
                 ;; Usiamo col-range per creare viste coerenti
                 (batch-x (mtfa-opencv-gpumat-col-range x-mat start-last total-samples))
                 (batch-y (mtfa-opencv-gpumat-col-range y-mat start-last total-samples))
                 ;; Otteniamo la predizione
                 (last-pred (mtfa-gann::query-batch gann batch-x)))
            
            ;; IMPORTANTE: Passiamo a calculate-batch-loss solo la vista corretta
            (let ((loss-val (mtfa-gann::calculate-batch-loss gann last-pred batch-y)))
              (format #t "    -> Epoca ~a completata | Loss (ultimo batch): ~8,5f~%" epoch loss-val))
          (force-output)))
        
        (loop-epoch (1+ epoch))))
    (format #t "=== ADDESTRAMENTO CONCLUSO ===~%")))


;; EVALUATE: Testa la rete ed emette il verdetto usando il Giudice C++
(define-method-public (mtfa-gann::evaluate (gann <mtfa-gann>) x-mat y-mat)
  (let* ((total-samples (mtfa-opencv-gpumat-get-cols x-mat))
         (net-bs        (mtfa-gann::batch-size gann)))
    
    (format #t "~%=== VALUTAZIONE DATASET (~a sample) ===~%" total-samples)
    
    (let loop-batch ((i 0) (total-correct 0))
      (if (< i total-samples)
          (let* ((end (min (+ i net-bs) total-samples))
                 (batch-x (mtfa-opencv-gpumat-col-range x-mat i end))
                 (batch-y (mtfa-opencv-gpumat-col-range y-mat i end)))
            
            ;; 1. Inferenza
            (let* ((pred (mtfa-gann::query-batch gann batch-x))
                   ;; 2. Giudizio C++
                   (eval-result (mtfa-opencv-gpumat-evaluate-batch pred batch-y))
                   (esatte      (car eval-result))
                   (campioni    (cdr eval-result)))
              
              ;; (Opzionale: stampa dettagli per i primissimi sample testati)
              (when (= i 0)
                (let loop-campioni ((c 0) (lista campioni))
                  (unless (null? lista)
                    (let* ((info (car lista))
                           (p (car info)) (t (cadr info)) (conf (caddr info)))
                      (format #t " Sample ~a -> Rete: ~a (~5,2f%) | Target: ~a ~a~%" 
                              c p conf t (if (= p t) "[OK]" "[ERR]"))
                      (loop-campioni (1+ c) (cdr lista))))))
              
              (loop-batch end (+ total-correct esatte))))
          
          ;; STAMPA VERDETTO FINALE
          (let ((accuracy (* 100.0 (/ total-correct (->float total-samples)))))
            (format #t "-------------------------------------------~%")
            (format #t "Risposte Esatte: ~a su ~a~%" total-correct total-samples)
            (format #t "ACCURACY TOTALE: ~6,2f %~%" accuracy)
            (format #t "===========================================~%"))))))

;; =============================================================================
;; FUNZIONI DI CARICAMENTO DATI
;; =============================================================================

;; Helper: Legge tutte le righe di un file in una lista
(define-public (read-all-lines port)
  (let loop ((lines '()))
    (let ((line (read-line port)))
      (if (eof-object? line)
          (reverse lines)
          (loop (cons line lines))))))
