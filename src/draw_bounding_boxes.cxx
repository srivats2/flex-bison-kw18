#include <iostream>
#include <vector>
#include "RPcustomKW18parser.hxx"
#include "filelisting_RP.hpp"
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <ctime>

//bison parser stuff

extern std::vector<int> my_kw18_data_x;
extern std::vector<int> my_kw18_data_y;
extern std::vector<int> my_kw18_data_frno;
extern std::vector<int> my_kw18_data_oid;

extern "C" int kw18_parse(char *);
//bison
struct unique_colors{
    std::vector<int> unique_ids;
    std::vector<cv::Scalar> colors;
};





//get unique color for unique ids
int get_unique_color_ids(std::vector<int> ids, struct unique_colors* UStruct ){
    std::vector<cv::Scalar> color_mat;

    std::vector<int> unique_ids = ids;

    std::vector<int>::iterator it;

   it = std::unique(unique_ids.begin(),unique_ids.end());

   //get unique ids
   unique_ids.resize(std::distance(unique_ids.begin(),it));

   UStruct->unique_ids = unique_ids;
   cv::RNG rng(12345);
   for(int i = 0 ; i < unique_ids.size();i++){
        cv::Scalar random_col = cv::Scalar((rng.uniform(0,255)+100)%255,(rng.uniform(0,255)+128)%255,(rng.uniform(0,255)+255)%255);
       color_mat.push_back(random_col);

   }

   UStruct->colors=color_mat;
   //for(it = unique_ids.begin();it!=unique_ids.end();++it){
    //   std::cout<<*it<<std::endl;
   //}

   return 0;

}

//get indices of the frames that match and return the centroids to draw

int get_indices_from_frames_vector(std::vector<int> frnum,int frno,std::vector<int> &matches){

//std::vector<int> matches;
//std::vector<int>::iterator p;
//while(true){
//p = std::find_if(frnum.begin(),frnum.end(),3);
// if(p==frnum.end())
//     break;
// matches.push_back();
//}
    for(int i = 0 ; i<frnum.size();i++){
        if(frnum.at(i)==frno){
            matches.push_back(i);
        }
    }

return 0;
}

std::string convertInt(int number)
{
    if (number == 0)
        return "0";
    std::string temp="";
    std::string returnvalue="";
    while (number>0)
    {
        temp+=number%10+48;
        number/=10;
    }
    for (int i=0;i<temp.length();i++)
        returnvalue+=temp[temp.length()-i-1];
    return returnvalue;
}


std::string ZeroPadNumber(int num, int total_digits)
{
    std::stringstream ss;

    // the number is converted to string with the help of stringstream
    ss << num;
    std::string ret;
    ss >> ret;

    // Append zero chars
    int str_length = ret.length();
    for (int i = 0; i < total_digits - str_length; i++)
        ret = "0" + ret;
    return ret;
}

int writeout_images(char *output_folder,cv::Mat &drawing_loft,int i){
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    try {
        imwrite(output_folder+ZeroPadNumber(i,7)+".png", drawing_loft, compression_params);
    }
    catch (std::runtime_error& ex) {
        fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
        return 1;
    }
}

