#include <stdio.h>
#include <stdlib.h>

/**
 * 
 * This function is called by the qsort function. It is used to compare two
 * elements of the vector to order.
 *
 */

static int compare_floats(const void *p1, const void *p2)
{
  float *num1, *num2;

  num1 = (float *) p1;
  num2 = (float *) p2;

  if (*num1 < *num2)
    return -1;
  if (*num1 > *num2)
    return 1;
  return 0;
}

/**
 * 
 * Main function. Demonstrates the use of the qsort function.
 *
 */

int main(void)
{
  int i;
  float vector[8] = {8.3, 4.0, 2.7, 3.1, 5.6, 6.4, 8.1, 5.5};

  /* We call here the qsort function which belongs to the standard library. As
   * parameters we need to indicate 1) the vector with the elements to order,
   * 2) the number of elements to order, 3) the size of each element, 4) the
   * function used to compare elements. The ordering is performed in-place,
   * i.e. the resulting ordered vector is the first parameter.
   */

  qsort(vector, 8, sizeof(float), compare_floats);

  printf("The ordered vector is: ");

  for(i = 0; i < 8; i++)
    printf("%f ", vector[i]);

  printf("\n");

  return 0;
}
