/*
./crashTech YEAR_DATA data.csv -> stat d'accident par année

./crashTech AGE_DATA data.csv -> stat d'accident par age

./crashTech CRASH_DATA data.csv -> stat d'accident liées à "un" accident qui sera recherché par son année et son collision_id
*/

#include <stdio.h>
#include <stdlib.h>

//declaration des structure  pour la structure victime
typedef enum {conducteur=1,passager=2,pieton=3}_victime_type;
typedef enum {masculin=1,féminin=2,autre=3}_victime_genre;
typedef enum {indemne=1,tue=2,blesse_grave=3,blesse_leger=4}_victime_gravite;

typedef struct{
	_victime_type type ;
	_victime_genre genre ;
	int age;
_victime_gravite gravite;

}_victime;

//declaration de la structure pour la liste chainé de structure victime
typedef struct _Chainon_victime{
    struct _Chainon_victime* p_next;
    _victime* victime;
}_chainon_victime;

//declaration des structure pour la structure vehicule
typedef enum {non_renseigne=0,travail=1,ecole=2,courses=3,deplacement_professionnel=4,loisirs=5,autres=6}_vehicule_purpose;

typedef struct{
	int vehicule_id;
	_vehicule_purpose purpose;
	_chainon_victime* victimes;//pointe sur le début de la liste chainé des victimes lors de l'accident

}_vehicule;

//declaration de la structure pour la liste chainé de structure victime
typedef struct _Chainon_vehicule{
    struct _Chainon_vehicule* p_next;
    _vehicule* vehicule;
}_chainon_vehicule;

//declaration de la structure accident
typedef struct{
    char* cle; // année + collision_id    ,char* supoosons que on peut avoir une grande variation d'anné dans le fichier ex: l'année 14025847
    _chainon_vehicule* tete_vehicule; //pointe sur le début de la liste chainé des véhicules lors de l'accident
   
}_accident;

//declaration de la structure pour L'AVL pour les accidents
typedef struct _AVL_Accident{
	_accident* accident;
	int eq; // l'equilibre

	struct _Accident* fg;
	struct _Accident* fd;

}_AVL_accident;




void exit_if_null(void* p){if(!p){printf("got null pointer\n");exit(0);}}//utile après les allocations de memoire




_victime* cree_victime(_victime_type type ,_victime_genre genre ,int age ,_victime_gravite gravite){
    //allocation
    p_new_victime = NULL;
    p_new_victime = malloc(sizeof(_victime));
    exit_if_null(p_new_victime);    
   
    //initialisation
    p_new_victime->type = type;
    p_new_victime->genre = genre;
    p_new_victime->age = age;
    p_new_victime->gravite = gravite;
   
    return p_new_victime;
}

_chainon_victime* cree_chainon_victime(_victime* victime){
    //allocation
    p_new_chainon = NULL;
    p_new_chainon = malloc(sizeof(_chainon_victime));
    exit_if_null(p_new_chainon);
   
    //initialisation
    p_new_chainon->p_next = NULL;
    p_new_chainon->victime = victime;
   
    return p_new_chainon;
   
}

_chainon_victime* ajout_liste_victime(_chainon_victime* victimes ,_victime* victime){//ajout un chainon victime au debut de la liste victime
    _chainon_victime* p_new = cree_chainon_victime(victime);
    p_new->p_next = victimes;
    return p_new;
}



_vehicule* cree_vehicule(int vehicule_id ,_vehicule_purpose purpose){
   
}



void main(){









}
