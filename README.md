# Introduction
Cet appareil se compose d'une carte Arduino UNO (appelée "module CAVE" ) placée dans la cave et permettant de :
- mesurer :
	- la hauteur d'eau dans un puit,
	- les deltas de hauteur sur 30 mn, 1 h, 3 h, 6 h, 12 h, 1 jour, 7 jour, 14 jour, 1 mois, 3 mois, 6 mois, 1 an,
	- la température,
	- l'humidité
- calculer des alarmes orange et rouge pour prévenir d'un débordement dû à la montée de la nappe phréatique,
- transmettre en Bluetooth ces éléments sous format JSON.

Un second appareil (appelé "module AFFICHAGE") permet d'afficher ces données. cet appareil peut-être :
- une carte Arduino,
- un PC sous LINUX,
- un téléphone Nokia,
- un téléphone Android 

    
# Structure JSON transmise
Les données transmises sont formatées en JSON de la façon suivante :
```
{
    "n": 52, 	// Nombre de mesures
    "d": [	// d comme "D"ate
        2023,	// index 0 : année
        12,	// index 1 : mois
        25,	// index 2 : jour
        12,	// index 3 : heures
        45,	// index 4 : minutes
        13	// index 5 : secondes
    ],
    "t": 30,	// t comme "T"empérature
    "h": 66,	// h comme "H"umidité
    "x": 334,	// x pour la hauteur d'eau
    "v": [	// v comme "V"ariation
        -1,	// index 0 : delta 30mn
        -2,	// index 0 : delta 1h
        -3,	// index 0 : delta 3h
        -4	// index 0 : delta 6h
    ]
}
```
Jeu de données pour tests : `{"n":525600,"d":[2023,12,25,12,45,13],"t":30,"x":334,"h":66,"v":[-1,-2,-3,-4]}`
# Module CAVE
## Hardware
- carte Arduino UNO
- carte VELLEMAN VMA 202 (RTC clock + SD card) utilisant les broches :
	- D10 : SD card
	- D11 : SD card
	- D12 : SD card
	- D13 : SD card
	- A4 : SDA RTC clock (bus I2C)
	- A5 : SCL RTC clock (bus I2C)
- capteur de profondeur
	- A3
- capteur humidité et température :
	- D7
- carte Bluetooth HC-05 :
	- D8 : RX avec un pont résistif 1,8k/3,2k pour passer en 3.3v
	- D9 : TX
- Terminal Arduino :
	- D0 : RX
	- D1 : TX
## Utilisation
## Améliorations
- Ajouter un horodatage aux mesures.

# Module AFFICHAGE
## Version Arduino
### Hardware
### Utilisation
### Améliorations
- Afficher les caractères accentués (https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts),
- Afficher les touches de fonctions du menu en cours en grisé, 
- Stoquer les données dans la carte SIM,
- Afficher une courbe de variation de hauteur,
- Passage de l'affichage en mode veille au bout d'un certain temps,
- Afficher température et humidité dans la maison.
- Afficher dans les logs le nombre de mesures manquantes et la durée de fonctionnement sans coupure
- Afficher les derniers message de log

## Version PC LINUX
## Version NOKIA 8110
## Version Android
-----------------------------------------------------------------------------

### V00
  #### Hardware :
    Arduino power plug from 7 to 12V (Volts) of DC (Direct Current) : 5.5mm diameter cylindrical plug with 2.1mm pin hole, and that provides Positive voltage on the inside pin hole and Negative (or common/ground) voltage on the outside cylindrical sleeve of the connector plug.
  #### partie CAVE
    ##### logiciel CAVE_01
    Il effectue les tâches suivantes
    - mesure toute les minutes de la profondeur
    - mémoire des mesures : 1 semaine, 24h, 12h, 6h, 3h, 1h, 30mn,
    - calcul des variations : 1 semaine, 24h, 12h, 6h, 3h, 1h, 30mn.
    - affichage de LEDS :
      - vert : pas d'augmentation de la hauteur de la nappe,
      - orange : risque de débordement sous 1 semaine,
      - rouge ; risque de débordement sous 24h?
    - envoie des données en Bluetooth
    - impression des données sur la console série
    ##### câblage
    - capteur de profondeur : A2
    - bluetooth :
    - LEDs : 
  #### partie AFFICHAGE
##### logiciel CAVE_01
##### câblage


  - capteur de profondeur : A2
  - bluetooth :
  - afficheur :
  - horloge
  - carte SD
  
#### Fonctionnement
- démarrer avec le switch en mode configuration, toutes les leds (verte, orange et rouge) sont allumée, on peut alors rentrer les commandes suivantes dans la console terminal, après avoir change la config du terminal pour `both NL & CR`:
	- réinitialiser les paramètres par défaut : `AT+ORGL`
	- effacer les appareils apairés : `AT+RMAAD`
	- configurer le mot de passe : `AT+PSWD=9362`
	- assigner le mode esclave : `AT+ROLE=0`
	- modifier le nom : `AT+NAME=CAVE_FB`
	- récupérer et noter l'adresse du module: `AT+ADDR?` : 98d3:11:fd232c
	
- arrêter et redémarrer avec le switch en mode normal. Une mesure est effectuée toutes les secondes et affichée sur la console. Les deltas de 30mn, 1h, 3h, 6h, 1j, 7j, 14j, 1m, 3m, 6m, 1 an sont aussi affichés sur la console. Les LEDs ont alors un rôle d'alarme :
	- LED verte : le niveau est stable ou baisse
	- LED orange : risque de débordement sous 1 semaine
	- LED rouge : risque de débordement sous 1 jour

