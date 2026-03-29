#include <netinet/tcp.h>
#include "VA_utils.h"
using namespace std;



string MakeHttpDateTime ( time_t now )
{
    char buf[1000];
    //time_t now = time(0);
    struct tm tm = *gmtime ( &now );
    strftime ( buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm );
    //printf("Time is: [%s]\n", buf);
    return string ( buf );
}

//La parte di codice riservata al multipart
namespace multipart
{
    //#define DEBUG_MULTIPART 1
    static void multipart_log ( const char* format, ... )
    {
        #ifdef DEBUG_MULTIPART
        va_list args;
        va_start ( args, format );
        
        fprintf ( stderr, "[HTTP_MULTIPART_PARSER] %s:%d: ", __FILE__, __LINE__ );
        vfprintf ( stderr, format, args );
        fprintf ( stderr, "\n" );
        #endif
    }
    
    #define NOTIFY_CB(FOR)                                                 \
    do {                                                                   \
        if (p->settings->on_##FOR) {                                         \
            if (p->settings->on_##FOR(p) != 0) {                               \
                return i;                                                        \
            }                                                                  \
        }                                                                    \
    } while (0)
    
    #define EMIT_DATA_CB(FOR, ptr, len)                                    \
    do {                                                                   \
        if (p->settings->on_##FOR) {                                         \
            if (p->settings->on_##FOR(p, ptr, len) != 0) {                     \
                return i;                                                        \
            }                                                                  \
        }                                                                    \
    } while (0)
    
    
    #define LF 10
    #define CR 13
    
//     struct multipart_parser
//     {
//         void* data;
//         
//         size_t index;
//         size_t boundary_length;
//         
//         unsigned char state;
//         
//         multipart_parser_settings* settings;
//         
//         char* lookbehind;
//         char multipart_boundary[1];
//     };
    
    enum state
    {
        s_uninitialized = 1,
        s_start,
        s_start_boundary,
        s_header_field_start,
        s_header_field,
        s_headers_almost_done,
        s_header_value_start,
        s_header_value,
        s_header_value_almost_done,
        s_part_data_start,
        s_part_data,
        s_part_data_almost_boundary,
        s_part_data_boundary,
        s_part_data_almost_end,
        s_part_data_end,
        s_part_data_final_hyphen,
        s_end
    };
    
    multipart_parser* multipart_parser_init
    ( const char* boundary, multipart_parser_settings* settings )
    {
        
        multipart_parser* p = ( multipart_parser* ) malloc ( sizeof ( multipart_parser ) +
        strlen ( boundary ) +
        strlen ( boundary ) + 9 );
        
        strcpy ( p->multipart_boundary, boundary );
        p->boundary_length = strlen ( boundary );
        
        p->lookbehind = ( p->multipart_boundary + p->boundary_length + 1 );
        
        p->index = 0;
        p->state = s_start;
        p->settings = settings;
        
        return p;
    }
    
    void multipart_parser_free ( multipart_parser* p )
    {
        free ( p );
    }
    
    void multipart_parser_set_data ( multipart_parser* p, void* data )
    {
        p->data = data;
    }
    
    void* multipart_parser_get_data ( multipart_parser* p )
    {
        return p->data;
    }
    
    size_t multipart_parser_execute ( multipart_parser* p, const char* buf, size_t len )
    {
        size_t i = 0;
        size_t mark = 0;
        char c, cl;
        int is_last = 0;
        
        while ( i < len )
        {
            c = buf[i];
            is_last = ( i == ( len - 1 ) );
            
            switch ( p->state )
            {
                case s_start:
                    multipart_log ( "s_start" );
                    p->index = 0;
                    p->state = s_start_boundary;
                    
                    /* fallthrough */
                    case s_start_boundary:
                        multipart_log ( "s_start_boundary" );
                        
                        if ( p->index == p->boundary_length )
                        {
                            if ( c != CR )
                            {
                                return i;
                            }
                            
                            p->index++;
                            break;
                        }
                        else if ( p->index == ( p->boundary_length + 1 ) )
                        {
                            if ( c != LF )
                            {
                                return i;
                            }
                            
                            p->index = 0;
                            NOTIFY_CB ( part_data_begin );
                            p->state = s_header_field_start;
                            break;
                        }
                        
                        if ( c != p->multipart_boundary[p->index] )
                        {
                            return i;
                        }
                        
                        p->index++;
                        break;
                        
                    case s_header_field_start:
                        multipart_log ( "s_header_field_start" );
                        mark = i;
                        p->state = s_header_field;
                        
                        /* fallthrough */
                        case s_header_field:
                            multipart_log ( "s_header_field" );
                            
                            if ( c == CR )
                            {
                                p->state = s_headers_almost_done;
                                break;
                            }
                            
                            if ( c == ':' )
                            {
                                EMIT_DATA_CB ( header_field, buf + mark, i - mark );
                                p->state = s_header_value_start;
                                break;
                            }
                            
                            cl = tolower ( c );
                            
                            if ( ( c != '-' ) && ( cl < 'a' || cl > 'z' ) )
                            {
                                multipart_log ( "invalid character in header name" );
                                return i;
                            }
                            
                            if ( is_last )
                                EMIT_DATA_CB ( header_field, buf + mark, ( i - mark ) + 1 );
                            
                            break;
                            
                        case s_headers_almost_done:
                            multipart_log ( "s_headers_almost_done" );
                            
                            if ( c != LF )
                            {
                                return i;
                            }
                            
                            p->state = s_part_data_start;
                            break;
                            
                        case s_header_value_start:
                            multipart_log ( "s_header_value_start" );
                            
                            if ( c == ' ' )
                            {
                                break;
                            }
                            
                            mark = i;
                            p->state = s_header_value;
                            
                            /* fallthrough */
                            case s_header_value:
                                multipart_log ( "s_header_value" );
                                
                                if ( c == CR )
                                {
                                    EMIT_DATA_CB ( header_value, buf + mark, i - mark );
                                    p->state = s_header_value_almost_done;
                                    break;
                                }
                                
                                if ( is_last )
                                    EMIT_DATA_CB ( header_value, buf + mark, ( i - mark ) + 1 );
                                
                                break;
                                
                            case s_header_value_almost_done:
                                multipart_log ( "s_header_value_almost_done" );
                                
                                if ( c != LF )
                                {
                                    return i;
                                }
                                
                                p->state = s_header_field_start;
                                break;
                                
                            case s_part_data_start:
                                multipart_log ( "s_part_data_start" );
                                NOTIFY_CB ( headers_complete );
                                mark = i;
                                p->state = s_part_data;
                                
                                /* fallthrough */
                                case s_part_data:
                                    multipart_log ( "s_part_data" );
                                    
                                    if ( c == CR )
                                    {
                                        EMIT_DATA_CB ( part_data, buf + mark, i - mark );
                                        mark = i;
                                        p->state = s_part_data_almost_boundary;
                                        p->lookbehind[0] = CR;
                                        break;
                                    }
                                    
                                    if ( is_last )
                                        EMIT_DATA_CB ( part_data, buf + mark, ( i - mark ) + 1 );
                                    
                                    break;
                                    
                                case s_part_data_almost_boundary:
                                    multipart_log ( "s_part_data_almost_boundary" );
                                    
                                    if ( c == LF )
                                    {
                                        p->state = s_part_data_boundary;
                                        p->lookbehind[1] = LF;
                                        p->index = 0;
                                        break;
                                    }
                                    
                                    EMIT_DATA_CB ( part_data, p->lookbehind, 1 );
                                    p->state = s_part_data;
                                    mark = i --;
                                    break;
                                    
                                case s_part_data_boundary:
                                    multipart_log ( "s_part_data_boundary" );
                                    
                                    if ( p->multipart_boundary[p->index] != c )
                                    {
                                        EMIT_DATA_CB ( part_data, p->lookbehind, 2 + p->index );
                                        p->state = s_part_data;
                                        mark = i --;
                                        break;
                                    }
                                    
                                    p->lookbehind[2 + p->index] = c;
                                    
                                    if ( ( ++ p->index ) == p->boundary_length )
                                    {
                                        NOTIFY_CB ( part_data_end );
                                        p->state = s_part_data_almost_end;
                                    }
                                    
                                    break;
                                    
                                case s_part_data_almost_end:
                                    multipart_log ( "s_part_data_almost_end" );
                                    
                                    if ( c == '-' )
                                    {
                                        p->state = s_part_data_final_hyphen;
                                        break;
                                    }
                                    
                                    if ( c == CR )
                                    {
                                        p->state = s_part_data_end;
                                        break;
                                    }
                                    
                                    return i;
                                    
                                case s_part_data_final_hyphen:
                                    multipart_log ( "s_part_data_final_hyphen" );
                                    
                                    if ( c == '-' )
                                    {
                                        NOTIFY_CB ( body_end );
                                        p->state = s_end;
                                        break;
                                    }
                                    
                                    return i;
                                    
                                case s_part_data_end:
                                    multipart_log ( "s_part_data_end" );
                                    
                                    if ( c == LF )
                                    {
                                        p->state = s_header_field_start;
                                        NOTIFY_CB ( part_data_begin );
                                        break;
                                    }
                                    
                                    return i;
                                    
                                case s_end:
                                    multipart_log ( "s_end: %02X", ( int ) c );
                                    break;
                                    
                                default:
                                    multipart_log ( "Multipart parser unrecoverable error" );
                                    return 0;
            }
            ++ i;
        }

        return len;
    }

