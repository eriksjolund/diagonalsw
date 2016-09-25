/* 
 * A high-performance version of the Smith-Waterman alignment
 * algorithm, implemented with the PowerPC Altivec/VMX instruction set.
 *
 * This file is a trivial command-line program that processes options and
 * calls the actual interface to score a sequence against a database
 * of FASTA-formatted sequences and return the best alignments as well
 * as probability scores.
 */

#include <cstdio>
#include <cstdlib>

#include <sys/time.h>
#include <sys/resource.h>

#include <cerrno>
#include <cstring>

#include "tbb/tick_count.h"
#include "tbb/pipeline.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_allocator.h"
//#include "tbb/cache_aligned_allocator.h"

#include "read_matrix.h"
#include "create_profile.h"
#include "read_profile.h"
#include "read_sequence.h"
#include "smith_waterman_reference_impl.h"
#include "smith_waterman_vector.h"
#include "cmdline_diagonalsw.h"
#include "config.h"

enum NeededWordVersion { yes, no};
enum PrintOutput { onlyScore, nothing, bothScoreAndSeqID };

class SeqData {
 public:
  static SeqData * construct( char *mem, int max_db_sequence_length, int max_sequence_name_length )  {
    SeqData *s = reinterpret_cast<SeqData *>(mem); 
    mem += sizeof(SeqData);
    s->db_seq_name =  static_cast<char *>(mem); 
    mem += ( max_sequence_name_length + 1 ) * sizeof(char *);
    s->db_seq =  reinterpret_cast<unsigned char *>(mem); 
    return s;
  }
  static size_t size( int max_db_sequence_length, int max_sequence_name_length )  { size_t size = sizeof(SeqData)   + max_db_sequence_length*sizeof(unsigned char *) + (max_sequence_name_length+1) * sizeof(char *); return size; }
  int score;
  char *seq_name;
  unsigned char *db_seq;
  char *db_seq_name;
 
  int db_length;
  long sequence_num_in_file;
  NeededWordVersion  needed_word_version;
};

class TokenData {
 public:
  static TokenData * construct( int max_db_sequence_length, int max_sequence_name_length, int sequences_per_compute_chunk )  {
    size_t seq_data_size = SeqData::size(max_db_sequence_length, max_sequence_name_length);
    size_t indata_size = sequences_per_compute_chunk * ( seq_data_size + sizeof(SeqData *));
    size_t needed_for_byte = workspace_needed_for_byte_version( max_db_sequence_length);
    size_t needed_for_word = workspace_needed_for_word_version( max_db_sequence_length);
    size_t workspace_size = needed_for_word > needed_for_byte ? needed_for_word : needed_for_byte;
    const int align = 16;
    size_t tot =  sizeof(TokenData) + indata_size + workspace_size + ( align - 1 );
    char * buff = tbb::tbb_allocator<char>().allocate( tot );
    //    char * buff = tbb::cache_aligned_allocator<char>().allocate( tot );
    if ( buff == NULL )  { fprintf(stderr,"error: could not allocate memory for the compute chunk\n"); exit(1); }
    TokenData* t = reinterpret_cast< TokenData *>(buff);;
    t->size = tot;
    buff +=  sizeof(TokenData) ;
    t->seq_data_ptr_array = reinterpret_cast< SeqData **>(buff);
    buff += sequences_per_compute_chunk * sizeof(SeqData *);
    for (int i=0; i< sequences_per_compute_chunk; ++i) {
      t->seq_data_ptr_array[i]=SeqData::construct(buff,max_db_sequence_length, max_sequence_name_length);
      buff += seq_data_size;
    }
    char * workspace_unaligned = buff;
    // SSE needs 16-byte aligned memory. Does the next line work on all platforms?
    t->workspace =    ( char *)  (((size_t)workspace_unaligned + align) & ~((size_t) ( align-1 )));
    return t;
  }

  void destruct() {
    //    tbb::cache_aligned_allocator<char>().deallocate((char*)this,size);
    tbb::tbb_allocator<char>().deallocate((char*)this,size);
  } 
  char *workspace;
  SeqData** seq_data_ptr_array;
  size_t size;
  int num_sequences;
};


void printScore( PrintOutput print_output, int score, char * seq_id ) 
{ 
  switch ( print_output ) {
  case onlyScore: fprintf( stdout ,"%i\n", score ); break;
  case bothScoreAndSeqID: fprintf( stdout ,"%i %s\n", score, seq_id ); break;
  case nothing: break;
  }
};

