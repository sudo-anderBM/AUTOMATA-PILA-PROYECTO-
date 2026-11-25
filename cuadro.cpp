#include "raylib.h"

#include <string>
#include <vector>

struct Paso {
    std::string estado;
    std::string porLeer;
    std::string pila;
};

struct Evaluacion {
    bool aceptada = false;
    bool entradaValida = true;
    std::vector<Paso> pasos;
};

bool esEntradaValida(const std::string& cadena)
{
    for (char simbolo : cadena) {
        if (simbolo != 'a' && simbolo != 'b') {
            return false;
        }
    }

    return true;
}

std::string restante(const std::string& cadena, int posicion)
{
    if (posicion >= static_cast<int>(cadena.size())) {
        return "e";
    }

    return cadena.substr(posicion);
}

Evaluacion evaluarCadena(const std::string& cadena)
{
    Evaluacion evaluacion;
    evaluacion.entradaValida = esEntradaValida(cadena);
    evaluacion.pasos.push_back({"q0", restante(cadena, 0), "Z"});

    if (!evaluacion.entradaValida) {
        return evaluacion;
    }

    std::string pila = "Z";
    const int mitad = static_cast<int>(cadena.size()) / 2;

    for (int posicion = 0; posicion < mitad; posicion++) {
        pila = cadena[posicion] + pila;
        evaluacion.pasos.push_back(
            {"q0", restante(cadena, posicion + 1), pila});
    }

    evaluacion.pasos.push_back({"q1", restante(cadena, mitad), pila});

    for (int posicion = mitad; posicion < static_cast<int>(cadena.size());
         posicion++) {
        if (pila.size() <= 1 || pila.front() != cadena[posicion]) {
            return evaluacion;
        }

        pila.erase(0, 1);
        evaluacion.pasos.push_back(
            {"q1", restante(cadena, posicion + 1), pila});
    }

    if (pila == "Z") {
        evaluacion.pasos.push_back({"q2", "e", pila});
        evaluacion.aceptada = true;
    }

    return evaluacion;
}

