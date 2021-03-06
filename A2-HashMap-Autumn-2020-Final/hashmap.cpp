/*
* Assignment 2: HashMap template implementation (BASIC SOLUTION)
*      Notes: this file is what we call a .tpp file. It's not a .cpp file,
*      because it's not exactly a regular source code file. Instead, we are
*      defining the template definitions for our HashMap class.
*
*      TODO: write a comment here.
*
*      You'll notice that the commenting provided is absolutely stunning.
*      It was really fun to read through the starter code, right?
*      Please emulate this commenting style in your code, so your grader
*      can have an equally pleasant time reading your code. :)
*/

#include "hashmap.h"
#include <unordered_set>


template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap() : HashMap(kDefaultBuckets) { }

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(size_t bucket_count, const H& hash) :
   _size(0),
   _hash_function(hash),
   _buckets_array(bucket_count, nullptr) { }

template <typename K, typename M, typename H>
HashMap<K, M, H>::~HashMap() {
   clear();
}

template <typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::size() const noexcept {
   return _size;
}

template <typename K, typename M, typename H>
inline bool HashMap<K, M, H>::empty() const noexcept {
   return size() == 0;
}

template <typename K, typename M, typename H>
inline float HashMap<K, M, H>::load_factor() const noexcept {
   return static_cast<float>(size())/bucket_count();
};

template <typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::bucket_count() const noexcept {
   return _buckets_array.size();
};

template <typename K, typename M, typename H>
bool HashMap<K, M, H>::contains(const K& key) const noexcept {
   return find_node(key).second != nullptr;
}

template <typename K, typename M, typename H>
void HashMap<K, M, H>::clear() noexcept {
   for (auto& curr : _buckets_array) {
       while (curr != nullptr) {
           auto trash = curr;
           curr = curr->next;
           delete trash;
       }
   }
   _size = 0;
}

template <typename K, typename M, typename H>
std::pair<typename HashMap<K, M, H>::value_type*, bool>
           HashMap<K, M, H>::insert(const value_type& value) {
   const auto& [key, mapped] = value;
   auto [prev, node_to_edit] = find_node(key);
   size_t index = _hash_function(key) % bucket_count();

   if (node_to_edit != nullptr) return {&(node_to_edit->value), false};
   _buckets_array[index] = new node(value, _buckets_array[index]);

   ++_size;
   return {&(_buckets_array[index]->value), true};
}

template <typename K, typename M, typename H>
M& HashMap<K, M, H>::at(const K& key) {
   auto [prev, node_found] = find_node(key);
   if (node_found == nullptr) {
       throw std::out_of_range("HashMap<K, M, H>::at: key not found");
   }
   return node_found->value.second;
}

