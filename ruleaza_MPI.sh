#!/bin/bash

cd ~/Desktop/TEMA_MPI

python3 resize.py

# Verifică dacă OpenMPI este instalat
if ! command -v mpic++ &> /dev/null; then
    echo "OpenMPI nu este instalat. Se instalează..."
    brew install open-mpi
fi

# Compilează programul MPI
mpic++ Rezolvare_MPI.cpp -o mpi_image_processor -std=c++11

if [ $? -eq 0 ]; then
    echo "Compilare reușită! Se rulează programul..."
    # Rulează programul cu 2 procese
    mpirun -np 2 ./mpi_image_processor
else
    echo "Eroare la compilare!"
fi

read -n 1 -s -r -p "Apasă orice tastă pentru a închide..."