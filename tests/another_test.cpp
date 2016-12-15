#include "virus_genealogy.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct MyType {
    string s_;
    MyType(const char *s) : s_(s) {};
    string show() const { return s_; }
};

class Virus {
  public:
    typedef MyType id_type;
    Virus(id_type const &_id) : id(_id) {
    }
    id_type get_id() const {
      return id;
    } 
  private:
    id_type id;
};

int counter = 0;

inline bool operator< (const MyType& lhs, const MyType& rhs){ 
    if (lhs.s_=="4b") counter++;
    if (counter==) { counter++; throw VirusNotFound(); } 
    //W zależności od implementacji, trzeba dać różne numery. 
    //W naszej między 17 a 35 wyjatek wychodzi w trakcie gen.remove("1b"); i na taką sytuację przeznaczony jest test.
    else return lhs.s_<rhs.s_; }
inline bool operator> (const MyType& lhs, const MyType& rhs){ return rhs < lhs; }
inline bool operator<=(const MyType& lhs, const MyType& rhs){ return !(lhs > rhs); }
inline bool operator>=(const MyType& lhs, const MyType& rhs){ return !(lhs < rhs); }
inline bool operator==(const MyType& lhs, const MyType& rhs){ return lhs.s_==rhs.s_; }
inline bool operator!=(const MyType& lhs, const MyType& rhs){ return !(lhs == rhs); }

int main() {
  VirusGenealogy<Virus> gen("root");
  Virus::id_type const id1 = gen.get_stem_id();
  vector<Virus::id_type> parents;
  
  try {
      gen.create("1a", id1);            /*       root       */
      gen.create("1b", id1);            /*       /  \       */
      gen.create("2a", "1b");           /*      1a  1b      */
      gen.create("2b", "1b");           /*      |   / \     */ 
                                        /*      |  2a 2b    */
      parents.push_back("2a");          /*      |   \ /     */
      parents.push_back("2b");          /*      |    3      */   
      gen.create("3", parents);         /*      |   / \     */
      gen.create("4a", "3");            /*      |  4a 4b    */
      gen.create("4b", "3");            /*      |   \ /     */
      gen.create("5", "4a");            /*      |    5      */
      gen.connect("5", "4b");           /*       \   /      */
      gen.create("6", "5");             /*        \ /       */
      gen.connect("6", "1a");           /*         6        */
      gen.create("7a", "6");            /*        / \       */
      gen.create("7b", "6");            /*       7a 7b      */
      gen.get_children("3");            
      parents = gen.get_parents("3");             
      
      gen.remove("1b");
      assert(!gen.exists("2a"));
      assert(!gen.exists("2b"));
      assert(!gen.exists("3"));
      assert(!gen.exists("4a")); 
      assert(!gen.exists("4b"));
      assert(!gen.exists("5"));
      assert(gen.exists("6"));
      assert(gen.exists("7a"));
      assert(gen.exists("7b"));
      cout << "o";
      gen.remove("1a");
      assert(!gen.exists("6"));
      assert(!gen.exists("7a"));
      assert(!gen.exists("7b"));
      cout << "k1\n";      
      
  } catch (...) {
      assert(gen.exists("2a"));
      assert(gen.exists("2b"));
      assert(gen.exists("3"));
      assert(gen.exists("4a"));
      assert(gen.exists("4b"));
      assert(gen.exists("5"));
      assert(gen.exists("6"));
      assert(gen.exists("7a"));
      assert(gen.exists("7b"));
      assert(parents == gen.get_parents("3"));
      gen.remove("1b");
      assert(!gen.exists("2a"));
      assert(!gen.exists("2b"));
      assert(!gen.exists("3"));
      assert(!gen.exists("4a"));
      assert(!gen.exists("4b"));
      assert(!gen.exists("5"));
      assert(gen.exists("6"));
      assert(gen.exists("7a"));
      assert(gen.exists("7b"));
      cout << "o";
      gen.remove("1a");
      assert(!gen.exists("6"));
      assert(!gen.exists("7a"));
      assert(!gen.exists("7b"));
      cout << "k2\n";
  }
  return 0;
}
