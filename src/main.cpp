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
#include "quantizer.h"

//
// Basile Fraboni 2019
//
// This code aim to reproduce the image quantization from DavidC answer on this thread :
// https://codegolf.stackexchange.com/questions/42217/paint-by-numbers
//
// I need to find or rewrite another quantization algorithm, better and faster than the one from Rosetta Code.
//
// Usage gaussian <image> <blur radius> <number of color>

int main( int argc, char * argv[] )
{
    Image im = read_image(argv[1]);
    write_image(im, "image.png");
    Image im_blur = blur(im, std::atof(argv[2]));
    write_image(im_blur, "blur.png");
    Image im_quant = color_quant(im_blur, std::atoi(argv[3]));
    write_image(im_quant, "quant.png");
    return 0;
}