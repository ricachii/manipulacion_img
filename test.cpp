#include "moving_image.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

int main() {
    // ---------------------------------------------------------
    // COMANDO DE LIMPIEZA AUTOMÁTICA
    cout << "Limpiando imagenes de la ejecucion anterior..." << endl;
    #ifdef _WIN32
    system("del /Q imagenes\\*.png 2>nul"); // Borra los PNG silenciosamente
    #else
    system("rm -f imagenes/*.png");
    #endif
    // ---------------------------------------------------------

    moving_image im;

    cout << "Generando secuencia move_down (10 frames)..." << endl;
    for (int i = 1; i <= 10; i++) {
        im.move_down(10);
        string nombre = "move_down_" + to_string(i) + ".png";
        im.draw(nombre.c_str());
    }

    cout << "Generando secuencia rotate (4 frames)..." << endl;
    for (int i = 1; i <= 4; i++) {
        im.rotate();
        string nombre = "rotate_" + to_string(i) + ".png";
        im.draw(nombre.c_str());
    }

    cout << "Añadiendo mas movimientos a la cola..." << endl;
    for (int i = 0; i < 20; i++) im.move_right(10);
    for (int i = 0; i < 20; i++) im.move_up(10);

    cout << "Ejecutando repeat_all..." << endl;
    im.repeat_all();

    cout << "¡Terminado! Revisa tu carpeta." << endl;
    
    return 0;
}