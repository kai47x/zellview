@echo off
java -DCPSYM=.;%JRoot%\libs;%JRoot%\libs\JvmSystem -classpath .;%JRoot% J2CPS.J2CPS %1 %2 %3

