// =============================================
//        Code pour le TP2 d'IFT-2001
//                Automne 2022
// =============================================

#include "File.h"
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

using namespace std;

File MaFile;

#define N_PRODUCTEURS 3        // Nombre de threads producteurs a demarrer
#define N_CONSOMMATEURS 3      // Nombre de threads consommateurs a demarrer
#define MAX_ITEMS_APRODUIRE 50 // Nombre d'items maximum a produire

// Variable globale à protéger
int ProchainNumeroSerie = 1;
pthread_mutex_t MutexNumeroSerie;

// Declaration des fonctions des threads
void *ThreadProducteur(void *tid);

void *ThreadConsommateur(void *tid);

// Fonction main()
int main(int argc, char *argv[]) {

    // Tableaux contenant l'information sur les threads
    pthread_t threadsProd[N_PRODUCTEURS];
    pthread_t threadsCons[N_CONSOMMATEURS];
    int status, i, iP[N_PRODUCTEURS], iC[N_CONSOMMATEURS];

    srand(time(NULL)); // initialisation de rand
    pthread_mutex_init(&MutexNumeroSerie, NULL); // initialisation du mutex

    // Creer les threads producteurs
    for (i = 0; i < N_PRODUCTEURS; i++) {
        iP[i] = i + 1;
        cout << "main(): en train de creer le thread producteur " << iP[i] << endl;
        status = pthread_create(&threadsProd[i], NULL, ThreadProducteur, (void *) &iP[i]);
        if (status != 0) {
            cout << "oops, pthread a retourne le code d'erreur " << status << endl;
            exit(-1);
        }
    }

    // Creer les threads consommateurs
    for (i = 0; i < N_CONSOMMATEURS; i++) {
        iC[i] = i + 1;
        cout << "main(): en train de creer le thread consommateur " << iC[i] << endl;
        status = pthread_create(&threadsCons[i], NULL, ThreadConsommateur, (void *) &iC[i]);
        if (status != 0) {
            cout << "oops, pthread a retourne le code d'erreur " << status << endl;
            exit(-1);
        }
    }

    // A ce moment-ci, les threads producteurs et consommateurs tournent.
    // La fonction main attend que les producteurs terminent leur production
    for (i = 0; i < N_PRODUCTEURS; i++) {
        pthread_join(threadsProd[i], NULL);
    }

    // La file indique à son tour aux threads consommateurs qu'ils doivent quitter, quand il n'y a plus d'items a consommer.
    // Cette indication est faite par la methode File::Retire qui retourne FILE_TERMINEE.
    cout << "Nous devons flusher les consommateurs de la file!" << endl;;
    int ItemsRestants = MaFile.FlushConsommateurs();
    cout << "Il reste " << ItemsRestants << " items dans la file!" << endl;

    // Attendre maintenant que tous les consommateurs finissent de consommer les items restants
    /// dans la file, puis quittent.
    for (i = 0; i < N_CONSOMMATEURS; i++) {
        pthread_join(threadsCons[i], NULL);
    }

    // Le programme peut maintenant terminer
    exit(0);
}

// Thread producteur
void *ThreadProducteur(void *tid) {
    int myId = *(int *) tid;
    int NumeroSerieProduit;
    cout << "Producteur " << myId << ": debute la production des items!" << endl;
    while (true) {

        pthread_mutex_lock(&MutexNumeroSerie);
        NumeroSerieProduit = ProchainNumeroSerie;
        ProchainNumeroSerie += 1;
        pthread_mutex_unlock(&MutexNumeroSerie);

        if (NumeroSerieProduit > MAX_ITEMS_APRODUIRE) {
            // Le programme a suffisamment produit d'items. On quitte.
            cout << "Producteur " << myId << ": quitte car on a suffisamment produit d'items." << endl;
            pthread_exit(NULL);
        }
        // Sinon, on produit un item en pigeant un produit au hasard
        Produit Item;
        Item.SetNumProduit(NumeroSerieProduit);
        Item.PigerProduit();
        cout << "Producteur " << myId << ": genere le produit: (" << Item.GetNomProduit() << ", "
             << Item.GetPrixProduit() << "$) avec le numero de serie " << NumeroSerieProduit << endl;

        // On l'insere dans la file. Cet appel doit bloquer si la file est pleine.
        MaFile.Insere(Item);

        // Le producteur va dormir un temps aleatoire entre 0 et 50 ms. Ne modifiez rien!
        // Le timing a ete concu pour parfois ralentir les producteurs, parfois ralentir les
        // consommateurs. Ainsi, la file sera parfois vide, parfois pleine.
        float phase = fabs(sin(0.1 * (float) NumeroSerieProduit));
        float SleepTime = 5.0 * phase * (float(rand() % 10000));
        usleep(long(SleepTime));
    }
}

void *ThreadConsommateur(void *tid) {
    int myId = *(int *) tid;
    int ret;
    Produit Item;
    int NumeroSerieProduit;
    cout << "Consommateur " << myId << ": debute la consommation des produits!" << endl;
    while (true) {
        // Le consommateur tente de retirer un objet
        ret = MaFile.Retire(Item);
        if (ret == FILE_TERMINEE) {
            // La file est vide, et la methode FlushConsommateurs() a ete appelee auparavent.
            // Le thread doit donc quitter!
            cout << "Consommateur " << myId << ": quitte car il ne reste plus de produits a consommer." << endl;
            pthread_exit(NULL);
        }

        // Un produit valide a ete retire de la file. La consommation consiste simplement a l'afficher
        // a l'ecran.
        NumeroSerieProduit = Item.GetNumProduit();
        cout << "Consommateur " << myId << ": consomme le produit: (" << Item.GetNomProduit() << ", "
             << Item.GetPrixProduit() << "$) avec le numero de serie " << NumeroSerieProduit << endl;

        // Le consommateur va dormir un temps aleatoire entre 0 et 50 ms. Ne modifiez rien!
        // Le timing a ete concu pour parfois ralentir les producteurs, parfois ralentir les
        // consommateurs. Ainsi, la file sera parfois vide, parfois pleine.
        float phase = fabs(cos(0.1 * (float) NumeroSerieProduit));
        float SleepTime = 5.0 * phase * (float(rand() % 10000));
        usleep(long(SleepTime));
    }
}
