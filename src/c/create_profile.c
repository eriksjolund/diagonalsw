#include <stdlib.h>

#include "sw_constants.h"
#include "create_profile.h"

unsigned short *
create_profile_word(unsigned char *      query_sequence,
                    int                  query_length,
                    unsigned char *      substitution_matrix,
                    unsigned char        bias)
{
    unsigned short *   profile;
    unsigned short *   p;
    unsigned short     data;
    int                i,j,k,l;
    int                diff;
    int                padding;
    
    diff            = query_length & 7;
    padding         = 8 - diff;
    
    /* NB: VALLOC, to get memory aligned to page boundary */
    profile = (unsigned short *) valloc(sizeof(short)*ALPHABET_SIZE*(query_length+padding));
    
    p = profile;
    
    for(i=0;i<query_length;i+=8)
    {
        for(j=0;j<ALPHABET_SIZE;j++)
        {
            for(k=0;k<8;k++)
            {
#if TARGET_WORD_1load_HFsave4th || TARGET_WORD_1load
                l = i+k;
                if(l<query_length)
                {
                    data = substitution_matrix[j*ALPHABET_SIZE+query_sequence[l]];
                }
                else
                {
                    data = 0;
                }
                *(p++) = data;
#else

                l = i+k;
		data =0;
		    if(k<4)
		      {
                if(l+4<query_length)
		  {

			data = substitution_matrix[j*ALPHABET_SIZE+query_sequence[l+4]];
		  }
		      }

		    else
		      { 
                if(l-4<query_length)
		  {

			data = substitution_matrix[j*ALPHABET_SIZE+query_sequence[l-4]];
	  }


		      }

                *(p++) = data;



#endif
            }
        }
    }
   
    return profile;
}


unsigned char *
create_profile_byte(unsigned char *      query_sequence,
                    int                  query_length,
                    unsigned char *      substitution_matrix,
                    unsigned char        bias)
{
    unsigned char *    profile;
    unsigned char *    p, *ptr;
    unsigned char      data;

    int                i,j,k,l;
    int                diff;
    int                padding;
    
    diff            = query_length & 15;
    padding         = 16 - diff;
    
    /* NB: VALLOC, to get memory aligned to page boundary */
    profile = (unsigned char *) valloc(sizeof(char)*ALPHABET_SIZE*(query_length+padding));
    
    p = profile;

    
    for(i=0;i<query_length;i+=16)
    {
        for(j=0;j<ALPHABET_SIZE;j++)
        {
            for(k=0;k<16;k++)
            {

#if TARGET_BYTE_1load_HFsave16th_var2 || TARGET_BYTE_1load_HFsave16th || TARGET_BYTE_1load


	      l = i+k;
	      if(l<query_length)
                {
		  data = substitution_matrix[j*ALPHABET_SIZE+query_sequence[l]];
                }
	      else
                {
		  data = 0;
                }
	      *(p++) = data;

#else

                l = i+k;
		data =0;
		    if(k<8)
		      {
                if(l+8<query_length)
		  {

			data = substitution_matrix[j*ALPHABET_SIZE+query_sequence[l+8]];
		  }
		      }

		    else
		      { 
                if(l-8<query_length)
		  {

			data = substitution_matrix[j*ALPHABET_SIZE+query_sequence[l-8]];
	  }


		      }

                *(p++) = data;
#endif
            }

        }
    }
    
    return profile;
}

