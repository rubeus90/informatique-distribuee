#include <stdio.h>

/* permutation de deux doubles */
void permute(Info *x, Info *y) {
  Info tmp;
  tmp = *x;
  *x = *y;
  *y = tmp;
}
/**************************************************
***************************************************
                    TRI BULLE
***************************************************
**************************************************/
 
int tri_bulle(Info* tabInfo, int n) {
  int num_balayage, i;
  for (num_balayage = 1 ; num_balayage < n ; ++num_balayage) /* n-1 balayages */
    for (i = 1 ; i < n ; ++i) /* balayage n°i : n-1 comparaisons */
      if ((tabInfo[i].estampille * 1000 + tabInfo[i].position) < (tabInfo[i-1].estampille * 1000 + tabInfo[i-1].position))  /* si deux elements ne sont pas dans l'ordre */
    	permute(&tabInfo[i], &tabInfo[i-1]); /* alors on les echange */
  return 0;
}