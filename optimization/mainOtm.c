//  Coding....: 1252 (ANSI - Latin I)
//  Algoritimo: main.c
//	Autores...: Rubens Santos Barbosa, Rui G. Moreno
//  Data......: 21/06/17
//  Objetivo..: Aplicar o algoritmo GLCM (Gray Level Cooccurence Matrix)
//				na plataforma ARM9-linux.
//
//	Entrada...: Matriz da imagem, 55 linhas e 55 colunas, eh armazenada na rom do PIC
//				num vetor de 3025 pixels. Imagem usada para coletar as informacoes
//				dos pixels foi einstein.pgm.
//	Saida.....: Motra os resultados do contraste, da homogeneidade, da energia e da correlacao
//              no terminal Tera Term através da comunicação serial.
//	Fonte.....: http://ppger.ifce.edu.br/1o-desafio-ppger-para-a-graduacao/
//
//  Copyright © 2017 Rubens Santos Barbosa and Rui G. Moreno. All rights reserved.

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

//  Como a extensao do arquivo eh PGM (Portable GrayMap) temos que pegar o header/cabecalho da imagem
//  O header dos arquivos PGM sao do seguinte padrao:
//  P5
//  55 55
//  255
//  Os caracteres P5 identificam o formato PGM. A largura da figura (55 pixels) e sua altura (55 pixels).
//  O terceiro inteiro corresponde ao valor maximo da escala de cinza. Para o nosso caso, sempre sera 255 (1 byte).
//  Apos o header, seguem os dados da imagem, um conjunto de inteiros positivos menores que 256, correspondendo as
//  tonalidades dos pixels, dispostos linha a linha.
//
//  O tom de cada pixel eh representado por um inteiro de 8 bits. O branco corresponde a 255, o preto, a 0.
//  Qualquer outro inteiro intermediario corresponde a uma tonalidade de cinza, sendo os mais claros proximos de 255,
//  os mais escuros proximos a 0. O gradiente de tons tem, portanto, 256 possibilidades diferentes.
//  Consideraremos a imagem real aquela que está sendo feita a análise dos pixels vizinhos(matriz de co-ocorrencia).
//  Enquanto que a imagem quantizada é a imagem que é utilizada como parte do processo da análise dos pixels vizinhos.
//  Portanto, como o valor da escala de cinza da imagem real varia entre 0 e 15, nesse caso do PIC18, então fizemos a
//  valueLuminance 16 [0 - 15].

// Contstantes
#define TOTPIXEL 3025       //  Total de pixels da imagem largura x altura -> 55*55 = 3.025 pixels
#define ROW 55
#define COL 55
#define valueLuminance 16
#define LC 4                //  Ordem das matrizes de quantização, co - ocorrencia e normalizao,
                            //  equivale a sqrt(valueLuminance).

