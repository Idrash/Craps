// Projet universitaire L2 Informatique 2018/2019
// Realiser par: TOGNETT Yohann
// N° etudiant: 21704017
//OS testé et fonctionnel: Windows

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "craps.h"

void flush(FILE* f){
	int c;
	while((c=fgetc(f)) != '\n' && c != EOF);
}


short tirage(){
	short d1,d2;
	d1=rand()%6 +1;
	d2=rand()%6 +1;
	return d1 + d2; //revoie la somme des deux dés a 6 face {1,2,3,4,5,6}
}




//fonction sur le joueur ========================================================================
player* playerInit() {
	player* P=(player*)malloc(sizeof(player));
	P->name = (char*)malloc(25);
	P->money = 0;
	//parie sur Pass et dontPass
	P->Pass = 0;
	P->dontPass = 0;
	//booléen
	P->modifDontPass=1;
	P->modifPass=1;
	return P;
}

void playerSetName(player* P,char* setName){
	strcpy(P->name,setName);
}

void playerSetMoney(player* P , int setMoney){
	P->money = setMoney;
}


int setPariePass(player* P,int value){
	//si -1 mise tout l'argent du joueur
	if(value==-1){
		P->Pass+=P->money;
		P->money=0;
		return 1;
	}
	if(value<-1){return -1;}//mise negative non valide
	if(value > P->money + P->Pass){return 0;}
	P->money+=P->Pass-value;
	P->Pass=value;
	return 1;
}

int setParieDontPass(player* P,int value){
	if(value==-1){
		P->dontPass+=P->money;
		P->money=0;
		return 1;
	}
	if(value<-1){return -1;}
	if(value > P->money + P->dontPass){return 0;}
	P->money += P->dontPass - value;
	P->dontPass = value;
	return 1;
}

int gainPass(player* P,int mult){
	P->money+= P->Pass*mult;
	int tmp=P->Pass;
	P->Pass=0;
	if(mult==0 && tmp!=0){return -tmp;}
	if(mult==1){return 0;}
	return tmp;
}

int gainDontPass(player* P,int mult){
	P->money+= P->dontPass*mult;
	int tmp=P->dontPass;
	P->dontPass=0;
	if(mult==0 && tmp!=0){return -tmp;}
	if(mult==1){return 0;}
	return  tmp;
}

int augmenterPass(player* P,int value){
	if(value>P->money){return 0;} //opperation non effectuer
	P->money-=value;
	P->Pass+=value;
	return 1; //opperation effectuer
}


int augmenterDontPass(player* P,int value){
	if(value>P->money){return 0;} //opperation non effectuer
	P->money-=value;
	P->dontPass+=value;
	return 1; //opperation effectuer
}

int diminuerDontPass(player* P,int value){
	if(value>P->dontPass){return 0;} //opperation non effectuer
	P->money+=value;
	P->dontPass-=value;
	return 1; //opperation effectuer
}

void savePlayer(player* P){
	//ouvertur en ecriture a la fin du fichier
	FILE* fichier = NULL;
	fichier = fopen("playerSave.txt", "a");
	if (fichier != NULL){
		//ecriture de la sauvegarde
		fprintf(fichier,"%s %d\n",P->name,P->money);
		fclose(fichier);
	}
}


int afficheSavePlayer(){ //renvoie le nombre de sauvegarde afficher +1(utiliser pour les menu)
	//ouverture en lecture du fichier de sauvegarde
	FILE* fichier = NULL;
	fichier = fopen("playerSave.txt", "r");
	if (fichier != NULL){
		int i=1;
		int tmpgain;
		char tmpname[25];
		//lis tout le fichier et affiche
		while(fscanf(fichier,"%s %d",tmpname,&tmpgain) != EOF){
			printf("%d : %s avec comme argent %d\n",i,tmpname,tmpgain);
			i++;
		}
		fclose(fichier);
		if(i==1){printf("Pas de joueur sauvegarder\n");}
		return i;
	}
	else{printf("Pas de joueur sauvegarder\n");}
	return 1;
}

