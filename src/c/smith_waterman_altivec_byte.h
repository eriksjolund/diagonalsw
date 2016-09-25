#ifndef SMITH_WATERMAN_ALTIVEC_BYTE_H
#define SMITH_WATERMAN_ALTIVEC_BYTE_H

#ifdef __cplusplus
extern "C" {
#endif 



size_t 
workspace_needed_for_byte_version(int db_length );


int
smith_waterman_altivec_byte(unsigned char *     query_sequence,
                            unsigned char *     query_profile_word,
                            int                 query_length,
                            unsigned char *     db_sequence,
                            int                 db_length,
                            unsigned char       bias,
                            unsigned char       gap_open,
                            unsigned char       gap_extend,
                            unsigned char *     workspace);

#ifdef __cplusplus
}
#endif 

#endif // SMITH_WATERMAN_ALTIVEC_BYTE_H
