// geom.hpp
// declaracoes de tipos de dados e funcoes para manipulacao de formas
// geometricas simples (ponto, retangulo, circulo)
//
// The MIT License (MIT)
//
// Copyright (c) 2017,2018 João Vicente Ferreira Lima, UFSM
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cmath>

namespace geom {

// Um ponto, constituído por uma coordenada no eixo x e no eixo y
struct Ponto {
    float x;
    float y;
};

// Tamanho, constituído por altura e largura, do tipo float
struct Tamanho {
    float larg;
    float alt;
};

// Retangulo, constituído por um ponto (que contém as coordenadas do canto
//              inferior esquerdo do retangulo) e um tamanho
struct Retangulo {
    Ponto pos;
    Tamanho tam;
};

// Circulo, constituido por um ponto (o centro do circulo) e um float (o raio)
struct Circulo {
    Ponto centro;
    float raio;
};

// Funções

// retorna a distancia entre dois pontos
inline float distancia(Ponto p1, Ponto p2) {
    return std::sqrt(std::pow((p2.x - p1.x), 2) + std::pow((p2.y - p1.y), 2));
}

// retorna true se o ponto estiver dentro do circulo, false caso contrario
inline bool ptemcirc(Ponto p, Circulo c) {
    if (distancia(p, c.centro) > c.raio)
        return false;
    else
        return true;
}

// retorna true se o ponto estiver dentro do retangulo
inline bool ptemret(Ponto p, Retangulo r) {
    return (r.pos.x < p.x && r.pos.x + r.tam.larg > p.x && r.pos.y < p.y &&
            r.pos.y + r.tam.alt > p.y);
    if ((p.x <= r.pos.x + r.tam.larg) && (p.y <= r.pos.y + r.tam.alt))
        return true;
    else
        return false;
}

// retorna true se houver uma interseccao entre o circulo e o retangulo
inline bool intercr(Circulo c, Retangulo r) {
    Ponto lim;
    if (ptemret(c.centro, r))
        return true;
    else {
        if (c.centro.x < r.pos.x)
            lim.x = r.pos.x - c.centro.x;
        else if (c.centro.x > r.pos.x + r.tam.larg)
            lim.x = c.centro.x - r.pos.x + r.tam.larg;
        else
            lim.x = 0;

        if (c.centro.y < r.pos.y)
            lim.y = r.pos.y - c.centro.y;
        else if (c.centro.y > r.pos.y + r.tam.alt)
            lim.y = c.centro.y - r.pos.y + r.tam.alt;
        else
            lim.y = 0;

        if (distancia(c.centro, lim) < c.raio)
            return true;
        else
            return false;
    }
}

// retorna true se houver uma interseccao entre os dois retangulos
inline bool interrr(Retangulo r1, Retangulo r2) {
    Ponto _r1, _r2;
    _r1.x = r1.pos.x + r1.tam.larg;
    _r1.y = r1.pos.y + r1.tam.alt;
    _r2.x = r2.pos.x + r2.tam.larg;
    _r2.y = r2.pos.y + r2.tam.alt;

    if (ptemret(r1.pos, r2) || ptemret(r2.pos, r1) || ptemret(_r1, r2) ||
        ptemret(_r2, r1))
        return true;
    else
        return false;
}

// retorna true se houver uma interseccao entre os dois circulos
inline bool intercc(Circulo c1, Circulo c2) {
    if (distancia(c1.centro, c2.centro) > c1.raio + c2.raio)
        return true;
    else
        return false;
}

}; // namespace geom
