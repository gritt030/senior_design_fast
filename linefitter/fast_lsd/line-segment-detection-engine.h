/** @file line-segment-detection-engine.h
 *  @brief The line-segment-detection-engine.h file.
 *
 *  This file contains the definition of
 *  the line-segment-detection-engine class.
 *  Created on: Feb 17, 2014
 *  Author: John O'Leary
 *  email: olear121@umn.edu
 *
 *  Based on code by: Ahmed Medhat
 *  email: medhat@cs.umn.edu
 */
#ifndef LINE_SEGMENT_DETECTION_ENGINE_H_
#define LINE_SEGMENT_DETECTION_ENGINE_H_
#include <demo-parameters/parameters.h>

#include <vector>   /// To use: std::vector
#include <engine/engine.h>  /// To use: MARS::Engine
#include <measurement/image-measurement.h>  /// To use: MARS::Image
#include <measurement/line-measurement.h>  /// To use: MARS::LineMeasurement
#include <core/matrix2d.h>

#include <Eigen/Core>  // To use Eigen::Vector2d

#include <imgproc/pyramiddown.h>
#include <imgproc/sobelgradient.h>
#include <imgproc/lsd.h>

#include <stdio.h>
#include <stdlib.h>
#include <imgproc/LSDfast/sobelgradientv2.h> 
#include <imgproc/LSDfast/regionDataStructure.h>

#define __USE_FAST_LSD__ 0
#define __TIME_INDIVIDUALLY__ 0

extern std::map<std::string, SimpleTimer > function_timers;  // To get timing info

/** @namespace Engines
 * The Engines namespace.
 */
namespace Engines {
/** @class LineSegmentDetectionEngine
 *  @brief The LineSegmentDetectionEngine class.
 *  The line segment detection engine, fills the frame with line measurements.
 *  @todo Possibly create an LSD class to use as an extractor. This could hold parameteres needs by LSD.
 */
class LineSegmentDetectionEngine : public MARS::Engine {
 public:
    region myRegion;
    dataStructure sobel2;
  /// Default constructor
  LineSegmentDetectionEngine() {
    engine_type_ = MARS::LINE_SEGMENT_DETECTION_ENGINE;
    this->Init();
    image_ = 0;
    unique_id_ = 0;
    min_line_length_ = 0;
    profiler_ = 0;

    #if __USE_FAST_LSD__
      myRegion.allocate(320 * 240); 
      sobel2.allocate(640 * 480);
    #endif
  }
  /// Construct the engine and initialize from Receiver.
  LineSegmentDetectionEngine(MARS::Receiver& receiver) {
    engine_type_ = MARS::LINE_SEGMENT_DETECTION_ENGINE;
    this->Init(receiver);
    image_ = 0;
    unique_id_ = 0;
    min_line_length_ = 0;
    profiler_ = 0;

    #if __USE_FAST_LSD__
      myRegion.allocate(320 * 240);
      sobel2.allocate(640 * 480);
    #endif
  }
  /// Destructor
  virtual ~LineSegmentDetectionEngine() {
    #if __USE_FAST_LSD__
      myRegion.deallocate();
      sobel2.deallocate();
    #endif
  }
  /// Setters and Getters
  inline void setImage(MARS::Image* _image) {
    image_ = _image;
  }
  inline MARS::Image* getImage() {
    return image_;
  }
  inline void setScaledImage(MARS::Image* _image) {
    scaled_image_ = _image;
  }
  inline MARS::Image* getScaledImage() {
    return scaled_image_;
  }
  inline void setScaledSobel(EyeMARS::Matrix2d<short>* _sobel) {
    scaled_sobel_ = _sobel;
  }
  inline EyeMARS::Matrix2d<short>* getScaledSobel() {
    return scaled_sobel_;
  }
  inline std::vector<MARS::LineMeasurement*>* getLines() {
    return &lines_;
  }
  inline void setProfiler(Profiler* _prof) {
    profiler_ = _prof;
  }
  inline Profiler* profiler() {
    return profiler_;
  }
  inline void setMinLineLength(double _min_line_length) {
    min_line_length_ = _min_line_length;
  }
  inline double getMinLineLength() {
    return min_line_length_;
  }
  /// Operation Functions
  /// @fn Init, Initialize the engine.
  void Init();
  /// @fn Init, Initialize the engine from receiver.
  inline void Init(MARS::Receiver& receiver) {
    /// Initialize the engine
    /// 1. Create the commands based on received types
    MARS::Engine::Init(receiver);
    /// TODO [LineSegmentDetectionEngine]: 2. Initialize the engine data
  }

