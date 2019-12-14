from typing import Tuple, Union
from random import random


class TreapNode:
    def __init__(self, value, priority, left=None, right=None, parent=None):
        """
        Вершина декартова дерева
        :param value: значение
        :param priority: приоритет
        :param left: левый потомок
        :param right: правый потомок
        :param parent: родитель
        """
        self.value = value
        self.priority = priority
        self.parent = parent
        self.left = left
        self.right = right
        self.power = 1  # сколько вершин в данном поддереве - повзоляет получить логарифмический поиск по индексу
        if left is not None:
            self.power += left.power
        if right is not None:
            self.power += right.power

    def change_power(self, by):
        """
        Меняет указанное количество вершин в поддереве, а так же у родителей
        :param by: на какое количество следует изменить
        """
        self.power -= by
        if self.parent is not None:
            self.parent.change_power(by)

    def find(self, index):
        """
        Находит элемент по индексу в поддереве
        :param index: идекс элемента (по убыванию роста)
        :return: Вершину с данным индексом
        """
        if self.right is not None:  # самые высокие - справа, оттуда и начинаем искать
            if self.right.power >= index:
                return self.right.find(index)
            index -= self.right.power  # если в правом поддереве не нашлось искомого - нужно сместить индекс
        elif index == 0:
            return self
        index -= 1  # если корень не подошел - тоже смещаем
        if self.left is not None:
            return self.left.find(index)


class Treap:
    def __init__(self, root: TreapNode = None):
        """
        Реализация декартова дерева
        :param root: корневая вершина
        """
        self.root = root

    def orphanize(self, *args):
        """Убирает у всех нод родителей"""
        for node in args:
            if node is not None:
                node.parent = None

    def split(self, node: TreapNode, value: int) -> Tuple[Union[TreapNode, None], Union[TreapNode, None]]:
        """
        Делит поддерево на два дерева по значению
        :param node: ссылка на корень поддерева
        :param value: делитель
        :return: два поддерева
        """
        if node is None:
            return None, None
        if node.value < value:
            first, second = self.split(node.right, value)
            self.orphanize(first, second)  # поскольку поддеревья отделены от предка, нужно ссылку на предка убрать
            node.right = first
            if second is not None:
                node.change_power(second.power)  # это поддерево перешло от нашего корня => уменьшаем мощность основного поддерева
            return node, second
        else:
            first, second = self.split(node.left, value)
            self.orphanize(first, second)
            node.left = second
            if first is not None:
                node.change_power(first.power)
            return first, node

    def merge(self, left: TreapNode, right: TreapNode) -> TreapNode:
        """
        Сливает два поддерева
        """
        if left is None: return right
        if right is None: return left

        if left.priority > right.priority:
            new_right = self.merge(left.right, right)
            return TreapNode(left.value, left.priority, left.left, new_right, parent=left)
        else:
            new_left = self.merge(left, right.left)
            return TreapNode(right.value, right.priority, new_left, right.right, parent=right)

    def add(self, value) -> int:
        """
        Добавляет ноду с указанным значением и возвращает ее индекс
        """
        if self.root is None:
            self.root = TreapNode(value, random())
            return 0
        node = TreapNode(value, random())
        first, second = self.split(self.root, value)
        node = self.merge(first, node)
        self.root = self.merge(node, second)
        self.root.parent = None  # на случай, если у нового корня был родитель
        if second is not None:  # позиция у новой вершины соответствует мощности правого от нее поддерева
            return second.power
        else:
            return 0

    def find(self, index):
        """
        Ищет вершину по индексу
        """
        return self.root.find(index)

    def remove(self, index):
        """
        Удаляет вершину по индексу
        """
        node = self.find(index)  # сначала находим вершину
        left, right = self.split(self.root, node.value)  # находим всех, кто слева, либо в одном поодереве от нее
        _ ,right = self.split(right, node.value + 0.5)  # слева останется только наша вершина, справа - все кто больше (в силу уникальности значений роста солдат)
        self.root = self.merge(left, right)
        del right


def main():
    import gc
    gc.disable()
    treap = Treap()
    n = int(input())
    for _ in range(n):
        command, value = input().split()
        value = int(value)
        if command == '1':
            print(treap.add(value))
        else:
            treap.remove(value)


if __name__ == '__main__':
    main()
