// https://github.com/soryuusi0219/compro/blob/main/library/math/permutation_subgroup.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

#include "../data_structure/dsu.hpp"

namespace soryuusi {

struct permutation_subgroup {
  private:
    struct Level {
      std::vector<int> positions, orbit, inverse, generators, scratch;
      std::vector<unsigned char> representative_sign, generator_sign;
      bool has_odd_generator = false;
    };
    int n;
    // The subgroups on these prefixes contain the whole symmetric or alternating group.
    int symmetric_prefix = 0, alternating_prefix = 1;
    std::vector<Level> levels;
    std::vector<std::vector<int>> pending;

    bool certifies_symmetric() const {
      if(pending.size() < 2 || n < 3) return false;
      // An odd common multiple of the odd cycle lengths isolates the sole 2-cycle.
      std::vector<unsigned char> seen(n);
      int first = -1, second = -1;
      for(const auto& p : pending){
        std::fill(seen.begin(),seen.end(),0);
        first = -1;
        for(int i = 0; i < n; ++i){
          if(seen[i]) continue;
          int length = 0, j = i;
          do {
            seen[j] = 1;
            j = p[j];
            ++length;
          }while(j != i);
          if(length & 1) continue;
          if(length != 2 || first >= 0){
            first = -1;
            break;
          }
          first = i;
          second = p[i];
        }
        if(first >= 0) break;
      }
      if(first < 0) return false;

      // Conjugating a transposition maps both endpoints through the generator.
      // Only edges merging two components are needed to certify connectivity.
      DSU connected(n);
      std::vector<std::pair<int,int>> edges;
      edges.reserve(n-1);
      connected.merge(first,second);
      edges.emplace_back(first,second);
      for(std::size_t i = 0; i < edges.size(); ++i){
        auto [u,v] = edges[i];
        for(const auto& p : pending){
          int a = p[u], b = p[v];
          if(connected.same(a,b)) continue;
          connected.merge(a,b);
          edges.emplace_back(a,b);
          if(static_cast<int>(edges.size()) == n-1) return true;
        }
      }
      return false;
    }

    bool contains(const int* p, int k, int* scratch) const {
      for(; k > symmetric_prefix; --k){
        int image = p[k];
        if(image == k) continue;
        const auto& row = levels[k];
        if(row.positions.empty() || row.positions[image] < 0) return false;
        const int* q = row.inverse.data()+std::size_t(row.positions[image])*(k+1);
        for(int i = 0; i < k; ++i) scratch[i] = q[p[i]];
        p = scratch;
      }
      return true;
    }

    bool odd_sign(const int* p, int* seen) const {
      std::fill_n(seen,n,0);
      bool odd = false;
      for(int i = 0; i < n; ++i){
        if(seen[i]) continue;
        seen[i] = 1;
        for(int j = p[i]; j != i; j = p[j]){
          seen[j] = 1;
          odd = !odd;
        }
      }
      return odd;
    }

