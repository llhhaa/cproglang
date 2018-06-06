/*
 * regen
 *
 * generate strings using regex-like
 * patterns.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 5000
#define MAX_RULES 100
#define MAXBUF 100
#define MAX_DIGIT 9

#define RULE_INIT {0, NULL, 0}

enum ruletypes {
  INVALID = 0,
  LITERAL,
  DIGIT,
  RANGE
};

struct rule {
  int type;
  char *value;
  int valuelen;
};

/** global variables **/
struct rule rules[MAX_RULES];
int ruleslen = 0;

/** function declarations **/
void parseRuleString(struct rule*, char**);
char *concatRuleValues(struct rule*);

struct rule *createLiteral(char[]);
struct rule *createDigitRule();
struct rule *createRangeRule(char[]);

int randomNumberInclusive(int, int);
char *strdupl(char *);

int main(int argc, char **argv) {

  int opt;

  // option flags
  while (--argc > 0 && *(*++argv) == '-') {
    while ((opt = *++(*argv))) {
      switch (opt) {
        case 'x':
          printf("sample opt1 found\n");
          break;
      }
    }
  }

  // initialize ruleset
  struct rule rules[MAX_RULES];

  if (argc != 1)
    printf("usage: \n");

  srand(time(NULL));

  parseRuleString(&rules[0], argv);

  printf("%s\n", concatRuleValues(rules));
}

char *concatRuleValues(struct rule *rulep) {
  char *temp = malloc(sizeof(char));
  while (--ruleslen != 0 && rulep->type > 0) {
    temp = realloc(temp, sizeof(temp) + sizeof(rulep->value));
    strcat(temp, rulep->value);
    rulep++;
  }
  return temp;
}

void parseRuleString(struct rule *rulep, char **s) {

  do {
    if (**s == '\\') {
      if (*++(*s) == 'd') {
        *rulep = *createDigitRule();
        rulep++;
        ruleslen++;
      }
    } if (**s == '[') {
      char buf[MAXBUF];
      int i;

      i = 0;

      while(*++(*s) != ']') {
        buf[i++] = **s;
      }

      if (strlen(buf) > 0) {
        *rulep = *createRangeRule(buf);
        rulep++;
        ruleslen++;
      }

    } else {
      char buf[] = {**s, '\0'};
      *rulep = *createLiteral(buf);
      rulep++;
      ruleslen++;
    }
  } while (*++(*s) != '\0');
}

struct rule *createLiteral(char token[]) {
  struct rule *temp = (struct rule *) malloc(sizeof(struct rule));
  temp->type = LITERAL;
  temp->value = (char *) strdupl(token);
  temp->valuelen = strlen(token);
  return temp;
}

struct rule *createDigitRule() {
  struct rule *temp = (struct rule *) malloc(sizeof(struct rule));

  char token[] = {randomNumberInclusive(0, MAX_DIGIT) + '0', '\0'};

  temp->type = DIGIT;
  temp->value = (char *) strdupl(token);
  temp->valuelen = strlen(token);
  return temp;
}

struct rule *createRangeRule(char buf[]) {

  struct rule *temp = (struct rule *) malloc(sizeof(struct rule));

  temp->type = RANGE;

  int possible[MAX]; // possible values
  unsigned int i, j, start, end;

  j = 0;

  for (i = 0; i < strlen(buf) - 1; i++) {
    if (buf[i] == ']') {
      break; // done creating range
    } else if (buf[i] == '-' && i != 0) {
      start = buf[i - 1];
      end = buf[++i];
    } else if (buf[i + 1] == '-') {
      continue; // ignore prior to `-` handle it in the above cond
    } else {
      possible[j++] = buf[i];
      continue; // literal charcter, add to possible and continue
    }

    possible[j++] = randomNumberInclusive(start, end);
  }

  char token[] = {possible[randomNumberInclusive(0, j - 1)], '\0'};

  temp->value = (char *) strdupl(token);
  temp->valuelen = strlen(token);

  return temp;
}

int randomNumberInclusive(int l, int u) {

  if (u < l) {
    printf("randomNumberInclusive: lower value greater than upper value");
    return -1;
  }

  int offset = u - l;

  int div = RAND_MAX/(offset + 1);
  int val;

  do {
    rand();
    val = rand() / div;
  } while (val > MAX_DIGIT);

  return val + l;
}

char *strdupl(char *src) {
  size_t len = strlen(src) + 1;
  char *s = malloc(len);
  if (s == NULL)
    return NULL;
  return (char *) memcpy(s, src, len);
}
