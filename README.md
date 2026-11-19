# Documentação Técnica — Citizen Assist AI (Serviços de Saúde)

**Disciplina:** Data Structures and Algorithms (DSA) — Turma 1CCPI 
**Avaliação:** Global Solution 2026.2 — Citizen Tech  
**Recorte Escolhido:** Serviços de Saúde Pública  
**Linguagem de Implementação:** C  
**Integrante:** Miguel Anção Rotílio Piedade — RM 572445  

---

## 1. Contextualização e Recorte Escolhido

O **Citizen Assist AI** é um protótipo de sistema de linha de comando desenvolvido para organizar, consultar e gerenciar informações úteis ao cidadão. O recorte escolhido é o de **Serviços de Saúde**, abrangendo pontos de atendimento e programas públicos essenciais, tais como:

* **Urgência e Emergência:** Unidades de Pronto Atendimento (UPAs 24h) e prontos-socorros.
* **Vacinação:** Campanhas e aplicação de imunizantes em Unidades Básicas de Saúde (UBSs).
* **Saúde Mental:** Centros de Atenção Psicossocial (CAPS) e triagem psicológica.
* **Farmácia Popular:** Dispensação gratuita de medicamentos de uso contínuo.
* **Exames Laboratoriais:** Coleta de exames básicos vinculados ao SUS.

O foco do projeto é a aplicação prática de Estruturas de Dados e Algoritmos implementados manualmente em linguagem C, garantindo gerenciamento seguro de memória, integridade dos dados e desempenho previsível nas operações de busca e ordenação.

---

## 2. Instruções de Compilação e Execução

O código-fonte (`main.c`) segue rigorosamente o padrão **ISO C11**, sem dependências de bibliotecas externas, sendo compatível com compiladores GCC e Clang em Linux, macOS e Windows (MinGW/MSYS2).

### Compilação (com flags estritas de verificação)

```bash
gcc -std=c11 -Wall -Wextra main.c -o citizen_assist
```

A compilação com essas flags não gera nenhum aviso (*warning*).

### Execução

* **Linux / macOS:**
  ```bash
  ./citizen_assist
  ```
* **Windows (CMD / PowerShell):**
  ```powershell
  citizen_assist.exe
  ```

---

## 3. Modelo de Dados e Lista Encadeada

### 3.1. Registro de Serviço de Saúde (`ServicoSaude`)

Cada serviço de saúde é modelado por meio de uma `struct` com `typedef`, contendo os atributos pertinentes ao atendimento ao cidadão:

| Campo | Tipo em C | Tamanho / Faixa | Descrição |
| :--- | :--- | :--- | :--- |
| `id` | `int` | Inteiro positivo ($\ge 1$) | Identificador único do serviço |
| `titulo` | `char[100]` | Até 99 caracteres | Nome resumido do serviço |
| `categoria` | `char[50]` | Até 49 caracteres | Classificação (ex.: Vacinação, Exames, UBS) |
| `descricao` | `char[240]` | Até 239 caracteres | Detalhamento do atendimento oferecido |
| `requisitos` | `char[180]` | Até 179 caracteres | Documentos ou condições exigidas do cidadão |
| `local` | `char[120]` | Até 119 caracteres | Unidade de saúde e endereço |
| `horario` | `char[60]` | Até 59 caracteres | Dias e horários de funcionamento |
| `prioridade` | `int` | $1$ a $5$ | Nível de criticidade ($1$ = Baixa até $5$ = Crítica/Emergência) |

### 3.2. Estrutura da Lista Encadeada (`No` e `ListaServicos`)

A coleção de serviços é armazenada em uma **lista simplesmente encadeada** implementada manualmente com ponteiros e alocação dinâmica:

* **`No`:** armazena a estrutura `ServicoSaude dados` e o ponteiro `struct No *prox` para o próximo elemento da sequência.
* **`ListaServicos`:** estrutura descritora que encapsula três metadados:
  * `No *inicio`: aponta para o primeiro nó da lista (ou `NULL` quando vazia).
  * `No *fim`: aponta para o último nó da lista, permitindo o encadeamento no final em tempo $O(1)$.
  * `size_t tamanho`: mantém a contagem exata de registros cadastrados, viabilizando consultas de cardinalidade em tempo $O(1)$.

### 3.3. Operações e Gerenciamento Dinâmico de Memória

