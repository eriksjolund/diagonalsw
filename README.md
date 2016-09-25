# diagonalsw

MIT-licensed C/C++ implementation of the Smith-Waterman algorithm by using SIMD instruction sets (SSE4.1 and Altivec). The first version of the software (for PowerPC Altivec) was written by professor [Erik Lindahl](https://www.scilifelab.se/researchers/erik-lindahl/). In 2009 Erik Sjölund translated the software to SSE and while doing that found a new way of how to load the diagonal vector. The work was done as a software development project for Erik Lindahl at Stockholm university. Version 0.9.0 of diagonalsw was released in November 26, 2009 at SourceForge. As of today (2016-09-25) this software has not yet been published in a scientific journal.

## Performance

As I remember it now (2016-09-25) a few years later, for shorter reads diagonalsw did well in a benchmark comparison against
 Striped Smith-Waterman [Farrar:2006](http://bioinformatics.oxfordjournals.org/content/23/2/156.abstract). diagonalsw might be even faster when the
sequence length is roughly the same as the SIMD vector length, but generally the Farrar implementation is faster.

Note, in 2011 a fast implementation of the Smith-Waterman algorithm was published by Torbjørn Rognes [implementation Faster Smith-Waterman database searches with inter-sequence SIMD parallelisation](http://bmcbioinformatics.biomedcentral.com/articles/10.1186/1471-2105-12-221). The software is called [swipe](https://github.com/torognes/swipe) and seems to be the fastest when you want to run many different query sequences.

## Requirements

diagonalsw has these requirements at build time: 

- cmake
- xsltproc
- gengetopt

## Implementation detail: Loading score values into a diagonal vector 

This is also documented as a Stackoverflow question:
[How to load a sliding diagonal vector from data stored column-wise with SSE](http://stackoverflow.com/questions/15198011/how-to-load-a-sliding-diagonal-vector-from-data-stored-column-wise-withsse)

The implementation only needs one column load per iteration. First some variables are initialized


```
const __m128i mask1=_mm_set_epi8(0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255);
const __m128i mask2=_mm_set_epi8(0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255);
const __m128i mask3=_mm_set_epi8(0,0,255,255,0,0,255,255,0,0,255,255,0,0,255,255);
const __m128i mask4=_mm_set_epi8(0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255);
__m128i v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15;
```

Then for each step the variable `v_column_load` is loaded with the next column.

```
v15 = v_column_load;
v7 = _mm_blendv_epi8(v7,v15,mask1);
v3 = _mm_blendv_epi8(v3,v7,mask2);
v1 = _mm_blendv_epi8(v1,v3,mask3);
v0 = _mm_blendv_epi8(v0,v1,mask4);
v_diagonal = v0;
```


In the next step the variable name numbers in `v0`, `v1`, `v3`, `v7`, `v15` are incremented by 1 and adjusted to be in the range 0 to 15. In other words: newnumber = ( oldnumber + 1 ) modulo 16.


```
v0 = v_column_load;
v8 = _mm_blendv_epi8(v8,v0,mask1);
v4 = _mm_blendv_epi8(v4,v8,mask2);
v2 = _mm_blendv_epi8(v2,v4,mask3);
v1 = _mm_blendv_epi8(v1,v2,mask4);
v_diagonal = v1;
```

After 16 iterations the `v_diagonal` will start to contain the correct diagonal values. 

Looking at `mask1`,`mask2`, `mask3`, `mask4`, we see a pattern that can be used to generalize this algorithm for other vector lengths (2^n).

For instance, for vector length 8, we would only need 3 masks and the iteration steps would look like this:

```
v7 = a a a a a a a a
v6 =
v5 =
v4 =
v3 =         a a a a
v2 =
v1 =             a a
v0 =               a

v0 = b b b b b b b b
v7 = a a a a a a a a
v6 =
v5 =
v4 =         b b b b
v3 =         a a a a
v2 =             b b
v1 =             a b

v1 = c c c c c c c c
v0 = b b b b b b b b
v7 = a a a a a a a a
v6 =
v5 =         c c c c
v4 =         b b b b
v3 =         a a c c
v2 =           a b c

v2 = d d d d d d d d
v1 = c c c c c c c c
v0 = b b b b b b b b
v7 = a a a a a a a a
v6 =         d d d d
v5 =         c c c c
v4 =         b b d d
v3 =         a a c d

v3 = e e e e e e e e
v2 = d d d d d d d d
v1 = c c c c c c c c
v0 = b b b b b b b b
v7 = a a a a e e e e
v6 =         d d d d
v5 =     a a c c e e
v4 =       a b b d a

v4 = f f f f f f f f
v3 = e e e e e e e e
v2 = d d d d d d d d
v1 = c c c c c c c c
v0 = b b b b f f f f
v7 = a a a a e e e e
v6 =     b b d d f f
v5 =     a b c d e f

v5 = g g g g g g g g
v4 = f f f f f f f f
v3 = e e e e e e e e
v2 = d d d d d d d d
v1 = c c c c g g g g
v0 = b b b b f f f f
v7 = a a c c e e g g
v6 =   a b c d e f g

v6 = h h h h h h h h
v5 = g g g g g g g g
v4 = f f f f f f f f
v3 = e e e e e e e e
v2 = d d d d h h h h
v1 = c c c c g g g g
v0 = b b d d f f h h
v7 = a b c d e f g h  <-- this vector now contains the diagonal

v7 = i i i i i i i i
v6 = h h h h h h h h
v5 = g g g g g g g g
v4 = f f f f f f f f
v3 = e e e e i i i i
v2 = d d d d h h h h
v1 = c c e e g g i i
v0 = b c d e f g h i  <-- this vector now contains the diagonal

v0 = j j j j j j j j
v7 = i i i i i i i i
v6 = h h h h h h h h
v5 = g g g g g g g g
v4 = f f f f j j j j
v3 = e e e e i i i i
v2 = d d f f h h j j
v1 = c d e f g h i j  <-- this vector now contains the diagonal
```

## Documentation

Documenation for diagonalsw is currently found at http://diagonalsw.sourceforge.net/
The plan is to convert that documentation into Markdown format and move it to Github.