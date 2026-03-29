(set! %load-compiled-path (cons "/new_devs/usr/local/mtfa-guile-libs/" %load-compiled-path))

(define-module (mtfa opencv)
  ;;I miei moduli
  #:use-module  (mtfa error-handler)
  #:use-module  (mtfa utils)
  #:use-module  (mtfa serializer)
  #:use-module  (mtfa unordered-set) ;;unordered set con chiavi (stringhe, numeri o sumimboli: tutto convertito in stringa). Persistente
  #:use-module  (mtfa unordered-map) ;;unordered map con chiavi (stringhe) e valori (qualsiasi cosa). persistente
  #:use-module  (mtfa star-map)      ;;Inserisce stringhe con o senza jolly, la stringa che definisce il jolly e il valore. Cerca le stringhe che matchano!
  #:use-module  (mtfa simple_db)
  #:use-module  (mtfa eis)
  ;;(mtfa fsm)
  #:use-module  (mtfa va)
  #:use-module  (mtfa extset)  ;;gestisce insiemi i cui elementi sono stringhe! consente operazioni di clone, set, check, get all.... Definisce una macro che consente di creare "al volo" una sottoclasse le cui istanze condividono gli stessi elementi.
  #:use-module  (mtfa umset)   ;;è una unordered map (non persistente) che ha stringhe come chiavi e ha insiemi di stringhe come valori. Ogni insert aggiunge all'insieme corrispondente. Definisce inoltre la mtfa-umap-list che consente di mappare liste come chiavi e qualsiasi valore come valore
  #:use-module  (mtfa web)
  #:use-module  (mtfa brg)
  ;;#:use-module  (mtfa nn)
  #:use-module  (mtfa avl)
  #:use-module  (mtfa eqt)
  #:use-module  (mtfa opencv)
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
  #:use-module  (ice-9 popen)
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
  #:use-module  (web client)
  ;;
  #:export (
	    mtfa-opencv-imgfile-to-mat
	    mtfa-opencv-mat-to-imgfile
	    mtfa-opencv-compute-image-hash-AverageHash
	    mtfa-opencv-compare-image-hash-AverageHash
	    mtfa-opencv-compute-image-hash-BlockMeanHash0
	    mtfa-opencv-compare-image-hash-BlockMeanHash0
	    mtfa-opencv-compute-image-hash-BlockMeanHash1
	    mtfa-opencv-compare-image-hash-BlockMeanHash1
	    mtfa-opencv-compute-image-hash-ColorMomentHash
	    mtfa-opencv-compare-image-hash-ColorMomentHash
	    mtfa-opencv-compute-image-hash-MarrHildrethHash
	    mtfa-opencv-compare-image-hash-MarrHildrethHash
	    mtfa-opencv-compute-image-hash-PHash
	    mtfa-opencv-compare-image-hash-PHash
	    mtfa-opencv-compute-image-hash-RadialVarianceHash
	    mtfa-opencv-compare-image-hash-RadialVarianceHash
	    mtfa-opencv-img-hash-to-bytevector
	    mtfa-opencv-bytevector-to-img-hash
	    
	    mtfa-opencv-detector-p
	    mtfa-opencv-detector-make
	    mtfa-opencv-detector-run
	    mtfa-opencv-detector-stop

	    mtfa-zed2-detector-p
	    mtfa-zed2-detector-make
	    mtfa-zed2-detector-run
	    mtfa-zed2-detector-stop

	    mtfa-yolo4-detector-p
	    mtfa-yolo4-detector-make
	    mtfa-yolo4-detector-run
	    mtfa-yolo4-detector-stop

	    mtfa-opencv-crop-rectangle
	    ;;mtfa-opencv-get-raw-cloud
	    mtfa-opencv-binary-op

	    mtfa-opencv-make-mat-zeros
	    mtfa-opencv-make-empty-mat

	    mtfa-opencv-draw-imshow
	    mtfa-opencv-draw-move-window
	    mtfa-opencv-draw-wait-key
	    mtfa-opencv-draw-rectangle
	    mtfa-opencv-draw-polygon 
	    mtfa-opencv-fill-polygons 
	    mtfa-opencv-fill-convex-polygon 
	    mtfa-opencv-draw-circle
	    mtfa-opencv-draw-ellipse
	    mtfa-opencv-draw-destroy-window
	    mtfa-opencv-draw-destroy-all-windows
	    mtfa-opencv-draw-named-window
	    mtfa-opencv-draw-resize-window
	    mtfa-opencv-draw-put-text
	    mtfa-opencv-draw-text-size 
	    mtfa-opencv-draw-line
	    
	    mtfa-opencv-extrun-detector ;;per l'esecuzione del task externo!
	    mtfa-opencv-mat-info  ;;torna righe e colonne della matrice
	    mtfa-opencv-cap-info

	    mtfa-opencv-draw-display-overlay
	    mtfa-opencv-draw-display-status-bar
	    mtfa-opencv-draw-set-window-property

	    mtfa-lap
	    mtfa-opencv-change-brightness
	    mtfa-opencv-change-contrast
	    mtfa-opencv-change-brightness-and-contrast
	    mtfa-opencv-resize
	    mtfa-opencv-prop-resize
	    mtfa-opencv-gray
	    mtfa-opencv-add-gaussian-noise
	    mtfa-opencv-salt-and-pepper
	    mtfa-opencv-gaussian-blur
	    mtfa-opencv-median-blur
	    mtfa-opencv-rotate
	    mtfa-opencv-erode
	    mtfa-opencv-contours
	    mtfa-opencv-dilate
	    mtfa-opencv-transform
	    mtfa-opencv-gamma-correction
	    mtfa-opencv-equalize-histogram
	    mtfa-opencv-add-watermark
	    mtfa-opencv-jpeg-compression

	    mtfa-opencv-open-input-stream
	    mtfa-opencv-close-input-stream
	    mtfa-opencv-read-input-stream
	    mtfa-opencv-input-stream-is-opened
	    mtfa-opencv-open-output-stream
	    mtfa-opencv-close-output-stream
	    mtfa-opencv-write-output-stream
	    mtfa-opencv-output-stream-is-opened
	    mtfa-opencv-make-polygon
	    mtfa-opencv-point-in-polygon
	    mtfa-opencv-compose
	    mtfa-opencv-clone
	    
	    mtfa-opencv-rgbtohsv
	    mtfa-opencv-create-trackbar
	    mtfa-opencv-find-circles
	    mtfa-opencv-img-mean
	    mtfa-opencv-set-trackbar-max
	    mtfa-opencv-set-trackbar-min
	    mtfa-opencv-set-trackbar-pos
	    mtfa-opencv-get-trackbar-pos
	    mtfa-opencv-set-mouse-callback

	    ))

(load-extension "/new_devs/usr/local/mtfa-guile-libs/mtfa/mtfa_opencv.so" "init_mtfa_opencv")

;;Interpolazioni e altro
(define-public mtfa-opencv::interpolator-types::_2D::ThinPlateSplineInterpolator 0)
(define-public mtfa-opencv::interpolator-types::_2D::BicubicInterpolator 1)
(define-public mtfa-opencv::interpolator-types::_2D::BilinearInterpolator 2)
(define-public mtfa-opencv::interpolator-types::_1D::CubicSplineInterpolator 3)
(define-public mtfa-opencv::interpolator-types::_1D::LinearInterpolator 4)
(define-public mtfa-opencv::interpolator-types::_1D::MonotonicInterpolator 5)

(define-public (mtfa-opencv-make-interpolator type)
  "(mtfa-opencv-make-interpolator type) where types are _2D::ThinPlateSplineInterpolator, _2D::BicubicInterpolator, _2D::BilinearInterpolator, _1D::CubicSplineInterpolator, _1D::LinearInterpolator, _1D::MonotonicInterpolator"
  (mtfa_opencv_make_interpolator type))

(define-public (mtfa-opencv-interpolator-add-serie interpolator list-of-series)
  "(mtfa-opencv-interpolator-add-serie type list-of-series) where interpolator has to have been created with mtfa-opencv-make-interpolator and the list is like ((x1 x2 ...)(y1 y2 ...)(z1 z2 ...)). Where zi is the value objective of the interpolation"
  (mtfa_opencv_interpolator_add_serie interpolator list-of-series))

(define-public (mtfa-opencv-interpolator-interpolate interpolator list-of-coordinates)
  "(mtfa-opencv-interpolator-interpolate type list-of-coordinates) where interpolator has to have been created with mtfa-opencv-make-interpolator and the list is like (x y). the interpolated value is returned"
  (mtfa_opencv_interpolator_interpolate interpolator list-of-coordinates))

;;Features e best matching
(define-public (mtfa-opencv-features-detector img)
  "Finds features using FAST detector and BEBLID descriptor algorithms. Returns two values: descriptors and points"
  (mtfa_opencv_features_detector img))

