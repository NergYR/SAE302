package com.example.gestionnairedabsence;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;

import java.net.Socket;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.ArrayList;
import android.widget.Button;

/**
 * Activité permettant à l'utilisateur de sélectionner une promotion.
 */
public class ChoirPromoActivity extends AppCompatActivity {
    private static final String TAG = "ChoirPromoActivity";
    private String selectedPromo = null;
    private ArrayList<String> listePromos;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_choix_promo);

        // Récupération des éléments graphiques de l'interface utilisateur
        TextView titre = findViewById(R.id.titrePromo);
        ListView listViewPromos = findViewById(R.id.listViewPromos);
        Button buttonRetour = findViewById(R.id.buttonRetour);

        titre.setText("Listes Promo");

        // Récupération des données passées par l'intent
        Intent intent = getIntent();
        final ArrayList<String> fileList = intent.getStringArrayListExtra("fileList");
        final String serverIp = intent.getStringExtra("serverIp");
        final int serverPort = intent.getIntExtra("serverPort", -1);

        // Vérification des données reçues
        if (fileList == null || fileList.isEmpty()) {
            Log.e(TAG, "Aucun fichier CSV reçu depuis l'Intent.");
            Toast.makeText(this, "Aucun fichier reçu. Retour à la page principale.", Toast.LENGTH_SHORT).show();
            navigateToMainActivity();
            return;
        }

        if (serverIp == null || serverPort <= 0) {
            Log.e(TAG, "Adresse IP ou port du serveur non valide.");
            Toast.makeText(this, "Erreur : IP ou port du serveur incorrect. Retour à la page principale.", Toast.LENGTH_SHORT).show();
            navigateToMainActivity();
            return;
        }

        // Création de la liste des promotions disponibles
        listePromos = new ArrayList<>();
        for (String fileName : fileList) {
            if (fileName.endsWith(".csv")) {
                listePromos.add(fileName.replace(".csv", "").toUpperCase());
            }
        }

        // Adaptateur pour afficher la liste des promotions dans un ListView
        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, listePromos);
        listViewPromos.setAdapter(adapter);

        // Gestion de la sélection d'une promotion dans la liste
        listViewPromos.setOnItemClickListener((parent, view, position, id) -> {
            selectedPromo = listePromos.get(position);
            Log.d(TAG, "Promotion sélectionnée : " + selectedPromo);
            fetchCSVContent(selectedPromo + ".csv", serverIp, serverPort);
        });

        // Gestion du bouton retour vers la page principale
        buttonRetour.setOnClickListener(v -> {
            Log.d(TAG, "Bouton Retour cliqué, retour à MainActivity.");
            navigateToMainActivity();
        });
    }

    /**
     * Fonction permettant de récupérer le contenu du fichier CSV de la promotion sélectionnée
     * depuis un serveur distant via une connexion socket.
     */
    private void fetchCSVContent(String fileName, String serverIp, int serverPort) {
        new Thread(() -> {
            try {
                // Connexion au serveur via un socket
                Socket socket = new Socket(serverIp, serverPort);
                PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                BufferedReader input = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                Log.d(TAG, "Connexion établie avec le serveur " + serverIp + ":" + serverPort);
                out.println(fileName); // Envoi du nom du fichier au serveur
                Log.d(TAG, "Requête envoyée : " + fileName);

                ArrayList<Etudiant> etudiants = new ArrayList<>();
                String line;
                int emptyLineCount = 0; // Compteur de lignes vides pour détecter la fin des données

                // Lecture des lignes de la réponse du serveur
                while ((line = input.readLine()) != null) {
                    Log.d(TAG, "Réponse du serveur : " + line);

                    String[] parts = line.split(";");
                    if (parts.length >= 7) { // Vérification du format du fichier CSV
                        etudiants.add(new Etudiant(parts[4], parts[6])); // Extraction des noms et prénoms
                    }

                    if (line.trim().isEmpty()) { // Gestion des lignes vides
                        emptyLineCount++;
                        if (emptyLineCount >= 2) { // Arrêt après deux lignes vides
                            Log.d(TAG, "Deux lignes vides détectées, fin de transmission.");
                            break;
                        }
                        continue;
                    }
                }

                // Lancement de l'activité ListeEtudiantsActivity avec les données récupérées
                new Handler(Looper.getMainLooper()).post(() -> {
                    Intent nextIntent = new Intent(ChoirPromoActivity.this, ListeEtudiantsActivity.class);
                    nextIntent.putExtra("promoSelectionnee", selectedPromo);
                    nextIntent.putExtra("listeEtudiants", etudiants);
                    nextIntent.putExtra("serverIp", serverIp);
                    nextIntent.putExtra("serverPort", serverPort);
                    startActivity(nextIntent);
                });
            } catch (Exception e) {
                Log.e(TAG, "Erreur lors de la récupération du fichier CSV : ", e);
                runOnUiThread(() ->
                        Toast.makeText(this, "Impossible de récupérer le fichier CSV.", Toast.LENGTH_SHORT).show()
                );
            }
        }).start();
    }

    /**
     * Fonction permettant de retourner à l'écran principal.
     */
    private void navigateToMainActivity() {
        Intent mainIntent = new Intent(ChoirPromoActivity.this, MainActivity.class);
        startActivity(mainIntent);
        finish(); // Termine l'activité actuelle
    }
}
