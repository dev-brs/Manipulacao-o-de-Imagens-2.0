#include "images.h"
#include <dirent.h>

Imagem::Imagem(const std::string &nome_da_imagem)
{
    nome_da_img = nome_da_imagem;
    diretorio_saida = extrairDiretorio(nome_da_imagem);
    caminho_do_arquivo = nome_da_imagem;
    __init__(nome_da_imagem);
}

void Imagem::__init__(const std::string &nome_da_imagem)
{
    std::ifstream imagem(nome_da_imagem, std::ios::binary);

    if (!imagem.is_open())
    {
        std::cerr << "Erro: A imagem de origem não pode ser aberta." << std::endl;
        return;
    }

    imagem.read(reinterpret_cast<char *>(&cabecalho), 14);
    imagem.read(reinterpret_cast<char *>(&dados), 40);

    imagem.close();
}

std::string Imagem::extrairDiretorio(const std::string &caminho)
{
    size_t pos = caminho.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        return caminho.substr(0, pos + 1);
    }
    return "";
}

bool Imagem::diretorioExiste(const std::string &diretorio)
{
    std::ifstream dir(diretorio);
    return dir.good();
}

bool Imagem::criarDiretorio(const std::string &diretorio)
{
    return mkdir(diretorio.c_str()) == 0;
}

void Imagem::diretorioSaida(const std::string &caminho_de_saida_escolhido)
{
    diretorio_saida = caminho_de_saida_escolhido + '/';

    if (!diretorioExiste(diretorio_saida))
    {
        if (criarDiretorio(diretorio_saida))
        {
            std::cout << "Diretório de saída criado: " << diretorio_saida << std::endl;
        }
        else
        {
            std::cerr << "Erro: Não foi possível criar o diretório de saída." << std::endl;
            diretorio_saida = "";
        }
    }
}

void Imagem::imprimirDados()
{
    std::cout << "Dados do Cabecalho:" << std::endl;
    std::cout << "Tipo do arquivo: " << cabecalho.fType[0] << cabecalho.fType[1] << std::endl;
    std::cout << "\nDados da Imagem:" << std::endl;
    std::cout << "Nome da imagem: " << nome_da_img << std::endl;
    std::cout << "Tamanho da imagem (bytes): " << dados.tamanho_img_byte << std::endl;
    std::cout << "Largura da imagem: " << dados.largura << std::endl;
    std::cout << "Altura da imagem: " << dados.altura << std::endl;
    std::cout << "Profundidade: " << dados.bitcount << std::endl;
}

void Imagem::copiar(const std::string &nome_nova_imagem)
{
    std::ofstream nova_imagem(diretorio_saida + nome_nova_imagem, std::ios::binary);

    if (!nova_imagem.is_open())
    {
        std::cerr << "Erro: Não foi possível criar a nova imagem." << std::endl;
        return;
    }

    nova_imagem.write(reinterpret_cast<char *>(&cabecalho), 14);
    nova_imagem.write(reinterpret_cast<char *>(&dados), 40);

    std::ifstream imagem(caminho_do_arquivo, std::ios::binary);
    imagem.seekg(cabecalho.byteshift, std::ios::beg);

    for (unsigned int i = 0; i < dados.altura; ++i)
    {
        for (unsigned int j = 0; j < dados.largura; ++j)
        {
            char pixel[3];
            imagem.read(pixel, 3);
            nova_imagem.write(pixel, 3);
        }
    }

    nova_imagem.close();
    imagem.close();

    std::cout << "Nova imagem criada: " << diretorio_saida + nome_nova_imagem << std::endl;
}

void Imagem::pretoBranco()
{
    std::ofstream nova_imagem_pb(diretorio_saida + "nova_imagem_pb.bmp", std::ios::binary);

    if (!nova_imagem_pb.is_open())
    {
        std::cerr << "Erro: Não foi possível criar a nova imagem em preto e branco." << std::endl;
        return;
    }

    nova_imagem_pb.write(reinterpret_cast<char *>(&cabecalho), 14);
    nova_imagem_pb.write(reinterpret_cast<char *>(&dados), 40);

    std::ifstream imagem(caminho_do_arquivo, std::ios::binary);
    imagem.seekg(cabecalho.byteshift, std::ios::beg);

    int linha_bytes = dados.largura * 3;
    std::vector<char> linha(linha_bytes);

    for (unsigned int i = 0; i < dados.altura; i++)
    {
        imagem.read(&linha[0], linha_bytes);
        for (int j = 0; j < linha_bytes; j += 3)
        {
            unsigned char blue = linha[j];
            unsigned char green = linha[j + 1];
            unsigned char red = linha[j + 2];

            unsigned char gray = ((red + green + blue) / 3);

            linha[j] = linha[j + 1] = linha[j + 2] = gray;
        }
        nova_imagem_pb.write(&linha[0], linha_bytes);
    }

    imagem.close();
    nova_imagem_pb.close();

    std::cout << "Nova imagem em preto e branco criada." << std::endl;
}

void Imagem::blur()
{
    std::ofstream nova_imagem_blur(diretorio_saida + "nova_imagem_blur.bmp", std::ios::binary);

    if (!nova_imagem_blur.is_open())
    {
        std::cerr << "Erro: Não foi possível criar a nova imagem com efeito blur." << std::endl;
        return;
    }

    nova_imagem_blur.write(reinterpret_cast<char *>(&cabecalho), 14);
    nova_imagem_blur.write(reinterpret_cast<char *>(&dados), 40);

    std::ifstream imagem(caminho_do_arquivo, std::ios::binary);
    imagem.seekg(cabecalho.byteshift, std::ios::beg);

    int linha_bytes = dados.largura * 3;
    std::vector<char> linha(linha_bytes);
    std::vector<char> linha_blur(linha_bytes);

    for (unsigned int i = 0; i < dados.altura; ++i)
    {
        imagem.read(&linha[0], linha_bytes);

        for (unsigned int j = 0; j < linha_bytes; j += 3)
        {
            unsigned int aux = j / 3;
            int media_red = linha[j];
            int media_green = linha[j + 1];
            int media_blue = linha[j + 2];

            if (aux > 0 && aux < dados.largura - 1)
            {
                media_red += (linha[j - 3] + linha[j] + linha[j + 3]) / 3;
                media_green += (linha[j - 2] + linha[j + 1] + linha[j + 4]) / 3;
                media_blue += (linha[j - 1] + linha[j + 2] + linha[j + 5]) / 3;

                media_red /= 2;
                media_green /= 2;
                media_blue /= 2;
            }

            linha_blur[j] = media_red;
            linha_blur[j + 1] = media_green;
            linha_blur[j + 2] = media_blue;
        }

        nova_imagem_blur.write(&linha_blur[0], linha_bytes);
    }

    imagem.close();
    nova_imagem_blur.close();

    std::cout << "Nova imagem com efeito blur criada." << std::endl;
}
