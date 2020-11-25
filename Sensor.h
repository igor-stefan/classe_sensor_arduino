#ifndef SENSOR_H
#define SENSOR_H
#include <Arduino.h>
#include <ArduinoJson.h>

class Sensor{

private:
	String gas; //gas que o sensor mede
  	int n_samples; //numero de repetiçoes
  	int n_interval; //tempo de intervalo
  	double a_eq; //valor "a" da equação
  	double b_eq; //valor "b" da equação
  	int pino; //valor do pino
  	double RS,RO;
  	double RO_AR_LIMPO; //fator RO para ar limpo
  	double VALOR_RL; //valor de RL definido no modulo do sensor

public:

//###########################CONSTRUTORES ###########################################
	Sensor(); //construtor para sensor
	Sensor(int a, int b, double c, double d); //construtor para sensor (pino,n_samples, a_eq, b_eq)
	Sensor(int a, int b, double c, double d, double e); //construtor para sensor (pino,n_samples,n_interval, a_eq, b_eq)

//############################# SETTERS########################################

	void setRO_AR_LIMPO(double ro_ar_limpo); //seta o valor de ro
	void setGas(String g);
  	void setPino(int p);
  	void setN_samples(int a);
  	void setValor_RL(double rl);
  	void setA_eq(double a);
  	void setB_eq(double b);
  	void setN_interval(int itvl);  //setters e getters

//############################# GETTERS ###############################################

	double getA_eq();
  	double getB_eq();
  	int getPino();
  	double getRs_ro();
  	String getGas();

//######################## FUNÇÕES DO SENSOR ##########################################
	
	double fazer_leitura(int l); //faz a leitura do pino do sensor
	double calibrate_RO(); //funçao que realiza a calibração do RO do sensor
	double measure_RS(); //função que mede o valor de RS
	double resistencia_sensor(double valor_lido);
	double valor_medido(double rs_ro,double a, double b);
	double mostrarDados(double rs_ro, double eqa, double eqb);
	double take_ppm();
	void make_setup(String gas, int n_samples, double n_interval, double ro_ar_limpo);
};
#endif

void wait(int x);
double ppm2ugm3(float ppm, String gas);
double ppm2ugm3(float ppm, int mw);
void fill_json(JsonArray *data, JsonArray *jumbo, double measures[]);

