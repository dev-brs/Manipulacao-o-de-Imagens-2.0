#pragma once

#include <iostream>
#include <fstream>
#include <vector>

struct CAB
{
    unsigned char fType[2];
    unsigned int fSize;
    unsigned short mR1;
    unsigned short mR2;
    unsigned int byteshift;
};

struct DADOS
{
    unsigned int tamanho_cab;
    unsigned int largura;
    unsigned int altura;
    unsigned short planos;
    unsigned short bitcount;
    unsigned int bCompress;
    unsigned long int tamanho_img_byte;
    unsigned int resXPPMeter;
    unsigned int resYPPMeter;
    unsigned int numClrUsed;
    unsigned int numClrImpor;
};

class Imagem
{
private:
    std::string nome_da_img;
    std::string diretorio_saida;
    std::string caminho_do_arquivo;
    CAB cabecalho;
    DADOS dados;

    std::string extrairDiretorio(const std::string &caminho);
    bool diretorioExiste(const std::string &diretorio);
    bool criarDiretorio(const std::string &diretorio);

public:
    Imagem(const std::string &nome_da_imagem);
    void __init__(const std::string &nome_da_imagem);
    void diretorioSaida(const std::string &caminho_de_saida_escolhido);
    void imprimirDados();
    void copiar(const std::string &nome_nova_imagem);
    void pretoBranco();
    void blur();
};
