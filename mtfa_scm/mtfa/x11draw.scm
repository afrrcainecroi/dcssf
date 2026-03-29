(define-module (mtfa x11draw)
  #:use-module  (mtfa error-handler)
  #:use-module  (mtfa utils)
  #:use-module  (mtfa serializer)
  #:use-module  (mtfa unordered-set) ;;unordered set con chiavi (stringhe, numeri o sumimboli: tutto convertito in stringa). Persistente
  #:use-module  (mtfa unordered-map) ;;unordered map con chiavi (stringhe) e valori (qualsiasi cosa). persistente
  #:use-module  (mtfa star-map)      ;;Inserisce stringhe con o senza jolly, la stringa che definisce il jolly e il valore. Cerca le stringhe che matchano!
  #:use-module  (mtfa simple_db)
  #:use-module  (mtfa eis)
  ;;#:use-module  (mtfa fsm)
  ;;#:use-module  (mtfa va)
  #:use-module  (mtfa extset)  ;;gestisce insiemi i cui elementi sono stringhe! consente operazioni di clone, set, check, get all.... Definisce una macro che consente di creare "al volo" una sottoclasse le cui istanze condividono gli stessi elementi.
  #:use-module  (mtfa umset)   ;;è una unordered map (non persistente) che ha stringhe come chiavi e ha insiemi di stringhe come valori. Ogni insert aggiunge all'insieme corrispondente. Definisce inoltre la mtfa-umap-list che consente di mappare liste come chiavi e qualsiasi valore come valore
  #:use-module  (mtfa web)
  #:use-module  (mtfa brg)
  ;;
;;  #:use-module  (pfds sets)
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
  #:use-module  (srfi srfi-9)
  #:use-module  (srfi srfi-11)
  #:use-module  ((srfi srfi-18)
		 #:prefix srfi-18::) ;;thread e mutex
  ;; date & time rinomina per avere un current time che non si sovrappone
  #:use-module  (srfi srfi-19)
  #:use-module  (srfi srfi-26)
  ;;(srfi srfi-28)
  #:use-module  (srfi srfi-43)
  #:use-module  (srfi srfi-60)
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
  #:use-module  (web client)
  ;;
  )

;; (define (dereference-uint8* ptr)
;;   (let ((b (pointer->bytevector ptr 1)))
;;     (bytevector-u8-ref b 0)))
;; (define (dereference-uint16* ptr)
;;   (let ((b (pointer->bytevector ptr 2)))
;;     (bytevector-u16-ref b 0 (native-endianness))))
;; (define (dereference-uint32* ptr)
;;   (let ((b (pointer->bytevector ptr 4)))
;;     (bytevector-u32-ref b 0 (native-endianness))))
;; (define (dereference-uint64* ptr)
;;   (let ((b (pointer->bytevector ptr 8)))
;;     (bytevector-u64-ref b 0 (native-endianness))))


;;
(define our-drawing-lib (dynamic-link "/new_devs/usr/local/shared/lib/our-opencv.so"))
;;
;;extern "C" void * our_Mat_zeros(int rows, int cols)
(define-public cv::mat::zeros (pointer->procedure '* (dynamic-func "our_Mat_zeros" our-drawing-lib)
					   (list int32 int32)))
