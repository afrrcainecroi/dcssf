(define-module (mtfa ethlib)
#:use-module (mtfa error-handler)
#:use-module (mtfa utils)
#:use-module (mtfa serializer)
#:use-module (mtfa unordered-set) ;;unordered set con chiavi (stringhe, numeri o sumimboli: tutto convertito in stringa). Persistente
#:use-module (mtfa unordered-map) ;;unordered map con chiavi (stringhe) e valori (qualsiasi cosa). persistente
#:use-module (mtfa star-map)      ;;Inserisce stringhe con o senza jolly, la stringa che definisce il jolly e il valore. Cerca le stringhe che matchano!
#:use-module (mtfa simple_db)
#:use-module (mtfa eis)
 ;;(mtfa fsm)
#:use-module (mtfa va)
#:use-module (mtfa extset)  ;;gestisce insiemi i cui elementi sono stringhe! consente operazioni di clone, set, check, get all.... Definisce una macro che consente di creare "al volo" una sottoclasse le cui istanze condividono gli stessi elementi.
#:use-module (mtfa umset)   ;;è una unordered map (non persistente) che ha stringhe come chiavi e ha insiemi di stringhe come valori. Ogni insert aggiunge all'insieme corrispondente. Definisce inoltre la mtfa-umap-list che consente di mappare liste come chiavi e qualsiasi valore come valore
#:use-module (mtfa web)
#:use-module (mtfa brg)
 ;;
 ;;i moduli di guile
#:use-module (srfi srfi-1)
#:use-module (srfi srfi-9)
#:use-module (srfi srfi-11)
 ;;(srfi srfi-18) ;;thread e mutex
 ;; date & time rinomina per avere un current time che non si sovrappone
#:use-module (srfi srfi-19)
#:use-module (srfi srfi-26)
 ;;(srfi srfi-28)
#:use-module (srfi srfi-43)
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
#:use-module (ice-9 pretty-print)
#:use-module (ice-9 regex)
#:use-module (ice-9 iconv)
#:use-module (ice-9 peg)
#:use-module (ice-9 peg string-peg)
#:use-module (ice-9 vlist)
#:use-module (ice-9 q)
#:use-module (ice-9 binary-ports)
#:use-module (ice-9 threads)
#:use-module (ice-9 hash-table)
#:use-module (ice-9 control)
#:use-module (ice-9 match)
#:use-module (ice-9 optargs)
;;
#:use-module (oop goops)
#:use-module (oop goops describe)
#:use-module (sxml simple)
#:use-module (sxml ssax)
#:use-module (sxml xpath)
#:use-module (json)
#:use-module (system syntax)
 ;;
#:use-module (web client)
 ;;
 )

;;
;;Ricostruzione di una transazione a partire dalla transazione RAW
#|
Una transazione è formata obbligatoriamente nel seguente modo
Field	Description	                                       Size (bytes)
Tn	Nonce: a very confusing name, the actual meaning 
        is “number of transactions sent by the sender”. 
        This starts from zero and increments each time a 
        transaction is sent. See more details here.	       up to 32
Tp	gasPrice	                                       up to 32
Tg	gasLimit	                                       up to 32
Tt	To address	                                       20
Tv	value	                                               up to 32
Ti / Td	Message call data(Td) / contract creation data (Ti)    unlimited
Tw	v of ECDSA signature output.	                       1  byte (value is either 27 or 28) 
                                                               without EIP-155 support, otherwise 
                                                               unlimited, see question here.
Tr	r of ECDSA signature output	                       32
Ts	s of ECDSA signature output	                       32

Gas Price is 20000000000 wei (per mainnet pari a 4a817c800 in hex)
gas estimation = 21000 units (per mainnet e una semplice transazione 5208 in hex)
gas cost estimation = 5320000000000000 wei
gas cost estimation = 0.00532 ether
                                                            5,320,000,000,000,000
                                                          471,238,800,000,000,000
                                              1 Ether = 1,000,000,000,000,000,000 WEI = 1 (EXA)WEI
                         1 (MILLI)ETHER = 0.001 ETHER = 1,000,000,000,000,000 WEI = 1 (PETA)WEI
                      1 (MICRO)ETHER = 0.000001 ETHER = 1,000,000,000,000 WEI = 1 (TERA)WEI
                    1 (Nano)ETHER = 0.000000001 ETHER = 1,000,000,000 WEI = 1 (GIGA)WEI
                 1 (PICO)ETHER = 0.000000000001 ETHER = 1,000,000 WEI = 1 (MEGA)WEI
             1 (FEMTO)ETHER = 0.000000000000001 ETHER = 1,000 WEI = 1 (KILO)WEI
           1 (ATTO)ETHER = 0.000000000000000001 ETHER = 1 WEI
|#
;;
;;utilities di conversione
(defun ToNumber (n)
  0 =>
  (if (number? n) n (string->number n 16)))
