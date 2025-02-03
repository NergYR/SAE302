package com.example.gestionnairedabsence;

import android.os.Bundle;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Récupération des boutons
        Button buttonSecure = findViewById(R.id.bouton_connexion_secure);
        Button buttonNonSecure = findViewById(R.id.bouton_connexion_non_secure);

        // Désactiver le bouton
        buttonSecure.setEnabled(false);
        buttonSecure.setAlpha(0.5f); // Facultatif : changer l'apparence pour indiquer qu'il est inactif

        // Redirection vers l'authentification sécurisée (le clic ne fonctionne plus car le bouton est désactivé)
        buttonSecure.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, AuthentificationSecureActivity.class);
                startActivity(intent);
            }
        });

        // Redirection vers l'authentification non sécurisée
        buttonNonSecure.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, AuthentificationNonSecureActivity.class);
                startActivity(intent);
            }
        });
    }
}
