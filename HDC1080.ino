//
// Teste do sensor de temperatura e humidade HDC1080
//

// Endereço I2C do sensor
#define SENSOR_ADDR  0x40

// Registradores do sensor
#define HDC1080_TEMP  0x00
#define HDC1080_UMID  0x01
#define HDC1080_CONF  0x02
#define HDC1080_ID0   0xFB
#define HDC1080_ID1   0xFC
#define HDC1080_ID2   0xFD
#define HDC1080_MFG   0xFE
#define HDC1080_DEV   0xFF

// Iniciação
void sensor_init() {
  
  // Espera terminar o reset
  while (sensor_read_reg(HDC1080_CONF) & 0x8000) {
    delay (100);
  }
  
  // Garante a configuração desejada
  sensor_write_reg(HDC1080_CONF, 0x1000);
}

// Informa a temperatura e umidade formatadas
void sensor_le (char *sTemp, char *sUmid) {
  uint16_t temp, umid;

  // Le os registradores
  sensor_read_tempumid(&temp, &umid);

  // Calcula e formata a temperatura em celsius
  uint32_t aux;
  aux = temp * 1650L;
  aux = (aux >> 16) - 400L;
  temp = (uint16_t) aux;
  sTemp[0] = (temp/100) + '0';
  sTemp[1] = ((temp /10) % 10) + '0';
  sTemp[2] = '.';
  sTemp[3] = (temp % 10) + '0';
  sTemp[4] = 0;

  // Calcula e formata a umidade em %
  aux = umid * 100L;
  aux = aux >> 16;
  umid = (uint16_t) aux;
  sUmid[0] = (umid > 99) ? (umid/100) + '0' : ' ';
  sUmid[1] = ((umid /10) % 10) + '0';
  sUmid[2] = (umid % 10) + '0';
  sUmid[3] = '%';
  sUmid[4] = 0;
}

// Dispara uma leitura de temperatura e umidade e
// pega o resultado.
void sensor_read_tempumid(uint16_t *pTemp, uint16_t *pUmid) {
  
  // Esta escrita dispara a leitura
  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write (HDC1080_TEMP);
  Wire.endTransmission();

  // Aguarda fazer a leitura
  delay(50);

  // Lê o resultado
  if (Wire.requestFrom(SENSOR_ADDR, 4) == 4) {
    *pTemp = Wire.read() << 8;
    *pTemp |= Wire.read();
    *pUmid = Wire.read() << 8;
    *pUmid |= Wire.read();
  } else {
    // algo deu errado
    *pTemp = *pUmid = 0;
  }
}

// Lê um registrador
// Não usar para ler temperatura e umidade!
uint16_t sensor_read_reg(uint8_t reg) {
  uint16_t val;
  
  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write (reg);
  Wire.endTransmission();

  if (Wire.requestFrom(SENSOR_ADDR, 2) == 2) {
    val = Wire.read() << 8;
    val |= Wire.read();
  } else {
    val = 0;  // algo deu errado
  }
  return val;
}

// Escreve em um registrador
void sensor_write_reg(uint8_t reg, uint16_t val) {
  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write (reg);
  Wire.write (val >> 8);
  Wire.write (val & 0xFF);
  Wire.endTransmission();
}
