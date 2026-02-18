DUQUEYROIX
SAUL

Bonjour, voici le rapport de mon projet de résolution du problème de Parenthésage Optimal d’une Chaîne de Multiplications de Matrices.

Étape 1: Analyse du programme de backtracking donné, je peux aller jusqu'à n=16 avec 5 instances mais cela prend environ 2 minutes d'éxécution

         gcc -Wall solve_backtracking.c -o solve_backtracking.exe
         ./generate_instances.c 1 16 10 50 gen_instances.txt
         ./solve_backtracking.exe gen_instances.txt solve_backtracking.txt

Étape 2: Élagage du backtracking, utilisation d'une boucle if pour enlever les cas où le coût calculé à un moment donné dépasse le coût optimal,
         à partir de n=20, le temps d'éxécution explose.

         gcc -Wall solve_backtracking_elag.c -o solve_backtracking_elag.exe
         ./generate_instances.c 1 19 10 50 gen_instances.txt
         ./solve_backtracking_elag.exe gen_instances.txt solve_backtracking_elag.txt

Étape 3: Programmation dynamique: Création de la fonction dynamic_programing() qui fonctionne comme décrit dans l'énoncé + une fonction build_parentheses(int i,int j) 
        avec i et j pour le début et la fin de la sous chaine de matrices pour construire le parenthésage.
        On l'appelle avec build_parentheses(1,n) dans dynamic_programing(). 
        Pas de problèmes de temps d'éxécution qui reste assez faible même pour n élevé.

        gcc -Wall solve_dp.c -o solve_dp.exe
        ./generate_instances.c 1 80 10 50 gen_instances.txt
        ./solve_dp.exe gen_instances.txt solve_dp.txt

Étape 4: Greedy Algorithms: Création des deux algorithmes greedy1 et greedy2.
        Le coût n'est pas toujours égal au coût optimal délivré par la programmation dynamique mais se rapproche du coût en étant plus rapide que dp.
        À n très grand (250), greedy2 est bien plus rapide que greedy1 lui-même plus rapide que dp.
        Le temps d'éxécution de greedy1 est trop élevé, il est trop proche de dp. Cela est du au fait que j'ai utilisé un tableau de Nodes que je parcours 
        à chaque appel récursif donc forcément le nombre d'opérations augmentent et casse la complexité en temps.

        gcc -Wall solve_greedy1.c -o solve_greedy1.exe
        ./generate_instances.c 1 80 10 50 gen_instances.txt
        ./solve_greedy1.exe gen_instances.txt solve_greedy1.txt

        gcc -Wall solve_greedy2.c -o solve_greedy2.exe
        ./generate_instances.c 1 80 10 50 gen_instances.txt
        ./solve_greedy2.exe gen_instances.txt solve_greedy2.txt

Étape 5: Création de la propre heuristique: De manière similaire au greedy1, j'utilise un tableau de nodes(matrices), que je parcours,
         je cherche le meilleur trio c'est à dire celui pour lequel la multiplication des 3 matrices est le plus faible.
         il existe 2 cas possibles: ((Mi Mj) Mk) ou bien (Mi (Mj Mk)), je compare les 2 côuts et retiens le minimum, je remplace Mi
         par cette multiplication puis je décale Mj et Mk à la fin du tableau et rappelle la fonction avec n-2 en supposant que j'avais n matrices.
         En termes de coût, j'ai observé de nombreux cas où le coût est inférieur aux 2 autres greedy mais je ne crois pas qu'il existe de cas 
         où il est meilleur en temps car le calcul du min prend forcément du temps. Cependant à n grand, il reste bien mieux que dp.
         Même problème que greedy1, j'utilise un tableau de Nodes que je parcours à chaque appel ce qui casse la complexité en temps.

         gcc -Wall solve_greedyperso.c -o solve_greedyperso.exe
        ./generate_instances.c 1 80 10 50 gen_instances.txt
        ./solve_greedyperso.exe gen_instances.txt solve_greedyperso.txt

Étape 6: Écart de rang et étendre le backtracking: J'ai créé un fichier dans lequel j ai à n donné énumérer tous les parenthésages possibles,
         avec coût associé. Puis j'ai trié ces solutions en fonction du coût avec un tri par insertion. J'ai ensuite calculé les greedy 1 2 et perso
         puis regarder leur classement, écart, pourcentage.

         gcc -Wall backtracking_ext.c -o backtracking_ext.exe
        ./generate_instances.c 1 80 10 50 gen_instances.txt
        ./backtracking_ext.exe gen_instances.txt solve.txt

Merci d'avoir lu le rapport.
        


