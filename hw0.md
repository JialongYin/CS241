Homework 0
----------

### Watch the videos and write up your answers to the following questions

**Important!**

The virtual machine-in-your-browser and the videos you need for HW0 are here:

<http://cs-education.github.io/sys/>

Questions? Comments? Use Piazza: <https://piazza.com/illinois/spring2019/cs241>

The in-browser virtual machine runs entirely in Javascript and is fastest in Chrome. Note the VM and any code you write is reset when you reload the page, **so copy your code to a separate document.** The post-video challenges are not part of homework 0 but you learn the most by doing rather than just passively watching - so we suggest you have some fun with each end-of-video challenge.

HW0 questions are below. Please use this document to write the answers. This will be hand graded.

### Chapter 1

In which our intrepid hero battles standard out, standard error, file descriptors and writing to files

1.  **Hello, World! (system call style)** Write a program that uses `write()` to print out “Hi! My name is &lt;Your Name&gt;”.

```c
#include <unistd.h>
int main(){
  write(1, "Hi! My name is Jialong Yin", 26);
  return 0;
}
```

2.  **Hello, Standard Error Stream!** Write a function to print out a triangle of height `n` to standard error. Your function should have the signature `void write_triangle(int n)` and should use `write()`. The triangle should look like this, for n = 3:

```
*
**
***
```

```c
#include <unistd.h>
void write_triangle(int n){
  for(int i = 0; i < n; i++){
    write(2, "*", 1);
  }
  write(2, "\n", 1);
}
int main(){
  int n;
  for(n = 1; n <= 3; n++){
    write_triangle(n);
  }
  return 0;
}
```

3.  **Writing to files** Take your program from “Hello, World!” modify it write to a file called `hello_world.txt`. Make sure to to use correct flags and a correct mode for `open()` (`man 2 open` is your friend).

```c
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
  mode_t mode = S_IRUSR | S_IWUSR;
  int fildes = open("hello_world.txt", O_CREAT | O_TRUNC | O_RDWR, mode);
  write(fildes, "Hello, World!", 13);
  close(fildes);
  return 0;
}

```

5. **Not everything is a system call** Take your program from “Writing to files” and replace `write()` with `printf()`. *Make sure to print to the file instead of standard out!*

```c
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
  mode_t mode = S_IRUSR | S_IWUSR;
  int fildes = open("hello_world.txt", O_CREAT | O_TRUNC | O_RDWR, mode);
  dprintf(3, "Hello World!");
  close(fildes);
  return 0;
}

```

6.  What are some differences between `write()` and `printf()`?

```c
write is a system call and printf is a library call; printf() has buffer; printf can use data type pattern to represent variable.
```

### Chapter 2

Sizing up C types and their limits, `int` and `char` arrays, and incrementing pointers

1.  How many bits are there in a byte?

```c
8
```
2.  How many bytes are there in a `char`?

```c
1
```

3.  How many bytes the following are on your machine? 

* `int`:4 
* `double`:8 
* `float`:4
* `long`:8
* `long long`:8 

4.  On a machine with 8 byte integers, the declaration for the variable `data` is `int data[8]`. If the address of data is `0x7fbd9d40`, then what is the address of `data+2`?

```c
0x7fbd9d50
```

5.  What is `data[3]` equivalent to in C? Hint: what does C convert `data[3]` to before dereferencing the address? Remember, the type of a string constant `abc` is an array.

```c
data+3
```

6.  Why does this segfault?

```c
char *ptr = "hello";
*ptr = 'J';
```
"hello" is constant that is stored in the system memory so it can't be changed.

7.  What does `sizeof("Hello\0World")` return?

```c
6
```

8.  What does `strlen("Hello\0World")` return?

```c
5
```

9.  Give an example of X such that `sizeof(X)` is 3.

```c
sizeof("ab")
```

10. Give an example of Y such that `sizeof(Y)` might be 4 or 8 depending on the machine.

```c
sizeof(2)
```

### Chapter 3

Program arguments, environment variables, and working with character arrays (strings)

1.  What are two ways to find the length of `argv`?
First parameter of main() "argc" indicates the length.
int count = 0; while(argv[++count] != NULL);
2.  What does `argv[0]` represent?
./program
3.  Where are the pointers to environment variables stored (on the stack, the heap, somewhere else)?
Somewhere else
4.  On a machine where pointers are 8 bytes, and with the following code:

    ``` c
    char *ptr = "Hello";
    char array[] = "Hello";
    ```

    What are the values of `sizeof(ptr)` and `sizeof(array)`? Why?

```c
4 and 6
ptr is char pointer which is 4 bytes.
array is char array which is 6 bytes plus zero at last.
```

5.  What data structure manages the lifetime of automatic variables?
stack
### Chapter 4

Heap and stack memory, and working with structs

1.  If I want to use data after the lifetime of the function it was created in ends, where should I put it? How do I put it there?
Put it in the heap. Add "static" in front of variable type or use malloc(). 
2.  What are the differences between heap and stack memory?
Stack is used for static memory allocation and Heap for dynamic memory allocation, both stored in the computer's RAM . Variables allocated on the stack are stored directly to the memory and access to this memory is very fast, and it's allocation is dealt with when the program is compiled.
3.  Are there other kinds of memory in a process?
Text Segment, Initialized Data Segment, Uninitialized Data Segment
4.  Fill in the blank: “In a good C program, for every malloc, there is a free()”.

