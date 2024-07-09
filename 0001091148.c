/* LATINI LUCA 0001091148 GRUPPO B luca.latini2@studio.unibo.it */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
 
typedef struct Node{
    int i;
    int j;
    int wet;
    int d;
    int wet_count;
    struct Node *p;
} Node;
 
typedef struct{
    int rows;
    int cols;
    Node ***g;
} Graph;
 
/* 
la segeunte funzione verifica che le coordinate (i;j) all'interno
della tabella che memorizza il grafo siano valide. Nel caso in cui i e j siano < di 0 
ritorna 0. Nel caso in cui i>G->rows siamo fuori dai limiti verticali del grafo 
invece se fosse j>G->cols allora siamo fuori dai limiti orizzontali del grafo.
se (i;j) è all'interno dei limiti della tabella ed esiste un nodo a quelle coordinate allora mi restituisce 1
*/
int valid(int i, int j, Graph *G){
    if(i<0 || i>=G->rows || j<0 || j>=G->cols) return 0;
    else return (G->g[i][j] != NULL);
}
 
Node *node_create(int i, int j){
    Node *tmp = calloc(sizeof(Node), 1);
    if(tmp){
        tmp->i = i;
        tmp->j = j;
        tmp->wet = 1;
        tmp->d = INT_MAX;
    }
    return tmp;
}
 
/* 
la funzione g_create crea un grafo partendo da un array bidimensionale. 
viene creata una matrice (tabella) di puntatori a puntatori a nodi (node). Nei due cicli for
nidificati vengono eseguiti per scorrere tutte le righe e colonne dell'array input. 
Durante questa iterazione, vengono creati nodi Node solo se 
il carattere corrispondente nell'array input è '0'(percorribile). Se è '0',
 viene chiamata la funzione node_create(i, j) per creare un
 nodo nella posizione (i, j) e il puntatore a questo nodo viene
 memorizzato nella matrice tabella alla posizione (i, j).
Nei due successivi cicli for annidati si scorre nuovamente l'input 
Durante questa iterazione, se il carattere nell'array input non è '0',
 viene estratto un valore intero h da esso sottraendo 
il valore ASCII del carattere '0'(ossia le celle caselle non bagnate dalla pioggia)
. Quindi, viene eseguito un ciclo while per impostare il campo wet di nodi specifici a zero
*/
Graph g_create(char **input, int rows, int cols){
    Graph G;
    int i, j, h;
    struct Node*** tabella = calloc(sizeof(Node **), rows);
    for(i = 0; i<rows; i++){
        tabella[i] = calloc(sizeof(Node *), cols);
    }
    for(i = 0; i<rows; i++) {
        for (j = 0; j < cols; j++) {
            if (input[i][j] == '0') {
                tabella[i][j] = node_create(i, j);
            }
        }
    }
 
    for(i = 0; i<rows; i++) {
        for (j = 0; j < cols; j++) {
            if (input[i][j] != '0') {
                h = input[i][j] - '0';
                while(h){
                    if((j+h < cols) && (tabella[i][j+h])) tabella[i][j+h]->wet = 0;
                    h--;
                }
            }
        }
    }
    
    G.rows = rows;
    G.cols = cols;
    G.g = tabella;
    return G;
}
 
/* la funzione g_destroy libera la memoria allocara per il grafo
i due cicli for annidati permetto di attraversare ogni cella del grafo il 
primo itera attravero le righe il secondo itera attraverso le colonne 
. Dopo aver liberato la memoria di tutti gli elementi della riga free(G->g[i][j]),
si passa a liberare la memoria  associata all'array di puntatori di quella riga
free(G->g[i]).Infine, una volta che tutti gli elementi della matrice 
sono stati liberati, viene chiamata free su G->g 
per liberare la memoria associata alla matrice stessa
*/ 
void g_destroy(Graph *G){
    int i, j;
    for(i = 0; i<G->rows; i++){
        for(j = 0; j<G->cols; j++){
            free(G->g[i][j]);
        }
        free(G->g[i]);
    }
    free(G->g);
}
 
typedef struct q{
    Node *n;
    struct q *next;
}q_elem;
 
typedef q_elem **Queue;
 
