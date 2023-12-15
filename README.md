# ARDUINO-profondeur-puit
Projet ARDUINO pour mesurer la hauteur d'eau dans un puit.

## Matériel nécessaire
- arduino avec carte mémoire pour enregistrer une mesure par heure :
  - tutorial et liste SD cards : https://docs.arduino.cc/learn/programming/sd-guide
- affichage pour voir les données :
  - E44 display + clavier 2x16 : https://whadda.com/fr/produit/module-lcd-et-clavier-pour-arduino-lcd1602-wpsh203/
  - écran tactile : https://www.gotronic.fr/art-shield-ecran-tactile-2-8-tf028-28507.htm
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
    

## Versions

### V00
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
- démarrer avec le switch en mode configuration, toutes les leds (verte, orange et rouge) sont allumée, on peut alors rentrer les commandes suivantes :
	- réinitialiser les paramètres par défaut : `AT+ORGL`
	- effacer les appareils apairés : `AT+RMAAD`
	- configurer le mot de passe : `AT+PSWD=9362`
	- assigner le mode esclave : `AT+ROLE=0`
	- modifier le nom : `AT+NAME=CAVE_FB`
	- récupérer et noter l'adresse du module: `AT+ADDR?`
	
- arrêter et redémarrer avec le switch en mode normal. Une mesure est effectuée toutes les secondes et affichée sur la console. Les deltas de 30mn, 1h, 3h, 6h, 1j, 7j, 14j, 1m, 3m, 6m, 1 an sont aussi affichés sur la console. Les LEDs ont alors un rôle d'alarme :
	- LED verte : le niveau est stable ou baisse
	- LED orange : risque de débordement sous 1 semaine
	- LED rouge : risque de débordement sous 1 jour

##### Problème
UNO a un ADC de 10 bits seulement => précision de 0,48 cm. c'est suffisant. Mais Pour des convertisseurs 12 bits, on peut utiliser les arduino suivants : The Zero, Due, MKR family and Nano 33 (BLE and IoT) boards have 12-bit ADC capabilities that can be accessed by changing the resolution to 12.


## Connexion avec un PC Ubuntu avec Bluetooth
Cf : `http://www.userk.co.uk/arduino-bluetooth-linux/`
### Installation
Avec UBUNTU, la stack de gestion Bluetooth est déjà installée `bluez`. Il reste à installer l'émulateur de terminal série `minicom` : 
- `apt update`
- `sudo apt install minicom`

### Utilisation
- faire la connexion
- configurer le terminal
- lancer le terminal avec la bonne config : `sudo minicom bluetooth`

## Connexion avec un Nokia 8118 en Bluetooth

## Connexion à un Android en Bluetooth

## Connexion à un autre Arduino avec afficheur en Bluetooth
