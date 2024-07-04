#include <stdio.h>
#include <stdlib.h>
#include <iostream>

typedef enum {RED, BLACK} Color;

typedef struct Node {
    int data;
    Color color;
    struct Node* left;
    struct Node* right;
    struct Node* parent;
} Node;

Node* createNode(int iData);
void rotateLeft(Node** root, Node* nodeA);
void rotateRight(Node** root, Node* nodeA);
Node* insertNode(Node* root, Node* newInsert);
void fixInsertion(Node** root, Node* currentNode);
void insert(Node** root, int iData);
void inorder(Node* root);
Node* searchValue(Node* root, int iValue);
Node* minValue(Node* root);
Node* maxValue(Node* root);
int height(Node* root);
bool validTreeTesting(Node* currentNode, int* blackTally, int blackCount);
bool validTreeTest(Node* root);
void fixDeletion(Node** root, Node* x);
Node* deleteNode(Node* root, int iData);

Node* createNode(int iData) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = iData;
    newNode->color = RED;
    newNode->left = nullptr;
    newNode->right = nullptr;
    newNode->parent = nullptr;
    return newNode;
}

void rotateLeft(Node** root, Node* nodeA) {
    // Faz a "rotação" para esquerda (descrisção no documento).

    // Node B é o filho direito de A
    Node* nodeB = nodeA->right;

    // Node A adota os esquerdos de B
    nodeA->right = nodeB->left;

    // Redefinimos o parent para A
    if (nodeA->right != nullptr)
        nodeA->right->parent = nodeA;

    // Node B recebe parent de A
    nodeB->parent = nodeA->parent;

    // Testa para caso A é raiz original, assim B viraria raiz; B é inserido como filho correto de parent de A;
    if (nodeA->parent == nullptr)
        *root = nodeB;
    else if (nodeA == nodeA->parent->left)
        nodeA->parent->left = nodeB;
    else
        nodeA->parent->right = nodeB;

    // A vira filho de B.
    nodeB->left = nodeA;
    nodeA->parent = nodeB;
}

void rotateRight(Node** root, Node* nodeA) {
    // Faz a "rotação" para direira (mesma coisa mas refletido).

    Node* nodeB = nodeA->left;

    nodeA->left = nodeB->right;

    if (nodeA->left != nullptr)
        nodeA->left->parent = nodeA;

    nodeB->parent = nodeA->parent;

    if (nodeA->parent == nullptr)
        *root = nodeB;
    else if (nodeA == nodeA->parent->left)
        nodeA->parent->left = nodeB;
    else
        nodeA->parent->right = nodeB;

    nodeB->right = nodeA;
    nodeA->parent = nodeB;
}

Node* insertNode(Node* root, Node* newInsert) {
    // Confere para ver se newInsert é a raiz e, caso não, recursivamente 
    // desce a arvore para colocar newInsert no local correto em termos de uma árvore binomial simples.
    // Após todas as iterações, retorna a raiz fundamental da árvore.

    if (root == nullptr)
        return newInsert;

    if (newInsert->data < root->data) {
        root->left = insertNode(root->left, newInsert);
        root->left->parent = root;
    }
    else if (newInsert->data > root->data) {
        root->right = insertNode(root->right, newInsert);
        root->right->parent = root;
    }

    return root;
}

void fixInsertion(Node** root, Node* currentNode) {

    // Nomeamos diretamente esses nodes para facilitar em descrever as mudanças de cor e rotações que ocorrerão.

    Node* tmpParent = nullptr;
    Node* tmpGrandparent = nullptr;

    // New não é root, e seu pai é RED. Como nodes começam RED, isso causa um erro. 
    // Usamos while para iterativaemnte subir a árvore até a raiz original, e assim ir corrigindo as mudanças.
    while ((currentNode != *root) && (currentNode->color != BLACK) && (currentNode->parent->color == RED)) {

        tmpParent = currentNode->parent;
        tmpGrandparent = currentNode->parent->parent;

        // Esquerda: Parent é esquerda de Grandparent.
        if (tmpParent == tmpGrandparent->left) {
            Node* tmpUncle = tmpGrandparent->right;

            // Caso 1 (Uncle é RED, invertemos cor da camada. Chamamos Grandparent de currentNode novo para subir a árvore.)
            if (tmpUncle != nullptr && tmpUncle->color == RED) {
                tmpGrandparent->color = RED;
                tmpParent->color = BLACK;
                tmpUncle->color = BLACK;
                currentNode = tmpGrandparent;
            }
            else {

                // Caso 2: (Caso triangular, newNode é direita de Parent, Uncle é BLACK. Temos que fazer uma rotação, e currentNode vira Parent.)
                if (currentNode == tmpParent->right) {
                    rotateLeft(root, tmpParent);
                    currentNode = tmpParent;
                    tmpParent = currentNode->parent;
                }

                // Caso 3: (Caso linear, ocorre para recolorir e finalizar caso triangular também.)
                rotateRight(root, tmpGrandparent);
                Color tmpColor = tmpParent->color;
                tmpParent->color = tmpGrandparent->color;
                tmpGrandparent->color = tmpColor;
                currentNode = tmpParent;
            }
        }

        // Direita: Parent é direita de Grandparent.
        // Repetimos as mesmas etapas para cada caso, exceto invertendo as direções de rotação.
        else {
            Node* tmpUncle = tmpGrandparent->left;

            // Caso 1:
            if (tmpUncle != nullptr && tmpUncle->color == RED) {
                tmpGrandparent->color = RED;
                tmpParent->color = BLACK;
                tmpUncle->color = BLACK;
                currentNode = tmpGrandparent;
            }
            else {
                // Caso 2:

                if (currentNode == tmpParent->left) {
                    rotateRight(root, tmpParent);
                    currentNode = tmpParent;
                    tmpParent = currentNode->parent;
                }

                // Caso 3:
                rotateLeft(root, tmpGrandparent);
                Color tmpColor = tmpParent->color;
                tmpParent->color = tmpGrandparent->color;
                tmpGrandparent->color = tmpColor;
                currentNode = tmpParent;
            }
        }
    }

    // Após todas as iterações, garantimos que o root original será BLACK
    (*root)->color = BLACK;
}

