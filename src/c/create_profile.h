#ifndef CREATE_PROFILE_H
#define CREATE_PROFILE_H

#ifdef __cplusplus
extern "C" {
#endif 



unsigned short *
create_profile_word(unsigned char *    query_sequence,
                    int                query_length,
                    unsigned char *    substitution_matrix,
                    unsigned char      bias);

unsigned char *
create_profile_byte(unsigned char *    query_sequence,
                    int                query_length,
                    unsigned char *    substitution_matrix,
                    unsigned char      bias);


#ifdef __cplusplus
}
#endif 

#endif // CREATE_PROFILE_H
