<?xml version="1.0"?>
<article  
 id="manual">
  <articleinfo>
    <title>diagonalsw</title>
  </articleinfo>
  <sect1 id="introduction">
    <title>Introduction</title>
    <para>
diagonalsw is a software that implements the <ulink url="http://en.wikipedia.org/wiki/Smith-Waterman_algorithm">Smith-Waterman algorithm</ulink> with the SIMD instruction sets found in modern CPUs ( <ulink url="">SSE4.1</ulink> for the x86 platform and <ulink url="http://en.wikipedia.org/wiki/AltiVec">AltiVec</ulink> for the PowerPC platform ). 

 The software is open source software. It is written in the programming languages C and C++ and it is licensed under the MIT license.</para>
    <para>
        The primary URL for this document is
         <ulink url="http://diagonalsw.sourceforge.net">http://diagonalsw.sourceforge.net</ulink>.
    </para>
  </sect1>




  <sect1 id="download">
    <title>Download</title>
    <para>      
       Download the software from the <ulink url="http://sourceforge.net/projects/diagonalsw">sourceforge</ulink> project page. 

 The latest version of diagonalsw is @PACKAGE_VERSION@. 
    </para>
  </sect1>
  <sect1 id="installation">
    <title>Installation</title>

    <sect2 id="installation_with_prebuilt_package">
      <title>Installation with prebuilt package</title>
    <para>      
As              <ulink url="http://www.threadingbuildingblocks.org/">Intel Intel® Threading Building Blocks (TBB)</ulink> version 2.2 is rather new and has not been packaged yet for all platforms, building from source code is the recommended way right now ( 2009-11-26 ). Try "make package" if you want to start building your own rpm or deb package. Related to the packaging of diagonalsw is the FAQ ( <ulink url="http://www.threadingbuildingblocks.org/wiki/index.php?title=Using_TBB#Is_there_a_version_of_TBB_that_provides_statically_linked_libraries.3F">Is there a version of TBB that provides statically linked libraries?</ulink> ).


    </para>





<!--
    <sect3 id="installation_on_windows">
      <title>Installation on Windows with .exe file</title>
<para>
To install diagonalsw on Windows, first download the diagonalsw-@PACKAGE_VERSION@-win32.exe and then execute the file ( click on it ). 
    </para>
  </sect3>


    <sect3 id="installation_on_ubuntu_and_debian">
      <title>Installation on Ubuntu and Debian</title>

<para>
To install diagonalsw on Ubuntu or Debian, first download the diagonalsw-@PACKAGE_VERSION@.deb  and then log in as root and  
<programlisting><![CDATA[
# dpkg -i diagonalsw-@PACKAGE_VERSION@.deb 
]]></programlisting>

    </para>
  </sect3>


    <sect3 id="installation_on_centos_and_fedora_linux">
      <title>Installation on CentOS and Fedora</title>

<para>
To install diagonalsw on Centos or Debian, first download the diagonalsw-@PACKAGE_VERSION@.Linux.rpm   and then log in as root and  
<programlisting><![CDATA[
# yum localinstall diagonalsw-@PACKAGE_VERSION@.Linux.rpm 
]]></programlisting>

    </para>
  </sect3> 





    <sect3 id="installation_MacOS_X">
      <title>Installation on Mac OS X</title>

<para>
To install diagonalsw on a Mac OS X v10.5 ( Leopard ) on a Mac computer with Intel cpu, first download the diagonalsw-@PACKAGE_VERSION@-MacOSX10.5.tar.gz   and then 
<programlisting><![CDATA[
$ tar xfz diagonalsw-@PACKAGE_VERSION@-MacOSX10.5.tar.gz  
]]></programlisting>

    </para>
<para>
To install diagonalsw on a Mac OS X v10.4 ( Tiger ) on a Mac computer with Intel cpu, first download the diagonalsw-@PACKAGE_VERSION@-MacOSX10.4.tar.gz   and then 
<programlisting><![CDATA[
$ tar xfz diagonalsw-@PACKAGE_VERSION@-MacOSX10.4.tar.gz
]]></programlisting>

    </para>
  </sect3>



