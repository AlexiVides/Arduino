#include <WiFi.h>                  // Biblioteca WiFi para ESP32
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Configuraciones de red WiFi
const char* ssid = "CLARO_247B0B";    // Tu SSID WiFi
const char* password = "355197EA32";  // Tu contraseña WiFi

// Configuración del bot de Telegram
const String BOT_TOKEN = "7529648410:AAH1zkywHLe5AJYeVvpBQM7hgGaOOvzbuRY";  // Token de tu bot de Telegram
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Pin donde está conectado el sensor de nivel de agua
int sensorPin = A0;  // Pin analógico A0 en ESP32 para lectura de sensor analógico
int nivelAgua;       // Variable para almacenar el nivel de agua

// Intervalo de chequeo de nuevos mensajes (cada 1 segundo)
unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  // Inicializamos la conexión serie para depuración
  Serial.begin(115200);

  // Conexión a la red WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conectado a la red WiFi");

  // Telegram SSL inicialización
  client.setInsecure();

  // Enviar mensaje de conexión al bot de Telegram
  bot.sendMessage("5215232511", "Conectado a la red WiFi", "");

  // Mostrar IP del dispositivo
  Serial.println(WiFi.localIP());
}

void loop() {
  // Chequear si hay nuevos mensajes del bot de Telegram
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
  }

  // Leer el nivel de agua desde el sensor
  leerNivelAgua();
}

// Función para leer el nivel de agua desde el sensor
void leerNivelAgua() {
  nivelAgua = analogRead(sensorPin);  // Lee el valor del sensor (0-4095 en ESP32)
  nivelAgua = map(nivelAgua, 0, 4095, 0, 100);  // Ajusta el valor a porcentaje (0-100%)
  Serial.println("Nivel de agua leído: " + String(nivelAgua));  // Imprime el nivel en el monitor serie
}

// Función para manejar los nuevos mensajes de Telegram
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    if (text == "/nivel") {
      leerNivelAgua();  // Asegurarse de que el valor esté actualizado
      String mensaje = "El nivel de agua actual es: " + String(nivelAgua) + "%";
      bot.sendMessage(chat_id, mensaje, "");
    } else {
      bot.sendMessage(chat_id, "Comando no reconocido. Usa /nivel para ver el nivel de agua.", "");
    }
  }
}
