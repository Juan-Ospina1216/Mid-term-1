#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

void ejecutar(vector<int>&ct, vector<int>&tiempo_ex, int total, vector<int>&inicio);
int en_ejecucion(int cant, vector<int> tiempo_ex, int t);
void rr(int*t, int*ord, vector<int>&ct, vector<int>&tiempo_ex, int total, vector<int>&inicio, int*completados);

int quantum = 3;
vector<int> ats;  // Tiempos de llegada
vector<int> bts;  // tiempos de ejecucion
vector<string> ids;  
vector<int> Qs;  // numero de cola
vector<int> orden;  // orden en que se ejecutan

int main(){
	
	string id;
	int at, bt, Q;
	vector<int> tiempo_ex;  // cuenta progresivamente los tiempos que se ha ejecutado cada proceso
	vector<int> ct;  // tiempos de completado
	vector<int> inicio;  // tiempos en que se ejecutan por primera vez (RT)
	int i = 0;
	
	string line;
	getline(cin, line);
	while(!line.empty()){
		istringstream iss(line);
		
		if(iss >> id >> at >> bt >> Q){
			ats.push_back(at);
			bts.push_back(bt);
			ids.push_back(id);
			Qs.push_back(Q);
			tiempo_ex.push_back(0);
			ct.push_back(0);
			orden.push_back(0);
			inicio.push_back(-1);
			i++;
		}
		getline(cin, line);
	}
	
	ejecutar(ct, tiempo_ex, i, inicio);

	float wt_prom = 0;
	float tat_prom = 0;
	for(int j = 0; j < i; j++){  
		int indice = orden[j];
		cout << ids[indice] << " " << inicio[indice] << " " << ct[indice] << endl;
		wt_prom += (ct[indice] - ats[indice] - bts[indice]);
		tat_prom += (ct[indice] - ats[indice]);  
	}
	wt_prom = wt_prom / i;
	tat_prom = tat_prom / i;
	cout << "WT promedio: " << wt_prom << ". TAT promedio: " << tat_prom << endl;
	
	return 0;	
}

void ejecutar(vector<int>&ct, vector<int>&tiempo_ex, int total, vector<int>&inicio){
	int completados = 0;
	int t = 0;
	int ejec;
	int tiempo;
	int ord = 0;
	while(completados != total){  //se ejecuta mientras no se hayan completado todos los procesos
	
		ejec = en_ejecucion(total, tiempo_ex, t);  
		
		if(Qs[ejec] == 1){  //round robin
			
			rr(&t, &ord, ct, tiempo_ex, total, inicio, &completados);
			
		}
		else{  //fcfs
			if((inicio[ejec]) == -1){
				inicio[ejec] = t;
				
				orden[ord] = ejec;
				ord ++;
			}
			tiempo_ex[ejec]++;
			if(tiempo_ex[ejec] == bts[ejec]){
				ct[ejec] = t + 1;
				completados++;
			}
			t++;
		}
	}
}

int en_ejecucion(int cant, vector<int> tiempo_ex, int t){ 

	/*devuelve el indice del proceso que se deberia ejecutar en el tiempo t*/

	int indice = -1;
	int menor = 10000000;
	for(int i = 0; i < cant; i++){  //busca si el proceso deberia ser de la cola de RR
		if(bts[i] != tiempo_ex[i]){
			
			if(ats[i] <= menor && Qs[i] == 1 && ats[i] <= t){
				
				menor = ats[i];
				indice = i;
			}
				
		}
	}
	if(indice == -1){ //si no hay procesos en la cola de RR
	
		for(int i = 0; i < cant; i++){  //busca el proceso en la cola de FCFS
			
			if(bts[i] != tiempo_ex[i]){
				if(ats[i] <= menor && Qs[i] == 2 && ats[i] <= t){
					menor = ats[i];
					indice = i;
				}
			}
		}
	}
	return indice;
}

void rr(int*t, int*ord, vector<int>&ct, vector<int>&tiempo_ex, int total, vector<int>&inicio, int*completados){
	
	/*Ejecuta los procesos de la cola de RR, teniendo en cuenta que se pueden terminar y seguir los de FCFS
	y posteriormente puede llegar otro procesos a RR*/
	
	vector<int> r_r;
	int tot = *t;
	map<int, bool> estan;
	int elementos = 0;
	
	for(int i=0; i<total; i++){
		
		for(int j=0; j<total; j++){
			
			if(tiempo_ex[j] != bts[j] && ats[j] <= tot && estan.find(j) == estan.end() && Qs[j] == 1){
				r_r.push_back(j);  //se agregan a la "cola" de RR los que no se hayan agregado
				elementos++;
				tot += bts[j];
				estan[j] = true;
			}
		}
	}
	
	int i = *t;
	int j = 0;
	int tiempo;
	int indice;
	while(i < tot){ 
		for(int j = 0; j < elementos; j++){  //recorre la cola de RR y los va ejecutando 
			indice = r_r[j];
			if(bts[indice] != tiempo_ex[indice]){
			
				if(inicio[indice] == -1){
					inicio[indice] = i;
					orden[*ord] = indice;
					(*ord)++; 
				}
				
				tiempo = min(bts[indice] - tiempo_ex[indice], quantum);  //tiene en cuenta el quantum o el tiempo que queda por ejecutar
				tiempo_ex[indice] += tiempo;
				
				if(tiempo_ex[indice] == bts[indice]){  //si se completo el BT del proceso
					ct[indice] = i + tiempo;
					(*completados)++;

				}
				i += tiempo;
			}
		}
	}
	
	*t = tot;
	
}
