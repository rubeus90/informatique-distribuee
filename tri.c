
/* tri.c 

  IN3ST02 TP3

  JCG 15/09/2008

  Maj 11/01/2012
  fonctions de tri
*/
#include <stdio.h>


/* utilitaires */

/* permutation de deux doubles */
void permute(double *x, double *y) {
  double tmp;
  tmp = *x;
  *x = *y;
  *y = tmp;
}
/**************************************************
***************************************************
                    TRI BULLE
***************************************************
***************************************************
*/

/******************************************* 
 * tri bulle non optimise n-1 balayages 
 * chaque balayage compare n-1 couples  
 ******************************************* 
 */
int tri_bulle_1(double t[], int n) {
  int num_balayage, i;
  for (num_balayage = 1 ; num_balayage < n ; ++num_balayage) /* n-1 balayages */
    for (i = 1 ; i < n ; ++i) /* balayage n°i : n-1 comparaisons */
      if (t[i] < t[i-1])  /* si deux elements ne sont pas dans l'ordre */
    	permute(&t[i], &t[i-1]); /* alors on les echange */
  return 0;
}


/******************************************* 
 * tri bulle améliore : n-1 balayages 
 * le balayage n°i ne compare que n-i couples 
 ****************************************** 
 */
int tri_bulle_2(double t[], int n) {
/* a ecrire */
  return -1;
}


/******************************************* 
 * tri bulle optimise : n-1 balayages au maximum
 *  le balayage n°i ne compare que n-i couples 
 *  des que le tableau est trie, on termine le programme
 ****************************************** 
 */
int tri_bulle_3(double t[], int n) {
/* a ecrire */
  return -1;
}

 
/******************************************
******************************************* 
             TRI SHELL
*******************************************
******************************************** 
 */
int tri_shell(double t[], int n) {
  int ecart, sous_tableau, i, j;
  for (ecart = n / 2 ; ecart > 0 ; ecart /= 2)  /* comp. éléments distants de ecart*/
    for (sous_tableau = 0 ; sous_tableau < ecart ; ++sous_tableau ) /* il y 'ecart' sous-tableaux distants de 'ecart' */
      for (i = sous_tableau ; i + ecart < n  ; i += ecart)
        for (j = i ; j >= 0 && t[j] > t[j + ecart] ; j -= ecart)  
          permute(&t[j], &t[j + ecart]);
  return 0;
}

/***********************************************************
************************************************************
                 QUICK SORT
************************************************************
************************************************************
*/
/***********************************************************
 la partition vue en td 
 ***********************************************************
*/


int partition(double *t, int d, int f)
{
	int e1,e2;
	e1=d;
	e2=f;
	while(e1<e2)
	{
		if(t[d] >= t[e1+1])
			e1++;
		else
		{
			permute(&t[e1+1],&t[e2]);
			e2--;
		}
	}
	permute(&t[d], &t[e1]);
	return e1;
}
		
 
/*********************************************  
 * quicksort version JCG
 ********************************************  
 */

int quicksort1(double t[], int n)
{
	if(n>1)
	{
		//int k = partition(t,0,n-1);
		int k = partition(t,0,n-1);
		quicksort1(t,k);
		quicksort1(t+k+1,  n-1-k);
	}
	return 0;
}



/*********************************************  
 * quick sort améliore si n<=6, on fait un shell sort 
 ********************************************  
 */
int quicksort2(double t[], int n) 
{ 
	if(n<=6)
		tri_shell(t, n);
	else{
		int k = partition(t,0,n-1);
		quicksort2(t,k);
		quicksort2(t+k+1,  n-1-k);
	}
	return 0;
} 

/******************************************
******************************************* 
             TRI INSERTION
*******************************************
******************************************** 
 */

/*********************************************  
 * trouve place sequentielle d'un double dans un tableau trie 
 ********************************************  
 */
int trouve_place_seq(double x, double t[], int n) 
{	
	int i;
	for(i=0; i<n ; i++)
	{
		if(x <= t[i])
			return i;
	}
	return n;
}

/*********************************************  
 * trouve place dichotomique d'un double dans un tableau trie 
 ********************************************  
 */

int trouve_place_dicho(double x, double t[], int n)
{
	int i;
	int k= n/2;
	int g= n/2;
	for(i=0; i<n/2;i++)
	{
		g = g/2;
		if(x<k)
			k=k-g;
		else
			k=k+g;
	}
	return k;
}

/*********************************************   
 * decalage droite de p a  q inclus
 * [p..q] -> [p+1..q+1]
 ********************************************  
 */
void decaleD(double t[], int p, int q) 
{
	int i;
	for(i=q ; i>=p; i--)
		t[i+1] = t[i];
	
} 


/*********************************************  
 * tri par insertion (recherche sequentielle)
 ********************************************  
 */
int tri_insertion_seq(double t[], int n)
{
	int i,k;
	for(i=1; i<n;i++)
	{
		k = trouve_place_seq(t[i],t,i);
		decaleD(t,k,i-1);
	}
	return 0;
}


/*********************************************  
 * tri par insertion (recherche dichotomique)
 ********************************************  
 */
int tri_insertion_dicho(double t[], int n) 
{
	int i,k;
	for(i=1; i<n;i++)
	{
		k = trouve_place_dicho(t[i],t,i);
		decaleD(t,k,i-1);
	}
	return 0;
}



/**********************************************
***********************************************
                HEAP SORT
 ********************************************  
 ********************************************  
 */
		
/**********************************************  
 * retablir un tas
 * tous les arbres sous 'pere_a_retablir' sont des tas sauf peut etre 'pere_a_retablir'   
 *********************************************  
 */
void retablir_tas(double t[], int n, int pere_a_retablir) {
  int pere, fils_le_plus_grand;
  pere = pere_a_retablir;
  fils_le_plus_grand = 2 * pere + 1; /* le fils gauche */
  while (fils_le_plus_grand < n) {
    if (fils_le_plus_grand + 1 < n && t[fils_le_plus_grand + 1] >= t[fils_le_plus_grand]) { /* si le fils droit existe et est plus grand, c'est lui que l'on compare au pere */
      ++fils_le_plus_grand;
    }
    if (t[fils_le_plus_grand] > t[pere]) {
      permute(&t[fils_le_plus_grand],&t[pere]);
	pere = fils_le_plus_grand;
	fils_le_plus_grand = 2*pere+1;
    }
    else { /* le pere est plus grand que ses fils  */
      break;
    }
  }
}

int hpsort(double t[], int n) {
  int pere_examine ;
  /* construction du tas */
  pere_examine = (n - 1) / 2; /* le dernier vrai pere */
  while (pere_examine >= 0) {
    retablir_tas(t, n, pere_examine);
    --pere_examine;
  }
  /* ici, on doit avoir un tas */
  while (n > 1) { /* tq on n'a au moins deux elements  */
    --n; 
    permute(&t[n], &t[0]);
    retablir_tas(t, n, 0); /* on retablit le tas a partir de la racine */
  }
  return 0;
}


/**********************************************
***********************************************
                TRI SELECTION
 ********************************************  
 ********************************************  
 */


/**********************************************  
 * indice du plus grand 
 *********************************************  
 */
int ipg(double *v, int n) {
  int imax, i;
  imax = 0;
  for (i=1;i<n;++i) {
    if (v[i]>v[imax])
      imax = i;
  }
  return imax;
}
int tri_selection(double *t,int n)
{
	int i;
	for(i=n; i>0; i--)
	{
		permute(&t[i-1], &t[ipg(t,i)]);
	}
	return 0;
}




