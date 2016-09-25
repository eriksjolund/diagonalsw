#include <stdio.h>

#include "sw_constants.h"
#include "smith_waterman_reference_impl.h"




int 
vectorMatrixIsOk( unsigned short * m, unsigned short * mRef, const char * name, int query_length, int db_length ) 
{
  int i,j;
  unsigned short * mIter = m;
  unsigned short * mRefIter = mRef;

  for ( j=0; j<db_length; j++) {
    for ( i=0; i<query_length; i++) {
      if ( *mIter != *mRefIter ) {


        fprintf(stderr, "error: vector (SSE) version and scalar version of the\n       %s matrix are not equal\n",name);
        fprintf(stderr,"       they differ at query position=%i, db position=%i\n",i+1,j+1);
        fprintf(stderr,"       scalar cell=%u vector cell=%u\n",*mRefIter,*mIter);
	/* uncomment this if you want to debug 
        printf(" Matrix %s, scalar version:\n",name);
        printMatrix(mRef,query_length, db_length);        
        printf(" Matrix %s, SSE version:\n",name);
        printMatrix(m,query_length, db_length);
*/        
        return 0;
      }
      mIter++;
      mRefIter++;
    }
 }
 return 1;
}



void 
printMatrix( unsigned short * m, int query_length, int db_length ) 
{ 
  int i,j;
  for ( j=0; j<db_length; j++) {
    for ( i=0; i<query_length; i++) {
      printf("%u ",*m);
    }
    printf("\n");
    m++;
 }
 return;
}



int
smith_waterman_reference_impl(unsigned char *     query_sequence,
                            int                 query_length,
                            unsigned char *     db_sequence,
                            int                 db_length,
                            unsigned char       bias,
                            unsigned char       gap_open,
                            unsigned char       gap_extend,
			    unsigned char *     substitution_matrix,
         		    unsigned short *    E,
			    unsigned short *    F,
			    unsigned short *    H)
{

  int i,j;    
  int gap_db_extend_score,gap_db_extend_score_old, query_with_gap_score ;
  int old_score_vector[query_length+1];
  int old_score_vector_with_gap[query_length+1];

  int new_score_vector[query_length+1];
  int max_score=0;
  int pos=0;

  for(i=0;i<query_length+1;i++) {           
    old_score_vector[i]=0;
    old_score_vector_with_gap[i]=-1000;
    new_score_vector[i]=0;
  }

  //  old_score_vector[0], old_score_vector_with_gap[0] and new_score_vector[0] are just outside the matrix.
  //  old_score_vector[1], old_score_vector_with_gap[1] and new_score_vector[1] are inside the matrix.

  for(j=0;j<db_length;j++) {
    gap_db_extend_score_old = -1000;
    for(i=1;i<query_length+1;i++) { 

      int align_score =  old_score_vector[i-1] + substitution_matrix[ db_sequence[j]*ALPHABET_SIZE+query_sequence[i-1] ] - bias;
      int gap_query_score;
      int gap_db_score;
	gap_query_score = old_score_vector[i] - gap_open ;
        query_with_gap_score = old_score_vector_with_gap[i] - gap_extend ;
        old_score_vector_with_gap[i] = ( query_with_gap_score > gap_query_score ) ? query_with_gap_score : gap_query_score;
        if ( old_score_vector_with_gap[i] < 0 ) old_score_vector_with_gap[i] = 0;
	gap_db_score = new_score_vector[i-1] - gap_open;
	gap_db_extend_score = 	gap_db_extend_score_old - gap_extend;
	gap_db_extend_score_old  = ( gap_db_extend_score > gap_db_score ) ? gap_db_extend_score : gap_db_score;
        if ( gap_db_extend_score_old < 0 )  { gap_db_extend_score_old=0; }

      int element_score = 0;
      if ( align_score > element_score ) element_score = align_score;
      if ( old_score_vector_with_gap[i] > element_score ) element_score = old_score_vector_with_gap[i];

      if ( gap_db_extend_score_old   > element_score ) {
        element_score = gap_db_extend_score_old; 
      }
      new_score_vector[i] = element_score;

   if ( element_score > max_score ) max_score = element_score;


     E[pos]= old_score_vector_with_gap[i];
     F[pos]= gap_db_extend_score_old;
     H[pos]= element_score;

   pos++;   

   /*
   printf("Eref(%.5d,%.5d) %d\n",j,i-1,old_score_vector_with_gap[i]);
   printf("Fref(%.5d,%.5d) %d\n",j,i-1,gap_db_extend_score_old);
   printf("Href(%.5d,%.5d) %d\n",j,i-1,element_score);
   */


    }    
  for(i=0;i<query_length+1;i++) {           
      old_score_vector[i] = new_score_vector[i];
    }
  }
  return max_score;
}

