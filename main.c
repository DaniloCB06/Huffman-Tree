#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HeapNodo {
    char caracter;
    unsigned freq;
    struct HeapNodo* left;
    struct HeapNodo* right;
} HeapNodo;

typedef struct Heap {
    unsigned size;
    struct HeapNodo** array;
} Heap;

HeapNodo* criaNodo(char caracter, unsigned freq) {
    HeapNodo* aux = (HeapNodo*)malloc(sizeof(HeapNodo));
    aux->left = NULL;
    aux->right = NULL;
    aux->caracter = caracter;
    aux->freq = freq;
    return aux;
}

void printBinary(unsigned int num) {
    // Assumimos que o inteiro tem 32 bits
    int numBits = sizeof(unsigned int) * 8;
    int leadingZero = 1; // Para evitar a impressão de zeros à esquerda

    for (int i = numBits - 1; i >= 0; i--) {
        int bit = (num >> i) & 1;
        if (bit) leadingZero = 0; // Encontramos o primeiro 1, então não ignorar mais zeros

        if (!leadingZero || i == 0) { // Imprime zero mais significativo
            printf("%d", bit);
        }
    }
    printf("\n");
}

void verificaPrioridade(Heap* heap, int idx) {
    int menor = idx;
    int esq = 2 * idx + 1;
    int dir = 2 * idx + 2;

    if (esq < heap->size &&
        heap->array[esq]->freq <
        heap->array[menor]
        ->freq) {
        menor = esq;
    }
    if (dir < heap->size && heap->array[dir]->freq < heap->array[menor]->freq) {
        menor = dir;
    }

    if (menor != idx) { // se nenhum filho for válido então troca o pai de nó e
        // verifica a sua prioridade
        HeapNodo* aux = heap->array[menor];
        heap->array[menor] = heap->array[idx];
        heap->array[idx] = aux;
        verificaPrioridade(heap, menor);
    }
}

HeapNodo* constroiHuffman(char array_caracteres[], int array_freq[],
    int tamanho_freq) {

    HeapNodo* nodoHuffman;
    HeapNodo* auxHuffman;
    HeapNodo* esq;
    HeapNodo* dir;
    HeapNodo* pai;

    Heap* heap = (Heap*)malloc(sizeof(Heap));
    heap->size = tamanho_freq;
    heap->array = (HeapNodo**)malloc(tamanho_freq * sizeof(HeapNodo*));

    for (int i = 0; i < tamanho_freq; ++i) {
        HeapNodo* aux = NULL;
        aux = criaNodo(array_caracteres[i], array_freq[i]);
        heap->array[i] = aux;
    }

    for (int i = (heap->size - 1) / 2; i >= 0;
        --i) { // constroi um minHeap para HuffMan de baixo para cima (de trás
        // para frente)
        verificaPrioridade(heap, i);
    }

    while (heap->size > 1) {
        HeapNodo* auxEsq;
        HeapNodo* auxDir;

        auxEsq = heap->array[0];
        heap->array[0] = heap->array[heap->size - 1];
        heap->size =  heap->size - 1;
        verificaPrioridade(heap, 0);
        esq = auxEsq;

        auxDir = heap->array[0];
        heap->array[0] = heap->array[heap->size - 1];
        heap->size = heap->size - 1;
        verificaPrioridade(heap, 0);
        dir = auxDir;

        pai = criaNodo(
            '@', esq->freq +
            dir->freq); // passa qualquer caracter que não é esperado no
        // arquivo Txt para diferenciar de qualquer filho
        pai->left = esq;
        pai->right = dir;

        heap->size++; // abre espaço no heap para a inserção
        int novoIndice =
            heap->size -
            1; // O pai sera colocado na última posição do heap. Pois corresponde ao
        // primeiro espaço vago após o último elemento existente no heap.

        for (;
            novoIndice && pai->freq < heap->array[(novoIndice - 1) / 2]->freq;
            novoIndice =
            (novoIndice - 1) / 2) {
            heap->array[novoIndice] = heap->array[(novoIndice - 1) / 2];
        }
        heap->array[novoIndice] = pai;
    }

    auxHuffman = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    heap->size = heap->size - 1;
    verificaPrioridade(heap, 0);
    nodoHuffman = auxHuffman;

    return nodoHuffman;
}

