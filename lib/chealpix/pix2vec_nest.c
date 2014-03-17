/* -----------------------------------------------------------------------------
 *
 * 
 *  Copyright (C) 1997-2010 Krzysztof M. Gorski, Eric Hivon,
 *                          Benjamin D. Wandelt, Anthony J. Banday, 
 *                          Matthias Bartelmann, 
 *                          Reza Ansari & Kenneth M. Ganga 
 *
 *  Modified by Fabien Baron for vertex calculation
 *
 *  This file is part of HEALPix.
 *
 *  HEALPix is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  HEALPix is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with HEALPix; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  For more information about HEALPix see http://healpix.jpl.nasa.gov
 *
 *----------------------------------------------------------------------------- */
/* pix2vec_nest */

/* Standard Includes */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Local Includes */
#include "chealpix.h"

void pix2vec_nest( long nside, long ipix, double *vec, double *vertex) {

  /*
    c=======================================================================
    subroutine pix2vec_nest(nside, ipix, vec)
    c=======================================================================
    c     gives theta and phi corresponding to pixel ipix (NESTED) 
    c     for a parameter nside
    c=======================================================================
  */
    
      int npix, npface, face_num;
      int  ipf, ip_low, ip_trunc, ip_med, ip_hi;
      int     ix, iy, jrt, jr, nr, jpt, jp, kshift, nl4, diff_phi, iphi_mod, iphi_rat;
      double z, fn, fact1, fact2, z_nv, z_sv, sth, sth_nv, sth_sv ;
      double phi_nv, phi_wv, phi_sv, phi_ev, phi_up, phi_dn, sin_phi, cos_phi, hdelta_phi;
      double piover2 = 0.5 * M_PI;
      double phi;
      int ns_max=8192;
      
      static int pix2x[1024], pix2y[1024];
      
      int jrll[12], jpll[12];// ! coordinate of the lowest corner of each face
      jrll[0]=2;
      jrll[1]=2;
      jrll[2]=2;
      jrll[3]=2;
      jrll[4]=3;
      jrll[5]=3;
      jrll[6]=3;
      jrll[7]=3;
      jrll[8]=4;
      jrll[9]=4;
      jrll[10]=4;
      jrll[11]=4;
      jpll[0]=1;
      jpll[1]=3;
      jpll[2]=5;
      jpll[3]=7;
      jpll[4]=0;
      jpll[5]=2;
      jpll[6]=4;
      jpll[7]=6;
      jpll[8]=1;
      jpll[9]=3;
      jpll[10]=5;
      jpll[11]=7;
      
      
      if( nside<1 || nside>ns_max ) {
	fprintf(stderr, "%s (%d): nside out of range: %ld\n", __FILE__, __LINE__, nside);
	exit(0);
      }
      npix = 12 * nside*nside;
      if( ipix<0 || ipix>npix-1 ) {
	fprintf(stderr, "%s (%d): ipix out of range: %ld\n", __FILE__, __LINE__, ipix);
	exit(0);
      }

      /* initiates the array for the pixel number -> (x,y) mapping */
      if( pix2x[1023] <= 0 ) 
	  mk_pix2xy(pix2x,pix2y);

      fn = 1.*nside;
      fact1 = 1./(3.*fn*fn);
      fact2 = 2./(3.*fn);
      nl4   = 4*nside;

      //c     finds the face, and the number in the face
      npface = nside*nside;

      face_num = ipix/npface;//  ! face number in {0,11}
      ipf = (int)fmod(ipix,npface);//  ! pixel number in the face {0,npface-1}

      //c     finds the x,y on the face (starting from the lowest corner)
      //c     from the pixel number
      ip_low = (int)fmod(ipf,1024);//        content of the last 10 bits
      ip_trunc =   ipf/1024 ;//        truncation of the last 10 bits
      ip_med = (int)fmod(ip_trunc,1024);//   content of the next 10 bits
      ip_hi  =     ip_trunc/1024   ; // content of the high weight 10 bits

      ix = 1024*pix2x[ip_hi] + 32*pix2x[ip_med] + pix2x[ip_low];
      iy = 1024*pix2y[ip_hi] + 32*pix2y[ip_med] + pix2y[ip_low];

      //c     transforms this in (horizontal, vertical) coordinates
      jrt = ix + iy;//  ! 'vertical' in {0,2*(nside-1)}
      jpt = ix - iy;//  ! 'horizontal' in {-nside+1,nside-1}

      //c     computes the z coordinate on the sphere
      jr =  jrll[face_num]*nside - jrt - 1;
       
      if( jr<nside ) 
      { //  north pole region
         nr = jr;
         z = 1. - nr*nr*fact1;
         kshift = 0;
         z_nv = 1. - (nr-1)*fact1*(nr-1);
         z_sv = 1. - (nr+1)*fact1*(nr+1);
      }
      else if (jr <= 3*nside) // equatorial region     
      {
       nr = nside;
       z  = (2*nside-jr)*fact2;
       kshift = (jr - nside) & 1;
       z_nv = (2*nside-jr+1)*fact2;
       z_sv = (2*nside-jr-1)*fact2;
       if (jr == nside)
       { // northern transition
         z_nv =  1.0 - (nside - 1 ) * fact1 * ( nside - 1);
       }
       else if (jr == 3*nside) 
         {// southern transition
           z_sv = -1.0 + (nside-1) * fact1 * (nside-1);
	 }
      }
      else if( jr>3*nside ) 
	{// then south pole region
         nr = nl4 - jr;
         z = - 1. + nr*nr*fact1;
         kshift = 0; 	
	 z_nv = - 1.0 + (nr+1)*fact1*(nr+1);
	 z_sv = - 1.0 + (nr-1)*fact1*(nr-1);
	}	      
      
      // computes the phi coordinate on the sphere, in [0,2Pi]
      // jp = (jpll[face_num+1]*nr + jpt + 1 + kshift)/2;//  ! 'phi' number in the ring in {1,4*nr}
      jp = (jpll[face_num]*nr + jpt + 1 + kshift)/2;
      if( jp>nl4 ) jp = jp - nl4;
      if( jp<1 )   jp = jp + nl4;

      phi = (jp - (kshift+1) * 0.5 ) * (piover2 / nr);

      // pixel center
      sth = sqrt(1.0-z*z);
      cos_phi = cos(phi);
      sin_phi = sin(phi)	;
      vec[0] = sth * cos_phi;
      vec[1] = sth * sin_phi;
      vec[2] = z;
      
      // DO vertex
      phi_nv = phi;
      phi_sv = phi;
      diff_phi = 0 ;// phi_nv = phi_sv = phi
      
      phi_up = 0.0;
      iphi_mod = (jp-1) % nr ; // in {0,1,... nr-1}
      iphi_rat = (jp-1) / nr ;     // in {0,1,2,3}
      if (nr > 1) 	
	phi_up = piover2 * (iphi_rat +  iphi_mod / (double)(nr-1));

      phi_dn = piover2 * (iphi_rat + (iphi_mod+1) /(double)(nr+1));
      
      if (jr < nside) 
      {
	// North polar cap
	phi_nv = phi_up;
	phi_sv = phi_dn;
	diff_phi = 3; // both phi_nv and phi_sv different from phi
      } 
      else if (jr > 3*nside)
      {// South polar cap
      phi_nv = phi_dn;
      phi_sv = phi_up;
      diff_phi = 3; // both phi_nv and phi_sv different from phi
      }
      else if (jr == nside)
      {      // North transition
      phi_nv = phi_up;
      diff_phi = 1;
      }
      else if (jr == 3*nside)
      {    // South transition
      phi_sv = phi_up;
      diff_phi = 2;
      }
      
      hdelta_phi = M_PI / (4.0 * nr);

      
      // FB: convention for vexter is vertex[i][j] = vertex[i+3*j]
      
      // west vertex
      phi_wv      = phi - hdelta_phi;
      vertex[3] = sth * cos(phi_wv);
      vertex[4] = sth * sin(phi_wv);
      vertex[5] = z;

      // east vertex
      phi_ev      = phi + hdelta_phi;
      vertex[9] = sth * cos(phi_ev);
      vertex[10] = sth * sin(phi_ev);
      vertex[11] = z;

      // north and south vertices
      sth_nv = sqrt((1.-z_nv)*(1.+z_nv));
      sth_sv = sqrt((1.-z_sv)*(1.+z_sv));
      if (diff_phi == 0) 
      {
	  vertex[0] = sth_nv * cos_phi;
	  vertex[1] = sth_nv * sin_phi;
	  vertex[6] = sth_sv * cos_phi;
	  vertex[7] = sth_sv * sin_phi;
      } 
      else
      {
	  vertex[0] = sth_nv * cos(phi_nv);
	  vertex[1] = sth_nv * sin(phi_nv);
	  vertex[6] = sth_sv * cos(phi_sv);
	  vertex[7] = sth_sv * sin(phi_sv);
      }
      vertex[2] = z_nv;
      vertex[8] = z_sv;
}