-->
 </sect2>




    <sect2 id="building_from_source">
      <title>Building from source</title>

    <sect3 id="building_from_source_on_unix">
      <title>Building from source on Unix</title>

      <para>To build diagonalsw on Unix ( e.g. Linux, MacOSX, CygWin ) you need to have this installed
        <itemizedlist mark="bullet">
          <listitem>
            <para>
              <ulink url="http://www.cmake.org">cmake</ulink>
            </para>
          </listitem>
          <listitem>
            <para>
              <ulink url="http://www.gnu.org/software/gengetopt/gengetopt.html">gengetopt</ulink>
            </para>
          </listitem>
          <listitem>
            <para>
              <ulink url="http://xmlsoft.org/XSLT/xsltproc2.html">xsltproc</ulink>
            </para>
          </listitem>

<!--
          <listitem>
            <para>
              <ulink url="http://xmlsoft.org/">libxml2</ulink>
            </para>
          </listitem>
-->
          <listitem>
            <para>
                <ulink url="http://www.threadingbuildingblocks.org/">Intel Intel® Threading Building Blocks (TBB)</ulink>
            </para>
          </listitem>

        </itemizedlist>
      </para>
      <para>

To install TBB run something like this:
<programlisting><![CDATA[
$ tar xfz /scratch/tbb22_20091101oss_src.tgz 
$ cd tbb22_20091101oss/
$ less README 
$ make
]]></programlisting>


If you have installed Intel Intel® Threading Building Blocks (TBB) into a non-standard location, please first source the <filename>tbbvars.sh</filename>, in other words something like this
<programlisting><![CDATA[
$ . /home/myuser/tbb22_20091101oss/build/linux_intel64_gcc_cc4.3.3_libc2.9_kernel2.6.28_release/tbbvars.sh
]]></programlisting>




If you have the diagonalsw source code in the directory <filename>/tmp/diagonalsw-@PACKAGE_VERSION@</filename> and you want to install diagonalsw into the directory <filename>/tmp/install</filename>, and you have installed gengetopt in the non-default location <filename>~/gengetopt/inst/bin/</filename>, then


first run <command>cmake</command> then <command>make</command> and then <command>make install</command>
<programlisting><![CDATA[
$ mkdir /tmp/build
$ cd /tmp/build
$ cmake -DCMAKE_INSTALL_PREFIX=/tmp/install -DCMAKE_PREFIX_PATH=~/gengetopt/inst/bin/ /tmp/diagonalsw--@PACKAGE_VERSION@  && make && make install
-- The C compiler identification is GNU
-- Check for working C compiler: /usr/bin/gcc
-- Check for working C compiler: /usr/bin/gcc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Configuring done
-- Generating done
-- Build files have been written to: /tmp/build
[  5%] Generating smith_waterman_sse_word.2loadEFH.c
[ 10%] Generating cmdline_diagonalsw.c, cmdline_diagonalsw.h
[ 15%] Generating smith_waterman_sse_byte.2load_HFsave8thnormal.c
[ 20%] Generating smith_waterman_sse_word.2loadnormal.c
[ 25%] Generating smith_waterman_sse_byte.2load_HFsave8thH.c
[ 30%] Generating smith_waterman_sse_word.2loadH.c
[ 35%] Generating smith_waterman_sse_byte.2load_HFsave8thEFH.c
Scanning dependencies of target diagonalsw
[ 40%] Building C object src/c/CMakeFiles/diagonalsw.dir/read_matrix.c.o
[ 45%] Building C object src/c/CMakeFiles/diagonalsw.dir/read_sequence.c.o
[ 50%] Building C object src/c/CMakeFiles/diagonalsw.dir/diagonalsw.c.o
[ 55%] Building C object src/c/CMakeFiles/diagonalsw.dir/cmdline_diagonalsw.c.o
[ 60%] Building C object src/c/CMakeFiles/diagonalsw.dir/create_profile.c.o
[ 65%] Building C object src/c/CMakeFiles/diagonalsw.dir/smith_waterman_reference_impl.c.o
[ 70%] Building C object src/c/CMakeFiles/diagonalsw.dir/smith_waterman_sse_byte.2load_HFsave8thnormal.c.o
[ 75%] Building C object src/c/CMakeFiles/diagonalsw.dir/smith_waterman_sse_word.2loadnormal.c.o
[ 80%] Building C object src/c/CMakeFiles/diagonalsw.dir/smith_waterman_sse_byte.2load_HFsave8thH.c.o
[ 85%] Building C object src/c/CMakeFiles/diagonalsw.dir/smith_waterman_sse_word.2loadH.c.o
[ 90%] Building C object src/c/CMakeFiles/diagonalsw.dir/smith_waterman_sse_byte.2load_HFsave8thEFH.c.o
[ 95%] Building C object src/c/CMakeFiles/diagonalsw.dir/smith_waterman_sse_word.2loadEFH.c.o
[100%] Building C object src/c/CMakeFiles/diagonalsw.dir/sse_funcs.c.o
Linking C executable diagonalsw
[100%] Built target diagonalsw
]]></programlisting>