* **Inicialização (`inicializar_lista`):** define `inicio = NULL`, `fim = NULL` e `tamanho = 0`.
* **Inserção (`inserir_servico`):** verifica previamente se o `id` já existe; em seguida, aloca dinamicamente o nó via `malloc(sizeof(No))`, valida se o ponteiro retornado não é `NULL`, acopla o novo nó em `lista->fim` e incrementa `lista->tamanho`.
* **Remoção (`remover_por_id`):** utiliza dois ponteiros auxiliares (`anterior` e `atual`) para localizar o registro e religar o encadeamento nos três cenários possíveis (remoção do primeiro nó, de um nó intermediário ou do último nó, atualizando `lista->fim`). Por fim, libera a memória com `free(atual)` e decrementa `lista->tamanho`.
* **Destruição (`destruir_lista`):** chamada obrigatoriamente antes do encerramento do programa na função `main`, percorre todos os nós salvando `atual->prox` antes de invocar `free(atual)`, garantindo ausência de vazamentos de memória (*memory leaks*).
* **Análise (`acao_analise`):** aloca dinamicamente um vetor temporário de `ContagemCategoria` via `malloc` proporcional ao número de registros e o libera com `free` ao término do relatório.

---

## 4. Interface de Menu, Validações e Indicadores

O sistema opera em laço interativo no terminal, oferecendo todas as operações exigidas:

| Opção | Operação | Função Responsável |
| :---: | :--- | :--- |
| `1` | Cadastrar serviço de saúde | `acao_cadastrar` |
| `2` | Listar todos os serviços | `acao_listar` |
| `3` | Consultar serviço por ID | `acao_consultar_id` |
| `4` | Pesquisar serviços (Título, Categoria ou Local) | `acao_pesquisar` |
| `5` | Atualizar serviço existente | `acao_atualizar` |
| `6` | Excluir serviço por ID | `acao_excluir` |
| `7` | Ordenar serviços (Merge Sort) | `acao_ordenar` |
| `8` | Exibir análise e indicadores | `acao_analise` |
| `9` | Carregar dados de exemplo (5 serviços com dados simulados) | `acao_carregar_exemplos` |
| `0` | Encerrar o sistema e liberar memória | `destruir_lista` |

### Tratamento de Entradas e Integridade

* **Leitura segura de strings (`ler_linha` e `ler_texto_obrigatorio`):** utiliza `fgets` com descarte de excesso no buffer de entrada (`limpar_restante_stdin`), impedindo *buffer overflow* e rejeitando campos compostos apenas por espaços em branco.
* **Validação numérica (`ler_inteiro`):** lê a entrada como texto e converte via `strtol`, verificando `errno == ERANGE`, caracteres residuais não numéricos e limites mínimo/máximo. Isso evita loops infinitos comuns ao uso de `scanf`.
* **Proteção contra lista vazia e IDs duplicados:** todas as ações de consulta, pesquisa, atualização, exclusão, ordenação e análise verificam `lista_vazia(lista)` antes de prosseguir; o cadastro bloqueia identificadores repetidos.
* **Atualização não destrutiva:** na edição de um serviço, pressionar `ENTER` em campos de texto (ou digitar `0` na prioridade) preserva o valor atual sem corromper o nó.

### Indicadores Analíticos

A opção `8` calcula dinamicamente os seguintes indicadores sobre os dados em memória:
1. Total de serviços cadastrados;
2. Quantidade e percentual de serviços com atendimento 24 horas;
3. Média geral do nível de prioridade;
4. Quantidade e participação percentual de serviços por categoria;
5. Distribuição absoluta por nível de prioridade (de $5$ a $1$).

---

## 5. Algoritmos Implementados e Análise Big-O

Considere $n$ o número de serviços cadastrados na lista encadeada, $m$ o comprimento do termo pesquisado, $L$ o comprimento médio do campo textual consultado e $k$ o número de categorias distintas ($k \le n$).

### 5.1. Inserção (`inserir_servico`)
* **Funcionamento:** Para garantir a regra de identificador único, a função executa `buscar_por_id` antes de alocar o nó. Após validado, o encadeamento no final usa o ponteiro `lista->fim` diretamente.
* **Complexidade:** $O(n)$ no pior caso e caso médio (dominada pela verificação de duplicidade); o encadeamento isolado do nó no final é $O(1)$.

### 5.2. Busca por Identificador (`buscar_por_id`)
* **Funcionamento:** Implementa **Busca Linear** sequencial a partir de `lista->inicio`, comparando `atual->dados.id == id` até localizar o nó ou atingir `NULL`. Como listas encadeadas não possuem acesso aleatório por índice em $O(1)$, a busca binária não é aplicável diretamente sobre a estrutura encadeada.
* **Complexidade:** Melhor caso $O(1)$ (primeiro nó); caso médio e pior caso $O(n)$ (último nó ou registro inexistente).