    //Le callback!
    //in alcuni casi se non ho appena creato un header, allora il primo header_value che prendo
    //è un nome
    //Facciamo in questo modo: se non ho ancora messo dati, appendo sempre al precedente
    int read_data ( multipart_parser* p, const char* at, size_t lenght )
    {
        //Questo aggiunge i dati
        string s;
        s.assign ( at, lenght );
        p->settings->lmr.back().data.append ( string ( at, lenght ) );
//         printf ( "Appende valore Data: %.*s\n", lenght, at );
        return 0;
    }

    int read_header_name ( multipart_parser* p, const char* at, size_t lenght )
    {
        if ( ( p->settings->lmr.size() == 0 ) || ( p->settings->lmr.back().data.size() > 0 ) )
        {
            //Questo definisce un nuovo elemento da aggiungere alla lista
            T_multipart_result mr;
            mr.name.assign ( at, lenght );
            p->settings->lmr.push_back ( mr );
//             printf ( "Crea HeaderName : %.*s\n", lenght, at );
            return 0;
        }
        else
        {
            p->settings->lmr.back().name.append ( "\n" );
            p->settings->lmr.back().name.append ( at, lenght );
//             printf ( "Appende HeaderName: %.*s\n", lenght, at );
            return 0;
        }
    }

    int read_header_value ( multipart_parser* p, const char* at, size_t lenght )
    {
        //Questo aggiunge il valore al nome header
        //Se un valore già esiste, appende prima fine linea
        if ( p->settings->lmr.back().value.length() == 0 )
        {
            p->settings->lmr.back().value.append ( at, lenght );
//             printf ( "Crea HeaderValue: %.*s\n", lenght, at );
            return 0;
        }
        else
        {
            p->settings->lmr.back().value.append ( "\n" );
            p->settings->lmr.back().value.append ( at, lenght );
//             printf ( "Appende HeaderValue: %.*s\n", lenght, at );
            return 0;
        }
    }
};

unordered_map<string, string> glb_mimetypes;

