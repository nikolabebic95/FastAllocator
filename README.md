FastAllocator
=============

The purpose of this project is to implement the fast allocator for all C++ types

Description
-----------

Standard C++ allocator is pretty fast, but when there is a need only for objects of certain type, and not arrays, allocation can be optimized. Besides that, allocation can be quicker if it does not happen on the heap. This allocator uses whatever memory is already allocated and passed to it, and allocates objects from there. So, one could allocate a static global array of bytes, and pass it to this allocator. This might be a little bit quicker than heap allocation.

Implementation
--------------

The allocator is implemented using the slab allocator as an idea, and it behaves as if the whole memory used is one slab. Internally, the allocator keeps a list of free objects, and when an objects should be allocated, the allocator removes one object from the list, and returns it. When an object is deallocated, the allocator simply returns that object to the list of free objects.
