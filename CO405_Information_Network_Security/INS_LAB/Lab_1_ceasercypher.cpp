#include <iostream>
#include <cstring>

std::string encoder(std::string P, int key)
{
    std::string result = "";

    for (int i = 0; i < P.length(); i++)
    {
        if (isupper(P[i]))
            result += char(int(P[i] + key - 65) % 256 + 65);
        else
            result += char(int(P[i] + key - 97) % 256 + 97);
    }
    return result;
}

std::string decoder(std::string C, int key)
{
    std::string result = "";

    for (int i = 0; i < C.length(); i++)
    {
        if (isupper(C[i]))
            result += char(int(C[i] - key - 65) % 256 + 65);
    
            result += char(int(C[i] - key - 97) % 256 + 97);
    }
    return result;
}

int main()
{
    std::string input, encoded, decoded;
    int key;
    std::cout << "Enter the string : ";
    std::getline(std::cin, input);
    std::cout << "Enter the key : ";
    std::cin >> key;
    encoded = encoder(input, key);
    std::cout << "Encoded string : " << encoded << std::endl;
    decoded = decoder(encoded, key);
    std::cout << "Decoded string : " << decoded << std::endl;
    return 0;
}