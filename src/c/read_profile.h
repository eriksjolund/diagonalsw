#ifndef READ_PROFILE_H
#define READ_PROFILE_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>

int
read_profile(  FILE * fp,
         int *query_length,
	       unsigned char *bias,
 int max_query_sequence_length,
	       unsigned char * profile_byte,
	  unsigned short * profile_word     
);

#ifdef __cplusplus
}
#endif 

#endif // READ_PROFILE_H