void liberaArvoreHuffman(HeapNodo* raiz) {
    if (raiz == NULL) {
        return;
    }

    liberaArvoreHuffman(raiz->left);
    liberaArvoreHuffman(raiz->right);

    free(raiz);
}

void criaTabelaHuffman(HeapNodo* raiz, int aux[], int pai, unsigned int codigoBinarioInt[], int tamanhoBits[]) {
    if (raiz->left != NULL) {
        aux[pai] = 0;
        criaTabelaHuffman(raiz->left, aux, pai + 1, codigoBinarioInt, tamanhoBits);
    }

    if (raiz->right != NULL) {
        aux[pai] = 1;
        criaTabelaHuffman(raiz->right, aux, pai + 1, codigoBinarioInt, tamanhoBits);
    }

    if (raiz->left == NULL && raiz->right == NULL) {
        unsigned int caminho = 0;
        for (int i = 0; i < pai; i++) {
            caminho = (caminho << 1) | aux[i];
        }
        codigoBinarioInt[(unsigned char)raiz->caracter] = caminho;
        tamanhoBits[(unsigned char)raiz->caracter] = pai; // Salva quantos bits foram percorridos
    }
}

void printTabelaHuffman(unsigned int codigoBinario[], int tamanhoBits[256]) {
    for (int i = 0; i < 256; i++) {
        if (tamanhoBits[i] > 0) {
            printf("Caracter: %c - Codigo Binario:", i);
            printBinary(codigoBinario[(unsigned char)i]);
            printf("Tamanho em Bits: %d\n", tamanhoBits[i]);
        }
    }
}

void descompacta(HeapNodo* raiz, FILE* entrada, FILE* saida) {
    HeapNodo* atual = raiz;
    unsigned char byte;
    int bit;


    while (fread(&byte, sizeof(unsigned char), 1, entrada) == 1) {
        for (int i = 7; i >= 0; i--) {
            bit = (byte >> i) & 1;
            if (bit == 0) {
                atual = atual->left;
            }
            else {
                atual = atual->right;
            }
            if (atual->left == NULL && atual->right == NULL) {
                if (atual->caracter == '!') {
                    fclose(entrada);
                    fclose(saida);
                    printf("Arquivo descompactado com sucesso\n");
                    return;
                }
                fputc(atual->caracter, saida);
                atual = raiz;
            }
        }
    }
    return;

}

void compacta(int contaCaracteres, int* codigoEmBinario, int tamanhoBits[], char caracteres[], FILE* file2) {

    unsigned char buffer = 0;
    int bitsNoBuffer = 0;
    unsigned char bit;
    for (int i = 0; i < contaCaracteres; i++) {
        unsigned char c = (unsigned char)caracteres[i];
        unsigned int codigo = codigoEmBinario[c];
        int tamanho = tamanhoBits[c]; 
        for (int j = tamanho - 1; j >= 0; j--) {

            bit = codigo >> j;
            bit &= 1;

            buffer <<= 1;
            buffer |= bit;

            bitsNoBuffer++;

            if (bitsNoBuffer == 8) {
                fwrite(&buffer, sizeof(unsigned char), 1, file2);
                buffer = 0;
                bitsNoBuffer = 0;
            }
        }
    }
    unsigned int fim = codigoEmBinario['!'];
    int tamanhoFim = tamanhoBits['!'];

    for (int j = tamanhoFim - 1; j >= 0; j--) {
        bit = fim >> j;
        bit &= 1;

        buffer <<=  1;
        buffer |= bit;
        bitsNoBuffer++;
        if (bitsNoBuffer == 8) {
            fwrite(&buffer, sizeof(unsigned char), 1, file2);
            buffer = 0;
            bitsNoBuffer = 0;
        }
    }

    if (bitsNoBuffer > 0) {
        buffer <<= 8 - bitsNoBuffer;
        fwrite(&buffer, sizeof(unsigned char), 1, file2);
    }

    printf("Arquivo compactado com sucesso \n");
    return;

}


