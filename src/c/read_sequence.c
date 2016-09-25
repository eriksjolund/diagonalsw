#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "sw_constants.h"
#include "read_sequence.h"

// the variable "name" will be null-terminated but the variable "sequence" will not be null-terminated

int
read_sequence(FILE *            fp,
              char *            name,
              unsigned char *   sequence,
              int               maxlength_name,
              int               maxlength_sequence )
{
    char    ch;
    char *  p;
    char    buf[65536];
    int     nchar;
    int     i;
    int     dummy_res     = ALPHABET_SIZE - 1;
    

    p = fgets(buf,65535,fp);
    
    if( p == NULL)
        return -1;
    
    if ( buf[0] != '>')
    {
        fprintf(stderr,"Fatal error - no sequence starting on this line:\n%s\n",buf);
        exit(1);
    }
    
    p = buf+1;
    while(isspace(*p))
        p++;
    strncpy(name,p,maxlength_name);
    name[maxlength_name+1]='\0';
    /* remove trailing newline and whitespace */
    p = name+strlen(name) - 1;
    
    while(p > name && isspace(*p))
        *(p--) = '\0';
        
    nchar = 0;
    while ( ((p = fgets(buf,65535,fp)) != NULL) && buf[0] != '>')
    {
        i=0;
        while( (ch=*p++) != '\0' )
        {
            if( ch >= 'A' && ch <= 'Z' )
                sequence[nchar++] = ch - 'A';
            else if (ch >= 'a' && ch <= 'z')
                sequence[nchar++] = ch - 'a';
            
            if(nchar == ( maxlength_sequence + 15 ) )
            {
	      fprintf(stderr,"Fatal error - sequence too long, maxlength = %d\n",  maxlength_sequence );
                exit(1);
            }
        }
    }
    
    if(buf[0] == '>')
    {
        fseek(fp,-strlen(buf),SEEK_CUR);
    }
    
    // Fill sequence with dummy characters 
    for(i=nchar;i<nchar+15;i++)
        sequence[i] = dummy_res;
    
    return nchar;    
}