int smith_waterman(  unsigned char *query_seq,
		     unsigned char *query_profile_byte,
		     unsigned short *query_profile_word,
		     int query_length,
		     unsigned char *db_seq,
		     char *db_seq_name,
		     int db_length,
		     unsigned char bias,
		     unsigned char gap_open,
		     unsigned char gap_extend,
		     char *workspace,
		     int verify_algorithm,
		     unsigned char * substitution_matrix,
		     NeededWordVersion * needed_word_version)
{
  *needed_word_version = no;
  int score = smith_waterman_vector_byte
    (query_seq,
     query_profile_byte,
     query_length,
     db_seq,
     db_length,
     bias,
     gap_open,
     gap_extend,
     (unsigned char *)workspace);

  if ( score == -1 ) { 
    *needed_word_version = yes;
    score =  smith_waterman_vector_word
      (query_seq,
       query_profile_word,
       query_length,
       db_seq,
       db_length,
       bias,
       gap_open,
       gap_extend,
       (unsigned short *)workspace);
  } 

  if ( verify_algorithm ) {
    int cells = query_length * db_length;
    //              unsigned short * tmpAllocated =  ( unsigned short * ) malloc( cells * 6 * ( sizeof( unsigned short ) ) ); 
    //	      size_t tot = cells * 6 * ( sizeof( unsigned short ) ) ;
    int tot = cells * 6 ;
    //	                    unsigned short * tmpAllocated =  ( unsigned short * )   tbb::tbb_allocator<char>().allocate( tot );
    unsigned short * tmpAllocated =  ( unsigned short * )   tbb::tbb_allocator<unsigned short>().allocate( tot );
    int it;
    for (it=0;it<( cells * 6 ); ++it) {  tmpAllocated[it]=0; }
    if ( tmpAllocated == NULL )  { fprintf(stderr,"error: could not allocate memory for the reference matrices (E,F,H)\n"); exit(1); }
    unsigned short * Eref = tmpAllocated;
    unsigned short * Fref = tmpAllocated + cells;
    unsigned short * Href = tmpAllocated + 2*cells;
    unsigned short * E = tmpAllocated + 3*cells;
    unsigned short * F = tmpAllocated + 4*cells;
    unsigned short * H = tmpAllocated + 5*cells;
    int bytescore = smith_waterman_vector_byte_EFH
      (query_seq,
       query_profile_byte,
       query_length,
       db_seq,
       db_length,
       bias,
       gap_open,
       gap_extend,
       (unsigned char *)workspace,
       E,
       F,
       H);
    int wordscore =  smith_waterman_vector_word_EFH
      (query_seq,
       query_profile_word,
       query_length,
       db_seq,
       db_length,
       bias,
       gap_open,
       gap_extend,
       (unsigned short *)workspace,
       E,
       F,
       H);
    int  refscore = smith_waterman_reference_impl
      (query_seq,
       query_length,
       db_seq,
       db_length,
       bias,
       gap_open,
       gap_extend,
       substitution_matrix, 
       Eref, 
       Fref, 
       Href );
    int error_found = 0;
    if ( refscore >= 255 ) {  
      if ( bytescore != -1  ) {
	fprintf(stderr, "error: refscore >= 255 and vector score( byteversion ) is not -1\n");  	   
	error_found = 1;
      }
    } else   {
      if ( bytescore != refscore  ) {
	fprintf(stderr, "error: refscore < 255 and vector score( byteversion ) != refscore\n");  	   
	error_found = 1;
      }
    }
    if ( wordscore != refscore  ) {
      fprintf(stderr, "error: vector score( wordversion ) != refscore\n");  	   
      error_found = 1;
    }
    if ( error_found ) {
      fprintf( stderr, "error: scalar score=%i,  vector score(byte)=%i, vector score(word)=%i\n", refscore, bytescore, wordscore); 
    }
    int matrixmismatch = 0;
    if (! vectorMatrixIsOk(E,Eref,"E",query_length,db_length)) { matrixmismatch = 1; };
    if (! vectorMatrixIsOk(F,Fref,"F",query_length,db_length)) { matrixmismatch = 1; };
    if (! vectorMatrixIsOk(H,Href,"H",query_length,db_length)) { matrixmismatch = 1; };
    if ( matrixmismatch ) { printf("scalar score=%i\nvector score=%i\ndb db_seq_name=\"%s\"\nquery_length=%i\ndb_length=%i\n",refscore,score,db_seq_name,query_length,db_length); exit(2); }
    tbb::tbb_allocator<unsigned short>().deallocate((unsigned short*)tmpAllocated,tot);
  };
  return score;
}

