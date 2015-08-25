libmmp is a little library, written in C, to wrap some system function between win32 and linux.

Right now it wraps:
  * threads
  * some synchronization objects (semaphores, events and mutexes for threads only)
  * shared memory
  * sockets (partially)
  * dynamic module loading (.DLL and .SO)
  * some string functions (like strncpy and similar)

It also contains:
  * TAP testing functions
  * Error tracking
  * Nice list, stack, binary tree, B-tree and B-array implementations
  * A simple hash table (implemented the K & R way)

libmmp can be built under linux and windows with to separate project files; in both of them there is a build profile for self-tap-testing the library itself.