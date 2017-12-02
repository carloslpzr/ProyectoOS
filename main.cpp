#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <set>
#include <algorithm>
#include <math.h>

using namespace std;

struct Proceso{
	int id = -1, bytes = 0, pageFaults = 0;
	vector<int> residencia, marcoPagina;
	double tiempoEntrada = 0, lastUsed = 0, tiempoSalida = 0;
};

inline bool operator<(const Proceso& p1, const Proceso& p2)
{
  return p1.id < p2.id;
}


vector<int> memPrincipal(2048, -1);
vector<int> swapping(4096, -1);
vector<int> marcos(128, -1);
deque<int> fifo;
set<Proceso> listaProcesos;
double tiempo;
int swaps;


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

void swapOut(bool FifoLRU)//true para FIFO, false para LRU
{
    //declaraciones
    int proceso(-1), total = 0;
    double  LRUtime(9999);

    //seleccion del proceso a intercambiar
    if(FifoLRU==true){
        proceso = fifo.front();
        fifo.pop_front();
    }else{
        for(auto elemento: listaProcesos){
                if(elemento.lastUsed<LRUtime){
                    proceso = elemento.id;
                    LRUtime = elemento.lastUsed;
                }
            }
    }

    //libera los marcos y actualiza el tiempo.
    for(int i = 0; i < marcos.size(); i++)
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
    cout << linea << endl;
    vector<string> instruccion = split(linea);
    if(instruccion.size() != 4){
        cout << "El numero de argumentos debe de ser 4." << endl;
        return;
    }
    bool shouldSwap = true;
    //Buscar si el proceso se encunetra en memoria.
    for(int m : marcos){
        if(m==instruccion[2]){
            shouldSwap = false;
            break;
        }
    }

    //Si el proceso no se encuentra en memoria,
    //Se hace un page fault y se mete a la memoria con swapping.
    if(shouldSwap){
        //Marcar un page fault en la lista de procesos.
        for(Proceso p : listaProcesos)
            if(p.id == instruccion[2])
                p.pageFaults++;
        //Deberia de meter el proceso que se deasea
        //accesar a memoria.
    }

    int pagCount = floor(instruccion[1]/16); //El numero de pagina de la direccion virtual del proceso
    int pagAct = 0;     //Contador para el numero de paginas que se han revisado que pertenecesn al proceso.
    int dir = -1;       //Direccion real del proceso.
    for(int i=0; i<marcos.size(); i++){
        if(marcos[i]==instruccion[2])pagAct++;
        if(pagAct==pagCount){
            int desplazamiento = 16*i;
            dir = desplazamiento+(instruccion[i]%16);
            break;
        }
    }
    if(dir!=-1){
        //La memoria se "modifico"
        if(instruccion[3]==1)count << "Pagina " << pagCount << " del proceso " << instruccion[2] << " modificada." << endl;
        count << "Direccion virtual: " << instruccion[1] << ". Direccion real: " << dir;
    }
}

void comentario(string linea)//imprime un comentario
{
    cout << linea << endl;
}

void exit(string linea)//sali del programa
{
    cout << linea << endl;
    cout << "Hasta luego!" << endl;
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

    cout << "Liberar los marcos de página ocupados por el proceso " << proceso << endl;

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

        set<int> marcosLiberados;

        for(int i = 0; i < marcos.size(); i++)
        {
            if(marcos[i] == proceso)
            {
                marcos[i] = -1;
                cont += 0.1;
                marcosLiberados.insert(i);
            }
        }
        tiempo += cont;

        for(int i = 0; i < memPrincipal.size(); i++)
        {
            if(memPrincipal[i] == proceso) memPrincipal[i] = -1;
        }


        for(auto p:listaProcesos)
        {
            if(p.id == proceso)
            {
                p.tiempoSalida = tiempo;
                p.residencia.assign(p.residencia.size(), -1);
            }
        }

        //remueve el proceso de la lista de procesos FIFO para que no haya error
        deque<int>::iterator index = find(fifo.begin(), fifo.end(), proceso);
        fifo.erase(index);

        cout << "Se liberan los marcos de página de memoria real: [";
        for(auto m: marcosLiberados)
        {
            cout << m << ",";
        }
        cout << "]" << endl;

    }
    catch(...)
    {
        cout << "El proceso " << proceso << " no existe." << endl;
        return;
    }

    return;
}

