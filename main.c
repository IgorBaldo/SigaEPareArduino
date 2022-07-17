#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <stdint.h>
#include <EEPROM.h>


#define MINPRESSURE 200
#define MAXPRESSURE 1000
#define LIGHT_BLUE   0x65D7
#define GREY         0x8492
#define RED          0xF800
#define GREEN        0x07E0
#define YELLOW       0xFFE0
#define WHITE        0xFFFF

#define VermelhoFase1 22
#define AmareloFase1 24
#define VerdeFase1 26
#define VermelhoFase2 28
#define AmareloFase2 30
#define VerdeFase2 32

MCUFRIEND_kbv tft;

// ---- variaveis e configuração para o display
const int XP = 6, XM = A2, YP = A1, YM = 7; //ID=0x9341
const int TS_LEFT = 924, TS_RT = 195, TS_TOP = 201, TS_BOT = 937;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 320);
Adafruit_GFX_Button normal_btn, piscante_btn, tempos_btn, vermelho_btn, inicio_btn, gravar_btn,
unidadeFase1_btn, dezenaFase1_btn, centenaFase1_btn,
unidadeFase2_btn, dezenaFase2_btn, centenaFase2_btn,
unidadeVelocidade_btn, dezenaVelocidade_btn, centenaVelocidade_btn,
dezenaDistancia_btn, centenaDistancia_btn, milharDistancia_btn;
int pixel_x, pixel_y;  


// ----- declaração de variaveis -----
enum Cor{
    apagado = 0,
    vermelho = 1,
    amarelo = 2,
    verde = 3,
};

// contadores 
uint16_t counter = 0;
uint16_t tempo1s = 0;
uint16_t tempoAtualizaTela = 0;

// variaveis referente as fases
int fase = 1;
Cor corAtual = verde;
uint16_t tempoVermelho = 2;
uint16_t tempoVerdeFase1 = 5;
uint8_t tempoAmarelo = 3;
uint16_t tempoVerdeFase2 = 2;
uint16_t tempoNormal = tempoVerdeFase1;

// flag de controle
bool flagAtualizaTelaPiscante = 0;
bool modoNormal = 1;
bool modoPiscante = 0;
bool flagModoPiscante = 1;
bool flagBotaoModoNormal = 0;
bool flagBotaoModoPiscante = 1;

// variaveis para controle do display
uint8_t menu = 1;
bool mostraLed = 1;
bool atualizaTela = 0;
bool down = 0;
bool telaPretaPronta = 0;

// variaveis auxiliares
uint16_t distancia = 510;
uint16_t velocidade = 80;
uint16_t calculoTempoVermelho = 0;

bool flagGrava = 0;


uint8_t unidadeVerdeFase1 = 0;
uint8_t dezenaVerdeFase1 = 0;
uint8_t centenaVerdeFase1 = 0;


uint8_t unidadeVerdeFase2 = 0;
uint8_t dezenaVerdeFase2 = 0;
uint8_t centenaVerdeFase2 = 0;

uint16_t dezenaVelocidade = 0;

uint16_t dezenaDistancia = 0;
uint16_t centenaDistancia = 0;
uint16_t milharDistancia = 0;

uint8_t flagAlteraBotaoMenu2 = 0xFF;
uint8_t flagAlteraBotaoMenu3 = 0xFF;

