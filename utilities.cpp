#include "utilities.h"

char* text_file_to_char(const char* file_name){
     FILE *fp;
     long lSize;
     char *buffer;

     fp = fopen ( file_name , "rb" );
     if( !fp ){
          printf("File could not be opened\n" );
          exit(1);
     }

     fseek( fp , 0L , SEEK_END);
     lSize = ftell( fp );
     rewind( fp );


     buffer = (char *)calloc( 1, lSize+1 );
     if( !buffer ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);


     if( 1!=fread( buffer , lSize, 1 , fp) )
       fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);

     fclose(fp);
     return buffer;
}
