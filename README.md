# Introduction
Cet appareil se compose d'une carte Arduino UNO (appelée "module CAVE" ) placée dans la cave et permettant de :
- mesurer :
	- la hauteur d'eau dans un puit,
	- les deltas de hauteur sur 30 mn, 1 h, 3 h, 6 h, 12 h, 1 jour, 7 jour, 14 jour, 1 mois, 3 mois, 6 mois, 1 an,
	- la température,
	- l'humidité
- calculer des alarmes orange et rouge pour prévenir d'un débordement dû à la montée de la nappe phréatique,
- transmettre en Bluetooth ces éléments sous format JSON,
- stoquer ces éléments sur une carte SD.

Un second appareil (appelé "module AFFICHAGE") permet d'afficher ces données. cet appareil peut-être :
- une carte Arduino,
- un PC sous LINUX,
- un téléphone Nokia,
- un téléphone Android.

    
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
	- D8 : vers HC-05 RX avec un pont résistif 1,8k/3,3k vers la masse pour passer en 3.3v
	- D9 : vers HC-05 TX
- Terminal Arduino :
	- D0 : RX
	- D1 : TX
	
Une alimentation 12V DC est nécessaire pour le capteur de profondeur. La prise Arduino a les specs suivantes : 5.5mm diameter cylindrical plug with 2.1mm pin hole, and that provides Positive voltage on the inside pin hole and Negative (or common/ground) voltage on the outside cylindrical sleeve of the connector plug.
## Utilisation
- configurer le module HC-05 en master (cf $ configuration HC-05)
## Améliorations
- l'Arduino UNO à un ADC de 10 bits seulement. Pour une plage de mesure de 1v à 4v (0 à 5m de profondeur) avec 10 bits de 0 à 5V, cela donne une précision de 0,8 cm environ. Envisager de passer en ADC 12 bits ?
# Module AFFICHAGE version Arduino
## Hardware
> Nb : compte tenu du fait que la carte Bluetooth utilise le port série standard de l'arduino, elle ne doit pas être branchée lorqu'on télécharge le code sur l'Arduino ! 

- carte Arduino UNO
- carte affichage touchscreen TFT028 de 2,8 pouces de diamètre
	- utilise toutes les E/S sauf :
		- A?
		- D0
		- D1
- carte Bluetooth HC-05 :
	- D0-RX : vers HC-05 RX avec un pont résistif 1,8k/3,3k vers la masse pour passer en 3.3v
	- D1-TX : vers HC-05 TX
## Utilisation
- configurer le module HC-05 en slave (cf $ configuration HC-05)
## Améliorations
- Afficher les caractères accentués (https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts),
- Afficher une courbe de variation de hauteur,
- Passage de l'affichage en mode veille au bout d'un certain temps,
- Afficher température et humidité dans la maison.
- Afficher dans les logs le nombre de mesures manquantes et la durée de fonctionnement sans coupure
- Afficher les derniers message de log

-----------------------------------------------------------------------------

# Configuration HC-05

> Nb : compte tenu du fait que la carte Bluetooth utilise le port série standard de l'arduino, elle ne doit pas être branchée lorqu'on télécharge le code sur l'Arduino !  

- Charger sur un Arduino UNO le soft par défaut de l'IHM Arduino (config qui ne fait rien),
- Connecter le HD-05 à l'Arduino :
	- D0-RX : vers HC-05 RX avec un pont résistif 1,8k/3,3k vers la masse pour passer en 3.3v
	- D1-TX : vers HC-05 TX
- Passer en mode terminal en `38400` bps avec `both NL & CR`. On peut alors taper la commande `AT` envoyée au HC-05 qui répond par `ÒK`.

## Configurer le slave (module CAVE) :
- réinitialiser les paramètres par défaut : `AT+ORGL`
- effacer les appareils apairés : `AT+RMAAD`
- configurer le mot de passe : `AT+PSWD=9362`
- assigner le mode esclave : `AT+ROLE=0`
- modifier le nom : `AT+NAME=CAVE_FB SLAVE`
- récupérer et noter l'adresse du module: `AT+ADDR?` : `98d3:11:fd232c`

## Configurer le master (module AFFICHAGE) :
- réinitialiser les paramètres par défaut : `AT+ORGL`
- effacer les appareils apairés : `AT+RMAAD`
- configurer le mot de passe : `AT+PSWD=9362`
- assigner le mode esclave : `AT+ROLE=1`
- modifier le nom : `AT+NAME=CAVE_FB MASTER`
- mettre en connexion vers adresse unique : `AT+CMODE=0`
- donner l'adresse du module distant : `AT+BIND=98d3,11,fd232c`
	
-----------------------------------------------------------------------------


## Afficheur version PC LINUX
## Afficheur version NOKIA 8110
## Afficheur version Android

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
