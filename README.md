# GLCM

Método estatístico de examinar a textura que considera a relação espacial de pixels. As características de textura são calculadas a partir da distribuição estatística das combinações observadas de intensidades em posições especificadas em relação à outra na imagem. De acordo com o número de pontos de intensidade (pixels) em cada combinação.  

*Gray Level Coocurrence Matrix* (GLCM) é uma forma de extração características de textura estatística. Um GLCM é uma matriz onde o número de linhas e colunas é igual ao número de níveis de cinza, G, na imagem. A GLCM é um algoritmo bastante utilizado em processamento digital de imagens (PDI) para avaliar padrões de textura presentes em uma imagem em níveis de cinza.  

Neste algoritmo consiste em extrair quatro atributos da matriz, a saber Contraste, Correlação, Energia e Homogeneidade. Não foi usado bibliotecas de processamento de imagens como *OpenCV* ou equivalente.

## Matriz = Imagem
A imagem deverá ser quadrada por exemplo: 512x512 pixels, pois no método GLCM abrigatoriamente utiliza-se matriz quadrada. Segue um exemplo de uma imagem quadrada no formato *.PGM (Portable GrayMap)*  

![images](https://cloud.githubusercontent.com/assets/17646546/26767592/db40bb6a-4977-11e7-9728-0828c26a291b.jpg)

## O Algoritmo deve
> 1) ler o arquivo da imagem 
> 2) armazenar a imagem em uma matriz
> 3) calcular os elementos da matriz GLCM na direção horizontal à direita com deslocamento unitário considerando 16 níveis de cinza
> 4) calcular 4 atributos da GLCM: contrast, energy, homogeneity and correlation.  

### Contraste 
É uma estimativa das variações locais ao quadrado dos níveis de cinza entre pares de pixels. Esta medida é também chamada de soma do quadrado da variância. Valores na diagonal da Matriz de Co-ocorrência representam pouco contraste, aumentando quando a distância da diagonal aumenta.

### Energia
Retorna a soma de elementos quadrados no GLCM.  

### Homogeneidade
![homogeneidade](https://cloud.githubusercontent.com/assets/17646546/26767174/b280b170-4973-11e7-8de5-4cbd31e311b8.png)  
Os valores dos pesos da homogeneidade são inversamente proporcionais aos pesos do contraste, com pesos diminuindo exponencialmente quanto mais distantes da diagonal.  homogeneidade resulta em grandes valores para níveis de cinza similares. Se os pesos diminuem distantes da diagonal, o resultado será maior para imagens com pouco contraste.

### Correlação
![correlacao](https://cloud.githubusercontent.com/assets/17646546/26767184/d3ca7352-4973-11e7-8f4c-c3cf6f4a3875.png)
O coeficiente de correlação utilizado em estatística é uma medida do grau de associação linear (negativa ou positiva) entre duas variáveis quantitativas. A correlação em textura mede a dependência linear de pixels em relação a sua vizinhança.
