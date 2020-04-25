//
// Rotinas simples para escrita em Display OLED
//

// Endereço I2C do display
#define DISP_ADDR  0x3C

// Comandos
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Tamanho da tela
#define SSD1306_LCDWIDTH                  128
#define SSD1306_LCDHEIGHT                 64

// Sequência de iniciação do display
byte cmdInit[] =
{
    SSD1306_DISPLAYOFF,
    SSD1306_SETDISPLAYCLOCKDIV, 0x80,
    SSD1306_SETMULTIPLEX, 0x3F,
    SSD1306_SETDISPLAYOFFSET, 0x00,
    SSD1306_SETSTARTLINE | 0x0,
    SSD1306_CHARGEPUMP, 0x14,
    SSD1306_MEMORYMODE, 0x00,
    SSD1306_SEGREMAP | 0x1,
    SSD1306_COMSCANDEC,
    SSD1306_SETCOMPINS, 0x12,
    SSD1306_SETCONTRAST, 0xCF,
    SSD1306_SETPRECHARGE, 0xF1,
    SSD1306_SETVCOMDETECT, 0x40,
    SSD1306_DISPLAYALLON_RESUME,
    SSD1306_NORMALDISPLAY,
    SSD1306_DISPLAYON
};

// Iniciação do display
void Display_init()
{
  Display_sendcmd (cmdInit, sizeof(cmdInit));
  Display_clear();
}

// Limpa o display
void Display_clear()
{
  // Define endereços iniciais e finais de colunas e páginas
  Display_sendcmd (SSD1306_COLUMNADDR);
  Display_sendcmd (0);
  Display_sendcmd (SSD1306_LCDWIDTH-1);
  Display_sendcmd (SSD1306_PAGEADDR);
  Display_sendcmd (0);
  Display_sendcmd (7);

  // Preenche a memória com zeros
  for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) 
  {
      Wire.beginTransmission(DISP_ADDR);
      Wire.write(0x40);  // Co=0, DC = 1
      for (uint8_t x=0; x<16; x++) 
      {
          Wire.write(0);
      }
      Wire.endTransmission();
  }
}

// Desenho dos números de 0 a 9 / % : espaço
byte gc[][7] =
{
 {0x3E, 0x61, 0x51, 0x49, 0x45, 0x43, 0x3E} // 0
,{0x40, 0x40, 0x42, 0x7F, 0x40, 0x40, 0x40} // 1
,{0x42, 0x61, 0x51, 0x49, 0x45, 0x42, 0x40} // 2
,{0x22, 0x41, 0x49, 0x49, 0x49, 0x49, 0x36} // 3
,{0x10, 0x18, 0x14, 0x12, 0x11, 0x7F, 0x50} // 4
,{0x27, 0x45, 0x45, 0x45, 0x45, 0x49, 0x31} // 5
,{0x3C, 0x4A, 0x49, 0x49, 0x49, 0x49, 0x32} // 6
,{0x41, 0x21, 0x11, 0x09, 0x05, 0x03, 0x01} // 7
,{0x36, 0x49, 0x49, 0x49, 0x49, 0x49, 0x36} // 8
,{0x26, 0x49, 0x49, 0x49, 0x49, 0x29, 0x1E} // 9
,{0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01} // /
,{0x43, 0x23, 0x10, 0x08, 0x04, 0x62, 0x61} // %
,{0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00} // :
,{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} // espaco
};

// Escreve um string a partir da linha l(0 a 7), coluna c(0 a 16)
// String só deve conter dígitos / % : espaço
// Não pode ter mudança de linha
void Display_print (byte l, byte c, char *str) {

  // Endereça o  primeiro caracter
  Display_sendcmd (SSD1306_COLUMNADDR);
  Display_sendcmd (c*8);
  Display_sendcmd ((c+strlen(str))*8 - 1);
  Display_sendcmd (SSD1306_PAGEADDR);
  Display_sendcmd (l);
  Display_sendcmd (l);
  Wire.beginTransmission(DISP_ADDR);
  Wire.write(0x40);  // Co=0, DC = 1

  while (*str != 0) {
    byte car = *str;
    if ((car >= '0') && (car <= '9')) {
      car = car - '0';  // digito
    } else if (car == '/') {
      car = 10;
    } else if (car == '%') {
      car = 11;
    } else if (car == ':') {
      car = 12;
    } else {
      car = 13;
    }
    byte *pc = gc[car];
    for (uint8_t x = 0;  x < 7; x++) 
    {
      Wire.write(*pc++);
    }
    str++;
  }
  Wire.endTransmission();
}

// Escreve um caracter na linha l(0 a 7), coluna c(0 a 16)
void Display_write (byte l, byte c, byte car)
{
  byte *pc = gc[car];

  // Endereça o caracter
  Display_sendcmd (SSD1306_COLUMNADDR);
  Display_sendcmd (c*8);
  Display_sendcmd (c*8 + 7);
  Display_sendcmd (SSD1306_PAGEADDR);
  Display_sendcmd (l);
  Display_sendcmd (l);
  
  // Escreve
  Wire.beginTransmission(DISP_ADDR);
  Wire.write(0x40);  // Co=0, DC = 1
  for (uint8_t x = 0;  x < 7; x++) 
  {
    Wire.write(*pc++);
  }
  Wire.endTransmission();
}

