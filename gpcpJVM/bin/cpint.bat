@echo off
REM run the class file
echo JVMRelease1.1.3
java -DCPSYM=.;%JRoot%\libs;%JRoot%\libs\JvmSystem -cp .;%JRoot% -Djava.compiler=NONE CP.%1.%1 %2 %3 %4 %5 %6 %7 %8 %9
