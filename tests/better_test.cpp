#include "virus_genealogy.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>

class Virus {
public:
    typedef std::string id_type;
    Virus(id_type const &_id) : id(_id) {
    }
    id_type get_id() const {
        return id;
    }
private:
    id_type id;
};

template<typename _id_type>
class _Virus {
public:
    typedef _id_type id_type;
    _Virus(id_type const &_id): id(_id) {
    }
    id_type get_id() const {
        return id;
    }
private:
    id_type id;
};

void test_1() {
    VirusGenealogy<Virus> Gene("root");
    Gene.create("1", "root");
    Gene.create("2", "1");
    Gene.connect("2", "root");

    Gene.remove("1");

    auto vec = Gene.get_children("root");
    assert(std::find(vec.begin(), vec.end(), "1") == vec.end());
    assert(std::find(vec.begin(), vec.end(), "2") != vec.end());

    Gene.create("1", "root");
    Gene.create("3", "2");
    Gene.connect("3", "1");

    Gene.remove("1");
    assert(Gene.exists("3"));
    Gene.remove("2");
    assert(!Gene.exists("3"));

    bool no_exception = true;
    try {
        Gene.connect("root", "3");
    } catch (std::exception &e) {
        assert(e.what() == "VirusNotFound");
        no_exception = false;
    }
    assert(!no_exception);
}

void test_2() {
    VirusGenealogy<Virus> Gene("root");
    Gene.create("1", "root");
    Gene.create("2", "root");
    Gene.connect("2", "root");
    assert((Gene.get_children("root")).size() == 2);
}

void test_3() {
    VirusGenealogy<Virus> Gene("root");
    bool no_exception = true;
    try {
        Gene.connect("1", "root");        
    } catch (std::exception &e) {
        assert(e.what() == "VirusNotFound");
        no_exception = false;
    }
    assert(!no_exception);
    no_exception = true;
    try {
        Gene.create("root", "root");
    } catch (std::exception &e) {
        assert(e.what() == "VirusAlreadyCreated");
        no_exception = false;
    }
    assert(!no_exception);
    no_exception = true;
    try {
        Gene.remove("root");
    } catch (std::exception &e) {
        assert(e.what() == "TriedToRemoveStemVirus");
        no_exception = false;
    }
    assert(!no_exception);
}

void test_4() {
    VirusGenealogy<Virus> Gene("root");
    bool no_exception = true;
    Gene.create("1", "root");
    Gene.create("2", "1");
    Gene.create("3", "2");
    Gene.create("4", "3");
    Gene.create("5", "4");
    Gene.connect("5", "1");
    Gene.create("6", "2");
    Gene.connect("6", "root");
    Gene.remove("1");
    assert(!Gene.exists("1"));
    assert(!Gene.exists("2"));
    assert(!Gene.exists("3"));
    assert(!Gene.exists("4"));
    assert(!Gene.exists("5"));
    assert(Gene.exists("6"));
    try {
        Gene.connect("5", "6");
    } catch (std::exception &e) {
        assert(e.what() == "VirusNotFound");
        no_exception = false;
    }
    assert(!no_exception);
}

void test_5() {
    VirusGenealogy<Virus> Gene("root");
    Gene.create("1", "root");
    Gene.create("2", "1");
    for (int i = 3; i <= 1000; ++i) {
        std::string is = std::to_string(i);
        Gene.create(is, "2");
    }
    for (int i = 1; i <= 500; ++i) {
        std::string is = std::to_string(i);
        Gene.connect(is, "1");
    }
    Gene.remove("2");
    assert(!Gene.exists("1000"));
    assert(!Gene.exists("501"));
    assert(Gene.exists("500"));
    Gene.remove("1");
    assert(!Gene.exists("500"));
}

int main() {
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();

    std::cout << "Correct!\n";

    return 0;
}
