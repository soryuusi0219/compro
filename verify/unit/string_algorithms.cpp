#include <algorithm>
#include <cassert>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include "library/string/aho_corasick.hpp"
#include "library/string/eertree.hpp"
#include "library/string/lyndon_factorization.hpp"
#include "library/string/manacher.hpp"
#include "library/string/z_algorithm.hpp"

namespace {

template<class Sequence>
void test_prefixes_and_palindromes(const Sequence& s){
  const int n = s.size();
  const auto z = soryuusi::z_algo(s);
  assert((int)z.size() == n);
  for(int i = 0; i < n; ++i){
    int length = 0;
    while(i+length < n && s[length] == s[i+length]) ++length;
    assert(z[i] == length);
  }
  const auto radii = soryuusi::Manacher(s);
  assert((int)radii.size() == std::max(0,2*n-1));
  for(int i = 0; i < (int)radii.size(); ++i){
    int left = i/2, right = (i+1)/2;
    while(left >= 0 && right < n && s[left] == s[right]){
      --left;
      ++right;
    }
    assert(radii[i] == right-left-1);
  }
}

void test_lyndon(){
  for(int n = 0; n <= 9; ++n){
    for(int mask = 0; mask < (1<<n); ++mask){
      std::string s(n,'a');
      for(int i = 0; i < n; ++i) s[i] += (mask>>i)&1;
      const auto starts = soryuusi::LyndonFactorization(s);
      assert(starts.front() == 0 && starts.back() == n);
      std::string previous;
      for(int i = 1; i < (int)starts.size(); ++i){
        const auto word = s.substr(starts[i-1],starts[i]-starts[i-1]);
        assert(!word.empty());
        if(i > 1) assert(previous >= word);
        for(int j = 1; j < (int)word.size(); ++j) assert(word < word.substr(j)+word.substr(0,j));
        previous = word;
      }
    }
  }
}

void test_aho(){
  soryuusi::AhoCorasick<> aho;
  const std::vector<std::string> patterns{"","aba","ba","a","aba","cb","ac"};
  for(const auto& pattern : patterns) aho.insert(pattern);
  const auto order = aho.lex_order();
  assert((int)order.size() == aho.size());
  aho.build();
  aho.build();
  assert(order == aho.lex_order());
  const std::string text = "abacbababaac";
  int expected = 0;
  for(int i = 1; i <= (int)text.size(); ++i)
    for(const auto& pattern : patterns)
      if(pattern.size() <= std::size_t(i) && text.compare(i-pattern.size(),pattern.size(),pattern) == 0) ++expected;
  assert(aho.query(text) == expected);
}

template<class Tree, class Sequence>
void test_eertree(const Sequence& s){
  const Tree tree(s);
  for(int v = 2; v < tree.node_size(); ++v){
    const auto [l,r] = tree.ref(v);
    assert(r-l == tree.length(v));
    assert(std::equal(s.begin()+l,s.begin()+r,s.rbegin()+s.size()-r));
    assert(tree.get_next(tree.parent(v),s[r-1]) == v);
    const auto [sl,sr] = tree.ref(tree.suffix_link(v));
    assert(sr-sl < r-l);
    assert(std::equal(s.begin()+sl,s.begin()+sr,s.begin()+r-(sr-sl)));
  }
  for(int i = 0; i < (int)s.size(); ++i){
    int l = 0;
    while(!std::equal(s.begin()+l,s.begin()+i+1,s.rbegin()+s.size()-i-1)) ++l;
    assert(tree.length(tree.maximal_suffix(i)) == i-l+1);
  }
}

}  // namespace

int main(){
  std::mt19937 gen(3380);
  for(int n = 0; n <= 65; ++n){
    std::string s(n,'a');
    std::vector<int> values(n);
    for(int i = 0; i < n; ++i){
      s[i] += gen()%3;
      values[i] = int(gen()%3)-1;
    }
    test_prefixes_and_palindromes(s);
    test_prefixes_and_palindromes(values);
    test_eertree<soryuusi::Eertree<>>(s);
    test_eertree<soryuusi::Eertree<int,0,0,true>>(values);
  }
  using Sparse = soryuusi::Eertree<int,0,0,true>;
  const int lo = std::numeric_limits<int>::min(), hi = std::numeric_limits<int>::max();
  test_eertree<Sparse>(std::vector<int>{lo,hi,hi,lo,hi,lo});
  assert(Sparse().get_next(0,lo) == -1);
  test_aho();
  test_lyndon();
}
