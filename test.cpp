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

    cout << "Probando movimientos basicos (secuencias)..." << endl;
    for (int i = 1; i <= 5; i++) {
        im.move_left(20);
        string nombre = "move_left_" + to_string(i) + ".png";
        im.draw(nombre.c_str());
    }

    for (int i = 1; i <= 5; i++) {
        im.move_right(20);
        string nombre = "move_right_" + to_string(i) + ".png";
        im.draw(nombre.c_str());
    }

    for (int i = 1; i <= 5; i++) {
        im.move_up(20);
        string nombre = "move_up_" + to_string(i) + ".png";
        im.draw(nombre.c_str());
    }

    for (int i = 1; i <= 5; i++) {
        im.move_down(20);
        string nombre = "move_down_" + to_string(i) + ".png";
        im.draw(nombre.c_str());
    }

    for (int i = 1; i <= 4; i++) {
        im.rotate();
        string nombre = "rotate_" + to_string(i) + ".png";
        im.draw(nombre.c_str());
    }

    cout << "Probando undo y redo..." << endl;
    im.undo();
    im.draw("undo_1.png");
    im.undo();
    im.draw("undo_2.png");
    im.redo();
    im.draw("redo_1.png");
    im.redo();
    im.draw("redo_2.png");

    cout << "Probando repeat..." << endl;
    im.repeat();
    im.draw("repeat_1.png");
    im.repeat();
    im.draw("repeat_2.png");

    cout << "Ejecutando repeat_all (pelicula)..." << endl;
    im.repeat_all();

    cout << "Terminado! Revisa tu carpeta." << endl;
    
    return 0;
}