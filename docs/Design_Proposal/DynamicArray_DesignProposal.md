# Design Proposal — Collections Library
## Section 1- Public API 
### Dynamic Array
Creating a dynamic array that can store any data type and let T represent that particular type- template<typename T> class dynamic array.
#### Core Operations
- void append(const T& val) — Adds value to the end. Doubles capacity if full. O(1) amortised.
- void insert(int index, const T& val) — Inserts at index, shifting elements right. Resizes if needed.
- void remove(int index) — Removes at index, shifting elements left. Destroys the duplicate last slot.
- T& get(int index) — Returns reference to element. Const overload provided. Calls error() if out of range.
- const T& get(int index) const — Const version — used by HashMap and other const contexts.
- int size() — Returns current element count.
- int capacity() — Returns current allocated capacity.
#### Utility & Algorithm Operations
- int find(T value) — Returns index of first occurrence; -1 if not found. Requires operator==. 
- int count(T value) — Counts all occurrences. Requires operator==.  
- void reverse() — Reverses in-place. O(N), zero extra allocation.  
- T min_element() — Returns smallest element. Requires operator<. Calls error() if empty.  
- T max_element() — Returns largest element. Requires operator<. Calls error() if empty.  
- bool equal(const DynamicArray<T>& other) — True if same size and all elements match.  
- int mismatch(const DynamicArray<T>& other) — Index of first difference; -1 if equal.  
- void swap(DynamicArray<T>& other) — Swaps pointer, size, capacity. O(1) — no elements touched.  [NEW]
- void clear() — Resets size to 0. Buffer kept allocated.  
- bool contains(T value) — Wrapper around find(). Requires operator==.
- void shrinkToFit() — Explicit user-controlled shrink to exactly size. Normal doubling resumes on next append.  
#### Memory Management
- ~DynamicArray() — Destroys all live objects, then frees buffer.
- DynamicArray(const DynamicArray<T>&) — Deep copy — fresh allocation, placement new per element.
- operator=(const DynamicArray<T>&) — Self-assignment safe deep copy.
T must be copy-constructible. find/count/equal/mismatch/contains require operator==. min/max require operator<.

## Section 2
### Dynamic Array and Linked List
![](image.png)

## Section 3
### DynamicArray Complexity Estimates
# DynamicArray Complexity Analysis

| Method | Best Case | Average Case | Worst Case | Explanation |
|---------|-----------|--------------|------------|-------------|
| `append(val)` | O(1) | O(1) Amortized | O(N) | Normally, the value is added at the end. If the array is full, a larger array is created and all elements are copied. Since resizing happens rarely, the average time is O(1). |
| `get(index)` | O(1) | O(1) | O(1) | The element is accessed directly using its index. No traversal is required. |
| `insert(index, val)` | O(1) | O(N) | O(N) | Inserting at the end takes O(1). Inserting in the middle requires shifting all elements after the index one position to the right. |
| `remove(index)` | O(1) | O(N) | O(N) | Removing the last element takes O(1). Removing from the middle requires shifting all remaining elements one position to the left. |
| `shrinkToFit()` | O(N) | O(N) | O(N) | A new array of the exact required size is created, all elements are copied, and the old array is deleted. |
| `find(val)` | O(1) | O(N) | O(N) | If the value is found at the beginning, it finishes immediately. Otherwise, the entire array is searched. |
| `count(val)` | O(N) | O(N) | O(N) | Every element is checked to count how many times the value appears. |
| `reverse()` | O(N) | O(N) | O(N) | Elements are swapped from both ends until the middle of the array is reached. |
| `min_element()` | O(N) | O(N) | O(N) | Every element is checked once to find the smallest value. |
| `max_element()` | O(N) | O(N) | O(N) | Every element is checked once to find the largest value. |
| `equal(other)` | O(N) | O(N) | O(N) | Both arrays are compared element by element until a difference is found or all elements are checked. |
| `mismatch(other)` | O(1) | O(N) | O(N) | If the first elements are different, it finishes immediately. Otherwise, it checks until the first mismatch is found. |
| `swap(other)` | O(1) | O(1) | O(1) | Only the pointers, size, and capacity are exchanged. No array elements are copied. |
| `clear()` | O(1) | O(1) | O(1) | The size is reset to 0. The allocated memory is kept for future use. |
| `contains(val)` | O(1) | O(N) | O(N) | It searches for the value. If found early, it finishes quickly; otherwise, it checks the entire array. |

## Section 4 — Design Decisions
### Dynamic Array
- Capacity doubling over a fixed increment, because growing capacity by a fixed amount let say +1 or +4 on every resize and appending N elements would then trigger O(N) resizes, each copying up to N elements, giving O(N^2) total copy work while Doubling grows the array capacity geometrically so the total number of resizes across N appends is O(log N) and the total copy work across all of them is O(N), which is what makes amortized O(1) append possible at all. 
- I also considered *3 or *4 instead of doubling but on appending it will simply have lot of unused memory. Also if we initiate *1.5 resizing factor it will bring the capacity into floating point that will eventually slow out the performance. 
- The array starts at a small initial capacity (e.g. 2 or 4) rather than 0, for this particular design I took 4 as the base case because of it's resizing advantage and the very first append dosen't need a specific empty buffer case. 
- Finally, the capacity shrinks when the user explicitly calls shrinkToFit() instead of automatic shrinking so as to avoid thrashing problem. This method is called by the user when they are certain that there are no more elements will be added. If they append afterwards then normal double resizes takes place.
- malloc + placement new instead of new() to understand manual memory management. malloc gives raw bites only while constructors run via placement new only when we actually store a value. We need to call destructors manually before free() which helps understand lifecycle better.
- Using Template T to make the data type generic and use any data type.

### Memory Management and Copying Strategy
Since our structures manage raw heap memory using pointers, relying on default shallow copies is highly dangerous. A shallow copy only copies the memory address pointer, meaning two separate objects end up sharing the exact same block of heap memory. This guarantees a catastrophic "double-free" crash the moment both objects go out of scope and try to delete the same memory space. To prevent this, the library enforces explicit deep copies for all copy constructors and assignment operators. Even though copying elements one-by-one into a freshly allocated block costs O(n) time, it is the only way to ensure each object has independent memory ownership and a safe lifetime.