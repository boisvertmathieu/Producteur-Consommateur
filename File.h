#ifndef FILE_H
#define FILE_H

#define MAX_PRODUITS_FILE 10 // Nombre de produit maximum que la file peut contenir

#include "Produit.h"
#include <queue>

// Code retourne par les methodes
enum CodeFile {
    FILE_TERMINEE,
    FILE_ITEM_VALIDE
};

class File {
private:
    // Une file FIFO de la STL
    std::queue <Produit> FileItems;

    // Vous pouvez ajouter ici d'autres variables privées
    pthread_cond_t condInsere;
    pthread_cond_t condRetirer;
    pthread_mutex_t mutex;
    bool flush;

public:
    File();

    ~File();

    CodeFile Retire(Produit &p);

    CodeFile Insere(Produit &p);

    int FlushConsommateurs(void);
};

#endif