If you want to build the html documentation ( i.e. this page ) you need to pass the -DBUILD_DOCUMENTATION=ON option to <application>cmake</application>.
      </para>
    </sect3>




    </sect2>





  </sect1>




  <sect1 id="usage">
    <title>Usage</title>
  <sect2 id="diagonalsw_command_line_options">
    <title>diagonalsw command line options</title>


<para>

Type <userinput>diagonalsw --help</userinput> to see the command line options

<programlisting><repl shell-cmd="diagonalsw --help"/></programlisting>
</para></sect2>


  <sect2 id="diagonalsw_input_file_formats">
    <title>diagonalsw input file formats</title>

<para>

<table frame='all'><title>diagonalsw input file formats</title>
<tgroup cols='3' align='left' colsep='1' rowsep='1'>
<thead>
<row>
<entry>file format</entry>
<entry>short option</entry>
<entry>description</entry>
</row>
</thead>
<tbody>
<row>
<entry>fasta format</entry>
<entry>-q</entry>
<entry>query file in fasta format <xref linkend="fasta_format"/></entry>
</row>
<row>
<entry>fasta format</entry>
<entry>-d</entry>
<entry>database file in fasta format <xref linkend="fasta_format"/></entry>
</row>

<row>
<entry>profile format</entry>
<entry>-p</entry>
<entry>query file in profile format <xref linkend="profile_file_format"/></entry>
</row>

<row>
<entry>scoring matrix file format</entry>
<entry>-s</entry>
<entry>Substitution matrix ( Scoring matrix ) <xref linkend="scoring_matrix_file_format"/></entry>
</row>


</tbody>
</tgroup>
</table>
</para>
  </sect2>


<!--
  <sect2 id="diagonalsw_output_formats">
    <title>diagonalsw output file formats</title>


<para>

<table frame='all'><title>diagonalsw output file formats</title>
<tgroup cols='3' align='left' colsep='1' rowsep='1'>
<thead>
<row>
<entry>file format</entry>
<entry>short option</entry>
<entry>description</entry>
</row>
</thead>
<tbody>
<row>
<entry>diagonalsw sequence XML format</entry>
<entry>-O xml</entry>
<entry><xref linkend="diagonalsw_distance_matrix_xml_format"/></entry>

</row>
<row>
<entry>phylip distance matrix format</entry>
<entry>-O phylip_dm</entry>
<entry><xref linkend="phylip_distance_matrix_format"/></entry>

</row>
</tbody>
</tgroup>
</table>
</para>
  </sect2>

-->


  <sect2 id="diagonalsw_examples">
    <title>Examples</title>
<para>
<example id="diagonalsw_with_query_in_fasta_format"><title>diagonalsw with query in fasta format</title><para>
We use the file described in <xref linkend="db.fasta"/> as database file and we use the file described in <xref linkend="blosum50.mat"/> as scoring matrix. 

<programlisting><repl shell-cmd="cat query.fasta"/></programlisting>
<programlisting><repl shell-cmd="diagonalsw -q query.fasta -d db.fasta -s blosum50.mat"/></programlisting>

</para>
</example>

