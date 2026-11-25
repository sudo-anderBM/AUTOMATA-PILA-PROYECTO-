#include "raylib.h"
#include <string>
#include <vector>

// ==========================================
// 1. ESTRUCTURAS DE DATOS
// ==========================================

// Representa una fila en la tabla dinámica de la simulación
struct Paso
{
    std::string estado;   // Estado actual del autómata (q0, q1 o q2)
    std::string porLeer;  // Subcadena restante por procesar
    std::string pila;     // Contenido actual de la pila (tope a la izquierda)
};

// Almacena el resultado general tras evaluar la cadena
struct Evaluacion
{
    bool aceptada = false;      // True si la cadena es un palíndromo par válido
    bool entradaValida = true;  // False si contiene caracteres distintos de 'a' y 'b'
    std::vector<Paso> pasos;    // Historial con todos los pasos generados
};

// ==========================================
// 2. LÓGICA DEL AUTÓMATA DE PILA (PDA)
// ==========================================

// Valida que el alfabeto sea únicamente {a, b}
bool esEntradaValida(const std::string& cadena)
{
    for (char simbolo : cadena)
    {
        if (simbolo != 'a' && simbolo != 'b')
        {
            return false;
        }
    }
    return true;
}

// Devuelve los símbolos restantes por leer o "e" (épsilon) si ya terminó
std::string restante(const std::string& cadena, int posicion)
{
    if (posicion >= static_cast<int>(cadena.size()))
    {
        return "e";
    }
    return cadena.substr(posicion);
}

// Simula el funcionamiento del autómata paso por paso
Evaluacion evaluarCadena(const std::string& cadena)
{
    Evaluacion eval;
    eval.entradaValida = esEntradaValida(cadena);

    // Configuración inicial: q0, cadena completa por leer y pila con símbolo de fondo 'Z'
    eval.pasos.push_back({"q0", restante(cadena, 0), "Z"});

    // Si la entrada no pertenece al alfabeto, se rechaza de inmediato
    if (!eval.entradaValida)
    {
        return eval;
    }

    std::string pila = "Z";
    int mitad = static_cast<int>(cadena.size()) / 2;

    // FASE 1 (q0): Lee la primera mitad de la cadena y apila cada símbolo
    for (int i = 0; i < mitad; i++)
    {
        pila = cadena[i] + pila;  // Push: se inserta al inicio (tope)
        eval.pasos.push_back({"q0", restante(cadena, i + 1), pila});
    }

    // Transición espontánea (épsilon): cambia de q0 a q1 a la mitad exacta
    eval.pasos.push_back({"q1", restante(cadena, mitad), pila});

    // FASE 2 (q1): Lee la segunda mitad, compara con el tope y desapila
    for (int i = mitad; i < static_cast<int>(cadena.size()); i++)
    {
        // Si la pila se vació antes de tiempo o el símbolo no coincide, se rechaza
        if (pila.size() <= 1 || pila.front() != cadena[i])
        {
            return eval;
        }
        pila.erase(0, 1);  // Pop: retira el símbolo en el tope
        eval.pasos.push_back({"q1", restante(cadena, i + 1), pila});
    }

    // FASE 3 (q2): Si la pila volvió a quedar solo en 'Z', la cadena es aceptada
    if (pila == "Z")
    {
        eval.pasos.push_back({"q2", "e", pila});
        eval.aceptada = true;
    }

    return eval;
}

// ==========================================
// 3. UTILIDADES DE INTERFAZ GRÁFICA
// ==========================================

