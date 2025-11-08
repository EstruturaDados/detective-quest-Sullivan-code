#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PISTAS 20
#define TAM_HASH 10

// Estrutura para a árvore binária de salas
typedef struct Sala {
    char nome[30];
    char pista[100];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// Estrutura para a BST de pistas
typedef struct PistaBST {
    char pista[100];
    struct PistaBST *esq;
    struct PistaBST *dir;
} PistaBST;

// Estrutura para a tabela hash
typedef struct EntradaHash {
    char pista[100];
    char suspeito[30];
    struct EntradaHash *prox;
} EntradaHash;

// Variáveis globais
PistaBST *bstPistas = NULL;
EntradaHash *tabelaHash[TAM_HASH];
int pistasColetadas = 0;
char pistasEncontradas[MAX_PISTAS][100];

// Protótipos das funções
Sala* criarSala(const char *nome, const char *pista);
void inserirPistaBST(const char *pista);
void inserirNaHash(const char *pista, const char *suspeito);
int hash(const char *pista);
void explorarSalas(Sala *raiz);
void listarPistasBST(PistaBST *raiz);
const char* encontrarSuspeito(const char *pista);
void verificarSuspeitoFinal();
void liberarBST(PistaBST *raiz);
void liberarHash();
void liberarSalas(Sala *raiz);

// Função para criar uma nova sala dinamicamente
Sala* criarSala(const char *nome, const char *pista) {
    Sala *novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala != NULL) {
        strcpy(novaSala->nome, nome);
        strcpy(novaSala->pista, pista);
        novaSala->esquerda = NULL;
        novaSala->direita = NULL;
    }
    return novaSala;
}

// Função hash simples para strings
int hash(const char *pista) {
    int soma = 0;
    for (int i = 0; pista[i] != '\0'; i++) {
        soma += pista[i];
    }
    return soma % TAM_HASH;
}

// Função para inserir pista na BST de forma ordenada
void inserirPistaBST(const char *pista) {
    PistaBST **atual = &bstPistas;
    
    while (*atual != NULL) {
        int cmp = strcmp(pista, (*atual)->pista);
        if (cmp < 0) {
            atual = &(*atual)->esq;
        } else if (cmp > 0) {
            atual = &(*atual)->dir;
        } else {
            return; // Pista já existe
        }
    }
    
    *atual = (PistaBST*)malloc(sizeof(PistaBST));
    strcpy((*atual)->pista, pista);
    (*atual)->esq = NULL;
    (*atual)->dir = NULL;
    
    // Adiciona também ao array de pistas encontradas
    if (pistasColetadas < MAX_PISTAS) {
        strcpy(pistasEncontradas[pistasColetadas], pista);
        pistasColetadas++;
    }
}

// Função para inserir associação pista-suspeito na tabela hash
void inserirNaHash(const char *pista, const char *suspeito) {
    int indice = hash(pista);
    EntradaHash *novaEntrada = (EntradaHash*)malloc(sizeof(EntradaHash));
    
    strcpy(novaEntrada->pista, pista);
    strcpy(novaEntrada->suspeito, suspeito);
    novaEntrada->prox = tabelaHash[indice];
    tabelaHash[indice] = novaEntrada;
}

// Função para encontrar suspeito associado a uma pista
const char* encontrarSuspeito(const char *pista) {
    int indice = hash(pista);
    EntradaHash *atual = tabelaHash[indice];
    
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->prox;
    }
    return "Desconhecido";
}

// Função para explorar as salas interativamente
void explorarSalas(Sala *raiz) {
    Sala *atual = raiz;
    char comando;
    
    printf("\n=== EXPLORAÇÃO DA MANSÃO ===\n");
    printf("Você está na: %s\n", atual->nome);
    
    while (1) {
        printf("\nComandos disponíveis:\n");
        printf("e - Ir para sala esquerda\n");
        printf("d - Ir para sala direita\n");
        printf("p - Procurar por pistas nesta sala\n");
        printf("l - Listar pistas coletadas\n");
        printf("s - Sair da exploração\n");
        printf("Escolha: ");
        scanf(" %c", &comando);
        getchar(); // Limpar buffer
        
        switch (tolower(comando)) {
            case 'e':
                if (atual->esquerda != NULL) {
                    atual = atual->esquerda;
                    printf("\nVocê se moveu para: %s\n", atual->nome);
                } else {
                    printf("\nNão há sala à esquerda!\n");
                }
                break;
                
            case 'd':
                if (atual->direita != NULL) {
                    atual = atual->direita;
                    printf("\nVocê se moveu para: %s\n", atual->nome);
                } else {
                    printf("\nNão há sala à direita!\n");
                }
                break;
                
            case 'p':
                if (strlen(atual->pista) > 0) {
                    printf("\n*** PISTA ENCONTRADA: %s ***\n", atual->pista);
                    inserirPistaBST(atual->pista);
                    printf("Pista adicionada ao seu caderno!\n");
                } else {
                    printf("\nNenhuma pista encontrada nesta sala.\n");
                }
                break;
                
            case 'l':
                printf("\n=== PISTAS COLETADAS ===\n");
                if (bstPistas == NULL) {
                    printf("Nenhuma pista coletada ainda.\n");
                } else {
                    listarPistasBST(bstPistas);
                }
                break;
                
            case 's':
                printf("\nSaindo da exploração...\n");
                return;
                
            default:
                printf("\nComando inválido! Tente novamente.\n");
        }
    }
}

// Função para listar pistas em ordem (in-order traversal)
void listarPistasBST(PistaBST *raiz) {
    if (raiz != NULL) {
        listarPistasBST(raiz->esq);
        printf("- %s\n", raiz->pista);
        listarPistasBST(raiz->dir);
    }
}