<example id="diagonalsw_with_specified_gap_penalties"><title>diagonalsw with specified gap penalties</title><para>
This example is the same as <xref linkend="diagonalsw_with_query_in_fasta_format"/> but with gap penalties specified.
The penalty for starting an alignment gap was chosen to be -8 and the penalty for extending an alignment gap 
was chosen to be -1.


<programlisting><repl shell-cmd="diagonalsw -q query.fasta -d db.fasta -s blosum50.mat -i -8 -e -1"/></programlisting>

</para>
</example>



<example id="diagonalsw_with_query_in_profile_file_format"><title>diagonalsw with query in profile file format</title><para>
<note><para>Specifying query in profile format is not implemented yet. The function read_profile() needs to be written.</para></note>
We use the file described in <xref linkend="1R69.psi"/> as the query. 
We use the file described in <xref linkend="db.fasta"/> as database file and we use the file described in <xref linkend="blosum50.mat"/> as scoring matrix. 

<programlisting><repl shell-cmd="diagonalsw -p 1R69.psi -d db.fasta -s blosum50.mat"/></programlisting>


</para>
</example>

<example id="diagonalsw_with_benchmarking"><title>diagonalsw with benchmarking</title><para>

This example is the same as <xref linkend="diagonalsw_with_query_in_fasta_format"/> but with benchmarking. The computation speed 
Gigacells/second is printed.
<programlisting><repl shell-cmd="diagonalsw -q query.fasta -d db.fasta -s blosum50.mat -b"/></programlisting> 
Note, db.fasta is too small ( containing only 3 sequences ) to give a fair result for Gigacells/second. See also <xref linkend="performance_benchmarked_computation_speed"/>.
</para>
</example>

<example id="diagonalsw_with_verbose_benchmarking"><title>diagonalsw with verbose benchmarking</title><para>

This example is the same as <xref linkend="diagonalsw_with_query_in_fasta_format"/> but with verbose benchmarking. Benchmarking information is printed.
<programlisting><repl shell-cmd="diagonalsw -q query.fasta -d db.fasta -s blosum50.mat -B"/></programlisting> 
Note, db.fasta is too small ( containing only 3 sequences ) to give a fair result for Gigacells/second. See also <xref linkend="performance_benchmarked_computation_speed"/>.

</para>
</example>


      </para>

    </sect2>







    </sect1>

  <sect1 id="file_formats">
    <title>File formats</title>
    <para>      
This software package handles the following file formats
</para>


  <sect2 id="fasta_format">
    <title>Fasta format</title>
<para>
Read more about the <ulink url="http://en.wikipedia.org/wiki/Fasta_format">Fasta format</ulink> on Wikipedia.
<!--The parser will take the whole header line as the sequence identifier name, i.e. all characters after the greater-than character ( ">" ). -->





<example id="db.fasta"><title>db.fasta, an example file in fasta format</title><para>
The example file <ulink url="example_files/db.fasta">db.fasta</ulink> contains
<programlisting><xi:include xmlns:xi="http://www.w3.org/2001/XInclude" parse="text" encoding="UTF-8" href="${CMAKE_CURRENT_SOURCE_DIR}/example_files/db.fasta">
<xi:fallback>
   couldn't xinclude file
</xi:fallback>
</xi:include></programlisting>

</para></example>


</para></sect2>


  <sect2 id="profile_file_format">
    <title>Profile file format</title>
<para>

<example id="1R69.psi"><title>1R69.psi, an example file in the profile file format</title><para>

The example file <ulink url="example_files/1R69.psi">1R69.psi</ulink> contains 

<programlisting><xi:include xmlns:xi="http://www.w3.org/2001/XInclude" parse="text" encoding="UTF-8" href="${CMAKE_CURRENT_SOURCE_DIR}/example_files/1R69.psi">
<xi:fallback>
   couldn't xinclude file
</xi:fallback>
</xi:include></programlisting>
</para></example>

</para></sect2>


  <sect2 id="scoring_matrix_file_format">
    <title>Scoring matrix file format</title>
<para>

<example id="blosum50.mat"><title>blosum50.mat, an example file in the Scoring matrix file format</title><para>

