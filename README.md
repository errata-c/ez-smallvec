# ez-smallvec
Provides a single template class under the header file `ez/smallvec.hpp`. The template class provided is an implementation of a vector like class with a fixed size, stack allocated internal storage. This is especially useful for instances where you need to store a small (but varying) number of elements quickly. The allocation overhead of a full standard vector in these instances can be problematic for runtime performance.

The class has the full interface of `std::vector`, minus the allocator template argument as it's not needed for stack based storage. 

