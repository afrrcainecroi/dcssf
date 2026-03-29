
std::string conf_file = R"V0G0N(##! ####Questo contiene sia VR che AR. Quindi vale sia per le VA che per i KAM
LbUser_VA_VRules = <<prefix>>.ars

####Questo vale solo per le VA#######
LbUser_VA_VReports = reports

####Variabili generali per tutti. OutIp principalmente utilizzata dai KAM#####
LbUser_OutIp	= 0.0.0.0
#192.168.163.70

##!
LbUser_AsDaemon=false

##!
LbUser_Id_WhoAmI=VA

##!
LbUser_Initialize=127.0.0.1:61100

##!
LbUser_SVA_Listen= #127.0.0.1:61101

##!
LbUser_FromVACommands= #127.0.0.1:61102  #ip:port

##!
LbUser_Hy_VA_Sisters= #ip, port, e certs delle va sorelle
LbUser_Hy_KAM_Children= #127.0.0.1:61103:certs/kam-01/kam-01.cert.pem

##!
LbUser_SecureVa= #127.0.0.1:61104

##!
LbUser_VAObservedApps= #iexplore;chrome;opera;safari;ab
LbUser_VA_GetAllHTTP= #true
LbUser_VA_QueryFilters= #none

##!
LbUser_VA_HwAgentPort= #61105
LbUser_VA_HwAgentIp= #127.0.0.1
LbUser_VA_SwAgentPort= #61106
LbUser_VA_SwAgentIp= #127.0.0.1
LbUser_VA_AskForUrl = #/va/askfor

LbUser_VA_LocalSH_on = false
#LbUser_VA_LocalSH_on = true

##!
LbUser_VA_TraceDomain=127.0.0.1:61107

##I comandi locali per la VA
LbUser_VA_KAM_CommandPort=61108
LbUser_VA_KAM_CommandIp=127.0.0.1

#Listen primario
LbUser_ListenIp=0.0.0.0
LbUser_ListenPort=9191

##I listen secondari
LbUser_VListenIpPort= #0.0.0.0:30000;0.0.0.0:40000

##!#LbUser_VTCPPort=127.0.0.1:9001:MANAGETCP_0;127.0.0.1:9002:MANAGETCP_1

##In ascolto per https
LbUser_VSSLPort= #0.0.0.0:443:certs/fullchain.pem:certs/privkey.pem|TLS|HIGH,MEDIUM,!aNULL,!MD5:ws1.com:

#####ALL#####
LbUser_HowManyInstances=2

LbUser_ThPoolSize=2

LbUser_LspCode=<<prefix>>.scm

##!
LbUser_LogFile=

##!
LbUser_NewCall=true

##!
LbUser_Rules=

##!
LbUser_BlockMessage=


)V0G0N";


std::string ars_file = R"V0G0N(DEFINE ipset serverip = { *.*.*.*:*, *.*.*.*:9191 };

