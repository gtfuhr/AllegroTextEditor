// principal.cpp
// Programa principal do Editor.
//
// The MIT License (MIT)
//
// Copyright (c) 2018 João Vicente Ferreira Lima, UFSM
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

#include <chrono>
#include <list>
#include <random>
#include <vector>
#include <iostream>
#include <functional>
#include <fstream>

#include "Tela.hpp"
#include "geom.hpp"

using namespace tela;
using namespace geom;

// estados do editor
enum Estado
{
  nada,
  fim,
  editando
};

struct janela_pos
{
  const int tamanho_linhas_tela = 26, qtd_char = 73;
  int x, y;
};

// Estrutura para controlar todos os objetos e estados do Editor Centipede
struct Editor
{
  std::vector<char *> linhas;
  Estado estado;   // estado do editor
  Tela t;          // estrutura que controla a tela
  int tecla;       // ultima tecla apertada pelo usuario
  int modificador; // ultima tecla modificador apertada pelo usuario
  int nlin;        // numero de linhas no texto (numero de elementos atualmente em linhas)
  int lincur;      // linha onde esta o cursor (comeca em 0)
  int colcur;      // coluna onde esta o cursoe na linha acima
  int lin1;        // linha do texto mostrada na primeira linha da tela
  int col1;        // coluna do texto mostrada na primeira coluna da tela
  struct janela_pos pos_janela;
  // inicia estruturas principais do editor
  void inicia(void)
  {
    t.inicia(600, 400, "Editor de texto");
    estado = Estado::nada;

    nlin = 0;
    lincur = 0;
    colcur = 0;
    lin1 = 0;
    col1 = 0;
    pos_janela.x = 0;
    pos_janela.y = 0;
  }

  // atualiza o editor, como se fosse um passo na execução
  void atualiza(void)
  {
    t.limpa();
    checa_capacidade_do_editor();
    desenha_texto();
    desenha_cursor();
    t.mostra();
    // espera 60 ms antes de atualizar a tela
    t.espera(60);
  }

  void checa_capacidade_do_editor(void)
  {
    if ((int)linhas.capacity() > tamanho() + 20)
      return;
    linhas.reserve(linhas.capacity() + 20);
  }

  // verifica se o editor terminou ou não
  // - retorna TRUE quando  termina
  // - retorna FALSE caso contrário
  bool verifica_fim(void)
  {
    // le ultima tecla
    tecla = t.tecla();
    modificador = t.tecla_modificador();
    bool teclaDigitavel = false;
    if ((tecla >= ALLEGRO_KEY_A && tecla <= ALLEGRO_KEY_Z) || tecla == ALLEGRO_KEY_SPACE)
      teclaDigitavel = true;

    /*Digitou um caractere*/
    if (teclaDigitavel == true && (estado == Estado::editando))
    {
      char letra = t.letra();
      if (modificador != 514)
        insere_char(letra);
    }
    if ((tecla == ALLEGRO_KEY_DELETE) && (estado == Estado::editando))
    {
      remove_char();
    }
    /* apertou CRTL + Q ? */
    if (tecla == ALLEGRO_KEY_Q && (modificador & ALLEGRO_KEYMOD_CTRL))
    {
      std::cout << "CTRL+Q SAIR\n";
      estado = Estado::fim;
    }
    if (tecla == ALLEGRO_KEY_S && (modificador & ALLEGRO_KEYMOD_CTRL))
    {
      std::cout << "CTRL+S SALVAR e PARAR DE EDITAR\n";
      /* muda estado na variável para não editar */
      estado = Estado::nada;
    }
    if (tecla == ALLEGRO_KEY_E && (modificador & ALLEGRO_KEYMOD_CTRL))
    {
      std::cout << "CTRL+E EDITAR\n";
      /* muda estado na variável para editando */
      estado = Estado::editando;
    }
    if (tecla == ALLEGRO_KEY_G && (modificador & ALLEGRO_KEYMOD_CTRL))
    {
      std::cout << "CTRL+G GRUDAR LINHA\n";
      gruda_linha();
    }
    if (tecla == ALLEGRO_KEY_B && (modificador & ALLEGRO_KEYMOD_CTRL))
    {
      std::cout << "CTRL+B QUEBRAR LINHA\n";
      quebra_linha();
    }

    /* teclas direcionais 
      ALLEGRO_KEY_LEFT
      ALLEGRO_KEY_RIGHT
      ALLEGRO_KEY_UP
      ALLEGRO_KEY_DOWN
    */
    if (tecla == ALLEGRO_KEY_LEFT)
      move_esq();
    if (tecla == ALLEGRO_KEY_RIGHT)
      move_dir();
    if (tecla == ALLEGRO_KEY_UP)
      move_cima();
    if (tecla == ALLEGRO_KEY_DOWN)
      move_baixo();

    if (estado != Estado::fim)
      return false;
    else
      return true;
  }

