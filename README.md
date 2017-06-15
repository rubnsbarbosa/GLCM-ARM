# GLCM

*Gray Level Coocurrence Matrix* (GLCM) é um método estatístico de extração características de textura que considera a relação espacial de pixels. As características de textura são calculadas a partir da distribuição estatística das combinações observadas de intensidades em posições especificadas em relação à outra na imagem de acordo com o número de pontos de intensidade (pixels) em cada combinação. A GLCM é um algoritmo bastante utilizado em processamento digital de imagens (PDI) para avaliar padrões de textura presentes em uma imagem em níveis de cinza.  

Neste algoritmo consiste em extrair quatro atributos da matriz, a saber Contraste, Correlação, Energia e Homogeneidade. Não foi usado bibliotecas de processamento de imagens como *OpenCV* ou equivalente.

## Matriz = Imagem
A imagem deverá ser quadrada por exemplo: 512x512 pixels, pois no método GLCM abrigatoriamente utiliza-se matriz quadrada. O formato da imagem, para o algoritmo, foi o *.PGM (Portable GrayMap)*  

![einstein](https://user-images.githubusercontent.com/17646546/27186139-cf19b10e-51bd-11e7-878e-c14a9edb6ce5.GIF)

## O Algoritmo deve
> 1) ler o arquivo da imagem 
> 2) armazenar a imagem em uma matriz
> 3) calcular os elementos da matriz GLCM na direção horizontal à direita com deslocamento unitário considerando 16 níveis de cinza
> 4) calcular 4 atributos da GLCM: contrast, energy, homogeneity and correlation.  

### Contraste
![contraste](https://user-images.githubusercontent.com/17646546/27185197-f833c848-51ba-11e7-90ee-7a83969a2b2c.png)

### Energia
![energia](https://user-images.githubusercontent.com/17646546/27185241-21795f56-51bb-11e7-8fd4-05426089c1aa.png) 

### Homogeneidade
![homogeneidade](https://user-images.githubusercontent.com/17646546/27185271-36ecb1bc-51bb-11e7-8423-5a93a7131ecf.png)

### Correlação
![correlacao](https://user-images.githubusercontent.com/17646546/27185290-438f735a-51bb-11e7-84ea-9983535c046b.png)