/*
 la funzione enqueue crea un nuovo elemento della coda 
con il nodo specificato e lo inserisce come primo elemento
 nella coda. La coda è rappresentata come una lista collegata,
 e questa operazione di inserimento viene eseguita in modo che
 il nuovo elemento diventi la nuova testa della lista, e quindi
 l'elemento successivo nella coda è l'elemento precedente 
(quello che era precedentemente in testa alla coda). 
*/
void enqueue(Queue Q, Node *n){
    q_elem *new = malloc(sizeof(q_elem));
    new->next = *Q;
    new->n = n;
    *Q = new;
}
 
/* 
dequeue utilizza un tmp per iterare attrverso la lista
l'elemento ret un puntatore a Node restituirà l'elemento rimosso
dalla coda. se la coda esiste e presenta almeno un elemento
la funzione procede a controllare se ci sono altri elementi nella coda 
oltre a quello in cima. Questo viene fatto con l'if successivo 
che verifica se (*Q)->next è diverso da NULL. Se c'è più di un 
elemento nella coda, la funzione entra nel ramo dell'if.
tmp scorrerà fino al penultimo elemento così facendo punterà 
all'elemento da rimuovere.Viene quindi assegnato a ret 
il valore di tmp->next->n, che è un puntatore al nodo Node
 rappresentante l'elemento.
L'elemento puntato da tmp->next 
viene quindi rimosso dalla memoria con free(tmp->next). 

*/
Node *dequeue(Queue Q){
    q_elem *tmp;
    Node *ret = NULL;
 
    if(Q && *Q){
        if((*Q)->next){
            tmp = *Q;
            if((*Q)->next->next) {
                while (tmp->next->next) tmp = tmp->next;
            }
            ret = tmp->next->n;
            free(tmp->next);
            tmp->next = NULL;
        }
        else{
            ret = (*Q)->n;
            free(*Q);
            *Q = NULL;
        }
    }
    return ret;
}
 
/*
La funzione relax prende in ingresso 
src:Rappresenta il nodo sorgente da cui stiamo 
cercando di raggiungere il nodo di destinazione (dst).
dst: È il nodo di destinazione che stiamo cercando di raggiungere.
Q: È una coda che viene utilizzata per gestire i nodi 
da esaminare nel contesto dell'algoritmo.
Se dst->d > src->d + 1 , allora possiamo "rilassare" 
il nodo in quanto abbiamo trovato un percorso più 
corto per raggiungere dst ( scelta prioritaria percorso minimo).
nel caso di percorsi con la medesima distanza (dst->d == src->d + 1), 
in questo caso se dst->wet_count è maggiore di src->wet_count + dst->wet
il nodo può essere "rilassato".
soddisfatte le condizioni dell'if il rilassamento prevedono:
dst->d = src->d + 1: Aggiorna la distanza di dst per indicare
che ora è possibile raggiungerlo con un percorso più corto.
st->wet_count = src->wet_count + dst->wet: 
il numero di caselle totali bagnate sarà uguale a quelle 
numero di caselle bagnate fino al nodo sorgente più il 
contatore wet del nodo destinazione che può essere 1 o 0.
dst->p = src: Imposta il predecessore di dst come src.
enqueue(Q, dst): Aggiunge il nodo dst alla coda Q per 
ulteriori esplorazioni (visto che i suoi valori di 
distanza e celle bagnate sono stati aggiornati).
 
*/
void relax(Node *src, Node *dst, Queue Q){
    if((dst->d > src->d + 1) || ((dst->d == src->d + 1) && (dst->wet_count > src->wet_count + dst->wet))){
        dst->d = src->d + 1;
        dst->wet_count = src->wet_count + dst->wet;
        dst->p = src;
        enqueue(Q, dst);
    }
}


