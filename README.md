# A2: C Memory Allocator

In this assignment, you will  build and profile a very simple implementation of malloc in C. Through this process, you will explore  the critical components of a memory management library. You will choose an allocation policy and a freelist structure. Then, you will explore how these choices can have performance implications!

## Part 0: Helpful Tools: GDB

A very helpful tool in this assignment will be GDB (GNU Debugger). GDB allows you to trace the execution of your programming, noting what lines trigger **Segmentation Faults** or other bugs!

Make sure you install gdb on your machine:
```
sudo apt install gdb
```

Note: The Google Colab notebook automatically installs GDB for you if you run the "Everytime your Runtime Resets" notebook cell.

To run GDB on an executable:
```
    gdb ./test
```


To run GDB on an executable with arguments:
```
    gdb --args ./test 0
```

Once you have started GDB you will see the following prompt:

```
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04.2) 12.1
Copyright (C) 2022 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./test...
(gdb)
```

To interact with GDB use the following commands:
- `run` starts the executable running
- `break main.c:43` sets a breakpoint at the given line, in this case, line 43 of main.c
    - `break func` you may also set a breakpoint at a function name
- `continue` continues execution after a break point
- `step` steps forward one line in the execution
- `print variable` prints the value of the variable at this point in time

For more information, follow the GNU-provided reference:
[https://ftp.gnu.org/old-gnu/Manuals/gdb/html_mono/gdb.html](https://ftp.gnu.org/old-gnu/Manuals/gdb/html_mono/gdb.html)

### Note: Segmentation Fault

A segmentation fault occurs when you attempt to **dereference** a memory address that is disallowed! This fault is raised by the hardware when a process attempts to access some prohibited or non-existent region of memory.

For example, the C code below, always triggers a segmentation fault, because the address 0x0 can not be dereferenced.
```c
int num = *((int*) 0x0);
```






## Part 1: Implement Malloc/Free with First Fit Strategy

A memory allocation library has two primary jobs:
1. responding to a user's call to `malloc(size_t size)` which returns a pointer to an available block of memory of at least `size` bytes
2. responding to a user's call to `free(void * ptr)` which marks the previously allocated block of memory as "free" or once again available to be allocated to future malloc requests.

In order to implement these two basic functionalities, the malloc library must maintain some data structure of what portions of memory are in-use or available.

We suggest the following strategy: an implicit doubly-linked list made from "header" metadata block at the start of each memory allocation.

```
memory:
    |  m_header | size bytes of data | next m_header | size bytes of data | ... 
```

The second question is which portion of the free memory should we allocate to this request? There are several choices:
- First-fit
- Best-fit
- ETC.

For speed, let's choose a first-fit strategy.

Secondly, what if the found block is too big? We must implement *splitting*. For example, if we have the following:

Say a client calls: `new_malloc(64)`
```
memory:
    |  m_header in_use=FALSE size=128 | 128 bytes of data | ... 
```

We should split memory and respond as follows:

```
memory:
    |  m_header in_use=TRUE size=64 | 64 bytes of data | m_header in_USE=FALSE size=128 - 64 - sizeof(m_header) |  128 - 64 - sizeof(m_header) bytes of data |
```

Make sure to take into consideration that after splitting a block of memory, we also need a new metadata header for the new block.

> [!IMPORTANT]
>  **Task: Implement new_malloc and new_free**
> - use a first-fit strategy to select a memory block
> - Make sure to **split** the chosen allocation to not allocate too much memory!

At this point running `make test` and `./test 0` and `./test 1` should pass.

## Part 2: Implement Malloc/Free with First Fit Strategy and Coalesce 

Now, notice there are some inefficiencies with our strategy.

Take a look at `./test 3`'s output.

Where does your strategy fall short?

> [!IMPORTANT]
> Task: In questions.txt, describe why allocations fail in ./test 3. Label your answer (1).

Now, we will implement a few improvements.

First, let us **align** our allocations to multiples of 16. 

If a client requests 10 bytes of data, we should treat it as if they requested 16 bytes.

Consider what affect this has on our freelist.

Secondly, let's **coalesce** blocks on calls to `free()`

How should this work?

Consider the following example:

```
memory:

    |m_header in_use=FALSE size = a | ... |  m_header in_use=TRUE size = b| ... | m_header in_use=FALSE size = c |

If we free the middle block, we want:

    |m_header in_use=FALSE size = a+b+c + sizeof(m_header) * 2 | ... |

```

On each free, we wish to check if the previous block and/or the next block are also not in use. If so, we can coalesce the current freed block with the next/previous block to get a larger region of continuous memory for future allocations.

> [!IMPORTANT]
> **Task: implement alignment and coalescing in memory-coalesce.c**

At this point running `make test-coalesce` and `./test-coalesce 1`, `./test-coalesce 2`, `./test-coalesce 3` should all pass.

## Part 3: Test your Basic Allocators

Let us test our two allocators.

Run the script `./plot.sh`, this will generate output text files in `data/` folder.

Open the files in `data/` and read through the outputted results. Do you notice any trends? 

## Part 4: Plot your Results

Let us test our two allocators.

Run the script `./plot.py data/t.out data/t2.out`, this will generate an output plot of time to allocate and failures for our two approaches.

> [!IMPORTANT]
> **Task: In `questions.txt`, describe what you see in plot-fail_rate.png and plot-thoughput.png.**
> Which approach has a better throughput and why? Label your answer `(2)`.
> Which approach has a better fail rate and why? Label your answer `(3)`.

## Submitting on Gradescope

To submit on Gradescope, submit all the files in this directory to the assignment upload.

You do not need to upload the `data/` subdirectory.

**DO NOT upload a zip.** Use Shift to select all the files in your assignment directory instead.

Note: To download files from google colab, navigate to the `Assignment-2` directory that should be saved in your **Google Drive**. (Assuming you did all your work in `/content/drive/MyDrive/Assignment-2`). Clicking the three vertical dots shows a "download" option that will download all files to your local computer for upload to gradescope.

You should see the autograder run and report a score. Ensure that you are happy with this score! Feel free to resubmit as many times as you wish before the deadline.
