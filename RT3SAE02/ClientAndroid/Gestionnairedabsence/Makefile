# Définition des variables
GRADLEW := ./gradlew
ADB := adb
APP_MODULE := app
BUILD_DIR := $(APP_MODULE)/build/outputs/apk
PACKAGE_NAME := com.example.gestionnairedabsence
DEVICE := $(shell $(ADB) devices | awk 'NR==2 {print $$1}') # Récupère l'ID du premier appareil connecté

# Compilation de l'application en mode Debug
.PHONY: build
build:
	$(GRADLEW) assembleDebug

# Nettoyage du projet
.PHONY: clean
clean:
	$(GRADLEW) clean

# Génération d'un APK Release
.PHONY: apk
apk:
	$(GRADLEW) assembleRelease

# Installation de l'application sur un appareil connecté
.PHONY: install
install: build
	$(ADB) install -r $(BUILD_DIR)/debug/app-debug.apk

# Désinstallation de l'application
.PHONY: uninstall
uninstall:
	$(ADB) uninstall $(PACKAGE_NAME)

# Lancement de l'application après installation
.PHONY: run
run: install
	$(ADB) shell monkey -p $(PACKAGE_NAME) -c android.intent.category.LAUNCHER 1

# Affichage des logs de l'application
.PHONY: logcat
logcat:
	$(ADB) logcat -s $(PACKAGE_NAME):V

# Démarrage d'un émulateur Android (modifier avec le nom de ton AVD)
.PHONY: emulator
emulator:
	$(ANDROID_HOME)/emulator/emulator -avd ton_avd_name &

# Nettoyer, recompiler, réinstaller et exécuter l'application
.PHONY: reinstall
reinstall: clean build install run