void insert(Node** root, int iData) {
    Node* newNode = createNode(iData);

    *root = insertNode(*root, newNode);

    fixInsertion(root, newNode);
}

void inorder(Node* root) {
    // Visto que inorder somente percorre e printf os valores da árvore em orderm crescente, não retorna nem afeta nada.

    if (root == nullptr)
        return;

    // Primeiro descemos ao maximo a esquerda, e depois subimos com aquilo acumulado no stack, 
    // cada vez preenchendo mais as partes direitas da arvore.
    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}

Node* searchValue(Node* root, int iValue) {
    // Procuramos recursivamente pelo valor e retornamos o node* desse valor.
    if (root == nullptr || root->data == iValue)
        return root;

    if (iValue < root->data)
        return searchValue(root->left, iValue);
    else
        return searchValue(root->right, iValue);
}

Node* minValue(Node* root) {
    // Min será a node mais na esquerda.
    Node* tmpNode = root;

    while (tmpNode->left != nullptr)
        tmpNode = tmpNode->left;

    return tmpNode;
}

Node* maxValue(Node* root) {
    // Max será a node mais na direita.
    Node* tmpNode = root;

    while (tmpNode->right != nullptr)
        tmpNode = tmpNode->right;

    return tmpNode;
}

int height(Node* root) {
    if (root == nullptr)
        return 0;

    int leftHeight = height(root->left);
    int rightHeight = height(root->right);
 
    // A arvore inteira é percorrida recursivamente e somente série de caminhos que resulta na maior altura é retornada.
    // Adicionamos +1 para contar o root.
    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

bool validTreeTesting(Node* currentNode, int* blackTally, int blackCount) {
    // Contagem de nodes pretas (balanceadas)
    if (currentNode == nullptr) {
        if (*blackTally == -1)
            *blackTally = blackCount;
        return *blackTally == blackCount;
    }

    // Acrescimo a contagem de nodes pretas (incluindo folhas)
    if (currentNode->color == BLACK)
        blackCount++;

    // Teste sobre propiedade de RED não pode ter filho RED
    if (currentNode->color == RED && ((currentNode->left != NULL && currentNode->left->color == RED) || (currentNode->right != NULL && currentNode->right->color == RED)))
        return false;

    // Recursivamente retornamos o resultado pelos caminhos a esquerda e direita, assim percorrendo toda a árvore.
    return validTreeTesting(currentNode->left, blackTally, blackCount) && validTreeTesting(currentNode->right, blackTally, blackCount);
}

bool validTreeTest(Node* root) {
    if (root == nullptr)
        return true;

    if (root->color != BLACK)
        return false;

    // Armazenamos a contagem de nodes pretos nas camadas para ver se está balanceada. -1 por conta da root. 
    int blackTally = -1;
    return validTreeTesting(root, &blackTally, 0);
}


void fixDeletion(Node** root, Node* currentNode) {
    if (currentNode == nullptr) return;

    // Fazemos correções recursivamente até currentNode ser a raiz original.
    while (currentNode != *root && currentNode->color == BLACK) {

        // Caso Esquerda:
        if (currentNode == currentNode->parent->left) {
            Node* tmpSibling = currentNode->parent->right;

            // Caso 1: Sibling é Vermelho;
            if (tmpSibling->color == RED) {
                tmpSibling->color = BLACK;
                currentNode->parent->color = RED;
                rotateLeft(root, currentNode->parent);
                tmpSibling = currentNode->parent->right;
            }

            // Caso 2: Sibling é Preto e filhos sao Pretos;
            if ((tmpSibling->left == nullptr || tmpSibling->left->color == BLACK) && (tmpSibling->right == nullptr || tmpSibling->right->color == BLACK)) {
                tmpSibling->color = RED;
                currentNode = currentNode->parent;
            } 

            // Caso 3: Sibling é Preto, filho da direita é Preto;
            else {
                if (tmpSibling->right == nullptr || tmpSibling->right->color == BLACK) {
                    if (tmpSibling->left != nullptr) 
                        tmpSibling->left->color = BLACK;

                    tmpSibling->color = RED;
                    rotateRight(root, tmpSibling);
                    tmpSibling = currentNode->parent->right;
                }

                //Caso 4: Sibling é Preto, filho da direita é Vermelho
                tmpSibling->color = currentNode->parent->color;
                currentNode->parent->color = BLACK;

                if (tmpSibling->right != nullptr)
                    tmpSibling->right->color = BLACK;

                rotateLeft(root, currentNode->parent);
                currentNode = *root;
            }
        } 

        // Caso Direita (mesma coisa mas com ponteiros e rotaçoes espelhados):
        else {
            Node* tmpSibling = currentNode->parent->left;

            if (tmpSibling->color == RED) {
                tmpSibling->color = BLACK;
                currentNode->parent->color = RED;
                rotateRight(root, currentNode->parent);
                tmpSibling = currentNode->parent->left;
            }

            if ((tmpSibling->left == nullptr || tmpSibling->left->color == BLACK) && (tmpSibling->right == nullptr || tmpSibling->right->color == BLACK)) {
                tmpSibling->color = RED;
                currentNode = currentNode->parent;
            } 
            
            else {
                if (tmpSibling->left == nullptr || tmpSibling->left->color == BLACK) {
                    if (tmpSibling->right != nullptr) tmpSibling->right->color = BLACK;
                    tmpSibling->color = RED;
                    rotateLeft(root, tmpSibling);
                    tmpSibling = currentNode->parent->left;
                }
                tmpSibling->color = currentNode->parent->color;
                currentNode->parent->color = BLACK;

                if (tmpSibling->left != nullptr) 
                    tmpSibling->left->color = BLACK;

                rotateRight(root, currentNode->parent);
                currentNode = *root;
            }
        }
    }
    // Após tudo, chegamos na raiz original e definimos como preta.
    currentNode->color = BLACK;
}


Node* deleteNode(Node* root, int iData) {
    // 1) Achar a node com valor iData ou retornar se não existe.
    Node* tmpNode = searchValue(root, iData);
    if (tmpNode == nullptr) return root;

    Node* toDelete = tmpNode;
    Node* toReplace; 
    Color originalColor = toDelete->color;

    // 2) Caso tmpNode não tem filho à esquerda:
    if (tmpNode->left == nullptr) {
        toReplace = tmpNode->right;
        if (tmpNode->parent == nullptr) {
            root = tmpNode->right;
        } else if (tmpNode == tmpNode->parent->left) {
            tmpNode->parent->left = tmpNode->right;
        } else {
            tmpNode->parent->right = tmpNode->right;
        }
        if (tmpNode->right != nullptr) {
            tmpNode->right->parent = tmpNode->parent;
        }
        free(tmpNode);
    }

    // 3) Caso tmpNode não tem filho à direita (mesma coisa mas muda direçao de pointers):
    else if (tmpNode->right == nullptr) {
        toReplace = tmpNode->left;
        if (tmpNode->parent == nullptr) {
            root = tmpNode->left;
        } else if (tmpNode == tmpNode->parent->left) {
            tmpNode->parent->left = tmpNode->left;
        } else {
            tmpNode->parent->right = tmpNode->left;
        }
        if (tmpNode->left != nullptr) {
            tmpNode->left->parent = tmpNode->parent;
        }
        free(tmpNode);
    }

    // 4) Caso tmpNode tem ambos filhos:
    else {
        // Achamos o toReplace;
        toDelete = minValue(tmpNode->right);
        originalColor = toDelete->color;
        toReplace = toDelete->right;

        // O Parent do toReplace será o apagado;
        if (toDelete->parent == tmpNode) {
            if (toReplace != nullptr) toReplace->parent = toDelete;
        }
        // Movemos o sucessor e filhos;
        else {
            if (toDelete->parent != nullptr) toDelete->parent->left = toReplace;
            if (toReplace != nullptr) toReplace->parent = toDelete->parent;
            toDelete->right = tmpNode->right;
            if (tmpNode->right != nullptr) tmpNode->right->parent = toDelete;
        }

        // Fazemos a substituição final e o sucessor adota a cor original.
        if (tmpNode->parent == nullptr) {
            root = toDelete;
        } else if (tmpNode == tmpNode->parent->left) {
            tmpNode->parent->left = toDelete;
        } else {
            tmpNode->parent->right = toDelete;
        }
        toDelete->parent = tmpNode->parent;
        toDelete->color = tmpNode->color;
        toDelete->left = tmpNode->left;
        if (tmpNode->left != nullptr) tmpNode->left->parent = toDelete;
        free(tmpNode);
    }

    // 5. Corrige a árvore.
    if (originalColor == BLACK) {
        fixDeletion(&root, toReplace);
    }

    return root;
}

int main() {

    // Testes:

    printf("=====================================\n");
    printf("Teste com Root1\n\n");

    Node* root1 = nullptr;

    insert(&root1, 7);
    insert(&root1, 3);
    insert(&root1, 18);
    insert(&root1, 10);
    insert(&root1, 22);
    insert(&root1, 8);
    insert(&root1, 11);
    insert(&root1, 26);
    insert(&root1, 118);
    insert(&root1, 2);
    insert(&root1, 1);
    insert(&root1, 23456);
    insert(&root1, 5);

    printf("Percuso Inorder root1:\n");
    inorder(root1);

    printf("\n\nTeste de validade da árvore: %s\n", validTreeTest(root1) ? "Válida" : "Não Válida");

    printf("\n\nBusca por valor 10:");

    Node* searchResult1 = searchValue(root1, 10);
    if (searchResult1) {
        printf("\nEncontrado valor 10, cor: %s\n", searchResult1->color == RED ? "RED" : "BLACK");
    } else {
        printf("\nNão encontrado valor 10.\n");
    }

    printf("\nApagando valor 10:\n");
    root1 = deleteNode(root1, 10);
    inorder(root1);

    printf("\n\nTeste de validade da árvore: %s\n", validTreeTest(root1) ? "Válida" : "Não Válida");

    printf("\n\nBusca por valor 10:");
    Node* searchResult2 = searchValue(root1, 10);
    if (searchResult2) {
        printf("\nEncontrado valor 10, cor: %s\n", searchResult2->color == RED ? "RED" : "BLACK");
    } else {
        printf("\nNão encontrado valor 10.\n");
    }

    printf("\nValor Maximo: %d\n", maxValue(root1)->data);
    printf("Valor Minimo: %d\n", minValue(root1)->data);

    printf("\nAltura: %d\n", height(root1));
    printf("\nTeste de validade da árvore: %s\n", validTreeTest(root1) ? "Válida" : "Não válida");

    //------------------------

    printf("=====================================\n");
    printf("Teste com Root2\n\n");
    Node* root2 = nullptr;

    insert(&root2, 1);
    insert(&root2, 2);
    insert(&root2, 3);
    insert(&root2, 4);
    insert(&root2, 400);
    insert(&root2, 500);
    insert(&root2, 5);
    insert(&root2, 399);
    insert(&root2, 501);
    insert(&root2, 401);
    insert(&root2, 499);
    insert(&root2, 402);
    insert(&root2, 6);

    printf("Percuso Inorder root1:\n");
    inorder(root2);

    printf("\n\nTeste de validade da árvore: %s\n", validTreeTest(root2) ? "Válida" : "Não Válida");

    printf("\n\nBusca por valor 6:");

    Node* searchResult3 = searchValue(root2, 6);
    if (searchResult3) {
        printf("\nEncontrado valor 6, cor: %s\n", searchResult3->color == RED ? "RED" : "BLACK");
    } else {
        printf("\nNão encontrado valor 6.\n");
    }

    printf("\nÁrvore apagando valor 6:\n");
    root2 = deleteNode(root2, 6);
    inorder(root2);

    printf("\n\nTeste de validade da árvore: %s\n", validTreeTest(root2) ? "Válida" : "Não Válida");

    printf("\n\nBusca por valor 400:");

    Node* searchResult4 = searchValue(root2, 400);
    if (searchResult4) {
        printf("\nEncontrado valor 400, cor: %s\n", searchResult4->color == RED ? "RED" : "BLACK");
    } else {
        printf("\nNão encontrado valor 400.\n");
    }

    printf("\nÁrvore apagando valor 6:\n");
    root2 = deleteNode(root2, 400);
    inorder(root2);

    printf("\nValor Maximo: %d\n", maxValue(root2)->data);
    printf("Valor Minimo: %d\n", minValue(root2)->data);

    printf("\nAltura: %d\n", height(root2));
    printf("\nTeste de validade da árvore: %s\n", validTreeTest(root2) ? "Válida" : "Não Válida");

    //--------------

    return 0;
}
