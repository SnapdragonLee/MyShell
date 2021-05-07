all: MyShell++

objects = Main.o Func_Definition.o

MyShell++:$(objects)
	gcc -o MyShell++ $(objects) -g -lpthread


main.o: ShellCMD.h

definition.o: ShellCMD.h


.PHONY:clean

clean:
	rm $(objects)
