#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "execute.h"
#include "traduction.h"
#define NbColonnes 200
#define NbLignes 1000

// Renvoie 1 si char ch est dans cette liste : 0123456789, 0 sinon
int estDecimal(char ch) {
  int i = ch;
  if (i <= 57 && i >= 48) {
    return 1;
  }
  return 0;
}

// Renvoie 1 si char ch est dans cette liste : 0123456789abcdef, 0 sinon
int estHexadecimal(char ch) {
  int i = ch;
  if (estDecimal(ch) || (i <= 70 && i >= 65) || (i <= 102 && i >= 97)) {
    return 1;
  }
  return 0;
}

// Fonction qui retourne le nombre de zeros à rajouter avant le
// debut de la chaine hexa pour avoir la chaine complete
int zeroAAjouter( int val) { 
  char str[14];
  sprintf(str, "%x", val);
  return (8 - strlen(str));
}

// Verifie si la ligne passe en parametres est vide, renvoie 1 dans ce cas, sinon - 0
int estLigneVide(char ligne[], int i) {
  for (int ind = i; ligne[ind] != '\n' && ligne[ind] != '\0'; ind++) {
    if (ligne[ind] != '\t' && ligne[ind] != ' ') {
      return 1;
    }
  }
  return 0;
}

// convertit une chaine de caracteres ecrit en hexa en un nombre decimal
long int chaineHexaEnDecimal(char hex[]) {
  long int base = 1;
  long int dec = 0;
  for (int i = strlen(hex) - 1; i >= 0; i--) {
    if (hex[i] >= '0' && hex[i] <= '9') {
      dec += (hex[i] - 48) * base;
      base *= 16;
    } else if (hex[i] >= 'a' && hex[i] <= 'f') {
      dec += (hex[i] - 87) * base;
      base *= 16;
    } else if (hex[i] >= 'A' && hex[i] <= 'F') {
      dec += (hex[i] - 55) * base;
      base *= 16;
    }
  }
  return dec;
}

// Fonction qui parcourt la ligne actuelle et stocke les etiquettes trouves
// dans le tableau "tableauEtiquettes"
int collectionDesEtiquettes(int ligneActuelleIndice, char ligneActuelle[], char tableauEtiquettes[NbLignes][NbColonnes]) {
  int tagPres = 0;
  // On verifie la presence d'une etiquette sur la ligne actuelle
  for (int i = 0; i < strlen(ligneActuelle); i++) {
    if (ligneActuelle[i] == ':') {
      tagPres = 1;
    }
  }
  int ind = 0;
  // Si la ligne contient une etiquette, alors on verifie qu'elle est ecrit sous la bonne forme
  if (tagPres) {
    while (ligneActuelle[ind] != ' ' && ligneActuelle[ind] != ':') {
      int carVal = ligneActuelle[ind];
      if (carVal < 48 || (carVal > 57 && carVal < 65) ||
          (carVal > 90 && carVal < 97) || carVal > 122) {
        printf("Erreur : une etiquette n'est pas passee sous la bonne forme !\n");
        printf("Indication : Une etiquette ne peut etre composee uniquement \n");
        printf("             des lettres majuscules, minuscules et de chiffres\n");
        return -1;
      } else {
        tableauEtiquettes[ligneActuelleIndice][ind] = ligneActuelle[ind];
        ind++;
      }
    }
    while (ligneActuelle[ind] != ':') {
      if (ligneActuelle[ind++] != ' ') {
        printf("Erreur : une etiquette n'est pas passee sous la bonne forme !\n");
        printf("Indication : Une etiquette doit etre un seul mot et ne doit pas contenir d'espaces !\n");
        return -1;
      }
    }
    ind++;
    return 1;
  }
  return 0;
}

// Fonction qui parcourt le tableaux d'etiquettes et renvoie 1 si elle trouve un
// doublon, et 0 sinon
int doublonEtiquette(char tableauEtiquettes[NbLignes][NbColonnes], int nbLines) {
  for (int i = 0; i < NbLignes; i++) {
    if (strcmp(tableauEtiquettes[i], "")) {
      for (int j = i + 1; j < nbLines; j++) {
        if (strcmp(tableauEtiquettes[i], tableauEtiquettes[j]) == 0) {
          return j;
        }
      }
    }
  }
  return 0;
}

