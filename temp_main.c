/*
lien github : https://github.com/pouchsentenacmarius-create/Projet-Info-S1-Groupe-SUP-1/tree/main
*/
/*
./crashTech YEAR_DATA data.csv -> stat d'accident par année

./crashTech AGE_DATA data.csv -> stat d'accident par age

./crashTech CRASH_DATA data.csv -> stat d'accident liées à "un" accident qui sera recherché par son année et son collision_id
*/

#include <stdio.h>
#include <stdlib.h>

// la longueur de l'année ,ex: '2024'-> 4 longueur
#define  anne_len 4 

//declaration des structure  pour la structure victime
typedef enum {conducteur=1,passager=2,pieton=3}_victime_type;
typedef enum {masculin=1,feminin=2,autre=3}_victime_genre;
typedef enum {indemne=1,tue=2,blesse_grave=3,blesse_leger=4}_victime_gravite;
typedef enum {locat_conducteur=1,locat_passager_avant=2,locat_pieton=10,locat_autre=11}_victime_location;

typedef struct{
	_victime_type type ;
	_victime_genre genre ;
	_victime_location location ;
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
    _chainon_vehicule* vehicules; //pointe sur le début de la liste chainé des véhicules lors de l'accident
   
}_accident;

//declaration de la structure pour L'AVL pour les accidents
typedef struct _AVL_Accident{
	_accident* accident;
	int eq; // l'equilibre

	struct _AVL_Accident* fg;
	struct _AVL_Accident* fd;

}_AVL_accident;




void exit_if_null(void* p){if(!p){printf("got null pointer\n");exit(0);}}//utile après les allocations de memoire
int abs(int a){return a * (-(a<0));}

int max(int a ,int b){if(a>b){return a;}return b;}
int min(int a ,int b){if(a<b){return a;}return b;}


_victime* cree_victime(_victime_type type ,_victime_genre genre ,int age ,_victime_gravite gravite ,_victime_location location){
    //allocation
    _victime* p_new_victime = NULL;
    p_new_victime = malloc(sizeof(_victime));
    exit_if_null(p_new_victime);    
   
    //initialisation
    p_new_victime->type = type;
    p_new_victime->genre = genre;
    p_new_victime->age = age;
    p_new_victime->gravite = gravite;
    p_new_victime->location = location;
   
    return p_new_victime;
}

_chainon_victime* cree_chainon_victime(_victime* victime){
    //allocation
    _chainon_victime* p_new_chainon = NULL;
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
	//allocation
   _vehicule* p_new_vehicule = NULL;
    p_new_vehicule = malloc(sizeof(_vehicule));
    exit_if_null(p_new_vehicule);
    
    //initialisation
    p_new_vehicule->vehicule_id = vehicule_id;
    p_new_vehicule->purpose = purpose;
    p_new_vehicule->victimes = NULL;
    
    return p_new_vehicule;
}

_vehicule* ajout_victime_a_vehicule(_vehicule* vehicule, _victime* victime){
	//verifi si 'vehicule' et 'victime' est bien défini
	if(!vehicule){
		printf("erreur le pointeur 'vehicule' dans _vehicule* ajout_victime_a_vehicule(...) est nul\n");
		exit(0);
	}
	else if(!victime){
		printf("erreur le pointeur 'victime' dans _accident* ajout_victime_a_vehicule(...) est nul\n");
		exit(0);
	}
	
	//ajoute la victime à la liste de victime du vehicule
	vehicule->victimes = ajout_liste_victime(vehicule->victimes ,victime);
	return vehicule;
}

_chainon_vehicule* cree_chainon_vehicule(_vehicule* vehicule){
    //allocation
    _chainon_vehicule* p_new_chainon = NULL;
    p_new_chainon = malloc(sizeof(_chainon_vehicule));
    exit_if_null(p_new_chainon);
   
    //initialisation
    p_new_chainon->p_next = NULL;
    p_new_chainon->vehicule = vehicule;
   
    return p_new_chainon;
   
}

_chainon_vehicule* ajout_liste_vehicule(_chainon_vehicule* vehicules ,_vehicule* vehicule){//ajout un chainon vehicule au debut de la liste vehicule
    _chainon_vehicule* p_new = cree_chainon_vehicule(vehicule);
    p_new->vehicule = vehicule;
    return p_new;
}

