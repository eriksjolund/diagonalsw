
#include <stdio.h>
#include "sse_funcs.h"


void printvec(__m128i v, char * a)
{
    int i;
    union { __m128i v; unsigned char i[16]; } conv;
    conv.v=v;
    printf("%s=",a);
    for(i=0;i<16;i++)
      printf("%6d",conv.i[i]);
    printf("\n");
}




void  set_matrix_values_from_diagonal_byte_vector(__m128i v, unsigned short * matrix, int queryindex,int query_length,int dbindex,int db_length)
{
  int i;
  union { __m128i v; unsigned char i[16]; } conv;
  conv.v=v;
  for(i=0;i<16;i++)
    {
      int dbindex2 = dbindex-i;
      int queryindex2 = queryindex+i;
      if (( queryindex2 >= 0 ) && ( queryindex2 < query_length ) && ( dbindex2 >= 0 ) && ( dbindex2 < db_length )) {
	//		printf("%s(%.5d,%.5d) %d\n",matrix,dbindex2,queryindex2,conv.i[i]);
        matrix[dbindex2*query_length + queryindex2 ]=conv.i[i];
      }
    }
}

void set_matrix_values_from_diagonal_word_vector(__m128i v, unsigned short * matrix,int queryindex,int query_length,int dbindex,int db_length)
{
  int i;
  union { __m128i v; unsigned short i[8]; } conv;
  conv.v=v;
  for(i=0;i<8;i++)
    {
      int dbindex2 = dbindex-i;
      int queryindex2 = queryindex+i;
      if (( queryindex2 >= 0 ) && ( queryindex2 < query_length ) && ( dbindex2 >= 0 ) && ( dbindex2 < db_length )) {
	//	printf("%s(%.5d,%.5d) %d\n",matrix,dbindex2,queryindex2,conv.i[i]);
        matrix[dbindex2*query_length + queryindex2 ]=conv.i[i];
      }
    }
}

