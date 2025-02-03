package com.example.gestionnairedabsence;

import java.io.Serializable;

public class Etudiant implements Serializable {
    private String nom;
    private String prenom;
    private int presence; // 0 = absent, 1 = présent

    public Etudiant(String nom, String prenom) {
        this.nom = nom;
        this.prenom = prenom;
        this.presence = 0; // Par défaut, absent
    }

    public String getNom() {
        return nom;
    }

    public String getPrenom() {
        return prenom;
    }

    public int getPresence() {
        return presence;
    }

    public void setPresence(int presence) {
        this.presence = presence;
    }
}