(define* (mtfa-opencv-features-find-transformation descr1 points1 descr2 points2 #:key (norm 6) (method 8))
  "Finds the number of matches and the number of inliers obtained matching points from the two set of features detected.
descr1/2 and points1/2 are obtained from the mtfa-opencv-features-detector.
Norm is an integer: NORM_INF 1, NORM_L1 2, NORM_L2 4, NORM_L2SQR 5, NORM_HAMMING 6, NORM_HAMMING2 7, NORM_RELATIVE 8, NORM_MINMAX 32 (default HAMMING).
Method is an integer (default RANSAC):
       DEFAULT 0,
       LMEDS  = 4,  least-median of squares algorithm
       RANSAC = 8,  RANSAC algorithm
       RHO    = 16, RHO algorithm
       USAC_DEFAULT = 32,  USAC algorithm, default settings
       USAC_PARALLEL = 33, USAC, parallel version
       USAC_FM_8PTS = 34,  USAC, fundamental matrix 8 points
       USAC_FAST = 35,     USAC, fast settings
       USAC_ACCURATE = 36, USAC, accurate settings
       USAC_PROSAC = 37,   USAC, sorted points, runs PROSAC
       USAC_MAGSAC = 38    USAC, runs MAGSAC++
"
  (mtfa_opencv_features_find_transformation descr1 points1 descr2 points2 norm method))
(export mtfa-opencv-features-find-transformation)

;;I ph-tree
(define-public (mtfa-phtree-box-2d-make)
  "Makes a ph-tree for 2d boxes"
  (let ((db (make-hash-table)))
    (cons db (mtfa_phtree_box_2d_make))))

(define-public (mtfa-phtree-box-3d-make)
  "Makes a ph-tree for 3d boxes"
  (let ((db (make-hash-table)))
    (cons db (mtfa_phtree_box_3d_make))))

(define-public (mtfa-phtree-point-2d-make)
  "Makes a ph-tree for 2d points"
  (let ((db (make-hash-table)))
    (cons db (mtfa_phtree_point_2d_make))))

(define-public (mtfa-phtree-point-3d-make)
  "Makes a ph-tree for 3d points"
  (let ((db (make-hash-table)))
    (cons db (mtfa_phtree_point_3d_make))))

(define-public (mtfa-phtree-2d-add-points ph-tree lop)
  "adds 2d points list to the tree: '((x0 y0 id0)(x1 y1 id1)...)"
  (for-each (lambda (pt)
	      (hash-set! (car ph-tree) (last pt) pt))
	    lop)
  (mtfa_phtree_2d_add_points (cdr ph-tree) lop))

(define-public (mtfa-phtree-3d-add-points ph-tree lop)
  "adds 3d points list to the tree: '((x0 y0 z0 id0)(x1 y1 z1 id1)...)"
  (for-each (lambda (pt)
	      (hash-set! (car ph-tree) (last pt) pt))
	    lop)
  (mtfa_phtree_3d_add_points (cdr ph-tree) lop))

(define-public (mtfa-phtree-2d-add-boxes ph-tree lob)
  "adds 2d boxes list to the tree: '(((xmin ymin)(xmax ymax) id) ((xmin ymin)(xmax ymax) id) ...)"
  (for-each (lambda (pt)
	      (hash-set! (car ph-tree) (last pt) pt))
	    lob)
  (mtfa_phtree_2d_add_boxes (cdr ph-tree) lob))

(define-public (mtfa-phtree-3d-add-boxes ph-tree lob)
  "adds 3d boxes list to the tree: '(((xmin ymin zmin)(xmax ymax zmax) id) ((xmin ymin zmin)(xmax ymax zmax) id) ...)"
  (for-each (lambda (pt)
	      (hash-set! (car ph-tree) (last pt) pt))
	    lob)
  (mtfa_phtree_3d_add_boxes (cdr ph-tree) lob))

(define-public (mtfa-phtree-2d-query-points ph-tree min-range max-range)
  "Queries 2d points contained in the box delimited by min-range (x y) and max-range (x y)"
  (mtfa_phtree_2d_query_points (cdr ph-tree) min-range max-range))

(define-public (mtfa-phtree-3d-query-points ph-tree min-range max-range)
  "Queries 3d points contained in the box delimited by min-range (x y z) and max-range (x y z)"
  (mtfa_phtree_3d_query_points (cdr ph-tree) min-range max-range))

(define-public (mtfa-phtree-2d-query-boxes ph-tree min-range max-range included)
  "Queries 2d boxes contained in the box delimited by min-range (x y) and max-range (x y). Included is booleand and defines if completely included or if intersected"
  (mtfa_phtree_2d_query_boxes (cdr ph-tree) min-range max-range included))

(define-public (mtfa-phtree-3d-query-boxes ph-tree min-range max-range included)
  "Queries 3d boxes contained in the box delimited by min-range (x y z) and max-range (x y z). Included is booleand and defines if completely included or if intersected"
  (mtfa_phtree_3d_query_boxes (cdr ph-tree) min-range max-range included))

(define-public (mtfa-phtree-2d-knn-points ph-tree min-result-size center)
  "gives at least min-result-size points as list of (id . distance) ordered by min distance from the point (x y) given as center."
	       (mtfa_phtree_2d_knn_points (cdr ph-tree) min-result-size center))

(define-public (mtfa-phtree-3d-knn-points ph-tree min-result-size center)
  "gives at least min-result-size points as list of (id . distance) ordered by min distance from the point (x y z) given as center."
	       (mtfa_phtree_3d_knn_points (cdr ph-tree) min-result-size center))

(define-public (mtfa-phtree-2d-getall-points ph-tree)
  "Gives the ids of all the 2d points"
  (hash-map->list (lambda (k v) k) (car ph-tree)))

(define-public (mtfa-phtree-3d-getall-points ph-tree)
  "Gives the ids of all the 3d points"
  (hash-map->list (lambda (k v) k) (car ph-tree)))

(define-public (mtfa-phtree-2d-getall-boxes ph-tree)
  "Gives the ids of all the 2d boxes"
  (hash-map->list (lambda (k v) k) (car ph-tree)))

(define-public (mtfa-phtree-3d-getall-boxes ph-tree)
  "Gives the ids of all the 3d boxes"
  (hash-map->list (lambda (k v) k) (car ph-tree)))

(define-public (mtfa-phtree-2d-get-point ph-tree pid)
  "Gives the complete data associated to the id"
  (hash-ref (car ph-tree) pid))

(define-public (mtfa-phtree-3d-get-point ph-tree pid)
  "Gives the complete data associated to the id"
  (hash-ref (car ph-tree) pid))

(define-public (mtfa-phtree-2d-get-box ph-tree pid)
  "Gives the complete data associated to the id"
  (hash-ref (car ph-tree) pid))

(define-public (mtfa-phtree-3d-get-box ph-tree pid)
  "Gives the complete data associated to the id"
  (hash-ref (car ph-tree) pid))

(define-public (mtfa-phtree-2d-erase-points ph-tree list-of-pid)
  (mtfa_phtree_2d_erase_points (cdr ph-tree)
			       (remove not (map (lambda (it) (hash-ref (car ph-tree) it)) list-of-pid)))
  (for-each (lambda (i) (hash-remove! (car ph-tree) i)) list-of-pid))
(define-public (mtfa-phtree-3d-erase-points ph-tree list-of-pid)
  (mtfa_phtree_3d_erase_points (cdr ph-tree)
			       (remove not (map (lambda (it) (hash-ref (car ph-tree) it)) list-of-pid)))
  (for-each (lambda (i) (hash-remove! (car ph-tree) i)) list-of-pid))
(define-public (mtfa-phtree-2d-erase-boxes ph-tree list-of-pid)
  (mtfa_phtree_2d_erase_boxes (cdr ph-tree)
			       (remove not (map (lambda (it) (hash-ref (car ph-tree) it)) list-of-pid)))
  (for-each (lambda (i) (hash-remove! (car ph-tree) i)) list-of-pid))
(define-public (mtfa-phtree-3d-erase-boxes ph-tree list-of-pid)
  (mtfa_phtree_3d_erase_boxes (cdr ph-tree)
			      (remove not (map (lambda (it) (hash-ref (car ph-tree) it)) list-of-pid)))
  (for-each (lambda (i) (hash-remove! (car ph-tree) i)) list-of-pid))





(define (mtfa-opencv-detector-p detector) (mtfa_opencv_detector_p detector))
(define (mtfa-opencv-detector-make cfg weights) (mtfa_opencv_detector_make cfg weights))
(define (mtfa-opencv-detector-run detector par-list) (mtfa_opencv_detector_run detector par-list))
(define (mtfa-opencv-detector-stop detector) (mtfa_opencv_detector_stop detector))
(define (mtfa-opencv-detector-get-zed-camera detector cameranum) (mtfa_opencv_detector_get_zed_camera detector cameranum))

(define (mtfa-zed2-detector-p detector) (mtfa_zed2_detector_p detector))
(define (mtfa-zed2-detector-make) (mtfa_zed2_detector_make))
(define (mtfa-zed2-detector-run detector par-list) (mtfa_zed2_detector_run detector par-list))
(define (mtfa-zed2-detector-stop detector) (mtfa_zed2_detector_stop detector))
(define (mtfa-zed2-detector-get-zed-camera detector cameranum) (mtfa_zed2_detector_get_zed_camera detector cameranum))

(define (mtfa-yolo4-detector-p detector) (mtfa_yolo4_detector_p detector))
(define (mtfa-yolo4-detector-make) (mtfa_yolo4_detector_make))
(define (mtfa-yolo4-detector-run detector par-list) (mtfa_yolo4_detector_run detector par-list))
(define (mtfa-yolo4-detector-stop detector) (mtfa_yolo4_detector_stop detector))

(define (mtfa-opencv-imgfile-to-mat imgpath) (mtfa_opencv_imgfile_to_mat imgpath))
(define (mtfa-opencv-mat-to-imgfile mat filepath) (mtfa_opencv_mat_to_imgfile mat filepath))
(define-public (mtfa-opencv-bytevector-to-imgmat bv) "bv is an image in any image format" (mtfa_opencv_bytevector_to_imgmat bv))
(define-public (mtfa-opencv-imgmat-to-bytevector imgmat format) "format is .jpg, .png, ..." (mtfa_opencv_imgmat_to_bytevector imgmat format))

(define (mtfa-opencv-crop-rectangle mat tlx tly width height) (mtfa_opencv_crop_rectangle mat tlx tly width height))
;;(define (mtfa-opencv-get-raw-cloud mat) (mtfa_opencv_get_raw_cloud mat))
(define (mtfa-opencv-binary-op m1 m2 op) (mtfa_opencv_binary_op m1 m2 op))
(define-public mtfa-opencv::binary-op::ADD 0)
(define-public mtfa-opencv::binary-op::SUB 1)
(define-public mtfa-opencv::binary-op::AND 2)
(define-public mtfa-opencv::binary-op::OR 3)
(define-public mtfa-opencv::binary-op::XOR 4)
(define-public mtfa-opencv::binary-op::MUL 5)
(define-public mtfa-opencv::binary-op::DIV 6)
(define (mtfa-opencv-compute-image-hash-AverageHash mat) (mtfa_opencv_compute_image_hash_AverageHash mat))
(define (mtfa-opencv-compare-image-hash-AverageHash hash-1 hash-2) (mtfa_opencv_compare_image_hash_AverageHash hash-1 hash-2))
(define (mtfa-opencv-compute-image-hash-BlockMeanHash0 mat) (mtfa_opencv_compute_image_hash_BlockMeanHash0 mat))
(define (mtfa-opencv-compare-image-hash-BlockMeanHash0 hash-1 hash2) (mtfa_opencv_compare_image_hash_BlockMeanHash0 hash-1 hash2))
(define (mtfa-opencv-compute-image-hash-BlockMeanHash1 mat) (mtfa_opencv_compute_image_hash_BlockMeanHash1 mat))
(define (mtfa-opencv-compare-image-hash-BlockMeanHash1 hash-1 hash2) (mtfa_opencv_compare_image_hash_BlockMeanHash1 hash-1 hash2))
(define (mtfa-opencv-compute-image-hash-ColorMomentHash mat) (mtfa_opencv_compute_image_hash_ColorMomentHash mat))
(define (mtfa-opencv-compare-image-hash-ColorMomentHash hash-1 hash2) (mtfa_opencv_compare_image_hash_ColorMomentHash hash-1 hash2))
(define (mtfa-opencv-compute-image-hash-MarrHildrethHash mat) (mtfa_opencv_compute_image_hash_MarrHildrethHash mat))
(define (mtfa-opencv-compare-image-hash-MarrHildrethHash hash-1 hash2) (mtfa_opencv_compare_image_hash_MarrHildrethHash hash-1 hash2))
(define (mtfa-opencv-compute-image-hash-PHash mat) (mtfa_opencv_compute_image_hash_PHash mat))
(define (mtfa-opencv-compare-image-hash-PHash hash-1 hash2) (mtfa_opencv_compare_image_hash_PHash hash-1 hash2))
(define (mtfa-opencv-compute-image-hash-RadialVarianceHash mat) (mtfa_opencv_compute_image_hash_RadialVarianceHash mat))
(define (mtfa-opencv-compare-image-hash-RadialVarianceHash hash-1 hash2) (mtfa_opencv_compare_image_hash_RadialVarianceHash hash-1 hash2))
(define (mtfa-opencv-img-hash-to-bytevector mat) (mtfa_opencv_img_hash_to_bytevector mat))
(define (mtfa-opencv-bytevector-to-img-hash bv) (mtfa_opencv_bytevector_to_img_hash bv))
(define (mtfa-opencv-make-mat-zeros rows cols bytelen type colors) (mtfa_opencv_make_mat_zeros rows cols bytelen type colors))
(define (mtfa-opencv-make-empty-mat) (mtfa_opencv_make_empty_mat))
(define (mtfa-opencv-draw-imshow wname mat) (mtfa_opencv_draw_imshow wname mat))
(define (mtfa-opencv-draw-move-window wname x y) (mtfa_opencv_draw_move_window wname x y))
(define (mtfa-opencv-draw-wait-key ms) (mtfa_opencv_draw_wait_key ms))
(define (mtfa-opencv-draw-rectangle mat x y w h rgb thick linetype) (mtfa_opencv_draw_rectangle mat x y w h rgb thick linetype))
(define (mtfa-opencv-draw-polygon mat points ncontours isClosed rgb thick linetype)  (mtfa_opencv_draw_polygon mat points ncontours isClosed rgb thick linetype))
(define (mtfa-opencv-fill-polygons mat list-of-poly rgb)  (mtfa_opencv_fill_polygons mat list-of-poly rgb))
(define (mtfa-opencv-fill-convex-polygon mat points rgb)  (mtfa_opencv_fill_convex_polygon mat points rgb))
(define (mtfa-opencv-draw-circle mat x y radius rgb thick linetype) (mtfa_opencv_draw_circle mat x y radius rgb thick linetype))
(define (mtfa-opencv-draw-ellipse mat x y axes angle start_angle end_angle rgb thick linetype) (mtfa_opencv_draw_ellipse mat x y axes angle start_angle end_angle rgb thick linetype))
(define (mtfa-opencv-draw-destroy-window name) (mtfa_opencv_draw_destroy_window name))
(define (mtfa-opencv-draw-destroy-all-windows) (mtfa_opencv_draw_destroy_all_windows))
(define (mtfa-opencv-draw-named-window name type) (mtfa_opencv_draw_named_window name type))
(define (mtfa-opencv-draw-resize-window name w h) (mtfa_opencv_draw_resize_window name w h))
(define (mtfa-opencv-draw-put-text mat text x y fontFace fontScale rgb thick linetype) (mtfa_opencv_draw_put_text mat text x y fontFace fontScale rgb thick linetype))
(define (mtfa-opencv-draw-text-size text fontface fontscale thick) (mtfa_opencv_draw_text_size text fontface fontscale thick))
(define (mtfa-opencv-draw-line mat x1 y1 x2 y2 rgb thick linetype) (mtfa_opencv_draw_line mat x1 y1 x2 y2 rgb thick linetype))
(define (mtfa-opencv-mat-info mat) (mtfa_opencv_mat_info mat))
(define (mtfa-opencv-cap-info cap)
  "returns list of 
POS_MSEC Current position of the video file in milliseconds.
POS_FRAMES 0-based index of the frame to be decoded/captured next.
POS_AVI_RATIO Relative position of the video file: 0=start of the film, 1=end of the film.
FRAME_WIDTH Width of the frames in the video stream.
FRAME_HEIGHT Height of the frames in the video stream.
FPS Frame rate.
FOURCC 4-character code of codec. see VideoWriter::fourcc .
FRAME_COUNT Number of frames in the video file."
  (mtfa_opencv_cap_info cap))
(define (mtfa-opencv-draw-display-overlay winname text ms) (mtfa_opencv_draw_display_overlay winname text ms))
(define (mtfa-opencv-draw-display-status-bar winname text ms) (mtfa_opencv_draw_display_status_bar winname text ms))
(define (mtfa-opencv-draw-set-window-property winname property value) (mtfa_opencv_draw_set_window_property winname property value))
(define* (mtfa-lap table) (mtfa_lap table))



(define (mtfa-opencv-change-brightness mat added) (mtfa_opencv_change_brightness mat added))
(define (mtfa-opencv-change-contrast mat multiplied) (mtfa_opencv_change_contrast mat multiplied))
(define (mtfa-opencv-change-brightness-and-contrast mat brightness:0..200 contrast:0..200) "Brightness 0..200, Contrast 0..200" (mtfa_opencv_change_brightness_and_contrast mat brightness:0..200 contrast:0..200))
(define (mtfa-opencv-resize mat resize_x resize_y) (mtfa_opencv_resize mat resize_x resize_y))
(define (mtfa-opencv-prop-resize mat resize_x) (mtfa_opencv_prop_resize mat resize_x))
(define (mtfa-opencv-gray mat) (mtfa_opencv_gray mat ))
(define (mtfa-opencv-add-gaussian-noise mat sigma mean) (mtfa_opencv_add_gaussian_noise mat sigma mean))
(define (mtfa-opencv-salt-and-pepper mat pa pb) (mtfa_opencv_salt_and_pepper mat pa pb))
(define (mtfa-opencv-gaussian-blur mat sigma_x sigma_y) (mtfa_opencv_gaussian_blur mat sigma_x sigma_y))
(define (mtfa-opencv-median-blur mat ksize) (mtfa_opencv_median_blur mat ksize))
(define (mtfa-opencv-rotate mat angle ) (mtfa_opencv_rotate mat angle ))
(define (mtfa-opencv-erode mat x_size y_size iterations) (mtfa_opencv_erode mat x_size y_size iterations))
(define (mtfa-opencv-contours mat canny blur-size-3) (mtfa_opencv_contours mat canny blur-size-3))
(define (mtfa-opencv-dilate mat x_size y_size iterations) (mtfa_opencv_dilate mat x_size y_size iterations))
(define (mtfa-opencv-transform mat tl_x tl_y tr_x tr_y br_x br_y bl_x bl_y) (mtfa_opencv_transform mat tl_x tl_y tr_x tr_y br_x br_y bl_x bl_y))
(define (mtfa-opencv-gamma-correction mat gamma ) (mtfa_opencv_gamma_correction mat gamma ))
(define (mtfa-opencv-equalize-histogram mat) (mtfa_opencv_equalize_histogram mat))
(define (mtfa-opencv-add-watermark img wat center_x_rel center_y_rel width_rel height_rel) (mtfa_opencv_add_watermark img wat center_x_rel center_y_rel width_rel height_rel))
(define (mtfa-opencv-jpeg-compression mat quality ) (mtfa_opencv_jpeg_compression mat quality ))
				  
(define* (mtfa-opencv-open-input-stream stream #:optional (preferencies '())) (if (not (null? preferencies))
										  (mtfa_opencv_open_input_stream stream preferencies)
										  (mtfa_opencv_open_input_stream stream)))
(define (mtfa-opencv-close-input-stream opened-stream) (mtfa_opencv_close_input_stream opened-stream))
(define (mtfa-opencv-read-input-stream mat opened-stream)(mtfa_opencv_read_input_stream mat opened-stream))
(define (mtfa-opencv-input-stream-is-opened opened-stream) (mtfa_opencv_input_stream_is_opened opened-stream))
    
(define (mtfa-opencv-open-output-stream stream fourcc fps size is_color) (mtfa_opencv_open_output_stream stream fourcc fps size is_color))
(define (mtfa-opencv-close-output-stream opened-stream) (mtfa_opencv_close_output_stream opened-stream))
(define (mtfa-opencv-write-output-stream mat opened-stream) (mtfa_opencv_write_output_stream mat opened-stream))
(define (mtfa-opencv-output-stream-is-opened opened-stream) (mtfa_opencv_output_stream_is_opened opened-stream))

(define (mtfa-opencv-make-polygon list-of-xypairs) (mtfa_opencv_make_polygon list-of-xypairs))
(define (mtfa-opencv-point-in-polygon xypair polygon) (mtfa_opencv_point_in_polygon xypair polygon))
(define (mtfa-opencv-compose im1 im2 vertical) (mtfa_opencv_compose im1 im2 vertical))
(define (mtfa-opencv-clone im1) (mtfa_opencv_clone im1))

(define (mtfa-opencv-rgbtohsv int_R int_G int_B) (mtfa_opencv_rgbtohsv int_R int_G int_B))
(define (mtfa-opencv-create-trackbar string_label string_window_name int_initial_value int_max_value call_TrackbarCallback void_userdata) (mtfa_opencv_create_trackbar string_label string_window_name int_initial_value int_max_value call_TrackbarCallback void_userdata))

(define (mtfa-opencv-set-trackbar-max string_label string_window_name int_value) (mtfa_opencv_set_trackbar_max string_label string_window_name int_value))
(define (mtfa-opencv-set-trackbar-min string_label string_window_name int_value) (mtfa_opencv_set_trackbar_min string_label string_window_name int_value))
(define (mtfa-opencv-set-trackbar-pos string_label string_window_name int_value) (mtfa_opencv_set_trackbar_pos string_label string_window_name int_value))
(define (mtfa-opencv-get-trackbar-pos string_label string_window_name) (mtfa_opencv_get_trackbar_pos string_label string_window_name))

(define*
  (mtfa-opencv-find-circles mat #:optional (int_dp:2 '()) (int_minDist:54 '()) (int_p1:90 '()) (int_p2:40 '()) (int_minr:7 '()) (int_maxr:19 '()) (int_blur:5 '()) (polygon '())) 
  (mtfa_opencv_find_circles mat int_dp:2 int_minDist:54 int_p1:90 int_p2:40 int_minr:7 int_maxr:19 int_blur:5 polygon))

(define (mtfa-opencv-img-mean mat_img int_center_x int_center_y int_side) (mtfa_opencv_img_mean mat_img int_center_x int_center_y int_side))
(define (mtfa-opencv-set-mouse-callback string_window_name call_MouseCallback void_userdata) (mtfa_opencv_set_mouse_callback string_window_name call_MouseCallback void_userdata))

;;I parametri utilizzati dalle funzioni OPENCV
(define-public mtfa-opencv::window-flags::WINDOW_NORMAL #x00000000)
(define-public mtfa-opencv::window-flags::WINDOW_AUTOSIZE #x00000001)
(define-public mtfa-opencv::window-flags::WINDOW_OPENGL #x00001000)
(define-public mtfa-opencv::window-flags::WINDOW_FULLSCREEN 1)
(define-public mtfa-opencv::window-flags::WINDOW_FREERATIO #x00000100)
(define-public mtfa-opencv::window-flags::WINDOW_KEEPRATIO #x00000000)
(define-public mtfa-opencv::window-flags::WINDOW_GUI_EXPANDED #x00000000)
(define-public mtfa-opencv::window-flags::WINDOW_GUI_NORMAL #x00000010)

(define-public mtfa-opencv::window-property-flags::WND_PROP_FULLSCREEN 0)
(define-public mtfa-opencv::window-property-flags::WND_PROP_AUTOSIZE 1)
(define-public mtfa-opencv::window-property-flags::WND_PROP_ASPECT_RATIO 2)
(define-public mtfa-opencv::window-property-flags::WND_PROP_OPENGL 3)
(define-public mtfa-opencv::window-property-flags::WND_PROP_VISIBLE 4)
(define-public mtfa-opencv::window-property-flags::WND_PROP_TOPMOST 5)

(define-public mtfa-opencv::MOUSE_EVENT_FLAG_LBUTTON 1)
(define-public mtfa-opencv::MOUSE_EVENT_FLAG_RBUTTON 2)
(define-public mtfa-opencv::MOUSE_EVENT_FLAG_MBUTTON 4)
(define-public mtfa-opencv::MOUSE_EVENT_FLAG_CTRLKEY 8)
(define-public mtfa-opencv::MOUSE_EVENT_FLAG_SHIFTKEY 16)
(define-public mtfa-opencv::MOUSE_EVENT_FLAG_ALTKEY 32)

(define-public mtfa-opencv::EVENT_MOUSEMOVE 0)
(define-public mtfa-opencv::EVENT_LBUTTONDOWN 1)
(define-public mtfa-opencv::EVENT_RBUTTONDOWN 2)
(define-public mtfa-opencv::EVENT_MBUTTONDOWN 3)
(define-public mtfa-opencv::EVENT_LBUTTONUP 4)
(define-public mtfa-opencv::EVENT_RBUTTONUP 5)
(define-public mtfa-opencv::EVENT_MBUTTONUP 6)
(define-public mtfa-opencv::EVENT_LBUTTONDBLCLK 7)
(define-public mtfa-opencv::EVENT_RBUTTONDBLCLK 8)
(define-public mtfa-opencv::EVENT_MBUTTONDBLCLK 9)
(define-public mtfa-opencv::EVENT_MOUSEWHEEL 10)
(define-public mtfa-opencv::EVENT_MOUSEHWHEEL 11)

(define-public mtfa-opencv::QT_PUSH_BUTTON 0)
(define-public mtfa-opencv::QT_CHECKBOX 1)
(define-public mtfa-opencv::QT_RADIOBOX 2)
(define-public mtfa-opencv::QT_NEW_BUTTONBAR 1024)
 
(define-public mtfa-opencv::QT_STYLE_NORMAL 0)
(define-public mtfa-opencv::QT_STYLE_ITALIC 1)
(define-public mtfa-opencv::QT_STYLE_OBLIQUE 2)


(define-public mtfa-opencv::QT_FONT_LIGHT 25)
(define-public mtfa-opencv::QT_FONT_NORMAL 50)
(define-public mtfa-opencv::QT_FONT_DEMIBOLD 63)
(define-public mtfa-opencv::QT_FONT_BOLD 75)
(define-public mtfa-opencv::QT_FONT_BLACK 87)

;;La ZED CAMERA
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::BRIGHTNESS 0)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::CONTRAST 1)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::HUE 2)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::SATURATION 3)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::SHARPNESS 4)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::GAMMA 5)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::GAIN 6)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::EXPOSURE 7)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::AEC_AGC 8)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::AEC_AGC_ROI 9)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::WHITEBALANCE_TEMPERATURE 10)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::WHITEBALANCE_AUTO 11)
(define-public mtfa-opencv-zed::VIDEO_SETTINGS::LED_STATUS 12)


