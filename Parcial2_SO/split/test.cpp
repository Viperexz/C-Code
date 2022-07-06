#include <iostream>
#include <string>

#include "split.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

int main(int argc, char * argv[]) {


  int n;
  if (argc == 1) {
    cerr << "Debe especificar una cadena" << endl;
    exit(EXIT_FAILURE);
  }
  string str(argv[1]);

  char ** x;

  if (argc >= 3) {
    x = split(str.data(), argv[2], &n);
  }else {
    x = split(str.data(), " \t\r\n", &n);
  }

  cout << "Tokens: " << n << endl;
  for (int i  = 0; i < n; i++) {
    cout << x[i] << endl;
  }
}
