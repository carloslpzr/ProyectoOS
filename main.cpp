#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <set>
#include <algorithm>
#include <math.h>

using namespace std;

vector<int> memPrincipal(2048, -1);
vector<int> swapping(4096, -1);
vector<int> marcos(128, -1);
deque<int> fifo;
set<int> listaProcesos;
double tiempo;


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
    cout << linea << endl;
    vector<string> instruccion = split(linea);
    int proceso;

    if(instruccion.size() != 2)
    {
        cout << "El numero de argumentos debe de ser 2." << endl;
        return;
    }

    try
    {
        proceso = stoi(instruccion[1]);
    }
    catch(...)
    {
        cout << "El argumento 2 debe de ser un numero entero" << endl;
        return;
    }
    /*
    if(listaProcesos.find(proceso) != listaProcesos.end())
    {
        cout << "Proceso " << proceso << " no existe" << endl;
        return;
    }
    */

    try
    {
        double cont = 0;
        for(int i = 0; i < swapping.size(); i++)
        {
            if(swapping[i] == proceso)
            {
                swapping[i] = -1;
                cont += 0.1 ;
            }
        }

        tiempo += cont/16.0;
        cont = 0;

        for(int i = 0; i < marcos.size(); i++)
        {
            if(marcos[i] == proceso)
            {
                marcos[i] = -1;
                cont += 0.1;
            }
        }
        tiempo += cont;

        for(int i = 0; i < memPrincipal.size(); i++)
        {
            if(memPrincipal[i] == proceso) memPrincipal[i] = -1;
        }

        deque<int>::iterator index = find(fifo.begin(), fifo.end(), proceso);
        fifo.erase(index);

    }
    catch(...)
    {
        cout << "El proceso " << proceso << " no existe." << endl;
        return;
    }

    //listaProcesos.erase(proceso);
    return;
}


void swapOut()
{
    int fifoFirst = fifo.front();
    vector <int>::iterator index = find(marcos.begin(), marcos.end(), fifoFirst);

    vector <int>::iterator indexSwap = find(swapping.begin(), swapping.end(), -1);
    swapping[indexSwap] = marcos[index];

    cout << "pagina " << index << "del proceso" << fifoFirst << " swappeada al marco " << indexSwap << " del area de swapping" << endl;
    marcos[index] = -1;

    //para checar si ya no hay marcos de pagina del primer proceso de la queue en memoria
    vector <int>::iterator ultimo = find(marcos.begin(), marcos.end(), fifo.front());
    if(ultimo == marcos.end())
        fifo.erase(fifo.begin());
}

void cargarProceso(string linea)//intenta cargar el proceso en memoria y si esta llena activa la politica de reemplazo.
{
    cout << linea << endl;
    vector<string> instruccion = split(linea);
    int nbits, proceso;

    if(instruccion.size() != 3)
    {
        cout << "El numero de argumentos debe de ser 3." << endl;
        return;
    }

    try
    {
        nbits = stoi(instruccion[1]);
        proceso = stoi(instruccion[2]);
    }
    catch(...)
    {
        cout << "Los argumentos 2 y 3 deben de ser un numero entero" << endl;
        return;
    }

    if(nbits > 2048 || nbits < 1)
    {
        cout << "los numeros de bits que puede ocupar un proceso deben de ser minimo de 1 y maximo 2048" << endl;
        return;
    }

    int iCountFreeSpace = 0;
    nPaginas = ceil(nbits/16.0); //numero de paginas que se necesitan
    for(int i = 0; i <= 127; i++)
        if(marcos[i] == -1)
            iCountFreeSpace++;

    int iCountFiller = 0;

    if(iCountFreeSpace < nPaginas) // si no hay espacio en memoria se debe swappear
    {
        for(int i = 0; i < nPaginas; i++)
        {
            swapOut();
        }
        tiempo += nPaginas;
    }

    //una vez se tiene el espacio en memoria, se carga el proceso dependiendo si esta en area de swapping o no
    vector <int>::iterator enSwapping = find(swapping.begin(), swapping.end(), proceso);
    if(enSwapping =! -1) //si el proceso ya estaba en area de swapping la carga de pagina es de 0.1
    {
        for(int i = 0; i <= 127; i++)
        {
            if(marcos[i] == -1 && iCountFiller < nPaginas)
            {
                marcos[i] = proceso;
                iCountFiller++;
                tiempo = tiempo + 0.1;
            }
        }
    }
    else //si el proceso no estaba en area de swapping previamente, la carga de pagina es de 1
    {
        for(int i = 0; i <= 127; i++)
        {
            if(marcos[i] == -1 && iCountFiller < nPaginas)
            {
                marcos[i] = proceso;
                iCountFiller++;
                tiempo++;
            }
        }
    }
    fifo.push_back(proceso); //agrega el proceso a la lista FIFO
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
                cout << "instruccion invalida" << endl;
                break;
        }

    }

    return 0;
}
