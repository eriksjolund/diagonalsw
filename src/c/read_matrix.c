#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "sw_constants.h"

#include "read_matrix.h"

unsigned char *
read_matrix(FILE * fp,
            unsigned char *   p_bias)
{
    char              buf[2048];
    char              ch;
    char *            p;
    int               i,j,k,tmp;
    int               nletters;
    char              horiz_letter[255];
    char              vert_letter[255];
    int *             tmpmatrix;
    unsigned char *   matrix;
    int               bias;
    int               dummy_res = ALPHABET_SIZE - 1;
    
    p = fgets(buf,2047,fp);
    
    /* skip initial comments */
    while ( p != NULL && (buf[0] == '#' || buf[0] == '/'))
    {
        p = fgets(buf,2047,fp);
    }
    
    /* Read horiz. letters */
    nletters = 0;
    
    for(i=0;i<strlen(buf);i++)
    {
        ch = buf[i];
        if(isspace(ch))
            continue;
        
        horiz_letter[nletters++] = toupper(ch) - 'A';
    }
    
    matrix    = (unsigned char *)malloc(sizeof(char)*ALPHABET_SIZE*ALPHABET_SIZE);
    tmpmatrix = (int *)malloc(sizeof(int)*ALPHABET_SIZE*ALPHABET_SIZE);
    
    for(i=0;i<ALPHABET_SIZE*ALPHABET_SIZE;i++)
            tmpmatrix[i] = 1234567;
    
    bias = 0;
    
    /* Read nletters lines */
    for(j=0;j<nletters;j++)
    {
        if(fscanf(fp," %c",&(vert_letter[j]))<1)
        {
            fprintf(stderr,"Fatal error - bad substitution matrix format.\n");
            exit(1);
        }
        
        vert_letter[j] = toupper(vert_letter[j]) - 'A';
        
        for(k=0;k<nletters;k++)
        {
            if(fscanf(fp,"%d",&tmp)<1)
            {
                fprintf(stderr,"Fatal error - bad substitution matrix format.\n");
                exit(1);
            }
            if( tmp > 127 || tmp < -127)
            {
                fprintf(stderr,"Fatal error - Matrix element out of range: %d\n",tmp);
                exit(1);
            }
            tmpmatrix[ vert_letter[j] * ALPHABET_SIZE + horiz_letter[k]] = (unsigned char) tmp;
            if (tmp < bias)
                bias = tmp;
        }
    }
    

    
    /* Add the bias */
    for(i=0;i<ALPHABET_SIZE*ALPHABET_SIZE;i++)
    {
        if(tmpmatrix[i]==1234567)
            tmpmatrix[i] = 0;
        
        matrix[i] = (unsigned char)(tmpmatrix[i] - bias);
    }
    
    // Neutral score for matching the dummy residue too
    for(i=0;i<ALPHABET_SIZE;i++)
    {
        matrix [ i*ALPHABET_SIZE + dummy_res ] = 0;
        matrix [ dummy_res*ALPHABET_SIZE + i ] = 0;
    }
    
    *p_bias = (unsigned char) -bias;

    return matrix;
}

