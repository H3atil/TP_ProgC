#include <stdio.h>
#include <string.h>

struct Etudiant
{
    char nom[32];
    char prenom[32];
    char adresse[96];
    float note_c;
    float note_systeme;
};

int main(void)
{
    struct Etudiant etudiants[5];
    const char *noms[5] = {"Dupont", "Martin", "Bernard", "Petit", "Robert"};
    const char *prenoms[5] = {"Marie", "Pierre", "Sophie", "Lucas", "Emma"};
    const char *adresses[5] = {
        "20, Boulevard Niels Bohr, Lyon",
        "22, Boulevard Niels Bohr, Lyon",
        "5, rue des Ecoles, Lyon",
        "8, avenue de la Republique, Lyon",
        "14, rue Victor Hugo, Lyon"
    };
    const float notes_c[5] = {16.5f, 14.0f, 15.2f, 12.8f, 17.0f};
    const float notes_systeme[5] = {12.1f, 14.1f, 13.5f, 16.0f, 15.8f};

    for (int i = 0; i < 5; i++)
    {
        strcpy(etudiants[i].nom, noms[i]);
        strcpy(etudiants[i].prenom, prenoms[i]);
        strcpy(etudiants[i].adresse, adresses[i]);
        etudiants[i].note_c = notes_c[i];
        etudiants[i].note_systeme = notes_systeme[i];
    }

    for (int i = 0; i < 5; i++)
    {
        printf("Etudiant.e %d :\n", i + 1);
        printf("Nom : %s\n", etudiants[i].nom);
        printf("Prenom : %s\n", etudiants[i].prenom);
        printf("Adresse : %s\n", etudiants[i].adresse);
        printf("Note 1 : %.1f\n", etudiants[i].note_c);
        printf("Note 2 : %.1f\n\n", etudiants[i].note_systeme);
    }

    return 0;
}