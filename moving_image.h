/**
 * @file moving_image.h
 * @brief Implementación de manipulación de imágenes usando estructuras de datos lineales.
 */

#ifndef MOVING_IMG_H
#define MOVING_IMG_H

#include "basics.h"
#include <iostream>
#include <stack>
#include <queue>
#include <cmath>
#include <string>
#include <sys/stat.h> // Para crear directorios de forma portable
#include <sys/types.h>  // Para mkdir en sistemas POSIX
#ifdef _WIN32 // Para compatibilidad con Windows
#include <direct.h> // Para _mkdir en Windows
#endif

using namespace std;

// Enumeración de acciones para el historial
enum TipoAccion { 
    MOVER_IZQ, MOVER_DER, MOVER_ARRIBA, MOVER_ABAJO, ROTAR 
};

struct NodoHistorial {
    TipoAccion tipo;
    int magnitud;
};

class moving_image {
private:
    // Capas dinámicas exigidas
    unsigned char **capa_roja;
    unsigned char **capa_verde;
    unsigned char **capa_azul;
    unsigned char **base_roja;
    unsigned char **base_verde;
    unsigned char **base_azul;

    // Estructuras de datos
    stack<NodoHistorial> pila_deshacer;
    stack<NodoHistorial> pila_rehacer;
    queue<NodoHistorial> cola_ejecucion;
    NodoHistorial ultima_accion;
    bool tiene_ultima_accion = false;

    void registrar_accion(const NodoHistorial& accion, bool limpiar_redo) {
        cola_ejecucion.push(accion);
        ultima_accion = accion;
        tiene_ultima_accion = true;
        if (limpiar_redo) limpiar_rehacer();
    }

    void asignar_memoria() {
        capa_roja = new unsigned char*[H_IMG];
        capa_verde = new unsigned char*[H_IMG];
        capa_azul = new unsigned char*[H_IMG];
        base_roja = new unsigned char*[H_IMG];
        base_verde = new unsigned char*[H_IMG];
        base_azul = new unsigned char*[H_IMG];
        for (int i = 0; i < H_IMG; i++) {
            capa_roja[i] = new unsigned char[W_IMG];
            capa_verde[i] = new unsigned char[W_IMG];
            capa_azul[i] = new unsigned char[W_IMG];
            base_roja[i] = new unsigned char[W_IMG];
            base_verde[i] = new unsigned char[W_IMG];
            base_azul[i] = new unsigned char[W_IMG];
        }
    }

    void liberar_memoria() {
        for (int i = 0; i < H_IMG; i++) {
            delete[] capa_roja[i];
            delete[] capa_verde[i];
            delete[] capa_azul[i];
            delete[] base_roja[i];
            delete[] base_verde[i];
            delete[] base_azul[i];
        }
        delete[] capa_roja;
        delete[] capa_verde;
        delete[] capa_azul;
        delete[] base_roja;
        delete[] base_verde;
        delete[] base_azul;
    }

    void limpiar_rehacer() {
        while (!pila_rehacer.empty()) pila_rehacer.pop();
    }

    void guardar_base() {
        for (int i = 0; i < H_IMG; i++) {
            for (int j = 0; j < W_IMG; j++) {
                base_roja[i][j] = capa_roja[i][j];
                base_verde[i][j] = capa_verde[i][j];
                base_azul[i][j] = capa_azul[i][j];
            }
        }
    }

    void restaurar_base() {
        for (int i = 0; i < H_IMG; i++) {
            for (int j = 0; j < W_IMG; j++) {
                capa_roja[i][j] = base_roja[i][j];
                capa_verde[i][j] = base_verde[i][j];
                capa_azul[i][j] = base_azul[i][j];
            }
        }
    }

    // --- LÓGICA DE MOVIMIENTOS INTERNOS ---
    void trasladar_horizontal(int d, bool es_historial = false) {
        if (!es_historial) {
            NodoHistorial acc = {d > 0 ? MOVER_DER : MOVER_IZQ, abs(d)};
            pila_deshacer.push(acc);
            registrar_accion(acc, true);
        }

        for (int i = 0; i < H_IMG; i++) {
            unsigned char *fila_t_r = new unsigned char[W_IMG];
            unsigned char *fila_t_g = new unsigned char[W_IMG];
            unsigned char *fila_t_b = new unsigned char[W_IMG];

            for (int j = 0; j < W_IMG; j++) {
                // Movimiento circular seguro con números negativos
                int nuevo_j = ((j + d) % W_IMG + W_IMG) % W_IMG; 
                fila_t_r[nuevo_j] = capa_roja[i][j];
                fila_t_g[nuevo_j] = capa_verde[i][j];
                fila_t_b[nuevo_j] = capa_azul[i][j];
            }
            for (int j = 0; j < W_IMG; j++) {
                capa_roja[i][j] = fila_t_r[j];
                capa_verde[i][j] = fila_t_g[j];
                capa_azul[i][j] = fila_t_b[j];
            }
            delete[] fila_t_r;
            delete[] fila_t_g;
            delete[] fila_t_b;
        }
    }

