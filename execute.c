#include <time.h>
#include <math.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include "execute.h"
#include "traduction.h"
#define REGISTRE_MAX 32767
#define REGISTRE_MIN -32768 
unsigned char Mem[65536]; // Memoire 
short int R[32]; // Tableau de registres
int CodeOp; // Code operationnel d'une commande
int dest; // Numero de registre de destination
int source1; // Numero de registre de source 1
int imm; // Est egal a 1 si le 2e source est une valeur immediate et 0 sinon
int source2; // Numero de registre de source 2
int Z, C, N; // Les 3 registres d'etat
unsigned char *PC; // Le pointeur sur le debut d'une prochaine instruction a executer
int nombreDeLignes; // Va contenir le nombre de commandes et donc la place a reserver dans la memoire pour le programme

int hexaADecimal(char ch) {
    switch (ch) {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'a':
      return 10;
    case 'b':
      return 11;
    case 'c':
      return 12;
    case 'd':
      return 13;
    case 'e':
      return 14;
    case 'f':
      return 15;
    default:
      return 89;
    }
  }

int int216bit(int x) {
  if (x > REGISTRE_MAX) {
    while (x > REGISTRE_MAX) {
      x -= 65536;
    }
  }
  if (x < REGISTRE_MIN) {
    while (x < REGISTRE_MIN) {
      x += 65536;
    }
  }
  return x;
}

void VIDE(void){
  return;
}

void ADD(void) {
  int res;
  if (imm == 1) {
    res = R[source1] + source2;
  } else {
    res = R[source1] + R[source2];
  }
  Z = (res == 0) ? 1 : 0;
  C = (res > REGISTRE_MAX) ? 1 : 0;
  N = (res >> 15) ? 1 : 0;
  R[dest] = int216bit(res);
  PC +=4;
}

void SUB(void) {
  int res;
  if (imm == 1) {
    res = R[source1] - source2;
  } else {
    res = R[source1] - R[source2];
  }
  Z = (res == 0) ? 1 : 0;
  C = (res < REGISTRE_MIN) ? 1 : 0;
  N = (res >> 15) ? 1 : 0;
  R[dest] = int216bit(res);
  PC +=4;
}

void MUL(void) { 
  int res;
  int S;
  if (imm == 1) {
    S = source2;
  } else {
    S = R[source2];
  }
  if (S > 0 ) {
    if (R[source1] > 0){
      res = (R[source1] & 255) * (S & 255);   // registre positif valeur positive
    }
    else {
      res = (-1) * (((-R[source1]) & 255) * (S & 255));   // registre negatif valeur positive
    }
  } else {
    if (R[source1] > 0){
      res = (-1) * ((R[source1] & 255) * ((-S) & 255));   // registre positif valeur negatif
    }
    else {
      res = (((-R[source1]) & 255) * ((-S) & 255));   // registre negatif valeur negatif
    }
  }
  Z = (res == 0) ? 1 : 0;
  C = (res < REGISTRE_MIN || res > REGISTRE_MAX) ? 1 : 0;
  N = (res >> 15) ? 1 : 0;
  R[dest] = res;
  PC +=4;
}

void DIV(void) {
  int res;
  if (imm == 1) {
    if (!source2) {
      printf("erreur division par 0 !");
      PC = NULL;
      return;
    }
    if (((source2>>15)&1)) { // nombre negatif
      source2 =((int)pow((-1),(source2>>15)&1))*((~((source2)-1))&16383);
    }
    res = R[source1] / source2;
  } else {
    if (!R[source2]) {
      printf("erreur division par 0 !");
      PC = NULL;
    }
    res = R[source1] / R[source2];

  }
  Z = (res == 0) ? 1 : 0;
  C = (res < REGISTRE_MIN || res > REGISTRE_MAX) ? 1 : 0;
  N = (res >> 15) ? 1 : 0;
  R[dest] = res;
  PC +=4;
}

void AND(void) {
  int res;
  if (imm == 1) {
    res = R[source1] & source2;
  } else {
    res = R[source1] & R[source2];
  }
  Z = (res == 0) ? 1 : 0;
  C = (res < REGISTRE_MIN || res > REGISTRE_MAX) ? 1 : 0;
  N = (res >> 15) ? 1 : 0;
  R[dest] = res;
  PC +=4;
}

void OR(void) {
  int res;
  if (imm == 1) {
    res = R[source1] | source2;
  } else {
    res = R[source1] | R[source2];
  }
  Z = (res == 0) ? 1 : 0;
  C = (res < REGISTRE_MIN || res > REGISTRE_MAX) ? 1 : 0;
  N = (res >> 15) ? 1 : 0;
  R[dest] = res;
  PC +=4;
}

void XOR(void) {
  int res;
  if (imm == 1) {
    res = R[source1] ^ source2;
  } else {
    res = R[source1] ^ R[source2];
  }
  Z = (res == 0) ? 1 : 0;
  C = (res < REGISTRE_MIN || res > REGISTRE_MAX) ? 1 : 0;
  N = (res >> 15) ? 1 : 0;
  R[dest] = res;
  PC +=4;
}