    // Knuth's A_k: add a generator and process the previously untested pairs.
    void extend(int k, const int* generator, bool odd){
      auto& row = levels[k];
      const int length = k+1;
      if(row.orbit.empty()){
        row.positions.assign(length,-1);
        row.positions[k] = 0;
        row.orbit.push_back(k);
        row.representative_sign.push_back(0);
        row.inverse.resize(length);
        for(int i = 0; i < length; ++i) row.inverse[i] = i;
      }
      const int old_orbit = row.orbit.size(), old_generators = row.generators.size()/length;
      row.generators.insert(row.generators.end(),generator,generator+length);
      row.generator_sign.push_back(odd);
      row.has_odd_generator |= odd;
      row.scratch.resize(2*std::size_t(length));
      int* residual = row.scratch.data();
      int* scratch = residual+length;
      auto process = [&](int s, int t, bool known_new){
        // Source and destination are stored as inverse maps.
        const int* g = row.generators.data()+std::size_t(t)*length;
        int point = row.orbit[s], image = g[point];
        int position = row.positions[image];
        bool product_sign = row.representative_sign[s]^row.generator_sign[t];
        // B_k uses the inverse of the representative of the new image.
        if(position < 0){
          const std::size_t offset = row.inverse.size();
          row.inverse.resize(offset+length);
          const int* source = row.inverse.data()+std::size_t(s)*length;
          int* destination = row.inverse.data()+offset;
          for(int i = 0; i < length; ++i) destination[g[i]] = source[i];
          row.positions[image] = row.orbit.size();
          row.orbit.push_back(image);
          row.representative_sign.push_back(product_sign);
          while(symmetric_prefix+1 < n && levels[symmetric_prefix+1].orbit.size() == static_cast<std::size_t>(symmetric_prefix+2)) ++symmetric_prefix;
          alternating_prefix = std::max(alternating_prefix,symmetric_prefix);
          while(alternating_prefix+1 < n){
            const auto& next = levels[alternating_prefix+1];
            if(next.has_odd_generator || next.orbit.size() != static_cast<std::size_t>(alternating_prefix+2)) break;
            ++alternating_prefix;
          }
        }else if(k-1 > symmetric_prefix){
          bool sign = product_sign^row.representative_sign[position];
          if(!sign && k-1 <= alternating_prefix) return;
          const int* reduced = g;
          if(s != 0 || position != 0){
            const int* source = row.inverse.data()+std::size_t(s)*length;
            const int* destination = row.inverse.data()+std::size_t(position)*length;
            for(int i = 0; i < length; ++i) residual[source[i]] = destination[g[i]];
            reduced = residual;
          }
          if(known_new || !contains(reduced,k-1,scratch)) extend(k-1,reduced,sign);
        }
      };
      // The identity representative paired with the new generator is already known to fail sifting.
      process(0,old_generators,true);
      if(k <= symmetric_prefix) return;
      for(int s = 1; s < old_orbit; ++s){
        process(s,old_generators,false);
        if(k <= symmetric_prefix) return;
      }
      for(int s = old_orbit; s < static_cast<int>(row.orbit.size()); ++s){
        for(int t = 0; t <= old_generators; ++t){
          process(s,t,false);
          if(k <= symmetric_prefix) return;
        }
      }
    }

  public:
    permutation_subgroup(int n_) : n(n_){
      assert(n >= 0);
      levels.resize(n);
    }
    permutation_subgroup(int n_, std::vector<std::vector<int>> gen) : permutation_subgroup(n_){
#ifndef NDEBUG
      for(const auto& p : gen) assert(p.size() == static_cast<std::size_t>(n));
#endif
      pending = std::move(gen);
    }

    void add(std::vector<int> p){
      assert(p.size() == static_cast<std::size_t>(n));
      for(int k = n-1; k > symmetric_prefix; --k){
        if(p[k] == k) continue;
        pending.push_back(std::move(p));
        return;
      }
    }

    void build(){
      if(pending.empty()) return;
      if(n-1 <= symmetric_prefix || certifies_symmetric()){
        symmetric_prefix = std::max(0,n-1);
        pending.clear();
        return;
      }
      std::vector<int> scratch(n);
      for(const auto& p : pending){
        if(n-1 <= symmetric_prefix) break;
        if(!contains(p.data(),n-1,scratch.data())) extend(n-1,p.data(),odd_sign(p.data(),scratch.data()));
      }
      pending.clear();
    }

    std::vector<int> indices() const {
      assert(pending.empty());
      std::vector<int> result(n);
      for(int k = 0; k < n; ++k) result[k] = k <= symmetric_prefix ? k+1 : std::max<int>(1,levels[k].orbit.size());
      return result;
    }

    bool is_in(std::vector<int> p) const {
      assert(p.size() == static_cast<std::size_t>(n) && pending.empty());
      return contains(p.data(),n-1,p.data());
    }
};

}  // namespace soryuusi
