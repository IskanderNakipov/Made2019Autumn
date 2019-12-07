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
#include <utility>
#include <vector>

class HashTable {
 public:
  explicit HashTable(size_t initial_size);
  ~HashTable() = default;;
  HashTable(const HashTable&) = delete;
  HashTable(HashTable&&) = delete;
  HashTable& operator=(const HashTable&) = delete;
  HashTable& operator=(HashTable&&) = delete;

  bool Has(const std::string& key) const;
  bool Add(const std::string& key);
  bool Remove(const std::string& key);

 private:
  struct HashTableNode {
    HashTableNode() = default;
    ~HashTableNode() = default;
    HashTableNode(const HashTable&) = delete;
    HashTableNode(HashTable&&) = delete;
    HashTableNode& operator=(const HashTable&) = delete;
    HashTableNode& operator=(HashTable&&) = delete;
    std::string key = "";
    bool deleted = false;
    explicit HashTableNode(std::string key_) : key(std::move(key_)) {};
    explicit operator bool() {return !key.empty();}
  };

  const int base_1=33113;
  const int base_2=33213;
  unsigned long num_elements=0;
  std::vector<HashTableNode> table;

  void rehash();
  void set(unsigned long hash, std::string key);
  static size_t Hash(const std::string &str, int base);
  size_t Hash_2(const std::string &str) const {return 2*Hash(str, base_2) + 1;}
  static bool is_empty(HashTableNode node) {return !node && !node.deleted;}
  std::pair<unsigned long, bool>  Find(const std::string &key) const;
  std::pair<unsigned long, bool> Find(const std::string &key, std::vector<HashTableNode> table_) const;
};

HashTable::HashTable(size_t initial_size) {
  table = std::vector<HashTableNode>(initial_size);
}

std::pair<unsigned long, bool> HashTable::Find(const std::string &key, std::vector<HashTableNode> table_) const {
  /*
   * Находит либо ячейку с ключом, либо первую пустую, возвращает ее номер.
   * Помимо этого, возвращает флаг того, нашелся ли элемент, или нет
   */
  assert(!key.empty());

  size_t hash = Hash(key, base_1) % table_.size();
  size_t step = Hash_2(key);
  for (unsigned long i=0; i<table_.size(); i++) {  //пока не пройдем все ячейки таблицы
    if (is_empty(table_[hash])) { return std::make_pair(hash, false); } // если пустая не удаленная нода
    if (table_[hash]) {
      if (table_[hash].key == key) { return std::make_pair(hash, true); }  // если нашли наш ключ
    }
    hash += step;
    hash = hash % table_.size();
  }
  return std::make_pair(hash, false);
}

std::pair<unsigned long, bool> HashTable::Find(const std::string &key) const {  //для удобства
  return Find(key, table);
}

void HashTable::rehash() {
  // перехеширование
  unsigned long index;
  std::vector<HashTableNode> new_table = std::vector<HashTableNode>(table.size()*2);
  for (auto & i : table) {
    if (i) {
      index = Find(i.key, new_table).first;
      new_table[index] = i;
      new_table[index].deleted = false;
    }
  }
  std::swap(table, new_table);
  std::vector<HashTableNode>().swap(new_table);
}


size_t HashTable::Hash(const std::string &str, const int base) {
  // подсчет полиномиального кэша с заданным основанием
  int hash = 0;
  for (char c : str) {
    hash = (hash*base + c);
  }
  return hash;
}

bool HashTable::Has(const std::string& key) const {
  return Find(key).second;
}

void HashTable::set(unsigned long hash, std::string key) {
  table[hash].key = std::move(key);
  if (!table[hash].deleted) num_elements++;  // как говорил Степан, правильнее будет оценивать кол-во элементов не по количеству непустых ключей,
  table[hash].deleted = false;               // а по количеству непустых ключей и удаленных ячеек
  if (4*num_elements >= 3*table.size()) rehash();
}

bool HashTable::Add(const std::string& key) {
  size_t hash = Hash(key, base_1) % table.size();
  size_t step = Hash_2(key);
  unsigned long vacant = table.size();
  for (unsigned long i=0; i<table.size(); i++) {  //пока не пройдем все ячейки таблицы
    if (table[hash].key == key) {return false;}  // нашли наш ключ
    if (!table[hash]) {  // если нашли пустой ключ - отмечаем, что в случае чего
      vacant = (vacant == table.size()) ? hash : vacant; // поставим значение в эту ячейку
    }
    if (is_empty(table[hash])){
      set(hash, key);
      return true;
    }
    hash += step;
    hash = hash % table.size();
  }
  assert(vacant != table.size()); // данная ситуация не должна произойти - это бы значило, что вся таблица заполнена
  set(vacant, key);
  return true;
}

bool HashTable::Remove(const std::string& key) {
  std::pair<int, bool> index = Find(key);
  if (!index.second) { return false; } // ненаход
  table[index.first].key = "";
  table[index.first].deleted = true;
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