#include <18F4620.h>
#device adc=10 
#fuses HS, NOFCMEN, NOIESO, PUT, NOBROWNOUT, NOWDT
#fuses NOPBADEN, NOMCLR, STVREN, NOLVP, NODEBUG
#use delay(clock=16000000)
//Definicion de LCD-----------------------------
#define lcd_rs_pin pin_d0
#define lcd_rw_pin pin_d1
#define lcd_enable_pin pin_d2
#define lcd_data4 pin_d4
#define lcd_data5 pin_d5
#define lcd_data6 pin_d6
#define lcd_data7 pin_d7 
#include <LCD.C>
#include "dht.h"
//---------------------------------------------
//Definicion de terminal----------------------
#define TX_232        PIN_C6
#define RX_232        PIN_C7
#use RS232(BAUD=9600, XMIT=TX_232, RCV=RX_232, BITS=8,PARITY=N, STOP=1)
//---------------------------------------------
//Botones
#define subir pin_b4
short botonSubir = 0;
#define bajar pin_b5
short botonBajar = 0;
#define enter pin_b6
short botonEnter = 0;
#define esc   pin_b7
short botonEsc = 0;
//Leds alarmas
#define ledTemperatura pin_b0
#define ledHumedad pin_b1
#define ledLPG pin_b2
#define ledAlcohol pin_b3
//Variables para el menu del LCD--------------
int posicionMenu=1;
long actualizacionLCD=100;
long actualizacionSerial=100;
//Banderas
short mensajeBienvenida = 0;
short configuraciones = 0;
short confSerial = 0;
short confUpdate = 0;
short confAlarma = 0;
short confTemp = 0;
short confHum = 0;
short confGasX = 0;
short confGasY = 0;
//Variables de configuraciones de tiempo de actualizacion
long tiempoSerial = 100; long auxiliarTS = 100;
long tiempoLCD = 100;    long auxiliarLCD = 100;
//Variable para las alarmas.
int alarmaTemp = 30; int auxiliarAT = 30;
int alarmaHum = 30;  int auxiliarAH = 30;
long alarmaLPG = 300; long auxiliarALPG = 300;
long alarmaAlcohol = 300; long auxiliarAA=300;
//Variables para lo sensado
int temp=0;
int hum=0;
long GasX=0;
long GasY=0;
//variables para funciones del PI
char identificador=" ";
//FUNCION BIENVENIDA---------------------------------------
void bienvenida()
{
   if(mensajeBienvenida == 0)
      {
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,"------VLM------");
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"  BIENVENIDO ");
         delay_ms(500); 
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,"Cargando...");
         for(int i=1;i<17;i++)
         {
            lcd_gotoxy(i,2);
            printf(lcd_putc,"*");   
            delay_ms(150);
         }
         printf(lcd_putc,"\f");
         mensajeBienvenida = 1;
      }
}
//---------------------------------------------------------
//FUNCION MENU---------------------------------------------
void menu()
{
   switch(posicionMenu)
   {
      case 1:
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,"->Tem:      %d%cC"temp,223);
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"Hum:         %d%%",hum);
      break;
      case 2:
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,"->Hum:       %d%%",hum);
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"LPG:       %ld%%  ",GasX);
      break;
      case 3:
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,"->LPG:     %ld%% ",GasX);
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"Alcohol:   %ld%% ",GasY);
      break;
      case 4:
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,"->Alcohol: %ld%% ",GasY);
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"Tem:        %d%cC"temp,223);
      break;
   }   
}
//---------------------------------------------------------
//Funcion Menu Configuraciones-----------------------------
void menuConfiguraciones()
{
   switch(posicionMenu)
   {
      case 1:
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,">Conf. Serial   ");
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"Conf. Update LCD");
      break;
      case 2:
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,">Conf.Update LCD ");
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"Conf. Alarmas   ");
      break;
      case 3:
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,">Conf. Alarmas  ");
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"Conf. Serial    ");
      break;
   }   
}
//---------------------------------------------------------
//Funcion menu conf serial.
void confSerialMenu()
{
   lcd_gotoxy(1,1);//Linea donde pintare.
   printf(lcd_putc,"     Tiempo     ");
   lcd_gotoxy(1,2);//Linea donde pintare.
   printf(lcd_putc,"  %ld  Ms",auxiliarTS);
}
//---------------------------------------------------------
//Funcion menu conf update.
void confUpdateMenu()
{
   lcd_gotoxy(1,1);//Linea donde pintare.
   printf(lcd_putc,"     Tiempo     ");
   lcd_gotoxy(1,2);//Linea donde pintare.
   printf(lcd_putc,"  %ld  Ms       ",auxiliarLCD);
}
//---------------------------------------------------------
//Funcion menu conf alarma.
void confAlarmaMenu()
{
   switch(posicionMenu)
   {
      case 1:
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,"->Temp     %d%cC",alarmaTemp,223);
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"Hum         %d%%",alarmaHum);
      break;
      case 2:
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,"->Hum       %d%%",alarmaHum);
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"LPG:       %ld%%",alarmaLPG);
      break;
      case 3:
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,"->LPG:     %ld%%",alarmaLPG);
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"Alcohol:   %ld%%",alarmaAlcohol);
      break;
      case 4:
         lcd_gotoxy(1,1);//Linea donde pintare.
         printf(lcd_putc,"->Alcohol: %ld%%",alarmaAlcohol);
         lcd_gotoxy(1,2);//Linea donde pintare.
         printf(lcd_putc,"Temp       %d%cC",alarmaTemp,223);
      break;
   }   

}
//---------------------------------------------------------
//Alarma temperatura
void confAlamarTemp()
{
   lcd_gotoxy(1,1);//Linea donde pintare.
   printf(lcd_putc,"  Temperatura   ");
   lcd_gotoxy(1,2);//Linea donde pintare.
   printf(lcd_putc,"  %d Grados  ",auxiliarAT);
}
//---------------------------------------------------------
//Alarma humedad
void confAlamarHum()
{
   lcd_gotoxy(1,1);//Linea donde pintare.
   printf(lcd_putc,"  Temperatura   ");
   lcd_gotoxy(1,2);//Linea donde pintare.
   printf(lcd_putc,"  %d %%  ",auxiliarAH);
}
//---------------------------------------------------------
//Alarma gasx
void confAlamarGasX()
{
   lcd_gotoxy(1,1);//Linea donde pintare.
   printf(lcd_putc,"  LPG   ");
   lcd_gotoxy(1,2);//Linea donde pintare.
   printf(lcd_putc,"  %ld %%  ",auxiliarALPG);
}
//---------------------------------------------------------
//Alarma gasy
void confAlamarGasY()
{
   
   lcd_gotoxy(1,1);//Linea donde pintare.
   printf(lcd_putc,"    Alcohol    ");
   lcd_gotoxy(1,2);//Linea donde pintare.
   printf(lcd_putc,"  %ld %%  ",auxiliarAA);
}
//---------------------------------------------------------
//Funcion de todos los menus-------------------------------
void menusAll()
{
   bienvenida();
   if(configuraciones == 1)
      {
         if(confSerial==0 & confUpdate==0 & confAlarma==0)
            menuConfiguraciones();
         if(confSerial==1)
            {
               confSerialMenu();
            }
         if(confUpdate==1)
            {
               confUpdateMenu();
            }
         if(confAlarma==1)
            {
               if(confTemp==1)
               {
                  confAlamarTemp();
               }
               else if(confHum==1)
               {
                  confAlamarHum();
               }
               else if(confGasX==1)
               {
                  confAlamarGasX();
               }
               else if(confGasY==1)
               {
                  confAlamarGasY();
               }
               else
               {
                  confAlarmaMenu();
               }
            }
      }
   else
      {
         menu();
      }
}
//---------------------------------------------------------
//Lectura temperatura humedad
void senseoTemperaturaHumedad(void)
{       
   hum = readData(); 
   int basura1 = readData();
   temp = readData();
   int basura2 = readData();
   int basura3 = readData();   
}
//----------------------------------------------------------
//Lectura LPG
void LPG(void)
{
   set_adc_channel(0);
   GasX=read_adc(ADC_READ_ONLY);
   delay_ms(1);
   read_adc(ADC_START_ONLY);      
}
//----------------------------------------------------------
//Lectura Alcohol
void Alcohol(void)
{
   set_adc_channel(1);
   GasY=read_adc(ADC_READ_ONLY);
   delay_ms(1);
   read_adc(ADC_START_ONLY);   
}
//----------------------------------------------------------
//funcion Comunicacion Serial
void comunicacionSerial()
{
   if(actualizacionSerial==tiempoSerial)
      {
         printf("Alarmas------------------\r");
         printf("Temperatura:   %d\r",alarmaTemp);
         printf("Humedad:       %d\r",alarmaHum);
         printf("LPG:           %ld\r",alarmaLPG);
         printf("Alcohol:       %ld\r",alarmaAlcohol);
         printf("-------------------------\r");
         actualizacionSerial=1;
      }
   else
      actualizacionSerial++;
}
//Funcion de actualizacion de LCD
void comunicacionLCD()
{
   if(actualizacionLCD>tiempoLCD)
   {  
       senseoTemperaturaHumedad();
       LPG();
       Alcohol();            
       actualizacionLCD=1;
   }
   else
       actualizacionLCD++;
}
void funcionPI()
{
   if(kbhit())
      identificador=getch();
   if(identificador=='1')
      {
         printf("%d&",temp);
         identificador = " ";
      }
   if(identificador == '2')
      {
         printf("%d&",hum);
         identificador = " ";
      }
   if(identificador == '3')
      {
         printf("%ld&",GasX);
         identificador = " ";
      }
   if(identificador == '4')
      {
         printf("%ld&",GasY);
         identificador = " ";
      }
}
//Funcion de validacion de alarmas
void alarmas()
{//-----------------------
   if(temp>=alarmaTemp)
      output_high(ledTemperatura);
   else
      output_low(ledTemperatura);
 //-----------------------     
   if(hum>=alarmaHum)
      output_high(ledHumedad);
   else
      output_low(ledHumedad);
 //-----------------------
   if(GasX>=alarmaLPG)
      output_high(ledLPG);
   else
      output_low(ledLPG);
 //-----------------------     
   if(GasY>=alarmaAlcohol)
      output_high(ledAlcohol);
   else
      output_low(ledAlcohol);
}
void botones()
{
   
         if(botonSubir == 1)
            {
               
         if(configuraciones==0)
         {
            if(posicionMenu==4)
               posicionMenu=1;
            else
            posicionMenu++;
         }
         else
         {
            if(confSerial == 1)
            {
               if(auxiliarTS==100)
                  auxiliarTS=5000;
               else
                  auxiliarTS-=100;
            }
            else if(confUpdate == 1)
            {
               if(auxiliarLCD == 100)
                  auxiliarLCD = 2000;
               else
                  auxiliarLCD-=100;
            }            
            else if(confAlarma == 1)
            {
               if(confTemp==1)
               {
                  if(auxiliarAT==1)
                     auxiliarAT=49;
                  else
                     auxiliarAT-=1;                     
               }
               if(confHum==1)
               {
                  if(auxiliarAH==21)
                     auxiliarAH=94;
                  else
                     auxiliarAH-=1;
               }
               if(confGasX==1)
               {
                  if(auxiliarALPG==300)
                     auxiliarALPG=10000;
                  else
                     auxiliarALPG-=100;
               }
               if(confGasY==1)
               {
                  if(auxiliarAA==300)
                     auxiliarAA=10000;
                  else
                     auxiliarAA-=100;
               }
               if(posicionMenu==4)
                  posicionMenu=1;
               else
                  posicionMenu++;
            }
            else
            {
               if(posicionMenu==3)
                  posicionMenu=1;
               else
                  posicionMenu++;
            }            
         }
         printf(lcd_putc,"\f");
         botonSubir=0;
            }
         if(botonBajar == 1)
            {
               
         if(configuraciones==0)
         {
            if(posicionMenu==1)
               posicionMenu=4;
            else
               posicionMenu--;
         }
         else
         {
            
            if(confSerial == 1)
            {
               if(auxiliarTS==5000)
                  auxiliarTS=100;
               else
                  auxiliarTS+=100;
            }
            else if(confUpdate == 1)
            {
               if(auxiliarLCD == 2000)
                  auxiliarLCD = 100;
               else
                  auxiliarLCD+=100;
            }
            else if(confAlarma == 1)
            {
               if(confTemp==1)
               {
                  if(auxiliarAT==49)
                     auxiliarAT=1;
                  else
                     auxiliarAT+=1;
                     
               }
               if(confHum==1)
               {
                  if(auxiliarAH==94)
                     auxiliarAH=21;
                  else
                     auxiliarAH+=1;
               }
               if(confGasX==1)
               {
                  if(auxiliarALPG==10000)
                     auxiliarALPG=300;
                  else
                     auxiliarALPG+=100;
               }
               if(confGasY==1)
               {
                  if(auxiliarAA==10000)
                     auxiliarAA=300;
                  else
                     auxiliarAA+=100;
               }
               if(posicionMenu==1)
                  posicionMenu=4;
               else
                  posicionMenu--;
            }
            else
            {
               if(posicionMenu==1)
                  posicionMenu=3;
               else
                  posicionMenu--;
            }  
         }
         printf(lcd_putc,"\f");
            botonBajar=0;
            }
         if(botonEnter == 1)
            {
              if(configuraciones == 1)//Si el menu de configuraciones habilitado
          {
            if(confSerial==0 && confUpdate==0 && confAlarma==0)
            {
               switch(posicionMenu)//Depende en la posicion donde se encuentre
               {//Se habilitara el siguiente menu.
                  case 1:
                     confSerial=1;
                  break;
                  case 2:
                     confUpdate=1;
                  break;
                  case 3:
                     confAlarma=1;
                     posicionMenu=1;//Reseteo el contador para el menu alarmas
                  break;
               }
            }
            else
            {
               if(confSerial == 1)//Checo si esta dentro de la confSerial
                  {
                     tiempoSerial = auxiliarTS;//Guardo el cambio
                  }
               if(confUpdate == 1)//Checo si esta entro de la confUpdateLCD
                  {
                     tiempoLCD = auxiliarLCD;//Guardo el cambio
                  }
               if(confAlarma == 1)//Checo si esta dentro de la confAlarma
                  {
                     if(confTemp==0 && confHum==0 && confGasX==0 && confGasY==0)
                     {
                        switch(posicionMenu)
                           {
                              case 1:
                                 confTemp = 1;
                              break;
                              case 2:
                                 confHum = 1;
                              break;
                              case 3:
                                 confGasX = 1;
                              break;
                              case 4:
                                 confGasY = 1;
                              break;
                           }
                     }
                     else
                     {
                        if(confTemp==1)
                        {
                           alarmaTemp = auxiliarAT;
                        }
                        if(confHum==1)
                        {
                           alarmaHum = auxiliarAH;
                        }
                        if(confGasX==1)
                        {
                           alarmaLPG = auxiliarALPG;
                        }
                        if(confGasY==1)
                        {
                           alarmaAlcohol = auxiliarAA;
                        }
                     }
                  }
              }
          }        
         if(configuraciones == 0)//SI el menu de configuraciones no esta mostrandose
            {
               configuraciones = 1;//Tons lo muestro.
               posicionMenu=1;//Reseteo el contador del menu
            }
         printf(lcd_putc,"\f");//Limpar la pantalla en cada cambio.
            botonEnter=0;
            }
         if(botonEsc == 1)
            {
            
         if(configuraciones == 1)//Si el menu de configuraciones habilitado
          {
            if(confSerial == 1)//Checo si esta dentro de la confSerial
               {
                  confSerial=0;//Guardo el cambio
               }
            else if(confUpdate == 1)//Checo si esta entro de la confUpdateLCD
               {
                  confUpdate = 0;//Guardo el cambio
               }
            else if(confAlarma == 1)//Checo si esta dentro de la confAlarma
               {
                 if(confTemp==1)
                    confTemp = 0;
                 else if(confHum==1)
                    confHum = 0;
                 else if(confGasX==1)
                    confGasX = 0;
                 else if(confGasY==1)
                    confGasY = 0;
                 else
                    {
                        confAlarma = 0;
                        posicionMenu=1;
                    }
               }
            else 
               {
                  configuraciones = 0;//Tons lo muestro.
                  posicionMenu=1;//Reseteo el contador del menu
               }
          }
         printf(lcd_putc,"\f");//Limpar la pantalla en cada cambio.
             botonEsc=0;
            }
}
#INT_RB
void interrupt_isr(void)
{     //Boton subir
      if(input(subir)==1)
      {
         botonSubir=1;
      }
      //Boton bajar
      if(input(bajar)==1)
      {
         botonBajar = 1;
      }
      //Boton enter
      if(input(enter)==1)
      {
         botonEnter = 1;
      }
      //Boton salir
      if(input(esc)==1)
      {
         botonEsc = 1;
      }
}
//Funcion principal
void main() 
{
   setup_oscillator(OSC_16MHZ);
   //Configuracion ADC
   setup_adc(ADC_CLOCK_DIV_32);
   setup_adc_ports(AN0_TO_AN1);
   //Interrupciones.
   enable_interrupts(INT_RB);
   enable_interrupts(GLOBAL);
   set_tris_a(0x03);
   set_tris_b(0xf0);
   set_tris_c(0xff);
   set_tris_d(0x00); 
   lcd_init();      //Inicializamos LCD   
   while(1)
   {
      inicioDht11();
      if(respuesta())
      {
         comunicacionLCD();
         //comunicacionSerial();
         menusAll();
         alarmas();
         botones();
         funcionPI();
      }
      
   }
}
      
