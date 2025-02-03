package com.example.gestionnairedabsence;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

public class ListeEtudiantsActivity extends Activity {
    private static final String TAG = "ListeEtudiantsActivity";
    private List<Etudiant> listeEtudiants;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_liste_etudiants);

        ListView listViewEtudiants = findViewById(R.id.listViewEtudiants);
        Button buttonValider = findViewById(R.id.buttonValider);

        // ✅ Récupération des étudiants envoyés depuis ChoirPromoActivity
        if (getIntent().hasExtra("listeEtudiants")) {
            listeEtudiants = (ArrayList<Etudiant>) getIntent().getSerializableExtra("listeEtudiants");
        } else {
            listeEtudiants = new ArrayList<>();
            Log.e(TAG, "Aucune donnée reçue via Intent.");
            Toast.makeText(this, "Aucune donnée reçue", Toast.LENGTH_SHORT).show();
        }

        if (listeEtudiants.isEmpty()) {
            Toast.makeText(this, "Liste vide. Vérifiez la connexion au serveur.", Toast.LENGTH_LONG).show();
            Log.w(TAG, "Liste des étudiants vide.");
        } else {
            Log.d(TAG, "Nombre d'étudiants reçus : " + listeEtudiants.size());
        }

        // ✅ Utilisation d'EtudiantAdapter pour afficher les étudiants
        EtudiantAdapter adapter = new EtudiantAdapter(this, listeEtudiants);
        listViewEtudiants.setAdapter(adapter);

        // ✅ Gestion du bouton de validation
        buttonValider.setOnClickListener(v -> validerPresence());
    }

    private void validerPresence() {
        if (listeEtudiants.isEmpty()) {
            Toast.makeText(this, "Aucun étudiant à valider.", Toast.LENGTH_SHORT).show();
            return;
        }

        StringBuilder result = new StringBuilder("Présents :\n");
        for (Etudiant etudiant : listeEtudiants) {
            result.append(etudiant.getNom()).append(" ").append(etudiant.getPrenom()).append("\n");
        }

        Log.d(TAG, "Liste des présents validée.");
        Toast.makeText(this, result.toString(), Toast.LENGTH_LONG).show();
    }
}
