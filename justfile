headers := "include/headers"
flags := "-std=c23 -O3"

#build exe
[group: 'build']
[windows]
build: compile-objects compile-exe

#compile .c to .o
[group: 'compile']
[windows]
compile-objects:
  clang {{flags}} -c include/*.c main.c --include-directory include/headers

  @mv *.o build/tmp/ 2>/dev/null || move *.o build/tmp/

#compile .o to .exe
[group: 'compile']
[windows]
compile-exe:
  clang build/tmp/*.o -o build/zip.exe