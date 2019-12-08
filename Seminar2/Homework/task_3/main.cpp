/* Дана очень длинная последовательность целых чисел длины n. Требуется вывести в отсортированном виде её наименьшие k элементов.
 * Последовательность может не помещаться в память. Время работы O(n * log(k)). Доп. память O(k). Использовать слияние. */


#include<iostream>
#include<string>

class Range {  // попытка заменить питоновский range.
 private:
  int end_val;
  int state;
 public:
  int next();
  bool done() { return state == end_val; };
 public:
  Range(int I, int I1);
};

int Range::next() {
  state += 1;
  return state - 1;
}
Range::Range(int I, int I1) {
  end_val = I;
  state = I1;

}

void merge(int arr[], int start_1, int end_1, int start_2, int end_2, int target[]) {
  /*
   * Представим, что множества, которые надо слить, хранятся в одном массиве, причем границы множеств мы знаем (end_i)
   * Результат слияния будем записывать в переменную target
   */
  Range iter_1 = {end_1, start_1};  //переменные, используемые для выяснения границ итереривания
  Range iter_2 = {end_2, start_2};
  int i = start_1;
  int j = start_2;
  bool change = true; // некоторый флаг, значение которого стагновится ясным далее

  while (change) {
    change = false;
    while (!iter_1.done()) {  // пока незакончилось множество, по которому мы сейчас итерируемся
      i = iter_1.next();
      if (arr[i] <= arr[j]) {
        target[i + j - start_2] = arr[i];
      } else {
        target[i + j - start_2] = arr[j];
        std::swap(i, j);;          // если меньшим оказался объект из второго множества - переворачиваем игру
        std::swap(iter_1, iter_2); // и начинаем итерироваться по нему
        change = true; // говорим, что мы вышли из цикла из-за смены итераторов
        break;
      }
    } // если вышли из цикла из-за того, что один из итераторов закончился - закидываем оставшиеся значения второго в конец
  }
  std::swap(i, j);
  j += 1;
  if (i + j - start_2 == end_2 - 1) {
    target[i + j - start_2] = arr[i];
  } else {
    while (!iter_2.done()) {
      target[i + j - start_2] = arr[i];
      i = iter_2.next();
    }
  }
  for (int I = start_1; I < end_2; I++) {  // переписываем значения target в оригинал
    arr[I] = target[I];
  }
}

int read_batch(int batch[], int len, int total, int already_read, int start) {
  //считывание k элементов, но так, чтобы не считать лишнего
  int i; //сколько элементов считали
  for (i = 0; (i < len) & (i + already_read < total); i++) {
    std::cin >> batch[i + start];
  }
  return i;
}

void merge_sort(int arr[], int start, int end, int target[]) {
  if (end - start == 1) {
    return;
  }
  int mid = start + (end - start) / 2; //т.к. значения хранятся в одном массиве - нужно общаться номерами элементов
  merge_sort(arr, start, mid, target);
  merge_sort(arr, mid, end, target);
  merge(arr, start, mid, mid, end, target);
}

int main() {
  int n;
  int k;
  int batch_size;
  std::cin >> n >> k;
  int elements_count = 0;
  int k_smallest[2 * k];
  int target[2 * k];
  elements_count += read_batch(k_smallest, k, n, 0, 0);
  merge_sort(k_smallest, 0, k, target);
  while (elements_count < n) {
    batch_size = read_batch(k_smallest, k, n, elements_count, k);
    elements_count += batch_size;
    merge_sort(k_smallest, k, k + batch_size, target);
    merge(k_smallest, 0, k, k, k + batch_size, target);
  }
  for (int I = 0; I < k; I++) {
    std::cout << k_smallest[I] << ' ';
  }
}