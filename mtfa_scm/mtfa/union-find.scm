(use-modules (srfi srfi-69))

;; Inizializza la struttura dati (una hash table vuota)
(define (make-union-find-dynamic)
  (make-hash-table))

(define (initialize-element uf-map x)
  ;; Se l'elemento non è presente nella mappa, lo aggiungiamo.
  (if (not (hash-table-exists? uf-map x))
      ;; Valore: (Genitore=x . Rango=0)
      (hash-table-set! uf-map x (cons x 0))
      #f))

(define (find uf-map x)
  ;; Assicurati che l'elemento esista prima di trovarlo
  (initialize-element uf-map x)
  
  (let* ((entry (hash-table-ref uf-map x))
         (parent (car entry)))
    
    ;; Se x è la radice (parent[x] = x), restituiscila
    (if (eq? parent x)
        x
        
        ;; Altrimenti, trova ricorsivamente la radice
        (let ((root (find uf-map parent)))
          ;; Path Compression: aggiorna il genitore di x (mantenendo il suo rango)
          (let ((rank (cdr entry)))
            (hash-table-set! uf-map x (cons root rank)))
          root))))

(define (union-sets uf-map x y)
  ;; Trova le radici. initialize-element è chiamato implicitamente da find.
  (let ((root-x (find uf-map x))
        (root-y (find uf-map y)))
    
    ;; Se sono già nello stesso set, non fare nulla
    (if (eq? root-x root-y)
        #f
        
        ;; Altrimenti, esegui l'unione basata sul Rango:
        (let* ((entry-x (hash-table-ref uf-map root-x))
               (entry-y (hash-table-ref uf-map root-y))
               (rank-x (cdr entry-x))
               (rank-y (cdr entry-y)))
          
          (cond 
            ;; Caso 1: Rank(x) < Rank(y) -> x punta a y
            ((< rank-x rank-y)
             (hash-table-set! uf-map root-x entry-y)) ; Cambia solo il genitore di root-x
            
            ;; Caso 2: Rank(x) > Rank(y) -> y punta a x
            ((> rank-x rank-y)
             (hash-table-set! uf-map root-y entry-x)) ; Cambia solo il genitore di root-y
            
            ;; Caso 3: Rank(x) = Rank(y) -> Unisci (es. y punta a x) e incrementa Rank(x)
            (else
             ;; y punta a x
             (hash-table-set! uf-map root-y entry-x) 
             ;; Incrementa il rango di x (crea una nuova coppia)
             (hash-table-set! uf-map root-x (cons root-x (+ rank-x 1)))))
          
          #t))))

(define uf-dyn (make-union-find-dynamic))

;; Inizializza e unisci 'Roma' e 'Lazio'
(union-sets uf-dyn 'roma 'lazio) ; #t

;; Inizializza e unisci 'Milano' e 'Lombardia'
(union-sets uf-dyn 'milano 'lombardia) ; #t

;; 'Roma' e 'Lombardia' sono in set diversi?
(eq? (find uf-dyn 'roma) (find uf-dyn 'lombardia)) ; #f

;; Unisci 'Roma' e 'Milano'
(union-sets uf-dyn 'roma 'milano) ; #t

;; 'Lazio' e 'Lombardia' sono ora nello stesso set?
(eq? (find uf-dyn 'lazio) (find uf-dyn 'lombardia)) ; #t (La radice è la stessa)
