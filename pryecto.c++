#include <iostream>
#include <fstream>
#include <windows.h>
#include <array>
#include <filesystem>

int cantidad = 100; // Cambia la cantidad de archivos a crear aquí
using namespace std;

namespace fs = std::filesystem;

void CrearArchivosEnDirectorio(const string& directorio, int cantidad);

int main() {
    // Creación de los directorios
    // en la carpeta de trabajo actual
    string directorio1("directorio1");
    string directorio2("directorio2");
  
    fs::create_directory(directorio1)
        ? cout << "created directory - " << directorio1 << endl
        : cout << "create_directory() failed" << endl;
    fs::create_directory(directorio2)
        ? cout << "created directory - " << directorio2 << endl
        : cout << "create_directory() failed" << endl;

    // Crear archivos en ambos directorios
    CrearArchivosEnDirectorio(directorio1, cantidad);
    CrearArchivosEnDirectorio(directorio2, cantidad);
    
    return EXIT_SUCCESS;
}

void CrearArchivosEnDirectorio(const string& directorio, int cantidad) {
    // Crear archivos en el directorio especificado
    string nombreArchivo = "file";
    for (int i = 0; i < cantidad; ++i) {
        string rutaArchivo = directorio + "/" + nombreArchivo + to_string(i + 1) + ".txt";
        
        ofstream archivo(rutaArchivo);
        if (archivo.is_open()) {
            archivo.close();
        } else {
            cout << "Error al crear el archivo: " << rutaArchivo << endl;
        }
    }
    cout << cantidad << " archivos creados con éxito en " << directorio << endl;
}