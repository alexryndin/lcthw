/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <assert.h>
int
main ()
{
  int use = 0;
  int *use_or = &use;

  use_or = 0;
  assert (*use_or == 1 || *use_or == 0  && "Some message");


  return 0;
}

