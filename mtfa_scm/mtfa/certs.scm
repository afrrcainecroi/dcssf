(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa certs)
  ;;I miei moduli
  #:use-module (mtfa utils)
  #:use-module (mtfa simple_db)
  #:use-module (mtfa  error-handler)
  
  ;;;i moduli di guile
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-18)
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
  #:use-module (ice-9 rdelim)

  #:use-module (sxml simple)
  #:use-module (json)
  #:export (
	    CA::MakeSerialNumber
	    CA::MakeSignature
	    CA::VerifySignature
	    CA::MakeX509CertAndPrivateKey
	    ))

(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_fs3.so" "init_mtfa_fs3")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_db_mysql.so" "init_mtfa_db_mysql")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_ph.so" "init_mtfa_ph")
(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_lib.so" "init_mtfa_lib")


;;;
;;;la sezione della firma elettronica
;;;

(define openssl "openssl")
;;(define serials "/new_devs/usr/local/CAPKI/ssl/certs/serials.txt")

(define db-pars (db-interface::set-db-coordinates
		 "global_db"
		 "franco"
		 "franco"
		 "x509_certificates"
		 3306))

;;;id unico
(define (CA::MakeSerialNumber)
	;; (let (
	;; 	(fin (open serials O_RDWR))
	;; 	(d (flock fin LOCK_EX))	;;Qui aspetta fino a che sia libero!
	;; 	(d (seek fin 0 SEEK_SET))
	;; 	(sn (string->number (read-line fin)))
	;; 	)
	;;     (seek fin 0 SEEK_SET)
	;;     (write-line (number->string (+ 1 sn)) fin)
	;;     (close fin)
	;; sn)
 (db-interface::DoSqlQuery db-pars
  "UPDATE unique_serial SET serial = LAST_INSERT_ID(serial + 1) where ca = 'sign'"
  #t
  )
  )

(define (CA::MakeSignature id-utente stringa-base64-da-firmare)
  ;;prende il certificato e la chiave dell'utente
  (Show "CA::MakeSignature: <" id-utente "> <" stringa-base64-da-firmare ">")
  (let* (
	 (user-cert (caar (db-interface::DoSqlQuery db-pars (string-append
						    "select certificati.cert from certificati where id_utente = "
						    id-utente))))
	 (user-key (caar (db-interface::DoSqlQuery db-pars (string-append
						    "select certificati.key from certificati where id_utente = "
						    id-utente))))
	 ;;crea i file per user-cert, user-key, da-firmare e p7m
	 (p-user-cert (mkstemp! (string-copy "/tmp/user-cert-XXXXXX")))
	 (p-user-key (mkstemp! (string-copy "/tmp/user-key-XXXXXX")))
	 (p-da-firmare (mkstemp! (string-copy "/tmp/da-firmare-XXXXXX")))
	 (p-p7m (mkstemp! (string-copy "/tmp/p7m-XXXXXX")))
	 ;;Prende i nomi dei file creati
	 (f-user-cert (port-filename p-user-cert))
	 (f-user-key (port-filename p-user-key))
	 (f-da-firmare (port-filename p-da-firmare))
	 (f-p7m (port-filename p-p7m))
	 (result-p7m #nil)  ;;per utilizzarlo in seguito con la set!
	 )
    ;;ora chiude i file in modo da poterci scrivere
    (close-port p-user-cert)
    (close-port p-user-key)
    (close-port p-da-firmare)
    (close-port p-p7m)
    ;;
    ;;Ora riempie i file
    (fs-io-from-string f-user-cert user-cert)
    (fs-io-from-string f-user-key user-key)
    (fs-io-from-bv f-da-firmare (mtfa-b64-decode-bv stringa-base64-da-firmare))
    ;;
    ;;Ora posso richiamare la firma
    (system* openssl
	     "cms" "-nosmimecap"
	     "-md" "sha256"
	     "-nodetach"
	     "-binary"
	     "-cades"
	     "-stream"
	     "-outform" "DER"
	     "-sign"
	     "-signer" f-user-cert
	     "-inkey" f-user-key
	     "-in" f-da-firmare
	     "-out" f-p7m
	     )
    ;;
    ;;Ora prendo i dati firmati, cancello il resto e restituisco in base64
    (set! result-p7m (mtfa-b64-encode (fs-io-to-bv f-p7m)))
    (delete-file f-user-cert)
    (delete-file f-user-key)
    (delete-file f-da-firmare)
    (delete-file f-p7m)
    result-p7m
    ))

(define (CA::VerifySignature stringa-b64-da-verificare)
  (let* (
	 (result #nil)  ;;mi serve dopo
	 (p-file-to-verify  (mkstemp! (string-copy "/tmp/to-verify-XXXXXX")))
	 (p-result-file     (mkstemp! (string-copy "/tmp/result-file-XXXXXX")))
	 (f-to-verify (port-filename p-file-to-verify))
	 (f-result (port-filename p-result-file))
	 (d (close-port p-file-to-verify))
	 (d (close-port p-result-file))
	 (d (fs-io-from-bv f-to-verify (mtfa-b64-decode-bv stringa-b64-da-verificare)))
	 (d (system* openssl
		     "cms"
		     "-in" f-to-verify
		     "-inform" "DER"
		     "-verify"
		     "-noverify"
		     "-out" f-result
		     )))
    (if (zero? (stat:size (stat f-result)))
	(set! result #nil)
	(set! result (fs-io-to-bv f-result)))
    (delete-file f-to-verify)
    (delete-file f-result)
    result))

(define-public (CA::GetFingerprint id-utente)
  (mtfa-noerr
   #f
   (let* (
	  (user-cert (caar (db-interface::DoSqlQuery db-pars (string-append
							      "select certificati.cert from certificati where id_utente = "
							      (if (number? id-utente) (number->string id-utente) id-utente)
							      ))))
	  ;;crea il file user-cert e torna la porta di lettura/scrittura sul file
	  (p-user-cert (mkstemp! (string-copy "/tmp/user-cert-XXXXXX")))
	  ;;Prende i nomi dei file creati
	  (f-user-cert (port-filename p-user-cert))
	  (fingerprint "")
	  (openssl-result #nil)
	  )
     ;;ora chiude i file in modo da poterci scrivere con altri comandi
     (close-port p-user-cert)
     ;;
     ;;Ora riempie i file
     (fs-io-from-string f-user-cert user-cert)
     ;;
     ;;Ora posso richiamare l'estrazione del fingerprint
     ;;
     (set! openssl-result (mtfa-run-ext-prog openssl  (string-append " x509 -noout -in " f-user-cert " -fingerprint ")  ""))
     (Show! "Openssl-result: " openssl-result)
     (set! fingerprint  (ReplaceAll (caar openssl-result)  "SHA1 Fingerprint="  ""))
     ;;
     ;;Finale, cancella i file creati
     (delete-file f-user-cert)
     ;;
     ;;Torna il risultato
     fingerprint
     ))
  )

;;Ricorda che l'archivio dei certificati
;; come id= -1 ha la rootca
;; e come id=-2 ha il file conf
;; ora additional-field è il protocollo-accesso
(define (CA::MakeX509CertAndPrivateKey id-utente fiscal-code sur-name given-name email additional-field )
  (let* (
	 (config (caar (db-interface::DoSqlQuery db-pars "select certificati.cert from certificati where id_utente = -2")))
	 (rootca-cert (caar (db-interface::DoSqlQuery db-pars "select certificati.cert from certificati where id_utente = -1")))
	 (rootca-key  (caar (db-interface::DoSqlQuery db-pars "select certificati.key  from certificati where id_utente = -1")))
	 (new-serial (CA::MakeSerialNumber))
	 (config (ReplaceAllMultiple config
				     (list
				      (list "<common-name>" fiscal-code;;(string-append given-name " " sur-name)
					    )
				      (list "<serial-number>" additional-field;;fiscal-code
					    )
				      (list "<sur-name>" sur-name)
				      (list "<given-name>" given-name)
				      (list "<email>" email)
				      ;;(list "<additional-field>" additional-field)
				      )))
	 (p-file-config (mkstemp! (string-copy "/tmp/ca-config-XXXXXX")))
	 (p-file-user-key  (mkstemp! (string-copy "/tmp/user-key-XXXXXX")))
	 (p-file-user-req  (mkstemp! (string-copy "/tmp/user-req-XXXXXX")))
	 (p-file-user-cert (mkstemp! (string-copy "/tmp/user-cert-XXXXXX")))
	 (p-file-ca-cert (mkstemp! (string-copy "/tmp/ca-cert-XXXXXX")))
	 (p-file-ca-key  (mkstemp! (string-copy "/tmp/ca-key-XXXXXX")))
	 (file-config (port-filename p-file-config))
	 (file-user-key (port-filename p-file-user-key))
	 (file-user-cert (port-filename p-file-user-cert))
	 (file-user-req (port-filename p-file-user-req))
	 (file-ca-cert (port-filename p-file-ca-cert))
	 (file-ca-key (port-filename p-file-ca-key))
	 )
    ;;
    ;;Chiude i file per poterci scrivere dentro
    ;;
    (close-port p-file-config)
    (close-port p-file-user-key)
    (close-port p-file-user-cert)
    (close-port p-file-user-req)
    (close-port p-file-ca-cert)
    (close-port p-file-ca-key)
    ;;
    ;;
    ;;Prepara i file di configurazione e di input
    ;;
    (fs-io-from-string file-config config)
    (fs-io-from-string file-ca-cert rootca-cert)
    (fs-io-from-string file-ca-key rootca-key)
    ;;
    ;;
    ;;Crea il csr e la chiave privata
    ;;
    (system* openssl
	     "req"
	     "-newkey" "rsa:1024"
	     "-out" file-user-req
	     "-keyout" file-user-key
	     "-config" file-config
	     "-new"
	     )
    ;;
    ;;
    ;;Emette il certificato
    ;;
    (Show "< x509 -req -days 1825 -sha256 "
	  " -in " file-user-req
	  " -CA " file-ca-cert
	  " -CAkey " file-ca-key
	  " -out " file-user-cert
	  " -extfile " file-config
	  " -extensions x509_extensions "
	  " -set_serial " new-serial
	  ">"
	  )
    (system* openssl
	     "x509"
	     "-req"
	     "-days" "1825"
	     "-sha256"
	     "-in" file-user-req
	     "-CA" file-ca-cert
	     "-CAkey" file-ca-key
	     "-out" file-user-cert
	     "-extfile" file-config
	     "-extensions" "x509_extensions"
	     "-set_serial" new-serial
	     )
    ;;
    ;;Salva il certificato su disco
    ;;
    ;;(Show "Salva il certificato su disco")
    (db-interface::DoSqlQuery db-pars (string-append
			       "insert into certificati (certificati.id_utente, certificati.cert, certificati.key) values ( "
			       id-utente ", "
			       "'" (fs-io-to-string file-user-cert) "', "
			       "'" (fs-io-to-string file-user-key) "'"
			       ")"
			       " ON DUPLICATE KEY UPDATE "
			       "certificati.cert='" (fs-io-to-string file-user-cert) "', "
			       "certificati.key='" (fs-io-to-string file-user-key) "';"
			       ))
    ;;(Show "Salvato il certificato su disco")
    ;;
    ;;
    ;;Cancella i file
    ;;
    (delete-file file-ca-cert)
    (delete-file file-ca-key)
    (delete-file file-config)
    (delete-file file-user-req)
    (delete-file file-user-cert)
    (delete-file file-user-key)
    ))



