FT_IRC - Votre Serveur IRC en C++



Bienvenue sur le dépôt GitHub de FT_IRC, un serveur IRC implémenté en C++ offrant une expérience de chat classique avec une touche moderne. Ce projet vise à recréer les fonctionnalités fondamentales d'un serveur IRC tout en introduisant des améliorations et des fonctionnalités supplémentaires pour enrichir l'expérience utilisateur.
Fonctionnalités

    Gestion des commandes IRC de base (NICK, USER, JOIN, PART, PRIVMSG, PING, QUIT et bien d'autres).
    Support de multiples canaux avec gestion des droits d'accès.
    Gestion efficace des utilisateurs et de leurs sessions.
    Extension du protocole pour inclure des fonctionnalités modernes telles que la gestion des avatars.
    Architecture modulaire permettant une extension facile du serveur.

![giphy](https://github.com/Evxn133/ft_irc/assets/144713484/236e35e5-49d0-4459-8a29-583c052124a6)

Démarrage Rapide
Prérequis

    Un système d'exploitation Linux ou UNIX.
    Compilateur C++ supportant C++98 (GCC recommandé).
    make pour la compilation du projet.

Compilation

Clonez le dépôt et naviguez dans le répertoire du projet :

bash

git clone https://github.com/yourusername/FT_IRC.git
cd FT_IRC

Compilez le projet :

bash

make

Lancer le Serveur

Pour démarrer le serveur sur le port 6667 avec le mot de passe coucou :

bash

./ircserv 6667 coucou

Utilisation

Connectez-vous au serveur IRC en utilisant votre client IRC favori (par exemple, Irssi, WeeChat, ou mIRC) en spécifiant l'adresse IP (ou le nom d'hôte) et le port sur lequel le serveur s'exécute.
Contribution

