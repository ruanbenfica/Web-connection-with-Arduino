
// Feito por : Ruan Gabriel Benfica Manoel






#include <SoftwareSerial.h>                                     // Inclusão de biblioteca
#include <dht11.h>                                              // Inclusão de biblioteca
#define RX 10                                                   // Define RX como o pino 10
#define TX 11                                                   // Define o TX como pino 11
#define rele 12                                                 // Define o pino de acionamento e leitura de estado do rele
#define dht_apin 13                                             // Pino sensor DHT11
dht11 dhtObject;                                                // Criação do objeto para DHT11
String AP = "NOME_DA_SUA_REDE_WIFI";                            // Rede wi-fi
String PASS = "SENHA_DA_SUA_REDE_WIFI";                         // Senha wi-fi
String API = "SK3X9S8RJFCTBV1R";                                // API ThingSpeak
String HOST = "api.thingspeak.com";                             // Endereço
String PORT = "80";                                             // Porta de conexão
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
  
SoftwareSerial esp8266(RX,TX);                                  // Define pinos de comunicação no arduino
  
void setup() {
  pinMode(rele,OUTPUT);                                         // Define o pino do rele como saída 
  digitalWrite(rele,HIGH);                                      // Inicia o programa com o rele desativado
  Serial.begin(9600);                                           // Inicia a serial
  esp8266.begin(9600);                                          // Inicia a serial do Esp8266
  sendCommand("AT",5,"OK");                                     // Envia os comandos at para configuração do esp8266
  sendCommand("AT+CWMODE=1",5,"OK");                            // Configuração Esp8266
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");  // Da entrada no esp8266 com login e senha do wifi
}

void loop() {
  
 String getData = "GET /update?api_key="+ API +"&field1="+getTemperatureValue()+"&field2="+getHumidityValue()+"&field3="+estadoRele(); // envia os falores dos campos para o ThinkSpeak
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");   // Comandos AT
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK"); 


 String temp = getTemperatureValue(); // Recebe o valor de retorno das funções
 String umid = getHumidityValue();

 

 int tempINT= atoi(temp.c_str());  // Transforma o valor de retorno em Inteiro
 int umidINT= atoi(umid.c_str());

 Serial.print("Temperatura= "); // Imprime no serial os valores retornados das funções
 Serial.print(tempINT); 
 Serial.println("ºC");
 
 Serial.print("Umidade= ");
 Serial.print(umidINT);
 Serial.println("%");
 

 
 
 if (tempINT>31 or umidINT>70) { // Condiçoes para ligar ou desligar o rele
    delay(2000);
    digitalWrite(rele,LOW);
   
}
  else { 
  
    digitalWrite(rele,HIGH);
    
}
}


String estadoRele(){  // Função para verificar e atualizar o estado do rele
  
  int estadorele = digitalRead(rele);
  if(estadorele == LOW){
    
      estadorele=1;
    
  }else{
    
    estadorele=0;
    
  }
    
  Serial.print("Estado do rele :"); // Imprime o estado do rele no Serial
  Serial.println(estadorele);
  delay(50);
  return String (estadorele);

}

String getTemperatureValue(){    // Função de verificação da temperatura

   dhtObject.read(dht_apin);
   int temperatura = dhtObject.temperature;
   delay(50);
   return String(temperatura); 
  
}


String getHumidityValue(){   // Função de verificação da umidade

   dhtObject.read(dht_apin);
   int umidade = dhtObject.humidity;
   delay(50);
   return String(umidade); 
  
}




void sendCommand(String command, int maxTime, char readReplay[]) {  // Função para envio dos comandos AT
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true) // Comandos para confirmação da conexão
  {
    Serial.println("OK");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
