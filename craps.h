// Projet universitaire L2 Informatique 2018/2019
// Realiser par: TOGNETT Yohann
// N° etudiant: 21704017
//OS testé et fonctionnel: Windows

#ifndef _CRAPS_H
#define _CRAPS_H

short tirage();//tire deux dés et renvoie le resultat

void flush(FILE* f);//fonction vider cache

struct bestGain{ //prend en compte que les gain >0
	char* name;  //nom de la personne
	int gain;	//gain de la personne
};
typedef struct bestGain bestGain;


struct player {
	char* name; //nom de la personne
	int money; //argent de la personne
	int Pass; //mise sur Pass
	int dontPass; //mise sur dontPass
	//pour la phase suspendu (booleen):
	//si =1: a pas douber ou diviser par deux il peut dont le faire ; si =0 a eja fait l'operation impposible de l'effectuer une seconde fois
	int modifPass;
	int modifDontPass;

};
typedef struct player player;


//element Liste chainée
typedef struct element element;
struct element
{
    player* P;
    element *suivant;
};



struct Liste{
    element *premier; //premier joueur (si existant)
    int nbplayer; //nombre de joueur dans la liste
    int Point; //Valeur du Point(pour la seconde phase)
    int Phase; //numero de la phase en cour
    bestGain *bGain; //tableau des 10 meillieurs gain (nom + gain)
};
typedef struct Liste Liste;


//fonction player-------------------------
player* playerInit(); //initialisation de la structure player
void playerSetName(player* P,char* setName); //mis en place du nom du joueur
void playerSetMoney(player* P , int setMoney); //mis en place de la money du joueur
int setPariePass(player* P,int value); //mis en place du parie sur Pass (transfert de P->money a Pass)
int setParieDontPass(player* P,int value); //mis en place du parie sur Pass (transfert de P->money a DontPass)
int gainPass(player* P,int mult); //transfere la parie Pass vers la money celon le coeficient multiplicateur multl
int gainDontPass(player* P,int mult); //transfere la parie dontPass vers la money celon le coeficient multiplicateur multl
int augmenterPass(player* P,int value); //augmente Pass de value (retire value a money pour la mettre a Pass)
int augmenterDontPass(player* P,int value); //augmente dontPass de value (retire value a money pour la mettre a dontPass)
int diminuerDontPass(player* P,int value); //diminue dont pass de value (ajoute value a money et retire value a dontpass)
void savePlayer(player* P); //sauvegarde d'un joueur
player* ChargePlayer(Liste* L,int n); //charger un joueur sauvegarder dans le fichier
int afficheSavePlayer(); //affiche la liste des joueur sauvegarder
void suppSave(int n); //supprime la sauvegarde quand (appeler quand le joueur est charger)



//List chainé-----------------------------
Liste* initListe(); //initialisation de la liste
void pushListe(Liste* L,player* newP); //ajout d'un joueur a la fin de la liste
void afficheListe(Liste* L); //affiche tous les joueur de la liste
void supElt(Liste* L,int n); //suprime un element n de la liste (puis free cette element par freeElt())
int estVide(Liste* L); //renvoie un booleen vide ou non
player* selectPlayer(Liste* L,int n); //selectionne le playe n pour pouvoir faire des opperation dessue: example modifier ces parie ,...

int afficheTirage_ActualiseGain(Liste*L); 
/*
-affiche la resultat de tirage
-actualise la phase de jeu
-finit la parti si necessaire
-actualise les gain de chaque joueur celon leurs paris
-actualise le tableau des meillieurs gain
*/

void resetModifVariable(Liste* L);//remet a 1 modifPass et modifdontPass a la fin de chaque lancée de dé


//element ---------------------------
void afficheElt(element* elt); //affiche le joueur lié a l'element elt

void freeElt(element* elt); 
//libere la memoire de elt soit:
//name du joueur
//le joueur
//l'element 


//traitement des menu---------------
player* createNewPlayer(Liste* L); //crée un joueur en par rapport au information entrer par le joueur
int traitementMenuPrincipale(Liste* L,int choix); //traite le choix fait lors du menu principale
int traitementMenuPhase(Liste* L,int choix); //traite le choix fait par le joueur durant le menu des phase
int traitementParie(Liste* L,int choix); //traite et affiche tout le menu ou l'on peut faire les parie ou les modifier celon les phase


//bestGain;-------------------------
bestGain* initBestGain(); //initialise la structure bestGain
void afficheBestGain(bestGain* B); //affiche le tableau des meillieur gain
void actualiseBestGain(bestGain* B,char* name,int gain); //met a jour le tableau des gain si le gain rentre dans le top 10 puis le sauvegarde
void chargeBestGain(bestGain* B); //charge le fichier qui contient les plus haut gain
void saveBestGain(bestGain* B); //sauvegarde les meillieur gain dans le fichier
//je considere que la structure bestgain n'a pas besoin de free car elle reste actif durant toutes la parti



#endif
