#include <iostream>
#include <fstream>
#include <windows.h>
#include <array>
#include <filesystem>

int cantidad = 100; // Cambia la cantidad de archivos a crear aquí
using namespace std;

namespace fs = std::filesystem;

void CrearArchivos(int cantidad);

int main() {
    // creacion de los directorios
    // en la carpeta de trabajo actual
    string directory_name("directorio1");
    string directory_name2("directorio2");
  
    fs::create_directory(directory_name)
        ? cout << "created directory - " << directory_name << endl
        : cout << "create_directory() failed" << endl;
    fs::create_directory(directory_name2)
        ? cout << "created directory - " << directory_name2 << endl
        : cout << "create_directory() failed" << endl;

    
    return EXIT_SUCCESS;

  }

void CrearArchivos(int cantidad) {
    // Crear archivos en el directorio1
    string nombreArchivo = "file";
    string nombresArchivos[cantidad];
    for (int i = 0; i < cantidad; ++i) {
        string rutaArchivo = "directorio1/" + nombreArchivo + to_string(i + 1) + ".txt";
        nombresArchivos[i] = rutaArchivo;

        ofstream archivo(ruta   Archivo);
        if (archivo.is_open()) {
            archivo.close();
        } else {
            cout << "Error al crear el archivo: " << rutaArchivo << endl;
        }
    }
    cout << cantidad << " Archivos creados con Exito" << endl;
}    