#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


#include "sw_constants.h"
#include "read_profile.h"


int
read_profile(FILE *fp,
     int *query_length,
     unsigned char *bias,
     int max_query_sequence_length,
	    unsigned char *profile_byte,
      unsigned short *profile_word
)
{

  /*
   This function is not yet ready. It still needs some work. Erik Sjolund /2009-11-25
   */
  fprintf(stderr, "error: the parser function read_profile() is not yet functionable.\n" ); exit(1);

    char    ch;
    char    not_used_char;
    int     not_used_int;
    int     nletters;
    char *  p;
    char    buf[2048];
    int     nchar;
    int     i,j;
    int     dummy_res     = ALPHABET_SIZE - 1;
    char              horiz_letter[255];
    char              vert_letter[255];

    buf[2047]='\0';    
    char secondline[]="Last position-specific scoring matrix computed";
    char thirdline[]="           A  R  N  D  C  Q  E  G  H  I  L  K  M  F  P  S  T  W  Y  V   A   R   N   D   C   Q   E   G   H   I   L   K   M   F   P   S   T   W   Y   V";
    p = fgets(buf,2047,fp);
    if( p == NULL)
        return -1;
    if ( strcmp(buf,"\n") != 0 )      {
      fprintf(stderr, "error: the first line in the profile file should be empty\n" );
      return -1;
}
    p = fgets(buf,2047,fp);
    if( p == NULL)
        return -1;

    if ( strncmp(buf,secondline,strlen(secondline)) != 0 )      {
      fprintf(stderr, "error: the second line in the profile file should be start with: %s\n",secondline );
      return -1;
}
    p = fgets(buf,2047,fp);
    if( p == NULL)
        return -1;
    if ( strncmp(buf,thirdline,strlen(thirdline)) != 0 )      {
      fprintf(stderr, "warning: the third line in the profile file should probably be start with: %s\n",thirdline );
}
    nletters = 0;
    
    for(i=0;i<strlen(buf);i++)
    {
        ch = buf[i];
        if(isspace(ch))
            continue;

	// The letters seem to be repeated twice
        if ( ch == horiz_letter[0] )
	    break;

        horiz_letter[nletters++] = toupper(ch) - 'A';
    }



       if(fscanf(fp," %c",&not_used_char)<1)
        return -1;


       if(fscanf(fp," %d",&not_used_int)<1)
        return -1;
       
       for(j=0;j<nletters;j++)
	 {

	   int letterInt;
	   if(fscanf(fp," %d", &letterInt)<1) {
      fprintf(stderr, "error: the second line in the profile file should be start with: %s\n",secondline );
      return -1;
	   }
	   if( letterInt<0 || letterInt > 256 ) {
      fprintf(stderr, "error: the number %d was out of range\n",letterInt );
      return -1;
	   } else {
	     vert_letter[j]= letterInt;
   }




}
       if(fscanf(fp," %d",&not_used_int)<1)
        return -1;

       while ( ((p = fgets(buf,2047,fp)) != NULL) && buf[0] != '>')











    while(isspace(*p))
        p++;




    while ( ((p = fgets(buf,2047,fp)) != NULL) && buf[0] != '>')
    {
        i=0;
        while( (ch=*p++) != '\0' )
        {
	  /*            if( ch >= 'A' && ch <= 'Z' )
	      //                sequence[nchar++] = ch - 'A';
            else if (ch >= 'a' && ch <= 'z')
                sequence[nchar++] = ch - 'a';

            if(nchar == ( maxlength_sequence + 15 ) )
            {
	      fprintf(stderr,"Fatal error - sequence too long, maxlength = %d\n",  maxlength_sequence );
                exit(1);
            }
	  */
        }
    }


    p = buf+1;


    //    strncpy(name,p,maxlength_name);
    //   name[maxlength_name+1]='\0';
    /* remove trailing newline and whitespace */
    //  p = name+strlen(name) - 1;
    
    //    while(p > name && isspace(*p))
    //   *(p--) = '\0';
        
    nchar = 0;


    
    if(buf[0] == '>')
    {
        fseek(fp,-strlen(buf),SEEK_CUR);
    }
    
    // Fill sequence with dummy characters 
    /*
    for(i=nchar;i<nchar+15;i++)
        sequence[i] = dummy_res;
    */
    return nchar;    
}





