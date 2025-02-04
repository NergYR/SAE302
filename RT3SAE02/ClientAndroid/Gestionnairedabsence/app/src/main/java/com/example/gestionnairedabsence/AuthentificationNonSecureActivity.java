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

/**
 * Activité permettant l'authentification non sécurisée d'un utilisateur.
 */
public class AuthentificationNonSecureActivity extends AppCompatActivity {
    private EditText serverIpInput, serverPortInput;
    private TextView messageTextView;
    private Button submitButton, retourBouton;
    private final ArrayList<String> fileList = new ArrayList<>();
    private static final String TAG = "AuthNonSecureActivity";
    private static final int TIMEOUT_MS = 5000; // Délai d'attente de 5 secondes

    private final Handler uiHandler = new Handler(Looper.getMainLooper());

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.page_authentification_non_secure);

        initializeViews(); // Initialisation des éléments graphiques
        setupListeners(); // Mise en place des écouteurs d'événements
    }

    /**
     * Initialise les éléments de l'interface utilisateur.
     */
    private void initializeViews() {
        serverIpInput = findViewById(R.id.server_ip_input);
        serverPortInput = findViewById(R.id.server_port_input);
        messageTextView = findViewById(R.id.message_pour_authentification);
        submitButton = findViewById(R.id.submit_button_authentification);
        retourBouton = findViewById(R.id.bouton_retour_accueil);
    }

    /**
     * Configure les écouteurs d'événements pour les boutons.
     */
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

                submitButton.setEnabled(false);
                messageTextView.setText("Connexion en cours...");
                new Thread(() -> connectToServer(serverIp, serverPort)).start();
            }
        });
    }

    /**
     * Vérifie si les entrées utilisateur sont valides.
     * @return true si valides, false sinon
     */
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

    /**
     * Tente de se connecter à un serveur via une socket TCP.
     * @param serverIp Adresse IP du serveur
     * @param serverPort Port du serveur
     */
    @SuppressLint("SetTextI18n")
    private void connectToServer(String serverIp, int serverPort) {
        Socket socket = null;
        try {
            Log.d(TAG, "Tentative de connexion au serveur : " + serverIp + ":" + serverPort);
            socket = new Socket();
            socket.connect(new java.net.InetSocketAddress(serverIp, serverPort), TIMEOUT_MS);
            socket.setSoTimeout(TIMEOUT_MS);

            Log.d(TAG, "Connexion établie avec succès");
            BufferedReader input = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            String line;
            fileList.clear();

            while ((line = input.readLine()) != null) {
                Log.d(TAG, "Ligne reçue : " + line);
                if (line.isEmpty()) break;
                fileList.add(line.trim());
            }

            uiHandler.post(() -> {
                if (!fileList.isEmpty()) {
                    Intent intent = new Intent(AuthentificationNonSecureActivity.this, ChoirPromoActivity.class);
                    intent.putStringArrayListExtra("fileList", fileList);
                    intent.putExtra("serverIp", serverIp);
                    intent.putExtra("serverPort", serverPort);
                    startActivity(intent);
                    finish();
                } else {
                    messageTextView.setText("Aucune donnée reçue du serveur.");
                    showToast("Aucun fichier disponible sur le serveur.");
                    submitButton.setEnabled(true);
                }
            });

        } catch (SocketTimeoutException e) {
            Log.e(TAG, "Timeout de la connexion", e);
            uiHandler.post(() -> {
                messageTextView.setText("Erreur : Délai de connexion dépassé (5s)");
                showToast("La connexion a pris trop de temps. Veuillez réessayer.");
                submitButton.setEnabled(true);
            });
        } catch (Exception e) {
            Log.e(TAG, "Erreur de connexion", e);
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

    /**
     * Affiche un message Toast sur l'interface utilisateur.
     * @param message Message à afficher
     */
    private void showToast(String message) {
        uiHandler.post(() -> Toast.makeText(this, message, Toast.LENGTH_SHORT).show());
    }
}