void chargePlayer(Liste* L,int n){
	player* P= playerInit();
	//ouverture en lecture du fichier de sauvegarde
	FILE* fichier = NULL;
	fichier = fopen("playerSave.txt", "r");
	if (fichier != NULL){
		int i=1;
		int tmpmoney;
		char tmpname[25];
		//lis tout lee fichier jusqu'a la sauvegarde n
		while(i<=n){
			fscanf(fichier,"%s %d",tmpname,&tmpmoney);
			i++;
		}
		//initilisation du joueur avec la sauvegarde
		strcpy(P->name,tmpname);
		P->money=tmpmoney;
		pushListe(L,P);
		fclose(fichier);
		//supprimer la sauvegarde
		suppSave(n);
	}
}
void suppSave(int n){
	//ouverture en lecture du fichier de sauvegarde
	FILE* fichier = NULL;
	fichier = fopen("playerSave.txt", "r");
	//creation d'un fichier tmp en ecriture
	FILE* tmp = NULL;
	tmp = fopen("tmp.txt","w");

	if (fichier != NULL && tmp!=NULL){
		int i=1;
		int tmpmoney;
		char tmpname[25];
		//lis tout les sauvegarde du fichier des sauvegarde
		while(fscanf(fichier,"%s %d",tmpname,&tmpmoney) != EOF){
			//ecrit toutes les sauvegarde dans le fichier tmp sauf la sauvegarde a enlever
			if(i!=n){fprintf(tmp,"%s %d\n",tmpname,tmpmoney);}
			i++;
		}

	}
	else{printf("Une erreur est survenue\n================================\n");}
	fclose(fichier);
	fclose(tmp);
	//remplacement du fichier playersave par tmp
	remove("playerSave.txt");
	rename("tmp.txt","playerSave.txt");

}


//fonction sur les liste =========================================================================================

Liste* initListe(){
    Liste *L = malloc(sizeof(Liste));
    L->premier = NULL;
    L->nbplayer = 0;
    L->Point = 0;
    L->Phase = 1;
    L->bGain =initBestGain();
    return L;
}

//ajout a la fin de la liste
void pushListe(Liste* L,player* newP){
	L->nbplayer+=1;
	element* newelt=malloc(sizeof(element));
	newelt->P=newP;
	newelt->suivant=NULL;
	//si premier joueur a etre ajouter
	if(L->premier==NULL){
		L->premier=newelt;
	}
	//sinon
	else{
		element* isLast=L->premier;
		while(isLast->suivant!=NULL){
			isLast=isLast->suivant;
		}
		isLast->suivant=newelt;
	}
}


void afficheListe(Liste* L){
	int numplayer=1;
	element* elt=L->premier;
	if(elt!=NULL){
		//on parcour tout les joueur
		while(elt->suivant!=NULL){
			//printf("%d\n",P->money);
			printf("%d : ",numplayer);
			afficheElt(elt);
			elt=elt->suivant;
			numplayer+=1;
		}
		printf("%d : ",numplayer);
		afficheElt(elt);
	}
	else{
		printf("Pas de joueur\n");
	}
}


void supElt(Liste* L,int n){
	if(L->premier==NULL){return;}
	L->nbplayer-=1;
	//si suppression du premier
	if(n==0){
		element* eltasup=L->premier;
		L->premier=eltasup->suivant;
		freeElt(eltasup);
	}

	else{
		element* eltavant=L->premier;
		n-=1;
		//on se deplace jusqua l'element que l'on veut suprimer
		while(n!=0){
			eltavant=eltavant->suivant;
			n-=1;
		}
		//on relie la liste sans elt a sup
		element* eltasup=eltavant->suivant;
		eltavant->suivant=eltasup->suivant;
		//on supprime l'element
		freeElt(eltasup);
	}

}