5.  What is one reason `malloc` can fail?
There is no enough memory to allocate.
6.  What are some differences between `time()` and `ctime()`?
time() is system call to get the current time. ctime() is to transfer it to human readable string time.
7.  What is wrong with this code snippet?

``` c
free(ptr);
free(ptr);
```
Double free the pointer and don't set it to NULL.
8.  What is wrong with this code snippet?

``` c
free(ptr);
printf("%s\n", ptr);
```
Use the freed pointer that is invalid.
9.  How can one avoid the previous two mistakes?
Set pointer to NULL after free it only once.

10. Use the following space for the next four questions

```c
// 10
struct Person{
  char* name;
  int age;
  struct Person* friends[10];
};
typedef struct Person person_t;
// 12
person_t* create(char* name, int age){
  person_t* person = (person_t*) malloc(sizeof(person_t));
  person->name = strdup(name);
  person->age = age;
  for(int count = 10; count; count--){
    person->friends[count] = NULL;
  }
  return person;
}
// 13
void destroy(person_t*  person){
  free(person->name);
  memset(person, 0 , sizeof(person_t));
  free(person);
}
int main() {
// 11
char* name1 = "Agent Smith";
char* name2 = "Sonny Moore";
person_t* p1 = create(name1, 128);
person_t* p2 = create(name2, 256);
p1->friends[0] = Sonny_Moore;
p2->friends[0] = Agent_Smith;
destroy(p1);
destroy(p2);
}
```

* Create a `struct` that represents a `Person`. Then make a `typedef`, so that `struct Person` can be replaced with a single word. A person should contain the following information: their name (a string), their age (an integer), and a list of their friends (stored as a pointer to an array of pointers to `Person`s). 

*  Now, make two persons on the heap, “Agent Smith” and “Sonny Moore”, who are 128 and 256 years old respectively and are friends with each other. Create functions to create and destroy a Person (Person’s and their names should live on the heap).

* `create()` should take a name and age. The name should be copied onto the heap. Use malloc to reserve sufficient memory for everyone having up to ten friends. Be sure initialize all fields (why?).

* `destroy()` should free up not only the memory of the person struct, but also free all of its attributes that are stored on the heap. Destroying one person should not destroy any others.


### Chapter 5

Text input and output and parsing using `getchar`, `gets`, and `getline`.

1.  What functions can be used for getting characters from `stdin` and writing them to `stdout`?
getchar() to get characters and putchar() to print them out.
2.  Name one issue with `gets()`.
Overflow of input will change other variables without warning.
3.  Write code that parses the string “Hello 5 World” and initializes 3 variables to “Hello”, 5, and “World”.

```c
char* data = "Hello 5 World";
char buffer1[10];
char buffer2[10];
int num = 0 ;
sscanf(data, "%s %d %s", buffer1, num, buffer2); 
```

4.  What does one need to define before including `getline()`?
#define _GNU_SOURCE char* buffer = NULL; size_t capacity = 0;
5.  Write a C program to print out the content of a file line-by-line using `getline()`.

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
  FILE *stream = fopen(argv[1], "r");;
  char* line = NULL;
  size_t len = 0;
  ssize_t nread;;
  while ((nread = getline(&line, &len, stream)) != -1) {
               printf(" %s \n", line);
    
  }
}
```

### C Development

These are general tips for compiling and developing using a compiler and git. Some web searches will be useful here


1.  What compiler flag is used to generate a debug build?
-g

2.  You fix a problem in the Makefile and type `make` again. Explain why this may be insufficient to generate a new build.
make clean first to clear previous .o files and then make again.

3.  Are tabs or spaces used to indent the commands after the rule in a Makefile?
Yes

4.  What does `git commit` do? What’s a `sha` in the context of git?
A commit, or "revision", is an individual change to a file (or set of files). It's like when you save a file, except with Git, every time you save it creates a unique ID (a.k.a. the "SHA" or "hash") that allows you to keep record of what changes were made when and by who. Commits usually contain a commit message which is a brief description of what changes were made.

5.  What does `git log` show you?
A Git log is a running record of commits. A full log has the following pieces: A commit hash (SHA1 40 character checksum of the commits contents). Because it is generated based on the commit contents it is unique.

6.  What does `git status` tell you and how would the contents of `.gitignore` change its output?

Displays paths that have differences between the index file and the current HEAD commit, paths that have differences between the working tree and the index file, and paths in the working tree that are not tracked by Git 
A gitignore file specifies intentionally untracked files that Git should ignore. Files already tracked by Git are not affected
7.  What does `git push` do? Why is it not just sufficient to commit with `git commit -m ’fixed all bugs’ `?
Use git push to push commits made on your local branch to a remote repository. The git push command takes two arguments: A remote name, for example, origin. A branch name, for example, master.
Because if we simply do commit, we commit files to the local.
8.  What does a non-fast-forward error `git push` reject mean? What is the most common way of dealing with this?
If another person has pushed to the same branch as you, Git won't be able to push your changes.
You can fix this by fetching and merging the changes made on the remote branch with the changes that you have made locallly.

### Optional: Just for fun

-   Convert your a song lyrics into System Programming and C code covered in this wiki book and share on Piazza.

-   Find, in your opinion, the best and worst C code on the web and post the link to Piazza.

-   Write a short C program with a deliberate subtle C bug and post it on Piazza to see if others can spot your bug.

-   Do you have any cool/disastrous system programming bugs you’ve heard about? Feel free to share with your peers and the course staff on piazza.
