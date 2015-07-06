#ifndef _TIFF_READER__
#define _TIFF_READER__    1
#include <tiffio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>



int find_tile_for_coords(uint32 x, uint32 y,uint32 imageWidth,uint32 imageLength, uint32 tileWidth, uint32 tileLength,uint32 &tile_top_x,uint32 &tile_top_y,int num_tiles);
TIFF* read_tiled_tiff(char* filename);

int get_search_window(uint32 x, uint32 y, uint32 imageWidth, uint32 imageLength, uint32 tileWidth, uint32 tileLength, uint32 &tile_top_x, uint32 &tile_top_y, int size_x, int size_y, int num_tiles, cv::Mat &draw, TIFF* tif);
cv::Mat return_valid_tile(uint32 x, uint32 y, uint32 imageWidth, uint32 imageLength, uint32 tileWidth, uint32 tileLength, uint32 &tile_top_x, uint32 &tile_top_y, int num_tiles,TIFF* tif);
cv::Mat return_grid_tiles(uint32 x, uint32 y, uint32 imageWidth, uint32 imageLength, uint32 tileWidth, uint32 tileLength, uint32 &tile_top_x, uint32 &tile_top_y, int num_tiles, TIFF* tif, uint32 &x_offset, uint32 &y_offset);
cv::Mat return_grid_tiles_multiplier_offset(uint32 x, uint32 y, uint32 imageWidth, uint32 imageLength, uint32 tileWidth, uint32 tileLength, uint32 &tile_top_x, uint32 &tile_top_y, int num_tiles, TIFF* tif, uint32 multiplier, cv::Mat center);

#endif
