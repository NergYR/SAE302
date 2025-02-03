package com.example.gestionnairedabsence;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class ListeEtudiantsActivity extends Activity {
    private List<Etudiant> listeEtudiants;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_liste_etudiants);

        ListView listViewEtudiants = findViewById(R.id.listViewEtudiants);
        Button buttonValider = findViewById(R.id.buttonValider);

        // ✅ Récupération des étudiants envoyés depuis ChoirPromoActivity
        listeEtudiants = (ArrayList<Etudiant>) getIntent().getSerializableExtra("listeEtudiants");

        if (listeEtudiants == null) {
            listeEtudiants = new ArrayList<>();
            Toast.makeText(this, "Aucune donnée reçue", Toast.LENGTH_SHORT).show();
        }

        // ✅ Utilisation d'EtudiantAdapter pour afficher les étudiants
        EtudiantAdapter adapter = new EtudiantAdapter(this, listeEtudiants);
        listViewEtudiants.setAdapter(adapter);

        // ✅ Gestion du bouton de validation
        buttonValider.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                validerPresence();
            }
        });
    }

    private void validerPresence() {
        StringBuilder result = new StringBuilder("Présents :\n");
        for (Etudiant etudiant : listeEtudiants) {
            result.append(etudiant.getNom()).append(" ").append(etudiant.getPrenom()).append("\n");
        }
        Toast.makeText(this, result.toString(), Toast.LENGTH_LONG).show();
    }
}
