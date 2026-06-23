#include <iostream>
#include <sstream> // Needed to parse a line of multiple inputs
#include <string>

using namespace std;

template<typename T>
class DynamicArray {
private:
    T* arr;
    int n;
    int capacity;

    void resize(int newCapacity) {
        T* temp = new T[newCapacity];

        for (int i = 0; i < n; i++) {
            temp[i] = arr[i];
        }

        delete[] arr;

        arr = temp;
        capacity = newCapacity;
    }

public:
    // Constructor to initialize the dynamic array
    DynamicArray() {
        n = 0;
        capacity = 4;
        arr = new T[capacity];
    }

    // Destructor to free the allocated memory
    ~DynamicArray() {
        delete[] arr;
    }

    // Add element at end of the array
    void append(T value) {
        if (n == capacity) {
            resize(capacity * 2);
        }

        arr[n] = value;
        n++;
    }

    // Remove a specific value from the array
    bool removeValue(T value) {
        int indexToRemove = -1;

        // Search for the value in the array
        for (int i = 0; i < n; i++) {
            if (arr[i] == value) {
                indexToRemove = i;
                break; // Stop at the first occurrence
            }
        }

        // If the value wasn't found
        if (indexToRemove == -1) {
            return false;
        }

        // Shift elements to the left to fill the gap
        for (int i = indexToRemove; i < n - 1; i++) {
            arr[i] = arr[i + 1];
        }

        n--; // Reduce size
        return true;
    }

    // Display array
    void print() const {
        if (n == 0) {
            cout << "[ ] (Array is empty)" << endl;
            return;
        }

        cout << "[ ";
        for (int i = 0; i < n; i++) {
            cout << arr[i] << " ";
        }
        cout << "]" << endl;
        cout << "Size = " << n << ", Capacity = " << capacity << endl;
    }
};

int main() {
    DynamicArray<int> arr;
    int choice;
    string inputLine;
    int element;

    do {
        cout << "\n--- Dynamic Array Menu ---" << endl;
        cout << "1. Append Multiple Elements" << endl;
        cout << "2. Remove Multiple Elements" << endl;
        cout << "3. Display Array" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        
        // Clear the input buffer to safely use getline later
        cin.ignore(); 

        switch (choice) {
            case 1: {
                cout << "Enter integer values separated by spaces (e.g., 10 20 30): " << endl;
                getline(cin, inputLine);
                
                // Use stringstream to read numbers one by one from the input line
                stringstream ss(inputLine);
                int count = 0;
                while (ss >> element) {
                    arr.append(element);
                    count++;
                }
                cout << "Successfully appended " << count << " element(s)." << endl;
                break;
            }

            case 2: {
                cout << "Enter integer values to remove separated by spaces (e.g., 20 40): " << endl;
                getline(cin, inputLine);
                
                stringstream ss(inputLine);
                int successCount = 0;
                int failCount = 0;
                while (ss >> element) {
                    if (arr.removeValue(element)) {
                        successCount++;
                    } else {
                        cout << "Warning: Value " << element << " not found in array." << endl;
                        failCount++;
                    }
                }
                cout << "Removed " << successCount << " element(s). Missed " << failCount << " element(s)." << endl;
                break;
            }

            case 3:
                cout << "\nCurrent Array Status:" << endl;
                arr.print();
                break;

            case 4:
                cout << "Exiting program..." << endl;
                break;

            default:
                cout << "Invalid choice! Please select an option between 1 and 4." << endl;
        }
    } while (choice != 4);

    return 0;
}