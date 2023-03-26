#include <iostream>
#include <stack>

template <typename T>
__interface Iterator {
	virtual bool hasMore() const = 0;
	virtual T getNext() = 0;
};

///////////////////////////////////////////////////////////////////

template <typename T>
__interface IterableCollection {
	virtual Iterator<T>* createIterator() = 0;
};

///////////////////////////////////////////////////////////////////

template <typename T>
struct Node {
	T value;
	Node<T>* parent = nullptr;
	Node<T>* right = nullptr;
	Node<T>* left = nullptr;
	Node(T val) : value(val) {}
	void showNode() const {
		std::cout << value << ", ";
	}
};

///////////////////////////////////////////////////////////////////

template <typename T>
class BinaryTree;

template <typename T>
class BinaryTreeIterator : public Iterator<T> {
public:
	BinaryTreeIterator(BinaryTree<T>* tree, Node<T>* start) {
		this->tree = tree;
		current = start;
	}

	bool hasMore() const override {
		return current != nullptr || !nodeStack.empty();
	}

	T getNext() override {
		if (current == nullptr && !nodeStack.empty()) {
			current = nodeStack.top();
			nodeStack.pop();
		}
		else if (current == nullptr && nodeStack.empty()) {
			throw "tree is empty";
		}

		T result = current->value;
		if (current->right != nullptr) {
			nodeStack.push(current->right);
		}
		current = current->left;
		if (current == nullptr && !nodeStack.empty()) {
			current = nodeStack.top();
			nodeStack.pop();
		}
		return result;
	}

	/////////////////////////////////////////////////////////////////

	bool operator!=(const BinaryTreeIterator<T>& other) const {
		return current != other.current;
	}

	T operator*() const {
		return current->value;
	}

	T* operator->() const {
		return &(current->value);
	}

	BinaryTreeIterator<T> operator++() {
		getNext();
		return *this;
	}

private:
	BinaryTree<T>* tree = nullptr;
	std::stack<Node<T>*> nodeStack;
	Node<T>* current = nullptr;
};

///////////////////////////////////////////////////////////////////

template <typename T>
class BinaryTree : public IterableCollection<T> {
	Node<T>* root = nullptr;

public:
	bool isEmpty() const {
		return root == nullptr;
	}

	BinaryTree() {}

	~BinaryTree() {
		clear();
	}

	Node<T>* getRoot() const {
		return root;
	}

	void showTree() const {
		std::cout << "\n";
		showTree(root);
		std::cout << "\n";
	}

private:
	void showTree(Node<T>* element) const {
		if (element != nullptr) {
			showTree(element->left);
			element->showNode();
			showTree(element->right);
		}
	}

public:
	void clear() {
		if (isEmpty())
			return;

		std::stack<Node<T>*> nodeStack;
		nodeStack.push(root);
		while (!nodeStack.empty()) {
			Node<T>* current = nodeStack.top();
			nodeStack.pop();
			if (current->right != nullptr)
				nodeStack.push(current->right);
			if (current->left != nullptr)
				nodeStack.push(current->left);
			delete current;
		}
		root = nullptr;
	}

public:
	Node<T>* findNode(const T& value) const {
		return findNodeHelper(root, value);
	}

private:
	Node<T>* findNodeHelper(Node<T>* current, const T& value) const {
		if (current == nullptr || current->value == value)
			return current;
		if (value < current->value) 
			return findNodeHelper(current->left, value);
		return findNodeHelper(current->right, value);
	}

public:
	void addNode(T value) {
		auto n = new Node<T>(value);
		Node<T>* parent = nullptr;
		auto current = root;

		while (current != nullptr) {
			parent = current;
			if (n->value < current->value)
				current = current->left;
			else 
				current = current->right;
		}

		if (parent == nullptr)
			root = n;
		else if (n->value < parent->value) 
			parent->left = n;
		else 
			parent->right = n;
		n->parent = parent;
	}

	// остальные методы дерева не реализованы по ненадобности

public:
	BinaryTreeIterator<T>* createIterator()  override { // iterator pattern method
		return new BinaryTreeIterator<T>(this, root);
	}

	////////////////////////////////////////////////////////////////////
	// range-based-for methods:
	BinaryTreeIterator<T> begin() {
		return *createIterator();
	}

	BinaryTreeIterator<T> end() {
		return BinaryTreeIterator<T>(this, nullptr);
	}

};

int main() {
	srand(time(0));
	BinaryTree<int> bt;
	for (int i = 0; i < 10; i++)
		bt.addNode(rand() % 100);

	// bt.showTree(); // показ дерева при наличии такого метода в классе

	////////////////////////////////////////////////////////////////////
	auto iter = bt.createIterator(); // использование паттерна Iterator
	while (iter->hasMore())
		std::cout << iter->getNext() << " ";
	delete iter;
	std::cout << "\n\n";
	////////////////////////////////////////////////////////////////////
	// cpp-specific range-based-for implementation
	// https://stackoverflow.com/questions/16504062/how-to-make-the-for-each-loop-function-in-c-work-with-a-custom-class
	for (auto elem : bt)
		std::cout << elem << " ";
	std::cout << "\n\n";
	////////////////////////////////////////////////////////////////////
	// строка for (auto elem : bt) { раскрывается в такой код:
	for (BinaryTreeIterator<int> iterator = bt.begin(); iterator != bt.end(); ++iterator) {
		int elem = *iterator;
		std::cout << elem << " ";
	}
	std::cout << "\n\n";
}

/*
этот код - реализация двоичного дерева поиска и его итератора в C++,
а также его использование с помощью диапазонного for и паттерна Iterator.

Класс BinaryTree определяет узлы дерева, методы для добавления, удаления и
поиска узлов, а также реализует интерфейс IterableCollection, который содержит
только один метод createIterator, который возвращает указатель на итератор
типа BinaryTreeIterator<T> для перебора узлов дерева.

Класс BinaryTreeIterator реализует интерфейс Iterator и перебирает узлы
дерева в порядке "левый, корневой, правый", используя стек для сохранения
узлов. Он содержит методы для проверки наличия следующего узла (hasMore) и
получения следующего узла (getNext). Он также перегружает операторы для
сравнения (!=), получения значения (*) и доступа к члену структуры (->),
что позволяет использовать итератор в циклах while и for.

main создает объект BinaryTree, заполняет его случайными числами,
использует итератор для перебора всех узлов дерева с помощью цикла while,
а затем использует цикл for для перебора всех узлов с помощью диапазонного for.
*/

// https://refactoring.guru/ru/design-patterns/iterator

/*
Итератор — это поведенческий паттерн проектирования, который даёт возможность
последовательно обходить элементы составных объектов, не раскрывая их
внутреннего представления.
*/