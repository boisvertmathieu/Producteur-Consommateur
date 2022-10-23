#ifndef PRODUIT_H
#define PRODUIT_H

#include <string>

class Produit {
public:
    Produit();

    int GetNumProduit(void);

    void SetNumProduit(int np);

    std::string GetNomProduit(void);

    float GetPrixProduit(void);

    void PigerProduit();

private:
    int NumProduit;         // Le numero de serie du produit
    std::string NomProduit; // Le nom du produit
    float PrixProduit;      // Le prix du produit
};

#endif
