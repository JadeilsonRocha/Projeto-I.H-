#include "fsm_config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>;

#define DHTPIN D3     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "teste";
const char* password = "adailson007";
const char* mqtt_server = "things.ubidots.com";
float hum;
float temp;
const int ledPin = D7;
const int buttonPin = D5;
int estadoBotao = 0;
int estadoLed = 0;
long tempoDaUltimaMensagem = 0;
char msg[50];
char msg2[50];
char teste[20];
long tempoPressionado = 3000;
int buttonState = LOW;             // armazena a leitura atual do botao
int lastButtonState = LOW;         // armazena a leitura anterior do botao
unsigned long lastDebounceTime = 0;  // armazena a ultima vez que a leitura da entrada variou
unsigned long debounceDelay = 50;    // tempo utilizado para implementar o debounce


// definicao das funcoes relativas a cada estado

event btn_envio_state(void) {
  Serial.println("Tentando enviar os dados pelo botão...! ");
  if(enviar_dados()){
  digitalWrite(ledPin, HIGH);
  Serial.print("Temperatura: ");
  Serial.println(temp);
  Serial.print("Humidade: ");
  Serial.println(hum);   
  delay(500);
  digitalWrite(ledPin, LOW);
  Serial.println("Dados enviados! ");
  delay(100);
  return evento_gerenciador;
  }
  return evento_btn;  
}

event tempo_envio_state(void) {
  Serial.println("Tentando enviar os dados pelo tempo 10s...! ");
  if(enviar_dados()){
  digitalWrite(ledPin, HIGH);
  Serial.print("Temperatura: ");
  Serial.println(temp);
  Serial.print("Humidade: ");
  Serial.println(hum);   
  delay(500);
  digitalWrite(ledPin, LOW);
  Serial.println("Dados enviados! ");
  delay(100);
  return evento_gerenciador;
  }
  return evento_tempo;  
}

event conexao_wifi_state(void) {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Indo para o estado MQTT");
    return evento_mqtt;
  } else {
    Serial.print(".");
    Serial.println("Tentando  conexão com WIFI!");
    delay(3000);
    return evento_wifi;
  }
}

event conexao_mqtt_state(void) {
  //client.setServer(mqtt_server, 1883);
  Serial.print("Attempting MQTT connection...");
  if (client.connect("ESP8266Client", "A1E-KqqzbpU37bzMoWVPnwi8Z9kRTMj2Fn", "")) {
    Serial.println("connected");
    Serial.println("Indo para o estado GERENCIADOR!");
    return evento_gerenciador;
  } else {
    Serial.println("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 2 seconds");
    delay(2000);
    Serial.println("Continuo no estado MQTT!");
    return evento_mqtt;
  }
}

event gerenciador_state(void) {
  long now = millis();
  if (read_button()==true) {
    Serial.println("Indo para o estado envio BOTÃO!");
    return evento_btn;
  } else if (now - tempoDaUltimaMensagem >= 10000) { //10S
    tempoDaUltimaMensagem = now;
    //delay(2000);
    Serial.println("Indo para o estado envio por TEMPO !");
    return evento_tempo;
  } else {
    Serial.println("Continuo no estado gerenciador!");
    return evento_gerenciador;
  }
}

event sair_state(void) {
    Serial.println("ESTADO SAÍDA LED FICA PISCANDO!");
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW); 
    delay(600);
    return evento_sair;
}

// variaveis que armazenam estado atual, evento atual e funcao de tratamento do estado atual
state cur_state = ENTRY_STATE;
event cur_evt;
event (* cur_state_function)(void);

// implementacao de funcoes auxiliares
int read_button() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  lastButtonState = reading;
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        return true;
      }
    }
  }
  return false;
}

void setup() {
  client.setServer(mqtt_server, 1883);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  dht.begin();
  Serial.begin(115200);
  //setup_wifi();
  //client.setServer(mqtt_server, 1883);
}

int enviar_dados() {
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  snprintf (msg, 75, "{\"value\":%s}", dtostrf(temp, 4, 6, teste));
  snprintf (msg2, 75, "{\"value\":%s}", dtostrf(hum, 4, 6, teste));
  client.publish("/v1.6/devices/projeto-ih/temperatura", msg);
  client.publish("/v1.6/devices/projeto-ih/humidade", msg2);
  return true;
}

void loop() {
  cur_state_function = state_functions[cur_state];
  cur_evt = (event) cur_state_function();
  if (EXIT_STATE == cur_state)
    return;
  cur_state = lookup_transitions(cur_state, cur_evt);
}