// ------- Função de interrupções -----
ISR(TIMER2_OVF_vect){
  TCNT2 = 100; // reincializa contador de timer a cada 10ms                                          
  counter++; // incrementa contador
  tempo1s++; 
  tempoAtualizaTela++;
  if(counter == 15){
      counter = 0;
      down = Touch_getXY();
      if(menu == 1 ){ 
          normal_btn.press(down && normal_btn.contains(pixel_x, pixel_y));
          piscante_btn.press(down && piscante_btn.contains(pixel_x, pixel_y));
          tempos_btn.press(down && tempos_btn.contains(pixel_x, pixel_y));
          if (normal_btn.justPressed()  && flagBotaoModoNormal) {
              normal_btn.drawButton(false);
              piscante_btn.drawButton(true);
              tempos_btn.drawButton(true);
              flagBotaoModoNormal = 0;
              flagBotaoModoPiscante = 1;
              modoNormal = 1;
              modoPiscante = 0;
              corAtual = vermelho;
              tempoNormal = 3;
              fase = 2;
              flagAtualizaTelaPiscante = 0;
          } else if (piscante_btn.justPressed()  && flagBotaoModoPiscante) {
              normal_btn.drawButton(true);
              piscante_btn.drawButton(false);
              tempos_btn.drawButton(true);
              flagBotaoModoNormal = 1;
              flagBotaoModoPiscante = 0;
              modoNormal = 0;
              modoPiscante = 1;
              flagAtualizaTelaPiscante = 1;
          } else if (tempos_btn.justPressed() ) {
              tempos_btn.drawButton(false);
              normal_btn.drawButton(true);
              piscante_btn.drawButton(true);
              menu = 2;
              telaPreta();
              mostraLed = 0;
              montaMenu2();
          }
        
      } else if(menu == 2 ){
          vermelho_btn.press(down && vermelho_btn.contains(pixel_x, pixel_y));
          unidadeFase1_btn.press(down && unidadeFase1_btn.contains(pixel_x, pixel_y));
          dezenaFase1_btn.press(down && dezenaFase1_btn.contains(pixel_x, pixel_y));
          centenaFase1_btn.press(down && centenaFase1_btn.contains(pixel_x, pixel_y));
          unidadeFase2_btn.press(down && unidadeFase2_btn.contains(pixel_x, pixel_y));
          dezenaFase2_btn.press(down && dezenaFase2_btn.contains(pixel_x, pixel_y));
          centenaFase2_btn.press(down && centenaFase2_btn.contains(pixel_x, pixel_y));

           if (centenaFase1_btn.justPressed() ) {
              if(flagAlteraBotaoMenu2 & 0x01){
                 flagAlteraBotaoMenu2 &= 0xFE; 
                 centenaFase1_btn.drawButton(true);
              } else{
                flagAlteraBotaoMenu2 |= 0x01;
                centenaFase1_btn.drawButton(false);
              }
              
              centenaVerdeFase1++;
              if(centenaVerdeFase1 > 9){
                centenaVerdeFase1 = 0;
              }
              tft.fillRect(60, 30, 25,25, LIGHT_BLUE);
              tft.setTextColor(WHITE);
              tft.setCursor(65, 35);
              tft.println(centenaVerdeFase1);
              
          } else if (dezenaFase1_btn.justPressed() ) {

              if(flagAlteraBotaoMenu2 & 0x02){
                 flagAlteraBotaoMenu2 &= 0xFD; 
                 dezenaFase1_btn.drawButton(true);
              } else{
                flagAlteraBotaoMenu2 |= 0x02;
                dezenaFase1_btn.drawButton(false);
              }
              
              dezenaVerdeFase1++;
              if(dezenaVerdeFase1 > 9){
                dezenaVerdeFase1 = 0;
              }
              tft.fillRect(110, 30, 25,25, LIGHT_BLUE);
              tft.setTextColor(WHITE);
              tft.setCursor(115, 35);
              tft.println(dezenaVerdeFase1);
              
          }else if (unidadeFase1_btn.justPressed() ) {
              if(flagAlteraBotaoMenu2 & 0x04){
                 flagAlteraBotaoMenu2 &= 0xFB; 
                 unidadeFase1_btn.drawButton(true);
              } else{
                flagAlteraBotaoMenu2 |= 0x04;
                unidadeFase1_btn.drawButton(false);
              }
              
              unidadeVerdeFase1++;
              if(unidadeVerdeFase1 > 9){
                unidadeVerdeFase1 = 1;
              }
              tft.fillRect(160, 30, 25,25, LIGHT_BLUE);
              tft.setTextColor(WHITE);
              tft.setCursor(165, 35);
              tft.println(unidadeVerdeFase1);
              
          }
          
          if (centenaFase2_btn.justPressed() ) {
              if(flagAlteraBotaoMenu2 & 0x08){
                 flagAlteraBotaoMenu2 &= 0xF7; 
                 centenaFase2_btn.drawButton(true);
              } else{
                flagAlteraBotaoMenu2 |= 0x08;
                centenaFase2_btn.drawButton(false);
              }
              
              centenaVerdeFase2++;
              if(centenaVerdeFase2 > 9){
                centenaVerdeFase2 = 0;
              }
              tft.fillRect(60, 150, 25,25, LIGHT_BLUE);
              tft.setTextColor(WHITE);
              tft.setCursor(65, 155);
              tft.println(centenaVerdeFase2);
              
          }else if (dezenaFase2_btn.justPressed() ) {

              if(flagAlteraBotaoMenu2 & 0x10){
                 flagAlteraBotaoMenu2 &= 0xEF; 
                 dezenaFase2_btn.drawButton(true);
              } else{
                flagAlteraBotaoMenu2 |= 0x10;
                dezenaFase2_btn.drawButton(false);
              }
              
              dezenaVerdeFase2++;
              if(dezenaVerdeFase2 > 9){
                dezenaVerdeFase2 = 0;
              }
              tft.fillRect(110, 150, 25,25, LIGHT_BLUE);
              tft.setTextColor(WHITE);
              tft.setCursor(115, 155);
              tft.println(dezenaVerdeFase2);
              
          }else if (unidadeFase2_btn.justPressed() ) {
              if(flagAlteraBotaoMenu2 & 0x20){
                 flagAlteraBotaoMenu2 &= 0xDF; 
                 unidadeFase2_btn.drawButton(true);
              } else{
                flagAlteraBotaoMenu2 |= 0x20;
                unidadeFase2_btn.drawButton(false);
              }
              
              unidadeVerdeFase2++;
              if(unidadeVerdeFase2 > 9){
                unidadeVerdeFase2 = 1;
              }
              tft.fillRect(160, 150, 25,25, LIGHT_BLUE);
              tft.setTextColor(WHITE);
              tft.setCursor(165, 155);
              tft.println(unidadeVerdeFase2);
              
          }
          if (vermelho_btn.justPressed() ) {
              vermelho_btn.drawButton(false);
              menu = 3;
              telaPreta();
              mostraLed = 0;
              montaMenu3();
          }
      } else if(menu == 3 ){
          inicio_btn.press(down && inicio_btn.contains(pixel_x, pixel_y));
          gravar_btn.press(down && gravar_btn.contains(pixel_x, pixel_y));
          dezenaVelocidade_btn.press(down && dezenaVelocidade_btn.contains(pixel_x, pixel_y));
          milharDistancia_btn.press(down && milharDistancia_btn.contains(pixel_x, pixel_y));
          centenaDistancia_btn.press(down && centenaDistancia_btn.contains(pixel_x, pixel_y));
          dezenaDistancia_btn.press(down && dezenaDistancia_btn.contains(pixel_x, pixel_y));

          if(dezenaVelocidade_btn.justPressed() ){
              if(flagAlteraBotaoMenu3 & 0x01){
                 flagAlteraBotaoMenu3 &= 0xFE; 
                 dezenaVelocidade_btn.drawButton(true);
              } else{
                flagAlteraBotaoMenu3 |= 0x01;
                dezenaVelocidade_btn.drawButton(false);
              }
              
              dezenaVelocidade++;
              if(dezenaVelocidade > 9){
                dezenaVelocidade = 1;
              }
              tft.fillRect(85, 30, 25,25, LIGHT_BLUE);
              tft.setTextColor(WHITE);
              tft.setCursor(90, 35);
              tft.println(dezenaVelocidade);

              distancia = milharDistancia * 1000 + centenaDistancia * 100 + dezenaDistancia * 10;
              velocidade = dezenaVelocidade * 10;

              calculoTempoVermelho = distancia / (velocidade * 10 / 36);

              
              tft.fillRect(150, 230, 75, 30, LIGHT_BLUE);
              tft.setCursor(155, 235);
              tft.setTextSize(3);
              tft.setTextColor(RED);
              if(calculoTempoVermelho >=2){
                tft.print(calculoTempoVermelho);
              } else {
                tft.print(2);
              }
          }
          
          if(milharDistancia_btn.justPressed() ){
              if(flagAlteraBotaoMenu3 & 0x02){
                 flagAlteraBotaoMenu3 &= 0xFD; 
                 milharDistancia_btn.drawButton(true);
              } else{
                flagAlteraBotaoMenu3 |= 0x02;
                milharDistancia_btn.drawButton(false);
              }
              
              milharDistancia++;
              if(milharDistancia > 9){
                milharDistancia = 0;
              }
              tft.fillRect(40, 150, 25,25, LIGHT_BLUE);
              tft.setTextColor(WHITE);
              tft.setCursor(45, 155);
              tft.println(milharDistancia);

              distancia = milharDistancia * 1000 + centenaDistancia * 100 + dezenaDistancia * 10;
              velocidade = dezenaVelocidade * 10;

              calculoTempoVermelho = distancia / (velocidade * 10 / 36);

              
              tft.fillRect(150, 230, 75, 30, LIGHT_BLUE);
              tft.setCursor(155, 235);
              tft.setTextSize(3);
              tft.setTextColor(RED);
              if(calculoTempoVermelho >=2){
                tft.print(calculoTempoVermelho);
              } else {
                tft.print(2);
              }
          }else if(centenaDistancia_btn.justPressed() ){
              if(flagAlteraBotaoMenu3 & 0x04){
                 flagAlteraBotaoMenu3 &= 0xFB; 
                 centenaDistancia_btn.drawButton(true);
              } else{
                flagAlteraBotaoMenu3 |= 0x04;
                centenaDistancia_btn.drawButton(false);
              }
              
              centenaDistancia++;
              if(centenaDistancia > 9){
                centenaDistancia = 0;
              }
              tft.fillRect(90, 150, 25,25, LIGHT_BLUE);
              tft.setTextColor(WHITE);
              tft.setCursor(95, 155);
              tft.println(centenaDistancia);

              distancia = milharDistancia * 1000 + centenaDistancia * 100 + dezenaDistancia * 10;
              velocidade = dezenaVelocidade * 10;

              calculoTempoVermelho = distancia / (velocidade * 10 / 36);

              
              tft.fillRect(150, 230, 75, 30, LIGHT_BLUE);
              tft.setCursor(155, 235);
              tft.setTextSize(3);
              tft.setTextColor(RED);
              if(calculoTempoVermelho >=2){
                tft.print(calculoTempoVermelho);
              } else {
                tft.print(2);
              }
          }else if(dezenaDistancia_btn.justPressed() ){
              if(flagAlteraBotaoMenu3 & 0x08){
                 flagAlteraBotaoMenu3 &= 0xF7; 
                 dezenaDistancia_btn.drawButton(true);
              } else{
                flagAlteraBotaoMenu3 |= 0x08;
                dezenaDistancia_btn.drawButton(false);
              }
              
              dezenaDistancia++;
              if(dezenaDistancia > 9){
                dezenaDistancia = 1;
              }
              tft.fillRect(140, 150, 25,25, LIGHT_BLUE);
              tft.setTextColor(WHITE);
              tft.setCursor(145, 155);
              tft.println(dezenaDistancia);

              distancia = milharDistancia * 1000 + centenaDistancia * 100 + dezenaDistancia * 10;
              velocidade = dezenaVelocidade * 10;

              calculoTempoVermelho = distancia / (velocidade * 10 / 36);

              
              tft.fillRect(150, 230, 75, 30, LIGHT_BLUE);
              tft.setCursor(155, 235);
              tft.setTextSize(3);
              tft.setTextColor(RED);
              if(calculoTempoVermelho >=2){
                tft.print(calculoTempoVermelho);
              } else {
                tft.print(2);
              }
          }
          
          if (inicio_btn.justPressed()) {
              inicio_btn.drawButton(false);
              menu = 1;
              telaPreta();
              mostraLed = 1;
              montaMenu1();
              
          }else if (gravar_btn.justPressed() ) {
              gravar_btn.drawButton(false);
              

              
              EEPROM.write(1, centenaVerdeFase1);
              EEPROM.write(2, dezenaVerdeFase1);
              EEPROM.write(3, unidadeVerdeFase1);
              
              EEPROM.write(4, centenaVerdeFase2);
              EEPROM.write(5, dezenaVerdeFase2);
              EEPROM.write(6, unidadeVerdeFase2);

              EEPROM.write(7, dezenaVelocidade);

              EEPROM.write(8, milharDistancia);
              EEPROM.write(9, centenaDistancia);
              EEPROM.write(10, dezenaDistancia);
              
              distancia = milharDistancia * 1000 + centenaDistancia * 100 + dezenaDistancia * 10;
              velocidade = dezenaVelocidade * 10;

              calculoTempoVermelho = distancia / (velocidade * 10 / 36);
              tempoVermelho = calculoTempoVermelho;

              tempoVerdeFase1 = (centenaVerdeFase1 * 100) + (dezenaVerdeFase1 * 10) + (unidadeVerdeFase1);
              tempoVerdeFase2 = (centenaVerdeFase2 * 100) + (dezenaVerdeFase2 * 10) + (unidadeVerdeFase2);
              
              flagGrava = 1;
              
              
              
              menu = 1;   
              mostraLed = 1;
              telaPreta();
              montaMenu1();
          }
      }
  
      

      
  }
  if(tempo1s == 100){ // tempo igual 1 segundo?
      tempo1s = 0;
      if(modoNormal){
          tempoNormal--;
          if(tempoNormal <= 0)tempoNormal = 0;
          if(fase == 1){
              switch(corAtual){
              case vermelho:
                 if(flagGrava){
                    flagGrava = 0;
                    if(tempoVermelho >= 2){
                        tempoNormal = tempoVermelho;
                    } else {
                        tempoNormal = 2;
                    }
                  }
                  if(tempoNormal == 0){
                      corAtual = verde;
                      tempoNormal = tempoVerdeFase2;
                      fase = 2;
                  }
                  break;
              case verde:
                if(flagGrava){
                  flagGrava = 0;
                  tempoNormal = tempoVerdeFase1;
                }
                if(tempoNormal == 0){
                      corAtual = amarelo;
                      tempoNormal = tempoAmarelo;
                  }
                  break;
              case amarelo:
                  if(tempoNormal == 0){
                      corAtual = vermelho;
                      tempoNormal = tempoVermelho;
                  }
                  break;
              case apagado:
                 break;
                  
              }
          } else if(fase == 2){
              switch(corAtual){
              case vermelho:
                  if(flagGrava){
                    flagGrava = 0;
                    if(tempoVermelho >= 2){
                        tempoNormal = tempoVermelho;
                    } else {
                        tempoNormal = 2;
                    }
                    
                  }
                  if(tempoNormal == 0){
                      corAtual = verde;
                      tempoNormal = tempoVerdeFase1;
                      fase = 1;
                  }
                  break;
              case verde:
                if(flagGrava){
                  flagGrava = 0;
                  tempoNormal = tempoVerdeFase2;
                }
                if(tempoNormal == 0){
                      corAtual = amarelo;
                      tempoNormal = tempoAmarelo;
                  }
                  break;
              case amarelo:
                  if(tempoNormal == 0){
                      corAtual = vermelho;
                      tempoNormal = tempoVermelho;
                  }
                  break;
              case apagado:
                 break;
                  
              }
          }
          
      } else if(modoPiscante) flagModoPiscante ^= 1;
      checarCorAtual(); 
  }


  /*if(tempoAtualizaTela == 1){ // atualiza a tela a cada 100ms
      tempoAtualizaTela = 0;
      
  }
  */
  
} // fim função de interrupção


