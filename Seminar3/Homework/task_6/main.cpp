/*
 * Дано число N < 106 и последовательность целых чисел из [-231..231] длиной N.
 * Требуется построить бинарное дерево, заданное наивным порядком вставки.
 * Т.е., при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K, то узел K добавляется в правое поддерево root; иначе в левое поддерево root.
 * Рекурсия запрещена.
 *
 * 6_1. Выведите элементы в порядке pre-order (сверху вниз).
 */



#include <iostream>
#include <vector>

// Узел бинарного дерева
struct TreeNode {
  explicit TreeNode(int _value) : value(_value) {}
  int value = 0;
  TreeNode *parent = nullptr;

  TreeNode *left = nullptr;
  TreeNode *right = nullptr;

};

class Tree {
 public:
  ~Tree();

  void Print();

  void Add(int value);

 protected:

  TreeNode *root = nullptr;
  void print_subtree(TreeNode *node);

  void delete_subtree(TreeNode *node);

  TreeNode *move_up(TreeNode *node);

  TreeNode *next(TreeNode *node);
};

Tree::~Tree() {
  delete_subtree(root->left);
  delete_subtree(root->right);
  delete root;
}

/*
 * Необходимо организовать проход дерева без рекурсии
 * Это можно сделать следующим образом: спускаемся влево столько, сколько можем.
 * Когда дошли до края - поднимаемся наверх до тех пор, пока не встанем на вершину, у которой есть правый потомок.
 * Когда нашли такую вершину - спускаемся на одну ноду вправо, а дальше опять спускаемся влево.
 * Если поднимаемся ИЗ ПРАВОГО поддерева, то смотрим на то, чтобы правый потомок вершины, в которой мы сейчас стоим,
 * не был той же вершиной, из которой мы пришли.
 * Повторяем, пока не пройдем все дерево.
 * Собственно, move_up как раз и реализует такой подъем
 */
TreeNode *Tree::move_up(TreeNode *node) {
  if (!node->parent) { return node; }
  TreeNode *parent = node->parent;
  while (parent->right == node or parent->right == nullptr) {
    node = parent;
    parent = parent->parent;
    if (parent == nullptr) { return node; }
  }
  return parent;
}

// Вспомогательная функция для спуска/подъема
TreeNode *Tree::next(TreeNode *node) {
  if (node->left) { return node->left; }
  if (node->right) { return node->right; }
  return node->parent;
}

void Tree::delete_subtree(TreeNode *node) {
  if (!node) return;
  TreeNode *n = next(node);
  TreeNode *parent = node->parent;
  if (!parent) { // если у ноды нет родителя - то это корень, и пора заканчивать
    delete node;
    return;
  }
  while (n != parent) {
    if (n == node->parent) {
      if (n->left) {
        n->left = nullptr;
      } else { n->right = nullptr; }
      delete node;
    }
    node = n;
    n = next(node);
  }
}

void Tree::Print() {
  std::cout << root->value << " ";  //Реализация pre-order вывода. Сначала выводим корень
  print_subtree(root->left);  // потом левое поддерево
  print_subtree(root->right); // потом правое
}

void Tree::print_subtree(TreeNode *node) {
  if (!node) return;
  TreeNode *parent;
  while (node) {  // собственно, обходим дерево с помощью алгоритма описанного выше
    std::cout << node->value << " ";
    while (node->left) {  // покуда можем спускаться налево
      node = node->left; // спускаемся
      std::cout << node->value << " ";
    }
    if (!node->right) {  // если не сожем шагнуть вправо
      parent = move_up(node);  // то поднимаемся наверх
      if (parent == root) { return; }  // если вернулись в корень - поддерево распечатано
      else { node = parent->right; } // иначе - шагаем вправо и продолжаем движение
    }
    else { node = node->right; }
  }
}

void Tree::Add(int value) {  // не изменилось с семинара, так как у наследника метод переопределен
  if (!root) {
    root = new TreeNode(value);
  }
}


class NaiveTree : public Tree {
 public:
  void Add(int value);
 private:
  TreeNode *add_left(TreeNode *node, int value);
  TreeNode *add_right(TreeNode *node, int value);
};

void NaiveTree::Add(int value) {
  if (!root) {
    root = new TreeNode(value);
    return;
  }
  TreeNode *node = root;
  /*
   * Функции add_left и add_right добавляют ноду в левое или правое поддерево
   * и возвращают корень поддерева, куда значение добавили
   */
  while (node) {  // покуда мы добавляем в поддерево - цикл идет
    if (node->value >= value) {
      node = add_left(node, value);
    } else {
      node = add_right(node, value);
    }
  }  // как только новая нода сама стала корнем поддерева - прекращаем
}

TreeNode *NaiveTree::add_left(TreeNode *node, int value) {
  if (node->left) { return node->left; }
  else {
    node->left = new TreeNode(value);
    node->left->parent = node;
    return nullptr;
  }
}

TreeNode *NaiveTree::add_right(TreeNode *node, int value) {
  if (node->right) { return node->right; }
  else {
    node->right = new TreeNode(value);
    node->right->parent = node;
    return nullptr;
  }
}

int main() {
  int n;
  int value;
  NaiveTree tree;
  std::cin >> n;
  for (int i = 0; i < n; i++) {
    std::cin >> value;
    tree.Add(value);
  }
  tree.Print();
  return 0;
}