void SHL(void) {
  int res;
  int S;
  if (imm == 1) {
    S = source2;
  } else {
    S = R[source2];
  }
  if (S > 15 ||
      S < -15) { // si on decale plus de 16 fois alors on a forcement 0
    C = S < 0 ? R[source1] >> 15 : R[source1] & 1;
    res = 0;
  } else if (S < 0) {

    C = R[source1] & ((int)pow(2, -S - 1));
    res = R[source1] >> (-S);
  } else {
    C = R[source1] & ((int)pow(2, 16 - S));
    res = R[source1] << S;
  }
  R[dest] = res & 65535;
  Z = (R[dest] == 0) ? 1 : 0;
  N = (R[dest] >> 15) ? 1 : 0;
  PC +=4;
}

void LDB(void) {
  int res;
  if (imm == 1) {
    res = R[source1] + source2 - 65535;
    R[dest] = Mem[(R[source1] + source2) % 65535];
  } else {
    res = R[source1] + R[source2] - 65535;
    R[dest] = Mem[(R[source1] + R[source2]) % 65535];
  }
  Z = (R[dest] == 0) ? 1 : 0;
  C = (res > 0) ? 1 : 0;
  N = (R[dest] >> 15) ? 1 : 0;
  PC +=4;
}

void LDW(void) {
  int res;
  if (imm == 1) {
    R[dest] = Mem[R[source1] + source2];
    res = R[source1] + source2 - 65535;
    R[dest] += Mem[(R[source1] + source2 +1) % 65535]*256;
  } else {
    R[dest] = Mem[R[source1] + R[source2]];
    res = R[source1] + R[source2] - 65535;
    R[dest] += Mem[(R[source1] + R[source2] +1) % 65535]*256;
  }
  Z = (R[dest] == 0) ? 1 : 0;
  C = (res > 0) ? 1 : 0;
  N = (R[dest] >> 15) ? 1 : 0;
  PC += 4;
}

void STB(void) {
  int res;
  if (imm == 1) {
    res = R[source1] + source2 - 65535;
    Mem[(R[dest] + source2) % 65535] = R[source1]&255;
  } else {
    res = R[source1] + R[source2] - 65535;
    Mem[(R[dest] + R[source2]) % 65535] = R[source1]&255;
  }
  Z = (R[source1] == 0) ? 1 : 0;
  C = (res > 0) ? 1 : 0;
  N = Mem[(R[dest] + R[source2]) % 65535] >> 15;
  PC += 4;
}

void STW(void) {
  int res;
  if (imm == 1) {
    res = R[source1] + source2 - 65535;
    Mem[(R[dest] + source2) % 65535] = (R[source1]&255);
    Mem[(R[dest] + source2+1) % 65535] = (R[source1]  >> 8);
  } else {
    res = R[source1] + R[source2] - 65535;
    Mem[(R[dest] + R[source2]) % 65535] = (R[source1]&255);
    Mem[(R[dest] + R[source2]+1) % 65535] = (R[source1] >> 8);
  }
  Z = (R[source1] == 0) ? 1 : 0;
  C = (res > 0) ? 1 : 0;
  N = (R[source1] >> 15) ? 1 : 0;
  PC += 4;
}

void JMP(void) {
  int src2 = imm ? source2 : R[source2];
  if (src2%4==0) {
    PC = &Mem[src2];
  } else {
    printf("Erreur : L'adresse n'est pas valide\n");
    printf("Indication : Si vous utilisez des nombre decimaux/hexa en tant qu'adresse de saut,\n");
    printf("             verifiez qu'ils sont bien les multiples de 4\n");
    PC = NULL;
  }
}

void JZS(void) {
  int src2 = imm ? source2 : R[source2];
  if (src2%4!=0) {
    printf("Erreur : L'adresse n'est pas valide\n");
    printf("Indication : Si vous utilisez des nombre decimaux/hexa en tant qu'adresse de saut,\n");
    printf("             verifiez qu'ils sont bien les multiples de 4\n");
    PC = NULL;
    return;
  }
  if (Z == 1) {
    if (imm) {
      PC = &Mem[source2];
    } else {
      PC = &Mem[R[source2]];
    }
  } else {
    PC += 4;
  }
}

void JZC(void) {
  int src2 = imm ? source2 : R[source2];
  if (src2%4!=0) {
    printf("Erreur : L'adresse n'est pas valide\n");
    printf("Indication : Si vous utilisez des nombre decimaux/hexa en tant qu'adresse de saut,\n");
    printf("             verifiez qu'ils sont bien les multiples de 4\n");
    PC = NULL;
    return;
  }
  if (Z == 0) {
    if (imm) {
      PC = &Mem[source2];
    } else {
      PC = &Mem[R[source2]];
    }
  } else {
    PC += 4;
  }
}

void JCS(void) {
  int src2 = imm ? source2 : R[source2];
  if (src2%4!=0) {
    printf("Erreur : L'adresse n'est pas valide\n");
    printf("Indication : Si vous utilisez des nombre decimaux/hexa en tant qu'adresse de saut,\n");
    printf("             verifiez qu'ils sont bien les multiples de 4\n");
    PC = NULL;
    return;
  }
  if (C == 1) {
    if (imm) {
      PC = &Mem[source2];
    } else {
      PC = &Mem[R[source2]];
    }
  } else {
    PC += 4;
  }
}

