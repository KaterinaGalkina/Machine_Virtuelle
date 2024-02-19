// Pour compiler et executer (a remplacer le nom de fichier par celui que vous voulez executer)

// gcc -Wall *.c -o simulateur
// ./simulateur nomFich.txt

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "execute.h"
#include "traduction.h"
#define NbColonnes 200
#define NbLignes 1000

int main(int argc, char *argv[]) {
  // Definition des variables 

  // La collection de tous les etiquettes contenues dans le fichier
  char tableauEtiquettes[NbLignes][NbColonnes]; 

  // Ligne actuelle
  char line[NbColonnes];     

  // Tous les lignes du fichier       
  char *tableauLignes[NbLignes]; 

  // Variable qui vaut 1 si pas d'erreur et 0 si erreur
  int ErrorPres = 1;

  // Le fichier avec le programme a executer
  FILE *fichier_orig = fopen(argv[1], "r");
  if (fichier_orig == NULL) {
    return 1;
  }

  // Indice qui aide a remplir le tableau "tableauLignes" et des que
  // le fichier est parcouru va indiquer le nombre de ligne
  // dans le fichier source
  int nbLines = 0; 

  // Variable qui indique la presence d'une nouvelle etiquette
  // sur une ligne ("ici : ..")
  int etiquettePres;

  printf("\n");
  // On parcourt le fichier pour trouver tous les etiquettes
  while (fgets(line, NbColonnes, fichier_orig) != NULL) {
    tableauLignes[nbLines] = strdup(line);
    etiquettePres = collectionDesEtiquettes(nbLines, tableauLignes[nbLines], tableauEtiquettes);
    if (etiquettePres == -1) {
      printf("Erreur : Le format d'une etiquette est invalide invalide a la ligne %d\n", nbLines + 1);
      ErrorPres = 0;
    }
    nbLines++;
  }
  if (doublonEtiquette(tableauEtiquettes, nbLines)) {
    printf("Erreur : Doublon d'une etiquette a la ligne %d\n", doublonEtiquette(tableauEtiquettes, nbLines + 1));
    
    ErrorPres = 0;
  }
  fclose(fichier_orig);

  // tableau qui contient les nouvelles lignes
  int ligneTraduit[NbLignes];

  // Indice avec lequele on va parcourir le tableau "tableauLignes" et
  // qui va indiquer la ligne ou on se trouve
  int ligneActuelleIndice = 0;
  while (ligneActuelleIndice < nbLines) {
    if (estLigneVide(tableauLignes[ligneActuelleIndice], 0)) {
      // On verifie si la ligne contient une initialisation d'une etiquette
      etiquettePres = collectionDesEtiquettes(ligneActuelleIndice, tableauLignes[ligneActuelleIndice], tableauEtiquettes);

      // verifie si il n'y a pas d'erreur sur la ligne et si il n'y en a pas eu avant
      ErrorPres = traduction(ligneActuelleIndice, tableauLignes[ligneActuelleIndice], tableauEtiquettes, etiquettePres, &ligneTraduit[ligneActuelleIndice]) && ErrorPres;
      ligneActuelleIndice++;
    } else {
      printf("Erreur : La ligne %d est vide\n", ligneActuelleIndice + 1);
      printf("Indication : Enlever cette ligne pour reessayer d'executer votre programme\n");
      ErrorPres = 0;
      ligneActuelleIndice++;
    }
  }

  // Si aucune erreur n'a ete detectee sur l'ensemble du fichier, 
  // nous traduisons en langage machine et executons le code
  // Sinon, on indique a l'utilisateur qu'il s'est trompe
  if (ErrorPres != 0) { 
    // Fichier contenant la traduction en langage machine
    FILE *fichierTraduitHexa = NULL;
    fichierTraduitHexa = fopen("hexa.txt", "w");
    if (fichierTraduitHexa == NULL) {
      fclose(fichierTraduitHexa);
      return 1;
    }
    for (int i = 0; i < nbLines; i++) {
      int nbZeros = zeroAAjouter(ligneTraduit[i]);
      for (int j = 0; j < nbZeros; j++) {
        fprintf(fichierTraduitHexa, "0");
      }
      fprintf(fichierTraduitHexa, "%x\n", ligneTraduit[i]);
    }
    fclose(fichierTraduitHexa);
    srandom(time(NULL));
    programInMem(&nombreDeLignes);
    PC = &Mem[0];
    // Tant qu'il y a une instruction a executer, on continue 
    while (PC){      
      ligne(PC, &CodeOp, &dest, &source1, &imm, &source2);
      // Pour les commandes liees a la memoire, on doit s'assurer qu'il n'y a pas d'erreur de segmentation 
      if (10 <= CodeOp && CodeOp <=11) {
        int valeurDeDeuxiemeSource = imm ? source2 : R[source2];
        if (((0 <= R[source1]+ valeurDeDeuxiemeSource) && (R[source1] + valeurDeDeuxiemeSource < nombreDeLignes * 4)) ||
         (CodeOp ==11 && 0 <= ((R[source1] + valeurDeDeuxiemeSource)%65536) && ((R[source1]+ valeurDeDeuxiemeSource + 1)%65536) < nombreDeLignes * 4)) {
          printf("Erreur : Vous essayez d'acceder a l'endroit ou votre programme est stocke !\n");
          PC = NULL;
        }
      }
      if (12 <= CodeOp && CodeOp <=13) {
        int valeurDeDeuxiemeSource = imm ? source2 : R[source2];
        if ((0 <= (R[dest] + valeurDeDeuxiemeSource) && (R[dest] + valeurDeDeuxiemeSource) < nombreDeLignes * 4) ||
        (CodeOp == 13 && 0 <= (((R[dest] + valeurDeDeuxiemeSource)+1)%65536) && (((R[dest] + valeurDeDeuxiemeSource)+1)%65536) < nombreDeLignes * 4)) {
          printf("Erreur : Vous essayez de stocker de l'information a l'endroit ou votre programme est stocke ! \n");
          PC = NULL;
        }
      }
      // Si il n'y a pas d'erreur de segmentation on continue a executer 
      if (PC) {
        (*tabCommandes[CodeOp])();
        R[0] = 0;
      }
    }
  } else {
    printf("\nLe fichier n'a pas pu etre traduit\n\n");
  }
  return 0;
}