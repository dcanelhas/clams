#include <iomanip>
#include <string>
#include <sdf_tracker.h>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp> 

int main(int argc, char* argv[])
{  
  boost::filesystem::path p(argv[1]); 

  //Parameters for an SDFtracker object 
  SDF_Parameters myParameters;

  myParameters.interactive_mode = true;
  myParameters.resolution = 0.04;
  myParameters.Dmax = 0.2;
  myParameters.Dmin = -0.1;

  // The sizes can be different from each other 
  // +Y is up +Z is forward.
  myParameters.XSize = 350; 
  myParameters.YSize = 100;
  myParameters.ZSize = 500;

  myParameters.image_width = 640;
  myParameters.image_height = 480;
  myParameters.fx = 525.0;
  myParameters.fy = 525.0;
  myParameters.cx = 320;
  myParameters.cy = 240;

  //Pose Offset as a transformation matrix
  // Eigen::Matrix4d initialTransformation = 
  // Eigen::MatrixXd::Identity(4,4);

  //define translation offsets in x y z if you want to offset the camera relative to the TSDF volume
  // initialTransformation(0,3) = 0.0;  //x 
  // initialTransformation(1,3) = 0.0;  //y
  // initialTransformation(2,3) = static_cast<double>( - 0.5 * myParameters.ZSize * myParameters.resolution) -0.44; //z
  // myParameters.pose_offset = initialTransformation;
  SDFTracker myTracker(myParameters);  
 
  cv::Mat depth(480,640,CV_32FC1);
  Vector6d poseVector;
  Eigen::Matrix4d currentTransformation;
  std::stringstream ss;

  std::string image_list_path = p.string() + "/assoc.txt";     
  if(!boost::filesystem::is_regular_file(image_list_path))        // is p a regular file?   
  {  
    std::cout << image_list_path << " is not a regular file!" << '\n';
    exit(1);  
  }

  std::string image_list_line;
  std::string depth_img_filename;
  double timestamp_depth_image;
  std::ifstream image_list_file(image_list_path.c_str());  

  try
  {
    if (boost::filesystem::exists(p))    // does p actually exist?
    {
      if (boost::filesystem::is_regular_file(p))        // is p a regular file?   
      {  
        std::cout << p << " is a file, not a directory!" << '\n';
        exit(1);
      }  
      else if (boost::filesystem::is_directory(p))      // is p a directory?
      {

        if(!boost::filesystem::exists(p.string() + "/trajectory"))
          boost::filesystem::create_directory(p.string() + "/trajectory");

        std::ofstream output(p.string() + "/trajectory/trajectory.txt");
    
        while(std::getline(image_list_file,image_list_line))
        {

          std::istringstream iss( image_list_line );  
          std::string result;
          std::getline(iss, result,' ');
          if(result.empty()) continue;
        
          ss << result;
          ss >> timestamp_depth_image;
          ss >> depth_img_filename;
          
          ss.clear();
          
          depth_img_filename = p.string() + "/depth/" + std::to_string(timestamp_depth_image) +".png";
        
          std::cout << "processing:  " << depth_img_filename << '\n';
          depth = cv::imread(depth_img_filename.c_str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR );

          if( !depth.data )// Check for invalid input
          {
            std::cerr <<  "Could not open or find the depth image" << std::endl ;
            break;
          }
          depth.convertTo(depth, CV_32FC1); 
          depth/=5000.f;
  
          cv::imshow("debug",depth/10.0f);
          cv::waitKey(3);

          myTracker.UpdateDepth(depth);

          poseVector = myTracker.EstimatePoseFromDepth();
          currentTransformation = myTracker.Twist(poseVector).exp()*myTracker.GetCurrentTransformation();

          myTracker.SetCurrentTransformation(currentTransformation);
          myTracker.FuseDepth();
            
          myTracker.Render();
        
          Eigen::Matrix3d Rot3d;
          Rot3d << currentTransformation(0,0), currentTransformation(0,1), currentTransformation(0,2),
                   currentTransformation(1,0), currentTransformation(1,1), currentTransformation(1,2),  
                   currentTransformation(2,0), currentTransformation(2,1), currentTransformation(2,2);
                   
          Eigen::AngleAxisd aa;  aa = Rot3d;   
        
          Eigen::Quaterniond Q; 
          Q = aa;
          
          if(std::isnan(Q.x())) continue;

          const Eigen::Vector4d translation(  
                      currentTransformation(0,3),
                      currentTransformation(1,3),
                      currentTransformation(2,3),
                      1.f);

          output << std::setprecision(6) << std::fixed << timestamp_depth_image << " " 
          << translation(0) <<" "<< translation(1) <<" "<< translation(2) <<" "
          << Q.x() <<" "<< Q.y() <<" "<< Q.z() <<" "<< Q.w()
          << std::endl;

          timestamp_depth_image += 1.0/30.0; // in absence of real time stamps just increment assuming 30Hz
        }
      }
      else
        std::cout << p << " exists, but is neither a regular file nor a directory\n";
    }
    else
      std::cout << p << " does not exist\n";
  }

  catch (const boost::filesystem::filesystem_error& ex)
  {
    std::cout << ex.what() << '\n';
  }
  return 0;
}
