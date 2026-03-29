(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa fsm)
  #:use-module (mtfa error-handler)
  #:use-module (mtfa utils)
  #:use-module (mtfa serializer)
  #:use-module (mtfa simple_db)
 ;; #:use-module (mtfa certs)
 ;; #:use-module (mtfa eis)
 ;; #:use-module (mtfa va  )

  ;;i moduli di guile
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-18) ;;thread e mutex
  ;; date & time rinomina per avere un current time che non si sovrappone
  #:use-module (srfi srfi-19)
  #:use-module (srfi srfi-26)
  ;;(srfi srfi-28)
  #:use-module ((srfi srfi-43) #:prefix datetime:)
  #:use-module (srfi srfi-60)
  #:use-module (web uri)
  #:use-module (system foreign)

  #:use-module (rnrs bytevectors)
  #:use-module (rnrs arithmetic bitwise)
  #:use-module ((rnrs io ports)
		#:select (string->bytevector bytevector->string)
		#:prefix ioports:)

  #:use-module (ice-9 format)
  #:use-module (ice-9 ftw)
  #:use-module (ice-9 rdelim)
  ;;(ice-9 pretty-print)
  #:use-module (ice-9 regex)
  #:use-module (ice-9 iconv)
  #:use-module (ice-9 peg)
  #:use-module (ice-9 peg string-peg)
  #:use-module (ice-9 vlist)
  #:use-module (ice-9 q)
  #:use-module (ice-9 binary-ports)
  #:use-module (ice-9 threads)
  #:use-module (ice-9 hash-table)
  #:use-module (ice-9 optargs)
  ;;(oop goops)
  #:use-module (sxml simple)
  #:use-module (sxml ssax)
  #:use-module (sxml xpath)
  #:use-module (json)

  )

;;I dati, gli stati, gli eventi e le azioni sono gestite all'esterno e nulla cambia durante l'esecuzione della macchina
;;Le uniche cose che sono salvate sono gli stati correnti di esecuzione.
;;i dati sono esclusivamente puntatori a dati le cui possibili alterazioni sono ricordate al di fuori della FSM
;;In questo modo è sufficiente per la FSM ricordare lo stato corrente.

;;una FSM è definita dal suo id (che descrive transizioni) e dalla sua istanza (che descrive il suo stato corrente)

