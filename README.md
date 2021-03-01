# Projet de compilation : Compilateur myC

Réalisé par : Ibrahim LMOURID et Hugo BERNARD

## Utilisation en ligne de commandes

- Avant la première utilisation: 
 `chmod +x compil` 

- Pour compiler un fichier .myc:
 `./compil <fichier>.myc`
    >Résulat: 
    - un exécutable **test** situé dans le répertoire **test/**
    - les fichiers **test/test.c** et **test/test.h**
                                                                 
- En particulier pour compiler nos tests: 
 `./compil test/test.myc`

- Pour exécuter le fichier obtenu: 
 `./test/test`
   >Résulat: 
    - affichage des fonctions appelées
    - pour chacune de ces fonctions, le nom et la valeur de ses paramètres
    - les valeurs de retour 

- Pour supprimer les différents fichiers générés :
  `make clean`


## Travail réalisé 

### Points demandés

 1. Un mécanisme de déclaration explicite de variables --> **fonctionnel**
 
 2. Des expresssion arithmétiques arbitraires de type calculatrice --> **fonctionnel**
 
 3. Des lectures ou écritures mémoires via des affectations avec variables 
    utilisateurs ou pointeurs --> **fonctionnel**
    
 4. Des structures de contrôles classiques (conditionelles et boucles) --> **fonctionnel**
 
 5. Un mécanisme de typage simple comprenant notamment des entiers int et 
    des pointeurs int * --> **fonctionnel**
    
 6. Définitions et appels de fonctions récursives --> **fonctionnel**
 
 
 ### Commentaires
 
 - Existence d'un type float, mais inutilisable comme paramètre ou valeur de retour d'une fonction
 
 - Inutilité des pointeurs, car impossible à initialiser dans la grammaire (pas d'opérateur &
   pour récupérer l'adresse d'une variable)
 
 - En cas d'erreur détectée par le compilateur myC, la ligne concernée est affichée dans le 
   message d'erreur. Ensuite, le script *compil* s'occupe de la suppression des fichiers inutiles du dépôt
   
 - Tout le code à trois adresses est généré dans *test.c* par le fichier *Production_de_code.c*
 
 - La pile d'appel ainsi que les fonctions nécessaires à son utilisation sont générées directement 
   par le compilateur au début du fichier *test.c*, afin de rendre le code à trois adresses généré
   plus lisible que si on y introduisait des manipulations de pointeurs.
   
 - Les tests réalisés mettent en évidence la capacité du compilateur à générer du code à trois adresses
   dans un maximum de situations (de la déclaration de varaible à l'appel de fonctions récursives). 
   
 - Tous ces tests sont concluants, seule la fonction testant les opérations avec pointeurs ne peut pas être
   appelée car les pointeur ne pouvant pas être initialisés, cela générerait un segfault à l'exécution
   
 - Les variables locales des fonctions ne sont pas empilées sur la pile d'appel
 
 - En rapport avec le point précédent, nous ne pouvons pas libérer de registre sans 
   dérégler les résulats lors d'un appel de fonction récursive. Il y a donc un très grand nombre de 
   registres créés
   
 
 ### Travail potentiel restant

 - Gérer l'empilement des variables locales et donc la libération des registres. Une fonction free_registre 
   existe et le principe de création de registre permettrait de libérer des registres, néanmoins
   dans le cas d'appel de fonctions récursives, cela générerait des problèmes. 
 
 - Faire plus de vérifications et générer plus de messages d'erreur. Exemples non exaustifs : 
 
    - si une fonction n'a pas le bon nombre d'arguments -> erreur
    - si une fonction de type void a une valeur de retour -> erreur
    - inversement si une fonction de type int ne retourne rien
    - si la pile d'appel est pleine -> à l'exécution, erreur stack overflow
     
 - Probablement des bugs à découvrir
 
 - Extention du langage : meilleurs prise en charge des float, ajout dans la grammaire de l'opérateur
   & afin d'utiliser les pointeurs



