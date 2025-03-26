#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace std;

struct Pixel {
    unsigned char r, g, b;
};

int main() {

    // Începe măsurarea timpului
    auto start = chrono::high_resolution_clock::now();

    // 1. Citirea imaginii (JPEG)
    int width, height, channels;
    unsigned char *imageData = stbi_load("input.jpg", &width, &height, &channels, 3);
    if (imageData == nullptr) {
        cerr << "Eroare la incarcarea imaginii!" << endl;
        return 1;
    }

    // Conversie la vector de pixeli
    vector<Pixel> pixels(width * height);
    for (int i = 0; i < width * height; ++i) {
        pixels[i].r = imageData[i * channels + 0]; 
        pixels[i].g = imageData[i * channels + 1];
        pixels[i].b = imageData[i * channels + 2];
    }
    stbi_image_free(imageData);



    // 2. Conversie la alb-negru (luminosity method)
    for (auto& pixel : pixels) {
        unsigned char gray = 0.2126 * pixel.r + 0.7152 * pixel.g + 0.0722 * pixel.b;
        pixel.r = pixel.g = pixel.b = gray;
    }


    // 3. Scrierea imaginii alb-negru (JPEG)
    unsigned char *outputData = new unsigned char[width * height * channels];
    for (int i = 0; i < width * height; ++i) {
        outputData[i * channels + 0] = pixels[i].r;
        outputData[i * channels + 1] = pixels[i].g;
        outputData[i * channels + 2] = pixels[i].b;
    }
    
    stbi_write_jpg("output.jpg", width, height, channels, outputData, 100);  // Calitate 100 (0-100)
    delete[] outputData;

     // Opreste măsurarea timpului
     auto end = chrono::high_resolution_clock::now();
     chrono::duration<double> duration = end - start;
     
 

    cout << "Conversia a fost realizata cu succes!" << endl;
    cout << "Timp de execuție: " << fixed << setprecision(2) << duration.count() << " secunde" << endl;


    return 0;
}