#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <queue>
#include <utility>
using namespace std;

typedef struct {
    int pid;            // ID del proceso
    int BT;             // Tiempo de ejecución
    int AT;             // Tiempo de llegada s
    int prioridadCola;  
    int tiempoRestante = 0; 
    bool tiempoInicio = true;
    int RT;             // Tiempo de respuesta
    int CT;             // Tiempo de finalización
    int WT;             // Tiempo de espera
    int TAT;            // Tsiempo de retorno
} proceso;

vector<proceso> finalizados; // Procesos que han finalizado
vector<int> ejecutados; // Procesos que han finalizado


bool comparacionAt(proceso a, proceso b) {
    return a.AT < b.AT;
}

int encontrarMinAT(vector<proceso>& procesos) {
    int minAT = 100; 
    int TAM = procesos.size();
    for (int i = 0; i < TAM; ++i) {
        if (procesos[i].AT < minAT) {
            minAT = procesos[i].AT; // Actualizar mínimo
        }
    }
    return minAT;
}
int encontrarMaxCola(vector<proceso>& procesos) {
    int maxQ = 0; 
    int TAM = procesos.size();
    for (int i = 0; i < TAM; ++i) {
        if (procesos[i].prioridadCola > maxQ) {
            maxQ = procesos[i].prioridadCola; // Actualizar maximo
        }
    }
    return maxQ;
}
pair<float, float> sumaPromedio(vector<proceso>& lista){
    float sumaWT = 0.0;
    float sumaTAT = 0.0;
    int TAM = lista.size();
    for (int i = 0; i < TAM; ++i) {
        sumaWT += lista[i].WT;
        sumaTAT += lista[i].TAT;
    }
    return make_pair(sumaWT, sumaTAT);
}

// Simulación de FCFS
int FCFS(vector<proceso>& procesos, int& tiempo) {
    sort(procesos.begin(), procesos.end(), comparacionAt);
    int TAM = procesos.size();
    
    for (int i = 0; i < TAM; ++i) {
        if (tiempo < procesos[i].AT) {
            tiempo = procesos[i].AT;
        }
        procesos[i].RT = tiempo;
        procesos[i].CT = tiempo + procesos[i].BT;
        procesos[i].WT = tiempo - procesos[i].AT;
        procesos[i].TAT = procesos[i].CT - procesos[i].AT;
        tiempo += procesos[i].BT;
        finalizados.push_back(procesos[i]);
        ejecutados.push_back(procesos[i].pid);
    }
    return tiempo;
}

// Función para simular Round Robin
int RR(queue<proceso>& q, int quantum, int& tiempo) {
    while (!q.empty()) {
        proceso p = q.front();
        q.pop();

        if (tiempo >= p.AT){
            if (p.tiempoInicio){
                p.RT = tiempo;
                p.tiempoInicio = false;
            }

            if (p.tiempoRestante > quantum) {
                tiempo += quantum;

                p.tiempoRestante -= quantum; 
                q.push(p);
            
            } else {
                tiempo += p.tiempoRestante;  // p.tiempoRestante = p.BT
                p.CT = tiempo;   
                p.TAT = p.CT - p.AT;  
                p.WT = p.TAT - p.BT;  
                finalizados.push_back(p);
            }
            ejecutados.push_back(p.pid);
        } else {
            q.push(p);
        }
    }
    return tiempo;
}

int main(){
    vector<proceso> procesos;
    ifstream archivo("samples.txt"); // Abrir archivo

    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo!" << endl;
        return 1; // Salir si no se puede abrir el archivo
    }

    proceso p;
    while (archivo >> p.pid >> p.BT >> p.AT >> p.prioridadCola >> p.tiempoRestante) {
        procesos.push_back(p); // Agregar el proceso leído al vector
    }
    archivo.close(); // Cerrar el archivo

    int maxCola = encontrarMaxCola(procesos);
    int tiempo = encontrarMinAT(procesos);
    int opcion = 0;
    int TAM = procesos.size();
    int quantum = 0;
    for (int i = 1; i <= maxCola; ++i) {
        cout << "Cual algoritmo quieres ejecutar para la cola " << i <<endl;
        cout << "1. FCFS" << endl;
        cout << "2. RR" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 1){
            vector<proceso> FCFSVector;
            for (int j = 0; j < TAM; ++j){
                if (procesos[j].prioridadCola == i){
                    FCFSVector.push_back(procesos[j]);
                }
            }
            tiempo = FCFS(FCFSVector,tiempo);
        }else {
            cout << "Escribe el quantum: ";
            cin >> quantum;
            queue<proceso> RRCola;
            for (int j = 0; j < TAM; ++j){
                if (procesos[j].prioridadCola == i){
                    RRCola.push(procesos[j]);
                }
            }
            tiempo = RR(RRCola,quantum,tiempo);
        }
    }

    int TAMfinalizados = finalizados.size();
    int TAMejecutados = ejecutados.size();
    cout << endl;
    cout << "Orden ascendentemente de los procesos ejecutados: ";
    for (int i = 0; i < TAMejecutados; ++i) {
        cout << ejecutados[i] << " ";
    }
    cout << endl;
    cout << endl;
    cout << "Informacion adicional de los procesos: " <<endl;
    for (int i = 0; i < TAMfinalizados; ++i) {
        cout << "Proceso ejecutado ID: " << finalizados[i].pid << endl;
        cout << "Tiempo de Respuesta/Inicio: " << finalizados[i].RT << endl;
        cout << "Tiempo de Completado: " << finalizados[i].CT << endl;
        printf("\n");
    }
    pair<float, float> res = sumaPromedio(finalizados);
    cout << "Ahora se muestran los promedios requeridos:" << endl;
    cout << "Promedio de WT: " << res.first/TAMfinalizados << endl;
    cout << "Promedio de TAT: " << res.second/TAMfinalizados << endl;
    
    return 0;
}