class ReadDatabaseFilter: public tbb::filter {
 public:
  ReadDatabaseFilter( FILE* fp_db, int  max_db_sequence_length, int max_sequence_name_length, int _sequences_per_compute_chunk);
  long db_length_sum( ){return db_length_sum_;}
  long num_read_sequences( ){return num_read_sequences_;}
 private:
  void* operator()( void * );
  int max_db_sequence_length;
  int max_sequence_name_length;
  FILE* fp_db;
  int     sequences_per_compute_chunk;
  long db_length_sum_;
  long num_read_sequences_;
};

ReadDatabaseFilter::ReadDatabaseFilter( FILE * _fp_db, int _max_db_sequence_length, int _max_sequence_name_length, int _sequences_per_compute_chunk ) : 
tbb::filter(serial_in_order),
  fp_db(_fp_db),
  max_db_sequence_length(_max_db_sequence_length), 
  max_sequence_name_length(_max_sequence_name_length),
  sequences_per_compute_chunk( _sequences_per_compute_chunk),
  db_length_sum_(0),
  num_read_sequences_(0)
{ 
}

void* ReadDatabaseFilter::operator()( void * ) {
  TokenData * chunk = TokenData::construct( max_db_sequence_length,max_sequence_name_length, sequences_per_compute_chunk );
  chunk->num_sequences = 0;
  for ( int i=0; i < sequences_per_compute_chunk; ++i) {
    SeqData * seq_data =  (chunk->seq_data_ptr_array[i]);
    if (( seq_data->db_length = read_sequence( fp_db, seq_data->db_seq_name, seq_data->db_seq, max_sequence_name_length, max_db_sequence_length )) > 0 ) {
      chunk->num_sequences += 1;         
      ++num_read_sequences_;
      db_length_sum_ += seq_data->db_length;
      seq_data->sequence_num_in_file = num_read_sequences_;
    }  else { break; }
  }
  if ( chunk->num_sequences == 0 ) { 
    return NULL; }
  for ( int i=0; i <   chunk->num_sequences ; ++i)    {
    SeqData * seq_data =  (chunk->seq_data_ptr_array[i]);
  }
  return chunk;
} 

class SmithWatermanFilter: public tbb::filter {
 public:
  SmithWatermanFilter(  unsigned char bias,  unsigned char gap_open,  unsigned char gap_extend , unsigned char *      query_profile_byte,    unsigned short *     query_profile_word, unsigned char * _query_seq  , int query_length,   unsigned char *      substitution_matrix,  unsigned int verify_algorithm
			);
  ~SmithWatermanFilter(  );
  void* operator()( void* item );
  unsigned char bias, gap_open, gap_extend;
  unsigned short *     query_profile_word;
  unsigned char *      query_profile_byte;
  unsigned char *     query_seq;
  unsigned int verify_algorithm_;
  int 	query_length;
  unsigned char *      substitution_matrix;
};

SmithWatermanFilter::SmithWatermanFilter(   unsigned char _bias,  unsigned char _gap_open,  unsigned char _gap_extend , unsigned char * _query_profile_byte,    unsigned short *  _query_profile_word,     unsigned char *     _query_seq,  int _query_length,      unsigned char *      _substitution_matrix, unsigned int verify_algorithm_ ) : 
tbb::filter(parallel), 
  bias(_bias),
  gap_open(_gap_open),
  query_seq( _query_seq ),
  gap_extend(_gap_extend), 
  query_profile_byte(_query_profile_byte),
  query_profile_word(_query_profile_word),
  query_length(_query_length), 	
  substitution_matrix(_substitution_matrix), 
  verify_algorithm_(verify_algorithm_)
{}  

SmithWatermanFilter::~SmithWatermanFilter(  )   { } 
void* SmithWatermanFilter::operator()( void* item ) {
  TokenData& b = *static_cast<TokenData*>(item);
  for (int i = 0 ; i <  b.num_sequences; ++i ) {
    SeqData & seq_data =  * b.seq_data_ptr_array[i];
    int score;
    NeededWordVersion needed_word_version;
    seq_data.score = smith_waterman(query_seq,
				    query_profile_byte,
				    query_profile_word,
				    query_length,
				    seq_data.db_seq,
				    seq_data.db_seq_name,
				    seq_data.db_length,
				    bias,
				    gap_open,
				    gap_extend,
				    b.workspace, 
				    verify_algorithm_, 
				    substitution_matrix, 
				    &(seq_data.needed_word_version));
  }
  return &b;  
}
      