// add a const version
template <typename K, typename M, typename H>
M& HashMap<K, M, H>::at(const K& key) const {
   auto [prev, node_found] = find_node(key);
   if (node_found == nullptr) {
       throw std::out_of_range("HashMap<K, M, H>::at: key not found");
   }
   return node_found->value.second;
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::node_pair HashMap<K, M, H>::find_node(const K& key) const {
   size_t index = _hash_function(key) % bucket_count();
   auto curr = _buckets_array[index];
   node* prev = nullptr; // if first node is the key, return {nullptr, front}
   while (curr != nullptr) {
       const auto& [found_key, found_mapped] = curr->value;
       if (found_key == key) return {prev, curr};
       prev = curr;
       curr = curr->next;
   }
   return {nullptr, nullptr}; // key not found at all.
}

template <typename K, typename M, typename H>
void HashMap<K, M, H>::debug() const {
   std::cout << std::setw(30) << std::setfill('-') << '\n' << std::setfill(' ')
             << "Printing debug information for your HashMap implementation\n"
             << "Size: " << size() << std::setw(15) << std::right
             << "Buckets: " << bucket_count() << std::setw(20) << std::right
             << "(load factor: " << std::setprecision(2) << load_factor() << ") \n\n";

   for (size_t i = 0; i < bucket_count(); ++i) {
       std::cout << "[" << std::setw(3) << i << "]:";
       auto curr = _buckets_array[i];
       while (curr != nullptr) {
           const auto& [key, mapped] = curr->value;
           // next line will not compile if << not supported for K or M
           std::cout <<  " -> " << key << ":" << mapped;
           curr = curr->next;
       }
       std::cout <<  " /" <<  '\n';
   }
   std::cout << std::setw(30) << std::setfill('-') << '\n';
}

template <typename K, typename M, typename H>
bool HashMap<K, M, H>::erase(const K& key) {
    auto [prev, node_to_erase] = find_node(key);
    if (node_to_erase == nullptr) {
        return false;
    } else {
        size_t index = _hash_function(key) % bucket_count();
        (prev ? prev->next : _buckets_array[index]) = node_to_erase->next;
        delete node_to_erase;
        --_size;
        return true;
    }
}

template <typename K, typename M, typename H>
void HashMap<K, M, H>::rehash(size_t new_bucket_count) {
    if (new_bucket_count == 0) {
        throw std::out_of_range("HashMap<K, M, H>::rehash: new_bucket_count must be positive.");
    }

    /* Optional Milestone 1: begin student code */

        // Hint: you should NOT call insert, and you should not call
        // new or delete in this function. You must reuse existing nodes.

    std::vector<node*> new_buckets_array(new_bucket_count, nullptr);
    for (size_t index = 0; index < _buckets_array.size(); ++index) {
        node* curr_node = _buckets_array[index], *next_node;
        while (curr_node != nullptr) {
            next_node = curr_node->next;
            const auto& [key, mapped] = curr_node->value;
            auto new_index = _hash_function(key) % new_bucket_count;
            // insert into corresponding position
            curr_node->next = new_buckets_array[new_index];
            new_buckets_array[new_index] = curr_node;

            curr_node = next_node;
        }
    }
    
    // assign class member to new value
    _buckets_array = new_buckets_array;

    /* end student code */
}

/*
    Milestone 2-3: begin student code

    Here is a list of functions you should implement:
    Milestone 2
        - operator[]
        - operator<<
        - operator== and !=
        - make existing functions const-correct

    Milestone 3
        - copy constructor
        - copy assignment
        - move constructor
        - move assignment
*/

// operator []
template <typename K, typename M, typename H>
M& HashMap<K, M, H>::operator[](const K& key) {
    auto [prev, curr] = find_node(key);
    if (curr == nullptr) { // key not in hashmap
        insert(std::make_pair(key, M())); // use default value of M
        auto [new_prev, new_curr] = find_node(key);
        return new_curr->value.second;
    } else { // key in hashmap
        return curr->value.second;
    }
}

template<typename K, typename M, typename H>
std::ostream& operator<<(std::ostream& out, const HashMap<K, M, H>& hashmap) {
    std::vector<typename HashMap<K, M, H>::value_type> pair_vec;
    out << '{';
    // move all the pairs into a linear vector
    for (size_t i = 0; i < hashmap.bucket_count(); ++i) {
        auto curr_node = hashmap._buckets_array[i];
        while (curr_node != nullptr) {
            pair_vec.push_back(curr_node->value); 
            curr_node = curr_node->next;
        }
    }
    // output
    for (size_t i = 0; i < pair_vec.size(); ++i) {
        const auto& [key, mapped] = pair_vec[i];
        out << key << ':' << mapped;
        if (i < pair_vec.size() - 1) {
            out << ", "; // if there are (key, mapped) pairs left
        }
    }
    out << '}'; // finish
    return out;
}

template<typename K, typename M, typename H>
bool operator==(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {
    // reference from check_map_equal function in tests.cpp
    if (lhs.empty() != rhs.empty() || lhs.size() != rhs.size()) {
        return false;
    }
    for (size_t i = 0; i < lhs.bucket_count(); ++i) {
        auto curr_node = lhs._buckets_array[i];
        while (curr_node != nullptr) {
            const auto& [key, mapped] = curr_node->value;
            if (rhs.contains(key) == false || rhs.at(key) != mapped) {
                return false;
            }
            curr_node = curr_node->next;
        }
    }
    return true;
}

template<typename K, typename M, typename H>
bool operator!=(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {
    return !(lhs == rhs); 
}

// copy constructor
template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(const HashMap<K, M, H>& other) {
    *this = other;
}

// move constructor
template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(HashMap<K, M, H>&& other) {
    if (&other != this) {
        this->clear(); // release resource
        this->_size = std::move(other._size);
        this->_buckets_array = std::move(other._buckets_array);
    }
}