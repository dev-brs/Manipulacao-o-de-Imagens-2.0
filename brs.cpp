#include <iostream>
#include <fstream>
#include <vector>

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
        CAB cabecalho;
        DADOS dados;
    public:
        // Construtor
        Imagem(const string &nome_da_imagem)
        {
            nome_da_img = nome_da_imagem;
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

        // Amostra de dados
        void imprimirDados(const string &nome_da_img)
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
            ofstream nova_imagem(nome_nova_imagem, ios::binary);

            if (!nova_imagem.is_open())
            {
                cerr << "Erro: Não foi possível criar a nova imagem." << endl;
                return;
            }

            nova_imagem.write(reinterpret_cast<char *>(&cabecalho), 14);
            nova_imagem.write(reinterpret_cast<char *>(&dados), 40);

            ifstream imagem("C:/Users/berna/Desktop/lg.bmp", ios::binary); // Adicione esta linha para reabrir o arquivo de origem
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

            cout << "Nova imagem criada: " << nome_nova_imagem << endl;
        }

        // Manipulação da imagem
        void pretoBranco()
        {
            ofstream nova_imagem_pb("C:/Users/berna/Desktop/nova_imagem_pb.bmp", ios::binary);

            if (!nova_imagem_pb.is_open())
            {
                cerr << "Erro: Não foi possível criar a nova imagem em preto e branco." << endl;
                return;
            }

            nova_imagem_pb.write(reinterpret_cast<char*>(&cabecalho), 14);
            nova_imagem_pb.write(reinterpret_cast<char*>(&dados), 40);

            ifstream imagem("C:/Users/berna/Desktop/lg.bmp", ios::binary); // Adicione esta linha para reabrir o arquivo de origem
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
};

int main()
{
    string nome_da_imagem = "C:/Users/berna/Desktop/lg.bmp";
    string nome_nova_imagem = "C:/Users/berna/Desktop/nova_imagem.bmp";

    Imagem minhaImagem(nome_da_imagem);

    minhaImagem.imprimirDados(nome_da_imagem);

    minhaImagem.pretoBranco();

    return 0;
}