The example file <ulink url="example_files/blosum50.mat">blosum50.mat</ulink> contains 

<programlisting><xi:include xmlns:xi="http://www.w3.org/2001/XInclude" parse="text" encoding="UTF-8" href="${CMAKE_CURRENT_SOURCE_DIR}/example_files/blosum50.mat">
<xi:fallback>
   couldn't xinclude file
</xi:fallback>
</xi:include></programlisting>
</para></example>

</para></sect2>


<!--
<![CDATA[
  <sect2 id="diagonalsw_distance_matrix_xml_format">
    <title>Diagonalsw distance matrix XML format</title>
<para>
The Diagonalsw sequence XML format is chosen by the option <userinput>-O xml</userinput> to diagonalsw and the option <userinput>-I xml</userinput> to fnj. 

Type <userinput>diagonalsw -print-relaxng-output</userinput> to see its relaxng schema

<programlisting><repl shell-cmd="diagonalsw -help"/></programlisting>

The Relax NG schema specifies that the extrainfo element is optional and can be inserted as a child to a seq element. The extrainfo element may contain any content.

<example id="dm.xml"><title>dm.xml, an example file in  Diagonalsw distance matrix XML format</title><para>
The example file <ulink url="example_files/dm.xml">dm.xml</ulink> contains

<programlisting><xi:include xmlns:xi="http://www.w3.org/2001/XInclude" parse="text" encoding="UTF-8" href="${CMAKE_CURRENT_SOURCE_DIR}/example_files/dm.xml">
<xi:fallback>
   couldn't xinclude file
</xi:fallback>
</xi:include></programlisting>
</para></example>


</para></sect2>
]]>
-->


  </sect1>






  <sect1 id="implementation_details">
    <title>Implementation details</title>


<para>

The elements in the score matrix are calculated in another order than for instance in the software projects in <xref linkend="striped_smith_waterman"/> and <xref linkend="swps3"/>. In the diagonalsw implementaion the elements will be calculatad as a diagonal vector ( of length 8 or 16 ) slides along the db sequence axis. Reaching the end it iterates the whole thing 8 or 16 positions further down the query sequence axis.

      </para>


  <sect2 id="rearranging_the_column_vectors_into_diagonal_vectors">
    <title>Rearranging the column vectors into diagonal vectors</title>


<para>
The alignment cost is precalculated so that an alignment cost vector for a certain db sequence element can be loaded as a column. These values must later be transferend into a sliding diagonal vector. If k is the length of the vectors, one diagonal vector will contain values from k column vectors. Or a bit more formally:

<programlisting>
Let k be the length of the vectors:

If the first load is the vector:

(0,0),(0,1),(0,2),...,(0,k)

and the n:th load 

(n,0),(n,1),(n,2),...,(n,k)

we need to create a vector with

(n,0),(n-1,1),...,(n-k,k)
</programlisting>

      </para>

<sect3 id="rearranging_the_column_vectors_into_diagonal_vectors_sse_version">
<title>SSE version</title>

<para>

The problem can be solved by loading column vectors and moving the elements around with the _mm_blendv_epi8 instruction. First we initialize some variables ( byte-version, 16 elements per vector )
<programlisting>
    const  __m128i mask1=_mm_set_epi8(0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255);
    const  __m128i mask2=_mm_set_epi8(0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255);
    const  __m128i mask3=_mm_set_epi8(0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255);
    const  __m128i mask4=_mm_set_epi8(0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255);
    __m128i v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15;
</programlisting>

Then for each step the variable v_score_load is loaded with the new column elements.

<programlisting>v15 = v_score_load;
v7 = _mm_blendv_epi8(v7,v15,mask1);
v3 = _mm_blendv_epi8(v3,v7,mask2);
v1 = _mm_blendv_epi8(v1,v3,mask3);
v0 = _mm_blendv_epi8(v0,v1,mask4);
v_diagonal = v0;
</programlisting>