//  O calculo da quantizaçao da matriz
//  a quantizacao consiste em amostrar os valores de brilho, intensidade ou luminancia da imagem.
//  Em estatistica amostragem eh parte de um conjunto, conhecido como espaço amostral, da população
//  que eh todo o conjunto. A tecnica de amostragem tem como objetivo dimuir um certo gasto.
//
//  Na quantização, amostramos os valores da luminancia para diminuir o custo computacional do algoritmo.
//  A luminancia que estavamos lendo do header da imagem era igual a 255. Como tivemos uma limitacao na memória
//  da arquitetura PIC18 por conta da alocacao do tamanho do vetor da co-ocorrencia, decidimos limitar os valores
//  dos pixels (faixa de tons) existentes no header da imagem para 15. Explicaremos melhor isso no calculo da
//  co-ocorrencia.
//  A quantização é feita dividindo cada pixel da imagem pelo fator LC. O fator LC é descoberto através da
//  raiz quadrada da luminancia(valueLuminance), ou seja, sqrt(16) = 4 ( tons variando de 0 a 3).
//
//  Armazenamos cada valor do pixel da imagem quantizada para um vetor, de modo estatico, do tamanho da quantidade
//  total de pixels da imagem. Fazemos isso para facilitar nosso proximo calculo que eh a co-ocorrencia.
//  Imagem de 55x55 esta sendo armazenada no vetor "imagem" de 3025 posicoes.
unsigned char imagem[TOTPIXEL] = {

0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,1,1,0,1,0,0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,0,1,0,0,0,1,1,0,
0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,0,1,0,0,0,0,0,0,0,1,1,3,2,0,1,1,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,0,1,1,
0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,1,1,0,0,1,1,
1,1,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,
1,0,1,1,0,1,1,0,0,1,1,0,1,1,0,1,0,0,1,0,0,1,0,0,1,0,1,1,1,1,1,0,0,1,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,0,1,1,0,1,0,0,1,0,0,1,1,1,1,1,0,1,1,1,3,2,1,
1,0,1,1,1,1,1,0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,1,
0,1,1,0,1,1,1,0,1,1,0,1,0,1,1,0,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,3,2,1,0,0,1,0,1,1,1,0,1,1,0,1,1,1,0,0,0,0,0,0,
0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,0,1,0,1,1,0,0,1,1,1,1,1,1,1,1,0,1,0,0,1,1,0,
1,1,1,1,1,0,1,0,1,1,0,0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,0,0,1,0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,3,2,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,0,
1,1,0,0,1,0,0,1,1,1,1,1,1,1,1,0,3,2,1,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,0,0,1,
0,1,1,0,0,1,1,0,1,1,1,1,0,0,1,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,
0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,3,2,1,1,0,1,1,0,1,1,0,1,0,0,1,0,1,1,0,1,1,
1,1,1,1,0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
0,3,2,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,3,2,3,2,1,1,0,1,1,1,1,1,0,1,1,0,1,0,0,1,1,0,1,1,1,3,2,1,0,0,1,0,0,0,
0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,
1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,3,2,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,0,1,1,0,0,0,1,
0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,3,2,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,
1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,3,2,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,0,
1,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,0,1,1,0,1,1,0,1,1,0,1,1,1,1,1,0,1,1,
0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,3,2,1,1,1,1,1,0,1,
1,0,1,1,1,1,1,1,1,1,0,1,0,0,1,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,1,0,0,1,0,0,1,0,1,1,1,1,1,1,0,1,1,1,1,
1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,3,2,1,1,1,
1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,0,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,
1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,
1,1,1,0,1,1,0,1,1,1,1,1,0,3,2,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,1,1,1,
0,0,0,1,0,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,3,2,
1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,3,2,1,1,0,1,1,0,1,0,0,1,0,
1,0,0,0,0,0,1,1,1,1,0,1,1,1,1,1,0,0,1,1,0,0,1,1,0,0,1,0,1,1,0,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,3,2,
1,0,0,1,1,1,0,1,1,0,0,1,1,0,1,1,0,3,2,1,1,0,3,2,1,1,1,1,1,0,1,1,0,3,2,1,1,0,1,0,1,1,1,0,0,0,1,0,0,0,0,0,1,1,1,
0,0,1,0,0,1,0,1,0,0,1,0,1,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,
0,3,2,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,3,2,1,1,1,1,1,0,1,1,0,3,2,1,1,0,1,0,1,1,1,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,
1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,3,2,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,3,2,1,0,1,1,0,0,1,0,0,1,
1,0,1,0,0,0,0,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,3,2,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,
1,1,0,1,1,0,1,1,0,1,1,1,3,2,1,1,1,1,1,0,1,1,0,3,2,1,1,0,1,1,0,1,1,0,1,1,0,3,2,1,1,0,1,1,0,1,1,0,1,1,1,1,0,1,1,
0,0,1,0,1,1,1,0,1,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,1,1,1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,3,2,1,1,0,1,1,0,1,1,1,
1,1,0,1,1,0,1,1,0,3,2,1,0,0,1,1,0,1,1,0,1,1,0,1,1,0,3,2,1,1,0,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,
1,1,0,3,2,1,0,0,1,1,0,1,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,1,1,3,2,1,1,1,1,1,0,0,1,0,1,1,1,0,1,1,1,0,
1,1,0,1,1,0,3,2,1,1,0,1,0,0,1,0,1,1,1,0,1,1,0,1,0,0,1,1,0,3,2,1,1,1,1,1,0,1,1,1,3,2,1,1,0,1,1,1,1,1,0,3,2,3,2,
1,0,0,1,0,0,1,0,0,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,1,1,0,0,1,1,1,0,1,3,2,1,0,0,1,0,0,1,1,1,1,1,0,1,1,1,0,1,1,
0,1,1,0,1,1,0,1,0,1,3,2,0,0,1,1,0,1,1,0,1,1,0,3,2,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,3,2,1,1,1,1,1,0,1,
0,0,1,0,1,1,1,0,1,1,0,0,0,1,0,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,3,2,1,1,0,1,1,1,1,0,1,1,1,1,0,0,1,1,0,1,1,1,3,2,3,
2,3,2,0,1,1,1,0,1,1,0,1,1,0,3,2,1,1,0,3,2,1,1,1,1,1,0,1,1,0,3,2,1,1,0,1,1,1,1,0,0,1,1,0,1,1,1,1,0,1,1,0,1,0,0,
0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,0,1,1,1,0,3,2,1,1,1,0,1,1,0,3,2,1,1,0,1,1,0,1,0,1,1,0,1,1,1,
0,1,1,0,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,3,2,1,1,0,1,1,1,3,2,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,0,0,1,0,0,1,1,
0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,0,1,1,0,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,
3,2,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,3,2,1,1,1,1,1,0,1,0,0,1,0,1,3,2,1,
0,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,1,1,0,3,2,1,1,0,1,1,1,0,3,2,1,1,0,1,1,0,1,1,1,1,
1,0,3,2,1,1,0,3,2,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1,1,3,2,1,1,0,1,1,0,1,1,
0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,3,2,1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,1,0,0,1,1,0,1,
1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1

};

