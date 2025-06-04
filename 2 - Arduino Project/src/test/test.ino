/*******************************************************************

  Web radio simple à base d'ESP32 et VS1053

  Basé sur un sketch de Vince Gellár (github.com/vincegellar)

  Bibliotheque VS1053 de baldram (https://github.com/baldram/ESP_VS1053_Library)

  Plus d'infos:  https://electroniqueamateur.blogspot.com/2021/03/esp32-et-vs1053-ecouter-la-radio-sur.html


*********************************************************************/

#include <VS1053.h>
#include <WiFi.h>
#include <math.h> // Pour les calculs audio avancés
#include <WiFiManager.h>

// broches utilisées
#define VS1053_CS     32
#define VS1053_DCS    33
#define VS1053_DREQ   15

// nom et mot de passe de votre réseau:
const char *ssid = "Anonymous";
const char *password = "password";

#define BUFFSIZE 64  //32, 64 ou 128
uint8_t mp3buff[BUFFSIZE];

#define BASS_MIN 0
#define BASS_MAX 15
#define TREBLE_MIN 0
#define TREBLE_MAX 15
#define SPATIAL_LEVELS 4
int spatial = 0;

// Fréquences par défaut
#define DEFAULT_BASS_FREQ 15 // 150Hz
#define DEFAULT_TREBLE_FREQ 2 // 2KHz

int volume = 100;  // volume sonore 0 à 100


#define NOMBRECHAINES 7 // nombre de chaînes prédéfinies
int chaine = 0; //station actuellement sélectionnée

//caractéristiques de la station actuellement sélectionnée
char host[40];
char path[40];
int httpPort;

// Réglages audio
uint8_t bassAmp = 0;
uint8_t trebleAmp = 0;
uint8_t spatialMode = 0;
uint8_t toneSettings[4] = {0, DEFAULT_TREBLE_FREQ, 0, DEFAULT_BASS_FREQ};

VS1053 player(VS1053_CS, VS1053_DCS, VS1053_DREQ);
WiFiClient client;

bool con;