int rechercher_vehicule_dans_liste(_chainon_vehicule* vehicules ,int vehicule_id ,_vehicule* vehicule){
	_chainon_vehicule* p = vehicules;
	while(p){
	
		//si on a trouvé le vehicule recherché 
		if(p->vehicule->vehicule_id == vehicule_id){
		
			vehicule->vehicule_id = p->vehicule->vehicule_id;
			vehicule->purpose = p->vehicule->purpose;
			vehicule->victimes = p->vehicule->victimes;
			
			vehicule = p->vehicule;
			return 1;
		}

		p = p->p_next;
	}
	//si le vehicule recherché n'est pas présent
	return 0;
}


_accident* cree_accident(char* cle){
	if(!cle){
		printf("erreur le pointeur 'cle' dans _accident* cree_accident(...) est nul\n");
		exit(0);
	}
	
	//allocation
	_accident* p_new_accident = NULL;
    p_new_accident = malloc(sizeof(_accident));
    exit_if_null(p_new_accident);
    
    //initialisation
    p_new_accident->cle = cle;
    p_new_accident->vehicules = NULL;

    return p_new_accident;
}

_accident* ajout_vehicule_a_accident(_accident* accident ,_vehicule* vehicule){
	//verifi si 'accident' et 'vehicule' est bien défini
	if(!accident){
		printf("erreur le pointeur 'accident' dans _accident* ajout_vehicule_a_accident(...) est nul\n");
		exit(0);
	}
	else if(!vehicule){
		printf("erreur le pointeur 'vehicule' dans _accident* ajout_vehicule_a_accident(...) est nul\n");
		exit(0);
	}
	
	//ajoute le vehicule à la liste des vehicules de accident
	accident->vehicules = ajout_liste_vehicule(accident->vehicules ,vehicule);
	return accident;
}



_accident* ajout_victime_a_accident(_accident* accident ,int vehicule_id ,_victime* victime){
	//verifi si 'accident' et 'victime' est bien défini
	if(!accident){
		printf("erreur le pointeur 'accident' dans _accident* ajout_victime_a_accident(...) est nul\n");
		exit(0);
	}
	else if(!victime){
		printf("erreur le pointeur 'victime' dans _accident* ajout_victime_a_accident(...) est nul\n");
		exit(0);
	}
	
	_vehicule* p_vehicule = NULL;
	if(rechercher_vehicule_dans_liste(accident->vehicules ,vehicule_id ,p_vehicule))/*verifi si le vehicule recherché est présent dans la liste des vehicules de l'accident*/{
		p_vehicule->victimes = ajout_liste_victime(p_vehicule->victimes ,victime);
	}
	else{
		printf("erreur le vehicule recherché est présent dans la liste des vehicules de l'accident dans _accident* ajout_victime_a_accident(...)\n");
		exit(0);
	}	
	
	return accident;
}

_AVL_accident* cree_AVL_accident(_accident* accident){
	if(!accident){
		printf("erreur le pointeur 'accident' dans _accident* cree_AVL_accident(...) est nul\n");
		exit(0);
	}
	
	//allocation
	_AVL_accident* p_new_AVL = NULL;
    p_new_AVL = malloc(sizeof(_AVL_accident));
    exit_if_null(p_new_AVL);
    
    //initialisation
    p_new_AVL->accident = accident;
    p_new_AVL->eq = 0;
    p_new_AVL->fg = NULL;
    p_new_AVL->fd = NULL;

    return p_new_AVL;
}

int	AVL_est_feuille(_AVL_accident* a){
	if(!a){
		return 0;
	}
	return !(a->fg || a->fd);
}

int	AVL_a_fils_gauche(_AVL_accident* a){
	if(!a){
		return 0;
	}
	return a->fg != NULL;
}

int	AVL_a_fils_droit(_AVL_accident* a){
	if(!a){
		return 0;
	}
	return a->fd != NULL;
}

int get_id_collision_len(char* cle){
	char* p = cle;
	p += anne_len;
	int len = 0;
	while(*p != '\0'){
		len++;
		p++;
	}
	return len;
}

