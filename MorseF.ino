#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

const char alfabeto[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                         'j', 'k', '1', 'm', 'n', 'ñ', 'o', 'p', 'q', 'r', 's', 't', 'u',
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

const char* ssid = "Hola";
const char* password = "99999999";
unsigned long channelID = 2379511;
const char* readAPIKey = "LQ7EEXLTA4STPIBQ";
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

void traducirPalabra() {
  palabra = "";
  if (Serial.available()) {
    palabra = Serial.readString();
    traduccion = "La traduccion a codigo morse de su mensaje es: ";
    enviado = false;
    boolean encontrado = false;

    if (palabra == "error")
      traduccion = error;
    else {
      for (int i = 0; i < palabra.length(); i++) {
        for (int j = 0; j < sizeof(alfabeto) && !encontrado; j++) {
          if (palabra.charAt(i) == alfabeto[j]) {
            encontrado = true;
            traduccion += codigo[j] + " ";
          } else if (palabra.charAt(i) == ' ') {
            encontrado = true;
            traduccion += espaciovacio;
          }
        }
        encontrado = false;
      }
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

  if (palabraRecibida > 0) {
    palabra = String(palabraRecibida);
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
    traducirPalabra();

    if (!enviado) {
      Serial.print(traduccion);
      enviado = true;
    }

    emiteMorse();
    emitefinMensaje();
    mensajeEnviado = true;

    // Reinicia el Arduino
    ESP.restart();
  }
}
