# Exercise Sheet 2

From here on, `Makefiles` with `all` (default) and `clean` rules are required. Your source code must compile by running make in the corresponding folder.

## Task 1

In this task you will familiarize yourself with the build system `make`.

### Using make for building C code

In the directory [task1](task1/) we provide some source code.
This exercise doesn't involve coding C, therefore don't modify the files in task1.
The task here is to create a Makefile in this directory which can be used to prepare and compile the provided code.

Your Makefile shall meet the following criteria:

- The rules `all`, `create_key` and `clean` must be available
    - `all` should prepare (create the `key.c` file - see below) and build the code.
    - `create_key` should create the missing `key.c` file. (`>>` and `whoami` from shell scripting might be helpful).
    This main file should look exactly like this:
    ```c 
    #include "key.h"

    const char* get_key() {
        return "your_username"; // here you need to replace "your_username" with the username of your account. Look at whoami(1).
    }
    ```
    - `clean` should remove all artifacts as well as the created executable and the `key.c` file.
- When running just `make` in the `task1` directory, the `all` rule should be executed.
- All calls to the C compiler should use the flags `-std=c11 -Wall -Wextra`.
- Make sure to use `.PHONY` correctly (see [here](https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html)).
- The C files should first be compiled into intermediary object files (`main.o`, `key.o`),
  which are then combined into the final executable in a separate rule.
- For clarity, the Makefile should echo `Compiling <file>` before compiling the respective object file (rule prerequisites might help here).

If everything works correctly, you should be able to call `make` in the directory `task1`,
which generates the executable `main`, and some object files. This call should yield an output as follows (note that the compiler commands might differ depending on your system):

```
Compiling main.c
cc -std=c11 -Wall -Wextra -c main.c
Creating key.c
Compiling key.c
cc -std=c11 -Wall -Wextra -c key.c
Linking main
cc main.o key.o -o main
```

Running `./main` should print (for a correct executable):

```
Key is valid. Hello, <your_username>!
```
When running `make clean`, the executable, `key.c` and the object files should be deleted.