  void finaliza(void)
  {
    // fecha a tela
    t.finaliza();
    auto tam = tamanho();
    std::string l;
    for (auto i = 0; i < tam; i++)
    {
      l = remove_linha(linhas[i]);
      std::cout << "DEL: " << l << std::endl;
    }
  }
  void fatia_string(char *inteira, char *fatia, int ini, int fim)
  {
    int j = 0;
    if (ini < (int)strlen(inteira))
      for (; inteira[ini] != '\0' && ini < fim; ini++, j++)
        fatia[j] = inteira[ini];
    fatia[j] = '\0';
  }

  // desenha todas as figuras e objetos na tela
  void desenha_texto()
  {
    Tamanho tt;
    Ponto pt;
    Cor preto = {0, 0, 0};
    // Cor azul = {0.2, 0.3, 0.8};
    // Cor vermelho = {1, 0.2, 0};
    // Cor verde = {0.2, 0.9, 0.6};
    char *sub_text;
    sub_text = new char[pos_janela.qtd_char];
    t.cor(preto);
    int tam = tamanho();
    for (auto i = pos_janela.y, j = 0; j < pos_janela.tamanho_linhas_tela && j < tam; j++, i++)
    {
      fatia_string(linhas[i], sub_text, pos_janela.x, pos_janela.x + pos_janela.qtd_char);
      tt = t.tamanho_texto(sub_text);

      /* calcula posicao da nova linha */
      pt.x = 1;
      pt.y = j * tt.alt + 1;

      /* desenha linha */
      t.texto(pt, sub_text);
    }
    delete[] sub_text;
  }

  void legenda(void)
  {
    std::cout << "Pressione: " << std::endl;
    std::cout << " - CTRL+q sair\n";
    std::cout << " - CTRL+s salvar e parar de editar\n";
    std::cout << " - CTRL+e editar\n";
    std::cout << " - CTRL+b quebra linha\n";
    std::cout << " - CTRL+g gruda linha\n";
  }

  void adiciona_linha(std::string linha)
  {
    char *linhaChar = new char[linha.length() + 1];
    strcpy(linhaChar, linha.c_str());
    linhas.push_back(linhaChar);
  }

  std::string remove_linha(char *l)
  {
    std::string retorno(l);
    delete[] l;
    return retorno;
  }

  int tamanho(void)
  {
    return linhas.size();
  }

  void desenha_cursor(void)
  {
    Cor preto = {0, 0, 0};
    Tamanho tt;
    Ponto pt1, pt2;
    int i;
    {
      char *subtexto = new char[colcur + 2];
      char *texto_exibido = new char[pos_janela.qtd_char];
      fatia_string(linhas[lincur + pos_janela.y], texto_exibido, pos_janela.x, pos_janela.x + pos_janela.qtd_char);
      for (i = 0; i < colcur + 1; i++)
        subtexto[i] = texto_exibido[i];
      subtexto[i] = '\0';
      tt = t.tamanho_texto(subtexto);
      delete[] subtexto;
    }

    /* posicao x (horizontal) do cursor */
    pt1.x = tt.larg + 0.2;
    /* posicao y (vertical) do cursor */
    pt1.y = lincur * tt.alt;
    pt2.x = pt1.x;
    pt2.y = pt1.y + tt.alt;
    t.cor(preto);
    t.linha(pt1, pt2);
  }

  void move_esq()
  {
    if (colcur >= 0)
      colcur--;
    else if (colcur == -1 && lincur + pos_janela.y > 0)
    {
      int lengthAcima = strlen(linhas[lincur + pos_janela.y - 1]);
      lincur--;
      colcur = strlen(linhas[lincur + pos_janela.y]) - 1;
      if (lengthAcima > pos_janela.qtd_char)
      {
        pos_janela.x = lengthAcima - pos_janela.qtd_char;
        colcur = lengthAcima - pos_janela.x - 1;
      }
    }
    else if (colcur < pos_janela.x && colcur != 0 && pos_janela.x != 0)
      pos_janela.x--;
  }

  void aumentaColcur()
  {
    (colcur) < (pos_janela.qtd_char) ? colcur++ : 0;

    if (colcur == pos_janela.qtd_char)
    {
      pos_janela.x++;
      colcur--;
    }
  }

  void move_dir()
  {
    int tam = strlen(linhas[lincur + pos_janela.y]);
    if (colcur < tam - 1)
      aumentaColcur();
    if (colcur + pos_janela.x == tam - 1 && lincur + pos_janela.y < tamanho() - 1)
    {
      lincur++;
      colcur = -1;
      pos_janela.x = 0;
    }
  }

  void aumentaLincur()
  {
    (lincur) < (pos_janela.tamanho_linhas_tela - 1) ? lincur++ : 0;
  }