### 5.3. Pesquisa por Atributo (`acao_pesquisar` e `contem_substr_ci`)
* **Funcionamento:** Percorre toda a lista encadeada e aplica busca de substring sem diferenciação de maiúsculas/minúsculas (`tolower`) sobre o atributo selecionado (Título, Categoria ou Local), exibindo todos os registros correspondentes e tratando consultas sem resultado.
* **Complexidade:** $O(n \cdot L \cdot m)$ no pior caso teórico da comparação de substrings; como $L$ é limitado por constantes pequenas de tamanho fixo (`TAM_TITULO = 100`, `TAM_CATEGORIA = 50`, `TAM_LOCAL = 120`), comporta-se na prática como $O(n \cdot m)$ (linear em relação à quantidade de registros $n$).

### 5.4. Remoção (`remover_por_id`)
* **Funcionamento:** Realiza busca linear mantendo os ponteiros `anterior` e `atual`. Uma vez encontrado o nó, a religação dos ponteiros e a chamada a `free` executam em tempo constante.
* **Complexidade:** Melhor caso $O(1)$ (remoção do primeiro elemento); caso médio e pior caso $O(n)$.

### 5.5. Ordenação — Merge Sort em Lista Encadeada (`ordenar_lista` / `merge_sort_nos`)
* **Funcionamento:** Foi implementado o algoritmo **Merge Sort** operando diretamente sobre os ponteiros dos nós, sem uso de `qsort` e sem cópia dos registros:
  1. **Divisão (`dividir_lista`):** emprega a técnica de dois ponteiros (*lento* avança 1 nó; *rápido* avança 2 nós) para localizar o ponto médio da lista em $O(n)$ e parti-la em duas sublistas independentes.
  2. **Conquista (`merge_sort_nos`):** ordena recursivamente a metade esquerda e a metade direita.
  3. **Intercalação (`intercalar_listas`):** utiliza um nó `sentinela` na pilha (*stack*) para combinar as duas sublistas ordenadas apenas redirecionando ponteiros `prox`, com base em um ponteiro de função `Comparador` (permitindo ordenar por ID, Título, Categoria ou Prioridade).
  4. **Atualização de `fim`:** ao concluir a ordenação, percorre a lista uma única vez ($O(n)$) para reposicionar `lista->fim`.
* **Justificativa da Escolha:** O Merge Sort é o algoritmo clássico mais adequado para listas encadeadas porque dispensa acesso aleatório, é **estável** (preserva a ordem relativa de elementos iguais) e mantém garantia assintótica de $O(n \log n)$ mesmo no pior caso (ao contrário do Quick Sort, que pode degradar para $O(n^2)$).
* **Complexidade:** Tempo $O(n \log n)$ em melhor, médio e pior caso ($\log_2 n$ níveis de recursão com trabalho $O(n)$ por nível); espaço auxiliar $O(\log n)$ referente apenas à pilha de chamadas recursivas.

### 5.6. Análise e Indicadores (`acao_analise`)
* **Funcionamento:** Percorre todos os $n$ nós acumulando contadores de prioridade e atendimento 24h em $O(1)$ por nó, e agrupa categorias em um vetor dinâmico de $k$ categorias distintas.
* **Complexidade:** Tempo $O(n \cdot k)$ (no pior caso, quando todas as categorias são distintas, $k = n$, resultando em $O(n^2)$); espaço auxiliar $O(n)$ para o vetor temporário de contagem.

### 5.7. Tabela Resumo de Complexidade

| Operação | Função | Melhor Caso | Pior Caso | Espaço Auxiliar |
| :--- | :--- | :---: | :---: | :---: |
| Inserção (com validação de ID) | `inserir_servico` | $O(1)$* | $O(n)$ | $O(1)$ |
| Busca por ID | `buscar_por_id` | $O(1)$ | $O(n)$ | $O(1)$ |
| Pesquisa por atributo textual | `acao_pesquisar` | $O(n)$ | $O(n \cdot L \cdot m)$ | $O(1)$ |
| Remoção por ID | `remover_por_id` | $O(1)$ | $O(n)$ | $O(1)$ |
| Ordenação (Merge Sort) | `ordenar_lista` | $O(n \log n)$ | $O(n \log n)$ | $O(\log n)$ |
| Análise e indicadores | `acao_analise` | $O(n)$ | $O(n \cdot k)$ | $O(n)$ |

*\*Somente na inserção do primeiro elemento (lista vazia), quando a verificação de duplicidade não percorre nenhum nó. Com a lista já preenchida, cadastrar um ID novo exige percorrer os $n$ nós para confirmar que ele não existe, por isso o custo típico da inserção é $O(n)$.*

---

