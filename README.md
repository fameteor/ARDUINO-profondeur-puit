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
    https://www.baeldung.com/linux/bluetooth-via-terminal

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