int main(int argc, char *argv[]){

    std::string frnum;
    std::ostringstream convert;
unique_colors* UStruct = new unique_colors();

unsigned int size_x = 14;
unsigned int size_y = 26;
char inputpath_frames[] = "/home/srivats/code/EOIR_LOFT_CSURF_V2/data/v2/";
//char kw18_filename[] = "/home/srivats/code/loft_matlab/LOFT_tracker/trunk/LoFT-V-1.4/loft-fpss-test-linear/0003.kw18";
//char kw18_filename[] = "/home/srivats/code/loft_matlab/LOFT_tracker/trunk/LoFT-V-1.4/loft-fpss-test-ftsg-rgb/0002.kw18";



//char inputpath_frames[] = "/home/srivats/Meru/usr/mvl4/Images/ARL/FPSS/ARLFPSS/dataset_1/rf20031216nc1/";
//char inputpath_frames[] = "/home/srivats/Meru/usr/mvl2/rvpnc4/FTSG_FPSS_rf20031216nc1/mask/";
//char kw18_filename[] = "/home/srivats/Meru/usr/mvl2/rvpnc4/FTSG_FPSS_rf20031216nc1/T_1segments_list.kw18";

//char kw18_filename[] = "MU_ARGUS_GT_V2Registered_no_neg_manually_corrected_10_06_2011_freq_1.kw18";
//char wildcard[] = "jpg";

char wildcard[] = "tiff";

//char output_folder[]="./output-loft-ftsg-fusion-cvpr/";
//char output_folder[] = "./output-filiz-tracking-results-masks/";
char output_folder[] = "./output-filiz-tracking-results/";
//char output_folder[]="./output-loft-ftsg-rgb-fusion-cvpr/";


filenames file_list;
file_list = file_listing(inputpath_frames,wildcard,0);
if(!file_list.size){
    std::cout<<"Not a valid directory..check path"<<std::endl;
    //return -1;
}
//call the customized bison parser!
std::clock_t start;
start = std::clock();

    kw18_parse(argv[1]);

     std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

if(0){
    //kw18s should be written out '0' based in the original dataset..not in the tracked..sigh..but what to do
    //std::transform(my_kw18_data_frno.begin(), my_kw18_data_frno.end(), my_kw18_data_frno.begin(),bind2nd(std::plus<double>(), 3.0));

    std::cout<<"# of lines : "<<my_kw18_data_frno.size()<<std::endl;


    get_unique_color_ids(my_kw18_data_oid,UStruct );

    for(unsigned int i =0 ; i < file_list.used ; ++i ){
        std::vector<int> matches;
        get_indices_from_frames_vector(my_kw18_data_frno,i,matches);



        //read the image in
        cv::Mat Image;
        Image = cv::imread(file_list.names[i]);
        if(! Image.data )                              // Check for invalid input
        {
            std::cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }
        cv::Mat drawing_loft = Image.clone();


        for(int j = 0 ; j < matches.size();j++){
            //std::cout<<matches.at(j)<<"\t";
            //cv::Scalar color = cv::Scalar(0, 2*(j), 255);
            cv::Scalar color;
            for(int k=0;k<UStruct->unique_ids.size();k++){
                if(UStruct->unique_ids.at(k)==my_kw18_data_oid.at(matches.at(j))){
                    color = UStruct->colors.at(k);
                    cv::circle(drawing_loft,cvPoint(my_kw18_data_x.at(matches.at(j)),my_kw18_data_y.at(matches.at(j))),3,color,2);
                }else{
                    color = cv::Scalar(0,0,0);
                    //cv::circle(drawing_loft,cvPoint(my_kw18_data_x.at(matches.at(j)),my_kw18_data_y.at(matches.at(j))),3,color,2);
                }

            }


        }
         cv::resize(drawing_loft,drawing_loft,cv::Size(1920,1080));

        cv::namedWindow( "MU-LoFT-RP-V-2.0", CV_WINDOW_AUTOSIZE );
        cv::imshow( "MU-LoFT-RP-V-2.0", drawing_loft );
        cv::waitKey(30);
//writeout_images(output_folder,drawing_loft,i);


/*
        cv::Mat Image;
        //std::cout<<"X :"<<my_kw18_data_x.at(i)<<" Y : "<<my_kw18_data_y.at(i)<<std::endl;
        convert.str("");
        //convert.clear();
        convert << i;
        frnum = convert.str();
       if(i<my_kw18_data_frno.size()){
        std::cout<<"Reading & Writting Fr # "<<my_kw18_data_frno.at(i)<<std::endl;
        Image = cv::imread(file_list.names[(int)my_kw18_data_frno.at(i)]);
       }else{
           std::cout<<"Reading & Writting Fr # "<<i<<std::endl;
           Image = cv::imread(file_list.names[i]);
       }
        if(! Image.data )                              // Check for invalid input
        {
            std::cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }
      cv::Mat drawing_loft = Image.clone();
   if(i<my_kw18_data_frno.size()){
      cv::Scalar color = cv::Scalar(0, 0, 255);
      //cv::rectangle( drawing_loft, cvPoint((my_kw18_data_x.at(i)-(size_x/2)),(my_kw18_data_y.at(i)-(size_y/2))),cvPoint((my_kw18_data_x.at(i)+(size_x/2)),(my_kw18_data_y.at(i)+(size_y/2))), color, 2, 8, 0 );
      cv::circle(drawing_loft,cvPoint(my_kw18_data_x.at(i),my_kw18_data_y.at(i)),3,color,2);
        }
      //cv::putText(drawing_loft, frnum, cvPoint(30,30), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,0,255), 1, CV_AA);
      cv::namedWindow( "MU-LoFT-RP-V-2.0", CV_WINDOW_AUTOSIZE );
      cv::imshow( "MU-LoFT-RP-V-2.0", drawing_loft );
      cv::waitKey(30);

      std::vector<int> compression_params;
      compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
      compression_params.push_back(9);

      try {
          imwrite(output_folder+ZeroPadNumber(i,7)+".png", drawing_loft, compression_params);
      }
      catch (std::runtime_error& ex) {
          fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
          return 1;
      }

*/
    }
}

}
