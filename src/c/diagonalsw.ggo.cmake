package "diagonalsw"
version "@PACKAGE_VERSION@"
purpose "A high-performance version of the Smith-Waterman alignment algorithm"

option "databasefile" d "A FASTA-formatted sequence database file" string typestr="filename" required
option "queryfile" q "A single sequence file in FASTA-format" string typestr="filename" optional
option "profilefile" p "A profile file" string typestr="filename" optional
option "matrixfile" s "Integer similarity matrix file ( scoring matrix )" string typestr="filename" required
option "gapopen" i "Penalty for starting alignment gap (non-positive integer)" int default="-10"  optional 
option "gapextend" e "Penalty for extending alignment gap (non-positive integer)" int default="-2" optional

option "print-sequence-id" u "In addition to the score, also print the sequence-id" flag off

option "verify-algorithm" V "verify vector implementation against scalar implementation" flag off
option "disable-threading" D "disable all threading" flag off

option "nr-of-threads" t "numbers of threads (normally autodetected)" int  optional
option "tokens-per-thread" T "numbers of tokens per thread (normally autodetected)" int default="5"  optional
option "sequences-per-compute-chunk" W  "nr of sequences each thread are given per computation iteration ( doesn't influence the result but may influence the speed )" int  default="10" optional



#option "skip-db_sequences" w "skip the first db sequences. Number to skip." int optional

section "benchmarking" sectiondesc="To get a more fair benchmark you should run a computation that lasts at least some minutes on a machine that is not under load by other processes."

option "benchmark" b "print computation speed (unit is GigaCells/s)" flag off     
option "benchmark-verbose" B "print computation speed (unit is GigaCells/s) and additional information such as user time, number of db sequences that needed to use the word sized function, length of querysequence, sum of db sequence lengths, number of db sequences" flag off     

section "limits" sectiondesc="the default limits should work fine in most cases. Warning: Increasing these values will increase the memory consumption of the program."



option "max-sequence-name-length" K "Maximum allowed length of a sequence name" default="256" int optional
option "max-query-sequence-length" L "Maximum allowed length of the query sequence" default="2000" int optional
option "max-db-sequence-length" M "Maximum allowed length of any of the db sequences" default="10000" int optional