// Desenho dos números de 0 a 9, matriz 16x24
byte gc16x24[][39] = 
{
  // 0
  { 
    0x03, 0x07, 0x0F, 0x1E, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1E, 0x0F, 0x07, 0x07,
    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
    0xF8, 0xFC, 0xFE, 0x0F, 0x07, 0x07, 0x07, 0x07, 0x07, 0x0F, 0xFE, 0xFC, 0xF8
  },
  // 1
  { 
    0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x1F, 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x18, 0x1C, 0x1E, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00
  },
  // 2
  { 
    0x1E, 0x1F, 0x1F, 0x1F, 0x1D, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
    0x00, 0x00, 0x80, 0xC0, 0xC0, 0xF0, 0x78, 0x3C, 0x1F, 0x0F, 0x07, 0x03, 0x01,
    0x18, 0x1C, 0x1E, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x8F, 0xFE, 0xFC, 0xF8
  },
  // 3
  { 
    0x03, 0x07, 0x0F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x0F, 0x07, 0x03,
    0x00, 0x00, 0x00, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x1F, 0xFF, 0xFF, 0xE0,
    0x18, 0x1C, 0x1E, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xFE, 0xFC, 0xF8
  },
  // 4
  { 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F,
    0x0F, 0x0F, 0x0F, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0xFF, 0xFF, 0xFF,
    0x00, 0x80, 0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x07, 0x03, 0xFF, 0xFF, 0xFF
  },
  // 5
  { 
    0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1E, 0x0F, 0x0F, 0x03,
    0x0F, 0x0F, 0x0F, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x1E, 0xFC, 0xFC, 0xF0,
    0xFF, 0xFF, 0xFF, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07
  },
  // 6
  { 
    0x03, 0x0F, 0x0F, 0x1E, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1E, 0x0F, 0x0F, 0x03,
    0xFF, 0xFF, 0xFF, 0x1C, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x1E, 0xFC, 0xFC, 0xF0,
    0xF8, 0xFE, 0xFE, 0x0F, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x0F, 0x0E, 0x0C
  },
  // 7
  { 
    0x1E, 0x1F, 0x1F, 0x1F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0xC0, 0xC0, 0xF0, 0x78, 0x3C, 0x1F, 0x0F, 0x07, 0x03, 0x01,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x87, 0xFF, 0xFF, 0xFF
  },
  // 8
  { 
    0x03, 0x07, 0x0F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x0F, 0x07, 0x03,
    0xE0, 0xFF, 0xFF, 0x0F, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x1F, 0xFF, 0xFF, 0xE0,
    0xF8, 0xFC, 0xFE, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xFE, 0xFC, 0xF8
  },
  // 9
  { 
    0x03, 0x07, 0x0F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x0F, 0x07, 0x03,
    0x00, 0x07, 0x0F, 0x0F, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x1F, 0xFF, 0xFF, 0xFF,
    0xF8, 0xFC, 0xFE, 0x0F, 0x07, 0x07, 0x07, 0x07, 0x07, 0x0F, 0xFE, 0xFC, 0xF8
  },
  // ponto
  { 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  }
};

// Escreve um string na linha l(0 a 1), coluna c(0 a 7) com matriz 16x24
void Display_print_16x24 (byte l, byte c, char *str) {
  while (*str) {
    byte car = *str;
    if ((car >= '0') && (car <= '9')) {
      car = car - '0';
    } else {
      car = 10;
    }
    Display_write_16x24(l, c, car);
    str++;
    if (++c == 8) {
      l++;
    }
  }
}

// Escreve um caracter na linha l(0 a 1), coluna c(0 a 7) com matriz 16x24
// Deixa uma faixa de 16 pontos no alto do display
void Display_write_16x24 (byte l, byte c, byte car)
{
  byte *pc = gc16x24[car];

  // 24 pontos = 3 faixas de 8 pontos
  for (int i = 2; i >= 0; i--)
  {
    // Endereça o caracter
    Display_sendcmd (SSD1306_COLUMNADDR);
    Display_sendcmd (c*16);
    Display_sendcmd (c*16 + 15);
    Display_sendcmd (SSD1306_PAGEADDR);
    Display_sendcmd (l*3+i+2);
    Display_sendcmd (l*3+i+2);
  
    // Escreve
    Wire.beginTransmission(DISP_ADDR);
    Wire.write(0x40);  // Co=0, DC = 1
    for (uint8_t x = 0;  x < 13; x++) 
    {
      Wire.write(*pc++);
    }
    Wire.endTransmission();
  }
}

// Envia sequência de comandos ao display
void Display_sendcmd (byte *cmd, int nCmds)
{
  Wire.beginTransmission(DISP_ADDR);
  Wire.write (0);  // Co= 0. DC = 0
  for (int i = 0; i < nCmds; i++)
  {
    Wire.write(cmd[i]);
  }
  Wire.endTransmission();
}

// Envia um byte de comando ao display
void Display_sendcmd (byte cmd)
{
  Wire.beginTransmission(DISP_ADDR);
  Wire.write (0);  // Co= 0. DC = 0
  Wire.write(cmd);
  Wire.endTransmission();
}