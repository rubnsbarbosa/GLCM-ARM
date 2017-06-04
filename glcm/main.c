//
//  main.c
//  GLCM (Gray Level Cooccurence Matrix) Algorithm
//
//  Created by Rubens Santos Barbosa on 01/06/17.
//  Copyright © 2017 Rubens Santos Barbosa. All rights reserved.
// 
//  No terminal siga os seguintes comandos
//  $ gcc main.c -lm -o main.o
//  $ ./main lena.pgm
//  
//  Para uso desse algoritmo a imagem devera ser quadrada por exemplo: 512x512 pixels
//  Porque a matriz, obrigatoriamente, eh quadrada
//


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//  Ordem das matrizes de quantizaçao, co - ocorrencia e normalizao; equivale a sqrt(luminance)
#define LC 16
#define TOTPIXEL 262144

//  Prototipos das funcoes
unsigned char **matrixAlloc(int row, int col);
void matrixDealloc(unsigned char **matrix, int row, int col);

int main(int argc, char **argv)
{
	//  Nome do arquivo digitado na linha de comando
	FILE *file = fopen(*(argv + 1), "rb");


	if (!file)
	{
		printf("File not found!\n");
		exit(1);
	}
	else
	{
		//  Pra entrar nesse else o nome da imagem tem que ser digitado corretamente na linha de comando
		unsigned char buffer = 0;

		//  Como a extensão do arquivo é PGM (Portable GrayMap) temos que pegar o header/cabeçalho da imagem
		//  O header dos arquivos PGM são do seguinte padrão:
		//  P5
		//  512 512
		//  255
		//  Os caracteres P5 identificam o formato PGM. A largura da figura (55 pixels) e sua altura (55 pixels). O terceiro inteiro
		//  corresponde ao valor máximo da escala de cinza. Para o nosso caso, sempre será 255 (1 byte). Após o header, seguem os dados
		//  da imagem, um conjunto de inteiros positivos menores que 256, correspondendo às tonalidades dos pixels, dispostos linha a
		//  linha.
		//
		//  O tom de cada pixel é representado por um inteiro de 8 bits. O branco corresponde a 255, o preto, a 0.
		//  Qualquer outro inteirointermediário corresponde a uma tonalidade de cinza, sendo os mais claros próximos de 255,
		//  os mais escuros próximos a 0. O gradiente de tons tem, portanto, 256 possibilidades diferentes.

		//  Descarta o header desnecessario do arquivo pgm
		while(getc(file) != '\n');
      while(getc(file) == '#');
    	while(getc(file) != '\n');

		int row  = 0;
		int col = 0;
		int valueLuminance = 0;

		//  Leitura do header necessario da imagem como altura, largura e valor maximo de cor (luminancia).
		fscanf(file, "%d", &row);
		fscanf(file, "%d", &col);
		fscanf(file, "%d", &valueLuminance);

		//  Alocacao da matriz de bytes da imagem
		unsigned char **matrix = matrixAlloc(row, col);
		getc(file);


		//  Leitura da imagem PGM
		//  Passando cada pixel da imagem para a matriz
		int i, j;
		for (i = 0; i < row; i++)
		{
			for(j = 0; j < col; j++)
			{
				buffer = getc(file);
				matrix[i][j] = buffer;
			}
		}

	
		//  Calculo da quantizacao da matriz
		for (i = 0; i < row; i++)
			for (j = 0; j < col; j++)
				matrix[i][j] = matrix[i][j]/LC;


		//  Armazenando os valores dos pixels da matriz quantizada[][] em um array[]	
		int pixelArray[TOTPIXEL], numPixel = 0;
		for (i = 0; i < row; i++)
			for (j = 0; j < col; j++) 
			{
				pixelArray[numPixel] = matrix[i][j];
				numPixel++;
			}	
	

		//  calculo da co-ocorrencia
		int coocorrence[TOTPIXEL];
		for (i = 0; i < TOTPIXEL; i++)
		{
			if ((i + 1) % (row) != 0)
			{
				coocorrence[pixelArray[i] * LC + pixelArray[i+1]] += 1;  	
			}
		}


		//  normalizaçao da matriz de co-ocorrencia
		unsigned int somatorio = 0;
		float normalization[valueLuminance];
		for (i = 0; i < valueLuminance + 1; i++)
			somatorio += coocorrence[i];
	
		for (i = 0; i < valueLuminance + 1; i++)
			normalization[i] = (float)coocorrence[i]/(float)somatorio;


		//  calculo do contraste
		float contrast = 0;
		for (i = 0; i < valueLuminance + 1; i++)
			contrast += (normalization[i] * pow(((i / LC) - (i % LC)), 2));


		//calculo da energia
		float energy = 0;
		for (i = 0; i < valueLuminance + 1; i++)
			energy += pow(normalization[i], 2);


		//  calculo da homogeneidade
		float homogeneity = 0;
		for (i = 0; i < valueLuminance + 1; i++)
			homogeneity += (normalization[i] / (float)(1 + abs((i / LC) - (i % LC ))));


		//calculo da correlacao
		float correlation = 0, mi_i = 0, mi_j = 0, r_i = 0, r_j = 0;

		for (i = 0; i < valueLuminance + 1; i++){
			mi_i += ((i / (int)sqrt(valueLuminance + 1)) * normalization[i]);
			mi_j += ((i % (int)sqrt(valueLuminance + 1)) * normalization[i]);
		}
	
		for (i = 0; i < valueLuminance + 1; i++){
			r_i += normalization[i] * pow(((i / (int)sqrt(valueLuminance + 1)) - mi_i), 2);
			r_j += normalization[i] * pow(((i % (int)sqrt(valueLuminance + 1)) - mi_j), 2);
		}
	
		//  Tirando a raiz quadrada dos elementos r_i e r_j para aplicar na formula de correlacao
		r_i = sqrt(r_i);
		r_j = sqrt(r_j);
		//  Formula de correlacao
		for (i = 0; i < valueLuminance + 1; i++)
			correlation += ((((float)(i / (int)sqrt(valueLuminance + 1))) - mi_i) * (((float)(i % (int)sqrt(valueLuminance + 1))) - mi_j) * normalization[i]) / (r_i * r_j);


		//  imprimindo os valores do contraste, energia, homogeneidade e correlacao
		printf("Contrast: %.4f\n", contrast);
		printf("Energy: %.4f\n", energy);
		printf("Homogeneity: %.4f\n", homogeneity);
		printf("Correlation: %.4f\n", correlation);

		fclose(file);
		matrixDealloc(matrix, row, row);
		
	}

	if (argc != 2){
		printf("O nome do arquivo nao foi passado na linha de comando!\n");
		exit(1);
	}

	return 0;
}

unsigned char **matrixAlloc(int row, int col)
{
    /* Se houver memoria disponivel, esta funcao, aloca uma matriz com L linhas (row) e C colunas (col)
    e devolve o endereço-base da matriz; Caso contrário, devolve um ponteiro nulo */

    unsigned char **matrix = malloc(sizeof(unsigned char*) * row);

    if(matrix == NULL) //  Memória insuficiente
    {
        return NULL; //  Ponteiro NULL é um ponteiro zero (nao aponta pra nada)
    }
    unsigned int i, j;
    for(i = 0; i < row; i++)
    {
        matrix[i] = malloc(sizeof(unsigned char*) * col);
        if(matrix[i] == NULL)
        {
            for(j = 0; j < i; j++)
            {
                free(matrix[j]);
            }
        free(matrix);
        return NULL;
        }
    }
    return matrix;
}

void matrixDealloc(unsigned char **matrix, int width, int height)
{
	 //  Desaloca uma matriz dinamicamente alocada com malloc
    if(matrix != NULL)
    {
        int i;
        for(i = 0; i < height; i++)
        {
            if (matrix[i] != NULL)
            {
                free(matrix[i]);
            }
        }
        free(matrix);
    }
}

