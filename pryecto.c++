#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <limits>
#include <random>
#include <sstream>
#include <curl/curl.h>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace fs = std::filesystem;
using namespace std;

// Estructura para el callback de CURL
struct upload_status {
    size_t bytes_read;
    string data;
};

// Callback para enviar datos por CURL
static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    upload_status *upload_ctx = (upload_status *)userp;
    const char *data = upload_ctx->data.c_str();
    size_t len = upload_ctx->data.length();
    
    if(size == 0 || nmemb == 0 || ((size*nmemb) < 1)) {
        return 0;
    }
    
    if(upload_ctx->bytes_read < len) {
        size_t to_copy = min(size * nmemb, len - upload_ctx->bytes_read);
        memcpy(ptr, data + upload_ctx->bytes_read, to_copy);
        upload_ctx->bytes_read += to_copy;
        return to_copy;
    }
    
    return 0;
}

// Función para cifrar una cadena con Cifrado César
string cifrarCesar(const string& texto, int desplazamiento) {
    string resultado;
    for (char c : texto) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = base + (c - base + desplazamiento) % 26;
        }
        resultado += c;
    }
    return resultado;
}

// Función para descifrar una cadena con Cifrado César
string descifrarCesar(const string& texto, int desplazamiento) {
    return cifrarCesar(texto, 26 - (desplazamiento % 26));
}

// Función para generar contenido aleatorio más variado
string generarContenido(int lineas) {
    static random_device rd;
    static mt19937 gen(rd());
    
    vector<vector<string>> temas = {
        // Tecnología
        {"tecnologia", "computadora", "algoritmo", "programacion", "software", "hardware", "internet", "servidor", "base", "datos"},
        // Ciencia
        {"ciencia", "investigacion", "experimento", "hipotesis", "teoria", "laboratorio", "resultado", "analisis", "observacion", "conclusion"},
        // Naturaleza
        {"naturaleza", "bosque", "montaña", "rio", "oceano", "animal", "planta", "ecosistema", "biodiversidad", "conservacion"},
        // Historia
        {"historia", "civilizacion", "cultura", "sociedad", "tradicion", "epoca", "siglo", "revolucion", "guerra", "paz"},
        // Arte
        {"arte", "pintura", "musica", "literatura", "escultura", "teatro", "creatividad", "expresion", "belleza", "inspiracion"}
    };
    
    string contenido;
    uniform_int_distribution<> tema_dist(0, temas.size() - 1);
    uniform_int_distribution<> palabra_dist(0, 9);
    uniform_int_distribution<> linea_dist(8, 15);
    
    for (int i = 0; i < lineas; ++i) {
        int tema_actual = tema_dist(gen);
        int palabras_por_linea = linea_dist(gen);
        
        for (int j = 0; j < palabras_por_linea; ++j) {
            contenido += temas[tema_actual][palabra_dist(gen)] + " ";
        }
        contenido += "\n";
    }
    
    return contenido;
}

// Función para crear los directorios y archivos
bool crearArchivos() {
    try {
        cout << "Creando directorios y archivos..." << endl;
        
        if (!fs::create_directory("cifrados") && !fs::exists("cifrados")) {
            cerr << "Error: No se pudo crear el directorio 'cifrados'" << endl;
            return false;
        }

        if (!fs::create_directory("no_cifrados") && !fs::exists("no_cifrados")) {
            cerr << "Error: No se pudo crear el directorio 'no_cifrados'" << endl;
            return false;
        }

        // Verificar si ya existen archivos
        bool archivos_existen = true;
        for (int i = 1; i <= 100; ++i) {
            string nombre_archivo = "archivo_" + to_string(i) + ".txt";
            if (!fs::exists(fs::path("cifrados") / nombre_archivo) || 
                !fs::exists(fs::path("no_cifrados") / nombre_archivo)) {
                archivos_existen = false;
                break;
            }
        }
        
        if (archivos_existen) {
            cout << "Los archivos ya existen. Continuando..." << endl;
            return true;
        }

        cout << "Generando 100 archivos cifrados y 100 no cifrados..." << endl;
        
        for (int i = 1; i <= 100; ++i) {
            string nombre_archivo = "archivo_" + to_string(i) + ".txt";
            string contenido = generarContenido(50 + (rand() % 30)); // 50-80 líneas

            // Crear archivo cifrado
            fs::path ruta_cifrado = fs::path("cifrados") / nombre_archivo;
            ofstream cifrado(ruta_cifrado);
            if (!cifrado) {
                cerr << "Error al crear archivo cifrado: " << ruta_cifrado << endl;
                continue;
            }
            cifrado << cifrarCesar(contenido, 3);
            cifrado.close();

            // Crear archivo no cifrado
            fs::path ruta_no_cifrado = fs::path("no_cifrados") / nombre_archivo;
            ofstream no_cifrado(ruta_no_cifrado);
            if (!no_cifrado) {
                cerr << "Error al crear archivo no cifrado: " << ruta_no_cifrado << endl;
                continue;
            }
            no_cifrado << contenido;
            no_cifrado.close();
            
            // Mostrar progreso
            if (i % 20 == 0) {
                cout << "Creados " << i << "/100 archivos..." << endl;
            }
        }
        
        cout << "Archivos creados exitosamente!" << endl;
        return true;
    } catch (const fs::filesystem_error& e) {
        cerr << "Error del sistema de archivos: " << e.what() << endl;
        return false;
    }
}

