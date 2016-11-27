#include <array>
#include <tmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <smmintrin.h>  
#include <pmmintrin.h>
#include <avx2intrin.h>
#include <stdio.h>
#include <cassert>
#include "sw_constants.h"
#include "smith_waterman_vector.h"
#include "sse_funcs.h"

//#include <stddef.h>
//size_t workspace_needed_for_byte_version(int db_length);


size_t workspace_needed_for_byte_version(int db_length) { 
  return ((db_length + 15)*2*16);
}

enum class Phase
{
     kStart,
     kMiddle,
     kEnd
};

struct DoNothing {
  DoNothing(unsigned short *) {}
  void operator()(const __m128i &v, int queryindex, int query_length, int dbindex, int db_length) {
}

};

struct StoreMatrixELements {
  StoreMatrixELements(unsigned short *matrix) : matrix_(matrix) {assert(matrix_);}
  void operator()(const __m128i &v, int queryindex, int query_length, int dbindex, int db_length) {
 
    set_matrix_values_from_diagonal_byte_vector(v, matrix_, queryindex, query_length, dbindex, db_length);
}

 private:
  unsigned short *matrix_;
};

template<typename E_MATRIX_WATCHER, typename F_MATRIX_WATCHER, typename H_MATRIX_WATCHER>
class SmithWatermanTemplate {
  public:
    SmithWatermanTemplate(E_MATRIX_WATCHER e_matrix_watcher,
                          F_MATRIX_WATCHER f_matrix_watcher,
                          H_MATRIX_WATCHER h_matrix_watcher) : 
e_matrix_watcher_(e_matrix_watcher), 
f_matrix_watcher_(f_matrix_watcher), 
h_matrix_watcher_(h_matrix_watcher) {}


// TODO: The inner_loop should be a template with at least "phase" and "innerloop_iter" as template parameters. 
inline void inner_loop(const Phase &phase, const int &innerloop_iter, std::array<__m128i, 16> &v, 
    int  &                   i,
    int &j,
    int &k,
    int &overflow,
    unsigned char *    &p,
    unsigned char      &score,
    __m128i & Fup,
    std::array<__m128i, 2> & Hup,
    __m128i & E,
    __m128i & F,
    __m128i & H,
    __m128i & tmp,
    __m128i & v_maxscore,
    __m128i & v_bias,
    __m128i & v_gapopen,
    __m128i & v_gapextend,
    __m128i & v_score,
    __m128i & v_score_load1,
                            int               &  db_length,
                            unsigned char *    & db_sequence,
const int & mask1,
const int & mask2,
const int & mask3,
const  __m128i & mask4,
                            unsigned char *   &  query_profile_byte,
   int            &     query_length,
int & dbindex
) { 

if (phase == Phase::kStart) {
k                 = db_sequence[(innerloop_iter % 16) + 1];
}

if (phase == Phase::kMiddle) {
k                = db_sequence[j+(innerloop_iter % 16)+1];
}

// prefetch score for next step 

if (phase == Phase::kEnd) {
            // save old values of F and H to use on next row
      _mm_store_si128( (__m128i *) p , F);
      _mm_store_si128((__m128i *)( p + 16 ), H);
}

if (phase == Phase::kStart) {
        Fup =  _mm_load_si128( (__m128i *) p );
        Hup[(innerloop_iter % 2)]   =   _mm_load_si128( (__m128i*) (p+16));
}

v[(innerloop_iter+15) % 16] = v_score_load1;
       v[(innerloop_iter+7) % 16]=_mm_blend_epi32(v[(innerloop_iter+7) % 16],v[(innerloop_iter+15) % 16],mask1);
       v[(innerloop_iter+3) % 16]=_mm_blend_epi32(v[(innerloop_iter+3) % 16],v[(innerloop_iter+7) % 16],mask2);
       v[(innerloop_iter+1) % 16]=_mm_blend_epi16(v[(innerloop_iter+1) % 16],v[(innerloop_iter+3) % 16],mask3);
       v[(innerloop_iter+0) % 16]=_mm_blendv_epi8(v[(innerloop_iter+0) % 16],v[(innerloop_iter+1) % 16],mask4);
v_score     = v[(innerloop_iter+0) % 16];

if (phase == Phase::kMiddle) { 
        Fup =  _mm_load_si128( (__m128i *) ( p + 512 ) );
        Hup[(innerloop_iter % 2)] =  _mm_load_si128( (__m128i *) ( p + 528 ) );
       // save old values of F and H to use on next row
      _mm_store_si128( (__m128i *) p , F);
      _mm_store_si128( (__m128i *)(p + 16) , H);
}

p += 32; // move ahead 32 bytes

if (phase == Phase::kEnd) {
            Fup = _mm_slli_si128(F,1);
            Hup[(innerloop_iter % 2)] = _mm_slli_si128(H,1); 
}

if (phase == Phase::kStart || phase == Phase::kMiddle) { 
// shift into place so we have complete F and H vectors
// that refer to the values one unit up from each cell
// that we are currently working on.
Fup    = _mm_alignr_epi8(F,Fup,15);
}
        v_score_load1 =  _mm_load_si128( (__m128i *) ( query_profile_byte + 16*k ) );

if (phase == Phase::kStart || phase == Phase::kMiddle) { 
   Hup[(innerloop_iter % 2)] = _mm_alignr_epi8(H,Hup[(innerloop_iter % 2)],15);
} 

// do the dynamic programming 

// update E value

E   = _mm_subs_epu8(E,v_gapextend);
tmp = _mm_subs_epu8(H,v_gapopen);
E   = _mm_max_epu8(E,tmp);

// update F value
F   = _mm_subs_epu8(Fup,v_gapextend);


tmp = _mm_subs_epu8(Hup[(innerloop_iter % 2)],v_gapopen);

F   = _mm_max_epu8(F,tmp);

// add score to H

H   =  _mm_adds_epu8(Hup[((innerloop_iter+1) % 2)],v_score);
//H   =  _mm_adds_epu8(Hup[((innerloop_iter+1) % 2)],v[(innerloop_iter+0) % 16]);

H   = _mm_subs_epu8(H,v_bias);
// set H to max of H,E,F
H   = _mm_max_epu8(H,E);

H   = _mm_max_epu8(H,F);

 e_matrix_watcher_(E,i,query_length,dbindex,db_length);
 f_matrix_watcher_(F,i,query_length,dbindex,db_length);
 h_matrix_watcher_(H,i,query_length,dbindex,db_length);
 dbindex++;

// Update highest score encountered this far
v_maxscore = _mm_max_epu8(v_maxscore,H);

}