int estVide(Liste* L){
	if(L->premier==NULL){return 1;}
	return 0;
}

player* selectPlayer(Liste* L,int n){
	element* elt=L->premier;
	//tant que ce n'est pas le joueur voulue on parcour la liste
	while(n!=0){
		elt=elt->suivant;
		n-=1;
	}
	return elt->P;
}


int afficheTirage_ActualiseGain(Liste* L){
	//debut du tirage
	//reinitialisation variable qui nous indique si le joueur a deja doubler ou diviser par 2 ca mise
	resetModifVariable(L);

	int phase = L->Phase;
	int resultatTirage=tirage();
	printf("================================\nResultat du tirage : %d\n",resultatTirage);
	int multPass=0;
	int multDontPass=0;

	//Premiere phase-------------------------
	if(phase==1){
		if(resultatTirage==2){
			printf("Craps\n");
			multDontPass=2;
		}
		else if(resultatTirage==12){
			printf("Craps\n");
			multDontPass=1;
		}
		else if(resultatTirage==7){
			multPass=2;
		}
		else{
			L->Phase+=1;
			L->Point=resultatTirage;
			printf("================================\n\n");
			//passage en phase 2
			return 2;
		}
	}

	//autre phase (point etabli)--------------------------------
	if(phase>=2){

		//dont pass gagne
		if(resultatTirage==7){
			multDontPass=2;
		}

		//point (pass gagne)
		else if(resultatTirage==L->Point){
			multPass=2;
		}

		//jeu suspendu on retir les dé (on peut modifier doubler ou diviser par deux les parie selon les phase)
		else{ 
			printf("================================\n\n");
			L->Phase+=1;
			return 2;
		}
	}

	//traitement des parie en cas de fin de parti;
	element* elt=L->premier;
	int gain=0;
	//pour tout les joueur
	do{
		//on calcule leurs gain
		gain+=gainPass(elt->P,multPass);
		gain+=gainDontPass(elt->P,multDontPass);

		//on affiche leurs gain
		if(gain>0){
			actualiseBestGain(L->bGain,elt->P->name,gain); //mise a jour des meillieur gain si necessaire
			printf("%s a gagner : %d\n",elt->P->name,gain);
		}
		if(gain==0){printf("%s n'a pas gagner ni perdu\n",elt->P->name);}
		if(gain<0){printf("%s a perdu : %d\n",elt->P->name,-gain);}
		gain=0;
		elt=elt->suivant;
	}while(elt!=NULL);
	saveBestGain(L->bGain); //mise a jour du fichier des gain
	printf("Retour au menu Principale\n");
	printf("================================\n\n");
	//retour au menue principale(fin du tour)
	L->Phase=1; //retour a la phase de depart
	return 1;
}

void resetModifVariable(Liste* L){
	if (L->premier==NULL){return;}
	element* elt = L->premier;
	//pour tout les elt
	do{
		elt->P->modifPass=1;
		elt->P->modifDontPass=1;
		elt= elt->suivant;
	}while(elt!=NULL);
}

//fonction sur les element =============================================================================================

void freeElt(element* elt){
	player* P=elt->P;
	free(P->name);
	free(P);
	free(elt);
}

void afficheElt(element* elt){
	player* P=elt->P;
	printf("nom: %s ; argent: %d\n",P->name,P->money);
}


//Les traitement des different menu: ===================================================================================

//interface creation nouveau joueur
player* createNewPlayer(Liste* L){
	player* P=playerInit();
	//set name
	char inputname[25];
	printf("entrer votre nom (24 caractere max): ");
	scanf("%s",inputname);
	playerSetName(P,inputname);
	flush(stdin);

	//set money
	int inputmoney;
	printf("entrer votre montant de depard : ");
	scanf("%d",&inputmoney);
	playerSetMoney(P,inputmoney);
	flush(stdin);
	//add to liste
	pushListe(L,P);
	system("@cls||clear");

}


