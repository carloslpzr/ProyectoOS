#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

vector<int> memPrincipal(2048);
vector<int> swapping(4096);
vector<int> marcos(128);

void accesar(string linea)
{

}

void comenzar(string linea)
{

}

void exit(string linea)
{

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
