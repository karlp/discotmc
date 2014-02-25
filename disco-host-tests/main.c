/* 
 * File:   main.c
 * Author: karlp
 *
 * Created on February 4, 2014, 11:31 PM
 */

#include <stdio.h>
#include <stdlib.h>

int sineTable[1025];

void generateTable() {
  int i, j, k;
  const int magic[33] = {
    0x691864, 0x622299, 0x2CB61A, 0x461622,
    0x62165A, 0x85965A, 0x0D3459, 0x65B10C,
    0x50B2D2, 0x4622D9, 0x88C45B, 0x461828,
    0x6616CC, 0x6CC2DA, 0x512543, 0x65B69A,
    0x6D98CC, 0x4DB50B, 0x86350C, 0x7136A2,
    0x6A974B, 0x6D531B, 0x70D514, 0x4EA714,
    0x5156A4, 0x393A9D, 0x714A6C, 0x755555,
    0x5246EB, 0x916556, 0x7245CD, 0xB4F3CE,
    0x6DBC7A
  };
  k = 0;
  for (i = 0; i < 33; i++) {
    for (j = 0; j < 24; j += 3) {
      sineTable[k++] = ((magic[i] >> j) & 7) - 4;
    }
  }
  sineTable[1] = 51472;
  for (i = 3; i > 0; i--) {
    for (j = i; j <= 256; j++) {
      k = sineTable[j - 1];
      sineTable[j] = k + sineTable[j];
      sineTable[513-j] = k;
      sineTable[511+j] = -k;
      sineTable[1025-j] = -k;
    }
  }
  sineTable[768] = -0x800000;
}

int main() {
  int i;

  generateTable();

  /* Printout */
  for (i = 0; i < 1025; i++) {
    printf("%d\t%d\n", i, sineTable[i]);
  }

  return 0;
}
