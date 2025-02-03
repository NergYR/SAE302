package com.example.gestionnairedabsence;

import java.io.Serializable;

public class Etudiant implements Serializable { // ✅ Doit être Serializable pour passer via Intent
    private String nom;
    private String prenom;
    private boolean present;

    public Etudiant(String nom, String prenom) {
        this.nom = nom;
        this.prenom = prenom;
        this.present = false; // Par défaut, pas présent
    }

    public String getNom() {
        return nom;
    }

    public String getPrenom() {
        return prenom;
    }

    public boolean isPresent() {
        return present;
    }

    public void setPresent(boolean present) {
        this.present = present;
    }
}