## 6. Evidências de Testes

### 6.1. Matriz de Casos de Teste

| ID | Cenário Testado | Entrada Fornecida | Resultado Esperado | Resultado Observado | Status |
| :---: | :--- | :--- | :--- | :--- | :---: |
| **CT-01** | Operação com lista vazia | Opção `2` (Listar) e Opção `6` (Excluir) ao iniciar o sistema | Exibir aviso de lista vazia sem falha de execução | Aviso exibido corretamente e retorno ao menu principal | Aprovado |
| **CT-02** | Entrada inválida no menu | Digitar `abc` e em seguida `99` no menu principal | Rejeitar entrada não numérica e valor fora da faixa $[0, 9]$ | Mensagem de erro exibida em ambos os casos sem loop infinito | Aprovado |
| **CT-03** | Cadastro normal | Opção `1` com ID `106`, Título `"Atendimento Odontologico"`, Categoria `"Odontologia"`, Prioridade `2` | Alocar nó dinamicamente e confirmar cadastro | Serviço cadastrado e exibido corretamente na listagem | Aprovado |
| **CT-04** | Identificador duplicado | Opção `1` informando ID `106` (já existente) | Bloquear inserção e alertar sobre ID duplicado | Mensagem `"Erro: ja existe um servico cadastrado com o ID 106"` exibida | Aprovado |
| **CT-05** | Consulta e pesquisa (com resultado) | Opção `9` (carregar exemplos); Opção `3` com ID `104`; Opção `4` por Título com `"vacin"` | Localizar serviço ID `104` e retornar registro contendo `"Vacinacao"` (case-insensitive) | Registros encontrados e exibidos com todos os campos | Aprovado |
| **CT-06** | Registro inexistente | Opção `3` com ID `999`; Opção `4` por Categoria com `"inexistente"` | Informar que nenhum registro foi encontrado | Mensagens de ausência de resultado exibidas adequadamente | Aprovado |
| **CT-07** | Atualização e exclusão | Opção `5` no ID `106` (manter título com ENTER, alterar prioridade para `4`); Opção `6` removendo ID `106` | Atualizar apenas o campo modificado; remover nó e manter lista íntegra | Campo atualizado, nó removido com `free` e listagem íntegra | Aprovado |
| **CT-08** | Ordenação por Prioridade (Merge Sort) | Opção `9` (exemplos); Opção `7` com critério `4` | Nós reordenados da prioridade 5 para a 1; empates resolvidos pelo ID | Ordem obtida: 104 (5), 105 (4), 101 (3), 102 (3), 103 (2) | Aprovado |
| **CT-09** | Ordenação por Título A–Z (Merge Sort) | Em seguida, Opção `7` com critério `2` | Nós reordenados em ordem alfabética do título, sem diferenciar maiúsculas/minúsculas | Ordem obtida: Acolhimento (105), Campanha (101), Coleta (103), Pronto-Socorro (104), Retirada (102) | Aprovado |
| **CT-10** | Análise e indicadores | Opção `8` com os 5 serviços de exemplo | Total de registros, atendimento 24h, média de prioridade, contagem por categoria e por prioridade | Total 5; 1 serviço 24h (20,0%); média 3,40 / 5,00; 1 serviço em cada categoria | Aprovado |

### 6.2. Registros Visuais (Prints do Terminal)

> Inserção de prints

* **Figura 1 — CT-01 (Operações com lista vazia):**  
  `[Print das opções 2 e 6 com as listas vazias]`

* **Figura 2 — CT-02 (Validação de entradas inválidas):**  
  `[Print de entradas rejeitadas pelo menu tipo: "ABC" e "99"]`

* **Figura 3 — CT-03 e CT-04 (Cadastro válido e bloqueio de ID duplicado):**  
  `[Print de erro ao tentar repetir um ID]`

* **Figura 4 — CT-05 (Consulta por ID e pesquisa por substring):**  
  `[Print de consulta ID 104 e pesquisa pelo termo "vacin"]`

* **Figura 5 — CT-06 (Tratamento de registros inexistentes):**  
  `[Print de consulta ID 999 e pesquisa pelo termo "inexistente"]`

* **Figura 6 — CT-07 (Atualização parcial e exclusão preservando integridade):**  
  `[Print de atualização e remoção do ID 106]`

* **Figura 7 — CT-08 e CT-09 (Ordenação via Merge Sort — Prioridade e Título A–Z):**  
  `[Inserir print: Resultado da Opção 7 exibindo a lista ordenada]`

* **Figura 8 — CT-10 (Relatório de análise e indicadores de saúde):**  
  `[Inserir print: Saída completa da Opção 8]`