#include <cassert>
#include <cstdio>
#include <limits>
#include <string>
#include <unistd.h>

#include "library/utility/fastio.hpp"
#include "library/utility/time.hpp"

namespace {

void test_input(){
  FILE* file = std::tmpfile();
  assert(file);
  std::string input;
  for(int offset = 0; offset < 64; ++offset){
    input.resize((input.size()+63)/64*64+offset,' ');
    input += "-170141183460469231731687303715884105728:12345678?340282366920938463463374607431768211455 ";
  }
  const std::string token(130,'a');
  input += " \t"+token+" x\n4294967295";
  assert(std::fwrite(input.data(),1,input.size(),file) == input.size());
  std::rewind(file);
  int saved = dup(STDIN_FILENO);
  assert(saved >= 0 && dup2(fileno(file),STDIN_FILENO) >= 0);
  std::clearerr(stdin);
  {
    soryuusi::Instream<64> in;
    for(int offset = 0; offset < 64; ++offset){
      __int128_t lo;
      __uint128_t hi;
      int mid;
      in >> lo >> mid >> hi;
      assert(lo == std::numeric_limits<__int128_t>::min());
      assert(mid == 12345678);
      assert(hi == std::numeric_limits<__uint128_t>::max());
    }
    std::string s;
    char c;
    unsigned int last;
    in >> s >> c >> last;
    assert(s == token && c == 'x' && last == 4294967295U);
    assert(in.nextChar() == '\0' && in.nextToken().empty());
  }
  assert(dup2(saved,STDIN_FILENO) >= 0);
  close(saved);
  std::clearerr(stdin);
  std::fclose(file);
}

void test_output(){
  FILE* file = std::tmpfile();
  assert(file);
  assert(std::fflush(stdout) == 0);
  int saved = dup(STDOUT_FILENO);
  assert(saved >= 0 && dup2(fileno(file),STDOUT_FILENO) >= 0);
  const std::string token(130,'b');
  const __uint128_t power = __uint128_t(10000000000000000000ULL)*10000000000000000000ULL;
  {
    soryuusi::Outstream<64> out;
    out << power << ' ' << power+15 << '\n';
    out << std::numeric_limits<__uint128_t>::max() << '\n';
    out << std::numeric_limits<__int128_t>::min() << '\n';
    out << std::numeric_limits<long long>::min() << ' ' << std::numeric_limits<unsigned long long>::max() << '\n';
    out.fixed19(27);
    out << ' ' << token << std::endl << "tail" << std::flush;
  }
  std::rewind(file);
  char actual[512];
  std::size_t length = std::fread(actual,1,sizeof(actual),file);
  const std::string expected = "100000000000000000000000000000000000000 100000000000000000000000000000000000015\n"
    "340282366920938463463374607431768211455\n"
    "-170141183460469231731687303715884105728\n"
    "-9223372036854775808 18446744073709551615\n"
    "0000000000000000027 " + token + "\ntail";
  assert(std::string(actual,length) == expected);
  assert(dup2(saved,STDOUT_FILENO) >= 0);
  close(saved);
  std::fclose(file);
}

}  // namespace

int main(){
  test_input();
  test_output();
  std::puts("fastio: all tests passed");
}
