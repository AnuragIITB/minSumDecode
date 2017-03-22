#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include<stdbool.h>

#include "compute.h"


int main(int argc, char* argv[])
{
	  if( argc< 5)
		{
		fprintf(stderr, "ERROR: not sufficient arguments \n");
		return(-1);
		}
  
	  ParityCheckMatrix pm;
	  int read_status = readMatrix(argv[1], &pm);
	  if ( read_status !=0 )
		{
		fprintf(stderr, "ERROR: file read fail %s\n", argv[1]);
		return(-1);
		}
//	printf ( "ParityCheckMatrix : %d, %d. %d, %d, %d", pm.nrows, pm.ncols, pm.ncol_ind, pm.col_ind[0], pm.row_ptr[0]);
	int max_nitr;
	max_nitr = atoi( argv[4] );
		
	int nitr_req;
//
//	reading code block 
//	
	int block_count = 0 ;
	FILE *in_code;
	in_code = fopen( argv[2], "r");

	
	int total_incorrect_bits=0;
	double total_time = 0;
	int correctly_decoded_block = 0;
	int tot_itr = 0 ; 
		
	while(1)
		{
		double code_block[pm.ncols];
		int read_status;
		
		read_status = 	readCodeBlock ( &pm, in_code , code_block);
		if (read_status != 0)
			{
			printf( "\t\t\tINFO: End of decoding.\n");
			break;
			}
		block_count++;
//		printf("\t\t\tBlock Count \t\t%d\n", block_count);
		
			
//
// decoding block 
//
		struct timespec tstart={0,0}, tend={0,0};
		clock_gettime(CLOCK_MONOTONIC, &tstart);
		nitr_req = minSumDecode( max_nitr , & pm , code_block ,atof( argv[5]) );
		tot_itr += nitr_req ;
		clock_gettime(CLOCK_MONOTONIC, &tend);

// BPSk demodulation
		bpskdemodulation( pm.ncols ,  code_block );
//
// printing output to file :
//
		FILE* decode_file;
		uint16_t j;
		decode_file = fopen ( "../include/decodedBlock/decodedOutput.txt" , "w");
		for (j = 0; j < pm.ncols ; j++)
			{
			fprintf( decode_file , "%lf\n" , code_block[j]);
			}
		fclose(decode_file);
//		printf( "INFO: Output decoded code word written to ../include/decodedOutput.txt \n");
//
// argv[3]		: output
// -time 		command line output shows time to decode the code block; termed as "time mode".
// -accuracy		command line output shows number of incorrect bits after decoding of  the code block; termed as "accuracy mode".
//	
		
		if ( strcmp(argv[3],"-time")==0)
			{
			double time_taken = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -  
		           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
			printf("\t\t\t-----------------------------------------------------------------------------------------------------\n \
			Block Number\t %d \t Time to decode \t %.5f seconds \t iteration required \t %d \t\n  \
			------------------------------------------------------------------------------------------------------\n", block_count,
		        time_taken, nitr_req) ;
			total_time += time_taken;
			}

		if ( strcmp(argv[3],"-accuracy")==0)
			{
			int incorrect_bits;
			incorrect_bits = findAccuracy( &pm );	
			if ( incorrect_bits == 0)
				{
				correctly_decoded_block ++;				
				}
			printf("\t\t\t----------------------------------------------------------\n \
			Block Number\t %d \t Number of incorrect bits \t %d \n  \
			----------------------------------------------------------\n",
			block_count,incorrect_bits); 
			total_incorrect_bits += incorrect_bits ;		
			}

	
		}
		long double BER ;
		BER = ((long double)total_incorrect_bits/(long double)block_count)/(long double)pm.ncols ;
			if ( strcmp(argv[3],"-time")==0)
			{
			
			printf("\t\t\t----------------------------------------------------------------------------------\n \
			Total block count\t %d \n \
			Average time to decode \t %.5f seconds \n \
			Avg iteration per block \t %f \n  \
			-----------------------------------------------------------------------------------\n", block_count,
		        (total_time/block_count),((float)tot_itr/block_count) ) ;
			
			}

			if ( strcmp(argv[3],"-accuracy")==0)
			{
			
			printf("\t\t\t----------------------------------------------------------\n \
			Total block count\t \t \t %d \n \
			correctly decoded blocks \t \t %d \n \
			Total number of incorrect bits \t \t %d \n  \
			BER \t \t \t \t %0.9Lf \n  \
			----------------------------------------------------------\n",
			block_count, correctly_decoded_block, total_incorrect_bits, BER ); 		
			}

		
	return 0;

}





