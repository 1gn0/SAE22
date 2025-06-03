# WebRadio ESP32 + VS1053

**Lecteur de radio Internet portable**  
**Matériel** : ESP32 Feather + VS1053 (Adafruit Music Maker FeatherWing optionnel)  
**Logiciel** : Arduino IDE, bibliothèques ESP_VS1053_Library (Baldram), ESP32_VS1053_Stream (Cellie), 
PubSubClient (MQTT), WiFiManager, PubSubClient.

---

## Table des matières

1. [Présentation du projet](#présentation-du-projet)  
2. [Matériel requis](#matériel-requis)  
3. [Installation et configuration](#installation-et-configuration)  
   1. [Arduino IDE](#arduino-ide)  
   2. [Déposer les bibliothèques](#déposer-les-bibliothèques)  
   3. [Organisation du dossier](#organisation-du-dossier)  
4. [Branchement matériel (schéma)](#branchement-matériel-schéma)  
5. [Configuration Wi-Fi (WiFiManager)](#configuration-wi-fi-wifimanager)  
6. [Liste des stations de radio](#liste-des-stations-de-radio)  
7. [Commandes clavier série](#commandes-clavier-série)  
8. [Pilotage via MQTT](#pilotage-via-mqtt)  
   1. [Topics et payloads](#topics-et-payloads)  
   2. [IoT MQTT Panel (Android)](#iot-mqtt-panel-android)  
9. [Fonctionnalités audio (VS1053)](#fonctionnalités-audio-vs1053)  
   1. [Volume](#volume)  
   2. [Tonalité (basses/aigus)](#tonalité-bassesaigus)  
   3. [Spatialisation (EarSpeaker)](#spatialisation-earspeaker)  
10. [Utilisation avancée](#utilisation-avancée)  
    1. [Égaliseur 5 bandes (EQ5 Plugin)](#égaliseur-5-bandes-eq5-plugin)  
    2. [Lecture depuis carte SD](#lecture-depuis-carte-sd)  
11. [Licence](#licence)  

---

## Présentation du projet

Ce projet propose de réaliser un **lecteur de radio Internet portable**, basé sur un **ESP32 Feather** (microcontrôleur Wi-Fi) et le **VS1053b** (codec audio).  
L’utilisateur peut :
- Sélectionner et écouter une station de radio Internet (flux MP3 ou AAC).  
- Contrôler **volume**, **basses**, **aigus**, et **effet de spatialisation** (EarSpeaker).  
- Changer dynamiquement de station en saisissant l’URL ou en choisissant parmi une liste prédéfinie.  
- Piloter l’ensemble via le **clavier série** ou à distance via **MQTT** (application « IoT MQTT Panel »).

Les fonctionnalités avancées (étape finale) permettent d’interagir totalement depuis un smartphone, sans brancher l’ESP32 au PC, grâce à la configuration **WiFiManager** et à la communication **MQTT**.  

_Ce projet s’appuie sur les activités du module SAE2.02 WebRadio (IUT Colmar), notamment le cahier des charges dispo dans `docs/SAE202-Web_Radio.pdf` :contentReference[oaicite:17]{index=17} et le diaporama `docs/RadioInternet_ESP32_VS1053.pptx` :contentReference[oaicite:18]{index=18}._

---

## Matériel requis

- **ESP32 Feather (Adafruit HUZZAH32)**  
  - Produit ID : 3619 (Feather HUZZAH32).  
  - Broches SPI disponibles, bouton poussoir, LED intégrée, connecteur LiPo, connecteur micro-USB.  
- **VS1053b** (Adafruit Music Maker FeatherWing) ou module équivalent  
  - Gestion audio tout-en-un (décodage MP3, AAC, Ogg, etc.).  
  - Slot pour carte SD (lecture de MP3 locaux éventuellement).  
- **Câbles et alimentation**  
  - Câble USB-Micro B pour alimentation + console série.  
  - Batterie LiPo 3.7 V (optionnel, si portabilité totale).  
- **Casque ou haut-parleurs** (jack 3,5 mm).  
- **Breadboard et fils dupont** (pour prototypage si non-featherWing).  
- **Smartphone Android** (pour pilotage MQTT via « IoT MQTT Panel »).  

---

## Installation et configuration

### Arduino IDE

1. **Télécharger et installer l’IDE Arduino** (≥ 1.8.x) :  
   - Site officiel : [arduino.cc/en/software](https://www.arduino.cc/en/software): contentReference[oaicite:19]{index=19}.  
2. Dans `Fichier → Préférences` : ajouter le gestionnaire de cartes ESP32:  
   - https://raw.githubusercontent.com/espressif/arduino-esp32/package_esp32_index.json

3. Ouvrir `Outils → Type de carte → Gestionnaire de cartes`, chercher **esp32 by Espressif**, installer la version **≥ 3.2.0**.  

### Déposer les bibliothèques

Placer les bibliothèques suivantes dans `Arduino_Project/libraries/` (ou via `Croquis → Inclure une bibliothèque → Gérer les bibliothèques`) :
- **ESP_VS1053_Library** (Baldram) : [github.com/baldram/ESP_VS1053_Library](https://github.com/baldram/ESP_VS1053_Library) (non reconnue automatiquement, à copier manuellement) :contentReference[oaicite:20]{index=20}.  
- **ESP32_VS1053_Stream** (Cellie) : [github.com/CelliesProjects/ESP32_VS1053_Stream](https://github.com/CelliesProjects/ESP32_VS1053_Stream) :contentReference[oaicite:21]{index=21}.  
- **PubSubClient** (MQTT) : sous `Manage Libraries…`, installer **PubSubClient by Nick O’Leary** (dernière version). :contentReference[oaicite:22]{index=22}.  
- **WiFiManager** (tzapu) : via `Manage Libraries…` ou copier le repo pour configuration Wi-Fi dynamique. :contentReference[oaicite:23]{index=23}.  

### Organisation du dossier

SAE22/
├── 1 - Documentation/
│ ├── SAE202-Web_Radio.pdf
│ ├── RadioInternet_ESP32_VS1053.pptx
│ ├── VS1053b_Datasheet.pdf
│ ├── schema_esp32_vs1053.pdf
│ └── pcb_layout/
├── 2 - Arduino_Project/
│ ├── WebRadio_ESP32_VS1053.ino
│ ├── libraries/
│ │ ├── ESP_VS1053_Library/
│ │ ├── ESP32_VS1053_Stream/
│ │ ├── PubSubClient/
│ │ └── WiFiManager/
│ ├── src/
│ │ ├── webradio_wifi.cpp
│ │ ├── webradio_spi.cpp
│ │ ├── webradio_audio.cpp
│ │ ├── webradio_mqtt.cpp
│ │ ├── webradio_utils.cpp
│ │ └── webradio_utils.h
│ └── config.h
├── README.md
└── LICENSE


- **`WebRadio_ESP32_VS1053.ino`** : point d’entrée du sketch, inclut `config.h` et les modules de `src/`.  
- **`config.h`** : définir les pins VS1053 (CS, DCS, DREQ), les topics MQTT, SSID par défaut, tableau d’URLs de stations, etc.  
- **`src/`** : code organisé en fichiers séparés pour faciliter la maintenance (initialisation SPI, fonctions audio, Wi-FiManager, MQTT, utilitaires).  

---

## Branchement matériel (schéma)

1. **ESP32 ↔ VS1053 (SPI)** :  
   - **MOSI (ESP32 GPIO18)** → MOSI (VS1053)  
   - **MISO (ESP32 GPIO19)** → MISO (VS1053)  
   - **SCK (ESP32 GPIO05)** → SCK (VS1053)  
   - **MP3CS / SCI_CS (ESP32 GPIO32)** → MP3CS (VS1053)  
   - **XDCS / SDI_CS (ESP32 GPIO33)** → XDCS (VS1053)  
   - **DREQ (ESP32 GPIO15)** → DREQ (VS1053)  
   - **TX0 / RX0 : UART vers console** (USB)  
   - **3.3 V (ESP32)** → AVDD, IOVDD (VS1053) ; GND commun.  

2. **Sortie audio** :  
   - LEFT/RIGHT (VS1053) → prise jack 3,5 mm (casque / haut-parleurs)  
   - GBUF (VS1053) : buffer commun, ne pas raccorder à la masse ; garder un condensateur 100 nF pour stabilisation.  
   - Si pas de casque, ajouter condensateurs de couplage sur LEFT/RIGHT.  

3. **Alimentation** :  
   - ESP32 alimenté en 5 V via USB, régulateur interne fournit 3.3 V.  
   - Optionnel : batterie LiPo connectée à la prise JST de l’Adafruit Feather (régulation interne + recharge USB).  

_Doc PDF détaillé du câblage disponible dans `hardware_schematics/schema_esp32_vs1053.pdf`._  

---

## Configuration Wi-Fi (WiFiManager)

- Au démarrage (`setup()`), on lance :
  ```cpp
  WiFiManager wm;
  bool res = wm.autoConnect("ModuleRadioInternet", "password");
  if (!res) {
    Serial.println("Connexion Wi-Fi impossible !");
  } else {
    Serial.println("Connecté au Wi-Fi !");
  }

Si aucun réseau configuré, l’ESP32 crée un AP nommé « ModuleRadioInternet » (mot de passe « password ») ; l’utilisateur y connecte son smartphone/PC, accède à la page captive (192.168.4.1) pour saisir SSID/Mot de passe de son réseau Wi-Fi.

Une fois validé, l’ESP32 redémarre en client Wi-Fi normal, se connecte au réseau spécifié. 

## Liste des stations de radio

Dans `config.h`, définir un tableau d’URLs :

```cpp
const char* stations[] = {
  "http://streaming.radio1.com:8000/stream.mp3",
  "http://streaming.radio2.com:8000/stream.ogg",
  "http://example.fm/128k.aac",
  // … ajouter jusqu’à N stations
};
const uint8_t NUM_STATIONS = sizeof(stations) / sizeof(stations[0]);
```

Lors du changement de station (commande série `n`/`v` ou MQTT), on incrémente/décrémente `currentStationIndex`, puis on transmet `stations[currentStationIndex]` à la fonction de stream du VS1053 (via `ESP32_VS1053_Stream`).

---

## Commandes clavier série

Une fois connecté en USB (115200 baud), les commandes suivantes sont reconnues :

| Touche | Fonction                   | Détails                                                     |
| ------ | -------------------------- | ----------------------------------------------------------- |
| `n`    | Changer de station (+)     | Incrémente `currentStationIndex` → play                     |
| `v`    | Changer de station (−)     | Décrémente `currentStationIndex` → play                     |
| `y`    | Volume +                   | Incrémente le registre `SCI_VOL` (max = `0x0000`)           |
| `b`    | Volume −                   | Décrémente `SCI_VOL` (min = `0xFEFE`)                       |
| `g`    | Basses +                   | Incrémente `Bass_Amp` (max = 15) → write `SCI_BASS`         |
| `f`    | Basses −                   | Décrémente `Bass_Amp` (min = 0) → write `SCI_BASS`          |
| `j`    | Aigus +                    | Incrémente `Treble_Amp` (max = 15) → write `SCI_BASS`       |
| `h`    | Aigus −                    | Décrémente `Treble_Amp` (min = 0) → write `SCI_BASS`        |
| `d`    | Tonalité par défaut        | Réinitialise `Bass_Amp = 0` et `Treble_Amp = 0`             |
| `s`    | Spatialisation On/Off      | Toggle bit spatialisation (EarSpeaker, registre `SCI_MODE`) |
| `?`    | Afficher menu de commandes | Ré-affiche la liste des touches et fonctions                |

### Note :

* Le registre `SCI_VOL` du VS1053 est de 16 bits :

  * MSB = voie gauche, LSB = voie droite.
  * `0x0000` → volume max, `0xFEFE` → volume min (−127.5 dB).

* Le registre `SCI_BASS` (16 bits) :

  * Bits 12–15 : amplitude des basses (0–15, chaque unité → +1 dB).
  * Bits 8–11 : fréquence de coupure des basses (0–15).
  * Bits 4–7 : amplitude des aigus (0–15, chaque unité → +1 dB).
  * Bits 0–3 : fréquence de coupure des aigus (0–15).

Pour plus de détails, voir le chapitre 9 du datasheet VS1053b.

---

## Pilotage via MQTT

### Topics et payloads

Dans `config.h` :

```cpp
// Broker MQTT (IP/URL, port)
const char* mqtt_server   = "192.168.4.2";
const uint16_t mqtt_port  = 1883;

// Topics pour le contrôle
const char* topicSetStation = "webradio/chaine/set";
const char* topicSetVolume  = "webradio/volume/set";
const char* topicSetBass    = "webradio/bass/set";
const char* topicSetTreble  = "webradio/treble/set";
const char* topicSetSpatial = "webradio/spatial/set";

// Topics pour l’état
const char* topicGetStation = "webradio/chaine/get";
const char* topicGetVolume  = "webradio/volume/get";
const char* topicGetBass    = "webradio/bass/get";
const char* topicGetTreble  = "webradio/treble/get";
const char* topicGetSpatial = "webradio/spatial/get";
```

#### Payloads attendus :

* `webradio/chaine/set` : index (entier) ou URL → lecture immédiate
* `webradio/volume/set` : 0–100 (%) ou 0–0xFEFE (VS1053)
* `webradio/bass/set` : 0–15 → `Bass_Amp`
* `webradio/treble/set` : 0–15 → `Treble_Amp`
* `webradio/spatial/set` : 0 ou 1 → (dés)activer spatialisation

### Remarque :

Lorsque l’ESP32 reçoit un message `…/set`, il exécute l’action, puis publie la valeur sur `…/get` pour synchroniser l’interface.

#### Exemple :

```cpp
// Callback MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String messageTemp;
  for (uint i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }
  if (String(topic) == topicSetVolume) {
    int vol = messageTemp.toInt();
    setVolume(vol);
    publishState(topicGetVolume, vol);
  }
  // … idem pour les autres
}
```

---

## IoT MQTT Panel (Android)

1. Installer **IoT MQTT Panel** sur Android
2. Créer les widgets :

   * **Bouton** : `webradio/chaine/set` → payload `0`, `1`, …
   * **Slider** : `webradio/volume/set` → 0–100
   * **Slider** : `webradio/bass/set` → 0–15
   * **Slider** : `webradio/treble/set` → 0–15
   * **Switch** : `webradio/spatial/set` → 0 ou 1
3. Souscrire aux topics `…/get` pour affichage d’état en temps réel
4. Renseigner IP (`192.168.4.2`) et port (`1883`) dans les paramètres MQTT de l’app

---

## Fonctionnalités audio (VS1053)

### Volume

Registre `SCI_VOL` (0x0B) : 16 bits (gauche + droite)

```cpp
void setVolume(uint8_t volumePercent) {
  uint16_t code = map(volumePercent, 0, 100, 0xFEFE, 0x0000);
  stream.writeRegister(SCI_VOL, code);
}

uint16_t curVol = stream.readRegister(SCI_VOL);
int volPercent = map(curVol, 0xFEFE, 0x0000, 0, 100);
```

---

### Tonalité (basses / aigus)

Registre `SCI_BASS` (0x02) :

```cpp
uint8_t bassAmp = 5, bassFreq = 2;
uint8_t trebleAmp = 3, trebleFreq = 4;

uint16_t bassReg = (bassAmp << 12) | (bassFreq << 8) | (trebleAmp << 4) | (trebleFreq);
stream.writeRegister(SCI_BASS, bassReg);
```

Réinitialisation : `bassAmp = 0`, `trebleAmp = 0`, `bassFreq = 15`, `trebleFreq = 2`

---

### Spatialisation (EarSpeaker)

Registre `SCI_MODE` (0x00), bit `SM_DACT`

```cpp
if (enableSpatial) {
  stream.setSpatialOn();
} else {
  stream.setSpatialOff();
}
```

---

## Utilisation avancée

### Égaliseur 5 bandes (EQ5 Plugin)

Le plugin EQ5 se charge après l'initialisation SPI :

```cpp
stream.LoadUserCode(eq5_plugin, sizeof(eq5_plugin));
stream.SetEQ5(eqParamTable, mode);
stream.GetEQ5(eqParamTable);
```

Permet un contrôle plus fin que `SCI_BASS` sur : low, mid1, mid2, mid3, high.

---

### Lecture depuis carte SD

Si vous utilisez un FeatherWing avec slot SD :

* Insérer une carte avec `/MP3/*.mp3`
* Exemple :

```cpp
stream.startPlayingFile("/MP3/song.mp3");
```

Ajouter des commandes série / MQTT pour contrôler la lecture locale.

---

## Licence

Ce projet est sous licence **MIT** (modifiable selon vos besoins). Voir le fichier `LICENSE`.

---

## Citations des ressources

* Cahier des charges SAE202 WebRadio (ESP32 + VS1053)
* Diaporama « RadioInternet\_ESP32\_VS1053.pptx »
* Datasheet VS1053b (registres SCI\_BASS, SCI\_VOL, SCI\_MODE)