void setup(void)
{
    // configurando o display
    
    unidadeVerdeFase1 = EEPROM.read(3);
    dezenaVerdeFase1 =  EEPROM.read(2);
    centenaVerdeFase1 = EEPROM.read(1);
    unidadeVerdeFase2 = EEPROM.read(6);
    dezenaVerdeFase2 = EEPROM.read(5);
    centenaVerdeFase2 = EEPROM.read(4);
    
    dezenaVelocidade =  EEPROM.read(7);
    
    dezenaDistancia = EEPROM.read(10);
    centenaDistancia = EEPROM.read(9);
    milharDistancia = EEPROM.read(8);
              
    distancia = milharDistancia * 1000 + centenaDistancia * 100 + dezenaDistancia * 10;
    velocidade = dezenaVelocidade * 10;
    
    calculoTempoVermelho = distancia / (velocidade * 10 / 36);
    tempoVermelho = calculoTempoVermelho;

    tempoVerdeFase1 = (centenaVerdeFase1 * 100) + (dezenaVerdeFase1 * 10) + unidadeVerdeFase1;
    tempoVerdeFase2 = (centenaVerdeFase2 * 100) + (dezenaVerdeFase2 * 10) + unidadeVerdeFase2;
    tempoNormal = tempoVerdeFase1;

    

    
    tft.reset();
    tft.begin(0x9341);
    tft.setRotation(0);            //PORTRAIT
    tft.fillScreen(LIGHT_BLUE);

    // configurando as saidas digitais
    pinMode(VermelhoFase1, OUTPUT);
    pinMode(AmareloFase1, OUTPUT);
    pinMode(VerdeFase1, OUTPUT);
    
    pinMode(VermelhoFase2, OUTPUT);
    pinMode(AmareloFase2, OUTPUT);
    pinMode(VerdeFase2, OUTPUT);

    Serial.begin(9600);
    
 
    normal_btn.initButton(&tft,  60, 240, 100, 40, WHITE, WHITE, LIGHT_BLUE, "NORMAL", 2);
    piscante_btn.initButton(&tft, 180, 240, 100, 40, WHITE, YELLOW, LIGHT_BLUE, "PISCANTE", 2);
    
    tempos_btn.initButton(&tft, 60, 290, 100, 40, WHITE, WHITE, LIGHT_BLUE, "TEMPOS", 2);

    vermelho_btn.initButton(&tft, 180, 290, 100, 40, WHITE, WHITE, LIGHT_BLUE, ">>", 2);

    inicio_btn.initButton(&tft, 60, 290, 100, 40, WHITE, WHITE, LIGHT_BLUE, "INICIO", 2);
    gravar_btn.initButton(&tft, 180, 290, 100, 40, WHITE, WHITE, GREY, "GRAVAR", 2);

    
    centenaFase1_btn.initButton(&tft, 60, 80, 50, 50, WHITE, LIGHT_BLUE, WHITE, "+", 2);
    dezenaFase1_btn.initButton(&tft, 120, 80, 50, 50, WHITE, LIGHT_BLUE, WHITE, "+", 2);
    unidadeFase1_btn.initButton(&tft, 180, 80, 50, 50, WHITE, LIGHT_BLUE, WHITE, "+", 2);

    centenaFase2_btn.initButton(&tft, 60, 200, 50, 50, WHITE, LIGHT_BLUE, WHITE, "+", 2);
    dezenaFase2_btn.initButton(&tft, 120, 200, 50, 50, WHITE, LIGHT_BLUE, WHITE, "+", 2);
    unidadeFase2_btn.initButton(&tft, 180, 200, 50, 50, WHITE, LIGHT_BLUE, WHITE, "+", 2);

    
    dezenaVelocidade_btn.initButton(&tft, 95, 80, 50, 50, WHITE, LIGHT_BLUE, WHITE, "+", 2);

    milharDistancia_btn.initButton(&tft, 40, 200, 50, 50, WHITE, LIGHT_BLUE, WHITE, "+", 2);
    centenaDistancia_btn.initButton(&tft, 100, 200, 50, 50, WHITE, LIGHT_BLUE, WHITE, "+", 2);
    dezenaDistancia_btn.initButton(&tft, 160, 200, 50, 50, WHITE, LIGHT_BLUE, WHITE, "+", 2);
    
    modoNormal = 1;
    modoPiscante = 0;
    corAtual = vermelho;
    tempoNormal = 3;
    fase = 2;
    
    if(menu == 1){
        montaMenu1();
    }

    digitalWrite(VermelhoFase1, LOW);
    digitalWrite(AmareloFase1, LOW);
    digitalWrite(VerdeFase1, HIGH);
    digitalWrite(VermelhoFase2, HIGH);
    digitalWrite(AmareloFase2, LOW);
    digitalWrite(VerdeFase2, LOW);

    TCCR2A = 0x00; // setando registrador do timer2
    TCCR2B = 0x07; // setando registrador do timer2
    TCNT2 = 100; // inicializando o contador to timer2 em 100
    TIMSK2 = 0x01; // habilitando a interrupção por estouro do timer2
    
} // fim setup

