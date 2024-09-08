#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

void ejecutar(int *t, int total, vector<int>&ct, vector<int>&tiempo_ex, vector<bool>&cola_rr);
bool revisar_cola(int *t, int total, vector<bool>&cola_rr, vector<int>&tiempo_ex);
void rr(int total, int*t, vector<int>&ct, vector<int>&tiempo_ex, vector<int>&orden, vector<bool>&cola_rr);

vector<string> ids;
vector<int> ats;
vector<int> bts;

int quantum = 3;

int main(){
	string id;
	int at, bt, Q;
	vector<int> tiempo_ex;  // cuenta progresivamente los tiempos que se ha ejecutado cada proceso
	vector<int> ct;  // tiempos de completado
	vector<bool> cola_rr;  // false: no se ha ejecutado en la cola 1. true: ya se ejecuto
	int i = 0;
	int t = 1000000;
	string line;
	getline(cin, line);
	while(!line.empty()){
		istringstream iss(line);
		
		if(iss >> id >> at >> bt){
			ats.push_back(at);
			bts.push_back(bt);
			ids.push_back(id);
			tiempo_ex.push_back(0);
			ct.push_back(0);
			cola_rr.push_back(false);
			if(at < t){
				t = at;
			}
			i++;
		}
		getline(cin, line);
	}
	
	ejecutar(&t, i, ct, tiempo_ex, cola_rr);
	
	float wt = 0;
	float tat = 0;
	
	for(int j = 0; j < i; j++){
		wt += (ct[j] - ats[j] - bts[j]);
		tat += (ct[j] - ats[j]);
	}
	
	wt = wt / i;
	tat = tat / i;
	
	cout << "WT promedio: " << wt << ". TAT promedio: " << tat;
	
	return 0;
}

void ejecutar(int *t, int total, vector<int>&ct, vector<int>&tiempo_ex, vector<bool>&cola_rr){
	
	/*Funcion que ejecuta los procesos*/
	
	vector<int> orden;
	rr(total, &*t, ct, tiempo_ex, orden, cola_rr);  //ejecuta los procesos en RR por primera vez
	int ind;
	int tiempo = 0;
	int i = 0;
	while(i < total){
		ind = orden[i];
		
		if(bts[ind] != tiempo_ex[ind]){  //si el proceso no ha completado su BT lo ejecuta
			tiempo = *t;
			while(bts[ind] != tiempo_ex[ind]){  //termina de ejecutar los procesos por orden de llegada (FCFS)
				if(revisar_cola(&*t, total, cola_rr, tiempo_ex) == false){
					(*t)++;  //aumenta el tiempo de 1 en 1 para revisar si llega un proceso a la cola 1 en algun momento
					tiempo_ex[ind]++;
					
				}else{
					
					cout << ids[ind] << " " << tiempo <<  " " << *t << endl;  //termina de ejecutar el proceso para atender al que llego a la cola 1
					rr(total, &*t, ct, tiempo_ex, orden, cola_rr);  //ejecuta el proceso de la cola 1
					tiempo = *t;
				}
			}
			cout << ids[ind] << " " << tiempo << " " << *t << endl;
			if(bts[ind] == tiempo_ex[ind]){
				ct[ind] = *t;
			}
				
		}else{i++;}
	}
	
}

bool revisar_cola(int *t, int total, vector<bool>&cola_rr, vector<int>&tiempo_ex){
	
	/*Revisa si en un tiempo t llego un proceso a la cola 1 y tiene que expropiar el proceso de la cola 2*/
	
	bool ans = false;
	for(int i = 0; i < total; i++){
		if(ats[i] == *t && cola_rr[i] == false && tiempo_ex[i] != bts[i]){
			ans = true;
		}
	}
	
	return ans;
}

void rr(int total, int*t, vector<int>&ct, vector<int>&tiempo_ex, vector<int>&orden, vector<bool>&cola_rr){
	
	/*Ordena los procesos y los ejecuta en RR. El orden queda establecido para los que no se ejecuten por completo en la cola 1*/
	
	map<int, bool> estan;
	int tot = *t;
	for(int i=0; i<total; i++){  //Ordena los procesos segun su orden de llegada para ejecutarse
		int menor = 1000000;
		int indice = -1;
		for(int j=0; j < total; j++){
			if(ats[j] < menor && estan.find(j) == estan.end() && ats[j] <= tot && cola_rr[j] == false && tiempo_ex[j] != bts[j]){
				menor = ats[j];
				indice = j;
			}
		}
		if(indice != -1){
			estan[indice] = true;
			cola_rr[indice] = true;
			
			tot += quantum;
			orden.push_back(indice);
		}
	}
	int ind;
	int tiempo;
	for(int i = 0; i < orden.size(); i++){  //ejecuta los procesos en la primera cola (RR)
		ind = orden[i];
		if(estan.find(ind) != estan.end()){
			tiempo = min(bts[ind] - tiempo_ex[ind], quantum);  //tiempo que se ejecutara el proceso
			tiempo_ex[ind] += tiempo;
			if(tiempo_ex[ind] == bts[ind]){
				ct[ind] = tiempo + *t;
				
			}
			cout << ids[ind] << " " << *t << " " << *t + tiempo  << endl;
			(*t) += tiempo;
		}
	}
}
