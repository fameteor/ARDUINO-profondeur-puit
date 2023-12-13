# ARDUINO-profondeur-puit
Projet ARDUINO pour mesurer la hauteur d'eau dans un puit.

## Matériel nécessaire
- arduino avec carte mémoire pour enregistrer une mesure par heure
- affichage pour voir les données
- capteur de profondeur (pas de nécessité de capteur de pression athmosphérique : la différence est mesurée) :
  - https://wiki.dfrobot.com/Throw-in_Type_Liquid_Level_Transmitter_SKU_KIT0139
  - https://www.gotronic.fr/art-capteur-de-pression-etanche-gravity-kit0139-32275.htm#complte_desc
- capteur de température
- carte bluetooth : HC05 est maitre/esclave, HC06 est seulement esclave :
  - carte HC05 HC06 : https://arduino-france.site/bluetooth-hc-05/
  - fonctionnement en maitre/esclave : https://wikilab.myhumankit.org/index.php?title=Ressources:Communication_Bluetooth_HC_05
- affichage possible sur :
  - autre arduino
  - nokia 8110
    - KAIos bluetooth API : https://developer.kaiostech.com/docs/api/web-apis/bluetooth/bluetooth/
  - android