void JCC(void) {
  int src2 = imm ? source2 : R[source2];
  if (src2%4!=0) {
    printf("Erreur : L'adresse n'est pas valide\n");
    printf("Indication : Si vous utilisez des nombre decimaux/hexa en tant qu'adresse de saut,\n");
    printf("             verifiez qu'ils sont bien les multiples de 4\n");
    PC = NULL;
    return;
  }
  if (C == 0) {
    if (imm) {
      PC = &Mem[source2];
    } else {
      PC = &Mem[R[source2]];
    }
  } else {
    PC += 4;
  }
}

void JNS(void) {
  int src2 = imm ? source2 : R[source2];
  if (src2%4!=0) {
    printf("Erreur : L'adresse n'est pas valide\n");
    printf("Indication : Si vous utilisez des nombre decimaux/hexa en tant qu'adresse de saut,\n");
    printf("             verifiez qu'ils sont bien les multiples de 4\n");
    PC = NULL;
    return;
  }
  if (N == 1) {
    if (imm) {
      PC = &Mem[source2];
    } else {
      PC = &Mem[R[source2]];
    }
  } else {
    PC += 4;
  }
}

void JNC(void) {
  int src2 = imm ? source2 : R[source2];
  if (src2%4!=0) {
    printf("Erreur : L'adresse n'est pas valide\n");
    printf("Indication : Si vous utilisez des nombre decimaux/hexa en tant qu'adresse de saut,\n");
    printf("             verifiez qu'ils sont bien les multiples de 4\n");
    PC = NULL;
    return;
  }
  if (N == 0) {
    if (imm) {
      PC = &Mem[source2];
    } else {
      PC = &Mem[R[source2]];
    }
  } else {
    PC += 4;
  }
}

void IN(void) {
  if (scanf("%hd", &R[dest]) !=1) {
    printf("La valeur doit etre passe en decimal !\n");
    printf("Si vous voulez passer un caractere il faut entrer son code numerique en ascii\n\n");
    PC = NULL;
  } else {
    R[dest] = int216bit(R[dest]);
    Z = (R[dest] == 0) ? 1 : 0;
    N = R[dest]>>15;
    C = 0;
    PC += 4;
  }
}

void OUT(void) {
  printf("%d\n", R[dest]);
  Z = (R[dest]) ? 0 : 1;
  N = R[dest]>>15;
  C = 0;
  PC += 4;
}

void RND(void) {
  int S;
  if (imm == 1) {
    S = source2;
  } else {
    S = R[source2];
  }
  int min = (R[source1] > S) ? S : R[source1];
  int max = (R[source1] > S) ? R[source1] : S;
  int r = (rand()%((max - min))) + min;
  R[dest] = r;
  Z = (r == 0) ? 1 : 0;
  N = r >> 15;
  C = 0; 
  PC += 4;
}

void HLT(void) {
  PC = NULL; 
  printf("Fin du programme.\n\n");
}

void (*tabCommandes[32])(void) = {ADD, SUB, MUL, DIV, AND, OR,  XOR, SHL, VIDE, VIDE, LDB, LDW, STB, STW, VIDE, VIDE, VIDE, VIDE, VIDE, VIDE, JMP, JZS, JZC, JCS, JCC, JNS, JNC, IN, OUT, RND, VIDE, HLT};

// Fonction pour stocker le programme dans la memoire
void programInMem(int *nombreDeLignes) { 
  FILE *fichier = NULL;
  fichier = fopen("hexa.txt", "r");
  if (fichier == NULL) {
    return;
  }
  // On lit tous les lignes du fichier par deux lettres et 
  // les convertit en decimale pour stocker dans la memoire
  char lettre1 = fgetc(fichier);
  char lettre2;
  int ligne = 0;
  int colonne = 0;
  int nb = 0;
  while (lettre1 != EOF) {
    if (lettre1 != '\n') {
      lettre2 = fgetc(fichier);
      nb = hexaADecimal(lettre1) * 16;
      nb += hexaADecimal(lettre2);

      Mem[4 * ligne + colonne] = nb;
      colonne++;
      lettre1 = fgetc(fichier);
    } else {
      ligne++;
      colonne = 0;
      lettre1 = fgetc(fichier);
    }
  }
  *nombreDeLignes = ligne+1;
}

// Recupere les numeros du code operationel et les parametres associes de la memoire
void ligne(unsigned char *line, int *CodeOp, int *dest, int *source1, int *imm, int *source2) {
  *CodeOp = *line >> 3;
  *dest = (*line & 7)*2*2 + (*(line+1) >> 6);
  *source1 = (*(line+1) & 63) >> 1;
  *imm = *(line+1) & 1;
  *source2 = (*(line+2))*16*16 +(*(line+3));
  if (*imm==1 && ((*source2>>15)&1)) { // nombre negatif
    *source2 = -((~((*source2)-1))&16383);
  }
}