int main(void) {
    FILE* file = fopen("texto.txt", "r");
    FILE* file2 = fopen("textoC.dat", "wb");
    int contaCaracteres = 0;
    int tamanhoTotal = 0;
    if (file == NULL) {
        printf("O arquivo não foi encontrado ou esta vazio\n");
        exit(1);
    }

    if (file2 == NULL) {
        printf("Erro.\n");
        exit(1);
    }

    int frequencia_e_caracteres[256] = {
        0 }; // a tabela de frequencia e caracteres inicia todos como zero
    int caracter;

    while ((caracter = fgetc(file)) != EOF) {
        unsigned char index = (unsigned char)caracter;
        frequencia_e_caracteres[index]++;
        contaCaracteres++;
    }
    fclose(file);

    frequencia_e_caracteres['!']++; // representação do final do arquivo

    char array_caracter[256];    // salva em um array os caracteres
    int array_freqCaracter[256]; // salva em um array as respectivas frequencias
    // de cada caracter
    int frequencia = 0; // inicia sua frequencia como 0 e ao final do for obtem o
    // sua frequencia total
    for (int i = 0; i < 256; ++i) {
        if (frequencia_e_caracteres[i] > 0) {
            array_caracter[frequencia] = i;
            array_freqCaracter[frequencia] = frequencia_e_caracteres[i];
            frequencia++;
        }
    }

    HeapNodo* arvore_huffMan =
        constroiHuffman(array_caracter, array_freqCaracter, frequencia);

    printf("\nCodigos de Huffman\n\n");

    unsigned int codigoEmBinario[256]; // inicia espaços nulos de tamanho 100 para
    // salvar os codigos em binario
    int tamanhoBits[256] = { 0 };     // inicia todos como zero
    int aux[100];                   // auxiliar para a função recursiva

    // tabela de caracter, código binario e tamanho do binario em bits
    criaTabelaHuffman(arvore_huffMan, aux, 0, codigoEmBinario, tamanhoBits); // salva os codigos binarios e seus respectivos tamanhos

    printTabelaHuffman(codigoEmBinario, tamanhoBits);
    printf("--------\n");

    // a tabela de frequencia é a frequencia_e_caracteres( caracter e frequencia )
    printf("\nFrequencia de Caracteres:\n\n");
    for (int i = 0; i < 256; ++i) {
        if (frequencia_e_caracteres[i] > 0) {
            printf("Caracter: %c - Frequencia: %d vezes\n", i,
                frequencia_e_caracteres[i]);
        }
    }

    // Abrindo o arquivo para leitura
    file = fopen("texto.txt", "r");
    if (file == NULL) {
        exit(1);
    }
    // Lendo o arquivo e armazenando os caracteres em um vetor
    char* caracteres = malloc(contaCaracteres * sizeof(char));
    if (caracteres == NULL) {
        exit(1);
    }
    int count = 0;
    char ch;

    while ((ch = fgetc(file)) != EOF) {
        caracteres[count++] = ch;
    }
    compacta(contaCaracteres, codigoEmBinario, tamanhoBits, caracteres, file2);

    fclose(file2);
    fclose(file);

    file = fopen("textoC.dat", "rb");
    file2 = fopen("textoD.txt", "w");

    descompacta(arvore_huffMan, file, file2);

    fclose(file);
    fclose(file2);
    liberaArvoreHuffman(arvore_huffMan);
    printf("Memoria liberada com sucesso!\n");


    return 0;
}