    int smith_waterman_vector_byte(unsigned char *     query_sequence,
                            unsigned char *     query_profile_byte,
                            int                 query_length,
                            unsigned char *     db_sequence,
                            int                 db_length,
                            unsigned char       bias,
                            unsigned char       gap_open,
                            unsigned char       gap_extend,
                            unsigned char *     workspace
 )
{
    int                     i,j,k;
    int                     overflow;
    unsigned char *         p;
    unsigned char           score;   
    std::array<__m128i,2> Hup;
    __m128i     Fup,E,F,H,tmp;
    __m128i    v_maxscore;
    __m128i    v_bias,v_gapopen,v_gapextend;
    __m128i    v_score;

    __m128i    v_score_load1;
//    __m128i    v_score_load2;  

    const int mask1 = (1 << 0) | (1 << 1);
    const int mask2 = (1 << 0) | (1 << 2);
    const int mask3 = (1 << 0) | (1 << 2) | (1 << 4) | (1 << 6);

    const  __m128i mask4=_mm_set_epi8(0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255);

    std::array<__m128i, 16> v;
      
    /* Load the bias to all elements of a constant */
    v_bias = _mm_set1_epi8(bias);
    
    /* Load gap opening penalty to all elements of a constant */

    v_gapopen = _mm_set1_epi8(gap_open);

    /* Load gap extension penalty to all elements of a constant */

    v_gapextend = _mm_set1_epi8(gap_extend);

    v_maxscore = _mm_setzero_si128();

    // Zero out the storage vector 
    k = 2*(db_length+15);

    __m128i * iter_ptr;
    for(i=0, iter_ptr =  ( __m128i * ) workspace  ;i<k;i++,iter_ptr++)
    {
        // borrow the zero value in v_maxscore to have something to store
      _mm_store_si128( (__m128i *) iter_ptr , v_maxscore);
    }
    
    for(i=0;i<query_length;i+=16)
    {
       int dbindex=0;

    // Why not zero out the last element? I've forgot the details.
        for(int index=0;index<15; ++index) {
            v[index]=_mm_set1_epi8(0);
        }

        k             = db_sequence[0];
        v_score_load1 =  _mm_load_si128( (__m128i *) ( query_profile_byte + 16*k ) );

        E          = _mm_setzero_si128();
        F          = _mm_setzero_si128();
        H          = _mm_setzero_si128();
        Hup[1]      = _mm_setzero_si128();

        // reset pointers to the start of the saved data from the last row
        p = workspace;
        
        // start directly and prefetch score column


/*  The for loop should be replaced by something like */
/* inner_loop< StartPhase, 0 >( v, i, j, k, overflow, p, and so on ... */
/* inner_loop< StartPhase, 1 >( v, i, j, k, overflow, p, and so on ... */
/* inner_loop< StartPhase, 2 >( v, i, j, k, overflow, p, and so on ... */
/* and so on ... */

        for (int it = 0; it < 16; ++it) {
           inner_loop(Phase::kStart, it, v, i, j, k, overflow, p, score, Fup,
                      Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen,
                      v_gapextend, v_score, v_score_load1,db_length,db_sequence,
                      mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);
        }
        p = workspace;

        for(j=16;j<db_length;j+=16)
        { 
            for (int it = 16; it < 32; ++it) {
                inner_loop(Phase::kMiddle, it, v, i, j, k, overflow, p, score, Fup,
                           Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen,
                           v_gapextend, v_score, v_score_load1, db_length,db_sequence,
                           mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);
            }
        }
 
/*
  It seems it will not be possible to use this approach, because the innerloop_iter parameter can not be deduced at compile time. I am thinking about the plan to make innner_loop a template.

    switch ((15 -j +db_length)) {
    case 15: inner_loop(Phase::kEnd, 16+1+                 + (15 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);

    case 14: inner_loop(Phase::kEnd, 16+1+                 + (16 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);

    case 13: inner_loop(Phase::kEnd, 16+1+                 + (17 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);

    case 12: inner_loop(Phase::kEnd, 16+1+                 + (18 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);

    case 11: inner_loop(Phase::kEnd, 16+1+                 + (19 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);

    case 10: inner_loop(Phase::kEnd, 16+1+                 + (20 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);

    case 9: inner_loop(Phase::kEnd, 16+1+                 + (21 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);
    case 8: inner_loop(Phase::kEnd, 16+1+                 + (22 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);


    case 7: inner_loop(Phase::kEnd, 16+1+                 + (23 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);


    case 6: inner_loop(Phase::kEnd, 16+1+                 + (24 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);


    case 5: inner_loop(Phase::kEnd, 16+1+                 + (25 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);


    case 4: inner_loop(Phase::kEnd, 16+1+                 + (26 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);


    case 3: inner_loop(Phase::kEnd, 16+1+                 + (27 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);


    case 2: inner_loop(Phase::kEnd, 16+1+                 + (28 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);

    case 1: inner_loop(Phase::kEnd, 16+1+                 + (29 -j +db_length)                                    ,v, i, j, k, overflow, p, score, Fup, Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen, v_gapextend, v_score, v_score_load1,db_length,db_sequence,mask1,mask2,mask3,mask4,query_profile_byte,query_length,dbindex);

*/

    for (int it = 32; it < 47; ++it) {
        if ( j >= db_length+15 ) { 
            goto ending;
        }
        inner_loop(Phase::kEnd, it, v, i, j, k, overflow, p, score, Fup,
                   Hup, E, F, H, tmp, v_maxscore, v_bias, v_gapopen,
                   v_gapextend, v_score, v_score_load1, db_length, db_sequence,
                   mask1, mask2, mask3, mask4, query_profile_byte, query_length, dbindex);
        j++;  
    }

ending:

      _mm_store_si128( (__m128i *) p , F);
      _mm_store_si128((__m128i *)( p + 16 ), H);
        query_profile_byte += 16*ALPHABET_SIZE;

        // End of this row (actually 16 rows due to SIMD).
        // Before we continue, check for overflow.

        tmp      = _mm_subs_epu8(_mm_set1_epi8(255),v_bias);
        tmp      = _mm_cmpeq_epi8 (tmp,v_maxscore);
        int noOverflow =  _mm_testc_si128( _mm_setzero_si128(),tmp);
        overflow = ! noOverflow ;
    }

    if(overflow)
    {
        return -1;
    }
    else
    {
        // find largest score in the v_maxscore vector

        tmp = _mm_alignr_epi8(v_maxscore,v_maxscore,8);
        v_maxscore = _mm_max_epu8(v_maxscore,tmp);
        tmp = _mm_alignr_epi8(v_maxscore,v_maxscore,4);
        v_maxscore = _mm_max_epu8(v_maxscore,tmp);
        tmp = _mm_alignr_epi8(v_maxscore,v_maxscore,2);
        v_maxscore = _mm_max_epu8(v_maxscore,tmp);
        tmp = _mm_alignr_epi8(v_maxscore,v_maxscore,1);
        v_maxscore = _mm_max_epu8(v_maxscore,tmp);
      
        // store in temporary variable
        score=_mm_extract_epi8(v_maxscore,0);
      
        // return largest score
        return score;
    }
  }
  private:
  E_MATRIX_WATCHER e_matrix_watcher_;
  F_MATRIX_WATCHER f_matrix_watcher_;                          
  H_MATRIX_WATCHER h_matrix_watcher_;
};








