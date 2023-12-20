#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

const char alfabeto[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                         'j', 'k', 'l', 'm', 'n', 'ñ', 'o', 'p', 'q', 'r', 's', 't', 'u',
                         'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
                         '8', '9', '.', ',', '?', '"', '!'};

String codigo[] = {".-", "-...", "-.-.", "-..", ".", "..-.",
                   "--.", "....", "..", ".---", "-.-", ".-..",
                   "--", "-.", "--.--", "---", ".--.", "--.-",
                   ".-.", "...", "-", "..--", "...-", ".--",
                   "-..-", "-.--", "--..", "-----", ".----",
                   "..--", "...--", "....-", ".....", "-....",
                   "--...", "---..", "---..", "----.", ".-.-.-",
                   "--..--", "..--..", ".-..-.", "-.-.--"};

String error = "../../..";
String palabra = "";
String traduccion = "";
char espaciovacio = '/';

int buzzer = D5;
int ledRaya = D1;
int ledPunto = D2;
int ledEspacio = D3;
int ledFin = D4;

int tiempoPunto = 90;
int tiempoRaya = tiempoPunto * 3;
int silencioLetra = tiempoPunto * 3;
int silencioPalabra = tiempoPunto * 5;

boolean enviado = false;
boolean mensajeEnviado = false;

const char* ssid = "LIB-8598266";
const char* password = "4uacCyqrPnau";
unsigned long channelID = 2385410;
const char * readAPIKey = "5975KJ3O878QRKZN";
const int fieldNumber = 1; // Campo del canal de ThingSpeak para la palabra

void emiteTono(int tiempoEmision, int pin, int frecuencia) {
  tone(buzzer, frecuencia);
  digitalWrite(pin, HIGH);
  delay(tiempoEmision);
  digitalWrite(pin, LOW);
  noTone(buzzer);
}

void emitePunto() {
  delay(silencioLetra);
  emiteTono(tiempoPunto, ledPunto, 1500);
}

void emiteLinea() {
  delay(silencioLetra);
  emiteTono(tiempoRaya, ledRaya, 500);
}

void emiteEspacio() {
  digitalWrite(ledEspacio, HIGH);
  delay(silencioPalabra);
  digitalWrite(ledEspacio, LOW);
}

void emitefinMensaje() {
  digitalWrite(ledFin, HIGH);
  delay(500);
  digitalWrite(ledFin, LOW);
}

void traducirPalabraCompleta(String palabraCompleta) {
  for (int i = 0; i < palabraCompleta.length(); i++) {
    char caracter = palabraCompleta.charAt(i);
    if (caracter == ' ') {
      // Espacio entre palabras
      emiteEspacio();
    } else {
      // Traducción del carácter actual a código Morse
      for (int j = 0; j < sizeof(alfabeto); j++) {
        if (caracter == alfabeto[j]) {
          // Se encontró el carácter, emite la secuencia Morse correspondiente
          for (int k = 0; k < codigo[j].length(); k++) {
            if (codigo[j][k] == '.') {
              emitePunto();
            } else if (codigo[j][k] == '-') {
              emiteLinea();
            }
          }
          // Agrega un espacio entre caracteres
          delay(silencioLetra);
          break;
        }
      }
    }
  }
}

void traducirPalabraANumeros() {
    if (palabra == "ayuda") {
    traducirPalabraCompleta("ayuda");
  } else if (palabra == "salvacion") {
    traducirPalabraCompleta("salvacion");
  } else if (palabra == "urgencia") {
    traducirPalabraCompleta("urgencia");
  } else if (palabra == "emergencia") {
    traducirPalabraCompleta("emergencia");
  } else if (palabra == "salir") {
    traducirPalabraCompleta("salir");
  } else if (palabra == "alerta") {
    traducirPalabraCompleta("alerta");
  } else if (palabra == "sos") {
    traducirPalabraCompleta("sos");
  } else if (palabra == "necesito") {
    traducirPalabraCompleta("necesito");
  } else if (palabra == "rescate") {
    traducirPalabraCompleta("rescate");
  }
}


void traducirNumerosAPalabra() {
  palabra = "";
  for (int i = 0; i < traduccion.length(); i++) {
    char token = traduccion.charAt(i);
    if (token == ' ') {
      palabra += ' ';
    } else {
      // Convertir el número recibido en una palabra específica
      if (traduccion.substring(i, i + 1) == "1") {
        palabra += "ayuda";
      } else if (traduccion.substring(i, i + 1) == "2") {
        palabra += "salvacion";
      } else if (traduccion.substring(i, i + 1) == "3") {
        palabra += "urgencia";
      } else if (traduccion.substring(i, i + 1) == "4") {
        palabra += "emergencia";
      } else if (traduccion.substring(i, i + 1) == "5") {
        palabra += "salir";
      } else if (traduccion.substring(i, i + 1) == "6") {
        palabra += "alerta";
      } else if (traduccion.substring(i, i + 1) == "7") {
        palabra += "sos";
      } else if (traduccion.substring(i, i + 1) == "8") {
        palabra += "necesito";
      } else if (traduccion.substring(i, i + 1) == "9") {
        palabra += "rescate";
      }
      i++; // Saltar el siguiente espacio
    }
  }
}

void emiteMorse() {
  for (int i = 0; i < traduccion.length(); i++) {
    if (traduccion.charAt(i) == '.')
      emitePunto();
    else if (traduccion.charAt(i) == '-')
      emiteLinea();
    else if (traduccion.charAt(i) == espaciovacio)
      emiteEspacio();
  }
}

void obtenerPalabraDesdeThingSpeak() {
  WiFiClient client;
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando...");
  }
  Serial.println("Conexión exitosa!");

  ThingSpeak.begin(client);

  int palabraRecibida = ThingSpeak.readLongField(channelID, fieldNumber, readAPIKey);

  Serial.print("Valor recibido desde ThingSpeak: ");
  Serial.println(palabraRecibida);

  if (palabraRecibida > 0) {
    // Convertir el número recibido en una cadena
    traduccion = String(palabraRecibida);
    Serial.print("Valor convertido a String: ");
    Serial.println(traduccion);
  } else {
    Serial.println("Error al obtener la palabra desde ThingSpeak.");
  }

  WiFi.disconnect();
}

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(ledRaya, OUTPUT);
  pinMode(ledPunto, OUTPUT);
  pinMode(ledEspacio, OUTPUT);
  pinMode(ledFin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (!mensajeEnviado) {
    obtenerPalabraDesdeThingSpeak();
    traducirNumerosAPalabra();

    if (!enviado) {
      Serial.print("Palabra recibida: ");
      Serial.println(palabra);
      enviado = true;
    }

    traducirPalabraANumeros();
    emitefinMensaje();
    mensajeEnviado = true;
  }
}