  /// @fn Run, Run the main functionality of the engine.
  void Run() {
    MARS::Engine::Run();
    int X = image_->width();
    int Y = image_->height();

    // Pyramid Down
    if (profiler_)  {
      profiler_->Start("LSD_PyramidDown");
    }
    EyeMARS::Image my_image(Y, X, image_->data());
    EyeMARS::Image rescaled_image(Y/2, X/2);
    if (!scaled_image_) {
      EyeMARS::PyramidDown(my_image, rescaled_image);
    } else {
      rescaled_image.Set(scaled_image_->data(), Y/2, X/2);
    }
    if (profiler_) {
      profiler_->Stop("LSD_PyramidDown");
    }
    

    // dataStructure sobel2;
    // sobel2.allocate( X * Y);

    X /= 2; //height
    Y /= 2; //widht

    #if __USE_FAST_LSD__

      if (profiler_) {
        profiler_->Start("LSD_gradientANDlines");
      }

      short *sd2 = (short*)calloc(X*2*Y, sizeof(short));

      float threshold = 1/(5.226251859505506*2);
      
      lines_.clear();

///////HENRY HERE/////////
      EyeMARS::lsd2(rescaled_image.data(), (short*)(sd2), threshold, lines_, sobel2, myRegion, min_line_length_, profiler_);
      myRegion.reset();

      if(sd2)
        free(sd2);

      if (profiler_) {
        profiler_->Stop("LSD_gradientANDlines");
      }

    #else

      if (profiler_) {
        profiler_->Start("LSD_gradientANDlines");
      }

      //---------------------------------------------------//
      //-----------------------Sobel-----------------------//
      #if __TIME_INDIVIDUALLY__
        if (profiler_) {
          profiler_->Start("LSD_gradient");
        }
      #endif
      EyeMARS::Matrix2d<short> sobel(X, 2*Y, (short*)NULL);
      short* sd = 0;
      if (!scaled_sobel_) {
        sd = (short*)malloc(sizeof(short)*rescaled_image.height()*2*rescaled_image.width());
        sobel.Set(sd,rescaled_image.height(),
                            2*rescaled_image.width());
        EyeMARS::SobelGradient(rescaled_image, sobel);
      } else {
        sobel.Set(scaled_sobel_->data(), scaled_sobel_->height(), scaled_sobel_->width());
      }
      #if __TIME_INDIVIDUALLY__
        if (profiler_) {
          profiler_->Stop("LSD_gradient");
        }
      #endif
      //---------------------------------------------------//
      //---------------------------------------------------//


      //---------------------------------------------------//
      //-------------------------LSD-----------------------//
      #if __TIME_INDIVIDUALLY__
        if (profiler_) {
          profiler_->Start("LSD_lines");
        }
      #endif
      float* image = (float *) malloc(X * Y * sizeof(float));
      for (int i = 0; i < X*Y; ++i)
        image[i] = (float) rescaled_image.data()[i];
      int n = 0;
      int regX, regY;
      double sigma_coef = 0.6;
      double quant = 10.0;
      double ang_th = 22.5;
      double log_eps = 0.00;
      double density_th = 0.7;
      double scale = 0.5;  // Would be 0.5 if we wanted output of a quater size.
                            // To get it back to full size image, scale by .25.
      int n_bins = 512;
      double* segs = LineSegmentDetection_gradient( &n, image, sobel, X, Y,
                                   scale,
                                   sigma_coef,
                                   quant,
                                   ang_th,
                                   log_eps,
                                   density_th,
                                   n_bins,
                                   NULL,
                                   &regX, &regY, profiler_);

      // Store lines as LineMeasurement
      lines_.clear();
      for (int i = 0; i < n; ++i) {
        Eigen::Vector2d temp1(2*segs[7*i]/2, 2*segs[7*i + 1]/2);
        Eigen::Vector2d temp2(2*segs[7*i + 2]/2, 2*segs[7*i + 3]/2);
        double line_length = (temp1 - temp2).norm();
        if (line_length >= min_line_length_) {
          MARS::LineMeasurement* measurement = new MARS::LineMeasurement(temp1, temp2);
          measurement->set_id(unique_id_++);
          lines_.push_back(measurement);
        }
      }
      //---------------------------------------------------//
      //---------------------------------------------------//

      // Free data
      free(image);
      if (sd)
        free(sd);
      
      free(segs);


      #if __TIME_INDIVIDUALLY__
        if (profiler_) {
          profiler_->Stop("LSD_lines");
        }
      #endif

      if (profiler_) {
        profiler_->Stop("LSD_gradientANDlines");
      }
    #endif

  }
  void Clear() {
    image_ = 0;
    scaled_image_ = 0;
    scaled_sobel_ = 0;
  }
 protected:
  /// Protected data members.
 private:
  /// Private data members.
  int unique_id_;
  MARS::Image* image_;
  MARS::Image* scaled_image_;
  EyeMARS::Matrix2d<short>* scaled_sobel_;
  std::vector<MARS::LineMeasurement*> lines_;
  Profiler* profiler_;
  double min_line_length_;
};
/** End of class: LineSegmentDetectionEngine */
} /** End of namespace: Engines */
#endif /** End of file: LINE_SEGMENT_DETECTION_ENGINE_H_ */


// NOTES FOR ELLIOT
// The key for the timers is the function name