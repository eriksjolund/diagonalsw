#ifndef SMITH_WATERMAN_SSE_WORD_H
#define SMITH_WATERMAN_SSE_WORD_H
#ifdef __cplusplus
extern "C" {
#endif 



size_t 
workspace_needed_for_word_version(int db_length );


int
smith_waterman_sse_word(unsigned char *     query_sequence,
                            unsigned short *    query_profile_word,
                            int                 query_length,
                            unsigned char *     db_sequence,
                            int                 db_length,
                            unsigned short      bias,
                            unsigned short      gap_open,
                            unsigned short      gap_extend,
                            unsigned short *    workspace);

#ifdef __cplusplus
}
#endif 

#endif // SMITH_WATERMAN_SSE_WORD_H
