#GUILE_INCLUDES=/new_devs/usr/local/guile-new/include/guile/3.0/
GUILE_INCLUDES=/usr/include/guile/3.0/
GUILE_LIBS=guile-3.0
#GUILE_LIBS_DIR=-L/new_devs/usr/local/guile-new/lib
GUILE_LIBS_DIR=

#OTHER_INCLUDES=-I/new_devs/usr/local/shared/include -I/new_devs/usr/local/shared/include/openssl  -I/new_devs/usr/local/shared/include/curl -I/volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/websocketpp -I/volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/rtrees/phtree-cpp/phtree -I/usr/include/mysql

OTHER_INCLUDES=-I/new_devs/usr/local/shared/include -I/volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/rtrees/phtree-cpp/phtree -I/usr/include/mysql
# -I/volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/websocketpp

#OTHER_LIBS=-L/new_devs/usr/local/shared/lib -L /volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/rtrees/phtree-cpp/build/phtree/
OTHER_LIBS=-L/new_devs/usr/local/shared/lib -L /volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/rtrees/phtree-cpp/build/phtree/

INCLUDES=$(OTHER_INCLUDES) -I/volume1/sources/NEW_DEVS/LISP/mtfa_libraries \
	  -I/volume1/sources/NEW_DEVS/LISP/onvif_libraries \
	  -I/volume1/sources/NEW_DEVS/LISP/va \
	  -I. \
	  -I$(GUILE_INCLUDES) \
	  -I /volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/LibInterpolate/build/include/ \
	  -I /volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/LibInterpolate/src/ \
	  -I /usr/include/eigen3 \
	 $(BDWGC_INCLUDE)

LIBS=   -L/volume1/sources/NEW_DEVS/LISP/mtfa_libraries/ -lmtfa \
	$(OTHER_LIBS) \
	-lcurl -lgmp -lgmpxx -lsqlite3 \
        -lssl -lcrypto -levent -L/usr/lib/x86_64-linux-gnu \
	-levent_core -levent_extra -levent_pthreads \
	-levent_openssl \
	-lpcre2-8 \
	$(GUILE_LIBS_DIR) -l$(GUILE_LIBS) -luchardet -lmagic \
	-ltre \
	-lrlpvalue -lunivalue -lsecp256k1 \
	-lphtree \
	-lX11 \
	-lmysqlclient \
	-lz -ldl -lpthread -lreadline -lcurses -ltermcap -lrt

	#-lplotter
	#-lnfnetlink -lnetfilter_queue


CPP=g++
CXX=g++
CC=gcc
LD=g++

ifdef RELEASE
	OPT= -pthread -g0 -O3 -fpermissive
	#-fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free 
else
	OPT= -pthread -g -O0 -fpermissive -Wunused
	#-fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free 
endif

CPPFLAGS= -Wmissing-declarations

CXXFLAGS=$(CPPFLAGS)  -std=c++17

LDFLAGS=-shared $(OPT)

all: mtfa_ph.so mtfa_fs3.so mtfa_fs3s.so mtfa_lib.so mtfa_db_mysql.so mtfa_su.so mtfa_cipher.so mtfa_star.so mtfa_brg.so mtfa_string_sequences.so mtfa_opencv.so mtfa_onvif.so
#mtfa_ws.so #mtfa_nary.so
	sudo mkdir -p /new_devs/usr/local/mtfa-guile-libs/mtfa/
	sudo /bin/cp -f *.so /new_devs/usr/local/mtfa-guile-libs/mtfa/

#mtfa_darknet.so
#mtfa_cimg.so mtfa_gfx.so
#mtfa_va.so
#sudo mkdir -p /new_devs/usr/local/mtfa-guile-libs/mtfa

#mtfa_netfilter_queue.so 
#mtfa_va.so: mtfa_va.o $(OBJS)
#	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_va.so mtfa_va.o $(LIBS) 

mtfa_brg.so: mtfa_brg.o $(OBJS)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_brg.so mtfa_brg.o $(LIBS) 

mtfa_db_mysql.so: mtfa_db_mysql.o $(OBJS)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_db_mysql.so mtfa_db_mysql.o $(LIBS) 

mtfa_cipher.so: mtfa_cipher.o $(OBJS)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_cipher.so mtfa_cipher.o $(LIBS) 

mtfa_ph.so: mtfa_ph.o $(OBJS)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_ph.so mtfa_ph.o $(LIBS) 

mtfa_fs3s.so: mtfa_fs3s.o $(OBJS)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_fs3s.so mtfa_fs3s.o $(LIBS) 

mtfa_fs3.so: mtfa_fs3.o $(OBJS)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_fs3.so mtfa_fs3.o $(LIBS) 

mtfa_lib.so: mtfa_lib.o $(OBJS)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_lib.so mtfa_lib.o $(LIBS) 

#mtfa_netfilter_queue.so: mtfa_netfilter_queue.o $(OBJS)
#	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_netfilter_queue.so mtfa_netfilter_queue.o $(LIBS) 

mtfa_su.so: mtfa_su.o $(OBJS)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_su.so mtfa_su.o $(LIBS) 

