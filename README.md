# tasksyslib
This library provides a system that runs function registered with list according to priority.

## Description
There are times when you want to specify the order and switching of functions to process at Game Creation.  
For example, there are state transition, title screens, menu screens, etc...  
In such a case, using this tasksyslib, you can easily build a task system.  
Specifically, function pointers and priorities are registered as tasks.  
Also, this tasksyslib is designed to use wastefully the memory space used by the task system itself.  

## Build instructions
I has recommended 'cmake'

### For example for Windows
`> cmake -G "Visual Studio 15 2017"`  
Build `ALL_BUILD` project which will hopefully successfully build whole library.  
However I added builed files to this repository.

## Reference
I wrote all functions at my homepage.  
See the link below for further details.  
<https://www.pit-ray.com/entry/tasksyslib_ref>

## Author
**pit-ray**  
**[homepage]**<https://www.pit-ray.com>  
**[E-mail]** pitxray*gmail.com ( * to @ )  
