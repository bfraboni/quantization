#ifndef BLUR_H
#define BLUR_H

#include "image.h"

void std_to_box (float boxes[], float sigma, int n)  
{
    // ideal filter width
    float wi = std::sqrt((12*sigma*sigma/n)+1); 
    int wl = std::floor(wi);  
    if(wl%2==0) wl--;
    int wu = wl+2;
                
    float mi = (12*sigma*sigma - n*wl*wl - 4*n*wl - 3*n)/(-4*wl - 4);
    int m = std::round(mi);
                
    for(int i=0; i<n; i++) 
        boxes[i] = i < m ? wl : wu;

    for(int i=0; i<n; i++)
        boxes[i] = (boxes[i] - 1) * 0.5f;
}

void horizontal_blur (Color * in, Color * out, int w, int h, float r) 
{
    float iarr = 1 / (r+r+1);
    for(int i=0; i<h; i++) 
    {
        int ti = i*w, li = ti, ri = ti+r;
        Color fv = in[ti], lv = in[ti+w-1], val = (r+1)*fv;

        for(int j=0; j<r; j++) val += in[ti+j];
        for(int j=0  ; j<=r ; j++) { val = val + in[ri++] - fv      ; out[ti++] = val*iarr; }
        for(int j=r+1; j<w-r; j++) { val = val + in[ri++] - in[li++]; out[ti++] = val*iarr; }
        for(int j=w-r; j<w  ; j++) { val = val + lv       - in[li++]; out[ti++] = val*iarr; }
    }
}

void total_blur (Color * in, Color * out, int w, int h, float r) 
{
    float iarr = 1 / (r+r+1);
    for(int i=0; i<w; i++) 
    {
        int ti = i, li = ti, ri = ti+r*w;
        Color fv = in[ti], lv = in[ti+w*(h-1)], val = (r+1)*fv;
        for(int j=0; j<r; j++) val += in[ti+j*w];
        for(int j=0  ; j<=r ; j++) { val = val + in[ri] - fv    ; out[ti] = val*iarr; ri+=w; ti+=w; }
        for(int j=r+1; j<h-r; j++) { val = val + in[ri] - in[li]; out[ti] = val*iarr; li+=w; ri+=w; ti+=w; }
        for(int j=h-r; j<h  ; j++) { val = val + lv     - in[li]; out[ti] = val*iarr; li+=w; ti+=w; }
    }
}

void box_blur (Color * in, Color * out, int w, int h, float r) 
{
    horizontal_blur (in, out, w, h, r);
    std::swap(in, out);
    total_blur (in, out, w, h, r);
    std::swap(in, out);
}

void fast_gaussian_blur (Color * in, Color * out, int w, int h, float sigma) 
{
    // sigma conversion to box dimensions
    float boxes[3];
    std_to_box(boxes, sigma, 3);
    for(int i = 0; i < 3; ++i)
        box_blur (in, out, w, h, boxes[i]);
    std::swap(in, out);
}

Image blur(const Image& source, float sigma)
{
    Image in = source, out(source.width(), source.height());
    fast_gaussian_blur(in.buffer(), out.buffer(), source.width(), source.height(), sigma);

    #pragma omp parallel for
    for(int i = 0; i < (int)out.size(); ++i)
        out(i) = clamp(out(i));

    return out;
}

#endif