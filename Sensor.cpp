#include <Sensor.h>
#include <Arduino.h>
#include <stdlib.h>
//####################################### CONSTRUTORES ###########################################

Sensor::Sensor(){ //construtor 
	n_samples = 100;
	a_eq = 1;
	b_eq = 1;
	pino = A0;
	pinMode(pino, INPUT);
}

Sensor::Sensor(int a, int b, double c, double d){ //construtor padrao
	setPino(a);
	setN_samples(b);
	setA_eq(c);
	setB_eq(d);
	pinMode(pino, INPUT);
}


Sensor::Sensor(int a, int b, double c, double d, double e){ //construtor padrao
	setPino(a);
	setN_samples(b);
	setN_interval(c);
	setA_eq(d);
	setB_eq(e);
	pinMode(pino, INPUT);
}
//##################### SETTERS ############################################
void Sensor::setGas(String g){ //seta o nome do gas que o sensor mede
  gas = g;
}
void Sensor::setN_interval(int itvl){ //seta o valor do intervalo entre as coletas de dados
	if(itvl <= 0) itvl = 100;
	n_interval = itvl;
} 
void Sensor::setN_samples(int a){ //seta o numero de vezes da coleta de dados
	if (a < 0) a = 100;	
	n_samples = a;
}
void Sensor::setA_eq(double a){ //seta o valor de A da equa��o
	a_eq = a;
 }
void Sensor::setB_eq(double b){ //seta o valor de B da equa��o
	b_eq = b;
 }
void Sensor::setPino(int p){ //seta o valor do PINO do sensor
  pino = p;
}
void Sensor::setRO_AR_LIMPO(double ro_ar_limpo){ //seta RO AR LIMPO
  RO_AR_LIMPO = ro_ar_limpo;
}
void Sensor::setValor_RL(double rl){
  VALOR_RL = rl;
}

//####################### GETTERS ###############################

double Sensor::getA_eq(){ //Pega o valor de A da equa��o 
	return a_eq;
}
double Sensor::getB_eq(){ //Pega o valor de B da equa��o
	return b_eq;
 }
int Sensor::getPino(){ //Pega o vaor do pino do sensor
	return pino;
}
String Sensor::getGas(){ //Retorna o tipo de gas que o sensor mede
	return gas;
}
double Sensor::getRs(){
	return RS;
}
double Sensor::getRo(){
	return RO;
}
double Sensor::getRs_ro(){
	return RS / RO;
}

//###################### FUNÇÕES DO SENSOR ##############################

double Sensor::fazer_leitura(int l){ //faz a leitura do pino
	return analogRead(l); //retorna o valor lido pela entrada analogica do arduino, valor de 0 a 1023.
}

double Sensor::resistencia_sensor(double valor_lido){ //recebe como parametro entrada um valor de 0 a 1023.
	double divisor_de_tensao = 0; //cria uma variavel para receber o valor do calculo do divisor de tensao.(apenas organiza��o).
	divisor_de_tensao = (double)VALOR_RL * ((1023 - valor_lido) / valor_lido); 
	return divisor_de_tensao; //Retorna Rsensor
}

double Sensor::calibrate_RO(){ 
	int i = 0, j = 0; //para contador
	double valor = 0.0, soma = 0.0; //para retornar valor de RO
	// Serial.println("Calibrando RO..."); //exibe 'calibrando...'
	while(j != n_samples){
		for(i = 0; i < n_samples; i++){
  			valor += resistencia_sensor(fazer_leitura(pino)); 
  			wait(n_interval); // a cada n_interval milisegundos
		}
		soma += (double) valor / n_samples; //tira a media dos valores encontrados
		valor = 0.0;
		j++;
	} 
	soma /= j;
	soma /= RO_AR_LIMPO; //encontra o valor de RO
	RO = soma;
	return soma; //retorna o valor de RO
}

double Sensor::measure_RS(){
	int j = 0;
	double rs = 0;
	for(j = 0; j < n_samples; j++){ //toma n_samples medidas
		rs += resistencia_sensor(fazer_leitura(pino)); //faz a leitura da resistencia do sensor n_samples vezes
		wait(n_interval); //a cada n_interval milisegundos
  }
  rs = (double) rs / n_samples; //tira a media
  RS = rs;
return rs; //retorna rs
}

double Sensor::valor_medido(double rs_ro,double a, double b){//a fun��o pow retorna uma variavel do tipo double atraves da qual encontramos y=a*x^b
  return a * (pow(rs_ro, b)); //retorna y=a*x^b do sensor. (Valor em PPM)
}

double Sensor::mostrarDados(double rs_ro,double eqa,double eqb){ //Mostra a taxa de concentra��o
	Serial.print("Taxa de ");
	Serial.print(getGas());
	Serial.print(" - ");
	Serial.print(valor_medido(rs_ro,eqa,eqb)); 
	Serial.print(" PPM"); //Apresenta algo do tipo "Taxa de (gas) - XXX PPM"
	return valor_medido(rs_ro, eqa, eqb);
}

void Sensor::make_setup(String gas, int n_samples, double n_interval, double ro_ar_limpo){
	setGas(gas);
	setN_samples(n_samples);  
  	setN_interval(n_interval);
  	setRO_AR_LIMPO(ro_ar_limpo); 
 	setValor_RL(1);
  	calibrate_RO();
}

double Sensor::take_ppm(){
	measure_RS();
	if(getGas() == "CO2"){
		return valor_medido(log10(getRs_ro()), getA_eq(), getB_eq());
	}
	else if(getGas() == "CO"){
		return valor_medido(getRs_ro(), getA_eq(), getB_eq()) / 10.0;
	}
	else if(getGas() == "O3") return valor_medido(getRs_ro(), getA_eq(), getB_eq()) / 1000.0;
	
	else return valor_medido(getRs_ro(), getA_eq(), getB_eq());
}


//############################# OUTRAS FUNÇÕES #################################################
void wait(int x){
	unsigned long tempo;
	int k = 0;
	tempo = millis();
	while(millis() - tempo <= x){
		k++; k--;
	}
}

double ppm2ugm3(float ppm, String gas){
	int mw = 0;	
	double mv = 24.46219;
	if(gas=="CO")mw = 28;
	else if(gas == "O3") mw = 48;
	else if(gas == "CO2") mw = 44;
	else if(gas == "SO2") mw = 64;
	else mw = 46;
	return (double) ppm * 1000 * (mw / mv);
}

double ppm2ugm3(float ppm, int mw){	
	double mv = 24.46219;
	return (double) ppm * 1000 * (mw / mv);
}

void fill_json(JsonArray *data, JsonArray *jumbo, double measures[]){
	int mw[5] = {28,44,48,46,64};
	for(int i = 0; i < 5; i++){
		data -> add(measures[i]);
		jumbo -> add(ppm2ugm3(measures[i], mw[i]));
	}
}