;;
(defun*-public mtfa-eth-to-wei (#:optional (amount 0) (currency ""))
  #f =>
  (set! currency (string-upcase currency))
  (match (list (ToNumber amount) currency)
    ((amount "ETHER") (inexact->exact (* amount 1000000000000000000)))
    ((amount "EWEI")  (inexact->exact (* amount 1000000000000000)))
    ((amount "TWEI")  (inexact->exact (* amount 1000000000000)))
    ((amount "GWEI")  (inexact->exact (* amount 1000000000)))
    ((amount "MWEI")  (inexact->exact (* amount 1000000)))
    ((amount "KWEI")  (inexact->exact (* amount 1000)))
    ((amount "WEI")   (inexact->exact (* amount 1)))
    (_
      (Show!
       "Cannot convert! REMEMBER!!
                                              1 Ether = 1,000,000,000,000,000,000 WEI = 1 (EXA)WEI
                         1 (MILLI)ETHER = 0.001 ETHER = 1,000,000,000,000,000 WEI = 1 (PETA)WEI
                      1 (MICRO)ETHER = 0.000001 ETHER = 1,000,000,000,000 WEI = 1 (TERA)WEI
                    1 (Nano)ETHER = 0.000000001 ETHER = 1,000,000,000 WEI = 1 (GIGA)WEI
                 1 (PICO)ETHER = 0.000000000001 ETHER = 1,000,000 WEI = 1 (MEGA)WEI
             1 (FEMTO)ETHER = 0.000000000000001 ETHER = 1,000 WEI = 1 (KILO)WEI
           1 (ATTO)ETHER = 0.000000000000000001 ETHER = 1 WEI"
       )
      #f)))
;;
(defun*-public mtfa-eth-from-wei (#:optional (amount 0) (currency ""))
  #f =>
  (set! currency (string-upcase currency))
  (match (list (ToNumber amount) currency)
    ((amount "ETHER") (exact->inexact (/ amount 1000000000000000000)))
    ((amount "EWEI")  (exact->inexact (/ amount 1000000000000000)))
    ((amount "TWEI")  (exact->inexact (/ amount 1000000000000)))
    ((amount "GWEI")  (exact->inexact (/ amount 1000000000)))
    ((amount "MWEI")  (exact->inexact (/ amount 1000000)))
    ((amount "KWEI")  (exact->inexact (/ amount 1000)))
    ((amount "WEI")   (exact->inexact (/ amount 1)))
    (_
      (Show!
       "Cannot convert! REMEMBER!!
                                              1 Ether = 1,000,000,000,000,000,000 WEI = 1 (EXA)WEI
                         1 (MILLI)ETHER = 0.001 ETHER = 1,000,000,000,000,000 WEI = 1 (PETA)WEI
                      1 (MICRO)ETHER = 0.000001 ETHER = 1,000,000,000,000 WEI = 1 (TERA)WEI
                    1 (Nano)ETHER = 0.000000001 ETHER = 1,000,000,000 WEI = 1 (GIGA)WEI
                 1 (PICO)ETHER = 0.000000000001 ETHER = 1,000,000 WEI = 1 (MEGA)WEI
             1 (FEMTO)ETHER = 0.000000000000001 ETHER = 1,000 WEI = 1 (KILO)WEI
           1 (ATTO)ETHER = 0.000000000000000001 ETHER = 1 WEI"
       )
      #f)))
;;
;;
(define-public mtfa-eth-empty-address "0000000000000000000000000000000000000000")
;;
(define-public (mtfa-eth-build-infura-req method params id)
  (when (nil? params) (set! params (make-vector 0)))
  `(("jsonrpc" . "2.0") ("method" . ,method) ("params" . ,params) ("id" . ,id))
  )
;;
;;I client per inviare le richieste INFURA!!!
;; curl https://mainnet.infura.io/v3/YOUR-PROJECT-ID \
;;     -X POST \
;;     -H "Content-Type: application/json" \
;;     -d '{"jsonrpc":"2.0","method":"eth_blockNumber","params": [],"id":1}'
(defun* mtfa-eth-get-infura (net path method #:optional (params #()) (id 1))
  #f =>
  ;;(Show! (scm->json-string (mtfa-eth-build-infura-req method params id)))
  (json-string->scm 
   (mtfa-bytevector->string
    (second
     (if (string-ci=? net "ganache")
	 (mtfa-http-get-page-ex "10.211.55.180"  ;;mainnet oppure ropsten
			    "7545"
			    "HTTP"
			    "POST"
			    path ;;"/v3/e109754bca1343a0aa2935179a80878c"
			    "Content-Type: application/json"
			    (scm->json-string (mtfa-eth-build-infura-req method params id))
			    ""
			    ""
			    0)
	 (mtfa-http-get-page-ex net  ;;mainnet oppure ropsten
			    "443"
			    "HTTPS"
			    "POST"
			    path ;;"/v3/e109754bca1343a0aa2935179a80878c"
			    "Content-Type: application/json"
			    (scm->json-string (mtfa-eth-build-infura-req method params id))
			    ""
			    ""
			    0))))))
(export mtfa-eth-get-infura)
;;
;; (defun ElencoRichieste ()
;;   (GetInfura ethereum-ropsten "eth_accounts")
;;   (GetInfura ethereum-ropsten "eth_blockNumber")
;;   (GetInfura ethereum-ropsten "eth_chainId")
;;   (GetInfura ethereum-ropsten "eth_gasPrice")
;;   (GetInfura ethereum-ropsten "eth_getBalance" #("0x1f53d15Ca271bBe0a02528D6F930bBb5d12DeB53a" "latest"))
;;   (GetInfura ethereum-ropsten "eth_getTransactionCount" #("0xf53d15Ca271bBe0a02528D6F930bBb5d12DeB53a" "latest"))

;;   (GetInfura ethereum-ropsten "eth_getWork")
;;   (GetInfura ethereum-ropsten "eth_hashrate")
;;   (GetInfura ethereum-ropsten "eth_mining")
;;   (GetInfura ethereum-ropsten "eth_protocolVersion")
;;   (GetInfura ethereum-ropsten "net_version")
;;   (GetInfura ethereum-ropsten "net_peerCount")
;;   (GetInfura ethereum-ropsten "net_listening")
;;   (GetInfura ethereum-ropsten "eth_syncing")
;;   (GetInfura ethereum-ropsten "eth_sendRawTransaction")
  

;;   #|
;;   eth_getBlockByHash, eth_getBlockByNumber, eth_getBlockTransactionCountByHash, 
;;   eth_getBlockTransactionCountByNumber eth_getCode
;;   eth_getLogs
;;   eth_getStorageAt
;;   eth_getTransactionByBlockHashAndIndex eth_getTransactionByBlockNumberAndIndex
;;   eth_getTransactionByHash 
;;   eth_getTransactionReceipt
;;   eth_submitWork
;;   |#
;;   )
;;
;;la conversione a rlp
(defun NumBytes (len)
  #f =>
  ;;(Show "NumBytes called with len=" len "(" (string? len) ")")
  (let ((num (number->string len 16)))
    (when (not (zero? (remainder (string-length num) 2)))
      (set! num (string-append "0" num)))
    (/ (string-length num) 2)))
(defun tohex (u8l)
  (map (lambda (i) (let
		  ((c (number->string i 16)))
		(if (= 1 (string-length c))
		    (string-append "0" c)
		    c))) u8l))
;; For a single byte whose value is in the [0x00, 0x7f] range, that byte is its own RLP encoding.
;; Otherwise, if a string is 0-55 bytes long, the RLP encoding consists of a single byte with value 0x80 plus the length of the string followed by the string. The range of the first byte is thus [0x80, 0xb7].
;; If a string is more than 55 bytes long, the RLP encoding consists of a single byte with value 0xb7 plus the length in bytes of the length of the string in binary form, followed by the length of the string, followed by the string. For example, a length-1024 string would be encoded as \xb9\x04\x00 followed by the string. The range of the first byte is thus [0xb8, 0xbf].
(defun ConvertToRlp (bv)
  #f =>
  (cond
   ((number? bv)
    (set! bv (mtfa-num-to-bv bv)))
   ((string? bv)
    (when (not (zero? (remainder (string-length bv) 2))) (set! bv (string-append "0" bv)))
    (set! bv (mtfa-hex-string-to-bv bv)))
   (#t #t))
  (let ((len (bytevector-length bv))
	(lbv (bytevector->u8-list bv))
	(lret (bytevector->u8-list (make-bytevector 0)))
	)
    (if (= 1 len)
	(append lret  (list (bytevector-u8-ref bv 0)))
	(begin
	  (if (<= len 55)
	      (append lret (list (+ #x80 len)) lbv)
	      (append lret (list (+ #xb7 (NumBytes len)) len) lbv))))))
;;
;; If the total payload of a list (i.e. the combined length of all its items being RLP encoded) is 0-55 bytes long, the RLP encoding consists of a single byte with value 0xc0 plus the length of the list followed by the concatenation of the RLP encodings of the items. The range of the first byte is thus [0xc0, 0xf7].
;; If the total payload of a list is more than 55 bytes long, the RLP encoding consists of a single byte with value 0xf7 plus the length in bytes of the length of the payload in binary form, followed by the length of the payload, followed by the concatenation of the RLP encodings of the items. The range of the first byte is thus [0xf8, 0xff].
(defun-public mtfa-eth-scmjson-to-hexrlp (l)
  #f =>
  ;;per prima cosa faccio tutti i pezzetti
  (let ((lall (map ConvertToRlp l))
	(lall-len #f)
	(lret #nil)
	)
    (set! lall-len (fold (lambda (c p) (+ (length c) p)) 0 lall))
    (if (<= lall-len 55)
	(set! lall (append (tohex (list (+ #xc0 lall-len))) (map tohex lall)))
	(set! lall (append (tohex (list (+ #xf7 (NumBytes lall-len)) lall-len)) (map tohex lall))))
    ;;ora devo appiattire questa lista di lista
    (fold-right (lambda (c p)
		  (string-append (if (list? c)
				     (fold-right (lambda (c1 p1) (string-append c1 p1)) "" c)
				     c
				     )
				 p))
		""
		lall
		)
    ))

;;questa è la ricostruzione, la costruzione è il contrario
(defun-public mtfa-eth-hexrlp-to-scmjson (hexrlp)
  (mtfa-rlp-to-scmjson (mtfa-hex-string-to-bv hexrlp)))
;;
(defun-public mtfa-eth-build-transaction (nonce gasPrice gasLimit toAddress value data)
  (list `("nonce" . ,nonce)
	`("gasPrice" . ,gasPrice)
	`("gasLimit" . ,gasLimit)
	`("to" . ,toAddress)
	`("value" . ,value)
	`("data" . ,data)))
;;
;;La costruzione della transazione (network=1 per mainnet e 3 per ropsten)
(defun-public mtfa-eth-build-signed-transaction (trans network hex-prk)
  #f =>
  (let* ((nonce (assoc-ref trans "nonce"))
	 (gasPrice (assoc-ref trans "gasPrice"))
	 (gasLimit (assoc-ref trans "gasLimit"))
	 (toAddress (assoc-ref trans "to"))
	 (value (assoc-ref trans "value"))
	 (data (assoc-ref trans "data"))
	 (v network)
	 (r "")
	 (s "")
	 (recovery #nil)
	 (rlp #nil)
	 (signed #nil)
	 (prk (mtfa-hex-string-to-bv hex-prk))
	 (puk (mtfa-eth-get-pub-key prk))
	 (hmsg #nil)
	 )
    ;;
    ;;alcuni valori dovrei prenderli dalla rete
    (when (equal? #f gasLimit) (set! gasLimit 21000)) ;;max 21000 units of gas
    (when (equal? #f gasPrice) (set! gasPrice 2000000000)) ;;2 GWei
    (when (and (equal? #f data) (equal? #f toAddress))
      (error "if toAddress is empty, data must be filled (calling a smart contract)"))
    (when (equal? data #f) (set! data ""))
    (when (equal? value #f) (set! value ""))
    (when (equal? #f nonce)
      (error "nonce must be defined"))
    ;;
    ;;ora convertiamo in un vettore
    ;;a questo punto costruiamo il vettore della transazione, da serializzare rlp
    (Show "TRANS: " (list nonce ", " gasPrice ", " gasLimit ", " toAddress ", " value ", " data ", " v ", " r ", " s))
    (Show "RLP  : " (mtfa-eth-scmjson-to-hexrlp (list nonce gasPrice gasLimit toAddress value data v r s)) )
    (set! rlp (mtfa-eth-scmjson-to-hexrlp (list nonce gasPrice gasLimit toAddress value data v r s)))
    (set! hmsg (mtfa-hash-keccak (mtfa-hex-string-to-bv rlp)))
    ;;
    ;;Ora firmo e ottengo risultato di firma
    (set! signed (mtfa-bv-to-hex-string (mtfa-eth-serialize-signature (mtfa-eth-sign-msg prk puk hmsg))))
    ;;
    ;;Calcolo v,r,s
    ;; r: sig.signature.slice(0, 32),
    ;; s: sig.signature.slice(32, 64),
    ;; v: chainId ? recovery + (chainId * 2 + 35) : recovery + 27,
    (set! r (substring signed 2 (+ 2 64)))
    (set! s (substring signed (+ 2 64) (+ 2 128)))
    (set! recovery (string->number (substring signed 0 2) 16))
    (set! v (+ recovery (+ 35 (* 2 network))))
    ;;
    ;;ora posso costruire la transazione completa
    (set! rlp (mtfa-eth-scmjson-to-hexrlp (list nonce gasPrice gasLimit toAddress value data v r s)))
    ;;
    ;;per ora mostro rlp finale
    rlp
  ))
;;
;;Il prk in bytevector oppure in stringa hex
(defun-public mtfa-eth-get-ethereum-address (prk)
  (when (string? prk)
    (set! prk (mtfa-hex-string-to-bv prk)))
  (string-take-right (mtfa-bv-to-hex-string (mtfa-hash-keccak (mtfa-hex-string-to-bv (substring (mtfa-bv-to-hex-string (mtfa-eth-get-pub-key-serialized  prk)) 2)))) 40))
;;
;;
(defun isNilOrEmpty (s)
  (or (nil? s) (and (string? s) (zero? (string-length s)))))
(defun add0x (s)
  (when (isNilOrEmpty s)
    (return s))
  (if (string? s)
      (if (> (string-length s) 2)
	  (if (string=? "0x" (substring s 0 2))
	      s ;;nulla da convertire
	      (string-append "0x" s)
	      )
	  (string-append "0x" s))
      (string-append "0x" (number->string s 16))))
;;
;;
(defun-public mtfa-eth-infura-gasPrice (net path)
  #f =>
  (substring (assoc-ref (mtfa-eth-get-infura net path "eth_gasPrice") "result") 2))
;;
;;
(defun-public mtfa-eth-infura-estimateGas (net path from to gasPrice value hdata)
  #f =>
  (when (isNilOrEmpty from) (set! from mtfa-eth-empty-address))
  (when (isNilOrEmpty to) (set! to mtfa-eth-empty-address))
  (let ((ret (mtfa-eth-get-infura net path "eth_estimateGas"
				  (vector `(("from" . ,(add0x from)) 
					    ("to" . ,(add0x to)) 
					    ("gasPrice" . ,(add0x gasPrice))
					    ("value" . ,(add0x value))
					    ("data" . ,(add0x hdata))
					    ))
				  )))
    ;;(Show ret)
    (substring (assoc-ref ret "result") 2)))
;;
;;
(defun-public mtfa-eth-infura-getBalance (net path who)
  #f =>
  (let ((ret (mtfa-eth-get-infura net path "eth_getBalance"
				  (vector (add0x who) "latest")
				  )))
    ;;(Show ret)
    (substring (assoc-ref ret "result") 2)))
;;
;;
(defun-public mtfa-eth-infura-getTransactionCount (net path who latest-earliest-pending)
  #f =>
  (let ((ret (mtfa-eth-get-infura net path "eth_getTransactionCount"
				  (vector (add0x who) latest-earliest-pending)
				  )))
    ;;(Show ret)
    (substring (assoc-ref ret "result") 2)))
;;
;;
(defun-public mtfa-eth-infura-sendRawTransaction (net path signed)
  (let ((ret #f))
    (mtfa-noerr
     (values #f ret)
     (set! ret (mtfa-eth-get-infura net path "eth_sendRawTransaction" (vector (add0x signed))))
     (values #t (substring (assoc-ref ret "result") 2)))))
;;
;;
(defun-public mtfa-eth-infura-getTransactionReceipt (net path trans-hash)
  #f =>
  (when (not (string? trans-hash)) 
    (return #f))
  (let ((ret (mtfa-eth-get-infura net path "eth_getTransactionReceipt"
				  (vector (add0x trans-hash))
				  )))
    ;;(Show ret)
    (set! ret (assoc-ref ret "result"))
    (ifnot ret
	   #f
	   ret)))
;;
;;
(defun-public mtfa-eth-transfer (net path user1 user1-prk user2 howmuch)
  (let ((nonce #f)
	(gasPrice #f)
	(gasLimit #f)
	(trans #f)
	(signed #f)
	(htrans #f)
	(result #f)
	)
    (set! nonce (mtfa-eth-infura-getTransactionCount net path user1 "pending"))
    (when-false nonce (return (values #f 0)))
    ;;
    (set! gasPrice (mtfa-eth-infura-gasPrice net path))
    (when-false gasPrice (return (values #f 1)))
    ;;
    (set! gasLimit (mtfa-eth-infura-estimateGas net path user1 user2 gasPrice howmuch ""))
    (when-false gasLimit (return (values #f 2)))
    ;;
    ;;Raddoppia il gasLimit
    (set! gasLimit (* 2 (string->number gasLimit 16)))
    ;;
    (set! trans (mtfa-eth-build-transaction nonce gasPrice gasLimit user2 howmuch ""))
    ;;
    (set! signed (mtfa-eth-build-signed-transaction trans 3 user1-prk))
    (when-false signed (return (values #f 4)))
    ;;
    (mtfa-eth-infura-sendRawTransaction net path signed)
    ))




;; ;;La prova di firma
;; (defun ProvaDiFirma ()
;;   #f =>
;; #|
;;   prk:  74a37f5614d8e093e9e66443acce626f50f32525c58d667987e6824a863f9373
;;   from: 0xf53d15Ca271bBe0a02528D6F930bBb5d12DeB53a
;;   to:   0x9938b093954c0F8Cb69B0a930318300f2d454ab2
;;   ea0a843b9aca00825208949938b093954c0f8cb69b0a930318300f2d454ab2872386f26fc1000080808080
;; => #("a" "3b9aca00" "5208" "9938b093954c0f8cb69b0a930318300f2d454ab2" "2386f26fc10000" "0" "0" "0" "0")
;; Serialized:  f86a0a843b9aca00825208949938b093954c0f8cb69b0a930318300f2d454ab2872386f26fc10000802aa0160d1c4db76c289568dae55e4b5f9d7a117ff8023253d427fe576469c59429e8a00cb17d66dbefbc1ffb3be8a3e5bcafb4d6a6434a3f955ba63ca72b48d3698056
;; => #("a" "3b9aca00" "5208" "9938b093954c0f8cb69b0a930318300f2d454ab2" "2386f26fc10000" "0" "2a" "160d1c4db76c289568dae55e4b5f9d7a117ff8023253d427fe576469c59429e8" "cb17d66dbefbc1ffb3be8a3e5bcafb4d6a6434a3f955ba63ca72b48d3698056")
;;   valori di V per ropsten: 29, 2a => 41/42
;;   42-6=36 | 41-6=35
;;   3*2+35=41 se pari +0, se dispari +1
;;   chainID * 2 + 35 + RecoveryID
;;   V,R,S
;;   questo mi convince di meno
;;   v = 27 + (y % 2), so 27 + the parity of r, as pybitcointools indicates.
;;   So for even r, we get v = 27, odd r we get v = 28.
;; |#
;;   (let* ((prk (mtfa-hex-string-to-bv "74a37f5614d8e093e9e66443acce626f50f32525c58d667987e6824a863f9373"))
;; 	(puk (mtfa-eth-get-pub-key prk))
;; 	(amsg "ea0c843b9aca00825208949938b093954c0f8cb69b0a930318300f2d454ab2871ff973cafa800080808080")
;; 	(msg (mtfa-hex-string-to-bv "ea0c843b9aca00825208949938b093954c0f8cb69b0a930318300f2d454ab2871ff973cafa800080808080"))
;; 	(msgb (mtfa-rlp-to-scmjson (mtfa-hex-string-to-bv "ea0c843b9aca00825208949938b093954c0f8cb69b0a930318300f2d454ab2871ff973cafa800080808080")))
;; 	(rmsg (u8-list->bytevector (vector-fold-right (lambda (i p c) (append (bytevector->u8-list (mtfa-hex-string-to-bv c)) p)) #nil msgb)))
;; 	;;f86a0c843b9aca00825208949938b093954c0f8cb69b0a930318300f2d454ab2871ff973cafa8000802aa09f44a26a48ab7f4c41c4c2e8a80e29f877f35f600dc105076ab57df8982fbc66a00a14db1ba3948875fa02cd072631e6b27e7c7f676645b656c45e24b63cdb6022
;; 	;;#("c" "3b9aca00" "5208" "9938b093954c0f8cb69b0a930318300f2d454ab2" "1ff973cafa8000" "0" "0" "0" "0")
;; 	;;("c" "3b9aca00" "5208" "9938b093954c0f8cb69b0a930318300f2d454ab2" "1ff973cafa8000" "0" "2a" "9f44a26a48ab7f4c41c4c2e8a80e29f877f35f600dc105076ab57df8982fbc66" "a14db1ba3948875fa02cd072631e6b27e7c7f676645b656c45e24b63cdb6022")
;; 	;;                                                                                             47cd60cb289f6986b64bcdf7aefe482376ae52bdcb8a607fc062d4c18861a5f6   c219752c31976f3728bbad1540cef4affb1df74d24c6bb3c32d93c39a5d83b27
;; 	)
;;     (Show msgb)
;;     (mtfa-eth-sign-msg prk puk msg)
;;   ))

;; ;; (define prk (mtfa-hex-string-to-bv "74a37f5614d8e093e9e66443acce626f50f32525c58d667987e6824a863f9373"))
;; ;; (define puk (mtfa-eth-get-pub-key prk))
;; ;; (define msg "eb108502540be400825208949938b093954c0f8cb69b0a930318300f2d454ab2871ff973cafa800080808080")
;; ;; (define signed (mtfa-eth-sign-msg prk puk (mtfa-hash-keccak (mtfa-hex-string-to-bv msg))))
;; ;; (define serialized (mtfa-eth-serialize-signature signed))
;; ;; (mtfa-bv-to-hex-string serialized)


;; ;; (define prk (mtfa-hex-string-to-bv "97ddae0f3a25b92268175400149d65d6887b9cefaf28ea2c078e05cdc15a3c0a"))
;; ;; (define puk (mtfa-eth-get-pub-key prk))
;; ;; (define msg (mtfa-string->bytevector "Message for signing"))
;; ;; (define signed (mtfa-eth-sign-msg prk puk (mtfa-hash-keccak (mtfa-hex-string-to-bv msg))))
;; ;; (define serialized (mtfa-eth-serialize-signature signed))
;; ;; (mtfa-bv-to-hex-string serialized)

;; ;; ea0f844190ab00825208949938b093954c0f8cb69b0a930318300f2d454ab2871ff973cafa800080808080
;; ;; (mtfa-rlp-to-scmjson (mtfa-hex-string-to-bv "f86a0f844190ab00825208949938b093954c0f8cb69b0a930318300f2d454ab2871ff973cafa8000802aa0dfaac550bd465f557d3e57c979355913ef723b2585ea7e8674be88321a5a8738a00ab42831c5cb4319816e55985df2ea03f70ae4f6273be3147e212e63e5c73332"))
;; ;; $8 = #("f" "4190ab00" "5208" "9938b093954c0f8cb69b0a930318300f2d454ab2" "1ff973cafa8000" "0" "2a" "dfaac550bd465f557d3e57c979355913ef723b2585ea7e8674be88321a5a8738" "ab42831c5cb4319816e55985df2ea03f70ae4f6273be3147e212e63e5c73332")


;; ;;
;; ;;Calcolo degli ID ethereum
;; #|

;; Table 4-1. Serialized EC public key prefixes
;; Prefix	Meaning	Length (bytes counting prefix)
;; 0x00 Point at infinity 1
;; 0x04 Uncompressed point 65
;; 0x02 Compressed point with even y 33
;; 0x03 Compressed point with odd y  33

;; Ethereum only uses uncompressed public keys; therefore the only prefix that is relevant is (hex) 04. The serialization concatenates the x and y coordinates of the public key:

;; 04 + x-coordinate (32 bytes/64 hex) + y-coordinate (32 bytes/64 hex)

;; G(compressed) = 02 79BE667E F9DCBBAC 55A06295 CE870B07 029BFCDB 2DCE28D9 59F2815B 16F81798

;; and

;; G(uncompressed) = 04 79BE667E F9DCBBAC 55A06295 CE870B07 029BFCDB 2DCE28D9 59F2815B 16F81798 483ADA77 26A3C465 5DA4FBFC 0E1108A8 FD17B448 A6855419 9C47D08F FB10D4B8
;; => x=79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798 
;; => y=483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8
;; y 2 mod p = ( x 3 + 7 ) mod p


;; Ethereum Addresses
;; Ethereum addresses are unique identifiers that are derived from public keys or contracts using the Keccak-256 one-way hash function.

;; In our previous examples, we started with a private key and used elliptic curve multiplication to derive a public key:

;; Private key k:

;; k = f8f8a2f43c8376ccb0871305060d7b27b0554d2cc72bccf41b2705608452f315
;; Public key K (x and y coordinates concatenated and shown as hex):
;; K = 6e145ccef1033dea239875dd00dfb4fee6e3348b84985c92f103444683bae07b83b5c38e5e...
;; 7be0ba83464403f1925c98848b34e3e6feb4df00dd759823ea3d03f1ce5c146ed0cc324cc842d07794ab4cc19a2dce1eaa6dd4643ffad729850c2b5e8ec3b583
;; NOTE
;; It is worth noting that the public key is not formatted with the prefix (hex) 04 when the address is calculated.

;; We use Keccak-256 to calculate the hash of this public key:

;; Keccak256(K) = 2a5bc342ed616b5ba5732269001d3f1ef827552ae1114027bd3ecf1f086ba0f9
;; Then we keep only the last 20 bytes (least significant bytes), which is our Ethereum address:

;; 001d3f1ef827552ae1114027bd3ecf1f086ba0f9

;; Most often you will see Ethereum addresses with the prefix 0x that indicates they are hexadecimal-encoded, like this:

;; 0x001d3f1ef827552ae1114027bd3ecf1f086ba0f9

;; COME?
;; (define prk (mtfa-hex-string-to-bv "f8f8a2f43c8376ccb0871305060d7b27b0554d2cc72bccf41b2705608452f315"))
;; (string-take-right (mtfa-bv-to-hex-string (mtfa-hash-keccak (mtfa-hex-string-to-bv (substring (mtfa-bv-to-hex-string (mtfa-eth-get-pub-key-serialized  prk)) 2)))) 40)

;; |#

;; ;;
;; ;;questi sono i formati!!!
;; #|
;; (bytevector->string (mtfa-rlp-to-json::internal (mtfa-hex-string-to-bv "f86b038504a817c800825208943f9c05871ebbd429608770a3382c41528958f5c9872386f26fc100008026a0642a62309dfa33c327ef5a1f3faedfb6bff895003fb50f264f5fb681052fab43a07f1b0413cb3b3d09d216566237d01f96c8b56d11105c12bef1bb380c6c53b19e")) "iso-8859-1")
;; $2 = "[\"\\u0003\",\"\\u0004¨\\u0017È\\u0000\",\"R\\b\",\"?\x9c\\u0005\x87\\u001e»Ô)`\x87p£8,AR\x89XõÉ\",\"#\x86òoÁ\\u0000\\u0000\",\"\",\"&\",\"d*b0\x9dú3Ã'ïZ\\u001f?®ß¶¿ø\x95\\u0000?µ\\u000f&O_¶\x81\\u0005/«C\",\"\\u007f\\u001b\\u0004\\u0013Ë;=\\tÒ\\u0016Vb7Ð\\u001f\x96Èµm\\u0011\\u0010\\\\\\u0012¾ñ»8\\flS±\x9e\"]"
;; e quindi, per decifrare
;; (vector-map (lambda (i v) (mtfa-bv-to-num (mtfa-string->bytevector v)))(json-string->scm (bytevector->string (mtfa-rlp-to-json::internal (mtfa-hex-string-to-bv "f86e048504a817c800825208943f9c05871ebbd429608770a3382c41528958f5c98aeb4964f32ddd851900008026a0f605e60b2e9f80819890f6a87f018d435568e5b51ad86b33542d9813fbf280d7a01c490c38787b87064dab2545700d0da8aa0f3dedf00878db44601860c932e321")) "ISO-8859-1")))
;; $10 = #(4 1339927115827200 21000 1718971408547687022934873660918576404782352985896957067154656301794185 3968640602724480583052730169884672 0 38 1974120201651396111268985111039299756564975532230417012381090045197075817673348030379333097210493163839363031126289466263 60417125359449890538966008822906519835089587606878464533872939086224450284759168931782545773929249)
;; |#
;; ;;

;; ;;Esempio:
;; ;; nonce   : 3
;; ;; gasPrice: 20000000000
;; ;; gasLimit: 21000
;; ;; toAddr  : 3f9c05871ebbd429608770a3382c41528958f5c9
;; ;; value   : 10000000000000000
;; ;; msgcall : 0
;; ;; v       : 0
;; ;; r       : 0
;; ;; s       : 0
;; ;;ee0485 012a05f200 825208 94 3f9c05871ebbd429608770a3382c41528958f5c9 8a eb4964f32ddd8519000 080808080
;; ;;ee0485 012a05f200 825208 94 3f9c05871ebbd429608770a3382c41528958f5c9 8a eb4964f32ddd8519000 080808080
;; ;;ee048  012a05f200 825208 94 3f9c05871ebbd429608770a3382c41528958f5c9 8a eb4964f32ddd8519000 080808080
;; ;;f80486 012a05c3b2008252089d3fc29c05c2871ec2bbc3942960c28770c2a3382c4152c28958c3b5c3898ec3ab4964c3b32dc39dc28519000080808080
;; ;;e9a833663963303538373165626264343239363038373730613333383263343135323839353866356339
;; ;;e9a833663963303538373165626264343239363038373730613333383263343135323839353866356339
;; ;;e9a833663963303538373165626264343239363038373730613333383263343135323839353866356339
;; ;,ea01a833663963303538373165626264343239363038373730613333383263343135323839353866356339
;; ;;cf8ec3ab4964c3b32dc39dc285190000
;; ;;eb4964f32ddd85190000
;; (defun esempio ()
;;   (mtfa-eth-serialize-transaction '(("nonce"     . 4) 
;; 				    ("gasPrice"  . 5000000000)
;; 				    ("gasLimit"  . 21000)
;; 				    ("toAddress" . "3f9c05871ebbd429608770a3382c41528958f5c9")
;; 				    ("value"     . "eb4964f32ddd85190000")
;; 				    ("msgcall"   . 0)
;; 				    ("v"         . 0)
;; 				    ("r"         . 0)
;; 				    ("s"         . 0))))

;; (defun Ricostruzione ()
;;   (let* (
;; 	 (raw "f849808609184e72a00082271094000000000000000000000000000000000000000080a47f7465737432000000000000000000000000000000000000000000000000000000600057808080")
;; 	 (raw "f86780862d79883d2000825208945df9b87991262f6ba471f09758cde1c0fc1de734827a69801ca088ff6cf0fefd94db46111149ae4bfc179e9b94721fffd821d38d16464b3f71d0a045e0aff800961cfce805daef7016b9b675c137a6a41a548f7b60a3484c06a33a")
;; 	 (raw "ec098504a817c800825208943535353535353535353535353535353535353535880de0b6b3a764000080018080")
;; 	 (raw "f86e820678850430e2340083015f9094ad322de69695859fc84f32d0f42c3802fe1018438501dcd650008082266ea027caed8171ad1857ff259554614152cda78949adda001e24472f84840bca5cd6a04a5f557baae23ce45c97b71363ea8da6740ac2652bd02b7f94b18cae62d7905a")
;; 	 (raw "F86A8086D55698372431831E848094F0109FC8DF283027B6285CC889F5AA624EAC1F55843B9ACA008025A009EBB6CA057A0535D6186462BC0B465B561C94A295BDB0621FC19208AB149A9CA0440FFD775CE91A833AB410777204D5341A6F9FA91216A6F3EE2C051FEA6A0428")
;; 	 (raw "f86a018477359400825208943f9c05871ebbd429608770a3382c41528958f5c9872386f26fc100008026a04b095ca6368c4a941a79ef40141940420c21db170b742c24a4e5784c273ac61da02a407d11ac36652355be3e730f0a4b0da343451ee2f110a3be08046c6a42f725")
;; 	 (raw "f86b028504a817c800825208943f9c05871ebbd429608770a3382c41528958f5c9872386f26fc100008025a0f6d04a1f7e20c0df9f866a03701fdbc1f3a402b737deb0e779cb0f4fd26bf98fa041f6444859de5037ee2b29bc37317824f4a8d42dc97e831ca715078bea14fa45")
;; 	 (raw "eb038504a817c800825208943f9c05871ebbd429608770a3382c41528958f5c9872386f26fc1000080808080")
;; 	 (decoded (mtfa-hexrlp-to-scmjson raw)) ;;ottengo un vettore di stringhe
;; 	 ;;per ogni elemento del vettore, lo converto in intero (e poi in stringa esadecimale per confrontarlo con il costruttore della transazione
;; 	 ;; in ordine di apparizione
;; 	 ;; - nonce
;; 	 ;; - gasPrice
;; 	 ;; - gasLimit
;; 	 ;; - toAddress
;; 	 ;; - value
;; 	 ;; - message call
;; 	 ;; - v
;; 	 ;; - r
;; 	 ;; - s
;; 	 (vector-of-nums (vector-map
;; 			  (lambda (i v)
;; 			    (if (zero? (string-length v))
;; 				0
;; 				(mtfa-bv-to-num (mtfa-string->bytevector v)))
;; 			    )
;; 			  decoded))
;; 	 )
;;     (Show! "DecodeLenght: " (vector-length decoded))
;;     (Show! "nonce   : " (vector-ref decoded 0))
;;     (Show! "gasPrice: " (string->number (vector-ref decoded 1) 16))
;;     (Show! "gasLimit: " (string->number (vector-ref decoded 2) 16))
;;     (Show! "toAddr  : " (vector-ref decoded 3))
;;     (Show! "value   : " (string->number (vector-ref decoded 4) 16))
;;     (Show! "msgcall : " (vector-ref decoded 5))
;;     (Show! "v       : " (vector-ref decoded 6))
;;     (Show! "r       : " (vector-ref decoded 7))
;;     (Show! "s       : " (vector-ref decoded 8))
;;     (Show vector-of-nums)
;;     (Show (vector-map (lambda (i v) (number->string v 16)) vector-of-nums))))

;; #|
;; transaction:  { to: '0x3F9C05871EbBD429608770a3382C41528958F5c9',
;;   value: '0x2386f26fc10000',
;;   gas: 21000,
;;   gasPrice: 2000000000,
;;   nonce: 1,
;;   chainId: 3 }
;; Serialized:  f86a018477359400825208943f9c05871ebbd429608770a3382c41528958f5c9872386f26fc100008026a04b095ca6368c4a941a79ef40141940420c21db170b742c24a4e5784c273ac61da02a407d11ac36652355be3e730f0a4b0da343451ee2f110a3be08046c6a42f725
;; 2019-7-15 2:50:30 PM	{   transactionHash:   "0x272ce1996a98f7f8e5adbf0703f55eadff04634d037fe07d686bb5107…",
;;                     	   transactionIndex:    0,
;;                     	          blockHash:   "0xe70cf26f494fb97f6216e2327f6528f471bd7a4bbb78d03a83436046f…",
;;                     	        blockNumber:    2,
;;                     	               from:   "0x1eace4798c7d46f70b8645bf4d3a8afae5831a94",
;;                     	                 to:   "0x3f9c05871ebbd429608770a3382c41528958f5c9",
;;                     	            gasUsed:    21000,
;;                     	  cumulativeGasUsed:    21000,
;;                     	    contractAddress:    null,
;;                     	               logs: [],
;;                     	             status:    true,
;;                     	          logsBloom:   "0x000000000000000000000000000000000000000000000000000000000…",
;;                     	                  v:   "0x26",
;;                     	                  r:   "0x4b095ca6368c4a941a79ef40141940420c21db170b742c24a4e5784c2…",
;;                     	                  s:   "0x2a407d11ac36652355be3e730f0a4b0da343451ee2f110a3be08046c6…"  }
;; |#
;; ;;La firma di una transazione deve tornare v,r,s
;; (defun mtfa-eth-hex-sign (prk hmsg)
;;   (mtfa-bv-to-hex-string
;;     (mtfa-eth-serialize-signature 
;;      (mtfa-eth-sign-msg (mtfa-hex-string-to-bv prk)
;; 			(mtfa-eth-get-pub-key (mtfa-hex-string-to-bv prk))
;; 			(mtfa-hex-string-to-bv hmsg))))
;;   )

;; #|Dimostrazione che la firma è corretta
;; arcieri@ubuntu:/volume1/sources/NEW_DEVS/DEPLOYED/ETHEREUM$ node go2.js 
;; Private key: 97ddae0f3a25b92268175400149d65d6887b9cefaf28ea2c078e05cdc15a3c0a
;; Public key : 7b83ad6afb1209f3c82ebeb08c0c5fa9bf6724548506f2fb4f991e2287a77090177316ca82b0bdf70cd9dee145c3002c0da1d92626449875972a27807b73b42e
;; Public key (compressed): 027b83ad6afb1209f3c82ebeb08c0c5fa9bf6724548506f2fb4f991e2287a77090

;; Msg: Message for signing
;; Msg hash: ce7df6b1b2852c5c156b683a9f8d4a8daeda2f35f025cb0cf34943dcac70d6a3
;; Signature: Signature {
;;   r:
;;    <BN: 6f0156091cbe912f2d5d1215cc3cd81c0963c8839b93af60e0921b61a19c5430>,
;;   s:
;;    <BN: c71006dd93f3508c432daca21db0095f4b16542782b7986f48a5d0ae3c583d4>,
;;   recoveryParam: 1 }

;; Recovered pubKey: 027b83ad6afb1209f3c82ebeb08c0c5fa9bf6724548506f2fb4f991e2287a77090
;; Signature valid? true

;; Fatto con il nostro codice:
;; scheme@(guile-user)> (mtfa-eth-hex-sign "97ddae0f3a25b92268175400149d65d6887b9cefaf28ea2c078e05cdc15a3c0a" (mtfa-bv-to-hex-string (mtfa-string->bytevector "Message for signing")))
;; WARNING: (mtfa utils): `string->bytevector' imported from both (rnrs io ports) and (ice-9 iconv)
;; ven lug 19 09:54:53+0200 2019 SERIALIZED: 6f0156091cbe912f2d5d1215cc3cd81c0963c8839b93af60e0921b61a19c54300c71006dd93f3508c432daca21db0095f4b16542782b7986f48a5d0ae3c583d4
;; ven lug 19 09:54:53+0200 2019 NOT SERZED: 30549ca1611b92e060af939b83c863091cd83ccc15125d2d2f91be1c0956016fd483c5e30a5d8af486792b784265b1f49500db21cada32c408353fd96d00710c

;; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
;; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
;; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
;; La firma della transazione prevede che i campi vuoti siano RLP come "", che V sia la rete (1 mainnet, 3 ropsten) e gli ultimo due (R e S), vuoti "" "":
;; (mtfa-bv-to-hex-string (mtfa-hash-keccak (mtfa-hex-string-to-bv (ConvertListToRlp '("13" "3b9aca00" "5208" "9938b093954c0F8Cb69B0a930318300f2d454ab2" "1ff973cafa8000" "" 3 "" "")))))
;; $57 = "9ebd877954d1c8405c62609bdf5ba75a9915317268932756741ec8fcf9f8d5f1"
;; (mtfa-bv-to-hex-string (mtfa-eth-sign-msg prk (mtfa-eth-get-pub-key prk) hmsg ))
;; $1 = "7f643465c1dbffd3f0b7f4c00c3de4a88b2f667f6a2f50ceb0267cc861ab3aceb3275960ee11319bf3b3f633c88c4f03e49d4f7542cb42e1f6a3b09e62b6b92c01"

;; ;;
;; ;;le funzioni che funzionano!!!
;; scheme@(guile-user)> (mtfa-bv-to-hex-string (mtfa-eth-serialize-signature (mtfa-eth-sign-msg prk (mtfa-eth-get-pub-key prk) hmsg )))
;; $2 = "01ce3aab61c87c26b0ce502f6a7f662f8ba8e43d0cc0f4b7f0d3ffdbc16534647f2cb9b6629eb0a3f6e142cb42754f9de4034f8cc833f6b3f39b3111ee605927b3"
;; scheme@(guile-user)> (mtfa-bv-to-hex-string (mtfa-eth-serialize-signature (mtfa-eth-sign-msg prk (mtfa-eth-get-pub-key prk) (mtfa-hex-string-to-bv "9faf293dae483e68a32a52bd3c91e7ff3933d4a3b61a71fa22af31e323608462" ))))
;; $3 = "0028a7d1b736e666f176ff68765f9f97f20f13de80bb286f237830b82ffb459d1945ba6102bcc92d455efffa272205f5238be2631ff37db93099a519d9acb120b5"

;; |#