mtfa_star.so: mtfa_star.o $(OBJS)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_star.so mtfa_star.o $(LIBS) 

# mtfa_ws.so: mtfa_ws.o $(OBJS)
# 	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_ws.so mtfa_ws.o $(LIBS) 

#mtfa_cimg.so: mtfa_cimg.o $(OBJS)
#	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_cimg.so mtfa_cimg.o $(LIBS) 
#
#mtfa_gfx.so: mtfa_gfx.o $(OBJS)
#	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_gfx.so mtfa_gfx.o $(LIBS) 

mtfa_string_sequences.so: mtfa_string_sequences.o $(OBJS)
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_string_sequences.so mtfa_string_sequences.o $(LIBS) 

mtfa_opencv.o: mtfa_opencv.cpp
	$(CXX) $(OPT) -fPIC -c $(CXXFLAGS) $(INCLUDES) -I/usr/local/include/opencv4/ -I/usr/local/cuda/include -I/usr/local/zed/include mtfa_opencv.cpp -o mtfa_opencv.o
	# $(CXX) $(OPT) -fPIC -c $(CXXFLAGS) $(INCLUDES) -I/new_devs/usr/local/opencv/include/opencv4/ -I/usr/local/cuda/include -I/usr/local/zed/include mtfa_opencv.cpp -o mtfa_opencv.o

mtfa_opencv.so: mtfa_opencv.o
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_opencv.so mtfa_opencv.o $(LIBS) \
		-L/usr/local/lib -lopencv_gapi -lopencv_stitching -lopencv_alphamat -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_cudabgsegm -lopencv_cudafeatures2d -lopencv_cudaobjdetect -lopencv_cudastereo -lopencv_dnn_objdetect -lopencv_dnn_superres -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hfs -lopencv_img_hash -lopencv_intensity_transform -lopencv_line_descriptor -lopencv_mcc -lopencv_quality -lopencv_rapid -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_signal -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_superres -lopencv_cudacodec -lopencv_surface_matching -lopencv_tracking -lopencv_highgui -lopencv_datasets -lopencv_text -lopencv_plot -lopencv_videostab -lopencv_cudaoptflow -lopencv_optflow -lopencv_cudalegacy -lopencv_videoio -lopencv_cudawarping -lopencv_wechat_qrcode -lopencv_xfeatures2d -lopencv_shape -lopencv_ml -lopencv_ximgproc -lopencv_video -lopencv_xobjdetect -lopencv_objdetect -lopencv_calib3d -lopencv_imgcodecs -lopencv_features2d -lopencv_dnn -lopencv_flann -lopencv_xphoto -lopencv_photo -lopencv_cudaimgproc -lopencv_cudafilters -lopencv_imgproc -lopencv_cudaarithm -lopencv_core -lopencv_cudev \
 -lm -lpthread -lcudart_static -ldl -lrt -lnppc -lnppial -lnppicc -lnppidei -lnppif -lnppig -lnppim -lnppist -lnppisu -lnppitc -lnpps -lcublas -lcudnn -lcufft \
	-L/usr/local/cuda/lib64 -lcuda \
	-L/usr/local/zed/lib -lsl_zed \
	-L/new_devs/usr/local/shared/lib -lfmt -lX11 -lrt -lm -lpthread
#-L/new_devs/usr/local/shared/lib -lfmt -ldarknet -lX11 -lrt -lm -lpthread
# -lopencv_cvv -lopencv_hdf -lopencv_viz 

mtfa_onvif.o: mtfa_onvif.cpp
	$(CXX) $(OPT) -fPIC -c -I/volume1/sources/NEW_DEVS/LISP/onvif_libraries -I /new_devs/usr/local/gsoap/include -I. -I /new_devs/usr/local/gsoap/share/gsoap/plugin -I /new_devs/usr/local/gsoap/share/gsoap/custom -I$(GUILE_INCLUDES) -fpermissive mtfa_onvif.cpp -o mtfa_onvif.o

mtfa_onvif.so: mtfa_onvif.o
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_onvif.so mtfa_onvif.o $(LIBS) \
	-L/volume1/sources/NEW_DEVS/LISP/onvif_libraries -lonvif \
	$(GUILE_LIBS_DIR) -l$(GUILE_LIBS) \
	-L/new_devs/usr/local/gsoap/lib/ -lgsoapssl++ -lgsoapck++ -lgsoap++ \
	-lfmt -lrt -lm -lpthread -lcrypto -lssl
	

#mtfa_nary.o: mtfa_nary.cpp
#	$(CXX) $(OPT) -fPIC -c $(CXXFLAGS) $(INCLUDES) -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include mtfa_nary.cpp -o mtfa_nary.o
#
#mtfa_nary.so: mtfa_nary.o
#	$(LD) $(LDFLAGS) $(CXXFLAGS) $(OPT) -fPIC -o mtfa_nary.so mtfa_nary.o $(LIBS) -lglib-2.0
	
.cpp.o: $(INCLUDES)
	$(CXX) $(OPT) -fPIC -c $(CXXFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f *.o *.so
