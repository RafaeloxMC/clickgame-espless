#include "MFS.h"
#include "mbed.h"
#include "pitches.h"
#include "platform/mbed_thread.h"
#include <cstdio>
#include <cstring>

#define DIGIT_AN 2
#define DURATION 5

MFS display;
char seg7[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
int digitAnzeige[4] = {0x1000, 0x2000, 0x4000, 0x8000};

int digitPos = 0;
int einer = 0;
int zehner = 0;
int hunderter = 0;
int tausender = 1;
bool isNegative = false;

int setzeEiner = digitAnzeige[digitPos] + seg7[einer];
int setzeZehner = digitAnzeige[digitPos + 1] + seg7[zehner];
int setzeHunderter =
    digitAnzeige[digitPos + 2] + seg7[hunderter] + 0x80;
int setzeTausender = digitAnzeige[digitPos + 3] + seg7[tausender];

DigitalIn Button1(A1); // START BUTTON
DigitalIn Button2(A2); // STOP BUTTON
DigitalIn Button3(A3); // RESET BUTTON

DigitalOut Piezo(PB_3);
DigitalOut StatusLed(LED1);

bool running = false;

void playTone(int frequency, int duration)
{
   int period = 1000000 / frequency;
   int halfPeriod = period / 2;
   int cycles = (duration * 1000) / period;

   for (int i = 0; i < cycles; i++)
   {
      Piezo = 1;
      wait_us(halfPeriod);
      Piezo = 0;
      wait_us(halfPeriod);
   }
}

void playWinningMelody()
{
   int melody[][2] = {{NOTE_E5, 80}, {NOTE_E5, 80}, {REST, 80}, {NOTE_E5, 80}, {REST, 80}, {NOTE_C5, 80}, {NOTE_E5, 80}, {REST, 80}, {NOTE_G5, 80}, {REST, 400}, {NOTE_G4, 80}, {REST, 40}};

   for (unsigned int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++)
   {
      playTone(melody[i][0], melody[i][1]);
      HAL_Delay(melody[i][1]);
   }
}

void displayAusgabe(void)
{
   if (isNegative)
   {
      display = digitAnzeige[digitPos + 3] +
                0x40;
   }
   else
   {
      display = setzeTausender;
   }
   HAL_Delay(DIGIT_AN);
   display = setzeHunderter;
   HAL_Delay(DIGIT_AN);
   display = setzeZehner;
   HAL_Delay(DIGIT_AN);
   display = setzeEiner;
   HAL_Delay(DIGIT_AN);
}

void isrTIM6(void)
{
   displayAusgabe();
   HAL_NVIC_ClearPendingIRQ(TIM6_IRQn);
   TIM6->SR = 0;
}

void init(void)
{
   RCC->APB1ENR |= 0b10000;
   TIM6->PSC = 31999;
   TIM6->ARR = 10;
   TIM6->CNT = 0;
   TIM6->SR = 0;
   TIM6->CR1 = 1;
   NVIC_SetVector(TIM6_IRQn, (uint32_t)&isrTIM6);
   HAL_NVIC_EnableIRQ(TIM6_IRQn);
   TIM6->DIER = 1;

   playTone(1000, 200);
   HAL_Delay(100);
   playTone(1200, 200);
}

void reset(void)
{
   running = false;
   einer = 0;
   zehner = 0;
   hunderter = 0;
   tausender = 1;
   isNegative = false;
   setzeEiner = digitAnzeige[digitPos] + seg7[einer];
   setzeZehner = digitAnzeige[digitPos + 1] + seg7[zehner];
   setzeHunderter =
       digitAnzeige[digitPos + 2] + seg7[hunderter] + 0x80;
   setzeTausender =
       digitAnzeige[digitPos + 3] + (isNegative ? 0x40 : seg7[tausender]);
}

void winningAnimation()
{
   for (int i = 0; i <= 5; i++)
   {
      HAL_NVIC_DisableIRQ(TIM6_IRQn);

      display = 0x0000;
      HAL_Delay(200);

      HAL_NVIC_EnableIRQ(TIM6_IRQn);

      HAL_Delay(200);
   }
}

void stop(void)
{
   if (!running)
      return;
   running = false;

   StatusLed = PullDown;

   if ((!isNegative && tausender == 0 && hunderter == 0 && zehner <= 2))
   {
      playWinningMelody();
      winningAnimation();
   }
   else
   {
      playTone(1000, 200);
      HAL_Delay(100);
      playTone(800, 200);
   }
}

int main()
{
   init();
   StatusLed = PullDown;

   while (true)
   {
      if (Button1 == false)
      {
         if (!(isNegative && hunderter == 9 && zehner == 9 && einer == 9) &&
             !running)
         {
            playTone(2000, 10);
            running = true;
         }
      }

      if (Button2 == false)
      {
         stop();
      }

      if (Button3 == false)
      {
         if (!(!isNegative && tausender == 1 && hunderter == 0 && zehner == 0 &&
               einer == 0))
         {
            reset();
            playTone(2000, 10);
         }
      }

      if (!running)
         continue;

      HAL_Delay(DURATION);

      if (!isNegative)
      {
         einer--;
         if (einer < 0)
         {
            einer = 9;
            zehner--;
            if (zehner < 0)
            {
               zehner = 9;
               hunderter--;
               if (hunderter < 0)
               {
                  hunderter = 9;
                  tausender--;
                  if (tausender < 0)
                  {
                     isNegative = true;
                     einer = 1;
                     zehner = 0;
                     hunderter = 0;
                     tausender = 0;
                  }
               }
            }
         }
      }
      else
      {
         einer++;
         if (einer > 9)
         {
            einer = 0;
            zehner++;
            if (zehner > 9)
            {
               zehner = 0;
               hunderter++;
               if (hunderter > 9)
               {
                  hunderter = 0;
                  tausender++;
               }
            }
         }
      }

      if (isNegative && hunderter == 9 && zehner == 9 && einer == 9)
      {
         running = false;
      }

      setzeEiner = digitAnzeige[digitPos] + seg7[einer];
      setzeZehner = digitAnzeige[digitPos + 1] + seg7[zehner];
      setzeHunderter =
          digitAnzeige[digitPos + 2] + seg7[hunderter] + 0x80;
      setzeTausender =
          digitAnzeige[digitPos + 3] + (isNegative ? 0x40 : seg7[tausender]);
   }
}