package com.example.gestionnairedabsence;

import java.io.Serializable;

/**
 * Classe représentant un étudiant dans le cadre de la gestion des absences.
 */
public class Etudiant implements Serializable {
    private final String nom; // Nom de l'étudiant
    private final String prenom; // Prénom de l'étudiant
    private int presence; // Indicateur de présence (0 = absent, 1 = présent)

    /**
     * Constructeur de la classe Etudiant.
     * @param nom Nom de l'étudiant
     * @param prenom Prénom de l'étudiant
     */
    public Etudiant(String nom, String prenom) {
        this.nom = nom;
        this.prenom = prenom;
        this.presence = 0; // Par défaut, l'étudiant est marqué comme absent
    }

    /**
     * Récupère le nom de l'étudiant.
     * @return Le nom de l'étudiant
     */
    public String getNom() {
        return nom;
    }

    /**
     * Récupère le prénom de l'étudiant.
     * @return Le prénom de l'étudiant
     */
    public String getPrenom() {
        return prenom;
    }

    /**
     * Récupère le statut de présence de l'étudiant.
     * @return 0 si absent, 1 si présent
     */
    public int getPresence() {
        return presence;
    }

    /**
     * Définit le statut de présence de l'étudiant.
     * @param presence 0 pour absent, 1 pour présent
     */
    public void setPresence(int presence) {
        this.presence = presence;
    }
}