//In questo modo definisco due url che chiamano le stesse api json
DEFINE urlset url_json = { /json, /json/, /json/*,
			   /microservices, /microservices/, /microservices/* }; // */

DEFINE urlset url_hook = { /hook, /hook/, /hook/* }; // */

DEFINE set nomi = { "franco" };

DEFINE AR "manage json"
	CONDITION
		//http.method is HTTP.POST //richiedo l'uso della post da parte del client
		http.url is in url_json
	ACTION
		MANAGE "JSONAPI"
	;

DEFINE AR "manage hook"
	CONDITION
		http.url is in url_hook
	ACTION
		MANAGE "HOOK", nomi   //posso passare anche un elenco di insiemi da gestire nel lisp
	;

DEFINE AR AREND
	CONDITION
		net.ipdst is in serverip
	ACTION
		ANSWER "END\r\n"
	;

//DEFINE AR AR1
//	CONDITION
//		EXISTS http.query["email"]
//	ACTION
//		ANSWER "Esiste in query\n"
//	;


//DEFINE AR AR2
//	CONDITION
//		EXISTS http.data["email"]
//	ACTION
//		ANSWER "Esiste in data\n"
//	;

//DEFINE AR AR3
//	CONDITION
//		!EXISTS http.query["email"]
//	ACTION
//		ANSWER "NON Esiste in data\n"
//	;


//DEFINE set utenti_bloccati = { "franco" };
//
//define set blocca = { "franco" };
//
//define urlset url_comando = { /comando, /comando/, /comando/* }; // */
//define set url_random = { };
//define dictionary url_keys = { };
//define set dummy = {};

//DEFINE VR random
//	CONDITION
//		net.ipdst is in serverip
//		http.url is in url_comando
//		!http.url is in url_random "/comando/" ""
//	VAR
//		url = http.url
//	ACTION
//		REPORT protette {http.url}
//	NEXT
//		net.ipdst is in serverip
//		http.url is url
//		!http.url is in url_random "/comando/" ""
//	ACTION
//		//è un modo per chiamare una funzione!
//		ADD CAT{"? (lambda (pbuf) (Show 1)(va::vaeng::AddToDict \"url_keys\" (mtfa-eis-get-current-uri pbuf) (mtfa-rand-alfanum 10 mtfa-char-set-alfa) \"60\" #f) \"dummy\")"} TO SET DUMMY
//		ADD http.url TO SET url_random 60 "/comando/" ""
//	;

//CAT{"? ((lambda () (mtfa-rand-alfanum 10 mtfa-char-set-alfa)))"} TO SET blocca 
//;

//DEFINE AR RANDOM_KEY
//	CONDITION
//		net.ipdst is in serverip
//		http.url is in url_random "/comando/" ""
//	ACTION
//		ANSWER "? (lambda (pbuf) (mtfa-eis-get-current-uri pbuf))"
//		//ANSWER "? ((lambda (pbuf) (let ((k (mtfa-rand-alfanum 10 mtfa-char-set-alfa))(v (mtfa-rand-alfanum 10 mtfa-char-set-alfa))) (va::vaeng::AddToDict \"url_keys\" k v 30 #f) k)) pbuf)"
//	;
(*
scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::AddToSet   va-c::vaeng::AddToSet)"));
scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::AddToDict  va-c::vaeng::AddToDict)"));
scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::DelFromSet va-c::vaeng::DelFromSet)"));
scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::GetFromSet va-c::vaeng::GetFromSet)"));
*)


///***************************TEST PATTERNS START
//Per vedere i set
//GET http://127.0.0.1:61108/a?CMD=set-list  | sed s/],/],\\n/g
//GET http://127.0.0.1:61108/a?CMD=set-get\&name=elenco_ipsrc_convalida_uri
//GET http://127.0.0.1:61108/a?CMD=set-get\&name=elenco_uri_ammesse| tr "," "\n"| grep 47| egrep -v "administr|maps"

(* /*
	Definire il modo di riconoscere l'amministratore (es: ip sorgente, oppure id
	dell'agente che sta sulla macchina, oppure un insieme di username dopo che hanno 
	effettuato un login corretto, ...)
	Definire il modo in cui le pagine sono riconosciute come pubbliche o private
	- ogni pagina (nuova) che viene richiesta, se è un "amministratore", la metto tra quelle pubbliche

	Come gestisco il fatto che diventa pubblica al secondo passaggio? e non al primo?
	esempio: due insiemi, insieme1 e insieme2 
	DEFINE VR
	CONDITION
		url not in insieme1
		url not in insieme2
		utente è amministratore
	ACTION
		add url to insieme1
	;

	DEFINE VR
	CONDITION
		url in insieme1
		url not in insieme2
		utente è amministratore
	ACTION
		add url to insieme2
		del url from insieme1 //non ha gran significato
	;

	DEFINE AR
	CONDITION
		url is in insieme2
	ACTION
		tcp.redirect alSito
	;

	CON LA OR:

	DEFINE VR
	CONDITION (
		url not in insieme1
		url not in insieme2
		utente è amministratore
	ACTION
		add url to insieme1
	) OR (
		url in insieme1
		url not in insieme2
		utente è amministratore
	ACTION
		add url to insieme2
		del url from insieme1 //non ha gran significato
	);

	Oppure!!
	DEFINE VR ...
	CONDITION
		not url is is urlok
		user is amministratore
	VAR
		laurl=http.url
	ACTION
		report ...
	NEXT
		url is laurl
		user is amministratore
	ACTION
		add url to urlok
	;
*/ *)

