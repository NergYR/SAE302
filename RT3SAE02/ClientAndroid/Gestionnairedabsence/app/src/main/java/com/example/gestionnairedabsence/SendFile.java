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
                for (Etudiant etudiant : etudiants) {
                    if ("nom".equalsIgnoreCase(etudiant.getNom()) && "prenom".equalsIgnoreCase(etudiant.getPrenom())) {
                        continue; // Ignore cette ligne et passe au prochain étudiant
                    }

                    // Convertir 0 en "Absent" et 1 en "Présent"
                    String statutPresence = etudiant.getPresence() == 1 ? "Présent" : "Absent";

                    data.append(etudiant.getNom()).append(";")
                            .append(etudiant.getPrenom()).append(";")
                            .append(statutPresence).append("\n");
                }



                // Envoyer les données au serveur
                writer.println(data.toString());
                System.out.println("Données envoyées au serveur :");
                System.out.println(data.toString());

            } catch (Exception e) {
                e.printStackTrace();
                System.err.println("Erreur lors de l'envoi des données : " + e.getMessage());
            }
        }).start();
    }
}
