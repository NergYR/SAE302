package com.example.gestionnairedabsence;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;
import java.util.ArrayList;
import java.util.List;

/**
 * Activité permettant d'afficher la liste des étudiants et de valider leur présence.
 */
public class ListeEtudiantsActivity extends Activity {
    private static final String TAG = "ListeEtudiantsActivity";
    private List<Etudiant> listeEtudiants;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_liste_etudiants);

        // Récupération des éléments de l'interface utilisateur
        ListView listViewEtudiants = findViewById(R.id.listViewEtudiants);
        Button buttonValider = findViewById(R.id.buttonValider);

        // Récupération de la liste des étudiants envoyée depuis ChoirPromoActivity
        if (getIntent().hasExtra("listeEtudiants")) {
            listeEtudiants = (ArrayList<Etudiant>) getIntent().getSerializableExtra("listeEtudiants");
        } else {
            listeEtudiants = new ArrayList<>();
            Log.e(TAG, "Aucune donnée reçue via Intent.");
            Toast.makeText(this, "Aucune donnée reçue", Toast.LENGTH_SHORT).show();
        }

        // Vérification si la liste des étudiants est vide
        if (listeEtudiants.isEmpty()) {
            Toast.makeText(this, "Liste vide. Vérifiez la connexion au serveur.", Toast.LENGTH_LONG).show();
            Log.w(TAG, "Liste des étudiants vide.");
        } else {
            Log.d(TAG, "Nombre d'étudiants reçus : " + listeEtudiants.size());
        }

        // Utilisation d'EtudiantAdapter pour afficher la liste des étudiants
        EtudiantAdapter adapter = new EtudiantAdapter(this, listeEtudiants);
        listViewEtudiants.setAdapter(adapter);

        // Gestion du bouton de validation de présence
        buttonValider.setOnClickListener(v -> validerPresence());
    }

    /**
     * Valide la présence des étudiants et envoie les données au serveur.
     */
    private void validerPresence() {
        if (listeEtudiants.isEmpty()) {
            Toast.makeText(this, "Aucun étudiant dans la liste.", Toast.LENGTH_SHORT).show();
            return;
        }

        // Récupération des informations du serveur depuis l'Intent
        String serverIp = getIntent().getStringExtra("serverIp");
        int serverPort = getIntent().getIntExtra("serverPort", -1);

        Log.d(TAG, "serverIp reçu : " + serverIp);
        Log.d(TAG, "serverPort reçu : " + serverPort);

        if (serverIp == null || serverPort == -1) {
            Toast.makeText(this, "Adresse ou port du serveur non configurés.", Toast.LENGTH_LONG).show();
            Log.e(TAG, "Erreur : serveurIp ou serveurPort non reçus !");
            return;
        }

        // Envoi des données de présence des étudiants au serveur
        SendFile sendFile = new SendFile();
        sendFile.sendFile(serverIp, serverPort, listeEtudiants);

        Toast.makeText(this, "Liste des présences envoyée au serveur.", Toast.LENGTH_LONG).show();

        // Retour automatique à l'activité précédente après 2 secondes
        new android.os.Handler().postDelayed(() -> {
            finish(); // Termine l’activité actuelle
        }, 2000);
    }
}