int traitementMenuPrincipale(Liste* L,int choix){
	//nouveau joueur
	if(choix==1){
		createNewPlayer(L);
		return 1;
	}

	//affiche liste joueur
	else if(choix==2){
		afficheListe(L);
		printf("\n================================\n\n");
		return 1;
	}

	//suppresion joueur
	else if(choix==3){
		printf("Selectionner le joueur a enlever par sont numero :\n");
		afficheListe(L);
		printf("%d : Annuler\nVotre choix :",L->nbplayer+1);
		scanf("%d",&choix);
		flush(stdin);
		system("@cls||clear");
		choix-=1;
		if(choix>=0 && choix < L->nbplayer){supElt(L,choix);}
		printf("\n\n================================\n\n");
		system("@cls||clear");
		return 1;
	}
	//save joueur
	else if(choix==4){
		printf("Selectionner le joueur a sauvegarder par sont numero :\n");
		afficheListe(L);
		printf("%d : Annuler\nVotre choix :",L->nbplayer+1);
		scanf("%d",&choix);
		flush(stdin);
		system("@cls||clear");
		choix-=1;
		if(choix>=0 && choix<=L->nbplayer){
			player* P=selectPlayer(L,choix);
			savePlayer(P);
		}
		else if(choix==L->nbplayer+1){return 1;}
		else{printf("Choix invalide\n================================\n\n");}
		return 1;
	}

	//charger joueur
	else if(choix==5){
		printf("Selectionner le joueur a charger par sont numero :\n");
		int nbsave;
		nbsave=afficheSavePlayer();
		printf("%d : retour au menu principale\n",nbsave);
		printf("Votre choix : ");
		scanf("%d",&choix);
		flush(stdin);
		system("@cls||clear");
		if(choix>=1 && choix<nbsave){chargePlayer(L,choix);}
		else if(choix==nbsave){return 1;}
		else{printf("Choix invalide\n================================\n\n");}
		return 1;
	}
	//commencer tour
	else if(choix==6){
		if(L->premier==NULL){
			printf("Impossible de commencer sans joueur\n================================\n\n");
			return 1; //menu principale
		}
		else{return 2;} //debut du tour
	}
	else if(choix==7){
		afficheBestGain(L->bGain);
		printf("================================\n\n");
		return 1;}

	//fin de la parti
	else if(choix==8){return 0;}
}


int traitementMenuPhase(Liste* L,int choix){

	//menu des parie
	if(choix==1){return 4;}

	//lancer les dés
	else if(choix==2){
		return afficheTirage_ActualiseGain(L);
	}

	//retour menu principale
	if(L->Phase==1){
		if(choix==3){return 1;}
	}
}



