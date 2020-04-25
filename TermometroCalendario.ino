//
// Termometro Calendário
//

#include <Wire.h>

// Iniciação
void setup() {
  Serial.begin(115200);
  Wire.begin();
  Display_init();
  sensor_init();
}

// Loop Principal
void loop() {
  char buf[9];      // dd/mm/aa e hh:mm
  char bufTemp[5];  // xx.x
  char bufUmid[5];  // xxx%
  
  while (Serial.available()) {
    trataAcerto();
  }

  // Mostra data e hora no topo do display
  rtc_leData(buf);
  Display_print(0, 0, buf);
  rtc_leHora(buf);
  Display_print(0, 11, buf);

  // Le temperatura e umidade
  sensor_le(bufTemp, bufUmid);
  Display_print_16x24 (0, 0, bufTemp);
  Display_print(3, 12, bufUmid);

  // Dá um tempo entre as atualizações
  delay(500);
}

// Tratamento do acerto do relogio
void trataAcerto() {
  static byte bufHora[12]; // ddmmaahhmmss
  static int iBuf = -1;
  
  int c = Serial.read();

  if (c == '<') {
    // inicio do acerto
    iBuf = 0;
  } else if (c == '>') {
    if (iBuf < 12) {
      // erro
      Serial.print("?");
    } else {
      // recebeu tudo, acertar a hora
      rtc_acerta(bufHora);      
      Serial.println("Hora acertada!");
    }
    iBuf = -1;
  } else if ((iBuf >= 0) && (iBuf < 12) && (c >= '0') && (c <= '9')) {
    bufHora[iBuf++] = (byte) c;
  }
}
