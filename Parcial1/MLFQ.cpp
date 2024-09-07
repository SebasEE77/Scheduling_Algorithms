#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <queue>
#include <utility>
using namespace std;
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

vector<proceso> finalizados;    // Procesos que han finalizado
vector<proceso> procesos;       // Procesos a trabajar
vector<int> ejecutados;         // Procesos ejecutados


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
int FCFS(int& tiempo) {
    sort(procesos.begin(), procesos.end(), comparacionAt);
    int TAM = procesos.size();
    //finalizados = procesos;
    int tiempoAux;
    for (int i = 0; i < TAM; ++i) {
        if (procesos[i].tiempoRestante > 0){
            ejecutados.push_back(procesos[i].pid);
            if (tiempo < procesos[i].AT) {
                tiempo = procesos[i].AT;
            }
            else if (procesos[i].tiempoInicio){
                procesos[i].RT = tiempo;
                procesos[i].tiempoInicio = false;
            }
            tiempoAux = tiempo;
            tiempo += procesos[i].tiempoRestante;
            procesos[i].CT = tiempo;
            procesos[i].TAT = procesos[i].CT - procesos[i].AT;
            procesos[i].WT = procesos[i].TAT - procesos[i].BT;
            if (tiempo - tiempoAux == procesos[i].tiempoRestante){
                finalizados.push_back(procesos[i]);
            }

        } 
    }
    return tiempo;
}

// Función para simular Round Robin
int RR(int quantum, int& tiempo) {
    int TAM = procesos.size();
    for (int i = 0; i < TAM; ++i) {
        if (tiempo >= procesos[i].AT){
            if (procesos[i].tiempoInicio){
                procesos[i].RT = tiempo;
                procesos[i].tiempoInicio = false;
            }

            if (procesos[i].tiempoRestante > 0){
                ejecutados.push_back(procesos[i].pid);
                if (procesos[i].tiempoRestante > quantum) {
                    tiempo += quantum;  
                    procesos[i].tiempoRestante -= quantum;
                    procesos[i].CT = tiempo;   
                    procesos[i].TAT = procesos[i].CT - procesos[i].AT;  
                    procesos[i].WT = procesos[i].TAT - procesos[i].BT;  
                } else {
                    tiempo += procesos[i].tiempoRestante;
                    procesos[i].CT = tiempo;   
                    procesos[i].TAT = procesos[i].CT - procesos[i].AT;  
                    procesos[i].WT = procesos[i].TAT - procesos[i].BT;  
                    procesos[i].tiempoRestante -= quantum;
                    if (procesos[i].tiempoRestante <= 0){
                        finalizados.push_back(procesos[i]);
                    }
                }
            }
        }

    }
    return tiempo;
}

int main(){
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

    int maxCola;
    int tiempo = encontrarMinAT(procesos);
    int opcion = 0;
    int TAM = procesos.size();
    int quantum = 0;
    cout << "Con cuantas colas vas a trabajar: ";
    cin >> maxCola;
    for (int i = 1; i <= maxCola; ++i) {
        cout << "Cual algoritmo quieres ejecutar para la cola " << i <<endl;
        cout << "1. FCFS" << endl;
        cout << "2. RR" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 1){
            tiempo = FCFS(tiempo);
        }else {
            cout << "Escribe el quantum: ";
            cin >> quantum;
            tiempo = RR(quantum,tiempo);
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

