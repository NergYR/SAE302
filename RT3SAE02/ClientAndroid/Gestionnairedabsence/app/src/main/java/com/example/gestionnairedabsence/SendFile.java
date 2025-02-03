package com.example.gestionnairedabsence;

import java.util.List;

public class SendFile {

    public void sendFile(List<String[]> lignes, List<Etudiant> etudiants) {
        // Parcourir chaque ligne existante
        for (String[] ligne : lignes) {
            // Chaque ligne contient des données, par exemple : ligne[0] = nom, ligne[1] = prénom
            String nom = ligne[0];
            String prenom = ligne[1];

            // Comparer avec les étudiants pour mettre à jour la colonne "présence"
            for (Etudiant etudiant : etudiants) {
                if (etudiant.getNom().equals(nom) && etudiant.getPrenom().equals(prenom)) {
                    // Ajouter ou mettre à jour la présence dans la ligne
                    ligne[2] = String.valueOf(etudiant.getPresence());
                }
            }
        }

        // Simuler l'envoi en affichant les données mises à jour
        System.out.println("Contenu des lignes mises à jour :");
        for (String[] ligne : lignes) {
            System.out.println(String.join(";", ligne));
        }
    }
}
