all: MyShell++

objects = Main.o Func_Definition.o

MyShell++:$(objects)
	gcc -o MyShell++ $(objects) -g -lpthread


Main.o: ShellCMD.h

Func_Definition.o: ShellCMD.h


.PHONY:clean

clean:
	rm -rf *~ *.o