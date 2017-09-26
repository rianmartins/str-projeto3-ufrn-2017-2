#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>     // getpriority(int which, int who)  setpriority(int which, int who, int prio);
#include <sys/resource.h>

#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"



void carga(){
  for (int i = 0; i < 10000; ++i)
  {
    for (int i = 0; i < 1500; ++i){}
  }
}

using namespace BlackLib;
using namespace std;

int main(int argc, char * argv[]){

	ADC adc_pid1(AIN0),adc_pid2(AIN1); //variaveis pra usar o adc
	BlackGPIO pid_1(GPIO_66,output),pid_2(GPIO_67,output); // porta 66 como saída para o led 1 e porta 67 como saída para o led 2

    setpriority(PRIO_PROCESS, 0, 0); // setando prioridade do processo pai em 0

	pid_t pid1, pid2;          // definindo um identificador de processo - inteiro longo
	int state1 = 0;
	int state2 = 0;

	int PRIORIDADE_BAIXA = 19;
	int PRIORIDADE_ALTA = 5;

	pid1 = fork();
	switch(pid1){
		case -1:
			exit(1);
		case 0:
			while(1){
				if(state1 == 0){
					cout << "Acende LED1" << endl;
					pid_1.setValue(high);
					state1 = 1;
				}
				else{
					cout << "Apaga LED1" << endl;
					pid_1.setValue(low);
					state1 = 0;
				}
				carga();
			}
		break;
		default:
			pid2 = fork();
			switch(pid2){
				case -1:
					exit(1);
				case 0:
					while(1){
						if(state2 == 0){
							cout << "Acende LED2" << endl;
							pid_2.setValue(high);
							state2 = 1;
						}
						else{
							cout << "Apaga LED2" << endl;
							pid_2.setValue(low);
							state2 = 0;
						}
						carga();
					}
				break;
				default:
					while(1){

						if((1.8 * (adc_pid1.getPercentValue()/100) ) > 1){
							
							setpriority(PRIO_PROCESS, pid1, PRIORIDADE_ALTA);
						}
						else{
							setpriority(PRIO_PROCESS, pid1, PRIORIDADE_BAIXA);
						}

						if((1.8 * (adc_pid2.getPercentValue()/ 100)) > 1){
							setpriority(PRIO_PROCESS, pid2, PRIORIDADE_ALTA);
						}
						else{
							setpriority(PRIO_PROCESS, pid2, PRIORIDADE_BAIXA);
						}
						
						sleep(0.5);
					}

				break;
			}
		break;
	}

	return 0;
}

