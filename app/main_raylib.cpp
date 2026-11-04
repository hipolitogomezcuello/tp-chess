// Composition root de la UI gráfica (arquitectura §3.10, D-17). En la misión 8.1 solo abre la ventana.
#include "adapters/raylib/RaylibUI.hpp"

int main() {
    chess::adapters::raylib::RaylibUI().run();
    return 0;
}
