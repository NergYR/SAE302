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
import java.io.Serializable;
import androidx.appcompat.app.AppCompatActivity;

import java.net.Socket;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.ArrayList;

public class ChoirPromoActivity extends AppCompatActivity {
    private static final String TAG = "ChoirPromoActivity";
    private String selectedPromo = null;
    private ArrayList<String> listePromos;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_choix_promo);

        TextView titre = findViewById(R.id.titrePromo);
        ListView listViewPromos = findViewById(R.id.listViewPromos);

        titre.setText("Listes Promo");

        Intent intent = getIntent();
        final ArrayList<String> fileList = intent.getStringArrayListExtra("fileList");
        final String serverIp = intent.getStringExtra("serverIp");
        final int serverPort = intent.getIntExtra("serverPort", -1);

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

        listePromos = new ArrayList<>();
        for (String fileName : fileList) {
            if (fileName.endsWith(".csv")) {
                listePromos.add(fileName.replace(".csv", "").toUpperCase());
            }
        }

        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, listePromos);
        listViewPromos.setAdapter(adapter);

        listViewPromos.setOnItemClickListener((parent, view, position, id) -> {
            selectedPromo = listePromos.get(position);
            Log.d(TAG, "Promotion sélectionnée : " + selectedPromo);
            fetchCSVContent(selectedPromo + ".csv", serverIp, serverPort);
        });
    }

    private void fetchCSVContent(String fileName, String serverIp, int serverPort) {
        new Thread(() -> {
            try {
                Socket socket = new Socket(serverIp, serverPort);
                PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                BufferedReader input = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                Log.d(TAG, "Connexion établie avec le serveur " + serverIp + ":" + serverPort);
                out.println(fileName);
                Log.d(TAG, "Requête envoyée : " + fileName);

                ArrayList<Etudiant> etudiants = new ArrayList<>();
                String line;
                boolean isHeader = true;

                while ((line = input.readLine()) != null) {
                    Log.d(TAG, "Réponse du serveur : " + line);

                    if (isHeader) {
                        isHeader = false;
                        continue;
                    }

                    String[] parts = line.split(";");
                    if (parts.length >= 7) { // Assurez-vous d'avoir assez de colonnes
                        etudiants.add(new Etudiant(parts[4], parts[6])); // Nom, prénom
                    }
                }

                //  Lancement immédiat de ListeEtudiantsActivity

                new Handler(Looper.getMainLooper()).post(() -> {
                    Intent nextIntent = new Intent(ChoirPromoActivity.this, ListeEtudiantsActivity.class);
                    nextIntent.putExtra("promoSelectionnee", selectedPromo);
                    nextIntent.putExtra("listeEtudiants", etudiants);
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

    private void navigateToMainActivity() {
        Intent mainIntent = new Intent(ChoirPromoActivity.this, MainActivity.class);
        startActivity(mainIntent);
        finish();
    }
}
