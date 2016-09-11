#include "ast.h"

void ParseNode_Free(ParseNode *pn) {
  if (!pn)
    return;
  switch (pn->t) {
  case N_PRED:
    // TODO: free the value
    break;
  case N_COND:
    ParseNode_Free(pn->cn.left);
    ParseNode_Free(pn->cn.right);
  }

  free(pn);
}

ParseNode *NewConditionNode(ParseNode *left, int op, ParseNode *right) {
  ParseNode *n = malloc(sizeof(ParseNode));
  n->t = N_COND;
  n->cn.left = left;
  n->cn.right = right;
  n->cn.op = op;

  return n;
}

ParseNode *NewPredicateNode(int propId, int op, SIValue v) {

  ParseNode *n = malloc(sizeof(ParseNode));
  n->t = N_PRED;
  n->pn.propId = propId;
  n->pn.op = op;
  n->pn.val = v;

  return n;
}

#define pad(n)                                                                 \
  {                                                                            \
    for (int i = 0; i < n; i++)                                                \
      printf("  ");                                                            \
  }

void printOp(int op) {
  switch (op) {
  case EQ:
    printf("=");
    break;
  case GT:
    printf(">");
    break;
  case LT:
    printf("<");
    break;
  case GE:
    printf(">=");
    break;
  case LE:
    printf("<=");
    break;
  case NE:
    printf("!=");
    break;
  }
}

void conditionNode_print(ConditionNode *n, int depth) {

  if (n->left) {
    printf("\n");
    ParseNode_print(n->left, depth + 1);
    // printf("\n");
    pad(depth);
  }

  if (n->op) {
    printf(n->op == AND ? "  AND " : "  OR");
  }

  if (n->right) {
    printf("\n");
    ParseNode_print(n->right, depth + 1);
    pad(depth);
  }
}

void predicateNode_print(PredicateNode *n, int depth) {
  char buf[1024];
  SIValue_ToString(n->val, buf, 1024);
  printf("$%d ", n->propId);
  printOp(n->op);
  printf(" %s", buf);
}

void ParseNode_print(ParseNode *n, int depth) {
  pad(depth);
  printf("(");
  switch (n->t) {
  case N_COND:
    conditionNode_print(&(n->cn), depth + 1);
    break;
  case N_PRED:
    predicateNode_print(&(n->pn), depth + 1);
    break;
  }
  printf(")\n");
}