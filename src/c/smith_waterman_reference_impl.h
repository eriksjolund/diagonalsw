#ifndef SMITH_WATERMAN_REFERENCE_IMPL_H
#define SMITH_WATERMAN_REFERENCE_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif 


int 
vectorMatrixIsOk( unsigned short * E, unsigned short * Eref, const char * name, int query_length, int db_length ) ;



void
printMatrix( unsigned short * m, int query_length, int db_length );

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
			    unsigned short *    H);

#ifdef __cplusplus
}
#endif 

#endif // SMITH_WATERMAN_REFERENCE_IMPL_H