class ResultFilter: public tbb::filter {
 public:
  ResultFilter( PrintOutput print_output );
  void* operator()(void* item);
  long num_needed_word_version() {
    return  num_needed_word_version_;
  };
 private:
  long num_needed_word_version_;
  PrintOutput print_output;
};

ResultFilter::ResultFilter( PrintOutput _print_output ): 
tbb::filter(serial_in_order), 
  num_needed_word_version_(0),
  print_output( _print_output )
{}

void* ResultFilter::operator()(void* item) {
  TokenData& b = *static_cast<TokenData*>(item);
  for (int i = 0 ; i <  b.num_sequences; ++i ) {
    SeqData & seq_data =  * b.seq_data_ptr_array[i];
    if (    seq_data.needed_word_version == yes ) 
      { ++num_needed_word_version_;  }
    printScore( print_output, seq_data.score, seq_data.db_seq_name );
  }
  b.destruct();
  return NULL;
}

void smith_waterman_pipeline( int max_num_of_live_tokens, FILE * fp_db, struct gengetopt_args_info  & args_info,     unsigned char bias,    unsigned char gap_open,     unsigned char gap_extend ,    unsigned char *  query_profile_byte,     unsigned short *  query_profile_word,    unsigned char *   query_seq, int  query_length,     unsigned char *  substitution_matrix,PrintOutput print_output, long *db_length_sum, long *num_needed_word_version, long *num_read_sequences )
{
  tbb::pipeline pipeline;
  ReadDatabaseFilter filter_in( fp_db, args_info.max_db_sequence_length_arg, args_info.max_sequence_name_length_arg, args_info.sequences_per_compute_chunk_arg );
  pipeline.add_filter( filter_in );
  SmithWatermanFilter filter_sw( bias, gap_open,  gap_extend , query_profile_byte,  query_profile_word, query_seq,  query_length, substitution_matrix, args_info.verify_algorithm_given ) ;
  pipeline.add_filter( filter_sw );
  ResultFilter filter_result( print_output );
  pipeline.add_filter( filter_result );
  pipeline.run(max_num_of_live_tokens);
  *db_length_sum = filter_in.db_length_sum();
  *num_needed_word_version = filter_result.num_needed_word_version();
  *num_read_sequences = filter_in.num_read_sequences();
  pipeline.clear(); 
  return;
};

void    validate_command_line_parameters(struct gengetopt_args_info &args_info)
{
  if ( ( args_info.profilefile_given && args_info.queryfile_given ) ||
       ( ! args_info.profilefile_given && ! args_info.queryfile_given ) ) {
    fprintf(stderr,"error: either --queryfile or --profilefile should be used\n"); exit(1);
  }
  if ( args_info.profilefile_given ) 
  {
    fprintf(stderr,"error: Sorry. The profile parsing function read_profile() is not yet implemented\n"); exit(1);
  }
  if ( args_info.sequences_per_compute_chunk_arg <= 0 ) { 
    fprintf(stderr,"error: --sequences-per-compute-chunk must be positive\n"); 
    exit(1); 
  }
  if ( args_info.gapopen_arg > 0 ) {
    fprintf(stderr,"error: --gapopen must not be positive\n"); 
    exit(1); 
  }
  if ( args_info.gapextend_arg > 0 ) {
    fprintf(stderr,"error: --gapextend must not be positive\n");
    exit(1); 
  }
  if ( args_info.gapopen_arg < -255 ) { 
    fprintf(stderr,"error: --gapopen must be bigger than -256\n"); 
    exit(1); 
  }
  if ( args_info.gapextend_arg < -255 ) {
    fprintf(stderr,"error: --gapextend must be bigger than -256\n"); 
    exit(1); 
  }
  if (( args_info.gapextend_arg +  args_info.gapopen_arg ) < -255 ) {
    fprintf(stderr,"error: the sum of --gapopen and --gapextend must be bigger than -256\n"); 
    exit(1); 
  }
  if ( args_info.max_sequence_name_length_arg <= 0 ) {
    fprintf(stderr,"error: --max_sequence_name_length must be positive\n"); 
    exit(1); 
  }
  if ( args_info.max_query_sequence_length_arg <= 0 ) {
    fprintf(stderr,"error: --max-query-sequence-length must be positive\n");
    exit(1); 
  }
  if ( args_info.max_db_sequence_length_arg <= 0 ) { 
    fprintf(stderr,"error: --nax-db-sequence-length must be positive\n"); 
    exit(1); 
  }
  if ( args_info.disable_threading_given && 
       ( args_info.nr_of_threads_given || args_info.sequences_per_compute_chunk_given 
         || args_info.tokens_per_thread_given )
       ) {
    fprintf(stderr,"error: --disable_threading can't be combined with --nr-of-threads or --sequences-per-compute-chunk or --tokens-per-thread\n"); 
    exit(1);
  }
  {
    int non_compatible_args = 0;
    if ( args_info.verify_algorithm_given ) {
      ++non_compatible_args; 
    };
    if ( args_info.benchmark_verbose_given ) { 
      ++non_compatible_args; 
    };
    if ( args_info.benchmark_given )  {
      ++non_compatible_args; 
    };
    if ( non_compatible_args > 1 ) { 
      fprintf(stderr,"error: the option flags --benchmark and --verify_algorithm and --verify_verbose_algorithm can't be combined\n");
      exit(1);
    }
  }
  if ( args_info.nr_of_threads_given && args_info.nr_of_threads_arg < 1 )  {
    fprintf(stderr,"error: --nr-of-threads must be a positive integer\n"); 
    exit(1); 
  }
}