//DEFINE set elenco_uri_ammesse = 
//	MySql{192.168.163.13:3306, "db", "username", "password", "select uri from sitoinuit"};

//in una action di VR:
//mysql.append(192.168.163.13:3306, "db", "username", "password",
//"insert into sitoinuit (id, uri, url, host, query) values (sha2($0, 512), $0, $1, $2, $3)", 
//{http.uri, http.url, http.host, http.query} )
//  $0         $1       $2         $3


(*
define set check-patterns = { "Lalralta" };
DEFINE AR "AR check patterns"
	CONDITION
		http.method is HTTP.GET
		http.url is "/check"
		http.cookie["uno"] is in check-patterns "1234" "4321" //> 4321567890
	ACTION
		ANSWER "Cookie ok"
		ADD "ciccia" to set check-patterns "c.c" "ppp"
	;

DEFINE AR "AR check patterns accept"
	CONDITION
		http.url is "/check"
	ACTION
		ANSWER "NO cookie"
	;

DEFINE VR "VR check patterns"
	CONDITION
		http.method is HTTP.GET
		http.url is "/check"
		exists http.cookie["uno"]
	ACTION
		add http.cookie["uno"] to set check-patterns "1234" "4321"  // 's/1234/4321/'
	;
///***************************TEST PATTERNS END


DEFINE AR "goto json"
	CONDITION
		http.method is HTTP.POST
		http.url is "/ReqJson"
	ACTION
		MANAGE "JSONAPI"
	;

DEFINE AR "goto lisp1"
	CONDITION
		http.url is "/API1"
		http.query["aaa"] is "100"
		http.data["a"] is "12345"
		http.cookie["pluto"] is "nome"
		CAT { "franco" } is in blocca
		CALL "LispFun1" WITH http.host, http.cookie["uno"]
	ACTION
		MANAGE "API1"
	;

DEFINE AR "goto lisp"
	CONDITION
		http.method is HTTP.POST
		http.url is "/API"
	ACTION
		MANAGE "API"
	;

DEFINE AR "wordpress aperto"
	CONDITION
		net.ipdst is in serverip
	ACTION
		tcp.redirect "127.0.0.1:8888"
	;

DEFINE VR "verifica login"
	CONDITION
	(
		obs.event is net.send
		CAT { "franco" } is in blocca
	ACTION
		REPORT login {CAT{ "\n", 
			" 1) URI: ", http.uri, "\n",				//uri
			" 2) All data: ", http.data["0_FULL_DATA"], "\n",	//data
			" 3) Var a: ", http.data["a"], "\n",			//data[...]
			" 4) JSON: ", http.data["latitude"], "\n",		//data json
			" 5) Query: ", http.query["0_FULL_QUERY"], "\n",	//query
			" 6) Query beto: ", http.query["beto"], "\n",		//query json
			" 7) Query bbb: ", http.query["bbb"], "\n",		//query std
			" 8) Headers: ", http.header["0_FULL_DATA"], "\n",	//headers
			" 9) User-Agent: ", http.header["User-Agent"], "\n",	//header[...]
			"10) cookie pluto: ", http.cookie["pluto"]		//cookies
		}}
	)
	OR
	(
		obs.event is net.recv
	ACTION
		REPORT login {CAT{ "\n", 
			"11) All data: ", http.answer.data["0_FULL_DATA"], "\n",
			"12) Var code: ", http.answer.data["code"], "\n",
			"13) JSON: ", http.answer.data["answer"], "\n",
			"14) Cookies: ", http.answer.cookie["0_FULL_DATA"], "\n",
			"15) Cookie ciccia: ", http.answer.cookie["ciccia"], "\n",
			"16) Headers: ", http.answer.header["0_FULL_DATA"], "\n",
			"17) User-Agent: ", http.answer.header["User-Agent"], "\n",
			"18) Myheader: ", http.answer.header["MyHeader"], "\n",
			"19) Server: ", http.answer.header["Server"], "\n"
		}}
		del "franco" from set blocca 30
	   )

	;
*)

