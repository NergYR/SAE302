.PHONY: all clean install compile run compile-android run-android

# Sous-répertoire de l'application Android
DIR_ANDROID = Gestionnairedabsence

# Nom de l'APK généré par Gradle
APK_PATH = $(DIR_ANDROID)/app/build/outputs/apk/debug/app-debug.apk

# Nom du package de votre application (à adapter selon votre projet)
APP_PACKAGE = com.example.gestionnairedabsence

# Nom de l'émulateur Android (à adapter selon votre configuration)
AVD_NAME = Nexus_5_API_30

# Cible principale
all: compile install run-android

# Nettoyage du projet
clean:
	cd $(DIR_ANDROID) && ./gradlew clean
	rm -rf $(DIR_ANDROID)/app/build

# Compilation de tout le projet
compile: compile-android

# Compilation de l'application Android
compile-android:
	cd $(DIR_ANDROID) && ./gradlew assembleDebug

# Installation de l'APK sur un appareil ou émulateur
install: compile-android
	# Vérifie si un appareil ou un émulateur est connecté
	@if ! adb get-state 1>/dev/null 2>&1; then \
		echo "Aucun appareil connecté. Veuillez démarrer un émulateur ou connecter un appareil."; \
		exit 1; \
	fi
	# Installe l'APK
	adb install -r $(APK_PATH)

# Exécution de l'application sur l'émulateur ou l'appareil
run-android:
	# Vérifie si l'émulateur est en cours d'exécution
	@if ! adb get-state 1>/dev/null 2>&1; then \
		echo "Démarrage de l'émulateur $(AVD_NAME)..."; \
		emulator -avd $(AVD_NAME) -netdelay none -netspeed full -no-snapshot-load & \
		sleep 20; \
	fi
	# Attendre que l'émulateur soit prêt
	adb wait-for-device
	# Lancer l'application installée
	adb shell monkey -p $(APP_PACKAGE) 1
