Gardens Point Component Pascal Compiler. (JVM on Microsoft)
---------------------------------------------
Instructions for compiling the various demo programs.
0. General Instructions:
----------------------------
a) The batch files cprun.bat and cpint.bat must be on your PATH, probably in
	C:\gpcp-JVM\bin
b) The compiler must be able to find the library symbol files, probably by
   using the env. variable JROOT. Typically JROOT is the root of your 
   installation, C:\gpcp-JVM.  The cprun batch file will pass the property
   CPSYM=.;%JROOT%;%JROOT$\JvmSystem to the compiler.

0. Hello.
---------------
a) compile with
	cprun gpcp Hello.cp, or even cprun gpcp -verbose Hello.cp
b) execute with
	cprun Hello

1. HelloWorld.
---------------
a) compile with
	cprun gpcp HelloWorld.cp, or even cprun gpcp -verbose HelloWorld.cp
b) execute with
	cprun HelloWorld
c) now try
	cprun HelloWorld more command line args

2. NQueens.
------------
a) compile with
	cprun gpcp NQueens.cp
b) execute with
	cprun NQueens
c) now you know how many solutions there are to the NQueens problem for the
	various board sizes!

3. Hennessy.
--------------
a) compile with
	cprun gpcp Hennessy.cp
b) execute with
	cprun Hennessy
c) These are the Hennessy integer benchmarks.  These might improve slightly
   for later versions, but they are pretty close to native speeds already.
   If you want to know how much slower the interpreter is, use
	cpint Hennessy
   which sets java.compiler=NONE

4. MkOver.
--------------
This program tests the hash table overflow detection
a) compile with
	cprun gpcp MkOver.cp
   this creates the executable CP\MkOver\MkOver.class
b) run MkOver with
	cprun MkOver
   When you run MkOver it will create a file TooBig.cp which has 10000 
   separate constants defined.  
c) try to compile TooBig.cp
	cprun gpcp TooBig.cp
   This should trap with a hash table overflow. 
d) Retry compiling with
	cprun gpcp "-hSize=10000" TooBig.cp
   All is now ok!

5. HelloUplevel.
--------------
a) compile with 
	cprun gpcp HelloUplevel.cp
b) run with 
	cprun HelloUplevel
c) this demonstrates the warnings for programs that perform uplevel addressing.


7. TypeNames.cp
--------------
a) compile with
	cprun gpcp TypeNames.cp
b) run with 
	cprun TypeNames
c) this demonstrates the RTS facilities for getting the target-specific
   names of the implementation types that gpcp uses for various CP types.

8. TestBangString
--------------
a) compile with 
	cprun gpcp ExportBangString.cp TestBangString.cp 
b) you can also compile with
	cprun CPMake TestBangString
c) run with 
	cprun TestBangString
d) this demonstrates the new "bang-string" extension in V1.3.12


9. MkstrAndBox
--------------
Some examples of the MKSTR and BOX functions.


--------------------------- Have Fun ------------------------------------------

