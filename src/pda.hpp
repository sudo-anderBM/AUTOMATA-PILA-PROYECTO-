#pragma once
#include <string>
#include <vector>

struct Paso {
    std::string estado;
    std::string porLeer;
    std::string pila;
};

struct Resultado {
    bool aceptada = false;
    std::vector<Paso> pasos;
};

class PDA {
public:
    Resultado evaluar(const std::string& w) const {
        Resultado r;
        std::vector<Paso> camino;
        r.aceptada = dfs(0, w, 0, "Z", camino);
        r.pasos = camino;
        return r;
    }
private:
    static Paso mk(int q, const std::string& w, size_t i, const std::string& pila) {
        std::string rest = w.substr(i);
        return { "q" + std::to_string(q), rest.empty() ? "e" : rest, pila };
    }
    bool dfs(int q, const std::string& w, size_t i, std::string pila,
             std::vector<Paso>& camino) const {
        camino.push_back(mk(q, w, i, pila));
        bool hay = i < w.size();
        if (q == 2) {
            if (i == w.size()) return true;
        } else if (q == 0) {
            if (hay && dfs(0, w, i + 1, std::string(1, w[i]) + pila, camino)) return true;
            if (dfs(1, w, i, pila, camino)) return true;
        } else if (q == 1) {
            if (hay && pila.size() > 1 && pila[0] == w[i] &&
                dfs(1, w, i + 1, pila.substr(1), camino)) return true;
            if (pila == "Z" && dfs(2, w, i, pila, camino)) return true;
        }
        camino.pop_back();
        return false;
    }
};
