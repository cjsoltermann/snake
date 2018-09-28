#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>

#define WIDTH 100
#define HEIGHT 100
#define MAXAPPLES 1
#define RANDRANGE(MIN, MAX) (rand() % (MAX) - (MIN)) + (MIN)

enum direction {
  UP,
  DOWN,
  LEFT,
  RIGHT
};

/* Linked list */
struct snakePart {
  char symbol;
  int x;
  int y;
  struct snakePart *next;
  struct snakePart *last;
};

struct apple {
  char symbol;
  int x;
  int y;
};

struct snakePart *createSnake(int x, int y, char symbol) {
  struct snakePart *new = malloc(sizeof(struct snakePart));
  new->symbol = symbol;
  new->x = x;
  new->y = y;
  new->next = NULL;
  new->last = NULL;
  return new;
}

struct apple *createApple(int x, int y, char symbol) {
  struct apple *new = malloc(sizeof(struct apple));
  new->symbol = symbol;
  new->x = x;
  new->y = y;
  return new;
}

int addApple(struct apple **apples, int x, int y) {
  int i;
  for(i = 0; i < MAXAPPLES; i++) {
    if (apples[i] == NULL) {
      apples[i] = createApple(x, y, 'B');
    }
  }
}

int removeApple(struct apple **apples, int x, int y) {
  int i;
  for(i = 0; i < MAXAPPLES; i++) {
    if(apples[i] && apples[i]->x == x && apples[i]->y == y) {
      free(apples[i]);
      apples[i] = NULL;
      return 1;
    }
  }
  return 0;
}

int newTail(struct snakePart *head) {
  struct snakePart *new;
  struct snakePart *i = head;
  while (i->next) {
    i = i->next;
  }
  new = createSnake(i->x, i->y, 'A');
  i->next = new;
  new->last = i;
}

int newSegment(struct snakePart *head) {
  struct snakePart *p = head;
  int i;
  while (p->next) 
    p = p->next;
  for(i = 0; i < 5; i++) {
    newTail(p);
    p = p->next;
  }
}

int drawSnake(struct snakePart *head) {
  struct snakePart *pos = head;
  mvaddch(head->y, head->x, head->symbol);
  while(pos->next) {
    mvaddch(pos->next->y, pos->next->x, pos->next->symbol);
    pos = pos->next;
  }
}

int drawApples(struct apple **apples) {
  int i;
  for(i = 0; i < MAXAPPLES; i++) {
    if(apples[i])
      mvaddch(apples[i]->y, apples[i]->x, apples[i]->symbol);
  }
}

int moveSnake(struct snakePart *head, int direction) {
  struct snakePart *pos = head;

  while(pos->next) {
    pos = pos->next;
  }
  while(pos->last) {
    pos->x = pos->last->x;
    pos->y = pos->last->y;
    pos = pos->last;
  }

  switch (direction) {
    case UP:
      head->y--;
      break;
    case DOWN:
      head->y++;
      break;
    case LEFT:
      head->x--;
      break;
    case RIGHT:
      head->x++;
      break;
  }

}

void setup() {
  setlocale(LC_ALL, "en_US.UTF-8");
  if(!initscr()) exit(1);
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  timeout(1);
  srand(time(NULL));
}

int hitApple(struct snakePart *head, struct apple **apples) {
  int i;
  for(i = 0; i < MAXAPPLES; i++) {
    if (apples[i] && head->x == apples[i]->x && head->y == apples[i]->y)
      return 1;
  }
  return 0;
}

int hitSelf(struct snakePart *head) {
  struct snakePart *p = head;
  if(!p->next) return 0;
  p = p->next;
  while(p->next) {
    if (head->x == p->x && head->y == p->y)
      return 1;
    p = p->next;
  }
}

int main(int argc, char **argv) {
  setup();
  struct snakePart *head = createSnake(50, 50, '@');
  struct apple **apples = malloc(sizeof(struct apple*) * MAXAPPLES);
  int k;
  char direction = RIGHT;
  addApple(apples, RANDRANGE(0, 50), RANDRANGE(0, 50));
  while(k != 'q') {
    k = getch();
    switch(k) {
      case 'w':
        direction = UP;
        break;
      case 's':
        direction = DOWN;
        break;
      case 'a':
        direction = LEFT;
        break;
      case 'd':
        direction = RIGHT;
        break;
    }
    if(hitSelf(head))
      break;
    if(hitApple(head, apples)) {
      newSegment(head);
      removeApple(apples, head->x, head->y);
      addApple(apples, RANDRANGE(0, 50), RANDRANGE(0, 50));
    }
    erase();
    drawSnake(head);
    drawApples(apples);
    moveSnake(head, direction);
    usleep(100000);
  }
  endwin();
}
