#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <random>
#include <cstdlib>
#include <cassert>
#include <functional>

#include "image_io.h"
#include "fast_blur.h"
#include "kmeans.h"

//
// Basile Fraboni 2019
//
// This code aim to reproduce the image quantization from DavidC answer on this thread :
// https://codegolf.stackexchange.com/questions/42217/paint-by-numbers
//
// Usage gaussian <image> <blur radius> <number of color>

int main( int argc, char * argv[] )
{
    Image im = read_image(argv[1]);
    // write_image(im, "image.png");
    
    bool bw = argc > 4 ? (bool)std::atoi(argv[4]) : false;
    if( bw )
    {
        for(int i = 0; i < im.width(); ++i)
        for(int j = 0; j < im.height(); ++j)
        {
            im(i,j) = Color(im(i,j).luminance());
        }
    }

    float radius = std::atof(argv[2]);
    Image im_blur = blur(im, radius);
    // write_image(im_blur, "blur.png");

    int k = std::atoi(argv[3]);
    KMeans<Color> kmeans(im_blur.m_data, k);
    Image im_quant_kmeans(im.width(), im.height(), Black());
    kmeans.replace(im_quant_kmeans.buffer());

    std::stringstream ss;
    ss << "radius" << int(radius) << "colors" << k << ".png";
    // write_image(im_quant_kmeans, "quant_kmeans.png");
    write_image(im_quant_kmeans, ss.str().c_str());

    return 0;

}