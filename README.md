# SAE22

### Partie 1 

Q1 : A quoi sert le bouton poussoir ?

Le bouton poussoir **RESET permet de hard reset l'esp32**

Q2 : Y-a-t-il une LED qui peut être utilisée dans un programme ? Quel numéro lui est affecté ? 

Oui, il ya une LED rouge d'indication affectée à **GPIO 13**.


Q3 : Que signifie GPIO ? Donner des exemples d’utilisation.

GPIO : **General Purpose Input Output**

ex : 
- Lire l’état d’un bouton poussoir (ex. lire digitalRead(GPIO14) pour savoir si le bouton est enfoncé)

- Piloter une LED (ex. écrire digitalWrite(GPIO13, HIGH) pour allumer la LED embarquée).


Q4 : Quelle est la fréquence du quartz ? **32 kHz** 

Q5 : Peut-on mettre une batterie sur la carte ? Peut-elle être chargée par l’USB ?

Oui. La carte dispose d’un connecteur JST 2 broches pour batterie LiPo (3,7 V / 4,2 V) et possède un circuit de charge interne :

Si la carte est alimentée en USB, l’ESP32 basculera automatiquement sur le 5 V USB et chargera la batterie LiPo (jusqu’à 200 mA).

Si l’USB est débranché, le système bascule sur la batterie en mode “backup” sans coupure (hot-swap).


Q6 :

Broches :

MISO - 19
MOSI - 18
SCK - 05
MP3CS - 32
DREQ - 15
XDCS - 33

**SPI : Serial Peripheral Interface**

C’est un protocole de communication synchrone, full-duplex, en architecture Maître-Esclave.

Quatre lignes principales :

SCLK (Serial Clock) : généré par le maître pour cadencer l’échange.

MOSI (Master Out, Slave In) : données envoyées du maître vers l’esclave.

MISO (Master In, Slave Out) : données envoyées de l’esclave vers le maître.

SS ou CS (Slave Select / Chip Select) : chaque esclave est “sélectionné” par une broche CS mise à LOW par le maître.