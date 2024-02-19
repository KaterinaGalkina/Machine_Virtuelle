#define NbColonnes 200
#define NbLignes 1000

// Renvoie 1 si char ch est dans cette liste : 0123456789, 0 sinon
int estDecimal(char ch);

// Renvoie 1 si char ch est dans cette liste : 0123456789abcdef, 0 sinon
int estHexadecimal(char ch);

// Fonction qui retourne le nombre de zeros à rajouter avant le
// debut de la chaine hexa pour avoir la chaine complete
int zeroAAjouter( int val);

// Verifie si la ligne passe en parametres est vide, renvoie 1 dans ce cas, sinon - 0
int estLigneVide(char ligne[], int i);

// Convertit une chaine de caracteres ecrit en hexa en un nombre decimal
long int chaineHexaEnDecimal(char hex[]);

// Fonction qui parcourt la ligne actuelle et stocke les etiquettes trouvees
// dans le tableau "allTags"
int collectionDesEtiquettes(int index, char actualLine[], char allTags[NbLignes][NbColonnes]);

// Fonction qui parcourt le tableaux d'etiquettes et renvoie 1 si il trouve un
// doublon, et 0 sinon
int doublonEtiquette(char allTags[NbLignes][NbColonnes], int nbLines);

// Fonction pour traduire une ligne du fichier du code assembleur en code hexadecimal, 
// renvoie
int traduction(int ligneActuelleIndice, char ligneActuelle[ligneActuelleIndice], char tableauEtiquettes[NbLignes][NbColonnes], int etiquettePres, int *ligneTraduit);