// Função para verificar a acusação final do jogador
void verificarSuspeitoFinal() {
    char suspeitoAcusado[30];
    int contador = 0;
    
    printf("\n=== FASE DE ACUSAÇÃO ===\n");
    printf("Pistas coletadas (%d no total):\n", pistasColetadas);
    
    if (pistasColetadas == 0) {
        printf("Nenhuma pista coletada! Você não pode fazer uma acusação.\n");
        return;
    }
    
    // Listar todas as pistas e seus suspeitos associados
    for (int i = 0; i < pistasColetadas; i++) {
        const char *suspeito = encontrarSuspeito(pistasEncontradas[i]);
        printf("%d. %s -> Aponta para: %s\n", i+1, pistasEncontradas[i], suspeito);
    }
    
    printf("\nSuspeitos conhecidos:\n");
    printf("- João Silva (Mordomo)\n");
    printf("- Maria Santos (Herdeira)\n"); 
    printf("- Carlos Lima (Empresário)\n");
    printf("- Ana Costa (Amiga da família)\n");
    printf("- Pedro Alves (Jardineiro)\n");
    
    printf("\nDigite o nome do suspeito que você acusa: ");
    fgets(suspeitoAcusado, 30, stdin);
    suspeitoAcusado[strcspn(suspeitoAcusado, "\n")] = 0;
    
    // Contar quantas pistas apontam para o suspeito acusado
    for (int i = 0; i < pistasColetadas; i++) {
        const char *suspeito = encontrarSuspeito(pistasEncontradas[i]);
        if (strcasecmp(suspeito, suspeitoAcusado) == 0) {
            contador++;
        }
    }
    
    printf("\n=== RESULTADO DA ACUSAÇÃO ===\n");
    printf("Suspeito acusado: %s\n", suspeitoAcusado);
    printf("Pistas que apontam para %s: %d\n", suspeitoAcusado, contador);
    
    if (contador >= 2) {
        printf("\n🎉 PARABÉNS! A acusação é sustentada pelas evidências!\n");
        printf("Você resolveu o caso! %s é o culpado!\n", suspeitoAcusado);
    } else {
        printf("\n❌ A acusação não é sustentada por evidências suficientes.\n");
        printf("São necessárias pelo menos 2 pistas apontando para o mesmo suspeito.\n");
        printf("O verdadeiro culpado escapou... Tente novamente!\n");
    }
}

// Funções para liberar memória
void liberarBST(PistaBST *raiz) {
    if (raiz != NULL) {
        liberarBST(raiz->esq);
        liberarBST(raiz->dir);
        free(raiz);
    }
}

void liberarHash() {
    for (int i = 0; i < TAM_HASH; i++) {
        EntradaHash *atual = tabelaHash[i];
        while (atual != NULL) {
            EntradaHash *temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
}

void liberarSalas(Sala *raiz) {
    if (raiz != NULL) {
        liberarSalas(raiz->esquerda);
        liberarSalas(raiz->direita);
        free(raiz);
    }
}

int main() {
    // Inicializar tabela hash
    for (int i = 0; i < TAM_HASH; i++) {
        tabelaHash[i] = NULL;
    }
    
    // Criar a árvore binária de salas (mapa fixo da mansão)
    Sala *raiz = criarSala("Hall Principal", "Uma carta rasgada com a letra 'J'");
    raiz->esquerda = criarSala("Biblioteca", "Um livro sobre venenos com a página 47 marcada");
    raiz->direita = criarSala("Sala de Jantar", "Um copo de vinho com resíduos estranhos");
    raiz->esquerda->esquerda = criarSala("Escritório", "Extratos bancários com transferências suspeitas");
    raiz->esquerda->direita = criarSala("Quarto de Hóspedes", "Um bilhete com a assinatura 'M'");
    raiz->direita->esquerda = criarSala("Cozinha", "Uma faca manchada encontrada na pia");
    raiz->direita->direita = criarSala("Jardim de Inverno", "Pegadas de barro levando à saída");
    
    // Preencher a tabela hash com associações pista-suspeito
    inserirNaHash("Uma carta rasgada com a letra 'J'", "João Silva");
    inserirNaHash("Um livro sobre venenos com a página 47 marcada", "Maria Santos");
    inserirNaHash("Um copo de vinho com resíduos estranhos", "Carlos Lima");
    inserirNaHash("Extratos bancários com transferências suspeitas", "Ana Costa");
    inserirNaHash("Um bilhete com a assinatura 'M'", "Maria Santos");
    inserirNaHash("Uma faca manchada encontrada na pia", "Pedro Alves");
    inserirNaHash("Pegadas de barro levando à saída", "Pedro Alves");
    
    int opcao;
    
    printf("🕵️‍♂️ === DETECTIVE QUEST === 🕵️‍♂️\n");
    printf("Bem-vindo detetive! Um crime foi cometido na mansão Blackwood.\n");
    printf("Explore os cômodos, colete pistas e descubra o culpado!\n");
    
    do {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Explorar a mansão\n");
        printf("2. Listar pistas coletadas\n");
        printf("3. Fazer acusação final\n");
        printf("0. Sair do jogo\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao) {
            case 1:
                explorarSalas(raiz);
                break;
            case 2:
                printf("\n=== PISTAS COLETADAS ===\n");
                if (bstPistas == NULL) {
                    printf("Nenhuma pista coletada ainda.\n");
                } else {
                    listarPistasBST(bstPistas);
                }
                break;
            case 3:
                verificarSuspeitoFinal();
                break;
            case 0:
                printf("Saindo do jogo...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 0);
    
    // Liberar memória alocada
    liberarBST(bstPistas);
    liberarHash();
    liberarSalas(raiz);
    
    printf("Obrigado por jogar Detective Quest!\n");
    return 0;
}