(define-public (mtfa-opencv-zed-camera-grab zed-camera-pointer) (mtfa_opencv_zed_camera_grab  zed-camera-pointer))
(define-public (mtfa-opencv-zed-camera-get-image zed-camera-pointer) (mtfa_opencv_zed_camera_get_image zed-camera-pointer))

(define-public (mtfa-opencv-zed-camera-set zed-camera-pointer video-setting value) (mtfa_opencv_zed_camera_set zed-camera-pointer video-setting value))
(define-public (mtfa-opencv-zed-camera-get zed-camera-pointer video-setting) (mtfa_opencv_zed_camera_get zed-camera-pointer video-setting))
(define-public (mtfa-opencv-zed-camera-reset zed-camera-pointer) (mtfa_opencv_zed_camera_reset zed-camera-pointer))
(define-public (mtfa-zed2-detector-get-zed-camera zed2-detector camera-num) (mtfa_zed2_detector_get_zed_camera zed2-detector camera-num))
(define-public (mtfa-opencv-detector-get-zed-camera detector camera-num) (mtfa_opencv_detector_get_zed_camera detector camera-num))

(define-public (mtfa-opencv-levenshtein-distance str1 str2)
  "Calcola il numero minimo di insert/replace/delete necessarie a trasformare str1 in str2"
  (mtfa_opencv_levenshtein_distance str1 str2))

