
#include "TiffReader.hpp"

TIFF* read_tiled_tiff(char* filename){

TIFF* tif = TIFFOpen(filename, "r");

return tif;

}



int find_tile_for_coords(uint32 x, uint32 y,uint32 imageWidth,uint32 imageLength, uint32 tileWidth, uint32 tileLength, uint32 &tile_top_x,uint32 &tile_top_y,int num_tiles){
//this function will calculate the top left x and y coord for a corresponding absolute image coordinate given the info about the tiles being read in
    //throw a runtime error if the coords are out of bounds...cannot proceed
    if((x > imageWidth || y > imageLength)){
       // std::cout<<"X : "<<x<<" "<<"Y : "<<y<<std::endl;
       //throw std::runtime_error("Invalid coordinates!...out of bounds");
        return -1;
    }
    //simply divide out and find the integer value

     tile_top_x = x/tileWidth;
     tile_top_y = y/tileLength;

     tile_top_x *= tileWidth;
     tile_top_y *= tileLength;



//std::cout<<"Top left x and y as computed : "<<tile_top_x<<" x "<<tile_top_y<<std::endl;

}

int get_search_window(uint32 x, uint32 y,uint32 imageWidth,uint32 imageLength, uint32 tileWidth, uint32 tileLength, uint32 &tile_top_x, uint32 &tile_top_y,int size_x,int size_y,int num_tiles,cv::Mat& draw,TIFF* tif){
   //This function will extract the tiles and put the requested coordinate in the center of the cropped image..it should combine multiple tiles and handle border effects properly
    //x,y -> coordinates will always be in global but the top_left_x and top_left_y global coords need to be supplied back in order to draw in the proper uncropped coordinate system
uint32 temp_top_x,temp_top_y,x_offset,y_offset;
uint32 ULx1,ULy1;
temp_top_x=tile_top_x;
temp_top_y=tile_top_y;
//int size_x=500;int size_y=500; //size that we need around the x,y, center

int width = size_x;
int height = size_y;

//find the tile top left x and y in global coordinates
 //uint32 tile_top_x;
 //uint32 tile_top_y;
//determine how many tiles one would need to read in for the desired size in order to properly center the coord

// # of * denotes the index of the tile given a coordinate in the middle ...center tile is enclosed between ()
// |--|--|--|---------------------|
// |-*-|-**-|-***-|---------------------|
// |-****-|-(*****)-|-******-|---------------------|
// |-*******-|-********-|-********-|---------------------|
// |--|--|--|---------------------|
// |--|--|--|---------------------|

//max # of tiles in x and y required to fit in the req. region

int num_tiles_x = std::ceil(size_x/tileWidth);
int num_tiles_y = std::ceil(size_y/tileLength);


std::cout<<"Number of tiles required in the x dim : "<<num_tiles_x<<std::endl;
std::cout<<"Number of tiles required in the y dim : "<<num_tiles_y<<std::endl;




//this is the top left of the center tile...
// if(x<std::ceil(size_x/2)){
//     x=0;
// }else{
//     x = x-std::ceil(size_x/2);
// }

// if(y<std::ceil(size_y/2)){
//     y=0;
// }else{
//     y= y-std::ceil(size_y/2);
// }

// if(y>imageLength){
//     y= imageLength;
// }
// if(x>imageWidth){
//     x=imageWidth;
// }

 //read in 9 tiles and then concat them



cv::Mat center = return_grid_tiles(x,y,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif,x_offset,y_offset);
cv::Mat final;
final = center;
for(int j = 2; j<std::max(num_tiles_x,num_tiles_y);j++){
final =return_grid_tiles_multiplier_offset(x, y,imageWidth, imageLength,  tileWidth,  tileLength,tile_top_x, tile_top_y, num_tiles,tif,j,final);
}

draw = final.clone();



ULx1 = x-tile_top_x;
ULy1 = y-tile_top_y;



std::cout<<"ULx1: "<<ULx1<<" "<<"ULy1: "<<ULy1<<std::endl;

//cv::Point px = cv::Point(x-tile_top_x+width/2,y-tile_top_y+height/2);
//cv::circle(draw,px,1,cv::Scalar(255,0,0),2);
cv::Rect ROI(ULx1,ULy1,width,height);
draw = draw(ROI);

//cv::namedWindow( "Tiled", CV_WINDOW_AUTOSIZE );
//cv::imshow( "Tiled", draw );
//cv::waitKey(0);

//cv::namedWindow( "Centered around coord", CV_WINDOW_AUTOSIZE );
//cv::imshow( "Centered around coord", draw );
//cv::waitKey(0);
//cv::Mat final =return_grid_tiles_multiplier_offset(x, y,imageWidth, imageLength,  tileWidth,  tileLength,tile_top_x, tile_top_y, num_tiles,tif,3,center);

//final =return_grid_tiles_multiplier_offset(x, y,imageWidth, imageLength,  tileWidth,  tileLength,tile_top_x, tile_top_y, num_tiles,tif,4,final);
//final =return_grid_tiles_multiplier_offset(x, y,imageWidth, imageLength,  tileWidth,  tileLength,tile_top_x, tile_top_y, num_tiles,tif,5,final);





//get the tile to the bottom left of the center
//find_tile_for_coords(tile_top_x-tileWidth,tile_top_y,imageWidth,imageLength,tileWidth,tileLength,tile_top_x2,tile_top_y2,num_tiles);




}