In the next step the variable name numbers in v0,v1,v3,v7,v15 are incremented by 1 and adjusted to be in the range 0 to 15. In other words: newnumber = ( oldnumber + 1 ) modulo 16. 
<programlisting>v0 = v_score_load;
v8 = _mm_blendv_epi8(v8,v0,mask1);
v4 = _mm_blendv_epi8(v4,v8,mask2);
v2 = _mm_blendv_epi8(v2,v4,mask3);
v1 = _mm_blendv_epi8(v1,v2,mask4);
v_diagonal = v1;
</programlisting>

After 16 iterations the v_diagonal will start to contain the correct diagonal values. This algorithm has one column load per iteration. It can be sligthly modified to to use two column loads per iteration, leading to one _mm_blendv_epi8 instruction less per iteration. 
      </para>
 </sect3> 



<sect3 id="rearranging_the_column_vectors_into_diagonal_vectors_altivec_version">
<title>Altivec version</title>
<para>
A different method is used for the Altivec version. Here the instruction vec_perm is used. The values are moving through some queue vectors. 
First we initialize some variables ( this is the word-version, 8 elements per vector )


<programlisting>
    vector unsigned short   v_score;
    vector unsigned short   v_score_q1;
    vector unsigned short   v_score_q2;
    vector unsigned short   v_score_q3;
    vector unsigned short   v_score_load; 
    vector unsigned char    queue1_to_score  = (vector unsigned char)(16,17,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
    vector unsigned char    queue2_to_queue1 = (vector unsigned char)(0,1,18,19,4,5,6,7,8,9,10,11,12,13,14,15);
    vector unsigned char    queue3_to_queue2 = (vector unsigned char)(16,16,16,16,16,21,16,0,16,1,16,2,16,3,16,4);
    vector unsigned char    queue3_with_load = (vector unsigned char)(23,5,6,7,8,25,9,10,11,27,12,13,29,14,31,16);
</programlisting>

Then for each step the variable v_score_load is loaded with the new column elements.

<programlisting>
        v_score    = vec_perm(v_score_q1, v_score_load, queue1_to_score);
        v_score_q1 = vec_perm(v_score_q2, v_score_load, queue2_to_queue1);
        v_score_q2 = vec_perm(v_score_q3, v_score_load, queue3_to_queue2);
        v_score_q3 = vec_perm(v_score_q3, v_score_load, queue3_with_load);
</programlisting>
v_score will contain the diagonal elements.
      </para>
 </sect3> 













</sect2>


  <sect2 id="parallelization">
    <title>Parallelization</title>
<para>
As the sequences in the db sequence database can be analyzed independently, <application>diagonalsw</application> could be parallized in a straightforward way. The initial attempt was done with a tbb::pipeline from the open source library <ulink url="http://www.threadingbuildingblocks.org/">Intel Intel® Threading Building Blocks (TBB)</ulink>. It doesn't scale linearly to the number of cores yet. It is a bit less. Right now there is roughly 3.4 times speedup on a Intel Core2 Quad. Maybe our threading implementation needs some more fine tuning or we need to implement it in another way.

     </para>     <para>You could test the parallelization speedup by adding a -D ( --disable-threading  ). If it does not scale linearly to the number of cores you will get a better computation speeds if you run multiple processes instead.

     </para>
</sect2>
  </sect1>

  <sect1 id="performance_benchmarked_computation_speed">
    <title>Performance - benchmarked computation speed</title>
    <para>
This has not been investigated that much. Up to 9 GigaCells/second has been seen on a "Intel(R) Core(TM)2 Quad  CPU Q9300  @ 2.50GHz" with a binary built with the Intel Compiler. Note that the computation speed depends strongly how often the byte function overflows and also on the length of the query sequence.

See also <xref linkend="diagonalsw_with_benchmarking"/> and <xref linkend="diagonalsw_with_verbose_benchmarking"/>. 

 </para>
  </sect1>



  <sect1 id="related_projects">
    <title>Related projects</title>



  <sect2 id="striped_smith_waterman">
    <title>Striped Smith-Waterman by Farrar</title>
<para>
In the article published 2006, <ulink url="http://bioinformatics.oxfordjournals.org/cgi/content/abstract/23/2/156">Striped Smith-Waterman speeds database searches six times over other SIMD implementations</ulink> <biblioref linkend="farrar:2006"/>, Michael Farrar describes a new clever way calculate the score matrix. Here SIMD vectors don't contain adjacent matrix elements, instead the matrix is striped. A SSE version was included in ssearch in the fasta software package ( see also <xref linkend="ssearch_in_the_fasta_software_package"/> ). 
</para><para>
A SSE version was also released in the software package found at <ulink url="http://farrar.michael.googlepages.com/smith-watermanfortheibmcellbe">Smith-Waterman for the Cell Broadband Engine</ulink>. That version also adds support for the Cell Broadband Engine. The software package is not open source but the source code can be downloaded. To get a permission to use the software you need to contact Michael Farrar ( read more in the file release/COPYRIGHT from the striped.tgz ).
      </para>
 </sect2>

  <sect2 id="swps3">
    <title>swps3</title>
<para>
<ulink url="http://www.inf.ethz.ch/personal/sadam/swps3/">swps3</ulink> is an open source reimplementation of Farrar's algorithm on IBM Cell/BE and 86/SSE by Adam Szalkowski, Christian Ledergerber, Philipp Krähenbühl and Christophe Dessimoz. The  software is presented in the technical note <ulink url="http://www.biomedcentral.com/1756-0500/1/107">SWPS3 - fast multi-threaded vectorized Smith-Waterman for IBM Cell/B.E. and 86/SSE</ulink>  <biblioref linkend="szalkowski:2008"/>. It is licensed under the MIT license. Unfortunately swps3 ( as of 2009-07-26 ) contains a <ulink url="http://www.sbc.su.se/~esjolund/swps3-bug-report-from-2009-07-26-shellsession.txt">bug</ulink> making it sometimes calculate the wrong result. The bug has been reported to Adam Szalkowski who acknowledges that the bug is reproducible.
      </para>
 </sect2>



  <sect2 id="ssearch_in_the_fasta_software_package">
    <title>ssearch in the fasta software package</title>
<para>
ssearch ( as of 2009-08-01 named ssearch_35 ) in the <ulink url="http://fasta.bioch.virginia.edu/fasta_www2/fasta_down.shtml">fasta software package</ulink> implements the Smith-Waterman algorithm. This software is not open source but the source code can be downloaded.  To get a permission to use the software you need to contact David Hudson, Assistant Provost for Research, University of Virginia ( read more in the file fasta-35.4.9/COPYRIGHT ). The SSE version is written by Farrar ( see also <xref linkend="striped_smith_waterman"/> ) and the AltiVec version is written by Erik Lindahl. The Altivec version is the same as the one found in diagonalsw.
      </para>
 </sect2>

</sect1>


<bibliography>
    <title>References</title>

<bibliomixed id="farrar:2006">


  <bibliomset relation="article">
  

    <author><firstname>Michael</firstname> <surname>Farrar</surname></author>
    <title role="article">Striped Smith-Waterman speeds database searches six times over other SIMD implementations</title>.
  </bibliomset>
  <bibliomset relation="journal">
    <title>Bioinformatics</title> 
   <volumenum>23</volumenum>(<issuenum>2</issuenum>):<pagenums>156-161</pagenums>, <pubdate>2006</pubdate></bibliomset>.
</bibliomixed>



<bibliomixed  id="szalkowski:2008">
  <bibliomset relation="article">
 
<!-- <abbrev id="hughey:1996.abbrev">Hughey:1994</abbrev>-->


    <author><firstname>Adam</firstname> <surname>Szalkowski</surname></author>, <author><firstname>Christian</firstname>, <surname>Ledergerber</surname></author> <author><firstname>Philipp</firstname>, <surname>Krähenbühl</surname></author> <author><firstname>Christophe</firstname>, <surname>Dessimoz</surname></author> 
    <title role="article">SWPS3 - fast multi-threaded vectorized Smith-Waterman for IBM Cell/B.E. and 86/SSE</title>.
  </bibliomset>
  <bibliomset relation="journal">
    <title>BMC Research Notes</title>,
    <issuenum>1</issuenum>:<pagenums>107</pagenums>, <pubdate>2008</pubdate></bibliomset>.
</bibliomixed>


    </bibliography>





</article>