int traitementParie(Liste* L,int choix){
	//retour au menu precedent
	if(choix==L->nbplayer){
		system("@cls||clear");
		return 2;
	}
	//information joueur selectionner
	player* P=selectPlayer(L,choix);
	printf("%s, il vous reste %d argent, vos parie actuelle sont:\n",P->name,P->money);
	printf("PassLine : %d\nDon't PassLine: %d\n",P->Pass, P->dontPass);

	if(L->Phase==1){
		//menu modifiaction Phase1
		printf("1 : Modifier le parie de PassLine\n");
		printf("2 : Modifier le parie de Don't PassLine\n");
		printf("3 : Selectionner un autre joueur\n");
		printf("Votre choix : ");
		int choixParie=0;
		scanf("%d",&choixParie);
		flush(stdin);
		printf("\n\n");

		//set Pass
		if(choixParie==1){
			printf("Entrer votre nouveau parie PassLine (-1 pour miser tout votre argent): ");
			int value=0;
			int valide;
			scanf("%d",&value);
			system("@cls||clear");
			flush(stdin);
			valide=setPariePass(P,value);
			if(valide==0){printf("Vous ne disposez pas asser d'argent\n================================\n\n");}
			else if(valide==-1){printf("Somme non valide\n================================\n\n");}
		}

		//set dontpass
		else if(choixParie==2){
			printf("Entrer votre nouveau parie sur Don't PassLine (-1 pour miser tout votre argent): ");
			int value=0;
			int valide;
			scanf("%d",&value);
			system("@cls||clear");
			flush(stdin);
			valide=setParieDontPass(P,value);
			if(valide==0){printf("Valeur trop elever\n================================\n\n");}
			else if(valide==-1){printf("Somme non valide\n================================\n\n");}
		}
		//retour a la selection des joueur
		else if(choixParie==3){
			system("@cls||clear");
			return 4;
		}

		//choix inatandue
		else{
			system("@cls||clear");
			printf("Choix invalide\n================================\n\n");
		}
		return traitementParie(L,choix);
		

	}
	//Phase 2:
	else if(L->Phase==2){

		//menu modifiaction après mise en place du Point
		printf("1 : Augmenter le mise de PassLine\n");
		printf("2 : Diminuer le mise de Don't PassLine\n");
		printf("3 : Selectionner un autre joueur\n");
		printf("Votre choix : ");
		int choixParie=0;
		scanf("%d",&choixParie);
		flush(stdin);
		printf("\n\n");

		//ajouter a Pass
		if(choixParie==1){
			printf("De combien voulez vous ajouter a votre mise sur PassLine: ");
			int value=0;
			int valide;
			scanf("%d",&value);
			system("@cls||clear");
			flush(stdin);
			if(value<0){printf("Vous ne pouver pas entrer de valeur negative\n================================\n\n");}
			else{
				valide=augmenterPass(P,value);
				if(valide==0){printf("Vous ne disposez pas asser d'argent\n================================\n\n");}
			}
		}

		//set dontpass
		else if(choixParie==2){

			printf("De combien voulez vous Diminuer votre mise sur Don't PassLine: ");
			int value=0;
			int valide;
			scanf("%d",&value);
			system("@cls||clear");
			flush(stdin);
			if(value<0){printf("Vous ne pouver pas entrer de valeur negative\n================================\n\n");}
			else{
				valide=diminuerDontPass(P,value);
				if(valide==0){printf("Valeur trop elever\n================================\n\n");}
			}
		}
		//retour a laselection des joueur
		else if(choixParie==3){
			system("@cls||clear");
			return 4;}

		//choix inatandue
		else{
			system("@cls||clear");
			printf("Choix invalide\n================================\n\n");
		}
		return traitementParie(L,choix);
		
	}

	//Phase suspendue
	else{
		//menu modifiaction après suspension (1er suspension multiplier)
		if(L->Phase==3 && P->modifPass==1){printf("1 : Doubler le mise de PassLine\n");}
		if(L->Phase==3 && P->modifDontPass==1){printf("2 : Double le mise de Don't PassLine\n");}
		if(L->Phase!=3 && P->modifPass==1){printf("1 : Diviser le mise de PassLine\n");}
		if(L->Phase!=3 && P->modifDontPass==1){printf("2 : Diviser le mise de Don't PassLine\n");}
		printf("3 : Selectionner un autre joueur\n");
		printf("Votre choix : ");
		int choixParie=0;
		scanf("%d",&choixParie);
		flush(stdin);
		system("@cls||clear");

		//fois 2 pass
		if(L->Phase==3 && choixParie==1 && P->modifPass==1){
			if(P->Pass==0){printf("Impossible car votre mise est de 0 sur PassLine\n================================\n\n");}
			else if(P->Pass > P->money){printf("Impossible vous n'avez pas asser d'argent\n================================\n\n");}
			else{
				P->modifPass=0;
				augmenterPass(P,P->Pass);
			}
		}


		//fois 2 dontpass
		else if(L->Phase==3 && choixParie==2 && P->modifDontPass==1){
			if(P->dontPass==0){printf("Impossible car votre mise est de 0 sur Don't PassLine\n================================\n\n");}
			else if(P->dontPass > P->money){printf("Impossible vous n'avez pas asser d'argent\n================================\n\n");}
			else{
				P->modifDontPass=0;
				augmenterDontPass(P,P->dontPass);}
			
		}

		//diviser 2 pass
		else if(choixParie==1 && P->modifPass==1){
			if(P->Pass==0 || P->Pass==1){printf("Impossible car votre mise est de %d sur PassLine\n================================\n\n",P->Pass);}
			else{
				P->modifPass=0;
				augmenterPass(P,(int)-P->Pass/2);
			}
		}

		//diviser 2 dontpass
		else if(choixParie==2 && P->modifDontPass==1){
			if(P->dontPass==0 || P->dontPass==1){printf("Impossible car votre mise est de %d sur PassLine\n================================\n\n",P->dontPass);}
			else{
				P->modifDontPass=0;
				augmenterDontPass(P,(int)-P->dontPass/2);
			}
			
		}
		//retour a laselection des joueur
		else if(choixParie==3){return 4;}
		else if(choixParie!=1 && choixParie!=2 && choixParie!=3){printf("Choix invalide\n================================\n\n");}
		else if(P->modifDontPass==0 || P->modifPass==0){printf("Cette operation peut s'effectuer une seul fois par tour\n================================\n\n");}
		//choix inatandue
		else{
			printf("Choix invalide\n================================\n\n");
		}
		return traitementParie(L,choix);
		
	}
	
}