(define*-public (mtfa-opencv-make-gpu-mat #:optional (rows '()) (cols '()) (fill '()))
  (mtfa_opencv_make_gpu_mat rows cols fill))

(define*-public (mtfa-opencv-convert-mat-to-gpumat mat #:optional (dst '()))
  (if (null? dst)
      (mtfa_opencv_convert_mat_to_gpumat mat)
      (mtfa_opencv_convert_mat_to_gpumat mat dst)))

(define*-public (mtfa-opencv-convert-gpumat-to-mat gpumat #:optional (dst '()))
  (if (null? dst)
      (mtfa_opencv_convert_gpumat_to_mat gpumat)
      (mtfa_opencv_convert_gpumat_to_mat gpumat dst)))

(define*-public (mtfa-opencv-make-gpumat-from-lolod listoflistofdouble #:optional (dst '()))
  (if (null? dst)
      (mtfa_opencv_make_gpumat_from_lolod listoflistofdouble)
      (mtfa_opencv_make_gpumat_from_lolod listoflistofdouble dst)))

(define-public (mtfa-opencv-convert-gpumat-to-lolod gpumat) "convert from gpumat to list of list of double"
	       (mtfa_opencv_convert_gpumat_to_lolod gpumat))

(define-public (mtfa-opencv-gpumat-gemm dst src1 src1_t src2 src2_t alpha src3 src3_t beta)
	       "dst = src1*src2*alpha+src3*beta, gli elementi booleani _t indicano che la matrice va trasposta. alpha e beta sono float. Se src2 è '(), non viene moltiplicato nulla a src1, tranne il valore alpha. Se src3 è '(), non viene sommato nulla al risultato, tranne il valore beta"
	(mtfa_opencv_gpumat_gemm dst src1 src1_t src2 src2_t alpha src3 src3_t beta))
(define-public (mtfa-opencv-transpose-gpumat dst src)
  (mtfa_opencv_transpose_gpumat dst src))
(define-public (mtfa-opencv-gpumat-rows-cols src)
  (mtfa_opencv_gpumat_rows_cols src))
(define-public (mtfa-opencv-gpumat-clone src dst)
  (mtfa_opencv_gpumat_clone src dst))
(define-public (mtfa-opencv-gpumat-math-compare dst add1 add2 cmd)
  (mtfa_opencv_gpumat_math_compare dst add1 add2 cmd))
(define-public (mtfa-opencv-gpumat-math-add dst add1 add2)
  (mtfa_opencv_gpumat_math_add dst add1 add2))
(define-public (mtfa-opencv-gpumat-math-sub dst sub1 sub2)
  (mtfa_opencv_gpumat_math_sub dst sub1 sub2))
(define-public (mtfa-opencv-gpumat-math-mul dst mol1 mol2 scale)
  (mtfa_opencv_gpumat_math_mul dst mol1 mol2 scale))
(define-public (mtfa-opencv-gpumat-math-div dst div1 div2 scale)
  (mtfa_opencv_gpumat_math_div dst div1 div2 scale))
(define-public (mtfa-opencv-gpumat-math-and dst div1 div2)
  (mtfa_opencv_gpumat_math_and dst div1 div2))
(define-public (mtfa-opencv-gpumat-math-or dst div1 div2)
  (mtfa_opencv_gpumat_math_or dst div1 div2))
(define-public (mtfa-opencv-gpumat-math-xor dst div1 div2)
  (mtfa_opencv_gpumat_math_xor dst div1 div2))
(define-public (mtfa-opencv-gpumat-math-not dst div1)
  (mtfa_opencv_gpumat_math_not dst div1))
(define-public (mtfa-opencv-gpumat-math-absdiff dst sub1 sub2)
  (mtfa_opencv_gpumat_math_absdiff dst sub1 sub2))
(define-public (mtfa-opencv-gpumat-math-abs dst src)
  (mtfa_opencv_gpumat_math_abs dst src))
(define-public (mtfa-opencv-gpumat-math-sqr dst src)
  (mtfa_opencv_gpumat_math_sqr dst src))
(define-public (mtfa-opencv-gpumat-math-sqrt dst src)
  (mtfa_opencv_gpumat_math_sqrt dst src))
(define-public (mtfa-opencv-gpumat-math-exp dst src)
  (mtfa_opencv_gpumat_math_exp dst src))
(define-public (mtfa-opencv-gpumat-math-log dst src)
  (mtfa_opencv_gpumat_math_log dst src))
(define-public (mtfa-opencv-gpumat-math-pow dst src exp)
  (mtfa_opencv_gpumat_math_pow dst src exp))
(define-public (mtfa-opencv-gpumat-math-min dst op1 op2)
  (mtfa_opencv_gpumat_math_min dst op1 op2))
(define-public (mtfa-opencv-gpumat-math-max dst op1 op2)
  (mtfa_opencv_gpumat_math_max dst op1 op2))
(define-public (mtfa-opencv-gpumat-math-add-weighted dst add1 alpha add2 beta gamma)
  (mtfa_opencv_gpumat_math_add_weighted dst add1 alpha add2 beta gamma))

(define*-public (mtfa-opencv-gpumat-make-diag side value #:optional (dst '()))
  (if (null? dst)
      (mtfa_opencv_gpumat_make_diag side value)
      (mtfa_opencv_gpumat_make_diag side value dst)))

(define-public (mtfa-opencv-gpumat-convert-vector-to-diag dst src)
   (mtfa_opencv_gpumat_convert_vector_to_diag dst src))

(define-public (mtfa-opencv-gpumat-math-find-min-max src)
  (mtfa_opencv_gpumat_math_find_min_max src))

(define-public (mtfa-opencv-gpumat-math-sum src)
  (mtfa_opencv_gpumat_math_sum src))

(define-public (mtfa-opencv-gpumat-transpose dst src)
  (mtfa_opencv_gpumat_transpose dst src))
(define-public (mtfa-opencv-gpumat-set mat row col value)
  (mtfa_opencv_gpumat_set mat row col value))

(define-public (mtfa-opencv-gpumat-append-row dst src row) (mtfa_opencv_gpumat_add_row dst src row))
(define-public (mtfa-opencv-gpumat-del-last-row dst src) (mtfa_opencv_gpumat_del_last_row dst src))

(define-public (mtfa-opencv-mat-add-row dst row) (mtfa_opencv_mat_add_row dst row))
(define-public (mtfa-opencv-make-mat-from-lolod lolod) (mtfa_opencv_make_mat_from_lolod lolod))

(define-public (mtfa-opencv-gpumat-cuda-reset)
   (mtfa_opencv_gpumat_cuda_reset))

(define-public (mtfa-opencv-gpumat-math-normalize scm_dst scm_src scm_alpha scm_beta scm_norm_type)
   (mtfa_opencv_gpumat_math_normalize scm_dst scm_src scm_alpha scm_beta scm_norm_type))

(define-public (mtfa-opencv-gpumat-reuse-buffer scm_mat scm_rows scm_cols)
   (mtfa_opencv_gpumat_reuse_buffer scm_mat scm_rows scm_cols))
(define-public (mtfa-opencv-gpumat-math-tanh scm_dst scm_src)
	       (mtfa_opencv_gpumat_math_tanh scm_dst scm_src))

;;extern "C" void mtfa_opencv_gpumat_upload_raw(cv::cuda::GpuMat* mat, int rows, int cols, float* data)
(define-public (mtfa-opencv-gpumat-upload-raw mat rows cols data)
  (mtfa_opencv_gpumat_upload_raw mat rows cols data))
(define-public (mtfa-opencv-gpumat-upload-batch mat data batch_size input_size)
  (mtfa_opencv_gpumat_upload_batch mat data batch_size input_size))
(define-public (mtfa-opencv-gpumat-reduce src dst dim reduceOp)
  (mtfa_opencv_gpumat_reduce src dst dim reduceOp))
(define-public (mtfa-opencv-gpumat-argmax-cols src dst_indices batch_size)
  (mtfa_opencv_gpumat_argmax_cols src dst_indices batch_size))

(define-public (mtfa-opencv-gpumat-argmax-cols src dst_indices batch_size)
  (mtfa_opencv_gpumat_argmax_cols src dst_indices batch_size))

(define-public (mtfa-opencv-gpumat-cross-entropy pred target)
  (mtfa_opencv_gpumat_cross_entropy pred target))
(define-public (mtfa-opencv-gpumat-row-range scm_mat scm_start scm_end)
  (mtfa_opencv_gpumat_row_range scm_mat scm_start scm_end))
(define-public (mtfa-opencv-gpumat-col-range scm_mat scm_start scm_end)
  (mtfa_opencv_gpumat_col_range scm_mat scm_start scm_end))
(define-public (mtfa-opencv-gpumat-max-val src)
  (mtfa_opencv_gpumat_max_val src))
(define-public (mtfa-opencv-gpumat-rand-uniform mat min max)
  (mtfa_opencv_gpumat_rand_uniform_cpu mat min max))
(define*-public (mtfa-opencv-gpumat-debug-dump scm_mat scm_name #:optional cols)
	(if (unspecified? cols)
		(mtfa_opencv_gpumat_debug_dump scm_mat scm_name)
		(mtfa_opencv_gpumat_debug_dump scm_mat scm_name cols)))



(define*-public (mtfa-opencv-gpumat-evaluate-batch scm_pred scm_target #:optional cols)
	(if (unspecified? cols)
		(mtfa_opencv_gpumat_evaluate_batch scm_pred scm_target)
		(mtfa_opencv_gpumat_evaluate_batch scm_pred scm_target cols)))
(define-public (mtfa-opencv-gpumat-math-sign dst op1)
	       (mtfa_opencv_gpumat_math_sign dst op1))
(define-public (mtfa-opencv-mat-get src r c)(mtfa_opencv_mat_get src r c))
(define-public (mtfa-opencv-make-cpu-mat r c)(mtfa_opencv_make_cpu_mat r c))
(define-public (mtfa-opencv-mat-set src r c v)(mtfa_opencv_mat_set src r c v))




(define-public mtfa-opencv::mat-info::CV_8U   0)
(define-public mtfa-opencv::mat-info::CV_8S   1)
(define-public mtfa-opencv::mat-info::CV_16U  2)
(define-public mtfa-opencv::mat-info::CV_16S  3)
(define-public mtfa-opencv::mat-info::CV_32S  4)
(define-public mtfa-opencv::mat-info::CV_32F  5)
(define-public mtfa-opencv::mat-info::CV_64F  6)
(define-public mtfa-opencv::mat-info::CV_15F  7)


;;Il modello fatto con pipeline ed esecuzione ri ptocesso esterno!!!
;;Attiva il detector

(define detector-path "/new_devs/usr/local/shared/bin/")
(define detector-app "detector")

(define* (mtfa-opencv-extrun-detector ;;cfg names weights tcpport isStationary showWindows threshold video/webcam featuresMax classes
	  #:key 
	  (names "coco.names")
	  (cfg "yolov4.cfg")
	  (weights "yolov4.weights")
	  (video 0)
	  (threshold 0.2)
	  (isStationary #t)
	  (callback -1)
	  (windows #t)
	  (classes "all")
	  (height '())
	  (width '())
	  (zed_resolution "HD720")
	  (zed_depth_mode "PERFORMANCE")
	  (zed_camera_ip "")
	  (flow_winsize 15)
	  (flow_iterations 5000)
	  (get_corners #t)
	  (corn_winsize 15)
	  (corn_period 5)
	  (max-count 1000)
	  (corn_qlev 0.01)
	  (corn_mindist 50)
	  (corn_critcount 30)
	  (corn_accuracy 0.03)
	  (corn_3dcoord #f)
	  (corn_3dsquare 20)
	  (manage-data (let ((count 0)) (lambda (json) (display json) (newline) (set! count (1+ count)) (if (> count 100) #t #f))))
	  (help #f)
	 )
  "get the usage calling (mtfa-opencv-extrun-detector #:help #t)"
  (when help
      (display "{names n         |                | file containing names }")(newline)
      (display "{cfg c           |                | file containing the configuration }")(newline)
      (display "{weights w       |                | file containing the weights }")(newline)
      (display "{video v         |                | movie, image, device or camera number }")(newline)
      (display "{threshold t     | 0.2            | threshold of detection }")(newline)
      (display "{isStationary s  | true           | camera is (true/false) stationary }")(newline)
      (display "{callback b      | -1             | port where to send detections }")(newline)
      (display "{windows a       | true           | show windows (true/false) during detection }")(newline)
      (display "{classes         | all            | list of the classes to show(\"class\",\"class\",\"class\",\"class\") or \"none\"}")(newline)
      (display "{height          |                | Height in pixel to resize the frames}")(newline)
      (display "{width           |                | Width in pixel to resize the frames}")(newline)
      (display "{zed_resolution  | HD720          | Zed camera resolution (HD720, HD1080, HD2K, VGA}")(newline)
      (display "{zed_depth_mode  | PERFORMANCE    | Zed camera depth mode (NONE, PERFORMANCE, QUALITY, ULTRA)}")(newline)
      (display "{flow_winsize    | 15             | the size (9, 15, 21, 31) of the SparsePyrLKOpticalFlow algorithm}")(newline)
      (display "{flow_iterations | 8000           | the iterations of the SparsePyrLKOpticalFlow algorithm}")(newline)
      (display "{get_corners     | true           | evaluate good corners in the image}")(newline)
      (display "{corn_winsize    | 10             | good corners win size}")(newline)
      (display "{corn_period     | 5              | good corners taken every 5 frame}") (newline)
      (display "{max-count m     | 1000           | good corners, max number of features to track }")(newline)
      (display "{corn_qlev       | 0.01           | good corners quality level}")(newline)
      (display "{corn_mindist    | 10             | good corners min distance}")(newline)
      (display "{corn_critcount  | 20             | good corners termcriteria count}")(newline)
      (display "{corn_accuracy   | 0.03           | good corners termcriteria accuracy}")(newline)
      (display "{corn_3dcoord    | false          | good corners get 3d coordinates}") (newline)
      (display "{corn_3dsquare   | 20             | good corners the square inside to take 3d coords}") (newline)
      (display "{manage-data     |                | when not given the default is one parameter, stops after 100}")(newline)
      
      (error "Mandatory arguments: names cfg weights and video")
      )
  ;;Converte i parametri in stringhe
  ;; (names "coco.names")
  ;; (cfg "yolov4.cfg")
  ;; (weights "yolov4.weights")
  (set! video (mtfa-to-string video))
  (set! threshold (mtfa-to-string threshold))
  (if isStationary (set! isStationary "true") (set! isStationary "false"))
  (set! callback (mtfa-to-string callback))
  (if windows (set! windows "true") (set! windows "false"))
  ;; (classes "all")
  
  ;;se nil non sono utilizzati
  (unless height (set! height (mtfa-to-string height)))
  (unless width (set! width (mtfa-to-string width)))
  ;; (zed_resolution "HD720")
  ;; (zed_depth_mode "PERFORMANCE")
  (set! flow_winsize (mtfa-to-string flow_winsize))
  (set! flow_iterations (mtfa-to-string flow_iterations))
  (if get_corners (set! get_corners "true") (set! get_corners "false"))
  (set! corn_winsize (mtfa-to-string corn_winsize))
  (set! corn_period (mtfa-to-string corn_period))
  (set! max-count (mtfa-to-string max-count))
  (set! corn_qlev (mtfa-to-string corn_qlev))
  (set! corn_mindist (mtfa-to-string corn_mindist))
  (set! corn_critcount (mtfa-to-string corn_critcount))
  (set! corn_accuracy (mtfa-to-string corn_accuracy))
  (if corn_3dcoord (set! corn_3dcoord "true") (set! corn_3dcoord "false"))
  (set! corn_3dsquare (mtfa-to-string corn_3dsquare))
  
  (let* ((parameters (list
		      (string-append "--names=" names)
		      (string-append "--cfg=" cfg)
		      (string-append "--weights=" weights)
		      (string-append "--video=" video)
		      (string-append "--threshold=" threshold)
		      (string-append "--isStationary=" isStationary)
		      (string-append "--callback=" callback)
		      (string-append "--windows=" windows)
		      (string-append "--classes=" classes)

		      (if (not (null? height)) (string-append "--height=" height) "")
		      (if (not (null? width)) (string-append "--width=" width) "")

		      (string-append "--zed_resolution=" zed_resolution)
		      (string-append "--zed_depth_mode=" zed_depth_mode)
		      (string-append "--flow_winsize=" flow_winsize)
		      (string-append "--flow_iterations=" flow_iterations)
		      (string-append "--get_corners=" get_corners)
		      (string-append "--corn_winsize=" corn_winsize)
		      (string-append "--corn_period=" corn_period)
		      (string-append "--max-count=" max-count)
		      (string-append "--corn_qlev=" corn_qlev)
		      (string-append "--corn_mindist=" corn_mindist)
		      (string-append "--corn_critcount="corn_critcount )
		      (string-append "--corn_accuracy=" corn_accuracy)
		      (string-append "--corn_3dcoord=" corn_3dcoord )
		      (string-append "--corn_3dsquare=" corn_3dsquare )
		      )))
    ;;
    (receive (from-port to-port pids) (pipeline (list (cons (string-append detector-path detector-app) parameters)))
      (Show "La pipeline: " (list (cons (string-append detector-path detector-app) parameters)))
      (Show "PIDS: " pids)
      ;;
      (let loop ((line (read-line from-port)))
	(if (eof-object? line)
	    (begin
	      ;; (display "Got eoln") (newline)
	      #t)
	    (begin
	      (Show line)
	      (if (string=? "server ready" line)
		  (begin
		    ;;Leggo un pochino di comunicazioni e poi termino
		    (receive (answer dataport)
			(http-request (string-append "http://127.0.0.1:" callback)
				      #:method 'GET
				      #:streaming? #t)
		      ;;la prima receive scompare poiché è una "["
		      (read-line dataport)
		      (do ((stop #f)) ((equal? #t stop))
			(let ((s (read-line dataport)))
			  (if (eof-object? s)
			      (begin
				(display "stopped...")
				(set! stop #t))
			      (mtfa-noerr 
			       #f
			       (set! stop (manage-data (json-string->scm (mtfa-replace-all ", *$" s "" #t 0))))))))
		      (close-port dataport))
		    (close-port from-port)
		    (close-port to-port)
		    (kill (car pids) SIGUSR1))
		  (loop (read-line from-port)))))))))



;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;ALBERO NARIO;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define-record-type <ntree>
  (ntree-make data next prev parent children)
  naryt?
  (data ntree-data ntree-data!)
  (next ntree-next ntree-next! )
  (prev ntree-prev ntree-prev! )
  (parent ntree-parent ntree-parent! )
  (children ntree-children ntree-children! )
  )
;;
(define-public (ntree-new data)
  (ntree-make data '() '() '() '()))

(export ntree-data
	ntree-data!
	ntree-next
	ntree-next!
	ntree-prev
	ntree-prev!
	ntree-parent
	ntree-parent!
	ntree-children
	ntree-children! )
;;
;;
(define-public (ntree-copy node)
  ;;Recursively copies a GNode (but does not deep-copy the data inside the nodes, see g_node_copy_deep() if you need that).
  #f
  )
;;
(define-public (ntree-copy-deep node func)
  ;;Recursively copies a GNode and the data inside the nodes.
  #f
  )
;;
(define-public (ntree-insert parent position node)
  ;;queste comunque ci vogliono
  (ntree-parent! node parent)
  ;;(ntree-children! node '())  !!Questa no poiché il nodo potrebbe già avere figli (che sposto con lui)
  (let ((pchi (ntree-children parent)))
    (cond
     ;;
     ;;-1 o dopo last o nessun figlio (appende a last)
     ((or (< position 0) (>= position (length pchi)) (zero? (length pchi)))
      (ntree-children! parent (append pchi (list node)))
      (ntree-next! node '())
      (when (> (length pchi) 0)
	(ntree-prev! node (last pchi))
	(ntree-next! (last pchi) node)))
     ;;
     ;;Esattamente al primo posto di una lista di figli non vuota
     ((= position 0)
      (ntree-next! node (first pchi))
      (ntree-prev! node '())
      (ntree-prev! (first pchi) node)
      (ntree-children! parent (append (list node) pchi)))
     ;;
     ;;Nel bel mezzo della lista dei figli, né al primo, né all'ultimo posto
     (#t
      (let ((prec (list-ref pchi (1- position)))
	    (next (list-ref pchi position)))
	(ntree-next! node next)
	(ntree-prev! node prec)
	(ntree-next! prec node)
	(ntree-prev! next node)
	(ntree-children! parent (append (take pchi position) (list node) (take-right pchi (- (length pchi) position)))))))))

(define-public (ntree-insert-before parent sibling node)
  ;;If sibling is NULL, the node is inserted as the last child of parent
  (cond
   ((null? sibling)
    (ntree-insert parent -1 node))
   (#t (ntree-insert parent (list-index (cut eq? sibling <>) (ntree-children parent)) node))))
;;
(define-public (ntree-insert-after parent sibling node)
  ;;If sibling is NULL, the node is inserted as the last child of parent
  (cond
   ((null? sibling)
    (ntree-insert parent -1 node))
   (#t (ntree-insert parent (1+ (list-index (cut eq? sibling <>) (ntree-children parent))) node))))
;;
(define-public (ntree-append parent node)
  (ntree-insert parent -1 node))
;;
(define-public (ntree-prepend parent node)
  (ntree-insert parent 0 node))
;;
(define-public (ntree-insert-data parent position data)
  (let ((node (ntree-new data)))
    (ntree-insert parent position node)
    node))
;;
(define-public (ntree-insert-data-before parent sibling data)
  (let ((node (ntree-new data)))
    (ntree-insert-before parent sibling node)
    node))
;;
(define-public (ntree-insert-data-after parent sibling data)
  (let ((node (ntree-new data)))
    (ntree-insert-after parent sibling node)
    node))
;;
(define-public (ntree-append-data parent data)
  (let ((node (ntree-new data)))
    (ntree-insert parent -1 node)
    node))
;;
(define-public (ntree-prepend-data parent data)
  (let ((node (ntree-new data)))
    (ntree-insert parent 0 node)
    node))
;;
(define-public (ntree-reverse-children node)
  (ntree-children! node 
		   (map (lambda (it) (let ((old-prev (ntree-prev it)) (old-next (ntree-next it)))
				  (ntree-prev! it old-next)
				  (ntree-next! it old-prev)
				  ;; (Show! "Data: " (ntree-data it)
				  ;; 	 ". Prev: " (if (null? (ntree-prev it)) "()" (ntree-data (ntree-prev it)))
				  ;; 	 ". next: " (if (null? (ntree-next it)) "()" (ntree-data (ntree-next it)))
				  ;; 	 )
				  it))
			     (reverse (ntree-children node)))))
(define-public ntree::TRAVERSE_LEAVES 'TRAVERSE_LEAVES)
(define-public ntree::TRAVERSE_ALL 'TRAVERSE_ALL)
(define-public ntree::TRAVERSE_NON_LEAVES 'TRAVERSE_NON_LEAVES)
(define-public ntree::IN_ORDER 'IN_ORDER)
(define-public ntree::PRE_ORDER 'PRE_ORDER)
(define-public ntree::POST_ORDER 'POST_ORDER)
(define-public ntree::LEVEL_ORDER 'LEVEL_ORDER)
;;
(defun-public ntree-traverse-or-find (root order flags max-depth func)
  (cond
   ((eqv? order 'IN_ORDER)
    (let loop ((r root) (depth 0))
      (if (or (null? r) (and (>= max-depth 0) (>= depth max-depth)))
	  '()
	  (if (null? (ntree-children r))
	      (if (or (eqv? flags 'TRAVERSE_LEAVES)(eqv? flags 'TRAVERSE_ALL))
		  (when (func r depth) (return r))
		  '())
	      ;;visit all children except the last
	      (begin
		(for-each (lambda (it) (loop it (1+ depth)))
			  (drop-right (ntree-children r) 1))
		(when (or (eqv? flags 'TRAVERSE_NON_LEAVES)(eqv? flags 'TRAVERSE_ALL))
		  (when (func r depth) (return r))
		  )
		(loop (last (ntree-children r)) (1+ depth)))))))
   ;;
   ((eqv? order 'PRE_ORDER)
    (let loop ((r root) (depth 0))
      (if (or (null? r) (and (>= max-depth 0) (>= depth max-depth)))
	  '()
	  (if (null? (ntree-children r))
	      (if (or (eqv? flags 'TRAVERSE_LEAVES)(eqv? flags 'TRAVERSE_ALL))
		  (when (func r depth) (return r))
		  '())
	      (begin
		(when  (or (eqv? flags 'TRAVERSE_NON_LEAVES)(eqv? flags 'TRAVERSE_ALL))
		  (when (func r depth) (return r)))
		;;visit all children
		(for-each (lambda (it) (loop it (1+ depth)))
			  (ntree-children r)))))))
   ;;
   ((eqv? order 'POST_ORDER)
    (let loop ((r root) (depth 0))
      (if (or (null? r) (and (>= max-depth 0) (>= depth max-depth)))
	  '()
	  (if (null? (ntree-children r))
	      (if (or (eqv? flags 'TRAVERSE_LEAVES)(eqv? flags 'TRAVERSE_ALL))
		  (when (func r depth) (return r))
		  '())
	      (begin
		(for-each (lambda (it) (loop it (1+ depth)))
			  (ntree-children r))
		(when  (or (eqv? flags 'TRAVERSE_NON_LEAVES)(eqv? flags 'TRAVERSE_ALL))
		  (when (func r depth) (return r))))))))
   ;;
   ((eqv? order 'LEVEL_ORDER)
    (if (null? root)
	'()
	(let loop ((nodes (list root))
		   (depth 0))
	  (if (null? nodes)
	      '()
	      ;;stampo la lista dei nodi attuali, faccio la lista dei figli, stampo la lista dei figli ...
	      (begin
		(for-each (lambda (n)
			    (cond
			     ((eqv? flags 'TRAVERSE_ALL) 
			      (when (func n depth) (return n)))
			     ((and (eqv? flags 'TRAVERSE_LEAVES) (null? (ntree-children n))) (when (func n depth) (return n)))
			     ((and (eqv? flags 'TRAVERSE_NON_LEAVES) (not (null? (ntree-children n)))) (when (func n depth) (return n)))))
			  nodes)
		(loop (append-map (lambda (n) (ntree-children n)) nodes) (1+ depth)))))))))

(document! ntree-traverse-or-find "order:  'IN_ORDER 'PRE_ORDER 'POST_ORDER or 'LEVEL_ORDER.
- G_IN_ORDER visits a node's left child first, then the node itself, then its right child. This is the one to use if you want the output sorted according to the compare function. The inorder traversal of an N-ary tree is defined as visiting all the children except the last then the root and finally the last child recursively
- G_PRE_ORDER visits a node, then its children.
- G_POST_ORDER visits the node's children, then the node itself.
- G_LEVEL_ORDER is not implemented for Balanced Binary Trees. For N-ary Trees, it vists the root node first, then its children, then its grandchildren, and so on. Note that this is less efficient than the other orders.

flags:  'TRAVERSE_ALL 'TRAVERSE_LEAVES and 'TRAVERSE_NON_LEAVES

max-depth: the maximum depth of the traversal. Nodes below this depth will not be visited. If max_depth is -1 all nodes in the tree are visited. If depth is 1, only the root is visited. If depth is 2, the root and its children are visited. And so on.

func: the function called for each node. Parameters: (node deep)
if the function returns #t, the evaluation stops and the node is returned
"
)
(define (i-build-node-list root flags max-depth func) ;;post order, per ora
  (define (nst r depth)
    (if (or (null? r) (and (>= max-depth 0) (>= depth max-depth)))
	'()
	(if (null? (ntree-children r))
	    (begin
	      (if (or (eqv? flags 'TRAVERSE_LEAVES)(eqv? flags 'TRAVERSE_ALL))
		  (cons (func r depth) '())
		  '()))
	    (begin
	      (append 
	       (append-map (lambda (it) (nst it (1+ depth))) (ntree-children r))
	       (if (or (eqv? flags 'TRAVERSE_NON_LEAVES)(eqv? flags 'TRAVERSE_ALL))
		   (list (func r depth))
		   '()))))))
  (nst root 0)
  )
;;
(define-public (ntree-traverse-map root order flags max-depth func)
  "Traverses the nary tree and builds a list with the results of func (node depth) applied to the traversed nodes.
order:  'IN_ORDER 'PRE_ORDER 'POST_ORDER or 'LEVEL_ORDER.
flags:  'TRAVERSE_ALL 'TRAVERSE_LEAVES and 'TRAVERSE_NON_LEAVES"
  (let ((result '()))
    (cond
     ((eqv? order 'IN_ORDER)
      (let loop ((r root) (depth 0))
	(if (or (null? r) (and (>= max-depth 0) (>= depth max-depth)))
	    '()
	    (if (and (null? (ntree-children r))
		   (or (eqv? flags 'TRAVERSE_LEAVES)(eqv? flags 'TRAVERSE_ALL)))
		(mtfa-m-cons (func r depth) result)
		(begin ;;visit all children except the last
		  (for-each (lambda (it) (loop it (1+ depth))) (drop-right (ntree-children r) 1))
		  (when (or (eqv? flags 'TRAVERSE_NON_LEAVES)(eqv? flags 'TRAVERSE_ALL))
		    (mtfa-m-cons (func r depth) result ))
		  (loop (last (ntree-children r)) (1+ depth)))))))
     ;;
     ((eqv? order 'PRE_ORDER)
      (let loop ((r root) (depth 0))
	(if (or (null? r) (and (>= max-depth 0) (>= depth max-depth)))
	    '()
	    (if (and (null? (ntree-children r))
		   (or (eqv? flags 'TRAVERSE_LEAVES)(eqv? flags 'TRAVERSE_ALL)))
		(mtfa-m-cons (func r depth) result)
		(begin
		  (when  (or (eqv? flags 'TRAVERSE_NON_LEAVES)(eqv? flags 'TRAVERSE_ALL))
		    (mtfa-m-cons (func r depth) result))
		  ;;visit all children
		  (for-each (lambda (it) (loop it (1+ depth))) (ntree-children r)))))))
     ;;
     ((eqv? order 'POST_ORDER)
      (set! result (i-build-node-list root flags max-depth func))
      ;; (let loop ((r root) (depth 0))
      ;; 	(if (or (null? r) (and (>= max-depth 0) (>= depth max-depth)))
      ;; 	    '()
      ;; 	    (if (and (null? (ntree-children r))
      ;; 		   (or (eqv? flags 'TRAVERSE_LEAVES)(eqv? flags 'TRAVERSE_ALL)))
      ;; 		(mtfa-m-cons (func r depth) result)
      ;; 		(begin
      ;; 		  (for-each (lambda (it) (loop it (1+ depth))) (ntree-children r))
      ;; 		  (when  (or (eqv? flags 'TRAVERSE_NON_LEAVES)(eqv? flags 'TRAVERSE_ALL))
      ;; 		    (mtfa-m-cons (func r depth) result ))))))
      )
     ;;
     ((eqv? order 'LEVEL_ORDER)
      (if (null? root)
	  '()
	  (let loop ((nodes (list root))
		     (depth 0))
	    (if (null? nodes)
		'()
		;;stampo la lista dei nodi attuali, faccio la lista dei figli, stampo la lista dei figli ...
		(begin
		  (for-each (lambda (n)
			      (cond
			       ((eqv? flags 'TRAVERSE_ALL) 
				(mtfa-m-cons (func n depth) result))
			       ((and (eqv? flags 'TRAVERSE_LEAVES) (null? (ntree-children n)))
				(mtfa-m-cons (func n depth) result))
			       ((and (eqv? flags 'TRAVERSE_NON_LEAVES) (not (null? (ntree-children n))))
				(mtfa-m-cons (func n depth) result))))
			    nodes)
		  (loop (append-map (lambda (n) (ntree-children n)) nodes) (1+ depth))))))))
    result))
;;
(define-public (ntree-traverse-fold root order flags max-depth func init)
  "Traverses the nary tree and builds as the fold function (node depth prec) applied to the traversed nodes.
order:  'IN_ORDER 'PRE_ORDER 'POST_ORDER or 'LEVEL_ORDER.
flags:  'TRAVERSE_ALL 'TRAVERSE_LEAVES and 'TRAVERSE_NON_LEAVES"
  (let ((prec init)
	(lst (ntree-traverse-map root order flags max-depth (lambda (c d) (cons c d)))))
    (fold (lambda (c p)
	    (func (car c) (cdr c) p))
	  init
	  lst)))
;;
(defun-public ntree-children-foreach-or-find (root flags func)
  (for-each (lambda (n)
	      (cond
	       ((eqv? flags 'TRAVERSE_ALL)
		(when (func n) (return n)))
	       ((and (eqv? flags 'TRAVERSE_LEAVES) (null? (ntree-children n))) 
		(when (func n) (return n)))
	       ((and (eqv? flags 'TRAVERSE_NON_LEAVES) (not (null? (ntree-children n))))
		(when (func n) (return n)))))
	    (ntree-children root)))
(document! ntree-children-foreach-or-find "flags:  'TRAVERSE_ALL 'TRAVERSE_LEAVES and 'TRAVERSE_NON_LEAVES
func: the function called for each node. Parameters: (node). If the function returns #t, stops and returns the current child
")
;;
(define-public (ntree-children-map root flags func)
  "Applies func to each child satisfying traversing flags, returns a list"
  (map (lambda (n)
	 (cond
	  ((eqv? flags 'TRAVERSE_ALL) (func n))
	  ((and (eqv? flags 'TRAVERSE_LEAVES) (null? (ntree-children n))) (func n))
	  ((and (eqv? flags 'TRAVERSE_NON_LEAVES) (not (null? (ntree-children n)))) (func n))
	  (#t '())))
       (ntree-children root)))
;;
(define-public (ntree-children-fold root flags func init)
  "Applies func (current prec) to each child satisfying traversing flags, returns the fold"
  (fold (lambda (curr prec)
	 (cond
	  ((eqv? flags 'TRAVERSE_ALL) (func curr prec))
	  ((and (eqv? flags 'TRAVERSE_LEAVES) (null? (ntree-children curr))) (func curr prec))
	  ((and (eqv? flags 'TRAVERSE_NON_LEAVES) (not (null? (ntree-children curr)))) (func curr prec))
	  (#t prec)))
	init
	(ntree-children root)))
;;
(defun-public ntree-parents-foreach-or-find (root func distance)
  (let loop ((root root) (func func) (dist 0))
    (if (or (null? root) (null? root) (> dist distance))
	'()
	(begin
	  (when (func root dist)
	    (return root))
	  (loop (ntree-parent root) func (1+ dist))))))
(document! ntree-parents-foreach-or-find "func: the function called for each node. Parameters: (node distance). If the function returns #t, stops and returns the current ancestor")
;;
(define-public (ntree-parents-map root func distance)
  "Applies func to each ancestor, returns a list"
  (let loop ((root root) (func func) (dist 0))
    (if (or (null? root) (null? root) (> dist distance))
	'()
	(cons (func root dist) (loop (ntree-parent root) func (1+ dist))))))
;;
(define-public (ntree-parents-fold root func init distance)
  "Applies func (current prec dist) to each ancestor until the distance is less than distance"
  (let loop ((root root) (func func) (dist 0)  (prec init))
    (if (or (null? root) (null? root) (> dist distance))
	prec
	(loop (ntree-parent root) func (1+ dist) (func root prec dist)))))
;;
(define-public (ntree-get-root node)
  (if (null? node)
      '()
      (if (null? (ntree-parent node))
	  node
	  (ntree-get-root (ntree-parent node)))))
;;
(define-public (ntree-child-index node)
  "Returns the position of the node in the children list of the parent"
  (if (or (null? node) (null? (ntree-parent node)))
      '()
      (list-index (cut eqv? node <>) (ntree-children (ntree-parent node)))))
;;
(define-public (ntree-first-child node)
  (if (or (null? node) (null? (ntree-children node)))
      '()
      (car (ntree-children node))))
;;
(define-public (ntree-last-child node)
  (if (or (null? node) (null? (ntree-children node)))
      '()
      (last (ntree-children node))))
;;
(define-public (ntree-nth-child node index)
  (if (or (null? node) (null? (ntree-children node)))
      '()
      (list-ref (ntree-children node) index)))
;;
(define-public (ntree-first-sibling node)
  (if (null? node)
      '()
      (if (null? (ntree-prev node))
	  node
	  (ntree-first-sibling (ntree-prev node)))))
;;
(define-public (ntree-last-sibling node)
  (if (null? node)
      '()
      (if (null? (ntree-next node))
	  node
	  (ntree-last-sibling (ntree-next node)))))
;;
(define-public (ntree-next-sibling node)
  (if (null? node)
      '()
      (ntree-next node)))
;;
(define-public (ntree-prev-sibling node)
  (if (null? node)
      '()
      (ntree-prev node)))
;;
(define-public (ntree-is-leaf n)
  (and (not (null? n)) (null? (ntree-children n))))
(define-public (ntree-is-root n)
  (and (not (null? n)) (null? (ntree-parent n))))
;;
(define-public (ntree-depth n)
  (if (null? n)
      0
      (1+ (ntree-depth (ntree-parent n)))))
;;
(define-public (ntree-nodes n flags)
  "The flags are TRAVERSE_ALL, TRAVERSE_LEAVES and TRAVERSE_NON_LEAVES"
  (let ((count 0))
    (ntree-traverse-or-find n 'PRE_ORDER flags -1 (lambda (n d) (set! count (1+ count)) #f))
    count))
;;
(define-public (ntree-n-children n)
  (if (null? n)
      0
      (length (ntree-children n))))
;;
(define-public (ntree-is-ancestor n d)
  (if (or (null? n) (null? d))
      #f
      (if (eqv? n (ntree-parent d))
	  #t
	  (ntree-is-ancestor n (ntree-parent d)))))
;;
(define-public (ntree-max-height n)
  (if (null? n)
      0
      (let ((hmax 0))
	(ntree-traverse-or-find n 'PRE_ORDER 'TRAVERSE_ALL -1 (lambda (n d) (when (< hmax d) (set! hmax d)) #f))
	(1+ hmax))))
;;
(define-public (ntree-unlink n)
  "Torna due valori, la radice del vecchio albero e n stesso. Se n è già radice, torna n e '()"
  (if (null? n)
      (values '() '())
      (if (null? (ntree-parent n))
	  (values n '())
	  (let ((root (ntree-get-root n))
		(parent (ntree-parent n))
		(prev (ntree-prev n))
		(next (ntree-next n)))
	    ;;lo rendo root!
	    (ntree-parent! n '())
	    (ntree-prev! n '())
	    (ntree-next! n '())
	    ;;
	    ;;aggiorno l'albero cui apparteneva
	    (if (and (null? prev) (null? next))
		(ntree-children! parent '()) ;;era figlio unico, lo tolgo
		(if (null? prev)
		    (begin ;;primo dei figli, sposto sul secondo
		      (ntree-children! parent (cdr (ntree-children parent)))
		      (ntree-prev! next '()))
		    (if (null? next)
			(begin ;;Ultimo dei figli
			  (ntree-children! parent (drop-right (ntree-children parent) 1))
			  (ntree-next! prev '()))
			(begin  ;;ha un precedente e un successivo
			  (ntree-children! parent (delv n (ntree-children parent)))
			  (ntree-next! prev next)
			  (ntree-prev! next prev)))))
	    (values root n)))))
;;
;; ;;La purga dei cammini troppo lunghi
;; (define-public (ntree-purge-chains root len)
;;   ;;tutte le catene più lunghe di len sono accorciate! Cioè a partire dalle foglie, risalgo di len e poi punto direttamente alla radice.
;;   ;;Quindi sto ricostruendo completamente l'albero
;;   ;;oppure utilizzo una struttura dati completamente diversa. Una lista di code circolari dove ogni coda circolare è lunga al massimo LEN
;;   )
;;
;;Some test
(define-public (ntree-print n)
  (Show! (map (lambda (i) 
	  (list (ntree-data i) 
		(ifnot (null? (ntree-prev i)) 
		       (ntree-data (ntree-prev i))
		       '())
		(ifnot (null? (ntree-next i))
		       (ntree-data (ntree-next i))
		       '())))
	(ntree-children n))))
(define-public (ntree-print-all n)
  (ntree-traverse-or-find n 'LEVEL_ORDER 'TRAVERSE_ALL -1 (lambda (n d) (display (ntree-data n)) (display " ") #f)) (newline))
;;
;;facciamo un albero casuale di grado massimo d e con n elementi
(define-public (ntree-build degree nodes)
  ;;per ogni nodo, in modo ricorsivo, fa la costruzione al contrario
  (let ((idx -1))
    (let loop ((degree degree) (nodes nodes))
      (if (> nodes 1)
	  (let ((nch (max 1 (min nodes (remainder (mtfa-rand-ui) degree))))
		(root (ntree-new (mtfa-m-add idx))))
	    ;;ho deciso quanti figli sono. Ora faccio i figli e li collego
	    (do ((i 0 (1+ i))) ((> i nch))
	      (let ((ch (loop degree (/ nodes (1+ nch)))))
		(if (not (null? ch))
		    (ntree-insert root -1 ch))))
	    root)
	  (ntree-new (mtfa-m-add idx))))))
;;

(define (ntree-test)
  (define root (ntree-new 0))
  (define ch1 (ntree-new 1))
  (define ch2 (ntree-new 2))
  (define ch3 (ntree-new 3))
  (define ch4 (ntree-new 4))
  (define ch5 (ntree-new 5))
  (define ch6 (ntree-new 6))
  (define ch7 (ntree-new 7))
  (define ch8 (ntree-new 8))
  (define ch9 (ntree-new 9))
  (define ch10 (ntree-new 10))
  (define ch11 (ntree-new 11))
  (define ch12 (ntree-new 12))
  (define ch13 (ntree-new 13))
#|
                     0
           1         2        3
         4  5       6     7  8  9  10
             11   12            13
|#
  (ntree-insert root -1 ch1)
  (ntree-insert root -1 ch2)
  (ntree-insert root -1 ch3)
  
  (ntree-insert ch1 -1 ch4)
  (ntree-insert ch1 -1 ch5)
  (ntree-insert ch2 -1 ch6)
  (ntree-insert ch3 -1 ch7)
  (ntree-insert ch3 -1 ch8)
  (ntree-insert ch3 -1 ch9)
  (ntree-insert ch3 -1 ch10)
  (ntree-insert ch5 -1 ch11)
  (ntree-insert ch6 -1 ch12)
  (ntree-insert ch9 -1 ch13)
  
  ;; (ntree-insert root -1 ch4)
  ;; (print root)
  ;; (display "(ntree-insert root 2 ch5)\n")
  ;; (ntree-insert root 2 ch5)
  ;; (print root)
  ;; (display "(ntree-insert root 1 ch6)\n")
  ;; (ntree-insert root 1 ch6)
  ;; (print root)
  ;; (display "(ntree-insert root 5 ch7)\n")
  ;; (ntree-insert root 5 ch7)
  ;; (print root)
  ;; (display "(ntree-insert root 0 ch8)\n")
  ;; (ntree-insert root 0 ch8)
  ;; (print root)
  ;; (display "(ntree-insert-before root ch5 ch9)\n")
  ;; (ntree-insert-before root ch5 ch9)
  ;; (print root)
  ;; (display "(ntree-insert-after root ch5 ch10)\n")
  ;; (ntree-insert-after root ch5 ch10)
  ;; (print root)
  ;; (display "(ntree-reverse-children root)\n")
  ;; (ntree-reverse-children root)
  (ntree-print root)

  )

