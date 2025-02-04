package com.example.gestionnairedabsence;

import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.List;

public class SendFile {

    public void sendFile(String serverIp, int serverPort, List<Etudiant> etudiants) {
        new Thread(() -> {
            try (Socket socket = new Socket(serverIp, serverPort);
                 OutputStream os = socket.getOutputStream();
                 PrintWriter writer = new PrintWriter(os, true)) {

                // Construire les données à envoyer
                StringBuilder data = new StringBuilder();
                System.out.println("=== Liste des étudiants envoyés au serveur ===");
                for (Etudiant etudiant : etudiants) {
                    if ("nom".equalsIgnoreCase(etudiant.getNom()) && "prenom".equalsIgnoreCase(etudiant.getPrenom())) {
                        continue; // Ignore la première ligne
                    }

                    // Convertir 0 en "Absent" et 1 en "Présent"
                    String statutPresence = etudiant.getPresence() == 1 ? "Présent" : "Absent";

                    // Ajouter aux données à envoyer
                    data.append(etudiant.getNom()).append(";")
                            .append(etudiant.getPrenom()).append(";")
                            .append(statutPresence).append("\n");

                    // Log pour chaque étudiant
                    System.out.println("Étudiant : " + etudiant.getNom() + " " + etudiant.getPrenom() + " - " + statutPresence);
                }

                // Ajouter le marqueur de fin
                data.append("END_OF_FILE\n");

                // Découper les données en segments compatibles avec le buffer du serveur
                int bufferSize = 1024; // Taille approximative du buffer serveur
                String csvString = data.toString();
                int length = csvString.length();
                int offset = 0;

                while (offset < length) {
                    // Calculer la fin du segment
                    int end = Math.min(offset + bufferSize, length);

                    // Envoyer le segment
                    writer.print(csvString.substring(offset, end));
                    writer.flush();

                    // Mettre à jour l'offset
                    offset = end;
                }

                System.out.println("=== Fin de la liste ===");
                System.out.println("Données envoyées au serveur avec succès.");

            } catch (Exception e) {
                e.printStackTrace();
                System.err.println("Erreur lors de l'envoi des données : " + e.getMessage());
            }
        }).start();
    }
}
