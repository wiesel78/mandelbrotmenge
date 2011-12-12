#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "bmp.h"

// Aufgabe 6.2 Mandelbrotmenge
// Philip Ullrich : 753088

int punkt_iteration(double cx, double cy, double max_betrag_quadrat, int max_iter)
{
    double betrag_quadrat = 0;
    int iter = 0;
    double x = 0;
    double y = 0;
    double xt;
    double yt;
    
    // ziemlich genau das was in pseudocode auf wikipedia steht
    // http://de.wikipedia.org/wiki/Mandelbrot-Menge#Programmbeispiel
    
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
    
    if (iterations_wert >= max_iteration)
    {
        farbe[0] = 0;
        farbe[1] = 0;
        farbe[2] = 0;
    }else
    {
        farbe[0] = 255*(max_iteration/iterations_wert);
        farbe[1] = 255*(max_iteration/iterations_wert);
        farbe[2] = 255*(max_iteration/iterations_wert);
        
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
    int pix_x, pix_y, i;
    int iterations_wert;
    int pixiter[3];
    char farbe[3];
    int size, rank;
    int max_x = 1600;               //Bildgroesse x
    int max_y = 1400;               //Bildgroesse y
    double min_cx = -2.2;           //Wertebreich X-Achse
    double min_cy = -1.4;           //Wertebreich Y-Achse
    double punkt_abstand = 0.002;   //Aufloesung
    double tar_x = 0;               //bestimmen wo gezoomt wird
    double tar_y = 0;               //falls gezoomt wird
    double max_betrag_quadrat = 4.0;// > x --> x in Mandelbrotmenge
    int max_iteration = 1000;       // Anzahl der Iterationen pro Pixel
    char rgb[3*max_x*max_y];        // Bild Array
    
    MPI_Status status;              
    
    //MPI initialisierung
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    
    // Fuer alle SlaveProzesse
    if(rank != 0)
    {    
        // Berechnungsbeispiel von Wikipedia als Vorlage
        // http://de.wikipedia.org/wiki/Mandelbrot-Menge#Programmbeispiel
        
        for ( pix_x = rank-1 ; pix_x < max_x ; pix_x = pix_x + size-1 )
        {
            cx = (min_cx + tar_x) + pix_x * punkt_abstand;
        
            for ( pix_y = 0 ; pix_y < max_y ; pix_y++ )
            {
                cy = (min_cy + tar_y) + pix_y * punkt_abstand;
            
                iterations_wert = punkt_iteration( cx, cy, max_betrag_quadrat, max_iteration);
                
                // Int-Array mit dem Iterationswert und dem Pixel
                pixiter[0] = iterations_wert; 
                pixiter[1] = pix_x;
                pixiter[2] = pix_y;
                
                // Senden von pixiter mit MPI an den "Master"
                MPI_Send(pixiter, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
    }
    
    // Fuer den Master Prozess
    if(rank == 0)
    {
        for (i = 0 ; i < (max_x * max_y) ; i++ )
        {
            // Empfangen des Iterationswertes + dem Pixel
            MPI_Recv(pixiter, 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            
            // muesste eigentlich waehle Graustufen heissen
            // die Berechnung der Graustufen ist auch nicht ganz richtig
            // aber ich wollte die coolen Blitze am Apfelmaennchen haben
            waehle_farbe(farbe, pixiter[0], max_iteration);
            
            // den Farb/Graustufen-Wert in das Array schreiben
            setpix(rgb, pixiter[1], pixiter[2], max_x, farbe);
        }
        
        //BMP-Schreiber von "bmp.h" das BildArray schreiben lassen usw.
        write_bmp("./apfel.bmp", max_x, max_y, rgb);
    }
    
    // Ende der Vorstellung
    MPI_Finalize();
    
    
    return EXIT_SUCCESS;
}
