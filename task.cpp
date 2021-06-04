// copyright leonid perlin 2021

#include <iostream>
#include <string>
#include <vector>

#define HASHING_VALUE_FIRST 11
#define HASHING_VALUE_SECOND 13

#define START_SIZE 8
#define FILL_REHASH_PERCENTAGE 0.75

#define OP_SUCCESS 0
#define OP_FAILURE 1

template <class T>
uint64_t createHashFirstAlg(const T& keys_array, int size) {
  uint64_t hash = 0;
  for (int i = keys_array.size(); i >= 0; i--) {
    hash += (HASHING_VALUE_FIRST * hash + keys_array[i]) % size;
  }
  return hash % size;
}

template <class T>
uint64_t createHashSecondAlg(const T& keys_array, int size) {
  uint64_t hash = 0;
  for (int i = keys_array.size(); i >= 0; i--) {
    hash += (HASHING_VALUE_SECOND * hash + keys_array[i]) % size;
  }
  return (hash * 2 + 1) % size;
}

template <class T>
class hashTableItem {
 private:
  T key;
  bool existsVal;

 public:
  explicit hashTableItem(const T& value) : key(value), existsVal(false) {}

  const T& getKey() const;

  void setKey(const T& key_value);

  bool exists() const;

  void deleteEX();

  void added();
};

template <class T>
const T& hashTableItem<T>::getKey() const {
  return key;
}

template <class T>
void hashTableItem<T>::setKey(const T& key_value) {
  key = key_value;
}

template <class T>
bool hashTableItem<T>::exists() const {
  return existsVal;
}

template <class T>
void hashTableItem<T>::deleteEX() {
  existsVal = true;
}

template <class T>
void hashTableItem<T>::added() {
  existsVal = false;
}

template <class T>
class Hash_table {
 private:
  std::vector<hashTableItem<T>*> buffer;
  int bufSiz;
  int size;

  void rehashTable();

 public:
  Hash_table() : buffer(START_SIZE, NULL), bufSiz(START_SIZE), size(0) {}

  ~Hash_table() {
    for (int i = 0; i < buffer.size(); i++) {
      delete buffer[i];
    }
  }
  uint8_t add(const T& key_value);
  uint8_t remove(const T& key_value);
  uint8_t contains(const T& key_value) const;
};

template <class T>
void Hash_table<T>::rehashTable() {
  int new_bufSiz = bufSiz * 2;
    std::vector<hashTableItem<T>*> new_buffer(new_bufSiz, NULL);
    for (int i = 0; i < bufSiz; i++) {
      if (buffer[i] != NULL &&
          !buffer[i]
               ->exists())  
      {
        int h1 = createHashFirstAlg(buffer[i]->getKey(), new_bufSiz);
        int h2 = createHashSecondAlg(buffer[i]->getKey(), new_bufSiz);
        int k = 0;
        while (new_buffer[h1] != NULL &&
               k < new_bufSiz) 
        {
          h1 = (h1 + h2) % new_bufSiz;
          k++;
        }
        new_buffer[h1] = buffer[i];
      } else  
      {
        delete buffer[i];
      }
      buffer[i] = NULL;
    }
    buffer = new_buffer;
    bufSiz = new_bufSiz;
}

template <class T>
uint8_t Hash_table<T>::add(const T& key_value) {
  if (((double)size / (double)bufSiz) >=
        FILL_REHASH_PERCENTAGE)  
    {
      rehashTable();
    }
    int h1 = createHashFirstAlg(key_value, bufSiz);
    int h2 = createHashSecondAlg(key_value, bufSiz);
    int i = 0;
    int first_deleted_elem = -1;

    while (buffer[h1] != NULL && i < bufSiz) {
      if (buffer[h1]->getKey() == key_value &&
          !buffer[h1]->exists())  
      {
        return OP_FAILURE;
      }
      if (buffer[h1]->exists() &&
          first_deleted_elem <
              0)  
      {
        first_deleted_elem = h1;
      }
      h1 = (h1 + h2) % bufSiz;
      i++;
    }

    if (first_deleted_elem < 0)  
    {
      buffer[h1] = new hashTableItem<T>(key_value);
    } else  
    {
      buffer[first_deleted_elem]->setKey(key_value);
      buffer[first_deleted_elem]->added();
    }
    size++;
    return OP_SUCCESS;
}

template <class T>
uint8_t Hash_table<T>::remove(const T& key_value) {
  int h1 = createHashFirstAlg(key_value, bufSiz);
    int h2 = createHashSecondAlg(key_value, bufSiz);
    int i = 0;
    while (buffer[h1] != NULL && i < bufSiz) {
      if (buffer[h1]->getKey() == key_value &&
          !buffer[h1]->exists()) 
      {
        buffer[h1]->deleteEX();
        size--;
        return OP_SUCCESS;
      }
      h1 = (h1 + h2) % bufSiz;
      i++;
    }
    return OP_FAILURE; 
}

template <class T>
uint8_t Hash_table<T>::contains(const T& key_value) const {
  int h1 = createHashFirstAlg(key_value, bufSiz);
    int h2 = createHashSecondAlg(key_value, bufSiz);
    int i = 0;
    while (buffer[h1] != NULL && i < bufSiz) {
      if (buffer[h1]->getKey() == key_value &&
          !buffer[h1]->exists())  //если нашли элемент
      {
        return OP_SUCCESS;
      }
      h1 = (h1 + h2) % bufSiz;
      i++;
    }
    return OP_FAILURE;  
}


int main() {
  Hash_table<std::string> hm;
  char command = ' ';
  std::string key("");
  uint8_t exitcode = false;

  while (std::cin >> command >> key) {
    switch (command) {
      case '+':
        exitcode = hm.add(key);
        break;
      case '?':
        exitcode = hm.contains(key);
        break;
      case '-':
        exitcode = hm.remove(key);
        break;
      default:
        continue;
    }

    if (exitcode == OP_SUCCESS) {
      std::cout << "OK" << std::endl;
    } else if (exitcode == OP_FAILURE) {
      std::cout << "FAIL" << std::endl;
    }
  }

  return 0;
}