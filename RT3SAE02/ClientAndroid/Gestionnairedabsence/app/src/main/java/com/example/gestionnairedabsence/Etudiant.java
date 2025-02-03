package com.example.gestionnairedabsence;

import java.io.Serializable;

public class Etudiant implements Serializable {
    private String nom;
    private String prenom;

    public Etudiant(String nom, String prenom) {
        this.nom = nom;
        this.prenom = prenom;
    }

    public String getNom() {
        return nom;
    }

    public String getPrenom() {
        return prenom;
    }
}