;;Tutto quanto non dichiarato global è locale!!
(define fsm-initialized #f)
(define runtime-ids #nil)     ;;Qui ci sono le associazioni tra sid e id di ogni stringa utilizzata (analogo al dato persistente su DB)
(define fsm-transictions #nil)  ;;Descrizione degli stati di ogni singola FSM istanziata.
(define initial-states #nil)  ;;Stati iniziali delle FSM
(define run-time-fsm #nil)    ;;Questi sono gli id delle FSM a run time (quando fai la new). indice: iden_id e dati: stato corrente, anche con i dati associati
;;hanno una corrispondenza 1 a 1 con il db
;;
(define db #nil)  ;;storage persistente degli stati correnti delle FSM attive e degli ID assoluti delle FSM
;;
(define (Sql command) (mtfa-sqlite3-execute db command))
;;
;;estrae dal db tutti gli id/sid e li mette nella struttura run-time di accesso diretto agli id
(define (SetupRunTimeFromPersistent)
  (mtfa-noerr
   #nil
   ;;TODO: da aggiungere la gestione degli stati oltre che dei sid ???
   ;;Gestione dei SID-ID
   (for-each (lambda (id-sid)
	       (InsertRunTimeSid (second id-sid) (first id-sid)))
	     (Sql "select id, sid from persistent_ids"))
   ;;Gestione degli stati delle istanze in esecuzione
   (for-each (lambda (id-state-data)
	       (InitRunTimeFsm (first id-state-data) (second id-state-data) (mtfa-deserializer-b64 (third id-state-data))))
	     (Sql "select instance_id, state, data from persistent_current_state"))))
;;
;;FUNZIONI OPERATIVE
;;Entra sid e esce id da DB persistente
(define counter 1)
(define indexes (make-hash-table))
(define (InsertDbSid sid)
  (mtfa-noerr #nil
	     (if (nil? (hash-ref indexes sid))
		 (begin
		   (hash-set! indexes sid counter)
		   (set! counter (1+ counter))
		   (number->string (1- counter)))
		 (number->string (hash-ref indexes sid)))
	     (Sql (string-append "insert into persistent_ids (sid) values ('" sid "')"))
	     (caar (Sql (string-append "select id from persistent_ids where sid='" sid "'")))
	     ))

;; (define (GetDbId sid)
;;   ;; (mtfa-noerr #nil
;; 	     ;; (let ((dbid (Sql (string-append "select id from persistent_ids where sid='" sid "'"))))
;; 	     ;;   (if dbid (caar dbid) #nil))
;;   (number->string (hash-ref indexes sid))
;;   );; )

;;aggiora sid/id in run time
(define (InsertRunTimeSid sid id)
   (hash-set! runtime-ids sid id)
   id)

;;entra sid e esce id da run time
(define (GetRunTimeId sid)
  (hash-ref runtime-ids sid)
  )

(define (InsertSid sid)
   ;;presuppongo che non ci sia in db e in runtime
   (InsertRunTimeSid sid (InsertDbSid sid)))

;;Utilizza esclusivament ID
(define (AddTransiction fsm-id ista-id fsta-id evnt-id action)
   (hash-set! fsm-transictions (string-append fsm-id "|" ista-id "|" evnt-id) (list fsta-id action))
   )

(define (GetTransiction fsm-id ista-id evnt-id)
   (hash-ref fsm-transictions (string-append fsm-id "|" ista-id "|" evnt-id))
   )

(define (SetInitialState nam-id ista-id fsta-id fsm-error-action)
  (hash-set! initial-states nam-id (list ista-id fsta-id fsm-error-action))
  )
(define (GetInitialState fsm-id)
  (hash-ref initial-states fsm-id)
  )

(define (InitRunTimeFsm iden-id state-id actual-data)
  (hash-set! run-time-fsm iden-id (list state-id actual-data))

  ;;Il disco
  (if (nil? (Sql (string-append "select state from persistent_current_state where instance_id = " iden-id ");")))
      (Sql (string-append "insert into persistent_current_state (instance_id, state, data) values ("
			  iden-id ", "
			  state-id ", "
			  "'" (mtfa-serializer-b64 actual-data) "');"))
      (Sql (string-append "update persistent_current_state set "
			  "state=" state-id ", "
			  "data='" (mtfa-serializer-b64 actual-data) "' "
			  "where instance_id=" iden-id))))

(define (UpdateRunTimeFsm iden-id state-id actual-data update-disk)
  (hash-set! run-time-fsm iden-id (list state-id actual-data))
  (if update-disk
      (Sql (string-append "update persistent_current_state set "
  			  "state=" state-id ", "
  			  "data='" (mtfa-serializer-b64 actual-data) "' "
  			  "where instance_id=" iden-id))
      (Sql (string-append "update persistent_current_state set "
  			  "state=" state-id " "
  			  "where instance_id=" iden-id))
      )
  )

(define (GetRunTimeFsmState iden-id)
  (hash-ref run-time-fsm iden-id)
  )

;;Costruzione dei descrittori SID con prefisso f- fsm, S- stato, e- evento, ...
(define (F name)
	     (string-append "F-" name))
(define (S state name)
	     (string-append "S-" state "-" (F name)))
(define (E event name)
	     (string-append "E-" event "-" (F name)))
(define (I identifier name)
	     (string-append "I-" identifier "-" (F name)))

#|
Per creare una FSM. nome dello stato iniziale e dati da utilizzare a run time
questa deve sempre essere fatta. Non ci sono ricostruzioni. é analoga a una definizione di classe in C++. Questa devi sempre farla.
Ogni istanza, poi, sarà associata a questa
fsm-error-action ha come input stato iniziale, evento e identificativo dell'istanza di FSM
in analogia alle action, torna una lista che, quando inizia per #t ha la parte dati da aggiornare
I parametri passati all'azione sono sempre: fsm-id iden-id ista evnt fsta + dati
L'azione può decidere di cambiare stato. La risposta deve sempre essere, infatti: 
- #t/#f: quando #t il terzo valore di ritorno sono i dati
- #nil/fsta-id: se non nil, indica lo stato di arrivo
- dati: i dati da aggiornare in memoria persistente
|#

(define*-public (mtfa-fsm-define fsm-name
				 fsm-initial-state
				 fsm-final-state
				 #:optional
				 (fsm-error-action
				  (lambda (fsm-id iden-id ista-id evnt-id dati) ;;nota che è assente fsta-id
				    (Show "Unexpected event to FSM: " fsm-id
					  " for instance: " iden-id
					  ". ISTATE=" ista-id ", EVNT: " evnt-id ". Dati: " dati)
				    (list #f #nil))))
  (mtfa-noerr
   #nil
   ;;per prima cosa devo vedere se nome e stato sono già presenti nelle strutture di run-time
   ;;opera esclusivamente sui SID e li converte in ID.
   (let*
       ;;Dichiarazioni di variabili locali
       (
	(name  (F fsm-name))
	(istate (S fsm-initial-state fsm-name))
	(fstate (S fsm-final-state fsm-name))
	(nam-id (GetRunTimeId name))
	(ista-id (GetRunTimeId istate))
	(fsta-id (GetRunTimeId fstate))
	)
     (when (nil? nam-id)
       (set! nam-id (InsertSid name)))
     (when (nil? ista-id)
       (set! ista-id (InsertSid istate)))
     (when (nil? fsta-id)
       (set! fsta-id (InsertSid fstate)))
     ;;gli id ci sono. Aggiorno gli stati iniziali e la gestione degli errori!
     (SetInitialState nam-id ista-id fsta-id fsm-error-action)
     )))

#|I parametri passati all'azione sono sempre: fsm-id iden-id ista evnt fsta + dati
L'azione può decidere di cambiare stato. La risposta deve sempre essere, infatti: 
- #t/#f: quando #t il terzo valore di ritorno sono i dati
- #nil/fsta-id: se non nil, indica lo stato di arrivo
- dati: i dati da aggiornare in memoria persistente
|#
(define*-public (mtfa-fsm-add-transiction fsm
					  start-state
					  event
					  final-state
					  #:optional
					  (action (lambda (fsm-id iden-id ista-id evnt-id fsta-id dati)
						    (Show "FSM: " fsm-id ". "
							  "IDEN: " iden-id ". "
							  "ISTA: " ista-id ", "
							  "ENVT: " evnt-id " => " "FSTA: " fsta-id ". DATA: " dati)
						    (list #f)  ;;in questo modo non si devono salvare i dati in uscita
						    )))
  (mtfa-noerr #nil
	     ;;per prima cosa devo vedere se nome e stato sono già presenti nelle strutture di run-time
	     ;;poi inserisco e riprendo da DB
	     (let*
		 ;;Dichiarazioni di variabili locali
		 (
		  (s-name  (F fsm))
		  (s-ista (S start-state fsm))
		  (s-fsta (S final-state fsm))
		  (s-evnt (E event fsm))
		  (nam-id (GetRunTimeId  s-name))
		  (ista-id (GetRunTimeId s-ista))
		  (fsta-id (GetRunTimeId s-fsta))
		  (evnt-id (GetRunTimeId s-evnt))
		  )
	       (when (nil? nam-id) (error (string-append "Undefined FSM: " fsm)))
	       (when (nil? ista-id) (set! ista-id (InsertSid s-ista)) )
	       (when (nil? fsta-id) (set! fsta-id (InsertSid s-fsta)) )
	       (when (nil? evnt-id) (set! evnt-id (InsertSid s-evnt)) )

	       ;;A questo punto posso aggiungere una transizione: fsm-id ista-id fsta-id evnt-id action
	       (AddTransiction nam-id ista-id fsta-id evnt-id action)
	       #t)) )

;;ora ho tutto. Posso definire le macchine e metterle in esecuzione
;;Problema: come faccio a mettere questo in memoria persistente in modo da poter riprendere l'esecuzione delle macchine già avviate
;;anche a fronte di un riavvio del sistema?
;;il problema sono gli actual-data. Ora, se gli actual-data sono una stringa, un intero o una lista di stringhe e interi, tutto ok. posso salvare su memoria secondaria e poi recuperare al prossimo riavvio.
;;Altrimenti potrebbe diventare un problema. Ma gli actual data sono aggiornabili oppure no? secondo me sono aggiornabili e quindi per ogni evento e cambio di stato, quando necessario, devo salvare gli actual data.
(define-public (mtfa-fsm-new fsm run-time-identifier actual-data reset-if-existing)
  ;; (mtfa-noerr
  ;;  #nil
   (let* (
	  (s-name  (F fsm))
	  (s-iden (I run-time-identifier fsm))
	  (fsm-id (GetRunTimeId s-name))
	  (iden-id (GetRunTimeId s-iden))
	  (initial-state-id (GetInitialState fsm-id))
	  ;;(data (mtfa-serialize-simple-items actual-data))
	  (val #nil)
	  )
     (when (nil? fsm-id) (error (string-append ("Undefined FSM: " fsm))))
     (when (nil? initial-state-id) (error (string-append ("Undefined initial state for FSM: " fsm))))
     (if (nil? iden-id)
	 (begin
	   (set! iden-id (InsertSid s-iden))
	   (InitRunTimeFsm iden-id (first initial-state-id) actual-data))
	 (if reset-if-existing
	     (begin
	       (Show (string-append "Resetting an instance already existing: " fsm ":" run-time-identifier))
	       (InitRunTimeFsm iden-id (first initial-state-id) actual-data))
	     (Show (string-append "Not initialized! Cannot create an instance already existing: " fsm ":" run-time-identifier))))
     ;;aggiorno le strutture dati in memoria e su disco. Nota che in memoria i dati sono estesi mentre su disco sono serializzati.
     ));; )

(define-public (mtfa-fsm-step fsm rtid event)
  ;; (mtfa-noerr
  ;;  #nil
	     (let* (
		    (s-name  (F fsm))
		    (s-iden (I rtid fsm))
		    (s-evnt (E event fsm))
		    (fsm-id (GetRunTimeId s-name))
		    (iden-id (GetRunTimeId s-iden))
		    (evnt-id (GetRunTimeId s-evnt))
		    (initial-state-id (GetInitialState fsm-id)) ;;Per la gestione degli errori e lo stato finale
		    (fsta-id (second initial-state-id))
		    (action-error (third initial-state-id))
		    (state-data (GetRunTimeFsmState iden-id))
		    (ista-id (first state-data))
		    (data (second state-data))
		    (trans (GetTransiction fsm-id ista-id evnt-id))
		    (fsta-id #nil)
		    (aret #nil)
		    )
	       ;;ma lo stato finale esiste?
	       (let ((result #f) (data #nil))
		 (if (nil? trans)
		     (begin
		       (set! aret (action-error fsm-id iden-id ista-id evnt-id data)))	    ;;Chiamo la funzione di errore
		     (begin
		       ;;  (mtfa-fs3-update fsm-transictions (string-append fsm-id "|" ista-id "|" evnt-id) (list fsta-id action))))
		       (set! fsta-id (first trans))
		       (set! aret ((second trans) fsm-id iden-id ista-id evnt-id fsta-id data)))	    ;;Chiamo la funzione di action
		     )
		 ;;gestione del risultato
		 (cond
		  ((list? aret)
		   (cond
		    ((= 1  (length aret)) (set! result (first aret)))
		    ((= 2  (length aret)) (set! result (first aret)) (set! fsta-id (second aret )))
		    ((>= 3 (length aret)) (set! result (first aret)) (set! fsta-id (second aret )) (set! data (third aret)))))
		  (#t #nil) ;;ho già i dati a posto
		  )
		 ;;Se lo stato finale sta a #nil, lo stato non cambia, rimetto il precedente in modo da gestire l'eventuale cambio di dati!
		 ;;TODO: da ottimizzare
		 (when (nil? fsta-id) (set! fsta-id ista-id))
		 ;;Ora, se lo stato è quello finale, azzero la macchina. Se inviece qualcuno volesse farla ripartire è
		 ;;sufficiente ripassare per lo stato iniziale
		 ;;TODO Stoppare la macchina in caso di stato finale
		 (if result
		     (begin
		       ;;aggiorno stato finale e dati su disco
		       (UpdateRunTimeFsm iden-id fsta-id (second aret) #t))
		     (begin
		       ;;aggiorno solo stato finale su disco
		       (UpdateRunTimeFsm iden-id fsta-id data #f)))
		 
		 )));; )

(define*-public (mtfa-fsm-initialize #:optional (db-name #nil))
  (when (not fsm-initialized)  ;;Se già inizializzata, non fare nulla
    (set! fsm-initialized #t)
    (set! runtime-ids (make-hash-table))      ;;Qui ci sono le associazioni tra sid e id di ogni stringa utilizzata (analogo al dato persistente su DB)
    (set! fsm-transictions (make-hash-table)) ;;Descrizione degli stati di ogni singola FSM istanziata.
    (set! initial-states (make-hash-table))   ;;Stati iniziali delle FSM
    (set! run-time-fsm (make-hash-table))     ;;Questi sono gli id delle FSM a run time (quando fai la new). indice: iden_id e dati: stato corrente, anche con i dati associati

    (when (nil? db-name)
      (set! db-name "fsm.db"))

    (set! db (mtfa-sqlite3-open db-name)) ;;storage persistente degli stati correnti delle FSM attive e degli ID assoluti delle FSM

    ;;Crea le tabelle di persistenza. Se già esistono, no problem, errore gestito
    (Sql "create table persistent_current_state (instance_id INTEGER PRIMARY KEY, state INTEGER, data VARCHAR);")
    (Sql "create table persistent_ids (id INTEGER PRIMARY KEY AUTOINCREMENT, sid VARCHAR);") ;;contiene gli id numerici del tutto!
    (Sql "create unique index persistent_ids_idx on persistent_ids (id, sid);")

    ;;Dopo questa esecuzione, se una cosa sta in db, è presente anche in run-time
    (SetupRunTimeFromPersistent)))

;;Fa la drop di FSM già aperte
(define-public (mtfa-fsm-clean-all)
  (mtfa-noerr
   (begin
     (Show! "Error initializing FSMs")
     #f)
   (set! fsm-initialized #t)
   (set! runtime-ids (make-hash-table)) ;;Qui ci sono le associazioni tra sid e id di ogni stringa utilizzata (analogo al dato persistente su DB)
   (set! fsm-transictions (make-hash-table)) ;;Descrizione degli stati di ogni singola FSM istanziata.
   (set! initial-states (make-hash-table))   ;;Stati iniziali delle FSM
   (set! run-time-fsm (make-hash-table)) ;;Questi sono gli id delle FSM a run time (quando fai la new). indice: iden_id e dati: stato corrente, anche con i dati associati

   (Sql "drop table persistent_current_state")
   (Sql "drop table persistent_ids")
   (Sql "drop index persistent_ids_idx")

   ;;Crea le tabelle di persistenza. Se già esistono, no problem, errore gestito
   (Sql "create table persistent_current_state (instance_id INTEGER PRIMARY KEY, state INTEGER, data VARCHAR);")
   (Sql "create table persistent_ids (id INTEGER PRIMARY KEY AUTOINCREMENT, sid VARCHAR);") ;;contiene gli id numerici del tutto!
   (Sql "create unique index persistent_ids_idx on persistent_ids (id, sid);")

   ;;Dopo questa esecuzione, se una cosa sta in db, è presente anche in run-time
   (SetupRunTimeFromPersistent)))

