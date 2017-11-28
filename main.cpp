#include <iostream>
#include <fstream>
#include <vector>
#include <queue>


using namespace std;

vector<int> memPrincipal(2048);
vector<int> swapping(4096);
vector<int> marcos(128);
queue<int> fifo;
priority_queue<int> lru;
int tiempo;

void accesar(string linea)
{

}

void comenzar(string linea)
{

}

void exit(string linea)
{
    cout << linea << endl;
    cout << "Hasta luego!" << endl;
}

void fin(string linea)
{

}

void liberar(string linea)
{

}

void cargarProceso(string linea)
{

}


int main()
{
    tiempo = 0;
    char comando;
    string archivo, linea;
    cin >> archivo;

    fstream instrucciones;
    instrucciones.open(archivo);
    while(instrucciones.good())
    {
        getline (instrucciones, linea);
        comando = linea[0];
        switch(comando)
        {
            case 'A':
                accesar(linea);
                break;
            case 'C':
                comenzar(linea);
                break;
            case 'E':
                exit(linea);
                break;
            case 'F':
                fin(linea);
                break;
            case 'L':
                liberar(linea);
                break;
            case 'P':
                cargarProceso(linea);
                break;
            default:
                break;
        }

    }

    return 0;
}