    void trasladar_vertical(int d, bool es_historial = false) {
        if (!es_historial) {
            NodoHistorial acc = {d > 0 ? MOVER_ABAJO : MOVER_ARRIBA, abs(d)};
            pila_deshacer.push(acc);
            registrar_accion(acc, true);
        }

        unsigned char **tmp_r = new unsigned char*[H_IMG];
        unsigned char **tmp_g = new unsigned char*[H_IMG];
        unsigned char **tmp_b = new unsigned char*[H_IMG];
        for (int i = 0; i < H_IMG; i++) {
            tmp_r[i] = new unsigned char[W_IMG];
            tmp_g[i] = new unsigned char[W_IMG];
            tmp_b[i] = new unsigned char[W_IMG];
        }

        for (int i = 0; i < H_IMG; i++) {
            int nuevo_i = ((i + d) % H_IMG + H_IMG) % H_IMG;
            for (int j = 0; j < W_IMG; j++) {
                tmp_r[nuevo_i][j] = capa_roja[i][j];
                tmp_g[nuevo_i][j] = capa_verde[i][j];
                tmp_b[nuevo_i][j] = capa_azul[i][j];
            }
        }

        for (int i = 0; i < H_IMG; i++) {
            for (int j = 0; j < W_IMG; j++) {
                capa_roja[i][j] = tmp_r[i][j];
                capa_verde[i][j] = tmp_g[i][j];
                capa_azul[i][j] = tmp_b[i][j];
            }
            delete[] tmp_r[i];
            delete[] tmp_g[i];
            delete[] tmp_b[i];
        }
        delete[] tmp_r;
        delete[] tmp_g;
        delete[] tmp_b;
    }

    void rotar_90(bool es_historial = false) {
        if (!es_historial) {
            NodoHistorial acc = {ROTAR, 0};
            pila_deshacer.push(acc);
            registrar_accion(acc, true);
        }

        unsigned char **tmp_r = new unsigned char*[H_IMG];
        unsigned char **tmp_g = new unsigned char*[H_IMG];
        unsigned char **tmp_b = new unsigned char*[H_IMG];
        for (int i = 0; i < H_IMG; i++) {
            tmp_r[i] = new unsigned char[W_IMG];
            tmp_g[i] = new unsigned char[W_IMG];
            tmp_b[i] = new unsigned char[W_IMG];
        }

        for (int i = 0; i < H_IMG; i++) {
            for (int j = 0; j < W_IMG; j++) {
                // Matemáticas ajustadas a rotación ANTIHORARIA
                tmp_r[W_IMG - 1 - j][i] = capa_roja[i][j];
                tmp_g[W_IMG - 1 - j][i] = capa_verde[i][j];
                tmp_b[W_IMG - 1 - j][i] = capa_azul[i][j];
            }
        }

        for (int i = 0; i < H_IMG; i++) {
            for (int j = 0; j < W_IMG; j++) {
                capa_roja[i][j] = tmp_r[i][j];
                capa_verde[i][j] = tmp_g[i][j];
                capa_azul[i][j] = tmp_b[i][j];
            }
            delete[] tmp_r[i];
            delete[] tmp_g[i];
            delete[] tmp_b[i];
        }
        delete[] tmp_r;
        delete[] tmp_g;
        delete[] tmp_b;
    }

public:
    moving_image() {
        asignar_memoria();
        
        for (int i = 0; i < H_IMG; i++) {
            for (int j = 0; j < W_IMG; j++) {
                capa_roja[i][j] = DEFAULT_R;
                capa_verde[i][j] = DEFAULT_G;
                capa_azul[i][j] = DEFAULT_B;
            }
        }
        
        for (int i = 0; i < 322; i++) {
            for (int j = 0; j < 256; j++) {
                if (s_R[i][j] != 0 || s_G[i][j] != 0 || s_B[i][j] != 0) {
                    capa_roja[INIT_Y + i][INIT_X + j] = s_R[i][j];
                    capa_verde[INIT_Y + i][INIT_X + j] = s_G[i][j];
                    capa_azul[INIT_Y + i][INIT_X + j] = s_B[i][j];
                }
            }
        }

        guardar_base();
    }

    ~moving_image() {
        liberar_memoria();
    }

