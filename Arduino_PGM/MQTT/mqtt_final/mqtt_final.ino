
#include <ESP32_VS1053_Stream.h>
#include <VS1053.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

#define VS1053_CS     32
#define VS1053_DCS    33
#define VS1053_DREQ   15


ESP32_VS1053_Stream radio;
const char* mqtt_server = "broker.mqtt-dashboard.com";
WiFiClient espClient;
PubSubClient client(espClient);

VS1053 player(VS1053_CS, VS1053_DCS, VS1053_DREQ);

const char* urls[] = {
  "http://cdn.nrjaudio.fm/audio1/fr/40046/aac_64.mp3 ",
  "http://icecast.skyrock.net/s/natio_mp3_128k?tvr_name=tunein16&tvr_section1=128mp3 ",
  "https://stream.funradio.fr/fun-1-48-128.mp3",
  "http://radios.rtbf.be/wr-c21-metal-128.mp3",
  "http://ecoutez.chyz.ca:8000/mp3",
  "http://ice4.somafm.com/seventies-128-mp3",
  "http://lyon1ere.ice.infomaniak.ch/lyon1ere-high.mp3"
};

const int NOMBRECHAINES = sizeof(urls) / sizeof(urls[0]);
int chaine = 0;
int volume = 85;  // 0 - 100
uint8_t bassAmp = 0;
uint8_t trebleAmp = 0;
uint8_t spatialMode = 0;

int modeSpatial = 0;
#define BASS_MIN 0
#define BASS_MAX 15
#define TREBLE_MIN 0
#define TREBLE_MAX 15
#define DEFAULT_BASS_FREQ 15 // 150Hz
#define DEFAULT_TREBLE_FREQ 2 // 2KHz

uint8_t toneSettings[4] = {0, DEFAULT_TREBLE_FREQ, 0, DEFAULT_BASS_FREQ};
String lastMQTTMessage = "";
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("MQTT reçu [");
  Serial.print(topic);
  Serial.print("] : ");
  Serial.println(message);
  if (String(topic) == "radioESP32Camille/commande") {
    lastMQTTMessage = message;
}
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connexion MQTT...");
    String clientId = "ESP32RadioCamille";
    if (client.connect(clientId.c_str())) {
      Serial.println("Connecté !");
      client.subscribe("radioESP32Camille/#"); 
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" -> attente 5s");
      delay(5000);
    }
  }
}

void setSpatialisationMode(int mode) {
  uint16_t value = player.read_register(0x00); // Lire la valeur du registre
  switch (mode) {
    case 0: 
    value = value & 0b1111111101101111;
     break; // OFF
    case 1: value = value | 0b0000000010000000;
          break; // LOW
    case 2: value | 0b0000000000010000 ;
          break; // MED
    case 3: value = value | 0b0000000010000000 | 0b0000000000010000;
          break; // HIGH
  }
  player.writeRegister(0x00, value); // SCI_MODE
  Serial.print("Spatialisation mode ");
  Serial.println(mode);
}

void resetTone() {
    bassAmp = 0;
    trebleAmp = 0;
    toneSettings[0] = trebleAmp;   // Treble Amplitude
    toneSettings[2] = bassAmp;     // Bass Amplitude
    player.setTone(toneSettings);
    Serial.println("Tonalité réinitialisée");
}
void setup() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.begin(115200);
  Serial.println("\n=== Radio WiFi ESP32 ===");
  player.setTone(toneSettings);
  Serial.println("Commandes : n/v = prochaine chaine, y/b = volume, g/f = basses, j/h = aigus, d = tonalité défaut, s = spatialisation cyclique");

  WiFiManager wm;
  if (!wm.autoConnect("ESP32-Camille", "12345678")) {
    Serial.println("Échec connexion WiFi, redémarrage...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("WiFi connecté !");
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());

  SPI.begin();

  // Initialise le player VS1053 pour le contrôle du son
  player.begin();
  player.switchToMp3Mode();
  player.setVolume(volume);
  setSpatialisationMode(modeSpatial);

  // Initialise le stream en passant les pins obligatoires
  radio.startDecoder(VS1053_CS, VS1053_DCS, VS1053_DREQ);

  // Connexion à la première chaîne avec affichage et gestion d'erreur
  Serial.print("Connexion à l'URL : ");
  Serial.println(urls[chaine]);

  if (!radio.connecttohost(urls[chaine])) {
    Serial.println("Erreur de connexion au flux !");
  } else {
    Serial.println("Connexion réussie !");
  }
}