;;
;;extern "C" void our_Mat_free(void *m)
(define-public cv::mat::free (pointer->procedure void (dynamic-func "our_Mat_free" our-drawing-lib)
					  (list '*)))
;;
;;extern "C" void our_imshow(char * atom_window, void *atom_image)
(define-public cv::imshow (pointer->procedure void (dynamic-func "our_imshow" our-drawing-lib)
				       (list '* '*)))
;;
;;extern "C" void our_moveWindow(char * atom_window, int x, int y)
(define-public cv::moveWindow (pointer->procedure void (dynamic-func "our_moveWindow" our-drawing-lib)
				       (list '* int32 int32)))
;;
;;extern "C" int our_waitKey(int delay)
(define-public cv::waitKey (pointer->procedure void (dynamic-func "our_waitKey" our-drawing-lib)
				       (list int32)))
;;
;;extern "C" void our_rectangle(
    ;; void *m, 
    ;; int x, int y,
    ;; int width, int height,
    ;; int r, int g, int b,
    ;; int thickness, //-1 rettangolo filled, altrimenti spessore linea
    ;; int linetype   //-1, 4, 8, 16
(define-public cv::rectangle (pointer->procedure void (dynamic-func "our_rectangle" our-drawing-lib)
				       (list '* int32 int32 int32 int32 int32 int32 int32 int32 int32)))
;;
;;extern "C" void our_line(void * img, 
  ;; int x1, int y1, int x2, int y2,
  ;; int r, int g, int b,
  ;; int thickness, int lineType
(define-public cv::line (pointer->procedure void (dynamic-func "our_line" our-drawing-lib)
				       (list '* int32 int32 int32 int32 int32 int32 int32 int32 int32)))
;;
;;extern "C" void ourcircle(void* img, int x, int y, int radius,
;;   int r, int g, int b,
;;   int thickness, int lineType
;; )
(define-public cv::circle (pointer->procedure void (dynamic-func "our_circle" our-drawing-lib)
				       (list '* int32 int32 int32 int32 int32 int32 int32 int32)))
;;
;;extern "C" void our_destroyWindow(char * atom_window)
(define-public cv::destroyWindow (pointer->procedure void (dynamic-func "our_destroyWindow" our-drawing-lib)
				       (list '*)))
;;extern "C" void our_namedWindow(char * atom_window, int type)
(define-public cv::namedWindow (pointer->procedure void (dynamic-func "our_namedWindow" our-drawing-lib)
				       (list '* int32)))
;;
;;extern "C" void our_destroyAllWindows()
(define-public cv::destroyAllWindows (pointer->procedure void (dynamic-func "our_destroyAllWindows" our-drawing-lib)
				       (list)))
;;
;;extern "C" void our_resizeWindow(char* winname, int width, int height)
(define-public cv::resizeWindow (pointer->procedure void (dynamic-func "our_resizeWindow" our-drawing-lib)
				       (list '* int32 int32)))


;; (defun* DrawSomething ( )
;;   (let ((mat (cv::mat::zeros 1000 1000))
;; 	(titolo (string->pointer "Titolo della nostra finestra"))
;; 	)
;;     (mtfa-noerr
;;      (begin
;;        (cv::mat::free mat)
;;        (cv::destroyAllWindows)
;;        )
;;      ;;
;;      (cv::namedWindow titolo 0)
;;      (cv::moveWindow titolo 0 0)
;;      (cv::imshow titolo mat)
;;      (cv::resizeWindow titolo 1000 1000)
;;      (cv::rectangle mat 0 0 1000 1000 200 50 50 -1 1)
;;      (cv::circle mat 500 500 100 100 10 200 -1 1)
;;      (cv::circle mat 800 800 100 100 100 50 -1 1)
;;      (cv::imshow titolo mat)
;;      (cv::waitKey 0)
;;      (cv::mat::free mat)
;;      (cv::destroyWindow titolo)
;;      )
;;     )
;;   )

#|
#include <unistd.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define w 400
using namespace cv;

void MyEllipse(Mat img, double angle);
void MyFilledCircle(Mat img, Point center);
void MyPolygon(Mat img);
void MyLine(Mat img, Point start, Point end);

int main(void)
{
  char atom_window[] = "Drawing 1: Atom";
  char rook_window[] = "Drawing 2: Rook";

Mat atom_image = Mat::zeros(w, w, CV_8UC3);
Mat rook_image = Mat::zeros(w, w, CV_8UC3);

MyEllipse(atom_image, 90);
  MyEllipse(atom_image, 0);
  MyEllipse(atom_image, 45);
  MyEllipse(atom_image, -45);
  MyFilledCircle(atom_image, Point(w / 2,
w / 2));
  MyPolygon(rook_image);
  rectangle(rook_image,
            Point(0, 7 * w / 8),
            Point(w, w),
            Scalar(0, 255, 255),
            FILLED,
            LINE_8);
  MyLine(rook_image, Point(0, 15 * w / 16), Point(w, 15 * w / 16));
  MyLine(rook_image, Point(w / 4, 7 * w / 8), Point(w / 4, w));
  MyLine(rook_image, Point(w / 2, 7 * w / 8), Point(w / 2, w));
  MyLine(rook_image, Point(3 * w / 4, 7 * w / 8), Point(3 * w / 4, w));
  imshow(atom_window, atom_image);
  moveWindow(atom_window, 0, 200);
  imshow(rook_window, rook_image);
  moveWindow(rook_window, w, 200);
  waitKey(10000);
  MyEllipse(atom_image, 190);
  MyEllipse(atom_image, 290);
  imshow(atom_window, atom_image);
  waitKey(0);
  return (0);
}
void MyEllipse(Mat img, double angle)
{
  int thickness = 2;
  int lineType = 8;
  ellipse(img,
          Point(w / 2, w / 2),
          Size(w / 4, w / 16),
          angle,
          0,
          360,
          Scalar(255, 0, 0),
          thickness,
          lineType);
}
void MyFilledCircle(Mat img, Point center)
{
  circle(img,
         center,
         w / 32,
         Scalar(0, 0, 255),
         FILLED,
         LINE_8);
}
void MyPolygon(Mat img)
{
  int lineType = LINE_8;
  Point rook_points[1][20];
  rook_points[0][0] = Point(w / 4, 7 * w / 8);
  rook_points[0][1] = Point(3 * w / 4, 7 * w / 8);
  rook_points[0][2] = Point(3 * w / 4, 13 * w / 16);
  rook_points[0][3] = Point(11 * w / 16, 13 * w / 16);
  rook_points[0][4] = Point(19 * w / 32, 3 * w / 8);
  rook_points[0][5] = Point(3 * w / 4, 3 * w / 8);
  rook_points[0][6] = Point(3 * w / 4, w / 8);
  rook_points[0][7] = Point(26 * w / 40, w / 8);
  rook_points[0][8] = Point(26 * w / 40, w / 4);
  rook_points[0][9] = Point(22 * w / 40, w / 4);
  rook_points[0][10] = Point(22 * w / 40, w / 8);
  rook_points[0][11] = Point(18 * w / 40, w / 8);
  rook_points[0][12] = Point(18 * w / 40, w / 4);
  rook_points[0][13] = Point(14 * w / 40, w / 4);
  rook_points[0][14] = Point(14 * w / 40, w / 8);
  rook_points[0][15] = Point(w / 4, w / 8);
  rook_points[0][16] = Point(w / 4, 3 * w / 8);
  rook_points[0][17] = Point(13 * w / 32, 3 * w / 8);
  rook_points[0][18] = Point(5 * w / 16, 13 * w / 16);
  rook_points[0][19] = Point(w / 4, 13 * w / 16);
  const Point *ppt[1] = {rook_points[0]};
  int npt[] = {20};
  fillPoly(img,
           ppt,
           npt,
           1,
           Scalar(255, 255, 255),
           lineType);
}
void MyLine(Mat img, Point start, Point end)
{
  int thickness = 2;
  int lineType = LINE_8;
  line(img,
       start,
       end,
       Scalar(0, 0, 0),
       thickness,
       lineType);
}
|#




;; ;; Messages
;; (define S2D_INFO  1)
;; (define S2D_WARN  2)
;; (define S2D_ERROR 3)

;; ;; Window attributes
;; (define S2D_RESIZABLE  #x20) ;;SDL_WINDOW_RESIZABLE
;; (define S2D_BORDERLESS #x10) ;;SDL_WINDOW_BORDERLESS
;; (define S2D_FULLSCREEN #x1000) ;;SDL_WINDOW_FULLSCREEN_DESKTOP
;; (define S2D_HIGHDPI    #x2000) ;;SDL_WINDOW_ALLOW_HIGHDPI
;; (define S2D_DISPLAY_WIDTH  0)
;; (define S2D_DISPLAY_HEIGHT 0)

;; ;; Viewport scaling modes
;; (define S2D_FIXED   1)
;; (define S2D_EXPAND  2)
;; (define S2D_SCALE   3)
;; (define S2D_STRETCH 4)

;; ;; Positions
;; (define S2D_CENTER       1)
;; (define S2D_TOP_LEFT     2)
;; (define S2D_TOP_RIGHT    3)
;; (define S2D_BOTTOM_LEFT  4)
;; (define S2D_BOTTOM_RIGHT 5)

;; ;; Keyboard events
;; (define S2D_KEY_DOWN 1)  ;; key is pressed
;; (define S2D_KEY_HELD 2)  ;; key is held down
;; (define S2D_KEY_UP   3)  ;; key is released

;; ;; Mouse events
;; (define S2D_MOUSE_DOWN   1)  ;; mouse button pressed
;; (define S2D_MOUSE_UP     2)  ;; mouse button released
;; (define S2D_MOUSE_SCROLL 3)  ;; mouse scrolling or wheel movement
;; (define S2D_MOUSE_MOVE   4)  ;; mouse movement
;; (define S2D_MOUSE_LEFT   1) ;;SDL_BUTTON_LEFT
;; (define S2D_MOUSE_MIDDLE 2) ;;SDL_BUTTON_MIDDLE
;; (define S2D_MOUSE_RIGHT  3) ;;SDL_BUTTON_RIGHT
;; (define S2D_MOUSE_X1     4) ;;SDL_BUTTON_X1
;; (define S2D_MOUSE_X2     5) ;;SDL_BUTTON_X2
;; (define S2D_MOUSE_SCROLL_NORMAL   #x0) ;;SDL_MOUSEWHEEL_NORMAL
;; (define S2D_MOUSE_SCROLL_INVERTED #x1) ;;SDL_MOUSEWHEEL_FLIPPED

;; ;; Controller events
;; (define S2D_AXIS        1)
;; (define S2D_BUTTON_DOWN 2)
;; (define S2D_BUTTON_UP   3)

;; ;; Controller axis labels
;; (define S2D_AXIS_INVALID      -1) ;;SDL_CONTROLLER_AXIS_INVALID
;; (define S2D_AXIS_LEFTX        0) ;;SDL_CONTROLLER_AXIS_LEFTX
;; (define S2D_AXIS_LEFTY        1) ;;SDL_CONTROLLER_AXIS_LEFTY
;; (define S2D_AXIS_RIGHTX       2) ;;SDL_CONTROLLER_AXIS_RIGHTX
;; (define S2D_AXIS_RIGHTY       3) ;;SDL_CONTROLLER_AXIS_RIGHTY
;; (define S2D_AXIS_TRIGGERLEFT  4) ;;SDL_CONTROLLER_AXIS_TRIGGERLEFT
;; (define S2D_AXIS_TRIGGERRIGHT 5) ;;SDL_CONTROLLER_AXIS_TRIGGERRIGHT
;; (define S2D_AXIS_MAX          6) ;;SDL_CONTROLLER_AXIS_MAX

;; ;; Controller button labels
;; (define S2D_BUTTON_INVALID       -1) ;;SDL_CONTROLLER_BUTTON_INVALID
;; (define S2D_BUTTON_A             0) ;;SDL_CONTROLLER_BUTTON_A
;; (define S2D_BUTTON_B             1) ;;SDL_CONTROLLER_BUTTON_B
;; (define S2D_BUTTON_X             2) ;;SDL_CONTROLLER_BUTTON_X
;; (define S2D_BUTTON_Y             3) ;;SDL_CONTROLLER_BUTTON_Y
;; (define S2D_BUTTON_BACK          4) ;;SDL_CONTROLLER_BUTTON_BACK
;; (define S2D_BUTTON_GUIDE         5) ;;SDL_CONTROLLER_BUTTON_GUIDE
;; (define S2D_BUTTON_START         6) ;;SDL_CONTROLLER_BUTTON_START
;; (define S2D_BUTTON_LEFTSTICK     7) ;;SDL_CONTROLLER_BUTTON_LEFTSTICK
;; (define S2D_BUTTON_RIGHTSTICK    8) ;;SDL_CONTROLLER_BUTTON_RIGHTSTICK
;; (define S2D_BUTTON_LEFTSHOULDER  9) ;;SDL_CONTROLLER_BUTTON_LEFTSHOULDER
;; (define S2D_BUTTON_RIGHTSHOULDER 10) ;;SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
;; (define S2D_BUTTON_DPAD_UP       11) ;;SDL_CONTROLLER_BUTTON_DPAD_UP
;; (define S2D_BUTTON_DPAD_DOWN     12) ;;SDL_CONTROLLER_BUTTON_DPAD_DOWN
;; (define S2D_BUTTON_DPAD_LEFT     13) ;;SDL_CONTROLLER_BUTTON_DPAD_LEFT
;; (define S2D_BUTTON_DPAD_RIGHT    14) ;;SDL_CONTROLLER_BUTTON_DPAD_RIGHT
;; (define S2D_BUTTON_MAX           15) ;;SDL_CONTROLLER_BUTTON_MAX

;; (define S2D_Event_struct (list
;; 			  int32 ;;0 which;
;; 			  int32 ;;1 type;
;; 			  int32 ;;2 button;
;; 			  uint8 ;;3 dblclick;
;; 			  '*    ;;4 char *key;
;; 			  int32 ;;5 x;
;; 			  int32 ;;6 y;
;; 			  int32 ;;7 delta_x;
;; 			  int32 ;;8 delta_y;
;; 			  int32 ;;9 direction;
;; 			  int32 ;;10 axis;
;; 			  int32 ;;11 value;
;; 			  ))

;; ;; typedef void (*S2D_Update)();
;; ;; typedef void (*S2D_Render)();
;; ;; typedef void (*S2D_On_Key)(S2D_Event e);
;; ;; typedef void (*S2D_On_Mouse)(S2D_Event e);
;; ;; typedef void (*S2D_On_Controller)(S2D_Event e);

;; ;;S2D_GL_Point, for graphics calculations
;; (define S2D_GL_Point_struct (list
;; 			     float ;;0 x			;
;; 			     float ;,1 y			;
;; 			     ))


;; (define S2D_Color_struct (list
;; 			  float ;;0 r		;
;; 			  float ;;1 g		;
;; 			  float ;;2 b		;
;; 			  float ;;3 a		;
;; 			  ))

;; (define S2D_Mouse_struct (list
;; 			  int32 ;;0 visible ;
;; 			  int32 ;;1 x	 ;
;; 			  int32 ;;2 y	 ;
;; 			  ))

;; (define  S2D_Viewport_struct (list
;; 			      int32 ;; width		;
;; 			      int32 ;; height		;
;; 			      int32 ;; mode		;
;; 			      ))

;; (define S2D_Window_struct (list
;; 			   '*     ;;0   SDL_Window *sdl;
;; 			   '*     ;;1   SDL_GLContext glcontext;
;; 			   '*     ;;2   const GLubyte *S2D_GL_VENDOR;
;; 			   '*     ;;3   const GLubyte *S2D_GL_RENDERER;
;; 			   '*     ;;4   const GLubyte *S2D_GL_VERSION;
;; 			   int32  ;;5   GLint S2D_GL_MAJOR_VERSION;
;; 			   int32  ;;6   GLint S2D_GL_MINOR_VERSION;
;; 			   '*     ;;7   const GLubyte *S2D_GL_SHADING_LANGUAGE_VERSION;
;; 			   '*     ;;8   const char *title;
;; 			   int32  ;;9   int width;
;; 			   int32  ;;10   int height;
;; 			   int32  ;;11   int orig_width;
;; 			   int32  ;;12   int orig_height;
;; 			   S2D_Viewport_struct ;;13
;; 			   '*     ;;14   S2D_Update update; function
;; 			   '*     ;;15   S2D_Render render; function
;; 			   int32  ;;16   int flags;
;; 			   S2D_Mouse_struct ;;17
;; 			   '*     ;;18   S2D_On_Key on_key; function
;; 			   '*     ;;19   S2D_On_Mouse on_mouse; function
;; 			   '*     ;;20   S2D_On_Controller on_controller; function
;; 			   uint8  ;;21   bool vsync;
;; 			   int32  ;;22   int fps_cap;
;; 			   S2D_Color_struct ;;23 background;
;; 			   '*     ;;24   const char *icon;
;; 			   uint32 ;;25   Uint32 frames;
;; 			   uint32 ;;26   Uint32 elapsed_ms;
;; 			   uint32 ;;27   Uint32 loop_ms;
;; 			   uint32 ;;28   Uint32 delay_ms;
;; 			   double ;;29   double fps;
;; 			   uint8  ;;30   bool close;
;; 			   uint32 ;;31   x
;; 			   uint32 ;;31   y
;; 			   uint8  ;;32   bool stopped;
;; 			   ))
;; (define S2D_Image_struct (list
;; 			  '*    ;;0 const char *path;
;; 			  '*    ;;1 SDL_Surface *surface;
;; 			  int32 ;;2 format;
;; 			  uint32;;3 texture_id;
;; 			  S2D_Color_struct ;;4 color;
;; 			  int32 ;;5 x;
;; 			  int32 ;;6 y;
;; 			  int32 ;;7 width;
;; 			  int32 ;;8 height;
;; 			  int32 ;;9 orig_width;
;; 			  int32 ;;10 orig_height;
;; 			  float ;;11 rotate;  // Rotation angle in degrees
;; 			  float ;;12 rx;      // X coordinate to be rotated around
;; 			  float ;;13 ry;      // Y coordinate to be rotated around
;; 			  ))

;; (define S2D_Sprite_struct (list 
;; 			   '*    ;;0 const char *path;
;; 			   S2D_Image_struct ;;1 *img;
;; 			   S2D_Color_struct ;;2 color;
;; 			   int32 ;;3 x;
;; 			   int32 ;;4 y;
;; 			   int32 ;;5 width;
;; 			   int32 ;;6 height;
;; 			   int32 ;;7 clip_width;
;; 			   int32 ;;8 clip_height;
;; 			   float ;;9 rotate;  // Rotation angle in degrees
;; 			   float ;;10 rx;      // X coordinate to be rotated around
;; 			   float ;;11 ry;      // Y coordinate to be rotated around
;; 			   float ;;12 tx1;
;; 			   float ;;13 ty1;
;; 			   float ;;14 tx2;
;; 			   float ;;15 ty2;
;; 			   float ;;16 tx3;
;; 			   float ;;17 ty3;
;; 			   float ;;18 tx4;
;; 			   float ;;19 ty4;
;; 			   ))
;; (define S2D_Text_struct (list
;; 			 '*    ;;0 const char *font;
;; 			 '*    ;;1 SDL_Surface_struct ;;1 *surface;
;; 			 uint32;;2 texture_id;
;; 			 '*    ;;3 TTF_Font *font_data;
;; 			 S2D_Color_struct ;;4 color;
;; 			 '*    ;;5 char *msg;
;; 			 int32 ;;6 x;
;; 			 int32 ;;7 y;
;; 			 int32 ;;8 width;
;; 			 int32 ;;9 height;
;; 			 float ;;10 rotate;  // Rotation angle in degrees
;; 			 float ;;11 rx;      // X coordinate to be rotated around
;; 			 float ;;12 ry;      // Y coordinate to be rotated around
;; 			 ))

;; ;; (procedure->pointer int
;; ;; 		    (lambda (x y)
;; ;; 		      ;; X and Y are pointers so,
;; ;; 		      ;; for convenience, dereference
;; ;; 		      ;; them before calling COMPARE.
;; ;; 		      (compare (dereference-uint8* x)
;; ;; 			       (dereference-uint8* y)))
;; ;; 		    (list '* '*))

;; ;;(define libsimple2d (dynamic-link "/volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/simple2d/simple2d/build/libsimple2d.so"))
;; (define libsimple2d (dynamic-link "/new_devs/usr/local/shared/lib/libsimple2d.so"))
;; ;;
;; ;;bool S2D_FileExists(const char *path);
;; (define S2D_FileExists (pointer->procedure uint8 (dynamic-func "S2D_FileExists" libsimple2d) (list '*)))
;; ;;
;; ;; void S2D_Log(int type, const char *msg, ...);
;; ;; void S2D_Error(const char *caller, const char *msg, ...);
;; ;; void S2D_Diagnostics(bool status);
;; ;;
;; ;;void S2D_Windows_EnableTerminalColors();
;; (define S2D_Windows_EnableTerminalColors (pointer->procedure void (dynamic-func "S2D_Windows_EnableTerminalColors" libsimple2d) (list)))
;; ;;
;; ;;bool S2D_Init();
;; (define S2D_Init (pointer->procedure uint8 (dynamic-func "S2D_Init" libsimple2d) (list)))
;; ;;
;; ;;void S2D_GetDisplayDimensions(int *w, int *h);
;; (define S2D_GetDisplayDimensions (pointer->procedure void (dynamic-func "S2D_GetDisplayDimensions" libsimple2d) (list '* '*)))
;; ;;
;; ;;void S2D_Quit(void)
;; (define S2D_Quit (pointer->procedure void (dynamic-func "S2D_Quit" libsimple2d) (list)))
;; ;;
;; ;;S2D_GL_Point S2D_RotatePoint(S2D_GL_Point p, GLfloat angle, GLfloat rx, GLfloat ry);
;; ;;S2D_GL_Point S2D_GetRectRotationPoint(int x, int y, int w, int h, int position);
;; ;;
;; ;;void S2D_DrawTriangle(GLfloat x1, GLfloat y1, GLfloat r1, GLfloat g1, GLfloat b1, GLfloat a1, GLfloat x2, GLfloat y2, GLfloat r2, GLfloat g2, GLfloat b2, GLfloat a2, GLfloat x3, GLfloat y3, GLfloat r3, GLfloat g3, GLfloat b3, GLfloat a3)
;; (define S2D_DrawTriangle (pointer->procedure void (dynamic-func "S2D_DrawTriangle" libsimple2d)
;; 					     (list float float float float float float
;; 						   float float float float float float
;; 						   float float float float float float)))
;; ;;
;; ;;void S2D_DrawQuad(GLfloat x1, GLfloat y1, GLfloat r1, GLfloat g1, GLfloat b1, GLfloat a1, GLfloat x2, GLfloat y2, GLfloat r2, GLfloat g2, GLfloat b2, GLfloat a2, GLfloat x3, GLfloat y3, GLfloat r3, GLfloat g3, GLfloat b3, GLfloat a3, GLfloat x4, GLfloat y4, GLfloat r4, GLfloat g4, GLfloat b4, GLfloat a4);
;; (define S2D_DrawQuad (pointer->procedure void (dynamic-func "S2D_DrawQuad" libsimple2d) (list float float float float float float float float float float float float float float float float float float float float float float float float)))
;; ;;
;; ;;void S2D_DrawLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat width, GLfloat r1, GLfloat g1, GLfloat b1, GLfloat a1, GLfloat r2, GLfloat g2, GLfloat b2, GLfloat a2, GLfloat r3, GLfloat g3, GLfloat b3, GLfloat a3, GLfloat r4, GLfloat g4, GLfloat b4, GLfloat a4);
;; ;;
;; (define S2D_DrawLine (pointer->procedure void (dynamic-func "S2D_DrawLine" libsimple2d) (list float float float float float float float float float float float float float float float float float float float float float)))
;; ;;
;; ;;void S2D_DrawCircle(GLfloat x, GLfloat y, GLfloat radius, int sectors, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
;; (define S2D_DrawCircle (pointer->procedure void (dynamic-func "S2D_DrawCircle" libsimple2d) (list float float float int32 float float float float)))
;; ;;
;; ;;S2D_Image *S2D_CreateImage(const char *path);
;; (define S2D_CreateImage (pointer->procedure '* (dynamic-func "S2D_CreateImage" libsimple2d) (list '*)))
;; ;;
;; ;;void S2D_RotateImage(S2D_Image *img, GLfloat angle, int position);
;; (define S2D_RotateImage (pointer->procedure void (dynamic-func "S2D_RotateImage" libsimple2d) (list '* float int32)))
;; ;;
;; ;;void S2D_DrawImage(S2D_Image *img);
;; (define S2D_DrawImage (pointer->procedure void (dynamic-func "S2D_DrawImage" libsimple2d) (list '*)))
;; ;;
;; ;;void S2D_FreeImage(S2D_Image *img);
;; (define S2D_FreeImage (pointer->procedure void (dynamic-func "S2D_FreeImage" libsimple2d) (list '*)))
;; ;;
;; ;;S2D_Sprite *S2D_CreateSprite(const char *path);
;; (define S2D_CreateSprite (pointer->procedure '* (dynamic-func "S2D_CreateSprite" libsimple2d) (list '*)))
;; ;;
;; ;;void S2D_ClipSprite(S2D_Sprite *spr, int x, int y, int w, int h);
;; (define S2D_ClipSprite (pointer->procedure void (dynamic-func "S2D_ClipSprite" libsimple2d) (list '* int32 int32 int32 int32)))
;; ;;
;; ;;void S2D_RotateSprite(S2D_Sprite *spr, GLfloat angle, int position);
;; (define S2D_RotateSprite (pointer->procedure void (dynamic-func "S2D_RotateSprite" libsimple2d) (list '* float int32)))
;; ;;
;; ;;void S2D_DrawSprite(S2D_Sprite *spr);
;; (define S2D_DrawSprite (pointer->procedure void (dynamic-func "S2D_DrawSprite" libsimple2d) (list '*)))
;; ;;
;; ;;void S2D_FreeSprite(S2D_Sprite *spr);
;; (define S2D_FreeSprite (pointer->procedure void (dynamic-func "S2D_FreeSprite" libsimple2d) (list '*)))
;; ;;
;; ;;S2D_Text *S2D_CreateText(const char *font, const char *msg, int size);
;; (define S2D_CreateText (pointer->procedure '* (dynamic-func "S2D_CreateText" libsimple2d) (list '* '* int32)))
;; ;;
;; ;;void S2D_SetText(S2D_Text *txt, const char *msg, ...);
;; ;;
;; ;;void S2D_RotateText(S2D_Text *txt, GLfloat angle, int position);
;; (define S2D_RotateText (pointer->procedure void (dynamic-func "S2D_RotateText" libsimple2d) (list '* float int32)))
;; ;;
;; ;;void S2D_DrawText(S2D_Text *txt);
;; (define S2D_DrawText (pointer->procedure void (dynamic-func "S2D_DrawText" libsimple2d) (list '*)))
;; ;;
;; ;;void S2D_FreeText(S2D_Text *txt);
;; (define S2D_FreeText (pointer->procedure void (dynamic-func "S2D_FreeText" libsimple2d) (list '*)))
;; ;;
;; ;;
;; ;; S2D_Sound *S2D_CreateSound(const char *path);
;; ;; void S2D_PlaySound(S2D_Sound *snd);
;; ;; int S2D_GetSoundVolume(S2D_Sound *snd);
;; ;; void S2D_SetSoundVolume(S2D_Sound *snd, int volume);
;; ;; int S2D_GetSoundMixVolume();
;; ;; void S2D_SetSoundMixVolume(int volume);
;; ;; void S2D_FreeSound(S2D_Sound *snd);
;; ;; S2D_Music *S2D_CreateMusic(const char *path);
;; ;; void S2D_PlayMusic(S2D_Music *mus, bool loop);
;; ;; void S2D_PauseMusic();
;; ;; void S2D_ResumeMusic();
;; ;; void S2D_StopMusic();
;; ;; int S2D_GetMusicVolume();
;; ;; void S2D_SetMusicVolume(int volume);
;; ;; void S2D_FadeOutMusic(int ms);
;; ;; void S2D_FreeMusic(S2D_Music *mus);
;; ;; void S2D_GetMouseOnViewport(S2D_Window *window, int wx, int wy, int *x, int *y);
;; ;; void S2D_ShowCursor();
;; ;; void S2D_HideCursor();
;; ;; void S2D_AddControllerMapping(const char *map);
;; ;; void S2D_AddControllerMappingsFromFile(const char *path);
;; ;; bool S2D_IsController(SDL_JoystickID id);
;; ;; void S2D_OpenControllers();
;; ;;
;; ;;S2D_Window *S2D_CreateWindow(const char *title, int x, int y, int width, int height, S2D_Update, S2D_Render, int flags)
;; (define S2D_CreateWindow (pointer->procedure '* (dynamic-func "S2D_CreateWindow" libsimple2d) (list '* int32 int32 int32 int32 '* '* int32)))
;; ;;
;; ;;int S2D_Show(S2D_Window *window);
;; (define S2D_Show (pointer->procedure int32 (dynamic-func "S2D_Show" libsimple2d) (list '*)))
;; ;;
;; ;;void S2D_SetIcon(S2D_Window *window, const char *icon);
;; (define S2D_SetIcon (pointer->procedure void (dynamic-func "S2D_SetIcon" libsimple2d) (list '* '*)))
;; (define C_Free (pointer->procedure void (dynamic-func "free" (dynamic-link)) (list '*)))
;; ;;
;; ;;void S2D_Screenshot(S2D_Window *window, const char *path);
;; ;;
;; ;;int S2D_Close(S2D_Window *window);
;; (define S2D_Close (pointer->procedure int32 (dynamic-func "S2D_Close" libsimple2d) (list '*)))
;; ;;
;; ;;int S2D_FreeWindow(S2D_Window *window);
;; (define S2D_FreeWindow (pointer->procedure int32 (dynamic-func "S2D_FreeWindow" libsimple2d) (list '* uint8)))
;; ;; ;;
;; ;; ;;
;; ;; (S2D_FileExists (pointer->procedure uint8 (dynamic-func "S2D_FileExists" libsimple2d) (list '*)))
;; ;; ;;
;; ;; ;;
;; ;; (S2D_FileExists (pointer->procedure uint8 (dynamic-func "S2D_FileExists" libsimple2d) (list '*)))
;; ;; ;;
;; ;; ;;
;; ;; (S2D_FileExists (pointer->procedure uint8 (dynamic-func "S2D_FileExists" libsimple2d) (list '*)))
;; ;; ;;
;; ;; ;;
;; ;; (S2D_FileExists (pointer->procedure uint8 (dynamic-func "S2D_FileExists" libsimple2d) (list '*)))
;; ;; ;;
;; ;;
;; ;;
;; (defun-public mtfa-draw-x11 (xpos ypos xsize ysize xview yview title)
;;   (let* (
;; 	 ;;La finestra!
;; 	 (window #f)
;; 	 (parsed-window #f)
;; 	 (stop #f)
;; 	 (drawings '() ;; (list (cons S2D_DrawTriangle '( 320  50 1 1 1 1
;; 		   ;; 				   540 430 1 1 1 1
;; 		   ;; 				   100 430 1 1 1 1)))
;; 		   )
;; 	 ;;
;; 	 ;;La callback di render!!!
;; 	 (render (procedure->pointer 
;; 		  void
;; 		  (lambda ()
;; 		    (unless stop
;; 		      (mtfa-noerr
;; 		       #f
;; 		       (for-each (lambda (p) (apply (car p) (cdr p))) drawings))))
;; 		  '()))
;; 	 (update (procedure->pointer 
;; 		  void
;; 		  (lambda ()
;; 		    #t
;; 		    (when stop
;; 		      1 ;;(S2D_Close window)
;; 		      )
;; 		    )
;; 		  '()))
;; 	 (th #f)
;; 	 (terminated #f)
;; 	 )
;;     ;;u
;;     ;;Fa la set della vista e mostra la finestra
;;     (set! window (S2D_CreateWindow (string->pointer title) xpos ypos xsize ysize update render 0))
;;     ;;
;;     ;;Agginge view
;;     (set! parsed-window (parse-c-struct window S2D_Window_struct))
;;     (list-set! parsed-window 13 (list xview yview S2D_FIXED))
;;     (set! window (make-c-struct S2D_Window_struct parsed-window))
;;     ;;
;;     ;;
;;     (set! th (make-thread (lambda () (S2D_Show window) (set! terminated #t))))
;;     ;;
;;     ;;A questo punto torna la closure
;;     (lambda (cmd . pars)
;;       (match (cons cmd pars)
;; 	(('close)
;; 	 (set! stop #t)
;; 	 (S2D_Close window)  ;;Con lo 0 non fa la free
;; 	 (sleep 2)
;; 	 (S2D_FreeWindow window 0)  ;;Con lo 0 non fa la free
;; 	 ;;(sleep 2)
;; 	 ;;(C_Free window) ;;la free la faccio io
;; 	 ;;(dynamic-unlink libsimple2d)
;; 	 (set! window %null-pointer))
;; 	(('rect x y w h r g b)
;; 	 (set! drawings (cons (cons S2D_DrawQuad 
;; 				    `(,(+ 0 x) ,(+ 0 y) ,r ,g ,b 1
;; 				      ,(+ w x) ,(+ 0 y) ,r ,g ,b 1
;; 				      ,(+ w x) ,(+ h y) ,r ,g ,b 1
;; 				      ,(+ 0 x) ,(+ h y) ,r ,g ,b 1)
;; 				    )
;; 			      drawings)))
;; 	(('square x y l r g b)
;; 	 (set! drawings (cons (cons S2D_DrawQuad 
;; 				    `(,(+ 0 x) ,(+ 0 y) ,r ,g ,b 1
;; 				      ,(+ l x) ,(+ 0 y) ,r ,g ,b 1
;; 				      ,(+ l x) ,(+ l y) ,r ,g ,b 1
;; 				      ,(+ 0 x) ,(+ l y) ,r ,g ,b 1)
;; 				    )
;; 			      drawings)))
;; 	(('clear)
;; 	 (set! drawings '()))
;; 	(_ (Show! "Commands:\n
;; 'close
;; 'rect x y w h r g b
;; 'square x y l r g b
;; 'clear
;; "))))))