    // --- INTERFAZ PÚBLICA ---
    void move_left(int d) { trasladar_horizontal(-d); }
    void move_right(int d) { trasladar_horizontal(d); }
    void move_up(int d) { trasladar_vertical(-d); }
    void move_down(int d) { trasladar_vertical(d); }
    void rotate() { rotar_90(); }

    void undo() {
        if (pila_deshacer.empty()) return;
        NodoHistorial ultima = pila_deshacer.top();
        pila_deshacer.pop();
        pila_rehacer.push(ultima);
        NodoHistorial aplicada;

        if (ultima.tipo == MOVER_DER) {
            trasladar_horizontal(-ultima.magnitud, true);
            aplicada = {MOVER_IZQ, ultima.magnitud};
        } else if (ultima.tipo == MOVER_IZQ) {
            trasladar_horizontal(ultima.magnitud, true);
            aplicada = {MOVER_DER, ultima.magnitud};
        } else if (ultima.tipo == MOVER_ABAJO) {
            trasladar_vertical(-ultima.magnitud, true);
            aplicada = {MOVER_ARRIBA, ultima.magnitud};
        } else if (ultima.tipo == MOVER_ARRIBA) {
            trasladar_vertical(ultima.magnitud, true);
            aplicada = {MOVER_ABAJO, ultima.magnitud};
        } else {
            rotar_90(true); rotar_90(true); rotar_90(true); // 3 rotaciones antihorarias = 1 horaria
            aplicada = {ROTAR, 0};
        }

        registrar_accion(aplicada, false);
    }

    void redo() {
        if (pila_rehacer.empty()) return;
        NodoHistorial rev = pila_rehacer.top();
        pila_rehacer.pop();
        
        pila_deshacer.push(rev);
        registrar_accion(rev, false);
        
        if (rev.tipo == MOVER_DER) trasladar_horizontal(rev.magnitud, true);
        else if (rev.tipo == MOVER_IZQ) trasladar_horizontal(-rev.magnitud, true);
        else if (rev.tipo == MOVER_ABAJO) trasladar_vertical(rev.magnitud, true);
        else if (rev.tipo == MOVER_ARRIBA) trasladar_vertical(-rev.magnitud, true);
        else if (rev.tipo == ROTAR) rotar_90(true);
    }

    void repeat_all() {
        restaurar_base();
        queue<NodoHistorial> copia = cola_ejecucion;
        int frame = 0;
        
        while (!copia.empty()) {
            NodoHistorial acc = copia.front();
            copia.pop();
            
            if (acc.tipo == MOVER_DER) trasladar_horizontal(acc.magnitud, true);
            else if (acc.tipo == MOVER_IZQ) trasladar_horizontal(-acc.magnitud, true);
            else if (acc.tipo == MOVER_ABAJO) trasladar_vertical(acc.magnitud, true);
            else if (acc.tipo == MOVER_ARRIBA) trasladar_vertical(-acc.magnitud, true);
            else if (acc.tipo == ROTAR) rotar_90(true);

            string numero = to_string(frame);
            if (frame < 10) numero = "0" + numero;
            string nombre = "repeat_all_" + numero + ".png";
            
            draw(nombre.c_str());
            frame++;
        }
    }

    void repeat() {
        if (!tiene_ultima_accion) return;
        NodoHistorial acc = ultima_accion;
        pila_deshacer.push(acc);
        registrar_accion(acc, true);

        if (acc.tipo == MOVER_DER) trasladar_horizontal(acc.magnitud, true);
        else if (acc.tipo == MOVER_IZQ) trasladar_horizontal(-acc.magnitud, true);
        else if (acc.tipo == MOVER_ABAJO) trasladar_vertical(acc.magnitud, true);
        else if (acc.tipo == MOVER_ARRIBA) trasladar_vertical(-acc.magnitud, true);
        else if (acc.tipo == ROTAR) rotar_90(true);
    }

    void draw(const char* nombre) {
        // Uso de HEAP para evitar Segmentation Fault
        unsigned char* rgb = new unsigned char[H_IMG * W_IMG * 3];
        unsigned char* p = rgb;
        
        for (int i = 0; i < H_IMG; i++) {
            for (int j = 0; j < W_IMG; j++) {
                *p++ = capa_roja[i][j];
                *p++ = capa_verde[i][j];
                *p++ = capa_azul[i][j];
            }
        }
        
        #ifdef _WIN32
        _mkdir("imagenes");
        #else
        mkdir("imagenes", 0755);
        #endif
        string ruta = string("imagenes/") + nombre;
        FILE *fp = fopen(ruta.c_str(), "wb");
        if (fp) {
            svpng(fp, W_IMG, H_IMG, rgb, 0);
            fclose(fp);
        }
        delete[] rgb;
    }
};

#endif