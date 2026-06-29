# Design Proposal — Collections Library
## Section 1- Public API     
### Linked List
template<typename T>  class LinkedList- Creating a linked list that can store any data type and let T represent that particular type.
The Linked List is a simple class that supports both singly-linked and Doubly-Linked behaviour controlled by a bool isDoubly passed during ondtruction and this eliminates code duplication, where all menthods and functions are written once so as not to make the code redundant. Original Design used one class with a flag, updated it later to two derived classes for better long-term maintainability and clean seperation of implementation.

#### SinglyLinkedList<T> — one pointer per node
- Node: data, next — No prev pointer. Less memory per node.
- deleteBack() — O(N) — must walk from head to find predecessor of tail.
- All other ops — Same as doubly. insertBack is O(1) because tail pointer is maintained.
- getHead() — Returns head pointer — used by HashMap to walk chains.
- removeVal(const T& val) — Removes first node matching val — used by HashMap.
- clear() — Frees all nodes, resets head/tail/n.
#### DoublyLinkedList<T> — two pointers per node
- Node: data, next, prev — Extra prev pointer. More memory, but O(1) deleteBack.
- deleteBack() — O(1) — jumps directly to tail->prev. This is the only meaningful difference from singly.
- All other ops — Same logic as singly, with extra prev pointer wiring in insert/remove/deleteFront.
- getHead() — Returns head pointer — used by HashMap to walk chains.
- removeVal(const T& val) — Removes first matching node. Fixes both next and prev links.
- clear() — Frees all nodes, resets head/tail/n.
#### Memory Management (both classes)
- makeNode(val) — malloc + placement new. Calls error() if malloc fails.
- freeNode(node) — Manual destructor call + free. Never assign after this.
- ~SinglyLinkedList() / ~DoublyLinkedList() — Walks full list, saves next before freeing each node.
- Copy constructor — Deep copy via insertBack on fresh nodes.
- operator= — Destroys current nodes, then deep copies from other.


## Section 2
### Dynamic Array and Linked List
![](image.png)

## Section 3
### Linked List Complexity Estimates 

| Method | Best Case | Average Case | Worst Case | Explanation |
|---------|-----------|--------------|------------|-------------|
| `insertFront(val)` | O(1) | O(1) | O(1) | A new node is created and added at the beginning. Only the head pointer is updated. |
| `insertBack(val)` | O(1) | O(1) | O(1) | Since a tail pointer is maintained, the new node is added directly at the end without traversal. |
| `deleteFront()` | O(1) | O(1) | O(1) | The first node is removed by moving the head pointer to the next node. |
| `deleteBack()` *(Doubly Linked List)* | O(1) | O(1) | O(1) | The tail pointer moves directly to the previous node (`tail->prev`) and the last node is deleted. |
| `deleteBack()` *(Singly Linked List)* | O(N) | O(N) | O(N) | The list must be traversed from the head to find the node before the last node. |
| `insert(index, val)` | O(1) | O(N) | O(N) | Inserting at the beginning or end is O(1). Otherwise, the list must be traversed to the required position before inserting the new node. |
| `remove(index)` | O(1) | O(N) | O(N) | Removing the first or last node is fast. Removing a node from the middle requires traversal to that position. |
| `search(val)` | O(1) | O(N) | O(N) | If the value is found in the first node, it takes O(1). Otherwise, every node is checked until the value is found or the list ends. |
| `find(val)` | O(1) | O(N) | O(N) | Works the same as search(). It checks each node until the value is found. |
| `count(val)` | O(N) | O(N) | O(N) | Every node is visited to count how many times the value appears. |
| `reverse()` | O(N) | O(N) | O(N) | Every node's pointer is changed once to reverse the list. No extra memory is required. |
| `contains(val)` | O(1) | O(N) | O(N) | It performs a search. If the value is found early, it finishes quickly; otherwise, the entire list is traversed. |
| `get(index)` | O(N) | O(N) | O(N) | Linked Lists do not support direct indexing, so the list is traversed from the head to the required index. |


## Section 4 — Design Decisions
### Linked List - Doubly link list chosen over singly link list.
- The initial approach considered creation of one doubly link list, but if the requirement was of singly link list then creation of two separate classed will produce redundant code, every method will be copy paste identically into both classes. Any future changes or bug fix would need to be applied in two places. 
- The next chosen solution is a singly LinkedList<T> class with a bool isDoubly flag set at construction time, All methods are written exactly once. The node struct always carries next and prev pointers when isDoubly is false then the previous pointer is never written or read.
- Now I changed the design to use an abstract base class with two separate derived classes SinglyLinkedList and DoublyLinkedList. This approch keeps both implementations independent. Each class has its own Node structure and its own insertion, deletion and traversal logic. A singly linked list node contains only a next pointer while a doubly linked list node contains both next and previous pointers. Because the two implementations are separate, changes made to one do not affect the other. This makes the code easier to understand, maintain, and extend in the future.

### Memory Management and Copying Strategy
Since our structures manage raw heap memory using pointers, relying on default shallow copies is highly dangerous. A shallow copy only copies the memory address pointer, meaning two separate objects end up sharing the exact same block of heap memory. This guarantees a catastrophic "double-free" crash the moment both objects go out of scope and try to delete the same memory space. To prevent this, the library enforces explicit deep copies for all copy constructors and assignment operators. Even though copying elements one-by-one into a freshly allocated block costs O(n) time, it is the only way to ensure each object has independent memory ownership and a safe lifetime.