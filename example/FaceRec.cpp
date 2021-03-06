#pragma warning(disable: 4819)

#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/FaceRecognizer.h>

#include <seeta/Struct.h>
#include <seeta/Struct_cv.h>
#include <seeta/CFaceInfo.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <map>
#include <iostream>
#include <string>

struct Feature
{
    bool isRec;
    float data[1025];
};

class FaceExt
{
  public:
    void init()
    {
        seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
        int id = 0;
        seeta::ModelSetting FD_model( "./model/fd_2_00.dat", device, id );
        seeta::ModelSetting FL_model( "./model/pd_2_00_pts81.dat", device, id );
        seeta::ModelSetting FR_model( "./model/fr_2_10.dat", device, id );

	    FD = new seeta::FaceDetector(FD_model);
	    FL = new seeta::FaceLandmarker(FL_model);
        FR = new seeta::FaceRecognizer(FR_model);

        FD->set(seeta::FaceDetector::PROPERTY_VIDEO_STABLE, 1);

   
    }
    Feature* getFeatures( char* image_path)
    {
        Feature* feature = new Feature();
           
        std::cout<<"Loading image... " << image_path << std::endl;
        auto img = cv::imread(image_path);
        seeta::cv::ImageData image = img;

        if(image.empty())
        {
            std::cerr << "Can not load image " << image_path << " , Please check your image path." << std::endl;
            feature->isRec = false;
            return feature;
        }

        auto faces = FD->detect(image);

        if(faces.size != 1)
        {
            std::cerr << "detect more than one face, Please check image" << image_path << std::endl;
            feature->isRec = false;
            return feature;
        }

        auto face = faces.data[0];
        auto points = FL->mark(image, face.pos);
        auto pps = &points[0];


        std::cout<<"start extract..."<<std::endl;
        if(FR->Extract(image, pps, feature->data))
        {
            std::cout<<"Extract pass"<<std::endl;
            std::cout<<FR->GetExtractFeatureSize() << " features!"<<std::endl;
            std::cout << feature->data[0] << std::endl;
            feature->isRec = true;
            return feature;
        }
        else
        {
            feature->isRec = false;
            return feature;
        }
        feature->isRec = false;
	    return feature;
    }

    seeta::FaceDetector *FD=nullptr;
    seeta::FaceLandmarker *FL=nullptr;
    seeta::FaceRecognizer *FR=nullptr;
};

extern "C"
{
    FaceExt* FaceExt_new(){ return new FaceExt();}
    void FaceExt_init(FaceExt* fe){return fe->init();}
    Feature* FaceExt_getFeature(FaceExt* fe, char* image_path) { return fe->getFeatures(image_path);} 
}

/*
int main()
{
    char *image_path = "./test.jpg";
    Feature* f = getFeatures( image_path);
    std::cout<< f->isR <<std::endl;
    return 0;        
}
*/
