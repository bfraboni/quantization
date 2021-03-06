
#ifndef _IMAGE_H
#define _IMAGE_H

#include <cmath>
#include <cassert>
#include <vector>
#include <algorithm>
#include <fstream>

#include "color.h"


//! \addtogroup image utilitaires pour manipuler des images
///@{

//! \file
//! manipulation simplifiee d'images

//! representation d'une image.
class Image
{
public:
    std::vector<Color> m_data;
    int m_width;
    int m_height;

    unsigned int offset (const int x, const int y ) const
    {
        int px= x;
        if(px < 0) px= 0;
        if(px > m_width-1) px= m_width-1;
        int py= y;
        if(py < 0) py= 0;
        if(py > m_height-1) py= m_height-1;
        
        assert(py * m_width + px < int(m_data.size()));
        assert(py * m_width + px >= 0);
        
        return py * m_width + px;
    }
    
public:
    Image( ) : m_data(), m_width(0), m_height(0) {}
    Image( const int w, const int h, const Color& color= Black() ) : m_data(w*h, color), m_width(w), m_height(h) {}
    
    /*! renvoie une reference sur la couleur d'un pixel de l'image.
    permet de modifier et/ou de connaitre la couleur d'un pixel :
    \code
    Image image(512, 512);
    
    image(10, 10)= make_red();      // le pixel (10, 10) devient rouge
    image(0, 0)= image(10, 10);     // le pixel (0, 0) recupere la couleur du pixel (10, 10)
    \endcode
    */
    Color& operator() ( const int x, const int y )
    {
        return m_data[offset(x, y)];
    }
    
    //! renvoie la couleur d'un pixel de l'image (image non modifiable).
    Color operator() ( const int x, const int y ) const
    {
        return m_data[offset(x, y)];
    }
    
    //! renvoie la couleur d'un pixel de l'image (image non modifiable).
    Color& operator() ( const int index )
    {
        return m_data[index];
    }

    //! renvoie la couleur d'un pixel de l'image (image non modifiable).
    Color operator() ( const int index ) const
    {
        return m_data[index];
    }
    
    //! renvoie la couleur interpolee a la position (x, y).
    Color sample( const float x, const float y ) const
    {
        // interpolation bilineaire 
        float u= x - std::floor(x);
        float v= y - std::floor(y);
        int ix= x;
        int iy= y;
        return (*this)(ix, iy)    * ((1 - u) * (1 - v))
            + (*this)(ix+1, iy)   * (u       * (1 - v))
            + (*this)(ix, iy+1)   * ((1 - u) * v)
            + (*this)(ix+1, iy+1) * (u       * v);
    }
    
    //! renvoie un pointeur sur le stockage des couleurs des pixels.
    const void * buffer( ) const
    {
        assert(!m_data.empty());
        return &m_data.front();
    }

    //! renvoie un pointeur sur le stockage des couleurs des pixels.
    Color * buffer( )
    {
        assert(!m_data.empty());
        return &m_data.front();
    }
    
    //! renvoie la largeur de l'image.
    int width( ) const { return m_width; }
    //! renvoie la hauteur de l'image.
    int height( ) const { return m_height; }
    //! renvoie le nombre de pixels de l'image.
    std::size_t size( ) const { return m_width * m_height; }
    
    /*! sentinelle pour la gestion d'erreur lors du chargement d'un fichier.
    exemple :
    \code
    Image image= read_image("debug.png");
    if(image == Image::error())
        return "erreur de chargement";
    \endcode
    */
    static Image& error( )
    {
        static Image image;
        return image;
    }

    void SavePPM( const char *aFilename )
    {
        std::ofstream ppm(aFilename);
        ppm << "P3" << std::endl;
        ppm << m_width << " " << m_height << std::endl;
        ppm << "255" << std::endl;

        Color *ptr = &m_data[0];

        for(int y=0; y<m_height; y++)
        {
            for(int x=0; x<m_height; x++)
            {
                ptr = &m_data[x + y * m_width];
                int r = std::min(255, std::max(0, int(ptr->r * 255.f)));
                int g = std::min(255, std::max(0, int(ptr->g * 255.f)));
                int b = std::min(255, std::max(0, int(ptr->b * 255.f)));
                ppm << r << " " << g << " " << b << " ";
            }
            ppm << std::endl;
        }
    }
    
    //! comparaison avec la sentinelle. \code if(image == Image::error()) { ... } \endcode
    bool operator== ( const Image& im ) const
    {
        // renvoie vrai si im ou l'objet est la sentinelle
        return (this == &im);
    }
};

///@}
#endif