void loop(void)
{
        
 
} // fim loop


// ------- Definições das funções -------

bool Touch_getXY(void) // função da tela
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

void montaMenu1(){
        if(modoNormal && mostraLed){  
            tft.fillCircle(70, 40, 30, GREY);
            tft.fillCircle(70, 110, 30, GREY);
            tft.fillCircle(70, 180, 30, GREY);
            tft.fillCircle(170, 40, 30, GREY);
            tft.fillCircle(170, 110, 30, GREY);
            tft.fillCircle(170, 180, 30, GREY);
            tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
            normal_btn.drawButton(false);
            piscante_btn.drawButton(true);
            
        } else if(modoPiscante && mostraLed){
            tft.fillCircle(70, 40, 30, GREY);
            tft.fillCircle(70, 110, 30, GREY);
            tft.fillCircle(70, 180, 30, GREY);
            tft.fillCircle(170, 40, 30, GREY);
            tft.fillCircle(170, 110, 30, GREY);
            tft.fillCircle(170, 180, 30, GREY);
            tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
            normal_btn.drawButton(true);
            piscante_btn.drawButton(false);
        }
        tempos_btn.drawButton(true);
      
} // fim montamenu1

void montaMenu2(){
        tft.setTextColor(WHITE);
        tft.setCursor(55, 10);
        tft.println("Verde Fase 1:");
        tft.setCursor(65, 35);
        tft.println(centenaVerdeFase1);
        tft.setCursor(115, 35);
        tft.println(dezenaVerdeFase1);
        tft.setCursor(165, 35);
        tft.println(unidadeVerdeFase1);
        tft.setCursor(190, 35);
        tft.println("s");

        
        tft.setCursor(55, 130);
        tft.println("Verde Fase 2:");
        tft.setCursor(65, 155);
        tft.println(centenaVerdeFase2);
        tft.setCursor(115, 155);
        tft.println(dezenaVerdeFase2);
        tft.setCursor(165, 155);
        tft.println(unidadeVerdeFase2);
        tft.setCursor(190, 155);
        tft.println("s");
        centenaFase1_btn.drawButton(false);
        dezenaFase1_btn.drawButton(false);
        unidadeFase1_btn.drawButton(false);
        centenaFase2_btn.drawButton(false);
        dezenaFase2_btn.drawButton(false);
        unidadeFase2_btn.drawButton(false);

        vermelho_btn.drawButton(true);


      
} // fim montamenu2

