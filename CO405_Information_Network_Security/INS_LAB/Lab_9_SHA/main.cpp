#include "sha1.h"
#include <string>
#include <iostream>
using std::string;
using std::cout;
using std::endl;

int main(int /* argc */, const char ** /* argv */)
{
    const string input = "hello world";

    SHA1 checksum;
    checksum.update(input);
    const string hash = checksum.final();

    cout << "The SHA-1 of \"" << input << "\" is: " << hash << endl;

    return 0;
}