int
main (int          argc,
      char **      argv)
{
  FILE *               fp_query;
  FILE *               fp_db;
  FILE *               fp_matrix;
  FILE *               fp_profile;

  unsigned char *      query_seq;
  unsigned char *      db_seq;
  unsigned short *     query_profile_word;
  unsigned char *      query_profile_byte;
  int                  query_length;
  int                  db_length;
  unsigned char *      substitution_matrix;
  unsigned char bias, gap_open, gap_extend;
  int i;

  struct gengetopt_args_info args_info;

  tbb::tick_count tick_count_start = tbb::tick_count::now();

  if(cmdline_parser(argc, argv, &args_info) != 0) {
    exit(1);
  }
  validate_command_line_parameters(args_info);
  PrintOutput print_output;
  if ( args_info.benchmark_given || args_info.benchmark_verbose_given ) {
    print_output = nothing; 
  } else {
    if ( args_info.print_sequence_id_given ) {
      print_output = bothScoreAndSeqID;     
    } else {
      print_output = onlyScore;     
    }
  }

  char query_seq_name[ args_info.max_sequence_name_length_arg + 1 ];
  gap_open   = (unsigned char) -args_info.gapopen_arg;
  gap_extend = (unsigned char) -args_info.gapextend_arg;
  gap_open += gap_extend;
    
  query_seq = (unsigned char *)valloc( args_info.max_query_sequence_length_arg + 15 ); // 15 extra dummy characters will be appended.
  if ( query_seq == NULL )  { fprintf(stderr,"error: could not allocate memory for the query sequence name\n"); exit(1); }
    
  /* get query sequence */

  if ( args_info.queryfile_given ) {
  if ((fp_query = fopen(args_info.queryfile_arg,"r")) == NULL) {
    fprintf(stderr,"Cannot open file: %s : %s\n", args_info.queryfile_arg, strerror(errno)); exit(1); }

  query_length = read_sequence(fp_query,query_seq_name,query_seq, args_info.max_sequence_name_length_arg, args_info.max_query_sequence_length_arg);
  fclose(fp_query);

  /* substitution matrix */

  if ((fp_matrix = fopen(args_info.matrixfile_arg,"r")) == NULL) {
    fprintf(stderr,"Cannot open file: %s : %s\n",args_info.matrixfile_arg, strerror(errno)); exit(1); }

  substitution_matrix = read_matrix(fp_matrix,&bias);
  
  /* make our profile */
  query_profile_byte = create_profile_byte(query_seq,query_length,substitution_matrix,bias);
  query_profile_word = create_profile_word(query_seq,query_length,substitution_matrix,bias);
  } else {  

    if ( args_info.profilefile_given ) {
      if ((fp_profile = fopen(args_info.profilefile_arg,"r")) == NULL) {
	fprintf(stderr,"Cannot open file: %s : %s\n",args_info.profilefile_arg, strerror(errno)); 
	exit(1); 
      }
      int res = read_profile(fp_profile, &query_length, &bias, args_info.max_query_sequence_length_arg, query_profile_byte, query_profile_word );
      if ( res == -1 ) {
	fprintf(stderr,"error: could not read profile\n"); 
	exit(1); 
      }


    } else {
      fprintf(stderr,"error: either --profilefile or --queryfile must be specified\n"); 
      exit(1); 
    }
  }


  
  if ((fp_db = fopen(args_info.databasefile_arg,"r")) == NULL) {
    fprintf(stderr,"Cannot open file: %s : %s\n", args_info.databasefile_arg, strerror(errno)); exit(1); }

  long db_length_sum = 0;
  long num_read_sequences = 0;
  long num_needed_word_version = 0;

  if ( args_info.disable_threading_given ) {
    char db_seq_name[ args_info.max_sequence_name_length_arg + 1 ];
    size_t needed_for_byte = workspace_needed_for_byte_version( args_info.max_db_sequence_length_arg);
    size_t needed_for_word = workspace_needed_for_word_version( args_info.max_db_sequence_length_arg);
    size_t workspace_size = needed_for_word > needed_for_byte ? needed_for_word : needed_for_byte;
    char *workspace;
    if (( workspace = (char *)valloc(  workspace_size )) == NULL ) { fprintf(stderr,"error: could not allocate enough memory\n"); exit(1); };
    db_seq    = (unsigned char *)valloc( args_info.max_db_sequence_length_arg + 15 ); // In this case unneccessary, but also here 15 extra dummy characters will be appended. 
    if ( db_seq == NULL )  { fprintf(stderr,"error: could not allocate memory for the db sequence name\n"); exit(1); }
    while((db_length = read_sequence(fp_db, db_seq_name, db_seq, args_info.max_sequence_name_length_arg, args_info.max_db_sequence_length_arg ))>0)
      {
	++num_read_sequences;
	db_length_sum += db_length;
    
	NeededWordVersion needed_word_version;
	int score = smith_waterman( query_seq,
				    query_profile_byte,
				    query_profile_word,
				    query_length,
				    db_seq,
				    db_seq_name,
				    db_length,
				    bias,
				    gap_open,
				    gap_extend,
				    workspace, 
				    args_info.verify_algorithm_given, 
				    substitution_matrix, 
				    &needed_word_version);

	if ( needed_word_version == yes ) { 
	  ++num_needed_word_version;
	}
	printScore( print_output, score, db_seq_name );
      }
    free(db_seq);
    free(workspace);
  } else { 

    if ( args_info.nr_of_threads_given ) {  
      tbb::task_scheduler_init tsched(args_info.nr_of_threads_arg);
      int max_num_of_live_tokens = args_info.tokens_per_thread_arg * args_info.nr_of_threads_arg;
      smith_waterman_pipeline( max_num_of_live_tokens, fp_db, args_info, bias, gap_open,  gap_extend , query_profile_byte,  query_profile_word, query_seq,  query_length, substitution_matrix, print_output, &db_length_sum, &num_needed_word_version, &num_read_sequences );
    } 
    else {
      tbb::task_scheduler_init tsched;
      int max_num_of_live_tokens = args_info.tokens_per_thread_arg * tsched.default_num_threads();
      smith_waterman_pipeline( max_num_of_live_tokens, fp_db, args_info, bias, gap_open,  gap_extend , query_profile_byte,  query_profile_word, query_seq,  query_length, substitution_matrix, print_output, &db_length_sum, &num_needed_word_version, &num_read_sequences );
    }
    fclose(fp_db);
  }
  tbb::tick_count::interval_t elapsed_ticks = tbb::tick_count::now() - tick_count_start ;
  double elapsed_seconds = elapsed_ticks.seconds();

  if ( args_info.benchmark_given || args_info.benchmark_verbose_given ) { 
    double speed = ( ( long ) query_length * db_length_sum ) / ( elapsed_seconds * 1000000000.0 );
    if ( args_info.benchmark_verbose_given ) { 
      printf("time = %g\n", elapsed_seconds);
      printf("nr of db sequences that needed to use the word sized function due to an overflow in the byte sized function = %ld\n",num_needed_word_version);
      printf("nr of db sequences = %ld\n",num_read_sequences);          
      printf("computation speed ( Giga cells/second ) = %f\n",speed);
    } else {
      printf("%f\n",speed);
    }
  }
  return 0;
}
