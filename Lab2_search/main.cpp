#include <iostream>
#include <stdexcept>
#include <string>

#include "iDictionary.h"
#include "hashFunctions.h"
#include "hashTable.h"
#include "sortedSequence.h"
#include "person.h"
#include "binarySearchTree.h"
#include "avlTree.h"

// ---------- мини-проверялка ----------
int passed = 0, failed = 0;

void Check(bool condition, const std::string& name) {
    if (condition) {
        passed++;
        std::cout << "  [OK]   " << name << "\n";
    } else {
        failed++;
        std::cout << "  [FAIL] " << name << "\n";
    }
}

// true, если выражение бросило исключение
template <class F>
bool Throws(F action) {
    try { action(); } catch (const std::exception&) { return true; }
    return false;
}

int BadHash(const int&) { return 0; }      // все ключи в одну корзину
int NegHash(const int& k) { return -k; }   // отрицательный хеш

// ---------- хеш-таблица ----------
void TestHashTable() {
    std::cout << "HashTable\n";

    HashTable<int, int> t(&HashInt, 10);
    Check(t.GetCount() == 0 && t.GetCapacity() == 10, "empty table: count 0, capacity 10");
    t.Add(1990, 5);
    t.Add(2001, 7);
    Check(t.ContainsKey(1990) && !t.ContainsKey(1985), "ContainsKey");
    Check(t.Get(2001) == 7 && t.GetCount() == 2, "Get + count");
    Check(Throws([&] { t.Add(1990, 1); }), "Add duplicate -> exception");
    Check(Throws([&] { t.Get(12345); }), "Get missing -> exception");
    t.Remove(1990);
    Check(!t.ContainsKey(1990) && t.Get(2001) == 7, "Remove does not touch neighbours");
    Check(Throws([&] { t.Remove(1990); }), "Remove missing -> exception");

    HashTable<int, int> big(&HashInt, 10);
    for (int i = 0; i < 1000; i++) big.Add(i, i * 10);
    Check(big.GetCount() == 1000 && big.GetCapacity() > 1000, "grows on 1000 items");
    bool allFound = true;
    for (int i = 0; i < 1000; i++) allFound = allFound && big.Get(i) == i * 10;
    Check(allFound, "all 1000 found after rehash");
    int capBefore = big.GetCapacity();
    for (int i = 0; i < 990; i++) big.Remove(i);
    Check(big.GetCapacity() < capBefore && big.Get(995) == 9950, "shrinks after removals");

    HashTable<int, int> bad(&BadHash, 4);
    for (int i = 0; i < 100; i++) bad.Add(i, i);
    bool badOk = true;
    for (int i = 0; i < 100; i++) badOk = badOk && bad.Get(i) == i;
    Check(badOk, "bad hash (all collide) still works");

    HashTable<int, int> neg(&NegHash, 10);
    neg.Add(7, 70);
    Check(neg.Get(7) == 70, "negative hash handled");

    HashTable<std::string, int> names(&HashString, 4);
    names.Add("Ivanov", 1);
    names.Add("Petrov", 2);
    Check(names.Get("Ivanov") == 1 && names.Get("Petrov") == 2, "string keys");
}

// ---------- отсортированная последовательность ----------
void TestSortedSequence() {
    std::cout << "SortedSequence\n";

    SortedSequence<int> empty;
    Check(empty.GetIsEmpty() && empty.GetLength() == 0, "empty sequence");
    Check(Throws([&] { empty.GetFirst(); }), "GetFirst on empty -> exception");
    Check(empty.IndexOf(5) == -1, "IndexOf on empty -> -1");

    int a[] = {3, 7, 7, 12, 15, 15, 15, 21};
    SortedSequence<int> s(a, 8);
    Check(s.LowerBound(15) == 4 && s.UpperBound(15) == 7, "Lower/Upper bound of 15");
    Check(s.LowerBound(10) == 3 && s.UpperBound(10) == 3, "bounds of missing 10");
    Check(s.LowerBound(1) == 0 && s.UpperBound(21) == 8, "bounds at edges");
    Check(s.IndexOf(15) == 4 && s.IndexOf(10) == -1 && s.IndexOf(100) == -1, "IndexOf");

    s.SetSplit(618, 382);
    Check(s.LowerBound(15) == 4 && s.UpperBound(15) == 7, "golden ratio: same answers");
    s.SetSplit(1, 3);
    Check(s.LowerBound(15) == 4 && s.UpperBound(15) == 7, "1:3 split: same answers");
    Check(Throws([&] { s.SetSplit(0, 1); }), "SetSplit(0,1) -> exception");

    int bad[] = {3, 12, 7};
    Check(Throws([&] { SortedSequence<int> x(bad, 3); }), "unsorted array -> exception");

    SortedSequence<int> added;
    int values[] = {5, 1, 9, 5, 3};
    for (int v : values) added.Add(v);
    Check(added.GetLength() == 5 && added.Get(0) == 1 && added.Get(1) == 3 &&
          added.Get(2) == 5 && added.Get(3) == 5 && added.Get(4) == 9, "Add keeps order: 1 3 5 5 9");

    SortedSequence<int>* range = s.GetRange(7, 15);
    Check(range->GetLength() == 6 && range->GetFirst() == 7 && range->GetLast() == 15, "GetRange(7,15)");
    delete range;

    SortedSequence<int>* none = s.GetRange(8, 11);
    Check(none->GetIsEmpty(), "GetRange with nothing inside -> empty");
    delete none;

    SortedSequence<int>* reversed = s.GetRange(15, 7);
    Check(reversed->GetIsEmpty(), "GetRange(a > b) -> empty");
    delete reversed;

    Check(Throws([&] { s.GetSubsequence(3, 2); }), "GetSubsequence(3,2) -> exception");

    ISortedSequence<int>* viaInterface = &s;           // работа через интерфейс
    ISortedSequence<int>* piece = viaInterface->GetSubsequence(1, 4);
    Check(viaInterface->IndexOf(12) == 3 && piece->GetLength() == 4 && piece->GetFirst() == 7,
          "works through ISortedSequence*");
    delete piece;                                        // виртуальный деструктор -> корректно

    SortedSequence<int> r;
    SortedSequence<int>* part = s.GetRange(7, 15);
    SortedSequence<int>* all = s.GetRange(0, 100);
    r = *part;
    r = *all;
    delete part;
    delete all;
    Check(r.GetLength() == 8, "assignment of result (operator=)");
}