// Comprueba si el cursor está sobre un rectángulo y se presionó clic izquierdo
bool botonPresionado(Rectangle boton)
{
    return CheckCollisionPointRec(GetMousePosition(), boton) &&
           IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// Dibuja una línea con punta triangular simulando una flecha dirigida
void dibujarFlecha(Vector2 inicio, Vector2 fin, Color color)
{
    DrawLineEx(inicio, fin, 2.0f, color);

    // Vértices del triángulo de la punta
    Vector2 puntaIzq = {fin.x - 8, fin.y - 4};
    Vector2 puntaDer = {fin.x - 8, fin.y + 4};
    DrawTriangle(fin, puntaIzq, puntaDer, color);
}

// ==========================================
// 4. DIBUJO DEL DIAGRAMA DE ESTADOS
// ==========================================

// Dibuja los estados q0, q1, q2, sus transiciones y descripciones
void dibujarDiagrama()
{
    DrawText("Diagrama del automata", 25, 82, 24, DARKBLUE);

    // Coordenadas centrales de cada círculo de estado
    Vector2 q0 = {120, 310};
    Vector2 q1 = {275, 310};
    Vector2 q2 = {430, 310};

    // Estado q0 (inicial)
    DrawCircleV(q0, 42, SKYBLUE);
    DrawCircleLines(120, 310, 42, BLACK);
    DrawText("q0", 107, 300, 22, BLACK);

    // Estado q1 (comparación)
    DrawCircleV(q1, 42, SKYBLUE);
    DrawCircleLines(275, 310, 42, BLACK);
    DrawText("q1", 262, 300, 22, BLACK);

    // Estado q2 (aceptación: color verde y doble contorno)
    DrawCircleV(q2, 42, GREEN);
    DrawCircleLines(430, 310, 42, BLACK);
    DrawCircleLines(430, 310, 35, BLACK);
    DrawText("q2", 417, 300, 22, BLACK);

    // Flechas de transición entre estados
    dibujarFlecha({40, 310}, {78, 310}, BLACK);     // Flecha de entrada al estado inicial
    dibujarFlecha({162, 310}, {233, 310}, BLACK);    // q0 -> q1
    dibujarFlecha({317, 310}, {388, 310}, BLACK);    // q1 -> q2

    // Etiquetas con las operaciones de transición (Lectura / Pila)
    DrawText("a/aZ, b/bZ", 62, 245, 17, DARKBLUE);  // En q0 se apila
    DrawText("e/e", 198, 285, 17, DARKBLUE);          // Transición épsilon q0 -> q1
    DrawText("a/a, b/b", 325, 245, 17, DARKBLUE);    // En q1 se desapila
    DrawText("e/Z", 370, 285, 17, DARKBLUE);          // Transición épsilon q1 -> q2

    // Leyenda de lo que hace cada estado
    DrawText("q0: guardar la primera mitad", 30, 410, 18, DARKGRAY);
    DrawText("q1: comparar y sacar de la pila", 30, 440, 18, DARKGRAY);
    DrawText("q2: aceptar", 30, 470, 18, DARKGRAY);
}

// ==========================================
// 5. DIBUJO DE LA TABLA DINÁMICA
// ==========================================

// Renderiza las columnas: Estado | Por leer | Pila con soporte para scroll
void dibujarTabla(const std::vector<Paso>& pasos, int desplazamiento)
{
    int x = 500;
    int y = 245;
    int ancho = 470;
    int altoFila = 28;
    int filasVisibles = 13;

    DrawText("Tabla de transiciones", x, 205, 24, DARKBLUE);

    // Encabezado de la tabla
    DrawRectangle(x, y, ancho, altoFila, DARKBLUE);
    DrawText("Estado",   x + 12,  y + 6, 18, WHITE);
    DrawText("Por leer", x + 115, y + 6, 18, WHITE);
    DrawText("Pila",     x + 285, y + 6, 18, WHITE);

    // Dibuja las filas visibles según el desplazamiento del scroll
    for (int fila = 0; fila < filasVisibles; fila++)
    {
        int indice = fila + desplazamiento;
        int filaY = y + altoFila * (fila + 1);

        // Fondo alternado (efecto cebra)
        Color colorFondo = (fila % 2 == 0)
            ? Color{235, 242, 250, 255}
            : Color{250, 250, 250, 255};

        DrawRectangle(x, filaY, ancho, altoFila, colorFondo);
        DrawRectangleLines(x, filaY, ancho, altoFila, LIGHTGRAY);

        // Si ya no hay más pasos en el historial, deja la fila en blanco
        if (indice >= static_cast<int>(pasos.size()))
        {
            continue;
        }

        // Datos del paso
        const Paso& paso = pasos[indice];
        DrawText(paso.estado.c_str(),  x + 12,  filaY + 5, 18, BLACK);
        DrawText(paso.porLeer.c_str(), x + 115, filaY + 5, 18, BLACK);
        DrawText(paso.pila.c_str(),    x + 285, filaY + 5, 18, BLACK);
    }

    // Indicador visual si hay más filas ocultas
    if (pasos.size() > static_cast<size_t>(filasVisibles))
    {
        DrawText("Usa la rueda del mouse para ver mas pasos.",
                 x, y + altoFila * (filasVisibles + 1) + 8, 16, DARKGRAY);
    }
}

// ==========================================
// 6. FUNCIÓN PRINCIPAL (BUCLE DEL JUEGO)
// ==========================================

int main()
{
    // Inicialización de la ventana de Raylib
    InitWindow(1000, 700, "Automata de pila - Palindromos pares");
    SetTargetFPS(60);

    // Dimensiones y posiciones de los componentes de la interfaz
    Rectangle cuadroEntrada = {500, 105, 300, 42};
    Rectangle botonEvaluar  = {815, 105, 155, 42};
    Rectangle botonLimpiar  = {815, 155, 155, 36};
    Rectangle botonSalir    = {815, 650, 155, 36};

    // Variables de control y estado de la aplicación
    char cadenaTexto[100] = "";
    int longitud = 0;
    bool editandoEntrada = false;
    bool seEvaluo = false;
    int desplazamientoTabla = 0;
    Evaluacion evaluacion;

    // Bucle principal: corre mientras no se cierre la ventana ni se presione Salir
    while (!WindowShouldClose())
    {
        // ------------------------------------------
        // A. ACTUALIZACIÓN / MANEJO DE EVENTOS
        // ------------------------------------------

        // Activa o desactiva el foco de texto al hacer clic
        if (botonPresionado(cuadroEntrada))
        {
            editandoEntrada = true;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            !CheckCollisionPointRec(GetMousePosition(), cuadroEntrada))
        {
            editandoEntrada = false;
        }

        // Lectura de teclado: captura solo caracteres 'a' y 'b'
        if (editandoEntrada)
        {
            int caracter = GetCharPressed();
            while (caracter > 0)
            {
                if ((caracter == 'a' || caracter == 'b') && longitud < 99)
                {
                    cadenaTexto[longitud] = static_cast<char>(caracter);
                    longitud++;
                    cadenaTexto[longitud] = '\0';
                }
                caracter = GetCharPressed();
            }

            // Borrar un carácter con Backspace
            if (IsKeyPressed(KEY_BACKSPACE) && longitud > 0)
            {
                longitud--;
                cadenaTexto[longitud] = '\0';
            }
        }

        // Evalúa al presionar el botón EVALUAR o pulsar Enter en el teclado
        bool solicitarEvaluacion = botonPresionado(botonEvaluar) ||
                                   (editandoEntrada && IsKeyPressed(KEY_ENTER));
        if (solicitarEvaluacion)
        {
            evaluacion = evaluarCadena(cadenaTexto);
            seEvaluo = true;
            desplazamientoTabla = 0;  // Reinicia la posición del scroll
        }

        // Botón LIMPIAR: reinicia la entrada y los resultados
        if (botonPresionado(botonLimpiar))
        {
            cadenaTexto[0] = '\0';
            longitud = 0;
            seEvaluo = false;
            evaluacion = Evaluacion{};
            desplazamientoTabla = 0;
            editandoEntrada = true;
        }

        // Botón SALIR: termina la aplicación
        if (botonPresionado(botonSalir))
        {
            break;
        }

        // Scroll de la tabla usando la rueda del ratón
        if (seEvaluo)
        {
            desplazamientoTabla -= static_cast<int>(GetMouseWheelMove());
            int maximo = static_cast<int>(evaluacion.pasos.size()) - 13;
            if (maximo < 0) maximo = 0;
            if (desplazamientoTabla < 0) desplazamientoTabla = 0;
            if (desplazamientoTabla > maximo) desplazamientoTabla = maximo;
        }

        // ------------------------------------------
        // B. RENDERIZADO / DIBUJO EN PANTALLA
        // ------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Títulos de cabecera
        DrawText("Simulador de automata de pila", 25, 25, 30, DARKBLUE);
        DrawText("Lenguaje: palindromos pares sobre {a,b}", 25, 55, 18, DARKGRAY);

        // Diagrama visual del autómata
        dibujarDiagrama();

        // Caja de texto de entrada
        DrawText("Escribe una cadena:", 500, 75, 20, BLACK);
        DrawRectangleRounded(cuadroEntrada, 0.15f, 8, WHITE);
        DrawRectangleLinesEx(cuadroEntrada,
                             editandoEntrada ? 3.0f : 1.0f,
                             editandoEntrada ? DARKBLUE : GRAY);
        DrawText(cadenaTexto, cuadroEntrada.x + 10, cuadroEntrada.y + 10, 20, BLACK);

        // Botones de acción
        DrawRectangleRounded(botonEvaluar, 0.15f, 8, DARKBLUE);
        DrawText("EVALUAR", 842, 117, 18, WHITE);

        DrawRectangleRounded(botonLimpiar, 0.15f, 8, GOLD);
        DrawText("LIMPIAR", 852, 165, 18, BLACK);

        // Mostrar tabla y veredicto si ya se evaluó una cadena
        if (seEvaluo)
        {
            dibujarTabla(evaluacion.pasos, desplazamientoTabla);

            // Mensaje final de aceptación o rechazo
            const char* mensaje = evaluacion.aceptada
                ? "Cadena aceptada" : "Cadena rechazada";
            Color colorMensaje = evaluacion.aceptada ? DARKGREEN : RED;
            DrawText(mensaje, 500, 635, 24, colorMensaje);

            // Mensaje extra si se detectó un carácter no permitido
            if (!evaluacion.entradaValida)
            {
                DrawText("Solo se permiten los simbolos a y b.",
                         500, 610, 17, RED);
            }
        }
        else
        {
            DrawText("La tabla aparecera despues de evaluar.",
                     500, 280, 20, DARKGRAY);
        }

        // Botón Salir
        DrawRectangleRounded(botonSalir, 0.15f, 8, MAROON);
        DrawText("SALIR", 865, 660, 18, WHITE);

        EndDrawing();
    }

    // Liberar recursos de la ventana
    CloseWindow();
    return 0;
}
