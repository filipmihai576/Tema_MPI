from PIL import Image

# Calea către imaginea de input (descarcată de pe internet)
input_image_path = "image.jpg"  # Înlocuiește cu calea ta

# Dimensiunile dorite pentru imaginea mărită
new_width = 5000
new_height = 5000


try:
    # Deschide imaginea
    image = Image.open(input_image_path)

    # Redimensionează imaginea
    resized_image = image.resize((new_width, new_height))

    # Salvează imaginea redimensionată
    resized_image.save("input.jpg")

    print("Imagine redimensionată cu succes!")

except FileNotFoundError:
    print(f"Eroare: Fișierul {input_image_path} nu a fost găsit.")
except Exception as e:
    print(f"A apărut o eroare: {e}")