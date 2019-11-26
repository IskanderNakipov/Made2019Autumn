/*
 * Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы с открытой адресацией.
 * Хранимые строки непустые и состоят из строчных латинских букв.
 * Хеш-функция строки должна быть реализована с помощью вычисления значения многочлена методом Горнера.
 * Начальный размер таблицы должен быть равным 8-ми.
 * Перехеширование выполняйте при добавлении элементов в случае, когда коэффициент заполнения таблицы достигает 3/4.
 * Структура данных должна поддерживать операции добавления строки в множество,
 * удаления строки из множества и проверки принадлежности данной строки множеству.
 * 1_2. Для разрешения коллизий используйте двойное хеширование.
 */


#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

class HashTable {
 public:
  explicit HashTable(size_t initial_size);
  ~HashTable();
  HashTable(const HashTable&) = delete;
  HashTable(HashTable&&) = delete;
  HashTable& operator=(const HashTable&) = delete;
  HashTable& operator=(HashTable&&) = delete;

  bool Has(std::basic_string<char> key);
  bool Add(const std::string& key);
  bool Remove(const std::string& key);

 private:
  int base_1=33113;
  int base_2=33213;
  unsigned long num_elements=0;
  std::vector<bool> deleted;
  struct HashTableNode {
    std::string key;
    explicit HashTableNode(std::string key_) : key(std::move(key_)) {}
  };
  std::vector<HashTableNode*> table;

  void rehash();
  static size_t Hash(const std::string &str, int &base);
  size_t Hash_2(const std::string &str) {return 2*Hash(str, base_2) + 1;}
  static bool is_empty(size_t hash, std::vector<HashTableNode*>& table_, const std::vector<bool>& deleted_) {
    return !table_[hash] && !deleted_[hash];
  }
  std::pair<unsigned long, bool>  Find(const std::string &key);
  std::pair<unsigned long, bool>  Find(const std::string& key, std::vector<HashTableNode*>& table_, const std::vector<bool>& deleted_);
};

HashTable::HashTable(size_t initial_size) {
  table = std::vector<HashTableNode*>(initial_size, nullptr);
  deleted = std::vector<bool>(initial_size, false);
}

std::pair<unsigned long, bool>  HashTable::Find(const std::string& key, std::vector<HashTableNode*>& table_, const std::vector<bool>& deleted_) {
  /*
   * Находит либо ячейку с ключом, либо первую пустую, возвращает ее номер.
   * Помимо этого, возвращает флаг того, нашелся ли элемент, или нет
   */
  assert(!key.empty());

  size_t hash = Hash(key, base_1) % table_.size();
  size_t step = Hash_2(key);
  for (unsigned long i=0; i<table_.size(); i++) {  //пока не пройдем все ячейки таблицы
    if (is_empty(hash,table_,  deleted_)) { return std::make_pair(hash, false); } // если пустая не удаленная нода
    if (table_[hash]) {
      if (table_[hash]->key == key) { return std::make_pair(hash, true); }  // если нашли наш ключ
    }
    hash += step;
    hash = hash % table_.size();
  }
  return std::make_pair(hash, false);
}

std::pair<unsigned long, bool> HashTable::Find(const std::string &key) {  //для удобства
  return Find(key, table, deleted);
}

void HashTable::rehash() {
  // перехеширование
  unsigned long index;
  std::vector<HashTableNode*> new_table = std::vector<HashTableNode*>(table.size()*2, nullptr);
  deleted = std::vector<bool>(new_table.size(), false);
  for (auto & i : table) {
    if (i) {
      index = Find(i->key, new_table, deleted).first;
      new_table[index] = i;
    }
  }
  table = new_table;
}


HashTable::~HashTable() {
  for (HashTableNode* node : table) {
    delete node;
  }
}

size_t HashTable::Hash(const std::string &str, int &base) {
  // подсчет полиномиального кэша с заданным основанием
  int hash = 0;
  for (char c : str) {
    hash = (hash*base + c);
  }
  return hash;
}

bool HashTable::Has(std::basic_string<char> key) {
  return Find(key).second;
}

bool HashTable::Add(const std::string& key) {
  if (Find(key).second) {return false; }  // если нашли значение - возвращаем false
  size_t hash = Hash(key, base_1) % table.size();
  size_t step = Hash_2(key);
  for (unsigned long i=0; i<table.size(); i++) {  //пока не пройдем все ячейки таблицы
    if (!table[hash]) {  // либо пока не найдем пустую
      break;
    }
    hash += step;
    hash = hash % table.size();
  }
  table[hash] = new HashTableNode(key);
  deleted[hash] = false;
  num_elements++;
  if (4*num_elements >= 3*table.size()) {rehash();}
  return true;
}

bool HashTable::Remove(const std::string& key) {
  std::pair<int, bool> index = Find(key);
  if (!index.second) { return false; } // ненаход
  delete table[index.first];
  table[index.first] = nullptr;
  deleted[index.first] = true;
  num_elements--;
  return true;
}

int main() {
  HashTable table(8);
  char command = ' ';
  std::string value;
  while (std::cin >> command >> value) {
    switch (command) {
      case '?':
        std::cout << (table.Has(value) ? "OK" : "FAIL") << std::endl;
        break;
      case '+':
        std::cout << (table.Add(value) ? "OK" : "FAIL") << std::endl;
        break;
      case '-':
        std::cout << (table.Remove(value) ? "OK" : "FAIL") << std::endl;
        break;
    }
  }
  return 0;
}