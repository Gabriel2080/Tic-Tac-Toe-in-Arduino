#include <Adafruit_NeoPixel.h>

#define led 13

#define NUMPIXELS 12

Adafruit_NeoPixel led_game = Adafruit_NeoPixel(NUMPIXELS, led, NEO_GRB + NEO_KHZ800);

#include <Wire.h>
#include <Keypad.h>

#define MEU_ENDERECO 1
#define OUTRO_ENDERECO 2

int matriz[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

int jogador;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'0','1','2','A'},
  {'4','5','6','B'},
  {'8','9','X','C'},
  {'*','I','#','D'}
};
byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {7, 6, 5, 4};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup()
{
  Wire.begin(MEU_ENDERECO);
  Wire.onReceive(receiveEvent);
  Wire.begin();
  Serial.begin(9600);
  led_game.begin();
  randomSeed(analogRead(0));
  Serial.println("jogador 1");
  sorteio();
}

bool aguardando_mensagem;

void espera_mensagem(){
  while (aguardando_mensagem == false) { 
    char key = keypad.getKey();
    while(!(key == '0' && matriz[0] == 0)&&!(key == '1' && matriz[1] == 0)&&!(key == '2' && matriz[2] == 0)
          &&!(key == '4' && matriz[4] == 0)&&!(key == '6' && matriz[6] == 0)&&!(key == '5' && matriz[5] == 0)
          &&!(key == '8' && matriz[8] == 0)&&!(key == '9' && matriz[9] == 0)&&!(key == 'X' && matriz[10] == 0)){
      key = keypad.getKey();
      if(aguardando_mensagem == true){break;}
    }
    if(aguardando_mensagem == true){break;}
    if(key == 'X'){
      led_game.setPixelColor(10, led_game.Color(0, 0, 255));
      matriz[10] = 1;
      led_game.show();
    }else{
      led_game.setPixelColor(key-'0', led_game.Color(0, 0, 255));
      matriz[key-'0'] = 1;
      led_game.show();
    }

    Wire.beginTransmission(OUTRO_ENDERECO);
    Wire.write('u');
    Wire.write(key);
    Wire.endTransmission();
    aguardando_mensagem = true;
  }
  delay(500);
}

void loop(){
  switch (jogador){
    case 1:espera_mensagem();break;

    case 3:sorteio();break;
  }
}

void vitoria(int vencendor){
  if(vencendor == 3){
    Serial.println("empate");
  }else{
    Serial.print("jogador ");
    Serial.print(vencendor);
    Serial.println(" venceu");
  }
  Wire.beginTransmission(OUTRO_ENDERECO);
  Wire.write('p');
  Wire.write(vencendor);
  Wire.endTransmission();
  desmarcar_LED_Matriz();
  jogador = 3;
}

void verificar_vitoria(){
  if((matriz[0] == matriz[1] && matriz[1] == matriz[2])
     && matriz[0]!=0 && matriz[1]!=0 && matriz[2]!=0){
    vitoria(matriz[0]);

  }else if((matriz[4] == matriz[5] && matriz[5] == matriz[6]) 
           && matriz[4]!=0 && matriz[5]!=0 && matriz[6]!=0){
    vitoria(matriz[4]);
  }else if((matriz[8] == matriz[9] && matriz[9] == matriz[10]) 
           && matriz[8]!=0 && matriz[9]!=0 && matriz[10]!=0){
    vitoria(matriz[8]);
  }
  ////////
  else if((matriz[0] == matriz[4] && matriz[4] == matriz[8]) 
          && matriz[0]!=0 && matriz[4]!=0 && matriz[8]!=0){
    vitoria(matriz[0]);
  }else if((matriz[1] == matriz[5] && matriz[5] == matriz[9]) 
           && matriz[1]!=0 && matriz[5]!=0 && matriz[9]!=0){
    vitoria(matriz[1]);
  }else if((matriz[2] == matriz[6] && matriz[6] == matriz[10]) 
           && matriz[2]!=0 && matriz[6]!=0 && matriz[10]!=0){
    vitoria(matriz[2]);
  }
  ///////
  else if((matriz[0] == matriz[5] && matriz[5] == matriz[10]) 
          && matriz[0]!=0 && matriz[5]!=0 && matriz[10]!=0){
    vitoria(matriz[0]);
  }else if((matriz[2] == matriz[5] && matriz[5] == matriz[8]) 
           && matriz[2]!=0 && matriz[5]!=0 && matriz[8]!=0){
    vitoria(matriz[2]);
  }
  ///////
  else if(matriz[0]!=0 && matriz[1]!=0 && matriz[2]!=0
          && matriz[4]!=0 && matriz[5]!=0 && matriz[6]!=0
          && matriz[8]!=0 && matriz[9]!=0 && matriz[10]!=0){
    vitoria(3);
  }
}

void sorteio(){
  int sorte = random(1, 3);
  if(sorte == 1){
    aguardando_mensagem = false;
  }else{
    aguardando_mensagem = true;
  }
  jogador = 1;
  Wire.beginTransmission(OUTRO_ENDERECO); 
  Serial.print("o primeiro a comecar e: ");
  Wire.write("k");
  Serial.print("jogador ");
  Serial.println(sorte);
  Wire.write(sorte);               
  Wire.endTransmission();
}

void desmarcar_LED_Matriz(){
  for(int i=0;i<NUMPIXELS;i++){
    led_game.setPixelColor(i, 0,0,0);
    matriz[i]= 0;
    led_game.show();
  }
}

void receiveEvent(int howMany){
  char x = Wire.read();

  if(x == 'k'){
    Serial.print("o primeiro a comecar e: ");
    int m = Wire.read();
    Serial.print("jogador ");
    Serial.println(m);
    jogador = 1;
    if(m == 1){
      aguardando_mensagem = false;
    }else{
      aguardando_mensagem = true;
    }

  }else if(x == 'p'){
    int m = Wire.read();
    if(m == 3){
      Serial.println("empate");
    }else{
      Serial.print("jogador ");
      Serial.print(m);
      Serial.println(" venceu");
    }
    desmarcar_LED_Matriz();
    aguardando_mensagem = true;

  }else{
    Serial.print("jogador 2 foi apertado: "); 
    char m = Wire.read();
    Serial.println(m);
    if((m >= '0'&& m <= '9')){
      led_game.setPixelColor(m-'0', led_game.Color(255, 0, 0));
      matriz[m-'0'] = 2;
      led_game.show();
    }if(m == 'X'){
      led_game.setPixelColor(10, led_game.Color(255, 0, 0));
      matriz[10] = 2;
      led_game.show();
    }
    aguardando_mensagem = false;
    verificar_vitoria();
  }
}