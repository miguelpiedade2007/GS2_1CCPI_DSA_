 /*
 * Citizen Assist AI - Prototipo em C / MedIndex (Recorte: Servicos de Saude)
 * Disciplina: Data Structures and Algorithms (DSA) - Global Solution 2026.2
 * Aluno: Miguel Anção Rotílio Piedade - RM 572445
 *
 * Compilacao:
 *   gcc -std=c11 -Wall -Wextra main.c -o main
 *
 * Execucao:
 *   ./main    no Linux e macOS
 *   main.exe  no Windows
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ============================================================================
 * CONSTANTES E MODELO DE DADOS
 * ============================================================================ */

#define TAM_TITULO     100
#define TAM_CATEGORIA   50
#define TAM_DESCRICAO  240
#define TAM_REQUISITOS 180
#define TAM_LOCAL      120
#define TAM_HORARIO     60
#define TAM_BUFFER     256

typedef struct {
    int  id;
    char titulo[TAM_TITULO];
    char categoria[TAM_CATEGORIA];
    char descricao[TAM_DESCRICAO];
    char requisitos[TAM_REQUISITOS];
    char local[TAM_LOCAL];
    char horario[TAM_HORARIO];
    int  prioridade; /* 1 (Baixa) a 5 (Critica / Emergencia) */
} ServicoSaude;

typedef struct No {
    ServicoSaude dados;
    struct No   *prox;
} No;

typedef struct {
    No    *inicio;
    No    *fim;
    size_t tamanho;
} ListaServicos;

typedef int (*Comparador)(const ServicoSaude *, const ServicoSaude *);

/* ============================================================================
 * UTILITARIOS DE STRING E ENTRADA SEGURA 
 * ============================================================================ */

static void limpar_restante_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta excesso do buffer */
    }
}

static bool ler_linha(const char *prompt, char *destino, size_t tamanho) {
    if (prompt != NULL) {
        printf("%s", prompt);
        fflush(stdout);
    }

    if (fgets(destino, (int)tamanho, stdin) == NULL) {
        return false;
    }

    size_t len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') {
        destino[len - 1] = '\0';
        len--;
    } else if (len == tamanho - 1) {
        limpar_restante_stdin();
    }

    if (len > 0 && destino[len - 1] == '\r') {
        destino[len - 1] = '\0';
    }

    return true;
}

static bool string_vazia(const char *s) {
    while (*s != '\0') {
        if (!isspace((unsigned char)*s)) {
            return false;
        }
        s++;
    }
    return true;
}

static bool ler_texto_obrigatorio(const char *prompt, char *destino, size_t tamanho) {
    for (;;) {
        if (!ler_linha(prompt, destino, tamanho)) {
            return false;
        }
        if (!string_vazia(destino)) {
            return true;
        }
        printf("Erro: este campo nao pode ficar vazio.\n");
    }
}

static bool ler_inteiro(const char *prompt, int min, int max, int *saida) {
    char buffer[TAM_BUFFER];

    for (;;) {
        if (!ler_linha(prompt, buffer, sizeof(buffer))) {
            return false;
        }

        if (string_vazia(buffer)) {
            printf("Erro: informe um numero inteiro entre %d e %d.\n", min, max);
            continue;
        }

        errno = 0;
        char *fim = NULL;
        long valor = strtol(buffer, &fim, 10);

        while (*fim != '\0' && isspace((unsigned char)*fim)) {
            fim++;
        }

        if (errno == ERANGE || *fim != '\0' || valor < min || valor > max) {
            printf("Erro: entrada invalida. Informe um numero inteiro entre %d e %d.\n", min, max);
            continue;
        }

        *saida = (int)valor;
        return true;
    }
}

static int comparar_str_ci(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0') {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) {
            return ca - cb;
        }
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

static bool contem_substr_ci(const char *texto, const char *termo) {
    if (*termo == '\0') {
        return true;
    }

    size_t len_termo = strlen(termo);
    for (const char *p = texto; *p != '\0'; p++) {
        size_t i = 0;
        while (i < len_termo && p[i] != '\0' &&
               tolower((unsigned char)p[i]) == tolower((unsigned char)termo[i])) {
            i++;
        }
        if (i == len_termo) {
            return true;
        }
    }
    return false;
}

