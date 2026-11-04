#pragma once

namespace chess::adapters::raylib {

// Adaptador gráfico (raylib). En la misión 8.1 solo abre la ventana; el tablero y la interacción
// se agregan en la 8.2.
class RaylibUI {
public:
    void run();
};

}  // namespace chess::adapters::raylib