/*
Questo algoritmo prende un grafo, una posizione di partenza (i, j) 
all'interno del grafo e cerca di esplorare il grafo  in ampiezza, 
visitando tutti i nodi raggiungibili dal nodo di partenza(0;0 , 
momento della chiamata della funzione).
La creazione della coda Q serve per tenere traccia dei nodi da 
esaminare durante la ricerca .
Il nodo iniziale G->g[i][j](G->g[0][0], al momento della chiamata della funzione),
viene inserito nella coda diventando il punto di partenza.
la distanza di G->g[i][j] viene inizializzata ad 1 e le caselle totali bagnate
vengono inizializzare pari al contarore wet di quel nodo.
fin quando la coda Q non è vuota si ripetono le istruzioni 
all'interno del while. Per ogni nodo estratto dalla coda
 si verifica per il suoi nodi vicini (sopra , sotto , sinistra e destra) che 
siano "validi" (entro i bordi del grafo ed esistente), se un vicino del nodo è valido
viene chiamata la funzione relax, per verifacare se sia possibile o meno
rilassarlo. Quando il ciclo while termina, significa che tutti 
i nodi raggiungibili dal nodo di partenza sono stati esplorati.
La memoria allocata per la coda Q viene liberata utilizzando free.
*/
void breadth_first_dijkstra(Graph *G, int i, int j){
    Node *n;
    Queue Q = calloc(sizeof(q_elem *), 1);
    enqueue(Q, G->g[i][j]);
 
    G->g[i][j]->d = 1;
    G->g[i][j]->wet_count = G->g[i][j]->wet;
 
    while((n = dequeue(Q))){
        if (valid(n->i+1, n->j, G)) relax(n, G->g[n->i+1][n->j], Q);
        if (valid(n->i-1, n->j, G)) relax(n, G->g[n->i-1][n->j], Q);
        if (valid(n->i, n->j+1, G)) relax(n, G->g[n->i][n->j+1], Q);
        if (valid(n->i, n->j-1, G)) relax(n, G->g[n->i][n->j-1], Q);
    }
 
    free(Q);
}

/* la funzione print_direction stampa video la seguenza del
percorso minimo. La funzione utilizza una ricorsione per risalire 
dal nodo "current" al suo predecessore, fino a raggiungere il nodo 
iniziale del cammino. A ogni passo della ricorsione, viene 
controllata la posizione relativa del nodo corrente e del
 suo predecessore sulla griglia del grafo, e viene stampata 
la lettera corrispondente alla direzione di provenienza
 (S=Sud, N=Nord, E=Est, O=Ovest).

*/
void print_direction(Graph *G, Node *current){
    Node *parent = current->p;
    if(parent){
        if (parent->i == current->i - 1) {
            print_direction(G, parent);
            putchar('S');
        } else if (parent->i == current->i + 1) {
            print_direction(G, parent);
            putchar('N');
        } else if (parent->j == current->j - 1) {
            print_direction(G, parent);
            putchar('E');
        } else if (parent->j == current->j + 1) {
            print_direction(G, parent);
            putchar('O');
        }
    }
}
 
int main(int argc, char *argv[]) {
    FILE *filein;
    int rows, cols, i, j;
    char **input;
    Graph G;
 
    if (argc != 2){
        printf("-1 -1\n");
        return 1;
    }
 
/* prende in esame il secondo argomento di argv ossia il nome del file 
e lo apre in modalità lettura , in row e col vado ad inserire tramite la scanf 
i valori trovati nelle prime due righe del file andando così ad 
allocare memoria a l'input (la mappa della città) , nel ciclo for 
vado ad inizializzare tutte le righe spostandomi colonnna per colonna 
*/
    filein = fopen(argv[1], "r");
 
    fscanf(filein, "%d %d", &rows, &cols);
    input = malloc(sizeof(char *) * rows);
    for (i = 0; i<rows; i++){
        input[i] = malloc(sizeof(char) * (cols+1));
        fscanf(filein, "%s", input[i]);
    }
 
    G = g_create(input, rows, cols);
 
    fclose(filein);
    for (i = 0; i<rows; i++){
        free(input[i]);
    }
    free(input);
 
    breadth_first_dijkstra(&G, 0, 0);
    
    /*destinazione*/
    i = rows - 1;
    j = cols - 1;
 
    if(G.g[i][j]->p){
        printf("%d %d\n", G.g[i][j]->d, G.g[i][j]->wet_count);
        print_direction(&G, G.g[i][j]);
        putchar('\n');
    }else{
        printf("-1 -1\n");
    }
 
    g_destroy(&G);
 
    return 0;
}
 
 
 
 