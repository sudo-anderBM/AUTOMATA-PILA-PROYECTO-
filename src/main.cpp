#include <iostream>
#include "pda.hpp"

int main() {
    PDA a;
    std::string w;
    while (true) {
        std::cout << "\nCadena (e = vacia, salir = terminar): ";
        if (!(std::cin >> w) || w == "salir") break;
        auto r = a.evaluar(w == "e" ? "" : w);
        for (auto& p : r.pasos)
            std::cout << p.estado << " | " << p.porLeer << " | " << p.pila << "\n";
        std::cout << (r.aceptada ? "Cadena aceptada\n" : "Cadena rechazada\n");
    }
}