  void move_baixo()
  {
    int tam = tamanho();
    if (lincur + pos_janela.y < tam - 1)
    {
      int lengthAbaixo = strlen(linhas[lincur + 1 + pos_janela.y]);

      if (colcur + 1 > lengthAbaixo)
      {
        colcur = lengthAbaixo - 1;
        aumentaLincur();
      }
      else
        aumentaLincur();
      int finalTela = pos_janela.tamanho_linhas_tela + pos_janela.y;
      if (lincur + pos_janela.y == finalTela - 1)
        pos_janela.y++;
    }
  }

  void move_cima()
  {
    if (lincur + pos_janela.y > 0)
    {
      int lengthAcima = strlen(linhas[lincur + pos_janela.y - 1]);
      if (colcur + 1 < lengthAcima)
        lincur--;
      else
      {
        colcur = lengthAcima - 1;
        lincur--;
      }
      if (lincur == pos_janela.y && lincur != 0)
        pos_janela.y--;
    }
  }

  void carrega(std::string arquivo)
  {
    std::string linha;
    std::ifstream entrada{arquivo};
    while (std::getline(entrada, linha))
    {
      adiciona_linha(linha);
    }
  }

  void salva(std::string arquivo)
  {
    std::ofstream saida{arquivo};
    auto tam = tamanho();
    saida.clear();
    for (auto i = 0; i < tam; i++)
    {
      std::string linha(linhas[i]);
      saida << linha << std::endl;
    }
    saida.close();
  }

  void insere_char(char c)
  {
    char *whole, *newWhole;
    whole = linhas[lincur + pos_janela.y];
    auto tamLinha = strlen(whole);
    newWhole = new char[tamLinha + 2];
    int i;

    for (i = 0; i < colcur + pos_janela.x + 1; i++)
      newWhole[i] = whole[i];
    newWhole[i] = c;
    for (int j = i + 1; whole[i] != '\0'; i++, j++)
      newWhole[j] = whole[i];
    newWhole[i + 1] = '\0';
    delete[] whole;
    linhas[lincur + pos_janela.y] = newWhole;
    colcur++;
  }

  void remove_char(void)
  {
    int lincurReal = lincur + pos_janela.y;
    std::vector<char *>::iterator pos = linhas.begin() + lincurReal;
    char *whole, *newWhole;
    whole = linhas[lincurReal];
    int tamLinha = strlen(whole);
    newWhole = new char[tamLinha];
    int i;

    for (i = 0; i <= colcur + pos_janela.x; i++)
    {
      if (whole[i] != '\0')
        newWhole[i] = whole[i];
      else
        newWhole[i] = '\0';
    }
    if (whole[i] != '\0')
      for (; i < tamLinha; i++)
      {
        newWhole[i] = whole[i + 1];
      }
    else
      newWhole[i] = '\0';
    delete[] whole;
    linhas.erase(pos);
    linhas.insert(pos, newWhole);
  }

  void gruda_linha(void)
  {
    auto tam = tamanho() - 1;
    int lincurReal = lincur + pos_janela.y;
    if (lincurReal < tam)
    {
      std::string s1(linhas[lincurReal]);
      std::string s2(linhas[lincurReal + 1]);
      delete[] linhas[lincurReal];
      delete[] linhas[lincurReal + 1];
      linhas[lincurReal] = new char[s1.size() + s2.size() + 1];
      strcpy(linhas[lincurReal], s1.c_str());
      strcat(linhas[lincurReal], s2.c_str());
      linhas.erase(linhas.begin() + lincurReal + 1);
    }
  }

  void quebra_linha(void)
  {
    char *whole, *right_half, *left_half;
    int lincurReal = lincur + pos_janela.y, i, j;
    whole = linhas[lincurReal];
    int tamLinha = strlen(whole), colcurReal = colcur + pos_janela.x;
    left_half = new char[colcurReal + 2];
    right_half = new char[(tamLinha - colcurReal) + 1];

    for (i = 0; i < colcurReal + 1; i++)
      left_half[i] = whole[i];
    left_half[i] = '\0';
    for (j = 0; i < tamLinha; i++, j++)
      right_half[j] = whole[i];
    right_half[j] = '\0';

    delete[] whole;
    linhas[lincurReal] = left_half;
    linhas.insert(linhas.begin() + lincurReal + 1, right_half);
    lincur++;
    colcur = -1;
    pos_janela.x = 0;
  }
};

int main(int argc, char **argv)
{
  Editor editor;

  editor.inicia(); //
  editor.carrega("texto.txt");
  editor.legenda();

  while (editor.verifica_fim() == false)
  {
    editor.atualiza();
  }
  editor.salva("saida.txt");
  editor.finaliza();

  return 0;
}
