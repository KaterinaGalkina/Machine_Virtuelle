# Virtual Machine Simulation

This is a program that simulates a fictional machine composed of a memory and a microprocessor. It performs two tasks: the first is an assembler that converts a program written in assembly language into machine language, and the second is the simulation which takes the machine language program and executes it instruction by instruction.

📁 **Please note**: Descriptive documents for the project are located in the `docs` folder and are written in **French**:
- `sujet.pdf` – the project instructions provided by the professor,
- `dev.pdf` – the development report explaining how the project was created,
- `user.pdf` – a user manual explaining how to use the virtual machine.
This is a collaborative academic project completed in pairs in 2024.

## Compilation and Execution of the Program

To compile and run the project, open your terminal and navigate to the directory containing the source code, then execute the following two commands:

```bash
gcc -Wall *.c -o simulateur -lm
```

```bash
./simulateur tests/inversion.txt
```

## Program Description in the Example

In this example, we first create an executable file named "simulator," then run it by passing one of the test files as a parameter.

Here the file "inversion.txt" contains a simple program, whose algorithm is described below:

```
ici: in r1
```

First, we initialize a label ("ici"), which will be used to indicate the address where the program should jump, then we wait for the user to enter an integer (positive or negative) within the following range: [-32768; 32767], which will be stored in register 1 (in r1).

```
jzs fin
sub r1, r0, r1
out r1
```

On the next line, we check if the user entered 0 and, if so, we stop the program by jumping to the address indicated by the label "fin", after which the "hlt" instruction is executed to halt the program.

```
fin: hlt
```

Otherwise, the program subtracts the entered number from 0, so register r1 contains the opposite of the initial number. Then, this resulting number is output.

```
jmp ici
```

Finally, we jump to the address indicated by the label "ici" with an unconditional jump (jmp).

The complete program:

```
ici: in r1
     jzs fin
     sub r1, r0, r1
     out r1
     jmp ici
fin: hlt
```

For other program examples, you can find a less detailed description at the end of the dev.pdf file.
