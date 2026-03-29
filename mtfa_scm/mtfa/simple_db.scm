(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa simple_db)
  ;;I miei moduli
  #:use-module (mtfa error-handler)
  #:use-module (mtfa utils)
  ;;;i moduli di guile
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
;;  #:use-module (srfi srfi-18)
  ;; date & time rinomina per avere un current time che non si sovrappone
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
  #:use-module (sxml simple)
  #:use-module (json)
  #:use-module (oop goops)
  #:use-module (oop goops describe)

  #:export (
	    	;;glb-db-ip
		;;glb-db-port
		;;glb-db-user
		;;glb-db-password
		;;glb-db-name
		db-interface::set-db-coordinates
		db-interface::DoSqlQuery
		db-interface::StartSqlSession
		db-interface::StopSqlSession
		db-interface::CommitSqlSession
		db-interface::RollbackSqlSession
		db-interface::DoSqlCommitted
		;;db-interface::dosql
		db-interface::get-lastid
	    ))

;;(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3.so" "init_mtfa_fs3")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_db_mysql.so" "init_mtfa_db_mysql")
;; (load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_ph.so" "init_mtfa_ph")
;; (load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_lib.so" "init_mtfa_lib")

;;;
;;;
;;;		ACCESSO DB INIZIO
;;;
;;;

;;(define glb-db-ip #nil)
;;(define glb-db-port #nil)
;;(define glb-db-user #nil)
;;(define glb-db-password #nil)
;;(define glb-db-name #nil)

(define (db-interface::set-db-coordinates ip user password name port)
   (list ip user password name port)
;;   (set! glb-db-ip ip)
;;   (set! glb-db-port port)
;;   (set! glb-db-user user)
;;   (set! glb-db-password password)
;;   (set! glb-db-name name)
  )

(define (local-check-db db)
  (if (or (nil? db) (not (pair? db)) (not (equal? #t (cdr db))))
      #f
      (car db)))

;;se lastid è #t allora richiede il lastid
(define* (db-interface::DoSqlQuery db-pars query #:optional lastid)
  (let (
        (not-already-connected (nil? (local-check-db db-pars)))  ;;se nil allora il sistema non era già connesso
        (db (local-check-db db-pars))
        (dbok #t)
        (qres #nil)
        )
    (when not-already-connected
      (set! db (mtfa-db-mysql-open))
      (set! dbok (mtfa-db-mysql-connect 
		  db 
		  (nth-item db-pars 0) ;;glb-db-ip 
		  (nth-item db-pars 1) ;;glb-db-user 
		  (nth-item db-pars 2) ;;glb-db-password 
		  (nth-item db-pars 3) ;;glb-db-name 
		  (nth-item db-pars 4) ;;glb-db-port)
		  )))
    (if (nil? dbok)
        (begin
          (when not-already-connected (mtfa-db-mysql-close db))
          #nil)
        (begin
          (set! qres (mtfa-db-mysql-do-sql db query))
          (when lastid (set! qres (mtfa-db-mysql-last-id db)))
          (when not-already-connected (mtfa-db-mysql-close db))
          (if (number? qres)
	      (number->string qres)
	      (if (null? qres)
	          #nil
                  qres))))))

(define-public (db-interface::DoConnect db-pars)
  (let* (
	 (db (mtfa-db-mysql-open))
	 (dbok (if (mtfa-db-mysql-p db) (mtfa-db-mysql-connect db 
					 (nth-item db-pars 0) ;;glb-db-ip 
					 (nth-item db-pars 1) ;;glb-db-user 
					 (nth-item db-pars 2) ;;glb-db-password 
					 (nth-item db-pars 3) ;;glb-db-name 
					 (nth-item db-pars 4) ;;glb-db-port)
					) #nil))
	 (qres (if (nil? dbok) #nil db)))
    (if (nil? qres)
        #nil
	(cons db  #t))))

(define (db-interface::DoDisconnect db)
  (if (local-check-db db)
      (mtfa-db-mysql-close db)
      #nil))

(define (db-interface::StartSqlSession db-pars)
  (let* (
	 (db (mtfa-db-mysql-open))
	 (dbok (if (mtfa-db-mysql-p db) (mtfa-db-mysql-connect db 
					 (nth-item db-pars 0) ;;glb-db-ip 
					 (nth-item db-pars 1) ;;glb-db-user 
					 (nth-item db-pars 2) ;;glb-db-password 
					 (nth-item db-pars 3) ;;glb-db-name 
					 (nth-item db-pars 4) ;;glb-db-port)
					) #nil))
	 (qres (if (nil? dbok) #nil (mtfa-db-mysql-start-transaction db)))
	 )
    (if (equal? qres #t)
	(cons db  #t)
	#nil
	)))

(define-syntax db-interface::StopSqlSession
  (syntax-rules ()
    ((db-interface::StopSqlSession db)
     (let ((thedb (local-check-db db)))
       (if thedb
	   (begin
	     (mtfa-db-mysql-close thedb)
	     (set! db #nil)
	     )
	   #nil)))))

;; (define (db-interface::StopSqlSession db)
;;   (let ((db (local-check-db db)))
;;     (if db
;;       (begin
;; 	(mtfa-db-mysql-close db)
;; 	)
;;       #nil)))

(define (db-interface::CommitSqlSession db)
  (let ((db (local-check-db db)))
    (if db
	(mtfa-db-mysql-commit db)
      #nil)))

(define (db-interface::RollbackSqlSession db)
  (let ((db (local-check-db db)))
    (if db
	(mtfa-db-mysql-rollback db)
      #nil)))

(define* (db-interface::DoSqlCommitted db query #:optional lastid)
  (let ((db (local-check-db db)))
    (if db
      (let (
	    (res (mtfa-db-mysql-do-sql db query)))
	(if (equal? lastid #t)
	    (mtfa-db-mysql-last-id db)
	    res))
      #nil)))

;; (define-syntax db-interface::dosql
;;   (syntax-rules ()
;;     ((db expr ...)
;;      (catch #t
;;        (lambda () 
;; 	 (letrec ((estrai
;; 		   (lambda (q) 
;; 		     (fold
;; 		      (lambda (i p)
;;      			(cond
;; 			 ((symbol? i) (string-append  p " " (symbol->string i)))
;; 			 ((number? i) (string-append  p " " (number->string i)))
;; 			 ((boolean? i) (string-append p " " (if i "1" "0")   ))
;; 			 ((list? i) (string-append p " ( " (estrai i) " ) " ))
;;      			 (#t (string-append p " \"" i "\""))
;; 			 ))
;; 		      ""
;; 		      q)))
;; 		  (query (estrai (cdr `(expr ...)))))
;; 	   (Show query)
;; 	   ;;(db-interface::DoSqlCommitted db query)
;; 	   ))
;;        (lambda (k . p)
;; 	 (Show "dosql error: " k " . " p)
;; 	 #nil)))))

(define (db-interface::get-lastid db)
  (let ((db (local-check-db db)))
    (if db
	(mtfa-db-mysql-last-id db)
	#nil)))


;;;;;;;;;;;;;;;;;;
;;;;;SQLITE3;;;;;;
;;;;;;;;;;;;;;;;;;
(define-public (db-interface::Sqlite3::Do connector sql)
  (if (string? connector)
      (mtfa-sqlite3-do connector sql)
      (mtfa-sqlite3-execute connector sql)))
(define-public (db-interface::Sqlite3::Open connector)
  (mtfa-sqlite3-open connector))
(define-public (db-interface::Sqlite3::Close connector)
  (mtfa-sqlite3-close connector))

;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;
;;;;;;DB MGM;;;;;;
;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;

(define DB::attribute::type (mtfa-fs3-make))

(define-syntax-rule (define-symbol-from-string s v)
  (eval (list 'define (string->symbol s) v) (interaction-environment)))
(export define-symbol-from-string)

#|
La struttura delle clausole la mettiamo in LISP (OR (> A B) (> C D))
|#
(define-syntax attrs-list->string
  (lambda (x)
    (syntax-case x ()
      ((_ ())
       #'(""))
      ((_ (p1))
       #'(mtfa-to-string p1))
      ((_ (p1 p2))
       #'(string-append (mtfa-to-string p1) ", " (attrs-list->string (p2))))
      ((_ (p1 p2 ...))
       #'(string-append (mtfa-to-string p1) ", " (attrs-list->string (p2 ...)))))))

(define (IsAString key)
          (let ((tipo (mtfa-fs3-get DB::attribute::type key)))
            ;;(Show! "key: " key  ". tipo: " tipo)
            (if (and (string? tipo) (FindPattern "(date)|(char)|(text)|(blob)" tipo))
                #t
                #f)))


;;La cosa più semplice, per ora, è quella di indicare con un operatore speciale se un valore deve essere stringa
(define-syntax where-clauses->string
  (lambda (x)
    (syntax-case x ()
      ((_ (op2 a b))
       #'(if (and (not (list? 'a)) (IsAString (mtfa-to-string a)))
             (string-append
              "("
              (where-clauses->string a) " "
              (symbol->string 'op2) " " 
              "'" (where-clauses->string b) "'"
              ")"
              )
             (string-append
              "("
              (where-clauses->string a) " "
              (symbol->string 'op2) " " 
              (where-clauses->string b)
              ")")))
      ((_ (op1 a))
       #'(if (eqv? '$ 'op1)
             (string-append
              "'"
              (where-clauses->string a)
              "'")
             (string-append
              "("
              (symbol->string 'op1) " "
              (where-clauses->string a)
              ")")))
      ((_ a)
       #'(string-append (mtfa-to-string a))))))

(define-syntax-rule (GetNameValue name data)
  (if (list? 'data)
      (if (eqv? '$ (car 'data))
          (cons (mtfa-to-string name) (string-append "'" (mtfa-to-string (second 'data)) "'"))
          (cons (mtfa-to-string name) (string-append (mtfa-to-string (second 'data))))
          )
      (if (IsAString (mtfa-to-string name))
          (cons (mtfa-to-string name) (string-append "'" (mtfa-to-string data) "'"))
          (cons (mtfa-to-string name) (mtfa-to-string data)))))

(define-syntax MakeSqlInsertAttributesValues
  (lambda (x)
    (syntax-case x ()
      ;;
      ((_ (a1) (v1))
       #'(let ((av (GetNameValue a1 v1)))
           (cons
            (car av) (cdr av))))
      ;;
      ((_ (a1 a2) (v1 v2))
       #'(let ((av1 (MakeSqlInsertAttributesValues (a1) (v1)))
               (av2 (MakeSqlInsertAttributesValues (a2) (v2))))
           (cons
            (string-append (car av1) ", " (car av2)) (string-append (cdr av1) ", " (cdr av2)))))
      ;;
      ((_ (a1 a2 ...) (v1 v2 ...))
       #'(let ((av1 (MakeSqlInsertAttributesValues (a1) (v1)))
               (av2 (MakeSqlInsertAttributesValues (a2 ...) (v2 ...))))
           (cons
            (string-append (car av1) ", " (car av2)) (string-append (cdr av1) ", " (cdr av2))))))))

(define-syntax MakeSqlInsert
  (lambda (x)
    (syntax-case x (INSERT INTO VALUES)
      ((_ INSERT INTO table-name attrs-list VALUES values-list)
       #'(let
             ((attr-values (MakeSqlInsertAttributesValues attrs-list values-list)))
           (string-append "INSERT INTO "
                          (mtfa-to-string table-name) " "
                          "("
                          (car attr-values)
                          ") VALUES ("
                          (cdr attr-values)
                          ")"))))))

;;Da utilizzare nella update
(define-syntax MakeSqlUpdateAttributesValues
  (lambda (x)
    (syntax-case x ()
      ;;
      ((_ (a1) (v1))
       #'(let ((av (GetNameValue a1 v1)))
           (string-append (car av) "=" (cdr av))))
      ;;
      ((_ (a1 a2) (v1 v2))
       #'(let ((av1 (MakeSqlUpdateAttributesValues (a1) (v1)))
               (av2 (MakeSqlUpdateAttributesValues (a2) (v2))))
           (string-append av1 ", " av2)))
      ;;
      ((_ (a1 a2 ...) (v1 v2 ...))
       #'(let ((av1 (MakeSqlUpdateAttributesValues (a1) (v1)))
               (av2 (MakeSqlUpdateAttributesValues (a2 ...) (v2 ...))))
           (string-append av1 ", " av2))))))

(define-syntax MakeSqlUpdate
  (lambda (x)
    (syntax-case x (UPDATE SET TO WHERE)
      ((_ UPDATE table-name SET attrs-list TO values-list WHERE where-clauses-list)
       #'(string-append "UPDATE " (mtfa-to-string table-name)
                        " SET " (MakeSqlUpdateAttributesValues attrs-list values-list)
                        " WHERE " (where-clauses->string where-clauses-list)))
      ((_ UPDATE table-name SET attrs-list TO values-list)
       #'(string-append "UPDATE " (mtfa-to-string table-name)
                        " SET " (MakeSqlUpdateAttributesValues attrs-list values-list))))))

(define-syntax MakeSqlSelect
  (lambda (x)
    (syntax-case x (SELECT FROM WHERE)
      ((_ SELECT attrs-list FROM from-list WHERE where-clauses-list )
       #'(begin
           (string-append "SELECT "
                          (attrs-list->string attrs-list)
                          " FROM "
                          (attrs-list->string from-list)
                          " WHERE "
                          (where-clauses->string where-clauses-list))))
      ((_ SELECT attrs-list FROM from-list)
       #'(begin
           (string-append "SELECT "
                          (attrs-list->string attrs-list)
                          " FROM "
                          (attrs-list->string from-list)))))))


(define-syntax-public SQL-BuildDbSchema
  (lambda (x)
    (syntax-case x ()
      ((_ db-connector )
       #'(let ((tables (db-interface::DoSqlQuery db-connector "show tables")))
           (if (nil? tables)
               "No data available"
               ;;
               ;;per ogni tavola, prende gli attributi
               (begin
                 ;;(Show! tables)
                 (let ((whole
                        (map
                         (lambda (i)
                           ;;
                           ;;Per ogni tavola costruisco lo schema degli identificatori
                           ;;si chiamano; nome db.tavola.attributo e poi il tipo
                           (cons
                            (list (string-append "DB::" (fourth db-connector) "." (car i))
                                  (string-append "`" (fourth db-connector) "`.`" (car i) "`")
                                  #f)
                            (map (lambda (attr)
                                   (let ((sym (string-append "DB::" (fourth db-connector) "." (car i) "." (car attr)))
                                         (txt (string-append "`" (fourth db-connector) "`.`" (car i) "`.`" (car attr) "`"))
                                         (typ (second attr)))
                                     (mtfa-fs3-add DB::attribute::type txt typ)
                                     (list sym txt typ)))
                                 (db-interface::DoSqlQuery db-connector (string-append "describe `" (car i) "`")))))
                         tables)))
                   ;;creo tutti i simboli che mi interessano
                   (for-each (lambda (it)
                               (for-each (lambda (jt)
                                           (define-symbol-from-string (car jt) (second jt)))
                                         it))
                             whole)
                   #t))))))))

(define-syntax-public SQL3-BuildDbSchema
  (lambda (x)
    (syntax-case x ()
      ((_ db-name )
       #'(let ((tables (filter (lambda (it) (not (string-match "^sqlite" (car it)))) (mtfa-sqlite3-do db-name "SELECT name FROM sqlite_master WHERE type='table'"))))
           (if (nil? tables)
               "No data available"
               ;;
               ;;per ogni tavola, prende gli attributi
               (begin
                 ;;(Show! tables)
                 (let ((whole
                        (map
                         (lambda (i)
                           ;;
                           ;;Per ogni tavola costruisco lo schema degli identificatori
                           ;;si chiamano; nome db.tavola.attributo e poi il tipo
                           (cons
                            (list (string-append "DB::" "main" "." (car i))
                                  (string-append "main" "." (car i))
                                  #f)
                            (map (lambda (attr)
                                   (let ((sym (string-append "DB::" "main" "." (car i) "." (second attr)))
                                         (txt (string-append "main" "." (car i) "." (second attr)))
                                         (typ (third attr)))
                                     (mtfa-fs3-add DB::attribute::type txt typ)
                                     (list sym txt typ)))
                                 (mtfa-sqlite3-do db-name (string-append "PRAGMA table_info(" (car i) ")")))))
                         tables)))
                   ;;creo tutti i simboli che mi interessano
                   (for-each (lambda (it)
                               (for-each (lambda (jt)
                                           (define-symbol-from-string (car jt) (second jt)))
                                         it))
                             whole)
                   #t))))))))

(define-syntax-public SQL-Select
  (lambda (x)
    (syntax-case x (SELECT FROM WHERE)
      ((_ connector SELECT attrs-list FROM from-list WHERE where-clauses-list show-sql-string)
       #'(let ((sql (MakeSqlSelect SELECT attrs-list FROM from-list WHERE where-clauses-list)))
           (when show-sql-string (display sql) (newline))
           (db-interface::DoSqlQuery connector sql)))
      ((_ connector SELECT attrs-list FROM from-list WHERE where-clauses-list)
       #'(let ((sql (MakeSqlSelect SELECT attrs-list FROM from-list WHERE where-clauses-list)))
           (db-interface::DoSqlQuery connector sql)))
      ((_ connector SELECT attrs-list FROM from-list show-sql-string)
       #'(let ((sql (MakeSqlSelect SELECT attrs-list FROM from-list)))
           (when show-sql-string (display sql) (newline))
           (db-interface::DoSqlQuery connector sql)))
      ((_ connector SELECT attrs-list FROM from-list)
       #'(let ((sql (MakeSqlSelect SELECT attrs-list FROM from-list)))
           (db-interface::DoSqlQuery connector sql)))
      )))

(define-syntax-public SQL-Update
  (lambda (x)
    (syntax-case x (UPDATE SET TO WHERE)
      ((_ connector UPDATE table-name SET attrs-list TO values-list WHERE where-clauses-list show-sql-string)
       #'(let ((sql (MakeSqlUpdate UPDATE table-name SET attrs-list TO values-list WHERE where-clauses-list)))
           (when show-sql-string (display sql) (newline))
           (db-interface::DoSqlQuery connector sql)))
      ((_ connector UPDATE table-name SET attrs-list TO values-list WHERE where-clauses-list)
       #'(let ((sql (MakeSqlUpdate UPDATE table-name SET attrs-list TO values-list WHERE where-clauses-list)))
           (db-interface::DoSqlQuery connector sql)))
      ((_ connector UPDATE table-name SET attrs-list TO values-list  show-sql-string)
       #'(let ((sql (MakeSqlUpdate UPDATE table-name SET attrs-list TO values-list)))
           (when show-sql-string (display sql) (newline))
           (db-interface::DoSqlQuery connector sql)))
      ((_ connector UPDATE table-name SET attrs-list TO values-list)
       #'(let ((sql (MakeSqlUpdate UPDATE table-name SET attrs-list TO values-list)))
           (db-interface::DoSqlQuery connector sql)))
      )))

(define-syntax-public SQL-Insert
  (lambda (x)
    (syntax-case x (INSERT INTO VALUES)
      ((_ connector INSERT INTO table-name attrs-list VALUES values-list show-sql-string return-lastid)
       #'(let ((sql (MakeSqlInsert INSERT INTO table-name attrs-list VALUES values-list)))
           (when show-sql-string (begin (display sql) (newline)))
           (if (equal? #f return-lastid)
               (db-interface::DoSqlQuery connector sql #t)
               (db-interface::DoSqlQuery connector sql #f))))
      ((_ connector INSERT INTO table-name attrs-list VALUES values-list return-lastid)
       #'(let ((sql (MakeSqlInsert INSERT INTO table-name attrs-list VALUES values-list)))
           (if (equal? #f return-lastid)
               (db-interface::DoSqlQuery connector sql #t)
               (db-interface::DoSqlQuery connector sql #f))))
      ((_ connector INSERT INTO table-name attrs-list VALUES values-list)
       #'(let ((sql (MakeSqlInsert INSERT INTO table-name attrs-list VALUES values-list)))
           (db-interface::DoSqlQuery connector sql #f))))))

(define-syntax-public SQL3-Select
  (lambda (x)
    (syntax-case x (SELECT FROM WHERE)
      ((_ connector SELECT attrs-list FROM from-list WHERE where-clauses-list show-sql-string)
       #'(let ((sql (MakeSqlSelect SELECT attrs-list FROM from-list WHERE where-clauses-list)))
           (when show-sql-string (display sql) (newline))
           (db-interface::Sqlite3::Do connector sql)))
      ((_ connector SELECT attrs-list FROM from-list WHERE where-clauses-list)
       #'(let ((sql (MakeSqlSelect SELECT attrs-list FROM from-list WHERE where-clauses-list)))
           (db-interface::Sqlite3::Do connector sql)))
      ((_ connector SELECT attrs-list FROM from-list show-sql-string)
       #'(let ((sql (MakeSqlSelect SELECT attrs-list FROM from-list)))
           (when show-sql-string (display sql) (newline))
           (db-interface::Sqlite3::Do connector sql)))
      ((_ connector SELECT attrs-list FROM from-list)
       #'(let ((sql (MakeSqlSelect SELECT attrs-list FROM from-list)))
           (db-interface::Sqlite3::Do connector sql))))))

(define-syntax-public SQL3-Update
  (lambda (x)
    (syntax-case x (UPDATE SET TO WHERE)
      ((_ connector UPDATE table-name SET attrs-list TO values-list WHERE where-clauses-list show-sql-string)
       #'(let ((sql (MakeSqlUpdate UPDATE table-name SET attrs-list TO values-list WHERE where-clauses-list)))
           (when show-sql-string (display sql) (newline))
           (db-interface::Sqlite3::Do connector sql)))
      ((_ connector UPDATE table-name SET attrs-list TO values-list WHERE where-clauses-list)
       #'(let ((sql (MakeSqlUpdate UPDATE table-name SET attrs-list TO values-list WHERE where-clauses-list)))
           (db-interface::Sqlite3::Do connector sql)))
      ((_ connector UPDATE table-name SET attrs-list TO values-list  show-sql-string)
       #'(let ((sql (MakeSqlUpdate UPDATE table-name SET attrs-list TO values-list)))
           (when show-sql-string (display sql) (newline))
           (db-interface::Sqlite3::Do connector sql)))
      ((_ connector UPDATE table-name SET attrs-list TO values-list)
       #'(let ((sql (MakeSqlUpdate UPDATE table-name SET attrs-list TO values-list)))
           (db-interface::Sqlite3::Do connector sql))))))

(define-syntax-public SQL3-Insert
  (lambda (x)
    (syntax-case x (INSERT INTO VALUES)
      ((_ connector INSERT INTO table-name attrs-list VALUES values-list show-sql-string)
       #'(let ((sql (MakeSqlInsert INSERT INTO table-name attrs-list VALUES values-list)))
           (when show-sql-string (begin (display sql) (newline)))
           (db-interface::Sqlite3::Do connector sql)))
      ((_ connector INSERT INTO table-name attrs-list VALUES values-list)
       #'(let ((sql (MakeSqlInsert INSERT INTO table-name attrs-list VALUES values-list)))
           (db-interface::Sqlite3::Do connector sql))))))