void swap2()
{
    int proceso = fifo.front();
    int indiceCambiar;
    bool cambio = false;

    for(auto p:listaProcesos)
    {
        if(p.id == proceso)
        {
            for(int i = 0; i < p.residencia.size(); i++)
            {
                if(p.residencia[i] == 1 && !cambio)
                {
                    cambio = true;
                    p.residencia[i] = 0;
                    indiceCambiar = p.marcoPagina[i];
                }
            }
        }
    }

    if(!cambio)
    {
        fifo.pop_front();
        proceso = fifo.front();
        for(auto p:listaProcesos)
        {
            if(p.id == proceso)
            {
                for(int i = 0; i < p.residencia.size(); i++)
                {
                    if(p.residencia[i] == 1 && !cambio)
                    {
                        cambio = true;
                        p.residencia[i] = 0;
                        indiceCambiar = p.marcoPagina[i];
                    }
                }
            }
        }
    }

    marcos[indiceCambiar] = -1;

    int dirReal = indiceCambiar * 16;
    int total = 0;

    for(int i = dirReal; i < dirReal + 16; i++)
    {
        if(memPrincipal[i] != -1)
        {
            memPrincipal[i] = -1;
            total++;
        }
    }

    for(int i = 0; i < swapping.size(); i += 16)
    {
        if(swapping[i] == -1)
        {
            for(int j = i; j < i + total; j++)
            {
                swapping[j] = proceso;
            }
        }
    }

    tiempo++;

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

    cout << "Asignar " << nbits << " bytes al proceso " << proceso << endl;

    int iCountFreeSpace = 0;
    int nPaginas = ceil(nbits/16.0); //numero de paginas que se necesitan
    for(int i = 0; i <= 127; i++)
        if(marcos[i] == -1)
            iCountFreeSpace++;

    while(iCountFreeSpace < nPaginas)
    {
        swap2();
        iCountFreeSpace++;
    }

    Proceso *p = new Proceso;
    p->id = proceso;
    p->bytes = nbits;
    p->tiempoEntrada = tiempo;
    p->lastUsed = tiempo;

    set<int> marcosCambiados;

    for(int i = 0; i < marcos.size(); i++)
    {
        if(marcos[i] == -1 && nPaginas > 0)
        {
            marcos[i] = proceso;
            tiempo++;
            nPaginas--;
            p->residencia.push_back(1);
            p->marcoPagina.push_back(i);
            marcosCambiados.insert(i);
        }
    }

    for(int i = 0; i < memPrincipal.size(); i += 16)
    {
        if(memPrincipal[i] == -1);
        {
            for(int j = i; j < i+16; j++)
            {
                if(nbits > 0)
                {
                    memPrincipal[j] == proceso;
                    nbits--;
                }
            }
        }
    }

    cout << "Se asignaron los marcos de pagina ";

    for(auto m:marcosCambiados)
    {
        cout << m << ", ";
    }

    cout << "al proceso " << proceso << endl;

    fifo.push_back(p->id);


    listaProcesos.insert(*p);
}

void fin(string linea)//termina el paquete de pedido
{
    cout << linea << endl;
    int trnTotal = 0;
    for(Proceso p : listaProcesos){
        trnTotal += p.tiempoSalida-p.tiempoEntrada;
        cout << "Proceso: " << p.id << endl;
        cout << "Turnaround: " << p.tiempoSalida-p.tiempoEntrada << endl;
        cout << "Page faults: " << p.pageFaults << endl;
        cout << "=====";
    }
    cout << "Turnaround promedio: " << trnTotal/listaProcesos.size() << endl;
    cout << "Swaps: " << swaps << endl;
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