//bestGain =======================================================================================
bestGain* initBestGain(){
	bestGain* B=(bestGain*) malloc(sizeof(bestGain)*10);
	int i;
	for(i=0;i<10;i++){
		B[i].name=(char*)malloc(sizeof(char)*25);
		B[i].gain = 0;
	}
	return B;

}


void afficheBestGain(bestGain* B){
	int i=0;
	printf("Tableau des 10 plus gros gain\n");
	while(i<10 && B[i].gain!=0){
		printf("%s : %d\n",B[i].name,B[i].gain);
		i++;
	}
}


void actualiseBestGain(bestGain* B,char* name,int gain){
	int i=0;
	while(i<10 && B[i].gain>gain){
		i++;
	}
	if(i==10){return;}
	int j;
	for(j=9;j>i;j--){
		strcpy(B[j].name,B[j-1].name);
		B[j].gain=B[j-1].gain;
	}
	strcpy(B[i].name,name);
	B[i].gain=gain;

}

void saveBestGain(bestGain* B){
	//ouverture du fichier bestGain.txt en ecriture ou creation si necessaire
	FILE* fichier = NULL;
	fichier = fopen("bestGain.txt", "w");
	int i=0;
	//si ouverture du fichier reussi
	if (fichier != NULL){
		//ecrit les 10 premiere ligne (car stock le top 10) ou s'arrete si il a moins de 10 gain sauvegarder
		while(i<10 && B[i].gain!=0){
			fprintf(fichier,"%s %d\n",B[i].name,B[i].gain);
			i++;
		}
		fclose(fichier);
	}
}

void chargeBestGain(bestGain* B){
	//ouverture du fichier bestGain.txt en lecture
	FILE* fichier = NULL;
	fichier = fopen("bestGain.txt", "r");
	if (fichier != NULL){
		int i=0;
		int tmpgain;
		char tmpname[25];
		//lis tout les 10 premier gain ou moins si ilen a moins
		while(fscanf(fichier,"%s %d",tmpname,&tmpgain) != EOF && i<10){
			//affectation des valeurs lue au tableau des gain
			strcpy(B[i].name,tmpname);
			B[i].name[24]='\0';
			B[i].gain=tmpgain;
			i++;
		}
	}
}