void montaMenu3(){
        tft.setTextColor(WHITE);
        tft.setCursor(55, 10);
        tft.println("Velocidade:");
        tft.setCursor(90, 35);
        tft.println(dezenaVelocidade);
        tft.setCursor(125, 35);
        tft.println(0);
        tft.setCursor(150, 35);
        tft.println("Km/h");
        
        
        tft.setCursor(55, 130);
        tft.println("Distancia:");
        tft.setCursor(45, 155);
        tft.println(milharDistancia);
        tft.setCursor(95, 155);
        tft.println(centenaDistancia);
        tft.setCursor(145, 155);
        tft.println(dezenaDistancia);
        tft.setCursor(180, 155);
        tft.println(0);
        tft.setCursor(205, 155);
        tft.println("m");
        tft.setCursor(30, 235);
        tft.print("Vermelho: ");
        tft.setCursor(155, 235);
        tft.setTextSize(3);
        tft.setTextColor(RED);
        if(calculoTempoVermelho >=2){
          tft.print(calculoTempoVermelho);
        } else {
          tft.print(2);
        }
        
        
        dezenaVelocidade_btn.drawButton(false);
        milharDistancia_btn.drawButton(false);
        centenaDistancia_btn.drawButton(false);
        dezenaDistancia_btn.drawButton(false);

        inicio_btn.drawButton(true);
        gravar_btn.drawButton(true);


      
} // fim montamenu2

