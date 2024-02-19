# Simulation d’une machine virtuelle

C’est un programme qui simule une machine fictive composee d’une memoire et d’un microprocesseur. Elle effectue deux taches : la premiere est un assembleur qui va transformer un programme ecrit en langage assembleur en un langage machine et la deuxieme c’est la simulation qui recupere le programme ecrit en langage machine et l'execute instruction par instruction. 

La description supplementaire sur la machine en soi et les commandes que vous pouvez utilisez se trouve dans le fichier user.pdf attachee, tandis que le fichier dev.pdf vous donnera l'information sur le processus de creation du projet. Vous pouvez egalement retrouver l'enonce complet du projet (sujet.pdf) avec toutes les explications sur le travail qui etait a faire.

## Compilation et execution du programme 

Pour compiler et executer le projet, ouvrez votre terminal et placez-vous dans le repertoire contenant le code source et ensuite executer les deux commandes suivantes : 

```bash
gcc -Wall *.c -o simulateur -lm
```

```bash
./simulateur tests/inversion.txt
```

## Description du programme dans l'exemple

Dans cet exemple, nous commencons par creer un fichier executable nomme "simulateur", puis nous l'exécutons en passant en parametres l'un des fichiers de tests.

Ici le fichier "inversion.txt" contient un simple programme, dont l'algorithme est decrit ci-dessous : 

```
ici: in r1
```

D'abord, nous initialisons une etiquette ("ici"), qui sera utilisee pour indiquer au programme l'adresse a laquelle il doit se rendre, puis nous attendons que l'utilisateur saisisse un nombre entier (positif ou negatif) dans un untervalle suivant : [-32768; 32767] qui sera stocke dans le registre 1 (in r1).

```
jzs fin
sub r1, r0, r1
out r1
```

A la prochaine ligne, nous verifions si l'utilisateur a entre 0 et, dans ce cas, nous arretons le programme en passent a l'adresse indique par l'etiquette "fin" apres quoi l'instruction "hlt" est executee pour arreter le programme. 

```
fin: hlt
```

Dans le cas contraire, le programme soustrait le nombre entré par l'utilisateur à 0, et ainsi le registre r1 contient l'oppose du nombre inital. Ensuite, ce nombre obtenu est affiche.

```
jmp ici
```

Finalement, nous sautons a l'adresse indique par l'etiquette "ici" avec un saut inconditionnel (jmp).

Le programme en entier :

```
ici: in r1
     jzs fin
     sub r1, r0, r1
     out r1
     jmp ici
fin: hlt
```

Pour les autres exemples de programme, vous pouvez consulter une description moins detaillee a la fin du fichier dev.pdf.