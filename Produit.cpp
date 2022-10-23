#include "Produit.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <vector>

using namespace std;

Produit::Produit() {
}

int Produit::GetNumProduit(void) {
    return NumProduit;
}

void Produit::SetNumProduit(int np) {
    NumProduit = np;
}

string Produit::GetNomProduit(void) {
    return NomProduit;
}

float Produit::GetPrixProduit(void) {
    return PrixProduit;
}

void Produit::PigerProduit(void) {
    const int INDEX_NUMERO_PRODUIT = 3;
    const int INDEX_PRIX = 10;
    const char DELIMITER = ';';

    fstream input("./Produits.dat");
    if (!input) {
        cout << "Erreur lors de l'ouverture du fichier." << endl;
        exit(1);
    }

    vector<string> produits;
    string ligne;
    getline(input, ligne); // On skip la récupération des entêtes des colonnes
    while (getline(input, ligne)) {
        produits.push_back(ligne);
    }

    // On génère un nombre aléatoire pas plus grand que la taille du vecteur de produits
    int index = rand() % produits.size();
    // On va chercher le produit aléatoire dans le vecteur
    string produit_str = produits.at(index);

    // On convertit le produit `produit_str` en stream afin de pouvoir split la chaine de caractères
    // par un delemiteur
    istringstream produit_stream(produit_str);
    vector<string> tokens;
    for (string token; getline(produit_stream, token, DELIMITER);) {
        tokens.push_back(token);
    }

    NomProduit = tokens.at(INDEX_NUMERO_PRODUIT);
    PrixProduit = 0;
    if (tokens.at(INDEX_PRIX).length() != 0) { // Si le prix n'est pas vide dans la ligne
        // On convertit la string en float pour associé au prix du produit
        stringstream ss(tokens.at(INDEX_PRIX));
        float prix;
        ss >> prix;
        PrixProduit = prix;
    }
}
