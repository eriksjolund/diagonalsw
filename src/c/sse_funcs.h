#ifndef SSE_FUNCS_H
#define SSE_FUNCS_H

#ifdef __cplusplus
extern "C" {
#endif 



#include <tmmintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <smmintrin.h>  
#include <pmmintrin.h>

void printvec(__m128i v, char * a);

void set_matrix_values_from_diagonal_byte_vector(__m128i v, unsigned short * matrix, int queryindex,int query_length,int dbindex,int db_length);
void set_matrix_values_from_diagonal_word_vector(__m128i v, unsigned short * matrix,int queryindex,int query_length,int dbindex,int db_length);

#ifdef CHECK_MATRICES_BYTE
void printvec_byte_index(__m128i v, char * matrix,int queryindex,int query_length,int dbindex,int db_length);
#endif


#ifdef CHECK_MATRICES_WORD
void printvec_word_index(__m128i v, char * matrix,int queryindex,int query_length,int dbindex,int db_length);
#endif

#ifdef __cplusplus
}
#endif 

#endif // SSE_FUNCS_H
