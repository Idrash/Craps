// Projet universitaire L2 Informatique 2018/2019
// Realiser par: TOGNETT Yohann
// N° etudiant: 21704017
//OS testé et fonctionnel: Windows

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "craps.h"

//menu principale :===================================================================================
int menuPrincipale(){
	printf("<<<Nice Craps>>>>\n");
	printf("Selectionner en entrant le numero correspondant\n");
	printf("1 : Ajouter un joueur\n");
	printf("2 : Afficher les joueur\n");
	printf("3 : Supprimer un joueur\n");
	printf("4 : Sauvegarder un joueur\n");
	printf("5 : Charger un joueur\n");
	printf("6 : Commencer le tour\n");
	printf("7 : Tableau des scores\n");
	printf("8 : Quiter le jeu\n");
	printf("Votre choix : ");
	int choix=0;
	scanf("%d",&choix);
	flush(stdin);
	system("@cls||clear"); //permet de clear la console sous linux ou windows
	if(choix>=1 && choix <=8){return choix;}
	else {
		printf("Choix invalide\n================================\n\n");
		return menuPrincipale();
	}
}






//parie :===================================================================================
int menuParie(Liste* L){
	printf("Selectionner le joueur en entrant le numero correspondant\n");
	afficheListe(L);
	printf("%d : pour retourner au menu precedent\n",L->nbplayer+1);
	printf("Votre choix : ");
	int choix;
	scanf("%d",&choix);
	flush(stdin);
	system("@cls||clear");
	choix-=1;
	//si le choix est valide
	if(choix>=0 && choix<=L->nbplayer){return choix;}

	//sinon recommencer
	else{
		printf("Choix invalide\n================================\n\n");
		return menuParie(L);
	}
}

//Menu Phase
int menuPhase(){
	printf("Selectionner en entrant le numero correspondant\n");
	printf("1 : miser ou modifier les mise\n");
	printf("2 : Lancer les des\n");
	printf("Votre choix : ");
	int choix=0;
	scanf("%d",&choix);
	flush(stdin);
	system("@cls||clear");
	//si le choix est valide
	if(choix==1 || choix ==2){return choix;}

	//sinon recommencer
	else{
		printf("Choix invalide\n================================\n\n");
		return menuPhase();
	}
}



//====================================Main=========================================

int main(int argc, char *argv[]) {
	//initialisation randome
	srand(time(NULL));
	//Liste chainé
	Liste* L=initListe();
	//chargement de la sauvegarde bestGain(si ell existe)
	chargeBestGain(L->bGain);
	//debut de la parti
	/*
	ingame contient l'etat de la parti : 
	0=fin;
	1=menu principale;
	2=phase de tirage;
	le 3 n'est plus present du a des modification
	4 = page de parie (change celon la phase de jeu)
	*/
	int ingame=1;
	int choix;
	while(ingame){
		//menu principale
		if(ingame==1){
			resetModifVariable(L);
			L->Point==0;
			choix=menuPrincipale();
			ingame = traitementMenuPrincipale(L,choix);
		}
		//menu de phase de tirage
		if(ingame==2){
			printf("Phase: %d\n",L->Phase);
			if(L->Point!=0){printf("Le point est : %d\n",L->Point);}
			choix=menuPhase();
			ingame=traitementMenuPhase(L,choix);
		}

		//menu de paris
		if(ingame==4){
			choix=menuParie(L);
			ingame=traitementParie(L,choix);
		}
	}
	return 0;
}
	