// Función para buscar archivos por nombre
void buscarPorNombre(const string& nombre) {
    cout << "\n=== Resultados de búsqueda por nombre '" << nombre << "' ===" << endl;
    bool encontrado = false;

    try {
        cout << "\nArchivos cifrados:" << endl;
        for (const auto& entry : fs::directory_iterator("cifrados")) {
            string nombre_archivo = entry.path().filename().string();
            if (nombre_archivo.find(nombre) != string::npos) {
                cout << "  [CIFRADO] " << nombre_archivo << endl;
                encontrado = true;
            }
        }

        cout << "\nArchivos no cifrados:" << endl;
        for (const auto& entry : fs::directory_iterator("no_cifrados")) {
            string nombre_archivo = entry.path().filename().string();
            if (nombre_archivo.find(nombre) != string::npos) {
                cout << "  [NO CIFRADO] " << nombre_archivo << endl;
                encontrado = true;
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Error al buscar archivos: " << e.what() << endl;
    }

    if (!encontrado) {
        cout << "No se encontraron archivos con ese nombre." << endl;
    }
}

// Función para buscar archivos por contenido
void buscarPorContenido(const string& texto) {
    cout << "\n=== Resultados de búsqueda por contenido '" << texto << "' ===" << endl;
    bool encontrado = false;
    const size_t MAX_FILE_SIZE = 10 * 1024 * 1024; // 10MB

    try {
        cout << "\nBuscando en archivos no cifrados..." << endl;
        for (const auto& entry : fs::directory_iterator("no_cifrados")) {
            if (fs::file_size(entry) > MAX_FILE_SIZE) {
                cerr << "Advertencia: Archivo " << entry.path().filename()
                     << " demasiado grande, omitiendo..." << endl;
                continue;
            }

            ifstream archivo(entry.path());
            if (!archivo) {
                cerr << "Error al abrir archivo: " << entry.path() << endl;
                continue;
            }

            string contenido((istreambuf_iterator<char>(archivo)),
                             istreambuf_iterator<char>());

            if (contenido.find(texto) != string::npos) {
                cout << "  [NO CIFRADO] " << entry.path().filename() << endl;
                encontrado = true;
            }
        }

        cout << "\nBuscando en archivos cifrados (descifrando en tiempo real)..." << endl;
        for (const auto& entry : fs::directory_iterator("cifrados")) {
            if (fs::file_size(entry) > MAX_FILE_SIZE) {
                cerr << "Advertencia: Archivo " << entry.path().filename()
                     << " demasiado grande, omitiendo..." << endl;
                continue;
            }

            ifstream archivo(entry.path());
            if (!archivo) {
                cerr << "Error al abrir archivo: " << entry.path() << endl;
                continue;
            }

            string cifrado((istreambuf_iterator<char>(archivo)),
                         istreambuf_iterator<char>());
            string descifrado = descifrarCesar(cifrado, 3);

            if (descifrado.find(texto) != string::npos) {
                cout << "  [CIFRADO] " << entry.path().filename() << endl;
                encontrado = true;
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Error al buscar archivos: " << e.what() << endl;
    }

    if (!encontrado) {
        cout << "No se encontraron archivos con ese contenido." << endl;
    }
}

// Función para validar email
bool validarEmail(const string& email) {
    size_t at_pos = email.find('@');
    if (at_pos == string::npos || at_pos == 0 || at_pos == email.length() - 1) {
        return false;
    }

    size_t dot_pos = email.find('.', at_pos);
    if (dot_pos == string::npos || dot_pos == email.length() - 1) {
        return false;
    }

    // Verificaciones adicionales
    for (char c : email) {
        if (!isalnum(c) && c != '@' && c != '.' && c != '_' && c != '-') {
            return false;
        }
    }

    return true;
}

// Función mejorada para enviar email
bool enviarEmail(const string& email, const string& contenido, const string& nombre_archivo) {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    upload_status upload_ctx;

    curl = curl_easy_init();
    if (!curl) {
        cout << "Error al inicializar CURL." << endl;
        return false;
    }

    // Crear el mensaje de email
    stringstream mensaje;
    mensaje << "To: " << email << "\r\n"
            << "From: douglaslisandr@gmail.com\r\n"
            << "Subject: Archivo descifrado: " << nombre_archivo << "\r\n"
            << "\r\n"
            << "Hola,\r\n\r\n"
            << "Adjunto encontrarás el contenido del archivo descifrado:\r\n\r\n"
            << "=== CONTENIDO DEL ARCHIVO ===\r\n"
            << contenido << "\r\n"
            << "=== FIN DEL CONTENIDO ===\r\n\r\n"
            << "Saludos,\r\n"
            << "Sistema de Búsqueda y Cifrado\r\n";

    upload_ctx.data = mensaje.str();
    upload_ctx.bytes_read = 0;

    try {
        // Configuración de CURL
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_USERNAME, "douglaslisandr@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "enmg xozq wreh zxwh"); // Cambiar por contraseña de aplicación
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "douglaslisandr@gmail.com");
        
        recipients = curl_slist_append(recipients, email.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        
        // Enviar el email
        res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            cout << "Error al enviar el correo: " << curl_easy_strerror(res) << endl;
            return false;
        } else {
            cout << "Correo enviado exitosamente a " << email << endl;
            return true;
        }
    } catch (const exception& e) {
        cout << "Error al enviar el correo: " << e.what() << endl;
        return false;
    }

    // Limpieza
    if (recipients) curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    
    return false;
}

// Función para descifrar y mostrar un archivo
void descifrarArchivo(const string& nombre) {
    string nombre_completo = nombre;
    if (nombre.find(".txt") == string::npos) {
        nombre_completo += ".txt";
    }

    fs::path ruta_archivo = fs::path("cifrados") / nombre_completo;

    if (!fs::exists(ruta_archivo)) {
        cout << "El archivo '" << nombre_completo << "' no existe en el directorio cifrado." << endl;
        return;
    }

    ifstream archivo_cifrado(ruta_archivo);
    if (!archivo_cifrado) {
        cout << "Error al abrir el archivo." << endl;
        return;
    }

    string cifrado((istreambuf_iterator<char>(archivo_cifrado)),
                   istreambuf_iterator<char>());
    string descifrado = descifrarCesar(cifrado, 3);

    cout << "\n" << string(50, '=') << endl;
    cout << "CONTENIDO DESCIFRADO DE: " << nombre_completo << endl;
    cout << string(50, '=') << endl;
    cout << descifrado << endl;
    cout << string(50, '=') << endl;

    // Preguntar si desea enviar por correo
    cout << "\n¿Desea recibir este archivo por correo electrónico? (s/n): ";
    char respuesta;
    cin >> respuesta;
    cin.ignore(); // Limpiar el buffer

    if (respuesta == 's' || respuesta == 'S') {
        cout << "Ingrese su dirección de correo electrónico: ";
        string email;
        getline(cin, email);

        if (!validarEmail(email)) {
            cout << "Dirección de email no válida." << endl;
            return;
        }

        cout << "Enviando correo..." << endl;
        if (enviarEmail(email, descifrado, nombre_completo)) {
            cout << "¡Correo enviado exitosamente!" << endl;
        } else {
            cout << "Error al enviar el correo." << endl;
        }
    }
}

void mostrarMenu() {
    cout << "\n" << string(40, '=') << endl;
    cout << "    SISTEMA DE BÚSQUEDA Y CIFRADO" << endl;
    cout << string(40, '=') << endl;
    cout << "1. Buscar por nombre de archivo" << endl;
    cout << "2. Buscar por contenido" << endl;
    cout << "3. Descifrar archivo específico" << endl;
    cout << "4. Salir" << endl;
    cout << string(40, '-') << endl;
    cout << "Seleccione una opción: ";
}

int main() {
    // Configurar consola en Windows
    #ifdef _WIN32
        system("COLOR 0F");
        SetConsoleCP(65001);
        SetConsoleOutputCP(65001);
    #endif

    // Inicializar libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    cout << "Iniciando Sistema de Búsqueda y Cifrado César..." << endl;
    
    if (!crearArchivos()) {
        cerr << "Error al crear archivos. El programa puede no funcionar correctamente." << endl;
        return 1;
    }

    int opcion;
    string entrada;

    do {
        mostrarMenu();

        while (!(cin >> opcion)) {
            cout << "Entrada no válida. Por favor ingrese un número." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            mostrarMenu();
        }
        cin.ignore();

        switch (opcion) {
            case 1:
                cout << "\nIngrese el nombre o parte del nombre a buscar: ";
                getline(cin, entrada);
                buscarPorNombre(entrada);
                break;
            case 2:
                cout << "\nIngrese el texto a buscar: ";
                getline(cin, entrada);
                buscarPorContenido(entrada);
                break;
            case 3:
                cout << "\nIngrese el nombre del archivo cifrado a descifrar: ";
                getline(cin, entrada);
                descifrarArchivo(entrada);
                break;
            case 4:
                cout << "\nSaliendo del programa..." << endl;
                break;
            default:
                cout << "Opción no válida. Intente de nuevo." << endl;
        }
        
        if (opcion != 4) {
            cout << "\nPresione Enter para continuar...";
            cin.get();
        }
        
    } while (opcion != 4);

    // Limpiar libcurl
    curl_global_cleanup();

    cout << "¡Gracias por usar el sistema!" << endl;
    return 0;
}