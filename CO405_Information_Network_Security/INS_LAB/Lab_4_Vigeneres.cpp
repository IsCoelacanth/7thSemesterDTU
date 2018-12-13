#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> Table;

void build_table()
{
    for (int i = 0; i < 26; i++)
    {
        std::string s = "";
        for (int j = 0; j < 26; j++)
        {
            s += char ((i + j) % 26 + 97);
        }
        Table.push_back(s);
    }
}

int find_index(char s, char k)
{
    for (int i = 0 ; i < 26; i++)
    {
        if (Table[int(k-'a')][i] == s)
            return i;
    }
    return -1;
}

void printTable()
{
    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 26; j++)
            std::cout << Table[i][j] << " | ";
        std::cout << std::endl;
    }
}

std::string Encrypt(std::string S, std::string key)
{
    std::string enc = "";
    std::cout << "Encrypting text" << std::endl;
    for (int i = 0; i < S.length(); i++)
    {
        enc += Table[int(key[(i)%key.length()] - 'a')][int(S[i] - 'a')];
    }
    return enc;
}

std::string Decrypt(std::string S, std::string key)
{
    std::string dec = "";
    std::cout << "Decrypting text" << std::endl;
    for (int i = 0; i < S.length(); i++)
    {
        dec += char(find_index(S[i], key[(i%key.length())]) + 97);
    }
    return dec;
}

int main()
{
    std::cout << "Building Vigenere's table" << std::endl;
    build_table();
    std::cout << "Built Vigenere's table: " << std::endl;
    printTable();
    std::cout << std::endl;
	std::string input, encoded, decoded, key;
    std::cout << "Enter the string : ";
    std::getline(std::cin, input);
    std::cout << "Enter the key : ";
    std::cin >> key;
    encoded = Encrypt(input, key);
    std::cout << "Encoded string : " << encoded << std::endl;
    decoded = Decrypt(encoded, key);
    std::cout << "Decoded string : " << decoded << std::endl;
    return 0;
}