cv::Mat return_valid_tile(uint32 x, uint32 y, uint32 imageWidth, uint32 imageLength, uint32 tileWidth, uint32 tileLength, uint32 &tile_top_x, uint32 &tile_top_y, int num_tiles,TIFF* tif){

    //get the first tile which contains the requested coordinate
    int success;
    cv::Mat buffer;

    tdata_t buf; //buffer
    buf = _TIFFmalloc(TIFFTileSize(tif)); //allocate size for the buffer

success=find_tile_for_coords(x,y,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles);
if(success==-1){
buffer = cv::Mat::zeros(tileWidth,tileLength,CV_8UC3);
return buffer;
}else{
TIFFReadTile(tif,buf,tile_top_x,tile_top_y,0,0);
cv::Mat Image(tileWidth,tileLength,CV_8UC3,static_cast <uchar*>(buf));
cv::cvtColor(Image,Image,CV_RGB2BGR);
return Image;
}

}

cv::Mat return_grid_tiles(uint32 x, uint32 y,uint32 imageWidth,uint32 imageLength, uint32 tileWidth, uint32 tileLength, uint32 &tile_top_x, uint32 &tile_top_y,int num_tiles,TIFF* tif,uint32 &x_offset,uint32 &y_offset){

   uint32 x_diff = 1*tileWidth;
   uint32 y_diff = 1*tileLength;



    //top center
    cv::Mat top_center = return_valid_tile(x,y-y_diff,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    //top right
    cv::Mat top_right = return_valid_tile(x+x_diff,y-y_diff,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    //left
    cv::Mat left = return_valid_tile(x-x_diff,y,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    //right
    cv::Mat right = return_valid_tile(x+x_diff,y,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    //bottom left
    cv::Mat bottom_left =return_valid_tile(x-x_diff,y+y_diff,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    //bottom center
    cv::Mat bottom_center =return_valid_tile(x,y+y_diff,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    //bottom right
    cv::Mat bottom_right=return_valid_tile(x+x_diff,y+y_diff,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    //center tile
    cv::Mat center = return_valid_tile(x,y,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    //compute offsets now
    x_offset = x-tile_top_x;
    y_offset = y-tile_top_y;


    //top left
    cv::Mat top_left = return_valid_tile(x-x_diff,y-y_diff,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);




    cv::Mat top,middle,bottom,final;
    cv::hconcat(top_left,top_center,top);
    cv::hconcat(top,top_right,top);

    cv::hconcat(left,center,middle);
    cv::hconcat(middle,right,middle);

    cv::hconcat(bottom_left,bottom_center,bottom);
    cv::hconcat(bottom,bottom_right,bottom);

    cv::vconcat(top,middle,final);
    cv::vconcat(final,bottom,final);
    return final;

}
cv::Mat return_grid_tiles_multiplier_offset(uint32 x, uint32 y,uint32 imageWidth,uint32 imageLength, uint32 tileWidth, uint32 tileLength, uint32 &tile_top_x, uint32 &tile_top_y,int num_tiles,TIFF* tif,uint32 multiplier,cv::Mat center){
//this function gets the padded multiplier around an already existing grid if needed..this is in case the requested scene size is larger...general case with a set of 9 tiles at the center and computes the rest by using the offset trick
//you can only call this function in successive multipliers...for e.g. 2,3,4,5 and if you skip a few numbers in between then the padding will not be correct
    //is there a better way to do this? probably....will need to think about that later..
    uint32 x_diff = multiplier*tileWidth;
    uint32 y_diff = multiplier*tileLength;

    uint32 grid_top_left_x,grid_top_left_y;


    cv::Mat top,left,right,bottom,temp,final;
    //get all the tiles on the top..compute the top left
    top = return_valid_tile(x-x_diff,y-y_diff,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    grid_top_left_x=tile_top_x;
    grid_top_left_y=tile_top_y;

    //number of tiles fetched would simply be a formula ...multiplier*2+1 should give the number of tiles to read
    for(int i = 1;i<multiplier*2+1;i++){
        temp=return_valid_tile(tile_top_x+tileWidth,tile_top_y,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
        cv::hconcat(top,temp,top);
    }

    //get all the tiles on the left
    left =return_valid_tile(x-x_diff,(y-((multiplier-1)*tileLength)),imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    for(int i = 1;i<multiplier*2-1;i++){
        temp=return_valid_tile(tile_top_x,tile_top_y+tileLength,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
        cv::vconcat(left,temp,left);
    }


    //get all the tiles on the right
    right = return_valid_tile(x+x_diff,(y-((multiplier-1)*tileLength)),imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    for(int i = 1;i<multiplier*2-1;i++){
        temp=return_valid_tile(tile_top_x,tile_top_y+tileLength,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
        cv::vconcat(right,temp,right);
    }

    //get all the tiles in the bottom row
    bottom = return_valid_tile(x-x_diff,y+y_diff,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
    for(int i = 1;i<multiplier*2+1;i++){
        temp=return_valid_tile(tile_top_x+tileWidth,tile_top_y,imageWidth,imageLength,tileWidth,tileLength,tile_top_x,tile_top_y,num_tiles,tif);
        cv::hconcat(bottom,temp,bottom);
    }

    //put everything together
    final=center;
    cv::hconcat(left,final,final);



    cv::hconcat(final,right,final);


    cv::vconcat(top,final,final);

    cv::vconcat(final,bottom,final);


    tile_top_x = grid_top_left_x;
    tile_top_y = grid_top_left_y;
    return final;




}

