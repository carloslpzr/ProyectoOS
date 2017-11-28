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

vector<string> split(const string& s)//lee un
{
	string buff{""};
	vector<string> v;

	for(auto n:s)
	{
		if(n != ' ') buff+=n; else
			if(n == ' ' && buff != "") { v.push_back(buff); buff = ""; }
	}
	if(buff != "") v.push_back(buff);

	return v;
}

void accesar(string linea)//intenta accesar al proceso en memoria y si no lo encuentra activa politica de swaping
{

}

void comentario(string linea)//imprime un comentario
{

}

void exit(string linea)//sali del programa
{
    cout << linea << endl;
    cout << "Hasta luego!" << endl;
}

void fin(string linea)//termina el paquete de pedido
{

}

void liberar(string linea)//libera el espacio de memoria
{

}

void cargarProceso(string linea)//intenta cargar el proceso en memoria y si esta llena activa la politica de reemplazo.
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
                comentario(linea);
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
