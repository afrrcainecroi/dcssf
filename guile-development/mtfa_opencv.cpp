#include<iostream>
#include<string>
#include<algorithm>
#include<list>
#include<stdexcept>
#include <libguile.h>

#include<boost/program_options.hpp>

#include <libInterpolate/libInterpolate_version.h>
#include <libInterpolate/Interpolate.hpp>
#include <libInterpolate/AnyInterpolator.hpp>
#include <libInterpolate/Utils/ReadFunction.hpp>

using namespace std;

typedef struct __my_interpolator_type__ {
    int itype; //0..5
    void * interpolator;
} T_My_InterpolatorType;

static void releaseInterpolator (void * i) {
    switch (((T_My_InterpolatorType*)i)->itype) {
        case 0: { delete ( _2D::ThinPlateSplineInterpolator<double>* ) ((T_My_InterpolatorType*)i)->interpolator;}
        break;
        case 1: { delete ( _2D::BicubicInterpolator<double>* ) ((T_My_InterpolatorType*)i)->interpolator;}
        break;
        case 2: { delete ( _2D::BilinearInterpolator<double>* ) ((T_My_InterpolatorType*)i)->interpolator;}
        break;
        case 3: { delete ( _1D::CubicSplineInterpolator<double>* ) ((T_My_InterpolatorType*)i)->interpolator;}
        break;
        case 4: { delete ( _1D::LinearInterpolator<double>* ) ((T_My_InterpolatorType*)i)->interpolator;}
        break;
        case 5: { delete ( _1D::MonotonicInterpolator<double>* ) ((T_My_InterpolatorType*)i)->interpolator;}
        break;
        default:
            ; //nop
    }
}

static SCM mtfa_opencv_make_interpolator(SCM scm_type) {
    int itype = scm_to_int(scm_type);
    auto i = new T_My_InterpolatorType();
    i->itype = itype;
    switch (itype)
    {
        case 0: {
            i->interpolator = (void*)new _2D::ThinPlateSplineInterpolator<double>();
            return scm_from_pointer ( (void*)i, releaseInterpolator);
        };
        case 1: {
            i->interpolator = (void*)new _2D::BicubicInterpolator<double>();
            return scm_from_pointer ( (void*)i, releaseInterpolator);
        };
        case 2: {
            i->interpolator = (void*)new _2D::BilinearInterpolator<double>();
            return scm_from_pointer ( (void*)i, releaseInterpolator);
        };
        case 3: {
            i->interpolator = (void*)new _1D::CubicSplineInterpolator<double>();
            return scm_from_pointer ( (void*)i, releaseInterpolator);
        };
        case 4: {
            i->interpolator = (void*)new _1D::LinearInterpolator<double>();
            return scm_from_pointer ( (void*)i, releaseInterpolator);
        };
        case 5: {
            i->interpolator = (void*)new _1D::MonotonicInterpolator<double>();
            return scm_from_pointer ( (void*)i, releaseInterpolator);
        };
        default:
            delete i;
            return SCM_EOL;
    }
}

static SCM mtfa_opencv_interpolator_add_serie(SCM scm_interpolator, SCM scm_list_of_data_series) {
    T_My_InterpolatorType * interpolator = (T_My_InterpolatorType*)scm_to_pointer(scm_interpolator);
    try {
        switch (interpolator->itype) {
            case 0: {
                _2D::ThinPlateSplineInterpolator<double>* i = (_2D::ThinPlateSplineInterpolator<double>*)interpolator->interpolator;
                //Qui sono liste che contengono serie x,y,z
                if (scm_to_int(scm_length(scm_list_of_data_series)) != 3)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_add_serie", 0, scm_list_of_data_series, "The serie must be ((x ...)(y ...)(z ...))" );
                vector<double> x, y, z;
                SCM lx, ly, lz;
                lx = scm_car(scm_list_of_data_series);
                ly = scm_cadr(scm_list_of_data_series);
                lz = scm_caddr(scm_list_of_data_series);
                while (lx != SCM_EOL) {
                    x.push_back(scm_to_double(scm_car(lx)));
                    lx = scm_cdr(lx);
                }
                while (ly != SCM_EOL) {
                    y.push_back(scm_to_double(scm_car(ly)));
                    ly = scm_cdr(ly);
                }
                while (lz != SCM_EOL) {
                    z.push_back(scm_to_double(scm_car(lz)));
                    lz = scm_cdr(lz);
                }
                i->setData(x, y, z);
                return SCM_BOOL_T;
            }
            break;
            case 1: {
                _2D::BilinearInterpolator<double>* i = (_2D::BilinearInterpolator<double>*)interpolator->interpolator;
                //Qui sono liste che contengono serie x,y,z
                if (scm_to_int(scm_length(scm_list_of_data_series)) != 3)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_add_serie", 0, scm_list_of_data_series, "The serie must be ((x ...)(y ...)(z ...))" );
                vector<double> x, y, z;
                SCM lx, ly, lz;
                lx = scm_car(scm_list_of_data_series);
                ly = scm_cadr(scm_list_of_data_series);
                lz = scm_caddr(scm_list_of_data_series);
                while (lx != SCM_EOL) {
                    x.push_back(scm_to_double(scm_car(lx)));
                    lx = scm_cdr(lx);
                }
                while (ly != SCM_EOL) {
                    y.push_back(scm_to_double(scm_car(ly)));
                    ly = scm_cdr(ly);
                }
                while (lz != SCM_EOL) {
                    z.push_back(scm_to_double(scm_car(lz)));
                    lz = scm_cdr(lz);
                }
                i->setData(x, y, z);
                return SCM_BOOL_T;
            }
            break;
            case 2: {
                _2D::BicubicInterpolator<double>* i = (_2D::BicubicInterpolator<double>*)interpolator->interpolator;
                //Qui sono liste che contengono serie x,y,z
                if (scm_to_int(scm_length(scm_list_of_data_series)) != 3)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_add_serie", 0, scm_list_of_data_series, "The serie must be ((x ...)(y ...)(z ...))" );
                vector<double> x, y, z;
                SCM lx, ly, lz;
                lx = scm_car(scm_list_of_data_series);
                ly = scm_cadr(scm_list_of_data_series);
                lz = scm_caddr(scm_list_of_data_series);
                while (lx != SCM_EOL) {
                    x.push_back(scm_to_double(scm_car(lx)));
                    lx = scm_cdr(lx);
                }
                while (ly != SCM_EOL) {
                    y.push_back(scm_to_double(scm_car(ly)));
                    ly = scm_cdr(ly);
                }
                while (lz != SCM_EOL) {
                    z.push_back(scm_to_double(scm_car(lz)));
                    lz = scm_cdr(lz);
                }
                i->setData(x, y, z);
                return SCM_BOOL_T;
            }
            break;
            case 3: {
                _1D::CubicSplineInterpolator<double>* i = (_1D::CubicSplineInterpolator<double>*)interpolator->interpolator;
                //Qui sono liste che contengono serie x,y
                if (scm_to_int(scm_length(scm_list_of_data_series)) != 2)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_add_serie", 0, scm_list_of_data_series, "The serie must be ((x ...)(y ...))" );
                vector<double> x, y;
                SCM lx, ly;
                lx = scm_car(scm_list_of_data_series);
                ly = scm_cadr(scm_list_of_data_series);
                while (lx != SCM_EOL) {
                    x.push_back(scm_to_double(scm_car(lx)));
                    lx = scm_cdr(lx);
                }
                while (ly != SCM_EOL) {
                    y.push_back(scm_to_double(scm_car(ly)));
                    ly = scm_cdr(ly);
                }
                i->setData(x, y);
                return SCM_BOOL_T;
            }
            break;
            case 4: {
                _1D::LinearInterpolator<double>* i = (_1D::LinearInterpolator<double>*)interpolator->interpolator;
                //Qui sono liste che contengono serie x,y
                if (scm_to_int(scm_length(scm_list_of_data_series)) != 2)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_add_serie", 0, scm_list_of_data_series, "The serie must be ((x ...)(y ...))" );
                vector<double> x, y;
                SCM lx, ly;
                lx = scm_car(scm_list_of_data_series);
                ly = scm_cadr(scm_list_of_data_series);
                while (lx != SCM_EOL) {
                    x.push_back(scm_to_double(scm_car(lx)));
                    lx = scm_cdr(lx);
                }
                while (ly != SCM_EOL) {
                    y.push_back(scm_to_double(scm_car(ly)));
                    ly = scm_cdr(ly);
                }
                i->setData(x, y);
                return SCM_BOOL_T;
            }
            break;
            case 5: {
                _1D::MonotonicInterpolator<double>* i = (_1D::MonotonicInterpolator<double>*)interpolator->interpolator;
                //Qui sono liste che contengono serie x,y
                if (scm_to_int(scm_length(scm_list_of_data_series)) != 2)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_add_serie", 0, scm_list_of_data_series, "The serie must be ((x ...)(y ...))" );
                vector<double> x, y;
                SCM lx, ly;
                lx = scm_car(scm_list_of_data_series);
                ly = scm_cadr(scm_list_of_data_series);
                while (lx != SCM_EOL) {
                    x.push_back(scm_to_double(scm_car(lx)));
                    lx = scm_cdr(lx);
                }
                while (ly != SCM_EOL) {
                    y.push_back(scm_to_double(scm_car(ly)));
                    ly = scm_cdr(ly);
                }
                i->setData(x, y);
                return SCM_BOOL_T;
            }
            break;
            default:
                return SCM_EOL;
        }
    } catch (std::exception ex) {
        cerr << ex.what() << endl;
        return SCM_EOL;
    }
}

static SCM mtfa_opencv_interpolator_interpolate(SCM scm_interpolator, SCM scm_list_of_coordinates) {
    T_My_InterpolatorType * interpolator = (T_My_InterpolatorType*)scm_to_pointer(scm_interpolator);
    try {
        switch (interpolator->itype) {
            case 0: {
                _2D::ThinPlateSplineInterpolator<double>* i = (_2D::ThinPlateSplineInterpolator<double>*)interpolator->interpolator;
                //Qui la lista di coordinate
                if (scm_to_int(scm_length(scm_list_of_coordinates)) != 2)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_interpolate", 0, scm_list_of_coordinates, "The list must contain (x y)" );
                auto val = (*i)(scm_to_double(scm_car(scm_list_of_coordinates)), scm_to_double(scm_cadr(scm_list_of_coordinates)));
                return scm_from_double(val);
            }
            break;
            case 1: {
                _2D::BilinearInterpolator<double>* i = (_2D::BilinearInterpolator<double>*)interpolator->interpolator;
                //Qui la lista di coordinate
                if (scm_to_int(scm_length(scm_list_of_coordinates)) != 2)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_interpolate", 0, scm_list_of_coordinates, "The list must contain (x y)" );
                auto val = (*i)(scm_to_double(scm_car(scm_list_of_coordinates)), scm_to_double(scm_cadr(scm_list_of_coordinates)));
                return scm_from_double(val);
            }
            break;
            case 2: {
                _2D::BicubicInterpolator<double>* i = (_2D::BicubicInterpolator<double>*)interpolator->interpolator;
                //Qui la lista di coordinate
                if (scm_to_int(scm_length(scm_list_of_coordinates)) != 2)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_interpolate", 0, scm_list_of_coordinates, "The list must contain (x y)" );
                auto val = (*i)(scm_to_double(scm_car(scm_list_of_coordinates)), scm_to_double(scm_cadr(scm_list_of_coordinates)));
                return scm_from_double(val);
            }
            break;
            case 3: {
                _1D::CubicSplineInterpolator<double>* i = (_1D::CubicSplineInterpolator<double>*)interpolator->interpolator;
                //Qui la lista di coordinate
                if (scm_to_int(scm_length(scm_list_of_coordinates)) != 1)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_interpolate", 0, scm_list_of_coordinates, "The list must contain (x)" );
                auto val = (*i)(scm_to_double(scm_car(scm_list_of_coordinates)));
                return scm_from_double(val);
            }
            break;
            case 4: {
                _1D::LinearInterpolator<double>* i = (_1D::LinearInterpolator<double>*)interpolator->interpolator;
                //Qui la lista di coordinate
                if (scm_to_int(scm_length(scm_list_of_coordinates)) != 1)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_interpolate", 0, scm_list_of_coordinates, "The list must contain (x)" );
                auto val = (*i)(scm_to_double(scm_car(scm_list_of_coordinates)));
                return scm_from_double(val);
            }
            break;
            case 5: {
                _1D::MonotonicInterpolator<double>* i = (_1D::MonotonicInterpolator<double>*)interpolator->interpolator;
                //Qui la lista di coordinate
                if (scm_to_int(scm_length(scm_list_of_coordinates)) != 1)
                    scm_wrong_type_arg_msg ( "mtfa_opencv_interpolator_interpolate", 0, scm_list_of_coordinates, "The list must contain (x)" );
                auto val = (*i)(scm_to_double(scm_car(scm_list_of_coordinates)));
                return scm_from_double(val);
            }
            break;
            default:
                return SCM_EOL;
        }
    } catch (std::exception ex) {
        cerr << ex.what() << endl;
        return SCM_EOL;
    }
}


#include <iostream>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <fstream>
#include <thread>
#include <future>
#include <atomic>
#include <mutex>         // std::mutex, std::unique_lock
#include <cmath>
#include <unistd.h>
#include <stdlib.h>
#include <csignal>
#include <unordered_set>
#include <fmt/format.h>
#include <boost/range/combine.hpp>
#include <vector>
#include <iostream>
#include <numeric>
#include <chrono>
#include <algorithm>
#include <stdexcept>

#include <mtfa_find.h>

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <list>

#include <opencv2/opencv.hpp>
#include <opencv2/core/version.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/core.hpp>


#include <opencv2/core/ocl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/img_hash.hpp>

#include <opencv2/core/utility.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaoptflow.hpp>
#include <opencv2/cudabgsegm.hpp>
#include <opencv2/cudafilters.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/cudaarithm.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

#include <sl/Camera.hpp>

using namespace cv;
using namespace cv::img_hash;
using namespace std;
using namespace dnn;


static string ScmToString ( SCM s )
{
    string sp;
    size_t len;
    char * p = scm_to_locale_stringn ( s, &len );
    sp.assign ( p, p+len );
    free ( p );
    return sp;
}
static int S2I ( SCM s )
{
    return scm_to_int ( s );
}
static int S2D ( SCM s )
{
    return scm_to_double( s );
}

// static std::string SToLower(const std::string & s)
// {
//     std::string ss(s);
//     transform(ss.begin(), ss.end(), ss.begin(), ::tolower);
//     return ss;
// }
static std::string SToUpper(const std::string & s)
{
    std::string ss(s);
    transform(ss.begin(), ss.end(), ss.begin(), ::toupper);
    return ss;
}

static cv::Mat * mtfa_internal_opencv_clone ( cv::Mat m )
{
    cv::Mat * out = new cv::Mat(m.rows, m.cols, m.type());
    m.copyTo(*out);
    return out;
}

static cv::Mat * mtfa_internal_opencv_clone ( cv::Mat * m )
{
    cv::Mat * out = new cv::Mat(m->rows, m->cols, m->type())  ;
    m->copyTo(*out);
    return out;
}

static void releaseMat ( void * mat )
{
    if (mat) {
        delete ( cv::Mat* ) mat;
    }
}

static void releaseVideoCapture ( void * cap )
{
    if (cap)
    {
        delete ( cv::VideoCapture* ) cap;
    }
//     if (cap && ((cv::VideoCapture* )cap)->isOpened())
//     {
//         ((cv::VideoCapture* )cap)->release();
//         delete ( cv::VideoCapture* ) cap;
//     }
}

static void releaseVideoWriter ( void * put)
{
    if (put)
    {
        delete ( cv::VideoWriter* ) put;
    }
//     if (put && ((cv::VideoWriter* )put)->isOpened())
//     {
//         ((cv::VideoWriter* )put)->release();
//         delete ( cv::VideoWriter* ) put;
//     }
}
//#define USE_OLD_DETECTOR

#ifndef USE_OLD_DETECTOR

#include <sl/Camera.hpp>
#include <X11/Xlib.h>
#pragma comment(lib, "sl_zed64.lib")



// Mapping between MAT_TYPE and CV_TYPE
int Zed_getOCVtype(sl::MAT_TYPE type);
int Zed_getOCVtype(sl::MAT_TYPE type) {
    int cv_type = -1;
    switch (type) {
    case sl::MAT_TYPE::F32_C1:
        cv_type = CV_32FC1;
        break;
    case sl::MAT_TYPE::F32_C2:
        cv_type = CV_32FC2;
        break;
    case sl::MAT_TYPE::F32_C3:
        cv_type = CV_32FC3;
        break;
    case sl::MAT_TYPE::F32_C4:
        cv_type = CV_32FC4;
        break;
    case sl::MAT_TYPE::U8_C1:
        cv_type = CV_8UC1;
        break;
    case sl::MAT_TYPE::U8_C2:
        cv_type = CV_8UC2;
        break;
    case sl::MAT_TYPE::U8_C3:
        cv_type = CV_8UC3;
        break;
    case sl::MAT_TYPE::U8_C4:
        cv_type = CV_8UC4;
        break;
    default:
        break;
    }
    return cv_type;
}



/**
* Conversion function between sl::Mat and cv::Mat
**/
cv::Mat Zed_slMat2cvMat(const sl::Mat& input);
cv::Mat Zed_slMat2cvMat(const sl::Mat& input) {
    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), Zed_getOCVtype(input.getDataType()), input.getPtr<sl::uchar1>(sl::MEM::CPU), input.getStepBytes(sl::MEM::CPU));
}

/**
* Conversion function between sl::Mat and cv::Mat
**/
cv::cuda::GpuMat Zed_slMat2cvMatGPU(const sl::Mat& input);
cv::cuda::GpuMat Zed_slMat2cvMatGPU(const sl::Mat& input) {
    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::cuda::GpuMat(input.getHeight(), input.getWidth(), Zed_getOCVtype(input.getDataType()), input.getPtr<sl::uchar1>(sl::MEM::GPU), input.getStepBytes(sl::MEM::GPU));
}

inline cv::Mat slMat2cvMat(const sl::Mat& input) {
    // Mapping between MAT_TYPE and CV_TYPE
    int cv_type = -1;
    switch (input.getDataType()) {
    case sl::MAT_TYPE::F32_C1:
        cv_type = CV_32FC1;
        break;
    case sl::MAT_TYPE::F32_C2:
        cv_type = CV_32FC2;
        break;
    case sl::MAT_TYPE::F32_C3:
        cv_type = CV_32FC3;
        break;
    case sl::MAT_TYPE::F32_C4:
        cv_type = CV_32FC4;
        break;
    case sl::MAT_TYPE::U8_C1:
        cv_type = CV_8UC1;
        break;
    case sl::MAT_TYPE::U8_C2:
        cv_type = CV_8UC2;
        break;
    case sl::MAT_TYPE::U8_C3:
        cv_type = CV_8UC3;
        break;
    case sl::MAT_TYPE::U8_C4:
        cv_type = CV_8UC4;
        break;
    default:
        break;
    }

//     cv::Mat bigframe_cvBGR;
//     cv::cvtColor(bigframe_cv,bigframe_cvBGR,cv::COLOR_RGB2BGR);

    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(sl::MEM::CPU));
}

cv::Mat zed_capture_rgb ( sl::Camera &zed );
cv::Mat zed_capture_rgb ( sl::Camera &zed ) {
    sl::Mat left;
//     if (zed.grab() == sl::ERROR_CODE::SUCCESS) {
    if (zed.retrieveImage ( left ) == sl::ERROR_CODE::SUCCESS)
    {
        cv::Mat left_rgb;
        cv::cvtColor ( slMat2cvMat ( left ), left_rgb, cv::COLOR_RGBA2RGB); //CV_RGBA2RGB );
        return left_rgb;
    }
    else
    {
        return cv::Mat();
    }
}

cv::Mat zed_capture_3d_orig(sl::Camera &zed);
cv::Mat zed_capture_3d_orig(sl::Camera &zed) {
    sl::Mat cur_cloud;
    zed.retrieveMeasure(cur_cloud, sl::MEASURE::XYZ);
    return slMat2cvMat(cur_cloud).clone();
}


/*cv::Mat */
vector<vector<float>> zed_capture_3d ( sl::Camera &zed );
vector<vector<float>> zed_capture_3d ( sl::Camera &zed )
{
    sl::Mat cur_cloud;
    //zed.retrieveMeasure ( cur_cloud, sl::MEASURE::XYZ);
    zed.retrieveMeasure ( cur_cloud, sl::MEASURE::DEPTH);
    vector<vector<float>> mat(cur_cloud.getHeight(), vector<float>(cur_cloud.getWidth()));
    const float * ptr = cur_cloud.getPtr<float>();
    for (int y=0; y<cur_cloud.getHeight(); ++y)
        std::copy(ptr+y*cur_cloud.getWidth(), ptr+(1+y)*cur_cloud.getWidth(), mat[y].begin());
    return mat;
    /*
        for (int y=0; y<cur_cloud.getHeight(); ++y)
        {
            for (int x=0; x<cur_cloud.getWidth(); ++x)
            {
                float v;
                cur_cloud.getValue<float>(x, y, &v);
                printf("%4d, %4d: %f - %f\n", x, y, mat[y][x], v);
            }
        }
        getchar();
        return slMat2cvMat ( cur_cloud ).clone();*/
}


#else

//Old detector, ora usare, quando sarà il caso, yolo8 e succ
// It makes sense only for video-Camera (not for video-File)
// To use - uncomment the following line. Optical-flow is supported only by OpenCV 3.x - 4.x
#define TRACK_OPTFLOW 1
#define GPU 1
#define OPENCV 1
#define CUDNN 1

// To use 3D-stereo camera ZED - uncomment the following line. ZED_SDK should be installed.
#define ZED_STEREO 1


#include <yolo_v2_class.hpp>    // imported functions from DLL

#ifdef OPENCV
#ifdef ZED_STEREO
#include <sl/Camera.hpp>
#include <X11/Xlib.h>

#undef GPU // avoid conflict with sl::MEM::GPU

#pragma comment(lib, "sl_zed64.lib")


float getMedian ( std::vector<float> &v )
{
    size_t n = v.size() / 2;
    std::nth_element ( v.begin(), v.begin() + n, v.end() );
    return v[n];
}

extern "C" int XInitThreads ( void );

std::vector<bbox_t> get_3d_coordinates ( std::vector<bbox_t> & bbox_vect, vector<vector<float>> & depth, cv::Mat & img)
{
    std::vector<bbox_t> bbox3d_vect;
    static vector<vector<float>> sfondo;
    static double howmanytimes = 1.0;

    XInitThreads();

    static bool first_time = true;
    if (first_time)
    {
        FILE * fin = fopen("sfondo.dat", "r");
        int cols;
        int rows;
        read(fileno(fin), &howmanytimes, sizeof(howmanytimes));
        read(fileno(fin), &rows, sizeof(rows));
        read(fileno(fin), &cols, sizeof(cols));
        cout << "Got Howmanytimes=" << howmanytimes << ". ROWS: " << rows << ". COLS: " << cols << endl;
        sfondo = vector<vector<float>>(rows, vector<float>(cols));
        for (int r=0; r<sfondo.size(); ++r)
        {
            int letti = read(fileno(fin), sfondo[r].data(), sfondo[0].size()*sizeof(sfondo[0][0]));
            while (letti < sfondo[0].size())
            {
                int l = read(fileno(fin), &(sfondo[r].data()[letti]), sfondo[0].size()*sizeof(sfondo[0][0])-letti);
                letti += l;
            }
        }
        fclose(fin);
        first_time = false;
    }

//#define SALVASFONDO 1
#ifdef SALVASFONDO
    printf("%f==============================================================================================\n", howmanytimes);
    //Questo è per salvare gli sfondi!!!
    if (sfondo.empty())
    {
        sfondo = depth;
        howmanytimes = 1;
    } else {
        //compongo i due sfondi secondo una semplice media
        for (int r=0; r<depth.size(); ++r)
        {
            for (int c=0; c<depth[0].size(); ++c)
            {
                if (isfinite(depth[r][c]))
                {
                    if (isfinite(sfondo[r][c]))
                    {
                        sfondo[r][c] = (sfondo[r][c]*howmanytimes+depth[r][c])/(howmanytimes+1.0);
                    } else {
                        sfondo[r][c] = depth[r][c];
                    }
                }
            }
        }
        howmanytimes+=1.0;
    }
    {
        FILE * fou = fopen("sfondo.dat", "w");
        int cols = sfondo[0].size();
        int rows = sfondo.size();
        write(fileno(fou), &howmanytimes, sizeof(howmanytimes));
        write(fileno(fou), &rows, sizeof(rows));
        write(fileno(fou), &cols, sizeof(cols));
        for (int r=0; r<sfondo.size(); ++r)
        {
            int scritti = write(fileno(fou), sfondo[r].data(), sfondo[0].size()*sizeof(sfondo[0][0]));
            while (scritti < sfondo[0].size())
            {
                int l = write(fileno(fou), &(sfondo[r].data()[scritti]), sfondo[0].size()*sizeof(sfondo[0][0])-scritti);
                scritti += l;
            }
        }
        fclose(fou);
    }
#endif

    //Per ogni bounding box
    for ( auto &cur_box : bbox_vect ) {

        vector<float> vz3d;
        for (int r=cur_box.y; r<(cur_box.y+cur_box.h); ++r)
        {
            if (r >= depth.size() || r >= sfondo.size())
                break;
            for (int c=cur_box.x; c<(cur_box.x+cur_box.w); ++c)
            {
                if (c >= depth[r].size() || c >= sfondo[r].size())
                    break;
                if (isfinite(depth[r][c]) && isfinite(sfondo[r][c]))
                {
                    float v=(sfondo[r][c] - depth[r][c]);
                    if (v > 0.0)
                        vz3d.push_back(v);
                }
            }
        }
        //prendo i più grandi e ne faccio la media (prendo da 12/16 fino a 14/16)
        {
            size_t n = 12*vz3d.size()/16;
            size_t n_fine = 14*vz3d.size()/16;
            std::nth_element ( vz3d.begin(), vz3d.begin() + n, vz3d.end() );
            float average = std::accumulate( vz3d.begin()+n, vz3d.begin()+n_fine, 0.0)/(double)(n_fine-n);
            cur_box.x_3d=cur_box.x+cur_box.w/2;
            cur_box.y_3d=cur_box.y+cur_box.h/2;
            cur_box.z_3d = average;
        }

//         getchar();
//
//         //per ogni rettangolo, punto al centro. solo per iniziare.
//         cur_box.x_3d=cur_box.x+cur_box.w/2;
//         cur_box.y_3d=cur_box.y+cur_box.h/2;
//         int x=cur_box.x_3d;
//         int y=cur_box.y_3d;
//         int s=0;
//         int cicli=0;
//         while (true)
//         {
//             bool stop = false;
//             for (int c=x-s; c<=x+s; ++c)
//             {
//                 for (int r=y-s; r <=y+s; ++r)
//                 {
//                     if (r >= 0 && c >= 0 && r <= depth.size() && c <= depth[0].size() && isfinite(depth[r][c]))
//                     {
//                         x=c; y=r;
//                         stop = true;
//                         break;
//                     }
//                 }
//             }
//             if (stop)
//                 break;
//             if (++cicli > 1000)
//                 break;
//             s++;
//         }
//         if (cicli < 1000)
//             cur_box.z_3d=depth[y][x];
//         else
//             cur_box.z_3d = std::nan("");
        bbox3d_vect.emplace_back ( cur_box );
    }
//     printf("==============================================================================================\n");
    return bbox3d_vect;
}

/**
 * @brief ...
 *
 * @param bbox_vect p_bbox_vect:...
 * @param xyzrgba p_xyzrgba:...
 * @param R_max_global p_R_max_global:... Defaults to 10.
 * @return std::vector< bbox_t >
 */
std::vector<bbox_t> get_3d_coordinates_orig ( std::vector<bbox_t> bbox_vect, cv::Mat xyzrgba, unsigned int R_max_global = 10)
{
    bool valid_measure;
    int i, j;
    //const unsigned int R_max_global = 10; 16 è migliore come valore

    std::vector<bbox_t> bbox3d_vect;

    typedef struct v4f {
        uchar x;
        uchar y;
        uchar z;
        uchar w;
    } Tv4f;

    try {
        //cout << xyzrgba.type() << ", " << xyzrgba.channels() << ", " << xyzrgba.depth() << ", " << xyzrgba.elemSize() << ", " << xyzrgba.elemSize1() << endl;
        for ( auto &cur_box : bbox_vect ) {

            const unsigned int obj_size = std::min ( cur_box.w, cur_box.h );
            const unsigned int R_max = std::min ( R_max_global, obj_size / 2 );
            int center_i = cur_box.x + cur_box.w * 0.5f, center_j = cur_box.y + cur_box.h * 0.5f;

            /*
             * Algoritmo ottimizzato consente di ridurre il numero di cicli almeno di un fattore 2*(logR -1). Es: R=32, divisione per 11.34(48000 => 4200), R=64, divisione per 22 (366145 => 16641), ... R=16, divisione per 6 (6500 >= 1000)
             * questo significa che la dimensione del risultato, prima del mediano, è la stessa ma il numero di accessi al vettore è inferiore
             */
            std::vector<float> x_vect, y_vect, z_vect;
            sl::float4 out ( NAN, NAN, NAN, NAN );
            for ( int R = 0; R < R_max; R++ ) {
                for ( int y = -R; y <= R; y++ ) {
                    for ( int x = -R; x <= R; x++ ) {
                        try {
                            i = center_i + x;
                            j = center_j + y;
                            if ( i >= 0 && i < xyzrgba.cols && j >= 0 && j < xyzrgba.rows ) {
                                cv::Vec4f &elem = xyzrgba.at<cv::Vec4f> ( j, i ); // x,y,z,w
                                out.x = elem[0];
                                out.y = elem[1];
                                out.z = elem[2];
                                out.w = elem[3];
                            }
                            valid_measure = std::isfinite ( out.z );
                            if ( valid_measure ) {
                                x_vect.push_back ( out.x );
                                y_vect.push_back ( out.y );
                                z_vect.push_back ( out.z );
                            }
                        } catch (cv::Exception ex)
                        {
                            cerr << ex.what() << endl;
                        } catch (exception ex)
                        {
                            cerr << ex.what() << endl;
                        }
                    }
                }
            }

            try {
                if ( x_vect.size() * y_vect.size() * z_vect.size() > 0 ) {
                    cur_box.x_3d = getMedian ( x_vect );
                    cur_box.y_3d = getMedian ( y_vect );
                    cur_box.z_3d = getMedian ( z_vect );
                } else {
                    cur_box.x_3d = NAN;
                    cur_box.y_3d = NAN;
                    cur_box.z_3d = NAN;
                }
            } catch (exception ex)
            {
                cerr << ex.what() << endl;
                cur_box.x_3d = NAN;
                cur_box.y_3d = NAN;
                cur_box.z_3d = NAN;
            }
            //cout << cur_box.x_3d << ", " << cur_box.y_3d << ", " << cur_box.z_3d << endl;
            bbox3d_vect.emplace_back ( cur_box );
        }
    } catch (cv::Exception ex) {
        cerr << "CVException: " << ex.what() << endl;
    } catch (exception ex) {
        cerr << "StdException: " << ex.what() << endl;
    }
    return bbox3d_vect;
}

/*
cv::Mat slMat2cvMat(const sl::Mat &input) {
    int cv_type = -1; // Mapping between MAT_TYPE and CV_TYPE
    if(input.getDataType() ==
        sl::MAT_TYPE::F32_C4
        ) {
        cv_type = CV_32FC4;
    } else cv_type = CV_8UC4; // sl::Mat used are either RGBA images or XYZ (4C) point clouds
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(
        sl::MEM::CPU
        ));
}


cv::Mat slMat2cvMat ( sl::Mat &zed_image )
{
    // Convert sl::Mat to cv::Mat (share buffer)
    cv::Mat cvImage;
    if (zed_image.getChannels() == 1)
        cvImage = cv::Mat((int) zed_image.getHeight(), (int) zed_image.getWidth(), CV_8UC1, zed_image.getPtr<sl::uchar1>(sl::MEM::CPU));
    else
        cvImage = cv::Mat((int) zed_image.getHeight(), (int) zed_image.getWidth(), CV_8UC4, zed_image.getPtr<sl::uchar1>(sl::MEM::CPU));

    return cvImage;

//     //Check that selection rectangle is valid and draw it on the image
//     if (!selection_rect.isEmpty() && selection_rect.isContained(sl::Resolution(cvImage.cols, cvImage.rows)))
//     cv::rectangle(cvImage, cv::Rect(selection_rect.x,selection_rect.y,selection_rect.width,selection_rect.height),cv::Scalar(0, 255, 0), 2);

//     int cv_type = -1; // Mapping between MAT_TYPE and CV_TYPE
//     if ( input.getDataType() == sl::MAT_TYPE::F32_C4) {
//         cv_type = CV_32FC4;
//     } else {
//         cv_type = CV_8UC4;    // sl::Mat used are either RGBA images or XYZ (4C) point clouds
//     }
//     return cv::Mat ( input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1> (
//                          sl::MEM::CPU) );
}*/


// // Sample variables
/*******************************************************************************************************/
/*******************************************************************************************************/
//! ! ! ! ! ! ! ! P E R  G E S T I R E  P A R A M E T R I  D E L L A  T E L E C A M E R A ! ! ! ! ! ! ! !
/*******************************************************************************************************/
/*******************************************************************************************************/
// sl::VIDEO_SETTINGS camera_settings_ = sl::VIDEO_SETTINGS::BRIGHTNESS;
// sl::VIEW view_mode = sl::VIEW::LEFT;
// string str_camera_settings = "BRIGHTNESS";
// int step_camera_setting = 1;
// bool led_on = true;
//
// void switchViewMode() {
//     view_mode = static_cast<sl::VIEW> ((int) view_mode + 1);
//
//     // reset to 1st setting
//     if (view_mode == sl::VIEW::DEPTH_RIGHT)
//         view_mode = sl::VIEW::LEFT;
//
//
//     //print("Switch to view mode: ", sl::ERROR_CODE::SUCCESS, string(sl::toString(view_mode).c_str()));
// }

#else   // ZED_STEREO
std::vector<bbox_t> get_3d_coordinates ( std::vector<bbox_t> bbox_vect, cv::Mat xyzrgba )
{
    return bbox_vect;
}
#endif  // ZED_STEREO


#include <opencv2/opencv.hpp>            // C++
#include <opencv2/core/version.hpp>
#ifndef CV_VERSION_EPOCH     // OpenCV 3.x and 4.x
#include <opencv2/videoio/videoio.hpp>
#include <unistd.h>
#define OPENCV_VERSION CVAUX_STR(CV_VERSION_MAJOR)"" CVAUX_STR(CV_VERSION_MINOR)"" CVAUX_STR(CV_VERSION_REVISION)
#ifndef USE_CMAKE_LIBS
#pragma comment(lib, "opencv_world" OPENCV_VERSION ".lib")
#ifdef TRACK_OPTFLOW
/*
 * #pragma comment(lib, "opencv_cudaoptflow" OPENCV_VERSION ".lib")
 * #pragma comment(lib, "opencv_cudaimgproc" OPENCV_VERSION ".lib")
 * #pragma comment(lib, "opencv_core" OPENCV_VERSION ".lib")
 * #pragma comment(lib, "opencv_imgproc" OPENCV_VERSION ".lib")
 * #pragma comment(lib, "opencv_highgui" OPENCV_VERSION ".lib")
 */
#endif    // TRACK_OPTFLOW
#endif    // USE_CMAKE_LIBS
#else     // OpenCV 2.x
#define OPENCV_VERSION CVAUX_STR(CV_VERSION_EPOCH)"" CVAUX_STR(CV_VERSION_MAJOR)"" CVAUX_STR(CV_VERSION_MINOR)
#ifndef USE_CMAKE_LIBS
#pragma comment(lib, "opencv_core" OPENCV_VERSION ".lib")
#pragma comment(lib, "opencv_imgproc" OPENCV_VERSION ".lib")
#pragma comment(lib, "opencv_highgui" OPENCV_VERSION ".lib")
#pragma comment(lib, "opencv_video" OPENCV_VERSION ".lib")
#endif    // USE_CMAKE_LIBS
#endif    // CV_VERSION_EPOCH


unordered_set<string> usv;
void draw_boxes ( cv::Mat mat_img, std::vector<bbox_t> result_vec, std::vector<std::string> obj_names,
                  int current_det_fps = -1, int current_cap_fps = -1, bool showAllClasses=true, unordered_set<string> &cltosh = usv )
{
    int const colors[6][3] = { { 1,0,1 },{ 0,0,1 },{ 0,1,1 },{ 0,1,0 },{ 1,1,0 },{ 1,0,0 } };

    for ( auto &i : result_vec ) {
        if ( !showAllClasses && obj_names.size() > i.obj_id && cltosh.find ( obj_names[i.obj_id] ) == cltosh.end() ) {
            continue; //di questo elemento non sono interessato
        }
        cv::Scalar color = obj_id_to_color ( i.obj_id );
        cv::rectangle ( mat_img, cv::Rect ( i.x, i.y, i.w, i.h ), color, 2 );
        if ( obj_names.size() > i.obj_id ) {
            std::string obj_name = obj_names[i.obj_id];
            if ( i.track_id > 0 ) {
                obj_name = obj_name.substr ( 0,3 ) + "(" + to_string ( i.prob ) + ":" + to_string ( i.track_id ) + ")";
            }
            cv::Size const text_size = getTextSize ( obj_name, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, 1, 0 );
            int max_width = ( text_size.width > i.w + 2 ) ? text_size.width : ( i.w + 2 );
            max_width = std::max ( max_width, ( int ) i.w + 2 );
            //max_width = std::max(max_width, 283);
            std::string coords_3d;
            if ( !std::isnan ( i.z_3d ) ) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision ( 0 ) << i.x_3d << ":" << i.y_3d << ":" << i.z_3d;
                coords_3d = ss.str();
                cv::Size const text_size_3d = getTextSize ( ss.str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, 1, 0 );
                int const max_width_3d = ( text_size_3d.width > i.w + 2 ) ? text_size_3d.width : ( i.w + 2 );
                if ( max_width_3d > max_width ) {
                    max_width = max_width_3d;
                }
            }

            //             cv::rectangle(mat_img, cv::Point2f(std::max((int)i.x - 1, 0), std::max((int)i.y - 35, 0)),
            //                           cv::Point2f(std::min((int)i.x + max_width, mat_img.cols - 1), std::min((int)i.y, mat_img.rows - 1)),
            //                           color, CV_FILLED, 8, 0);
            putText ( mat_img, obj_name, cv::Point2f ( i.x, i.y - text_size.height - 3 ), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar ( 0, 0, 0 ), 1 );
            if ( !coords_3d.empty() ) {
                putText ( mat_img, coords_3d, cv::Point2f ( i.x, i.y-1 ), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar ( 0, 0, 0 ), 1 );
            }
        }
    }
    if ( current_det_fps >= 0 && current_cap_fps >= 0 ) {
        std::string fps_str = "FPS detection: " + std::to_string ( current_det_fps ) + "   FPS capture: " + std::to_string ( current_cap_fps );
        putText ( mat_img, fps_str, cv::Point2f ( 10, 20 ), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, cv::Scalar ( 50, 255, 0 ), 2 );
    }
}
#endif    // OPENCV


void show_console_result ( std::vector<bbox_t> const result_vec, std::vector<std::string> const obj_names, int frame_id = -1 )
{
    if ( frame_id >= 0 ) {
        std::cout << " Frame: " << frame_id << std::endl;
    }
    for ( auto &i : result_vec ) {
        if ( obj_names.size() > i.obj_id ) {
            std::cout << obj_names[i.obj_id] << " - ";
        }
        std::cout << "obj_id = " << i.obj_id << ",  x = " << i.x << ", y = " << i.y
                  << ", w = " << i.w << ", h = " << i.h
                  << std::setprecision ( 3 ) << ", prob = " << i.prob <<
                  ", track_id: " << i.track_id <<
                  std::endl;
    }
}

std::vector<std::string> objects_names_from_file ( std::string const filename )
{
    std::ifstream file ( filename );
    std::vector<std::string> file_lines;
    if ( !file.is_open() ) {
        return file_lines;
    }
    for ( std::string line; getline ( file, line ); ) {
        file_lines.push_back ( line );
    }
    std::cout << "object names loaded \n";
    return file_lines;
}

typedef struct detection_data_t {
    cv::Mat cap_frame;
    std::shared_ptr<image_t> det_image;
    std::vector<bbox_t> result_vec;
    cv::Mat draw_frame;
    bool new_detection;
    uint64_t frame_id;
    bool exit_flag;
    //vector<vector<float>> zed_cloud;
    cv::Mat zed_cloud;
    std::queue<cv::Mat> track_optflow_queue;
    uint64_t microsecs; //tempo della rilevazione
    detection_data_t() {
        exit_flag = false;
        new_detection = false;
    };
} detection_data_t;


class send_one_replaceable_object_t
{
    const bool sync;
    std::atomic<detection_data_t *> a_ptr;
public:
    static bool to_be_stopped;

    void send ( detection_data_t const& _obj )
    {
        detection_data_t *new_ptr = new detection_data_t;
        *new_ptr = _obj;
        if ( sync ) {
            while ( a_ptr.load() && !to_be_stopped ) {
                std::this_thread::sleep_for ( std::chrono::microseconds ( 1 ) );
            }
        }
        std::unique_ptr<detection_data_t> old_ptr ( a_ptr.exchange ( new_ptr ) );
    }

    detection_data_t receive()
    {
        std::unique_ptr<detection_data_t> ptr;
        do {
            while ( !a_ptr.load() && !to_be_stopped ) {
                std::this_thread::sleep_for ( std::chrono::microseconds ( 1 ) );
            }
            if ( !to_be_stopped ) {
                ptr.reset ( a_ptr.exchange ( NULL ) );
            }
        } while ( !ptr && !to_be_stopped );
        detection_data_t obj;
        if ( !to_be_stopped ) {
            obj = *ptr;
        } else {
            obj = detection_data_t();
        }
        return obj;
    }

    bool is_object_present()
    {
        return ( a_ptr.load() != NULL );
    }

    send_one_replaceable_object_t ( bool _sync ) : sync ( _sync ), a_ptr ( NULL )
    {
        to_be_stopped = false;
    }
};
bool send_one_replaceable_object_t::to_be_stopped = false;

#endif

// template<typename T>
// class send_one_replaceable_object_t
// {
//     const bool sync;
//     std::atomic<T *> a_ptr;
// public:
//     static bool to_be_stopped;
//
//     void send ( T const& _obj )
//     {
//         T *new_ptr = new T;
//         *new_ptr = _obj;
//         if ( sync ) {
//             while ( a_ptr.load() && !to_be_stopped ) {
//                 std::this_thread::sleep_for ( std::chrono::milliseconds ( 3 ) );
//             }
//         }
//         std::unique_ptr<T> old_ptr ( a_ptr.exchange ( new_ptr ) );
//     }
//
//     T receive()
//     {
//         std::unique_ptr<T> ptr;
//         do {
//             while ( !a_ptr.load() && !to_be_stopped ) {
//                 std::this_thread::sleep_for ( std::chrono::milliseconds ( 3 ) );
//             }
//             if ( !to_be_stopped ) {
//                 ptr.reset ( a_ptr.exchange ( NULL ) );
//             }
//         } while ( !ptr && !to_be_stopped );
//         T obj;
//         if ( !to_be_stopped ) {
//             obj = *ptr;
//         } else {
//             obj = T();
//         }
//         return obj;
//     }
//
//     bool is_object_present()
//     {
//         return ( a_ptr.load() != NULL );
//     }
//
//     send_one_replaceable_object_t ( bool _sync ) : sync ( _sync ), a_ptr ( NULL )
//     {
//         to_be_stopped = false;
//     }
// };
//

void call_rt_callback(const cv::Mat * cap_frame, int fps, int width, int height, SCM runlisp);
void call_rt_callback(const cv::Mat * cap_frame, int fps, int width, int height, SCM runlisp)
{
    //auto mat = scm_from_pointer ( new cv::Mat ( cap_frame ), releaseMat );

    //Il primo parametro indica se immagine o dati!
    //l'ultimo parametro (0) indica che proviene dal sottosistema di AI
    //non crea una nuova mat poiché questa la spedisce immediatamente al sistema di streaming e quindi non ci sono code nel mezzo, è una funzione sincrona!
    scm_call_2( runlisp, SCM_BOOL_T, scm_list_5(scm_from_pointer((void*)cap_frame, NULL), scm_from_int(fps), scm_from_int(width), scm_from_int(height), scm_from_int(0)));
}


// //modified to send also the tracking points
// bool send_json_http ( const cv::Mat & cap_frame, std::vector<bbox_t> cur_bbox_vec, std::vector<std::string> obj_names, int frame_id, vector<cv::Point2f> & vpoints_new, vector<cv::Point2f> & vpoints_old, std::vector<bbox_t> zcoord_new, int port, SCM runlisp, uint64_t microsecs,
//                       bool showAllClasses=true, unordered_set<string> &cltosh = usv,
//                       std::string filename = std::string(), int timeout = 400000 )
// {
//     if ( port >= 0 ) {
//         //invia stringa json
//         std::string send_str;
//         int imgheight = cap_frame.size().height;
//         int imgwidth  = cap_frame.size().width;
//         if ( !filename.empty() ) {
//             send_str = fmt::format ( "{{\"frame_id\":{0},\"microsecs\":{2},\"filename\":\"{1}\",\"imgheight\":\"{3}\",\"imgwidth\":\"{4}\",\"objects\":[", frame_id, filename, microsecs, imgheight, imgwidth );
//         } else {
//             send_str = fmt::format ( "{{\"frame_id\":{0},\"microsecs\":{1},\"imgheight\":\"{2}\",\"imgwidth\":\"{3}\",\"objects\":[", frame_id, microsecs, imgheight, imgwidth );
//         }
// 
//         for ( auto & i : cur_bbox_vec ) {
//             if ( !showAllClasses && obj_names.size() > i.obj_id && cltosh.find ( obj_names[i.obj_id] ) == cltosh.end() ) {
//                 //cerr << "Questo oggetto " << obj_names[i.obj_id] << " non è previsto." << endl;
//                 continue; //di questo elemento non sono interessato
//             }
//             send_str += fmt::format ( "{{\"class_id\":{0},\"name\":\"{1}\",\"absolute_coordinates\":{{\"tl_x\":{2},\"tl_y\":{3},\"width\":{4},\"height\":{5}}},\"confidence\":{6},\"track_id\":{7}", i.obj_id, obj_names[i.obj_id], i.x, i.y, i.w, i.h, i.prob, i.track_id );
// 
//             if ( !std::isnan ( i.z_3d ) ) {
//                 send_str += fmt::format ( ",\"coordinates_in_meters\":{{\"x_3d\":{0},\"y_3d\":{1},\"z_3d\":{2}}}",
//                                           fmt::format ( "{:.{}f}", i.x_3d, 2 ),
//                                           fmt::format ( "{:.{}f}", i.y_3d, 2 ),
//                                           fmt::format ( "{:.{}f}", i.z_3d, 2 ) );
//             }
//             send_str += "},";
//         }
// 
//         if ( send_str.back() == ',' ) {
//             send_str.pop_back();
//         }
//         send_str += "]";
// 
//         string new_features_x, new_features_y, new_features_z;
//         for ( int i=0; i<vpoints_new.size(); ++i ) {
//             new_features_x += fmt::format ( "{:.{}f}", vpoints_new[i].x, 2 ) + ",";
//             new_features_y += fmt::format ( "{:.{}f}", vpoints_new[i].y, 2 ) + ",";
//             if ( zcoord_new.size() > 0 )
//                 new_features_z += "[" +
//                                   fmt::format ( "{:.{}f}", zcoord_new[i].x_3d, 2 ) + "," +
//                                   fmt::format ( "{:.{}f}", zcoord_new[i].y_3d, 2 ) + "," +
//                                   fmt::format ( "{:.{}f}", zcoord_new[i].z_3d, 2 ) + "]" +
//                                   ",";
//         }
//         if ( new_features_x.size() > 0 ) {
//             new_features_x.pop_back();
//         }
// 
//         if ( new_features_y.size() > 0 ) {
//             new_features_y.pop_back();
//         }
// 
//         if ( new_features_z.size() > 0 ) {
//             new_features_z.pop_back();
//         }
// 
//         string old_features_x, old_features_y;
//         for ( int i=0; i<vpoints_old.size(); ++i ) {
//             old_features_x += fmt::format ( "{:.{}f}", vpoints_old[i].x, 2 ) + ",";
//             old_features_y += fmt::format ( "{:.{}f}", vpoints_old[i].y, 2 ) + ",";
//         }
//         if ( old_features_x.size() > 0 ) {
//             old_features_x.pop_back();
//         }
//         if ( old_features_y.size() > 0 ) {
//             old_features_y.pop_back();
//         }
// 
//         //     cerr << "new_features: " << new_features_x << ", " << new_features_y << endl;
//         //     cerr << "old_features: " << old_features_x << ", " << old_features_y << endl;
//         send_str += ",\"new_features\":{\"x\":[" + new_features_x + "],\"y\":[" + new_features_y + "],\"z\":[" + new_features_z + "]}" +
//                     ",\"old_features\":{\"x\":[" + old_features_x + "],\"y\":[" + old_features_y + "]}" +
//                     "}";
// 
//         send_json_custom ( send_str.c_str(), port, timeout );
//     } else if ( runlisp != SCM_EOL ) {
//         //Chiama funzione lisp con una alist
//         static SCM scm_frame_id = scm_from_utf8_string ( "frame_id" );
//         static SCM scm_objects = scm_from_utf8_string ( "objects" );
//         static SCM scm_filename = scm_from_utf8_string ( "filename" );
//         static SCM scm_class_id = scm_from_utf8_string ( "class_id" );
//         static SCM scm_name = scm_from_utf8_string ( "name" );
//         static SCM scm_absolute_coordinates = scm_from_utf8_string ( "absolute_coordinates" );
//         static SCM scm_center_x = scm_from_utf8_string ( "tl_x" );
//         static SCM scm_center_y = scm_from_utf8_string ( "tl_y" );
//         static SCM scm_width = scm_from_utf8_string ( "width" );
//         static SCM scm_height = scm_from_utf8_string ( "height" );
//         static SCM scm_imgwidth = scm_from_utf8_string ( "imgwidth" );
//         static SCM scm_imgheight = scm_from_utf8_string ( "imgheight" );
//         static SCM scm_confidence = scm_from_utf8_string ( "confidence" );
//         static SCM scm_track_id = scm_from_utf8_string ( "track_id" );
//         static SCM scm_coordinates_in_meters = scm_from_utf8_string ( "coordinates_in_meters" );
//         static SCM scm_x_3d = scm_from_utf8_string ( "x_3d" );
//         static SCM scm_y_3d = scm_from_utf8_string ( "y_3d" );
//         static SCM scm_z_3d = scm_from_utf8_string ( "z_3d" );
//         static SCM scm_new_features = scm_from_utf8_string ( "new_features" );
//         static SCM scm_old_features = scm_from_utf8_string ( "old_features" );
//         static SCM scm_x = scm_from_utf8_string ( "x" );
//         static SCM scm_y = scm_from_utf8_string ( "y" );
//         static SCM scm_z = scm_from_utf8_string ( "z" );
//         static SCM scm_microsecs = scm_from_utf8_string ( "microsecs" );
// 
//         int imgheight = cap_frame.size().height;
//         int imgwidth  = cap_frame.size().width;
// 
//         // bool send_json_http ( const cv::Mat & cap_frame, std::vector<bbox_t> cur_bbox_vec, std::vector<std::string> obj_names, int frame_id, vector<cv::Point2f> & vpoints_new, vector<cv::Point2f> & vpoints_old, std::vector<bbox_t> zcoord_new, int port = 8070, SCM runlisp, std::string filename = std::string(), int timeout = 400000 )
//         SCM head = SCM_EOL;
//         head = scm_cons ( scm_cons ( scm_frame_id, scm_from_int ( frame_id ) ), head );
//         head = scm_cons ( scm_cons ( scm_microsecs, scm_from_long_long ( microsecs ) ), head );
//         head = scm_cons ( scm_cons ( scm_imgheight, scm_from_int ( imgheight ) ), head );
//         head = scm_cons ( scm_cons ( scm_imgwidth, scm_from_int ( imgwidth ) ), head );
//         if ( !filename.empty() ) {
//             head = scm_cons ( scm_cons ( scm_filename, scm_from_latin1_string ( filename.c_str() ) ), head );
//         }
//         //questo è un array di oggetti
//         SCM objects = SCM_EOL; //scm_make_vector(scm_from_int(cur_bbox_vec.size()), 0);
//         //int pos = 0;
//         for ( auto & i : cur_bbox_vec ) {
//             if ( !showAllClasses && obj_names.size() > i.obj_id && cltosh.find ( obj_names[i.obj_id] ) == cltosh.end() ) {
//                 //cerr << "Questo oggetto " << obj_names[i.obj_id] << " non è previsto." << endl;
//                 continue; //di questo elemento non sono interessato
//             }
//             SCM item = SCM_EOL;
//             item = scm_cons ( scm_cons ( scm_class_id, scm_from_int ( i.obj_id ) ), item );
//             item = scm_cons ( scm_cons ( scm_name, scm_from_latin1_string ( obj_names[i.obj_id].c_str() ) ), item );
//             item = scm_cons ( scm_cons ( scm_center_x, scm_from_int ( i.x ) ), item );
//             item = scm_cons ( scm_cons ( scm_center_y, scm_from_int ( i.y ) ), item );
//             item = scm_cons ( scm_cons ( scm_width, scm_from_int ( i.w ) ), item );
//             item = scm_cons ( scm_cons ( scm_height, scm_from_int ( i.h ) ), item );
//             item = scm_cons ( scm_cons ( scm_confidence, scm_from_double ( i.prob ) ), item );
//             item = scm_cons ( scm_cons ( scm_track_id, scm_from_int ( i.track_id ) ), item );
//             if ( !std::isnan ( i.z_3d ) ) {
//                 item = scm_cons ( scm_cons ( scm_x_3d, scm_from_double ( i.x_3d ) ), item );
//                 item = scm_cons ( scm_cons ( scm_y_3d, scm_from_double ( i.y_3d ) ), item );
//                 item = scm_cons ( scm_cons ( scm_z_3d, scm_from_double ( i.z_3d ) ), item );
//             }
//             objects = scm_cons ( item, objects );
// //             scm_vector_set_x(objects, scm_from_int(pos), item);
// //             pos++;
//         }
//         objects = scm_vector ( objects );
// 
//         //altri due array di features (i punti good dell'immagine)
//         SCM new_features_x, new_features_y, new_features_z;
//         new_features_x = scm_make_vector ( scm_from_int ( vpoints_new.size() ), SCM_EOL );
//         new_features_y = scm_make_vector ( scm_from_int ( vpoints_new.size() ), SCM_EOL );
//         new_features_z = scm_make_vector ( scm_from_int ( vpoints_new.size() ), SCM_EOL );
//         for ( int i=0; i<vpoints_new.size(); ++i ) {
//             scm_vector_set_x ( new_features_x, scm_from_int ( i ), scm_from_double ( vpoints_new[i].x ) );
//             scm_vector_set_x ( new_features_y, scm_from_int ( i ), scm_from_double ( vpoints_new[i].y ) );
//             if ( zcoord_new.size() > 0 ) {
//                 SCM v3 = scm_make_vector ( scm_from_int ( 3 ), SCM_EOL );
//                 scm_vector_set_x ( v3, scm_from_int ( 0 ), scm_from_double ( zcoord_new[i].x_3d ) );
//                 scm_vector_set_x ( v3, scm_from_int ( 1 ), scm_from_double ( zcoord_new[i].y_3d ) );
//                 scm_vector_set_x ( v3, scm_from_int ( 2 ), scm_from_double ( zcoord_new[i].z_3d ) );
//                 scm_vector_set_x ( new_features_z, scm_from_int ( i ), v3 );
//             }
//         }
// 
//         SCM old_features_x, old_features_y;
//         old_features_x = scm_make_vector ( scm_from_int ( vpoints_old.size() ), SCM_EOL );
//         old_features_y = scm_make_vector ( scm_from_int ( vpoints_old.size() ), SCM_EOL );
//         for ( int i=0; i<vpoints_old.size(); ++i ) {
//             scm_vector_set_x ( old_features_x, scm_from_int ( i ), scm_from_double ( vpoints_old[i].x ) );
//             scm_vector_set_x ( old_features_y, scm_from_int ( i ), scm_from_double ( vpoints_old[i].y ) );
//         }
// 
//         SCM newfl = SCM_EOL;
//         newfl = scm_cons ( scm_cons ( scm_x, new_features_x ), newfl );
//         newfl = scm_cons ( scm_cons ( scm_y, new_features_y ), newfl );
//         newfl = scm_cons ( scm_cons ( scm_z, new_features_z ), newfl );
//         newfl = scm_cons ( scm_new_features, newfl );
// 
//         SCM oldfl = SCM_EOL;
//         oldfl = scm_cons ( scm_cons ( scm_x, old_features_x ), oldfl );
//         oldfl = scm_cons ( scm_cons ( scm_y, old_features_y ), oldfl );
//         oldfl = scm_cons ( scm_old_features, oldfl );
// 
//         head = scm_cons ( scm_cons ( scm_objects, objects ), head );
//         head = scm_cons ( newfl, head );
//         head = scm_cons ( oldfl, head );
// 
//         auto matclone = mtfa_internal_opencv_clone(cap_frame);
//         auto mat   = scm_from_pointer ( matclone, releaseMat );
// 
//         SCM bret = scm_call_2 ( runlisp, mat, head );
//         if ( bret == SCM_BOOL_T ) {
//             return true;
//         } else {
//             return false;
//         }
//     }
//     return true;
// }


extern unsigned long long GetULLTime();

void zed2_send_json_http (const cv::Mat & cvmat, const uint64_t & frame_id, const uint &fps, const uint &width, const uint &height, const sl::Objects & objects, const SCM & callback);
void zed2_send_json_http (const cv::Mat & cvmat, const uint64_t & frame_id, const uint &fps, const uint &width, const uint &height, const sl::Objects & objects, const SCM & callback)
{
    SCM objects_list = SCM_EOL;
    //auto start = GetULLTime();

    //cout << objects.object_list.size() << endl;
    for ( const sl::ObjectData & obj : objects.object_list ) {
        if (obj.bounding_box_2d.size() < 4 || obj.bounding_box.size() < 8)
            continue;

        if (obj.tracking_state == sl::OBJECT_TRACKING_STATE::OK) {
            SCM scm_mask = SCM_EOL;
//             try {
                if (obj.mask.isInit()) {

                    cv::Mat tmp = slMat2cvMat(obj.mask);
                    cv::Rect roi = cv::Rect ( obj.bounding_box_2d[0].x, obj.bounding_box_2d[0].y,
                                              obj.bounding_box_2d[2].x-obj.bounding_box_2d[0].x, obj.bounding_box_2d[2].y-obj.bounding_box_2d[0].y);
                    cv::Mat result, r1;
                    result = cvmat(roi);
                    result.copyTo(r1, tmp); //ottengo solo la parte definita dalla maschera!!

                    auto result1 = new cv::Mat(r1);
                    scm_mask = scm_from_pointer( (void*)result1, releaseMat);
                }

            objects_list = scm_cons(scm_list_n(
                                        scm_from_int((int)obj.action_state),
                                        scm_from_double(obj.bounding_box[0].x),
                                        scm_from_double(obj.bounding_box[0].y),
                                        scm_from_double(obj.bounding_box[0].z),
                                        scm_from_double(obj.bounding_box[1].x),
                                        scm_from_double(obj.bounding_box[1].y),
                                        scm_from_double(obj.bounding_box[1].z),
                                        scm_from_double(obj.bounding_box[2].x),
                                        scm_from_double(obj.bounding_box[2].y),
                                        scm_from_double(obj.bounding_box[2].z),
                                        scm_from_double(obj.bounding_box[3].x),
                                        scm_from_double(obj.bounding_box[3].y),
                                        scm_from_double(obj.bounding_box[3].z),
                                        scm_from_double(obj.bounding_box[4].x),
                                        scm_from_double(obj.bounding_box[4].y),
                                        scm_from_double(obj.bounding_box[4].z),
                                        scm_from_double(obj.bounding_box[5].x),
                                        scm_from_double(obj.bounding_box[5].y),
                                        scm_from_double(obj.bounding_box[5].z),
                                        scm_from_double(obj.bounding_box[6].x),
                                        scm_from_double(obj.bounding_box[6].y),
                                        scm_from_double(obj.bounding_box[6].z),
                                        scm_from_double(obj.bounding_box[7].x),
                                        scm_from_double(obj.bounding_box[7].y),
                                        scm_from_double(obj.bounding_box[7].z),
                                        scm_from_int(obj.bounding_box_2d[0].x),
                                        scm_from_int(obj.bounding_box_2d[0].y),
                                        scm_from_int(obj.bounding_box_2d[1].x),
                                        scm_from_int(obj.bounding_box_2d[1].y),
                                        scm_from_int(obj.bounding_box_2d[2].x),
                                        scm_from_int(obj.bounding_box_2d[2].y),
                                        scm_from_int(obj.bounding_box_2d[3].x),
                                        scm_from_int(obj.bounding_box_2d[3].y),
                                        scm_from_double(obj.confidence),
                                        scm_from_double(obj.position_covariance[0]),
                                        scm_from_double(obj.position_covariance[1]),
                                        scm_from_double(obj.position_covariance[2]),
                                        scm_from_double(obj.position_covariance[3]),
                                        scm_from_double(obj.position_covariance[4]),
                                        scm_from_double(obj.position_covariance[5]),
                                        scm_from_int(obj.dimensions.x),
                                        scm_from_int(obj.dimensions.y),
                                        scm_from_int(obj.dimensions.z),
                                        scm_from_int(obj.id),
                                        scm_from_int((int)obj.label),
                                        scm_from_double(obj.position.x),
                                        scm_from_double(obj.position.y),
                                        scm_from_double(obj.position.z),
                                        scm_from_int((int)obj.sublabel),
                                        scm_from_int((int)obj.tracking_state),
                                        scm_from_double(obj.velocity.x),
                                        scm_from_double(obj.velocity.y),
                                        scm_from_double(obj.velocity.z),
                                        scm_mask,
                                        SCM_UNDEFINED), objects_list);
        }
    }
    scm_call_7(callback, scm_from_pointer ( (void*)&cvmat, nullptr ), scm_from_long_long(frame_id), scm_from_uint(fps), scm_from_uint(width), scm_from_uint(height), objects_list, scm_from_ulong_long(objects.timestamp.data_ns / 1000ULL));
    
    //cerr << "Microsecondi per una chiamata: " << GetULLTime() - start << endl;
}


// static void download ( const cv::cuda::GpuMat& d_mat, vector<cv::Point2f>& vec )
// {
//     vec.resize ( d_mat.cols );
//     cv::Mat mat ( 1, d_mat.cols, CV_32FC2, ( void* ) &vec[0] );
//     d_mat.download ( mat );
// }

// static void download ( const cv::cuda::GpuMat& d_mat, vector<uchar>& vec )
// {
//     vec.resize ( d_mat.cols );
//     cv::Mat mat ( 1, d_mat.cols, CV_8UC1, ( void* ) &vec[0] );
//     d_mat.download ( mat );
// }
static void download ( const cv::Mat& mat, vector<uchar>& vec )
{
    cv::Mat flat = mat.reshape ( 1, mat.total() *mat.channels() );
    vec = mat.isContinuous() ? flat : flat.clone();
}
// static void download ( const cv::Mat& mat, vector<Vec4f>& vec )
// {
//     cv::Mat flat = mat.reshape ( 1, mat.total() * mat.channels() );
//     vec = mat.isContinuous() ? flat : flat.clone();
// }

static cv::Mat * upload ( const vector<uint8_t> & v )
{
    //for (int i=0; i<v.size(); ++i) cout << (uint)v[i] << " "; cout << endl;
//     cv::Mat * ret = new cv::Mat( v ); //.reshape( 0, v.size());
//     ret->reshape(0, 0);
//
//     ret->convertTo( *ret, CV_8UC1 );

    cv::Mat * m = new cv::Mat ( 1, v.size(), CV_8UC1 ); // initialize matrix of uchar of 1-channel where you will store vec data
    //copy vector to mat
    memcpy ( m->data, v.data(), v.size() *sizeof ( uchar ) ); // change uchar to any type of data values that you want to use instead
    return m;
}


// #ifdef USE_OLD_DETECTOR
// 
// /**************INTERFACCIA GUILE*****************/
// 
// static SCM mtfa_detector_type;  //il tipo del su
// 
// #define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))
// 
// class MyDetector
// {
// public:
//     Detector * detector;
//     mutex mtx;
//     bool stop;
//     bool terminated;
//     bool running;
// 
//     //Per il calcolo (preciso) delle distanze
//     cv::Mat last_cap_frame;
//     cv::Mat last_zed_cloud;
// 
//     //l'una o l'altra, per poterle governare da fuori
//     sl::Camera zed; // ZED-camera, SVO movies
//     cv::VideoCapture cap; //La camera normale (inclusi movie)
// 
//     MyDetector ( Detector * d )
//     {
//         detector = d;
//         stop = false;
//         terminated=false;
//         running=false;
//     };
//     ~MyDetector()
//     {
//         //qui dovrei prima stoppare i thread
//         while ( !terminated && running ) {
//             stop = true;
//             std::this_thread::sleep_for ( std::chrono::milliseconds ( 1 ) );
//         }
//         last_cap_frame.release();
//         last_zed_cloud.release();
//         delete detector;
//     }
//     void zed_reset_to_default();
// //     void zed_set_video_settings(const sl::VIDEO_SETTINGS vs, const int value, const sl::Rect & selection_rect, const bool &reset);
// //     void zed_get_video_settings(const sl::VIDEO_SETTINGS vs, int & value, sl::Rect & selection_rect);
//     void zed_get_sensors();
// };
// 
// void MyDetector::zed_get_sensors()
// {
//     //per ora non ne faccio nulla
//     sl::SensorsData sensors_data;
//     zed.getSensorsData(sensors_data, sl::TIME_REFERENCE::IMAGE); // Get frame synchronized sensor data
// 
//     // Extract multi-sensor data
//     auto imu_data = sensors_data.imu;
//     auto barometer_data = sensors_data.barometer;
//     auto magnetometer_data = sensors_data.magnetometer;
// 
//     // Retrieve linear acceleration and angular velocity
//     sl::float3 linear_acceleration = imu_data.linear_acceleration;
//     sl::float3 angular_velocity = imu_data.angular_velocity;
// 
//     // Retrieve pressure and relative altitude
//     float pressure = barometer_data.pressure;
//     float relative_altitude = barometer_data.relative_altitude;
// 
//     // Retrieve magnetic field
//     sl::float3 magnetic_field = magnetometer_data.magnetic_field_uncalibrated;
// }
// void MyDetector::zed_reset_to_default()
// {
//     for (int s = (int) sl::VIDEO_SETTINGS::BRIGHTNESS; s <= (int) sl::VIDEO_SETTINGS::WHITEBALANCE_TEMPERATURE; s++)
//         zed.setCameraSettings(static_cast<sl::VIDEO_SETTINGS> (s), sl::VIDEO_SETTINGS_VALUE_AUTO);
// }
// // void MyDetector::zed_set_video_settings(const sl::VIDEO_SETTINGS vs, const int value, const sl::Rect & selection_rect, const bool & reset)
// // {
// //     switch (vs)
// //     {
// //         case sl::VIDEO_SETTINGS::BRIGHTNESS: /**< Defines the brightness control. Affected value should be between 0 and 8.*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         case sl::VIDEO_SETTINGS::CONTRAST: /**< Defines the contrast control. Affected value should be between 0 and 8.*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         case sl::VIDEO_SETTINGS::HUE: /**< Defines the hue control. Affected value should be between 0 and 11.*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         case sl::VIDEO_SETTINGS::SATURATION: /**< Defines the saturation control. Affected value should be between 0 and 8.*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         case sl::VIDEO_SETTINGS::SHARPNESS: /**< Defines the digital sharpening control. Affected value should be between 0 and 8.*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         case sl::VIDEO_SETTINGS::GAMMA: /** < Defines the ISP gamma control. Affected value should be between 1 and 9.*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         case sl::VIDEO_SETTINGS::GAIN: /**< Defines the gain control. Affected value should be between 0 and 100 for manual control.*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         case sl::VIDEO_SETTINGS::EXPOSURE: /**< Defines the exposure control. Affected value should be between 0 and 100 for manual control.\n The exposition is mapped linearly in a percentage of the following max values. Special case for the setExposure(0) that corresponds to 0.17072ms.\n The conversion to milliseconds depends on the framerate: <ul><li>15fps setExposure(100) -> 19.97ms</li><li>30fps setExposure(100) -> 19.97ms</li><li>60fps setExposure(100) -> 10.84072ms</li><li>100fps setExposure(100) -> 10.106624ms</li></ul>*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         case sl::VIDEO_SETTINGS::AEC_AGC: /**< Defines if the Gain and Exposure are in automatic mode or not. Setting a Gain or Exposure through @GAIN or @EXPOSURE values will automatically set this value to 0.*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         case sl::VIDEO_SETTINGS::AEC_AGC_ROI: /**< Defines the region of interest for automatic exposure/gain computation. To be used with overloaded @setCameraSettings/@getCameraSettings functions.*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, selection_rect, sl::SIDE::BOTH, true);
// //             else
// //                 zed.setCameraSettings(vs, selection_rect, sl::SIDE::BOTH);
// //             break;
// //         case sl::VIDEO_SETTINGS::WHITEBALANCE_TEMPERATURE: /**< Defines the color temperature value. Setting a value will automatically set @WHITEBALANCE_AUTO to 0. Affected value should be between 2800 and 6500 with a step of 100.*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         case sl::VIDEO_SETTINGS::WHITEBALANCE_AUTO: /**< Defines if the White balance is in automatic mode or not*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         case sl::VIDEO_SETTINGS::LED_STATUS: /**< Defines the status of the camera front LED. Set to 0 to disable the light, 1 to enable the light. Default value is on. Requires Camera FW 1523 at least.*/
// //             if (reset)
// //                 zed.setCameraSettings(vs, sl::VIDEO_SETTINGS_VALUE_AUTO);
// //             else
// //                 zed.setCameraSettings(vs, value);
// //             break;
// //         default:
// //             break;
// //     }
// // }
// // void MyDetector::zed_get_video_settings(const sl::VIDEO_SETTINGS vs, int & value, sl::Rect & selection_rect)
// // {
// //     switch (vs)
// //     {
// //         case sl::VIDEO_SETTINGS::BRIGHTNESS: /**< Defines the brightness control. Affected value should be between 0 and 8.*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         case sl::VIDEO_SETTINGS::CONTRAST: /**< Defines the contrast control. Affected value should be between 0 and 8.*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         case sl::VIDEO_SETTINGS::HUE: /**< Defines the hue control. Affected value should be between 0 and 11.*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         case sl::VIDEO_SETTINGS::SATURATION: /**< Defines the saturation control. Affected value should be between 0 and 8.*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         case sl::VIDEO_SETTINGS::SHARPNESS: /**< Defines the digital sharpening control. Affected value should be between 0 and 8.*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         case sl::VIDEO_SETTINGS::GAMMA: /** < Defines the ISP gamma control. Affected value should be between 1 and 9.*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         case sl::VIDEO_SETTINGS::GAIN: /**< Defines the gain control. Affected value should be between 0 and 100 for manual control.*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         case sl::VIDEO_SETTINGS::EXPOSURE: /**< Defines the exposure control. Affected value should be between 0 and 100 for manual control.\n The exposition is mapped linearly in a percentage of the following max values. Special case for the setExposure(0) that corresponds to 0.17072ms.\n The conversion to milliseconds depends on the framerate: <ul><li>15fps setExposure(100) -> 19.97ms</li><li>30fps setExposure(100) -> 19.97ms</li><li>60fps setExposure(100) -> 10.84072ms</li><li>100fps setExposure(100) -> 10.106624ms</li></ul>*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         case sl::VIDEO_SETTINGS::AEC_AGC: /**< Defines if the Gain and Exposure are in automatic mode or not. Setting a Gain or Exposure through @GAIN or @EXPOSURE values will automatically set this value to 0.*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         case sl::VIDEO_SETTINGS::AEC_AGC_ROI: /**< Defines the region of interest for automatic exposure/gain computation. To be used with overloaded @setCameraSettings/@getCameraSettings functions.*/
// //             zed.getCameraSettings(vs, selection_rect);
// //             break;
// //         case sl::VIDEO_SETTINGS::WHITEBALANCE_TEMPERATURE: /**< Defines the color temperature value. Setting a value will automatically set @WHITEBALANCE_AUTO to 0. Affected value should be between 2800 and 6500 with a step of 100.*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         case sl::VIDEO_SETTINGS::WHITEBALANCE_AUTO: /**< Defines if the White balance is in automatic mode or not*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         case sl::VIDEO_SETTINGS::LED_STATUS: /**< Defines the status of the camera front LED. Set to 0 to disable the light, 1 to enable the light. Default value is on. Requires Camera FW 1523 at least.*/
// //             value = zed.getCameraSettings(vs);
// //             break;
// //         default:
// //             break;
// //     }
// // }
// 
// static void finalize_mtfa_detector ( SCM mtfa_detector )
// {
//     MyDetector* pt = ( MyDetector* ) scm_foreign_object_signed_ref ( mtfa_detector, 0 );
// 
//     if ( pt ) {
//         scm_foreign_object_signed_set_x ( mtfa_detector, 0, 0 );
//         delete pt;
//     }
// }
// 
// 
// static SCM mtfa_detector_p ( SCM mtfa_detector )
// {
//     auto const body = [] ( void* data ) {
//         scm_assert_foreign_object_type ( mtfa_detector_type, * ( SCM* ) data );
//         return SCM_BOOL_T;
//     };
//     auto const handler = [] ( void* data, SCM key, SCM args ) {
//         return SCM_BOOL_F;
//     };
//     SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_detector, handler, nullptr );
//     return ret;
// }
// 
// 
// static SCM mtfa_detector_make ( SCM cfg, SCM weights )
// {
//     if ( scm_is_string ( cfg ) == 1 && scm_is_string ( weights ) == 1 ) {
//         char * s_cfg = scm_to_latin1_string ( cfg );
//         char * s_weights = scm_to_latin1_string ( weights );
//         if ( access ( s_cfg, F_OK ) == 0 && access ( s_weights, F_OK ) == 0 ) {
//             //posso inizializzare il detector
//             Detector * detector = new Detector ( s_cfg, s_weights );
//             free ( s_cfg );
//             free ( s_weights );
// 
//             XInitThreads();
// 
//             MyDetector * p = new MyDetector ( detector );
//             return scm_make_foreign_object_1 ( mtfa_detector_type, ( void* ) p );
//         }
//         free ( s_cfg );
//         free ( s_weights );
//     }
//     return SCM_BOOL_F;
// }
// 
// static SCM mtfa_detector_stop ( SCM scm_detector )
// {
//     if ( mtfa_detector_p ( scm_detector ) == SCM_BOOL_F ) {
//         scm_wrong_type_arg_msg ( "mtfa-detector-stop", 0, scm_detector, "mtfa-detector type" );
//     }
// 
//     MyDetector* mydetector = ( MyDetector* ) scm_foreign_object_signed_ref ( scm_detector, 0 );
//     mydetector->stop = true;
// 
//     return SCM_BOOL_T;
// }
// 
// 
// #endif

// static uint64_t micros()
// {
//     uint64_t us = std::chrono::duration_cast<std::chrono::microseconds> ( std::chrono::high_resolution_clock::now().time_since_epoch() ).count();
//     return us;
// }

static void setvalue ( const SCM alist, const string s, int & val )
{
    auto found = scm_assoc_ref ( alist, scm_from_latin1_string ( s.c_str() ) );
    if ( found != SCM_BOOL_F ) {
        val = scm_to_int ( found );
    }
}
static void setvalue ( const SCM alist, const string s, double & val )
{
    auto found = scm_assoc_ref ( alist, scm_from_latin1_string ( s.c_str() ) );
    if ( found != SCM_BOOL_F ) {
        val = scm_to_double ( found );
    }
}
static void setvalue ( const SCM alist, const string s, bool & val )
{
    auto found = scm_assoc ( scm_from_latin1_string ( s.c_str() ), alist );
    if ( found != SCM_BOOL_F ) {
        val = ( scm_to_bool(scm_cdr(found)) == 1 );
    }
}

static void setvalue ( const SCM alist, const string s, string & val )
{
    auto found = scm_assoc_ref ( alist, scm_from_latin1_string ( s.c_str() ) );
    if ( found != SCM_BOOL_F ) {
        char * s = scm_to_latin1_string ( found );
        val.assign ( s );
        free ( s );
    }
}

static void setvalue ( const SCM alist, const string s, SCM & val )
{
    auto found = scm_assoc ( scm_from_latin1_string ( s.c_str() ), alist);
    if ( found != SCM_BOOL_F ) {
        val = scm_cdr(found);
    }
}

#define SETVAL(name) setvalue(par_list, #name, name)


static sl::VIDEO_SETTINGS convert_to_vs(int v)
{
    switch (v)
    {
    case 0:
        return sl::VIDEO_SETTINGS::BRIGHTNESS;
    case 1:
        return sl::VIDEO_SETTINGS::CONTRAST;
    case 2:
        return sl::VIDEO_SETTINGS::HUE;
    case 3:
        return sl::VIDEO_SETTINGS::SATURATION;
    case 4:
        return sl::VIDEO_SETTINGS::SHARPNESS;
    case 5:
        return sl::VIDEO_SETTINGS::GAMMA;
    case 6:
        return sl::VIDEO_SETTINGS::GAIN;
    case 7:
        return sl::VIDEO_SETTINGS::EXPOSURE;
    case 8:
        return sl::VIDEO_SETTINGS::AEC_AGC;
    case 9:
        return sl::VIDEO_SETTINGS::AEC_AGC_ROI;
    case 10:
        return sl::VIDEO_SETTINGS::WHITEBALANCE_TEMPERATURE;
    case 11:
        return sl::VIDEO_SETTINGS::WHITEBALANCE_AUTO;
    case 12:
        return sl::VIDEO_SETTINGS::LED_STATUS;
    default:
        return sl::VIDEO_SETTINGS::LAST;
    }
}
static SCM mtfa_zed_camera_grab ( SCM scm_zed_camera )
{
    sl::Camera * zed = (sl::Camera *)scm_to_pointer(scm_zed_camera);
    if (zed->grab() == sl::ERROR_CODE::SUCCESS)
        return SCM_BOOL_T;
    else
        return SCM_BOOL_F;
}
static SCM mtfa_zed_camera_get_image ( SCM scm_zed_camera )
{
    sl::Camera * zed = (sl::Camera *)scm_to_pointer(scm_zed_camera);

    auto image = sl::Mat();
    if (sl::ERROR_CODE::SUCCESS == zed->retrieveImage(image, sl::VIEW::LEFT)) {
        auto cvmat = new cv::Mat();
        cv::cvtColor (Zed_slMat2cvMat(image), *cvmat, cv::COLOR_RGBA2RGB); //CV_RGBA2RGB );
        return scm_from_pointer ( cvmat, releaseMat );
    } else {
        return SCM_EOL;
    }
}


static SCM mtfa_zed_camera_get ( SCM scm_zed_camera, SCM scm_what)
{
    sl::Camera * zed = (sl::Camera *)scm_to_pointer(scm_zed_camera);
    int what = scm_to_int32(scm_what);
    sl::VIDEO_SETTINGS vs = convert_to_vs(what);
    int setting;
    switch (vs)
    {
    case sl::VIDEO_SETTINGS::BRIGHTNESS: /**< Defines the brightness control. Affected value should be between 0 and 8.*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    case sl::VIDEO_SETTINGS::CONTRAST: /**< Defines the contrast control. Affected value should be between 0 and 8.*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    case sl::VIDEO_SETTINGS::HUE: /**< Defines the hue control. Affected value should be between 0 and 11.*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    case sl::VIDEO_SETTINGS::SATURATION: /**< Defines the saturation control. Affected value should be between 0 and 8.*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    case sl::VIDEO_SETTINGS::SHARPNESS: /**< Defines the digital sharpening control. Affected value should be between 0 and 8.*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    case sl::VIDEO_SETTINGS::GAMMA: /** < Defines the ISP gamma control. Affected value should be between 1 and 9.*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    case sl::VIDEO_SETTINGS::GAIN: /**< Defines the gain control. Affected value should be between 0 and 100 for manual control.*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    case sl::VIDEO_SETTINGS::EXPOSURE: /**< Defines the exposure control. Affected value should be between 0 and 100 for manual control.\n The exposition is mapped linearly in a percentage of the following max values. Special case for the setExposure(0) that corresponds to 0.17072ms.\n The conversion to milliseconds depends on the framerate: <ul><li>15fps setExposure(100) -> 19.97ms</li><li>30fps setExposure(100) -> 19.97ms</li><li>60fps setExposure(100) -> 10.84072ms</li><li>100fps setExposure(100) -> 10.106624ms</li></ul>*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    case sl::VIDEO_SETTINGS::AEC_AGC: /**< Defines if the Gain and Exposure are in automatic mode or not. Setting a Gain or Exposure through @GAIN or @EXPOSURE values will automatically set this value to 0.*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    case sl::VIDEO_SETTINGS::AEC_AGC_ROI: /**< Defines the region of interest for automatic exposure/gain computation. To be used with overloaded @setCameraSettings/@getCameraSettings functions.*/
    {
        sl::Rect selection_rect;
        zed->getCameraSettings(vs, selection_rect);
        SCM l=SCM_EOL;
        l = scm_cons(scm_from_int(selection_rect.height), l);
        l = scm_cons(scm_from_int(selection_rect.width), l);
        l = scm_cons(scm_from_int(selection_rect.y), l);
        l = scm_cons(scm_from_int(selection_rect.x), l);
        return l;
    }
    break;
    case sl::VIDEO_SETTINGS::WHITEBALANCE_TEMPERATURE: /**< Defines the color temperature value. Setting a value will automatically set @WHITEBALANCE_AUTO to 0. Affected value should be between 2800 and 6500 with a step of 100.*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    case sl::VIDEO_SETTINGS::WHITEBALANCE_AUTO: /**< Defines if the White balance is in automatic mode or not*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    case sl::VIDEO_SETTINGS::LED_STATUS: /**< Defines the status of the camera front LED. Set to 0 to disable the light, 1 to enable the light. Default value is on. Requires Camera FW 1523 at least.*/
        zed->getCameraSettings(vs, setting);
        return scm_from_int(setting);
        break;
    default:
        return SCM_BOOL_F;
    }
}

static SCM mtfa_zed_camera_set ( SCM scm_zed_camera, SCM scm_what, SCM value)
{
    sl::Camera * zed = (sl::Camera *)scm_to_pointer(scm_zed_camera);

    int what = scm_to_int32(scm_what);
    sl::VIDEO_SETTINGS vs = convert_to_vs(what);
    //cerr << "In mtfa_zed_camera_set " << what << " => " << vs << endl;
    switch (vs)
    {
    case sl::VIDEO_SETTINGS::BRIGHTNESS: /**< Defines the brightness control. Affected value should be between 0 and 8.*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    case sl::VIDEO_SETTINGS::CONTRAST: /**< Defines the contrast control. Affected value should be between 0 and 8.*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    case sl::VIDEO_SETTINGS::HUE: /**< Defines the hue control. Affected value should be between 0 and 11.*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    case sl::VIDEO_SETTINGS::SATURATION: /**< Defines the saturation control. Affected value should be between 0 and 8.*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    case sl::VIDEO_SETTINGS::SHARPNESS: /**< Defines the digital sharpening control. Affected value should be between 0 and 8.*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    case sl::VIDEO_SETTINGS::GAMMA: /** < Defines the ISP gamma control. Affected value should be between 1 and 9.*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    case sl::VIDEO_SETTINGS::GAIN: /**< Defines the gain control. Affected value should be between 0 and 100 for manual control.*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    case sl::VIDEO_SETTINGS::EXPOSURE: /**< Defines the exposure control. Affected value should be between 0 and 100 for manual control.\n The exposition is mapped linearly in a percentage of the following max values. Special case for the setExposure(0) that corresponds to 0.17072ms.\n The conversion to milliseconds depends on the framerate: <ul><li>15fps setExposure(100) -> 19.97ms</li><li>30fps setExposure(100) -> 19.97ms</li><li>60fps setExposure(100) -> 10.84072ms</li><li>100fps setExposure(100) -> 10.106624ms</li></ul>*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    case sl::VIDEO_SETTINGS::AEC_AGC: /**< Defines if the Gain and Exposure are in automatic mode or not. Setting a Gain or Exposure through @GAIN or @EXPOSURE values will automatically set this value to 0.*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    case sl::VIDEO_SETTINGS::AEC_AGC_ROI: /**< Defines the region of interest for automatic exposure/gain computation. To be used with overloaded @setCameraSettings/@getCameraSettings functions.*/
    {
        sl::Rect selection_rect;
        selection_rect.x = scm_to_int(scm_car(value));
        selection_rect.y = scm_to_int(scm_cadr(value));
        selection_rect.width = scm_to_int(scm_caddr(value));
        selection_rect.height = scm_to_int(scm_cadddr(value));
        zed->setCameraSettings(vs, selection_rect);
    }
    break;
    case sl::VIDEO_SETTINGS::WHITEBALANCE_TEMPERATURE: /**< Defines the color temperature value. Setting a value will automatically set @WHITEBALANCE_AUTO to 0. Affected value should be between 2800 and 6500 with a step of 100.*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    case sl::VIDEO_SETTINGS::WHITEBALANCE_AUTO: /**< Defines if the White balance is in automatic mode or not*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    case sl::VIDEO_SETTINGS::LED_STATUS: /**< Defines the status of the camera front LED. Set to 0 to disable the light, 1 to enable the light. Default value is on. Requires Camera FW 1523 at least.*/
        zed->setCameraSettings(vs, scm_to_int(value));
        break;
    default:
        return SCM_BOOL_F;
    }
    return SCM_BOOL_F;
}

static SCM mtfa_zed_camera_reset ( SCM scm_zed_camera )
{
    sl::Camera * zed = (sl::Camera *)scm_to_pointer(scm_zed_camera);
    for (int s = (int) sl::VIDEO_SETTINGS::BRIGHTNESS; s <= (int) sl::VIDEO_SETTINGS::WHITEBALANCE_TEMPERATURE; s++)
        zed->setCameraSettings(static_cast<sl::VIDEO_SETTINGS> (s), sl::VIDEO_SETTINGS_VALUE_AUTO);
    return SCM_BOOL_T;
}

//Quando la ZED 2 camera è in streaming da jetson nano
vector< string> split(const string& s, char seperator);
vector< string> split(const string& s, char seperator) {
    vector< string> output;
    string::size_type prev_pos = 0, pos = 0;

    while ((pos = s.find(seperator, pos)) != string::npos) {
        string substring(s.substr(prev_pos, pos - prev_pos));
        output.push_back(substring);
        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos - prev_pos));
    return output;
}

void setStreamParameter(sl::InitParameters& init_p, string& argument);
void setStreamParameter(sl::InitParameters& init_p, string& argument) {
    vector< string> configStream = split(argument, ':');
    String ip(configStream.at(0).c_str());
    if (configStream.size() == 2) {
        init_p.input.setFromStream(ip.c_str(), atoi(configStream.at(1).c_str()));
    } else init_p.input.setFromStream(ip.c_str());
}

bool OpenOrReopen(sl::Camera & zed, sl::InitParameters & init_params);
bool OpenOrReopen(sl::Camera & zed, sl::InitParameters & init_params) {
    //cerr << "Opening or reopening...\n";
    if ( zed.isOpened() ) {
        try {
            zed.close();
        } catch (...)
        {}
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    auto ret = zed.open ( init_params );
    if (ret != sl::ERROR_CODE::SUCCESS)
        return false;
    else
    {
        //alcuni valori di default che settiamo
        // static int zed_camera_brightness = 6;
        // static int zed_camera_contrast = 6;
        // static int zed_camera_sharpness = 3;
        // static int zed_camera_saturation = 7;

//         zed.setCameraSettings(sl::VIDEO_SETTINGS::BRIGHTNESS, zed_camera_brightness);
//         zed.setCameraSettings(sl::VIDEO_SETTINGS::CONTRAST, zed_camera_contrast);
//         zed.setCameraSettings(sl::VIDEO_SETTINGS::SHARPNESS, zed_camera_sharpness);
//         zed.setCameraSettings(sl::VIDEO_SETTINGS::SATURATION, zed_camera_saturation);

        zed.disableObjectDetection();
        zed.disablePositionalTracking();
        zed.disableRecording();

        return true;
    }
}

bool CameraRead(sl::Camera & zed, cv::Mat & mat);
bool CameraRead(sl::Camera & zed, cv::Mat & mat) {
//     runtime_parameters.sensing_mode = SENSING_MODE::STANDARD; // Use STANDARD sensing mode

    if ( zed.grab() != sl::ERROR_CODE::SUCCESS ) {
        return false;
    }
    try {
        mat = zed_capture_rgb ( zed );
        return true;
    } catch (...) {
        return false;
    }
}

bool OpenOrReopen(cv::VideoCapture &cap, int numcam);
bool OpenOrReopen(cv::VideoCapture &cap, int numcam)
{
    if ( cap.isOpened() ) {
        try {
            cap.release();
        } catch (...)
        {}
    }
    return cap.open ( numcam );
}

bool OpenOrReopen(cv::VideoCapture &cap, string & thecam);
bool OpenOrReopen(cv::VideoCapture &cap, string & thecam)
{
    if ( cap.isOpened() ) {
        try {
            cap.release();
        } catch (...)
        {}
    }
    return cap.open ( thecam );
}

bool CameraRead(cv::VideoCapture &cap, cv::Mat & mat);
bool CameraRead(cv::VideoCapture &cap, cv::Mat & mat)
{
    return cap.read ( mat );
}


// static SCM mtfa_detector_get_zed_camera ( SCM scm_detector, SCM scm_zed_camera_num )
// {
//     if ( mtfa_detector_p ( scm_detector ) == SCM_BOOL_F ) {
//         scm_wrong_type_arg_msg ( "mtfa-detector-get-zed-camera", 0, scm_detector, "mtfa-detector type" );
//     }
// 
//     MyDetector* mydetector = ( MyDetector* ) scm_foreign_object_signed_ref ( scm_detector, 0 );
//     return scm_from_pointer (&mydetector->zed, nullptr);
// }

//#define OLD_VERSION 1
// #ifdef OLD_VERSION
// static SCM mtfa_detector_run ( SCM scm_detector, SCM par_list )
// {
//     //i parametri con i loro default
//     string names;
//     string video="0";
//     double threshold = 0.2;
//     bool isStationary = true;
//     SCM callback;  //by default, -1 : nulla
//     SCM rt_callback;
//     bool windows = true;
//     string classes = "all";
//     int height = 0;
//     int width = 0;
//     string zed_resolution = "HD720";
//     string zed_depth_mode = "ULTRA";
//     int flow_winsize = 15;
//     int flow_iterations = 8000;
//     bool get_corners = false;
//     int corn_winsize = 10;
//     int corn_period = 5;
//     int max_count = 1000;
//     double corn_qlev = 0.01;
//     double corn_mindist = 10;
//     int corn_critcount = 20;
//     double corn_accuracy = 0.03;
//     bool corn_3dcoord = false;
//     int corn_3dsquare = 20;
//     bool show_original=false;
//     int remove_frames = 0;
//     bool detection_sync =false;
//     string zed_camera_ip; // = "0.0.0.0:30000";
//     int active_rect_tlx = -1;
//     int active_rect_tly = -1;
//     int active_rect_w = -1;
//     int active_rect_h = -1;
// 
//     if ( scm_detector == SCM_UNDEFINED || par_list == SCM_UNDEFINED || scm_assoc_ref ( par_list, scm_from_latin1_string ( "help" ) ) != SCM_BOOL_F ) {
//         cout <<
//              "First parameter must be the result of a (mtfa-opencv-detector-make cfg weights). The other are in alist format and are the following: " << endl <<
//              "Minimum parameters are: (mtfa-opencv-detector-run det '((\"names\" . \"../darknet/data/coco.names\")(\"video\" . \"test.mp4\")))" << endl << endl <<
//              "{help            |                | this message }" << endl <<
//              "{names           |                | file containing names }" << endl <<
// //         "{cfg             |                | file containing the configuration }" << endl <<
// //         "{weights         |                | file containing the weights }" << endl <<
//              "{video           | 0              | movie, image, device or camera number }" << endl <<
//              "{threshold       | 0.2            | threshold of detection }" << endl <<
//              "{isStationary    | true           | camera is (true/false) stationary }" << endl <<
//              "{callback        | -1/port/fname  | do not send detections data, send detections data to tcp port, call the function fname with two pars: img, json-detections}" << endl <<
//              "{rt_callback     | SCM_EOL/fname  | do not send real time data detected, call the function fname with one pars: img}" << endl <<
//              "{windows         | true           | show windows (true/false) during detection }" << endl <<
//              "{classes         | all            | list of the classes to show(\"class\",\"class\",\"class\",\"class\") or \"none\"}" << endl <<
//              "{height          |                | Height in pixel to resize the frames}" << endl <<
//              "{width           |                | Width in pixel to resize the frames}" << endl <<
//              "{zed_resolution  | HD720          | Zed camera resolution (HD720, HD1080, HD2K, VGA}" << endl <<
//              "{zed_depth_mode  | ULTRA          | Zed camera depth mode (NONE, PERFORMANCE, QUALITY, ULTRA)}" << endl <<
//              "{flow_winsize    | 15             | the size (9, 15, 21, 31) of the SparsePyrLKOpticalFlow algorithm}" << endl <<
//              "{flow_iterations | 8000           | the iterations of the SparsePyrLKOpticalFlow algorithm}" << endl <<
//              "{get_corners     | false          | evaluate good corners in the image}" << endl <<
//              "{corn_winsize    | 10             | good corners win size}" << endl <<
//              "{corn_period     | 5              | good corners taken every 5 frame}" << endl <<
//              "{max_count       | 1000           | good corners, max number of features to track }" << endl <<
//              "{corn_qlev       | 0.01           | good corners quality level}" << endl <<
//              "{corn_mindist    | 10             | good corners min distance}" << endl <<
//              "{corn_critcount  | 20             | good corners termcriteria count}" << endl <<
//              "{corn_accuracy   | 0.03           | good corners termcriteria accuracy}" << endl <<
//              "{corn_3dcoord    | false          | good corners get 3d coordinates}" << endl <<
//              "{corn_3dsquare   | 20             | good corners the square inside to take 3d coords}" << endl <<
//              "{show_original   | false          | show the original video/frame }" << endl <<
//              "{remove_frames   | 0              | how many frames to remove }" << endl <<
//              "{detection_sync  | false          | should be trues for movies }" << endl <<
//              "{zed_camera_ip   | empty          | for receiving ZED streams. IE: 0.0.0.0:3000}" << endl <<
//              "{active_rect_tlx | -1             | use whole image}" << endl <<
//              "{active_rect_tly | -1             | use whole image}" << endl <<
//              "{active_rect_w   | -1             | use whole image}" << endl <<
//              "{active_rect_h   | -1             | use whole image}" << endl <<
//              endl << "All given in alist format\n";
//         return SCM_BOOL_T;
//     }
// 
//     if ( mtfa_detector_p ( scm_detector ) == SCM_BOOL_F ) {
//         scm_wrong_type_arg_msg ( "mtfa-detector-run", 0, scm_detector, "mtfa-detector type" );
//     }
// 
//     MyDetector* mydetector = ( MyDetector* ) scm_foreign_object_signed_ref ( scm_detector, 0 );
//     lock_guard<mutex> lck ( mydetector->mtx );
// 
//     mydetector->stop = false;
//     mydetector->running = true;
//     Detector * detector = mydetector->detector;
// 
//     SETVAL ( names );
//     SETVAL ( video );
//     SETVAL ( threshold );
//     SETVAL ( isStationary );
//     SETVAL ( callback );
//     SETVAL ( rt_callback );
//     SETVAL ( windows );
//     SETVAL ( classes );
//     SETVAL ( height );
//     SETVAL ( width );
//     SETVAL ( zed_resolution );
//     SETVAL ( zed_depth_mode );
//     SETVAL ( flow_winsize );
//     SETVAL ( flow_iterations );
//     SETVAL ( get_corners );
//     SETVAL ( corn_winsize );
//     SETVAL ( corn_period );
//     SETVAL ( max_count );
//     SETVAL ( corn_qlev );
//     SETVAL ( corn_mindist );
//     SETVAL ( corn_critcount );
//     SETVAL ( corn_accuracy );
//     SETVAL ( corn_3dcoord );
//     SETVAL ( corn_3dsquare );
//     SETVAL ( show_original );
//     SETVAL ( remove_frames );
//     SETVAL ( detection_sync );
//     SETVAL ( zed_camera_ip );
//     SETVAL ( active_rect_tlx );
//     SETVAL ( active_rect_tly );
//     SETVAL ( active_rect_w );
//     SETVAL ( active_rect_h);
// 
//     if ( show_original ) {
//         cv::namedWindow ( "original", 0 );
//     }
//     if ( windows ) {
//         cv::namedWindow ( "computed", 0 );
//     }
// 
//     //Se la callback è un numero, allora inviamo via http su questa porta, altrimenti chiamo la funzione lisp
//     int i_callback;
//     SCM runlisp;
//     cout << "CB  : " << callback << endl;
//     if ( scm_is_integer ( callback ) ) {
//         i_callback = scm_to_int ( callback );
//         cout << i_callback  << endl;
//         runlisp = SCM_EOL;
//     } else {
//         i_callback = -1;
//         runlisp = callback;
//     }
// 
//     bool rt_call_callback;
//     if (scm_is_true(scm_procedure_p(rt_callback)))
//     {
//         cout << "Will call the rt callback\n";
//         rt_call_callback = true;
//     }
//     else {
//         cout << "Will not call the rt callback\n";
//         rt_call_callback = false;
//     }
// 
//     string names_file = names;
// //     string cfg_file = parser.get<string> ( "cfg" );
// //     string weights_file = parser.get<string> ( "weights" );
// 
//     float thresh = threshold;
//     bool show_windows = windows;
//     bool showAllClasses = true;
//     string classesToShow = classes;
// 
//     sl::RESOLUTION camera_resolution;
//     string s_camera_resolution = zed_resolution;
//     if ( s_camera_resolution == "HD720" ) {
//         camera_resolution = sl::RESOLUTION::HD720;
//     } else if ( s_camera_resolution == "HD1080" ) {
//         camera_resolution = sl::RESOLUTION::HD1080;
//     } else if ( s_camera_resolution == "HD2K" ) {
//         camera_resolution = sl::RESOLUTION::HD2K;
//     } else if ( s_camera_resolution == "VGA" ) {
//         camera_resolution = sl::RESOLUTION::VGA;
//     } else {
//         camera_resolution = sl::RESOLUTION::HD720;
//     }
// 
//     sl::DEPTH_MODE camera_depth_mode;
//     string s_camera_depth_mode = zed_depth_mode;
//     if ( s_camera_depth_mode == "NONE" ) {
//         camera_depth_mode = sl::DEPTH_MODE::NONE;
//     } else if ( s_camera_depth_mode == "PERFORMANCE" ) {
//         camera_depth_mode = sl::DEPTH_MODE::PERFORMANCE;
//     } else if ( s_camera_depth_mode == "QUALITY" ) {
//         camera_depth_mode = sl::DEPTH_MODE::QUALITY;
//     } else if ( s_camera_depth_mode == "ULTRA" ) {
//         camera_depth_mode = sl::DEPTH_MODE::ULTRA;
//     } else {
//         camera_depth_mode = sl::DEPTH_MODE::PERFORMANCE;
//     }
// 
//     int sparse_win_size = flow_winsize;
//     int sparse_iterations = flow_iterations;
// 
//     cv::Size w_corn_winsize ( corn_winsize, corn_winsize );
//     cv::TermCriteria termcrit ( cv::TermCriteria::COUNT|cv::TermCriteria::EPS,corn_critcount, corn_accuracy );
// 
//     if ( height || width ) {
//         cout << "Images will be resized to " << height << "x" << width << endl;
//     }
// 
//     unordered_set<string> cltosh; //classes to show
//     showAllClasses = true;
//     if ( classesToShow.compare ( "all" ) == 0 ) {
//         showAllClasses = true;
//     } else {
//         //prende tutte le classi che sono tra le virgole
//         cerr << "Classi da inserire: " << classesToShow << endl;
//         showAllClasses = false;
//         typedef vector<string> T_V_Split;
//         T_V_Split mtfa_string_split ( const string& str, const string& delimiters );
//         T_V_Split mtfa_string_split2 ( const string& str, const string& delimiter );
//         string mtfa_replace_matching_pattern ( const string & data, const string & pattern, const string & replace, bool all );
//         auto vs = mtfa_string_split (
//                       mtfa_replace_matching_pattern (
//                           mtfa_replace_matching_pattern (
//                               mtfa_replace_matching_pattern ( classesToShow, "\"", "", true ),
//                               ", ", ",", true ),
//                           " ,", ",", true ),
//                       "," );
//         for ( auto it: vs ) {
//             cerr << "Inserisco classe: " << it << endl;
//             cltosh.insert ( it );
//         }
//     }
// 
//     //In ogni caso non salvo su disco!
//     std::string filename;
//     int numcam = -1;
//     if ( isdigit ( video[0] ) ) {
//         numcam = stoi ( video );
//         filename = "web_camera";
//     } else {
//         filename = video;
//     }
// 
//     //i punti significativi dell'immagine,quando attivati
//     vector<cv::Point2f> vpoints_old;
//     vector<cv::Point2f> vpoints_new;
//     std::vector<bbox_t> tredvec;
// 
// //    Detector * detector = new Detector( cfg_file, weights_file );
// 
//     auto obj_names = objects_names_from_file ( names_file );
//     //    std::string out_videofile = "result.avi";
//     //    bool const save_output_videofile = false;   // true - for history
//     bool const send_network = ( i_callback != -1 || runlisp != SCM_EOL );      // true - for remote detection
//     bool const use_kalman_filter = isStationary; //false;   // true - for stationary camera
// 
//     //bool detection_sync = true;             // true - for video-file, assigned by parameters
// 
// #ifdef TRACK_OPTFLOW    // for slow GPU
//     //detection_sync = false;
//     Tracker_optflow tracker_flow ( detector->cur_gpu_id, sparse_win_size, 1, sparse_iterations );
// //     if ( numcam >= 0 || filename == "zed_camera" ) {
// //         detector->wait_stream = true;
// //     }
// #endif  // TRACK_OPTFLOW
// 
//     //creo un array di colori
//     vector<cv::Scalar> colors;
//     cv::RNG rng;
//     for ( int i = 0; i < 100; i++ ) {
//         int r = rng.uniform ( 0, 256 );
//         int g = rng.uniform ( 0, 256 );
//         int b = rng.uniform ( 0, 256 );
//         colors.push_back ( cv::Scalar ( r,g,b ) );
//     }
// 
//     //Le ho dichiarate in mydetector, per poterle governare da fuori
// //     sl::Camera zed; // ZED-camera
// //     cv::VideoCapture cap; //La camera normale (inclusi movie)
// 
//     while ( true ) {
//         try {
// #ifdef OPENCV
//             std::string const file_ext = filename.substr ( filename.find_last_of ( "." ) + 1 );
//             std::string const protocol = filename.substr ( 0, 7 );
//             if ( file_ext == "avi" || file_ext == "mp4" || file_ext == "mjpg" || file_ext == "mov" ||    // video file
//                     protocol == "rtmp://" || protocol == "rtsp://" || protocol == "http://" || protocol == "https:/" ||    // video network stream
//                     filename == "zed_camera" || file_ext == "svo" || filename == "web_camera" )  // ZED stereo camera
// 
//             {
//                 cv::Mat cur_frame;
//                 std::atomic<int> fps_cap_counter ( 0 ), fps_det_counter ( 0 );
//                 std::atomic<int> current_fps_cap ( 0 ), current_fps_det ( 0 );
//                 std::atomic<bool> exit_flag ( false );
//                 std::chrono::steady_clock::time_point steady_start, steady_end;
//                 int video_fps = 25;
//                 bool use_zed_camera = false;
// 
//                 track_kalman_t track_kalman;
// 
//                 //Qui apre e poi legge un frame, come preparazione per leggere in continua
// 
// #ifdef ZED_STEREO
//                 sl::InitParameters init_params;
//                 init_params.depth_minimum_distance = 2;
//                 init_params.depth_maximum_distance = 40;
//                 init_params.depth_mode = camera_depth_mode; // sl::DEPTH_MODE::PERFORMANCE;
//                 init_params.camera_resolution = camera_resolution; // sl::RESOLUTION::HD720; //sl::RESOLUTION::HD720;// sl::RESOLUTION::HD1080, sl::RESOLUTION::HD720
//                 init_params.coordinate_units = sl::UNIT::METER;
// 
//                 if ( file_ext == "svo" ) {
//                     init_params.input.setFromSVOFile ( filename.c_str() );
//                 }
//                 //init_params.sdk_cuda_ctx = (CUcontext)detector.get_cuda_context();
//                 init_params.sdk_gpu_id = -1; //detector.cur_gpu_id;
//                 if (zed_camera_ip.size() > 0)
//                     setStreamParameter(init_params, zed_camera_ip);
//                 cout << "Streaming from " << zed_camera_ip << endl;
// 
//                 if ( filename == "zed_camera" || file_ext == "svo" ) {
//                     use_zed_camera = true;
//                     while (true)
//                     {
//                         if (mydetector->stop)
//                             goto fine_run;
//                         try {
//                             if (!OpenOrReopen(mydetector->zed, init_params))
//                             {
//                                 std::cout << "Error: ZED Camera should be connected to USB 3.0. And ZED_SDK should be installed.\n";
//                                 std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//                             }
//                             else
//                             {
//                                 std::cout << "ZED 3D Camera opened." << std::endl;
//                                 break;
//                             }
//                         } catch (exception ex)
//                         {
//                             cerr << "Eccezione: " << ex.what() << endl;
//                             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//                         }
//                     }
// 
//                     // Print camera information
//                     auto camera_info = mydetector->zed.getCameraInformation();
//                     cout << "ZED Model                 : " << camera_info.camera_model << endl;
//                     cout << "ZED Serial Number         : " << camera_info.serial_number << endl;
//                     cout << "ZED Camera Firmware       : " << camera_info.camera_configuration.firmware_version << "/" << camera_info.sensors_configuration.firmware_version << endl;
//                     cout << "ZED Camera Resolution     : " << camera_info.camera_configuration.resolution.width << "x" << camera_info.camera_configuration.resolution.height << endl;
//                     cout << "ZED Camera FPS            : " << mydetector->zed.getInitParameters().camera_fps << endl;
// 
//                     //sl::PositionalTrackingParameters::set_as_static = true;
//                     //mydetector->zed.getPositionalTrackingParameters()
// 
//                     //legge il primo frame, eventualmente rimuovendo i frame richiesti
//                     while (true)
//                     {
//                         if (mydetector->stop)
//                             goto fine_run;
//                         try {
//                             if ( !CameraRead(mydetector->zed, cur_frame)) {
//                                 if (!mydetector->zed.isOpened())
//                                 {
//                                     OpenOrReopen(mydetector->zed, init_params);
//                                     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//                                 }
//                                 else
//                                     std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                             }
//                             else
//                                 break;
//                         } catch (exception ex)
//                         {
//                             cerr << "Eccezione: " << ex.what() << endl;
//                             OpenOrReopen(mydetector->zed, init_params);
//                             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//                         }
//                     }
//                     if ( show_original ) {
//                         cv::imshow ( "original", cur_frame );
//                         cv::waitKey ( 1 );
//                     }
//                 }
// #endif  // ZED_STEREO
// 
//                 if (!use_zed_camera)
//                 {
//                     //per prima cosa apro in lettura la webcam o il file
//                     while (true)
//                     {
//                         try {
//                             bool bret;
//                             if (mydetector->stop)
//                                 goto fine_run;
// 
//                             if ( numcam >= 0 )
//                                 bret = OpenOrReopen(mydetector->cap, numcam);
//                             else
//                                 bret = OpenOrReopen(mydetector->cap, filename);
// 
//                             if (!bret)
//                             {
//                                 std::cout << "Error: Camera/file should be connected/existing.\n";
//                                 std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//                             }
//                             else
//                                 break;
//                         } catch (exception ex)
//                         {
//                             cerr << "Eccezione: " << ex.what() << endl;
//                             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//                         }
//                     }
// 
//                     while (true)
//                     {
//                         if (mydetector->stop)
//                             goto fine_run;
//                         try {
//                             if ( !CameraRead(mydetector->cap, cur_frame) ) {
//                                 if (!mydetector->cap.isOpened())
//                                 {
//                                     if ( numcam >= 0 )
//                                         OpenOrReopen(mydetector->cap, numcam);
//                                     else
//                                         OpenOrReopen(mydetector->cap, filename);
//                                     std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                                 }
//                                 else
//                                     std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                             }
//                             else
//                                 break;
//                         } catch (exception ex)
//                         {
//                             cerr << "Eccezione: " << ex.what() << endl;
//                             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//                             if ( numcam >= 0 )
//                                 OpenOrReopen(mydetector->cap, numcam);
//                             else
//                                 OpenOrReopen(mydetector->cap, filename);
//                         }
//                     }
//                     if ( show_original ) {
//                         cv::imshow ( "original", cur_frame );
//                         cv::waitKey ( 1 );
//                     }
//                 }
// 
//                 if (use_zed_camera)
//                 {
//                     video_fps = mydetector->zed.getInitParameters().camera_fps;
//                 }
//                 else
//                 {
//                     video_fps = mydetector->cap.get ( cv::CAP_PROP_FPS );
//                 }
// 
//                 cv::Size frame_size = cur_frame.size();
//                 cout << "\n Video size: " << frame_size << endl;
//                 cout << "starting" << endl;
// 
//                 send_one_replaceable_object_t soro_cap2prepare ( detection_sync ), soro_cap2draw ( detection_sync ), soro_prepare2detect ( detection_sync ), soro_detect2draw ( detection_sync), soro_draw2show ( detection_sync ), soro_draw2net ( detection_sync );
// 
//                 std::thread t_cap, t_prepare, t_detect, t_draw, t_network; //t_post
// 
//                 bool please_stop = false;
//                 send_one_replaceable_object_t::to_be_stopped=false;
// 
//                 //capture new video-frame
//                 t_cap = std::thread ( [&]() {
//                     scm_init_guile();  //faccio una chiamata a guile!!!
// 
//                     uint64_t frame_id = 0;
//                     detection_data_t detection_data;
//                     do {
//                         if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                             break;
// 
//                         detection_data = detection_data_t();
// #ifdef ZED_STEREO
//                         if ( use_zed_camera ) {
//                             static int numF = remove_frames;
//                             while ( numF-- >= 0 ) {
//                                 bool bret=false;
//                                 while (!bret)
//                                 {
//                                     //faccio 10 tentativi di lettura, poi riapro il flusso verso la telecamera
//                                     for (int i=0; i<10; ++i)
//                                     {
//                                         if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                                         {
//                                             detection_data.exit_flag=true;
//                                             goto fine_run;
//                                         }
//                                         bret = CameraRead(mydetector->zed, detection_data.cap_frame);
//                                         if ( bret )
//                                         {
//                                             //la callback in tempo reale
//                                             if (rt_call_callback)
//                                                 call_rt_callback(&detection_data.cap_frame, video_fps, frame_size.width, frame_size.height, rt_callback);
//                                             break;
//                                         }
//                                         std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                                     }
//                                     if (!bret)
//                                     {
//                                         std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                                         OpenOrReopen(mydetector->zed, init_params);
//                                     }
//                                 }
//                                 detection_data.zed_cloud = zed_capture_3d_orig( mydetector->zed );
//                                 if ( show_original ) {
//                                     cv::imshow ( "original", detection_data.cap_frame );
//                                     cv::waitKey ( 1 );
//                                 }
//                             }
//                             numF = remove_frames;
//                         } else
// #endif   // ZED_STEREO
//                         {
//                             //cam normale o file normale
//                             static int numF = remove_frames;
//                             while ( numF-- >= 0 ) {
//                                 bool bret=false;
//                                 while (!bret)
//                                 {
//                                     for (int i=0; i<10; ++i)
//                                     {
//                                         if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                                         {
//                                             detection_data.exit_flag=true;
//                                             goto fine_run;
//                                         }
//                                         bret = CameraRead(mydetector->cap, detection_data.cap_frame);
//                                         if ( bret )
//                                         {
//                                             //la callback in tempo reale
//                                             if (rt_call_callback)
//                                                 call_rt_callback(&detection_data.cap_frame, video_fps, frame_size.width, frame_size.height, rt_callback);
//                                             break;
//                                         }
//                                         std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                                     }
//                                     if (!bret)
//                                     {
//                                         std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                                         (numcam>=0)?OpenOrReopen(mydetector->cap, numcam):OpenOrReopen(mydetector->cap, filename);
//                                     }
//                                 }
//                                 if ( show_original ) {
//                                     cv::imshow ( "original", detection_data.cap_frame );
//                                     cv::waitKey ( 1 );
//                                 }
//                             }
//                             numF = remove_frames;
//                         }
// 
// fine_run:
// //#define APPLY_BACKGROUND 1
// #define USE_CUDA_MOG2 1
// #if APPLY_BACKGROUND
//                         {
//                             static double learning_rate = 0.01;
//                             static int morph_size = 1;
//                             static int morph_type = cv::MORPH_ELLIPSE;
//                             static bool first_open = true;
//                             static bool do_open = true;
//                             static bool do_close = false;
// 
//                             //int64 start = cv::getTickCount();
// 
//                             static Ptr<BackgroundSubtractor> bg_subtractor = nullptr;
// #ifdef USE_CUDA_MOG2
//                             cv::cuda::GpuMat d_fgmask;
//                             cv::cuda::GpuMat d_fgimg;
//                             cv::cuda::GpuMat d_bgimg;
//                             cv::cuda::GpuMat d_frame;
// #endif
//                             cv::Mat fgmask;
//                             cv::Mat fgimg;
//                             cv::Mat bgimg;
// 
//                             if (bg_subtractor == nullptr)
//                             {
// #ifdef USE_CUDA_MOG2
//                                 bg_subtractor = cv::cuda::createBackgroundSubtractorMOG2();
// #else
//                                 bg_subtractor = cv::createBackgroundSubtractorKNN();
// #endif
//                             }
// 
// #ifdef USE_CUDA_MOG2
//                             d_frame=cv::cuda::GpuMat(detection_data.cap_frame);
//                             bg_subtractor->apply(d_frame, d_fgmask); //, (learning_rate>0.00001)?(learning_rate/=1.01):learning_rate);
// 
//                             bg_subtractor->getBackgroundImage(d_bgimg);
// 
//                             d_fgimg.create(d_frame.size(), d_frame.type());
//                             d_fgimg.setTo(Scalar::all(0));
// 
//                             //toglie le ombre
//                             cv::cuda::threshold(d_fgmask, d_fgmask, 128, 255, cv::THRESH_BINARY);
// 
//                             //cv::MORPH_RECT, cv::MORPH_CROSS, cv::MORPH_ELLIPSE
//                             cv::Mat sElem =  getStructuringElement( morph_type, Size( 2 * morph_size + 1, 2 * morph_size + 1 ) );
//                             Ptr<cv::cuda::Filter> openFilter = cv::cuda::createMorphologyFilter(cv::MORPH_OPEN, d_fgmask.type(), sElem);
//                             Ptr<cv::cuda::Filter> closeFilter = cv::cuda::createMorphologyFilter(cv::MORPH_CLOSE, d_fgmask.type(), sElem);
// 
//                             if (first_open)
//                             {
//                                 if (do_open)
//                                     openFilter->apply(d_fgmask, d_fgmask);
//                                 if (do_close)
//                                     closeFilter->apply(d_fgmask, d_fgmask);
//                             } else {
//                                 if (do_close)
//                                     closeFilter->apply(d_fgmask, d_fgmask);
//                                 if (do_open)
//                                     openFilter->apply(d_fgmask, d_fgmask);
//                             }
// 
// 
//                             d_frame.copyTo(d_fgimg, d_fgmask);
// 
//                             d_fgimg.download(fgimg);
//                             d_fgmask.download(fgmask);
// 
//                             if (!d_bgimg.empty())
//                                 d_bgimg.download(bgimg);
// #else
//                             bg_subtractor->apply(detection_data.cap_frame, fgmask, (learning_rate>0.00001)?(learning_rate/=1.01):learning_rate);
//                             cout << learning_rate << endl;
// 
//                             bg_subtractor->getBackgroundImage(bgimg);
// 
//                             fgimg.create(detection_data.cap_frame.size(), detection_data.cap_frame.type());
//                             fgimg.setTo(Scalar::all(0));
// 
//                             //Toglie le ombre: Parallel execution using C++11 lambda
//                             fgmask.forEach<uint8_t>([](uint8_t &p, const int * position) -> void {
//                                 if (p != 255 && p != 0) p=0;
//                             });
// 
//                             //cv::MORPH_RECT, cv::MORPH_CROSS, cv::MORPH_ELLIPSE
//                             cv::Mat sElem =  getStructuringElement( morph_type, Size( 2 * morph_size + 1, 2 * morph_size + 1 ) );
// 
//                             if (first_open)
//                             {
//                                 if (do_open)
//                                     cv::morphologyEx(fgmask, fgmask, cv::MORPH_OPEN, sElem, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT, morphologyDefaultBorderValue());
//                                 if (do_close)
//                                     cv::morphologyEx(fgmask, fgmask, cv::MORPH_CLOSE, sElem, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT, morphologyDefaultBorderValue());
//                             }
//                             else
//                             {
//                                 if (do_close)
//                                     cv::morphologyEx(fgmask, fgmask, cv::MORPH_CLOSE, sElem, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT, morphologyDefaultBorderValue());
//                                 if (do_open)
//                                     cv::morphologyEx(fgmask, fgmask, cv::MORPH_OPEN, sElem, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT, morphologyDefaultBorderValue());
//                             }
// 
//                             detection_data.cap_frame.copyTo(fgimg, fgmask);
// #endif
// 
// //                         double fps = cv::getTickFrequency() / (cv::getTickCount() - start);
// //                         std::cout << "FPS : " << fps << std::endl;
// 
// 
//                             cv::imshow("image", detection_data.cap_frame);
//                             cv::imshow("foreground mask", fgmask);
//                             cv::imshow("foreground image", fgimg);
//                             if (!bgimg.empty())
//                                 cv::imshow("mean background image", bgimg);
// 
// 
// //    32 | !  33 | "  34 | #  35 | $  36 | %  37 | &  38 | '  39 | (  40 | )  41 | *  42 | +  43 | ,  44 | -  45 | .  46 | /  47 |
// // 0  48 | 1  49 | 2  50 | 3  51 | 4  52 | 5  53 | 6  54 | 7  55 | 8  56 | 9  57 | :  58 | ;  59 | <  60 | =  61 | >  62 | ?  63 |
// // @  64 | A  65 | B  66 | C  67 | D  68 | E  69 | F  70 | G  71 | H  72 | I  73 | J  74 | K  75 | L  76 | M  77 | N  78 | O  79 |
// // P  80 | Q  81 | R  82 | S  83 | T  84 | U  85 | V  86 | W  87 | X  88 | Y  89 | Z  90 | [  91 | \  92 | ]  93 | ^  94 | _  95 |
// // `  96 | a  97 | b  98 | c  99 | d 100 | e 101 | f 102 | g 103 | h 104 | i 105 | j 106 | k 107 | l 108 | m 109 | n 110 | o 111 |
// // p 112 | q 113 | r 114 | s 115 | t 116 | u 117 | v 118 | w 119 | x 120 | y 121 | z 122 | { 123 | | 124 | } 125 | ~ 126 |
// 
//                             auto help = [&] () {
//                                 cout << "w increase morph_size" << endl;
//                                 cout << "s decrease morph_size" << endl;
//                                 cout << "e increase morph_type (0,1,2)" << endl;
//                                 cout << "d decrease morph_type (0,1,2)" << endl;
//                                 cout << "r first open"<< endl;
//                                 cout << "f first close" << endl;
//                                 cout << "t do open"<< endl;
//                                 cout << "g do'nt open"<< endl;
//                                 cout << "y do close" << endl;
//                                 cout << "h do'nt close" << endl;
//                             };
// //                         static int morph_size = 1;
// //                         static int morph_type = cv::MORPH_RECT;
// //                         static bool first_open = true;
// //                         static bool do_open = true;
// //                         static bool do_close = true;
// 
//                             int c = cv::waitKey(1);
//                             switch (c)
//                             {
//                             case 119: //w
//                                 morph_size++;
//                                 cout << "morph_size = " << morph_size << endl;
//                                 help();
//                                 break;
//                             case 115: //s
//                                 morph_size--;
//                                 cout << "morph_size = " << morph_size << endl;
//                                 help();
//                                 break;
//                             case 101: //e
//                                 morph_type++;
//                                 cout << "morph_type= " << morph_type<< endl;
//                                 help();
//                                 break;
//                             case 100: //d
//                                 morph_type--;
//                                 cout << "morph_type= " << morph_type<< endl;
//                                 help();
//                                 break;
//                             case 114: //r
//                                 first_open = true;
//                                 cout << "first_open=true" << endl;
//                                 help();
//                                 break;
//                             case 102: //f
//                                 first_open = false;
//                                 cout << "first_open=false" << endl;
//                                 help();
//                                 break;
//                             case 116: //t
//                                 do_open = true;
//                                 cout << "do_open=true" << endl;
//                                 help();
//                                 break;
//                             case 103: //g
//                                 do_open = false;
//                                 cout << "do_open=false" << endl;
//                                 help();
//                                 break;
//                             case 121: //y
//                                 do_close = true;
//                                 cout << "do_close=true" << endl;
//                                 help();
//                                 break;
//                             case 104: //h
//                                 do_close= false;
//                                 cout << "do_close=false" << endl;
//                                 help();
//                                 break;
//                             default:
//                                 ; //help();
//                             }
//                         }
// #endif
//                         fps_cap_counter++;
//                         detection_data.frame_id = frame_id++;
//                         detection_data.microsecs = micros();
//                         if ( detection_data.exit_flag || mydetector->stop || detection_data.cap_frame.empty() ) {
//                             detection_data.exit_flag = true;
//                             detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                             send_one_replaceable_object_t::to_be_stopped=true;
//                         }
// 
//                         //se richiesto di ridimensionare l'immagine, lo fa
//                         if (active_rect_tlx != -1 && active_rect_tly != -1 && active_rect_w != -1 && active_rect_h != -1)
//                         {
//                             detection_data.cap_frame = detection_data.cap_frame(cv::Rect(active_rect_tlx, active_rect_tly, active_rect_w, active_rect_h));
//                             frame_size = detection_data.cap_frame.size();
//                         }
// 
//                         if ( !detection_sync ) {
//                             soro_cap2draw.send ( detection_data );    // skip detection
//                         }
//                         soro_cap2prepare.send ( detection_data );
//                         if ( mydetector->stop || detection_data.exit_flag ) {
//                             detection_data.exit_flag = true;
//                         }
//                     } while ( !detection_data.exit_flag );
//                     std::cout << " t_cap exit \n";
// 
//                     //per sicurezza ripeto le assegnazioni!
//                     please_stop = true;
//                     send_one_replaceable_object_t::to_be_stopped=true;
//                     detection_data.exit_flag = true;
//                     detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                 } );
// 
//                 // pre-processing video frame (resize, convertion)
//                 t_prepare = std::thread ( [&]() {
//                     std::shared_ptr<image_t> det_image;
//                     detection_data_t detection_data;
//                     do {
//                         if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                             break;
// 
//                         detection_data = soro_cap2prepare.receive();
// 
//                         if ( detection_data.exit_flag || mydetector->stop || please_stop ) {
//                             goto endrun;
//                         }
// 
//                         det_image = detector->mat_to_image_resize ( detection_data.cap_frame );
//                         detection_data.det_image = det_image;
// 
//                         soro_prepare2detect.send ( detection_data ); // detection
// endrun:
//                         if ( detection_data.exit_flag || mydetector->stop ) {
//                             please_stop = true;
//                             send_one_replaceable_object_t::to_be_stopped=true;
//                             detection_data.exit_flag = true;
//                         }
//                     } while ( !detection_data.exit_flag );
//                     please_stop = true;
//                     detection_data.exit_flag = true;
//                     detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                     send_one_replaceable_object_t::to_be_stopped=true;
//                     std::cout << " t_prepare exit \n";
//                 } );
// 
//                 //detection by Yolo
//                 t_detect = std::thread ( [&]() {
//                     std::shared_ptr<image_t> det_image;
//                     detection_data_t detection_data;
//                     do {
//                         std::vector<bbox_t> result_vec;
//                         if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                             break;
// 
//                         detection_data = soro_prepare2detect.receive();
//                         if ( detection_data.exit_flag || mydetector->stop) {
//                             goto endrun;
//                         }
// 
//                         det_image = detection_data.det_image;
// 
//                         if ( det_image ) {
//                             result_vec = detector->detect_resized ( *det_image, frame_size.width, frame_size.height, thresh, true );    // true
//                         }
//                         fps_det_counter++;
//                         //std::this_thread::sleep_for(std::chrono::milliseconds(150));
// 
//                         detection_data.new_detection = true;
//                         detection_data.result_vec = result_vec;
//                         soro_detect2draw.send ( detection_data );
// endrun:
//                         if ( detection_data.exit_flag || mydetector->stop ) {
//                             send_one_replaceable_object_t::to_be_stopped=true;
//                             detection_data.exit_flag = true;
//                         }
//                     } while ( !detection_data.exit_flag );
//                     please_stop = true;
//                     detection_data.exit_flag = true;
//                     detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                     send_one_replaceable_object_t::to_be_stopped=true;
//                     std::cout << " t_detect exit \n";
//                 } );
// 
//                 //draw rectangles (and track objects)
//                 t_draw = std::thread ( [&]() {
//                     std::queue<cv::Mat> track_optflow_queue;
//                     detection_data_t detection_data;
//                     do {
//                         if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                             break;
// 
//                         // for Video-file
//                         cv::Mat cap_frame;
//                         cv::Mat draw_frame;
//                         cv::Mat old_cap_frame;
//                         std::vector<bbox_t> old_result_vec;
//                         std::vector<bbox_t> result_vec;
//                         if ( please_stop )
//                             goto endrun;
//                         if ( detection_sync ) {
//                             detection_data = soro_detect2draw.receive();
//                         }
//                         // for Video-camera
//                         else {
//                             // get new Detection result if present
//                             if ( soro_detect2draw.is_object_present() ) {
//                                 old_cap_frame = detection_data.cap_frame;   // use old captured frame
//                                 detection_data = soro_detect2draw.receive();
//                                 if ( !old_cap_frame.empty() ) {
//                                     detection_data.cap_frame = old_cap_frame;
//                                 }
//                             }
//                             // get new Captured frame
//                             else {
//                                 old_result_vec = detection_data.result_vec; // use old detections
//                                 detection_data = soro_cap2draw.receive();
//                                 detection_data.result_vec = old_result_vec;
//                             }
//                         }
//                         cap_frame = detection_data.cap_frame;
//                         if ( detection_data.exit_flag || mydetector->stop ) {
//                             goto endrun;
//                         }
// 
//                         if ( show_windows ) {
//                             draw_frame = detection_data.cap_frame.clone();
//                         }
// 
//                         result_vec = detection_data.result_vec;
// #ifdef TRACK_OPTFLOW
//                         try {
//                             if ( detection_data.new_detection ) {
//                                 tracker_flow.update_tracking_flow ( detection_data.cap_frame, detection_data.result_vec );
//                                 while ( track_optflow_queue.size() > 0 ) {
//                                     draw_frame = track_optflow_queue.back();
//                                     result_vec = tracker_flow.tracking_flow ( track_optflow_queue.front(), false );
//                                     track_optflow_queue.pop();
//                                 }
//                             } else {
//                                 track_optflow_queue.push ( cap_frame.clone() );
//                                 result_vec = tracker_flow.tracking_flow ( cap_frame, false );
//                             }
//                         } catch ( exception ex ) {
//                             cerr << "1: " << ex.what() << endl;
//                             detection_data.exit_flag = true;
//                             mydetector->stop=true;
//                             goto endrun;
//                         }
//                         detection_data.new_detection = true;    // to correct kalman filter
// #endif //TRACK_OPTFLOW
//                         // track ID by using kalman filter
//                         if ( use_kalman_filter ) {
//                             if ( detection_data.new_detection ) {
//                                 result_vec = track_kalman.correct ( result_vec );
//                             } else {
//                                 result_vec = track_kalman.predict();
//                             }
//                         }
//                         // track ID by using custom function
//                         else {
//                             int frame_story = std::max ( 5, current_fps_cap.load() );
//                             result_vec = detector->tracking_id ( result_vec, true, frame_story, 40 );
//                         }
// 
//                         if ( use_zed_camera && !detection_data.zed_cloud.empty() ) {
//                             //result_vec   = get_3d_coordinates ( result_vec, detection_data.zed_cloud, detection_data.cap_frame);
//                             result_vec   = get_3d_coordinates_orig( result_vec, detection_data.zed_cloud);
//                         }
// 
//                         //ogni quanti frame prendo i corner?
//                         static int steppedhere = 0;
//                         if ( get_corners && ( steppedhere++ == corn_period ) ) {
//                             steppedhere = 0; //ricomincio
//                             static cv::cuda::GpuMat d_vPts;
//                             static cv::Ptr<cv::cuda::CornersDetector> cdetector;
//                             static bool first_time = true;
//                             static cv::Mat frame_gray;
// 
//                             if ( first_time ) {
//                                 cv::cvtColor ( detection_data.cap_frame, frame_gray, cv::COLOR_BGR2GRAY );
//                                 cdetector = cv::cuda::createGoodFeaturesToTrackDetector ( frame_gray.type(), max_count, corn_qlev, corn_mindist );
//                                 first_time = false;
//                             } else {
//                                 cv::cvtColor ( detection_data.cap_frame, frame_gray, cv::COLOR_BGR2GRAY );
//                             }
// 
//                             cv::cuda::GpuMat curmat ( frame_gray );
// 
//                             vpoints_old = vpoints_new;
//                             vpoints_new.clear();
//                             cdetector->detect ( curmat, d_vPts );
//                             if ( d_vPts.cols > 0 ) {
//                                 download ( d_vPts, vpoints_new );
//                                 cv::cornerSubPix ( frame_gray, vpoints_new, w_corn_winsize, cv::Size ( -1,-1 ), termcrit );
// 
//                                 if ( corn_3dcoord ) {
//                                     //ora che ho i corner, posso trovare le distanze dei punti
//                                     if ( use_zed_camera && !detection_data.zed_cloud.empty() && vpoints_new.size() > 0 ) {
//                                         tredvec.resize ( vpoints_new.size() );
//                                         for ( int i=0; i<vpoints_new.size(); ++i ) {
//                                             tredvec[i].x=vpoints_new[i].x;
//                                             tredvec[i].y=vpoints_new[i].y;
//                                             tredvec[i].w = corn_3dsquare;
//                                             tredvec[i].h = corn_3dsquare;
//                                         }
//                                         //tredvec = get_3d_coordinates ( tredvec, detection_data.zed_cloud, detection_data.cap_frame );
//                                         tredvec = get_3d_coordinates_orig( tredvec, detection_data.zed_cloud);
//                                     }
//                                 }
//                             }
//                         }
// 
//                         if ( show_windows ) {
//                             for ( int i=0; i<vpoints_new.size(); ++i ) {
//                                 if ( tredvec.size() > 0 ) {
//                                     putText ( draw_frame, to_string ( tredvec[i].z_3d ), vpoints_new[i], cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar ( 0, 0, 255 ), 2 );
//                                 } else {
//                                     putText ( draw_frame, "#", vpoints_new[i], cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar ( 0, 0, 255 ), 2 );
//                                 }
//                             }
// 
//                             //small_preview.set(draw_frame, result_vec);
//                             //large_preview.set(draw_frame, result_vec);
//                             draw_boxes ( draw_frame, result_vec, obj_names, current_fps_det, current_fps_cap, showAllClasses, cltosh );
//                             //show_console_result(result_vec, obj_names, detection_data.frame_id);
//                             //large_preview.draw(draw_frame);
//                             //small_preview.draw(draw_frame, true);
//                         }
//                         detection_data.draw_frame = draw_frame;
//                         detection_data.result_vec = result_vec;
//                         soro_draw2show.send ( detection_data );
//                         if ( send_network ) {
//                             soro_draw2net.send ( detection_data );
//                         }
// endrun:
//                         if ( mydetector->stop || detection_data.exit_flag ) {
//                             please_stop = true;
//                             send_one_replaceable_object_t::to_be_stopped=true;
//                             detection_data.exit_flag = true;
//                         }
//                     } while ( !detection_data.exit_flag );
//                     std::cout << " t_draw exit \n";
//                     please_stop = true;
//                     detection_data.exit_flag = true;
//                     detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                     send_one_replaceable_object_t::to_be_stopped=true;
//                 } );
// 
//                 // send detection to the network
//                 t_network = std::thread ( [&]() {
//                     scm_init_guile();
//                     static bool first_step = true;
//                     if ( send_network ) {
//                         detection_data_t detection_data;
//                         do {
//                             if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                                 break;
// 
//                             detection_data = soro_draw2net.receive();
//                             bool bret;
//                             if ( detection_data.exit_flag || mydetector->stop ) {
//                                 goto endrun;
//                             }
// 
//                             bret = send_json_http ( detection_data.cap_frame, detection_data.result_vec, obj_names, detection_data.frame_id, vpoints_new, vpoints_old, tredvec, i_callback, runlisp, detection_data.microsecs, showAllClasses, cltosh, filename );
//                             if ( !bret ) {
//                                 mydetector->stop=true;
//                             }
// 
//                             if ( first_step ) {
//                                 first_step = false;
//                                 cout << "server ready" << endl;
//                             }
// 
// endrun:
//                             if ( mydetector->stop || detection_data.exit_flag ) {
//                                 please_stop = true;
//                                 send_one_replaceable_object_t::to_be_stopped=true;
//                                 detection_data.exit_flag = true;
//                             }
//                         } while ( !detection_data.exit_flag );
//                         please_stop = true;
//                         detection_data.exit_flag = true;
//                         detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                         send_one_replaceable_object_t::to_be_stopped=true;
//                     }
//                     std::cout << " t_network exit \n";
//                 } );
// 
//                 // show detection
//                 detection_data_t detection_data;
//                 do {
//                     if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                         break;
// 
//                     steady_end = std::chrono::steady_clock::now();
//                     float time_sec = std::chrono::duration<double> ( steady_end - steady_start ).count();
//                     if ( time_sec >= 1 ) {
//                         current_fps_det = fps_det_counter.load() / time_sec;
//                         current_fps_cap = fps_cap_counter.load() / time_sec;
//                         steady_start = steady_end;
//                         fps_det_counter = 0;
//                         fps_cap_counter = 0;
//                     }
//                     cv::Mat draw_frame;
// 
//                     detection_data = soro_draw2show.receive();
//                     if ( detection_data.exit_flag || mydetector->stop ) {
//                         break;
//                     }
// 
//                     draw_frame = detection_data.draw_frame;
// 
//                     //if (extrapolate_flag) {
//                     //    cv::putText(draw_frame, "extrapolate", cv::Point2f(10, 40), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(50, 50, 0), 2);
//                     //}
// 
//                     if ( show_windows ) {
//                         cv::imshow ( "computed", draw_frame );
// 
//                         int key = cv::waitKey ( 1 ); // 3 or 16ms
//                         if ( key == 27 || key == 113) {
//                             exit_flag = true;
//                         }
//                     } else {
//                         std::this_thread::sleep_for ( std::chrono::milliseconds ( 16 ) );
//                     }
// 
// //                     //Ricevuto da un signal!!!
// //                     if ( stop_please ) {
// //                         exit_flag = true;
// //                     }
// 
//                     //std::cout << " current_fps_det = " << current_fps_det << ", current_fps_cap = " << current_fps_cap << std::endl;
// endrun:
//                     if ( mydetector->stop || exit_flag ) {
//                         send_one_replaceable_object_t::to_be_stopped=true;
//                         detection_data.exit_flag = true;
//                         mydetector->stop = true;
//                     }
//                 } while ( !detection_data.exit_flag );
//                 std::cout << " show detection exit \n";
// 
//                 // wait for all threads
//                 if ( t_prepare.joinable() ) {
//                     cerr << "Join t_prepare\n";
//                     t_prepare.join();
//                 }
//                 if ( t_detect.joinable() ) {
//                     cerr << "Join t_detect\n";
//                     t_detect.join();
//                 }
// //                 if ( t_post.joinable() ) {
// //                     t_post.join();
// //                 }
//                 if ( t_draw.joinable() ) {
//                     cerr << "Join t_draw\n";
//                     t_draw.join();
//                 }
//                 if ( t_network.joinable() ) {
//                     cerr << "Join t_network\n";
//                     t_network.join();
//                 }
//                 if ( t_cap.joinable() ) {
//                     cerr << "Join t_cap\n";
//                     t_cap.join();
//                 }
// 
//                 if ( show_windows )
//                     try {
//                         cv::destroyWindow ( "computed" );
//                     } catch ( cv::Exception ex ) {
//                         cerr << "2: " << ex.what() << endl;
//                     }
//                 if ( show_original )
//                     try {
//                         cv::destroyWindow ( "original" );
//                     } catch ( cv::Exception ex ) {
//                         cerr << "3: " << ex.what() << endl;
//                     }
//                 break;
// 
//             } else if ( file_ext == "txt" ) { // list of image files
//                 std::ifstream file ( filename );
//                 if ( !file.is_open() ) {
//                     std::cout << "File not found! \n";
//                 } else
//                     for ( std::string line; file >> line; ) {
//                         std::cout << line << std::endl;
//                         cv::Mat mat_img = cv::imread ( line );
//                         std::vector<bbox_t> result_vec = detector->detect ( mat_img );
//                         show_console_result ( result_vec, obj_names );
//                         //draw_boxes(mat_img, result_vec, obj_names);
//                         //cv::imwrite("res_" + line, mat_img);
//                     }
// 
//             } else {  // image file
//                 // to achive high performance for multiple images do these 2 lines in another thread
//                 cv::Mat mat_img = cv::imread ( filename );
//                 auto det_image = detector->mat_to_image_resize ( mat_img );
// 
//                 auto start = std::chrono::steady_clock::now();
//                 std::vector<bbox_t> result_vec = detector->detect_resized ( *det_image, mat_img.size().width, mat_img.size().height );
//                 auto end = std::chrono::steady_clock::now();
//                 std::chrono::duration<double> spent = end - start;
//                 std::cout << " Time: " << spent.count() << " sec \n";
// 
//                 result_vec = detector->tracking_id ( result_vec ); // comment it - if track_id is not required
// 
//                 vpoints_old.clear();
//                 tredvec.clear();
//                 vpoints_new.clear();
//                 if ( get_corners ) {
//                     cv::cuda::GpuMat d_vPts;
//                     cv::Ptr<cv::cuda::CornersDetector> cdetector;
//                     cv::Mat frame_gray;
//                     cv::cvtColor ( mat_img, frame_gray, cv::COLOR_BGR2GRAY );
//                     cdetector = cv::cuda::createGoodFeaturesToTrackDetector ( frame_gray.type(), max_count, corn_qlev, corn_mindist );
//                     cv::cvtColor ( mat_img, frame_gray, cv::COLOR_BGR2GRAY );
//                     cv::cuda::GpuMat curmat ( frame_gray );
//                     cdetector->detect ( curmat, d_vPts );
//                     if ( d_vPts.cols > 0 ) {
//                         download ( d_vPts, vpoints_new );
//                         cv::cornerSubPix ( frame_gray, vpoints_new, w_corn_winsize, cv::Size ( -1,-1 ), termcrit );
//                     }
//                 }
// 
//                 send_json_http ( mat_img, result_vec, obj_names, 0, vpoints_new, vpoints_old, tredvec, i_callback, runlisp, micros(), showAllClasses, cltosh, filename );
// 
//                 if ( show_windows ) {
//                     draw_boxes ( mat_img, result_vec, obj_names, -1, -1, showAllClasses, cltosh );
//                     cv::imshow ( "computed", mat_img );
//                     show_console_result ( result_vec, obj_names );
//                     cv::waitKey ( 0 );
//                     try {
//                         cv::destroyWindow ( "computed" );
//                     } catch ( cv::Exception ex ) {
//                         cerr << "4: " << ex.what() << endl;
//                     }
//                 }
//                 break;
//             }
// #else   // OPENCV
//             //std::vector<bbox_t> result_vec = detector.detect(filename);
// 
//             auto img = detector->load_image ( filename );
//             std::vector<bbox_t> result_vec = detector->detect ( img );
//             detector->free_image ( img );
//             show_console_result ( result_vec, obj_names );
// #endif  // OPENCV
//         } catch ( std::exception &ex ) {
//             cerr << "5: " << ex.what() << endl;
//         } catch ( ... ) {
//             std::cerr << "6: unknown exception \n";
//         }
//         filename.clear();
//     }
// 
// fine_run:
//     if ( mydetector->zed.isOpened() )
//         mydetector->zed.close();
// 
//     if (mydetector->cap.isOpened())
//         mydetector->cap.release();
// 
// 
//     send_one_replaceable_object_t::to_be_stopped=true;
//     mydetector->terminated=true;
//     mydetector->stop=false;
//     mydetector->running = false;
//     return SCM_BOOL_T;
// }
// 
// #else
// 
// static SCM mtfa_detector_run ( SCM scm_detector, SCM par_list )
// {
//     scm_init_guile();  //faccio una chiamata a guile!!!
// 
//     //i parametri con i loro default
//     string names;
//     string video="0";
//     double threshold = 0.2;
//     bool isStationary = true;
//     SCM callback;  //by default, -1 : nulla
//     SCM rt_callback;
//     bool windows = true;
//     string classes = "all";
//     int height = 0;
//     int width = 0;
//     string zed_resolution = "HD720";
//     string zed_depth_mode = "ULTRA";
//     int flow_winsize = 15;
//     int flow_iterations = 8000;
//     bool get_corners = false;
//     int corn_winsize = 10;
//     int corn_period = 5;
//     int max_count = 1000;
//     double corn_qlev = 0.01;
//     double corn_mindist = 10;
//     int corn_critcount = 20;
//     double corn_accuracy = 0.03;
//     bool corn_3dcoord = false;
//     int corn_3dsquare = 20;
//     bool show_original=false;
//     int remove_frames = 0;
//     bool detection_sync =false;
//     string zed_camera_ip; // = "0.0.0.0:30000";
//     int active_rect_tlx = -1;
//     int active_rect_tly = -1;
//     int active_rect_w = -1;
//     int active_rect_h = -1;
// 
//     if ( scm_detector == SCM_UNDEFINED || par_list == SCM_UNDEFINED || scm_assoc_ref ( par_list, scm_from_latin1_string ( "help" ) ) != SCM_BOOL_F ) {
//         cout <<
//              "First parameter must be the result of a (mtfa-opencv-detector-make cfg weights). The other are in alist format and are the following: " << endl <<
//              "Minimum parameters are: (mtfa-opencv-detector-run det '((\"names\" . \"../darknet/data/coco.names\")(\"video\" . \"test.mp4\")))" << endl << endl <<
//              "{help            |                | this message }" << endl <<
//              "{names           |                | file containing names }" << endl <<
// //         "{cfg             |                | file containing the configuration }" << endl <<
// //         "{weights         |                | file containing the weights }" << endl <<
//              "{video           | 0              | movie, image, device or camera number }" << endl <<
//              "{threshold       | 0.2            | threshold of detection }" << endl <<
//              "{isStationary    | true           | camera is (true/false) stationary }" << endl <<
//              "{callback        | -1/port/fname  | do not send detections data, send detections data to tcp port, call the function fname with two pars: img, json-detections}" << endl <<
//              "{rt_callback     | SCM_EOL/fname  | do not send real time data detected, call the function fname with one pars: img}" << endl <<
//              "{windows         | true           | show windows (true/false) during detection }" << endl <<
//              "{classes         | all            | list of the classes to show(\"class\",\"class\",\"class\",\"class\") or \"none\"}" << endl <<
//              "{height          |                | Height in pixel to resize the frames}" << endl <<
//              "{width           |                | Width in pixel to resize the frames}" << endl <<
//              "{zed_resolution  | HD720          | Zed camera resolution (HD720, HD1080, HD2K, VGA}" << endl <<
//              "{zed_depth_mode  | ULTRA          | Zed camera depth mode (NONE, PERFORMANCE, QUALITY, ULTRA)}" << endl <<
//              "{flow_winsize    | 15             | the size (9, 15, 21, 31) of the SparsePyrLKOpticalFlow algorithm}" << endl <<
//              "{flow_iterations | 8000           | the iterations of the SparsePyrLKOpticalFlow algorithm}" << endl <<
//              "{get_corners     | false          | evaluate good corners in the image}" << endl <<
//              "{corn_winsize    | 10             | good corners win size}" << endl <<
//              "{corn_period     | 5              | good corners taken every 5 frame}" << endl <<
//              "{max_count       | 1000           | good corners, max number of features to track }" << endl <<
//              "{corn_qlev       | 0.01           | good corners quality level}" << endl <<
//              "{corn_mindist    | 10             | good corners min distance}" << endl <<
//              "{corn_critcount  | 20             | good corners termcriteria count}" << endl <<
//              "{corn_accuracy   | 0.03           | good corners termcriteria accuracy}" << endl <<
//              "{corn_3dcoord    | false          | good corners get 3d coordinates}" << endl <<
//              "{corn_3dsquare   | 20             | good corners the square inside to take 3d coords}" << endl <<
//              "{show_original   | false          | show the original video/frame }" << endl <<
//              "{remove_frames   | 0              | how many frames to remove }" << endl <<
//              "{detection_sync  | false          | should be trues for movies }" << endl <<
//              "{zed_camera_ip   | empty          | for receiving ZED streams. IE: 0.0.0.0:3000}" << endl <<
//              "{active_rect_tlx | -1             | use whole image}" << endl <<
//              "{active_rect_tly | -1             | use whole image}" << endl <<
//              "{active_rect_w   | -1             | use whole image}" << endl <<
//              "{active_rect_h   | -1             | use whole image}" << endl <<
//              endl << "All given in alist format\n";
//         return SCM_BOOL_T;
//     }
// 
//     if ( mtfa_detector_p ( scm_detector ) == SCM_BOOL_F ) {
//         scm_wrong_type_arg_msg ( "mtfa-detector-run", 0, scm_detector, "mtfa-detector type" );
//     }
// 
//     MyDetector* mydetector = ( MyDetector* ) scm_foreign_object_signed_ref ( scm_detector, 0 );
//     lock_guard<mutex> lck ( mydetector->mtx );
// 
//     mydetector->stop = false;
//     mydetector->running = true;
//     Detector * detector = mydetector->detector;
// 
//     SETVAL ( names );
//     SETVAL ( video );
//     SETVAL ( threshold );
//     SETVAL ( isStationary );
//     SETVAL ( callback );
//     SETVAL ( rt_callback );
//     SETVAL ( windows );
//     SETVAL ( classes );
//     SETVAL ( height );
//     SETVAL ( width );
//     SETVAL ( zed_resolution );
//     SETVAL ( zed_depth_mode );
//     SETVAL ( flow_winsize );
//     SETVAL ( flow_iterations );
//     SETVAL ( get_corners );
//     SETVAL ( corn_winsize );
//     SETVAL ( corn_period );
//     SETVAL ( max_count );
//     SETVAL ( corn_qlev );
//     SETVAL ( corn_mindist );
//     SETVAL ( corn_critcount );
//     SETVAL ( corn_accuracy );
//     SETVAL ( corn_3dcoord );
//     SETVAL ( corn_3dsquare );
//     SETVAL ( show_original );
//     SETVAL ( remove_frames );
//     SETVAL ( detection_sync );
//     SETVAL ( zed_camera_ip );
//     SETVAL ( active_rect_tlx );
//     SETVAL ( active_rect_tly );
//     SETVAL ( active_rect_w );
//     SETVAL ( active_rect_h);
// 
//     if ( show_original ) {
//         cv::namedWindow ( "original", 0 );
//     }
//     if ( windows ) {
//         cv::namedWindow ( "computed", 0 );
//     }
// 
//     //Se la callback è un numero, allora inviamo via http su questa porta, altrimenti chiamo la funzione lisp
//     int i_callback;
//     SCM runlisp;
//     cout << "CB  : " << callback << endl;
//     if ( scm_is_integer ( callback ) ) {
//         i_callback = scm_to_int ( callback );
//         cout << i_callback  << endl;
//         runlisp = SCM_EOL;
//     } else {
//         i_callback = -1;
//         runlisp = callback;
//     }
// 
//     bool rt_call_callback;
//     if (scm_is_true(scm_procedure_p(rt_callback)))
//     {
//         cout << "Will call the rt callback\n";
//         rt_call_callback = true;
//     }
//     else {
//         cout << "Will not call the rt callback\n";
//         rt_call_callback = false;
//     }
// 
//     string names_file = names;
// 
//     float thresh = threshold;
//     bool show_windows = windows;
//     bool showAllClasses = true;
//     string classesToShow = classes;
// 
//     sl::RESOLUTION camera_resolution;
//     string s_camera_resolution = zed_resolution;
//     if ( s_camera_resolution == "HD720" ) {
//         camera_resolution = sl::RESOLUTION::HD720;
//     } else if ( s_camera_resolution == "HD1080" ) {
//         camera_resolution = sl::RESOLUTION::HD1080;
//     } else if ( s_camera_resolution == "HD2K" ) {
//         camera_resolution = sl::RESOLUTION::HD2K;
//     } else if ( s_camera_resolution == "VGA" ) {
//         camera_resolution = sl::RESOLUTION::VGA;
//     } else {
//         camera_resolution = sl::RESOLUTION::HD720;
//     }
// 
//     sl::DEPTH_MODE camera_depth_mode;
//     string s_camera_depth_mode = zed_depth_mode;
//     if ( s_camera_depth_mode == "NONE" ) {
//         camera_depth_mode = sl::DEPTH_MODE::NONE;
//     } else if ( s_camera_depth_mode == "PERFORMANCE" ) {
//         camera_depth_mode = sl::DEPTH_MODE::PERFORMANCE;
//     } else if ( s_camera_depth_mode == "QUALITY" ) {
//         camera_depth_mode = sl::DEPTH_MODE::QUALITY;
//     } else if ( s_camera_depth_mode == "ULTRA" ) {
//         camera_depth_mode = sl::DEPTH_MODE::ULTRA;
//     } else {
//         camera_depth_mode = sl::DEPTH_MODE::PERFORMANCE;
//     }
// 
//     int sparse_win_size = flow_winsize;
//     int sparse_iterations = flow_iterations;
// 
//     cv::Size w_corn_winsize ( corn_winsize, corn_winsize );
//     cv::TermCriteria termcrit ( cv::TermCriteria::COUNT|cv::TermCriteria::EPS,corn_critcount, corn_accuracy );
// 
//     if ( height || width ) {
//         cout << "Images will be resized to " << height << "x" << width << endl;
//     }
// 
//     unordered_set<string> cltosh; //classes to show
//     showAllClasses = true;
//     if ( classesToShow.compare ( "all" ) == 0 ) {
//         showAllClasses = true;
//     } else {
//         //prende tutte le classi che sono tra le virgole
//         cerr << "Classi da inserire: " << classesToShow << endl;
//         showAllClasses = false;
//         typedef vector<string> T_V_Split;
//         T_V_Split mtfa_string_split ( const string& str, const string& delimiters );
//         T_V_Split mtfa_string_split2 ( const string& str, const string& delimiter );
//         string mtfa_replace_matching_pattern ( const string & data, const string & pattern, const string & replace, bool all );
//         auto vs = mtfa_string_split (
//                       mtfa_replace_matching_pattern (
//                           mtfa_replace_matching_pattern (
//                               mtfa_replace_matching_pattern ( classesToShow, "\"", "", true ),
//                               ", ", ",", true ),
//                           " ,", ",", true ),
//                       "," );
//         for ( auto it: vs ) {
//             cerr << "Inserisco classe: " << it << endl;
//             cltosh.insert ( it );
//         }
//     }
// 
//     //In ogni caso non salvo su disco!
//     std::string filename;
//     int numcam = -1;
//     if ( isdigit ( video[0] ) ) {
//         numcam = stoi ( video );
//         filename = "web_camera";
//     } else {
//         filename = video;
//     }
// 
//     //I punti significativi dell'immagine,quando attivati
//     vector<cv::Point2f> vpoints_old;
//     vector<cv::Point2f> vpoints_new;
//     std::vector<bbox_t> tredvec;
// 
//     auto obj_names = objects_names_from_file ( names_file );
//     bool const send_network = ( i_callback != -1 || runlisp != SCM_EOL );      // true - for remote detection
//     bool const use_kalman_filter = isStationary; //false;   // true - for stationary camera
// 
//     //bool detection_sync = true;             // true - for video-file, assigned by parameters
// 
//     Tracker_optflow tracker_flow ( detector->cur_gpu_id, sparse_win_size, 1, sparse_iterations );
// 
//     //creo un array di colori
//     vector<cv::Scalar> colors;
//     cv::RNG rng;
//     for ( int i = 0; i < 100; i++ ) {
//         int r = rng.uniform ( 0, 256 );
//         int g = rng.uniform ( 0, 256 );
//         int b = rng.uniform ( 0, 256 );
//         colors.push_back ( cv::Scalar ( r,g,b ) );
//     }
// 
//     //al posto delle goto ecc..
//     auto clean_and_return = [&]() {
//         if ( mydetector->zed.isOpened() )
//             mydetector->zed.close();
// 
//         if (mydetector->cap.isOpened())
//             mydetector->cap.release();
// 
// 
//         send_one_replaceable_object_t::to_be_stopped=true;
//         mydetector->terminated=true;
//         mydetector->stop=false;
//         mydetector->running = false;
//         return SCM_BOOL_T;
//     };
// 
//     //Iniziano i lavori!!
//     cv::Mat cur_frame;
//     std::atomic<int> fps_cap_counter ( 0 ), fps_det_counter ( 0 );
//     std::atomic<int> current_fps_cap ( 0 ), current_fps_det ( 0 );
//     std::atomic<bool> exit_flag ( false );
//     std::chrono::steady_clock::time_point steady_start, steady_end;
//     int video_fps = 25;
//     bool use_zed_camera = false;
// 
//     track_kalman_t track_kalman;
// 
//     sl::InitParameters init_params;
//     init_params.depth_minimum_distance = 2;
//     init_params.depth_maximum_distance = 40;
//     init_params.depth_mode = camera_depth_mode; // sl::DEPTH_MODE::PERFORMANCE;
//     init_params.camera_resolution = camera_resolution; // sl::RESOLUTION::HD720; //sl::RESOLUTION::HD720;// sl::RESOLUTION::HD1080, sl::RESOLUTION::HD720
//     init_params.coordinate_units = sl::UNIT::METER;
//     init_params.sdk_gpu_id = -1; //detector.cur_gpu_id;
// 
//     while ( true ) {
//         try {
//             std::string const file_ext = filename.substr ( filename.find_last_of ( "." ) + 1 );
//             std::string const protocol = filename.substr ( 0, 7 );
//             if ( file_ext == "avi" || file_ext == "mp4" || file_ext == "mjpg" || file_ext == "mov" ||    // video file
//                     protocol == "rtmp://" || protocol == "rtsp://" || protocol == "http://" || protocol == "https:/" ||    // video network stream
//                     filename == "zed_camera" || file_ext == "svo" || filename == "web_camera" )  // ZED stereo camera
//             {
//                 if (zed_camera_ip.size() > 0)
//                     setStreamParameter(init_params, zed_camera_ip);
//                 cout << "Streaming from " << zed_camera_ip << endl;
// 
// 
//                 if ( filename == "zed_camera" || file_ext == "svo" )
//                 {
//                     if ( file_ext == "svo" )
//                         init_params.input.setFromSVOFile ( filename.c_str() );
// 
//                     use_zed_camera = true;
//                 } else {
//                     use_zed_camera = false;
//                 }
// 
//                 //apre la camera
//                 while (true)
//                 {
//                     if (mydetector->stop)
//                         return clean_and_return();
// 
//                     bool bret;
// 
//                     if (use_zed_camera)
//                     {
//                         if (!OpenOrReopen(mydetector->zed, init_params))
//                         {
//                             std::cout << "Error: ZED Camera should be connected to USB 3.0. And ZED_SDK should be installed.\n";
//                             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//                         }
//                         else
//                         {
//                             std::cout << "ZED 3D Camera opened." << std::endl;
//                             break;
//                         }
//                     } else {
//                         if ( numcam >= 0 )
//                             bret = OpenOrReopen(mydetector->cap, numcam);
//                         else
//                             bret = OpenOrReopen(mydetector->cap, filename);
// 
//                         if (!bret)
//                         {
//                             std::cout << "Error: Camera/file should be connected/existing.\n";
//                             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//                         }
//                         else
//                             break;
//                     }
//                 }
// 
//                 //camera aperta, proseguo
//                 while (true)
//                 {
//                     if (mydetector->stop)
//                         return clean_and_return();
//                     if (use_zed_camera)
//                     {
//                         // Print camera information
//                         auto camera_info = mydetector->zed.getCameraInformation();
//                         cout << "ZED Model                 : " << camera_info.camera_model << endl;
//                         cout << "ZED Serial Number         : " << camera_info.serial_number << endl;
//                         cout << "ZED Camera Firmware       : " << camera_info.camera_configuration.firmware_version << "/" << camera_info.sensors_configuration.firmware_version << endl;
//                         cout << "ZED Camera Resolution     : " << camera_info.camera_configuration.resolution.width << "x" << camera_info.camera_configuration.resolution.height << endl;
//                         cout << "ZED Camera FPS            : " << mydetector->zed.getInitParameters().camera_fps << endl;
// 
//                         //legge il primo frame
//                         if ( !CameraRead(mydetector->zed, cur_frame)) {
//                             if (!mydetector->zed.isOpened())
//                             {
//                                 OpenOrReopen(mydetector->zed, init_params);
//                                 std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//                             }
//                             else
//                                 std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                         }
//                         else
//                             break;
//                     } else {
//                         if ( !CameraRead(mydetector->cap, cur_frame) ) {
//                             if (!mydetector->cap.isOpened())
//                             {
//                                 if ( numcam >= 0 )
//                                     OpenOrReopen(mydetector->cap, numcam);
//                                 else
//                                     OpenOrReopen(mydetector->cap, filename);
//                                 std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                             }
//                             else
//                                 std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                         }
//                         else
//                             break;
//                     }
//                 }
// 
//                 if (use_zed_camera)
//                 {
//                     video_fps = mydetector->zed.getInitParameters().camera_fps;
//                 }
//                 else
//                 {
//                     video_fps = mydetector->cap.get ( cv::CAP_PROP_FPS );
//                 }
// 
//                 cv::Size frame_size = cur_frame.size();
//                 cout << "\n Video size: " << frame_size << endl;
//                 cout << "starting" << endl;
// 
//                 send_one_replaceable_object_t soro_cap2prepare ( detection_sync ), soro_cap2draw ( detection_sync ), soro_prepare2detect ( detection_sync ), soro_detect2draw ( detection_sync), soro_draw2show ( detection_sync ), soro_draw2net ( detection_sync );
// 
//                 std::thread t_cap, t_prepare, t_detect, t_draw, t_network; //t_post
// 
//                 bool please_stop = false;
// 
//                 send_one_replaceable_object_t::to_be_stopped=false;
// 
//                 uint64_t frame_id = 0;
// 
//                 auto read_frame_from_camera = [&] (detection_data_t & detection_data) {
//                     scm_init_guile();
//                     int toremove = remove_frames;
//                     do {
//                         bool bret=false;
//                         while (!bret)
//                         {
//                             //faccio 10 tentativi di lettura, poi riapro il flusso verso la telecamera
//                             for (int i=0; i<10; ++i)
//                             {
//                                 if ( please_stop )
//                                 {
//                                     detection_data.exit_flag=true;
//                                     return false;
//                                 }
//                                 if (use_zed_camera)
//                                 {
//                                     if ((bret = CameraRead(mydetector->zed, detection_data.cap_frame)))
//                                         break;
//                                 } else {
//                                     if ((bret = CameraRead(mydetector->cap, detection_data.cap_frame)))
//                                         break;
//                                 }
//                                 std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                             }
//                             if (!bret)
//                             {
//                                 std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                                 if (use_zed_camera)
//                                     OpenOrReopen(mydetector->zed, init_params);
//                                 else
//                                     (numcam>=0)?OpenOrReopen(mydetector->cap, numcam):OpenOrReopen(mydetector->cap, filename);
//                             }
//                         }
//                         if (bret)
//                         {
//                             //la callback in tempo reale
//                             if (rt_call_callback)
//                                 call_rt_callback(&detection_data.cap_frame, video_fps, frame_size.width, frame_size.height, rt_callback);
//                         }
//                     } while (toremove-->0);
// 
//                     if (use_zed_camera)
//                         detection_data.zed_cloud = zed_capture_3d_orig( mydetector->zed );
//                     return true;
//                 };
// 
// 
//                 //capture new video-frame
//                 t_cap = std::thread ( [&]() {
//                     detection_data_t detection_data;
//                     do {
//                         if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                             break;
// 
//                         detection_data = detection_data_t();
//                         if (!read_frame_from_camera(detection_data)) {
//                             detection_data.exit_flag=true;
//                         } else {
//                             if ( show_original ) {
//                                 cv::imshow ( "original", detection_data.cap_frame );
//                                 cv::waitKey ( 1 );
//                             }
//                         }
// 
//                         fps_cap_counter++;
//                         detection_data.frame_id = frame_id++;
//                         detection_data.microsecs = micros();
//                         if ( detection_data.exit_flag || mydetector->stop || detection_data.cap_frame.empty() ) {
//                             detection_data.exit_flag = true;
//                             detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                             send_one_replaceable_object_t::to_be_stopped=true;
//                         }
// 
//                         //se richiesto di ridimensionare l'immagine, lo fa
//                         if (active_rect_tlx != -1 && active_rect_tly != -1 && active_rect_w != -1 && active_rect_h != -1)
//                         {
//                             detection_data.cap_frame = detection_data.cap_frame(cv::Rect(active_rect_tlx, active_rect_tly, active_rect_w, active_rect_h));
//                             frame_size = detection_data.cap_frame.size();
//                         }
// 
//                         if ( !detection_sync ) {
//                             soro_cap2draw.send ( detection_data );    // skip detection
//                         }
//                         soro_cap2prepare.send ( detection_data );
//                         if ( mydetector->stop || detection_data.exit_flag ) {
//                             detection_data.exit_flag = true;
//                         }
//                     } while ( !detection_data.exit_flag );
//                     std::cout << " t_cap exit \n";
// 
//                     //per sicurezza ripeto le assegnazioni!
//                     please_stop = true;
//                     send_one_replaceable_object_t::to_be_stopped=true;
//                     detection_data.exit_flag = true;
//                     detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                 } );
// 
//                 // pre-processing video frame (resize, convertion)
//                 t_prepare = std::thread ( [&]() {
//                     std::shared_ptr<image_t> det_image;
//                     detection_data_t detection_data;
//                     do {
//                         if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                             break;
// 
//                         detection_data = soro_cap2prepare.receive();
// 
//                         if ( detection_data.exit_flag || mydetector->stop || please_stop ) {
//                             goto endrun;
//                         }
// 
//                         det_image = detector->mat_to_image_resize ( detection_data.cap_frame );
//                         detection_data.det_image = det_image;
// 
//                         soro_prepare2detect.send ( detection_data ); // detection
// endrun:
//                         if ( detection_data.exit_flag || mydetector->stop ) {
//                             please_stop = true;
//                             send_one_replaceable_object_t::to_be_stopped=true;
//                             detection_data.exit_flag = true;
//                         }
//                     } while ( !detection_data.exit_flag );
//                     please_stop = true;
//                     detection_data.exit_flag = true;
//                     detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                     send_one_replaceable_object_t::to_be_stopped=true;
//                     std::cout << " t_prepare exit \n";
//                 } );
// 
//                 //detection by Yolo
//                 t_detect = std::thread ( [&]() {
//                     std::shared_ptr<image_t> det_image;
//                     detection_data_t detection_data;
//                     do {
//                         std::vector<bbox_t> result_vec;
//                         if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                             break;
// 
//                         detection_data = soro_prepare2detect.receive();
//                         if ( detection_data.exit_flag || mydetector->stop) {
//                             goto endrun;
//                         }
// 
//                         det_image = detection_data.det_image;
// 
//                         if ( det_image ) {
//                             result_vec = detector->detect_resized ( *det_image, frame_size.width, frame_size.height, thresh, true );    // true
//                         }
//                         fps_det_counter++;
//                         //std::this_thread::sleep_for(std::chrono::milliseconds(150));
// 
//                         detection_data.new_detection = true;
//                         detection_data.result_vec = result_vec;
//                         soro_detect2draw.send ( detection_data );
// endrun:
//                         if ( detection_data.exit_flag || mydetector->stop ) {
//                             send_one_replaceable_object_t::to_be_stopped=true;
//                             detection_data.exit_flag = true;
//                         }
//                     } while ( !detection_data.exit_flag );
//                     please_stop = true;
//                     detection_data.exit_flag = true;
//                     detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                     send_one_replaceable_object_t::to_be_stopped=true;
//                     std::cout << " t_detect exit \n";
//                 } );
// 
//                 //draw rectangles (and track objects)
//                 t_draw = std::thread ( [&]() {
//                     std::queue<cv::Mat> track_optflow_queue;
//                     detection_data_t detection_data;
//                     do {
//                         if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                             break;
// 
//                         // for Video-file
//                         cv::Mat cap_frame;
//                         cv::Mat draw_frame;
//                         cv::Mat old_cap_frame;
//                         std::vector<bbox_t> old_result_vec;
//                         std::vector<bbox_t> result_vec;
//                         if ( please_stop )
//                             goto endrun;
//                         if ( detection_sync ) {
//                             detection_data = soro_detect2draw.receive();
//                         }
//                         // for Video-camera
//                         else {
//                             // get new Detection result if present
//                             if ( soro_detect2draw.is_object_present() ) {
//                                 old_cap_frame = detection_data.cap_frame;   // use old captured frame
//                                 detection_data = soro_detect2draw.receive();
//                                 if ( !old_cap_frame.empty() ) {
//                                     detection_data.cap_frame = old_cap_frame;
//                                 }
//                             }
//                             // get new Captured frame
//                             else {
//                                 old_result_vec = detection_data.result_vec; // use old detections
//                                 detection_data = soro_cap2draw.receive();
//                                 detection_data.result_vec = old_result_vec;
//                             }
//                         }
//                         cap_frame = detection_data.cap_frame;
//                         if ( detection_data.exit_flag || mydetector->stop ) {
//                             goto endrun;
//                         }
// 
//                         if ( show_windows ) {
//                             draw_frame = detection_data.cap_frame.clone();
//                         }
// 
//                         result_vec = detection_data.result_vec;
// 
//                         try {
//                             if ( detection_data.new_detection ) {
//                                 tracker_flow.update_tracking_flow ( detection_data.cap_frame, detection_data.result_vec );
//                                 while ( track_optflow_queue.size() > 0 ) {
//                                     draw_frame = track_optflow_queue.back();
//                                     result_vec = tracker_flow.tracking_flow ( track_optflow_queue.front(), false );
//                                     track_optflow_queue.pop();
//                                 }
//                             } else {
//                                 track_optflow_queue.push ( cap_frame.clone() );
//                                 result_vec = tracker_flow.tracking_flow ( cap_frame, false );
//                             }
//                         } catch ( exception ex ) {
//                             cerr << "1: " << ex.what() << endl;
//                             detection_data.exit_flag = true;
//                             mydetector->stop=true;
//                             goto endrun;
//                         }
//                         detection_data.new_detection = true;    // to correct kalman filter
// 
//                         // track ID by using kalman filter
//                         if ( use_kalman_filter ) {
//                             if ( detection_data.new_detection ) {
//                                 result_vec = track_kalman.correct ( result_vec );
//                             } else {
//                                 result_vec = track_kalman.predict();
//                             }
//                         }
//                         // track ID by using custom function
//                         else {
//                             int frame_story = std::max ( 5, current_fps_cap.load() );
//                             result_vec = detector->tracking_id ( result_vec, true, frame_story, 40 );
//                         }
// 
//                         if ( use_zed_camera && !detection_data.zed_cloud.empty() ) {
//                             //result_vec   = get_3d_coordinates ( result_vec, detection_data.zed_cloud, detection_data.cap_frame);
//                             result_vec   = get_3d_coordinates_orig( result_vec, detection_data.zed_cloud);
//                         }
// 
//                         //ogni quanti frame prendo i corner?
//                         static int steppedhere = 0;
//                         if ( get_corners && ( steppedhere++ == corn_period ) ) {
//                             steppedhere = 0; //ricomincio
//                             static cv::cuda::GpuMat d_vPts;
//                             static cv::Ptr<cv::cuda::CornersDetector> cdetector;
//                             static bool first_time = true;
//                             static cv::Mat frame_gray;
// 
//                             if ( first_time ) {
//                                 cv::cvtColor ( detection_data.cap_frame, frame_gray, cv::COLOR_BGR2GRAY );
//                                 cdetector = cv::cuda::createGoodFeaturesToTrackDetector ( frame_gray.type(), max_count, corn_qlev, corn_mindist );
//                                 first_time = false;
//                             } else {
//                                 cv::cvtColor ( detection_data.cap_frame, frame_gray, cv::COLOR_BGR2GRAY );
//                             }
// 
//                             cv::cuda::GpuMat curmat ( frame_gray );
// 
//                             vpoints_old = vpoints_new;
//                             vpoints_new.clear();
//                             cdetector->detect ( curmat, d_vPts );
//                             if ( d_vPts.cols > 0 ) {
//                                 download ( d_vPts, vpoints_new );
//                                 cv::cornerSubPix ( frame_gray, vpoints_new, w_corn_winsize, cv::Size ( -1,-1 ), termcrit );
// 
//                                 if ( corn_3dcoord ) {
//                                     //ora che ho i corner, posso trovare le distanze dei punti
//                                     if ( use_zed_camera && !detection_data.zed_cloud.empty() && vpoints_new.size() > 0 ) {
//                                         tredvec.resize ( vpoints_new.size() );
//                                         for ( int i=0; i<vpoints_new.size(); ++i ) {
//                                             tredvec[i].x=vpoints_new[i].x;
//                                             tredvec[i].y=vpoints_new[i].y;
//                                             tredvec[i].w = corn_3dsquare;
//                                             tredvec[i].h = corn_3dsquare;
//                                         }
//                                         //tredvec = get_3d_coordinates ( tredvec, detection_data.zed_cloud, detection_data.cap_frame );
//                                         tredvec = get_3d_coordinates_orig( tredvec, detection_data.zed_cloud);
//                                     }
//                                 }
//                             }
//                         }
// 
//                         if ( show_windows ) {
//                             for ( int i=0; i<vpoints_new.size(); ++i ) {
//                                 if ( tredvec.size() > 0 ) {
//                                     putText ( draw_frame, to_string ( tredvec[i].z_3d ), vpoints_new[i], cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar ( 0, 0, 255 ), 2 );
//                                 } else {
//                                     putText ( draw_frame, "#", vpoints_new[i], cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar ( 0, 0, 255 ), 2 );
//                                 }
//                             }
// 
//                             //small_preview.set(draw_frame, result_vec);
//                             //large_preview.set(draw_frame, result_vec);
//                             draw_boxes ( draw_frame, result_vec, obj_names, current_fps_det, current_fps_cap, showAllClasses, cltosh );
//                             //show_console_result(result_vec, obj_names, detection_data.frame_id);
//                             //large_preview.draw(draw_frame);
//                             //small_preview.draw(draw_frame, true);
//                         }
//                         detection_data.draw_frame = draw_frame;
//                         detection_data.result_vec = result_vec;
//                         soro_draw2show.send ( detection_data );
//                         if ( send_network ) {
//                             soro_draw2net.send ( detection_data );
//                         }
// endrun:
//                         if ( mydetector->stop || detection_data.exit_flag ) {
//                             please_stop = true;
//                             send_one_replaceable_object_t::to_be_stopped=true;
//                             detection_data.exit_flag = true;
//                         }
//                     } while ( !detection_data.exit_flag );
//                     std::cout << " t_draw exit \n";
//                     please_stop = true;
//                     detection_data.exit_flag = true;
//                     detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                     send_one_replaceable_object_t::to_be_stopped=true;
//                 } );
// 
//                 // send detection to the network
//                 t_network = std::thread ( [&]() {
//                     scm_init_guile();
//                     static bool first_step = true;
//                     if ( send_network ) {
//                         detection_data_t detection_data;
//                         do {
//                             if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                                 break;
// 
//                             detection_data = soro_draw2net.receive();
//                             bool bret;
//                             if ( detection_data.exit_flag || mydetector->stop ) {
//                                 goto endrun;
//                             }
// 
//                             bret = send_json_http ( detection_data.cap_frame, detection_data.result_vec, obj_names, detection_data.frame_id, vpoints_new, vpoints_old, tredvec, i_callback, runlisp, detection_data.microsecs, showAllClasses, cltosh, filename );
//                             if ( !bret ) {
//                                 mydetector->stop=true;
//                             }
// 
//                             if ( first_step ) {
//                                 first_step = false;
//                                 cout << "server ready" << endl;
//                             }
// 
// endrun:
//                             if ( mydetector->stop || detection_data.exit_flag ) {
//                                 please_stop = true;
//                                 send_one_replaceable_object_t::to_be_stopped=true;
//                                 detection_data.exit_flag = true;
//                             }
//                         } while ( !detection_data.exit_flag );
//                         please_stop = true;
//                         detection_data.exit_flag = true;
//                         detection_data.cap_frame = cv::Mat ( frame_size, CV_8UC3 );
//                         send_one_replaceable_object_t::to_be_stopped=true;
//                     }
//                     std::cout << " t_network exit \n";
//                 } );
// 
//                 // show detection
//                 detection_data_t detection_data;
//                 do {
//                     if ( please_stop || send_one_replaceable_object_t::to_be_stopped)
//                         break;
// 
//                     steady_end = std::chrono::steady_clock::now();
//                     float time_sec = std::chrono::duration<double> ( steady_end - steady_start ).count();
//                     if ( time_sec >= 1 ) {
//                         current_fps_det = fps_det_counter.load() / time_sec;
//                         current_fps_cap = fps_cap_counter.load() / time_sec;
//                         steady_start = steady_end;
//                         fps_det_counter = 0;
//                         fps_cap_counter = 0;
//                     }
//                     cv::Mat draw_frame;
// 
//                     detection_data = soro_draw2show.receive();
//                     if ( detection_data.exit_flag || mydetector->stop ) {
//                         break;
//                     }
// 
//                     draw_frame = detection_data.draw_frame;
// 
//                     //if (extrapolate_flag) {
//                     //    cv::putText(draw_frame, "extrapolate", cv::Point2f(10, 40), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(50, 50, 0), 2);
//                     //}
// 
//                     if ( show_windows ) {
//                         cv::imshow ( "computed", draw_frame );
// 
//                         int key = cv::waitKey ( 1 ); // 3 or 16ms
//                         if ( key == 27 || key == 113) {
//                             exit_flag = true;
//                         }
//                     } else {
//                         std::this_thread::sleep_for ( std::chrono::milliseconds ( 16 ) );
//                     }
// 
// //                     //Ricevuto da un signal!!!
// //                     if ( stop_please ) {
// //                         exit_flag = true;
// //                     }
// 
//                     //std::cout << " current_fps_det = " << current_fps_det << ", current_fps_cap = " << current_fps_cap << std::endl;
// endrun:
//                     if ( mydetector->stop || exit_flag ) {
//                         send_one_replaceable_object_t::to_be_stopped=true;
//                         detection_data.exit_flag = true;
//                         mydetector->stop = true;
//                     }
//                 } while ( !detection_data.exit_flag );
//                 std::cout << " show detection exit \n";
// 
//                 // wait for all threads
//                 if ( t_prepare.joinable() ) {
//                     cerr << "Join t_prepare\n";
//                     t_prepare.join();
//                 }
//                 if ( t_detect.joinable() ) {
//                     cerr << "Join t_detect\n";
//                     t_detect.join();
//                 }
// //                 if ( t_post.joinable() ) {
// //                     t_post.join();
// //                 }
//                 if ( t_draw.joinable() ) {
//                     cerr << "Join t_draw\n";
//                     t_draw.join();
//                 }
//                 if ( t_network.joinable() ) {
//                     cerr << "Join t_network\n";
//                     t_network.join();
//                 }
//                 if ( t_cap.joinable() ) {
//                     cerr << "Join t_cap\n";
//                     t_cap.join();
//                 }
// 
//                 if ( show_windows )
//                     try {
//                         cv::destroyWindow ( "computed" );
//                     } catch ( cv::Exception ex ) {
//                         cerr << "2: " << ex.what() << endl;
//                     }
//                 if ( show_original )
//                     try {
//                         cv::destroyWindow ( "original" );
//                     } catch ( cv::Exception ex ) {
//                         cerr << "3: " << ex.what() << endl;
//                     }
//                 break;
// 
//             } else if ( file_ext == "txt" ) { // list of image files
//                 std::ifstream file ( filename );
//                 if ( !file.is_open() ) {
//                     std::cout << "File not found! \n";
//                 } else
//                     for ( std::string line; file >> line; ) {
//                         std::cout << line << std::endl;
//                         cv::Mat mat_img = cv::imread ( line );
//                         std::vector<bbox_t> result_vec = detector->detect ( mat_img );
//                         show_console_result ( result_vec, obj_names );
//                         //draw_boxes(mat_img, result_vec, obj_names);
//                         //cv::imwrite("res_" + line, mat_img);
//                     }
// 
//             } else {  // image file
//                 // to achive high performance for multiple images do these 2 lines in another thread
//                 cv::Mat mat_img = cv::imread ( filename );
//                 auto det_image = detector->mat_to_image_resize ( mat_img );
// 
//                 auto start = std::chrono::steady_clock::now();
//                 std::vector<bbox_t> result_vec = detector->detect_resized ( *det_image, mat_img.size().width, mat_img.size().height );
//                 auto end = std::chrono::steady_clock::now();
//                 std::chrono::duration<double> spent = end - start;
//                 std::cout << " Time: " << spent.count() << " sec \n";
// 
//                 result_vec = detector->tracking_id ( result_vec ); // comment it - if track_id is not required
// 
//                 vpoints_old.clear();
//                 tredvec.clear();
//                 vpoints_new.clear();
//                 if ( get_corners ) {
//                     cv::cuda::GpuMat d_vPts;
//                     cv::Ptr<cv::cuda::CornersDetector> cdetector;
//                     cv::Mat frame_gray;
//                     cv::cvtColor ( mat_img, frame_gray, cv::COLOR_BGR2GRAY );
//                     cdetector = cv::cuda::createGoodFeaturesToTrackDetector ( frame_gray.type(), max_count, corn_qlev, corn_mindist );
//                     cv::cvtColor ( mat_img, frame_gray, cv::COLOR_BGR2GRAY );
//                     cv::cuda::GpuMat curmat ( frame_gray );
//                     cdetector->detect ( curmat, d_vPts );
//                     if ( d_vPts.cols > 0 ) {
//                         download ( d_vPts, vpoints_new );
//                         cv::cornerSubPix ( frame_gray, vpoints_new, w_corn_winsize, cv::Size ( -1,-1 ), termcrit );
//                     }
//                 }
// 
//                 send_json_http ( mat_img, result_vec, obj_names, 0, vpoints_new, vpoints_old, tredvec, i_callback, runlisp, micros(), showAllClasses, cltosh, filename );
// 
//                 if ( show_windows ) {
//                     draw_boxes ( mat_img, result_vec, obj_names, -1, -1, showAllClasses, cltosh );
//                     cv::imshow ( "computed", mat_img );
//                     show_console_result ( result_vec, obj_names );
//                     cv::waitKey ( 0 );
//                     try {
//                         cv::destroyWindow ( "computed" );
//                     } catch ( cv::Exception ex ) {
//                         cerr << "4: " << ex.what() << endl;
//                     }
//                 }
//                 break;
//             }
//         } catch ( std::exception &ex ) {
//             cerr << "5: " << ex.what() << endl;
//         } catch ( ... ) {
//             std::cerr << "6: unknown exception \n";
//         }
//         filename.clear();
//     }
// 
// fine_run:
//     if ( mydetector->zed.isOpened() )
//         mydetector->zed.close();
// 
//     if (mydetector->cap.isOpened())
//         mydetector->cap.release();
// 
// 
//     send_one_replaceable_object_t::to_be_stopped=true;
//     mydetector->terminated=true;
//     mydetector->stop=false;
//     mydetector->running = false;
//     return SCM_BOOL_T;
// }
// 
// 
// 
// #endif



/**************** ALTRE FUNZIONI OPENCV *************************/

static SCM mtfa_opencv_clone ( SCM scm_mat)
{
    cv::Mat * input = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    auto out = mtfa_internal_opencv_clone(input);
    return scm_from_pointer ( out, releaseMat );
}

//convert to jpeg, png, ... bytevector 
static SCM mtfa_opencv_imgmat_to_bytevector( SCM mat, SCM format )
{
    auto img = ( cv::Mat* ) scm_to_pointer ( mat );
    if ( img == nullptr ) {
        scm_wrong_type_arg_msg("mtfa_opencv_imgmat_to_bytevector", 0, mat, "a cv::Mat type");
    }

    if ( scm_is_string (format) != 1 ) {
        scm_wrong_type_arg_msg("mtfa_opencv_imgmat_to_bytevector", 1, format, "jpg, png, ...");
    }
    
    char * sformat = scm_to_latin1_string ( format );
    std::vector<uchar> buf;
    cv::imencode(sformat, *img, buf);
    free ( sformat );
    
    uchar * p = (uchar *)malloc(buf.size());
    std::memcpy(p, &buf[0], buf.size());
    
    SCM scm_ret = scm_pointer_to_bytevector (
            scm_from_pointer ( ( void* ) p, free ),
            scm_from_int ( buf.size()),
            scm_from_int ( 0 ),
            scm_string_to_symbol ( scm_from_locale_string( "u8" ) ) );

    return scm_ret;
}

static SCM mtfa_opencv_bytevector_to_imgmat( SCM scm_bv ) {
	if ( !scm_is_bytevector ( scm_bv ))
    {
		scm_wrong_type_arg_msg("mtfa_opencv_bytevector_to_imgmat", 0, scm_bv, "bytevector");
    }
    int len = SCM_BYTEVECTOR_LENGTH ( scm_bv );
    unsigned char * bv = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( scm_bv );
    
    cv::Mat * mat = new cv::Mat();
    *mat = cv::imdecode(vector<unsigned char>(bv, bv+len), IMREAD_COLOR); 
    return scm_from_pointer ( mat, releaseMat );
}

static SCM mtfa_opencv_imgfile_to_mat ( SCM imgfile )
{
    if ( scm_is_string ( imgfile ) == 1 ) {
        char * s = scm_to_latin1_string ( imgfile );
        if ( access ( s, F_OK ) !=0 ) {
            free ( s );
            return SCM_BOOL_F;
        }

        cv::Mat * input = new cv::Mat();
        *input = cv::imread ( s );
        free ( s );
        
        return scm_from_pointer ( input, releaseMat );
    }
    return SCM_BOOL_F;
}

static SCM mtfa_opencv_mat_to_imgfile ( SCM mat, SCM imgfile )
{
    if ( scm_is_string ( imgfile ) == 1 ) {
        char * s = scm_to_latin1_string ( imgfile );
        cv::Mat * input = ( cv::Mat* ) scm_to_pointer ( mat );
        cv::imwrite ( s, *input );
        free ( s );
        return SCM_BOOL_T;
    }
    return SCM_BOOL_F;
}

static SCM mtfa_opencv_mat_info ( SCM mat )
{
    try {
        cv::Mat * input = ( cv::Mat* ) scm_to_pointer ( mat );
        int cols = input->cols;
        int rows = input->rows;
        int t_pe = input->type();
        return scm_values_3 ( scm_from_int ( rows ), scm_from_int ( cols ), scm_from_int(t_pe) );
    } catch ( cv::Exception ex ) {
        cerr << "7: " << ex.what() << endl;
        return scm_values_3 ( SCM_BOOL_F, SCM_BOOL_F, SCM_BOOL_F  );
    }
}
static SCM mtfa_opencv_cap_info ( SCM scm_cap )
{
    try {
        auto cap = ( cv::VideoCapture* ) scm_to_pointer ( scm_cap );
        auto POS_MSEC = cap->get (CAP_PROP_POS_MSEC); //       =0, //!< Current position of the video file in milliseconds.
        auto POS_FRAMES = cap->get (CAP_PROP_POS_FRAMES); //     =1, //!< 0-based index of the frame to be decoded/captured next.
        auto POS_AVI_RATIO = cap->get(CAP_PROP_POS_AVI_RATIO); //	  =2, //!< Relative position of the video file: 0=start of the film, 1=end of the film.
        auto FRAME_WIDTH = cap->get(CAP_PROP_FRAME_WIDTH); //	    =3, //!< Width of the frames in the video stream.
        auto FRAME_HEIGHT = cap->get(CAP_PROP_FRAME_HEIGHT); //	   =4, //!< Height of the frames in the video stream.
        auto FPS = cap->get(CAP_PROP_FPS); //	            =5, //!< Frame rate.
        auto FOURCC = cap->get(CAP_PROP_FOURCC); //	         =6, //!< 4-character code of codec. see VideoWriter::fourcc .
        auto FRAME_COUNT = cap->get(CAP_PROP_FRAME_COUNT); //	    =7, //!< Number of frames in the video file.
//         auto BRIGHTNESS = cap->get(CAP_PROP_BRIGHTNESS);	    =10, //!< Brightness of the image (only for those cameras that support).
//         auto CONTRAST = cap->get(CAP_PROP_CONTRAST);	      =11, //!< Contrast of the image (only for cameras).
//         auto SATURATION = cap->get(CAP_PROP_SATURATION);	    =12, //!< Saturation of the image (only for cameras).
//         auto HUE = cap->get(CAP_PROP_HUE);	           =13, //!< Hue of the image (only for cameras).
//         auto GAIN = cap->get(CAP_PROP_GAIN);	          =14, //!< Gain of the image (only for those cameras that support).
//         auto EXPOSURE = cap->get(CAP_PROP_EXPOSURE);	      =15, //!< Exposure (only for those cameras that support).
//         auto CONVERT_RGB = cap->get(CAP_PROP_CONVERT_RGB);	   =16, //!< Boolean flags indicating whether images should be converted to RGB. <br/>
//         auto MONOCHROME = cap->get(CAP_PROP_MONOCHROME);	    =19,
//         auto SHARPNESS = cap->get(CAP_PROP_SHARPNESS);	     =20,
//         auto AUTO_EXPOSURE = cap->get(CAP_PROP_AUTO_EXPOSURE);	 =21, //!< DC1394: exposure control done by camera, user can adjust reference level using this feature.
//         auto GAMMA = cap->get(CAP_PROP_GAMMA);	         =22,
//         auto TEMPERATURE = cap->get(CAP_PROP_TEMPERATURE);	   =23,
//         auto TRIGGER = cap->get(CAP_PROP_TRIGGER);	       =24,
//         auto TRIGGER_DELAY = cap->get(CAP_PROP_TRIGGER_DELAY);	 =25,
//         auto WHITE_BALANCE_RED_V = cap->get(CAP_PROP_WHITE_BALANCE_RED_V);	 =26,
//         auto ZOOM = cap->get(CAP_PROP_ZOOM);	          =27,
//         auto FOCUS = cap->get(CAP_PROP_FOCUS);	         =28,
//         auto GUID = cap->get(CAP_PROP_GUID);	          =29,
//         auto ISO_SPEED = cap->get(CAP_PROP_ISO_SPEED);	     =30,
//         auto BACKLIGHT = cap->get(CAP_PROP_BACKLIGHT);	     =32,
//         auto PAN = cap->get(CAP_PROP_PAN);	           =33,
//         auto TILT = cap->get(CAP_PROP_TILT);	          =34,
//         auto ROLL = cap->get(CAP_PROP_ROLL);	          =35,
//         auto IRIS = cap->get(CAP_PROP_IRIS);	          =36,
//         auto SETTINGS = cap->get(CAP_PROP_SETTINGS);	      =37, //!< Pop up video/camera filter dialog (note: only supported by DSHOW backend currently. The property value is ignored)
//         auto BUFFERSIZE = cap->get(CAP_PROP_BUFFERSIZE);	    =38,
//         auto AUTOFOCUS = cap->get(CAP_PROP_AUTOFOCUS);	     =39,
//         auto SAR_NUM = cap->get(CAP_PROP_SAR_NUM);	       =40, //!< Sample aspect ratio: num/den (num)
//         auto SAR_DEN = cap->get(CAP_PROP_SAR_DEN);	       =41, //!< Sample aspect ratio: num/den (den)
//         auto BACKEND = cap->get(CAP_PROP_BACKEND);	       =42, //!< Current backend (enum VideoCaptureAPIs). Read-only property
//         auto CHANNEL = cap->get(CAP_PROP_CHANNEL);	       =43, //!< Video input or Channel Number (only for those cameras that support)
//         auto AUTO_WB = cap->get(CAP_PROP_AUTO_WB);	       =44, //!< enable/ disable auto white-balance
//         auto WB_TEMPERATURE=45, = cap->get(CAP_PROP_WB_TEMPERATURE=45,);	 //!< white-balance color temperature
//         auto CODEC_PIXEL_FORMAT = cap->get(CAP_PROP_CODEC_PIXEL_FORMAT);	 =46,    //!< (read-only) codec's pixel format. 4-character code - see VideoWriter::fourcc . Subset of [AV_PIX_FMT_*](https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/raw.c) or -1 if unknown
//         auto BITRATE = cap->get(CAP_PROP_BITRATE);	       =47, //!< (read-only) Video bitrate in kbits/s
//         auto ORIENTATION_META=48, = cap->get(CAP_PROP_ORIENTATION_META=48,);	 //!< (read-only) Frame rotation defined by stream meta (applicable for FFmpeg back-end only)
//         auto ORIENTATION_AUTO=49, = cap->get(CAP_PROP_ORIENTATION_AUTO=49,);	 //!< if true - rotates output frames of CvCapture considering video file's metadata  (applicable for FFmpeg back-end only) (https://github.com/opencv/opencv/issues/15499)


        return scm_cons ( scm_from_int ( POS_MSEC ),
                          scm_cons ( scm_from_int ( POS_FRAMES),
                                     scm_cons ( scm_from_int ( POS_AVI_RATIO),
                                                scm_cons ( scm_from_int ( FRAME_WIDTH),
                                                        scm_cons ( scm_from_int ( FRAME_HEIGHT),
                                                                scm_cons ( scm_from_int (FPS),
                                                                        scm_cons ( scm_from_int ( FOURCC),
                                                                                scm_cons ( scm_from_int ( FRAME_COUNT), SCM_EOL ))))))));
    } catch ( cv::Exception ex ) {
        return SCM_EOL;
    }
}

static SCM mtfa_opencv_img_hash_to_bytevector ( SCM scm_hashmat )
{
    try {
        auto hashmat = ( cv::Mat* ) scm_to_pointer ( scm_hashmat );
        //cv::Ptr<ImgHashBase> func = AverageHash::create();

        vector<uchar> vec;
        download ( *hashmat, vec );

        SCM u8 = scm_from_utf8_string ( "u8" );
        uchar * p = ( uchar* ) malloc ( vec.size() );
        memcpy ( p, &vec[0], vec.size() );
        SCM scm_ret = scm_pointer_to_bytevector (
                          scm_from_pointer ( ( void* ) p, free ),
                          scm_from_int ( vec.size() ),
                          scm_from_int ( 0 ),
                          scm_string_to_symbol ( u8 ) );
        return scm_ret;
    } catch ( exception ex ) {
        cerr << "8: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}

static SCM mtfa_opencv_bytevector_to_img_hash ( SCM scm_bv )
{
    try {
        if ( scm_is_bytevector ( scm_bv ) ) {
            //in questo caso la cifra avviene sull'oggetto, senza duplicazioni
            int len = SCM_BYTEVECTOR_LENGTH ( scm_bv );
            char * p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_bv );
            vector<uint8_t> v ( p, p+len );
            cv::Mat * ret = upload ( v );
            return scm_from_pointer ( ret, releaseMat );
        }
        /*
                auto hashmat = ( cv::Mat* ) scm_to_pointer ( scm_hashmat );
                cv::Ptr<ImgHashBase> func = AverageHash::create();

                vector<uchar> vec;
                download( *hashmat, vec);

                SCM u8 = scm_from_utf8_string ( "u8" );
                uchar * p = (uchar*)malloc(vec.size());
                memcpy(p, &vec[0], vec.size());
                SCM scm_ret = scm_pointer_to_bytevector (
                            scm_from_pointer ( ( void* ) p, free ),
                            scm_from_int ( vec.size() ),
                            scm_from_int ( 0 ),
                            scm_string_to_symbol ( u8 ) );
                return scm_ret;*/
    } catch ( exception ex ) {
        cerr << "9: " << ex.what() << endl;
    }
    return SCM_BOOL_F;
}

//Serializza o deserializza una Mat, ma ora non mi serve
/*
 *
void matwrite(const string& filename, const Mat& mat)
{
    ofstream fs(filename, fstream::binary);

    // Header
    int type = mat.type();
    int channels = mat.channels();
    fs.write((char*)&mat.rows, sizeof(int));    // rows
    fs.write((char*)&mat.cols, sizeof(int));    // cols
    fs.write((char*)&type, sizeof(int));        // type
    fs.write((char*)&channels, sizeof(int));    // channels

    // Data
    if (mat.isContinuous())
    {
        fs.write(mat.ptr<char>(0), (mat.dataend - mat.datastart));
    }
    else
    {
        int rowsz = CV_ELEM_SIZE(type) * mat.cols;
        for (int r = 0; r < mat.rows; ++r)
        {
            fs.write(mat.ptr<char>(r), rowsz);
        }
    }
}

Mat matread(const string& filename)
{
    ifstream fs(filename, fstream::binary);

    // Header
    int rows, cols, type, channels;
    fs.read((char*)&rows, sizeof(int));         // rows
    fs.read((char*)&cols, sizeof(int));         // cols
    fs.read((char*)&type, sizeof(int));         // type
    fs.read((char*)&channels, sizeof(int));     // channels

    // Data
    Mat mat(rows, cols, type);
    fs.read((char*)mat.data, CV_ELEM_SIZE(type) * rows * cols);

    return mat;
}

 * */

//Distanza di hamming e distanza di Levenshtein
SCM LevenshteinDistance(SCM i1, SCM i2);
SCM LevenshteinDistance(SCM i1, SCM i2)
{
    string s1, s2;
    if (scm_is_string ( i1 ) == 1) {
        //è una stringa
        size_t lent1, lent2;
        char * t1 = scm_to_locale_stringn(i1, &lent1);
        char * t2 = scm_to_locale_stringn(i2, &lent2);
        s1 = string(t1, t1+lent1);
        s2 = string(t2, t2+lent2);
        free(t1);
        free(t2);
    } else {
        //è un bytevector
        size_t lent1 = SCM_BYTEVECTOR_LENGTH(i1);
        size_t lent2 = SCM_BYTEVECTOR_LENGTH(i2);
        char * t1 = ( char* ) SCM_BYTEVECTOR_CONTENTS ( i1 );
        char * t2 = ( char* ) SCM_BYTEVECTOR_CONTENTS ( i2 );
        s1 = string(t1, t1+lent1);
        s2 = string(t2, t2+lent2);
    }
    
  const size_t m(s1.size()), n(s2.size());
 
  if( m==0 ) return scm_from_int(n);
  if( n==0 ) return scm_from_int(m);
 
  // allocation below is not ISO-compliant,
  // it won't work with -pedantic-errors.
  size_t costs[n + 1];
 
  for( size_t k=0; k<=n; k++ ) costs[k] = k;
 
  size_t i { 0 };
  for (char const &c1 : s1) 
  {
    costs[0] = i+1;
    size_t corner { i },
           j      { 0 };
    for (char const &c2 : s2)
    {
      size_t upper { costs[j+1] };
      if( c1 == c2 ) costs[j+1] = corner;
      else {
        size_t t(upper<corner? upper: corner);
        costs[j+1] = (costs[j]<t?costs[j]:t)+1;
      }
 
      corner = upper;
      j++;
    }
    i++;
  }
 
  return scm_from_int(costs[n]);
}

SCM HammingDistance(SCM i1, SCM i2);
SCM HammingDistance(SCM i1, SCM i2)
{
    string s1, s2;
    if (scm_is_string ( i1 ) == 1) {
        //è una stringa
        size_t lent1, lent2;
        char * t1 = scm_to_locale_stringn(i1, &lent1);
        char * t2 = scm_to_locale_stringn(i2, &lent2);
        s1 = string(t1, t1+lent1);
        s2 = string(t2, t2+lent2);
        free(t1);
        free(t2);
    } else {
        //è un bytevector
        size_t lent1 = SCM_BYTEVECTOR_LENGTH(i1);
        size_t lent2 = SCM_BYTEVECTOR_LENGTH(i2);
        char * t1 = ( char* ) SCM_BYTEVECTOR_CONTENTS ( i1 );
        char * t2 = ( char* ) SCM_BYTEVECTOR_CONTENTS ( i2 );
        s1 = string(t1, t1+lent1);
        s2 = string(t2, t2+lent2);
    }
    
  const size_t m(s1.size()), n(s2.size());
 
  if( m==0 ) return scm_from_int(n);
  if( n==0 ) return scm_from_int(m);
 
  // allocation below is not ISO-compliant,
  // it won't work with -pedantic-errors.
  size_t costs[n + 1];
 
  for( size_t k=0; k<=n; k++ ) costs[k] = k;
 
  size_t i { 0 };
  for (char const &c1 : s1) 
  {
    costs[0] = i+1;
    size_t corner { i },
           j      { 0 };
    for (char const &c2 : s2)
    {
      size_t upper { costs[j+1] };
      if( c1 == c2 ) costs[j+1] = corner;
      else {
        size_t t(upper<corner? upper: corner);
        costs[j+1] = (costs[j]<t?costs[j]:t)+1;
      }
 
      corner = upper;
      j++;
    }
    i++;
  }
 
  return scm_from_int(costs[n]);
}


//I vari hash che inizializziamo!!
static SCM mtfa_opencv_compute_img_hash_AverageHash ( SCM imgmat )
{
    try {
        cv::Mat *imgInput = ( cv::Mat* ) scm_to_pointer ( imgmat );
        cv::Mat *hashOutput=new cv::Mat();

        cv::Ptr<ImgHashBase> func = AverageHash::create();
        func->compute ( *imgInput, *hashOutput );
        return scm_from_pointer ( hashOutput, releaseMat );
    } catch ( exception ex ) {
        cerr << "10: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}

static SCM mtfa_opencv_compare_img_hash_AverageHash ( SCM scm_mat1, SCM scm_mat2 )
{
    cv::Mat *mat1, *mat2;
    mat1 = ( cv::Mat* ) scm_to_pointer ( scm_mat1 );
    mat2 = ( cv::Mat* ) scm_to_pointer ( scm_mat2 );

    try {
        Ptr<ImgHashBase> func = AverageHash::create();
        auto result = func->compare ( *mat1, *mat2 );
        return scm_from_double ( result );
    } catch ( exception ex ) {
        cerr << "11: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}

static SCM mtfa_opencv_compute_img_hash_PHash ( SCM imgmat )
{
    try {
        cv::Mat *imgInput, *hashOutput=new cv::Mat();
        imgInput= ( cv::Mat* ) scm_to_pointer ( imgmat );
        cv::Ptr<ImgHashBase> func = PHash::create();
        func->compute ( *imgInput, *hashOutput );
        return scm_from_pointer ( hashOutput, releaseMat );
    } catch ( exception ex ) {
        cerr << "12: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}
static SCM mtfa_opencv_compare_img_hash_PHash ( SCM scm_mat1, SCM scm_mat2 )
{
    cv::Mat *mat1, *mat2;
    mat1 = ( cv::Mat* ) scm_to_pointer ( scm_mat1 );
    mat2 = ( cv::Mat* ) scm_to_pointer ( scm_mat2 );

    try {
        Ptr<ImgHashBase> func = PHash::create();
        auto result = func->compare ( *mat1, *mat2 );
        return scm_from_double ( result );
    } catch ( exception ex ) {
        cerr << "13: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}
static SCM mtfa_opencv_compute_img_hash_MarrHildrethHash ( SCM imgmat )
{
    try {
        cv::Mat *imgInput, *hashOutput=new cv::Mat();
        imgInput= ( cv::Mat* ) scm_to_pointer ( imgmat );
        cv::Ptr<ImgHashBase> func = MarrHildrethHash::create();
        func->compute ( *imgInput, *hashOutput );
        return scm_from_pointer ( hashOutput, releaseMat );
    } catch ( exception ex ) {
        cerr << "14: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}
static SCM mtfa_opencv_compare_img_hash_MarrHildrethHash ( SCM scm_mat1, SCM scm_mat2 )
{
    cv::Mat *mat1, *mat2;
    mat1 = ( cv::Mat* ) scm_to_pointer ( scm_mat1 );
    mat2 = ( cv::Mat* ) scm_to_pointer ( scm_mat2 );

    try {
        Ptr<ImgHashBase> func = MarrHildrethHash::create();
        auto result = func->compare ( *mat1, *mat2 );
        return scm_from_double ( result );
    } catch ( exception ex ) {
        cerr << "15: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}
static SCM mtfa_opencv_compute_img_hash_RadialVarianceHash ( SCM imgmat )
{
    try {
        cv::Mat *imgInput, *hashOutput=new cv::Mat();
        imgInput= ( cv::Mat* ) scm_to_pointer ( imgmat );
        cv::Ptr<ImgHashBase> func = RadialVarianceHash::create();
        func->compute ( *imgInput, *hashOutput );
        return scm_from_pointer ( hashOutput, releaseMat );
    } catch ( exception ex ) {
        cerr << "16: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}
static SCM mtfa_opencv_compare_img_hash_RadialVarianceHash ( SCM scm_mat1, SCM scm_mat2 )
{
    cv::Mat *mat1, *mat2;
    mat1 = ( cv::Mat* ) scm_to_pointer ( scm_mat1 );
    mat2 = ( cv::Mat* ) scm_to_pointer ( scm_mat2 );

    try {
        Ptr<ImgHashBase> func = RadialVarianceHash::create();
        auto result = func->compare ( *mat1, *mat2 );
        return scm_from_double ( result );
    } catch ( exception ex ) {
        cerr << "17: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}
static SCM mtfa_opencv_compute_img_hash_BlockMeanHash0 ( SCM imgmat )
{
    try {
        cv::Mat *imgInput, *hashOutput=new cv::Mat();
        imgInput= ( cv::Mat* ) scm_to_pointer ( imgmat );
        cv::Ptr<ImgHashBase> func = BlockMeanHash::create ( 0 );
        func->compute ( *imgInput, *hashOutput );
        return scm_from_pointer ( hashOutput, releaseMat );
    } catch ( exception ex ) {
        cerr << "18: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}
static SCM mtfa_opencv_compare_img_hash_BlockMeanHash0 ( SCM scm_mat1, SCM scm_mat2 )
{
    cv::Mat *mat1, *mat2;
    mat1 = ( cv::Mat* ) scm_to_pointer ( scm_mat1 );
    mat2 = ( cv::Mat* ) scm_to_pointer ( scm_mat2 );

    try {
        Ptr<ImgHashBase> func = BlockMeanHash::create();
        auto result = func->compare ( *mat1, *mat2 );
        return scm_from_double ( result );
    } catch ( exception ex ) {
        cerr << "19: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}
static SCM mtfa_opencv_compute_img_hash_BlockMeanHash1 ( SCM imgmat )
{
    try {
        cv::Mat *imgInput, *hashOutput=new cv::Mat();
        imgInput= ( cv::Mat* ) scm_to_pointer ( imgmat );
        cv::Ptr<ImgHashBase> func = BlockMeanHash::create ( 1 );
        func->compute ( *imgInput, *hashOutput );
        return scm_from_pointer ( hashOutput, releaseMat );
    } catch ( exception ex ) {
        cerr << "20: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}
static SCM mtfa_opencv_compare_img_hash_BlockMeanHash1 ( SCM scm_mat1, SCM scm_mat2 )
{
    cv::Mat *mat1, *mat2;
    mat1 = ( cv::Mat* ) scm_to_pointer ( scm_mat1 );
    mat2 = ( cv::Mat* ) scm_to_pointer ( scm_mat2 );

    try {
        Ptr<ImgHashBase> func = BlockMeanHash::create ( 1 );
        auto result = func->compare ( *mat1, *mat2 );
        return scm_from_double ( result );
    } catch ( exception ex ) {
        cerr << "21: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}
static SCM mtfa_opencv_compute_img_hash_ColorMomentHash ( SCM imgmat )
{
    try {
        cv::Mat *imgInput, *hashOutput=new cv::Mat();
        imgInput= ( cv::Mat* ) scm_to_pointer ( imgmat );
        cv::Ptr<ImgHashBase> func = ColorMomentHash::create();
        func->compute ( *imgInput, *hashOutput );
        return scm_from_pointer ( hashOutput, releaseMat );
    } catch ( exception ex ) {
        cerr << "22: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}
static SCM mtfa_opencv_compare_img_hash_ColorMomentHash ( SCM scm_mat1, SCM scm_mat2 )
{
    cv::Mat *mat1, *mat2;
    mat1 = ( cv::Mat* ) scm_to_pointer ( scm_mat1 );
    mat2 = ( cv::Mat* ) scm_to_pointer ( scm_mat2 );

    try {
        Ptr<ImgHashBase> func = ColorMomentHash::create();
        auto result = func->compare ( *mat1, *mat2 );
        return scm_from_double ( result );
    } catch ( cv::Exception ex ) {
        cerr << "23: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}

static SCM mtfa_opencv_crop_rectangle ( SCM mat, SCM tlx, SCM tly, SCM iw, SCM ih )
{
    auto orig = ( cv::Mat* ) scm_to_pointer ( mat );
    auto x = scm_to_int ( tlx );
    auto y = scm_to_int ( tly );
    auto w = scm_to_int ( iw );
    auto h = scm_to_int ( ih );
    if ( x<0 ) {
        x=0;
    }
    if ( y<0 ) {
        y=0;
    }
    if ( w<=0 ) {
        return SCM_BOOL_F;
    }
    if ( h<=0 ) {
        return SCM_BOOL_F;
    }

    while ( ( x+w ) > 0 && ( x+w ) >= orig->cols ) {
        w--;
    }
    while ( ( y+h ) > 0 && ( y+h ) >= orig->rows ) {
        h--;
    }
    try {
        cv::Rect roi = cv::Rect ( x, y, w, h );
        //auto cropped = mtfa_internal_opencv_clone(( cv::Mat* ) scm_to_pointer ( mat ));
        //*cropped = cv::Mat ( *cropped, roi );
        auto tmp = new cv::Mat ( *orig, roi );
        return scm_from_pointer ( tmp, releaseMat );
    } catch ( cv::Exception ex ) {
        cerr << "24: " << ex.what() << endl;
        return SCM_BOOL_F;
    }
}

// static SCM mtfa_opencv_get_raw_cloud(SCM cloud)
// {
//     auto tmp = (( cv::Mat* ) scm_to_pointer ( cloud ));
//     try {
//         int pos = 0;
//         float * fel = (float*)malloc(tmp->rows*tmp->cols*3);
//         for (int r=0; r<tmp->rows; ++r)
//         {
//             for (int c=0; c<tmp->cols; ++c)
//             {
//                 auto data = tmp->at<cv::Vec4f>(c,r);
//                 fel[pos++]=data[0];
//                 fel[pos++]=data[1];
//                 fel[pos++]=data[2];
//             }
//         }
//         SCM scm_ret = scm_pointer_to_bytevector (
//                     scm_from_pointer ( ( void* ) fel, free ),
//                     scm_from_int ( tmp->rows*tmp->cols*3 ),
//                     scm_from_int ( 0 ),
//                     scm_string_to_symbol ( scm_from_locale_string( "u8" ) ) );
//         return scm_ret;
//     } catch ( cv::Exception ex ) {
//         return SCM_BOOL_F;
//     }
// }

static SCM mtfa_opencv_binary_op(SCM scm_m1, SCM scm_m2, SCM scm_op)
{
    auto m1 = ( cv::Mat* ) scm_to_pointer ( scm_m1 );
    auto m2 = ( cv::Mat* ) scm_to_pointer ( scm_m2 );

    cv::Mat * ret = new cv::Mat();

    int op = scm_to_int(scm_op);
    switch (op)
    {
    case 0:  //add
    {
        *ret = *m1 + *m2;
        return scm_from_pointer ( ret, releaseMat );
    }
    break;
    case 1:  //sub
    {
        *ret = *m1 - *m2;
        return scm_from_pointer ( ret, releaseMat );
    }
    break;
    case 2:  //and
    {
        *ret = *m1 & *m2;
        return scm_from_pointer ( ret, releaseMat );
    }
    break;
    case 3:  //or
    {
        *ret = *m1 | *m2;
        return scm_from_pointer ( ret, releaseMat );
    }
    break;
    case 4:  //xor
    {
        *ret = *m1 ^ *m2;
        return scm_from_pointer ( ret, releaseMat );
    }
    break;
    case 5:  //multiply
    {
        *ret = *m1 * *m2;
        return scm_from_pointer ( ret, releaseMat );
    }
    break;
    case 6:  //divide
    {
        *ret = *m1 / *m2;
        return scm_from_pointer ( ret, releaseMat );
    }
    break;
    default:
        return SCM_BOOL_F;
    }
}

static SCM mtfa_opencv_make_empty_mat ( )
{
    auto m = new cv::Mat();
    return scm_from_pointer ( m, releaseMat );
}

//bytelen: 8, 16, 32, 64
//type: U, S, F
//colors: 1, 2, 3, 4
static SCM mtfa_opencv_make_mat_zeros ( SCM scm_rows, SCM scm_cols, SCM scm_bytelen, SCM scm_type, SCM scm_colors )
{
    static int m3d[4][3][4]= {
        {{CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4}, {CV_8SC1, CV_8SC2, CV_8SC3, CV_8SC4}, {-1, -1, -1, -1}},
        {{CV_16UC1, CV_16UC2, CV_16UC3, CV_16UC4}, {CV_16SC1, CV_16SC2, CV_16SC3, CV_16SC4}, {CV_16FC1, CV_16FC2, CV_16FC3, CV_16FC4}},
        {{-1, -1, -1, -1}, {CV_32SC1, CV_32SC2, CV_32SC3, CV_32SC4}, {CV_32FC1, CV_32FC2, CV_32FC3, CV_32FC4}},
        {{-1, -1, -1, -1}, {-1, -1, -1, -1}, {CV_64FC1, CV_64FC2, CV_64FC3, CV_64FC4}}
    };
    auto bytelen = scm_to_int ( scm_bytelen );
    auto ctype = ScmToString ( scm_type );
    auto colors = scm_to_int ( scm_colors );

    if ( ( bytelen != 8 && bytelen != 16 && bytelen != 32 ) || ( ctype != "U" && ctype != "S" && ctype != "F" ) || ( colors != 1 && colors != 2 && colors != 3 && colors != 4 ) ) {
        cerr << "bytelen={8, 16, 32, 64}, type={\"U\", \"S\", \"F\"}, colors={1,2,3,4}" << endl;
        return SCM_BOOL_F;
    }
    int type = 0;
    if ( ctype == "U" ) {
        type = 0;
    } else if ( ctype == "S" ) {
        type = 1;
    } else if ( ctype == "F" ) {
        type = 2;
    }

    if ( bytelen == 8 ) {
        bytelen=0;
    } else if ( bytelen == 16 ) {
        bytelen=1;
    } else if ( bytelen == 32 ) {
        bytelen=2;
    } else {
        bytelen=3;
    }
    auto mat_type = m3d[bytelen][type][colors];
    if ( mat_type == -1 ) {
        return SCM_BOOL_F;
    }

    cv::Mat * m = new cv::Mat ( cv::Mat::zeros ( cv::Size ( scm_to_int ( scm_rows ), scm_to_int ( scm_cols ) ), mat_type ) );
    return scm_from_pointer ( m, releaseMat );
}

static SCM mtfa_opencv_draw_wait_key ( SCM s_ms )
{
    return scm_from_int ( cv::waitKey ( scm_to_int ( s_ms ) ) );
}

static SCM mtfa_opencv_draw_line ( SCM s_mat, SCM s_x1, SCM s_y1, SCM s_x2, SCM s_y2, SCM s_rgb, SCM s_thick, SCM s_linetype )
{
    auto mat = ( cv::Mat* ) scm_to_pointer ( s_mat );
    cv::line ( *mat, cv::Point ( S2I ( s_x1 ), S2I ( s_y1 ) ), cv::Point ( S2I ( s_x2 ), S2I ( s_y2 ) ), cv::Scalar ( S2I ( scm_car ( s_rgb ) ), S2I ( scm_cadr ( s_rgb ) ), S2I ( scm_caddr ( s_rgb ) ) ), S2I ( s_thick ), S2I ( s_linetype ) );
    return scm_cons ( s_x2, s_y2 );
}

static SCM mtfa_opencv_draw_polygon ( SCM s_mat, SCM scm_points, SCM s_ncontours, SCM s_isClosed, SCM s_rgb, SCM s_thick, SCM s_linetype )
{
    auto mat = ( cv::Mat* ) scm_to_pointer ( s_mat );

    int npoints = scm_to_int(scm_length(scm_points));
    cv::Point ** pnts = (cv::Point**)malloc(sizeof(cv::Point*)*npoints);
    for (int i=0; i<npoints; ++i)
    {
        auto point = scm_car(scm_points);
        pnts[i] = new cv::Point(scm_to_int(scm_car(point)), scm_to_int(scm_cdr(point)));
        scm_points = scm_cdr(scm_points);
    }

    cv::polylines(*mat, pnts, &npoints, scm_to_int(s_ncontours), scm_is_true(s_isClosed),
                  cv::Scalar(S2I(scm_car(s_rgb)), S2I(scm_cadr(s_rgb)), S2I(scm_caddr(s_rgb))),
                  S2I(s_thick), S2I ( s_linetype ));

    for (int i=0; i<npoints; ++i)
    {
        delete pnts[i];
    }
    free(pnts);
    return SCM_BOOL_T;
}

//questa è molto generare. Usa un array di poligoni dove ogni poligono è un array di punti
//nel nostro caso lista di liste
static SCM mtfa_opencv_fill_polygons ( SCM s_mat, SCM scm_points, SCM s_rgb)
{
    auto mat = ( cv::Mat* ) scm_to_pointer ( s_mat );

    int ncontours = scm_to_int(scm_length(scm_points));
    cv::Point ** pnts = (cv::Point**)malloc(sizeof(cv::Point*)*ncontours);
    int * npnts = (int*)malloc(sizeof(int)*ncontours);
    int pos = 0;
    while (!scm_is_null(scm_points))
    {
        SCM poly = scm_car(scm_points);
        npnts[pos] = scm_to_int(scm_length(poly));
        pnts[pos] = (cv::Point*)malloc(sizeof(cv::Point)*npnts[pos]);
        int i=0;
        while (!scm_is_null(poly))
        {
            pnts[pos][i].x = scm_to_int(scm_car(scm_car(poly)));
            pnts[pos][i].y = scm_to_int(scm_cdr(scm_car(poly)));
            i++;
            poly = scm_cdr(poly);
        }
        pos++;
        scm_points = scm_cdr(scm_points);
    }
    cv::fillPoly(*mat, (const Point**)pnts, npnts, ncontours, cv::Scalar(S2I(scm_car(s_rgb)), S2I(scm_cadr(s_rgb)), S2I(scm_caddr(s_rgb))));

    for (int i=0; i<ncontours; ++i)
    {
        free(pnts[i]);
    }
    free(pnts);
    free(npnts);
    return SCM_BOOL_T;
}
static SCM mtfa_opencv_fill_convex_polygon ( SCM s_mat, SCM scm_points, SCM s_rgb)
{
    auto mat = ( cv::Mat* ) scm_to_pointer ( s_mat );

    int npoints = scm_to_int(scm_length(scm_points));
    cv::Point * pnts = (cv::Point*)malloc(sizeof(cv::Point)*npoints);
    for (int i=0; i<npoints; ++i)
    {
        auto point = scm_car(scm_points);
        pnts[i].x = scm_to_int(scm_car(point));
        pnts[i].y = scm_to_int(scm_cdr(point));
        scm_points = scm_cdr(scm_points);
    }

    cv::fillConvexPoly(*mat, pnts, npoints, cv::Scalar(S2I(scm_car(s_rgb)), S2I(scm_cadr(s_rgb)), S2I(scm_caddr(s_rgb))));

    free(pnts);
    return SCM_BOOL_T;
}


static SCM mtfa_opencv_draw_rectangle ( SCM s_mat, SCM s_x, SCM s_y, SCM s_w, SCM s_h, SCM s_rgb, SCM s_thick, SCM s_linetype )
{
    auto mat = ( cv::Mat* ) scm_to_pointer ( s_mat );
    cv::rectangle ( *mat, cv::Point ( S2I ( s_x ), S2I ( s_y ) ), cv::Point ( S2I ( s_x )+S2I ( s_w ), S2I ( s_y )+S2I ( s_h ) ), cv::Scalar ( S2I ( scm_car ( s_rgb ) ), S2I ( scm_cadr ( s_rgb ) ), S2I ( scm_caddr ( s_rgb ) ) ), S2I ( s_thick ), S2I ( s_linetype ) );
    return scm_cons ( s_x, s_y );
}
static SCM mtfa_opencv_draw_circle ( SCM s_mat, SCM s_x, SCM s_y, SCM s_radius, SCM s_rgb, SCM s_thick, SCM s_linetype )
{
    auto mat = ( cv::Mat* ) scm_to_pointer ( s_mat );
    cv::circle ( *mat, cv::Point ( S2I ( s_x ), S2I ( s_y ) ), S2I ( s_radius ), cv::Scalar ( S2I ( scm_car ( s_rgb ) ), S2I ( scm_cadr ( s_rgb ) ), S2I ( scm_caddr ( s_rgb ) ) ), S2I ( s_thick ), S2I ( s_linetype ) );
    return scm_cons ( s_x, s_y );
}
static SCM mtfa_opencv_draw_ellipse ( SCM s_mat, SCM s_x, SCM s_y, SCM s_axes, SCM s_angle, SCM s_start_angle, SCM s_end_angle, SCM s_rgb, SCM s_thick, SCM s_linetype )
{
    auto mat = ( cv::Mat* ) scm_to_pointer ( s_mat );
    cv::ellipse(*mat, cv::Point ( S2I ( s_x ), S2I ( s_y ) ), cv::Size( S2I(scm_car(s_axes)),S2I(scm_cadr(s_axes))),
                        S2D(s_angle), S2D(s_start_angle), S2D(s_end_angle),
                        cv::Scalar ( S2I ( scm_car ( s_rgb ) ), S2I ( scm_cadr ( s_rgb ) ), S2I ( scm_caddr ( s_rgb ) ) ), S2I ( s_thick ), S2I ( s_linetype ));
    //cv::ellipse ( *mat, cv::Point ( S2I ( s_x ), S2I ( s_y ) ), S2I ( s_radius ), cv::Scalar ( S2I ( scm_car ( s_rgb ) ), S2I ( scm_cadr ( s_rgb ) ), S2I ( scm_caddr ( s_rgb ) ) ), S2I ( s_thick ), S2I ( s_linetype ) );
    return scm_cons ( s_x, s_y );
}
static SCM mtfa_opencv_draw_destroy_window ( SCM s_name )
{
    try {
        cv::destroyWindow ( ScmToString ( s_name ) );
    } catch ( cv::Exception ex ) {
        cerr << "25: " << ex.what() << endl;
    }

    return SCM_BOOL_T;
}
static SCM mtfa_opencv_draw_destroy_all_windows()
{
    try {
        cv::destroyAllWindows();
    } catch ( cv::Exception ex ) {
        cerr << "26: " << ex.what() << endl;
    }
    return SCM_BOOL_T;
}
static SCM mtfa_opencv_draw_put_text ( SCM s_mat, SCM s_text, SCM s_x, SCM s_y, SCM s_fontFace, SCM s_fontScale, SCM s_rgb, SCM s_thick, SCM s_linetype )
{
    auto mat = ( cv::Mat* ) scm_to_pointer ( s_mat );
    cv::putText ( *mat, ScmToString ( s_text ), Point ( S2I ( s_x ), S2I ( s_y ) ), S2I ( s_fontFace ), scm_to_double ( s_fontScale ), cv::Scalar ( S2I ( scm_car ( s_rgb ) ), S2I ( scm_cadr ( s_rgb ) ), S2I ( scm_caddr ( s_rgb ) ) ), S2I ( s_thick ), S2I ( s_linetype ) );
    return scm_cons ( s_x, s_y );
}
static SCM mtfa_opencv_draw_text_size ( SCM s_text, SCM s_fontFace, SCM s_fontScale, SCM s_thick)
{
    int baseline;
    auto size = cv::getTextSize(ScmToString ( s_text ), S2I ( s_fontFace ), scm_to_double ( s_fontScale ), S2I ( s_thick ), &baseline);
    auto scm_width = scm_from_int(size.width);
    auto scm_height = scm_from_int(size.height);
    auto scm_baseline = scm_from_int(baseline);
    return scm_values_3( scm_width, scm_height, scm_baseline );
}
//        WINDOW_NORMAL     = 0x00000000, //!< the user can resize the window (no constraint) / also use to switch a fullscreen window to a normal size.
//        WINDOW_AUTOSIZE   = 0x00000001, //!< the user cannot resize the window, the size is constrainted by the image displayed.
//        WINDOW_OPENGL     = 0x00001000, //!< window with opengl support.
//
//        WINDOW_FULLSCREEN = 1,          //!< change the window to fullscreen.
//        WINDOW_FREERATIO  = 0x00000100, //!< the image expends as much as it can (no ratio constraint).
//        WINDOW_KEEPRATIO  = 0x00000000, //!< the ratio of the image is respected.
//        WINDOW_GUI_EXPANDED=0x00000000, //!< status bar and tool bar
//        WINDOW_GUI_NORMAL = 0x00000010, //!< old fashious way
static SCM mtfa_opencv_draw_named_window ( SCM s_name, SCM s_type )
{
    cv::namedWindow ( ScmToString ( s_name ), S2I ( s_type ) );
    return SCM_BOOL_T;
}
static SCM mtfa_opencv_draw_resize_window ( SCM s_name, SCM s_w, SCM s_h )
{
    cv::resizeWindow ( ScmToString ( s_name ), S2I ( s_w ), S2I ( s_h ) );
    return SCM_BOOL_T;
}
static SCM mtfa_opencv_draw_display_overlay ( SCM scm_winname, SCM scm_text, SCM scm_delayms )
{
    string winname = ScmToString ( scm_winname );
    string text = ScmToString ( scm_text );
    cv::displayOverlay ( winname, text, scm_to_int ( scm_delayms ) );
    return SCM_BOOL_T;
}
static SCM mtfa_opencv_draw_display_statusbar ( SCM scm_winname, SCM scm_text, SCM scm_delayms )
{
    string winname = ScmToString ( scm_winname );
    string text = ScmToString ( scm_text );
    cv::displayStatusBar ( winname, text, scm_to_int ( scm_delayms ) );
    return SCM_BOOL_T;
}
static SCM mtfa_opencv_draw_set_window_property ( SCM scm_winname, SCM scm_prop_id, SCM scm_prop_value )
{
    cv::setWindowProperty ( ScmToString ( scm_winname ), S2I ( scm_prop_id ), scm_to_double ( scm_prop_value ) );
    return SCM_BOOL_T;
}

static SCM mtfa_opencv_draw_imshow ( SCM scm_wname, SCM scm_mat )
{
    auto tmp = ( cv::Mat* ) scm_to_pointer ( scm_mat );

    if (!tmp || tmp->rows <= 0 || tmp->cols <= 0)
        return SCM_BOOL_F;

    string wname = ScmToString ( scm_wname );
    cv::imshow ( wname, *tmp );
    return SCM_BOOL_T;
}
static SCM mtfa_opencv_draw_move_window ( SCM scm_wname, SCM s_x, SCM s_y )
{
    string wname = ScmToString ( scm_wname );
    cv::moveWindow ( wname, scm_to_int ( s_x ), scm_to_int ( s_y ) );
    return scm_cons ( s_x, s_y );
}

/****************************************************************************/
/******************************LAP algorithm*********************************/
/****************************************************************************/

//NUOVA IMPLEMENTAZIONE
namespace hung_franco
{
/**
 * Cell markings
 **/
enum
{
    UNMARKED = 0,
    MARKED,
    PRIME
};

/**
 *  Value type for marking
 */
typedef int_fast8_t Mark;

/**
 *  Value type for cells
 */
//typedef signed long int Cell;
typedef double Cell;

typedef int_fast8_t Boolean;

typedef int_fast64_t BitSetLimb;

/**
 * Bit set, a set of fixed number of bits/booleans
 */
typedef struct
{
    /**
     * The set of all limbs, a limb consist of 64 bits
     */
    BitSetLimb *limbs;

    /**
     * Singleton array with the index of the first non-zero limb
     */
    size_t first;

    /**
     * Array the the index of the previous non-zero limb for each limb
     */
    size_t *prev;

    /**
     * Array the the index of the next non-zero limb for each limb
     */
    size_t *next;

    char _buf[];
} BitSet;

typedef struct
{
    size_t row;
    size_t col;
} CellPosition;

/**
 * Calculates the floored binary logarithm of a positive integer
 *
 * @param   value  The integer whose logarithm to calculate
 * @return         The floored binary logarithm of the integer
 */
#if defined(__GNUC__)
__attribute__((__const__))
#endif
static size_t lb(BitSetLimb value)
{
    size_t rc = 0;
    BitSetLimb v = value;

    if (v & (int_fast64_t)0xFFFFFFFF00000000LL)
    {
        rc |= 32L;
        v >>= 32;
    }
    if (v & (int_fast64_t)0x00000000FFFF0000LL)
    {
        rc |= 16L;
        v >>= 16;
    }
    if (v & (int_fast64_t)0x000000000000FF00LL)
    {
        rc |= 8L;
        v >>= 8;
    }
    if (v & (int_fast64_t)0x00000000000000F0LL)
    {
        rc |= 4L;
        v >>= 4;
    }
    if (v & (int_fast64_t)0x000000000000000CLL)
    {
        rc |= 2L;
        v >>= 2;
    }
    if (v & (int_fast64_t)0x0000000000000002LL)
    {
        rc |= 1L;
    }

    return rc;
}

/**
 * Constructor for BitSet
 *
 * @param   size  The (fixed) number of bits to bit set should contain
 * @return        The a unique BitSet instance with the specified size
 */
static BitSet *
bitset_create(size_t size)
{
    size_t c = (size >> 6) + !!(size & 63L);
    BitSet *athis = (BitSet *)calloc(1, offsetof(BitSet, _buf) + c * sizeof(BitSetLimb) + 2 * (c + 1) * sizeof(size_t));

    athis->limbs = (BitSetLimb *)&athis->_buf[0];
    athis->prev = (size_t *)&athis->_buf[c * sizeof(BitSetLimb)];
    athis->next = (size_t *)&athis->_buf[c * sizeof(BitSetLimb) + c * sizeof(size_t)];

    return athis;
}

/**
 * Gets the index of any set bit in a bit set
 *
 * @param   athis  The bit set
 * @return        The index of any set bit
 */
#if defined(__GNUC__)
__attribute__((__pure__))
#endif
static ssize_t
bitset_any(BitSet *athis)
{
    size_t i;

    if (!athis->first)
        return -1;

    i = athis->first - 1;
    return (ssize_t)(lb(athis->limbs[i] & -athis->limbs[i]) + (i << 6));
}

/**
 * Turns off a bit in a bit set
 *
 * @param  athis  The bit set
 * @param  i     The index of the bit to turn off
 */
static void
bitset_unset(BitSet *athis, size_t i)
{
    size_t p, n, j = i >> 6;
    BitSetLimb old = athis->limbs[j];

    athis->limbs[j] &= ~(1LL << (i & 63L));

    if (!athis->limbs[j] ^ !old)
    {
        j++;
        p = athis->prev[j];
        n = athis->next[j];
        athis->prev[n] = p;
        athis->next[p] = n;
        if (athis->first == j)
            athis->first = n;
    }
}

/**
 * Turns on a bit in a bit set
 *
 * @param  athis  The bit set
 * @param  i     The index of the bit to turn on
 */
static void
bitset_set(BitSet *athis, size_t i)
{
    size_t j = i >> 6;
    BitSetLimb old = athis->limbs[j];

    athis->limbs[j] |= 1LL << (i & 63L);

    if (!athis->limbs[j] ^ !old)
    {
        j++;
        athis->prev[athis->first] = j;
        athis->prev[j] = 0;
        athis->next[j] = athis->first;
        athis->first = j;
    }
}

/**
 * Reduces the values on each rows so that, for each row, the
 * lowest cells value is zero, and all cells' values is decrease
 * with the same value [the minium value in the row].
 *
 * @param  n  The table's height
 * @param  m  The table's width
 * @param  t  The table in which to perform the reduction
 */
static void kuhn_reduce_rows(size_t n, size_t m, Cell **t)
{
    size_t i, j;
    Cell min, *ti;

    for (i = 0; i < n; i++)
    {
        ti = t[i];
        min = *ti;
        for (j = 1; j < m; j++)
            if (min > ti[j])
                min = ti[j];
        for (j = 0; j < m; j++)
            ti[j] -= min;
    }
}

/**
 * Determines whether the marking is complete, that is
 * if each row has a marking which is on a unique column.
 *
 * @param   n            The table's height
 * @param   m            The table's width
 * @param   marks        The marking matrix
 * @param   col_covered  Column cover array
 * @return               Whether the marking is complete
 */
static Boolean kuhn_is_done(size_t n, size_t m, Mark **marks, Boolean col_covered[])
{
    size_t i, j, count = 0;

    memset(col_covered, 0, m * sizeof(*col_covered));

    for (j = 0; j < m; j++)
    {
        for (i = 0; i < n; i++)
        {
            if (marks[i][j] == MARKED)
            {
                col_covered[j] = 1;
                break;
            }
        }
    }

    for (j = 0; j < m; j++)
        count += (size_t)col_covered[j];

    return count == n;
}

/**
 * Create a matrix with marking of cells in the table whose
 * value is zero [minimal for the row]. Each marking will
 * be on an unique row and an unique column.
 *
 * @param   n  The table's height
 * @param   m  The table's width
 * @param   t  The table in which to perform the reduction
 * @return     A matrix of markings as described in the summary
 */
static Mark **kuhn_mark(size_t n, size_t m, Cell **t)
{
    size_t i, j;
    Mark **marks;
    Boolean *row_covered, *col_covered;

    marks = (Mark **)malloc(n * sizeof(Mark *));
    for (i = 0; i < n; i++)
        marks[i] = (Mark *)calloc(m, sizeof(Mark)); /* UNMARKED == 0 */

    row_covered = (Boolean *)calloc(n, sizeof(Boolean));
    col_covered = (Boolean *)calloc(m, sizeof(Boolean));

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            if (!row_covered[i] && !col_covered[j] && !t[i][j])
            {
                marks[i][j] = MARKED;
                row_covered[i] = 1;
                col_covered[j] = 1;
            }
        }
    }

    free(row_covered);
    free(col_covered);
    return marks;
}

/**
 * Finds a prime
 *
 * @param   n            The table's height
 * @param   m            The table's width
 * @param   t            The table
 * @param   marks        The marking matrix
 * @param   row_covered  Row cover array
 * @param   col_covered  Column cover array
 * @param   primep       Output parameter for the row and column of the found prime
 * @return               1 if a prime was found, 0 otherwise
 */
static Boolean kuhn_find_prime(size_t n, size_t m, Cell **t, Mark **marks, Boolean row_covered[], Boolean col_covered[], CellPosition *primep)
{
    size_t i, j, row, col;
    ssize_t p;
    Boolean mark_in_row;
    BitSet *zeroes = bitset_create(n * m);

    for (i = 0; i < n; i++)
        if (!row_covered[i])
            for (j = 0; j < m; j++)
                if (!col_covered[j] && !t[i][j])
                    bitset_set(zeroes, i * m + j);

    for (;;)
    {
        p = bitset_any(zeroes);
        if (p < 0)
        {
            free(zeroes);
            return 0;
        }

        row = (size_t)p / m;
        col = (size_t)p % m;

        marks[row][col] = PRIME;

        mark_in_row = 0;
        for (j = 0; j < m; j++)
        {
            if (marks[row][j] == MARKED)
            {
                mark_in_row = 1;
                col = j;
            }
        }

        if (mark_in_row)
        {
            row_covered[row] = 1;
            col_covered[col] = 0;

            for (i = 0; i < n; i++)
            {
                if (!t[i][col] && row != i)
                {
                    if (!row_covered[i] && !col_covered[col])
                        bitset_set(zeroes, i * m + col);
                    else
                        bitset_unset(zeroes, i * m + col);
                }
            }

            for (j = 0; j < m; j++)
            {
                if (!t[row][j] && col != j)
                {
                    if (!row_covered[row] && !col_covered[j])
                        bitset_set(zeroes, row * m + j);
                    else
                        bitset_unset(zeroes, row * m + j);
                }
            }

            if (!row_covered[row] && !col_covered[col])
                bitset_set(zeroes, row * m + col);
            else
                bitset_unset(zeroes, row * m + col);
        }
        else
        {
            free(zeroes);
            primep->row = row;
            primep->col = col;
            return 1;
        }
    }
}

/**
 * Removes all prime marks and modifies the marking
 *
 * @param  n           The table's height
 * @param  m           The table's width
 * @param  marks       The marking matrix
 * @param  alt         Marking modification paths
 * @param  col_marks   Markings in the columns
 * @param  row_primes  Primes in the rows
 * @param  prime       The last found prime
 */
static void kuhn_alt_marks(size_t n, size_t m, Mark **marks, CellPosition alt[],
                           ssize_t col_marks[], ssize_t row_primes[], const CellPosition *prime)
{
    size_t i, j, index = 0;
    ssize_t row, col;
    Mark *markx, *marksi;

    alt[0].row = prime->row;
    alt[0].col = prime->col;

    for (i = 0; i < n; i++)
        row_primes[i] = -1;

    for (i = 0; i < m; i++)
        col_marks[i] = -1;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            if (marks[i][j] == MARKED)
                col_marks[j] = (ssize_t)i;
            else if (marks[i][j] == PRIME)
                row_primes[i] = (ssize_t)j;
        }
    }

    while ((row = col_marks[alt[index].col]) >= 0)
    {
        index++;
        alt[index].row = (size_t)row;
        alt[index].col = alt[index - 1].col;

        col = row_primes[alt[index].row];
        index++;
        alt[index].row = alt[index - 1].row;
        alt[index].col = (size_t)col;
    }

    for (i = 0; i <= index; i++)
    {
        markx = &marks[alt[i].row][alt[i].col];
        *markx = *markx == MARKED ? UNMARKED : MARKED;
    }

    for (i = 0; i < n; i++)
    {
        marksi = marks[i];
        for (j = 0; j < m; j++)
            if (marksi[j] == PRIME)
                marksi[j] = UNMARKED;
    }
}

/**
 * Depending on whether the cells' rows and columns are covered,
 * the the minimum value in the table is added, subtracted or
 * neither from the cells.
 *
 * @param  n            The table's height
 * @param  m            The table's width
 * @param  t            The table to manipulate
 * @param  row_covered  Array that tell whether the rows are covered
 * @param  col_covered  Array that tell whether the columns are covered
 */
static void kuhn_add_and_subtract(size_t n, size_t m, Cell **t, Boolean row_covered[], Boolean col_covered[])
{
    size_t i, j;
    Cell min = 0x7FFFFFFFL;

    for (i = 0; i < n; i++)
        if (!row_covered[i])
            for (j = 0; j < m; j++)
                if (!col_covered[j] && min > t[i][j])
                    min = t[i][j];

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            if (row_covered[i])
                t[i][j] += min;
            if (!col_covered[j])
                t[i][j] -= min;
        }
    }
}

/**
 * Creates a list of the assignment cells
 *
 * @param   n      The table's height
 * @param   m      The table's width
 * @param   marks  Matrix markings
 * @return         The assignment, an array of row–coloumn pairs
 */
static CellPosition *
kuhn_assign(size_t n, size_t m, Mark **marks)
{
    CellPosition *assignment = (CellPosition *)malloc(n * sizeof(CellPosition));
    size_t i, j;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            if (marks[i][j] == MARKED)
            {
                assignment[i].row = i;
                assignment[i].col = j;
            }
        }
    }

    return assignment;
}

/**
 * Calculates an optimal bipartite minimum weight matching using an
 * O(n³)-time implementation of The Hungarian Algorithm, also known
 * as Kuhn's Algorithm.
 *
 * @param   n      The height of the table
 * @param   m      The width of the table
 * @param   table  The table in which to perform the matching
 * @return         The optimal assignment, an array of row–coloumn pairs
 */
static CellPosition * kuhn_match(size_t n, size_t m, Cell **table)
{
    size_t i;
    ssize_t *row_primes, *col_marks;
    Mark **marks;
    Boolean *row_covered, *col_covered;
    CellPosition *ret, prime, *alt;

    /* Not copying table since it will only be used once. */
    row_covered = (Boolean *)calloc(n, sizeof(Boolean));
    col_covered = (Boolean *)calloc(m, sizeof(Boolean));

    row_primes = (ssize_t *)malloc(n * sizeof(ssize_t));
    col_marks = (ssize_t *)malloc(m * sizeof(ssize_t));

    alt = (CellPosition *)malloc(n * m * sizeof(CellPosition));

    kuhn_reduce_rows(n, m, table);
    marks = kuhn_mark(n, m, table);

    while (!kuhn_is_done(n, m, marks, col_covered))
    {
        while (!kuhn_find_prime(n, m, table, marks, row_covered, col_covered, &prime))
            kuhn_add_and_subtract(n, m, table, row_covered, col_covered);
        kuhn_alt_marks(n, m, marks, alt, col_marks, row_primes, &prime);
        memset(row_covered, 0, n * sizeof(*row_covered));
        memset(col_covered, 0, m * sizeof(*col_covered));
    }

    free(row_covered);
    free(col_covered);
    free(alt);
    free(row_primes);
    free(col_marks);

    ret = kuhn_assign(n, m, marks);

    for (i = 0; i < n; i++)
        free(marks[i]);
    free(marks);

    return ret;
}

typedef size_t **(*Tassigned);
// static void	print(size_t n, size_t m, Cell **t, CellPosition assignment[]) {
//     size_t i, j;
//     Tassigned assigned;
// 
//     assigned = (Tassigned)calloc(1, sizeof(ssize_t[n][m]));
// 
//     if (assignment)
//         for (i = 0; i < n; i++)
//             (*assigned)[assignment[i].row][assignment[i].col] += 1;
// 
//     for (i = 0; i < n; i++)
//     {
//         printf("    ");
//         for (j = 0; j < m; j++)
//         {
//             if ((*assigned)[i][j])
//                 printf("\033[%im", (int)(30 + (*assigned)[i][j]));
//             printf("%5lf%s\033[m   ", (Cell)t[i][j], (*assigned)[i][j] ? "^" : " ");
//         }
//         printf("\n\n");
//     }
//     free(assigned);
// }
}

static size_t internalGetUniformElementSize ( scm_t_array_handle* handle )
{
    auto const body = [] ( void* data ) {
        size_t uniform_element_size = scm_array_handle_uniform_element_size ( ( scm_t_array_handle* ) data );
        return scm_from_size_t ( uniform_element_size );
    };
    auto const handler = [] ( void* data, SCM key, SCM args ) {
        return scm_from_size_t ( 0 );
    };
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, handle, handler, nullptr );
    return scm_to_int ( ret );
}
static SCM mtfa_lap ( SCM scm_table )
{
    if ( !scm_array_p ( scm_table, SCM_UNDEFINED ) ) {
        scm_wrong_type_arg_msg ( "mtfa_lap", 0, scm_table, "uniform two dimensional table of f64 values. Like #2f64((1 2) (6 7))" );
        return SCM_BOOL_F;
    }

    SCM tipo = scm_array_type(scm_table);
    char * tipocelle = scm_to_locale_string(scm_symbol_to_string(tipo));
    if (strcmp(tipocelle, "f64") != 0)
    {
        free(tipocelle);
        scm_wrong_type_arg_msg ( "mtfa_lap", 0, scm_table, "uniform two dimensional table of f64 values. Like #2f64((1 2) (6 7))" ); //E qui esce
    }
    free(tipocelle);

    scm_t_array_handle handle;
    scm_array_get_handle ( scm_table, &handle );

    size_t rank = scm_array_handle_rank ( &handle );
    if ( rank != 2 ) {
        scm_array_handle_release ( &handle );
        scm_wrong_type_arg_msg ( "mtfa_lap", 0, scm_table, "uniform two dimensional table. Like #2f64((1 2) (6 7))" ); //E qui esce
    }

    const scm_t_array_dim * dims = scm_array_handle_dims ( &handle );
    if ( dims == nullptr ) {
        scm_array_handle_release ( &handle );
        scm_wrong_type_arg_msg ( "mtfa_lap", 0, scm_table, "uniform two dimensional table. Like #2f64((1 2) (6 7))" ); //E qui esce
    }

    int element_size = internalGetUniformElementSize ( &handle );

    if ( element_size == 0 ) {
        scm_array_handle_release ( &handle );
        scm_wrong_type_arg_msg ( "mtfa_lap", 0, scm_table, "uniform two dimensional table. Like #2f64((1 2) (6 7))" ); //E qui esce
    }

    
    const double * ptr = scm_array_handle_f64_elements ( &handle );
    if ( ptr == nullptr ) {
        scm_array_handle_release ( &handle );
        scm_wrong_type_arg_msg ( "mtfa_lap", 0, scm_table, "uniform two dimensional table. Like #2f64((1 2) (6 7))" ); //E qui esce
    }

    int numRows = dims[0].ubnd-dims[0].lbnd+1;
    int numCols = dims[1].ubnd-dims[1].lbnd+1;
    if (numRows > numCols)
    {
        scm_array_handle_release ( &handle );
        scm_wrong_type_arg_msg ( "mtfa_lap", 0, scm_table, "uniform two dimensional table (rows = cols). Like #2f64((1 2) (6 7))" ); //E qui esce
    }

    //lo converto in array di array...purtroppo...
    hung_franco::Cell **table;
    int n = numRows;
    int m = numCols;
    table = (hung_franco::Cell **)malloc(n * sizeof(hung_franco::Cell *));
    for (int i = 0; i < n; i++)
    {
        table[i] = (hung_franco::Cell *)malloc(m * sizeof(hung_franco::Cell));
        for (int j = 0; j < m; j++)
            table[i][j] = ptr[i*numCols+j];
    }
    scm_array_handle_release ( &handle );

    auto assignment =  hung_franco::kuhn_match(numRows, numCols, table);

    //Torna la lista delle assegnazioni
    SCM lista = SCM_EOL;
    for (int i=numRows-1; i>=0; --i)
        lista = scm_cons(scm_cons(scm_from_int(assignment[i].row), scm_from_int(assignment[i].col)), lista);
    free(assignment);

    for (int r=0; r<numRows; ++r)
        free(table[r]);
    free(table);

    return lista;
}


/****Funzioni di manipolazione immagini****/
static SCM mtfa_opencv_change_brightness_and_contrast ( SCM _scm_mat, SCM _bright, SCM _contrast ) //0..200, 0..200
{
    auto img = ( cv::Mat* ) scm_to_pointer ( _scm_mat );

    int brightness = scm_to_int(_bright);
    int contrast = scm_to_int(_contrast);
    /*
    * The algorithm is by Werner D. Streidt
    * (http://visca.com/ffactory/archives/5-99/msg00021.html)
    */
    double a, b;
    if( contrast > 0 )
    {
        double delta = 127.*contrast/100;
        a = 255./(255. - delta*2);
        b = a*(brightness - delta);
    }
    else
    {
        double delta = -128.*contrast/100;
        a = (256.-delta*2)/255.;
        b = a*brightness + delta;
    }
    img->convertTo(*img, -1, a, b);
    return _scm_mat;
}

static SCM mtfa_opencv_change_brightness ( SCM scm_mat, SCM added )
{
    auto img = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    img->convertTo(*img, -1, 1, scm_to_int(added));
    return scm_mat;
}
static SCM mtfa_opencv_change_contrast ( SCM scm_mat, SCM multiplied )
{
    auto img = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    img->convertTo(*img, -1, scm_to_double(multiplied), 0);
    return scm_mat;
}


static SCM mtfa_opencv_prop_resize ( SCM scm_mat, SCM scm_resize )
{
    auto img = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    try {
        cv::resize(*img, *img, cv::Size(), scm_to_double(scm_resize), scm_to_double(scm_resize));
    } catch (...) {}
    return scm_mat;
}
static SCM mtfa_opencv_resize ( SCM scm_mat, SCM scm_resize_x, SCM scm_resize_y )
{
    auto img = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    cv::resize(*img, *img, cv::Size(scm_to_int(scm_resize_x), scm_to_int(scm_resize_y)));
    return scm_mat;
}
static SCM mtfa_opencv_gray ( SCM scm_mat)
{
    auto img = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    cv::cvtColor(*img, *img, cv::COLOR_BGR2GRAY);
    return scm_mat;
}
static SCM mtfa_opencv_add_gaussian_noise ( SCM scm_mat, SCM scm_sigma, SCM scm_mean)
{
    auto img = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    auto NoiseArr = new Mat(img->rows, img->cols, img->type());
    RNG rng;
    rng.fill(*NoiseArr, RNG::NORMAL, scm_to_double(scm_mean),scm_to_double(scm_sigma));
    cv::add(*img, *NoiseArr, *img);
    delete NoiseArr;
    return scm_mat;
}

static SCM mtfa_opencv_salt_and_pepper ( SCM scm_mat, SCM scm_pa, SCM scm_pb)
{
    auto srcArr= ( cv::Mat* ) scm_to_pointer ( scm_mat );
    auto cols = srcArr->cols;
    auto rows = srcArr->rows;

    RNG rng;
    srand(time(nullptr));
    int amount1=rows*cols*scm_to_double(scm_pa);
    int amount2=rows*cols*scm_to_double(scm_pb);
    for(int counter=0; counter<amount1; ++counter)
    {
        Point xy;
        xy.x=rand() % cols; //rng.uniform( 0, srcArr.cols);
        xy.y=rand() % rows; //rng.uniform( 0, srcArr.rows);
        cv::circle(*srcArr, xy, 0, Scalar(0,0,0), -1);
    }
    for (int counter=0; counter<amount2; ++counter)
    {
        Point xy;
        xy.x=rand() % cols; //rng.uniform( 0, srcArr.cols);
        xy.y=rand() % rows; //rng.uniform( 0, srcArr.rows);
        cv::circle(*srcArr, xy, 0, Scalar(255, 255, 255), -1);
        //srcArr.at<uchar>(xy) = 255;
    }
    return scm_mat;
}

static SCM mtfa_opencv_gaussian_blur( SCM scm_mat, SCM scm_sigma_x, SCM scm_sigma_y)
{
    auto srcArr = ( cv::Mat* ) scm_to_pointer ( scm_mat );

    cv::GaussianBlur(*srcArr, *srcArr, cv::Size(), scm_to_double(scm_sigma_x), scm_to_double(scm_sigma_y));
    return scm_mat;
}

static SCM mtfa_opencv_median_blur( SCM scm_mat, SCM scm_ksize)
{
    auto srcArr= ( cv::Mat* ) scm_to_pointer ( scm_mat );

    cv::medianBlur(*srcArr, *srcArr, scm_to_int(scm_ksize));
    return scm_mat;
}

static SCM mtfa_opencv_rotate( SCM scm_mat, SCM scm_angle )
{
    auto src = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    double angle = scm_to_double(scm_angle);
    auto cols = src->cols;
    auto rows = src->rows;
    auto size = src->size();

    // get rotation matrix for rotating the image around its center in pixel coordinates
    cv::Point2f center((cols-1)/2.0, (rows-1)/2.0);
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle, center not relevant
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), size, angle).boundingRect2f();
    // adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - cols/2.0;
    rot.at<double>(1,2) += bbox.height/2.0 - rows/2.0;

    cv::warpAffine(*src, *src, rot, bbox.size());
    return scm_mat;
}

static SCM mtfa_opencv_erode( SCM scm_mat, SCM scm_x_size, SCM scm_y_size, SCM scm_iterations)
{
    auto src = ( cv::Mat* ) scm_to_pointer ( scm_mat );

    cv::erode(*src, *src, getStructuringElement(MORPH_RECT, Size(scm_to_int(scm_x_size), scm_to_int(scm_y_size))), cv::Point(-1, -1), scm_to_int(scm_iterations));
    return scm_mat;
}

static SCM mtfa_opencv_contours( SCM scm_mat, SCM scm_canny, SCM scm_blur_size)
{
    auto img = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    cv::Mat * src = mtfa_internal_opencv_clone(img);
    auto blur_size = scm_to_int(scm_blur_size);

    cvtColor( *src, *src, cv::COLOR_BGRA2GRAY );
    blur( *src, *src, Size(blur_size, blur_size));

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    /// Detect edges using canny
    cv::Canny( *src, *src, scm_to_int(scm_canny), scm_to_int(scm_canny)*2, 3);
    /// Find contours
    findContours( *src, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, Point(0, 0) );

    /// Draw contours
    Mat drawing = Mat::zeros( src->size(), CV_8UC3 );
    Scalar color = Scalar( 255, 255, 255 );
    for( int i = 0; i< contours.size(); i++ ) {
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }
    delete src;
    cv::Mat * out = mtfa_internal_opencv_clone(drawing);
    return scm_from_pointer ( out, releaseMat );
}


static SCM mtfa_opencv_dilate( SCM scm_mat, SCM scm_x_size, SCM scm_y_size, SCM scm_iterations)
{
    auto src = ( cv::Mat* ) scm_to_pointer ( scm_mat );

    cv::dilate(*src, *src, getStructuringElement(MORPH_RECT, Size(scm_to_int(scm_x_size), scm_to_int(scm_y_size))), cv::Point(-1, -1), scm_to_int(scm_iterations));
    return scm_mat;
}

static SCM mtfa_opencv_transform( SCM scm_mat, SCM scm_tl_x, SCM scm_tl_y, SCM scm_tr_x, SCM scm_tr_y, SCM scm_br_x, SCM scm_br_y, SCM scm_bl_x, SCM scm_bl_y)
{
    auto src = ( cv::Mat* ) scm_to_pointer ( scm_mat );

    auto x1 = scm_to_int(scm_tl_x);
    auto y1 = scm_to_int(scm_tl_y);
    auto x2 = scm_to_int(scm_tr_x);
    auto y2 = scm_to_int(scm_tr_y);
    auto x3 = scm_to_int(scm_br_x);
    auto y3 = scm_to_int(scm_br_y);
    auto x4 = scm_to_int(scm_bl_x);
    auto y4 = scm_to_int(scm_bl_y);

    int max_x = x1;
    if (max_x < x2) max_x = x2;
    if (max_x < x3) max_x = x3;
    if (max_x < x4) max_x = x4;

    int max_y = y1;
    if (max_y < y2) max_x = y2;
    if (max_y < y3) max_x = y3;
    if (max_y < y4) max_x = y4;

    Point2f inputQuad[4];
    Point2f outputQuad[4];

    inputQuad[0] = Point2f(x1, y1);
    inputQuad[1] = Point2f(x2, y2);
    inputQuad[2] = Point2f(x3, y3);
    inputQuad[3] = Point2f(x4, y4);

    // The 4 points where the mapping is to be done , from top-left in clockwise order
    outputQuad[0] = Point2f( 0,0 );
    outputQuad[1] = Point2f( src->cols-1,0);
    outputQuad[2] = Point2f( src->cols-1, src->rows-1);
    outputQuad[3] = Point2f( 0, src->rows-1);

    //Mat lambda( 2, 4, CV_32FC1 );
    Mat lambda = Mat::zeros( src->rows, src->cols, src->type() );
    lambda = getPerspectiveTransform( inputQuad, outputQuad );
    Mat *dst = new cv::Mat(src->rows, src->cols, src->type());
    warpPerspective(*src, *dst, lambda, dst->size());

    return scm_from_pointer ( dst, releaseMat );
}


static SCM mtfa_opencv_gamma_correction ( SCM scm_mat, SCM gamma )
{
    auto img = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    auto fGamma = scm_to_double(gamma);

    unsigned char lut[256];
    for ( int i = 0; i < 256; i++ )
        lut[i] = saturate_cast<uchar> ( pow ( ( float ) ( i / 255.0 ), fGamma ) * 255.0f );

    const int channels = img->channels();
    switch ( channels )
    {
    case 1:
    {
        MatIterator_<uchar> it, end;
        for ( it = img->begin<uchar>(), end = img->end<uchar>(); it != end; it++ )
            *it = lut[ ( *it )];
        break;
    }
    case 3:
    {
        MatIterator_<Vec3b> it, end;
        for ( it = img->begin<Vec3b>(), end = img->end<Vec3b>(); it != end; it++ )
        {
            ( *it ) [0] = lut[ ( ( *it ) [0] )];
            ( *it ) [1] = lut[ ( ( *it ) [1] )];
            ( *it ) [2] = lut[ ( ( *it ) [2] )];
        }
        break;
    }
    }
    return scm_mat;
}
static SCM mtfa_opencv_equalize_histogram ( SCM scm_mat)
{
    //Convert the image from BGR to YCrCb color space
    auto tmp = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    cv::Mat *hist_equalized_image = new cv::Mat();
    cvtColor(*tmp, *hist_equalized_image, COLOR_BGR2YCrCb);

    //Split the image into 3 channels; Y, Cr and Cb channels respectively and store it in a std::vector
    vector<Mat> vec_channels;
    cv::split(*hist_equalized_image, vec_channels);

    //Equalize the histogram of only the Y channel
    cv::equalizeHist(vec_channels[0], vec_channels[0]);

    //Merge 3 channels in the vector to form the color image in YCrCB color space.
    cv::merge(vec_channels, *hist_equalized_image);

    //Convert the histogram equalized image from YCrCb to BGR color space again
    cv::cvtColor(*hist_equalized_image, *hist_equalized_image, COLOR_YCrCb2BGR);

    auto mat = scm_from_pointer ( hist_equalized_image, releaseMat );
    return mat;
}

static SCM mtfa_opencv_add_watermark ( SCM scm_img, SCM scm_wat, SCM scm_center_x_rel, SCM scm_center_y_rel, SCM scm_width_rel, SCM scm_height_rel)
{
    auto img = ( cv::Mat* ) scm_to_pointer ( scm_img );
    auto old_wat = ( cv::Mat* ) scm_to_pointer ( scm_wat );
    auto wat = mtfa_internal_opencv_clone(old_wat);

    //Calcola coordinate centro watermark
    int center_x = img->cols*scm_to_double(scm_center_x_rel);
    int center_y = img->rows*scm_to_double(scm_center_y_rel);

    //calcola dimensioni watermark
    int wat_width  = img->cols*scm_to_double(scm_width_rel);
    int wat_height = img->rows*scm_to_double(scm_height_rel);

    //se il watermark non entra, riduce le sue dimensioni in modo proporzionale
    if ((center_x + wat_width/2) >= img->cols)
        wat_width=img->cols;

    if ((center_y + wat_height/2) >= img->rows)
        wat_height=img->rows;

    cv::resize(*wat, *wat, cv::Size(wat_width, wat_height), 0, 0, cv::INTER_LINEAR);

    // define image ROI at image bottom-right
    Mat imageROI= (*img)(cv::Rect(center_x-wat_width/2,center_y-wat_height/2, wat_width, wat_height));

    // here we inverted the color (so all black are now white and all white are now black)
    cv::Mat invSrc =  cv::Scalar::all(255) - *wat;

    // use the logo as a mask (must be gray-level)
    cv::Mat mask(invSrc);

    // insert by copying only at locations of non-zero mask
    invSrc.copyTo(imageROI, mask);

    delete wat;
    return scm_img;
}

static SCM mtfa_opencv_jpeg_compression ( SCM scm_mat, SCM scm_quality )
{
    auto img = ( cv::Mat* ) scm_to_pointer ( scm_mat );
    auto quality = scm_to_uint(scm_quality) % 100 + 1;

    vector<int> compression_params;
    compression_params.push_back(IMWRITE_JPEG_QUALITY);
    compression_params.push_back(quality);
//     compression_params.push_back(IMWRITE_JPEG_PROGRESSIVE);
//     compression_params.push_back(1);
//     compression_params.push_back(IMWRITE_JPEG_OPTIMIZE);
//     compression_params.push_back(1);
//     compression_params.push_back(IMWRITE_JPEG_LUMA_QUALITY);
//     compression_params.push_back(30);
    //FILE * tmpfile ( void );

    //Crea file temporaneo
    char buffer [256];
    strcpy(buffer, "/tmp/jpeg_cmpr_XXXXXX.jpg");
    close(mkstemps(buffer, 4));

    cv::imwrite((const string)buffer, *img, compression_params);

    cv::Mat * input = new cv::Mat();
    *input = cv::imread ( (const string)buffer );
    unlink(buffer);

    return scm_from_pointer ( input, releaseMat );
}

static SCM mtfa_opencv_open_input_stream ( SCM scm_stream, SCM scm_preferencies )
{
    cv::VideoCapture *cap = new cv::VideoCapture();
    if (cap == nullptr)
        return SCM_EOL;

    bool bRet;
    if (scm_is_number(scm_stream)) {
        //camera collegata
        if (scm_preferencies == SCM_UNDEFINED)
            bRet = cap->open(scm_to_int(scm_stream));
        else
            bRet = cap->open(scm_to_int(scm_stream), scm_to_uint(scm_preferencies));
    }
    else {
        //filmato o protocollo di input
        if (scm_preferencies == SCM_UNDEFINED)
            bRet = cap->open(ScmToString(scm_stream));
        else
            bRet = cap->open(ScmToString(scm_stream), scm_to_uint(scm_preferencies));
    }

    if (!bRet)
    {
        delete cap;
        return SCM_EOL;
    }
    else
    {
        return scm_from_pointer ( cap, releaseVideoCapture );
    }
}

static SCM mtfa_opencv_read_input_stream ( SCM scm_mat, SCM scm_cap )
{
    try {
        auto cap = ( cv::VideoCapture* ) scm_to_pointer ( scm_cap );
        auto mat = ( cv::Mat* ) scm_to_pointer ( scm_mat );
        if (cap && mat)
        {
            bool ok = cap->read(*mat);
            if (ok)
                return scm_mat;
            else
                return SCM_BOOL_F;
        }
        else
            return SCM_BOOL_F;
    }
    catch (cv::Exception ex)
    {
        cerr << ex.what() << endl;
        return SCM_BOOL_F;
    }
    catch (std::exception ex)
    {
        cerr << ex.what() << endl;
        return SCM_BOOL_F;
    }
}

static SCM mtfa_opencv_input_stream_is_opened ( SCM scm_cap )
{
    try {
        auto cap= ( cv::VideoCapture* ) scm_to_pointer ( scm_cap );
        if (cap && cap->isOpened())
            return SCM_BOOL_T;
        else
            return SCM_BOOL_F;
    }
    catch (cv::Exception ex)
    {
        cerr << ex.what() << endl;
        return SCM_BOOL_F;
    }
    catch (std::exception ex)
    {
        cerr << ex.what() << endl;
        return SCM_BOOL_F;
    }
}

static SCM mtfa_opencv_close_input_stream ( SCM scm_cap )
{
    //non fa nulla, lascia fare ai distruttori!
    return SCM_BOOL_T;
//     try {
//         auto cap = ( cv::VideoCapture* ) scm_to_pointer ( scm_cap );
//         if (cap && cap->isOpened())
//         {
//             cap->release();
//             delete cap;
//         }
//         return SCM_BOOL_T;
//     }
//     catch (...)
//     {
//         return SCM_BOOL_F;
//     }
}

static SCM mtfa_opencv_open_output_stream ( SCM scm_stream, SCM scm_fourcc, SCM scm_fps, SCM scm_size, SCM scm_is_color )
{
    //int apiPreference, int fourcc, double fps, Size frameSize, bool isColor = true
//     @param fourcc 4-character code of codec used to compress the frames. For example,
//     VideoWriter::fourcc('P','I','M','1') is a MPEG-1 codec, VideoWriter::fourcc('M','J','P','G') is a
//     motion-jpeg codec etc. List of codes can be obtained at [Video Codecs by
//     FOURCC](http://www.fourcc.org/codecs.php) page. FFMPEG backend with MP4 container natively uses
//     other values as fourcc code: see [ObjectType](http://mp4ra.org/#/codecs),
//     so you may receive a warning message from OpenCV about fourcc code conversion.
    /*
    res["h264"] = VideoWriter::fourcc('H','2','6','4');
    res["h265"] = VideoWriter::fourcc('H','E','V','C');
    res["mpeg2"] = VideoWriter::fourcc('M','P','E','G');
    res["mpeg4"] = VideoWriter::fourcc('M','P','4','2');
    res["mjpeg"] = VideoWriter::fourcc('M','J','P','G');
    res["vp8"] = VideoWriter::fourcc('V','P','8','0');

     */

    int fourcc;
    double fps;
    Size frameSize;
    bool isColor;
    auto filename = ScmToString(scm_stream);

    //scm_fourcc è  una stringa di 4 caratteri che viene convertita in un intero
    string sfcc = ScmToString(scm_fourcc);
    if (sfcc.size() != 4)
        sfcc="\0x00\0x00\0x00\0x00";
    fourcc = cv::VideoWriter::fourcc(sfcc[0], sfcc[1], sfcc[2], sfcc[3]);

    fps = scm_to_double(scm_fps);

    //scm_size è una lista (width, height)
    frameSize = cv::Size(scm_to_int(scm_car(scm_size)), scm_to_int(scm_cadr(scm_size)));

    isColor = scm_to_bool(scm_is_color);

    cv::VideoWriter *put = new cv::VideoWriter();
    if (put == nullptr)
        return SCM_EOL;
    bool bRet = put->open(
                    //Questa stringa è buona! "appsrc  ! videoconvert  ! video/x-raw,format=I420,width=1280,height=720,framerate=60/1 ! jpegenc ! tcpserversink host=127.0.0.1 port=5000 recover-policy=keyframe sync-method=latest-keyframe"
                    //,CAP_GSTREAMER,0,60,Size(1280,720)); //Lo vedi con ffplay tcp://127.0.0.1:5000 e con gst-launch-1.0 -v tcpclientsrc host=127.0.0.1 port=5000 ! decodebin ! autovideosink (più veloce!!!)
                    ScmToString(scm_stream), CAP_GSTREAMER, fourcc, fps, frameSize, isColor);


    //"appsrc ! videoconvert ! video/x-raw, format=I420, width=1920, height=1280, framerate=60/1 ! rtpvrawpay ! udpsink host=127.0.0.1 port=5000",CAP_GSTREAMER,0,30,Size(640,480));
    /*
     gst-launch-1.0 -v udpsrc port=5000 ! "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:0, depth=(string)8, width=(string)640, height=(string)480, payload=(int)96" ! rtpvrawdepay ! xvimagesink
    */

    //"appsrc  ! videoconvert !  video/x-raw, format=I420, width=640, height=480, framerate=30/1 ! jpegenc ! rtpjpegpay ! udpsink host=127.0.0.1 port=5000",CAP_GSTREAMER,0,30,Size(640,480));
    //"appsrc  ! videoconvert !  video/x-raw, format=I420, width=1280, height=720, framerate=60/1 ! jpegenc ! rtpjpegpay ! udpsink host=127.0.0.1 port=5000",CAP_GSTREAMER,0,60,Size(1280,720));
    //"appsrc  ! videoconvert !  video/x-raw, format=I420, width=1280, height=720, framerate=60/1 ! jpegenc ! rtpjpegpay ! udpsink host=224.1.1.1  auto-multicast=true port=5000",CAP_GSTREAMER,0,60,Size(1280,720));

    //"appsrc  ! videoconvert !  video/x-raw, format=I420, width=1280, height=720, framerate=60/1 ! jpegenc ! rtpjpegpay ! tee name=t ! queue ! tcpserversink host=127.0.0.1 port=5000 t. ! queue ! tcpserversink host=127.0.0.1 port=5001"
    //"appsrc  ! videoconvert !  video/x-raw, format=I420, width=1280, height=720, framerate=60/1 ! jpegenc ! rtspjpegpay ! tcpserversink host=127.0.0.1 port=5000 "

    //Buona!
//           "appsrc  ! videoconvert  ! video/x-raw,format=I420,width=1280,height=720,framerate=60/1 ! jpegenc ! tcpserversink host=127.0.0.1 port=5000 recover-policy=keyframe sync-method=latest-keyframe"
//           ,CAP_GSTREAMER,0,60,Size(1280,720)); //Lo vedi con ffplay tcp://127.0.0.1:5000 e con gst-launch-1.0 -v tcpclientsrc host=127.0.0.1 port=5000 ! decodebin ! autovideosink (più veloce!!!)
    /*
     * ffmpeg -i rtp://10.211.55.1:5000 -c copy 703.mp4, al posto di vlc o cvlc che vuole il configuratore sdp
        pippo.sdp:  v=0
                    c=IN IP4 127.0.0.1
                    m=video 5000 RTP/AVP 26
                    a=rtpmap:26 JPEG/90000
    */

//     if (scm_preferencies == SCM_UNDEFINED)
//         *put= cv::VideoWriter();
//     else
//         *put = cv::VideoCapture(ScmToString(scm_stream), scm_to_uint(scm_preferencies));
    /*
     * Esempio gstreamer
     * gst-launch-1.0 videotestsrc pattern=11 ! videoconvert ! autovideosink
     * gst-launch-1.0 videotestsrc ! videoconvert ! tee name=t ! queue ! autovideosink t. ! queue ! autovideosink //2 video
     * gst-launch-1.0 souphttpsrc location=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm ! matroskademux name=d d.video_0 ! matroskamux ! filesink location=sintel_video.mkv  //scrive sul file mkv
     * */

    if (!bRet)
    {
        delete put;
        return SCM_EOL;
    }
    else
    {
        return scm_from_pointer ( put, releaseVideoWriter );
    }
}

static SCM mtfa_opencv_output_stream_is_opened ( SCM scm_put )
{
    try {
        auto put = ( cv::VideoWriter* ) scm_to_pointer ( scm_put );
        if (put && put->isOpened())
            return SCM_BOOL_T;
        else
            return SCM_BOOL_F;
    }
    catch (...)
    {
        return SCM_BOOL_F;
    }
}


static SCM mtfa_opencv_close_output_stream ( SCM scm_put )
{
    return SCM_BOOL_T;
//     try {
//         auto put = ( cv::VideoWriter* ) scm_to_pointer ( scm_put );
//         //Se ce ne fosse bisogno, lo disassocio
//         if (put && put->isOpened())
//         {
//             delete put;
//         }
//         return SCM_BOOL_T;
//     }
//     catch (cv::Exception ex)
//     {
//         cerr << ex.what() << endl;
//         return SCM_BOOL_F;
//     }
//     catch (std::exception ex)
//     {
//         cerr << ex.what() << endl;
//         return SCM_BOOL_F;
//     }
}

static SCM mtfa_opencv_write_output_stream ( SCM scm_mat, SCM scm_put )
{
    try {
        auto put = ( cv::VideoWriter* ) scm_to_pointer ( scm_put );
        auto mat = ( cv::Mat* ) scm_to_pointer ( scm_mat );
        if (put && mat)
            put->write(*mat);
        return SCM_BOOL_T;
    }
    catch (cv::Exception ex)
    {
        cerr << ex.what() << endl;
        return SCM_BOOL_F;
    }
    catch (std::exception ex)
    {
        cerr << ex.what() << endl;
        return SCM_BOOL_F;
    }
}

static void FreeStdVectorOfPoints(void * p)
{
    delete (std::vector<cv::Point2f>*)p;
}

static SCM mtfa_opencv_make_polygon(SCM scm_2dpoints)
{
    std::vector<cv::Point2f> * points = new std::vector<cv::Point2f>();

    if (scm_is_pair(scm_2dpoints) == 1)
    {
        while (!scm_is_null(scm_2dpoints))
        {
            Point p;
            SCM point = scm_car(scm_2dpoints);
            if (scm_is_pair(point)==1) {
                p.x = scm_to_double(scm_car(point));
                p.y = scm_to_double(scm_cdr(point));
                points->push_back(p);
                scm_2dpoints = scm_cdr(scm_2dpoints);
            } else {
                delete points;
                scm_wrong_type_arg_msg("mtfa-opencv_make_polygon", 0, scm_2dpoints, "list of (x . y)");  //E qui esce
            }
        }
    } else {
        delete points;
        scm_wrong_type_arg_msg("mtfa-opencv_make_polygon", 0, scm_2dpoints, "list of (x . y)");  //E qui esce
    }
    return scm_from_pointer ( points, FreeStdVectorOfPoints);
}

static SCM mtfa_opencv_point_in_polygon(SCM scm_point, SCM scm_polygon)
{
    std::vector<cv::Point2f> * points = (std::vector<cv::Point2f>*)scm_to_pointer(scm_polygon);
    cv::Point2f p;
    p.x = scm_to_double(scm_car(scm_point));
    p.y = scm_to_double(scm_cdr(scm_point));

    auto iret = cv::pointPolygonTest(*points, p, false);
    if (iret < 0)
        return SCM_BOOL_F;
    else
        return SCM_BOOL_T;
}

static SCM mtfa_opencv_compose(SCM scm_img_1, SCM scm_img_2, SCM scm_vertical)
{
    auto img1 = ( cv::Mat* ) scm_to_pointer ( scm_img_1 );
    auto img2 = ( cv::Mat* ) scm_to_pointer ( scm_img_2 );
    auto vertical = scm_to_bool(scm_vertical);

    if (vertical)
    {

        int rows = img1->rows + img2->rows;
        int cols = max(img1->cols, img2->cols);

        cv::Mat3b * res = new cv::Mat3b(rows, cols, Vec3b(0,0,0));

        // Copy images in correct position
        img1->copyTo((*res)(Rect(0, 0, img1->cols, img1->rows)));
        img2->copyTo((*res)(Rect(0, img1->rows, img2->cols, img2->rows)));

        return scm_from_pointer ( res, releaseMat );
    } else {
        int rows = max(img1->rows, img2->rows);
        int cols = img1->cols + img2->cols;

        cv::Mat3b * res = new cv::Mat3b(rows, cols, Vec3b(0,0,0));

        // Copy images in correct position
        img1->copyTo((*res)(Rect(0, 0, img1->cols, img1->rows)));
        img2->copyTo((*res)(Rect(img1->cols, 0, img2->cols, img2->rows)));

        return scm_from_pointer ( res, releaseMat );
    }
}

// VideoWriter writer( "appsrc ! videoconvert ! videoscale ! video/x-raw,width=320,height=240 ! theoraenc ! oggmux ! tcpserversink host=192.168.0.116 port=8080 recover-policy=keyframe sync-method=latest-keyframe unit-format=buffers units-max=1 buffers-max=0 sync=true ",
//         0,
//         5,
//         Size(320, 240),
//         true);
// VideoWriter writer(
// "appsrc ! videoconvert ! videoscale ! video/x-raw,width=320,height=240 ! theoraenc ! oggmux ! tcpserversink host=192.168.0.116 port=8080 recover-policy=keyframe sync-method=latest-keyframe unit-format=buffers units-max=1 buffers-max=0 sync=true ",
// 0,
// 5,
// Size(320, 240),
// true);

/****************************************************************************/
// /***************************
//Lo zed2 detector, using only zed2 libraries


void zed_draw_boxes ( cv::Mat & mat_img, std::vector<sl::ObjectData> & object_list, vector<cv::Scalar> & colors);
void zed_draw_boxes ( cv::Mat & mat_img, std::vector<sl::ObjectData> & object_list, vector<cv::Scalar> & colors)
{
    for ( auto &obj : object_list ) {
        if (obj.tracking_state != sl::OBJECT_TRACKING_STATE::OK || obj.bounding_box_2d.size() < 4 || obj.bounding_box.size() < 8)
            continue;

        cv::Scalar color = colors[obj.id%256];
        
        //cout << obj.bounding_box_2d.size() << endl;

        cv::rectangle ( mat_img, cv::Rect ( obj.bounding_box_2d[0].x,
                                            obj.bounding_box_2d[0].y,
                                            obj.bounding_box_2d[1].x-obj.bounding_box_2d[0].x,
                                            obj.bounding_box_2d[1].y-obj.bounding_box_2d[0].y),
                        color, 2 );
        int width  = obj.bounding_box_2d[1].x-obj.bounding_box_2d[0].x;
        // int height = obj.bounding_box_2d[1].y-obj.bounding_box_2d[0].y;
        std::string obj_name = string(toString(obj.label).c_str());
        obj_name = obj_name.substr ( 0,3 ) + "(" /*+ to_string ( obj.confidence ) + ":"*/ + to_string ( obj.id ) +
                   ":" + to_string(obj.velocity.x) +","+to_string(obj.velocity.y) +","+to_string(obj.velocity.z) +
                   ((obj.action_state==sl::OBJECT_ACTION_STATE::IDLE)?": idle":((obj.action_state==sl::OBJECT_ACTION_STATE::MOVING)?": moving":"LAST")) +
                   ")";
        cv::Size const text_size = getTextSize ( obj_name, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, 1, 0 );
        int max_width = ( text_size.width > width + 2 ) ? text_size.width : ( width + 2 );
        max_width = std::max ( max_width, ( int ) width + 2 );

        std::string coords_3d;
        if ( obj.bounding_box.size() > 0 && !std::isnan (obj.bounding_box[0].x) ) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision ( 0 ) << obj.bounding_box[0].x << ":" << obj.bounding_box[0].y << ":" << obj.bounding_box[0].z;
            coords_3d = ss.str();
            cv::Size const text_size_3d = getTextSize ( ss.str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, 1, 0 );
            int const max_width_3d = ( text_size_3d.width > width + 2 ) ? text_size_3d.width : ( width + 2 );
            if ( max_width_3d > max_width ) {
                max_width = max_width_3d;
            }
        }

        putText ( mat_img, obj_name, cv::Point2f ( obj.bounding_box_2d[0].x, obj.bounding_box_2d[0].y - text_size.height - 3 ), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar ( 0, 0, 0 ), 1 );
        if ( !coords_3d.empty() ) {
            putText ( mat_img, coords_3d, cv::Point2f ( obj.bounding_box_2d[0].x, obj.bounding_box_2d[0].y-1 ), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar ( 0, 0, 0 ), 1 );
        }
    }
}


void zed_call_rt_callback(const cv::Mat * cap_frame, int fps, int width, int height, SCM runlisp);
void zed_call_rt_callback(const cv::Mat * cap_frame, int fps, int width, int height, SCM runlisp)
{
    //non crea una nuova mat poiché questa la spedisce immediatamente al sistema di streaming e quindi non ci sono code nel mezzo, è una funzione sincrona!
    //cout << "Da c++\n";
    scm_call_2( runlisp, SCM_BOOL_T, scm_list_5(scm_from_pointer((void*)cap_frame, NULL), scm_from_int(fps), scm_from_int(width), scm_from_int(height), scm_from_int(0)));
}


#ifdef USE_OLD_DETECTOR

/***************************************************************/
/****************************** YOLO 4 *************************/

class Yolo4Detector
{
private:
public:
    std::atomic_bool stopped;
    std::atomic_bool stop;

    sl::Camera zed; // ZED-camera, SVO movies
    cv::VideoCapture cap; //La camera normale (inclusi movie)

    bool isZedCamera;

    unordered_set<string> cltosh; //classes to show for standard camera (yolov4)
    bool showAllClasses;
    
    unsigned long long frame_id;

    int video_fps;
    int video_height;
    int video_width;
    
    //I parametri di chiamata
    string names; //i tre parametri per yolov4 (data, cfg, weight)
    string config;
    string weights;
    
    string video; //"zed_camera"
    double threshold;
    bool isStationary;
    SCM callback;  //by default, nulla
    SCM rt_callback;
    bool windows;
    string classes;
    string zed_resolution;
    string zed_depth_mode;
    bool show_original;
    int remove_frames;
    int drop_frames;
    string zed_camera_ip;
    int active_rect_tlx;
    int active_rect_tly;
    int active_rect_w;
    int active_rect_h;
    
    int gpu_id;
    int win_size;
    int max_level;
    int iterations;
    
    bool detect_use_mean;
    
    unsigned long long absid; //id crescente assoluto per tutti gli elementi riconsociuti


    float thresh;
    string classesToShow;
    
    sl::RESOLUTION camera_resolution;
    string s_camera_resolution;
    sl::DEPTH_MODE camera_depth_mode;
    string s_camera_depth_mode;
    sl::InitParameters init_params;
    sl::ERROR_CODE returned_state;

    //Create ZED Objects filled in the main loop
    sl::Mat zed_image;
    cv::Mat cvmat_image;

    int removed;
    int dropped;
    int numerrors;
    
    std::string filename;
    int numcam;

    std::vector<std::string> obj_names;
    
    Detector * detector;
    
    //la sezione DNN
    float confThreshold; // Confidence threshold
    float maskThreshold; // Mask threshold
    String textGraph;
    String modelWeights;

    // Load the network
    Net net;

    Yolo4Detector()
    {
        stop = true;
        stopped = true;
        isZedCamera = false;
        showAllClasses = false;
        frame_id = 0;
        detect_use_mean = false;
        absid = 1; //il primo oggetto avrà absid pari a 1

//         names; //i tre parametri per yolov4 (data, cfg, weight)
//         config;
//         weights;
    
        video="0"; //"zed_camera"
        threshold = 0.2;
        isStationary = true;
        callback = SCM_EOL;  //by default, nulla
        rt_callback = SCM_EOL;
        windows = false;
        classes = "all";
        zed_resolution = "HD720";
        zed_depth_mode = "ULTRA";
        show_original=false;
        remove_frames = 0;
        drop_frames = 0;
        zed_camera_ip="";
        active_rect_tlx = -1;
        active_rect_tly = -1;
        active_rect_w = -1;
        active_rect_h = -1;
        
        gpu_id = 0;
        win_size = 15;
        max_level = 3;
        iterations = 8000;

        //La sezione DNN
//         confThreshold = 0.5; // Confidence threshold
//         maskThreshold = 0.3; // Mask threshold

//         textGraph = "./mask_rcnn_inception_v2_coco_2018_01_28.pbtxt";
//         modelWeights = "./frozen_inference_graph.pb";

//         // Load the network
//         net = readNetFromTensorflow(modelWeights, textGraph);
        detector=nullptr;
        
//         cerr << "DNN uses GPU device" << endl;
//         net.setPreferableBackend(DNN_BACKEND_CUDA);
//         net.setPreferableTarget(DNN_TARGET_CUDA);
        
    };

    ~Yolo4Detector()
    {
        //aspetta che si stoppi il tutto
        stop=true;
        while (!stopped)
            usleep(10000);
        if (detector)
            delete detector;
    }

    SCM run( SCM par_list );
};

void yolov4_call_rt_callback(const cv::Mat * cap_frame, int fps, int width, int height, SCM runlisp)
{
    //non crea una nuova mat poiché questa la spedisce immediatamente al sistema di streaming e quindi non ci sono code nel mezzo, è una funzione sincrona!
    //cout << "Da c++\n";
    scm_call_2( runlisp, SCM_BOOL_T, scm_list_5(scm_from_pointer((void*)cap_frame, NULL), scm_from_int(fps), scm_from_int(width), scm_from_int(height), scm_from_int(0)));
}


static SCM mtfa_yolo4_detector_type;  //il tipo del su

static void finalize_yolo4_detector ( SCM mtfa_yolo4_detector )
{
    Yolo4Detector* pt = ( Yolo4Detector* ) scm_foreign_object_signed_ref ( mtfa_yolo4_detector, 0 );

    if ( pt ) {
        scm_foreign_object_signed_set_x ( mtfa_yolo4_detector, 0, 0 );
        pt->stop = true;
        while (!pt->stopped)
            usleep(1000);

        delete pt;
    }
}

static SCM mtfa_yolo4_detector_p ( SCM mtfa_yolo4_detector )
{
    auto const body = [] ( void* data ) {
        scm_assert_foreign_object_type ( mtfa_yolo4_detector_type, * ( SCM* ) data );
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void* data, SCM key, SCM args ) {
        return SCM_BOOL_F;
    };
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_yolo4_detector, handler, nullptr );
    return ret;
}

static SCM mtfa_yolo4_detector_make ()
{
    XInitThreads();

    Yolo4Detector * p = new Yolo4Detector ();
    if (p)
        return scm_make_foreign_object_1 ( mtfa_yolo4_detector_type, ( void* ) p );
    else {
        delete p;
        return SCM_BOOL_F;
    }
}

static SCM mtfa_yolo4_detector_stop ( SCM scm_yolo4_detector )
{
    if ( mtfa_yolo4_detector_p ( scm_yolo4_detector ) == SCM_BOOL_F ) {
        scm_wrong_type_arg_msg ( "mtfa-yolo4-detector-stop", 0, scm_yolo4_detector, "mtfa-yolo4-detector type" );
    }

    Yolo4Detector* yolo4detector = ( Yolo4Detector* ) scm_foreign_object_signed_ref ( scm_yolo4_detector, 0 );
    yolo4detector->stop = true;
    while (!yolo4detector->stopped)
        usleep(1000);
    return SCM_BOOL_T;
}

static SCM mtfa_yolo4_detector_get_zed_camera ( SCM scm_yolo4_detector, SCM scm_zed_camera_num )
{
    if ( mtfa_yolo4_detector_p ( scm_yolo4_detector ) == SCM_BOOL_F ) {
        scm_wrong_type_arg_msg ( "mtfa-yolo4-detector-stop", 0, scm_yolo4_detector, "mtfa-yolo4-detector type" );
    }

    Yolo4Detector* yolo4detector = ( Yolo4Detector* ) scm_foreign_object_signed_ref ( scm_yolo4_detector, 0 );
    return scm_from_pointer (&yolo4detector->zed, nullptr);
}
static SCM mtfa_yolo4_detector_get_opencv_camera ( SCM scm_yolo4_detector, SCM scm_zed_camera_num )
{
    if ( mtfa_yolo4_detector_p ( scm_yolo4_detector ) == SCM_BOOL_F ) {
        scm_wrong_type_arg_msg ( "mtfa-yolo4-detector-stop", 0, scm_yolo4_detector, "mtfa-yolo4-detector type" );
    }

    Yolo4Detector* yolo4detector = ( Yolo4Detector* ) scm_foreign_object_signed_ref ( scm_yolo4_detector, 0 );
    return scm_from_pointer (&yolo4detector->cap, nullptr);
}

static SCM mtfa_yolo4_detector_run ( SCM scm_yolo4_detector, SCM par_list )
{
    if ( mtfa_yolo4_detector_p ( scm_yolo4_detector ) == SCM_BOOL_F ) {
        scm_wrong_type_arg_msg ( "mtfa-yolo4-detector-run", 0, scm_yolo4_detector, "mtfa-yolo4-detector type" );
    }

    Yolo4Detector* yolo4detector = ( Yolo4Detector* ) scm_foreign_object_signed_ref ( scm_yolo4_detector, 0 );

    return yolo4detector->run(par_list);
}

#if 0
static void postprocess(Mat& frame, const vector<Mat>& outs, float & confThreshold)
{
    Mat outDetections = outs[0];
    Mat outMasks = outs[1];
    
    // Output size of masks is NxCxHxW where
    // N - number of detected boxes
    // C - number of classes (excluding background)
    // HxW - segmentation shape
    const int numDetections = outDetections.size[2];
    const int numClasses = outMasks.size[1];
    
    outDetections = outDetections.reshape(1, outDetections.total() / 7);
    for (int i = 0; i < numDetections; ++i)
    {
        float score = outDetections.at<float>(i, 2);
        if (score > confThreshold)
        {
            // Extract the bounding box
            int classId = static_cast<int>(outDetections.at<float>(i, 1));
            int left = static_cast<int>(frame.cols * outDetections.at<float>(i, 3));
            int top = static_cast<int>(frame.rows * outDetections.at<float>(i, 4));
            int right = static_cast<int>(frame.cols * outDetections.at<float>(i, 5));
            int bottom = static_cast<int>(frame.rows * outDetections.at<float>(i, 6));
            
            left = max(0, min(left, frame.cols - 1));
            top = max(0, min(top, frame.rows - 1));
            right = max(0, min(right, frame.cols - 1));
            bottom = max(0, min(bottom, frame.rows - 1));
            Rect box = Rect(left, top, right - left + 1, bottom - top + 1);
            
            // Extract the mask for the object
            Mat objectMask(outMasks.size[2], outMasks.size[3],CV_32F, outMasks.ptr<float>(i,classId));
        }
    }
}
#endif


SCM Yolo4Detector::run( SCM par_list )
{
    //Riparto, quindi tutto lo stopped lo metto a false!
    stop = false;
    stopped = false;

    if ( par_list == SCM_UNDEFINED || scm_assoc_ref ( par_list, scm_from_latin1_string ( "help" ) ) != SCM_BOOL_F ) {
        cout <<
             "First parameter must be the result of a (mtfa-opencv-yolov4-detector-make). The other are in alist format and are the following: " << endl <<
             "Minimum parameters are: (mtfa-opencv-yolov4-detector-run det '((\"names\" . \"../darknet/data/coco.names\")...config weights...(\"video\" . \"test.mp4\")))" << endl << endl <<
             "{help            |                | this message }" << endl <<
             "{names           |                | file containing names }" << endl <<
             "{config          |                | file containing yolov4 cfg }" << endl <<
             "{weights         |                | file containing yolov4 weights }" << endl <<
             "{video           | 0              | movie, image, device or camera number }" << endl <<
             "{threshold       | 0.2            | threshold of detection }" << endl <<
             "{isStationary    | true           | camera is (true/false) stationary }" << endl <<
             "{callback        | '()/fname | calls the function fname with two pars: img, json-data}" << endl <<
             "{rt_callback     | '()/fname      | calls the function fname with one pars: img}" << endl <<
             "{windows         | false          | show windows (true/false) during detection }" << endl <<
             "{classes         | all            | list of the classes to show(\"class\",\"class\",\"class\",\"class\") or \"none\"}" << endl <<
             "{zed_resolution  | HD720          | Zed camera resolution (HD720, HD1080, HD2K, VGA}" << endl <<
             "{zed_depth_mode  | ULTRA          | Zed camera depth mode (NONE, PERFORMANCE, QUALITY, ULTRA)}" << endl <<
             "{show_original   | false          | show the original video/frame }" << endl <<
             "{remove_frames   | 0              | how many frames to remove from detection }" << endl <<
             "{drop_frames     | 0              | how many frames to drop}" << endl <<
             "{zed_camera_ip   | empty          | for receiving ZED streams. IE: 0.0.0.0:3000}" << endl <<
             "{active_rect_tlx | -1             | use whole image}" << endl <<
             "{active_rect_tly | -1             | use whole image}" << endl <<
             "{active_rect_w   | -1             | use whole image}" << endl <<
             "{active_rect_h   | -1             | use whole image}" << endl <<
             "{gpu_id          | 0              | for track_flow initialization}"  << endl <<
             "{win_size        | 15             | for track_flow initialization}"  << endl <<
             "{max_level       | 3              | for track_flow initialization}"  << endl <<
             "{iterations      | 8000           | for track_flow initialization}"  << endl <<
             "{detect_use_mean | false          | for detection}"  << endl <<
             endl << "All given in alist format\n";
        return SCM_BOOL_T;
    }

    SETVAL ( names );
    SETVAL ( config );
    SETVAL ( weights );
    SETVAL ( video );   //nella classe
    SETVAL ( threshold );
    SETVAL ( isStationary );
    SETVAL ( callback );
    SETVAL ( rt_callback );
    SETVAL ( windows );
    SETVAL ( classes );
    SETVAL ( zed_resolution );
    SETVAL ( zed_depth_mode );
    SETVAL ( show_original );
    SETVAL ( remove_frames );
    SETVAL ( drop_frames );
    SETVAL ( zed_camera_ip );   //nella classe
    SETVAL ( active_rect_tlx );
    SETVAL ( active_rect_tly );
    SETVAL ( active_rect_w );
    SETVAL ( active_rect_h);

    SETVAL ( gpu_id);
    SETVAL ( win_size);
    SETVAL ( max_level);
    SETVAL ( iterations);

    if ( show_original ) {
        cv::namedWindow ( "original", 0 );
    }
    if ( windows ) {
        cv::namedWindow ( "computed", 0 );
    }
    
    thresh = threshold;
    classesToShow = classes;

    isZedCamera = false;
    if (video == "zed_camera" || (video.size() > 4 && video.find(".svo", video.size()-4) != string::npos))
        isZedCamera = true;


    //Ad ogni buon conto, inizializzo tutto, anche le sezioni specifiche di ZED camera
    s_camera_resolution = zed_resolution;
    if ( s_camera_resolution == "HD720" ) {
        camera_resolution = sl::RESOLUTION::HD720;
        cerr << "RESOLUTION: " << "HD720" << endl;
    } else if ( s_camera_resolution == "HD1080" ) {
        camera_resolution = sl::RESOLUTION::HD1080;
        cerr << "RESOLUTION: " << "HD1080" << endl;
    } else if ( s_camera_resolution == "HD2K" ) {
        camera_resolution = sl::RESOLUTION::HD2K;
        cerr << "RESOLUTION: " << "HD2K" << endl;
    } else if ( s_camera_resolution == "VGA" ) {
        camera_resolution = sl::RESOLUTION::VGA;
        cerr << "RESOLUTION: " << "VGA" << endl;
    } else {
        camera_resolution = sl::RESOLUTION::HD720;
        cerr << "RESOLUTION: " << "HD720" << endl;
    }

    s_camera_depth_mode = zed_depth_mode;
    if ( s_camera_depth_mode == "NONE" ) {
        camera_depth_mode = sl::DEPTH_MODE::NONE;
        cerr << "DEPTH MODE: " << "NONE" << endl;
    } else if ( s_camera_depth_mode == "PERFORMANCE" ) {
        camera_depth_mode = sl::DEPTH_MODE::PERFORMANCE;
        cerr << "DEPTH MODE: " << "PERFORMANCE" << endl;
    } else if ( s_camera_depth_mode == "QUALITY" ) {
        camera_depth_mode = sl::DEPTH_MODE::QUALITY;
        cerr << "DEPTH MODE: " << "QUALITY" << endl;
    } else if ( s_camera_depth_mode == "ULTRA" ) {
        camera_depth_mode = sl::DEPTH_MODE::ULTRA;
        cerr << "DEPTH MODE: " << "ULTRA" << endl;
    } else {
        camera_depth_mode = sl::DEPTH_MODE::PERFORMANCE;
        cerr << "DEPTH MODE: " << "PERFORMANCE" << endl;
    }

    showAllClasses = false;
    if ( classesToShow.compare ( "all" ) == 0 ) {
        showAllClasses = true;
        cltosh = {};
    } else {
        //prende tutte le classi che sono tra le virgole
        cerr << "Classi da inserire: " << classesToShow << endl;
        showAllClasses = false;
        typedef vector<string> T_V_Split;
        T_V_Split mtfa_string_split ( const string& str, const string& delimiters );
        T_V_Split mtfa_string_split2 ( const string& str, const string& delimiter );
        string mtfa_replace_matching_pattern ( const string & data, const string & pattern, const string & replace, bool all );
        auto vs = mtfa_string_split (
                    mtfa_replace_matching_pattern (
                        mtfa_replace_matching_pattern (
                            mtfa_replace_matching_pattern ( classesToShow, "\"", "", true ),
                            ", ", ",", true ),
                        " ,", ",", true ),
                    "," );
        for ( auto it: vs ) {
            cerr << "Inserisco classe: " << it << endl;
            cltosh.insert ( it );
        }
    }

    //Alcune inizializzazioni (nel caso di zed camera)
    init_params.depth_minimum_distance = 2;
    init_params.depth_maximum_distance = 10000.0; //tanto sempre 20 o 40 metri
    init_params.depth_mode = camera_depth_mode; // sl::DEPTH_MODE::PERFORMANCE;
    init_params.camera_resolution = camera_resolution; // sl::RESOLUTION::HD720; //sl::RESOLUTION::HD720;// sl::RESOLUTION::HD1080, sl::RESOLUTION::HD720
    init_params.coordinate_units = sl::UNIT::METER;
    //init_params.coordinate_system = sl::COORDINATE_SYSTEM::RIGHT_HANDED_Y_UP;
    //init_params.enable_right_side_measure = true;
    
    removed = 0;
    dropped = 0;
    
    numcam = -1;
    
    static auto chechForNumber = mtfa_compile_pattern ("^ *[0-9]+ *$");
    if (!isZedCamera) {
        if ( mtfa_find_pattern(video, chechForNumber) ) {
            numcam = stoi ( video );
            filename = "web_camera";
        } else {
            filename = video;
        }
    }

    obj_names = objects_names_from_file ( names );
    
    detector = new Detector ( config, weights );

    auto runme = [](Yolo4Detector * me) {
        scm_init_guile();  //inizializza il thread per guile e per il garbage collector
        bool bret;
        
        while (!me->stop && !me->stopped) { //per recuperare in caso di errori continuativi
            cerr << "Loop starts\n";
            
            if (me->isZedCamera) {
                if (me->video.find(".svo") != string::npos)
                {
                    me->init_params.input.setFromSVOFile(me->video.c_str());
                } else if (me->zed_camera_ip != "") {
                    unsigned int a, b, c, d, port;
                    if (sscanf(me->zed_camera_ip.c_str(), "%u.%u.%u.%u:%d", &a, &b, &c, &d, &port) == 5) {
                        // Stream input mode - IP + port
                        string ip_address = to_string(a) + "." + to_string(b) + "." + to_string(c) + "." + to_string(d);
                        me->init_params.input.setFromStream(sl::String(ip_address.c_str()), port);
                    } else {
                        cerr << "Scanning ip and port: <" << me->zed_camera_ip << ">" << endl;
                        break;
                    }
                }

                // Open the camera
                if (me->zed.isOpened()) {
                    me->zed.close();
                }

                me->returned_state = me->zed.open(me->init_params);
                if (me->returned_state != sl::ERROR_CODE::SUCCESS) {
                    cerr << "Open Camera " << me->returned_state << "\nExit program." << endl;
                    //zed.close();
                    usleep(1000000); //pensaci un pochino!
                    continue;
                }
                me->zed.disableRecording();
            } else {
                std::string const file_ext = me->video.substr ( me->video.find_last_of ( "." ) + 1 );
                std::string const protocol = me->video.substr ( 0, 7 );
                if ( file_ext == "avi" || file_ext == "mp4" || file_ext == "mjpg" || file_ext == "mov" ||    // video file
                    protocol == "rtmp://" || protocol == "rtsp://" || protocol == "http://" || protocol == "https:/" ||    // video network stream
                    me->filename == "web_camera" ) {

                    //apre la camera
                    if (me->cap.isOpened())
                        me->cap.release();
                    
                    if ( me->numcam >= 0 ) {
                        bret = me->cap.open ( me->numcam );
                    } else {
                        bret = me->cap.open ( me->filename );
                    }

                    if (!bret)
                    {
                        std::cout << "Error: Camera/file should be connected/existing.\n";
                        usleep(1000000); //pensaci un pochino!
                        continue;
                    }
                }
            }
            
            //Giunti a questo punto la camera è stata aperta e il flusso dati è pronto, sia esso ZED, sia OpenCV
            bool const use_kalman_filter = me->isStationary;

            Tracker_optflow tracker_flow ( me->gpu_id, me->win_size, me->max_level, me->iterations );

            if (me->isZedCamera) {
                auto caminfo = me->zed.getCameraInformation();
                me->video_fps = caminfo.camera_configuration.fps;
                me->video_height = caminfo.camera_configuration.resolution.height;
                me->video_width = caminfo.camera_configuration.resolution.width;

                // Print camera information
                cout << "ZED Model                 : " << caminfo.camera_model << endl;
                cout << "ZED Serial Number         : " << caminfo.serial_number << endl;
                cout << "ZED Camera Firmware       : " << caminfo.camera_configuration.firmware_version << "/" << caminfo.sensors_configuration.firmware_version << endl;
                cout << "ZED Camera Resolution     : " << caminfo.camera_configuration.resolution.width << "x" << caminfo.camera_configuration.resolution.height << endl;
                cout << "ZED Camera FPS            : " << me->zed.getInitParameters().camera_fps << endl;
            } else {
                me->video_fps = me->cap.get(cv::CAP_PROP_FPS);
                me->video_height = me->cap.get(cv::CAP_PROP_FRAME_HEIGHT);
                me->video_width = me->cap.get(cv::CAP_PROP_FRAME_WIDTH);
            }
            
            //camera aperta, proseguo. in caso di errore, break di questo e continue del ciclo precedente!
            while (!me->stop && !me->stopped)
            {
                me->frame_id++;
                if (me->isZedCamera)
                {
                    if (me->zed.grab() == sl::ERROR_CODE::SUCCESS) {
                        me->numerrors=0;  //se non ci sono errori resetto il contatore!
                        me->dropped++;
                        if (me->dropped > me->drop_frames) {
                            //zed.retrieveImage(image, sl::VIEW::LEFT, sl::MEM::CPU);
                            me->zed.retrieveImage(me->zed_image, sl::VIEW::RIGHT_UNRECTIFIED, sl::MEM::CPU);
                            //cv::cvtColor (Zed_slMat2cvMat(image), cvmat, CV_RGBA2RGB );
                            //cv::cvtColor (slMat2cvMat(image), cvmat, CV_RGBA2RGB );
                            me->cvmat_image = slMat2cvMat(me->zed_image);
                            me->dropped = 0;
                        } else {
                            continue;
                        }
                    } else {
                        me->numerrors++;
                        usleep(1000);
                        if (me->numerrors > 5) { //Riparto! è un pochino empirica come soluzione ma...
                            cerr << "Restarts....\n";
                            break;
                        }
                        continue;
                    }
                } else {
                    if (me->cap.read(me->cvmat_image)) {
                        me->numerrors=0;  //se non ci sono errori resetto il contatore!
                        me->dropped++;
                        if (me->dropped > me->drop_frames) {
                            me->dropped = 0;
                        } else {
                            continue;
                        }
                    } else {
                        me->numerrors++;
                        usleep(1000);
                        if (me->numerrors > 5) { //Riparto! è un pochino empirica come soluzione ma...
                            cerr << "Restarts....\n";
                            break;
                        }
                        continue;
                    }
                }

                //calcolo un timestamp non appena prendo i dati (ma lo calcolo dal frame di lettura)
                //int64_t timestamp = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
                int64_t timestamp;
                sl::Timestamp last_image_timestamp ;
                if (me->isZedCamera) {
                    last_image_timestamp = me->zed.getTimestamp(sl::TIME_REFERENCE::IMAGE);
                } else {
                    timestamp = me->cap.get(CAP_PROP_POS_MSEC);
                }
                
                //se rt_callback è attiva, la esegue
                if (scm_is_null(me->rt_callback) == 0)
                {
                    yolov4_call_rt_callback(&me->cvmat_image, me->video_fps/(me->drop_frames+1), me->video_width, me->video_height, me->rt_callback);
                }
                
                if (me->removed < me->remove_frames)
                {
                    me->removed++;
                    continue;
                } else {
                    me->removed = 0;
                }

                if (scm_is_null(me->callback) == 0) {
                    auto result_vec = me->detector->detect( me->cvmat_image, me->thresh, me->detect_use_mean); //true );    // true
                    
                    //Costruisce una lista di elementi (non sono tanti)
                    SCM objects_list = SCM_EOL;
                    list<cv::Mat *> cropped_list;  //per eliminarle in uscita
                    int localid = 0;
                    for (auto it: result_vec)
                    {
                        //Ricorda: it.obj_id=0 per le persone
//                         if (it.obj_id != 0)
//                             continue;
    //                     cerr << "x,y,w,h : " << it.x << ", " << it.y << ", " << it.w << ", " << it.h << endl;
    //                     cerr << "Prob    : " << it.prob << endl;
    //                     cerr << "Class   : " << it.obj_id << endl;
    //                     cerr << "Track_id: " << it.track_id << endl;
    //                     cerr << "Fr_Count: " << it.frames_counter << endl;
    //                     cerr <<"3d x y z : " << it.x_3d << ", " << it.y_3d << ", " << it.z_3d << endl;
                        
                        //estraggo la parte contenente la persona
                        cv::Rect roi = cv::Rect ( it.x, it.y, it.w, it.h);
                        cv::Mat * cropped = nullptr;
                        try {
                            cropped = new cv::Mat(me->cvmat_image, roi);
                        } catch (...) {
                        }
                        
                        cropped_list.push_back(cropped);
                        
                        objects_list = scm_cons(
                            scm_list_n(
                                scm_from_int(localid++),
                                scm_from_long_long(me->absid++),
                                scm_from_int(it.obj_id),
                                scm_from_int(it.track_id),
                                scm_from_int(it.frames_counter),
                                scm_from_double(it.prob),
                                scm_from_int(it.x),
                                scm_from_int(it.y),
                                scm_from_int(it.w),
                                scm_from_int(it.h),
                                ((cropped==nullptr)?SCM_EOL:scm_from_pointer(cropped, nullptr)),
                                SCM_UNDEFINED),
                                objects_list);
    //                     cv::Mat blob;
    //                     blobFromImage(cropped, blob, 1.0, Size(cropped.cols, cropped.rows), Scalar(), true, false);
    //                     //Sets the input to the network
    //                     me->net.setInput(blob);
    //                     
    //                     // Runs the forward pass to get output from the output layers
    //                     std::vector<String> outNames(2);
    //                     outNames[0] = "detection_out_final";
    //                     outNames[1] = "detection_masks";
    //                     vector<Mat> outs;
    //                     me->net.forward(outs, outNames);
    //                     
    //                     // Extract the bounding box and mask for each of the detected objects
    //                     postprocess(cropped, outs, me->confThreshold);
    //                     
    //                     // Put efficiency information. The function getPerfProfile returns the overall time for inference(t) and the timings for each of the layers(in layersTimes)
    //                     vector<double> layersTimes;
    //                     double freq = getTickFrequency() / 1000;
    //                     double t = me->net.getPerfProfile(layersTimes) / freq;
    //                     string label = format("Mask-RCNN on 2.5 GHz Intel Core i7 CPU, Inference time for a frame : %0.0f ms", t);
    //                     cerr << "Num of subimages: " << outs.size() << ". " << label << endl;
    // //                     if (outs.size() > 0) {
    // //                         cv::imshow(to_string(it.obj_id), outs[0]);
    // //                     }

    //                     cv::imshow(std::to_string(it.obj_id), cropped);

    /*                    
                        cv::line ( me->cvmat_image, cv::Point (it.x, it.y), cv::Point(it.x+it.w, it.y), cv::Scalar ( 255, 255, 255 ), 4); 
                        cv::line ( me->cvmat_image, cv::Point (it.x+it.w, it.y), cv::Point(it.x+it.w, it.y+it.h), cv::Scalar ( 255, 255, 255 ), 4); 
                        cv::line ( me->cvmat_image, cv::Point (it.x+it.w, it.y+it.h), cv::Point(it.x, it.y+it.h), cv::Scalar ( 255, 255, 255 ), 4); 
                        cv::line ( me->cvmat_image, cv::Point (it.x, it.y+it.h), cv::Point(it.x, it.y), cv::Scalar ( 255, 255, 255 ), 4);*/
                    }
                    //Chiama la callback LISP
                    scm_call_7(me->callback,
                               scm_from_pointer ( (void*)&me->cvmat_image, nullptr ),
                               scm_from_long_long(me->frame_id),
                               scm_from_uint(me->video_fps),
                               scm_from_uint(me->video_width),
                               scm_from_uint(me->video_height),
                               objects_list,
                               (me->isZedCamera?scm_from_ulong_long(last_image_timestamp.getMilliseconds()):scm_from_ulong_long(timestamp))
                               //scm_from_ulong_long(timestamp)
                              );
                    //qui rilascia tutte le finestrelle "cropped"
                    for (auto & it: cropped_list) {
                        if (it)
                            delete it;
                    }
                }
                //cv::imshow("detected", me->cvmat_image);
                //cv::waitKey(1);
            }
        }
        me->stop=true;
        me->stopped = true;
        delete me->detector;
        me->detector=nullptr;
    };
    
    thread run(runme, this);
    run.detach();
    return SCM_BOOL_T;
}

#endif



/************************** YOLO 4 *****************************/
/***************************************************************/


class Zed2Detector
{
private:
public:
    std::atomic_bool stopped;
    std::atomic_bool stop;

    sl::Camera zed; // ZED-camera, SVO movies
    sl::InitParameters init_params;
    sl::PositionalTrackingParameters positional_tracking_parameters;  //tutti al default
    sl::ObjectDetectionParameters obj_det_params;
    std::vector<sl::OBJECT_CLASS> cltosh;
    sl::CameraConfiguration camera_info;
    sl::ObjectDetectionRuntimeParameters objectTracker_parameters_rt;

    Zed2Detector()
    {
        //Allocare qualcosa???
        stop = false;
        stopped = false;
    };
    ~Zed2Detector()
    {
        //aspetta che si stoppi il tutto
        stop=true;
        while (!stopped)
            usleep(10000);
    }
    SCM run( SCM par_list );
};


static SCM mtfa_zed2_detector_type;  //il tipo del su

static void finalize_zed2_detector ( SCM mtfa_zed2_detector )
{
    Zed2Detector* pt = ( Zed2Detector* ) scm_foreign_object_signed_ref ( mtfa_zed2_detector, 0 );

    if ( pt ) {
        scm_foreign_object_signed_set_x ( mtfa_zed2_detector, 0, 0 );
        pt->stop = true;
        while (!pt->stopped)
            usleep(1000);

        delete pt;
    }
}


static SCM mtfa_zed2_detector_p ( SCM mtfa_zed2_detector )
{
    auto const body = [] ( void* data ) {
        scm_assert_foreign_object_type ( mtfa_zed2_detector_type, * ( SCM* ) data );
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void* data, SCM key, SCM args ) {
        return SCM_BOOL_F;
    };
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_zed2_detector, handler, nullptr );
    return ret;
}

static SCM mtfa_zed2_detector_make ()
{
    XInitThreads();

    Zed2Detector * p = new Zed2Detector ();
    if (p)
        return scm_make_foreign_object_1 ( mtfa_zed2_detector_type, ( void* ) p );
    else {
        return SCM_BOOL_F;
    }
}

static SCM mtfa_zed2_detector_stop ( SCM scm_zed2_detector )
{
    if ( mtfa_zed2_detector_p ( scm_zed2_detector ) == SCM_BOOL_F ) {
        scm_wrong_type_arg_msg ( "mtfa-zed2-detector-stop", 0, scm_zed2_detector, "mtfa-zed2-detector type" );
    }

    Zed2Detector* zed2detector = ( Zed2Detector* ) scm_foreign_object_signed_ref ( scm_zed2_detector, 0 );
    zed2detector->stop = true;
    while (!zed2detector->stopped)
        usleep(1000);
    return SCM_BOOL_T;
}

static SCM mtfa_zed2_detector_get_zed_camera ( SCM scm_zed2_detector, SCM scm_zed_camera_num )
{
    if ( mtfa_zed2_detector_p ( scm_zed2_detector ) == SCM_BOOL_F ) {
        scm_wrong_type_arg_msg ( "mtfa-zed2-detector-stop", 0, scm_zed2_detector, "mtfa-zed2-detector type" );
    }

    Zed2Detector* zed2detector = ( Zed2Detector* ) scm_foreign_object_signed_ref ( scm_zed2_detector, 0 );
    return scm_from_pointer (&zed2detector->zed, nullptr);
}


static SCM mtfa_zed2_detector_run ( SCM scm_zed2_detector, SCM par_list )
{
    if ( mtfa_zed2_detector_p ( scm_zed2_detector ) == SCM_BOOL_F ) {
        scm_wrong_type_arg_msg ( "mtfa-zed2-detector-run", 0, scm_zed2_detector, "mtfa-zed2-detector type" );
    }

    Zed2Detector* zed2detector = ( Zed2Detector* ) scm_foreign_object_signed_ref ( scm_zed2_detector, 0 );

    return zed2detector->run(par_list);
}

/* TO CHECK
(define p (mtfa-zed2-detector-make))
(mtfa-zed2-detector-run p '(("video" . "/volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/detector/biagio/movies/90000.svo") ("isStationary" . #t)("callback" . ()) ("rt_callback" . ()) ("windows" . #t) ("show_original" . #t)))
*/

SCM Zed2Detector::run(SCM par_list)
{
    scm_init_guile();  //faccio una chiamata a guile!!!

    //i parametri con i loro default
    string video="zed_camera"; //al più è un .svo
    double threshold = 25.00;
    bool isStationary = true;
    SCM callback=SCM_EOL;  //by default, nulla
    SCM rt_callback=SCM_EOL;
    bool windows = true;
    string classes = "all";
    string zed_resolution = "HD720";
    string zed_depth_mode = "ULTRA";
    string zed_detection_model = "MULTI_CLASS_BOX_ACCURATE";
    bool show_original=false;
    int remove_frames = 0;  //for tracking
    int drop_frames = 0; //to reduce fps
    string zed_camera_ip; // = "0.0.0.0:30000";
    int active_rect_tlx = -1;
    int active_rect_tly = -1;
    int active_rect_w = -1;
    int active_rect_h = -1;
    int open_timeout_sec = 5;
    bool svo_real_time_mode = false;

    if ( par_list == SCM_UNDEFINED || scm_assoc_ref ( par_list, scm_from_latin1_string ( "help" ) ) != SCM_BOOL_F ) {
        cout <<
             "First parameter must be the result of a (mtfa-opencv-detector-make cfg weights). The other are in alist format and are the following: " << endl <<
             "Minimum parameters are: (mtfa-opencv-detector-run det '((\"names\" . \"../darknet/data/coco.names\")(\"video\" . \"test.mp4\")))" << endl << endl <<
             "{help            |                | this message }" << endl <<
             "{video           | 0              | svo movie or \"zed_camera\" }" << endl <<
             "{threshold       | 25.0           | threshold of detection 0..100}" << endl <<
             "{isStationary    | true           | camera is (true/false) stationary }" << endl <<
             "{callback        | '()/fname  | calls the function fname with two pars: img, json-data}" << endl <<
             "{rt_callback     | '()/fname  | calls the function fname with one pars: img}" << endl <<
             "{windows         | true           | show windows (true/false) during detection }" << endl <<
             "{classes         | all            | list of the classes to show(\"class\",\"class\",\"class\",\"class\") or \"none\"}" << endl <<
             "{zed_resolution  | HD720          | Zed camera resolution (HD720, HD1080, HD2K, VGA}" << endl <<
             "{zed_depth_mode  | ULTRA          | Zed camera depth mode (NONE, PERFORMANCE, QUALITY, ULTRA)}" << endl <<
             "{zed_detection_model | MULTI_CLASS_BOX_ACCURATE | Zed camera detection model ( MULTI_CLASS_BOX, MULTI_CLASS_BOX_ACCURATE, HUMAN_BODY_FAST, HUMAN_BODY_ACCURATE)}" << endl <<
             "{show_original   | false          | show the original video/frame }" << endl <<
             "{remove_frames   | 0              | how many frames to remove before tracking }" << endl <<
             "{drop_frames     | 0              | how many frames to drop to reduce fps }" << endl <<
             "{zed_camera_ip   | empty          | for receiving ZED streams. IE: 0.0.0.0:3000}" << endl <<
             "{active_rect_tlx | -1             | use whole image}" << endl <<
             "{active_rect_tly | -1             | use whole image}" << endl <<
             "{active_rect_w   | -1             | use whole image}" << endl <<
             "{active_rect_h   | -1             | use whole image}" << endl <<
             "{open_timeout_sec | 5             | retry 5 times in case of opencamera error (-1: indefinitively, 0: give error immediately)}" << endl <<
             "{svo_real_time_mode | false       | true for playing SVO in rel time, false otherwise}" << endl <<
             endl << "All given in alist format\n";
        return SCM_BOOL_F;
    }

    SETVAL ( video );
    SETVAL ( threshold );
    SETVAL ( isStationary );
    SETVAL ( callback );
    SETVAL ( rt_callback );
    SETVAL ( windows );
    SETVAL ( classes );
    SETVAL ( zed_resolution );
    SETVAL ( zed_depth_mode );
    SETVAL ( zed_detection_model );
    SETVAL ( show_original );
    SETVAL ( remove_frames );
    SETVAL ( drop_frames );
    SETVAL ( zed_camera_ip );
    SETVAL ( active_rect_tlx );
    SETVAL ( active_rect_tly );
    SETVAL ( active_rect_w );
    SETVAL ( active_rect_h);
    SETVAL(open_timeout_sec);
    SETVAL(svo_real_time_mode);


    if ( show_original ) {
        cv::namedWindow ( "original", 0 );
    }
    if ( windows ) {
        cv::namedWindow ( "computed", 0 );
    }

//      cout << "Original: " << show_original << endl;
//      cout << "Windows : " << windows << endl;

    float thresh = threshold;
    string classesToShow = classes;

    sl::RESOLUTION camera_resolution;
    string s_camera_resolution = zed_resolution;
    if ( s_camera_resolution == "HD720" ) {
        camera_resolution = sl::RESOLUTION::HD720;
        cerr << "RESOLUTION: " << "HD720" << endl;
    } else if ( s_camera_resolution == "HD1080" ) {
        camera_resolution = sl::RESOLUTION::HD1080;
        cerr << "RESOLUTION: " << "HD1080" << endl;
    } else if ( s_camera_resolution == "HD2K" ) {
        camera_resolution = sl::RESOLUTION::HD2K;
        cerr << "RESOLUTION: " << "HD2K" << endl;
    } else if ( s_camera_resolution == "VGA" ) {
        camera_resolution = sl::RESOLUTION::VGA;
        cerr << "RESOLUTION: " << "VGA" << endl;
    } else {
        camera_resolution = sl::RESOLUTION::HD720;
        cerr << "RESOLUTION: " << "HD720" << endl;
    }

    sl::DEPTH_MODE camera_depth_mode;
    string s_camera_depth_mode = zed_depth_mode;
    if ( s_camera_depth_mode == "NONE" ) {
        camera_depth_mode = sl::DEPTH_MODE::NONE;
        cerr << "DEPTH MODE: " << "NONE" << endl;
    } else if ( s_camera_depth_mode == "PERFORMANCE" ) {
        camera_depth_mode = sl::DEPTH_MODE::PERFORMANCE;
        cerr << "DEPTH MODE: " << "PERFORMANCE" << endl;
    } else if ( s_camera_depth_mode == "QUALITY" ) {
        camera_depth_mode = sl::DEPTH_MODE::QUALITY;
        cerr << "DEPTH MODE: " << "QUALITY" << endl;
    } else if ( s_camera_depth_mode == "ULTRA" ) {
        camera_depth_mode = sl::DEPTH_MODE::ULTRA;
        cerr << "DEPTH MODE: " << "ULTRA" << endl;
    } else if ( s_camera_depth_mode == "NEURAL" ) {
        camera_depth_mode = sl::DEPTH_MODE::NEURAL;
        cerr << "DEPTH MODE: " << "ULTRA" << endl;
    } else {
        camera_depth_mode = sl::DEPTH_MODE::PERFORMANCE;
        cerr << "DEPTH MODE: " << "PERFORMANCE" << endl;
    }

    sl::OBJECT_DETECTION_MODEL detection_model;
    if (zed_detection_model == "MULTI_CLASS_BOX") {
        detection_model = sl::OBJECT_DETECTION_MODEL::MULTI_CLASS_BOX_MEDIUM;
        cerr << "DETECTION MODEL: " << "MULTI_CLASS_BOX_MEDIUM" << endl;
    } else if (zed_detection_model == "MULTI_CLASS_BOX_FAST") {
        detection_model = sl::OBJECT_DETECTION_MODEL::MULTI_CLASS_BOX_FAST;
        cerr << "DETECTION MODEL: " << "MULTI_CLASS_BOX_FAST" << endl;
    } else if (zed_detection_model == "MULTI_CLASS_BOX_MEDIUM") {
        detection_model = sl::OBJECT_DETECTION_MODEL::MULTI_CLASS_BOX_MEDIUM;
        cerr << "DETECTION MODEL: " << "MULTI_CLASS_BOX_MEDIUM" << endl;
    } else if (zed_detection_model == "MULTI_CLASS_BOX_ACCURATE") {
        detection_model = sl::OBJECT_DETECTION_MODEL::MULTI_CLASS_BOX_ACCURATE;
        cerr << "DETECTION MODEL: " << "MULTI_CLASS_BOX_ACCURATE" << endl;
//     } else if (zed_detection_model == "HUMAN_BODY_FAST") {
//         detection_model = sl::OBJECT_DETECTION_MODEL::HUMAN_BODY_FAST;
//         cerr << "DETECTION MODEL: " << "HUMAN_BODY_FAST" << endl;
//     } else if (zed_detection_model == "HUMAN_BODY_ACCURATE") {
//         detection_model = sl::OBJECT_DETECTION_MODEL::HUMAN_BODY_ACCURATE;
//         cerr << "DETECTION MODEL: " << "HUMAN_BODY_ACCURATE" << endl;
//     } else if (zed_detection_model == "MULTI_CLASS_BOX_MEDIUM") {
//         detection_model = sl::OBJECT_DETECTION_MODEL::MULTI_CLASS_BOX_MEDIUM;
//         cerr << "DETECTION MODEL: " << "MULTI_CLASS_BOX_MEDIUM" << endl;
//     } else if (zed_detection_model == "HUMAN_BODY_MEDIUM") {
//         detection_model = sl::OBJECT_DETECTION_MODEL::HUMAN_BODY_MEDIUM;
//         cerr << "DETECTION MODEL: " << "HUMAN_BODY_MEDIUM" << endl;
    } else if (zed_detection_model == "PERSON_HEAD_BOX_ACCURATE") {
        detection_model = sl::OBJECT_DETECTION_MODEL::PERSON_HEAD_BOX_ACCURATE;
        cerr << "DETECTION MODEL: " << "PERSON_HEAD_BOX_ACCURATE" << endl;
    } else if (zed_detection_model == "PERSON_HEAD_BOX_FAST") {
        detection_model = sl::OBJECT_DETECTION_MODEL::PERSON_HEAD_BOX_FAST;
        cerr << "DETECTION MODEL: " << "PERSON_HEAD_BOX_FAST" << endl;
    } else {
        detection_model = sl::OBJECT_DETECTION_MODEL::MULTI_CLASS_BOX_MEDIUM;
        cerr << "DETECTION MODEL: " << "MULTI_CLASS_BOX_MEDIUM" << endl;
    }

//         enum class OBJECT_CLASS {
//         PERSON = 0, /**< For people detection */
//         VEHICLE = 1, /**< For vehicle detection. It can be cars, trucks, buses, motorcycles etc */
//         BAG = 2, /**< For bag detection (backpack, handbag, suitcase) */
//         ANIMAL = 3, /**< For animal detection (cow, sheep, horse, dog, cat, bird, etc) */
//         ELECTRONICS = 4, /**< For electronic device detection (cellphone, laptop, etc) */
//         FRUIT_VEGETABLE = 5, /**<  For fruit and vegetable detection (banana, apple, orange, carrot, etc) */

    if ( classesToShow.compare ( "all" ) == 0 ) {
        cltosh = {};
    } else {
        //prende tutte le classi che sono tra le virgole
        cerr << "Classi da inserire: " << classesToShow << endl;
        typedef vector<string> T_V_Split;
        T_V_Split mtfa_string_split ( const string& str, const string& delimiters );
        T_V_Split mtfa_string_split2 ( const string& str, const string& delimiter );
        string mtfa_replace_matching_pattern ( const string & data, const string & pattern, const string & replace, bool all );
        auto vs = mtfa_string_split (
                      mtfa_replace_matching_pattern (
                          mtfa_replace_matching_pattern (
                              mtfa_replace_matching_pattern ( classesToShow, "\"", "", true ),
                              ", ", ",", true ),
                          " ,", ",", true ),
                      "," );
        for ( auto & it: vs ) {
            cerr << "Inserisco classe: " << it << endl;
            if (SToUpper(it) == string("ANIMAL"))
                cltosh.push_back(sl::OBJECT_CLASS::ANIMAL);
            else if (SToUpper(it) == string("BAG"))
                cltosh.push_back(sl::OBJECT_CLASS::BAG);
            else if (SToUpper(it) == string("ELECTRONICS"))
                cltosh.push_back(sl::OBJECT_CLASS::ELECTRONICS);
            else if (SToUpper(it) == string("FRUIT_VEGETABLE"))
                cltosh.push_back(sl::OBJECT_CLASS::FRUIT_VEGETABLE);
            else if (SToUpper(it) == string("PERSON"))
                cltosh.push_back(sl::OBJECT_CLASS::PERSON);
            else if (SToUpper(it) == string("VEHICLE"))
                cltosh.push_back(sl::OBJECT_CLASS::VEHICLE);
            else if (SToUpper(it) == string("SPORT"))
                cltosh.push_back(sl::OBJECT_CLASS::SPORT);
        }
    }

    //Iniziano i lavori!!
//     init_params.depth_minimum_distance = 2;
//     init_params.depth_maximum_distance = 10000.0;
    init_params.depth_mode = camera_depth_mode; // sl::DEPTH_MODE::PERFORMANCE;
    init_params.camera_resolution = camera_resolution; // sl::RESOLUTION::HD720; //sl::RESOLUTION::HD720;// sl::RESOLUTION::HD1080, sl::RESOLUTION::HD720
    init_params.coordinate_units = sl::UNIT::METER;
    init_params.open_timeout_sec = open_timeout_sec;
    init_params.svo_real_time_mode = svo_real_time_mode;
    init_params.sdk_verbose = true;
    init_params.camera_fps = 30;
    init_params.camera_image_flip=sl::FLIP_MODE::ON;
    init_params.coordinate_system = sl::COORDINATE_SYSTEM::RIGHT_HANDED_Y_UP;
    //init_params.enable_right_side_measure = true;


    //dichiarati prima del ciclo per evitare errori di compilazione
    sl::ERROR_CODE returned_state;
    vector<cv::Scalar> colors;
    cv::RNG rng;
    //Create ZED Objects filled in the main loop
    sl::Mat image;
    cv::Mat cvmat;
    cv::cuda::GpuMat gpumat;
    unsigned long long numframes = 0;
    int removed = 0;
    int dropped = 0;
    int numerrors;

ciclo:  //per recuperare in caso di errori continuativi
    cerr << "Loop starts\n";
    if (video.find(".svo") != string::npos)
    {
        //input da file
        init_params.input.setFromSVOFile(video.c_str());
    } else if (video == "zed_camera" && zed_camera_ip != "")
    {
        unsigned int a, b, c, d, port;
        cerr << "Apre camera ip: " << zed_camera_ip << endl;
        if (sscanf(zed_camera_ip.c_str(), "%u.%u.%u.%u:%d", &a, &b, &c, &d, &port) == 5) {
            // Stream input mode - IP + port
            string ip_address = to_string(a) + "." + to_string(b) + "." + to_string(c) + "." + to_string(d);
            init_params.input.setFromStream(sl::String(ip_address.c_str()), port);
            cerr << "Input from stream: " << ip_address << ":" << port << endl;
        } else {
            cerr << "Scanning ip and port: <" << zed_camera_ip << ">" << endl;
            return SCM_BOOL_F;
        }
    }

    // Open the camera
    if (zed.isOpened()) {
        zed.close();
    }

    returned_state = zed.open(init_params);
    if (returned_state != sl::ERROR_CODE::SUCCESS) {
        cerr << "Open Camera " << returned_state << "\nExit program." << endl;
        zed.close();
        usleep(1000000); //pensaci un pochino!
        goto ciclo;
    }
    
    {
        // Print camera information
        auto camera_info = zed.getCameraInformation();
        cout << endl;
        cout << "ZED Model                 : " << camera_info.camera_model << endl;
        cout << "ZED Serial Number         : " << camera_info.serial_number << endl;
        cout << "ZED Camera Firmware       : " << camera_info.camera_configuration.firmware_version << "/" << camera_info.sensors_configuration.firmware_version << endl;
        cout << "ZED Camera Resolution     : " << camera_info.camera_configuration.resolution.width << "x" << camera_info.camera_configuration.resolution.height << endl;
        cout << "ZED Camera FPS            : " << zed.getInitParameters().camera_fps << endl;
    }

    //alcuni valori di default che settiamo
//     static int zed_camera_brightness = 6;
//     static int zed_camera_contrast = 6;
//     static int zed_camera_sharpness = 3;
//     static int zed_camera_saturation = 7;
//
//     zed.setCameraSettings(sl::VIDEO_SETTINGS::BRIGHTNESS, zed_camera_brightness);
//     zed.setCameraSettings(sl::VIDEO_SETTINGS::CONTRAST, zed_camera_contrast);
//     zed.setCameraSettings(sl::VIDEO_SETTINGS::SHARPNESS, zed_camera_sharpness);
//     zed.setCameraSettings(sl::VIDEO_SETTINGS::SATURATION, zed_camera_saturation);
    zed.disableRecording();

    //Enable Positional tracking (mandatory for object detection)
    //If the camera is static, uncomment the following line to have better performances and boxes sticked to the ground.
    positional_tracking_parameters.set_as_static = isStationary;
    positional_tracking_parameters.enable_area_memory = true;

    returned_state = zed.enablePositionalTracking(positional_tracking_parameters);
    if (returned_state != sl::ERROR_CODE::SUCCESS) {
        cerr << "enablePositionalTracking " << returned_state << "\nExit program." << endl;
        zed.close();
        usleep(100000); //pensaci un pochino!
        goto ciclo;
    }

    // Enable the Objects detection module
    obj_det_params.enable_tracking = true;  //sempre!!
    obj_det_params.detection_model = detection_model;
    obj_det_params.enable_segmentation = false;
    obj_det_params.enable_tracking = false;
    //obj_det_params.enable_mask_output = true;
    obj_det_params.batch_parameters.enable = false;
    //obj_det_params.image_sync = true;
    returned_state = zed.enableObjectDetection(obj_det_params);
    if (returned_state != sl::ERROR_CODE::SUCCESS) {
        cerr << "enableObjectDetection " << returned_state << "\nExit program." << endl;
        zed.close();
        usleep(100000); //pensaci un pochino!
        goto ciclo;
    }

    camera_info = zed.getCameraInformation().camera_configuration;

    // Configure object detection runtime parameters
    cerr << "CONFIDENCE: " << thresh << endl;
    objectTracker_parameters_rt.detection_confidence_threshold = thresh; // = sl::ObjectDetectionRuntimeParameters(thresh);
    //.detection_confidence_threshold = thresh;

    // To select a set of specific object classes, like car, bicycle and bus for instance:
    objectTracker_parameters_rt.object_class_filter = cltosh; //{sl::OBJECT_CLASS::PERSON}; //cltosh;   //, OBJECT_CLASS::VEHICLE, OBJECT_CLASS::ANIMAL };
    // To set a specific threshold
    //objectTracker_parameters_rt.object_class_detection_confidence_threshold[sl::OBJECT_CLASS::PERSON] = thresh;
// 	//detection_parameters_rt.object_class_detection_confidence_threshold[OBJECT_CLASS::CAR] = 35;

    //Questo non è più un thread
//     auto mythread = [] (bool show_original, bool windows, SCM callback, SCM rt_callback, sl::ObjectDetectionRuntimeParameters objectTracker_parameters_rt, int remove_frames, Zed2Detector * detector) {
    //creo un array di colori
    for ( int i = 0; i < 256; i++ ) {
        int r = rng.uniform ( 0, 256 );
        int g = rng.uniform ( 0, 256 );
        int b = rng.uniform ( 0, 256 );
        colors.push_back ( cv::Scalar ( r,g,b ) );
    }


//     unsigned long long old_measure = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//     int numobjects = 0;

//     sl::RuntimeParameters rt_params;
//     rt_params.sensing_mode = sl::SENSING_MODE::STANDARD;
//     rt_params.enable_depth = true;
//     rt_params.confidence_threshold = 100;
//     rt_params.texture_confidence_threshold = 100;
//     rt_params.measure3D_reference_frame = sl::REFERENCE_FRAME::CAMERA;


    // int objects_is_new = 0; //se non è news per più di tre volte, resetta la camera!!!
    numerrors = 0;
    uint64_t frame_id = 0;
    while (!stop) {
        //cout << "Frame: " << frame_id << endl;
        frame_id++;
        
        //cerr << "GRAB fa la grab..." << endl;
        auto grab_result = zed.grab(); //rt_params);
        //cerr << "GRAB RESULT: " << grab_result << endl;
        if (grab_result == sl::ERROR_CODE::SUCCESS) {
            numerrors=0;  //se non ci sono errori resetto il contatore!

            // Retrieve left image. Ma la ritrova sempre? certo altrimenti non farebbe un bel nulla
            // la rtcallback è sempre esaudita
            dropped++;
            if (dropped > drop_frames) {
                //zed.retrieveImage(image, sl::VIEW::LEFT, sl::MEM::CPU);
                //cerr << "GRAB Retrieve image" << endl;
                auto error = zed.retrieveImage(image, sl::VIEW::RIGHT_UNRECTIFIED, sl::MEM::CPU); //sl::MEM::GPU); //, sl::MEM::CPU);
                if (error != sl::ERROR_CODE::SUCCESS) {
                    cerr << "Error retrieving image..." << endl;
                    numerrors++;
                    zed.reboot(0, false);
                    goto ciclo;
                }
                //cv::cvtColor (Zed_slMat2cvMat(image), cvmat, CV_RGBA2RGB );
                //cv::cvtColor (slMat2cvMat(image), cvmat, cv::COLOR_RGBA2RGB );
                //cvmat = slMat2cvMat(image);
                
                
                //cv::cvtColor(slMat2cvMat(image), cvmat, cv::COLOR_BGRA2BGR);
                
                //cerr << "GRAB cvtColor" << endl;
                //Prima converto in gpu
//                 cv::cuda::GpuMat gpucvmat = Zed_slMat2cvMatGPU(image);
//                 
//                 try {
//                     //poi converto in cvmat
//                     cvmat = cv::Mat(gpucvmat);
// 
//                     //poi converto colore???
//                     cv::cvtColor(cvmat, cvmat, cv::COLOR_BGRA2BGR);
//                 } catch (exception ex) {
//                     cerr << "Eccezione in Zed_slMat2cvMatGPU: " << ex.what() << endl;
//                     numerrors++;
//                     cerr << "Camera error!!!\n";
//                     zed.close();
//                     //zed.reboot(0, false);
//                     cerr << "Restarts....\n";
//                     goto ciclo;
//                     continue;
//                 }
                //cerr << "GRAB exited cvtColor" << endl;
                cv::Mat cvImage(image.getHeight(), image.getWidth(), (image.getChannels() == 1) ? CV_8UC1 : CV_8UC4, image.getPtr<sl::uchar1>(sl::MEM::CPU));
                cv::cvtColor(cvImage, cvmat, cv::COLOR_BGRA2BGR);
                dropped = 0;
            } else {
                continue;
            }

            //se rt_callback è attiva, la esegue
            if (scm_is_null(rt_callback) == 0)
            {
                //cerr << "GRAB zed_call_rt_callback" << endl;
                zed_call_rt_callback(&cvmat, camera_info.fps/(drop_frames+1), camera_info.resolution.width, camera_info.resolution.height, rt_callback);
                //cerr << "GRAB exited zed_call_rt_callback" << endl;
            }

            if (removed < remove_frames)
            {
                removed++;
                continue;
            } else {
                removed = 0;
            }

//             //i centri reali dell'immagine???
//             {
//                 sl::Mat depth;
//                 sl::uchar4 centerBGRA;
//                 image.getValue<sl::uchar4>(image.getWidth() / 2, image.getHeight() / 2, &centerBGRA);
//                 std::cout << "Center pixel B:" << (int)centerBGRA[0] << " G:" << (int)centerBGRA[1] << " R:" << (int)centerBGRA[2] << std::endl;
//
//                 //Display a pixel depth
//                 zed.retrieveMeasure(depth, sl::MEASURE::DEPTH); // Get the depth map
//                 float centerDepth;
//                 depth.getValue<float>(depth.getWidth() / 2, depth.getHeight() / 2, &centerDepth);
//                 std::cout << "Center depth:" << centerDepth << std::endl;
//             }

            //Ora per avere un flusso continuo, questa dovrebbe essere richiamata dentro una coda

//             //il tutto è ritrovato in tempo reale e poi è chiamata la callback di valutazione
//             auto start = std::chrono::high_resolution_clock::now();

            sl::Objects objects;
//             static long long called = 0;
//             static long long retrieved = 0;
            //cerr << "GRAB Retrieving objects " << ++called << endl;
            auto ret_state = //sl::ERROR_CODE::LAST; //
                             zed.retrieveObjects(objects, objectTracker_parameters_rt);
// 	    retrieved += objects.object_list.size(); 
// 	    if (called++ > 1000) {
// 		    called = 0;
// 		    cerr << "GRAB Retrieved so far: " << retrieved << " objects" << endl;
// 	    }
            //cerr << "GRAB Retrieved objects" << endl;
            //impiega circa 50ms. mentre in remote dura circa 20ms. ora a 30 fps ogni frame dura 1000/30=30ms, quindi devo mettere in coda altrimenti avrei un flusso bilanciato
//             cout << ret_state << ": " << objectTracker_parameters_rt.detection_confidence_threshold << ", " <<
//             objects.object_list.size() << endl;
            
            //             auto elapsed = std::chrono::high_resolution_clock::now() - start;
//
//             cout << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << endl;
             

            numframes++;
//   0, 1279, 0, 579 ====-19.4258, 14.8199, -3.65274, 3.29791, -22.6145, 0
//   0, 1279, 0, 719 ====-19.9373, 21.0044, -4.32697, 4.7489,  -26.63, 0


//                 unsigned long long new_measure = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//                 if ((new_measure-old_measure) > 1000)
//                 {
//                     cout << "Esaminati: " << numframes << " in " << new_measure-old_measure << " ms\n";
//                     cout << "Tot Objs: " << numobjects << endl;
//                     old_measure = new_measure;
//                     numframes=0;
//                     numobjects = 0;
//                 }
//
//             numobjects+=objects.object_list.size();
//             continue;


//             gpumat = Zed_slMat2cvMatGPU(image);
//             gpumat.download(cvmat);


            if (show_original || windows)
            {
                if ( show_original ) {
                    //cv::namedWindow ( "original", 0 );
                    cv::imshow("original", cvmat);
                }
                if ( windows ) {
                    //Disegno rettangolo intorno agli oggetti
                    if (ret_state == sl::ERROR_CODE::SUCCESS && objects.is_new) {
                        zed_draw_boxes ( cvmat, objects.object_list, colors);
                        cv::imshow("computed", cvmat);
                    }
                }
                if (113 == cv::waitKey ( 1 ))
                    stop = true;
            }

            if (scm_is_null(callback) == 0)
            {
                //cout << "Chiama, forse, lacallback..." << endl;
//                 if (!objects.is_new && objects_is_new++ > 30) {
//                     zed.disableObjectDetection();
//                     zed.enableObjectDetection(obj_det_params);
//                     zed.close();
// //                     zed.reboot(0, false);
//                     cerr << "Restarts for news....\n";
//                     goto ciclo;
//                 } else
                if (ret_state == sl::ERROR_CODE::SUCCESS && objects.is_new) {
                    //cout << "La Chiama per davvero..." << endl;
                    //cerr << "GRAB zed2_send_json_http " << endl;
                    //objects_is_new = 0;
                    zed2_send_json_http ( cvmat, frame_id, camera_info.fps/(drop_frames+1), camera_info.resolution.width, camera_info.resolution.height, objects, callback);
                    //cerr << "exited GRAB zed2_send_json_http " << endl;
                }
            }
        } else {
            numerrors++;
            cerr << "Camera error!!!\n";
            zed.reboot(0, false);
            cerr << "Restarts....\n";
            goto ciclo;

            usleep(1000);
            if (numerrors > 5) { //Riparto!
                cerr << "Restarts....\n";
                goto ciclo;
            }
        }
    }

    cerr << "Runner terminated\n";

    // Release objects
    image.free();
    //objects.object_list.clear();

    // Disable modules
    zed.disableObjectDetection();
    zed.disablePositionalTracking();
    zed.close();

    if (show_original)
        cv::destroyWindow ( "original" );
    if (windows)
        cv::destroyWindow ( "computed" );

// 	//eseguo il thread!!
// 	std::thread runme(mythread, show_original, windows, callback, rt_callback, objectTracker_parameters_rt, remove_frames, this);
// 	runme.detach();

    //terminates...
    stopped = true;
    return SCM_BOOL_T;
}
// */


static SCM mtfa_opencv_rgbtohsv (SCM R, SCM G, SCM B)
{
//  fR Red component, used as output, range: [0, 1]
//  fG Green component, used as output, range: [0, 1]
//  fB Blue component, used as output, range: [0, 1]
//  fH Hue component, used as input, range: [0, 360] => 360
//  fS Hue component, used as input, range: [0, 1]   => 100
//  fV Hue component, used as input, range: [0, 1]   => 100

    float fR, fG, fB, fH, fS, fV;
    fR = scm_to_double(R);
    fR /= 256.0;
    fG = scm_to_double(G);
    fG /= 256.0;
    fB = scm_to_double(B);
    fB /= 256.0;

    float fCMax = max(max(fR, fG), fB);
    float fCMin = min(min(fR, fG), fB);
    float fDelta = fCMax - fCMin;

    if(fDelta > 0) {
        if(fCMax == fR) {
            fH = 60 * (fmod(((fG - fB) / fDelta), 6));
        } else if(fCMax == fG) {
            fH = 60 * (((fB - fR) / fDelta) + 2);
        } else if(fCMax == fB) {
            fH = 60 * (((fR - fG) / fDelta) + 4);
        }

        if(fCMax > 0) {
            fS = fDelta / fCMax;
        } else {
            fS = 0;
        }

        fV = fCMax;
    } else {
        fH = 0;
        fS = 0;
        fV = fCMax;
    }

    if(fH < 0) {
        fH = 360 + fH;
    }

    uint uiH = fH;
    uint uiS = fS * 100.0;
    uint uiV = fV * 100.0;
    return scm_values_3(scm_from_uint(uiH), scm_from_uint(uiS), scm_from_uint(uiV));
}

static SCM	mtfa_opencv_create_trackbar(SCM string_label, SCM string_window_name, SCM int_initial_value, SCM int_max_value, SCM call_TrackbarCallback, SCM void_userdata)
{
    //typedef void (*TrackbarCallback)(int pos, void* userdata);
    string label = ScmToString(string_label);
    string window_name = ScmToString(string_window_name);
    int initial_value = scm_to_int(int_initial_value);
    int max_value = scm_to_int(int_max_value);

    int n = createTrackbar(label, window_name, nullptr, max_value, [] (int pos, void * userdata) {
        SCM scm_pos = scm_from_int(pos);
        scm_call_2(scm_car((SCM)userdata), scm_pos, scm_cdr((SCM)userdata));
    }, (void*)scm_cons(call_TrackbarCallback, void_userdata));
    setTrackbarPos(label, window_name, initial_value);
    return scm_from_int(n);
}

static SCM mtfa_opencv_set_mouse_callback(SCM string_window_name, SCM call_MouseCallback, SCM void_userdata)
{
    string window_name = ScmToString(string_window_name);
    setMouseCallback(window_name, [] (int event, int x, int y, int flags, void* userdata) {
        SCM scm_event = scm_from_int(event);
        SCM scm_x = scm_from_int(x);
        SCM scm_y= scm_from_int(y);
        SCM scm_flags= scm_from_int(flags);
        scm_call_5(scm_car((SCM)userdata), scm_event, scm_x, scm_y, scm_flags, scm_cdr((SCM)userdata));
    }, (void*)scm_cons(call_MouseCallback, void_userdata));
    return SCM_BOOL_T;
}

static SCM mtfa_opencv_set_trackbar_max(SCM string_label, SCM string_window_name, SCM int_value)
{
    string label = ScmToString(string_label);
    string window_name = ScmToString(string_window_name);
    int value = scm_to_int(int_value);
    setTrackbarMax(label, window_name, value);
    return SCM_BOOL_T;
}
static SCM mtfa_opencv_set_trackbar_min(SCM string_label, SCM string_window_name, SCM int_value)
{
    string label = ScmToString(string_label);
    string window_name = ScmToString(string_window_name);
    int value = scm_to_int(int_value);
    setTrackbarMin(label, window_name, value);
    return SCM_BOOL_T;
}
static SCM mtfa_opencv_set_trackbar_pos(SCM string_label, SCM string_window_name, SCM int_value)
{
    string label = ScmToString(string_label);
    string window_name = ScmToString(string_window_name);
    int value = scm_to_int(int_value);
    setTrackbarPos(label, window_name, value);
    return SCM_BOOL_T;
}
static SCM mtfa_opencv_get_trackbar_pos(SCM string_label, SCM string_window_name)
{
    string label = ScmToString(string_label);
    string window_name = ScmToString(string_window_name);
    int value = getTrackbarPos(label, window_name);
    return scm_from_int(value);
}

static SCM mtfa_opencv_find_circles (SCM scm_mat, SCM int_dp, SCM int_minDist, SCM int_p1, SCM int_p2, SCM int_minr, SCM int_maxr, SCM int_blur, SCM scm_polygon)
{
    auto img = ( cv::Mat* ) scm_to_pointer ( scm_mat );

    Mat gray;

    int blur = 5;
    int dp = 2;
    int minDist = 54;
    int p1 = 90;
    int p2 = 40;
    int minr = 7;
    int maxr = 19;
    std::vector<cv::Point2f> * points = nullptr;

    if (!scm_is_null(int_blur))
        blur = scm_to_int(int_blur);
    if (!scm_is_null(int_dp))
        dp = scm_to_int(int_dp);
    if (!scm_is_null(int_minDist))
        minDist = scm_to_int(int_minDist);
    if (!scm_is_null(int_p1))
        p1 = scm_to_int(int_p1);
    if (!scm_is_null(int_p2))
        p2 = scm_to_int(int_p2);
    if (!scm_is_null(int_minr))
        minr = scm_to_int(int_minr);
    if (!scm_is_null(int_maxr))
        maxr = scm_to_int(int_maxr);
    if (!scm_is_null(scm_polygon))
        points = (std::vector<cv::Point2f>*)scm_to_pointer(scm_polygon);


// dp	Inverse ratio of the accumulator resolution to the image resolution.
//      For example, if dp=1 , the accumulator has the same resolution as the input image.
//      If dp=2 , the accumulator has half as big width and height. For
//      HOUGH_GRADIENT_ALT the recommended value is dp=1.5, unless some small very
//      circles need to be detected.
// minDist	Minimum distance between the centers of the detected circles. If the
//          parameter is too small, multiple neighbor circles may be falsely detected
//          in addition to a true one. If it is too large, some circles may be missed.
// param1	First method-specific parameter. In case of HOUGH_GRADIENT and
//          HOUGH_GRADIENT_ALT, it is the higher threshold of the two passed to the
//           Canny edge detector (the lower one is twice smaller). Note that HOUGH_GRADIENT_ALT
//           uses Scharr algorithm to compute image derivatives, so the threshold value
//           shough normally be higher, such as 300 or normally exposed and contrasty images.
// param2	Second method-specific parameter. In case of HOUGH_GRADIENT, it is the
//          accumulator threshold for the circle centers at the detection stage. The
//          smaller it is, the more false circles may be detected. Circles, corresponding
//           to the larger accumulator values, will be returned first. In the case of
//          HOUGH_GRADIENT_ALT algorithm, this is the circle "perfectness" measure. The
//          closer it to 1, the better shaped circles algorithm selects. In most cases 0.9
//           should be fine. If you want get better detection of small circles, you may
//          decrease it to 0.85, 0.8 or even less. But then also try to limit the search
//          range [minRadius, maxRadius] to avoid many false circles.
// minRadius	Minimum circle radius.
// maxRadius	Maximum circle radius. If <= 0, uses the maximum image dimension.
//              If < 0, HOUGH_GRADIENT returns centers without finding the radius.
//              HOUGH_GRADIENT_ALT always computes circle radiuses.


    cvtColor(*img, gray, COLOR_BGR2GRAY);
    medianBlur( gray, gray, blur);

    vector<Vec3f> circles;
    try {
        HoughCircles(  gray,  circles, HOUGH_GRADIENT, dp, minDist, p1, p2, minr, maxr);
    } catch (cv::Exception ex)
    {
        return SCM_EOL; //nulla di cui
    }

    SCM alist = SCM_EOL;
    for (auto & it: circles)
    {
        cv::Point2f p;
        p.x = it[0];
        p.y = it[1];

        auto iret = (points==nullptr)?0:(cv::pointPolygonTest(*points, p, false));
        if (iret >= 0) // && it[2] >= minr && it[2] <= maxr)
        {
            alist = scm_cons(scm_vector(scm_list_3(scm_from_short(it[0]), scm_from_short(it[1]), scm_from_short(it[2]))), alist);
        }
    }
    return alist;
}


static SCM mtfa_opencv_img_mean (SCM mat_img, SCM int_center_x, SCM int_center_y, SCM int_side)
{
    auto img = ( cv::Mat* ) scm_to_pointer ( mat_img );

    int center_x = scm_to_int(int_center_x);
    int center_y = scm_to_int(int_center_y);
    int side = scm_to_int(int_side);

    double numcells = (side*2+1)*(side*2+1);

    //1 o 3, gray or colored
    if (img->channels() == 1) {
        double mean = 0.0;
        for (int c = -side; c<=side; ++c) {
            for (int r=--side; r<=side; ++r) {
                auto pcolor = img->at<Vec3b>(Point(center_x+c,center_y+r));
                mean += pcolor[0];
            }
        }
        mean /= numcells;
        float v[3];
        v[0]=v[1]=v[2]=mean;
        return scm_vector(scm_list_3(scm_from_double(v[0]), scm_from_double(v[1]), scm_from_double(v[2])));
    } else {
        cv::Scalar mean = {0.0, 0.0, 0.0};
        for (int c = -side; c <= side; ++c) {
            for (int r = -side; r <= side; ++r) {
                auto pcolor = img->at<Vec3b>(Point(center_x+c,center_y+r));
                mean[0] += pcolor[0];
                mean[1] += pcolor[1];
                mean[2] += pcolor[2];
            }
        }
        mean[0] /= numcells;
        mean[1] /= numcells;
        mean[2] /= numcells;
        float v[3];
        v[0]=mean[0];
        v[1]=mean[1];
        v[2]=mean[2];
        return scm_vector(scm_list_3(scm_from_double(v[0]), scm_from_double(v[1]), scm_from_double(v[2])));
    }
}

//PH-TREE, bi e 3 dimensionali
#include <phtree.h>

// static void release ( void * mat )
// {
//     if (mat) {
//         delete ( cv::Mat* ) mat;
//     }
// }

using namespace improbable::phtree;

/*  Facciamola semplice 2d e 3d, solo int
typedef enum {
    phtree_whole_point_2_int=0,
    phtree_whole_point_3_int,
    phtree_whole_point_2_double,
    phtree_whole_point_3_double,
    phtree_whole_point_2_string,
    phtree_whole_point_3_string,
    phtree_whole_box_2_int,
    phtree_whole_box_3_int,
    phtree_whole_box_2_double,
    phtree_whole_box_3_double,
    phtree_whole_box_2_string,
    phtree_whole_box_3_string,
} T_PHTREE_WHOLE_TYPE;

typedef struct __phtree_type__ {
//     T_PHTREE_KEY_TYPE tree_key_type; //0: point ,1: boxe
//     T_PHTREE_VALUE_TYPE tree_value_type; //0: int, 1: double, 2: string
//     int dimensions; //2 or 3
    T_PHTREE_WHOLE_TYPE tree_whole_type;
    void * pt;  //l'oggetto vero e proprio
} T_PHTREE_SCM;

static void release_phtree(void * _p) {
    T_PHTREE_SCM * p = (T_PHTREE_SCM*)_p;
    switch (p->tree_whole_type) {
        case phtree_whole_point_2_int:
            delete (PhTreeD<2, int>*)p->pt;
            break;
        case phtree_whole_point_3_int:
            delete (PhTreeD<3, int>*)p->pt;
            break;
        case phtree_whole_point_2_double:
            delete (PhTreeD<2, double>*)p->pt;
            break;
        case phtree_whole_point_3_double:
            delete (PhTreeD<3, double>*)p->pt;
            break;
        case phtree_whole_point_2_string:
            delete (PhTreeD<2, string>*)p->pt;
            break;
        case phtree_whole_point_3_string:
            delete (PhTreeD<3, string>*)p->pt;
            break;
        case phtree_whole_box_2_int:
            delete (PhTreeBoxD<2, int>*)p->pt;
            break;
        case phtree_whole_box_3_int:
            delete (PhTreeBoxD<3, int>*)p->pt;
            break;
        case phtree_whole_box_2_double:
            delete (PhTreeBoxD<2, double>*)p->pt;
            break;
        case phtree_whole_box_3_double:
            delete (PhTreeBoxD<3, double>*)p->pt;
            break;
        case phtree_whole_box_2_string:
            delete (PhTreeBoxD<2, string>*)p->pt;
            break;
        case phtree_whole_box_3_string:
            delete (PhTreeBoxD<3, string>*)p->pt;
            break;
    }
};

static SCM mtfa_phtree_make(SCM _key_type, SCM _dimensions, SCM _value_type) {
    if (scm_is_string(_key_type) == 0)   scm_wrong_type_arg_msg("mtfa_phtree_make" , 0, _key_type, "string: point, box");  //E qui esce
    if (scm_is_string(_value_type) == 0) scm_wrong_type_arg_msg("mtfa_phtree_make" , 0, _value_type, "string: int, double, string");  //E qui esce
    if (scm_is_integer(_dimensions) == 0) scm_wrong_type_arg_msg("mtfa_phtree_make" , 0, _dimensions, "int");  //E qui esce

    string key_type = scm_to_locale_string(_key_type);
    if (key_type != "point" && key_type != "box") scm_wrong_type_arg_msg("mtfa_phtree_make" , 0, _key_type, "string: point, box");  //E qui esce

    string value_type = scm_to_locale_string(_value_type);
    if (value_type != "int" && value_type != "double" && value_type != "string" && value_type != "pointer" )
        scm_wrong_type_arg_msg("mtfa_phtree_make" , 0, _value_type, "string: int, double, string, pointer");  //E qui esce

    int dimensions = scm_to_int(_dimensions);
    if (dimensions != 2 && dimensions != 3) scm_wrong_type_arg_msg("mtfa_phtree_make" , 0, _dimensions, "int: 2 or 3");  //E qui esce
    //Costruisco il contenitore

    T_PHTREE_WHOLE_TYPE wt;
    T_PHTREE_SCM * pt = new T_PHTREE_SCM();
    if (dimensions==2) {
        if (key_type == "point") {
            if (value_type == "int") {
                pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_point_2_int;
                pt->pt = new PhTreeD<2, int>();
            } else if (value_type == "double") {
                pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_point_2_double;
                pt->pt = new PhTreeD<2, double>();
            } else {
                pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_point_2_string;
                pt->pt = new PhTreeD<2, string>();
            }
        }else {
            if (value_type == "int") {
                pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_box_2_int;
                pt->pt = new PhTreeD<3, int>();
            } else if (value_type == "double") {
                pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_box_2_double;
                pt->pt = new PhTreeD<3, double>();
            } else {
                pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_box_2_string;
                pt->pt = new PhTreeD<3, string>();
            }
        }
    } else if (key_type == "point") {
        if (value_type == "int") {
            pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_point_3_int;
            pt->pt = new PhTreeBoxD<2, int>();
        } else if (value_type == "double") {
            pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_point_3_double;
            pt->pt = new PhTreeBoxD<2, double>();
        } else {
            pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_point_3_string;
            pt->pt = new PhTreeBoxD<2, string>();
        }
    } else {
        if (value_type == "int") {
            pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_box_3_int;
            pt->pt = new PhTreeBoxD<3, int>();
        } else if (value_type == "double") {
            pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_box_3_double;
            pt->pt = new PhTreeBoxD<3, double>();
        } else {
            pt->tree_whole_type = T_PHTREE_WHOLE_TYPE::phtree_whole_box_3_string;
            pt->pt = new PhTreeBoxD<3, string>();
        }
    }
    return scm_from_pointer ( pt, release_phtree );
}
static SCM mtfa_phtree_insert_points(SCM _phtree_ptr, SCM scm_list_of_points) {
    auto phtree_ptr = ( T_PHTREE_SCM* ) scm_to_pointer ( _phtree_ptr );
    switch (phtree_ptr->tree_whole_type) {
        case phtree_whole_point_2_int:
            auto ok = ((PhTreeD<2, int>*)phtree_ptr->pt)->emplace(;
            break;
        case phtree_whole_point_3_int:
            auto ok = ((PhTreeD<3, int>*)phtree_ptr->pt);
            break;
        case phtree_whole_point_2_double:
            auto ok = ((PhTreeD<2, double>*)phtree_ptr->pt);
            break;
        case phtree_whole_point_3_double:
            auto ok = ((PhTreeD<3, double>*)phtree_ptr->pt);
            break;
        case phtree_whole_point_2_string:
            auto ok = ((PhTreeD<2, string>*)phtree_ptr->pt);
            break;
        case phtree_whole_point_3_string:
            auto ok = ((PhTreeD<3, string>*)phtree_ptr->pt);
            break;
        case phtree_whole_box_2_int:
            auto ok = ((PhTreeBoxD<2, int>*)phtree_ptr->pt);
            break;
        case phtree_whole_box_3_int:
            auto ok = ((PhTreeBoxD<3, int>*)phtree_ptr->pt);
            break;
        case phtree_whole_box_2_double:
            auto ok = ((PhTreeBoxD<2, double>*)phtree_ptr->pt);
            break;
        case phtree_whole_box_3_double:
            auto ok = ((PhTreeBoxD<3, double>*)phtree_ptr->pt);
            break;
        case phtree_whole_box_2_string:
            auto ok = ((PhTreeBoxD<2, string>*)phtree_ptr->pt);
            break;
        case phtree_whole_box_3_string:
            auto ok = ((PhTreeBoxD<3, string>*)phtree_ptr->pt);
            break;
    }
}
*/

static void release_phtree_box_2d(void * _p) {
    delete (PhTreeBoxD<2, int>*)_p;
};
static void release_phtree_box_3d(void * _p) {
    delete (PhTreeBoxD<3, int>*)_p;
};
static void release_phtree_point_2d(void * _p) {
    delete (PhTreeD<2, int>*)_p;
};
static void release_phtree_point_3d(void * _p) {
    delete (PhTreeD<3, int>*)_p;
};

static SCM mtfa_phtree_box_2d_make() {
    auto ptr = new PhTreeBoxD<2, int>();
    return scm_from_pointer ( (void*)ptr, release_phtree_box_2d);
}
static SCM mtfa_phtree_box_3d_make() {
    auto ptr = new PhTreeBoxD<3, int>();
    return scm_from_pointer ( (void*)ptr, release_phtree_box_3d);
}
static SCM mtfa_phtree_point_2d_make() {
    auto ptr = new PhTreeD<2, int>();
    return scm_from_pointer ( (void*)ptr, release_phtree_point_2d);
}
static SCM mtfa_phtree_point_3d_make() {
    auto ptr = new PhTreeD<3, int>();
    return scm_from_pointer ( (void*)ptr, release_phtree_point_3d);
}

//la lista di punti è una lista di liste di 3 elementi: x, y, id
static SCM mtfa_phtree_2d_add_points(SCM scm_ptr, SCM scm_list_of_points) {
    auto ptr = (PhTreeD<2, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    while (scm_list_of_points != SCM_EOL) {
        auto point = scm_car(scm_list_of_points);
        PhPointD<2> pt;
        pt[0] = scm_to_double(scm_car(point));
        pt[1] = scm_to_double(scm_cadr(point));
        int id = scm_to_int(scm_caddr(point));
        auto ret = ptr->emplace(pt, id);
        lista = scm_cons(scm_from_bool(ret.second), lista);
        scm_list_of_points = scm_cdr(scm_list_of_points);
    }
    return lista;
}
//la lista di punti è una lista di liste di 4 elementi: x, y, z, id
static SCM mtfa_phtree_3d_add_points(SCM scm_ptr, SCM scm_list_of_points) {
    auto ptr = (PhTreeD<3, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    while (scm_list_of_points != SCM_EOL) {
        auto point = scm_car(scm_list_of_points);
        PhPointD<3> pt;
        pt[0] = scm_to_double(scm_car(point));
        pt[1] = scm_to_double(scm_cadr(point));
        pt[2] = scm_to_double(scm_caddr(point));
        int id = scm_to_int(scm_cadddr(point));
        auto ret = ptr->emplace(pt, id);
        lista = scm_cons(scm_from_bool(ret.second), lista);
        scm_list_of_points = scm_cdr(scm_list_of_points);
    }
    return lista;
}

//la lista di box è una lista di liste di 3 elementi: ((tl: x, y)(br: x, y), id)
static SCM mtfa_phtree_2d_add_boxes(SCM scm_ptr, SCM scm_list_of_boxes) {
    auto ptr = (PhTreeBoxD<2, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    while (scm_list_of_boxes!= SCM_EOL) {
        auto boxes = scm_car(scm_list_of_boxes);
        PhPointD<2> tl, br;
        tl[0] = scm_to_double(scm_car (scm_car(boxes)));
        tl[1] = scm_to_double(scm_cadr(scm_car(boxes)));
        br[0] = scm_to_double(scm_car (scm_cadr(boxes)));
        br[1] = scm_to_double(scm_cadr(scm_cadr(boxes)));
        int id = scm_to_int(scm_caddr(boxes));
        PhBoxD<2> box(tl, br);
        auto ret = ptr->emplace(box, id);
        lista = scm_cons(scm_from_bool(ret.second), lista);
        scm_list_of_boxes = scm_cdr(scm_list_of_boxes);
    }
    return lista;
}
//la lista di box è una lista di liste di 3 elementi: ((tl: x, y, z)(br: x, y, z), id)
static SCM mtfa_phtree_3d_add_boxes(SCM scm_ptr, SCM scm_list_of_boxes) {
    auto ptr = (PhTreeBoxD<3, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    while (scm_list_of_boxes!= SCM_EOL) {
        auto boxes = scm_car(scm_list_of_boxes);
        PhPointD<3> tl, br;
        tl[0] = scm_to_double(scm_car  (scm_car(boxes)));
        tl[1] = scm_to_double(scm_cadr (scm_car(boxes)));
        tl[2] = scm_to_double(scm_caddr(scm_car(boxes)));
        br[0] = scm_to_double(scm_car  (scm_cadr(boxes)));
        br[1] = scm_to_double(scm_cadr (scm_cadr(boxes)));
        br[2] = scm_to_double(scm_caddr(scm_cadr(boxes)));
        int id = scm_to_int(scm_caddr(boxes));
        PhBoxD<3> box(tl, br);
        auto ret = ptr->emplace(box, id);
        lista = scm_cons(scm_from_bool(ret.second), lista);
        scm_list_of_boxes = scm_cdr(scm_list_of_boxes);
    }
    return lista;
}

//Manca la query! intersect o include?
static SCM mtfa_phtree_2d_query_points(SCM scm_ptr, SCM min_range, SCM max_range) {
    auto ptr = (PhTreeD<2, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    PhPointD<2> ptMin, ptMax;
    ptMin[0]=scm_to_double(scm_car(min_range));
    ptMin[1]=scm_to_double(scm_cadr(min_range));
    ptMax[0]=scm_to_double(scm_car(max_range));
    ptMax[1]=scm_to_double(scm_cadr(max_range));
    PhBoxD<2> box(ptMin, ptMax);
    for (auto it = ptr->begin_query(box); it != ptr->end(); ++it) {
        lista = scm_cons(scm_from_int(it.second()), lista);
    }
    return lista;
}
static SCM mtfa_phtree_3d_query_points(SCM scm_ptr, SCM min_range, SCM max_range) {
    auto ptr = (PhTreeD<3, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    PhPointD<3> ptMin, ptMax;
    ptMin[0]=scm_to_double(scm_car(min_range));
    ptMin[1]=scm_to_double(scm_cadr(min_range));
    ptMin[2]=scm_to_double(scm_caddr(min_range));
    ptMax[0]=scm_to_double(scm_car(max_range));
    ptMax[1]=scm_to_double(scm_cadr(max_range));
    ptMax[2]=scm_to_double(scm_caddr(max_range));
    PhBoxD<3> box(ptMin, ptMax);
    for (auto it = ptr->begin_query(box); it != ptr->end(); ++it) {
        lista = scm_cons(scm_from_int(it.second()), lista);
    }
    return lista;
}
static SCM mtfa_phtree_2d_query_boxes(SCM scm_ptr, SCM min_range, SCM max_range, SCM includep) {
    auto ptr = (PhTreeBoxD<2, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    PhPointD<2> ptMin, ptMax;
    ptMin[0]=scm_to_double(scm_car(min_range));
    ptMin[1]=scm_to_double(scm_cadr(min_range));
    ptMax[0]=scm_to_double(scm_car(max_range));
    ptMax[1]=scm_to_double(scm_cadr(max_range));
    PhBoxD<2> box(ptMin, ptMax);
    if (scm_to_bool(includep)) {
        for (auto it = ptr->begin_query(box, FilterNoOp(), QueryInclude()); it != ptr->end(); ++it) {
            lista = scm_cons(scm_from_int(it.second()), lista);
        }
    } else {
        for (auto it = ptr->begin_query(box, FilterNoOp(), QueryIntersect()); it != ptr->end(); ++it) {
            lista = scm_cons(scm_from_int(it.second()), lista);
        }
    }
    return lista;
}
static SCM mtfa_phtree_3d_query_boxes(SCM scm_ptr, SCM min_range, SCM max_range, SCM includep) {
    auto ptr = (PhTreeBoxD<3, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    PhPointD<3> ptMin, ptMax;
    ptMin[0]=scm_to_double(scm_car(min_range));
    ptMin[1]=scm_to_double(scm_cadr(min_range));
    ptMin[2]=scm_to_double(scm_caddr(min_range));
    ptMax[0]=scm_to_double(scm_car(max_range));
    ptMax[1]=scm_to_double(scm_cadr(max_range));
    ptMax[2]=scm_to_double(scm_caddr(max_range));
    PhBoxD<3> box(ptMin, ptMax);
    if (scm_to_bool(includep)) {
        for (auto it = ptr->begin_query(box, FilterNoOp(), QueryInclude()); it != ptr->end(); ++it) {
            lista = scm_cons(scm_from_int(it.second()), lista);
        }
    } else {
        for (auto it = ptr->begin_query(box, FilterNoOp(), QueryIntersect()); it != ptr->end(); ++it) {
            lista = scm_cons(scm_from_int(it.second()), lista);
        }
    }
    return lista;
}

static SCM mtfa_phtree_3d_knn_points(SCM scm_ptr, SCM scm_min_result_size, SCM scm_center) {
    auto ptr = (PhTreeD<3, int>*)scm_to_pointer(scm_ptr);
    PhPointD<3> pt_center;
    pt_center[0] = scm_to_double(scm_car(scm_center));
    pt_center[1] = scm_to_double(scm_cadr(scm_center));
    pt_center[2] = scm_to_double(scm_caddr(scm_center));
    SCM lista = SCM_EOL;
    size_t min_size = scm_to_int(scm_min_result_size);
    for (auto it = ptr->begin_knn_query(min_size, pt_center, DistanceEuclidean<3>()); it != ptr->end(); ++it) {
        lista = scm_cons(scm_cons(scm_from_int(it.second()), scm_from_double(it.distance())), lista);
    }
    return scm_reverse(lista);
}
static SCM mtfa_phtree_2d_knn_points(SCM scm_ptr, SCM scm_min_result_size, SCM scm_center) {
    auto ptr = (PhTreeD<2, int>*)scm_to_pointer(scm_ptr);
    PhPointD<2> pt_center;
    pt_center[0] = scm_to_double(scm_car(scm_center));
    pt_center[1] = scm_to_double(scm_cadr(scm_center));
    SCM lista = SCM_EOL;
    size_t min_size = scm_to_int(scm_min_result_size);
    for (auto it = ptr->begin_knn_query(min_size, pt_center, DistanceEuclidean<2>()); it != ptr->end(); ++it) {
        lista = scm_cons(scm_cons(scm_from_int(it.second()), scm_from_double(it.distance())), lista);
    }
    return scm_reverse(lista);
}

static SCM mtfa_phtree_2d_getall_points(SCM scm_ptr) {
    auto ptr = (PhTreeD<2, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    for (auto it = ptr->begin(); it != ptr->end(); ++it) {
        lista = scm_cons(scm_from_int(it.second()), lista);
    }
    return scm_reverse(lista);
}
static SCM mtfa_phtree_3d_getall_points(SCM scm_ptr) {
    auto ptr = (PhTreeD<3, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    for (auto it = ptr->begin(); it != ptr->end(); ++it) {
        lista = scm_cons(scm_from_int(it.second()), lista);
    }
    return scm_reverse(lista);
}
static SCM mtfa_phtree_2d_getall_boxes(SCM scm_ptr) {
    auto ptr = (PhTreeBoxD<2, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    for (auto it = ptr->begin(); it != ptr->end(); ++it) {
        lista = scm_cons(scm_from_int(it.second()), lista);
    }
    return scm_reverse(lista);
}
static SCM mtfa_phtree_3d_getall_boxes(SCM scm_ptr) {
    auto ptr = (PhTreeBoxD<3, int>*)scm_to_pointer(scm_ptr);
    SCM lista = SCM_EOL;
    for (auto it = ptr->begin(); it != ptr->end(); ++it) {
        lista = scm_cons(scm_from_int(it.second()), lista);
    }
    return scm_reverse(lista);
}
static SCM mtfa_phtree_2d_erase_points(SCM scm_ptr, SCM scm_2d_points_list) {
    auto ptr = (PhTreeD<2, int>*)scm_to_pointer(scm_ptr);
    while (!scm_is_null(scm_2d_points_list)) {
        PhPointD<2> pt;
        pt[0] = scm_to_double(scm_car(scm_car(scm_2d_points_list)));
        pt[1] = scm_to_double(scm_cadr(scm_car(scm_2d_points_list)));
        ptr->erase(pt);
        scm_2d_points_list = scm_cdr(scm_2d_points_list);
    }
    return SCM_BOOL_T;
}
static SCM mtfa_phtree_3d_erase_points(SCM scm_ptr, SCM scm_3d_points_list) {
    auto ptr = (PhTreeD<3, int>*)scm_to_pointer(scm_ptr);
    while (!scm_is_null(scm_3d_points_list))
    {
        if (scm_is_null(scm_3d_points_list))
            break;
        PhPointD<3> pt;
        pt[0] = scm_to_double(scm_car(scm_car(scm_3d_points_list)));
        pt[1] = scm_to_double(scm_cadr(scm_car(scm_3d_points_list)));
        pt[2] = scm_to_double(scm_caddr(scm_car(scm_3d_points_list)));
        ptr->erase(pt);
        scm_3d_points_list = scm_cdr(scm_3d_points_list);
    }
    return SCM_BOOL_T;
}
static SCM mtfa_phtree_2d_erase_boxes(SCM scm_ptr, SCM scm_2d_boxes_list) {
    auto ptr = (PhTreeBoxD<2, int>*)scm_to_pointer(scm_ptr);
    while (!scm_is_null(scm_2d_boxes_list)) {
        PhBoxD<2> pt;
        pt.min()[0] = scm_to_double(scm_car(scm_car(scm_car(scm_2d_boxes_list))));
        pt.min()[1] = scm_to_double(scm_cadr(scm_car(scm_car(scm_2d_boxes_list))));
        pt.max()[0] = scm_to_double(scm_car(scm_cadr(scm_car(scm_2d_boxes_list))));
        pt.max()[1] = scm_to_double(scm_cadr(scm_cadr(scm_car(scm_2d_boxes_list))));
        ptr->erase(pt);
        scm_2d_boxes_list = scm_cdr(scm_2d_boxes_list);
    }
    return SCM_BOOL_T;
}
static SCM mtfa_phtree_3d_erase_boxes(SCM scm_ptr, SCM scm_3d_boxes_list) {
    auto ptr = (PhTreeBoxD<3, int>*)scm_to_pointer(scm_ptr);
    while (!scm_is_null(scm_3d_boxes_list)) {
        PhBoxD<3> pt;
        pt.min()[0] = scm_to_double(scm_car(scm_car(scm_car(scm_3d_boxes_list))));
        pt.min()[1] = scm_to_double(scm_cadr(scm_car(scm_car(scm_3d_boxes_list))));
        pt.min()[2] = scm_to_double(scm_caddr(scm_car(scm_car(scm_3d_boxes_list))));
        pt.max()[0] = scm_to_double(scm_car(scm_cadr(scm_car(scm_3d_boxes_list))));
        pt.max()[1] = scm_to_double(scm_cadr(scm_cadr(scm_car(scm_3d_boxes_list))));
        pt.max()[2] = scm_to_double(scm_caddr(scm_cadr(scm_car(scm_3d_boxes_list))));
        ptr->erase(pt);
        scm_3d_boxes_list = scm_cdr(scm_3d_boxes_list);
    }
    return SCM_BOOL_T;
}


// int main() {
//     std::cout << "PH-Tree example with 3D `double` coordinates." << std::endl;
//     PhPointD<3> p1({1, 1, 1});
//     PhPointD<3> p2({2, 2, 2});
//     PhPointD<3> p3({3, 3, 3});
//     PhPointD<3> p4({4, 4, 4});
//
//     {
//         //per farla semplice, facciamo 1000000 box (in uno spazio 100x100x100)
//         PhTreeBoxD<3, int> boxtree;
//         int count=0;
// #define COUNT 1000
//         for (int x=0; x<COUNT; ++x) {
//             for (int y=0; y<COUNT; ++y) {
//                 for (int z=0; z<COUNT; ++z) {
//                     PhPointD<3> tl, br;
//                     tl[0]=x*1.0;
//                     tl[1]=y*1.0;
//                     tl[2]=z*1.0;
//                     br[0]=x+1.0;
//                     br[1]=y+1.0;
//                     br[2]=z+1.0;
//                     PhBoxD<3> box(tl, br);
//                     auto bret = boxtree.emplace(box, count++);
//                     if (!bret.second)
//                         cout << "Elemento " << count << " non inserito" << endl;
//                 }
//             }
//         }
//
//         //vediamo quanti elementi sono in un certo spazio
//         auto query_type = QueryIntersect();
//         auto found = boxtree.begin_query({{1,1,1},{80,80,80}}, FilterNoOp(), query_type);
//         while(!found.Finished())
//         {
//             cout << found.second() << endl;
//             found++;
//         }
//
//         getchar();
//         exit(0);
//     }
//




//Per consentire a eigen di compilarsi
#ifdef Success
  #undef Success
#endif
#undef eigen_assert
#define eigen_assert(x) \
  if (!(x)) { throw (std::runtime_error("Eigen matrix exception")); }




//Estrazione delle features...
static void releasePoints ( void * points )
{
    if (points) {
        delete ( std::vector<cv::KeyPoint>* ) points;
    }
}

SCM mtfa_opencv_features_detector(SCM imgmat);
SCM mtfa_opencv_features_detector(SCM imgmat) {
    try {
        cv::Mat *img = ( cv::Mat* ) scm_to_pointer ( imgmat );
        auto fastDetector = cv::FastFeatureDetector::create();
        auto *points = new std::vector<cv::KeyPoint>();
        fastDetector->detect(*img, *points);
        auto descriptor = cv::xfeatures2d::BEBLID::create(1 /*0.75*/, cv::xfeatures2d::BEBLID::BeblidSize::SIZE_512_BITS);
        auto descriptors = new cv::Mat();
        descriptor->compute(*img, *points, *descriptors);
        return scm_values_2(scm_from_pointer ( descriptors, releaseMat ), scm_from_pointer(points, releasePoints));
    } catch ( exception ex ) {
        cerr << "mtfa_opencv_features_detector: " << ex.what() << endl;
        return scm_values_2(SCM_BOOL_F, SCM_BOOL_F);
    }
}

//NORM_INF 1, NORM_L1 2, NORM_L2 4, NORM_L2SQR 5, NORM_HAMMING 6, NORM_HAMMING2 7, NORM_RELATIVE 8, NORM_MINMAX 32
/*Methods:
       DEFAULT 0,
       LMEDS  = 4,  //!< least-median of squares algorithm
       RANSAC = 8,  //!< RANSAC algorithm
       RHO    = 16, //!< RHO algorithm
       USAC_DEFAULT  = 32, //!< USAC algorithm, default settings
       USAC_PARALLEL = 33, //!< USAC, parallel version
       USAC_FM_8PTS = 34,  //!< USAC, fundamental matrix 8 points
       USAC_FAST = 35,     //!< USAC, fast settings
       USAC_ACCURATE = 36, //!< USAC, accurate settings
       USAC_PROSAC = 37,   //!< USAC, sorted points, runs PROSAC
       USAC_MAGSAC = 38    //!< USAC, runs MAGSAC++
*/
SCM mtfa_opencv_features_find_transformation(SCM scm_descr1, SCM scm_points1, SCM scm_descr2, SCM scm_points2, SCM scm_norm, SCM scm_method);
SCM mtfa_opencv_features_find_transformation(SCM scm_descr1, SCM scm_points1, SCM scm_descr2, SCM scm_points2, SCM scm_norm, SCM scm_method) {
    try {
        cv::Mat *descr1= ( cv::Mat* ) scm_to_pointer ( scm_descr1 );
        cv::Mat *descr2= ( cv::Mat* ) scm_to_pointer ( scm_descr2 );
        
        std::vector<cv::KeyPoint> * points1 = (std::vector<cv::KeyPoint> *)scm_to_pointer ( scm_points1);
        std::vector<cv::KeyPoint> * points2 = (std::vector<cv::KeyPoint> *)scm_to_pointer ( scm_points2 );
        
        int norm = scm_to_int(scm_norm);
        int method = scm_to_int(scm_method);
        
        // Match the generated descriptors for img1 and img2 using brute force matching
        cv::BFMatcher matcher(norm, true);
        std::vector<cv::DMatch> matches;
        matcher.match(*descr1, *descr2, matches);
        
        if (matches.size() == 0)
            return scm_values_2(scm_from_int(0), scm_from_int(0));

        std::vector<cv::Point2d> matched_pts1, matched_pts2;
        for (cv::DMatch match : matches)
        {
            matched_pts1.push_back((*points1)[match.queryIdx].pt);
            matched_pts2.push_back((*points2)[match.trainIdx].pt);
        }
        // Find the homography that transforms a point in the first image to a point in the second image.
        cv::Mat inliers;
        cv::Mat H = cv::findHomography(matched_pts1, matched_pts2, method, 3, inliers);
        return scm_values_2(scm_from_int(matches.size()), scm_from_int(cv::sum(inliers)[0]));
    } catch ( exception ex ) {
        cerr << "mtfa_opencv_features_find_transformation: " << ex.what() << endl;
        return scm_values_2(scm_from_int(0), scm_from_int(0));
    }
}

//E ora le funzioni di calcolo CUDA
static void releaseGpuMat ( void * mat )
{
    if (mat) {
        delete ( cv::cuda::GpuMat* ) mat;
    }
}

//#define MYTYPE CV_64F
#define MYTYPE CV_32F

static SCM mtfa_opencv_make_gpu_mat ( SCM scm_rows, SCM scm_cols, SCM scm_fill)
{
    if (scm_is_null(scm_rows) == 1 || scm_is_null(scm_cols) == 1) {
        auto m = new cv::cuda::GpuMat();
        return scm_from_pointer ( m, releaseGpuMat );
    } else {
        int rows = scm_to_int(scm_rows);
        int cols = scm_to_int(scm_cols);
        if (scm_is_null(scm_fill) == 1) {
            auto m = new cv::cuda::GpuMat(rows, cols, MYTYPE);
            return scm_from_pointer ( m, releaseGpuMat );
        } else {
            auto m = new cv::cuda::GpuMat(rows, cols, MYTYPE, cv::Scalar(scm_to_double(scm_fill)));
            return scm_from_pointer ( m, releaseGpuMat );
        }
    }
}

static SCM mtfa_opencv_convert_mat_to_gpumat(SCM scmMat, SCM scmDst) {
    if (scmDst == SCM_UNDEFINED) {
        cv::Mat *m= ( cv::Mat* ) scm_to_pointer ( scmMat );
        auto gm = new cv::cuda::GpuMat(*m);
        return scm_from_pointer ( gm, releaseGpuMat );
    } else {
        cv::Mat *m= ( cv::Mat* ) scm_to_pointer ( scmMat );
        cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scmDst );
        dst->upload(*m);
        return scmDst;
    }
}

static SCM mtfa_opencv_convert_gpumat_to_mat(SCM scmGpuMat, SCM scm_dst) {
    if (scm_dst == SCM_UNDEFINED) {
        cv::cuda::GpuMat *m= ( cv::cuda::GpuMat* ) scm_to_pointer ( scmGpuMat );
        auto cvm = new cv::Mat(*m);
        return scm_from_pointer ( cvm, releaseMat );
    } else {
        cv::cuda::GpuMat *m= ( cv::cuda::GpuMat* ) scm_to_pointer ( scmGpuMat );
        cv::Mat *dst= ( cv::Mat* ) scm_to_pointer ( scm_dst );
        m->download(*dst);
        return scm_dst;
    }
}

//listoflistofdouble è in effetti una lista di float
static SCM mtfa_opencv_make_gpumat_from_lolod(SCM listoflistofdouble, SCM scm_dst) {  //rows and columns
    int rows = scm_to_int(scm_length(listoflistofdouble));
    int cols = scm_to_int(scm_length(scm_car(listoflistofdouble)));
    float * v= new float[rows*cols];
    int pos=0;
    while (listoflistofdouble != SCM_EOL) {
        auto p = scm_car(listoflistofdouble);
        while (p != SCM_EOL) {
            v[pos++] = (float)scm_to_double(scm_car(p));
            p = scm_cdr(p);
        }
        listoflistofdouble = scm_cdr(listoflistofdouble);
    }
    auto mat = cv::Mat(rows, cols, MYTYPE, v);
    if (scm_dst == SCM_UNDEFINED) {
        cv::cuda::GpuMat *m= new cv::cuda::GpuMat(mat);
        delete [] v;
        return scm_from_pointer( m, releaseGpuMat );
    } else {
        cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
        dst->upload(mat);
        delete [] v;
        return scm_dst;
    }
}

static SCM mtfa_opencv_make_mat_from_lolod(SCM listoflistofdouble) {  //rows and columns
    int rows = scm_to_int(scm_length(listoflistofdouble));
    int cols = scm_to_int(scm_length(scm_car(listoflistofdouble)));
    float * v= new float[rows*cols];
    int pos=0;
    while (listoflistofdouble != SCM_EOL) {
        auto p = scm_car(listoflistofdouble);
        while (p != SCM_EOL) {
            v[pos++] = (float)scm_to_double(scm_car(p));
            p = scm_cdr(p);
        }
        listoflistofdouble = scm_cdr(listoflistofdouble);
    }

    cv::Mat mat_tmp(rows, cols, MYTYPE, v);
    auto mat_final = new cv::Mat();
    mat_tmp.copyTo(*mat_final);

    delete [] v;
    return scm_from_pointer ( mat_final, releaseMat );
}

// string type2str(int type) {
//   string r;
// 
//   uchar depth = type & CV_MAT_DEPTH_MASK;
//   uchar chans = 1 + (type >> CV_CN_SHIFT);
// 
//   switch ( depth ) {
//     case CV_8U:  r = "8U"; break;
//     case CV_8S:  r = "8S"; break;
//     case CV_16U: r = "16U"; break;
//     case CV_16S: r = "16S"; break;
//     case CV_32S: r = "32S"; break;
//     case CV_32F: r = "32F"; break;
//     case CV_64F: r = "64F"; break;
//     default:     r = "User"; break;
//   }
// 
//   r += "C";
//   r += (chans+'0');
// 
//   return r;
// }


static SCM mtfa_opencv_convert_gpumat_to_lolod (SCM scmGpuMat) {
    cv::cuda::GpuMat *m= ( cv::cuda::GpuMat* ) scm_to_pointer ( scmGpuMat );
    auto cvm = cv::Mat(*m);

    auto tipo = cvm.type();
    uchar depth = tipo & CV_MAT_DEPTH_MASK;

    SCM lolod = SCM_EOL;
    for (int r=0; r < cvm.rows; ++r) {
        SCM l = SCM_EOL;
        for (int c=0; c < cvm.cols; ++c) {
            switch ( depth ) {
                case CV_8U:
                    l = scm_cons(scm_from_uint8(cvm.at<uint8_t>(r, c)), l);
                    break;
                case CV_8S:
                    l = scm_cons(scm_from_int8(cvm.at<int8_t>(r, c)), l);
                    break;
                case CV_16U:
                    l = scm_cons(scm_from_uint16(cvm.at<uint16_t>(r, c)), l);
                    break;
                case CV_16S:
                    l = scm_cons(scm_from_int16(cvm.at<int16_t>(r, c)), l);
                    break;
                case CV_32S:
                    l = scm_cons(scm_from_int(cvm.at<int32_t>(r, c)), l);
                    break;
                case CV_32F:
                    l = scm_cons(scm_from_double((double)cvm.at<float>(r, c)), l);
                    break;
                case CV_64F:
                    l = scm_cons(scm_from_double(cvm.at<double>(r, c)), l);
                    break;
                default:    
                    l = scm_cons(scm_from_double(cvm.at<double>(r, c)), l);
                    break;
            }
        }
        lolod = scm_cons(scm_reverse(l), lolod);
    }
    return scm_reverse(lolod);
}
// enum GemmFlags { GEMM_1_T = 1, //!< transposes src1
//                  GEMM_2_T = 2, //!< transposes src2
//                  GEMM_3_T = 4 //!< transposes src3
//                };
// static SCM mtfa_opencv_gpumat_gemm(SCM scm_dst, SCM scm_src1, SCM scm_src1_t, SCM scm_src2, SCM scm_src2_t, SCM scm_alpha, SCM scm_src3, SCM scm_src3_t, SCM scm_beta) {
//     //src1*src2*alpha+src3*beta, con src1, src2 e src3 che possono essere trasposte
//     cv::cuda::GpuMat *dst, *src1, *src2, *src3;
//     double alpha, beta;
//     int flags;
    
//     if (scm_is_null(scm_src1) == 1) {
//         scm_wrong_type_arg_msg ( "mtfa_opencv_gpumat_gemm", 0, scm_src1, "src1 cannot be nil" );
//     }
//     if (scm_is_null(scm_src2) == 1) {
//         scm_wrong_type_arg_msg ( "mtfa_opencv_gpumat_gemm", 0, scm_src2, "src2 cannot be nil" );
//     }

//     dst  = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
//     src1 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_src1 );
//     src2 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_src2 );
//     if (src1->type() != MYTYPE || src2->type() != MYTYPE) {
//         scm_error(scm_from_latin1_symbol("opencv-error"),
//                   "mtfa_opencv_gpumat_gemm",
//                   "Matrix type mismatch: expected 32-bit float (CV_32F)",
//                   SCM_EOL, SCM_BOOL_F);
//     }
//     if (scm_is_null(scm_src3) == 1) src3 = nullptr; else src3 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_src3 );

//     alpha = scm_to_double(scm_alpha);
//     beta =  scm_to_double(scm_beta);

//     int transpose_src1 = scm_to_bool(scm_src1_t)?GEMM_1_T:0;
//     int transpose_src2 = scm_to_bool(scm_src2_t)?GEMM_2_T:0;
//     int transpose_src3 = scm_to_bool(scm_src3_t)?GEMM_3_T:0;

//     flags = transpose_src1 | transpose_src2 | transpose_src3;

//     cv::cuda::gemm(*src1, (src2 == nullptr)?cv::cuda::GpuMat():*src2, alpha, (src3 == nullptr)?cv::cuda::GpuMat():*src3, beta, *dst, flags);
//     return scm_dst;
// }


static SCM mtfa_opencv_gpumat_gemm(SCM scm_dst, SCM scm_src1, SCM scm_src1_t, SCM scm_src2, SCM scm_src2_t, SCM scm_alpha, SCM scm_src3, SCM scm_src3_t, SCM scm_beta) {
    // 1. Conversione Puntatori e Parametri
    cv::cuda::GpuMat *dst = (cv::cuda::GpuMat*) scm_to_pointer(scm_dst);
    cv::cuda::GpuMat *src1 = (cv::cuda::GpuMat*) scm_to_pointer(scm_src1);
    cv::cuda::GpuMat *src2 = (cv::cuda::GpuMat*) scm_to_pointer(scm_src2);
    
    double alpha = scm_to_double(scm_alpha);
    double beta = scm_to_double(scm_beta);

    // 2. Gestione Flags Transposta
    int flags = 0;
    if (scm_is_true(scm_src1_t)) flags |= cv::GEMM_1_T;
    if (scm_is_true(scm_src2_t)) flags |= cv::GEMM_2_T;
    // src3_t lo gestiamo manualmente se necessario, ma per il bias vettore non serve solitamente

    try {
        // --- STEP 1: Calcolo Principale (W * X) ---
        // Calcoliamo dst = alpha * src1 * src2.
        // Passiamo un array vuoto come src3 (bias) per ora e beta=0.
        // Questo evita il crash sulle dimensioni.
        cv::cuda::gemm(*src1, *src2, alpha, cv::cuda::GpuMat(), 0.0, *dst, flags);

        // --- STEP 2: Aggiunta del Bias (Se presente) ---
        if (!scm_is_false(scm_src3) && !scm_is_null(scm_src3)) {
            cv::cuda::GpuMat *src3 = (cv::cuda::GpuMat*) scm_to_pointer(scm_src3);
            
            if (src3 && !src3->empty()) {
                
                // CASO A: Dimensioni identiche (es. batch size 1 o bias è già una matrice completa)
                if (src3->size() == dst->size()) {
                    // dst = 1.0 * dst + beta * src3
                    cv::cuda::addWeighted(*dst, 1.0, *src3, beta, 0.0, *dst);
                }
                
                // CASO B: Broadcasting (Bias è vettore colonna, Dst è matrice batch)
                // src3: [Rows x 1]
                // dst:  [Rows x Cols]
                else if (src3->rows == dst->rows && src3->cols == 1) {
                    // TRUCCO: Creiamo un vettore riga di '1' lungo quanto il batch size (cols)
                    // Moltiplicando il Bias(Nx1) per Ones(1xM) otteniamo una matrice (NxM) con il bias copiato.
                    // Usiamo gemm per sommare direttamente al dst esistente.
                    
                    cv::cuda::GpuMat ones(1, dst->cols, dst->type());
                    ones.setTo(cv::Scalar(1.0));

                    // Formula GEMM: dst = alpha*src1*src2 + beta*src3
                    // Qui usiamo:   dst = beta * (src3 * ones) + 1.0 * dst
                    cv::cuda::gemm(*src3, ones, beta, *dst, 1.0, *dst, 0); 
                }
                else {
                    // Caso non gestito (dimensioni incompatibili)
                    fprintf(stderr, "MTFA_OPENCV_GEMM WARN: Bias dim mismatch (Bias: %dx%d vs Dst: %dx%d). Bias ignorato.\n", 
                            src3->rows, src3->cols, dst->rows, dst->cols);
                }
            }
        }
    } catch (const cv::Exception& e) {
        fprintf(stderr, "MTFA_OPENCV_GEMM ERROR: %s\n", e.what());
        // Rilanciamo l'errore a Guile o ritorniamo False
        return SCM_BOOL_F; 
    }

    return scm_dst;
}



static SCM mtfa_opencv_transpose_gpumat (SCM scm_dst, SCM scm_src) {
    //CV_EXPORTS_W void transpose(InputArray src1, OutputArray dst, Stream& stream = Stream::Null());
    cv::cuda::GpuMat *src = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_src );
    cv::cuda::GpuMat *dst = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    cv::cuda::transpose(*src, *dst);
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_rows_cols (SCM scm_src) {
    cv::cuda::GpuMat *src = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_src );
    return scm_values_2(scm_from_int(src->rows), scm_from_int(src->cols));
}

static SCM mtfa_opencv_gpumat_clone (SCM scm_src, SCM scm_dst) {
    cv::cuda::GpuMat *src = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_src );
    if (scm_dst == SCM_UNDEFINED) {
        cv::cuda::GpuMat *dst=new cv::cuda::GpuMat();
        *dst = src->clone();
        return scm_from_pointer ( dst, releaseGpuMat );
    } else {
        cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
        src->copyTo(*dst);
        return scm_dst;
    }
}

// RIMOZIONE BIAS - Operazione nativa GPU e sicura in-place
static SCM mtfa_opencv_gpumat_del_last_row (SCM scm_dst, SCM scm_src) {
    cv::cuda::GpuMat *src = (cv::cuda::GpuMat*) scm_to_pointer(scm_src);
    // Se dst non è fornito o è uguale a src, operiamo in-place
    cv::cuda::GpuMat *dst = (scm_is_null(scm_dst) || scm_is_false(scm_dst)) ? src : (cv::cuda::GpuMat*) scm_to_pointer(scm_dst);

    if (src->empty() || src->rows <= 1) return scm_dst;

    try {
        // Creiamo una vista (header) che esclude l'ultima riga
        cv::cuda::GpuMat sub = src->rowRange(0, src->rows - 1);
        
        if (dst == src) {
            // Per l'operazione in-place dobbiamo clonare la vista 
            // prima di riassegnarla alla GpuMat originale
            cv::cuda::GpuMat tmp = sub.clone();
            *dst = tmp;
        } else {
            sub.copyTo(*dst);
        }
    } catch (const cv::Exception& e) {
        fprintf(stderr, "ERRORE del_last_row: %s\n", e.what());
    }
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_math_compare(SCM scm_dst, SCM scm_op1, SCM scm_op2, SCM scm_cmd) {
    if (scm_is_false(scm_dst) || !scm_to_pointer(scm_dst)) return scm_dst;
    
    cv::cuda::GpuMat *dst = (cv::cuda::GpuMat*)scm_to_pointer(scm_dst);
    cv::cuda::GpuMat *m1 = (cv::cuda::GpuMat*)scm_to_pointer(scm_op1);
    int cmd = scm_to_int(scm_cmd);

    // Se m1 è nullo o vuoto (magari a causa di un GEMM fallito prima), usciamo subito
    if (!m1 || m1->empty()) return scm_dst;

    try {
        cv::cuda::GpuMat mask;
        if (scm_is_number(scm_op2)) {
            cv::cuda::compare(*m1, cv::Scalar(scm_to_double(scm_op2)), mask, (cv::CmpTypes)cmd);
        } else {
            cv::cuda::GpuMat *m2 = (cv::cuda::GpuMat*)scm_to_pointer(scm_op2);
            if (!m2 || m2->empty() || m1->size() != m2->size()) return scm_dst;
            cv::cuda::compare(*m1, *m2, mask, (cv::CmpTypes)cmd);
        }
        
        if (!mask.empty()) {
            dst->create(mask.size(), MYTYPE); // MYTYPE è CV_32F
            // Converte i 255 (True) di OpenCV in 1.0 (float) per la derivata ReLU
            mask.convertTo(*dst, MYTYPE, 1.0/255.0);
        }
    } catch (const cv::Exception& e) {
        fprintf(stderr, "ERRORE math_compare: %s\n", e.what());
    }
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_math_and (SCM scm_dst, SCM scm_op1, SCM scm_op2) {
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    if (scm_is_number(scm_op1)==1) {
        if (scm_is_number(scm_op2)==1) {
            cv::cuda::bitwise_and(scm_to_double(scm_op1), scm_to_double(scm_op2), *dst);
        } else {
            cv::cuda::bitwise_and(scm_to_double(scm_op1), *( cv::cuda::GpuMat* ) scm_to_pointer(scm_op2), *dst);
        }
    } else {
        if (scm_is_number(scm_op2)==1) {
            cv::cuda::bitwise_and(*( cv::cuda::GpuMat* ) scm_to_pointer(scm_op1), scm_to_double(scm_op2), *dst);
        } else {
            cv::cuda::bitwise_and(*( cv::cuda::GpuMat* ) scm_to_pointer(scm_op1), *( cv::cuda::GpuMat* ) scm_to_pointer(scm_op2), *dst);
        }
    }
    return scm_dst;
}
static SCM mtfa_opencv_gpumat_math_or (SCM scm_dst, SCM scm_op1, SCM scm_op2) {
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    if (scm_is_number(scm_op1)==1) {
        if (scm_is_number(scm_op2)==1) {
            cv::cuda::bitwise_or(scm_to_double(scm_op1), scm_to_double(scm_op2), *dst);
        } else {
            cv::cuda::bitwise_or(scm_to_double(scm_op1), *( cv::cuda::GpuMat* ) scm_to_pointer(scm_op2), *dst);
        }
    } else {
        if (scm_is_number(scm_op2)==1) {
            cv::cuda::bitwise_or(*( cv::cuda::GpuMat* ) scm_to_pointer(scm_op1), scm_to_double(scm_op2), *dst);
        } else {
            cv::cuda::bitwise_or(*( cv::cuda::GpuMat* ) scm_to_pointer(scm_op1), *( cv::cuda::GpuMat* ) scm_to_pointer(scm_op2), *dst);
        }
    }
    return scm_dst;
}
static SCM mtfa_opencv_gpumat_math_xor (SCM scm_dst, SCM scm_op1, SCM scm_op2) {
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    if (scm_is_number(scm_op1)==1) {
        if (scm_is_number(scm_op2)==1) {
            cv::cuda::bitwise_xor(scm_to_double(scm_op1), scm_to_double(scm_op2), *dst);
        } else {
            cv::cuda::bitwise_xor(scm_to_double(scm_op1), *( cv::cuda::GpuMat* ) scm_to_pointer(scm_op2), *dst);
        }
    } else {
        if (scm_is_number(scm_op2)==1) {
            cv::cuda::bitwise_xor(*( cv::cuda::GpuMat* ) scm_to_pointer(scm_op1), scm_to_double(scm_op2), *dst);
        } else {
            cv::cuda::bitwise_xor(*( cv::cuda::GpuMat* ) scm_to_pointer(scm_op1), *( cv::cuda::GpuMat* ) scm_to_pointer(scm_op2), *dst);
        }
    }
    return scm_dst;
}


static SCM mtfa_opencv_gpumat_math_not (SCM scm_dst, SCM scm_op1) {
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    cv::cuda::bitwise_not(*( cv::cuda::GpuMat* ) scm_to_pointer(scm_op1), *dst);
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_math_absdiff (SCM scm_dst, SCM scm_op1, SCM scm_op2) {
    cv::cuda::GpuMat *op1 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op1 );
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    cv::cuda::GpuMat *op2 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op2 );
    cv::cuda::absdiff(*op1, *op2, *dst);
    return scm_dst;
}
static SCM mtfa_opencv_gpumat_math_abs(SCM scm_dst, SCM scm_op1) {
    cv::cuda::GpuMat *op1 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op1 );
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    cv::cuda::abs(*op1, *dst);
    return scm_dst;
}
static SCM mtfa_opencv_gpumat_math_sqr(SCM scm_dst, SCM scm_op1) {
    cv::cuda::GpuMat *op1 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op1 );
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    cv::cuda::sqr(*op1, *dst);
    return scm_dst;
}
static SCM mtfa_opencv_gpumat_math_sqrt(SCM scm_dst, SCM scm_op1) {
    cv::cuda::GpuMat *op1 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op1 );
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    cv::cuda::sqrt(*op1, *dst);
    return scm_dst;
}
static SCM mtfa_opencv_gpumat_math_exp(SCM scm_dst, SCM scm_op1) {
    cv::cuda::GpuMat *op1 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op1 );
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );

    // Controllo critico: evita che Guile passi dati a 64bit in un kernel a 32bit
    if (op1->type() != MYTYPE) {
        scm_error(scm_from_latin1_symbol("opencv-type-error"), "gpumat_math_exp", "Expected CV_32F", SCM_EOL, SCM_BOOL_F);
    }

    cv::cuda::exp(*op1, *dst);
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_math_log(SCM scm_dst, SCM scm_op1) {
    cv::cuda::GpuMat *op1 = (cv::cuda::GpuMat*)scm_to_pointer(scm_op1);
    cv::cuda::GpuMat *dst = (cv::cuda::GpuMat*)scm_to_pointer(scm_dst);

    // Controllo tipo (per coerenza con MYTYPE)
    if (op1->type() != MYTYPE) {
        scm_error(scm_from_latin1_symbol("opencv-error"), "gpumat_math_log", "Expected CV_32F", SCM_EOL, SCM_BOOL_F);
    }

    // Strategia Anti-Crash: log(x + epsilon)
    // Usiamo un epsilon di 1e-7f per evitare log(0)
    // cv::cuda::add può sommare uno scalare direttamente a una matrice sulla GPU
    cv::cuda::add(*op1, cv::Scalar(1e-7), *dst); 
    
    // Ora calcoliamo il logaritmo sul valore traslato
    cv::cuda::log(*dst, *dst);

    return scm_dst;
}

static SCM mtfa_opencv_gpumat_math_pow(SCM scm_dst, SCM scm_op1, SCM scm_op2) {
    cv::cuda::GpuMat *op1 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op1 );
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    double scale = scm_to_double ( scm_op2 );
    cv::cuda::pow(*op1, scale, *dst);
    return scm_dst;
}
static SCM mtfa_opencv_gpumat_math_min (SCM scm_dst, SCM scm_op1, SCM scm_op2) {
    cv::cuda::GpuMat *op1 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op1 );
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    if (scm_is_number(scm_op2)==1) {
        cv::cuda::min(*op1, scm_to_double(scm_op2), *dst);
    } else {
        cv::cuda::GpuMat *op2 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op2 );
        cv::cuda::min(*op1, *op2, *dst);
    }
    return scm_dst;
}
static SCM mtfa_opencv_gpumat_math_max (SCM scm_dst, SCM scm_op1, SCM scm_op2) {
    cv::cuda::GpuMat *op1 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op1 );
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    if (scm_is_number(scm_op2)==1) {
        cv::cuda::max(*op1, scm_to_double(scm_op2), *dst);
    } else {
        cv::cuda::GpuMat *op2 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op2 );
        cv::cuda::max(*op1, *op2, *dst);
    }
    return scm_dst;
}
static SCM mtfa_opencv_gpumat_math_add_weighted (SCM scm_dst, SCM scm_op1, SCM scm_alpha, SCM scm_op2, SCM scm_beta, SCM scm_gamma) {
    cv::cuda::GpuMat *op1 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op1 );
    cv::cuda::GpuMat *op2 = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op2 );
    cv::cuda::GpuMat *dst= ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    double alpha = scm_to_double(scm_alpha);
    double beta = scm_to_double(scm_beta);
    double gamma = scm_to_double(scm_gamma);
    cv::cuda::addWeighted(*op1, alpha, *op2, beta, gamma, *dst);
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_math_find_min_max(SCM scm_op) {
    cv::cuda::GpuMat *op = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_op );
    double vmin, vmax;
    cv::cuda::minMax(*op, &vmin, &vmax);
    return scm_values_2(scm_from_double(vmin), scm_from_double(vmax));
}

static SCM mtfa_opencv_gpumat_math_sum(SCM scm_src) {
    if (scm_is_false(scm_src) || !scm_to_pointer(scm_src)) return scm_from_double(0.0);
    
    cv::cuda::GpuMat *src = (cv::cuda::GpuMat*)scm_to_pointer(scm_src);
    if (src->empty()) return scm_from_double(0.0);
    
    try {
        // Obblighiamo OpenCV a ignorare la maschera usando cv::noArray()
        cv::Scalar s = cv::cuda::sum(*src, cv::noArray());
        return scm_from_double(s[0]);
    } catch (const cv::Exception& e) {
        fprintf(stderr, "ERRORE math_sum: %s\n", e.what());
        return scm_from_double(0.0);
    }
}

static SCM mtfa_opencv_gpumat_transpose(SCM scm_src, SCM scm_dst) {
    // 1. Otteniamo i puntatori
    cv::cuda::GpuMat *src = (cv::cuda::GpuMat*) scm_to_pointer(scm_src);
    cv::cuda::GpuMat *dst = (cv::cuda::GpuMat*) scm_to_pointer(scm_dst);

    // 2. Controllo sicurezza
    if (!src || src->empty()) return SCM_BOOL_F;

    // 3. ALLOCAZIONE AUTOMATICA (Cruciale per evitare NPP_NULL_POINTER_ERROR)
    // Se dst non è della dimensione corretta (src->cols x src->rows), lo creiamo.
    if (dst->rows != src->cols || dst->cols != src->rows || dst->type() != src->type()) {
        // La trasposta inverte Righe e Colonne
        *dst = cv::cuda::GpuMat(src->cols, src->rows, src->type());
    }

    // 4. Esecuzione sulla GPU
    cv::cuda::transpose(*src, *dst);
    
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_make_diag(SCM scm_op1, SCM scm_op2, SCM scm_dst) {
    auto side = scm_to_double(scm_op1);
    auto value = scm_to_double(scm_op2);
    cv::Mat A = cv::Mat::eye(side, side, MYTYPE)*value;
    if (scm_dst == SCM_UNDEFINED) {
        return scm_from_pointer( new cv::cuda::GpuMat(A), releaseGpuMat );
    } else {
        cv::cuda::GpuMat *dst = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
        dst->upload(A);
        return scm_dst;
    }
}

static SCM mtfa_opencv_gpumat_convert_vector_to_diag(SCM scm_dst, SCM scm_src) {
    cv::cuda::GpuMat *dst = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    cv::cuda::GpuMat *src = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_src );
    
    //faccio una matrice normale che ha righe e colonne come la matrice unidimensionale (src)
    auto cvm = cv::Mat(*src);
    
    //faccio una matrice normale come la destinazione
    auto dst_mat = cv::Mat(dst->rows, dst->cols, MYTYPE, Scalar(0.0));
    
    //copio le righe nella destinazione
    for (int r=0; r<dst_mat.rows; ++r) {
            dst_mat.at<float>(r,r) = cvm.at<float>(r, 0);
    }
    
    dst->upload(dst_mat);
    
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_set(SCM scm_src, SCM scm_row, SCM scm_col, SCM scm_value) {
    cv::cuda::GpuMat *src = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_src );
    auto cvm = cv::Mat(*src);
    cvm.at<float>(scm_to_int(scm_row), scm_to_int(scm_col)) = (float)scm_to_double(scm_value);
    src->upload(cvm);
    return scm_src;
}
static SCM mtfa_opencv_mat_get(SCM scm_src, SCM scm_row, SCM scm_col) {
    cv::Mat *src = (cv::Mat*) scm_to_pointer(scm_src);
    int r = scm_to_int(scm_row);
    int c = scm_to_int(scm_col);

    // Controllo di sicurezza per evitare crash o freeze
    if (r >= src->rows || c >= src->cols || r < 0 || c < 0) {
        return scm_from_double(0.0);
    }

    // Accesso diretto al buffer float (molto più veloce e stabile per i binding)
    float* data = (float*)src->data;
    float val = data[r * src->cols + c];
    return scm_from_double((double)val);
}

static SCM mtfa_opencv_gpumat_add_row (SCM scm_dst, SCM scm_src, SCM scm_row)
{
    auto dst = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
    auto src = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_src );
    auto row = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_row );
    cv::Mat m_src(*src);
    cv::Mat m_row(*row);

    if (m_row.type() != m_src.type()) {
        m_row.convertTo(m_row, m_src.type()); // Conversione automatica di sicurezza
    }

    m_src.push_back(m_row);
    dst->upload(m_src);
    return scm_dst;
}

static SCM mtfa_opencv_mat_add_row (SCM scm_dst, SCM scm_row)
{
    auto dst = ( cv::Mat* ) scm_to_pointer ( scm_dst );
    auto row = ( cv::Mat* ) scm_to_pointer ( scm_row );
    
    dst->push_back(*row);
    return scm_dst;
}

// static SCM mtfa_opencv_gpumat_add_row (SCM scm_dst, SCM scm_src, SCM scm_row)
// {
//     auto dst = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_dst );
//     auto src = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_src );
//     auto row = ( cv::cuda::GpuMat* ) scm_to_pointer ( scm_row );
//     cv::Mat m_src(*src);
//     cv::Mat m_row(*row);
//     
//     m_src.push_back(m_row);
//     dst->upload(m_src);
//     return scm_dst;
// }


static SCM mtfa_opencv_gpumat_cuda_reset() {
    cudaDeviceReset();
    return SCM_EOL;
}

static SCM mtfa_opencv_gpumat_math_normalize(SCM scm_dst, SCM scm_src, SCM scm_alpha, SCM scm_beta, SCM scm_norm_type) {
    cv::cuda::GpuMat *dst = (cv::cuda::GpuMat*)scm_to_pointer(scm_dst);
    cv::cuda::GpuMat *src = (cv::cuda::GpuMat*)scm_to_pointer(scm_src);

    // Signature corretta per OpenCV 4 CUDA:
    // src, dst, alpha, beta, norm_type, dtype, mask, stream
    cv::cuda::normalize(*src, *dst, 
                        scm_to_double(scm_alpha), 
                        scm_to_double(scm_beta), 
                        scm_to_int(scm_norm_type),
                        MYTYPE,                   // dtype: forziamo CV_32F
                        cv::noArray(),            // mask: nessuna maschera
                        cv::cuda::Stream::Null()  // stream: default sincrono
    );
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_reuse_buffer(SCM scm_mat, SCM scm_rows, SCM scm_cols) {
    cv::cuda::GpuMat *m = (cv::cuda::GpuMat*)scm_to_pointer(scm_mat);
    int r = scm_to_int(scm_rows);
    int c = scm_to_int(scm_cols);

    if (m->rows != r || m->cols != c) {
        // Riassegna la memoria solo se le dimensioni cambiano
        m->create(r, c, MYTYPE);
    }
    // Non facciamo delete, teniamo il buffer allocato
    return scm_mat;
}


static SCM mtfa_opencv_gpumat_cuda_set_device(SCM scm_num_device) {
    if (cudaError::cudaSuccess == cudaSetDevice(scm_to_int(scm_num_device))) {
        return SCM_BOOL_T;
    } else {
        return SCM_BOOL_F;
    }
}

static SCM mtfa_opencv_gpumat_upload_raw(cv::cuda::GpuMat* mat, int rows, int cols, float* data) {
    // Creiamo una testata Mat (CPU) che punta alla memoria del bytevector di Guile
    // CV_32FC1 indica float a 32 bit (singola precisione), perfetto per la 1050 Ti
    cv::Mat cpu_wrapper(rows, cols, MYTYPE, data);

    // Upload massivo sulla GPU. OpenCV userà un trasferimento veloce PCIe.
    mat->upload(cpu_wrapper);
    return SCM_BOOL_T;
}

static SCM mtfa_opencv_gpumat_upload_batch(SCM scm_mat, SCM scm_data, SCM scm_rows, SCM scm_cols) {
    cv::cuda::GpuMat* mat = (cv::cuda::GpuMat*)scm_to_pointer(scm_mat);
    int rows = scm_to_int(scm_rows);
    int cols = scm_to_int(scm_cols);
    
    const float* data_ptr;
    scm_t_array_handle handle; // Handle necessario per Guile 3.0
    bool is_f32vec = scm_is_true(scm_f32vector_p(scm_data));

    if (is_f32vec) {
        size_t len;
        ssize_t inc;
        // Firma a 4 argomenti: (vector, handle_ptr, len_ptr, inc_ptr)
        data_ptr = scm_f32vector_elements(scm_data, &handle, &len, &inc);
    } else {
        data_ptr = (const float*)scm_to_pointer(scm_data);
    }

    try {
        // Creiamo il wrapper CPU (senza copia)
        cv::Mat batch_mat(rows, cols, MYTYPE, (void*)data_ptr);

        if (mat->rows != rows || mat->cols != cols) {
            *mat = cv::cuda::GpuMat(rows, cols, MYTYPE);
        }

        // Upload sulla GPU
        mat->upload(batch_mat);
        
    } catch (const cv::Exception& e) {
        if (is_f32vec) scm_array_handle_release(&handle);
        scm_error(scm_from_utf8_symbol("opencv-error"), "upload_batch", e.what(), SCM_EOL, SCM_EOL);
    }

    // Rilasciamo l'handle: ora il GC di Guile può di nuovo gestire liberamente la memoria
    if (is_f32vec) {
        scm_array_handle_release(&handle);
    }

    return SCM_BOOL_T;
}


static SCM mtfa_opencv_gpumat_reduce(SCM scm_src, SCM scm_dst, SCM scm_dim, SCM scm_op) {
    // 1. Estrazione dei puntatori dalle SMOB di Guile (o dal tuo sistema di wrapping)
    // Assumo che tu abbia una macro o funzione mtfa_scm_to_gpumat
    cv::cuda::GpuMat* src = (cv::cuda::GpuMat*)scm_to_pointer(scm_src);
    cv::cuda::GpuMat* dst = (cv::cuda::GpuMat*)scm_to_pointer(scm_dst);

    // 2. Conversione fondamentale da SCM a tipi C nativi
    int dim = scm_to_int(scm_dim);
    int reduceOp = scm_to_int(scm_op);

    // 3. Chiamata OpenCV CUDA
    cv::cuda::reduce(*src, *dst, dim, reduceOp, -1);

    return SCM_BOOL_T;
}

static SCM mtfa_opencv_gpumat_math_tanh(SCM scm_dst, SCM scm_src) {
    cv::cuda::GpuMat *dst = (cv::cuda::GpuMat*)scm_to_pointer(scm_dst);
    cv::cuda::GpuMat *src = (cv::cuda::GpuMat*)scm_to_pointer(scm_src);

    // Formula: tanh(x) = (exp(2x) - 1) / (exp(2x) + 1)
    cv::cuda::GpuMat exp_2x;
    
    // 1. 2 * x
    cv::cuda::multiply(*src, cv::Scalar(2.0), exp_2x);
    // 2. exp(2x)
    cv::cuda::exp(exp_2x, exp_2x);
    
    cv::cuda::GpuMat num, den;
    // 3. Numeratore: exp(2x) - 1
    cv::cuda::subtract(exp_2x, cv::Scalar(1.0), num);
    // 4. Denominatore: exp(2x) + 1
    cv::cuda::add(exp_2x, cv::Scalar(1.0), den);
    
    // 5. Risultato finale
    cv::cuda::divide(num, den, *dst);

    return scm_dst;
}

static SCM mtfa_opencv_gpumat_argmax_cols(SCM scm_src, SCM scm_dst_ptr, SCM scm_batch) {
    // printf("DEBUG: Entrato in argmax\n"); fflush(stdout);
    
    cv::cuda::GpuMat *src = (cv::cuda::GpuMat*)scm_to_pointer(scm_src);
    int *dst_indices = (int*)scm_to_pointer(scm_dst_ptr);
    int batch_size = scm_to_int(scm_batch);

    if (!src || src->empty()) {
        // printf("DEBUG: src è vuoto o NULL!\n"); fflush(stdout);
        return SCM_BOOL_F;
    }

    // printf("DEBUG: Download in corso (Size: %dx%d)...\n", src->rows, src->cols); fflush(stdout);
    
    // NOTA: Il download dalla GPU alla CPU è costoso. 
    // Farlo ad ogni batch rallenta il training, ma è necessario se calcoli l'accuratezza qui.
    cv::Mat cpu_out;
    src->download(cpu_out); 
    
    // printf("DEBUG: Download completato. Inizio ciclo...\n"); fflush(stdout);
    
    // Controllo di sicurezza: non leggere oltre la memoria disponibile
    int limit = std::min(batch_size, cpu_out.cols);

    for (int c = 0; c < limit; ++c) {
        float maxV = -std::numeric_limits<float>::infinity();
        int maxIdx = 0;
        
        // Loop sulle righe (le classi, es. 0-9)
        for (int r = 0; r < cpu_out.rows; ++r) {
            float val = cpu_out.at<float>(r, c);
            if (val > maxV) { 
                maxV = val; 
                maxIdx = r; 
            }
        }
        dst_indices[c] = maxIdx;
    }
    
    // printf("DEBUG: argmax completato con successo\n"); fflush(stdout);
    return SCM_BOOL_T;
}


static SCM mtfa_opencv_gpumat_math_add (SCM scm_dst, SCM scm_op1, SCM scm_op2) {
    cv::cuda::GpuMat *dst = (cv::cuda::GpuMat*) scm_to_pointer(scm_dst);
    if (scm_is_number(scm_op1)) {
        double v1 = scm_to_double(scm_op1);
        if (scm_is_number(scm_op2)) cv::cuda::add(cv::Scalar(v1), cv::Scalar(scm_to_double(scm_op2)), *dst);
        else cv::cuda::add(cv::Scalar(v1), *(cv::cuda::GpuMat*)scm_to_pointer(scm_op2), *dst);
    } else {
        cv::cuda::GpuMat *m1 = (cv::cuda::GpuMat*)scm_to_pointer(scm_op1);
        if (scm_is_number(scm_op2)) cv::cuda::add(*m1, cv::Scalar(scm_to_double(scm_op2)), *dst);
        else {
            cv::cuda::GpuMat *m2 = (cv::cuda::GpuMat*)scm_to_pointer(scm_op2);
            if (m1->size() != m2->size()) {
                cv::cuda::GpuMat exp;
                if (m1->rows == 1 && m1->cols == m2->cols) { // m1 è riga, m2 è matrice
                    dst->create(m2->size(), m2->type());
                    exp.create(m2->size(), m2->type());
                    cv::cuda::GpuMat ones(m2->rows, 1, m2->type(), cv::Scalar(1.0));
                    cv::cuda::gemm(ones, *m1, 1.0, cv::noArray(), 0.0, exp);
                    cv::cuda::add(exp, *m2, *dst);
                } else if (m2->rows == 1 && m2->cols == m1->cols) { // m2 è riga, m1 è matrice
                    dst->create(m1->size(), m1->type());
                    exp.create(m1->size(), m1->type());
                    cv::cuda::GpuMat ones(m1->rows, 1, m1->type(), cv::Scalar(1.0));
                    cv::cuda::gemm(ones, *m2, 1.0, cv::noArray(), 0.0, exp);
                    cv::cuda::add(*m1, exp, *dst);
                } else {
                    printf("ADD DIM MISMATCH: m1:%dx%d m2:%dx%d\n", m1->rows, m1->cols, m2->rows, m2->cols);
                }
            } else cv::cuda::add(*m1, *m2, *dst);
        }
    }
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_math_sub (SCM scm_dst, SCM scm_op1, SCM scm_op2) {
    // 1. Validazione puntatori (Indispensabile per prevenire Segfault)
    if (scm_is_false(scm_dst) || !scm_to_pointer(scm_dst)) return scm_dst;
    cv::cuda::GpuMat *dst = (cv::cuda::GpuMat*) scm_to_pointer(scm_dst);

    // 2. Caso: OP1 è un numero (es. 1.0 - GpuMat)
    if (scm_is_number(scm_op1)) {
        double v1 = scm_to_double(scm_op1);
        if (scm_is_number(scm_op2)) {
            // Entrambi numeri (raro, ma gestito)
            cv::cuda::subtract(cv::Scalar(v1), cv::Scalar(scm_to_double(scm_op2)), *dst);
        } else {
            cv::cuda::GpuMat *m2 = (cv::cuda::GpuMat*)scm_to_pointer(scm_op2);
            dst->create(m2->size(), m2->type());
            cv::cuda::subtract(cv::Scalar(v1), *m2, *dst);
        }
        return scm_dst;
    }

    // 3. Caso: OP1 è una GpuMat
    cv::cuda::GpuMat *m1 = (cv::cuda::GpuMat*) scm_to_pointer(scm_op1);

    // 4. Caso: OP1 (Mat) - OP2 (Numero)
    if (scm_is_number(scm_op2)) {
        double v2 = scm_to_double(scm_op2);
        dst->create(m1->size(), m1->type());
        cv::cuda::subtract(*m1, cv::Scalar(v2), *dst);
        return scm_dst;
    }

    // 5. Caso: OP1 (Mat) - OP2 (Mat) con Broadcasting
    cv::cuda::GpuMat *m2 = (cv::cuda::GpuMat*) scm_to_pointer(scm_op2);

    if (m1->size() != m2->size()) {
        // Broadcasting solo se le colonne coincidono e uno dei due è un vettore riga
        if (m1->cols == m2->cols && (m1->rows == 1 || m2->rows == 1)) {
            int final_rows = std::max(m1->rows, m2->rows);
            dst->create(final_rows, m1->cols, m1->type());
            
            cv::cuda::GpuMat exp(final_rows, m1->cols, m1->type());
            cv::cuda::GpuMat ones(final_rows, 1, m1->type(), cv::Scalar(1.0));

            if (m1->rows == 1) {
                cv::cuda::gemm(ones, *m1, 1.0, cv::noArray(), 0.0, exp);
                cv::cuda::subtract(exp, *m2, *dst);
            } else {
                cv::cuda::gemm(ones, *m2, 1.0, cv::noArray(), 0.0, exp);
                cv::cuda::subtract(*m1, exp, *dst);
            }
        } else {
            // ERRORE CRITICO: qui probabilmente hai il 129x64 vs 10x64
            fprintf(stderr, "!!! SUB DIM MISMATCH: m1(%dx%d) m2(%dx%d)\n", m1->rows, m1->cols, m2->rows, m2->cols);
            return scm_dst;
        }
    } else {
        // Dimensioni identiche
        dst->create(m1->size(), m1->type());
        cv::cuda::subtract(*m1, *m2, *dst);
    }

    return scm_dst;
}

static SCM mtfa_opencv_gpumat_math_mul (SCM scm_dst, SCM scm_op1, SCM scm_op2, SCM scm_scale) {
    cv::cuda::GpuMat *dst = (cv::cuda::GpuMat*) scm_to_pointer(scm_dst);
    double s = scm_to_double(scm_scale);
    if (scm_is_number(scm_op1)) {
        double v1 = scm_to_double(scm_op1);
        if (scm_is_number(scm_op2)) cv::cuda::multiply(cv::Scalar(v1), cv::Scalar(scm_to_double(scm_op2)), *dst, s);
        else cv::cuda::multiply(cv::Scalar(v1), *(cv::cuda::GpuMat*)scm_to_pointer(scm_op2), *dst, s);
    } else {
        cv::cuda::GpuMat *m1 = (cv::cuda::GpuMat*)scm_to_pointer(scm_op1);
        if (scm_is_number(scm_op2)) cv::cuda::multiply(*m1, cv::Scalar(scm_to_double(scm_op2)), *dst, s);
        else {
            cv::cuda::GpuMat *m2 = (cv::cuda::GpuMat*)scm_to_pointer(scm_op2);
            if (m1->size() != m2->size()) {
                cv::cuda::GpuMat exp;
                if (m1->rows == 1 && m1->cols == m2->cols) {
                    dst->create(m2->size(), m2->type());
                    exp.create(m2->size(), m2->type());
                    cv::cuda::GpuMat ones(m2->rows, 1, m2->type(), cv::Scalar(1.0));
                    cv::cuda::gemm(ones, *m1, 1.0, cv::noArray(), 0.0, exp);
                    cv::cuda::multiply(exp, *m2, *dst, s);
                } else if (m2->rows == 1 && m2->cols == m1->cols) {
                    dst->create(m1->size(), m1->type());
                    exp.create(m1->size(), m1->type());
                    cv::cuda::GpuMat ones(m1->rows, 1, m1->type(), cv::Scalar(1.0));
                    cv::cuda::gemm(ones, *m2, 1.0, cv::noArray(), 0.0, exp);
                    cv::cuda::multiply(*m1, exp, *dst, s);
                }
            } else cv::cuda::multiply(*m1, *m2, *dst, s);
        }
    }
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_math_div (SCM scm_dst, SCM scm_op1, SCM scm_op2, SCM scm_scale) {
    cv::cuda::GpuMat *dst = (cv::cuda::GpuMat*) scm_to_pointer(scm_dst);
    double s = scm_to_double(scm_scale);
    if (scm_is_number(scm_op1)) {
        double v1 = scm_to_double(scm_op1);
        if (scm_is_number(scm_op2)) cv::cuda::divide(cv::Scalar(v1), cv::Scalar(scm_to_double(scm_op2)), *dst, s);
        else cv::cuda::divide(cv::Scalar(v1), *(cv::cuda::GpuMat*)scm_to_pointer(scm_op2), *dst, s);
    } else {
        cv::cuda::GpuMat *m1 = (cv::cuda::GpuMat*)scm_to_pointer(scm_op1);
        if (scm_is_number(scm_op2)) cv::cuda::divide(*m1, cv::Scalar(scm_to_double(scm_op2)), *dst, s);
        else {
            cv::cuda::GpuMat *m2 = (cv::cuda::GpuMat*)scm_to_pointer(scm_op2);
            if (m1->size() != m2->size()) {
                cv::cuda::GpuMat exp;
                if (m1->rows == 1 && m1->cols == m2->cols) {
                    dst->create(m2->size(), m2->type());
                    exp.create(m2->size(), m2->type());
                    cv::cuda::GpuMat ones(m2->rows, 1, m2->type(), cv::Scalar(1.0));
                    cv::cuda::gemm(ones, *m1, 1.0, cv::noArray(), 0.0, exp);
                    cv::cuda::divide(exp, *m2, *dst, s);
                } else if (m2->rows == 1 && m2->cols == m1->cols) {
                    dst->create(m1->size(), m1->type());
                    exp.create(m1->size(), m1->type());
                    cv::cuda::GpuMat ones(m1->rows, 1, m1->type(), cv::Scalar(1.0));
                    cv::cuda::gemm(ones, *m2, 1.0, cv::noArray(), 0.0, exp);
                    cv::cuda::divide(*m1, exp, *dst, s);
                }
            } else cv::cuda::divide(*m1, *m2, *dst, s);
        }
    }
    return scm_dst;
}

static SCM mtfa_opencv_gpumat_cross_entropy(SCM scm_pred, SCM scm_target) {
    cv::cuda::GpuMat *pred = (cv::cuda::GpuMat*) scm_to_pointer(scm_pred);
    cv::cuda::GpuMat *target = (cv::cuda::GpuMat*) scm_to_pointer(scm_target);

    if (pred->empty() || target->empty()) return scm_from_double(0.0);

    // 1. Clamp values: assicura che 1e-7 <= pred <= 1.0
    //    Questo rimuove ogni possibilita' di log(0)
    cv::cuda::GpuMat safe_pred;
    cv::cuda::max(*pred, cv::Scalar(1e-7), safe_pred); 
    cv::cuda::min(safe_pred, cv::Scalar(1.0), safe_pred);

    // 2. Logaritmo
    cv::cuda::log(safe_pred, safe_pred);

    // 3. Prodotto scalare (Target * Log(Pred))
    cv::cuda::GpuMat product;
    cv::cuda::multiply(*target, safe_pred, product);

    // 4. Somma
    cv::Scalar total_sum = cv::cuda::sum(product);
    
    // 5. Media negativa
    //double loss = -total_sum[0] / (double)pred->rows;
    double loss = -total_sum[0] / (double)pred->cols;

    return scm_from_double(loss);
}

// Crea una "vista" (ROI) di una matrice esistente senza copiare i dati.
// Equivalente a: mat.rowRange(start, end)
static SCM mtfa_opencv_gpumat_row_range(SCM scm_mat, SCM scm_start, SCM scm_end) {
    cv::cuda::GpuMat *src = (cv::cuda::GpuMat*) scm_to_pointer(scm_mat);
    int start = scm_to_int(scm_start);
    int end = scm_to_int(scm_end);

    // Controllo limiti
    if (start < 0) start = 0;
    if (end > src->rows) end = src->rows;
    if (start >= end) return SCM_BOOL_F;

    // Crea un nuovo header GpuMat che punta alla stessa memoria (O(1) complexity)
    cv::cuda::GpuMat *dst = new cv::cuda::GpuMat(src->rowRange(start, end));

    return scm_from_pointer(dst, releaseGpuMat);
}

static SCM mtfa_opencv_gpumat_col_range(SCM scm_mat, SCM scm_start, SCM scm_end) {
    cv::cuda::GpuMat *src = (cv::cuda::GpuMat*) scm_to_pointer(scm_mat);
    int start = scm_to_int(scm_start);
    int end = scm_to_int(scm_end);

    if (start < 0 || end > src->cols || start >= end) return SCM_BOOL_F;

    // Estrae un batch di colonne (i campioni)
    cv::cuda::GpuMat *dst = new cv::cuda::GpuMat(src->colRange(start, end));
    return scm_from_pointer(dst, releaseGpuMat);
}
static SCM mtfa_opencv_gpumat_max_val(SCM scm_src) {
    cv::cuda::GpuMat *src = (cv::cuda::GpuMat*)scm_to_pointer(scm_src);
    if (src->empty()) return scm_from_double(0.0);
    
    double minVal, maxVal;
    // minMax trova il valore minimo e massimo nella matrice GPU
    cv::cuda::minMax(*src, &minVal, &maxVal);
    
    return scm_from_double(maxVal);
}
// Registrala come: mtfa-opencv-gpumat-max-val


static SCM mtfa_opencv_gpumat_rand_uniform_cpu(SCM scm_mat, SCM scm_min, SCM scm_max) {
    cv::cuda::GpuMat *d_mat = (cv::cuda::GpuMat*)scm_to_pointer(scm_mat);
    
    // Safety Check
    if (!d_mat || d_mat->empty()) {
        return SCM_BOOL_F; // O lancia un errore
    }

    double min_val = scm_to_double(scm_min);
    double max_val = scm_to_double(scm_max);

    // 1. Creiamo una matrice temporanea in RAM (CPU)
    cv::Mat h_mat(d_mat->size(), d_mat->type());

    // 2. La riempiamo usando la CPU (disponibile ovunque)
    cv::randu(h_mat, cv::Scalar(min_val), cv::Scalar(max_val));

    // 3. Spediamo tutto alla GPU
    d_mat->upload(h_mat);

    return SCM_BOOL_T;
}

#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <libguile.h>

// Aggiungiamo un 3° parametro opzionale: scm_print_cols
SCM mtfa_opencv_gpumat_debug_dump(SCM scm_mat, SCM scm_name, SCM scm_print_cols) {
    cv::cuda::GpuMat* mat = (cv::cuda::GpuMat*) scm_to_pointer(scm_mat);
    char* name = scm_to_locale_string(scm_name);
    
    // Gestione dell'argomento opzionale di Guile
    int print_limit = 0;
    if (!SCM_UNBNDP(scm_print_cols) && scm_is_number(scm_print_cols)) {
        print_limit = scm_to_int(scm_print_cols);
    }

    if(!mat || mat->empty()) {
        std::cout << "[DEBUG] " << name << " -> ERRORE: Matrice NULL o VUOTA!" << std::endl;
        free(name);
        return SCM_BOOL_F;
    }
    
    cv::Mat cpu_mat;
    mat->download(cpu_mat); // Portiamo i dati sulla CPU per leggerli
    
    double minVal, maxVal;
    cv::minMaxLoc(cpu_mat, &minVal, &maxVal); // Ignora i NaN se presenti
    
    std::cout << "[DEBUG] " << name 
              << " | Dimensioni: " << cpu_mat.rows << "x" << cpu_mat.cols 
              << " | Min: " << minVal << " | Max: " << maxVal;
              
    // Se non abbiamo chiesto di stampare colonne specifiche, stampa solo [0,0] (comportamento classico)
    if (print_limit <= 0) {
         std::cout << " | [0,0]: " << cpu_mat.at<float>(0,0) << std::endl;
    } else {
         std::cout << std::endl;
         
         // Limiti di sicurezza per non inondare la console se sbagliamo numero
         int cols_to_print = std::min(cpu_mat.cols, print_limit);
         int rows_to_print = std::min(cpu_mat.rows, 10); 
         
         for (int r = 0; r < rows_to_print; ++r) {
             std::cout << "   Riga " << r << ": [";
             for (int c = 0; c < cols_to_print; ++c) {
                 std::cout << cpu_mat.at<float>(r, c);
                 if (c < cols_to_print - 1) std::cout << ", ";
             }
             if (cols_to_print < cpu_mat.cols) std::cout << ", ...";
             std::cout << "]" << std::endl;
         }
         if (rows_to_print < cpu_mat.rows) std::cout << "   ..." << std::endl;
    }
              
    // Controllo NaN brutale
    int nan_count = cv::countNonZero(cv::Mat(cpu_mat != cpu_mat));
    if (nan_count > 0) {
        std::cout << "   !!! ALLARME CRITICO: Trovati " << nan_count << " NaN in " << name << " !!!" << std::endl;
    }
    
    free(name);
    return SCM_BOOL_T;
}

SCM mtfa_opencv_gpumat_evaluate_batch(SCM scm_pred, SCM scm_target, SCM scm_print_cols) {
    cv::cuda::GpuMat* d_pred = (cv::cuda::GpuMat*) scm_to_pointer(scm_pred);
    cv::cuda::GpuMat* d_target = (cv::cuda::GpuMat*) scm_to_pointer(scm_target);

    if(!d_pred || !d_target || d_pred->empty() || d_target->empty()) 
        return scm_cons(scm_from_int(0), SCM_EOL);

    // Gestione dell'argomento opzionale di Guile
    int print_limit = 5;
    if (!SCM_UNBNDP(scm_print_cols) && scm_is_number(scm_print_cols)) {
        print_limit = scm_to_int(scm_print_cols);
    }

    cv::Mat pred, target;
    d_pred->download(pred);
    d_target->download(target);

    int correct = 0;
    int batch_size = pred.cols;
    int num_samples_to_return = std::min(print_limit, batch_size);
    
    // LA MAGIA: Riconosce automaticamente il tipo di rete!
    bool is_binary = (pred.rows == 1); 

    // 1. Calcolo del totale delle risposte esatte
    for(int c = 0; c < batch_size; ++c) {
        if (is_binary) {
            float p_val = pred.at<float>(0, c);
            float t_val = target.at<float>(0, c);
            int p_class = (p_val >= 0.5f) ? 1 : 0;
            int t_class = (t_val >= 0.5f) ? 1 : 0;
            if (p_class == t_class) correct++;
        } else {
            double minP, maxP, minT, maxT;
            cv::Point minLocP, maxLocP, minLocT, maxLocT;
            cv::minMaxLoc(pred.col(c), &minP, &maxP, &minLocP, &maxLocP);
            cv::minMaxLoc(target.col(c), &minT, &maxT, &minLocT, &maxLocT);
            if (maxLocP.y == maxLocT.y) correct++;
        }
    }

    // 2. Creazione della lista per Scheme
    SCM samples_list = SCM_EOL;
    for(int c = num_samples_to_return - 1; c >= 0; --c) {
        int p_class, t_class;
        double conf;

        if (is_binary) {
            float p_val = pred.at<float>(0, c);
            float t_val = target.at<float>(0, c);
            p_class = (p_val >= 0.5f) ? 1 : 0;
            t_class = (t_val >= 0.5f) ? 1 : 0;
            // Nel binario, la confidenza è quanto sei vicino a 1.0 o a 0.0
            conf = (p_val >= 0.5f) ? (p_val * 100.0) : ((1.0f - p_val) * 100.0);
        } else {
            double minP, maxP, minT, maxT;
            cv::Point minLocP, maxLocP, minLocT, maxLocT;
            cv::minMaxLoc(pred.col(c), &minP, &maxP, &minLocP, &maxLocP);
            cv::minMaxLoc(target.col(c), &minT, &maxT, &minLocT, &maxLocT);
            p_class = maxLocP.y;
            t_class = maxLocT.y;
            conf = maxP * 100.0;
        }

        SCM sample_info = scm_list_3(scm_from_int(p_class), scm_from_int(t_class), scm_from_double(conf));
        samples_list = scm_cons(sample_info, samples_list);
    }

    return scm_cons(scm_from_int(correct), samples_list);
}

static SCM mtfa_opencv_gpumat_math_sign(SCM scm_dst, SCM scm_op1) {
    cv::cuda::GpuMat *op1 = (cv::cuda::GpuMat*)scm_to_pointer(scm_op1);
    cv::cuda::GpuMat *dst = (cv::cuda::GpuMat*)scm_to_pointer(scm_dst);

    // Controllo tipo 
    if (op1->type() != MYTYPE) {
        scm_error(scm_from_latin1_symbol("opencv-error"), "gpumat_math_sign", "Expected CV_32F", SCM_EOL, SCM_BOOL_F);
    }

    // Creiamo una matrice temporanea per calcolare il denominatore
    cv::cuda::GpuMat abs_mat;

    // 1. abs_mat = |x|
    cv::cuda::abs(*op1, abs_mat);

    // 2. abs_mat = |x| + epsilon (per evitare divisione per zero se x è esattamente 0.0)
    cv::cuda::add(abs_mat, cv::Scalar(1e-7f), abs_mat);

    // 3. dst = x / (|x| + epsilon)
    // Se x è positivo -> ~1.0. Se x è negativo -> ~ -1.0. Se x è 0 -> 0.0
    cv::cuda::divide(*op1, abs_mat, *dst);

    return scm_dst;
}

static SCM mtfa_opencv_make_cpu_mat(SCM scm_rows, SCM scm_cols) {
    int r = scm_to_int(scm_rows);
    int c = scm_to_int(scm_cols);
    // Creiamo esplicitamente una matrice di float a 32 bit
    cv::Mat *mat = new cv::Mat(r, c, MYTYPE); 
    return scm_from_pointer(mat, releaseMat);
}

// SET: prende un numero Scheme e lo scrive come float 32-bit
static SCM mtfa_opencv_mat_set(SCM scm_mat, SCM scm_row, SCM scm_col, SCM scm_val) {
    cv::Mat *mat = (cv::Mat*) scm_to_pointer(scm_mat);
    mat->at<float>(scm_to_int(scm_row), scm_to_int(scm_col)) = (float)scm_to_double(scm_val);
    return scm_mat;
}


/****************************************************************************/
extern "C" void init_mtfa_opencv();
void init_mtfa_opencv()
{
    // SCM name;
    // SCM slots;
    // scm_t_struct_finalize finalizer;
    // name = scm_from_latin1_symbol ( "mtfa-opencv" );
    // slots = scm_list_1 ( scm_from_latin1_symbol ( "fapncv" ) );
    // finalizer = finalize_mtfa_detector;
    // mtfa_detector_type = scm_make_foreign_object_type ( name, slots, finalizer );

    SCM zed2_name, zed2_slots;
    scm_t_struct_finalize zed2_finalizer;
    zed2_name = scm_from_latin1_symbol ( "mtfa-zed2" );
    zed2_slots = scm_list_1 ( scm_from_latin1_symbol ( "fapnz2" ) );
    zed2_finalizer = finalize_zed2_detector;
    mtfa_zed2_detector_type = scm_make_foreign_object_type ( zed2_name, zed2_slots, zed2_finalizer );

    // SCM yolo4_name, yolo4_slots;
    // scm_t_struct_finalize yolo4_finalizer;
    // yolo4_name = scm_from_latin1_symbol ( "mtfa-yolo4" );
    // yolo4_slots = scm_list_1 ( scm_from_latin1_symbol ( "fapny4" ) );
    // yolo4_finalizer = finalize_yolo4_detector;
    // mtfa_yolo4_detector_type = scm_make_foreign_object_type ( yolo4_name, yolo4_slots, yolo4_finalizer );

    // scm_c_define_gsubr ( "mtfa_opencv_detector_p", 1, 0, 0, ( void* ) mtfa_detector_p );
    // scm_c_define_gsubr ( "mtfa_opencv_detector_make", 2, 0, 0, ( void* ) mtfa_detector_make );
    // scm_c_define_gsubr ( "mtfa_opencv_detector_run", 0, 2, 0, ( void* ) mtfa_detector_run );
    // scm_c_define_gsubr ( "mtfa_opencv_detector_stop", 0, 1, 0, ( void* ) mtfa_detector_stop );
    // scm_c_define_gsubr ( "mtfa_opencv_detector_get_zed_camera", 2, 0, 0, ( void* ) mtfa_detector_get_zed_camera );

    scm_c_define_gsubr ( "mtfa_zed2_detector_p", 1, 0, 0, ( void* ) mtfa_zed2_detector_p );
    scm_c_define_gsubr ( "mtfa_zed2_detector_make", 0, 0, 0, ( void* ) mtfa_zed2_detector_make );
    scm_c_define_gsubr ( "mtfa_zed2_detector_run", 2, 0, 0, ( void* ) mtfa_zed2_detector_run );
    scm_c_define_gsubr ( "mtfa_zed2_detector_stop", 1, 0, 0, ( void* ) mtfa_zed2_detector_stop );
    scm_c_define_gsubr ( "mtfa_zed2_detector_get_zed_camera", 2, 0, 0, ( void* ) mtfa_zed2_detector_get_zed_camera );

    // scm_c_define_gsubr ( "mtfa_yolo4_detector_p", 1, 0, 0, ( void* ) mtfa_yolo4_detector_p );
    // scm_c_define_gsubr ( "mtfa_yolo4_detector_make", 0, 0, 0, ( void* ) mtfa_yolo4_detector_make );
    // scm_c_define_gsubr ( "mtfa_yolo4_detector_run", 2, 0, 0, ( void* ) mtfa_yolo4_detector_run );
    // scm_c_define_gsubr ( "mtfa_yolo4_detector_stop", 1, 0, 0, ( void* ) mtfa_yolo4_detector_stop );

    scm_c_define_gsubr ( "mtfa_opencv_imgfile_to_mat", 1, 0, 0, ( void* ) mtfa_opencv_imgfile_to_mat );
    scm_c_define_gsubr ( "mtfa_opencv_mat_to_imgfile", 2, 0, 0, ( void* ) mtfa_opencv_mat_to_imgfile );

    scm_c_define_gsubr ( "mtfa_opencv_bytevector_to_imgmat", 1, 0, 0, ( void* ) mtfa_opencv_bytevector_to_imgmat );
    scm_c_define_gsubr ( "mtfa_opencv_imgmat_to_bytevector", 2, 0, 0, ( void* ) mtfa_opencv_imgmat_to_bytevector );
    
    scm_c_define_gsubr ( "mtfa_opencv_crop_rectangle", 5, 0, 0, ( void* ) mtfa_opencv_crop_rectangle );

    scm_c_define_gsubr ( "mtfa_opencv_compute_image_hash_AverageHash", 1, 0, 0, ( void* ) mtfa_opencv_compute_img_hash_AverageHash );
    scm_c_define_gsubr ( "mtfa_opencv_compare_image_hash_AverageHash", 2, 0, 0, ( void* ) mtfa_opencv_compare_img_hash_AverageHash );
    scm_c_define_gsubr ( "mtfa_opencv_compute_image_hash_BlockMeanHash0", 1, 0, 0, ( void* ) mtfa_opencv_compute_img_hash_BlockMeanHash0 );
    scm_c_define_gsubr ( "mtfa_opencv_compare_image_hash_BlockMeanHash0", 2, 0, 0, ( void* ) mtfa_opencv_compare_img_hash_BlockMeanHash0 );
    scm_c_define_gsubr ( "mtfa_opencv_compute_image_hash_BlockMeanHash1", 1, 0, 0, ( void* ) mtfa_opencv_compute_img_hash_BlockMeanHash1 );
    scm_c_define_gsubr ( "mtfa_opencv_compare_image_hash_BlockMeanHash1", 2, 0, 0, ( void* ) mtfa_opencv_compare_img_hash_BlockMeanHash1 );
    scm_c_define_gsubr ( "mtfa_opencv_compute_image_hash_ColorMomentHash", 1, 0, 0, ( void* ) mtfa_opencv_compute_img_hash_ColorMomentHash );
    scm_c_define_gsubr ( "mtfa_opencv_compare_image_hash_ColorMomentHash", 2, 0, 0, ( void* ) mtfa_opencv_compare_img_hash_ColorMomentHash );
    scm_c_define_gsubr ( "mtfa_opencv_compute_image_hash_MarrHildrethHash", 1, 0, 0, ( void* ) mtfa_opencv_compute_img_hash_MarrHildrethHash );
    scm_c_define_gsubr ( "mtfa_opencv_compare_image_hash_MarrHildrethHash", 2, 0, 0, ( void* ) mtfa_opencv_compare_img_hash_MarrHildrethHash );
    scm_c_define_gsubr ( "mtfa_opencv_compute_image_hash_PHash", 1, 0, 0, ( void* ) mtfa_opencv_compute_img_hash_PHash );
    scm_c_define_gsubr ( "mtfa_opencv_compare_image_hash_PHash", 2, 0, 0, ( void* ) mtfa_opencv_compare_img_hash_PHash );
    scm_c_define_gsubr ( "mtfa_opencv_compute_image_hash_RadialVarianceHash", 1, 0, 0, ( void* ) mtfa_opencv_compute_img_hash_RadialVarianceHash );
    scm_c_define_gsubr ( "mtfa_opencv_compare_image_hash_RadialVarianceHash", 2, 0, 0, ( void* ) mtfa_opencv_compare_img_hash_RadialVarianceHash );

    scm_c_define_gsubr ( "mtfa_opencv_img_hash_to_bytevector", 1, 0, 0, ( void* ) mtfa_opencv_img_hash_to_bytevector );
    scm_c_define_gsubr ( "mtfa_opencv_bytevector_to_img_hash", 1, 0, 0, ( void* ) mtfa_opencv_bytevector_to_img_hash );

    scm_c_define_gsubr ( "mtfa_opencv_make_empty_mat", 0, 0, 0, (void*)mtfa_opencv_make_empty_mat);
    scm_c_define_gsubr ( "mtfa_opencv_make_mat_zeros", 5, 0, 0, ( void* ) mtfa_opencv_make_mat_zeros ); // (SCM scm_rows, SCM scm_cols, SCM scm_bytelen, SCM scm_type, SCM scm_colors)

    scm_c_define_gsubr ( "mtfa_opencv_draw_imshow", 2, 0, 0, ( void* ) mtfa_opencv_draw_imshow ); // (SCM scm_wname, SCM scm_mat)
    scm_c_define_gsubr ( "mtfa_opencv_draw_move_window", 3, 0, 0, ( void* ) mtfa_opencv_draw_move_window ); // (SCM scm_wname, SCM s_x, SCM s_y)
    scm_c_define_gsubr ( "mtfa_opencv_draw_wait_key", 1, 0, 0, ( void* ) mtfa_opencv_draw_wait_key ); // (SCM s_ms)
    scm_c_define_gsubr ( "mtfa_opencv_draw_rectangle", 8, 0, 0, ( void* ) mtfa_opencv_draw_rectangle ); //(SCM s_mat, SCM s_x, SCM s_y, SCM s_w, SCM s_h, SCM s_rgb, SCM s_thick, SCM s_linetype)
    scm_c_define_gsubr ( "mtfa_opencv_draw_circle", 7, 0, 0, ( void* ) mtfa_opencv_draw_circle ); //(SCM s_mat, SCM s_x, SCM s_y, SCM s_radius, SCM s_rgb, SCM s_thick, SCM s_linetype)
    scm_c_define_gsubr ( "mtfa_opencv_draw_ellipse", 10, 0, 0, ( void* ) mtfa_opencv_draw_ellipse); //( SCM s_mat, SCM s_x, SCM s_y, SCM s_axes, SCM s_angle, SCM s_start_angle, SCM s_end_angle, SCM s_rgb, SCM s_thick, SCM s_linetype )
    scm_c_define_gsubr ( "mtfa_opencv_draw_polygon", 7, 0, 0, (void*)mtfa_opencv_draw_polygon); // ( SCM s_mat, SCM scm_points, SCM s_ncontours, SCM s_isClosed, SCM s_rgb, SCM s_thick, SCM s_linetype )
    scm_c_define_gsubr ( "mtfa_opencv_fill_polygons", 3, 0, 0, (void*)mtfa_opencv_fill_polygons); // ( SCM s_mat, SCM scm_points, SCM s_rgb)
    scm_c_define_gsubr ( "mtfa_opencv_fill_convex_polygon", 3, 0, 0, (void*)mtfa_opencv_fill_convex_polygon); //( SCM s_mat, SCM scm_points, SCM s_rgb)

    scm_c_define_gsubr ( "mtfa_opencv_draw_destroy_window", 1, 0, 0, ( void* ) mtfa_opencv_draw_destroy_window ); //(SCM s_name)
    scm_c_define_gsubr ( "mtfa_opencv_draw_destroy_all_windows", 0, 0, 0, ( void* ) mtfa_opencv_draw_destroy_all_windows );
    scm_c_define_gsubr ( "mtfa_opencv_draw_named_window", 2, 0, 0, ( void* ) mtfa_opencv_draw_named_window ); //(SCM s_name, SCM s_type)
    scm_c_define_gsubr ( "mtfa_opencv_draw_resize_window", 3, 0, 0, ( void* ) mtfa_opencv_draw_resize_window ); //(SCM s_name, SCM s_w, SCM s_h)
    scm_c_define_gsubr ( "mtfa_opencv_draw_put_text", 9, 0, 0, ( void* ) mtfa_opencv_draw_put_text ); //(SCM s_mat, SCM s_text, SCM s_x, SCM s_y, SCM s_fontFace, SCM s_fontScale, SCM s_rgb, SCM s_thick, SCM s_linetype)
    scm_c_define_gsubr ( "mtfa_opencv_draw_text_size", 4, 0, 0, ( void* ) mtfa_opencv_draw_text_size ); //( SCM s_text, SCM s_fontFace, SCM s_fontScale, SCM s_thick)

    scm_c_define_gsubr ( "mtfa_opencv_draw_line", 8, 0, 0, ( void* ) mtfa_opencv_draw_line ); //(SCM s_mat, SCM s_x1, SCM s_y1, SCM s_x2, SCM s_y2, SCM s_rgb, SCM s_thick, SCM s_linetype)
    scm_c_define_gsubr ( "mtfa_opencv_mat_info", 1, 0, 0, ( void* ) mtfa_opencv_mat_info ); //(SCM s_mat)
    scm_c_define_gsubr ( "mtfa_opencv_cap_info", 1, 0, 0, ( void* ) mtfa_opencv_cap_info ); //(SCM scm_cap)

    scm_c_define_gsubr ( "mtfa_opencv_draw_display_overlay", 3, 0, 0, ( void* ) mtfa_opencv_draw_display_overlay );
    scm_c_define_gsubr ( "mtfa_opencv_draw_display_status_bar", 3, 0, 0, ( void* ) mtfa_opencv_draw_display_statusbar );
    scm_c_define_gsubr ( "mtfa_opencv_draw_set_window_property", 3, 0, 0, ( void* ) mtfa_opencv_draw_set_window_property );

    scm_c_define_gsubr ( "mtfa_lap", 1, 0, 0, ( void* ) mtfa_lap );

    scm_c_define_gsubr ( "mtfa_opencv_change_brightness", 2, 0, 0, (void*)mtfa_opencv_change_brightness);
    scm_c_define_gsubr ( "mtfa_opencv_change_contrast", 2, 0, 0, (void*)mtfa_opencv_change_contrast);
    scm_c_define_gsubr ( "mtfa_opencv_resize", 3, 0, 0, (void*)mtfa_opencv_resize);
    scm_c_define_gsubr ( "mtfa_opencv_prop_resize", 2, 0, 0, (void*)mtfa_opencv_prop_resize);
    scm_c_define_gsubr ( "mtfa_opencv_gray", 1, 0, 0, (void*)mtfa_opencv_gray);
    scm_c_define_gsubr ( "mtfa_opencv_add_gaussian_noise", 3, 0, 0, (void*)mtfa_opencv_add_gaussian_noise);
    scm_c_define_gsubr ( "mtfa_opencv_salt_and_pepper", 3, 0, 0, (void*)mtfa_opencv_salt_and_pepper);
    scm_c_define_gsubr ( "mtfa_opencv_gaussian_blur", 3, 0, 0, (void*)mtfa_opencv_gaussian_blur);
    scm_c_define_gsubr ( "mtfa_opencv_median_blur", 2, 0, 0, (void*)mtfa_opencv_median_blur);
    scm_c_define_gsubr ( "mtfa_opencv_rotate", 2, 0, 0, (void*)mtfa_opencv_rotate);
    scm_c_define_gsubr ( "mtfa_opencv_erode", 4, 0, 0, (void*)mtfa_opencv_erode);
    scm_c_define_gsubr ( "mtfa_opencv_contours", 3, 0, 0, (void*)mtfa_opencv_contours);
    scm_c_define_gsubr ( "mtfa_opencv_dilate", 4, 0, 0, (void*)mtfa_opencv_dilate);
    scm_c_define_gsubr ( "mtfa_opencv_transform", 9, 0, 0, (void*)mtfa_opencv_transform);
    scm_c_define_gsubr ( "mtfa_opencv_gamma_correction", 2, 0, 0, (void*)mtfa_opencv_gamma_correction);
    scm_c_define_gsubr ( "mtfa_opencv_equalize_histogram", 1, 0, 0, (void*)mtfa_opencv_equalize_histogram);
    scm_c_define_gsubr ( "mtfa_opencv_add_watermark", 6, 0, 0, (void*)mtfa_opencv_add_watermark);
    scm_c_define_gsubr ( "mtfa_opencv_jpeg_compression", 2, 0, 0, (void*)mtfa_opencv_jpeg_compression);

    scm_c_define_gsubr ( "mtfa_opencv_open_input_stream", 1, 1, 0, (void*)mtfa_opencv_open_input_stream);
    scm_c_define_gsubr ( "mtfa_opencv_close_input_stream", 1, 0, 0, (void*)mtfa_opencv_close_input_stream);
    scm_c_define_gsubr ( "mtfa_opencv_read_input_stream", 2, 0, 0, (void*)mtfa_opencv_read_input_stream);
    scm_c_define_gsubr ( "mtfa_opencv_input_stream_is_opened", 1, 0, 0, (void*)mtfa_opencv_input_stream_is_opened);

    scm_c_define_gsubr ( "mtfa_opencv_open_output_stream", 5, 0, 0, (void*)mtfa_opencv_open_output_stream);
    scm_c_define_gsubr ( "mtfa_opencv_close_output_stream", 1, 0, 0, (void*)mtfa_opencv_close_output_stream);
    scm_c_define_gsubr ( "mtfa_opencv_write_output_stream", 2, 0, 0, (void*)mtfa_opencv_write_output_stream);
    scm_c_define_gsubr ( "mtfa_opencv_output_stream_is_opened", 1, 0, 0, (void*)mtfa_opencv_output_stream_is_opened);

    scm_c_define_gsubr ( "mtfa_opencv_zed_camera_grab", 1, 0, 0, (void*)mtfa_zed_camera_grab);
    scm_c_define_gsubr ( "mtfa_opencv_zed_camera_get_image", 1, 0, 0, (void*)mtfa_zed_camera_get_image);
    scm_c_define_gsubr ( "mtfa_opencv_zed_camera_get", 2, 0, 0, (void*)mtfa_zed_camera_get);
    scm_c_define_gsubr ( "mtfa_opencv_zed_camera_set", 3, 0, 0, (void*)mtfa_zed_camera_set);
    scm_c_define_gsubr ( "mtfa_opencv_zed_camera_reset", 1, 0, 0, (void*)mtfa_zed_camera_reset);
//     scm_c_define_gsubr ( "mtfa_opencv_get_raw_cloud", 1, 0, 0, (void*)mtfa_opencv_get_raw_cloud);
    scm_c_define_gsubr ( "mtfa_opencv_binary_op", 3, 0, 0, (void*)mtfa_opencv_binary_op);

    scm_c_define_gsubr ( "mtfa_opencv_make_polygon", 1, 0, 0, (void*)mtfa_opencv_make_polygon);
    scm_c_define_gsubr ( "mtfa_opencv_point_in_polygon", 2, 0, 0, (void*)mtfa_opencv_point_in_polygon);
    scm_c_define_gsubr ( "mtfa_opencv_compose", 3, 0, 0, (void*)mtfa_opencv_compose);
    scm_c_define_gsubr ( "mtfa_opencv_clone", 1, 0, 0, (void*)mtfa_opencv_clone);

    scm_c_define_gsubr ( "mtfa_opencv_rgbtohsv", 3, 0, 0, (void*)mtfa_opencv_rgbtohsv);

    scm_c_define_gsubr ( "mtfa_opencv_create_trackbar", 6, 0, 0, (void*)mtfa_opencv_create_trackbar);
    scm_c_define_gsubr ( "mtfa_opencv_set_trackbar_max", 3, 0, 0, (void*)mtfa_opencv_set_trackbar_max);
    scm_c_define_gsubr ( "mtfa_opencv_set_trackbar_min", 3, 0, 0, (void*)mtfa_opencv_set_trackbar_min);
    scm_c_define_gsubr ( "mtfa_opencv_set_trackbar_pos", 3, 0, 0, (void*)mtfa_opencv_set_trackbar_pos);
    scm_c_define_gsubr ( "mtfa_opencv_get_trackbar_pos", 2, 0, 0, (void*)mtfa_opencv_get_trackbar_pos);

    scm_c_define_gsubr ( "mtfa_opencv_find_circles", 9, 0, 0, (void*)mtfa_opencv_find_circles);
    scm_c_define_gsubr ( "mtfa_opencv_change_brightness_and_contrast", 3, 0, 0, (void*)mtfa_opencv_change_brightness_and_contrast); //0..200, 0..200
    scm_c_define_gsubr ( "mtfa_opencv_img_mean", 4, 0, 0, (void*)mtfa_opencv_img_mean);

    scm_c_define_gsubr ( "mtfa_opencv_set_mouse_callback", 3, 0, 0, (void*)mtfa_opencv_set_mouse_callback);

    //I phtree!!
    scm_c_define_gsubr ( "mtfa_phtree_box_2d_make", 0, 0, 0, (void*)mtfa_phtree_box_2d_make);
    scm_c_define_gsubr ( "mtfa_phtree_box_3d_make", 0, 0, 0, (void*)mtfa_phtree_box_3d_make);
    scm_c_define_gsubr ( "mtfa_phtree_point_2d_make", 0, 0, 0, (void*)mtfa_phtree_point_2d_make);
    scm_c_define_gsubr ( "mtfa_phtree_point_3d_make", 0, 0, 0, (void*)mtfa_phtree_point_3d_make);

    scm_c_define_gsubr ( "mtfa_phtree_2d_add_points", 2, 0, 0, (void*)mtfa_phtree_2d_add_points);
    scm_c_define_gsubr ( "mtfa_phtree_3d_add_points", 2, 0, 0, (void*)mtfa_phtree_3d_add_points);

    scm_c_define_gsubr ( "mtfa_phtree_2d_add_boxes", 2, 0, 0, (void*)mtfa_phtree_2d_add_boxes);
    scm_c_define_gsubr ( "mtfa_phtree_3d_add_boxes", 2, 0, 0, (void*)mtfa_phtree_3d_add_boxes);

    scm_c_define_gsubr ( "mtfa_phtree_2d_query_points", 3, 0, 0, (void*)mtfa_phtree_2d_query_points);
    scm_c_define_gsubr ( "mtfa_phtree_3d_query_points", 3, 0, 0, (void*)mtfa_phtree_3d_query_points);

    scm_c_define_gsubr ( "mtfa_phtree_2d_query_boxes", 4, 0, 0, (void*)mtfa_phtree_2d_query_boxes);
    scm_c_define_gsubr ( "mtfa_phtree_3d_query_boxes", 4, 0, 0, (void*)mtfa_phtree_3d_query_boxes);

    scm_c_define_gsubr ( "mtfa_phtree_2d_knn_points", 3, 0, 0, (void*)mtfa_phtree_2d_knn_points);
    scm_c_define_gsubr ( "mtfa_phtree_3d_knn_points", 3, 0, 0, (void*)mtfa_phtree_3d_knn_points);

    scm_c_define_gsubr ( "mtfa_phtree_2d_getall_points", 1, 0, 0, (void*)mtfa_phtree_2d_getall_points);
    scm_c_define_gsubr ( "mtfa_phtree_3d_getall_points", 1, 0, 0, (void*)mtfa_phtree_3d_getall_points);
    scm_c_define_gsubr ( "mtfa_phtree_2d_getall_boxes", 1, 0, 0, (void*)mtfa_phtree_2d_getall_boxes);
    scm_c_define_gsubr ( "mtfa_phtree_3d_getall_boxes", 1, 0, 0, (void*)mtfa_phtree_3d_getall_boxes);

    scm_c_define_gsubr ( "mtfa_phtree_2d_erase_points", 2, 0, 0, (void*)mtfa_phtree_2d_erase_points);
    scm_c_define_gsubr ( "mtfa_phtree_3d_erase_points", 2, 0, 0, (void*)mtfa_phtree_3d_erase_points);
    scm_c_define_gsubr ( "mtfa_phtree_2d_erase_boxes", 2, 0, 0, (void*)mtfa_phtree_2d_erase_boxes);
    scm_c_define_gsubr ( "mtfa_phtree_3d_erase_boxes", 2, 0, 0, (void*)mtfa_phtree_3d_erase_boxes);

    scm_c_define_gsubr ( "mtfa_opencv_make_interpolator", 1, 0, 0, (void*)mtfa_opencv_make_interpolator);
    scm_c_define_gsubr ( "mtfa_opencv_interpolator_add_serie", 2, 0, 0, (void*)mtfa_opencv_interpolator_add_serie);
    scm_c_define_gsubr ( "mtfa_opencv_interpolator_interpolate", 2, 0, 0, (void*)mtfa_opencv_interpolator_interpolate);
    
    
    scm_c_define_gsubr ( "mtfa_opencv_levenshtein_distance", 2, 0, 0, (void*)LevenshteinDistance);
    scm_c_define_gsubr ( "mtfa_opencv_features_detector", 1, 0, 0, (void*)mtfa_opencv_features_detector);
    scm_c_define_gsubr ( "mtfa_opencv_features_find_transformation", 6, 0, 0, (void*)mtfa_opencv_features_find_transformation);

    scm_c_define_gsubr ( "mtfa_opencv_make_gpu_mat", 3, 0, 0, (void*)mtfa_opencv_make_gpu_mat);
    scm_c_define_gsubr ( "mtfa_opencv_convert_mat_to_gpumat", 1, 1, 0, (void*)mtfa_opencv_convert_mat_to_gpumat);
    scm_c_define_gsubr ( "mtfa_opencv_convert_gpumat_to_mat", 1, 1, 0, (void*)mtfa_opencv_convert_gpumat_to_mat);
    scm_c_define_gsubr ( "mtfa_opencv_make_gpumat_from_lolod", 1, 1, 0, (void*)mtfa_opencv_make_gpumat_from_lolod);
    scm_c_define_gsubr ( "mtfa_opencv_convert_gpumat_to_lolod", 1, 0, 0, (void*)mtfa_opencv_convert_gpumat_to_lolod);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_gemm", 9, 0, 0, (void*)mtfa_opencv_gpumat_gemm);
    scm_c_define_gsubr ( "mtfa_opencv_transpose_gpumat", 2, 0, 0, (void*)mtfa_opencv_transpose_gpumat);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_rows_cols", 1, 0, 0, (void*)mtfa_opencv_gpumat_rows_cols);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_clone", 1, 1, 0, (void*)mtfa_opencv_gpumat_clone);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_compare", 4, 0, 0, (void*)mtfa_opencv_gpumat_math_compare);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_add", 3, 0, 0, (void*)mtfa_opencv_gpumat_math_add);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_sub", 3, 0, 0, (void*)mtfa_opencv_gpumat_math_sub);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_mul", 4, 0, 0, (void*)mtfa_opencv_gpumat_math_mul);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_div", 4, 0, 0, (void*)mtfa_opencv_gpumat_math_div);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_and", 3, 0, 0, (void*)mtfa_opencv_gpumat_math_and);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_or", 3, 0, 0, (void*)mtfa_opencv_gpumat_math_or);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_xor", 3, 0, 0, (void*)mtfa_opencv_gpumat_math_xor);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_not", 2, 0, 0, (void*)mtfa_opencv_gpumat_math_not);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_absdiff", 3, 0, 0, (void*)mtfa_opencv_gpumat_math_absdiff);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_abs", 2, 0, 0, (void*)mtfa_opencv_gpumat_math_abs);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_sqr", 2, 0, 0, (void*)mtfa_opencv_gpumat_math_sqr);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_sqrt", 2, 0, 0, (void*)mtfa_opencv_gpumat_math_sqrt);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_exp", 2, 0, 0, (void*)mtfa_opencv_gpumat_math_exp);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_log", 2, 0, 0, (void*)mtfa_opencv_gpumat_math_log);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_pow", 3, 0, 0, (void*)mtfa_opencv_gpumat_math_pow);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_min", 3, 0, 0, (void*)mtfa_opencv_gpumat_math_min);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_max", 3, 0, 0, (void*)mtfa_opencv_gpumat_math_max);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_add_weighted", 6, 0, 0, (void*)mtfa_opencv_gpumat_math_add_weighted);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_make_diag", 2, 1, 0, (void*)mtfa_opencv_gpumat_make_diag);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_cuda_reset", 0, 0, 0, (void*)mtfa_opencv_gpumat_cuda_reset);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_cuda_set_device", 1, 0, 0, (void*)mtfa_opencv_gpumat_cuda_set_device);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_find_min_max", 1, 0, 0, (void*)mtfa_opencv_gpumat_math_find_min_max);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_sum", 1, 0, 0, (void*)mtfa_opencv_gpumat_math_sum);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_transpose", 2, 0, 0, (void*)mtfa_opencv_gpumat_transpose);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_convert_vector_to_diag", 2, 0, 0, (void*)mtfa_opencv_gpumat_convert_vector_to_diag);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_set", 4, 0, 0, (void*)mtfa_opencv_gpumat_set);

    scm_c_define_gsubr ( "mtfa_opencv_gpumat_add_row", 3, 0, 0, (void*)mtfa_opencv_gpumat_add_row);
    scm_c_define_gsubr ( "mtfa_opencv_mat_add_row", 2, 0, 0, (void*)mtfa_opencv_mat_add_row);
    scm_c_define_gsubr ( "mtfa_opencv_make_mat_from_lolod", 1, 0, 0, (void*)mtfa_opencv_make_mat_from_lolod);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_del_last_row", 2, 0, 0, (void*)mtfa_opencv_gpumat_del_last_row);

    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_normalize", 5, 0, 0, (void*)mtfa_opencv_gpumat_math_normalize);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_reuse_buffer", 3, 0, 0, (void*)mtfa_opencv_gpumat_reuse_buffer);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_upload_raw", 4, 0, 0, (void*)mtfa_opencv_gpumat_upload_raw);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_upload_batch", 4, 0, 0, (void*)mtfa_opencv_gpumat_upload_batch);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_reduce", 4, 0, 0, (void*)mtfa_opencv_gpumat_reduce);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_tanh", 2, 0, 0, (void*)mtfa_opencv_gpumat_math_tanh);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_argmax_cols", 3, 0, 0, (void*)mtfa_opencv_gpumat_argmax_cols);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_cross_entropy", 2, 0, 0, (void*)mtfa_opencv_gpumat_cross_entropy);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_row_range", 3, 0, 0, (void*)mtfa_opencv_gpumat_row_range);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_col_range", 3, 0, 0, (void*)mtfa_opencv_gpumat_col_range);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_max_val", 1, 0, 0, (void*)mtfa_opencv_gpumat_max_val);

    scm_c_define_gsubr ( "mtfa_opencv_gpumat_rand_uniform_cpu", 3, 0, 0, (void*)mtfa_opencv_gpumat_rand_uniform_cpu);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_debug_dump", 2, 1, 0, (void*)mtfa_opencv_gpumat_debug_dump);   
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_evaluate_batch", 2, 1, 0, (void*)mtfa_opencv_gpumat_evaluate_batch);
    scm_c_define_gsubr ( "mtfa_opencv_gpumat_math_sign", 2, 0, 0, (void *)mtfa_opencv_gpumat_math_sign);    
    scm_c_define_gsubr ( "mtfa_opencv_mat_get", 3, 0, 0, (void *)mtfa_opencv_mat_get);
    scm_c_define_gsubr ( "mtfa_opencv_make_cpu_mat", 2, 0, 0, (void *)mtfa_opencv_make_cpu_mat);    
    scm_c_define_gsubr ( "mtfa_opencv_mat_set", 4, 0, 0, (void *)mtfa_opencv_mat_set);
}
