#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

// non stampa con il formato corretto
// non stampa il risultato corretto
// capire come funziona acquisiione e lavorazione
// rifare algoritmo calcolo IP

// ========================== DEFINE ==========================
#define RESERVERIP 2
#define IPLENGHT 32
#define MAXNAMELENGHT 50
#define IPLENGHT 32
#define MAXLENGHTIP 20

// ========================== DEFINIZIONE LISTE e STRUTTURE ==========================

typedef struct networkTable_s {
    // head1
    char networkName[MAXNAMELENGHT];
    int hostNumber;    // numero host richiesti
    int bitHost;     // bit necessari per gli host (calcolato)
    struct networkTable_s *next;
} networkTable_t;

typedef struct group_s {
	// head2
    int bitHost;                    // bit host del gruppo
    int networkCount;               // quante reti in questo gruppo
    struct group_s *next;
} group_t;

typedef struct IPlist_s {
	// head3
    uint32_t ip;                // ip assegnato (in formato intero)
    int netmask;                // maschera in bit (es. 24)
    int bitHost;                // bit host usati (per ordinamento/visualizzazione)
    struct IPlist_s *next;
} IPlist_t;


// ========================== PROTOTIPI ==========================

// lista 1
networkTable_t* aquisizioneLista1(networkTable_t *, int *);
void print_network_list(networkTable_t *);

// lista 2
group_t* find_groups(networkTable_t *, group_t *, int *);
group_t* append_group(group_t *, int , int);
void print_group_list(group_t *);

// parte 3
void assegna_subnet(group_t *head2, networkTable_t *head1, uint32_t starterIp, int netmaskBit, uint32_t ipMax);
char *ip_to_string(uint32_t ip, char *buffer);
void stampa_subnet(uint32_t subnetIP, int netmask, networkTable_t **head, int livello);

// ========================== MAIN ==========================
int main() {

    // 1 - Acquisizione e ordinamento della lista 1
    int bitHost;
    int *ptr_bitHost = &bitHost;

    networkTable_t *head1 = NULL;
    head1 = aquisizioneLista1(head1, ptr_bitHost);

    // 2 - Ordinamento in gruppi e creazione della lista 2
    group_t *head2 = NULL;
    head2 = find_groups(head1, head2, ptr_bitHost);

    // 3 - Acquisizione IP di partenza e netmask
    char buffer[MAXLENGHTIP];
    char ip_str[MAXLENGHTIP];
    unsigned int b1, b2, b3, b4;
    uint32_t starterIp;
    int netmaskBit;
    uint32_t ipMax;

    printf("\n--- DISTRIBUZIONE SUBNET ---\n");

    printf("Inserisci IP di partenza (es: 131.175.184.0): ");
    scanf("%15s", ip_str);

    if (sscanf(ip_str, "%u.%u.%u.%u", &b1, &b2, &b3, &b4) != 4
        || b1 > 255 || b2 > 255 || b3 > 255 || b4 > 255) {
        printf("IP non valido!\n");
        return 1;
    }

    starterIp = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;

    printf("Inserisci netmask (es: 21): ");
    scanf("%d", &netmaskBit);
    printf("\n");

    // Calcolo indirizzo IP massimo per il blocco iniziale
    ipMax = starterIp | ((1 << (IPLENGHT - netmaskBit)) - 1);

    // Avvio assegnamento subnet
    assegna_subnet(head2, head1, starterIp, netmaskBit, ipMax);

    return 0;
}


// ========================== SOTTOPROGRAMMI ==========================

// lista 1
networkTable_t* aquisizioneLista1(networkTable_t *head1, int *ptr_bitHost) {
    char networkName[50];
    int hostNumber;

    printf("\n");
    printf("inserire numero di host e nome delle reti ('.' per terminare)\n");

    while (true) {
        printf("Inserisci nome della rete : ");
        scanf("%s", networkName);

        if (strcmp(networkName, ".") == 0)
            break;

        printf("Inserisci numero di host: ");
        scanf("%d", &hostNumber);
        
        *ptr_bitHost = (int) ceil(log2(hostNumber + RESERVERIP));
        networkTable_t *new = malloc(sizeof(networkTable_t));

        if (!new) {
            printf("Errore allocazione memoria 'aquisizioneLista1'\n");
            continue;
        }

        strcpy(new->networkName, networkName);
        new->hostNumber = hostNumber;
        new->bitHost = *ptr_bitHost;
        new->next = NULL;

         // Caso 1: lista vuota o va messo in testa
        if (head1 == NULL || new->bitHost > head1->bitHost) {
            new->next = head1;
            head1 = new;

        // Caso 2: inserimento in mezzo o in fondo
        } else {
            networkTable_t *current = head1;
            while (current->next != NULL && current->next->bitHost >= new->bitHost) {
                current = current->next;
            }
            new->next = current->next;
            current->next = new;
        }
    }

    // Stampo la lista completa una sola volta alla fine
    print_network_list(head1);
    return head1;
}
void print_network_list(networkTable_t *head) {
    if (head == NULL) {
        printf("La lista è vuota.\n");
        return;
    }

    printf("\n--- LISTA COMPLETA DELLE RETI ---\n");
    networkTable_t *temp = head;
    while (temp != NULL) {
        printf("Rete: %s | Host richiesti: %d | Bit host: %d\n", 
               temp->networkName, temp->hostNumber, temp->bitHost);
        temp = temp->next;
    }
}

