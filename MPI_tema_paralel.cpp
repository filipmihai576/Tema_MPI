#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <thread> 
#include <algorithm> 

using namespace std;

struct Pixel {
    unsigned char r, g, b;
};


void convert_chunk_to_grayscale(vector<Pixel>& pixels_chunk, size_t start_index, size_t end_index) {
    for (size_t i = start_index; i < end_index; ++i) {
        unsigned char gray = static_cast<unsigned char>(
            0.2126 * pixels_chunk[i].r +
            0.7152 * pixels_chunk[i].g +
            0.0722 * pixels_chunk[i].b
        );
        pixels_chunk[i].r = gray;
        pixels_chunk[i].g = gray;
        pixels_chunk[i].b = gray;
    }
}


int main() {

   
    int width, height, channels;
    unsigned char *imageData = stbi_load("input.jpg", &width, &height, &channels, 3);
    if (imageData == nullptr) {
        cerr << "Eroare la incarcarea imaginii!" << endl;
        return 1;
    }

    vector<Pixel> pixels(width * height);
    for (int i = 0; i < width * height; ++i) {
        pixels[i].r = imageData[i * channels + 0];
        pixels[i].g = imageData[i * channels + 1];
        pixels[i].b = imageData[i * channels + 2];
    }
    stbi_image_free(imageData);

    // Începe măsurarea timpului pentru conversie
    auto start_conversion = chrono::high_resolution_clock::now();

    
    size_t num_pixels = pixels.size();
    if (num_pixels > 0) { 
        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) {
            num_threads = 1; 
        }
        // Pentru imagini foarte mici, nu are sens sa folosim mai multe thread-uri decat pixeli
        num_threads = std::min(num_threads, static_cast<unsigned int>(num_pixels));
        if (num_threads == 0 && num_pixels > 0) num_threads = 1; 


        vector<thread> threads;
        threads.reserve(num_threads); 

        size_t chunk_size = (num_pixels + num_threads - 1) / num_threads; 

        for (unsigned int i = 0; i < num_threads; ++i) {
            size_t start_idx = i * chunk_size;
            size_t end_idx = std::min(start_idx + chunk_size, num_pixels);

            if (start_idx < end_idx) { 
                threads.emplace_back(convert_chunk_to_grayscale, std::ref(pixels), start_idx, end_idx);
            }
        }

        // Așteptăm ca toate thread-urile să își termine execuția
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    } else {
        
        for (auto& pixel : pixels) {
            unsigned char gray = static_cast<unsigned char>(0.2126 * pixel.r + 0.7152 * pixel.g + 0.0722 * pixel.b);
            pixel.r = gray;
            pixel.g = gray;
            pixel.b = gray;
        }
    }


    // Opreste măsurarea timpului pentru conversie
    auto end_conversion = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_conversion = end_conversion - start_conversion;


    
    unsigned char *outputData = new unsigned char[width * height * 3]; 
    for (int i = 0; i < width * height; ++i) {
        outputData[i * 3 + 0] = pixels[i].r;
        outputData[i * 3 + 1] = pixels[i].g;
        outputData[i * 3 + 2] = pixels[i].b;
    }


    stbi_write_jpg("output_parallel.jpg", width, height, 3, outputData, 100);  
    delete[] outputData;

    cout << "Conversia paralelizata a fost realizata cu succes!" << endl;
    cout << "Timp de execuție conversie alb-negru: " << fixed << setprecision(5) << duration_conversion.count() << " secunde" << endl;


    return 0;
}