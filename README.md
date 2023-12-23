README
Ce projet implémente un client et un serveur TFTP en langage C. Le protocole TFTP (Trivial File Transfer Protocol) est un protocole léger pour transférer des fichiers sur un réseau.

1. Utilisation des arguments passés à la ligne de commande: 

Le programme prend en entrée les arguments de la ligne de commande (argc et argv).
Il vérifie si le nombre d'arguments est suffisant (au moins 4 arguments sont requis).
Si le nombre d'arguments est insuffisant, le programme affiche un message d'erreur et renvoie un code d'erreur.
Les valeurs des arguments (<server>, <file>, <port>) sont copiées dans des tableaux de caractères dédiés.
Les informations (hôte, fichier, port) sont affichées sur la sortie standard.


2. Appel à getaddrinfo pour obtenir l'adresse du serveur:

Le programme initialise une structure struct addrinfo pour spécifier les critères de recherche d'adresse.
Il appelle getaddrinfo pour obtenir les informations sur l'adresse du serveur en fonction des arguments passés.
En cas d'échec, il affiche un message d'erreur. Sinon, il affiche un message de succès.


3. Réservation d'un socket de connexion vers le serveur:

Le programme crée un socket en utilisant les informations obtenues précédemment avec getaddrinfo.
En cas d'échec, il affiche un message d'erreur. Sinon, il affiche un message de succès.


4. Pour gettftp (a, b, c):

Construction d'une requête en lecture (RRQ) correctement formée et envoi au serveur:
Il construit un paquet RRQ basé sur le protocole RFC1350 et l'envoie au serveur.
Réception d'un fichier constitué d'un seul paquet de données (DAT) et son acquittement (ACK):
Il reçoit un paquet de données du serveur, extrait les informations, écrit les données dans un fichier et envoie un ACK en réponse.
Réception d'un fichier constitué de plusieurs paquets de données (DAT) et leurs acquittements respectifs (ACK):
Il continue à recevoir et traiter les paquets de données jusqu'à la fin du transfert, en envoyant des ACK pour chaque paquet reçu.


5. Pour puttftp (a, b, c):

Construction d'une requête en écriture (WRQ) correctement formée et envoi au serveur:
Il construit un paquet WRQ basé sur le protocole RFC1350 et l'envoie au serveur.
Envoi d'un fichier constitué d'un seul paquet de données (DAT) et réception de son acquittement (ACK):
Il lit les données d'un fichier, les envoie au serveur sous forme de paquet de données, puis attend et traite l'ACK correspondant.
Envoi d'un fichier constitué de plusieurs paquets de données (DAT) et réception de leurs acquittements respectifs (ACK):
Il continue à lire, envoyer des paquets de données et recevoir des ACK jusqu'à la fin du fichier.


