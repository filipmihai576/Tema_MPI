#!/bin/bash

# Navighează la directorul temei
cd ~/Desktop/tema_mpi

# Rulează scriptul Python
python3 resize.py

# Verifică dacă rularea scriptului Python a avut succes
if [ $? -eq 0 ]; then
    # Compilează codul C++
    g++ MPI_tema.cpp -o MPI_tema -std=c++11

    # Verifică dacă compilarea a avut succes
    if [ $? -eq 0 ]; then
        # Rulează programul C++
        ./MPI_tema
    else
        # Afișează un mesaj de eroare dacă compilarea a eșuat
        echo "Eroare la compilare!"
    fi
else
    # Afișează un mesaj de eroare dacă rularea scriptului Python a eșuat
    echo "Eroare la rularea scriptului Python!"
fi


# Așteaptă o apăsare de tastă înainte de a închide fereastra (opțional)
read -n 1 -s -r -p "Apasă orice tastă pentru a închide..."