Date: June 23

Duration:
3 Hours

Goal:
Understand a better way to manage memory in DynamicArray before implementation of code.

Problem Encountered:
My current DynamicArray uses new() and delete() that manages memory internally, calls constructor and destructor on it's own and I wanted to understand how real STL containers like std::vector handle memory.

What I Tried:
Studied:

* malloc() and free()
* Placement new
* Manual destructor calls

Learned the difference between:

* Allocating memory
* Creating objects

Understood that STL containers keep raw memory first and create objects only when needed.

Also discussed how resize(), append(), remove(), and clear() would work with this approach.

Outcome:
Decided to redesign DynamicArray using STL-style memory management.

New approach which will involve the following four function:

* Allocate raw memory using malloc()
* Create objects using placement new
* Destroy objects manually using ~T()
* Release memory using free()

This gives better control over object lifetime and is closer to how std::vector works internally.

Also planned helper functions to avoid duplicate code:

* allocateRaw()
* destroyElements()
* ensureCapacity()

Next Step:
Refactor DynamicArray to use raw memory and placement new instead of new[] and delete[].