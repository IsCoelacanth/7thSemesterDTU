#include<iostream>
#include<cmath>
#define K 5

using namespace std;

class HillCipher
{
public:
	HillCipher() {}
	void encrypt();
	void decrypt();
	void SetData();
	void CalculateInverse();
	float enc[K][1];
	float dec[K][1];
	float A[K][K];
	float B[K][K];
	float C[K][K];
	float message[K][1];
};

void getCofactor(float A[K][K], float temp[K][K], int p, int q, int n)
{
    int i = 0, j = 0;

    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            if (row != p && col != q)
            {
                temp[i][j++] = A[row][col];
                if (j == n - 1)
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

int determinant(float A[K][K], int n)
{
    int D = 0;
    if (n == 1)
        return A[0][0];

    float temp[K][K];

    int sign = 1;

    for (int f = 0; f < n; f++)
    {
        getCofactor(A, temp, 0, f, n);
        D += sign * A[0][f] * determinant(temp, n - 1);

        sign = -sign;
    }

    return D;
}

void adjoint(float A[K][K], float adj[K][K])
{
    if (K == 1)
    {
        adj[0][0] = 1;
        return;
    }

    int sign = 1;
    float temp[K][K];

    for (int i = 0; i < K; i++)
    {
        for (int j = 0; j < K; j++)
        {
            getCofactor(A, temp, i, j, K);

            sign = ((i + j) % 2 == 0) ? 1 : -1;

            adj[j][i] = (sign)*(determinant(temp, K - 1));
        }
    }
}

bool inverse(float A[K][K], float inverse[K][K])
{
    float det = determinant(A, K);
    if (det == 0)
    {
        cout << "Singular matrix, can't find its inverse";
        return false;
    }

    float adj[K][K];
    adjoint(A, adj);

    for (int i = 0; i < K; i++)
        for (int j = 0; j < K; j++)
            inverse[i][j] = adj[i][j] / float(det);

    return true;
}

void HillCipher::encrypt()
{
	int i, k;

	for (i = 0; i < K; i++)
		for (k = 0; k < K; k++)
			enc[i][0] = enc[i][0] + A[i][k] * message[k][0];

	cout << "Encrypted string is: ";
	for (int i = 0; i < K; i++)
		cout << (char)(fmod(enc[i][0], 26) + 97);
	cout << endl;
}

void HillCipher::decrypt()
{
	int i, k;
	CalculateInverse();

	for ( i = 0; i < K; i++)
		for ( k = 0; k < K; k++)
			dec[i][0] = dec[i][0] + B[i][k] * enc[k][0];

	cout << "Decrypted string is: ";
	for (int i = 0; i < K; i++)
		cout << (char)(fmod((int)dec[i][0], 26) + 97);
	cout << endl;
}

void HillCipher::SetData()
{
	char msg[K];

	cout << "Enter the " << K << "x" << K << " matrix i.e key" << endl;
	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < K; j++)
		{
			cin >> A[i][j];
			C[i][j] = A[i][j];
		}
	}

	cout << "Enter a " << K << " len message" << endl;
	cin >> msg;
	for (int i = 0; i < K; i++)
	{
		message[i][0] = msg[i] - 97;
	}
}



void HillCipher::CalculateInverse()
{
    bool b = inverse(A, B);

	cout << "Found inverse matrix for decryption" << endl;
	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < K; j++)
		{
			cout << B[i][j] << '\t';
		}
		cout << endl;
	}
}

int main()
{
	HillCipher cipher;
	cipher.SetData();
	cipher.encrypt();
	cipher.decrypt();
	return 0;
}