int compare_cle(char* cle_A ,char* cle_B)/*return: 1(A>B) 0(A==B) -1(A<B)*/{ 		//"2005--------------------------------\0"
	int len_A = get_id_collision_len(cle_A);
	int len_B = get_id_collision_len(cle_A);
	if(!(len_A && len_B)){
		printf("erreur de longueur de id_collision dans int compare_cle(...)\n");
		exit(0);
	}
	
	//conmparé les années
	for(int i=0;i<anne_len;i++){
		if(cle_A[i] < cle_B[i]){
			return 1;
		}
		else if(cle_A[i] > cle_B[i]){
			return -1;
		}
	}
	
	//conmparé les id_collision
	int taille_diff =  max(len_A ,len_B) - min(len_A ,len_B);
	
	if(len_A > len_B){
		for(int i=0;i<taille_diff;i++){
			if(cle_A[i] != '0'){
				return 1;
			}
		}
	}
	else if(len_A < len_B){
		for(int i=0;i<taille_diff;i++){
			if(cle_B[i] != '0'){
				return 1;
			}
		}
	}
	
	char* pA = cle_A;
	char* pB = cle_B; 
	if(len_A > len_B){
		pA += taille_diff;
	}
	else if(len_A < len_B){
		pB += taille_diff;
	}
	
	int taille_min = min(len_A ,len_B);
	for(int i=0;i<taille_min;i++){
		if(cle_A[i] > cle_B[i]){
			return 1;
		}
		else if(cle_A[i] < cle_B[i]){
			return -1;
		}	
	}
	return 0;
	
}


_AVL_accident* insert_AVL_rec(_AVL_accident* a ,_accident* accident ,int* h){
	if(!a){
		return cree_AVL_accident(accident);
	}
	
	switch(compare_cle(a->accident->cle ,accident->cle)){
		default:
			*h = 0;
			return a;
			
		case 1://insert à gauche
			a->fg = insert_AVL_rec(a->fg ,accident ,h);
			*h *= -1;
			break;
			
		case -1://insert à droite
			a->fd = insert_AVL_rec(a->fd ,accident ,h);
			//*h *= 1;
			break;
	}
	if(*h){
		a->eq += *h;
		*h = a->eq != 0;
	}
	return a;
}

_AVL_accident* insert_AVL(_AVL_accident* a ,_accident* accident){
	int h = 1;
	return insert_AVL_rec(a ,accident ,&h);
}
	
/*
int	AVL_a_fils_gauche(_AVL_accident* a)
int	AVL_a_fils_droit(_AVL_accident* a)
*/

_AVL_accident* AVL_rotation_droite(_AVL_accident* a){
	if(!AVL_a_fils_droit(a)){
		return a;
	}
	_AVL_accident* p = a->fd;
	a->fd = p->fg;
	p->fg = a;
	
	//mise à jour de l'equilibre
	int eq_a = a->eq;
	int eq_p = p->eq;
	a->eq = eq_a - min(eq_p ,0) + 1;
	p->eq = max(max(eq_a+2 ,eq_a+eq_p+2) ,eq_p+1);
	
	return p;
}

_AVL_accident* AVL_rotation_gauche(_AVL_accident* a){
	if(!AVL_a_fils_gauche(a)){
		return a;
	}
	_AVL_accident* p = a->fg;
	a->fg = p->fd;
	p->fd = a;
	
	//mise à jour de l'equilibre
	int eq_a = a->eq;
	int eq_p = p->eq;
	a->eq = eq_a - max(eq_p ,0) - 1;
	p->eq = min(min(eq_a-2 ,eq_a+eq_p-2) ,eq_p-1);
	
	return p;
}

_AVL_accident* AVL_douvle_rotation_gauche(_AVL_accident* a){
	if(!a){
		return a;
	}
	a->fd = AVL_rotation_droite(a->fd);
	a = AVL_rotation_gauche(a);
	return a;
}

_AVL_accident* AVL_douvle_rotation_droite(_AVL_accident* a){
	if(!a){
		return a;
	}
	a->fg = AVL_rotation_gauche(a->fg);
	a = AVL_rotation_droite(a);
	return a;
}

_AVL_accident* equilibrer_AVL(_AVL_accident* a){
	if(!a){
		return a;
	}
	if(a->eq <= -2){
		if(a->fg->eq <= 0){
			a = AVL_rotation_droite(a);
		}
		else{
			a = AVL_douvle_rotation_droite(a);
		}
	}
	else if(a->eq >= 2){
		if(a->fd->eq >= 0){
			a = AVL_rotation_gauche(a);
		}
		else{
			a = AVL_douvle_rotation_gauche(a);
		}
	}
	return a;

}





void main(){









}
