#ifndef READ_SEQUENCE_H
#define READ_SEQUENCE_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <stdio.h>

int
read_sequence(FILE *             fp,
              char *             name,
              unsigned char *    sequence,
              int                maxlength_name,
              int                maxlength_sequence);

#ifdef __cplusplus
}
#endif 

#endif // READ_SEQUENCE_H
