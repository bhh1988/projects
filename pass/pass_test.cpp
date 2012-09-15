#include "pass.h"
#include "useful.h"
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;
using namespace pass;

int main(void) {
    RequestPass();
    LoadPasswords();
    ViewAll();
}