bool botonPresionado(Rectangle boton)
{
    return CheckCollisionPointRec(GetMousePosition(), boton) &&
           IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void dibujarFlecha(Vector2 inicio, Vector2 fin, Color color)
{
    DrawLineEx(inicio, fin, 2.0f, color);

    const float tamano = 8.0f;
    Vector2 puntaIzquierda = {
        fin.x - tamano,
        fin.y - tamano / 2.0f
    };
    Vector2 puntaDerecha = {
        fin.x - tamano,
        fin.y + tamano / 2.0f
    };

    DrawTriangle(fin, puntaIzquierda, puntaDerecha, color);
}

void dibujarDiagrama()
{
    DrawText("Diagrama del automata", 25, 82, 24, DARKBLUE);

    const Vector2 q0 = {120, 310};
    const Vector2 q1 = {275, 310};
    const Vector2 q2 = {430, 310};

    DrawCircleV(q0, 42, SKYBLUE);
    DrawCircleLines(static_cast<int>(q0.x), static_cast<int>(q0.y), 42, BLACK);
    DrawCircleV(q1, 42, SKYBLUE);
    DrawCircleLines(static_cast<int>(q1.x), static_cast<int>(q1.y), 42, BLACK);
    DrawCircleV(q2, 42, GREEN);
    DrawCircleLines(static_cast<int>(q2.x), static_cast<int>(q2.y), 42, BLACK);
    DrawCircleLines(static_cast<int>(q2.x), static_cast<int>(q2.y), 35, BLACK);

    DrawText("q0", 107, 300, 22, BLACK);
    DrawText("q1", 262, 300, 22, BLACK);
    DrawText("q2", 417, 300, 22, BLACK);

    dibujarFlecha({40, 310}, {77, 310}, BLACK);
    dibujarFlecha({162, 310}, {233, 310}, BLACK);
    dibujarFlecha({317, 310}, {388, 310}, BLACK);

    DrawText("a/aZ, b/bZ", 62, 245, 17, DARKBLUE);
    DrawText("e/e", 198, 285, 17, DARKBLUE);
    DrawText("a/a, b/b", 325, 245, 17, DARKBLUE);
    DrawText("e/Z", 370, 285, 17, DARKBLUE);

    DrawText("q0: guardar la primera mitad", 30, 410, 18, DARKGRAY);
    DrawText("q1: comparar y sacar de la pila", 30, 440, 18, DARKGRAY);
    DrawText("q2: aceptar", 30, 470, 18, DARKGRAY);
}

void dibujarTabla(const std::vector<Paso>& pasos, int desplazamiento)
{
    const int x = 500;
    const int y = 245;
    const int ancho = 470;
    const int altoFila = 28;
    const int filasVisibles = 13;

    DrawText("Tabla de transiciones", x, 205, 24, DARKBLUE);
    DrawRectangle(x, y, ancho, altoFila, DARKBLUE);
    DrawText("Estado", x + 12, y + 6, 18, WHITE);
    DrawText("Por leer", x + 115, y + 6, 18, WHITE);
    DrawText("Pila", x + 285, y + 6, 18, WHITE);

    for (int fila = 0; fila < filasVisibles; fila++) {
        const int indice = fila + desplazamiento;
        const int filaY = y + altoFila * (fila + 1);

        DrawRectangle(
            x, filaY, ancho, altoFila,
            fila % 2 == 0 ? Color{235, 242, 250, 255}
                          : Color{250, 250, 250, 255});
        DrawRectangleLines(x, filaY, ancho, altoFila, LIGHTGRAY);

        if (indice >= static_cast<int>(pasos.size())) {
            continue;
        }

        const Paso& paso = pasos[indice];
        DrawText(paso.estado.c_str(), x + 12, filaY + 5, 18, BLACK);
        DrawText(paso.porLeer.c_str(), x + 115, filaY + 5, 18, BLACK);
        DrawText(paso.pila.c_str(), x + 285, filaY + 5, 18, BLACK);
    }

    if (pasos.size() > static_cast<size_t>(filasVisibles)) {
        DrawText("Usa la rueda del mouse para ver mas pasos.",
                 x, y + altoFila * (filasVisibles + 1) + 8, 16, DARKGRAY);
    }
}

int main()
{
    InitWindow(1000, 700, "Automata de pila - Palindromos pares");
    SetTargetFPS(60);

    Rectangle cuadroEntrada = {500, 105, 300, 42};
    Rectangle botonEvaluar = {815, 105, 155, 42};
    Rectangle botonLimpiar = {815, 155, 155, 36};
    Rectangle botonSalir = {815, 650, 155, 36};

    char cadenaTexto[100] = "";
    int longitud = 0;
    bool editandoEntrada = false;
    bool seEvaluo = false;
    int desplazamientoTabla = 0;
    Evaluacion evaluacion;

    while (!WindowShouldClose()) {
        if (botonPresionado(cuadroEntrada)) {
            editandoEntrada = true;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            !CheckCollisionPointRec(GetMousePosition(), cuadroEntrada)) {
            editandoEntrada = false;
        }

        if (editandoEntrada) {
            int caracter = GetCharPressed();

            while (caracter > 0) {
                if ((caracter == 'a' || caracter == 'b') && longitud < 99) {
                    cadenaTexto[longitud] = static_cast<char>(caracter);
                    longitud++;
                    cadenaTexto[longitud] = '\0';
                }

                caracter = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && longitud > 0) {
                longitud--;
                cadenaTexto[longitud] = '\0';
            }
        }

        bool solicitarEvaluacion = botonPresionado(botonEvaluar) ||
                                   (editandoEntrada && IsKeyPressed(KEY_ENTER));

        if (solicitarEvaluacion) {
            evaluacion = evaluarCadena(cadenaTexto);
            seEvaluo = true;
            desplazamientoTabla = 0;
        }

        if (botonPresionado(botonLimpiar)) {
            cadenaTexto[0] = '\0';
            longitud = 0;
            seEvaluo = false;
            evaluacion = Evaluacion{};
            desplazamientoTabla = 0;
            editandoEntrada = true;
        }

        if (botonPresionado(botonSalir)) {
            break;
        }

        if (seEvaluo && CheckCollisionPointRec(
                            GetMousePosition(), {500, 245, 470, 420})) {
            desplazamientoTabla -= static_cast<int>(GetMouseWheelMove());
            const int maximoDesplazamiento =
                evaluacion.pasos.size() > 13
                    ? static_cast<int>(evaluacion.pasos.size()) - 13
                    : 0;

            if (desplazamientoTabla < 0) {
                desplazamientoTabla = 0;
            }
            if (desplazamientoTabla > maximoDesplazamiento) {
                desplazamientoTabla = maximoDesplazamiento;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Simulador de automata de pila", 25, 25, 30, DARKBLUE);
        DrawText("Lenguaje: palindromos pares sobre {a,b}", 25, 55, 18, DARKGRAY);

        dibujarDiagrama();

        DrawText("Escribe una cadena:", 500, 75, 20, BLACK);
        DrawRectangleRounded(cuadroEntrada, 0.15f, 8, WHITE);
        DrawRectangleLinesEx(
            cuadroEntrada, editandoEntrada ? 3.0f : 1.0f,
            editandoEntrada ? DARKBLUE : GRAY);
        DrawText(cadenaTexto, cuadroEntrada.x + 10, cuadroEntrada.y + 10,
                 20, BLACK);

        DrawRectangleRounded(botonEvaluar, 0.15f, 8, DARKBLUE);
        DrawText("EVALUAR", 842, 117, 18, WHITE);

        DrawRectangleRounded(botonLimpiar, 0.15f, 8, GOLD);
        DrawText("LIMPIAR", 852, 165, 18, BLACK);

        if (seEvaluo) {
            dibujarTabla(evaluacion.pasos, desplazamientoTabla);

            const char* mensaje = evaluacion.aceptada
                                      ? "Cadena aceptada"
                                      : "Cadena rechazada";
            const Color colorMensaje =
                evaluacion.aceptada ? DARKGREEN : RED;
            DrawText(mensaje, 500, 635, 24, colorMensaje);

            if (!evaluacion.entradaValida) {
                DrawText("Solo se permiten los simbolos a y b.",
                         500, 610, 17, RED);
            }
        } else {
            DrawText("La tabla aparecera despues de evaluar.",
                     500, 280, 20, DARKGRAY);
        }

        DrawRectangleRounded(botonSalir, 0.15f, 8, MAROON);
        DrawText("SALIR", 865, 660, 18, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
