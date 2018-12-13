// #include<iostream>
// #include<string>
// #include<utility>
// #include<cmath>
#include<bits/stdc++.h>

using namespace std;

long long int power(long long int x,  long long int y, long long int p) 
{ 
    long long int res = 1;      // Initialize result 
  
    x = x % p;  // Update x if it is more than or  
                // equal to p 
  
    while (y > 0) 
    { 
        // If y is odd, multiply x with result 
        if (y & 1) 
            res = (res*x) % p; 
  
        // y must be even now 
        y = y>>1; // y = y/2 
        x = (x*x) % p;   
    } 
    return res; 
}


long long Hash(string S, int p)
{
    long long hash = 1;
    int prev = 1;
    for (int i = 0; i < S.length(); i++)
    {
        hash = (hash + int(S[i])*prev % p);
        prev = int(S[i]);
    }
    return hash;
}

long long int modInverse(long long int a, long long int m) 
{ 
    long long int m0 = m; 
    long long int y = 0, x = 1; 
  
    if (m == 1) 
      return 0; 
  
    while (a > 1) 
    { 
        // q is quotient 
        long long int q = a / m; 
        long long int t = m; 
  
        // m is remainder now, process same as 
        // Euclid's algo 
        m = a % m, a = t; 
        t = y; 
  
        // Update y and x 
        y = x - q * y; 
        x = t; 
    } 
  
    // Make x positive 
    if (x < 0) 
       x += m0; 
  
    return x; 
}

class DSA
{
private:
    long long int x;
    long long int g;
    long long int y;
    long long int p;
    long long int q;
public:
    DSA(int , int );
    pair<long long int, long long int> sign(string);
    bool verify(string, pair<long long int, long long int>);

};

DSA::DSA(int P, int Q)
{
    x = rand() % 100 + 1;
    p = P;
    q = Q;
    g = power(2, (p-1)/q, p);
    y = power(g, x, p);
}

pair<long long int, long long int> DSA::sign(string s)
{
    long long int s1 = 0;
    long long int s2 = 0;
    long long int r;
    do 
    {
        r = rand() % q + 1;
        s1 = power(g, r, p) % q;
        long long k = modInverse(r, q);
        s2 = (k * (Hash(s, p) + x * s1)) % q;
    }while(s1 == 0 || s2 == 0);
    return make_pair(s1, s2);
}


bool DSA::verify(string s, pair<long long int, long long int> sign)
{
    long long int h = Hash(s, p);
    long long int w, u1, u2, v;
    w = modInverse(sign.second, p);
    u1 = (h * w);
    // cout << u1 << endl;
    u2 = (sign.first * w);
    // cout << u2 << endl;
    v = ((power(g, u1, p) * power(y, u2, p))%p)%q;
    // cout << v << endl;
    return (abs(v) == sign.first);
}

int main()
{
    long long int p, q;
    cout << "Enter the public primes : ";
    cin >> p >> q;
    cout << "Enter message to sign : ";
    string s;
    cin >> s;
    DSA D(p, q);
    pair<long long int, long long int> sin = D.sign(s);
    cout << "The signature is: " << sin.first << ' ' << sin.second << endl;
    if (D.verify(s, sin))
        cout << "verified" << endl;
    else
        cout <<"Rejected" <<endl;
}