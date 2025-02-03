package com.example.gestionnairedabsence;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.ArrayList;

public class AuthentificationNonSecureActivity extends AppCompatActivity {
    private EditText serverIpInput, serverPortInput;
    private TextView messageTextView;
    private Button submitButton, retourBouton;
    private final ArrayList<String> fileList = new ArrayList<>();
    private static final String TAG = "AuthNonSecureActivity";
    private static final int TIMEOUT_MS = 5000; // 5 secondes en millisecondes

    private final Handler uiHandler = new Handler(Looper.getMainLooper());

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.page_authentification_non_secure);

        initializeViews();
        setupListeners();
    }

    private void initializeViews() {
        serverIpInput = findViewById(R.id.server_ip_input);
        serverPortInput = findViewById(R.id.server_port_input);
        messageTextView = findViewById(R.id.message_pour_authentification);
        submitButton = findViewById(R.id.submit_button_authentification);
        retourBouton = findViewById(R.id.bouton_retour_accueil);
    }

    @SuppressLint("SetTextI18n")
    private void setupListeners() {
        retourBouton.setOnClickListener(v -> {
            Intent intent = new Intent(AuthentificationNonSecureActivity.this, MainActivity.class);
            startActivity(intent);
            finish();
        });

        submitButton.setOnClickListener(v -> {
            if (validateInputs()) {
                String serverIp = serverIpInput.getText().toString().trim();
                int serverPort = Integer.parseInt(serverPortInput.getText().toString().trim());

                // Désactiver le bouton pendant la connexion
                submitButton.setEnabled(false);
                messageTextView.setText("Connexion en cours...");

                // Lancer un Thread pour gérer la connexion au serveur
                new Thread(() -> connectToServer(serverIp, serverPort)).start();
            }
        });
    }

    private boolean validateInputs() {
        String serverIp = serverIpInput.getText().toString().trim();
        String portString = serverPortInput.getText().toString().trim();

        if (serverIp.isEmpty() || portString.isEmpty()) {
            showToast("Veuillez entrer une adresse IP et un port.");
            return false;
        }

        try {
            Integer.parseInt(portString);
            return true;
        } catch (NumberFormatException e) {
            showToast("Port invalide.");
            return false;
        }
    }

    @SuppressLint("SetTextI18n")
    private void connectToServer(String serverIp, int serverPort) {
        Socket socket = null;
        try {
            Log.d(TAG, "Tentative de connexion au serveur : " + serverIp + ":" + serverPort);
            socket = new Socket();
            socket.connect(new java.net.InetSocketAddress(serverIp, serverPort), TIMEOUT_MS); // Timeout à 5 secondes pour la connexion
            socket.setSoTimeout(TIMEOUT_MS); // Timeout pour la lecture des données

            Log.d(TAG, "Connexion établie avec succès");
            BufferedReader input = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            Log.d(TAG, "Début de la lecture des données");
            String line;
            fileList.clear();

            // Lecture des données ligne par ligne
            while ((line = input.readLine()) != null) {
                Log.d(TAG, "Ligne reçue : " + line);
                if (line.isEmpty()) {
                    // Si une ligne vide est reçue, on considère que c'est la fin des données
                    break;
                }
                fileList.add(line.trim());
            }

            Log.d(TAG, "Nombre de fichiers reçus : " + fileList.size());

            // Une fois les données reçues, on passe à l'activité suivante
            uiHandler.post(() -> {
                if (!fileList.isEmpty()) {
                    Intent intent = new Intent(AuthentificationNonSecureActivity.this, ChoirPromoActivity.class);
                    intent.putStringArrayListExtra("fileList", fileList);
                    intent.putExtra("serverIp", serverIp); // L'adresse IP du serveur
                    intent.putExtra("serverPort", serverPort); // Le port du serveur
                    startActivity(intent);
                    finish();
                } else {
                    messageTextView.setText("Aucune donnée reçue du serveur.");
                    showToast("Aucun fichier disponible sur le serveur.");
                    submitButton.setEnabled(true);
                }
            });

        } catch (SocketTimeoutException e) {
            Log.e(TAG, "Timeout de la connexion ou de la lecture des données", e);
            uiHandler.post(() -> {
                messageTextView.setText("Erreur : Délai de connexion dépassé (5 secondes)");
                showToast("La connexion ou la lecture des données a pris trop de temps. Veuillez réessayer.");
                submitButton.setEnabled(true);
            });
        } catch (Exception e) {
            Log.e(TAG, "Erreur de connexion : ", e);
            uiHandler.post(() -> {
                messageTextView.setText("Erreur : " + e.getMessage());
                showToast("Erreur de connexion au serveur.");
                submitButton.setEnabled(true);
            });
        } finally {
            if (socket != null && !socket.isClosed()) {
                try {
                    socket.close();
                } catch (Exception e) {
                    Log.e(TAG, "Erreur lors de la fermeture du socket", e);
                }
            }
        }
    }

    private void showToast(String message) {
        uiHandler.post(() -> Toast.makeText(this, message, Toast.LENGTH_SHORT).show());
    }
}
