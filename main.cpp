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

void swapFifo()
{
    int proceso = fifo.front(), total = 0;
    fifo.pop_front();

    for(int i = 0; i < marcos.size; i++)
    {
        if(marcos[i] == proceso)
        {
            marcos[i] = -1;
            tiempo += 1;
        }
    }

    for(int i = 0; i < memPrincipal.size(); i++)
    {
        if(memPrincipal[i] == proceso)
        {
            memPrincipal[i] = -1;
            total++;
        }
    }

    for(int i = 0; i < swapping.size(); i += 16)
    {
        if(swapping[i] == -1)
        {
            for(int j = i; j < i+16; j++)
            {
                if(total > 0)
                {
                    swapping[j] = proceso;
                    total--;
                }
            }
        }
    }

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

void liberar(string linea, bool bSwap = false)//libera el espacio de memoria
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
        //cont: variable para sumar la cantidad de tiempo que se le debe de agregar al tiempo global
        double cont = 0;
        for(int i = 0; i < swapping.size(); i++)
        {
            if(swapping[i] == proceso)
            {
                swapping[i] = -1;
                cont += 0.1 ;
            }
        }

        //se divide entre 16 ya que el tiempo de liberacion es de 0.1 por pagina, osea 16 bytes.
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


        //remueve el proceso de la lista de procesos FIFO para que no haya error
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
