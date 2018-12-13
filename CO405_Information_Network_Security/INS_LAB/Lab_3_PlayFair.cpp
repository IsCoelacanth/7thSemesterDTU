#include <bits/stdc++.h>

using namespace std;

class Playfair
{
	string key;
	int find_in_key(char a)
	{
		for(int i=0 ; i<25 ; i++)
		{
			if(a == key[i])
				return i;
		}
		return -1;
	}
	pair<char, char> encipher_pair(char a, char b)
	{
		int a_id = find_in_key(a), b_id = find_in_key(b);
		int x_a=a_id/5, y_a=a_id%5, x_b=b_id/5, y_b=b_id%5;
		
		if(x_a == x_b)
			return make_pair(key[x_a*5 + (y_a+1)%5], key[x_b*5 + (y_b+1)%5]);
		if(y_a == y_b)
			return make_pair(key[((x_a+1)%5)*5 + y_a], key[((x_b+1)%5)*5 + y_b]);
		else
			return make_pair(key[x_a*5 + y_b], key[x_b*5 + y_a]);
	}
	pair<char, char> decipher_pair(char a, char b)
	{
		int a_id = find_in_key(a), b_id = find_in_key(b);
		int x_a=a_id/5, y_a=a_id%5, x_b=b_id/5, y_b=b_id%5;
		
		if(x_a == x_b)
			return make_pair(key[x_a*5 + (y_a-1)%5], key[x_b*5 + (y_b-1)%5]);
		if(y_a == y_b)
			return make_pair(key[((x_a-1)%5)*5 + y_a], key[((x_b-1)%5)*5 + y_b]);
		else
			return make_pair(key[x_a*5 + y_b], key[x_b*5 + y_a]);
	}
public:
	Playfair(string KEY)
	{
		key = KEY;
	}
	string encipher(string plainText)
	{
		if(plainText.length()%2 == 1)
			plainText += "x";
		string cipherText = plainText;
		for(int i=0 ; i<plainText.length() ; i+=2)
		{
			pair<char, char> t = encipher_pair(plainText[i], plainText[i+1]);
			cipherText[i] = t.first;
			cipherText[i+1] = t.second;
		}
		return cipherText;
	}
	string decipher(string cipherText)
	{
		if(cipherText.length()%2 == 1)
			cipherText += "x";
		string plainText = cipherText;
		for(int i=0 ; i<cipherText.length() ; i+=2)
		{
			pair<char, char> t = decipher_pair(cipherText[i], cipherText[i+1]);
			plainText[i] = t.first;
			plainText[i+1] = t.second;
		}
		return plainText;
	}
};

int main()
{
	string key = "alonpzmihxvyrswukdfteqgcb";
	Playfair p(key);
	string plainText;
	cout << "Enter the plainText : ";
	cin >> plainText;
	cout << "cipherText : " << p.encipher(plainText) << endl;
	cout << "Deciphered plainText : " << p.decipher(p.encipher(plainText)) << endl;
}