// ---------- Person ----------
void TestPerson() {
    std::cout << "Person\n";

    Person p("Ivan", "Petrovich", "Ivanov", 1990);
    Check(p.GetFullName() == "Ivanov Ivan Petrovich", "GetFullName");
    Check(p.GetFIO() == "Ivanov I.P.", "GetFIO");

    Person noMiddle("Ivan", "", "Ivanov", 1990);
    Check(noMiddle.GetFIO() == "Ivanov I.", "GetFIO without middle name");
    Check(noMiddle.GetFullName() == "Ivanov Ivan", "GetFullName without middle name");

    Check(p.GetAge(2026) == 36, "GetAge");
    Check(Throws([&] { p.GetAge(1980); }), "GetAge before birth -> exception");

    Person empty;
    Check(empty.GetLastName().empty() && empty.GetBirthYear() == 0, "default constructor");

    MutableArraySequence<Person> people;
    people.Append(p);
    people.Append(Person("Anna", "Sergeevna", "Petrova", 2001));
    Check(people.GetLength() == 2 && people[1].GetLastName() == "Petrova", "Person in MutableArraySequence");
}

// ---------- обычное дерево поиска ----------
void TestBinarySearchTree() {
    std::cout << "BinarySearchTree\n";

    BinarySearchTree<int> t;
    Check(t.GetCount() == 0, "empty tree: count 0");
    Check(t.GetHeight() == 0, "empty tree: height 0");

    int vals[] = {50, 30, 70, 20, 40, 60, 80};
    for (int v : vals) t.Add(v);
    Check(t.GetCount() == 7, "Add: count 7");
    Check(t.Contains(40) && !t.Contains(45), "Contains");
    Check(t.Contains(20) && t.Contains(80) && !t.Contains(10) && !t.Contains(90), "Contains: leaves and outside values");
    Check(t.GetHeight() == 3, "balanced input: height 3");
    Check(Throws([&] { t.Add(30); }), "Add duplicate -> exception");
    Check(t.GetCount() == 7, "count unchanged after failed Add");

    std::string order;
    t.ForEach([&](const int& x) { order += std::to_string(x) + " "; });
    Check(order == "20 30 40 50 60 70 80 ", "ForEach: in-order is sorted");

    int sum = 0;
    t.ForEach([&](const int& x) { sum += x; });
    Check(sum == 350, "ForEach: sum via lambda");

    BinarySearchTree<int> line;
    for (int i = 1; i <= 100; i++) line.Add(i);
    Check(line.GetHeight() == 100, "sorted input: degenerates to a list (height 100)");
}

// ---------- AVL-дерево ----------
void TestAVLTree() {
    std::cout << "AVLTree\n";

    AVLTree<int> asc;
    for (int i = 1; i <= 100; i++) asc.Add(i);
    Check(asc.GetHeight() == 7 && asc.GetCount() == 100, "sorted input 1..100: height 7 (BST had 100)");

    AVLTree<int> desc;
    for (int i = 100; i >= 1; i--) desc.Add(i);
    Check(desc.GetHeight() == 7, "reverse input 100..1: height 7");

    AVLTree<int> zig;
    zig.Add(30); zig.Add(10); zig.Add(20);
    Check(zig.GetHeight() == 2, "zigzag 30,10,20: double rotation, height 2");

    std::string order;
    asc.ForEach([&](const int& x) { order += std::to_string(x) + " "; });
    std::string expected;
    for (int i = 1; i <= 100; i++) expected += std::to_string(i) + " ";
    Check(order == expected, "rotations keep in-order sorted");

    bool allFound = true;
    for (int i = 1; i <= 100; i++) allFound = allFound && asc.Contains(i);
    Check(allFound && !asc.Contains(0) && !asc.Contains(101), "all found after rotations");

    Check(Throws([&] { asc.Add(50); }), "AVL: duplicate -> exception");

    BinarySearchTree<int>* viaBase = new AVLTree<int>();
    for (int i = 1; i <= 100; i++) viaBase->Add(i);
    Check(viaBase->GetHeight() == 7, "virtual Rebalance works through base pointer");
    delete viaBase;
}

int main() {
    TestHashTable();
    TestSortedSequence();
    TestPerson();
    TestBinarySearchTree();
    TestAVLTree();
    std::cout << "\nPassed: " << passed << ", failed: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
