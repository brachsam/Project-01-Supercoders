# Design Proposal — Collections Library
## Section 1- Public API     
### Hash Map 
#### Core Operations
•	void set(K key, V val) — Insert or update. Rehashes if load factor >= 0.7. Probe P=3 first, chain if all full.
•	V& get(K key) const — Returns value reference. Calls error() if key not found.
•	bool exists(K key) const — Returns true if key present.
•	void remove(K key) — Removes entry. Calls error() if not found.
•	int size() const — Returns stored pair count.
•	float loadFactor() const — Returns size / bucketCount.

#### Utility & Algorithm Operations
- DynamicArray<K> keys() const — Returns a DynamicArray containing all the keys stored in the HashMap.
- DynamicArray<V> values() const — Returns a DynamicArray containing all the keys stored in the HashMap.
- void merge(const HashMap<K,V>& other) — Copies all key-value pairs from another HashMap into the current HashMap. If the same key exists in both, the current HashMap keeps its own value and ignores the new one.
- bool contains(K key) const — Checks whether the given key exists in the HashMap. It works exactly like exists() but has a simpler name
- void clear() — Removes all key-value pairs from the HashMap and resets the size to 0. The bucket array is not deleted, so it can be reused without allocating memory again.

### Hash Specializations - compiler picks automatically based on K
- Hash<int> — Knuth multiplicative hash — u * 2654435761.
- Hash<char> — Casts to int, delegates to Hash<int>.
- =Hash<float> — Reinterprets bits as unsigned int, then mod.
- Hash<double> — 64-bit mix then mod.
- Hash<string> — djb2 — fast and good distribution for short strings.
- Hash<const char*> — Same djb2 walking a raw pointer.
- User-defined types — User writes Hash<MyType> specialization once. Compiler picks it automatically.

#### Memory Management
- ~HashMap() — Destructor — frees bucket array and all chain nodes.
- HashMap(const HashMap<K,V>&) — Copy constructor — new bucket array, deep copies every chain.
- operator=(const HashMap<K,V>&) — Copy assignment — self-assignment safe deep copy.

## Section 2
### Hash Map
![alt text](image-1.png)

## Section 3
### HashMap Complexity Estimates

| Method | Best Case | Average Case | Worst Case | Explanation |
|---------|-----------|--------------|------------|-------------|
| `set(key, val)` | O(1) | O(1) | O(N) | The hash value is calculated to find the bucket. Normally the key is inserted immediately. If collisions occur, probing and chain traversal may be required. In the worst case, all elements may be checked. |
| `get(key)` | O(1) | O(1) | O(N) | The hash value is used to locate the bucket. If collisions exist, probing and chain traversal continue until the key is found or all nodes are checked. |
| `exists(key)` | O(1) | O(1) | O(N) | Works the same as `get()`. It checks whether the given key is present in the HashMap. |
| `remove(key)` | O(1) | O(1) | O(N) | The key is first located using hashing. If found, it is removed from the collision chain. In the worst case, the entire chain may be traversed. |
| `rehash()` | O(N) | O(N) | O(N) | A larger bucket array is created and every key-value pair is inserted again using the new bucket count. |
| `keys()` | O(N) | O(N) | O(N) | Every bucket and every collision chain is traversed to collect all keys. |
| `values()` | O(N) | O(N) | O(N) | Every bucket and every collision chain is traversed to collect all values. |
| `merge(other)` | O(M) | O(M) | O(M × N) | Every element from the other HashMap is inserted into the current HashMap. Under normal conditions each insertion is O(1). |
| `contains(key)` | O(1) | O(1) | O(N) | It calls `exists()`. If the key is found quickly, it finishes immediately; otherwise, it may traverse the entire collision chain. |
| `clear()` | O(N) | O(N) | O(N) | Every collision chain is cleared by deleting all nodes. The bucket array remains allocated for future use. |

## Section 4 — Design Decisions
### HashMap — hybrid bounded probing with chaining fallback, P = 3
- Pure separate chaining always resolves a collision by extending the home bucket's chain, pure linear probing always tries to find the next free available slot to store the element. Use of hybrid of linear probing and seperate chaining avoids problems such as clustring by checking a small window of P=3 buckets. P=3 was chosen by wighing marginal benefit against marginal cose ratherr than just picking arbitrarily. The probability a given bucket is already occupied is roughly 50% using the standard approximation. Each additional prob slot costs one more bucket check on every future operation. Approx probability of fallback to chaining depending upon the value of P are stated as follows: For P=1 ->50% P=2 -> ~25%, P=3 -> ~13%, P=4 -> ~6%.
-  As more elements are inserted, collisions become more frequent and to maintain a good performance, the HashMap automatically resizes when the load factoe exceeds 0.7, meaning the hash map will resize reaching 70% of size. 
(Load Factor = Number of elements/Number of Buckets). Doubling the size reduces collisiond and keeps operations fast.
- The bucket count is increased by *2 instead of *1.5 or 3x as only few resizes are needed, bucket count remains a power of two. Index calculation becomes faster internally as there is no floating point number. Also overall performance remains consostent as the map grows.

- Hash<int> — Uses Knuth's multiplication constant (2654435761) to spread integer values uniformly across buckets. The integer is multiplied by the constant, and the result is taken modulo the bucket count to obtain the bucket index. This reduces clustering for consecutive integer keys.
- Hash<char> — A character is first converted to its ASCII value and then hashed using the integer hash function. This avoids duplicate logic and ensures consistent hashing for both char and int keys.
- =Hash<float> — The floating-point value is first converted to its raw binary representation using memcpy. The binary value is then hashed by taking modulo with the bucket count. Using the bit representation preserves the complete floating-point value instead of losing the decimal part through integer conversion.
- Hash<double> — The double value is copied into a 64-bit integer using memcpy. Bit-mixing operations (XOR and multiplication with a large constant) are applied to distribute similar values more uniformly before taking modulo with the bucket count. This reduces collisions for floating-point keys.
- Hash<string> — djb2 string hashing algorithm — fast and good distribution for short strings. Strarts with a string where initial value is 5381, it reads it first character multiply current hash by 33, adds ASCII value and thus repeating the same process for every character. Ending this will result in final hash value. At the end take modulo with bucket count to find the bucket index.
- Hash<const char*> — C-string hashing — Uses the same DJB2 algorithm as std::string. It starts with 5381, processes each character until the null terminator ('\0'), repeatedly multiplies the current hash by 33, adds the character's ASCII value, and finally takes modulo with the bucket count to determine the bucket index. 
- User-defined types — User writes Hash<MyType> specialization once. Compiler picks it automatically. Combines the x and y coordinates using multiplication with two different prime numbers followed by the XOR operator. This produces a single hash value from both coordinates, which is then taken modulo with the bucket count to determine the bucket index.


### Memory Management and Copying Strategy
Since our structures manage raw heap memory using pointers, relying on default shallow copies is highly dangerous. A shallow copy only copies the memory address pointer, meaning two separate objects end up sharing the exact same block of heap memory. This guarantees a catastrophic "double-free" crash the moment both objects go out of scope and try to delete the same memory space. To prevent this, the library enforces explicit deep copies for all copy constructors and assignment operators. Even though copying elements one-by-one into a freshly allocated block costs O(n) time, it is the only way to ensure each object has independent memory ownership and a safe lifetime.