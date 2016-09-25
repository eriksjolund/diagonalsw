#ifndef SMITH_WATERMAN_VECTOR_H
#define SMITH_WATERMAN_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif 



size_t 
workspace_needed_for_byte_version(int db_length );


size_t 
workspace_needed_for_word_version(int db_length );


int
smith_waterman_vector_byte(unsigned char *     query_sequence,
                            unsigned char *     query_profile_word,
                            int                 query_length,
                            unsigned char *     db_sequence,
                            int                 db_length,
                            unsigned char       bias,
                            unsigned char       gap_open,
                            unsigned char       gap_extend,
                            unsigned char *     workspace);


int
smith_waterman_vector_byte_H(unsigned char *     query_sequence,
                            unsigned char *     query_profile_word,
                            int                 query_length,
                            unsigned char *     db_sequence,
                            int                 db_length,
                            unsigned char       bias,
                            unsigned char       gap_open,
                            unsigned char       gap_extend,
                            unsigned char *     workspace,
			    unsigned short *    Hmatrix
);


int
smith_waterman_vector_byte_EFH(unsigned char *     query_sequence,
                            unsigned char *     query_profile_word,
                            int                 query_length,
                            unsigned char *     db_sequence,
                            int                 db_length,
                            unsigned char       bias,
                            unsigned char       gap_open,
                            unsigned char       gap_extend,
                            unsigned char *     workspace,
                            unsigned short *    Ematrix,
			    unsigned short *    Fmatrix,
			    unsigned short *    Hmatrix
);

int
smith_waterman_vector_word(unsigned char *     query_sequence,
                            unsigned short *    query_profile_word,
                            int                 query_length,
                            unsigned char *     db_sequence,
                            int                 db_length,
                            unsigned short      bias,
                            unsigned short      gap_open,
                            unsigned short      gap_extend,
                            unsigned short *    workspace);


int
smith_waterman_vector_word_H(unsigned char *     query_sequence,
                            unsigned short *    query_profile_word,
                            int                 query_length,
                            unsigned char *     db_sequence,
                            int                 db_length,
                            unsigned short      bias,
                            unsigned short      gap_open,
                            unsigned short      gap_extend,
                            unsigned short *    workspace,
			    unsigned short *    Hmatrix);

int
smith_waterman_vector_word_EFH(unsigned char *     query_sequence,
                            unsigned short *    query_profile_word,
                            int                 query_length,
                            unsigned char *     db_sequence,
                            int                 db_length,
                            unsigned short      bias,
                            unsigned short      gap_open,
                            unsigned short      gap_extend,
                            unsigned short *    workspace,
                            unsigned short *    Ematrix,
			    unsigned short *    Fmatrix,
			    unsigned short *    Hmatrix
);

#ifdef __cplusplus
}
#endif 


#endif // SMITH_WATERMAN_VECTOR_H
