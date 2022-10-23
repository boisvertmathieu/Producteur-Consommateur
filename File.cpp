#include "File.h"
#include <iostream>

using namespace std;

// Constructeur
File::File() {
    flush = false;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condInsere, NULL);
    pthread_cond_init(&condRetirer, NULL);
}

// Destructeur
File::~File() {
    while (!FileItems.empty()) {
        FileItems.pop();
    }
}

/**************************************************************************************
  Méthode CodeFile File::Retire(Produit & p)
    Cette méthode retire un produit de la file.
    Si la file est vide, cette méthode doit bloquer.

    La fonction retourne :
        FILE_ITEM_VALIDE    si l'item est valide
        FILE_TERMINEE       si la file est vide et que la file a recu la commande
                            Flus && FileItems.empty()hConsommateurs();

 **************************************************************************************/
CodeFile File::Retire(Produit &p) {
    pthread_mutex_lock(&mutex);

    while (FileItems.empty()) {
        // Si la condition de flush des consommateurs est à `true`, on libère le mutex et on
        // retourne FILE_TERMINEE
        if (flush) {
            cout << "--File:Retire() La file termine, nous devons quitter." << endl;
            pthread_mutex_unlock(&mutex);
            return FILE_TERMINEE;
        }

        // On attend que la file ne soit plus vide avec la condition `condRetirer`
        cout << "--File:Retire() La file est vide. Allons dormir!" << endl;
        pthread_cond_wait(&condRetirer, &mutex);
    }

    // On associe la valeure qui sera enlevée de la file au Produit `p`
    p = FileItems.front();
    FileItems.pop();
    cout << "--File::Retire() produit avec numero de serie " << p.GetNumProduit()
         << ". Nombre de produits dans la file = " << FileItems.size() << endl;

    // On signal à un thread producteurs qu'un produit a été enlevé de la file
    pthread_cond_signal(&condInsere);
    pthread_mutex_unlock(&mutex);

    return FILE_ITEM_VALIDE;
}

/**************************************************************************************
  Méthode CodeFile File::Insere(Produit & p)
    Cette méthode insere un produit dans la file.
    Si la file est pleine, cette méthode doit bloquer.

    La fonction retourne :
        FILE_ITEM_VALIDE    si l'item est valide

 **************************************************************************************/
CodeFile File::Insere(Produit &p) {
    pthread_mutex_lock(&mutex);
    while (FileItems.size() == MAX_PRODUITS_FILE) {
        // On attend que la file ne soit plus pleine avec la condition `condInsere`
        cout << "++File::Insere() File pleine! Allons dormir!" << endl;
        pthread_cond_wait(&condInsere, &mutex);
    }

    FileItems.push(p);

    // On signale à un thread consommateur qu'un produit a été inséré dans la file
    pthread_cond_signal(&condRetirer);
    pthread_mutex_unlock(&mutex);

    cout << "++File::Insere() produit avec numero de serie " << p.GetNumProduit()
         << ". Nombre d'items dans la file = " << FileItems.size() << endl;
    return FILE_ITEM_VALIDE;
}

/***************************************************************************************
  Fonction int File::FlushConsommateurs(void)
    Cette méthode est appelée pour indiquer à la file qu’aucun nouvel item
    ne sera inséré dans la file à partir de ce moment. Après cet appel, la file continue de fonctionner normalement,
    jusqu’au moment où la file est vide.

    La fonction retourne simplement le nombre d'item present dans la file.

 **************************************************************************************/
int File::FlushConsommateurs(void) {
    // On met le boolean représentant le flush des consommateurs à true, et on envoie un message
    // à tous les threads consommateurs que plus aucune produit ne sera ajouté à la queue
    flush = true;
    pthread_cond_broadcast(&condRetirer);
    return (int)FileItems.size();
}