//curl --request GET -k "http://10.211.55.1:8888/API1?\{\"aaa\":100,\"bbb\":200\}" --cookie "pluto=nome" --data "data1=10&a=12345&data2=20"
//curl --request POST -k "http://10.211.55.1:8888/API1?\{\"aaa\":100,\"bbb\":200\}" --cookie "pluto=nome" --data "data1=10&a=12345&data2=20"
)V0G0N";



std::string scm_file = R"V0G0N((use-modules
 (mtfa error-handler)
 (mtfa utils)
 (mtfa serializer)
 (mtfa unordered-set) ;;unordered set con chiavi (stringhe, numeri o sumimboli: tutto convertito in stringa). Persistente
 (mtfa unordered-map) ;;unordered map con chiavi (stringhe) e valori (qualsiasi cosa). persistente
 (mtfa star-map)      ;;Inserisce stringhe con o senza jolly, la stringa che definisce il jolly e il valore. Cerca le stringhe che matchano!
 (mtfa simple_db)
 (mtfa certs)
 (mtfa eis)
 ;;(mtfa fsm)
 (mtfa va)
 (mtfa extset)  ;;gestisce insiemi i cui elementi sono stringhe! consente operazioni di clone, set, check, get all.... Definisce una macro che consente di creare "al volo" una sottoclasse le cui istanze condividono gli stessi elementi.
 (mtfa umset)   ;;è una unordered map (non persistente) che ha stringhe come chiavi e ha insiemi di stringhe come valori. Ogni insert aggiunge all'insieme corrispondente. Definisce inoltre la mtfa-umap-list che consente di mappare liste come chiavi e qualsiasi valore come valore
 (mtfa web)
 (mtfa brg)
 (mtfa lazy-seq)
 (mtfa domain-fiber-server)
 (mtfa nn)
 (mtfa x11draw)
 (mtfa avl)
 (mtfa eqt)
 (mtfa opencv)
 ;;
 (pfds sets)
 (simple-zmq)
 ((lens) #:prefix lens::)
 ;;
 (gnutls)
 ;;
 ;;La libreria guile lib
 (logging logger)
 (logging rotating-log)
 (logging port-log)
 (scheme kwargs)
 (search basic)
 (math primes)
 (match-bind)
 (graph topological-sort)
 ;;
 ;;i moduli di guile
 ;;((rnrs records syntactic) #:prefix rnrs::)
 (rnrs bytevectors)
 (rnrs arithmetic bitwise)
 ;; ((rnrs io ports)
 ;;  #:select (string->bytevector bytevector->string)
 ;;  #:prefix ioports::)
 ;;((rnrs) :version (6))
 ;;
 (srfi srfi-1)
 (srfi srfi-9)
 (srfi srfi-11)
 ((srfi srfi-18)
  #:prefix srfi-18::) ;;thread e mutex
 ;; date & time rinomina per avere un current time che non si sovrappone
 (srfi srfi-19)
 (srfi srfi-26)
 ;;(srfi srfi-28)
 (srfi srfi-41) ;;streams
 (srfi srfi-42) ;;Eager Comprehensions
 (srfi srfi-43)
 (srfi srfi-45)
 (srfi srfi-60)
 (srfi srfi-111)  ;;Boxes
 (srfi srfi-171)
 ;;
 (web uri)
 (system foreign)
 ;;
 (ice-9 format)
 (ice-9 ftw)
 (ice-9 rdelim)
 (ice-9 pretty-print)
 (ice-9 regex)
 (ice-9 iconv)
 (ice-9 string-fun)
 (ice-9 peg)
 (ice-9 peg string-peg)
 (ice-9 vlist)
 (ice-9 q)
 (ice-9 binary-ports)
 (ice-9 textual-ports)
 (ice-9 threads)
 (ice-9 hash-table)
 (ice-9 control)
 (ice-9 match)
 (ice-9 receive)
 (ice-9 eval-string)
 (ice-9 textual-ports)
 (ice-9 arrays)
 (ice-9 popen)
 (ice-9 exceptions)
 (ice-9 optargs)
 ;;
 (oop goops)
 (oop goops describe)
 ;; (sxml simple)
 ;; (sxml ssax)
 ;; (sxml xpath)
 (json)
 (system syntax)
 (system foreign)
 ;;
 ;;(fibers web server)
 (web server)
 (web request)
 (web response)
 (web uri)
 ;;
 (web client)
 ;;
 (ffi blis)
 
 )
;;Esempi di integrazione con il file ARS
;; (define (KamRun dummy1 dummy2 fun pbuf)
;;   ((eval-string fun) pbuf))
;; (define (LispFun1 host cookie)
;;   (Show host ", " cookie)
;;   #t)

;;Legge tutte le variabili dal un file di configurazione informato json
(eval-when (expand load compile eval)
  (mtfa-define-symbol-value-pairs-from-json "cfg" "<<prefix>>.json")) ;;il primo parametro è il prefisso delle variabili presenti nel file json
;;
;;All'avvio avvia il log
(mtfa-logging-run cfg::log-file-name)
;;
;;
;;Da mandare in uscita!!
;;(mtfa-logging-shutdown)

;;Init del generatore di numeri casuali
(mtfa-rand-seed (mtfa-micros))
;;

;;Per la gestione di richieste JSON
(define (json-error err-code err-string)
  (let ((ans `((completed . #f) (err-code . ,err-code) (err-string . ,err-string))))
    (log-msg 'CRITICAL ans)
    (eis::GiveHTTPJSONAnswer ans)))
(define (json-ok result . other)
  (if (null? other)
      (let ((ans `((completed . #t) (result . ,result))))
	(log-msg 'CRITICAL ans)
	(eis::GiveHTTPJSONAnswer ans))
      (let ((ans `((completed . #t) (result . ,result) ,(car other))))
	(log-msg 'CRITICAL ans)
	(eis::GiveHTTPJSONAnswer ans))))
;;
(define JsonRequestManager (mtfa-web-make-json-requests-manager "JSONAPI")) ;;add user data #:auxiliary-info `(,detector-box ,rt-manager ,trajectories ,box-abs-index ,ipcamera)
;;and in the ARS file
#|
DEFINE AR "how-to-add-an-hook"
CONDITION
  http.url is "/jsonpage"
ACTION
  MANAGE "JSONAPI"
;
|#

;; ;;e aggiungiamo gli handler
;; (JsonRequestManager 'add-handler "kill" API::Kill)
;; (JsonRequestManager 'add-handler "authenticate" API::Authenticate)
;; (JsonRequestManager 'add-handler "checkid" API::CheckId)
;; (JsonRequestManager 'add-handler "ptzcamera" API::PtzCamera)
;; (JsonRequestManager 'add-handler "startrecording" API::StartRecording)
;; (JsonRequestManager 'add-handler "stoprecording" API::StopRecording)
;;
;;HTTP microservices example
;;
;;Microservice definition
(define (API::Time k v pbuf set-names auxiliary-info) ;;aux-info = (,detector-box ,rt-manager ,trajectories ,box-abs-index ,ipcamera)
  (mtfa-noerr
   (json-error 999 "anomalous data received")
   ;; (let-values (((detector-box rt-manager trajectories box-abs-index ipcamera) (apply values auxiliary-info)))
     (log-msg 'CRITICAL "K: " k ". V: " v)
     (json-ok (mtfa-micros)))) ;;)
;;
;;Microservice registration: Esempio di chiamata curl -k -X POST http://127.0.0.1:9191/json -H "Content-Type: application/json"  -d '{"time":{"dummy": "value"}}'
(JsonRequestManager 'add-handler "time" API::Time) ;;The HTTP call GET or POST with JSON in payload or in the uri {"time": {json data}}
;;
;;Http hooks. Esempio di chiamata
(defun Manage::Hook (actionl pbuf)  ;;dove actionl è una lista che dal secondo posto in poi contiene i nomi dei set passati dalla MANAGE
  (eis::GiveErrorHTTP401) =>
;;
  #|Tutti gli elementi che possono essere utilizzati dalla sessione
            (mtfa-eis-get-value-current-query pbuf "variabile")
            (mtfa-eis-get-value-current-headers pbuf "Content-Type")
            mtfa-eis-get-current-headers
            mtfa-eis-get-value-current-cookies
            mtfa-eis-get-current-body
            mtfa-eis-get-current-method
            mtfa-eis-get-current-protocol
            mtfa-eis-get-current-host
            mtfa-eis-get-current-port
            mtfa-eis-get-current-pars
            mtfa-eis-get-current-tag
            mtfa-eis-get-current-url
            mtfa-eis-get-current-uri
            mtfa-eis-get-current-user-passwd
            mtfa-eis-get-current-ip-src
            mtfa-eis-get-current-ip-dst
            mtfa-eis-get-current-port-src
            mtfa-eis-get-current-port-dst

	    Per inviare risposte al client o all'ambiente della ARS
            eis::BaseLib::PassToServer => si collega con un altro server a attiva una comunicazione come se fosse un proxy (tcp.redirect)
            eis::BaseLib::NeedMoreData => i dati associati alla richiesta sono insufficienti (content-lengt di 100 e dati arrivati 50)
            eis::BaseLib::ContinueAfterAccept
            eis::BaseLib::BlockSession
            eis::GiveAnswerAndCloseAll risposta e chiudo la connessione
            eis::GiveAnswerAndNotClose
            eis::Redirect http location...203
            eis::GiveFileAndClose
            eis::GiveFileAndNotClose
            eis::http-content-type-html
            eis::http-content-type-text
            eis::http-content-type-b64
            eis::http-content-type-xml
            eis::http-content-type-javascript
            eis::http-answer-ok
            eis::http-content-length sinonimo di 200 ok
            eis::http-connection-close
            eis::http-connection-keep-alive
            eis::http-eoln
            eis::http-end-of-headers
            eis::BaseLib::BuildHTTPAnswer
            eis::GiveHTTPAnswer
            eis::GiveHTTPHtmlAnswer
            eis::GiveHTTPXmlAnswer
            eis::GiveHTTPTextAnswer
            eis::GiveHTTPB64Answer
            eis::GiveHTTPJSONAnswer
            eis::GiveErrorHTTP404
            eis::GiveErrorHTTP401
            eis::GiveErrorHTTP500
            eis::LoadPage
            eis::SplitPostData  var=valore&var=valore => hash k,v
|#

  (Show "In the hook ...")
  
  ;;se dovete controllare squando un particolare valore transitato è superiore a X, allora
  ;;potete fare una MANAGE e dentro l'hook prendere il valore e verificarlo e poi far transitare verso 
  ;;il server di destinazione
  ;;se sono "" o #nil passa quello ricevuto dal client
  ;;(eis::BaseLib::PassToServer ip_dest porta_dest host method headers url body)
  
  (log-msg 'CRITICAL "Data from uri: a=" (mtfa-eis-get-value-current-query pbuf "a") ", " (mtfa-eis-get-value-current-query pbuf "0_FULL_QUERY"))
  (eis::GiveHTTPAnswer "HTTP/1.1 200 OK" "MyHeader: none" "Set-Cookie: ciccia=ariciccia" "answer=ok&code=199")
  )

;;How to add an HOOK
;;in the ARS file:
#|
DEFINE AR "how-to-add-an-hook"
CONDITION
  http.url is "/page1"
ACTION
  MANAGE "HOOK"
;
|#
(eis::function-pointer-add "HOOK" Manage::Hook)
;;

)V0G0N";



std::string json_file = R"V0G0N({
    "log-file-name": "<<prefix>>.log",
    "uname": "franco-XPS-15-9570"
}
)V0G0N";