// Global Variable
clock_t start, end;     // variavel para medir tempo de computacao
double cpu_time_used;   // variavel para medir tempo de computacao

int coocorrence[256];
int normalization[256];

int 	i, j, z, numPixel = 0, somatorio = 0;
float   contrast = 0,
		homogeneity = 0,
        energy = 0,
		correlation = 0,
		mi_i = 0, mi_j = 0, r_i = 0, r_j = 0;


int main(){

	for (i = 0; i < 256; i++)
        coocorrence[i] = 0;

    //  Calculo da co-ocorrencia
	//  A co-ocorrencia considera a relacao Q entre dois pixels: pixel referencia e pixel vizinho.
	//  essa matriz eh uma representação dos pixels da imagem, tomadas dois a dois, ela será uma matriz
	//  N x N, onde N eh o numero de cinza da imagem
	//  Ex.: Q = (0, 1), significa a quantidade de zeros (pixel 0) são vizinhos de uns (pixel 1).
	//  Estatística de segunda ordem como a GLCM se preocupa com o relacionamento espacial entre
	//  dois pixels ou a co-ocorrencia entre eles. Assim, uma imagem que contem a mesma quantidade
	//  de diferentes níveis de cinzas que outra diferenciando-se, somente, em termos de posicionamento
	//  dos mesmos, irá apresentar um mesmo histograma.
	//
	//  Como estamos trabalhando nesse momento com um vetor com tamanho total de pixels e nao mais a matriz da imagem
	//  fazemos o uso desse if ((i + 1) % (row) != 0) para sabermos onde será o fim da linha e a partir dai passar para a proxima linha
	//  e a partir desse if fazemos o contador de pixels da vizinha onde pegamos sempre i e i+1 e multiplicamos por LC = 4 para voltar
	//  ao valor original do tom de cinza da imagem.
    //  Observe que nessa parte do codigo estamos voltando os pixels da imagem para 16 tons (0-15).
    //  Por exemplo, considere i = 0, imagem[0] = 3 e imagem[0 + 1] = 3. Ao realizarmos a conta dentro do colchetes[]
    //  da co-ocorrencia, notamos que: 3 * 4 + 3 = 15. Essa eh a posição que sera armazenada a quantidade de pixels
    //  vizinhos do tom 15. Por conta disso, tivemos que quantizar a matriz duas vezes, ou seja, trabalhar com valores
    //  de 0-3 pixels para voltar aos valores de 0-15 pixels já que nesse trecho de código ele faz o processo inverso
    //  da quantizacao.
    //  Portanto, lembre-se que a imagem adicionada na rom não eh a nossa imagem "real" por conta do processo inverso
    //  que eh feito neste trecho de codigo. Nossa imagem real possui tons de 0-15. O que esta na rom eh a imagem
    //  quantizada da real.
    //
    start = clock();
    for(z = 0; z < 1000; z++)
    {
        for (i = 0; i < TOTPIXEL; i++)
        {
            if ((i + 1) % (ROW) != 0)
            {
                coocorrence[imagem[i] * LC + imagem[i+1]] += 1;
            }
        }
     
         //  Normalizacao da co-ocorrencia
         //  A normalizacao das imagens esta relacionada com a remocao da influencia da luminosidade sobre as mesmas.
         //  Normalizacao eh o processo de transformar uma matriz de co-ocorrencia em uma aproximacao de uma tabela de probalidades
         //  Isso quer dizer que os valores do calculo da normalizacao estará entre zero e um.
         //  Para isso é feito o somatorio de todos os elementos (pixels) da matriz de co-ocorrencia
         //  depois dividindo cada elemento da co-ocorrencia com o valor do somatorio para entao obtermos a normalizacao
        for (i = 0; i < valueLuminance; i++)
            somatorio += coocorrence[i];

        for (i = 0; i < valueLuminance; i++)
            normalization[i] = ( (float)coocorrence[i] / (float)somatorio ) * 10000;

         //  calculo do contraste ou variancia
         //  Medida da quantidade de variacao local de niveis de cinza.
         //  Afirmam que se este valor for pequeno, os niveis de cinza da imagem analisada sao todos proximos da media
        for (i = 0; i < valueLuminance; i++)
            contrast += ( ((float)normalization[i]/10000) * pow(((i / LC) - (i % LC)), 2));

        //  calculo da energia: avalia a uniformidade textural.
        for (i = 0; i < valueLuminance; i++)
            energy += pow( ((float)normalization[i]/10000), 2);

        //  calculo da homogeneidade: mede a homogeneidade da imagem.
        for (i = 0; i < valueLuminance; i++)
            homogeneity += ( ((float)normalization[i]/10000) / (float)(1 + fabs( (i / LC) - (i % LC ) )) );


        //  Calculo da correlacao: mede a dependência linear de um nível de cinza em relação aos vizinhos,
        //  ou seja, representa uma ideia de linearidade de dependencia de tons de cinza
        //  sera utilizada o calculo da variancia e da media GLCM na vertical(j) e na horizontal (i) para
        //  o calculo da correlacao.

        // calculo da media GLCM
        for (i = 0; i < valueLuminance; i++){
            mi_i += ((i / (int)sqrt(valueLuminance)) * ((float)normalization[i]/10000) );
            mi_j += ((i % (int)sqrt(valueLuminance)) * ((float)normalization[i]) );
        }
        // calculo da variancia GLCM
        for (i = 0; i < valueLuminance; i++){
            r_i += ((float)normalization[i]/10000) * pow(((i / (int)sqrt(valueLuminance)) - mi_i), 2);
            r_j += ((float)normalization[i]/10000) * pow(((i % (int)sqrt(valueLuminance)) - mi_j), 2);
        }

        //  Tirando a raiz quadrada dos elementos r_i e r_j para aplicar na formula de correlacao
        r_i = sqrt(r_i);
        r_j = sqrt(r_j);
        //  Formula de correlacao
        for (i = 0; i < valueLuminance; i++)
            correlation += (
                             ( ( (float)(i / (int)sqrt(valueLuminance) ) ) - mi_i) *
                             ( ( (float)(i % (int)sqrt(valueLuminance) ) ) - mi_j) *
                             ( (float)normalization[i]/ 10000 )
                           )
                           / (r_i * r_j);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nTempo de execucao: %.50f\n", cpu_time_used / 1000);

	return 0;
}
