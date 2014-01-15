#include <stdio.h>

/* permutation de deux doubles */
void permute(int *x, int *y) {
  double tmp;
  tmp = *x;
  *x = *y;
  *y = tmp;
}
/**************************************************
***************************************************
                    TRI BULLE
***************************************************
**************************************************/
 
int tri_bulle(int t[], int n) {
  int num_balayage, i;
  for (num_balayage = 1 ; num_balayage < n ; ++num_balayage) /* n-1 balayages */
    for (i = 1 ; i < n ; ++i) /* balayage n°i : n-1 comparaisons */
      if (t[i] < t[i-1])  /* si deux elements ne sont pas dans l'ordre */
    	permute(&t[i], &t[i-1]); /* alors on les echange */
  return 0;
}