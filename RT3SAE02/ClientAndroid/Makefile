# Définition des variables
APP_NAME := Gestionnairedabsence
GRADLEW := ./gradlew
ADB := adb
PACKAGE_NAME := com.example.gestionnairedabsence
DEVICE := $(shell $(ADB) devices | awk 'NR==2 {print $$1}') # Récupère l'ID du premier appareil connecté

# Commande pour compiler l'application
.PHONY: build
build:
	$(GRADLEW) assembleDebug

# Commande pour nettoyer le projet
.PHONY: clean
clean:
	$(GRADLEW) clean

# Commande pour générer un APK prêt à être installé
.PHONY: apk
apk:
	$(GRADLEW) assembleRelease

# Commande pour exécuter l'application sur un appareil connecté
.PHONY: install
install: build
	$(ADB) install -r app/build/outputs/apk/debug/app-debug.apk

# Commande pour désinstaller l'application de l'appareil
.PHONY: uninstall
uninstall:
	$(ADB) uninstall $(PACKAGE_NAME)

# Commande pour afficher les logs de l'application
.PHONY: logcat
logcat:
	$(ADB) logcat -s $(PACKAGE_NAME):V

# Commande pour exécuter l'application après l'installation
.PHONY: run
run: install
	$(ADB) shell monkey -p $(PACKAGE_NAME) -c android.intent.category.LAUNCHER 1

# Commande pour démarrer un émulateur spécifique (remplace ton_avd_name par ton AVD)
.PHONY: emulator
emulator:
	$(ANDROID_HOME)/emulator/emulator -avd ton_avd_name

# Commande pour recompiler et exécuter directement
.PHONY: reinstall
reinstall: clean build install run
