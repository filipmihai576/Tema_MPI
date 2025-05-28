#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <mpi.h>

using namespace std;

struct Pixel {
    unsigned char r, g, b;
};

int main(int argc, char** argv) {
    // Inițializare MPI
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Doar procesul rădăcină citește imaginea
    int width = 0, height = 0, channels = 0;
    unsigned char *imageData = nullptr;
    vector<Pixel> pixels;
    
    if (rank == 0) {
        // 1. Citirea imaginii (JPEG)
        imageData = stbi_load("input.jpg", &width, &height, &channels, 3);
        if (imageData == nullptr) {
            cerr << "Eroare la incarcarea imaginii!" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }

        // Conversie la vector de pixeli
        pixels.resize(width * height);
        for (int i = 0; i < width * height; ++i) {
            pixels[i].r = imageData[i * channels + 0]; 
            pixels[i].g = imageData[i * channels + 1];
            pixels[i].b = imageData[i * channels + 2];
        }
        stbi_image_free(imageData);
    }

    // Transmiterea dimensiunilor imaginii către toate procesele
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&channels, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcularea dimensiunii porțiunii pentru fiecare proces
    int total_pixels = width * height;
    int chunk_size = total_pixels / size;
    int remainder = total_pixels % size;
    
    // Ajustarea dimensiunilor porțiunilor pentru procese
    vector<int> sendcounts(size);
    vector<int> displs(size);
    
    int offset = 0;
    for (int i = 0; i < size; i++) {
        sendcounts[i] = chunk_size + (i < remainder ? 1 : 0);
        displs[i] = offset;
        offset += sendcounts[i];
    }

    // Alocarea buffer-ului local pentru fiecare proces
    vector<Pixel> local_pixels(sendcounts[rank]);

    // Distribuirea pixelilor către toate procesele
    MPI_Scatterv(pixels.data(), sendcounts.data(), displs.data(), 
                 MPI_BYTE, local_pixels.data(), sendcounts[rank] * sizeof(Pixel),
                 MPI_BYTE, 0, MPI_COMM_WORLD);

    // Începere măsurare timp
    double start_time = MPI_Wtime();
    
    // Procesarea porțiunii locale
    for (auto& pixel : local_pixels) {
        unsigned char gray = 0.2126 * pixel.r + 0.7152 * pixel.g + 0.0722 * pixel.b;
        pixel.r = pixel.g = pixel.b = gray;
    }

    // Colectarea pixelilor procesați înapoi la procesul rădăcină
    MPI_Gatherv(local_pixels.data(), sendcounts[rank], MPI_BYTE,
                pixels.data(), sendcounts.data(), displs.data(),
                MPI_BYTE, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();
    double duration = end_time - start_time;

    // Doar procesul rădăcină scrie imaginea de ieșire
    if (rank == 0) {
        // Conversie înapoi la date brute
        unsigned char *outputData = new unsigned char[width * height * channels];
        for (int i = 0; i < width * height; ++i) {
            outputData[i * channels + 0] = pixels[i].r;
            outputData[i * channels + 1] = pixels[i].g;
            outputData[i * channels + 2] = pixels[i].b;
        }
        
        stbi_write_jpg("output.jpg", width, height, channels, outputData, 100);
        delete[] outputData;

        cout << "Conversia a fost realizata cu succes!" << endl;
        cout << "Timp de execuție: " << fixed << setprecision(2) << duration << " secunde" << endl;
    }

    MPI_Finalize();
    return 0;
}