#include "utilities.h"

namespace def{
	char* text_file_to_char(const char* file_name){
	     FILE *fp;
	     long lSize;
	     char *buffer;

	     fopen_s (&fp, file_name , "rb" );
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

	int check_if_file_exists(const char * filename)
	{
	    FILE *file;
	    if (fopen_s(&file, filename, "r") == 0)
	    {
	        fclose(file);
	        return 1;
	    }

	    return 0;
	}

	int get_word_count(const char *string){
		int count = 1;
		for(int i = 1; string[i] != 0; i++){
			if(string[i - 1] == ' ' && string[i] == ' ') continue;
			else if(string[i - 1] != ' ' && string[i] == ' ') count++;
		}
		return count;
	}

	void get_first_two_words(const char *string, char *first, char *second){
		int index = 0;
		for(int i = 0 ; string[i] != 0; i++, index++){
			if(string[index - 1] != ' ' && string[index] == ' '){
				first[i] = 0;
				index++;
				break;
			}
			else{
				first[i] = string[index];
			}
		}
		
		for(int i = 0 ; string[i] != 0; i++, index++){
			if(string[index - 1] != ' ' && string[index] == ' '){
				second[i] = 0;
				break;
			}
			else{
				second[i] = string[index];
			}
		}
	}
}