void InitMimetypes()
{
    glb_mimetypes[".323"] = "text/h323";
    glb_mimetypes[".3g2"] = "video/3gpp2";
    glb_mimetypes[".3gp"] = "video/3gpp";
    glb_mimetypes[".3gp2"] = "video/3gpp2";
    glb_mimetypes[".3gpp"] = "video/3gpp";
    glb_mimetypes[".7z"] = "application/x-7z-compressed";
    glb_mimetypes[".aa"] = "audio/audible";
    glb_mimetypes[".AAC"] = "audio/aac";
    glb_mimetypes[".aaf"] = "application/octet-stream";
    glb_mimetypes[".aax"] = "audio/vnd.audible.aax";
    glb_mimetypes[".ac3"] = "audio/ac3";
    glb_mimetypes[".aca"] = "application/octet-stream";
    glb_mimetypes[".accda"] = "application/msaccess.addin";
    glb_mimetypes[".accdb"] = "application/msaccess";
    glb_mimetypes[".accdc"] = "application/msaccess.cab";
    glb_mimetypes[".accde"] = "application/msaccess";
    glb_mimetypes[".accdr"] = "application/msaccess.runtime";
    glb_mimetypes[".accdt"] = "application/msaccess";
    glb_mimetypes[".accdw"] = "application/msaccess.webapplication";
    glb_mimetypes[".accft"] = "application/msaccess.ftemplate";
    glb_mimetypes[".acx"] = "application/internet-property-stream";
    glb_mimetypes[".AddIn"] = "text/xml";
    glb_mimetypes[".ade"] = "application/msaccess";
    glb_mimetypes[".adobebridge"] = "application/x-bridge-url";
    glb_mimetypes[".adp"] = "application/msaccess";
    glb_mimetypes[".ADT"] = "audio/vnd.dlna.adts";
    glb_mimetypes[".ADTS"] = "audio/aac";
    glb_mimetypes[".afm"] = "application/octet-stream";
    glb_mimetypes[".ai"] = "application/postscript";
    glb_mimetypes[".aif"] = "audio/aiff";
    glb_mimetypes[".aifc"] = "audio/aiff";
    glb_mimetypes[".aiff"] = "audio/aiff";
    glb_mimetypes[".air"] = "application/vnd.adobe.air-application-installer-package+zip";
    glb_mimetypes[".amc"] = "application/mpeg";
    glb_mimetypes[".anx"] = "application/annodex";
    glb_mimetypes[".application"] = "application/x-ms-application";
    glb_mimetypes[".art"] = "image/x-jg";
    glb_mimetypes[".asa"] = "application/xml";
    glb_mimetypes[".asax"] = "application/xml";
    glb_mimetypes[".ascx"] = "application/xml";
    glb_mimetypes[".asd"] = "application/octet-stream";
    glb_mimetypes[".asf"] = "video/x-ms-asf";
    glb_mimetypes[".ashx"] = "application/xml";
    glb_mimetypes[".asi"] = "application/octet-stream";
    glb_mimetypes[".asm"] = "text/plain";
    glb_mimetypes[".asmx"] = "application/xml";
    glb_mimetypes[".aspx"] = "application/xml";
    glb_mimetypes[".asr"] = "video/x-ms-asf";
    glb_mimetypes[".asx"] = "video/x-ms-asf";
    glb_mimetypes[".atom"] = "application/atom+xml";
    glb_mimetypes[".au"] = "audio/basic";
    glb_mimetypes[".avi"] = "video/x-msvideo";
    glb_mimetypes[".axa"] = "audio/annodex";
    glb_mimetypes[".axs"] = "application/olescript";
    glb_mimetypes[".axv"] = "video/annodex";
    glb_mimetypes[".bas"] = "text/plain";
    glb_mimetypes[".bcpio"] = "application/x-bcpio";
    glb_mimetypes[".bin"] = "application/octet-stream";
    glb_mimetypes[".bmp"] = "image/bmp";
    glb_mimetypes[".c"] = "text/plain";
    glb_mimetypes[".cab"] = "application/octet-stream";
    glb_mimetypes[".caf"] = "audio/x-caf";
    glb_mimetypes[".calx"] = "application/vnd.ms-office.calx";
    glb_mimetypes[".cat"] = "application/vnd.ms-pki.seccat";
    glb_mimetypes[".cc"] = "text/plain";
    glb_mimetypes[".cd"] = "text/plain";
    glb_mimetypes[".cdda"] = "audio/aiff";
    glb_mimetypes[".cdf"] = "application/x-cdf";
    glb_mimetypes[".cer"] = "application/x-x509-ca-cert";
    glb_mimetypes[".cfg"] = "text/plain";
    glb_mimetypes[".chm"] = "application/octet-stream";
    glb_mimetypes[".class"] = "application/x-java-applet";
    glb_mimetypes[".clp"] = "application/x-msclip";
    glb_mimetypes[".cmd"] = "text/plain";
    glb_mimetypes[".cmx"] = "image/x-cmx";
    glb_mimetypes[".cnf"] = "text/plain";
    glb_mimetypes[".cod"] = "image/cis-cod";
    glb_mimetypes[".config"] = "application/xml";
    glb_mimetypes[".contact"] = "text/x-ms-contact";
    glb_mimetypes[".coverage"] = "application/xml";
    glb_mimetypes[".cpio"] = "application/x-cpio";
    glb_mimetypes[".cpp"] = "text/plain";
    glb_mimetypes[".crd"] = "application/x-mscardfile";
    glb_mimetypes[".crl"] = "application/pkix-crl";
    glb_mimetypes[".crt"] = "application/x-x509-ca-cert";
    glb_mimetypes[".cs"] = "text/plain";
    glb_mimetypes[".csdproj"] = "text/plain";
    glb_mimetypes[".csh"] = "application/x-csh";
    glb_mimetypes[".csproj"] = "text/plain";
    glb_mimetypes[".css"] = "text/css";
    glb_mimetypes[".csv"] = "text/csv";
    glb_mimetypes[".cur"] = "application/octet-stream";
    glb_mimetypes[".cxx"] = "text/plain";
    glb_mimetypes[".dat"] = "application/octet-stream";
    glb_mimetypes[".datasource"] = "application/xml";
    glb_mimetypes[".dbproj"] = "text/plain";
    glb_mimetypes[".dcr"] = "application/x-director";
    glb_mimetypes[".def"] = "text/plain";
    glb_mimetypes[".deploy"] = "application/octet-stream";
    glb_mimetypes[".der"] = "application/x-x509-ca-cert";
    glb_mimetypes[".dgml"] = "application/xml";
    glb_mimetypes[".dib"] = "image/bmp";
    glb_mimetypes[".dif"] = "video/x-dv";
    glb_mimetypes[".dir"] = "application/x-director";
    glb_mimetypes[".disco"] = "text/xml";
    glb_mimetypes[".divx"] = "video/divx";
    glb_mimetypes[".dll"] = "application/x-msdownload";
    glb_mimetypes[".dll.config"] = "text/xml";
    glb_mimetypes[".dlm"] = "text/dlm";
    glb_mimetypes[".doc"] = "application/msword";
    glb_mimetypes[".docm"] = "application/vnd.ms-word.document.macroEnabled.12";
    glb_mimetypes[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    glb_mimetypes[".dot"] = "application/msword";
    glb_mimetypes[".dotm"] = "application/vnd.ms-word.template.macroEnabled.12";
    glb_mimetypes[".dotx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.template";
    glb_mimetypes[".dsp"] = "application/octet-stream";
    glb_mimetypes[".dsw"] = "text/plain";
    glb_mimetypes[".dtd"] = "text/xml";
    glb_mimetypes[".dtsConfig"] = "text/xml";
    glb_mimetypes[".dv"] = "video/x-dv";
    glb_mimetypes[".dvi"] = "application/x-dvi";
    glb_mimetypes[".dwf"] = "drawing/x-dwf";
    glb_mimetypes[".dwp"] = "application/octet-stream";
    glb_mimetypes[".dxr"] = "application/x-director";
    glb_mimetypes[".eml"] = "message/rfc822";
    glb_mimetypes[".emz"] = "application/octet-stream";
    glb_mimetypes[".eot"] = "application/vnd.ms-fontobject";
    glb_mimetypes[".eps"] = "application/postscript";
    glb_mimetypes[".etl"] = "application/etl";
    glb_mimetypes[".etx"] = "text/x-setext";
    glb_mimetypes[".evy"] = "application/envoy";
    glb_mimetypes[".exe"] = "application/octet-stream";
    glb_mimetypes[".exe.config"] = "text/xml";
    glb_mimetypes[".fdf"] = "application/vnd.fdf";
    glb_mimetypes[".fif"] = "application/fractals";
    glb_mimetypes[".filters"] = "application/xml";
    glb_mimetypes[".fla"] = "application/octet-stream";
    glb_mimetypes[".flac"] = "audio/flac";
    glb_mimetypes[".flr"] = "x-world/x-vrml";
    glb_mimetypes[".flv"] = "video/x-flv";
    glb_mimetypes[".fsscript"] = "application/fsharp-script";
    glb_mimetypes[".fsx"] = "application/fsharp-script";
    glb_mimetypes[".generictest"] = "application/xml";
    glb_mimetypes[".gif"] = "image/gif";
    glb_mimetypes[".group"] = "text/x-ms-group";
    glb_mimetypes[".gsm"] = "audio/x-gsm";
    glb_mimetypes[".gtar"] = "application/x-gtar";
    glb_mimetypes[".gz"] = "application/x-gzip";
    glb_mimetypes[".h"] = "text/plain";
    glb_mimetypes[".hdf"] = "application/x-hdf";
    glb_mimetypes[".hdml"] = "text/x-hdml";
    glb_mimetypes[".hhc"] = "application/x-oleobject";
    glb_mimetypes[".hhk"] = "application/octet-stream";
    glb_mimetypes[".hhp"] = "application/octet-stream";
    glb_mimetypes[".hlp"] = "application/winhlp";
    glb_mimetypes[".hpp"] = "text/plain";
    glb_mimetypes[".hqx"] = "application/mac-binhex40";
    glb_mimetypes[".hta"] = "application/hta";
    glb_mimetypes[".htc"] = "text/x-component";
    glb_mimetypes[".htm"] = "text/html";
    glb_mimetypes[".html"] = "text/html";
    glb_mimetypes[".htt"] = "text/webviewhtml";
    glb_mimetypes[".hxa"] = "application/xml";
    glb_mimetypes[".hxc"] = "application/xml";
    glb_mimetypes[".hxd"] = "application/octet-stream";
    glb_mimetypes[".hxe"] = "application/xml";
    glb_mimetypes[".hxf"] = "application/xml";
    glb_mimetypes[".hxh"] = "application/octet-stream";
    glb_mimetypes[".hxi"] = "application/octet-stream";
    glb_mimetypes[".hxk"] = "application/xml";
    glb_mimetypes[".hxq"] = "application/octet-stream";
    glb_mimetypes[".hxr"] = "application/octet-stream";
    glb_mimetypes[".hxs"] = "application/octet-stream";
    glb_mimetypes[".hxt"] = "text/html";
    glb_mimetypes[".hxv"] = "application/xml";
    glb_mimetypes[".hxw"] = "application/octet-stream";
    glb_mimetypes[".hxx"] = "text/plain";
    glb_mimetypes[".i"] = "text/plain";
    glb_mimetypes[".ico"] = "image/x-icon";
    glb_mimetypes[".ics"] = "application/octet-stream";
    glb_mimetypes[".idl"] = "text/plain";
    glb_mimetypes[".ief"] = "image/ief";
    glb_mimetypes[".iii"] = "application/x-iphone";
    glb_mimetypes[".inc"] = "text/plain";
    glb_mimetypes[".inf"] = "application/octet-stream";
    glb_mimetypes[".ini"] = "text/plain";
    glb_mimetypes[".inl"] = "text/plain";
    glb_mimetypes[".ins"] = "application/x-internet-signup";
    glb_mimetypes[".ipa"] = "application/x-itunes-ipa";
    glb_mimetypes[".ipg"] = "application/x-itunes-ipg";
    glb_mimetypes[".ipproj"] = "text/plain";
    glb_mimetypes[".ipsw"] = "application/x-itunes-ipsw";
    glb_mimetypes[".iqy"] = "text/x-ms-iqy";
    glb_mimetypes[".isp"] = "application/x-internet-signup";
    glb_mimetypes[".ite"] = "application/x-itunes-ite";
    glb_mimetypes[".itlp"] = "application/x-itunes-itlp";
    glb_mimetypes[".itms"] = "application/x-itunes-itms";
    glb_mimetypes[".itpc"] = "application/x-itunes-itpc";
    glb_mimetypes[".IVF"] = "video/x-ivf";
    glb_mimetypes[".jar"] = "application/java-archive";
    glb_mimetypes[".java"] = "application/octet-stream";
    glb_mimetypes[".jck"] = "application/liquidmotion";
    glb_mimetypes[".jcz"] = "application/liquidmotion";
    glb_mimetypes[".jfif"] = "image/pjpeg";
    glb_mimetypes[".jnlp"] = "application/x-java-jnlp-file";
    glb_mimetypes[".jpb"] = "application/octet-stream";
    glb_mimetypes[".jpe"] = "image/jpeg";
    glb_mimetypes[".jpeg"] = "image/jpeg";
    glb_mimetypes[".jpg"] = "image/jpeg";
    glb_mimetypes[".js"] = "application/javascript";
    glb_mimetypes[".json"] = "application/json";
    glb_mimetypes[".jsx"] = "text/jscript";
    glb_mimetypes[".jsxbin"] = "text/plain";
    glb_mimetypes[".latex"] = "application/x-latex";
    glb_mimetypes[".library-ms"] = "application/windows-library+xml";
    glb_mimetypes[".lit"] = "application/x-ms-reader";
    glb_mimetypes[".loadtest"] = "application/xml";
    glb_mimetypes[".lpk"] = "application/octet-stream";
    glb_mimetypes[".lsf"] = "video/x-la-asf";
    glb_mimetypes[".lst"] = "text/plain";
    glb_mimetypes[".lsx"] = "video/x-la-asf";
    glb_mimetypes[".lzh"] = "application/octet-stream";
    glb_mimetypes[".m13"] = "application/x-msmediaview";
    glb_mimetypes[".m14"] = "application/x-msmediaview";
    glb_mimetypes[".m1v"] = "video/mpeg";
    glb_mimetypes[".m2t"] = "video/vnd.dlna.mpeg-tts";
    glb_mimetypes[".m2ts"] = "video/vnd.dlna.mpeg-tts";
    glb_mimetypes[".m2v"] = "video/mpeg";
    glb_mimetypes[".m3u"] = "audio/x-mpegurl";
    glb_mimetypes[".m3u8"] = "audio/x-mpegurl";
    glb_mimetypes[".m4a"] = "audio/m4a";
    glb_mimetypes[".m4b"] = "audio/m4b";
    glb_mimetypes[".m4p"] = "audio/m4p";
    glb_mimetypes[".m4r"] = "audio/x-m4r";
    glb_mimetypes[".m4v"] = "video/x-m4v";
    glb_mimetypes[".mac"] = "image/x-macpaint";
    glb_mimetypes[".mak"] = "text/plain";
    glb_mimetypes[".man"] = "application/x-troff-man";
    glb_mimetypes[".manifest"] = "application/x-ms-manifest";
    glb_mimetypes[".map"] = "text/plain";
    glb_mimetypes[".master"] = "application/xml";
    glb_mimetypes[".mda"] = "application/msaccess";
    glb_mimetypes[".mdb"] = "application/x-msaccess";
    glb_mimetypes[".mde"] = "application/msaccess";
    glb_mimetypes[".mdp"] = "application/octet-stream";
    glb_mimetypes[".me"] = "application/x-troff-me";
    glb_mimetypes[".mfp"] = "application/x-shockwave-flash";
    glb_mimetypes[".mht"] = "message/rfc822";
    glb_mimetypes[".mhtml"] = "message/rfc822";
    glb_mimetypes[".mid"] = "audio/mid";
    glb_mimetypes[".midi"] = "audio/mid";
    glb_mimetypes[".mix"] = "application/octet-stream";
    glb_mimetypes[".mk"] = "text/plain";
    glb_mimetypes[".mmf"] = "application/x-smaf";
    glb_mimetypes[".mno"] = "text/xml";
    glb_mimetypes[".mny"] = "application/x-msmoney";
    glb_mimetypes[".mod"] = "video/mpeg";
    glb_mimetypes[".mov"] = "video/quicktime";
    glb_mimetypes[".movie"] = "video/x-sgi-movie";
    glb_mimetypes[".mp2"] = "video/mpeg";
    glb_mimetypes[".mp2v"] = "video/mpeg";
    glb_mimetypes[".mp3"] = "audio/mpeg";
    glb_mimetypes[".mp4"] = "video/mp4";
    glb_mimetypes[".mp4v"] = "video/mp4";
    glb_mimetypes[".mpa"] = "video/mpeg";
    glb_mimetypes[".mpe"] = "video/mpeg";
    glb_mimetypes[".mpeg"] = "video/mpeg";
    glb_mimetypes[".mpf"] = "application/vnd.ms-mediapackage";
    glb_mimetypes[".mpg"] = "video/mpeg";
    glb_mimetypes[".mpp"] = "application/vnd.ms-project";
    glb_mimetypes[".mpv2"] = "video/mpeg";
    glb_mimetypes[".mqv"] = "video/quicktime";
    glb_mimetypes[".ms"] = "application/x-troff-ms";
    glb_mimetypes[".msi"] = "application/octet-stream";
    glb_mimetypes[".mso"] = "application/octet-stream";
    glb_mimetypes[".mts"] = "video/vnd.dlna.mpeg-tts";
    glb_mimetypes[".mtx"] = "application/xml";
    glb_mimetypes[".mvb"] = "application/x-msmediaview";
    glb_mimetypes[".mvc"] = "application/x-miva-compiled";
    glb_mimetypes[".mxp"] = "application/x-mmxp";
    glb_mimetypes[".nc"] = "application/x-netcdf";
    glb_mimetypes[".nsc"] = "video/x-ms-asf";
    glb_mimetypes[".nws"] = "message/rfc822";
    glb_mimetypes[".ocx"] = "application/octet-stream";
    glb_mimetypes[".oda"] = "application/oda";
    glb_mimetypes[".odb"] = "application/vnd.oasis.opendocument.database";
    glb_mimetypes[".odc"] = "application/vnd.oasis.opendocument.chart";
    glb_mimetypes[".odf"] = "application/vnd.oasis.opendocument.formula";
    glb_mimetypes[".odg"] = "application/vnd.oasis.opendocument.graphics";
    glb_mimetypes[".odh"] = "text/plain";
    glb_mimetypes[".odi"] = "application/vnd.oasis.opendocument.image";
    glb_mimetypes[".odl"] = "text/plain";
    glb_mimetypes[".odm"] = "application/vnd.oasis.opendocument.text-master";
    glb_mimetypes[".odp"] = "application/vnd.oasis.opendocument.presentation";
    glb_mimetypes[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    glb_mimetypes[".odt"] = "application/vnd.oasis.opendocument.text";
    glb_mimetypes[".oga"] = "audio/ogg";
    glb_mimetypes[".ogg"] = "audio/ogg";
    glb_mimetypes[".ogv"] = "video/ogg";
    glb_mimetypes[".ogx"] = "application/ogg";
    glb_mimetypes[".one"] = "application/onenote";
    glb_mimetypes[".onea"] = "application/onenote";
    glb_mimetypes[".onepkg"] = "application/onenote";
    glb_mimetypes[".onetmp"] = "application/onenote";
    glb_mimetypes[".onetoc"] = "application/onenote";
    glb_mimetypes[".onetoc2"] = "application/onenote";
    glb_mimetypes[".opus"] = "audio/ogg";
    glb_mimetypes[".orderedtest"] = "application/xml";
    glb_mimetypes[".osdx"] = "application/opensearchdescription+xml";
    glb_mimetypes[".otf"] = "application/font-sfnt";
    glb_mimetypes[".otg"] = "application/vnd.oasis.opendocument.graphics-template";
    glb_mimetypes[".oth"] = "application/vnd.oasis.opendocument.text-web";
    glb_mimetypes[".otp"] = "application/vnd.oasis.opendocument.presentation-template";
    glb_mimetypes[".ots"] = "application/vnd.oasis.opendocument.spreadsheet-template";
    glb_mimetypes[".ott"] = "application/vnd.oasis.opendocument.text-template";
    glb_mimetypes[".oxt"] = "application/vnd.openofficeorg.extension";
    glb_mimetypes[".p10"] = "application/pkcs10";
    glb_mimetypes[".p12"] = "application/x-pkcs12";
    glb_mimetypes[".p7b"] = "application/x-pkcs7-certificates";
    glb_mimetypes[".p7c"] = "application/pkcs7-mime";
    glb_mimetypes[".p7m"] = "application/pkcs7-mime";
    glb_mimetypes[".p7r"] = "application/x-pkcs7-certreqresp";
    glb_mimetypes[".p7s"] = "application/pkcs7-signature";
    glb_mimetypes[".pbm"] = "image/x-portable-bitmap";
    glb_mimetypes[".pcast"] = "application/x-podcast";
    glb_mimetypes[".pct"] = "image/pict";
    glb_mimetypes[".pcx"] = "application/octet-stream";
    glb_mimetypes[".pcz"] = "application/octet-stream";
    glb_mimetypes[".pdf"] = "application/pdf";
    glb_mimetypes[".pfb"] = "application/octet-stream";
    glb_mimetypes[".pfm"] = "application/octet-stream";
    glb_mimetypes[".pfx"] = "application/x-pkcs12";
    glb_mimetypes[".pgm"] = "image/x-portable-graymap";
    glb_mimetypes[".pic"] = "image/pict";
    glb_mimetypes[".pict"] = "image/pict";
    glb_mimetypes[".pkgdef"] = "text/plain";
    glb_mimetypes[".pkgundef"] = "text/plain";
    glb_mimetypes[".pko"] = "application/vnd.ms-pki.pko";
    glb_mimetypes[".pls"] = "audio/scpls";
    glb_mimetypes[".pma"] = "application/x-perfmon";
    glb_mimetypes[".pmc"] = "application/x-perfmon";
    glb_mimetypes[".pml"] = "application/x-perfmon";
    glb_mimetypes[".pmr"] = "application/x-perfmon";
    glb_mimetypes[".pmw"] = "application/x-perfmon";
    glb_mimetypes[".png"] = "image/png";
    glb_mimetypes[".pnm"] = "image/x-portable-anymap";
    glb_mimetypes[".pnt"] = "image/x-macpaint";
    glb_mimetypes[".pntg"] = "image/x-macpaint";
    glb_mimetypes[".pnz"] = "image/png";
    glb_mimetypes[".pot"] = "application/vnd.ms-powerpoint";
    glb_mimetypes[".potm"] = "application/vnd.ms-powerpoint.template.macroEnabled.12";
    glb_mimetypes[".potx"] = "application/vnd.openxmlformats-officedocument.presentationml.template";
    glb_mimetypes[".ppa"] = "application/vnd.ms-powerpoint";
    glb_mimetypes[".ppam"] = "application/vnd.ms-powerpoint.addin.macroEnabled.12";
    glb_mimetypes[".ppm"] = "image/x-portable-pixmap";
    glb_mimetypes[".pps"] = "application/vnd.ms-powerpoint";
    glb_mimetypes[".ppsm"] = "application/vnd.ms-powerpoint.slideshow.macroEnabled.12";
    glb_mimetypes[".ppsx"] = "application/vnd.openxmlformats-officedocument.presentationml.slideshow";
    glb_mimetypes[".ppt"] = "application/vnd.ms-powerpoint";
    glb_mimetypes[".pptm"] = "application/vnd.ms-powerpoint.presentation.macroEnabled.12";
    glb_mimetypes[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    glb_mimetypes[".prf"] = "application/pics-rules";
    glb_mimetypes[".prm"] = "application/octet-stream";
    glb_mimetypes[".prx"] = "application/octet-stream";
    glb_mimetypes[".ps"] = "application/postscript";
    glb_mimetypes[".psc1"] = "application/PowerShell";
    glb_mimetypes[".psd"] = "application/octet-stream";
    glb_mimetypes[".psess"] = "application/xml";
    glb_mimetypes[".psm"] = "application/octet-stream";
    glb_mimetypes[".psp"] = "application/octet-stream";
    glb_mimetypes[".pub"] = "application/x-mspublisher";
    glb_mimetypes[".pwz"] = "application/vnd.ms-powerpoint";
    glb_mimetypes[".qht"] = "text/x-html-insertion";
    glb_mimetypes[".qhtm"] = "text/x-html-insertion";
    glb_mimetypes[".qt"] = "video/quicktime";
    glb_mimetypes[".qti"] = "image/x-quicktime";
    glb_mimetypes[".qtif"] = "image/x-quicktime";
    glb_mimetypes[".qtl"] = "application/x-quicktimeplayer";
    glb_mimetypes[".qxd"] = "application/octet-stream";
    glb_mimetypes[".ra"] = "audio/x-pn-realaudio";
    glb_mimetypes[".ram"] = "audio/x-pn-realaudio";
    glb_mimetypes[".rar"] = "application/x-rar-compressed";
    glb_mimetypes[".ras"] = "image/x-cmu-raster";
    glb_mimetypes[".rat"] = "application/rat-file";
    glb_mimetypes[".rc"] = "text/plain";
    glb_mimetypes[".rc2"] = "text/plain";
    glb_mimetypes[".rct"] = "text/plain";
    glb_mimetypes[".rdlc"] = "application/xml";
    glb_mimetypes[".reg"] = "text/plain";
    glb_mimetypes[".resx"] = "application/xml";
    glb_mimetypes[".rf"] = "image/vnd.rn-realflash";
    glb_mimetypes[".rgb"] = "image/x-rgb";
    glb_mimetypes[".rgs"] = "text/plain";
    glb_mimetypes[".rm"] = "application/vnd.rn-realmedia";
    glb_mimetypes[".rmi"] = "audio/mid";
    glb_mimetypes[".rmp"] = "application/vnd.rn-rn_music_package";
    glb_mimetypes[".roff"] = "application/x-troff";
    glb_mimetypes[".rpm"] = "audio/x-pn-realaudio-plugin";
    glb_mimetypes[".rqy"] = "text/x-ms-rqy";
    glb_mimetypes[".rtf"] = "application/rtf";
    glb_mimetypes[".rtx"] = "text/richtext";
    glb_mimetypes[".ruleset"] = "application/xml";
    glb_mimetypes[".s"] = "text/plain";
    glb_mimetypes[".safariextz"] = "application/x-safari-safariextz";
    glb_mimetypes[".scd"] = "application/x-msschedule";
    glb_mimetypes[".scr"] = "text/plain";
    glb_mimetypes[".sct"] = "text/scriptlet";
    glb_mimetypes[".sd2"] = "audio/x-sd2";
    glb_mimetypes[".sdp"] = "application/sdp";
    glb_mimetypes[".sea"] = "application/octet-stream";
    glb_mimetypes[".searchConnector-ms"] = "application/windows-search-connector+xml";
    glb_mimetypes[".setpay"] = "application/set-payment-initiation";
    glb_mimetypes[".setreg"] = "application/set-registration-initiation";
    glb_mimetypes[".settings"] = "application/xml";
    glb_mimetypes[".sgimb"] = "application/x-sgimb";
    glb_mimetypes[".sgml"] = "text/sgml";
    glb_mimetypes[".sh"] = "application/x-sh";
    glb_mimetypes[".shar"] = "application/x-shar";
    glb_mimetypes[".shtml"] = "text/html";
    glb_mimetypes[".sit"] = "application/x-stuffit";
    glb_mimetypes[".sitemap"] = "application/xml";
    glb_mimetypes[".skin"] = "application/xml";
    glb_mimetypes[".sldm"] = "application/vnd.ms-powerpoint.slide.macroEnabled.12";
    glb_mimetypes[".sldx"] = "application/vnd.openxmlformats-officedocument.presentationml.slide";
    glb_mimetypes[".slk"] = "application/vnd.ms-excel";
    glb_mimetypes[".sln"] = "text/plain";
    glb_mimetypes[".slupkg-ms"] = "application/x-ms-license";
    glb_mimetypes[".smd"] = "audio/x-smd";
    glb_mimetypes[".smi"] = "application/octet-stream";
    glb_mimetypes[".smx"] = "audio/x-smd";
    glb_mimetypes[".smz"] = "audio/x-smd";
    glb_mimetypes[".snd"] = "audio/basic";
    glb_mimetypes[".snippet"] = "application/xml";
    glb_mimetypes[".snp"] = "application/octet-stream";
    glb_mimetypes[".sol"] = "text/plain";
    glb_mimetypes[".sor"] = "text/plain";
    glb_mimetypes[".spc"] = "application/x-pkcs7-certificates";
    glb_mimetypes[".spl"] = "application/futuresplash";
    glb_mimetypes[".spx"] = "audio/ogg";
    glb_mimetypes[".src"] = "application/x-wais-source";
    glb_mimetypes[".srf"] = "text/plain";
    glb_mimetypes[".SSISDeploymentManifest"] = "text/xml";
    glb_mimetypes[".ssm"] = "application/streamingmedia";
    glb_mimetypes[".sst"] = "application/vnd.ms-pki.certstore";
    glb_mimetypes[".stl"] = "application/vnd.ms-pki.stl";
    glb_mimetypes[".sv4cpio"] = "application/x-sv4cpio";
    glb_mimetypes[".sv4crc"] = "application/x-sv4crc";
    glb_mimetypes[".svc"] = "application/xml";
    glb_mimetypes[".svg"] = "image/svg+xml";
    glb_mimetypes[".swf"] = "application/x-shockwave-flash";
    glb_mimetypes[".t"] = "application/x-troff";
    glb_mimetypes[".tar"] = "application/x-tar";
    glb_mimetypes[".tcl"] = "application/x-tcl";
    glb_mimetypes[".testrunconfig"] = "application/xml";
    glb_mimetypes[".testsettings"] = "application/xml";
    glb_mimetypes[".tex"] = "application/x-tex";
    glb_mimetypes[".texi"] = "application/x-texinfo";
    glb_mimetypes[".texinfo"] = "application/x-texinfo";
    glb_mimetypes[".tgz"] = "application/x-compressed";
    glb_mimetypes[".thmx"] = "application/vnd.ms-officetheme";
    glb_mimetypes[".thn"] = "application/octet-stream";
    glb_mimetypes[".tif"] = "image/tiff";
    glb_mimetypes[".tiff"] = "image/tiff";
    glb_mimetypes[".tlh"] = "text/plain";
    glb_mimetypes[".tli"] = "text/plain";
    glb_mimetypes[".toc"] = "application/octet-stream";
    glb_mimetypes[".tr"] = "application/x-troff";
    glb_mimetypes[".trm"] = "application/x-msterminal";
    glb_mimetypes[".trx"] = "application/xml";
    glb_mimetypes[".ts"] = "video/vnd.dlna.mpeg-tts";
    glb_mimetypes[".tsv"] = "text/tab-separated-values";
    glb_mimetypes[".ttf"] = "application/font-sfnt";
    glb_mimetypes[".tts"] = "video/vnd.dlna.mpeg-tts";
    glb_mimetypes[".txt"] = "text/plain";
    glb_mimetypes[".u32"] = "application/octet-stream";
    glb_mimetypes[".uls"] = "text/iuls";
    glb_mimetypes[".user"] = "text/plain";
    glb_mimetypes[".ustar"] = "application/x-ustar";
    glb_mimetypes[".vb"] = "text/plain";
    glb_mimetypes[".vbdproj"] = "text/plain";
    glb_mimetypes[".vbk"] = "video/mpeg";
    glb_mimetypes[".vbproj"] = "text/plain";
    glb_mimetypes[".vbs"] = "text/vbscript";
    glb_mimetypes[".vcf"] = "text/x-vcard";
    glb_mimetypes[".vcproj"] = "application/xml";
    glb_mimetypes[".vcs"] = "text/plain";
    glb_mimetypes[".vcxproj"] = "application/xml";
    glb_mimetypes[".vddproj"] = "text/plain";
    glb_mimetypes[".vdp"] = "text/plain";
    glb_mimetypes[".vdproj"] = "text/plain";
    glb_mimetypes[".vdx"] = "application/vnd.ms-visio.viewer";
    glb_mimetypes[".vml"] = "text/xml";
    glb_mimetypes[".vscontent"] = "application/xml";
    glb_mimetypes[".vsct"] = "text/xml";
    glb_mimetypes[".vsd"] = "application/vnd.visio";
    glb_mimetypes[".vsi"] = "application/ms-vsi";
    glb_mimetypes[".vsix"] = "application/vsix";
    glb_mimetypes[".vsixlangpack"] = "text/xml";
    glb_mimetypes[".vsixmanifest"] = "text/xml";
    glb_mimetypes[".vsmdi"] = "application/xml";
    glb_mimetypes[".vspscc"] = "text/plain";
    glb_mimetypes[".vss"] = "application/vnd.visio";
    glb_mimetypes[".vsscc"] = "text/plain";
    glb_mimetypes[".vssettings"] = "text/xml";
    glb_mimetypes[".vssscc"] = "text/plain";
    glb_mimetypes[".vst"] = "application/vnd.visio";
    glb_mimetypes[".vstemplate"] = "text/xml";
    glb_mimetypes[".vsto"] = "application/x-ms-vsto";
    glb_mimetypes[".vsw"] = "application/vnd.visio";
    glb_mimetypes[".vsx"] = "application/vnd.visio";
    glb_mimetypes[".vtx"] = "application/vnd.visio";
    glb_mimetypes[".wav"] = "audio/wav";
    glb_mimetypes[".wave"] = "audio/wav";
    glb_mimetypes[".wax"] = "audio/x-ms-wax";
    glb_mimetypes[".wbk"] = "application/msword";
    glb_mimetypes[".wbmp"] = "image/vnd.wap.wbmp";
    glb_mimetypes[".wcm"] = "application/vnd.ms-works";
    glb_mimetypes[".wdb"] = "application/vnd.ms-works";
    glb_mimetypes[".wdp"] = "image/vnd.ms-photo";
    glb_mimetypes[".webarchive"] = "application/x-safari-webarchive";
    glb_mimetypes[".webm"] = "video/webm";
    glb_mimetypes[".webp"] = "image/webp"; /* https://en.wikipedia.org/wiki/WebP */
    glb_mimetypes[".webtest"] = "application/xml";
    glb_mimetypes[".wiq"] = "application/xml";
    glb_mimetypes[".wiz"] = "application/msword";
    glb_mimetypes[".wks"] = "application/vnd.ms-works";
    glb_mimetypes[".WLMP"] = "application/wlmoviemaker";
    glb_mimetypes[".wlpginstall"] = "application/x-wlpg-detect";
    glb_mimetypes[".wlpginstall3"] = "application/x-wlpg3-detect";
    glb_mimetypes[".wm"] = "video/x-ms-wm";
    glb_mimetypes[".wma"] = "audio/x-ms-wma";
    glb_mimetypes[".wmd"] = "application/x-ms-wmd";
    glb_mimetypes[".wmf"] = "application/x-msmetafile";
    glb_mimetypes[".wml"] = "text/vnd.wap.wml";
    glb_mimetypes[".wmlc"] = "application/vnd.wap.wmlc";
    glb_mimetypes[".wmls"] = "text/vnd.wap.wmlscript";
    glb_mimetypes[".wmlsc"] = "application/vnd.wap.wmlscriptc";
    glb_mimetypes[".wmp"] = "video/x-ms-wmp";
    glb_mimetypes[".wmv"] = "video/x-ms-wmv";
    glb_mimetypes[".wmx"] = "video/x-ms-wmx";
    glb_mimetypes[".wmz"] = "application/x-ms-wmz";
    glb_mimetypes[".woff"] = "application/font-woff";
    glb_mimetypes[".wpl"] = "application/vnd.ms-wpl";
    glb_mimetypes[".wps"] = "application/vnd.ms-works";
    glb_mimetypes[".wri"] = "application/x-mswrite";
    glb_mimetypes[".wrl"] = "x-world/x-vrml";
    glb_mimetypes[".wrz"] = "x-world/x-vrml";
    glb_mimetypes[".wsc"] = "text/scriptlet";
    glb_mimetypes[".wsdl"] = "text/xml";
    glb_mimetypes[".wvx"] = "video/x-ms-wvx";
    glb_mimetypes[".x"] = "application/directx";
    glb_mimetypes[".xaf"] = "x-world/x-vrml";
    glb_mimetypes[".xaml"] = "application/xaml+xml";
    glb_mimetypes[".xap"] = "application/x-silverlight-app";
    glb_mimetypes[".xbap"] = "application/x-ms-xbap";
    glb_mimetypes[".xbm"] = "image/x-xbitmap";
    glb_mimetypes[".xdr"] = "text/plain";
    glb_mimetypes[".xht"] = "application/xhtml+xml";
    glb_mimetypes[".xhtml"] = "application/xhtml+xml";
    glb_mimetypes[".xla"] = "application/vnd.ms-excel";
    glb_mimetypes[".xlam"] = "application/vnd.ms-excel.addin.macroEnabled.12";
    glb_mimetypes[".xlc"] = "application/vnd.ms-excel";
    glb_mimetypes[".xld"] = "application/vnd.ms-excel";
    glb_mimetypes[".xlk"] = "application/vnd.ms-excel";
    glb_mimetypes[".xll"] = "application/vnd.ms-excel";
    glb_mimetypes[".xlm"] = "application/vnd.ms-excel";
    glb_mimetypes[".xls"] = "application/vnd.ms-excel";
    glb_mimetypes[".xlsb"] = "application/vnd.ms-excel.sheet.binary.macroEnabled.12";
    glb_mimetypes[".xlsm"] = "application/vnd.ms-excel.sheet.macroEnabled.12";
    glb_mimetypes[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    glb_mimetypes[".xlt"] = "application/vnd.ms-excel";
    glb_mimetypes[".xltm"] = "application/vnd.ms-excel.template.macroEnabled.12";
    glb_mimetypes[".xltx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.template";
    glb_mimetypes[".xlw"] = "application/vnd.ms-excel";
    glb_mimetypes[".xml"] = "text/xml";
    glb_mimetypes[".xmta"] = "application/xml";
    glb_mimetypes[".xof"] = "x-world/x-vrml";
    glb_mimetypes[".XOML"] = "text/plain";
    glb_mimetypes[".xpm"] = "image/x-xpixmap";
    glb_mimetypes[".xps"] = "application/vnd.ms-xpsdocument";
    glb_mimetypes[".xrm-ms"] = "text/xml";
    glb_mimetypes[".xsc"] = "application/xml";
    glb_mimetypes[".xsd"] = "text/xml";
    glb_mimetypes[".xsf"] = "text/xml";
    glb_mimetypes[".xsl"] = "text/xml";
    glb_mimetypes[".xslt"] = "text/xml";
    glb_mimetypes[".xsn"] = "application/octet-stream";
    glb_mimetypes[".xss"] = "application/xml";
    glb_mimetypes[".xspf"] = "application/xspf+xml";
    glb_mimetypes[".xtp"] = "application/octet-stream";
    glb_mimetypes[".xwd"] = "image/x-xwindowdump";
    glb_mimetypes[".z"] = "application/x-compress";
    glb_mimetypes[".zip"] = "application/zip";
}


void SetSocketOptions ( long sock )
{
    return;
    //Settaggio delle opzioni dei socket
    
    struct linger l;
    int one = 1;
    
    //Mette il linger e il reuse
    /*
     *     Case 1: linger->l_onoff is zero (linger->l_linger has no meaning):
     *         This is the default.
     *         On close(), the underlying stack attempts to gracefully shutdown the connection after
     *         ensuring all unsent data is sent. In the case of connection-oriented protocols such as
     *         TCP, the stack also ensures that sent data is acknowledged by the peer. The stack will
     *         perform the above-mentioned graceful shutdown in the background (after the call to
     *         close() returns), regardless of whether the socket is blocking or non-blocking.
     * 
     *     Case 2: linger->l_onoff is non-zero and linger->l_linger is zero:
     *         A close() returns immediately. The underlying stack discards any unsent data, and, in
     *         the case of connection-oriented protocols such as TCP, sends a RST (reset) to the peer
     *         (this is termed a hard or abortive close). All subsequent attempts by the peer's
     *         application to read()/recv() data will result in an ECONNRESET.
     * 
     *     Case 3: linger->l_onoff is non-zero and linger->l_linger is non-zero:
     *         A close() will either block (if a blocking socket) or fail with EWOULDBLOCK (if
     *         non-blocking) until a graceful shutdown completes or the time specified in
     *         linger->l_linger elapses (time-out). Upon time-out the stack behaves as in case 2 above.
     */
    
    setsockopt ( sock, SOL_SOCKET, SO_REUSEADDR, ( const char * ) &one, sizeof ( one ) );
    l.l_onoff = 0; //Il default
    l.l_linger = 0;
    if ( setsockopt ( sock, SOL_SOCKET, SO_LINGER, ( const char* ) &l, sizeof ( l ) ) < 0 )
        ;
    //perror("setsockopt");
    
    //Elimina delay e cork nei socket
    long opt = 1;
    setsockopt ( sock, IPPROTO_TCP, TCP_NODELAY, ( char * ) ( &opt ), sizeof ( long ) );
    opt = 0;
    setsockopt ( sock, IPPROTO_TCP, TCP_CORK, ( char * ) ( &opt ), sizeof ( long ) );
}

void SetSocketNoDelay ( long sock )
{
    //#ifndef _WIN32
    //Settaggio delle opzioni dei socket
    
    int opt = 1;
    int on = 0;
    setsockopt ( sock, IPPROTO_TCP, TCP_NODELAY, ( char * ) ( &opt ), sizeof ( long ) );
    //#ifdef TCP_CORK
    setsockopt ( sock, SOL_TCP, TCP_CORK, &on, sizeof ( on ) ); /* cork */
    //#endif
    //#endif
    
    return;
}

void SetSocketReuse ( long sock )
{
    //Settaggio delle opzioni dei socket
    
    int one = 1;
    
    setsockopt ( sock, SOL_SOCKET, SO_REUSEADDR, ( const char * ) &one, sizeof ( one ) );
}

void SetSocketUnblocking(int sock_descriptor)
{
    int flags; 
    if ((flags = fcntl(sock_descriptor, F_GETFL, 0)) < 0) 
    { 
        /* Handle error */ 
        return;
    } 
    
    if (fcntl(sock_descriptor, F_SETFL, flags | O_NONBLOCK) < 0) 
    { 
        /* Handle error */ 
        return;
    } 
}