##### Problème
UNO a un ADC de 10 bits seulement => précision de 0,48 cm. c'est suffisant. Mais Pour des convertisseurs 12 bits, on peut utiliser les arduino suivants : The Zero, Due, MKR family and Nano 33 (BLE and IoT) boards have 12-bit ADC capabilities that can be accessed by changing the resolution to 12.


## Logiciel CAVE_02
Correspond à l'afficheur Arduino des données transmises par la cave.
La configuration du module HC05 est alors la suivante :
- réinitialiser les paramètres par défaut : `AT+ORGL`
- effacer les appareils apairés : `AT+RMAAD`
- configurer le mot de passe : `AT+PSWD=9362`
- assigner le mode esclave : `AT+ROLE=1`
- modifier le nom : `AT+NAME=CAVE_FB MASTER`
- mettre en connexion vers adresse unique : `AT+CMODE=0`
- donner l'adresse du module distant : `AT+BIND=98d3,11,fd232c`

## B1) Connexion avec un PC Ubuntu avec Bluetooth
Cf : `http://www.userk.co.uk/arduino-bluetooth-linux/`
### Installation
Avec UBUNTU, la stack de gestion Bluetooth est déjà installée `bluez`. Il reste à installer l'émulateur de terminal série `minicom` : 
- `apt update`
- `sudo apt install minicom`

### Utilisation
- 1) Connecter le device Bluetooth
	- utiliser `bluetoothctl`
	- `scan on`
	- `agent on`
	- `pair 98:D3:11:FD:23:2C` en fournissant éventuellement le PIN code
	- `connect 98:D3:11:FD:23:2C`
- 2) Y attacher le device série `rfcommm0`
	- `sudo killall rfcomm`
	- `sudo rfcomm connect /dev/rfcomm0 98:D3:11:FD:23:2C 1 &`
- 3) Lancer le terminal sur le device série connecté sur `/dev/rfcomm0` (config appelée `bluetooth`) : `sudo minicom bluetooth`

Nb :
- pour supprimer avec bluetoothctl (si PIN code pas demandé) :  disconnect, untrust, and remove the device by bluetooth MAC address.
- pour obtenir des informations sur l'état du device : `bluetoothctl info 98:D3:11:FD:23:2C`, à noter `UUID: Serial Port `.
- pour voir les ports RFCOMM utilisés : `rfcomm -a`

## B2) Connexion avec un Nokia 8118 en Bluetooth

## B3) Connexion à un Android en Bluetooth
Il faut une application supportant le profile Bluetooth : `Serial Port Bluetooth Profile (SPP)` appelé aussi `RFCOMM`
Application utilisables :
- Serial Bluetooth Terminal : `https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&hl=fr&gl=US`
- Bluetooth SPP Manager : `https://play.google.com/store/apps/details?id=at.rtcmanager&hl=en_US`
- BTerm SPP : `https://play.google.com/store/apps/details?id=kibo.bterm`
- BlueSPP : `https://play.google.com/store/apps/details?id=com.shenyaocn.android.BlueSPP&hl=fr&gl=US`

## B4) Connexion à un autre Arduino avec afficheur en Bluetooth
Informations :
- https://wiki.mozilla.org/B2G/Bluetooth


## Annexes
### Bluetooth information
- pour obtenir les profiles Bluetooth d'un objet :
	- sur Ubuntu : run `bluetoothctl`
	- obtenir l'adresse du device voulu : `scan le`
	- puis obtenir les infos du device : `info 20:39:56:28:1B:F7`
  Each profile as a globally unique ID, the UUID. Profiles standardized by the Bluetooth Special Interests Group (SIG) have a 128bit-UUID in the form of 0000XXXX-0000-1000-8000-00805F9B34FB. 
  
Ce qui nous intéresse semble le `Serial Port Bluetooth Profile (SPP)`

## Matériel nécessaire
- arduino avec carte mémoire pour enregistrer une mesure par heure :
  - tutorial et liste SD cards : https://docs.arduino.cc/learn/programming/sd-guide
- affichage pour voir les données :
  - E44 display + clavier 2x16 : https://whadda.com/fr/produit/module-lcd-et-clavier-pour-arduino-lcd1602-wpsh203/
  - écran tactile : https://www.gotronic.fr/art-shield-ecran-tactile-2-8-tf028-28507.htm
  	- tutorial : https://docs.arduino.cc/retired/getting-started-guides/TFT
- capteur de profondeur (pas de nécessité de capteur de pression athmosphérique : la différence est mesurée) :
  - https://wiki.dfrobot.com/Throw-in_Type_Liquid_Level_Transmitter_SKU_KIT0139
  - https://www.gotronic.fr/art-capteur-de-pression-etanche-gravity-kit0139-32275.htm#complte_desc
- capteur de température
- carte bluetooth : HC05 est maitre/esclave, HC06 est seulement esclave :
  - carte HC05 HC06 : https://arduino-france.site/bluetooth-hc-05/
  - fonctionnement en maitre/esclave : https://wikilab.myhumankit.org/index.php?title=Ressources:Communication_Bluetooth_HC_05
  - datasheet : https://components101.com/wireless/hc-05-bluetooth-module
  - the basis of bluetooth : https://www.opensourceforu.com/2015/06/linux-without-wires-the-basics-of-bluetooth/
- affichage possible sur :
  - autre arduino
  - nokia 8110
    - KAIos bluetooth API : https://developer.kaiostech.com/docs/api/web-apis/bluetooth/bluetooth/
  - android
  - PC :
    - https://www.baeldung.com/linux/bluetooth-via-terminal
    - send data via Bluetooth : http://www.userk.co.uk/arduino-bluetooth-linux/
    - another solution : https://avilpage.com/2017/10/bluetooth-communication-between-ubuntu-android.html
