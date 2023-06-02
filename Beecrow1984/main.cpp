#include <iostream>

using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_INSTRUCTIONS 2000
#define MAX_VAR_NAME_SIZE 8
#define ASCII_ZERO 48
#define ASCII_NINE 57
#define ASCII_LOWERA 97
#define ASCII_LOWERZ 122
#define ASCII_UPPERA 65
#define ASCII_UPPERZ 90

typedef struct reg {
    char *name;
    int type;
} Register;

typedef struct reg_node {
    Register reg;
    void *next;
} RegisterNode;

typedef struct instruction {
    int op;
    char **tokens;
} Instruction;

typedef struct range {
    int start;
    int end;
} Range;

enum {
    declaration,
    sum
};

enum {
    literal,
    expression
};

RegisterNode *head = NULL;

int inRange(int nArgs, ...) {
    va_list valist;
    va_start(valist, nArgs);
    int result = 0, i, n;
    Range range;

    // Le os argumentos passados para a funcao
    for (i=0; i < nArgs; i++) {
        // O primeiro argumento sempre é o numero a ser testado
        if (i == 0) {
            n = va_arg(valist, int);
        }
        else {
            // Le e testa cada um dos ranges recebidos
            range = va_arg(valist, Range);
            result = (n-range.start)*(n-range.end) <= 0 ? 1 : result;
        }
    }

    va_end(valist);

    return result;
}

// Insere o registrador na lista ligada
void insertFirst(Register data) {
    RegisterNode *link = (RegisterNode *) malloc(sizeof(RegisterNode ));

    link->reg = data;
    link->next = head;
    head = link;
}

// Verifica se o registrador passado já está na lista, usando o nome como chave
RegisterNode *find(char *key) {
    RegisterNode *current = head;

    if(head == NULL) {
        return NULL;
    }

    while(strcmp(current->reg.name, key) != 0) {
        if(current->next == NULL) {
            return NULL;
        } else {
            current = current->next;
        }
    }

    return current;
}

RegisterNode *delete(char key) {

    //start from the first link
    RegisterNode *current = head;
    RegisterNode *previous = NULL;

    //if list is empty
    if(head == NULL) {
        return NULL;
    }

    //navigate through list
    while(strcmp(current->reg.name, key) != 0) {

        //if it is last node
        if(current->next == NULL) {
            return NULL;
        } else {
            //store reference to current link
            previous = current;
            //move to next link
            current = current->next;
        }
    }

    //found a match, update the link
    if(current == head) {
        //change first to point to next link
        head = head->next;
    } else {
        //bypass the current link
        previous->next = current->next;
    }

    return current;
}

Instruction extractInstructionData(char *codeline) {
    int i = 0, step = 0;
    char *token;
    Instruction inst;
    inst.tokens = malloc(sizeof(char*));

    token = strtok(codeline, " ");

    // Le token a token da instrucao recebida, usando espaco como separador
    while(token != NULL) {
        switch (step++) {
            case 0:
                inst.tokens[0] = malloc(strlen(token) + 1);
                strcpy(inst.tokens[0], token);
                break;
            case 1:
                if (strcmp(token, ":=") != 0){
                    token = NULL;
                    continue;
                }
                break;
            case 2:
                inst.tokens[1] = calloc(strlen(token) + 1, sizeof(char));
                strcpy(inst.tokens[1], token);
                break;
            case 3:
                if (strcmp(token, "+") != 0){
                    token = NULL;
                    continue;
                }
                break;
            case 4:
                inst.tokens[2] = calloc(11, sizeof(char));
                strcpy(inst.tokens[2], token);
                break;
            default:
                break;
        }
        token = strtok(NULL, " ");
    }
    // Caso a leitura dos tokens tenha sido interrompida em := ou +, ha um erro na digitacao da instrucao
    if (step == 2 || step == 4)
        return (Instruction) {.tokens = NULL, .op = -1};

    // Decide qual a operacao de acordo com a existencia do segundo paramentro
    inst.op = (step == 5) ? sum : declaration;
    return inst;
}

int setRegister(Instruction inst) {
    Register reg;
    int i, valtypes[2] = {literal}, nameSize = strlen(inst.tokens[0]);

    // Verifica se o nome esta dentro do limite de caracteres
    if (nameSize > 8)
        return 0;

    reg.name = malloc(nameSize * sizeof(char));
    strcpy(reg.name, inst.tokens[0]);

    switch (inst.op) {
        case declaration:
            // Caso seja uma declaracao, simplesmente marca o registrador como literal
            reg.type = literal;
            break;
        case sum:
            reg.type = literal;
            for (i = 1; i <= 2; i++) {
                // Caso seja um soma, verifica se algum dos parametros eh uma variavel
                if (inRange(3, inst.tokens[i][0], (Range) {.start = ASCII_LOWERA, .end = ASCII_LOWERZ}, (Range) {.start = ASCII_UPPERA, .end = ASCII_UPPERZ})){
                    reg.type = expression;
                    valtypes[i-1] = expression;
                }
            }
            break;
    }

    // Verifica se o registrador de destino ja existe, retornado erro caso positivo
    if (find(reg.name)) {
        return 0;
    }

    insertFirst(reg);

    RegisterNode * var;
    int control = 0;

    // Certificacao de que os dois parametros sao ou podem ser reduzidos a literais
    for (i = 0; i < 2; i++) {
        if (valtypes[i] == expression) {
            var = find(inst.tokens[i+1]);
            control += var->reg.type == literal;
        }
        else
            control++;
    }
    return control == 2;
}

int main() {
    int i, j, nInstructions;
    char currentChar;
    Instruction instruction;

    char **instructionList = malloc(MAX_INSTRUCTIONS * sizeof(char*));
    for (i = 0; i < MAX_INSTRUCTIONS; i++)
        instructionList[i] = calloc((MAX_VAR_NAME_SIZE * 3 + 8), sizeof(char));

    i = 0;
    j = 0;

    // Faz a leitura das instrucoes
    while (1) {
        currentChar = getchar();
        if (currentChar == 10) {
            if (j == 0)
                break;
            else {
                j = 0;
                i++;
                continue;
            }
        }
        instructionList[i][j++] = currentChar;
    }

    nInstructions = i;

    for (i = 0; i < nInstructions; i++) {
        // Extrai as informacoes de cada instrucao
        instruction = extractInstructionData(instructionList[i]);
        for (j = 0; j < strlen(instruction.tokens[0]); j++) {
            // Verifica se todos os caracteres do nome do registrador sao validos
            if (!inRange(3, instruction.tokens[0][j], (Range) {.start = ASCII_LOWERA, .end = ASCII_LOWERZ}, (Range) {.start = ASCII_UPPERA, .end = ASCII_UPPERZ})) {
                printf("Compilation error\n");
                return 0;
            }
        }

        // Tenta armazenar as informacoes para a instrucao recebida
        if (!setRegister(instruction)) {
            printf("Compilation error\n");
            return 0;
        }
    }

    printf("OK\n");

    return 0;
}
