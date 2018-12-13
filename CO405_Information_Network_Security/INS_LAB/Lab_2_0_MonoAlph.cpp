#include <iostream>
#include <cstring>

int enc_array[] = {9, 5, 25, 11, 8, 16, 19, 12, 6, 10, 18, 15, 20, 14, 7, 2, 4, 21, 23, 17, 3, 22, 24, 0, 1, 13};
int dec_array[26];

std::string Encrypt(std::string str, int n)
{
	std::string ans = "";
	for (int i = 0; i < n; i++)
	{
		ans += char('a' + enc_array[str[i] - 'a']);
	}
	return ans;
}

std::string Decrypt(std::string str, int n)
{
	std::string ans = "";
	for (int i = 0; i < n; i++)
	{
		ans += char('a' + dec_array[str[i] - 'a']);
	}
	return ans;
}

int main()
{
	for (int i = 0; i < 26; ++i)
	{
		dec_array[enc_array[i]] = i;
	}
	std::string input, enc, dec;
    std::cout << "Enter the string : ";
    std::getline(std::cin, input);
    enc = Encrypt(input, input.length());
    std::cout << "Encoded string : " << enc << std::endl;
    dec = Decrypt(enc, enc.length());
    std::cout << "Decoded string : " << dec << std::endl;
    for (int i = 0; i < 26; ++i)
	{
		std::cout << enc_array[i] << ' ' ;
	}
	std::cout << std::endl;
	for (int i = 0; i < 26; ++i)
	{
		std::cout << dec_array[i] << ' ' ;
	}
	std::cout << std::endl;
    return 0;
}
