package com.example.gestionnairedabsence;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import androidx.appcompat.app.AppCompatActivity;

public class AuthentificationSecureActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.page_authentification_secure);

        Button retourBouton = findViewById(R.id.bouton_retour_accueil);
        retourBouton.setOnClickListener(v -> {
            Intent intent = new Intent(AuthentificationSecureActivity.this, MainActivity.class);
            startActivity(intent);
            finish(); // Ferme l'activité actuelle pour éviter le retour avec le bouton physique
        });

    }
}

