//
// Rotinas simples para uso do RTC DS1307
//

// Endereço I2C do DS1307
#define RTC_ADDR  0x68

// Endereço dos registradores
#define DS1307_SEG  0
#define DS1307_MIN  1
#define DS1307_HOR  2
#define DS1307_DIA  4
#define DS1307_MES  5
#define DS1307_ANO  6

// Le a data atual no formato "dd/mm/aa"
void rtc_leData (char *buf) {
  // Seleciona o primeiro registador
  Wire.beginTransmission(RTC_ADDR);
  Wire.write (DS1307_DIA);
  Wire.endTransmission();

  // Le os valores
  Wire.requestFrom(RTC_ADDR, 3);
  byte val;
  val = Wire.read();
  buf[0] = ((val & 0x30) >> 4) + '0';
  buf[1] = (val & 0x0F) + '0';
  buf[2] = '/';
  val = Wire.read();
  buf[3] = ((val & 0x10) >> 4) + '0';
  buf[4] = (val & 0x0F) + '0';
  buf[5] = '/';
  val = Wire.read();
  buf[6] = (val >> 4) + '0';
  buf[7] = (val & 0x0F) + '0';
  buf[8] = 0;
}

// Le a hora no formato "hh:mm"
void rtc_leHora (char *buf) {
  // Seleciona o primeiro registador
  Wire.beginTransmission(RTC_ADDR);
  Wire.write (DS1307_MIN);
  Wire.endTransmission();

  // Le os valores
  Wire.requestFrom(RTC_ADDR, 2);
  byte val;
  val = Wire.read();
  buf[3] = ((val & 0x70) >> 4) + '0';
  buf[4] = (val & 0x0F) + '0';
  buf[5] = 0;
  val = Wire.read();
  if ((val & 0x40) != 0) {
    // Quem configurou para formato 12h?
    if (val & 0x20) {
      val = (val & 0x1F) + 0x12;
    }
  }
  buf[0] = ((val & 0x30) >> 4) + '0';
  buf[1] = (val & 0x0F) + '0';
  buf[2] = ':';
}

// Acerta e data e hora
// dhAtual deve estar no formato ddmmaahhmmss
void rtc_acerta(byte *dhAtual) {
  byte reg[7];

  // Calcula o valor dos registradores
  reg[0] = ((dhAtual[10]-'0') << 4) + (dhAtual[11] - '0');
  reg[1] = ((dhAtual[8]-'0') << 4) + (dhAtual[9] - '0');
  reg[2] = ((dhAtual[6]-'0') << 4) + (dhAtual[7] - '0');
  reg[3] = 1; // ignora dia da semana
  reg[4] = ((dhAtual[0]-'0') << 4) + (dhAtual[1] - '0');
  reg[5] = ((dhAtual[2]-'0') << 4) + (dhAtual[3] - '0');
  reg[6] = ((dhAtual[4]-'0') << 4) + (dhAtual[5] - '0');

  // Programa o relógio
  Wire.beginTransmission(RTC_ADDR);
  Wire.write (DS1307_SEG);
  for (int i = 0; i < 7; i++) {
    Wire.write(reg[i]);
  }
  Wire.endTransmission();
}
