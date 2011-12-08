#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int punkt_iteration(double cx, double cy, double max_betrag_quadrat, int max_iter)
{
    double betrag_quadrat = 0;
    int iter = 0;
    double x = 0;
    double y = 0;
    double xt;
    double yt;
    
    while (betrag_quadrat <= max_betrag_quadrat && iter < max_iter)
    {
        xt = x * x - y * y + cx;
        yt = 2 * x * y + cy;
        x = xt;
        y = yt;
        iter = iter + 1;
        betrag_quadrat = x * x + y * y;
    }
    
    return iter;
}

int waehle_farbe(char* farbe, int iterations_wert, int max_iteration)
{
    
    if (iterations_wert >= max_iteration/2)
    {
        farbe[0] = 0;
        farbe[1] = 0;
        farbe[2] = 0;
    }else
    {
        farbe[0] = 255*(max_iteration/iterations_wert)*1;
        farbe[1] = 255*(max_iteration/iterations_wert)*1;
        farbe[2] = 255*(max_iteration/iterations_wert)*1;
    }
    
    return(1);
}

int setpix(char* rgb, int pix_x, int pix_y, int max_x, char* farbe)
{
    int ipos = 3*(pix_y*max_x+pix_x);
    rgb[ipos]   = farbe[0];
    rgb[ipos+1] = farbe[1];
    rgb[ipos+2] = farbe[2];
    
    return(1);
}

int main(int argc, char* argv[])
{
    double cx, cy;
    int pix_x, pix_y;
    int iterations_wert;
    char farbe[3];
    
    int max_x = 1600;
    int max_y = 1400;
    
    double min_cx = -2.2;
    double min_cy = -1.4;
    
    double punkt_abstand = 0.002;
    
    double tar_x = 0;
    double tar_y = 0;
    
    double max_betrag_quadrat = 1000.0;
    
    int max_iteration = 10000;
    
    char rgb[3*max_x*max_y];
    
    
    for ( pix_x = 0 ; pix_x < max_x ; pix_x++ )
    {
        cx = (min_cx + tar_x) + pix_x * punkt_abstand;
        
        for ( pix_y = 0 ; pix_y < max_y ; pix_y++ )
        {
            cy = (min_cy + tar_y) + pix_y * punkt_abstand;
            
            iterations_wert = punkt_iteration( cx, cy, max_betrag_quadrat, max_iteration);
            
            waehle_farbe(farbe, iterations_wert, max_iteration);
            
            setpix(rgb, pix_x, pix_y, max_x, farbe);
        }
    }
    
    write_bmp("apfel.bmp", max_x, max_y, rgb);
    
    return EXIT_SUCCESS;
}
