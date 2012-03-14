#include "stdafx.h"
#include "mask.h"
#include "cimage.h"
#include <stdio.h>
char 		*argv0;		/* It's a magic ! */

#define M_PI	3.1415926535897932384626433832795

//
GrayImage gabor(GrayImage in,int width,int height,double degree,double sigma_x,
                           double sigma_y,double f)
{
  Filter_mask real_part, img_part;    
  GrayImage bank;

  making_gabor_filter(f, M_PI*(degree/180.0), 
		      sigma_x, sigma_y, &real_part, &img_part);

   //bank = process(in,width,height, &real_part, &img_part); 
   return(bank);
}


GrayImage process(GrayImage in,int width,int height,Filter_mask *real_part,
	       Filter_mask *img_part)
{

  register int i, j;	/* work valiable */
//  gray  max_val= MAX_VAL ;
  long **work0, **work1;         /*  .$B%o!<%-%s%0NN0h.(B */
  long **conv() ;
  
  /*.$BF~=PNO$OGr9u$N$?$a$K.(B gray   .$B$K@_Dj$7$?.(B*/
  /*-- allocate the memory --*/
  
   
  GrayImage dest(width,height);

  //printf("destf=%d\n",dest[1][1]);

//  GrayImage work0;

  work0 = conv(width, height, in, real_part);
  work1 = conv(width, height, in, img_part);

  //printf("work0=%d\n",work0[0][0]);
  //printf("work1=%d\n",work1[0][0]);

  fusion( width, height,  work0, work1, dest);

  //printf("dests=%d\n",dest[1][1]);

  pgm_freearray((GrayImage )work0,height);
  pgm_freearray((GrayImage )work1,height);


  /** pgm_writepgm( outstream, dest ,width, height, MAX_VAL, 0 ); **/

  return(dest);
} 

/*
long **conv(width, height, in, fil)
  
     int width,height;
     gray   **in;    
     Filter_mask *fil;

{


  register int row,column,i,j, h_size,v_size;
  double sum;
  long **work;

  work = (long **)calloc(height,sizeof(long *));
  for(i=0;i<height;i++)
    work[i]=(long *)calloc(width,sizeof(long));
  
  if (fil->sum < 1.0){
    for( row = 0; row < height; row++ )
      for( column = 0; column<  width; column++)
	work[row][column]=0;
    
    //f//printf(stderr, "warning:filter coefficient are all zero\n");
    return( work);
  }
  
  v_size = (fil->vsize -1)/2;
  h_size = (fil->hsize -1)/2;
  
  for( row = 0; row < height; row++ )	
    for( column =0; column< width; column++){ 
      sum=0.0;
      for (i= -v_size; i<= v_size; i++)
	for (j= -h_size; j<= h_size; j++)
	  if (row+i >= 0 && row+i < height &&
	      column+j >= 0 && column+j < width)
	    sum+= (double)in[row+i][column+j]*(double)fil->co[i+v_size][j+h_size];
      work[row][column] = (long)sum/fil->sum;
      /*  //f//printf(stderr,"%f\n ",work[row+v_size][column+h_size]);
    }
  //printf("work[0][0]=%d\n",work[0][0]);
  return(work);
  
}
*/
void fusion(int width,int height,long int **w0,long int **w1,GrayImage dest)
    
{
  register int row,column;
  double d;
  double max;
  double ratio;
  
  max =0.0;
  for( row = 0; row < height; row++ )
    for( column = 0; column<  width; column++){ 
      d = (double)w0[row][column]*(double)w0[row][column]
	+ (double)w1[row][column]*(double)w1[row][column];
      d =sqrt(d);
      /*	if(d >max)
		max=d;
		*/
      w0[row][column] = (long)d;
      dest[row][column] = (gray  )0;
      
    }
  max =0.0;
  for( row = 10; row < /** hd->cv_rows-10 **/ height-10; row++ )
    for( column = 10; column< /** hd->cv_cols-10 **/ width-10; column++)
      if(w0[row][column] >max)
	max=(double)w0[row][column];
  
  if (max > 0.0){
    ratio = (double)MAX_VAL/max;
    for( row = 0; row < height; row++ )
      for( column = 0; column< width; column++){ 
	d = rint((double)w0[row][column]*ratio);
	if (d > MASK)
	  dest[row][column] = MASK;
	else
	  dest[row][column] = (gray)d;
	/**	//f//printf(stderr, "%ld ",w0[row][column]); **/

      }
  }
  
}