// Fonction pour traduire une ligne du programme assembleur et la stocker sous forme decimale dans un tableau int *ligneTraduit
int traduction(int ligneActuelleIndice, char ligneActuelle[ligneActuelleIndice], char tableauEtiquettes[NbLignes][NbColonnes], int etiquettePres, int *ligneTraduit) {
  // Collection des commandes du code opération
  char *codeOpVals[32] = {"add", "sub", "mul", "div", "and", "or",  "xor",
    "shl", "",    "",    "ldb", "ldw", "stb", "stw",
    "",    "",    "",    "",    "",    "",    "jmp",
    "jzs", "jzc", "jcs", "jcc", "jns", "jnc", "in",
    "out", "rnd", "",    "hlt"};
   
  // Ligne traduite en decimal; 1 element : code op., 2 - dest, 3 - Src 1, 4 - Imm, 5 - Src 2
  long ligneTraduitDecimal[5] = {0, 0, 0, 0, 0};

  // Indice principal avec lequel on parcourt la ligne une fois
  int ind = 0; 

  // Est egale a 1 s'il n'y a pas d'erreur
  int erreurLigne = 1; 

  // On parcourt la ligne actuelle jusqu'a la fin de l'etiquette
  if (etiquettePres) { 
    while (ligneActuelle[ind] != ':') {
      ind++;
    }
    ind++;
  }

  // On parcourt la ligne actuelle jusqu'a ce qu'on tombe sur une commande
  while (ligneActuelle[ind] == ' ' || ligneActuelle[ind] == '0' ||
         ligneActuelle[ind] == '1' || ligneActuelle[ind] == '\t') {
    ind++;
  }

  // Chaine de caractere qui contient la commande de la ligne actuelle
  char command[10] = ""; 

  // Indice auxiliaire pour stocker lettre par lettre la commande actuelle
  int j = 0; 

  // Stockage de la commande actuelle lettre par lettre
  while (ligneActuelle[ind] != ' ' && ligneActuelle[ind] != '\0' &&
         ligneActuelle[ind] != '\n') {
    command[j] = ligneActuelle[ind];
    ind++;
    j++;
  }

  // Indice qui va contenir le numero de la commande
  int i = 0; 

  // Recherche du numero de la commande dans la Collection des commande du code opératoire
  while (i < 32 && strcmp(codeOpVals[i], command)) {
    i++;
  }

  // Si la commande existe et est trouve on la traduit en binaire
  if (i < 32) {
    ligneTraduitDecimal[0] = i;
  } else {
    printf("Erreur : La commande %s n'existe pas a la ligne %d\n", command, ligneActuelleIndice + 1);
    printf("Indication : Verifiez bien que la commande est bien dans la liste precisee dans le user.pdf\n");
    printf("             et est bien en minuscules\n");
    erreurLigne = 0;
  }

  // Tableau de caracteres qui contient tous les arguments de la ligne actuelle
  char parametres[3][100] = {"", "", ""}; 

  // S'il y a des espaces on les passe
  while (ligneActuelle[ind] == ' ') {
    ind++;
  }

  // Indice pour stocker le numero de l'argument qu'on est en train de parcourir
  int col = 0; 

  if (ligneTraduitDecimal[0] >= 10 && ligneTraduitDecimal[0] <= 11) {
    // Indice auxiliaire qui aide a stocker lettre par lettre un
    // parametre qu'on est en train de parcourir
    j = 0;

    while (ligneActuelle[ind] != ',' && ligneActuelle[ind] != '\n' &&
           ligneActuelle[ind] != '\0' && ligneActuelle[ind] != ' ') {
      parametres[col][j] = ligneActuelle[ind];
      ind++;
      j++;
    }
    while (ligneActuelle[ind] == ' ') {
      ind++;
    }

    if (ligneActuelle[ind] != ',') {
      printf("Erreur : Il manque une virgule a la ligne %d\n", ligneActuelleIndice + 1);
      erreurLigne = 0;
    } else {
      ind++;
    } // On passe les caracteres ","

    while (ligneActuelle[ind] == ' ') {
      ind++;
    };
    col++;
    if (ligneActuelle[ind] != '(') {
      printf("Erreur : Il manque une paranthese '(' a la ligne %d", ligneActuelleIndice + 1);
      printf("Indication : Les instructions de transfert ldb et ldw s'ecrivent sous le format suivant :\n");
      printf("             'ldb rd, (rn)S' et 'ldw rd, (rn)S'\n");
      erreurLigne = 0;
    } else {
      ind++;
    } // On passe le "("
    j = 0;
    while (ligneActuelle[ind] == ' ') {
      ind++;
    };
    while (ligneActuelle[ind] != ')' && ligneActuelle[ind] != ' ') {
      parametres[col][j] = ligneActuelle[ind];
      ind++;
      j++;
    }
    col++;
    while (ligneActuelle[ind] == ' ') {
      ind++;
    };
    if (ligneActuelle[ind] != ')') {
      printf("Erreur : Il manque une paranthese ')' a la ligne %d\n", ligneActuelleIndice + 1);
      printf("Indication : Les instructions de transfert ldb et ldw s'ecrivent sous le format suivant :\n");
      printf("             'ldb rd, (rn)S' et 'ldw rd, (rn)S'\n");
      erreurLigne = 0;
    } else {
      ind++;
    } // On passe le ")"
    while (ligneActuelle[ind] == ' ') {
      ind++;
    };
    j = 0;
    while (ligneActuelle[ind] != '\0' && ligneActuelle[ind] != '\n' &&
           ligneActuelle[ind] != ' ' && ligneActuelle[ind] != ',') {
      parametres[col][j] = ligneActuelle[ind];
      ind++;
      j++;
    }
    col++;
    if (estLigneVide(ligneActuelle, ind)) {
      printf("Erreur : La ligne %d contient trop de parametres\n", ligneActuelleIndice + 1);
      erreurLigne = 0;
    }
    for (int i = 0; i < NbLignes; i++) {
      if (strcmp(parametres[2], tableauEtiquettes[i]) == 0) {
        printf("Erreur : Absence d'une etiquette possible à la ligne %d \n", ligneActuelleIndice + 1);
        erreurLigne = 0;
      }
    }
  } else if (ligneTraduitDecimal[0] >= 12 && ligneTraduitDecimal[0] <= 13) {
    while (ligneActuelle[ind] == ' ') {
      ind++;
    };
    if (ligneActuelle[ind] != '(') { // On passe le "("
      printf("Erreur : Il manque une paranthese '(' a la ligne %d\n", ligneActuelleIndice + 1);
      printf("Indication : Les instructions de transfert stb et stw s'ecrivent sous le format suivant :\n");
      printf("             'stb (rd)S, rn' et 'stw (rd)S, rn'\n");
      erreurLigne = 0;
    } else {
      ind++;
    }
    while (ligneActuelle[ind] == ' ') {
      ind++;
    };
    j = 0;
    while (ligneActuelle[ind] != ')' && ligneActuelle[ind] != ' ') {
      parametres[0][j] = ligneActuelle[ind];
      ind++;
      j++;
    }
    col++;
    while (ligneActuelle[ind] == ' ') {
      ind++;
    };
    if (ligneActuelle[ind] != ')') { // On passe le ")"
      printf("Erreur : Il manque une parenthese ')' a la ligne %d", ligneActuelleIndice + 1);
      printf("Indication : Les instructions de transfert stb et stw s'ecrivent sous le format suivant :\n");
      printf("             'stb (rd)S, rn' et 'stw (rd)S, rn'\n");
      erreurLigne = 0;
    } else {
      ind++;
    }
    while (ligneActuelle[ind] == ' ') {
      ind++;
    };

    j = 0;
    while (ligneActuelle[ind] != '\0' && ligneActuelle[ind] != '\n' &&
           ligneActuelle[ind] != ',' && ligneActuelle[ind] != ' ') {
      parametres[2][j] = ligneActuelle[ind];
      ind++;
      j++;
    }
    col++;
    while (ligneActuelle[ind] == ' ') {
      ind++;
    };
    if (ligneActuelle[ind] != ',') {
      printf("Erreur : Il manque une virgule a la ligne %d\n", ligneActuelleIndice + 1);
      erreurLigne = 0;
    } else {
      ind++;
    } // On passe les caracteres ","

    while (ligneActuelle[ind] == ' ') {
      ind++;
    };

    // Indice auxiliaire qui aide a stocker lettre par lettre un
    // parametre qu'on est en train de parcourir
    j = 0; 
    while (ligneActuelle[ind] != ',' && ligneActuelle[ind] != '\n' &&
           ligneActuelle[ind] != '\0' && ligneActuelle[ind] != ' ') {
      parametres[1][j] = ligneActuelle[ind];
      ind++;
      j++;
    }
    col++;

    if (estLigneVide(ligneActuelle, ind)) {
      printf("Erreur : la ligne %d contient trop de parametres\n", ligneActuelleIndice + 1);
      erreurLigne = 0;
    }

    for (int i = 0; i < NbLignes; i++) {
      if (strcmp(parametres[2], tableauEtiquettes[i]) == 0) {
        printf("Erreur : La commande à la ligne %d ne prend pas en parametre une etiquette\n",ligneActuelleIndice + 1);
        printf("Indication : Verifier que la commande est bien dans la liste precisee dans le user.pdf\n");
        erreurLigne = 0;
      }
    }
  } else {
    // on passe les espaces si il en a
    while (ligneActuelle[ind] == ' ') {
      ind++;
    };
    // On stocke tous les argument de la ligne actuelle
    while (ligneActuelle[ind] != '\n' && ligneActuelle[ind] != '\0' && col < 4) {
      // Indice auxiliaire qui aide a stocker lettre par lettre un
      // parametre qu'on est en train de parcourir
      j = 0;
      while (ligneActuelle[ind] != ',' && ligneActuelle[ind] != '\n' &&
             ligneActuelle[ind] != '\0' && ligneActuelle[ind] != ' ') {

        parametres[col][j] = ligneActuelle[ind];

        ind++;
        j++;
      }
      while (ligneActuelle[ind] == ' ') {
        ind++;
      };
      if (ligneActuelle[ind] == ',' && !estLigneVide(ligneActuelle, ind + 1)) {
        printf("Erreur : Il y a trop de virgules a la ligne %d\n", ligneActuelleIndice + 1);
        erreurLigne = 0;
      }
      if (ligneActuelle[ind] != ',' && ligneActuelle[ind] != '\n' &&
          ligneActuelle[ind] != '\0') {
        printf("Erreur : Il manque une virgule a la ligne %d\n", ligneActuelleIndice + 1);
        erreurLigne = 0;
      } else { // On passe les caracteres ","
        ind++;
      }
      while (ligneActuelle[ind] == ' ') {
        ind++;
      };
      col++;
    }
    if (col > 3) {
      printf("Erreur : Mauvais nombre d'argument a la ligne %d\n", ligneActuelleIndice + 1);
      erreurLigne = 0;
    }
  }

  if (((ligneTraduitDecimal[0] >= 10 && ligneTraduitDecimal[0] <= 13) ||
       (ligneTraduitDecimal[0] >= 0 && ligneTraduitDecimal[0] <= 7) ||
       ligneTraduitDecimal[0] == 29 ||
       ((ligneTraduitDecimal[0] >= 10 && ligneTraduitDecimal[0] <= 13))) &&
      col != 3) {
    printf("Erreur : Mauvais nombre d'argument a la ligne %d\n", ligneActuelleIndice + 1);
    printf("Indication : La commande '%s' prend 3 parametres\n", command);
    erreurLigne = 0;
  }
  if ((ligneTraduitDecimal[0] >= 20 && ligneTraduitDecimal[0] <= 28) && col != 1) {
    printf("Erreur : Mauvais nombre d'argument a la ligne %d\n", ligneActuelleIndice + 1);
    printf("Indication : La commande '%s' prend 1 parametre\n", command);
    erreurLigne = 0;
  }
  if (ligneTraduitDecimal[0] == 31 && col != 0) {
    printf("Erreur : Mauvais nombre d'argument a la ligne %d\n", ligneActuelleIndice + 1);
    printf("Indication : La commande '%s' prend 0 parametres\n", command);
    erreurLigne = 0;
  }

  // Le numero d'argument qu'on est en train de parcourir (j<col)
  // ()"col" contient le nombre d'arguments)
  j = 0;

  while (j < col) {
    // Indice auxiliaire pour comparaison des lettres
    int y;

    // Variable auxiliaire pour recuperer la valeur decimale du numero du
    // parametre ou de l'adrese ou pointe l'etiquette
    long int parNb = 0;

    // Variable auxiliaire qui aide a calculer la valeur immediate
    long int nbAux;

    if (parametres[j][0] == '#') {
      if (j != 2 && (ligneTraduitDecimal[0] < 20 || ligneTraduitDecimal[0] > 26)) {
        printf("Erreur : La valeur immediate doit etre passe en 2e source a la ligne %d\n", ligneActuelleIndice + 1);
        erreurLigne = 0;
      }
      // Le cas ou la valeur immediate est en hexadecimal
      if (parametres[j][1] == 'h') {
        y = 2;
        // Valeur qui va contenir la valeur passe en hexadecimal
        char valHexadec[5] = "";

        while ((parametres[j][y] != '\0')) {
          if (!estHexadecimal(parametres[j][y])) {
            printf("Erreur : La valeur immediate a la ligne %d n'est pas sous la bonne forme\n", ligneActuelleIndice + 1);
            printf("Indication : La valeur immediate doit etre un entier ecrit en \n");
            printf("             decimal (positif ou negatif (ex : #1 ou #-1)\n");
            printf("             ou en hexadecimal (positif ex : #h1) ! ligne %d\n", ligneActuelleIndice + 1);
            erreurLigne = 0;
          }
          valHexadec[y - 2] = parametres[j][y];
          y++;
        }
        valHexadec[y] = '\0';
        parNb = chaineHexaEnDecimal(valHexadec);
      } else {
        // Le cas ou la valeur immediate est negative
        if (parametres[j][1] == '-') {
          y = 2;
          while ((parametres[j][y] != '\0')) {
            if (!estDecimal(parametres[j][y])) {
              printf("Erreur : La valeur immediate a la ligne %d n'est pas sous la bonne forme\n", ligneActuelleIndice + 1);
              printf("Indication : La valeur immediate doit etre un entier ecrit en \n");
              printf("             decimal (positif ou negatif (ex : #1 ou #-1)\n");
              printf("             ou en hexadecimal (positif ex : #h1) ! ligne %d\n", ligneActuelleIndice + 1);
              erreurLigne = 0;
            }
            nbAux = (parametres[j][y] - 48) *
                    pow(10, strlen(parametres[j]) - 1 -
                                y); // convertir le caractere ascii en entier (0
                                    // car = 48 en decimal)
            parNb += nbAux;
            y++;
          }
          parNb = -parNb;
        } else {
          y = 1;
          while ((parametres[j][y] != '\0')) {
            if (!estDecimal(parametres[j][y])) {
              printf("Erreur : La valeur immediate a la ligne %d n'est pas sous la bonne forme\n", ligneActuelleIndice + 1);
              printf("Indication : La valeur immediate doit etre un entier ecrit en \n");
              printf("             decimal (positif ou negatif (ex : #1 ou #-1)\n");
              printf("             ou en hexadecimal (positif ex : #h1) ! ligne %d\n", ligneActuelleIndice + 1);
              erreurLigne = 0;
            }
            // Convertir le caractere ascii en entier (le caractere '0' est 48 en decimal)
            nbAux = (parametres[j][y] - 48) * pow(10, strlen(parametres[j]) - 1 - y);
            parNb += nbAux;
            y++;
          }
        }
      }
      ligneTraduitDecimal[3] = 1;
      if (parNb < -32768 || parNb > 32767) {
        printf("Erreur : La valeur immediate n'est pas donnee sur 16 bits a la ligne %d\n", ligneActuelleIndice + 1);
        printf("Indication : Verifier que cette valeur est dans l'intervalle [-32768 ; 32767]\n");
        erreurLigne = 0;
      }
      ligneTraduitDecimal[4] = parNb;
    }
    
    else {
      int registre = 0;
      if (parametres[j][0] == 'r'){
        int etiqR = 0; 
        for (int i = 0; i < NbLignes ; i++){
          if (strcmp(tableauEtiquettes[i],parametres[j]) == 0){
            etiqR = 1;
          }
        }
        registre = (etiqR == 1) ? 0 : 1;
      }
      // Si l'argument est un registre, on traduit son numero en decimal
      if (registre == 1) {  
        y = 1;
        while ((parametres[j][y] != '\0')) {
          // Convertir le caractere ascii en entier (0 car = 48 en decimal)
          nbAux = (parametres[j][y] - 48) * pow(10, strlen(parametres[j]) - 1 - y);
          parNb += nbAux;
          y++;
        }
        if (j != 2 && (ligneTraduitDecimal[0] < 20 || ligneTraduitDecimal[0] > 26)) {
          ligneTraduitDecimal[j + 1] = parNb;
        } else {
          ligneTraduitDecimal[3] = 0;
          ligneTraduitDecimal[4] = parNb;
        }
        if (parNb < 0 || parNb > 31) {
          printf("Erreur : Le registre '%s' a la ligne %d n'existe pas\n", parametres[j], ligneActuelleIndice + 1);
          erreurLigne = 0;
        }
      }
      // Si l'argument est une etiquette , on calcule son adresse
      else {   
        // Indice auxiliaire qui va indiquer sur l'indice de l'etiquette dans le tableau des etiquettes
        i = 0; 

        // Variable qui = 1 si on a trouve l'indice de l'etiquette dans le tableau des etiquettes
        int etiquetteEstDansTableau = 0
        ;
        // On cherche l'indice de l'etiquette dans e tableau des etiquettes
        while (i < NbLignes && !etiquetteEstDansTableau) {
          y = 0;
          while (tableauEtiquettes[i][y] != '\0' && parametres[j][y] != '\0' &&
                tableauEtiquettes[i][y] == parametres[j][y]) {
            y++;
          }
          if (y == strlen(parametres[j]) && y == strlen(tableauEtiquettes[i])) {
            etiquetteEstDansTableau = 1;
            // Des qu'on trouve l'indice de l'etiquette et comme chaque
            // instruction fait 32 bits, soit 4 octets, on
            // multiplie l'indice par 4
            parNb = i * 4;
          }
          i++;
        }
        if (!etiquetteEstDansTableau) {
          printf("Erreur : L'etiquette '%s' a la ligne %d n'existe pas\n", parametres[j], ligneActuelleIndice + 1);
          printf("Indication : Si vous utilisez des nombre decimaux/hexa en tant qu'adresse de saut,\n");
          printf("             verifiez qu'ils sont precedes par '#'\n");
          erreurLigne = 0;
        } else {
          ligneTraduitDecimal[3] = 1;
          ligneTraduitDecimal[4] = parNb;
        }
        }
    }
    j++;
  }
  // On stocke la ligne traduit 
  *ligneTraduit = (ligneTraduitDecimal[0] << 27) + (ligneTraduitDecimal[1] << 22) +
                   (ligneTraduitDecimal[2] << 17) + (ligneTraduitDecimal[3] << 16) +
                   ((0xffff)&ligneTraduitDecimal[4]);
  return erreurLigne;
}