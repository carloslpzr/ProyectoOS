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
set<Proceso*> listaProcesos;
double tiempo;
int swapOut = 0, swapIn = 0;


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


void swapout()
{
    int proceso = fifo.front();
    int indiceCambiar, paginaCambiada;
    bool cambio = false;


    for(auto p:listaProcesos)
    {
        if(p->id == proceso)
        {
            for(int i = 0; i < p->residencia.size(); i++)
            {
                if(p->residencia[i] == 1 && !cambio)
                {
                    cambio = true;
                    p->residencia[i] = 0;
                    paginaCambiada = i;
                    indiceCambiar = p->marcoPagina[i];
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
            if(p->id == proceso)
            {
                for(int i = 0; i < p->residencia.size(); i++)
                {
                    if(p->residencia[i] == 1 && !cambio)
                    {
                        cambio = true;
                        p->residencia[i] = 0;
                        paginaCambiada = i;
                        indiceCambiar = p->marcoPagina[i];
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

    int marcoSwapping;
    for(int i = 0; i < swapping.size(); i += 16)
    {
        if(swapping[i] == -1 && total > 0)
        {
            marcoSwapping = i/16;
            while(total > 0)
            {
                int j = i;
                swapping[j++] = proceso;
                total--;
            }
        }
    }

    cout << "pagina " << paginaCambiada << " del proceso " << proceso << " swappeada al marco " << marcoSwapping << " del area de swapping" << endl;
    tiempo++;
    swapOut++;

}

void accesar(string linea)//intenta accesar al proceso en memoria y si no lo encuentra activa politica de swaping
{
    cout << linea << endl;
    vector<string> instruccion = split(linea);
    if(instruccion.size() != 4){
        cout << "El numero de argumentos debe de ser 4." << endl;
        return;
    }
    //declaracion de variables
    bool proceso_existe(false), should_swapin(false); //el proceso esta em memoiria o swapping? deberia hacerse swapping?
    int pagina_especifica = stoi(instruccion[1])/16;
    int id_del_proceso_a_accesar = stoi(instruccion[2]);
    Proceso *proceso_a_accesar;

    /*Pregunta si la pagina especifica se encuentra en memoria o en swapping
    * Si la pagina esta en memoria, nomas accesa
    * si la pagina no esta en memoria pero si en el swapping entonces manda a llamar swapin
    */
    for(auto proceso : listaProcesos){
        if(proceso->id==id_del_proceso_a_accesar){//buscar la referencia
            proceso_a_accesar = proceso;//obtener la referencia
            if(proceso_a_accesar->residencia[pagina_especifica]==1){//proceso existe y esta en memoria
                proceso_existe = true;
                should_swapin = false;
            }else if(proceso_a_accesar->residencia[pagina_especifica]==0){//proceso existe y no esta en memoria
                proceso_existe = true;
                should_swapin = true;
            }else if(proceso_a_accesar->residencia[pagina_especifica]==-1){//proceso no existe
                proceso_existe = false;
                should_swapin = false;
            }
        }
    }

    bool hay_espacio(false); //(si/no) hay espacio en los marcos de pagina?
    bool meter_a_fifo(false); //(si/no) deberia meterse a queue de fifo el proceso?
    if(proceso_existe==true){
        if(should_swapin==true){
                //checar si hay espacio en los marcos de pagina
                for(int n = 0; n<128; n++){
                    if(marcos[n] == -1){//marco disponible
                        hay_espacio = true;
                        //meter la pagina de nuestro proceso en ese marco de pagina
                        marcos[n] = proceso_a_accesar->id;
                        //actualizar la tabla de paginacion del proceso a accesar
                            //la pagin se encuentra en memoria
                        proceso_a_accesar->residencia[pagina_especifica] = 1;
                            //la pagina se encuentra en el marco de pagina n
                        proceso_a_accesar->marcoPagina[pagina_especifica] = n;
                            //last used = tiempo
                        proceso_a_accesar->lastUsed = tiempo;
                            //sumarle page faults
                        proceso_a_accesar->pageFaults++;
                        break;

                    }
                }
                //si no hay espacio sacar una pagina de memoria y meter la nuestra en el espacio disponible
                if(hay_espacio == false){
                    //swapout una pagina de memoria
                    swapout();
                    swapIn++;
                    //verificar si el 'proceso a accesar' deberia meterse a fifo nuevamente
                    for(int n = 0; n<128; n++){
                        if(marcos[n]==proceso_a_accesar->id){//si encontro una pagina previa
                            meter_a_fifo=false;//no deberia meterse a fifo
                        }
                    }
                    //mete la pagina a memoria
                    for(int n = 0; n<128; n++){
                        if(marcos[n] == -1){//marco disponible
                            //meter la pagina del 'proceso a accesar' en ese marco de pagina
                            marcos[n] = proceso_a_accesar->id;
                            //actualizar la tabla de paginacion del 'proceso a accesar'
                                //la pagin se encuentra en memoria = true
                            proceso_a_accesar->residencia[pagina_especifica] = 1;
                                //la pagina se encuentra en el marco de pagina n
                            proceso_a_accesar->marcoPagina[pagina_especifica] = n;
                                //last used = tiempo
                            proceso_a_accesar->lastUsed = tiempo;
                                //sumarle page faults
                            proceso_a_accesar->pageFaults++;

                        }
                    }
                    tiempo += 1;
                    //mete a fifo
                    if(meter_a_fifo==true){
                        fifo.push_back(proceso_a_accesar->id);
                    }

                    if(stoi(instruccion[3])==0){
                        cout<<"Direccion virtual: "<<instruccion[1]<<". Direccion real: "
                        <<(proceso_a_accesar->marcoPagina[pagina_especifica] * 16) + (stoi(instruccion[1])%16)<<endl;
                    } else
                    {
                        cout<<"Pagina "<< pagina_especifica <<" del proceso " << id_del_proceso_a_accesar << " modificada." << endl;
                        cout<<"Direccion virtual: "<<instruccion[1]<<". Direccion real: "
                        <<(proceso_a_accesar->marcoPagina[pagina_especifica] * 16) + (stoi(instruccion[1])%16)<<endl;
                    }
                }


        }else{
            //accesar
            if(stoi(instruccion[3])==0){
                cout<<"Direccion virtual: "<<instruccion[1]<<". Direccion real: "
                <<(proceso_a_accesar->marcoPagina[pagina_especifica] * 16) + (stoi(instruccion[1])%16)<<endl;
            } else
            {
                cout<<"Pagina "<< pagina_especifica <<" del proceso " << id_del_proceso_a_accesar << " modificada." << endl;
                cout<<"Direccion virtual: "<<instruccion[1]<<". Direccion real: "
                <<(proceso_a_accesar->marcoPagina[pagina_especifica] * 16) + (stoi(instruccion[1])%16)<<endl;
            }
        }
    }else{
            //generar error
            cout<<"Error: El proceso que se intenta accesar no ha sido cargado"<<endl;
    }

    tiempo += 0.1;

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

        //variable usada para guardar los numeros de paginas liberadas
        set<int> marcosLiberados;

        //libera  marcos
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

        //libera memoria principal
        for(int i = 0; i < memPrincipal.size(); i++)
        {
            if(memPrincipal[i] == proceso) memPrincipal[i] = -1;
        }


        //actualiza el tiempo de salida del proceso y sus bits de residencia, -1 significa que ya salio de memoria.
        for(auto p:listaProcesos)
        {
            if(p->id == proceso)
            {
                p->tiempoSalida = tiempo;
                p->residencia.assign(p->residencia.size(), -1);
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
    //cuenta pagians libres
    for(int i = 0; i <= 127; i++)
        if(marcos[i] == -1)
            iCountFreeSpace++;

    while(iCountFreeSpace < nPaginas)
    {
        //saca una pagina
        swapout();
        iCountFreeSpace++;
    }

    //crea nuevo proceso

    Proceso *p = new Proceso;
    p->id = proceso;
    p->bytes = nbits;
    p->tiempoEntrada = tiempo;
    p->lastUsed = tiempo;

    set<int> marcosCambiados;

    //carga a marcos y llena la info necesaria
    for(int i = 0; i < marcos.size(); i++)
    {
        if(marcos[i] == -1 && nPaginas > 0)
        {
            marcos[i] = proceso;
            tiempo++;
            nPaginas--;
            p->residencia.push_back(1); //cambia bit de residencia
            p->marcoPagina.push_back(i); // guarda marco de pagina donde esta ubicado
            marcosCambiados.insert(i);
        }
    }


    //carga a memoria principal el proceso
    for(int i = 0; i < memPrincipal.size(); i += 16)
    {
        if(memPrincipal[i] == -1);
        {
            for(int j = i; j < i+16; j++)
            {
                if(nbits > 0)
                {
                    memPrincipal[j] = proceso;
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

    //fifo
    fifo.push_back(p->id);

    //guarda en la lista de procesos
    listaProcesos.insert(p);
}

void fin(string linea)//termina el paquete de pedido
{
    cout << linea << endl;
    int trnTotal = 0;
    for(Proceso* p : listaProcesos){
        trnTotal += p->tiempoSalida - p->tiempoEntrada;
        cout << "Proceso: " << p->id << endl;
        cout << "Turnaround: " << p->tiempoSalida-p->tiempoEntrada << endl;
        cout << "Page faults: " << p->pageFaults << endl;
        cout << "=====";
    }
    cout << "Turnaround promedio: " << trnTotal/listaProcesos.size() << endl;
    cout << "Swapouts: " << swapOut << endl;
    cout << "Swapins: " << swapIn << endl;
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
                cout << endl;
                break;
            case 'C':
                comentario(linea);
                cout << endl;
                break;
            case 'E':
                exit(linea);
                cout << endl;
                break;
            case 'F':
                fin(linea);
                cout << endl;
                break;
            case 'L':
                liberar(linea);
                cout << endl;
                break;
            case 'P':
                cargarProceso(linea);
                cout << endl;
                break;
            default:
                cout << "instruccion invalida" << endl;
                break;
        }

    }

    return 0;
}