void telaPreta(){
    tft.fillScreen(LIGHT_BLUE);
}


void checarCorAtual() // função para checar a cor acesa
{
    //if(atualizaTela){
        if(fase == 1){ 
            if(corAtual == vermelho && !modoPiscante){
                if(menu == 1 && mostraLed){

                      tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                      tft.setCursor(135, 275);
                      tft.setTextSize(4);
                      tft.setTextColor(RED);
                      tft.println(tempoNormal);
                      tft.fillCircle(70, 40, 30, RED);
                      tft.fillCircle(70, 110, 30, GREY);
                      tft.fillCircle(70, 180, 30, GREY);
                  }
                  digitalWrite(VermelhoFase1, HIGH);
                  digitalWrite(AmareloFase1, LOW);
                  digitalWrite(VerdeFase1, LOW);
                 
                
            } else if(corAtual == verde && !modoPiscante){
                if(menu == 1 && mostraLed){
                      tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                      tft.setCursor(135, 275);
                      tft.setTextSize(4);
                      tft.setTextColor(GREEN);
                      tft.println(tempoNormal);
                      tft.fillCircle(70, 40, 30, GREY);
                      tft.fillCircle(70, 110, 30, GREY);
                      tft.fillCircle(70, 180, 30, GREEN);
                      tft.fillCircle(170, 40, 30, RED);
                      tft.fillCircle(170, 110, 30, GREY);
                      tft.fillCircle(170, 180, 30, GREY);
                  }
                  digitalWrite(VermelhoFase1, LOW);
                  digitalWrite(AmareloFase1, LOW);
                  digitalWrite(VerdeFase1, HIGH);
                
            } else if(corAtual == amarelo && !modoPiscante){
                if(menu == 1 && mostraLed){
                      tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                      tft.setCursor(135, 275);
                      tft.setTextSize(4);
                      tft.setTextColor(YELLOW);
                      tft.println(tempoNormal);
                      tft.fillCircle(70, 40, 30, GREY);
                      tft.fillCircle(70, 110, 30, YELLOW);
                      tft.fillCircle(70, 180, 30, GREY);
                      tft.fillCircle(170, 40, 30, RED);
                      tft.fillCircle(170, 110, 30, GREY);
                      tft.fillCircle(170, 180, 30, GREY);
                    }

                  digitalWrite(VermelhoFase1, LOW);
                  digitalWrite(AmareloFase1, HIGH);
                  digitalWrite(VerdeFase1, LOW);
                  
                
            } else if(modoPiscante){
                if(flagAtualizaTelaPiscante){
                  flagAtualizaTelaPiscante = 0;
                    if(menu == 1 && mostraLed){
                          tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                          tft.fillCircle(70, 40, 30, GREY);
                          tft.fillCircle(70, 110, 30, GREY);
                          tft.fillCircle(70, 180, 30, GREY);
                          tft.fillCircle(170, 40, 30, GREY);
                          tft.fillCircle(170, 110, 30, GREY);
                          tft.fillCircle(170, 180, 30, GREY);
                      }

                       digitalWrite(VermelhoFase1, LOW);
                       digitalWrite(AmareloFase1, LOW);
                       digitalWrite(VerdeFase1, LOW);
                       digitalWrite(VermelhoFase2, LOW);
                       digitalWrite(AmareloFase2, LOW);
                       digitalWrite(VerdeFase2, LOW);
                    
                }
                if(flagModoPiscante){
                    if(menu == 1 && mostraLed){
                          tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                          tft.fillCircle(70, 110, 30, YELLOW);
                          tft.fillCircle(170, 110, 30, YELLOW);
                      }
                       digitalWrite(AmareloFase1, HIGH);
                       digitalWrite(AmareloFase2, HIGH);
                    
                } else {
                    if(menu == 1 && mostraLed){
                          tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                          tft.fillCircle(70, 110, 30, GREY);
                          tft.fillCircle(170, 110, 30, GREY);
                      }
                       digitalWrite(AmareloFase1, LOW);
                       digitalWrite(AmareloFase2, LOW);
                    
                }
         }
        } else if(fase == 2){
            if(corAtual == vermelho && !modoPiscante){
                if(menu == 1 && mostraLed){
                        tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                        tft.setCursor(135, 275);
                        tft.setTextSize(4);
                        tft.setTextColor(RED);
                        tft.println(tempoNormal);
                        tft.fillCircle(70, 40, 30, RED);
                        tft.fillCircle(70, 110, 30, GREY);
                        tft.fillCircle(70, 180, 30, GREY);
                        tft.fillCircle(170, 40, 30, RED);
                        tft.fillCircle(170, 110, 30, GREY);
                        tft.fillCircle(170, 180, 30, GREY);
                    }
                       digitalWrite(VermelhoFase1, HIGH);
                       digitalWrite(AmareloFase1, LOW);
                       digitalWrite(VerdeFase1, LOW);
                       digitalWrite(VermelhoFase2, HIGH);
                       digitalWrite(AmareloFase2, LOW);
                       digitalWrite(VerdeFase2, LOW);
                 
            } else if(corAtual == verde && !modoPiscante){
                if(menu == 1 && mostraLed){
                        tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                        tft.setCursor(135, 275);
                        tft.setTextSize(4);
                        tft.setTextColor(GREEN);
                        tft.println(tempoNormal);
                       tft.fillCircle(70, 40, 30, RED);
                       tft.fillCircle(70, 110, 30, GREY);
                       tft.fillCircle(70, 180, 30, GREY);
                       tft.fillCircle(170, 40, 30, GREY);
                       tft.fillCircle(170, 110, 30, GREY);
                       tft.fillCircle(170, 180, 30, GREEN);
                    }
                     digitalWrite(VermelhoFase2, LOW);
                     digitalWrite(AmareloFase2, LOW);
                     digitalWrite(VerdeFase2, HIGH);
                
            } else if(corAtual == amarelo && !modoPiscante){
                 if(menu == 1 && mostraLed){
                        tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                        tft.setCursor(135, 275);
                        tft.setTextSize(4);
                        tft.setTextColor(YELLOW);
                        tft.println(tempoNormal);
                        tft.fillCircle(70, 40, 30, RED);
                        tft.fillCircle(70, 110, 30, GREY);
                        tft.fillCircle(70, 180, 30, GREY);
                        tft.fillCircle(170, 40, 30, GREY);
                        tft.fillCircle(170, 110, 30, YELLOW);
                        tft.fillCircle(170, 180, 30, GREY);
                    }

                     digitalWrite(VermelhoFase2, LOW);
                     digitalWrite(AmareloFase2, HIGH);
                     digitalWrite(VerdeFase2, LOW);
                 
            } else if(modoPiscante){
                if(flagAtualizaTelaPiscante){
                  flagAtualizaTelaPiscante = 0;
                    if(menu == 1 && mostraLed){
                          tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                          tft.fillCircle(70, 40, 30, GREY);
                          tft.fillCircle(70, 110, 30, GREY);
                          tft.fillCircle(70, 180, 30, GREY);
                          tft.fillCircle(170, 40, 30, GREY);
                          tft.fillCircle(170, 110, 30, GREY);
                          tft.fillCircle(170, 180, 30, GREY);
                      }
                       digitalWrite(VermelhoFase1, LOW);
                       digitalWrite(AmareloFase1, LOW);
                       digitalWrite(VerdeFase1, LOW);
                       digitalWrite(VermelhoFase2, LOW);
                       digitalWrite(AmareloFase2, LOW);
                       digitalWrite(VerdeFase2, LOW);
                    
                }
                if(flagModoPiscante){
                    if(menu == 1 && mostraLed ){
                          tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                          tft.fillCircle(70, 110, 30, YELLOW);
                          tft.fillCircle(170, 110, 30, YELLOW);
                      }
                       digitalWrite(AmareloFase1, HIGH);
                       digitalWrite(AmareloFase2, HIGH);
                    
                } else {
                    if(menu == 1 && mostraLed){
                          tft.fillRect(135 , 275, 95, 35, LIGHT_BLUE);
                          tft.fillCircle(70, 110, 30, GREY);
                          tft.fillCircle(170, 110, 30, GREY);
                      }
                       digitalWrite(AmareloFase1, LOW);
                       digitalWrite(AmareloFase2, LOW);
                    
                }
             }  
        } // fim fase 2
    //} // fim atualiza tela
} // fim checarCorAtual
