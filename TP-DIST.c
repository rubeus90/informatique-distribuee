/* T. Grandpierre - Application distribue'e pour TP IF4-DIST 2004-2005

But : 

fournir un squelette d'application capable de recevoir des messages en 
mode non bloquant provenant de sites connus. L'objectif est de fournir
une base pour implementer les horloges logique/vectorielle/scalaire, ou
bien pour implementer l'algorithme d'exclusion mutuelle distribue'

Syntaxe :
         arg 1 : Numero du 1er port
	 arg 2 et suivant : nom de chaque machine

--------------------------------
Exemple pour 3 site :

Dans 3 shells lances sur 3 machines executer la meme application:

pc5201a>./dist 5000 pc5201a.esiee.fr pc5201b.esiee.fr pc5201c.esiee.fr
pc5201b>./dist 5000 pc5201a.esiee.fr pc5201b.esiee.fr pc5201c.esiee.fr
pc5201c>./dist 5000 pc5201a.esiee.fr pc5201b.esiee.fr pc5201c.esiee.fr

pc5201a commence par attendre que les autres applications (sur autres
sites) soient lance's

Chaque autre site (pc5201b, pc5201c) attend que le 1er site de la
liste (pc5201a) envoi un message indiquant que tous les sites sont lance's


Chaque Site passe ensuite en attente de connexion non bloquante (connect)
sur son port d'ecoute (respectivement 5000, 5001, 5002).
On fournit ensuite un exemple permettant 
1) d'accepter la connexion 
2) lire le message envoye' sur cette socket
3) il est alors possible de renvoyer un message a l'envoyeur ou autre si
necessaire 

*/


#include <stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<fcntl.h>
#include<netdb.h>
#include<string.h>

typedef struct{
  char* nomSite;
  int position;
  int estampille;
} Info;

int GetSitePos(int Nbsites, char *argv[]) ;
void WaitSync(int socket);
void SendSync(char *site, int Port);

/*Identification de ma position dans la liste */
int GetSitePos(int NbSites, char *argv[]) {
  char MySiteName[20]; 
  int MySitePos=-1;
  int i;
  gethostname(MySiteName, 20);
  for (i=0;i<NbSites;i++) 
    if (strcmp(MySiteName,argv[i+2])==0) {
      MySitePos=i;
      //printf("L'indice de %s est %d\n",MySiteName,MySitePos);
      return MySitePos;
    }
  if (MySitePos == -1) {
    printf("Indice du Site courant non trouve' dans la liste\n");
    exit(-1);
  }
  return (-1);
}


/*Attente bloquante d'un msg de synchro sur la socket donne'e*/
void WaitSync(int s_ecoute) {
  char texte[40];
  int l;
  int s_service;
  struct sockaddr_in sock_add_dist;
  int size_sock;
  size_sock=sizeof(struct sockaddr_in);
  printf("WaitSync : ");fflush(0);
  s_service=accept(s_ecoute,(struct sockaddr*) &sock_add_dist,&size_sock);
  l=read(s_service,texte,39);
  texte[l] ='\0';
  printf("%s\n",texte); fflush(0);
  close (s_service);
} 

/*Envoie d'un msg de synchro a la machine Site/Port*/
void SendSync(char *Site, int Port) {
  struct hostent* hp;
  int s_emis;
  char chaine[15];
  int longtxt;
  struct sockaddr_in sock_add_emis;
  int size_sock;
  int l;
  
  if ( (s_emis=socket(AF_INET, SOCK_STREAM,0))==-1) {
    perror("SendSync : Creation socket");
    exit(-1);
  }
    
  hp = gethostbyname(Site);
  if (hp == NULL) {
    perror("SendSync: Gethostbyname");
    exit(-1);
  }

  size_sock=sizeof(struct sockaddr_in);
  sock_add_emis.sin_family = AF_INET;
  sock_add_emis.sin_port = htons(Port);
  memcpy(&sock_add_emis.sin_addr.s_addr, hp->h_addr, hp->h_length);
  
  if (connect(s_emis, (struct sockaddr*) &sock_add_emis,size_sock)==-1) {
    perror("SendSync : Connect");
    exit(-1);
  }
     
  sprintf(chaine,"**SYNCHRO**");
  longtxt =strlen(chaine);
  /*Emission d'un message de synchro*/
  l=write(s_emis,chaine,longtxt);
  close (s_emis); 
}

void envoyer_message(char *Site, int Port, char* message, Info info) {
  struct hostent* hp;
  int s_emis;
  char chaine[30];
  int longtxt;
  struct sockaddr_in sock_add_emis;
  int size_sock;
  int l;
  
  if ( (s_emis=socket(AF_INET, SOCK_STREAM,0))==-1) {
    perror("Demande : Creation socket");
    exit(-1);
  }
    
  hp = gethostbyname(Site);
  if (hp == NULL) {
    perror("Demande: Gethostbyname");
    exit(-1);
  }

  size_sock=sizeof(struct sockaddr_in);
  sock_add_emis.sin_family = AF_INET;
  sock_add_emis.sin_port = htons(Port);
  memcpy(&sock_add_emis.sin_addr.s_addr, hp->h_addr, hp->h_length);
  
  if (connect(s_emis, (struct sockaddr*) &sock_add_emis,size_sock)==-1) {
    perror("Demande : Connect");
    exit(-1);
  }
     
  sprintf(chaine,"%s %d %d", message, info.position, info.estampille);

  longtxt =strlen(chaine);
  /*Emission d'un message de synchro*/
  l=write(s_emis,chaine,longtxt);
  close (s_emis);
} 