void loop() {
    if (!client.connected()) {
    reconnect();
  }
  client.loop();

  radio.loop();

  if (Serial.available() || lastMQTTMessage != "") {
    char c = Serial.read();

    if (c == 'n' || lastMQTTMessage == "n") {
      chaine = (chaine + 1) % NOMBRECHAINES;
      Serial.println("Changement de chaine...");
      radio.stopSong();

      Serial.print("Connexion à l'URL : ");
      Serial.println(urls[chaine]);

      if (!radio.connecttohost(urls[chaine])) {
        Serial.println("Erreur de connexion au flux !");
      } else {
        Serial.println("Connexion réussie !");
      }
      lastMQTTMessage="";
    }

    if (c == 'v' || lastMQTTMessage == "v") {
      chaine = (chaine - 1) % NOMBRECHAINES;
      Serial.println("Changement de chaine...");
      radio.stopSong();

      Serial.print("Connexion à l'URL : ");
      Serial.println(urls[chaine]);

      if (!radio.connecttohost(urls[chaine])) {
        Serial.println("Erreur de connexion au flux !");
      } else {
        Serial.println("Connexion réussie !");
      }
      lastMQTTMessage="";
    }

    if (c == 'y' && volume < 100 || lastMQTTMessage == "y" && volume < 100) {
      volume++;
      player.setVolume(volume);
      Serial.print("Volume : ");
      Serial.println(volume);
      lastMQTTMessage="";
    }

    if (c == 'b' && volume > 0 || lastMQTTMessage == "b" && volume > 0 ) {
      volume--;
      player.setVolume(volume);
      Serial.print("Volume : ");
      Serial.println(volume);
      lastMQTTMessage="";
    }

    if (c == 'g' || lastMQTTMessage == "g") {
      if (bassAmp < BASS_MAX) {
        bassAmp++;
        toneSettings[2] = bassAmp;
        player.setTone(toneSettings);
        Serial.print("Basses: ");
        Serial.println(bassAmp);
        lastMQTTMessage="";
      }
    }
    if (c == 'f' || lastMQTTMessage == "f") {
      if (bassAmp > BASS_MIN) {
        bassAmp--;
        toneSettings[2] = bassAmp;
        player.setTone(toneSettings);
        Serial.print("Basses: ");
        Serial.println(bassAmp);
        lastMQTTMessage="";
      }
    }

    // Contrôle des aigus
    if (c == 'j' || lastMQTTMessage == "j") {
      if (trebleAmp < TREBLE_MAX) {
        trebleAmp++;
        toneSettings[0] = trebleAmp;
        player.setTone(toneSettings);
        Serial.print("Aigus: ");
        Serial.println(trebleAmp);
        lastMQTTMessage="";
      }
    }
    if (c == 'h' || lastMQTTMessage == "h") {
      if (trebleAmp > TREBLE_MIN) {
        trebleAmp--;
        toneSettings[0] = trebleAmp;
        player.setTone(toneSettings);
        Serial.print("Aigus: ");
        Serial.println(trebleAmp);
        lastMQTTMessage="";
      }
    }

    if (c == 'd' || lastMQTTMessage == "d") {
      resetTone();
      player.setVolume(85);
      Serial.println("tonalité et volume par défaut");
      lastMQTTMessage="";
    }

    if (c == 's' || lastMQTTMessage == "s") {
      modeSpatial = (modeSpatial + 1) % 4;
      setSpatialisationMode(modeSpatial);
      lastMQTTMessage="";
    }
  }
}




