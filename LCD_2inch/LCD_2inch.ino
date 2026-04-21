#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"

void setup()
{
  Config_Init();
  LCD_Init();
  LCD_Clear(0xffff);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, BLACK);
  Paint_Clear(BLACK);
  Paint_Clear(0x123456);
  Paint_DrawString_EN(0, 0, "123", &Font24, YELLOW, RED);
  Paint_DrawString_EN(0, 24, "ABC", &Font24, BLUE, CYAN);
  
  // Paint_DrawString_CN(50,180, "微雪电子",  &Font24CN, WHITE, RED);
  
  // Paint_DrawRectangle(125, 10, 225, 58, RED,  DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
  // Paint_DrawLine(125, 10, 225, 58, MAGENTA,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  // Paint_DrawLine(225, 10, 125, 58, MAGENTA,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  
  // Paint_DrawCircle(150,100, 25, BLUE,   DOT_PIXEL_2X2,   DRAW_FILL_EMPTY);
  // Paint_DrawCircle(180,100, 25, BLACK,  DOT_PIXEL_2X2,   DRAW_FILL_EMPTY);
  // Paint_DrawCircle(210,100, 25, RED,    DOT_PIXEL_2X2,   DRAW_FILL_EMPTY);
  // Paint_DrawCircle(165,125, 25, YELLOW, DOT_PIXEL_2X2,   DRAW_FILL_EMPTY);
  // Paint_DrawCircle(195,125, 25, GREEN,  DOT_PIXEL_2X2,   DRAW_FILL_EMPTY);

  // New Code
  Paint_DrawString_EN(0, 100, "1234567890", &Font24, GREEN, BLACK);
  delay(1000);
  Paint_DrawString_EN(0, 200, "Beatmaster+", &Font24, BLACK, WHITE);
  Paint_DrawString_EN(0, 205, "Beat", &Font24, BLACK, WHITE);
  Paint_DrawString_EN(0, 210, "asdf", &Font24, BLACK, WHITE);
  Paint_DrawString_EN(0, 215, "qwerty", &Font24, BLACK, WHITE);
  Paint_DrawString_EN(0, 215, "1234567890asdfghjkl", &Font24, BLACK, WHITE);
  Paint_Clear(BLUE);
  Paint_DrawString_EN(0, 0, "This is the fastest speed at which we can type.", &Font16, BLACK, WHITE);
  delay(1000);
  Paint_DrawString_EN(0, 32, "This is a bit slower.", &Font20, BLACK, WHITE);
  delay(1000);
  Paint_DrawString_EN(0, 52, "This is slowest.", &Font24, BLACK, WHITE);
  Paint_Clear(LGRAY);
  Paint_DrawString_EN(0, 0, "Welcome to BeatMaster+", &Font16, LGRAY, BLACK);
  Paint_DrawString_EN(0, 16, "Select Mode:", &Font16, LGRAY, BLACK);
  Paint_DrawString_EN(0, 32, "Rudiment Exercises", &Font16, LGRAY, BLACK);
  Paint_DrawString_EN(0, 48, "Freeplay", &Font16, LGRAY, BLACK);
  // At height of 220, the words stop writing properly :(
  // Paint_DrawString_EN(0, 220, "wheat", &Font24, BLACK, WHITE);
  // Brightness test
  // DEV_Set_PWM(50);  // 50% brightness
  // delay(1000);
  // DEV_Set_PWM(0);   // 0% brightness
  // delay(1000);
  // DEV_Set_PWM(1);   // 1% brightness
  // delay(1000);
  // DEV_Set_PWM(100); // 100% brightness

  Paint_DrawImage(gImage_70X70, 20, 80, 70, 70); 
  //Paint_DrawFloatNum (5, 150 ,987.654321,4,  &Font20,    WHITE,   LIGHTGREEN);

  Paint_DrawLine(200, 100, 100, 200, WHITE, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
  Paint_DrawLine(100, 200, 300, 200, WHITE, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
  Paint_DrawLine(300, 200, 200, 100, WHITE, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

}
void loop()
{
  // int xVal = (rand() % (320 + 1));
  // int yVal = (rand() % (240 + 1));
  // int colorVal = (rand() % (0xffffff + 1));
  // Paint_DrawPoint(xVal, yVal, colorVal, DOT_PIXEL_2X2, DOT_FILL_AROUND);

  
}



/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