/*******************************************************************/
/********** Gestion de la queue ************************************/

Info derniereValeurQueue(int max, Info* tabInfo){
  return tabInfo[max-1];
}

void enleverQueue(int* max, Info* tabInfo){
  *max -= 1;
}

void ajouterQueue(int* max, Info* tabInfo, Info valeur){
  tabInfo[*max] = valeur;
  *max += 1; 
}

void afficherQueue(Info* tabInfo, int max){
  int i;
  Info info;

  printf("------------------------------\n");
  printf("La liste d'attente : \n");

  for(i=0; i< max; i++){
    info = tabInfo[i];
    printf("La position %d contient la machine %d avec l'estampille = %d\n", i, info.position, info.estampille);
  }

  printf("------------------------------\n");
}

void permute(Info *x, Info *y) {
  Info tmp;
  tmp = *x;
  *x = *y;
  *y = tmp;
}
 
int tri_bulle(Info* tabInfo, int n) {
  int num_balayage, i;
  for (num_balayage = 1 ; num_balayage < n ; ++num_balayage) /* n-1 balayages */
    for (i = 1 ; i < n ; ++i) /* balayage n°i : n-1 comparaisons */
      if ((tabInfo[i].estampille * 1000 + tabInfo[i].position) > (tabInfo[i-1].estampille * 1000 + tabInfo[i-1].position))  /* si deux elements ne sont pas dans l'ordre */
      permute(&tabInfo[i], &tabInfo[i-1]); /* alors on les echange */
  return 0;
}

void section_critique(){
  printf("Je suis dans la section critique\n");
  sleep(1);
  printf("Je sors de la section critique\n");
}







/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

