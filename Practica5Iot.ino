// Se definen los datos del proyecto en Blynk (ID, nombre y token de autenticación)
#define BLYNK_TEMPLATE_ID "TMPL2bDABVplu"
#define BLYNK_TEMPLATE_NAME "Practica5"
#define BLYNK_AUTH_TOKEN "KJHVAAsm0L35E6YxiPLvJNDa_u8xmJlO"

// Se incluyen las librerías necesarias para la conexión WiFi y el uso de Blynk con el ESP32
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Se definen las credenciales de la red WiFi a la cual se conectará el ESP32
char auth[] = BLYNK_AUTH_TOKEN;   // Token de autenticación de Blynk
char ssid[] = "iPhone de Shadows"; // Nombre de la red WiFi
char pass[] = "123456789";         // Contraseña de la red WiFi

#define PIR_PIN 14      // Pin donde se conecta la señal del sensor de movimiento PIR
#define LED_PIN 27      // Pin donde se conecta el LED físico
#define BUZZER_PIN 26   // Pin donde se conecta el buzzer


// Estos pines corresponden a los controles dentro de la aplicación Blynk
#define VIRTUAL_PIR V0      // LED virtual en la app que indica detección de movimiento
#define VIRTUAL_LED V1      // Switch virtual para controlar el LED físico
#define VIRTUAL_BUZZER V2   // Switch virtual para controlar el buzzer

bool ledActivo = false;          // Indica si el LED está activado desde la app
bool buzzerActivo = false;       // Indica si el buzzer está activado desde la app
bool movimientoDetectado = false; // Indica si se ha detectado movimiento recientemente

// Función que lee el estado del switch del LED en la app 
BLYNK_WRITE(VIRTUAL_LED) {
  ledActivo = param.asInt(); // Guarda el valor del switch (1 = encendido, 0 = apagado)
  Serial.println(ledActivo ? "LED activado desde app" : "LED desactivado desde app");
}

// Función que lee el estado del switch del buzzer en la app
BLYNK_WRITE(VIRTUAL_BUZZER) {
  buzzerActivo = param.asInt(); // Guarda el valor del switch (1 = encendido, 0 = apagado)
  Serial.println(buzzerActivo ? "Buzzer activado desde app" : "Buzzer desactivado desde app");
}

// Configuración inicial del programa 
void setup() {
  Serial.begin(115200); // Inicializa la comunicación serial para monitoreo

  // Configura los pines como entrada o salida según el dispositivo
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Asegura que al iniciar, el LED y el buzzer estén apagados
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Conexión a la red WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) { // Espera hasta que se conecte
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado a WiFi");
  Blynk.begin(auth, ssid, pass); // Conexión con el servidor Blynk
  Serial.println("Conectado a Blynk");

  // Sincroniza los estados iniciales de los switches con los valores actuales en la app
  Blynk.syncVirtual(VIRTUAL_LED);
  Blynk.syncVirtual(VIRTUAL_BUZZER);
}

// Bucle principal 
void loop() {
  Blynk.run(); // Mantiene la conexión y comunicación con Blynk activa

  // Lectura del valor del sensor PIR
  int valorPIR = digitalRead(PIR_PIN);

  // Envía el estado del sensor a la app para actualizar el LED virtual
  Blynk.virtualWrite(VIRTUAL_PIR, valorPIR);

  // Si el sensor detecta movimiento
  if (valorPIR == HIGH) {
    // Si antes no se había detectado, muestra un mensaje en el monitor
    if (!movimientoDetectado) {
      movimientoDetectado = true;
      Serial.println("Movimiento detectado!");
    }

    // Si el LED está activado desde la app, se enciende
    if (ledActivo) digitalWrite(LED_PIN, HIGH);
    // Si el buzzer está activado desde la app, suena
    if (buzzerActivo) digitalWrite(BUZZER_PIN, HIGH);

  } else {
    // Si no hay movimiento y antes sí lo había, se notifica
    if (movimientoDetectado) {
      movimientoDetectado = false;
      Serial.println("Sin movimiento...");
    }

    // Apaga el LED y el buzzer cuando no se detecta movimiento
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(1000); // Retardo de un segundo antes de repetir el ciclo
}

