#include <iostream>
#include <fstream>
#include <vector>
#include <direct.h>

using namespace std;

#define DESLOCAMENTO 3

struct CAB
{
    unsigned char fType[2];    // tipo do arquivo
    unsigned int fSize;        // Tamanho do arquivo (cabeçalho)
    unsigned short mR1;        // espaço reservado
    unsigned short mR2;        // espaço reservado
    unsigned int byteshift;    // especifica o DESLOCAMENTO em bytes do início
};

struct DADOS
{
    unsigned int tamanho_cab; // tamanho do cabeçalho
    unsigned int largura;     // largura da imagem em pixels
    unsigned int altura;      // altura da imagem em pixels
    unsigned short planos;    // quantidade de planos
    unsigned short bitcount;  // bits/pixel
    unsigned int bCompress;   // compressão utilizada
    unsigned long int tamanho_img_byte; // tamanho da imagem em byte
    unsigned int resXPPMeter; // resolução horizontal pixel/metro
    unsigned int resYPPMeter; // resolução vertical pixel/metro
    unsigned int numClrUsed;  // número de cores na imagem
    unsigned int numClrImpor; // número de cores importantes
};

class Imagem
{
    private:
        string nome_da_img;
        string diretorio_saida;
        string caminho_do_arquivo; // Novo membro para armazenar o caminho do arquivo
        CAB cabecalho;
        DADOS dados;

        // Diretório Default
        string extrairDiretorio(const string &caminho)
        {
            size_t pos = caminho.find_last_of("/\\");
            if (pos != string::npos)
            {
                return caminho.substr(0, pos + 1);
            }
            return "";
        }
        // Função para verificar se um diretório existe
        bool diretorioExiste(const string &diretorio)
        {
            ifstream dir(diretorio);
            return dir.good();
        }
        // Função para criar um diretório
        bool criarDiretorio(const string &diretorio)
        {
            return mkdir(diretorio.c_str()) == 0;
        }
    public:
        // Construtor
        Imagem(const string &nome_da_imagem)
        {
            nome_da_img = nome_da_imagem;
            diretorio_saida = extrairDiretorio(nome_da_imagem);
            caminho_do_arquivo = nome_da_imagem; // Inicialize o caminho do arquivo aqui
            __init__(nome_da_imagem);
        }
        // Inicialização da instância
        void __init__(const string &nome_da_imagem)
        {
            ifstream imagem(nome_da_imagem, ios::binary);

            if (!imagem.is_open())
            {
                cerr << "Erro: A imagem de origem não pode ser aberta." << endl;
                return;
            }

            imagem.read(reinterpret_cast<char *>(&cabecalho), 14);
            imagem.read(reinterpret_cast<char *>(&dados), 40);

            imagem.close();
        }
        // Inicialização de um diretório de saída especifico, caso não seja utilizado, será utilizado o mesmo diretório da imagem original
        void diretorioSaida(const string &caminho_de_saida_escolhido)
        {
            diretorio_saida = caminho_de_saida_escolhido + '/';

            // Verificar se o diretório de saída existe e criar se necessário
            if (!diretorioExiste(diretorio_saida))
            {
                if (criarDiretorio(diretorio_saida))
                {
                    cout << "Diretório de saída criado: " << diretorio_saida << endl;
                }
                else
                {
                    cerr << "Erro: Não foi possível criar o diretório de saída." << endl;
                    diretorio_saida = ""; // Limpar o diretório de saída se houver erro
                }
            }
        }
        // Amostra de dados
        void imprimirDados()
        {
            cout << "Dados do Cabecalho:" << endl;
            cout << "Tipo do arquivo: " << cabecalho.fType[0] << cabecalho.fType[1] << endl;
            cout << "\nDados da Imagem:" << endl;
            cout << "Nome da imagem: " << nome_da_img << endl;
            cout << "Tamanho da imagem (bytes): " << dados.tamanho_img_byte << endl;
            cout << "Largura da imagem: " << dados.largura << endl;
            cout << "Altura da imagem: " << dados.altura << endl;
            cout << "Profundidade: " << dados.bitcount << endl;
        }

