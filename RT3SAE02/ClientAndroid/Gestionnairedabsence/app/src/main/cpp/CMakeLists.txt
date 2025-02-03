# Définir la version minimale requise de CMake
cmake_minimum_required(VERSION 3.22.1)

# Déclarer le nom du projet
project("gestionnairedabsence")

# Ajouter les fichiers sources du projet
add_library(${CMAKE_PROJECT_NAME} SHARED
        Client.c
        client_back.c
        unSecured_server.c)

# Lier les bibliothèques Android et log
target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log)