void making_gabor_filter(double omega,double theta,double sigma_x,double sigma_y,Filter_mask *real_part,Filter_mask *img_part)
{
  int i,h,v, h_half,v_half;
  double h_sum, v_sum;
  double h_, v_,h_1,v_1, ch, cv;
  double gauss;
  double gaussian();

  /*.$B&R$NCM$,$"$^$j$K>.$5$1$l$PITE,Ev$H$7$F%(%i!<$H$9$k.(B*/
  if (sigma_x < 0.1 && sigma_y < 0.1){
    //f//printf(stderr, "too small SIGMAs\n");
    exit(1);
  }
  
  /*.$B%U%#%k%?%5%$%:$r7h$a$k.(B*/
  h_half = horizontal_filter_size(theta,sigma_x, sigma_y);
  v_half = vertical_filter_size(theta,sigma_x, sigma_y);



  real_part->hsize = 2*h_half+1;
  real_part->vsize = 2*v_half +1;
  img_part->hsize = real_part->hsize;
  img_part->vsize = real_part->vsize;


  /*.$B%U%#%k%?78?t$N3JG<$KI,MW$JNN0h$r3NJ]$9$k.(B*/

  real_part->co = (long **)calloc(real_part->vsize,sizeof(long *));
  for(i=0;i<real_part->vsize;i++)
    real_part->co[i]=(long *)calloc(real_part->hsize,sizeof(long));

  img_part->co = (long **)calloc(img_part->vsize,sizeof(long *));
  for(i=0;i<img_part->vsize;i++)
    img_part->co[i]=(long *)calloc(img_part->hsize,sizeof(long));
  h_sum = 0; 
  v_sum = 0; 

  for(v= -v_half; v <=v_half; v++){
    for(h= -h_half; h <=h_half; h++)
      {  
	 h_ = ((double)h)*cos(theta) + ((double)v) *sin(theta);
	 v_ = - ((double)h)*sin(theta) + ((double)v) *cos(theta);  
	
     /**  h_1 = ((double)h)*cos(theta) + ((double)v) *(-sin(theta));
	v_1 = ((double)h)*sin(theta) + ((double)v) *cos(theta); **/
      
	gauss = gaussian(h_,v_, sigma_x, sigma_y);
     	ch = OFFSET *  gauss  * cos(2.0*PI*omega*h_);
	h_sum += fabs(ch);
	cv = OFFSET *  gauss  * sin(2.0*PI*omega*h_);
	v_sum += fabs(cv);

	real_part->co[v+v_half][h+h_half]
	  =(long)rint(ch);
	img_part->co[v+v_half][h+h_half]
	  = (long)rint(cv); 
         
      }
  }
  for(v=0;v<2*v_half;v++)
    for(h=0;h<2*h_half;h++)
      //printf("real_part->co[%d][%d] = %d\n ",v,h,real_part->co[v][h]);
  
  real_part->sum = h_sum;
  img_part->sum = v_sum;
  //printf("Rpart_sum=%f\n",real_part->sum);
  //printf("Ipart_sum=%f\n",img_part->sum);
 

}


double gaussian(x,y, sigma_x, sigma_y)
     double x,y,sigma_x,sigma_y;
{
  register double r;

  r = -((x/sigma_x)*(x/sigma_x) + (y/sigma_y)*(y/sigma_y))/2;
  r = exp(r);
  r /= 2.0*PI*sigma_x*sigma_y;
  return (r);

}


/*.$B%U%#%k%?%5%$%:$r7h$a$k!$?eJ?J}8~.(B*/
horizontal_filter_size(theta,sigma_x, sigma_y)
     double theta, sigma_x, sigma_y;
{

  double h0,h1;
  h0 = FILTER_SIZE_RATIO_SIGMA*cos(theta)*sigma_x;
  h1 = FILTER_SIZE_RATIO_SIGMA*sin(theta)*sigma_y;

  if(h0 > h1)
    return ( ceil(h0) );
  else
    return ( ceil(h1) );
  
}

vertical_filter_size(theta,sigma_x, sigma_y)
     double theta, sigma_x, sigma_y;
{

  double h0,h1;
  h0 = FILTER_SIZE_RATIO_SIGMA*sin(theta)*sigma_x;
  h1 = FILTER_SIZE_RATIO_SIGMA*cos(theta)*sigma_y;
  if(h0 > h1)
    return ( ceil(h0) );
  else
    return ( ceil(h1) );

}