        // Copiar imagem
        void copiar(const string &nome_nova_imagem)
        {
            ofstream nova_imagem(diretorio_saida + nome_nova_imagem, ios::binary);

            if (!nova_imagem.is_open())
            {
                cerr << "Erro: Não foi possível criar a nova imagem." << endl;
                return;
            }

            nova_imagem.write(reinterpret_cast<char *>(&cabecalho), 14);
            nova_imagem.write(reinterpret_cast<char *>(&dados), 40);

            ifstream imagem(caminho_do_arquivo, ios::binary); // Use o caminho do arquivo salvo
            imagem.seekg(cabecalho.byteshift, ios::beg); // Pule para a posição correta dos pixels

            // Agora, copiar os pixels
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

            cout << "Nova imagem criada: " << diretorio_saida + nome_nova_imagem << endl;
        }

        // Manipulação da imagem
        void pretoBranco()
        {
            ofstream nova_imagem_pb(diretorio_saida + "nova_imagem_pb.bmp", ios::binary); 

            if (!nova_imagem_pb.is_open())
            {
                cerr << "Erro: Não foi possível criar a nova imagem em preto e branco." << endl;
                return;
            }

            nova_imagem_pb.write(reinterpret_cast<char*>(&cabecalho), 14);
            nova_imagem_pb.write(reinterpret_cast<char*>(&dados), 40);

            ifstream imagem(caminho_do_arquivo, ios::binary);
            imagem.seekg(cabecalho.byteshift, ios::beg); // Pule para a posição correta dos pixels

            int linha_bytes = dados.largura * 3;
            vector<char> linha(linha_bytes);

            for (unsigned int i = 0; i < dados.altura; i++)
            {
                imagem.read(&linha[0], linha_bytes);
                for (int j = 0; j < linha_bytes; j += 3)
                {
                    unsigned char blue = linha[j];
                    unsigned char green = linha[j + 1];
                    unsigned char red = linha[j + 2];

                    unsigned char gray = ((red + green + blue) / 3);

                    linha[j] = linha[j+1] =  linha[j+2] = gray;
                }
                nova_imagem_pb.write(&linha[0], linha_bytes);
            }

            imagem.close();
            nova_imagem_pb.close();

            cout << "Nova imagem em preto e branco criada." << endl;
        }

        void blur()
        {
            ofstream nova_imagem_blur(diretorio_saida + "nova_imagem_blur.bmp", ios::binary);

            if (!nova_imagem_blur.is_open())
            {
                cerr << "Erro: Não foi possível criar a nova imagem com efeito blur." << endl;
                return;
            }

            nova_imagem_blur.write(reinterpret_cast<char*>(&cabecalho), 14);
            nova_imagem_blur.write(reinterpret_cast<char*>(&dados), 40);

            ifstream imagem(caminho_do_arquivo, ios::binary);
            imagem.seekg(cabecalho.byteshift, ios::beg); // Pule para a posição correta dos pixels

            int linha_bytes = dados.largura * 3;
            vector<char> linha(linha_bytes);
            vector<char> linha_blur(linha_bytes);

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
                        media_red += (linha[j - 3] + linha[j] + linha[j + 3]) / 3; // pixels na mesma linha
                        media_green += (linha[j - 2] + linha[j + 1] + linha[j + 4]) / 3; // pixels na mesma linha
                        media_blue += (linha[j - 1] + linha[j + 2] + linha[j + 5]) / 3; // pixels na mesma linha

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

            cout << "Nova imagem com efeito blur criada." << endl;
        }

};

int main()
{
    Imagem minhaImagem("C:/Users/berna/Desktop/livro.bmp");

    minhaImagem.diretorioSaida("C:/Users/berna/Desktop/e agora");
    minhaImagem.imprimirDados();
    minhaImagem.copiar("nova_imagem.bmp");
    minhaImagem.blur();
    
    return 0;
}