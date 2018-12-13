#include <iostream>
#include <cstring>

int enc_array[] = {9, 5, 25, 11, 8, 16, 19, 12, 6, 10, 18, 15, 20, 14, 7, 2, 4, 21, 23, 17, 3, 22, 24, 0, 1, 13};
int dec_array[26];

std::string Encrypt(std::string str, int key, int n)
{
	std::string ans = "";
	char prev;
	for (int i = 0; i < n; i++)
	{
		if (i == 0)
		{
			ans += char(int(str[i] + key - 'a') % 26 + 97);
		}
		else
			ans += char(int(str[i] + prev - 'a') % 26 + 97);
		prev = str[i] - 'a';
	}
	return ans;
}

std::string Decrypt(std::string str, int key, int n)
{
	std::string ans = "";
	char prev;
	for (int i = 0; i < n; i++)
	{
		if (i == 0)
		{
			ans += char(int(str[i] - key - 'a') % 26 + 97);
		}
		else
			ans += char(int(str[i] - prev - 'a') % 26 + 97);
		prev = ans[i] - 'a';
	}
	return ans;
}

int main()
{
	std::string input, encoded, decoded;
    int key;
    std::cout << "Enter the string : ";
    std::getline(std::cin, input);
    std::cout << "Enter the key : ";
    std::cin >> key;
    encoded = Encrypt(input, key, input.length());
    std::cout << "Encoded string : " << encoded << std::endl;
    decoded = Decrypt(encoded, key, encoded.length());
    std::cout << "Decoded string : " << decoded << std::endl;
    return 0;
}