Comprehensive documentation of GNU make can be found [online](https://www.gnu.org/software/make/manual/html_node/index.html).

### Using make to automate other tasks

`make` is mainly used for building code, but it can also be helpful for many other tasks.
For each rule, the recipe which is executed can run arbitrary shell commands (as seen before with creating the `key.c` file).

Have a look at the provided [Makefile](../lab02/Makefile) and answer the following questions:

* What does it do? 
* How would you call it?
* Where are you supposed to put it?
* Can you think of an additional use case for `make`?

Note your answer in a `task1.txt` or `task1.md` file.

## Task 2

This task is about working with permissions.

The provided archive [`task2.tar`](task2/task2.tar) contains two files. One of them is a shell script called `exercise.sh` and the other a text file, `file.txt`.

Please use the following command to unpack the archive on **ZID-GPL** or a **Unix-based** system:
```sh
tar -xpf task2.tar
```

Start by executing the shell script `exercise.sh`. The script will tell you, what you need to do and what needs to be done with `file.txt`. Therefore, ensure that the file is in the same folder as the script. Do **not** edit the files, other than their permissions.
To execute the script, run `./exercise.sh`. This execution might not initially work, then look at your permissions and adjust them as required.

When you list the contents of a directory with details you can see the permissions, owning user, and owning group among other details. For example:

```sh
$ ls -l
drwxrwxr-x. 2 c7012345 lxusers 4096 Mar 9 19:11 task_1_queue
-rw-rw-r--. 1 c7012345 lxusers 1088 Mar 9 04:20 task_2_summary.txt
-rw-rw-r--. 1 c7012345 lxusers   61 Mar 9 00:07 task_3_summary.txt

 ↑            ↑        ↑
 Permissions  User     Group
```

*Hint:* Take a look at `chmod(1)`.

## Task 3

The goal of this task is to estimate the probability of rolling specific numbers on two `n`-sided dice.

Write a C program that creates child processes (one for each possible outcome of both dice rollings) using `fork(2)`.

Each fork estimates the probability of rolling a specific number `t` when rolling two `n`-sided dice.
This is achieved by rolling the dice `s` times and calculating the percentage of tries where the dice rolled the number `t` (see **dice-roll simulation** [example](https://www.geogebra.org/m/UsoH4eNl)).
The values for `n` and `s` can be taken from the command line.

First, the parent process creates **all children processes**, then **awaits** until they have exited, and finally prints `Done.`.
The parent must wait for all children **only after** they have **all** been created.

Every child:

- Computes the probability of rolling a specific target number `t` on two `n`-sided dice using a dice-roll simulation with `s` steps.
- Prints a message of the form `Child <i> PID = <PID>. DR_p(<t,n,s>) = <DR_p(t,n,s)>. Elapsed time = <time>.` where
  `<i>` is the child number,
  `<PID>` is the child's process id,
  `<t>` is the target number to be rolled,
  `<n>` is the number of sides of the die,
  `<s>` is the amount of times that the die is rolled,
  `<DR_p(t,n,s)>` is the calculation result,
  and `<time>` is the elapsed time since the parent was started in seconds.

Make sure to only once measure the starting time of the parent process.

_Hint:_ You can use `clock_gettime()` (have a look at the man page `clock_gettime(2)`) to measure wall time.
To make this work, you can define the `-D_DEFAULT_SOURCE` feature test macro (see `feature_test_macros(7)`).

Use the following implementation for the dice-roll simulation, which returns the **d**ice-**r**oll **p**robability:

```c
double DR_p(const int t, const int n, const unsigned long long s) {
  unsigned long long h = 0;
  for (unsigned long long i = s; i--;) {
    h += (rand() % n + rand() % n + 2 == t);
  }
  return (double)h / s;
}
```

_Hint_: A parent process can wait for its children using `wait(2)` or `waitpid(2)`.

Example output:

```console
$ ./task3 6 100000000
Child 0 PID = 71128. DR_p(2,6,100000000) = 0.027784. Elapsed time = 4.124977s
Child 1 PID = 71129. DR_p(3,6,100000000) = 0.055538. Elapsed time = 4.143733s
Child 6 PID = 71134. DR_p(8,6,100000000) = 0.138880. Elapsed time = 4.152976s
Child 5 PID = 71133. DR_p(7,6,100000000) = 0.166692. Elapsed time = 4.171105s
Child 4 PID = 71132. DR_p(6,6,100000000) = 0.138892. Elapsed time = 4.185170s
Child 9 PID = 71137. DR_p(11,6,100000000) = 0.055574. Elapsed time = 4.189200s
Child 10 PID = 71138. DR_p(12,6,100000000) = 0.027768. Elapsed time = 4.190203s
Child 7 PID = 71135. DR_p(9,6,100000000) = 0.111100. Elapsed time = 4.193678s
Child 2 PID = 71130. DR_p(4,6,100000000) = 0.083324. Elapsed time = 4.272846s
Child 8 PID = 71136. DR_p(10,6,100000000) = 0.083370. Elapsed time = 4.303788s
Child 3 PID = 71131. DR_p(5,6,100000000) = 0.111095. Elapsed time = 4.359631s
Done.
```

Experiment with different values for `n` and `s`.
Analyze the obtained output.
Is the order of the messages consistent?
Can the order of these messages be predicted?
What does it depend on?

Notice that the function `DR_p` uses `rand(3)` to repeatedly generate pseudo-random numbers.
By default, this function returns the same sequence of numbers every time.
To get different estimates from each process, _seed_ the random number generator using `srand(getpid())`.
Does it matter whether you do this before or after the call to `fork()`?
Explain your answer.

--------------

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc02_csXXXXXX.zip
├── Makefile             # second Makefile from task1
├── group.txt            # optional
├── task1
│   ├── key.h
│   ├── main.c
│   ├── Makefile
│   └── task1.txt        # or .md
├── task2
│   └── task2.txt        # or .md
└── task3
    ├── Makefile
    ├── task3.txt        # or .md
    └── task3.c
```

Requirements

- [ ] Any implementation MUST NOT produce any additional output
- [ ] If you work in a group, create a `group.txt` file according to the format specified below.
- [ ] Auto-format all source files
- [ ] Check your submission on ZID-GPL
- [ ] Check your file structure (and permissions!)
- [ ] Submit zip
- [ ] Mark solved exercises in OLAT

If you worked in a group, the `group.txt` file must be present
and have one line per student which contains the matriculation number
in the beginning, followed by a space and the student's name.
For example, if the group consists of Jane Doe,
who has matriculation number 12345678,
and Max Mustermann, who has matriculation number 87654321,
the `group.txt` file should look like this:

```text
12345678 Jane Doe
87654321 Max Mustermann
```