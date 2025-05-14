#!/bin/bash

cd ~/Desktop/tema_mpi

python3 resize.py

if [ $? -eq 0 ]; then
    g++ MPI_tema_paralel.cpp -o MPI_tema_paralel_exec -std=c++17 -O2 -pthread

    if [ $? -eq 0 ]; then
        ./MPI_tema_paralel_exec
    else
        echo "Eroare la compilare C++!"
    fi
else
    echo "Eroare la rularea scriptului Python!"
fi

read -n 1 -s -r -p "Apasă orice tastă pentru a închide..."