int main (int argc, char* argv[]) {
  struct sockaddr_in sock_add, sock_add_dist;
  int size_sock;
  int s_ecoute, s_service;
  char texte[40];
  int i,l;
  float t;
  int PortBase=-1; /*Numero du port de la socket a` creer*/
  int NSites=-1; /*Nb total de sites*/


  if (argc<3) {
    printf("Erreur: il faut donner au moins 2 sites pour faire fonctionner l'application: NumeroPortBase et liste_des_sites");
    exit(-1);
  }

  /*----Nombre de sites (adresses de machines)---- */
  NSites=argc-2;


  /*******************************************************************************
  ********************************************************************************
                                 Préparation                   
  ********************************************************************************
  *******************************************************************************/

  //La structure de l'info de la requete                               
  Info info;
  //Initialiser l'horloge logique
  info.estampille = 0;
  //Position de la machine dans la liste
  info.position = GetSitePos(NSites, argv);
  //Compteur des réponses pour rentrer en section critique
  int compteurSC = 0;
  //La queue des requetes
  Info* tabInfo = malloc(1000 * sizeof(Info));
  //Derniere position dans la queue
  int max=0;
  //Un boolean pour empecher une machine de demander de rentrer en SC 2 fois de suite
  int boo = 1;

  /*******************************************************************************
  *******************************************************************************/

  /*CREATION&BINDING DE LA SOCKET DE CE SITE*/
  PortBase=atoi(argv[1])+GetSitePos(NSites, argv);
  printf("Numero de port de ce site %d\n",PortBase);

  sock_add.sin_family = AF_INET;
  sock_add.sin_addr.s_addr= htons(INADDR_ANY);  
  sock_add.sin_port = htons(PortBase);

  if ( (s_ecoute=socket(AF_INET, SOCK_STREAM,0))==-1) {
    perror("Creation socket");
    exit(-1);
  }

  if ( bind(s_ecoute,(struct sockaddr*) &sock_add, \
      sizeof(struct sockaddr_in))==-1) {
    perror("Bind socket");
    exit(-1);
  }
  
  listen(s_ecoute,30);
  /*----La socket est maintenant cre'e'e, binde'e et listen----*/

  if (GetSitePos(NSites, argv) ==0) { 
    /*Le site 0 attend une connexion de chaque site : */
    for(i=0;i<NSites-1;i++) 
      WaitSync(s_ecoute);
    printf("Toutes les synchros recues \n");fflush(0);
    /*et envoie un msg a chaque autre site pour les synchroniser */
    for(i=0;i<NSites-1;i++) 
      SendSync(argv[3+i], atoi(argv[1])+i+1);
    } else {
      /* Chaque autre site envoie un message au site0 
   (1er  dans la liste) pour dire qu'il est lance'*/
      SendSync(argv[2], atoi(argv[1]));
      /*et attend un message du Site 0 envoye' quand tous seront lance's*/
      printf("Wait Synchro du Site 0\n");fflush(0);
      WaitSync(s_ecoute);
      printf("Synchro recue de  Site 0\n");fflush(0);
  }

  
  /* Passage en mode non bloquant du accept pour tous*/
  /*---------------------------------------*/
  fcntl(s_ecoute,F_SETFL,O_NONBLOCK);
  size_sock=sizeof(struct sockaddr_in);

  /*******************************************************************************
  ********************************************************************************
                      Boucle infinie                   
  ********************************************************************************
  *******************************************************************************/
  while(1) {

    /*******************************************************************************
    ********************************************************************************
                      Tirage au sort                 
    ********************************************************************************
    *******************************************************************************/
    
    int random = rand() % 100;

    if(random <80){
      //On ne fait rien
    }
    else if(random >=80 && random <90 && boo){
      boo = 0;
      //On envoit la demande de rentrer en SC au tout le monde sauf cette machine meme
      int i;
      for(i=0; i<NSites; i++){
        if(i != GetSitePos(NSites, argv)){
          envoyer_message(argv[2+i], atoi(argv[1])+i, "requete", info);
        }
      }

      //On s'ajoute dans la queue d'attente
      ajouterQueue(&max, tabInfo, info);
      tri_bulle(tabInfo, max);
      afficherQueue(tabInfo, max);

      //On incremente l'estampille
      info.estampille += NSites-1;
    }
    else{
      //On simule un evenement local, donc on augmente l'estampille
      info.estampille++;
    }



    /*******************************************************************************
    ********************************************************************************
                      Lire le message recu         
    ********************************************************************************
    *******************************************************************************/
    s_service=accept(s_ecoute,(struct sockaddr*) &sock_add_dist,&size_sock);

    if (s_service>0) {
      /*Extraction et affichage du message */
      l=read(s_service,texte,39);
      texte[l] ='\0';
      printf("Message recu : %s\n",texte);     

      /*******************************************************************************
      ********************************************************************************
                        Traiter le message                 
      ********************************************************************************
      *******************************************************************************/

      char* m_type; 
      char* m_position;
      char* m_estampille;
      m_type = strtok(texte, " ");
      m_position = strtok(NULL, " ");
      m_estampille = strtok(NULL, " ");

      //On incremente l'estampille en fonction de l'estampille recue
      int estampille_recue = atoi(m_estampille);
      if(info.estampille < estampille_recue){
        info.estampille = estampille_recue + 1;
      }
      else{
        info.estampille++;
      }

      /*******************************************************************************
      ********************************************************************************
                    Si le message est une demande d'entrer en section critique                 
      ********************************************************************************
      *******************************************************************************/

      if(strcmp(m_type, "requete") == 0){
        Info infoRecue;
        infoRecue.position = atoi(m_position);
        infoRecue.estampille = atoi(m_estampille);

        //On ajoute la requete dans la queue
        ajouterQueue(&max, tabInfo, infoRecue);
        tri_bulle(tabInfo, max);
        afficherQueue(tabInfo, max);

        //On envoit un message de reponse
        envoyer_message(argv[2 + infoRecue.position], atoi(argv[1]) + infoRecue.position, "reponse", info);
        //On augmente l'estampille quand on envoit le message
        info.estampille++;
      }

      /*******************************************************************************
      ********************************************************************************
                    Si le message est une reponse                 
      ********************************************************************************
      *******************************************************************************/

      else if(strcmp(m_type, "reponse") == 0){
          compteurSC++;
      }

      /*******************************************************************************
      ********************************************************************************
                    Si le message est un signal de liberation               
      ********************************************************************************
      *******************************************************************************/

      else if(strcmp(m_type, "liberation") == 0){
        enleverQueue(&max, tabInfo);
        tri_bulle(tabInfo, max);
        afficherQueue(tabInfo, max);
      }
    }

    

    /*******************************************************************************
    ********************************************************************************
          Si on recoit toutes les reponses alors on essaye de rentrer en SC                 
    ********************************************************************************
    *******************************************************************************/

    if(compteurSC == NSites-1){
      Info derniere = derniereValeurQueue(max,tabInfo); // on recupere la derniere ligne de la queue

      if(info.position == derniere.position){
        //On remet le compteur des reponses a zero
        compteurSC = 0;
        //On rentre en section_critique
        section_critique();
        //On reactive la possibilite de demande de rentrer en section critique
        boo = 1;
        //A la sortie, on envoit le message de liberation a toutes les autres machines
        int j;
        for(j=0; j<NSites; j++){
          if(j != GetSitePos(NSites, argv)){
            envoyer_message(argv[2+j], atoi(argv[1])+j, "liberation", info);
          }
        }
        //On s'enleve de la queue
        enleverQueue(&max, tabInfo);
        tri_bulle(tabInfo, max);
        afficherQueue(tabInfo, max);
      }
    }


    sleep(1);   
  }


  close (s_ecoute);  
  return 0;
}