/* ============================================================================
 * OPERACOES DA LISTA ENCADEADA
 * ============================================================================ */

static void inicializar_lista(ListaServicos *lista) {
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

static bool lista_vazia(const ListaServicos *lista) {
    return lista->inicio == NULL;
}

static void destruir_lista(ListaServicos *lista) {
    No *atual = lista->inicio;
    while (atual != NULL) {
        No *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

/* Busca linear por ID - Complexidade: O(n) */
static No *buscar_por_id(const ListaServicos *lista, int id) {
    No *atual = lista->inicio;
    while (atual != NULL) {
        if (atual->dados.id == id) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

/* Insercao no final da lista - Complexidade: O(1) apos validacao de unicidade O(n) */
static bool inserir_servico(ListaServicos *lista, const ServicoSaude *servico) {
    if (buscar_por_id(lista, servico->id) != NULL) {
        return false;
    }

    No *novo = (No *)malloc(sizeof(No));
    if (novo == NULL) {
        fprintf(stderr, "Erro critico: falha ao alocar memoria para novo registro.\n");
        return false;
    }

    novo->dados = *servico;
    novo->prox = NULL;

    if (lista->inicio == NULL) {
        lista->inicio = novo;
        lista->fim = novo;
    } else {
        lista->fim->prox = novo;
        lista->fim = novo;
    }

    lista->tamanho++;
    return true;
}

/* Remocao por ID - Complexidade: O(n) */
static bool remover_por_id(ListaServicos *lista, int id) {
    No *anterior = NULL;
    No *atual = lista->inicio;

    while (atual != NULL && atual->dados.id != id) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual == NULL) {
        return false;
    }

    if (anterior == NULL) {
        lista->inicio = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }

    if (atual == lista->fim) {
        lista->fim = anterior;
    }

    free(atual);
    lista->tamanho--;
    return true;
}

/* ============================================================================
 * ALGORITMO DE ORDENACAO: MERGE SORT EM LISTA ENCADEADA - O(n log n)
 * ============================================================================ */

static int cmp_id_asc(const ServicoSaude *a, const ServicoSaude *b) {
    if (a->id < b->id) return -1;
    if (a->id > b->id) return 1;
    return 0;
}

static int cmp_titulo_asc(const ServicoSaude *a, const ServicoSaude *b) {
    return comparar_str_ci(a->titulo, b->titulo);
}

static int cmp_categoria_asc(const ServicoSaude *a, const ServicoSaude *b) {
    int r = comparar_str_ci(a->categoria, b->categoria);
    return (r != 0) ? r : comparar_str_ci(a->titulo, b->titulo);
}

static int cmp_prioridade_desc(const ServicoSaude *a, const ServicoSaude *b) {
    if (a->prioridade > b->prioridade) return -1;
    if (a->prioridade < b->prioridade) return 1;
    return cmp_id_asc(a, b);
}

static void dividir_lista(No *origem, No **metade_esq, No **metade_dir) {
    No *lento = origem;
    No *rapido = origem->prox;

    while (rapido != NULL) {
        rapido = rapido->prox;
        if (rapido != NULL) {
            lento = lento->prox;
            rapido = rapido->prox;
        }
    }

    *metade_esq = origem;
    *metade_dir = lento->prox;
    lento->prox = NULL;
}

static No *intercalar_listas(No *esq, No *dir, Comparador cmp) {
    No sentinela;
    No *cauda = &sentinela;
    sentinela.prox = NULL;

    while (esq != NULL && dir != NULL) {
        if (cmp(&esq->dados, &dir->dados) <= 0) {
            cauda->prox = esq;
            esq = esq->prox;
        } else {
            cauda->prox = dir;
            dir = dir->prox;
        }
        cauda = cauda->prox;
    }

    cauda->prox = (esq != NULL) ? esq : dir;
    return sentinela.prox;
}

static No *merge_sort_nos(No *cabeca, Comparador cmp) {
    if (cabeca == NULL || cabeca->prox == NULL) {
        return cabeca;
    }

    No *esq = NULL;
    No *dir = NULL;
    dividir_lista(cabeca, &esq, &dir);

    esq = merge_sort_nos(esq, cmp);
    dir = merge_sort_nos(dir, cmp);

    return intercalar_listas(esq, dir, cmp);
}

static void ordenar_lista(ListaServicos *lista, Comparador cmp) {
    if (lista->tamanho < 2) {
        return;
    }

    lista->inicio = merge_sort_nos(lista->inicio, cmp);

    /* Atualiza o ponteiro fim apos a reordenacao */
    No *atual = lista->inicio;
    while (atual != NULL && atual->prox != NULL) {
        atual = atual->prox;
    }
    lista->fim = atual;
}

/* ============================================================================
 * EXIBICAO E INTERACAO COM O USUARIO
 * ============================================================================ */

static const char *rotulo_prioridade(int p) {
    switch (p) {
        case 1: return "1 - Baixa";
        case 2: return "2 - Moderada";
        case 3: return "3 - Media";
        case 4: return "4 - Alta";
        case 5: return "5 - Critica/Emergencia";
        default: return "Indefinida";
    }
}

static void exibir_servico(const ServicoSaude *s) {
    printf("------------------------------------------------------------------------\n");
    printf("ID:          %d\n", s->id);
    printf("Titulo:      %s\n", s->titulo);
    printf("Categoria:   %s\n", s->categoria);
    printf("Prioridade:  %s\n", rotulo_prioridade(s->prioridade));
    printf("Local:       %s\n", s->local);
    printf("Horario:     %s\n", s->horario);
    printf("Descricao:   %s\n", s->descricao);
    printf("Requisitos:  %s\n", s->requisitos);
}

static void acao_cadastrar(ListaServicos *lista) {
    ServicoSaude s;
    printf("\n=== CADASTRAR SERVICO DE SAUDE ===\n");

    if (!ler_inteiro("ID unico (positivo): ", 1, INT_MAX, &s.id)) {
        return;
    }

    if (buscar_por_id(lista, s.id) != NULL) {
        printf("Erro: ja existe um servico cadastrado com o ID %d.\n", s.id);
        return;
    }

    if (!ler_texto_obrigatorio("Titulo do servico: ", s.titulo, sizeof(s.titulo))) return;
    if (!ler_texto_obrigatorio("Categoria (ex: Vacinacao, Urgencia, Exames, UBS): ",
                               s.categoria, sizeof(s.categoria))) return;
    if (!ler_texto_obrigatorio("Descricao: ", s.descricao, sizeof(s.descricao))) return;
    if (!ler_texto_obrigatorio("Requisitos (documentos/condicoes): ",
                               s.requisitos, sizeof(s.requisitos))) return;
    if (!ler_texto_obrigatorio("Local de atendimento: ", s.local, sizeof(s.local))) return;
    if (!ler_texto_obrigatorio("Horario de funcionamento: ", s.horario, sizeof(s.horario))) return;
    if (!ler_inteiro("Nivel de prioridade (1=Baixa a 5=Critica): ", 1, 5, &s.prioridade)) return;

    if (inserir_servico(lista, &s)) {
        printf("Sucesso: servico [%d - %s] cadastrado.\n", s.id, s.titulo);
    } else {
        printf("Erro: nao foi possivel cadastrar o servico.\n");
    }
}

static void acao_listar(const ListaServicos *lista) {
    printf("\n=== LISTA DE SERVICOS DE SAUDE (%zu registros) ===\n", lista->tamanho);
    if (lista_vazia(lista)) {
        printf("Aviso: nenhum servico cadastrado.\n");
        return;
    }

    const No *atual = lista->inicio;
    while (atual != NULL) {
        exibir_servico(&atual->dados);
        atual = atual->prox;
    }
    printf("------------------------------------------------------------------------\n");
}

static void acao_consultar_id(const ListaServicos *lista) {
    printf("\n=== CONSULTAR SERVICO POR ID ===\n");
    if (lista_vazia(lista)) {
        printf("Aviso: a lista esta vazia.\n");
        return;
    }

    int id = 0;
    if (!ler_inteiro("Informe o ID do servico: ", 1, INT_MAX, &id)) {
        return;
    }

    const No *encontrado = buscar_por_id(lista, id);
    if (encontrado == NULL) {
        printf("Resultado: nenhum servico encontrado com o ID %d.\n", id);
        return;
    }

    exibir_servico(&encontrado->dados);
    printf("------------------------------------------------------------------------\n");
}

/* Pesquisa linear por atributo (titulo ou categoria) - Complexidade: O(n * m) */
static void acao_pesquisar(const ListaServicos *lista) {
    printf("\n=== PESQUISAR SERVICOS DE SAUDE ===\n");
    if (lista_vazia(lista)) {
        printf("Aviso: a lista esta vazia.\n");
        return;
    }

    printf("1 - Pesquisar por Titulo\n");
    printf("2 - Pesquisar por Categoria\n");
    printf("3 - Pesquisar por Local\n");

    int modo = 0;
    if (!ler_inteiro("Escolha o criterio: ", 1, 3, &modo)) {
        return;
    }

    char termo[TAM_TITULO];
    if (!ler_texto_obrigatorio("Digite o termo de busca: ", termo, sizeof(termo))) {
        return;
    }

    size_t encontrados = 0;
    const No *atual = lista->inicio;
    while (atual != NULL) {
        bool corresponde = false;
        if (modo == 1) {
            corresponde = contem_substr_ci(atual->dados.titulo, termo);
        } else if (modo == 2) {
            corresponde = contem_substr_ci(atual->dados.categoria, termo);
        } else {
            corresponde = contem_substr_ci(atual->dados.local, termo);
        }

        if (corresponde) {
            exibir_servico(&atual->dados);
            encontrados++;
        }
        atual = atual->prox;
    }

    if (encontrados == 0) {
        printf("Resultado: nenhum servico encontrado para o termo \"%s\".\n", termo);
    } else {
        printf("------------------------------------------------------------------------\n");
        printf("Total encontrado: %zu registro(s).\n", encontrados);
    }
}

static void atualizar_campo_texto(const char *rotulo, char *destino, size_t tamanho) {
    char buffer[TAM_BUFFER];
    printf("%s atual: %s\n", rotulo, destino);
    if (ler_linha("Novo valor (ENTER para manter): ", buffer, sizeof(buffer))) {
        if (!string_vazia(buffer)) {
            strncpy(destino, buffer, tamanho - 1);
            destino[tamanho - 1] = '\0';
        }
    }
}

static void acao_atualizar(ListaServicos *lista) {
    printf("\n=== ATUALIZAR SERVICO DE SAUDE ===\n");
    if (lista_vazia(lista)) {
        printf("Aviso: a lista esta vazia.\n");
        return;
    }

    int id = 0;
    if (!ler_inteiro("Informe o ID do servico a atualizar: ", 1, INT_MAX, &id)) {
        return;
    }

    No *no = buscar_por_id(lista, id);
    if (no == NULL) {
        printf("Resultado: nenhum servico encontrado com o ID %d.\n", id);
        return;
    }

    printf("Editando servico ID %d:\n", no->dados.id);
    atualizar_campo_texto("Titulo", no->dados.titulo, sizeof(no->dados.titulo));
    atualizar_campo_texto("Categoria", no->dados.categoria, sizeof(no->dados.categoria));
    atualizar_campo_texto("Descricao", no->dados.descricao, sizeof(no->dados.descricao));
    atualizar_campo_texto("Requisitos", no->dados.requisitos, sizeof(no->dados.requisitos));
    atualizar_campo_texto("Local", no->dados.local, sizeof(no->dados.local));
    atualizar_campo_texto("Horario", no->dados.horario, sizeof(no->dados.horario));

    printf("Prioridade atual: %s\n", rotulo_prioridade(no->dados.prioridade));
    int nova_prioridade = 0;
    if (ler_inteiro("Nova prioridade (1-5, ou 0 para manter): ", 0, 5, &nova_prioridade)) {
        if (nova_prioridade != 0) {
            no->dados.prioridade = nova_prioridade;
        }
    }

    printf("Sucesso: servico ID %d atualizado.\n", no->dados.id);
}

static void acao_excluir(ListaServicos *lista) {
    printf("\n=== EXCLUIR SERVICO DE SAUDE ===\n");
    if (lista_vazia(lista)) {
        printf("Aviso: a lista esta vazia.\n");
        return;
    }

    int id = 0;
    if (!ler_inteiro("Informe o ID do servico a excluir: ", 1, INT_MAX, &id)) {
        return;
    }

    if (remover_por_id(lista, id)) {
        printf("Sucesso: servico ID %d removido da lista.\n", id);
    } else {
        printf("Resultado: nenhum servico encontrado com o ID %d.\n", id);
    }
}

static void acao_ordenar(ListaServicos *lista) {
    printf("\n=== ORDENAR SERVICOS (MERGE SORT) ===\n");
    if (lista_vazia(lista)) {
        printf("Aviso: a lista esta vazia.\n");
        return;
    }

    printf("1 - Ordenar por ID (crescente)\n");
    printf("2 - Ordenar por Titulo (A-Z)\n");
    printf("3 - Ordenar por Categoria (A-Z)\n");
    printf("4 - Ordenar por Prioridade (Critica -> Baixa)\n");

    int criterio = 0;
    if (!ler_inteiro("Escolha o criterio de ordenacao: ", 1, 4, &criterio)) {
        return;
    }

    switch (criterio) {
        case 1: ordenar_lista(lista, cmp_id_asc); break;
        case 2: ordenar_lista(lista, cmp_titulo_asc); break;
        case 3: ordenar_lista(lista, cmp_categoria_asc); break;
        case 4: ordenar_lista(lista, cmp_prioridade_desc); break;
        default: return;
    }

    printf("Sucesso: lista ordenada com Merge Sort.\n");
    acao_listar(lista);
}

typedef struct {
    const char *nome;
    size_t      quantidade;
} ContagemCategoria;

/* Analise e indicadores - Complexidade: O(n * k), sendo k o numero de categorias distintas */
static void acao_analise(const ListaServicos *lista) {
    printf("\n=== ANALISE E INDICADORES (SERVICOS DE SAUDE) ===\n");
    if (lista_vazia(lista)) {
        printf("Aviso: nenhum registro disponivel para analise.\n");
        return;
    }

    ContagemCategoria *cats = (ContagemCategoria *)malloc(lista->tamanho * sizeof(ContagemCategoria));
    if (cats == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memoria para analise.\n");
        return;
    }

    size_t num_cats = 0;
    size_t por_prioridade[6] = {0};
    size_t soma_prioridades = 0;
    size_t servicos_24h = 0;

    const No *atual = lista->inicio;
    while (atual != NULL) {
        const ServicoSaude *s = &atual->dados;

        if (s->prioridade >= 1 && s->prioridade <= 5) {
            por_prioridade[s->prioridade]++;
            soma_prioridades += (size_t)s->prioridade;
        }

        if (contem_substr_ci(s->horario, "24h") || contem_substr_ci(s->horario, "24 horas")) {
            servicos_24h++;
        }

        bool achou = false;
        for (size_t i = 0; i < num_cats; i++) {
            if (comparar_str_ci(cats[i].nome, s->categoria) == 0) {
                cats[i].quantidade++;
                achou = true;
                break;
            }
        }
        if (!achou) {
            cats[num_cats].nome = s->categoria;
            cats[num_cats].quantidade = 1;
            num_cats++;
        }

        atual = atual->prox;
    }

    double media_prioridade = (double)soma_prioridades / (double)lista->tamanho;

    printf("Total de servicos cadastrados: %zu\n", lista->tamanho);
    printf("Servicos com atendimento 24h:  %zu (%.1f%%)\n",
           servicos_24h, (100.0 * (double)servicos_24h) / (double)lista->tamanho);
    printf("Media de prioridade:           %.2f / 5.00\n", media_prioridade);

    printf("\nQuantidade por categoria:\n");
    for (size_t i = 0; i < num_cats; i++) {
        double pct = (100.0 * (double)cats[i].quantidade) / (double)lista->tamanho;
        printf("  - %-25s: %zu (%.1f%%)\n", cats[i].nome, cats[i].quantidade, pct);
    }

    printf("\nDistribuicao por prioridade:\n");
    for (int p = 5; p >= 1; p--) {
        printf("  - %-23s: %zu\n", rotulo_prioridade(p), por_prioridade[p]);
    }

    free(cats);
}

static void acao_carregar_exemplos(ListaServicos *lista) {
    const ServicoSaude exemplos[] = {
        {
            104,
            "Pronto-Socorro Clinico e Traumatico",
            "Urgencia e Emergencia",
            "Atendimento imediato para casos graves, traumas, dor toracica e risco de vida.",
            "Documento oficial com foto e Cartao SUS (atendimento garantido mesmo sem documentos em emergencia).",
            "UPA 24h Zona Oeste - Av. Escola Politecnica, 1200",
            "Todos os dias, 24 horas",
            5
        },
        {
            101,
            "Campanha de Vacinacao contra Influenza e COVID-19",
            "Vacinacao",
            "Aplicacao gratuita de vacinas do calendario nacional para grupos prioritarios e publico geral.",
            "Documento com foto, CPF ou Cartao SUS e caderneta de vacinacao.",
            "UBS Vila Sao Francisco - Rua Dr. Candido Motta Filho, 450",
            "Segunda a Sexta, 07h00 as 19h00",
            3
        },
        {
            105,
            "Acolhimento em Saude Mental",
            "Saude Mental",
            "Triagem psicologica, acompanhamento psiquiatrico e grupos terapeuticos.",
            "Documento com foto e comprovante de residencia (demanda espontanea).",
            "CAPS II Butanta - Rua Salvador Riscali, 88",
            "Segunda a Sexta, 08h00 as 18h00",
            4
        },
        {
            102,
            "Retirada de Medicamentos Continuos",
            "Farmacia Popular",
            "Dispensacao gratuita de medicamentos para hipertensao, diabetes e asma.",
            "Receita medica valida (ate 180 dias), CPF e documento com foto.",
            "Farmacia Municipal Central - Praca da Republica, 310",
            "Segunda a Sabado, 08h00 as 17h00",
            3
        },
        {
            103,
            "Coleta de Exames Laboratoriais Basicos",
            "Exames",
            "Coleta de sangue e urina para hemograma, glicemia, colesterol e funcao renal.",
            "Pedido medico do SUS, agendamento previo, jejum conforme orientacao e documento com foto.",
            "UBS Jardim Bonfiglioli - Rua Joao Adolfo, 215",
            "Segunda a Sexta, 06h30 as 10h30",
            2
        }
    };

    size_t total = sizeof(exemplos) / sizeof(exemplos[0]);
    size_t inseridos = 0;

    for (size_t i = 0; i < total; i++) {
        if (inserir_servico(lista, &exemplos[i])) {
            inseridos++;
        }
    }

    printf("\nCarregamento concluido: %zu novo(s) servico(s) de exemplo inserido(s).\n", inseridos);
}

static void exibir_menu(void) {
    printf("\n========================================================================\n");
    printf("              CITIZEN ASSIST AI - SERVICOS DE SAUDE (DSA)\n");
    printf("========================================================================\n");
    printf(" 1 - Cadastrar servico de saude\n");
    printf(" 2 - Listar todos os servicos\n");
    printf(" 3 - Consultar servico por ID\n");
    printf(" 4 - Pesquisar servicos (Titulo / Categoria / Local)\n");
    printf(" 5 - Atualizar servico existente\n");
    printf(" 6 - Excluir servico\n");
    printf(" 7 - Ordenar servicos (Merge Sort)\n");
    printf(" 8 - Analise e indicadores\n");
    printf(" 9 - Carregar dados de exemplo\n");
    printf(" 0 - Encerrar\n");
    printf("========================================================================\n");
}

int main(void) {
    ListaServicos lista;
    inicializar_lista(&lista);

    int opcao = -1;
    do {
        exibir_menu();
        if (!ler_inteiro("Escolha uma opcao: ", 0, 9, &opcao)) {
            printf("\nEntrada encerrada (EOF). Finalizando sistema...\n");
            break;
        }

        switch (opcao) {
            case 1: acao_cadastrar(&lista); 
                    break;
            case 2: acao_listar(&lista);    
                    break;
            case 3: acao_consultar_id(&lista);
                  break;
            case 4: acao_pesquisar(&lista);   
                  break;
            case 5: acao_atualizar(&lista);   
                  break;
            case 6: acao_excluir(&lista);     
                  break;
            case 7: acao_ordenar(&lista);      
                 break;
            case 8: acao_analise(&lista);     
                  break;
            case 9: acao_carregar_exemplos(&lista);
             break;
            case 0: printf("Encerrando Citizen Assist AI...\n");
             break;
            default: printf("Opcao invalida.\n");  
             break;
        }
    } while (opcao != 0);

    destruir_lista(&lista);
    return 0;
}