// connexion à une chaine
void connexionChaine () {

  switch (chaine) {
    case 0:
      strcpy(host, "stream03.ustream.ca");
      strcpy(path, "/cism128.mp3");
      httpPort = 8000;
      break;
      
    case 1:
      strcpy(host, "chisou-02.cdn.eurozet.pl");
      strcpy(path, "/;");
      httpPort = 8112;
      break;
      
    case 2:
      strcpy(host, "streamer01.sti.usherbrooke.ca");
      strcpy(path, "/cfak.mp3");
      httpPort = 8000;
      break;

    case 3:
      strcpy(host, "radios.rtbf.be");
      strcpy(path, "/wr-c21-metal-128.mp3");
      httpPort = 80;
      break;

    case 4:
      strcpy(host, "ecoutez.chyz.ca");
      strcpy(path, "/mp3");
      httpPort = 8000;
      break;

    case 5:
      strcpy(host, "ice4.somafm.com");
      strcpy(path, "/seventies-128-mp3");
      httpPort = 80;
      break;

    case 6:
      strcpy(host, "lyon1ere.ice.infomaniak.ch");
      strcpy(path, "/lyon1ere-high.mp3");
      httpPort = 80;
      break;
  }

  Serial.print("Connection a ");
  Serial.println(host);

  if (!client.connect(host, httpPort)) {
    Serial.println("Echec de la connexion");
    return;
  }

  Serial.print("Demande du stream: ");
  Serial.println(path);

  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
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
  Serial.begin(115200);

  Serial.println("\n\nRadio WiFi");
  Serial.println("");

  Serial.println("Controles: ");
  Serial.println("\t n: synthoniser une autre chaine");
  Serial.println("\t v: chaine précédente");
  Serial.println("\t y/+: augmenter volume");
  Serial.println("\t b/-: diminuer volume");
  Serial.println("\t g: basses +");
  Serial.println("\t f: basses -");
  Serial.println("\t j: aigus +");
  Serial.println("\t h: aigus -");
  Serial.println("\t d: tonalité par défaut");
  Serial.println("\t s: spatialisation");

  WiFi.mode(WIFI_STA);
  WiFiManager wm;

  wm.resetSettings();

  con = wm.autoConnect(ssid, password);

  if (!con) {
    Serial.println(" Échec connexion WiFi, reboot...");
  }
  else {
    //if you get here you have connected to the WiFi    
    Serial.println("connected...yeey :)");
  }

  Serial.println("\nWiFi connecte");
  Serial.println("Adresse IP: ");
  Serial.println(WiFi.localIP());

  SPI.begin();

  player.begin();
  player.switchToMp3Mode();
  player.setVolume(volume);
  player.setTone(toneSettings);  // appliquer les graves et aigus au démarrage

  connexionChaine();

}
void loop() {

  if (Serial.available()) {
    char c = Serial.read();

    // n: prochaine chaine
    if (c == 'n') {
      Serial.println("On change de chaine");
      client.stop();
      if (chaine < (NOMBRECHAINES - 1)) {
        chaine++;
      }
      else { // retour au début de la liste
        chaine = 0;
      }
      connexionChaine();
    }

    // Changer de chaîne (précédente)
    if (c == 'v') {
      Serial.println("Chaine précédente");
      client.stop();
      if (chaine > 0) {
        chaine--;
      } 
      else {
        chaine = NOMBRECHAINES - 1;
      }
      connexionChaine();
    }

    // Augmenter le volume
    if (c == 'y' || c == '+') {
      if (volume < 100) {
        Serial.println("Plus fort");
        volume++;
        player.setVolume(volume);
      }
    }

    // Diminuer le volume
    if (c == 'b' || c == '-') {
      if (volume > 0) {
        Serial.println("Moins fort");
        volume--;
        player.setVolume(volume);
      }
    }

    // Contrôle des basses
    if (c == 'g') {
      if (bassAmp < BASS_MAX) {
        bassAmp++;
        toneSettings[2] = bassAmp;
        player.setTone(toneSettings);
        Serial.print("Basses: ");
        Serial.println(bassAmp);
      }
    }
    if (c == 'f') {
      if (bassAmp > BASS_MIN) {
        bassAmp--;
        toneSettings[2] = bassAmp;
        player.setTone(toneSettings);
        Serial.print("Basses: ");
        Serial.println(bassAmp);
      }
    }

    // Contrôle des aigus
    if (c == 'j') {
      if (trebleAmp < TREBLE_MAX) {
        trebleAmp++;
        toneSettings[0] = trebleAmp;
        player.setTone(toneSettings);
        Serial.print("Aigus: ");
        Serial.println(trebleAmp);
      }
    }
    if (c == 'h') {
      if (trebleAmp > TREBLE_MIN) {
        trebleAmp--;
        toneSettings[0] = trebleAmp;
        player.setTone(toneSettings);
        Serial.print("Aigus: ");
        Serial.println(trebleAmp);
      }
    }

    // Tonalité par défaut
    if (c == 'd') {
        resetTone();
    }

    
  if (c == 's') {
        spatial = (spatial + 1) % 4;
        uint16_t value = 0x0800;
        switch (spatial) {
          case 0: value = 0x0800; break;
          case 1: value = 0x0820; break;
          case 2: value = 0x0840; break;
          case 3: value = 0x0880; break;
        }
        player.writeRegister(0x00, value);
        Serial.print("Spatialisation niveau ");
        Serial.println(spatial);
        player.setVolume(volume); // rétablit un niveau sonore correct

      }

  }

  if (client.available() > 0) {
    uint8_t bytesread = client.read(mp3buff, BUFFSIZE);
    if (bytesread) {
      player.playChunk(mp3buff, bytesread);
    }
  }
}
