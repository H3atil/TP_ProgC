#include <stdio.h>

int main(void)
{
    const char *noms[5] = {"Dupont", "Martin", "Bernard", "Petit", "Robert"};
    const char *prenoms[5] = {"Marie", "Pierre", "Sophie", "Lucas", "Emma"};
    const char *adresses[5] = {
        "20, Boulevard Niels Bohr, Lyon",
        "22, Boulevard Niels Bohr, Lyon",
        "5, rue des Ecoles, Lyon",
        "8, avenue de la Republique, Lyon",
        "14, rue Victor Hugo, Lyon"
    };
    float notes_c[5] = {16.5f, 14.0f, 15.2f, 12.8f, 17.0f};
    float notes_systeme[5] = {12.1f, 14.1f, 13.5f, 16.0f, 15.8f};

    for (int i = 0; i < 5; i++)
    {
        printf("Etudiant.e %d :\n", i + 1);
        printf("Nom : %s\n", noms[i]);
        printf("Prenom : %s\n", prenoms[i]);
        printf("Adresse : %s\n", adresses[i]);
        printf("Note programmation C : %.1f\n", notes_c[i]);
        printf("Note systeme d'exploitation : %.1f\n\n", notes_systeme[i]);
    }

    return 0;
}