// lista 2
group_t* find_groups(networkTable_t *head1, group_t *head2, int *ptr_bitHost) {
    int count;
    int i;
    networkTable_t *current1;

    if (!head1) {
        printf("Errore: lista vuota in 'find_groups'\n");
        return head2;
    }

    for (i = 32; i > 0; i--) {
        count = 0;
        current1 = head1;

        while (current1 != NULL) {
            if (current1->bitHost == i) 
                count++;

            current1 = current1->next;
        }

        if (count != 0)
            head2 = append_group(head2, i, count);
        
    }

    print_group_list(head2);
    return head2;
}
group_t* append_group(group_t *head, int bitHost, int networkCount) {
    group_t *newNode = malloc(sizeof(group_t));
    if (!newNode) {
        printf("Errore allocazione memoria in append_group\n");
        return head;
    }

    newNode->bitHost = bitHost;
    newNode->networkCount = networkCount;
    newNode->next = NULL;

    if (head == NULL) {
        return newNode;
    }

    group_t *current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = newNode;
    return head;
}
void print_group_list(group_t *head) {
    group_t *current = head;

    if (!current) {
        printf("Lista 2 vuota.\n");
        return;
    }

    printf("\n--- GRUPPI TROVATI ---\n");
    while (current != NULL) {
        printf("Bit host: %d | Numero reti: %d\n", current->bitHost, current->networkCount);
        current = current->next;
    }
}

// parte 3
char *ip_to_string(uint32_t ip, char *buffer) {
    sprintf(buffer, "%u.%u.%u.%u",
            (ip >> 24) & 0xFF,
            (ip >> 16) & 0xFF,
            (ip >> 8) & 0xFF,
            ip & 0xFF);
    return buffer;
}

void assegna_subnet(group_t *head2, networkTable_t *head1, uint32_t starterIp, int netmaskBit, uint32_t ipMax) {
    uint32_t currentIP;
    uint32_t nextSubnetToDivide;
    int nextSubnetMask;
    int livello;
    int bitHost;
    int subnetBits;
    int subnetSize;
    int finalNetmask;
    int newBitHost;
    int newSubnetBits;
    int newSubnetSize;
    int newFinalMask;
    uint32_t tmpIP;
    int j;
    int n;

    group_t *current2;
    networkTable_t *current1;

    currentIP = starterIp;       // IP da cui iniziare a lavorare
    nextSubnetToDivide = 0;      // Subnet libera da suddividere al prossimo gruppo
    nextSubnetMask = 0;          // Netmask di quella subnet libera
    livello = 0;                 // livello esterno

    current2 = head2;
    current1 = head1;

    while (current2 != NULL && current1 != NULL) {
        bitHost = current2->bitHost;
        subnetBits = IPLENGHT - netmaskBit - bitHost;
        subnetSize = (1 << bitHost);
        finalNetmask = netmaskBit + subnetBits;
        n = current2->networkCount;

        // Calcolo intervalli per verificare spazio
        uint32_t lastIPextra = currentIP + (n + 1) * subnetSize - 1;
        uint32_t lastIP = currentIP + n * subnetSize - 1;

        if (lastIPextra <= ipMax) {
            // Caso 1: assegno n subnet + 1 libera
            for (j = 0; j < n; j++) {
                stampa_subnet(currentIP, finalNetmask, &current1, livello + 1);
                currentIP += subnetSize;
            }
            // Salvo la subnet libera per la prossima suddivisione
            nextSubnetToDivide = currentIP;
            nextSubnetMask = finalNetmask;

            currentIP += subnetSize;  // salto subnet libera, la riservo per la prossima iterazione

        } else if (lastIP <= ipMax && nextSubnetToDivide != 0) {
            // Caso 2: spazio per n subnet, ma senza subnet libera
            // Ora suddivido la subnet salvata precedentemente in nextSubnetToDivide/nextSubnetMask

            newBitHost = bitHost;
            newSubnetBits = IPLENGHT - nextSubnetMask - newBitHost;
            newSubnetSize = (1 << newBitHost);
            newFinalMask = nextSubnetMask + newSubnetBits;

            tmpIP = nextSubnetToDivide;

            for (j = 0; j < n; j++) {
                stampa_subnet(currentIP, finalNetmask, &current1, livello + 1);
                currentIP += subnetSize;
            }

            // Azzeriamo la subnet libera perché l'abbiamo appena suddivisa
            nextSubnetToDivide = 0;
            nextSubnetMask = 0;

            currentIP = tmpIP;

        } else {
            // Caso 3: spazio insufficiente
            for (int k = 0; k < livello; k++) {
                printf("    ");
            }
            printf("!! ERRORE - spazio insufficiente per %d subnet\n", n);
            break;
        }

        livello++;  // aumento indentazione ad ogni nuovo gruppo


        // Nuovo punto di partenza: subnet libera salvata
        if (nextSubnetToDivide != 0) {
            currentIP = nextSubnetToDivide;
        }


        current2 = current2->next;
    }
}

void stampa_subnet(uint32_t subnetIP, int netmask, networkTable_t **head, int livello) {
    char buffer[17];
    ip_to_string(subnetIP, buffer);

    printf("|");
    // Costruiamo l'indentazione (esempio 4 spazi per livello)
    for (int i = 0; i < livello; i++) {
        printf("--");
    }

    printf(" %s/%d - %s\n", buffer, netmask, (*head)->networkName);

    *head = (*head)->next;
}








