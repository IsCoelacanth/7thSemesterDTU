#include<iostream>
#include<string>
#include<vector>
#include<cstdlib>
#include<cmath>

using namespace std;

long long int power(long long int a, long long int b, 
                                     long long int P) 
{  
    if (b == 1) 
        return a; 
  
    else
        return (((long long int)pow(a, b)) % P); 
}

class Node
{
    private:
        long long int g;
        long long int modulus;
        long long int power_element;
        long long int key_1;
        long long int key;
    public:
        Node(int p, int G)
        {
            g = G;
            modulus = p;
        }

        void select_a();
        long long int generate_key1();
        void generate_key(long long int);
        bool check_key(long long int);
        void print_power()
        {
            cout << "selected power: " << power_element << endl;
        }

        void print_key()
        {
            cout << "selected key: " << key << endl;
        }

        long long int get_key() { return key; }
};


void Node::select_a()
{
    power_element = rand() % 10; // a = [0, 100)
}

long long int Node::generate_key1()
{
    // cout << g << "^" << power_element << "\%" << modulus << endl;
    long long int A = power(g, power_element, modulus);
    // cout << A << endl;
    key_1 = A;
    return A;
}

void Node::generate_key(long long int A)
{
    long long int s = power(A, power_element, modulus);
    key = s;
}

bool Node::check_key(long long int O)
{
    if (O == key)
        return true;
    return false;
}

int main()
{
    int p, g;
    cout << "Enter the modular and g(a primitive root for modular)" << endl;
    cin >> p >> g;
    Node A(p, g);
    Node B(p, g);

    cout << "Starting key exchange" << endl;
    cout << "Node A" << endl;
    A.select_a();
    A.print_power();
    cout << "Node B" << endl;
    B.select_a();
    B.print_power();

    long long int channel1 = A.generate_key1();
    long long int channel2 = B.generate_key1();
    cout << "Node 1 is sending " << channel1 << " to Node 2" << endl;
    cout << "Node 2 is sending " << channel2 << " to Node 1" << endl;

    A.generate_key(channel2);
    B.generate_key(channel1);
    cout << "Node 1 ";
    A.print_key();
    cout << "Node 2 ";
    B.print_key();

    if (A.check_key(B.get_key()) && B.check_key(A.get_key()))
    {
        cout << "Both users have matching keys, starting secured communication" << endl;
    }
    else
    {
        cout << "Key generation failed, exiting" << endl;
    }


    return 0;
}
