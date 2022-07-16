//libs necessarias
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 10
#define MAXPRESSURE 1000
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SimpleTimer.h>
//pino do led
#define led 17

// dados do sensor de temperatura
#define pino_sensor 21
#define TEMPERATURE_PRECISION 9
OneWire oneWire(pino_sensor);
DallasTemperature sensors(&oneWire);


//dados de entrada e calibrção
const int XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9341
const int TS_LEFT = 79, TS_RT = 895, TS_TOP = 945, TS_BOT = 96;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); // 300 = sensibilidade

Adafruit_GFX_Button on_btn, off_btn;

int pixel_x, pixel_y;     //carrega as posições dos pixels
bool Touch_getXY(void)    //detecta o toque na tela
{
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);       //definição dos pinos
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);   
  digitalWrite(XM, HIGH);
  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {//definição para tela na horizontal
    pixel_x = map(p.y, TS_LEFT, TS_RT, 0, 320); // tamanho na horizontal
    pixel_y = map(p.x,   TS_TOP, TS_BOT, 0, 240 ); // tamanho na vetical
  }
  return pressed;
}

// lista de cores
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


//definição do timer
SimpleTimer timer;

//void da temperatura para o sensor DS18B20
void temp() {
  sensors.begin();
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  tft.setTextColor(WHITE, RED);
  tft.setTextSize(2.7);     // Fonte em 8 pixels.  ht = 8*2.7=21,6
  tft.setCursor(20, 88);    //posição na tela da variavel
  tft.print(tempC);
}
void botoes() {// logica para o toque nos botões ON/OFF
  bool down = Touch_getXY();
  on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
  off_btn.press(down && off_btn.contains(pixel_x, pixel_y));
  if (on_btn.justReleased())
    on_btn.drawButton();
  if (off_btn.justReleased())
    off_btn.drawButton();
  if (on_btn.justPressed()) {
    on_btn.drawButton(true); // se precionado liga o led e altera a cor do quadrado para verde
    tft.fillRect(230, 190, 40, 40, GREEN);
    digitalWrite(led, HIGH);
  }
  if (off_btn.justPressed()) {
    off_btn.drawButton(true);// se precionado desliga o led e altera a cor do quadrado para vermelho
    tft.fillRect(230, 190, 40, 40, RED);
    digitalWrite(led, LOW);
  }
}
void setup() {

  Serial.begin(9600);
  pinMode(led, OUTPUT);
  uint16_t ID = tft.readID();

  if (ID == 0xD3D3) ID = 0x9341; // escreva o ID da tela e motdelo
  tft.begin(ID);
  tft.setRotation(1);           //horizontal| vertical trocar o valor para (0)
  tft.fillScreen(BLACK);
                        //|posição|    info bototão     |  escrita interna |
                        // X  | Y |Tamanho| Borda| fundo| cor  |texto|fonte|
  on_btn.initButton(&tft,  60, 210, 100, 40, WHITE, CYAN, BLACK, "ON", 2);
  off_btn.initButton(&tft, 170, 210, 100, 40, WHITE, CYAN, BLACK, "OFF", 2);
  on_btn.drawButton(false);//inicia os valores em false(desligados)
  off_btn.drawButton(false);
  tft.fillRect(230, 190, 40, 40, RED);
  //titulo
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2.7);     // Fonte em 8 pixels.  ht = 8*2.7=21,6
  tft.setCursor(15, 20);
  tft.print("GAMBIARRAS DA TECNOLOGIA");
  
  
  // informações da temperatura
  // retangulo de fundo
  tft.fillRect(15, 80, 90, 30, RED);
  
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2.7);    // Fonte em 8 pixels.  ht = 8*2.7=21,6
  tft.setCursor(15, 60);
  tft.print("TEMPERATURA");

  tft.setTextColor(WHITE, RED);
  tft.setTextSize(2.7);    // Fonte em 8 pixels.  ht = 8*2.7=21,6
  tft.setCursor(90, 88);
  tft.print("C");
  // tempos de atualição
  timer.setInterval(1500L, temp);
  timer.setInterval(1L, botoes);

}


void loop() {
  timer.run();//inicia o timer execultando as funções(voids)
}
