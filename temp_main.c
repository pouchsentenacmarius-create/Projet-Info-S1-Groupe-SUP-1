/*
lien github : https://github.com/pouchsentenacmarius-create/Projet-Info-S1-Groupe-SUP-1/tree/main
*/
/*
./crashTech YEAR_DATA data.csv -> stat d'accident par année
les données seront classifié comme
stat_année tableau[20 année] {
	//par année:
	- nb accident total
		- int nb_accident
		
	- nb de severité:
		- nb_indemne
		- nb_tue
		- nb_blesse_grave
		- nb_blesse_leger
}
	
./crashTech AGE_DATA data.csv -> stat d'accident par age

les données seront classifié comme
stat_age liste(taille non connue) {
	- types de personnes
		- nb_conducteur
		- nb_passager
		- nb_pieton
	- genre
		- nb_masculin
		- nb_feminin
		- nb_autre
}

./crashTech CRASH_DATA data.csv -> stat d'accident liées à "un" accident qui sera recherché par son année et son collision_id
les données seront classifié comme
	l'avl accident -> accident -> liste_vehicule->vehicule -> liste_victime -> victime

(BONUS) ./crashTech PLACE_DU_MORT data.csv
	- int nb_passager_avant_mort 

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// la longueur de l'année ,ex: '2024'-> 4 longueur
#define  anne_len 4 
#define file_name_max_len 100

//declaration des structure  pour la structure victime
typedef enum {conducteur=1,passager=2,pieton=3}_victime_type;
typedef enum {masculin=1,feminin=2,autre=3}_victime_genre;
typedef enum {indemne=1,tue=2,blesse_grave=3,blesse_leger=4}_victime_gravite;
typedef enum {locat_conducteur=1,locat_passager_avant=2,locat_passager_arriere=4,locat_pieton=10,locat_autre=11}_victime_location;

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

int randint(int a,int b){return rand()%(b-a+1)+a;}

int puissance(int base ,int exposant){
	int signe_exposant;
	if(exposant >= 0){
		signe_exposant = 1;
	}
	else{
		signe_exposant = -1;
	}
	exposant = abs(exposant);

	int total = 1;
	for(int i=0;i<exposant;i++){
		if(signe_exposant >= 1){
			total *= base ;
		}
		else{
			total /= base ;
		}
	}
	return total;
}



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
    p_new->victime = victime;
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
   	p_new->p_next = vehicules;
    return p_new;
}

_vehicule* rechercher_vehicule_dans_liste(_chainon_vehicule* vehicules ,int vehicule_id){
	if(!vehicules){
		//printf("erreur le pointeur 'vehicules' dans _vehicule* rechercher_vehicule_dans_liste(...) est nul\n");
		//exit(0);
		return NULL;
	}
	_chainon_vehicule* p = vehicules;
	while(p){
	
		//si on a trouvé le vehicule recherché 
		if(p->vehicule->vehicule_id == vehicule_id){
			return p->vehicule;
		}

		p = p->p_next;
	}
	//si le vehicule recherché n'est pas présent
	return NULL;
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
	
	_vehicule* p_vehicule = rechercher_vehicule_dans_liste(accident->vehicules ,vehicule_id);
	if(p_vehicule)/*verifi si le vehicule recherché est présent dans la liste des vehicules de l'accident*/{
		//printf("%p \n",p_vehicule);
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


int get_char_len(char* c){
	int len = 0;
	while(*c != '\0'){
		len++;
		c++;
	}
	return len;
}

int get_id_collision_len(char* cle){
	return get_char_len(cle + anne_len);
}

int compare_cle(char* cle_A ,char* cle_B)/*return: 1:(A>B) 0:(A==B) -1:(A<B)*/{ 		//"2005--------------------------------\0"
	int len_A = get_id_collision_len(cle_A);
	int len_B = get_id_collision_len(cle_B);
	
	if(!(len_A && len_B)){
		
		printf("erreur de longueur de id_collision dans int compare_cle(...)\n");
		printf("!(%d && %d)\n",len_A ,len_B);
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
			//printf("%d<%c>",cle_A[i] != '0',cle_A[i]);
			if(cle_A[i+anne_len] != '0'){
				//printf("\n");
				//printf("a1\n");
				return 1;
			}
		}printf("\n");
	}
	else if(len_A < len_B){
		for(int i=0;i<taille_diff;i++){
			if(cle_B[i+anne_len] != '0'){
				//printf("b1\n");
				return -1;
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
	for(int i=anne_len;i<taille_min+anne_len;i++){
		if(pA[i] > pB[i]){
			return 1;
		}
		else if(pA[i] < pB[i]){
			return -1;
		}	
	}
	return 0;
	
}

_AVL_accident* AVL_rotation_droite(_AVL_accident* a){
	if(!AVL_a_fils_gauche(a)){
		return a;
	}
	_AVL_accident* p = a->fg;
	a->fg = p->fd;
	p->fd = a;
	
	//mise à jour de l'equilibre
	int eq_a = a->eq;
	int eq_p = p->eq;
	a->eq = eq_a - min(eq_p ,0) + 1;
	p->eq = max(max(eq_a+2 ,eq_a+eq_p+2) ,eq_p+1);
	
	return p;
}

_AVL_accident* AVL_rotation_gauche(_AVL_accident* a){
	if(!AVL_a_fils_droit(a)){
		return a;
	}
	_AVL_accident* p = a->fd;
	a->fd = p->fg;
	p->fg = a;
	
	//mise à jour de l'equilibre
	int eq_a = a->eq;
	int eq_p = p->eq;
	a->eq = eq_a - max(eq_p ,0) - 1;
	p->eq = min(min(eq_a-2 ,eq_a+eq_p-2) ,eq_p-1);
	
	return p;
}

_AVL_accident* AVL_double_rotation_gauche(_AVL_accident* a){
	if(!AVL_a_fils_droit(a)){
		return a;
	}
	a->fd = AVL_rotation_droite(a->fd);
	a = AVL_rotation_gauche(a);
	return a;
}

_AVL_accident* AVL_double_rotation_droite(_AVL_accident* a){
	if(!AVL_a_fils_gauche(a)){
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
	//printf("		cle : %s			eq: %d\n",a->accident->cle ,a->eq);
	if(a->eq <= -2){
		if(a->fg->eq <= 0){
			//printf("			|	rd\n");
			a = AVL_rotation_droite(a);
		}
		else{
			//printf("			|	2rd\n");
			a = AVL_double_rotation_droite(a);
		}
	}
	else if(a->eq >= 2){
		if(a->fd->eq >= 0){
			//printf("			|	rg\n");
			a = AVL_rotation_gauche(a);
		}
		else{
			//printf("			|	2rg\n");
			a = AVL_double_rotation_gauche(a);
		}
	}
	return a;

}

_AVL_accident* insert_AVL_rec(_AVL_accident* a ,_accident* accident ,int* h){
	if(!a){
	    *h = 1;
	    printf("\n");
		return cree_AVL_accident(accident);
	}
	//printf("<%d>\n",compare_cle(a->accident->cle ,accident->cle ));
	switch(compare_cle(accident->cle ,a->accident->cle )){
		default:
			*h = 0;
			return a;
			
		case -1://insert à gauche
			a->fg = insert_AVL_rec(a->fg ,accident ,h);
			*h *= -1;
			break;
			
		case 1://insert à droite
			a->fd = insert_AVL_rec(a->fd ,accident ,h);
			*h *= 1;
			break;
	}
	if((*h)){
		a->eq += *h;
		a = equilibrer_AVL(a);
		*h = a->eq != 0;
	}
	return a;
}

_AVL_accident* insert_AVL(_AVL_accident* a ,_accident* accident){
	int h = 1;
	//printf("    || <%s> <%p>\n",accident->cle ,accident->cle);
	return insert_AVL_rec(a ,accident ,&h);
}


void print_victime(_victime* victime){
	if(!victime){
		printf("                     |victime : NULL\n");
		printf("                     |________\n");
		return ;
	}
	printf("                     |victime (%p):\n",victime);
	printf("                     |       type	:%d\n",victime->type);
	printf("                     |       genre	:%d\n",victime->genre);
	printf("                     |       location:%d\n",victime->location);
	printf("                     |       age		:%d ans\n",victime->age);
	printf("                     |_______gravite	:%d\n",victime->gravite);

}

void print_victime_liste(_chainon_victime* victimes){
	if(!victimes){
		printf("                     |victimes : <empty>\n");
		printf("                     |________\n");
		return ;
	}
	
	print_victime(victimes->victime);
	
	_chainon_victime* p = victimes->p_next;
	while(p){
		printf("                          ||\n");
		printf("                          ||\n");
		printf("                          \\/\n");
		print_victime(p->victime);
		p = p->p_next;
	}
}

void print_vehicule(_vehicule* vehicule){
	if(!vehicule){
		printf("              |vehicule : NULL\n");
		printf("              |________\n");
		return ;
	}
	printf("              |vehicule (%p):\n",vehicule);
	printf("              |       id      :%d\n",vehicule->vehicule_id);
	printf("              |       purpose	:%d\n",vehicule->purpose);
	printf("              |_______victimes:\n");
	print_victime_liste(vehicule->victimes);
	
}

void print_vehicule_liste(_chainon_vehicule* vehicules){
	if(!vehicules){
		printf("              |vehicules : <empty>\n");
		printf("              |________\n");
		return ;
	}
	
	print_vehicule(vehicules->vehicule);
	
	_chainon_vehicule* p = vehicules->p_next;
	while(p){
		printf("                 ||\n");
		printf("                 ||\n");
		printf("                 \\/\n");
		print_vehicule(p->vehicule);
		p = p->p_next;
	}
}

void print_accident(_accident* accident){
	if(!accident){
		printf("      |accident : NULL\n");
		return ;
	}
	printf("      |accident(%p):\n",accident);
	printf("      |           cle:%s\n",accident->cle);
	printf("      |_____vehicules:\n");
	print_vehicule_liste(accident->vehicules);
}

void print_noeud_AVL_accident(_AVL_accident* a){
	if(!a){
		printf("|noeud_AVL : NULL\n");
		return ;
	}
	printf("|noeud_AVL(%p):\n",a);
	printf("|     eq:%d\n",a->eq);
	printf("|     fg: %p			|_fg: %p\n",a->fg,a->fd);
	printf("|_____accident:\n",a->accident);
	print_accident(a->accident);
}

void print_AVL_accident_suffixe(_AVL_accident* a){
		if(AVL_a_fils_gauche(a)){
			print_AVL_accident_suffixe(a->fg);	
			printf("\n");
		}
		if(AVL_a_fils_droit(a)){
			print_AVL_accident_suffixe(a->fd);
			printf("\n");
		}
		print_noeud_AVL_accident(a);	
		printf("\n");
}

void print_AVL_accident_prefixe(_AVL_accident* a){
		print_noeud_AVL_accident(a);	
		printf("\n");
		if(AVL_a_fils_gauche(a)){
			print_AVL_accident_prefixe(a->fg);
			printf("\n");	
		}
		if(AVL_a_fils_droit(a)){
			print_AVL_accident_prefixe(a->fd);	
			printf("\n");
		}
}

void print_AVL_accident_interfixe(_AVL_accident* a){
		if(AVL_a_fils_gauche(a)){
			print_AVL_accident_interfixe(a->fg);	
			printf("\n");
		}
		print_noeud_AVL_accident(a);	
		printf("\n");
		if(AVL_a_fils_droit(a)){
			print_AVL_accident_interfixe(a->fd);
			printf("\n");	
		}
}

_AVL_accident* rechercher_accident_dans_AVL(_AVL_accident* a ,char* cle){
    if(!a){
        return NULL;    
    }
    int nb = compare_cle(cle ,a->accident->cle);
    //printf(" %d     <%s> <%s> ",nb ,cle ,a->accident->cle);
    switch(nb){
        case -1: //recherche à gauche
            //printf("g\n");
            return rechercher_accident_dans_AVL(a->fg ,cle);
        case 1: // recherche à droite
            //printf("d\n");
            return rechercher_accident_dans_AVL(a->fd ,cle);
        default: // l'AVL de l'accident recherché est trouvé
            //printf("");
            return a;
    }
}



char* cree_cle(char* annee ,char* collision_id){
	int collision_id_len = get_char_len(collision_id); //anne_len
	int taille_cle = collision_id_len + anne_len +1;
	char* new = NULL;
	new = malloc(taille_cle*sizeof(char));
	exit_if_null(new);
	
	for(int i=0;i<anne_len;i++){
		new[i] = annee[i];
	}
	
	for(int i=0;i<collision_id_len;i++){
		new[i+anne_len] = collision_id[i];
	}
	new[taille_cle-1] = '\0';
	
	return new;
}

/*utile pour debugger le code*/
char* rand_char_generator(int len){
	char* new = NULL;
	len++;
	new = malloc((len)*sizeof(char)); 
	exit_if_null(new);
	
	for(int i=0;i<len-1;i++){
		new[i] = randint('0','9');
	}
	new[len] = '\0';
	//printf("a <%p> <%s> \n",new,new);
	return new;
}



_AVL_accident* getDataFromFile(const char* file_name){

    _AVL_accident* data_AVL = NULL;

    char temp_year[10];
    char temp_collision_id[30];
    char temp_birth_year[10];

    
    _victime_location temp_location;
    _victime_type temp_user_type;
    _victime_gravite temp_severity;
    _victime_genre temp_gender;
    int temp_age = 0;
    
    _vehicule_purpose temp_purpose;
	int temp_vehicul_id;


    _AVL_accident* temp_AVL = NULL;
    _accident* temp_accident = NULL;
    _vehicule* temp_vehicule = NULL;
    _victime* temp_victime = NULL;

    FILE* f = fopen(file_name, "r");

    if(f == NULL){
        perror("Erreur ouverture fichier");
        return NULL;
    }

    // Ignorer la première ligne (en-tête)
    char header[256];

    if(fgets(header, sizeof(header), f) == NULL){
        fclose(f);
        return NULL;
    }

    int e;

    while(1){

        e = fscanf(f,
            " %9[^;];%29[^;];%d;%d;%d;%d;%d;%9[^;];%d",
            temp_year,
            temp_collision_id,
            &temp_vehicul_id,
            &temp_location,
            &temp_user_type,
            &temp_severity,
            &temp_gender,
            temp_birth_year,
            &temp_purpose
        );
	/*
		printf("<e==%d> %s;%s;%d;%d;%d;%d;%d;%s;%d\n",e
    	,temp_year
    	,temp_collision_id
    	,temp_vehicul_id
    	,temp_location
    	,temp_user_type
    	,temp_severity
    	,temp_gender
    	,temp_birth_year
    	,temp_purpose
    	);
    	*/
    	
        // Vérification de la lecture
        if(e == EOF){
            break;
        }

        if(e != 9){
            printf("Erreur de lecture d'une ligne : %d champs lus\n", e);
            break;
        }
        
        
        // Recherche de l'accident dans l'AVL
        temp_AVL = rechercher_accident_dans_AVL(data_AVL,cree_cle(temp_year, temp_collision_id));
        if(!temp_AVL){
        	 // Création de l'accident
        	temp_accident = cree_accident( cree_cle(temp_year, temp_collision_id)  );
        	data_AVL = insert_AVL(data_AVL, temp_accident);
        }
        else{
        	temp_accident = temp_AVL->accident;
        }
        
        
        // Recherche du véhicule
        temp_vehicule = rechercher_vehicule_dans_liste(temp_accident->vehicules ,temp_vehicul_id);

        if(!temp_vehicule){
            // Création du véhicule
            temp_vehicule = cree_vehicule(temp_vehicul_id, temp_purpose);
            ajout_vehicule_a_accident(temp_accident ,temp_vehicule);
        }
        temp_age = atoi(temp_year) - atoi(temp_birth_year);
        temp_victime = cree_victime(temp_user_type ,temp_gender ,temp_age ,temp_severity ,temp_location);
        temp_vehicule = ajout_victime_a_vehicule(temp_vehicule ,temp_victime);
    }
	
    fclose(f);

    return data_AVL;
}

/*
do {
		error = scanf(" %c",&menu);
		// savoir si le choix de menu est correct en fonction du menu actuelle
		menu_choice_correct = ((current_menu == select_menu)&&((	(((menu=='s')||(menu=='d'))&&(current_game->play==1))||((menu=='j')&&(savefile))	)||(menu=='f')||(menu=='g')||(menu=='h'))) || ((current_menu != select_menu)&&((menu=='h')||(menu=='q')));  //savoir si le menu choisi est correct en fonction du menu actuel
		if(!error){
		    printf("input went wrong,flushing scanf()\n");
		    do{
		    }while(!scanf("%c"),&menu);
		}
		if(!menu_choice_correct) {
			printf("Le menu choisi est incorrect,veuillez recommencez\n");
		}
	} while(!menu_choice_correct);
	
	*/
	
	
void flush_scanf(){
	char c ;
	//int e = 0;
	do{
		//printf("flush >>>>%c\n",c);
	}while(!scanf("%c",&c));

}

void fonction_CRASH_DATA( _AVL_accident* data_AVL){
	if(!data_AVL){
		printf("il y a aucun donnée dans data_AVL pour void fonction_CRASH_DATA(..)\n");
		exit(0);
 	}
 	
 	while(1){
 	
 	char temp_year[4];
 	char temp_collision_id[30];
 	char* cle = NULL;
 	
 	int e = 0;
 	
 	//flush_scanf();
 	while(!e){
 		printf("Veuillez entrez l'année de l'accident\n");
 		
 		e = scanf(" %4s",temp_year);
 		printf("e==%d ,temp_year==%s\n",e,temp_year);
 		if(!e){
 			flush_scanf();
 			printf("erreur ,veuillez recommencez\n");
 		}
 	}
 	flush_scanf();
 	e = 0;
 	while(!e){
 		printf("Veuillez entrez l'id de l'accident\n");
 		
 		e = scanf(" %29s",temp_collision_id);
 		if(!e){
 			flush_scanf();
 			printf("erreur ,veuillez recommencez\n");
 		}
 	}

	cle = cree_cle(temp_year ,temp_collision_id);
	printf("\ncle : %s\n\n\n",cle);
	
 	_AVL_accident* accident_recherher = rechercher_accident_dans_AVL(data_AVL ,cle);
 	
 	if(!accident_recherher){
 		printf("aucun accident coorespondant a été trouvé pour la clé:%s\n",cle);
 	}
 	else{
 		print_accident(accident_recherher->accident);
 	}
 	
 	}
}


void main(){

        for(int i=0;i<500;i++){
		printf("\n");
	}



	printf("startig program\n\n");

	
                                     
	//srand(time(NULL));
/*
	_AVL_accident* a = NULL;
	_accident* accident = NULL;
	int nb_accident = 1000; 
	int nb_vehicule = 0; 
	int nb_victime = 0;
	for(int i=0;i<nb_accident;i++){
		accident = cree_accident(rand_char_generator(10));
		
		for(int i=0;i<nb_vehicule;i++){
			accident = ajout_vehicule_a_accident(accident ,cree_vehicule(i,0));
		}
		
		for(int i=0;i<nb_victime;i++){
			accident = ajout_victime_a_accident(accident ,randint(0,nb_vehicule-1) ,cree_victime(randint(1,3) ,randint(1,3) ,randint(0,100) ,randint(0,4) ,randint(1,2)));
		}
		
		a = insert_AVL(a ,accident);
	}
	
	print_AVL_accident_suffixe(a);
	//print_noeud_AVL_accident(a);

	


    printf("\n%p\n",rechercher_accident_dans_AVL(a ,"5547656938\0"));
*/
    const char file_name[file_name_max_len] = "cy_crash_tech_big_shuffled.csv\0";
    //const char file_name[file_name_max_len] = "cy_crash_tech_small_shuffled.csv\0";
    
    printf("<%s>\n",cree_cle("2021\0","00123456789\0"));
    
    _AVL_accident* a = NULL;
    a = getDataFromFile(file_name);
    print_AVL_accident_suffixe(a);
	fonction_CRASH_DATA(a);



	printf("program ended\n");
	
	for(int i=0;i<0;i++){
		printf("\n");
	}
}
